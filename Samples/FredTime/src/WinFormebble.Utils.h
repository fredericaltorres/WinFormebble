/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
 */
#include <pebble.h>
    
#define WINFORMEBBLE 1

#define private    
    
// Util Defines


/// REMOVE THIS 
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

//struct tm * DateTime_Now();
//struct tm * DateTime(int Year, int Month, int Day, int Hour, int Minutes, int Seconds);
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


/* ============== localDB Singleton ================== */
/*
    Allow to access the local storage on the PEBBLE Watch.
    Offer an OO syntax
    http://developer.getpebble.com/guides/pebble-apps/app-structure/persistent-storage/
*/
    
typedef struct  {

	void(*SetBool)  (int key, bool val);
	void(*SetInt)   (int key, int val);
	void(*SetString)(int key, char* val);
    void(*Delete)   (int key);

	bool (*GetBool)  (int key);
	int  (*GetInt)   (int key);
	char*(*GetString)(int key, char* buffer, int maxBuffer);    

} LOCALDB_CLASS;

#define LOCALDB LOCALDB_CLASS*
    
LOCALDB localDB();


/* ============== Watch Singleton ================== */
/*
http://developer.getpebble.com/docs/c/group___watch_info.html#struct_watch_info_version  
*/
     
typedef struct  {
    
    char*(*GetColor)();
    char*(*GetFirmwareVersion)();    

} WATCH_CLASS;

#define WATCH WATCH_CLASS*
    
WATCH watch();


/* ============== MemoryM ================== */

/*
MemoryM
A Simple memory manager for C.

(C) Torres Frederic 2014
MIT License

This library was created for PEBBLE Watch development used with the library
WinFormebble (https://github.com/fredericaltorres/WinFormebble)

*/
#ifndef _MEMORYM_H_
#define _MEMORYM_H_

#if !defined(WINFORMEBBLE)

    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <time.h> 
    #include <string.h>
    #include "darray.h"

#endif

#define MEMORYM_MAX_REPORT_SIZE 1024
#define MEMORYM_TRUE "true"
#define MEMORYM_FALSE "false"
#define MEMORYM_STACK_CONTEXT_SIZE 4

    /* ============== MemoryM  ==================

    A memory manager for C
    */

    // First implement a dynamic array to store all allocation
    typedef struct {

        int size;
        void * data;
    } MemoryAllocation;

    DArray*             MemoryAllocation_New       ();
    void                MemoryAllocation_PushA     (DArray *array, MemoryAllocation *s);
    void                MemoryAllocation_Push      (DArray *array, int size, void *data);
    MemoryAllocation*   MemoryAllocation_Pop       (DArray *array);
    MemoryAllocation*   MemoryAllocation_Get       (DArray *array, int index);
    void                MemoryAllocation_Set       (DArray *array, int index, MemoryAllocation *s);
    void                MemoryAllocation_Destructor(DArray *array);
    int                 MemoryAllocation_GetLength (DArray *array);

    void MemoryAllocation_FreeAllocation(MemoryAllocation *a);

    typedef struct {

        DArray*	_memoryAllocation;
        int _contextStack[MEMORYM_STACK_CONTEXT_SIZE];
        int _contextStackIndex;

        // Allocate a new boolean
        bool*(*NewBool)();
        // Allocate a new int
        int *(*NewInt)();
        // Allocate a new string for len size (do not add the extra char for the \0)
        char*(*NewStringLen)(int size);
        // Allocate a new string identical to the string passed
        char*(*NewString)(char* s);
        // Re allocate a new string identical to the string passed, but re use the internal MemoryAllocation object
        char*(*ReNewString)(char* s, char* previousAllocation);
        // Concat the string s to the string previousAllocation already managed by MemoryM 
        char*(*StringConcat)(char* s, char* previousAllocation);
        
        // Allocate a new DateTime set to now
        struct tm *(*NewDate)();
        // Allocate a new DateTime set to now
        struct tm *(*ReNewDate)(struct tm * previousAllocation);
        // Allocate a new DateTime set to a specific date
        struct tm *(*NewDateTime)(int year, int month, int day, int hour, int minutes, int seconds);

        // Format using sprintf, but return a string allocated by MemoryM
        char*(*Format)(char* s, ...);
        // Format the Date using strftime(), but return a string allocated by MemoryM
        char*(*FormatDateTime)(struct tm *date, char* format);
        // Re allocate and re format the Date using strftime(), but re use the internal MemoryAllocation object
        char*(*ReFormatDateTime)(struct tm *date, char* format, char * previousAllocation);

        // Free a specific allocation
        bool(*Free)(void* data);
        // Free multiple specific allocation
        int(*FreeMultiple)(int n, ...);

        // Return a string allocated by MemoryM, presenting the current memory allocation
        char*(*GetReport)();
        // Return how many total byte are allocated
        int  (*GetMemoryUsed)();
        // Free all
        void (*FreeAll)();
        // Return the total number of allocation created
        int  (*GetCount)();
        
        // Mark the state of the memory manager
        bool(*PushContext)();
        // Restore the state of the memory manager to the previous Push
        bool(*PopContext)();

        bool(*UnitTests)();

    } MemoryManager;

    // Function that return the sigleton instance
    MemoryManager* memoryM(); 

    #endif