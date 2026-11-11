// Deux chaines d'echange, une par oeil, et un redimensionnement de fenetre.
//
// Banc de test autonome : pas de fenetre, pas de carte graphique, pas de casque.
// Une chaine d'echange est de la comptabilite, pas du dessin.
//
// Modes (argument en ligne de commande) :
//   (aucun)              chemin correct : detruire, recreer, reenregistrer
//   sans-reenregistrer   chaines recreees mais images jamais reenregistrees
//   sans-recreer         images redimensionnees mais chaines gardees telles quelles
// Seul le chemin correct ecrit trace.txt et resize.txt.

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

// ---------------------------------------------------------------------------
// Faux GPU : une table de textures. Le numero de ligne sert d'identifiant opaque.
// La ligne 0 reste invalide : l'identifiant zero ne designe jamais rien.
// ---------------------------------------------------------------------------
struct Texture {
    uint32 largeur;
    uint32 hauteur;
    int    oeil;     // 0 = gauche, 1 = droit : qui a le droit d'ecrire dedans
    uint32 trame;    // numero de l'image dessinee en dernier dedans
    bool   vivante;
};

static std::vector<Texture> gGpu(1, Texture{0, 0, -1, 0, false});

static uint64 CreerTexture(uint32 l, uint32 h, int oeil) {
    gGpu.push_back(Texture{l, h, oeil, 0, true});
    return static_cast<uint64>(gGpu.size() - 1);
}

static void DetruireTexture(uint64 id) {
    if (id != 0 && id < gGpu.size()) { gGpu[id].vivante = false; }
}

static bool TextureValide(uint64 id) {
    return id != 0 && id < gGpu.size() && gGpu[id].vivante;
}

// ---------------------------------------------------------------------------
// La chaine d'echange : tableau a taille fixe, identifiants 64 bits sans type,
// un booleen, deux retours false. Zero allocation.
// ---------------------------------------------------------------------------
class Chaine {
public:
    static constexpr uint32 kMaxImages = 4;

    // Refuse une chaine trop grande (ou vide).
    bool Create(const char* nom, uint32 nombreImages) {
        if (nombreImages == 0 || nombreImages > kMaxImages) { return false; }
        mNom      = nom;
        mCount    = nombreImages;
        mNext     = 0;
        mAcquired = false;
        mCreated  = true;
        for (uint32 i = 0; i < kMaxImages; ++i) { mImages[i] = 0; }
        return true;
    }

    void Destroy() {
        mCreated  = false;
        mAcquired = false;
        mCount    = 0;
        mNext     = 0;
        for (uint32 i = 0; i < kMaxImages; ++i) { mImages[i] = 0; }
    }

    // L'application fabrique les images, la chaine ne fait que les retenir.
    bool RegisterImage(uint32 index, uint64 id) {
        if (!mCreated || index >= mCount) { return false; }
        mImages[index] = id;
        return true;
    }

    // Temps 1 : emprunter. Refuse le double emprunt.
    bool AcquireImage(uint32& index) {
        if (!mCreated || mAcquired) { return false; }
        index     = mNext;
        mAcquired = true;
        return true;
    }

    // Temps 3 : rendre. Refuse de rendre sans avoir emprunte. Avance sur la suivante.
    bool ReleaseImage() {
        if (!mCreated || !mAcquired) { return false; }
        mAcquired = false;
        mNext     = (mNext + 1u) % mCount;
        return true;
    }

    // Temps 4 : on soumet la derniere image RELACHEE, pas celle obtenue a l'emprunt.
    uint32 GetLastReleasedIndex() const {
        return (mNext + mCount - 1u) % mCount;
    }

    uint64      GetImage(uint32 index) const { return index < kMaxImages ? mImages[index] : 0; }
    uint32      GetImageCount() const        { return mCount; }
    const char* GetName() const              { return mNom; }

private:
    const char* mNom                = "";
    uint64      mImages[kMaxImages] = {0, 0, 0, 0};
    uint32      mCount              = 0;
    uint32      mNext               = 0;
    bool        mAcquired           = false;
    bool        mCreated            = false;
};

// ---------------------------------------------------------------------------
// L'application
// ---------------------------------------------------------------------------
enum Oeil { GAUCHE = 0, DROIT = 1 };

