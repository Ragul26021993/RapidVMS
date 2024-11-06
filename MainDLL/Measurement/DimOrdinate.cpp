#include "StdAfx.h"
#include "DimOrdinate.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

DimOrdinate::DimOrdinate(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{IsValid(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0001", __FILE__, __FUNCSIG__); }
}

DimOrdinate::DimOrdinate(bool simply):DimBase(false)
{
	try{IsValid(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0002", __FILE__, __FUNCSIG__); }
}

DimOrdinate::~DimOrdinate()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimOrdinate::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX)
			dimcnt = ordinatenoX++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY)
			dimcnt = ordinatenoY++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ)
			dimcnt = ordinatenoZ++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimOrdinate::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double cpnt[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		GRAFIX->drawDiametere3D(&cpnt[0], 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0005", __FILE__, __FUNCSIG__); }
}

void DimOrdinate::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	UpdateMeasurement();
}

void DimOrdinate::CalculateMeasurement(Vector *v1)
{
	try
	{
		this->ParentPoint1 = v1;
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				setDimension(ParentPoint1->getX());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				setDimension(ParentPoint1->getY());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
				setDimension(ParentPoint1->getZ());
				break;
		}
		double cpnt[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
	    RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(cpnt, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0006", __FILE__, __FUNCSIG__); }
}

void DimOrdinate::UpdateMeasurement()
{
	CalculateMeasurement(ParentPoint1);
}

DimBase* DimOrdinate::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX)
			myname = _T("Pt_X");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY)
			myname = _T("Pt_Y");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ)
			myname = _T("Pt_Z");
		DimOrdinate* Cdim = new DimOrdinate((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimOrdinate::CreateDummyCopy()
{
	try
	{
		DimOrdinate* Cdim = new DimOrdinate(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimOrdinate::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0009", __FILE__, __FUNCSIG__); }
}

int DimOrdinate::ordinatenoX = 0;
int DimOrdinate::ordinatenoY = 0;
int DimOrdinate::ordinatenoZ = 0;
void DimOrdinate::reset()
{
	ordinatenoX = 0;
	ordinatenoY = 0;
	ordinatenoZ = 0;
}

wostream& operator<<( wostream& os, DimOrdinate& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimOrdinate"<<endl;
		os <<"ParentPoint1:values"<<endl<< (*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os<<"EndDimOrdinate"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimOrdinate& x)
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
			if(Tagname==L"DimOrdinate")
			{
			while(Tagname!=L"EndDimOrdinate")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimOrdinate& x)
{
	try
	{
		x.ParentPoint1 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint1));
		x.UpdateMeasurement();		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMORD0011", __FILE__, __FUNCSIG__); }
}