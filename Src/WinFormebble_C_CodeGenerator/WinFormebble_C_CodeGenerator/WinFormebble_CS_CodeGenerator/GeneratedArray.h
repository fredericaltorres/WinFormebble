
	/*
		WinFormebble
		Generated Array Classes
	*/ 


	/**
	 * MemoryAllocation Header File
	 * Array Class: MemoryAllocation
	 * Generated Code based on the darray.c library 
	 */
			 
	typedef struct {
		int type;
		int data;
	} MemoryAllocation;

	DArray*      MemoryAllocation_New       ();
	void         MemoryAllocation_Push      (DArray *array, MemoryAllocation *s);
	MemoryAllocation*  MemoryAllocation_Pop  (DArray *array);
	MemoryAllocation*  MemoryAllocation_Get  (DArray *array, int index);
	void         MemoryAllocation_Set       (DArray *array, int index, MemoryAllocation *s);
	void         MemoryAllocation_Destructor(DArray *array);
	int          MemoryAllocation_GetLength (DArray *array);

		
