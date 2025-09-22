// Separating Axis Theorem.

#include <helpers.h>
#include <math.h>
#include <raymath.h>

typedef struct {
  Vector2 *vertices;
  size_t vertices_count;
  Vector2 position;
  Vector2 velocity;
} SAT_object;

typedef struct {
  double min;
  double max;
} AxisRange;

// Find the range (minimum and maximum) of the projected "shadows"
// onto a perpendicular plane of a side/edge.
static AxisRange projected_range(SAT_object a, Vector2 normal) {
  // Take the object's whole position into account.
  double first = Vector2DotProduct(Vector2Add(a.position, a.vertices[0]), normal);
  double min = first;
  double max = first;

  for (size_t i = 0; i < a.vertices_count; i++) {
    double dot = Vector2DotProduct(Vector2Add(a.position, a.vertices[i]), normal);
    min = fmin(min, dot);
    max = fmax(max, dot);
  }

  return (AxisRange){min, max};
}

// Check if there is a gap between the two ranges.
static bool range_overlap(AxisRange a, AxisRange b) { return !(a.max < b.min || b.max < a.min); }

static bool SAT_colliding(SAT_object a, SAT_object b) {
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

void SAT_simulate(SAT_object objects[], size_t amount, float dt) {
  printf("SAT colliding: %s\n", SAT_colliding(objects[0], objects[1]) ? "true" : "false");
}