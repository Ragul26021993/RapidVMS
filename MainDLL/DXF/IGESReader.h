#pragma once
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

class DimBase;
class Shape;
class PointCollection;

class IGESReader
{
private:
	ifstream IGESData;
	Shape* dxfShape;
	int ncount;
	double multipleValue;
	PointCollection *Allcloud_points;
	bool rapidi, alibre, param, cflag, cloudPointFlag;
	int rap1, ali1, rap2, rap3, checkduplicate, check102, checkduplicate116, checkduplicate120;
  
	bool cylinderflag, sphereflag;
	std::list<std::string> mylist, mylist1, mylist12, templiststr;
	std::string   liststr1,liststr2, liststr3, liststr4, str17;
	std::string delimiter1, rlimiter1, unittype1, directrix1, generatrix1, tmatrix1;
	std::list<std::string> temp2, temp3;
	
public:
	//Constructor and destructor..//
	IGESReader();
	~IGESReader();
	//Load the DXF file..//
	bool LoadFile(char* filename);
	
private:
	//function to remove spaces....//
	void removespace(std::string *result, std::string inpstr);
	//function to split string....//
	void split(std::list <std::string> *temp2, std::string str1, char delim );
	//function to read the cylinder and cone  parameter...//
	void Cylinder_Coneparameter();
	//function to read the 3DLine  parameter...//
	void getLine3D_parameter(double *endpts);
	//function to get polygon parameter...//
	void getPolygon_parameter(PointCollection *polygonpoints);
	//function to get parameter section data...//
	void getParameterData();
	//function to read sphere parameter...//
	void Sphereparameter();
	//function to get point parameter.......
	void PointParameter(PointCollection *polygonpoints);
	//function to get generatrix and direcrix.....for 120 entity...
	void getGeneratrix_Directrix120(std::list<std::string>::iterator itr);
	//function to get generatrix and direcrix.....for 122 entity...
	void getGeneratrix_Directrix122(std::list<std::string>::iterator itr);
	//function to read polygon points.....
	void getPolygon_Points(std::list<std::string>::iterator itr);
	//function to get cloud points data......
	void getAllCloudPoints_Collection(std::list<std::string>::iterator itr);


//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
};