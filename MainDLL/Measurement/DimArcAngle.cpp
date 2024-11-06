#include "StdAfx.h"
#include "DimArcAngle.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"

DimArcAngle::DimArcAngle(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try
	{
		this->DistanceMeasurement(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0001", __FILE__, __FUNCSIG__); }
}

DimArcAngle::DimArcAngle(bool simply):DimBase(false)
{
	try
	{
		this->DistanceMeasurement(false);
	} 
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0002", __FILE__, __FUNCSIG__); }
}

DimArcAngle::~DimArcAngle()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimArcAngle::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype = RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE)
			dimcnt = arcangno++;
		else if(Mtype = RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH)
			dimcnt = arclenno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimArcAngle::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		Circle* CirShape = (Circle*)ParentShape1;
		double center[3], startpoint[2], endpoint[2];
		CirShape->getCenter()->FillDoublePointer(&center[0], 3);
		CirShape->getendpoint1()->FillDoublePointer(&startpoint[0]); CirShape->getendpoint2()->FillDoublePointer(&endpoint[0]);
		GRAFIX->drawArclength(center[0], center[1], mposition.getX(), mposition.getY() ,&startpoint[0], &endpoint[0], CirShape->Radius(), CirShape->Startangle(), CirShape->Sweepangle(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, center[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0005", __FILE__, __FUNCSIG__); }
}

void DimArcAngle::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		double angle = ((Circle*)ParentShape1)->Sweepangle();
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE)
		{
			this->DistanceMeasurement(false);
			this->setAngularDimension(angle);
		}
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH)
		{
			this->DistanceMeasurement(true);
			this->setDimension(((Circle*)ParentShape1)->Length());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0006", __FILE__, __FUNCSIG__); }
}

void DimArcAngle::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimArcAngle::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimArcAngle::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE)
			myname = _T("ArcAng");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH)
			myname = _T("Arclen");
		DimArcAngle* Cdim = new DimArcAngle((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimArcAngle::CreateDummyCopy()
{
	try
	{
		DimArcAngle* Cdim = new DimArcAngle(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimArcAngle::CopyMeasureParameters(DimBase* Cdim)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0009", __FILE__, __FUNCSIG__); }
}

int DimArcAngle::arcangno = 0;
int DimArcAngle::arclenno = 0;
void DimArcAngle::reset()
{
	arcangno = 0;
	arclenno = 0;
}

wostream& operator<<( wostream& os, DimArcAngle& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimArcAngle"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os<<"EndDimArcAngle"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimArcAngle& x)
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
			if(Tagname==L"DimArcAngle")
			{
			while(Tagname!=L"EndDimArcAngle")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimArcAngle& x)
{
	try
	{
		int n;		
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMARCANG0011", __FILE__, __FUNCSIG__);}
}