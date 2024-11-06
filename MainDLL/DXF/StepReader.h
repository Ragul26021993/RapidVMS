//Dxf reader class..//
//Reads the dxf file and instantiate the respective shape/ measurement classes
#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <map>
#include "..\Shapes\Shape.h"
#include "..\Measurement\DimBase.h"
#include "..\Shapes\SinglePoint.h"
#include "..\Engine\PointCollection.h"

using namespace std;

class StepReader
{
private:
	//map to hold the shapes with ID(relation between shape and measurement..//)
	map<std::string, Shape*>ParentShapeList1;
	map<std::string, Shape*>ParentShapeList2;
	ifstream STEPData;
	Shape* dxfShape;
	DimBase* dxfdim;

	double multipleValue;
	int ali1;
  
	bool cylinderflag, sphereflag, param;
	
public:
	//Constructor and destructor..//
	StepReader();
	~StepReader();
	//Load the DXF file..//
	bool LoadFile(char* filename);
	
private:
	
	//function to read the cylinder and cone  parameter...//
	void Cylinder_Coneparameter(bool isCylinder);
	//function to read sphere parameter...//
	void Sphereparameter();

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
};