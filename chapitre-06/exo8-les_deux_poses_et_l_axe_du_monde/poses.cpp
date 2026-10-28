#include <cmath>
#include <cstdio>

#include "NKMath/NkAngle.h"
#include "NKMath/NkQuat.h"

using namespace nkentseu;
using namespace nkentseu::math;

namespace {

struct Pose {
    const char* nom;
    NkQuatf orientation;
};

void Imprimer(const char* nom, const char* methode, const NkVec3f& g, const NkVec3f& d) {
    std::printf("%s, %s : G (%.4f, %.4f, %.4f) D (%.4f, %.4f, %.4f)\n", nom, methode,
                (double)g.x, (double)g.y, (double)g.z,
                (double)d.x, (double)d.y, (double)d.z);
}

} 

void CalculerLesPoses() {
    const NkVec3f positionTete(0.30f, 1.70f, 1.20f);
    const float ipd = 0.063f;
    const float demiEcart = ipd * 0.5f;

    const NkVec3f droiteSalle(1.f, 0.f, 0.f);

    const Pose poses[3] = {
        { "neutre",    NkQuatf::Identity() },
        { "roulis 30", NkQuatf::RotateZ(NkAngle::FromDeg(30.f)) },
        { "lacet 90",  NkQuatf::RotateY(NkAngle::FromDeg(90.f)) },
    };

    float ecartsMm[3] = { 0.f, 0.f, 0.f };

    for (int i = 0; i < 3; ++i) {
        const NkVec3f droiteTete = poses[i].orientation * droiteSalle;

        const NkVec3f gaucheTete(positionTete.x - droiteTete.x * demiEcart,
                                 positionTete.y - droiteTete.y * demiEcart,
                                 positionTete.z - droiteTete.z * demiEcart);
        const NkVec3f droitTete(positionTete.x + droiteTete.x * demiEcart,
                                positionTete.y + droiteTete.y * demiEcart,
                                positionTete.z + droiteTete.z * demiEcart);

        const NkVec3f gaucheMonde(positionTete.x - droiteSalle.x * demiEcart,
                                  positionTete.y - droiteSalle.y * demiEcart,
                                  positionTete.z - droiteSalle.z * demiEcart);
        const NkVec3f droitMonde(positionTete.x + droiteSalle.x * demiEcart,
                                 positionTete.y + droiteSalle.y * demiEcart,
                                 positionTete.z + droiteSalle.z * demiEcart);

        Imprimer(poses[i].nom, "tete", gaucheTete, droitTete);
        Imprimer(poses[i].nom, "monde", gaucheMonde, droitMonde);

        const float dx = gaucheTete.x - gaucheMonde.x;
        const float dy = gaucheTete.y - gaucheMonde.y;
        const float dz = gaucheTete.z - gaucheMonde.z;
        ecartsMm[i] = std::sqrt(dx * dx + dy * dy + dz * dz) * 1000.f;
    }

    for (int i = 0; i < 3; ++i) {
        std::printf("%s : ecart %.3f mm\n", poses[i].nom, (double)ecartsMm[i]);
    }
}