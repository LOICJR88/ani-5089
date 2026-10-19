// Exercice 8 : le SDF mesure.
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "NKFont/Core/NkFontParser.h"
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
    d.appName = "Sdf8";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const float ECHELLE = 8.f;   

static float Borne(float x, float a, float b) {
    return x < a ? a : (x > b ? b : x);
}

static float smoothstep(float a, float b, float x) {
    float t = Borne((x - a) / (b - a), 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

static float LireBilineaire(const nkft_uint8 *data, int w, int h, float u, float v) {
    const float fx = u * (float)w - 0.5f;
    const float fy = v * (float)h - 0.5f;
    const int x0 = (int)std::floor(fx);
    const int y0 = (int)std::floor(fy);
    const float tx = fx - (float)x0;
    const float ty = fy - (float)y0;
    auto px = [&](int x, int y) -> float {
        if (x < 0) x = 0;
        if (x >= w) x = w - 1;
        if (y < 0) y = 0;
        if (y >= h) y = h - 1;
        return (float)data[y * w + x];
    };
    const float haut = px(x0, y0) * (1.f - tx) + px(x0 + 1, y0) * tx;
    const float bas = px(x0, y0 + 1) * (1.f - tx) + px(x0 + 1, y0 + 1) * tx;
    return haut * (1.f - ty) + bas * ty;
}

static bool ConstruireAtlas(NkFontAtlas &atlas, float taille, bool sdf, NkFont *&police,
                            nkft_uint8 *&alpha, nkft_int32 &w, nkft_int32 &h) {
    if (sdf) {
        atlas.sdfMode = true;
        atlas.sdfSpread = 6;
        if (const char *sp = std::getenv("PANNEAU_SPREAD")) { atlas.sdfSpread = std::atoi(sp); }
        if (const char *pd = std::getenv("PANNEAU_PAD")) { atlas.texGlyphPadding = std::atoi(pd); }
    }
    police = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, taille);
    if (!police) { return false; }
    if (!atlas.Build()) { return false; }
    alpha = nullptr;
    w = 0;
    h = 0;
    atlas.GetTexDataAsAlpha8(&alpha, &w, &h);
    return alpha && w > 0 && h > 0;
}

static NkImage FabriquerImage(const NkFont *mise, const NkFont *police, const nkft_uint8 *alpha,
                              int texL, int texH, bool sdf, float s, int L, int H) {
    NkImage img = NkImage::Create((uint32)L, (uint32)H, 4, 0x0B6B2EFFu);
    if (!img.IsValid()) { return img; }
    const uint8 fondR = 0x0B, fondG = 0x6B, fondB = 0x2E;
    const char *texte = "Sortie";
    const float base = ECHELLE * mise->ascent;
    float curseur = 0.f;
    for (int i = 0; i < 6; ++i) {
        const NkFontGlyph *g = police->FindGlyph((NkFontCodepoint)texte[i]);
        const NkFontGlyph *gm = mise->FindGlyph((NkFontCodepoint)texte[i]);
        if (!g || !gm) { continue; }
        const float gx0 = curseur + ECHELLE * g->x0, gx1 = curseur + ECHELLE * g->x1;
        const float gy0 = base + ECHELLE * g->y0, gy1 = base + ECHELLE * g->y1;
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
                    const float valeur = LireBilineaire(alpha, texL, texH, u, v) / 255.f;
                    const float couv = sdf ? smoothstep(0.5f - s, 0.5f + s, valeur) : valeur;
                    const uint8 r = (uint8)(fondR + (255 - fondR) * couv);
                    const uint8 gg = (uint8)(fondG + (255 - fondG) * couv);
                    const uint8 b = (uint8)(fondB + (255 - fondB) * couv);
                    img.SetPixel(px, py, math::NkColor(r, gg, b, 255));
                }
            }
        }
        curseur += ECHELLE * gm->advanceX;
    }
    return img;
}

