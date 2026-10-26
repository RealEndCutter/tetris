#include "tetris_test_backend.h"

static GameState_t state;
static activeFigure_t active_figure;
static figureType next_figure;
static field_t settled_field;
static GameInfo_t game_info;
static GameContext_t ctx;

static void setup(void) {
  state = START_STATE;
  settled_field = createField(FIELD_WIDTH, FIELD_HEIGHT);
  fillField(settled_field, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);

  next_figure = O_FIGURE;
  active_figure = (activeFigure_t){0};
  game_info = (GameInfo_t){0};

  ctx.state = &state;
  ctx.active_figure = &active_figure;
  ctx.next_figure = &next_figure;
  ctx.settled_field = settled_field;
  ctx.game_info = &game_info;
}

static void teardown(void) { free(settled_field); }

START_TEST(test_fsm_start_waits_for_start_action) {
  fsmUpdate(&ctx, true);
  ck_assert_int_eq(state, START_STATE);

  fsmUpdateControl(&ctx, Left, false);
  ck_assert_int_eq(state, START_STATE);

  fsmUpdateControl(&ctx, Start, false);
  ck_assert_int_eq(state, SPAWN_STATE);
}
END_TEST

START_TEST(test_fsm_spawn_resolves_to_move_on_empty_field) {
  state = SPAWN_STATE;

  fsmUpdate(&ctx, false);

  ck_assert_int_eq(state, MOVE_STATE);
  ck_assert_int_eq(active_figure.y, 0);
}
END_TEST

START_TEST(test_fsm_gravity_moves_figure_down_on_tick) {
  state = MOVE_STATE;
  active_figure.type = O_FIGURE;
  active_figure.rotation = 0;
  active_figure.x = 4;
  active_figure.y = 0;

  fsmUpdate(&ctx, false);
  ck_assert_int_eq(active_figure.y, 0);
  ck_assert_int_eq(state, MOVE_STATE);

  fsmUpdate(&ctx, true);
  ck_assert_int_eq(active_figure.y, 1);
  ck_assert_int_eq(state, MOVE_STATE);
}
END_TEST

START_TEST(test_fsm_landing_locks_and_spawns_next_in_one_call) {
  state = MOVE_STATE;
  active_figure.type = O_FIGURE;
  active_figure.rotation = 0;
  active_figure.x = 4;

  active_figure.y = 18;

  fsmUpdate(&ctx, true);

  ck_assert_int_eq(settled_field[18][4], O_FIGURE + 1);
  ck_assert_int_eq(settled_field[19][5], O_FIGURE + 1);
  ck_assert_int_eq(state, MOVE_STATE);
  ck_assert_int_eq(active_figure.y, 0);
}
END_TEST

START_TEST(test_fsm_clears_completed_line_and_scores) {
  for (int j = 4; j < FIELD_WIDTH; ++j)
    settled_field[FIELD_HEIGHT - 1][j] = CELL_ON;

  state = MOVE_STATE;
  active_figure.type = I_FIGURE;
  active_figure.rotation = 0;
  active_figure.x = 0;
  active_figure.y = FIELD_HEIGHT - 2;

  fsmUpdate(&ctx, true);

  ck_assert_int_eq(game_info.score, ONE_LINE_SCORE);

  for (int j = 0; j < FIELD_WIDTH; ++j)
    ck_assert_int_eq(settled_field[FIELD_HEIGHT - 1][j], CELL_OFF);
}
END_TEST

START_TEST(test_fsm_game_over_on_blocked_spawn) {
  fillField(settled_field, FIELD_WIDTH, FIELD_HEIGHT, CELL_ON);

  state = SPAWN_STATE;
  fsmUpdate(&ctx, false);

  ck_assert_int_eq(state, END_STATE);
}
END_TEST

START_TEST(test_fsm_restart_from_end_state_resets_everything) {
  state = END_STATE;
  game_info.score = 999;
  settled_field[10][3] = CELL_ON;

  fsmUpdateControl(&ctx, Start, false);

  ck_assert_int_eq(state, SPAWN_STATE);
  ck_assert_int_eq(game_info.score, 0);
  ck_assert_int_eq(settled_field[10][3], CELL_OFF);
}
END_TEST

START_TEST(test_fsm_pause_blocks_movement_but_toggle_always_works) {
  state = MOVE_STATE;
  active_figure.type = O_FIGURE;
  active_figure.x = 4;
  active_figure.y = 5;
  game_info.pause = 1;

  fsmUpdateControl(&ctx, Left, false);
  ck_assert_int_eq(active_figure.x, 4);

  fsmUpdateControl(&ctx, Pause, false);
  ck_assert_int_eq(game_info.pause, 0);

  fsmUpdateControl(&ctx, Left, false);
  ck_assert_int_eq(active_figure.x, 3);
}
END_TEST

START_TEST(test_fsm_hold_gate_ignores_repeated_action_and_pause) {
  state = MOVE_STATE;
  active_figure.type = T_FIGURE;
  active_figure.rotation = 0;
  active_figure.x = 4;
  active_figure.y = 5;

  fsmUpdateControl(&ctx, Action, true);
  ck_assert_int_eq(active_figure.rotation, 0);

  fsmUpdateControl(&ctx, Pause, true);
  ck_assert_int_eq(game_info.pause, 0);

  fsmUpdateControl(&ctx, Action, false);
  ck_assert_int_eq(active_figure.rotation, 1);
}
END_TEST

START_TEST(test_fsm_hard_drop_falls_to_union_state) {
  state = MOVE_STATE;
  active_figure.type = O_FIGURE;
  active_figure.rotation = 0;
  active_figure.x = 4;
  active_figure.y = 0;

  fsmUpdateControl(&ctx, Down, false);

  ck_assert_int_eq(state, UNION_STATE);
  ck_assert_int_eq(settled_field[FIELD_HEIGHT - 1][4], CELL_OFF);

  fsmUpdate(&ctx, false);
  ck_assert_int_eq(settled_field[FIELD_HEIGHT - 1][4], O_FIGURE + 1);
  ck_assert_int_eq(state, MOVE_STATE);
}
END_TEST

Suite *fsm_suite(void) {
  Suite *s = suite_create("fsm");
  TCase *tc = tcase_create("core");

  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, test_fsm_start_waits_for_start_action);
  tcase_add_test(tc, test_fsm_spawn_resolves_to_move_on_empty_field);
  tcase_add_test(tc, test_fsm_gravity_moves_figure_down_on_tick);
  tcase_add_test(tc, test_fsm_landing_locks_and_spawns_next_in_one_call);
  tcase_add_test(tc, test_fsm_clears_completed_line_and_scores);
  tcase_add_test(tc, test_fsm_game_over_on_blocked_spawn);
  tcase_add_test(tc, test_fsm_restart_from_end_state_resets_everything);
  tcase_add_test(tc, test_fsm_pause_blocks_movement_but_toggle_always_works);
  tcase_add_test(tc, test_fsm_hold_gate_ignores_repeated_action_and_pause);
  tcase_add_test(tc, test_fsm_hard_drop_falls_to_union_state);

  suite_add_tcase(s, tc);
  return s;
}
