#include <iostream>
#include <map>
#include <string>
#include <vector>

int main() {
    const std::map<std::string, std::string> noms = {
        {"OPENGL",   "OpenGL"},
        {"OPENGLES", "OpenGL ES"},
        {"WEBGL",    "WebGL"},
        {"VULKAN",   "Vulkan"},
        {"DX11",     "DirectX 11"},
        {"DX12",     "DirectX 12"},
        {"METAL",    "Metal"},
        {"SOFTWARE", "Software"},
    };

    int d = 0;
    std::cin >> d;
    std::vector<std::string> disponibles(d);
    for (int i = 0; i < d; ++i) {
        std::cin >> disponibles[i];
    }

    int m = 0;
    std::cin >> m;
    for (int i = 0; i < m; ++i) {
        std::string demande;
        std::cin >> demande;

        std::string retenu;  
        if (demande == "NONE") {
            
            if (!disponibles.empty()) {
                retenu = disponibles[0];
            }
        } else {
            
            for (const auto& code : disponibles) {
                if (code == demande) {
                    retenu = code;
                    break;
                }
            }
        }

        auto it = noms.find(retenu);
        if (it != noms.end()) {
            std::cout << it->second << "\n";
        } else {
            std::cout << "None" << "\n";   
        }
    }
    return 0;
}