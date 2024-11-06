#include "StdAfx.h"
#include "DimPointToConeDistance.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"

DimPointToConeDistance::DimPointToConeDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0001", __FILE__, __FUNCSIG__); }
}

DimPointToConeDistance::DimPointToConeDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0002", __FILE__, __FUNCSIG__); }
}

DimPointToConeDistance::~DimPointToConeDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0003", __FILE__, __FUNCSIG__); }
}

void DimPointToConeDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToConeDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToConeDistance::CalculateMeasurement(Shape *ParentShape1, Vector* pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = ParentShape1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE;
		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()}, Dist, dir[4];
		((Cone*)ParentShape1)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape1)->getEndPoints(&ConeEndPoints[0]);
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Ln(&point[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	    RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&ConeParam[3], &MeasurementPlane[0], &dir[0]);
		((Cone*)ParentShape1)->getSurfaceLine(&dir[0], true, &SfLine1[0], &SfEndPnts1[0]);
		((Cone*)ParentShape1)->getSurfaceLine(&dir[0], false, &SfLine2[0], &SfEndPnts2[0]);
		Dist = RMATH3DOBJECT->Distance_Point_Cone(&point[0], &ConeParam[0], &SfLine1[0], &SfLine2[0], NormalmeasureType());
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0006", __FILE__, __FUNCSIG__); }
}

void DimPointToConeDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		if(NormalmeasureType() == 0)
			GRAFIX->drawPoint_FinitelineDistance3D(point, SfEndPnts1, SfLine1, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if (NormalmeasureType() == 1)
			GRAFIX->drawPoint_FinitelineDistance3D(point, ConeEndPoints, ConeParam, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if (NormalmeasureType() == 2)
			GRAFIX->drawPoint_FinitelineDistance3D(point, SfEndPnts2, SfLine2, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0007", __FILE__, __FUNCSIG__); }
}


void DimPointToConeDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			ParentShape1 = (Shape*)sender;
		CalculateMeasurement(ParentShape1, ParentPoint1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0008", __FILE__, __FUNCSIG__); }
}

void DimPointToConeDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPointToConeDistance::Clone(int n)
{
	try
	{
		DimPointToConeDistance* Cdim = new DimPointToConeDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToConeDistance::CreateDummyCopy()
{
	try
	{
		DimPointToConeDistance* Cdim = new DimPointToConeDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToConeDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToConeDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToConeDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os<<"EndDimPointToConeDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToConeDistance& x)
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
			if(Tagname==L"DimPointToConeDistance")
			{
			while(Tagname!=L"EndDimPointToConeDistance")
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToConeDistance& x)
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0013", __FILE__, __FUNCSIG__);  }
}