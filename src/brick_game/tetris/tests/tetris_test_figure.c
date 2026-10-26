#include "tetris_test_backend.h"

START_TEST(test_figure_to_matrix) {
  int **m = (int **)createField(FIGURE_MATRIX_SIZE, FIGURE_MATRIX_SIZE);
  ck_assert_ptr_nonnull(m);

  figureToMatrix(O_FIGURE, 0, m);

  int expected[4][4] = {
      {O_FIGURE + 1, O_FIGURE + 1, 0, 0},
      {O_FIGURE + 1, O_FIGURE + 1, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
  };

  for (int i = 0; i < FIGURE_MATRIX_SIZE; ++i)
    for (int j = 0; j < FIGURE_MATRIX_SIZE; ++j)
      ck_assert_int_eq(m[i][j], expected[i][j]);

  free(m);
}
END_TEST

START_TEST(test_next_figure) {
  activeFigure_t figure;
  figureType next = O_FIGURE;

  setNextFigure(&figure, &next);

  char seen[FIGURE_COUNT] = {0};
  for (int i = 0; i < FIGURE_COUNT; ++i) {
    setNextFigure(&figure, &next);
    seen[figure.type] = 1;
  }

  for (int t = 0; t < FIGURE_COUNT; ++t)
    ck_assert_msg(seen[t], "figure %d never droped from bag", t);
}
END_TEST

START_TEST(test_place_figure_on_field) {
  field_t f = createField(FIELD_WIDTH, FIELD_HEIGHT);
  ck_assert_ptr_nonnull(f);
  fillField(f, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);

  activeFigure_t figure;
  figure.rotation = 0;
  figure.type = L_FIGURE;
  figure.x = 0;
  figure.y = 0;

  placeFigureOnField(f, FIELD_WIDTH, FIELD_HEIGHT, &figure);

  int expected[FIGURE_MATRIX_SIZE][FIGURE_MATRIX_SIZE] = {
      {0, 0, L_FIGURE + 1, 0},
      {L_FIGURE + 1, L_FIGURE + 1, L_FIGURE + 1, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
  };

  for (int i = 0; i < FIGURE_MATRIX_SIZE; ++i)
    for (int j = 0; j < FIGURE_MATRIX_SIZE; ++j)
      ck_assert_int_eq(f[i][j], expected[i][j]);

  free(f);
}
END_TEST

START_TEST(test_figure_move) {
  field_t f = createField(FIELD_WIDTH, FIELD_HEIGHT);
  ck_assert_ptr_nonnull(f);
  fillField(f, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);

  activeFigure_t figure;
  figure.rotation = 0;
  figure.type = L_FIGURE;
  figure.x = 0;
  figure.y = 0;

  ck_assert_int_eq(tryMove(&figure, 1, 0, f, FIELD_WIDTH, FIELD_HEIGHT), 1);
  figure.x = 0;
  ck_assert_int_eq(tryMove(&figure, -1, 0, f, FIELD_WIDTH, FIELD_HEIGHT), 0);

  figure.y = FIELD_HEIGHT - 2;
  ck_assert_int_eq(tryMove(&figure, 0, 1, f, FIELD_WIDTH, FIELD_HEIGHT), 0);

  free(f);
}
END_TEST

START_TEST(test_figure_rotate_succeeds_in_open_space) {
  field_t f = createField(FIELD_WIDTH, FIELD_HEIGHT);
  ck_assert_ptr_nonnull(f);
  fillField(f, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);

  activeFigure_t figure;
  figure.rotation = 0;
  figure.type = T_FIGURE;
  figure.x = 4;
  figure.y = 4;

  ck_assert_int_eq(tryRotate(&figure, f, FIELD_WIDTH, FIELD_HEIGHT), 1);
  ck_assert_int_eq(figure.rotation, 1);

  free(f);
}
END_TEST

START_TEST(test_figure_rotate_fails_at_bottom_wall) {
  field_t f = createField(FIELD_WIDTH, FIELD_HEIGHT);
  ck_assert_ptr_nonnull(f);
  fillField(f, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);

  activeFigure_t figure;
  figure.rotation = 0;
  figure.type = I_FIGURE;
  figure.x = 3;
  figure.y = FIELD_HEIGHT - 2;

  ck_assert_int_eq(tryRotate(&figure, f, FIELD_WIDTH, FIELD_HEIGHT), 0);
  ck_assert_int_eq(figure.rotation, 0);

  free(f);
}
END_TEST

START_TEST(test_move_blocked_by_settled_block) {
  field_t f = createField(FIELD_WIDTH, FIELD_HEIGHT);
  ck_assert_ptr_nonnull(f);
  fillField(f, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);
  f[5][5] = CELL_ON;

  activeFigure_t figure;
  figure.rotation = 0;
  figure.type = O_FIGURE;
  figure.x = 3;
  figure.y = 3;

  ck_assert_int_eq(tryMove(&figure, 0, 1, f, FIELD_WIDTH, FIELD_HEIGHT), 1);
  ck_assert_int_eq(tryMove(&figure, 1, 0, f, FIELD_WIDTH, FIELD_HEIGHT), 0);

  free(f);
}
END_TEST

Suite *figure_suite(void) {
  Suite *s = suite_create("figure");
  TCase *tc_core = tcase_create("core");
  TCase *tc_tf = tcase_create("transform");

  tcase_add_test(tc_core, test_figure_to_matrix);
  tcase_add_test(tc_core, test_next_figure);
  tcase_add_test(tc_core, test_place_figure_on_field);

  tcase_add_test(tc_tf, test_figure_move);
  tcase_add_test(tc_tf, test_figure_rotate_succeeds_in_open_space);
  tcase_add_test(tc_tf, test_figure_rotate_fails_at_bottom_wall);
  tcase_add_test(tc_tf, test_move_blocked_by_settled_block);

  suite_add_tcase(s, tc_core);
  suite_add_tcase(s, tc_tf);

  return s;
}