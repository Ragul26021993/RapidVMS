#include "StdAfx.h"
#include "DimSphereToCircle3DDistance.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"

DimSphereToCircle3DDistance::DimSphereToCircle3DDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0001", __FILE__, __FUNCSIG__); }
}

DimSphereToCircle3DDistance::DimSphereToCircle3DDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0002", __FILE__, __FUNCSIG__); }
}

DimSphereToCircle3DDistance::~DimSphereToCircle3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0003", __FILE__, __FUNCSIG__); }
}

void DimSphereToCircle3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimSphereToCircle3DDistance::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		distno++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(distno, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimSphereToCircle3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double Dist;
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCIRCLE3DDISTANCE;
		if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		((Sphere*)ParentShape2)->getParameters(&SphereParam[0]);
		((Circle*)ParentShape1)->getParameters(&Circle3D[0]);
		double CirclePlane[4] = {Circle3D[3], Circle3D[4], Circle3D[5], (Circle3D[0] * Circle3D[3] + Circle3D[1] * Circle3D[4] + Circle3D[2] * Circle3D[5])};
		Dist = RMATH3DOBJECT->Distance_Plane_Sphere(&CirclePlane[0], &SphereParam[0], NormalmeasureType());
		setDimension(Dist);
		RMATH3DOBJECT->TransformationMatrix_Dist_Pln2Spr(&CirclePlane[0], &SphereParam[0], &Circle3D[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0006", __FILE__, __FUNCSIG__); }
}

void DimSphereToCircle3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double CirclePlane[4] = {Circle3D[3], Circle3D[4], Circle3D[5], Circle3D[0] * Circle3D[3] + Circle3D[1] * Circle3D[4] + Circle3D[2] * Circle3D[5]};
	    double SprPt[3];
		RMATH3DOBJECT->MeasureModeCalc_Plane_Sphere(CirclePlane, SphereParam, NormalmeasureType(), SprPt);
		GRAFIX->drawPoint_Circle3DDistance3D(SprPt, Circle3D,  doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0],  &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0007", __FILE__, __FUNCSIG__); }
}

void  DimSphereToCircle3DDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0008", __FILE__, __FUNCSIG__); }
}

void  DimSphereToCircle3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase*  DimSphereToCircle3DDistance::Clone(int n)
{
	try
	{
		 DimSphereToCircle3DDistance* Cdim = new  DimSphereToCircle3DDistance(_T("Dist"));
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase*  DimSphereToCircle3DDistance::CreateDummyCopy()
{
	try
	{
		 DimSphereToCircle3DDistance* Cdim = new  DimSphereToCircle3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0010", __FILE__, __FUNCSIG__); return NULL; }
}

void  DimSphereToCircle3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os,  DimSphereToCircle3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<< "DimSphereToCircle3DDistance"<<endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os << "ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<< "EndDimSphereToCircle3DDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is,  DimSphereToCircle3DDistance& x)
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
			if(Tagname==L"DimSphereToCircle3DDistance")
			{
			while(Tagname!=L"EndDimSphereToCircle3DDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimSphereToCircle3DDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0013", __FILE__, __FUNCSIG__); }
}