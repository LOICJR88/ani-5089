#include <iostream>
#include <string>

int main() {
    unsigned int vol = 0;
    int n = 0;
    if (!(std::cin >> vol >> n)) {
        return 0;
    }

    unsigned int enVol = 0;
    int bloque = 0;

    for (int tour = 1; tour <= n; ++tour) {
        std::string commande;
        if (!(std::cin >> commande)) {
            break;
        }

        bool attend = false;

        if (commande == "SOUMET") {
            if (enVol >= vol) {
                attend = true;
            } else {
                ++enVol;
            }
        } else if (commande == "FINI") {
            if (enVol > 0) {
                --enVol;
            }
        }

        if (attend && bloque == 0) {
            bloque = tour;
        }

        std::cout << tour << " " << commande << " " << enVol << " "
                  << (attend ? "ATTEND" : "PASSE") << "\n";
    }

    std::cout << "BLOQUE " << bloque << "\n";
    return 0;
}