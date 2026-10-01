#include <iostream>
#include <sstream>
#include <string>
#include <map>

int main() {
    int v;
    std::cin >> v;
    std::cin.ignore();

    std::map<std::string, std::string> machine;
    for (int i = 0; i < v; ++i) {
        std::string line;
        std::getline(std::cin, line);

        size_t eq = line.find('=');
        std::string cle = line.substr(0, eq);
        std::string valeur = line.substr(eq + 1);
        machine[cle] = valeur;
    }

    int f;
    std::cin >> f;
    std::cin.ignore();

    for (int i = 0; i < f; ++i) {
        std::string line;
        std::getline(std::cin, line);

        std::istringstream iss(line);
        std::string token;
        bool filtreOk = true;

        while (iss >> token) {
            if (token == "&&") {
                continue;
            }

            bool nie = false;
            std::string terme = token;
            if (!terme.empty() && terme[0] == '!') {
                nie = true;
                terme = terme.substr(1);
            }

            size_t eq = terme.find('=');
            std::string cle = terme.substr(0, eq);
            std::string valeur = terme.substr(eq + 1);

            bool vrai;
            auto it = machine.find(cle);
            if (it == machine.end()) {
                vrai = false;
            } else {
                vrai = (it->second == valeur);
            }

            if (nie) {
                vrai = !vrai;
            }

            if (!vrai) {
                filtreOk = false;
            }
        }

        std::cout << (filtreOk ? "OUI" : "NON") << "\n";
    }

    return 0;
}
