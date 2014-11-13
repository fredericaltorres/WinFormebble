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
