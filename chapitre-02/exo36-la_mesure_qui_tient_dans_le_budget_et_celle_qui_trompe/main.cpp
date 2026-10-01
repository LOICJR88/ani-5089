#include <iostream>
#include <string>

int main() {
    long long budget;
    std::cin >> budget;

    int s;
    std::cin >> s;

    int trompe = 0;

    for (int i = 0; i < s; ++i) {
        std::string nom;
        long long debug, release;
        std::cin >> nom >> debug >> release;

        long long facteur = (debug + release / 2) / release;

        bool tient = (release <= budget);
        bool debugDepasse = (debug > budget);

        std::cout << nom << " " << facteur << " " << (tient ? "TIENT" : "DEPASSE") << "\n";

        if (debugDepasse && tient) {
            trompe++;
        }
    }

    std::cout << "TROMPE " << trompe << "\n";

    return 0;
}
