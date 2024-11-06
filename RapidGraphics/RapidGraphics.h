#ifdef RAPIDGRAPHICS_EXPORTS
#define RapidGraphics_API __declspec(dllexport)
#else
#define RapidGraphics_API __declspec(dllimport)
#endif

//Basic OpenGl and Windows Header files..//
#include <windows.h>
#include "gl/gl.h"
#include "gl/glu.h"
#include "stdafx.h"

//Disable type casting warnings..
#pragma warning(disable: 4251) 
#pragma warning(disable: 4244) 