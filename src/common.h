// Define our publicly accessible constants and structs.
#define VIRTUAL_WIDTH 1024.00
#define VIRTUAL_HEIGHT 800.00
// The denominator is the height in meters.
// HEIGHT / 10 -> 10 meters at the height of the application.
#define SCALE (VIRTUAL_HEIGHT / 10.00)
#define WIDTH (VIRTUAL_WIDTH / SCALE)
#define HEIGHT (VIRTUAL_HEIGHT / SCALE)
#define GRAVITY 9.82

#pragma once

typedef struct {
  double time;
  double fps;
} JSONDataPoint;

typedef struct {
  int object_count;
  JSONDataPoint *points;
} JSONData;
