#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NB_TURN 20
#define NB_CONCEPT 50
#define NB_WORD 1000
#define MAX_SIZE_WORD 15


int u(char* m, char* w) {
  return abs((int) strlen(m) - (int) strlen(w));
}

int t(char* m, char* w) {
  int count = 0;
  for (int i = 0; i < (int) strlen(m); i++) {
    for (int j = 0; j < (int) strlen(w); j++) {
      if (m[i] == w[j])
        count++;
    }
  }
  return count;
}

/*
int score(char* w, char* m, char list_concept[][255], char* list_word[][NB_CONCEPT+1]) {
  int position_w = 0;
  for (int i = 0; i < NB_CONCEPT; i++) {
    if (w == list_concept[i]) {
      position_w = i;
      break;
    }
  }

  for (int i = 0; i < NB_WORD; i++) {
    if (m == list_word[i][0])
      return atoi(list_word[i][position_w+1]);
  }
  return 0;
}
*/

struct mot {
  char* v;
  char* concepts[NB_CONCEPT];
  int score[NB_CONCEPT];
};

struct joueur {
  int J;
  int P;
  int END;
  int ST;
};

/* Méthode pour les tableaux */

int is_char_in_range(char* c, int a, int b, char* tab[]) {
  for (int i = a; i < b; i++) {
    if (c == tab[i]) {
      return true;
    }
  }
  return false;
}

/* FIN Méthode tableaux */


