// import libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// defined const
#define NB_TURN 20
#define NB_CONCEPT 50
#define NB_WORD 1000

// const for string size
#define MAX_SIZE_WORD 15
#define MAX_SIZE_CONCEPT 30

// const for precision of goddess
#define NB_COEFF 4
#define COEFF {5,35,65,95}


/* TODO */
// Factoriser les méthodes de récupération des données
// Gérer le cas où des personnes sont éliminés ==> sinon mauvaise récupération des données et erreur
// optimiser les mallocs (taille des mots & concepts)
// vérifier que tous les mallocs sont free()
// remplacer dans les fonctions les tableaux en tableaux dynamiques


/* BEGIN Struct */
struct game_param {
  int NJ;
  int J;
  int END;
};

struct word {
  char* v;
  char* concepts[NB_CONCEPT];
  int score[NB_CONCEPT];
};

struct list_word {
  struct word* list;
  int size_list;
};

struct joueur {
  int J;
  int P;
  int END;
  int ST;
};

struct coeff_goddess { // TODO : à améliorer
  int list_possible_coeff[NB_COEFF*NB_COEFF*NB_COEFF][3];
  int size_list;
};

struct param_goddess {
  int p;
  int q;
  int p_founded; // status to know if p is founded or not
  struct coeff_goddess coeff;
};
/* END Struct */


/* BEGIN global variables */

struct param_goddess goddess;
struct list_word* liste_mot;
struct list_word* liste_mot_initiale; // liste initiale triée avec tous les mots
struct word liste_mot_candidat[NB_WORD];  // liste de mot qui change au cours d'une manche
char* list_turn_concept[NB_TURN];  // liste de concept à chaque tour donné par la déesse

/* END global variables */


/* BEGIN Print methods */

void print_pass() {
  fprintf(stdout, "PASS\n");
  fflush(stdout);
}

void guess_word(char* word) {
  // fprintf(stderr, "Je Guess : %s \n!!!!", word);
  // fflush(stderr);
  fprintf(stdout, "GUESS %s\n", word);
  fflush(stdout);
}

/* END Print methods */


/* BEGIN Retrieve game infos methods */

void get_every_words_and_concepts() {
  char* line = malloc(255 * sizeof(char));

  // get the 50 concepts
  for (int i = 0; i < NB_CONCEPT; i++) {
    // memset(line, 0, 255);
    scanf("%s", line);

    // add each concept in every word
    for (int j = 0; j < NB_WORD; j++) {
      sscanf(line, "%s", liste_mot->list[j].concepts[i]);
    }
  }

  // Récupération de tous les mots
  for (int i = 0; i < NB_WORD; i++) {
    // memset(line, 0, 255);
    scanf("%s", line);
    sscanf(line, "%s", liste_mot->list[i].v);

    // fprintf(stderr, "Récupération mot : %s\n", line);
    // fflush(stderr);
    for (int j = 1; j < NB_CONCEPT+1; j++) {
      // memset(line, 0, 255);
      scanf("%s", line);

      // dans le struct
      sscanf(line, "%d", &liste_mot->list[i].score[j-1]);
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
      fprintf(stderr, "mot : %s, con : %s et s : %d\n", liste_mot->list[i].v, liste_mot->list[i].concepts[j], liste_mot->list[i].score[j]);
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
      if (m[i] == w[j] && m[i] != '\0')
        count++;
    }
  }
  return count;
}

/* FIN Méthode paramètre déesse */

/* BEGIN Methods for game's array */

// TODO : A optimiser en recherche dichotomique
int find_word_position(char* word) {
  for (int i = 0; i < NB_WORD; i++) {
    if (strcmp(word, liste_mot_initiale->list[i].v) == 0) {
      // fprintf(stderr, "secret w : %s\n", liste_mot_initiale->list[i].v);
      // fflush(stderr);
      return i;
    }
  }
  return -1; // TODO : tester l'erreur
}

int find_concept_position(char* concept) {
  for (int i = 0; i < NB_CONCEPT; i++) {
    if (strcmp(concept, liste_mot_initiale->list[0].concepts[i]) == 0) {
      return i;
    }
  }
  return -1; // TODO : tester l'erreur
}

int find_score_concept_with_word_position(char* concept, int w_pos) {
  for (int i = 0; i < NB_CONCEPT; i++) {
    if (strcmp(liste_mot_initiale->list[w_pos].concepts[i], concept)) {
      return liste_mot_initiale->list[w_pos].score[i];
    }
  }
  return -1; // TODO : tester l'erreur
}

