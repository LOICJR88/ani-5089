#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>

int main() {
    int n;
    std::cin >> n;
    std::cin.ignore();

    std::map<std::string, std::vector<std::string>> deps;
    for (int i = 0; i < n; ++i) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string name;
        iss >> name;

        std::vector<std::string> need;
        std::string tok;
        while (iss >> tok) {
            need.push_back(tok);
        }
        deps[name] = need;
    }

    int m;
    std::cin >> m;
    std::cin.ignore();

    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::vector<std::string> named;
    std::string tok;
    while (iss >> tok) {
        named.push_back(tok);
    }

    std::set<std::string> closure;
    std::queue<std::string> file;

    for (const auto& nom : named) {
        if (closure.insert(nom).second) {
            file.push(nom);
        }
    }

    while (!file.empty()) {
        std::string courant = file.front();
        file.pop();

        auto it = deps.find(courant);
        if (it != deps.end()) {
            for (const auto& besoin : it->second) {
                if (closure.insert(besoin).second) {
                    file.push(besoin);
                }
            }
        }
    }

    std::map<std::string, std::vector<std::string>> besoinsDe;
    for (const auto& module : closure) {
        auto it = deps.find(module);
        besoinsDe[module] = (it != deps.end()) ? it->second : std::vector<std::string>{};
    }

    std::map<std::string, int> compte;
    for (const auto& module : closure) {
        compte[module] = 0;
    }
    for (const auto& module : closure) {
        for (const auto& besoin : besoinsDe[module]) {
            compte[besoin]++;
        }
    }

    std::set<std::string> disponibles;
    for (const auto& module : closure) {
        if (compte[module] == 0) {
            disponibles.insert(module);
        }
    }

    std::set<std::string> sortis;
    std::vector<std::string> ordre;

    while (sortis.size() < closure.size()) {
        if (disponibles.empty()) {
            std::cout << "CYCLE\n";
            return 0;
        }

        std::string courant = *disponibles.begin();
        disponibles.erase(disponibles.begin());
        sortis.insert(courant);
        ordre.push_back(courant);

        for (const auto& besoin : besoinsDe[courant]) {
            if (sortis.find(besoin) == sortis.end()) {
                compte[besoin]--;
                if (compte[besoin] == 0) {
                    disponibles.insert(besoin);
                }
            }
        }
    }

    for (const auto& module : ordre) {
        std::cout << module << "\n";
    }

    return 0;
}
