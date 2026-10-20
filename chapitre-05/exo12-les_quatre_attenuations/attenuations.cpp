// Exercice 12 : les quatre attenuations, mesurees par le moteur.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKLogger/NkLog.h"
#include "NKTime/NkChrono.h"
#include "NKAudio/NkAudio.h"

using namespace nkentseu;
using namespace nkentseu::audio;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Attenuations12";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

struct Modele { const char *nom; AttenuationModel valeur; };
static const Modele MODELES[4] = {
    { "NONE",        AttenuationModel::NONE },
    { "INVERSE",     AttenuationModel::INVERSE },
    { "LINEAR",      AttenuationModel::LINEAR },
    { "EXPONENTIAL", AttenuationModel::EXPONENTIAL },
};
static const float DISTANCES[5] = { 0.5f, 1.f, 2.f, 4.f, 8.f };
static const float ROLLOFFS[2] = { 1.f, 2.f };

static VoiceParams Reglages(AttenuationModel m, float rolloff, float d) {
    VoiceParams p;
    p.looping = true;
    p.source3d.positional = true;
    p.source3d.minDistance = 1.f;
    p.source3d.maxDistance = 100.f;
    p.source3d.rolloffFactor = rolloff;
    p.source3d.attenuationModel = m;
    p.source3d.position[0] = 0.f;
    p.source3d.position[1] = 0.f;
    p.source3d.position[2] = -d;        
    return p;
}

static double Mesurer(AudioEngine &son, const AudioSample &sinus, AttenuationModel m, float rolloff, float d) {
    AudioHandle h = son.Play(sinus, Reglages(m, rolloff, d));
    const int TOTAL = 48000, BLOC = 256;
    float tampon[BLOC * 2];
    double somme = 0.0;
    int fait = 0;
    while (fait < TOTAL) {
        const int n = (TOTAL - fait < BLOC) ? (TOTAL - fait) : BLOC;
        son.RenderToBuffer(tampon, n, 2);
        for (int i = 0; i < n * 2; ++i) { somme += (double)tampon[i] * (double)tampon[i]; }
        fait += n;
    }
    son.Stop(h);                     
    return std::sqrt(somme / (double)(TOTAL * 2));
}

int nkmain(const NkEntryState &state) {
    (void)state;

    AudioSample sinus = AudioGenerator::GenerateTone(440.f, 2.f, WaveformType::SINE, 48000, 0.1f);

    const char *eModele = std::getenv("ATT_MODELE");
    if (eModele) {
        AudioEngineConfig cfgSon;
        cfgSon.sampleRate = 48000;
        AudioEngine &son = AudioEngine::Instance();
        if (!son.Initialize(cfgSon)) { logger.Error("[Att12] Initialize KO"); return 1; }
        son.SetListenerPosition(0.f, 0.f, 0.f);
        son.SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);
        const char *eDist = std::getenv("ATT_DIST");
        const float d = eDist ? (float)std::atof(eDist) : 1.f;
        AttenuationModel m = AttenuationModel::INVERSE;
        for (int i = 0; i < 4; ++i) { if (std::strcmp(MODELES[i].nom, eModele) == 0) { m = MODELES[i].valeur; } }
        std::printf("ecoute : %s a %g m (4 secondes)\n", eModele, (double)d);
        son.Play(sinus, Reglages(m, 1.f, d));
        NkChrono::SleepMilliseconds(4000);
        son.Shutdown();
        AudioLoader::Free(sinus);
        return 0;
    }

    AudioEngineConfig cfg;
    cfg.backend = AudioBackendType::NULL_OUTPUT;
    cfg.sampleRate = 48000;
    AudioEngine &son = AudioEngine::Instance();
    if (!son.Initialize(cfg)) { logger.Error("[Att12] Initialize KO"); return 1; }
    son.SetListenerPosition(0.f, 0.f, 0.f);
    son.SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);

    double rms[2][4][5];
    for (int r = 0; r < 2; ++r)
        for (int m = 0; m < 4; ++m)
            for (int d = 0; d < 5; ++d)
                rms[r][m][d] = Mesurer(son, sinus, MODELES[m].valeur, ROLLOFFS[r], DISTANCES[d]);

    FILE *sortie = std::fopen("mesures.txt", "w");
    FILE *donnees = std::fopen("donnees.csv", "w");  
    for (int r = 0; r < 2; ++r) {
        for (int m = 0; m < 4; ++m) {
            for (int d = 0; d < 5; ++d) {
                const double ref = rms[r][0][d];     
                const double gain = (ref > 0.0) ? rms[r][m][d] / ref : 0.0;
                char ligne[160];
                std::snprintf(ligne, sizeof(ligne), "%s, rolloff %g, %g m : %.3f",
                              MODELES[m].nom, (double)ROLLOFFS[r], (double)DISTANCES[d], gain);
                std::printf("%s\n", ligne);
                if (sortie) { std::fprintf(sortie, "%s\n", ligne); }
                if (donnees) { std::fprintf(donnees, "%s,%g,%g,%.6f\n", MODELES[m].nom, (double)ROLLOFFS[r], (double)DISTANCES[d], gain); }
            }
        }
    }
    if (sortie) { std::fclose(sortie); }
    if (donnees) { std::fclose(donnees); }

    son.Shutdown();
    AudioLoader::Free(sinus);
    return 0;
}