#include <iostream>
#include <sstream>
#include <string>

bool finitPar(const std::string& s, const std::string& suffixe) {
    if (s.size() < suffixe.size()) return false;
    return s.compare(s.size() - suffixe.size(), suffixe.size(), suffixe) == 0;
}

bool commencePar(const std::string& s, const std::string& prefixe) {
    if (s.size() < prefixe.size()) return false;
    return s.compare(0, prefixe.size(), prefixe) == 0;
}

int main() {
    std::string arch;
    std::cin >> arch;

    int f;
    std::cin >> f;
    std::cin.ignore();

    long long total = 0;
    bool signe = false;
    bool abiOk = false;
    int inutile = 0;

    const std::string prefixeLib = "lib/";
    const std::string prefixeMeta = "META-INF/";
    const std::string prefixeArch = prefixeLib + arch + "/";

    for (int i = 0; i < f; ++i) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string chemin;
        long long taille;
        iss >> chemin >> taille;

        total += taille;

        if (commencePar(chemin, prefixeMeta) &&
            (finitPar(chemin, ".RSA") || finitPar(chemin, ".DSA") || finitPar(chemin, ".EC"))) {
            signe = true;
        }

        if (commencePar(chemin, prefixeLib)) {
            if (commencePar(chemin, prefixeArch)) {
                abiOk = true;
            } else {
                inutile++;
            }
        }
    }

    std::cout << total << "\n";
    std::cout << (signe ? "SIGNE" : "NON SIGNE") << "\n";
    std::cout << (abiOk ? "ABI OUI" : "ABI NON") << "\n";
    std::cout << "INUTILE " << inutile << "\n";

    return 0;
}
