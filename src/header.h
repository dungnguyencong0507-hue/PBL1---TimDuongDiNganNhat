#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <math.h>
#include <stack>
#include <raylib.h>
#include <string.h>
using namespace std;

// #include <stdlib.h>
// #ifdef __cplusplus
// extern "C" {
// #endif
//     inline float TextToFloat(const char *text) { return (float)atof(text); }
// #ifdef __cplusplus
// }
// #endif
// -------------------------------------------------------------
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

float ratio = 1.0f;
float stdratio = 1.0f;
#define NODE_RADIUS 18.0f * stdratio
#define LINE_WEIGHT 2.0f  
#define GRAPH_PANEL_MARGIN 30.0f
#define MAX_ROW 8192
#define MAX_COL 128
#define N 105
#define M 10005
#define inf 1e30

struct Node{
    int id;            // Chỉ số đỉnh
    Vector2 position;  // Vị trí đỉnh
    Vector2 veloc;     // Vận tốc đỉnh
    bool isDragging;   // Đỉnh có đang được kéo
    bool isFixed;      // Đỉnh có đang cố định
};

struct Edge{
    int src;           // Đỉnh nguồn
    int dest;          // Đỉnh đích
    float weight;      // Trọng số cạnh
    bool isPath;       // Cạnh có thuộc đường đi
    Vector2 mid;       // Điểm giữa của cạnh
};

Node nodes[N];
Edge edges[M];
int numEdges = 0;
int numNodes = 0;

float sw, sh;
float padding, leftW, centerW, rightW, leftX, centerX, rightX, topH, panelY, panelH;

int dum1, prevDum1;
bool isDirectedMode = true;
int algoChoice = 0; 

bool isDijkstraValid;
bool isFloydValid;

int preStartNode = 0;
int startNode = 1;
int endNode = 5;

Vector2 mousePos;
float wheelMove;

const char *vietnameseGlyphs = "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ:;\"\'<>,.?/{}[]|\\1234567890!@#$%^&*()-=_+`~aàáảãạăằắẳẵặâầấẩẫậeèéẻẽẹêềếểễệiìíỉĩịoòóỏõọôồốổỗộơờớởỡợuùúủũụưừứửữựyỳýỷỹỵAÀÁẢÃẠĂẰẮẲẴẶÂẦẤẨẪẬEÈÉẺẼẸÊỀẾỂỄỆIÌÍỈĨỊOÒÓỎÕỌÔỒỐỔỖỘƠỜỚỞỠỢUÙÚỦŨỤƯỪỨỬỮỰYỲÝỶỸỴĐđ";
int codepointsCount = 0;
int *codepoints = LoadCodepoints(vietnameseGlyphs, &codepointsCount);
Font stdFont, grpFont, dataFont;
float bigFontsz = 20;

char graphOutput[1000];
char graphDataInput[MAX_ROW][MAX_COL] = {"7\0", "1 2 4\0", "2 1 3\0", "1 6 10\0", "2 3 6\0", "4 2 18\0", "6 4 19\0", "7 6 10\0", "7 4 8\0", "5 4 2\0", "5 3 12\0", "4 3 15\0"};
char fileNameInput[256] = "data.txt";
char fileNameOutput[256] = "result.txt";
char statusMsg[150] = "Sẵn sàng!";

bool editStart = false, editEnd = false;
bool editGraphDataMode = false; 
bool editInputFileNameMode = false;
bool editOutputFileNameMode = false;
bool graphUpdated = false;

int numRow = 21;
int textScrollY = 0;
int textScrollX = 0;
int ptrX = 0;
int ptrY = 0;

int clickCount = 0;
float doubleClickTimer = 0;
int selectedNodeIndex = -1;

#endif