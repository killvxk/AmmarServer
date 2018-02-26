#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "26";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.02";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 34;
	static const long BUILD  = 813;
	static const long REVISION  = 4098;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 976;
	#define RC_FILEVERSION 0,34,813,4098
	#define RC_FILEVERSION_STRING "0, 34, 813, 4098\0"
	static const char FULLVERSION_STRING [] = "0.34.813.4098";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 78;
	

#endif //VERSION_H
