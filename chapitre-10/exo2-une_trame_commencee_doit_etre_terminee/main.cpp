#include <iostream>
#include <string>

enum class Etat { Repos, Attendue, Commencee };

int main() {
    int n = 0;
    std::cin >> n;

    Etat etat = Etat::Repos;
    int fautes = 0;
    int bloque = 0;

    for (int i = 1; i <= n; ++i) {
        std::string geste;
        std::cin >> geste;

        std::string verdict = "OK";

        if (geste == "ATTEND") {
            if (etat == Etat::Repos) {
                etat = Etat::Attendue;
            } else if (etat == Etat::Attendue) {
                verdict = "DEJA ATTENDUE";
            } else {
                verdict = "TRAME EN COURS";
                if (bloque == 0) {
                    bloque = i;
                }
            }
        } else if (geste == "COMMENCE") {
            if (etat == Etat::Attendue) {
                etat = Etat::Commencee;
            } else if (etat == Etat::Repos) {
                verdict = "PAS ATTENDUE";
            } else {
                verdict = "DEJA COMMENCEE";
            }
        } else if (geste == "FINIT") {
            if (etat == Etat::Commencee) {
                etat = Etat::Repos;
            } else {
                verdict = "PAS COMMENCEE";
            }
        }

        if (verdict != "OK") {
            ++fautes;
        }

        std::cout << i << " " << geste << " " << verdict << "\n";
    }

    std::cout << "FAUTES " << fautes << "\n";
    std::cout << "BLOQUE " << bloque << "\n";
    return 0;
}