// Separating Axis Theorem.

#include <math.h>
#include <raymath.h>
#include <utils.h>

typedef struct {
  Vector2 *vertices;
  size_t vertices_count;
  Vector2 position;
  Vector2 velocity;
  Color col;
} SAT_Object;

typedef struct {
  double min;
  double max;
} AxisRange;

static double SAT_top(SAT_Object a) {
  double min = a.position.y + a.vertices[0].y;

  for (size_t i = 1; i < a.vertices_count; i++) {
    min = fmin(min, a.position.y + a.vertices[i].y);
  }

  return min;
}

static double SAT_right(SAT_Object a) {
  double max = a.position.x + a.vertices[0].x;

  for (size_t i = 1; i < a.vertices_count; i++) {
    max = fmax(max, a.position.x + a.vertices[i].x);
  }

  return max;
}

static double SAT_bottom(SAT_Object a) {
  double max = a.position.y + a.vertices[0].y;

  for (size_t i = 1; i < a.vertices_count; i++) {
    max = fmax(max, a.position.y + a.vertices[i].y);
  }

  return max;
}

static double SAT_left(SAT_Object a) {
  double min = a.position.x + a.vertices[0].x;

  for (size_t i = 1; i < a.vertices_count; i++) {
    min = fmin(min, a.position.x + a.vertices[i].x);
  }

  return min;
}

