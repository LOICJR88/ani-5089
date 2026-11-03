#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "NKMath/NkQuat.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

	const float32 kPiece = 4.00f;	  
	const float32 kPlafond = 2.50f;	  
	const float32 kMur = 0.10f;		  
	const float32 kPorteL = 0.90f;	 
	const float32 kPorteH = 2.00f;	 
	const float32 kFenL = 1.20f;	 
	const float32 kFenH = 1.00f;	 
	const float32 kFenY = 1.40f;	 
	const float32 kTableL = 1.20f;	  
	const float32 kTableP = 0.80f;	  
	const float32 kTableH = 0.75f;	  
	const float32 kYeux = 1.70f;	 

	const float32 kDemi = kPiece * 0.5f;

	void Pave(NkRender3D *r3d, NkMeshSystem *meshes, const NkVec3f &centre,
			  const NkVec3f &taille, const NkVec3f &teinte, float32 rugosite = 1.f) {
		NkDrawCall3D dc;
		dc.mesh = meshes->GetCube();
		dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
		dc.aabb = { { centre.x - taille.x * 0.5f, centre.y - taille.y * 0.5f,
					  centre.z - taille.z * 0.5f },
					{ centre.x + taille.x * 0.5f, centre.y + taille.y * 0.5f,
					  centre.z + taille.z * 0.5f } };
		dc.tint = teinte;
		dc.metallic = 0.f;
		dc.roughness = rugosite;
		r3d->Submit(dc);
	}

	void SoumettreLaSalle(NkRender3D *r3d, NkMeshSystem *meshes) {
		const NkVec3f grisMur = { 0.72f, 0.70f, 0.66f };
		const NkVec3f grisSol = { 0.38f, 0.36f, 0.34f };
		const NkVec3f bois = { 0.55f, 0.38f, 0.22f };
		const NkVec3f vitre = { 0.75f, 0.85f, 0.92f };

		{ 
			NkDrawCall3D dc;
			dc.mesh = meshes->GetPlane();
			dc.transform = NkMat4f::Scale({ kPiece, 1.f, kPiece });
			dc.aabb = { { -kDemi, 0.f, -kDemi }, { kDemi, 0.f, kDemi } };
			dc.castShadow = false;
			dc.tint = grisSol;
			dc.metallic = 0.f;
			dc.roughness = 1.f;
			r3d->Submit(dc);
		}

		const float32 yMur = kPlafond * 0.5f;
		const float32 bord = kDemi + kMur * 0.5f;

		Pave(r3d, meshes, { 0.f, yMur, -bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { 0.f, yMur, bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { -bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);
		Pave(r3d, meshes, { bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);

		Pave(r3d, meshes, { 0.f, kPlafond + kMur * 0.5f, 0.f },
			 { kPiece, kMur, kPiece }, grisMur);

		Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
			 { kPorteL, kPorteH, 0.04f }, bois, 0.6f);

		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);

		const float32 tx = 0.60f, tz = 0.70f;
		const float32 ep = 0.05f;
		Pave(r3d, meshes, { tx, kTableH - ep * 0.5f, tz },
			 { kTableL, ep, kTableP }, bois, 0.5f);

		const float32 pied = 0.06f;
		const float32 hp = kTableH - ep;
		for (int i = 0; i < 4; ++i) {
			const float32 dx = ((i & 1) ? 1.f : -1.f) * (kTableL * 0.5f - pied);
			const float32 dz = ((i & 2) ? 1.f : -1.f) * (kTableP * 0.5f - pied);
			Pave(r3d, meshes, { tx + dx, hp * 0.5f, tz + dz },
				 { pied, hp, pied }, bois, 0.6f);
		}

		Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.f, 1.f, 1.f },
			 { 0.85f, 0.80f, 0.30f }, 0.8f);
	}

	const double kPi = 3.14159265358979323846;
	const double kPeriode = 1.0 / 72.0;		
	const double kDebutGeste = 1.0;			
	const unsigned kMaxEchantillons = 64;	

	double Lacet(double t) { 
		const double amplitude = 90.0, duree = 0.400;
		if (t <= 0.0) return 0.0;
		if (t >= duree) return amplitude;
		return amplitude * 0.5 * (1.0 - std::cos(kPi * t / duree));
	}

	NkQuatf Pose(double instant) {
		return NkQuatf::RotateY(NkAngle((float32)Lacet(instant - kDebutGeste)));
	}

	double LacetDe(const NkQuatf &q) {
		NkQuatf r = q;
		if (r.w < 0.f) {
			r = NkQuatf(-r.x, -r.y, -r.z, -r.w);
		}
		return 2.0 * std::atan2((double)r.y, (double)r.w) * 180.0 / kPi;
	}

	NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dtSeconds) {
		if (dtSeconds <= 0.f) {
			return NkVec3f(0.f, 0.f, 0.f);
		}
		NkQuatf delta = (to * from.Conjugate()).Normalized();

		if (delta.w < 0.f) {
			delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
		}
		const float32 angleRad = delta.Angle().Rad();
		if (angleRad <= 1e-6f) {
			return NkVec3f(0.f, 0.f, 0.f);
		}
		return delta.Axis() * (angleRad / dtSeconds);
	}

	NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse, float32 dtSeconds) {
		const float32 speedRad = vitesse.Len();
		if (speedRad > 1e-6f) {
			const NkVec3f axis = vitesse * (1.f / speedRad);
			const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
			return (delta * orientation).Normalized();
		}
		return orientation;
	}

	struct Echantillon {
		double temps;	  
		NkQuatf pose;
		NkVec3f vitesse; 
	};

	struct Historique {
		Echantillon s[kMaxEchantillons];
		unsigned tete = 0;	
		unsigned compte = 0; 
	};

	void Ajouter(Historique &h, double temps, const NkQuatf &pose, const NkVec3f &vitesse) {
		h.tete = (h.compte == 0u) ? 0u : (h.tete + 1u) % kMaxEchantillons;
		h.s[h.tete].temps = temps;
		h.s[h.tete].pose = pose;
		h.s[h.tete].vitesse = vitesse;
		if (h.compte < kMaxEchantillons) ++h.compte;
	}

	const Echantillon &EchantillonA(const Historique &h, double maintenant, double latenceS) {
		if (latenceS <= 0.0 || h.compte <= 1u) {
			return h.s[h.tete];
		}

		const double cible = maintenant - latenceS;
		unsigned index = h.tete;
		for (unsigned i = 0u; i < h.compte; ++i) {
			if (h.s[index].temps <= cible) {
				return h.s[index];
			}
			index = (index + kMaxEchantillons - 1u) % kMaxEchantillons;
		}

		return h.s[(h.tete + kMaxEchantillons - (h.compte - 1u)) % kMaxEchantillons];
	}

	NkQuatf PredireA(const Echantillon &e, double quand, bool borne, float32 *dtUtilise) {
		float32 dt = (float32)(quand - e.temps);
		if (borne) {
			if (dt > 0.1f) dt = 0.1f;
			if (dt < -0.1f) dt = -0.1f;
		}
		if (dtUtilise != nullptr) *dtUtilise = dt;
		return Extrapoler(e.pose, e.vitesse, dt);
	}

	void MesurerLatence(int latenceMs) {
		const double latenceS = (double)latenceMs * 1e-3;
		Historique h;
		NkQuatf precedente = Pose(0.0);
		double sommeAvec = 0.0, sommeSans = 0.0, maxAvec = 0.0, maxSans = 0.0;
		size_t n = 0;

		for (size_t k = 0; k < 216; ++k) { 
			const double t = (double)k * kPeriode;
			const NkQuatf pose = Pose(t);
			const NkVec3f v = (k > 0) ? VitesseAngulaire(precedente, pose, (float32)kPeriode)
									  : NkVec3f(0.f, 0.f, 0.f);
			Ajouter(h, t, pose, v);
			precedente = pose;

			if (k >= 72 && k <= 136) { 
				const Echantillon &e = EchantillonA(h, t, latenceS);
				const NkQuatf predite = PredireA(e, t + kPeriode, true, nullptr);
				const NkQuatf vraie = Pose(t + kPeriode);
				const double errAvec =
					(double)(vraie * predite.Conjugate()).Normalized().Angle().Deg();
				const double errSans =
					(double)(vraie * e.pose.Conjugate()).Normalized().Angle().Deg();
				sommeAvec += errAvec;
				sommeSans += errSans;
				if (errAvec > maxAvec) maxAvec = errAvec;
				if (errSans > maxSans) maxSans = errSans;
				++n;
			}
		}
		std::printf("latence %d ms : avec %.3f deg (max %.3f), sans %.3f deg (max %.3f)\n",
					latenceMs, sommeAvec / (double)n, maxAvec, sommeSans / (double)n, maxSans);
	}

	void MesurerBorne(bool borne) {
		Historique h;
		NkQuatf precedente = Pose(0.0);
		Echantillon meilleur;
		meilleur.temps = 0.0;
		double meilleurMaintenant = 0.0;
		double meilleurEcart = 1e9;

		for (size_t k = 0; k < 216; ++k) {
			const double t = (double)k * kPeriode;
			const NkQuatf pose = Pose(t);
			const NkVec3f v = (k > 0) ? VitesseAngulaire(precedente, pose, (float32)kPeriode)
									  : NkVec3f(0.f, 0.f, 0.f);
			Ajouter(h, t, pose, v);
			precedente = pose;

			const Echantillon &e = EchantillonA(h, t, 1.0);
			const double ecart = std::fabs(e.temps - 1.2);
			if (ecart < meilleurEcart) {
				meilleurEcart = ecart;
				meilleur = e;
				meilleurMaintenant = t;
			}
		}

		const double quand = meilleurMaintenant + kPeriode;
		float32 dt = 0.f;
		const NkQuatf predite = PredireA(meilleur, quand, borne, &dt);
		std::printf("%s : echantillon a %.3f s, horizon %.1f ms, predit %.3f deg, vrai %.3f deg\n",
					borne ? "avec borne" : "sans borne", meilleur.temps,
					(double)dt * 1000.0, LacetDe(predite), LacetDe(Pose(quand)));
	}
	uint64 EnvU64(const char *nom, uint64 defaut) {
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0') return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}

} 

