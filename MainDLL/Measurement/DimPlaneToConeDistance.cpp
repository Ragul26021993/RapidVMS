#include "StdAfx.h"
#include "DimPlaneToConeDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"

DimPlaneToConeDistance::DimPlaneToConeDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0002", __FILE__, __FUNCSIG__); }
}

DimPlaneToConeDistance::~DimPlaneToConeDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0003", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConeDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0004", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConeDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Cone_PlaneMType)
	{
		case CONE_PLANEMEASURETYPE ::ANGLE:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CONE_PLANEMEASURETYPE::DISTANCE:
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

void DimPlaneToConeDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(Cone_PlaneMType)
		{
			case CONE_PLANEMEASURETYPE::ANGLE:
				GRAFIX->drawPlane_CylinderDistOrAng3D(&PlaneParam[0], &PlaneEndPts[0], &ConeParam[0], &ConeEndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], false, NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case CONE_PLANEMEASURETYPE::DISTANCE:
				GRAFIX->drawPlane_CylinderDistOrAng3D(&PlaneParam[0], &PlaneEndPts[0], &ConeParam[0], &ConeEndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], true, NormalmeasureType(), &DimSelectionLine[0],  windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0006", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConeDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		bool ProjectLine = false;
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCONEDISTANCE;
		this->DistanceMeasurement(true);

		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CONE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
	    ((Plane*)ParentShape2)->getParameters(&PlaneParam[0]);
		((Plane*)ParentShape2)->getEndPoints(&PlaneEndPts[0]);
		((Cone*)ParentShape1)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape1)->getEndPoints(&ConeEndPts[0]);

		double PlnPts[12] = {0};
		((Plane*)ParentShape2)->getPoint1()->FillDoublePointer(&PlnPts[0], 3);
		((Plane*)ParentShape2)->getPoint2()->FillDoublePointer(&PlnPts[3], 3);
		((Plane*)ParentShape2)->getPoint3()->FillDoublePointer(&PlnPts[6], 3);
		((Plane*)ParentShape2)->getPoint4()->FillDoublePointer(&PlnPts[9], 3);
		double mid[3] = {(PlnPts[0] + PlnPts[6])/2, (PlnPts[1] + PlnPts[7])/2, (PlnPts[2] + PlnPts[8])/2};

		if(RMATH3DOBJECT->Checking_Parallel_Line_to_Plane(&ConeParam[0], &PlaneParam[0]))
		{
			Cone_PlaneMType = CONE_PLANEMEASURETYPE::DISTANCE;
			TotalNormalmeasureType(3);
			double Dist = RMATH3DOBJECT->Distance_Plane_Cylinder(&PlaneParam[0], &ConeParam[0], NormalmeasureType());
			setDimension(Dist);
			RMATH3DOBJECT->TransformationMatrix_Dist_Pln2Cylndr(&PlaneParam[0], &PlnPts[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
		}
		else
		{
			Cone_PlaneMType = CONE_PLANEMEASURETYPE::ANGLE;
			TotalNormalmeasureType(1);
			RMATH3DOBJECT->TransformationMatrix_Angle_Pln2Cylndr(&PlaneParam[0], &PlnPts[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Angle_Line_Plane(&ConeParam[0], &ConeEndPts[0], &PlaneParam[0], &PlnPts[0], MeasurementPlane, DimSelectionLine, ProjectLine);
			this->DistanceMeasurement(false);
			this->setAngularDimension(Dist);
		}

		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0007", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConeDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0008", __FILE__, __FUNCSIG__); }
}

void DimPlaneToConeDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimPlaneToConeDistance::Clone(int n)
{
	try
	{
		DimPlaneToConeDistance* Cdim = new DimPlaneToConeDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPlaneToConeDistance::CreateDummyCopy()
{
	try
	{
		DimPlaneToConeDistance* Cdim = new DimPlaneToConeDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPlaneToConeDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPlaneToConeDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPlaneToConeDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimPlaneToConeDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPlaneToConeDistance& x)
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
			if(Tagname==L"DimPlaneToConeDistance")
			{
			while(Tagname!=L"EndDimPlaneToConeDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__); return is;}
}
void ReadOldPP(wistream& is, DimPlaneToConeDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__);}
}