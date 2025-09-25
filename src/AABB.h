// Axis-Aligned Bounding Box.

#include <common.h>
#include <math.h>
#include <raymath.h>

typedef struct {
  double x;
  double y;
  double width;
  double height;
  double dx;
  double dy;
  float mass;
  Color col;
  bool isCircle;
} AABB_Object;

typedef enum { Top = 0, Right, Bottom, Left } Side;

static double top(AABB_Object a) { return a.y; }

static double right(AABB_Object a) { return a.x + a.width * (a.isCircle ? 2 : 1); }

static double bottom(AABB_Object a) { return a.y + (a.isCircle ? a.width * 2 : a.height); }

static double left(AABB_Object a) { return a.x; }

static double width(AABB_Object a) { return a.isCircle ? a.width * 2 : a.width; }

static double height(AABB_Object a) { return a.isCircle ? a.width * 2 : a.height; }

static Vector2 center(AABB_Object a) {
  return a.isCircle ? (Vector2){a.x + a.width, a.y + a.width} : (Vector2){a.x + a.width / 2, a.y + a.height / 2};
}

static bool pointInsideRectangle(AABB_Object circle, AABB_Object rectangle) {
  Vector2 point = center(circle);

  return point.x > left(rectangle) && point.x < right(rectangle) && point.y > top(rectangle) &&
         point.y < bottom(rectangle);
}

// (x - a)^2 + (y - b)^ = r^2
static bool pointInsideCircle(Vector2 point, AABB_Object circle) {
  Vector2 circleCenter = center(circle);

  return pow(point.x - circleCenter.x, 2) + pow(point.y - circleCenter.y, 2) < pow(circle.width, 2);
}

static bool AABB_colliding(AABB_Object a, AABB_Object b) {
  if (!a.isCircle && !b.isCircle) {
    // Rectangle-rectangle collision.
    return (left(a) < right(b) && right(a) > left(b) && top(a) < bottom(b) && bottom(a) > top(b));
  } else if (a.isCircle && b.isCircle) {
    // Circle-circle collision.
    double dx = a.x + b.width - (b.x + b.width);
    double dy = a.y + b.width - (b.y + b.width);
    double distance = sqrt(pow(dx, 2) + pow(dy, 2));

    return distance < a.width + b.width;
  } else {
    // Circle-rectangle collision.
    AABB_Object circle = a.isCircle ? a : b;
    AABB_Object rectangle = a.isCircle ? b : a;

    return pointInsideRectangle(circle, rectangle) ||
           pointInsideCircle((Vector2){left(rectangle), top(rectangle)}, circle) ||
           pointInsideCircle((Vector2){right(rectangle), top(rectangle)}, circle) ||
           pointInsideCircle((Vector2){right(rectangle), bottom(rectangle)}, circle) ||
           pointInsideCircle((Vector2){left(rectangle), bottom(rectangle)}, circle);
  }
}

// returns which side on object a collides with (is closest to) object b. a must be a rectangle, b does not
static Side rectangle_side(AABB_Object a, AABB_Object b) {
  // Compare side gaps with regard to object a.
  double rightgap = fabs(right(a) - left(b));
  double leftgap = fabs(left(a) - right(b));
  double bottomgap = fabs(bottom(a) - top(b));
  double topgap = fabs(top(a) - bottom(b));

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

// get the change in velocity of object A after a collision
static float getDV(AABB_Object A, AABB_Object B, Side axis) {
  float v0 = A.dx;
  float u0 = B.dx;
  
  if (axis == Top || axis == Bottom) {
    v0 = A.dy;
    u0 = B.dy;
  }

  // the other axis velocity stays constant at a 90 | 0 degree collision
  float vMinU = v0 - u0;
  float massSum = A.mass + B.mass;

  // v_0 - (2 * m_2 * (v_0 - u_0))/(m_1+m_2) = v
  float quotient = (-2.0F * B.mass * vMinU) / massSum;
  return quotient;
}

// get the change in velocity of object B after a collision
static float getDU(AABB_Object A, AABB_Object B, Side axis) {
  float v0 = A.dx;
  float u0 = B.dx;
  
  if (axis == Top || axis == Bottom) {
    v0 = A.dy;
    u0 = B.dy;
  }

  // the other axis velocity stays constant at a 90 | 0 degree collision
  float vMinU = v0 - u0;
  float massSum = A.mass + B.mass;

  // (2 * m_1 * (v_0 - u_0))/(m_1+m_2) + u_0 = u
  float quotient = (2.0F * A.mass * vMinU) / massSum;
  return quotient;
}

void AABB_simulate(AABB_Object obj[], size_t objSize, float dt) {
  for (size_t i = 0; i < objSize; i++) {
    obj[i].dy += GRAVITY * dt;
  }

  for (size_t i = 0; i < objSize; i++) {
    // -------------------- Check for collision with the walls. ----------------------------
    if (left(obj[i]) < 0) {
      obj[i].dx = -obj[i].dx;
      obj[i].x = 0;
    }
    if (right(obj[i]) > WIDTH) {
      obj[i].dx = -obj[i].dx;
      obj[i].x = WIDTH - width(obj[i]);
    }
    if (top(obj[i]) < 0) {
      obj[i].dy = -obj[i].dy;
      obj[i].y = 0;
    }
    
    // ------------- Check for collision with another object. --------------------
    // when colliding, they will bounce according to the law of conservation of momentum.
    // because of limitations in AABB collision detection and what-not, every object will
    // bounce in the x or y axis, never at an angle. Therefore, the collision will be done
    // according to shape, but the calculations according to a rectangle.
    for (size_t j = 0; j<objSize; j++) {
      // check if colliding
      if (!AABB_colliding(obj[i],obj[j]) || i == j) continue;
      
      printf("Colliding? %s\n", (AABB_colliding(obj[i],obj[j])) ? "yes!" : "no!");
      fflush(stdout);

      // get the axis of bounce in regards to obj[i]
      Side axis = rectangle_side(obj[i],obj[j]);
      if (axis == Top || axis == Bottom) { // when hit on the y-axis, dy is changed and dx is constant
        printf("%d - %f %f - %f %f\n",axis, obj[i].y,obj[j].y,getDV(obj[i],obj[j],axis),getDU(obj[i],obj[j],axis));
        float dyi = getDV(obj[i],obj[j],axis);
        float dyj = getDU(obj[i],obj[j],axis);
        obj[i].dy += dyi;
        obj[j].dy += dyj;

        // move out of each other
        if (axis == Top) {
          obj[i].y += fabs(top(obj[i]) - bottom(obj[j]));
        } else {
          obj[i].y -= fabs(bottom(obj[i]) - top(obj[j]));
        }

      } else { // if not on y-axis, then on x-axis
        printf("%d - %f %f - %f %f\n",axis, obj[i].x,obj[j].x,getDV(obj[i],obj[j],axis),getDU(obj[i],obj[j],axis));
        float dxi = getDV(obj[i],obj[j],axis);
        float dxj = getDU(obj[i],obj[j],axis);
        obj[i].dx += dxi;
        obj[j].dx += dxj;

        // move out of each other
        if (axis == Right) {
          obj[i].x -= fabs(right(obj[i]) - left(obj[j]));
        } else {
          obj[i].x += fabs(left(obj[i]) - right(obj[j]));
        }
      }
    }

    // ------------- Check if collision with the floor is present in the next frame. ------------------
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


    // --------------- Check if they *will* collide. ------------------------

    // --------------- Iterate velocity per delta T (dt). -------------------
    obj[i].x += obj[i].dx * dt;
    obj[i].y += obj[i].dy * dt;
  }
}