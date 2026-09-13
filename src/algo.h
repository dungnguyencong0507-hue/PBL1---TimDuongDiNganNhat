#include "header.h"

struct cost{
    int v;     // Chỉ số đỉnh
    float w;   // Khoảng cách
};

// Cấu trúc danh sách kề
struct adjlist{
    cost e[N]; // Danh sách các cạnh kề
    int size;  // Kích thước danh sách

    adjlist(){
        size = 0;
    }
};

void push(adjlist &a, cost x)
{
    a.e[a.size] = x;
    a.size++;
}

void free_adjlist(adjlist a[])
{
    for (int i=1; i<=numNodes; i++) a[i].size = 0;
}

// Định nghĩa heap - priority_queue
struct heap{
    cost *que; // Mảng lưu hàng đợi ưu tiên
    int size;  // Kích thước hiện tại của mảng
    // Khởi tạo hàng đợi ngày khi khai báo
    heap(int maxSize){
        que = new cost[maxSize + 1];
        size = 0;
    }
    // Giải phóng bộ nhớ khi dùng xong heap
    ~heap(){
        delete[] que;
    }
};

bool isHeapEmpty(heap &h)
{
    return !h.size;
}

void swap(cost &a, cost &b)
{
    cost c = a;
    a = b; b = c;
}

void min_heap(heap &h, int i)
{
    int m = i;
    int l = i*2;
    int r = i*2 + 1;
    if (l <= h.size && h.que[l].w < h.que[m].w) m = l;
    if (r <= h.size && h.que[r].w < h.que[m].w) m = r;
    if (m != i){
        swap(h.que[m], h.que[i]);
        min_heap(h, m);
    }
}

void push(heap &h, cost x)
{
    h.size++;
    h.que[h.size] = x;
    int i = h.size;
    while (i > 1){
        if (h.que[i].w < h.que[i/2].w) swap(h.que[i], h.que[i/2]);
        else break;
        i = i/2;
    }
}

cost top(heap &h)
{
    return h.que[1];
}

void pop(heap &h)
{
    if (isHeapEmpty(h)) return;
    h.que[1] = h.que[h.size];
    h.size--;
    if (h.size) min_heap(h, 1);
}

adjlist adj[N];
float a[N][N];

float DIJKSTRA[N];
int DIJKSTRA_TRACE[N];
float FLOYD[N][N];
int FLOYD_TRACE[N][N];

// =========================================
// ===========THUẬT TOÁN DIJKSTRA===========
// =========================================

bool dijkstra(int s, adjlist a[], int n) {
    // Kiểm tra xem có cạnh âm hay không
    for (int i=1; i<=n; i++){
        for (int j=0; j<a[i].size; j++) 
            if (a[i].e[j].w < 0.0f) return 0; 
    }
    heap q(numEdges);
    bool kt[N];
    // Khởi tạo giá trị ban đầu
    for (int i=1; i<=n; i++){ DIJKSTRA[i] = inf; kt[i] = 0; }
    DIJKSTRA[s] = 0; DIJKSTRA_TRACE[s] = s;
    push(q, (cost){s,0});
    int u, i;
    float w;
    
    while(!isHeapEmpty(q)){
        u = top(q).v; // Tìm đỉnh cần dán nhãn
        pop(q);
        if (kt[u]) continue;
        kt[u] = 1; // Dán nhãn
        // Giãn cạnh
        for(int j=0; j<a[u].size; j++) {
            i = a[u].e[j].v;
            w = a[u].e[j].w;
            if(!kt[i] && DIJKSTRA[i] > DIJKSTRA[u] + w){
                DIJKSTRA[i] = DIJKSTRA[u] + w;
                DIJKSTRA_TRACE[i] = u;
                push(q, (cost){i, DIJKSTRA[i]});
            }
        }
    }
    return 1;
}

void get_path_dijkstra(int s, int t, int path[], int &n) {
    n = 0;
    int u = t;
    stack<int> d; 
    d.push(t);
    while(u != s){ u = DIJKSTRA_TRACE[u]; d.push(u); }
    while(!d.empty()){ 
        path[n++] = d.top(); 
        d.pop();
    }
}

// =========================================
// ========THUẬT TOÁN FLOYD-WARSHALL========
// =========================================

bool floyd(float a[][N], int n) {
    for (int i=1; i<=n; i++) for (int j=1; j<=n; j++) FLOYD[i][j] = a[i][j];
    for (int i=1; i<=n; i++) for (int j=1; j<=n; j++) FLOYD_TRACE[i][j] = j;
        
    for (int k=1; k<=n; k++)
        for (int i=1; i<=n; i++)
            for (int j=1; j<=n; j++)
                if (FLOYD[i][k] + FLOYD[k][j] < FLOYD[i][j]){
                    FLOYD[i][j] = FLOYD[i][k] + FLOYD[k][j];
                    FLOYD_TRACE[i][j] = FLOYD_TRACE[i][k];
                }
    
    // kiểm tra xem có chu trình âm hay không
    for (int i=1; i<=n; i++)
        if (FLOYD[i][i] < 0.0f) return 0;
    return 1;
}

void get_path_floyd(int s, int t, int path[], int &n) {
    n = 0; 
    int u = s; 
    path[n++] = s;
    while (u != t) { 
        u = FLOYD_TRACE[u][t]; 
        path[n++] = u; 
    }
}