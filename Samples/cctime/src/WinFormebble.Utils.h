/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
 */
#include <pebble.h>
    
#define private    
    
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
struct tm * DateTime(int Year, int Month, int Day, int Hour, int Minutes, int Seconds);
time_t DateTime_ToTimeT(struct tm * dateTime);
int DateTime_Diff(char unit, struct tm * dateTime1, struct tm * dateTime2);

/* ============== PDPCLIB ================== */
/*
  This code is derived from PDPCLIB, the public domain C runtime
  library by Paul Edwards. http://pdos.sourceforge.net/
  This code is released to the public domain.
*/

time_t p_mktime(struct tm *tmptr);
char *p_strtok(char *s1, const char *s2);
long int p_strtol(const char *nptr, char **endptr, int base);

/* ============== DARRAY ================== */
/*
	darray - Dynamic array for C based on darray.h from https://gist.github.com/dce/5187025
	https://gist.github.com/dce
	David Eisinger
*/

typedef struct {
	void **data;
	int last;
	int size;
} DArray;

#define PDArray DArray*
#define PDatatArray void**

DArray* darray_init      ();
void    darray_resize    (DArray *array, int size);
void*   darray_get       (DArray *array, int index);
void    darray_free      (DArray *array);
void    darray_set       (DArray *array, int index, void *value);
void    darray_push      (DArray *array, void *value);
void*   darray_pop       (DArray *array);
DArray* darray_radix_sort(DArray *array);


