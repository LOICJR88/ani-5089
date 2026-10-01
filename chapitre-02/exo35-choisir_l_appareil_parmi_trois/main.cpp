#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

struct Appareil {
    std::string serie;
    std::string etat;
    std::string modele;
};

int main() {
    int d;
    std::cin >> d;
    std::cin.ignore();

    std::vector<Appareil> appareils;
    for (int i = 0; i < d; ++i) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        Appareil a;
        iss >> a.serie >> a.etat >> a.modele;
        appareils.push_back(a);
    }

    std::string cible;
    std::getline(std::cin, cible);

    if (cible != "-") {
        const Appareil* trouve = nullptr;
        for (const auto& a : appareils) {
            if (a.serie == cible) {
                trouve = &a;
                break;
            }
        }

        if (trouve == nullptr) {
            std::cout << "ERREUR cible introuvable\n";
        } else if (trouve->etat != "device") {
            std::cout << "ERREUR " << trouve->serie << " est " << trouve->etat << "\n";
        } else {
            std::cout << trouve->serie << "\n";
        }
        return 0;
    }

    std::vector<std::string> prets;
    for (const auto& a : appareils) {
        if (a.etat == "device") {
            prets.push_back(a.serie);
        }
    }

    if (prets.empty()) {
        std::cout << "ERREUR aucun appareil\n";
    } else if (prets.size() == 1) {
        std::cout << prets[0] << "\n";
    } else {
        std::sort(prets.begin(), prets.end());
        std::cout << "ERREUR plusieurs appareils\n";
        for (const auto& s : prets) {
            std::cout << s << "\n";
        }
    }

    return 0;
}
