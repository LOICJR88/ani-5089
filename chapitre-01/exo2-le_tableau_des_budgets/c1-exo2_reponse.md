# Exercice 2: Le tableau des budgets

-Etape 01 : Les Capteurs mesurent le mouvement

vrarwiki.com situe l'échantillonnage des capteurs autour de 1 à 2 ms pour les centrales inertielles (IMU), contre 15 à 33 ms pour une caméra, qui est beaucoup plus lente. C'est cohérent avec le chiffre du cours à condition qu'il s'agisse bien d'un capteur inertiel et non d'une caméra de tracking.

-Etape 02 : Le Système transmet la mesure

Je ne trouve aucune source qui donne une valeur isolée et universelle pour cette étape seule, vrarwiki.com la fusionne avec le calcul de pose (6 degrés de liberté).

-Etape 03 : L'application décide et dessine

Toujours à la même source, le traitement CPU par le moteur applicatif vise généralement moins de 5 à 10 ms, un ordre de grandeur qui correspond bien à celui du cours.

-Etape 04 : Le compositeur assemble

Je ne trouve aucune documentation qui ne publie de valeur fixe en millisecondes pour le temps de composition seul. vrarwiki.com expose des métriques de mesure en temps réel (TimeWarp to Mid-Photon, Flip to Photon) plutôt qu'un chiffre de référence, parce que ce temps dépend du GPU et du casque utilisés.

-Etape 05 : L'écran affiche la ligne

Ici les sources que je trouve divergent selon la technologie d'écran. Un article sur uploadvr.com explique que les écrans OLED, contrairement aux LCD, s'illuminent eux-mêmes et ont un temps de réponse extrêmement rapide, ce qui permettait de les éteindre pendant la majeure partie de l'image pour éviter le flou de mouvement. Un LCD typique de casque grand public a un temps de réponse nettement plus lent que ce qu'indique le cours pour cette étape.

| Étape | Valeur du cours | Valeur mesurée trouvée | Source |
|---|---|---|---|
| Le Capteurs mesurent le mouvement | 1-2 ms | 1-2 ms (IMU) ; 15-33 ms (caméra) | vrarwiki.com, "Motion-to-photon latency" |
| Le Système transmet la mesure | 1-3 ms | 1-4 ms | vrarwiki.com, "Motion-to-photon latency" |
| L'Application décide et dessine | 5-11 ms | < 5-10 ms (cible typique) | vrarwiki.com, "Motion-to-photon latency" |
| Le Compositeur assemble | 1-2 ms | 1-2 ms (pas de valeur fixe officielle trouvée : dépend du matériel) | vrarwiki.com ; Meta Horizon Developer Docs, "Performance Head-Up Display" (métriques TimeWarp to Mid-Photon, Flip to Photon — pas de chiffre absolu) |
| L'Écran affiche la ligne | 2-5 ms | Variable selon la dalle : OLED quasi instantané (réponse pixel très rapide), LCD nettement plus lent | UploadVR, "Meta Revealed The Detailed Specs Of Quest 2's LCD Display" |
