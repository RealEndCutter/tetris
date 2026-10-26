#ifndef TETRIS_FCM_H
#define TETRIS_FCM_H

#include "tetris.h"
#include "tetris_backend.h"

typedef struct {
  GameState_t *state;
  activeFigure_t *active_figure;
  figureType *next_figure;
  field_t settled_field;
  GameInfo_t *game_info;
} GameContext_t;

void fsmUpdateControl(GameContext_t *ctx, UserAction_t action, bool hold);
void fsmUpdate(GameContext_t *ctx, bool tick);

#endif  // TETRIS_FCM_H