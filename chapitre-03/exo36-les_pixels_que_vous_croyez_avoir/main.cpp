#include <iostream>

int main() {
    int n = 0;
    if (!(std::cin >> n)) {
        std::cout << "LISIBLE 0\n";
        return 0;
    }

    int lisibles = 0;

    for (int i = 0; i < n; ++i) {
        long long largeur = 0;
        long long hauteur = 0;
        long long echelle = 0;
        long long champ = 1;
        if (!(std::cin >> largeur >> hauteur >> echelle >> champ)) {
            break;
        }

        long long largeurReelle = largeur * echelle / 100;
        long long hauteurReelle = hauteur * echelle / 100;
        long long ppd = 0;
        if (champ > 0) {
            ppd = (largeurReelle + champ / 2) / champ;
        }

        if (ppd >= 15) {
            ++lisibles;
        }

        std::cout << largeurReelle << " " << hauteurReelle << " " << ppd << "\n";
    }

    std::cout << "LISIBLE " << lisibles << "\n";
    return 0;
}