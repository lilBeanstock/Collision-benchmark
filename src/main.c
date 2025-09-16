#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#include <AABB.h> // Align-Axis Bounding Box
#include <SAT.h> // Serparating Axis Thereom

#define FRAMERATE 90.00
#define VIRTUAL_WIDTH 1024.00
#define VIRTUAL_HEIGHT 800.00
#define SCALE (VIRTUAL_HEIGHT/10.00) // the height is the denominator in meters, HEIGHT/10 -> 10 meters at the height of the application
#define GRAVITY 9.82

float scale;

void drawAABB(AABB_Object a, Color col) {
	int winW = GetScreenWidth();
	int winH = GetScreenHeight();
	float scaleX = (float)winW / VIRTUAL_WIDTH;
	float scaleY = (float)winH / VIRTUAL_HEIGHT;
	//scale = (scaleX < scaleY) ? scaleX : scaleY; // fit

	// map meters -> pixels: VIRTUAL_HEIGHT represents 10 meters in world (change 10.0f if needed)
	float metersToPixels = VIRTUAL_HEIGHT / 10.0f; // pixels per meter
	float windowScale = (scaleX < scaleY) ? scaleX : scaleY; // fit
	scale = metersToPixels * windowScale;
	
	// convert physics meters -> pixels and flip Y so physics y=0 is bottom
    DrawRectangle(
			a.x * SCALE,
			a.y * SCALE,
			a.width * SCALE,
			a.height * SCALE,
			col
	);
}

int main() {
    InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "raylib [core] example - input keys");

    SetTargetFPS(FRAMERATE);
    float DT = 0;

    char printMe[15];
    char printSide[10];

    // global/runtime variables
    AABB_Object simpleObjects[1024] = {0};
    size_t objCount = 1;

    simpleObjects[0] = (AABB_Object){0,1,1,1,0,0,10};

    // program loop
    while (!WindowShouldClose()) {
        DT = GetFrameTime();
        
        // ---------- GET USER INPUT ----------
        strcpy(printMe,"not colliding");
        if (AABBcolliding(simpleObjects[0],simpleObjects[1])) {
            strcpy(printMe,"colliding");
            switch (whichSide(simpleObjects[0],simpleObjects[1])) {
                case Top: strcpy(printSide,"Top"); break;
                case Right: strcpy(printSide,"Right"); break;
                case Bottom: strcpy(printSide,"Bottom"); break;
                case Left: strcpy(printSide,"Left"); break;
                default: strcpy(printSide,"Error"); break;
            }
        }

        // ---------- SIMULATE ----------
        simulate(
            simpleObjects,
            objCount,
            DT,
            VIRTUAL_WIDTH / SCALE,
            VIRTUAL_HEIGHT / SCALE,
            GRAVITY
        );

        // ---------- DRAW ----------
        BeginDrawing();
            ClearBackground((Color){20,20,20,255});

            for (size_t i = 0; i < objCount; i++) {
                drawAABB(simpleObjects[i], WHITE);
            }

            DrawText(printMe, 5, 5, 20, WHITE);
            DrawText(printSide, 5, 30, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
