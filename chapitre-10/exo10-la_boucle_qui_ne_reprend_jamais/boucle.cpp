#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

namespace {

const int kLargeur = 640;
const int kHauteur = 360;
const double kPi = 3.14159265358979323846;
const double kFocale = 320.0;

const int kTrames = 20;
const int kPauseDebut = 6;  
const int kPauseFin = 10;   
const double kVitesse = 1.5; 

enum Statut { kRendue = 0, kPause = 1, kBloquee = 2 };

struct Couche {
    double lacet; 
};

class Session {
public:
    bool WaitFrame() const { return !mOuverte; }
    void BeginFrame() { mOuverte = true; }
    void EndFrame(const Couche* couche) {
        mOuverte = false;
        if (couche != nullptr) {
            mAffiche = couche->lacet;  
        }
    }
    bool Ouverte() const { return mOuverte; }
    double Affiche() const { return mAffiche; }

private:
    bool mOuverte = false;
    double mAffiche = 0.0;
};

void PomperEvenements() {}      
void SynchroniserActions() {}   
bool ShouldRender(int trame) {
    return !(trame >= kPauseDebut && trame <= kPauseFin);
}

void Journal(std::FILE* f, int trame, int rend, int debut, int fin) {
    std::printf("trame : %d, rend : %d, debut : %d, fin : %d\n", trame, rend, debut, fin);
    if (f != nullptr) {
        std::fprintf(f, "trame : %d, rend : %d, debut : %d, fin : %d\n", trame, rend, debut, fin);
    }
}


struct Image {
    std::vector<unsigned char> pixels;
    Image() : pixels(static_cast<std::size_t>(kLargeur) * kHauteur * 3, 0) {}
};

void Point(Image& img, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x < 0 || x >= kLargeur || y < 0 || y >= kHauteur) {
        return;
    }
    const std::size_t i = (static_cast<std::size_t>(y) * kLargeur + static_cast<std::size_t>(x)) * 3;
    img.pixels[i] = r;
    img.pixels[i + 1] = g;
    img.pixels[i + 2] = b;
}

void Rect(Image& img, int x0, int y0, int x1, int y1,
          unsigned char r, unsigned char g, unsigned char b) {
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            Point(img, x, y, r, g, b);
        }
    }
}

int Colonne(double azimut, double lacet) {
    double delta = azimut - lacet;
    delta = std::max(-80.0, std::min(80.0, delta));
    return static_cast<int>(std::lround(kLargeur / 2.0 + kFocale * std::tan(delta * kPi / 180.0)));
}

void Objet(Image& img, double lacet, double a0, double a1, int y0, int y1,
           unsigned char r, unsigned char g, unsigned char b) {
    Rect(img, Colonne(a0, lacet), y0, Colonne(a1, lacet), y1, r, g, b);
}

Image Capturer(double lacetAffiche, const int* statuts, bool bloque) {
    Image img;
    Rect(img, 0, 0, kLargeur, kHauteur, 200, 196, 188);
    Rect(img, 0, 270, kLargeur, kHauteur, 120, 100, 80);
    Objet(img, lacetAffiche, -30.0, -14.0, 90, 270, 60, 80, 140);
    Objet(img, lacetAffiche, 8.0, 28.0, 215, 270, 150, 100, 50);
    Objet(img, lacetAffiche, -1.5, 1.5, 40, 270, 40, 160, 80);

    if (bloque) {
        Rect(img, 0, 0, kLargeur, 12, 210, 40, 40);
    } else {
        Rect(img, 0, 0, kLargeur, 12, 60, 170, 80);
    }

    for (int i = 0; i < kTrames; ++i) {
        const int x = 20 + i * 30;
        if (statuts[i] == kRendue) {
            Rect(img, x, 318, x + 24, 342, 60, 170, 80);
        } else if (statuts[i] == kPause) {
            Rect(img, x, 318, x + 24, 342, 230, 150, 40);
        } else {
            Rect(img, x, 318, x + 24, 342, 210, 40, 40);
        }
    }
    return img;
}


void Octets32(std::vector<unsigned char>& v, std::uint32_t x) {
    v.push_back(static_cast<unsigned char>((x >> 24) & 0xFF));
    v.push_back(static_cast<unsigned char>((x >> 16) & 0xFF));
    v.push_back(static_cast<unsigned char>((x >> 8) & 0xFF));
    v.push_back(static_cast<unsigned char>(x & 0xFF));
}

std::uint32_t Crc32(const std::vector<unsigned char>& donnees) {
    std::uint32_t c = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < donnees.size(); ++i) {
        c ^= donnees[i];
        for (int k = 0; k < 8; ++k) {
            c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
    }
    return c ^ 0xFFFFFFFFu;
}

