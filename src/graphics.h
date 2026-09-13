#include "header.h"
#include "algo.h"
#include "physics.h"

void DrawVertex(Node node)
{
    Color c;
    if (node.id == startNode) c = YELLOW;
    else if (node.id == endNode) c = BLUE;
    else c = WHITE;
    if (node.isDragging) c = ColorLerp(c, GRAY, 0.7);

    DrawCircleV(node.position, NODE_RADIUS, c);

    if (node.isFixed) DrawRing(node.position, NODE_RADIUS - 3, NODE_RADIUS, 0, 360, 36, BLACK);
    else DrawCircleLinesV(node.position, NODE_RADIUS, BLACK);

    float fontSize = 22 * stdratio;
    Vector2 textSize = MeasureTextEx(grpFont, TextFormat("%d", node.id), fontSize, 1);
    DrawTextEx(grpFont, TextFormat("%d", node.id), {node.position.x - textSize.x/2.0f, node.position.y - textSize.y/2.0f}, fontSize, 1, BLACK);
}

void DrawEdge(Edge edge)
{
    Vector2 start = nodes[edge.src].position;
    Vector2 control = edge.mid;
    Vector2 end = nodes[edge.dest].position;

    Color c = edge.isPath ? RED : GRAY;
    float border = edge.isPath ? LINE_WEIGHT*1.5 : LINE_WEIGHT;
    if (isDirectedMode){
        Vector2 a = {control.x - end.x, control.y - end.y};
        
        float l = GetDist(a, (Vector2){0,0});
        float t = NODE_RADIUS / l;
        end.x += a.x * t;
        end.y += a.y * t;
        
        a = (Vector2){a.x/l, a.y/l};

        Vector2 n = {-a.y, a.x};
        DrawLineEx(end, (Vector2){end.x + a.x * 15 * ratio + n.x * 5 * ratio, end.y + a.y * 15 * ratio + n.y * 5 * ratio}, border, c);
        DrawLineEx(end, (Vector2){end.x + a.x * 15 * ratio - n.x * 5 * ratio, end.y + a.y *15 * ratio - n.y * 5 * ratio}, border, c);
    }

    DrawSplineSegmentBezierQuadratic(start, control, end, border, c);

    Vector2 midPoint;
    midPoint.x = 0.25*start.x + 0.5*control.x + 0.25*end.x;
    midPoint.y = 0.25*start.y + 0.5*control.y + 0.25*end.y;
    
    float fontSize = 20 * stdratio;
    Vector2 textSize = MeasureTextEx(stdFont, TextFormat("%.1f", edge.weight), fontSize, 1);
    float padding = 3 * stdratio;

    Rectangle bgRec = { 
        midPoint.x - (textSize.x / 2.0f) - padding, 
        midPoint.y - (textSize.y / 2.0f) - padding, 
        textSize.x + padding*2, 
        textSize.y + padding*2 
    };

    DrawRectangleRec(bgRec, RAYWHITE);
    DrawTextEx(stdFont, TextFormat("%.1f", edge.weight), {bgRec.x + padding, bgRec.y + padding}, fontSize, 1, BLACK);
}

void ShuffleVertex()
{
    for (int i=1; i<=numNodes; i++){
        nodes[i].position = (Vector2){(float)GetRandomValue(centerX + 50, centerX + centerW - 50), (float)GetRandomValue(panelY + 50, panelY + panelH - 50)};
    }
}

