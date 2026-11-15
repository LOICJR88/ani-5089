#include <iostream>
#include <map>
#include <string>

int main() {
    int a;
    if (!(std::cin >> a)) {
        return 1;
    }

    std::map<std::string, std::string> types;   
    int entrees = 0;
    int sorties = 0;

    for (int i = 0; i < a; ++i) {
        std::string nom, type;
        std::cin >> nom >> type;
        types[nom] = type;
        if (type == "HAPTIC") {
            ++sorties;
        } else {
            ++entrees;
        }
    }

    int o = 0;
    std::cin >> o;
    for (int i = 0; i < o; ++i) {
        std::string nom, operation;
        std::cin >> nom >> operation;

        auto it = types.find(nom);
        if (it == types.end()) {
            continue;   
        }

        const bool sortie = (it->second == "HAPTIC");
        if (sortie && operation == "LIT") {
            std::cout << nom << " LIT UNE SORTIE\n";
        } else if (!sortie && operation == "ECRIT") {
            std::cout << nom << " ECRIT UNE ENTREE\n";
        }
    }

    std::cout << "ENTREES " << entrees << '\n';
    std::cout << "SORTIES " << sorties << '\n';
    return 0;
}