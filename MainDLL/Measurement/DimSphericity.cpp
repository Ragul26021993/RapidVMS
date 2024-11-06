#include "StdAfx.h"
#include "DimSphericity.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"

DimSphericity::DimSphericity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try
	{
		IsValid(false);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0001", __FILE__, __FUNCSIG__); }
}

DimSphericity::DimSphericity(bool simply):DimBase(false)
{
	try
	{
		IsValid(false);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0002", __FILE__, __FUNCSIG__); }
}

DimSphericity::~DimSphericity()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimSphericity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY)
			dimcnt = sphericitycnt++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC)
			dimcnt = micno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC)
			dimcnt = mccno++;
		dimcnt++;
		sphericitycnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(sphericitycnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimSphericity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(&SphereParam[0], SphereParam[3] * 2, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0005", __FILE__, __FUNCSIG__); }
}

void DimSphericity::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;		
		((Sphere*)ParentShape1)->getParameters(&SphereParam[0]);
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY:
				setDimension(((Sphere*)s1)->Sphericity());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC:
				setDimension(((Sphere*)s1)->MinInscribedia());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC:
				setDimension(((Sphere*)s1)->MaxCircumdia());
				break;
		}
		RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(SphereParam, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0006", __FILE__, __FUNCSIG__); }
}

void DimSphericity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimSphericity::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

int DimSphericity::sphericitycnt = 0;
int DimSphericity::micno = 0;
int DimSphericity::mccno = 0;
void DimSphericity::reset()
{
	micno = 0;
	mccno = 0;
	sphericitycnt = 0;
}

DimBase* DimSphericity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY)
			myname = _T("Sph");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC)
			myname = _T("Sphmic");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC)
			myname = _T("Sphmcc");
		DimSphericity* Cdim = new DimSphericity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimSphericity::CreateDummyCopy()
{
	try
	{
		DimSphericity* Cdim = new DimSphericity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimSphericity::CopyMeasureParameters(DimBase* Cdim)
{
	try{}

	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimSphericity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimSphericity"<<endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"EndDimSphericity"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DimSphericity& x)
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
			if(Tagname==L"DimSphericity")
			{
			while(Tagname!=L"EndDimSphericity")
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
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0011", __FILE__, __FUNCSIG__); return is;}
}
void ReadOldPP(wistream& is, DimSphericity& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH0011", __FILE__, __FUNCSIG__); }
}