# Exercice 6: La pire image

## Programme utilisé

J'utilise une boucle de rendu minimale qui, Pour chaque image dans 1000 images au total, efface le canevas et redessine un rectangle plein.

## Mesures

| Mesure | Valeur |
|---|---|
| Durée de la plus longue image | 10,438 ms |
| Nombre d'images au dessus de 11 ms | 0 image sur 1000 |

## Ce programme tiendrait-il dans un casque ?

Sur ce test précis, le programme reste sous le seuil de 11 ms même dans son pire cas, mais ce résultat ne veut pas dire qu'il tiendrait réellement dans un casque. La marge est quasi nulle, 10,438 ms sur un budget de 11 ms, c'est une marge de moins de 0,6 ms alors que le programme ne fait rien d'autre qu'effacer l'écran. Le chapitre 1 rappelle que ce qui reste vraiment au code, une fois passées les étapes capteurs, transmission, composition et affichage tourne autour de 5 à 11 ms selon la cadence, et ici, la simple boucle de rendu consomme déjà la quasi totalité de ce budget, avant même d'y ajouter un rendu.

**Conclusion : non, ce programme ne tiendrait pas dans un casque tel quel.** Le résultat est numériquement dans le budget, mais la marge est trop fine 
