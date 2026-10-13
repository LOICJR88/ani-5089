// Exercice 26 : le meme programme sur deux interfaces graphiques.
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <string>
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
    d.appName = "Interfaces26";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)


static void Noter(const std::string &ligne) {
    std::ofstream f("resultat.txt", std::ios::app);
    f << ligne << "\n";
}

int nkmain(const NkEntryState &state) {
    (void)state;

    
    const char *voulu = std::getenv("NK_API");
    const std::string nomVoulu = voulu ? voulu : "SOFTWARE";
    NkGraphicsApi api = NkGraphicsApi::NK_GFX_API_SOFTWARE;
    if (nomVoulu == "OPENGL") api = NkGraphicsApi::NK_GFX_API_OPENGL;
    else if (nomVoulu == "DX11") api = NkGraphicsApi::NK_GFX_API_DX11;
    else if (nomVoulu == "DX12") api = NkGraphicsApi::NK_GFX_API_DX12;
    else if (nomVoulu == "VULKAN") api = NkGraphicsApi::NK_GFX_API_VULKAN;

    NkWindowConfig winCfg;
    winCfg.title = ("Interfaces : " + nomVoulu).c_str();
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Interfaces26] Creation fenetre KO");
        Noter("refus " + nomVoulu + " : fenetre invalide");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    
    NkIDevice *device = NkDeviceFactory::CreateForApi(api, devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Interfaces26] Interface refusee");
        Noter("refus " + nomVoulu + " : CreateForApi a rendu un peripherique absent ou invalide (voir logs/app.log)");
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 2;
    }

    
    const std::string nomAffiche = NkGraphicsApiName(api);
    logger.Info(("[Interfaces26] Interface : " + nomAffiche).c_str());

    
    NkRendererConfig cfg = NkRendererConfig::ForGame(api, devInfo.width, devInfo.height);
    cfg.api = api;
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Interfaces26] Init renderer KO");
        Noter("refus " + nomVoulu + " : le renderer ne s'initialise pas (voir logs/app.log)");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();

    bool running = true;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) running = false;
    });

    
    const float32 demiSalle = 2.0f, epaisseurMur = 0.10f, hauteur = 2.5f;
    const float32 centreMur = demiSalle + epaisseurMur * 0.5f;
    const float32 longueurMur = 2.f * demiSalle + 2.f * epaisseurMur;
    const float32 epaisseurPanneau = 0.02f, decalage = 0.001f;
    const float32 zPorte = -demiSalle - epaisseurPanneau * 0.5f + decalage;
    const float32 xFenetre = -demiSalle - epaisseurPanneau * 0.5f + decalage;
    const float32 tableX = 0.60f, tableZ = 0.70f, longueur = 1.20f, profondeur = 0.80f;
    const float32 epaisseur = 0.05f, section = 0.06f, dessus = 0.75f;
    const float32 yPlateau = dessus - epaisseur * 0.5f;
    const float32 hauteurPied = dessus - epaisseur;
    const float32 decalX = longueur * 0.5f - section * 0.5f;
    const float32 decalZ = profondeur * 0.5f - section * 0.5f;
    const float32 signes[2] = {-1.f, 1.f};
    const NkVec3f camPos = {1.4f, 1.60f, 1.8f};
    const NkVec3f camTarget = {-0.6f, 1.0f, -0.8f};

    
    const int echauffement = 20;
    const int mesurees = 400;
    int numero = 0;
    double totalMs = 0.0;
    bool noteEcrite = false;

    while (running && window.IsOpen()) {
        const auto debutImage = std::chrono::steady_clock::now();
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

        const NkVec3f mur = {0.72f, 0.70f, 0.66f};
        boite({0.f, -epaisseurMur * 0.5f, 0.f}, {longueurMur, epaisseurMur, longueurMur}, {0.30f, 0.28f, 0.26f});
        boite({0.f, hauteur + epaisseurMur * 0.5f, 0.f}, {longueurMur, epaisseurMur, longueurMur}, mur);
        boite({0.f, hauteur * 0.5f, -centreMur}, {longueurMur, hauteur, epaisseurMur}, mur);
        boite({0.f, hauteur * 0.5f, centreMur}, {longueurMur, hauteur, epaisseurMur}, mur);
        boite({-centreMur, hauteur * 0.5f, 0.f}, {epaisseurMur, hauteur, longueurMur}, mur);
        boite({centreMur, hauteur * 0.5f, 0.f}, {epaisseurMur, hauteur, longueurMur}, mur);
        boite({-0.70f, 1.00f, zPorte}, {0.90f, 2.00f, epaisseurPanneau}, {0.35f, 0.20f, 0.10f});
        boite({xFenetre, 1.40f, -0.90f}, {epaisseurPanneau, 1.00f, 1.20f}, {0.50f, 0.70f, 0.90f});
        boite({xFenetre, 1.40f, 0.90f}, {epaisseurPanneau, 1.00f, 1.20f}, {0.50f, 0.70f, 0.90f});
        boite({tableX, yPlateau, tableZ}, {longueur, epaisseur, profondeur}, {0.75f, 0.55f, 0.30f});
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                boite({tableX + signes[i] * decalX, hauteurPied * 0.5f, tableZ + signes[j] * decalZ},
                      {section, hauteurPied, section}, {0.45f, 0.30f, 0.15f});
            }
        }
        boite({0.60f, dessus + 0.10f, 0.70f}, {0.20f, 0.20f, 0.20f}, {0.85f, 0.20f, 0.20f});

        renderer->Present();
        renderer->EndFrame();

        
        const std::chrono::duration<double, std::milli> duree = std::chrono::steady_clock::now() - debutImage;
        numero++;
        if (numero > echauffement && numero <= echauffement + mesurees) {
            totalMs += duree.count();
        }
        if (!noteEcrite && numero == echauffement + mesurees) {
            const double moyenne = totalMs / mesurees;
            Noter(nomAffiche + " " + std::to_string(moyenne) + " ms");
            noteEcrite = true;
        }
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}