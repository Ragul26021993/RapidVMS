#include "StdAfx.h"
#include "DimPlaneToSphereDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"

DimPlaneToSphereDistance::DimPlaneToSphereDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0001", __FILE__, __FUNCSIG__); }
}

DimPlaneToSphereDistance::DimPlaneToSphereDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0002", __FILE__, __FUNCSIG__); }
}

DimPlaneToSphereDistance::~DimPlaneToSphereDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0003", __FILE__, __FUNCSIG__); }
}

void DimPlaneToSphereDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPlaneToSphereDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPlaneToSphereDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double Dist;
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANETOSPHEREDISTANCE;
		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::PLANE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		((Plane*)ParentShape1)->getParameters(&PlaneParam[0]);
		((Plane*)ParentShape1)->getEndPoints(&PlaneEndPoints[0]);
		double pt1[3], pt2[3];
		((Plane*)ParentShape1)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape1)->getPoint3()->FillDoublePointer(&pt2[0], 3);
		double mid1[3] = {(pt1[0] + pt2[0])/2, (pt1[1] + pt2[1])/2, (pt1[2] + pt2[2])/2};
		((Sphere*)ParentShape2)->getParameters(&SphereParam[0]);
		Dist = RMATH3DOBJECT->Distance_Plane_Sphere(&PlaneParam[0], &SphereParam[0], NormalmeasureType());
		setDimension(Dist);
		RMATH3DOBJECT->TransformationMatrix_Dist_Pln2Spr(&PlaneParam[0], &SphereParam[0], &mid1[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0006", __FILE__, __FUNCSIG__); }
}

void DimPlaneToSphereDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawPlane_SphereDistance3D(&PlaneParam[0], &PlaneEndPoints[0], &SphereParam[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0007", __FILE__, __FUNCSIG__); }
}

void DimPlaneToSphereDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0008", __FILE__, __FUNCSIG__); }
}

void DimPlaneToSphereDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimPlaneToSphereDistance::Clone(int n)
{
	try
	{
		DimPlaneToSphereDistance* Cdim = new DimPlaneToSphereDistance(_T("Dist"));
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPlaneToSphereDistance::CreateDummyCopy()
{
	try
	{
		DimPlaneToSphereDistance* Cdim = new DimPlaneToSphereDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPlaneToSphereDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPlaneToSphereDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPlaneToSphereDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimPlaneToSphereDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPlaneToSphereDistance& x)
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
			if(Tagname==L"DimPlaneToSphereDistance")
			{
				while(Tagname!=L"EndDimPlaneToSphereDistance")
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
						if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}		
						if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPlaneToSphereDistance& x)
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2SPH0013", __FILE__, __FUNCSIG__);  }
}