void EditGraphData(int visibleRows, int visibleCols)
{
    int len;
    if (IsKeyPressed(KEY_UP) && ptrY > 0) {
        if (ptrY == textScrollY) textScrollY--;
        ptrY--;
        len = (int)strlen(graphDataInput[ptrY]);
        if (ptrX > len) ptrX = len;
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }
    if (IsKeyPressed(KEY_DOWN) && ptrY < numRow - 1) {
        if (ptrY == textScrollY + visibleRows - 1) textScrollY++;
        ptrY++;
        len = (int)strlen(graphDataInput[ptrY]);
        if (ptrX >= len) ptrX = len;
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        if (ptrX == (int)strlen(graphDataInput[ptrY]) && ptrY < numRow - 1){
            if (ptrY == textScrollY + visibleRows - 1) textScrollY++;
            ptrY++; 
            ptrX = 0;
        }
        else ptrX++;
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }
    if (IsKeyPressed(KEY_LEFT)) {
        if (ptrX == 0 && ptrY > 0){
            if (ptrY == textScrollY) textScrollY--;
            ptrY--; 
            ptrX = (int)strlen(graphDataInput[ptrY]);
        }
        else if (ptrX > 0){
            ptrX--;
        }
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        len = (int)strlen(graphDataInput[ptrY]);
        if (len > 0 && ptrX > 0) {
            for (int j=ptrX-1; j<len; j++) graphDataInput[ptrY][j] = graphDataInput[ptrY][j+1];
            if (ptrX == textScrollX) textScrollX--;
            ptrX--;
        } else if (len == 0 && ptrY > 0) {
            for (int j=ptrY; j<numRow-1; j++) strcpy(graphDataInput[j], graphDataInput[j+1]);
            numRow--;
            if (textScrollY > 0) textScrollY--;
            ptrY--;
            ptrX = (int)strlen(graphDataInput[ptrY]);
        }
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }

    if (IsKeyPressed(KEY_DELETE)) {
        len = (int)strlen(graphDataInput[ptrY]);
        if (ptrX < len && len > 0) {
            for (int j=ptrX; j<len; j++) graphDataInput[ptrY][j] = graphDataInput[ptrY][j+1];
        }
        if (ptrX == len && ptrY < numRow-1 && (int)strlen(graphDataInput[ptrY+1]) == 0){
            for (int j=ptrY+1; j<numRow-1; j++) strcpy(graphDataInput[j], graphDataInput[j+1]);
            numRow--;
        }
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (ptrY == numRow - 1 && numRow < MAX_ROW) numRow++;
        if (ptrY == textScrollY + visibleRows - 1) textScrollY++;
        ptrY++;
        ptrX = 0;
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }

    len = (int)strlen(graphDataInput[ptrY]);
    char key = GetCharPressed();
    if ((((key >= '0') && (key <= '9')) || (key == ' ') || (key == '.') || (key == '-') || (key == 'e')) && (len < MAX_COL - 1)) {
        for (int j=len; j>ptrX; j--) graphDataInput[ptrY][j] = graphDataInput[ptrY][j-1];
        if (ptrX == textScrollX + visibleCols) textScrollX++;
        graphDataInput[ptrY][ptrX++] = key;
        graphDataInput[ptrY][len + 1] = '\0';
        if (ptrX > textScrollX + visibleCols - 1 || ptrX < textScrollX) textScrollX = ptrX - min(ptrX, visibleCols - 1);
        if (ptrY > textScrollY + visibleRows || ptrY < textScrollY) textScrollY = min(ptrY, numRow - visibleRows);
    }
}

void LoadGraphFromString(const char textData[][MAX_COL]) {
    if (sscanf(textData[0], "%d", &numNodes) < 1) return;     
    if (numNodes > 100) return; 

    for (int i=1; i<=numNodes; i++) {
        nodes[i].id = i;
        nodes[i].veloc = (Vector2){0,0};
        nodes[i].isDragging = false;
        nodes[i].isFixed = false;
        KeepInScreen(nodes[i].position);
    }     
    for (int i=1; i<=numNodes; i++) {
        for (int j=1; j<=numNodes; j++) {
            a[i][j] = (i == j ? 0 : inf);
        }
    } 

    free_adjlist(adj);

    int row = 1;
    int x, y;
    float z;     
    while (row <= numRow) {
        if (sscanf(textData[row++], "%d %d %f", &x, &y, &z) < 2) continue;

        if (z < a[x][y]) a[x][y] = z; 
        else continue;
        push(adj[x], (cost){y, z});
        
        if (!isDirectedMode) {
            a[y][x] = a[x][y];
            push(adj[y], (cost){x, z});
        }
    }

    numEdges = 0;
    for (int i=1; i<=numNodes; i++){
        for (int j=1; j<=numNodes; j++) if (a[i][j] < inf) {
            if (!isDirectedMode && i > j) continue;
            
            edges[numEdges].src = i;
            edges[numEdges].dest = j;
            edges[numEdges].weight = a[i][j];
            edges[numEdges].isPath = false;
            numEdges++;
        }
    }

    graphUpdated = true;
}

