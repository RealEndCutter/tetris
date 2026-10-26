#include "tetris_backend.h"

field_t createField(int w, int h) {
  field_t field = NULL;
  int **data_ptr = (int **)malloc(w * h * sizeof(int) + h * sizeof(int *));

  if (data_ptr) {
    field = (field_t)data_ptr;
    int *filed_ptr = (int *)(data_ptr + h);
    for (int i = 0; i < h; ++i) field[i] = filed_ptr + i * w;
  }

  return field;
}

void fillField(field_t field, int w, int h, int value) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) field[i][j] = value;
  }
}

void copyField(field_t dest, field_t src, int w, int h) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      dest[i][j] = src[i][j];
    }
  }
}

int updateSettledField(field_t field, int w, int h) {
  int line_count = 0;
  int write_row = h - 1;

  for (int read_row = h - 1; read_row >= 0; --read_row) {
    bool complete = true;
    for (int j = 0; j < w && complete; ++j) {
      if (field[read_row][j] == CELL_OFF) {
        complete = false;
      }
    }

    if (complete) {
      ++line_count;
      continue;
    }

    if (write_row != read_row) {
      for (int j = 0; j < w; ++j) {
        field[write_row][j] = field[read_row][j];
      }
    }
    --write_row;
  }

  for (int row = write_row; row >= 0; --row) {
    for (int j = 0; j < w; ++j) {
      field[row][j] = CELL_OFF;
    }
  }

  return line_count;
}