int nkmain(const NkEntryState &state) {
    (void)state;
    char ligne[160];

    NkFontAtlas atlasOrd, atlasSdf, atlas128;
    NkFont *policeOrd = nullptr, *policeSdf = nullptr, *police128 = nullptr;
    nkft_uint8 *alphaOrd = nullptr, *alphaSdf = nullptr, *alpha128 = nullptr;
    nkft_int32 ordL = 0, ordH = 0, sdfL = 0, sdfH = 0, g128L = 0, g128H = 0;

    if (!ConstruireAtlas(atlasOrd, 32.f, false, policeOrd, alphaOrd, ordL, ordH)) { logger.Error("[Sdf8] atlas ordinaire 32 KO"); return 10; }
    if (!ConstruireAtlas(atlasSdf, 32.f, true, policeSdf, alphaSdf, sdfL, sdfH)) { logger.Error("[Sdf8] atlas sdf 32 KO"); return 11; }
    if (!ConstruireAtlas(atlas128, 128.f, false, police128, alpha128, g128L, g128H)) { logger.Error("[Sdf8] atlas ordinaire 128 KO"); return 12; }

    FILE *fa = std::fopen("atlas.txt", "w");
    if (fa) {
        std::fprintf(fa, "ordinaire 32 : %d x %d\n", (int)ordL, (int)ordH);
        std::fprintf(fa, "sdf 32 : %d x %d\n", (int)sdfL, (int)sdfH);
        std::fprintf(fa, "ordinaire 128 : %d x %d\n", (int)g128L, (int)g128H);
        std::fclose(fa);
    }

    auto SauverAtlas = [](const nkft_uint8 *data, int w, int h, const char *nom) {
        NkImage a = NkImage::Create((uint32)w, (uint32)h, 4, 0x000000FFu);
        long long nonNuls = 0;
        int maxi = 0;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const int v = data[y * w + x];
                if (v != 0) { ++nonNuls; }
                if (v > maxi) { maxi = v; }
                a.SetPixel(x, y, math::NkColor((uint8)v, (uint8)v, (uint8)v, 255));
            }
        }
        a.SavePNG(nom);
        std::printf("%s : %lld pixels non nuls, valeur max %d\n", nom, nonNuls, maxi);
    };
    SauverAtlas(alphaOrd, (int)ordL, (int)ordH, "atlas_ord.png");
    SauverAtlas(alphaSdf, (int)sdfL, (int)sdfH, "atlas_sdf.png");

    std::snprintf(ligne, sizeof(ligne), "atlas : ordinaire 32 = %d x %d, sdf 32 = %d x %d, ordinaire 128 = %d x %d",
                  (int)ordL, (int)ordH, (int)sdfL, (int)sdfH, (int)g128L, (int)g128H);
    std::printf("%s\n", ligne);
    logger.Info(ligne);

    float largeurF = 0.f;
    const char *texte = "Sortie";
    for (int i = 0; i < 6; ++i) {
        const NkFontGlyph *g = policeOrd->FindGlyph((NkFontCodepoint)texte[i]);
        if (!g) { logger.Error("[Sdf8] lettre absente"); return 13; }
        largeurF += ECHELLE * g->advanceX;
    }
    const int L = (int)std::ceil(largeurF);
    const int H = (int)std::ceil(ECHELLE * (policeOrd->ascent - policeOrd->descent));

    float s = 0.01f;                
    if (const char *es = std::getenv("PANNEAU_S")) { s = (float)std::atof(es); }

    NkImage imgOrd = FabriquerImage(policeOrd, policeOrd, alphaOrd, (int)ordL, (int)ordH, false, s, L, H);
    NkImage imgSdfMoteur = FabriquerImage(policeOrd, policeSdf, alphaSdf, (int)sdfL, (int)sdfH, true, s, L, H);
    imgSdfMoteur.SavePNG("sdf_moteur.png");

    std::vector<nkft_uint8> sdfCorrige((size_t)ordL * (size_t)ordH);
    nkfont::NkMakeSDFFromBitmap(alphaOrd, ordL, ordH, sdfCorrige.data(), 6);
    NkImage imgSdf = FabriquerImage(policeOrd, policeOrd, sdfCorrige.data(), (int)ordL, (int)ordH, true, s, L, H);
    if (!imgOrd.IsValid() || !imgSdf.IsValid()) { logger.Error("[Sdf8] image KO"); return 14; }

    std::snprintf(ligne, sizeof(ligne), "images : ordinaire %d x %d, sdf %d x %d, s = %.3f",
                  imgOrd.Width(), imgOrd.Height(), imgSdf.Width(), imgSdf.Height(), (double)s);
    std::printf("%s\n", ligne);
    logger.Info(ligne);

    imgOrd.SavePNG("ordinaire.png");
    imgSdf.SavePNG("sdf.png");

    NkImage texOrd = imgOrd.Copy();
    NkImage texSdf = imgSdf.Copy();
    texOrd.FlipHorizontal();
    texSdf.FlipHorizontal();
    texOrd.SavePNG("ordinaire_tex.png");
    texSdf.SavePNG("sdf_tex.png");

    if (std::getenv("PANNEAU_IMAGES_SEULES")) { return 0; } 

    const char *mode = std::getenv("PANNEAU_MODE");
    const bool montrerSdf = mode && std::string(mode) == "sdf";
    const char *fichierTexture = montrerSdf ? "sdf_tex.png" : "ordinaire_tex.png";
    const float ratio = (float)L / (float)H;

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) { logger.Error("[Sdf8] Creation fenetre KO"); return 1; }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) { logger.Error("[Sdf8] Creation device KO"); window.Close(); return 2; }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Sdf8] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlan = renderer->GetMeshSystem()->GetPlane();

    auto texPanneau = renderer->GetTextures()->Load(fichierTexture);
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
    const float32 panL = panH * ratio;
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