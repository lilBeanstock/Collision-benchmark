#include <raymath.h>

// Flatten two Vector2 arrays into one array.
Vector2 *concatVector2Arrays(const Vector2 *a, size_t len_a, const Vector2 *b, size_t len_b) {
  size_t len_c = len_a + len_b;
  Vector2 *c = malloc(len_c * sizeof(Vector2));

  if (!c) {
    // Allocation failed.
    return NULL;
  }

  // Copy elements of a.
  for (size_t i = 0; i < len_a; i++) {
    c[i] = a[i];
  }
  // Copy elements of b.
  for (size_t j = 0; j < len_b; j++) {
    c[len_a + j] = b[j];
  }

  return c;
}
