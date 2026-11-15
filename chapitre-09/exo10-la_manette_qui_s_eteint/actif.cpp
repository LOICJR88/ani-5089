#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

struct EtatBool {
    bool current = false;   
    bool changed = false;   
    bool active  = false;  
};

struct Vec2 { float x; float y; };

static const int LARGEUR = 320;
static const int HAUTEUR = 240;
static std::vector<uint8_t> image(LARGEUR * HAUTEUR * 3, 255);

static void Rect(int x0, int y0, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    for (int y = y0; y < y0 + h; ++y) {
        for (int x = x0; x < x0 + w; ++x) {
            if (x < 0 || y < 0 || x >= LARGEUR || y >= HAUTEUR) continue;
            const size_t i = (static_cast<size_t>(y) * LARGEUR + x) * 3;
            image[i] = r; image[i + 1] = g; image[i + 2] = b;
        }
    }
}

static void Put32(std::vector<uint8_t>& v, uint32_t x) {
    v.push_back(static_cast<uint8_t>(x >> 24));
    v.push_back(static_cast<uint8_t>(x >> 16));
    v.push_back(static_cast<uint8_t>(x >> 8));
    v.push_back(static_cast<uint8_t>(x));
}

static uint32_t Crc(const std::vector<uint8_t>& d) {
    uint32_t c = 0xFFFFFFFFu;
    for (uint8_t b : d) {
        c ^= b;
        for (int k = 0; k < 8; ++k) c = (c & 1u) ? (c >> 1) ^ 0xEDB88320u : (c >> 1);
    }
    return ~c;
}

static void Chunk(std::vector<uint8_t>& out, const char* type, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> tmp(type, type + 4);
    tmp.insert(tmp.end(), data.begin(), data.end());
    Put32(out, static_cast<uint32_t>(data.size()));
    out.insert(out.end(), tmp.begin(), tmp.end());
    Put32(out, Crc(tmp));
}

static bool EcrirePng(const std::string& nom) {
    std::vector<uint8_t> brut;
    for (int y = 0; y < HAUTEUR; ++y) {
        brut.push_back(0);
        const size_t debut = static_cast<size_t>(y) * LARGEUR * 3;
        brut.insert(brut.end(), image.begin() + debut, image.begin() + debut + LARGEUR * 3);
    }

    std::vector<uint8_t> z = {0x78, 0x01};
    size_t pos = 0;
    while (pos < brut.size()) {
        const size_t n = (brut.size() - pos > 65535) ? 65535 : brut.size() - pos;
        const bool dernier = (pos + n == brut.size());
        z.push_back(dernier ? 1 : 0);
        z.push_back(static_cast<uint8_t>(n & 0xFF));
        z.push_back(static_cast<uint8_t>(n >> 8));
        z.push_back(static_cast<uint8_t>(~n & 0xFF));
        z.push_back(static_cast<uint8_t>((~n >> 8) & 0xFF));
        z.insert(z.end(), brut.begin() + pos, brut.begin() + pos + n);
        pos += n;
    }
    uint32_t a = 1, b = 0;
    for (uint8_t x : brut) { a = (a + x) % 65521u; b = (b + a) % 65521u; }
    Put32(z, (b << 16) | a);

    std::vector<uint8_t> ihdr;
    Put32(ihdr, LARGEUR);
    Put32(ihdr, HAUTEUR);
    ihdr.push_back(8);
    ihdr.push_back(2);
    ihdr.push_back(0); ihdr.push_back(0); ihdr.push_back(0);

    std::vector<uint8_t> png = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    Chunk(png, "IHDR", ihdr);
    Chunk(png, "IDAT", z);
    Chunk(png, "IEND", {});

    FILE* f = std::fopen(nom.c_str(), "wb");
    if (!f) return false;
    std::fwrite(png.data(), 1, png.size(), f);
    std::fclose(f);
    return true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage : actif.exe avant|apres fichier.png\n");
        return 1;
    }
    const std::string mode = argv[1];
    const std::string sortie = argv[2];
    const bool corrige = (mode == "apres");

    const int NB_IMAGES = 20;
    const int IMAGE_CAPTURE = 10;    
    const float SOL = 188.f;

    FILE* journal = std::fopen("journal.txt", "w");
    if (!journal) {
        std::fprintf(stderr, "impossible d'ecrire journal.txt\n");
        return 1;
    }

    EtatBool saisir;
    Vec2 cube = {60.f, 125.f};
    Vec2 attache = {0.f, 0.f};      
    float vitesseChute = 0.f;
    bool tient = false;
    bool dejaSaisi = false;

    for (int n = 1; n <= NB_IMAGES; ++n) {
        const Vec2 paume = {40.f + 8.f * n, 110.f};

        const bool appui = (n >= 2 && n <= 19);

        const bool manetteAllumee = !(n >= 6 && n <= 15);

        const bool nouveau = manetteAllumee ? appui : false;
        saisir.active  = manetteAllumee;
        saisir.changed = (nouveau != saisir.current);   
        saisir.current = nouveau;

        if (saisir.current && saisir.changed && !tient) {
            attache = {cube.x - paume.x, cube.y - paume.y};
            tient = true;
            dejaSaisi = true;
        }

        if (corrige) {
            if (saisir.active && !saisir.current && saisir.changed) {
                tient = false;
            }
        } else {
            if (!saisir.current) {
                tient = false;
            }
        }

        if (tient) {
            cube = {paume.x + attache.x, paume.y + attache.y};   
            vitesseChute = 0.f;
        } else if (dejaSaisi) {
            vitesseChute += 6.f;
            cube.y += vitesseChute;
            if (cube.y > SOL) { cube.y = SOL; vitesseChute = 0.f; }
        }

        std::fprintf(journal, "image : %d, courant : %d, change : %d, actif : %d\n",
                     n, saisir.current ? 1 : 0, saisir.changed ? 1 : 0, saisir.active ? 1 : 0);

        if (n == IMAGE_CAPTURE) {
            Rect(0, 200, LARGEUR, 4, 90, 90, 90);                                                     
            Rect(static_cast<int>(paume.x) - 20, static_cast<int>(paume.y) - 6, 40, 12, 40, 90, 200); 
            Rect(static_cast<int>(cube.x) - 12, static_cast<int>(cube.y) - 12, 24, 24, 230, 120, 30);   
            Rect(10, 10, 20, 20, tient ? 60 : 200, tient ? 180 : 60, 60);                              
            if (!saisir.active) {
                Rect(40, 10, 20, 20, 30, 30, 30);                                                      
            }
        }
    }

    std::fclose(journal);

    if (!EcrirePng(sortie)) {
        std::fprintf(stderr, "impossible d'ecrire %s\n", sortie.c_str());
        return 1;
    }
    return 0;
}