int find_word_index_in_string_array(char** l, int l_size, char* word) {
  for (int i = 0; i < l_size; i++) {
    if (strcmp(word, l[i]) == 0) {
      return i;
    }
  }
  return -1;
}

void remove_string_from_struct_word_by_index(struct list_word* l, int index) {
  int l_size = l->size_list;
  for (int i = index; i < l_size-1; i++) {
    l->list[i].v = l->list[i+1].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      l->list[i].concepts[j] = l->list[i+1].concepts[j];
      l->list[i].score[j] = l->list[i+1].score[j];
    }
  }
  /*
  free(l->list[l_size-1].v);
  for (int i = 0; i < NB_CONCEPT; i++) {
    free(l->list[l_size-1].concepts[i]);
  }*/
  l->size_list = l_size - 1;
}

void sort_concept_by_score_asc() {
  for (int i = 0; i < NB_WORD; i++) { // à optimiser
    for (int j = NB_CONCEPT-1; j > 0; j--) {
      for (int k = 0; k < j; k++) {
        if (liste_mot->list[i].score[k + 1] < liste_mot->list[i].score[k]) {
          int tmp_i = liste_mot->list[i].score[k + 1];
          liste_mot->list[i].score[k + 1] = liste_mot->list[i].score[k];
          liste_mot->list[i].score[k] = tmp_i;

          char* tmp_s = malloc(255 * sizeof(char));
          tmp_s = liste_mot->list[i].concepts[k + 1];
          liste_mot->list[i].concepts[k + 1] = liste_mot->list[i].concepts[k];
          liste_mot->list[i].concepts[k] = tmp_s;
        } else if (liste_mot->list[i].score[k + 1] == liste_mot->list[i].score[k]) { // comparaison des caractères
          for (int p = 0; p < (int) strlen(liste_mot->list[i].concepts[k]); p++) {
            if (liste_mot->list[i].concepts[k][p] > liste_mot->list[i].concepts[k+1][p]) { // plus grand k que k+1
              int tmp_i = liste_mot->list[i].score[k + 1];
              liste_mot->list[i].score[k + 1] = liste_mot->list[i].score[k];
              liste_mot->list[i].score[k] = tmp_i;

              char* tmp_s = malloc(255 * sizeof(char));
              tmp_s = liste_mot->list[i].concepts[k + 1];
              liste_mot->list[i].concepts[k + 1] = liste_mot->list[i].concepts[k];
              liste_mot->list[i].concepts[k] = tmp_s;
              break;
            } else if (liste_mot->list[i].concepts[k][p] < liste_mot->list[i].concepts[k+1][p]) { // plus petit
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
    liste_mot->list[i].v = liste_mot_initiale->list[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      liste_mot->list[i].concepts[j] = liste_mot_initiale->list[i].concepts[j];
      liste_mot->list[i].score[j] = liste_mot_initiale->list[i].score[j];
    }
  }
  liste_mot->size_list = NB_WORD;
}

void reduce_possible_list_with_concept(int counter_concept_recup) {
  // params :
  // & counter_concept_recup
  fprintf(stderr, "algo1 je commence avec %d\n", liste_mot->size_list);
  fflush(stderr);

  for (int i = 0; i < liste_mot->size_list; i++) {
    // fprintf(stderr, "Analyse du mot %s\n", liste_mot->list[i].v);
    //fflush(stderr);

    int partie_basse = 0;
    for (int j = 0; j < 10-goddess.p; j++) {

      if (strcmp(list_turn_concept[counter_concept_recup],liste_mot->list[i].concepts[j]) == 0) {
        partie_basse = 1;
        break;
      }
    }

    int partie_haute = 0;
    if (partie_basse == 0) {
      for (int j = NB_CONCEPT - (10 + goddess.q); j < NB_CONCEPT; j++) {

        if (strcmp(list_turn_concept[counter_concept_recup], liste_mot->list[i].concepts[j]) == 0) {
          // fprintf(stderr, "%s dans les 7 derniers\n", liste_mot->list[i].v);
          // fflush(stderr);
          partie_haute = 1;
          break;
        }
      }
    }

    if (partie_basse == 0 && partie_haute == 0) {
      //fprintf(stderr, "mot enlevé : %s\n", liste_mot->list[i].v);
      //fflush(stderr);
      remove_string_from_struct_word_by_index(liste_mot, i);
      i--;
    }

    /*
    if (partie_basse == 1 || partie_haute == 1) {


      liste_mot_candidat[count_possible_word].v = liste_mot->list[i].v;
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot_candidat[count_possible_word].concepts[j] = liste_mot->list[i].concepts[j];
        liste_mot_candidat[count_possible_word].score[j] = liste_mot->list[i].score[j];
      }
      count_possible_word++;
    }
    */
  }

  // liste_mot->size_list = count_possible_word;
  /*
  // change the original list by the new one
  // liste_mot = liste_mot_candidat; // marche pas
  for (int i = 0; i < NB_WORD; i++) {
    if (i < count_possible_word) {
      liste_mot->list[i].v = liste_mot_candidat[i].v;
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot->list[i].concepts[j] = liste_mot_candidat[i].concepts[j];
        liste_mot->list[i].score[j] = liste_mot_candidat[i].score[j];
      }
    } else {
      liste_mot->list[i].v = "";
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot->list[i].concepts[j] = "";
        liste_mot->list[i].score[j] = 0;
      }
    }
  }
  */
  /*
  for (int i = 0; i < liste_mot->size_list; i++) {
    fprintf(stderr, "algo1 mot n°%d : %s\n",i +1, liste_mot->list[i].v);
    fflush(stderr);
  }
  */

  fprintf(stderr, "taille liste_mot : %d\n", liste_mot->size_list);
  fflush(stderr);
}


void reduce_possible_list_with_abc(int* count_possible_word, int counter_concept_recup) {
  char* last_two_concept[2];

  int before_last = counter_concept_recup-2;
  int last = counter_concept_recup-1;
  last_two_concept[0] = list_turn_concept[before_last];
  last_two_concept[1] = list_turn_concept[last];
  fprintf(stderr, "last 2 top c : %s et %s\n", last_two_concept[0], last_two_concept[1]);
  fflush(stderr);

  // for all left words : check all coeff combination if right order
  for (int i = 0; i < *count_possible_word; i++) {
    int remove_word = 1;
    int word_pos = find_word_position(liste_mot->list[i].v);

    for (int j = 0; j < goddess.coeff.size_list; j++) {
      int score_c1 = find_score_concept_with_word_position(last_two_concept[0], word_pos);
      int score_c2 = find_score_concept_with_word_position(last_two_concept[1], word_pos);

      // goddess.coeff.list_possible_coeff[j][0];
      int t_val_c1 = goddess.coeff.list_possible_coeff[j][0] * score_c1 + 10*goddess.coeff.list_possible_coeff[j][1]*t(last_two_concept[0], liste_mot->list[i].v) + 10*goddess.coeff.list_possible_coeff[j][2]*u(last_two_concept[0], liste_mot_candidat[i].v);
      fprintf(stderr, "t = %d ;; %s/%s = t_c1 : %d\n", t(last_two_concept[0], liste_mot->list[i].v), last_two_concept[0], liste_mot->list[i].v, t_val_c1);
      fflush(stderr);
      int t_val_c2 = goddess.coeff.list_possible_coeff[j][0] * score_c1 + 10*goddess.coeff.list_possible_coeff[j][1]*t(last_two_concept[1], liste_mot->list[i].v) + 10*goddess.coeff.list_possible_coeff[j][2]*u(last_two_concept[1], liste_mot_candidat[i].v);
      fprintf(stderr, "%s/%s = t_c2 : %d\n\n", last_two_concept[1], liste_mot->list[i].v, t_val_c2);
      fflush(stderr);
      if (t_val_c1 >= t_val_c2) { // good for this triplet (a,b,c)
        remove_word = 0;
        break;
      }
    }

    // TODO : créée une fonction qui enlève les éléments d'un tableau
    if (remove_word == 1) { // Remove the word from list of word possible
      fprintf(stderr, "j'enlève i = %d : %s\n",i , liste_mot->list[i].v);
      fflush(stderr);
      for (int k = i; k < (*count_possible_word) - 1; k++) {
        liste_mot->list[k].v = liste_mot->list[k+1].v;
        for (int l = 0; l < NB_CONCEPT; l++) {
          liste_mot->list[k].concepts[l] = liste_mot->list[k+1].concepts[l];
          liste_mot->list[k].score[l] = liste_mot->list[k+1].score[l];
        }
      }
      i--;
      (*count_possible_word) -= 1;
    }
  }

  // print word possible list
  for (int i = 0; i < *count_possible_word; i++) {
    fprintf(stderr, "algo2 mot %d : %s\n", i, liste_mot->list[i].v);
    fflush(stderr);
  }
}

void algo_find_p(char* secret_word) { // TODO : A optimiser
  int sw_pos = find_word_position(secret_word);
  int tab[NB_CONCEPT] = {0};

  /*
  fprintf(stderr, "secret wooord : %s\n", secret_word);
  fflush(stderr);

  fprintf(stderr, "secret wooord dans liste initi : %s\n", liste_mot_initiale->list[sw_pos].v);
  fflush(stderr);

  for (int i = 0; i < NB_CONCEPT; i++) {
    fprintf(stderr, "concept du mot :%s\n", liste_mot_initiale->list[sw_pos].concepts[i]);
    fflush(stderr);
  }
  */

  // Si on est à la manche 1 : on trouve plus facilement p si le concept est à la 7ème ou 33ème position
  for (int i = 0; i < NB_TURN; i++) {
    for (int j = 0; j < NB_CONCEPT; j++) {
      if (strcmp(list_turn_concept[i], liste_mot_initiale->list[sw_pos].concepts[j]) == 0) {
        tab[j] = 1;
      }
    }
  }

  /*
  // print tab
  fprintf(stderr, "[");
  fflush(stderr);
  for (int i = 0; i < NB_CONCEPT; i++) {
    fprintf(stderr, "%d, ", tab[i]);
    fflush(stderr);
  }
  fprintf(stderr, "]\n");
  fflush(stderr);
  */


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

// Test all combination of value a, b, c = {5, 35, 65, 95}
void algo_find_abc(char* secret_word) { // TODO : A optimiser
  int sw_pos = find_word_position(secret_word);
  // liste_mot_initiale->list[sw_pos].concepts[i];
  int tab_coeff_possible[NB_COEFF] = COEFF;
  int all_combination_coeff[NB_COEFF*NB_COEFF*NB_COEFF][4]; // [][a, b, c, state]
  int count = 0;

  // 3 first concepts
  char* top_concept[3];
  for (int i = 0; i < 3; i++) {
    top_concept[i] = list_turn_concept[i];
  }
  fprintf(stderr, "vrai top 3 : %s %s %s\n", top_concept[0], top_concept[1], top_concept[2]);
  fflush(stderr);

  // tab of sort 3 top concept depending on a, b, c
  int t_val_concept[3];

  // list all combination of coeff
  for (int a = 0; a < NB_COEFF; a++) {
    for (int b = 0; b < NB_COEFF; b++) {
      for (int c = 0; c < NB_COEFF; c++) {
        int a_, b_, c_;
        a_ = tab_coeff_possible[a];
        b_ = tab_coeff_possible[b];
        c_ = tab_coeff_possible[c];
        all_combination_coeff[count][0] = a_;
        all_combination_coeff[count][1] = b_;
        all_combination_coeff[count][2] = c_;


        // find_score_concept_with_word_position()
        for (int i = 0; i < 3; i++) {
          int sc = find_score_concept_with_word_position(top_concept[i], sw_pos);
          int t_ = t(top_concept[i], secret_word);
          int u_ = u(top_concept[i], secret_word);
          // fprintf(stderr, "sc, t, u : %d %d %d\n", sc, t_, u_);
          // fflush(stderr);
          t_val_concept[i] = a_ * sc + 10 * b_ * t_ + 10 * c_ * u_;
        }
        // fprintf(stderr, "t_val : %d %d %d\n", t_val_concept[0], t_val_concept[1], t_val_concept[2]);
        // fflush(stderr);

        char* concept_sorted_abc[3];
        // copy top_val
        for (int i = 0; i < 3; i++) {
          concept_sorted_abc[i] = top_concept[i];
        }

        // sort 3 top concept by t_val
        for (int i = 3 - 1; i > 0; i--) { // TODO : actuellement tri à bulle -- > à optimiser et/ou factoriser en fonction
          for (int j = 0; j < 3 -1; j++) {
            if (t_val_concept[j+1] > t_val_concept[j]) { // value order
              int tmp = t_val_concept[j+1];
              t_val_concept[j+1] = t_val_concept[j];
              t_val_concept[j] = tmp;

              char* s_tmp = concept_sorted_abc[j+1];
              concept_sorted_abc[j+1] = concept_sorted_abc[j];
              concept_sorted_abc[j] = s_tmp;
            } else if (t_val_concept[j+1] == t_val_concept[j]) { // alphabetic order
              for (int k = 0; k < (int) strlen(concept_sorted_abc[j+1]); k++) {
                if (concept_sorted_abc[j+1][k] > concept_sorted_abc[j][k]) { // greater 'z' > 'a'
                  break;
                } else if (concept_sorted_abc[j+1][k] < concept_sorted_abc[j][k]) {
                  int tmp = t_val_concept[j+1];
                  t_val_concept[j+1] = t_val_concept[j];
                  t_val_concept[j] = tmp;

                  char* s_tmp = concept_sorted_abc[j+1];
                  concept_sorted_abc[j+1] = concept_sorted_abc[j];
                  concept_sorted_abc[j] = s_tmp;
                  break;
                }
              }
            }
          }
        }

        all_combination_coeff[count][3] = 1;

        // Check if right position of concept
        for (int i = 0; i < 3; i++){
          if (strcmp(concept_sorted_abc[i], top_concept[i]) != 0) {
            all_combination_coeff[count][3] = 0;
            break;
          }
        }

        // add to goddess possible coeff list
        if (all_combination_coeff[count][3] == 1) {
          goddess.coeff.list_possible_coeff[goddess.coeff.size_list][0] = all_combination_coeff[count][0];
          goddess.coeff.list_possible_coeff[goddess.coeff.size_list][1] = all_combination_coeff[count][1];
          goddess.coeff.list_possible_coeff[goddess.coeff.size_list][2] = all_combination_coeff[count][2];
          goddess.coeff.size_list++;
        }


        // print check sortie
        fprintf(stderr, "pour a,b,c = [%d, %d, %d]\n", a_, b_, c_);
        fflush(stderr);
        fprintf(stderr, "position des tval : %s %s %s\n", concept_sorted_abc[0], concept_sorted_abc[1], concept_sorted_abc[2]);
        fflush(stderr);


        count++;
      }
    }
  }

  // print of possible combination of coeff that works
  for (int i = 0; i < goddess.coeff.size_list; i++) {
    fprintf(stderr, "a = %d ", goddess.coeff.list_possible_coeff[i][0]);
    fflush(stderr);
    fprintf(stderr, "b = %d ", goddess.coeff.list_possible_coeff[i][1]);
    fflush(stderr);
    fprintf(stderr, "c = %d \n", goddess.coeff.list_possible_coeff[i][2]);
    fflush(stderr);
  }
}

/* END Methods for game's array */


/* BEGIN Decision methods */

void print_decision(int* has_proposed, char** word_proposed, int manche, int tour) {
  if (manche == 0 && tour < 20) { // Get all the concept first
    //fprintf(stderr, "Je passe !!!\n");
    //fflush(stderr);
    print_pass();
  } else {
    if (liste_mot->size_list == 2) {
      *word_proposed = liste_mot->list[0].v;
      guess_word(liste_mot->list[0].v);
      *has_proposed = 1;
    } else if (liste_mot->size_list == 1) {
      *word_proposed = liste_mot->list[0].v;
      guess_word(liste_mot->list[0].v);
      *has_proposed = 1;
    } else {
      //fprintf(stderr, "Je passe !!!\n");
      //fflush(stderr);
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
  goddess.coeff.size_list = 0;

  // Init arrays
  for (int i = 0; i < NB_TURN; i++) {
    list_turn_concept[i] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
  }

  // word list
  liste_mot = malloc(sizeof(struct list_word));
  liste_mot->list = malloc(NB_WORD * sizeof(struct word));
  liste_mot->size_list = NB_WORD;
  for (int i = 0; i < NB_WORD; i++) {
      liste_mot->list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
      for (int j = 0; j < NB_CONCEPT; j++) {
          liste_mot->list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
      }
  }

  for (int i = 0; i < NB_WORD; i++) {
    liste_mot_candidat[i].v = malloc(255 * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      liste_mot_candidat[i].concepts[j] = malloc(255 * sizeof(char));
    }
  }

  liste_mot_initiale = malloc(sizeof(struct list_word));
  liste_mot_initiale->list = malloc(NB_WORD * sizeof(struct word));
  liste_mot_initiale->size_list = NB_WORD;
  for (int i = 0; i < NB_WORD; i++) {
    liste_mot_initiale->list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      liste_mot_initiale->list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }
}

void free_global_variables() {
  for (int i = 0; i < NB_TURN; i++) {
    free(list_turn_concept[i]);
  }

  for (int i = 0; i < NB_WORD; i++) {
    free(liste_mot->list[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      free(liste_mot->list[i].concepts[j]);
    }
  }

  for (int i = 0; i < NB_WORD; i++) {
    free(liste_mot_candidat[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      free(liste_mot_candidat[i].concepts[j]);
    }
  }

  for (int i = 0; i < NB_WORD; i++) {
    free(liste_mot_initiale->list[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      free(liste_mot_initiale->list[i].concepts[j]);
    }
  }
}

/* END Initialisation & free mallocs methods */


int main(void) {
    // variables
    struct game_param game;


    // Liste totale des concepts

    init_global_variables(); // malloc for every array

    // Initialisation phase:
    char* line = malloc(255 * sizeof(char)); // to optimize

    // get game infos
    scanf(" %[^\n]s\n", line);
    sscanf(line, "%d %d", &game.NJ, &game.J);
    // fprintf(stderr, "Récup game infos : %s\n", line);
    // fflush(stderr);

    // get every words & concepts
    get_every_words_and_concepts();


    // trie concept ordre croissant du score ===> tri à bulle
    sort_concept_by_score_asc();

    // mémoire dans une copie la liste des mots triées
    for (int i = 0; i < NB_WORD; i++) {
      liste_mot_initiale->list[i].v = liste_mot->list[i].v;
      for (int j = 0; j < NB_CONCEPT; j++) {
        liste_mot_initiale->list[i].concepts[j] = liste_mot->list[i].concepts[j];
        liste_mot_initiale->list[i].score[j] = liste_mot->list[i].score[j];
      }
    }

    // print_concepts_words_list(); // Check words & concepts associated

    struct joueur liste_joueur[game.NJ]; // array of player's struct

    // BEGIN GAME
    // every round
    for (int manche = 0; manche < 5; manche++) {
      print_concepts_words_list();
      if (manche == 0) {
        // fprintf(stderr, "début de manche hehehehn°%d\n", manche);
        // fflush(stderr);



        // Récupération information de début de manche
        for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
          int J, P, END;

          scanf(" %[^\n]s\n", line);
          sscanf(line, "%d %d %d", &J, &P, &END);
          // fprintf(stderr, "Récup toute la ligne : %s\n", line);
          // fflush(stderr);

          liste_joueur[J].J = J;
          liste_joueur[J].P = P;
          liste_joueur[J].END = END;

          // fprintf(stderr, "Récup Joueur J, P, END : %d %d %d\n", J, P, END);
          // fflush(stderr);


          // scanf("%d %d %d", &liste_joueur[nb_player].J, &liste_joueur[nb_player].P, &liste_joueur[nb_player].END);

          if (liste_joueur[J].END == 1) {
            liste_joueur[J].ST = 1;
          }
        }

        /*
        for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
          fprintf(stderr, "Joueur %d, points = %d, sortie ? %d\n", liste_joueur[nb_player].J, liste_joueur[nb_player].P, liste_joueur[nb_player].ST);
          fflush(stderr);
        }
        */
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
        // fprintf(stderr, "Tour n°%d\n", tour);
        // fflush(stderr);

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
          /*
          fprintf(stderr, "J'ai proposé un truc faux frérot : %s \n!!!!", liste_mot->list[0].v);
          fflush(stderr);
          fprintf(stderr, "Du coup je Guess l'autre mot : %s \n!!!!", liste_mot->list[1].v);
          fflush(stderr);
          */
          fprintf(stdout, "GUESS %s\n", liste_mot->list[1].v);
          fflush(stdout);
          word_proposed = liste_mot->list[1].v;
        } else { // si j'ai PAS proposé quelque chose
          if (me_trouve == 0) { // Si j'ai pas trouvé le mot
            reduce_possible_list_with_concept(counter_concept_recup-1); // algo() ; counter_concept_recup-1 == last concept's position
            fprintf(stderr, "On a récup avec concept : %d\n", liste_mot->size_list);
            fflush(stderr);

            // if (manche != 0 && counter_concept_recup >= 2 && goddess.coeff.size_list > 0) {
              // reduce_possible_list_with_abc(&taille_mot_candidat, counter_concept_recup); // TODO : À sépérer dans une liste différente
            // }
            // fprintf(stderr, "On a récup avec (abc) %d\n", liste_mot->size_list);
            // fflush(stderr);

            print_decision(&has_proposed, &word_proposed, manche, tour); // Send decision
            // fprintf(stderr, "mot proposé récup : %s\n", word_proposed);
            // fflush(stderr);

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
        // algo_find_abc(word_proposed);
        free(word_proposed);
      }



      reset_list_word_to_init(); // Remettre la liste_mot à l'initiale
    }
    // END GAME

    free_global_variables(); // Free the malloc

    return 0;
}
