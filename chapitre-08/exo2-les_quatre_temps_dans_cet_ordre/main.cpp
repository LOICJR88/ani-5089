#include <iostream>
#include <string>

int main() {
    int n = 0;
    if (!(std::cin >> n)) {
        return 0;
    }

    bool acquise = false;
    bool attendSoumission = false;
    int fautes = 0;

    for (int tour = 1; tour <= n; ++tour) {
        std::string appel;
        if (!(std::cin >> appel)) {
            break;
        }

        const char* refus = nullptr;

        if (appel == "ACQUIERT") {
            if (acquise) {
                refus = "DEJA ACQUISE";
            } else {
                acquise = true;
            }
        } else if (appel == "DESSINE") {
            if (!acquise) {
                refus = "PAS ACQUISE";
            }
        } else if (appel == "REND") {
            if (!acquise) {
                refus = "RIEN ACQUIS";
            } else {
                acquise = false;
                attendSoumission = true;
            }
        } else if (appel == "SOUMET") {
            if (!attendSoumission) {
                refus = "PAS RENDUE";
            } else {
                attendSoumission = false;
            }
        }

        std::cout << tour << " " << appel << " ";
        if (refus == nullptr) {
            std::cout << "OK\n";
        } else {
            std::cout << refus << "\n";
            ++fautes;
        }
    }

    std::cout << "FAUTES " << fautes << "\n";
    return 0;
}