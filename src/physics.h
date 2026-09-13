#include "header.h"

#define FORCE_CONSTANT 0.5f
#define FRICTION 0.2f   
#define IDEAL_LENGTH 200.0f * ratio    
#define AVOID_DISTANCE 50.0f * ratio
#define BEND_STRENGTH 100.0f * ratio
#define MAX_SPEED 100.0f


float GetDist(Vector2 p1, Vector2 p2) 
{
    return sqrtf(powf(p2.x - p1.x, 2) + powf(p2.y - p1.y, 2));
}

float GetDotProduct(Vector2 v1, Vector2 v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

Vector2 GetMidPoint(Vector2 p1, Vector2 p2)
{
    return (Vector2){(p1.x + p2.x) / 2, (p1.y + p2.y) / 2};
}

Vector2 Vertext_VertexCompulsion(Vector2 p1, Vector2 p2)
{
    float d = GetDist(p1, p2);
    if (d >= IDEAL_LENGTH) return (Vector2){0,0};
    if (d < 1.0f) d = 1.0f;
    float f = FORCE_CONSTANT * (IDEAL_LENGTH - d);
    return (Vector2){(p1.x - p2.x) / d * f, (p1.y - p2.y) / d * f};
}

Vector2 AttractiveForce(Vector2 p1, Vector2 p2)
{
    float d = GetDist(p1, p2);
    if (d <= IDEAL_LENGTH) return (Vector2){0,0};
    float f = FORCE_CONSTANT * (d - IDEAL_LENGTH);
    if (d < 1.0f) d = 1.0f;
    return (Vector2){(p2.x - p1.x) / d * f, (p2.y - p1.y) / d * f};
}

void Vertex_EdgeCompulsion(Edge &edge, Node node)
{
    if (node.id == edge.src || node.id == edge.dest) return;
    float d, t, l;
    Vector2 n, a, b;

    a = {nodes[edge.dest].position.x - nodes[edge.src].position.x, nodes[edge.dest].position.y - nodes[edge.src].position.y};
    l = GetDist(a, {0,0});
    b = {nodes[edge.src].position.x - node.position.x, nodes[edge.src].position.y - node.position.y};
    t = - GetDotProduct(a, b) / (l*l);

    if (t < -1e-9 || t > 1.0f) return;

    n = {b.x + a.x * t, b.y + a.y * t};

    d = GetDist(n, {0,0});
    if (d > AVOID_DISTANCE) return;
    if (d < 1.0){
        n = {-a.y, a.x};
        d = GetDist(n, {0,0});
    } 
    n = {n.x / d, n.y / d};
    edge.mid.x += n.x * BEND_STRENGTH;
    edge.mid.y += n.y * BEND_STRENGTH;
}

void Edge_EdgeCompulsion(Edge &edge1, Edge edge2)
{
    if (edge1.src != edge2.dest || edge1.dest != edge2.src) return;
    
    float d;
    Vector2 n;
    n = {nodes[edge1.src].position.y - nodes[edge1.dest].position.y, nodes[edge1.dest].position.x - nodes[edge1.src].position.x};
    d = GetDist(n, {0,0});
    n = {n.x / d, n.y / d};

    edge1.mid.x += n.x * BEND_STRENGTH * 0.3;
    edge1.mid.y += n.y * BEND_STRENGTH * 0.3;
}

void KeepInScreen(Vector2 &p)
{
    if (p.x > centerX + centerW - GRAPH_PANEL_MARGIN) p.x = centerX + centerW - GRAPH_PANEL_MARGIN;
    if (p.x < centerX + GRAPH_PANEL_MARGIN) p.x = centerX + GRAPH_PANEL_MARGIN;
    if (p.y > panelY + panelH - GRAPH_PANEL_MARGIN) p.y = panelY + panelH - GRAPH_PANEL_MARGIN;
    if (p.y < panelY + GRAPH_PANEL_MARGIN) p.y = panelY + GRAPH_PANEL_MARGIN;
}

void PhysicsCalc()
{
    Vector2 DeltaVeloc;
    float speed;

    for (int i=1; i<=numNodes; i++) {
        if (nodes[i].isDragging) {
            nodes[i].position = mousePos;
            nodes[i].veloc = (Vector2){0,0};
            KeepInScreen(nodes[i].position);
            continue;
        }

        if (nodes[i].isFixed) continue;

        for (int j=1; j<=numNodes; j++) {
            if (i == j) continue;
            DeltaVeloc = Vertext_VertexCompulsion(nodes[i].position, nodes[j].position);
            nodes[i].veloc.x += DeltaVeloc.x;
            nodes[i].veloc.y += DeltaVeloc.y;
        }

        for (int j=0; j<numEdges; j++) {
            int other = -1;
            if (edges[j].src == i) other = edges[j].dest;
            else if (edges[j].dest == i) other = edges[j].src;
            if (other == -1) continue;
            DeltaVeloc = AttractiveForce(nodes[i].position, nodes[other].position);
            nodes[i].veloc.x += DeltaVeloc.x;
            nodes[i].veloc.y += DeltaVeloc.y;
        }

        speed = GetDist(nodes[i].veloc, {0,0});
        if (speed > MAX_SPEED){
            (nodes[i].veloc.x /= speed) *= MAX_SPEED;
            (nodes[i].veloc.y /= speed) *= MAX_SPEED;
        }

        nodes[i].position.x += nodes[i].veloc.x;
        nodes[i].position.y += nodes[i].veloc.y;
        nodes[i].veloc.x *= FRICTION;
        nodes[i].veloc.y *= FRICTION;

        KeepInScreen(nodes[i].position); 
    }


    for (int i=0; i<numEdges; i++){
        edges[i].mid = GetMidPoint(nodes[edges[i].src].position, nodes[edges[i].dest].position);

        for (int j=1; j<=numNodes; j++)
            Vertex_EdgeCompulsion(edges[i], nodes[j]);

        for (int j=0; j<numEdges; j++)
            Edge_EdgeCompulsion(edges[i], edges[j]); 
    }
}