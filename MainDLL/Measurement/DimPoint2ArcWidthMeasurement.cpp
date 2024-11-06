#include "StdAfx.h"
#include "DimPoint2ArcWidthMeasurement.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

DimPoint2ArcWidthMeasurement::DimPoint2ArcWidthMeasurement(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0001", __FILE__, __FUNCSIG__); }
}

DimPoint2ArcWidthMeasurement::DimPoint2ArcWidthMeasurement(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0002", __FILE__, __FUNCSIG__); }
}

DimPoint2ArcWidthMeasurement::~DimPoint2ArcWidthMeasurement()
{
}

void DimPoint2ArcWidthMeasurement::init()
{
	try
	{
		NormalmeasureType(0);
		TotalNormalmeasureType(1);
		LinearmeasureType(0);
		TotalLinearmeasureType(1);
		findPosition = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPoint2ArcWidthMeasurement::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPoint2ArcWidthMeasurement::CalculateMeasurement(Shape *ParentShape1, Vector* pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = ParentShape1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINT2ARCWIDTHMEASUREMENT;
		double cen[3] = {0};
		((Circle*)ParentShape1)->getCenter()->FillDoublePointer(cen, 3);
		double pt[2] = {ParentPoint1->getX(), ParentPoint1->getY()}, Dist = 0;
        double radius1 = ((Circle*)ParentShape1)->Radius(), strtAng1 = ((Circle*)ParentShape1)->Startangle(), sweepang1 = ((Circle*)ParentShape1)->Sweepangle();
		double slope = 0, intercept = 0; 
		Dist = RMATH2DOBJECT->Pt2Pt_distance(cen, pt);
		if(Dist != 0)
		{
		   RMATH2DOBJECT->TwoPointLine(pt, cen, &slope, &intercept);
		}  
		else
		{
		   RMATH2DOBJECT->Intercept_LinePassing_Point(cen, slope, &intercept);
		}
		double midPnt[2] = {0};
        ((Circle*)ParentShape1)->getMidPoint()->FillDoublePointer(midPnt);
		RMATH2DOBJECT->PointOnArcForWidth(pt, slope, intercept, cen, radius1, strtAng1, sweepang1, MeasurePoints, &CirclePositon, midPnt, findPosition);
		MeasurePoints[2] = cen[2];
		Dist = RMATH2DOBJECT->Pt2Pt_distance(pt, MeasurePoints);
        this->setDimension(Dist);
		MeasurementPlane[3] = cen[0] * MeasurementPlane[0] + cen[1] * MeasurementPlane[1] + cen[2] * MeasurementPlane[2];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0005", __FILE__, __FUNCSIG__); }
}

void DimPoint2ArcWidthMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawPoint_PointDistance(ParentPoint1->getX(), ParentPoint1->getY(), MeasurePoints[0], MeasurePoints[1], mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, MeasurePoints[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0006", __FILE__, __FUNCSIG__); }
}

void DimPoint2ArcWidthMeasurement::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			ParentShape1 = (Shape*)sender;
		CalculateMeasurement(ParentShape1, ParentPoint1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0007", __FILE__, __FUNCSIG__); }
}

void DimPoint2ArcWidthMeasurement::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPoint2ArcWidthMeasurement::Clone(int n)
{
	try
	{
		DimPoint2ArcWidthMeasurement* Cdim = new DimPoint2ArcWidthMeasurement();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPoint2ArcWidthMeasurement::CreateDummyCopy()
{
	try
	{
		DimPoint2ArcWidthMeasurement* Cdim = new DimPoint2ArcWidthMeasurement(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPoint2ArcWidthMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPoint2ArcWidthMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPoint2ArcWidthMeasurement"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os <<"MeasurePoints0:"<< x.MeasurePoints[0] << endl;
		os <<"MeasurePoints1:"<< x.MeasurePoints[1] << endl;
		os <<"MeasurePoints2:"<< x.MeasurePoints[2] << endl;
		os<<"EndDimPoint2ArcWidthMeasurement"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPoint2ArcWidthMeasurement& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		x.findPosition = false;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimPoint2ArcWidthMeasurement")
		{
	    	while(Tagname!=L"EndDimPoint2ArcWidthMeasurement")
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
					else if(Tagname==L"MeasurePoints0")
					{
						x.MeasurePoints[0] = atof((const char*)(Val).c_str());						
					}
					else if(Tagname==L"MeasurePoints1")
					{
						x.MeasurePoints[1] = atof((const char*)(Val).c_str());						
					}
					else if(Tagname==L"MeasurePoints2")
					{
						x.MeasurePoints[2] = atof((const char*)(Val).c_str());						
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2ARC0012", __FILE__, __FUNCSIG__); return is; }
}
