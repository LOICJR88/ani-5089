#include <iostream>
#include <string>

int main() {
    long long largeur = 0;
    long long hauteur = 0;
    int n = 0;
    if (!(std::cin >> largeur >> hauteur >> n)) {
        return 0;
    }

    int bloquantes = 0;

    for (int i = 0; i < n; ++i) {
        std::string nom;
        int cible = 0;
        int taille = 0;
        long long lc = 0;
        long long hc = 0;
        if (!(std::cin >> nom >> cible >> taille >> lc >> hc)) {
            break;
        }

        long long reelleL = largeur;
        long long reelleH = hauteur;
        const char* verdict = "PAS RELIEE";

        if (cible == 0) {
            verdict = "PAS RELIEE";
        } else if (taille == 1) {
            reelleL = lc;
            reelleH = hc;
            verdict = "OK";
        } else if (largeur != lc || hauteur != hc) {
            verdict = "BLOQUE";
            ++bloquantes;
        } else {
            verdict = "SANS EFFET";
        }

        std::cout << nom << " " << reelleL << " " << reelleH << " "
                  << verdict << "\n";
    }

    std::cout << "BLOQUANTES " << bloquantes << "\n";
    return 0;
}