// Exercice 11 : le son qui a une place.
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
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
#include "NKAudio/NkAudio.h"

using namespace nkentseu;
using namespace nkentseu::renderer;
using namespace nkentseu::audio;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Place11";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkVec3f camPos = {-1.5f, 1.60f, -0.9f};           
    const char *choixCam = std::getenv("PLACE_CAM");
    if (choixCam && std::string(choixCam) == "centre") { camPos = {0.f, 1.60f, 0.f}; }
    if (choixCam && std::string(choixCam) == "coin")   { camPos = {1.7f, 1.60f, 1.7f}; }
    const NkVec3f camTarget = {0.f, 1.4f, -2.0f};      

    float fx = camTarget.x - camPos.x, fy = camTarget.y - camPos.y, fz = camTarget.z - camPos.z;
    const float norme = std::sqrt(fx * fx + fy * fy + fz * fz);
    fx /= norme; fy /= norme; fz /= norme;

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) { logger.Error("[Place11] Creation fenetre KO"); return 1; }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) { logger.Error("[Place11] Creation device KO"); window.Close(); return 2; }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Place11] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlan = renderer->GetMeshSystem()->GetPlane();

    AudioEngineConfig cfgSon;
    cfgSon.sampleRate = 48000;
    AudioEngine &son = AudioEngine::Instance();
    const bool sonOk = son.Initialize(cfgSon);
    AudioSample pluie = AudioGenerator::GenerateNoise(4.f, WaveformType::NOISE_PINK);
    if (sonOk && pluie.IsValid()) {
        son.SetListenerPosition(camPos.x, camPos.y, camPos.z);
        son.SetListenerOrientation(fx, fy, fz, 0.f, 1.f, 0.f);

        VoiceParams p;
        p.looping = true;
        p.source3d.position[0] = -1.98f;     
        p.source3d.position[1] = 1.40f;
        p.source3d.position[2] = -0.90f;
        p.source3d.minDistance = 0.5f;       
        p.source3d.maxDistance = 8.0f;      
        const char *pos = std::getenv("PLACE_POSITIONAL");
        p.source3d.positional = !(pos && std::string(pos) == "0");
        son.Play(pluie, p);
        logger.Info(p.source3d.positional ? "[Place11] pluie : positional" : "[Place11] pluie : SANS positional");
    } else {
        logger.Error("[Place11] moteur de son ou pluie KO");
    }

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

        auto cube = [&](NkVec3f centre, NkVec3f taille, NkVec3f tint) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
            dc.aabb = {{centre.x - taille.x * 0.5f, centre.y - taille.y * 0.5f, centre.z - taille.z * 0.5f},
                       {centre.x + taille.x * 0.5f, centre.y + taille.y * 0.5f, centre.z + taille.z * 0.5f}};
            dc.castShadow = false;
            dc.tint = tint;
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

        cube({-1.95f, 1.40f, -0.90f}, {0.05f, 0.60f, 0.60f}, {0.55f, 0.75f, 0.95f});

        renderer->Present();
        renderer->EndFrame();
    }

    if (sonOk) { son.Shutdown(); }
    AudioLoader::Free(pluie);

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}