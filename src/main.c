#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include <AABB.h> // Align-Axis Bounding Box
#include <SAT.h> // Serparating Axis Thereom

void drawAABB(AABB_Object a,Color col) {
    DrawRectangle(a.x,a.y,a.width,a.height,col);
}

int main() {
    
    const int WIDTH = 1024;
    const int HEIGHT = 800;

    InitWindow(WIDTH, HEIGHT, "raylib [core] example - input keys");

    SetTargetFPS(60);

    char c = 'b';
    char printMe[] = "YOU PRESSED THE ' ' KEY";

    // global/runtime variables
    AABB_Object simpleObjects[1024] = {0};
    float gravity = -9.82f;

    simpleObjects[0] = (AABB_Object){0,0,100,100};
    simpleObjects[1] = (AABB_Object){70,70,100,100};
    printf("%d\n", AABBcolliding(simpleObjects[0],simpleObjects[1]));
    printf("%d\n", whichSide(simpleObjects[0],simpleObjects[1]));

    // program loop
    while (!WindowShouldClose()) {
        

        if (IsKeyDown(KEY_W)) c = 'w';

        printMe[17] = c;

        // ---------- DRAW ----------
        BeginDrawing();
            ClearBackground((Color){20,20,20,255});

            drawAABB(simpleObjects[0],WHITE);
            drawAABB(simpleObjects[1],BLUE);

            DrawText(printMe, 5, 5, 20, WHITE);
        EndDrawing();
            
    }

    CloseWindow();
    return 0;
}

// int main() {
//     printf("%d",addone(givetwo()));
//     return 0;
// }