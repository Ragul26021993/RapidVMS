#include "StdAfx.h"
#include "DimPlaneToCircle3DDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimPlaneToCircle3DDistance::DimPlaneToCircle3DDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0002", __FILE__, __FUNCSIG__); }
}

DimPlaneToCircle3DDistance::~DimPlaneToCircle3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0003", __FILE__, __FUNCSIG__); }
}

void DimPlaneToCircle3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0004", __FILE__, __FUNCSIG__); }
}

void DimPlaneToCircle3DDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Circle3D_PlaneMType)
	{
		case CIRCLE3D_PLANEMEASURETYPE ::ANGLE:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CIRCLE3D_PLANEMEASURETYPE::DISTANCE:
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

void DimPlaneToCircle3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCIRCLE3DDISTANCE;
		this->DistanceMeasurement(true);

		if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}

		((Plane*)ParentShape2)->getParameters(&Plane2Param[0]);
		((Plane*)ParentShape2)->getEndPoints(&Plane2EndPoints[0]);
	    ((Circle*)ParentShape1)->getParameters(&Circle3D[0]);
	    double CirclePln[4] = {Circle3D[3], Circle3D[4], Circle3D[5], (Circle3D[0]*Circle3D[3] + Circle3D[1]*Circle3D[4] + Circle3D[2]*Circle3D[5])};
		
		double pt1[3] = {0}, pt2[3] = {0};
		((Plane*)ParentShape2)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape2)->getPoint3()->FillDoublePointer(&pt2[0], 3);
		double mid2[3] = {(pt1[0] + pt2[0])/2, (pt1[1] + pt2[1])/2, (pt1[2] + pt2[2])/2};
			
		if(RMATH3DOBJECT->Checking_Parallel_Plane_to_Plane(&CirclePln[0], &Plane2Param[0]))
		{
			double Dist = RMATH3DOBJECT->Distance_Plane_Plane(&CirclePln[0], &Plane2Param[0]);
			setDimension(Dist);
			RMATH3DOBJECT->TransformationMatrix_Dist_Pln2Pln(&CirclePln[0], &Plane2Param[0], &Circle3D[0], &mid2[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	        Circle3D_PlaneMType = CIRCLE3D_PLANEMEASURETYPE::DISTANCE;
		}
		else
		{
			this->DistanceMeasurement(false);
			RMATH3DOBJECT->TransformationMatrix_Angle_Pln2Pln(&CirclePln[0], &Plane2Param[0], &Circle3D[0], &TransformationMatrix[0], &MeasurementPlane[0]);
			double Dist = RMATH3DOBJECT->Angle_Plane_Plane(&CirclePln[0], &Plane2Param[0], MeasurementPlane, DimSelectionLine);
			this->setAngularDimension(Dist);
			Circle3D_PlaneMType = CIRCLE3D_PLANEMEASURETYPE::ANGLE;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0006", __FILE__, __FUNCSIG__); }
}

void DimPlaneToCircle3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawCircle3D_PlaneDistOrAng3D(&Circle3D[0], &Plane2Param[0], &Plane2EndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], DistanceMeasurement(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0007", __FILE__, __FUNCSIG__); }
}

void DimPlaneToCircle3DDistance::UpdateForParentChange(BaseItem* sender)
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

void DimPlaneToCircle3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimPlaneToCircle3DDistance::Clone(int n)
{
	try
	{
		DimPlaneToCircle3DDistance* Cdim = new DimPlaneToCircle3DDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPlaneToCircle3DDistance::CreateDummyCopy()
{
	try
	{
		DimPlaneToCircle3DDistance* Cdim = new DimPlaneToCircle3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPlaneToCircle3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPlaneToCircle3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPlaneToCircle3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimPlaneToCircle3DDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPlaneToCircle3DDistance& x)
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
			if(Tagname==L"DimPlaneToCircle3DDistance")
			{
			while(Tagname!=L"EndDimPlaneToCircle3DDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPlaneToCircle3DDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0013", __FILE__, __FUNCSIG__); }
}