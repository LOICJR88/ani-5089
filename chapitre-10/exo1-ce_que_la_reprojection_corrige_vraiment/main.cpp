#include <cstdlib>
#include <iostream>

int main() {
    int n = 0;
    std::cin >> n;

    int justes = 0;
    int pires = 0;

    for (int i = 0; i < n; ++i) {
        int rendu = 0;
        int reel = 0;
        int soumis = 0;
        std::cin >> rendu >> reel >> soumis;

        const int sans = rendu - reel;
        const int avec = rendu - soumis;

        const char* verdict = nullptr;
        if (avec == 0) {
            verdict = "JUSTE";
            ++justes;
        } else if (std::abs(avec) < std::abs(sans)) {
            verdict = "PARTIEL";
        } else if (std::abs(avec) == std::abs(sans)) {
            verdict = "SANS EFFET";
        } else {
            verdict = "PIRE";
            ++pires;
        }

        std::cout << "SANS " << sans << " AVEC " << avec << " " << verdict << "\n";
    }

    std::cout << "JUSTES " << justes << "\n";
    std::cout << "PIRES " << pires << "\n";
    return 0;
}