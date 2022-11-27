// import libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// defined const
#define NB_TURN 20
#define NB_CONCEPT 50
#define NB_WORD 1000
#define MAX_SIZE_WORD 15


/* TODO */
// Factoriser les méthodes de récupération des données
// Gérer le cas où des personnes sont éliminés ==> sinon mauvaise récupération des données et erreur
// optimiser les mallocs (taille des mots & concepts)
// vérifier que tous les mallocs sont free()


/* BEGIN Struct */
struct game_param {
  int NJ;
  int J;
  int END;
};

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

struct param_goddess {
  int p;
  int q;
  int p_founded; // status to know if p is founded or not
  int a;
  int b;
  int c;
};
/* END Struct */


/* BEGIN global variables */

struct param_goddess goddess;
struct mot liste_mot[NB_WORD];
struct mot liste_mot_candidat[NB_WORD];  // liste de mot qui change au cours d'une manche
struct mot liste_mot_initiale[NB_WORD]; // liste initiale avec tous les mots
char* list_turn_concept[NB_TURN];  // liste de concept à chaque tour donné par la déesse

/* END global variables */


/* BEGIN Print methods */

void print_pass() {
  fprintf(stdout, "PASS\n");
  fflush(stdout);
}

void guess_word(char* word) {
  fprintf(stderr, "Je Guess : %s \n!!!!", word);
  fflush(stderr);
  fprintf(stdout, "GUESS %s\n", word);
  fflush(stdout);
}

/* END Print methods */


/* BEGIN Retrieve game infos methods */

void get_every_words_and_concepts() {
  char* line;

  // get the 50 concepts
  for (int i = 0; i < NB_CONCEPT; i++) {
    // memset(line, 0, 255);
    scanf("%s", line);

    // add each concept in every word
    for (int j = 0; j < NB_WORD; j++) {
      sscanf(line, "%s", liste_mot[j].concepts[i]);
    }
  }

  // Récupération de tous les mots
  for (int i = 0; i < NB_WORD; i++) {
    // memset(line, 0, 255);
    scanf("%s", line);
    sscanf(line, "%s", liste_mot[i].v);

    // fprintf(stderr, "Récupération mot : %s\n", line);
    // fflush(stderr);
    for (int j = 1; j < NB_CONCEPT+1; j++) {
      // memset(line, 0, 255);
      scanf("%s", line);

      // dans le struct
      sscanf(line, "%d", &liste_mot[i].score[j-1]);
    }
  }
}

/* END Retrieve game infos methods */


/* BEGIN Methods for array manipulation */

int is_char_in_range(char* c, int a, int b, char* tab[]) {
  for (int i = a; i < b; i++) {
    if (c == tab[i]) {
      return true;
    }
  }
  return false;
}

void print_concepts_words_list() {
  for (int i = 0; i < NB_WORD; i++) {
    for (int j = 0; j < NB_CONCEPT; j++) {
      fprintf(stderr, "mot : %s, con : %s et s : %d\n", liste_mot[i].v, liste_mot[i].concepts[j], liste_mot[i].score[j]);
      fflush(stderr);
    }
  }
}

/* END Methods for array manipulation */

/* Méthode pour le calcul des paramètres de la déesse */

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

/* FIN Méthode paramètre déesse */

/* BEGIN Methods for game's array */

void sort_concept_by_score_asc() {
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
}

void reset_list_word_to_init() {
  for (int i = 0; i < NB_WORD; i++) {
    liste_mot[i].v = liste_mot_initiale[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      liste_mot[i].concepts[j] = liste_mot_initiale[i].concepts[j];
      liste_mot[i].score[j] = liste_mot_initiale[i].score[j];
    }
  }
}

