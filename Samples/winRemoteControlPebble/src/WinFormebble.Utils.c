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

/* ============== localDB Singleton ================== */
/*
	Allow to access the local storage on the PEBBLE Watch.
    Offer an OO syntax 
    http://developer.getpebble.com/guides/pebble-apps/app-structure/persistent-storage/
*/
LOCALDB_CLASS __localDbInstance; // Singleton instance
// Singleton implementation
bool __getBool(int key) { 
    return  persist_read_bool(key);
}
int  __getInt(int key) {
    return persist_read_int(key);
}
char*__getString(int key, char* buffer, int maxBuffer) { 
    memset(buffer,0,sizeof(buffer));
    persist_read_string(key, buffer, maxBuffer);    
    return buffer;
}
void __setBool(int key, bool val) { 
      persist_write_bool(key, val);
}
void __setInt(int key, int val) { 
    persist_write_int(key, val);
}
void __setString(int key, char* val) { 
    persist_write_string(key, val);
}
void __delete(int key) { 
    persist_delete(key);
}

LOCALDB localDB() {

	if (__localDbInstance.GetBool == NULL) {

		__localDbInstance.GetBool   = __getBool;
		__localDbInstance.GetInt    = __getInt;
		__localDbInstance.GetString = __getString;
		__localDbInstance.SetBool   = __setBool;
		__localDbInstance.SetInt    = __setInt;
		__localDbInstance.SetString = __setString;
        __localDbInstance.Delete    = __delete;
	}
	return &__localDbInstance;
}

/* ============== memoryH Singleton ================== */

 /*
    MemoryM
    A Simple memory manager for C.

    (C) Torres Frederic 2014
    MIT License

    This library was created for PEBBLE Watch development used with the library
    WinFormebble (https://github.com/fredericaltorres/WinFormebble)

 */

#if !defined(WINFORMEBBLE)
  #include <stdlib.h>
  #include <stdio.h>
  #include <assert.h>
  #include <time.h>
  #include <string.h>
  #include <stdarg.h>
  #include "darray.h"
  #include "MemoryM.h"
#endif

/*
    snprintf support for Microsoft C compiler
    http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
*/
#ifdef _MSC_VER

    #define snprintf c99_snprintf

    inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
    {
        int count = -1;

        if (size != 0)
            count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
        if (count == -1)
            count = _vscprintf(format, ap);

        return count;
    }

    inline int c99_snprintf(char* str, size_t size, const char* format, ...)
    {
        int count;
        va_list ap;

        va_start(ap, format);
        count = c99_vsnprintf(str, size, format, ap);
        va_end(ap);

        return count;
    }

#endif // _MSC_VER

// First a dynamic array of MemoryAllocation

DArray*             MemoryAllocation_New()                                              { return darray_init(); }
void                MemoryAllocation_PushA(DArray *array, MemoryAllocation *s)          { darray_push(array, s); }
MemoryAllocation*   MemoryAllocation_Pop(DArray *array)                                 { return (MemoryAllocation *)darray_pop(array); }
MemoryAllocation*   MemoryAllocation_Get(DArray *array, int index)                      { return (MemoryAllocation *)darray_get(array, index); }
void                MemoryAllocation_Set(DArray *array, int index, MemoryAllocation *s) { darray_set(array, index, s); }
void                MemoryAllocation_Destructor(DArray *array)                          { darray_free(array); }
int                 MemoryAllocation_GetLength(DArray *array)                           { return array->last; }

void MemoryAllocation_FreeAllocation(MemoryAllocation *a) {  

    if (a->data != NULL) {
        free(a->data);
        a->data = NULL;
    }
}
void MemoryAllocation_Push(DArray *array, int size, void *data) {

    MemoryAllocation* ma = (MemoryAllocation*)malloc(sizeof(MemoryAllocation));
    ma->data             = data;
    ma->size             = size;
    MemoryAllocation_PushA(array, ma);
}

// Second the MemoryManager Class

MemoryManager __localMemoryM; // Single instance allocated

