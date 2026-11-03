// =============================================================================
//  Etape 5 - la salle, en cubes, aux dimensions du chapitre 2.
//  Version exo 9 : la salle est soumise K fois (copies decalees de 5 m le long
//  de x), on mesure l'intervalle entre deux debuts d'image, et on peut verrouiller
//  la cadence a deux periodes de l'ecran.
//
//  Variables d'environnement :
//     MASALLE_HZ      frequence de l'ecran, en Hz (48 sur ma machine)
//     MASALLE_COPIES  nombre de copies de la salle (1 par defaut)
//     MASALLE_VERROU  1 = verrou de demi-cadence, 0 = sans
// =============================================================================
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
#include <vector>
#include "NKTime/NkChrono.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

	// -- Les dimensions de la salle, en un seul endroit ----------------------
	const float32 kPiece = 4.00f;	  // cote interieur du sol
	const float32 kPlafond = 2.50f;	  // hauteur sous plafond
	const float32 kMur = 0.10f;		  // epaisseur d'un mur
	const float32 kPorteL = 0.90f;	  // porte : largeur
	const float32 kPorteH = 2.00f;	  //         hauteur
	const float32 kFenL = 1.20f;	  // fenetre : largeur
	const float32 kFenH = 1.00f;	  //           hauteur
	const float32 kFenY = 1.40f;	  //           hauteur de son centre
	const float32 kTableL = 1.20f;	  // table : longueur
	const float32 kTableP = 0.80f;	  //         profondeur
	const float32 kTableH = 0.75f;	  //         hauteur du plateau
	const float32 kYeux = 1.70f;	  // hauteur des yeux, debout

	const float32 kDemi = kPiece * 0.5f;

	// Decalage de la copie courante de la salle, le long de x (en metres)
	NkVec3f gDecalage = { 0.f, 0.f, 0.f };

	// Un pave pose dans la salle. `centreLocal` est le centre geometrique dans la
	// salle, `taille` les trois dimensions en metres. Le decalage de la copie
	// courante s'ajoute au centre.
	void Pave(NkRender3D *r3d, NkMeshSystem *meshes, const NkVec3f &centreLocal,
			  const NkVec3f &taille, const NkVec3f &teinte, float32 rugosite = 1.f) {
		const NkVec3f centre = { centreLocal.x + gDecalage.x, centreLocal.y + gDecalage.y,
								 centreLocal.z + gDecalage.z };
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

		{ // le sol : le plan est unitaire, une echelle S donne un cote S
			NkDrawCall3D dc;
			dc.mesh = meshes->GetPlane();
			dc.transform = NkMat4f::Translate(gDecalage) * NkMat4f::Scale({ kPiece, 1.f, kPiece });
			dc.aabb = { { -kDemi + gDecalage.x, 0.f, -kDemi }, { kDemi + gDecalage.x, 0.f, kDemi } };
			dc.castShadow = false;
			dc.tint = grisSol;
			dc.metallic = 0.f;
			dc.roughness = 1.f;
			r3d->Submit(dc);
		}

		const float32 yMur = kPlafond * 0.5f;
		const float32 bord = kDemi + kMur * 0.5f;

		// Quatre murs. La porte est dans celui du fond, les fenetres a gauche.
		Pave(r3d, meshes, { 0.f, yMur, -bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { 0.f, yMur, bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { -bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);
		Pave(r3d, meshes, { bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);

		// Le plafond.
		Pave(r3d, meshes, { 0.f, kPlafond + kMur * 0.5f, 0.f },
			 { kPiece, kMur, kPiece }, grisMur);

		// La porte, fermee : un panneau plaque contre le mur du fond.
		Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
			 { kPorteL, kPorteH, 0.04f }, bois, 0.6f);

		// Deux fenetres sur le mur de gauche.
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);

		// La table : un plateau et quatre pieds.
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

		// Le repere d'un metre : un cube d'un metre de cote, pose au sol.
		Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.f, 1.f, 1.f },
			 { 0.85f, 0.80f, 0.30f }, 0.8f);
	}

	// Attend jusqu'a l'instant cible (ns, horloge de NkChrono).
	// Sans minuterie fine, SleepMilliseconds(1) dure ~15,5 ms (NkChrono.h, mesure du
	// 2026-08-15) : on ne s'en sert alors jamais. Avec elle, 1 ms dure ~1,9 ms : on
	// dort tant qu'il reste plus de 3 ms, puis on attend en boucle active sur
	// l'horloge, ce qui tient l'instant vise a une fraction de milliseconde, au prix
	// d'un coeur occupe.
	void AttendreJusqua(uint64 cibleNs, bool minuterieFine) {
		for (;;) {
			const uint64 maintenant = (uint64)NkChrono::Now().nanoseconds;
			if (maintenant >= cibleNs) return;
			const double resteMs = (double)(cibleNs - maintenant) * 1e-6;
			if (minuterieFine && resteMs > 3.0) {
				NkChrono::SleepMilliseconds(1);
			}
		}
	}

	uint64 EnvU64(const char *nom, uint64 defaut) {
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0') return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}

} // namespace

