#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <time.h>

#include <AABB.h> // Align-Axis Bounding Box
#include <SAT.h> // Serparating Axis Thereom

#define FRAMERATE 90.00
#define VIRTUAL_WIDTH 1024.00
#define VIRTUAL_HEIGHT 800.00
#define SCALE (VIRTUAL_HEIGHT/10.00) // the height is the denominator in meters, HEIGHT/10 -> 10 meters at the height of the application
#define GRAVITY 9.82
#define MAXOBJECTS 1000000
#define FRAMESPERAVERAGE 30

void drawAABB(AABB_Object a, Color col) {
	// TODO?: scale to window size.
	
	// convert physics meters -> pixels and flip Y so physics y=0 is bottom
    DrawRectangle(
			a.x * SCALE,
			a.y * SCALE,
			a.width * SCALE,
			a.height * SCALE,
			col
	);
}

// remove in production?
float r(int min, int max) {
    int shit = (float)(rand() % (max*10 - min*10 + 1) + min*10);
    return (float)(shit)/10.0F;
}

int main() {
    srand(time(NULL));
    
    InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "raylib [core] example - input keys");
    SetTargetFPS(FRAMERATE);
    float DT = 0;
    float trueFramerate = 0;
    float framerateAverage = 0;
    double frTot = 0;
    int frameCounter = 0;

    char framerateDisplay[10];
    char frameAvgDisplay[10];
    char frameCounterDisplay[20];

    // global/runtime variables
    AABB_Object* simpleObjects = (AABB_Object*)calloc(MAXOBJECTS,sizeof(AABB_Object));
    size_t objects = 600;

    // simpleObjects[0] = (AABB_Object){
	// 		// x, y.
	// 		0, 1,
	// 		// width, height.
	// 		1, 1,
	// 		// dx, dy.
	// 		2, 0,
	// 		// mass.
	// 		10
	// 	};
	// 	simpleObjects[1] = (AABB_Object){
	// 		5, 2,
	// 		3, 2,
	// 		4, -1,
	// 		5
	// 	};

    for (int i = 0; i<600;i++) {
        simpleObjects[i] = (AABB_Object){
            r(1,5),r(1,5),
            1,1,
            r(0,1),r(0,1),
            10
        };
    }

    // program loop
    bool paused = false;
    while (!WindowShouldClose()) {
        int key = GetKeyPressed();
        if (key != 0 && key == KEY_P) {
            printf("pausing\n");
            fflush(stdout);
            paused = !paused;
        }


        DT = GetFrameTime();
        trueFramerate = 1/DT;

        sprintf(framerateDisplay, "%f", (trueFramerate));
        framerateDisplay[6] = '\0';
        sprintf(frameCounterDisplay, "%d", (frameCounter));
        frameCounterDisplay[sizeof(frameCounterDisplay)/sizeof(frameCounterDisplay[0]) - 1] = '\0';
        sprintf(frameAvgDisplay, "%f", (framerateAverage));
        frameAvgDisplay[sizeof(frameAvgDisplay)/sizeof(frameAvgDisplay[0]) - 1] = '\0';

        frTot += trueFramerate/(float)FRAMESPERAVERAGE;
        frameCounter++;
        if (frameCounter % FRAMESPERAVERAGE == 0) {
            framerateAverage = frTot;
            frTot = 0;
        }

        
        // ---------- GET USER INPUT ----------
        

        // ---------- SIMULATE ----------
        if (!paused) {
            simulate(
                simpleObjects,
                objects,
                DT,
                VIRTUAL_WIDTH / SCALE,
                VIRTUAL_HEIGHT / SCALE,
                GRAVITY
            );
        }

        // ---------- DRAW ----------
        BeginDrawing();
            ClearBackground((Color){20,20,20,255});

            for (size_t i = 0; i < objects; i++) {
                drawAABB(simpleObjects[i], WHITE);
            }

            DrawText(framerateDisplay, 5, 5, 20, WHITE);
            DrawText(frameAvgDisplay, 5, 30, 20, WHITE);
            DrawText(frameCounterDisplay, 120, 5, 20, WHITE);
        EndDrawing();
    }

    free(simpleObjects);
    CloseWindow();
    return 0;
}
