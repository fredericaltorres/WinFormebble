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
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>

/* ============== PDPCLIB ================== */
/*
  This code is derived from PDPCLIB, the public domain C runtime
  library by Paul Edwards. http://pdos.sourceforge.net/

  This code is released to the public domain.
*/
//#include <pebble.h>
//#include <ctype.h>

/* scalar date routines    --    public domain by Ray Gardner
** These will work over the range 1-01-01 thru 14699-12-31
** The functions written by Ray are isleap, months_to_days,
** years_to_days, ymd_to_scalar, scalar_to_ymd.
** modified slightly by Paul Edwards
*/

static int isleap (unsigned yr) {
  return yr % 400 == 0 || (yr % 4 == 0 && yr % 100 != 0);
}

static unsigned months_to_days (unsigned month) {
  return (month * 3057 - 3007) / 100;
}

static unsigned years_to_days (unsigned yr) {
  return yr * 365L + yr / 4 - yr / 100 + yr / 400;
}

static long ymd_to_scalar (unsigned yr, unsigned mo, unsigned day) {
  long scalar;

  scalar = day + months_to_days(mo);
  if (mo > 2) /* adjust if past February */
    scalar -= isleap(yr) ? 1 : 2;
  yr--;
  scalar += years_to_days(yr);
  return scalar;
}

static void scalar_to_ymd (long scalar,
                           unsigned *pyr,
                           unsigned *pmo,
                           unsigned *pday) {
  unsigned n; // compute inverse of years_to_days()

  n = (unsigned)((scalar * 400L) / 146097L);
  while ((long)years_to_days(n) < scalar) {
    n++;
  }
  for ( n = (unsigned)((scalar * 400L) / 146097L); (long)years_to_days(n) < scalar; )
    n++; // 146097 == years_to_days(400)
  *pyr = n;
  n = (unsigned)(scalar - years_to_days(n-1));
  if ( n > 59 ) { // adjust if past February
    n += 2;
    if ( isleap(*pyr) )
      n -= n > 61 ? 1 : 2;
  }
  *pmo = (n * 100 + 3007) / 3057; // inverse of months_to_days()
  *pday = n - months_to_days(*pmo);
  return;
}

time_t p_mktime (struct tm *timeptr) {
  time_t tt;

  if ((timeptr->tm_year < 70) || (timeptr->tm_year > 120)) {
    tt = (time_t)-1;
  } else {
    tt = ymd_to_scalar(timeptr->tm_year + 1900,
                       timeptr->tm_mon + 1,
                       timeptr->tm_mday)
      - ymd_to_scalar(1970, 1, 1);
    tt = tt * 24 + timeptr->tm_hour;
    tt = tt * 60 + timeptr->tm_min;
    tt = tt * 60 + timeptr->tm_sec;
  }
  return tt;
}

char *p_strtok(char *s1, const char *s2) {
  static char *old = NULL;
  char *p;
  size_t len;
  size_t remain;

  if (s1 != NULL) old = s1;
  if (old == NULL) return (NULL);
  p = old;
  len = strspn(p, s2);
  remain = strlen(p);
  if (remain <= len) { old = NULL; return (NULL); }
  p += len;
  len = strcspn(p, s2);
  remain = strlen(p);
  if (remain <= len) { old = NULL; return (p); }
  *(p + len) = '\0';
  old = p + len + 1;
  return(p);
}

unsigned long int strtoul(const char *nptr, char **endptr, int base) {
  unsigned long x = 0;
  int undecided = 0;

  if (base == 0) {
    undecided = 1;
  }
  while (isspace((unsigned char)*nptr)) {
    nptr++;
  }
  while (1) {
    if (isdigit((unsigned char)*nptr)) {
      if (base == 0) {
  if (*nptr == '0') {
    base = 8;
  } else {
    base = 10;
    undecided = 0;
  }
      }
      x = x * base + (*nptr - '0');
      nptr++;
    } else if (isalpha((unsigned char)*nptr)) {
      if ((*nptr == 'X') || (*nptr == 'x')) {
  if ((base == 0) || ((base == 8) && undecided)) {
    base = 16;
    undecided = 0;
    nptr++;
  } else if (base == 16) {
    /* hex values are allowed to have an optional 0x */
    nptr++;
  } else {
    break;
  }
      } else if (base <= 10) {
  break;
      } else {
  x = x * base + (toupper((unsigned char)*nptr) - 'A') + 10;
  nptr++;
      }
    } else {
      break;
    }
  }
  if (endptr != NULL) {
    *endptr = (char *)nptr;
  }
  return (x);
}

long int strtol(const char *nptr, char **endptr, int base) {
  unsigned long y;
  long x;
  int neg = 0;

  while (isspace((unsigned char)*nptr)) {
    nptr++;
  }
  if (*nptr == '-') {
    neg = 1;
    nptr++;
  } else if (*nptr == '+') {
    nptr++;
  }
  y = strtoul(nptr, endptr, base);
  if (neg) {
    x = (long)-y;
  } else {
    x = (long)y;
  }
  return (x);
}
/* ============== PDPCLIB ================== */

/* ============== WinFormebble.Untils ================== */

