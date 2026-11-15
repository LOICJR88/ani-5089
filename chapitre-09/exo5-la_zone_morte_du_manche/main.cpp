#include <iostream>

int main() {
    long long seuil, maximum;
    if (!(std::cin >> seuil >> maximum)) {
        return 1;
    }

    int n;
    std::cin >> n;

    const long long course = 1000 - seuil;   
    int morts = 0;

    for (int i = 0; i < n; ++i) {
        long long valeur;
        std::cin >> valeur;

        const long long absolue = valeur < 0 ? -valeur : valeur;

        long long vitesse = 0;
        if (absolue < seuil) {
            ++morts;                        
        } else if (course > 0) {
            const long long reste = absolue - seuil;
            vitesse = maximum * reste * reste / (course * course);
        }

        if (valeur < 0) {
            vitesse = -vitesse;               
        }

        std::cout << vitesse << '\n';
    }

    std::cout << "MORTS " << morts << '\n';
    return 0;
}