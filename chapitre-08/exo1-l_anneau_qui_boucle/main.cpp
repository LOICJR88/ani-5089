#include <iostream>
#include <string>

int main() {
    unsigned int images = 0;
    int n = 0;
    if (!(std::cin >> images >> n)) {
        return 0;
    }

    unsigned int suivant = 0;
    bool emprunte = false;
    int prets = 0;

    for (int tour = 1; tour <= n; ++tour) {
        std::string commande;
        if (!(std::cin >> commande)) {
            break;
        }

        if (commande == "EMPRUNTE") {
            if (emprunte || images == 0) {
                std::cout << tour << " EMPRUNTE REFUSE\n";
            } else {
                emprunte = true;
                ++prets;
                std::cout << tour << " EMPRUNTE " << suivant << "\n";
            }
        } else if (commande == "REND") {
            if (!emprunte) {
                std::cout << tour << " REND REFUSE\n";
            } else {
                unsigned int rendue = suivant;
                emprunte = false;
                suivant = (suivant + 1u) % images;
                std::cout << tour << " REND " << rendue << "\n";
            }
        }
    }

    std::cout << "PRETS " << prets << "\n";
    return 0;
}