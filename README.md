# Reste à faire

- Créer des prototypes des fonctions et les placer dans l'ordre en haut :  
  - Cela permettra de placer les définitions des fonctions dans n'importe quel ordre.
- Dans les fonctions, enlever les mallocs inutiles et les remplacer par des tableaux.
- Créer la documentation.
- Ajouter la complexité dans la doc des fonctions.
- ... à définir  
  
  
Implémenter ia_algo4(), voici sa logique :
- Réduit les mots comme ia_algo1()
- new_candidat_list = ... (initialisation)
- a, b, c = [1, 100]
- Pour chaque mot restant :
  - remove_word = 0
  - Récupère le dernier concept donné par la déesse, c_last
  - Calcul (t, u) de c_last avec mot_i
  - Calcul t_val_true avec (a, b, c) borné
  - Pour tous les concepts possibles (10-p et 10+p concepts) sauf les n concepts donnée par la déesse :
    - Calcul (t_j, u_j)
    - Si (t, u) == (t_j, u_j) :
      - Calcul t_val_j avec (a, b, c) borné
      - Si tous les t_val_j > t_val_true :
        - remove_word = 1
  - Si remove_word != 1 :
    - Copie le mot  dans new_candidat_list
- Copie de new_candidat_list dans algo4.candidat_list.list
