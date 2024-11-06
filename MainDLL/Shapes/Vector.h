#pragma once
#include "..\MainDLL.h"
using namespace std;
class MAINDLL_API Vector
{	
private:
	double x, y, z, r, p;
public:
	bool IntersectionPoint;
	bool IsSelected;
	Vector(double x1 = 0, double y1 = 0, double z1 = 0, double r1 = 0, double p1 = 0);
	Vector(Vector& vector);
	~Vector(void);

	//initialise functions
	void set(double x1, double y1, double z1 = 0, double r1 = 0, double p1 = 0);
	void set(Vector& source);
	double getX();
	double getY();
	double getZ();
	double getR();
	double getP();

	void FillDoublePointer(double *PointArray, int length = 2);
	void FillDoublePointer_Int(int *PointArray, int length = 2);
	
	//Operator Overload
	void operator += (Vector& operand);
	void operator -= (Vector& operand);
	void operator = (Vector& operand);
	bool operator == (Vector& operand);
	
	friend wostream& operator<<(wostream& os, Vector& x);
	friend wistream& operator>>(wistream& is, Vector& x); 
	friend void ReadOldPP(wistream& is, Vector& x);
};