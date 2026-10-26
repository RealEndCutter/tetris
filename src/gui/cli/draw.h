#ifndef DRAW_H
#define DRAW_H

#include <ncurses.h>

/**
 * @brief Sets up the ncurses color pairs used by the rest of this module.
 * Must be called once, after ncurses has been initialized (initscr()) and
 * before any other draw_* function.
 */
void draw_init_colors(void);

/**
 * @brief Draws the static chrome: panel borders and labels. Call once per
 * frame, before the other draw_* functions.
 */
void draw_overlay(void);

/**
 * @brief Draws score/level/best score into the stats panel.
 */
void draw_stats(int level, int score, int best_score);

/**
 * @brief Draws the next-figure preview.
 * @param next a FIGURE_MATRIX_SIZE x FIGURE_MATRIX_SIZE matrix as returned
 * by GameInfo_t::next; a non-zero cell is rendered as filled.
 */
void draw_next(int **next);

/**
 * @brief Draws the playing field.
 * @param field a field_w x field_h matrix as returned by GameInfo_t::field;
 * a non-zero cell is rendered as filled.
 * @param field_w width of @p field, in cells.
 * @param field_h height of @p field, in cells.
 */
void draw_field(int **field, int field_w, int field_h);

/**
 * @brief Draws the "press start" overlay centered on the field. Call after
 * draw_field() so it renders on top.
 */
void draw_start_box(void);

/**
 * @brief Draws the "paused" overlay centered on the field. Call after
 * draw_field() so it renders on top.
 */
void draw_pause_box(void);

/**
 * @brief Draws the "game over" overlay centered on the field, including
 * the final score. Call after draw_field() so it renders on top.
 */
void draw_end_box(int score);

#endif  // DRAW_H