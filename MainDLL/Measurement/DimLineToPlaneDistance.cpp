#include "StdAfx.h"
#include "DimLineToPlaneDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLineToPlaneDistance::DimLineToPlaneDistance(bool simply):DimBase(false)
{
    try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0002", __FILE__, __FUNCSIG__); }
}

DimLineToPlaneDistance::~DimLineToPlaneDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0003", __FILE__, __FUNCSIG__); }
}

void DimLineToPlaneDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
		memset(LineParam, 0, sizeof(double) * 6);
		LineParam[5] = 1;
		memset(PlaneParam, 0, sizeof(double) * 4);
		PlaneParam[2] = 1;
		memset(LineEndPoints, 0, sizeof(double) * 6);
		memset(PlaneEndPoints, 0, sizeof(double) * 12);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0004", __FILE__, __FUNCSIG__); }
}

void DimLineToPlaneDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Line_PlaneMType)
	{
		case LINE_PLANEMEASURETYPE ::ANGLE:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case LINE_PLANEMEASURETYPE::DISTANCE:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
	}
	 setModifiedName(myname);
}

void DimLineToPlaneDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		bool ProjectLine = false;
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOPLANEDISTANCE;
		this->DistanceMeasurement(true);
		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::PLANE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}

		((Plane*)ParentShape1)->getParameters(&PlaneParam[0]);
		((Plane*)ParentShape1)->getEndPoints(&PlaneEndPoints[0]);
		((Line*)ParentShape2)->getParameters(&LineParam[0]);
		((Line*)ParentShape2)->getEndPoints(&LineEndPoints[0]);
		if(((Line*)ParentShape2)->LineType == RapidEnums::LINETYPE::INFINITELINE)
		{
		     for(int i = 0; i < 3; i++)
			 {
				 LineParam[i] = LineParam[i] / pow(10.0, 5);
				 LineEndPoints[0 + i] /= pow(10.0, 5);
				 LineEndPoints[3 + i] /= pow(10.0, 5);
			 }
		}

		if(RMATH3DOBJECT->Checking_Parallel_Line_to_Plane(&LineParam[0], &PlaneParam[0]))
		{
			RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Pln(&LineParam[0], &PlaneParam[0], &PlaneEndPoints[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Distance_Line_Plane(&LineParam[0], &PlaneParam[0]);
			setDimension(Dist);
			Line_PlaneMType = LINE_PLANEMEASURETYPE::DISTANCE;
		}
		else
		{
			this->DistanceMeasurement(false);
			RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Pln(&LineParam[0], &PlaneParam[0], &PlaneEndPoints[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Angle_Line_Plane(&LineParam[0], &LineEndPoints[0], &PlaneParam[0], &PlaneEndPoints[0], MeasurementPlane, &DimSelectionLine[0], ProjectLine);
			this->setAngularDimension(Dist);
			Line_PlaneMType = LINE_PLANEMEASURETYPE::ANGLE;
		}

		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0006", __FILE__, __FUNCSIG__); }
}

void DimLineToPlaneDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawFiniteline_PlaneDistOrAng3D(&LineParam[0], &LineEndPoints[0], &PlaneParam[0], &PlaneEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], DistanceMeasurement(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0007", __FILE__, __FUNCSIG__); }
}

void DimLineToPlaneDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			CalculateMeasurement((Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			CalculateMeasurement(ParentShape1, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0008", __FILE__, __FUNCSIG__); }
}

void DimLineToPlaneDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLineToPlaneDistance::Clone(int n)
{
	try
	{
		DimLineToPlaneDistance* Cdim = new DimLineToPlaneDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLineToPlaneDistance::CreateDummyCopy()
{
	try
	{
		DimLineToPlaneDistance* Cdim = new DimLineToPlaneDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLineToPlaneDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimLineToPlaneDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToPlaneDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToPlaneDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToPlaneDistance& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"DimLineToPlaneDistance")
			{
			while(Tagname!=L"EndDimLineToPlaneDistance")
				{	
					std::wstring Linestr;				
					is >> Linestr;
					if(is.eof())
					{						
						MAINDllOBJECT->PPLoadSuccessful(false);
						break;
					}
					int ColonIndx = Linestr.find(':');
					if(ColonIndx==-1)
					{
						Tagname=Linestr;
					}
					else
					{
						std::wstring Tag = Linestr.substr(0, ColonIndx);
						std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname=Tag;												
						if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}		
						else if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}	
					}
				}
				x.UpdateMeasurement();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLineToPlaneDistance& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0013", __FILE__, __FUNCSIG__); }
}