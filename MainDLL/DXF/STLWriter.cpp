# include "stdafx.h"
#include "STLWriter.h"
#include "..\Engine\RCadApp.h"


//Constructor ..//
STLWriter::STLWriter()
{
	solid = "solid ascii"; facetNormal = "  facet normal"; outerLoop = "    outer loop"; vertex = "      vertex  "; endloop = "    endloop";
	endfacet = "  endfacet"; endsolid = "endsolid";
}

//Destructor...//
STLWriter::~STLWriter()
{
}

void STLWriter::AddSurfaceParameter(double* param)
{
	try
	{
		createString(facetNormal, &param[9]);
		StlList.push_back(outerLoop);
		createString(vertex, &param[0]);
		createString(vertex, &param[3]);
		createString(vertex, &param[6]);
		StlList.push_back(endloop);
		StlList.push_back(endfacet);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STLW001", __FILE__, __FUNCSIG__);  }
}

void STLWriter::createString(std::string str, double* data)
{
	try
	{
		std::string S1, S2, S3, finalStr;
		char cd[15] = "\0";
		if(abs(data[0]) > 100000)
			S1 = "0.0";
		else
		{
			RMATH2DOBJECT->Double2String(data[0], 10, cd, false); 
			S1 = cd;
		}
		if(abs(data[1]) > 100000)
			S2 = "0.0";
		else
		{
			RMATH2DOBJECT->Double2String(data[1], 10, cd, false); 
			S2 = cd;
		}
		if(abs(data[2]) > 100000)
			S3 = "0.0";
		else
		{
			RMATH2DOBJECT->Double2String(data[2], 10, cd, false); 
			S3 = cd;
		}
		finalStr = str + " " + S1 + " " + S2 + " " + S3;
		StlList.push_back(finalStr);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STLW002", __FILE__, __FUNCSIG__);  }
}

//Generate the Stl file///
void STLWriter::generateSTLFile(char* filename)
{
	try
	{	 
		 STLDataWriter.open(filename);
		 if(!STLDataWriter) return;
		 STLDataWriter << solid << endl;

		 for(std::list<std::string>::iterator SGlistIterator = StlList.begin(); SGlistIterator != StlList.end(); SGlistIterator++)
			 STLDataWriter << *SGlistIterator << endl;

		 STLDataWriter << endsolid << endl;
		 STLDataWriter.close();
   		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STLW003", __FILE__, __FUNCSIG__);}
}