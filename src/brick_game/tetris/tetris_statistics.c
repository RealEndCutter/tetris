#include <stdio.h>

#include "tetris_backend.h"

int updateStatistics(int *score, int *best_score, int lines_count) {
  if (lines_count == 1)
    *score += ONE_LINE_SCORE;
  else if (lines_count == 2)
    *score += TWO_LINE_SCORE;
  else if (lines_count == 3)
    *score += THREE_LINE_SCORE;
  else if (lines_count == 4)
    *score += FOUR_LINE_SCORE;
  else if (lines_count > 4)
    *score += FOUR_LINE_SCORE;

  if (*score > *best_score) *best_score = *score;

  int level = *score / LEVEL_UP_SCORE;

  if (level > MAX_LEVEL) level = MAX_LEVEL;

  return level;
}

int resetStatistics(int *score) {
  *score = 0;
  return 0;
}

void saveBestScore(const char *filename, int best_score) {
  FILE *f = fopen(filename, "wb");
  if (f == NULL) return;

  fwrite(&best_score, sizeof(int), 1, f);
  fclose(f);
}

int loadBestScore(const char *filename) {
  FILE *f = fopen(filename, "rb");

  if (f == NULL) return 0;

  int best_score = 0;
  if (fread(&best_score, sizeof(int), 1, f) != 1) best_score = 0;

  fclose(f);
  return best_score;
}