#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

int main() {
    int s = 0;
    std::cin >> s;

    std::vector<int>       bits(s);
    std::vector<long long> couts(s);
    std::map<std::string, int> index;
    long long coutTout = 0;

    for (int i = 0; i < s; ++i) {
        std::string nom;
        std::cin >> nom >> bits[i] >> couts[i];
        index[nom] = i;
        coutTout += couts[i];
    }

    int r = 0;
    std::cin >> r;
    std::string reste;
    std::getline(std::cin, reste);

    long long plusPetit = -1;

    for (int i = 0; i < r; ++i) {
        std::string ligne;
        std::getline(std::cin, ligne);
        if (!ligne.empty() && ligne.back() == '\r') {
            ligne.pop_back();
        }

        std::vector<bool> allume(s, false);
        std::istringstream mots(ligne);
        std::string mot;
        while (mots >> mot) {
            if (mot == "tout") {
                for (int k = 0; k < s; ++k) allume[k] = true;
            } else {
                auto it = index.find(mot);
                if (it != index.end()) allume[it->second] = true;
            }
        }

        long long masque = 0, cout = 0;
        int nombre = 0;
        for (int k = 0; k < s; ++k) {
            if (allume[k]) {
                masque += 1LL << bits[k];
                cout   += couts[k];
                ++nombre;
            }
        }

        std::cout << masque << " " << cout << " " << nombre << "\n";
        if (plusPetit < 0 || cout < plusPetit) plusPetit = cout;
    }

    long long economie = (plusPetit < 0) ? 0 : coutTout - plusPetit;
    std::cout << "ECONOMIE " << economie << "\n";
    return 0;
}