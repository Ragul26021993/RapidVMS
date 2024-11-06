//STL reader class..//
//Reads the stl file and instantiate the respective shape class
#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <list>

using namespace std;

class SinglePoint;

class StlReader
{
private:
	ifstream STLData;
	double dirCosine[3];
	void SurfaceParameter();
	//function to split string....//
	void split(std::list<std::string> *temp2, std::string str1, char delim = ' ');
	void AddTriangles(SinglePoint* Pt1, SinglePoint* Pt2, SinglePoint* Pt3);
	void getCordinate(std::list<std::string> liststr, double* dValue, bool normalVector);
	std::map<int, double*> TrianglePtCollection;
	int TriangleCnt;
	void AddTriangularSurface();
public:
	//Constructor and destructor..//
	StlReader();
	~StlReader();
	//Load the DXF file..//
	bool LoadFile(char* filename);

//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
};