#include <iostream>
#include <string>

int main() {
    int n = 0;
    if (!(std::cin >> n)) {
        return 0;
    }

    int dx = 0;
    int dy = 0;
    int bx = 0;
    int by = 0;

    for (int i = 0; i < n; ++i) {
        std::string commande;
        if (!(std::cin >> commande)) {
            break;
        }

        if (commande == "bouge") {
            int mx = 0;
            int my = 0;
            std::cin >> mx >> my;
            dx += mx;
            dy += my;
            bx = mx;
            by = my;
        } else if (commande == "image") {
            std::cout << dx << " " << dy << " " << bx << " " << by << "\n";
            dx = 0;
            dy = 0;
        }
    }

    return 0;
}
