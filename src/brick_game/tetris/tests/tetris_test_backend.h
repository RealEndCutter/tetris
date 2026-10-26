#ifndef TETRIS_TEST_BACKEND_H
#define TETRIS_TEST_BACKEND_H

#include <check.h>

#include "tetris.h"
#include "tetris_backend.h"
#include "tetris_fsm.h"

Suite *field_suite(void);
Suite *figure_suite(void);
Suite *statistics_suite(void);
Suite *fsm_suite(void);
Suite *tetris_suite(void);

#endif  // TETRIS_TEST_BACKEND_H