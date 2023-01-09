// Work by Laurent Ngeth
/** EXPLICATION DES ALGORITHMES
 * Il existe 3 algorithmes qui travaillent indépendamment les uns des autres :
 *
 * -- Fonctionnement Algo 1 :
 *   !Caractéristiques!
 *   - Utilise connaissance de 'p' pour trouver plus facilement le mot secret.
 *   - Possède à coup sûr, dans sa liste de mot candidat, le mot secret de la déesse.
 *   - Au début, on ne connait pas p, on regarde donc les 7 premiers et 17 derniers scores.
 *     --> L'algo sera plus efficace quand le paramètre 'p' est trouvé, c'est à dire après avoir trouvé le mot secret du round.
 *     --> Si pas mot secret pas trouvé au 1er round, la recherche de p continue au round suivant et ainsi de suite.
 *
 *   !Logique GLOBALE en Pseudo-code!
 *   - Récupération du dernier concept donnée par la déesse c_last
 *   - Pour chaque mot, mot_i, restant de la liste de mot candidat :
 *       - Si le concept c_last fait partie du range des '10 - p' moins scorés ou '10 + p' plus scorés concept de mot_i :
 *         --> On garde le mot.
 *       - Sinon, on ne le garde pas.
 *
 *
 * -- Fonctionnement Algo 2 :
 *   !Caractéristiques!
 *   - Utilise les triplets de valeurs possibles de (a, b, c) valides parmi la liste de choix suivants : { 5, 35, 65, 95 }.
 *   - Fonctionne en 2 étapes :
 *     1- Lister les coefficients possibles (fonction algo2_find_abc()) :
 *       - 64 combinaisons possibles ==> moins long que [1, 100] pour chaque paramètre.
 *       - listage à la fin de chaque round si on pas déjà lister le coefficient et qu'on a trouvé le mot secret.
 *     2- Utilisation des coefficients valides pour supprimer des mots.
 *       - Se lance quand on une liste de coeff > 0 et que la déesse a donné au moins 2 concepts (à partir tour 2).
 *
 *   !Logique GLOBALE en Pseudo-code!
 *     Etape 1 : Lister les coefficients possibles
 *     - Pour chaque triplet (a, b, c) possibles :
 *       - garde_triplet = 0
 *       - Pour les 3 premiers concepts donné par la déesse :
 *         - Calcul t_val_concept_i = a*score + 10*b*t + 10*b*u
 *       - Si t_val_concept_1 > t_val_concept_2 :
 *         - Si t_val_concept_2 > t_val_concept_3 :
 *           --> garde_triplet = 1
 *       - Sinon (cas si t_val_concept_i = t_val_concept_j --> Ordre alphabétique) :
 *         - etc...
 *       - Si garde_triplet == 0:
 *         --> On enlève de la liste des triplets possibles.
 *
 *      Etape 2 : Utilisation des coefficients
 *      - Si taille de la liste de coefficient possibles > 0 :
 *        - Pour chaque mot de la liste candidat possible :
 *          - garde_mot = 0
 *          - Pour les 2 derniers concepts donnés par la déesse :
 *            - Pour chaque triplet (a, b, c) :
 *              - Calcul t_val_concept_i = a*score + 10*b*t + 10*b*u
 *              - Si t_val_concept_i > t_val_concept_j (avec i concept donné avant le concept j) :
 *                --> garde_mot = 1
 *              - Sinon si (même valeur --> Ordre alphabétique):
 *                --> etc...
 *          - Si garde_mot = 0 :
 *            --> On enlève le mot de la liste.
 *
 *
 * -- Fonctionnement Algo 3 :
 *   !Caractéristiques!
 *   - Utilise les 2 premiers algorithmes cités en haut pour essayer de trouver le mot plus rapidement.
 *   - Il fait l'intersection entre les mots candidats de l'algo 1 et 2, puis avec sa propre liste (sauvegarde de l'intersection des listes au tour précédent).
 *   --> Exécuté uniquement si algo2 éxécuté (quand liste algo2 > 0).
 *   --> Peut ne pas avoir le mot secret dans sa liste.
 *
 *   !Logique GLOBALE en Pseudo-code!
 *   - Si liste_algo1 > 0 && liste_algo2 > 0:
 *   - Si tour = 1 :
 *     --> liste_algo1 /\ liste_algo2 = liste_algo3
 *   - Sinon :
 *     --> liste_algo1 /\ liste_algo2 /\ liste_algo3 = liste_algo3
 *
 *
 * -- Tous les algos :
 *   - Si liste de mot candidat nulle, l'algo ne sera plus éxécuté jusqu'à la fin de la manche.
 *
 */

// import libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* constants */
#define NB_WORD 1000
#define NB_CONCEPT 50
#define NB_ROUND 5
#define NB_TURN 20
#define NB_COEFF 4

#define MAX_SIZE_WORD 25
#define MAX_SIZE_CONCEPT 15

/* All structs */
struct list_concept {
  char* list[NB_CONCEPT];
  int size;
};

struct word {
  char* v;
  char* concepts[NB_CONCEPT];
  int scores[NB_CONCEPT];
};

struct list_word {
  struct word* list;
  int size;
};

struct player {
  int J; // Number player
  int score;
  int ST; // is out of game or not
};

struct game_state {
  int round;
  int turn;
  int is_all_founded;
  int is_me_founded;
};

struct game_config {
  struct player* players;
  int NJ;
  int me_nb_j; // my pythie number
  struct game_state state;
};

struct p_val {
  int upper;
  int lower;
  int founded;
};

struct coeffs {
  int a;
  int b;
  int c;
};

struct goddess_param {
  struct p_val p;
  struct coeffs* t_coeff;
  int size_t_coeff;
  int secret_words_index[5]; // all 5 rounds secret word's index in the initial_sorted_word_list
};

struct algo_data {
  struct list_word candidate_word_list;
  char* last_word_proposed;
};

/** ALL PROTOTYPES **/
/* Initialization & Free */
// Global variables
void init_global_variables();
void free_global_variables();
// Temporary variables
void init_new_list_word(struct list_word*, int);
void free_list_word(struct list_word*);

/* Math/calculation */
int get_random_int_in_range(int, int);
int get_index_word_based_on_abc(struct algo_data*);
int t(char*, char*);
int u(char*, char*);

/* Struct/array manipulation */
// getter
int find_word_index_in_sorted_list(char*);
int find_score_concept_with_word_position(char*, int);
int find_score_of_concept_by_word_position_in_algo_list(struct algo_data*, char*, int);
// delete
void remove_word_from_algos_word_list(struct algo_data*, char*);
// update
void sort_concept_by_score_asc();
void copy_sorted_list_in_algos(struct algo_data*);
void copy_list_word_to_another(struct list_word*, struct list_word*);
void reset_game_state();

/* Scanf retrieve infos */
void get_all_words_and_concepts();
void get_round_infos();
void get_turn_infos();
void get_concept();

/* Debug print */
void print_all_valid_coeffs();
void print_all_infos_candidate_word_list(struct list_word*);

/* Output print */
void print_pass();
void guess_word(struct algo_data*, char*);
void print_decision();

/* Algos */
// algo1
void find_p(int, int, int);
void ia_algo1();
// algo2
void algo2_find_abc();
void ia_algo2();
// algo3
void ia_algo3();
void all_algo();


