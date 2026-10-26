#include <locale.h>
#include <stdio.h>
#include <tetris.h>

#include "draw.h"

static UserAction_t getActionFromKey(int key);
static void draw(const GameInfo_t *info, GameState_t state);
static void gameLoop(void);

int main(void) {
  printf("--- Brick game v1.0 (Tetris executable) ---");
  setlocale(LC_ALL, "");

  initscr();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);
  timeout(10);
  draw_init_colors();

  if (initialize()) {
    gameLoop();
    timeout(-1);
    endwin();
    final();
  }

  return 0;
}

UserAction_t getActionFromKey(int key) {
  switch (key) {
    case KEY_UP:
      return Up;
    case KEY_DOWN:
      return Down;
    case KEY_LEFT:
      return Left;
    case KEY_RIGHT:
      return Right;
    case ' ':
      return Action;
    case '\n':
    case '\r':
    case KEY_ENTER:
      return Start;
    case 'p':
      return Pause;
    case 27:
      return Terminate;
  }

  return Action;
}

void draw(const GameInfo_t *info, GameState_t state) {
  erase();

  draw_overlay();
  draw_stats(info->level, info->score, info->high_score);
  draw_next(info->next);
  draw_field(info->field, FIELD_WIDTH, FIELD_HEIGHT);

  if (state == START_STATE)
    draw_start_box();
  else if (info->pause == true)
    draw_pause_box();
  else if (state == END_STATE)
    draw_end_box(info->score);

  refresh();
}

void gameLoop() {
  GameInfo_t info;
  while (true) {
    int ch = getch();

    if (ch != ERR) {
      UserAction_t action = getActionFromKey(ch);
      if (action == Terminate) break;
      userInput(action, false);
    }

    info = updateCurrentState();

    draw(&info, getTetrisState());
  }
}