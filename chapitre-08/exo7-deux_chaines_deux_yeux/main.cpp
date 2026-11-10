#include <iostream>
#include <string>

struct Anneau {
    unsigned int suivant = 0;
    bool emprunte = false;
    int rendus = 0;
};

int main() {
    unsigned int images = 0;
    int n = 0;
    if (!(std::cin >> images >> n)) {
        return 0;
    }

    Anneau yeux[2];
    int desync = 0;

    for (int tour = 1; tour <= n; ++tour) {
        std::string oeil;
        std::string commande;
        if (!(std::cin >> oeil >> commande)) {
            break;
        }

        Anneau& a = (oeil == "G") ? yeux[0] : yeux[1];

        std::cout << tour << " " << oeil << " " << commande << " ";

        if (commande == "EMPRUNTE") {
            if (a.emprunte || images == 0) {
                std::cout << "REFUSE\n";
            } else {
                a.emprunte = true;
                std::cout << a.suivant << "\n";
            }
        } else {
            if (!a.emprunte) {
                std::cout << "REFUSE\n";
            } else {
                unsigned int rendue = a.suivant;
                a.emprunte = false;
                a.suivant = (a.suivant + 1u) % images;
                ++a.rendus;
                std::cout << rendue << "\n";
            }
        }

        int diff = yeux[0].rendus - yeux[1].rendus;
        if (diff < 0) {
            diff = -diff;
        }
        if (diff > 1 && desync == 0) {
            desync = tour;
        }
    }

    int decalage = yeux[0].rendus - yeux[1].rendus;
    if (decalage < 0) {
        decalage = -decalage;
    }

    std::cout << "DECALAGE " << decalage << "\n";
    std::cout << "DESYNC " << desync << "\n";
    return 0;
}