#include "tetris_test_backend.h"

static field_t field;

static void setup(void) {
  field = createField(FIELD_WIDTH, FIELD_HEIGHT);
  if (field != NULL) fillField(field, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);
}

static void teardowwn(void) { free(field); }

START_TEST(test_create_field_not_null) { ck_assert_ptr_nonnull(field); }
END_TEST

START_TEST(test_fill_field_sets_value) {
  fillField(field, FIELD_WIDTH, FIELD_HEIGHT, CELL_ON);
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      ck_assert_int_eq(field[i][j], CELL_ON);
    }
  }
}
END_TEST

START_TEST(test_copy_field_matches_source) {
  field_t source = createField(FIELD_WIDTH, FIELD_HEIGHT);
  ck_assert_ptr_nonnull(source);

  fillField(source, FIELD_WIDTH, FIELD_HEIGHT, 5);

  copyField(field, source, FIELD_WIDTH, FIELD_HEIGHT);
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      ck_assert_int_eq(field[i][j], source[i][j]);
    }
  }
}
END_TEST

START_TEST(test_update_settled_field) {
  for (int j = 0; j < FIELD_WIDTH; ++j) field[FIELD_HEIGHT - 1][j] = CELL_ON;

  int cleared = updateSettledField(field, FIELD_WIDTH, FIELD_HEIGHT);

  ck_assert_int_eq(cleared, 1);
  ck_assert_int_eq(field[FIELD_HEIGHT - 1][0], CELL_OFF);
}
END_TEST

Suite *field_suite(void) {
  Suite *s = suite_create("field");
  TCase *tc = tcase_create("core");

  tcase_add_checked_fixture(tc, setup, teardowwn);
  tcase_add_test(tc, test_create_field_not_null);
  tcase_add_test(tc, test_fill_field_sets_value);
  tcase_add_test(tc, test_copy_field_matches_source);
  tcase_add_test(tc, test_update_settled_field);

  suite_add_tcase(s, tc);

  return s;
}