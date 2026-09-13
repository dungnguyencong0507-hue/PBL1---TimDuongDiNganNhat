#include "header.h"
#include "graphics.h"

using namespace std;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(1200, 700, "Tim duong di ngan nhat");
    SetTargetFPS(60); 

    stdFont = LoadFontEx("resources/fonts/static/Roboto_SemiCondensed-Medium.ttf", 50, codepoints, codepointsCount);
    grpFont = LoadFontEx("resources/fonts/static/Roboto_Condensed-Black.ttf", 50, codepoints, codepointsCount);
    dataFont = LoadFontEx("resources/fonts/consolas/Consolas-Regular.ttf", 50, codepoints, codepointsCount);
    GuiSetFont(stdFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    UnloadCodepoints(codepoints);

    dum1 = prevDum1 = isDirectedMode;

    LoadGraphFromString(graphDataInput);
    ShuffleVertex();

    while (!WindowShouldClose()) {
        sw = GetScreenWidth();
        sh = GetScreenHeight();
        UpdateLayout();

        mousePos = GetMousePosition();
        wheelMove = GetMouseWheelMove();

        BeginDrawing();
        SetUpUI(); 
        DrawGraph(); 
        EndDrawing();
    }

    CloseWindow();
    return 0;
}