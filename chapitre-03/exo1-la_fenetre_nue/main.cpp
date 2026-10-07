// Exercice 1 : la fenetre nue
#include <stdlib.h>
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWESystem.h"

using namespace nkentseu;

int main() {
    NkWindowConfig config;
    config.title  = "Ma salle";
    config.width  = 1280;
    config.height = 720;

    NkWindow fenetre(config);
    
    if (!fenetre.IsValid()) {
        return 1;
    }
    
    while (fenetre.IsOpen()) {
        NkEvents().PollEvents();
    }
    return 0;
}
