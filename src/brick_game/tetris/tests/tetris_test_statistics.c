#include <stdio.h>

#include "tetris_test_backend.h"

START_TEST(test_update_statistics_line_scores) {
  int score = 0, best = 0;

  updateStatistics(&score, &best, 0);
  ck_assert_int_eq(score, 0);

  updateStatistics(&score, &best, 1);
  ck_assert_int_eq(score, ONE_LINE_SCORE);

  updateStatistics(&score, &best, 2);
  ck_assert_int_eq(score, ONE_LINE_SCORE + TWO_LINE_SCORE);

  updateStatistics(&score, &best, 3);
  ck_assert_int_eq(score, ONE_LINE_SCORE + TWO_LINE_SCORE + THREE_LINE_SCORE);

  updateStatistics(&score, &best, 4);
  ck_assert_int_eq(score, ONE_LINE_SCORE + TWO_LINE_SCORE + THREE_LINE_SCORE +
                              FOUR_LINE_SCORE);
}
END_TEST

START_TEST(test_update_statistics_more_than_four_lines_scores_like_four) {
  int score = 0, best = 0;

  updateStatistics(&score, &best, 5);
  ck_assert_int_eq(score, FOUR_LINE_SCORE);
}
END_TEST

START_TEST(test_update_statistics_best_score_never_decreases) {
  int score = 0, best = 500;

  updateStatistics(&score, &best, 1);  // score = 100, меньше текущего рекорда
  ck_assert_int_eq(best, 500);  // рекорд не должен уменьшиться

  score = 1000;
  updateStatistics(&score, &best, 1);  // score = 1100, больше рекорда
  ck_assert_int_eq(best, 1100);
}
END_TEST

START_TEST(test_update_statistics_level_progression) {
  int score = 0, best = 0;

  int level = updateStatistics(&score, &best, 0);
  ck_assert_int_eq(level, 0);

  score = LEVEL_UP_SCORE;
  level = updateStatistics(&score, &best, 0);
  ck_assert_int_eq(level, 1);

  score = LEVEL_UP_SCORE * MAX_LEVEL + 1000;  // далеко за верхней границей
  level = updateStatistics(&score, &best, 0);
  ck_assert_int_eq(level, MAX_LEVEL);
}
END_TEST

START_TEST(test_reset_statistics) {
  int score = 12345;

  int level = resetStatistics(&score);

  ck_assert_int_eq(score, 0);
  ck_assert_int_eq(level, 0);
}
END_TEST

static const char *TEST_SCORE_FILE = "test_highscore.dat";

START_TEST(test_save_and_load_best_score_roundtrip) {
  saveBestScore(TEST_SCORE_FILE, 4242);
  ck_assert_int_eq(loadBestScore(TEST_SCORE_FILE), 4242);

  remove(TEST_SCORE_FILE);
}
END_TEST

START_TEST(test_load_best_score_missing_file_returns_zero) {
  remove(TEST_SCORE_FILE);  // на случай, если файл остался от прошлого прогона

  ck_assert_int_eq(loadBestScore(TEST_SCORE_FILE), 0);
}
END_TEST

Suite *statistics_suite(void) {
  Suite *s = suite_create("statistics");
  TCase *tc_scoring = tcase_create("scoring");
  TCase *tc_persistence = tcase_create("persistence");

  tcase_add_test(tc_scoring, test_update_statistics_line_scores);
  tcase_add_test(tc_scoring,
                 test_update_statistics_more_than_four_lines_scores_like_four);
  tcase_add_test(tc_scoring, test_update_statistics_best_score_never_decreases);
  tcase_add_test(tc_scoring, test_update_statistics_level_progression);
  tcase_add_test(tc_scoring, test_reset_statistics);

  tcase_add_test(tc_persistence, test_save_and_load_best_score_roundtrip);
  tcase_add_test(tc_persistence,
                 test_load_best_score_missing_file_returns_zero);

  suite_add_tcase(s, tc_scoring);
  suite_add_tcase(s, tc_persistence);

  return s;
}