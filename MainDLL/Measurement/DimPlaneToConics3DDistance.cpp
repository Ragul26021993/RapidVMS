#include "StdAfx.h"
#include "DimPlaneToConics3DDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Conics3D.h"
#include "..\Engine\RCadApp.h"


DimPlaneToConics3DDistance::DimPlaneToConics3DDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0002", __FILE__, __FUNCSIG__); }
}

DimPlaneToConics3DDistance::~DimPlaneToConics3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0003", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConics3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0004", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConics3DDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(PlaneConicsMtype)
	{
		case PLANECONICSMEASURETYPE ::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case PLANECONICSMEASURETYPE::DISTANCEONLY:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
	}
	 setModifiedName(myname);
}

void DimPlaneToConics3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCONICS3DDISTANCE;
		this->DistanceMeasurement(true);

		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ELLIPSE3D)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}

		((Plane*)ParentShape2)->getParameters(&PlaneParam[0]);
		((Plane*)ParentShape2)->getEndPoints(&PlaneEndPoints[0]);
	    ((Conics3D*)ParentShape1)->getParameters(Conics3DParam);
	  		
		double pt1[3], pt2[3], Max, Min, dir[6], Cen[3];
		((Plane*)ParentShape2)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape2)->getPoint3()->FillDoublePointer(&pt2[0], 3);
		double mid2[3] = {(pt1[0] + pt2[0])/2, (pt1[1] + pt2[1])/2, (pt1[2] + pt2[2])/2};
		Max = (2 * Conics3DParam[13] * Conics3DParam[16])/(1 - Conics3DParam[13] * Conics3DParam[13]);
		Min = sqrt((Max * Max) * (1 - Conics3DParam[13] * Conics3DParam[13]));
		RMATH3DOBJECT->Create_Perpendicular_Line_2_Line(&Conics3DParam[4], &Conics3DParam[7], &dir[0]);
		double focallgt = (Max/2) - (Conics3DParam[13] * Conics3DParam[16])/(1 + Conics3DParam[13]);
		for(int i = 0; i < 3; i++) Cen[i] = Conics3DParam[4 + i] + focallgt * dir[3 + i];

		if(RMATH3DOBJECT->Checking_Parallel_Plane_to_Plane(Conics3DParam, PlaneParam))
		{
			double Dist = RMATH3DOBJECT->Distance_Plane_Plane(Conics3DParam, PlaneParam);
			setDimension(Dist);
			RMATH3DOBJECT->TransformationMatrix_Dist_Pln2Pln(Conics3DParam, PlaneParam, Cen, &mid2[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	    	PlaneConicsMtype = PLANECONICSMEASURETYPE::DISTANCEONLY;
		}
		else
		{
			this->DistanceMeasurement(false);
			RMATH3DOBJECT->TransformationMatrix_Angle_Pln2Pln(Conics3DParam, PlaneParam, Cen, &TransformationMatrix[0], &MeasurementPlane[0]);
			double Dist = RMATH3DOBJECT->Angle_Plane_Plane(Conics3DParam, PlaneParam, MeasurementPlane, DimSelectionLine);
			this->setAngularDimension(Dist);
			PlaneConicsMtype = PLANECONICSMEASURETYPE::ANGLEONLY;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0006", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConics3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawConics3D_PlaneDistOrAng3D(&Conics3DParam[0], &PlaneParam[0], &PlaneEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], DistanceMeasurement(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0007", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConics3DDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0008", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConics3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimPlaneToConics3DDistance::Clone(int n)
{
	try
	{
		DimPlaneToConics3DDistance* Cdim = new DimPlaneToConics3DDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPlaneToConics3DDistance::CreateDummyCopy()
{
	try
	{
		DimPlaneToConics3DDistance* Cdim = new DimPlaneToConics3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPlaneToConics3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPlaneToConics3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPlaneToConics3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimPlaneToConics3DDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPlaneToConics3DDistance& x)
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
			if(Tagname==L"DimPlaneToConics3DDistance")
			{
			while(Tagname!=L"EndDimPlaneToConics3DDistance")
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPlaneToConics3DDistance& x)
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0013", __FILE__, __FUNCSIG__); }
}