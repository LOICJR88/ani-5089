#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

int main() {
    int p;
    std::cin >> p;
    std::cin.ignore();

    std::vector<std::pair<std::string, std::string>> prefixes;
    for (int i = 0; i < p; ++i) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string prefixe, module;
        iss >> prefixe >> module;
        prefixes.push_back({prefixe, module});
    }

    int l;
    std::cin >> l;
    std::cin.ignore();

    const std::string marqueur = "undefined reference to '";
    std::set<std::string> modules;
    int inconnu = 0;

    for (int i = 0; i < l; ++i) {
        std::string line;
        std::getline(std::cin, line);

        size_t pos = line.find(marqueur);
        if (pos == std::string::npos) {
            continue;
        }

        size_t debut = pos + marqueur.size();
        size_t fin = line.find('\'', debut);
        if (fin == std::string::npos) {
            continue;
        }

        std::string symbole = line.substr(debut, fin - debut);

        std::string meilleurPrefixe;
        std::string moduleTrouve;
        bool trouve = false;

        for (const auto& pm : prefixes) {
            const std::string& prefixe = pm.first;
            if (symbole.size() >= prefixe.size() &&
                symbole.compare(0, prefixe.size(), prefixe) == 0) {
                if (!trouve || prefixe.size() > meilleurPrefixe.size()) {
                    meilleurPrefixe = prefixe;
                    moduleTrouve = pm.second;
                    trouve = true;
                }
            }
        }

        if (trouve) {
            modules.insert(moduleTrouve);
        } else {
            inconnu++;
        }
    }

    for (const auto& module : modules) {
        std::cout << module << "\n";
    }

    if (inconnu > 0) {
        std::cout << "INCONNU " << inconnu << "\n";
    }

    return 0;
}
