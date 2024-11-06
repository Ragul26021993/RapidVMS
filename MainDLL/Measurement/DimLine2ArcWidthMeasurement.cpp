#include "StdAfx.h"
#include "DimLine2ArcWidthMeasurement.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLine2ArcWidthMeasurement::DimLine2ArcWidthMeasurement(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0001", __FILE__, __FUNCSIG__); }
}

DimLine2ArcWidthMeasurement::DimLine2ArcWidthMeasurement(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0002", __FILE__, __FUNCSIG__); }
}

DimLine2ArcWidthMeasurement::~DimLine2ArcWidthMeasurement()
{
}

void DimLine2ArcWidthMeasurement::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINE2ARCWIDTHMEASUREMENT;
		NormalmeasureType(0);
		TotalNormalmeasureType(1);
		LinearmeasureType(0);
		TotalLinearmeasureType(1);
		findPosition = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimLine2ArcWidthMeasurement::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0004", __FILE__, __FUNCSIG__); return _T("NA");}
} 

void DimLine2ArcWidthMeasurement::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ARC)
		{
			Shape *temp = ParentShape1;
			ParentShape1 = ParentShape2;
			ParentShape2 = temp;
		}	
		Circle* CircShape = (Circle*)ParentShape1; Line* LinShape = (Line*)ParentShape2;
		double angle = LinShape->Angle(), interceptLine = LinShape->Intercept(), Dist = 0;
		double cen[3] = {0};
		CircShape->getCenter()->FillDoublePointer(cen);
        double radius1 = CircShape->Radius(), strtAng1 = CircShape->Startangle(), sweepang1 = CircShape->Sweepangle();
		double slope = 0, intercept = 0; 
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle, interceptLine, cen, MeasurePoints);
		Dist = RMATH2DOBJECT->Pt2Pt_distance(cen, MeasurePoints);
		if(Dist != 0)
		{
		   RMATH2DOBJECT->TwoPointLine(MeasurePoints, cen, &slope, &intercept);
		}  
		else
		{
		   RMATH2DOBJECT->Intercept_LinePassing_Point(cen, slope, &intercept);
		}
		double midPnt[2] = {0};
        CircShape->getMidPoint()->FillDoublePointer(midPnt);
		RMATH2DOBJECT->PointOnArcForWidth(MeasurePoints, slope, intercept, cen, radius1, strtAng1, sweepang1, &MeasurePoints[3], &CirclePositon, midPnt, findPosition);
		MeasurePoints[2] = cen[2];		
		Dist = RMATH2DOBJECT->Pt2Pt_distance(MeasurePoints, &MeasurePoints[3]);
        this->setDimension(Dist);
		MeasurementPlane[3] = cen[0] * MeasurementPlane[0] + cen[1] * MeasurementPlane[1] + cen[2] * MeasurementPlane[2];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0004", __FILE__, __FUNCSIG__); }
}

void DimLine2ArcWidthMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawPoint_PointDistance(MeasurePoints[0], MeasurePoints[1], MeasurePoints[3], MeasurePoints[4], mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, MeasurePoints[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0005", __FILE__, __FUNCSIG__); }
}

void DimLine2ArcWidthMeasurement::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0006", __FILE__, __FUNCSIG__); }
}

void DimLine2ArcWidthMeasurement::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLine2ArcWidthMeasurement::Clone(int n)
{
	try
	{
		DimLine2ArcWidthMeasurement* Cdim = new DimLine2ArcWidthMeasurement();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0007", __FILE__, __FUNCSIG__); }
}

DimBase* DimLine2ArcWidthMeasurement::CreateDummyCopy()
{
	try
	{
		DimLine2ArcWidthMeasurement* Cdim = new DimLine2ArcWidthMeasurement(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLine2ArcWidthMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	
}

wostream& operator<<( wostream& os, DimLine2ArcWidthMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimLine2ArcWidthMeasurement"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"MeasurePoints0:"<< x.MeasurePoints[0] << endl;
		os <<"MeasurePoints1:"<< x.MeasurePoints[1] << endl;
		os <<"MeasurePoints3:"<< x.MeasurePoints[3] << endl;
		os <<"MeasurePoints4:"<< x.MeasurePoints[4] << endl;
		os<<"EndDimLine2ArcWidthMeasurement"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0009", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLine2ArcWidthMeasurement& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		x.findPosition = false;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimLine2ArcWidthMeasurement")
		{
			while(Tagname!=L"EndDimLine2ArcWidthMeasurement")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLTARC0010", __FILE__, __FUNCSIG__); return is; }
}
