#include "StdAfx.h"
#include "DimPointToPointDistLAngle.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\RPoint.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimPointToPointDistLAngle::DimPointToPointDistLAngle(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0001", __FILE__, __FUNCSIG__); }
}

DimPointToPointDistLAngle::DimPointToPointDistLAngle(bool simply):DimBase(false)
{
	try
	{
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0002", __FILE__, __FUNCSIG__); }
}

DimPointToPointDistLAngle::~DimPointToPointDistLAngle()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToPointDistLAngle::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToPointDistLAngle::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawPoint_PointDistance(this->ParentPoint1->getX(), this->ParentPoint1->getY(), this->ParentPoint2->getX(), this->ParentPoint2->getY(), this->mposition.getX(), this->mposition.getY(), this->doubledimesion(), this->getCDimension(), this->getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, this->ParentPoint1->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0005", __FILE__, __FUNCSIG__); }
}

void DimPointToPointDistLAngle::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	UpdateMeasurement();
}

void DimPointToPointDistLAngle::CalculateMeasurement(Vector *vv1, Vector *vv2, Shape* Cshape)
{
	try
	{
		this->ParentPoint1 = vv1;
		this->ParentPoint2 = vv2;
		this->ParentShape1 = Cshape;
		double angle = ((Line*)ParentShape1)->Angle();
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE_LANG;
		double pp1[2] = {ParentPoint1->getX(), ParentPoint1->getY()};
		double pp2[2] = {ParentPoint2->getX(), ParentPoint2->getY()};
		double mp[2] = {mposition.getX(), mposition.getY()};
		double dis = RMATH2DOBJECT->LXPt2Pt_distance_angle(&pp1[0], &pp2[0], angle);
		this->setDimension(dis);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0006", __FILE__, __FUNCSIG__); }
}

void DimPointToPointDistLAngle::UpdateMeasurement()
{
	CalculateMeasurement(this->ParentPoint1, this->ParentPoint2, this->ParentShape1);
}

DimBase* DimPointToPointDistLAngle::Clone(int n)
{
	try
	{
		DimPointToPointDistLAngle* Cdim = new DimPointToPointDistLAngle();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToPointDistLAngle::CreateDummyCopy()
{
	try
	{
		DimPointToPointDistLAngle* Cdim = new DimPointToPointDistLAngle(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToPointDistLAngle::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToPointDistLAngle& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToPointDistLAngle"<<endl;
		os <<"ParentPoint1:value"<<endl<< (*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os <<"ParentPoint2:value"<<endl<< (*static_cast<Vector*>(x.ParentPoint2)) << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os<<"EndDimPointToPointDistLAngle"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToPointDistLAngle& x)
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
			if(Tagname==L"DimPointToPointDistLAngle")
			{
			while(Tagname!=L"EndDimPointToPointDistLAngle")
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
						else if(Tagname==L"ParentPoint2")
						{
							x.ParentPoint2 = new Vector(0,0,0);
							is >> (*static_cast<Vector*>(x.ParentPoint2));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToPointDistLAngle& x)
{
	try
	{
		int n; bool flag;
		x.ParentPoint1 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint1));
		x.ParentPoint2 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint2));
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PTLA0011", __FILE__, __FUNCSIG__); }
}