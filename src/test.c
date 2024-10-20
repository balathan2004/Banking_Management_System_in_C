#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>

void main() {
  cJSON *json = cJSON_CreateObject();

  FILE *file = fopen("index.json", "w");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  cJSON_AddStringToObject(json, "name", "John Doe");
  cJSON_AddNumberToObject(json, "age", 30);
  cJSON_AddStringToObject(json, "city", "New York");

  char *jsonString = cJSON_Print(json);

  fprintf(file, "%s\n", jsonString);
}