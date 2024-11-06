#include "StdAfx.h"
#include "DimPointToCircle3DDistance.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimPointToCircle3DDistance::DimPointToCircle3DDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0001", __FILE__, __FUNCSIG__); }
}

DimPointToCircle3DDistance::DimPointToCircle3DDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0002", __FILE__, __FUNCSIG__); }
}

DimPointToCircle3DDistance::~DimPointToCircle3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0003", __FILE__, __FUNCSIG__); }
}

void DimPointToCircle3DDistance::init()
{
	try
	{
		NormalmeasureType(0);
		TotalNormalmeasureType(2);
		LinearmeasureType(0);
		TotalLinearmeasureType(0);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToCircle3DDistance::genName(const TCHAR* prefix)
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

void DimPointToCircle3DDistance::CalculateMeasurement(Shape *ParentShape1, Vector* pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = ParentShape1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE;
		double Circle3D[7] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(),((Circle*)ParentShape1)->getCenter()->getZ(), (((Circle*)ParentShape1)->dir_l()) , (((Circle*)ParentShape1)->dir_m()),(((Circle*)ParentShape1)->dir_n()), (((Circle*)ParentShape1)->Radius())};
		pt[0] = ParentPoint1->getX();
		pt[1] = ParentPoint1->getY();
		pt[2] = ParentPoint1->getZ();
		double Dist = 0;
		double CircleAsPlane[4] = {Circle3D[3], Circle3D[4], Circle3D[5], Circle3D[0] * Circle3D[3] + Circle3D[1] * Circle3D[4] + Circle3D[2] * Circle3D[5] };
		Dist = RMATH3DOBJECT->Pt2Circle3D_distance_Type(&pt[0], &Circle3D[0], &CircleAsPlane[0], Circle3D[6], NormalmeasureType());
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pln(&pt[0], &CircleAsPlane[0], &Circle3D[0], &TransformationMatrix[0], &MeasurementPlane[0]);
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0006", __FILE__, __FUNCSIG__); }
}

void DimPointToCircle3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double Circle3D[7] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(),((Circle*)ParentShape1)->getCenter()->getZ(), (((Circle*)ParentShape1)->dir_l()) , (((Circle*)ParentShape1)->dir_m()),(((Circle*)ParentShape1)->dir_n()), (((Circle*)ParentShape1)->Radius())};
		if (NormalmeasureType() == 0)
			GRAFIX->drawPoint_PointDistance3D(pt[0], pt[1], pt[2], Circle3D[0], Circle3D[1], Circle3D[2], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	    else
			GRAFIX->drawPoint_Circle3DDistance3D(&pt[0], &Circle3D[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0007", __FILE__, __FUNCSIG__); }
}

void DimPointToCircle3DDistance::UpdateForParentChange(BaseItem* sender)
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

void DimPointToCircle3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPointToCircle3DDistance::Clone(int n)
{
	try
	{
		DimPointToCircle3DDistance* Cdim = new DimPointToCircle3DDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToCircle3DDistance::CreateDummyCopy()
{
	try
	{
		DimPointToCircle3DDistance* Cdim = new DimPointToCircle3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToCircle3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToCircle3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToCircle3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os<<"EndDimPointToCircle3DDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToCircle3DDistance& x)
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
			if(Tagname==L"DimPointToCircle3DDistance")
			{
				while(Tagname!=L"EndDimPointToCircle3DDistance")
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2C0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToCircle3DDistance& x)
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