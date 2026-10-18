// filtres.cpp : quatre filtres, une mesure.
#include "NKImage/Core/NkImage.h"
#include "NKTime/NkChrono.h"
#include <cstdio>
#include <utility>

using namespace nkentseu;

static const int REPETITIONS = 20;

static long long CompterDifferences(const NkImage &a, const NkImage &b) {
    if (a.Width() != b.Width() || a.Height() != b.Height()) { return -1; }
    long long n = 0;
    for (int y = 0; y < a.Height(); ++y) {
        for (int x = 0; x < a.Width(); ++x) {
            auto pa = a.GetPixel(x, y);
            auto pb = b.GetPixel(x, y);
            if (pa.r != pb.r || pa.g != pb.g || pa.b != pb.b || pa.a != pb.a) { ++n; }
        }
    }
    return n;
}

int main() {
    NkImage photo;
    if (!photo.Load("photo.jpg")) {
        std::printf("Chargement de photo.jpg impossible : lance depuis le dossier de la photo\n");
        return 1;
    }

    NkImage source = photo.Resize(2048, 2048);
    if (!source.IsValid()) {
        std::printf("Resize 2048 x 2048 impossible\n");
        return 1;
    }
    std::printf("source : %d x %d\n", source.Width(), source.Height());

    struct Cas { const char *nom; const char *fichier; NkResizeFilter filtre; };
    Cas cas[4] = {
        { "NK_NEAREST",  "nearest.png",  NkResizeFilter::NK_NEAREST  },
        { "NK_BILINEAR", "bilinear.png", NkResizeFilter::NK_BILINEAR },
        { "NK_BICUBIC",  "bicubic.png",  NkResizeFilter::NK_BICUBIC  },
        { "NK_LANCZOS3", "lanczos3.png", NkResizeFilter::NK_LANCZOS3 },
    };

    NkImage resultats[4];
    long long moyenne[4] = {0, 0, 0, 0};

    for (int i = 0; i < 4; ++i) {
        { NkImage chauffe = source.Resize(256, 256, cas[i].filtre); }

        double total = 0.0;
        for (int r = 0; r < REPETITIONS; ++r) {
            NkChrono chrono;
            NkImage reduite = source.Resize(256, 256, cas[i].filtre);
            total += chrono.Elapsed().ToMicroseconds();
            if (r == REPETITIONS - 1) { resultats[i] = std::move(reduite); }
        }
        moyenne[i] = (long long)(total / REPETITIONS + 0.5);
        std::printf("%s : %lld us (moyenne de %d)\n", cas[i].nom, moyenne[i], REPETITIONS);

        if (!resultats[i].IsValid() || !resultats[i].SavePNG(cas[i].fichier)) {
            std::printf("  ATTENTION : %s non ecrit\n", cas[i].fichier);
        }
    }

    FILE *sortie = std::fopen("mesures.txt", "w");
    if (!sortie) { return 1; }
    for (int i = 0; i < 4; ++i) {
        long long d = CompterDifferences(resultats[i], resultats[1]);
        std::fprintf(sortie, "%s : %lld us, differences : %lld\n", cas[i].nom, moyenne[i], d);
        std::printf("%s : %lld us, differences : %lld\n", cas[i].nom, moyenne[i], d);
    }
    std::fclose(sortie);
    return 0;
}