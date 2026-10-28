#include "NKWindow/NKMain.h"
#include "NKMath/NkVec.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"
#include "NKMath/NkAngle.h"
#include "NKTime/NkChrono.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

using namespace nkentseu;
using namespace nkentseu::math;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

	const int kIterations = 1000000;
	const float kTangageDeg = 30.0f;

	NkQuatf ParMatrices(float lacetDeg, float tangageDeg) {
		const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) *
								 NkMat4f::RotationX(NkAngle(tangageDeg));
		return NkQuatf(rotation).Normalized();
	}

	NkQuatf ParQuaternions(float lacetDeg, float tangageDeg) {
		const NkQuatf qLacet = NkQuatf::RotateY(NkAngle(lacetDeg));
		const NkQuatf qTangage = NkQuatf::RotateX(NkAngle(tangageDeg));
		return (qLacet * qTangage).Normalized();
	}

	double BoucleMatrices() {
		double somme = 0.0;
		for (int i = 0; i < kIterations; ++i) {
			const float lacetDeg = (float)(i % 3600) * 0.1f;
			const NkQuatf q = ParMatrices(lacetDeg, kTangageDeg);
			somme += (double)q.x + (double)q.y + (double)q.z + (double)q.w;
		}
		return somme;
	}

	double BoucleQuaternions() {
		double somme = 0.0;
		for (int i = 0; i < kIterations; ++i) {
			const float lacetDeg = (float)(i % 3600) * 0.1f;
			const NkQuatf q = ParQuaternions(lacetDeg, kTangageDeg);
			somme += (double)q.x + (double)q.y + (double)q.z + (double)q.w;
		}
		return somme;
	}

}

int nkmain(const NkEntryState &state) {
	(void)state;

	const char *config = getenv("MASALLE_CONFIG");
	if (config == nullptr || *config == '\0') config = "Debug";

	{
		const NkQuatf a = ParMatrices(45.0f, kTangageDeg);
		const NkQuatf b = ParQuaternions(45.0f, kTangageDeg);
		const double scalaire = (double)a.x * b.x + (double)a.y * b.y +
								(double)a.z * b.z + (double)a.w * b.w;
		std::printf("verification lacet 45, tangage 30 :\n");
		std::printf("  matrices    : (%.6f, %.6f, %.6f, %.6f)\n",
					(double)a.x, (double)a.y, (double)a.z, (double)a.w);
		std::printf("  quaternions : (%.6f, %.6f, %.6f, %.6f)\n",
					(double)b.x, (double)b.y, (double)b.z, (double)b.w);
		std::printf("  produit scalaire = %.6f\n", scalaire);
	}

	double sommeTotale = 0.0;
	for (int essai = 0; essai < 3; ++essai) {
		{
			NkChrono c;
			const double s = BoucleMatrices();
			const double ms = (double)c.Elapsed().ToMilliseconds();
			sommeTotale += s;
			std::printf("%s, matrices : %.1f ms\n", config, ms);
		}
		{
			NkChrono c;
			const double s = BoucleQuaternions();
			const double ms = (double)c.Elapsed().ToMilliseconds();
			sommeTotale += s;
			std::printf("%s, quaternions : %.1f ms\n", config, ms);
		}
	}

	std::printf("somme = %.3f\n", sommeTotale);
	std::fflush(stdout);
	return 0;
}