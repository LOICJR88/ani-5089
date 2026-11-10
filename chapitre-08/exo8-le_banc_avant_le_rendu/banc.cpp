#include <iostream>
#include "chaine.h"

static void Verdict(const char* nom, bool accepte) {
    std::cout << nom << " : " << (accepte ? "ACCEPTE" : "REFUSE") << "\n";
}

int main() {
    ChaineEchange chaine;
    if (!chaine.Creer(3u)) {
        return 1;
    }

    for (int tour = 1; tour <= 6; ++tour) {
        std::uint32_t index = 0u;
        if (chaine.Emprunter(index)) {
            std::cout << "tour " << tour << " EMPRUNTE " << index << "\n";
        }
        if (chaine.Rendre()) {
            std::cout << "tour " << tour << " REND " << index << "\n";
        }
    }
    std::cout << "derniere rendue : " << chaine.DernierRendu() << "\n";

    std::uint32_t index = 0u;
    Verdict("emprunter", chaine.Emprunter(index));
    Verdict("emprunter deux fois", chaine.Emprunter(index));
    chaine.Rendre();
    Verdict("rendre sans avoir emprunte", chaine.Rendre());

    ChaineEchange trop;
    Verdict("creer 5 images", trop.Creer(5u));

    return 0;
}