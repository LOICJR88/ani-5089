#include <iostream>
#include <string>

int main() {
    int n;
    if (!(std::cin >> n)) {
        return 1;
    }

    int fautes = 0;

    for (int i = 0; i < n; ++i) {
        std::string nom, intention, pose;
        std::cin >> nom >> intention >> pose;

        std::cout << nom << ' ';
        if (intention == "TENIR" && pose == "AIM") {
            std::cout << "FLOTTE";
            ++fautes;
        } else if (intention == "POINTER" && pose == "GRIP") {
            std::cout << "PART DE TRAVERS";
            ++fautes;
        } else {
            std::cout << "OK";
        }
        std::cout << '\n';
    }

    std::cout << "FAUTES " << fautes << '\n';
    return 0;
}