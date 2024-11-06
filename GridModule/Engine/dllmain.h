#ifdef GRIDMODULE_EXPORTS
#define GRIDMODULE_API __declspec(dllexport)
#else
#define GRIDMODULE_API __declspec(dllimport)
#endif

#pragma warning(disable: 4251) 
#pragma warning(disable: 4244)
#pragma warning(disable: 4996)