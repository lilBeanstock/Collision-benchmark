#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <AABB.h>
#include <SAT.h>
#include <common.h>

#define FRAMERATE 90.00
#define MAXOBJECTS 1000000
#define FRAMES_PER_AVERAGE 30

static void drawAABB(AABB_Object a, Color col) {
  // TODO?: scale to window size.

  // Convert physical meters -> pixels and flip Y so physical y=0 is at the bottom.
  DrawRectangle(a.x * SCALE, a.y * SCALE, a.width * SCALE, a.height * SCALE, col);
}

// Remove in production?
static float rando(int min, int max) {
  int generated = (float)(rand() % (max * 10 - min * 10 + 1) + min * 10);

  return (float)(generated) / 10.0F;
}

int main() {
  srand(time(NULL));

  InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "Collision Algorithm Benchmark");
  SetTargetFPS(FRAMERATE);

  float dt = 0;
  float trueFramerate = 0;
  float framerateAverage = 0;
  double frTot = 0;
  int frameCounter = 0;

  char framerateDisplay[10];
  char frameAvgDisplay[10];
  char frameCounterDisplay[20];

  AABB_Object *simpleAABBObjects = (AABB_Object *)calloc(MAXOBJECTS, sizeof(AABB_Object));
  size_t AABBSize = 600;

  for (size_t i = 0; i < AABBSize; i++) {
    simpleAABBObjects[i] = (AABB_Object){rando(1, 5), rando(1, 5), 1, 1, rando(0, 1), rando(0, 1)};
  }

  SAT_object SATObjects[2] = {};
  Vector2 *SATobj1 = (Vector2[]){{1.5, 0}, {3.0, 3.0}, {0, 3.0}};
  Vector2 *SATobj2 = (Vector2[]){{0, 0}, {2, 0}, {2, 2}, {0, 2}};
  SATObjects[0] = (SAT_object){SATobj1, 3, 0, 0, 0, 0};
  SATObjects[1] = (SAT_object){SATobj2, 4, 0, 0, 0, 0};

  bool paused = false;

  while (!WindowShouldClose()) {
    int key = GetKeyPressed();

    if (key != 0 && key == KEY_P) {
      printf("pausing\n");
      fflush(stdout);
      paused = !paused;
    }

    // Update the time since the last frame/tick.
    dt = GetFrameTime();
    trueFramerate = 1 / dt;

    // Get user input.

    // Simulate.
    if (!paused) {
      AABB_simulate(simpleAABBObjects, AABBSize, dt);
    }

    // Draw.
    BeginDrawing();
    ClearBackground((Color){20, 20, 20, 255});

    for (size_t i = 0; i < AABBSize; i++) {
      drawAABB(simpleAABBObjects[i], WHITE);
    }

    // Calculate and draw the FPS count to the screen.
    sprintf(framerateDisplay, "%f", (trueFramerate));
    framerateDisplay[6] = '\0';

    sprintf(frameCounterDisplay, "%d", (frameCounter));
    frameCounterDisplay[sizeof(frameCounterDisplay) / sizeof(frameCounterDisplay[0]) - 1] = '\0';

    sprintf(frameAvgDisplay, "%f", (framerateAverage));
    frameAvgDisplay[sizeof(frameAvgDisplay) / sizeof(frameAvgDisplay[0]) - 1] = '\0';

    frTot += trueFramerate / (float)FRAMES_PER_AVERAGE;
    frameCounter++;

    if (frameCounter % FRAMES_PER_AVERAGE == 0) {
      framerateAverage = frTot;
      frTot = 0;
    }

    DrawText(framerateDisplay, 5, 5, 20, WHITE);
    DrawText(frameAvgDisplay, 5, 30, 20, WHITE);
    DrawText(frameCounterDisplay, 120, 5, 20, WHITE);
    EndDrawing();
  }

  // Free the allocated memory by the stress-test objects.
  free(simpleAABBObjects);
  CloseWindow();
  return 0;
}
