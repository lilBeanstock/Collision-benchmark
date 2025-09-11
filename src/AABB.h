#include <math.h>

typedef struct {
    float x;
    float y;
    float width;
    float height;
} AABB_Object;

typedef struct {
    float x;
    float y;
} Point;

enum Side {
    Top, Right, Bottom, Left
};

bool AABBcolliding(AABB_Object a, AABB_Object b) {
    return (
        a.x < b.x + b.width &&
        a.x + a.width > b.x &&
        a.y < b.y + b.height &&
        a.y + a.height > b.y
    );
}

Point getCenter(float x1,float y1,float x2,float y2) {
    float xmiddle = (x1+x2)/(2);
    float ymiddle = (y1+y2)/(2);
    Point t = (Point){xmiddle,ymiddle};
    return t;
}

// returns the Side enum classifications in form of integers
int whichSide(AABB_Object a, AABB_Object b) {
    // compare side gaps with regard to a
    float rightgap = fabs((a.x+a.width)-b.x);
    float leftgap = fabs(a.x-(b.x+b.width));
    float bottomgap = fabs((a.y+a.height)-b.y);
    float topgap = fabs(a.y-(b.y+b.height));
    
    printf("%f %f %f %f\n", rightgap, leftgap, bottomgap, topgap);

    // check which one is smallest, with topgap as default.
    enum Side lowestGapSide = Top;
    float lowestDist = topgap;
    if (lowestDist > bottomgap) {
        lowestGapSide = Bottom;
        lowestDist = bottomgap;
    }
    if (lowestDist > rightgap) {
        lowestGapSide = Right;
        lowestDist = rightgap;
    }
    if (lowestDist > leftgap) {
        lowestGapSide = Left;
        lowestDist = leftgap;
    }

    return lowestGapSide;
}