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

static void drawAABB(AABB_Object a) {
  // TODO?: scale to window size.

  // Convert physical meters -> pixels (WIP: and flip Y so physical y=0 is at the bottom).
  if (a.isCircle) {
    DrawCircle((a.x + a.width) * SCALE, (a.y + a.width) * SCALE, a.width * SCALE, a.col);
  } else {
    DrawRectangle(a.x * SCALE, a.y * SCALE, a.width * SCALE, a.height * SCALE, a.col);
  }
}

static void drawSAT(SAT_Object a) {
  // Draw lines only, no fill.
  Vector2 v1;
  Vector2 v2;

  for (size_t i = 0; i < a.vertices_count - 1; i++) {
    fflush(stdout);
    // add position to vertex as offset, then scale by SCALE
    v1 = Vector2Scale(Vector2Add(a.vertices[i], a.position), SCALE);
    v2 = Vector2Scale(Vector2Add(a.vertices[i + 1], a.position), SCALE);

    DrawLineV(v1, v2, a.col);
  }

  DrawLineV(Vector2Scale(Vector2Add(a.vertices[a.vertices_count - 1], a.position), SCALE),
            Vector2Scale(Vector2Add(a.vertices[0], a.position), SCALE), a.col);
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

  char framerateDisplay[11];
  char frameAvgDisplay[10];
  char frameCounterDisplay[20];

  AABB_Object *simpleAABBObjects = (AABB_Object *)calloc(MAXOBJECTS, sizeof(AABB_Object));
  size_t AABBSize = 2;

  // for (size_t i = 0; i < AABBSize; i++) {
  //   simpleAABBObjects[i] = (AABB_Object){rando(1, 5), rando(1, 5), 1, 1, rando(-1, 1), rando(-1, 1), 1, 0,};
  // }
  simpleAABBObjects[0] = (AABB_Object){1, 1, 1, 1, 2, 0, 1, WHITE, 0};
  simpleAABBObjects[1] = (AABB_Object){4, 1, 1, 1, -2, 0, 10, RED, 0};

  // SAT_Object *SATObjects = (SAT_Object *)calloc(MAXOBJECTS, sizeof(SAT_Object));
  // size_t SATsize = 2;
  // Vector2 *SATobj1 = (Vector2[]){{1.0606602, -1.0606602}, {4.2426407f, 0.0}, {2.1213203, 2.1213203}};
  // Vector2 *SATobj2 = (Vector2[]){{0, 0}, {2, 0}, {2, 2}, {0, 2}};
  // SATObjects[0] = (SAT_Object){SATobj1, 3, (Vector2){5, 5}, (Vector2){-5, 10}, WHITE};
  // SATObjects[1] = (SAT_Object){SATobj2, 4, (Vector2){2, 3}, (Vector2){-5, 0}, RED};

  // game loop
  bool paused = true;
  bool onetickonly = true;

  while (!WindowShouldClose()) {
    // Get user input.
    int key = GetKeyPressed();

    if (key != 0 && key == KEY_P) {
      printf("pausing\n");
      fflush(stdout);
      paused = !paused;
    }

    if (key != 0 && key == KEY_T) {
      printf("ticking\n");
      fflush(stdout);
      onetickonly = true;
    }

    // Update the time since the last frame/tick.
    dt = GetFrameTime();
    trueFramerate = 1 / dt;

    if (onetickonly) {
      AABB_simulate(simpleAABBObjects, AABBSize, dt);
      // SAT_simulate(SATObjects, SATsize, dt);
      onetickonly = false;
    }

    // Simulate.
    if (!paused && !onetickonly) {
      AABB_simulate(simpleAABBObjects, AABBSize, dt);
      // SAT_simulate(SATObjects, SATsize, dt);
    }

    // Draw.
    BeginDrawing();
    ClearBackground((Color){20, 20, 20, 255});

    for (size_t i = 0; i < AABBSize; i++) {
      drawAABB(simpleAABBObjects[i]);
    }
    // for (size_t i = 0; i < SATsize; i++) {
    //   drawSAT(SATObjects[i]);
    // }

    // Calculate and draw the FPS count to the screen.
    sprintf(framerateDisplay, "FPS: %.2f", trueFramerate);
    framerateDisplay[10] = '\0';

    sprintf(frameCounterDisplay, "%d", frameCounter);
    frameCounterDisplay[sizeof(frameCounterDisplay) / sizeof(frameCounterDisplay[0]) - 1] = '\0';

    sprintf(frameAvgDisplay, "%f", framerateAverage);
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
  // free(SATObjects);
  CloseWindow();
  return 0;
}
