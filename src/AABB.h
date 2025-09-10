typedef struct {
    float width;
    float height;
    float x;
    float y;
} AABB_Object;

typedef struct {
    float x;
    float y;
} Point;

enum Side {
    Top, Left, Bottom, Right
};

bool AABBcolliding(AABB_Object a, AABB_Object b) {
    return (
        a.x < b.x + b.width &&
        a.x + a.width > b.width &&
        a.y < b.y + b.height &&
        a.y + a.height > b.width
    );
}

Point getCenter(x1,y1,x2,y2) {
    float xmiddle = (x1+x2)/(2);
    float ymiddle = (y1+y2)/(2);
    Point t = (Point){xmiddle,ymiddle};
    return t;
}

// returns the Side enum classifications in form of integers
int whichSide(AABB_Object a, AABB_Object b) {
    // get center of each side of each object
    Point aSides[] = {
        getCenter(a.x,a.y,a.x+a.width,a.y), // top part, from x,y to x+width,y
        getCenter(a.x+a.width,a.y,a.x+a.width,a.y+a.height), // right part, from x+width,y to x+width,y+height
        getCenter(a.x,a.y+a.height,a.x+a.width,a.y+a.height), // bottom part, from x,y+height to x+width,y+height
        getCenter(a.x,a.y,a.x,a.y+a.height) // left part, from x,y to x,y+height
    };
    Point bSides[] = {
        getCenter(b.x,b.y,b.x+b.width,b.y), // top part, from x,y to x+width,y
        getCenter(b.x+b.width,b.y,b.x+b.width,b.y+b.height), // right part, from x+width,y to x+width,y+height
        getCenter(b.x,b.y+b.height,b.x+b.width,b.y+b.height), // bottom part, from x,y+height to x+width,y+height
        getCenter(b.x,b.y,b.x,b.y+b.height) // left part, from x,y to x,y+height
    };

}