int nkmain(const NkEntryState &state) {
	(void)state;

	const int latencesMs[5] = { 0, 10, 20, 40, 80 };
	for (int i = 0; i < 5; ++i) {
		MesurerLatence(latencesMs[i]);
	}

	MesurerBorne(false);
	MesurerBorne(true);
	NkWindowConfig cfg;
	cfg.title = "Ma salle";
	cfg.width = 1280;
	cfg.height = 720;
	cfg.centered = true;
	cfg.vsync = true;

	NkWindow fenetre(cfg);
	if (!fenetre.IsValid()) {
		std::printf("[MaSalle] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}

	NkDeviceInitInfo devInit{};
	devInit.surface = fenetre.GetSurfaceDesc();
	devInit.width = fenetre.GetSize().x;
	devInit.height = fenetre.GetSize().y;

	NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
	if (device == nullptr) {
		std::printf("[MaSalle] aucune API graphique disponible\n");
		return 1;
	}

	NkRendererConfig cfgEcran =
		NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
	NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
	if (rEcran == nullptr || !rEcran->Initialize()) {
		std::printf("[MaSalle] le renderer d'ecran n'a pas pu s'initialiser\n");
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkOffscreenDesc od;
	od.width = devInit.width;
	od.height = devInit.height;
	od.hasDepth = true;
	od.hdr = false;
	od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	od.readable = true;
	od.readback = true; 
	od.name = "MaSalle_Cible";

	NkOffscreenTarget *cible = rEcran->CreateOffscreen(od);
	if (cible == nullptr) {
		std::printf("[MaSalle] cible hors ecran refusee\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkRendererConfig cfgSalle =
		NkRendererConfig::ForGame(devInit.api, devInit.width, devInit.height);
	cfgSalle.Disable(NK_SS_RENDER2D);
	cfgSalle.Disable(NK_SS_TEXT);
	cfgSalle.Disable(NK_SS_OVERLAY);
	cfgSalle.Enable(NK_SS_OFFSCREEN);

	if (EnvU64("MASALLE_POST", 1) == 0) {
		cfgSalle.Disable(NK_SS_POST_PROCESS);
	}

	NkRenderer *renderer = NkRenderer::Create(device, cfgSalle);
	if (renderer == nullptr || !renderer->Initialize()) {
		std::printf("[MaSalle] le renderer de la salle n'a pas pu s'initialiser\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}
	renderer->SetFinalColorTarget(
		rEcran->GetTextures()->GetRHIHandle(cible->GetColorResult()));

	std::printf("[MaSalle] salle de %.1f m sur %.1f m, plafond a %.2f m\n",
				(double)kPiece, (double)kPiece, (double)kPlafond);

	const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
	const uint64 imageDeCapture = EnvU64("MASALLE_SHOT", 0);

	uint64 image = 0;
	bool tourne = true;

	while (tourne && fenetre.IsOpen()) {
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev)) {
			if (ev->As<NkWindowCloseEvent>() != nullptr) {
				tourne = false;
			}
		}
		if (!tourne) break;

		if (rEcran->BeginFrame()) {
			NkICommandBuffer *cmd = rEcran->GetCmd();

			NkRender3D *r3d = renderer->GetRender3D();

			renderer->FlushGraphRebuilds();
			r3d->ResetFrame();
			if (auto *mc = renderer->GetMaterialCollection()) {
				mc->Upload();
			}

			NkCamera3DData cd;

			cd.position = { 0.30f, kYeux, 1.20f };
			cd.target = { -0.70f, 1.10f, -kDemi };
			cd.up = { 0.f, 1.f, 0.f };
			cd.fovY = 70.f;
			cd.aspect = (float32)devInit.width / (float32)devInit.height;
			cd.nearPlane = 0.05f;
			cd.farPlane = 100.f;

			NkSceneContext sctx;
			sctx.camera = NkCamera3D(cd);
			sctx.time = (float32)image / 60.f;
			r3d->SetIBLStrength(0.35f);

			NkLightDesc soleil;
			soleil.type = NkLightType::NK_DIRECTIONAL;
			soleil.direction = { -0.5f, -1.f, -0.35f };
			soleil.color = { 1.f, 0.96f, 0.88f };
			soleil.intensity = 3.f;
			soleil.castShadow = false;
			sctx.lights.PushBack(soleil);

			r3d->BeginScene(sctx);
			SoumettreLaSalle(r3d, renderer->GetMeshSystem());

			if (auto *graph = renderer->GetRenderGraph()) {
				graph->Execute(cmd);
			}

			if (auto *r2d = rEcran->GetRender2D()) {
				r2d->DrawImage(cible->GetColorResult(),
							   { 0.f, 0.f, (float32)devInit.width,
								 (float32)devInit.height });
			}

			rEcran->Present();
			rEcran->EndFrame();
		}

		++image;

		if (imageDeCapture != 0 && image == imageDeCapture) {
			const char *chemin = getenv("MASALLE_SHOT_PATH");
			if (chemin == nullptr || *chemin == '\0') {
				chemin = "masalle.png";
			}
			const bool ok = cible->Capture(chemin);
			std::printf("[MaSalle] capture image %llu -> %s (%s)\n",
						(unsigned long long)image, chemin, ok ? "ok" : "echec");
		}

		if (imageDeSortie != 0 && image >= imageDeSortie) {
			tourne = false;
		}
	}

	std::printf("[MaSalle] %llu images\n", (unsigned long long)image);

	NkRenderer::Destroy(renderer);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return 0;
}
