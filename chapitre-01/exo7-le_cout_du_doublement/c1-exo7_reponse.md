# Exercice 7: Le coût du doublement

## Mesure du rendu seul

Le temps de rendu moyen que prends une image est de 1,312 ms
Le temps de rendu maximal qu'a prise une image est de 4,119 ms

## Estimation du rendu fait deux fois

Le rendu moyen fait deux fois: 2,623 ms
Le rendu maximal fait deux fois : 8,238 ms

## Ce qu'il resterait pour le reste

Avec le rendu moyen il resterai 8,377 ms
Avec le rendu maximal il resterai 2,762 ms

## Conclusion

L'écart entre le cas moyen et le pire cas est spectaculaire : en moyenne, doubler le rendu ne coûte que 2,623 ms et laisse une marge confortable de 8,377 ms pour le reste. Mais dans le pire cas mesuré, le rendu doublé grimpe à 8,238 ms, avec le budget total de 11 ms, ne laissant plus que 2,762 ms pour absolument tout le reste de l'image.

C'est exactement l'avertissement du chapitre 1 : une expérience en casque se juge à sa pire image, pas à sa moyenne. Si on dimensionne l'application sur le cas moyen, le pire cas risque de faire dépasser l'échéance dès qu'une image un peu plus lourde que la moyenne se présente.

**Ce qu'il faudrait réduire en priorité est la variance du rendu, pas seulement sa moyenne.**
