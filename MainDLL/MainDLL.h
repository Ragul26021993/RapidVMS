// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MAINDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MAINDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MAINDLL_EXPORTS
#define MAINDLL_API __declspec(dllexport)
#else
#define MAINDLL_API __declspec(dllimport)
#endif

#pragma warning(disable: 4251) 
#pragma warning(disable: 4715)
#pragma warning(disable: 4996)
#pragma warning(disable: 4800)
#pragma warning(disable: 4101)
#pragma warning(disable: 4804)
#pragma warning(disable: 4482)
#pragma warning(disable: 4244)
#pragma warning(disable: 4305)
#include <iostream>
using namespace std;
//to avoid STL member export warning
