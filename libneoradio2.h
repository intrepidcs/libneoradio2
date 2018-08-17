// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBNEORADIO2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBNEORADIO2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBNEORADIO2_EXPORTS
#define LIBNEORADIO2_API __declspec(dllexport)
#else
#define LIBNEORADIO2_API __declspec(dllimport)
#endif

// This class is exported from the libneoradio2.dll
class LIBNEORADIO2_API Clibneoradio2 {
public:
	Clibneoradio2(void);
	// TODO: add your methods here.
};

extern LIBNEORADIO2_API int nlibneoradio2;

LIBNEORADIO2_API int fnlibneoradio2(void);
