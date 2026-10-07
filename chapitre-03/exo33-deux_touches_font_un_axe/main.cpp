#include <iostream>
#include <string>
#include <vector>

int main() {
    int c = 0;
    if (!(std::cin >> c)) {
        return 0;
    }

    std::vector<long long> echelles(c, 0);
    std::vector<long long> seuils(c, 0);

    for (int i = 0; i < c; ++i) {
        std::string nom;
        std::cin >> nom >> echelles[i] >> seuils[i];
    }

    int t = 0;
    if (!(std::cin >> t)) {
        return 0;
    }

    for (int i = 0; i < t; ++i) {
        long long somme = 0;
        for (int j = 0; j < c; ++j) {
            long long brute = 0;
            std::cin >> brute;
            long long contribution = brute * echelles[j] / 1000;
            long long absolue = contribution < 0 ? -contribution : contribution;
            if (absolue >= seuils[j]) {
                somme += contribution;
            }
        }
        std::cout << somme << "\n";
    }

    return 0;
}