#include <iostream>
#include <string>
#include <vector>

int main() {
    int a;
    if (!(std::cin >> a)) {
        return 1;
    }

    std::vector<std::string> noms(a);
    std::vector<std::string> usages(a);
    for (int i = 0; i < a; ++i) {
        std::cin >> noms[i] >> usages[i];
    }

    int l = 0;
    std::cin >> l;
    std::vector<std::string> fournis(l);
    for (int i = 0; i < l; ++i) {
        std::cin >> fournis[i];
    }

    int mortes = 0;
    for (int i = 0; i < a; ++i) {
        bool active = false;
        for (int j = 0; j < l; ++j) {
            if (fournis[j] == usages[i]) {
                active = true;
                break;
            }
        }
        if (!active) {
            ++mortes;
        }
        std::cout << noms[i] << ' ' << (active ? "ACTIVE" : "MORTE") << '\n';
    }

    std::cout << "MORTES " << mortes << '\n';
    return 0;
}