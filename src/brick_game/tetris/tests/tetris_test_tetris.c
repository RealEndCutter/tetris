#include <stdio.h>

#include "tetris_test_backend.h"

static bool score_file_existed;
static int saved_high_score;

static void setup(void) {
  FILE *f = fopen(BEST_SCORE_FILENAME, "rb");
  score_file_existed = (f != NULL);
  if (f) fclose(f);

  saved_high_score = loadBestScore(BEST_SCORE_FILENAME);
}

static void teardown(void) {
  if (score_file_existed)
    saveBestScore(BEST_SCORE_FILENAME, saved_high_score);
  else
    remove(BEST_SCORE_FILENAME);
}

static int countOccupiedCells(int **field) {
  int count = 0;
  for (int i = 0; i < FIELD_HEIGHT; ++i)
    for (int j = 0; j < FIELD_WIDTH; ++j)
      if (field[i][j] != CELL_OFF) ++count;
  return count;
}

static int sumOccupiedColumns(int **field) {
  int sum = 0;
  for (int i = 0; i < FIELD_HEIGHT; ++i)
    for (int j = 0; j < FIELD_WIDTH; ++j)
      if (field[i][j] != CELL_OFF) sum += j;
  return sum;
}

START_TEST(test_tetris_initial_state_is_empty) {
  ck_assert_int_eq(initialize(), SUCCESS);
  ck_assert_int_eq(getTetrisState(), START_STATE);

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(countOccupiedCells(info.field), 0);

  final();
}
END_TEST

START_TEST(test_tetris_initialize_is_idempotent) {
  ck_assert_int_eq(initialize(), SUCCESS);
  ck_assert_int_eq(initialize(), SUCCESS);

  final();
}
END_TEST

START_TEST(test_tetris_start_spawns_figure) {
  initialize();

  userInput(Start, false);
  GameInfo_t info = updateCurrentState();

  ck_assert_int_eq(getTetrisState(), MOVE_STATE);
  ck_assert(countOccupiedCells(info.field) > 0);

  final();
}
END_TEST

START_TEST(test_tetris_move_right_shifts_figure) {
  initialize();
  userInput(Start, false);

  GameInfo_t before = updateCurrentState();
  int cells = countOccupiedCells(before.field);
  int sum_before = sumOccupiedColumns(before.field);

  userInput(Right, false);
  GameInfo_t after = updateCurrentState();

  ck_assert_int_eq(sumOccupiedColumns(after.field), sum_before + cells);

  final();
}
END_TEST

START_TEST(test_tetris_pause_toggle_reflected_in_game_info) {
  initialize();
  userInput(Start, false);

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.pause, 0);

  userInput(Pause, false);
  info = updateCurrentState();
  ck_assert_int_eq(info.pause, 1);

  userInput(Pause, false);
  info = updateCurrentState();
  ck_assert_int_eq(info.pause, 0);

  final();
}
END_TEST

START_TEST(test_tetris_loads_high_score_from_file_on_initialize) {
  saveBestScore(BEST_SCORE_FILENAME, 777);

  initialize();
  GameInfo_t info = updateCurrentState();

  ck_assert_int_eq(info.high_score, 777);

  final();
}
END_TEST

Suite *tetris_suite(void) {
  Suite *s = suite_create("tetris");
  TCase *tc = tcase_create("core");

  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, test_tetris_initial_state_is_empty);
  tcase_add_test(tc, test_tetris_initialize_is_idempotent);
  tcase_add_test(tc, test_tetris_start_spawns_figure);
  tcase_add_test(tc, test_tetris_move_right_shifts_figure);
  tcase_add_test(tc, test_tetris_pause_toggle_reflected_in_game_info);
  tcase_add_test(tc, test_tetris_loads_high_score_from_file_on_initialize);

  suite_add_tcase(s, tc);
  return s;
}