static const char* kNomOeil[2]   = {"gauche", "droit"};
static const char* kNomChaine[2] = {"oeil gauche", "oeil droit"};
static const char* kAccord[2]    = {"gauche", "droite"};   // pour "la chaine ..."

static const uint32 kNombreImagesTotal = 30;   // images tracees
static const uint32 kImageDuResize     = 15;   // le redimensionnement arrive apres cette image
static const uint32 kImagesParChaine   = 2;    // comme les vrais runtimes : deux ou trois

static uint32 gLargeur = 1280;
static uint32 gHauteur = 720;
static int    gErreurs = 0;

static Chaine gChaine[2];

// Cree la chaine d'un oeil et, si demande, ses textures + leur enregistrement.
// enregistrer = false sert a reproduire l'erreur classique, ou a separer les etapes.
static bool MonterOeil(int oeil, bool enregistrer) {
    if (!gChaine[oeil].Create(kNomChaine[oeil], kImagesParChaine)) { return false; }
    if (!enregistrer) { return true; }
    for (uint32 i = 0; i < kImagesParChaine; ++i) {
        gChaine[oeil].RegisterImage(i, CreerTexture(gLargeur, gHauteur, oeil));
    }
    return true;
}

// Les quatre temps pour un oeil. Rend l'index emprunte (pour la trace).
static bool UneImagePourUnOeil(int oeil, uint32 image, uint32& indexEmprunte) {
    Chaine& c = gChaine[oeil];

    // Temps 1 : emprunter
    if (!c.AcquireImage(indexEmprunte)) {
        std::fprintf(stderr, "erreur : double emprunt sur la chaine %s\n", c.GetName());
        ++gErreurs;
        return false;
    }

    // Temps 2 : dessiner, dans l'image empruntee et nulle part ailleurs
    const uint64 cible = c.GetImage(indexEmprunte);
    if (TextureValide(cible)) {
        gGpu[cible].trame = image;   // le "dessin" : on y ecrit le numero de l'image
    }                                // sinon : silence, comme un vrai rendu hors ecran

    // Temps 3 : rendre
    if (!c.ReleaseImage()) {
        std::fprintf(stderr, "erreur : rendu sans emprunt sur la chaine %s\n", c.GetName());
        ++gErreurs;
        return false;
    }

    // Temps 4 : soumettre la derniere image relachee
    const uint32 aSoumettre = c.GetLastReleasedIndex();
    const uint64 id         = c.GetImage(aSoumettre);

    if (id == 0) {
        std::fprintf(stderr, "erreur : image %u, oeil %s : on soumet l'identifiant zero\n",
                     image, kNomOeil[oeil]);
        ++gErreurs;
    } else if (!TextureValide(id)) {
        std::fprintf(stderr, "erreur : image %u, oeil %s : identifiant %llu detruit\n",
                     image, kNomOeil[oeil], static_cast<unsigned long long>(id));
        ++gErreurs;
    } else if (gGpu[id].oeil != oeil) {
        std::fprintf(stderr, "erreur : image %u, oeil %s : l'image appartient a l'autre oeil\n",
                     image, kNomOeil[oeil]);
        ++gErreurs;
    } else if (gGpu[id].largeur != gLargeur || gGpu[id].hauteur != gHauteur) {
        std::fprintf(stderr, "erreur : image %u, oeil %s : taille %ux%u, fenetre %ux%u\n",
                     image, kNomOeil[oeil], gGpu[id].largeur, gGpu[id].hauteur,
                     gLargeur, gHauteur);
        ++gErreurs;
    } else if (gGpu[id].trame != image) {
        std::fprintf(stderr, "erreur : image %u, oeil %s : on soumet la trame %u\n",
                     image, kNomOeil[oeil], gGpu[id].trame);
        ++gErreurs;
    }
    return true;
}

// Ecrit une ligne dans resize.txt (si le fichier est ouvert) et a l'ecran.
static void Journal(std::FILE* f, const char* format, ...) {
    char ligne[256];
    va_list args;
    va_start(args, format);
    std::vsnprintf(ligne, sizeof(ligne), format, args);
    va_end(args);
    std::printf("%s\n", ligne);
    if (f) { std::fprintf(f, "%s\n", ligne); }
}

