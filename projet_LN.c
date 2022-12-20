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

/**
 \ All prototypes // TODO : To complete
 */
int t(char*, char*);
int u(char*, char*);

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
  int secret_words_index[5];
};


struct algo_data {
  struct list_word candidate_word_list;
  char* last_word_proposed;
};


/* Global variables */
static struct list_word initial_sorted_word_list = { NULL, 0 };
static struct list_concept list_concepts_of_round = { NULL, 0 };
static struct algo_data algo1 = { { NULL, 0 }, NULL }; // only p strat
static struct algo_data algo2 = { { NULL, 0 }, NULL }; // only with (a, b, c) params
static struct algo_data algo3 = { { NULL, 0 }, NULL }; // with both p & (a, b, c)
static struct goddess_param goddess = { { 0, 0, 0 }, NULL, 0, {-1, -1, -1, -1, -1} };
static struct game_config game = { NULL, 0, 0, { 0, 0, 0, 0 } };
static int possible_coeff[NB_COEFF] = {5, 35, 65, 95};

void print_all_valid_coeffs() {
  for (int i = 0; i < NB_COEFF*NB_COEFF*NB_COEFF; i++) {
    fprintf(stderr, "a : %d, b : %d, c : %d\n", goddess.t_coeff[i].a, goddess.t_coeff[i].b, goddess.t_coeff[i].c);
    fflush(stderr);
  }
}

/* All functions */
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

  // print_all_valid_coeffs();
}

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

void print_concepts_words_list(struct list_word* struct_list_to_print) {
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

          tmp_s = initial_sorted_word_list.list[i].concepts[k + 1];
          initial_sorted_word_list.list[i].concepts[k + 1] = initial_sorted_word_list.list[i].concepts[k];
          initial_sorted_word_list.list[i].concepts[k] = tmp_s;
        } else if (initial_sorted_word_list.list[i].scores[k + 1] == initial_sorted_word_list.list[i].scores[k]) {
          if (strcmp(initial_sorted_word_list.list[i].concepts[k], initial_sorted_word_list.list[i].concepts[k+1]) == 1) {
            tmp_i = initial_sorted_word_list.list[i].scores[k + 1];
            initial_sorted_word_list.list[i].scores[k + 1] = initial_sorted_word_list.list[i].scores[k];
            initial_sorted_word_list.list[i].scores[k] = tmp_i;

            tmp_s = initial_sorted_word_list.list[i].concepts[k + 1];
            initial_sorted_word_list.list[i].concepts[k + 1] = initial_sorted_word_list.list[i].concepts[k];
            initial_sorted_word_list.list[i].concepts[k] = tmp_s;
          }
        }
      }
    }
  }
  free(tmp_s);
}

void copy_sorted_list_in_algos(struct algo_data* algo) {
  for (int i = 0; i < NB_WORD; i++) {
    algo->candidate_word_list.list[i].v = initial_sorted_word_list.list[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      algo->candidate_word_list.list[i].concepts[j] = initial_sorted_word_list.list[i].concepts[j];
      algo->candidate_word_list.list[i].scores[j] = initial_sorted_word_list.list[i].scores[j];
    }
  }

  // print_concepts_words_list(&algo->candidate_word_list);
  // fprintf(stderr, "taille final : %d\n", algo->candidate_word_list.size);
  // fflush(stderr);
}

void reset_game_state() {
  game.state.is_all_founded = 0;
  game.state.is_me_founded = 0;

  memset(algo1.last_word_proposed, 0, MAX_SIZE_WORD);
  memset(algo2.last_word_proposed, 0, MAX_SIZE_WORD);
  memset(algo3.last_word_proposed, 0, MAX_SIZE_WORD);

  // reset the list of concept
  list_concepts_of_round.size = 0;

  algo1.candidate_word_list.size = NB_WORD;
  algo2.candidate_word_list.size = NB_WORD;
  algo3.candidate_word_list.size = NB_WORD;

  copy_sorted_list_in_algos(&algo1);
  copy_sorted_list_in_algos(&algo2);
  copy_sorted_list_in_algos(&algo3);
}

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

