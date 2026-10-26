#include "tetris_backend.h"

const figure_t figures[FIGURE_COUNT] = {
    [O_FIGURE] = {.rotation = {0xCC00, 0xCC00, 0xCC00, 0xCC00}},
    [I_FIGURE] = {.rotation = {0x0F00, 0x2222, 0x0F00, 0x2222}},
    [T_FIGURE] = {.rotation = {0xE400, 0x2620, 0x04E0, 0x8C80}},
    [L_FIGURE] = {.rotation = {0x2E00, 0x4460, 0x0E80, 0xC440}},
    [J_FIGURE] = {.rotation = {0xE200, 0x2260, 0x08E0, 0xC880}},
    [S_FIGURE] = {.rotation = {0x6C00, 0x4620, 0x6C00, 0x4620}},
    [Z_FIGURE] = {.rotation = {0xC600, 0x2640, 0xC600, 0x2640}},
};

static bool checkFigureCell(figureType type, int rotPose, int i, int j) {
  int shift = FIGURE_MATRIX_SIZE * FIGURE_MATRIX_SIZE - 1 -
              (i * FIGURE_MATRIX_SIZE + j);
  return (figures[type].rotation[rotPose] >> shift) & 1;
}

static void figureColumnBounds(figureType type, int rotation, int *min_col,
                               int *max_col) {
  *min_col = FIGURE_MATRIX_SIZE;
  *max_col = -1;

  for (int j = 0; j < FIGURE_MATRIX_SIZE; ++j) {
    for (int i = 0; i < FIGURE_MATRIX_SIZE; ++i) {
      if (checkFigureCell(type, rotation, i, j)) {
        if (j < *min_col) *min_col = j;
        if (j > *max_col) *max_col = j;
      }
    }
  }
}

static void refillBag(figureType *bag_ptr, int bag_size, int *bag_pose) {
  for (int i = 0; i < bag_size; ++i) bag_ptr[i] = (figureType)i;

  for (int i = bag_size - 1; i > 0; --i) {
    int j = rand() % (i + 1);
    figureType tmp = bag_ptr[i];
    bag_ptr[i] = bag_ptr[j];
    bag_ptr[j] = tmp;
  }

  *bag_pose = 0;
}

bool tryMove(activeFigure_t *figure, int dx, int dy, field_t field, int w,
             int h) {
  int new_x = figure->x + dx;
  int new_y = figure->y + dy;

  if (checkFigureCollision(field, w, h, figure->type, figure->rotation, new_x,
                           new_y))
    return false;

  figure->x = new_x;
  figure->y = new_y;
  return true;
}

bool tryRotate(activeFigure_t *figure, field_t field, int w, int h) {
  int next_rot = (figure->rotation + 1) % FIGURE_ROTATIONS_COUNT;  // Rotate CCW

  if (checkFigureCollision(field, w, h, figure->type, next_rot, figure->x,
                           figure->y))
    return false;

  figure->rotation = next_rot;
  return true;
}

void figureToMatrix(figureType type, int rotPose, int **matrix) {
  if (rotPose >= FIGURE_ROTATIONS_COUNT) {
    rotPose = 0;
  }

  for (int i = 0; i < FIGURE_MATRIX_SIZE; ++i) {
    for (int j = 0; j < FIGURE_MATRIX_SIZE; ++j) {
      matrix[i][j] =
          checkFigureCell(type, rotPose, i, j) ? (int)type + 1 : CELL_OFF;
    }
  }
}

void setNextFigure(activeFigure_t *figure, figureType *next) {
  static figureType bag[FIGURE_COUNT];
  static int bag_pos = FIGURE_COUNT;

  if (bag_pos >= FIGURE_COUNT) refillBag(bag, FIGURE_COUNT, &bag_pos);

  figure->type = *next;
  figure->rotation = 0;
  figure->y = 0;

  int min_col, max_col;
  figureColumnBounds(figure->type, figure->rotation, &min_col, &max_col);
  int width = max_col - min_col + 1;
  figure->x = (FIELD_WIDTH - width) / 2 - min_col;

  *next = bag[bag_pos++];
}

bool checkFigureCollision(field_t field, int w, int h, figureType type,
                          int rotation, int xPos, int yPos) {
  bool is_collide = false;

  for (int i = 0; i < FIGURE_MATRIX_SIZE && !is_collide; ++i) {
    int y = i + yPos;
    for (int j = 0; j < FIGURE_MATRIX_SIZE; ++j) {
      int x = j + xPos;

      if (checkFigureCell(type, rotation, i, j)) {
        if (x >= w || x < 0 || y >= h) {
          is_collide = true;
          break;
        } else if (y >= 0 && field[y][x] != CELL_OFF) {
          is_collide = true;
          break;
        }
      }
    }
  }

  return is_collide;
}

void placeFigureOnField(field_t field, int w, int h, activeFigure_t *figure) {
  for (int i = 0; i < FIGURE_MATRIX_SIZE; ++i) {
    int y = i + figure->y;
    for (int j = 0; j < FIGURE_MATRIX_SIZE; ++j) {
      int x = j + figure->x;

      if (checkFigureCell(figure->type, figure->rotation, i, j) && x >= 0 &&
          x < w && y >= 0 && y < h)
        field[y][x] = (int)figure->type + 1;
    }
  }
}