// Exercice 11 : mesure.cpp, sans fenetre et sans sortie sonore.
#include <cmath>
#include <cstdio>
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKLogger/NkLog.h"
#include "NKAudio/NkAudio.h"

using namespace nkentseu;
using namespace nkentseu::audio;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Mesure11";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    AudioEngineConfig cfg;
    cfg.backend = AudioBackendType::NULL_OUTPUT; 
    cfg.sampleRate = 48000;
    AudioEngine &son = AudioEngine::Instance();
    if (!son.Initialize(cfg)) {
        logger.Error("[Mesure11] Initialize KO");
        return 1;
    }

    son.SetListenerPosition(0.f, 0.f, 0.f);
    son.SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);

    AudioSample pluie = AudioGenerator::GenerateNoise(4.f, WaveformType::NOISE_PINK);
    if (!pluie.IsValid()) {
        logger.Error("[Mesure11] pluie KO");
        son.Shutdown();
        return 2;
    }

    struct Lieu { const char *nom; float x, y, z; };
    const Lieu lieux[3] = {
        { "gauche", -2.f, 0.f,  0.f },
        { "devant",  0.f, 0.f, -2.f },
        { "droite",  2.f, 0.f,  0.f },
    };

    FILE *sortie = std::fopen("mesures.txt", "w");
    const int TOTAL = 48000;
    const int BLOC = 256;
    float tampon[BLOC * 2];

    for (int positional = 1; positional >= 0; --positional) {
        for (int i = 0; i < 3; ++i) {
            VoiceParams p;
            p.looping = true;
            p.source3d.position[0] = lieux[i].x;
            p.source3d.position[1] = lieux[i].y;
            p.source3d.position[2] = lieux[i].z;
            p.source3d.minDistance = 0.5f;
            p.source3d.maxDistance = 8.0f;
            p.source3d.positional = (positional == 1);
            AudioHandle h = son.Play(pluie, p);

            double sommeG = 0.0, sommeD = 0.0;
            int fait = 0;
            while (fait < TOTAL) {
                const int n = (TOTAL - fait < BLOC) ? (TOTAL - fait) : BLOC;
                son.RenderToBuffer(tampon, n, 2);
                for (int t = 0; t < n; ++t) {
                    const double g = tampon[2 * t];      
                    const double d = tampon[2 * t + 1];
                    sommeG += g * g;
                    sommeD += d * d;
                }
                fait += n;
            }
            const double rmsG = std::sqrt(sommeG / TOTAL);
            const double rmsD = std::sqrt(sommeD / TOTAL);
            son.Stop(h);                                  

            char ligne[160];
            std::snprintf(ligne, sizeof(ligne), "%s %d : G %.5f D %.5f", lieux[i].nom, positional, rmsG, rmsD);
            std::printf("%s\n", ligne);
            logger.Info(ligne);
            if (sortie) { std::fprintf(sortie, "%s\n", ligne); }
        }
    }

    if (sortie) { std::fclose(sortie); }
    son.Shutdown();
    AudioLoader::Free(pluie);
    return 0;
}