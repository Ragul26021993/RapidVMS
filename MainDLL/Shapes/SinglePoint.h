//Created on March 2011.. 2nd Week..
//To hold the properties of a single point.. i.e for the shapes..
#pragma once                                         
#include "..\MainDLL.h"
#define PC_ITER map<int,SinglePoint*>::iterator
class MAINDLL_API SinglePoint
{
public:
	//Variables declared..
	int PtID, PLR;
	double X, Y, Z, R, Pdir;
	bool IsSelected, InValid;
	//Constructor, Destructor..
	SinglePoint(double x = 0, double y = 0, double z = 0, double r = 0, double Probedir = 10, int side = -1);
	~SinglePoint();
	void SetValues(double x, double y, double z, double r = 0, double Probedir = 10, int side = -1);
	bool Point_IsInWindow(double minX, double minY, double maxX, double maxY);
    void Translate(double*);
    void Transform(double*);
};