// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MAINDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MAINDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HARDWARECOMMUNICATION_EXPORTS
#define DRO_API __declspec(dllexport)
#else
#define DRO_API __declspec(dllimport)
#endif

#pragma warning(disable: 4251) // replace xxxx with warning number
#pragma warning(disable: 4482)
#pragma warning(disable: 4018)
#pragma warning(disable: 4996)
//to avoid STL member export warning