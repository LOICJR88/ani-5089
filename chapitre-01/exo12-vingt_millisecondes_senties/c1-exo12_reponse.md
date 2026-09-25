# Exercice 12: Vingt millisecondes senties

## Programme utilisé

Programme qui enregistre en continu la position de la souris avec horodatage, et affiche un point qui suit cette position avec un retard réglable de 0 à 200 ms via un curseur. Le retard est augmenté progressivement pendant que chaque personne déplace la souris, jusqu'à ce qu'elle signale percevoir un décalage.

## Les cinq seuils mesurés

| Personne | Seuil de perception du retard |
|---|---|
| Personne 1 | Léger décalage perceptible dès 5 ms, mais souvent rattrapé et pouvant passer inaperçu ; parfaitement visible à 10 ms |
| Personne 2 | Même comportement : amorce de décalage vers 5 ms, net et évident à 10 ms |
| Personne 3 | Même comportement : amorce de décalage vers 5 ms, net et évident à 10 ms |
| Personne 4 | Décalage net situé plutôt dans la tranche 10-15 ms |
| Personne 5 | Décalage net à 15 ms |

## Comparaison au budget de vingt millisecondes

Les cinq seuils mesurés (entre 5 et 15 ms selon les personnes, avec un décalage clairement établi autour de 10-15 ms pour tout le monde) sont étonnamment proches du budget de 20 ms évoqué au chapitre 1 pour un casque, plus proches que ce à quoi on pourrait s'attendre pour un simple curseur sur écran. Cela dit, il s'agit ici d'un décalage toujours visible et jugé gênant, alors que le seuil de 20 ms du casque est un seuil d'inconfort et de malaise, pas seulement de perception visuelle : les deux ne mesurent pas exactement la même chose.

## Pourquoi le seuil est bien plus bas dans un casque

Ce n'est pas le constat observé, l'œil humain détecte déjà un retard très fin, même sur un simple curseur. Le seuil visuel de détection n'est donc pas si différent entre écran et casque. Ce qui change radicalement, ce n'est pas la sensibilité de détection, mais ce que ce retard déclenche une fois détecté : une gêne visuelle bénigne sur écran, contre un vrai conflit sensoriel potentiellement nauséeux en casque, parce que la tête y est en mouvement réel mesuré par l'oreille interne.
