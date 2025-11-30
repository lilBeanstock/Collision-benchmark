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
  double mass;
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

// yields v_0|| as per projection description, chapter 4.1.2 - 25-11-30
static double SAT_project(Vector2 v_0, Vector2 a) {
  double proj_length = Vector2DotProduct(v_0, a) / Vector2Length(a);

  return proj_length; // v_0|| = |v_0| * cos(theta) / |a|   * a
  // Vector2Scale(a, proj_length / a_length)
}

static Vector2 SAT_perpendicular(Vector2 v_ii, Vector2 v) { return Vector2Subtract(v, v_ii); }

// in the impulse calculations, a big portion of the equations can be compressed into a single value, as to make it
// easier to implement
static double SAT_DuDvMagicNumber(double v_ii0, double u_ii0, double mass_1, double mass_2) {
  double difference = v_ii0 - u_ii0;
  double massSum = mass_1 + mass_2;

  return (2.0 * difference) / massSum;
}

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

      if (HEIGHT - SAT_bottom(obj[i]) < 0) { // if its already in the ground

        // need to go backwards in time, get v_0 without knowing t. This method uses
        // v_0 = sqrt(v^2-2sa)
        double s = HEIGHT - SAT_bottom(obj[i]);
        double v_0 = sqrt(pow(obj[i].velocity.y, 2) - 2 * s * GRAVITY);

        obj[i].velocity.y = -v_0;
        obj[i].position.y = HEIGHT - SAT_height(obj[i]);
      } else {

        double v_0 = obj[i].velocity.y - (GRAVITY * dt);
        double s = HEIGHT - SAT_bottom(obj[i]);
        double t = -((v_0 - sqrt(pow(v_0, 2) + 2 * GRAVITY * s)) / GRAVITY);

        // v = v_0 + a * t
        double v = v_0 + GRAVITY * t;

        obj[i].velocity.y = -v;
        obj[i].position.y = HEIGHT - SAT_height(obj[i]);
      }
    }

    // check for collision between objects
    for (int j = i + 1; j < amount; j++) {
      SAT_Object *A = &obj[i];
      SAT_Object *B = &obj[j];
      if (!SAT_colliding(*A, *B))
        continue;

      Vector2 a = SAT_findOptimalNormal(*A, *B);

      double A_iilength = SAT_project((*A).velocity, a);
      Vector2 A_ii = Vector2Scale(a, A_iilength / Vector2Length(a));
      Vector2 A_perp = SAT_perpendicular(A_ii, (*A).velocity);

      double B_iilength = SAT_project((*B).velocity, a);
      Vector2 B_ii = Vector2Scale(a, B_iilength / Vector2Length(a));
      Vector2 B_perp = SAT_perpendicular(B_ii, (*B).velocity);

      double coefficient = SAT_DuDvMagicNumber(A_iilength, B_iilength, (*A).mass, (*B).mass);
      double new_speed_A = A_iilength - (*B).mass * coefficient;
      double new_speed_B = B_iilength + (*A).mass * coefficient;

      // v = |v|/|a| * a   ( parallel vectors )
      Vector2 A_vel_res = Vector2Scale(a, new_speed_A / Vector2Length(a));
      Vector2 B_vel_res = Vector2Scale(a, new_speed_B / Vector2Length(a));

      Vector2 A_true_res = Vector2Add(A_perp, A_vel_res);
      Vector2 B_true_res = Vector2Add(B_perp, B_vel_res);

      (*A).velocity = A_true_res;
      (*B).velocity = B_true_res;

      // move object B distance away at same angle as "a" vector in order to ensure that they are not colliding next
      // frame. Do this by finding the vertex which collided and find its distance (will point inside object A)
      double currDist = 0;
      double smallestDist = 10000;
      int vertexSide = SAT_findSide(*A, *B);
      Vector2 mid = vectorMiddle(Vector2Add((*A).vertices[vertexSide], (*A).position),
                                 Vector2Add((*A).vertices[(vertexSide + 1) % (*A).vertices_count], (*A).position));
      for (int k = 0; k < (*B).vertices_count; k++) {
        currDist = Vector2Distance(mid, Vector2Add((*B).vertices[k], (*B).position));
        if (currDist < smallestDist) {
          smallestDist = currDist;
        }
      }
      Vector2 moveoutthefuckingway = Vector2Scale(a, smallestDist / Vector2Length(a));

      // move object
      (*B).position = Vector2Add((*B).position, moveoutthefuckingway);
    }

    // ---------- Iterate velocity per delta T (dt). ----------
    obj[i].position = Vector2Add(obj[i].position, Vector2Scale(obj[i].velocity, dt));
  }
}