void LoadStringFromFile(char *filePath){
    FILE *fp = fopen(filePath, "r");

    if (fp != NULL) {
        for (int i=0; i<numRow; i++) strcpy(graphDataInput[i], "");
        int rowptr = 0;
        while (numRow < MAX_ROW && fgets(graphDataInput[rowptr], MAX_COL, fp)) {
            graphDataInput[rowptr][strcspn(graphDataInput[rowptr], "\r\n")] = '\0';
            if (rowptr == numRow - 1) numRow++;
            rowptr++;
        }
        
        sprintf(statusMsg, "Đã tải file: %s", fileNameInput);
        fclose(fp);
    } else {
        sprintf(statusMsg, "LỖI: Không tìm thấy file!");
    }    
}

void SaveFileFromString(char *filePath){
    FILE *fp = fopen(filePath, "w");

    if (fp != NULL) {
        fprintf(fp, graphOutput);
        
        sprintf(statusMsg, "Đã lưu file: %s", fileNameOutput);
        fclose(fp);
    } else {
        sprintf(statusMsg, "LỖI: Không thể lưu file!");
    }
}

void HighlightPath(int pathNodes[], int n) {
    for (int i=0; i<numEdges; i++) edges[i].isPath = false;

    for (int i=0; i<n-1; i++) {
        int u = pathNodes[i];
        int v = pathNodes[i+1];
        for (int j=0; j<numEdges; j++) {
            if (isDirectedMode) {
                if (edges[j].src == u && edges[j].dest == v) edges[j].isPath = true;
            } else {
                if ((edges[j].src == u && edges[j].dest == v) || (edges[j].src == v && edges[j].dest == u)) edges[j].isPath = true;
            }
        }
    }
}

void FindShortestPath()
{
    int path[N], pathLength = 0;
        
    char *startByte, *currByte = graphOutput;

    if (graphUpdated) {
        isFloydValid = floyd(a, numNodes);

        isDijkstraValid = dijkstra(startNode, adj, numNodes);
        preStartNode = startNode;

        graphUpdated = false;
    }
    
    if (algoChoice == 0) {
        if (preStartNode != startNode){
            isDijkstraValid = dijkstra(startNode, adj, numNodes);
            preStartNode = startNode;
        }

        if (isDijkstraValid){
            if (DIJKSTRA[endNode] < inf){
                get_path_dijkstra(startNode, endNode, path, pathLength);

                sprintf(statusMsg, "Đã tìm ra đường đi ngắn nhất");

                startByte = currByte += sprintf(currByte, "Dijkstra: Chi phí = %.2f\n", DIJKSTRA[endNode]);
                currByte += sprintf(currByte, "Đường đi: %d ", path[0]);
                for (int i=1; i<pathLength; i++){
                    if ((currByte - startByte) >= 22){
                        currByte += sprintf(currByte, "\n");
                        startByte = currByte;
                    }
                    currByte += sprintf(currByte, "-> %d ", path[i]);
                }
            } else {
                sprintf(statusMsg, "Không tìm thấy đường đi");
                strcpy(graphOutput, "");
            }
        } else {
            sprintf(statusMsg, "LỖI: Tồn tại cạnh âm!"); 
            strcpy(graphOutput, "");
        }
    } else {
        if (isFloydValid){
            if (FLOYD[startNode][endNode] < inf){
                get_path_floyd(startNode, endNode, path, pathLength);

                sprintf(statusMsg, "Đã tìm ra đường đi ngắn nhất");

                startByte = currByte += sprintf(currByte, "Floyd: Chi phí = %.2f\n", FLOYD[startNode][endNode]);
                currByte += sprintf(currByte, "Đường đi: %d ", path[0]);
                for (int i=1; i<pathLength; i++){
                    if ((currByte - startByte) >= 22){
                        currByte += sprintf(currByte, "\n");
                        startByte = currByte;
                    }
                    currByte += sprintf(currByte, "-> %d ", path[i]);
                }
            } else {
                sprintf(statusMsg, "Không tìm thấy đường đi");
                strcpy(graphOutput, "");
            }
        } else {
            sprintf(statusMsg, "LỖI: Tồn tại chu trình âm!");
            strcpy(graphOutput, "");
        }
    }

    HighlightPath(path, pathLength);
}

