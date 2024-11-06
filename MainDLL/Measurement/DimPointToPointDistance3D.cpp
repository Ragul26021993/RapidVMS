#include "StdAfx.h"
#include "DimPointToPointDistance3D.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

DimPointToPointDistance3D::DimPointToPointDistance3D(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		LinearmeasureType(0);
		TotalLinearmeasureType(3);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0001", __FILE__, __FUNCSIG__); }
}

DimPointToPointDistance3D::DimPointToPointDistance3D(bool simply):DimBase(false)
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0002", __FILE__, __FUNCSIG__); }
}

DimPointToPointDistance3D::~DimPointToPointDistance3D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToPointDistance3D::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToPointDistance3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(LinearMeasurementMode())
			GRAFIX->drawPoint_PointDistance3DLinear(ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ(), ParentPoint2->getX(), ParentPoint2->getY(),ParentPoint2->getZ(), doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, this->LinearmeasureType());
		else
    		GRAFIX->drawPoint_PointDistance3D(ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ(), ParentPoint2->getX(), ParentPoint2->getY(), ParentPoint2->getZ(), doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0005", __FILE__, __FUNCSIG__); }
}

void DimPointToPointDistance3D::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	UpdateMeasurement();
}

void DimPointToPointDistance3D::CalculateMeasurement(Vector *vv1, Vector *vv2)
{
	try
	{
		double Dist;
		this->ParentPoint1 = vv1;
		this->ParentPoint2 = vv2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D;
		double point1[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		double point2[3] = {ParentPoint2->getX(), ParentPoint2->getY(), ParentPoint2->getZ()};
		if(LinearMeasurementMode())
		{
			Dist = abs(point1[LinearmeasureType()] - point2[LinearmeasureType()]);
		}
		else
		{
			RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(&point1[0], &point2[0], &TransformationMatrix[0], &MeasurementPlane[0]);
			Dist = RMATH3DOBJECT->Distance_Point_Point(&point1[0], &point2[0]);
		}
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0006", __FILE__, __FUNCSIG__); }
}

void DimPointToPointDistance3D::UpdateMeasurement()
{
	try
	{
		if(!IsValid()) return;
		CalculateMeasurement(ParentPoint1, ParentPoint2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0007", __FILE__, __FUNCSIG__); }
}

DimBase* DimPointToPointDistance3D::Clone(int n)
{
	try
	{
		DimPointToPointDistance3D* Cdim = new DimPointToPointDistance3D();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToPointDistance3D::CreateDummyCopy()
{
	try
	{
		DimPointToPointDistance3D* Cdim = new DimPointToPointDistance3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToPointDistance3D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToPointDistance3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToPointDistance3D"<<endl;
		os <<"ParentPoint1:value"<<endl<< (*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os <<"ParentPoint2:value"<<endl<< (*static_cast<Vector*>(x.ParentPoint2)) << endl;
		os<<"EndDimPointToPointDistance3D"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToPointDistance3D& x)
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
			if(Tagname==L"DimPointToPointDistance3D")
			{
			while(Tagname!=L"EndDimPointToPointDistance3D")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0012", __FILE__, __FUNCSIG__); return is; }
}

void ReadOldPP(wistream& is, DimPointToPointDistance3D& x)
{
	try
	{
		int n; bool flag;
		x.ParentPoint1 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint1));
		x.ParentPoint2 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint2));
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT3D0012", __FILE__, __FUNCSIG__); }
}