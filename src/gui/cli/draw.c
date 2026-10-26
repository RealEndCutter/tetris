#include "draw.h"

#include <string.h>

#include "layouts.h"

static const int GAME_FIELD_X = IDENT_X + STATS_RECT_W + SPACING_A + 1;
static const int GAME_FIELD_Y = IDENT_Y;
static const int NEXT_X = GAME_FIELD_X + GAME_FIELD_RECT_H + SPACING_B + 2;
static const int NEXT_Y = IDENT_Y;

// Helpers
static void draw_rectangle(int x, int y, int w, int h);
static void draw_cell(int x, int y, int color_id);
static void draw_box();

static const short FIGURE_COLORS[] = {
    COLOR_YELLOW,   // O
    COLOR_CYAN,     // I
    COLOR_MAGENTA,  // T
    COLOR_WHITE,    // L
    COLOR_BLUE,     // J
    COLOR_GREEN,    // S
    COLOR_RED,      // Z
};

#define BOX_COLOR_PAIR 8

void draw_init_colors(void) {
  if (!has_colors()) return;

  start_color();
  use_default_colors();

  int count = (int)(sizeof(FIGURE_COLORS) / sizeof(FIGURE_COLORS[0]));
  for (int i = 0; i < count; ++i)
    init_pair((short)(i + 1), COLOR_BLACK, FIGURE_COLORS[i]);

  init_pair(BOX_COLOR_PAIR, COLOR_BLACK, COLOR_WHITE);
}

void draw_overlay(void) {
  const int stats_x = IDENT_X;
  const int stats_y = IDENT_Y;

  const int help_x = NEXT_X;
  const int help_y = NEXT_Y + NEXT_RECT_H + SPACING_C + 1;

  draw_rectangle(stats_x, stats_y, STATS_RECT_W, STATS_RECT_H);
  mvprintw(stats_y, stats_x + STATS_RECT_W / 2 - 2, "Stats");
  mvprintw(stats_y + 2, stats_x + 2, "Score");
  mvprintw(stats_y + 4, stats_x + 2, "Level");
  mvprintw(stats_y + 6, stats_x + 2, "Best");
  mvprintw(stats_y + 7, stats_x + 2, "Score");

  draw_rectangle(GAME_FIELD_X, GAME_FIELD_Y, GAME_FIELD_RECT_W,
                 GAME_FIELD_RECT_H);

  draw_rectangle(NEXT_X, NEXT_Y, NEXT_RECT_W, NEXT_RECT_H);
  mvprintw(NEXT_Y, NEXT_X + NEXT_RECT_W / 2 - 1, "Next");

  draw_rectangle(help_x, help_y, HELP_RECT_W, HELP_RECT_H);
  mvprintw(help_y, help_x + HELP_RECT_W / 2 - 1, "Help");

  const char *const HELP_CONTROL[] = {
      "Left", "Right", "Drop", "Rotate", "Quit",
  };

  const char *const HELP_BUTTONS[] = {"←", "→", "↓", "space", "esc"};

  for (int i = 0; i < 5; ++i) {
    mvprintw(help_y + 2 + i, help_x + 2, "%s", HELP_CONTROL[i]);
    if (i > 2)
      mvprintw(help_y + 2 + i, help_x + 11, "%5s", HELP_BUTTONS[i]);
    else
      mvprintw(help_y + 2 + i, help_x + 15, "%s", HELP_BUTTONS[i]);
  }
}

void draw_stats(int level, int score, int best_score) {
  const int stats_data_x = IDENT_X + STATS_RECT_W - 8;
  const int stats_data_y = IDENT_Y + 2;

  mvprintw(stats_data_y, stats_data_x, "%7d", score);
  mvprintw(stats_data_y + 2, stats_data_x, "%7d", level);
  mvprintw(stats_data_y + 5, stats_data_x, "%7d", best_score);
}

void draw_next(int **next) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (next[i][j]) draw_cell(NEXT_X + 5 + j * 2, NEXT_Y + i + 2, next[i][j]);
    }
  }
}

void draw_field(int **field, int field_w, int field_h) {
  for (int i = 0; i < field_h; ++i) {
    for (int j = 0; j < field_w; ++j) {
      if (field[i][j])
        draw_cell(GAME_FIELD_X + 1 + j * 2, GAME_FIELD_Y + i + 1, field[i][j]);
    }
  }
}

void draw_rectangle(int x, int y, int w, int h) {
  // Corners
  mvaddch(y, x, ACS_ULCORNER);
  mvaddch(y + h, x, ACS_LLCORNER);
  mvaddch(y, x + w, ACS_URCORNER);
  mvaddch(y + h, x + w, ACS_LRCORNER);

  // Vertica lines
  for (int j = y + 1; j < y + h; ++j) {
    mvaddch(j, x, ACS_VLINE);
    mvaddch(j, x + w, ACS_VLINE);
  }

  // Horizontal lines
  for (int i = x + 1; i < x + w; ++i) {
    mvaddch(y, i, ACS_HLINE);
    mvaddch(y + h, i, ACS_HLINE);
  }
}

void draw_cell(int x, int y, int color_id) {
  attr_t attr = COLOR_PAIR(color_id);
  mvaddch(y, x, ' ' | attr);
  mvaddch(y, x + 1, ' ' | attr);
}

void draw_start_box(void) {
  const char *lines[] = {"Press  'Enter'", "to Start"};
  draw_box(lines, 2);
}

void draw_pause_box(void) {
  const char *lines[] = {"Pause"};
  draw_box(lines, 1);
}

void draw_end_box(int score) {
  char score_line[32];
  snprintf(score_line, sizeof(score_line), "%d", score);

  const char *lines[] = {"Game Over!", "Your score:", score_line,
                         "Press  'Enter'", "to restart"};
  draw_box(lines, 5);
}

static void draw_box(const char *const *lines, int line_count) {
  int box_w = 0;

  for (int i = 0; i < line_count; ++i) {
    int len = (int)strlen(lines[i]);
    if (len > box_w) box_w = len;
  }

  box_w += 4;
  int box_h = line_count + 2;

  int box_x = GAME_FIELD_X + 1 + (GAME_FIELD_RECT_W - 1 - box_w) / 2;
  int box_y = GAME_FIELD_Y + (GAME_FIELD_RECT_H - box_h) / 2;

  attron(COLOR_PAIR(BOX_COLOR_PAIR));

  for (int j = 0; j < box_h; ++j)
    for (int i = 0; i < box_w; ++i) mvaddch(box_y + j, box_x + i, ' ');

  for (int i = 0; i < line_count; ++i) {
    int len = (int)strlen(lines[i]);
    mvprintw(box_y + 1 + i, box_x + (box_w - len) / 2, "%s", lines[i]);
  }

  attroff(COLOR_PAIR(BOX_COLOR_PAIR));
}
