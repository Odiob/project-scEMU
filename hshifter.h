#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Shifter {
  uint8_t offset;
  uint8_t x;
  uint8_t y;
} Shifter;

void Shift(Shifter *state, Ports *ports);
