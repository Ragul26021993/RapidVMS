#include "StdAfx.h"
#include "DimPointToSphereDistance.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"

DimPointToSphereDistance::DimPointToSphereDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0001", __FILE__, __FUNCSIG__); }
}

DimPointToSphereDistance::DimPointToSphereDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0002", __FILE__, __FUNCSIG__); }
}

DimPointToSphereDistance::~DimPointToSphereDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0003", __FILE__, __FUNCSIG__); }
}

void DimPointToSphereDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToSphereDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToSphereDistance::CalculateMeasurement(Shape *ParentShape1, Vector* pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = ParentShape1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOSPHEREDISTANCE;

		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()},Dist;
		((Sphere*)ParentShape1)->getParameters(&SphereParam[0]);
		Dist = RMATH3DOBJECT->Distance_Point_Sphere(&point[0], &SphereParam[0], NormalmeasureType());
		setDimension(Dist);
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Spr(&point[0], &SphereParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0006", __FILE__, __FUNCSIG__); }
}

void DimPointToSphereDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		GRAFIX->drawPoint_SphereDistance3D(&point[0], &SphereParam[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0007", __FILE__, __FUNCSIG__); }
}

void DimPointToSphereDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			ParentShape1 = (Shape*)sender;
		CalculateMeasurement(ParentShape1, ParentPoint1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0008", __FILE__, __FUNCSIG__); }
}

void DimPointToSphereDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPointToSphereDistance::Clone(int n)
{
	try
	{
		DimPointToSphereDistance* Cdim = new DimPointToSphereDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0009", __FILE__, __FUNCSIG__); return NULL;}
}

DimBase* DimPointToSphereDistance::CreateDummyCopy()
{
	try
	{
		DimPointToSphereDistance* Cdim = new DimPointToSphereDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToSphereDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToSphereDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToSphereDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentPoint1:values"<<endl<< (*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os<<"EndDimPointToSphereDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToSphereDistance& x)
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
			if(Tagname==L"DimPointToSphereDistance")
			{
			while(Tagname!=L"EndDimPointToSphereDistance")
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
						if(Tagname==L"ParentPoint1")
						{
							x.ParentPoint1 = new Vector(0,0,0);
							is >> (*static_cast<Vector*>(x.ParentPoint1));
						}	
						else if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToSphereDistance& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.ParentPoint1 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint1));
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2SPH0013", __FILE__, __FUNCSIG__); }
}