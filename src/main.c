#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utils.h>

#include <AABB.h>
#include <SAT.h>
#include <common.h>

#define FRAMERATE 90
#define MAXOBJECTS 1000000
#define FRAMES_PER_AVERAGE 30

char TEXTDEBUGTMP[256];

static void drawAABB(AABB_Object a) {
  // TODO?: scale to window size.

  // Convert physical meters -> pixels (WIP: and flip Y so physical y=0 is at the bottom).
  if (a.isCircle) {
    DrawCircle((a.x + a.width) * SCALE, (a.y + a.width) * SCALE, a.width * SCALE, a.col);
  } else {
    DrawRectangle(a.x * SCALE, a.y * SCALE, a.width * SCALE, a.height * SCALE, a.col);
    // sprintf(TEXTDEBUGTMP, "%.3f %.3f", a.x, a.y);
    // DrawText(TEXTDEBUGTMP, a.x * SCALE, (a.y + a.height) * SCALE + 16, 15, a.col);
    // sprintf(TEXTDEBUGTMP, "%.3f %.3f", a.dx, a.dy);
    // DrawText(TEXTDEBUGTMP, a.x * SCALE, (a.y + a.height) * SCALE + 36, 15, a.col);
    // sprintf(TEXTDEBUGTMP, "%.3f - %.2f %.2f", a.mass, a.width, a.height);
    // DrawText(TEXTDEBUGTMP, a.x * SCALE, (a.y + a.height) * SCALE + 56, 15, a.col);
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
  DrawCircle(SAT_center(a).x * SCALE, SAT_center(a).y * SCALE, 5, WHITE);
}

// Remove in production?
static float rando(int min, int max) {
  int generated = (float)(rand() % (max * 10 - min * 10 + 1) + min * 10);

  return (float)(generated) / 10.0F;
}

static void configureAABB(AABB_Object *AABBs[], size_t *realObjCount, size_t desiredObjCount) {
  // TODO: fix some rectangles not rendering bug.
  // Color col = (Color){0, 0, 0, 255};

  for (size_t i = 0; i < desiredObjCount; i++) {
    // col.r = 255;
    // col.g = 255;
    // col.b = 255;
    (*AABBs)[i] = (AABB_Object){rando(1, 5),  rando(1, 5), rando(0.5, 1), rando(0.5, 1), rando(-1, 2),
                                rando(-1, 2), rando(1, 5), VIOLET,        false};
  }

  *realObjCount = desiredObjCount; // overwrite, all other object data will be ignored
}

static void configureSAT(SAT_Object *SATs[], size_t *realObjCount, size_t desiredObjCount) {
  Color col = (Color){0, 0, 0, 255};

  for (size_t i = 0; i < desiredObjCount; i++) {
    col.r = (int)rando(100, 230);
    col.g = (int)rando(100, 230);
    col.b = (int)rando(100, 230);
    Vector2 *vertices = (Vector2 *)calloc(6, sizeof(Vector2));
    int verticesCount = (int)rando(2, 6);

    for (int i = 0; i < verticesCount; i++) {
      vertices[i] = (Vector2){rando(1, 5), rando(1, 5)};
    }

    (*SATs)[i] = (SAT_Object){vertices, verticesCount, (Vector2){rando(1, 5), rando(1, 5)},
                              (Vector2){rando(0.5, 1), rando(0.5, 1)}, col};
  }

  *realObjCount = desiredObjCount; // overwrite, all other object data will be ignored
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

  // AABB_Object *simpleAABBObjects = (AABB_Object *)calloc(MAXOBJECTS, sizeof(AABB_Object));
  // size_t AABBSize = 0;
  // size_t desiredAABBSize = 10;
  // configureAABB(&simpleAABBObjects, &AABBSize, desiredAABBSize);

  SAT_Object *SATObjects = (SAT_Object *)calloc(MAXOBJECTS, sizeof(SAT_Object));
  size_t SATsize = 0;
  size_t desiredSATSize = 3000;
  configureSAT(&SATObjects, &SATsize, desiredSATSize);

  // game loop
  bool paused = false;
  bool onetickonly = false;

  // JSONDataPoint JSONDataPoints[500];
  JSONDataPoint *JSONDataPoints = (JSONDataPoint *)calloc(MAXOBJECTS, sizeof(JSONDataPoint));
  clock_t startTime;

  while (!WindowShouldClose()) {
    if (frameCounter == 1) {
      startTime = clock();
    }

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
      // AABB_simulate(simpleAABBObjects, AABBSize, dt);
      SAT_simulate(SATObjects, SATsize, dt);
      onetickonly = false;
    }

    // Simulate.
    if (!paused && !onetickonly) {
      // AABB_simulate(simpleAABBObjects, AABBSize, dt);
      SAT_simulate(SATObjects, SATsize, dt);
    }

    // Draw.
    BeginDrawing();
    ClearBackground((Color){20, 20, 20, 255});

    // for (size_t i = 0; i < AABBSize; i++) {
    //   drawAABB(simpleAABBObjects[i]);
    // }
    for (size_t i = 0; i < SATsize; i++) {
      drawSAT(SATObjects[i]);
    }

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

    if (frameCounter > 1 && frameCounter < 502) {
      JSONDataPoints[frameCounter - 2].time = clock() - startTime;
      JSONDataPoints[frameCounter - 2].fps = trueFramerate;
    }

    if (frameCounter == 502) {
      JSONData data = (JSONData){desiredSATSize, JSONDataPoints};
      char *json = dataToJSON(data, frameCounter - 2);
      FILE *dataFile = fopen("./data/SAT_run_4.json", "w");
      fprintf(dataFile, json);

      fclose(dataFile);
      free(json);
    }
  }

  // Free the allocated memory by the stress-test objects.
  // free(simpleAABBObjects);
  free(SATObjects);
  CloseWindow();
  return 0;
}
