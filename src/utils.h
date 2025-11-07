#include <cJSON.h>
#include <common.h>
#include <math.h>
#include <raymath.h>
#include <stdio.h>

#pragma once

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

// https://github.com/DaveGamble/cJSON?tab=readme-ov-file#printing
// NOTE: Returns a heap allocated string, you are required to free it after use.
char *dataToJSON(JSONData data, size_t pointCount) {
  char *string = NULL;
  cJSON *points = NULL;
  cJSON *jsonFile = cJSON_CreateObject();

  if (cJSON_AddNumberToObject(jsonFile, "object_count", data.object_count) == NULL) {
    goto end;
  }

  points = cJSON_AddArrayToObject(jsonFile, "points");

  if (points == NULL) {
    goto end;
  }

  for (size_t i = 0; i < pointCount; i++) {
    cJSON *point = cJSON_CreateObject();

    if (cJSON_AddNumberToObject(point, "time", data.points[i].time / 1000000) == NULL) {
      goto end;
    }

    if (cJSON_AddNumberToObject(point, "fps", data.points[i].fps) == NULL) {
      goto end;
    }

    cJSON_AddItemToArray(points, point);
  }

  string = cJSON_Print(jsonFile);

  if (string == NULL) {
    fprintf(stderr, "Failed to print JSON object.\n");
  }

end:
  cJSON_Delete(jsonFile);
  return string;
}