static void Redimensionner(uint32 nouvelleLargeur, uint32 nouvelleHauteur,
                           bool recreer, bool reenregistrer, std::FILE* resize) {
    const uint32 ancienneLargeur = gLargeur;
    const uint32 ancienneHauteur = gHauteur;
    gLargeur = nouvelleLargeur;
    gHauteur = nouvelleHauteur;

    if (!recreer) {
        // Mauvais chemin : on refait les textures a la nouvelle taille,
        // mais les chaines gardent leurs anciens identifiants.
        for (int oeil = 0; oeil < 2; ++oeil) {
            for (uint32 i = 0; i < kImagesParChaine; ++i) {
                DetruireTexture(gChaine[oeil].GetImage(i));
                CreerTexture(gLargeur, gHauteur, oeil);   // jamais communique a la chaine
            }
        }
        return;
    }

    // 1) detruire les deux chaines (et leurs anciennes images)
    for (int oeil = 0; oeil < 2; ++oeil) {
        for (uint32 i = 0; i < kImagesParChaine; ++i) {
            DetruireTexture(gChaine[oeil].GetImage(i));
        }
        gChaine[oeil].Destroy();
        Journal(resize, "detruire la chaine %s (%ux%u)", kAccord[oeil],
                ancienneLargeur, ancienneHauteur);
    }

    // 2) recreer les deux chaines
    for (int oeil = 0; oeil < 2; ++oeil) {
        MonterOeil(oeil, false);   // chaine neuve : tableau plein de zeros
        Journal(resize, "recreer la chaine %s (%u images, %ux%u)", kAccord[oeil],
                kImagesParChaine, gLargeur, gHauteur);
    }

    // 3) reenregistrer les images
    if (reenregistrer) {
        for (int oeil = 0; oeil < 2; ++oeil) {
            for (uint32 i = 0; i < kImagesParChaine; ++i) {
                gChaine[oeil].RegisterImage(i, CreerTexture(gLargeur, gHauteur, oeil));
            }
            Journal(resize, "reenregistrer les %u images de la chaine %s", kImagesParChaine,
                    kAccord[oeil]);
        }
    }
}

int main(int argc, char** argv) {
    bool recreer       = true;
    bool reenregistrer = true;
    if (argc > 1 && std::strcmp(argv[1], "sans-reenregistrer") == 0) { reenregistrer = false; }
    if (argc > 1 && std::strcmp(argv[1], "sans-recreer") == 0)       { recreer = false; }
    const bool cheminCorrect = recreer && reenregistrer;

    std::FILE* trace  = cheminCorrect ? std::fopen("trace.txt", "w")  : nullptr;
    std::FILE* resize = cheminCorrect ? std::fopen("resize.txt", "w") : nullptr;
    if (cheminCorrect && (!trace || !resize)) {
        std::fprintf(stderr, "impossible d'ouvrir trace.txt ou resize.txt\n");
        return 1;
    }

    // Deux chaines independantes, nommees : le nom sert aux outils de mise au point.
    for (int oeil = 0; oeil < 2; ++oeil) {
        if (!MonterOeil(oeil, true)) {
            std::fprintf(stderr, "creation de la chaine %s refusee\n", kNomOeil[oeil]);
            return 1;
        }
    }

    for (uint32 image = 1; image <= kNombreImagesTotal; ++image) {
        uint32 indexGauche = 0;
        uint32 indexDroit  = 0;
        UneImagePourUnOeil(GAUCHE, image, indexGauche);
        UneImagePourUnOeil(DROIT,  image, indexDroit);

        if (trace) {
            std::fprintf(trace, "image : %u, gauche : %u, droit : %u\n",
                         image, indexGauche, indexDroit);
        }

        if (image == kImageDuResize) {
            std::printf("--- redimensionnement de la fenetre : 1280x720 -> 1600x900 ---\n");
            Redimensionner(1600, 900, recreer, reenregistrer, resize);
        }
    }

    if (trace)  { std::fclose(trace); }
    if (resize) { std::fclose(resize); }

    std::printf("images tracees : %u, erreurs de soumission : %d\n", kNombreImagesTotal, gErreurs);
    return gErreurs == 0 ? 0 : 2;
}