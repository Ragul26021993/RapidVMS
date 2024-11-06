#include "StdAfx.h"
#include "DimPointToPlaneDistance.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Plane.h"
#include "..\Engine\RCadApp.h"

DimPointToPlaneDistance::DimPointToPlaneDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0001", __FILE__, __FUNCSIG__); }
}

DimPointToPlaneDistance::DimPointToPlaneDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0002", __FILE__, __FUNCSIG__); }
}

DimPointToPlaneDistance::~DimPointToPlaneDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0003", __FILE__, __FUNCSIG__); }
}

void DimPointToPlaneDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToPlaneDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToPlaneDistance::CalculateMeasurement(Shape *ParentShape1, Vector* pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = ParentShape1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE;

		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()},Dist;
		((Plane*)ParentShape1)->getParameters(&PlaneParam[0]);
		((Plane*)ParentShape1)->getEndPoints(&PlaneEndPoints[0]);
		double pt1[3], pt2[3];
		((Plane*)ParentShape1)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape1)->getPoint3()->FillDoublePointer(&pt2[0], 3);
		double mid1[3] = {(pt1[0] + pt2[0])/2, (pt1[1] + pt2[1])/2, (pt1[2] + pt2[2])/2};
		Dist = RMATH3DOBJECT->Distance_Point_Plane(&point[0], &PlaneParam[0]);
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pln(&point[0], &PlaneParam[0], &mid1[0], &TransformationMatrix[0], &MeasurementPlane[0]);
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0006", __FILE__, __FUNCSIG__); }
}

void DimPointToPlaneDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		GRAFIX->drawPoint_PlaneDistance3D(&point[0], &PlaneParam[0], &PlaneEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0007", __FILE__, __FUNCSIG__); }
}

void DimPointToPlaneDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			ParentShape1 = (Shape*)sender;
		CalculateMeasurement(ParentShape1, ParentPoint1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0008", __FILE__, __FUNCSIG__); }
}

void DimPointToPlaneDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPointToPlaneDistance::Clone(int n)
{
	try
	{
		DimPointToPlaneDistance* Cdim = new DimPointToPlaneDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToPlaneDistance::CreateDummyCopy()
{
	try
	{
		DimPointToPlaneDistance* Cdim = new DimPointToPlaneDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToPlaneDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToPlaneDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os << "DimPointToPlaneDistance"<< endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os << "ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os << "EndDimPointToPlaneDistance"<< endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToPlaneDistance& x)
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
			if(Tagname==L"DimPointToPlaneDistance")
			{
			while(Tagname!=L"EndDimPointToPlaneDistance")
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToPlaneDistance& x)
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PLANE0013", __FILE__, __FUNCSIG__);  }
}