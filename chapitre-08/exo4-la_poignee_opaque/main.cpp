#include <iostream>
#include <map>
#include <string>

int main() {
    long long images = 0;
    int n = 0;
    if (!(std::cin >> images >> n)) {
        return 0;
    }

    std::map<long long, unsigned long long> poignees;

    for (int i = 0; i < n; ++i) {
        std::string commande;
        if (!(std::cin >> commande)) {
            break;
        }

        if (commande == "POSE") {
            long long index = 0;
            unsigned long long valeur = 0;
            if (!(std::cin >> index >> valeur)) {
                break;
            }
            if (index < 0 || index >= images) {
                std::cout << "HORS ANNEAU\n";
            } else {
                poignees[index] = valeur;
            }
        } else if (commande == "LIT") {
            long long index = 0;
            if (!(std::cin >> index)) {
                break;
            }
            std::map<long long, unsigned long long>::const_iterator it =
                poignees.find(index);
            if (index < 0 || index >= images || it == poignees.end()) {
                std::cout << "VIDE\n";
            } else {
                std::cout << it->second << "\n";
            }
        }
    }

    std::cout << "POSEES " << poignees.size() << "\n";
    return 0;
}