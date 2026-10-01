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
    std::queue<std::string> queue;

    for (const auto& nom : named) {
        if (closure.insert(nom).second) {
            queue.push(nom);
        }
    }

    while (!queue.empty()) {
        std::string courant = queue.front();
        queue.pop();

        auto it = deps.find(courant);
        if (it != deps.end()) {
            for (const auto& besoin : it->second) {
                if (closure.insert(besoin).second) {
                    queue.push(besoin);
                }
            }
        }
    }

    for (const auto& module : closure) {
        std::cout << module << "\n";
    }

    return 0;
}