int __getCount() {

    return MemoryAllocation_GetLength(__localMemoryM._memoryAllocation);
}
void* __newAllocOnly(int size) {

    void * d = malloc(size);
    memset(d, 0, size);
    return d;
}
void* __newAlloc(int size) {

    void * d = __newAllocOnly(size);
    MemoryAllocation_Push(__localMemoryM._memoryAllocation, size, d);
    return d;
}
bool* __newBool() {

    return (bool*)__newAlloc(sizeof(bool));
}
int* __newInt() {

    return (int*)__newAlloc(sizeof(int));
}
char* __newString(int size) {

    return (char*)__newAlloc(size + 1);
}
char* __string(char *s) {

    int size = strlen(s);
    char * newS = __newString(size + 1);
    strcpy(newS, s);
    return newS;
}
MemoryAllocation* __getMemoryAllocation(void* data) {

    int count = __getCount();
    for (int i = 0; i <= count; i++) {

        MemoryAllocation * ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        if (ma->data == data) {
            return ma;
        }
    }
    return NULL;
}
bool __freeAllocation(void* data) {

    MemoryAllocation* ma = __getMemoryAllocation(data);
    if (ma != NULL) {
        MemoryAllocation_FreeAllocation(ma);
    }
    return true;
}
//////////////////////////////////////////////////////////////////
/// __format
/// Format and allocate a string following the sprintf format
///     http://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm
/// 
/// For now there is a limit for the formated string of 1k.
/// Formating padding is not implemented yet.
char * __format(char *format, ...) {

    char * formated = (char*)__newAllocOnly(MEMORYM_MAX_FORMATED_TEMP_STRING_SIZE + 1);
    char tmpBuf[(MEMORYM_MAX_FORMATED_TEMP_STRING_SIZE / 2)+1];
    va_list argptr;
    va_start(argptr, format);

    while(*format != '\0') {

        if (*format == '%') {
            format++;
            if (*format == '%') { // string
                strcat(formated, "%");
            }
            else if (*format == 's') { // string
                char* s = va_arg(argptr, char *); strcat(formated, s);
            }
            else if (*format == 'c') { // character
                char c = (char)va_arg(argptr, int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%c", c); strcat(formated, tmpBuf);
            }
            else if (*format == 'd') { // integer
                int d = va_arg(argptr, int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%d", d); strcat(formated, tmpBuf);
            }
            else if (*format == 'u') { // un signed integer
                unsigned int ui = va_arg(argptr, unsigned int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%u", ui); strcat(formated, tmpBuf);
            }
            else if (*format == 'x') { // un signed integer hexa
                unsigned int ui = va_arg(argptr, unsigned int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%x", ui); strcat(formated, tmpBuf);
            }
            else if (*format == 'X') { // un signed integer hexa uppercase
                unsigned int ui = va_arg(argptr, unsigned int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%X", ui); strcat(formated, tmpBuf);
            }
            else if (*format == 'f') { // double
                double d = va_arg(argptr, double);
                snprintf(tmpBuf, sizeof(tmpBuf), "%f", d); strcat(formated, tmpBuf);
            }
            else if (*format == 'b') { // boolean not standard
                int d = va_arg(argptr, int);
                if (d)
                    strcpy(tmpBuf, MEMORYM_TRUE);
                else
                    strcpy(tmpBuf, MEMORYM_FALSE);
                strcat(formated, tmpBuf);
            }
        }
        else {
            char c = format[0];
            snprintf(tmpBuf, sizeof(tmpBuf), "%c", c);
            strcat(formated, tmpBuf);
        }
        format++;
    }
    va_end(argptr);
    // Allocate a new string for the exact size of the formated result
    char* r = __string(formated);
    free(formated);
    return r;
}
void __freeAll() {

    // Free all registered memory allocation first
    int count = __getCount();
    for (int i = 0; i <= count; i++) {
        
        MemoryAllocation_FreeAllocation(MemoryAllocation_Get(__localMemoryM._memoryAllocation, i));
    }
    // Free the MemoryAllocation dynamic array
    MemoryAllocation_Destructor(__localMemoryM._memoryAllocation);
}
char * __getReport() {

    char  buffer2[100]; // TODO: Fix this
    char* buffer = __newString(MEMORYM_MAX_REPORT_SIZE);
    int   count  = __getCount();

    //snprintf(buffer2, sizeof(buffer2), "Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());
    strcat(buffer, buffer2);

    for (int i = 0; i <= count; i++) {

        MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        snprintf(buffer2, sizeof(buffer2), "(%3d) %4d - %X\r\n", i, ma->size, (unsigned int)ma->data);
        strcat(buffer, buffer2);
    }
    return buffer;
} 
int __getMemoryUsed() {

    int total = 0;
    int count = __getCount();
    for (int i = 0; i <= count; i++) {

        MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        if (ma->data != NULL)
            total += ma->size;
    }
    return total;
}
void __Initialize() {

    __localMemoryM._memoryAllocation  = MemoryAllocation_New();
    __localMemoryM._contextStackIndex = -1;
}
//////////////////////////////////////////////////////////////////
/// __PushContext
/// 
/// Push in the stack the current state of the memory manager
bool __PushContext() {

    __localMemoryM._contextStackIndex++;

    if (__localMemoryM._contextStackIndex < MEMORYM_STACK_CONTEXT_SIZE) {
        __localMemoryM._contextStack[__localMemoryM._contextStackIndex] = __getCount();
        return true;
    }
    else {
        __localMemoryM._contextStackIndex--;
        return false;
    }
}
//////////////////////////////////////////////////////////////////
/// __PopContext
/// 
/// Restore the state of the memory manager based on the last push
bool __PopContext() {

    if (__localMemoryM._contextStackIndex > -1) {

        int numberOfAllocToPop = __getCount() - __localMemoryM._contextStack[__localMemoryM._contextStackIndex];
        __localMemoryM._contextStackIndex--;

        for(int i = 0; i < numberOfAllocToPop; i++) {
            
            MemoryAllocation* ma = MemoryAllocation_Pop(__localMemoryM._memoryAllocation); // Remove and return the allocation at the end of the array
            MemoryAllocation_FreeAllocation(ma); // Free the allocation
        }
        return true;
    }
    else 
        return false;
}

#if !defined(WINFORMEBBLE)

  void assertString(char *s1, char *s2) {
      assert(!strcmp(s1, s2));
  }

  //////////////////////////////////////////////////////////////////
  /// MemoryM.c 
  /// A simple memory manager for C.
  ///  memoryM() returns singleton object
  void __Samples() {

      bool * b1 = memoryM()->NewBool();
      int  * i1 = memoryM()->NewInt();

      // Allocate string
      char * s1 = memoryM()->NewString(10);    
      char * s2 = memoryM()->String("Hello World");
      
      // Format and allocate string
      char * s3 = memoryM()->Format("b:%b, b:%b", true, false);
      char * s4 = memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128);
      char * s5 = memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes");
      char * s6 = memoryM()->Format("c:%c, c:%c", 'A', 'z');
      char * s7 = memoryM()->Format("%d%%", 1);
      char * s8 = memoryM()->String("Hello World");
      memoryM()->FreeAllocation(s8); // Just free an allocation

      // Push/Pop memory context and free all allocation after previous Push
      memoryM()->PushContext();

          char * s22   = memoryM()->NewString(100);
          char* report = memoryM()->GetReport(); // Get allocation report
          printf(report);

      memoryM()->PopContext(); // Force to free all allocated since previous push

      memoryM()->FreeAll();
  }

  //////////////////////////////////////////////////////////////////
  /// __UnitTests
  bool __UnitTests() {
      
      // Verify bool allocation
      bool * b1 = memoryM()->NewBool();
      bool * b2 = memoryM()->NewBool();
      assert(2 == memoryM()->GetMemoryUsed());

      // Verify int allocation
      int  * i1 = memoryM()->NewInt();
      int  * i2 = memoryM()->NewInt();
      assert(10 == memoryM()->GetMemoryUsed());

      // Verify string allocation
      char * s1 = memoryM()->NewString(10);
      char * s2 = memoryM()->NewString(100);
      assert(10+11+101 == memoryM()->GetMemoryUsed());

      // Verify allocation of a string with a static string
      char * helloWorld = "Hello World";
      char * s3         = memoryM()->String(helloWorld);
      assertString(helloWorld, s3);
      assert(10 + 11 + 101 + 13 == memoryM()->GetMemoryUsed());
      
      // Verify Format()
      assertString("b:true, b:false",          memoryM()->Format("b:%b, b:%b", true, false));
      assertString("n:128, u:128, x:80, X:80", memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128));
      assertString("s:ok les filles, a:Yes",   memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes"));
      assertString("c:A, c:z",                 memoryM()->Format("c:%c, c:%c", 'A','z'));
      assertString("1%",                       memoryM()->Format("%d%%", 1));
      
      // Verify that FreeAllocation() free the memory
      int a1     = memoryM()->GetMemoryUsed();
      char * s5  = memoryM()->String(helloWorld);
      int a2     = memoryM()->GetMemoryUsed();
                   memoryM()->FreeAllocation(s5);
      int a3     = memoryM()->GetMemoryUsed();
      assert(a1 == a3);

      memoryM()->PushContext();

      int v1 = memoryM()->GetCount();
      int m1 = memoryM()->GetMemoryUsed();

      char * s22 = memoryM()->NewString(100);
      char* report = memoryM()->GetReport();
      printf(report);
      printf("Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());

      memoryM()->PopContext(); // Force to free all allocated since previous push

      printf("Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());

      int v2 = memoryM()->GetCount();
      int m2 = memoryM()->GetMemoryUsed();

      assert(v1 == v2);
      assert(m1 == m2);

      memoryM()->FreeAll();

      return true;
  }

#endif

MemoryManager * memoryM() {

    if (__localMemoryM.NewBool == NULL) {

        __localMemoryM.NewBool        = __newBool;
        __localMemoryM.NewInt         = __newInt;
        __localMemoryM.NewString      = __newString;
        __localMemoryM.FreeAll        = __freeAll;
        __localMemoryM.GetCount       = __getCount;
        __localMemoryM.String         = __string;
        __localMemoryM.Format         = __format;
        __localMemoryM.GetReport      = __getReport;
        __localMemoryM.GetMemoryUsed  = __getMemoryUsed;
        __localMemoryM.FreeAllocation = __freeAllocation;
        __localMemoryM.PushContext    = __PushContext;
        __localMemoryM.PopContext     = __PopContext;

        #if !defined(WINFORMEBBLE)        
          __localMemoryM.UnitTests    = __UnitTests;
        #endif

        __Initialize();
    }
    return &__localMemoryM;
}
