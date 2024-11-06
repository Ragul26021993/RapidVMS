#include "StdAfx.h"
#include "DimPointToCylinderDistance.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"

DimPointToCylinderDistance::DimPointToCylinderDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0001", __FILE__, __FUNCSIG__); }
}

DimPointToCylinderDistance::DimPointToCylinderDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0002", __FILE__, __FUNCSIG__); }
}

DimPointToCylinderDistance::~DimPointToCylinderDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0003", __FILE__, __FUNCSIG__); }
}

void DimPointToCylinderDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(5);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToCylinderDistance::genName(const TCHAR* prefix)
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

void DimPointToCylinderDistance::CalculateMeasurement(Shape *ParentShape1, Vector* pp1)
{
	try
	{
		this->ParentPoint1 = pp1;
		this->ParentShape1 = ParentShape1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE;

		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()},Dist;
		Cylinder* cylinderShp = (Cylinder*)ParentShape1;
		cylinderShp->getParameters(&CylinderParam[0]);
		cylinderShp->getEndPoints(&CylinderEndPoints[0]);
        if(NormalmeasureType() < 3)
		{
		    Dist = RMATH3DOBJECT->Distance_Point_Cylinder(&point[0], &CylinderParam[0], NormalmeasureType());
		    RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Cylndr(&point[0], &CylinderParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
		}
		else
		{
			  double baseCircleCenter[3], baseCircleRadius, baseCirclePlane[4]= {cylinderShp->dir_l(), cylinderShp->dir_m(), cylinderShp->dir_n(), 0};
			  if(NormalmeasureType() == 3)
			  {
	   			baseCircleCenter[0] = cylinderShp->getCenter1()->getX();  baseCircleCenter[1] = cylinderShp->getCenter1()->getY();  baseCircleCenter[2] = cylinderShp->getCenter1()->getZ();
				baseCirclePlane[3] =  baseCircleCenter[0] * baseCirclePlane[0] + baseCircleCenter[1] * baseCirclePlane[1] + baseCircleCenter[2] * baseCirclePlane[2];
				baseCircleRadius = cylinderShp->Radius1();
			  }
			  else
			  {
				baseCircleCenter[0] = cylinderShp->getCenter2()->getX();  baseCircleCenter[1] = cylinderShp->getCenter2()->getY();  baseCircleCenter[2] = cylinderShp->getCenter2()->getZ();
				baseCirclePlane[3] =  baseCircleCenter[0] * baseCirclePlane[0] + baseCircleCenter[1] * baseCirclePlane[1] + baseCircleCenter[2] * baseCirclePlane[2];
				baseCircleRadius = cylinderShp->Radius2();
			  }
			  Dist = RMATH3DOBJECT->Pt2Circle3D_distance_Type(&point[0], &baseCircleCenter[0], &baseCirclePlane[0], baseCircleRadius, 1);
			  RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pln(&point[0], &baseCirclePlane[0], &baseCircleCenter[0], &TransformationMatrix[0], &MeasurementPlane[0]);
		}
		this->setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0006", __FILE__, __FUNCSIG__); }
}

void DimPointToCylinderDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		Cylinder* cylinderShp = (Cylinder*)ParentShape1;
		double point[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		if(NormalmeasureType() < 3)
		{
		  GRAFIX->drawPoint_CylinderDistance3D(&point[0], &CylinderParam[0], &CylinderEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		}
		else
		{
		  double baseCircle[7] = {0, 0, 0, cylinderShp->dir_l(), cylinderShp->dir_m(), cylinderShp->dir_n(), 0};
		  if(NormalmeasureType() == 3)
		  {
	   	    baseCircle[0] = cylinderShp->getCenter1()->getX();  baseCircle[1] = cylinderShp->getCenter1()->getY();  baseCircle[2] = cylinderShp->getCenter1()->getZ();
		    baseCircle[6] = cylinderShp->Radius1();
		  }
		  else
		  {
		    baseCircle[0] = cylinderShp->getCenter2()->getX();  baseCircle[1] = cylinderShp->getCenter2()->getY();  baseCircle[2] = cylinderShp->getCenter2()->getZ();
		   	baseCircle[6] = cylinderShp->Radius2();
		  }
		 GRAFIX->drawPoint_Circle3DDistance3D(&point[0], &baseCircle[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0007", __FILE__, __FUNCSIG__); }
}

void DimPointToCylinderDistance::UpdateForParentChange(BaseItem* sender)
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

void DimPointToCylinderDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);
}

DimBase* DimPointToCylinderDistance::Clone(int n)
{
	try
	{
		DimPointToCylinderDistance* Cdim = new DimPointToCylinderDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToCylinderDistance::CreateDummyCopy()
{
	try
	{
		DimPointToCylinderDistance* Cdim = new DimPointToCylinderDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToCylinderDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToCylinderDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToCylinderDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os<<"EndDimPointToCylinderDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToCylinderDistance& x)
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
			if(Tagname==L"DimPointToCylinderDistance")
			{
			while(Tagname!=L"EndDimPointToCylinderDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToCylinderDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2CY0013", __FILE__, __FUNCSIG__); }
}