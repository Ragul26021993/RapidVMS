//STL writer class... 
//Writes the shape parameters to a stl file format...//
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
using namespace std;

class STLWriter
{
//Variable declaration...//
	ofstream STLDataWriter;
	std::list<std::string> StlList;
	std::string solid, facetNormal, outerLoop, vertex, endloop, endfacet, endsolid;
	void createString(std::string str, double* data);
public:
	//Constructor and destructor...//
	STLWriter();
    ~STLWriter();
	void AddSurfaceParameter(double* param);
	void generateSTLFile(char* filename);

};


//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!