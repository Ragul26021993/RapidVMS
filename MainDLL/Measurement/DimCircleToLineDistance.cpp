#include "StdAfx.h"
#include "DimCircleToLineDistance.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimCircleToLineDistance::DimCircleToLineDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTL0001", __FILE__, __FUNCSIG__); }
}

DimCircleToLineDistance::DimCircleToLineDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0002", __FILE__, __FUNCSIG__); }
}

DimCircleToLineDistance::~DimCircleToLineDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0003", __FILE__, __FUNCSIG__); }
}

void DimCircleToLineDistance::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOLINEDISTANCE;
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
		LinearmeasureType(0);
		TotalLinearmeasureType(6);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimCircleToLineDistance::genName(const TCHAR* prefix)
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

void DimCircleToLineDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CIRCLE && ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ARC)
		{
			Shape *temp = ParentShape1;
			ParentShape1 = ParentShape2;
			ParentShape2 = temp;
		}	
		Circle* CircShape = (Circle*)ParentShape1; Line* LinShape = (Line*)ParentShape2;
		double angle = LinShape->Angle(), intercept = LinShape->Intercept(), Dist = 0;
		double cen[2] = {CircShape->getCenter()->getX(), CircShape->getCenter()->getY()};
		this->CLMeasuretype = CIRCLELINEMESURETYPE((int)ParentShape2->ShapeType);
		Dist = RMATH2DOBJECT->Line2Circle_distance_Type(angle, intercept, &cen[0], CircShape->Radius(), NormalmeasureType());
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0006", __FILE__, __FUNCSIG__); }
}

void DimCircleToLineDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		Circle* CircShape = (Circle*)ParentShape1;
		Line* LinShape = (Line*)ParentShape2;
		double center[2] = {CircShape->getCenter()->getX(), CircShape->getCenter()->getY()}, point[4];
		switch(CLMeasuretype)
		{
			case CIRCLELINEMESURETYPE::CIRCLE_FLINE:
				point[0] = LinShape->getPoint1()->getX(); point[1] = LinShape->getPoint1()->getY(); point[2] = LinShape->getPoint2()->getX(); point[3] = LinShape->getPoint2()->getY();
				GRAFIX->drawCircle_FinitelineDistance(LinShape->Angle(), LinShape->Intercept(), &point[0], &point[2], &center[0], CircShape->Radius(), mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), NormalmeasureType(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, LinShape->getPoint2()->getZ());
				break;
			case CIRCLELINEMESURETYPE::CIRCLE_XRAY:
				point[0] = LinShape->getPoint1()->getX(); point[1] = LinShape->getPoint1()->getY();
				GRAFIX->drawCircle_RayDistance(LinShape->Angle(), LinShape->Intercept(), &point[0], &center[0], CircShape->Radius(), mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), NormalmeasureType(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, LinShape->getPoint2()->getZ());
				break;
			case CIRCLELINEMESURETYPE::CIRCLE_XLINE:
				GRAFIX->drawCircle_InfinitelineDistance(LinShape->Angle(), LinShape->Intercept(), &center[0], CircShape->Radius(), mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), NormalmeasureType(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, LinShape->getPoint2()->getZ());
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0007", __FILE__, __FUNCSIG__); }
}

void DimCircleToLineDistance::UpdateForParentChange(BaseItem* sender)
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

void DimCircleToLineDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimCircleToLineDistance::Clone(int n)
{
	try
	{
		DimCircleToLineDistance* Cdim = new DimCircleToLineDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0009", __FILE__, __FUNCSIG__); }
}

DimBase* DimCircleToLineDistance::CreateDummyCopy()
{
	try
	{
		DimCircleToLineDistance* Cdim = new DimCircleToLineDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCircleToLineDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimCircleToLineDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCircleToLineDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimCircleToLineDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCircleToLineDistance& x)
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
			if(Tagname==L"DimCircleToLineDistance")
			{
				while(Tagname!=L"EndDimCircleToLineDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCircleToLineDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCTC0013", __FILE__, __FUNCSIG__);}
}