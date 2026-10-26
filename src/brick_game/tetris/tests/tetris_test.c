#include "tetris_test.h"

#include "tetris_test_backend.h"

int main(void) {
  int failed = 0;

  Suite *suites[] = {field_suite(), figure_suite(), statistics_suite(),
                     fsm_suite(), tetris_suite()};

  for (size_t i = 0; i < sizeof(suites) / sizeof(*suites); ++i) {
    SRunner *sr = srunner_create(suites[i]);
    srunner_run_all(sr, CK_NORMAL);
    failed += srunner_ntests_failed(sr);
    srunner_free(sr);
  }

  return failed == 0 ? 0 : 1;
}