int main(void) {
    // variables
    int NJ, J, ST;

    // Liste totale des concepts

    // liste de concept à chaque tour donné par la déesse
    char* list_turn_concept[NB_TURN];
    for (int i = 0; i < NB_TURN; i++) {
      list_turn_concept[i] = malloc(255 * sizeof(char));
    }

    // Test struct mot
    struct mot liste_mot[NB_WORD];
    for (int i = 0; i < NB_WORD; i++) {
      liste_mot[i].v = malloc(255 * sizeof(char));
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot[i].concepts[j] = malloc(255 * sizeof(char));
      }
    }

    // liste de mot qui change au cours d'une manche
    struct mot liste_mot_candidat[NB_WORD];
    for (int i = 0; i < NB_WORD; i++) {
      liste_mot_candidat[i].v = malloc(255 * sizeof(char));
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot_candidat[i].concepts[j] = malloc(255 * sizeof(char));
      }
    }

    // liste initiale avec tous les mots
    struct mot liste_mot_initiale[NB_WORD];
    for (int i = 0; i < NB_WORD; i++) {
      liste_mot_initiale[i].v = malloc(255 * sizeof(char));
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot_initiale[i].concepts[j] = malloc(255 * sizeof(char));
      }
    }

    int taille_mot_candidat = NB_WORD;


    /*
    // variables de la formule
    int a, b, c; // [1 et 100]
    //int p; // [3, 7]
    // supposition initiale
    a = 50;
    b = 50;
    c = 50;
    // p = 5;
    */

    // Initialisation phase:
    char line[255]; // to optimize

    memset(line, 0, 255);
    scanf("%[^\n]s", line);
    fprintf(stderr, "%s\n", line);
    fflush(stderr);
    // First line
    sscanf(line, "%d %d", &NJ, &J);
    // fprintf(stderr, "Recu : %s\n", *line);
    fprintf(stderr, "Nombre de joueur : %d; Mon numéro de pythie : %d\n", NJ, J);
    fflush(stderr);


    // Récupération de tous les concepts
    for (int i = 0; i < NB_CONCEPT; i++) {
        memset(line, 0, 255);
        scanf("%s", line);

        // ajout des concepts dans chaque mot
        for (int j = 0; j < NB_WORD; j++) {
          sscanf(line, "%s", liste_mot[j].concepts[i]);
        }
    }

    // Récupération de tous les mots
    for (int i = 0; i < NB_WORD; i++) {

        memset(line, 0, 255);
        scanf("%s", line);

        // dans le struct
        sscanf(line, "%s", liste_mot[i].v);

        // fprintf(stderr, "Récupération mot : %s\n", line);
        // fflush(stderr);
        for (int j = 1; j < NB_CONCEPT+1; j++) {
          memset(line, 0, 255);
          scanf("%s", line);

          // dans le struct
          sscanf(line, "%d", &liste_mot[i].score[j-1]);
        }
    }


    // trie concept ordre croissant du score ===> tri à bulle
    for (int i = 0; i < NB_WORD; i++) { // à optimiser
      for (int j = NB_CONCEPT-1; j > 0; j--) {
        for (int k = 0; k < j; k++) {
          if (liste_mot[i].score[k + 1] < liste_mot[i].score[k]) {
            int tmp_i = liste_mot[i].score[k + 1];
            liste_mot[i].score[k + 1] = liste_mot[i].score[k];
            liste_mot[i].score[k] = tmp_i;

            char* tmp_s = malloc(255 * sizeof(char));
            tmp_s = liste_mot[i].concepts[k + 1];
            liste_mot[i].concepts[k + 1] = liste_mot[i].concepts[k];
            liste_mot[i].concepts[k] = tmp_s;
          } else if (liste_mot[i].score[k + 1] == liste_mot[i].score[k]) { // comparaison des caractères
            for (int p = 0; p < (int) strlen(liste_mot[i].concepts[k]); p++) {
              if (liste_mot[i].concepts[k][p] > liste_mot[i].concepts[k+1][p]) { // plus grand k que k+1
                int tmp_i = liste_mot[i].score[k + 1];
                liste_mot[i].score[k + 1] = liste_mot[i].score[k];
                liste_mot[i].score[k] = tmp_i;

                char* tmp_s = malloc(255 * sizeof(char));
                tmp_s = liste_mot[i].concepts[k + 1];
                liste_mot[i].concepts[k + 1] = liste_mot[i].concepts[k];
                liste_mot[i].concepts[k] = tmp_s;
                break;
              } else if (liste_mot[i].concepts[k][p] < liste_mot[i].concepts[k+1][p]) { // plus petit
                break;
              } else { // same char
                continue;
              }
            }
          }
        }
      }
    }

    // mémore dans une copie la liste des mots triées
    for (int i = 0; i < NB_WORD; i++) {
      liste_mot_initiale[i].v = liste_mot[i].v;
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot_initiale[i].concepts[j] = liste_mot[i].concepts[j];
        liste_mot_initiale[i].score[j] = liste_mot[i].score[j];
      }
    }

    /*
    // Affichage struct
    for (int i = 0; i < NB_WORD; i++) {
      for (int j = 0; j < NB_CONCEPT; j++) {
        fprintf(stderr, "mot : %s, con : %s et s : %d\n", liste_mot[i].v, liste_mot[i].concepts[j], liste_mot[i].score[j]);
        fflush(stderr);
      }
    }
    */

    struct joueur liste_joueur[NJ];

    // every round
    for (int manche = 0; manche < 5; manche++) {
      fprintf(stderr, "début de manche n°%d\n", manche);
      fflush(stderr);


      // Récupération information de début de manche
      for (int nb_player = 0; nb_player < NJ; nb_player++) {
        scanf("%d %d %d", &liste_joueur[nb_player].J, &liste_joueur[nb_player].P, &liste_joueur[nb_player].END);

        if (liste_joueur[nb_player].END == 1) {
          liste_joueur[nb_player].ST = 1;
        }

        fprintf(stderr, "Joueur %d, points = %d, sortie ? %d\n", liste_joueur[nb_player].J, liste_joueur[nb_player].P, liste_joueur[nb_player].ST);
        fflush(stderr);
      }

      int tour = 1;
      int counter_concept_recup = 0;
      int is_tous_trouve = 0;
      int me_trouve = 0;

      while(is_tous_trouve == 0) {
        fprintf(stderr, "Tour n°%d\n", tour);
        fflush(stderr);

        if (tour < 21) {
          // Récupération du concept
          memset(line, 0, 255);
          scanf("%s", line);
          sscanf(line, "%s", list_turn_concept[counter_concept_recup]); // mot concept à chaque tour
          fprintf(stderr, "Récupération mot : %s\n", list_turn_concept[counter_concept_recup]);
          fflush(stderr);
        } // fin récupération concept

        // Si j'ai pas trouvé le mot ==> Algo()
        if (liste_joueur[0].ST == 1) {
          // Algo()
          int count_possible_word = 0;

          for (int i = 0; i < taille_mot_candidat; i++) {
            // fprintf(stderr, "Analyse du mot %s\n", liste_mot[i].v);
            //fflush(stderr);
            int partie_basse = 0;

            for (int j = 0; j < 7; j++) {

              if (strcmp(list_turn_concept[counter_concept_recup],liste_mot[i].concepts[j]) == 0) {
                partie_basse = 1;
                break;
              }
            }

            int partie_haute = 0;
            if (partie_basse == 0) {
              for (int j = 33; j < NB_CONCEPT; j++) {

                if (strcmp(list_turn_concept[counter_concept_recup],liste_mot[i].concepts[j]) == 0) {
                  // fprintf(stderr, "%s dans les 7 derniers\n", liste_mot[i].v);
                  // fflush(stderr);
                  partie_haute = 1;
                  break;
                }
              }
            }

            if (partie_basse == 1 || partie_haute == 1) {
              liste_mot_candidat[count_possible_word].v = liste_mot[i].v;
              for (int j = 0; j < NB_CONCEPT; j++) {
                liste_mot_candidat[count_possible_word].concepts[j] = liste_mot[i].concepts[j];
                liste_mot_candidat[count_possible_word].score[j] = liste_mot[i].score[j];
              }
              count_possible_word++;
            }
          }
          for (int i = 0; i < count_possible_word; i++) {
            fprintf(stderr, "mot n°%d : %s\n",i +1, liste_mot_candidat[i].v);
            fflush(stderr);
          }
          taille_mot_candidat = count_possible_word;

          // change the original list by the new one
          // liste_mot = liste_mot_candidat; // marche pas
          for (int i = 0; i < NB_WORD; i++) {
            if (i < count_possible_word) {
              liste_mot[i].v = liste_mot_candidat[i].v;
              for (int j = 0; j < NB_CONCEPT; j++) {
                liste_mot[i].concepts[j] = liste_mot_candidat[i].concepts[j];
                liste_mot[i].score[j] = liste_mot_candidat[i].score[j];
              }
            } else {
              liste_mot[i].v = "";
              for (int j = 0; j < NB_CONCEPT; j++) {
                liste_mot[i].concepts[j] = "";
                liste_mot[i].score[j] = 0;
              }
            }
          }

          fprintf(stderr, "On a récup %d\n", count_possible_word);
          fflush(stderr);

          if (taille_mot_candidat == 1) {
            fprintf(stderr, "Je Guess : %s \n!!!!", liste_mot[0].v);
            fflush(stderr);
            fprintf(stdout, "GUESS %s\n", liste_mot[0].v);
            fflush(stdout);
          } else {
            fprintf(stderr, "Je passe !!!\n");
            fflush(stderr);
            fprintf(stdout, "PASS\n");
            fflush(stdout);
          }

          counter_concept_recup++;
        } // Fin si j'ai pas trouvé le mot
        else {
          // Si j'ai trouvé le mot
          if (me_trouve != 1) { // Affecte qu'une fois le changement à trouvé
            me_trouve = 1;
          }
          if (tour < 21) {
            fprintf(stderr, "Je passe !!!\n");
            fflush(stderr);
            fprintf(stdout, "PASS\n");
            fflush(stdout);
          }
        }
        tour++;

        // Récupération informations joueurs restants et ce qu'ils ont proposés
        for (int nb_player = 0; nb_player < NJ; nb_player++) {
          memset(line, 0, 255);
          scanf("%s %d %d", line, &J, &ST);

          liste_joueur[nb_player].ST = ST;

          fprintf(stderr, "Joueur %d propose %s. Sortie ? %d\n", J, line, ST);
          fflush(stderr);
        }

        // Si tout le monde à trouvé, on passe à la manche suivante
        if (me_trouve == 1 && liste_joueur[0].ST == 1) {
          is_tous_trouve = 1;
        }
      }

      // Remettre la liste_mot à l'initiale
      fprintf(stderr, "réinitialisation!\n");
      fflush(stderr);
      for (int i = 0; i < NB_WORD; i++) {
        liste_mot[i].v = liste_mot_initiale[i].v;
        for (int j = 0; j < NB_CONCEPT; j++) {
          liste_mot[i].concepts[j] = liste_mot_initiale[i].concepts[j];
          liste_mot[i].score[j] = liste_mot_initiale[i].score[j];
        }
      }
      taille_mot_candidat = NB_WORD;
    }

    // Free the malloc
    // liste de concept à chaque tour donné par la déesse
    for (int i = 0; i < NB_TURN; i++) {
      free(list_turn_concept[i]);
    }

    // Test struct mot
    for (int i = 0; i < NB_WORD; i++) {
      free(liste_mot[i].v);
      for (int j = 0; j < NB_CONCEPT; j++) {
        free(liste_mot[i].concepts[j]);
      }
    }

    // liste de mot qui change au cours d'une manche
    for (int i = 0; i < NB_WORD; i++) {
      free(liste_mot_candidat[i].v);
      for (int j = 0; j < NB_CONCEPT; j++) {
        free(liste_mot_candidat[i].concepts[j]);
      }
    }

    // liste initiale avec tous les mots
    for (int i = 0; i < NB_WORD; i++) {
      free(liste_mot_initiale[i].v);
      for (int j = 0; j < NB_CONCEPT; j++) {
        free(liste_mot_initiale[i].concepts[j]);
      }
    }

    return 0;
}
