#include <iostream>

int main() {
    int n = 0;
    std::cin >> n;

    for (int i = 0; i < n; ++i) {
        long long p[3], s[3], t[3];
        for (int k = 0; k < 3; ++k) std::cin >> p[k];
        for (int k = 0; k < 3; ++k) std::cin >> s[k];
        for (int k = 0; k < 3; ++k) std::cin >> t[k];

        long long bon[3], mauvais[3];
        for (int k = 0; k < 3; ++k) {
            bon[k]     = p[k] * s[k] / 1000 + t[k]; 
            mauvais[k] = (p[k] + t[k]) * s[k] / 1000;
        }

        std::cout << bon[0] << " " << bon[1] << " " << bon[2] << "\n";
        std::cout << mauvais[0] << " " << mauvais[1] << " " << mauvais[2] << "\n";
    }
    return 0;
}