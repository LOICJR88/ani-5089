#include <iostream>

int main() {
    int n;
    if (!(std::cin >> n)) {
        return 1;
    }

    int precedent = 0;  
    int fronts = 0;
    int niveaux = 0;

    for (int i = 1; i <= n; ++i) {
        int presse, lie;
        std::cin >> presse >> lie;

        int actif = lie;
        int courant = actif ? presse : 0;  
        int change = (courant != precedent) ? 1 : 0;

        if (courant == 1 && change == 1) {
            ++fronts;                          
        }
        if (courant == 1) {
            ++niveaux;                        
        }

        std::cout << i << ' ' << courant << ' ' << change << ' ' << actif << '\n';
        precedent = courant;
    }

    std::cout << "FRONTS " << fronts << '\n';
    std::cout << "NIVEAUX " << niveaux << '\n';
    return 0;
}
