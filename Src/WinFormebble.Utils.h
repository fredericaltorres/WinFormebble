/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
 */
#include <pebble.h>
    
// Util Defines

#define StringFormat snprintf
#define AsBuffer(buffer) buffer, sizeof(buffer)
//snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "(%s, %s)", temperature_buffer, conditions_buffer);        
    
#define StringFormatTime(tick_time, format, buffer) __StringFormatTime(tick_time, format, buffer, sizeof(buffer))    
char *__StringFormatTime(struct tm * tick_time, char * format, char * buffer, int bufferSize);

#define StringFormatInt(value, format, buffer) __StringFormatInt(value, format, buffer, sizeof(buffer))
char *__StringFormatInt(int value, char * format, char * buffer, int bufferSize);

#define StringFormatString(value, format, buffer) __StringFormatString(value, format, buffer, sizeof(buffer))
char *__StringFormatString(char * value, char * format, char * buffer, int bufferSize);

/*
 * Datetime Method
 */

struct tm * DateTime_Now();

// vector.h
// http://www.happybearsoftware.com/implementing-a-dynamic-array.html
// The implement a indexed list of integer    
#define VECTOR_INITIAL_CAPACITY 100
// Define a vector type
typedef struct {
  int size;      // slots used so far
  int capacity;  // total available slots
  int *data;     // array of integers we're storing
} Vector;
void vector_init(Vector *vector);
void vector_append(Vector *vector, int value);
int vector_get(Vector *vector, int index);
void vector_set(Vector *vector, int index, int value);
void vector_double_capacity_if_full(Vector *vector);
void vector_free(Vector *vector);
