#include <iostream>

int main() {
    int n;
    if (!(std::cin >> n)) {
        return 1;
    }

    int precedent = 0;   // état de "courant" à l'image précédente (0 avant la première image)
    int fronts = 0;
    int niveaux = 0;

    for (int i = 1; i <= n; ++i) {
        int presse, lie;
        std::cin >> presse >> lie;

        int actif = lie;
        int courant = actif ? presse : 0;          // action inactive => courant faux
        int change = (courant != precedent) ? 1 : 0;

        if (courant == 1 && change == 1) {
            ++fronts;                              // front de saisie uniquement
        }
        if (courant == 1) {
            ++niveaux;                             // niveau : tenu, quel que soit change
        }

        std::cout << i << ' ' << courant << ' ' << change << ' ' << actif << '\n';
        precedent = courant;
    }

    std::cout << "FRONTS " << fronts << '\n';
    std::cout << "NIVEAUX " << niveaux << '\n';
    return 0;
}