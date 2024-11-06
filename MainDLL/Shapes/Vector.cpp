#include "StdAfx.h"
#include "Vector.h"
#include "..\Engine\RCadApp.h"

Vector::Vector(double x1, double y1, double z1, double r1, double p1)
{
	IsSelected = false; IntersectionPoint = false;
	set(x1, y1, z1, r1, p1);
}

Vector::Vector(Vector& v)
{
	IsSelected = false; IntersectionPoint = false;
	this->set(v);
}

Vector::~Vector(void)
{
}

void Vector::set(double x1, double y1, double z1, double r1, double p1)
{
	this->x = x1; this->y = y1; this->z = z1; this->r = r1; this->p = p1;
}

void Vector::set(Vector& source)
{
	this->x = source.getX(); this->y = source.getY(); this->z = source.getZ(); this->r = source.getR(); this->p = source.getP();
}

double Vector::getX()
{
	return x;
}

double Vector::getY()
{
	return y;
}

double Vector::getZ()
{
	try {
		return z;
	}
	catch (...)
	{
		return 0;
	}
}

double Vector::getR()
{
	return r;
}

double Vector::getP()
{
	return p;
}


void Vector::FillDoublePointer(double *PointArray, int length)
{
	PointArray[0] = x; PointArray[1] = y;
	if(length == 3)
		PointArray[2] = z;
	else if(length == 4)
	{
		PointArray[2] = z; PointArray[3] = r;
	}
	else if(length == 5)
	{
		PointArray[2] = z; PointArray[3] = r; PointArray[4] = p;
	}
}

void Vector::FillDoublePointer_Int(int *PointArray, int length)
{
	PointArray[0] = x; PointArray[1] = y;
	if(length == 3)
		PointArray[2] = z;
	else if(length == 4)
	{
		PointArray[2] = z; PointArray[3] = r;
	}
	else if(length == 5)
	{
		PointArray[2] = z; PointArray[3] = r; PointArray[4] = p;
	}
}

void Vector::operator +=(Vector &operand)
{
	x += operand.getX(); y += operand.getY();
	z += operand.getZ(); r += operand.getR(); p += operand.getP();
}

void Vector::operator -=(Vector &operand)
{
	x -= operand.getX(); y -= operand.getY();
	z -= operand.getZ(); r -= operand.getR(); p -= operand.getP();
}

void Vector::operator =(Vector& operand)
{
	this->x = operand.getX(); this->y = operand.getY();
	this->z = operand.getZ(); this->r = operand.getR(); this->p = operand.getP();
}

bool Vector::operator ==(Vector &operand)
{
	double snapDist = MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX() * SNAPDIST;
	return (abs(this->getX() - operand.getX()) < snapDist && abs(this->getY() - operand.getY()) < snapDist && abs(this->getZ() - operand.getZ()) < snapDist);
}

wostream& operator<<(wostream& os, Vector& x)
{
	os<<"x:" << x.x  << endl<<"y:" << x.y  << endl<<"z:" << x.z << endl<<"r:" << x.r << endl<<"p:" << x.p;
	return os;
}

wistream& operator>>( wistream& is, Vector& x )
{
	try
	{
		if (MAINDllOBJECT->IsOldPPFormat())
		{
			ReadOldPP(is, x);
		}
		else
		{
			std::wstring Linestr;
			for (int i = 0; i < 5; i++)
			{
				is >> Linestr;
				int ColonIndx = Linestr.find(':');
				std::wstring Tag = Linestr.substr(0, ColonIndx);
				std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
				std::string Val(TagVal.begin(), TagVal.end());
				if (Tag == L"x")
					x.x = atof((const char*)Val.c_str());
				else if (Tag == L"y")
					x.y = atof((const char*)Val.c_str());
				else if (Tag == L"z")
					x.z = atof((const char*)Val.c_str());
				else if (Tag == L"r")
					x.r = atof((const char*)Val.c_str());
				else if (Tag == L"p")
					x.p = atof((const char*)Val.c_str());
			}
		}
	}
	catch (...)
	{
	}
	return is;
}
void ReadOldPP(wistream& is, Vector& x)
{
	is >> x.x >> x.y >> x.z >> x.r >> x.p;
}