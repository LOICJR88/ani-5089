# Démo 1 — Le budget au tableau

## Placement initial par la classe (avant révélation)

Le premier réflexe de la classe est de répartir les cinq étapes à peu près à égale distance les unes des autres sur toute la longueur de la barre, chacune occupant environ 4 cm. L'étape application est souvent placée en dernier ou avec le plus grand segment, sur l'intuition que c'est elle qui coûte le plus cher visuellement, alors que les étapes de capteurs et de transmission sont sous-estimées, placées avec des segments très fins, comme si elles étaient quasi instantanées.

## Proportions réelles révélées

| Étape | Durée | Position sur la barre |
|---|---|---|
| Capteurs | 1-2 ms | de 0 - 1,5 cm |
| Transmission | 1-3 ms | 1,5 - 3,5 cm |
| Application | 5-11 ms | 3,5 - 11,5 cm |
| Compositeur | 1-2 ms | 11,5 - 13 cm |
| Écran | 2-5 ms | 13 - 16,5 cm |

Une fois les vraies proportions dévoilées, on redessine les segments à l'échelle : les quatre étapes matérielles (capteurs, transmission, compositeur, écran) sont toutes fines et se regroupent presque en un seul bloc compact, alors que l'étape application à elle seule occupe presque la moitié de la barre entière.

## Réactions de la classe

La surprise porte surtout sur deux points : d'abord, la classe découvre que les quatre étapes matérielles réunies (capteurs + transmission + compositeur + écran, soit environ 8,5 ms au maximum) pèsent presque aussi lourd que la seule étape applicative, alors qu'elle semblait intuitivement anecdotique face au vrai travail de rendu. Ensuite, plusieurs remarquent qu'il ne reste, une fois les cinq segments placés bout à bout, qu'une marge très mince avant d'atteindre les 20 ms — et qu'un dépassement, même léger, de l'étape applicative suffirait à consommer toute la marge de sécurité.

## Ce que la démo fait ressortir

Une fois les cinq segments placés à l'échelle, il ne reste qu'environ 3,5 ms de marge avant d'atteindre les 20 ms totaux, l'essentiel du budget étant déjà englouti par les cinq étapes elles-mêmes. C'est le point central du chapitre 1 : la contrainte des vingt millisecondes n'est pas un objectif confortable mais une échéance stricte, où même le poste le plus généreux (l'application, à 5-11 ms) ne laisse quasiment aucune place à l'erreur une fois combiné aux quatre autres étapes matérielles, incompressibles.
