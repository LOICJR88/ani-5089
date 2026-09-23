# Exercice 2: Le tableau des budgets

##Etape 01 : Les Capteurs mesurent le mouvement

La source https://vrarwiki.com/wiki/Motion-to-photon_latency situe l'échantillonnage des capteurs autour de 1 à 2 ms pour les centrales inertielles (IMU), contre 15 à 33 ms pour une caméra, qui est beaucoup plus lente. C'est cohérent avec le chiffre du cours à condition qu'il s'agisse bien d'un capteur inertiel et non d'une caméra de tracking.

## Ce que je n'ai pas trouvé

Aucune source ne donne une valeur isolée et universelle pour "transmission des données du capteur au CPU" seule — les documentations et articles trouvés la fusionnent presque toujours avec le calcul de pose (étape 2 et 3 confondues). De même, aucune documentation constructeur consultée (Meta) ne publie de valeur fixe en millisecondes pour le temps de composition seul : ils exposent des métriques de mesure en temps réel (TimeWarp to Mid-Photon, Flip to Photon) plutôt qu'un chiffre de référence, parce que ce temps dépend du GPU et du casque utilisés.

## Sources

- vrarwiki.com — "Motion-to-photon latency", tableau de répartition du budget de latence
- developers.meta.com — Documentation Meta Horizon PC SDK, "Performance Head-Up Display" et "dg-performance-stats"
- uploadvr.com — "Meta Revealed The Detailed Specs Of Quest 2's LCD Display"
