#include "StdAfx.h"
#include "DimCircleToCircleDistance.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

DimCircleToCircleDistance::DimCircleToCircleDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0001", __FILE__, __FUNCSIG__); }
}

DimCircleToCircleDistance::DimCircleToCircleDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0002", __FILE__, __FUNCSIG__); }
}

DimCircleToCircleDistance::~DimCircleToCircleDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0003", __FILE__, __FUNCSIG__); }
}

void DimCircleToCircleDistance::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOCIRCLEDISTANCE;
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
		LinearmeasureType(0);
		TotalLinearmeasureType(6);
		CirclePositon[0] = 0;
		CirclePositon[1] = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimCircleToCircleDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0005", __FILE__, __FUNCSIG__); return _T("NA");}
} 

void DimCircleToCircleDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
    	this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		Circle* CircShape1 = (Circle*)ParentShape1; Circle* CircShape2 = (Circle*)ParentShape2;
		double center1[2], center2[2], radius1 = CircShape1->Radius(), radius2 = CircShape2->Radius(), Dist = 0;
		CircShape1->getCenter()->FillDoublePointer(&center1[0]); CircShape2->getCenter()->FillDoublePointer(&center2[0]);
		if(LinearMeasurementMode())
			Dist = RMATH2DOBJECT->LCircle2Circle_distance_Type(&center1[0], &center2[0], radius1, radius2, LinearmeasureType());
		else
			Dist = RMATH2DOBJECT->Circle2Circle_distance_Type(&center1[0], &center2[0], radius1, radius2, NormalmeasureType());
        this->setDimension(Dist);
		MeasurementPlane[3] = center1[0] * MeasurementPlane[0] + center1[1] * MeasurementPlane[1] + center1[2] * MeasurementPlane[2];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0006", __FILE__, __FUNCSIG__); }
}

void DimCircleToCircleDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		Circle* CircShape1 = (Circle*)ParentShape1; Circle* CircShape2 = (Circle*)ParentShape2;
		double center1[3], center2[3], radius1 = CircShape1->Radius(), radius2 = CircShape2->Radius(), Dist = 0;
		CircShape1->getCenter()->FillDoublePointer(&center1[0], 3); CircShape2->getCenter()->FillDoublePointer(&center2[0], 3);
		if(LinearMeasurementMode())
			GRAFIX->drawCircle_CircleDistance_Linear(center1[0], center1[1], radius1, center2[0], center2[1], radius2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), LinearmeasureType(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, center1[2]);
		else
			GRAFIX->drawCircle_CircleDistance(center1[0], center1[1], radius1, center2[0], center2[1], radius2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), NormalmeasureType(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, center1[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0007", __FILE__, __FUNCSIG__); }
}

void DimCircleToCircleDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0008", __FILE__, __FUNCSIG__); }
}

void DimCircleToCircleDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimCircleToCircleDistance::Clone(int n)
{
	try
	{
		DimCircleToCircleDistance* Cdim = new DimCircleToCircleDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCircleToCircleDistance::CreateDummyCopy()
{
	try
	{
		DimCircleToCircleDistance* Cdim = new DimCircleToCircleDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCircleToCircleDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, DimCircleToCircleDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCircleToCircleDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"CirclePosition0:"<< x.CirclePositon[0] << endl;
		os <<"CirclePosition1:"<< x.CirclePositon[1] << endl;
		os<<"EndDimCircleToCircleDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DimCircleToCircleDistance& x)
{
	try
	{
		is >> (*(DimBase*)&x);	
		x.CirclePositon[0] = -1;
		x.CirclePositon[1] = -1;
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"DimCircleToCircleDistance")
			{
				while(Tagname!=L"EndDimCircleToCircleDistance")
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
						else if(Tagname==L"CirclePosition0")
						{
							x.CirclePositon[0] = atoi((const char*)(Val).c_str());						
						}
						else if(Tagname==L"CirclePosition1")
						{
							x.CirclePositon[1] = atoi((const char*)(Val).c_str());						
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCircleToCircleDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0013", __FILE__, __FUNCSIG__); }
}