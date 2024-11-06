//Include required headerfiles...//
#include "stdafx.h"
#include "StlReader.h"
#include "..\Engine\RCadApp.h"
#include "..\MainDLL.h"
#include "..\Shapes\TriangularSurface.h"
#include "..\Shapes\SinglePoint.h"
#include "DXFExpose.h"

//Constructor..//
StlReader::StlReader()
{  
	dirCosine[0] = 0; dirCosine[1] = 0; dirCosine[2] = 0;
	TriangleCnt = 0;
}

//Destructor...//
StlReader::~StlReader()
{
	try
	{
		for(int i = TriangleCnt - 1; i >= 0; i--)
		{
			double* pt = TrianglePtCollection[i];
			TrianglePtCollection.erase(i);
			delete [] pt;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STL0000", __FILE__, __FUNCSIG__); }
}

//Load the iges file.///
bool StlReader::LoadFile(char* filename)
{
	try
	{
		MAINDllOBJECT->ShowHideWaitCursor(1);
		std::string ParentStr;
		STLData.open(filename);
		if(!STLData)
			return false;
		//Read till end of the file///
		while(!STLData.eof())
		{
			getline(STLData, ParentStr);
			int ReadVertex = -1, file_End = -1;
			ReadVertex = ParentStr.find("facet normal");
			file_End = ParentStr.find("endsolid");
			if(file_End > -1)
				break;
			if(ReadVertex  > -1)
			{
				std::list<std::string> liststr;
				split(&liststr, ParentStr);
				if(liststr.size() > 4)
					getCordinate(liststr, dirCosine, true);
				SurfaceParameter();
			}
		}
		STLData.close();
		AddTriangularSurface();
		MAINDllOBJECT->ShowHideWaitCursor(2);
		MAINDllOBJECT->ZoomToWindowExtents(2);
		return true;
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STL0001", __FILE__, __FUNCSIG__); MAINDllOBJECT->ShowHideWaitCursor(2); return false;}
}

void StlReader::SurfaceParameter()
{
	try
	{
		std::string VertexStr;
		int vertexfound = -1;
		getline(STLData, VertexStr);
		vertexfound = VertexStr.find("outer loop");
		if(vertexfound > -1)
		{
			std::map<int, SinglePoint*> PtsCollection;
			int Loopend = -1, facetend = -1, PtsCount = 0;
			std::string Linestr;
			do
			{
				int ptfound = -1;
				getline(STLData, Linestr);
				Loopend = Linestr.find("endloop");
				ptfound = Linestr.find("vertex");
				facetend = Linestr.find("endfacet");
				if(ptfound > -1)
				{
					std::list<std::string> liststr;
					split(&liststr, Linestr);
					if(liststr.size() > 3)
					{
						double dValue[3] = {0};
						getCordinate(liststr, dValue, false);
						SinglePoint* Spt = new SinglePoint();
						Spt->SetValues(dValue[0], dValue[1], dValue[2]);
						PtsCollection[PtsCount++] = Spt;
					}				
				}
			}while(Loopend < 0 && !STLData.eof() && facetend < 0);
			if(PtsCount == 3)
			{
				AddTriangles(PtsCollection[0], PtsCollection[1], PtsCollection[2]);
				for(int i = 2; i >= 0; i--)
				{
					SinglePoint* Spt1 = PtsCollection[i];
					PtsCollection.erase(i);
					delete Spt1;
				}
			}
			else if(PtsCount == 4)
			{
				AddTriangles(PtsCollection[0], PtsCollection[1], PtsCollection[2]);
				AddTriangles(PtsCollection[2], PtsCollection[3], PtsCollection[0]);
				for(int i = 3; i >= 0; i--)
				{
					SinglePoint* Spt1 = PtsCollection[i];
					PtsCollection.erase(i);
					delete Spt1;
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STL0002", __FILE__, __FUNCSIG__); }
}

 //remove continuous delimiter...
 void StlReader::split(std::list<std::string> *temp, std::string str1, char delim)	
{
	try
	{
		temp->clear();
		std::string tmp = "";
		for(std::string::iterator itr1 = str1.begin(); itr1 != str1.end(); itr1++)
		{
    		if((const char)*itr1 != delim)
			{
				tmp += *itr1;
			}
			else 
			{
				if(tmp != "")
				{
					temp->push_back(tmp);
					tmp = "";
				}
			}		
		}
		if(tmp != "")
			temp->push_back(tmp);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STL0003", __FILE__, __FUNCSIG__); }
}

 void StlReader::AddTriangles(SinglePoint* Pt1, SinglePoint* Pt2, SinglePoint* Pt3)
 {
	 try
	 {
		 double *TriangleParam = new double[12];
		 TriangleParam[0] = Pt1->X; TriangleParam[1] = Pt1->Y; TriangleParam[2] = Pt1->Z;
		 TriangleParam[3] = Pt2->X; TriangleParam[4] = Pt2->Y; TriangleParam[5] = Pt2->Z;
		 TriangleParam[6] = Pt3->X; TriangleParam[7] = Pt3->Y; TriangleParam[8] = Pt3->Z;
		 TriangleParam[9] = dirCosine[0]; TriangleParam[10] = dirCosine[1]; TriangleParam[11] = dirCosine[2];
		 TrianglePtCollection[TriangleCnt++] = TriangleParam;
	 }
	 	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STL0004", __FILE__, __FUNCSIG__); }
}

 void StlReader::getCordinate(std::list<std::string> liststr, double* dValue, bool normalVector)
{
	 try
	 {
		 std::list<std::string>::iterator itr1;
		 itr1 = liststr.begin();
		 itr1++;
		 if(normalVector)
			 itr1++;
		 dValue[0] = atof((const char*)(*itr1).c_str());
		 itr1++;
		 dValue[1] = atof((const char*)(*itr1).c_str());
		 itr1++;
		 dValue[2] = atof((const char*)(*itr1).c_str());
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STL0005", __FILE__, __FUNCSIG__);}
}

 void StlReader::AddTriangularSurface()
{
	 try
	 {
		 Shape* dxfShape;
		 dxfShape = new TriangularSurface(false);
		 dxfShape->IsDxfShape(true);
		 dxfShape->DoNotSaveInPP = true;
		 for(int i = 0; i < TriangleCnt; i++)
			 ((TriangularSurface*)dxfShape)->AddNewTriangle(TrianglePtCollection[i]);
		 DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
	 }
	 	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STL0006", __FILE__, __FUNCSIG__); }
}