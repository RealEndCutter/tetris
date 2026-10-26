#include "tetris.h"

#include <time.h>

#include "tetris_backend.h"
#include "tetris_fsm.h"

static GameInfo_t game_info;
static GameState_t game_state = START_STATE;
static activeFigure_t active_figure;
static figureType next_figure;
static field_t settled_field;
static long long last_tick_ms = 0;
static bool is_initialized = false;  // singletone guard

static void syncField();

static long long currentTime(void) {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return (long long)ts.tv_sec * 1000 +
         ts.tv_nsec / 1000000;  // nanoseconds to ms
}

int initialize() {
  if (!is_initialized) {
    srand(time(NULL));

    // --- field for setteld figures
    settled_field = createField(FIELD_WIDTH, FIELD_HEIGHT);

    // --- active figure
    active_figure.rotation = 0;
    active_figure.type = 0;
    active_figure.x = FIELD_WIDTH / 2;
    active_figure.y = 0;

    // --- next figure
    setNextFigure(&active_figure, &next_figure);

    // --- Game info init
    game_info.field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    game_info.level = 0;
    game_info.high_score = loadBestScore(BEST_SCORE_FILENAME);
    game_info.next = createField(FIGURE_MATRIX_SIZE, FIGURE_MATRIX_SIZE);
    game_info.score = 0;
    game_info.speed = 0;

    // --- validation
    if (!settled_field || !game_info.field || !game_info.next) return FAIL;

    // --- start data set
    figureToMatrix(next_figure, 0, game_info.next);
    fillField(settled_field, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);
    fillField(game_info.field, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);

    // initialization complete
    is_initialized = true;
  }

  return SUCCESS;
}

void final() {
  if (is_initialized) {
    free(settled_field);
    free(game_info.field);
    free(game_info.next);
  }

  saveBestScore(BEST_SCORE_FILENAME, game_info.high_score);
}

GameInfo_t updateCurrentState() {
  static const int LEVEL_SPEED_MS[MAX_LEVEL + 1] = {
      1000, 900, 800, 700, 600, 500, 400, 300, 250, 200, 150};

  GameContext_t ctx;
  ctx.state = &game_state;
  ctx.active_figure = &active_figure;
  ctx.game_info = &game_info;
  ctx.next_figure = &next_figure;
  ctx.settled_field = settled_field;

  long long now = currentTime();

  if (!game_info.pause) {
    if (now - last_tick_ms >= LEVEL_SPEED_MS[game_info.level]) {
      fsmUpdate(&ctx, true);
      last_tick_ms = now;
    } else
      fsmUpdate(&ctx, false);
  }

  syncField();
  figureToMatrix(next_figure, 0, game_info.next);

  return game_info;
}

void userInput(UserAction_t action, bool hold) {
  GameContext_t ctx;

  ctx.state = &game_state;
  ctx.active_figure = &active_figure;
  ctx.game_info = &game_info;
  ctx.next_figure = &next_figure;
  ctx.settled_field = settled_field;

  fsmUpdateControl(&ctx, action, hold);
}

GameState_t getTetrisState() { return game_state; }

static void syncField() {
  copyField((field_t)game_info.field, settled_field, FIELD_WIDTH, FIELD_HEIGHT);
  if (game_state == MOVE_STATE || game_state == SHIFT_STATE)
    placeFigureOnField((field_t)game_info.field, FIELD_WIDTH, FIELD_HEIGHT,
                       &active_figure);
}