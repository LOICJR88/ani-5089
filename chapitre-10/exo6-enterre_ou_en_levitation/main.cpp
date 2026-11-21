#include <iostream>
#include <string>

int main() {
    int hauteur = 0;
    int n = 0;
    std::cin >> hauteur >> n;

    int decalees = 0;

    for (int i = 0; i < n; ++i) {
        std::string nom;
        std::string espacePose;
        std::string espaceCouche;
        std::cin >> nom >> espacePose >> espaceCouche;

        int decalage = 0;
        std::string verdict = "OK";

        if (espacePose != espaceCouche) {
            if (espacePose == "STAGE") {
                decalage = hauteur;
                verdict = "LEVITATION";
            } else {
                decalage = -hauteur;
                verdict = "ENTERRE";
            }
            ++decalees;
        }

        std::cout << nom << " " << decalage << " " << verdict << "\n";
    }

    std::cout << "DECALEES " << decalees << "\n";
    return 0;
}