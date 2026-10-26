/**
 * @mainpage BrickGame Tetris
 *
 * This covers the public interface of the two modules meant to be reused
 * on their own:
 *
 * - tetris.h — the game logic library: initialize()/final() to set up and
 *   tear down a game, userInput() to feed it player actions, and
 *   updateCurrentState() to advance it and read back the current field for
 *   rendering.
 * - draw.h — the ncurses CLI frontend built on top of that library: draws
 *   the field, side panel and start/pause/game over overlays.
 *
 * Internal modules (the finite state machine, figure/field backend,
 * scoring) are not part of this public documentation.
 */

#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <stdlib.h>

#include "defines.h"

/**
 * @brief User-initiated inputs the library accepts through userInput().
 *
 * Matches the eight physical buttons of the BrickGame console. In this
 * game specifically: Up is unused, Action rotates the active figure, and
 * Down performs a hard drop (moves the figure all the way down).
 */
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

/**
 * @brief High-level phase of the game, as reported by getTetrisState().
 *
 * SPAWN_STATE, MOVE_STATE, SHIFT_STATE and UNION_STATE are internal steps
 * of a single automatic update and are only ever observed momentarily.
 */
typedef enum {
  START_STATE,
  SPAWN_STATE,
  MOVE_STATE,
  SHIFT_STATE,
  UNION_STATE,
  END_STATE
} GameState_t;

/**
 * @brief Snapshot of the current game state, returned by updateCurrentState().
 *
 * @c field and @c next are owned by the library: they point at internal
 * buffers that stay valid until final() is called and must not be freed
 * by the caller. A cell value of 0 means empty; any other value identifies
 * which figure type occupies that cell.
 */
typedef struct {
  int **field; /**< FIELD_WIDTH x FIELD_HEIGHT playing field matrix. */
  int **next;  /**< FIGURE_MATRIX_SIZE x FIGURE_MATRIX_SIZE preview of the next
                  figure. */
  int score;   /**< Current score of the running game. */
  int high_score; /**< Best score ever reached, persisted across runs. */
  int level;      /**< Current level, 0..MAX_LEVEL; controls fall speed. */
  int speed; /**< Reserved for frontend use; not populated by the library. */
  int pause; /**< Non-zero while the game is paused. */
} GameInfo_t;

/**
 * @brief Allocates and resets all game state. Idempotent: a second call
 * while already initialized is a no-op. Loads the saved high score from
 * disk. Must be called once before any other library function.
 * @return SUCCESS on success, FAIL if allocation failed.
 */
int initialize();

/**
 * @brief Frees resources allocated by initialize() and persists the
 * current high score to disk. Safe to call once at shutdown.
 */
void final(void);

/**
 * @brief Feeds one user action into the game.
 * @param action the action to apply.
 * @param hold whether the underlying key is being held down (as opposed
 * to a fresh press); the CLI frontend currently always passes false.
 */
void userInput(UserAction_t action, bool hold);

/**
 * @brief Advances automatic game logic (gravity, etc.) and returns the
 * current state for rendering. Must be called periodically (e.g. once
 * per frame) to keep the game and the frontend in sync.
 * @return a snapshot of the current game state.
 */
GameInfo_t updateCurrentState();

/**
 * @brief Returns the current high-level game phase.
 * @return the current GameState_t; use this instead of introspecting
 * GameInfo_t to decide which overlay (start/pause/game over) to draw.
 */
GameState_t getTetrisState();

#endif  // TETRIS_H