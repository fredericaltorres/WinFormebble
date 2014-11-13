/*
darray - Dynamic array for C based on darray.h from https://gist.github.com/dce/5187025 - David Eisinger
Modified by Frederic Torres 2014
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "darray.h"

#include "D:\DVT\Watch.Dev\WinFormebble\Src\WinFormebble_C_CodeGenerator\WinFormebble_C_CodeGenerator\WinFormebble_CS_CodeGenerator\GeneratedArray.h"

void test_darray_init()
{
	DArray *array = darray_init();

	assert(array->last == -1);
	assert(array->size == 16);

	darray_free(array);
}

void test_darray_get_set()
{
	DArray *array = darray_init();

	int *i;
	i = (int*)malloc(sizeof(int));
	*i = 1500;

	darray_set(array, 0, i);

	assert(array->last == 0);
	assert(array->size == 16);
	assert(*(int *)darray_get(array, 0) == 1500);

	darray_free(array);
}

/*
typedef struct {
	int type;
	int data;
} TestStruct;

DArray *     TestStruct_New       ()                                        { return darray_init();                          }
void         TestStruct_Push      (DArray *array, TestStruct *s)            { darray_push(array, s);                         }
TestStruct * TestStruct_Pop       (DArray *array)                           { return (TestStruct *)darray_pop(array);        }        
TestStruct * TestStruct_Get       (DArray *array, int index)                { return (TestStruct *)darray_get(array, index); } 
void         TestStruct_Set       (DArray *array, int index, TestStruct *s) { darray_set(array, index, s);                   }
void         TestStruct_Destructor(DArray *array)                           { darray_free(array);                            }
*/

TestStruct * TestStruct_NewInstance(int type, int data) {

	TestStruct*i;
	i = (TestStruct*)malloc(sizeof(TestStruct));
	i->data = data;
	i->type = type;
	return i;
}

void test_darray_PushPopStruct()
{
	DArray *array = TestStruct_New();
	TestStruct *s;
	int j;

	for (j = 0; j < 16; j++) {
		TestStruct_Push(array, TestStruct_NewInstance(j, j * 2));
	}

	assert(array->last == 15);
	assert(array->size == 16);

	for (j = 0; j < 16; j++) {
		s = TestStruct_Get(array, j);
		assert(s->type == j);
		assert(s->data == j*2);
	}

	int z = 15;
	for (j = 0; j < 15; j++) {
		s = TestStruct_Pop(array);
		assert(s->type == z);
		assert(s->data == z * 2);
		free(s); // Popped element must be freed
		z--;
	}
	assert(array->last == 0);

	s = TestStruct_Pop(array);
	assert(s->type == z);
	assert(s->data == z * 2);

	assert(array->last == -1);

	TestStruct_Destructor(array);
}


int * GetNewInt(int v) {
	int *i;
	i = (int*)malloc(sizeof(int));
	*i = v;
	return i;
}




void test_darray_resize()
{
	DArray *array = darray_init();

	darray_set(array, 0, GetNewInt(1500));
	darray_set(array, 1, GetNewInt(1500));
	darray_set(array, 50, GetNewInt(1500));

	assert(array->last == 50);
	assert(array->size == 64);
	assert(*(int *)darray_get(array, 0) == 1500);
	assert(*(int *)darray_get(array, 1) == 1500);
	assert(darray_get(array, 2) == NULL);
	assert(*(int *)darray_get(array, 50) == 1500);

	darray_set(array, 100, GetNewInt(1600));

	assert(array->last == 100);
	assert(array->size == 128);
	assert(*(int *)darray_get(array, 50) == 1500);
	assert(*(int *)darray_get(array, 100) == 1600);

	darray_free(array);
}

void test_darray_push_pop()
{
	DArray *array = darray_init();
	int *i, j;

	for (j = 0; j < 16; j++) {
		i = (int*)malloc(sizeof(int));
		*i = j;
		darray_push(array, i);
	}

	assert(array->last == 15);
	assert(array->size == 16);
	assert(*(int *)darray_get(array, 0) == 0);
	assert(*(int *)darray_get(array, 15) == 15);

	i = (int*)malloc(sizeof(int));
	*i = 16;
	darray_push(array, i);

	assert(array->last == 16);
	assert(array->size == 32);
	assert(*(int *)darray_get(array, 0) == 0);
	assert(*(int *)darray_get(array, 15) == 15);
	assert(*(int *)darray_get(array, 16) == 16);

	assert(*(int *)darray_pop(array) == 16);
	assert(darray_get(array, 16) == NULL);
	assert(array->last == 15);
	assert(array->size == 32);

	darray_free(array);
}


void test_darray_radix_sort()
{
	srand(time(NULL));

	DArray *array;
	int *i, j, v1, v2;

	array = darray_init();

	for (j = 0; j < 10; j++) {

		i = (int*)malloc(sizeof(int));
		*i = rand() % 1000;
		darray_push(array, i);
	}

	darray_radix_sort(array);

	for (j = 0; j < 9; j++) {
		v1 = *(int *)darray_get(array, j);
		v2 = *(int *)darray_get(array, j + 1);
		assert(v1 <= v2);
	}

	darray_free(array);
}

void test_darray_all() {

	test_darray_PushPopStruct();
	test_darray_init();
	test_darray_get_set();
	test_darray_resize();
	test_darray_push_pop();
	test_darray_radix_sort();
}
