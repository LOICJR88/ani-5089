#include <iostream>
#include <string>

int main() {
    int p = 0;
    int f = 0;
    if (!(std::cin >> p >> f)) {
        std::cout << "PREMIER 0\n";
        return 0;
    }

    int compteur = 0;
    int premier = 0;

    for (int i = 1; i <= f; ++i) {
        std::string action;
        if (!(std::cin >> action)) {
            break;
        }

        if (action == "releve") {
            compteur = 0;
        } else if (action == "travaille") {
            ++compteur;
        }

        bool morte = compteur >= p;
        if (morte && premier == 0) {
            premier = i;
        }

        std::cout << compteur << " " << (morte ? "MORTE" : "VIVANTE") << "\n";
    }

    std::cout << "PREMIER " << premier << "\n";
    return 0;
}