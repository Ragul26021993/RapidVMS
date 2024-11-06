#include "StdAfx.h"
#include "DimCylindricity.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"

DimCylindricity::DimCylindricity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0001", __FILE__, __FUNCSIG__); }
}

DimCylindricity::DimCylindricity(bool simply):DimBase(false)
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0002", __FILE__, __FUNCSIG__); }
}

DimCylindricity::~DimCylindricity()
{
}

TCHAR* DimCylindricity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY || Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED)
			dimcnt = cylindricitycnt++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC)
			dimcnt = micno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC)
			dimcnt = mccno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(cylindricitycnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0003", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimCylindricity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(&CylinderParam[0], &CylinderEndPoints[0], getCDimension(), getModifiedName(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0004", __FILE__, __FUNCSIG__); }
}

void DimCylindricity::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;	
		((Cylinder*)ParentShape1)->getParameters(&CylinderParam[0]);
		((Cylinder*)ParentShape1)->getEndPoints(&CylinderEndPoints[0]);
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY:
				setDimension(((Cylinder*)s1)->Cylindricity());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED:
				setDimension(((Cylinder*)s1)->FilteredCylindricity());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC:
				setDimension(((Cylinder*)s1)->MinInscribedia());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC:
				setDimension(((Cylinder*)s1)->MaxCircumdia());
				break;
		}
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0005", __FILE__, __FUNCSIG__); }
}

void DimCylindricity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimCylindricity::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimCylindricity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY || this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED)
			myname = _T("Cyl");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC)
			myname = _T("Cylmic");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC)
			myname = _T("Cylmcc");
		DimCylindricity* Cdim = new DimCylindricity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0006", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCylindricity::CreateDummyCopy()
{
	try
	{
		DimCylindricity* Cdim = new DimCylindricity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0007", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCylindricity::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0008", __FILE__, __FUNCSIG__); }
}

int DimCylindricity::cylindricitycnt = 0;
int DimCylindricity::micno = 0;
int DimCylindricity::mccno = 0;
void DimCylindricity::reset()
{
	cylindricitycnt = 0;
	micno = 0;
	mccno = 0;
}

wostream& operator<<( wostream& os, DimCylindricity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCylindricity"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;		
		os<<"EndDimCylindricity"<<endl;	
		return os;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0009", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DimCylindricity& x)
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
			if(Tagname==L"DimCylindricity")
			{
				while(Tagname!=L"EndDimCylindricity")
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
						std::string Val(TagVal.begin(), TagVal.end() );
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0010", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCylindricity& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYTY0010", __FILE__, __FUNCSIG__);}
}