static Vector2 vectorMiddle(Vector2 a, Vector2 b) { return (Vector2){(a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f}; }

static double SAT_width(SAT_Object a) { return SAT_right(a) - a.position.x; }

static double SAT_height(SAT_Object a) { return SAT_bottom(a) - a.position.y; }

Vector2 SAT_center(SAT_Object a) {
  Vector2 sum = (Vector2){0, 0};

  for (size_t i = 0; i < a.vertices_count; i++) {
    sum = Vector2Add(sum, Vector2Add(a.vertices[i], a.position));
  }

  return Vector2Scale(sum, 1 / (double)a.vertices_count);
}

// Find the range (minimum and maximum) of the projected "shadows"
// onto a perpendicular plane of a side/edge.
static AxisRange projected_range(SAT_Object a, Vector2 normal) {
  // Take the object's whole position into account.
  double first = Vector2DotProduct(Vector2Add(a.position, a.vertices[0]), normal);
  double min = first;
  double max = first;

  for (size_t i = 1; i < a.vertices_count; i++) {
    double dot = Vector2DotProduct(Vector2Add(a.position, a.vertices[i]), normal);
    min = fmin(min, dot);
    max = fmax(max, dot);
  }

  return (AxisRange){min, max};
}

// Check if there is a gap between the two ranges.
static bool range_overlap(AxisRange a, AxisRange b) { return !(a.max < b.min || b.max < a.min); }

static bool SAT_colliding(SAT_Object a, SAT_Object b) {
  Vector2 *vertices = concatVector2Arrays(a.vertices, a.vertices_count, b.vertices, b.vertices_count);
  size_t size = a.vertices_count + b.vertices_count;

  // Loop through each edge.
  for (size_t i = 0; i < size; i++) {
    Vector2 edge = Vector2Subtract(vertices[(i + 1) % size], vertices[i]);
    // Find its normal/perpendicular axis.
    Vector2 normal = Vector2Normalize((Vector2){-edge.y, edge.x});

    // Get each normal range of shape A and B.
    // If there is a gap between the normal ranges, there is no collision, else continue searching.
    if (!range_overlap(projected_range(a, normal), projected_range(b, normal))) {
      return false;
    }
  }

  // If there is no gap, a collision is guaranteed.
  return true;
}

// find colliding side (vertex) by position of center
static int SAT_findSide(SAT_Object A, SAT_Object B) {
  // find colliding side, do it by comparing distances from the middle of each side with the vertices of the second
  // object
  double currDist = 0;
  double smallestDist = 100000;
  int contendor = 0;
  for (int i = 0; i < A.vertices_count; i++) {
    // get middle of two vertices
    Vector2 mid = vectorMiddle(Vector2Add(A.vertices[i], A.position),
                               Vector2Add(A.vertices[(i + 1) % A.vertices_count], A.position));

    // compare with each vertex
    for (int j = 0; j < B.vertices_count; j++) {
      currDist = Vector2Distance(mid, Vector2Add(B.vertices[j], B.position));
      if (currDist < smallestDist) {
        contendor = i;
        smallestDist = currDist;
      }
    }
  }

  return contendor;
  /*
  middle point (side) find using
    vectorMiddle(Vector2Add(A.vertices[contendor], A.position),
                 Vector2Add(A.vertices[(contendor + 1) % A.vertices_count], A.position));
  */
}

// find normal vector which points towards other object
static Vector2 SAT_findOptimalNormal(SAT_Object A, SAT_Object B) {
  int cont = SAT_findSide(A, B);
  // find side vector
  Vector2 side = (Vector2){A.vertices[cont].x - A.vertices[(cont + 1) % A.vertices_count].x,
                           A.vertices[cont].y - A.vertices[(cont + 1) % A.vertices_count].y};
  Vector2 sideCenter = vectorMiddle(Vector2Add(A.vertices[cont], A.position),
                                    Vector2Add(A.vertices[(cont + 1) % A.vertices_count], A.position));

  Vector2 normalA = (Vector2){-side.y, side.x};
  Vector2 normalB = (Vector2){-normalA.x, -normalA.y};

  Vector2 centrePoint = SAT_center(B);
  if (Vector2Distance(Vector2Add(normalA, sideCenter), centrePoint) <
      Vector2Distance(Vector2Add(normalB, sideCenter), centrePoint)) {
    return normalA;
  } else {
    return normalB;
  }
}

static Vector2 SAT_project() {}

void SAT_simulate(SAT_Object obj[], size_t amount, float dt) {
  // Apply gravitational acceleration first before checking for collisions.
  for (size_t i = 0; i < amount; i++) {
    obj[i].velocity.y += GRAVITY * dt;
  }

  for (size_t i = 0; i < amount; i++) {
    // ---------- Check for collision with the walls. ----------
    if (SAT_left(obj[i]) < 0) {
      obj[i].velocity.x = -obj[i].velocity.x;
      obj[i].position.x -= SAT_left(obj[i]);
    }

    if (SAT_right(obj[i]) > WIDTH) {
      obj[i].velocity.x = -obj[i].velocity.x;
      obj[i].position.x = WIDTH - SAT_width(obj[i]);
    }

    if (SAT_top(obj[i]) < 0) {
      obj[i].velocity.y = -obj[i].velocity.y;
      obj[i].position.y -= SAT_top(obj[i]);
    }
    // Check if collision with the floor is present in the next frame.
    if (SAT_bottom(obj[i]) + obj[i].velocity.y * dt > HEIGHT) {
      // Figure out the speed at the exact time when the object and floor intersect.
      // s = v_0 * t + a * t^2 / 2
      double v_0 = obj[i].velocity.y - (GRAVITY * dt);
      double s = HEIGHT - SAT_bottom(obj[i]);
      double t = -((v_0 - sqrt(pow(v_0, 2) + 2 * GRAVITY * s)) / GRAVITY);

      // v = v_0 + a * t
      double v = v_0 + GRAVITY * t;

      obj[i].velocity.y = -v;
      obj[i].position.y = HEIGHT - SAT_height(obj[i]);
    }

    // ---------- Iterate velocity per delta T (dt). ----------
    obj[i].position = Vector2Add(obj[i].position, Vector2Scale(obj[i].velocity, dt));
  }

  // printf("SAT colliding: %s\n", SAT_colliding(obj[0], obj[1]) ? "true" : "false");
  // printf("CENTER %f %f\n", SAT_center(obj[1]).x, SAT_center(obj[1]).y);
}