void UpdateLayout()
{
    padding = sw * 0.015f;
    topH = sh * 0.06f;
    leftW = rightW = sw * 0.25f;
    centerW = sw - leftW - rightW - (padding * 4);
    leftX = padding;
    centerX = leftX + leftW + padding;
    rightX = centerX + centerW + padding;
    panelY = topH + padding * 2;
    panelH = sh - topH - (padding * 3);

    bigFontsz = min(sw, sh) * 0.028;
}

void SetUpUI()
{
    ClearBackground(RAYWHITE);

    // Tên đề tài
    DrawTextEx(stdFont, "PBL1 - Lập trình đồ án tính toán\nĐề tài: Tìm đường đi ngắn nhất (Floyd, Dijkstra)\nGiảng viên hướng dẫn: Đỗ Thị Tuyết Hoa", (Vector2){leftX, padding*0.5f}, bigFontsz, 1.0f, DARKBLUE);
    DrawTextEx(stdFont, "Sinh viên thực hiện:\n-Nguyễn Công Dũng - 25T_DT3\n-Nguyễn Văn Tài - 25T_DT3", (Vector2){leftX + 0.36f*sw, padding*0.5f}, bigFontsz, 1.0f, DARKBLUE);

    // ----- TOP MENU -----
    float menuW = fmin(200.0f, sw * 0.33f); 
    GuiToggleGroup((Rectangle){ sw - menuW*2 - padding, padding, menuW, topH }, "Vô hướng;Có hướng", &dum1);
    if (dum1 != prevDum1) {
        isDirectedMode = (dum1 == 1);
        LoadGraphFromString(graphDataInput); 
        prevDum1 = dum1;
    }

    // ----- LEFT PANEL -----
    GuiPanel((Rectangle){ leftX, panelY, leftW, panelH }, "Quản lí dữ liệu");
    
    float innerPad = leftW * 0.08f; 
    float innerW = leftW - (innerPad * 2);
    float currentY = panelY + 35.0f; 

    // Nhập tên file input
    DrawTextEx(stdFont, "1. Dữ liệu từ File:", (Vector2){leftX + innerPad, currentY}, bigFontsz, 1, DARKBLUE);
    currentY += 25.0f;

    GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(DARKBLUE));
    if (GuiTextBox((Rectangle){ leftX + innerPad, currentY, innerW, 35.0f }, fileNameInput, 256, editInputFileNameMode)) {
        editInputFileNameMode = !editInputFileNameMode;
        editGraphDataMode = editOutputFileNameMode = editEnd = editStart = false;
    }
    currentY += 45.0f;

    // Nút cập nhật dữ liệu từ file
    float buttonH = 40.0f;
    if (GuiButton((Rectangle){ leftX + innerPad, currentY, innerW, buttonH }, "#01# TẢI FILE")) {
        char filePathInput[512];
        sprintf(filePathInput, "test/%s", fileNameInput);
        
        LoadStringFromFile(filePathInput);
        LoadGraphFromString(graphDataInput);
        
        editInputFileNameMode = false;
    }
    currentY += 60.0f;

    // Hiển thị và thay đổi data
    DrawTextEx(stdFont, "2. Chỉnh sửa thủ công:", (Vector2){leftX + innerPad, currentY}, bigFontsz, 1, DARKBLUE);
    currentY += 25.0f;

    float gutterWidth = 40.0f;
    float textH = (panelY + panelH) - currentY - buttonH - 15.0f; 
    float textX = leftX + innerPad + gutterWidth;
    float rowMargin = 2;
    float textSize = 16;
    float textPad = 5;
    Rectangle textArea = { leftX + innerPad + gutterWidth, currentY, innerW - gutterWidth, textH };
    Vector2 charW = MeasureTextEx(dataFont, "a", textSize, 1.0f);
    float lineHeight = charW.y;
    int visibleRows = (int)((textH - textPad) / (lineHeight + rowMargin));
    int visibleCols = (int)((innerW - gutterWidth - textPad) / (charW.x + 1.0f));

    DrawRectangle(leftX + innerPad, currentY, gutterWidth, textH, LIGHTGRAY);
    DrawRectangleLinesEx(textArea, 1, LIGHTGRAY);

    if (CheckCollisionPointRec(mousePos, textArea)){
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            editGraphDataMode = true;

            ptrX = textScrollX + (int) (mousePos.x - textX) / (charW.x + 1.0f);
            ptrY = textScrollY + (int) (mousePos.y - currentY - textPad) / (charW.y + rowMargin);
            ptrX = min(ptrX, (int)strlen(graphDataInput[ptrY]));
        }

        if (wheelMove != 0.0f){
            if (wheelMove > 0.0f && textScrollY > 0) textScrollY--;
            else if (wheelMove < 0.0f && textScrollY + visibleRows <= numRow) textScrollY++;
        }
    }
    
    if (editGraphDataMode) EditGraphData(visibleRows, visibleCols);

    char realText[visibleCols + 1];
    int endRow = min(textScrollY + visibleRows, numRow);
    Rectangle lineRect = { textX, currentY + textPad, leftW - 2*innerPad - gutterWidth, lineHeight };

    for (int i = textScrollY; i < endRow; i++) {
        if (i == ptrY && editGraphDataMode) {
            DrawRectangleRec(lineRect, Fade(SKYBLUE, 0.3f));
            
            if (((int)(GetTime() * 2) % 2 == 0)) {
                float ptrPos = 5 + (ptrX - textScrollX) * (charW.x + 1.0f) - 1.0f;
                DrawRectangle(lineRect.x + ptrPos, lineRect.y, 2, lineHeight, BLACK);
            }
        }

        for (int j = textScrollX; j < textScrollX + visibleCols; j++){
            if ((realText[j - textScrollX] = graphDataInput[i][j]) == '\0') break;
            realText[j+1] = '\0';
        } 

        DrawTextEx(dataFont, TextFormat("%d", i), {leftX + innerPad + 10, lineRect.y}, textSize, 1.0f, DARKGRAY);
        DrawTextEx(dataFont, realText, {lineRect.x + textPad, lineRect.y}, textSize, 1.0f, BLACK);

        lineRect.y += rowMargin + lineHeight;
    } 

    // Nút cập nhật đồ thị
    if (GuiButton((Rectangle){ leftX + innerPad, currentY + textH + 5.0f, innerW, buttonH }, "CẬP NHẬT ĐỒ THỊ")) {
        LoadGraphFromString(graphDataInput);
        sprintf(statusMsg, "Đồ thị đã được cập nhật!");

        editGraphDataMode = false;
    }

    // ----- GRAPH PANEL -----
    Rectangle graphArea = {centerX, panelY, centerW, panelH};
    DrawRectangleLinesEx(graphArea, 2.0f, GRAY);

    if (CheckCollisionPointRec(mousePos, graphArea) && wheelMove != 0.0f){
        if (wheelMove < 0.0f && ratio > 0.2f) ratio -= 0.02;
        else if (wheelMove > 0.0f && ratio < 1.0f) ratio += 0.02;
        stdratio = fmax(ratio, 0.6f);
    }

    // ----- RIGHT PANEL -----
    GuiPanel((Rectangle){ rightX, panelY, rightW, panelH }, "Thanh công cụ");
    
    float rPad = rightW * 0.08f;
    float rInW = rightW - (rPad * 2);

    float rY = panelY + 40.0f;
    float rowH = panelH * 0.07f;
    float spacing = panelH * 0.01f;

    // Chọn thuật toán
    GuiLabel((Rectangle){ rightX + rPad, rY, rInW * 0.4f, rowH }, "Thuật toán");
    GuiComboBox((Rectangle){ rightX + rPad + (rInW * 0.4f), rY, rInW * 0.6f, rowH }, "Dijkstra;Floyd", &algoChoice);

    rY += rowH + spacing;

    // Nhập đỉnh bắt đầu
    GuiLabel((Rectangle){ rightX + rPad, rY, rInW * 0.4f, rowH }, "Bắt đầu:");
    if (GuiValueBox((Rectangle){ rightX + rPad + (rInW * 0.4f), rY, rInW * 0.6f, rowH }, NULL, &startNode, 1, numNodes, editStart)) {
        editStart = !editStart;
        editInputFileNameMode = editOutputFileNameMode = editGraphDataMode = false;
    }
    rY += rowH + spacing;

    // Nhập đỉnh kết thúc
    GuiLabel((Rectangle){ rightX + rPad, rY, rInW * 0.4f, rowH }, "Kết thúc:");
    if (GuiValueBox((Rectangle){ rightX + rPad + (rInW * 0.4f), rY, rInW * 0.6f, rowH }, NULL, &endNode, 1, numNodes, editEnd)) {
        editEnd = !editEnd;
        editInputFileNameMode = editOutputFileNameMode = editGraphDataMode = false;
    }
    rY += rowH + spacing * 1.5f;

    // Nút thực thi thuật toán tìm đường đi ngắn nhất
    if (GuiButton((Rectangle){ rightX + rPad, rY, rInW, rowH }, "TÌM ĐƯỜNG ĐI")) {
        FindShortestPath();
    }
    rY += rowH + spacing * 1.5f;

    // Nút xáo trộn đỉnh
    if (GuiButton((Rectangle){ rightX + rPad, rY, rInW, rowH }, "XÁO ĐỈNH")) ShuffleVertex();
    rY += rowH + spacing * 1.5f;

    // Hiển thị trạng thái
    DrawTextEx(stdFont, "Trạng thái:", {rightX + rPad, rY}, bigFontsz, 1, GRAY);
    DrawTextEx(stdFont, statusMsg, {rightX + rPad, rY + panelH * 0.04f}, bigFontsz, 1, RED);
    rY += rowH + spacing;

    // Hiển thị kết quả
    float outTextH = panelH * 0.24f;
    GuiTextBox((Rectangle){rightX + rPad, rY, rInW, outTextH}, graphOutput, bigFontsz, false);
    rY += outTextH + spacing;

    // Nhập file output
    GuiLabel((Rectangle){ rightX + rPad, rY, rInW * 0.3f, rowH }, "File out:");
    GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(DARKBLUE));
    if (GuiTextBox((Rectangle){ rightX + rPad + rInW * 0.35f, rY, rInW * 0.65f, rowH }, fileNameOutput, 256, editOutputFileNameMode)) {
        editOutputFileNameMode = !editOutputFileNameMode;
        editGraphDataMode = editInputFileNameMode = false;
    }
    rY += rowH + spacing;
    
    // Lưu kết quả file
    if (GuiButton((Rectangle){ rightX + rPad, rY, rInW, rowH }, "#02# LƯU FILE")) {
        char filePathOutput[512];
        sprintf(filePathOutput, "test/%s", fileNameOutput);
        
        SaveFileFromString(filePathOutput);

        editOutputFileNameMode = false;
    }
}

void MouseCheck(Vector2 mousePos)
{
    float deltaTime = GetFrameTime();
    if (doubleClickTimer > 0) doubleClickTimer -= deltaTime;
    else clickCount = 0;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i=1; i<=numNodes; i++) {
            if (CheckCollisionPointCircle(mousePos, nodes[i].position, NODE_RADIUS)) {
                clickCount++;
                if (clickCount == 1) {
                    doubleClickTimer = 0.25f; 
                    selectedNodeIndex = i;
                    nodes[i].isDragging = true;
                } 
                else if (clickCount == 2 && selectedNodeIndex == i) {
                    nodes[i].isFixed = !nodes[i].isFixed;
                    clickCount = 0;
                    doubleClickTimer = 0;
                    nodes[i].isDragging = false;
                }
                break;
            }
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        for (int i=1; i<=numNodes; i++) 
            nodes[i].isDragging = false;
    }
}

void DrawGraph()
{
    MouseCheck(mousePos);

    PhysicsCalc();

    for (int i=0; i<numEdges; i++) DrawEdge(edges[i]);
    for (int i=1; i<=numNodes; i++) DrawVertex(nodes[i]);
}