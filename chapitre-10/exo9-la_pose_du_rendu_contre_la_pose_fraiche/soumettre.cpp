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

const double kVitesse = 120.0;      
const double kPeriode = 1.0 / 72.0; 
const double kDureeRendu = 0.060;   
const int kTrameCapture = 3;        

struct Pose {
    double lacet;  
};

struct Couche {
    double lacetSoumis;  
};

struct Image {
    std::vector<unsigned char> pixels;
    Image() : pixels(static_cast<std::size_t>(kLargeur) * kHauteur * 3, 0) {}
};

Pose PoseTete(double t) {
    return Pose{kVitesse * t};
}

int Colonne(double azimut, double lacet) {
    double delta = azimut - lacet;
    delta = std::max(-80.0, std::min(80.0, delta));
    return static_cast<int>(std::lround(kLargeur / 2.0 + kFocale * std::tan(delta * kPi / 180.0)));
}

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

void Objet(Image& img, double lacet, double a0, double a1, int y0, int y1,
           unsigned char r, unsigned char g, unsigned char b) {
    Rect(img, Colonne(a0, lacet), y0, Colonne(a1, lacet), y1, r, g, b);
}

Image Rendre(double lacetVue, double lacetReperes) {
    Image img;
    Rect(img, 0, 0, kLargeur, kHauteur, 200, 196, 188);  
    Rect(img, 0, 270, kLargeur, kHauteur, 120, 100, 80); 
    Objet(img, lacetVue, -30.0, -14.0, 90, 270, 60, 80, 140);  
    Objet(img, lacetVue, 8.0, 28.0, 215, 270, 150, 100, 50);   
    Objet(img, lacetVue, -1.5, 1.5, 40, 270, 40, 160, 80);     

    const int xRef = Colonne(0.0, lacetReperes);
    Rect(img, xRef - 1, 0, xRef + 2, 25, 220, 30, 30);
    Rect(img, xRef - 1, 335, xRef + 2, kHauteur, 220, 30, 30);
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
    const char* nomFichier = nullptr;
    Image capture;

    for (int trame = 1; trame <= kTrameCapture; ++trame) {
        const double tDebut = trame * kPeriode;
        const double tSoumission = tDebut + kDureeRendu;

        const Pose vuesRendu = PoseTete(tDebut);

        const Pose poseReelle = PoseTete(tSoumission);

        Couche couche{};

          couche.lacetSoumis = vuesRendu.lacet;
          nomFichier = "rendu.png";

        //couche.lacetSoumis = PoseTete(tSoumission).lacet;
        //nomFichier = "fraiche.png";

        const double correction = poseReelle.lacet - couche.lacetSoumis;
        const double lacetEffectif = vuesRendu.lacet + correction;
        const double erreurRestante = vuesRendu.lacet - couche.lacetSoumis;

        std::printf("trame %d : rendu %.1f, reel %.1f, soumis %.1f, correction %.1f, erreur restante %.1f\n",
                    trame, vuesRendu.lacet, poseReelle.lacet, couche.lacetSoumis, correction,
                    erreurRestante);

        if (trame == kTrameCapture) {
            capture = Rendre(lacetEffectif, poseReelle.lacet);
        }
    }

    if (!EcrirePng(nomFichier, capture)) {
        std::fprintf(stderr, "impossible d'ecrire %s\n", nomFichier);
        return 1;
    }
    std::printf("ecrit : %s\n", nomFichier);
    return 0;
}