int find_word_index_in_sorted_list(char* word) {
  for (int i = 0; i < NB_WORD; i++) {
    if (strcmp(word, initial_sorted_word_list.list[i].v) == 0) {
      fprintf(stderr, "secret w : %s\n", initial_sorted_word_list.list[i].v);
      fflush(stderr);
      return i;
    }
  }
}

void remove_word_from_algos_word_list(struct algo_data* algo, char* word) {
  for (int i = 0; i < algo->candidate_word_list.size; i++) {
    if (strcmp(algo->candidate_word_list.list[i].v, word) == 0) {
      int last_element_index = (algo->candidate_word_list.size)-1;
      for (int j = i; j < last_element_index; j++) {
        algo->candidate_word_list.list[j].v = algo->candidate_word_list.list[j+1].v;
        for (int k = 0; k < NB_CONCEPT; k++) {
          algo->candidate_word_list.list[j].concepts[k] = algo->candidate_word_list.list[j+1].concepts[k];
          algo->candidate_word_list.list[j].scores[k] = algo->candidate_word_list.list[j+1].scores[k];
        }
      }
      algo->candidate_word_list.size = last_element_index;
      break;
    }
  }
}

/* Get all players informations : can be (J P END) or (W J ST)
 *
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
        goddess.secret_words_index[game.state.round] = find_word_index_in_sorted_list(algo1.last_word_proposed);
      }

      fprintf(stderr, "W: %s , J: %d , ST: %d\n", first_string, sec_info, third_info);
      fflush(stderr);
    } else if (game.state.is_me_founded == 1 & third_info == 1) { // new round with (J, P, END)
      int player_i = atoi(first_string);
      game.players[player_i].score = sec_info;
      game.players[player_i].ST = third_info;
      game.state.is_all_founded = 1;

      fprintf(stderr, "J: %d , P: %d , END: %d\n", player_i, sec_info, third_info);
      fflush(stderr);
    } else { // (W J ST)
      game.players[sec_info].ST = third_info;

      fprintf(stderr, "W: %s , J: %d , ST: %d\n", first_string, sec_info, third_info);
      fflush(stderr);
    }
  }

  if (game.state.is_me_founded == 0) {
    struct algo_data* algo_to_clean;

    if (strlen(algo1.last_word_proposed) != 0) {
      algo_to_clean = &algo1;
    } else if (strlen(algo2.last_word_proposed) != 0) {
      algo_to_clean = &algo2;
    } else if (strlen(algo3.last_word_proposed) != 0) {
      algo_to_clean = &algo3;
    } else {
      algo_to_clean = NULL;
    }

    if (algo_to_clean != NULL) {
      remove_word_from_algos_word_list(algo_to_clean, algo_to_clean->last_word_proposed);
      memset(algo_to_clean->last_word_proposed, 0, MAX_SIZE_WORD);
    }
  }
}

void get_concept() {
  char line[MAX_SIZE_CONCEPT];
  scanf("%s", line);

  int new_size = list_concepts_of_round.size + 1;
  sscanf(line, "%s", list_concepts_of_round.list[new_size-1]);
  list_concepts_of_round.size = new_size;

  fprintf(stderr, "Récupération mot : %s, taille = %d \n", list_concepts_of_round.list[new_size-1], list_concepts_of_round.size);
  fflush(stderr);
}

void ia_algo1() {
  clock_t tps;
  tps = clock();

  fprintf(stderr, "Je commence avec : %d\n", algo1.candidate_word_list.size);
  fflush(stderr);

  // print_concepts_words_list(&algo1.candidate_word_list);
  int algo1_list_size = algo1.candidate_word_list.size;
  /* // TODO : Voir pq ca marche pas cette manière
  struct list_word new_candidat_list;

  struct word list[algo1_list_size];
  new_candidat_list.list = &list[0];

  char v[algo1_list_size][MAX_SIZE_WORD];
  char concepts[algo1_list_size][NB_CONCEPT][MAX_SIZE_WORD];
  for (int i = 0; i < algo1_list_size; i++) {
    new_candidat_list.list[i].v = &v[i];
    for (int j = 0; j < NB_CONCEPT; j++) {
      new_candidat_list.list[i].concepts[j] = &concepts[i][j];
    }
  }
  new_candidat_list.size = 0;
  */

  struct list_word new_candidat_list;
  new_candidat_list.list = malloc(algo1_list_size * sizeof(struct word));
  for (int i = 0; i < algo1_list_size; i++) {
    new_candidat_list.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      new_candidat_list.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }
  new_candidat_list.size = 0;

  for (int i = 0; i < algo1.candidate_word_list.size; i++) {
    int partie_basse = 0;
    for (int j = 0; j < 10-goddess.p.lower; j++) {
      if (strcmp(list_concepts_of_round.list[list_concepts_of_round.size-1], algo1.candidate_word_list.list[i].concepts[j]) == 0) {
        partie_basse = 1;
        break;
      }
    }

    int partie_haute = 0;
    if (partie_basse == 0) {
      for (int j = NB_CONCEPT - (10 + goddess.p.upper); j < NB_CONCEPT; j++) {
        if (strcmp(list_concepts_of_round.list[list_concepts_of_round.size-1], algo1.candidate_word_list.list[i].concepts[j]) == 0) {
          partie_haute = 1;
          break;
        }
      }
    }

    if (partie_basse == 1 || partie_haute == 1) {
      int new_size = new_candidat_list.size;
      new_candidat_list.list[new_size].v = algo1.candidate_word_list.list[i].v;
      for (int j = 0; j < NB_CONCEPT; j++) {
        new_candidat_list.list[new_size].concepts[j] = algo1.candidate_word_list.list[i].concepts[j];
        new_candidat_list.list[new_size].scores[j] = algo1.candidate_word_list.list[i].scores[j];
      }
      new_candidat_list.size++;
    }
  }

  for (int i = 0; i < new_candidat_list.size; i++) {
    algo1.candidate_word_list.list[i].v = new_candidat_list.list[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      algo1.candidate_word_list.list[i].concepts[j] = new_candidat_list.list[i].concepts[j];
      algo1.candidate_word_list.list[i].scores[j] = new_candidat_list.list[i].scores[j];
    }
  }
  algo1.candidate_word_list.size = new_candidat_list.size;

  /*
  for (int i = 0; i < algo1.candidate_word_list.size; i++) {
    fprintf(stderr, "mot restant : %s\n", algo1.candidate_word_list.list[i].v);
    fflush(stderr);
  }
  */

  fprintf(stderr, "(Algo 1) Nombre restant : %d\n", algo1.candidate_word_list.size);
  fflush(stderr);

  tps = clock() - tps;
  double time_taken = ((double)tps)/CLOCKS_PER_SEC;
  fprintf(stderr, "Temps d'éxécution de l'algo 1: %f sec\n", time_taken);
  fflush(stderr);
}

