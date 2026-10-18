#include "NKImage/Core/NkImage.h"
#include <cstdio>

using namespace nkentseu;

static long TailleFichier(const char* chemin) {
    FILE* f = std::fopen(chemin, "rb");
    if (!f) { return -1; }
    std::fseek(f, 0, SEEK_END);
    long taille = std::ftell(f);
    std::fclose(f);
    return taille;
}

int main() {
    NkImage photo;
    if (!photo.Load("photo.jpg")) {
        std::printf("Chargement de photo.jpg impossible : verifie le repertoire de lancement\n");
        return 1;
    }
    const int L0 = photo.Width();
    const int H0 = photo.Height();
    std::printf("photo : %d x %d, %ld octets sur le disque\n", L0, H0, TailleFichier("photo.jpg"));

    std::printf("en memoire : %llu octets (%d canaux)\n",
                (unsigned long long)photo.TotalBytes(), photo.Channels());

    struct Cas { const char* nom; const char* fichier; bool ecrit; };
    Cas cas[5] = {
        { "png",  "sortie.png", photo.SavePNG("sortie.png")  },
        { "jpeg", "sortie.jpg", photo.SaveJPEG("sortie.jpg") },
        { "bmp",  "sortie.bmp", photo.SaveBMP("sortie.bmp")  },
        { "tga",  "sortie.tga", photo.SaveTGA("sortie.tga")  },
        { "qoi",  "sortie.qoi", photo.SaveQOI("sortie.qoi")  },
    };

    FILE* sortie = std::fopen("formats.txt", "w");
    if (!sortie) { return 1; }

    for (const Cas& c : cas) {
        const long octets = TailleFichier(c.fichier);
        NkImage relue;                
        if (c.ecrit && relue.Load(c.fichier)) {
            std::fprintf(sortie, "%s : %ld octets, %d x %d\n",
                         c.nom, octets, relue.Width(), relue.Height());
            std::printf("%s : %ld octets, %d x %d\n", c.nom, octets, relue.Width(), relue.Height());
            if (relue.Width() != L0 || relue.Height() != H0) {
                std::printf("  ATTENTION : dimensions differentes de la photo (%d x %d)\n", L0, H0);
            }
        } else {
            std::fprintf(sortie, "%s : %ld octets, illisible\n", c.nom, octets);
            std::printf("%s : %ld octets, illisible (ecrit=%d)\n", c.nom, octets, c.ecrit ? 1 : 0);
        }
    }
    std::fclose(sortie);
    return 0;
}