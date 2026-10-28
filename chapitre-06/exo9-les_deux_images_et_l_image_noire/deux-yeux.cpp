#include <cstdio>
#include <cstdlib>

#include "NKWindow/Core/NkWindow.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKWindow/Core/NkWESystem.h"
#include "NKEvent/NkKeyboardEvent.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKImage/Core/NkImage.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

void SoumettreLaSalle(NkRender3D *r3d, NkMeshSystem *meshes);
void PointeurTouche(NkWindow &fenetre, NkKey touche);

namespace {

constexpr uint32 kLargeurOeil = 640;
constexpr uint32 kHauteurOeil = 720;
constexpr uint64 kImageDeCapture = 30;

bool Comparer(const char *fichierGauche, const char *fichierDroit,
              const char *fichierDifference, const char *etiquette) {
    NkImage gauche;
    NkImage droit;
    if (!gauche.Load(fichierGauche, 4) || !droit.Load(fichierDroit, 4)) {
        std::printf("[deux-yeux] impossible de recharger %s ou %s\n", fichierGauche, fichierDroit);
        return false;
    }
    if (gauche.Width() != droit.Width() || gauche.Height() != droit.Height()) {
        std::printf("[deux-yeux] les deux images n'ont pas la meme taille\n");
        return false;
    }

    const int32 largeur = gauche.Width();
    const int32 hauteur = gauche.Height();
    NkImage difference = NkImage::Create((uint32)largeur, (uint32)hauteur, 4);
    if (!difference.IsValid()) {
        std::printf("[deux-yeux] image de difference refusee\n");
        return false;
    }

    long long pixelsDifferents = 0;
    long long nonBlancsGauche = 0;
    long long nonBlancsDroit = 0;
    int ecartMax = 0;

    for (int32 y = 0; y < hauteur; ++y) {
        for (int32 x = 0; x < largeur; ++x) {
            const math::NkColor a = gauche.GetPixel(x, y);
            const math::NkColor b = droit.GetPixel(x, y);

            const int dr = std::abs((int)a.r - (int)b.r);
            const int dg = std::abs((int)a.g - (int)b.g);
            const int db = std::abs((int)a.b - (int)b.b);
            int ecart = dr;
            if (dg > ecart) ecart = dg;
            if (db > ecart) ecart = db;

            if (ecart > 1) ++pixelsDifferents;
            if (ecart > ecartMax) ecartMax = ecart;

            if (!(a.r == 255 && a.g == 255 && a.b == 255)) ++nonBlancsGauche;
            if (!(b.r == 255 && b.g == 255 && b.b == 255)) ++nonBlancsDroit;

            int valeur = ecart * 4;
            if (valeur > 255) valeur = 255;
            difference.SetPixel(x, y, math::NkColor((uint8)valeur, (uint8)valeur, (uint8)valeur, 255));
        }
    }

    const long long total = (long long)largeur * (long long)hauteur;
    const bool ecrite = difference.SavePNG(fichierDifference);

    std::printf("[verif] pixels non blancs : gauche %lld, droit %lld, sur %lld\n",
                nonBlancsGauche, nonBlancsDroit, total);
    std::printf("[deux-yeux] %s : %s\n", fichierDifference, ecrite ? "ecrit" : "ECHEC");

    std::printf("%s : %lld pixels sur %lld, ecart max %d\n", etiquette, pixelsDifferents, total, ecartMax);
    if (FILE *f = std::fopen("difference.txt", "a")) {
        std::fprintf(f, "%s : %lld pixels sur %lld, ecart max %d\n", etiquette, pixelsDifferents, total, ecartMax);
        std::fclose(f);
    }
    return ecrite;
}

}

