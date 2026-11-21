#include <iostream>

#include "couches.h"

int FinTrame(const CoucheProjection* projection) {
    return (projection != nullptr) ? 1 : 0;
}

int main() {
    const int nombreImages = 3;
    const int nombreTrames = 6;

    int index = 0;  
    int fins = 0;   

    for (int trame = 1; trame <= nombreTrames; ++trame) {
        const bool onRend = (trame != 3 && trame != 4);
        const float lacet = static_cast<float>(trame);  

        CoucheProjection couche;
        const CoucheProjection* soumise = nullptr;
        int imageEmployee = -1;

        if (onRend) {
            imageEmployee = index;
            index = (index + 1) % nombreImages; 

            couche.espace = Espace::Stage;
            couche.vues[0] = Vue{imageEmployee, lacet};
            couche.vues[1] = Vue{imageEmployee, lacet};
            soumise = &couche;
        }

        const int couchesSoumises = FinTrame(soumise);
        ++fins;

        std::cout << trame << " " << (onRend ? "OUI" : "NON") << " "
                  << couchesSoumises << " ";
        if (onRend) {
            std::cout << imageEmployee;
        } else {
            std::cout << "-";
        }
        std::cout << "\n";
    }

    return (fins == nombreTrames) ? 0 : 1;
}