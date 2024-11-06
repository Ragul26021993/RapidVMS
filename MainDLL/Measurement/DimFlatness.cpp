#include "StdAfx.h"
#include "DimFlatness.h"
#include "..\Shapes\Plane.h"
#include "..\Engine\RCadApp.h"

DimFlatness::DimFlatness(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		IsValid(false);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0001", __FILE__, __FUNCSIG__); }
}

DimFlatness::DimFlatness(bool simply):DimBase(false)
{
	try
	{
		IsValid(false);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0002", __FILE__, __FUNCSIG__); }
}

DimFlatness::~DimFlatness()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimFlatness::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		flatnesscnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(flatnesscnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimFlatness::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(&PlaneMidPt[0], 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0005", __FILE__, __FUNCSIG__); }
}

void DimFlatness::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;		
		double pt1[3], pt2[3];
		((Plane*)ParentShape1)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape1)->getPoint3()->FillDoublePointer(&pt2[0], 3);
		PlaneMidPt[0] = (pt1[0] + pt2[0])/2;
		PlaneMidPt[1] = (pt1[1] + pt2[1])/2;
		PlaneMidPt[2] = (pt1[2] + pt2[2])/2;
		PlaneMidPt[3] = 0;
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS:
				this->setDimension(((Plane*)s1)->Flatness());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED:
				this->setDimension(((Plane*)s1)->FilteredFlatness());
				break;
		}
		
		((Plane*)ParentShape1)->getParameters(MeasurementPlane);
		RMATH3DOBJECT->GetTMatrixForPlane(MeasurementPlane, TransformationMatrix);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0006", __FILE__, __FUNCSIG__); }
}

void DimFlatness::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimFlatness::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimFlatness::Clone(int n)
{
	try
	{
		DimFlatness* Cdim = new DimFlatness();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimFlatness::CreateDummyCopy()
{
	try
	{
		DimFlatness* Cdim = new DimFlatness(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimFlatness::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0009", __FILE__, __FUNCSIG__); }
}

int DimFlatness::flatnesscnt = 0;
void DimFlatness::reset()
{
	flatnesscnt = 0;
}

wostream& operator<<( wostream& os, DimFlatness& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimFlatness"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;		
		os<<"EndDimFlatness"<<endl;	
		return os;		

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DimFlatness& x)
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
			if(Tagname==L"DimFlatness")
			{
				while(Tagname!=L"EndDimFlatness")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimFlatness& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMFLT0011", __FILE__, __FUNCSIG__);}
}