#include <iostream>
#include "actions.h"

static void Afficher(const EtatAction& e) {
    std::cout << e.courant << ' ' << e.change << ' ' << e.actif;
}

int main() {
    const int images = 6;

    const bool toucheE[images] = {false, true,  true,  true,  false, false};
    const bool toucheF[images] = {false, false, false, false, true,  false};
    const bool toucheG[images] = {false, false, true,  false, false, false};

    Action saisir(true);    
    Action tirer(true);    
    Action morte(false);  

    std::cout << "image | saisir c ch a | tirer c ch a | morte c ch a\n";

    for (int i = 0; i < images; ++i) {
        saisir.Synchroniser(toucheE[i]);
        tirer.Synchroniser(toucheF[i]);
        morte.Synchroniser(toucheG[i]);   

        std::cout << (i + 1) << " | ";
        Afficher(saisir.Etat());
        std::cout << " | ";
        Afficher(tirer.Etat());
        std::cout << " | ";
        Afficher(morte.Etat());
        std::cout << '\n';
    }
    return 0;
}