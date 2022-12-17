// import libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* constants */
#define NB_WORD 1000
#define NB_CONCEPT 50
#define NB_ROUND 5
#define NB_TURN 20

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
};


struct algo_data {
  struct list_word candidate_word_list;
  char* last_word_proposed;
};


/* Global variables */
struct list_word initial_sorted_word_list;
struct list_concept list_concepts_of_round;
struct algo_data algo1; // only p strat
struct algo_data algo2; // only with (a, b, c) params
struct algo_data algo3; // with both p & (a, b, c)
struct goddess_param goddess;
struct game_config game;

/* All functions */
void init_global_variables() {
  initial_sorted_word_list.list = malloc(NB_WORD*sizeof(struct word));
  initial_sorted_word_list.size = NB_WORD;

  list_concepts_of_round.size = 0;
  for (int i = 0; i < NB_TURN; i++) {
    list_concepts_of_round.list[i] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
  }

  algo1.candidate_word_list.list = malloc(NB_WORD*sizeof(struct word));
  algo1.candidate_word_list.size = NB_WORD;
  algo1.last_word_proposed = malloc(MAX_SIZE_WORD * sizeof(char));

  algo2.candidate_word_list.list = malloc(NB_WORD*sizeof(struct word));
  algo2.candidate_word_list.size = NB_WORD;
  algo2.last_word_proposed = malloc(MAX_SIZE_WORD * sizeof(char));

  algo3.candidate_word_list.list = malloc(NB_WORD*sizeof(struct word));
  algo3.candidate_word_list.size = NB_WORD;
  algo3.last_word_proposed = malloc(MAX_SIZE_WORD * sizeof(char));

  for (int i = 0; i < NB_WORD; i++) {
    initial_sorted_word_list.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    algo1.candidate_word_list.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    algo2.candidate_word_list.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));
    algo3.candidate_word_list.list[i].v = malloc(MAX_SIZE_WORD * sizeof(char));

    for (int j = 0; j < NB_CONCEPT; j++) {
      initial_sorted_word_list.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
      algo1.candidate_word_list.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
      algo2.candidate_word_list.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
      algo3.candidate_word_list.list[i].concepts[j] = malloc(MAX_SIZE_CONCEPT * sizeof(char));
    }
  }

  goddess.p.upper = 7;
  goddess.p.lower = 3;
  goddess.p.founded = 0;

  game.state.round = 0;
  game.state.turn = 0;
  game.state.is_all_founded = 0;
  game.state.is_me_founded = 0;
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
  char* line = malloc(MAX_SIZE_WORD * sizeof(char));

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

void print_concepts_words_list() {
  for (int i = 0; i < NB_WORD; i++) {
    for (int j = 0; j < NB_CONCEPT; j++) {
      fprintf(stderr, "mot : %s, con : %s et s : %d\n", initial_sorted_word_list.list[i].v, initial_sorted_word_list.list[i].concepts[j], initial_sorted_word_list.list[i].scores[j]);
      fflush(stderr);
    }
    if (i == 49) {
      fprintf(stderr, "\n");
      fflush(stderr);
    }
  }
}

