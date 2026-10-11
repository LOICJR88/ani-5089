// Exercice 22 : la porte et les fenetres.
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
    d.appName = "Ouvertures22";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "La porte et les fenetres";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Ouvertures22] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Ouvertures22] Creation device KO");
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Ouvertures22] Init renderer KO");
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
    
    const NkVec3f camPos = {1.0f, 1.60f, 1.5f};
    const NkVec3f camTarget = {-1.2f, 1.30f, -1.0f};

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
        
        auto boite = [&](NkVec3f c, NkVec3f t, NkVec3f tint) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate(c) * NkMat4f::Scale(t);
            dc.aabb = {{c.x - t.x * 0.5f, c.y - t.y * 0.5f, c.z - t.z * 0.5f},
                       {c.x + t.x * 0.5f, c.y + t.y * 0.5f, c.z + t.z * 0.5f}};
            dc.castShadow = false;
            dc.tint = tint;
            r3d->Submit(dc);
        };

        const NkVec3f couleurMur = {0.72f, 0.70f, 0.66f};
        
        boite({0.f, -epaisseurMur * 0.5f, 0.f}, {longueurMur, epaisseurMur, longueurMur}, {0.30f, 0.28f, 0.26f});
        boite({0.f, hauteur + epaisseurMur * 0.5f, 0.f}, {longueurMur, epaisseurMur, longueurMur}, couleurMur);
        boite({0.f, hauteur * 0.5f, -centreMur}, {longueurMur, hauteur, epaisseurMur}, couleurMur); 
        boite({0.f, hauteur * 0.5f, centreMur}, {longueurMur, hauteur, epaisseurMur}, couleurMur);  
        boite({-centreMur, hauteur * 0.5f, 0.f}, {epaisseurMur, hauteur, longueurMur}, couleurMur); 
        boite({centreMur, hauteur * 0.5f, 0.f}, {epaisseurMur, hauteur, longueurMur}, couleurMur);  
        
        boite({-0.70f, 1.00f, zPorte}, {0.90f, 2.00f, epaisseurPanneau}, {0.35f, 0.20f, 0.10f});
        
        boite({xFenetre, 1.40f, -0.90f}, {epaisseurPanneau, 1.00f, 1.20f}, {0.50f, 0.70f, 0.90f});
        boite({xFenetre, 1.40f, 0.90f}, {epaisseurPanneau, 1.00f, 1.20f}, {0.50f, 0.70f, 0.90f});

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}