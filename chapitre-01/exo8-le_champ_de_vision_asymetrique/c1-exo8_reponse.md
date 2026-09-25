# Exercice 8: Le champ de vision asymétrique

## Les quatre angles pou l'œil gauche

Casque utilisé : Valve Index (144 Hz)

| Angle | Valeur |
|---|---|
| Gauche | -54,00° |
| Droite | 42,98° |
| Haut | 54,63° |
| Bas | -54,52° |

## Source

J'ai trouvé une mesure réelle publiée pour le Valve Index (un casque du commerce), obtenue avec l'outil hmdq qui interroge directement le runtime OpenVR/SteamVR de l'appareil.
URL : https://risa2000.github.io/hmdgdb/hmd_cfgs/Index_Native_R144.html

## Ce qui changerait avec un champ symétrique de même surface : 

Un champ symétrique de même surface centrerait artificiellement l'axe de visée entre les deux yeux au lieu de le décaler vers l'extérieur, on perdrait alors une partie du champ périphérique externe précieux pour l'immersion et l'évitement d'objets, au profit d'une zone interne vers le nez qui, de toute façon, n'est jamais visible à cause de l'anatomie du visage, gaspillant des pixels de rendu sur une zone masquée plutôt que de les concentrer là où l'œil regarde réellement.
