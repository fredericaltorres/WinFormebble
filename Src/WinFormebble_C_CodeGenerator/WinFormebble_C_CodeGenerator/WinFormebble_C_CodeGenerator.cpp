
#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h> 
#include <string.h>

#include "darray.h"
#include "darray_unittests.h"

/* ============== localDB Singleton ================== */
/*
Allow to access the local storage on the PEBBLE Watch.
Offer an OO syntax
*/
typedef struct  {
	void(*SetBool)  (int key, bool val);
	void(*SetInt)   (int key, int val);
	void(*SetString)(int key, char* val);

	bool (*GetBool)  (int key);
	int  (*GetInt)  (int key);
	char*(*GetString)  (int key);
} LOCALDB_CLASS;

#define LOCALDB LOCALDB_CLASS*

LOCALDB localDB();



LOCALDB_CLASS __localDbInstance;

bool __getBool  (int key)            { return true; }
int  __getInt   (int key)            { return 122; }
char*__getString(int key)            { return "aaa"; }
void __setBool  (int key, bool val)  { }
void __setInt   (int key, int val)   { }
void __setString(int key, char* val) { }

LOCALDB localDB() {

	if (__localDbInstance.GetBool == NULL) {

		__localDbInstance.GetBool   = __getBool;
		__localDbInstance.GetInt    = __getInt;
		__localDbInstance.GetString = __getString;
		__localDbInstance.SetBool   = __setBool;
		__localDbInstance.SetInt    = __setInt;
		__localDbInstance.SetString = __setString;
	}
	return &__localDbInstance;
}

int _tmain(int argc, _TCHAR* argv[])
{
	localDB()->SetBool(0, true);
	localDB()->SetInt(0, 112);
	localDB()->SetString(0, "aa");
	localDB()->GetBool(0);
	localDB()->GetInt(0);
	localDB()->GetString(0);

	printf("running");
	//test_darray_all();
	return 0;
}

