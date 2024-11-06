// RapidMath3D.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "RapidMath3D.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "stdlib.h"
#include <string>

#define PRECISION 0.00001
#define ANGLEPRECISION 0.001

//RapidMath2D *Rmath2D = new RapidMath2D;


///////3D functions////////07/01/11
/*
Following structure has been assumed for the double array in all the functions:
point = {x,y,z}
Line = {x,y,z,l,m,n}
plane = {l,m,n,p}
sphere = {x,y,z,r}
cylinder = {x,y,z,l,m,n,r}
*/


RapidMath3D::RapidMath3D()
{
	PrecisionCt = 5;
	RMath3DError = NULL;
}

RapidMath3D::~RapidMath3D()
{
}

void RapidMath3D::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(RMath3DError != NULL)
		RMath3DError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}


//------------------------------------------------------------------------
//Distance functions
//------------------------------------------------------------------------

//To calculate direction cosines from the direction ratios
bool RapidMath3D::DirectionCosines(double *DirRatios, double *DirCosines, bool FourthParameter)
{
	try
	{
		/*
		when the direction ratios are given by: a, b, c
		then direction cosines l, m, n are defined by:

					   a						  b							  c
		l =  ---------------------, m =  ---------------------, n =  ---------------------
			 sqrt(a^2 + b^2 + c^2)		 sqrt(a^2 + b^2 + c^2)		 sqrt(a^2 + b^2 + c^2)

		*/

		double a = 0, b = 0, c = 0, d = 0;
		a = DirRatios[0];
		b = DirRatios[1];
		c = DirRatios[2];
		//Calculate the dinominator value
		double term = sqrt(pow(a,2) + pow(b,2) + pow(c,2));
	
		//if its 0, then we can not calculate cosines
		if(abs(term) < PRECISION)
			return false;

		//Calculate l, m, n
		DirCosines[0] = a / term;
		DirCosines[1] = b / term;
		DirCosines[2] = c / term;

		//Now if a 4th parameter is also present, divide it by dinominator
		//This 4th parameter comes in handy when a plane is concerned
		if(FourthParameter)
		{
			d = DirRatios[3];
			DirCosines[3] = d / term;
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0001", __FILE__, __FUNCSIG__); return false;}
}

//point to point distance
double RapidMath3D::Distance_Point_Point(double *Point1, double *Point2)
{
	try
	{
		//This is a straight forward calculation that every 10th pass guy will know
		double Dist = sqrt(pow(Point1[0] - Point2[0],2) + pow(Point1[1] - Point2[1],2) + pow(Point1[2] - Point2[2],2));
		if(Dist < PRECISION) Dist = 0;
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0002", __FILE__, __FUNCSIG__); return 0; }
}

//Point to line distance
double RapidMath3D::Distance_Point_Line(double* Point, double* Line)
{
	try
	{
		/*
		Let the point be x1, y1, z1
		Let the point on the line be x2, y2, z2
		And the direction cosines of the line be l, m, n
		then:

		val1 = (x1 - x2)^2 + (y1 - y2)^2 + (z1 - z2)^2
		val2 = l(x1 - x2) + m(y1 - y2) + n(z1 - z2)

		shortest distance = sqrt(val1 - val2^2)
		*/  
		double dist = 0, tD = 0;

		//Calculating val1
		for (int i = 0; i < 3; i ++)
			dist += pow(Point[i] - Line[i], 2);

		//Calculating val2
		for (int i = 0; i < 3; i ++)
			tD += Line[3 + i] * (Point[i] - Line[i]);
	
		dist = sqrt(abs(dist - tD * tD));
		if(dist < PRECISION) dist = 0;
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0003", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate distance of a point from plane where the plane equation is in normal form///
double RapidMath3D::Distance_Point_Plane(double *point, double *plane)
{
	try
	{
		//equation calculates the distance of the point from the plane//
		double dist = 0;
		//double dis2 = 0;
		for(int i = 0; i < 3; i++)
		{
			dist = dist + (*(point + i))*(*(plane + i));
		}
		dist = abs(dist - *(plane + 3));
		if(dist < PRECISION) dist = 0;
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0004", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::Distance_Point_PlaneAlongSeletedAxis(double *point, double *plane, int selectedAxis)
{
	try
	{
		//equation calculates the distance of the point from the plane//
		double dist = 0, tmpZ = 0;
		if(plane[2] == 0) return -1;
		tmpZ = (-1 * (point[0] * plane[0] + point[1] * plane[1] - plane[3])) / plane[2];
	  /*  for(int i = 0; i < 3; i++)
		{
			pointLine[i] = point[i];
			if(i == selectedAxis)
				pointLine[3 + i] = 1;
		}
		if(Intersection_Line_Plane(pointLine, plane, intersectionPnt) == 0) return -1;*/
		dist = point[2] - tmpZ;
		if(abs(dist) < PRECISION) dist = 0;
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0004", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the distance of a pt from plane with sign..i.e pt above plane is +ve dist, pt below plane plane is -ve dist
double RapidMath3D::Distance_Point_Plane_with_Dir(double *point, double *plane)
{
	try
	{
		//equation calculates the distance of the point from the plane//
		double dist = 0;
		//double dis2 = 0;
		for(int i = 0; i < 3; i++)
		{
			dist = dist + (*(point + i))*(*(plane + i));
		}
		dist = dist - *(plane + 3);
		if(abs(dist) < PRECISION) dist = 0;
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0005", __FILE__, __FUNCSIG__); return 0; }
}

//Point to sphere distance with 3 measurement modes
double RapidMath3D::Distance_Point_Sphere(double *Point, double *Sphere, int MeasureType)
{
	try
	{
		double dist = Distance_Point_Point(Point, Sphere);
		if(MeasureType == 0)
			return abs(dist - Sphere[3]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Sphere[3];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0006", __FILE__, __FUNCSIG__); return 0; }
}

//Point to sphere distance with sign..i.e pt within sphere is -ve dist and pt outside sphere is +ve dist
double RapidMath3D::Distance_Point_Sphere_with_Dir(double *Point, double *Sphere)
{
	return Distance_Point_Point(Point, Sphere) - Sphere[3];
}

double RapidMath3D::Distance_Point_Cone(double *Point, double *Cone, double *Sf1, double *Sf2, int MeasureType)
{
	try
	{
		if(MeasureType == 0)
			return abs(Distance_Point_Line(Point, Sf1));
		else if(MeasureType == 1)
			return abs(Distance_Point_Line(Point, Cone));
		else
			return abs(Distance_Point_Line(Point, Sf2));
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0007", __FILE__, __FUNCSIG__); return 0; }
}

//Point to Cylinder distance with 3 measurement modes
double RapidMath3D::Distance_Point_Cylinder(double *Point, double *Cylinder, int MeasureType)
{
	try
	{
		double dist = Distance_Point_Line(Point, Cylinder);
		if(MeasureType == 0)
			return abs(dist - Cylinder[6]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Cylinder[6];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0007", __FILE__, __FUNCSIG__); return 0; }
}

//Point to Cylinder distance with sign..i.e pt within cylinder is -ve dist and pt outside cylinder is +ve dist
double RapidMath3D::Distance_Point_Cylinder_with_Dir(double *Point, double *Cylinder)
{
	return Distance_Point_Line(Point, Cylinder) - Cylinder[6];
}
double RapidMath3D::Pt2Circle3D_distance_Type(double *point, double *center, double *dir, double radius, int type)
{
	try
	{
		double dist = 0, distance = 0;
		//Calculate the sum of square of the differences of each coordinate point and center// // (x1-x2)^2 + (y1-y2)^2
		if(type == 0)	//point to center distance//
		{
			for(int i = 0; i < 3; i ++)
				dist += pow((point[i] - center[i]), 2);
			distance = sqrt(dist);
		}
		if(type == 1)
		{
			for(int i = 0; i < 3; i++)
			{
				dist = dist + (*(point + i))*(*(dir + i));
			}
			dist = abs(dist - *(dir + 3));
			if(dist < PRECISION) dist = 0;
			distance = dist; 
		}
		if(type == 2)
		{
			double projectedPnt[3] = {0};
			Projection_Point_on_Plane(point, dir, projectedPnt);
			for(int i = 0; i < 3; i ++)
				dist += pow((projectedPnt[i] - center[i]), 2);
			if(dist < PRECISION) dist = 0;
			distance = sqrt(dist);
		}
		return distance;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0150", __FILE__, __FUNCSIG__); return 0; }
}
//function to find distance between two lines
double RapidMath3D::Distance_Line_Line(double *Line1, double *Line2)
{
	try
	{
		double ratios[3], cosines[3], Dist = 0, sum = 0;
		double TmpAng = Angle_Btwn_2Directions(&Line1[3], &Line2[3], false, false);

		if(abs(TmpAng) < ANGLEPRECISION || abs(TmpAng - M_PI) < ANGLEPRECISION)
		{
			for(int i = 0; i < 3; i ++) ratios[i] = Line1[i] - Line2[i];

			if(!DirectionCosines(&ratios[0], &cosines[0])) return -1.0;
	
			Dist = Distance_Point_Point(Line1, Line2);
			for(int i = 0; i < 3; i ++)
				sum += cosines[i] * Line1[3 + i];
			Dist = Dist * sin(acos(sum));

			Dist = abs(Dist);
			if(Dist < PRECISION) Dist = 0;
		
			return Dist;
		}
		else
		{
			for(int i = 0; i < 3; i ++) ratios[i] = Line1[3 + (i + 1) % 3] * Line2[3 + (i + 2) % 3] - Line2[3 + (i + 1) % 3] * Line1[3 + (i + 2) % 3];
	
			if(!DirectionCosines(&ratios[0], &cosines[0])) return -1.0;

			for(int i = 0; i < 3; i++) Dist += (Line2[i] - Line1[i]) * cosines[i];

			Dist = abs(Dist);
			if(Dist < PRECISION) Dist = 0;
		
			return Dist;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0008", __FILE__, __FUNCSIG__); return 0; }
}

//function to find distance between line and plane
double RapidMath3D::Distance_Line_Plane(double *Line, double *Plane)
{
	try
	{
		double sum = 0;
		for(int i = 0; i < 3; i++)
			sum += Line[3 + i] * Plane[i];

		if(sum > PRECISION)
			return 0;

		return Distance_Point_Plane(Line, Plane);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0009", __FILE__, __FUNCSIG__); return 0; }
}

//function to find distance between line and sphere with 3 measurement modes
double RapidMath3D::Distance_Line_Sphere(double *Line, double *Sphere, int MeasureType)
{
	try
	{
		double dist = Distance_Point_Line(Sphere, Line);
		if(MeasureType == 0)
			return abs(dist - Sphere[3]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Sphere[3];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0010", __FILE__, __FUNCSIG__); return 0; }
}

//Line to Cylinder distance with 3 measurement modes
double RapidMath3D::Distance_Line_Cylinder(double *Line, double *Cylinder, int MeasureType)
{
	try
	{
		double dist = Distance_Line_Line(Line, Cylinder);
		if(MeasureType == 0)
			return abs(dist - Cylinder[6]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Cylinder[6];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0011", __FILE__, __FUNCSIG__); return 0; }
}

//Line to Cone distance
double RapidMath3D::Distance_Line_Cone(double *Line, double *Cone)
{
	try
	{
		double dist = Distance_Line_Line(Line, Cone);
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0117", __FILE__, __FUNCSIG__); return 0; }
}

//function to find the distance between 2 planes
double RapidMath3D::Distance_Plane_Plane(double *Plane1, double *Plane2)
{
	try
	{
		double Dist = 0;	
		if(abs(Plane1[0] - Plane2[0]) < ANGLEPRECISION && abs(Plane1[1] - Plane2[1]) < ANGLEPRECISION && abs(Plane1[2] - Plane2[2]) < ANGLEPRECISION)
			Dist = abs(Plane1[3] - Plane2[3]);
		else if(abs(Plane1[0] + Plane2[0]) < ANGLEPRECISION && abs(Plane1[1] + Plane2[1]) < ANGLEPRECISION && abs(Plane1[2] + Plane2[2]) < ANGLEPRECISION)
			Dist = abs(Plane1[3] + Plane2[3]);
		else
			Dist = 0;
		return Dist;
		//return abs(Plane1[3] - Plane2[3]);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0012", __FILE__, __FUNCSIG__); return 0; }
}

//plane to sphere distance with 3 measurement modes
double RapidMath3D::Distance_Plane_Sphere(double *Plane, double *Sphere, int MeasureType)
{
	try
	{
		double dist = Distance_Point_Plane(Sphere, Plane);
		if(MeasureType == 0)
			return abs(dist - Sphere[3]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Sphere[3];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0013", __FILE__, __FUNCSIG__); return 0; }
}

//plane to cylinder distance with 3 measurement modes
double RapidMath3D::Distance_Plane_Cylinder(double *Plane, double *Cylinder, int MeasureType)
{
	try
	{
		double dist = Distance_Line_Plane(Cylinder, Plane);
		if(MeasureType == 0)
			return abs(dist - Cylinder[6]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Cylinder[6];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0014", __FILE__, __FUNCSIG__); return 0; }
}

//sphere to sphere distance with 3 measurement modes
double RapidMath3D::Distance_Sphere_Sphere(double *Sphere1, double *Sphere2, int MeasureType)
{
	try
	{
		double dist = Distance_Point_Point(Sphere1, Sphere2);
		if(MeasureType == 0)
			return abs(dist - Sphere1[3] - Sphere2[3]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Sphere1[3] + Sphere2[3];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0015", __FILE__, __FUNCSIG__); return 0; }
}

//sphere to cylinder distance with 3 measurement modes
double RapidMath3D::Distance_Sphere_Cylinder(double *Sphere, double *Cylinder, int MeasureType)
{
	try
	{
		double dist = Distance_Point_Line(Sphere, Cylinder);
		if(MeasureType == 0)
			return abs(dist - Sphere[3] - Cylinder[6]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Sphere[3] + Cylinder[6];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0016", __FILE__, __FUNCSIG__); return 0; }
}

//cylinder to cylinder distance with 3 measurement modes
double RapidMath3D::Distance_Cylinder_Cylinder(double *Cylinder1, double *Cylinder2, int MeasureType)
{
	try
	{
		double dist = Distance_Line_Line(Cylinder1, Cylinder2);
		if(MeasureType == 0)
			return abs(dist - Cylinder1[6] - Cylinder2[6]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Cylinder1[6] + Cylinder2[6];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0017", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::Distance_Cylinder_Cone(double *Cylinder, double *Cone, int MeasureType)
{
	try
	{
		double dist = Distance_Line_Line(Cylinder, Cone);
		if(MeasureType == 0)
			return abs(dist - Cylinder[6]);
		else if(MeasureType == 1)
			return dist;
		else
			return dist + Cylinder[6];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0017", __FILE__, __FUNCSIG__); return 0; 
	}
}

double RapidMath3D::Distance_Cone_Cone(double *Cone1, double *Cone2)
{
	try
	{
		double dist = Distance_Line_Line(Cone1, Cone2);
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0017", __FILE__, __FUNCSIG__); return 0; }
}

//--------------------------------------------------------------------------
//Angle Functions
//--------------------------------------------------------------------------

//Function to calculate angle between 2 directions(cosines or ratios). Also an option to get the cos of angle directly
double RapidMath3D::Angle_Btwn_2Directions(double *Direction1, double *Direction2, bool Ratios, bool CosOfAngle)
{
	try
	{
		double DirCos1[3] = {0}, DirCos2[3] = {0}, Angle = 0;
		if(Ratios)
		{
			DirectionCosines(Direction1, DirCos1);
			DirectionCosines(Direction2, DirCos2);
			for(int i = 0; i < 3; i++)
			{
				Angle += DirCos1[i] * DirCos2[i];
			}
		}
		else
		{
			for(int i = 0; i < 3; i++)
			{
				Angle += Direction1[i] * Direction2[i];
			}
		}
		if(Angle > 1) 
			Angle = 1;
		else if(Angle < -1) 
			Angle = -1;
		if(CosOfAngle) return Angle;

		return acos(Angle);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0018", __FILE__, __FUNCSIG__); return 0; }
}

bool RapidMath3D::CheckVectorBtwn2Vectors(double *Direction1, double *Direction2, double *checkDir, bool Ratios, bool equalVector)
{
	try
	{
		double DirCos1[3] = {Direction1[0], Direction1[1], Direction1[2]};
		double DirCos2[3] = {Direction2[0], Direction2[1], Direction2[2]};
		double CheckDirCos[3] = {checkDir[0], checkDir[1], checkDir[2]};
		if(Ratios)
		{
			DirectionCosines(Direction1, DirCos1);
			DirectionCosines(Direction2, DirCos2);
			DirectionCosines(checkDir, CheckDirCos);
		}
		double crossdir = 0, Angle1 = 0, Angle2 = 0;
		double crossProduct1[3] = {(DirCos1[1] * DirCos2[2] - DirCos1[2] * DirCos2[1]), (DirCos1[2] * DirCos2[0] - DirCos1[0] * DirCos2[2]), (DirCos1[0] * DirCos2[1] - DirCos1[1] * DirCos2[0])};
		double crossProduct2[3] = {(DirCos1[1] * CheckDirCos[2] - DirCos1[2] * CheckDirCos[1]), (DirCos1[2] * CheckDirCos[0] - DirCos1[0] * CheckDirCos[2]), (DirCos1[0] * CheckDirCos[1] - DirCos1[1] * CheckDirCos[0])};
		for(int i = 0; i < 3; i++)
			crossdir += (crossProduct1[i] * crossProduct2[i]);
		if(equalVector)
		{
			if(crossdir >= 0)
			{
			   for(int i = 0; i < 3; i++)
					Angle1 += (DirCos1[i] * DirCos2[i]);
			   for(int i = 0; i < 3; i++)
					Angle2 += (DirCos1[i] * CheckDirCos[i]);
			   if(acos(Angle2) <= acos(Angle1))
				  return true;
			 }
		}
		else
		{
			if(crossdir > 0)
			{
			   for(int i = 0; i < 3; i++)
					Angle1 += (DirCos1[i] * DirCos2[i]);
			   for(int i = 0; i < 3; i++)
					Angle2 += (DirCos1[i] * CheckDirCos[i]);
			   if(acos(Angle2) <= acos(Angle1))
				  return true;
			 }
		}
		return false;
	}

	catch(...){ SetAndRaiseErrorMessage("RMATH3D0018", __FILE__, __FUNCSIG__); return 0; }

}
//function to find angle between 2 lines
double RapidMath3D::Angle_Line_Line(double *Line1, double *Line2)
{
	return Angle_Btwn_2Directions(&Line1[3], &Line2[3], false, false);
}

//function to find angle between a line and a plane
double RapidMath3D::Angle_Line_Plane(double *Line, double *Plane)
{
	try
	{
		double sum = 0, TmpAng;
		//sum of the products of direction cosines// 
		for(int i = 0; i < 3; i ++)
		{
			sum += (Line[3 + i] * Plane[i]);
		}
	
		TmpAng = asin(sum);
		return abs(TmpAng);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0019", __FILE__, __FUNCSIG__); return 0; }
}

//function to find angle between a plane and a plane
double RapidMath3D::Angle_Plane_Plane(double *Plane1, double *Plane2)
{
	return Angle_Btwn_2Directions(Plane1, Plane2, false, false);
}



//--------------------------------------------------------------------------
//Intersection Functions
//--------------------------------------------------------------------------

//Function to find the intersection point of 2 intersecting lines
bool RapidMath3D::Intersection_Line_Line(double *Line1, double *Line2, double *IntersectionPt)
{
	try
	{
		double TmpAng = Angle_Btwn_2Directions(&Line1[3], &Line2[3], false, false);
		if(abs(TmpAng) < ANGLEPRECISION || abs(TmpAng - M_PI) < ANGLEPRECISION) return false;

		if(Distance_Line_Line(Line1, Line2) != 0)
			return false;

		double r1, r2;

		if(abs(Line1[4] * Line2[3] - Line1[3] * Line2[4]) > PRECISION && abs(Line1[4] * Line2[3] - Line1[3] * Line2[4]) > PRECISION)
		{
			r1 = (Line2[3] * (Line2[1] - Line1[1]) - Line2[4] * (Line2[0] - Line1[0])) / (Line1[4] * Line2[3] - Line1[3] * Line2[4]);
			r2 = (Line1[3] * (Line2[1] - Line1[1]) - Line1[4] * (Line2[0] - Line1[0])) / (Line1[4] * Line2[3] - Line1[3] * Line2[4]);
		}
		else if(abs(Line1[5] * Line2[3] - Line1[3] * Line2[5]) > PRECISION && abs(Line1[5] * Line2[3] - Line1[3] * Line2[5]) > PRECISION)
		{
			r1 = (Line2[3] * (Line2[2] - Line1[2]) - Line2[5] * (Line2[0] - Line1[0])) / (Line1[5] * Line2[3] - Line1[3] * Line2[5]);
			r2 = (Line1[3] * (Line2[2] - Line1[2]) - Line1[5] * (Line2[0] - Line1[0])) / (Line1[5] * Line2[3] - Line1[3] * Line2[5]);
		}
		else
		{
			r1 = (Line2[5] * (Line2[1] - Line1[1]) - Line2[4] * (Line2[2] - Line1[2])) / (Line1[4] * Line2[5] - Line1[5] * Line2[4]);
			r2 = (Line1[5] * (Line2[1] - Line1[1]) - Line1[4] * (Line2[2] - Line1[2])) / (Line1[4] * Line2[5] - Line1[5] * Line2[4]);
		}

		for(int i = 0; i < 3; i++)
			IntersectionPt[i] = Line1[3 + i] * r1 + Line1[i];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0020", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath3D::Plane_Triangle_Intersection(double* trianglePts, double Value, int Axisno, double* Ans)
{
	try
	{	
		double PlaneParam[4] = {1, 0, 0, Value};
		if(Axisno == 1)
		{
			PlaneParam[0] = 0; PlaneParam[1] = 1; PlaneParam[2] = 0;
		}
		else if(Axisno == 2)
		{
			PlaneParam[0] = 0; PlaneParam[1] = 0; PlaneParam[2] = 1;
		}

		int Count1 = 0, Count2 = 0, Count3 = 0;
		double IntersectPt1[3] = {0}, IntersectPt2[3] = {0}, IntersectPt3[3] = {0}; 

		DCs_TwoEndPointsofLine(trianglePts, IntersectPt1);
		DCs_TwoEndPointsofLine(&trianglePts[3], IntersectPt2);
		double LinePts[6] = {trianglePts[6], trianglePts[7], trianglePts[8], trianglePts[0], trianglePts[1], trianglePts[2]};
		DCs_TwoEndPointsofLine(LinePts, IntersectPt3);

		double Line1[6] = {trianglePts[0], trianglePts[1], trianglePts[2], IntersectPt1[0], IntersectPt1[1], IntersectPt1[2]}, 
			Line2[6] = {trianglePts[3], trianglePts[4], trianglePts[5], IntersectPt2[0], IntersectPt2[1], IntersectPt2[2]}, 
			Line3[6] = {trianglePts[6], trianglePts[7], trianglePts[8], IntersectPt3[0], IntersectPt3[1], IntersectPt3[2]};
		Count1 = Intersection_Line_Plane(Line1, PlaneParam, IntersectPt1);		
		Count2 = Intersection_Line_Plane(Line2, PlaneParam, IntersectPt2);		
		Count3 = Intersection_Line_Plane(Line3, PlaneParam, IntersectPt3);
		if(Count1 == 1)
		{
			if(Checking_Point_On_LineSegment(trianglePts, IntersectPt1)) 
			{
				Ans[0] = IntersectPt1[0]; Ans[1] = IntersectPt1[1]; Ans[2] = IntersectPt1[2];
				if(Count2 == 1)
				{
					if(Checking_Point_On_LineSegment(&trianglePts[3], IntersectPt2))
					{
						Ans[3] = IntersectPt2[0]; Ans[4] = IntersectPt2[1]; Ans[5] = IntersectPt2[2];
						return true;
					}
				}
				if(Count3 == 1)
				{
					if(!Checking_Point_On_LineSegment(LinePts, IntersectPt3)) return false;
					Ans[3] = IntersectPt3[0]; Ans[4] = IntersectPt3[1]; Ans[5] = IntersectPt3[2];
				}
				else 
					return false;
			}
			else
			{
				if(Count2 == 1)
				{
					if(!Checking_Point_On_LineSegment(&trianglePts[3], IntersectPt2)) return false;
					Ans[0] = IntersectPt2[0]; Ans[1] = IntersectPt2[1]; Ans[2] = IntersectPt2[2];
					if(Count3 == 1)
					{
						if(!Checking_Point_On_LineSegment(LinePts, IntersectPt3)) return false;
						Ans[3] = IntersectPt3[0]; Ans[4] = IntersectPt3[1]; Ans[5] = IntersectPt3[2];
					 }
				}
				else
					return false;
			}
		}
		else if(Count2 == 1)
		{
			if(!Checking_Point_On_LineSegment(&trianglePts[3], IntersectPt2)) return false;
			Ans[0] = IntersectPt2[0]; Ans[1] = IntersectPt2[1]; Ans[2] = IntersectPt2[2];
			if(Count3 == 1)
			{
				if(!Checking_Point_On_LineSegment(LinePts, IntersectPt3)) return false;
				Ans[3] = IntersectPt3[0]; Ans[4] = IntersectPt3[1]; Ans[5] = IntersectPt3[2];
			}
			else 
				return false;
		}
		else
			return false;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0020a", __FILE__, __FUNCSIG__); return false; }
}

//function to find the intersection point of a line with a plane. Returns 0 if line on the plane, 1 if intersection
int RapidMath3D::Intersection_Line_Plane(double *Line, double *Plane, double *IntersectionPt)
{
	try
	{
		double sum1 = 0, sum2 = 0, R = 0;
	
		for(int i = 0; i < 3; i++)
		{
			sum1 += (Line[i] * Plane[i]);
			sum2 += (Line[3 + i] * Plane[i]);
		}

		if(abs(sum2) < ANGLEPRECISION) return 0;
		R = (Plane[3] - sum1) / sum2;

		for (int i = 0; i < 3; i++) {
			IntersectionPt[i] = Line[i] + Line[3 + i] * R;
		}
		return 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0021", __FILE__, __FUNCSIG__); return -1; }
}

//function to find the intersection of a line and a sphere
int RapidMath3D::Intersection_Line_Sphere(double *Line, double *Sphere, double *IntersectionPts)
{
	try
	{
		double parameters[3] = {0}, answers[3], temp[3];

		for(int i = 0; i < 3; i++)
			temp[i] = Line[i] - Sphere[i];

		parameters[0] = 1;

		for(int i = 0; i < 3; i++)
			parameters[1] += Line[3 + i] * temp[i];

		parameters[1] = parameters[1] * 2;

		for(int i = 0; i < 3; i++)
			parameters[2] += pow(temp[i], 2);

		parameters[2] = parameters[2] - (Sphere[3] * Sphere[3]);

		int NoOfSolns = Solve_Quadratic_Equation(parameters, answers);

		if(NoOfSolns == 0) return 0;

		if(NoOfSolns == 1)
		{
			for(int i = 0; i < 3; i++)
				IntersectionPts[i] = Line[3 + i] * answers[0] + Line[i];

			return 1;
		}

		if(NoOfSolns == 2)
		{
			for(int i = 0; i < 3; i++)
			{
				IntersectionPts[i] = Line[3 + i] * answers[0] + Line[i];
				IntersectionPts[3 + i] = Line[3 + i] * answers[1] + Line[i];
			}

			return 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0022", __FILE__, __FUNCSIG__); return -1; }
}
int RapidMath3D::Intersection_Line_elipse(double *Line, double *elipse, double *IntersectionPts)
{
	try
	{
		double parameters[3] = {0}, Intercpt, Max, Min, Linintrpnt[3];
		Max = (2 * elipse[13] * elipse[16])/(1 - elipse[13] * elipse[13]);
		Min = sqrt((Max * Max) * (1 - elipse[13] * elipse[13]));
		double dir[6], Cen[3];
		Create_Perpendicular_Line_2_Line(&elipse[4], &elipse[7], &dir[0]);
		double focallgt = (Max/2) - (elipse[13] *elipse[16])/(1 + elipse[13]);
		for(int i = 0; i < 3; i++) Cen[i] = elipse[4 + i] + focallgt * dir[3 + i];
		Max = Max / 2;
		Min = Min / 2;
		double FocalLine[6] = {elipse[4], elipse[5], elipse[6], elipse[10], elipse[11], elipse[12]};
		if( Intersection_Line_Line(FocalLine, Line, Linintrpnt))
		{
			Intercpt = Distance_Point_Point(&elipse[4], &Linintrpnt[0]);
		}
		if(elipse[10] * (Linintrpnt[0] - elipse[4]) + elipse[11] * (Linintrpnt[1] - elipse[5]) + elipse[12]* (Linintrpnt[2] - elipse[6]) < 0)
		{
		   for(int i = 0; i < 3; i++)
			   FocalLine[3 + i] =  -FocalLine[3 + i];
		}
		if (FocalLine[3] * Line[3] + FocalLine[4] * Line[4] + FocalLine[5] * Line[5] <0)
		{   
		   for(int i = 0; i < 3; i++)
			   Line[3 + i] = -Line[3 + i];
		}
		double SlopeAng = Angle_Line_Line(dir, Line);
		double tempdir[3];
		 for(int i = 0; i < 3; i++)
			  tempdir[i] = dir[3 + i] * cos(SlopeAng) + FocalLine[3 + i] * sin(SlopeAng);
		if (tempdir[0] * Line[3] + tempdir[1] * Line[4] + tempdir[2] * Line[5] < 0)
			  SlopeAng = -SlopeAng;
		double Slope = tan(SlopeAng);
		double Parameter1 =  elipse[16];
		double Parameter2 =  Intercpt - elipse[16] * Slope;
		double Alpha = atan(Parameter2/Parameter1);
		double Beta, Gamma = (Intercpt/(elipse[13] * sqrt(Parameter1 * Parameter1 + Parameter2 * Parameter2)));
		if (1 > Gamma && Gamma > -1)
		{
			 Beta = asin(Gamma);
			 double Theta1 = Beta - Alpha;
			 double Theta2 = M_PI + Beta - Alpha;
			 double Intrsectdir1[3], Intrsectdir2[3];
			 for(int i = 0; i < 3; i++)
			 {
				Intrsectdir1[i] = dir[3 + i] * cos(Theta1) + FocalLine[3 + i] * sin(Theta1);
				Intrsectdir2[i] = dir[3 + i] * cos(Theta2) + FocalLine[3 + i] * sin(Theta2);
			 }
			  double R1 = (elipse[16]/((1/elipse[13]) - cos(Theta1)));
			 double R2 = (elipse[16]/((1/elipse[13]) - cos(Theta2)));
			 for (int i = 0; i < 3; i++)
			 {
			 IntersectionPts[i] = elipse[4 +i] + R1 * Intrsectdir1[i];
			 IntersectionPts[3 + i] = elipse[4 +i] + R2 * Intrsectdir2[i];
			 }
			 return 2;
		}
		else 
		{
		return 0;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0022", __FILE__, __FUNCSIG__); return -1; }
}

//Function to find the intersection of a line and a cylinder. Returns 0 if no intersection, 1 if line tangential to surface, 2 if intersection
int RapidMath3D::Intersection_Line_Cylinder(double *Line, double *Cylinder, double *IntersectionPts)
{
	try
	{
		double a, b, c, l1, m1, n1, l2term, m2term, n2term, Xdiff, Ydiff, Zdiff, l1m1, l2m2, m1n1, m2n2, l1n1, l2n2;
		double Answer[2];
		int NoOfAns;

		l1 = Line[3];
		m1 = Line[4];
		n1 = Line[5];
		l2term = (1 - pow(Cylinder[3],2));
		m2term = (1 - pow(Cylinder[4],2));
		n2term = (1 - pow(Cylinder[5],2));
		Xdiff = Line[0] - Cylinder[0];
		Ydiff = Line[1] - Cylinder[1];
		Zdiff = Line[2] - Cylinder[2];
		l1m1 = l1 * m1;
		m1n1 = m1 * n1;
		l1n1 = l1 * n1;
		l2m2 = Cylinder[3] * Cylinder[4];
		m2n2 = Cylinder[4] * Cylinder[5];
		l2n2 = Cylinder[3] * Cylinder[5];

		a =  pow(l1,2) * l2term
		   + pow(m1,2) * m2term
		   + pow(n1,2) * n2term
		   - 2 * l1m1 * l2m2
		   - 2 * m1n1 * m2n2
		   - 2 * l1n1 * l2n2;

		b =  2 * l1 * Xdiff *l2term
		   + 2 * m1 * Ydiff *m2term
		   + 2 * n1 * Zdiff *n2term
		   - 2 * l2m2 * (l1 * Ydiff + m1 * Xdiff)
		   - 2 * m2n2 * (m1 * Zdiff + n1 * Ydiff)
		   - 2 * l2n2 * (l1 * Zdiff + n1 * Xdiff);

		c =  pow(Xdiff,2) * l2term
		   + pow(Ydiff,2) * m2term
		   + pow(Zdiff,2) * n2term
		   - 2 * l2m2 * Xdiff * Ydiff
		   - 2 * m2n2 * Ydiff * Zdiff
		   - 2 * l2n2 * Xdiff * Zdiff
		   - pow(Cylinder[6], 2);

		double Parameters[] = {a, b, c};
		NoOfAns = Solve_Quadratic_Equation(Parameters, Answer);

		if(NoOfAns == 0)
			return 0;
		else if(NoOfAns == 1)
		{
			for(int i = 0; i < 3; i++)
				IntersectionPts[i] = Line[3 + i] * Answer[0] + Line[i];
			return 1;
		}
		else if(NoOfAns == 2)
		{
			for(int i = 0; i < 3; i++)
			{
				IntersectionPts[i] = Line[3 + i] * Answer[0] + Line[i];
				IntersectionPts[3 + i] = Line[3 + i] * Answer[1] + Line[i];
			}
			return 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0023", __FILE__, __FUNCSIG__); return -1; }
}

//Function to find the intersection of a line and a cone. Returns 0 if no intersection, 1 if line tangential to surface, 2 if intersection
int RapidMath3D::Intersection_Line_Cone(double *Line, double *Cone, double *IntersectionPts)
{
	try
	{
		double a, b, c, l1, m1, n1, l2term, m2term, n2term, Xdiff, Ydiff, Zdiff, l1m1, l2m2, m1n1, m2n2, l1n1, l2n2;
		double Answer[2];
		int NoOfAns;
		double t = tan (Cone[7]);

		l1 = Line[3];
		m1 = Line[4];
		n1 = Line[5];
		l2term = (1 - pow(Cone[3],2));
		m2term = (1 - pow(Cone[4],2));
		n2term = (1 - pow(Cone[5],2));
		Xdiff = Line[0] - Cone[0];
		Ydiff = Line[1] - Cone[1];
		Zdiff = Line[2] - Cone[2];
		l1m1 = l1 * m1;
		m1n1 = m1 * n1;
		l1n1 = l1 * n1;
		l2m2 = Cone[3] * Cone[4];
		m2n2 = Cone[4] * Cone[5];
		l2n2 = Cone[3] * Cone[5];

		a =  pow(l1,2) * l2term
		   + pow(m1,2) * m2term
		   + pow(n1,2) * n2term
		   - 2 * l1m1 * l2m2
		   - 2 * m1n1 * m2n2
		   - 2 * l1n1 * l2n2;

		b =  2 * l1 * Xdiff *l2term
		   + 2 * m1 * Ydiff *m2term
		   + 2 * n1 * Zdiff *n2term
		   - 2 * l2m2 * (l1 * Ydiff + m1 * Xdiff)
		   - 2 * m2n2 * (m1 * Zdiff + n1 * Ydiff)
		   - 2 * l2n2 * (l1 * Zdiff + n1 * Xdiff);

		c =  pow(Xdiff,2) * l2term
		   + pow(Ydiff,2) * m2term
		   + pow(Zdiff,2) * n2term
		   - 2 * l2m2 * Xdiff * Ydiff
		   - 2 * m2n2 * Ydiff * Zdiff
		   - 2 * l2n2 * Xdiff * Zdiff
		   - pow(Cone[6], 2);

		double da = - pow ( t * (Cone[3] * l1 + Cone[4] * m1 + Cone[5] * n1), 2);
		double dc = - pow ( t * (Cone[3] * Xdiff + Cone[4] * Ydiff + Cone[5] * Zdiff), 2) - 2 * Cone[6] * t * (Cone[3] * Xdiff + Cone[4] * Ydiff + Cone[5] * Zdiff);
		double db = - 2 * t * (Cone[3] * l1 + Cone[4] * m1 + Cone[5] * n1) * (Cone[6] + t * (Cone[3] * Xdiff + Cone[4] * Ydiff + Cone[5] * Zdiff));

		a = a + da;
		b = b + db;
		c = c + dc;

		double Parameters[] = {a, b, c};
		NoOfAns = Solve_Quadratic_Equation(Parameters, Answer);

		if(NoOfAns == 0)
			return 0;
		else if(NoOfAns == 1)
		{
			for(int i = 0; i < 3; i++)
				IntersectionPts[i] = Line[3 + i] * Answer[0] + Line[i];
			return 1;
		}
		else if(NoOfAns == 2)
		{
			for(int i = 0; i < 3; i++)
			{
				IntersectionPts[i] = Line[3 + i] * Answer[0] + Line[i];
				IntersectionPts[3 + i] = Line[3 + i] * Answer[1] + Line[i];
			}
			return 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0116", __FILE__, __FUNCSIG__); return -1; }
}

bool RapidMath3D::Intersection_Line_Torus(double* Line, double* Torus, double wupp, int* pt_cnt, double* intersection_pts)
{
	try
	{
		bool intersection_found = false;
		*pt_cnt = 0;
		double x_center[3] = {Torus[0],Torus[1],Torus[2]};
		double axis[3] = {Torus[3],Torus[4],Torus[5]};
		double outer_radius = Torus[6];
		double inner_radius = Torus[7];
		double incr_angle1 = 2 * M_PI * wupp / outer_radius;
		double incr_angle2 = 2 * M_PI * wupp / inner_radius;
		double p1[3]={1,0,0};
		p1[0] = -axis[1];
		p1[1] = axis[0];
		double norm = sqrt (axis[0] * axis[0]  + axis[1] * axis[1]);
		if (norm != 0)
		{
			p1[0] /= norm; 
			p1[1] /= norm;
		}
		else
		{
			p1[0] = 1;
			p1[1] = 0;
		}
		double p2[3] = {axis[1] * p1[2] - axis[2] * p1[1], axis[2] * p1[0] - axis[0] * p1[2], axis[0] * p1[1] - axis[1] * p1[0]};
		double pt[3] = {0};
		double pt_torus[3] = {0};
		double dist1, dist2;
		bool pt_already_included;

		for(double angle1 = 0; angle1 < 2 * M_PI; angle1 += incr_angle1/3)
		{
			for (int i = 0; i < 3; i++)
			{
				pt[i] = x_center[i] + (p1[i] * cos(angle1) + p2[i] * sin(angle1)) *outer_radius;
			}
			
			for(double angle2 = 0; angle2 < 2 * M_PI; angle2 += incr_angle2/3)
			{
				for (int i = 0; i < 3; i++)
				{
					pt_torus[i] = pt[i] + ((p1[i] * cos(angle1) + p2[i] * sin(angle1)) * cos(angle2) + axis[i] * sin(angle2)) * inner_radius ;
				}
				pt_already_included = false;
				for (int j = 0; j < *pt_cnt; j++)
				{
					dist1 = Distance_Point_Point(pt_torus, intersection_pts + 3*j);
					if (dist1 < 6 * wupp)
					{
						pt_already_included = true;
						break;
					}
				}
				if (pt_already_included == false)
				{
					dist2 = Distance_Point_Line(pt_torus, Line);
					if (dist2 < 3 * wupp)
					{
						intersection_found = true;
						for (int j = 0; j < 3; j++)
						{
							intersection_pts[3 * (*pt_cnt) + j] = pt_torus[j];
						}
						*pt_cnt = *pt_cnt + 1;
						if (*pt_cnt == 4)
							return true;
					}
				}
			}
		}
		return intersection_found;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0116", __FILE__, __FUNCSIG__); return false; }
}

//Function to find the intersection line of 2 planes. Returns 0 if no intersection, 1 if both coincide, 2 if intersection
int RapidMath3D::Intersection_Plane_Plane(double *Plane1, double *Plane2, double *IntersectionLine)
{
	try
	{
		double TmpAng = Angle_Btwn_2Directions(Plane1, Plane2, false, false);
		if(abs(TmpAng) < ANGLEPRECISION)
		{
			if(abs(Plane1[3] - Plane2[3]) < PRECISION) return 1;
			else return 0;
		}

		if(abs(TmpAng - M_PI) < ANGLEPRECISION)
		{
			if(abs(Plane1[3] + Plane2[3]) < PRECISION) return 1;
			else return 0;
		}

		double ratios[3] = {0,0,1}, cosines[3] = {0,0,1}, eq1[3] = {0}, eq2[3] = {0}, solution[2] = {0};

		for(int i = 0; i < 3; i++)
			ratios[i] = Plane1[(i + 1) % 3] * Plane2[(i + 2) % 3] - Plane2[(i + 1) % 3] * Plane1[(i + 2) % 3];

		DirectionCosines(ratios, cosines);
		IntersectionLine[3] = cosines[0];
		IntersectionLine[4] = cosines[1];
		IntersectionLine[5] = cosines[2];

		if(abs(cosines[2]) > PRECISION)
		{
			eq1[0] = Plane1[0];
			eq1[1] = Plane1[1];
			eq1[2] = -1 * Plane1[3];

			eq2[0] = Plane2[0];
			eq2[1] = Plane2[1];
			eq2[2] = -1 * Plane2[3];

			SolveEquation(eq1, eq2, solution);
			IntersectionLine[0] = solution[0];
			IntersectionLine[1] = solution[1];
			IntersectionLine[2] = 0;
		}
		else if(abs(cosines[1]) > PRECISION)
		{
			eq1[0] = Plane1[0];
			eq1[1] = Plane1[2];
			eq1[2] = -1 * Plane1[3];

			eq2[0] = Plane2[0];
			eq2[1] = Plane2[2];
			eq2[2] = -1 * Plane2[3];

			SolveEquation(eq1, eq2, solution);
			IntersectionLine[0] = solution[0];
			IntersectionLine[1] = 0;
			IntersectionLine[2] = solution[1] ;
		}
		else
		{
			eq1[0] = Plane1[1];
			eq1[1] = Plane1[2];
			eq1[2] = -1 * Plane1[3];

			eq2[0] = Plane2[1];
			eq2[1] = Plane2[2];
			eq2[2] = -1 * Plane2[3];

			SolveEquation(eq1, eq2, solution);
			IntersectionLine[0] = 0;
			IntersectionLine[1] = solution[0];
			IntersectionLine[2] = solution[1];
		}

		return 2;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0024", __FILE__, __FUNCSIG__); return -1; }
}

//Function to find the intersection circle of a plane and a sphere. Returns 0 if no intersection, 1 if tangent, 2 if intersection
int RapidMath3D::Intersection_Plane_Sphere(double *Plane, double *Sphere, double *IntersectionCircle)
{
	try
	{
		double dist = Distance_Point_Plane(Sphere, Plane);

		if(dist > Sphere[3]) return 0;

		double Line[6];
		for(int i = 0; i < 3; i++)
		{
			Line[i] = Sphere[i];
			Line[3 + i] = Plane[i];
		}
		Intersection_Line_Plane(Line, Plane, IntersectionCircle);

		if(dist == Sphere[3])
		{
			return 1;
		}

		if(dist < Sphere[3])
		{
			for(int i = 0; i < 3; i++)
				IntersectionCircle[3 + i] = Plane[i];

			IntersectionCircle[6] = sqrt(pow(Sphere[3],2) - pow(dist,2));
			return 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0025", __FILE__, __FUNCSIG__); return -1; }
}

//Function to find the intersection circle of a plane and a cylinder(Provided the axis of the cylinder is parallel to plane)
bool RapidMath3D::Intersection_Plane_Cylinder(double *Plane, double *Cylinder, double *IntersectionCircle)
{
	try
	{
		double TmpAng = Angle_Btwn_2Directions(Plane, &Cylinder[3], false, false);
		if(abs(TmpAng) > ANGLEPRECISION && abs(TmpAng - M_PI) > ANGLEPRECISION) return false;

		Intersection_Line_Plane(Cylinder, Plane, IntersectionCircle);

		for(int i = 0; i < 3; i++)
			IntersectionCircle[3 + i] = Cylinder[3 + i];

		IntersectionCircle[6] = Cylinder[6];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0026", __FILE__, __FUNCSIG__); return false; }
}



//--------------------------------------------------------------------------
//Condition Checking Functions
//--------------------------------------------------------------------------

bool RapidMath3D::Checking_Point_On_LineSegment(double *EndPts, double *Point)
{
	try
	{
		double MinX = 0, MaxX = 0, MinY = 0, MaxY = 0, MinZ = 0, MaxZ = 0;
		if(EndPts[0] < EndPts[3])
		{
			MinX = EndPts[0];
			MaxX = EndPts[3];
		}
		else
		{
			MinX = EndPts[3];
			MaxX = EndPts[0];
		}
		if(EndPts[1] < EndPts[4])
		{
			MinY = EndPts[1];
			MaxY = EndPts[4];
		}
		else
		{
			MinY = EndPts[4];
			MaxY = EndPts[1];
		}
		if(EndPts[2] < EndPts[5])
		{
			MinZ = EndPts[2];
			MaxZ = EndPts[5];
		}
		else
		{
			MinZ = EndPts[5];
			MaxZ = EndPts[2];
		}
		if(MinX <= Point[0] && MaxX >= Point[0] && MinY <= Point[1] && MaxY >= Point[1] && MinZ <= Point[2] && MaxZ >= Point[2])
			return true;
		else
			return false;

		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0027", __FILE__, __FUNCSIG__); return false; }
}

//Function to check if a given point is within the bounds of X, Y and Z extremes of a group of points
bool RapidMath3D::Checking_Point_within_Block(double *Point, double *Points, int NoOfPts)
{
	try
	{
		if(NoOfPts < 2) return false;

		double MinX = Points[0], MaxX = Points[0], MinY = Points[1], MaxY = Points[1], MinZ = Points[2], MaxZ = Points[2];
		for(int i = 1; i < NoOfPts; i++)
		{
			int temp = 3 * i;
			if(Points[temp] > MaxX) MaxX = Points[temp];
			if(Points[temp] < MinX) MinX = Points[temp];
			if(Points[temp + 1] > MaxY) MaxY = Points[temp + 1];
			if(Points[temp + 1] < MinY) MinY = Points[temp + 1];
			if(Points[temp + 2] > MaxZ) MaxZ = Points[temp + 2];
			if(Points[temp + 2] < MinZ) MinZ = Points[temp + 2];
		}
		MaxX += PRECISION; MinX -= PRECISION;
		MaxY += PRECISION; MinY -= PRECISION;
		MaxZ += PRECISION; MinZ -= PRECISION;

		if(Point[0] <= MaxX && Point[0] >= MinX &&
		   Point[1] <= MaxY && Point[1] >= MinY &&
		   Point[2] <= MaxZ && Point[2] >= MinZ) return true;

		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0027", __FILE__, __FUNCSIG__); return false; }
}

//function to check if a point is on the line or not
bool RapidMath3D::Checking_Point_on_Line(double *Point, double *Line)
{
	try
	{
		double term1, term2, term3;

		term1 = (Point[0] - Line[0]) / Line[3];
		term2 = (Point[1] - Line[1]) / Line[4];
		term3 = (Point[2] - Line[2]) / Line[5];

		if(abs(term1 - term2) < PRECISION && abs(term2 - term3) < PRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0028", __FILE__, __FUNCSIG__); return false; }
}

//function to check if a point is on the plane or not
bool RapidMath3D::Checking_Point_on_Plane(double *Point, double *Plane)
{
	try
	{
		double sum = 0;

		for(int i = 0; i < 3; i++)
			sum += Plane[i] * Point[i];

		if(abs(sum - Plane[3]) < PRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0029", __FILE__, __FUNCSIG__); return false; }
}

//function to check if a point is on the sphere or not
bool RapidMath3D::Checking_Point_on_Sphere(double *Point, double *Sphere)
{
	try
	{
		double sum = 0;

		for(int i = 0; i < 3; i++)
			sum += pow(Point[i] - Sphere[i], 2);

		if(abs(sqrt(sum) - Sphere[3]) < PRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0030", __FILE__, __FUNCSIG__); return false; }
}

//function to check if a point is on the cylinder or not
bool RapidMath3D::Checking_Point_on_Cylinder(double *Point, double *Cylinder)
{
	try
	{
		double dist = Distance_Point_Line(Point, Cylinder);

		if(abs(dist - Cylinder[6]) < PRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0031", __FILE__, __FUNCSIG__); return false; }
}

//function to check if a line is contained in the plane
bool RapidMath3D::Checking_Line_on_Plane(double *Line, double *Plane)
{
	try
	{
		double sum = 0;
	
		for(int i = 0; i < 3; i++)
			sum += Line[3 + i] * Plane[i];
	
		if(abs(sum) < ANGLEPRECISION) sum = 0;

		if(sum != 0) return false;

		for(int i = 0; i < 3; i++)
			sum += Plane[i] * Line[i];

		if(abs(sum - Plane[3]) < PRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0032", __FILE__, __FUNCSIG__); return false; }
}

//function to check if 2 lines are parallel
bool RapidMath3D::Checking_Parallel_Line_to_Line(double *Line1, double *Line2)
{
	try
	{
		double TmpAng = Angle_Btwn_2Directions(&Line1[3], &Line2[3], false, false);
		if(abs(TmpAng) < ANGLEPRECISION || abs(TmpAng - M_PI) < ANGLEPRECISION) return true;
		
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0033", __FILE__, __FUNCSIG__); return false; }
}

//function to check if 2 lines are perpendicular
bool RapidMath3D::Checking_Perpendicular_Line_to_Line(double *Line1, double *Line2)
{
	try
	{
		double sum = 0;
	
		for(int i = 0; i < 3; i++)
			sum += Line1[3 + i] * Line2[3 + i];
	
		if(abs(sum) < ANGLEPRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0034", __FILE__, __FUNCSIG__); return false; }
}

//function to check if 2 lines are intersecting
bool RapidMath3D::Checking_Intersection_Line_Line(double *Line1, double *Line2)
{
	try
	{
		double TmpAng = Angle_Btwn_2Directions(&Line1[3], &Line2[3], false, false);
		if(abs(TmpAng) < ANGLEPRECISION || abs(TmpAng - M_PI) < ANGLEPRECISION) return false;

		if(Distance_Line_Line(Line1, Line2) != 0)
			return false;

		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0035", __FILE__, __FUNCSIG__); return false; }
}

//function to check if a line is parallel to a plane
bool RapidMath3D::Checking_Parallel_Line_to_Plane(double *Line, double *Plane)
{
	try
	{
		double sum = 0;
	
		for(int i = 0; i < 3; i++)
			sum += Line[3 + i] * Plane[i];
	
		if(abs(sum) < ANGLEPRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0036", __FILE__, __FUNCSIG__); return false; }
}

//function to check if a line is perpendicular to a plane
bool RapidMath3D::Checking_Perpendicular_Line_to_Plane(double *Line, double *Plane)
{
	try
	{
		double TmpAng = Angle_Btwn_2Directions(&Line[3], Plane, false, false);
		if(abs(TmpAng) < ANGLEPRECISION || abs(TmpAng - M_PI) < ANGLEPRECISION) return true;
	
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0037", __FILE__, __FUNCSIG__); return false; }
}

//function to check if 2 planes are parallel
bool RapidMath3D::Checking_Parallel_Plane_to_Plane(double *Plane1, double *Plane2)
{
	try
	{
		double TmpAng = Angle_Btwn_2Directions(Plane1, Plane2, false, false);
		
		if(abs(TmpAng) < ANGLEPRECISION || abs(TmpAng - M_PI) < ANGLEPRECISION) return true;
		/*if((abs(Plane1[0] - Plane2[0]) < PRECISION && abs(Plane1[1] - Plane2[1]) < PRECISION && abs(Plane1[2] - Plane2[2]) < PRECISION) ||
			(abs(Plane1[0] + Plane2[0]) < PRECISION && abs(Plane1[1] + Plane2[1]) < PRECISION && abs(Plane1[2] + Plane2[2]) < PRECISION))
				return true;*/
	
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0038", __FILE__, __FUNCSIG__); return false; }
}

//function to check if 2 planes are perpendicular
bool RapidMath3D::Checking_Perpendicular_Plane_to_Plane(double *Plane1, double *Plane2)
{
	try
	{
		double sum = 0;
	
		for(int i = 0; i < 3; i++)
			sum += Plane1[i] * Plane2[i];
	
		if(abs(sum) < ANGLEPRECISION) return true;
		else return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0039", __FILE__, __FUNCSIG__); return false; }
}



//--------------------------------------------------------------------------
//Create Functions
//--------------------------------------------------------------------------

//Function to measure the direction cosines of a direction perpendicular to 2 given directions
bool RapidMath3D::Create_Perpendicular_Direction_2_2Directions(double *Direction1, double *Direction2, double* PerpendDir)
{
	try
	{
		double Solution[3];
		SolveEquationRatio(Direction1, Direction2, Solution);

		if(!DirectionCosines(Solution, PerpendDir))
			return false;

		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0040", __FILE__, __FUNCSIG__); return false; }
}

//Function to create angle bisector of 2 lines
bool RapidMath3D::Create_AngleBisector_Lines(double *Line1, double *Line2, double *Bisector1, double *Bisector2)
{
	try
	{
		double IntersectionPt[3];

		if(!Intersection_Line_Line(Line1, Line2, IntersectionPt))
			return false;
	
		for(int i = 0; i < 3; i++)
		{
			Bisector1[i] = IntersectionPt[i];
			Bisector2[i] = IntersectionPt[i];
			Bisector1[3 + i] = cos((acos(Line1[3 + i]) + acos(Line2[3 + i])) / 2);
			Bisector2[3 + i] = cos((acos(-Line1[3 + i]) + acos(Line2[3 + i])) / 2);
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0041", __FILE__, __FUNCSIG__); return false; }
}

//Function to create angle bisector of 2 planes
bool RapidMath3D::Create_AngleBisector_Planes(double *Plane1, double *Plane2, double *Bisector1, double *Bisector2)
{
	try
	{
		if(Plane1[0] == Plane2[0] && Plane1[1] == Plane2[1] && Plane1[2] == Plane2[2])
			return false;

		double ratios[4], cosines[4];
	
		for(int i = 0; i < 4; i++)
			ratios[i] = Plane1[i] - Plane2[i];
		DirectionCosines(ratios, cosines, true);
		for(int i = 0; i < 4; i++)
			Bisector1[i] = cosines[i];

		for(int i = 0; i < 4; i++)
			ratios[i] = Plane1[i] + Plane2[i];
		DirectionCosines(ratios, cosines, true);
		for(int i = 0; i < 4; i++)
			Bisector2[i] = cosines[i];

		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0042", __FILE__, __FUNCSIG__); return false; }
}

//Function to create a line through a point, parallel to another line
void RapidMath3D::Create_Parallel_Line_2_Line(double *Point, double *Line, double *CreatedLine, double offset)
{
	try
	{
		for(int i = 0; i < 3; i++)
		{
			CreatedLine[i] = Point[i];
			CreatedLine[3 + i] = Line[3 + i];
		}
		if (offset != 0)
		{

		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0043", __FILE__, __FUNCSIG__); }
}

//Function to create a line perpendicular to another line, passing through a given point.
bool RapidMath3D::Create_Perpendicular_Line_2_Line(double *Point, double *Line, double *CreatedLine)
{
	try
	{
		double ProjectionPoint[3] = {0}, ratios[3] = {0,0,1}, cosines[3] = {0,0,1};

		if(!Projection_Point_on_Line(Point, Line, ProjectionPoint)) return false;
		for(int i = 0; i < 3; i++)
			ratios[i] =  Point[i] - ProjectionPoint[i];

		if(!DirectionCosines(ratios, cosines)) return false;

		for(int i = 0; i < 3; i++)
		{
			CreatedLine[i] = Point[i];
			CreatedLine[3 + i] = cosines[i];
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0044", __FILE__, __FUNCSIG__); return false; }
}

//Function to find a line perpendicular to a plane passing through a given point
void RapidMath3D::Create_Perpendicular_Line_2_Plane(double *Point, double *Plane, double *CreatedLine)
{
	try
	{
		for(int i = 0; i < 3; i++)
		{
			CreatedLine[i] = Point[i];
			CreatedLine[3 + i] = Plane[i];
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0045", __FILE__, __FUNCSIG__); }
}

//Function to create a parallel plane through a point, parallel to another plane
void RapidMath3D::Create_Parallel_Plane_2_Plane(double *Point, double *Plane, double *CreatedPlane)
{
	try
	{
		double sum = 0;

		for(int i = 0; i < 3; i++)
		{
			CreatedPlane[i] = Plane[i];
			sum += Point[i] * Plane[i];
		}
		CreatedPlane[3] = sum;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0046", __FILE__, __FUNCSIG__); }
}

//Function to create a plane perpendicular to a plane containing a line
bool RapidMath3D::Create_Perpendicular_Plane_2_Plane(double *Line, double *Plane, double *CreatedPlane)
{
	try
	{
		if(Line[3] == Plane[0] && Line[4] == Plane[1] && Line[5] == Plane[2])
			return false;

		double ratios[3], sum = 0;
		SolveEquationRatio(Plane, &Line[3], ratios);
		DirectionCosines(ratios, CreatedPlane);

		for(int i = 0; i < 3; i++)
			sum += CreatedPlane[i] * Line[i];

		CreatedPlane[3] = sum;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0047", __FILE__, __FUNCSIG__); return false; }
}

//Function to create a plane perpendicular to a plane containing 2 given points
bool RapidMath3D::Create_Perpendicular_Plane_2_Plane(double *Point1, double *Point2, double *Plane, double *CreatedPlane)
{
	try
	{
		double ratios[3], cosines[3], Line[6];

		for(int i = 0; i < 3; i++)
			ratios[i] = Point1[i] - Point2[i];

		if(!DirectionCosines(ratios, cosines)) return false;

		for(int i = 0; i < 3; i++)
		{
			Line[i] = Point1[i];
			Line[3 + i] = cosines[i];
		}

		if(!Create_Perpendicular_Plane_2_Plane(Line, Plane, CreatedPlane)) return false;
		else return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0048", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath3D::Create_Plane_Line_2_Line(double *Line1, double *Line2, double *CreatedPlane)
{
		double IntersctPnt[3], PerpndiculrLn[6];
		if(Intersection_Line_Line(Line1, Line2, IntersctPnt))
		{
			Create_Perpendicular_Direction_2_2Directions(&Line1[3], &Line2[3], CreatedPlane);
			CreatedPlane[3] = CreatedPlane[0] * IntersctPnt[0] + CreatedPlane[1] * IntersctPnt[1] + CreatedPlane[2] * IntersctPnt[2];
			return true;
		}
		else if(Checking_Parallel_Line_to_Line(Line1, Line2))
		{
			if(!(Distance_Line_Line(Line1, Line2) == 0))
			{
			   Create_Perpendicular_Line_2_Line(Line1, Line2, PerpndiculrLn);
			   Create_Perpendicular_Direction_2_2Directions(&PerpndiculrLn[3], &Line2[3], CreatedPlane);
			   CreatedPlane[3] = CreatedPlane[0] * Line1[0] + CreatedPlane[1] * Line1[1] + CreatedPlane[2] * Line1[2];
			   return true;
			}
			else
			   return false;
		}
		else
		{
			Create_Perpendicular_Direction_2_2Directions(&Line1[3], &Line2[3], CreatedPlane);
			CreatedPlane[3] = CreatedPlane[0] * Line1[0] + CreatedPlane[1] * Line1[1] + CreatedPlane[2] * Line1[2];
			return true;
		}
		return false;
}

//Get 2 3D points at a given distance from a given point on a given line
bool RapidMath3D::Create_Points_On_Line(double* Line, double* Point, double Dist, double* PointsOnLine)
{
	double RefPtonLine[3] = { 0 };
	//double pDist = Distance_Point_Line(Point, Line);
	//if (pDist > PRECISION)
	Projection_Point_on_Line(Point, Line, RefPtonLine);
	//for a given distance D, we have for a 3D line, x-x0 = l * D
	for (int i = 0; i < 3; i++)
	{
		PointsOnLine[i] = RefPtonLine[i] + Line[3 + i] * Dist;
		PointsOnLine[3 + i] = RefPtonLine[i] - Line[3 + i] * Dist;
	}
	return true;
}



//--------------------------------------------------------------------------
//Other Functions
//--------------------------------------------------------------------------

//Function to calculate the ratio of 3 variables with 2 linear equations in 3 variables given.
void RapidMath3D::SolveEquationRatio(double *eq1, double *eq2, double *solution)
{
	try
	{
		/*
		For the equations
		eq1: a1.x + b1.y + c1.z = 0
		eq2: a2.x + b2.y + c2.z = 0

		the ratios of x, y and z is given by:
			  x               y               z
		------------- = ------------- = -------------
		b1.c2 - b2.c1   c1.a2 - c2.a1   a1.b2 - a2.b1

		Hence x:y:z = The ratio of the dinominators in that order

		*/
	
		for(int i = 0; i < 3; i++)
			solution[i] = eq1[(i + 1) % 3] * eq2[(i + 2) % 3] - eq2[(i + 1) % 3] * eq1[(i + 2) % 3];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0049", __FILE__, __FUNCSIG__); }
}

//function to solve linear equation in 2 variables
void RapidMath3D::SolveEquation(double *eq1, double *eq2, double *solution)
{
	try
	{
		/*
		This function assums the following structure for the 2 equations
		a1.x + b1.y + c1 = 0
		a2.x + b2.y + c2 = 0
		*/
		double tmp1, tmp2, tmp3;

		tmp1 = eq1[0] * eq2[1] - eq2[0] * eq1[1];
		tmp2 = eq1[1] * eq2[2] - eq2[1] * eq1[2];
		tmp3 = eq2[0] * eq1[2] - eq1[0] * eq2[2];

		solution[0] = tmp2 / tmp1;
		solution[1] = tmp3 / tmp1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0050", __FILE__, __FUNCSIG__); }
}

//function to solve linear equation in 3 variables
void RapidMath3D::SolveEquation(double *eq1, double *eq2, double *eq3, double *solution)
{
	try
	{
		/*
		This function assums the following structure for the 2 equations
		a1.x + b1.y + c1.z = d1
		a2.x + b2.y + c2.z = d2
		a3.x + b3.y + c3.z = d3

		it uses Crammer's rule to solve the equations
		By Crammer's rule we have:

				  |a1 b1 c1|		   |d1 b1 c1|			|a1 d1 c1|			 |a1 b1 d1|
		Matrix1 = |a2 b2 c2| Matrix2 = |d2 b2 c2| Matrix3 = |a2 d2 c2| Matrix4 = |a2 b2 d2|
				  |a3 b3 c3|		   |d3 b3 c3|			|a3 d3 c3|			 |a3 b3 d3|
	
			determinant(Matrix2)	  determinant(Matrix3)		determinant(Matrix4)
		x = --------------------, y = --------------------, z = --------------------
			determinant(Matrix1)	  determinant(Matrix1)		determinant(Matrix1)
		*/

		double tmp1, tmp2, tmp3, tmp4, matrx1[9], matrx2[9], matrx3[9], matrx4[9];

		//First get all the matrix filled with same value as matrix1
		for(int i = 0; i < 3; i++)
		{
			matrx1[i] = matrx2[i] = matrx3[i] = matrx4[i] = eq1[i];
			matrx1[3 + i] = matrx2[3 + i] = matrx3[3 + i] = matrx4[3 + i] = eq2[i];
			matrx1[6 + i] = matrx2[6 + i] = matrx3[6 + i] = matrx4[6 + i] = eq3[i];
		}

		//Now fill appropriate columns in appropriate matrices with d1, d2, d3 values.
		matrx2[0] = matrx3[1] = matrx4[2] = eq1[3];
		matrx2[3] = matrx3[4] = matrx4[5] = eq2[3];
		matrx2[6] = matrx3[7] = matrx4[8] = eq3[3];
	
		//Now get the determinant of each matrix
		 tmp1 = Determinant_3x3Matrix(matrx1);
		 tmp2 = Determinant_3x3Matrix(matrx2);
		 tmp3 = Determinant_3x3Matrix(matrx3);
		 tmp4 = Determinant_3x3Matrix(matrx4);

		//Calculate the values of x, y, z
		solution[0] = tmp2 / tmp1;
		solution[1] = tmp3 / tmp1;
		solution[2] = tmp4 / tmp1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0051", __FILE__, __FUNCSIG__); }
}

//function to solve a quadratic equation
int RapidMath3D::Solve_Quadratic_Equation(double *Parameters, double *Answers)
{
	try
	{
		double a = Parameters[0], b = Parameters[1], c = Parameters[2];
		double D = pow(b,2) - 4 * a * c;

		if(D < 0)
			return 0;

		if(D == 0)
		{
			Answers[0] = -b / (2 * a);
			return 1;
		}

		Answers[0] = (-b + sqrt(D)) / (2 * a);
		Answers[1] = (-b - sqrt(D)) / (2 * a);
		return 2;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0052", __FILE__, __FUNCSIG__); return -1; }
}

//Function to find the projection of a point on a line
bool RapidMath3D::Projection_Point_on_Line(double *Point, double *Line, double *PointOfProjectn)
{
	try
	{
		double ratios[3], cosines[3], sum = 0, dist, ProjPtDist;

		for(int i = 0; i < 3; i++)
			ratios[i] = Point[i] - Line[i];
		if(!DirectionCosines(ratios, cosines))
		{
			for(int i = 0; i < 3; i++)
				PointOfProjectn[i] = Point[i];
			return true;
		}

		for(int i = 0; i < 3; i++)
			sum += cosines[i] * Line[3 + i];

		dist = Distance_Point_Point(Point, Line);
		ProjPtDist = dist * sum;

		for(int i = 0; i < 3; i++)
			PointOfProjectn[i] = Line[3 + i] * ProjPtDist + Line[i];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0053", __FILE__, __FUNCSIG__); return false; }
}

//Function to find the projection of a point on a plane
bool RapidMath3D::Projection_Point_on_Plane(double *Point, double *Plane, double *PointOfProjectn)
{
	try
	{
		double PerpendicularLn[6];

		for(int i = 0; i < 3; i++)
		{
			PerpendicularLn[i] = Point[i];
			PerpendicularLn[3 + i] = Plane[i];
		}

		Intersection_Line_Plane(PerpendicularLn, Plane, PointOfProjectn);

		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0054", __FILE__, __FUNCSIG__); return false; }
}

//Function to find the projection of a line on a plane
bool RapidMath3D::Projection_Line_on_Plane(double *Line, double *Plane, double *LineOfProjectn)
{
	try
	{
		double dir[3];

		//Project the line point onto the plane
		Projection_Point_on_Plane(Line, Plane, LineOfProjectn);

		//Find the direction perpendicular to the line and the normal to the plane
		if(!Create_Perpendicular_Direction_2_2Directions(&Line[3], Plane, dir)) return false;
		Create_Perpendicular_Direction_2_2Directions(dir, Plane, &LineOfProjectn[3]);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0054.1", __FILE__, __FUNCSIG__); return false; }
}

//Function to be called for calculating the parameters required for plane to plane angle measurement
void RapidMath3D::Measurement_Angle_Plane_Plane(double *Plane1, double *Plane2, double *MousePt, double *Line1, double *Line2, double* TransMousePnt, double *RotX, double *RotY)
{
	try
	{
		//Calculate the line of intersection of the 2 planes
		double LineOfIntersectn[6];
		Intersection_Plane_Plane(Plane1, Plane2, LineOfIntersectn);

		//Calculate the point of projection of the mouse point on the intersection line (Pt1)
		double Pt1[3];
		Projection_Point_on_Line(MousePt, LineOfIntersectn, Pt1);

		//Calculate the 2 points of projection of the mouse point on the 2 planes (Pt2, Pt3)
		double Pt2[3], Pt3[3];
		Projection_Point_on_Plane(MousePt, Plane1, Pt2);
		Projection_Point_on_Plane(MousePt, Plane2, Pt3);

		//Get the direction cosines of the plane of measurement
		double PlnOfMeasurement[4], Ratios[3];
		SolveEquationRatio(Plane1, Plane2, Ratios);
		DirectionCosines(Ratios, PlnOfMeasurement);

		//Calculate the rotation angles around Y and X axes using these cosines
		double RotatnAngles[2];
		PlaneRotationAngles(PlnOfMeasurement, RotatnAngles);	
		*RotY = RotatnAngles[0];
		*RotX = RotatnAngles[1];

		//Transform the points on to a plane parallel to z=0 plane using the rotation angles.
		double TransMousePt[3], TransPt1[3], TransPt2[3], TransPt3[3], sinXRot, cosXRot, sinYRot, cosYRot;
		sinXRot = sin(RotatnAngles[1]);
		cosXRot = cos(RotatnAngles[1]);
		sinYRot = sin(RotatnAngles[0]);
		cosYRot = cos(RotatnAngles[0]);

		TransMousePt[0] = MousePt[0] * cosYRot + MousePt[2] * sinYRot;
		TransMousePt[1] = MousePt[1] * cosXRot + MousePt[0] * sinYRot * sinXRot - MousePt[2] * cosYRot * sinXRot;
		TransMousePt[2] = MousePt[1] * sinXRot - MousePt[0] * sinYRot * cosXRot + MousePt[2] * cosYRot * cosXRot;

		TransPt1[0] = Pt1[0] * cosYRot + Pt1[2] * sinYRot;
		TransPt1[1] = Pt1[1] * cosXRot + Pt1[0] * sinYRot * sinXRot - Pt1[2] * cosYRot * sinXRot;
		TransPt1[2] = Pt1[1] * sinXRot - Pt1[0] * sinYRot * cosXRot + Pt1[2] * cosYRot * cosXRot;

		TransPt2[0] = Pt2[0] * cosYRot + Pt2[2] * sinYRot;
		TransPt2[1] = Pt2[1] * cosXRot + Pt2[0] * sinYRot * sinXRot - Pt2[2] * cosYRot * sinXRot;
		TransPt2[2] = Pt2[1] * sinXRot - Pt2[0] * sinYRot * cosXRot + Pt2[2] * cosYRot * cosXRot;

		TransPt3[0] = Pt3[0] * cosYRot + Pt3[2] * sinYRot;
		TransPt3[1] = Pt3[1] * cosXRot + Pt3[0] * sinYRot * sinXRot - Pt3[2] * cosYRot * sinXRot;
		TransPt3[2] = Pt3[1] * sinXRot - Pt3[0] * sinYRot * cosXRot + Pt3[2] * cosYRot * cosXRot;

		//Fill up the values into the parameters and return
		for(int i = 0; i < 3; i++)
		{
			Line1[i] = TransPt1[i];
			Line1[3 + i] = TransPt2[i];
			Line2[i] = TransPt1[i];
			Line2[3 + i] = TransPt3[i];
			TransMousePnt[i] = TransMousePt[i];
		}

		//Calculate the necessary parameters for calling the 2D measurement function
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0055", __FILE__, __FUNCSIG__); }
}

//Function that returns the y-roll and x-roll angles(in that order) for a plane to be parallel with the z=0 plane
void RapidMath3D::PlaneRotationAngles(double *Plane, double *RotationAngles)
{
	try
	{
		//Get an arbitrary point(X, Y, Z) on the normal line to the plane
		double Point[3];
		for(int i = 0; i < 3; i++)
			Point[i] = 10 * Plane[i];

		//Get the projection of that point on the XOZ plane as (X1, Y1, Z1)
		double ProjectedPt[3];
		double XOZPlane[] = {0, 1, 0, 0};
		Projection_Point_on_Plane(Point, XOZPlane, ProjectedPt);

		//Get the direction cosine of the line (X1, Y1, Z1) -> (0, 0, 0)
		double DirCosines[3];
		DirectionCosines(ProjectedPt, DirCosines);

		//Use that direction cosine to get the angle that it makes to the Z axis and take it as Phi
		double Phi = acos(DirCosines[2]);

		//get the angle Theta = 90 - acos(m) where m is the y component of the direction cosine of the normal to the plane
		double Theta = M_PI_2 - acos(Plane[1]);

		//Now return the values Phi for Y-roll and Theta for X-roll, the sequence of roatation being in that order.
		RotationAngles[0] = Phi;
		RotationAngles[1] = Theta;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0055", __FILE__, __FUNCSIG__); }
}

//Function to find the points of closest distance on given 2 lines. returns false if lines are parallel
bool RapidMath3D::Points_Of_ClosestDist_Btwn2Lines(double *Line1, double *Line2, double *Points)
{
	try
	{
		/*
		Let the parameters of the 2 lines be
		x1, y1, z1, l1, m1, n1 and x2, y2, z2, l2, m2, n2
		*/
	
		double TmpAng = Angle_Btwn_2Directions(&Line1[3], &Line2[3], false, false);
		if(abs(TmpAng) < ANGLEPRECISION || abs(TmpAng - M_PI) < ANGLEPRECISION) return false;

		//get the term l1.l2 + m1.m2 + n1.n2
		double Term1 = Angle_Btwn_2Directions(&Line1[3], &Line2[3], false, true);
		double Term2 = Line1[3] * (Line1[0] - Line2[0]) +
					   Line1[4] * (Line1[1] - Line2[1]) +
					   Line1[5] * (Line1[2] - Line2[2]);
		double Term3 = Line2[3] * (Line1[0] - Line2[0]) +
					   Line2[4] * (Line1[1] - Line2[1]) +
					   Line2[5] * (Line1[2] - Line2[2]);
		double eq1[3] = {1, -1 * Term1, Term2};
		double eq2[3] = {Term1, -1, Term3};
		double Ans[2];

		SolveEquation(eq1, eq2, Ans);

		Points[0] = Line1[3] * Ans[0] + Line1[0];
		Points[1] = Line1[4] * Ans[0] + Line1[1];
		Points[2] = Line1[5] * Ans[0] + Line1[2];
		Points[3] = Line2[3] * Ans[1] + Line2[0];
		Points[4] = Line2[4] * Ans[1] + Line2[1];
		Points[5] = Line2[5] * Ans[1] + Line2[2];

		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0056", __FILE__, __FUNCSIG__); return false; }
}

//Function to check if a line intersects another line(given only 2 end points of the line)
bool RapidMath3D::Checking_Intersection_2Lines(double *Line1, double *Line2Pts, double *IntersectnPt)
{
	try
	{
		double Line2[6] = {Line2Pts[0], Line2Pts[1], Line2Pts[2]};
		double Ratios[3] = {Line2Pts[0] - Line2Pts[3], Line2Pts[1] - Line2Pts[4], Line2Pts[2] - Line2Pts[5]};
		double TmpIntersectnPt[3] = {0};

		DirectionCosines(Ratios, &Line2[3]);
		if(!Intersection_Line_Line(Line1, Line2, TmpIntersectnPt)) return false;
	
		if(Checking_Point_within_Block(TmpIntersectnPt, Line2Pts, 2))
		{
			IntersectnPt[0] = TmpIntersectnPt[0];
			IntersectnPt[1] = TmpIntersectnPt[1];
			IntersectnPt[2] = TmpIntersectnPt[2];
			return true;
		}
		else
			return false;
	}catch(...){ SetAndRaiseErrorMessage("RMATH3D0057", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath3D::Checking_PointIsInTriangle(double *point, double *Vertex1, double *Vertex2, double *Vertex3)
{
	try
	{
		double dir1[3] = {Vertex1[0] - Vertex2[0], Vertex1[1] - Vertex2[1], Vertex1[2] - Vertex2[2]};
		double dir2[3] = {Vertex3[0] - Vertex2[0], Vertex3[1] - Vertex2[1], Vertex3[2] - Vertex2[2]};
		double PtDir[3] = {point[0] - Vertex2[0], point[1] - Vertex2[1], point[2] - Vertex2[2]};
		if(abs(PtDir[0]) < PRECISION && abs(PtDir[1]) < PRECISION && abs(PtDir[2]) < PRECISION)
			return true;
		if(CheckVectorBtwn2Vectors(dir1, dir2, PtDir, true, true))
		{
			dir1[0] = Vertex2[0] - Vertex3[0]; dir1[1] = Vertex2[1] - Vertex3[1]; dir1[2] = Vertex2[2] - Vertex3[2];
			dir2[0] = Vertex1[0] - Vertex3[0]; dir2[1] = Vertex1[1] - Vertex3[1]; dir2[2] = Vertex1[2] - Vertex3[2];
			PtDir[0] = point[0] - Vertex3[0]; PtDir[1] = point[1] - Vertex3[1]; PtDir[2] = point[2] - Vertex3[2];
			if(abs(PtDir[0]) < PRECISION && abs(PtDir[1]) < PRECISION && abs(PtDir[2]) < PRECISION)
				return true;
			if(CheckVectorBtwn2Vectors(dir1, dir2, PtDir, true, true))
			{
				dir1[0] = Vertex3[0] - Vertex1[0]; dir1[1] = Vertex3[1] - Vertex1[1]; dir1[2] = Vertex3[2] - Vertex1[2];
				dir2[0] = Vertex2[0] - Vertex1[0]; dir2[1] = Vertex2[1] - Vertex1[1]; dir2[2] = Vertex2[2] - Vertex1[2];
				PtDir[0] = point[0] - Vertex1[0]; PtDir[1] = point[1] - Vertex1[1]; PtDir[2] = point[2] - Vertex1[2];
				if(abs(PtDir[0]) < PRECISION && abs(PtDir[1]) < PRECISION && abs(PtDir[2]) < PRECISION)
					return true;
				if(CheckVectorBtwn2Vectors(dir1, dir2, PtDir, true, true))
					return true;
				else 
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0058", __FILE__, __FUNCSIG__); return false; }
}

//Function to calculate the standard form of a line given its 2 end points, returns false if both points are same
bool RapidMath3D::Calculate_StandardForm_Line(double *LineEndPt1, double *LineEndPt2, double *LineStdForm)
{
	try
	{
		double Ratio[3] = {LineEndPt1[0] - LineEndPt2[0], LineEndPt1[1] - LineEndPt2[1], LineEndPt1[2] - LineEndPt2[2]};

		LineStdForm[0] = LineEndPt1[0];
		LineStdForm[1] = LineEndPt1[1];
		LineStdForm[2] = LineEndPt1[2];
		return DirectionCosines(Ratio, &LineStdForm[3]);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0058", __FILE__, __FUNCSIG__); return false; }
}

//Function to calculate determinant and inverse of a square matrix//
double RapidMath3D::OperateMatrix4X4(double* matrix, int Size, int Flag, double* Answer)
{
	try
	{
		//RowEchelon will be the local matrix on which row reduction will be done to get
		//Upper Diagonal matrix
		double* RowEchelon =(double*) malloc(sizeof(double) * Size * Size);
		//Inverse will be the inverse of the matrix, if required; 
		double* Inverse =(double*) malloc(sizeof(double) * Size * Size);
		int i = 0, j = 0; 
		int FlipCount = 0; //Keeps a count of the number of pivot steps that have been done 
		double max; int pivotRow = 0, currentRow; 
		double Determinant, Eliminator;
		//Copy all the elements of Matrix to RowEchelon
		memcpy((void*)RowEchelon, (void*)matrix , sizeof(double) * Size * Size);
		//For computing the inverse, initialise the inverse as the unit matrix
		for (j = 0; j < Size; j++)	
		{	for (i = 0; i < Size; i ++)
			{	if (i == j) *(Inverse + j * Size + i) = 1; else *(Inverse + j * Size + i) = 0;
			}
		}

		//Loop through all the columns
		for (currentRow = 0; currentRow < Size - 1; currentRow ++)
		{	max = abs(*(RowEchelon + currentRow * (Size + 1)));
			//Find out the largest member of the current column
			for (i = currentRow + 1; i < Size; i ++) 
			{	if (abs(*(RowEchelon + i * Size + currentRow)) > max) {max = abs(*(RowEchelon + i * Size + currentRow)); pivotRow = i;}
			}
			//If all the entries below the current row are 0, then we determinant 0 and non-invertible matrix!
			if (max == 0) continue;
			
			//Now flip currentRow and pivotRow, so that we can get an upper triangular matrix without awkward 0 in the diagonal
			if (pivotRow > currentRow) 
			{	FlipCount += 1;
				double Temp = 0.0;
				for (i = 0; i < Size; i ++)
				{	//Do it for the RowEchelon Matrix
					Temp = *(RowEchelon + currentRow * Size + i);
					*(RowEchelon + currentRow * Size + i) = *(RowEchelon + pivotRow * Size + i);
					*(RowEchelon + pivotRow * Size + i) = Temp;
					//Repeat for the Inverse Matrix
					Temp = *(Inverse + currentRow * Size + i);
					*(Inverse + currentRow * Size + i) = *(Inverse + pivotRow * Size + i);
					*(Inverse + pivotRow * Size + i) = Temp;
				}
			}
			//Eliminate to zeroes all the fellows below the currentrow in the current column
			//if (currentRow == 0) continue; 
			for (i = currentRow + 1; i < Size; i ++)
			{	Eliminator = *(RowEchelon + i * Size + currentRow) / *(RowEchelon + currentRow * (Size + 1));
				for (j = 0; j < Size; j ++)
				{	*(RowEchelon + i * Size + j) -= *(RowEchelon + currentRow * Size + j) * Eliminator;
					*(Inverse + i * Size + j) -= *(Inverse + currentRow * Size + j) * Eliminator;
				}
			}
		}
		//Finished with an upper diagonal matrix. 
		//Calculate the Determinant
		Determinant = 1;
		for (i = 0; i < Size; i ++)
		{	Determinant *= *(RowEchelon + i * (Size + 1));
		}
		Determinant *= pow(-1.0, FlipCount);
		//If inverse not required, and only determinant is required, 
		//fill answer with row echelon for linear solver, and return determinant
		//if (Flag == 0) //Require only determinant optional filling of upper diag matrix
		//{	
		//	//memcpy((void*)Answer, (void*)&Determinant, sizeof(double) * Size * Size);
		//}
		if (Flag == 1) //Require Inverse
		{	for (currentRow = 0; currentRow < Size; currentRow ++)
			{	//Process until RowEchelon becomes identity matrix
				//Make the diagonal element one, by dividing the row by the diagonal element of that row! 
				Eliminator = *(RowEchelon + currentRow * (Size + 1));
				for (j = 0; j < Size; j ++)
				{	*(RowEchelon + currentRow * Size + j) /= Eliminator;
					*(Inverse + currentRow * Size + j) /= Eliminator;
				}
			}
			for (currentRow = 0; currentRow < Size; currentRow ++)
			{	//Eliminate column-wise all elements until we get zeroes simply subtract 
				// To do this, for column no. i, get row number i (which will be all zero until this column!) and eliminate!
				for (i = currentRow + 1; i < Size; i ++)
				{	Eliminator = *(RowEchelon + currentRow * Size + i);	
					for (j = 0; j < Size; j ++)
					{	*(RowEchelon + currentRow * Size + j) -= *(RowEchelon + i * Size + j) * Eliminator;
						*(Inverse + currentRow * Size + j) -= *(Inverse + i * Size + j) * Eliminator;
					}
				}
			}
			memcpy((void*)Answer, (void*)Inverse, sizeof(double) * Size * Size);
		}
		free(RowEchelon); free(Inverse);
		return Determinant;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0059", __FILE__, __FUNCSIG__); return 0; }
}

//Function to round off double values to specified no of decimal precision
double RapidMath3D::RoundDecimal(double valu, int decPlace)
{
	try
	{
		double power = int(pow(10.0,decPlace));
		double temp1 = int(valu * power) / power;
		return temp1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0060", __FILE__, __FUNCSIG__); return 0; }
}



//--------------------------------------------------------------------------------------
//Measurement functions to calculate transformation matrix for each kind of measurement
//--------------------------------------------------------------------------------------

//Given a plane, take a random line on plane as x-axis and get the transformation matrix for the plane
void RapidMath3D::GetTMatrixForPlane(double *Plane, double *TransMatrix)
{
	try
	{
		  double RandomPt1[3] = {500, 0, 0}, RandomPt2[3] = {-500, 0, 0};
		  double ProjectedPt1[3], ProjectedPt2[3];

		  if(abs(Plane[1]) < PRECISION && abs(Plane[2]) < PRECISION)
			  RandomPt2[2] = 100;

		  Projection_Point_on_Plane(RandomPt1, Plane, ProjectedPt1);
		  Projection_Point_on_Plane(RandomPt2, Plane, ProjectedPt2);

		  double ratio[3] = {ProjectedPt1[0] - ProjectedPt2[0], ProjectedPt1[1] - ProjectedPt2[1], ProjectedPt1[2] - ProjectedPt2[2]};
		  DirectionCosines(ratio, TransMatrix);

		  Create_Perpendicular_Direction_2_2Directions(TransMatrix, Plane, TransMatrix + 3);
		  for(int i = 0; i < 3; i++)
				TransMatrix[6 + i] = Plane[i];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0061", __FILE__, __FUNCSIG__); }
}

//Point to point distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pt2Pt(double *Point1, double *Point2, double *Matrix, double *MeasurementPlane)
{
	try
	{
		double Dir1[3] = {Point2[0] - Point1[0], Point2[1] - Point1[1], Point2[2] - Point1[2]};
		double Dir2[3] = {1, 0, 0};
		double PerpendDir[3];

		if(Create_Perpendicular_Direction_2_2Directions(Dir1, Dir2, PerpendDir))
		{
			double YDir[3];
			Create_Perpendicular_Direction_2_2Directions(Dir2, PerpendDir, YDir);

			Matrix[0] = Dir2[0];		Matrix[1] = Dir2[1];		Matrix[2] = Dir2[2];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = PerpendDir[0];	Matrix[7] = PerpendDir[1];	Matrix[8] = PerpendDir[2];
		}
		else
		{
			Matrix[0] = 1;	Matrix[1] = 0;	Matrix[2] = 0;
			Matrix[3] = 0;	Matrix[4] = 1;	Matrix[5] = 0;
			Matrix[6] = 0;	Matrix[7] = 0;	Matrix[8] = 1;
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Point1[0] * Matrix[6] + Point1[1] * Matrix[7] + Point1[2] * Matrix[8];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0062", __FILE__, __FUNCSIG__); }
}

//Point to Line distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pt2Ln(double *Point, double *Line, double *Matrix, double *MeasurementPlane)
{
	try
	{
		double Dir1[3] = {Line[3], Line[4], Line[5]};
		double YLine[6];
		double PerpendDir[3];

		Create_Perpendicular_Line_2_Line(Point, Line, YLine);
		Create_Perpendicular_Direction_2_2Directions(Dir1, &YLine[3], PerpendDir);

		Matrix[0] = Dir1[0];		Matrix[1] = Dir1[1];		Matrix[2] = Dir1[2];
		Matrix[3] = YLine[3];		Matrix[4] = YLine[4];		Matrix[5] = YLine[5];
		Matrix[6] = PerpendDir[0];	Matrix[7] = PerpendDir[1];	Matrix[8] = PerpendDir[2];

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Point[0] * Matrix[6] + Point[1] * Matrix[7] + Point[2] * Matrix[8];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0063", __FILE__, __FUNCSIG__); }
}

//Point to Plane distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pt2Pln(double *Point, double *Plane, double *PlnMidPt, double *Matrix, double *MeasurementPlane)
{
	try
	{
		double ProjectedPt[3], XDir[3], ZDir[3], Ratios[3] = {Point[0] - PlnMidPt[0], Point[1] - PlnMidPt[1], Point[2] - PlnMidPt[2]};

		if(Create_Perpendicular_Direction_2_2Directions(Ratios, Plane, ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(ZDir, Plane, XDir);

			Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
			Matrix[3] = Plane[0];		Matrix[4] = Plane[1];		Matrix[5] = Plane[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		{
			Projection_Point_on_Plane(Point, Plane, ProjectedPt);
			TransformationMatrix_Dist_Pt2Pt(Point, ProjectedPt, Matrix, MeasurementPlane);
		}
	
		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Point[0] * Matrix[6] + Point[1] * Matrix[7] + Point[2] * Matrix[8];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0064", __FILE__, __FUNCSIG__); }
}

//Point to Sphere distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pt2Spr(double *Point, double *Sphere, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Pt2Pt(Point, Sphere, Matrix, MeasurementPlane);
}

//Point to Cylinder distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pt2Cylndr(double *Point, double *Cylinder, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Pt2Ln(Point, Cylinder, Matrix, MeasurementPlane);
}

//Line to Line distance measurement
void RapidMath3D::TransformationMatrix_Dist_Ln2Ln(double *Line1, double *Line2, double *Matrix, double *MeasurementPlane)
{
	try
	{
		double Points[6];

		if(Points_Of_ClosestDist_Btwn2Lines(Line1, Line2, Points))
		{
			TransformationMatrix_Dist_Pt2Pt(&Points[0], &Points[3], Matrix, MeasurementPlane);
		}
		else
		{
			double Dir1[3] = {Line2[0] - Line1[0], Line2[1] - Line1[1], Line2[2] - Line1[2]};
			double Dir2[3] = {Line1[3], Line1[4], Line1[5]};
			double PerpendDir[3];

			if(Create_Perpendicular_Direction_2_2Directions(Dir1, Dir2, PerpendDir))
			{
				double YDir[3];
				Create_Perpendicular_Direction_2_2Directions(Dir2, PerpendDir, YDir);

				Matrix[0] = Dir2[0];		Matrix[1] = Dir2[1];		Matrix[2] = Dir2[2];
				Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
				Matrix[6] = PerpendDir[0];	Matrix[7] = PerpendDir[1];	Matrix[8] = PerpendDir[2];
			}
			else
			{
				Matrix[0] = 1;	Matrix[1] = 0;	Matrix[2] = 0;
				Matrix[3] = 0;	Matrix[4] = 1;	Matrix[5] = 0;
				Matrix[6] = 0;	Matrix[7] = 0;	Matrix[8] = 1;
			}

			MeasurementPlane[0] = Matrix[6];
			MeasurementPlane[1] = Matrix[7];
			MeasurementPlane[2] = Matrix[8];
			MeasurementPlane[3] = Line1[0] * Matrix[6] + Line1[1] * Matrix[7] + Line1[2] * Matrix[8];
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0065", __FILE__, __FUNCSIG__); }
}

//Line to Plane distance measurement
void RapidMath3D::TransformationMatrix_Dist_Ln2Pln(double *Line, double *Plane, double *PlanePts, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	try
	{
		double ZDir[3], XDir[3] = {1, 0, 0}, YDir[3];
		//to find a plane that is perpendicular to the given plane and containing the given line
		if(Create_Perpendicular_Direction_2_2Directions(&Line[3], Plane, ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(&Line[3], ZDir, YDir);
		
			Matrix[0] = Line[3];		Matrix[1] = Line[4];		Matrix[2] = Line[5];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		//if the line is perpendicular to the plane, we have a problem in fixing the measurement plane
		{
			if(Create_Perpendicular_Direction_2_2Directions(&Line[3], XDir, ZDir))
			{
				Create_Perpendicular_Direction_2_2Directions(XDir, ZDir, YDir);

				Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
				Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
				Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
			}
			else
			{
				Matrix[0] = 1;		Matrix[1] = 0;		Matrix[2] = 0;
				Matrix[3] = 0;		Matrix[4] = 1;		Matrix[5] = 0;
				Matrix[6] = 0;		Matrix[7] = 0;		Matrix[8] = 1;
			}
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Line[0] * Matrix[6] + Line[1] * Matrix[7] + Line[2] * Matrix[8];

		//Now check if the projection of the line of plane cuts across the boundaries of the finite plane
		double IntersectnLn[6], PlnEdgePoints[6] = {PlanePts[0], PlanePts[1], PlanePts[2], PlanePts[9], PlanePts[10], PlanePts[11]};
		double IntersectnPts[3];
		bool IntersectsPlane = false;
	
		Intersection_Plane_Plane(Plane, MeasurementPlane, IntersectnLn);
	
		if(Checking_Intersection_2Lines(IntersectnLn, &PlanePts[0], &IntersectnPts[0]))
			IntersectsPlane = true;
		else if(Checking_Intersection_2Lines(IntersectnLn, &PlanePts[3], &IntersectnPts[0]))
			IntersectsPlane = true;
		else if(Checking_Intersection_2Lines(IntersectnLn, &PlanePts[6], &IntersectnPts[0]))
			IntersectsPlane = true;
		else if(Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[0]))
			IntersectsPlane = true;

		if(!IntersectsPlane)
			MeasurementPlane[3] = (PlanePts[0] + PlanePts[6]) / 2 * Matrix[6] + (PlanePts[1] + PlanePts[7]) / 2 * Matrix[7] + (PlanePts[2] + PlanePts[8]) / 2 * Matrix[8];
		*ProjectLine = !IntersectsPlane;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0066", __FILE__, __FUNCSIG__); }
}
//Line to Circle3D distance measurement
void RapidMath3D::TransformationMatrix_Dist_Ln2Cir3D(double *Line, double *Circle3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	try
	{
		double ZDir[3], XDir[3] = {1, 0, 0}, YDir[3];
		//to find a plane that is perpendicular to the given plane and containing the given line
		if(Create_Perpendicular_Direction_2_2Directions(&Line[3], &Circle3D[3], ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(&Line[3], ZDir, YDir);
		
			Matrix[0] = Line[3];		Matrix[1] = Line[4];		Matrix[2] = Line[5];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		//if the line is perpendicular to the plane, we have a problem in fixing the measurement plane
		{
			if(Create_Perpendicular_Direction_2_2Directions(&Line[3], XDir, ZDir))
			{
				Create_Perpendicular_Direction_2_2Directions(XDir, ZDir, YDir);

				Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
				Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
				Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
			}
			else
			{
				Matrix[0] = 1;		Matrix[1] = 0;		Matrix[2] = 0;
				Matrix[3] = 0;		Matrix[4] = 1;		Matrix[5] = 0;
				Matrix[6] = 0;		Matrix[7] = 0;		Matrix[8] = 1;
			}
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Line[0] * Matrix[6] + Line[1] * Matrix[7] + Line[2] * Matrix[8];

		//Now check if the projection of the line of plane cuts across the boundaries of the finite plane
		double IntersectnLn[6];
		double IntersectnPts[6];
		bool IntersectsPlane = false;
		double Circle3DAsSphere[4] = {Circle3D[0], Circle3D[1], Circle3D[2], Circle3D[6]};
		double Circle3DAsPln[4] = {Circle3D[3], Circle3D[4], Circle3D[5], Circle3D[0] * Circle3D[3] + Circle3D[1] * Circle3D[4] + Circle3D[2] * Circle3D[5]};
		Intersection_Plane_Plane(&Circle3DAsPln[0], MeasurementPlane, IntersectnLn);
		int CntIntrsctn =  Intersection_Line_Sphere(IntersectnLn, Circle3DAsSphere, IntersectnPts);
		if (CntIntrsctn == 2)
		IntersectsPlane = true;

		if(!IntersectsPlane)
			MeasurementPlane[3] = Circle3D[0] * Matrix[6] + Circle3D[1] * Matrix[7] + Circle3D[2] * Matrix[8];
		*ProjectLine = !IntersectsPlane;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0066", __FILE__, __FUNCSIG__); }
}

//Line to Sphere distance measurement
void RapidMath3D::TransformationMatrix_Dist_Ln2Spr(double *Line, double *Sphere, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Pt2Ln(Sphere, Line, Matrix, MeasurementPlane);
}

//Line to Cylinder distance measurement
void RapidMath3D::TransformationMatrix_Dist_Ln2Cylndr(double *Line, double *Cylinder, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Ln2Ln(Line, Cylinder, Matrix, MeasurementPlane);
}

//Plane to Plane distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pln2Pln(double *Plane1, double *Plane2, double *Pln1MidPt, double *Pln2MidPt, double *Matrix, double *MeasurementPlane)
{
	try
	{
		double YDir[3] = {Plane1[0], Plane1[1], Plane1[2]};
		double Dir1[3] = {Pln1MidPt[0] - Pln2MidPt[0], Pln1MidPt[1] - Pln2MidPt[1], Pln1MidPt[2] - Pln2MidPt[2]};
		double ZDir[3];
		double XDir[3];

		if(Create_Perpendicular_Direction_2_2Directions(Dir1, YDir, ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(YDir, ZDir, XDir);

			Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		{
			XDir[0] = 1; XDir[1] = 0; XDir[2] = 0;

			if(Create_Perpendicular_Direction_2_2Directions(XDir, Plane1, ZDir))
			{
				Create_Perpendicular_Direction_2_2Directions(XDir, ZDir, YDir);

				Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
				Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
				Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
			}
			else
			{
				Matrix[0] = 1;	Matrix[1] = 0;	Matrix[2] = 0;
				Matrix[3] = 0;	Matrix[4] = 1;	Matrix[5] = 0;
				Matrix[6] = 0;	Matrix[7] = 0;	Matrix[8] = 1;
			}
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Pln1MidPt[0] * Matrix[6] + Pln1MidPt[1] * Matrix[7] + Pln1MidPt[2] * Matrix[8];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0067", __FILE__, __FUNCSIG__); }
}

//Plane to Sphere distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pln2Spr(double *Plane, double *Sphere, double *PlnMidPt, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Pt2Pln(Sphere, Plane, PlnMidPt, Matrix, MeasurementPlane);
}

//Plane to Cylinder distance measurement
void RapidMath3D::TransformationMatrix_Dist_Pln2Cylndr(double *Plane, double *PlanePts, double *Cylinder, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	TransformationMatrix_Dist_Ln2Pln(Cylinder, Plane, PlanePts, Matrix, MeasurementPlane, ProjectLine);
}

//Sphere to Sphere distance measurement
void RapidMath3D::TransformationMatrix_Dist_Spr2Spr(double *Sphere1, double *Sphere2, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Pt2Pt(Sphere1, Sphere2, Matrix, MeasurementPlane);
}

//Sphere to Cylinder distance measurement
void RapidMath3D::TransformationMatrix_Dist_Spr2Cylndr(double *Sphere, double *Cylinder, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Pt2Ln(Sphere, Cylinder, Matrix, MeasurementPlane);
}

//Cylinder to Cylinder distance measurement
void RapidMath3D::TransformationMatrix_Dist_Cylndr2Cylndr(double *Cylinder1, double *Cylinder2, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Dist_Ln2Ln(Cylinder1, Cylinder2, Matrix, MeasurementPlane);
}

//Line to Line angle measurement
void RapidMath3D::TransformationMatrix_Angle_Ln2Ln(double *Line1, double *Line2, double *Matrix, double *MeasurementPlane)
{
	try
	{
		double ZDir[3], YDir[3];
	/*	if (Checking_Parallel_Line_to_Line(&Line1[0], &Line2[0]))
		{
					TransformationMatrix_Dist_Pt2Pt(&Line1[0], &Line2[0], &Matrix[0], &MeasurementPlane[0]);
		}
		else
		{*/
		Create_Perpendicular_Direction_2_2Directions(&Line1[3], &Line2[3], ZDir);
		Create_Perpendicular_Direction_2_2Directions(&Line1[3], ZDir, YDir);

		Matrix[0] = Line1[3];		Matrix[1] = Line1[4];		Matrix[2] = Line1[5];
		Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
		Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Line1[0] * Matrix[6] + Line1[1] * Matrix[7] + Line1[2] * Matrix[8];
	/*	}*/
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0068", __FILE__, __FUNCSIG__); }
}

//Line to Plane angle measurement
void RapidMath3D::TransformationMatrix_Angle_Ln2Pln(double *Line, double *Plane, double *PlanePts, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	try
	{
		double ZDir[3] = {0,0,1}, YDir[3] = {0,0,1}, PlnMidPt[3] = {(PlanePts[0] + PlanePts[6]) / 2, (PlanePts[1] + PlanePts[7]) / 2, (PlanePts[2] + PlanePts[8]) / 2};
		if(Create_Perpendicular_Direction_2_2Directions(&Line[3], Plane, ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(&Line[3], ZDir, YDir);

			Matrix[0] = Line[3];		Matrix[1] = Line[4];		Matrix[2] = Line[5];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		{
			if(Distance_Point_Line(PlnMidPt, Line) != 0)
			{
				TransformationMatrix_Dist_Pt2Ln(PlnMidPt, Line, Matrix, MeasurementPlane);
			}
			else
			{
				double XDir[3] = {1, 0, 0};

				if(Create_Perpendicular_Direction_2_2Directions(&Line[3], XDir, ZDir))
				{
					Create_Perpendicular_Direction_2_2Directions(XDir, ZDir, YDir);

					Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
					Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
					Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
				}
				else
				{
					Matrix[0] = 1;	Matrix[1] = 0;	Matrix[2] = 0;
					Matrix[3] = 0;	Matrix[4] = 1;	Matrix[5] = 0;
					Matrix[6] = 0;	Matrix[7] = 0;	Matrix[8] = 1;
				}
			}
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Line[0] * Matrix[6] + Line[1] * Matrix[7] + Line[2] * Matrix[8];

		//Now check if the projection of the line of plane cuts across the boundaries of the finite plane
		double IntersectnLn[6] = {0, 0, 0, 0, 0, 1}, PlnEdgePoints[6] = {PlanePts[0], PlanePts[1], PlanePts[2], PlanePts[9], PlanePts[10], PlanePts[11]};
		double IntersectnPts[3] = {0};
		bool IntersectsPlane = false;
	
		Intersection_Plane_Plane(Plane, MeasurementPlane, IntersectnLn);
	
		if(Checking_Intersection_2Lines(IntersectnLn, &PlanePts[0], &IntersectnPts[0]))
			IntersectsPlane = true;
		else if(Checking_Intersection_2Lines(IntersectnLn, &PlanePts[3], &IntersectnPts[0]))
			IntersectsPlane = true;
		else if(Checking_Intersection_2Lines(IntersectnLn, &PlanePts[6], &IntersectnPts[0]))
			IntersectsPlane = true;
		else if(Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[0]))
			IntersectsPlane = true;

		if(!IntersectsPlane)
			MeasurementPlane[3] = (PlanePts[0] + PlanePts[6]) / 2 * Matrix[6] + (PlanePts[1] + PlanePts[7]) / 2 * Matrix[7] + (PlanePts[2] + PlanePts[8]) / 2 * Matrix[8];
		*ProjectLine = !IntersectsPlane;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0069", __FILE__, __FUNCSIG__); }
}
//Line to Cicle3D angle measurement
void RapidMath3D::TransformationMatrix_Angle_Ln2Cir3D(double *Line, double *Circle3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	try
	{
		double ZDir[3], YDir[3], MidPt[3] = {Circle3D[0], Circle3D[1], Circle3D[2]};
		if(Create_Perpendicular_Direction_2_2Directions(&Line[3], &Circle3D[3], ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(&Line[3], ZDir, YDir);

			Matrix[0] = Line[3];		Matrix[1] = Line[4];		Matrix[2] = Line[5];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		{
			if(Distance_Point_Line(MidPt, Line) != 0)
			{
				TransformationMatrix_Dist_Pt2Ln(MidPt, Line, Matrix, MeasurementPlane);
			}
			else
			{
				double XDir[3] = {1, 0, 0};

				if(Create_Perpendicular_Direction_2_2Directions(&Line[3], XDir, ZDir))
				{
					Create_Perpendicular_Direction_2_2Directions(XDir, ZDir, YDir);

					Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
					Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
					Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
				}
				else
				{
					Matrix[0] = 1;	Matrix[1] = 0;	Matrix[2] = 0;
					Matrix[3] = 0;	Matrix[4] = 1;	Matrix[5] = 0;
					Matrix[6] = 0;	Matrix[7] = 0;	Matrix[8] = 1;
				}
			}
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Line[0] * Matrix[6] + Line[1] * Matrix[7] + Line[2] * Matrix[8];

		//Now check if the projection of the line of plane cuts across the boundaries of the finite plane
		double IntersectnLn[6];
		double IntersectnPts[6];
		bool IntersectsPlane = false;
		double CirclePln[4] = {Circle3D[3], Circle3D[4], Circle3D[5], (Circle3D[0]*Circle3D[3] + Circle3D[1]*Circle3D[4] + Circle3D[2]*Circle3D[5])};
		double Circle3DAsSphere[4] = {Circle3D[0], Circle3D[1], Circle3D[2], Circle3D[6]};
		Intersection_Plane_Plane(CirclePln, MeasurementPlane, IntersectnLn);
		int CntIntrsctn =  Intersection_Line_Sphere(IntersectnLn, Circle3DAsSphere, IntersectnPts);
		if (CntIntrsctn == 2)
		IntersectsPlane = true;
		if(!IntersectsPlane)
			MeasurementPlane[3] = Circle3D[0] * Matrix[6] + Circle3D[1] * Matrix[7] + Circle3D[2] * Matrix[8];
		*ProjectLine = !IntersectsPlane;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0069", __FILE__, __FUNCSIG__); }
}

void RapidMath3D::TransformationMatrix_Dist_Ln2Conics3D(double *Line, double *Conics3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	try
	{
		double ZDir[3], XDir[3] = {1, 0, 0}, YDir[3];
		//to find a plane that is perpendicular to the given plane and containing the given line
		if(Create_Perpendicular_Direction_2_2Directions(&Line[3], &Conics3D[0], ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(&Line[3], ZDir, YDir);
		
			Matrix[0] = Line[3];		Matrix[1] = Line[4];		Matrix[2] = Line[5];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		//if the line is perpendicular to the plane, we have a problem in fixing the measurement plane
		{
			if(Create_Perpendicular_Direction_2_2Directions(&Line[3], XDir, ZDir))
			{
				Create_Perpendicular_Direction_2_2Directions(XDir, ZDir, YDir);

				Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
				Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
				Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
			}
			else
			{
				Matrix[0] = 1;		Matrix[1] = 0;		Matrix[2] = 0;
				Matrix[3] = 0;		Matrix[4] = 1;		Matrix[5] = 0;
				Matrix[6] = 0;		Matrix[7] = 0;		Matrix[8] = 1;
			}
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Line[0] * Matrix[6] + Line[1] * Matrix[7] + Line[2] * Matrix[8];

		//Now check if the projection of the line of plane cuts across the boundaries of the finite plane
		double IntersectnLn[6];
		double IntersectnPts[6];
		bool IntersectsPlane = false;
		Intersection_Plane_Plane(&Conics3D[0], MeasurementPlane, IntersectnLn);
		int CntIntrsctn =  Intersection_Line_elipse(IntersectnLn, Conics3D, IntersectnPts);
		if (CntIntrsctn == 2)
		IntersectsPlane = true;

		if(!IntersectsPlane)
			MeasurementPlane[3] = Conics3D[4] * Matrix[6] + Conics3D[5] * Matrix[7] + Conics3D[6] * Matrix[8];
		*ProjectLine = !IntersectsPlane;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0066", __FILE__, __FUNCSIG__); }
}

void RapidMath3D::TransformationMatrix_Angle_Ln2Conics3D(double *Line, double *Conics3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	try
	{
		double dir[6], Cen[3];
		double Max = (2 * Conics3D[13] *Conics3D[16])/(1 - Conics3D[13] * Conics3D[13]);
		Create_Perpendicular_Line_2_Line(&Conics3D[4], &Conics3D[7], &dir[0]);
		double focallgt = (Max/2) - (Conics3D[13] *Conics3D[16])/(1 + Conics3D[13]);
		for(int i = 0; i < 3; i++) Cen[i] = Conics3D[4 + i] + focallgt * dir[3 + i];
		double ZDir[3], YDir[3];
		if(Create_Perpendicular_Direction_2_2Directions(&Line[3], &Conics3D[0], ZDir))
		{
			Create_Perpendicular_Direction_2_2Directions(&Line[3], ZDir, YDir);

			Matrix[0] = Line[3];		Matrix[1] = Line[4];		Matrix[2] = Line[5];
			Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
			Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
		}
		else
		{
			if(Distance_Point_Line(Cen, Line) != 0)
			{
				TransformationMatrix_Dist_Pt2Ln(Cen, Line, Matrix, MeasurementPlane);
			}
			else
			{
				double XDir[3] = {1, 0, 0};

				if(Create_Perpendicular_Direction_2_2Directions(&Line[3], XDir, ZDir))
				{
					Create_Perpendicular_Direction_2_2Directions(XDir, ZDir, YDir);

					Matrix[0] = XDir[0];		Matrix[1] = XDir[1];		Matrix[2] = XDir[2];
					Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
					Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];
				}
				else
				{
					Matrix[0] = 1;	Matrix[1] = 0;	Matrix[2] = 0;
					Matrix[3] = 0;	Matrix[4] = 1;	Matrix[5] = 0;
					Matrix[6] = 0;	Matrix[7] = 0;	Matrix[8] = 1;
				}
			}
		}

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = Line[0] * Matrix[6] + Line[1] * Matrix[7] + Line[2] * Matrix[8];

		//Now check if the projection of the line of plane cuts across the boundaries of the finite plane
		double IntersectnLn[6];
		double IntersectnPts[6];
		bool IntersectsPlane = false;
		Intersection_Plane_Plane(Conics3D, MeasurementPlane, IntersectnLn);
		int CntIntrsctn =  Intersection_Line_elipse(IntersectnLn, Conics3D, IntersectnPts);
		if (CntIntrsctn == 2)
		IntersectsPlane = true;
		if(!IntersectsPlane)
			MeasurementPlane[3] = Cen[0] * Matrix[6] + Cen[1] * Matrix[7] + Cen[2] * Matrix[8];
		*ProjectLine = !IntersectsPlane;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0069", __FILE__, __FUNCSIG__); }
}

//Line to Cylinder angle measurement
void RapidMath3D::TransformationMatrix_Angle_Ln2Cylndr(double *Line, double *Cylinder, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Angle_Ln2Ln(Line, Cylinder, Matrix, MeasurementPlane);
}

//Plane to Plane angle measurement
void RapidMath3D::TransformationMatrix_Angle_Pln2Pln(double *Plane1, double *Plane2, double *PtOfRef, double *Matrix, double *MeasurementPlane)
{
	try
	{
		double YDir[3], ZDir[3];

		Create_Perpendicular_Direction_2_2Directions(Plane1, Plane2, ZDir);
		Create_Perpendicular_Direction_2_2Directions(Plane1, ZDir, YDir);

		Matrix[0] = Plane1[0];		Matrix[1] = Plane1[1];		Matrix[2] = Plane1[2];
		Matrix[3] = YDir[0];		Matrix[4] = YDir[1];		Matrix[5] = YDir[2];
		Matrix[6] = ZDir[0];		Matrix[7] = ZDir[1];		Matrix[8] = ZDir[2];

		MeasurementPlane[0] = Matrix[6];
		MeasurementPlane[1] = Matrix[7];
		MeasurementPlane[2] = Matrix[8];
		MeasurementPlane[3] = PtOfRef[0] * Matrix[6] + PtOfRef[1] * Matrix[7] + PtOfRef[2] * Matrix[8];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0070", __FILE__, __FUNCSIG__); }
}

//Plane to Cylinder angle measurement
void RapidMath3D::TransformationMatrix_Angle_Pln2Cylndr(double *Plane, double *PlanePts, double *Cylinder, double *Matrix, double *MeasurementPlane, bool *ProjectLine)
{
	TransformationMatrix_Angle_Ln2Pln(Cylinder, Plane, PlanePts, Matrix, MeasurementPlane, ProjectLine);
}

//Cylinder to Cylinder angle measurement
void RapidMath3D::TransformationMatrix_Angle_Cylndr2Cylndr(double *Cylinder1, double *Cylinder2, double *Matrix, double *MeasurementPlane)
{
	TransformationMatrix_Angle_Ln2Ln(Cylinder1, Cylinder2, Matrix, MeasurementPlane);
}

void RapidMath3D::TransformationMatrix_Dia_Circle(double *circleParam, double *Matrix, double *MeasurementPlane)
{
	try
	{
		MeasurementPlane[3] = 0;
		double tmpdir[3] = {1, 0, 0}, tmpdir2[3] = {0, 1, 0};
		for(int i = 0; i < 3; i++)
		{
			Matrix[6 + i] = MeasurementPlane[i] = circleParam[3 + i];
			MeasurementPlane[3] += circleParam[i] * circleParam[3 + i];
		}
		if(Angle_Btwn_2Directions(tmpdir, MeasurementPlane, false, true) == 0)
		{
			Create_Perpendicular_Direction_2_2Directions(MeasurementPlane, tmpdir, &Matrix[3]);
			Create_Perpendicular_Direction_2_2Directions(&Matrix[3], &Matrix[6], &Matrix[0]);
		}
		else
		{
			Create_Perpendicular_Direction_2_2Directions(tmpdir2, MeasurementPlane, &Matrix[0]);
			Create_Perpendicular_Direction_2_2Directions(&Matrix[6], &Matrix[0], &Matrix[3]);
		}
	}
	catch(...)
		{ SetAndRaiseErrorMessage("RMATH3D0071a", __FILE__, __FUNCSIG__); }
}

void RapidMath3D::TransformationMatrix_Dia_Sphere(double *sphereParam, double *selLine, double *Matrix, double *MeasurementPlane)
{
	try
	{
		MeasurementPlane[3] = 0;
		for(int i = 0; i < 3; i++)
		{
			MeasurementPlane[i] = selLine[3 + i];
			MeasurementPlane[3] += sphereParam[i] * selLine[3 + i];
		}
		GetTMatrixForPlane(MeasurementPlane, Matrix);
	}
	catch(...)
		{ SetAndRaiseErrorMessage("RMATH3D0071a", __FILE__, __FUNCSIG__); }
}

//--------------------------------------------------------------------------------------
//Measurement functions to calculate toggle mode parameters for sphere and cylinder
//--------------------------------------------------------------------------------------

//Function to calculate surface point during Point to sphere distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Point_Sphere(double *Point, double *Sphere, int MeasureType, double *SprSurfacePt)
{
	try
	{
		//point to center of sphere
		if(MeasureType == 1)
		{
			for(int i = 0; i < 3; i++)
				SprSurfacePt[i] = Sphere[i];
		}
		//point to nearest/farthest point on surface of sphere
		else
		{
			double Dir[3], Ratios[3] = {Point[0] - Sphere[0], Point[1] - Sphere[1], Point[2] - Sphere[2]};
			DirectionCosines(Ratios, &Dir[0]);
		
			if(MeasureType == 0)
				GetSprSurfacePt(Sphere, Dir, true, SprSurfacePt);
			else
				GetSprSurfacePt(Sphere, Dir, false, SprSurfacePt);
		}		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0071", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface line during Point to cylinder distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Point_Cylinder(double *Point, double *Cylinder, double *CylndrEndPts, int MeasureType, double *CylndrSurfaceLn, double *SurfaceLnEndPts)
{
	try
	{
		//point to axis of cylinder
		if(MeasureType == 1)
		{
			for(int i = 0; i < 6; i++)
			{
				CylndrSurfaceLn[i] = Cylinder[i];
				SurfaceLnEndPts[i] = CylndrEndPts[i];
			}
		}
		//point to nearest/farthest line on surface of cylinder
		else
		{
			double IntersectnLine[6];
			Create_Perpendicular_Line_2_Line(Point, Cylinder, IntersectnLine);
		
			if(MeasureType == 0)
			{
				GetCylndrSurfaceLn(Cylinder, CylndrEndPts, &IntersectnLine[3], true, CylndrSurfaceLn, SurfaceLnEndPts);
			}
			else
			{
				GetCylndrSurfaceLn(Cylinder, CylndrEndPts, &IntersectnLine[3], false, CylndrSurfaceLn, SurfaceLnEndPts);
			}
		}		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0072", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface point during line to sphere distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Line_Sphere(double *Line, double *Sphere, int MeasureType, double *SprSurfacePt)
{
	try
	{
		//line to centre of sphere
		if(MeasureType == 1)
		{
			for(int i = 0; i < 3; i++)
				SprSurfacePt[i] = Sphere[i];
		}
		//line to nearest/farthest point on surface of sphere
		else
		{
			double IntersectnLine[6]; 
		
			Create_Perpendicular_Line_2_Line(Sphere, Line, IntersectnLine);
			if(MeasureType == 0)
				GetSprSurfacePt(Sphere, &IntersectnLine[3], false, SprSurfacePt);
			else
				GetSprSurfacePt(Sphere, &IntersectnLine[3], true, SprSurfacePt);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0073", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface line during line to cylinder distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Line_Cylinder(double *Line, double *Cylinder, double *CylndrEndPts, int MeasureType, double *CylndrSurfaceLn, double *SurfaceLnEndPts)
{
	try
	{
		double PerpendDir[3], Line1[6], Line2[6], Ln1Pts[6], Ln2Pts[6], Dist1, Dist2;
		bool ExpectedDir = false;

		//line to axis of cylinder
		if(MeasureType == 1)
		{
			for(int i = 0; i < 6; i++)
			{
				CylndrSurfaceLn[i] = Cylinder[i];
				SurfaceLnEndPts[i] = CylndrEndPts[i];
			}
		}
		//line to nearest/farthest line on surface of cylinder
		else
		{
			if(!Create_Perpendicular_Direction_2_2Directions(&Line[3], &Cylinder[3], PerpendDir))
			{
				double PerpendLn[6];
				Create_Perpendicular_Line_2_Line(Line, Cylinder, PerpendLn);
				PerpendDir[0] = PerpendLn[3]; PerpendDir[1] = PerpendLn[4]; PerpendDir[2] = PerpendLn[5];
			}

			GetCylndrSurfaceLn(Cylinder, CylndrEndPts, PerpendDir, true, Line1, Ln1Pts);
			GetCylndrSurfaceLn(Cylinder, CylndrEndPts, PerpendDir, false, Line2, Ln2Pts);

			Dist1 = Distance_Line_Line(Line, Line1);
			Dist2 = Distance_Line_Line(Line, Line2);
			if(Dist1 < Dist2) ExpectedDir = true;

			if(MeasureType == 0)
			{
				if(ExpectedDir)
					for(int i = 0; i < 6; i++)
					{
						CylndrSurfaceLn[i] = Line1[i];	SurfaceLnEndPts[i] = Ln1Pts[i];
					}
				else
					for(int i = 0; i < 6; i++)
					{
						CylndrSurfaceLn[i] = Line2[i];	SurfaceLnEndPts[i] = Ln2Pts[i];
					}
			}
			else
			{
				if(ExpectedDir)
					for(int i = 0; i < 6; i++)
					{
						CylndrSurfaceLn[i] = Line2[i];	SurfaceLnEndPts[i] = Ln2Pts[i];
					}
				else
					for(int i = 0; i < 6; i++)
					{
						CylndrSurfaceLn[i] = Line1[i];	SurfaceLnEndPts[i] = Ln1Pts[i];
					}
			}
		}		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0074", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface point during plane to sphere distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Plane_Sphere(double *Plane, double *Sphere, int MeasureType, double *SprSurfacePt)
{
	try
	{
		bool AbovePlane = true;

		//plane to centre of sphere
		if(MeasureType == 1)
		{
			for(int i = 0; i < 3; i++)
				SprSurfacePt[i] = Sphere[i];
		}
		//plane to nearest/farthest point on surface of sphere
		else
		{
			if((Sphere[0] * Plane[0] + Sphere[1] * Plane[1] + Sphere[2] * Plane[2]) < Plane[3]) AbovePlane = false;

			if(MeasureType == 0)
				GetSprSurfacePt(Sphere, Plane, !AbovePlane, SprSurfacePt);
			else
				GetSprSurfacePt(Sphere, Plane, AbovePlane, SprSurfacePt);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0075", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface line during plane to cylinder distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Plane_Cylinder(double *Plane, double *Cylinder, double *CylndrEndPts, int MeasureType, double *CylndrSurfaceLn, double *SurfaceLnEndPts)
{
	try
	{
		bool AbovePlane = true;

		//plane to axis of cylinder
		if(MeasureType == 1)
		{
			for(int i = 0; i < 6; i++)
			{
				CylndrSurfaceLn[i] = Cylinder[i];
				SurfaceLnEndPts[i] = CylndrEndPts[i];
			}
		}
		//plane to nearest/farthest line on surface of cylinder
		else
		{
			if((Cylinder[0] * Plane[0] + Cylinder[1] * Plane[1] + Cylinder[2] * Plane[2]) < Plane[3]) AbovePlane = false;

			if(MeasureType == 0)
			{
				GetCylndrSurfaceLn(Cylinder, CylndrEndPts, Plane, !AbovePlane, CylndrSurfaceLn, SurfaceLnEndPts);
			}
			else
			{
				GetCylndrSurfaceLn(Cylinder, CylndrEndPts, Plane, AbovePlane, CylndrSurfaceLn, SurfaceLnEndPts);
			}
		}		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0076", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface points during sphere to sphere distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Sphere_Sphere(double *Sphere1, double *Sphere2, int MeasureType, double *Spr1SurfacePt, double *Spr2SurfacePt)
{
	try
	{
		//centre of sphere1 to centre of sphere2 respectively
		if(MeasureType == 1)
		{
			for(int i = 0; i < 3; i++)
			{
				Spr1SurfacePt[i] = Sphere1[i];
				Spr2SurfacePt[i] = Sphere2[i];
			}
		}
		//nearest/farthest point on surface of sphere1 to nearest/farthest point on surface of sphere2 respectively
		else
		{
			double directn[3], ratio[3] = {Sphere2[0] - Sphere1[0], Sphere2[1] - Sphere1[1], Sphere2[2] - Sphere1[2]};
			DirectionCosines(ratio, directn);
			if(MeasureType == 0)
			{
				GetSprSurfacePt(Sphere1, directn, true, Spr1SurfacePt);
				GetSprSurfacePt(Sphere2, directn, false, Spr2SurfacePt);
			}
			else
			{
				GetSprSurfacePt(Sphere1, directn, false, Spr1SurfacePt);
				GetSprSurfacePt(Sphere2, directn, true, Spr2SurfacePt);
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0077", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface point/line during sphere to cylinder distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Sphere_Cylinder(double *Sphere, double *Cylinder, double *CylndrEndPts, int MeasureType, double *SprSurfacePt, double *CylndrSurfaceLn, double *SurfaceLnEndPts)
{
	try
	{
		//centre of sphere to axis of cylinder
		if(MeasureType == 1)
		{
			//The Sphere part		
			for(int i = 0; i < 3; i++)
			{
				SprSurfacePt[i] = Sphere[i];
			}
			//The Cylinder part
			for(int i = 0; i < 6; i++)
			{
				CylndrSurfaceLn[i] = Cylinder[i];
				SurfaceLnEndPts[i] = CylndrEndPts[i];
			}
		}
		//nearest/farthest point on surface of sphere to nearest/farthest line on surface of cylinder
		else
		{
			double IntersectnLine[6]; 
		
			//Calculate the sphere surface points
			Create_Perpendicular_Line_2_Line(Sphere, Cylinder, IntersectnLine);
			
			//Calculate the sphere surface points and cylinder surface line and end points
			if(MeasureType == 0)
			{
				GetSprSurfacePt(Sphere, &IntersectnLine[3], false, SprSurfacePt);
				GetCylndrSurfaceLn(Cylinder, CylndrEndPts, &IntersectnLine[3], true, CylndrSurfaceLn, SurfaceLnEndPts);
			}
			else
			{
				GetSprSurfacePt(Sphere, &IntersectnLine[3], true, SprSurfacePt);
				GetCylndrSurfaceLn(Cylinder, CylndrEndPts, &IntersectnLine[3], false, CylndrSurfaceLn, SurfaceLnEndPts);
			}
		}	
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0078", __FILE__, __FUNCSIG__); }
}

//Function to calculate surface lines during cylinder to cylinder distance measure mode toggle
void RapidMath3D::MeasureModeCalc_Cylinder_Cylinder(double *Cylinder1, double *Cylndr1EndPts, double *Cylinder2, double *Cylndr2EndPts, int MeasureType, double *Cylndr1SurfaceLn, double *SurfaceLn1EndPts, double *Cylndr2SurfaceLn, double *SurfaceLn2EndPts)
{
	try
	{
		double PerpendDir[3], Line1[6], Line2[6], Line3[6], Line4[6], Ln1Pts[6], Ln2Pts[6], Ln3Pts[6], Ln4Pts[6], Dist1, Dist2;
		bool ExpectedDir = false;
	
		//axis of cylinder1 to axis of cylinder2
		if(MeasureType == 1)
		{
			for(int i = 0; i < 6; i++)
			{
				//First Cylinder
				Cylndr1SurfaceLn[i] = Cylinder1[i];
				SurfaceLn1EndPts[i] = Cylndr1EndPts[i];
				//Second Cylinder
				Cylndr2SurfaceLn[i] = Cylinder2[i];
				SurfaceLn2EndPts[i] = Cylndr2EndPts[i];
			}
		}
		//nearest/farthest line on surface of cylinder1 to nearest/farthest line on surface of cylinder2
		else
		{
			if(!Create_Perpendicular_Direction_2_2Directions(&Cylinder2[3], &Cylinder1[3], PerpendDir))
			{
				double PerpendLn[6];
				Create_Perpendicular_Line_2_Line(Cylinder2, Cylinder1, PerpendLn);
				PerpendDir[0] = PerpendLn[3]; PerpendDir[1] = PerpendLn[4]; PerpendDir[2] = PerpendLn[5];
			}

			GetCylndrSurfaceLn(Cylinder1, Cylndr1EndPts, PerpendDir, true, Line1, Ln1Pts);
			GetCylndrSurfaceLn(Cylinder2, Cylndr2EndPts, PerpendDir, false, Line2, Ln2Pts);
			GetCylndrSurfaceLn(Cylinder1, Cylndr1EndPts, PerpendDir, false, Line3, Ln3Pts);
			GetCylndrSurfaceLn(Cylinder2, Cylndr2EndPts, PerpendDir, true, Line4, Ln4Pts);

			Dist1 = Distance_Line_Line(Line1, Line2);
			Dist2 = Distance_Line_Line(Line3, Line4);
			if(Dist1 < Dist2) ExpectedDir = true;

			if(MeasureType == 0)
			{
				if(ExpectedDir)
					for(int i = 0; i < 6; i++)
					{
						Cylndr1SurfaceLn[i] = Line1[i];	SurfaceLn1EndPts[i] = Ln1Pts[i];
						Cylndr2SurfaceLn[i] = Line2[i];	SurfaceLn2EndPts[i] = Ln2Pts[i];
					}
				else
					for(int i = 0; i < 6; i++)
					{
						Cylndr1SurfaceLn[i] = Line3[i];	SurfaceLn1EndPts[i] = Ln3Pts[i];
						Cylndr2SurfaceLn[i] = Line4[i];	SurfaceLn2EndPts[i] = Ln4Pts[i];
					}
			}
			else
			{
				if(ExpectedDir)
					for(int i = 0; i < 6; i++)
					{
						Cylndr1SurfaceLn[i] = Line3[i];	SurfaceLn1EndPts[i] = Ln3Pts[i];
						Cylndr2SurfaceLn[i] = Line4[i];	SurfaceLn2EndPts[i] = Ln4Pts[i];
					}
				else
					for(int i = 0; i < 6; i++)
					{
						Cylndr1SurfaceLn[i] = Line1[i];	SurfaceLn1EndPts[i] = Ln1Pts[i];
						Cylndr2SurfaceLn[i] = Line2[i];	SurfaceLn2EndPts[i] = Ln2Pts[i];
					}
			}
		}		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0079", __FILE__, __FUNCSIG__); }
}

//Function to get the surafce point on the sphere according to the direction
void RapidMath3D::GetSprSurfacePt(double *Sphere, double *Direction, bool Forward, double *SurfacePt)
{
	try
	{
		int MulFactor = -1;
		if(Forward) MulFactor = 1;

		for(int i = 0; i < 3; i++)
			SurfacePt[i] = Sphere[i] + MulFactor * Direction[i] * Sphere[3];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0080", __FILE__, __FUNCSIG__); }
}

//Function to get the surafce line on the cylinder according to the direction
void RapidMath3D::GetCylndrSurfaceLn(double *Cylinder, double *CylndrEndPts, double *Direction, bool Forward, double *SurfaceLn, double *SurfaceLnPts)
{
	try
	{
		int MulFactor = -1;
		if(Forward) MulFactor = 1;

		for(int i = 0; i < 3; i++)
		{
			SurfaceLn[i] = SurfaceLnPts[i] = CylndrEndPts[i] + MulFactor * Direction[i] * Cylinder[6];
			SurfaceLnPts[3 + i] = CylndrEndPts[3 + i] + MulFactor * Direction[i] * Cylinder[6];
			SurfaceLn[3 + i] = Cylinder[3 + i];
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0081", __FILE__, __FUNCSIG__); }
}

//Function to get the surafce point on the cone according to the direction
//cone param expects radius1 and radius2 in last two params - 6 and 7
void RapidMath3D::GetConeSurfaceLn(double *Cone, double *ConeEndPts, double *Direction, bool Forward, double *SurfaceLn, double *SurfaceLnPts)
{
	try
	{
		int MulFactor = -1;
		if(Forward) MulFactor = 1;

		for(int i = 0; i < 3; i++)
		{
			SurfaceLn[i] = SurfaceLnPts[i] = ConeEndPts[i] + MulFactor * Direction[i] * Cone[6];
			SurfaceLnPts[3 + i] = ConeEndPts[3 + i] + MulFactor * Direction[i] * Cone[7];
			SurfaceLn[3 + i] = Cone[3 + i];
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0118", __FILE__, __FUNCSIG__); }
}



//--------------------------------------------------------------------------------------
//Angle calculation functions according to the mouse position
//--------------------------------------------------------------------------------------

//Angle between 2 lines
double RapidMath3D::Angle_Line_Line(double *Line1, double *Line2, double *MeasurementPlane, double *MouseSelectionLn, double *Line1MidPt)
{
	try
	{
		double MousePt[3], Line2Projected[6], PerpendLn1[6], PerpendLn2[6];

		//Get the mouse position by intersecting measurement plane and the mouse selection line
		Intersection_Line_Plane(MouseSelectionLn, MeasurementPlane, MousePt);
	
		double CurrentPoint[3] = {0};
		if (!Intersection_Line_Line(Line1, Line2, CurrentPoint)) //return 0;				
			//..vinod ..before it was if(!Intersection_Line_Line(Line1, Line2, CurrentPoint)) return 0;  so angle was always came zero..
		{
			//Project the line2 on to the measurement plane, to use it as the 2nd line(in case Line2 dosent lie on the plane)
			Projection_Line_on_Plane(Line2, MeasurementPlane, Line2Projected);
			if (!Intersection_Line_Line(Line1, Line2Projected, CurrentPoint)) return 0;				
				//Projection_Point_on_Plane(Line2, MeasurementPlane, Line2Projected);
		}
		for(int i = 0; i < 3; i++)
		{
			Line2Projected[i] = CurrentPoint[i];
			//Line2Projected[i] = Line1MidPt[i];
			Line2Projected[3 + i] = Line2[3 + i];
		}
		//Now drop a line from the mouse pt to each of the 2 lines
		if(Create_Perpendicular_Line_2_Line(MousePt, Line1, PerpendLn1) &&
			Create_Perpendicular_Line_2_Line(MousePt, Line2Projected, PerpendLn2))
			return (M_PI - Angle_Line_Line(PerpendLn1, PerpendLn2));
		else
			return Angle_Line_Line(Line1, Line2);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0082", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::Angle_Line_Line(double *Line1, double *Line2, double *MeasurementPlane, double *MouseSelectionLn)
{
	try
	{
		double MousePt[3], PerpendLn1[6], PerpendLn2[6];
		Intersection_Line_Plane(MouseSelectionLn, MeasurementPlane, MousePt);
	
		//Now drop a line from the mouse pt to each of the 2 lines
		if(Create_Perpendicular_Line_2_Line(MousePt, Line1, PerpendLn1) &&
			Create_Perpendicular_Line_2_Line(MousePt, Line2, PerpendLn2))
			return (M_PI - Angle_Line_Line(PerpendLn1, PerpendLn2));
		else
			return Angle_Line_Line(Line1, Line2);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0082", __FILE__, __FUNCSIG__); return 0; }
}

//Angle between a xline and a plane..vinod
double RapidMath3D::Angle_XLine_Plane(double *XLineDirratio, double *PlaneDircosine)//PlaneDircosine means normal vector's DC's....
{
	try
	{	
		double PlaneDircosineDotXLineDirratio = 0.0, ModPlaneDircosine =0.0, ModXLineDirratio = 0.0, angle = 0.0;
		PlaneDircosineDotXLineDirratio = abs((PlaneDircosine[0] * XLineDirratio[0]) + (PlaneDircosine[1] * XLineDirratio[1]) + (PlaneDircosine[2] * XLineDirratio[2]));//vectorN.vectorU
		ModPlaneDircosine = sqrt((PlaneDircosine[0] * PlaneDircosine[0]) + (PlaneDircosine[1] * PlaneDircosine[1]) + (PlaneDircosine[2] * PlaneDircosine[2]));//    |vectorN|
		ModXLineDirratio = sqrt((XLineDirratio[0] * XLineDirratio[0]) + (XLineDirratio[1] * XLineDirratio[1]) + (XLineDirratio[2] * XLineDirratio[2]));//    |vectorU|
		angle = asin(PlaneDircosineDotXLineDirratio/(ModPlaneDircosine*ModXLineDirratio));		//alpha = arcsin(abs(vectorN.vectorU)/( |vectorN|* |vectorU|))
		return angle;			//hiis angle is always<M_PI/2
	}
	catch(...)
	{ 
		SetAndRaiseErrorMessage("RMATH3D0083", __FILE__, __FUNCSIG__); return 0; 
	}
}

//Angle between a line and a plane
double RapidMath3D::Angle_Line_Plane(double *Line, double *LinePts, double *Plane, double *PlanePts, double *MeasurementPlane, double *MouseSelectionLn, bool ProjectLine)
{
	try
	{
		double Dist = 0, ProjectedMidPt[3] = {0},  TheTmpLine[6] = {0,0,0,0,0,1};
		double LnMidPt[3] = {(LinePts[0] + LinePts[3]) / 2, (LinePts[1] + LinePts[4]) / 2, (LinePts[2] + LinePts[5]) / 2};
		double PlnMidPt[3] = {(PlanePts[0] + PlanePts[6]) / 2, (PlanePts[1] + PlanePts[7]) / 2, (PlanePts[2] + PlanePts[8]) / 2};
		
		if(ProjectLine)
		{
			int multiplier = 1;

			//Get the distance from line mid pt to plane
			Dist = Distance_Point_Plane(LnMidPt, Plane);

			//Set the multiplier according to pt on +ve or -ve side of the plane
			if(LnMidPt[0] * Plane[0] + LnMidPt[1] * Plane[1] + LnMidPt[2] * Plane[2] > Plane[3])
				multiplier = 1;
			else
				multiplier = -1;

			//Now get the pt where the projected line must be drawn
			for(int i = 0; i < 3; i++)
				ProjectedMidPt[i] = PlnMidPt[i] + multiplier * Plane[i] * Dist;

			double DashedLine[6] = {0,0,0,0,0,1};
			for(int i = 0; i < 3; i++)
			{
				DashedLine[i] = ProjectedMidPt[i];
				DashedLine[3 + i] = Line[3 + i];
			}
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = DashedLine[i];
			}
		}
		else 
		{
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = Line[i];
			}
		}
		
		double LnOnPlane[6] = {0,0,0,0,0,1}, MousePt[3] = {0}, PerpendLn1[6] = {0,0,0,0,0,1}, PerpendLn2[6] = {0,0,0,0,0,1};
		//Get the mouse position by intersecting measurement plane and the mouse selection line
		Intersection_Line_Plane(MouseSelectionLn, MeasurementPlane, MousePt);
		//Project the line2 on to the measurement plane, to use it as the 2nd line(in case Line2 dosent lie on the plane)
		
		Intersection_Plane_Plane(Plane, MeasurementPlane, LnOnPlane);
		
		//Now drop a line from the mouse pt to each of the 2 lines
		if(Create_Perpendicular_Line_2_Line(MousePt, TheTmpLine, PerpendLn1) &&
			Create_Perpendicular_Line_2_Line(MousePt, LnOnPlane, PerpendLn2))
			return (M_PI - Angle_Line_Line(PerpendLn1, PerpendLn2));
		else
			return Angle_Line_Line(TheTmpLine, LnOnPlane);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0083", __FILE__, __FUNCSIG__); return 0; }
}

 double RapidMath3D::Angle_Line_Circle3D(double *Line, double *endpnts, double *Circle3D, double *MeasurementPlane, double *MouseSelectionLn, bool ProjectLine)
 { 
	 try 
		 {
			double Dist, LnOnPlane[6], PerpendLn1[6], PerpendLn2[6], ProjectedMidPt[3],TheTmpLine[6], MousePt[3];
			double LnMidPt[3] = {(endpnts[0] + endpnts[3]) / 2, (endpnts[1] + endpnts[4]) / 2, (endpnts[2] + endpnts[5]) / 2};
			double PlnMidPt[3] = {Circle3D[0], Circle3D[1], Circle3D[2]};
			int multiplier;
			double CirclePln[4] = {Circle3D[3], Circle3D[4], Circle3D[5], Circle3D[0] * Circle3D[3] + Circle3D[1] * Circle3D[4] + Circle3D[2] * Circle3D[5] };
			//Get the distance from line mid pt to plane
			Intersection_Line_Plane(MouseSelectionLn, MeasurementPlane, MousePt);
			Intersection_Plane_Plane( CirclePln, MeasurementPlane, LnOnPlane);

			if (ProjectLine)
			{
				Dist = Distance_Point_Plane(LnMidPt, CirclePln);

				//Set the multiplier according to pt on +ve or -ve side of the plane
				if(LnMidPt[0] * CirclePln[0] + LnMidPt[1] * CirclePln[1] + LnMidPt[2] * CirclePln[2] > CirclePln[3])
					multiplier = 1;
				else
					multiplier = -1;

				//Now get the pt where the projected line must be drawn
				for(int i = 0; i < 3; i++)
					ProjectedMidPt[i] = PlnMidPt[i] + multiplier * 	CirclePln[i] * Dist;
						//Get the end pts of the projected line
									
				//Set the line and line points
				double DashedLine[6];
				for(int i = 0; i < 3; i++)
				{
					DashedLine[i] = ProjectedMidPt[i];
					DashedLine[3 + i] = Line[3 + i];
				}
				for(int i = 0; i < 6; i++)
				{
					TheTmpLine[i] = DashedLine[i];
				}
			}
			else 
			{
				for(int i = 0; i < 6; i++)
				{
					TheTmpLine[i] = Line[i];
				}
			}
			if(Create_Perpendicular_Line_2_Line(MousePt, TheTmpLine, PerpendLn1) && Create_Perpendicular_Line_2_Line(MousePt, LnOnPlane, PerpendLn2))
				return (M_PI - Angle_Line_Line(PerpendLn1, PerpendLn2));
			else
				return Angle_Line_Line(TheTmpLine, LnOnPlane);
	 }
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0083", __FILE__, __FUNCSIG__); return 0; }
}

//Angle between 2 planes
double RapidMath3D::Angle_Plane_Plane(double *Plane1, double *Plane2, double *MeasurementPlane, double *MouseSelectionLn)
{
	try
	{
		double LnOnPlane1[6], LnOnPlane2[6], MousePt[3], PerpendLn1[6], PerpendLn2[6];
		Intersection_Plane_Plane(Plane1, MeasurementPlane, LnOnPlane1);
		Intersection_Plane_Plane(Plane2, MeasurementPlane, LnOnPlane2);
		//Get the mouse position by intersecting measurement plane and the mouse selection line
		Intersection_Line_Plane(MouseSelectionLn, MeasurementPlane, MousePt);
		//Now drop a line from the mouse pt to each of the 2 lines
		if(Create_Perpendicular_Line_2_Line(MousePt, LnOnPlane1, PerpendLn1) &&
			Create_Perpendicular_Line_2_Line(MousePt, LnOnPlane2, PerpendLn2))
			return (M_PI - Angle_Line_Line(PerpendLn1, PerpendLn2));
		else
			return Angle_Line_Line(LnOnPlane1, LnOnPlane2);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0084", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::Angle_Line_Conics3D(double *Line, double *endpnts, double *Conics3D, double *MeasurementPlane, double *MouseSelectionLn, bool ProjectLine)
 { 
	 try 
		 {
			 double dir[6], Cen[3];
			 double Max = (2 * Conics3D[13] *Conics3D[16])/(1 - Conics3D[13] * Conics3D[13]);
			 Create_Perpendicular_Line_2_Line(&Conics3D[4], &Conics3D[7], &dir[0]);
			 double focallgt = (Max/2) - (Conics3D[13] *Conics3D[16])/(1 + Conics3D[13]);
			for(int i = 0; i < 3; i++) Cen[i] = Conics3D[4 + i] + focallgt * dir[3 + i];
			double Dist, LnOnPlane[6], PerpendLn1[6],ProjectedMidPt[3], PerpendLn2[6],TheTmpLine[6], MousePt[3];
			double LnMidPt[3] = {(endpnts[0] + endpnts[3]) / 2, (endpnts[1] + endpnts[4]) / 2, (endpnts[2] + endpnts[5]) / 2};
			double PlnMidPt[3] = {Cen[0], Cen[1], Cen[2]};
			int multiplier;
		  //Get the distance from line mid pt to plane
			Intersection_Line_Plane(MouseSelectionLn, MeasurementPlane, MousePt);
			Intersection_Plane_Plane(Conics3D, MeasurementPlane, LnOnPlane);

			if (ProjectLine)
			{
			Dist = Distance_Point_Plane(LnMidPt, Conics3D);

			//Set the multiplier according to pt on +ve or -ve side of the plane
			if(LnMidPt[0] * Conics3D[0] + LnMidPt[1] * Conics3D[1] + LnMidPt[2] * Conics3D[2] > Conics3D[3])
				multiplier = 1;
			else
				multiplier = -1;

			//Now get the pt where the projected line must be drawn
			for(int i = 0; i < 3; i++)
				ProjectedMidPt[i] = PlnMidPt[i] + multiplier * Conics3D[i] * Dist;
					//Get the end pts of the projected line
									
			//Set the line and line points
			double DashedLine[6];
			for(int i = 0; i < 3; i++)
			{
				DashedLine[i] = ProjectedMidPt[i];
				DashedLine[3 + i] = Line[3 + i];
			}
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = DashedLine[i];
			}
			}
			else 
			{
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = Line[i];
			}
			}
			if(Create_Perpendicular_Line_2_Line(MousePt, TheTmpLine, PerpendLn1) &&
			Create_Perpendicular_Line_2_Line(MousePt, LnOnPlane, PerpendLn2))
			return (M_PI - Angle_Line_Line(PerpendLn1, PerpendLn2));
			else
			return Angle_Line_Line(TheTmpLine, LnOnPlane);
	 }
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0083", __FILE__, __FUNCSIG__); return 0; }
}

//--------------------------------------------------------------------------------------
//Nearest and farthest point to object calculations
//--------------------------------------------------------------------------------------

//Nearest point to the given point
double RapidMath3D::Nearest_Point_to_Point(double *PointsList, int NoOfPts, double *Point, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Point(PointsList, Point);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Point(&PointsList[itr * 3], Point);
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0085", __FILE__, __FUNCSIG__); return 0; }
}

//Nearest point to the given line
double RapidMath3D::Nearest_Point_to_Line(double *PointsList, int NoOfPts, double *Line, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Line(PointsList, Line);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Line(&PointsList[itr * 3], Line);
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0086", __FILE__, __FUNCSIG__); return 0; }
}

//Nearest point to the given Plane
double RapidMath3D::Nearest_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Plane(PointsList, Plane);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Plane(&PointsList[itr * 3], Plane);
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0087", __FILE__, __FUNCSIG__); return 0; }
}

//Nearest point to the given Sphere
double RapidMath3D::Nearest_Point_to_Sphere(double *PointsList, int NoOfPts, double *Sphere, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Point(PointsList, Sphere) - Sphere[3];
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Point(&PointsList[itr * 3], Sphere) - Sphere[3];
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0088", __FILE__, __FUNCSIG__); return 0; }
}

//Nearest point to the given Cylinder
double RapidMath3D::Nearest_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Line(PointsList, Cylinder) - Cylinder[6];
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Line(&PointsList[itr * 3], Cylinder) - Cylinder[6];
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0089", __FILE__, __FUNCSIG__); return 0; }
}

//Farthest point to the given Point
double RapidMath3D::Farthest_Point_to_Point(double *PointsList, int NoOfPts, double *Point, double *FarthestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Point(PointsList, Point);
		for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Point(&PointsList[itr * 3], Point);
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0090", __FILE__, __FUNCSIG__); return 0; }
}

//Farthest point to the given Line
double RapidMath3D::Farthest_Point_to_Line(double *PointsList, int NoOfPts, double *Line, double *FarthestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Line(PointsList, Line);
		for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Line(&PointsList[itr * 3], Line);
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0091", __FILE__, __FUNCSIG__); return 0; }
}

//Farthest point to the given Plane
double RapidMath3D::Farthest_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *FarthestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Plane(PointsList, Plane);
		for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Plane(&PointsList[itr * 3], Plane);
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0092", __FILE__, __FUNCSIG__); return 0; }
}

//Farthest point to the given Sphere
double RapidMath3D::Farthest_Point_to_Sphere(double *PointsList, int NoOfPts, double *Sphere, double *FarthestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Point(PointsList, Sphere) - Sphere[3];
		for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Point(&PointsList[itr * 3], Sphere) - Sphere[3];
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0093", __FILE__, __FUNCSIG__); return 0; }
}

//Farthest point to the given Cylinder
double RapidMath3D::Farthest_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *FarthestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Line(PointsList, Cylinder) - Cylinder[6];
		for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Line(&PointsList[itr * 3], Cylinder) - Cylinder[6];
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) FarthestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0094", __FILE__, __FUNCSIG__); return 0; }
}

//smallest angle of cone
double RapidMath3D::Smallest_Angle_of_Cone(double *PointsList, int NoOfPts, double *Cone, double *Pt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double RadialDist = Distance_Point_Line(PointsList, Cone) - Cone[6];
		double ProjDist = (PointsList[0] - Cone[0]) * Cone[3] + (PointsList[1] - Cone[1]) * Cone[4] + (PointsList[2] - Cone[2]) * Cone[5];
		double Angle;
		if (ProjDist != 0)
			Angle = atan (RadialDist/ProjDist);
		double TempAngle = 0;

		for(int i = 0; i < 3; i++) Pt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			RadialDist = Distance_Point_Line(&PointsList[itr * 3], Cone) - Cone[6];
			ProjDist = (PointsList[itr * 3] - Cone[0]) * Cone[3] + (PointsList[itr * 3 + 1] - Cone[1]) * Cone[4] + (PointsList[itr * 3 + 2] - Cone[2]) * Cone[5];
			if (ProjDist == 0) continue;
			TempAngle = atan (RadialDist/ProjDist);
			if(TempAngle < Angle)
			{
				Angle = TempAngle;
				for(int i = 0; i < 3; i++) Pt[i] = PointsList[itr * 3 + i];
			}
		}
		return Angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0114", __FILE__, __FUNCSIG__); return 0; }
}

//largest angle of cone
double RapidMath3D::Largest_Angle_of_Cone(double *PointsList, int NoOfPts, double *Cone, double *Pt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double RadialDist = Distance_Point_Line(PointsList, Cone) - Cone[6];
		double ProjDist = (PointsList[0] - Cone[0]) * Cone[3] + (PointsList[1] - Cone[1]) * Cone[4] + (PointsList[2] - Cone[2]) * Cone[5];
		double Angle;
		if (ProjDist != 0)
			Angle = atan (RadialDist/ProjDist);
		double TempAngle = 0;

		for(int i = 0; i < 3; i++) Pt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			RadialDist = Distance_Point_Line(&PointsList[itr * 3], Cone) - Cone[6];
			ProjDist = (PointsList[itr * 3] - Cone[0]) * Cone[3] + (PointsList[itr * 3 + 1] - Cone[1]) * Cone[4] + (PointsList[itr * 3 + 2] - Cone[2]) * Cone[5];
			if (ProjDist == 0) continue;
			TempAngle = atan (RadialDist/ProjDist);
			if(TempAngle > Angle)
			{
				Angle = TempAngle;
				for(int i = 0; i < 3; i++) Pt[i] = PointsList[itr * 3 + i];
			}
		}
		return Angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0115", __FILE__, __FUNCSIG__); return 0; }
}

//to find point with least directional distance to the given Plane
double RapidMath3D::Least_DirectionalDist_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Plane_with_Dir(PointsList, Plane);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Plane_with_Dir(&PointsList[itr * 3], Plane);
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0095", __FILE__, __FUNCSIG__); return 0; }
}

//to find point with least directional distance to the given Sphere
double RapidMath3D::Least_DirectionalDist_Point_to_Sphere(double *PointsList, int NoOfPts, double *Sphere, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Sphere_with_Dir(PointsList, Sphere);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Sphere_with_Dir(&PointsList[itr * 3], Sphere);
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0096", __FILE__, __FUNCSIG__); return 0; }
}

//to find point with least directional distance to the given Cylinder
double RapidMath3D::Least_DirectionalDist_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Cylinder_with_Dir(PointsList, Cylinder);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Cylinder_with_Dir(&PointsList[itr * 3], Cylinder);
			if(TempDist < Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0097", __FILE__, __FUNCSIG__); return 0; }
}

//to find point with most directional distance to the given Plane
double RapidMath3D::Most_DirectionalDist_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Plane_with_Dir(PointsList, Plane);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Plane_with_Dir(&PointsList[itr * 3], Plane);
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0098", __FILE__, __FUNCSIG__); return 0; }
}

//to find point with most directional distance to the given Sphere
double RapidMath3D::Most_DirectionalDist_Point_to_Sphere(double *PointsList, int NoOfPts, double *Sphere, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Sphere_with_Dir(PointsList, Sphere);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Sphere_with_Dir(&PointsList[itr * 3], Sphere);
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0099", __FILE__, __FUNCSIG__); return 0; }
}

//to find point with most directional distance to the given Cylinder
double RapidMath3D::Most_DirectionalDist_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *NearestPt)
{
	try
	{
		if(NoOfPts < 1) return 0;

		double TempDist, Dist = Distance_Point_Cylinder_with_Dir(PointsList, Cylinder);
		for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[i];

		for(int itr = 1; itr < NoOfPts; itr++)
		{
			TempDist = Distance_Point_Cylinder_with_Dir(&PointsList[itr * 3], Cylinder);
			if(TempDist > Dist)
			{
				Dist = TempDist;
				for(int i = 0; i < 3; i++) NearestPt[i] = PointsList[itr * 3 + i];
			}
		}
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0100", __FILE__, __FUNCSIG__); return 0; }
}



//--------------------------------------------------------------------------------------
//3D GD&T calculations
//--------------------------------------------------------------------------------------

//Function to calculate the straightness of points wrt to a line
double RapidMath3D::Straightness(double *PointsList, int NoOfPts, double *Line)
{
	try
	{
		double TmpPt[3];
		//double Dist1 = Nearest_Point_to_Line(PointsList, NoOfPts, Line, TmpPt);
		double Dist = Farthest_Point_to_Line(PointsList, NoOfPts, Line, TmpPt);
		//return (Dist2 - Dist1);
		return Dist * 2;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0101", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::FilteredStraightness(double *PointsList, int NoOfPts, double *Line, int filterfactor)
{
	try
	{
		double dist = 0, tmpdist = 0;
		for(int i = 0; i < NoOfPts; i++)
		{
			tmpdist = Distance_Point_Line(&PointsList[3 * i], Line);
			dist += abs(tmpdist);
		} 
		dist /= NoOfPts;
		int newcnt = 0;
		double *filterPnts = new double[NoOfPts * 3];
		for(int i = 0; i < NoOfPts; i++)
		{
			tmpdist = Distance_Point_Line(&PointsList[3 * i], Line);
			if(abs(tmpdist) < filterfactor * dist)
			{
			   filterPnts[3 * i] = PointsList[3 * i];
			   filterPnts[3 * i + 1] = PointsList[3 * i + 1];
			   filterPnts[3 * i + 2] = PointsList[3 * i + 2];
			   newcnt++;
			}
		} 
		double TmpPt[3];
		//double Dist1 = Nearest_Point_to_Line(PointsList, NoOfPts, Line, TmpPt);
		double Dist = Farthest_Point_to_Line(filterPnts, newcnt, Line, TmpPt);
		//return (Dist2 - Dist1);
		delete [] filterPnts;
		return Dist * 2;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0101b", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the Angularity of a Line wrt Plane
double RapidMath3D::Angularity_Line_wrt_Line(double *Line, double *RefLine, double Angle, double Land)
{
	try
	{
		double Ang = Angle_Line_Line(Line, RefLine);
		return Land * tan(Angle - Ang);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0102", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the flatness of points wrt plane
double RapidMath3D::Flatness(double *PointsList, int NoOfPts, double *Plane)
{
	try
	{
		double TmpPt[3];
		double Dist1 = Least_DirectionalDist_Point_to_Plane(PointsList, NoOfPts, Plane, TmpPt);
		double Dist2 = Most_DirectionalDist_Point_to_Plane(PointsList, NoOfPts, Plane, TmpPt);
		return (Dist2 - Dist1);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0103", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::FilteredFlatness(double *PointsList, int NoOfPts, double *Plane, int filterfactor)
{
	try
	{
		double dist = 0, tmpdist = 0;
		for(int i = 0; i < NoOfPts; i++)
		{
			tmpdist = Distance_Point_Plane(&PointsList[3 * i], Plane);
			dist += abs(tmpdist);
		} 
		dist /= NoOfPts;
		int newcnt = 0;
		double *filterPnts = new double[NoOfPts * 3];
		for(int i = 0; i < NoOfPts; i++)
		{
			tmpdist = Distance_Point_Plane(&PointsList[3 * i], Plane);
			if(abs(tmpdist) < filterfactor * dist)
			{
			   filterPnts[3 * i] = PointsList[3 * i];
			   filterPnts[3 * i + 1] = PointsList[3 * i + 1];
			   filterPnts[3 * i + 2] = PointsList[3 * i + 2];
			   newcnt++;
			}
		}
		double TmpPt[3];
		double Dist1 = Least_DirectionalDist_Point_to_Plane(filterPnts, newcnt, Plane, TmpPt);
		double Dist2 = Most_DirectionalDist_Point_to_Plane(filterPnts, newcnt, Plane, TmpPt);
		delete [] filterPnts;
		return (Dist2 - Dist1);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0103b", __FILE__, __FUNCSIG__); return 0; }
}
//Function to calculate the Angularity of a Plane wrt Plane
double RapidMath3D::Angularity_Plane_wrt_Plane(double *Plane, double *RefPlane, double Angle, double Land)
{
	try
	{
		double Ang = Angle_Plane_Plane(Plane, RefPlane);
		return Land * tan(Angle - Ang);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0104", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the cylindricity of a cylinder
double RapidMath3D::Cylindricity(double *PointsList, int NoOfPts, double *Cylinder)
{
	try
	{
		double TmpPt[3];
		double Dist1 = Least_DirectionalDist_Point_to_Cylinder(PointsList, NoOfPts, Cylinder, TmpPt);
		double Dist2 = Most_DirectionalDist_Point_to_Cylinder(PointsList, NoOfPts, Cylinder, TmpPt);
		return (Dist2 - Dist1);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0105", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::FilteredCylindricity(double *PointsList, int NoOfPts, double *Cylinder, int filterfactor)
{
	try
	{
		double dist = 0, tmpdist = 0;
		for(int i = 0; i < NoOfPts; i++)
		{
			tmpdist = Distance_Point_Line(&PointsList[3 * i], Cylinder);
			dist += abs(tmpdist);
		} 
		dist /= NoOfPts;
		int newcnt = 0;
		double *filterPnts = new double[NoOfPts * 3];
		for(int i = 0; i < NoOfPts; i++)
		{
			tmpdist = Distance_Point_Line(&PointsList[3 * i], Cylinder);
			if(abs(tmpdist) < filterfactor * dist)
			{
			   filterPnts[3 * i] = PointsList[3 * i];
			   filterPnts[3 * i + 1] = PointsList[3 * i + 1];
			   filterPnts[3 * i + 2] = PointsList[3 * i + 2];
			   newcnt++;
			}
		} 
		double TmpPt[3];
		double Dist1 = Least_DirectionalDist_Point_to_Cylinder(filterPnts, newcnt, Cylinder, TmpPt);
		double Dist2 = Most_DirectionalDist_Point_to_Cylinder(filterPnts, newcnt, Cylinder, TmpPt);
		delete [] filterPnts;
		return (Dist2 - Dist1);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0105", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the conicality of a cone
double RapidMath3D::Conicality(double *PointsList, int NoOfPts, double *Cone)
{
	try
	{
		double TmpPt[3];
		double Ang1 = Smallest_Angle_of_Cone(PointsList, NoOfPts, Cone, TmpPt);
		double Ang2= Largest_Angle_of_Cone(PointsList, NoOfPts, Cone, TmpPt);
		return (Ang2 - Ang1);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0113", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath3D::Coaxiality(double *curLinePnts, double *refLine, double Land)
{
	double dist[4] = {0}, coaxiality = 0;
	dist[0] = Distance_Point_Line(curLinePnts, refLine);
	dist[1] = Distance_Point_Line(&curLinePnts[3], refLine);
	coaxiality = dist[0];
	if(coaxiality < dist[1])
		coaxiality = dist[1];
	return Land * coaxiality;
}

double RapidMath3D::Angularity_Line_wrt_Plane(double *Line, double *Plane, double Angle, double Land)
{
	try
	{
		double Ang = Angle_Line_Plane(Line, Plane);
		return Land * tan(Angle - Ang);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0106", __FILE__, __FUNCSIG__); return 0; }
}


//--------------------------------------------------------------------------------------
//Probe Point Manipulation calculations
//--------------------------------------------------------------------------------------

//Function to calculate the plane surface pt when probe centre is given
void RapidMath3D::Plane_SurfacePt_for_Probe(double *Plane, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt)
{
	try
	{
	   int MulFactor = -1;
	   if(InnerSurface) MulFactor = 1;

	   for(int i = 0; i < 3; i++) SurfacePt[i] = ProbePt[i] + MulFactor * ProbeRadius * Plane[1 + i];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0107", __FILE__, __FUNCSIG__); }
}

//Function to calculate the Sphere surface pt when probe centre is given
void RapidMath3D::Sphere_SurfacePt_for_Probe(double *SphereCentrePt, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt)
{
	try
	{
	   int MulFactor = -1;
	   double DirRatios[3] = {ProbePt[0] - SphereCentrePt[0], ProbePt[1] - SphereCentrePt[1], ProbePt[2] - SphereCentrePt[2]}, DirCosines[3];

	   if(InnerSurface) MulFactor = 1;

	   DirectionCosines(DirRatios, DirCosines);

	   for(int i = 0; i < 3; i++) SurfacePt[i] = ProbePt[i] + MulFactor * ProbeRadius * DirCosines[i];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0108", __FILE__, __FUNCSIG__); }
}

//Function to calculate the cylinder surface pt when probe centre is given
void RapidMath3D::Cylinder_SurfacePt_for_Probe(double *CylinderAxisLine, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt)
{
	try
	{
	   int MulFactor = -1;
	   double DirLine[6];

	   if(InnerSurface) MulFactor = 1;

	   Create_Perpendicular_Line_2_Line(ProbePt, CylinderAxisLine, DirLine);
	   
	   for(int i = 0; i < 3; i++) SurfacePt[i] = ProbePt[i] + MulFactor * ProbeRadius * DirLine[3 + i];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0109", __FILE__, __FUNCSIG__); }
}

void RapidMath3D::Torus_SurfacePt_for_Probe(double *TorusParam, double *ProbePt, double ProbeRadius, double *SurfacePt)
{
	try
	{
		   double Dir1[3], Dir2[3], Dir3[3], DirLine[3], TorusPipeCen[3], TempPntsCol1[6], TempPntsCol2[6];
		   for(int i = 0; i < 3; i++)
		   {
			 TempPntsCol1[i] = ProbePt[i];
			 TempPntsCol1[3 + i] = TorusParam[i];
		   }
		   DCs_TwoEndPointsofLine(TempPntsCol1, Dir1);
		   if(!Create_Perpendicular_Direction_2_2Directions(Dir1, &TorusParam[3], Dir2)) return;
		   Create_Perpendicular_Direction_2_2Directions(Dir2, &TorusParam[3], Dir3);
		   for(int i = 0; i < 3; i++) TorusPipeCen[i] = TorusParam[i] + (TorusParam[7] + (TorusParam[6] - TorusParam[7])/2 ) * Dir3[i];
		   for(int i = 0; i < 3; i++)
		   {
			 TempPntsCol2[i] = TorusPipeCen[i];
			 TempPntsCol2[3 + i] = ProbePt[i];
		   }
		   DCs_TwoEndPointsofLine(TempPntsCol2, DirLine);
		   for(int i = 0; i < 3; i++) SurfacePt[i] = ProbePt[i] + ProbeRadius * DirLine[i];
   }
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0109", __FILE__, __FUNCSIG__); }
}

//Function to calculate the cone surface pt when probe centre is given
void RapidMath3D::Cone_SurfacePt_for_Probe(double *ConeParam, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt)
{
	try
	{
	   int MulFactor = -1;
	   double Direction[3];
	   double Ratios[3];

	   if(InnerSurface) MulFactor = 1;

	   //first get projection of probe point on cone axis.
		double ProjectionPoint[3];
		Projection_Point_on_Line(ProbePt, ConeParam, ProjectionPoint);
		//next get point on cone axis such that
		//the line joining this point to probe point is perpendicular to cone surface
		double ConeAxialProjectionPoint[3];
		double radius = Distance_Point_Point(ProbePt, ProjectionPoint);
		double ApexAngle = * (ConeParam + 7);
		for (int i=0; i< 3; i++)
		{
			ConeAxialProjectionPoint[i] = ProjectionPoint[i] + radius * tan (ApexAngle) * *(ConeParam + 3 + i);
		}

	   for (int i =0; i< 3; i++)
	   {
		   Ratios[i] = *(ProbePt + i) - ConeAxialProjectionPoint[i];
	   }
	   
	   DirectionCosines(Ratios, Direction);

	   for(int i = 0; i < 3; i++) SurfacePt[i] = ProbePt[i] + MulFactor * ProbeRadius * Direction[i];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0112", __FILE__, __FUNCSIG__); }
}



//--------------------------------------------------------------------------------------
//DXF shape arrangement functions
//--------------------------------------------------------------------------------------

//Function to group a collection of shapes into clusters that form a closed loop
void RapidMath3D::ArrangeDXFShapesInOrder(double *ShapePts, int *ShapeID, int NoOfShapes, std::list<std::list<int>*> *ClusterList)
{
	try
	{
		struct Shapes
		{
			double Pt1X, Pt1Y, Pt2X, Pt2Y;
			int ID;
			//bool Flipped;
		}TmpShape;
		std::list <Shapes> AllShapeList, ClosedShapeList;
		int Counter = 0; //Reset counter
		std::list<int> *ClosedShpsIDList;
		double TmpVarForFlip;
		bool AddingToCluster = true, StilToAdd = false;
		std::list<Shapes>::iterator itr;

		//Put all the shapes into list
		for(int i = 0; i < NoOfShapes; i++)
		{
			TmpShape.Pt1X = ShapePts[i * 4];
			TmpShape.Pt1Y = ShapePts[i * 4 + 1];
			TmpShape.Pt2X = ShapePts[i * 4 + 2];
			TmpShape.Pt2Y = ShapePts[i * 4 + 3];
			TmpShape.ID = ShapeID[i];
			//TmpShape.Flipped = false;
			AllShapeList.push_back(TmpShape);
		}

		//While there are shapes in main list
		while((int)AllShapeList.size() != 0)
		{
			//Put first shape from main list to cluster list
			TmpShape = *(AllShapeList.begin());
			ClosedShapeList.push_back(TmpShape);
			AllShapeList.pop_front();
			//Set still adding to cluster
			AddingToCluster = true;
			StilToAdd = false;

			//while still adding shapes to cluster
			while(AddingToCluster)
			{
				StilToAdd = false;
				itr = AllShapeList.begin();
				//search each shape in list for continuity
				for each(Shapes Shp in AllShapeList)
				{
					if(abs((*(--(ClosedShapeList.end()))).Pt2X - Shp.Pt1X) < 0.001 && abs((*(--(ClosedShapeList.end()))).Pt2Y - Shp.Pt1Y) < 0.001)
					{
						//Since we have a continuity shape, take it out of main list
						AllShapeList.erase(itr);
						//Add the shape to cluster list
						ClosedShapeList.push_back(Shp);
						//Now we need to search for continuity afresh
						StilToAdd = true;
						break;
					}
					else if(abs((*(--(ClosedShapeList.end()))).Pt2X - Shp.Pt2X) < 0.001 && abs((*(--(ClosedShapeList.end()))).Pt2Y - Shp.Pt2Y) < 0.001)
					{
						//Since we have a continuity shape, take it out of main list
						AllShapeList.erase(itr);
						//Now since the shape is 'ulta', we flip the shape
						TmpVarForFlip = Shp.Pt1X; Shp.Pt1X = Shp.Pt2X; Shp.Pt2X = TmpVarForFlip;
						TmpVarForFlip = Shp.Pt1Y; Shp.Pt1Y = Shp.Pt2Y; Shp.Pt2Y = TmpVarForFlip;
						//Shp.Flipped = true;
						//Add the shape to cluster list
						ClosedShapeList.push_back(Shp);
						//Now we need to search for continuity afresh
						StilToAdd = true;
						break;
					}
					itr++;
				}
				if(!StilToAdd)
					AddingToCluster = false; //Since its end of loop, we have no more continuity shape in main list
			}
			//Now we need to validate clusterlist to see if its a closed loop
			if(abs((*(--(ClosedShapeList.end()))).Pt2X - (*(ClosedShapeList.begin())).Pt1X) < 0.001 && abs((*(--(ClosedShapeList.end()))).Pt2Y - (*(ClosedShapeList.begin())).Pt1Y) < 0.001)
			{
				//Create new list for the new closed loop shapes
				ClosedShpsIDList = new std::list<int>;
				
				for each(Shapes Shp in ClosedShapeList)
				{
					//Fill the list with shape IDs in odd positions
					ClosedShpsIDList->push_back(Shp.ID);
					//Fill the list with filpped flag in even positions
					//ClosedShpsIDList->push_back(Shp.Flipped?1:0);
				}
				ClusterList->push_back(ClosedShpsIDList);	//Put the ID array into the clusterlist
			}
			ClosedShapeList.clear(); //clear the cluster shape list for next round of additions
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0110", __FILE__, __FUNCSIG__); }
}

//Gets a list of shapes that form a closed loop and returs an array stating which shapes need to be flipped for continuity
void RapidMath3D::GetDXFClosedLoopFlipState(double *ShapePts, int NoOfShapes, bool *FlipStateArray)
{
	try
	{
		FlipStateArray[0] = false;	//We take 1st shape as reference, and hence it shall not be flipped
		for(int i = 0; i < NoOfShapes - 1; i++)
		{
			//if the current shape is not to be flipped
			if(!FlipStateArray[i])
			{
				//Compare pt2 of current shape with pt1 of next shape
				if(ShapePts[i * 4 + 2] == ShapePts[(i + 1) * 4] && ShapePts[i * 4 + 3] == ShapePts[(i + 1) * 4 + 1])
					//There is continuity
					FlipStateArray[i + 1] = false;
				else
					//There is no continuity. Hence flip the next shape
					FlipStateArray[i + 1] = true;
			}
			else
			{
				//Compare pt1 of current shape with pt1 of next shape
				if(ShapePts[i * 4] == ShapePts[(i + 1) * 4] && ShapePts[i * 4 + 1] == ShapePts[(i + 1) * 4 + 1])
					//There is continuity
					FlipStateArray[i + 1] = false;
				else
					//There is no continuity. Hence flip the next shape
					FlipStateArray[i + 1] = true;
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0111", __FILE__, __FUNCSIG__); }
}



//--------------------------------------------------------------------------------------
//Base plate allignment function
//-------------------------------------------------------------------------------------- 

//Function that calculates the transformation matrix by analysing the base entities with the secondary entities
void RapidMath3D::TransformationMatrix_BasePlaneAllignment(double *BasePlane, double *BaseLine, double *BasePoint, double *OtherPlane, double *OtherLine, double *OtherPoint, double *TransMatrix, double *BaseOrigin, double *OtherOrigin, bool usingFrameGrab)
{
	try
	{
		double BaseProjectedLine[6], OtherProjectedLine[6], TmpDir1[3], TmpDir2[3], TmpMatrix[16] = {0}, Frameplane[3]= {0,0,1};
		int PlaneFlipMultiplier = 1;
		memset(TransMatrix,0,16*sizeof(double));
		memset(BaseOrigin,0,3*sizeof(double));
		memset(OtherOrigin,0,3*sizeof(double));
		//Check if the other plane is directed opposite to the base plane. If yes, flip it to make it compatible with the base plane
		/*if(usingFrameGrab)
		{
			if(BasePlane[2] < 0)
				Frameplane[2] = -1;
			for(int i = 0; i < 3; i++)
			{
				BasePlane[i]  = Frameplane[i];
				TmpOtherPlane[i] = Frameplane[i];
			}	
		}*/
		//Calculate the 2 origins
		Projection_Line_on_Plane(BaseLine, BasePlane, BaseProjectedLine);
		Projection_Point_on_Line(BasePoint, BaseProjectedLine, BaseOrigin);

		Projection_Line_on_Plane(OtherLine, OtherPlane, OtherProjectedLine);
		Projection_Point_on_Line(OtherPoint, OtherProjectedLine, OtherOrigin);

		//We have one direction(plane normal), a perpendicular direction to that(line direction). Calculate a direction perpendicular to these two
		//We can use these mutually perpendicular directions to calculate the angles they make with the base cordinate system
		//Now these angles can be used to find the transformed coordinate system that the other entities define
		Create_Perpendicular_Direction_2_2Directions(BasePlane, &BaseProjectedLine[3], TmpDir1);
		Create_Perpendicular_Direction_2_2Directions(OtherPlane, &OtherProjectedLine[3], TmpDir2);

		//First lets calculate the x-axis part(1st row) of the trans matrix
	/*	double eq1[4] = {TmpOtherPlane[0], TmpOtherPlane[1], TmpOtherPlane[2], BasePlane[0]};
		double eq2[4] = {OtherProjectedLine[3], OtherProjectedLine[4], OtherProjectedLine[5], BaseProjectedLine[3]};
		double eq3[4] = {TmpDir2[0], TmpDir2[1], TmpDir2[2], TmpDir1[0]};
		SolveEquation(eq1, eq2, eq3, &TmpMatrix[0]);*/

		double eq1[4] = {BasePlane[0], BasePlane[1], BasePlane[2], OtherPlane[0]};
		double eq2[4] = {BaseProjectedLine[3], BaseProjectedLine[4], BaseProjectedLine[5], OtherProjectedLine[3]};
		double eq3[4] = {TmpDir1[0], TmpDir1[1], TmpDir1[2], TmpDir2[0]};
		SolveEquation(eq1, eq2, eq3, &TransMatrix[0]);
		//Next we calculate the y-axis part(2nd row) of the trans matrix, in a similar fashion to the above
		//eq1[3] = BasePlane[1];
		//eq2[3] = BaseProjectedLine[4];
		//eq3[3] = TmpDir1[1];
		//SolveEquation(eq1, eq2, eq3, &TmpMatrix[4]);


		eq1[3] = OtherPlane[1];
		eq2[3] = OtherProjectedLine[4];
		eq3[3] = TmpDir2[1];
		SolveEquation(eq1, eq2, eq3, &TransMatrix[4]);
		//Now using the 1st row(X axis) and 2nd row(Y axis), we calculate the 3rd row(Z axis)
	/*	Create_Perpendicular_Direction_2_2Directions(&TmpMatrix[0], &TmpMatrix[4], &TmpMatrix[8]);*/

		Create_Perpendicular_Direction_2_2Directions(&TransMatrix[0], &TransMatrix[4], &TransMatrix[8]);
		//Fill the rest of the transformation matrix
		/*TmpMatrix[15] = 1;*/
		TransMatrix[15] = 1;

		//Calculate the inverse matrix
	/*	OperateMatrix4X4(TmpMatrix, 4, 1, TransMatrix);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0112", __FILE__, __FUNCSIG__); }
}


//--------------------------------------------------------------------------------------
//Mirroring functions
//-------------------------------------------------------------------------------------- 

//Function to get the mirror or a point wrt a plane
void RapidMath3D::Mirror_Pt_wrt_Plane(double *Point, double *Plane, double *MirrorPoint)
{
	try
	{
		double dist = 2 * Distance_Point_Plane_with_Dir(Point, Plane);  //multiply by 2 so that we have the distance between the original and the mirror pt
		
		//Now distance will be positive if pt on the upper side of plane, else it will be -ve
		//In any case we need to subtract the dist(i.e. 2 x dist between pt and plane) along the direction of normal to the plane
		//if original pt on the upper side, dist is +ve, mirrorpt on the lower side of plane after subtraction
		//The case becomes reversed if original pt on the lower side of the plane
		MirrorPoint[0] = Point[0] - dist * Plane[0];
		MirrorPoint[1] = Point[1] - dist * Plane[1];
		MirrorPoint[2] = Point[2] - dist * Plane[2];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0113", __FILE__, __FUNCSIG__); }
}



//--------------------------------------------------------------------------------------
//IGES Allignment
//--------------------------------------------------------------------------------------

//function to calculate transformation matrix for IGES alignment...............
void RapidMath3D::TransformationMatrix_IGESAlignment_1Line(double *igeslinePts, double *userlinePts, double *userpt, double *transformMatrix)
 {
	 try
	 {
		   double DCigesLn[3], DCuserLn[3];
		   
		   //first calculate direction cosines of iges and all the 3 perpend directions for it
		   DCs_TwoEndPointsofLine(igeslinePts, DCigesLn);
		   double igesLine[6] = {igeslinePts[0], igeslinePts[1], igeslinePts[2], DCigesLn[0], DCigesLn[1], DCigesLn[2]};
		   double igesPerpendLine[3], igesZDir[3];
		   double xAxis_point[3] = {1, 0, 0}, yAxis_point[3] = {0, 1, 0};
		   if(!Create_Perpendicular_Direction_2_2Directions(&igesLine[3],  xAxis_point, igesPerpendLine))
		   {
			   Create_Perpendicular_Direction_2_2Directions(&igesLine[3],  yAxis_point, igesPerpendLine);
		   }
		   Create_Perpendicular_Direction_2_2Directions(&igesLine[3],  &igesPerpendLine[0], igesZDir);
		   
		   //next calculate direction cosines of user and all the 3 perpend directions for it
		   DCs_TwoEndPointsofLine(userlinePts, DCuserLn);
		   double userLine[6] = {userlinePts[0], userlinePts[1], userlinePts[2], DCuserLn[0], DCuserLn[1], DCuserLn[2]};
		   double userPerpendLine[6], userZDir[3];
		   Create_Perpendicular_Line_2_Line(userpt, userLine, userPerpendLine);
		   Create_Perpendicular_Direction_2_2Directions(&userLine[3], &userPerpendLine[3], userZDir);

		   //First lets calculate the x-axis part(1st row) of the trans matrix......
		   double eq1[4] = {igesLine[3], igesLine[4], igesLine[5], userLine[3]};
		   double eq2[4] = {igesPerpendLine[0], igesPerpendLine[1], igesPerpendLine[2], userPerpendLine[3]};
		   double eq3[4] = {igesZDir[0], igesZDir[1], igesZDir[2], userZDir[0]};
		   SolveEquation(eq1, eq2, eq3, &transformMatrix[0]);
		   //Next we calculate the y-axis part(2nd row) of the trans matrix, in a similar fashion to the above.....
		   eq1[3] = userLine[4];
		   eq2[3] = userPerpendLine[4];
		   eq3[3] = userZDir[1];
		   SolveEquation(eq1, eq2, eq3, &transformMatrix[4]);
		   //Now using the 1st row(X axis) and 2nd row(Y axis), we calculate the 3rd row(Z axis)
		   Create_Perpendicular_Direction_2_2Directions(&transformMatrix[0], &transformMatrix[4], &transformMatrix[8]);
		   // transformation matrix............
		   transformMatrix[3] = 0, transformMatrix[7] = 0, transformMatrix[11] = 0, transformMatrix[12] = 0, transformMatrix[13] = 0, transformMatrix[14] = 0, transformMatrix[14] = 1;
				  
	 }  

	catch(...){ SetAndRaiseErrorMessage("RMATH3D0114", __FILE__, __FUNCSIG__); }
}

// function to calculate direction cosines of line if end points are given........
void RapidMath3D::DCs_TwoEndPointsofLine(double *point1, double *ans)
 {
	 try
	 {
		 double directionRatio[3] = {point1[0] - point1[3], point1[1] - point1[4], point1[2] - point1[5]};
		 DirectionCosines(directionRatio, ans);

	 }
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0115", __FILE__, __FUNCSIG__); }
}

//function to calculate determinant of 3*3 matrix....
double RapidMath3D::Determinant_3x3Matrix(double *matrix)
{
	 try
	 {
		 double determinant = matrix[0] * matrix[4] * matrix[8] + matrix[1] * matrix[5] * matrix[6] + matrix[2] * matrix[3] * matrix[7] - matrix[6] * matrix[4] * matrix[2] - matrix[0] * matrix[7] * matrix[5] - matrix[8] * matrix[3] * matrix[1];
		 return determinant;
	 }
	 catch(...){ SetAndRaiseErrorMessage("RMATH3D0116", __FILE__, __FUNCSIG__); return 0; }
}

 

//--------------------------------------------------------------------------------------
//Shortest Path algorithm for Partprogram
//--------------------------------------------------------------------------------------

void RapidMath3D::ShortestPathForPP(double *PPLocationPts, int *PtID, int NoOfPts, int *ArrangedIDs)
{
	try
	{
		struct Pt
		{
			double X, Y;
			int id;
		}*TmpPt, *NearestPt;
		double NearestDist;
		std::list<Pt*> AllptsList, ArrangedPtsList;

		for(int i = 0; i < NoOfPts; i++)
		{
			TmpPt = new Pt();
			TmpPt->X = PPLocationPts[2 * i];
			TmpPt->Y = PPLocationPts[2 * i + 1];
			TmpPt->id = PtID[i];
			AllptsList.push_back(TmpPt);
		}
		
		TmpPt = *(AllptsList.begin());
		AllptsList.pop_front();
		ArrangedPtsList.push_back(TmpPt);
		while((int)AllptsList.size() != 0)
		{
			TmpPt = *(--(ArrangedPtsList.end()));
			NearestPt = *(AllptsList.begin());
			NearestDist = sqrt(pow(NearestPt->X - TmpPt->X, 2) + pow(NearestPt->Y - TmpPt->Y, 2));
			for each(Pt* LstPt in AllptsList)
			{
				if(sqrt(pow(TmpPt->X - LstPt->X, 2) + pow(TmpPt->Y - LstPt->Y, 2)) < NearestDist)
				{
					NearestDist = sqrt(pow(TmpPt->X - LstPt->X, 2) + pow(TmpPt->Y - LstPt->Y, 2));
					NearestPt = LstPt;
				}
			}
			AllptsList.remove(NearestPt);
			ArrangedPtsList.push_back(NearestPt);
		}

		int i = 0;
		for each(Pt* LstPt in ArrangedPtsList)
		{
			ArrangedIDs[i++] = LstPt->id;
			delete(LstPt);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0117", __FILE__, __FUNCSIG__); }
}
//function to calculate transformation matrix for IGES alignment...............
void RapidMath3D::TransformationMatrix_IGESAlignment_1Pt1Line(double *igeslinePts, double *igespt, double *userlinePts, double *userpt, double *transformMatrix)
 {
	 try
	 {
		   double DCigesLn[3], DCuserLn[3];
		   
		   //first calculate direction cosines of iges and all the 3 perpend directions for it
		   DCs_TwoEndPointsofLine(igeslinePts, DCigesLn);
		   double igesLine[6] = {igeslinePts[0], igeslinePts[1], igeslinePts[2], DCigesLn[0], DCigesLn[1], DCigesLn[2]};
		   double igesPerpendLine[6], igesZDir[3];
		   Create_Perpendicular_Line_2_Line(igespt, igesLine, igesPerpendLine);
		   Create_Perpendicular_Direction_2_2Directions(&igesLine[3], &igesPerpendLine[3], igesZDir);
		   
		   //next calculate direction cosines of user and all the 3 perpend directions for it
		   DCs_TwoEndPointsofLine(userlinePts, DCuserLn);
		   double userLine[6] = {userlinePts[0], userlinePts[1], userlinePts[2], DCuserLn[0], DCuserLn[1], DCuserLn[2]};
		   double userPerpendLine[6], userZDir[3];
		   Create_Perpendicular_Line_2_Line(userpt, userLine, userPerpendLine);
		   Create_Perpendicular_Direction_2_2Directions(&userLine[3], &userPerpendLine[3], userZDir);

		   //First lets calculate the x-axis part(1st row) of the trans matrix......
		   double eq1[4] = {igesLine[3], igesLine[4], igesLine[5], userLine[3]};
		   double eq2[4] = {igesPerpendLine[3], igesPerpendLine[4], igesPerpendLine[5], userPerpendLine[3]};
		   double eq3[4] = {igesZDir[0], igesZDir[1], igesZDir[2], userZDir[0]};
		   SolveEquation(eq1, eq2, eq3, &transformMatrix[0]);
		   //Next we calculate the y-axis part(2nd row) of the trans matrix, in a similar fashion to the above.....
		   eq1[3] = userLine[4];
		   eq2[3] = userPerpendLine[4];
		   eq3[3] = userZDir[1];
		   SolveEquation(eq1, eq2, eq3, &transformMatrix[4]);
		   //Now using the 1st row(X axis) and 2nd row(Y axis), we calculate the 3rd row(Z axis)
		   Create_Perpendicular_Direction_2_2Directions(&transformMatrix[0], &transformMatrix[4], &transformMatrix[8]);
		   // transformation matrix............
		   transformMatrix[3] = 0, transformMatrix[7] = 0, transformMatrix[11] = 0, transformMatrix[12] = 0, transformMatrix[13] = 0, transformMatrix[14] = 0, transformMatrix[14] = 1;
				  
	 }  

	catch(...){ SetAndRaiseErrorMessage("RMATH3D0114", __FILE__, __FUNCSIG__); }
}


bool RapidMath3D::Circle_3Pt_3D(double* P, double* Ans)
{
	try
	{
		double v1[3] = {*(P+3) - *(P+0), *(P+4) - *(P+1), *(P+5) - *(P+2)};
		double v2[3] = {*(P+6) - *(P+3), *(P+7) - *(P+4), *(P+8) - *(P+5)};


		double w[3] = {v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]};

		double norm = sqrt(pow(w[0],2) + pow(w[1],2) + pow(w[2],2));
		if (norm==0) return false;
		for (int i = 0; i < 3; i++)
			w[i] /= norm;

		double x1[3] = {(*(P+3) + *(P+0))/2, (*(P+4) + *(P+1))/2, (*(P+5) + *(P+2))/2};
		double x2[3] = {(*(P+6) + *(P+3))/2, (*(P+7) + *(P+4))/2, (*(P+8) + *(P+5))/2};

		norm = sqrt(pow(v1[0],2) + pow(v1[1],2) + pow(v1[2],2));
		for (int i = 0; i < 3; i++)
			v1[i] /= norm;

		norm = sqrt(pow(v2[0],2) + pow(v2[1],2) + pow(v2[2],2));
		for (int i = 0; i < 3; i++)
			v2[i] /= norm;

		double l1[3] = {v1[1]*w[2] - v1[2]*w[1], v1[2]*w[0] - v1[0]*w[2], v1[0]*w[1] - v1[1]*w[0]};
		double l2[3] = {v2[1]*w[2] - v2[2]*w[1], v2[2]*w[0] - v2[0]*w[2], v2[0]*w[1] - v2[1]*w[0]};
		double line1[6] = {x1[0], x1[1], x1[2], l1[0], l1[1], l1[2]};
		double line2[6] = {x2[0], x2[1], x2[2], l2[0], l2[1], l2[2]};

		double center[3] = {0};
		if (!Intersection_Line_Line(line1, line2, center))
			return false;

		double radius = Distance_Point_Point(P, center);

		*(Ans +0) = center[0]; *(Ans +1) = center[1]; *(Ans +2) = center[2]; *(Ans +3) = w[0]; *(Ans +4) = w[1]; *(Ans + 5) = w[2]; *(Ans + 6) = radius;
		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RMATH3D0115a", __FILE__, __FUNCSIG__); return false;}
}
bool RapidMath3D::GetIntermediateCirclePts(double* currentpos, double* intpt, double* target, int noofintstops, double* intermediatePts)
{
	try
	{
		double Pts[9] = {0};
		for (int i = 0; i < 3; i++)
		{
			Pts[i] = currentpos[i];
			Pts[3+i] = intpt[i];
			Pts[6+i] = target[i];
		}
		double circle3d[7] = {0};
		if (!Circle_3Pt_3D(Pts,circle3d))
			return false;
		double ray1[3], ray2[3], ray3[3];
		for (int i = 0; i < 3; i++)
		{
			ray1[i] = currentpos[i] - circle3d[i];
			ray2[i] = intpt[i] - circle3d[i];
			ray3[i] = target[i] - circle3d[i];
		}
		double norm =0;
		for (int i = 0; i<3; i++) norm += ray1[i]*ray1[i];
		for (int i = 0; i<3; i++) ray1[i] /= sqrt(norm);

		norm = 0;
		for (int i = 0; i<3; i++) norm += ray2[i]*ray2[i];
		for (int i = 0; i<3; i++) ray2[i] /= sqrt(norm);

		norm = 0;
		for (int i = 0; i<3; i++) norm += ray3[i]*ray3[i];
		for (int i = 0; i<3; i++) ray3[i] /= sqrt(norm);

		double y[3] = {circle3d[4]*ray1[2] - circle3d[5]*ray1[1], circle3d[5]*ray1[0] - circle3d[3]*ray1[2], circle3d[3]*ray1[1] - circle3d[4]*ray1[0]};
		double angle1, angle2;

		double c1 = 0;
		for (int i = 0; i < 3; i++) c1 += ray1[i]*ray3[i];
		double s1 = 0;
		for (int i = 0; i < 3; i++) s1 += y[i]*ray3[i];

		angle1 = acos(c1);
		if (s1 < 0)
			angle1 = 2 * M_PI - angle1;

		double c2 = 0;
		for (int i = 0; i < 3; i++) c2 += ray1[i]*ray2[i];
		double s2 = 0;
		for (int i = 0; i < 3; i++) s2 += y[i]*ray2[i];

		angle2 = acos(c2);
		if (s2 < 0)
			angle2 = 2 * M_PI - angle2;

		if (angle2 > angle1)
		{
			angle1 -= 2 * M_PI;
			angle2 -= 2 * M_PI;
		}

		double incrangle = angle1/noofintstops;
		double tmpangle = 0;
		for (int i = 1; i < noofintstops; i++)
		{
			tmpangle = i * incrangle;
			for (int j = 0; j < 3; j++)
			{
				*(intermediatePts + 3 * (i - 1) + j) = circle3d[j] + circle3d[6] * (ray1[j] * cos (tmpangle) + y[j] * sin (tmpangle));
			}
		}
		for (int j = 0; j < 3; j++)
		{
			*(intermediatePts + 3 * (noofintstops - 1) + j) = target[j];
		}
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RMATH3D0115", __FILE__, __FUNCSIG__);
		return false;
	}
}

void RapidMath3D::MousePntsForLineIntersection(double *endPnts1, double *endPnts2, double *IntersectPnts, double *mousePnts, double *measurePnts)
{
	try
	{
		double DirCos1[3] = {0}, DirCos2[3] = {0}, DirCos3[3] = {0}, DirCos4[3] = {0}, DirCos5[3] = {0};
		double *finalDir1, *finalDir2;
		double dir1[3] = {endPnts1[0] - IntersectPnts[0], endPnts1[1] - IntersectPnts[1], endPnts1[2] - IntersectPnts[2]};
		double dir2[3] = {endPnts1[3] - IntersectPnts[0], endPnts1[4] - IntersectPnts[1], endPnts1[5] - IntersectPnts[2]};
		double dir3[3] = {endPnts2[0] - IntersectPnts[0], endPnts2[1] - IntersectPnts[1], endPnts2[2] - IntersectPnts[2]};
		double dir4[3] = {endPnts2[3] - IntersectPnts[0], endPnts2[4] - IntersectPnts[1], endPnts2[5] - IntersectPnts[2]};
		double currentdir[3] = {mousePnts[0] - IntersectPnts[0], mousePnts[1] - IntersectPnts[1], mousePnts[2] - IntersectPnts[2]};

		DirectionCosines(dir1, DirCos1);
		DirectionCosines(dir2, DirCos2);
		DirectionCosines(dir3, DirCos3);
		DirectionCosines(dir4, DirCos4);
		DirectionCosines(currentdir, DirCos5);
		double dist = Distance_Point_Point(mousePnts, IntersectPnts);

		if(CheckVectorBtwn2Vectors(DirCos1, DirCos3,  DirCos5, false))
		{
			finalDir1 = &DirCos1[0];
			finalDir2 = &DirCos3[0];
		}
		else if(CheckVectorBtwn2Vectors(DirCos3, DirCos2,  DirCos5, false))
		{
			finalDir2 = &DirCos2[0];
			finalDir1 = &DirCos3[0];
		}
		else if(CheckVectorBtwn2Vectors(DirCos2, DirCos4,  DirCos5, false))
		{
			finalDir1 = &DirCos2[0];
			finalDir2 = &DirCos4[0];
		}
		else if(CheckVectorBtwn2Vectors(DirCos4, DirCos1,  DirCos5, false))
		{
			finalDir2 = &DirCos1[0];
			finalDir1 = &DirCos4[0];
		}
		double finaldir[3] = {0},  finalcos[3] = {0};
		for(int i = 0; i < 3; i++)
		   finaldir[i] = ((finalDir1[i]) + (finalDir2[i]));
		DirectionCosines(finaldir, finalcos);
		for(int i = 0; i < 3; i++)
		   measurePnts[i] = IntersectPnts[i]  + dist * (finalcos[i]);
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0075a", __FILE__, __FUNCSIG__); return; }
}

void RapidMath3D::ReflectedPointsAlongPlane(double *points, int pointsSize, double *reflectionSurfaceParam, double *reflectedPoints)
{
	double tmpPoint[3] = {0};
	for(int i = 0; i < pointsSize; i++)
	{
	   Projection_Point_on_Plane(&points[3 * i], reflectionSurfaceParam, tmpPoint);
	   for(int j = 0; j < 3; j++) {reflectedPoints[3 * i + j] = 2 * tmpPoint[j] - points[3 * i + j];}
	}
}

void RapidMath3D::ReflectedPointsAlongLine(double *points, int pointsSize, double *reflectionSurfaceParam, double *reflectedPoints)
{
	double tmpPoint[3] = {0};
	for(int i = 0; i < pointsSize; i++)
	{
	   Projection_Point_on_Line(&points[3 * i], reflectionSurfaceParam, tmpPoint);
	   for(int j = 0; j < 3; j++) {reflectedPoints[3 * i + j] = 2 * tmpPoint[j] - points[3 * i + j];}
	}

}

bool RapidMath3D::MultiplyMatrix1(double *M1, int *S1, double *M2, int *S2, double *answer)
{
	try
	{
		//For multiplication the no. of columns of the first matrix should be equal to no. of rows of the second matrix//
		if(S1[1] != S2[0]) return false; 
		//Sum of product of row elements of first matrix with  the column elements of second//
		for(int i = 0; i < S1[0]; i++)
		{	
			for(int j = 0; j < S2[1]; j++)
			{	
				//Initialing the answer to zero after calculation of each element of matrix//
				answer[S2[1] * i + j] = 0;
				for(int k = 0; k < S2[0]; k++)
				{	
					//answer holds the result of matrix multiplication(sum of the products)//
					answer[S2[1] * i + j] += M1[S1[1] * i + k] * M2[S2[1] * k + j];
				}
			}	
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH3D0025b", __FILE__, __FUNCSIG__); return false; }
}

void RapidMath3D::Intersection_Cylinder_Cone(double *cylinderParam, double *coneParam, double *cylinderCenters, double *coneCenters, double phi_min, double phi_max, double minIncrementFactor, double incrementFactor, double wupp, std::list<double> *IntersectionPoint)
{
	try
	{
		double ratio = (coneParam[7] - coneParam[6]) / Distance_Point_Point(coneCenters, &coneCenters[3]);
		double vx = cylinderCenters[3] - cylinderCenters[0], vy = cylinderCenters[4] - cylinderCenters[1], vz = cylinderCenters[5] - cylinderCenters[2];
		double v = sqrt(vx * vx + vy * vy + vz * vz);
		if(v == 0) return;
		double ProjctnPt[3] = {0}, CylinderX[3] = {0}, currentP[3] = {0}, vDiff = incrementFactor, LMin = 0, lenghtMin = 0, LMax = v, lenghtmax = v;
		int noofslices = (int)ceil(v/ vDiff);
		if(cylinderParam[3] == 0)
		{
			CylinderX[0] = 1;
		}
		else
		{
			double modv = sqrt(cylinderParam[3] * cylinderParam[3]  + cylinderParam[4] * cylinderParam[4]);
			CylinderX[0] =  (-1 * cylinderParam[4]) / modv;
			CylinderX[1] = cylinderParam[3] / modv;
		}
		double CylinderY[3] = {(cylinderParam[4] * CylinderX[2] - cylinderParam[5] * CylinderX[1]), (cylinderParam[5] * CylinderX[0] - cylinderParam[3] * CylinderX[2]), (cylinderParam[3] * CylinderX[1] - cylinderParam[4] * CylinderX[0])};
		while(vDiff >= minIncrementFactor)
		{
			for(int i = 0; i <= noofslices; i++)
			{
				double z1val = LMin + vDiff * i;
				if(z1val > LMax)
					z1val = LMax;
				double increAng = vDiff/cylinderParam[6];	
				for(double angle = phi_min; angle < phi_max; angle += increAng)
				{
					for(int i = 0; i < 3; i++)
					{
					  currentP[i] = cylinderCenters[i] + z1val * cylinderParam[3 + i] + cylinderParam[6] * (CylinderX[i] * cos(angle) + CylinderY[i] * sin(angle));
					}
					Projection_Point_on_Line(currentP, coneParam, ProjctnPt);
					double tmph = Distance_Point_Point(ProjctnPt, coneCenters);
					if(Checking_Point_within_Block(ProjctnPt, coneCenters, 2))
					{
						if(vDiff == minIncrementFactor)
						{
							double tmpr = abs(Distance_Point_Line(currentP, coneParam));
							double curntr = (coneParam[6] + tmph * ratio);
							if(abs(tmpr - curntr) < 2 * vDiff)
							{
								IntersectionPoint->push_back(currentP[0]);
								IntersectionPoint->push_back(currentP[1]);
								IntersectionPoint->push_back(currentP[2]);
							}
						}
						else if(lenghtMin == LMin)
							lenghtMin = z1val;
						else
							lenghtmax = z1val;
					}
				}
			}
			if(vDiff == minIncrementFactor)	break;
			vDiff /= 2;
			if(vDiff < minIncrementFactor) vDiff = minIncrementFactor;
			LMin = lenghtMin;
			LMax = lenghtmax;
			noofslices = (int)ceil((lenghtmax - lenghtMin)/ vDiff);
		}
	}
	catch(...)
	{
	}
}

void RapidMath3D::Intersection_Sphere_Cone(double *sphereParam, double *coneParam, double *coneCenters, double phi_min, double phi_max, double minIncrementFactor, double incrementFactor, double wupp, std::list<double> *IntersectionPoint, double *avgZ)
{
	try
	{
		double ratio = (coneParam[7] - coneParam[6]) / Distance_Point_Point(coneCenters, &coneCenters[3]);
		double v = 2 * sphereParam[3];
		double ProjctnPt[3] = {0}, sphereX[3] = {0}, currentP[3] = {0}, vDiff = incrementFactor, LMin = -sphereParam[3], lenghtMin = -sphereParam[3], LMax = sphereParam[3], lenghtmax = sphereParam[3];
		int noofslices = (int)ceil(v/ vDiff);
		if(coneParam[3] == 0)
		{
			sphereX[0] = 1;
		}
		else
		{
			double modv = sqrt(coneParam[3] * coneParam[3]  + coneParam[4] * coneParam[4]);
			sphereX[0] =  (-1 * coneParam[4]) / modv;
			sphereX[1] = coneParam[3] / modv;
		}
		double sphereY[3] = {(coneParam[4] * sphereX[2] - coneParam[5] * sphereX[1]), (coneParam[5] * sphereX[0] - coneParam[3] * sphereX[2]), (coneParam[3] * sphereX[1] - coneParam[4] * sphereX[0])};
	  
		while(vDiff >= minIncrementFactor)
		{
			bool btmFound = false, tpFound = false;
			for(int i = 0; i <= noofslices; i++)
			{
				double z1val = LMin + vDiff * i;
				if(z1val > LMax) {z1val = LMax;}
				double currentR = sqrt(pow(sphereParam[3],2) - pow(z1val, 2));
				double increAng = phi_max;
				if(currentR > 0.001) 
					increAng =  vDiff/currentR;
				bool found = false;
				for(double angle = phi_min; angle <= phi_max; angle += increAng)
				{
					for(int i = 0; i < 3; i++)
					{
					  currentP[i] = sphereParam[i] + z1val * coneParam[3 + i];
					  currentP[i] += currentR * (sphereX[i] * cos(angle) + sphereY[i] * sin(angle));
					}
					Projection_Point_on_Line(currentP, coneParam, ProjctnPt);
					double tmph = Distance_Point_Point(ProjctnPt, coneCenters);
					if((ProjctnPt[2] - coneCenters[2]) * (coneCenters[5] - coneCenters[2]) < 0)
						tmph -= tmph;
					double tmpr = abs(Distance_Point_Line(currentP, coneParam));
					double curntr = (coneParam[6] + tmph * ratio);
					if(abs(tmpr - curntr) < vDiff)
					{
						if(vDiff == minIncrementFactor)
						{					
							IntersectionPoint->push_back(currentP[0]);
							IntersectionPoint->push_back(currentP[1]);
							IntersectionPoint->push_back(currentP[2]);
							*avgZ += currentP[2];
						}
						found = true;
					}		
				}
				if(!found)
				{
					if(!btmFound) { lenghtMin = z1val;}
					else {tpFound = true;}
				}
				else
				{
					if(!btmFound) { btmFound = true; }
					if(!tpFound) { lenghtmax = z1val;}
					else {tpFound = false; lenghtmax = z1val;}

				}
				
			}
			if(vDiff == minIncrementFactor)
			{
				vDiff /= 2;
				break;
			}
			vDiff /= 2;
			if(vDiff < minIncrementFactor) vDiff = minIncrementFactor;
			LMin = lenghtMin;
			LMax = lenghtmax;
			noofslices = (int)ceil((lenghtmax - lenghtMin)/ vDiff);
		}
	}
	catch(...)
	{}
}

bool RapidMath3D::Intersection_Cylinder_Plane(double *cylinderParam, double *cylinderCenters, double *planeParam, double *planeCenter, double phi_min, double phi_max, double wupp, std::list<double> *IntersectionPoint)
{
	try
	{
		double increamentAng = M_PI/180,  ProjctnPt[3] = {0}, shiftedPoint[3] = {0}, intersectionPnts[6] = {0}, ang = 0, currentline[6] = {0}, rotationOrigin[6] ={0};
		int s1[2] = {3,3}, s2[2] = {3,1};
		Intersection_Line_Plane(cylinderParam, planeParam, rotationOrigin);
		for(int i = 0; i < 3; i++)
		{
			shiftedPoint[i] = planeCenter[i] - rotationOrigin[i];
			currentline[3 + i] = rotationOrigin[i];
		}
		   
		for(ang = 0; ang <= M_PI; ang += increamentAng)
		{
			double Rmatrix[9] = {pow(planeParam[0], 2) * (1 - cos(ang)) + cos(ang), planeParam[0] * planeParam[1] * (1 - cos(ang)) - planeParam[2] * sin(ang), planeParam[0] * planeParam[2] * (1 - cos(ang)) + planeParam[1] * sin(ang),
			planeParam[0] * planeParam[1] * (1 - cos(ang)) + planeParam[2] * sin(ang), pow(planeParam[1], 2) * (1 - cos(ang)) + cos(ang), planeParam[1] * planeParam[2] * (1 - cos(ang)) - planeParam[0] * sin(ang),
			planeParam[0] * planeParam[2] * (1 - cos(ang)) - planeParam[1] * sin(ang), planeParam[1] * planeParam[2] * (1 - cos(ang)) + planeParam[0] * sin(ang), pow(planeParam[2], 2) * (1 - cos(ang)) + cos(ang)};
			MultiplyMatrix1(Rmatrix, s1, shiftedPoint, s2, &rotationOrigin[3]);
			for(int i = 0; i < 3; i++)
			{ 
				rotationOrigin[3 + i] += rotationOrigin[i];
				currentline[i] = rotationOrigin[i];
			}
			DCs_TwoEndPointsofLine(rotationOrigin, &currentline[3]);
			int intpnt = Intersection_Line_Cylinder(currentline, cylinderParam, intersectionPnts);
			for(int k = 0; k < intpnt; k++)
			{
				Projection_Point_on_Line(&intersectionPnts[3 * k], cylinderParam, ProjctnPt);
				if(Checking_Point_within_Block(ProjctnPt, cylinderCenters, 2))
				{
					IntersectionPoint->push_back(intersectionPnts[3 * k]);
					IntersectionPoint->push_back(intersectionPnts[3 * k + 1]);
					IntersectionPoint->push_back(intersectionPnts[3 * k + 2]);
				}
			}
		}
		return(!IntersectionPoint->empty());
	}
	catch(...)
	{
	   return false;
	}
}

bool RapidMath3D::Intersection_Cone_Plane(double *coneParam, double *coneCenters, double *planeParam, double *planeCenter, double phi_min, double phi_max, double wupp, std::list<double> *IntersectionPoint, bool chkInBlock)
{
	try
	{
		  double tmpDir[3] = {0}, increamentAng = M_PI/180, ProjctnPt[3] = {0}, shiftedPoint[3] = {0}, intersectionPnts[6] = {0}, ang = 0, currentline[6] = {0}, rotationOrigin[6] ={0};
		  int s1[2] = {3,3}, s2[2] = {3,1};
		  Intersection_Line_Plane(coneParam, planeParam, rotationOrigin);
		  if(Distance_Point_Point(planeCenter, rotationOrigin) == 0)
		  {
			if(planeParam[3] == 0)
				tmpDir[0] = 1;
			else
			{
				double modv = sqrt(planeParam[0] * planeParam[0]  + planeParam[1] * planeParam[1]);
				tmpDir[0] =  (-1 * planeParam[1]) / modv;
				tmpDir[1] = planeParam[0] / modv;
			}
			for(int i = 0; i < 3; i++){planeCenter[i] += tmpDir[i] * 2;}
		  }
		  for(int i = 0; i < 3; i++)
		  {
			  shiftedPoint[i] = planeCenter[i] - rotationOrigin[i];
			  currentline[3 + i] = rotationOrigin[i];
		  }
		  for(ang = 0; ang <= M_PI; ang += increamentAng)
		  {
			   double Rmatrix[9] = {pow(planeParam[0], 2) * (1 - cos(ang)) + cos(ang), planeParam[0] * planeParam[1] * (1 - cos(ang)) - planeParam[2] * sin(ang), planeParam[0] * planeParam[2] * (1 - cos(ang)) + planeParam[1] * sin(ang),
			   planeParam[0] * planeParam[1] * (1 - cos(ang)) + planeParam[2] * sin(ang), pow(planeParam[1], 2) * (1 - cos(ang)) + cos(ang), planeParam[1] * planeParam[2] * (1 - cos(ang)) - planeParam[0] * sin(ang),
			   planeParam[0] * planeParam[2] * (1 - cos(ang)) - planeParam[1] * sin(ang), planeParam[1] * planeParam[2] * (1 - cos(ang)) + planeParam[0] * sin(ang), pow(planeParam[2], 2) * (1 - cos(ang)) + cos(ang)};
			   MultiplyMatrix1(Rmatrix, s1, shiftedPoint, s2, &rotationOrigin[3]);
			   for(int i = 0; i < 3; i++)
			   { 
					rotationOrigin[3 + i] += rotationOrigin[i];
					currentline[i] = rotationOrigin[i];
			   }
			   DCs_TwoEndPointsofLine(rotationOrigin, &currentline[3]);
			   int intpnt = Intersection_Line_Cone(currentline, coneParam, intersectionPnts);
			   for(int k = 0; k < intpnt; k++)
			   {
				  Projection_Point_on_Line(&intersectionPnts[3 * k], coneParam, ProjctnPt);
				  if(!chkInBlock || Checking_Point_within_Block(ProjctnPt, coneCenters, 2))
				  {
					  IntersectionPoint->push_back(intersectionPnts[3 * k]);
					  IntersectionPoint->push_back(intersectionPnts[3 * k + 1]);
					  IntersectionPoint->push_back(intersectionPnts[3 * k + 2]);
				  }
			   }
		   }
		  return(!IntersectionPoint->empty());
	}
	catch(...)
	{
	   return false;
	}
}

void RapidMath3D::RotationAround3DAxisThroughOrigin(double rotateAngle, double *axisDir, double *rMatrix)
{
	try
	{
		rMatrix[0] = pow(axisDir[0], 2) * (1 - cos(rotateAngle)) + cos(rotateAngle); 
		rMatrix[1] = axisDir[0] * axisDir[1] * (1 - cos(rotateAngle)) - axisDir[2] * sin(rotateAngle); 
		rMatrix[2] = axisDir[0] * axisDir[2] * (1 - cos(rotateAngle)) + axisDir[1] * sin(rotateAngle);
		rMatrix[3] = axisDir[0] * axisDir[1] * (1 - cos(rotateAngle)) + axisDir[2] * sin(rotateAngle);
		rMatrix[4] = pow(axisDir[1], 2) * (1 - cos(rotateAngle)) + cos(rotateAngle); 
		rMatrix[5] = axisDir[1] * axisDir[2] * (1 - cos(rotateAngle)) - axisDir[0] * sin(rotateAngle);
		rMatrix[6] = axisDir[0] * axisDir[2] * (1 - cos(rotateAngle)) - axisDir[1] * sin(rotateAngle);
		rMatrix[7] = axisDir[1] * axisDir[2] * (1 - cos(rotateAngle)) + axisDir[0] * sin(rotateAngle); 
		rMatrix[8] = pow(axisDir[2], 2) * (1 - cos(rotateAngle)) + cos(rotateAngle);
	}
	catch(...)
	{}
}

void RapidMath3D::CrossProductOf2Vectors(double *vect1, double *vect2, double *resultVect)
{
	try
	{
		resultVect[0] = vect1[1] * vect2[2] - vect1[2] * vect2[1];
		resultVect[1] = vect1[2] * vect2[0] - vect1[0] * vect2[2];
		resultVect[2] = vect1[0] * vect2[1] - vect1[1] * vect2[0];
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RMATH3D0120", __FILE__, __FUNCSIG__);
		return;
	}
}

void RapidMath3D::MovePointInGivenDirection(double *pnt, double *dir, double dist, double *movdPnt)
{
	for(int i = 0; i < 3; i++)
	{
	   movdPnt[i] = pnt[i] + dir[i] * dist;
	}
}

void RapidMath3D::LeftMostCloudPoints(double *pnts, int noOfPts, double *lineparam, std::list<double> *tmpList, int noOfCordinate)
{
	for(int i = 0; i < noOfPts; i++)
	{
		double tmpx = 0;
		if(abs(abs(lineparam[0]) - M_PI_2) > 0.001)
			tmpx = (pnts[noOfCordinate* i + 1] - lineparam[1]) / tan(lineparam[0]);
		else
			tmpx = lineparam[1];
		if(pnts[noOfCordinate * i] <= tmpx)
		{
			tmpList->push_back(pnts[noOfCordinate * i]);
			tmpList->push_back(pnts[noOfCordinate * i + 1]);
			if(noOfCordinate == 3)
				tmpList->push_back(pnts[noOfCordinate * i + 2]);
			else
				tmpList->push_back(0);
		}
	}
}

void RapidMath3D::RightMostCloudPoints(double *pnts, int noOfPts, double *lineparam, std::list<double> *tmpList, int noOfCordinate)
{
	for(int i = 0; i < noOfPts; i++)
	{
		double tmpx = 0;
		if(abs(abs(lineparam[0]) - M_PI_2) > 0.001)
			tmpx = (pnts[noOfCordinate* i + 1] - lineparam[1]) / tan(lineparam[0]);
		else
			tmpx = lineparam[1];
		if(pnts[noOfCordinate * i] >= tmpx)
		{
			tmpList->push_back(pnts[noOfCordinate * i]);
			tmpList->push_back(pnts[noOfCordinate * i + 1]);
			if(noOfCordinate == 3)
				tmpList->push_back(pnts[noOfCordinate * i + 2]);
			else
				tmpList->push_back(0);
		}
	}
}

void RapidMath3D::TopMostCloudPoints(double *pnts, int noOfPts, double *lineparam, std::list<double> *tmpList, int noOfCordinate)
{
	for(int i = 0; i < noOfPts; i++)
	{
		double tmpy = 0;
		if(abs(abs(lineparam[0]) - M_PI_2) > 0.001)
			tmpy = pnts[noOfCordinate * i] * tan(lineparam[0]) + lineparam[1];
		else
			tmpy = lineparam[1];
		if(pnts[noOfCordinate * i + 1] >= tmpy)
		{
			tmpList->push_back(pnts[noOfCordinate * i]);
			tmpList->push_back(pnts[noOfCordinate * i + 1]);
			if(noOfCordinate == 3)
				tmpList->push_back(pnts[noOfCordinate * i + 2]);
			else
				tmpList->push_back(0);
		}
	}
}
	
void RapidMath3D::BottomMostCloudPoints(double *pnts, int noOfPts, double *lineparam, std::list<double> *tmpList, int noOfCordinate)
{
	for(int i = 0; i < noOfPts; i++)
	{
		double tmpy = 0;
		if(abs(abs(lineparam[0]) - M_PI_2) > 0.001)
			tmpy = pnts[noOfCordinate * i] * tan(lineparam[0]) + lineparam[1];
		else
			tmpy = lineparam[1];
		if(pnts[noOfCordinate * i + 1] <= tmpy)
		{
			tmpList->push_back(pnts[noOfCordinate * i]);
			tmpList->push_back(pnts[noOfCordinate * i + 1]);
			if(noOfCordinate == 3)
				tmpList->push_back(pnts[noOfCordinate * i + 2]);
			else
				tmpList->push_back(0);
		}
	}
}

//--------------------------------------------------------------------------------------
//Shortest Path algorithm for Partprogram
//--------------------------------------------------------------------------------------

void RapidMath3D::RotatePoint_Around_C_Axis_BC(double *OriginalPt, double* Axis, double* RotatedPt)
{	//Rotate the point back to its original position IF there were no C Axis rotation.
	//Get C-Axis XY at this Z position
	double tx[2]; // = 0, tY = 0;
	this->GetC_Axis_XY(OriginalPt[2], Axis, &tx[0]);
	//shift the point with respect to this as origin
	double XY[2] = {0.0, 0.0};
	double xx = OriginalPt[0] - tx[0];// - 2 * MAINDllOBJECT->TIS_CAxis[0];
	double yy = OriginalPt[1] - tx[1];// - 2 * MAINDllOBJECT->TIS_CAxis[1];
	double costheta = cos(OriginalPt[3]);
	double sintheta = sin(OriginalPt[3]);
	//Rotate the point about the centre of C-Axis
	//double rr = sqrt(pow(xx - MAINDllOBJECT->TIS_CAxis[0], 2) + pow(yy - MAINDllOBJECT->TIS_CAxis[1], 2));
	//ofstream pointsFile;
	//pointsFile.open("D:\\SpacingPts.csv", ios::app);
	//pointsFile << Position[0] << "," << Position[1] << "," << Position[2] << "," << Position[3];

	XY[0] = xx * costheta + yy * sintheta; 
	XY[1] = yy * costheta - xx * sintheta;
	//Shift the point back to original coordinate system
	RotatedPt[0] = XY[0] + Axis[0];
	RotatedPt[1] = XY[1] + Axis[1];
	RotatedPt[2] = OriginalPt[2];
	RotatedPt[3] = OriginalPt[3];
	//pointsFile << Position[0] << "," << Position[1] << endl;
	//pointsFile.close();
	//Position[0] = rr*costheta + MAINDllOBJECT->TIS_CAxis[0];
	//Position[1] = rr*sintheta + MAINDllOBJECT->TIS_CAxis[1];
	
}

void RapidMath3D::RotatePoint_Around_C_Axis(double *OriginalPt, double* Axis, double *Centre, double* RotatedPt)
{	
	//Calculate runout at this point. 
	double Rx = 0;
	double Ry = 0;
	//Rx = tan(Axis[3]) * (OriginalPt[2] - Axis[2]) + Axis[0] - Centre[0];
	//Ry = tan(Axis[4]) * (OriginalPt[2] - Axis[2]) + Axis[1] - Centre[1];
	//Rx = [3Axis] * (OriginalPt[2] - Axis[2]) / Axis[5];// + Axis[0] - Centre[0];
	//Ry = Axis[4] * (OriginalPt[2] - Axis[2]) / Axis[5];// + Axis[1] - Centre[1]; // + RotatedPt[1];

	Rx = Axis[0] * (OriginalPt[2] - Centre[2]) + Axis[1];// ;
	Ry = Axis[2] * (OriginalPt[2] - Centre[2]) + Axis[3];// ;

	//Convert given point to polar coordinates with respect to Axis centre
	double tempAng = 0;
	if (OriginalPt[0] == Centre[0])
	{
		if (OriginalPt[1] > Centre[1]) 
			tempAng = M_PI_2;
		else
			tempAng = 3 * M_PI_2;
	}
	else
	{
		tempAng = atan((Centre[1] - OriginalPt[1])/(Centre[0] - OriginalPt[0]));
		if (OriginalPt[0] < Centre[0] && OriginalPt[1] >= Centre[1]) 
			tempAng += M_PI;
		else if (OriginalPt[0] < Centre[0] && OriginalPt[1] <= Centre[1]) 
			tempAng += M_PI;
		else if (OriginalPt[0] > Centre[0] && OriginalPt[1] <= Centre[1]) 
			tempAng += 2 * M_PI;
	}
	//Get the R value
	double rotateRad = sqrt(pow((Centre[0] - OriginalPt[0]), 2) + pow((Centre[1] - OriginalPt[1]), 2));

	//Rotate the point and give final answer
	RotatedPt[0] = Centre[0] + rotateRad * cos(tempAng - OriginalPt[3]) - Rx;
	RotatedPt[1] = Centre[1] + rotateRad * sin(tempAng - OriginalPt[3]) - Ry;
	RotatedPt[2] = OriginalPt[2];
	RotatedPt[3] = OriginalPt[3];
}

void RapidMath3D::GetC_Axis_XY(double ZPos, double* Axis, double *Ans)
{	
	//double r = (ZPos - Axis[2])/Axis[5];			//BCXValue use where you want
	//get X value
	*(Ans++) = ZPos * Axis[0] + Axis[1];
	//Get Y value
	*(Ans) = ZPos * Axis[2] + Axis[3];
	//Ans[0] = Axis[0];
	//Ans[1] = Axis[1];
}

void RapidMath3D::UnRotatePoint_Around_C_Axis_BC(double* RotatedPt, double* Axis, double* OriginalPt)
{
	double tx[2]; // = 0, tY = 0;
	this->GetC_Axis_XY(RotatedPt[2], Axis, &tx[0]);
	double XY[2] = {0.0, 0.0};
	XY[0] = RotatedPt[0] - Axis[0];
	XY[1] = RotatedPt[1] - Axis[1];
	
	double costheta = cos(RotatedPt[3]);
	double sintheta = sin(RotatedPt[3]);
	//shift the point with respect to this as origin
	double xx; //= OriginalPt[0] - tx[0];// - 2 * MAINDllOBJECT->TIS_CAxis[0];
	double yy; // = OriginalPt[1] - tx[1];// - 2 * MAINDllOBJECT->TIS_CAxis[1];
	//Rotate the point about the centre of C-Axis

	xx = XY[0] * costheta - XY[1] * sintheta; 
	yy = XY[0] * sintheta + XY[1] * costheta;
	//Shift the point back to original coordinate system
	OriginalPt[0] = xx + tx[0];
	OriginalPt[1] = yy + tx[1];
	OriginalPt[2] = RotatedPt[2];
	OriginalPt[3] = RotatedPt[3];
}

void RapidMath3D::UnRotatePoint_Around_C_Axis(double* RotatedPt, double* Axis, double *Centre, double* OriginalPt)
{
	//Calculate runout at this point. 
	double Rx = 0;
	double Ry = 0;
	//Rx = Axis[3] * (RotatedPt[2] - Axis[2]) / Axis[5] + RotatedPt[0]; // + Axis[0] - Centre[0]
	//Ry = Axis[4] * (RotatedPt[2] - Axis[2]) / Axis[5] + RotatedPt[1]; // + Axis[1] - Centre[1]
	Rx = (Axis[0] * (RotatedPt[2] - Centre[2]) + Axis[1]) + RotatedPt[0]; //
	Ry = (Axis[2] * (RotatedPt[2] - Centre[2]) + Axis[3]) + RotatedPt[1]; //

	//Rotate the point and give final answer
	double tempAng = 0;
	if (Rx == Centre[0])
	{
		if (Ry > Centre[1]) 
			tempAng = M_PI_2;
		else
			tempAng = 3 * M_PI_2;
	}
	else
	{
		tempAng = atan((Centre[1] - Ry)/(Centre[0] - Rx));
		if (Rx < Centre[0] && Ry >= Centre[1]) 
			tempAng += M_PI;
		else if (Rx < Centre[0] && Ry <= Centre[1]) 
			tempAng += M_PI;
		else if (Rx > Centre[0] && Ry <= Centre[1]) 
			tempAng += 2 * M_PI;
	}
	double rotateRad = sqrt(pow((Centre[0] - Rx), 2) + pow((Centre[1] - Ry), 2));

	OriginalPt[0] = Centre[0] + rotateRad * cos(tempAng + RotatedPt[3]);
	OriginalPt[1] = Centre[1] + rotateRad * sin(tempAng + RotatedPt[3]);
	OriginalPt[2] = RotatedPt[2];
	OriginalPt[3] = RotatedPt[3];
}


void RapidMath3D::Correct_BC_Tilt(double* Point, double* BC_Param)
{
	double r = (Point[2] - BC_Param[2])/BC_Param[5];			//BCXValue use where you want
	//get X value
	Point[0] -= r * BC_Param[3];
	//Get Y value
	Point[1] -= r * BC_Param[4];
}

void RapidMath3D::Restore_BC_Tilt(double* Point, double* BC_Param)
{
	double r = (Point[2] - BC_Param[2])/BC_Param[5];			//BCXValue use where you want
	//get X value
	Point[0] += r * BC_Param[3];
	//Get Y value
	Point[1] += r * BC_Param[4];
}

void RapidMath3D::Rotate_Point_2D(double* OriginalPt, double* Axis, double * RotatedPt, double Angle)
{
	//Convert given point to polar coordinates with respect to Axis centre
	double tempAng = 0;
	if (OriginalPt[0] == Axis[0])
	{
		if (OriginalPt[1] > Axis[1]) 
			tempAng = M_PI_2;
		else
			tempAng = 3 * M_PI_2;
	}
	else
	{
		tempAng = atan((Axis[1] - OriginalPt[1])/(Axis[0] - OriginalPt[0]));
		if (OriginalPt[0] < Axis[0] && OriginalPt[1] >= Axis[1]) 
			tempAng += M_PI;
		else if (OriginalPt[0] < Axis[0] && OriginalPt[1] <= Axis[1]) 
			tempAng += M_PI;
		else if (OriginalPt[0] > Axis[0] && OriginalPt[1] <= Axis[1]) 
			tempAng += 2 * M_PI;
	}
	//Get the R value
	double rotateRad = sqrt(pow((Axis[0] - OriginalPt[0]), 2) + pow((Axis[1] - OriginalPt[1]), 2));

	//Rotate the point and give final answer
	RotatedPt[0] = Axis[0] + rotateRad * cos(tempAng + Angle);
	RotatedPt[1] = Axis[1] + rotateRad * sin(tempAng + Angle);
	RotatedPt[2] = OriginalPt[2];
	RotatedPt[3] = OriginalPt[3];
}

