//  VERIFICATION A LA MAIN de la composante y, tete qui leve les yeux de 30 deg :
//    Une tete tourne d'abord (lacet), puis leve les yeux (tangage). Le tangage
//    est donc la derniere rotation sur le regard, et le lacet, autour de la
//    verticale Y, ne change pas y.
//    Regard apres tangage seul : y = sin(30) = 0.5000, z = -cos(30) = -0.8660.
//    Ensuite le lacet de 45 deg tourne autour de Y : y reste 0.5000.
//    y attendu pour une tete = sin(30) = 0.5000, quel que soit le lacet.
//    Avec l'ordre inverse (lacet puis tangage sur le regard deja tourne), on
//    trouve y = sin(30) * cos(45) = 0.5 * 0.7071 = 0.3536 : le regard decrit
//    un cone au lieu de rester a l'altitude voulue.
//    Composantes completes attendues pour une tete :
//      x = -cos(30) * sin(45) = -0.6124
//      y =  sin(30)           =  0.5000
//      z = -cos(30) * cos(45) = -0.6124

#include "NKWindow/NKMain.h"
#include "NKMath/NkVec.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"
#include "NKMath/NkAngle.h"

#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::math;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {
	void Imprimer(const char *nom, const NkVec3f &v) {
		std::printf("%s : (%.4f, %.4f, %.4f)\n", nom,
					(double)v.x, (double)v.y, (double)v.z);
	}
}

int nkmain(const NkEntryState &state) {
	(void)state;

	const float lacetDeg = 45.0f;
	const float tangageDeg = 30.0f;
	const NkVec3f avant(0.f, 0.f, -1.f);

	const NkQuatf qLacet = NkQuatf::RotateY(NkAngle(lacetDeg));
	const NkQuatf qTangage = NkQuatf::RotateX(NkAngle(tangageDeg));

	const NkQuatf q1 = qLacet * qTangage;
	const NkQuatf q2 = qTangage * qLacet;

	const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) *
							 NkMat4f::RotationX(NkAngle(tangageDeg));
	const NkQuatf q3 = NkQuatf(rotation).Normalized();

	Imprimer("lacet*tangage", q1 * avant);
	Imprimer("tangage*lacet", q2 * avant);
	Imprimer("matrices", q3 * avant);

	std::printf("attendu a la main (tete) : (-0.6124, 0.5000, -0.6124)\n");
	std::fflush(stdout);
	return 0;
}