// Exercice 23 : la table.
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
    d.appName = "Table23";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "La table";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Table23] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Table23] Creation device KO");
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Table23] Init renderer KO");
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
    
    const float32 tableX = 0.60f;         
    const float32 tableZ = 0.70f;            
    const float32 longueur = 1.20f;          
    const float32 profondeur = 0.80f;        
    const float32 dessus = 0.75f;            
    const float32 epaisseur = 0.05f;         
    const float32 section = 0.06f;           
    
    const float32 yPlateau = dessus - epaisseur * 0.5f;  
    
    const float32 hauteurPied = dessus - epaisseur;           
    const float32 yPied = hauteurPied * 0.5f;                   
    
    const float32 decalX = longueur * 0.5f - section * 0.5f;   
    const float32 decalZ = profondeur * 0.5f - section * 0.5f;  
    
    const NkVec3f camPos = {2.2f, 1.60f, 2.6f};
    const NkVec3f camTarget = {0.6f, 0.5f, 0.7f};

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
        
        boite({0.f, -0.05f, 0.f}, {6.f, 0.10f, 6.f}, {0.30f, 0.28f, 0.26f});
        
        boite({tableX, yPlateau, tableZ}, {longueur, epaisseur, profondeur}, {0.75f, 0.55f, 0.30f});
        
        const float32 signes[2] = {-1.f, 1.f};
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                boite({tableX + signes[i] * decalX, yPied, tableZ + signes[j] * decalZ},
                      {section, hauteurPied, section}, {0.45f, 0.30f, 0.15f});
            }
        }

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}