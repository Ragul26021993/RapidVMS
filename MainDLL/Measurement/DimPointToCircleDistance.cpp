#include "StdAfx.h"
#include "DimPointToCircleDistance.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

DimPointToCircleDistance::DimPointToCircleDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0001", __FILE__, __FUNCSIG__); }
}

DimPointToCircleDistance::DimPointToCircleDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0002", __FILE__, __FUNCSIG__); }
}

DimPointToCircleDistance::~DimPointToCircleDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0003", __FILE__, __FUNCSIG__); }
}

void DimPointToCircleDistance::init()
{
	try
	{
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
		LinearmeasureType(0);
		TotalLinearmeasureType(6);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToCircleDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToCircleDistance::CalculateMeasurement(Shape *ParentShape1, Vector* pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = ParentShape1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE;
		double cen[2] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY()};
		double pt[2] = {ParentPoint1->getX(), ParentPoint1->getY()}, Dist = 0;
		MeasurementPlane[3] = ((Circle*)ParentShape1)->getCenter()->getZ();
		if(LinearMeasurementMode())
			Dist = RMATH2DOBJECT->LPt2Circle_distance_Type(&pt[0], &cen[0], ((Circle*)ParentShape1)->Radius(), LinearmeasureType());
		else
			Dist = RMATH2DOBJECT->Pt2Circle_distance_Type(&pt[0], &cen[0], ((Circle*)ParentShape1)->Radius(), NormalmeasureType());
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0006", __FILE__, __FUNCSIG__); }
}

void DimPointToCircleDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(LinearMeasurementMode())
			GRAFIX->drawCircle_PointDistance_Linear(((Circle*)ParentShape1)->getCenter()->getX(),((Circle *)ParentShape1)->getCenter()->getY(), ((Circle *)ParentShape1)->Radius(), ParentPoint1->getX(), ParentPoint1->getY(), doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), LinearmeasureType(),true, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, MeasurementPlane[3]);
		else
			GRAFIX->drawCircle_PointDistance(ParentPoint1->getX(), ParentPoint1->getY(),((Circle *)ParentShape1)->Radius(),((Circle *)ParentShape1)->getCenter()->getX(),((Circle *)ParentShape1)->getCenter()->getY(), doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(),NormalmeasureType(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, MeasurementPlane[3]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0007", __FILE__, __FUNCSIG__); }
}

void DimPointToCircleDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			ParentShape1 = (Shape*)sender;
		CalculateMeasurement(ParentShape1, ParentPoint1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0008", __FILE__, __FUNCSIG__); }
}

void DimPointToCircleDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPointToCircleDistance::Clone(int n)
{
	try
	{
		DimPointToCircleDistance* Cdim = new DimPointToCircleDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToCircleDistance::CreateDummyCopy()
{
	try
	{
		DimPointToCircleDistance* Cdim = new DimPointToCircleDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToCircleDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToCircleDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToCircleDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os<<"EndDimPointToCircleDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToCircleDistance& x)
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
			if(Tagname==L"DimPointToCircleDistance")
			{
			while(Tagname!=L"EndDimPointToCircleDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToCircleDistance& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.ParentPoint1 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint1));
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0013", __FILE__, __FUNCSIG__);  }
}