/*
char *StringFormat2(char * format, char * buffer, int bufferSize, ...) {
     va_list ap;
    va_start(ap, n args);
    snprintf(buffer, bufferSize, format, ap); 
    va_end(ap);
    return buffer;
   
}*/


/*
 * Datetime Method
 * http://www.epochconverter.com/
 * To do: Create a DateTime define for struct tm *
 * and a DateTime_Destructor(***) and all DateTime_New should do a
 * malloc
 */

private struct tm * DateTime_Now_NonReEntrant() {
    
    time_t temp             = time(NULL);  // Get a tm structure
    struct tm *tick_time    = localtime(&temp);
    return tick_time;
}    

struct tm * DateTime_Now() {
    
    static struct tm DateTime_tick_time; // TODO: Not ReEntrant
    memcpy(&DateTime_tick_time, DateTime_Now_NonReEntrant(), sizeof(struct tm));
    return &DateTime_tick_time;
}

struct tm * DateTime(int year, int month, int day, int hour, int minutes, int seconds) {
    
    static struct tm DateTime_tick_time; // Not ReEntrant
    memcpy(&DateTime_tick_time, DateTime_Now_NonReEntrant(), sizeof(struct tm));
     
    DateTime_tick_time.tm_sec  = seconds;
    DateTime_tick_time.tm_min  = minutes;
    DateTime_tick_time.tm_hour = hour;
    DateTime_tick_time.tm_min  = minutes;
    DateTime_tick_time.tm_mday = day;
    DateTime_tick_time.tm_mon  = month - 1;
    DateTime_tick_time.tm_year = year - 1900;
    //tick_time.tm_wday = ?;
    //tick_time.tm_yday = ?;
    DateTime_tick_time.tm_isdst = 0;
    
    return &DateTime_tick_time;
}

time_t DateTime_ToTimeT(struct tm * dateTime) {
    
    return p_mktime(dateTime);
}

int DateTime_Diff(char unit, struct tm * dateTime1, struct tm * dateTime2) {
    
    time_t second1 = DateTime_ToTimeT(dateTime1);
    time_t second2 = DateTime_ToTimeT(dateTime2);
    int difference;
  
    switch(unit) {
        
        case 's' : difference = second2 - second1;                       break; // Seconds
        case 'm' : difference = (second2 - second1) / 60;                break; // Minutes
        case 'h' : difference = ((second2 - second1) / 60) / 60;         break; // Hour
        case 'd' : difference = (((second2 - second1) / 60) / 60) / 24;  break; // Days
        default  : return -1; break;
    }
    if(difference < 0) {
        difference = 0;
    }
    return difference;
}

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

/* ============== DARRAY ================== */
/*
  darray - Dynamic array for C based on darray.h from https://gist.github.com/dce/5187025 - David Eisinger
  Modified by Frederic Torres 2014
*/

DArray * darray_init() {

  DArray *array;
  array = (PDArray)malloc(sizeof(DArray));
  array->last = -1;
  array->size = 16;
  array->data = (PDatatArray)calloc(array->size, sizeof(void *));
  
  for (int i = 0; i < array->size; i++) {
    array->data[i] = NULL;
  }
  return array;
}

void darray_resize(DArray *array, int size) {

  array->data = (PDatatArray)realloc(array->data, size * sizeof(void *));
  array->size = size;

  for (int i = array->last+1; i < array->size; i++) {
    array->data[i] = NULL;
  }
}

void * darray_get(DArray *array, int index) {

  return array->data[index];
}

void darray_free(DArray *array) {

  int i = array->last;

  while (i >= 0) {

    void * ptr = darray_get(array, i);
    if (ptr != NULL) {
      free(ptr);
    }
    i--;
  }
  free(array);
}

void darray_set(DArray *array, int index, void *value) {

  int new_size = array->size;

  while (index > (new_size - 1)) {
    new_size *= 2;
  }

  if (new_size != array->size) {
    darray_resize(array, new_size);
  }

  array->data[index] = value;

  if (index > array->last) {
    array->last = index;
  }
}

void darray_push(DArray *array, void *value) {

  array->last++;

  if (array->last == array->size) {
    darray_resize(array, array->size * 2);
  }
  array->data[array->last] = value;
}

void * darray_pop(DArray *array) {

  void *value;
  value = darray_get(array, array->last);
  darray_set(array, array->last, NULL);
  array->last--;
  return value;
}

DArray * darray_radix_sort(DArray *array) {

  DArray *buckets, *bucket;
  int *val, i, j, cur, mask, sortval, sorted;

  buckets = darray_init();

  mask = 1;

  do {
    sorted = 1;

    // reset the buckets
    for (i = 0; i < 10; i++) {
      darray_set(buckets, i, darray_init());
    }

    // sort the values into buckets
    for (i = 0; i <= (array->last); i++) {
      val = (int*)darray_get(array, i);
      sortval = (*val / mask) % 10;
      if (sortval > 0) { sorted = 0; }
      darray_push((PDArray)darray_get(buckets, sortval), val);
    }

    // rebuild array
    cur = 0;

    for (i = 0; i < 10; i++) {
      bucket = (PDArray)darray_get(buckets, i);

      for (j = 0; j <= (bucket->last); j++) {
        darray_set(array,
          cur++,
          darray_get(bucket, j));
      }
    }

    mask *= 10;

  } while (!sorted);

  return array;
}
