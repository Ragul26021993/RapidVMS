#include "StdAfx.h"
#include "DimPointToLineDistance3D.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

DimPointToLineDistance3D::DimPointToLineDistance3D(TCHAR* myname):DimBase(genName(myname))
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0001", __FILE__, __FUNCSIG__); }
}

DimPointToLineDistance3D::DimPointToLineDistance3D(bool simply):DimBase(false)
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0002", __FILE__, __FUNCSIG__); }
}

DimPointToLineDistance3D::~DimPointToLineDistance3D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToLineDistance3D::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToLineDistance3D::CalculateMeasurement(Shape *s1, Vector *pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = s1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D;
		((Line*)ParentShape1)->getParameters(&LineParam[0]);
		((Line*)ParentShape1)->getEndPoints(&LineEndpoints[0]);
		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()},Dist;
		Dist = RMATH3DOBJECT->Distance_Point_Line(&point[0], &LineParam[0]);
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Ln(&point[0], &LineParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0005", __FILE__, __FUNCSIG__); }
}

void DimPointToLineDistance3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		GRAFIX->drawPoint_FinitelineDistance3D(&point[0], &LineEndpoints[0], &LineParam[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0006", __FILE__, __FUNCSIG__); }
}

void DimPointToLineDistance3D::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			ParentShape1 = (Shape*)sender;
		CalculateMeasurement(ParentShape1, ParentPoint1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0007", __FILE__, __FUNCSIG__); }
}

void DimPointToLineDistance3D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPointToLineDistance3D::Clone(int n)
{
	try
	{
		DimPointToLineDistance3D* Cdim = new DimPointToLineDistance3D(_T("Dist"));
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToLineDistance3D::CreateDummyCopy()
{
	try
	{
		DimPointToLineDistance3D* Cdim = new DimPointToLineDistance3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToLineDistance3D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToLineDistance3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os << "DimPointToLineDistance3D"<< endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os << "ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os << "EndDimPointToLineDistance3D"<< endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToLineDistance3D& x)
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
			if(Tagname==L"DimPointToLineDistance3D")
			{
			while(Tagname!=L"EndDimPointToLineDistance3D")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToLineDistance3D& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE3D0012", __FILE__, __FUNCSIG__); }
}