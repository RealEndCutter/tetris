#ifndef TETRIS_BACKEND_H
#define TETRIS_BACKEND_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "defines.h"

#define FIGURE_ROTATIONS_COUNT 4
#define FIGURE_MATRIX_SIZE 4

// --- Types ---
typedef enum {
  O_FIGURE,
  I_FIGURE,
  T_FIGURE,
  L_FIGURE,
  J_FIGURE,
  S_FIGURE,
  Z_FIGURE,
  FIGURE_COUNT
} figureType;

typedef struct {
  uint16_t rotation[FIGURE_ROTATIONS_COUNT];
} figure_t;

typedef struct {
  int x;
  int y;
  int rotation;
  figureType type;
} activeFigure_t;

typedef int **field_t;

// --- Constants ---
extern const figure_t figures[FIGURE_COUNT];

// --- Field backend ---
field_t createField(int w, int h);
void fillField(field_t field, int w, int h, int value);
void copyField(field_t dest, field_t src, int w, int h);
int updateSettledField(field_t field, int w, int h);

// --- Figure backend ---
bool tryMove(activeFigure_t *figure, int dx, int dy, field_t field, int w,
             int h);
bool tryRotate(activeFigure_t *figure, field_t field, int w, int h);
void moveFigure(activeFigure_t *figure, int dx, int dy);
void figureToMatrix(figureType type, int rotPose, int **matrix);
void setNextFigure(activeFigure_t *figure, figureType *next);
bool checkFigureCollision(field_t field, int w, int h, figureType type,
                          int rotation, int xPos, int yPos);
void placeFigureOnField(field_t field, int w, int h, activeFigure_t *figure);

// --- Statistics ---
int updateStatistics(int *score, int *best_score, int lines_count);
int resetStatistics(int *score);
int loadBestScore(const char *filename);
void saveBestScore(const char *filename, int best_score);

#endif  // TETRIS_BACKEND