/**
 \ Find the score of an concept associated with the word at position i, in the wanted algo's candidate list
 \
 */
int find_score_of_concept_by_word_position_in_algo_list(struct algo_data* algo, char* concept, int index) {
  for (int i = 0; i < NB_CONCEPT; i++) {
    if (strcmp(algo->candidate_word_list.list[index].concepts[i], concept) == 0) {
      return algo->candidate_word_list.list[index].scores[i];
    }
  }
  return -1;
}

void ia_algo2() {
  clock_t tps;
  tps = clock();

  int algo2_list_size = algo2.candidate_word_list.size;


  struct list_word new_candidat_list;
  new_candidat_list.list = malloc(algo2_list_size * sizeof(struct word));
  for (int i = 0; i < algo2_list_size; i++) {
    new_candidat_list.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      new_candidat_list.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }
  new_candidat_list.size = 0;

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
        t_val_last_2_concept[k] = goddess.t_coeff[j].a * score_last_2_concept[k] + 10 * goddess.t_coeff[j].b * t(last_two_concept[k], algo2.candidate_word_list.list[i].v) + 10 * goddess.t_coeff[j].c * u(last_two_concept[k], algo2.candidate_word_list.list[i].v);
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
      new_candidat_list.list[new_size].v = algo2.candidate_word_list.list[i].v;
      for (int j = 0; j < NB_CONCEPT; j++) {
        new_candidat_list.list[new_size].concepts[j] = algo2.candidate_word_list.list[i].concepts[j];
        new_candidat_list.list[new_size].scores[j] = algo2.candidate_word_list.list[i].scores[j];
      }
      new_candidat_list.size++;
    }
  }

  for (int i = 0; i < new_candidat_list.size; i++) {
    algo2.candidate_word_list.list[i].v = new_candidat_list.list[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      algo2.candidate_word_list.list[i].concepts[j] = new_candidat_list.list[i].concepts[j];
      algo2.candidate_word_list.list[i].scores[j] = new_candidat_list.list[i].scores[j];
    }
  }
  algo2.candidate_word_list.size = new_candidat_list.size;

  fprintf(stderr, "(Algo 2) Nombre restant : %d\n", algo2.candidate_word_list.size);
  fflush(stderr);

  tps = clock() - tps;
  double time_taken = ((double)tps)/CLOCKS_PER_SEC;
  fprintf(stderr, "Temps d'éxécution de l'algo 2: %f sec\n", time_taken);
  fflush(stderr);
}

