#include "StdAfx.h"
#include "DimLineToSphereDistance.h"
#include "..\Shapes\Sphere.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLineToSphereDistance::DimLineToSphereDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0001", __FILE__, __FUNCSIG__); }
}

DimLineToSphereDistance::DimLineToSphereDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0002", __FILE__, __FUNCSIG__); }
}

DimLineToSphereDistance::~DimLineToSphereDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0003", __FILE__, __FUNCSIG__); }
}

void DimLineToSphereDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimLineToSphereDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimLineToSphereDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOSPHEREDISTANCE;
		if(ParentShape2->ShapeType != RapidEnums::SHAPETYPE::SPHERE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		((Sphere*)ParentShape2)->getParameters(&SphereParam[0]);
		((Line*)ParentShape1)->getParameters(&LineParam[0]);
		((Line*)ParentShape1)->getEndPoints(&LineEndPoints[0]);
			 
		double dist = RMATH3DOBJECT->Distance_Line_Sphere(&LineParam[0], &SphereParam[0], NormalmeasureType());
		setDimension(dist);
		RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Spr(&LineParam[0], &SphereParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0006", __FILE__, __FUNCSIG__); }
}

void DimLineToSphereDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawFiniteline_SphereDistance3D(&LineParam[0], &LineEndPoints[0],  &SphereParam[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0007", __FILE__, __FUNCSIG__); }
}

void DimLineToSphereDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0008", __FILE__, __FUNCSIG__); }
}

void DimLineToSphereDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLineToSphereDistance::Clone(int n)
{
	try
	{
		DimLineToSphereDistance* Cdim = new DimLineToSphereDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLineToSphereDistance::CreateDummyCopy()
{
	try
	{
		DimLineToSphereDistance* Cdim = new DimLineToSphereDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLineToSphereDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimLineToSphereDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToSphereDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToSphereDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToSphereDistance& x)
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
			if(Tagname==L"DimLineToSphereDistance")
			{
			while(Tagname!=L"EndDimLineToSphereDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLineToSphereDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2SPH0013", __FILE__, __FUNCSIG__); }
}