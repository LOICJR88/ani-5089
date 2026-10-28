#include "NKWindow/Core/NkWindow.h"
#include "NKEvent/NkKeyboardEvent.h"

using namespace nkentseu;

namespace {

constexpr bool kRetenir = true;

constexpr bool kProteger = true;

bool gPris = false;

} 

void PointeurPrendre(NkWindow& fenetre) {
    if (kProteger && gPris) {
        return;
    }
    fenetre.ShowMouse(false);
    fenetre.CaptureMouse(true);
    if (kRetenir) {
        fenetre.ClipMouseToClient(true);
    }
    gPris = true;
}

void PointeurRendre(NkWindow& fenetre) {
    if (kProteger && !gPris) {
        return;
    }
    fenetre.ShowMouse(true);
    fenetre.CaptureMouse(false);
    fenetre.ClipMouseToClient(false);
    gPris = false;
}

void PointeurTouche(NkWindow& fenetre, NkKey touche) {
    if (touche == NkKey::NK_ESCAPE) {
        PointeurRendre(fenetre);
    } else if (touche == NkKey::NK_F1) {
        PointeurPrendre(fenetre);
    }
}