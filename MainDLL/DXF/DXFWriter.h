//DXF writer class... 
//Writes the shape parameters to a dxf file format...//
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "../Shapes/Vector.h"
#include "../Shapes/SinglePoint.h"
#include "../Engine/PointCollection.h"
using namespace std;

class DXFWriter
{
//Variable declaration...//
private:
	list<string> DXFList;
	list<string>::iterator DXFListIterator;
	ofstream DXFDataWriter;
	char* filename;
	char cd[30];
	//Double to string conversion functions
	char* DoubletoString(double x);
	char* IntegertoString(int x);
	bool WriteHeader();
	bool WriteTail();
	int HandleCount;

public:
	//Constructor and destructor...//
	DXFWriter();
	~DXFWriter();

	//Add the point parameter...//
	void addPointparameter(double x1, double y1, double z1, int colorcode);
	//Add circle parameter..//
	void addCircleParameter(double cx, double cy, double cz, double crad, int colorcode);
	//Add the arc parameter...//
	void addArcParameter(double ax, double ay, double az, double arad, double startangle, double endangle, int colorcode);
	//Add the line parameter...//
	void addLineParameter(char* str, double lsx, double lsy, double lsz, double lex, double ley, double lez, int colorcode);
	//Add the line parameter...//
	void addXLineParameter(char* str, double lsx, double lsy, double lsz, double lex, double ley, double lez, int colorcode);
	//Add the line parameter...//
	void addRayParameter(char* str, double lsx, double lsy, double lsz, double lex, double ley, double lez, int colorcode);
	//Adds the text parameter...
	void addtextparameter(double ax, double bx, double cx, double width, double height, char* text, int colorcode);
	//Add polyline2D parameters...//
	void AddPolylineparameters2D(PointCollection* Pointlist, int colorcode);
	//Add polyline parameters...//
	void AddPolylineparameters(PointCollection* Pointlist, int colorcode);
	//Add Spline Parameter...
	void AddSplineParameter(PointCollection* Pointlist, int NumberOfKnots, double* Knots_Array, bool ClosedSpline, int ColorCode);
	//Set the common parameters to each shape...///
	void setCommonParameter(char* SubclassMarker, char* EntityName, double x1, double x2, double x3, int colorcode, bool flag = true);
	//Generate the dxf file..//
	bool generateDXFFile(char* filename);
	void ResetHandleCount();
};


//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!