#include "StdAfx.h"
#include "DimLine3D.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLine3D::DimLine3D(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0001", __FILE__, __FUNCSIG__); }
}

DimLine3D::DimLine3D(bool simply):DimBase(false)
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0002", __FILE__, __FUNCSIG__); }
}

DimLine3D::~DimLine3D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimLine3D::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		dimcnt = straightnessno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimLine3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		Line* line = (Line*)ParentShape1;
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED:
				{
				GRAFIX->drawDiametere3D(&LineMidPt[0], 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
				}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0005", __FILE__, __FUNCSIG__); }
}

void DimLine3D::CalculateMeasurement(Line* line)
{
	try
	{
		this->ParentShape1 = line;
		line->getMidPoint()->FillDoublePointer(&LineMidPt[0], 3);
		LineMidPt[3] = 0;
		double lineparam[6] = {0};
		line->getParameters(lineparam);
		MeasurementPlane[3] = 0;
		for(int i = 0; i < 3; i++)
		{
			MeasurementPlane[i] = lineparam[3 + i];
			MeasurementPlane[3] += LineMidPt[i] * lineparam[3 + i];
		}
		RMATH3DOBJECT->GetTMatrixForPlane(MeasurementPlane, TransformationMatrix);
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS:
				this->setDimension(line->Straightness());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED:
				this->setDimension(line->FilteredStraightness());
				break;
		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0006", __FILE__, __FUNCSIG__); }
}

void DimLine3D::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		CalculateMeasurement((Line*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0007", __FILE__, __FUNCSIG__); }
}

void DimLine3D::UpdateMeasurement()
{
	CalculateMeasurement((Line*)ParentShape1);
}

DimBase* DimLine3D::Clone(int n)
{
	try
	{
		DimLine3D* Cdim = new DimLine3D(_T("Dist"), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLine3D::CreateDummyCopy()
{
	try
	{
		DimLine3D* Cdim = new DimLine3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLine3D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0010", __FILE__, __FUNCSIG__); }
}

int DimLine3D::straightnessno = 0;
void DimLine3D::reset()
{
	straightnessno = 0;
}

wostream& operator<<( wostream& os, DimLine3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimLine3D"<<endl;		
		os << "ParentShape1Id:"<< x.ParentShape1->getId() << endl;
		os<<"EndDimLine3D"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLine3D& x)
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
			if(Tagname==L"DimLine3D")
			{
			while(Tagname!=L"EndDimLine3D")
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
						 if(Tagname==L"ParentShape1Id")
						 {
							x.ParentShape1 =(Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0012", __FILE__, __FUNCSIG__); return is;}
}
void ReadOldPP(wistream& is, DimLine3D& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3D0012", __FILE__, __FUNCSIG__); }
}