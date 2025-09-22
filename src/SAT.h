// Separating Axis Theorem.

#include <math.h>
#include <raymath.h>

typedef struct {
  Vector2 *vertices;
  size_t vertices_count;
  Vector2 pos;
  double dx;
  double dy;
} SAT_object;

typedef struct {
  double min;
  double max;
} AxisRange;

static AxisRange range(Vector2 vertices[], size_t size, Vector2 distanceOffset) {
  double min = 0;
  double max = 0;

  // Loop through all sides of the object.
  for (size_t i = 1; i < size; i++) {
    Vector2 axis;

    // Find the perpendicular axis to the side.
    axis.x = (-vertices[i].y - vertices[i - 1].y);
    axis.y = vertices[i].x - vertices[i - 1].x;

    // Normalise the axis (side length = 1).
    axis = Vector2Normalize(axis);
    double magnitude = Vector2Length(axis);

    if (magnitude != 0) {
      axis.x *= 1 / magnitude;
      axis.y *= 1 / magnitude;
    }

    // Find the range of the shadow projection onto the axis.
    for (size_t j = 0; j < size; j++) {
      double dot = Vector2DotProduct(axis, vertices[j]);
      min = fmin(min, dot);
      max = fmin(max, dot);
    }

    // Take the distance between the objects into consideration
    // by adding the distance onto the shadow axis projection.
    double scalarOffset = Vector2DotProduct(axis, distanceOffset);
    min += scalarOffset;
    max += scalarOffset;
  }

  return (AxisRange){min, max};
}

// CURRENTLY INCORRECT!
static bool SAT_colliding(SAT_object a, SAT_object b) {
  Vector2 distanceOffset = (Vector2){a.pos.x - b.pos.x, a.pos.y - b.pos.y};

  AxisRange aRange = range(a.vertices, a.vertices_count, distanceOffset);
  AxisRange bRange = range(b.vertices, b.vertices_count, distanceOffset);

  // return (aRange.min - bRange.max > 0) || (bRange.min - aRange.max > 0) == true ? false : true;
  return (aRange.min - bRange.max < 0) && (bRange.min - aRange.max < 0);
}

void SAT_simulate(SAT_object objects[], size_t amount, float dt) {
  printf("SAT colliding: %s\n", SAT_colliding(objects[0], objects[1]) ? "true" : "false");
}