/** Global variables **/
static struct list_word initial_sorted_word_list; // contains all words & in each word, all concepts in ascendent score order
static struct list_concept list_concepts_of_round; // all 20 concepts gave by Goddess during a round
static struct algo_data algo1; // only p strat
static struct algo_data algo2; // only with (a, b, c) params
static struct algo_data algo3; // with both p & (a, b, c)
static struct goddess_param goddess;
static struct game_config game;
static int possible_coeff[NB_COEFF] = { 5, 35, 65, 95 };
static char secret_word_save[MAX_SIZE_WORD];
static struct coeffs mean_coeffs;


/** All functions **/

/* Initialization & Free */

/** Malloc all the global variables
 */
void init_global_variables() {
  list_concepts_of_round.size = 0;
  for (int i = 0; i < NB_TURN; i++) {
    list_concepts_of_round.list[i] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
  }

  initial_sorted_word_list.list = (struct word*) malloc(NB_WORD*sizeof(struct word));
  initial_sorted_word_list.size = NB_WORD;

  algo1.candidate_word_list.list = (struct word*) malloc(NB_WORD*sizeof(struct word));
  algo1.candidate_word_list.size = NB_WORD;
  algo1.last_word_proposed = (char*) malloc(MAX_SIZE_WORD * sizeof(char));

  algo2.candidate_word_list.list = (struct word*) malloc(NB_WORD*sizeof(struct word));
  algo2.candidate_word_list.size = NB_WORD;
  algo2.last_word_proposed = (char*) malloc(MAX_SIZE_WORD * sizeof(char));

  algo3.candidate_word_list.list = (struct word*) malloc(NB_WORD*sizeof(struct word));
  algo3.candidate_word_list.size = NB_WORD;
  algo3.last_word_proposed = (char*) malloc(MAX_SIZE_WORD * sizeof(char));

  for (int i = 0; i < NB_WORD; i++) {
    initial_sorted_word_list.list[i].v = (char*) malloc(MAX_SIZE_WORD * sizeof(char));
    algo1.candidate_word_list.list[i].v = (char*) malloc(MAX_SIZE_WORD * sizeof(char));
    algo2.candidate_word_list.list[i].v = (char*) malloc(MAX_SIZE_WORD * sizeof(char));
    algo3.candidate_word_list.list[i].v = (char*) malloc(MAX_SIZE_WORD * sizeof(char));

    for (int j = 0; j < NB_CONCEPT; j++) {
      initial_sorted_word_list.list[i].concepts[j] = (char*) malloc(MAX_SIZE_CONCEPT * sizeof(char));
      algo1.candidate_word_list.list[i].concepts[j] = (char*) malloc(MAX_SIZE_CONCEPT * sizeof(char));
      algo2.candidate_word_list.list[i].concepts[j] = (char*) malloc(MAX_SIZE_CONCEPT * sizeof(char));
      algo3.candidate_word_list.list[i].concepts[j] = (char*) malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }

  goddess.p.upper = 7;
  goddess.p.lower = 3;
  goddess.p.founded = 0;
  for (int i = 0; i < 5; i++) {
    goddess.secret_words_index[i] = -1;
  }

  game.state.round = 0;
  game.state.turn = 1;
  game.state.is_all_founded = 0;
  game.state.is_me_founded = 0;

  goddess.t_coeff = malloc(NB_COEFF*NB_COEFF*NB_COEFF * sizeof(struct coeffs));

  int new_size_t_coeff = 0;
  for (int a = 0; a < NB_COEFF; a++) {
    for (int b = 0; b < NB_COEFF; b++) {
      for (int c = 0; c < NB_COEFF; c++) {
        goddess.t_coeff[new_size_t_coeff] = (struct coeffs) { .a = possible_coeff[a], .b = possible_coeff[b], .c = possible_coeff[c] };
        new_size_t_coeff++;
      }
    }
  }
  goddess.size_t_coeff = new_size_t_coeff;

  mean_coeffs.a = 0;
  mean_coeffs.b = 0;
  mean_coeffs.c = 0;

  // print_all_valid_coeffs();
}

/** Free all the global variables
 */
void free_global_variables() {
  for (int i = 0; i < NB_WORD; i++) {
    free(initial_sorted_word_list.list[i].v);
    free(algo1.candidate_word_list.list[i].v);
    free(algo2.candidate_word_list.list[i].v);
    free(algo3.candidate_word_list.list[i].v);

    for (int j = 0; j < NB_CONCEPT; j++) {
      free(initial_sorted_word_list.list[i].concepts[j]);
      free(algo1.candidate_word_list.list[i].concepts[j]);
      free(algo2.candidate_word_list.list[i].concepts[j]);
      free(algo3.candidate_word_list.list[i].concepts[j]);
    }
  }

  free(initial_sorted_word_list.list);

  for (int i = 0; i < NB_TURN; i++) {
    free(list_concepts_of_round.list[i]);
  }

  free(algo1.candidate_word_list.list);
  free(algo1.last_word_proposed);

  free(algo2.candidate_word_list.list);
  free(algo2.last_word_proposed);

  free(algo3.candidate_word_list.list);
  free(algo3.last_word_proposed);

  free(game.players);
}

/** Malloc a struct list_word passed in parameter
 * \param list the address of the struct list_word to malloc
 * \param size length of array to init
 */
void init_new_list_word(struct list_word* list, int size) {
  list->list = malloc(size * sizeof(struct word));
  for (int i = 0; i < size; i++) {
    list->list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      list->list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }
  list->size = 0;
}

/** Free a struct list_word passed in parameter
 * \param list the address of the struct list_word to free
 */
void free_list_word(struct list_word* list) {
  for (int i = 0; i < list->size; i++) {
    free(list->list[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      free(list->list[i].concepts[j]);
    }
  }
  free(list->list);
}

/* Math/calculation */

/** Generate a random integer from a certain range [min, max]
 * \param min the lower bound
 * \param max the upper bound
 * \return a random number within range [min, max]
 */
int get_random_int_in_range(int min, int max) {
  srand(time(0));
  return (rand() % (max - min + 1)) + min;
}

/** Return the index of the word that has the maximum t_value, condition that its t_value is lower than the t_value of the last concept gave by the goddess
 * \param algo the algo to retrieve word index
 */
int get_index_word_based_on_abc(struct algo_data* algo) {
  int t_val_tab[algo->candidate_word_list.size];
  int t_index_tab[algo->candidate_word_list.size];
  int count_t_val = 0;
  for (int i = 0; i < algo->candidate_word_list.size; i++) {
    t_val_tab[i] = 0;
    t_index_tab[i] = 0;
  }

  int t_val_last_concept[2];
  for (int i = 0; i < algo->candidate_word_list.size; i++) {
    int score = find_score_of_concept_by_word_position_in_algo_list(algo, list_concepts_of_round.list[list_concepts_of_round.size-2], i);
    int t_ = t(algo->candidate_word_list.list[i].v, list_concepts_of_round.list[list_concepts_of_round.size-2]);
    int u_ = u(algo->candidate_word_list.list[i].v, list_concepts_of_round.list[list_concepts_of_round.size-2]);
    t_val_last_concept[0] = mean_coeffs.a * score + 10*t_*mean_coeffs.b + 10*u_*mean_coeffs.c;

    score = find_score_of_concept_by_word_position_in_algo_list(algo, list_concepts_of_round.list[list_concepts_of_round.size-1], i);
    t_ = t(algo->candidate_word_list.list[i].v, list_concepts_of_round.list[list_concepts_of_round.size-1]);
    u_ = u(algo->candidate_word_list.list[i].v, list_concepts_of_round.list[list_concepts_of_round.size-1]);
    t_val_last_concept[1] = mean_coeffs.a * score + 10*t_*mean_coeffs.b + 10*u_*mean_coeffs.c;

    if (t_val_last_concept[0] >= t_val_last_concept[1]) {
      t_val_tab[count_t_val] = t_val_last_concept[1];
      t_index_tab[count_t_val] = i;
      count_t_val++;
    }
  }

  int best_t_val = 0;
  int best_t_val_index = 0;

  for (int i = 0; i < count_t_val; i++) {
    if (t_val_tab[i] > best_t_val) {
      best_t_val = t_val_tab[i];
      best_t_val_index = t_index_tab[i];
    }
  }

  return best_t_val_index;
}

/** Count the number of character that can be found from a string in another
 * \param sw the first string
 * \param c the second string
 * \return count of same character from both string
 */
int t(char* sw, char* c) {
  int count = 0;
  for (int i = 0; i < (int) strlen(sw); i++) {
    for (int j = 0; j < (int) strlen(c); j++) {
      if (sw[i] == c[j] && sw[i] != '\0') {
        count++;
        break;
      }
    }
  }
  return count;
}

/** Return the length's difference between two strings
 * \param m first string
 * \param w second string
 * \return integer length's difference
 */
int u(char* m, char* w) {
  return abs((int) strlen(m) - (int) strlen(w));
}


/* Struct/array manipulation */

/** Get the index of a string inside the initial sorted list
 * \param word string to find in sorted list
 * \return index/position of the string in the initial sorted list
 */
int find_word_index_in_sorted_list(char* word) {
  for (int i = 0; i < NB_WORD; i++) {
    if (strcmp(word, initial_sorted_word_list.list[i].v) == 0) {
      fprintf(stderr, "secret w : %s\n", initial_sorted_word_list.list[i].v);
      fflush(stderr);
      return i;
    }
  }
  return -1;
}

/** Get the score of a concept with the word at the specified position in initial sorted list
 * \param concept the interested concept
 * \param w_pos the position of the word in the initial sorted list
 * \return integer the score of the concept with the word at w_pos index
 */
int find_score_concept_with_word_position(char* concept, int w_pos) {
  for (int i = 0; i < NB_CONCEPT; i++) {
    if (strcmp(initial_sorted_word_list.list[w_pos].concepts[i], concept)) {
      return initial_sorted_word_list.list[w_pos].scores[i];
    }
  }
  return -1; // TODO : tester l'erreur
}

/** Get the score of an concept associated with the word at position i, in the wanted algo's candidate list
 * \param algo the address of the interested struct algo_data to look
 * \param concept the interested concept
 * \param index the integer position of the word in the interested algo
 * \return integer the score of the concept associated with the algo
 */
int find_score_of_concept_by_word_position_in_algo_list(struct algo_data* algo, char* concept, int index) {
  for (int i = 0; i < NB_CONCEPT; i++) {
    if (strcmp(algo->candidate_word_list.list[index].concepts[i], concept) == 0) {
      return algo->candidate_word_list.list[index].scores[i];
    }
  }
  return -1;
}

/** Remove a word from a specified struct algo_data (Shift back all the next struct)
 * \param algo the address of the interested struct algo_data
 * \param word the word to remove
 */
void remove_word_from_algos_word_list(struct algo_data* algo, char* word) {
  for (int i = 0; i < algo->candidate_word_list.size; i++) {
    if (strcmp(algo->candidate_word_list.list[i].v, word) == 0) {
      int last_element_index = (algo->candidate_word_list.size)-1;
      for (int j = i; j < last_element_index; j++) {
        strcpy(algo->candidate_word_list.list[j].v, algo->candidate_word_list.list[j+1].v);
        for (int k = 0; k < NB_CONCEPT; k++) {
          strcpy(algo->candidate_word_list.list[j].concepts[k], algo->candidate_word_list.list[j+1].concepts[k]);
          algo->candidate_word_list.list[j].scores[k] = algo->candidate_word_list.list[j+1].scores[k];
        }
      }
      algo->candidate_word_list.size = last_element_index;
      break;
    }
  }
}

/** Bubble sort of initial_sorted_word_list by score in ascending order
 */
void sort_concept_by_score_asc() {
  char* tmp_s = malloc(MAX_SIZE_CONCEPT * sizeof(char));
  for (int i = 0; i < NB_WORD; i++) {
    for (int j = NB_CONCEPT-1; j > 0; j--) {
      for (int k = 0; k < j; k++) {
        int tmp_i;
        if (initial_sorted_word_list.list[i].scores[k + 1] < initial_sorted_word_list.list[i].scores[k]) {
          tmp_i = initial_sorted_word_list.list[i].scores[k + 1];
          initial_sorted_word_list.list[i].scores[k + 1] = initial_sorted_word_list.list[i].scores[k];
          initial_sorted_word_list.list[i].scores[k] = tmp_i;

          strcpy(tmp_s, initial_sorted_word_list.list[i].concepts[k + 1]);
          strcpy(initial_sorted_word_list.list[i].concepts[k + 1], initial_sorted_word_list.list[i].concepts[k]);
          strcpy(initial_sorted_word_list.list[i].concepts[k], tmp_s);
        } else if (initial_sorted_word_list.list[i].scores[k + 1] == initial_sorted_word_list.list[i].scores[k]) {
          if (strcmp(initial_sorted_word_list.list[i].concepts[k], initial_sorted_word_list.list[i].concepts[k+1]) == 1) {
            tmp_i = initial_sorted_word_list.list[i].scores[k + 1];
            initial_sorted_word_list.list[i].scores[k + 1] = initial_sorted_word_list.list[i].scores[k];
            initial_sorted_word_list.list[i].scores[k] = tmp_i;

            strcpy(tmp_s, initial_sorted_word_list.list[i].concepts[k + 1]);
            strcpy(initial_sorted_word_list.list[i].concepts[k + 1], initial_sorted_word_list.list[i].concepts[k]);
            strcpy(initial_sorted_word_list.list[i].concepts[k], tmp_s);
          }
        }
      }
    }
  }
  free(tmp_s);
}

/** Copy all the struct & values of initial_sorted_word_list into the specified algo
 * \param algo the address of the interested struct algo_data
 */
void copy_sorted_list_in_algos(struct algo_data* algo) {
  for (int i = 0; i < NB_WORD; i++) {
    strcpy(algo->candidate_word_list.list[i].v, initial_sorted_word_list.list[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      strcpy(algo->candidate_word_list.list[i].concepts[j], initial_sorted_word_list.list[i].concepts[j]);
      algo->candidate_word_list.list[i].scores[j] = initial_sorted_word_list.list[i].scores[j];
    }
  }
}

/** Copy all struct & values from a struct list_word to another
 * \param to the destination address of struct list_word to copy to
 * \param from the source address of struct list_word to copy from
 */
void copy_list_word_to_another(struct list_word* to, struct list_word* from) {
  for (int i = 0; i < from->size; i++) {
    strcpy(to->list[i].v, from->list[i].v);
    for (int j = 0; j < NB_CONCEPT; j++) {
      strcpy(to->list[i].concepts[j], from->list[i].concepts[j]);
      to->list[i].scores[j] = from->list[i].scores[j];
    }
  }
  to->size = from->size;
}

/** Reset to initial value some necessary global variables for the sake of the game
 */
void reset_game_state() {
  game.state.is_all_founded = 0;
  game.state.is_me_founded = 0;

  memset(algo1.last_word_proposed, 0, MAX_SIZE_WORD);
  memset(algo2.last_word_proposed, 0, MAX_SIZE_WORD);
  memset(algo3.last_word_proposed, 0, MAX_SIZE_WORD);
  memset(secret_word_save, 0, MAX_SIZE_WORD);

  // reset the list of concept
  list_concepts_of_round.size = 0;

  algo1.candidate_word_list.size = NB_WORD;
  algo2.candidate_word_list.size = NB_WORD;
  algo3.candidate_word_list.size = NB_WORD;

  copy_sorted_list_in_algos(&algo1);
  copy_sorted_list_in_algos(&algo2);
  copy_sorted_list_in_algos(&algo3);
}

/* Scanf retrieve infos */

/** Scan from the stdin all the 1000 words & for each word the 50 concepts string and score values into the initial sorted list
 */
void get_all_words_and_concepts() {
  char line[MAX_SIZE_WORD];

  // Get all concepts
  for (int i = 0; i < NB_CONCEPT; i++) {
    scanf("%s", line);

    for (int j = 0; j < NB_WORD; j++) { // add each concept in every word
      sscanf(line, "%s", initial_sorted_word_list.list[j].concepts[i]);
    }
  }

  // Get all words
  for (int i = 0; i < NB_WORD; i++) {
    scanf("%s", line);
    sscanf(line, "%s", initial_sorted_word_list.list[i].v);

    for (int j = 0; j < NB_CONCEPT; j++) {
      scanf("%s", line);
      sscanf(line, "%d", &initial_sorted_word_list.list[i].scores[j]);
    }
  }
}

/** Scan from stdin all the round infos (J, P, END)
 */
void get_round_infos() {
  char line[MAX_SIZE_WORD];
  for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
    scanf(" %[^\n]s\n", &line[0]);

    int J, P, END;

    sscanf((char *) &line, "%d %d %d", &J, &P, &END);
    game.players[J].score = P;
    game.players[J].ST = END;

    fprintf(stderr, "J: %d , P: %d , END: %d\n", J, P, END);
    fflush(stderr);
  }
}

/** Scan from stdin all players informations : can be (J P END) or (W J ST)
 */
void get_turn_infos() {
  char line[MAX_SIZE_WORD];
  for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
    scanf(" %[^\n]s\n", line);

    char first_string[MAX_SIZE_WORD]; // W or J ?
    int sec_info; // P or J ?
    int third_info; // END or ST ?
    sscanf(line, "%s %d %d", (char *) &first_string, &sec_info, &third_info);

    if (third_info == 2) {
      if (game.state.is_me_founded == 0) {
        game.state.is_me_founded = 1;
      }
      game.players[sec_info].ST = third_info;

      if (goddess.secret_words_index[game.state.round] == -1) { // Save the index of the secret word of the round
        char* word_to_save;
        if (algo1.candidate_word_list.size > 0) {
          word_to_save = algo1.last_word_proposed;
        } else {
          word_to_save = secret_word_save;
        }
        goddess.secret_words_index[game.state.round] = find_word_index_in_sorted_list(word_to_save);
      }

      fprintf(stderr, "W: %s , J: %d , ST: %d\n", first_string, sec_info, third_info);
      fflush(stderr);
    } else if (game.state.is_me_founded == 1 && third_info == 1) { // new round with (J, P, END)
      int player_i = atoi(first_string);
      game.players[player_i].score = sec_info;
      game.players[player_i].ST = third_info;
      game.state.is_all_founded = 1;

      fprintf(stderr, "J: %d , P: %d , END: %d\n", player_i, sec_info, third_info);
      fflush(stderr);
    } else { // (W J ST)
      game.players[sec_info].ST = third_info;

      if (strcmp(first_string, "NULL") != 0) { // Someone (can be me) proposed a word which is wrong
        remove_word_from_algos_word_list(&algo1, first_string);
        remove_word_from_algos_word_list(&algo2, first_string);
        remove_word_from_algos_word_list(&algo3, first_string);
        memset(algo1.last_word_proposed, 0, MAX_SIZE_WORD); // TODO : Créer une fonction qui clean
        memset(algo2.last_word_proposed, 0, MAX_SIZE_WORD);
        memset(algo3.last_word_proposed, 0, MAX_SIZE_WORD);
      }

      fprintf(stderr, "W: %s , J: %d , ST: %d\n", first_string, sec_info, third_info);
      fflush(stderr);
    }
  }

  if (algo3.candidate_word_list.size == 0 && algo2.candidate_word_list.size != 0) { // algo3 went wrong, we must stop algo2 if it's still running
    algo2.candidate_word_list.size = 0;
  }
}

/** Scan from stdin the concept gave by the Goddess
 */
void get_concept() {
  char line[MAX_SIZE_CONCEPT];
  scanf("%s", line);

  int new_size = list_concepts_of_round.size + 1;
  sscanf(line, "%s", list_concepts_of_round.list[new_size-1]);
  list_concepts_of_round.size = new_size;
}


/* Debug print */

/** Print in stderr all triplets (a, b, c) kept
 */
void print_all_valid_coeffs() {
  for (int i = 0; i < NB_COEFF*NB_COEFF*NB_COEFF; i++) {
    fprintf(stderr, "a : %d, b : %d, c : %d\n", goddess.t_coeff[i].a, goddess.t_coeff[i].b, goddess.t_coeff[i].c);
    fflush(stderr);
  }
}

/** Print in stderr the specified struct list_word for debug purposes
 * \param struct_list_to_print the address of the interested struct list_word
 */
void print_all_infos_candidate_word_list(struct list_word* struct_list_to_print) {
  for (int i = 0; i < NB_WORD; i++) {
    for (int j = 0; j < NB_CONCEPT; j++) {
      fprintf(stderr, "mot : %s, con : %s et s : %d\n", struct_list_to_print->list[i].v, struct_list_to_print->list[i].concepts[j], struct_list_to_print->list[i].scores[j]);
      fflush(stderr);
    }
    if (i == 49) {
      fprintf(stderr, "-----------------------------\n");
      fflush(stderr);
    }
  }
}


/* Output print */

/** Print the wanted pass string in stdout
 */
void print_pass() {
  fprintf(stderr, "JE PASSE\n");
  fflush(stderr);

  fprintf(stdout, "PASS\n");
  fflush(stdout);
}

/** Print the specified word string in stdout & store this string inside the wanted algo
 * \param algo the address of the struct algo_data we want to save
 * \param word the string to guess & to store
 */
void guess_word(struct algo_data* algo, char* word) {
  fprintf(stderr, "JE GUESS : %s\n", word);
  fflush(stderr);

  fprintf(stdout, "GUESS %s\n", word);
  fflush(stdout);

  strcpy(algo->last_word_proposed, word);
}

/** Algorithm that decide either it prints pass or a word
 */
void print_decision() {
  if (game.state.round == 0 || goddess.p.founded == 0) {
    if (list_concepts_of_round.size <= NB_TURN) {
      // if (goddess.p.founded == 1 && algo1.candidate_word_list.size == 1 && game.state.is_me_founded == 0) {
      if (algo1.candidate_word_list.size > 0 && game.state.is_me_founded == 0) {
        fprintf(stderr, "1 \n");
        fflush(stderr);
        int index_to_guess = get_random_int_in_range(0, algo1.candidate_word_list.size-1);
        guess_word(&algo1, algo1.candidate_word_list.list[index_to_guess].v);
      } else {
        fprintf(stderr, "2 \n");
        fflush(stderr);
        print_pass();
      }
    } else {
      if (game.state.is_me_founded == 0) { // can't pass anymore, must try something even if not sure
        if (algo1.candidate_word_list.size > 0) {
          fprintf(stderr, "3.1 \n");
          fflush(stderr);
          int index_to_guess = get_random_int_in_range(0, algo1.candidate_word_list.size-1);
          guess_word(&algo1, algo1.candidate_word_list.list[index_to_guess].v);
        } else { // in case all concepts erased every possible word
          fprintf(stderr, "3.2 \n");
          fflush(stderr);
          guess_word(&algo1, secret_word_save);
        }
      } else if (game.state.is_me_founded == 1 && game.state.round % 21 == 0) {
        print_pass();
      }
    }
  } else {
    if (list_concepts_of_round.size <= NB_TURN) {
      if (game.state.is_me_founded == 1) {
        fprintf(stderr, "4 \n");
        fflush(stderr);
        print_pass();
      } else {
        fprintf(stderr, "5 \n");
        fflush(stderr);
        struct algo_data* strongest_algo;

        int algo_num = 1;
        strongest_algo = &algo1;
        if (goddess.size_t_coeff > 0 && list_concepts_of_round.size > 0 && algo2.candidate_word_list.size > 0 && algo2.candidate_word_list.size < strongest_algo->candidate_word_list.size) {
          strongest_algo = &algo2;
          algo_num = 2;
        }
        if (goddess.size_t_coeff > 0 && list_concepts_of_round.size > 0 && algo3.candidate_word_list.size > 0 && algo3.candidate_word_list.size < strongest_algo->candidate_word_list.size) {
          strongest_algo = &algo3;
          algo_num = 3;
        }

        int index_to_guess;
        if (strongest_algo->candidate_word_list.size > 1) {
          if (goddess.size_t_coeff > 0 && list_concepts_of_round.size > 1) {
            // index_to_guess = get_index_word_based_on_abc(strongest_algo);
            index_to_guess = get_random_int_in_range(0, strongest_algo->candidate_word_list.size-1);
          } else {
            index_to_guess = get_random_int_in_range(0, strongest_algo->candidate_word_list.size-1);
          }
        } else {
          index_to_guess = 0;
        }
        fprintf(stderr, "De algo num %d\n", algo_num);
        fflush(stderr);
        guess_word(strongest_algo, strongest_algo->candidate_word_list.list[index_to_guess].v);
      }
    } else {
      if (game.state.is_me_founded == 0) { // can't pass anymore, must try something even if not sure
        fprintf(stderr, "6 \n");
        fflush(stderr);
        // Retrieve the algo that have the minest size of word possible
        struct algo_data* less_worse_algo;
        less_worse_algo = &algo1;
        if (goddess.size_t_coeff > 0 && algo1.candidate_word_list.size > algo2.candidate_word_list.size) {
          less_worse_algo = &algo2;
        }
        if (goddess.size_t_coeff > 0 && algo3.candidate_word_list.size < less_worse_algo->candidate_word_list.size) {
          less_worse_algo = &algo3;
        }

        int index_to_guess;
        if (less_worse_algo->candidate_word_list.size > 1) {
          if (goddess.size_t_coeff > 0) {
            // index_to_guess = get_index_word_based_on_abc(less_worse_algo);
            index_to_guess = get_random_int_in_range(0, less_worse_algo->candidate_word_list.size-1);
          } else {
            index_to_guess = get_random_int_in_range(0, less_worse_algo->candidate_word_list.size-1);
          }
        } else {
          index_to_guess = 0;
        }

        guess_word(less_worse_algo, less_worse_algo->candidate_word_list.list[index_to_guess].v);
      }
    }
  }
}


/* Algos */

/** Algo to guess the 'p' param of the goddess
 * Must be executed only if we know for sure what is the secret word
 * Logic (for simplicity, index from [1,50]) :
 *   - if the concept is at position 5, that means that the concept is one of the lowest score --> the 'max score concept' indexes can't go lower than index (50-15 =) 35.
 *   - And at contrary, if concept is at 35, the upper bound can't go higher than (10 - 5 =) 5.
 * With that, we can kind of play with the p bound from [3, 7] until the upper == lower which will correspond to the final p value of the goddess.
 *
 * \param start_index the index of the lower bound range
 * \param end_index the index of the upper bound range
 * \param index_sw the secret word's index in initial_sorted_word_list of the actual round
 */
void find_p(int start_index, int end_index, int index_sw) { // TODO : A optimiser
  for (int i = start_index; i < end_index+1; i++) {
    for (int j = 0; j < NB_CONCEPT; j++) {
      if (strcmp(list_concepts_of_round.list[i], initial_sorted_word_list.list[index_sw].concepts[j]) == 0) {
        if (j == 6) {
          goddess.p.upper = 3;
          goddess.p.lower = 3;
          goddess.p.founded = 1;
        } else if (j == 33) {
          goddess.p.upper = 7;
          goddess.p.lower = 7;
          goddess.p.founded = 1;
        } else {
          if (j < 7) { // Lower bound
            if (j > 2 && (10 - (j + 1)) < goddess.p.upper) {
              goddess.p.upper = (10 - (j + 1));
            }
          } else { // Upper bound
            if (j < 37 && (((NB_CONCEPT-1)-(j-1)) - 10) > goddess.p.lower) {
              goddess.p.lower = ((NB_CONCEPT-1)-(j-1)) - 10;
            }
          }
        }

        if (goddess.p.lower == goddess.p.upper) {
          goddess.p.founded = 1;
        }
        break;
      }
    }

    if (goddess.p.founded == 1)
      break;
  }

  fprintf(stderr, "p : [%d, %d]\n", goddess.p.lower, goddess.p.upper);
  fflush(stderr);
}

/** The algorithm 1 which plays with the p parameter to find the secret word.
*/
void ia_algo1() {
  clock_t tps;
  tps = clock();

  int algo1_list_size = algo1.candidate_word_list.size;
  fprintf(stderr, "Algo 1 début : %d\n", algo1.candidate_word_list.size);
  fflush(stderr);

  struct list_word new_candidat_list;
  init_new_list_word(&new_candidat_list, algo1_list_size);

  for (int i = 0; i < algo1.candidate_word_list.size; i++) {
    // Copy all the (10 - p) and (10 + p) concepts into temporary char* list
    char list_20_concept_of_word_i[(10 - goddess.p.lower) + (10 + goddess.p.upper)][MAX_SIZE_CONCEPT];
    int list_20_score_of_word_i[(10 - goddess.p.lower) + (10 + goddess.p.upper)];
    int count_list = 0;
    for (int j = 0; j < 10-goddess.p.lower; j++) {
      strcpy(list_20_concept_of_word_i[count_list], algo1.candidate_word_list.list[i].concepts[j]);
      list_20_score_of_word_i[count_list] = algo1.candidate_word_list.list[i].scores[j];
      count_list++;
    }

    for (int j = NB_CONCEPT - (10 + goddess.p.upper); j < NB_CONCEPT; j++) {
      strcpy(list_20_concept_of_word_i[count_list], algo1.candidate_word_list.list[i].concepts[j]);
      list_20_score_of_word_i[count_list] = algo1.candidate_word_list.list[i].scores[j];
      count_list++;
    }

    // Check if the word contains the last concept gave by goddess
    int is_concept_present = 0;
    for (int j = 0; j < count_list; j++) {
      if (strcmp(list_concepts_of_round.list[list_concepts_of_round.size-1], list_20_concept_of_word_i[j]) == 0) {
        is_concept_present = 1;
        break;
      }
    }

    // Check if the rest of possible 20 concepts of the word that have same t() & u() with last concept gave by goddess has lower score, if not remove word
    int remove_word = 0;
    if (list_concepts_of_round.size > 1 && is_concept_present == 1) {
      int t_[2];
      int u_[2];
      int s_[2];
      t_[0] = t(algo1.candidate_word_list.list[i].v, list_concepts_of_round.list[list_concepts_of_round.size - 1]);
      u_[0] = u(algo1.candidate_word_list.list[i].v, list_concepts_of_round.list[list_concepts_of_round.size - 1]);
      s_[0] = find_score_of_concept_by_word_position_in_algo_list(&algo1, list_concepts_of_round.list[list_concepts_of_round.size - 1], i);
      for (int j = 0; j < count_list; j++) {
        if (goddess.p.founded == 0) { // if p is not founded yet, we consider that each word has at least 3 lowest score concept & 13 highest score concept for SURE
          int ignore_index = 0;
          for (int k = 3; k < 11; k++) {
            if (j == k) {
              ignore_index = 1;
              break;
            }
          }
          if (ignore_index == 1) {
            continue;
          }
        }

        int is_concept_already_gave = 0;
        for (int k = 0; k < list_concepts_of_round.size; k++) {
          if (strcmp(list_20_concept_of_word_i[j], list_concepts_of_round.list[k]) == 0) {
            is_concept_already_gave = 1;
            break;
          }
        }

        if (is_concept_already_gave == 0) {
          t_[1] = t(algo1.candidate_word_list.list[i].v, list_20_concept_of_word_i[j]);
          u_[1] = u(algo1.candidate_word_list.list[i].v, list_20_concept_of_word_i[j]);
          s_[1] = list_20_score_of_word_i[j];

          if (t_[0] == t_[1] && u_[0] == u_[1]) {
            if (s_[1] > s_[0]) {
              remove_word = 1;
            }
            // If have the same score, they have same t_val --> ordered alphabetically
            else if (s_[1] == s_[0] && strcmp(list_concepts_of_round.list[list_concepts_of_round.size - 1], list_20_concept_of_word_i[j]) == 1) {
              remove_word = 1;
            }
          }
        }
      }
    } else if (is_concept_present == 0) {
      remove_word = 1;
    }

    if (remove_word == 0) {
      int new_size = new_candidat_list.size;
      strcpy(new_candidat_list.list[new_size].v, algo1.candidate_word_list.list[i].v);
      for (int j = 0; j < NB_CONCEPT; j++) {
        strcpy(new_candidat_list.list[new_size].concepts[j], algo1.candidate_word_list.list[i].concepts[j]);
        new_candidat_list.list[new_size].scores[j] = algo1.candidate_word_list.list[i].scores[j];
      }
      new_candidat_list.size++;
    }
  }

  copy_list_word_to_another(&(algo1.candidate_word_list), &new_candidat_list);
  free_list_word(&new_candidat_list);

  fprintf(stderr, "(Algo 1) Nombre restant : %d\n", algo1.candidate_word_list.size);
  fflush(stderr);

  if (algo1.candidate_word_list.size == 1) // save word in case all concepts gave erase every word (should not be possible however, I saw it happened once)
    strcpy(secret_word_save, algo1.candidate_word_list.list[0].v);

  tps = clock() - tps;
  double time_taken = ((double)tps)/CLOCKS_PER_SEC;
  fprintf(stderr, "Temps d'éxécution de l'algo 1: %f sec\n", time_taken);
  fflush(stderr);
}

/** First part of algo 2 that list all the triplets (a, b, c) that order the first 3 concepts gave by the Goddess, correctly.
 */
void algo2_find_abc() {
  // TODO : Créer une fonction qui réinitialise les coeffs à l'initiale
  if (goddess.size_t_coeff == 0) { // reset all posssible coeffs (a, b, c)
    int new_size_t_coeff = 0;
    for (int a = 0; a < NB_COEFF; a++) {
      for (int b = 0; b < NB_COEFF; b++) {
        for (int c = 0; c < NB_COEFF; c++) {
          goddess.t_coeff[new_size_t_coeff] = (struct coeffs) { .a = possible_coeff[a], .b = possible_coeff[b], .c = possible_coeff[c] };
          new_size_t_coeff++;
        }
      }
    }
    goddess.size_t_coeff = new_size_t_coeff;
  }

  struct coeffs new_possible_coeffs[goddess.size_t_coeff];
  int nb_coeffs_to_keep = 0;

  char top_3_concept[3][MAX_SIZE_CONCEPT];
  for (int i = 0; i < 3; i++) {
    strcpy(top_3_concept[i], list_concepts_of_round.list[i]);
  }

  int stu[3][3];// 's' score, 't' value and 'u' value
  for (int j = 0; j < 3; j++) {
    stu[j][0] = find_score_concept_with_word_position(top_3_concept[j], goddess.secret_words_index[game.state.round]); // 2nd param is secret word's index of considered round
    stu[j][1] = t(initial_sorted_word_list.list[goddess.secret_words_index[game.state.round]].v, top_3_concept[j]);
    stu[j][2] = u(initial_sorted_word_list.list[goddess.secret_words_index[game.state.round]].v, top_3_concept[j]);
  }

  int t_val_top_3_concept[3];
  for (int i = 0; i < goddess.size_t_coeff; i++) {
    int a_ = goddess.t_coeff[i].a;
    int b_ = goddess.t_coeff[i].b;
    int c_ = goddess.t_coeff[i].c;

    for (int j = 0; j < 3; j++) {
      t_val_top_3_concept[j] = a_ * stu[j][0] + 10 * b_ * stu[j][1] + 10 * c_ * stu[j][2];
    }

    char top_concept_sorted_by_tval[3][MAX_SIZE_CONCEPT];
    for (int j = 0; j < 3; j++) {
      strcpy(top_concept_sorted_by_tval[j], top_3_concept[j]);
    }

    // sort 3 top concept by t_val
    for (int j = 3 - 1; j > 0; j--) { // bubble sort // TODO : OPTIMIZE
      for (int k = 0; k < 3 -1; k++) {
        if (t_val_top_3_concept[k+1] > t_val_top_3_concept[k]) { // value order
          int tmp = t_val_top_3_concept[k+1];
          t_val_top_3_concept[k+1] = t_val_top_3_concept[k];
          t_val_top_3_concept[k] = tmp;

          char s_tmp[MAX_SIZE_CONCEPT];
          strcpy(s_tmp, top_concept_sorted_by_tval[k+1]);
          strcpy(top_concept_sorted_by_tval[k+1], top_concept_sorted_by_tval[k]);
          strcpy(top_concept_sorted_by_tval[k], s_tmp);
        } else if (t_val_top_3_concept[k+1] == t_val_top_3_concept[k]) { // alphabetic order
          if (strcmp(top_concept_sorted_by_tval[k], top_concept_sorted_by_tval[k+1]) == 1) {
            int tmp = t_val_top_3_concept[k+1];
            t_val_top_3_concept[k+1] = t_val_top_3_concept[k];
            t_val_top_3_concept[k] = tmp;

            char s_tmp[MAX_SIZE_CONCEPT];
            strcpy(s_tmp, top_concept_sorted_by_tval[k+1]);
            strcpy(top_concept_sorted_by_tval[k+1], top_concept_sorted_by_tval[k]);
            strcpy(top_concept_sorted_by_tval[k], s_tmp);
          }
        }
      }
    }

    int remove_coeffs = 0;
    for (int j = 0; j < 3; j++) {
      if (strcmp(top_concept_sorted_by_tval[j], top_3_concept[j]) != 0) {
        remove_coeffs = 1;
        break;
      }
    }

    if (remove_coeffs == 0) { // we keep the coeffs
      new_possible_coeffs[nb_coeffs_to_keep] = (struct coeffs) { .a = a_, .b = b_, .c = c_};
      nb_coeffs_to_keep++;
    }
  }

  // copy all the new coeff to keep
  for (int i = 0; i < nb_coeffs_to_keep; i++) {
    goddess.t_coeff[i] = new_possible_coeffs[i];
  }
  goddess.size_t_coeff = nb_coeffs_to_keep;

  // check coeff to keep
  fprintf(stderr, "Nombre de coeff à garder : %d\n", nb_coeffs_to_keep);
  fflush(stderr);

  if (nb_coeffs_to_keep > 0) {
    /*
    for (int i = 0; i < nb_coeffs_to_keep; i++) {
      fprintf(stderr, "a : %d, b : %d, c : %d\n", new_possible_coeffs[i].a, new_possible_coeffs[i].b, new_possible_coeffs[i].c);
      fflush(stderr);
    }
    */

    // mean of all coeffs
    for (int i = 0; i < nb_coeffs_to_keep; i++) {
      mean_coeffs.a += new_possible_coeffs[i].a;
      mean_coeffs.b += new_possible_coeffs[i].b;
      mean_coeffs.c += new_possible_coeffs[i].c;
    }
    mean_coeffs.a /= nb_coeffs_to_keep;
    mean_coeffs.b /= nb_coeffs_to_keep;
    mean_coeffs.c /= nb_coeffs_to_keep;

    /*
    fprintf(stderr, "a_mean : %d, b_mean : %d, c_mean : %d\n", mean_coeffs.a, mean_coeffs.b, mean_coeffs.c);
    fflush(stderr);
    */
  }
}

/** The algorithm 2 which plays with triplets of (a, b, c) to find the secret word.
*/
void ia_algo2() {
  clock_t tps;
  tps = clock();

  int algo2_list_size = algo2.candidate_word_list.size;

  fprintf(stderr, "Algo 2 début : %d\n", algo2.candidate_word_list.size);
  fflush(stderr);

  struct list_word new_candidat_list;
  init_new_list_word(&new_candidat_list, algo2_list_size);

  char last_two_concept[2][MAX_SIZE_CONCEPT];
  strcpy(last_two_concept[0], list_concepts_of_round.list[list_concepts_of_round.size - 2]); // 1 before last concept
  strcpy(last_two_concept[1], list_concepts_of_round.list[list_concepts_of_round.size - 1]); // last concept

  for (int i = 0; i < algo2_list_size; i++) {
    int remove_word = 1;

    for (int j = 0; j < goddess.size_t_coeff; j++) {
      int t_val_last_2_concept[2];
      int score_last_2_concept[2];

      for (int k = 0; k < 2; k++) {
        score_last_2_concept[k] = find_score_of_concept_by_word_position_in_algo_list(&algo2, last_two_concept[k], i);
        t_val_last_2_concept[k] = goddess.t_coeff[j].a * score_last_2_concept[k] + 10 * goddess.t_coeff[j].b * t(algo2.candidate_word_list.list[i].v, last_two_concept[k]) + 10 * goddess.t_coeff[j].c * u(last_two_concept[k], algo2.candidate_word_list.list[i].v);
      }

      if (t_val_last_2_concept[0] > t_val_last_2_concept[1]) {
        remove_word = 0;
        break;
      } else if (t_val_last_2_concept[0] == t_val_last_2_concept[1]) { // TODO : vérifier avec cette condition
        if (strcmp(last_two_concept[0], last_two_concept[1]) == -1) { // if the 2 concepts have the same t_val, the first concept must be first alphabetically
          remove_word = 0;
          break;
        }
      }
    }

    if (remove_word == 0) { // We keep the word
      int new_size = new_candidat_list.size;
      strcpy(new_candidat_list.list[new_size].v, algo2.candidate_word_list.list[i].v);
      for (int j = 0; j < NB_CONCEPT; j++) {
        strcpy(new_candidat_list.list[new_size].concepts[j], algo2.candidate_word_list.list[i].concepts[j]);
        new_candidat_list.list[new_size].scores[j] = algo2.candidate_word_list.list[i].scores[j];
      }
      new_candidat_list.size++;
    }
  }

  copy_list_word_to_another(&(algo2.candidate_word_list), &new_candidat_list);
  free_list_word(&new_candidat_list);

  fprintf(stderr, "(Algo 2) Nombre restant : %d\n", algo2.candidate_word_list.size);
  fflush(stderr);

  tps = clock() - tps;
  double time_taken = ((double)tps)/CLOCKS_PER_SEC;
  fprintf(stderr, "Temps d'éxécution de l'algo 2: %f sec\n", time_taken);
  fflush(stderr);
}

/** The algorithm 3 which plays with intersection of candidate_word_list of algo1 & 2 to have its own list.
 */
void ia_algo3() {
  clock_t tps;
  tps = clock();

  fprintf(stderr, "Algo 3 début : %d\n", algo3.candidate_word_list.size);
  fflush(stderr);

  struct algo_data* smaller;
  struct algo_data* bigger;

  if (algo1.candidate_word_list.size >= algo2.candidate_word_list.size) {
    bigger = &algo1;
    smaller = &algo2;
  } else {
    bigger = &algo2;
    smaller = &algo1;
  }

  int smaller_size = smaller->candidate_word_list.size;

  struct list_word intersection_of_candidate_word;
  init_new_list_word(&intersection_of_candidate_word, smaller_size);

  for (int i = 0; i < smaller->candidate_word_list.size; i++) {
    for (int j = 0; j < bigger->candidate_word_list.size; j++) {
      if (strcmp(smaller->candidate_word_list.list[i].v, bigger->candidate_word_list.list[j].v) == 0) {
        strcpy(intersection_of_candidate_word.list[intersection_of_candidate_word.size].v, smaller->candidate_word_list.list[i].v);
        for (int k = 0; k < NB_CONCEPT; k++) {
          strcpy(intersection_of_candidate_word.list[intersection_of_candidate_word.size].concepts[k], smaller->candidate_word_list.list[i].concepts[k]);
          intersection_of_candidate_word.list[intersection_of_candidate_word.size].scores[k] = smaller->candidate_word_list.list[i].scores[k];
        }
        intersection_of_candidate_word.size++;
        break;
      }
    }
  }

  struct list_word* smaller_lw;
  struct list_word* bigger_lw;
  if (intersection_of_candidate_word.size >= algo3.candidate_word_list.size) {
    bigger_lw = &intersection_of_candidate_word;
    smaller_lw = &algo3.candidate_word_list;
  } else {
    smaller_lw = &intersection_of_candidate_word;
    bigger_lw = &algo3.candidate_word_list;
  }

  smaller_size = smaller_lw->size;
  struct list_word final_inter_of_candidate_word;
  init_new_list_word(&final_inter_of_candidate_word, smaller_size);

  for (int i = 0; i < smaller_lw->size; i++) {
    for (int j = 0; j < bigger_lw->size; j++) {
      if (strcmp(smaller_lw->list[i].v, bigger_lw->list[j].v) == 0) {
        strcpy(final_inter_of_candidate_word.list[final_inter_of_candidate_word.size].v, smaller_lw->list[i].v);
        for (int k = 0; k < NB_CONCEPT; k++) {
          strcpy(final_inter_of_candidate_word.list[final_inter_of_candidate_word.size].concepts[k], smaller_lw->list[i].concepts[k]);
          final_inter_of_candidate_word.list[final_inter_of_candidate_word.size].scores[k] = smaller_lw->list[i].scores[k];
        }
        final_inter_of_candidate_word.size++;
        break;
      }
    }
  }

  // Copy the final intersection into the algo3 candidate word list
  struct list_word* algo_to_keep;
  if (final_inter_of_candidate_word.size != 0) {
    algo_to_keep = &final_inter_of_candidate_word;
  } else {
    algo_to_keep = &intersection_of_candidate_word;
  }

  if (algo_to_keep->size == 0) { // that means that algo 2 has no commum words with algo1: Algo 2 made a mistake !! No need to use algo2 and 3 anymore
    algo2.candidate_word_list.size = 0;
    algo3.candidate_word_list.size = 0;
    fprintf(stderr, "Algo 2 et 3 ne sont plus valides !\n");
    fflush(stderr);
  } else {
    copy_list_word_to_another(&(algo3.candidate_word_list), algo_to_keep);
  }
  free_list_word(&intersection_of_candidate_word);
  free_list_word(&final_inter_of_candidate_word);

  fprintf(stderr, "(Algo 3) Nombre restant : %d\n", algo3.candidate_word_list.size);
  fflush(stderr);

  tps = clock() - tps;
  double time_taken = ((double)tps)/CLOCKS_PER_SEC;
  fprintf(stderr, "Temps d'éxécution de l'algo 3: %f sec\n", time_taken);
  fflush(stderr);
}

/** The function that select which Algo to execute depending on several criterias
 * algo 1:
 *  - executed every time
 * algo 2 if :
 *  - after first round & have at least one triplets (a, b, c) valid & have at least one concept gave by goddess & not empty candidate word list & p param is founded
 * algo 3 if :
 *  - algo 2 is working & candidate word list of algo 1, 2 and its own are not empty
 */
void all_algo() {
  clock_t t;
  t = clock();

  ia_algo1();
  if (game.state.round != 0 && goddess.size_t_coeff > 0 && list_concepts_of_round.size > 1 && algo2.candidate_word_list.size > 0) {
    if (list_concepts_of_round.size == 2) { // copy candidate_word_list of algo1 in algo2 & algo3
      for (int i = 0; i < NB_WORD; i++) { // do both copy in same loop is more effective than using a function to copy individually
        strcpy(algo2.candidate_word_list.list[i].v, algo1.candidate_word_list.list[i].v);
        strcpy(algo3.candidate_word_list.list[i].v, algo1.candidate_word_list.list[i].v);
        for (int j = 0; j < NB_CONCEPT; j++) {
          strcpy(algo2.candidate_word_list.list[i].concepts[j], algo1.candidate_word_list.list[i].concepts[j]);
          strcpy(algo3.candidate_word_list.list[i].concepts[j], algo1.candidate_word_list.list[i].concepts[j]);
          algo2.candidate_word_list.list[i].scores[j] = algo1.candidate_word_list.list[i].scores[j];
          algo3.candidate_word_list.list[i].scores[j] = algo1.candidate_word_list.list[i].scores[j];
        }
      }
      algo2.candidate_word_list.size = algo1.candidate_word_list.size;
      algo3.candidate_word_list.size = algo1.candidate_word_list.size;
    }
    ia_algo2();
    if (algo1.candidate_word_list.size > 1 && algo2.candidate_word_list.size > 1 && algo3.candidate_word_list.size != 0) {
      ia_algo3();
    }
  }

  t = clock() - t;
  double time_taken = ((double)t)/(CLOCKS_PER_SEC/1000);
  fprintf(stderr, "Temps d'éxécution : %f milli\n", time_taken);
  fflush(stderr);
}


int main() {
  init_global_variables();

  /* get first game informations */
  char line[MAX_SIZE_WORD];
  scanf(" %[^\n]s\n", line);
  sscanf(line, "%d %d", &game.NJ, &game.me_nb_j);
  game.players = malloc(game.NJ * sizeof(struct player));
  fprintf(stderr, "Récup game infos : %s\n", line);
  fflush(stderr);

  get_all_words_and_concepts();

  sort_concept_by_score_asc();
  // print_all_infos_candidate_word_list(&initial_sorted_word_list); // print the list for debug

  copy_sorted_list_in_algos(&algo1);
  copy_sorted_list_in_algos(&algo2);
  copy_sorted_list_in_algos(&algo3);

  int* round = &(game.state.round);

  for (*round = 0; *round < NB_ROUND; (*round)++) {
    for (int turn = 1; turn < NB_TURN + 3; turn++) {
      fprintf(stderr, "start turn %d\n", game.state.turn);
      fflush(stderr);

      if (turn == 1) {
        get_round_infos();
      } else if (turn == 22) {
        fprintf(stderr, "last turn :\n");
        fflush(stderr);
      } else {
        get_concept();
        if (game.state.is_me_founded == 0) {
          all_algo();
        }
        if (goddess.p.founded == 0 && algo1.candidate_word_list.size == 1)
          find_p(0, list_concepts_of_round.size-1, find_word_index_in_sorted_list(algo1.candidate_word_list.list[0].v));

        print_decision();
        get_turn_infos();
      }
      game.state.turn++;
    }

    if (goddess.secret_words_index[*round] != -1) { // we found the secret round at that round
      if (goddess.p.founded == 0)
        find_p(0, list_concepts_of_round.size-1, goddess.secret_words_index[*round]);
      if (*round == 0 || (goddess.size_t_coeff == 0 && list_concepts_of_round.size > 2)) {
        algo2_find_abc();
      }
    }

    reset_game_state();
    fprintf(stderr, "reset game, nouveau round : %d \n", game.state.round);
    fflush(stderr);
  }

  fprintf(stderr, "END OF GAME\n");
  fflush(stderr);

  free_global_variables();

  return 0;
}
