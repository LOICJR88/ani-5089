# Démo 4: Le retard réglable

Le programme à retard réglable de l'exercice 12 est projeté à l'écran devant toute la classe. Trois volontaires viennent, chacun à leur tour, bouger la souris devant le groupe pendant que le retard est augmenté progressivement par paliers de 5 ms, en partant de 0. Chaque volontaire doit dire stop dès qu'il perçoit un décalage, sans connaître la valeur affichée à l'avance.

## Déroulement et seuils simulés

**Volontaire 1** : reste silencieux jusqu'à 10 ms, puis annonce un décalage net. En redescendant à 5 ms pour vérifier, il hésite et dit qu'il n'est plus sûr de le voir. seuil retenu : 10 ms.

**Volontaire 2** : réagit plus tôt que les deux autres, dès 5 ms, en disant percevoir quelque chose qui traîne un peu sans certitude totale. il confirme un décalage net et incontestable à 10 ms. seuil retenu : 5 ms (perception incertaine) à 10 ms (perception certaine).

**Volontaire 3** : ne signale rien avant 15 ms, où il annonce enfin un décalage clair. La classe s'étonne qu'il n'ait rien vu aux paliers précédents alors que les deux premiers volontaires réagissaient déjà. seuil retenu : 15 ms.

## Réaction de la classe

L'écart entre les trois seuils (10, 5-10, et 15 ms) suscite une discussion spontanée : certains élèves demandent si le troisième volontaire a une vue moins fine, pendant que d'autres remarquent qu'ils auraient eux-mêmes réagi à des paliers différents. La variabilité individuelle, déjà observée à la démo précédente pour le mal des transports, revient ici sous une autre forme : la sensibilité à un simple retard visuel diffère aussi d'une personne à l'autre.

## Conclusion sur le budget d'une image

Ces trois seuils (5 à 15 ms) sont déjà très proches du budget de 20 ms fixé pour un casque, alors qu'il ne s'agit ici que d'un retard visuel isolé sur un curseur de souris, sans aucun conflit avec l'oreille interne. Cela implique une conséquence importante pour le budget d'une image : concevoir pour la moyenne des utilisateurs ne suffit pas. Si le volontaire le plus sensible détecte déjà un décalage à 5 ms, viser un budget qui flirte avec 15 ou 20 ms garantit qu'une partie du public percevra un défaut, même avant tout risque de malaise. Le budget de 20 ms n'est donc pas une marge confortable pour ne rien voir, c'est déjà, pour les utilisateurs les plus sensibles, une valeur au-delà de laquelle quelque chose est visible. sa justification tient au seuil du conflit sensoriel, pas à l'absence totale de perception du retard.