int nkmain(const NkEntryState &state) {
	(void)state;

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

	// Deux renderers : le premier POSSEDE l'image (BeginFrame, Present, EndFrame)
	// et ne dessine que du 2D ; le second dessine la salle dans une cible hors
	// ecran, que le premier affiche ensuite dans la fenetre.
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
	od.readback = true; // sans quoi Capture() ne peut rien relire
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

	// NK_SS_POST_PROCESS porte le tonemap, le bloom, le FXAA et le SSAO.
	// MASALLE_POST=0 le coupe.
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

	// Sortie automatique : MASALLE_EXIT=<image> arrete proprement.
	const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
	const uint64 imageDeCapture = EnvU64("MASALLE_SHOT", 0);

	// -- Exo 9 : parametres de la mesure et du verrou ------------------------
	// HZ : la frequence de l'ecran, lue dans les parametres d'affichage de Windows
	const double hzLu = (double)EnvU64("MASALLE_HZ", 72);
	const double hz = (hzLu > 1.0) ? hzLu : 72.0;
	const uint64 periodeNs = (uint64)(1e9 / hz);
	const uint64 copies = EnvU64("MASALLE_COPIES", 1);
	const bool verrou = (EnvU64("MASALLE_VERROU", 0) != 0);

	// Minuterie fine (NkChrono.h) : demandee une fois, rendue a la fin si c'est nous
	const bool etaitActive = NkChrono::IsPreciseTimingActive();
	if (!etaitActive) {
		NkChrono::BeginPreciseTiming();
	}
	const bool minuterieFine = NkChrono::IsPreciseTimingActive();

	const uint64 kEcartes = 30;	  // images de demarrage ecartees
	const size_t kMesures = 1000; // intervalles gardes
	std::vector<double> intervalles;
	intervalles.reserve(kMesures);
	uint64 debutPrecedent = 0;

	uint64 image = 0;
	bool tourne = true;

	while (tourne && fenetre.IsOpen()) {
		// Debut d'image : l'instant present, avant tout le reste
		const uint64 debutImage = (uint64)NkChrono::Now().nanoseconds;
		if (image > kEcartes && intervalles.size() < kMesures) {
			// l'intervalle entre deux debuts d'image, pas la duree du travail
			intervalles.push_back((double)(debutImage - debutPrecedent) * 1e-6);
			if (intervalles.size() >= kMesures) {
				tourne = false;
			}
		}
		debutPrecedent = debutImage;

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

			// Ce que BeginFrame ferait si ce renderer possedait l'image.
			renderer->FlushGraphRebuilds();
			r3d->ResetFrame();
			if (auto *mc = renderer->GetMaterialCollection()) {
				mc->Upload();
			}

			NkCamera3DData cd;
			// A hauteur d'yeux, regardant la porte. Camera fixe.
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

			// La salle, soumise `copies` fois, chaque copie decalee de 5 m le long de x
			for (uint64 k = 0; k < copies; ++k) {
				gDecalage = { 5.f * (float32)k, 0.f, 0.f };
				SoumettreLaSalle(r3d, renderer->GetMeshSystem());
			}
			gDecalage = { 0.f, 0.f, 0.f };

			// Le graphe de la salle s'execute sur le command buffer de l'ecran,
			// AVANT son Present : deux passes imbriquees sont interdites.
			if (auto *graph = renderer->GetRenderGraph()) {
				graph->Execute(cmd);
			}

			// La texture, montree dans la fenetre.
			if (auto *r2d = rEcran->GetRender2D()) {
				r2d->DrawImage(cible->GetColorResult(),
							   { 0.f, 0.f, (float32)devInit.width,
								 (float32)devInit.height });
			}

			rEcran->Present();
			rEcran->EndFrame();
		}

		++image;

		// Le verrou de demi-cadence : fin d'image = debut + DEUX periodes de l'ecran
		if (verrou) {
			AttendreJusqua(debutImage + 2 * periodeNs, minuterieFine);
		}

		// La capture, a une image nommee.
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

	// -- Exo 9 : les resultats, apres la boucle ------------------------------
	if (!intervalles.empty()) {
		double somme = 0.0, maxi = 0.0;
		for (double v : intervalles) {
			somme += v;
			if (v > maxi) maxi = v;
		}
		const double n = (double)intervalles.size();
		const double moyenne = somme / n;
		double carres = 0.0;
		for (double v : intervalles) {
			carres += (v - moyenne) * (v - moyenne);
		}
		const double ecartType = std::sqrt(carres / n);

		std::printf("copies de la salle : %llu\n", (unsigned long long)copies);
		std::printf("%s : moyenne %.3f ms, ecart type %.3f ms, max %.3f ms\n",
					verrou ? "avec verrou" : "sans verrou", moyenne, ecartType, maxi);
		std::printf("[verrou] %llu intervalles, periode de l'ecran %.3f ms (%.3f Hz)\n",
					(unsigned long long)intervalles.size(), 1e3 / hz, hz);
		std::printf("[verrou] minuterie fine : active %d, deja active au lancement %d\n",
					(int)minuterieFine, (int)etaitActive);
	}
	if (!etaitActive) {
		NkChrono::EndPreciseTiming();
	}

	std::printf("[MaSalle] %llu images\n", (unsigned long long)image);

	NkRenderer::Destroy(renderer);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return 0;
}