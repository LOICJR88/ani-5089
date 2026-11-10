#include <iostream>
#include <sstream>
#include <string>

int main() {
    int n = 0;
    if (!(std::cin >> n)) {
        return 0;
    }

    std::string ligne;
    std::getline(std::cin, ligne);

    int survivants = 0;
    int lues = 0;

    while (lues < n && std::getline(std::cin, ligne)) {
        while (!ligne.empty() && (ligne.back() == '\r' || ligne.back() == ' ')) {
            ligne.pop_back();
        }
        if (ligne.empty()) {
            continue;
        }
        ++lues;

        std::istringstream flux(ligne);
        std::string nom;
        flux >> nom;

        int posDetruit = -1;
        int posRecree = -1;
        bool reenregistre = false;
        int rang = 0;

        std::string mot;
        while (flux >> mot) {
            if (mot == "DETRUIT" && posDetruit < 0) {
                posDetruit = rang;
            } else if (mot == "RECREE" && posRecree < 0) {
                posRecree = rang;
            } else if (mot == "REENREGISTRE") {
                reenregistre = true;
            }
            ++rang;
        }

        const char* verdict = "SURVIT";
        if (posDetruit >= 0 && posRecree >= 0 && posRecree < posDetruit) {
            verdict = "MAUVAIS ORDRE";
        } else if (posDetruit < 0 || posRecree < 0) {
            verdict = "CHAINE PERIMEE";
        } else if (!reenregistre) {
            verdict = "POIGNEES PERDUES";
        } else {
            ++survivants;
        }

        std::cout << nom << " " << verdict << "\n";
    }

    std::cout << "SURVIVANTS " << survivants << "\n";
    return 0;
}