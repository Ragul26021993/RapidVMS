#include "StdAfx.h"
#include "DimAreaMeasurement.h"
#include "..\Shapes\AreaShape.h"
#include "..\Engine\RCadApp.h"

DimAreaMeasurement::DimAreaMeasurement(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0001", __FILE__, __FUNCSIG__); }
}

DimAreaMeasurement::DimAreaMeasurement(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0002", __FILE__, __FUNCSIG__); }
}

DimAreaMeasurement::~DimAreaMeasurement()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0003", __FILE__, __FUNCSIG__); }
}

void DimAreaMeasurement::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT;
		center[0] = 0; center[1] = 0; center[2] = 0;
		IsValid(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimAreaMeasurement::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimAreaMeasurement::CalculateMeasurement(Shape* s1)
{
	try
	{		
		this->ParentShape1 = s1;
		this->setDimension(((AreaShape*)ParentShape1)->TotArea);
		Vector Point = *((AreaShape*)ParentShape1)->getCenter();
		center[0] = Point.getX(); center[1] = Point.getY(); center[2] = Point.getZ();
		RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(center, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0006", __FILE__, __FUNCSIG__); }
}

void DimAreaMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(center, 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0007", __FILE__, __FUNCSIG__); }
}

void DimAreaMeasurement::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

void DimAreaMeasurement::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

DimBase* DimAreaMeasurement::Clone(int n)
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimAreaMeasurement::CreateDummyCopy()
{
	try
	{
		DimAreaMeasurement* Cdim = new DimAreaMeasurement(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimAreaMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimAreaMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimAreaMeasurement"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"EndDimAreaMeasurement"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimAreaMeasurement& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		int n = 0;
		if(Tagname==L"DimAreaMeasurement")
		{
		while(Tagname!=L"EndDimAreaMeasurement")
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
				}
			}
			x.UpdateMeasurement();
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
		
		return is;		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMAREA0012", __FILE__, __FUNCSIG__); return is; }
}