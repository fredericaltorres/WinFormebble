
	/*
		WinFormebble
		Generated Array Classes
	*/ 


	/**
	 * TestStruct Header File
	 * Array Class: TestStruct
	 * Generated Code based on the darray.c library
	 */
			 
	typedef struct {
		int type;
		int data;
	} TestStruct;

	DArray*      TestStruct_New       ();
	void         TestStruct_Push      (DArray *array, TestStruct *s);
	TestStruct*  TestStruct_Pop  (DArray *array);
	TestStruct*  TestStruct_Get  (DArray *array, int index);
	void         TestStruct_Set       (DArray *array, int index, TestStruct *s);
	void         TestStruct_Destructor(DArray *array);
	int          TestStruct_GetLength (DArray *array);


	/**
	 * ControlInfo Header File
	 * Array Class: ControlInfo
	 * Generated Code based on the darray.c library
	 */
			 
	typedef struct {
		int type;
		int data;
	} ControlInfo;

	DArray*      ControlInfo_New       ();
	void         ControlInfo_Push      (DArray *array, ControlInfo *s);
	ControlInfo*  ControlInfo_Pop  (DArray *array);
	ControlInfo*  ControlInfo_Get  (DArray *array, int index);
	void         ControlInfo_Set       (DArray *array, int index, ControlInfo *s);
	void         ControlInfo_Destructor(DArray *array);
	int          ControlInfo_GetLength (DArray *array);

		
