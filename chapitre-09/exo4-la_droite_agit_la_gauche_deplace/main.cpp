#include <iostream>
#include <set>
#include <sstream>
#include <string>

int main() {
    std::string ligne;
    if (!std::getline(std::cin, ligne)) {
        return 1;
    }
    int n = std::stoi(ligne);

    const std::set<std::string> gauche = {
        "SELECT_LEFT", "GRAB_LEFT", "AIM_POSE_LEFT",
        "GRIP_POSE_LEFT", "HAPTIC_LEFT", "MENU", "MOVE"
    };
    const std::set<std::string> droite = {
        "SELECT", "GRAB", "AIM_POSE", "GRIP_POSE", "HAPTIC", "MOVE_RIGHT"
    };

    int deuxMains = 0;

    for (int i = 0; i < n; ++i) {
        if (!std::getline(std::cin, ligne)) {
            break;
        }
        if (!ligne.empty() && ligne.back() == '\r') {
            ligne.pop_back();   
        }

        std::istringstream flux(ligne);
        std::string nom;
        flux >> nom;

        int nbDroite = 0;
        int nbGauche = 0;
        std::string usage;
        while (flux >> usage) {
            if (gauche.count(usage) > 0) {
                ++nbGauche;
            } else if (droite.count(usage) > 0) {
                ++nbDroite;
            }
        }

        const bool deux = (nbDroite > 0 && nbGauche > 0);
        if (deux) {
            ++deuxMains;
        }

        std::cout << nom << ' ' << nbDroite << ' ' << nbGauche << ' '
                  << (deux ? "DEUX MAINS" : "UNE MAIN") << '\n';
    }

    std::cout << "DEUX MAINS " << deuxMains << '\n';
    return 0;
}