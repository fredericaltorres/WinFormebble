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
