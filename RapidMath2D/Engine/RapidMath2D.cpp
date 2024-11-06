// RapidMath2D.cpp : Defines the exported functions for the DLL application.
//
//Includes standard header files...//

#include "stdafx.h"
#include <string>
#include "tchar.h"
#define _USE_MATH_DEFINES
#include <cmath> 
#include <math.h>
#include "RapidMath2D.h"
#include "Windows.h"


//Constructor....
RapidMath2D::RapidMath2D()
{
	RMath2DError = NULL;
}

//Destructor...
RapidMath2D::~RapidMath2D()
{
}

//To Raise the Error Happened..
void RapidMath2D::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(RMath2DError != NULL)
		RMath2DError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}




//Function to calculate point to point distance
double RapidMath2D::Pt2Pt_distance(double *point1, double *point2)
{
	try
	{
		/*__try 
		{
		}
		__finally
		{
		}*/
		double dis = 0;
		//Calculate the sum of square of the differences of each coordinate of points//// (x1-x2)^2 + (y1-y2)^2
		for(int i = 0; i < 2; i ++)
			dis += pow((point1[i] - point2[i]), 2);  
		// Square root of the sum of squares above is the distance between the pts//
		return sqrt(dis);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0001", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate point to point distance
double RapidMath2D::Pt2Pt_distance(double x1, double y1, double x2, double y2)
{
	double p1[2] = {x1, y1}, p2[2] = {x2, y2};
	double dist = Pt2Pt_distance(&p1[0], &p2[0]);
	return dist;
}

//function to find the linear shift
double RapidMath2D::LPt2Pt_distance(double *point1, double *point2, bool CurrentStatus, bool *NextStatus, double *mousept)
{
	try
	{
		double dist = 0;
		*NextStatus = CurrentStatus;
		if(CurrentStatus)
		{
			if(IsInBetween(mousept[0], point1[0], point2[0], MeasureDecValue) && !IsInBetween(mousept[1], point1[1], point2[1], MeasureDecValue))
			{
				*NextStatus = false;
				dist = abs(point1[0] - point2[0]);
			}
			else
				dist = abs(point1[1] - point2[1]);
		}
		else
		{
			if(IsInBetween(mousept[1], point1[1], point2[1], MeasureDecValue) && !IsInBetween(mousept[0], point1[0], point2[0], MeasureDecValue))
			{
				*NextStatus = true;
				dist = abs(point1[1] - point2[1]);
			}
			else
				dist = abs(point1[0] - point2[0]);
		}
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0017", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate the Xshift with some slope//
double RapidMath2D::LXPt2Pt_distance_angle(double *point1, double *point2, double slope)
{	
	try
	{
		double intercept1, slope1, dist = 0;
		//Xshift between the points with respect to given slope//
		if(slope == M_PI_2)
		{
			slope1 = 0;
			intercept1 = point1[1] - (tan(slope1)) * point1[0];
		}
		else if(slope == 0)
		{
			slope1 = M_PI_2;
			intercept1 = point1[0];
		}
		else
		{
			slope1 = atan(-1/tan(slope));
			intercept1 = point1[1] - (tan(slope1)) * point1[0];
		}
		//point to line distance//
		dist = Pt2Line_Dist(point2[0], point2[1], slope1, intercept1);
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0018", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate the Yshift with some slope//
double RapidMath2D::LYPt2Pt_distance_angle(double *point1, double *point2, double slope)
{
	try
	{
		double intercept1, dist = 0;
		//Xshift between the points with respect to given slope//
		if(slope == M_PI_2)
			intercept1 = point1[0];
		else
			intercept1 = point1[1] - (tan(slope)) * point1[0];
		dist = Pt2Line_Dist(point2[0], point2[1], slope, intercept1);
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0019", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the Point to line distance//
double RapidMath2D::Pt2Line_Dist(double x, double y, double slope, double intercept)
{
	try
	{
		double Dist = 0;
		if(IsEqual(slope, M_PI_2, MeasureDecValue) || IsEqual(slope, 3 * M_PI_2, MeasureDecValue))  
			Dist = abs(x - intercept);  
		else
			Dist = abs((tan(slope) * x + intercept - y) * cos(slope));
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0003", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the Point to line distance - Signed//
double RapidMath2D::Pt2Line_DistSigned(double x, double y, double slope, double intercept)
{
	try
	{
		double Dist = 0;
		if(IsEqual(slope, M_PI_2, MeasureDecValue) || IsEqual(slope, 3 * M_PI_2, MeasureDecValue))  
			Dist = x - intercept;  
		else
			Dist = (tan(slope) * x + intercept - y) * cos(slope);
		return Dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0004", __FILE__, __FUNCSIG__); return 0; }
}

//function to find the line to line distance //
double RapidMath2D::Line2Line_dis(double L1slope, double intercept1, double L2slope, double intercept2)
{
	try
	{
		double dist = 0;
		//take the slope range 0 t0 180//
		Angle_FirstScndQuad(&L1slope); Angle_FirstScndQuad(&L2slope);
		if(IsEqual(L1slope, L2slope, MeasureDecValue))
		{
			if(L1slope == M_PI_2) //If lines are vetical then the distance betwen them is diff. between intercepts//
				dist = abs(intercept1 - intercept2);
			else
				dist = abs((cos(L1slope)) * (intercept1 - intercept2));
		}
		return dist;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0006", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate the point to circle distance with respect mouse point//
double RapidMath2D::Pt2Circle_distance_Type(double *point, double *center, double radius, int type)
{
	try
	{
		double dis = 0, distance = 0;
		//Calculate the sum of square of the differences of each coordinate point and center// // (x1-x2)^2 + (y1-y2)^2
		for(int i = 0; i < 2; i ++)
			dis += pow((point[i] - center[i]), 2);
		if(type == 0)	//point to circumference distance (min)
			distance = abs(sqrt(dis) - radius);
		else if(type == 1)	//point to center distance//
			distance = sqrt(dis);
		else if(type == 2)	//point to circumference distance (max)
			distance = sqrt(dis) + radius;
		return distance;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0150", __FILE__, __FUNCSIG__); return 0; }
}

//Linear distance(X/Y shift) between point and circle..//
double RapidMath2D::LPt2Circle_distance_Type(double *point, double *center, double radius, int type)
{
	try
	{
		double distance = 0;
		if(type == 0)	//point to circumference distance (min)
			distance = abs(abs(center[0] - point[0]) - radius);
		else if(type == 1)	//point to center distance//
			distance = abs(center[0] - point[0]); 
		else if(type == 2)	//point to circumference distance (max)//
			distance = abs(center[0] - point[0]) + radius;
		else if(type == 3)	//point to circumference distance (min)
			distance = abs(abs(center[1] - point[1]) - radius);
		else if(type == 4)	//point to center distance//
			distance = abs(center[1] - point[1]);
		else if(type == 5)	//point to circumference distance (max)//
			distance = abs(center[1] - point[1]) + radius;
		return distance;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0158", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate the line to circle distance with respect mouse point//
double RapidMath2D::Line2Circle_distance_Type(double slope, double intercept, double *center, double radius, int type)
{
	try
	{
		double intersec[2], distance = 0;
		//Calculate the perpendicular intersection point from center and call point toline distance..//
		Point_PerpenIntrsctn_Line(slope, intercept, center, &intersec[0]);
		distance = Pt2Circle_distance_Type(&intersec[0], center, radius, type);
		return distance;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0152", __FILE__, __FUNCSIG__); return 0; }
}

//Circle to circle to distance with respect to mouse point..//
double RapidMath2D::Circle2Circle_distance_Type(double *center1, double *center2, double radius1, double radius2, int type)
{
	try
	{
		double distance = 0, dis = 0;
		//if both circle having same center(cocentric) then the circumference to circumference distance is diff. b/w the radii//
		if(center1[0] == center2[0] && center1[1] == center2[1])
			distance = abs(radius1 - radius2);
		//Calculate the sum of square of the differences of each coordinate of centers//// (x1-x2)^2 + (y1-y2)^2
		for(int i = 0; i < 2; i ++)
			dis += pow((*(center1 + i) - *(center2 + i)),2);
		if(type == 0)	//circumference to circumference distance (min)//
			distance = abs(sqrt(dis) - (radius1 + radius2));
		else if(type == 1)	//center to center distance//
			distance = sqrt(dis);
		else if(type == 2)	//circumference to circumference distance (max)
			distance = sqrt(dis) + radius1 + radius2;
		return distance;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0154", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath2D::Circle2Circle_distance_NewType(double *center1, double *center2, double radius1, double radius2, int type1, int type2)
{
	try
	{
		double distance = 0, dis = 0;
		//if both circle having same center(cocentric) then the circumference to circumference distance is diff. b/w the radii//
		if(center1[0] == center2[0] && center1[1] == center2[1])
			distance = abs(radius1 - radius2);
		//Calculate the sum of square of the differences of each coordinate of centers//// (x1-x2)^2 + (y1-y2)^2
		for(int i = 0; i < 2; i ++)
			dis += pow((*(center1 + i) - *(center2 + i)),2);
		if(type1 == 0 )	
		{
			if(type2 == 0 )	
			  distance = abs(sqrt(dis) - radius1 - radius2);
			else if(type2 == 1)	
			  distance = abs(sqrt(dis) - radius1);
			else if(type2 == 2)	
			  distance = abs(sqrt(dis) - radius1 + radius2);
		}
		else if(type1 == 1)	
		{
			if(type2 == 0 )	
			  distance = abs(sqrt(dis) - radius2);
			else if(type2 == 1)	
			 distance = sqrt(dis);
			else if(type2 == 2)	
			  distance = abs(sqrt(dis) + radius2);			
		}
		else if(type1 == 2)	//circumference to circumference distance (max)
		{
			if(type2 == 0 )	
			  distance = abs(sqrt(dis) + radius1 - radius2);
			else if(type2 == 1)	
			 distance = abs(sqrt(dis) + radius1);
			else if(type2 == 2)	
			  distance = abs(sqrt(dis) + radius1 + radius2);
			
		}
		return distance;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0154b", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate the linear distance between two circles//
double RapidMath2D::LCircle2Circle_distance_Type(double *center1, double *center2, double radius1, double radius2, int type)
{
	try
	{
		double distance = 0;
		if(type == 0)	//circumference to circumference distance (min)//
			distance = abs(abs(center1[0] - center2[0]) - (radius1 + radius2));
		else if(type == 1)	//center to center distance//
			distance = abs(center1[0] - center2[0]);
		if(type == 2)	//circumference to circumference distance (max)
			distance = abs(center1[0] - center2[0]) + radius1 + radius2;
		else if(type == 3)	//circumference to circumference distance (min)//
			distance = abs(abs(center1[1] - center2[1]) - (radius1 + radius2));
		else if(type == 4)	//center to center distance//
			distance = abs(center1[1] - center2[1]);
		else if(type == 5)	//circumference to circumference distance (max)
			distance = abs(center1[1] - center2[1]) + radius1 + radius2;
		return distance;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0155", __FILE__, __FUNCSIG__); return 0; }
}

//vinod.. Angle of Line With Two Pts wrt Xaxis 0 To 2PI
double RapidMath2D::AngleofLineWithTwoPtswrtXaxis0To2PI(double x1, double y1, double x2, double y2)		//direction of line with pt (x2,y2) is any where,and wrt (x1,y1)
{//if parameteres got in order (x1,y1,x2,y2) then here angle is A then if parameteres got in order (x2,y2,x1,y1) then angle will callculates here A-PI if(A>PI) and A+PI if(A<PI)
	double NeededSlope = 0.0, y2_minus_y1=0.0, x2_minus_x1=0.0;
		y2_minus_y1 = y2 - y1;		
		x2_minus_x1 = x2 - x1;
		NeededSlope = atan(y2_minus_y1/x2_minus_x1);							//(x2,y2) is in 1st coordinate wrt (x1,y1).. by default
		if(x2_minus_x1 < 0)
			NeededSlope = M_PI + NeededSlope;									//2nd and 3rd.. coordinate
		else if(x2_minus_x1 >= 0 && y2_minus_y1 < 0)                               //4th coordinate
			NeededSlope = (2 * M_PI) + NeededSlope;
		return NeededSlope;
}
	
//angle b/w two points with respect to  a center point ........vinod......07/03/2014
double RapidMath2D::AngleBwTwoPtswrtcenter(double x1, double y1, double x2, double y2, double centerx, double centery)
{
	//Modified by RAN on 28 Dec 2015. This will return the sweep angle from first point to second point in an anti-clockwise direction!
	double Angle1 = 0.0, Angle2 = 0.0 , BwAngle = 0.0;
	Angle1 = AngleofLineWithTwoPtswrtXaxis0To2PI(centerx, centery, x1, y1);
	Angle2 = AngleofLineWithTwoPtswrtXaxis0To2PI(centerx, centery, x2, y2);

	if (Angle1 <= Angle2)
		BwAngle = Angle2 - Angle1;
	else
		BwAngle = 2 * M_PI - Angle1 + Angle2; //Sweep the first point to 2*pi (or zero! = X-axis first quadrant as 0 position) and then sweep to second point which is the same as angle2!
	//BwAngle = abs(Angle1-Angle2);								
	//if(BwAngle>M_PI)						//if b/w angle is reflex angle... make it acute/Obtuse(means less then 180) angle
	//	BwAngle = (2*M_PI)-BwAngle;
	return BwAngle;
}

int RapidMath2D::GetZoneOfPtBetweenLines(double* L1, double* L2, double* p, double* ans, double* Radius)
{
	double centre[3] = { 0 };
	//The centre of rotation is the intersection pt of the two lines
	int gotIntPt = Line_lineintersection(L1[0], L1[1], L2[0], L2[1], centre);
	//Angle1 is the angle of the first Line from X Axis and so on in a 360 degree sense.
	double Angle1 = 0.0, Angle2 = 0.0, BwAngle = 0.0, originalAngle2 = 0.0, OriBwAngle = 0.0;
	int Zone = 0;
	Angle1 = L1[0]; //(centre[0], centre[1], centre[0] +  1.0, L1[0] * (centre[0] + 1.0) + L1[1]);
	Angle2 = L2[0]; // AngleofLineWithTwoPtswrtXaxis0To2PI(centre[0], centre[1], centre[0] + 1.0, L2[0] * (centre[0] + 1.0) + +L2[1]);
	BwAngle = AngleofLineWithTwoPtswrtXaxis0To2PI(centre[0], centre[1], p[0], p[1]);
	originalAngle2 = Angle2; OriBwAngle = BwAngle;
	//Now we make the first line the X-Axis. So we subtract each angle by Angle1. 
	BwAngle -= Angle1; Angle2 -= Angle1; //Angle1 = 0;
	//We need ANgle of point in a 360 degree sense. 
	if (BwAngle < 0) BwAngle += M_PI * 2;
	//We go to the complimentary angle for Zone finding for Angle2. So we add only PI if its negative! We are extending the line to the other side of the intersection pt
	if (Angle2 < 0) Angle2 += M_PI;

	//Now we check and get the zone in which the pt is present. 
	if (BwAngle < Angle2)
		Zone = 1;
	else if (BwAngle < M_PI)
		Zone = 2;
	else if (BwAngle < Angle2 + M_PI)
		Zone = 3;
	else
		Zone = 4;
	//Calculate radius at which pt is present - 
	*Radius = sqrt((p[0] - centre[0]) * (p[0] - centre[0]) + (p[1] - centre[1]) * (p[1] - centre[1]));

	//We get the correct touch pt based on the zone.
	GetPtInTwoLineQuadrant(Angle1, Angle2, Zone, centre, *Radius, ans);
	return Zone;
}

void RapidMath2D::GetPtInTwoLineQuadrant(double Angle1, double Angle2, int Zone, double* Centre, double rad, double* mPt)
{
	//Now we calculate angle bisector based on the zone... 
	double finalangle = Angle1 + Angle2 / 2;
	switch (Zone)
	{
	case 2:
		finalangle += M_PI_2;
		break;
	case 3:
		finalangle += M_PI;
		break;
	case 4:
		finalangle += M_PI_2 * 3;
		break;
	default:
		break;
	}
	mPt[0] = Centre[0] + rad * cos(finalangle);
	mPt[1] = Centre[1] + rad * sin(finalangle);
}

//function to calculate the line slope with respect to mouse point//
double RapidMath2D::Line_slopemousept(double slope, double intercept, double *mousept)
{
	try
	{
		double d, ang[4], angle = 0;
		Angle_FirstScndQuad(&slope);
		if(slope == 0 || slope == M_PI_2)
			return M_PI_2;
		else if(slope < M_PI_2 && slope > 0)
		{
			ang[0] = slope;
			ang[1] = M_PI_2 - slope;
			ang[2] = M_PI - (M_PI_2 - slope);
			ang[3] = M_PI - slope;
		}
		else
		{
			ang[0] = M_PI - slope;
			ang[1] = slope - M_PI_2;
			ang[2] = M_PI - ang[0];
			ang[3] = M_PI - ang[1];
		}
		//calculate the distance of the mouse point from the line//
		//to check the position of the point//
		d = Pt2Line_DistSigned(mousept[0], mousept[1], slope, intercept);
		//If the line slope is less than 90//
		if(slope < M_PI_2 && slope > 0)
		{
			if(d > 0 && mousept[1] > 0)
				angle = ang[0];
			else if(d < 0 && mousept[0] > 0)
				angle = ang[1];
			else if(d > 0 && mousept[1] < 0)
				angle = ang[3];
			else
				angle = ang[2];
		}
		else
		{
			if(d < 0 && mousept[1] > 0)
				angle = ang[0];
			else if(d > 0 && mousept[0] < 0)
				angle = ang[1];
			else if(d < 0 && mousept[1] < 0)
				angle = ang[2];
			else
				angle = ang[3];
		}
		return angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0164", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the angle between two lines//
double RapidMath2D::Line2Line_angle(double L1slope, double intercept1, double L2slope, double intercept2, double *mousept)
{
	try
	{
		double pt1[2], pt2[2], temp;
		double angle = 0;
		//take the slope range 0 t0 180//
		Angle_FirstScndQuad(&L1slope); Angle_FirstScndQuad(&L2slope);
		//If the two lines are parallel then return false//
		if(IsEqual(L1slope, L2slope, MeasureDecValue)) return 0;				 
		//new points on the line with repect to mouse move//
		//X value remains same ,  y1 = m1*x + c1 and y2 = m2*x+c2//
		if(L1slope == M_PI_2)
		{
			pt1[0] = intercept1;
			pt1[1] = mousept[1];
			temp = pt1[0];
		}
		else
		{
			pt1[0] = mousept[0];
			pt1[1] = ((tan(L1slope)) * mousept[0] + intercept1); 
			temp = pt1[1] ;
		}
		
		if(L2slope == M_PI_2)	
		{
			pt2[0] = intercept2;
			pt2[1] = mousept[1];
			temp = pt2[1];
		}
		else
		{
			pt2[0] = mousept[0];
			pt2[1] = ((tan(L2slope)) * mousept[0] + intercept2); 
			temp = pt2[1];
		}
		//checking position of the point on line1 and point on line2 with respect to intersection point//
		//considring that poisition calculate the angle between the two lines//
		//line1 to line2 in anticlockwise if the angle be "theta" then line1 to line2 in clockwise the angle should be PI - "theta"//
		
		if((pt1[1] < mousept[1]) && (pt2[1] < mousept[1])|| (pt1[1] > mousept[1]) && (pt2[1] > mousept[1]))
		{
			angle = abs(L2slope - L1slope);
			if(((L2slope > M_PI_2) && (L1slope > M_PI_2)) || ((L2slope < M_PI_2) && (L1slope < M_PI_2)))
			{
				angle = M_PI - angle;
			}
				
		}
		else if((pt1[1] < mousept[1]) && (pt2[1] > mousept[1])|| (pt1[1] > mousept[1]) && (pt2[1]  < mousept[1]))
		{
			if(((L2slope > M_PI_2) && (L1slope > M_PI_2)) || ((L2slope < M_PI_2) && (L1slope < M_PI_2)))
			{
				angle = abs(L2slope - L1slope); 
			}
			else
			{
				angle = abs(L2slope - L1slope); 
				angle = M_PI - angle; 
			}
		}
		else if((pt1[1] == mousept[1]) || (pt2[1] == mousept[1]))
		{
			if((pt2[1] < mousept[1]) || (pt1[1] < mousept[1]))
			{
				if((pt1[0] > mousept[0]) || (pt2[0] > mousept[0]))
				{
					angle = abs(L2slope - L1slope); 
					if((L2slope < M_PI_2) || (L1slope < M_PI_2))
						angle = M_PI - angle; 
				}
				else if((pt1[0] < mousept[0]) || (pt2[0] < mousept[0]))
				{
					angle = abs(L2slope - L1slope); 
					if((L2slope > M_PI_2) || (L1slope > M_PI_2))
						angle = M_PI - angle; 
				}
			}
			else if((pt2[1] > mousept[1]) || (pt1[1] > mousept[1]))
			{
				if((pt1[0] < mousept[0]) || (pt2[0] < mousept[0]))
				{
					angle = abs(L2slope - L1slope);
					if((L2slope < M_PI_2) || (L1slope < M_PI_2))
						angle = M_PI - angle; 
				}
				else if((pt1[0] > mousept[0]) || (pt2[0] > mousept[0]))
				{
					angle = abs(L2slope - L1slope); 
					if((L2slope > M_PI_2) || (L1slope > M_PI_2))
						angle = M_PI - angle; 
				} 
			}
		}
		return angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0007", __FILE__, __FUNCSIG__); return 0; }
}

//function to find the circle radius and center passing throu three points//
bool RapidMath2D::Circle_2Pt(double *point1, double *point2, double *center, double *radius)
{
	try
	{
		Mid_point(point1, point2, center);
		*radius = Pt2Pt_distance(center, point1);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0013", __FILE__, __FUNCSIG__); return false; }
}

//function to find the circle radius and center passing throu three points//
bool RapidMath2D::Circle_3Pt(double *point1, double *point2, double *point3, double *center, double *radius)
{
	try
	{
		double m, n, o, p, q, r, s;
		m = pow(point1[0], 2) + pow(point1[1], 2);
		n = pow(point2[0], 2) + pow(point2[1], 2);
		o = pow(point3[0], 2) + pow(point3[1], 2);
		p = point3[0] - point2[0];
		q = point2[0] - point1[0];
		r = point3[1] - point2[1];
		s = point2[1] - point1[1];
		//Calculating the X and Y co-ods of the centre of circle
		if((p * s - q * r)!= 0)
		{
			center[0] = -((n - o) * s - (m - n) * r) / (2 * ((p * s) - (q * r)));
			center[1] = -((n - o) * q - (m - n) * p) / (2 * ((q * r) - (p * s)));
			*radius = Pt2Pt_distance(center, point1);
			return true;
		}
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0013", __FILE__, __FUNCSIG__); return false; }
}

//function to calculate arc parameters : given two points and take mouse position as third point//
int RapidMath2D::Arc_3Pt(double *point1, double *point2, double *point3, double *center, double *radius, double *startang, double *sweepang)
{
	try
	{
		//calculate the center and radius of the arc using circle 3 point function//
		Circle_3Pt(point1, point3, point2, center, radius);
		//slope of the line joining the center and point1 --start angle//
		double slope1 = ray_angle(center, point1);
		//slope of the line joining the center and point2 -- end angle//
		double slope2 = ray_angle(center, point2);
		//slope of the line joining the center and point1 --start angle//
		double slope3 = ray_angle(center, point3);
		//for clockwise return 2 and for anticlockwise return 1..
		if(slope1 > slope2)
		{
			if(slope3 < slope1 && slope3 > slope2)
			{
				*startang = slope2;
				*sweepang = slope1 - slope2;
				return 2;
			}
			else
			{
				*startang = slope1;
				*sweepang = 2 * M_PI - (slope1 - slope2);
				return 1;
			}
		}
		else
		{
			if(slope3 < slope2 && slope3 > slope1)
			{
				*startang = slope1;
				*sweepang = slope2 - slope1;
				return 1;
			}
			else
			{
				*startang = slope2;
				*sweepang = 2 * M_PI - (slope2 - slope1);
				return 2;
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0014", __FILE__, __FUNCSIG__); return 0; }
}

//Arc with Fixed Radius..
bool RapidMath2D::Arc_FixedDia(double *center, double *point1, double *mousept, double radius, double *startang, double *sweepang, bool Anticlockwise)
{
	try
	{
		double slope1, slope2, temp;
		//slope of the line joining the center and point1 --start angle//
		slope1 = ray_angle(center, point1);
		//slope of the line joining the center and point2 -- end angle//
		slope2 = ray_angle(center, mousept);
		if(Anticlockwise)
		{
			*startang = slope1;
			temp = slope2 - slope1;
			if(temp < 0)
				temp += 2 * M_PI;
			*sweepang = temp;
		}
		else
		{
			*startang = slope2;
			temp = slope1 - slope2;
			if(temp < 0)
				temp += 2 * M_PI;
			*sweepang = temp;
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0015", __FILE__, __FUNCSIG__); return false; }
}

//One Point circle with fixed Diameter
bool RapidMath2D::Circle_1Pt_FixedDia(double *point1, double diameter, double *mousept, double *point2)
{
	try
	{
		//angle of the line joining the center and mouse point//
		double slope = ray_angle(point1, mousept);
		point2[0] = point1[0] + diameter * cos(slope);
		point2[1] = point1[1] + diameter * sin(slope);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0084", __FILE__, __FUNCSIG__); return false; }
}

//Three point circle with fixed dia
bool RapidMath2D::Circle_3Pt_FixedDia(double *point1, double *point2, double radius, double *mousept, double *center, double *point3)
{
	try
	{
		double slope, slope1;
		double d, pt1[2], pt2[2];
		//angle of the line joining the center and mouse point//
		slope = ray_angle(point1, point2);
		d = Pt2Pt_distance(point1, point2);
		slope1 = acos(d/(radius * 2));
		pt1[0] = point1[0] + radius * cos(slope + slope1);
		pt1[1] = point1[1] + radius * sin(slope + slope1);
		pt2[0] = point1[0] + radius * cos(slope - slope1);
		pt2[1] = point1[1] + radius * sin(slope - slope1);
		if(Pt2Pt_distance(&pt1[0], mousept) < Pt2Pt_distance(&pt2[0], mousept))
		{
			center[0] = pt1[0]; center[1] = pt1[1]; 
		}
		else
		{
			center[0] = pt2[0]; center[1] = pt2[1]; 
		}
		slope = ray_angle(center, mousept);
		point3[0] = center[0] + radius * cos(slope);
		point3[1] = center[1] + radius * sin(slope);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0085", __FILE__, __FUNCSIG__); return false; }
}

//Function to convert radian to decimal degree//
double RapidMath2D::Radian2Degree(double radian_val)
{	
	try
	{	
		double DecimalDeg = radian_val * (180 / M_PI);  //Converts radian to decimal degree//
		return DecimalDeg;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0009", __FILE__, __FUNCSIG__); return 0; }
}

//Function to convert radian to degree,min,seconds//
bool RapidMath2D::Radian2Deg_Min_Sec(double radian, char* Anglev, int NodigitsDeg)
{	
	try
	{
		char DegSym = char(176);  //Symbol for degree//
		char MinSym = char(39);   //Symbol for minute//
		int deci = 0, sign = 0; //return values from str conversion
		int DecCnt = 1;
				
		double DecimalDeg = radian * 180 / M_PI;  
		int IntDeg = (int)DecimalDeg; //integer part of Degree
		//int IntDeg = floor(DecimalDeg); //integer part of Degree
		double DecimalMin = abs((DecimalDeg - IntDeg)) * 60;
		int IntMin = (int)DecimalMin; //integer part of Min
		//int IntMin = floor(DecimalMin); //integer part of Min
		double DecimalSec = abs((DecimalMin - IntMin)) * 60;
		//int IntSec = (int)(DecimalSec + 0.5); // integer part of Sec
		int IntSec = (int)round(DecimalSec);
		//int IntSec = floor(DecimalSec + 0.5); // integer part of Sec

		if(IntSec == 60) // Worst case.!
		{
			IntMin++;
			IntSec = 0;
		}
		if(IntMin == 60)
		{
			IntDeg++;
			IntMin = 0;
		}
		//For loop to count the number of digits in integer part of the degree//
		int Decdeg = IntDeg;
		while(Decdeg != 0)
		{
			Decdeg /= 10;
			DecCnt++;
		}
		if(DecCnt != 1) DecCnt -= 1;
		std::string Deg_Str, Mini_Str, Sec_Str, ConcatedStr;    
		Deg_Str = (const char*)_ecvt(IntDeg, DecCnt, &deci, &sign);  //double to string conversion of degree//
		if(sign == 1 || radian < 0)
			Deg_Str = "-" + Deg_Str;
		if(NodigitsDeg != 0)
		{
			if(NodigitsDeg > DecCnt)
			{
				int NoofCount = NodigitsDeg - DecCnt;
				for(int i = 0; i < NoofCount; i++)
					Deg_Str = "0" + Deg_Str;
			}
		}
		if(IntMin >= 10)
			Mini_Str = (const char*)_ecvt(IntMin, 2, &deci, &sign);   //double to string conversion of min//
		else
		{
			Mini_Str = (const char*)_ecvt(IntMin, 1, &deci, &sign);
			Mini_Str = "0" + Mini_Str;
		}
		if(IntSec >= 10)
			Sec_Str = (const char*)_ecvt(IntSec, 2, &deci, &sign);	//double to string conversion of sec//
		else
		{
			Sec_Str = (const char*)_ecvt(IntSec, 1, &deci, &sign);
			Sec_Str = "0" + Sec_Str;
		}
		ConcatedStr = Deg_Str + DegSym + " " + Mini_Str + MinSym +" "+ Sec_Str + MinSym + MinSym;  //String concatenation//
		strcpy(Anglev, ConcatedStr.c_str());  //Copy the string to Destination//
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0010", __FILE__, __FUNCSIG__); return false; }
}

//Function to convert degree to radian//
double RapidMath2D::Degree2Radian(double Decideg)
{	
	try
	{
		double radian = Decideg * (M_PI / 180);//Converts decimal degree to radian//
		return radian;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0011", __FILE__, __FUNCSIG__); return 0; }
}

//Function to convert degree_min_sec to radian//
double RapidMath2D::Deg_Min_Sec2Radian(char* degree)
{
	try
	{
		double Radian_val = 0;
		char DegSym = char(176);  //Symbol for degree//
		char MinSym = char(39);   //Symbol for minute//
		
		std::string Deg_Min_Sec = (const char*)degree;
		int StrLen = Deg_Min_Sec.length();
		int DecPos = Deg_Min_Sec.find(DegSym);
		std::string Degreevalue =  Deg_Min_Sec.substr(0, DecPos);
		std::string Remainvalue =  Deg_Min_Sec.substr(DecPos + 1, StrLen);

		StrLen = Remainvalue.length();
		DecPos = Remainvalue.find(MinSym);
		std::string Minvalue = Remainvalue.substr(0, DecPos);
		Remainvalue = Remainvalue.substr(DecPos + 1, StrLen);

		StrLen = Remainvalue.length();
		DecPos = Remainvalue.find(MinSym);
		std::string Secvalue = Remainvalue.substr(0, DecPos);
	
		double IntDeg = (double)_atoi64((char*)Degreevalue.c_str());
		double IntMin = (double)_atoi64((char*)Minvalue.c_str());
		double IntSec = (double)_atoi64((char*)Secvalue.c_str());
		
		Radian_val = (IntDeg + (IntMin / 60) + (IntSec / 3600)) * (M_PI / 180);
		return Radian_val;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0012", __FILE__, __FUNCSIG__); return 0; }
}

//concat 1st and 2nd string and save in 3rd
bool RapidMath2D::Concat2stringin3rd(std::string& tempStr1, std::string& tempStr2, char* tempStr3)
{
	try
	{
		std::string tempStr;		//takking temprary string to store result
		tempStr = tempStr1 + tempStr2;
		strcpy(tempStr3, tempStr.c_str());
		return true;
	}
	catch (...)
	{
		SetAndRaiseErrorMessage("RMATH2D0010", __FILE__, __FUNCSIG__);
		return false;
	}
}

//Convert from double to String..
bool RapidMath2D::Double2String(double doub_value, int NoofDec, char* Conv_Str, bool Check_DecimalSeperateor)
{
	try
	{
		std::string DecSeperator = ".";
		//get decimal seperator...........Added By Rahul(1 May 12)...
		if(Check_DecimalSeperateor)
		{
			DWORD value;
			int ret = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, (LPWSTR)&value, sizeof(value) / sizeof(WCHAR) );
			char ch[2];
			ch[0] = (char)value;
			ch[1] = '\0';
			DecSeperator = (const char*)(ch);   
		}

		double Tmp_doub_value = pow(10.0, -(NoofDec + 1)) * 5 + abs(doub_value);
		if(doub_value < 0) Tmp_doub_value *= -1;
		doub_value = Tmp_doub_value;
		int deci, sign; //return values from str conversion
		bool Val_sign = false;
		if(doub_value < 0) Val_sign = true;
		doub_value = abs(doub_value);
		int IntdoubleVal = (int)doub_value; //integer part of double
		//int IntdoubleVal = floor(doub_value); //integer part of double
		double DecdoubleVal = doub_value - IntdoubleVal;

		int DecCnt = 1;
		int Intdb = IntdoubleVal;
		while(Intdb != 0)
		{
			Intdb /= 10;
			DecCnt++;
		}
		if(DecCnt != 1) DecCnt -= 1;

		std::string TempConvStr1 = "0", TempConvStr2 = "", ConcatedStr;    
		TempConvStr1 = (const char*)_ecvt(IntdoubleVal, DecCnt, &deci, &sign);  //double to string conversion 
		TempConvStr2 = (const char*)_ecvt(DecdoubleVal, 20, &deci, &sign); // double to string conversion
		int deci_cnt = abs(deci);
		while(deci_cnt != 0)
		{
			//deci_cnt /= 10; // bad... decrement
			deci_cnt--;
			TempConvStr2 = "0" + TempConvStr2;
		}

		if(TempConvStr2.length() > NoofDec)
			TempConvStr2 = TempConvStr2.substr(0, NoofDec);
		ConcatedStr = TempConvStr1 + DecSeperator + TempConvStr2;
		if(Val_sign)
			ConcatedStr = "-" + ConcatedStr;
		strcpy(Conv_Str, ConcatedStr.c_str());  //Copy the string to Destination//
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0010", __FILE__, __FUNCSIG__); return false; }
}

//Convert string to wstring conversion
std::wstring RapidMath2D::StringToWString(const std::string& Str)
{
	try
	{
		std::wstring temp(Str.length(), L' ');
		std::copy(Str.begin(), Str.end(), temp.begin());
		return temp;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0010", __FILE__, __FUNCSIG__); return L""; }
}

//Convert wstring to string
std::string RapidMath2D::WStringToString(const std::wstring& Str)
{
	try
	{
		std::string temp(Str.length(), ' ');
		std::copy(Str.begin(), Str.end(), temp.begin());
		return temp;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0010", __FILE__, __FUNCSIG__); return ""; }
}

//Function to perform addition of two matrices//
bool RapidMath2D::AddMatrix(double *M1, int *S1, double *M2, int *S2, double *Ans)
{
	try
	{
		//if size of the matrices are equal then performs addittion//
		if(S1[0] == S2[1] && S1[1] == S2[1]) 
		{	
			int j = S1[0] * S1[1]; //size of the matrix..i.e.3x2 then j = 6(total number of elements)//
			for(int i = 0; i < j; i++)
				Ans[i] = M1[i] + M2[i]; //Ans holds the result of addition-- Ans[i] = M1[i]+M2[i]//
		}
		//if the size of the matrices are not equal then return false// 
		else
			return false;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0021", __FILE__, __FUNCSIG__); return false; }
}

//Function to perform subtraction of two matrices//
bool RapidMath2D::SubMatrix(double *M1, int *S1, double *M2, int *S2, double *Ans)
{
	try
	{
		//if size of the matrix equal then performs subtraction//
		if(S1[0] == S2[1] && S1[1] == S2[1]) 
		{	
			int j = S1[0] * S1[1]; //size of the matrix..i.e.3x2 then j = 6(total number of elements)//
			for(int i = 0; i < j; i++)
				Ans[i] = M1[i] - M2[i]; //Ans holds the result of sub-- Ans[i] = M1[i]-M2[i]//
		}
		else
			return false;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0022", __FILE__, __FUNCSIG__); return false; }
}

//Function to multiply the matrix by a number
bool RapidMath2D::Multiplication_number(double *M1, int *S1, int num, double *Ans)
{
	try
	{
		int j = S1[0] * S1[1];  //size of the matrix..i.e.3x2 then j=6, gives total no. of elements//
		//Ans holds the result(array of elements) of multiplication of matrix  by a number//
		for(int i = 0; i < j; i ++)
			Ans[i] = M1[i] * num;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0023", __FILE__, __FUNCSIG__); return false; }
}

//Function to divide the matrix by a number
bool RapidMath2D::Division_number( double *M1, int *S1,int num, double *Ans )
{
	try
	{
		int j = S1[0] * S1[1];  //size of the matrix..i.e.3x2 then j=6, gives total no. of elements//
		//Ans holds the result(array of elements) of division of matrix  by a number//
		for(int i = 0; i < j; i ++)	
			Ans[i] = M1[i] / num;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0024", __FILE__, __FUNCSIG__); return false; }
}

//Function to calculate the multiplication of two matrix
bool RapidMath2D::MultiplyMatrix1(double *M1, int *S1, double *M2, int *S2, double *answer)
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
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0025", __FILE__, __FUNCSIG__); return false; }
}

//function to caluculate the matrix with lower triangle(cholesky factorization)
bool RapidMath2D::Cholesky_matrix(double *M1, int size, double* L1)
{
	double temp = 0,temp1 = 0; //temp to hold sum while adding using a loop//
	bool flag = true;   //for matrix is symmetric//
	int i, j, k;
	try
	{
		for(i = 0; i < size; i ++)
		{
			for(j = 0; j < size; j ++)
			{
				//checking the symmetry of the matrix//
				if(*(M1 + i * (size) + j) != *(M1 + i + j * (size)))  
				{
					flag = false;   //if matrix is not symmetric cholesky factorization is not possible//
					break;
				}
			}
			if(!flag)
				break;
		}
		//If symmetric calculate the cholesky matrix elements//
		if(flag == 1)  
		{
			for(i = 0; i < size; i ++)
			{
				for(j = 0; j < size; j ++)
				{
					if(i == j)
					{
						//taking sum of the squares of each row elements//
						for(k = 0; k < i; k ++)
						{
							temp += pow(*(L1 + i * (size) + k),2);	
						}
						//diagonal elements of the cholesky matrix//
						*(L1 + i + i * (size)) = sqrt(*(M1 + i * (size) + i) - temp);	
					}
					else if(i > j)
					{
						for(k = 0; k < j; k ++) 
						{
							temp1 += (*(L1 + i * (size) + k)*(*(L1 + j * (size) + k))) ;
						}
						//Lower diagonal elements of the cholesky matrix//
						*(L1 + i * (size) + j) = (*(M1 + i * (size) + j) - temp1) / (*(L1 + j + j * (size)));
					}
					temp = 0; temp1 = 0;
				}
			}
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0026", __FILE__, __FUNCSIG__); return false; }
}

//Inverse / Determinant of a matrix..
double RapidMath2D::OperateMatrix4X4(double* matrix, int Size, int Flag, double* Answer)
{	
	//RowEchelon will be the local matrix on which row reduction will be done to get
	//Upper Diagonal matrix
	double* RowEchelon = (double*) malloc(sizeof(double) * Size * Size);
	//Inverse will be the inverse of the matrix, if required; 
	double* Inverse = (double*) malloc(sizeof(double) * Size * Size);
	int i = 0, j = 0; 
	int FlipCount = 0; //Keeps a count of the number of pivot steps that have been done 
	double max; int pivotRow = 0, currentRow; 
	double Determinant, Eliminator;
	//Copy all the elements of Matrix to RowEchelon
	memcpy((void*)RowEchelon, (void*)matrix , sizeof(double) * Size * Size);
	try
	{
		//For computing the inverse, initialise the inverse as the unit matrix
		for (j = 0; j < Size; j++)	
		{	for (i = 0; i < Size; i ++)
			{	if (i == j) *(Inverse + j * Size + i) = 1; else *(Inverse + j * Size + i) = 0;
			}
		}

		//Loop through all the columns
		for (currentRow = 0; currentRow < Size - 1; currentRow ++)
		{	
			max = abs(*(RowEchelon + currentRow * (Size + 1)));
			pivotRow = 0;
			//Find out the largest member of the current column
			for (i = currentRow + 1; i < Size; i ++) 
			{	
				if (abs(*(RowEchelon + i * Size + currentRow)) > max) {max = abs(*(RowEchelon + i * Size + currentRow)); pivotRow = i;}
			}
			//If all the entries below the current row are 0, then we determinant 0 and non-invertible matrix!
			if(max == 0) continue;
			
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
			{	
				Eliminator = *(RowEchelon + i * Size + currentRow) / *(RowEchelon + currentRow * (Size + 1));
				if(Eliminator == 0) continue;
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
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0029", __FILE__, __FUNCSIG__); return false; }
}

//Transpose of a matrix..
bool RapidMath2D::TransposeMatrix(double *InputMatrix, int size, double *TransposedMatrix)
{
	try
	{
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < size; j++)
				TransposedMatrix[j * size + i] = InputMatrix[size * i + j];
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0031", __FILE__, __FUNCSIG__); return false; }
}

//Load Identity Matrix..
bool RapidMath2D::LoadIdentityMatrix(double* matrix, int size)
{
	try
	{
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < size; j++)
			{
				if(i == j)
					matrix[i * size + j] = 1;
				else
					matrix[i * size + j] = 0;
			}
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0032", __FILE__, __FUNCSIG__); return false; }
}

//Function to calculate parallel line to a given line parameters//
void RapidMath2D::Parallelline(double slope, double intercept, double *point, double len, double *endpt1, double *endpt2, double *offset, bool flag, int LinePosition)
{   
	try
	{ 
		double interceptnew, point1[2]; //intersection point of the perpendicular//
		if(flag)  //If offset is given then calcultate new intercept and end points// 
		{
			if(IsEqual(slope, M_PI_2, MeasureDecValue))
			{
				interceptnew = intercept + *offset;
				/*point1[0] = point[0];  
				point1[1] = point[1];*/
			}
			else
			{
				interceptnew = intercept - *offset / cos(slope);   //Intercept of the parallel line//
			//	Point_PerpenIntrsctn_Line(slope, interceptnew, point, &point1[0]);
			}
			Point_PerpenIntrsctn_Line(slope, interceptnew, point, &point1[0]);
			//endpoint with respect to offset //
			EquiDistPointsOnLine(slope, len, &point1[0], endpt1, endpt2, LinePosition);
		}
		else
		{
			*offset = Pt2Line_DistSigned(point[0], point[1], slope, intercept);
			EquiDistPointsOnLine(slope, len, point, endpt1, endpt2, LinePosition);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0046", __FILE__, __FUNCSIG__); }
}

//function to calculate perpendicular line parameter//
void RapidMath2D::Perpedicularlline(double slope, double intercept, double *point, double len, double *endpt1, double *endpt2, int LinePosition)
{
	try
	{
		double slope1 = 0, interceptnew = 0; //slope and Intercept of the pependicular line//
		if(slope == 0)  //slope of perpendicular //
			slope1 = M_PI_2;
		else
			slope1 = atan(-1 / tan(slope));
		//Two end points of the line are 
		EquiDistPointsOnLine(slope1, len, &point[0], endpt1, endpt2, LinePosition);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0048", __FILE__, __FUNCSIG__); }
}

//Equidistance points on a line from given points..
void RapidMath2D::EquiDistPointsOnLine(double slope, double len, double *point, double *endpt1, double *endpt2, int MousePosition)
{
	try
	{
		len = len / 2;
		//Two end points of the line for the given length are// 
		double cosang = len * cos(slope), sinang = len * sin(slope);
		if(MousePosition == 2)
		{
			endpt1[0] = point[0]; endpt1[1] = point[1];
			endpt2[0] = point[0] - 2 * cosang; endpt2[1] = point[1] - 2 * sinang;
		}
		else if(MousePosition == 1)
		{
			endpt1[0] = point[0]; endpt1[1] = point[1];
			endpt2[0] = point[0] + 2 * cosang; endpt2[1] = point[1] + 2 * sinang;
		}
		else
		{
			endpt1[0] = point[0] + cosang; endpt1[1] = point[1] + sinang;
			endpt2[0] = point[0] - cosang; endpt2[1] = point[1] - sinang;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0047", __FILE__, __FUNCSIG__);}
}

//function to draw line angle bisector of two lines//
void RapidMath2D::Angle_bisector(double slope1, double intercept1, double slope2, double intercept2, double *point, double length, double *endpt1, double *endpt2, int LinePosition)
{
	try
	{
		double intersectnpt[2];  //intersection point of lines//
		double slop1, slop2, interc1, interc2, d1, d2, intercept, slope;
		Angle_FirstScndQuad(&slope1); Angle_FirstScndQuad(&slope2);
		slop1 = (slope1 + slope2) / 2;
		if(IsEqual(slope1, slope2, MeasureDecValue))
		{
			intercept = (intercept1 + intercept2)/2;
			slope = slop1;
		}
		else
		{
			Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectnpt[0]);
			if(slop1 == M_PI_2)
			{
				slop2 = 0;
				interc1 = intersectnpt[0];
				interc2 = intersectnpt[1];
			}
			else if(slop1 == 0)
			{
				slop2 = M_PI_2;
				interc1 = intersectnpt[1];
				interc2 = intersectnpt[0];
			}
			else
			{
				slop2 = atan(-1 / tan(slop1));
				Angle_FirstScndQuad(&slop2);
				//Intercept of the bisector line//
				interc1 = intersectnpt[1] - tan(slop1) * intersectnpt[0];   
				interc2 = intersectnpt[1] - tan(slop2) * intersectnpt[0];
			}
			d1 = Pt2Line_Dist(point[0], point[1], slop1, interc1);
			d2 = Pt2Line_Dist(point[0], point[1], slop2, interc2);
			if(d1 < d2)
			{
				intercept = interc1;
				slope = slop1;
			}
			else
			{
				intercept = interc2;
				slope = slop2;
			}
		}
		//calculate the intersection of mouse point with the angle bisector//
		//line will move with respect to mouse point//
		Point_PerpenIntrsctn_Line(slope, intercept, point, &intersectnpt[0]);
		EquiDistPointsOnLine(slope, length, &intersectnpt[0], endpt1, endpt2, LinePosition);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0072", __FILE__, __FUNCSIG__); }
}

//Angle bisector..
void RapidMath2D::Angle_bisector(double slope1, double intercept1, double slope2, double intercept2, double *mousepoint, double *slope, double *intercept)
{
	
	try
	{
		double slop1, slop2, interc1, interc2, d1, d2, intersectnpt[2];
		Angle_FirstScndQuad(&slope1); Angle_FirstScndQuad(&slope2);
		slop1 = (slope1 + slope2) / 2;
		if(IsEqual(slope1, slope2, MeasureDecValue))
		{
			*intercept = (intercept1 + intercept2)/2;
			*slope = slop1;
		}
		else
		{
			Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectnpt[0]);
			if(slop1 == M_PI_2)
			{
				slop2 = 0;
				interc1 = intersectnpt[0];
				interc2 = intersectnpt[1];
			}
			else if(slop1 == 0)
			{
				slop2 = M_PI_2;
				interc1 = intersectnpt[1];
				interc2 = intersectnpt[0];
			}
			else
			{
				slop2 = atan(-1 / tan(slop1));
				Angle_FirstScndQuad(&slop2);
				//Intercept of the bisector line//
				interc1 = intersectnpt[1] - tan(slop1) * intersectnpt[0];   
				interc2 = intersectnpt[1] - tan(slop2) * intersectnpt[0];
			}
			d1 = Pt2Line_Dist(mousepoint[0], mousepoint[1], slop1, interc1);
			d2 = Pt2Line_Dist(mousepoint[0], mousepoint[1], slop2, interc2);
			if(d1 < d2)
			{
				*intercept = interc1;
				*slope = slop1;
			}
			else
			{
				*intercept = interc2;
				*slope = slop2;
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0073", __FILE__, __FUNCSIG__); }
}

//Function to calculte the midpoint of the line//
void RapidMath2D::Mid_point(double *endpt1, double *endpt2, double *midpoint)
{
	try
	{
		//Calculte the midpoint by taking average of coordinates of points////(x1+x2)/2 and (y1+y2)/2
		midpoint[0] = (endpt1[0] + endpt2[0]) / 2;
		midpoint[1] = (endpt1[1] + endpt2[1]) / 2;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0052", __FILE__, __FUNCSIG__); }
}

//function to draw tangent-- tangent on a given circle that is closest to a given point///
void RapidMath2D::Tangent2aCircle(double *center, double radius, double *point, double lengthoftan, double *endpt1, double *endpt2, int LinePosition)
{
	try
	{
		double nearestpt[2], slope1;  //nearest point on the circle  to a given point
		double intercept, slope; //slope of the line joining the center and the nearest point//
		//Function to find the nearest point in the circle for a given point//
		Point_onCircle(center, radius, point, &nearestpt[0]);  
		if(center[0] == nearestpt[0])   
		{
			slope = 0;   
			intercept = nearestpt[1] - (tan(slope)) * nearestpt[0];
		}
		else
		{
			slope1 = (center[1] - nearestpt[1]) / (center[0] - nearestpt[0]);
			if(slope1 == 0)     //line joining the center and the nearest point is horizontal, then the tangent is vertical//
			{
				slope = M_PI_2;
				intercept = nearestpt[0];
			}
			else
			{
				slope = atan(-1 / (slope1));
				intercept = nearestpt[1] - (tan(slope)) * nearestpt[0];
			}
		}
		//Endpoint calculations//
		EquiDistPointsOnLine(slope, lengthoftan, &nearestpt[0], endpt1, endpt2, LinePosition);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0074", __FILE__, __FUNCSIG__); }
}

//function to draw tangent parallel to a given line and nearest to given point//
void RapidMath2D::Tangent_Parallel(double *center, double radius, double slope, double intercept, double length, double *point, double *endpt1, double *endpt2, int LinePosition)
{
	try
	{
		double point1[2], point2[2], midpoint[2], dist1, dist2, slope1;
		if(slope == 0)  //if the slope of the line zero(horizontal) then the slope of the perpedicular is PI/2 //
			slope1 = M_PI_2;
		else
			slope1 = atan(-1 / tan(slope));
		//Two tangential points //
		EquiDistPointsOnLine(slope1, radius * 2, center, &point1[0], &point2[0]);
		dist1 = Pt2Pt_distance(&point1[0], point);
		dist2 = Pt2Pt_distance(&point2[0], point);
		
		if(dist1 < dist2)  //point1 is the nearest point to the given point//
		{
			midpoint[0] = point1[0]; midpoint[1] = point1[1];
		}
		else //Point2 is the nearest point to the given point//
		{
			midpoint[0] = point2[0]; midpoint[1] = point2[1];
		}
		//Endpoint calculations//
		EquiDistPointsOnLine(slope, length, &midpoint[0], endpt1, endpt2, LinePosition);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0075", __FILE__, __FUNCSIG__); }
}

//function to draw tangent perpendicular to a given line and nearest to given point//
void RapidMath2D::Tangent_Perpendicular(double *center, double radius, double slope, double intercept, double length, double *point, double *endpt1, double *endpt2, int LinePosition)
{	
	try
	{
		double point1[2], point2[2], midpoint[2], dist1, dist2, slopetan;
		if(slope == 0)  //slope of perpendicular //
			slopetan = M_PI_2;
		else
			slopetan = atan(-1 / tan(slope));
		//Two tangential points //
		EquiDistPointsOnLine(slope, radius * 2, center, &point1[0], &point2[0]);
		dist1 = Pt2Pt_distance(&point1[0], point);
		dist2 = Pt2Pt_distance(&point2[0], point);

		if(dist1 < dist2)  //finding the nearest point(1st point) is nearest to the given point//
		{
			midpoint[0] = point1[0]; midpoint[1] = point1[1];
		}
		else  //2nd point is nearest to the given point//
		{
			midpoint[0] = point2[0]; midpoint[1] = point2[1];
		}
		//Endpoint calculations//
		EquiDistPointsOnLine(slopetan, length, &midpoint[0], endpt1, endpt2, LinePosition);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0076", __FILE__, __FUNCSIG__); }
}

//tangent through a given a point//
void RapidMath2D::Tangent_ThruPoint(double *center, double radius, double *tanpoint, double *mousept, double length, double *endpt1, double *endpt2, int LinePosition)
{
	try
	{
		double point1[2],point2[2], midpoint[2], dis, dis1, dist1, dist2, slope1, slope2, slope; 
	
		dis = Pt2Pt_distance(tanpoint, center);
		if(dis < radius) //Point is inside the circle -- no tangent//
			return;
		dis1 = sqrt(pow(dis, 2) - pow(radius, 2));
		//angle made by the line joining the point and center and perpendicular to tangent//
		slope1 = asin(radius / dis);  
		//slope of the line joining the point and the center//
		slope2 = ray_angle(tanpoint, center);
		//point on circles..
		point1[0] = tanpoint[0] + dis1 * cos(slope1 + slope2);      
		point1[1] = tanpoint[1] + dis1 * sin(slope1 + slope2);
		point2[0] = tanpoint[0] + dis1 * cos(slope2 - slope1);      
		point2[1] = tanpoint[1] + dis1 * sin(slope2 - slope1);

		dist1 = Pt2Pt_distance(mousept, &point1[0]);
		dist2 = Pt2Pt_distance(mousept, &point2[0]);

		if(dist1 < dist2)  //point1 is the nearest point to the given point//
		{
			midpoint[0] = point1[0]; midpoint[1] = point1[1];
			slope = slope1 + slope2;
		}
		else //Point2 is the nearest point to the given point//
		{
			midpoint[0] = point2[0]; midpoint[1] = point2[1];
			slope = slope2 - slope1;
		}
		//Endpoint calculations//
		EquiDistPointsOnLine(slope, length, &midpoint[0], endpt1, endpt2, LinePosition);
		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0077", __FILE__, __FUNCSIG__); }
}

//Tangent to two circles -- the tangent that is simultaneously parallel to two given circlesle 
void RapidMath2D::Tangent_TwoCircles(double *center1, double radius1, double *center2, double radius2, double length,double *refpoint,double *endpt1,double *endpt2)
{
	
	try
	{
		int i,j=0;
		double slope1,slope,slope2, angle,angle1, dis1 = 0;  //slope1 is the slope of the line joining the centers//
		double dis[4],dist[4],intpoint1[2],intpoint2[2],intercepts[4];//,intercept1,intercept2,intercept3,intercept4;  
		double slopes[4],point1[4],point2[4];		
		//If the radii of both the circles are equal then two tangents are parallel to the line joining the centers //
		if(radius1 == radius2)   
		{
			slope1 = ray_angle(center1, center2);
			//Intersection point of tangents//
			for(i = 0; i < 2; i ++)
			{
				intpoint1[i] = ((radius2) * (*(center1 + i)) + (radius1)*(*(center2 + i))) / (radius1 + radius2);
			}
			for(i = 0; i < 2; i ++)
			{
				dis1 = dis1 + pow((intpoint1[i]-*(center1 + i)),2);
			}
			//distance between the center and the intersection point of tangents//
			dis[0] = sqrt(dis1);
			//angle of tangent with line joining the centers//
			angle = asin(radius1/dis[0]);
			//slope of tangents//
			slopes[0] = slope1 + angle;   
			slopes[1] = slope1 - angle;
			slopes[2] = slope1;
			slopes[3] = slope1;
			slope2  = slope1 + M_PI_2;
			point1[0] = *center1 + radius1 * (cos(slope2));   //end points for tangent1//
			point1[1] = *(center1 + 1) + radius1 * (sin(slope2));
			point1[2] = *center1 - radius1 * (cos(slope2));
			point1[3] = *(center1 + 1) - radius1 * (sin(slope2));
			//intercepts of the tangents;
			
			Intercept_LinePassing_Point(&intpoint1[0], slopes[0], &intercepts[0]);
			Intercept_LinePassing_Point(&intpoint1[0], slopes[1], &intercepts[1]);
			Intercept_LinePassing_Point(&point1[0], slope2, &intercepts[2]);
			Intercept_LinePassing_Point(&point1[2], slope2, &intercepts[3]);
				
		}
		else
		{
			slope1 = ray_angle(center1, center2);
			//Intersection point of tangents//
			for(i = 0; i < 2; i ++)
			{
				intpoint1[i] = ((radius2) * (*(center1 + i)) + (radius1) * (*(center2 + i))) / (radius1 + radius2);
			}
			//Intersection point of tangents//
			for(i = 0; i < 2; i ++)
			{
				intpoint2[i] = ((radius2) * (*(center1 + i)) - (radius1)*(*(center2 + i)))/(radius2 -  radius1);
			}
			for(i = 0; i < 2; i ++)
			{
				dis1 = dis1 + pow((intpoint1[i] -*(center1 + i)),2);
			}
			//distance between the center1 and the intersection point1 of tangents//
			dis[0] = sqrt(dis1);
			dis1 = 0;
			for(i = 0; i < 2; i ++)
			{
				dis1 = dis1 + pow((intpoint2[i] - *(center1 + i)),2);
			}
			//distance between the center2 and the intersection point1 of tangents//
			dis[1] = sqrt(dis1);
			dis1 = 0;
			
			//angle of tangents with line joining the centers//
			angle = asin(radius1 / dis[0]);
			angle1 = asin(radius1 / dis[1]);
			//slope of tangents//
			slopes[0] = slope1 + angle;  
			slopes[1] = slope1 - angle;
			slopes[2] = slope1 + angle1;
			slopes[3] = slope1 - angle1;
			Intercept_LinePassing_Point(&intpoint1[0], slopes[0], &intercepts[0]);
			Intercept_LinePassing_Point(&intpoint1[0], slopes[1], &intercepts[1]);
			Intercept_LinePassing_Point(&intpoint2[0], slopes[2], &intercepts[2]);
			Intercept_LinePassing_Point(&intpoint2[0], slopes[3], &intercepts[3]);

		}
		Point_PerpenIntrsctn_Line(slopes[2], intercepts[2], &intpoint1[0], &point2[0]);
		Point_PerpenIntrsctn_Line(slopes[3], intercepts[3], &intpoint1[0], &point2[2]);



		for(i = 0; i < 4; i ++)
		{
			if(slopes[i] == M_PI_2)
			{
				//dstance between the point and line//
				dist[i] = abs(*refpoint - intercepts[i]);  
			}
			else
			{
				dist[i] = abs((tan(slopes[i]) * (*refpoint) + intercepts[i] - *(refpoint + 1)) * cos(slopes[i]));
			}
		}
		for(i = 0; i < 4; i++)
		{
			if(dist[0] > dist[i])
				{
					dist[0] = dist[i];
					j = i;
				}
		}

		//slope of tangent//
		slope = slopes[j];		
		if(j < 2)
		{
			*endpt1 =  intpoint1[0]  + length *cos(slope) / 2;  
			*(endpt1 + 1) = intpoint1[1]  + length *sin(slope) / 2;;
			*endpt2 = intpoint1[0] - length *cos(slope) / 2; 
			*(endpt2 + 1) = intpoint1[1] - length *sin(slope) / 2; 
		}
		else if(j == 2)
		{
			//Endpoints of tangent//
			*endpt1 =  point2[0]  + length *cos(slope) / 2;  
			*(endpt1 + 1) = point2[1]  + length *sin(slope) / 2;;
			*endpt2 = point2[0] - length *cos(slope) / 2; 
			*(endpt2 + 1) = point2[1] - length *sin(slope) / 2; 
		}
		else
		{
			//Endpoints of tangent//
			*endpt1 =  point2[2]  + length *cos(slope) / 2;  
			*(endpt1 + 1) = point2[3]  + length *sin(slope) / 2;
			*endpt2 = point2[2] - length *cos(slope) / 2; 
			*(endpt2 + 1) = point2[3] - length *sin(slope) / 2; 

		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0079", __FILE__, __FUNCSIG__); }
}

//circle that is tangential to two given lines//
void RapidMath2D::Circle_Tangent2TwoLines(double slope1, double intercept1, double slope2, double intercept2, double *radius, double *point, double *center)
{ 
	try
	{
		double angle_bs1, angle_bs2, intersectnpt[2], center1[8];
		double Tempradius = *radius;
		Angle_FirstScndQuad(&slope1); Angle_FirstScndQuad(&slope2);
		if(IsEqual(slope1, slope2, MeasureDecValue))  //If lines are parallel
		{
			double dist = Line2Line_dis(slope1, intercept1, slope2, intercept2);
			*radius = dist/2;
			Point_PerpenIntrsctn_Line(slope1, intercept1, point, &intersectnpt[0]);
			Point_PerpenIntrsctn_Line(slope2, intercept2, point, &center1[0]);
			center[0] = (intersectnpt[0] + center1[0])/2;
			center[1] = (intersectnpt[1] + center1[1])/2;
		}
		//slope of the angle bisector//
		angle_bs1 = (slope1 + slope2) / 2; 
		if(angle_bs1 == 0)
			angle_bs2 = M_PI_2; 
		else if(angle_bs1 == M_PI_2)
			angle_bs2 = 0;
		else
			angle_bs2 = atan(-1 / tan(angle_bs1));
		Angle_FirstScndQuad(&angle_bs1); Angle_FirstScndQuad(&angle_bs2);
		//distance of the centers from the intersection point//
		double dist1 = abs(Tempradius / sin(angle_bs1 - slope1));
		double dist2 = abs(Tempradius / sin(slope1 -angle_bs2));

		Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectnpt[0]);
		double cosangle1 = dist1 * cos(angle_bs1), sinangle1 = dist1 * sin(angle_bs1);
		double cosangle2 = dist2 * cos(angle_bs2), sinangle2 = dist2 * sin(angle_bs2);
		//center of the circles on 1st angle bisector//
		center1[0] = intersectnpt[0] + cosangle1; center1[1] = intersectnpt[1] + sinangle1;
		center1[2] = intersectnpt[0] - cosangle1; center1[3] = intersectnpt[1] - sinangle1;
		
		//center of the circles on 2nd angle bisector//
		center1[4] = intersectnpt[0] + cosangle2; center1[5] = intersectnpt[1] + sinangle2;
		center1[6] = intersectnpt[0] - cosangle2; center1[7] = intersectnpt[1] - sinangle2;
		//Find the Nearest center point to the given point//
		Nearestpoint_point(4, center1, point, center);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0078", __FILE__, __FUNCSIG__); }
}

//Circle Inside a triangle..
void RapidMath2D::Circle_InsideATriangle(double slope1, double intercept1, double slope2, double intercept2, double slope3, double intercept3, double *circleParam)
{
	try
	{
		double intersectnpt1[2], intersectnpt2[2], intersectnpt3[2];
		Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectnpt1[0]);
		Line_lineintersection(slope2, intercept2, slope3, intercept3, &intersectnpt2[0]);
		Line_lineintersection(slope3, intercept3, slope1, intercept1, &intersectnpt3[0]);

		double midpt1[2] = {(intersectnpt1[0] + intersectnpt2[0])/2, (intersectnpt1[1] + intersectnpt2[1])/2};
		double midpt2[2] = {(intersectnpt2[0] + intersectnpt3[0])/2, (intersectnpt2[1] + intersectnpt3[1])/2};
		double midpt3[2] = {(intersectnpt3[0] + intersectnpt1[0])/2, (intersectnpt3[1] + intersectnpt1[1])/2};

		double angle_bs1_Angle, angle_bs1_Intercept, angle_bs2_Angle, angle_bs2_Intercept, angle_bs3_Angle, angle_bs3_Intercept;
		Angle_bisector(slope1, intercept1, slope2, intercept2, &midpt2[0], &angle_bs1_Angle, &angle_bs1_Intercept);
		Angle_bisector(slope2, intercept2, slope3, intercept3, &midpt3[0], &angle_bs2_Angle, &angle_bs2_Intercept);
		Angle_bisector(slope3, intercept3, slope1, intercept1, &midpt1[0], &angle_bs3_Angle, &angle_bs3_Intercept);
		
		//Center and radius calculation.
		Line_lineintersection(angle_bs1_Angle, angle_bs1_Intercept, angle_bs2_Angle, angle_bs2_Intercept, circleParam);
		circleParam[2] = Pt2Line_Dist(circleParam[0], circleParam[1],  slope1, intercept1);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0077", __FILE__, __FUNCSIG__); }
}

//Circle outside a triangle..
void RapidMath2D::Circle_Circum2Triangle(double slope1, double intercept1, double slope2, double intercept2, double slope3, double intercept3, double *circleParam)
{
	try
	{
		double intersectnpt1[2], intersectnpt2[2], intersectnpt3[2];
		Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectnpt1[0]);
		Line_lineintersection(slope2, intercept2, slope3, intercept3, &intersectnpt2[0]);
		Line_lineintersection(slope3, intercept3, slope1, intercept1, &intersectnpt3[0]);
		Circle_3Pt(&intersectnpt1[0], &intersectnpt2[0], &intersectnpt3[0],circleParam, circleParam + 2);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0077", __FILE__, __FUNCSIG__); }
}

//function to calculate the circle tangential to circle through the mousepoint//
void RapidMath2D::Circle_Tangent2Circle(double *center, double radius, double *mousept, double *cen, double *radius1)
{
	try
	{
		double PtOnc[2];
		Point_onCircle(center, radius, mousept, &PtOnc[0]);
		Mid_point(&PtOnc[0], mousept, cen);
		*radius1 = Pt2Pt_distance(cen, mousept);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0080", __FILE__, __FUNCSIG__); }
}

//function to calculate the circle tangential to the given circle with given radius//
void RapidMath2D::Circle_Tangent2CircleFixedDia(double *center, double radius, double radius1, double *mousept, double *center1)
{
	try
	{
		//angle of the line joining the center and mouse point//
		double slope = ray_angle(center, mousept);
		center1[0] = center[0] + (radius + radius1) * cos(slope);
		center1[1] = center[1] + (radius + radius1) * sin(slope);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0081", __FILE__, __FUNCSIG__); }
}

//Tangential Circle 2 Two circles..
void RapidMath2D::Circle_Tangent2Circles(double *center1, double radius1, double *center2, double radius2, double *mousept, double *radius_circle, double *center)
{
	try
	{
		double intersectionpt[4] = {0}, rad = *radius_circle, Dist = 0;
		int Cnt = 0;
		if(Circle_circleintersection(center1, center2, radius1, radius2, &intersectionpt[0]) > 1)
		{
			bool InsideCircle = false;
			int CirCount1[2] = {0}, CirCount2[2] = {0};
			double _itersectPt1[4] = {0}, _itersectPt2[4] = {0};
			double Point1[2] = {0}, Point2[2] = {0};
			double centerDist = Pt2Pt_distance(center1, center2);
			if((centerDist < radius1 && centerDist < radius2) || centerDist < radius1 || centerDist < radius2)
			{
				if(radius1 + radius2 - centerDist > 2 * rad)
					InsideCircle = true;
			}
			else
			{
				if(Finiteline_circle(center1, center2, center1, radius1, _itersectPt1, CirCount1) > 1)
				{
					bool Flag1 = false;
					if(CirCount1[0] == 1)
					{
						Flag1 = true;
						Point1[0] = _itersectPt1[0]; Point1[1] = _itersectPt1[1];
					}
					else if(CirCount1[1] == 1)
					{
						Flag1 = true;
						Point1[0] = _itersectPt1[2]; Point1[1] = _itersectPt1[3];
					}
					if(Finiteline_circle(center1, center2, center2, radius2, _itersectPt2, CirCount2) > 1)
					{
						bool Flag2 = false;
						if(CirCount2[0] == 1)
						{
							Flag2 = true;
							Point2[0] = _itersectPt2[0]; Point2[1] = _itersectPt2[1];
						}
						else if(CirCount2[1] == 1)
						{
							Flag2 = true;
							Point2[0] = _itersectPt2[2]; Point2[1] = _itersectPt2[3];
						}
						if(Flag1 && Flag2)
						{
							if(Pt2Pt_distance(Point1, Point2) > 2 * rad)
								InsideCircle = true;
						}
					}
				}
			}
			if(InsideCircle)
			{
				if(Pt2Pt_distance(mousept, center1) < radius1 || Pt2Pt_distance(mousept, center2) < radius2)
					Cnt = Circle_circleintersection(center1, center2, radius1 - rad, radius2 - rad, &intersectionpt[0]);
				else
					Cnt = Circle_circleintersection(center1, center2, radius1 + rad, radius2 + rad, &intersectionpt[0]);
			}
			else
				Cnt = Circle_circleintersection(center1, center2, radius1 + rad, radius2 + rad, &intersectionpt[0]);
		}
		else
			Cnt = Circle_circleintersection(center1, center2, radius1 + rad, radius2 + rad, &intersectionpt[0]);
		if(Cnt == 0) //There is no tangential Circle for the given radius...
		{
			Dist = Pt2Pt_distance(center1, center2);
			rad = (Dist - (radius1 + radius2)) / 2;
			*radius_circle = rad;
			Cnt = Circle_circleintersection(center1, center2, radius1 + rad, radius2 + rad, &intersectionpt[0]);
		}
		if(Cnt == 1)
		{
			center[0] = intersectionpt[0]; 
			center[1] = intersectionpt[1];
		}
		else
		{
			if(Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[0], intersectionpt[1]) < Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[2], intersectionpt[3]))
			{
				center[0] = intersectionpt[0];
				center[1] = intersectionpt[1];
			}
			else
			{
				center[0] = intersectionpt[2];
				center[1] = intersectionpt[3];
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0081c", __FILE__, __FUNCSIG__); }
}

//Tangential Circle 2 Two circles..
void RapidMath2D::Circle_Tangent2Circles_Inside(double *center1, double radius1, double *center2, double radius2, double *mousept, double radius_circle, double *center)
{
	try
	{
		double intersectionpt[4], rad = radius_circle;
		double Dist = Pt2Pt_distance(center1, center2);
		if(Dist < 0.0001)
		{
			if(radius1 < radius2)
				Circle_Tangent2CircleFixedDia(center1, radius1, rad, mousept, center);
			else
				Circle_Tangent2CircleFixedDia(center2, radius2, rad, mousept, center);
		}
		else
		{
			int Cnt = 0;
			if(radius1 > radius2)
				Cnt = Circle_circleintersection(center1, center2, radius1 - rad, radius2 + rad, &intersectionpt[0]);
			else
				Cnt = Circle_circleintersection(center1, center2, radius1 + rad, radius2 - rad, &intersectionpt[0]);
			if(Cnt == 0) //There is no tangential Circle for the given radius...
				return;
			if(Cnt == 1)
			{
				center[0] = intersectionpt[0]; 
				center[1] = intersectionpt[1];
			}
			else
			{
				if(Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[0], intersectionpt[1]) < Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[2], intersectionpt[3]))
				{
					center[0] = intersectionpt[0];
					center[1] = intersectionpt[1];
				}
				else
				{
					center[0] = intersectionpt[2];
					center[1] = intersectionpt[3];
				}
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0081g", __FILE__, __FUNCSIG__); }
}

//Tangential Circle 2 Two circles..
void RapidMath2D::Circle_Tangent2Circles_Outside(double *center1, double radius1, double *center2, double radius2, double *mousept, double *radius_circle, double *center)
{
	try
	{
		double intersectionpt[4], rad = *radius_circle, Dist = 0;
		int Cnt = 0;
		Cnt = Circle_circleintersection(center1, center2, rad - radius1, rad - radius2, &intersectionpt[0]);
		if(Cnt == 0) //There is no tangential Circle for the given radius...
		{
			Dist = Pt2Pt_distance(center1, center2);
			rad = (Dist + radius1 + radius2)/2;
			*radius_circle = rad;
			Cnt = Circle_circleintersection(center1, center2, radius1 + rad, radius2 + rad, &intersectionpt[0]);
		}
		if(Cnt == 1)
		{
			center[0] = intersectionpt[0]; 
			center[1] = intersectionpt[1];
		}
		else
		{
			if(Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[0], intersectionpt[1]) < Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[2], intersectionpt[3]))
			{
				center[0] = intersectionpt[0];
				center[1] = intersectionpt[1];
			}
			else
			{
				center[0] = intersectionpt[2];
				center[1] = intersectionpt[3];
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0081c", __FILE__, __FUNCSIG__); }
}

//Tangential Circle 2 Line and Circle
void RapidMath2D::Circle_Tangent2LineCircle(double *center, double radius, double slope, double intercept, double *mousept, double radius_circle, double *rcenter)
{
	try
	{
		double intercept1, intersectionpt[4], pt[2], tempangle;
		Point_PerpenIntrsctn_Line(slope, intercept, center, &pt[0]);
		tempangle = ray_angle(pt[0], pt[1], center[0], center[1]);
		if(tempangle == 0 || tempangle == M_PI)
			intercept1 = intercept - radius_circle;
		else
			intercept1 = intercept - (radius_circle)/cos(tempangle + M_PI_2);
		int Cnt = Line_circleintersection(slope, intercept1, center, radius + radius_circle, &intersectionpt[0]);
		if(Cnt == 0) return; // there is no tangential circle for the given radius..
		else
		{
			if(Cnt == 1)
			{
				rcenter[0] = intersectionpt[0]; 
				rcenter[1] = intersectionpt[1];
			}
			else
			{
				if(Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[0], intersectionpt[1]) < Pt2Pt_distance(mousept[0], mousept[1], intersectionpt[2], intersectionpt[3]))
				{
					rcenter[0] = intersectionpt[0];
					rcenter[1] = intersectionpt[1];
				}
				else
				{
					rcenter[0] = intersectionpt[2];
					rcenter[1] = intersectionpt[3];
				}
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0083a", __FILE__, __FUNCSIG__); }
}

//Fillet arc for two lines..
bool RapidMath2D::FilletForTwoLines(double slope1, double intercept1, double *endpt11, double *endpt12, double slope2, double intercept2, double *endpt21, double *endpt22, double radius, double *center)
{
	try
	{
		double slope3, slope4, slope5, intersectnpt[2], dist;
		double TempPt[2], Clinter1[2], Clinter2[2];
		Angle_FirstScndQuad(&slope1); Angle_FirstScndQuad(&slope2);
		if(IsEqual(slope1, slope2, MeasureDecValue)) 
		{
			return false;
		}
		Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectnpt[0]);
		if(Pt2Pt_distance(intersectnpt[0], intersectnpt[1], endpt11[0], endpt11[1]) > Pt2Pt_distance(intersectnpt[0], intersectnpt[1], endpt12[0], endpt12[1]))
		{
			TempPt[0] = endpt11[0]; TempPt[1] = endpt11[1];
			endpt11[0] = endpt12[0]; endpt11[1] = endpt12[1];
			endpt12[0] = TempPt[0]; endpt12[1] = TempPt[1];
		}
		if(Pt2Pt_distance(intersectnpt[0], intersectnpt[1], endpt21[0], endpt21[1]) > Pt2Pt_distance(intersectnpt[0], intersectnpt[1], endpt22[0], endpt22[1]))
		{
			TempPt[0] = endpt21[0]; TempPt[1] = endpt21[1];
			endpt21[0] = endpt22[0]; endpt21[1] = endpt22[1];
			endpt22[0] = TempPt[0]; endpt22[1] = TempPt[1];
		}
		slope3 = ray_angle(intersectnpt[0], intersectnpt[1], endpt12[0], endpt12[1]);
		slope4 = ray_angle(intersectnpt[0], intersectnpt[1], endpt22[0], endpt22[1]);
		if(abs(slope3 - slope4) > M_PI)
		{
			if(slope3 < slope4) slope3 += 2 * M_PI;
			else slope4 += 2 * M_PI;
		}
		slope5 = (slope3 + slope4) / 2;
		dist = abs(radius/sin(slope3 - slope5));
		if(slope5 > 2 * M_PI) slope5 -= 2 * M_PI;
		double ccenter[2], tempslope1, tempslope2;
		ccenter[0] = intersectnpt[0] + dist * cos(slope5);
		ccenter[1] = intersectnpt[1] + dist * sin(slope5);
		Line_circleintersection(slope1, intercept1, &ccenter[0], radius, &Clinter1[0]);
		Line_circleintersection(slope2, intercept2, &ccenter[0], radius, &Clinter2[0]);
		if((slope3 > 2 * M_PI) || IsEqual(slope3, 2 * M_PI, MeasureDecValue)) slope3 -= 2 * M_PI;
		if((slope4 > 2 * M_PI) || IsEqual(slope4, 2 * M_PI, MeasureDecValue)) slope4 -= 2 * M_PI;
		tempslope1 = ray_angle(Clinter1[0], Clinter1[1], endpt12[0], endpt12[1]);
		tempslope2 = ray_angle(Clinter2[0], Clinter2[1], endpt22[0], endpt22[1]);
		if(IsEqual(slope3, tempslope1, MeasureDecValue) && IsEqual(slope4, tempslope2, MeasureDecValue))
		{
			endpt11[0] = Clinter1[0]; endpt11[1] = Clinter1[1];
			endpt21[0] = Clinter2[0]; endpt21[1] = Clinter2[1];
			double tempangle = ray_angle(ccenter[0], ccenter[1], intersectnpt[0], intersectnpt[1]);
			center[0] = Clinter1[0]; center[1] = Clinter1[1];
			center[2] = ccenter[0] + radius * cos(tempangle); center[3] = ccenter[1] + radius * sin(tempangle);
			center[4] = Clinter2[0]; center[5] = Clinter2[1];
			return true;
		}
		else
			return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0083j", __FILE__, __FUNCSIG__); return false; }
}

//Fillet arc for Two arc..
//To be implemented... There will be cases when non intersection circle inside another circle..
//Look into tangential circle to two circles inside and it can be done easily!
bool RapidMath2D::FilletForTwoArcs(double *center1, double radius1, double startang1, double sweepang1, double *center2, double radius2, double startang2, double sweepang2, double radius, double *Arc1Points, double *Arc2Points, double *FilletArc)
{
	try
	{
		double intersectionpt[4];
		int Cnt = Circle_circleintersection(center1, center2, radius1 + radius, radius2 + radius, &intersectionpt[0]);
		if(Cnt == 0) //There is no Fillet.. Intersection is not there..
		{
			return false;
		}
		else
		{
			if(Cnt == 1) //Semi Circle Fillet..Take one from two possibilities
			{
				InternalFilletArctoArc(center1, radius1, startang1, sweepang1, center2, radius2, startang2, sweepang2, &intersectionpt[0], radius, Arc1Points, Arc2Points, FilletArc);
				return true;
			}
			else //Take one from four possibilties..
			{
				double Arc1Points1[6], Arc2Points1[6], FilletArc1[6], Arc1Points2[6], Arc2Points2[6], FilletArc2[6];
				InternalFilletArctoArc(center1, radius1, startang1, sweepang1, center2, radius2, startang2, sweepang2, &intersectionpt[0], radius, &Arc1Points1[0], &Arc2Points1[0], &FilletArc1[0]);
				InternalFilletArctoArc(center1, radius1, startang1, sweepang1, center2, radius2, startang2, sweepang2, &intersectionpt[2], radius, &Arc1Points2[0], &Arc2Points2[0], &FilletArc2[0]);
				double Arc1Cen1[2], Arc1Rad1, Arc1StAng1, Arc1SwpAng1, Arc2Cen1[2], Arc2Rad1, Arc2StAng1, Arc2SwpAng1;
				double Arc1Cen2[2], Arc1Rad2, Arc1StAng2, Arc1SwpAng2, Arc2Cen2[2], Arc2Rad2, Arc2StAng2, Arc2SwpAng2;
				Arc_3Pt(&Arc1Points1[0], &Arc1Points1[2], &Arc1Points1[4], &Arc1Cen1[0], &Arc1Rad1, &Arc1StAng1, &Arc1SwpAng1);
				Arc_3Pt(&Arc2Points1[0], &Arc2Points1[2], &Arc2Points1[4], &Arc2Cen1[0], &Arc2Rad1, &Arc2StAng1, &Arc2SwpAng1);
				Arc_3Pt(&Arc1Points2[0], &Arc1Points2[2], &Arc1Points2[4], &Arc1Cen2[0], &Arc1Rad2, &Arc1StAng2, &Arc1SwpAng2);
				Arc_3Pt(&Arc2Points2[0], &Arc2Points2[2], &Arc2Points2[4], &Arc2Cen2[0], &Arc2Rad2, &Arc2StAng2, &Arc2SwpAng2);
				if((Arc1SwpAng1 + Arc2SwpAng1) > (Arc1SwpAng2 + Arc2SwpAng2))
				{
					for(int i = 0; i < 6; i++)
					{
						Arc1Points[i] = Arc1Points1[i];
						Arc2Points[i] = Arc2Points1[i];
						FilletArc[i] = FilletArc1[i];
					}
				}
				else
				{
					for(int i = 0; i < 6; i++)
					{
						Arc1Points[i] = Arc1Points2[i];
						Arc2Points[i] = Arc2Points2[i];
						FilletArc[i] = FilletArc2[i];
					}
				}
				return true;
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0083g", __FILE__, __FUNCSIG__); return false; }
}
//
bool RapidMath2D::InternalFilletArctoArc(double *center1, double radius1, double startang1, double sweepang1, double *center2, double radius2, double startang2, double sweepang2, double *tancenter, double tanradius, double *Arc1Points, double *Arc2Points, double *FilletArc)
{
	try
	{
		double C2CInterSctn1[2], C2CInterSctn2[2];
		double endangle1 = startang1 + sweepang1, endangle2 = startang2 + sweepang2;
		Angle_FourQuad(&endangle1); Angle_FourQuad(&endangle2);
		Circle_circleintersection(center1, tancenter, radius1, tanradius, &C2CInterSctn1[0]);
		Circle_circleintersection(center2, tancenter, radius2, tanradius, &C2CInterSctn2[0]);
		double intersctnAngle1 = ray_angle(tancenter[0], tancenter[1], C2CInterSctn1[0], C2CInterSctn1[1]);
		double intersctnAngle2 = ray_angle(tancenter[0], tancenter[1], C2CInterSctn2[0], C2CInterSctn2[1]);
		double pintercept_c1, pintercept_c2;
		Intercept_LinePassing_Point(tancenter, intersctnAngle1, &pintercept_c1);
		Intercept_LinePassing_Point(tancenter, intersctnAngle2, &pintercept_c2);
		
		double MidAngleDiff1 = intersctnAngle1 - intersctnAngle2, MidAngleDiff2 = intersctnAngle2 - intersctnAngle1;
		Angle_FourQuad(&MidAngleDiff1); Angle_FourQuad(&MidAngleDiff2);
		double MidAngle1 = intersctnAngle2 + MidAngleDiff1/2, MidAngle2 = intersctnAngle1 + MidAngleDiff2/2;
		Angle_FourQuad(&MidAngle1); Angle_FourQuad(&MidAngle2);
		double ArcMidpt1[2] = {tancenter[0] + tanradius * cos(MidAngle1), tancenter[1] + tanradius * sin(MidAngle1)};
		double ArcMidpt2[2] = {tancenter[0] + tanradius * cos(MidAngle2), tancenter[1] + tanradius * sin(MidAngle2)};
		
		FilletArc[0] = C2CInterSctn1[0]; FilletArc[1] = C2CInterSctn1[1]; //End points of fillet arc.
		FilletArc[4] = C2CInterSctn2[0]; FilletArc[5] = C2CInterSctn2[1];

		double Arc1_StartAngle, Arc1_EndAngle, Arc1_MidAngle, Arc2_StartAngle, Arc2_EndAngle, Arc2_MidAngle;
		double Arc1_StartAngTemp1, Arc1_MidAngTemp1, Arc1_EndAngTemp1, Arc1_SweepAngTemp1, Arc1_StartAngTemp2, Arc1_MidAngTemp2, Arc1_EndAngTemp2, Arc1_SweepAngTemp2;
		double Arc2_StartAngTemp1, Arc2_MidAngTemp1, Arc2_EndAngTemp1, Arc2_SweepAngTemp1, Arc2_StartAngTemp2, Arc2_MidAngTemp2, Arc2_EndAngTemp2, Arc2_SweepAngTemp2;

		double circle1angle = ray_angle(center1[0], center1[1], C2CInterSctn1[0], C2CInterSctn1[1]); // angle of the intersection point.
		double circle2angle = ray_angle(center2[0], center2[1], C2CInterSctn2[0], C2CInterSctn2[1]); // angle of the intersection point.
		
		Arc1_StartAngTemp1 = circle1angle;
		Arc1_EndAngTemp1 = endangle1;
		if(Arc1_EndAngTemp1 < Arc1_StartAngTemp1) Arc1_EndAngTemp1 += 2 * M_PI;
		Arc1_SweepAngTemp1 = Arc1_EndAngTemp1 - Arc1_StartAngTemp1;
		Arc1_MidAngTemp1 = (Arc1_StartAngTemp1 + Arc1_EndAngTemp1)/2;
		Angle_FourQuad(&Arc1_MidAngTemp1);
		double Arc1_MidptTmp1[2] = {center1[0] + radius1 * cos(Arc1_MidAngTemp1), center1[1] + radius1 * sin(Arc1_MidAngTemp1)};
		double Arc1_MidAngTemp_Tan1 = ray_angle(tancenter[0], tancenter[1], Arc1_MidptTmp1[0], Arc1_MidptTmp1[1]);
		
		Arc1_StartAngTemp2 = startang1;
		Arc1_EndAngTemp2 = circle1angle;
		if(Arc1_EndAngTemp2 < Arc1_StartAngTemp2) Arc1_EndAngTemp2 += 2 * M_PI;
		Arc1_SweepAngTemp2 = Arc1_EndAngTemp2 - Arc1_StartAngTemp2;
		Arc1_MidAngTemp2 = (Arc1_StartAngTemp2 + Arc1_EndAngTemp2)/2;
		Angle_FourQuad(&Arc1_MidAngTemp2);
		double Arc1_MidptTmp2[2] = {center1[0] + radius1 * cos(Arc1_MidAngTemp2), center1[1] + radius1 * sin(Arc1_MidAngTemp2)};
		
		Arc2_StartAngTemp1 = circle2angle;
		Arc2_EndAngTemp1 = endangle2;
		if(Arc2_EndAngTemp1 < Arc2_StartAngTemp1) Arc2_EndAngTemp1 += 2 * M_PI;
		Arc2_SweepAngTemp1 = Arc2_EndAngTemp1 - Arc2_StartAngTemp1;
		Arc2_MidAngTemp1 = (Arc2_StartAngTemp1 + Arc2_EndAngTemp1)/2;
		Angle_FourQuad(&Arc2_MidAngTemp1);
		double Arc2_MidptTmp1[2] = {center2[0] + radius2 * cos(Arc2_MidAngTemp1), center2[1] + radius2 * sin(Arc2_MidAngTemp1)};
		double Arc2_MidAngTemp_Tan1 = ray_angle(tancenter[0], tancenter[1], Arc2_MidptTmp1[0], Arc2_MidptTmp1[1]);
			
		Arc2_StartAngTemp2 = startang2;
		Arc2_EndAngTemp2 = circle2angle;
		if(Arc2_EndAngTemp2 < Arc2_StartAngTemp2) Arc2_EndAngTemp2 += 2 * M_PI;
		Arc2_SweepAngTemp2 = Arc2_EndAngTemp2 - Arc2_StartAngTemp2;
		Arc2_MidAngTemp2 = (Arc2_StartAngTemp2 + Arc2_EndAngTemp2)/2;
		Angle_FourQuad(&Arc2_MidAngTemp2);
		double Arc2_MidptTmp2[2] = {center2[0] + radius2 * cos(Arc2_MidAngTemp2), center2[1] + radius2 * sin(Arc2_MidAngTemp2)};

		bool SwapArcPositions = false;
		if(IsInBetween(Arc1_MidAngTemp_Tan1, intersctnAngle2, intersctnAngle1, MeasureDecValue))
		{
			if(IsInBetween(Arc2_MidAngTemp_Tan1, intersctnAngle2, intersctnAngle1, MeasureDecValue))
				SwapArcPositions = false;
			else
				SwapArcPositions = true;
		}
		else
		{
			if(IsInBetween(Arc2_MidAngTemp_Tan1, intersctnAngle2, intersctnAngle1, MeasureDecValue))
				SwapArcPositions = true;
			else
				SwapArcPositions = false;
		}

		if(SwapArcPositions)
		{
			SwapTwoValues(&Arc2_StartAngTemp1, &Arc2_StartAngTemp2);
			SwapTwoValues(&Arc2_EndAngTemp1, &Arc2_EndAngTemp2);
			SwapTwoValues(&Arc2_SweepAngTemp1, &Arc2_SweepAngTemp2);
			SwapTwoValues(&Arc2_MidAngTemp1, &Arc2_MidAngTemp2);
		}
		if((Arc1_SweepAngTemp1 + Arc2_SweepAngTemp1) > (Arc1_SweepAngTemp2 + Arc2_SweepAngTemp2))
		{
			Arc1_StartAngle = Arc1_StartAngTemp1; Arc1_EndAngle = Arc1_EndAngTemp1; Arc1_MidAngle = Arc1_MidAngTemp1;
			Arc2_StartAngle = Arc2_StartAngTemp1; Arc2_EndAngle = Arc2_EndAngTemp1; Arc2_MidAngle = Arc2_MidAngTemp1;
		}
		else
		{
			Arc1_StartAngle = Arc1_StartAngTemp2; Arc1_EndAngle = Arc1_EndAngTemp2; Arc1_MidAngle = Arc1_MidAngTemp2;
			Arc2_StartAngle = Arc2_StartAngTemp2; Arc2_EndAngle = Arc2_EndAngTemp2; Arc2_MidAngle = Arc2_MidAngTemp2;
		}
		
		Arc1Points[0] = center1[0] + radius1 * cos(Arc1_StartAngle); Arc1Points[1] = center1[1] + radius1 * sin(Arc1_StartAngle);
		Arc1Points[2] = center1[0] + radius1 * cos(Arc1_MidAngle); Arc1Points[3] = center1[1] + radius1 * sin(Arc1_MidAngle);
		Arc1Points[4] = center1[0] + radius1 * cos(Arc1_EndAngle); Arc1Points[5] = center1[1] + radius1 * sin(Arc1_EndAngle);

		Arc2Points[0] = center2[0] + radius2 * cos(Arc2_StartAngle); Arc2Points[1] = center2[1] + radius2 * sin(Arc2_StartAngle);
		Arc2Points[2] = center2[0] + radius2 * cos(Arc2_MidAngle); Arc2Points[3] = center2[1] + radius2 * sin(Arc2_MidAngle);
		Arc2Points[4] = center2[0] + radius2 * cos(Arc2_EndAngle); Arc2Points[5] = center2[1] + radius2 * sin(Arc2_EndAngle);

		double Swp_ArcStart, Swp_ArcEnd;
		if(intersctnAngle1 < intersctnAngle2) 
		{
			Swp_ArcStart = intersctnAngle1;
			Swp_ArcEnd = intersctnAngle2;
		}	
		else
		{
			Swp_ArcStart = intersctnAngle2;
			Swp_ArcEnd = intersctnAngle1;
		}
		double Arc1_MidAng_Temp = ray_angle(tancenter[0], tancenter[1], Arc1Points[2], Arc1Points[3]);
		if(!IsInBetween(Arc1_MidAng_Temp, intersctnAngle2, intersctnAngle1, MeasureDecValue))
		{
			if(intersctnAngle1 < intersctnAngle2) 
			{
				Swp_ArcStart = intersctnAngle2;
				Swp_ArcEnd = intersctnAngle1 + 2 * M_PI;
			}	
			else
			{
				Swp_ArcStart = intersctnAngle1;
				Swp_ArcEnd = intersctnAngle2 + 2 * M_PI;
			}
		}
		
		if(!AngleIsinArcorNot(Swp_ArcStart, Swp_ArcEnd, MidAngle1))
		{
			FilletArc[2] = ArcMidpt1[0]; FilletArc[3] = ArcMidpt1[1];
		}
		else
		{
			FilletArc[2] = ArcMidpt2[0]; FilletArc[3] = ArcMidpt2[1];
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0083g", __FILE__, __FUNCSIG__); return false; }
}

//Fillet arc for one Line and One Arc.
bool RapidMath2D::FilletForLineArc(double slope1, double intercept1, double *endpt11, double *endpt12, double *center1, double radius1, double startang1, double sweepang1, double radius, double *Arc1Points, double *FilletArc)
{
	try
	{
		double intercept2, intersectionpt[4], pt[2], tempangle;
		Point_PerpenIntrsctn_Line(slope1, intercept1, center1, &pt[0]);
		tempangle = ray_angle(pt[0], pt[1], center1[0], center1[1]);
		if(tempangle == 0 || tempangle == M_PI)
			intercept2 = intercept1 - radius;
		else
			intercept2 = intercept1 - (radius)/cos(tempangle + M_PI_2);
		int Cnt = Line_circleintersection(slope1, intercept2, center1, radius1 + radius, &intersectionpt[0]);
		if(Cnt == 0) 
			return false; // there is no tangential circle for the given radius..
		else 
		{
			if(Cnt == 1) // Intersecting at one point
			{
				bool CheckFlag = InternalFilletLinetoArc(slope1, intercept1, endpt11, endpt12, center1, radius1, startang1, sweepang1, &intersectionpt[0], radius, Arc1Points, FilletArc);
				return CheckFlag;
			}
			else // Intersecting at two points..
			{
				//Two Possible fillet arc's.. Select one from this.
				double LineEndpt11[2] = {endpt11[0], endpt11[1]}, LineEndpt12[2] = {endpt12[0], endpt12[1]}, Arc1Points1[6], FilletArc1[6];
				double LineEndpt21[2] = {endpt11[0], endpt11[1]}, LineEndpt22[2] = {endpt12[0], endpt12[1]}, Arc1Points2[6], FilletArc2[6];
				bool CheckFlag1 = InternalFilletLinetoArc(slope1, intercept1, &LineEndpt11[0], &LineEndpt12[0], center1, radius1, startang1, sweepang1, &intersectionpt[0], radius, &Arc1Points1[0], &FilletArc1[0]);
				bool CheckFlag2 = InternalFilletLinetoArc(slope1, intercept1, &LineEndpt21[0], &LineEndpt22[0], center1, radius1, startang1, sweepang1, &intersectionpt[2], radius, &Arc1Points2[0], &FilletArc2[0]);
				double llength1 = Pt2Pt_distance(LineEndpt11[0], LineEndpt11[1], LineEndpt12[0], LineEndpt12[1]);
				double llength2 = Pt2Pt_distance(LineEndpt21[0], LineEndpt21[1], LineEndpt22[0], LineEndpt22[1]);
				double Arc1Cen[2], Arc1Rad, Arc1StAng, Arc1SwpAng, Arc2Cen[2], Arc2Rad, Arc2StAng, Arc2SwpAng;
				Arc_3Pt(&Arc1Points1[0], &Arc1Points1[2], &Arc1Points1[4], &Arc1Cen[0], &Arc1Rad, &Arc1StAng, &Arc1SwpAng);
				Arc_3Pt(&Arc1Points2[0], &Arc1Points2[2], &Arc1Points2[4], &Arc2Cen[0], &Arc2Rad, &Arc2StAng, &Arc2SwpAng);
				if((!CheckFlag1 && !CheckFlag2)) 
					return false;
				else if(CheckFlag1 && CheckFlag2)
				{
					if(llength1 > llength2) //if(Arc1SwpAng > Arc2SwpAng) // Take first arc
					{
						endpt11[0] = LineEndpt11[0]; endpt11[1] = LineEndpt11[1];
						endpt12[0] = LineEndpt12[0]; endpt12[1] = LineEndpt12[1];
						for(int i = 0; i < 6; i++)
						{
							Arc1Points[i] = Arc1Points1[i];
							FilletArc[i] = FilletArc1[i];
						}
					}
					else
					{
						endpt11[0] = LineEndpt21[0]; endpt11[1] = LineEndpt21[1];
						endpt12[0] = LineEndpt22[0]; endpt12[1] = LineEndpt22[1];
						for(int i = 0; i < 6; i++)
						{
							Arc1Points[i] = Arc1Points2[i];
							FilletArc[i] = FilletArc2[i];
						}
					}
					return true;
				}
				else if(CheckFlag1)
				{
					endpt11[0] = LineEndpt11[0]; endpt11[1] = LineEndpt11[1];
					endpt12[0] = LineEndpt12[0]; endpt12[1] = LineEndpt12[1];
					for(int i = 0; i < 6; i++)
					{
						Arc1Points[i] = Arc1Points1[i];
						FilletArc[i] = FilletArc1[i];
					}
					return true;
				}
				else
				{
					endpt11[0] = LineEndpt21[0]; endpt11[1] = LineEndpt21[1];
					endpt12[0] = LineEndpt22[0]; endpt12[1] = LineEndpt22[1];
					for(int i = 0; i < 6; i++)
					{
						Arc1Points[i] = Arc1Points2[i];
						FilletArc[i] = FilletArc2[i];
					}
					return true;
				}
			}
			return false;
		}
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0081c", __FILE__, __FUNCSIG__); return false; }
}
//
bool RapidMath2D::InternalFilletLinetoArc(double slope1, double intercept1, double *endpt11, double *endpt12, double *center1, double radius1, double startang1, double sweepang1, double *tancenter, double tanradius, double *Arc1Points, double *FilletArc)
{
	try
	{
		double C2CInterSctn1[2], C2LineInterSctn1[2];
		double endangle1 = startang1 + sweepang1;
		Line_circleintersection(slope1, intercept1, tancenter, tanradius, &C2LineInterSctn1[0]);
		Circle_circleintersection(center1, tancenter, radius1, tanradius, &C2CInterSctn1[0]);
		double intersctnAngle1 = ray_angle(tancenter[0], tancenter[1], C2LineInterSctn1[0], C2LineInterSctn1[1]);
		double intersctnAngle2 = ray_angle(tancenter[0], tancenter[1], C2CInterSctn1[0], C2CInterSctn1[1]);
		double PerAngle = ray_angle(tancenter[0], tancenter[1], center1[0], center1[1]);
		double PerAngleLine = ray_angle(tancenter[0], tancenter[1], C2LineInterSctn1[0], C2LineInterSctn1[1]);


		double MidAngleDiff1 = intersctnAngle1 - intersctnAngle2, MidAngleDiff2 = intersctnAngle2 - intersctnAngle1;
		Angle_FourQuad(&MidAngleDiff1); Angle_FourQuad(&MidAngleDiff2);
		double MidAngle1 = intersctnAngle2 + MidAngleDiff1/2, MidAngle2 = intersctnAngle1 + MidAngleDiff2/2;
		Angle_FourQuad(&MidAngle1); Angle_FourQuad(&MidAngle2);

		FilletArc[0] = C2LineInterSctn1[0]; FilletArc[1] = C2LineInterSctn1[1]; //End points of fillet arc.
		FilletArc[4] = C2CInterSctn1[0]; FilletArc[5] = C2CInterSctn1[1];

		double Arc_StartAngle, Arc_EndAngle;
		double circle1angle = ray_angle(center1[0], center1[1], C2CInterSctn1[0], C2CInterSctn1[1]); // angle of the intersection point.
		if(circle1angle < startang1) circle1angle += 2 * M_PI;
		if(circle1angle > startang1 && circle1angle < endangle1) // Proper Intersection of arc and tangential circle.
		{
			double anglediff1 = circle1angle - startang1;
			double anglediff2 = endangle1 - circle1angle;
			if(anglediff1 < anglediff2) // Arc start is near to intersection point
			{
				Arc_StartAngle = circle1angle;
				Arc_EndAngle = endangle1;
			}
			else // end point is near to intersection point.
			{
				Arc_StartAngle = startang1;
				Arc_EndAngle = circle1angle;
			}
		}
		else
		{
			double anglediff1 = circle1angle - startang1,  anglediff2 = endangle1 - circle1angle;
			Angle_FourQuad(&anglediff1); Angle_FourQuad(&anglediff2);
			if(anglediff1 >= M_PI) anglediff1 = 2 * M_PI - anglediff1;
			if(anglediff2 >= M_PI) anglediff2 = 2 * M_PI - anglediff2;
			if(anglediff1 < anglediff2) 
			{
				Arc_StartAngle = circle1angle;
				Arc_EndAngle = endangle1;
			}
			else
			{
				Arc_StartAngle = startang1;
				Arc_EndAngle = circle1angle;
			}
		}
		Angle_FourQuad(&Arc_StartAngle); Angle_FourQuad(&Arc_EndAngle);
		if(Arc_EndAngle < Arc_StartAngle) Arc_EndAngle += 2 * M_PI;
		Arc1Points[0] = center1[0] + radius1 * cos(Arc_StartAngle); Arc1Points[1] = center1[1] + radius1 * sin(Arc_StartAngle);
		Arc1Points[2] = center1[0] + radius1 * cos((Arc_StartAngle + Arc_EndAngle)/2); Arc1Points[3] = center1[1] + radius1 * sin((Arc_StartAngle + Arc_EndAngle)/2);
		Arc1Points[4] = center1[0] + radius1 * cos(Arc_EndAngle); Arc1Points[5] = center1[1] + radius1 * sin(Arc_EndAngle);
				
		double ArcMidpt1[2] = {tancenter[0] + tanradius * cos(MidAngle1), tancenter[1] + tanradius * sin(MidAngle1)};
		double ArcMidpt2[2] = {tancenter[0] + tanradius * cos(MidAngle2), tancenter[1] + tanradius * sin(MidAngle2)};

		double pintercept;
		Intercept_LinePassing_Point(tancenter, PerAngle, &pintercept);

		double Adist1 = Pt2Line_DistSigned(ArcMidpt1[0], ArcMidpt1[1], PerAngle, pintercept);
		double Adist2 = Pt2Line_DistSigned(ArcMidpt2[0], ArcMidpt2[1], PerAngle, pintercept);
		double Adist3 = Pt2Line_DistSigned(Arc1Points[2], Arc1Points[3], PerAngle, pintercept);

		if((Adist1 < 0 && Adist3 > 0) || (Adist1 > 0 && Adist3 < 0))
		{
			FilletArc[2] = ArcMidpt1[0]; FilletArc[3] = ArcMidpt1[1];
		}
		else
		{
			FilletArc[2] = ArcMidpt2[0]; FilletArc[3] = ArcMidpt2[1];
		}
			
		double pinterceptLine;
		Intercept_LinePassing_Point(tancenter, PerAngleLine, &pinterceptLine);
		double Ldist11 = Pt2Line_DistSigned(endpt11[0], endpt11[1], PerAngleLine, pinterceptLine);
		double Ldist12 = Pt2Line_DistSigned(FilletArc[2], FilletArc[3], PerAngleLine, pinterceptLine);
		double Ldist21 = Pt2Line_DistSigned(endpt12[0], endpt12[1], PerAngleLine, pinterceptLine);
				
		if((Ldist11 < 0 && Ldist12 > 0) || (Ldist11 > 0 && Ldist12 < 0))
		{
			endpt12[0] = C2LineInterSctn1[0]; endpt12[1] = C2LineInterSctn1[1];
		}
		else if((Ldist21 < 0 && Ldist12 > 0) || (Ldist21 > 0 && Ldist12 < 0))
		{
			endpt11[0] = C2LineInterSctn1[0]; endpt11[1] = C2LineInterSctn1[1];
		}
		else
		{
			endpt11[0] = C2LineInterSctn1[0]; endpt11[1] = C2LineInterSctn1[1];
			endpt12[0] = C2LineInterSctn1[0]; endpt12[1] = C2LineInterSctn1[1];
			return false;
			/*double LrAngle = ray_angle(C2LineInterSctn1[0], C2LineInterSctn1[1], endpt11[0], endpt11[1]);
			double Llength = Pt2Pt_distance(endpt11[0], endpt11[1], endpt12[0], endpt12[1]);
			endpt11[0] = C2LineInterSctn1[0]; endpt11[1] = C2LineInterSctn1[1];
			endpt12[0] = C2LineInterSctn1[0] + Llength * cos(LrAngle + M_PI); endpt12[1] = C2LineInterSctn1[1] + Llength * sin(LrAngle + M_PI);*/
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0083a", __FILE__, __FUNCSIG__);  return false;}
}

//Check whether Circle inside other circle..
bool RapidMath2D::CircleInsideAnotherCircle(double *center1, double radius1, double *center2, double radius2)
{
	try
	{
		double largeRad, SmallRad;
		double Dist = Pt2Pt_distance(&center1[0], &center2[0]);
		if(radius1 < radius2) 
		{
			SmallRad = radius1;
			largeRad = radius2;	
		}
		else
		{
			SmallRad = radius2;
			largeRad = radius1;
		}
		if(Dist < largeRad) // Circle Inside other Circle..
		{
			double intersectionpt[4];
			int Cnt = Circle_circleintersection(center1, center2, radius1, radius2, &intersectionpt[0]);
			if(Cnt == 0)
				return true;
			else
				return false;
		}
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0083a", __FILE__, __FUNCSIG__); return false; }
}

//Find the nearest point in a set of points to a given point//
double RapidMath2D::Nearestpoint_point(int totalpoints, double *setofpoints, double *point, double *nearestpoint)
{
	try
	{
		double min, dis = 0, dist = 0;  
		//Calcultes the distances of points from the given point//
		for(int i = 0; i < totalpoints; i++)
		{
			//Calculate the sum fo the square of differences of each coordinate axis values of points//
			for(int k = 0; k < 2; k ++)
				dis += pow((point[k] - setofpoints[2 * i + k]), 2);
			if(i == 0)
			{
				min = sqrt(dis);
				nearestpoint[0] = setofpoints[2 * i];
				nearestpoint[1] = setofpoints[2 * i + 1];
			}
			else
			{
				dist = sqrt(dis);
				if(min > dist)
				{
					min = dist;
					nearestpoint[0] = setofpoints[2 * i];
					nearestpoint[1] = setofpoints[2 * i + 1];
				}
			}
			dis = 0;
		}
		return min;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0053", __FILE__, __FUNCSIG__); return 0;}
}

//Find the nearest point in a set of points to a given line//
double RapidMath2D::Nearestpoint_line(int totalpoints, double *setofpoints, double slope, double intercept, double *nearestpoint )
{
	try
	{
		double min, dist = 0;
		if(slope == M_PI_2)
		{
			for(int i = 0; i < totalpoints; i++)
			{
				if(i == 0)
				{
					dist = abs(setofpoints[2 * i] - intercept);
					min = dist;
					nearestpoint[0] = setofpoints[2 * i];
					nearestpoint[1] = setofpoints[2 * i + 1];
				}
				else
				{
					dist = abs(setofpoints[2 * i] - intercept);
					if(min > dist)
					{
						min = dist;
						nearestpoint[0] = setofpoints[2 * i];
						nearestpoint[1] = setofpoints[2 * i + 1];
					}
				}
			}
		}
		else
		{
			for(int i = 0; i < totalpoints; i++)
			{
				if(i == 0)
				{
					dist = abs((tan(slope) * setofpoints[2 * i] + intercept - setofpoints[2 * i + 1]) * cos(slope));
					min = dist;
					nearestpoint[0] = setofpoints[2 * i];
					nearestpoint[1] = setofpoints[2 * i + 1];
				}
				else
				{
					dist = abs((tan(slope) * setofpoints[2 * i] + intercept - setofpoints[2 * i + 1]) * cos(slope));
					if(min > dist)
					{
						min = dist;
						nearestpoint[0] = setofpoints[2 * i];
						nearestpoint[1] = setofpoints[2 * i + 1];
					}
				}
			}
		}
		return min;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0054", __FILE__, __FUNCSIG__); return 0; }
}

//Function to Find the nearest point in a set of points to a given circle///
double RapidMath2D::Nearestpoint_circle(int totalpoints, double *setofpoints, double *center, double radius, double *nearestpoint)
{
	try
	{
		double min, dis = 0, dist = 0;
		//Calcultes the distances of points from circle center//
		for(int i = 0; i < totalpoints; i++)
		{
			for(int k = 0; k < 2; k ++)
				dis += pow((center[k] - setofpoints[2 * i + k]), 2);
			dist = abs(sqrt(dis) - radius);
			if(i == 0)
			{
				min = dist;
				nearestpoint[0] = setofpoints[2 * i];
				nearestpoint[1] = setofpoints[2 * i + 1];
			}
			else
			{
				if(min > dist)
				{
					min = dist;
					nearestpoint[0] = setofpoints[2 * i];
					nearestpoint[1] = setofpoints[2 * i + 1];
				}
			}
			dis = 0;
		}
		return min;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0055", __FILE__, __FUNCSIG__); return 0; }
}

//Find the farthest point in a set of points to a given point//
double RapidMath2D::Farthestpoint_point(int totalpoints, double *setofpoints, double *point, double *farthestpoint)
{
	try
	{
		double max, dis = 0, dist = 0;  
		//Calcultes the distances of points from the given point//
		for(int i = 0; i < totalpoints; i++)
		{
			//Calculate the sum fo the square of differences of each coordinate axis values of points//
			for(int k = 0; k < 2; k ++)
				dis += pow((point[k] - setofpoints[2 * i + k]), 2);
			if(i == 0)
			{
				max = sqrt(dis);
				farthestpoint[0] = setofpoints[2 * i];
				farthestpoint[1] = setofpoints[2 * i + 1];
			}
			else
			{
				dist = sqrt(dis);
				if(max < dist)
				{
					max = dist;
					farthestpoint[0] = setofpoints[2 * i];
					farthestpoint[1] = setofpoints[2 * i + 1];
				}
			}
			dis = 0;
		}
		return max;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0056", __FILE__, __FUNCSIG__); return 0; }
}

//Find the farthest point in a set of points to a given line//
double RapidMath2D::Farthestpoint_line(int totalpoints, double *setofpoints, double slope, double intercept, double *farthestpoint)
{
	try
	{
		double max, dist = 0;
		//if the line is vertical then the perpendicular distances are diff. b/n intercept and X.point//
		if(slope == M_PI_2)
		{
			for(int i = 0; i < totalpoints; i++)
			{
				if(i == 0)
				{
					dist = abs(setofpoints[2 * i] - intercept);
					max = dist;
					farthestpoint[0] = setofpoints[2 * i];
					farthestpoint[1] = setofpoints[2 * i + 1];
				}
				else
				{
					dist = abs(setofpoints[2 * i] - intercept);
					if(max < dist)
					{
						max = dist;
						farthestpoint[0] = setofpoints[2 * i];
						farthestpoint[1] = setofpoints[2 * i + 1];
					}
				}
			}
		}
		else
		{
			for(int i = 0; i < totalpoints; i ++)
			{
				if(i == 0)
				{
					dist = abs((tan(slope) * setofpoints[2 * i] + intercept - setofpoints[2 * i + 1]) * cos(slope));
					max = dist;
					farthestpoint[0] = setofpoints[2 * i];
					farthestpoint[1] = setofpoints[2 * i + 1];
				}
				else
				{
					dist = abs((tan(slope) * setofpoints[2 * i] + intercept - setofpoints[2 * i + 1]) * cos(slope));
					if(max < dist)
					{
						max = dist;
						farthestpoint[0] = setofpoints[2 * i];
						farthestpoint[1] = setofpoints[2 * i + 1];
					}
				}
			}
		}
		return max;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0057", __FILE__, __FUNCSIG__); return 0; }
}

//Function to Find the farthest point in a set of points to a given circle///
double RapidMath2D::Farthestpoint_circle(int totalpoints, double *setofpoints, double *center, double radius, double *farthestpoint)
{
	try
	{
		double max, dis = 0, dist = 0;
		//Calcultes the distances of points from circle center//
		for(int i = 0; i < totalpoints; i ++)
		{
			for(int k = 0; k < 2; k ++)
				dis += pow((center[k] - setofpoints[2 * i + k]), 2);
			dist = abs(sqrt(dis) - radius);
			if(i == 0)
			{
				max = dist;
				farthestpoint[0] = setofpoints[2 * i];
				farthestpoint[1] = setofpoints[2 * i + 1];
			}
			else
			{
				if(max < dist)
				{
					max = dist;
					farthestpoint[0] = setofpoints[2 * i];
					farthestpoint[1] = setofpoints[2 * i + 1];
				}
			}
			dis = 0;
		}
		return max;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0059", __FILE__, __FUNCSIG__); return 0; }
}

//Function to find the intersection point of two line//
int RapidMath2D::Line_lineintersection(double slope1, double intercept1, double slope2, double intercept2, double *intersectnpt)
{ 
	try
	{
		Angle_FirstScndQuad(&slope1); Angle_FirstScndQuad(&slope2);
		//if slopes are equal then the lines are parallel//
		if(abs(slope1 - slope2) < 0.00001)
			return 0;  
		else
		{
			if(slope1 == M_PI_2)  //If one line is vertical then the intersection point of the lines//
			{
				intersectnpt[0] = intercept1;
				intersectnpt[1] = (tan(slope2)) * (*intersectnpt) + intercept2;
			}
			else if(slope2 == M_PI_2) //If other line is vertical then the intersection point of the lines//
			{
				intersectnpt[0] = intercept2;
				intersectnpt[1] = (tan(slope1) * intersectnpt[0]) + intercept1;
			}
			else
			{
				intersectnpt[0] = (intercept1 - intercept2) / (tan(slope2) - tan(slope1));   
				intersectnpt[1] = (tan(slope1) * intersectnpt[0]) + intercept1;
			}
		}
		return 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0061", __FILE__, __FUNCSIG__); return false; }
}

//function to calculate the line to finite line intersection
int RapidMath2D::Line_finiteline(double slope1, double intecept1, double *endpt1, double *endpt2, double slopeofline, double intercept2, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[2];//Intersection of the point ray and line
		if(IsEqual(endpt1[0], endpt2[0], MeasureDecValue) && IsEqual(endpt1[1], endpt2[1], MeasureDecValue))
			return 0;
		if(Line_lineintersection(slopeofline, intercept2, slope1, intecept1, &point1[0]) == 0)
			return 0;
		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))
		{
			intersectnpt1[0]  = point1[0];   
			intersectnpt1[1]  = point1[1];
			Count[0] = 1;
		}
		else
		{
			intersectnpt2[0]  = point1[0];   
			intersectnpt2[1]  = point1[1];
			Count[1] = 1;
		}
		return 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0096", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate the intersection point of ray with given infinte line//
int RapidMath2D::raylineint(double slopeofline, double intercept, double *point, double slopeofray, double interceptofray, double *intersectnpt1, double *intersectnpt2, int *Count )
{
	try
	{
		double point1[2], slope1; 
		//intersection point of line and ray//
		if(Line_lineintersection(slopeofline, intercept, slopeofray, interceptofray, &point1[0]) == 0)
			return 0;
		slope1 = ray_angle(point, &point1[0]);
		if(IsEqual(slopeofray, slope1, MeasureDecValue))
		{	
			intersectnpt1[0] = point1[0];   
			intersectnpt1[1] = point1[1];
			Count[0] = 1;
		}	
		else
		{
			intersectnpt2[0] = point1[0];   
			intersectnpt2[1] = point1[1];
			Count[1] = 1;
		}
		return 1;

		/*if(IsEqual(slopeofray, slope1, MeasureDecValue))
		{	
			intersectnpt2[0] = point1[0];   
			intersectnpt2[1] = point1[1];
			Count[1] = 1;
		}	
		else
		{
			intersectnpt1[0] = point1[0];   
			intersectnpt1[1] = point1[1];
			Count[0] = 1;
		}
		return 1;*/
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0091", __FILE__, __FUNCSIG__); return 0; }
}

//function to find the intersection point of finite line with the given  ray//
int RapidMath2D::ray_finitelineint(double *endpt1, double *endpt2, double slopeofline, double intercept, double *point, double slopeofray, double interceptofray, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[2] = {0}, slope1 = 0; //Intersection of the point ray and line
		//calculate the inersection point//
		if(Line_lineintersection(slopeofline, intercept, slopeofray, interceptofray, &point1[0]) == 0)
			return 0; //if lines are parellel..............
		slope1 = ray_angle(point, &point1[0]);
		if(IsEqual(slopeofray, slope1, MeasureDecValue)) // if intersection point is on ray....
		{	
			if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))  //check whether the intersection point is in the line or not//
			{
				intersectnpt1[0] = point1[0];   
				intersectnpt1[1] = point1[1];
				Count[0] = 1;
				return 1;
			}
			else
			{
				intersectnpt2[0]  = point1[0];   
				intersectnpt2[1]  = point1[1];
				Count[1] = 1;
				return 2;
			}
		}
		else
		{
			if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2)) //check whether the intersection point is in the line or not//
			{
				intersectnpt1[0] = point1[0];   
				intersectnpt1[1] = point1[1];
				Count[0] = 1;
				return 3;
			}
			else
			{
				intersectnpt2[0]  = point1[0];   
				intersectnpt2[1]  = point1[1];
				Count[1] = 1;
				return 4;
			}
		}



		//double point1[2] = {0}, slope1 = 0; //Intersection of the point ray and line
		////calculate the inersection point//
		//if(Line_lineintersection(slopeofline, intercept, slopeofray, interceptofray, &point1[0]) == 0)
		//	return 0; //if lines are parellel..............
		//slope1 = ray_angle(point, &point1[0]);
		//if(IsEqual(slopeofray, slope1, MeasureDecValue))
		//{	
		//	intersectnpt1[0]  = point1[0];   
		//	intersectnpt1[1]  = point1[1];
		//	Count[0] = 1;
		//	return 1;
		//}

		///*if(IsEqual(slopeofray, slope1, MeasureDecValue))
		//{	
		//	intersectnpt2[0]  = point1[0];   
		//	intersectnpt2[1]  = point1[1];
		//	Count[1] = 1;
		//	return 1;
		//}*/

		////check whether the intersection point is in the line or not//
		//if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))
		//{
		//	intersectnpt1[0] = point1[0];   
		//	intersectnpt1[1] = point1[1];
		//	Count[0] = 1;
		//}
		//else
		//{
		//	intersectnpt2[0]  = point1[0];   
		//	intersectnpt2[1]  = point1[1];
		//	Count[1] = 1;
		//}
		//return 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0092", __FILE__, __FUNCSIG__); return 0; }
}

//function to calculate the ray to ray intersection//
int RapidMath2D::ray_rayintersec(double *point, double slopeofray, double intercept, double *point1, double slopeofray1, double intercept1, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double slope1,slope2, point11[2];
		//calculate the intersection points of the two rays//
		if(Line_lineintersection(slopeofray, intercept, slopeofray1, intercept1, &point11[0]) == 0)
			return 0;
		slope1 = ray_angle(point, &point11[0]);
		slope2 = ray_angle(point1, &point11[0]);
		//if the intersection outside the ray return false//
		if(IsEqual(slope1, slopeofray, MeasureDecValue) && IsEqual(slope2, slopeofray1, MeasureDecValue))
		{
			intersectnpt1[0] = point11[0];   
			intersectnpt1[1] = point11[1];
			Count[0] = 1;
		}
		else
		{
			intersectnpt2[0] = point11[0];   
			intersectnpt2[1] = point11[1];
			Count[1] = 1;
		}
		return 1;

		/*if(IsEqual(slope1, slopeofray, MeasureDecValue) || IsEqual(slope2, slopeofray1, MeasureDecValue))
		{
			intersectnpt2[0] = point11[0];   
			intersectnpt2[1] = point11[1];
			Count[1] = 1;
		}
		else
		{
			intersectnpt1[0] = point11[0];   
			intersectnpt1[1] = point11[1];
			Count[0] = 1;
		}
		return 1;*/

	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0098", __FILE__, __FUNCSIG__); return 0; }
}

//function to find the finite line to finite line intersection//
int RapidMath2D::Finiteline_line(double *endpt1, double *endpt2, double *endpt3, double *endpt4, double slope1, double intercept1, double slope2, double intercept2, double *intersectnpt1, double *intersectnpt2, int *count)
{
	try
	{
		double point1[2];
		//Caclculate the intesection point of lines//
		if(Line_lineintersection(slope1, intercept1, slope2, intercept2, &point1[0]) == 0) 
			return 0;
		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2) && PtisinwindowRounded(point1[0], point1[1], endpt3, endpt4))
		{
			intersectnpt1[0] = point1[0];   
			intersectnpt1[1] = point1[1];
			count[0] = 1;
		}
		else
		{
			intersectnpt2[0] = point1[0];   
			intersectnpt2[1] = point1[1];
			count[1] = 1;
		}
		return 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0100", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the intersection point of line and a circle//
int RapidMath2D::Line_circleintersection(double slope, double intercept, double *center, double radius, double *intersectnpt)
{
	try
	{
		double distance, dis, distance1;
		Angle_FirstScndQuad(&slope);
		//Circle center to Line Distance..
		distance = Pt2Line_DistSigned(center[0], center[1], slope, intercept);
		distance1 = abs(distance);
		//if(IsGreater(distance1, radius))  //Line is outside the circle .. there is no intersection point//
		//	return 0;
		if((distance1 - radius) > 0.001)  //Line is outside the circle .. there is no intersection point//
			return 0;
		else if(IsEqual(distance1, radius, MeasureDecValue)) //Line is tangent to the circle.. one intersection point
		{
			if(slope == M_PI_2)
			{
				intersectnpt[0] = center[0] - distance;   //Intersection point when the line is vertical//
				intersectnpt[1] = center[1];
			}
			else
			{
				intersectnpt[0] = center[0] - distance * sin(slope);            
				intersectnpt[1] = center[1] + distance * cos(slope);
			}
			return 1;
		}
		else  //line intersecting the circle at two points//
		{
			if(slope == M_PI_2) //if the line is vertical//
			{
				//middle point of intersection points//
				double point[2] = {center[0] - distance, center[1]};
				//distance between middle point and intersection point//
				dis = sqrt(pow(radius, 2) - pow(distance1, 2));
				//Intersection points //
				intersectnpt[0] = point[0];							
				intersectnpt[1] = point[1] + dis;
				intersectnpt[2] = point[0];
				intersectnpt[3] = point[1] - dis;
			}
			else
			{
				//middle point of intersection points//
				double point[2] = {center[0] - distance * sin(slope), center[1] + distance * cos(slope)};
				//distance between middle point and intersection point//
				dis = sqrt(pow(radius, 2) - pow(distance1, 2));
				intersectnpt[0] = point[0] + dis * cos(slope);			//Intersection points //
				intersectnpt[1] = point[1] + dis * sin(slope);
				intersectnpt[2] = point[0] - dis * cos(slope);
				intersectnpt[3] = point[1] - dis * sin(slope);
			}
			return 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0062", __FILE__, __FUNCSIG__); return 0; }
}

//function to find the intersection between the finite line and circle//
int RapidMath2D::Finiteline_circle(double *endpt1, double *endpt2, double *center, double radius, double *intersectnpt, int *Count)
{
	try
	{
		int NoOfIntersection = 0;
		double slope = 0, intercept = 0, point1[4] = {0};
		TwoPointLine(endpt1, endpt2, &slope, &intercept);
		//calculate line to circle intersection//
		int Cnt = Line_circleintersection(slope, intercept, center, radius, &point1[0]);
		NoOfIntersection = Cnt;
		intersectnpt[0]  = point1[0];   
		intersectnpt[1]  = point1[1];
		intersectnpt[2]  = point1[2];   
		intersectnpt[3]  = point1[3];
		Count[0] = 0;
		Count[1] = 0;
		if(Cnt == 1)
		{				
			if(Ptisinwindow(&point1[0], endpt1, endpt2))
				Count[0] = 1;
		}
		else if(Cnt == 2)
		{
			if(Ptisinwindow(&point1[0], endpt1, endpt2) && Ptisinwindow(&point1[2], endpt1, endpt2))
			{
				Count[0] = 2;
			}
			else if(Ptisinwindow(&point1[0], endpt1, endpt2))
			{
				Count[0] = 1;
			}
			else if(Ptisinwindow(&point1[2], endpt1, endpt2))
			{
				Count[1] = 1;
			}
		}
		return NoOfIntersection;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0103", __FILE__, __FUNCSIG__); return 0; }
}

int RapidMath2D::Finiteline_circle(double *endpt1, double *endpt2, double slope1, double intercept1, double *center, double radius, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[4];
		//calculate line to circle intersection//
		int Cnt = Line_circleintersection(slope1, intercept1, center, radius, &point1[0]);
		if(Cnt == 0) return 0;
		if(Cnt == 1)
		{
			if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))
			{
				*intersectnpt1  = point1[0];   
				*(intersectnpt1 + 1)  = point1[1];
				*Count = 1;
			}
			else
			{
				*intersectnpt2  = point1[0];   
				*(intersectnpt2 + 1)  = point1[1];
				*(Count + 1) = 1;
			}
		}
		else
		{
			if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2) && PtisinwindowRounded(point1[2], point1[3], endpt1, endpt2))
			{
				*intersectnpt1 = point1[0];   
				*(intersectnpt1 + 1) = point1[1];
				*(intersectnpt1 + 2) = point1[2];  
				*(intersectnpt1 + 3) = point1[3];
				*Count = 2;
			}
			else if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))
			{
				*intersectnpt1  = point1[0];   
				*(intersectnpt1 + 1)  = point1[1];
				*intersectnpt2  = point1[2];   
				*(intersectnpt2 + 1)  = point1[3];
				*Count = 1;
				*(Count + 1) = 1;
			}
			else if(PtisinwindowRounded(point1[2], point1[3], endpt1, endpt2))
			{
				*intersectnpt1  = point1[2];   
				*(intersectnpt1 + 1)  = point1[3];
				*intersectnpt2  = point1[0];   
				*(intersectnpt2 + 1)  = point1[1];
				*Count = 1;
				*(Count + 1) = 1;
			}
			else
			{
				*intersectnpt2 = point1[0];   
				*(intersectnpt2 + 1) = point1[1];
				*(intersectnpt2 + 2) = point1[2];  
				*(intersectnpt2 + 3) = point1[3];
				*(Count + 1) = 2;
			}
		}
		return 2;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0103", __FILE__, __FUNCSIG__); return 0; }
}

//ray to circle intersection //
int RapidMath2D::ray_circleint(double *center, double radius, double *point, double slopeofray, double intercept, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[4], slope1, slope2;
		//Call line to circle intercsection//
		if(Line_circleintersection(slopeofray,intercept,center,radius,&point1[0]) == 0)
			return 0;
		slope1 = ray_angle(point, &point1[0]);
		slope2 = ray_angle(point, &point1[2]);
		if(IsEqual(slopeofray, slope1, MeasureDecValue) && IsEqual(slopeofray, slope2, MeasureDecValue)) //if both intersection point is on ray...
		{
			for(int i = 0; i < 4; i++)
				intersectnpt1[i] = point1[i];
			Count[0] = 2;
			return 1;
		}
		else if(IsEqual(slopeofray, slope1, MeasureDecValue)) //if first intersection point is on ray...
		{
			intersectnpt1[0] = point1[0];   
			intersectnpt1[1] = point1[1];
			intersectnpt2[0] = point1[2];   
			intersectnpt2[1] = point1[3];
			Count[0] = 1;
			Count[1] = 1;
			return 2;
		}
		else if(IsEqual(slopeofray, slope2, MeasureDecValue)) //if second intersection point is on ray...
		{
			intersectnpt1[0] = point1[2];   
			intersectnpt1[1] = point1[3];  
			intersectnpt2[0] = point1[0];   
			intersectnpt2[1] = point1[1];
			Count[0] = 1;
			Count[1] = 1;
			return 3;
		}
		else
		{
			for(int i = 0; i < 4; i++)
				intersectnpt2[i] = point1[i];
			Count[1] = 2;
			return 4;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0102", __FILE__, __FUNCSIG__); return 0; }
}

//Calculate the Line to Arc intersection..//
int RapidMath2D::Line_arcinter(double slope, double intercept, double *center, double radius, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[4], slope1, slope2;
		double endangle, temp, temp1, temp2, temp3, temp4;
		//calculate the line to circle intersection.//
		if(Line_circleintersection(slope, intercept, center, radius, &point1[0]) == 0)
			return 0;
		slope1 = ray_angle(center, &point1[0]);
		slope2 = ray_angle(center, &point1[2]);
		endangle = startang + sweepang;
		if(startang > endangle)
		{
			temp = startang;
			startang  = endangle;
			endangle = temp;
		}
		if(endangle > 2 * M_PI)
			endangle -= 2 * M_PI;
		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, startang, &temp);
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, startang, &temp2);
		ArcAngleDiff(endangle, slope2, &temp3);
		ArcAngleDiff(slope2, startang, &temp4);
		
		bool flag1 = IsGreaterOrEqual(temp, temp1, MeasureDecValue);
		bool flag2 = IsGreaterOrEqual(temp, temp2, MeasureDecValue);
		bool flag3 = IsGreaterOrEqual(temp, temp3, MeasureDecValue);
		bool flag4 = IsGreaterOrEqual(temp, temp4, MeasureDecValue);
		//Check whether the intersection points are in arc or not
		if(flag1 && flag2 && flag3 && flag4)
		{
			for(int i = 0; i < 4; i++)
				intersectnpt1[i] = point1[i];
			Count[0] = 2;
		}
		else if(flag1 && flag2)
		{
			intersectnpt1[0] = point1[0];   
			intersectnpt1[1] = point1[1];
			intersectnpt2[0] = point1[2];   
			intersectnpt2[1] = point1[3];
			Count[0] = 1;
			Count[1] = 1;
		}
		else if(flag3 && flag4)
		{
			intersectnpt1[0] = point1[2];   
			intersectnpt1[1] = point1[3];  
			intersectnpt2[0] = point1[0];   
			intersectnpt2[1] = point1[1];
			Count[0] = 1;
			Count[1] = 1;
		}
		else
		{
			for(int i = 0; i < 4; i++)
				intersectnpt2[i] = point1[i];
			Count[1] = 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0105", __FILE__, __FUNCSIG__); return 0; }
} 

//function to calculate the finite line to circle intersection//
int RapidMath2D::Finiteline_arcinter(double *endpt1, double *endpt2, double slope, double intercept, double *center, double radius, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[4], slope1, slope2;
		double endangle, temp, temp1, temp2, temp3, temp4;
		//calculate the line to circle intersection.//
		if(Line_circleintersection(slope,intercept,center,radius,&point1[0]) == 0)
			return 0;
		slope1 = ray_angle(center, &point1[0]);
		slope2 = ray_angle(center, &point1[2]);
		endangle = startang + sweepang;
		if(startang > endangle)
		{
			temp = startang;
			startang  = endangle;
			endangle = temp;
		}
		if(endangle > 2*M_PI)
			endangle -= 2*M_PI;
		//angles to check the position of the points on the arc//
		ArcAngleDiff(endangle, startang, &temp);
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, startang, &temp2);
		ArcAngleDiff(endangle, slope2, &temp3);
		ArcAngleDiff(slope2, startang, &temp4);

		bool flag1 = IsGreaterOrEqual(temp, temp1, MeasureDecValue);
		bool flag2 = IsGreaterOrEqual(temp, temp2, MeasureDecValue);
		bool flag3 = IsGreaterOrEqual(temp, temp3, MeasureDecValue);
		bool flag4 = IsGreaterOrEqual(temp, temp4, MeasureDecValue);

		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2) && PtisinwindowRounded(point1[2], point1[3], endpt1, endpt2))
		{
			if(flag1 && flag2 && flag3 && flag4)
			{
				*intersectnpt1  = point1[0];   
				*(intersectnpt1 + 1) = point1[1];
				*(intersectnpt1 + 2) = point1[2];  
				*(intersectnpt1 + 3) = point1[3];  
				*Count = 2;
			}
			else if(flag1 && flag2)
			{
				*intersectnpt1 = point1[0];   
				*(intersectnpt1 + 1) = point1[1];
				*intersectnpt2 = point1[2];    
				*(intersectnpt2 + 1) = point1[3];
				*Count = 1;
				*(Count + 1) = 1;
			}
			else if(flag3 && flag4)
			{
				*intersectnpt1 = point1[2];    
				*(intersectnpt1 + 1) = point1[3];
				*intersectnpt2 = point1[0];    
				*(intersectnpt2 + 1) = point1[1];
				*Count = 1;
				*(Count + 1) = 1;
			}
			else
			{
				*intersectnpt2 = point1[0];   
				*(intersectnpt2 + 1) = point1[1];
				*(intersectnpt2 + 2) = point1[2];  
				*(intersectnpt2 + 3) = point1[3];  
				*(Count + 1) = 2;
			}
		}
		else if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2) && flag1 && flag2)
		{
			*intersectnpt1 = point1[0];   
			*(intersectnpt1 + 1) = point1[1];
			*intersectnpt2 = point1[2];    
			*(intersectnpt2 + 1) = point1[3];
			*Count = 1;
			*(Count + 1) = 1;
		}
		else if(PtisinwindowRounded(point1[2], point1[3], endpt1, endpt2) && flag3 && flag4)
		{
			*intersectnpt1 = point1[2];    
			*(intersectnpt1 + 1) = point1[3];
			*intersectnpt2 = point1[0];    
			*(intersectnpt2 + 1) = point1[1];
			*Count = 1;
			*(Count + 1) = 1;
		}
		else
		{	
			*intersectnpt2 = point1[0];   
			*(intersectnpt2 + 1) = point1[1];
			*(intersectnpt2 + 2) = point1[2];  
			*(intersectnpt2 + 3) = point1[3];  
			*(Count + 1) = 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0106", __FILE__, __FUNCSIG__); return 0; }
}

//Calculate the Ray to arc intersection..//
int RapidMath2D::ray_arcinter(double *point, double slopeofray, double intercept, double *center, double radius, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[4], slope1, slope2, slope3, slope4;
		double endangle, temp, temp1, temp2, temp3, temp4;
		//calculate the line to circle intersection//
		if(Line_circleintersection(slopeofray,intercept,center,radius,&point1[0]) == 0)
			return 0;
		slope1 = ray_angle(center, &point1[0]);
		slope2 = ray_angle(center, &point1[2]);
		slope3 = ray_angle(point, &point1[0]);
		slope4 = ray_angle(point, &point1[2]);

		endangle = startang + sweepang;
		if(startang > endangle)
		{
			temp = startang;
			startang  = endangle;
			endangle = temp;

		}
		if(endangle > 2 * M_PI)
			endangle -= 2 * M_PI;
		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, startang, &temp);
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, startang, &temp2);
		ArcAngleDiff(endangle, slope2, &temp3);
		ArcAngleDiff(slope2, startang, &temp4);
		
		bool flag1 = IsGreaterOrEqual(temp, temp1, MeasureDecValue);
		bool flag2 = IsGreaterOrEqual(temp, temp2, MeasureDecValue);
		bool flag3 = IsGreaterOrEqual(temp, temp3, MeasureDecValue);
		bool flag4 = IsGreaterOrEqual(temp, temp4, MeasureDecValue);

		//according to the position of the arc and ray get the intersection points//
		if(IsEqual(slope3, slopeofray, MeasureDecValue) && IsEqual(slope4, slopeofray, MeasureDecValue))
		{
			if(flag1 && flag2 && flag3 && flag4)
			{
				*intersectnpt1  = point1[0];   
				*(intersectnpt1 + 1) = point1[1];
				*(intersectnpt1 + 2) = point1[2];  
				*(intersectnpt1 + 3) = point1[3];  
				*Count = 2;
				return 1;
			}
			else if(flag1 && flag2)
			{
				*intersectnpt1  = point1[0];   
				*(intersectnpt1 + 1) = point1[1];
				*intersectnpt2  = point1[2];   
				*(intersectnpt2 + 1) = point1[3];
				*Count = 1;
				*(Count + 1) = 1;
				return 2;
			}
			else if(flag3 && flag4)
			{
				*intersectnpt1  = point1[2];   
				*(intersectnpt1 + 1) = point1[3];
				*intersectnpt2  = point1[0];   
				*(intersectnpt2 + 1) = point1[1];
				*Count = 1;
				*(Count + 1) = 1;
				return 3;
			}
			else
			{
				*intersectnpt2  = point1[0];   
				*(intersectnpt2 + 1) = point1[1];
				*(intersectnpt2 + 2) = point1[2];  
				*(intersectnpt2 + 3) = point1[3];  
				*(Count + 1) = 2;
				return 4;
			}

		}
		else if(IsEqual(slope3, slopeofray, MeasureDecValue) && flag1 && flag2)
		{
			*intersectnpt1  = point1[0];   
			*(intersectnpt1 + 1) = point1[1];
			*intersectnpt2  = point1[2];   
			*(intersectnpt2 + 1) = point1[3];
			*Count = 1;
			*(Count + 1) = 1;
			return 5;
		}
		else if(IsEqual(slope4, slopeofray, MeasureDecValue) && flag3 && flag4)
		{
			*intersectnpt1  = point1[2];   
			*(intersectnpt1 + 1) = point1[3];
			*intersectnpt2  = point1[0];   
			*(intersectnpt2 + 1) = point1[1];
			*Count = 1;
			*(Count + 1) = 1;
			return 6;
		}
		else
		{	
			*intersectnpt2  = point1[0];   
			*(intersectnpt2 + 1) = point1[1];
			*(intersectnpt2 + 2) = point1[2];  
			*(intersectnpt2 + 3) = point1[3];  
			*(Count + 1) = 2;
			return 7;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0108", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the intersection point of circle to circle 
int RapidMath2D::Circle_circleintersection(double *center1, double *center2, double radius1, double radius2, double *intersectnpt)
{
	try
	{
		double slope = ray_angle(center1, center2);
		double distance = Pt2Pt_distance(center1, center2);
		double distAdd = radius1 + radius2;
		double distDiff = abs(radius1 - radius2);
		if(IsGreater(distDiff, distance, MeasureDecValue)) //Circle inside another circle..//
			return 0;
		if(IsEqual(distance, 0, MeasureDecValue))  //cocentric circles//
			return 0; 
		if(IsGreater(distance, distAdd, MeasureDecValue))  //circles are far away//
			return 0;

		if(IsEqual(distance, distAdd, MeasureDecValue))  //circles touch at a point tangentially.. External Intersection//
		{ 
			intersectnpt[0] = center1[0] + radius1 * cos(slope);
			intersectnpt[1] = center1[1] + radius1 * sin(slope);  	
			return 1;
		}
		else if(IsEqual(distance, distDiff, MeasureDecValue))  //circles touch at a point tangentially.. Internal Intersection//
		{
			double r_ang = slope;
			if(radius1 < radius2)
				r_ang += M_PI;
			intersectnpt[0] = center1[0] + radius1 * cos(r_ang);
			intersectnpt[1] = center1[1] + radius1 * sin(r_ang);  	
			return 1;
		}
		else    //Circles intersect at two points//
		{
			//angle of the triangle made by the centers and intersection point//
			double angle = acos((pow(radius1, 2) + pow(distance, 2) - pow(radius2, 2)) / (2 * (radius1) * (distance)));
			//resultant slope of the line joining the center and the intersection points//
			double slope1 = slope + angle, slope2 = slope - angle;
			intersectnpt[0] = center1[0] + radius1 * cos(slope1);
			intersectnpt[1] = center1[1] + radius1 * sin(slope1);  	
			intersectnpt[2] = center1[0] + radius1 * cos(slope2);
			intersectnpt[3] = center1[1] + radius1 * sin(slope2);  	
			return 2;	
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0063", __FILE__, __FUNCSIG__); return 0; }
}

//Circle to arc intersection...//
int RapidMath2D::Circle_arcintersection(double *center1, double radius1, double *center2, double radius2, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[4],slope1,slope2;
		double endangle;
		int InterCount = 0;
		//Calculate the intersection points//
		InterCount = Circle_circleintersection(center1, center2, radius1, radius2, &point1[0]);
		if(InterCount == 0)	return 0;
		//calculate the angle of intersection points with the center//
		slope1 =  ray_angle(center2, &point1[0]);
		slope2 = ray_angle(center2, &point1[2]);
		endangle = startang + sweepang;
		if(startang > endangle)
			endangle += 2 * M_PI;
		if(endangle > 2 * M_PI)
		{
			if(slope1 < startang) slope1 += 2 * M_PI;
			if(slope2 < startang) slope2 += 2 * M_PI;
		}
		if(InterCount == 1)
		{
			if(slope1 < endangle && slope1 > startang)
			{
				intersectnpt1[0] = point1[0];   
				intersectnpt1[1] = point1[1];
				Count[0] = 1;
			}
			else
			{
				intersectnpt2[0] = point1[0];   
				intersectnpt2[1] = point1[1];
				Count[1] = 1;
			}
			return 1;
		}
		else if(InterCount == 2)
		{
			if(((slope1 < endangle) && (slope1 > startang)) && ((slope2 < endangle) && (slope2 > startang)))
			{
				for(int i = 0; i < 4; i++)
					intersectnpt1[i] = point1[i];
				Count[0] = 2;
			}
			else if((slope1 < endangle) && (slope1 > startang))
			{
				intersectnpt1[0] = point1[0];   
				intersectnpt1[1] = point1[1];
				intersectnpt2[0] = point1[2];   
				intersectnpt2[1] = point1[3];
				Count[0] = 1; Count[1] = 1;
			}
			else if((slope2 < endangle) && (slope2 > startang))
			{
				intersectnpt1[0] = point1[2];   
				intersectnpt1[1] = point1[3];
				intersectnpt2[0] = point1[0];   
				intersectnpt2[1] = point1[1];
				Count[0] = 1; Count[1] = 1;
			}
			else
			{
				for(int i = 0; i < 4; i++)
					intersectnpt2[i] = point1[i];
				Count[1] = 2;
			}
			return 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0109", __FILE__, __FUNCSIG__); return false; }
}

//function to calculate the arc to arc intersection point//
int RapidMath2D::arc_arcintersection(double *center1, double radius1, double startang1, double sweepang1, double *center2, double radius2, double startang2, double sweepang2, double *intersectnpt1, double *intersectnpt2, int *Count)
{
	try
	{
		double point1[4], slope1, slope2, slope3, slope4;
		double endangle1, endangle2, temp, tempc, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		//Calculate the intersection points//
		if(Circle_circleintersection(center1,center2,radius1,radius2,&point1[0]) == 0) 
			return 0;
		//calculate the angle of line joinig intersection points with the center1//
		slope1 = ray_angle(center1, &point1[0]);
		slope2 = ray_angle(center1, &point1[2]);
		//calculate the angle of line joinig intersection point with respect to center2
		slope3 = ray_angle(center2, &point1[0]);
		slope4 = ray_angle(center2, &point1[2]);
		endangle1 = startang1 + sweepang1;
		endangle2 = startang2 + sweepang2;
		if(endangle1 > 2 * M_PI)
			endangle1 -= 2 * M_PI;
		if(endangle2 > 2 * M_PI)
			endangle2 -= 2 * M_PI;
		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle1, startang1, &temp);
		ArcAngleDiff(endangle2, startang2, &tempc);
		//temp1 to temp4 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle1, slope1, &temp1);
		ArcAngleDiff(slope1, startang1, &temp2);
		ArcAngleDiff(endangle1, slope2, &temp3);
		ArcAngleDiff(slope2, startang1, &temp4);
		//temp5 to temp8 are the angle difference between startangle or endangle with the intersection angle of arc2//
		ArcAngleDiff(endangle2, slope3, &temp5);
		ArcAngleDiff(slope3, startang2, &temp6);
		ArcAngleDiff(endangle2, slope4, &temp7);
		ArcAngleDiff(slope4, startang2, &temp8);
		//check whether the intersection point lies on the arc or not//
		//If both points are in arc then return true//
		if(((temp1 <= temp) && (temp2 <= temp)) && ((temp3 <= temp) && (temp4 <= temp)))
		{
			if(((temp5 <= tempc) && (temp6 <= tempc)) && ((temp7 <= tempc) && (temp8 <= tempc)))
			{
				*intersectnpt1  = point1[0];   
				*(intersectnpt1 + 1) = point1[1];
				*(intersectnpt1 + 2) = point1[2];  
				*(intersectnpt1 + 3) = point1[3];  
				*Count = 2;
				return 2;
			}
			else if((temp5 <= tempc) && (temp6 <= tempc))
			{
				*intersectnpt1  = point1[0];   
				*(intersectnpt1 + 1) = point1[1];
				*intersectnpt2  = point1[2];   
				*(intersectnpt2 + 1) = point1[3];
				*Count = 1;
				*(Count + 1) = 1;
				return 1;
			}
			else if((temp7 <= tempc) && (temp8 <= tempc))
			{
				*intersectnpt1  = point1[2];   
				*(intersectnpt1 + 1) = point1[3];
				*intersectnpt2  = point1[0];   
				*(intersectnpt2 + 1) = point1[1];
				*Count = 1;
				*(Count + 1) = 1;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else if((temp1 <= temp) && (temp2 <= temp) && (temp5 <= tempc) && (temp6 <= tempc))
		{
			*intersectnpt1  = point1[0];   
			*(intersectnpt1 + 1) = point1[1];
			*intersectnpt2  = point1[2];   
			*(intersectnpt2 + 1) = point1[3];
			*Count = 1;
			*(Count + 1) = 1;
			return 1;
			
		}
		else if((temp3 <= temp) && (temp4 <= temp) && (temp7 <= tempc) && (temp8 <= tempc))
		{
			*intersectnpt1  = point1[2];   
			*(intersectnpt1 + 1) = point1[3];
			*intersectnpt2  = point1[0];   
			*(intersectnpt2 + 1) = point1[1];
			*Count = 1;
			*(Count + 1) = 1;
			return 1;
		}
		else	
		{
			*intersectnpt2  = point1[0];   
			*(intersectnpt2 + 1) = point1[1];
			*(intersectnpt2 + 2)  = point1[2];   
			*(intersectnpt2 + 3) = point1[3];
			*(Count + 1) = 2;
			return 2;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0110", __FILE__, __FUNCSIG__); return 0; }
}

//Finite line to finite line intersection ,,,,/
int RapidMath2D::Finitelineto_Finiteline(double *point1, double *point2, double *point3, double *point4, double *point, double *points)
{
	try
	{
		double dl1 = Pt2Pt_distance(point1, point2);
		double dl2 = Pt2Pt_distance(point3, point4);
		double d1 = Pt2Pt_distance(point, point1);
		double d2 = Pt2Pt_distance(point, point2);
		double d3 = Pt2Pt_distance(point, point3);
		double d4 = Pt2Pt_distance(point, point4);
		if(d1 < dl1 && d2 < dl1)
		{
			if(d3 < d4)
			{
				*points = *point3;
				*(points + 1) = *(point3 + 1);
			}
			else
			{
				*points = *point4;
				*(points + 1) = *(point4 + 1);
			}
			return 1;
		}
		else if(d3 < dl2 && d4 < dl2)
		{
			if(d1 < d2)
			{
				*points = *point1;
				*(points + 1) = *(point1 + 1);
			}
			else
			{
				*points = *point2;
				*(points + 1) = *(point2 + 1);
			}
			return 1;
		}
		else
		{
			if(d1 < d2)
			{
				*points = *point1;
				*(points + 1) = *(point1 + 1);
			}
			else
			{
				*points = *point2;
				*(points + 1) = *(point2 + 1);
			}
			if(d3 < d4)
			{
				*(points + 2) = *point3;
				*(points + 3) = *(point3 + 1);
			}
			else
			{
				*(points + 2) = *point4;
				*(points + 3) = *(point4 + 1);
			}
			return 2;
		}
		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0173", __FILE__, __FUNCSIG__); return false; }
}

//Arc to arc intersection...//
int RapidMath2D::Arcto_Arc(double *center1, double radius1, double *center2, double radius2, double *endpoints1, double *endpoints2, double *point, double *points)
{	
	try
	{
		double slope1, slope2, slope3;
		double endangle1, endangle2, temp, tempc, temp1, temp2, temp3, temp4;
		//calculate the angle of line joinig intersection points with the center1//
		slope1 = ray_angle(center1, point);
		slope2 = ray_angle(center2, point);
		endangle1 = *endpoints1 + *(endpoints1 + 1);
		endangle2 = *endpoints2 + *(endpoints2 + 1);
		if(endangle1 > 2 * M_PI)
			endangle1 -= 2 * M_PI;
		if(endangle2 > 2 * M_PI)
			endangle2 -= 2 * M_PI;

		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle1, *endpoints1, &temp);
		ArcAngleDiff(endangle1, *endpoints2, &tempc);
	
		//temp1 to temp4 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle1, slope1, &temp1);
		ArcAngleDiff(slope1, *endpoints1, &temp2);
		ArcAngleDiff(endangle2, slope2, &temp3);
		ArcAngleDiff(slope2, *endpoints2, &temp4);
		
		//check whether the intersection point lies on the arc or not//
		//If both points are in arc then return true//
		if((temp1 <= temp) && (temp2 <= temp))
		{
			*points = *center2;
			*(points  + 1) = *(center2 + 1);
			*(points  + 2) = radius2;
			if(temp4 < temp3)
			{
				*(points  + 3) = endangle2;
				slope3 = slope2 - endangle2;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 4) = slope3;
			}
			else
			{
				*(points  + 3) = slope2;
				slope3 = *endpoints2  - slope2;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 4) = slope3;
			}
			return 1;
		}
		else if((temp3 <= tempc) && (temp4 <= tempc))
		{
			*points = *center1;
			*(points  + 1) = *(center1 + 1);
			*(points  + 2) = radius1;
			if(temp2 < temp1)
			{
				*(points  + 3) = endangle1;
				slope3 = slope1 - endangle1;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 4) = slope3;
			}
			else
			{
				*(points  + 3) = slope1;
				slope3 = *endpoints1  - slope1;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 4) = slope3;
			}
			return 1;
		}
		else 
		{
			if(temp2 < temp1)
			{
				*(points) = endangle1;
				slope3 = slope1 - endangle1;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 1) = slope3;
			}
			else
			{
				*(points) = slope1;
				slope3 = *endpoints1  - slope1;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 1) = slope3;
			}
			
			if(temp4 < temp3)
			{
				*(points  + 2) = endangle2;
				slope3 = slope2 - endangle2;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 3) = slope3;
			}
			else
			{
				*(points  + 2) = slope2;
				slope3 = *endpoints2  - slope2;
				if(slope3 < 0)
					slope3 += 2 * M_PI;
				*(points  + 3) = slope3;
			}
			return 2;
		}
		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0174", __FILE__, __FUNCSIG__); return false; }
}

//Ray to ray intersection...//
int RapidMath2D::rayto_ray(double *pt1, double slope1, double *pt2, double slope2, double *point, double *points)
{
	try
	{
		double slope3,slope4;
		slope3 = ray_angle(pt1, point);
		slope4 = ray_angle(pt2, point);
		if(IsEqual(slope1, slope3, MeasureDecValue))
		{
			*points = *pt2;
			*(points + 1) = *(pt2 + 1);
			return 1;
		}
		else if(IsEqual(slope2, slope4, MeasureDecValue))
		{
			*points = *pt1;
			*(points + 1) = *(pt1 + 1);
			return 1;
		}
		else
		{
			*points = *pt2;
			*(points + 1) = *(pt2 + 1);
			*(points + 2) = *pt1;
			*(points + 3) = *(pt1 + 1);
			return 2;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0175", __FILE__, __FUNCSIG__); return false; }
}

//Line to finite line intersection////
int RapidMath2D::Lineto_finiteline(double *pt1, double *pt2, double *point, double *points)
{
	try
	{
		double d1 = Pt2Pt_distance(pt1, point);
		double d2 = Pt2Pt_distance(pt2, point);
		if(d1 < d2)
		{
			points[0] = pt1[0];
			points[1] = pt1[1];
		}
		else
		{
			points[0] = pt2[0];
			points[1] = pt2[1];
		}
		return 1;

	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0176", __FILE__, __FUNCSIG__); return false; }
}

//Finite line to ray intersection...//
int RapidMath2D::Finitelineto_ray(double *pt1, double *pt2, double *pt, double slope, double *point, double *points)
{
	try
	{
		double slope1;
		double d1 = Pt2Pt_distance(pt1, point);
		double d2 = Pt2Pt_distance(pt2, point);
		double d = Pt2Pt_distance(pt2, pt1);
		slope1= ray_angle(pt, point);
		if(IsEqual(slope, slope1, MeasureDecValue))
		{
			if(d1 < d2)
			{
				*points = *pt1;
				*(points + 1) = *(pt1 + 1);
			}
			else
			{
				*points = *pt2;
				*(points + 1) = *(pt2 + 1);
			}
			return 1;
		}
		else
		{
			if(d1 < d && d2 < d)
			{
				*points = *pt;
				*(points + 1) = *(pt + 1);
				return 1;
			}
			else
			{
				if(d1 < d2)
				{
					*points = *pt1;
					*(points + 1) = *(pt1 + 1);
				}
				else
				{
					*points = *pt2;
					*(points + 1) = *(pt2 + 1);
				}
				return 2;
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0177", __FILE__, __FUNCSIG__); return false; }
}

//Finite line to circle intersection...//
int RapidMath2D::Finitelineto_Circle(double *pt1, double *pt2, double *center, double radius, double *point, double *points)
{
	try
	{
		double d1 = Pt2Pt_distance(pt1, point);
		double d2 = Pt2Pt_distance(pt2, point);
		if(d1 < d2)
		{
			points[0] = pt1[0];
			points[1] = pt1[1];
		}
		else
		{
			points[0] = pt2[0];
			points[1] = pt2[1];
		}
		return 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0178", __FILE__, __FUNCSIG__); return false; }
}

//Finite line to arc intersection point calciulation..
int RapidMath2D::Finitelineto_arc(double *pt1, double *pt2, double *center, double radius, double *angles, double *point, double *points)
{
	try
	{
		double endangle, slope1,temp, temp1, temp2, slope2;
		double d = Pt2Pt_distance(pt1, pt2);
		double d1 = Pt2Pt_distance(pt1, point);
		double d2 = Pt2Pt_distance(pt2, point);
		endangle  = *angles + *(angles + 1);
		//calculate the angle of line joinig intersection points with the center1//
		slope1 = ray_angle(center, point);
		if(endangle > 2 * M_PI)
		{
			endangle -= 2 * M_PI;
		}
		
		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, *angles, &temp);
		//temp1 to temp2 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, *angles, &temp2);
		//check whether the intersection point lies on the arc or not//
		//If both points are in arc then return true//
		if((temp1 <= temp) && (temp2 <= temp))
		{
			if(d1 < d2)
			{
				points[0] = pt1[0];
				points[1] = pt1[1];
			}
			else
			{
				points[0] = pt2[0];
				points[1] = pt2[1];
			}
			return 1;
		}
		else 
		{
			if(d1 < d2)
			{
				points[0] = pt1[0];
				points[1] = pt1[1];
			}
			else
			{
				points[0] = pt2[0];
				points[1] = pt2[1];
			}
			if(temp2 < temp1)
			{
				*(points + 2) = endangle;
				slope2 = slope1 - endangle;
				if(slope2 < 0)
					slope2 += 2 * M_PI;
				*(points + 3) = slope2;
			}
			else
			{
				*(points + 2) = slope1;
				slope2 = *angles  - slope1;
				if(slope2 < 0)
					slope2 += 2 * M_PI;
				*(points + 3) = slope2;
			}
			return 2;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0179", __FILE__, __FUNCSIG__); return false; }
}

//Ray yo arc interstion point..//
int RapidMath2D::Rayto_Arc(double *pt, double slope, double *center, double radius, double *angles, double *point, double *points)
{
	try
	{
		double endangle,slope1, slope2, temp,temp1,temp2;
		
		endangle  = *angles + *(angles + 1);
		//calculate the angle of line joinig intersection points with the center1//
		slope1 = ray_angle(center, point);
		if(endangle > 2 * M_PI)
		{
			endangle -= 2 * M_PI;
		}

		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, *angles, &temp);
		//temp1 to temp2 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, *angles, &temp2);
		//check whether the intersection point lies on the arc or not//
		//If both points are in arc then return true//
		if((temp1 <= temp) && (temp2 <= temp))
		{
			*points = *pt;
			*(points + 1) = *(pt + 1);
			return 1;
		}
		else 
		{
			*points = *pt;
			*(points + 1) = *(pt + 1);
			if(temp2 < temp1)
			{
				*(points + 2) = endangle;
				slope2 = slope1 - endangle;
				if(slope2 < 0)
					slope2 += 2 * M_PI;
				*(points + 3) = slope2;
			}
			else
			{
				*(points + 2) = slope1;
				slope2 = *angles  - slope1;
				if(slope2 < 0)
					slope2 += 2 * M_PI;
				*(points + 3) = slope2;
			}
			return 2;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0180", __FILE__, __FUNCSIG__); return false; }
}

//Infinite line to arc intersection..//
int RapidMath2D::Infinitelineto_Arc(double *center, double radius, double *angles, double *point, double *points)
{
	try
	{
		double endangle,slope1, slope2, temp,temp1,temp2;

		endangle  = *angles + *(angles + 1);
		//calculate the angle of line joinig intersection points with the center1//
		slope1 = ray_angle(center, point);
		if(endangle > 2 * M_PI)
		{
			endangle -= 2 * M_PI;
		}

		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, *angles, &temp);
		//temp1 to temp2 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, *angles, &temp2);
		//check whether the intersection point lies on the arc or not//
		//If both points are in arc then return true//
		if(temp2 < temp1)
		{
			*(points) = endangle;
			slope2 = slope1 - endangle;
			if(slope2 < 0)
				slope2 += 2 * M_PI; 
			*(points  + 1) = slope2;
		}
		else
		{
			*(points) = slope1;
			slope2 = *angles  - slope1;
			if(slope2 < 0)
				slope2 += 2 * M_PI; 
			*(points  + 1) = slope2;
		}
		return 1;

	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0181", __FILE__, __FUNCSIG__); return false; }
}

//Circle to arc intersection..//
int RapidMath2D::Circleto_arc(double *center, double radius, double *angles, double *point, double *points)
{
	try
	{
		double endangle,slope1,slope2,temp,temp1,temp2;

		endangle  = *angles + *(angles + 1);
		//calculate the angle of line joinig intersection points with the center1//
		slope1 = ray_angle(center, point);
		if(endangle > 2 * M_PI)
		{
			endangle -= 2 * M_PI;
		}

		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, *angles, &temp);
		//temp1 to temp2 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, *angles, &temp2);
		//check whether the intersection point lies on the arc or not//
		//If both points are in arc then return true//
		if(temp2 < temp1)
		{
			*(points) = endangle;
			slope2 = slope1 - endangle;
			if(slope2 < 0)
				slope2 += 2 * M_PI;
			*(points  + 1) = slope2;
		}
		else
		{
			*(points) = slope1;
			slope2 = *angles  - slope1;
			if(slope2 < 0)
				slope2 += 2 * M_PI;
			*(points  + 1) = slope2;
		}
		return 1;

	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0182", __FILE__, __FUNCSIG__); return false; }
}

//mouse point on finite line
bool RapidMath2D::MousePt_OnFiniteline(double *endpt1, double *endpt2, double slope, double intercept, double *point, double sp, bool *ShapeforImplicit)
{
	try
	{
		if(MousePt_OnInfiniteline(slope, intercept, point, sp))
		{
			/*if(PtisinwindowRounded(point[0], point[1], endpt1, endpt2))
			{
				*ShapeforImplicit = false;
				return true;
			}
			else
			{
				*ShapeforImplicit = true;
				return false;
			}*/
			if(slope < 0)
				slope += M_PI;
			if(slope < M_PI_4 || slope > 3 * M_PI_4)
			{
				if(((*endpt1 < *point + sp) && (*endpt2 > *point - sp)) || ((*endpt1 > *point - sp) && (*endpt2 < *point + sp)))
				{
					*ShapeforImplicit = false;
					return true;
				}
				else
				{
					*ShapeforImplicit = true;
					return false;
				}
			}
			else if(((*(endpt1 + 1) < *(point + 1) + sp) && (*(endpt2 + 1) > *(point + 1) - sp)) || ((*(endpt1 + 1) > *(point + 1) - sp) && (*(endpt2 + 1) < *(point + 1) + sp)))
			{
				*ShapeforImplicit = false;
				return true;
			}
			else
			{
				*ShapeforImplicit = true;
				return false;
			}
		}
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0067", __FILE__, __FUNCSIG__); return false; }
}

//function to find the nearest point on ray//
bool RapidMath2D::MousePt_OnRay(double *pt, double slope, double intercept, double *point, double sp)
{
	try
	{
		double nearestpt[2] = {0};
		Angle_FourQuad(&slope);
		if(MousePt_OnInfiniteline(slope, intercept, point, sp))
		{
			Point_PerpenIntrsctn_Line(slope, intercept, point, &nearestpt[0]);
			double sl = ray_angle(pt, &nearestpt[0]);
			Angle_FourQuad(&sl);
			if(IsEqual(sl, slope, MeasureDecValue))
				return true;
		}
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0069", __FILE__, __FUNCSIG__); return false; }
}

//function to find the nearest point on line
bool RapidMath2D::MousePt_OnInfiniteline(double slope, double intercept, double *point, double sp)
{
	try
	{
		double d = Pt2Line_Dist(point[0], point[1], slope, intercept);
		if(d < sp)
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0068", __FILE__, __FUNCSIG__); return false; }
}

//Function to calcualte the mouse distance to circle..//
bool RapidMath2D::MousePt_OnCircle(double *center, double radius, double *point, double sp)
{
	try
	{
		double d = Pt2Pt_distance(center, point);
		if(abs(d - radius) < sp)
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0070", __FILE__, __FUNCSIG__); return false; }
}

//function to find the nearest point on arc//
bool RapidMath2D::MousePt_OnArc(double *center, double radius, double startang, double sweepang, double *endpt1, double *endpt2, double sp, double *Mousepoint, bool *ShapeforImplicit)
{
	try
	{
		if(MousePt_OnCircle(center, radius, Mousepoint, sp))
		{
			double slope = ray_angle(center, Mousepoint);
			double endangle = startang + sweepang;
			if((endangle > 2 * M_PI) && (slope < startang))
				slope += 2 * M_PI;
			//calculate endpoints of the arc using start angle and end angle//
			if((slope >= startang - (sp / radius)) && (slope <= endangle + (sp / radius)))
			{
				*ShapeforImplicit = false;
				return true;
			}
			else
			{
				*ShapeforImplicit = true;
				return false;
			}
		}
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0065", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::PtisinTriangle(double *point, double *Vertex1, double *Vertex2, double *Vertex3)
{
	try
	{
		double TotalArea = AreaofTriangle(Vertex1, Vertex2, Vertex3);
		double Area1 = AreaofTriangle(point, Vertex2, Vertex3);
		double Area2 = AreaofTriangle(Vertex1, point, Vertex3);
		double Area3 = AreaofTriangle(Vertex1, Vertex2, point);

		if(abs(TotalArea - (Area1 + Area2 + Area3)) < MeasureDecValue)
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0117a", __FILE__, __FUNCSIG__); return false; }
}

//Point is in window..
bool RapidMath2D::Ptisinwindow(double x, double y, double minx, double miny, double maxx, double maxy)
{
	try
	{
		if(x >= minx && x <= maxx && y >= miny && y <= maxy)
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0117", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::PtisinCube(double x, double y, double z, double minx, double miny, double minz, double maxx, double maxy, double maxz)
{
	try
	{
		if((x >= minx && x <= maxx) && (y >= miny && y <= maxy) && (z >= minz && z <= maxz))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0117", __FILE__, __FUNCSIG__); return false; }
}

//Is in Window function.. Check point is in window..//
bool RapidMath2D::Ptisinwindow(double *point, double *corner1, double *corner2)
{
	try
	{
		double minx = min(corner1[0], corner2[0]);
		double miny = min(corner1[1], corner2[1]);
		double maxx = max(corner1[0], corner2[0]);
		double maxy = max(corner1[1], corner2[1]);
		if(point[0] >= minx && point[0] <= maxx && point[1] >= miny && point[1] <= maxy)
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0118", __FILE__, __FUNCSIG__); return false; }
}

//Is in Window function.. Check point is in window..//
bool RapidMath2D::PtisinwindowRounded(double x, double y, double *corner1, double *corner2)
{
	try
	{
		double minx = min(corner1[0], corner2[0]);
		double miny = min(corner1[1], corner2[1]);
		double maxx = max(corner1[0], corner2[0]);
		double maxy = max(corner1[1], corner2[1]);
		if(IsGreaterOrEqual(x, minx, MeasureDecValue) && IsGreaterOrEqual(maxx, x, MeasureDecValue) && IsGreaterOrEqual(y, miny, MeasureDecValue) && IsGreaterOrEqual(maxy, y, MeasureDecValue))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0119", __FILE__, __FUNCSIG__); return false; }	
}

//Circle is in window function..../
bool RapidMath2D::Circle_isinwindow(double *center, double radius, double *point1, double *point2, double *point3, double *point4)
{
	try
	{
		if(Ptisinwindow(*center, *(center + 1), *point1, *(point3 + 1), *point3, *(point1 + 1)) ||
			Finiteline_circle1(point1,point2,0,*(point1 + 1),center,radius)|| 
				Finiteline_circle1(point2,point3,M_PI_2,*point2,center,radius)||
					Finiteline_circle1(point3,point4,0,*(point3 + 1),center,radius)|| 
						Finiteline_circle1(point4,point1,M_PI_2,*point4,center,radius))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0120", __FILE__, __FUNCSIG__); return false; }
}

//Function to check infinte line is inwindow..//
bool RapidMath2D::Inline_isinwindow(double slope, double intercept, double *point1, double *point2, double *point3, double *point4)
{
	try
	{
		if(Line_finiteline1(slope,intercept,point1,point2, 0, (*point1 + 1))||
				Line_finiteline1(slope,intercept,point2,point3, M_PI_2, *point2)||
					Line_finiteline1(slope,intercept,point3,point4, 0, *(point3 + 1))||
						Line_finiteline1(slope,intercept,point4,point1, M_PI_2, *point4))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0121", __FILE__, __FUNCSIG__); return false; }
}

//function to check the ray is in window or not//
bool RapidMath2D::ray_isinwindow(double *pt, double slope, double intercept, double *point1, double *point2, double *point3, double *point4)
{
	try
	{
		if(Ptisinwindow(*pt, *(pt + 1), *point1, *(point3 + 1), *point3, *(point1 + 1))||
			ray_finitelineint1(pt,slope,intercept,point1,point2,0,*(point1 + 1))||
				ray_finitelineint1(pt,slope,intercept,point2,point3,M_PI_2,*point2)||
					ray_finitelineint1(pt,slope,intercept,point3,point4,0,*(point3 + 1))||
						ray_finitelineint1(pt,slope,intercept,point4,point1,M_PI_2,*point4))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0122", __FILE__, __FUNCSIG__); return false; }
}

//function to check whether the f line is in window or not//
bool RapidMath2D::Fline_isinwindow(double *endp1, double *endp2, double slope, double intercept, double *point1, double *point2, double *point3, double *point4)
{
	try
	{
		//check whether the line is intersecting to window//
		if(Ptisinwindow(*endp1, *(endp1 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1)) ||
			Ptisinwindow(*endp2, *(endp2 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1))||
			Finiteline_line1(endp1,endp2,slope, intercept, point1,point2, 0, *(point1 + 1))|| 
				Finiteline_line1(endp1,endp2,slope, intercept,point2,point3, M_PI_2, *point2)|| 
					Finiteline_line1(endp1,endp2,slope, intercept,point3,point4, 0, *(point3 + 1))|| 
						Finiteline_line1(endp1,endp2,slope, intercept,point4,point1, M_PI_2, *point4))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0123", __FILE__, __FUNCSIG__); return false; }
}

//function to check whether the given is in window or not
bool RapidMath2D::arc_isinwindow(double *center, double radius, double startang, double sweepang, double *endp1, double *endp2, double *point1, double *point2, double *point3, double *point4)
{
	try
	{
		//check whether the line is intersectin to window//
		if(Ptisinwindow(*endp1, *(endp1 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1)) ||
			Ptisinwindow(*endp2, *(endp2 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1)) ||
				Finiteline_arcinter1(center,radius,startang,sweepang,point1,point2, 0, *(point1 + 1))|| 
					Finiteline_arcinter1(center,radius,startang,sweepang,point2,point3, M_PI_2, *point2)||
						Finiteline_arcinter1(center,radius,startang,sweepang,point3,point4, 0, *(point3 + 1))|| 
							Finiteline_arcinter1(center,radius,startang,sweepang,point4,point1, M_PI_2, *point4))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0124", __FILE__, __FUNCSIG__); return false; }
}

//function to check the given rectangle is in window or not//
bool RapidMath2D::Rect_isinwindow(double *p1, double *p2, double *p3, double *p4, double *point1, double *point2, double *point3, double *point4)
{
	try
	{
		if(Ptisinwindow(*p1, *(p1 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1))||
				Ptisinwindow(*p2, *(p2 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1))||
					Ptisinwindow(*p3, *(p3 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1))||
						Ptisinwindow(*p4, *(p4 + 1), *point1, *(point3 + 1), *point3, *(point1 + 1)))
			return true;
		if(Ptisinwindow(*point1, *(point1 + 1), *p1, *(p3 + 1), *p3, *(p1 + 1))||
				Ptisinwindow(*point2, *(point2 + 1), *p1, *(p3 + 1), *p3, *(p1 + 1))||
					Ptisinwindow(*point3, *(point3 + 1), *p1, *(p3 + 1), *p3, *(p1 + 1))||
						Ptisinwindow(*point4, *(point4 + 1), *p1, *(p3 + 1), *p3, *(p1 + 1)))
			return true;
		if((*(p1 + 1) <= *(point1 + 1) && *(p3 + 1) >= *(point3 + 1)) && (*p1 <= *point1 && *p3 >= *point3) ||
			(*(point1 + 1) <= *(p1 + 1) && *(point3 + 1) >= *(p3 + 1)) && (*point1 <= *p1 && *point3 >= *p3))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0125", __FILE__, __FUNCSIG__); return false; }
}

//Point is in Rectangle in any Orientation..
bool RapidMath2D::PtisinRectangle(double *point1, double *point2, double *point3, double *point4, double *mousept)
{
	try
	{
		double slope1, slope2, slope3, slope4;
		double intercept1, intercept2, intercept3, intercept4;
		TwoPointLine(point1, point2, &slope1, &intercept1);
		TwoPointLine(point2, point3, &slope2, &intercept2);
		TwoPointLine(point3, point4, &slope3, &intercept3);
		TwoPointLine(point4, point1, &slope4, &intercept4);
		double d1 = Pt2Line_DistSigned(mousept[0], mousept[1], slope1, intercept1);
		double d2 = Pt2Line_DistSigned(mousept[0], mousept[1], slope2, intercept2);
		double d3 = Pt2Line_DistSigned(mousept[0], mousept[1], slope3, intercept3);
		double d4 = Pt2Line_DistSigned(mousept[0], mousept[1], slope4, intercept4);
		if(((d1 > 0 && d3 < 0) || (d1 < 0 && d3 > 0)) && ((d2 > 0 && d4 < 0) || (d2 < 0 && d4 > 0)))
			return true;
		else
			return false;
	}
	catch(...)
	{
		return false;
	}
}

//function to find the intersection point of finite line with the given  ray//
bool RapidMath2D::ray_finitelineint1(double *point, double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2)
{
	double point1[2],tempslope;
	try
	{
		if(Line_lineintersection(slope1, intercept1, slope2, intercept2, &point1[0]) == 0)
			return false;
		tempslope = ray_angle(point, &point1[0]);
		if(!IsEqual(slope1, tempslope, MeasureDecValue))
			return false;
		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0093", __FILE__, __FUNCSIG__); return false; }
}

//function to calculate the line to finite line intersection
bool RapidMath2D::Line_finiteline1(double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2 )
{
	try
	{
		double point1[2];
		if(Line_lineintersection(slope1,intercept1,slope2,intercept2,&point1[0]) == 0)
			return false;
		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0097", __FILE__, __FUNCSIG__); return false; }
}

//function to find the finite line to finite line intersection//
bool RapidMath2D::Finiteline_line1(double *endpt1, double *endpt2, double slope1, double intercept1, double *endpt3, double *endpt4, double slope2, double intercept2)
{
	try
	{
		double point1[2];
		if(Line_lineintersection(slope1,intercept1,slope2,intercept2,&point1[0]) == 0)
			return false;
		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2) && PtisinwindowRounded(point1[0], point1[1], endpt3, endpt4))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0101", __FILE__, __FUNCSIG__); return false; }
}

//function to find the intersection between the finite line and circle//
bool RapidMath2D::Finiteline_circle1(double *endpt1, double *endpt2, double slope, double intercept, double *center, double radius)
{
	try
	{
		double point1[4];
		//calculate line to circle intersection//
		if(Line_circleintersection(slope, intercept, center, radius, &point1[0]) == 0)
			return false;
		//Check whether the intersection point lies on the line or not//
		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2) && PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0104", __FILE__, __FUNCSIG__); return false; }
}

//function to calculate the finite line to circle intersection//
bool RapidMath2D::Finiteline_arcinter1(double *center, double radius, double startang, double sweepang, double *endpt1, double *endpt2, double slope, double intercept)
{
	try
	{
		double point1[4], slope1, slope2;
		double endangle, temp, temp1, temp2, temp3, temp4;
		if(Line_circleintersection(slope,intercept,center,radius,&point1[0]) == 0)
			return false;
		slope1 = ray_angle(center, &point1[0]);
		slope2 = ray_angle(center, &point1[2]);
		endangle = startang + sweepang;
		if(startang > endangle)
		{
			temp = startang;
			startang  = endangle;
			endangle = temp;
		}
		if(endangle > 2*M_PI)
			endangle -= 2*M_PI;
		//angles to check the position of the points on the arc//
		ArcAngleDiff(endangle, startang, &temp);
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, startang, &temp2);
		ArcAngleDiff(endangle, slope2, &temp3);
		ArcAngleDiff(slope2, startang, &temp4);

		bool flag1 = IsGreaterOrEqual(temp, temp1, MeasureDecValue);
		bool flag2 = IsGreaterOrEqual(temp, temp2, MeasureDecValue);
		bool flag3 = IsGreaterOrEqual(temp, temp3, MeasureDecValue);
		bool flag4 = IsGreaterOrEqual(temp, temp4, MeasureDecValue);
		
		if(PtisinwindowRounded(point1[0], point1[1], endpt1, endpt2) && flag1 && flag2)
			return true;
		if(PtisinwindowRounded(point1[2], point1[3], endpt1, endpt2) && flag3 && flag4)
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0107", __FILE__, __FUNCSIG__); return false; }
}

//function to calculte the mean ,median, variance and stddev of no.s
void RapidMath2D::statistics(int n, double *values, double *mean, double *median, double *variance, double *stddev)
{
	try
	{
		double sum = 0, temp = 0; //sum of all values, temp to swap the values//
		for(int i = 0; i < n; i++)
			sum += values[i]; // sum of all numbers
		//mean(average) of the values//
		*mean = sum / n;
		//sorting the values in ascending order//
		for(int i = 0; i < n; i++)
		{
			for(int j = i + 1; j < n; j++)
			{
				if(*(values + i) > *(values + j))
				{
					 //swapping the values//
					temp  = values[i];  
					values[i] = values[j];
					values[j] = temp;
				}
			}
		}
		if(n % 2 != 0)  //for odd number of inputs median is the middle element in the array//
			*median = values[(n - 1) / 2];
		else  //even number of inputs median is the average of two middle elements in the array//
			*median = (values[n / 2 - 1] + values[n / 2]) / 2;
		temp = 0;
		//Calculte sum of the squares of differences in each value and median///
		for(int i = 0; i < n; i++)
		{
			temp += pow((values[i] - *median), 2);
		}
		//average of above is the variance// 
		*variance  = temp / n;
		//square root of variance  > standard deviation//
		*stddev  = sqrt(*variance);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0085", __FILE__, __FUNCSIG__); }
}

//Function to calculte the Sxx or Syy///
void RapidMath2D::Sxxvalue(int n, double *values, double *Sxx)
{
	try
	{
		double sum = 0, avg; // sum of all numbers, average of numbers//
		// sum of all numbers//
		for(int i = 0; i < n; i++)
			sum += values[i];
		//average(mean) of all the numbers//
		avg = sum / n;
		sum = 0;
		//Calculte sum of the squares of differences in each value and average//
		for(int i = 0; i < n; i++)
			sum += pow((values[i] - avg), 2);
		*Sxx = sum;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0086", __FILE__, __FUNCSIG__); }
}

//function to calculate Sxy//
void RapidMath2D::Sxyvalue(int n, double *valuesX, double *valuesY, double *Sxy)
{
	try
	{
		double sum = 0, avg1, avg2; //sum of all numbers//
		//sum of all numbers of first array //
		for(int i = 0; i < n; i++)
			sum += valuesX[i];
		//average of first array//
		avg1 = sum / n;
		sum = 0;
		//sum of all numbers of the second array//
		for(int i = 0; i < n; i++)
			sum += valuesY[i];
		//average of second array//
		avg2 = sum / n;
		sum = 0;
		//Calculte sum of the product of differences in each value and average of first and second array resp.//
		for(int i = 0; i < n; i ++)
		{
			sum += (valuesX[i] - avg1) * (valuesY[i] - avg2);
		}
		*Sxy  = sum;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0087", __FILE__, __FUNCSIG__); }
}

//function to find the angle of a ray : given two points//
double RapidMath2D::ray_angle(double x1, double y1, double x2, double y2)
{
	try
	{
		double point1[2] = {x1, y1}; double point2[2] = {x2, y2};
		double angle = 0;
		angle = ray_angle(&point1[0], &point2[0]);
		return angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0089", __FILE__, __FUNCSIG__); return 0; }
}

//function to find the angle of a ray : given two points//
double RapidMath2D::ray_angle(double *point1, double *point2)
{
	try
	{
		double angle = 0;
		//If both points are same then return false//
		if(IsEqual(point1[0], point2[0], MeasureDecValue) && IsEqual(point1[1], point2[1], MeasureDecValue))
			return 0;
		if(IsEqual(point1[0], point2[0], MeasureDecValue))
		{
			if(point1[1] < point2[1])
				angle = M_PI_2;
			else
				angle = 3 * M_PI_2;
			return angle;
		}
		else if(IsEqual(point1[1], point2[1], MeasureDecValue))
		{
			if(point1[0] < point2[0])
				angle = 0;
			else
				angle = M_PI;
			return angle;
		}
		//calculte the slope of the ray from the given two points//
		angle  = atan((point1[1]  - point2[1]) / (point1[0] - point2[0])); 
		//check the position of points: point2 with respect to point1(start point of the ray)//
		//depending on the position of second point calculte the angle of the ray i between 0 to 360//
		if(point1[0] < point2[0])
		{
			if(point1[1] > point2[1])
				angle =  2 * M_PI - abs(angle);
		}
		else
			angle  = angle + M_PI;
		return angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0090", __FILE__, __FUNCSIG__); return 0; }
}

//Area of polygon.. Perimeter..
double RapidMath2D::AreaOfPolygon(double *ShapeParam, bool *Shapetype, int totalnoofshapes)
{
	try
	{
		//In worst case there is one problem... minimal set of shapes. lines/arc..needs to be handled..
		int index = 0;
		double TotalArea = 0;
		double *shapeParameters = (double*)calloc(totalnoofshapes  * 9, sizeof(double));
		bool *Pshapetype = (bool*)calloc(totalnoofshapes, sizeof(bool));

		//Swap the end points If required...
		SwapEndpointsForArea(ShapeParam, totalnoofshapes);
		//Get the point with maximum y..
		double maxy = ShapeParam[1];
		for(int i = 0; i < totalnoofshapes; i++)
		{
			if(maxy < ShapeParam[9 * i + 1])
			{
				maxy = ShapeParam[9 * i + 1];
				index = i;
			}
		}

		//Arrange the shape parameters in order... clock wise..!!
		int tempindex = index;
		if(index == totalnoofshapes - 1) 
		{
			if(ShapeParam[9 * index] < ShapeParam[0])
			{
				//n, 0, 1, ... n-1;
				for(int i = 0; i < totalnoofshapes; i++)
				{
					for(int j = 0; j < 9; j++)
						shapeParameters[9 * i + j] = ShapeParam[9 * tempindex + j];
					Pshapetype[i] = Shapetype[tempindex];
					tempindex++;
					if(tempindex > totalnoofshapes - 1)
						tempindex = 0;
				}
			}
			else
			{
				//n-1... to 0, n;
				for(int i = 0; i < totalnoofshapes; i++)
				{
					tempindex--;
					if(tempindex < 0) 
						tempindex = totalnoofshapes - 1;
					for(int j = 0; j < 9; j++)
						shapeParameters[9 * i + j] = ShapeParam[9 * tempindex + j];
					Pshapetype[i] = Shapetype[tempindex];
				}
			}

		}
		else if(index == 0)
		{
			if(ShapeParam[0] > ShapeParam[9])
			{
				//n, n-1... to 0;
				tempindex = totalnoofshapes - 1;
				for(int i = 0; i < totalnoofshapes; i++)
				{
					for(int j = 0; j < 9; j++)
						shapeParameters[9 * i + j] = ShapeParam[9 * tempindex + j];
					Pshapetype[i] = Shapetype[tempindex];
					tempindex--;
				}
			}
			else
			{
				for(int i = 0; i < totalnoofshapes; i++)
				{
					for(int j = 0; j < 9; j++)
						shapeParameters[9 * i + j] = ShapeParam[9 * i + j];
					Pshapetype[i] = Shapetype[i];
				}
			}
		}
		else
		{
			//
			if(ShapeParam[9 * index] < ShapeParam[9 * (index + 1)])
			{
				//index, index+1, ..n, 0, 1, ... index-1;
				for(int i = 0; i < totalnoofshapes; i++)
				{
					for(int j = 0; j < 9; j++)
						shapeParameters[9 * i + j] = ShapeParam[9 * tempindex + j];
					Pshapetype[i] = Shapetype[tempindex];
					tempindex++;
					if(tempindex > totalnoofshapes - 1)
						tempindex = 0;
				}
			}
			else
			{
				int tempindex1 = tempindex;
				// index, index-1 .. 0, n, n-1 .. index-1;
				for(int i = 0; i < totalnoofshapes; i++)
				{
					
					for(int j = 0; j < 9; j++)
						shapeParameters[9 * i + j] = ShapeParam[9 * tempindex1 + j];
					Pshapetype[i] = Shapetype[tempindex1];
					tempindex--;
					tempindex1 = tempindex;
					if(tempindex < 0) 
						tempindex1 += totalnoofshapes;
				}
			}
		}

		// Area calculations....
		if(totalnoofshapes == 2)
		{
			//Possible cases: 
			//1: 1 line and arc can form a closed loop or
			//2: 2 arc's can form a closed loop
			for(int i = 0; i < totalnoofshapes; i++)
			{
				if(!Pshapetype[i])
					TotalArea += AreaofPartofCircle(shapeParameters + 9 * i);
			}
		}
		else
		{
			//0, 1, ... n;
			for(int i = 0; i < totalnoofshapes - 2; i++)
			{
				//Angles... 0 to 360.. decides the add/subtract..
				double angle1 = ray_angle(shapeParameters[0], shapeParameters[1], shapeParameters[9 * (i + 1)], shapeParameters[9 * (i + 1)  + 1]);
				double angle2 = ray_angle(shapeParameters[0], shapeParameters[1], shapeParameters[9 * (i + 2)], shapeParameters[9 * (i + 2)  + 1]);
				if(abs(angle1 - angle2) > M_PI) // May occur in some cases... when the point is in 1st and 4th quadrant..!
				{
					if(angle1 < angle2)
						angle1 += 2 * M_PI;
					else
						angle2 += 2 * M_PI;
				}
				//Triangle End points..
				double point1[2] = {shapeParameters[0], shapeParameters[1]};
				double point2[2] = {shapeParameters[9 * (i + 1)], shapeParameters[9 * (i + 1) + 1]};
				double point3[2] = {shapeParameters[9 * (i + 2)], shapeParameters[9 * (i + 2) + 1]};
				//Calculate the area of the triangle...
				if((angle1 > angle2) || IsEqual(angle1, angle2, MeasureDecValue))				
					TotalArea += AreaofTriangle(&point1[0], &point2[0], &point3[0]);
				else
					TotalArea -= AreaofTriangle(&point1[0], &point2[0], &point3[0]);
				if(!Pshapetype[i])	// If the shape is arc then add/subtract the area of arc portion..
					TotalArea += AreaofPartofCircle(shapeParameters + 9 * i);
			}
		}
		free(Pshapetype);
		free(shapeParameters);
		return abs(TotalArea);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0201", __FILE__, __FUNCSIG__); return -1; }
}

//Area of triangle..
double RapidMath2D::AreaofTriangle(double *point1, double *point2, double *point3)
{
	try
	{
		double base, height, intercept, slope, AreaofT;
		//Calculating base of the triangle...
		base = Pt2Pt_distance(point1[0], point1[1], point2[0], point2[1]);
		//Base line..
		TwoPointLine(point1, point2, &slope, &intercept);
		// Third point to base line distance..
		height = Pt2Line_Dist(point3[0], point3[1], slope, intercept);
		//Area of the triangle..
		AreaofT = base * height / 2;
		return AreaofT;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0202", __FILE__, __FUNCSIG__); return -1; }
}

//Area of arc sector..
double RapidMath2D::AreaofSector(double angle, double radius)
{
	try
	{
		//Area of sector ... (r*r * theta/2)..
		double AreaS = angle * radius * radius / 2;
		return AreaS;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0203", __FILE__, __FUNCSIG__); return -1; }
}

//Area of part of the circle..
double RapidMath2D::AreaofPartofCircle(double *ShapeParam)
{
	try
	{
		double AreaCPart = 0, Area_S = 0, Area_T = 0;
		//Angles... 0 to 360.. decides the add/subtract..
		double angle1 = ray_angle(ShapeParam[0], ShapeParam[1], ShapeParam[4], ShapeParam[5]);
		double angle2 = ray_angle(ShapeParam[0], ShapeParam[1], ShapeParam[2], ShapeParam[3]);
		if(abs(angle1 - angle2) > M_PI) //If the points are in 1st and 4th quadrant..
		{
			if(angle1 < angle2)
				angle1 += 2 * M_PI;
			else
				angle2 += 2 * M_PI;
		}
		//Area of the sector...
		Area_S = AreaofSector(ShapeParam[7], ShapeParam[8]);
		//Area of the triangle formed by the center and arc end points..
		Area_T = AreaofTriangle(&ShapeParam[0], &ShapeParam[2], &ShapeParam[4]);
		if(ShapeParam[7] > M_PI)
		{
			AreaCPart = Area_S + Area_T;
			if(angle1 <= angle2) // Subtract the area...		
				AreaCPart = -AreaCPart;
			else	// Add the area...
				AreaCPart = abs(AreaCPart);
		}
		else
		{
			AreaCPart = Area_S - Area_T;
			if(angle1 <= angle2) // Add the area...		
				AreaCPart = abs(AreaCPart);
			else	// Subtract the area...
				AreaCPart = -AreaCPart;
		}
		return AreaCPart;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0204", __FILE__, __FUNCSIG__); return -1; }
}

//Swap end points of shape for area..
void RapidMath2D::SwapEndpointsForArea(double *ShapeParam, int totalnoofshapes)
{
	try
	{
		for(int i = 0; i < totalnoofshapes - 1; i++)
		{
			double Shape1Point1[2] = {ShapeParam[9 * i], ShapeParam[9 * i + 1]};
			double Shape1Point2[2] = {ShapeParam[9 * i + 2], ShapeParam[9 * i + 3]};
			double Shape2Point1[2] = {ShapeParam[9 * (i + 1)], ShapeParam[9 * (i + 1) + 1]};
			double Shape2Point2[2] = {ShapeParam[9 * (i + 1) + 2], ShapeParam[9 * (i + 1) + 3]};
			if(!(IsEqual(Shape1Point2[0], Shape2Point1[0], MeasureDecValue) && IsEqual(Shape1Point2[1], Shape2Point1[1], MeasureDecValue))) // Swap the Second Shape Points...
			{
				ShapeParam[9 * (i + 1)] = Shape2Point2[0]; ShapeParam[9 * (i + 1) + 1] = Shape2Point2[1];
				ShapeParam[9 * (i + 1) + 2] = Shape2Point1[0]; ShapeParam[9 * (i + 1) + 3] = Shape2Point1[1];
			}
		}
		for(int i = 0; i < totalnoofshapes - 1; i++)
		{
			if(abs(ShapeParam[9 * i]) < 0.00001) ShapeParam[9 * i] = 0;
			if(abs(ShapeParam[9 * i + 1]) < 0.00001) ShapeParam[9 * i + 1] = 0;
			if(abs(ShapeParam[9 * i + 2]) < 0.00001) ShapeParam[9 * i + 2] = 0;
			if(abs(ShapeParam[9 * i + 3]) < 0.00001) ShapeParam[9 * i + 3] = 0;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0205", __FILE__, __FUNCSIG__); }
}

//Arc frame grab calculations..//
void RapidMath2D::ArcFrameGrabEndPts(double *center, double radius1, double radius2, double startang, double sweepang, double *endpt1, double *endpt2)
{
	try
	{		
		double endangle  = startang + sweepang; 
		if(endangle > 2*M_PI)
			endangle -= 2*M_PI;
		double cosstangle = cos(startang), sinstangle = sin(startang);
		double cosendangle = cos(endangle), sinendangle = sin(endangle);
		//end points at start angle//
		endpt1[0] = center[0] + radius1 * cosstangle;
		endpt1[1] = center[1] + radius1 * sinstangle;
		endpt1[2] = center[0] + radius2 * cosstangle;
		endpt1[3] = center[1] + radius2 * sinstangle;

		endpt2[0] = center[0] + radius1 * cosendangle;
		endpt2[1] = center[1] + radius1 * sinendangle;
		endpt2[2] = center[0] + radius2 * cosendangle;
		endpt2[3] = center[1] + radius2 * sinendangle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0131", __FILE__, __FUNCSIG__); }
}

//Arc frame grab end point calculations...//
void RapidMath2D::ArcEndPoints_Fill(double *point1, double *point2, double width, double *endpt1, double *endpt2)
{
	try
	{	
		double slope, wdth = width * 2;
		if(point1[0] == point2[0])
			slope = 0;
		else if(point1[1] == point2[1])
			slope = M_PI_2;
		else 
			slope = atan((point1[0] - point2[0])/(point1[1] - point2[1]));
		EquiDistPointsOnLine(slope, wdth, point1, endpt1, endpt1 + 2);
		EquiDistPointsOnLine(slope, wdth, point2, endpt2, endpt2 + 2);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0136", __FILE__, __FUNCSIG__); }
}

//Circle parameters for Framegrab..//
void RapidMath2D::CircleParameters_FG(double *point1, double *point2, int width,double *C_array)
{
	try
	{
		Mid_point(point1, point2, C_array);
		double rad = Pt2Pt_distance(point1, point2) / 2;
		C_array[2] = rad - width;
		C_array[3] = rad + width;
		C_array[4] = 0;
		C_array[5] = 2 * M_PI;		
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0190", __FILE__, __FUNCSIG__); }
}

// Arc parameters for frame grab..//
void RapidMath2D::ArcParameters_FG(double *point1, double *point2, double *mousept, int space, double *C_array)
{
	try
	{
		double startang, sweepang, rad;
		//calculate the center ,radius, startangle,sweepangle of the arc//
		Arc_3Pt(point1, point2, mousept, C_array, &rad, &startang, &sweepang);
		C_array[2] = int(rad)  - space; 
		C_array[3] = int(rad)  + space;
		C_array[4] = startang; 
		C_array[5] = startang + sweepang;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0192", __FILE__, __FUNCSIG__); }
}

//Rectangle framegrab points calculation(corner points..)..//
void RapidMath2D::RectangleFG(double *point1, double *point2, double sp, double wup, double *points , bool flag)
{
	try
	{
		double slope, slope1;
		double Wvalue1 = 25 * wup, Wvalue2 = 18 * wup;
		//slope and intercept of the line joining two points//
		slope = ray_angle(point1, point2);
		//TwoPointLine(point1,point2,&slope,&intercept);
		slope1 = slope + M_PI_2; // atan(-1/tan(slope));
		//end points of the lines//
		*points = *point1 + (sp) * cos(slope1);
		*(points + 1) = *(point1 + 1) + (sp) * sin(slope1);
		*(points + 2) = *point1 - (sp) * cos(slope1);
		*(points + 3) = *(point1 + 1) - (sp) * sin(slope1);
		*(points + 4) = *(point2) + (sp) * cos(slope1);
		*(points + 5) = *(point2 + 1) + (sp) * sin(slope1);
		*(points + 6) = *(point2) - (sp) * cos(slope1);
		*(points + 7) = *(point2 + 1) - (sp) * sin(slope1);
		if(!flag)
		{
			*(points + 8) = (*(points + 2) + *(points + 6))/2 + Wvalue1 * cos(slope);
			*(points + 9) = (*(points + 3) + *(points + 7))/2 + Wvalue1 * sin(slope);
			*(points + 10) = (*(points + 2) + *(points + 6))/2 - Wvalue1 * cos(slope);
			*(points + 11) = (*(points + 3) + *(points + 7))/2 - Wvalue1 * sin(slope);
			*(points + 12) = (*(points + 2) + *(points + 6))/2 - Wvalue2 * cos(slope1);
			*(points + 13) = (*(points + 3) + *(points + 7))/2 - Wvalue2 * sin(slope1);
			/*if((slope < M_PI_4 * 3 && slope >= 0) || (slope < 2 * M_PI && slope >  M_PI_4 * 7))
			{
				*(points + 8) = (*(points + 2) + *(points + 6))/2 + Wvalue1 * cos(slope);
				*(points + 9) = (*(points + 3) + *(points + 7))/2 + Wvalue1 * sin(slope);
				*(points + 10) = (*(points + 2) + *(points + 6))/2 - Wvalue1 * cos(slope);
				*(points + 11) = (*(points + 3) + *(points + 7))/2 - Wvalue1 * sin(slope);
				*(points + 12) = (*(points + 2) + *(points + 6))/2 - Wvalue2 * cos(slope1);
				*(points + 13) = (*(points + 3) + *(points + 7))/2 - Wvalue2 * sin(slope1);
			}
			else
			{
				*(points + 8) = (*points + *(points + 4))/2 + Wvalue1 * cos(slope);
				*(points + 9) = (*(points + 1) + *(points + 5))/2 + Wvalue1 * sin(slope);
				*(points + 10) = (*points + *(points + 4))/2 - Wvalue1 * cos(slope);
				*(points + 11) = (*(points + 1) + *(points + 5))/2 - Wvalue1 * sin(slope);
				*(points + 12) = (*points + *(points + 4))/2 + Wvalue2 * cos(slope1);
				*(points + 13) = (*(points + 1) + *(points + 5))/2 + Wvalue2 * sin(slope1);
			}*/
		}
		else
		{
			*(points + 8) = (*points + *(points + 4))/2 + Wvalue1 * cos(slope);
			*(points + 9) = (*(points + 1) + *(points + 5))/2 + Wvalue1 * sin(slope);
			*(points + 10) = (*points + *(points + 4))/2 - Wvalue1 * cos(slope);
			*(points + 11) = (*(points + 1) + *(points + 5))/2 - Wvalue1 * sin(slope);
			*(points + 12) = (*points + *(points + 4))/2 + Wvalue2 * cos(slope1);
			*(points + 13) = (*(points + 1) + *(points + 5))/2 + Wvalue2 * sin(slope1);
			/*if((slope < M_PI_4 * 3 && slope >= 0) || (slope < 2 * M_PI && slope >  M_PI_4 * 7))
			{
				*(points + 8) = (*points + *(points + 4))/2 + Wvalue1 * cos(slope);
				*(points + 9) = (*(points + 1) + *(points + 5))/2 + Wvalue1 * sin(slope);
				*(points + 10) = (*points + *(points + 4))/2 - Wvalue1 * cos(slope);
				*(points + 11) = (*(points + 1) + *(points + 5))/2 - Wvalue1 * sin(slope);
				*(points + 12) = (*points + *(points + 4))/2 + Wvalue2 * cos(slope1);
				*(points + 13) = (*(points + 1) + *(points + 5))/2 + Wvalue2 * sin(slope1);
			}
			else
			{
				*(points + 8) = (*(points + 2) + *(points + 6))/2 + Wvalue1 * cos(slope);
				*(points + 9) = (*(points + 3) + *(points + 7))/2 + Wvalue1 * sin(slope);
				*(points + 10) = (*(points + 2) + *(points + 6))/2 - Wvalue1 * cos(slope);
				*(points + 11) = (*(points + 3) + *(points + 7))/2 - Wvalue1 * sin(slope);
				*(points + 12) = (*(points + 2) + *(points + 6))/2 - Wvalue2 * cos(slope1);
				*(points + 13) = (*(points + 3) + *(points + 7))/2 - Wvalue2 * sin(slope1);
			}*/
		}

	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0160", __FILE__, __FUNCSIG__); }
}

//Rectangle framegrab direction..
void RapidMath2D::RectangleFGDirection(double *pt1,double *pt2, int direction, double wup, double *points)
{
	try
	{
		double point1[2], point2[2];
		GetTopLeftNBtnRht(pt1, pt2, &point1[0], &point2[0]);
		double Wvalue1 = 48 * wup, Wvalue2 = 24 * wup, Wvalue3 = 18 * wup;
		switch(direction)
		{
			case 0:
				points[0] = point2[0]; points[1] = (point1[1] + point2[1])/2 + Wvalue2;
				points[2] = point2[0]; points[3] = points[1] - Wvalue1;
				points[4] = point2[0] + Wvalue3; points[5] = points[1] - Wvalue2;
				break;
			case 1:
				points[0] = (point1[0] + point2[0])/2 + Wvalue2; points[1] = point2[1];
				points[2] = points[0] - Wvalue1; points[3] = point2[1];
				points[4] = points[0] - Wvalue2; points[5] = points[1] - Wvalue3;
				break;
			case 2:
				points[0] = point1[0]; points[1] = (point1[1] + point2[1])/2 + Wvalue2;
				points[2] = point1[0]; points[3] = points[1] - Wvalue1;
				points[4] = point1[0] - Wvalue3; points[5] = points[1] - Wvalue2;
				break;
			case 3:
				points[0] = (point1[0] + point2[0])/2 + Wvalue2; points[1] = point1[1];
				points[2] = points[0] - Wvalue1; points[3] = point1[1];
				points[4] = points[0] - Wvalue2; points[5] = points[1] + Wvalue3;
				break;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0161", __FILE__, __FUNCSIG__); }
}

//endpoints of the arc//
void RapidMath2D::ArcEndPoints(double *center, double radius, double startangle, double sweepangle, double *endpts)
{
	try
	{
		double endangle = startangle + sweepangle;
		endpts[0] = center[0] + radius * cos(startangle);
		endpts[1] = center[1] + radius * sin(startangle);
		endpts[2] = center[0] + radius * cos(endangle);
		endpts[3] = center[1] + radius * sin(endangle);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0137", __FILE__, __FUNCSIG__); }
}

//Arc angles calculation
void RapidMath2D::ArcBestFitAngleCalculation(double *points, int size, double *center, double radius, double *angles)
{	
	try
	{
		double MinAngle1, MinAngle2, MaxAngle1, MaxAngle2, TempAngle;
		bool FirstHalf = false, SecondHalf = false;
		TempAngle = ray_angle(center, points);
		MinAngle1 = M_PI; MinAngle2 = 2 * M_PI;
		MaxAngle1 = 0; MaxAngle2 = M_PI;
		for(int i = 0; i < size; i++)
		{
			TempAngle = ray_angle(center, points + 2 * i);
			if(TempAngle > M_PI)
			{
				if(MinAngle2 > TempAngle) MinAngle2 = TempAngle;
				if(MaxAngle2 < TempAngle) MaxAngle2 = TempAngle;
				SecondHalf = true;
			}
			else
			{
				if(MinAngle1 > TempAngle) MinAngle1 = TempAngle;
				if(MaxAngle1 < TempAngle) MaxAngle1 = TempAngle;
				FirstHalf = true;
			}
		}
		if(FirstHalf && SecondHalf)
		{
			double diffAngle1 = 2 * M_PI - (MaxAngle2 - MinAngle1);
			double diffAngle2 = MinAngle2 - MaxAngle1;
			if(diffAngle1 < diffAngle2)
			{
				angles[0] = MinAngle2;
				angles[1] = 2 * M_PI - (MinAngle2 - MaxAngle1);
			}
			else
			{
				angles[0] = MinAngle1;
				angles[1] = MaxAngle2 - MinAngle1;
			}
		}
		else if(FirstHalf)
		{
			angles[0] = MinAngle1;
			angles[1] = MaxAngle1 - MinAngle1;
		}
		else
		{
			angles[0] = MinAngle2;
			angles[1] = MaxAngle2 - MinAngle2;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0185", __FILE__, __FUNCSIG__); }
}

//Calculate maximum and minimum point in the collection to get the end points..
void RapidMath2D::LineEndPoints(double *points, int size, double *endpt1, double *endpt2, double angle)
{
	try
	{
		Angle_FirstScndQuad(&angle);
		double minx = points[0], miny = points[1];
		double maxx = points[0], maxy = points[1];
		for(int i = 0; i < size; i++)
		{
			if(minx > points[2 * i])
				minx = points[2 * i];
			if(miny > points[2 * i + 1])
				miny = points[2 * i + 1];
			if(maxx < points[2 * i])
				maxx = points[2 * i];
			if(maxy < points[2 * i + 1])
				maxy = points[2 * i + 1];
		}
		if(angle  < M_PI_2)
		{
			endpt1[0] = minx; endpt1[1] = miny;
			endpt2[0] = maxx; endpt2[1] = maxy;
		}
		else
		{
			endpt1[0] = minx; endpt1[1] = maxy;
			endpt2[0] = maxx; endpt2[1] = miny;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0183", __FILE__, __FUNCSIG__); }
}

//Minimum value in a array of double values of size n...//
double RapidMath2D::minimum(double *values, int n)
{
	try
	{
		double min = values[0];
		for(int i = 1; i < n; i++)
		{
			if(min > values[i])
				min = values[i]; 
		}
		return min;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0147", __FILE__, __FUNCSIG__); return 0; }
}

//Maximum value in a array of double values of size n..//
double RapidMath2D::maximum(double *values, int n)
{
	try
	{
		double max = values[0];
		for(int i = 1; i < n; i++)
		{
			if(max < values[i])
				max = values[i]; 
		}
		return max;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0148", __FILE__, __FUNCSIG__); return 0; }
}

//Function to calculate the Rectangle enclosing Circle
void RapidMath2D::RectangleEnclosing_Circle(double *center, double radius, double *Lefttop, double *RightBottom)
{
	try
	{
		Lefttop[0] = center[0] - radius;
		Lefttop[1] = center[1] + radius;
		RightBottom[0] = center[0] + radius;
		RightBottom[1] = center[1] - radius;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0139", __FILE__, __FUNCSIG__); }
}

//Function to get the Rectangle Enclsoing for line
void RapidMath2D::RectangleEnclosing_Line(double *point1, double *point2, double *Lefttop, double *RightBottom)
{
	try
	{
		GetTopLeftNBtnRht(point1, point2, Lefttop, RightBottom);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0139", __FILE__, __FUNCSIG__); }
}

//Function to get the Rectangle Enclsoing for Arc
void RapidMath2D::RectangleEnclosing_Arc(double *center, double *endpt1, double *endpt2, double radius, double startang, double endangle,double *Lefttop, double *RightBottom)
{
	try
	{
		double PtX[6],PtY[6];
		int Cnt = 0;
		if(AngleIsinArcorNot(startang, endangle, 0))
		{
			PtX[Cnt] = *center + (radius)*cos(0.0);
			PtY[Cnt++] = *(center + 1) + (radius)*sin(0.0);
		}
		if(AngleIsinArcorNot(startang, endangle, M_PI_2))
		{
			PtX[Cnt] = *center + (radius)*cos(M_PI_2);
			PtY[Cnt++] = *(center + 1) + (radius)*sin(M_PI_2);
		}
		if(AngleIsinArcorNot(startang, endangle, M_PI))
		{
			PtX[Cnt] = *center + (radius)*cos(M_PI);
			PtY[Cnt++] = *(center + 1) + (radius)*sin(M_PI);
		}
		if(AngleIsinArcorNot(startang, endangle, M_PI_2 * 3))
		{
			PtX[Cnt] = *center + (radius)*cos(M_PI_2 * 3);
			PtY[Cnt++] = *(center + 1) + (radius)*sin(M_PI_2 * 3);
		}
		PtX[Cnt] = *endpt1;
		PtY[Cnt++] = *(endpt1 + 1);
		PtX[Cnt] = *endpt2;
		PtY[Cnt++] = *(endpt2 + 1);
		*Lefttop = minimum(&PtX[0], Cnt);
		*(Lefttop + 1) = maximum(&PtY[0], Cnt);
		*RightBottom = maximum(&PtX[0], Cnt);
		*(RightBottom + 1) = minimum(&PtY[0], Cnt);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0140", __FILE__, __FUNCSIG__); }
}

//Angle is in Arc or not.
bool RapidMath2D::AngleIsinArcorNot(double startangle, double endangle, double angle)
{
	try
	{
		double temp, temp1, temp2;
		if(startangle > endangle)
		{
			temp = startangle;
			startangle  = endangle;
			endangle = temp;
		}
		if(endangle > 2*M_PI)
			endangle -= 2*M_PI;
		//angles to check the position of the points on arc//
		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, startangle, &temp);
		//temp1 to temp2 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle, angle, &temp1);
		ArcAngleDiff(angle, startangle, &temp2);
		//check whether the point is on arc or not//
		//if yes the point is nearest point on arc to the given point//
		if((temp1 <= temp) && (temp2 <= temp))
			return true;
		return false;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0139", __FILE__, __FUNCSIG__); return false; }
}

//Get left top and right bottom of two points
void RapidMath2D::GetTopLeftNBtnRht(double *point1, double *point2, double *Lefttop, double *RightBottom)
{
	try
	{
		Lefttop[0] = min(point1[0], point2[0]);
		Lefttop[1] = max(point1[1], point2[1]);
		RightBottom[0] = max(point1[0], point2[0]);
		RightBottom[1] = min(point1[1], point2[1]);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0196", __FILE__, __FUNCSIG__); }
}

//get left top and right bottom of set of points..
void RapidMath2D::GetTopLeftAndRightBottom(double* Points, int Pointscount, int pointsize, double* LeftTop, double* RightBottom)
{
	try
	{
		if(Pointscount > 0)
		{
			double minx = Points[0], miny = Points[1];
			double maxx = Points[0], maxy = Points[1];
			for(int i = 0; i < Pointscount; i++)
			{
				if(minx > Points[pointsize * i])
					minx = Points[pointsize * i];
				if(miny > Points[pointsize * i + 1])
					miny = Points[pointsize * i + 1];
				if(maxx < Points[pointsize * i])
					maxx = Points[pointsize * i];
				if(maxy < Points[pointsize * i + 1])
					maxy = Points[pointsize * i + 1];
			}
			LeftTop[0] = minx; LeftTop[1] = maxy;
			RightBottom[0] = maxx; RightBottom[1] = miny;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0196", __FILE__, __FUNCSIG__); }
}

//One Point alignment Transformation matrix.. i,e. Only Translation..
void RapidMath2D::TransformM_OnePtAlign(double *point1, double *point2, double *TransformMatrix)
{
	try
	{
		TransformMatrix[0] = 1; TransformMatrix[1] = 0; TransformMatrix[2] = point2[0] - point1[0];
		TransformMatrix[3] = 0; TransformMatrix[4] = 1; TransformMatrix[5] = point2[1] - point1[1];
		TransformMatrix[6] = 0; TransformMatrix[7] = 0; TransformMatrix[8] = 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0033", __FILE__, __FUNCSIG__); }
}

//Function to calculate transformation matrix to Rotate about a Point..
void RapidMath2D::TransformM_RotateAboutPoint(double *point, double angle, double *TransformMatrix)
{
	try
	{
		double cosangle = cos(angle), sinangle = sin(angle);
		double OriginX = point[0] - cosangle * point[0] + sinangle * point[1];   
		double OriginY = point[1] - sinangle * point[0] - cosangle * point[1];
		TransformMatrix[0] = cosangle; TransformMatrix[1] = -sinangle; TransformMatrix[2] = OriginX;
		TransformMatrix[3] = sinangle; TransformMatrix[4] = cosangle; TransformMatrix[5] = OriginY;
		TransformMatrix[6] = 0; TransformMatrix[7] = 0; TransformMatrix[8] = 1;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0040", __FILE__, __FUNCSIG__); }
}

//Function to calculate Transformation Matrix for Two Point Alignment.
void RapidMath2D::TransformM_TwoPointAlign(double *point1, double* point2, double *point3, double *point4, double *TransformMatrix, double *RotatedAngle, double *ShiftedPt)
{
	try
	{
		double angle1 = ray_angle(point1[0], point1[1], point2[0], point2[1]);
		double angle2 = ray_angle(point3[0], point3[1], point4[0], point4[1]);
		double angle = angle2 - angle1;
		if(angle < 0) angle += 2 * M_PI;
		double cosangle = cos(angle), sinangle = sin(angle);
		ShiftedPt[0] = point3[0] - cosangle * point1[0] + sinangle * point1[1];
		ShiftedPt[1] = point3[1] - sinangle * point1[0] - cosangle * point1[1];
		TransformMatrix[0] = cosangle; TransformMatrix[1] = -sinangle; TransformMatrix[2] = ShiftedPt[0];
		TransformMatrix[3] = sinangle; TransformMatrix[4] = cosangle; TransformMatrix[5] = ShiftedPt[1];
		TransformMatrix[6] = 0; TransformMatrix[7] = 0; TransformMatrix[8] = 1;
		*RotatedAngle = angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0041", __FILE__, __FUNCSIG__); }
}

//Function to calculate Transformation Matrix for One Point One Line Alignment.
void RapidMath2D::TransformM_PointLineAlign(double *point1, double angle1, double intercept1, double *point2, double angle2, double intercept2, double *TransformMatrix, double *RotatedAngle, double *ShiftedPt)
{
	try
	{
		double angle = 0;
		Angle_FirstFourthQuad(&angle1); Angle_FirstFourthQuad(&angle2); // 0 to PI angle..
		//double Dist1 = Pt2Line_DistSigned(point1[0], point1[1], angle1, intercept1);
		//double Dist2 = Pt2Line_DistSigned(point2[0], point2[1], angle2, intercept2);
		angle = angle2 - angle1;
		/*if((Dist1 > 0 && Dist2 < 0) || (Dist1 < 0 && Dist2 > 0))
			angle = angle + M_PI;*/
		if(angle > M_PI_2)
			angle -= M_PI;
		else if(angle < -M_PI_2)
			angle += M_PI;
		double cosangle = cos(angle), sinangle = sin(angle);
		double OriginX = point2[0] - cosangle * point1[0] + sinangle * point1[1];   
		double OriginY = point2[1] - sinangle * point1[0] - cosangle * point1[1];
		TransformMatrix[0] = cosangle; TransformMatrix[1] = -sinangle; TransformMatrix[2] = OriginX;
		TransformMatrix[3] = sinangle; TransformMatrix[4] = cosangle; TransformMatrix[5] = OriginY;
		TransformMatrix[6] = 0; TransformMatrix[7] = 0; TransformMatrix[8] = 1;
		*RotatedAngle = angle; ShiftedPt[0] = OriginX; ShiftedPt[1] = OriginY;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0041", __FILE__, __FUNCSIG__); }
}

//Function to calculate Transformation Matrix for Two Line Alignment.
void RapidMath2D::TransformM_TwoLineAlign(double* Line1Param, double* Line2Param, double* Line3Param, double* Line4Param, double *TransformMatrix, double *RotatedAngle, double *ShiftedPt)
{
	try
	{
		//double CenterPt1[2] = {0}, CenterPt2[2] = {0};
		//double Angle1 = Line1Param[0], Angle2 = Line2Param[0], Angle3 = Line3Param[0], Angle4 = Line4Param[0];
		////Angle_FirstScndQuad(&Angle1); Angle_FirstScndQuad(&Angle2); Angle_FirstScndQuad(&Angle3); Angle_FirstScndQuad(&Angle4);
		//Angle_FourQuad(&Angle1); Angle_FourQuad(&Angle2); Angle_FourQuad(&Angle3); Angle_FourQuad(&Angle4);
		//double diffAngle1, diffAngle2;
		//if ((abs(Angle3 - Angle1) > M_PI_2) && (abs(Angle3 - Angle1) < 3*M_PI_2))
		//{
		//	if (Angle3 > Angle1)
		//		Angle3 -= M_PI;
		//	else
		//		Angle3 += M_PI;
		//}
		//diffAngle1 = (Angle3 - Angle1);
		//Angle_FirstFourthQuad(&diffAngle1);
		//if ((abs(Angle4 - Angle2) > M_PI_2) && (abs(Angle4 - Angle2) < 3*M_PI_2))
		//{
		//	if (Angle4 > Angle2)
		//		Angle4 -= M_PI;
		//	else
		//		Angle4 += M_PI;
		//}
		//diffAngle2 = (Angle4 - Angle2);
		//Angle_FirstFourthQuad(&diffAngle2);
		//Line_lineintersection(Line1Param[0], Line1Param[1], Line2Param[0], Line2Param[1], &CenterPt1[0]);
		//Line_lineintersection(Line3Param[0], Line3Param[1], Line4Param[0], Line4Param[1], &CenterPt2[0]);

		//if ((abs(diffAngle2 - diffAngle1) > M_PI_2) && (abs(diffAngle2 - diffAngle1) < 3*M_PI_2))
		//{
		//	if (diffAngle2 > diffAngle1)
		//		diffAngle2 -= M_PI;
		//	else
		//		diffAngle2 += M_PI;
		//}
		////Angle_FourQuad(&MidAngle1); Angle_FourQuad(&MidAngle2); // 0 to 2PI angle..
		//double angle = (diffAngle2 + diffAngle1)/2;
		//double cosangle = cos(angle), sinangle = sin(angle);
		//double OriginX = CenterPt2[0] - cosangle * CenterPt1[0] + sinangle * CenterPt1[1];   
		//double OriginY = CenterPt2[1] - sinangle * CenterPt1[0] - cosangle * CenterPt1[1];

		//TransformMatrix[0] = cosangle; TransformMatrix[1] = -sinangle; TransformMatrix[2] = OriginX;
		//TransformMatrix[3] = sinangle; TransformMatrix[4] = cosangle; TransformMatrix[5] = OriginY;
		//TransformMatrix[6] = 0; TransformMatrix[7] = 0; TransformMatrix[8] = 1;


		double CenterPt1[2] = {0}, CenterPt2[2] = {0};
		double Angle1 = Line1Param[0], Angle2 = Line2Param[0], Angle3 = Line3Param[0], Angle4 = Line4Param[0];
		Angle_FourQuad(&Angle1); Angle_FourQuad(&Angle2); Angle_FourQuad(&Angle3); Angle_FourQuad(&Angle4);
		double diffAngle1 = 0, diffAngle2 = 0;
		if ((abs(Angle3 - Angle1) > M_PI_2) && (abs(Angle3 - Angle1) < 3*M_PI_2))
		{
			if (Angle3 > Angle1)
				Angle3 -= M_PI;
			else
				Angle3 += M_PI;
		}
		diffAngle1 = (Angle3 - Angle1);
		Angle_FirstFourthQuad(&diffAngle1);

		if(diffAngle1 > M_PI_2)
			diffAngle1 -= M_PI;
		else if(diffAngle1 < -M_PI_2)
			diffAngle1 += M_PI;
		
		Line_lineintersection(Line1Param[0], Line1Param[1], Line2Param[0], Line2Param[1], &CenterPt1[0]);
		Line_lineintersection(Line3Param[0], Line3Param[1], Line4Param[0], Line4Param[1], &CenterPt2[0]);

		double cosangle = cos(diffAngle1), sinangle = sin(diffAngle1);
		double OriginX = CenterPt2[0] - cosangle * CenterPt1[0] + sinangle * CenterPt1[1];   
		double OriginY = CenterPt2[1] - sinangle * CenterPt1[0] - cosangle * CenterPt1[1];

		TransformMatrix[0] = cosangle; TransformMatrix[1] = -sinangle; TransformMatrix[2] = OriginX;
		TransformMatrix[3] = sinangle; TransformMatrix[4] = cosangle; TransformMatrix[5] = OriginY;
		TransformMatrix[6] = 0; TransformMatrix[7] = 0; TransformMatrix[8] = 1;
		*RotatedAngle = diffAngle1; ShiftedPt[0] = OriginX; ShiftedPt[1] = OriginY;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0041", __FILE__, __FUNCSIG__); }
}

//function to convert the angle to 0 to 180 range//
void RapidMath2D::Angle_FirstScndQuad(double *slope)
{
	try
	{
		if(*slope < 0)
			*slope += M_PI;
		if(*slope >= M_PI)
			*slope -= M_PI;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0132", __FILE__, __FUNCSIG__);}
}

//function to convert the angle to -90 to +90 range//
void RapidMath2D::Angle_FirstFourthQuad(double *slope)
{
	try
	{
		if(*slope < 0)
			*slope += M_PI;
		if(*slope >= M_PI_2)
			*slope -= M_PI;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0134", __FILE__, __FUNCSIG__);}
}

//function to convert the angle to 0 to 360 range//
void RapidMath2D::Angle_FourQuad(double *slope)
{
	try
	{
		if(*slope < 0)
			*slope += 2 * M_PI;
		if(*slope > 2 * M_PI)
			*slope -= 2 * M_PI;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0135", __FILE__, __FUNCSIG__);}
}

//Arc Angle difference between two angles ( 0 to 360)
void RapidMath2D::ArcAngleDiff(double angle1, double angle2, double *angle)
{
	try
	{
		*angle = angle1 - angle2;
		if(*angle < 0)
			*angle += 2 * M_PI;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0111", __FILE__, __FUNCSIG__); }
}

//function to find intersection point of perpendicular from a given point//
void RapidMath2D::Point_PerpenIntrsctn_Line(double slope, double intercept, double *point, double *intersection)
{
	try
	{
		Angle_FirstScndQuad(&slope);
		double dis = Pt2Line_DistSigned(point[0], point[1], slope, intercept);
		intersection[0] = point[0] - dis * sin(slope);
		intersection[1] = point[1] + dis * cos(slope);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0060", __FILE__, __FUNCSIG__); }
}

//Straightness of line..
double RapidMath2D::StraightNessofLine(int totalpoints, double *setofpoints, double slope, double intercept)
{
	try
	{
		double dist = 0, dist1 = 0, dist2 = 0;
		for(int i = 0; i < totalpoints; i ++)
		{
			dist = Pt2Line_DistSigned(setofpoints[2 * i], setofpoints[2 * i + 1], slope, intercept);
			if(i == 0)
			{
				if(dist > 0)
					dist1 = dist;
				else
					dist2 = dist;
			}
			else
			{
				if(dist > dist1)
					dist1 = dist;
				if(dist < dist2)
					dist2 = dist;
			}
		}
		return (dist1 + abs(dist2));
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0058", __FILE__, __FUNCSIG__); return 0; }
}

double RapidMath2D::FilteredStraightNessofLine(int totalpoints, double *setofpoints, double slope, double intercept, int filterFactor)
{
	try
	{
		double dist = 0, tmpdist = 0;
		for(int i = 0; i < totalpoints; i++)
		{
			tmpdist = Pt2Line_Dist(setofpoints[2 * i], setofpoints[2 * i + 1], slope, intercept);
			dist += abs(tmpdist);
		} 
		dist /= totalpoints;
		int newcnt = 0;
		double *filterPnts = new double[totalpoints * 2];
		for(int i = 0; i < totalpoints; i++)
		{
			tmpdist = Pt2Line_Dist(setofpoints[2 * i], setofpoints[2 * i + 1], slope, intercept);
			if(abs(tmpdist) < filterFactor * dist)
			{
			   filterPnts[2 * i] = setofpoints[2 * i];
			   filterPnts[2 * i + 1] = setofpoints[2 * i + 1];
			   newcnt++;
			}
		} 
		double strtness = StraightNessofLine(newcnt, filterPnts, slope, intercept);
		delete [] filterPnts;
		return strtness;
	 }
	catch(...)
	{
	   SetAndRaiseErrorMessage("RMATH2D0058b", __FILE__, __FUNCSIG__); return 0; 
	}
}

//Line Calculation for Shift ON..
void RapidMath2D::LineForShiftON(double angle, double *endpt1, double *endpt2, double *rpoint, double *rangle, double *rintercept)
{
	try
	{
		double Anglevalue = M_PI_4/2;
		double point1[2] = {endpt1[0], endpt1[1]}, point2[2] = {endpt2[0], endpt2[1]}, intercept;
		double QuadCnt1 = floor((angle / (M_PI_4/2)));
		if(angle < Anglevalue || angle > (15 * Anglevalue))
		{
			angle = 0;
			intercept = point1[1];
		}
		else if(angle > Anglevalue && angle < (3 * Anglevalue))
		{
			angle = M_PI_4;
			Intercept_LinePassing_Point(&point1[0], angle, &intercept);
		}
		else if(angle > (3 * Anglevalue) && angle < (5 * Anglevalue))
		{
			angle = M_PI_2;
			Intercept_LinePassing_Point(&point1[0], angle, &intercept);
		}
		else if(angle > (5 * Anglevalue) && angle < (7 * Anglevalue))
		{
			angle = 3 * M_PI_4;
			Intercept_LinePassing_Point(&point1[0], angle, &intercept);
		}
		else if(angle > (7 * Anglevalue) && angle < (9 * Anglevalue))
		{
			angle = M_PI;
			intercept = point1[1];
		}
		else if(angle > (9 * Anglevalue) && angle < (11 * Anglevalue))
		{
			angle = 5 * M_PI_4;
			Intercept_LinePassing_Point(&point1[0], angle, &intercept);
		}
		else if(angle > (11 * Anglevalue) && angle < (13 * Anglevalue))
		{
			angle = 6 * M_PI_4;
			intercept = point1[0];
		}
		else
		{
			angle = 7 * M_PI_4;
			Intercept_LinePassing_Point(&point1[0], angle, &intercept);
		}
		Point_PerpenIntrsctn_Line(angle, intercept, &point2[0], rpoint);
		*rintercept = intercept;
		*rangle = angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0194", __FILE__, __FUNCSIG__); }
}

//Horizontal or Vertical calculation  for Digital Micrometer Measurement..
void RapidMath2D::LineHorizontal_Vertical(double angle, double *pt1, double *pt2, double *rpoint, double *rangle, double *rintercept, bool Horizon)
{
	try
	{
		double point1[2] = {pt1[0], pt1[1]}, point2[2] = {pt2[0], pt2[1]};
		double intercept;
		if(Horizon)
		{
			angle = 0;
			Intercept_LinePassing_Point(&point1[0], angle, &intercept);
		}
		else
		{
			angle = M_PI_2;
			Intercept_LinePassing_Point(&point1[0], angle, &intercept);
		}
		Point_PerpenIntrsctn_Line(angle, intercept, &point2[0], rpoint);
		*rintercept = intercept;
		*rangle = angle;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0195", __FILE__, __FUNCSIG__); }
}

//Line Angularity Calculation.
void RapidMath2D::AngularityCalculation(double angle1, double angle2, double land, double referenceangle, double *angularity)
{
	try
	{
		double anglrty;
		Angle_FirstScndQuad(&angle1); Angle_FirstScndQuad(&angle2);
		anglrty = abs(referenceangle - abs(angle1 - angle2));
		anglrty = land * tan(anglrty);
		*angularity = anglrty;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0197", __FILE__, __FUNCSIG__); }
}

//function calculate the intercept of the line passing throu given point//
void RapidMath2D::Intercept_LinePassing_Point(double *point, double slope, double *intercept)
{
	try
	{
		if(slope == M_PI_2 ||slope == 3 * M_PI_2)
			*intercept = point[0];
		else
			*intercept = point[1] - (tan(slope)) * point[0];
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0008", __FILE__, __FUNCSIG__); }
}

 //function to calculoate the slope of line joining two points//
void RapidMath2D::TwoPointLine(double *point1, double *point2, double *slope, double *intercept)
{
	try
	{
		/*if(point1[0] == point2[0])
		{
			*slope = M_PI_2;
			*intercept = point1[0];
		}
		else
		{
			*slope = atan((point1[1] - point2[1]) / (point1[0] - point2[0]));
			*intercept  = point1[1] - (tan(*slope) * point1[0]);
		}
		if(*slope < 0)
			*slope += M_PI;*/
		*slope = atan((point1[1] - point2[1]) / (point1[0] - point2[0]));
		if(*slope < 0)
			*slope += M_PI;
		if(abs(*slope - M_PI_2) > 0.00058)
		{
			*intercept  = point1[1] - (tan(*slope) * point1[0]);
		}
		else
		{
			*slope = M_PI_2;
			*intercept = point1[0];
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0016", __FILE__, __FUNCSIG__); }
}

//Function to Find the nearest point on circle to a given point///
void RapidMath2D::Point_onCircle(double *center, double radius, double *point, double *nearestpt)
{
	try
	{
		//slope of the line joining the center and point//
		double slope = ray_angle(center[0], center[1], point[0], point[1]);
		double cosangle = radius * cos(slope), sinangle = radius * sin(slope);
		nearestpt[0] = center[0] + radius * cos(slope);
		nearestpt[1] = center[1] + radius * sin(slope);
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0071", __FILE__, __FUNCSIG__); }
}

//Point on arc with respect to mouse point.../ need to modify!
bool RapidMath2D::Point_onArc(double *center, double radius, double startangle, double sweepangle, double *mousept, double *point)
{
	double point1[2] = {0}, point2[2] = {0}, point3[3] = {0}, nearestpt[2] = {0};
	double d1 = 0, d2 = 0, slope1 = 0, endangle = 0, temp = 0, temp1 = 0, temp2 = 0;
	try
	{
		//calculate the angle of the line joining the mousepoint and center of the arc//
		slope1 = ray_angle(center, mousept);
		//point on arc near to that point//
		point1[0] = *center + radius * (cos(slope1));
		point1[1] = *(center + 1) + radius * (sin(slope1));
		//endangle of the arc//
		endangle = startangle + sweepangle;
		//calculate endpoints of the arc using start angle and end angle//
		point2[0] = *center + radius * (cos(startangle));
		point2[1] = *(center + 1) + radius * (sin(startangle));

		point3[0] = *center + radius * (cos(endangle));
		point3[1] = *(center + 1) + radius * (sin(endangle));
		d1 = Pt2Pt_distance(point, &point2[0]);
		d2 = Pt2Pt_distance(point, &point3[0]);
		if(startangle > endangle)
		{
			temp = startangle;
			startangle  = endangle;
			endangle = temp;

		}
		if(endangle > 2*M_PI)
		{
			endangle -= 2*M_PI;
		}
		//angles to check the position of the points on arc//
		ArcAngleDiff(endangle, startangle, &temp);
		//temp1 to temp2 are the angle difference between startangle or endangle with the intersection angle of arc1//
		ArcAngleDiff(endangle, slope1, &temp1);
		ArcAngleDiff(slope1, startangle, &temp2);
		//check whether the point is on arc or not//
		//if yes the point is nearest point on arc to the given point//
		if((temp1 <= temp) && (temp2 <= temp))
		{
			nearestpt[0] = point1[0];
			nearestpt[1] = point1[1];
		}
		//If not one of the end point is the nearest point//
		else
		{
			if(d1 < d2)
			{
				nearestpt[0] = point2[0];
				nearestpt[1] = point2[1];

			}
			else
			{
				nearestpt[0] = point3[0];
				nearestpt[1] = point3[1];
			}

		}
		
		*point = nearestpt[0];
		*(point + 1) = nearestpt[1];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0189", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Projection_Of_Point_OnArc(double* Center, double Radius, double StartAngle, double EndAngle, double* Point, double* Answer)
{
	try
	{
		double slope = ray_angle(Center, Point);
		if(slope > StartAngle && slope < EndAngle)
		{
			Answer[0] = Center[0] + Radius * cos(slope);
			Answer[1] = Center[1] + Radius * sin(slope);
		}
		else
			return false;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0189a", __FILE__, __FUNCSIG__); return false; }
}

//vinod.......................Projection_Of_Point_Online..........................................................................................
void RapidMath2D::Projection_Of_Point_Online(double* ProjectablePt, double TargetLineAngle, double targetLineIntercept, double* projectedPt)
{
	double NewLineSlope = tan(TargetLineAngle + M_PI_2);
	double TargetLineSlope = tan(TargetLineAngle);
	double NewLineIntercept = ProjectablePt[1]-(NewLineSlope*ProjectablePt[0]);
	projectedPt[0] = (targetLineIntercept-NewLineIntercept)/(NewLineSlope-TargetLineSlope);
	projectedPt[1] = ((NewLineIntercept*TargetLineSlope)-(targetLineIntercept*NewLineSlope))/(TargetLineSlope-NewLineSlope);
}

void RapidMath2D::ReflectPointsAlongLine(double *OriginalPoints, int PtsCnt, double slope, double Intercept, double* ReflectedPts)
{
	try
	{
		Angle_FirstScndQuad(&slope);
		for(int i = 0; i < PtsCnt; i++)
		{
			double dis = Pt2Line_DistSigned(OriginalPoints[3 * i], OriginalPoints[3 * i + 1], slope, Intercept);
			ReflectedPts[3 * i] = OriginalPoints[3 * i] - dis * sin(slope) * 2;
			ReflectedPts[3 * i + 1] = OriginalPoints[3 * i + 1] + dis * cos(slope) * 2;
		}	
	}
	catch(...){ SetAndRaiseErrorMessage("RMATH2D0189b", __FILE__, __FUNCSIG__); }
}

//Function to draw graphics for point to point distance//
bool RapidMath2D::Pt2Ptdis(double *point1, double *point2, double *mousept,double *space, double dis,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	 //slope -- slope of the line joining the two points , slope1 -- slope of the perpendicular line//
	double slope,slope1,d = 0,intercept,intercept1,intercept2;
	double endpt11[2],endpt21[2],endpt12[2],endpt22[2];
	try
	{
		//for calculation purpose consider point1 holds the lower point , if not swap the points//
		swap2(point1,point2);
		//function  calculates the slope and itercept of givn two points//
		slopesintercept(point1,point2,mousept,&slope,&intercept,&slope1);
		Angle_FirstScndQuad(&slope);
		//If slope of line2 is -ve then add PI//
		Angle_FirstScndQuad(&slope1);
		
		*slopear = slope;
		//If the slope of perpendicular is 90 then the intercepts of the lines are X coordinate of the points//
		if(slope1 == M_PI_2)
		{
			intercept1 = *point1;
			intercept2 = *point2;
		}
		//calculate the intercepts of the lines//
		else
		{
			intercept1  = *(point1 + 1) - (tan(slope1)) * (*point1);
			intercept2  = *(point2 + 1) - (tan(slope1)) * (*point2);
		} 
		//depending on the mouse position calculate points near(some distance) to the given points //
		//these points are starting point for the dimensiong//
		
		*endpt1  = *point1 - (cos(slope1)) * d;
		*(endpt1 + 1) = *(point1 + 1) - (sin(slope1)) * d;
		*endpt2 = *point2 - (cos(slope1)) * d;
		*(endpt2 + 1) = *(point2 + 1) - (sin(slope1)) * d;

		//calculate the intersection point of the lines//
		Line_lineintersection(slope,intercept,slope1,intercept1, &endpt11[0]);
		Line_lineintersection(slope,intercept,slope1,intercept2, &endpt21[0]);
		//copy the end points to the array//
		for(int i = 0; i < 2; i ++)
		{
			*(endpt1 + 2 + i) = endpt11[i];
			*(endpt2 + 2 + i) = endpt21[i]; 
		}
		//Depending on the mouse position calculate the line end points and point where the disatnce should be displayed//
		Pt2ptdis1(endpt11,endpt21,space, dis,&slope,midpt,&endpt12[0],&endpt22[0]);
		//copy the end points to the array//
		for(int i = 0; i < 2; i ++)
		{
			*(endpt1 + 4 + i) = endpt12[i];
			*(endpt2 + 4 + i) = endpt22[i]; 
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0001", __FILE__, __FUNCSIG__); return false; }
}

//function to to caluclate the end points of the line when the mouse positon is in between the two points//
bool RapidMath2D::Pt2ptdis1(double *endpt1,double *endpt2, double *space,double dis, double *slope, double *midpt,double *endpt11,double *endpt21)
{
	try
	{
		double StrWidth, Swidth, angle = *slope;
		//depending on line orientation select the calcualate the space required for string///
		if((angle >= 0 && angle < (20*M_PI/180)) || (angle >= (160*M_PI/180) && angle <=M_PI))
			StrWidth = space[0];
		else
			StrWidth = space[1] * 3;
		Swidth = 3 * StrWidth / 2;
		//If the distance between the points is greater than space then display the distance in between the lines, and direction of line segment should be inside to outside//
		if(dis > Swidth)
		{
			midpt[0] = (endpt1[0] + endpt2[0]) / 2;
			midpt[1] = (endpt1[1] + endpt2[1]) / 2;
			//Changed the value   endpt11[0] = midpt[0] - cos(angle) * Swidth;   to  endpt11[0] = midpt[0] - cos(angle) * Swidth / 4; so as to reduce the distance .
			//Shweytank Mishra .....date 6 nov 2012
			endpt11[0] = midpt[0] - cos(angle) * Swidth / 4;
			endpt11[1] = midpt[1] - sin(angle) * Swidth / 4;
			endpt21[0] = midpt[0] + cos(angle) * Swidth / 4;
			endpt21[1] = midpt[1] + sin(angle) * Swidth / 4;
		}
		//display distance on the right side of the drawing--  midpt on the right side of the drawing//
		else
		{
			double Midptgap = space[0];
			double LineWidth = space[0]/2;
			if(MeasurementDirection)
			{
				if(angle < M_PI_2)
				{
					midpt[0] = endpt2[0] + cos(angle) * Midptgap;
					midpt[1] = endpt2[1] + sin(angle) * Midptgap;
				}
				else
				{
					midpt[0] = endpt1[0] - cos(angle) * Midptgap;
					midpt[1] = endpt1[1] - sin(angle) * Midptgap;
				}
			}
			else
			{
				if(angle >= M_PI_2)
				{
					midpt[0] = endpt2[0] + cos(angle) * Midptgap;
					midpt[1] = endpt2[1] + sin(angle) * Midptgap;
				}
				else
				{
					midpt[0] = endpt1[0] - cos(angle) * Midptgap;
					midpt[1] = endpt1[1] - sin(angle) * Midptgap;
				}
			}
			endpt11[0] = endpt1[0] - cos(angle) * LineWidth;
			endpt11[1] = endpt1[1] - sin(angle) * LineWidth;
			endpt21[0] = endpt2[0] + cos(angle) * LineWidth;
			endpt21[1] = endpt2[1] + sin(angle) * LineWidth;
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0002", __FILE__, __FUNCSIG__); return false; }
}

//function to to caluclate the end points of the line when the mouse positon is in between the two points//
bool RapidMath2D::Pt2ptdis2(double *endpt1,double *endpt2, double *space,double dis, double *slope, double *midpt,double *endpt11,double *endpt21)
{
	try
	{
		double StrWidth, Swidth, angle = *slope;
		//depending on line orientation select the calcualate the space required for string///
		if((angle >= 0 && angle < (20*M_PI/180)) || (angle >= (160*M_PI/180) && angle <=M_PI))
			StrWidth = space[0];
		else
			StrWidth = space[1] * 3;
		Swidth = 3 * StrWidth / 2;
		//If the distance between the points is greater than space then display the distance in between the lines, and direction of line segment should be inside to outside//
		if(dis > Swidth)
		{
			midpt[0] = (endpt1[0] + endpt2[0]) / 2;
			midpt[1] = (endpt1[1] + endpt2[1]) / 2;
			//Changed the value   endpt11[0] = midpt[0] - cos(angle) * Swidth/2;   to  endpt11[0] = midpt[0] - cos(angle) * Swidth / 4; so as to reduce the distance ...
			//Shweytank Mishra .....date 6 nov 2012
			endpt11[0] = midpt[0] + cos(angle) * Swidth/4;
			endpt11[1] = midpt[1] + sin(angle) * Swidth/4;
			endpt21[0] = midpt[0] - cos(angle) * Swidth/4;
			endpt21[1] = midpt[1] - sin(angle) * Swidth/4;
		}
		//display distance on the right side of the drawing--  midpt on the right side of the drawing//
		else
		{
			double Midptgap = space[0];
			double LineWidth = space[0]/2;
			if(MeasurementDirection)
			{
				if(angle < M_PI_2)
				{
					midpt[0] = endpt2[0] + cos(angle) * Midptgap;
					midpt[1] = endpt2[1] + sin(angle) * Midptgap;
				}
				else
				{
					midpt[0] = endpt1[0] - cos(angle) * Midptgap;
					midpt[1] = endpt1[1] - sin(angle) * Midptgap;
				}
			}
			else
			{
				if(angle >= M_PI_2)
				{
					midpt[0] = endpt2[0] + cos(angle) * Midptgap;
					midpt[1] = endpt2[1] + sin(angle) * Midptgap;
				}
				else
				{
					midpt[0] = endpt1[0] - cos(angle) * Midptgap;
					midpt[1] = endpt1[1] - sin(angle) * Midptgap;
				}
			}
			endpt11[0] = endpt1[0] + cos(angle) * LineWidth;
			endpt11[1] = endpt1[1] + sin(angle) * LineWidth;
			endpt21[0] = endpt2[0] - cos(angle) * LineWidth;
			endpt21[1] = endpt2[1] - sin(angle) * LineWidth;
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0003", __FILE__, __FUNCSIG__); return false; }
}

//Digital Micrometer draw Calculation
void RapidMath2D::Digital_Micrometer(double *point1, double *point2, double dis, double* space, double wupp, double *endpt1, double *endpt2, double* midpt, double *slopear, bool nearMousePt)
{
	try
	{
		double slope = ray_angle(point1[0], point1[1], point2[0], point2[1]);
		double StrWidth, Swidth, angle = slope, slope1 = slope + M_PI_2;
		Angle_FirstScndQuad(&angle);
		*slopear = slope;
		//depending on line orientation select the calcualate the space required for string///
		if((angle >= 0 && angle < (20*M_PI/180)) || (angle >= (160*M_PI/180) && angle <=M_PI))
			StrWidth = space[0];
		else
			StrWidth = space[1] * 4;
		Swidth = 3 * StrWidth / 2;
		double LineWidth = StrWidth / 2;
		double cosangle = wupp * cos(slope), sinangle = wupp * sin(slope);
		double cosangle1 = wupp * cos(slope1), sinangle1 = wupp * sin(slope1);

		endpt1[0] = point1[0] - cosangle1; endpt1[1] = point1[1] - sinangle1;
		endpt1[2] = point1[0] + cosangle1; endpt1[3] = point1[1] + sinangle1;
		endpt2[0] = point2[0] - cosangle1; endpt2[1] = point2[1] - sinangle1;
		endpt2[2] = point2[0] + cosangle1; endpt2[3] = point2[1] + sinangle1;

		endpt1[4] = point1[0]; endpt1[5] = point1[1];
		endpt2[4] = point2[0]; endpt2[5] = point2[1];

		//If the distance between the points is greater than space then display the distance in between the lines, and direction of line segment should be inside to outside//
		if(dis > Swidth)
		{
			if(nearMousePt)
			{
				midpt[0] = point2[0] - space[0] * cos(slope); midpt[1] = point2[1] - space[0] * sin(slope);
			}
			else
			{
				midpt[0] = (point1[0] + point2[0])/2; midpt[1] = (point1[1] + point2[1])/2;
			}
			endpt1[6] = midpt[0] - cos(slope) * LineWidth;
			endpt1[7] = midpt[1] - sin(slope) * LineWidth;
			endpt2[6] = midpt[0] + cos(slope) * LineWidth;
			endpt2[7] = midpt[1] + sin(slope) * LineWidth;
		}
		//display distance on the right side of the drawing--  midpt on the right side of the drawing//
		else
		{
			endpt1[6] = point1[0] - cos(slope) * LineWidth;
			endpt1[7] = point1[1] - sin(slope) * LineWidth;
			endpt2[6] = point2[0] + cos(slope) * LineWidth;
			endpt2[7] = point2[1] + sin(slope) * LineWidth;
			if(MeasurementDirection)
			{
				midpt[0] = endpt1[6] - cos(slope) * space[0]/2;
				midpt[1] = endpt1[7] - sin(slope) * space[0]/2;
			}
			else
			{
				midpt[0] = endpt2[6] + cos(slope) * space[0]/2;
				midpt[1] = endpt2[7] + sin(slope) * space[0]/2;
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0049", __FILE__, __FUNCSIG__); }
}

//Edit DMM
void RapidMath2D::EditDigitalMicrometer(double *point1, double *point2, double *mousept, double *endpt1, double *endpt2)
{
	try
	{
		double slope, slope1, intercept1, intercept2;
		slope = ray_angle(point1, point2);
		slope1 = slope + M_PI_2;
		intercept1 = InterceptOfline(slope1, point1);
		intercept2 = InterceptOfline(slope1, point2);
		intersection(slope1, intercept1, mousept, endpt1);
		intersection(slope1, intercept2, mousept, endpt2);
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0005", __FILE__, __FUNCSIG__); }
}

//Function to caluclate arrow parameters//
bool RapidMath2D::arrow(double *mousept, double slopear, double *endpt1, double *endpt2, double dis, double Awidth, double *space, double *pt12, double *pt13, double *pt22, double *pt23)
{
	
	try
	{
		double slope1 = slopear + M_PI / 15, slope2 = slopear - M_PI / 15, StrWidth;
		double angle = slopear; Angle_FirstScndQuad(&angle);
		//depending on line orientation select the calcualate the space required for string///
		if((angle >= 0 && angle < (20*M_PI/180)) || (angle >= (160*M_PI/180) && angle <= M_PI))
			StrWidth = space[0];
		else
			StrWidth = space[1] * 4;
		if(dis > StrWidth * 1.5)
			Awidth = Awidth;
		else
			Awidth = -Awidth;
		pt12[0] = endpt1[0] + cos(slope1) * Awidth;
		pt12[1] = endpt1[1] + sin(slope1) * Awidth;
		pt13[0] = endpt1[0] + cos(slope2) * Awidth;
		pt13[1] = endpt1[1] + sin(slope2) * Awidth;
		pt22[0] = endpt2[0] - cos(slope1) * Awidth;
		pt22[1] = endpt2[1] - sin(slope1) * Awidth;
		pt23[0] = endpt2[0] - cos(slope2) * Awidth;
		pt23[1] = endpt2[1] - sin(slope2) * Awidth;
		return true;	
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0006", __FILE__, __FUNCSIG__); return false; }
}

//function to calculate arrow values(points), arrow direction is right to left//
bool RapidMath2D::arrow1(double *endpt1, double slope1, double slope2, double *pt12, double *pt13, double w)
{
	try
	{
		pt12[0] = endpt1[0] + cos(slope1) * w;
		pt12[1] = endpt1[1] + sin(slope1) * w;
		pt13[0] = endpt1[0] + cos(slope2) * w;
		pt13[1] = endpt1[1] + sin(slope2) * w;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0007", __FILE__, __FUNCSIG__); return false; }
}

///
//function to calculate arrow values(points), arrow direction is ;eft to right//
bool RapidMath2D::arrow2(double *endpt2, double slope1, double slope2, double *pt22, double *pt23, double w)
{
	try
	{
		pt22[0] = endpt2[0] - cos(slope1) * w;
		pt22[1] = endpt2[1] - sin(slope1) * w;
		pt23[0] = endpt2[0] - cos(slope2) * w;
		pt23[1] = endpt2[1] - sin(slope2) * w;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0008", __FILE__, __FUNCSIG__); return false; }
}
//
//funcion to draw shapes for point to line distance
bool RapidMath2D::Point_FinitelineDistance(double *point,double slope,double intercept,double *endp1, double *endp2,double *intercept1,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	try
	{
		//calculate the intercept of the line passing throught the point //
		Intercept_LinePassing_Point(point, slope, intercept1);
		*slopear = slope + M_PI_2;
		Angle_FirstScndQuad(slopear);
		intersection(slope,*intercept1,mousept,endpt1 + 2);
		intersection(slope,intercept,mousept,endpt2 + 2);
		if(*intercept1 > intercept)
			Pt2ptdis2(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		else
			Pt2ptdis1(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		*(endpt1 + 4) = *point;
		*(endpt1 + 5) = *(point + 1);
		if(Ptisinwindow(endpt2 + 2, endp1, endp2))
		{
			*(endpt2 + 4) = *(endpt2 + 2);
			*(endpt2 + 5) = *(endpt2 + 3);
		}
		else
		{
			if(Pt2Pt_distance(endpt2 + 2, endp1) < Pt2Pt_distance(endpt2 + 2, endp2))
			{
				*(endpt2 + 4) = *endp1; 
				*(endpt2 + 5) = *(endp1 + 1);
			}
			else
			{
				*(endpt2 + 4) = *endp2; 
				*(endpt2 + 5) = *(endp2 + 1);
			}
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0009", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Point_InfinitelineDistance(double *point,double slope,double intercept, double *intercept1,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	try
	{
		//calculate the intercept of the line passing throught the point //
		Intercept_LinePassing_Point(point, slope, intercept1);
		*slopear = slope + M_PI_2;
		Angle_FirstScndQuad(slopear);
		intersection(slope,*intercept1,mousept,endpt1 + 2);
		intersection(slope,intercept,mousept,endpt2 + 2);
		if(*intercept1 > intercept)
			Pt2ptdis2(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		else
			Pt2ptdis1(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		*(endpt1 + 4) = *point;
		*(endpt1 + 5) = *(point + 1);
		*(endpt2 + 4) = *(endpt2 + 2);
		*(endpt2 + 5) = *(endpt2 + 3);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0010", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Point_RayDistance(double *point,double slope,double intercept,double *endp1, double *intercept1,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	double slopeofray;
	try
	{
		//calculate the intercept of the line passing throught the point //
		slopeofray = slope;
		Angle_FirstScndQuad(&slope);
		Intercept_LinePassing_Point(point, slope, intercept1);
		*slopear = slope + M_PI_2;
		Angle_FirstScndQuad(slopear);
		intersection(slope,*intercept1,mousept,endpt1 + 2);
		intersection(slope,intercept,mousept,endpt2 + 2);
		if(*intercept1 > intercept)
			Pt2ptdis2(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		else
			Pt2ptdis1(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		*(endpt1 + 4) = *point;
		*(endpt1 + 5) = *(point + 1);
		double tempangle = ray_angle(endp1,endpt2 + 2);
		if(RoundDecimal(tempangle, 4) == RoundDecimal(slopeofray, 4))
		{
			*(endpt2 + 4) = *(endpt2 + 2);
			*(endpt2 + 5) = *(endpt2 + 3);
		}
		else
		{
			*(endpt2 + 4) = *endp1;
			*(endpt2 + 5) = *(endp1 + 1);
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0011", __FILE__, __FUNCSIG__); return false; }
}
 

bool RapidMath2D::Infiniteline_InfinitelineDistance(double slope1,double intercept1,double slope2,double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	try
	{
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		*slopear = slope1 + M_PI_2;
		Angle_FirstScndQuad(slopear);
		//calculate the intersection of perpendicular to line1//
		intersection(slope1,intercept1,mousept,endpt1 + 2);
		intersection(slope2,intercept2,mousept,endpt2 + 2);
		if(intercept1 > intercept2)
			Pt2ptdis2(endpt1 + 2, endpt2 + 2, space, dis, slopear, midpt, endpt1, endpt2);
		else
			Pt2ptdis1(endpt1 + 2, endpt2 + 2, space, dis, slopear, midpt, endpt1, endpt2);
		*(endpt1 + 4) = *(endpt1 + 2);
		*(endpt1 + 5) = *(endpt1 + 3);
		*(endpt2 + 4) = *(endpt2 + 2);
		*(endpt2 + 5) = *(endpt2 + 3);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0012", __FILE__, __FUNCSIG__); return false; }
}


//function to dimension line to line distace//
bool RapidMath2D::Finiteline_FinitelineDistance(double *endp1,double *endp2,double *endp3,double *endp4, double slope1,double intercept1,double slope2,double intercept2, double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	try
	{
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		*slopear = slope1 + M_PI_2;
		Angle_FirstScndQuad(slopear);
		//calculate the intersection of perpendicular to line1//
		intersection(slope1,intercept1,mousept,endpt1 + 2);
		intersection(slope2,intercept2,mousept,endpt2 + 2);
		if(intercept1 > intercept2)
			Pt2ptdis2(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		else
			Pt2ptdis1(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		if(Ptisinwindow(endpt1 + 2,endp1, endp2))
		{
			*(endpt1 + 4) = *(endpt1 + 2);
			*(endpt1 + 5) = *(endpt1 + 3);
		}
		else
		{
			if(Pt2Pt_distance(endpt1 + 2, endp1) < Pt2Pt_distance(endpt1 + 2, endp2))
			{
				*(endpt1 + 4) = *endp1; 
				*(endpt1 + 5) = *(endp1 + 1);
			}
			else
			{
				*(endpt1 + 4) = *endp2; 
				*(endpt1 + 5) = *(endp2 + 1);
			}
		}
		if(Ptisinwindow(endpt2 + 2, endp3, endp4))
		{
			*(endpt2 + 4) = *(endpt2 + 2);
			*(endpt2 + 5) = *(endpt2 + 3);
		}
		else
		{
			if(Pt2Pt_distance(endpt2 + 2, endp3) < Pt2Pt_distance(endpt2 + 2, endp4))
			{
				*(endpt2 + 4) = *endp3; 
				*(endpt2 + 5) = *(endp3 + 1);
			}
			else
			{
				*(endpt2 + 4) = *endp4; 
				*(endpt2 + 5) = *(endp4 + 1);
			}
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0013", __FILE__, __FUNCSIG__); return false; }
}

//Ray to ray distance calculations..//
bool RapidMath2D::Ray_RayDistance( double *point1,double slope1, double intercept1, double *point2,double slope2,double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear )
{
	double sloperay1, sloperay2;
	try
	{
		sloperay1 = slope1;
		sloperay2 = slope2;
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		*slopear = slope1 + M_PI_2;
		Angle_FirstScndQuad(slopear);
		//calculate the intersection of perpendicular to line1//
		intersection(slope1,intercept1,mousept,endpt1 + 2);
		intersection(slope2,intercept2,mousept,endpt2 + 2);
		if(intercept1 > intercept2)
			Pt2ptdis2(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		else
			Pt2ptdis1(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		double tempangle1 = ray_angle(endpt1 + 2,point1), tempangle2 = ray_angle(endpt2 + 2,point2);
		if(RoundDecimal(tempangle1, 4) == RoundDecimal(sloperay1, 4))
		{
			*(endpt1 + 4) = *point1; 
			*(endpt1 + 5) = *(point1 + 1);
		}
		else
		{
			*(endpt1 + 4) = *(endpt1 + 2);
			*(endpt1 + 5) = *(endpt1 + 3);
		}
		if(RoundDecimal(tempangle2, 4) == RoundDecimal(sloperay2, 4))
		{
			*(endpt2 + 4) = *point2; 
			*(endpt2 + 5) = *(point2 + 1);
		}
		else
		{
			*(endpt2 + 4) = *(endpt2 + 2);
			*(endpt2 + 5) = *(endpt2 + 3);
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0014", __FILE__, __FUNCSIG__); return false; }
}

//Line to finite line distance..//
bool RapidMath2D::Infiniteline_FinitelineDistance( double slope1,double intercept1,double *endp1,double *endp2,double slope2, double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear )
{
	try
	{
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		*slopear = slope1 + M_PI_2;
		Angle_FirstScndQuad(slopear);
		//calculate the intersection of perpendicular to line1//
		intersection(slope1,intercept1,mousept,endpt1 + 2);
		intersection(slope2,intercept2,mousept,endpt2 + 2);
		if(intercept1 > intercept2)
			Pt2ptdis2(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		else
			Pt2ptdis1(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		if(Ptisinwindow(endpt1 + 2, endp1, endp2))
		{
			*(endpt1 + 4) = *(endpt1 + 2);
			*(endpt1 + 5) = *(endpt1 + 3);
		}
		else
		{
			if(Pt2Pt_distance(endpt1 + 2, endp1) < Pt2Pt_distance(endpt1 + 2, endp2))
			{
				*(endpt1 + 4) = *endp1; 
				*(endpt1 + 5) = *(endp1 + 1);
			}
			else
			{
				*(endpt1 + 4) = *endp2; 
				*(endpt1 + 5) = *(endp2 + 1);
			}
		}
		*(endpt2 + 4) = *(endpt2 + 2);
		*(endpt2 + 5) = *(endpt2 + 3);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0015", __FILE__, __FUNCSIG__); return false; }
}

//Line to ray distance..//
bool RapidMath2D::Infiniteline_RayDistance( double slope1,double intercept1,double *point,double slope2,double intercept2, double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear )
{
	double sloperay;
	try
	{
		sloperay = slope2;
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		*slopear = slope1 + M_PI_2;
		Angle_FirstScndQuad(slopear);
		//calculate the intersection of perpendicular to line1//
		intersection(slope1,intercept1,mousept,endpt1 + 2);
		intersection(slope2,intercept2,mousept,endpt2 + 2);
		if(intercept1 > intercept2)
			Pt2ptdis2(endpt1 + 2, endpt2 + 2, space, dis, slopear, midpt, endpt1, endpt2);
		else
			Pt2ptdis1(endpt1 + 2, endpt2 + 2, space, dis, slopear, midpt, endpt1, endpt2);
		double tempangle = ray_angle(endpt2 + 2,point);
		if(RoundDecimal(tempangle, 4) == RoundDecimal(sloperay, 4))
		{
			endpt2[4] = point[0]; 
			endpt2[5] = point[1];
		}
		else
		{
			endpt2[4] = endpt2[2];
			endpt2[4] = endpt2[3];
		}
		endpt1[4] = endpt1[2];
		endpt1[5] = endpt1[3];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0016", __FILE__, __FUNCSIG__); return false; }
}

//Finite line to ray distance....//
bool RapidMath2D::Finiteline_RayDistance(double *endp1,double *endp2, double slope1, double intercept1, double *point,double slope2, double intercept2, double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear )
{
	double sloperay;
	try
	{
		sloperay = slope2;
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		*slopear = slope1 + M_PI_2;
		Angle_FirstScndQuad(slopear);
		//calculate the intersection of perpendicular to line1//
		intersection(slope1,intercept1,mousept,endpt1 + 2);
		intersection(slope2,intercept2,mousept,endpt2 + 2);
		if(intercept1 > intercept2)
			Pt2ptdis2(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		else
			Pt2ptdis1(endpt1 + 2,endpt2 + 2,space, dis,slopear,midpt,endpt1,endpt2);
		double tempangle1 = ray_angle(endpt2 + 2,point);
		if(RoundDecimal(tempangle1, 4) == RoundDecimal(sloperay, 4))
		{
			*(endpt2 + 4) = *point; 
			*(endpt2 + 5) = *(point + 1);
		}
		else
		{
			*(endpt2 + 4) = *(endpt2 + 2);
			*(endpt2 + 5) = *(endpt2 + 3);
		}
		if(Ptisinwindow(endpt1 + 2, endp1, endp2))
		{
			*(endpt1 + 4) = *(endpt1 + 2);
			*(endpt1 + 5) = *(endpt1 + 3);
		}
		else
		{
			if(Pt2Pt_distance(endpt1 + 2, endp1) < Pt2Pt_distance(endpt1 + 2, endp2))
			{
				*(endpt1 + 4) = *endp1; 
				*(endpt1 + 5) = *(endp1 + 1);
			}
			else
			{
				*(endpt1 + 4) = *endp2; 
				*(endpt1 + 5) = *(endp2 + 1);
			}
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0017", __FILE__, __FUNCSIG__); return false; }
}


//
//function to dimension the line to line angle//
bool RapidMath2D::Finiteline_FinitelineAngle(double slope1, double intercept1, double *endpt11, double *endpt12, double slope2, double intercept2, double *endpt21, double *endpt22,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear)
{
	double angles[4], mpslope,endang;
	int i;
	double temp,temp1,temp2,spangle,endangle,sweepangle,startangle;
	try
	{
		if(slope1 == slope2)
			return false;
		//If slope greater than 180 convert it to 0 to 180//
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		//intrersection of two lines//
		Line_lineintersection(slope1,intercept1,slope2,intercept2,center);
		if(slope1 > slope2)
		{
			temp = slope1;
			slope1 = slope2;
			slope2 = temp;
			swaptwopoints(&endpt11,&endpt21);
			swaptwopoints(&endpt12,&endpt22);
		}
		//all four angles set up ascending order
		angles[0] = slope1;
		angles[1] = slope2;
		angles[2] = slope1 + M_PI;
		angles[3] = slope2 + M_PI;
		//slope of the line joining the mouse pointer and center//
		mpslope = atan2((*(mousept + 1) - *(center + 1)), (*mousept - *center));
		if (mpslope < 0)
			mpslope += M_PI * 2;
		//check the position of mouse pointer//
		for (i = 0; i < 4; i ++)
		{
			if (angles[i] > mpslope){ break;}
		}
		//start and end angle with respect to mouse position//
		if (i > 0 && i < 4)
		{
			startangle = angles[i - 1];
			sweepangle = angles[i] - angles[i - 1];
			if(i == 2)
			{
				swaptwopoints(&endpt11,&endpt21);
				swaptwopoints(&endpt12,&endpt22);
			}
		}
		else
		{
			angles[0] += M_PI*2;
			startangle = angles[3];
			sweepangle = angles[0] - angles[3];
			swaptwopoints(&endpt11,&endpt21);
			swaptwopoints(&endpt12,&endpt22);
		}
		//calculate the radius//
	   *radius = Pt2Pt_distance(center, mousept);
	   endangle = startangle + sweepangle;
	   endang = endangle;
	   //caluclate the mid point to dislpay the value//
	   temp = (startangle + endangle)/2;
	   temp1  = startangle;
	   temp2 = endangle;
	   if(endangle > 2*M_PI)
		   temp2 = endangle - 2 * M_PI;
	   //Calculate the space according the aligment of start and end angles//
	   if(((abs(temp1 - (M_PI - temp2))) < (35*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (35*M_PI/180)))
			spangle = atan((space[0]/2 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (50*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (50*M_PI/180)))
			spangle = atan((space[0]/3 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (70*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (70*M_PI/180)))
			spangle = atan((space[0]/4 + space[1]/2) / *radius);
	   else
			spangle = atan((space[1] + space[1]/2) / *radius);
	  //start angle and end angle with respect space of the string//
	   *startang = startangle;
	   *sweepang  = (temp  - (startangle + spangle));
	   *(startang + 1) = temp  + spangle;
	   *(sweepang + 1)  = (endangle  - *(startang + 1));
	   if(endangle > 2*M_PI)
		   endangle -= 2*M_PI;
	   if(temp > 2*M_PI)
		   temp -= 2*M_PI;
	   //Calculate the mid point to display value// 
	   *points  = *center + *radius * (cos(temp));
	   *(points + 1)  = *(center + 1) + *radius * (sin(temp));
	   //points to draw arrow
	   *(points + 2)  = *center + *radius * (cos(startangle));
	   *(points + 3)  = *(center + 1) + *radius * (sin(startangle));
	   //Point to draw arrow at angle//
	   *(points + 6)  = *center + *radius * (cos(endangle));
	   *(points + 7)  = *(center + 1) + *radius * (sin(endangle));
		if(Ptisinwindow(points + 2, endpt11, endpt12))
		{
			*(points + 4) = *(points + 2);
			*(points + 5) = *(points + 3);
		}
		else
		{
			if(Pt2Pt_distance(points + 2, endpt11) < Pt2Pt_distance(points + 2, endpt12))
			{
				*(points + 4) = *endpt11; 
				*(points + 5) = *(endpt11 + 1);
			}
			else
			{
				*(points + 4) = *endpt12; 
				*(points + 5) = *(endpt12 + 1);
			}
		}
		if(Ptisinwindow(points + 6, endpt21, endpt22))
		{
			*(points + 8) = *(points + 6);
			*(points + 9) = *(points + 7);
		}
		else
		{
			if(Pt2Pt_distance(points + 6, endpt21) < Pt2Pt_distance(points + 6, endpt22))
			{
				*(points + 8) = *endpt21; 
				*(points + 9) = *(endpt21 + 1);
			}
			else
			{
				*(points + 8) = *endpt22; 
				*(points + 9) = *(endpt22 + 1);
			}
		}
		*slopear = startangle + M_PI_2;
		*(slopear + 1) = endang - M_PI_2;
	   return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0018", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::PCDAngleCalculation(double slope1, double intercept1, double slope2, double intercept2, double *center, double *mousepoint, bool drawdir,double *space,double *radius, double *startang, double *sweepang, double *points, double *slopear)
{
	try
	{
		if(slope1 == slope2)
			return false;
		double temp, temp1, temp2, spangle, endangle, endang, sweepangle, startangle;
		
		*radius = Pt2Pt_distance(center, mousepoint);
		if(drawdir)
		{
			startangle = slope1;
			sweepangle = slope2 - slope1;
		}
		else
		{
			startangle = slope2;
			sweepangle = 2 * M_PI - (slope2 - slope1);
		}
	   endangle = startangle + sweepangle;
	   endang = endangle;
	   //caluclate the mid point to dislpay the value//
	   temp = (startangle + endangle)/2;
	   temp1  = startangle;
	   temp2 = endangle;
	   if(endangle > 2*M_PI)
		   temp2 = endangle - 2 * M_PI;
	   //Calculate the space according the aligment of start and end angles//
	   if(((abs(temp1 - (M_PI - temp2))) < (35*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (35*M_PI/180)))
			spangle = atan((space[0]/2 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (50*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (50*M_PI/180)))
			spangle = atan((space[0]/3 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (70*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (70*M_PI/180)))
			spangle = atan((space[0]/4 + space[1]/2) / *radius);
	   else
			spangle = atan((space[1] + space[1]/2) / *radius);
	  //start angle and end angle with respect space of the string//
	   startang[0] = startangle;
	   sweepang[0]  = (temp  - (startangle + spangle));
	   startang[1] = temp  + spangle;
	   sweepang[1]  = endangle  - startang[1];
	   if(endangle > 2*M_PI)
		   endangle -= 2*M_PI;
	   if(temp > 2*M_PI)
		   temp -= 2*M_PI;
	   //Calculate the mid point to display value// 
	   points[0]  = center[0] + *radius * cos(temp);
	   points[1]  = center[1] + *radius * sin(temp);
	   //points to draw arrow
	   points[2]  = center[0] + *radius * cos(startangle);
	   points[3]  = center[1] + *radius * sin(startangle);
	   //Point to draw arrow at angle//
	   points[6]  = center[0] + *radius * cos(endangle);
	   points[7]  = center[1] + *radius * sin(endangle);
	   points[4] = center[0];
	   points[5] = center[1];
	   points[8] = center[0];
	   points[9] = center[1];
	   slopear[0] = startangle + M_PI_2;
	   slopear[1] = endang - M_PI_2;
	   return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0019", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Infiniteline_InfinitelineAngle(double slope1, double intercept1, double slope2, double intercept2, double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear)
{
	double angles[4], mpslope,endang;
	int i;
	double temp,temp1,temp2,spangle,endangle,sweepangle,startangle;
	try
	{
		if(slope1 == slope2)
			return false;
		//If slope greater than 180 convert it to 0 to 180//
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		//intrersection of two lines//
		Line_lineintersection(slope1,intercept1,slope2,intercept2,center);
		if(slope1 > slope2)
		{
			temp = slope1;
			slope1 = slope2;
			slope2 = temp;
		}
		//all four angles set up ascending order
		angles[0] = slope1;
		angles[1] = slope2;
		angles[2] = slope1 + M_PI;
		angles[3] = slope2 + M_PI;
		//slope of the line joining the mouse pointer and center//
		mpslope = atan2((*(mousept + 1) - *(center + 1)), (*mousept - *center));
		if (mpslope < 0)
			mpslope += M_PI * 2;
		//check the position of mouse pointer//
		for (i = 0; i < 4; i ++)
		{
			if (angles[i] > mpslope){ break;}
		}
		//start and end angle with respect to mouse position//
		if (i > 0 && i < 4)
		{
			startangle = angles[i - 1];
			sweepangle = angles[i] - angles[i - 1];
		}
		else
		{
			angles[0] += M_PI*2;
			startangle = angles[3];
			sweepangle = angles[0] - angles[3];
		}
		//calculate the radius//
	   *radius = Pt2Pt_distance(center, mousept);
	   endangle = startangle + sweepangle;
	   endang = endangle;
	   //caluclate the mid point to dislpay the value//
	   temp = (startangle + endangle)/2;
	   temp1  = startangle;
	   temp2 = endangle;
	   if(endangle > 2*M_PI)
		   temp2 = endangle - 2 * M_PI;
		//Calculate the space according the aligment of start and end angles//
		if(((abs(temp1 - (M_PI - temp2))) < (35*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (35*M_PI/180)))
			spangle = atan((space[0]/2 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (50*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (50*M_PI/180)))
			spangle = atan((space[0]/3 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (70*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (70*M_PI/180)))
			spangle = atan((space[0]/4 + space[1]/2) / *radius);
	   else
			spangle = atan((space[1] + space[1]/2) / *radius);
	  //start angle and end angle with respect space of the string//
	   *startang = startangle;
	   *sweepang  = (temp  - (startangle + spangle));
	   *(startang + 1) = temp  + spangle;
	   *(sweepang + 1)  = (endangle  - *(startang + 1));
	   if(endangle > 2*M_PI)
		   endangle -= 2*M_PI;
	   if(temp > 2*M_PI)
		   temp -= 2*M_PI;
	   //Calculate the mid point to display value// 
	   *points  = *center + *radius * (cos(temp));
	   *(points + 1)  = *(center + 1) + *radius * (sin(temp));
	   //points to draw arrow
	   *(points + 2)  = *center + *radius * (cos(startangle));
	   *(points + 3)  = *(center + 1) + *radius * (sin(startangle));
	   //Point to draw arrow at angle//
	   *(points + 6)  = *center + *radius * (cos(endangle));
	   *(points + 7)  = *(center + 1) + *radius * (sin(endangle));
		*(points + 4) = *(points + 2);
		*(points + 5) = *(points + 3);
		*(points + 8) = *(points + 6);
		*(points + 9) = *(points + 7);
		*slopear = startangle + M_PI_2;
		*(slopear + 1) = endang - M_PI_2;
	   return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0020", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Ray_RayAngle(double slope1, double intercept1, double *endpt1,double slope2, double intercept2, double *endpt2,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear)
{
	double angles[4], mpslope,endang;
	int i;
	double temp,temp1,temp2,spangle,endangle,sweepangle,startangle;
	double slopeofray1,slopeofray2;
	try
	{
		if(slope1 == slope2)
			return false;
		//If slope greater than 180 convert it to 0 to 180//
		slopeofray1 = slope1;
		slopeofray2 = slope2;
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		//intrersection of two lines//
		Line_lineintersection(slope1,intercept1,slope2,intercept2,center);
		if(slope1 > slope2)
		{
			SwapTwoValues(&slope1, &slope2);
			SwapTwoValues(&slopeofray1, &slopeofray2);
			swaptwopoints(&endpt1,&endpt2);
		}
		//all four angles set up ascending order
		angles[0] = slope1;
		angles[1] = slope2;
		angles[2] = slope1 + M_PI;
		angles[3] = slope2 + M_PI;
		//slope of the line joining the mouse pointer and center//
		mpslope = atan2((*(mousept + 1) - *(center + 1)), (*mousept - *center));
		if (mpslope < 0)
			mpslope += M_PI * 2;
		//check the position of mouse pointer//
		for (i = 0; i < 4; i ++)
		{
			if (angles[i] > mpslope){ break;}
		}
		//start and end angle with respect to mouse position//
		if (i > 0 && i < 4)
		{
			startangle = angles[i - 1];
			sweepangle = angles[i] - angles[i - 1];
			if(i == 2)
			{
				swaptwopoints(&endpt1,&endpt2);
				SwapTwoValues(&slopeofray1, &slopeofray2);
			}
		}
		else
		{
			angles[0] += M_PI*2;
			startangle = angles[3];
			sweepangle = angles[0] - angles[3];
			swaptwopoints(&endpt1,&endpt2);
			SwapTwoValues(&slopeofray1, &slopeofray2);
		}
		//calculate the radius//
	   *radius = Pt2Pt_distance(center, mousept);
	   endangle = startangle + sweepangle;
	   endang = endangle;
	   //caluclate the mid point to dislpay the value//
	   temp = (startangle + endangle)/2;
	   temp1  = startangle;
	   temp2 = endangle;
	   if(endangle > 2*M_PI)
		   temp2 = endangle - 2 * M_PI;
	   //Calculate the space according the aligment of start and end angles//
	   if(((abs(temp1 - (M_PI - temp2))) < (35*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (35*M_PI/180)))
			spangle = atan((space[0]/2 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (50*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (50*M_PI/180)))
			spangle = atan((space[0]/3 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (70*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (70*M_PI/180)))
			spangle = atan((space[0]/4 + space[1]/2) / *radius);
	   else
			spangle = atan((space[1] + space[1]/2) / *radius);
	  //start angle and end angle with respect space of the string//
	   *startang = startangle;
	   *sweepang  = (temp  - (startangle + spangle));
	   *(startang + 1) = temp  + spangle;
	   *(sweepang + 1)  = (endangle  - *(startang + 1));
	   if(endangle > 2*M_PI)
		   endangle -= 2*M_PI;
	   if(temp > 2*M_PI)
		   temp -= 2*M_PI;
	   //Calculate the mid point to display value// 
	   *points  = *center + *radius * (cos(temp));
	   *(points + 1)  = *(center + 1) + *radius * (sin(temp));
	   //points to draw arrow
	   *(points + 2)  = *center + *radius * (cos(startangle));
	   *(points + 3)  = *(center + 1) + *radius * (sin(startangle));
	   //Point to draw arrow at angle//
	   *(points + 6)  = *center + *radius * (cos(endangle));
	   *(points + 7)  = *(center + 1) + *radius * (sin(endangle));
		double tempangle1 = ray_angle(endpt1, points + 2), tempangle2 = ray_angle(endpt2, points + 6);
		if(RoundDecimal(tempangle1, 4) == RoundDecimal(slopeofray1, 4))
		{
			*(points + 4) = *(points + 2);
			*(points + 5) = *(points + 3);
		}
		else
		{
			*(points + 4) = *endpt1;
			*(points + 5) = *(endpt1 + 1);
		}
		if(RoundDecimal(tempangle2, 4) == RoundDecimal(slopeofray2, 4))
		{
			*(points + 8) = *(points + 6);
			*(points + 9) = *(points + 7);
		}
		else
		{
			*(points + 8) = *endpt2;
			*(points + 9) = *(endpt2 + 1);
		}
		*slopear = startangle + M_PI_2;
		*(slopear + 1) = endang - M_PI_2;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0021", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Infiniteline_FinitelineAngle(double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2, double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear)
{
	double angles[4], mpslope,endang;
	int i;
	double temp,temp1,temp2,spangle,endangle,sweepangle,startangle;
	bool swapflag = false;
	try
	{
		if(slope1 == slope2)
			return false;
		//If slope greater than 180 convert it to 0 to 180//
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		//intrersection of two lines//
		Line_lineintersection(slope1,intercept1,slope2,intercept2,center);
		if(slope1 > slope2)
		{
			SwapTwoValues(&slope1, &slope2);
			swapflag = !swapflag;
		}
		//all four angles set up ascending order
		angles[0] = slope1;
		angles[1] = slope2;
		angles[2] = slope1 + M_PI;
		angles[3] = slope2 + M_PI;
		//slope of the line joining the mouse pointer and center//
		mpslope = atan2((*(mousept + 1) - *(center + 1)), (*mousept - *center));
		if (mpslope < 0)
			mpslope += M_PI * 2;
		//check the position of mouse pointer//
		for (i = 0; i < 4; i ++)
		{
			if (angles[i] > mpslope){ break;}
		}
		//start and end angle with respect to mouse position//
		if (i > 0 && i < 4)
		{
			startangle = angles[i - 1];
			sweepangle = angles[i] - angles[i - 1];
			if(i == 2)
				swapflag = !swapflag;
		}
		else
		{
			angles[0] += M_PI*2;
			startangle = angles[3];
			sweepangle = angles[0] - angles[3];
			swapflag = !swapflag;
		}
		//calculate the radius//
	   *radius = Pt2Pt_distance(center, mousept);
	   endangle = startangle + sweepangle;
	   endang = endangle;
	   //caluclate the mid point to dislpay the value//
	   temp = (startangle + endangle)/2;
	   temp1  = startangle;
	   temp2 = endangle;
	   if(endangle > 2*M_PI)
		   temp2 = endangle - 2 * M_PI;
		//Calculate the space according the aligment of start and end angles//
	   if(((abs(temp1 - (M_PI - temp2))) < (35*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (35*M_PI/180)))
			spangle = atan((space[0]/2 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (50*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (50*M_PI/180)))
			spangle = atan((space[0]/3 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (70*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (70*M_PI/180)))
			spangle = atan((space[0]/4 + space[1]/2) / *radius);
	   else
			spangle = atan((space[1] + space[1]/2) / *radius);
	  //start angle and end angle with respect space of the string//
	   *startang = startangle;
	   *sweepang  = (temp  - (startangle + spangle));
	   *(startang + 1) = temp  + spangle;
	   *(sweepang + 1)  = (endangle  - *(startang + 1));
	   if(endangle > 2*M_PI)
		   endangle -= 2*M_PI;
	   if(temp > 2*M_PI)
		   temp -= 2*M_PI;
	   //Calculate the mid point to display value// 
	   *points  = *center + *radius * (cos(temp));
	   *(points + 1)  = *(center + 1) + *radius * (sin(temp));
	   //points to draw arrow
	   *(points + 2)  = *center + *radius * (cos(startangle));
	   *(points + 3)  = *(center + 1) + *radius * (sin(startangle));
	   //Point to draw arrow at angle//
	   *(points + 6)  = *center + *radius * (cos(endangle));
	   *(points + 7)  = *(center + 1) + *radius * (sin(endangle));
	   if(!swapflag)
	   {
			if(Ptisinwindow(points + 2, endpt1, endpt2))
			{
				*(points + 4) = *(points + 2);
				*(points + 5) = *(points + 3);
			}
			else
			{
				if(Pt2Pt_distance(points + 2, endpt1) < Pt2Pt_distance(points + 2, endpt2))
				{
					*(points + 4) = *endpt1; 
					*(points + 5) = *(endpt1 + 1);
				}
				else
				{
					*(points + 4) = *endpt2; 
					*(points + 5) = *(endpt2 + 1);
				}
			}
			*(points + 8) = *(points + 6);
			*(points + 9) = *(points + 7);
	   }
	   else
	   {
		   if(Ptisinwindow(points + 6, endpt1, endpt2))
			{
				*(points + 8) = *(points + 6);
				*(points + 9) = *(points + 7);
			}
			else
			{
				if(Pt2Pt_distance(points + 6, endpt1) < Pt2Pt_distance(points + 6, endpt2))
				{
					*(points + 8) = *endpt1; 
					*(points + 9) = *(endpt1 + 1);
				}
				else
				{
					*(points + 8) = *endpt2; 
					*(points + 9) = *(endpt2 + 1);
				}
			}
			*(points + 4) = *(points + 2);
			*(points + 5) = *(points + 3);
	   }
		*slopear = startangle + M_PI_2;
		*(slopear + 1) = endang - M_PI_2;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0022", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Infiniteline_RayAngle(double slope1, double intercept1, double *endpt1,double slope2, double intercept2,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear)
{
	double angles[4], mpslope,endang;
	int i;
	double temp,temp1,temp2,spangle,endangle,sweepangle,startangle;
	double slopeofray;
	bool swapflag = false;
	try
	{
		if(slope1 == slope2)
			return false;
		slopeofray = slope1;
		//If slope greater than 180 convert it to 0 to 180//
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		//intrersection of two lines//
		Line_lineintersection(slope1,intercept1,slope2,intercept2,center);
		if(slope1 > slope2)
		{
			SwapTwoValues(&slope1, &slope2);
			swapflag = !swapflag;
		}
		//all four angles set up ascending order
		angles[0] = slope1;
		angles[1] = slope2;
		angles[2] = slope1 + M_PI;
		angles[3] = slope2 + M_PI;
		//slope of the line joining the mouse pointer and center//
		mpslope = atan2((*(mousept + 1) - *(center + 1)), (*mousept - *center));
		if(mpslope < 0)
			mpslope += M_PI * 2;
		//check the position of mouse pointer//
		for (i = 0; i < 4; i ++)
		{
			if (angles[i] > mpslope){ break;}
		}
		//start and end angle with respect to mouse position//
		if (i > 0 && i < 4)
		{
			startangle = angles[i - 1];
			sweepangle = angles[i] - angles[i - 1];
			if(i == 2)
				swapflag = !swapflag;
		}
		else
		{
			angles[0] += M_PI*2;
			startangle = angles[3];
			sweepangle = angles[0] - angles[3];
			swapflag = !swapflag;
		}
		//calculate the radius//
	   *radius = Pt2Pt_distance(center, mousept);
	   endangle = startangle + sweepangle;
	   endang = endangle;
	   //caluclate the mid point to dislpay the value//
	   temp = (startangle + endangle)/2;
	   temp1  = startangle;
	   temp2 = endangle;
	   if(endangle > 2*M_PI)
		   temp2 = endangle - 2 * M_PI;
		//Calculate the space according the aligment of start and end angles//
		if(((abs(temp1 - (M_PI - temp2))) < (35*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (35*M_PI/180)))
			spangle = atan((space[0]/2 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (50*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (50*M_PI/180)))
			spangle = atan((space[0]/3 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (70*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (70*M_PI/180)))
			spangle = atan((space[0]/4 + space[1]/2) / *radius);
	   else
			spangle = atan((space[1] + space[1]/2) / *radius);
	  //start angle and end angle with respect space of the string//
	   *startang = startangle;
	   *sweepang  = (temp  - (startangle + spangle));
	   *(startang + 1) = temp  + spangle;
	   *(sweepang + 1)  = (endangle  - *(startang + 1));
	   if(endangle > 2*M_PI)
		   endangle -= 2*M_PI;
	   if(temp > 2*M_PI)
		   temp -= 2*M_PI;
	   //Calculate the mid point to display value// 
	   *points  = *center + *radius * (cos(temp));
	   *(points + 1)  = *(center + 1) + *radius * (sin(temp));
	   //points to draw arrow
	   *(points + 2)  = *center + *radius * (cos(startangle));
	   *(points + 3)  = *(center + 1) + *radius * (sin(startangle));
	   //Point to draw arrow at angle//
	   *(points + 6)  = *center + *radius * (cos(endangle));
	   *(points + 7)  = *(center + 1) + *radius * (sin(endangle));
	   if(!swapflag)
	   {
			double tempangle = ray_angle(endpt1, points + 2);
			if(RoundDecimal(tempangle, 4) == RoundDecimal(slopeofray, 4))
			{
				*(points + 4) = *(points + 2);
				*(points + 5) = *(points + 3);
			}
			else
			{
				*(points + 4) = *endpt1;
				*(points + 5) = *(endpt1 + 1);
			}
			*(points + 8) = *(points + 6);
			*(points + 9) = *(points + 7);
	   }
	   else
	   {
		   double tempangle = ray_angle(endpt1,points + 6);
			if(RoundDecimal(tempangle, 4) == RoundDecimal(slopeofray, 4))
			{
				*(points + 8) = *(points + 6);
				*(points + 9) = *(points + 7);
			}
			else
			{
				*(points + 8) = *endpt1;
				*(points + 9) = *(endpt1 + 1);
			}
			*(points + 4) = *(points + 2);
			*(points + 5) = *(points + 3);
	   }
		*slopear = startangle + M_PI_2;
		*(slopear + 1) = endang - M_PI_2;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0023", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Finiteline_RayAngle(double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2, double *endpt3,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear)
{
	double angles[4], mpslope,endang;
	int i;
	double temp,temp1,temp2,spangle,endangle,sweepangle,startangle;
	double slopeofray;
	bool swapflag = false;
	try
	{
		if(slope1 == slope2)
			return false;
		slopeofray = slope2;
		//If slope greater than 180 convert it to 0 to 180//
		Angle_FirstScndQuad(&slope1);
		Angle_FirstScndQuad(&slope2);
		//intrersection of two lines//
		Line_lineintersection(slope1,intercept1,slope2,intercept2,center);
		if(slope1 > slope2)
		{
			SwapTwoValues(&slope1, &slope2);
			swapflag = !swapflag;
		}
		//all four angles set up ascending order
		angles[0] = slope1;
		angles[1] = slope2;
		angles[2] = slope1 + M_PI;
		angles[3] = slope2 + M_PI;
		//slope of the line joining the mouse pointer and center//
		mpslope = atan2((*(mousept + 1) - *(center + 1)), (*mousept - *center));
		if(mpslope < 0)
			mpslope += M_PI * 2;
		//check the position of mouse pointer//
		for (i = 0; i < 4; i ++)
		{
			if (angles[i] > mpslope){ break;}
		}
		//start and end angle with respect to mouse position//
		if (i > 0 && i < 4)
		{
			startangle = angles[i - 1];
			sweepangle = angles[i] - angles[i - 1];
			if(i == 2)
				swapflag = !swapflag;
		}
		else
		{
			angles[0] += M_PI*2;
			startangle = angles[3];
			sweepangle = angles[0] - angles[3];
			swapflag = !swapflag;
		}
		//calculate the radius//
	   *radius = Pt2Pt_distance(center, mousept);
	   endangle = startangle + sweepangle;
	   endang = endangle;
	   //caluclate the mid point to dislpay the value//
	   temp = (startangle + endangle)/2;
	   temp1  = startangle;
	   temp2 = endangle;
	   if(endangle > 2*M_PI)
		   temp2 = endangle - 2 * M_PI;
	   //Calculate the space according the aligment of start and end angles//
		if(((abs(temp1 - (M_PI - temp2))) < (35*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (35*M_PI/180)))
			spangle = atan((space[0]/2 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (50*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (50*M_PI/180)))
			spangle = atan((space[0]/3 + space[1]/2) / *radius);
	   else if(((abs(temp1 - (M_PI - temp2))) < (70*M_PI/180)) ||((abs(M_PI - (temp1 - (2 * M_PI - temp2)))) < (70*M_PI/180)))
			spangle = atan((space[0]/4 + space[1]/2) / *radius);
	   else
			spangle = atan((space[1] + space[1]/2) / *radius);
	  //start angle and end angle with respect space of the string//
	   *startang = startangle;
	   *sweepang  = (temp  - (startangle + spangle));
	   *(startang + 1) = temp  + spangle;
	   *(sweepang + 1)  = (endangle  - *(startang + 1));
	   if(endangle > 2*M_PI)
		   endangle -= 2*M_PI;
	   if(temp > 2*M_PI)
		   temp -= 2*M_PI;
	   //Calculate the mid point to display value// 
	   *points  = *center + *radius * (cos(temp));
	   *(points + 1)  = *(center + 1) + *radius * (sin(temp));
	   //points to draw arrow
	   *(points + 2)  = *center + *radius * (cos(startangle));
	   *(points + 3)  = *(center + 1) + *radius * (sin(startangle));
	   //Point to draw arrow at angle//
	   *(points + 6)  = *center + *radius * (cos(endangle));
	   *(points + 7)  = *(center + 1) + *radius * (sin(endangle));
	   if(!swapflag)
	   {
			if(Ptisinwindow(points + 2, endpt1, endpt2))
			{
				*(points + 4) = *(points + 2);
				*(points + 5) = *(points + 3);
			}
			else
			{
				if(Pt2Pt_distance(points + 2, endpt1) < Pt2Pt_distance(points + 2, endpt2))
				{
					*(points + 4) = *endpt1; 
					*(points + 5) = *(endpt1 + 1);
				}
				else
				{
					*(points + 4) = *endpt2; 
					*(points + 5) = *(endpt2 + 1);
				}
			}
			double tempangle = ray_angle(endpt3, points + 6);
			if(RoundDecimal(tempangle, 4) == RoundDecimal(slopeofray, 4))
			{
				*(points + 8) = *(points + 6);
				*(points + 9) = *(points + 7);
			}
			else
			{
				*(points + 8) = *endpt3;
				*(points + 9) = *(endpt3 + 1);
			}
	   }
	   else
	   {
		   if(Ptisinwindow(points + 6, endpt1, endpt2))
			{
				*(points + 8) = *(points + 6);
				*(points + 9) = *(points + 7);
			}
			else
			{
				if(Pt2Pt_distance(points + 6, endpt1) < Pt2Pt_distance(points + 6, endpt2))
				{
					*(points + 8) = *endpt1; 
					*(points + 9) = *(endpt1 + 1);
				}
				else
				{
					*(points + 8) = *endpt2; 
					*(points + 9) = *(endpt2 + 1);
				}
			}
			 double tempangle = ray_angle(endpt3,points + 2);
			if(RoundDecimal(tempangle, 4) == RoundDecimal(slopeofray, 4))
			{
				*(points + 4) = *(points + 2);
				*(points + 5) = *(points + 3);
			}
			else
			{
				*(points + 4) = *endpt3;
				*(points + 5) = *(endpt3 + 1);
			}
	   }
		*slopear = startangle + M_PI_2;
		*(slopear + 1) = endang - M_PI_2;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0024", __FILE__, __FUNCSIG__); return false; }
}
//function to find the 4 point in circle//
bool RapidMath2D::circle_points(double *center,double radius,double slope,double *pt1,double *pt2,double *pt3,double *pt4)
{
	try
	{
		*pt1 = *center - (cos(slope)) *(radius);
		*(pt1 + 1) = *(center + 1) - (sin(slope)) *(radius);

		*pt2 = *center + (cos(slope)) * (radius);
		*(pt2 + 1) = *(center + 1) + (sin(slope)) * (radius);

		*pt3 = *center - (cos(slope)) *(radius / 2);
		*(pt3 + 1) = *(center + 1) - (sin(slope)) *(radius / 2);

		*pt4 = *center + (cos(slope)) * (radius / 2);
		*(pt4 + 1) = *(center + 1) + (sin(slope)) * (radius / 2);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0025", __FILE__, __FUNCSIG__); return false; }
}


//function to calculate the slope , intercept of given two points  and slope of perpendicular to that line// 
bool RapidMath2D::slopesintercept(double *point1,double *point2,double *mousept,double *slope, double *intercept, double *slope1)
{
	try
	{
		//slope of the line joining two points//
		//If X coordinates are equal then line is vertical// 
		if(abs(*point1 - *point2) < 0.0001)
		{
			*slope = M_PI_2;
			*slope1 = 0;
			*intercept  = *mousept;
		}
		//if the Y coordinate are equal then the line is horizontal//
		//slope = 0; slope of perpendicular  = 90//
		else if(abs(*(point1 + 1) - *(point2 + 1)) < 0.0001)
		{
			*slope = 0;
			*slope1 = M_PI_2;
			*intercept  = *(mousept + 1);
		}
		else
		{
			*slope = atan((*(point1 + 1) - *(point2 + 1)) / (*point1 - *point2));
			//slope of the perpendiclar
			*slope1 = atan(-1 / tan(*slope));
			*intercept = *(mousept + 1) - (tan(*slope)) * (*mousept);
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0026", __FILE__, __FUNCSIG__); return false; }
}


double RapidMath2D::InterceptOfline(double slope, double *point)
{
	try
	{
		double intercept = 0;
		Angle_FirstScndQuad(&slope);
		if(abs(slope - M_PI_2) < 0.00001)
			intercept = point[0];
		else
			intercept = point[1] - (tan(slope) * point[0]);
		return intercept;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0027", __FILE__, __FUNCSIG__); return false; }
}

//
//Linear mode calculations...//
bool RapidMath2D::XY_shift(double *point1, double *point2, bool CurrentStatus, double *mousept, double *space, double dis, double *endpt1, double *endpt2, double *midpt, double *slopear)
{
	double intercepts[4], d1, d2, d3, d4;
	try
	{
		swap2(point1,point2);
		intercepts[0] = point1[0]; intercepts[1] = point2[0];
		intercepts[2] = point1[1]; intercepts[3] = point2[1];
		if(intercepts[0] < intercepts[1])
		{
			double temp = intercepts[0];
			intercepts[0] = intercepts[1];
			intercepts[1] = temp;
		}
		d1 = intercepts[0] - mousept[0]; d2 = intercepts[1] - mousept[0];
		d3 = intercepts[2] - mousept[1]; d4 = intercepts[3] - mousept[1];
		if(CurrentStatus && ((d1 < 0 || d2 > 0) || (d4 > 0  && d3 < 0)))
			Y_shift(point1, point2, mousept, space, dis, endpt1, endpt2, midpt, slopear);
		else if(!CurrentStatus &&((d4 < 0 || d3 > 0) || (d1 > 0  && d2 < 0)))
			X_shift(point1, point2, mousept, space, dis, endpt1, endpt2, midpt, slopear);
		else if((d1 < 0 || d2 > 0))
			Y_shift(point1, point2, mousept, space, dis, endpt1, endpt2, midpt, slopear);
		else
			X_shift(point1,point2,mousept,space,dis,endpt1,endpt2,midpt,slopear);
		endpt2[0] = point2[0];	endpt2[1] = point2[1];
		endpt1[0] = point1[0];	endpt1[1] = point1[1];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0028", __FILE__, __FUNCSIG__); return false; }
}


//
//function to dimension the x shift between two points//
bool RapidMath2D::X_shift(double *point1,double *point2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	double temp,pt1[2],pt2[2]; //temp for swapping the points//
	try
	{
		//swap the points if point1.X is greater than the point2.X//
		if(*point1 > *point2)
		{
			temp = *point1;
			*point1 = *point2;
			*point2 = temp;
			temp = *(point1 + 1);
			*(point1 + 1) = *(point2 + 1);
			*(point2 + 1) = temp;
		}
		
		//Points exactly below or above the given points with respect to mouse position the mouse position //
		pt1[0] = *point1;
		pt2[0] = *point2;
		pt1[1] = *(point1 + 1);
		pt2[1] = *(point1 + 1);
		//call point to point distance function//
		Pt2Ptdis(&pt1[0],&pt2[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0029", __FILE__, __FUNCSIG__); return false; }
}

//
//function to dimension the Y shift measure between two points// 
bool RapidMath2D::Y_shift(double *point1,double *point2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	double temp,pt1[2],pt2[2]; //temp for swapping the points//
	try
	{
		//swap the points if point1.Y is greater than the point2.Y//
		if(*(point1 + 1) > *(point2 + 1))
		{
			temp = *point1;
			*point1 = *point2;
			*point2 = temp;
			temp = *(point1 + 1);
			*(point1 + 1) = *(point2 + 1);
			*(point2 + 1) = temp;
		}
		//points exactly right or left of the points with respecr to mouse position//
		pt1[0] = *point2;
		pt2[0] = *point2;
		pt1[1] = *(point1 + 1);
		pt2[1] = *(point2 + 1);
		//call point to point distance function//
		Pt2Ptdis(&pt1[0],&pt2[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0030", __FILE__, __FUNCSIG__); return false; }
}

//
//Linear mode with some angle..//
bool RapidMath2D::X_shiftangle(double *point1,double *point2,double slope,double intercept,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	double intercept1,pt[2];
	
	try
	{
		if(slope == M_PI_2)
		{
			pt[0] = *(point2);
			pt[1] = *(point2 + 1);

		}
		else
		{
			//line parallel to given line passing through the point1//
			intercept1 = *(point1 + 1) - (tan(slope)) * (*point1);
			//intersection of perpendicular from point2//
			intersection(slope,intercept1,point2,&pt[0]);
			//point to point distance//
		}
		 X_shift(point1,&pt[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
		 return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0031", __FILE__, __FUNCSIG__); return false; }
}

//
//Linear mode with some angle..//
bool RapidMath2D::Y_shiftangle(double *point1,double *point2,double slope,double intercept,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear)
{
	double intercept1,pt[2],slope1;
	try
	{
		if(slope == M_PI_2)
		{
			pt[0] = *(point2);
			pt[1] = *(point2 + 1);
		}
		else
		{
			slope1 = atan(-1 / tan(slope));
			//line parallel to given line passing through the point1//
			intercept1 = *(point1 + 1) - (tan(slope1)) * (*point1);
			//intersection of perpendicular from point2//
			intersection(slope1,intercept1,point2,&pt[0]);
		}
		 Y_shift(point1,&pt[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
		 return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0032", __FILE__, __FUNCSIG__); return false; }
}

//Function to dimension the measure of radius of a circle//
bool RapidMath2D::Circle_radius( double *center,double radius,double *mousept,double *endpt1,double *slopear )
{
	try
	{
		double pt1[2];
		double slope = ray_angle(center, mousept);
		//calculate the slope and intercept of the line joining the center and mouse position//
		//slopesintercept(center,mousept,center,&slope,&intercept,&slope1);
		*slopear = slope;
		//caluclate the intersection poin ts of that line with the circle//
		pt1[0] = *center + (cos(slope)) * (radius);
		pt1[1] = *(center + 1) + (sin(slope)) * (radius);
		//end points for the line//
		*endpt1 = pt1[0];
		*(endpt1 + 1) = pt1[1];
		*(endpt1 + 2) = *mousept;
		*(endpt1 + 3) = *(mousept + 1);
		*(endpt1 + 4) = (*mousept) + 10;
		*(endpt1 + 5) = *(mousept + 1);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0033", __FILE__, __FUNCSIG__); return false; }
}

//
//Function to dimension the measure of dia of a circle//
bool RapidMath2D::Circle_diameter(double *center, double dia, double *mousept, double *space, double *endpt1, double *slopear)
{
	try
	{
		double pt1[2];
		double slope = ray_angle(center, mousept);
		//calculate the slope and intercept of the line joining the center and mouse position//
		//slopesintercept(center,mousept,center,&slope,&intercept,&slope1);
		*slopear = slope;
		//caluclate the intersection poin ts of that line with the circle//
		pt1[0] = *center + (cos(slope)) * (dia / 2);
		pt1[1] = *(center + 1) + (sin(slope)) * (dia / 2);
		//call point to point distance to calculate the distance of the mouse position from the intersection poins//
		//end points for the line//
		if(slope > 0 && slope <= M_PI_2 || slope >= M_PI_2 * 3)
		{
			*endpt1 = pt1[0];
			*(endpt1 + 1) = pt1[1];
			*(endpt1 + 2) = *mousept;
			*(endpt1 + 3) = *(mousept + 1);
			*(endpt1 + 4) = *mousept + space[0]/4;
			*(endpt1 + 5) = *(mousept + 1);
			*(endpt1 + 6) = *mousept + 3 * space[0]/4;
			*(endpt1 + 7) = *(mousept + 1);
		}
		else
		{
			*endpt1 = pt1[0];
			*(endpt1 + 1) = pt1[1];
			*(endpt1 + 2) = *mousept;
			*(endpt1 + 3) = *(mousept + 1);
			*(endpt1 + 4) = *mousept - space[0]/4;
			*(endpt1 + 5) = *(mousept + 1);
			*(endpt1 + 6) = *mousept - 3 * space[0]/4;
			*(endpt1 + 7) = *(mousept + 1);
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0034", __FILE__, __FUNCSIG__); return false; }
}


//
//function to dimension arc radius//
bool RapidMath2D::Arc_radius(double *center, double radius, double *mousept, double startang, double sweepang, double *space, double *endpt1, double *LastmpX, double *LastmpY, double *slopear)
{
	double point1[8],sl,slope1,temp,temp1,temp2,endangle;
	try
	{
		//call circle radius function//
		//Circle_diameter(center,2 * radius,mousept,space,&point1[0],&sl);
		endangle = startang + sweepang;
		//calculate the angle of line joining center and point1//
		slope1 = ray_angle(center, mousept);
		if(endangle < startang)
		{
			temp = startang;
			startang  = endangle;
			endangle = temp;
		}
		if(endangle > 2*M_PI)
			endangle -= 2*M_PI;
		//angles to check the position of the points on arc//
		temp = (endangle - startang);
		temp1 = (endangle - slope1);
		temp2 = (slope1 - startang);
		arcfun(&temp);
		arcfun(&temp1);
		arcfun(&temp2);
		if(temp1 < temp && temp2 < temp)
		{
			Circle_diameter(center, 2 * radius, mousept, space, &point1[0], &sl);
			*LastmpX = mousept[0]; *LastmpY = mousept[1];
			*slopear = sl;
			for(int i = 0; i < 8; i ++)
				endpt1[i]  = point1[i];
		}
		else
		{
			double Cmp[2] = {*LastmpX, *LastmpY};
			Circle_diameter(center, 2 * radius, &Cmp[0], space, &point1[0], &sl);
			*slopear = sl;
			for(int i = 0; i < 8; i ++)
				endpt1[i]  = point1[i];
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0035", __FILE__, __FUNCSIG__); return false; }
}


//function to measure the arc angle/length//
bool RapidMath2D::Arc_Angle(double *center, double radius, double startang, double sweepang, double *endp1, double *endp2, double *mousept, double *space, double *points, double *startang1, double *sweepang1, double *r1, double *slopear)
{
	try
	{
		double temp, temp1, temp2, spangle, r, endangle;
		r = Pt2Pt_distance(center, mousept);
		endangle = startang + sweepang;
	   //caluclate the mid point to dislpay the value//
	   temp = (startang + endangle)/2;
	   //start and 
		*r1 = r;
	   temp1  = startang;
	   temp2 = endangle;
	   if(startang > M_PI)
			temp1  = startang - M_PI;
	   if(endangle > M_PI)
		   temp2 = endangle - M_PI;
	   //Calculate the space according the aligment of start and end angles//
	   if(abs(temp1 - (M_PI - temp2)) > (30*M_PI/180))
			spangle = atan((*space/2) / r);
		else
			spangle = atan((*(space + 1)/2) / r);
	  //start angle and end angle with respect space of the string//
	   *startang1 = startang;
		*sweepang1  = temp  - (startang + spangle);
		*(startang1 + 1) = temp  + spangle;
		*(sweepang1 + 1)  = endangle  - *(startang1 + 1);

		double mid = (startang + endangle)/2;
	   //Calculate the mid point to display value// 
		*points  = *center + r * (cos(mid));
		*(points + 1)  = *(center + 1) + r * (sin(mid));
		
		*(points + 2)  = *center + radius * (cos(startang));
	   *(points + 3)  = *(center + 1) + radius * (sin(startang));
	   *(points + 4)  = *endp1 + (r - radius) * (cos(startang));
	   *(points + 5)  = *(endp1 + 1) + (r - radius) * (sin(startang));
		
		*(points + 6)  = *center + radius * (cos(endangle));
	   *(points + 7)  = *(center + 1) + radius * (sin(endangle));
	   *(points + 8)  = *endp2 + (r -  radius) * (cos(endangle));
	   *(points + 9)  = *(endp2 + 1) + (r - radius) * (sin(endangle));
	
		*slopear = startang + M_PI_2;
		*(slopear + 1) = endangle - M_PI_2;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0036", __FILE__, __FUNCSIG__); return false; }
}




//


//Point to circle distance dimensioning......//
bool RapidMath2D::Circle_PointDistance( double *point,double *center,double radius,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type )
{
	double slope,slope1,intercept;
	double pt1[2],pt2[2],pt3[2],pt4[2];
	try
	{
		//function to calculate the slope and intercept of the line joinig the center and point//
		slopesintercept(point,center,point,&slope,&intercept,&slope1);
		//function to calculate 4 points on line joining center and point (2 intersection points and two at half of the radius) //
		circle_points(center,radius,slope,&pt1[0],&pt2[0],&pt3[0],&pt4[0]);
		Angle_FirstScndQuad(&slope1);
		//check the position of the mouse point , depending on that display the distance//
		//Point is below the circle//
		if(slope1 < M_PI_2)
			swap2(&pt1[0],&pt2[0]);
		if(*(point + 1) < *(center + 1))
			goto label1;
		// line joining the point and circle center is horizontal// 
		else if(*(point + 1) > *(center + 1))
			goto label2;
		else if(*(point + 1) == *(center + 1))
		{
			if(*point < *center)
				goto label1;
			else
				goto label2;
		}
label1:
		{
			if(type == 0)
				//point to circumference distance (min)
				Pt2Ptdis(point,pt1,mousept,space,dis,endpt1,endpt2,midpt,slopear);
			else if(type == 1)
				//point to center distance//
				Pt2Ptdis(point,center,mousept,space,dis,endpt1,endpt2,midpt,slopear);
			else if(type == 2)
				//point to circumference distance (max)
				Pt2Ptdis(point,pt2,mousept,space,dis,endpt1,endpt2,midpt,slopear);
			goto end;
		}
label2:
		{
			if(type == 0)
				//point to circumference distance (min)
				Pt2Ptdis(point,pt2,mousept,space,dis,endpt1,endpt2,midpt,slopear); 
			else if(type == 1)
				//point to center distance//
				Pt2Ptdis(point,center,mousept,space,dis,endpt1,endpt2,midpt,slopear);
			else if(type == 2)
				//point to circumference distance (max)
				Pt2Ptdis(point,pt1,mousept,space,dis,endpt1,endpt2,midpt,slopear);
			goto end;
		}
end:
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0037", __FILE__, __FUNCSIG__); return false; }	
}


bool RapidMath2D::Circle_FinitelineDistance(double slope, double intercept, double *endp1, double *endp2,double *center, double radius, double *intercept1,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type)
{
	double intersec[2], point[2];
	try
	{
		//Function to calculate the Intersection point of perpendicular from center to the line//
		intersection(slope,intercept,center,&intersec[0]);
		PtonCircle_Type(&intersec[0], center, radius, type, &point[0]);
		Point_FinitelineDistance(&point[0], slope, intercept, endp1, endp2, intercept1, dis, mousept, space, endpt1, endpt2, midpt, slopear);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0038", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Circle_InfinitelineDistance(double slope, double intercept, double *center, double radius, double *intercept1,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type)
{
	double intersec[2], point[2];
	try
	{
		//Function to calculate the Intersection point of perpendicular from center to the line//
		intersection(slope,intercept,center,&intersec[0]);
		PtonCircle_Type(&intersec[0], center, radius, type, &point[0]);
		Point_InfinitelineDistance(&point[0], slope, intercept, intercept1, dis, mousept, space, endpt1, endpt2, midpt, slopear);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0039", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Circle_RayDistance(double slope, double intercept, double *endp1,double *center, double radius, double *intercept1,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type)
{
	double intersec[2], point[2];
	try
	{
		//Function to calculate the Intersection point of perpendicular from center to the line//
		intersection(slope,intercept,center,&intersec[0]);
		PtonCircle_Type(&intersec[0], center, radius, type, &point[0]);
		Point_RayDistance(&point[0], slope, intercept, endp1, intercept1, dis, mousept, space, endpt1, endpt2, midpt, slopear);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0040", __FILE__, __FUNCSIG__); return false; }
}

//Circle to circle distance dimensioning...//////
bool RapidMath2D::Circle_CircleDistance( double *center1, double *center2,double radius1,double radius2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type )
{
	//8 points on the line joining of the circle centers//
	double pt1[2],pt2[2],pt3[2],pt4[2],pt5[2],pt6[2],pt7[2],pt8[2]; 
	double intercept,slope,slope1;
	try
	{
		//slope of the line joining the centers//
		slopesintercept(center1,center2,center1,&slope,&intercept,&slope1);
		//If center1 is greater than the center2 then swap the values//
		swap1(center1,center2,&radius1,&radius2);
		//Function to calculate 4 points on the line(2 intersection points and two at half of the radius)  --circle1//
		circle_points(center1,radius1,slope,&pt1[0],&pt2[0],&pt3[0],&pt4[0]);
		//Function to calculate 4 points on the line(2 intersection points and two at half of the radius)  --circle2//
		circle_points(center2,radius2,slope,&pt5[0],&pt6[0],&pt7[0],&pt8[0]);
		//Function to calculate the line passing on points pt3, pt4 ,pt7, pt8 with slope1//
		if(slope1 < M_PI_2)
		{
			swap2(&pt3[0],&pt4[0]);
			swap2(&pt7[0],&pt8[0]);
		}
		Angle_FirstScndQuad(&slope1);
		if(slope1 < M_PI_2)
		{
			swap2(&pt1[0],&pt2[0]);
			swap2(&pt5[0],&pt6[0]);
		}
		if(type == 2)
			//circumference to circumference distance (max)//
			Pt2Ptdis(pt1,pt6,mousept,space,dis,endpt1,endpt2,midpt,slopear);
		else if(type == 1)
			//center to center distance//
			Pt2Ptdis(center1,center2,mousept,space,dis,endpt1,endpt2,midpt,slopear);
		else if(type == 0)
			//circumference to circumference distance (min)//
			Pt2Ptdis(pt2,pt5,mousept,space,dis,endpt1,endpt2,midpt,slopear);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0044", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::LineSlope(double slope1, double intercept1, double *mousept, double *space, double *radius ,double *center, double *startang, double *sweepang, double *points, double *slopear)
{
	double p1[2],p2[2],d,endangle,temp,spangle = 0;
	//If slope of line1 is -ve then add PI/
	try
	{
		Angle_FirstScndQuad(&slope1);
		if(slope1 == M_PI_2)
		{
			p1[0] = intercept1;
			p1[1] = 0;
		}
		else if(slope1 == 0 || slope1 == M_PI)
		{
			p1[0] = 0;
			p1[1] =intercept1;
			slope1 = 0;
		}
		else
		{
			p1[0] = -(intercept1 / tan(slope1));
			p1[1] = 0;
			p2[0] = 0;
			p2[1] = intercept1;
		}
		//calculate the distance of the mouse point from the line//
		//to check the position of the point//
		d = Pt2Line_DistSigned(mousept[0], mousept[1], slope1, intercept1);
		//If the line slope is less than 90//
		if(slope1 < M_PI_2 && slope1 > 0)
		{
			//angle with x axis//
			if((d > 0) && (*(mousept + 1) > 0))
			{
				*center = p1[0];
				*(center + 1) = p1[1];
				*startang = 0;
				*sweepang = slope1;
			}
			//angle with the y axis//
			else if((d < 0) && (*(mousept) > 0))
			{
				*center = p2[0];
				*(center + 1) = p2[1];
				*startang = slope1;
				*sweepang = M_PI_2 - slope1;
			}
			else if((d > 0) && (*(mousept + 1) < 0))
			{
				*center = p1[0];
				*(center + 1) = p1[1];
				*startang = M_PI + slope1;
				*sweepang = M_PI - slope1;
			}
			else
			{
				*center = p2[0];
				*(center + 1) = p2[1];
				*startang = M_PI_2;
				*sweepang = M_PI_2 + slope1;
			}
		}
		else if((slope1 == M_PI_2) || (slope1 == 3 * M_PI_2))
		{
			*center = p1[0];
			*(center + 1) = p1[1];
			*sweepang = M_PI_2;
			if((d < 0) && (*(mousept + 1) > 0))
			{	*startang = 0;
			}
			else if((d < 0) && (*(mousept + 1) < 0))
			{	*startang = 3 * M_PI_2;
			}
			else if((d > 0) && (*(mousept + 1) > 0))
			{	*startang = M_PI_2;
			}
			else //if((d > 0) && (*(mousept + 1) < 0))
			{	*startang = M_PI;
			}

		}
		else if(slope1 == 0 || slope1 == M_PI)
		{

			*center = p1[0];
			*(center + 1) = p1[1];
			*sweepang = M_PI_2;
			if((d < 0) && (*(mousept) > 0))
			{	*startang = 0;
			}
			else if((d < 0) && (*(mousept) < 0))
			{	*startang = M_PI_2;
			}
			else if((d > 0) && (*(mousept) < 0))
			{	*startang = M_PI;
			}
			else //if((d > 0) && (*(mousept) > 0))
			{	*startang = 3 * M_PI_2;
			}
		}
		else
		{
			if((d < 0) && (*(mousept + 1) > 0))
			{
				*center = p1[0];
				*(center + 1) = p1[1];
				*startang = slope1;
				*sweepang = M_PI - *startang;
			}
			else if((d > 0) && (*(mousept) < 0))
			{
				*center = p2[0];
				*(center + 1) = p2[1];
				*startang = M_PI_2;
				*sweepang =  slope1 - M_PI_2;
			}
			else if((d < 0) && (*(mousept + 1) < 0))
			{
				*center = p1[0];
				*(center + 1) = p1[1];
				*startang = M_PI;
				*sweepang =  M_PI - (M_PI - slope1);
			}
			else
			{
				*center = p2[0];
				*(center + 1) = p2[1];
				*startang = slope1 + M_PI;
				*sweepang = M_PI - (slope1 -M_PI_2) ;
			}
		}
		//calculate the radius of the arc//
		*radius = Pt2Pt_distance(center, mousept);
		spangle = atan(((*(space + 1)/2) + 10) / *radius);
		endangle = *startang + *sweepang;
		temp = (*startang + endangle)/2;
		//starting and ending angles for the given space//
		*startang = *startang;
		*sweepang  = (temp  - (*startang+ spangle));
		*(startang + 1) = temp  + spangle;
		*(sweepang + 1)  = (endangle  - *(startang + 1));


		//Calculate the mid point to display value// 
		*points  = *center + *radius * (cos(temp));
		*(points + 1)  = *(center + 1) + *radius * (sin(temp));
		//points to draw arrow
		*(points + 2)  = *center + *radius * (cos(*startang));
		*(points + 3)  = *(center + 1) + *radius * (sin(*startang));

		//Point to draw arrow at angle//
		*(points + 4)  = *center + *radius * (cos(endangle));
		*(points + 5)  = *(center + 1) + *radius * (sin(endangle));

		*slopear = *startang + M_PI_2;
		*(slopear + 1) = endangle - M_PI_2;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0048", __FILE__, __FUNCSIG__); return false; }
}


//Linear mesurement calculations between circle to circle.....//
bool RapidMath2D::Circle_CircleDistance_Linear(double *center1,double *center2,double radius1, double radius2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type )
{
	try
	{
		double pt1[4],pt2[4],pt3[4],pt4[4],pt5[4],pt6[4],pt7[4],pt8[4]; 
		double slope,inter,temp;
		bool fl = false;
		//If center1 is greater than the center2 then swap the values//
		swap1(center1,center2,&radius1,&radius2);
		//Function to calculate 4 points on the line(2 intersection points and two at half of the radius)  --circle1//
		Lcircle_Points(center1,radius1,&pt1[0],&pt2[0],&pt3[0],&pt4[0]);
		//Function to calculate 4 points on the line(2 intersection points and two at half of the radius)  --circle2//
		Lcircle_Points(center2,radius2,&pt5[0],&pt6[0],&pt7[0],&pt8[0]);

		TwoPointLine(center1,center2,&slope,&inter);
		Angle_FirstScndQuad(&slope);
		if(slope > M_PI_2)
		{
			swaptwopoints(&pt2[0],&pt2[2]);
			swaptwopoints(&pt6[0],&pt6[2]);
			swaptwopoints(&pt1[0],&pt1[2]); 
			swaptwopoints(&pt5[0],&pt5[2]);
		}
		if(pt2[0] > pt6[2])
		{
			temp  = pt2[0];
			pt2[0] = pt6[2];
			pt6[2] = temp;
			temp  = pt1[0];
			pt1[0] = pt5[2];
			pt5[2] = temp;
			fl = true;
		}
		if(pt2[2] > pt6[0])
		{
			temp  = pt2[2];
			pt2[2] = pt6[0];
			pt6[0] = temp;
			temp  = pt1[2];
			pt1[2] = pt5[0];
			pt5[0] = temp;
		}
		if(fl)
		{
			temp  = pt1[0];	pt1[0] = pt5[2]; pt5[2] = temp;
			temp  = pt1[2];	pt1[2] = pt5[0]; pt5[0] = temp;
		}
		if(type == 2)
		{
			//circumference to circumference distance (max)
			X_shift(&pt1[0],&pt5[2],mousept,space,dis,endpt1,endpt2,midpt,slopear);
			*endpt1 = pt1[0];	*(endpt1 + 1) = pt1[1];
			*endpt2 = pt5[2];	*(endpt2 + 1) = pt5[3];	
		} 
		else if(type == 1)
		{
			//center to center distance//
			X_shift(center1,center2,mousept,space,dis,endpt1,endpt2,midpt,slopear);
			*endpt1 = *center1;	*(endpt1 + 1) = *(center1 + 1);
			*endpt2 = *center2;	*(endpt2 + 1) = *(center2 + 1);		
		} 
		else if(type == 0)
		{
			//circumference to circumference distance (min)//
			X_shift(&pt1[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
			*endpt1 = pt1[2];	*(endpt1 + 1) = pt1[3];
			*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];		
		}
		else if(type == 5)
		{
			//circumference to circumference distance (max)
			Y_shift(&pt3[0],&pt7[2],mousept,space,dis,endpt1,endpt2,midpt,slopear);
			*endpt1 = pt3[0];	*(endpt1 + 1) = pt3[1];
			*endpt2 = pt7[2];	*(endpt2 + 1) = pt7[3];	
		} 
		else if(type == 4)
		{
			//center to center distance//
			Y_shift(center1,center2,mousept,space,dis,endpt1,endpt2,midpt,slopear);
			*endpt1 = *center1;	*(endpt1 + 1) = *(center1 + 1);
			*endpt2 = *center2;	*(endpt2 + 1) = *(center2 + 1);		
		} 
		else if(type == 3)
		{
			//circumference to circumference distance (min)//
			Y_shift(&pt3[2],&pt7[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
			*endpt1 = pt3[2];	*(endpt1 + 1) = pt3[3];
			*endpt2 = pt7[0];	*(endpt2 + 1) = pt7[1];	
		}
		else
		{
			//circumference to circumference distance (max)
			X_shift(&pt1[0],&pt5[2],mousept,space,dis,endpt1,endpt2,midpt,slopear);
			*endpt1 = pt1[0];	*(endpt1 + 1) = pt1[1];
			*endpt2 = pt5[2];	*(endpt2 + 1) = pt5[3];	
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0050", __FILE__, __FUNCSIG__); return false; }
}
//Part program direction: arrow end points calculations......
bool RapidMath2D::PartProgramDirection(double *point, double *point1, double *rectendpts, double *arrowendpts, double *MidArrowendpts, double width, double height)
{
	try
	{
		double slope, intercept;
		TwoPointLine(point, point1, &slope, &intercept);
		double rslope1 = ray_angle(point, point1);
		double point2[2] = {point1[0] - (cos(rslope1)) * 5 * height, point1[1] - (sin(rslope1)) * 5 * height};
		double point3[2] = {(point[0] + point1[0])/2, (point[1] + point1[1])/2};
		double slope1 = atan(-1/tan(slope));
		double cosangle = cos(slope1), sinangle = sin(slope1);
		double RectCos = width * cosangle, RectSin = width * sinangle;
		double EarrwCos = height * cosangle, EarrwSin = height * sinangle;
		double MarrwCos = 2.5 * EarrwCos, MarrwSin = 2.5 * EarrwSin;
		double ArrwEndCos = (cos(rslope1)) * 5 * height, ArrwEndSin = (sin(rslope1)) * 5 * height;
		//Rectangle..
		rectendpts[0] = point[0] + RectCos; rectendpts[1] = point[1] + RectSin;
		rectendpts[2] = point[0] - RectCos; rectendpts[3] = point[1] - RectSin;
		rectendpts[4] = point2[0] + RectCos; rectendpts[5] = point2[1] + RectSin;
		rectendpts[6] = point2[0] - RectCos; rectendpts[7] = point2[1] - RectSin;

		///Middle arrow
		MidArrowendpts[0] = point3[0] + MarrwCos; MidArrowendpts[1] = point3[1] + MarrwSin;
		MidArrowendpts[2] = point3[0] - MarrwCos; MidArrowendpts[3] = point3[1] - MarrwSin;
		MidArrowendpts[4] = point3[0] + ArrwEndCos; MidArrowendpts[5] = point3[1] + ArrwEndSin;

		//Arrow end points..
		arrowendpts[0] = point2[0] + MarrwCos; arrowendpts[1] = point2[1] + MarrwSin;
		arrowendpts[2] = point2[0] - MarrwCos; arrowendpts[3] = point2[1] - MarrwSin;
		arrowendpts[4] = point2[0] + ArrwEndCos; arrowendpts[5] = point2[1] + ArrwEndSin;

		point1[0] = point2[0];
		point1[1] = point2[1];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0051", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::PartProgramStartingPoint(double *point, double *endpoints, double wid, double length)
{
	try
	{
		endpoints[0] = point[0] - wid;
		endpoints[1] = point[1] + wid;
		endpoints[2] = point[0] - wid;
		endpoints[3] = point[1] - wid;
		endpoints[4] = point[0] + wid;
		endpoints[5] = point[1] - wid;
		endpoints[6] = point[0] + wid;
		endpoints[7] = point[1] + wid;
		//Cross mark///
		endpoints[8] = point[0];
		endpoints[9] = point[1] - length;
		endpoints[10] = point[0];
		endpoints[11] = point[1] + length;
		endpoints[12] = point[0] + length;
		endpoints[13] = point[1];
		endpoints[14] = point[0] - length;
		endpoints[15] = point[1];
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0052", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::Circle_PointDistance_Linear( double *center, double radius,double *point, double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type )
{

	double slope,inter;
	double pt1[4],pt2[4],pt3[4],pt4[4],pt5[2];
	try
	{
		//function to calculate 4 points on line joining center and point (2 intersection points and two at half of the radius) //
		Lcircle_Points(center,radius,&pt1[0],&pt2[0],&pt3[0],&pt4[0]);
		pt5[0] = *point; pt5[1] = *(point + 1);
		TwoPointLine(center,point,&slope,&inter);
		Angle_FirstScndQuad(&slope);
		if(slope > M_PI_2)
		{
			swaptwopoints(&pt2[0],&pt2[2]);
			swaptwopoints(&pt1[0],&pt1[2]);
		}
		if(pt5[1] > *(center + 1))
		{
			swaptwopoints(&pt4[0],&pt4[2]);
			swaptwopoints(&pt3[0],&pt3[2]);
		}
		if(*(point + 1) < *(center + 1))
		{
			if(*point < *center)
				goto label1;
			else
				goto label2;
		}
		else if(*(point + 1) > *(center + 1))
		{
			if(*point < *center)
				goto label3;
			else
				goto label4;
		}

label1:
		{

			if(type == 0) 
			{
				//point to circumference distance (min)
				X_shift(&pt1[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[0];	*(endpt1 + 1) = pt1[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 1) 
			{
				//point to center distance//
				X_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 2) 
			{
				//point to circumference distance (max)//
				X_shift(&pt1[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[2];	*(endpt1 + 1) = pt1[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			else if(type == 3) 
			{
				//point to circumference distance (min)
				Y_shift(&pt3[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[0];	*(endpt1 + 1) = pt3[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 4) 
			{
				//point to center distance//
				Y_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 5) 
			{
				//point to circumference distance (max)//
				Y_shift(&pt3[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[2];	*(endpt1 + 1) = pt3[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			goto end;
		}

label2:
		{
			if(type == 0) 
			{
				//point to circumference distance (min)
				X_shift(&pt1[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[0];	*(endpt1 + 1) = pt1[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 1) 
			{
				//point to center distance//
				X_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 2) 
			{
				//point to circumference distance (max)//
				X_shift(&pt1[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[2];	*(endpt1 + 1) = pt1[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			else if(type == 3) 
			{
				//point to circumference distance (min)
				Y_shift(&pt3[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[0];	*(endpt1 + 1) = pt3[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 4) 
			{
				//point to center distance//
				Y_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 5) 
			{
				//point to circumference distance (max)//
				Y_shift(&pt3[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[2];	*(endpt1 + 1) = pt3[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			goto end;
		}

label3:
		{
			if(type == 0) 
			{
				//point to circumference distance (min)
				X_shift(&pt1[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[2];	*(endpt1 + 1) = pt1[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 1) 
			{
				//point to center distance//
				X_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 2) 
			{
				//point to circumference distance (max)//
				X_shift(&pt1[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[0];	*(endpt1 + 1) = pt1[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			else if(type == 3) 
			{
				//point to circumference distance (min)
				Y_shift(&pt3[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[0];	*(endpt1 + 1) = pt3[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 4) 
			{
				//point to center distance//
				Y_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 5) 
			{
				//point to circumference distance (max)//
				Y_shift(&pt3[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[2];	*(endpt1 + 1) = pt3[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			goto end;
		}

label4:
		{
			if(type == 0) 
			{
				//point to circumference distance (min)
				X_shift(&pt1[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[2];	*(endpt1 + 1) = pt1[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 1) 
			{
				//point to center distance//
				X_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 2) 
			{
				//point to circumference distance (max)//
				X_shift(&pt1[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt1[0];	*(endpt1 + 1) = pt1[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			else if(type == 3) 
			{
				//point to circumference distance (min)
				Y_shift(&pt3[0],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[0];	*(endpt1 + 1) = pt3[1];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 4) 
			{
				//point to center distance//
				Y_shift(center,&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = *center;	*(endpt1 + 1) = *(center + 1);
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			} 
			else if(type == 5) 
			{
				//point to circumference distance (max)//
				Y_shift(&pt3[2],&pt5[0],mousept,space,dis,endpt1,endpt2,midpt,slopear);
				*endpt1 = pt3[2];	*(endpt1 + 1) = pt3[3];
				*endpt2 = pt5[0];	*(endpt2 + 1) = pt5[1];	
			}
			goto end;
		}

end:	return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0053", __FILE__, __FUNCSIG__); return false; }
}


void RapidMath2D::swaptwopoints(double **point1,double **point2)
{
	try
	{
		double *temp = *point1;
		*point1 = *point2;
		*point2 = temp;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0054", __FILE__, __FUNCSIG__); }
}
void RapidMath2D::swaptwopoints(double *point1,double *point2)
{
	try
	{
		double temp;
		temp = *point1;
		*point1 = *point2;
		*point2 = temp;
		temp = *(point1 + 1);
		*(point1 + 1) = *(point2 + 1);
		*(point2 + 1) = temp;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0055", __FILE__, __FUNCSIG__); }
}

//If the angle is negative convert to 0 to 360//
void RapidMath2D::arcfun(double* temp)
{
	try
	{
		if(*temp < 0)
			*temp += 2*M_PI;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0058", __FILE__, __FUNCSIG__); }
}

//function to find intersection point of perpendicular from a given point//
bool RapidMath2D::intersection( double slope, double intercept, double *point, double *intersectn )
{
	double dis;  //Perpendicular distance between point and  line//
	try
	{
		if(slope >= M_PI)
			slope -= M_PI;
		//if the line is vertical then the perpendicular distance is diff. b/n intercept and X.point//
		if(slope == M_PI_2)
			dis = (*point -  intercept);
		else
			//perpendicular distance from point to line//
			dis = (tan(slope) * (*point) + intercept-*(point + 1)) * cos(slope);
		//Intersection point of perpendicular//
		*intersectn = *point - dis * sin(slope);
		*(intersectn + 1) = *(point + 1) + dis * cos(slope);
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0059", __FILE__, __FUNCSIG__); return false; }
}


void RapidMath2D::SwapTwoValues(double* value1, double* value2)
{
	try
	{
		double temp = *value1;
		*value1 = *value2;
		*value2 = temp;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0062", __FILE__, __FUNCSIG__);}
}

//function to swap the points for the calculation purpose//
bool RapidMath2D::swap1(double *point1,double *point2,double *r1,double *r2)
{
	double temp;
	try
	{
		if(*(point1 + 1) > *(point2 + 1))
		{
			temp = *point1;
			*point1 = *point2;
			*point2 = temp;
			temp = *(point1 + 1);
			*(point1 + 1) = *(point2 + 1);
			*(point2 + 1) = temp;
			temp = *r1;
			*r1 = *r2;
			*r2 = temp;
		}
		else if(*(point1 + 1) == *(point2 + 1))
		{
			if(*point1 > *point2)
			{
				temp = *point1;
				*point1 = *point2;
				*point2 = temp;
				temp = *r1;
				*r1 = *r2;
				*r2 = temp;
			}
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0063", __FILE__, __FUNCSIG__); return false; }
}

//swap function..//
bool RapidMath2D::swap2( double *point1,double *point2 )
{
	double temp;
	try
	{
		if((*(point1 + 1) - *(point2 + 1)) > 0.00001)
		{
			temp = *point1;
			*point1 = *point2;
			*point2 = temp;
			temp = *(point1 + 1);
			*(point1 + 1) = *(point2 + 1);
			*(point2 + 1) = temp;
			
		}
		else if(*(point1 + 1) == *(point2 + 1))
		{
			if(*point1 > *point2)
			{
				temp = *point1;
				*point1 = *point2;
				*point2 = temp;
			
			}
		}
		
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0064", __FILE__, __FUNCSIG__); return false; }
}


//Points pn circle to calculate the distances(Linear measure) with respect to mouse point..///
bool RapidMath2D::Lcircle_Points( double *center,double radius,double *pt1,double *pt2,double *pt3,double *pt4 )
{
	try
	{
		*pt1 = *center - radius;
		*(pt1 + 1) = *(center + 1);
		*(pt1 + 2) = *center + radius;
		*(pt1 + 3) = *(center + 1);
		*pt2 = *center - radius/2;
		*(pt2 + 1) = *(center + 1);
		*(pt2 + 2) = *center + radius/2;
		*(pt2 + 3) = *(center + 1);
		*pt3 = *center;
		*(pt3 + 1) = *(center + 1) - radius;
		*(pt3 + 2) = *center;
		*(pt3 + 3) = *(center + 1) + radius;
		*pt4 = *center;
		*(pt4 + 1) = *(center + 1) - radius/2;
		*(pt4 + 2) = *center;
		*(pt4 + 3) = *(center + 1) + radius/2;
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0066", __FILE__, __FUNCSIG__); return false; }
}

//Round the double vaule : decplace: no. of decimal places
double RapidMath2D::RoundDecimal(double valu, int decPlace)
{
	try
	{
		double power = int(pow(10.0,decPlace));
		double temp1 = int(valu * power) / power;
		return temp1;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0074", __FILE__, __FUNCSIG__); return false; }
}

//bool RapidMath2D::PtonCircle_Type(double *point,double *center,double radius,int type, double *rpoint)
//{
//	double slope,slope1,intercept;
//	double pt1[2],pt2[2],pt3[2],pt4[2];
//	try
//	{
//		//function to calculate the slope and intercept of the line joinig the center and point//
//		slopesintercept(point,center,point,&slope,&intercept,&slope1);
//		//function to calculate 4 points on line joining center and point (2 intersection points and two at half of the radius) //
//		circle_points(center,radius,slope,&pt1[0],&pt2[0],&pt3[0],&pt4[0]);
//		Angle_FirstScndQuad(&slope1);
//		//check the position of the mouse point , depending on that display the distance//
//		//Point is below the circle//
//		if(slope1 < M_PI_2)
//			swap2(&pt1[0],&pt2[0]);
//		if(*(point + 1) < *(center + 1))
//			goto label1;
//		// line joining the point and circle center is horizontal// 
//		else if(*(point + 1) > *(center + 1))
//			goto label2;
//		else if(*(point + 1) == *(center + 1))
//		{
//			if(*point < *center)
//				goto label1;
//			else
//				goto label2;
//		}
//label1:
//		{
//			if(type == 0)	//point to circumference distance (min)
//			{
//				*rpoint = pt1[0];
//				*(rpoint + 1) = pt1[1];
//			}
//			else if(type == 1)	//point to center distance//
//			{
//				*rpoint = *center;
//				*(rpoint + 1) = *(center + 1);
//			}
//			else if(type == 2)	//point to circumference distance (max)
//			{
//				*rpoint = pt2[0];
//				*(rpoint + 1) = pt2[1];
//			}
//			goto end;
//		}
//label2:
//		{
//			if(type == 0)	//point to circumference distance (min)
//			{
//				*rpoint = pt2[0];
//				*(rpoint + 1) = pt2[1];
//			}
//			else if(type == 1)	//point to center distance//
//			{
//				*rpoint = *center;
//				*(rpoint + 1) = *(center + 1);
//			}
//			else if(type == 2)	//point to circumference distance (max)
//			{
//				*rpoint = pt1[0];
//				*(rpoint + 1) = pt1[1];
//			}
//			goto end;
//		}
//end:
//		return true;
//	}
//	catch(...){ SetAndRaiseErrorMessage("GRF2D0075", __FILE__, __FUNCSIG__); return false; }
//}

bool RapidMath2D::PtonCircle_Type(double *point,double *center,double radius,int type, double *rpoint)
{
	try
	{
		double slope = 0, intercept = 0, InterSectionPt[4] = {0}, MinPt[2] = {0}, MaxPt[2] = {0};
		//function to calculate the slope and intercept of the line joinig the center and point//
		TwoPointLine(point, center, &slope, &intercept);
		//infinite line and circle intersection...
		int count = Line_circleintersection(slope, intercept, center, radius, InterSectionPt);
		if(count == 2)
		{
			double dist1 = Pt2Pt_distance(point, InterSectionPt);
			double dist2 = Pt2Pt_distance(point, &InterSectionPt[2]);
			if(dist1 < dist2)
			{
				MinPt[0] = InterSectionPt[0];
				MinPt[1] = InterSectionPt[1];
				MaxPt[0] = InterSectionPt[2];
				MaxPt[1] = InterSectionPt[3];
			}
			else
			{
				MaxPt[0] = InterSectionPt[0];
				MaxPt[1] = InterSectionPt[1];
				MinPt[0] = InterSectionPt[2];
				MinPt[1] = InterSectionPt[3];
			}
		}
		else if(count == 1)
		{
			MinPt[0] = InterSectionPt[0];
			MinPt[1] = InterSectionPt[1];
			MaxPt[0] = InterSectionPt[0];
			MaxPt[1] = InterSectionPt[1];
		}
		else
			return false;
		if(type == 0)	//point to circumference distance (min)
		{
			*rpoint = MinPt[0];
			*(rpoint + 1) = MinPt[1];
		}
		else if(type == 1)	//point to center distance//
		{
			*rpoint = *center;
			*(rpoint + 1) = *(center + 1);
		}
		else if(type == 2)	//point to circumference distance (max)
		{
			*rpoint = MaxPt[0];
			*(rpoint + 1) = MaxPt[1];
		}
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0075", __FILE__, __FUNCSIG__); return false; }
}

bool RapidMath2D::PtonArc_Type(double *point,double *center,double radius, double startang,double sweepang,int type,double *rpoint)
{
	double slope,slope1,intercept;
	double pt1[2],pt2[2],pt3[2],pt4[2];
	double endangle,sl1,sl2,temp,temp1,temp2,temp3,temp4;
	try
	{
		//function to calculate the slope and intercept of the line joinig the center and point//
		slopesintercept(point,center,point,&slope,&intercept,&slope1);
		Angle_FirstScndQuad(&slope);
		//function to calculate 4 points on line joining center and point (2 intersection points and two at half of the radius) //
		circle_points(center,radius,slope,&pt1[0],&pt2[0],&pt3[0],&pt4[0]);
		sl1 = ray_angle(center, &pt1[0]);
		sl2 = ray_angle(center, &pt2[0]);
		endangle = startang + sweepang;
		if(startang > endangle)
		{
			temp = startang;
			startang  = endangle;
			endangle = temp;
		}
		if(endangle > 2*M_PI)
			endangle -= 2*M_PI;
		//angles to check the position of the points on arc//
		temp = (endangle - startang);
		temp1 = (endangle -sl1);
		temp2 = (sl1 - startang);
		temp3 = (endangle -sl2);
		temp4 = (sl2 - startang);

		arcfun(&temp);
		arcfun(&temp1);
		arcfun(&temp2);
		arcfun(&temp3);
		arcfun(&temp4);
		
		//check the position of the mouse point , depending on that display the distance//
		//Point is below the circle//
		if(((temp1 <= temp) && (temp2 <= temp)) && ((temp3 <= temp) && (temp4 <= temp)))
		{
			if(*(point + 1) < *(center + 1))
				goto label1;
			// line joining the point and circle center is horizontal// 
			else if(*(point + 1) > *(center + 1))
				goto label2;
			else if(*(point + 1) == *(center + 1))
			{
				if(*point < *center)
					goto label1;
				else
					goto label2;
			}
		}
		else if((temp1 <= temp) && (temp2 <= temp))
		{
			if(*(point + 1) < *(center + 1))
				goto label3;
			// line joining the point and circle center is horizontal// 
			else if(*(point + 1) > *(center + 1))
				goto label4;
			else if(*(point + 1) == *(center + 1))
			{
				if(*point < *center)
					goto label3;
				else
					goto label4;
			}
		}
		else if((temp3 <= temp) && (temp4 <= temp))
		{
			if(*(point + 1) < *(center + 1))
				goto label5;
			// line joining the point and circle center is horizontal// 
			else if(*(point + 1) > *(center + 1))
				goto label6;
			else if(*(point + 1) == *(center + 1))
			{
				if(*point < *center)
					goto label5;
				else
					goto label6;
			}
		}
		else
			goto label7;
label1:
		{
			if(type == 2)	//point to circumference distance (min)
			{
				*rpoint = pt1[0];
				*(rpoint + 1) = pt1[1];
			}
			else if(type == 0)	//point to center distance//
			{
				*rpoint = *center;
				*(rpoint + 1) = *(center + 1);
			}
			else if(type == 1)	//point to circumference distance (max)
			{
				*rpoint = pt2[0];
				*(rpoint + 1) = pt2[1];
			}
			goto end;
		}
label2:
		{
			if(type == 2)	//point to circumference distance (min)
			{
				*rpoint = pt2[0];
				*(rpoint + 1) = pt2[1];
			}
			else if(type == 0)	//point to center distance//
			{
				*rpoint = *center;
				*(rpoint + 1) = *(center + 1);
			}
			else if(type == 1)	//point to circumference distance (max)
			{
				*rpoint = pt1[0];
				*(rpoint + 1) = pt1[1];
			}
			goto end;
		}

label3:
		{
			if(type == 2)	//point to circumference distance (min)
			{
				*rpoint = pt1[0];
				*(rpoint + 1) = pt1[1];
			}
			else	//point to center distance//
			{
				*rpoint = *center;
				*(rpoint + 1) = *(center + 1);
			}
			goto end;
		}
label4:
		{
			if(type == 1)	//point to circumference distance (max)
			{
				*rpoint = pt1[0];
				*(rpoint + 1) = pt1[1];
			}
			else	//point to center distance//
			{
				*rpoint = *center;
				*(rpoint + 1) = *(center + 1);
			}
			goto end;
		}
label5:
		{
			if(type == 1)	//point to circumference distance (max)
			{
				*rpoint = pt2[0];
				*(rpoint + 1) = pt2[1];
			}
			else	//point to center distance//
			{
				*rpoint = *center;
				*(rpoint + 1) = *(center + 1);
			}
			goto end;
		}
label6:
		{
			if(type == 2)	//point to circumference distance (min)
			{
				*rpoint = pt2[0];
				*(rpoint + 1) = pt2[1];
			}
			else	//point to center distance//
			{
				*rpoint = *center;
				*(rpoint + 1) = *(center + 1);
			}
			goto end;
		}
label7:
		{	
			//point to center distance//
			*rpoint = *center;
			*(rpoint + 1) = *(center + 1);
			goto end;
		}

end:
		return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0076", __FILE__, __FUNCSIG__); return false; }
}

void RapidMath2D::SortDoubleArray(double *numbarArray, int low, int high)
{
	try
	{
		int pivotElement, index, smallIndex;
		double pivot;
		if (low < high)
		{
			SwapArrayElement(numbarArray, low, ( low + high) / 2) ;
			pivot = numbarArray[low];
			smallIndex = low;
			for(index = low + 1; index <= high; index++)
			{
				if (numbarArray[low] < pivot)
				{
					smallIndex++;
					SwapArrayElement(numbarArray, smallIndex, index);
				}
			}
			SwapArrayElement(numbarArray, low, smallIndex) ;
			pivotElement = smallIndex;
			SortDoubleArray(numbarArray, low, pivotElement - 1);
			SortDoubleArray(numbarArray, pivotElement + 1, high);
		}
		else
			return;
	}
	catch(...){ SetAndRaiseErrorMessage("GRF2D0075a", __FILE__, __FUNCSIG__); return; }
}

void RapidMath2D::SwapArrayElement(double *numberArray, int first, int second)
{
	double temp = numberArray[first];
	numberArray[first] = numberArray[second];
	numberArray[second] = temp;
}

 void RapidMath2D::Median_Double_Array(double *numberArray, int arrayLength, double *median)
 {
	if(arrayLength > 1)
	{
		int temp = arrayLength/2;
		if(arrayLength % 2 == 0)
			*median = numberArray[temp];
		else
			*median = (numberArray[temp] + numberArray[temp + 1]) / 2;
	}
 }

int RapidMath2D::GetTransformedProbeDirection(double *transform, int PreviousDir)
 {
	 try
	 {
		 int TransformedDir = 0, AxisDir = 0;
		 int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		 double temp1[4] = {0}, temp2[4] = {0}, tempTransform[16] = {0};
		 for (int i = 0; i <16; i++)
			 tempTransform[i] = transform[i];
		 tempTransform[3] = 0; tempTransform[7] = 0; tempTransform[11] = 0; tempTransform[15] = 1;
		 switch(PreviousDir)
		 {
		 case 0:
			 {
				temp2[0] = -1; temp2[1] = 0; temp2[2] = 0; temp2[3] = 1;
			 }
			 break;
		 case 1:
			  {
				temp2[0] = 1; temp2[1] = 0; temp2[2] = 0; temp2[3] = 1;
			  }
			 break;
		 case 2:
			  {
				temp2[0] = 0; temp2[1] = -1; temp2[2] = 0; temp2[3] = 1;
			  }
			 break;
		 case 3:
			  {
				temp2[0] = 0; temp2[1] = 1; temp2[2] = 0; temp2[3] = 1;
			  }
			 break;
		 case 4:
			  {
				temp2[0] = 0; temp2[1] = 0; temp2[2] = -1; temp2[3] = 1;
			  }
			 break;
		 case 5:
			  {
				temp2[0] = 0; temp2[1] = 0; temp2[2] = 1; temp2[3] = 1;
			  }
			 break;
		 }
		 MultiplyMatrix1(tempTransform, Order1, temp2, Order2, temp1);

		 double tempValue = temp1[0];
		 if(abs(tempValue) < abs(temp1[1]))
		 {
			 tempValue = temp1[1];
			 AxisDir = 1;
		 }
		 if(abs(tempValue) < abs(temp1[2]))
			 AxisDir = 2;
		 
		 if(AxisDir == 0)
		 {
			 if(temp1[0] > 0)
				 TransformedDir = 1;
		 }
		 else if(AxisDir == 1)
		 {
			 if(temp1[1] > 0)
				 TransformedDir = 3;
			 else
				 TransformedDir = 2;
		 }
		 else
		 {
			  if(temp1[2] > 0)
				 TransformedDir = 5;
			 else
				 TransformedDir = 4;
		 }
		 return TransformedDir;
	 }
	catch(...){ SetAndRaiseErrorMessage("GRF2D0076", __FILE__, __FUNCSIG__); return 0; }
}

void RapidMath2D::PointOnArcForWidth(double *cpoint, double slope, double intercept, double* center, double radius, double startang, double sweepang, double *resultpoints, int *nearestSide, double *midPnts, bool checknearest)
{
	try
	{
		double intersectionPnts[4] = {0};
		int intCnt = Line_circleintersection(slope, intercept, center, radius, intersectionPnts);
		double dis1 = Pt2Pt_distance(cpoint, intersectionPnts);
		double dis2 = Pt2Pt_distance(cpoint, &intersectionPnts[2]);
		if(dis1 > dis2)
		{
			double tmp[2] = {0};
			for(int i = 0; i < 2; i++)
			{
			   tmp[i] = intersectionPnts[i];
			   intersectionPnts[i] = intersectionPnts[2 + i];
			   intersectionPnts[2 + i] = tmp[i];
			}
		}
		else if((dis1 == dis2) && (intersectionPnts[1] < intersectionPnts[3]))
		{
			double tmp[2] = {0};
			for(int i = 0; i < 2; i++)
			{
			   tmp[i] = intersectionPnts[i];
			   intersectionPnts[i] = intersectionPnts[2 + i];
			   intersectionPnts[2 + i] = tmp[i];
			}
		}
		if(checknearest)
		{
			double pointonarc[2] = {0};
			Point_onArc(center, radius, startang, sweepang, intersectionPnts, pointonarc);
			if(abs(Pt2Pt_distance(intersectionPnts, pointonarc)) < 0.0001)
				*nearestSide = 1;
			else
			{
				Point_onArc(center, radius, startang, sweepang, &intersectionPnts[2], pointonarc);
				if(abs(Pt2Pt_distance(&intersectionPnts[2], pointonarc)) < 0.0001)
					*nearestSide = 0;
				else
					*nearestSide = 1;
			}
		}
		if(*nearestSide == 1)
		{
		   resultpoints[0] = intersectionPnts[0];
		   resultpoints[1] = intersectionPnts[1];
		}
		else
		{
		   resultpoints[0] = intersectionPnts[2];
		   resultpoints[1] = intersectionPnts[3];
		}
	}
	catch(...)
	{
	
	}
}


void RapidMath2D::Rotate_2D_Pt_Origin(double* givenPt, double* centre, double RotateAngle, double *answer)
{
	double dx = givenPt[0] - centre[0];
	double dy = givenPt[1] - centre[1];
	double radius = sqrt(dx * dx + dy * dy);
	double theta = 0;
	
	if (dx == 0)
	{
		if (dy == 0)
			theta = 0;
		else if (dy > 0)
			theta = M_PI_2;
		else
			theta = 3 * M_PI_2;
	}
	else
	{
		theta = atan(dy / dx);
		if (dx < 0) theta += M_PI;
		if (dy < 0 && dx > 0) theta += 2 * M_PI;
	}
	answer[0] = radius * cos(theta - RotateAngle);
	answer[1] = radius * sin(theta - RotateAngle);
}

void RapidMath2D::Rotate_2D_Pt(double* givenPt, double* centre, double RotateAngle, double *answer)
{
	double dx = givenPt[0] - centre[0]; // offsetPt[0];
	double dy = givenPt[1] - centre[1]; // offsetPt[1];
	double radius = sqrt(dx * dx + dy * dy);
	double theta = 0;

	if (dx == 0)
	{
		if (dy == 0)
			theta = 0;
		else if (dy > 0)
			theta = M_PI_2;
		else
			theta = 3 * M_PI_2;
	}
	else
	{
		theta = atan(dy / dx);
		if (dx < 0) theta += M_PI;
		if (dy < 0 && dx > 0) theta += 2 * M_PI;
	}
	answer[0] = centre[0] + radius * cos(theta - RotateAngle);
	answer[1] = centre[1] + radius * sin(theta - RotateAngle);
}

void RapidMath2D::UnRotate_2D_Pt(double* givenPt, double* centre, double RotateAngle, double *answer)
{
	double dx = givenPt[0];
	double dy = givenPt[1];
	double radius = sqrt(dx * dx + dy * dy);
	double theta = 0;

	if (dx == 0)
	{
		if (dy == 0)
			theta = 0;
		else if (dy > 0)
			theta = M_PI_2;
		else
			theta = 3 * M_PI_2;
	}
	else
	{
		theta = atan(dy / dx);
		if (dx < 0) theta += M_PI;
		if (dy < 0 && dx > 0) theta += 2 * M_PI;
	}
	answer[0] = centre[0] + radius * cos(theta + RotateAngle);
	answer[1] = centre[1] + radius * sin(theta + RotateAngle);
}