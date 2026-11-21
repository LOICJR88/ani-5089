#include <iostream>

int main() {
    int images = 0;
    int trames = 0;
    std::cin >> images >> trames;

    int index = 0;
    int rendues = 0;

    for (int i = 1; i <= trames; ++i) {
        int dessine = 0;
        std::cin >> dessine;

        if (dessine == 1) {
            std::cout << i << " OUI 1 " << index << "\n";
            ++rendues;
            if (images > 0) {
                index = (index + 1) % images;
            }
        } else {
            std::cout << i << " NON 0 -" << "\n";
        }
    }

    std::cout << "RENDUES " << rendues << "\n";
    std::cout << "FINS " << trames << "\n";
    return 0;
}