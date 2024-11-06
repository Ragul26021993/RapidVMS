#include "StdAfx.h"
#include "DimPlaneToPlaneDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimPlaneToPlaneDistance::DimPlaneToPlaneDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0002", __FILE__, __FUNCSIG__); }
}

DimPlaneToPlaneDistance::~DimPlaneToPlaneDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0003", __FILE__, __FUNCSIG__); }
}

void DimPlaneToPlaneDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0004", __FILE__, __FUNCSIG__); }
}

void DimPlaneToPlaneDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(PlaneMtype)
	{
		case PLANEMEASURETYPE ::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case PLANEMEASURETYPE::DISTANCEONLY:
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

void DimPlaneToPlaneDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANETOPLANEDISTANCE;
		this->DistanceMeasurement(true);
		((Plane*)ParentShape1)->getParameters(&Plane1Param[0]);
		((Plane*)ParentShape2)->getParameters(&Plane2Param[0]);
		((Plane*)ParentShape1)->getEndPoints(&Plane1EndPoints[0]);
		((Plane*)ParentShape2)->getEndPoints(&Plane2EndPoints[0]);
	
		double pt1[3], pt2[3];
		((Plane*)ParentShape1)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape1)->getPoint3()->FillDoublePointer(&pt2[0], 3);
		double mid1[3] = {(pt1[0] + pt2[0])/2, (pt1[1] + pt2[1])/2, (pt1[2] + pt2[2])/2};

		((Plane*)ParentShape2)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape2)->getPoint3()->FillDoublePointer(&pt2[0], 3);

		double mid2[3] = {(pt1[0] + pt2[0])/2, (pt1[1] + pt2[1])/2, (pt1[2] + pt2[2])/2};
		if(RMATH3DOBJECT->Checking_Parallel_Plane_to_Plane(&Plane1Param[0], &Plane2Param[0]))
		{
			TotalNormalmeasureType(1);
			double Dist = RMATH3DOBJECT->Distance_Plane_Plane(&Plane1Param[0], &Plane2Param[0]);
			setDimension(Dist);
			RMATH3DOBJECT->TransformationMatrix_Dist_Pln2Pln(&Plane1Param[0], &Plane2Param[0], &mid1[0], &mid2[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	        PlaneMtype = PLANEMEASURETYPE::DISTANCEONLY;
		}
		else
		{
			this->DistanceMeasurement(false);
			if(NormalmeasureType() == 0)
			{
		 		RMATH3DOBJECT->TransformationMatrix_Angle_Pln2Pln(&Plane1Param[0], &Plane2Param[0], &mid1[0], &TransformationMatrix[0], &MeasurementPlane[0]);
			    double Dist = RMATH3DOBJECT->Angle_Plane_Plane(&Plane1Param[0], &Plane2Param[0], MeasurementPlane, DimSelectionLine);
		        this->setAngularDimension(Dist);
			}
			else
			{
				double XYPlane[4] = {0, 0, 1, 0}, XYProjectLine[6];
				RMATH3DOBJECT->Intersection_Plane_Plane(&Plane1Param[0], &Plane2Param[0], IntersectionLine);
				RMATH3DOBJECT->Projection_Line_on_Plane(IntersectionLine, XYPlane, XYProjectLine);
				RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Ln(&IntersectionLine[0], &XYProjectLine[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				double Dist = RMATH3DOBJECT->Angle_Line_Line(&IntersectionLine[0], &XYProjectLine[0], MeasurementPlane, &DimSelectionLine[0]);
				this->setAngularDimension(Dist);
			}
			PlaneMtype = PLANEMEASURETYPE::ANGLEONLY;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0006", __FILE__, __FUNCSIG__); }
}

void DimPlaneToPlaneDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(NormalmeasureType() == 0)
		{
		   GRAFIX->drawPlane_PlaneDistOrAng3D(&Plane1Param[0], &Plane1EndPoints[0], &Plane2Param[0], &Plane2EndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], DistanceMeasurement(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		}
		else
		{
		    double projectionPnts[24], EndPt1[6], Minr, Maxr, XYPlane[4] = {0, 0, 1, 0}, XYProjctLine[6], XYProjctPnts[6];
		    for(int i = 0; i < 4; i++)
		    {
			   RMATH3DOBJECT->Projection_Point_on_Line(&Plane1EndPoints[3 * i], IntersectionLine, &projectionPnts[3 * i]);
			   RMATH3DOBJECT->Projection_Point_on_Line(&Plane2EndPoints[3 * i], IntersectionLine, &projectionPnts[12 + 3 * i]);
		    }
			HELPEROBJECT->CalculateEndPoints_3DLineBestfit(8, projectionPnts, IntersectionLine, EndPt1, &EndPt1[3], &Minr, &Maxr);
			RMATH3DOBJECT->Projection_Line_on_Plane(IntersectionLine, XYPlane,  XYProjctLine);
			RMATH3DOBJECT->Projection_Point_on_Plane(EndPt1, XYPlane, XYProjctPnts);
			RMATH3DOBJECT->Projection_Point_on_Plane(&EndPt1[3], XYPlane, &XYProjctPnts[3]);
			GRAFIX->drawLine_3DStipple(EndPt1[0], EndPt1[1], EndPt1[2], EndPt1[3], EndPt1[4], EndPt1[5]);
			GRAFIX->drawLine_3DStipple(XYProjctPnts[0], XYProjctPnts[1], XYProjctPnts[2], XYProjctPnts[3], XYProjctPnts[4], XYProjctPnts[5]);
			GRAFIX->drawFiniteline_FinitelineAngle3D(&IntersectionLine[0], &EndPt1[0], &XYProjctLine[0], &XYProjctPnts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		    double mid1[3] = {(Plane1EndPoints[0] + Plane1EndPoints[6])/2, (Plane1EndPoints[1] + Plane1EndPoints[7])/2, (Plane1EndPoints[2] + Plane1EndPoints[8])/2};
			double mid2[3] = {(Plane2EndPoints[0] + Plane2EndPoints[6])/2, (Plane2EndPoints[1] + Plane2EndPoints[7])/2, (Plane2EndPoints[2] + Plane2EndPoints[8])/2};
	        RMATH3DOBJECT->Projection_Point_on_Line(mid1, IntersectionLine, XYProjctPnts);
            RMATH3DOBJECT->Projection_Point_on_Line(mid2, IntersectionLine, &XYProjctPnts[3]);
			GRAFIX->drawLine_3DStipple(mid1[0], mid1[1], mid1[2], XYProjctPnts[0], XYProjctPnts[1], XYProjctPnts[2]);
			GRAFIX->drawLine_3DStipple(mid2[0], mid2[1], mid2[2], XYProjctPnts[3], XYProjctPnts[4], XYProjctPnts[5]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0007", __FILE__, __FUNCSIG__); }
}

void DimPlaneToPlaneDistance::UpdateForParentChange(BaseItem* sender)
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

void DimPlaneToPlaneDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimPlaneToPlaneDistance::Clone(int n)
{
	try
	{
		DimPlaneToPlaneDistance* Cdim = new DimPlaneToPlaneDistance(_T("Dist"));
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPlaneToPlaneDistance::CreateDummyCopy()
{
	try
	{
		DimPlaneToPlaneDistance* Cdim = new DimPlaneToPlaneDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPlaneToPlaneDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPlaneToPlaneDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPlaneToPlaneDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimPlaneToPlaneDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPlaneToPlaneDistance& x)
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
			if(Tagname==L"DimPlaneToPlaneDistance")
			{
			while(Tagname!=L"EndDimPlaneToPlaneDistance")
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

void ReadOldPP(wistream& is, DimPlaneToPlaneDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0013", __FILE__, __FUNCSIG__);}
}