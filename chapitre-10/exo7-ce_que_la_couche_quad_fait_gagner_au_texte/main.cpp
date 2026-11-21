#include <iostream>
#include <string>

int main() {
    long long rendu = 0;
    long long deformation = 0;
    long long reprojection = 0;
    long long seuil = 0;
    std::cin >> rendu >> deformation >> reprojection >> seuil;

    int n = 0;
    std::cin >> n;

    int gagnes = 0;
    int perdus = 0;

    for (int i = 0; i < n; ++i) {
        std::string nom;
        long long pixels = 0;
        std::cin >> nom >> pixels;

        long long scene = pixels;
        scene = scene * rendu / 1000;
        scene = scene * deformation / 1000;
        scene = scene * reprojection / 1000;

        const long long quad = pixels;

        const bool sceneLisible = scene >= seuil;
        const bool quadLisible = quad >= seuil;

        std::cout << nom << " " << scene << " " << quad << " "
                  << (sceneLisible ? "LISIBLE" : "FLOU") << " "
                  << (quadLisible ? "LISIBLE" : "FLOU") << "\n";

        if (!sceneLisible && quadLisible) {
            ++gagnes;
        }
        if (sceneLisible && !quadLisible) {
            ++perdus;
        }
    }

    std::cout << "GAGNES " << gagnes << "\n";
    std::cout << "PERDUS " << perdus << "\n";
    return 0;
}