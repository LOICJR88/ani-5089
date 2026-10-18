// Exercice 4 : le premier mur texture.
#include <string>
#include "NKImage/Core/NkImage.h"
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
#include "NKRenderer/Materials/NkMaterial.h"
#include "NKFileSystem/NkPath.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Mur4";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Mur4] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Mur4] Creation device KO");
        window.Close();
        return 2;
    }

    const std::string nomInterface = NkGraphicsApiName(devInfo.api);
    logger.Info(("[Mur4] Interface graphique : " + nomInterface).c_str());

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Mur4] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlan = renderer->GetMeshSystem()->GetPlane();

    NkString chemin = NkPath::LocateResource("murs/mur.jpg");
    if (chemin.Empty()) {
        logger.Error("[Chemin6] LocateResource n'a pas trouve murs/mur.jpg");
        chemin = "murs/mur.jpg";
    }
    auto tex = renderer->GetTextures()->Load(chemin);
    auto mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    mat->SetAlbedoMap(tex);

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
        cube({0.f, hauteur * 0.5f, -centreMur}, {longueurMur, hauteur, epaisseur}, mur, true);
        cube({0.f, hauteur * 0.5f, centreMur}, {longueurMur, hauteur, epaisseur}, mur);
        cube({-centreMur, hauteur * 0.5f, 0.f}, {epaisseur, hauteur, longueurMur}, mur);
        cube({centreMur, hauteur * 0.5f, 0.f}, {epaisseur, hauteur, longueurMur}, mur);

        cube({0.f, hauteur + epaisseur * 0.5f, 0.f}, {longueurMur, epaisseur, longueurMur}, {0.85f, 0.85f, 0.82f});

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}