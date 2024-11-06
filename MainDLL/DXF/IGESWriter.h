//DXF writer class... 
//Writes the shape parameters to a dxf file format...//
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
using namespace std;

class PointCollection;
class IGESWriter
{
//Variable declaration...//
private:
	std::list<std::string> DElist, PElist;
	std::list<std::string> DXFList;
	std::list<std::string>::iterator SGlistIterator, PElistIterator, DElistIterator;
	std::string dstr1, dstr2, dstr3, dstr4, dstr5, dstr6, dstr7, dstr8, dstr9, dstr10, dstr11, dstr12, dstr13, rev, parametric, trimmed, trans;
	double generatrix1[6], generatrix2[6], directrix[6];
	char* filename;
	char cd[30];
	ofstream DXFDataWriter;
	//Double to string conversion functions
	char* DoubletoString(double x);
	
public:
	//Constructor and destructor...//
	IGESWriter();
    ~IGESWriter();

public:
	//Add cone parameter...//
	void addConeParameter(double X1, double Y1, double Z1, double X2, double Y2, double Z2, double r1, double r2);
	//Add cylinder parameter..//
	void addCylinderParameter(double X1, double Y1, double Z1, double X2, double Y2, double Z2, double r1, double r2);
	//Add the sphere parameter...//
	void addSphereParameter(double ax, double bx, double cx, double r);
	//Add Plane parameter........
	void addPlaneParameter(PointCollection* PolygonPts, double X1, double Y1, double Z1, double X2, double Y2, double Z2, double X3, double Y3, double Z3);
	//Add 3D Line parameter........
	void addLine3DParameter(double X1, double Y1, double Z1, double X2, double Y2, double Z2);
	// Add point parameter.......
	void addPointparameter(double X1, double Y1, double Z1);
	//get generatrix coordinates........
	void getgeneratrix(double X1, double Y1, double Z1, double X2, double Y2, double Z2, double r1, double r2);
	//get parameter and directory list for point entity.....
	void getstring116(double X1, double Y1, double Z1);
	// get parameter and directory list......
	void getstring110(double dbl[], int size);
	//get directrixstring..........
	void getstring(std::string *result, double xyz[], int size);
	// get string for surface of revolution entity.......
	void getstring120(std::string S1, std::string S2);
    // get string for tabulated cylinder entity.......
	void getstring122(std::string S1, double X1, double Y1, double Z1);
	// get string for parametric curve surface entity........
	void getstring142(std::string S1, std::string surface);
	// get string for curve on  parametric surface entity........
	void getstring102(std::list<std::string> S1);
	// get string for trimmed curve surface entity.........
	void getstring144(std::string S1, std::string S2);
	// get upper disc of cylinder............
	void get_upper_disc();
	// get lower disc of cylinder......
	void get_lower_disc();
	//get arc in 3D....
	void get_arc(double r, std::string trans2);
	//get string 124......
	void getstring124(double x, double y, double z);
	//add post space...........
	void postspace(std::string *result, std::string inpstr, int size);
	//add pre space.........
	void prespace(std::string *result, std::string inpstr, int size);
	//split string in two parts............
	void split_string(std::string *result1, std::string *result2, std::string inpstr);
	bool generateDXFFile(char* filename);

};


//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!