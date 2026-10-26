#include "tetris_fsm.h"

static void spawnStateHandler(GameContext_t *ctx) {
  setNextFigure(ctx->active_figure, ctx->next_figure);
  bool is_over = checkFigureCollision(
      ctx->settled_field, FIELD_WIDTH, FIELD_HEIGHT, ctx->active_figure->type,
      ctx->active_figure->rotation, ctx->active_figure->x,
      ctx->active_figure->y);

  *ctx->state = is_over ? END_STATE : MOVE_STATE;
}

static void moveStateHandler(GameContext_t *ctx) { *ctx->state = SHIFT_STATE; }

static void shiftStateHandler(GameContext_t *ctx) {
  *ctx->state = tryMove(ctx->active_figure, 0, 1, ctx->settled_field,
                        FIELD_WIDTH, FIELD_HEIGHT)
                    ? MOVE_STATE
                    : UNION_STATE;
}

static void unionStateHandler(GameContext_t *ctx) {
  placeFigureOnField(ctx->settled_field, FIELD_WIDTH, FIELD_HEIGHT,
                     ctx->active_figure);

  int count = updateSettledField(ctx->settled_field, FIELD_WIDTH, FIELD_HEIGHT);

  ctx->game_info->level = updateStatistics(&ctx->game_info->score,
                                           &ctx->game_info->high_score, count);

  *ctx->state = SPAWN_STATE;
}

static void endStateHandler(GameContext_t *ctx) {
  fillField(ctx->settled_field, FIELD_WIDTH, FIELD_HEIGHT, CELL_OFF);
  ctx->game_info->level = resetStatistics(&ctx->game_info->score);
}

void fsmUpdate(GameContext_t *ctx, bool tick) {
  switch (*ctx->state) {
    case START_STATE:
      break;

    case SPAWN_STATE:
      spawnStateHandler(ctx);
      fsmUpdate(ctx, tick);
      break;

    case MOVE_STATE:
      if (tick) {
        moveStateHandler(ctx);
        fsmUpdate(ctx, tick);
      }
      break;

    case SHIFT_STATE:
      shiftStateHandler(ctx);
      fsmUpdate(ctx, false);
      break;

    case UNION_STATE:
      unionStateHandler(ctx);
      fsmUpdate(ctx, tick);
      break;

    case END_STATE:
      break;
  }
}

void fsmUpdateControl(GameContext_t *ctx, UserAction_t action, bool hold) {
  if (*ctx->state == START_STATE) {
    if (action == Start) *ctx->state = SPAWN_STATE;
  } else if (*ctx->state == END_STATE) {
    if (action == Start) {
      endStateHandler(ctx);
      *ctx->state = SPAWN_STATE;
    }
  } else if (*ctx->state == MOVE_STATE) {
    if (!ctx->game_info->pause) {
      if (action == Left)
        tryMove(ctx->active_figure, -1, 0, ctx->settled_field, FIELD_WIDTH,
                FIELD_HEIGHT);
      if (action == Right)
        tryMove(ctx->active_figure, 1, 0, ctx->settled_field, FIELD_WIDTH,
                FIELD_HEIGHT);
      if (action == Action && !hold)
        tryRotate(ctx->active_figure, ctx->settled_field, FIELD_WIDTH,
                  FIELD_HEIGHT);
      if (action == Down && !hold) {
        do {
          shiftStateHandler(ctx);
        } while (*ctx->state == MOVE_STATE);
      }
    }
    if (action == Pause && !hold)
      ctx->game_info->pause = !ctx->game_info->pause;
  }
}