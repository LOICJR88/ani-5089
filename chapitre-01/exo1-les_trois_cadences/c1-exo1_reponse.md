# Exercice 1 — Les trois cadences

## Durée d'une image

| Cadence | Durée d'une image |
|---|---|
| 72 Hz | 13,9 ms |
| 90 Hz | 11,1 ms |
| 120 Hz | 8,3 ms |

## Part fixe (capteurs, transmission, composition, affichage)

8,0 ms, à chaque cadence.

## Ce qu'il reste à mon code

| Cadence | Durée d'une image − 8 ms | Reste pour le code |
|---|---|---|
| 72 Hz | 13,9 − 8,0 | **5,9 ms** |
| 90 Hz | 11,1 − 8,0 | **3,1 ms** |
| 120 Hz | 8,3 − 8,0 | **0,3 ms** |

## Ce que ça veut dire

Plus la cadence monte, plus la marge disponible pour la logique et le rendu de l'application se resserre — jusqu'à devenir quasi nulle à 120 Hz. Ce n'est pas une moyenne à tenir : c'est une échéance à respecter sur chaque image, sans exception.