int LancerLesDeuxYeux(NkWindow &fenetre, bool vueOubliee) {
    NkDeviceInitInfo devInit{};
    devInit.surface = fenetre.GetSurfaceDesc();
    devInit.width = fenetre.GetSize().x;
    devInit.height = fenetre.GetSize().y;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
    if (device == nullptr) {
        std::printf("[deux-yeux] aucune API graphique disponible\n");
        return 1;
    }

    NkRendererConfig cfgEcran = NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
    NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
    if (rEcran == nullptr || !rEcran->Initialize()) {
        std::printf("[deux-yeux] le renderer d'ecran n'a pas pu s'initialiser\n");
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    NkOffscreenTarget *cibles[2] = { nullptr, nullptr };
    NkRenderer *renderers[2] = { nullptr, nullptr };

    for (int oeil = 0; oeil < 2; ++oeil) {
        NkOffscreenDesc od;
        od.width = kLargeurOeil;
        od.height = kHauteurOeil;
        od.hasDepth = true;
        od.hdr = false;
        od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
        od.readable = true;
        od.readback = true;
        od.name = (oeil == 0) ? "Oeil_Gauche" : "Oeil_Droit";

        cibles[oeil] = rEcran->CreateOffscreen(od);
        if (cibles[oeil] == nullptr) {
            std::printf("[deux-yeux] cible hors ecran refusee\n");
            return 1;
        }

        NkRendererConfig cfgSalle = NkRendererConfig::ForGame(devInit.api, devInit.width, devInit.height);
        cfgSalle.Disable(NK_SS_RENDER2D);
        cfgSalle.Disable(NK_SS_TEXT);
        cfgSalle.Disable(NK_SS_OVERLAY);
        cfgSalle.Enable(NK_SS_OFFSCREEN);

        renderers[oeil] = NkRenderer::Create(device, cfgSalle);
        if (renderers[oeil] == nullptr || !renderers[oeil]->Initialize()) {
            std::printf("[deux-yeux] le renderer de l'oeil %d n'a pas pu s'initialiser\n", oeil);
            return 1;
        }
        renderers[oeil]->SetFinalColorTarget(
            rEcran->GetTextures()->GetRHIHandle(cibles[oeil]->GetColorResult()));
        renderers[oeil]->SetRenderSizeOverride(kLargeurOeil, kHauteurOeil);
    }

    const float demiEcart = 0.063f * 0.5f;
    const float teteX = 0.30f, teteY = 1.70f, teteZ = 1.20f;
    const float yeuxX[2] = {
        vueOubliee ? teteX : teteX - demiEcart,
        vueOubliee ? teteX : teteX + demiEcart,
    };

    uint64 image = 0;
    bool tourne = true;

    while (tourne && fenetre.IsOpen() && image <= kImageDeCapture) {
        NkEvent *ev = nullptr;
        while (NkEvents().PollEvent(ev)) {
            if (ev->As<NkWindowCloseEvent>() != nullptr) {
                tourne = false;
            }
            if (auto *touche = ev->As<NkKeyPressEvent>()) {
                PointeurTouche(fenetre, touche->GetKey());
            }
        }
        if (!tourne) break;

        if (rEcran->BeginFrame()) {
            NkICommandBuffer *cmd = rEcran->GetCmd();

            for (int oeil = 0; oeil < 2; ++oeil) {
                NkRenderer *renderer = renderers[oeil];
                NkRender3D *r3d = renderer->GetRender3D();

                renderer->FlushGraphRebuilds();
                r3d->ResetFrame();
                if (auto *mc = renderer->GetMaterialCollection()) {
                    mc->Upload();
                }

                NkCamera3DData cd;
                cd.position = { yeuxX[oeil], teteY, teteZ };
                cd.target = { yeuxX[oeil], teteY, teteZ - 1.f };
                cd.up = { 0.f, 1.f, 0.f };
                cd.fovY = 70.f;
                cd.aspect = (float32)kLargeurOeil / (float32)kHauteurOeil;
                cd.nearPlane = 0.05f;
                cd.farPlane = 100.f;

                NkSceneContext sctx;
                sctx.camera = NkCamera3D(cd);
                sctx.time = (float32)image / 60.f;
                r3d->SetIBLStrength(0.35f);

                NkLightDesc soleil;
                soleil.type = NkLightType::NK_DIRECTIONAL;
                soleil.direction = { -0.5f, -1.f, -0.35f };
                soleil.color = { 1.f, 0.96f, 0.88f };
                soleil.intensity = 3.f;
                soleil.castShadow = false;
                sctx.lights.PushBack(soleil);

                r3d->BeginScene(sctx);
                SoumettreLaSalle(r3d, renderer->GetMeshSystem());

                if (auto *graph = renderer->GetRenderGraph()) {
                    graph->Execute(cmd);
                }
            }

            if (auto *r2d = rEcran->GetRender2D()) {
                r2d->DrawImage(cibles[0]->GetColorResult(),
                               { 0.f, 0.f, (float32)kLargeurOeil, (float32)kHauteurOeil });
                r2d->DrawImage(cibles[1]->GetColorResult(),
                               { (float32)kLargeurOeil, 0.f, (float32)kLargeurOeil, (float32)kHauteurOeil });
            }

            rEcran->Present();
            rEcran->EndFrame();
        }

        ++image;

        if (image == kImageDeCapture) {
            const char *fichierGauche = vueOubliee ? "oeil-gauche-oubliee.png" : "oeil-gauche.png";
            const char *fichierDroit = vueOubliee ? "oeil-droit-oubliee.png" : "oeil-droit.png";
            const bool okG = cibles[0]->Capture(fichierGauche);
            const bool okD = cibles[1]->Capture(fichierDroit);
            std::printf("[deux-yeux] capture image %llu : %s (%s), %s (%s)\n",
                        (unsigned long long)image, fichierGauche, okG ? "ok" : "echec",
                        fichierDroit, okD ? "ok" : "echec");
        }
    }

    NkRenderer::Destroy(renderers[1]);
    NkRenderer::Destroy(renderers[0]);
    NkRenderer::Destroy(rEcran);
    NkDeviceFactory::Destroy(device);

    if (image <= kImageDeCapture) {
        std::printf("[deux-yeux] arret avant l'image %llu : pas de capture\n",
                    (unsigned long long)kImageDeCapture);
        return 1;
    }

    if (vueOubliee) {
        return Comparer("oeil-gauche-oubliee.png", "oeil-droit-oubliee.png",
                        "difference-vue-oubliee.png", "vue oubliee") ? 0 : 1;
    }
    return Comparer("oeil-gauche.png", "oeil-droit.png", "difference.png", "deux yeux") ? 0 : 1;
}