#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include <AABB.h> // Align-Axis Bounding Box
#include <SAT.h> // Serparating Axis Thereom

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

    // program loop
    while (!WindowShouldClose()) {
        

        if (IsKeyDown(KEY_W)) c = 'w';

        printMe[17] = c;

        // ---------- DRAW ----------
        BeginDrawing();
            ClearBackground((Color){20,20,20,255});

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