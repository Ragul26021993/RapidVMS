//Include required headerfiles...//
#include "stdafx.h"
#include "StepReader.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\ShapeHeaders.h"

#include "..\MainDLL.h"
#include "..\Shapes\Shape.h"
#include "..\Measurement\DimBase.h"
#include "..\Measurement\MeasurementHeaders.h"
#include "..\Helper\General.h"
#include "..\Actions\AddDimAction.h"
#include "math.h"
#include "DXFExpose.h"

//Constructor..//
StepReader::StepReader()
{  
	 ali1 = 1; multipleValue = 0;param = false;
}

//Destructor...//
StepReader::~StepReader()
{
}

//Load the iges file.///
bool StepReader::LoadFile(char* filename)
{
	try
	{
		MAINDllOBJECT->ShowHideWaitCursor(1);
		ParentShapeList2.clear();
		ParentShapeList1.clear();
		STEPData.open(filename);
		string line;
		if(!STEPData)
			return false;
		while(!STEPData.eof())
		{
			try
			{
				STEPData>>line;
				if(line != "")
				{
					if(line == "Unit")
					{
						int unittype;
						STEPData >> unittype;
		                if(unittype == 10)
		                {
			                multipleValue = 10;
	                  	}
						else 
		                {
			                multipleValue = 1;
		                }
						

					}
					if(line == "Sphere")
					{
						line = "";
						Sphereparameter();
						DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
					}
					if(line == "Cylinder")
					{
						line = "";
						Cylinder_Coneparameter(true);
						DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
					}
					if(line == "Cone")
					{
						line = "";
						Cylinder_Coneparameter(false);
						DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
					}
				}
			}
			catch(...)
			{
			}
		}
		MAINDllOBJECT->ShowHideWaitCursor(2);
		MAINDllOBJECT->ZoomToWindowExtents(2);
		return true;
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STEO0001", __FILE__, __FUNCSIG__); }
}

void StepReader::Cylinder_Coneparameter(bool isCylinder)
{   
	try
	{
		double x1,y1,z1,x2,y2,z2;
			double Cradius1,Cradius2;
		if(isCylinder)
		{
			STEPData >> Cradius1;
			Cradius2 = Cradius1;
			STEPData >> x1,STEPData >> y1,STEPData >> z1,STEPData >> x2,STEPData >> y2,STEPData >> z2;
			Cylinder* mycylinder = new Cylinder();
			mycylinder->Radius1 (Cradius1 * multipleValue);
			mycylinder->Radius2 (Cradius2 * multipleValue);
			mycylinder->SetCenter1 (Vector(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue));
			mycylinder->SetCenter2 (Vector(x2 * multipleValue, y2 * multipleValue, z2 * multipleValue));
			mycylinder->Length(sqrt(pow(x2 * multipleValue - x1 * multipleValue, 2) + pow(y2 * multipleValue - y1 * multipleValue, 2) + pow(z2 * multipleValue - z1 * multipleValue, 2)));
			mycylinder->dir_l((x2 * multipleValue - x1 * multipleValue) / mycylinder->Length());
			mycylinder->dir_m((y2 * multipleValue - y1 * multipleValue) / mycylinder->Length());
			mycylinder->dir_n((z2 * multipleValue - z1 * multipleValue) / mycylinder->Length());
			dxfShape = mycylinder;
			dxfShape->IsValid(true);
			ali1++;
			param = true;
		}
		else
		{
			double tempangle ;
			STEPData >> tempangle;
			STEPData >> Cradius2;
			Cradius1 = 0;
			STEPData >> x1, STEPData >> y1, STEPData >> z1, STEPData >> x2, STEPData >> y2, STEPData >> z2;
			Cone* mycone = new Cone();
			mycone->Radius1 (Cradius1 * multipleValue);
			mycone->Radius2 (Cradius2 * multipleValue);
			mycone->SetCenter1 (Vector(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue));
			mycone->SetCenter2 (Vector(x2 * multipleValue, y2 * multipleValue, z2 * multipleValue));
			mycone->Length(sqrt(pow(x2 * multipleValue - x1 * multipleValue, 2) + pow(y2 * multipleValue - y1 * multipleValue, 2) + pow(z2 * multipleValue - z1 * multipleValue, 2)));
			mycone->dir_l((x2 * multipleValue - x1 * multipleValue) / mycone->Length());
			mycone->dir_m((y2 * multipleValue - y1 * multipleValue) / mycone->Length());
			mycone->dir_n((z2 * multipleValue - z1 * multipleValue) / mycone->Length());
			mycone->ApexAngle(tempangle);
			dxfShape = mycone;
			dxfShape->IsValid(true);
			ali1++;
			param = true;
		}

   }
catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STEP0002", __FILE__, __FUNCSIG__); }
}
void StepReader::Sphereparameter()
{
	try 
	{
		double x1, y1, z1, Sradius;
		STEPData >> Sradius;
		STEPData >> x1, STEPData >> y1, STEPData >> z1;
		Sphere* mysphere = new Sphere();
		mysphere->Radius (Sradius * multipleValue);
		mysphere->setCenter (Vector(x1 * multipleValue,y1 * multipleValue, z1 * multipleValue));
		dxfShape = mysphere;
		dxfShape->IsValid(true);
		ali1++;
			
    }
catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("STEP0003", __FILE__, __FUNCSIG__); }
}




	