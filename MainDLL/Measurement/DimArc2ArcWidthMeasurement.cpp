#include "StdAfx.h"
#include "DimArc2ArcWidthMeasurement.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

DimArc2ArcWidthMeasurement::DimArc2ArcWidthMeasurement(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0001", __FILE__, __FUNCSIG__); }
}

DimArc2ArcWidthMeasurement::DimArc2ArcWidthMeasurement(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0002", __FILE__, __FUNCSIG__); }
}

DimArc2ArcWidthMeasurement::~DimArc2ArcWidthMeasurement()
{
}

void DimArc2ArcWidthMeasurement::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_ARC2ARCWIDTHMEASUREMENT;
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
		LinearmeasureType(0);
		TotalLinearmeasureType(6);
		CirclePositon[0] = 0;
		CirclePositon[1] = 0;
		findPosition = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimArc2ArcWidthMeasurement::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0004", __FILE__, __FUNCSIG__); return _T("NA");}
} 

void DimArc2ArcWidthMeasurement::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
    	this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		Circle* CircShape1 = (Circle*)ParentShape1; Circle* CircShape2 = (Circle*)ParentShape2;
		double center1[3] =  {0}, center2[3] = {0}, radius1 = CircShape1->Radius(), strtAng1 = CircShape1->Startangle(), sweepang1 = CircShape1->Sweepangle(), radius2 = CircShape2->Radius(), strtAng2 = CircShape2->Startangle(), sweepang2 = CircShape2->Sweepangle(), Dist = 0;
		CircShape1->getCenter()->FillDoublePointer(&center1[0], 3); CircShape2->getCenter()->FillDoublePointer(&center2[0], 3);
		double slope = 0, intercept = 0;
		Dist = RMATH2DOBJECT->Pt2Pt_distance(center1, center2);
		if(Dist != 0)
		{
		   RMATH2DOBJECT->TwoPointLine(center1, center2, &slope, &intercept);
		}  
		else
		{
		   RMATH2DOBJECT->Intercept_LinePassing_Point(center1, slope, &intercept);
		}
		double midPnt[2] = {0};
        CircShape1->getMidPoint()->FillDoublePointer(midPnt);
		RMATH2DOBJECT->PointOnArcForWidth(center2, slope, intercept, center1, radius1, strtAng1, sweepang1, MeasurePoints, CirclePositon, midPnt, findPosition);
        CircShape2->getMidPoint()->FillDoublePointer(midPnt);
		RMATH2DOBJECT->PointOnArcForWidth(center1, slope, intercept, center2, radius2, strtAng2, sweepang2, &MeasurePoints[3], &CirclePositon[1], midPnt, findPosition);
		MeasurePoints[2] = center1[2];
		Dist  = RMATH2DOBJECT->Pt2Pt_distance(MeasurePoints, &MeasurePoints[3]);
        this->setDimension(Dist);
		MeasurementPlane[3] = center1[0] * MeasurementPlane[0] + center1[1] * MeasurementPlane[1] + center1[2] * MeasurementPlane[2];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0005", __FILE__, __FUNCSIG__); }
}

void DimArc2ArcWidthMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawPoint_PointDistance(MeasurePoints[0], MeasurePoints[1], MeasurePoints[3], MeasurePoints[4], mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, MeasurePoints[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0006", __FILE__, __FUNCSIG__); }
}

void DimArc2ArcWidthMeasurement::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0007", __FILE__, __FUNCSIG__); }
}

void DimArc2ArcWidthMeasurement::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimArc2ArcWidthMeasurement::Clone(int n)
{
	try
	{
		DimArc2ArcWidthMeasurement* Cdim = new DimArc2ArcWidthMeasurement();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimArc2ArcWidthMeasurement::CreateDummyCopy()
{
	try
	{
		DimArc2ArcWidthMeasurement* Cdim = new DimArc2ArcWidthMeasurement(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimArc2ArcWidthMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, DimArc2ArcWidthMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimArc2ArcWidthMeasurement"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"CirclePosition0:"<< x.CirclePositon[0] << endl;
		os <<"CirclePosition1:"<< x.CirclePositon[1] << endl;
        os <<"MeasurePoints0:"<< x.MeasurePoints[0] << endl;
		os <<"MeasurePoints1:"<< x.MeasurePoints[1] << endl;
		os <<"MeasurePoints3:"<< x.MeasurePoints[3] << endl;
		os <<"MeasurePoints4:"<< x.MeasurePoints[4] << endl;
		os<<"EndDimArc2ArcWidthMeasurement"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DimArc2ArcWidthMeasurement& x)
{
	try
	{
		is >> (*(DimBase*)&x);	
		x.CirclePositon[0] = -1;
		x.CirclePositon[1] = -1;
		x.findPosition = false;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimArc2ArcWidthMeasurement")
		{
			while(Tagname!=L"EndDimArc2ArcWidthMeasurement")
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
					else if(Tagname==L"MeasurePoints0")
					{
						x.MeasurePoints[0] = atof((const char*)(Val).c_str());						
					}
					else if(Tagname==L"MeasurePoints1")
					{
						x.MeasurePoints[1] = atof((const char*)(Val).c_str());						
					}
					else if(Tagname==L"MeasurePoints3")
					{
						x.MeasurePoints[3] = atof((const char*)(Val).c_str());						
					}
					else if(Tagname==L"MeasurePoints4")
					{
						x.MeasurePoints[4] = atof((const char*)(Val).c_str());						
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMATAW0012", __FILE__, __FUNCSIG__); return is; }
}