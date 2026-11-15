#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

// ---------- Les trois champs d'une action (comme au chapitre 12) ----------
struct EtatBool {
    bool current = false;   // vrai tant que la saisie est maintenue
    bool changed = false;   // a basculé depuis la synchronisation précédente
    bool active  = false;   // une source matérielle est liée
};

struct Vec2 { float x; float y; };

// ---------- Mini dessin en mémoire ----------
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

// ---------- Écriture d'un PNG sans bibliothèque (blocs deflate "stockés") ----------
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
        brut.push_back(0);   // filtre : aucun
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
    ihdr.push_back(8);   // 8 bits par canal
    ihdr.push_back(2);   // RGB
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

// ---------- La scène : une main qui avance, un cube qu'on saisit ----------
int main(int argc, char** argv) {
    const std::string sortie = (argc > 1) ? argv[1] : "capture.png";

    const int IMAGE_CAPTURE = 7;      // même instant du geste pour les deux captures
    const float SOL = 188.f;          // hauteur du centre du cube posé au sol

    EtatBool saisir;
    saisir.active = true;             // la saisie est liée à une source

    Vec2 cube = {70.f, 125.f};        // le cube attend à côté de la main
    Vec2 attache = {0.f, 0.f};        // pose du cube dans le repère de la main, figée à la saisie
    float vitesseChute = 0.f;
    bool dejaSaisi = false;

    for (int image_n = 0; image_n <= IMAGE_CAPTURE; ++image_n) {
        // La main (paume) avance vers la droite à vitesse constante.
        const Vec2 paume = {40.f + 15.f * image_n, 110.f};

        // Le joueur appuie de l'image 2 à l'image 9.
        const bool appui = (image_n >= 2 && image_n <= 9);
        saisir.changed = (appui != saisir.current);
        saisir.current = appui;

        // À la saisie, une fois : on fige la pose du cube dans le repère de la main
        // (équivalent de attache = posePaume.Inverted() * pose de l'objet).
        if (saisir.current && saisir.changed) {
            attache = {cube.x - paume.x, cube.y - paume.y};
        }

        // VERSION FAUTIVE (front seulement) : l'objet n'est tenu qu'à l'image de l'appui,
        // puis il échappe à la main.
        //const bool tenu = saisir.current && saisir.changed;

        // VERSION JUSTE (niveau) : l'objet est tenu tant que la saisie est vraie.
        const bool tenu = saisir.active && saisir.current;

        if (tenu) {
            cube = {paume.x + attache.x, paume.y + attache.y};   // posePaume * attache
            vitesseChute = 0.f;
            dejaSaisi = true;
        } else if (dejaSaisi) {
            vitesseChute += 6.f;                                  // l'objet tombe
            cube.y += vitesseChute;
            if (cube.y > SOL) { cube.y = SOL; vitesseChute = 0.f; }
        }

        if (image_n == IMAGE_CAPTURE) {
            Rect(0, 200, LARGEUR, 4, 90, 90, 90);                                        // sol
            Rect(static_cast<int>(paume.x) - 20, static_cast<int>(paume.y) - 6, 40, 12, 40, 90, 200);   // main
            Rect(static_cast<int>(cube.x) - 12, static_cast<int>(cube.y) - 12, 24, 24, 230, 120, 30);   // cube
            Rect(10, 10, 20, 20, tenu ? 60 : 200, tenu ? 180 : 60, 60);                // vert = tenu, rouge = lâché
        }
    }

    if (!EcrirePng(sortie)) {
        std::fprintf(stderr, "impossible d'ecrire %s\n", sortie.c_str());
        return 1;
    }
    return 0;
}