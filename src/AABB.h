// Axis-Aligned Bounding Box.

#include <common.h>
#include <math.h>
#include <raymath.h>

typedef struct {
  double x;
  double y;
  // The width is the radius if the shape is a circle.
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

// Circle's equation: (x - a)^2 + (y - b)^2 = r^2.
// If LHS < RHS => inside, LHS = RHS => on the circumference, LHS > RHS => outside.
static bool pointInsideCircle(Vector2 point, AABB_Object circle) {
  Vector2 circleCenter = center(circle);

  // We want a collision detected if the point has contact with the circle, hence "<=" and not "<".
  return pow(point.x - circleCenter.x, 2) + pow(point.y - circleCenter.y, 2) <= pow(circle.width, 2);
}

static bool AABB_colliding(AABB_Object a, AABB_Object b) {
  if (!a.isCircle && !b.isCircle) {
    // Rectangle-rectangle collision.
    return (left(a) < right(b) && right(a) > left(b) && top(a) < bottom(b) && bottom(a) > top(b));
  } else if (a.isCircle && b.isCircle) {
    // Circle-circle collision.
    Vector2 aCenter = center(a);
    Vector2 bCenter = center(b);

    double dx = aCenter.x - bCenter.x;
    double dy = aCenter.y - bCenter.y;
    double distance = sqrt(pow(dx, 2) + pow(dy, 2));

    return distance <= a.width + b.width;
  } else {
    // Circle-rectangle collision.
    AABB_Object circle = a.isCircle ? a : b;
    AABB_Object rectangle = a.isCircle ? b : a;

    // Technically this is axis-aligned since we cannot rotate
    // the rectangle with its current implementation/type definition.
    return pointInsideRectangle(circle, rectangle) ||
           pointInsideCircle((Vector2){left(rectangle), top(rectangle)}, circle) ||
           pointInsideCircle((Vector2){right(rectangle), top(rectangle)}, circle) ||
           pointInsideCircle((Vector2){right(rectangle), bottom(rectangle)}, circle) ||
           pointInsideCircle((Vector2){left(rectangle), bottom(rectangle)}, circle);
  }
}

// Find which side object a collides with (is closest to) object b.
// a must be a rectangle, but not b.
static Side rectangle_side(AABB_Object a, AABB_Object b) {
  // Compare side gaps with regard to object a.
  double rightgap = fabs(right(a) - left(b));
  double leftgap = fabs(left(a) - right(b));
  double bottomgap = fabs(bottom(a) - top(b));
  double topgap = fabs(top(a) - bottom(b));

  // printf("A: (%d)\n%.3f %.3f %.3f %.3f\n", a.col.g, topgap, rightgap, bottomgap, leftgap);

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

// Get the change in velocity of object A & B after a collision.
static Vector2 getDV_DU(AABB_Object A, AABB_Object B, bool yDirection) {
  Vector2 v0_u0 = yDirection ? (Vector2){A.dy, B.dy} : (Vector2){A.dx, B.dx};

  // The other axis velocity stays constant at a 90 | 0 degree collision.
  float vMinU = v0_u0.x - v0_u0.y;
  float massSum = A.mass + B.mass;

  // v = -(2 * m_2 * (v_0 - u_0)) / (m_1 + m_2) + v_0
  // u = (2 * m_1 * (v_0 - u_0)) / (m_1 + m_2) + u_0
  return Vector2Scale((Vector2){(-2.0F * B.mass * vMinU), (2.0F * A.mass * vMinU)}, 1 / massSum);
}

void AABB_simulate(AABB_Object obj[], size_t objSize, float dt) {
  // Apply gravitational acceleration first before checking for collisions.
  for (size_t i = 0; i < objSize; i++) {
    obj[i].dy += GRAVITY * dt;
  }

  for (size_t i = 0; i < objSize; i++) {
    // ---------- Check for collision with the walls. ----------
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

    // Check if collision with the floor is present in the next frame.
    if (bottom(obj[i]) + obj[i].dy * dt > HEIGHT) {
      // Figure out the speed at the exact time when the object and floor intersect.
      // s = v_0 * t + a * t^2 / 2
      double v_0 = obj[i].dy - (GRAVITY * dt);
      double s = HEIGHT - bottom(obj[i]);
      double t = -((v_0 - sqrt(pow(v_0, 2) + 2 * GRAVITY * s)) / GRAVITY);

      // v = v_0 + a * t
      double v = v_0 + GRAVITY * t;

      obj[i].dy = -v;
      obj[i].y = HEIGHT - height(obj[i]);
    }

    // TODO: make this work correctly with circle and circle-rectangle collisions.
    // ---------- Check for collision with another object. ----------
    // When colliding, they will bounce according to the law of conservation of momentum.
    // Due to limitations in AABB collision detection and what-not, every object will
    // bounce in the x- or y-axis, never at an angle. Therefore, the collision will be done
    // according to their shapes, but with the calculations according to a rectangle.
    for (size_t j = 0; j < objSize; j++) {
      // Check if they are colliding.
      if (!AABB_colliding(obj[i], obj[j]) || i == j)
        continue;

      // Get the axis of bounce in regards to obj[i].
      Side axis = rectangle_side(obj[i], obj[j]);

      // ensure that they don't give a false positive
      bool falsePositive = (axis == Top && obj[j].dy < obj[i].dy) || (axis == Bottom && obj[j].dy > obj[i].dy) ||
                           (axis == Left && obj[j].dx < obj[i].dx) || (axis == Right && obj[j].dx > obj[i].dx);

      if (falsePositive)
        break;

      if (!obj[i].isCircle && !obj[j].isCircle) {
        // When hit on the y-axis, dy is changed and dx is constant.
        if (axis == Top || axis == Bottom) {
          Vector2 dy = getDV_DU(obj[i], obj[j], true);

          // printf("VERTICAL: (%d)\n%.3f %.3f\n", obj[i].col.g, dy.x, dy.y);

          obj[i].dy += dy.x;
          obj[j].dy += dy.y;

          // Move out of each other.
          if (axis == Top) {
            obj[i].y += fabs(top(obj[i]) - bottom(obj[j]));
          } else {
            obj[i].y -= fabs(bottom(obj[i]) - top(obj[j]));
          }
        } else {
          // If not on the y-axis, then on the x-axis.
          Vector2 dx = getDV_DU(obj[i], obj[j], false);

          // printf("HORIZ: (%d)\n%.3f %.3f\n", obj[i].col.g, dx.x, dx.y);

          obj[i].dx += dx.x;
          obj[j].dx += dx.y;

          // Move out of each other.
          if (axis == Right) {
            obj[i].x -= fabs(right(obj[i]) - left(obj[j]));
          } else {
            obj[i].x += fabs(left(obj[i]) - right(obj[j]));
          }
        }
      } else if (obj[i].isCircle && obj[j].isCircle) {
        // TODO: x-direction is incorrect on vertical bounces.
        Vector2 dx = getDV_DU(obj[i], obj[j], false);
        Vector2 dy = getDV_DU(obj[i], obj[j], true);

        obj[i].dx += dx.x;
        obj[j].dx += dx.y;
        obj[i].dy += dy.x;
        obj[j].dy += dy.y;

        // The code below simulates circle collisions really well,
        // however the derivation of the physics is not stated nor directly trivial.
        // Dynamic Circle-Circle Collision: https://ericleong.me/research/circle-circle/

        // double distance = sqrt(pow(obj[i].x - obj[j].x, 2) + pow(obj[i].y - obj[j].y, 2));
        // Vector2 norm = Vector2Scale((Vector2){obj[j].x - obj[i].x, obj[j].y - obj[i].y}, 1 / distance);
        // double p = 2 * (obj[i].dx * norm.x + obj[i].dy * norm.y - obj[j].dx * norm.x - obj[j].dy * norm.y) /
        //            (obj[i].mass + obj[j].mass);

        // obj[i].dx -= p * obj[i].mass * norm.x;
        // obj[i].dy -= p * obj[i].mass * norm.y;
        // obj[j].dx += p * obj[j].mass * norm.x;
        // obj[j].dy += p * obj[j].mass * norm.y;
      }
    }

    // ---------- Check if they *will* collide. ----------

    // ---------- Iterate velocity per delta T (dt). ----------
    obj[i].x += obj[i].dx * dt;
    obj[i].y += obj[i].dy * dt;
  }
}