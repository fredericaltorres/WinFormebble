
// Util Defines

// Format a char array charBuffer. charBuffer must be already allocated
#define StringFormat(charBuffer, format, value) snprintf(charBuffer, sizeof(charBuffer), format, value)
    
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
