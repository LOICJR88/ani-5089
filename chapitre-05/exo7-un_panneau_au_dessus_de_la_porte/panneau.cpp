// Exercice 7 : un panneau au-dessus de la porte.
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "NKImage/Core/NkImage.h"
#include "NKFont/NkFont.h"
#include "NKFont/Embedded/NkFontEmbedded.h"
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKLogger/NkLog.h"
#include "NKMath/NkFunctions.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKRenderer/Materials/NkMaterial.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Panneau7";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;
    char ligne[160];

    NkFontAtlas atlas;
    NkFont *police = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, 32.f);
    if (!police) {
        logger.Error("[Panneau7] AddToAtlas KO");
        return 10;
    }
    if (!atlas.Build()) {
        logger.Error("[Panneau7] atlas.Build KO");
        return 11;
    }
    nkft_uint8 *alpha = nullptr;
    nkft_int32 texL = 0, texH = 0;
    atlas.GetTexDataAsAlpha8(&alpha, &texL, &texH);
    if (!alpha || texL <= 0 || texH <= 0) {
        logger.Error("[Panneau7] GetTexDataAsAlpha8 KO");
        return 12;
    }
    std::snprintf(ligne, sizeof(ligne), "atlas : %d x %d", (int)texL, (int)texH);
    std::printf("%s\n", ligne);
    logger.Info(ligne);

    const char *texte = "Sortie";
    const int nbLettres = 6;

    float largeurF = 0.f;
    for (int i = 0; i < nbLettres; ++i) {
        const NkFontGlyph *g = police->FindGlyph((NkFontCodepoint)texte[i]);
        if (!g) { logger.Error("[Panneau7] lettre absente de l'atlas"); return 13; }
        largeurF += g->advanceX;
    }
    const int L = (int)std::ceil(largeurF);
    const int H = (int)std::ceil(police->ascent - police->descent);
    const float base = police->ascent;          

    const uint8 fondR = 0x0B, fondG = 0x6B, fondB = 0x2E;   
    NkImage img = NkImage::Create((uint32)L, (uint32)H, 4, 0x0B6B2EFFu);
    if (!img.IsValid()) {
        logger.Error("[Panneau7] NkImage::Create KO");
        return 14;
    }

    float curseur = 0.f;
    for (int i = 0; i < nbLettres; ++i) {
        const NkFontGlyph *g = police->FindGlyph((NkFontCodepoint)texte[i]);
        const float gx0 = curseur + g->x0, gx1 = curseur + g->x1;
        const float gy0 = base + g->y0,    gy1 = base + g->y1;
        if (gx1 > gx0 && gy1 > gy0) {
            const int px0 = (int)std::floor(gx0), px1 = (int)std::ceil(gx1);
            const int py0 = (int)std::floor(gy0), py1 = (int)std::ceil(gy1);
            for (int py = py0; py < py1; ++py) {
                for (int px = px0; px < px1; ++px) {
                    if (px < 0 || px >= L || py < 0 || py >= H) { continue; }
                    const float fx = ((float)px + 0.5f - gx0) / (gx1 - gx0);
                    const float fy = ((float)py + 0.5f - gy0) / (gy1 - gy0);
                    if (fx < 0.f || fx > 1.f || fy < 0.f || fy > 1.f) { continue; }
                    const float u = g->u0 + fx * (g->u1 - g->u0);
                    const float v = g->v0 + fy * (g->v1 - g->v0);
                    int ax = (int)(u * (float)texL);
                    int ay = (int)(v * (float)texH);
                    if (ax < 0) ax = 0;
                    if (ax >= texL) ax = texL - 1;
                    if (ay < 0) ay = 0;
                    if (ay >= texH) ay = texH - 1;
                    const float couv = (float)alpha[ay * texL + ax] / 255.f;
                    const uint8 r = (uint8)(fondR + (255 - fondR) * couv);
                    const uint8 gg = (uint8)(fondG + (255 - fondG) * couv);
                    const uint8 b = (uint8)(fondB + (255 - fondB) * couv);
                    img.SetPixel(px, py, math::NkColor(r, gg, b, 255));
                }
            }
        }
        curseur += g->advanceX;
    }

    std::snprintf(ligne, sizeof(ligne), "image : %d x %d", img.Width(), img.Height());
    std::printf("%s\n", ligne);
    logger.Info(ligne);

    if (!img.SavePNG("panneau.png")) {
        logger.Error("[Panneau7] SavePNG panneau.png KO");
    }

    NkImage tex = img.Copy();
    tex.FlipHorizontal();
    tex.SavePNG("panneau_tex.png");

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Panneau7] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Panneau7] Creation device KO");
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Panneau7] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlan = renderer->GetMeshSystem()->GetPlane();

    auto texPanneau = renderer->GetTextures()->Load("panneau_tex.png");
    auto mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    mat->SetAlbedoMap(texPanneau);

    bool running = true;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) running = false;
    });

    const float32 demiSalle = 2.0f;
    const float32 epaisseur = 0.10f;
    const float32 hauteur = 2.5f;
    const float32 centreMur = demiSalle + epaisseur * 0.5f;
    const float32 longueurMur = 2.f * demiSalle + 2.f * epaisseur;
    const float32 panH = 0.20f;
    const float32 panL = panH * (float32)img.Width() / (float32)img.Height();
    const float32 panE = 0.01f;
    const float32 panZ = -2.0f + panE * 0.5f + 0.003f;   
    const NkVec3f panCentre = {-0.70f, 2.20f, panZ};

    NkVec3f camPos = {-0.70f, 1.70f, 1.00f};
    const char *choixCam = std::getenv("PANNEAU_CAM");
    if (choixCam && std::string(choixCam) == "50cm") {
        camPos = {-0.70f, 2.20f, -1.50f};
    }
    const NkVec3f camTarget = panCentre;

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;

        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.position = camPos;
        camData.target = camTarget;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)devInfo.width / (float32)devInfo.height;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;
        NkCamera3D cam(camData);

        NkSceneContext sctx;
        sctx.camera = cam;
        sctx.time = 0.f;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sun.castShadow = false;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        auto cube = [&](NkVec3f centre, NkVec3f taille, NkVec3f tint, bool avecTexture = false) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
            dc.aabb = {{centre.x - taille.x * 0.5f, centre.y - taille.y * 0.5f, centre.z - taille.z * 0.5f},
                       {centre.x + taille.x * 0.5f, centre.y + taille.y * 0.5f, centre.z + taille.z * 0.5f}};
            dc.castShadow = false;
            dc.tint = tint;
            if (avecTexture) { dc.material = mat->GetInstHandle(); }
            r3d->Submit(dc);
        };

        {
            NkDrawCall3D dc;
            dc.mesh = meshPlan;
            dc.transform = NkMat4f::Scale({4.2f, 1.f, 4.2f});
            dc.aabb = {{-4.2f, 0.f, -4.2f}, {4.2f, 0.f, 4.2f}};
            dc.castShadow = false;
            dc.tint = {0.30f, 0.28f, 0.26f};
            r3d->Submit(dc);
        }

        const NkVec3f mur = {0.72f, 0.70f, 0.66f};
        cube({0.f, hauteur * 0.5f, -centreMur}, {longueurMur, hauteur, epaisseur}, mur);
        cube({0.f, hauteur * 0.5f, centreMur}, {longueurMur, hauteur, epaisseur}, mur);
        cube({-centreMur, hauteur * 0.5f, 0.f}, {epaisseur, hauteur, longueurMur}, mur);
        cube({centreMur, hauteur * 0.5f, 0.f}, {epaisseur, hauteur, longueurMur}, mur);
        cube({0.f, hauteur + epaisseur * 0.5f, 0.f}, {longueurMur, epaisseur, longueurMur}, {0.85f, 0.85f, 0.82f});

        cube(panCentre, {panL, panH, panE}, {1.f, 1.f, 1.f}, true);

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}