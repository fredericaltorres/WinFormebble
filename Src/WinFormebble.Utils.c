/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
 */
#include <pebble.h>
#include "WinFormebble.Utils.h"
#include <stdarg.h>

// vector.c
// http://www.happybearsoftware.com/implementing-a-dynamic-array.html
#include <stdio.h>
#include <stdlib.h>
//#include "vector.h"
void vector_init(Vector *vector) {
  vector->size = 0;
  vector->capacity = VECTOR_INITIAL_CAPACITY;
  vector->data = malloc(sizeof(int) * vector->capacity);
}
void vector_append(Vector *vector, int value) {
  vector_double_capacity_if_full(vector);
  vector->data[vector->size++] = value;
}
int vector_get(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    return -32767; // Not really good
  }
  return vector->data[index];
}
void vector_set(Vector *vector, int index, int value) {  
  while (index >= vector->size) {
    vector_append(vector, 0);
  }
  vector->data[index] = value;
}
void vector_double_capacity_if_full(Vector *vector) {
  if (vector->size >= vector->capacity) {
    vector->capacity *= 2;
    vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
  }
}
void vector_free(Vector *vector) {
  free(vector->data);
}

/*
char *StringFormat2(char * format, char * buffer, int bufferSize, ...) {
     va_list ap;
    va_start(ap, n args);
    snprintf(buffer, bufferSize, format, ap); 
    va_end(ap);
    return buffer;
   
}*/

/*
 * String Format Methods
 */

char * __StringFormatInt(int value, char * format, char * buffer, int bufferSize) {
        
    snprintf(buffer, bufferSize, format, value);
    return buffer;
}
char * __StringFormatString(char * value, char * format, char * buffer, int bufferSize) {
    
    snprintf(buffer, bufferSize, format, value);
    return buffer;
}
/*
 * http://www.cplusplus.com/reference/ctime/strftime/
 */
char *__StringFormatTime(struct tm *tick_time, char * format, char * buffer, int bufferSize) {
    
    strftime(buffer, bufferSize, format, tick_time); // http://www.cplusplus.com/reference/ctime/strftime/
    return buffer;
}