// =============================================================================
//  Exercice 10 -- la projection decentree, ecrite a la main.
//
//  Une projection decentree n'est pas une autre formule que celle d'un jeu :
//  c'est la meme, avec deux termes (mat[2][0] et mat[2][1]) qui cessent d'etre
//  nuls. Ce fichier l'ecrit a partir de la matrice NULLE, puis la compare :
//    1. a NkMat4f::Perspective pour un champ symetrique ;
//    2. a la camera du moteur (SetFovAsym) pour l'oeil gauche d'un Quest 2.
//
//  NkMat4f range ses coefficients par colonne : mat[colonne][ligne].
//  Les angles sont en RADIANS et signes : gauche et bas sont negatifs.
// =============================================================================
#include <cmath>
#include <cstdio>

#include "NKMath/NkMat.h"
#include "NKRenderer/Core/NkCamera.h"

using namespace nkentseu;
using namespace nkentseu::renderer;
using namespace nkentseu::math;

namespace {

constexpr float32 kPi = 3.14159265358979f;

float32 EnRadians(float32 degres) { return degres * kPi / 180.f; }

// La matrice decentree, ecrite a la main. Rien du moteur n'est appele ici.
NkMat4f Projection(float32 gauche, float32 droite, float32 haut, float32 bas,
                   float32 nearPlane, float32 farPlane) {
    const float32 tanLeft = std::tan(gauche);
    const float32 tanRight = std::tan(droite);
    const float32 tanUp = std::tan(haut);
    const float32 tanDown = std::tan(bas);
    const float32 tanWidth = tanRight - tanLeft;
    const float32 tanHeight = tanUp - tanDown;

    NkMat4f result; // la matrice NULLE, pas l'identite
    result.mat[0][0] = 2.f / tanWidth;
    result.mat[1][1] = 2.f / tanHeight;
    result.mat[2][0] = (tanRight + tanLeft) / tanWidth;   // decentrage horizontal
    result.mat[2][1] = (tanUp + tanDown) / tanHeight;     // decentrage vertical
    result.mat[2][3] = -1.f;
    // La profondeur, entre -1 et 1 : les memes coefficients que Perspective.
    result.mat[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    result.mat[3][2] = 2.f * farPlane * nearPlane / (nearPlane - farPlane);
    return result;
}

// Affiche une matrice, ligne par ligne : la ligne r montre mat[0][r] a mat[3][r].
void Imprimer(const char *nom, const NkMat4f &m) {
    std::printf("[projection] %s\n", nom);
    for (int ligne = 0; ligne < 4; ++ligne) {
        std::printf("  ligne %d : %10.6f %10.6f %10.6f %10.6f\n", ligne,
                    (double)m.mat[0][ligne], (double)m.mat[1][ligne],
                    (double)m.mat[2][ligne], (double)m.mat[3][ligne]);
    }
}

// Ecrit les quatre lignes d'ecarts d'une epreuve, puis son plus grand ecart.
// Ligne r : les ecarts des colonnes 0 a 3, |a.mat[c][r] - b.mat[c][r]|.
void Comparer(const char *nom, const NkMat4f &a, const NkMat4f &b, std::FILE *fichier) {
    float32 plusGrand = 0.f;
    for (int ligne = 0; ligne < 4; ++ligne) {
        float32 e[4];
        for (int colonne = 0; colonne < 4; ++colonne) {
            e[colonne] = std::fabs(a.mat[colonne][ligne] - b.mat[colonne][ligne]);
            if (e[colonne] > plusGrand) plusGrand = e[colonne];
        }
        std::printf("%s, ligne %d : %.3e %.3e %.3e %.3e\n", nom, ligne,
                    (double)e[0], (double)e[1], (double)e[2], (double)e[3]);
        if (fichier != nullptr) {
            std::fprintf(fichier, "%s, ligne %d : %.3e %.3e %.3e %.3e\n", nom, ligne,
                         (double)e[0], (double)e[1], (double)e[2], (double)e[3]);
        }
    }
    std::printf("%s : plus grand ecart %.3e\n", nom, (double)plusGrand);
    if (fichier != nullptr) {
        std::fprintf(fichier, "%s : plus grand ecart %.3e\n", nom, (double)plusGrand);
    }
}

// Projette le point (x, y, z) par la matrice m, et rend ses coordonnees
// normalisees (NDC). Rangement par colonne : clip_ligne = somme sur c de
// m.mat[c][ligne] * v[c], avec v = (x, y, z, 1), puis division par w.
void ProjeterNdc(const NkMat4f &m, float32 x, float32 y, float32 z,
                 float32 &ndcX, float32 &ndcY) {
    const float32 cx = m.mat[0][0] * x + m.mat[1][0] * y + m.mat[2][0] * z + m.mat[3][0];
    const float32 cy = m.mat[0][1] * x + m.mat[1][1] * y + m.mat[2][1] * z + m.mat[3][1];
    const float32 cw = m.mat[0][3] * x + m.mat[1][3] * y + m.mat[2][3] * z + m.mat[3][3];
    ndcX = cx / cw;
    ndcY = cy / cw;
}

} // namespace

// Appelee au demarrage de la salle, avant l'ouverture de la fenetre.
void CalculerLesProjections() {
    const float32 nearPlane = 0.05f;
    const float32 farPlane = 100.f;

    std::FILE *fichier = std::fopen("ecarts.txt", "w");

    // ---- Epreuve symetrique : 70 degres verticalement, rapport 1280/720 ----
    // Demi-champ vertical : 35 degres. La demi-largeur du champ se deduit du
    // rapport : tan(demi-largeur) = tan(demi-hauteur) * rapport.
    const float32 rapport = 1280.f / 720.f;
    const float32 demiHauteur = EnRadians(70.f) * 0.5f;
    const float32 demiLargeur = std::atan(std::tan(demiHauteur) * rapport);

    const float32 gaucheS = -demiLargeur;
    const float32 droiteS = demiLargeur;
    const float32 hautS = demiHauteur;
    const float32 basS = -demiHauteur;

    std::printf("[projection] champ symetrique : gauche %.4f droite %.4f haut %.4f bas %.4f (rad)\n",
                (double)gaucheS, (double)droiteS, (double)hautS, (double)basS);

    const NkMat4f maSymetrique = Projection(gaucheS, droiteS, hautS, basS, nearPlane, farPlane);
    const NkMat4f moteurSymetrique =
        NkMat4f::Perspective(NkAngle(70.f), rapport, nearPlane, farPlane);

    Imprimer("ma matrice symetrique", maSymetrique);
    Imprimer("Perspective du moteur", moteurSymetrique);
    Comparer("symetrique", maSymetrique, moteurSymetrique, fichier);

    // ---- Epreuve quest : l'oeil gauche d'un Quest 2, releve sur un vrai casque ----
    const float32 gaucheQ = EnRadians(-52.f);
    const float32 droiteQ = EnRadians(45.f);
    const float32 hautQ = EnRadians(48.f);
    const float32 basQ = EnRadians(-50.f);

    const NkMat4f maQuest = Projection(gaucheQ, droiteQ, hautQ, basQ, nearPlane, farPlane);

    NkCamera3D cam;
    cam.SetNearFar(nearPlane, farPlane);
    cam.SetFovAsym(gaucheQ, droiteQ, hautQ, basQ);
    const NkMat4f moteurQuest = cam.GetProj();

    Imprimer("ma matrice quest", maQuest);
    Imprimer("camera du moteur (quest)", moteurQuest);
    Comparer("quest", maQuest, moteurQuest, fichier);

    // Verification de plus : les quatre bords du champ doivent tomber a +/-1,
    // et l'axe de visee (0, 0, -1) doit tomber hors du centre de l'image.
    {
        const float32 tg = std::tan(gaucheQ);
        const float32 td = std::tan(droiteQ);
        const float32 th = std::tan(hautQ);
        const float32 tb = std::tan(basQ);
        float32 nx = 0.f, ny = 0.f;

        ProjeterNdc(maQuest, tg, 0.f, -1.f, nx, ny);
        std::printf("[verif] bord gauche  : ndc x = %.6f (attendu -1)\n", (double)nx);
        ProjeterNdc(maQuest, td, 0.f, -1.f, nx, ny);
        std::printf("[verif] bord droit   : ndc x = %.6f (attendu +1)\n", (double)nx);
        ProjeterNdc(maQuest, 0.f, th, -1.f, nx, ny);
        std::printf("[verif] bord haut    : ndc y = %.6f (attendu +1)\n", (double)ny);
        ProjeterNdc(maQuest, 0.f, tb, -1.f, nx, ny);
        std::printf("[verif] bord bas     : ndc y = %.6f (attendu -1)\n", (double)ny);
        ProjeterNdc(maQuest, 0.f, 0.f, -1.f, nx, ny);
        std::printf("[verif] axe de visee : ndc x = %.6f, ndc y = %.6f (attendu 0.1228, 0.0352)\n",
                    (double)nx, (double)ny);
    }

    if (fichier != nullptr) {
        std::fclose(fichier);
        std::printf("[projection] ecarts.txt ecrit\n");
    } else {
        std::printf("[projection] ecarts.txt n'a pas pu etre ecrit\n");
    }
}