void reduce_possible_list_with_concept(int* taille_mot_candidat, int counter_concept_recup) {
  // params :
  // & counter_concept_recup
  // & taille_mot_candidat

  int count_possible_word = 0;

  for (int i = 0; i < *taille_mot_candidat; i++) {
    // fprintf(stderr, "Analyse du mot %s\n", liste_mot[i].v);
    //fflush(stderr);

    int partie_basse = 0;
    for (int j = 0; j < 10-goddess.p; j++) {

      if (strcmp(list_turn_concept[counter_concept_recup],liste_mot[i].concepts[j]) == 0) {
        partie_basse = 1;
        break;
      }
    }

    int partie_haute = 0;
    if (partie_basse == 0) {
      for (int j = NB_CONCEPT - (10 + goddess.q); j < NB_CONCEPT; j++) {

        if (strcmp(list_turn_concept[counter_concept_recup], liste_mot[i].concepts[j]) == 0) {
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
    // fprintf(stderr, "mot n°%d : %s\n",i +1, liste_mot_candidat[i].v);
    // fflush(stderr);
  }
  *taille_mot_candidat = count_possible_word;

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
}

// TODO : A optimiser en recherche dichotomique
int find_word_position(char* word) {
  for (int i = 0; i < NB_WORD; i++) {
    if (strcmp(word, liste_mot_initiale[i].v) == 0) {
      fprintf(stderr, "secret w : %s\n", liste_mot_initiale[i].v);
      fflush(stderr);
      return i;
    }
  }
}

void algo_find_p(char* secret_word) { // TODO : A optimiser
  int sw_pos = find_word_position(secret_word);
  int tab[NB_CONCEPT] = {0};

  fprintf(stderr, "secret wooord : %s\n", secret_word);
  fflush(stderr);

  fprintf(stderr, "secret wooord dans liste initi : %s\n", liste_mot_initiale[sw_pos].v);
  fflush(stderr);

  for (int i = 0; i < NB_CONCEPT; i++) {
    fprintf(stderr, "concept du mot :%s\n", liste_mot_initiale[sw_pos].concepts[i]);
    fflush(stderr);

  }

  // Si on est à la manche 1 : on trouve plus facilement p si le concept est à la 7ème ou 33ème position
  for (int i = 0; i < NB_TURN; i++) {
    int founded = 0;

    for (int j = 0; j < NB_CONCEPT; j++) {
      if (strcmp(list_turn_concept[i], liste_mot_initiale[sw_pos].concepts[j]) == 0) {
        tab[j] = 1;
      }
    }
  }

  // print tab
  fprintf(stderr, "[");
  fflush(stderr);
  for (int i = 0; i < NB_CONCEPT; i++) {
    fprintf(stderr, "%d, ", tab[i]);
    fflush(stderr);
  }
  fprintf(stderr, "]\n");
  fflush(stderr);


  int count = 0;
  for (int i = 0; i < 10 - goddess.p; i++) {
    if (tab[i] == 1)
      count++;
    else
      break;
  }

  goddess.p = 10 - count;
  goddess.q = 10 - count;
  goddess.p_founded = 1;
}

/* END Methods for game's array */


/* BEGIN Decision methods */

void print_decision(int taille_mot_candidat, int* has_proposed, char** word_proposed, int manche, int tour) {
  if (manche == 0 && tour < 20) { // Get all the concept first
    fprintf(stderr, "Je passe !!!\n");
    fflush(stderr);
    print_pass();
  } else {
    if (taille_mot_candidat == 2) {
      *word_proposed = liste_mot[0].v;
      guess_word(liste_mot[0].v);
      *has_proposed = 1;
    } else if (taille_mot_candidat == 1) {
      *word_proposed = liste_mot[0].v;
      guess_word(liste_mot[0].v);
      *has_proposed = 1;
    } else {
      fprintf(stderr, "Je passe !!!\n");
      fflush(stderr);
      print_pass();
    }
  }
}

/* END Decision methods */


/* BEGIN Initialisation & free mallocs methods */

void init_global_variables() {
  // Init goddess params
  goddess.p = 3;
  goddess.q = 7;
  goddess.p_founded = 0;

  // Init arrays
  for (int i = 0; i < NB_TURN; i++) {
    list_turn_concept[i] = malloc(255 * sizeof(char));
  }

  for (int i = 0; i < NB_WORD; i++) {
    liste_mot[i].v = malloc(255 * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      liste_mot[i].concepts[j] = malloc(255 * sizeof(char));
    }
  }

  for (int i = 0; i < NB_WORD; i++) {
    liste_mot_candidat[i].v = malloc(255 * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      liste_mot_candidat[i].concepts[j] = malloc(255 * sizeof(char));
    }
  }

  for (int i = 0; i < NB_WORD; i++) {
    liste_mot_initiale[i].v = malloc(255 * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      liste_mot_initiale[i].concepts[j] = malloc(255 * sizeof(char));
    }
  }
}

void free_global_variables() {
  for (int i = 0; i < NB_TURN; i++) {
    free(list_turn_concept[i]);
  }

  for (int i = 0; i < NB_WORD; i++) {
    free(liste_mot[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      free(liste_mot[i].concepts[j]);
    }
  }

  for (int i = 0; i < NB_WORD; i++) {
    free(liste_mot_candidat[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      free(liste_mot_candidat[i].concepts[j]);
    }
  }

  for (int i = 0; i < NB_WORD; i++) {
    free(liste_mot_initiale[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      free(liste_mot_initiale[i].concepts[j]);
    }
  }
}

/* END Initialisation & free mallocs methods */


int main(void) {
    // variables
    int J, ST;
    struct game_param game;


    // Liste totale des concepts

    init_global_variables(); // malloc for every array


    int taille_mot_candidat = NB_WORD;

    // Initialisation phase:
    char* line = malloc(255 * sizeof(char)); // to optimize

    // get game infos
    scanf(" %[^\n]s\n", line);
    sscanf(line, "%d %d", &game.NJ, &game.J);
    fprintf(stderr, "Récup game infos : %s\n", line);
    fflush(stderr);

    // get every words & concepts
    get_every_words_and_concepts();


    // trie concept ordre croissant du score ===> tri à bulle
    sort_concept_by_score_asc();

    // mémoire dans une copie la liste des mots triées
    for (int i = 0; i < NB_WORD; i++) {
      liste_mot_initiale[i].v = liste_mot[i].v;
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot_initiale[i].concepts[j] = liste_mot[i].concepts[j];
        liste_mot_initiale[i].score[j] = liste_mot[i].score[j];
      }
    }

    // print_concepts_words_list(); // Check words & concepts associated

    struct joueur liste_joueur[game.NJ]; // array of player's struct

    // BEGIN GAME
    // every round
    for (int manche = 0; manche < 5; manche++) {
      if (manche == 0) {
        fprintf(stderr, "début de manche hehehehn°%d\n", manche);
        fflush(stderr);


        // Récupération information de début de manche
        for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
          int J, P, END;

          scanf(" %[^\n]s\n", line);
          sscanf(line, "%d %d %d", &J, &P, &END);
          fprintf(stderr, "Récup toute la ligne : %s\n", line);
          fflush(stderr);

          liste_joueur[J].J = J;
          liste_joueur[J].P = P;
          liste_joueur[J].END = END;

          fprintf(stderr, "Récup Joueur J, P, END : %d %d %d\n", J, P, END);
          fflush(stderr);


          // scanf("%d %d %d", &liste_joueur[nb_player].J, &liste_joueur[nb_player].P, &liste_joueur[nb_player].END);

          if (liste_joueur[J].END == 1) {
            liste_joueur[J].ST = 1;
          }
        }

        for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
          fprintf(stderr, "Joueur %d, points = %d, sortie ? %d\n", liste_joueur[nb_player].J, liste_joueur[nb_player].P, liste_joueur[nb_player].ST);
          fflush(stderr);
        }
      }

      fprintf(stderr, "deviner p ::: %d\n", goddess.p);
      fflush(stderr);
      fprintf(stderr, "deviner q ::: %d\n", goddess.q);
      fflush(stderr);


      int tour = 1;
      int counter_concept_recup = 0;
      int is_tous_trouve = 0;
      int me_trouve = 0;
      int has_proposed = 0;
      int is_player_removed = 0;
      char* word_proposed = malloc(255 * sizeof(char));


      // fprintf(stderr, "On a trouvé p : %d\n", goddess.p);
      // fflush(stderr);

      while(is_tous_trouve == 0 || tour < 21) {
        fprintf(stderr, "Tour n°%d\n", tour);
        fflush(stderr);

        if (tour < 21) {
          // Récupération du concept
          memset(line, 0, 255);
          scanf("%s", line);
          sscanf(line, "%s", list_turn_concept[counter_concept_recup]); // mot concept à chaque tour
          counter_concept_recup++;
          fprintf(stderr, "Récupération mot : %s\n", list_turn_concept[counter_concept_recup-1]);
          fflush(stderr);
        } // fin récupération concept


        // Si j'ai proposé quelque chose
        if (has_proposed == 1 && me_trouve == 0) {
          has_proposed = 0;
          fprintf(stderr, "J'ai proposé un truc faux frérot : %s \n!!!!", liste_mot[0].v);
          fflush(stderr);
          fprintf(stderr, "Du coup je Guess l'autre mot : %s \n!!!!", liste_mot[1].v);
          fflush(stderr);
          fprintf(stdout, "GUESS %s\n", liste_mot[1].v);
          fflush(stdout);
          word_proposed = liste_mot[1].v;
        } else { // si j'ai PAS proposé quelque chose
          if (me_trouve == 0) { // Si j'ai pas trouvé le mot
            reduce_possible_list_with_concept(&taille_mot_candidat, counter_concept_recup-1); // algo() ; counter_concept_recup-1 == last concept's position

            // fprintf(stderr, "On a récup %d\n", count_possible_word);
            // fflush(stderr);

            print_decision(taille_mot_candidat, &has_proposed, &word_proposed, manche, tour); // Send decision
            fprintf(stderr, "mot proposé récup : %s\n", word_proposed);
            fflush(stderr);

          } else { // Si j'ai trouvé le mot
            if (tour < 21) { // continue to pass until getting all concepts (20 turns)
              print_pass();
            }
          }
        }

        tour++;


        // Récupération informations joueurs restants et ce qu'ils ont proposés
        for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
          int sec_info; // P or J ?
          int third_info; // END or ST ?

          // test
          char* first_string = malloc(255 * sizeof(char));
          scanf(" %[^\n]s\n", line);
          sscanf(line, "%s %d %d", first_string, &sec_info, &third_info);

          // TODO : A factoriser
          if (third_info == 2) { // I found the word
            if (me_trouve == 0) {
              me_trouve = 1;
              has_proposed = 0;
            }
            liste_joueur[sec_info].ST = third_info;
          } else if (me_trouve == 1 && third_info == 1) { // new round
            int player_i = atoi(first_string);
            liste_joueur[player_i].P = sec_info;
            liste_joueur[player_i].ST = third_info;
            if (is_tous_trouve == 0)
              is_tous_trouve = 1;
          } else {
            if (third_info == 0) { // player got removed
              if (is_player_removed == 0)
                is_player_removed = 1;
            }
            liste_joueur[sec_info].ST = third_info;
          }
        }
      }


      if (manche == 0) { // Guess the p param if at manche 0
        algo_find_p(word_proposed);
        free(word_proposed);
      }



      reset_list_word_to_init(); // Remettre la liste_mot à l'initiale
      taille_mot_candidat = NB_WORD;
    }
    // END GAME

    free_global_variables(); // Free the malloc

    return 0;
}
