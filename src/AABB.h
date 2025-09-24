// Axis-Aligned Bounding Box.

#include <common.h>
#include <math.h>

typedef struct {
  double x;
  double y;
  double width;
  double height;
  double dx;
  double dy;
  bool isCircle;
} AABB_Object;

typedef enum { Top = 0, Right, Bottom, Left } Side;

static double top(AABB_Object a) { return a.y; }

static double right(AABB_Object a) { return a.x + a.width * (a.isCircle ? 2 : 1); }

static double bottom(AABB_Object a) { return a.y + (a.isCircle ? a.width * 2 : a.height); }

static double left(AABB_Object a) { return a.x; }

static double height(AABB_Object a) { return a.isCircle ? a.width * 2 : a.height; }

static bool AABB_colliding(AABB_Object a, AABB_Object b) {
  if (!a.isCircle && !b.isCircle) {
    // Rectangle-rectangle collision.
    return (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y);
  } else if (a.isCircle && b.isCircle) {
    // Circle-circle collision.
    double dx = a.x + b.width - (b.x + b.width);
    double dy = a.y + b.width - (b.y + b.width);
    double distance = sqrt(pow(dx, 2) + pow(dy, 2));

    return distance < a.width + b.width;
  } else {
    // TODO: Circle-rectangle collision.
  }
}

static Side rectangle_side(AABB_Object a, AABB_Object b) {
  // Compare side gaps with regard to object a.
  double rightgap = fabs((a.x + a.width) - b.x);
  double leftgap = fabs(a.x - (b.x + b.width));
  double bottomgap = fabs((a.y + a.height) - b.y);
  double topgap = fabs(a.y - (b.y + b.height));

  // Check which one is the smallest, with topgap as the default.
  Side lowestGapSide = Top;
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

void AABB_simulate(AABB_Object obj[], size_t objSize, float dt) {
  for (size_t i = 0; i < objSize; i++) {
    obj[i].dy += GRAVITY * dt;
  }

  for (size_t i = 0; i < objSize; i++) {
    // Check for collision with the walls.
    if (obj[i].x < 0) {
      obj[i].dx = -obj[i].dx;
      obj[i].x = 0;
    }
    if (right(obj[i]) > WIDTH) {
      obj[i].dx = -obj[i].dx;
      obj[i].x = WIDTH - obj[i].width * (obj[i].isCircle ? 2 : 1);
    }
    if (obj[i].y < 0) {
      obj[i].dy = -obj[i].dy;
      obj[i].y = 0;
    }

    // Check if collision with the floor is present in the next frame.
    if (bottom(obj[i]) + obj[i].dy * dt > HEIGHT) {
      // Figure out the speed at the exact time when the object and floor intersect.
      // s = v_0*t + a*t^2/2
      double v_0 = obj[i].dy - (GRAVITY * dt);
      double s = HEIGHT - bottom(obj[i]);
      double t = -((v_0 - sqrt(pow(v_0, 2) + 2 * GRAVITY * s)) / GRAVITY);

      // v = v_0 + a*t
      double v = v_0 + GRAVITY * t;

      obj[i].dy = -v;
      obj[i].y = HEIGHT - height(obj[i]);
    }

    // Check for collision with another object.
    // Check if they *will* collide.

    // Iterate velocity per delta T (dt).
    obj[i].x += obj[i].dx * dt;
    obj[i].y += obj[i].dy * dt;
  }
}