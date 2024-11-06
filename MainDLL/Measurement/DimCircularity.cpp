#include "StdAfx.h"
#include "DimCircularity.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimCircularity::DimCircularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0001", __FILE__, __FUNCSIG__); }
}

DimCircularity::DimCircularity(bool simply):DimBase(false)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0002", __FILE__, __FUNCSIG__); }
}

DimCircularity::~DimCircularity()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimCircularity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype = RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY)
			dimcnt = circularityno++;
		else if(Mtype = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC)
			dimcnt = micno++;
		else if(Mtype = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC)
			dimcnt = mccno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimCircularity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double center[3] = {0};
		((Circle*)ParentShape1)->getCenter()->FillDoublePointer(center, 3);
		switch(ParentShape1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
			case RapidEnums::SHAPETYPE::ARC:
				GRAFIX->drawDiametere3D(center, ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0005", __FILE__, __FUNCSIG__); }
}


void DimCircularity::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		double cirParam[7] = {0};
		((Circle*)ParentShape1)->getParameters(cirParam);
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY:
				setDimension(((Circle*)s1)->Circularity());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC:
				setDimension(((Circle*)s1)->MinInscribedia());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC:
				setDimension(((Circle*)s1)->MaxCircumdia());
				break;
		}
		RMATH3DOBJECT->TransformationMatrix_Dia_Circle(cirParam, &TransformationMatrix[0], &MeasurementPlane[0]);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0006", __FILE__, __FUNCSIG__); }
}

void DimCircularity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimCircularity::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimCircularity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY)
			myname = _T("Circ");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC)
			myname = _T("Mic");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC)
			myname = _T("Mcc");
		DimCircularity* Cdim = new DimCircularity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCircularity::CreateDummyCopy()
{
	try
	{
		DimCircularity* Cdim = new DimCircularity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCircularity::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0009", __FILE__, __FUNCSIG__); }
}

int DimCircularity::circularityno = 0;
int DimCircularity::micno = 0;
int DimCircularity::mccno = 0;
void DimCircularity::reset()
{
	circularityno = 0;
	micno = 0;
	mccno = 0;
}

wostream& operator<<( wostream& os, DimCircularity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCircularity"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os<<"EndDimCircularity"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0010", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimCircularity& x)
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
			if(Tagname==L"DimCircularity")
			{
				while(Tagname!=L"EndDimCircularity")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCircularity& x)
{
	try
	{
		int n;		
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCIRC0011", __FILE__, __FUNCSIG__); }
}