#pragma once
//#include "..\BestFit\LinesAndArcs.h"
#include "..\BestFit\RBF.h"

template<class T> bool compareboth(T item1, T item2, T value1, T value2)
{
	if((item1 == value1 && item2 == value2) || (item1 == value2 && item2 == value1))
		return true;
	return false;
}

template<class T> class RapidProperty
{
private: T value;
public:
	RapidProperty(){}
	~RapidProperty(){}
	void operator()(T tt){value = tt;}
	T operator()(){return value;}
};

template<class T> class RapidToggleProperty
{
private: T value;
public:
	RapidToggleProperty(){}
	~RapidToggleProperty(){}
	void operator()(T tt){value = tt;}
	T operator()(){return value;}
	void Toggle(){value = !value;}
};

struct RColor
{
	double r, g, b;
	RColor(double rv = 1, double gv = 1, double bv = 1)
	{ set(rv, gv, bv); }
	void set(double r, double g, double b)
	{ this->r = r; this->g = g; this->b = b; }
	~RColor(){ }
};

struct SurfaceED_Params
{
	int MaskFactor, Binary_Lower, Binary_Upper, MaskingLevel, Threshold;
	bool FilterByAverage = true;
	SurfaceED_Params(){	}
	SurfaceED_Params(int MF, int BL, int BU, int ML, int Th, bool DoAverageFilter)
	{
		MaskFactor = MF; MaskingLevel = ML;
		Binary_Lower = BL; Binary_Upper = BU;
		Threshold = Th;
		FilterByAverage = DoAverageFilter;
	}
	~SurfaceED_Params(){	}
};

//BOOL Is64BitWindows()
//{
//#if defined(_WIN64)
//	return TRUE;  // 64-bit programs run only on Win64
//#elif defined(_WIN32)
//	// 32-bit programs run on both 32-bit and 64-bit Windows
//	// so must sniff
//	BOOL f64 = FALSE;
//	return IsWow64Process(GetCurrentProcess(), &f64) && f64;
//#else
//	return FALSE; // Win64 does not support Win16
//#endif
//}