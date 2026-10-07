// Exercice 9 : le defaut de l'accumulateur de souris, reproduit puis corrige.
#include <stdlib.h>
#include <chrono>
#include <fstream>
#include <thread>
#include <utility>
#include <vector>
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWESystem.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkMouseEvent.h"

using namespace nkentseu;

struct AccumulateurSouris {
    int dx = 0;
    int dy = 0;

    void Ajouter(int ex, int ey) {
        dx += ex;
        dy += ey;
    }

    void Consommer(int& sx, int& sy) {
        sx = dx;
        sy = dy;
        dx = 0;
        dy = 0;
    }
};

int main() {
    NkWindowConfig config;
    config.title  = "Accumulateur";
    config.width  = 1280;
    config.height = 720;

    NkWindow fenetre(config);
    if (!fenetre.IsValid()) {
        return 1;
    }

    AccumulateurSouris acc;

    auto garde = NkEvents().AddEventCallbackGuard<NkMouseRawEvent>(
        [&acc](NkMouseRawEvent* e) {
            acc.Ajouter(static_cast<int>(e->GetDeltaX()), static_cast<int>(e->GetDeltaY()));
        });

    std::vector<std::pair<int, int>> serieAvant, serieApres;
    
    const auto depart = std::chrono::steady_clock::now();
    const auto duree  = std::chrono::seconds(10);

    while (fenetre.IsOpen() && (std::chrono::steady_clock::now() - depart) < duree) {
        NkEvents().PollEvents();

        const auto& souris = NkEvents().GetInputState().mouse;
        serieAvant.push_back({souris.rawDeltaX, souris.rawDeltaY});

        int ax = 0, ay = 0;
        acc.Consommer(ax, ay);
        serieApres.push_back({ax, ay});

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    int k = -1;
    for (int i = static_cast<int>(serieApres.size()) - 1; i >= 0; --i) {
        if (serieApres[i].first != 0 || serieApres[i].second != 0) { k = i; break; }
    }
    if (k < 0) {
        return 2;   // aucun mouvement detecte : aucun fichier ecrit
    }
    int debut = k - 3;
    if (debut < 0) debut = 0;
    int fin = debut + 10;
    if (fin > static_cast<int>(serieApres.size())) {
        fin = static_cast<int>(serieApres.size());
        debut = fin - 10 < 0 ? 0 : fin - 10;
    }

    std::ofstream fa("avant.txt"), fp("apres.txt");
    for (int i = debut; i < fin; ++i) {
        fa << serieAvant[i].first << " " << serieAvant[i].second << "\n";
        fp << serieApres[i].first << " " << serieApres[i].second << "\n";
    }
    return 0;
}