void sort_concept_by_score_asc() {
  char* tmp_s = malloc(MAX_SIZE_CONCEPT * sizeof(char));
  for (int i = 0; i < NB_WORD; i++) {
    for (int j = NB_CONCEPT-1; j > 0; j--) {
      for (int k = 0; k < j; k++) {
        if (initial_sorted_word_list.list[i].scores[k + 1] < initial_sorted_word_list.list[i].scores[k]) {
          int tmp_i = initial_sorted_word_list.list[i].scores[k + 1];
          initial_sorted_word_list.list[i].scores[k + 1] = initial_sorted_word_list.list[i].scores[k];
          initial_sorted_word_list.list[i].scores[k] = tmp_i;

          // char* tmp_s = malloc(255 * sizeof(char));

          tmp_s = initial_sorted_word_list.list[i].concepts[k + 1];
          initial_sorted_word_list.list[i].concepts[k + 1] = initial_sorted_word_list.list[i].concepts[k];
          initial_sorted_word_list.list[i].concepts[k] = tmp_s;

          //tmp_s = initial_sorted_word_list.list[i].concepts[k + 1];
          //initial_sorted_word_list.list[i].concepts[k + 1] = initial_sorted_word_list.list[i].concepts[k];
          //initial_sorted_word_list.list[i].concepts[k] = tmp_s;
        } else if (initial_sorted_word_list.list[i].scores[k + 1] == initial_sorted_word_list.list[i].scores[k]) {
          for (int p = 0; p < (int) strlen(initial_sorted_word_list.list[i].concepts[k]); p++) {
            if (initial_sorted_word_list.list[i].concepts[k][p] > initial_sorted_word_list.list[i].concepts[k+1][p]) {
              int tmp_i = initial_sorted_word_list.list[i].scores[k + 1];
              initial_sorted_word_list.list[i].scores[k + 1] = initial_sorted_word_list.list[i].scores[k];
              initial_sorted_word_list.list[i].scores[k] = tmp_i;

              // char* tmp_s = malloc(255 * sizeof(char));
              tmp_s = initial_sorted_word_list.list[i].concepts[k + 1];
              initial_sorted_word_list.list[i].concepts[k + 1] = initial_sorted_word_list.list[i].concepts[k];
              initial_sorted_word_list.list[i].concepts[k] = tmp_s;

              //tmp_s = initial_sorted_word_list.list[i].concepts[k + 1];
              //initial_sorted_word_list.list[i].concepts[k + 1] = initial_sorted_word_list.list[i].concepts[k];
              //initial_sorted_word_list.list[i].concepts[k] = tmp_s;
              break;
            } else if (initial_sorted_word_list.list[i].concepts[k][p] < initial_sorted_word_list.list[i].concepts[k+1][p]) {
              break;
            } else { // same char
              continue;
            }
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
}

void reset_game_state() {
  game.state.is_all_founded = 0;
  game.state.is_me_founded = 0;

  // reset the list of concept
  /*
  for (int i = 0; i < list_concepts_of_round.size; i++) {
    memset(list_concepts_of_round.list[i], 0, MAX_SIZE_CONCEPT);
  }*/
  list_concepts_of_round.size = 0;

  // copy_sorted_list_in_algos(&algo1);
  for (int i = 0; i < NB_WORD; i++) {
    algo1.candidate_word_list.list[i].v = initial_sorted_word_list.list[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      algo1.candidate_word_list.list[i].concepts[j] = initial_sorted_word_list.list[i].concepts[j];
      algo1.candidate_word_list.list[i].scores[j] = initial_sorted_word_list.list[i].scores[j];
    }
  }

  algo1.candidate_word_list.size = NB_WORD;
}

// Récupération informations joueurs restants et ce qu'ils ont proposés
/* Get all players informations : can be (J P END) or (W J ST)
 *
 */
void get_game_infos() {
  for (int nb_player = 0; nb_player < game.NJ; nb_player++) {
    char* line = malloc(MAX_SIZE_WORD * sizeof(char));
    scanf(" %[^\n]s\n", line);
    fprintf(stderr, "Récupération ligne %s :\n", line);
    fflush(stderr);

    if (game.state.turn == 0) { // (J P END)

      int J, P, END;

      sscanf(line, "%d %d %d", &J, &P, &END);
      game.players[J].score = P;
      game.players[J].ST = END;

      fprintf(stderr, "Tour 0 ,, J: %d , P: %d , END: %d\n", J, P, END);
      fflush(stderr);
    } else {
      char* first_string = malloc(MAX_SIZE_WORD * sizeof(char)); // W or J ?
      int sec_info; // P or J ?
      int third_info; // END or ST ?
      sscanf(line, "%s %d %d", first_string, &sec_info, &third_info);

      if (third_info == 2) {
        if (game.state.is_me_founded == 0) {
          game.state.is_me_founded = 1;
        }
        game.players[sec_info].ST = third_info;

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

    free(line);
  }
}

void remove_word_from_candidate_list_algos_by_index(struct algo_data* algo, int index) {
  int last_element_index = (algo->candidate_word_list.size)-1;
  for (int i = index; i < last_element_index; i++) {
    algo->candidate_word_list.list[i].v = algo->candidate_word_list.list[i+1].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      algo->candidate_word_list.list[i].concepts[j] = algo->candidate_word_list.list[i+1].concepts[j];
      algo->candidate_word_list.list[i].scores[j] = algo->candidate_word_list.list[i+1].scores[j];
    }
  }

  algo->candidate_word_list.size = last_element_index;
}

void get_concept() {
  char* line = malloc(MAX_SIZE_CONCEPT*sizeof(char));
  scanf("%s", line);

  int new_size = list_concepts_of_round.size + 1;
  sscanf(line, "%s", list_concepts_of_round.list[new_size-1]);
  list_concepts_of_round.size = new_size;

  fprintf(stderr, "Récupération mot : %s, taille = %d \n", list_concepts_of_round.list[new_size-1], list_concepts_of_round.size);
  fflush(stderr);
  // free(line);
}

void ia_algo1() {

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

    if (partie_basse == 0 && partie_haute == 0) {
      // fprintf(stderr, "id %d, algo1 j'enlève : %s\n", i, algo1.candidate_word_list.list[i].v);
      // fflush(stderr);
      // remove_word_from_candidate_list_algos_by_index(&algo1, i);

      int last_element_index = (algo1.candidate_word_list.size)-1;
      for (int j = i; j < last_element_index; j++) {
        algo1.candidate_word_list.list[j].v = algo1.candidate_word_list.list[j+1].v;
        for (int k = 0; k < NB_CONCEPT; k++) {
          algo1.candidate_word_list.list[j].concepts[k] = algo1.candidate_word_list.list[j+1].concepts[k];
          algo1.candidate_word_list.list[j].scores[k] = algo1.candidate_word_list.list[j+1].scores[k];
        }
      }

      algo1.candidate_word_list.size = last_element_index;

      i--;
    }
  }
}

void all_algo() {
  ia_algo1();
}

/* All print methods */
void print_pass() {
  fprintf(stdout, "PASS\n");
  fflush(stdout);
}

void guess_word(char* word) {
  fprintf(stdout, "GUESS %s\n", word);
  fflush(stdout);
}

void print_decision() {
  if (game.state.round == 0) {
    if (list_concepts_of_round.size < NB_TURN) {
      fprintf(stderr, "1 \n");
      fflush(stderr);
      print_pass();
    } else {
      if (game.state.is_me_founded == 0) {
        if (algo1.candidate_word_list.size == 1) {
          fprintf(stderr, "2 \n");
          fflush(stderr);
          guess_word(algo1.candidate_word_list.list[0].v);
        } else {
          fprintf(stderr, "tentative de guess au pif \n");
          fflush(stderr);
        }
      }
    }
  } else {
    if (list_concepts_of_round.size <= NB_TURN) {
      if (game.state.is_me_founded == 1) {
        fprintf(stderr, "3 \n");
        fflush(stderr);
        print_pass();
      } else {
        if (algo1.candidate_word_list.size == 1) {
          fprintf(stderr, "4 \n");
          fflush(stderr);
          guess_word(algo1.candidate_word_list.list[0].v);
        } else {
          fprintf(stderr, "5 \n");
          fflush(stderr);
          print_pass();
        }
      }
    } else {
      if (game.state.is_me_founded == 0) {
        if (algo1.candidate_word_list.size == 1) {
          fprintf(stderr, "6 \n");
          fflush(stderr);
          guess_word(algo1.candidate_word_list.list[0].v);
        } else {
          fprintf(stderr, "7 \n");
          fflush(stderr);
          fprintf(stderr, "tentative de guess au pif \n");
          fflush(stderr);
        }
      }
    }
  }
}


int main() {
  init_global_variables();

  /* get first game informations */
  char* line = malloc(255 * sizeof(char));

  // get game infos
  scanf(" %[^\n]s\n", line);
  sscanf(line, "%d %d", &game.NJ, &game.me_nb_j);
  game.players = malloc(game.NJ * sizeof(struct player));
  fprintf(stderr, "Récup game infos : %s\n", line);
  fflush(stderr);

  get_all_words_and_concepts();

  sort_concept_by_score_asc();

  // copy_sorted_list_in_algos(&algo1);
  for (int i = 0; i < NB_WORD; i++) {
    algo1.candidate_word_list.list[i].v = initial_sorted_word_list.list[i].v;
    for (int j = 0; j < NB_CONCEPT; j++) {
      algo1.candidate_word_list.list[i].concepts[j] = initial_sorted_word_list.list[i].concepts[j];
      algo1.candidate_word_list.list[i].scores[j] = initial_sorted_word_list.list[i].scores[j];
    }
  }

  // print_concepts_words_list(); // print the list for debug

  while(1) {
    if (game.state.round == 5) {
      break;
    }
    get_game_infos();
    if (game.state.is_all_founded == 1) {
      reset_game_state();
      game.state.round += 1;
      game.state.turn = 0;
      fprintf(stderr, "reset game, nouveau round : %d \n", game.state.round);
      fflush(stderr);
    }

    if (game.state.turn < NB_TURN) {
      get_concept();
    }
    if (game.state.is_me_founded == 0) {
      all_algo();
    }
    print_decision();
    game.state.turn++;
  }

  fprintf(stderr, "END OF GAME\n");
  fflush(stderr);

  free(line);
  free_global_variables();

  return 0;
}
