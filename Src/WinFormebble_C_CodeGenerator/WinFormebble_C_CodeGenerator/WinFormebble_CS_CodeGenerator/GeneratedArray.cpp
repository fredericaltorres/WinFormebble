
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "darray.h"
#include "GeneratedArray.h"

   

	/**
	 * TestStruct Clode File
	 * Array Class: TestStruct
	 * Generated Code based on the darray.c library 
	 */

	DArray*      TestStruct_New           ()                                             { return darray_init();                               }
	void         TestStruct_Push          (DArray *array, TestStruct *s)            { darray_push(array, s);                              }
	TestStruct* TestStruct_Pop       (DArray *array)                                { return (TestStruct *)darray_pop(array);        }        
	TestStruct* TestStruct_Get       (DArray *array, int index)                     { return (TestStruct *)darray_get(array, index); } 
	void         TestStruct_Set           (DArray *array, int index, TestStruct *s)  { darray_set(array, index, s);                        }
	void         TestStruct_Destructor    (DArray *array)                                { darray_free(array);                                 }
	int          ControlInfo_GetLength         (DArray *array)                                { return array->last; }
            
   

	/**
	 * ControlInfo Clode File
	 * Array Class: ControlInfo
	 * Generated Code based on the darray.c library 
	 */

	DArray*      ControlInfo_New           ()                                             { return darray_init();                               }
	void         ControlInfo_Push          (DArray *array, ControlInfo *s)            { darray_push(array, s);                              }
	ControlInfo* ControlInfo_Pop       (DArray *array)                                { return (ControlInfo *)darray_pop(array);        }        
	ControlInfo* ControlInfo_Get       (DArray *array, int index)                     { return (ControlInfo *)darray_get(array, index); } 
	void         ControlInfo_Set           (DArray *array, int index, ControlInfo *s)  { darray_set(array, index, s);                        }
	void         ControlInfo_Destructor    (DArray *array)                                { darray_free(array);                                 }
	int          ControlInfo_GetLength         (DArray *array)                                { return array->last; }
            