void ia_algo3() {
  clock_t tps;
  tps = clock();

  struct algo_data* smaller;
  struct algo_data* bigger;

  if (algo1.candidate_word_list.size >= algo2.candidate_word_list.size) {
    bigger = &algo1;
    smaller = &algo2;
  } else {
    bigger = &algo2;
    smaller = &algo1;
  }

  struct list_word intersection_of_candidate_word;
  int smaller_size = smaller->candidate_word_list.size;
  intersection_of_candidate_word.list = malloc(smaller_size * sizeof(struct word));
  for (int i = 0; i < smaller_size; i++) {
    intersection_of_candidate_word.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      intersection_of_candidate_word.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }
  intersection_of_candidate_word.size = 0;

  for (int i = 0; i < smaller->candidate_word_list.size; i++) {
    for (int j = 0; j < bigger->candidate_word_list.size; j++) {
      if (strcmp(smaller->candidate_word_list.list[i].v, bigger->candidate_word_list.list[j].v) == 0) {
        intersection_of_candidate_word.list[intersection_of_candidate_word.size].v = smaller->candidate_word_list.list[i].v;
        for (int k = 0; k < NB_CONCEPT; k++) {
          intersection_of_candidate_word.list[intersection_of_candidate_word.size].concepts[k] = smaller->candidate_word_list.list[i].concepts[k];
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

  struct list_word final_inter_of_candidate_word;
  smaller_size = smaller_lw->size;
  final_inter_of_candidate_word.list = malloc(smaller_size * sizeof(struct word));
  for (int i = 0; i < smaller_size; i++) {
    final_inter_of_candidate_word.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    for (int j = 0; j < NB_CONCEPT; j++) {
      final_inter_of_candidate_word.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }
  final_inter_of_candidate_word.size = 0;

  for (int i = 0; i < smaller_lw->size; i++) {
    for (int j = 0; j < bigger_lw->size; j++) {
      if (strcmp(smaller_lw->list[i].v, bigger_lw->list[j].v) == 0) {
        final_inter_of_candidate_word.list[final_inter_of_candidate_word.size].v = smaller_lw->list[i].v;
        for (int k = 0; k < NB_CONCEPT; k++) {
          final_inter_of_candidate_word.list[final_inter_of_candidate_word.size].concepts[k] = smaller_lw->list[i].concepts[k];
          final_inter_of_candidate_word.list[final_inter_of_candidate_word.size].scores[k] = smaller_lw->list[i].scores[k];
        }
        final_inter_of_candidate_word.size++;
        break;
      }
    }
  }

  // Copy the final intersection into the algo3 candidate word list
  for (int i = 0; i < final_inter_of_candidate_word.size; i++) {
    algo3.candidate_word_list.list[i].v = final_inter_of_candidate_word.list[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      algo3.candidate_word_list.list[i].concepts[j] = final_inter_of_candidate_word.list[i].concepts[j];
      algo3.candidate_word_list.list[i].scores[j] = final_inter_of_candidate_word.list[i].scores[j];
    }
  }
  algo3.candidate_word_list.size = final_inter_of_candidate_word.size;

  fprintf(stderr, "(Algo 3) Nombre restant : %d\n", algo3.candidate_word_list.size);
  fflush(stderr);

  tps = clock() - tps;
  double time_taken = ((double)tps)/CLOCKS_PER_SEC;
  fprintf(stderr, "Temps d'éxécution de l'algo 3: %f sec\n", time_taken);
  fflush(stderr);
}

int find_score_concept_with_word_position(char* concept, int w_pos) {
  for (int i = 0; i < NB_CONCEPT; i++) {
    if (strcmp(initial_sorted_word_list.list[w_pos].concepts[i], concept)) {
      return initial_sorted_word_list.list[w_pos].scores[i];
    }
  }
  return -1; // TODO : tester l'erreur
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

int u(char* m, char* w) {
  return abs((int) strlen(m) - (int) strlen(w));
}

/* Algo 2 to find goddess param (a, b, c)
 * Only to use at the end of first round
 */
void algo2_find_abc() {
  struct coeffs new_possible_coeffs[goddess.size_t_coeff];
  int nb_coeffs_to_keep = 0;

  char top_3_concept[3][MAX_SIZE_CONCEPT];
  for (int i = 0; i < 3; i++) {
    strcpy(top_3_concept[i], list_concepts_of_round.list[i]);
  }

  int stu[3][3];// 's' score, 't' value and 'u' value
  for (int j = 0; j < 3; j++) {
    stu[j][0] = find_score_concept_with_word_position(top_3_concept[j], goddess.secret_words_index[0]); // 2nd param is secret word's index of first round
    stu[j][1] = t(top_3_concept[j], initial_sorted_word_list.list[goddess.secret_words_index[0]].v);
    stu[j][2] = u(top_3_concept[j], initial_sorted_word_list.list[goddess.secret_words_index[0]].v);
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
  for (int i = 0; i < nb_coeffs_to_keep; i++) {
    fprintf(stderr, "a : %d, b : %d, c : %d\n", new_possible_coeffs[i].a, new_possible_coeffs[i].b, new_possible_coeffs[i].c);
    fflush(stderr);
  }
}

void all_algo() {
  clock_t t;
  t = clock();

  ia_algo1();
  if (game.state.round != 0 && goddess.size_t_coeff > 0 && list_concepts_of_round.size > 1) {
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

/* All print methods */
void print_pass() {
  fprintf(stderr, "JE PASSE\n");
  fflush(stderr);

  fprintf(stdout, "PASS\n");
  fflush(stdout);
}

void guess_word(struct algo_data* algo, char* word) {
  fprintf(stderr, "JE GUESS : %s\n", word);
  fflush(stderr);

  fprintf(stdout, "GUESS %s\n", word);
  fflush(stdout);

  algo->last_word_proposed = word;
}

int get_random_int_in_range(int min, int max) {
  srand(time(0));
  return (rand() % (max - min + 1)) + min;
}

void print_decision() {
  if (game.state.round == 0) {
    if (list_concepts_of_round.size < NB_TURN) {
      fprintf(stderr, "1 \n");
      fflush(stderr);
      print_pass();
    } else {
      if (game.state.is_me_founded == 0) { // can't pass anymore, must try something even if not sure
        fprintf(stderr, "2 \n");
        fflush(stderr);
        int index_to_guess = get_random_int_in_range(0, algo1.candidate_word_list.size-1);
        guess_word(&algo1, algo1.candidate_word_list.list[index_to_guess].v);
      }
    }
  } else {
    if (list_concepts_of_round.size <= NB_TURN) {
      if (game.state.is_me_founded == 1) {
        fprintf(stderr, "3 \n");
        fflush(stderr);
        print_pass();
      } else {
        struct algo_data* strongest_algo;
        if (algo1.candidate_word_list.size == 1) {
          strongest_algo = &algo1;
        } else if (algo2.candidate_word_list.size == 1){
          strongest_algo = &algo2;
        } else if (algo3.candidate_word_list.size == 1){
          strongest_algo = &algo3;
        } else {
          strongest_algo = NULL;
        }

        if (strongest_algo == NULL) {
          print_pass();
        } else {
          guess_word(strongest_algo, strongest_algo->candidate_word_list.list[0].v);
        }
      }
    } else {
      if (game.state.is_me_founded == 0) { // can't pass anymore, must try something even if not sure
        // Retrieve the algo that have the minest size of word possible
        struct algo_data* less_worse_algo;
        if (algo1.candidate_word_list.size <= algo2.candidate_word_list.size) {
          less_worse_algo = &algo1;
        } else {
          less_worse_algo = &algo2;
        }
        if (algo3.candidate_word_list.size < less_worse_algo->candidate_word_list.size) {
          less_worse_algo = &algo3;
        }

        int index_to_guess = get_random_int_in_range(0, less_worse_algo->candidate_word_list.size-1);
        guess_word(less_worse_algo, less_worse_algo->candidate_word_list.list[index_to_guess].v);
      }
    }
  }
}

/* Algo to guess the 'p' param of the goddess
 * Supposed to only be executed at the end of first round
 */
void find_p() { // TODO : A optimiser
  int tab[NB_CONCEPT] = {0};


  for (int i = 0; i < NB_TURN; i++) {
    for (int j = 0; j < NB_CONCEPT; j++) {
      if (strcmp(list_concepts_of_round.list[i], initial_sorted_word_list.list[goddess.secret_words_index[game.state.round]].concepts[j]) == 0) {
        tab[j] = 1;
        break;
      }
    }

    // p can be found faster if it is at position 6 or 33
    if (tab[6] == 1) {
      goddess.p.lower = 3;
      goddess.p.upper = 3;
      goddess.p.founded = 1;
      break;
    }
    if (tab[33] == 1) {
      goddess.p.lower = 7;
      goddess.p.upper = 7;
      goddess.p.founded = 1;
      break;
    }
  }

  if (goddess.p.founded == 0) {
    // visualize the logic I made
    /*
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
    for (int i = 0; i < 10 - goddess.p.lower; i++) {
      if (tab[i] == 1)
        count++;
      else
        break;
    }

    goddess.p.lower = 10 - count;
    goddess.p.upper = 10 - count;
    goddess.p.founded = 1;
  }
  fprintf(stderr, "trouvé p : %d\n", goddess.p.upper);
  fflush(stderr);
}


int main() {
  init_global_variables();

  /* get first game informations */
  char line[MAX_SIZE_WORD];

  // get game infos
  scanf(" %[^\n]s\n", line);
  sscanf(line, "%d %d", &game.NJ, &game.me_nb_j);
  game.players = malloc(game.NJ * sizeof(struct player));
  fprintf(stderr, "Récup game infos : %s\n", line);
  fflush(stderr);

  get_all_words_and_concepts();

  sort_concept_by_score_asc();

  copy_sorted_list_in_algos(&algo1);
  copy_sorted_list_in_algos(&algo2);
  copy_sorted_list_in_algos(&algo3);
  // print_concepts_words_list(&algo2.candidate_word_list);

  // print_concepts_words_list(&initial_sorted_word_list); // print the list for debug

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
        print_decision();
        get_turn_infos();
      }
      game.state.turn++;
    }

    if (*round == 0 && goddess.secret_words_index[0] != -1) {
      find_p();
      algo2_find_abc();
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
