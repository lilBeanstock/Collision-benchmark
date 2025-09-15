#include <math.h>

typedef struct {
    double x;
    double y;
    double width;
    double height;
    double dx;
    double dy;
    double mass;
} AABB_Object;

typedef struct {
    double x;
    double y;
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

// returns the Side enum classifications in form of integers
int whichSide(AABB_Object a, AABB_Object b) {
    // compare side gaps with regard to a
    double rightgap = fabs((a.x+a.width)-b.x);
    double leftgap = fabs(a.x-(b.x+b.width));
    double bottomgap = fabs((a.y+a.height)-b.y);
    double topgap = fabs(a.y-(b.y+b.height));
    
    
    // check which one is smallest, with topgap as default.
    enum Side lowestGapSide = Top;
    double lowestDist = topgap;
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

void simulate(
    AABB_Object obj[], 
    size_t amount, 
    double dt,
    double xMax, 
    double yMax,
    double gravity
) {
    for (size_t i = 0; i<amount; i++) {
        // check for collision with wall
        if (obj[i].x < 0) {
            obj[i].dx = -obj[i].dx;
            obj[i].x = 0;
        }
        if (obj[i].x + obj[i].width > xMax) {
            obj[i].dx = -obj[i].dx;
            obj[i].x = xMax-obj[i].width;
        }
        if (obj[i].y < 0) {
            obj[i].dy = -obj[i].dy;
            obj[i].y = 0;
        }
        if (obj[i].y + obj[i].height > yMax) {
            printf("Got DY as %f and Y as %f\n",obj[i].dy,obj[i].y);
            fflush(stdout);
            // obj[i].dy -= ((obj[i].y+obj[i].height) - yMax) * gravity * dt; ?????+
            obj[i].dy = -obj[i].dy;
            obj[i].y = yMax-obj[i].height;
        }

        // check for collision another object

        // check side and simulate impulse:
        // m_1 * v_0 + m_2 * u_0 = m_1 * v + m_2 * u ;; where v_0 and u_0 are before the collision, and the other pair after.
        // m_1 * v_0^2 / 2 + m_2 * u_0^2 / 2 = m_1 * v^2 / 2 + m_2 * u^2 / 2 ;; total energy before and after are equal, as 
        //                                                                      is a this is an ideal situation.
        // collisions are only done in the x or y-axis, therefore the other component is identical whilst collisions use the
        // equations above to find the x or y component's result after the collision.

        // TODO: DO ALGEBRA FOR V AND U

        // check if they *will* collide 

        // iterate velocity per delta T (dt)
        obj[i].x += obj[i].dx * dt;
        obj[i].y += obj[i].dy * dt;
    }
} 