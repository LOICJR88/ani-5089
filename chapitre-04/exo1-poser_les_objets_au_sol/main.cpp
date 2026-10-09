#include <iostream>
#include <string>

int main() {
    int n = 0;
    std::cin >> n;

    long long haut = 0;
    for (int i = 0; i < n; ++i) {
        std::string nom;
        long long hauteur = 0;
        long long support = 0;
        std::cin >> nom >> hauteur >> support;

        long long centre = support + hauteur / 2;
        long long sommet = support + hauteur;

        std::cout << nom << " " << centre << "\n";
        if (sommet > haut) {
            haut = sommet;
        }
    }

    std::cout << "HAUT " << haut << "\n";
    return 0;
}