std::uint32_t Adler32(const std::vector<unsigned char>& donnees) {
    std::uint32_t a = 1;
    std::uint32_t b = 0;
    for (std::size_t i = 0; i < donnees.size(); ++i) {
        a = (a + donnees[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 16) | a;
}

std::vector<unsigned char> Zlib(const std::vector<unsigned char>& brut) {
    std::vector<unsigned char> z;
    z.push_back(0x78);
    z.push_back(0x01);
    std::size_t pos = 0;
    while (pos < brut.size()) {
        const std::size_t n = std::min<std::size_t>(65535, brut.size() - pos);
        const bool dernier = (pos + n == brut.size());
        z.push_back(static_cast<unsigned char>(dernier ? 1 : 0));
        z.push_back(static_cast<unsigned char>(n & 0xFF));
        z.push_back(static_cast<unsigned char>((n >> 8) & 0xFF));
        z.push_back(static_cast<unsigned char>(~n & 0xFF));
        z.push_back(static_cast<unsigned char>((~n >> 8) & 0xFF));
        z.insert(z.end(), brut.begin() + static_cast<std::ptrdiff_t>(pos),
                 brut.begin() + static_cast<std::ptrdiff_t>(pos + n));
        pos += n;
    }
    Octets32(z, Adler32(brut));
    return z;
}

void Chunk(std::vector<unsigned char>& fichier, const char* type,
           const std::vector<unsigned char>& data) {
    Octets32(fichier, static_cast<std::uint32_t>(data.size()));
    std::vector<unsigned char> corps;
    for (int i = 0; i < 4; ++i) {
        corps.push_back(static_cast<unsigned char>(type[i]));
    }
    corps.insert(corps.end(), data.begin(), data.end());
    fichier.insert(fichier.end(), corps.begin(), corps.end());
    Octets32(fichier, Crc32(corps));
}

bool EcrirePng(const char* nom, const Image& img) {
    std::vector<unsigned char> brut;
    for (int y = 0; y < kHauteur; ++y) {
        brut.push_back(0);
        const std::size_t debut = static_cast<std::size_t>(y) * kLargeur * 3;
        brut.insert(brut.end(), img.pixels.begin() + static_cast<std::ptrdiff_t>(debut),
                    img.pixels.begin() + static_cast<std::ptrdiff_t>(debut + kLargeur * 3));
    }

    std::vector<unsigned char> fichier;
    const unsigned char signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    fichier.insert(fichier.end(), signature, signature + 8);

    std::vector<unsigned char> ihdr;
    Octets32(ihdr, static_cast<std::uint32_t>(kLargeur));
    Octets32(ihdr, static_cast<std::uint32_t>(kHauteur));
    ihdr.push_back(8);
    ihdr.push_back(2);
    ihdr.push_back(0);
    ihdr.push_back(0);
    ihdr.push_back(0);
    Chunk(fichier, "IHDR", ihdr);
    Chunk(fichier, "IDAT", Zlib(brut));
    Chunk(fichier, "IEND", std::vector<unsigned char>());

    std::FILE* f = std::fopen(nom, "wb");
    if (f == nullptr) {
        return false;
    }
    const std::size_t ecrits = std::fwrite(fichier.data(), 1, fichier.size(), f);
    std::fclose(f);
    return ecrits == fichier.size();
}

}  

int main() {
    std::FILE* journal = nullptr;
#ifdef FAUTIVE
    const char* nomCapture = "avant.png";
    journal = std::fopen("journal.txt", "w");
    if (journal == nullptr) {
        std::fprintf(stderr, "impossible d'ouvrir journal.txt\n");
        return 1;
    }
#else
    const char* nomCapture = "apres.png";
#endif

    Session session;
    int statuts[kTrames];

    for (int trame = 1; trame <= kTrames; ++trame) {
        PomperEvenements();

        if (!session.WaitFrame()) {
            Journal(journal, trame, 0, 0, 0);
            statuts[trame - 1] = kBloquee;
            continue;
        }
        const bool shouldRender = ShouldRender(trame);

        session.BeginFrame();

        SynchroniserActions();

        Couche couche{0.0};
        int fin = 0;
        if (shouldRender) {
            couche.lacet = kVitesse * trame;  
#ifdef FAUTIVE
            // VERSION FAUTIVE : la fin de trame est DANS le test de rendu.
            session.EndFrame(&couche);
            fin = 1;
#endif
        }

#ifndef FAUTIVE
        session.EndFrame(shouldRender ? &couche : nullptr);
        fin = 1;
#endif

        Journal(journal, trame, shouldRender ? 1 : 0, 1, fin);
        if (fin == 0) {
            statuts[trame - 1] = kBloquee;
        } else {
            statuts[trame - 1] = shouldRender ? kRendue : kPause;
        }
    }

    if (journal != nullptr) {
        std::fclose(journal);
    }

    const Image ecran = Capturer(session.Affiche(), statuts, session.Ouverte());
    if (!EcrirePng(nomCapture, ecran)) {
        std::fprintf(stderr, "impossible d'ecrire %s\n", nomCapture);
        return 1;
    }
    std::printf("ecrit : %s\n", nomCapture);
    return 0;
}