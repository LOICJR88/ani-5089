// Exercice 24 : deux fonctions qui font gagner du temps.
#include <cstdio>
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

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Poser24";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const float HAUTEUR_TABLE = 0.75f;  

// DEBUT SALLE (fonctions)
static NkVec3f CentreAuSol(NkVec3f taille, float x, float z) {
    return {x, taille.y * 0.5f, z};
}
static NkVec3f CentreSurTable(NkVec3f taille, float x, float z) {
    return {x, HAUTEUR_TABLE + taille.y * 0.5f, z};
}
NkMat4f PoserAuSol(NkVec3f taille, float x, float z) {
    return NkMat4f::Translate(CentreAuSol(taille, x, z)) * NkMat4f::Scale(taille);
}
NkMat4f PoserSurTable(NkVec3f taille, float x, float z) {
    return NkMat4f::Translate(CentreSurTable(taille, x, z)) * NkMat4f::Scale(taille);
}
// FIN SALLE (fonctions)

static bool Proche(float a, float b) {
    return (a - b) < 1e-5f && (b - a) < 1e-5f;
}

int nkmain(const NkEntryState &state) {
    (void)state;
    
    {
        const NkVec3f pave = {0.40f, 0.20f, 0.60f};
        if (!Proche(CentreAuSol(pave, 0.f, 0.f).y, 0.10f)) { std::printf("PoserAuSol FAUX\n"); return 4; }
        if (!Proche(CentreSurTable(pave, 0.f, 0.f).y, 0.85f)) { std::printf("PoserSurTable FAUX\n"); return 5; }
    }

    NkWindowConfig winCfg;
    winCfg.title = "Poser";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Poser24] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Poser24] Creation device KO");
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Poser24] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();

    bool running = true;
    uint32 W = devInfo.width;
    uint32 H = devInfo.height;
    NkEventSystem &events = NkEvents();

    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) running = false;
    });
    events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e) {
        const uint32 w = (uint32)e->GetWidth(), h = (uint32)e->GetHeight();
        if (w > 0 && h > 0 && (w != W || h != H)) {
            W = w;
            H = h;
            renderer->OnResize(w, h);
        }
    });

    
    const float32 demiSalle = 2.0f;
    const float32 epaisseurMur = 0.10f;
    const float32 hauteur = 2.5f;
    const float32 centreMur = demiSalle + epaisseurMur * 0.5f;
    const float32 longueurMur = 2.f * demiSalle + 2.f * epaisseurMur;

    
    const float32 epaisseurPanneau = 0.02f;
    const float32 decalage = 0.001f;
    const float32 zPorte = -demiSalle - epaisseurPanneau * 0.5f + decalage;
    const float32 xFenetre = -demiSalle - epaisseurPanneau * 0.5f + decalage;

    
    const float32 tableX = 0.60f, tableZ = 0.70f;
    const float32 longueur = 1.20f, profondeur = 0.80f;
    const float32 epaisseur = 0.05f, section = 0.06f;
    const float32 yPlateau = HAUTEUR_TABLE - epaisseur * 0.5f;
    const float32 hauteurPied = HAUTEUR_TABLE - epaisseur;
    const float32 decalX = longueur * 0.5f - section * 0.5f;
    const float32 decalZ = profondeur * 0.5f - section * 0.5f;
    const float32 signes[2] = {-1.f, 1.f};

    const NkVec3f couleurSol = {0.30f, 0.28f, 0.26f};
    const NkVec3f couleurMur = {0.72f, 0.70f, 0.66f};
    const NkVec3f couleurPorte = {0.35f, 0.20f, 0.10f};
    const NkVec3f couleurVitre = {0.50f, 0.70f, 0.90f};
    const NkVec3f couleurPlateau = {0.75f, 0.55f, 0.30f};
    const NkVec3f couleurPied = {0.45f, 0.30f, 0.15f};
    const NkVec3f couleurVase = {0.85f, 0.20f, 0.20f};

    const NkVec3f camPos = {1.4f, 1.60f, 1.8f};
    const NkVec3f camTarget = {-0.6f, 1.0f, -0.8f};

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;

        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.position = camPos;
        camData.target = camTarget;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)W / (float32)H;
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

        
        auto soumettre = [&](const NkMat4f &m, NkVec3f c, NkVec3f t, NkVec3f tint) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = m;
            dc.aabb = {{c.x - t.x * 0.5f, c.y - t.y * 0.5f, c.z - t.z * 0.5f},
                       {c.x + t.x * 0.5f, c.y + t.y * 0.5f, c.z + t.z * 0.5f}};
            dc.castShadow = false;
            dc.tint = tint;
            r3d->Submit(dc);
        };
        auto boite = [&](NkVec3f c, NkVec3f t, NkVec3f tint) {
            soumettre(NkMat4f::Translate(c) * NkMat4f::Scale(t), c, t, tint);
        };
        auto auSol = [&](NkVec3f t, float x, float z, NkVec3f tint) {
            soumettre(PoserAuSol(t, x, z), CentreAuSol(t, x, z), t, tint);
        };
        auto surTable = [&](NkVec3f t, float x, float z, NkVec3f tint) {
            soumettre(PoserSurTable(t, x, z), CentreSurTable(t, x, z), t, tint);
        };

        // DEBUT SALLE (objets)
        
        boite({0.f, -epaisseurMur * 0.5f, 0.f}, {longueurMur, epaisseurMur, longueurMur}, couleurSol);
        boite({0.f, hauteur + epaisseurMur * 0.5f, 0.f}, {longueurMur, epaisseurMur, longueurMur}, couleurMur);
        
        auSol({longueurMur, hauteur, epaisseurMur}, 0.f, -centreMur, couleurMur);
        auSol({longueurMur, hauteur, epaisseurMur}, 0.f, centreMur, couleurMur);
        auSol({epaisseurMur, hauteur, longueurMur}, -centreMur, 0.f, couleurMur);
        auSol({epaisseurMur, hauteur, longueurMur}, centreMur, 0.f, couleurMur);
        auSol({0.90f, 2.00f, epaisseurPanneau}, -0.70f, zPorte, couleurPorte);
        // Fenetres et plateau : suspendus, ils ne reposent pas sur le sol.
        boite({xFenetre, 1.40f, -0.90f}, {epaisseurPanneau, 1.00f, 1.20f}, couleurVitre);
        boite({xFenetre, 1.40f, 0.90f}, {epaisseurPanneau, 1.00f, 1.20f}, couleurVitre);
        boite({tableX, yPlateau, tableZ}, {longueur, epaisseur, profondeur}, couleurPlateau);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                auSol({section, hauteurPied, section}, tableX + signes[i] * decalX, tableZ + signes[j] * decalZ, couleurPied);
            }
        }
        
        surTable({0.20f, 0.20f, 0.20f}, 0.60f, 0.70f, couleurVase);
        // FIN SALLE (objets)

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}