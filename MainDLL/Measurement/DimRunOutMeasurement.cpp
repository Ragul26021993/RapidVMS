#include "StdAfx.h"
#include "DimRunOutMeasurement.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "../Shapes/Cylinder.h"
#include "../Shapes/Cone.h"
#include "..//Shapes/Plane.h"
#include "..\Engine\PointCollection.h"

DimRunOutMeasurement::DimRunOutMeasurement(TCHAR* myname, RapidEnums::MEASUREMENTTYPE mtype):DimBase(genName(myname))
{
	try
	{
		IntersectionPnts[0] = 0;
		IntersectionPnts[1] = 0;
		IntersectionPnts[2] = 0;
		IntersectionPnts[3] = 0;
        this->MeasurementType =  mtype;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("DimRunOutMeasurement001", __FILE__, __FUNCSIG__);
	}
}

DimRunOutMeasurement::DimRunOutMeasurement(bool simply):DimBase(false)
{
	IntersectionPnts[0] = 0;
	IntersectionPnts[1] = 0;
	IntersectionPnts[2] = 0;
	IntersectionPnts[3] = 0;
}

DimRunOutMeasurement::~DimRunOutMeasurement()
{
}

TCHAR* DimRunOutMeasurement::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DimRunOutMeasurement002", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimRunOutMeasurement::CalculateMeasurement()
{
	try
	{
		double dist = 0;
		if (ParentsShape.size() < 2) return;
		if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTFARTHEST)
		{
			dist = -1000;
			for (list<Shape*>::iterator it1 = ParentsShape.begin(); it1 != ParentsShape.end(); it1++)
			{
				Line* line1 = (Line*)(*it1);
				for (list<Shape*>::iterator it2 = ParentsShape.begin(); it2 != ParentsShape.end(); it2++)
				{
					if (line1->getId() == (*it2)->getId()) continue;
					Line* line2 = (Line*)(*it2);
					Vector* MidPt;
					MidPt = line1->getMidPoint();
					double tmpdist = RMATH2DOBJECT->Pt2Line_Dist(MidPt->getX(), MidPt->getY(), line2->Angle(), line2->Intercept());
					if (abs(tmpdist) > dist)
					{
						dist = abs(tmpdist);
						ParentShape1 = line1;
						ParentShape2 = line2;
					}
				}
			}
		}
		else if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTNEAREST)
		{
			dist = 1000;
			for (list<Shape*>::iterator it1 = ParentsShape.begin(); it1 != ParentsShape.end(); it1++)
			{
				Line* line1 = (Line*)(*it1);
				for (list<Shape*>::iterator it2 = ParentsShape.begin(); it2 != ParentsShape.end(); it2++)
				{
					if (line1->getId() == (*it2)->getId()) continue;
					Line* line2 = (Line*)(*it2);
					double tmpdist = RMATH2DOBJECT->Line2Line_dis(line1->Angle(), line1->Intercept(), line2->Angle(), line2->Intercept());
					if (abs(tmpdist) < dist)
					{
						dist = abs(tmpdist);
						ParentShape1 = line1;
						ParentShape2 = line2;
					}
				}
			}
		}
		else if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTFARTHEST)
		{
			if (ParentsShape.size() < 3) return;
			dist = -1000;
			list<Shape*>::iterator it1 = ParentsShape.begin();
			Line* parentLine = (Line*)(*it1);
			for (it1 = ParentsShape.begin(); it1 != ParentsShape.end(); it1++)
			{
				Line* line1 = (Line*)(*it1);
				if (parentLine->getId() == line1->getId()) continue;
				for (list<Shape*>::iterator it2 = ParentsShape.begin(); it2 != ParentsShape.end(); it2++)
				{
					if (parentLine->getId() == (*it2)->getId()) continue;
					if (line1->getId() == (*it2)->getId()) continue;
					double intersectPnt[4] = { 0 };
					Line* line2 = (Line*)(*it2);
					RMATH2DOBJECT->Line_lineintersection(line1->Angle(), line1->Intercept(), parentLine->Angle(), parentLine->Intercept(), intersectPnt);
					RMATH2DOBJECT->Line_lineintersection(line2->Angle(), line2->Intercept(), parentLine->Angle(), parentLine->Intercept(), &intersectPnt[2]);
					double tmpdist = RMATH2DOBJECT->Pt2Pt_distance(intersectPnt[0], intersectPnt[1], intersectPnt[2], intersectPnt[3]);
					if (abs(tmpdist) > dist)
					{
						dist = abs(tmpdist);
						for (int i = 0; i < 4; i++)
							IntersectionPnts[i] = intersectPnt[i];
						IntersectionPnts[4] = line1->getPoint1()->getZ();
					}
				}
			}
		}
		else if ((this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER) || (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_MICROMETRICCUTTINGDIAMETER))
		{
			if (ParentsShape.size() < 2) return;
			if (MAINDllOBJECT->Vblock_CylinderAxisLine == NULL) return;
			Line* AxisLine = (Line*)MAINDllOBJECT->Vblock_CylinderAxisLine;
			list<Shape*>::iterator it1 = ParentsShape.begin();
			Line* Line1 = (Line*)(*it1);
			it1++;
			Line* Line2 = (Line*)(*it1);
			Vector* MidPt;
			MidPt = Line1->getMidPoint();
			double tmpdist1 = RMATH2DOBJECT->Pt2Line_Dist(MidPt->getX(), MidPt->getY(), AxisLine->Angle(), AxisLine->Intercept());
			MidPt = Line2->getMidPoint();
			double tmpdist2 = RMATH2DOBJECT->Pt2Line_Dist(MidPt->getX(), MidPt->getY(), AxisLine->Angle(), AxisLine->Intercept());
			if (tmpdist1 > tmpdist2)
			{
				ParentShape1 = Line1;
				ParentShape2 = Line2;
				if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER)
					dist = 2.0 * tmpdist1;
				else
					dist = tmpdist1 + tmpdist2;
			}
			else
			{
				ParentShape1 = Line2;
				ParentShape2 = Line1;
				if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER)
					dist = 2.0 * tmpdist2;
				else
					dist = tmpdist1 + tmpdist2;
			}
		}

		else if ((this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DIST1) || (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DIST2))
		{
			if (ParentsShape.size() < 2) return;
			if (MAINDllOBJECT->Vblock_CylinderAxisLine == NULL) return;
			Line* AxisLine = (Line*)MAINDllOBJECT->Vblock_CylinderAxisLine;
			list<Shape*>::iterator it1 = ParentsShape.begin();
			Line* Line1 = (Line*)(*it1);
			it1++;
			Line* Line2 = (Line*)(*it1);
			Vector* MidPt;
			MidPt = Line1->getMidPoint();
			double tmpdist1 = RMATH2DOBJECT->Pt2Line_Dist(MidPt->getX(), MidPt->getY(), AxisLine->Angle(), AxisLine->Intercept());
			MidPt = Line2->getMidPoint();
			double tmpdist2 = RMATH2DOBJECT->Pt2Line_Dist(MidPt->getX(), MidPt->getY(), AxisLine->Angle(), AxisLine->Intercept());

			if (tmpdist1 > tmpdist2)
			{
				ParentShape1 = Line1;
				ParentShape2 = Line2;
			}
			else
			{
				ParentShape1 = Line2;
				ParentShape2 = Line1;
			}
			if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DIST1)
				dist = tmpdist1;
			else
				dist = tmpdist2;
		}

		else if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTNEAREST)
		{
			if (ParentsShape.size() < 3) return;
			dist = 1000;
			list<Shape*>::iterator it1 = ParentsShape.begin();
			Line* parentLine = (Line*)(*it1);
			for (it1 = ParentsShape.begin(); it1 != ParentsShape.end(); it1++)
			{
				Line* line1 = (Line*)(*it1);
				if (parentLine->getId() == line1->getId()) continue;
				for (list<Shape*>::iterator it2 = ParentsShape.begin(); it2 != ParentsShape.end(); it2++)
				{
					if (parentLine->getId() == (*it2)->getId()) continue;
					if (line1->getId() == (*it2)->getId()) continue;
					double intersectPnt[4] = { 0 };
					Line* line2 = (Line*)(*it2);
					RMATH2DOBJECT->Line_lineintersection(line1->Angle(), line1->Intercept(), parentLine->Angle(), parentLine->Intercept(), intersectPnt);
					RMATH2DOBJECT->Line_lineintersection(line2->Angle(), line2->Intercept(), parentLine->Angle(), parentLine->Intercept(), &intersectPnt[2]);
					double tmpdist = RMATH2DOBJECT->Pt2Pt_distance(intersectPnt[0], intersectPnt[1], intersectPnt[2], intersectPnt[3]);
					if (abs(tmpdist) < abs(dist))
					{
						dist = abs(tmpdist);
						for (int i = 0; i < 4; i++)
							IntersectionPnts[i] = intersectPnt[i];
						IntersectionPnts[4] = line1->getPoint1()->getZ();
					}
				}
			}
		}

		else if (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_AXIALRUNOUT)
		{
			if (ParentsShape.size() < 2) return;
			dist = 1000;
			double Plane1Param[4] = { 0 }, RefShape[8] = { 0 }, TmpPt[3] = { 0 }, IdealPlane[4] = { 0 };
			Plane* ActualPlane = (Plane*)ParentShape1;
			if (ParentShape2->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				((Plane*)ParentShape2)->getParameters(&IdealPlane[0]);
			else
			{
				if (ParentShape2->ShapeType == RapidEnums::SHAPETYPE::LINE3D || ParentShape2->ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
					((Line*)ParentShape2)->getParameters(&RefShape[0]);
				else if (ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
					((Cylinder*)ParentShape2)->getParameters(&RefShape[0]);
				else if (ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CONE)
					((Cone*)ParentShape2)->getParameters(&RefShape[0]);
				else
					return;
				ActualPlane->getParameters(&IdealPlane[0]);
				//IdealPlane[3] = 0;
				for (int j = 0; j < 3; j++)
				{
					IdealPlane[j] = RefShape[3 + j];
					//IdealPlane[3] += IdealPlane[j] * RefShape[j];
				}
			}
			dist = RMATH3DOBJECT->Most_DirectionalDist_Point_to_Plane(ActualPlane->pts, ActualPlane->PointsList->Pointscount(), &RefShape[3], TmpPt);
			dist -= RMATH3DOBJECT->Least_DirectionalDist_Point_to_Plane(ActualPlane->pts, ActualPlane->PointsList->Pointscount(), &RefShape[3], TmpPt);
		}
		this->setDimension(dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0005", __FILE__, __FUNCSIG__); }
}

void DimRunOutMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(this->MeasurementType ==  RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTFARTHEST || this->MeasurementType ==  RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTNEAREST)
		{
			if(ParentShape1 == NULL || ParentShape2 == NULL) return;
			Line* line1 = (Line*)ParentShape1;
			Line* line2 = (Line*)ParentShape2;
			double Endpoints[8] = {0}, Zlevel = line1->getPoint1()->getZ();
			line1->getPoint1()->FillDoublePointer(&Endpoints[0], 2);
			line1->getPoint2()->FillDoublePointer(&Endpoints[2], 2);
			line2->getPoint1()->FillDoublePointer(&Endpoints[4], 2);
			line2->getPoint2()->FillDoublePointer(&Endpoints[6], 2);
			this->mposition.set((Endpoints[0] + Endpoints[2] + Endpoints[4] + Endpoints[6]) / 4, (Endpoints[1] + Endpoints[3] + Endpoints[5] + Endpoints[7]) / 4, Zlevel);
			GRAFIX->drawFiniteline_FinitelineDistance(&Endpoints[0], &Endpoints[2], &Endpoints[4], &Endpoints[6], line1->Angle(), line1->Intercept(), line2->Angle(), line2->Intercept(), doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
		}
		else if((this->MeasurementType ==  RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER) || (this->MeasurementType ==  RapidEnums::MEASUREMENTTYPE::DIM_MICROMETRICCUTTINGDIAMETER))
		{
			if(ParentShape1 == NULL || MAINDllOBJECT->Vblock_CylinderAxisLine == NULL) return;
			Line* line1 = (Line*)ParentShape1;
			Line* line2 = (Line*)MAINDllOBJECT->Vblock_CylinderAxisLine ;
			double Endpoints[8] = {0}, Zlevel = line1->getPoint1()->getZ();
			line1->getPoint1()->FillDoublePointer(&Endpoints[0], 2);
			line1->getPoint2()->FillDoublePointer(&Endpoints[2], 2);

			RMATH2DOBJECT->Projection_Of_Point_Online(&Endpoints[0], line2->Angle(), line2->Intercept(), &Endpoints[4]);
			RMATH2DOBJECT->Projection_Of_Point_Online(&Endpoints[2], line2->Angle(), line2->Intercept(), &Endpoints[6]);
			
			this->mposition.set((Endpoints[0] + Endpoints[2] + Endpoints[4] + Endpoints[6]) / 4, (Endpoints[1] + Endpoints[3] + Endpoints[5] + Endpoints[7]) / 4, Zlevel);
			GRAFIX->drawFiniteline_FinitelineDistance(&Endpoints[0], &Endpoints[2], &Endpoints[4], &Endpoints[6], line1->Angle(), line1->Intercept(), line2->Angle(), line2->Intercept(), doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
		}
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_AXIALRUNOUT)
		{
			double pt1[3], pt2[3], PlaneMidPt[4] = { 0 };
			((Plane*)ParentShape1)->getMidPoint()->FillDoublePointer(PlaneMidPt, 3); // getPoint1()->FillDoublePointer(&pt1[0], 3);
			//((Plane*)ParentShape1)->getPoint3()->FillDoublePointer(&pt2[0], 3);
			//PlaneMidPt[0] = (pt1[0] + pt2[0]) / 2;
			//PlaneMidPt[1] = (pt1[1] + pt2[1]) / 2;
			//PlaneMidPt[2] = (pt1[2] + pt2[2]) / 2;
			GRAFIX->drawAngularity3D(&DimSelectionLine[0], PlaneMidPt[0], PlaneMidPt[1], PlaneMidPt[2], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 3, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);

		}
		else
		{
			double Zlevel = IntersectionPnts[4];
			this->mposition.set((IntersectionPnts[0] + IntersectionPnts[2]) / 2, (IntersectionPnts[1] + IntersectionPnts[3]) / 2, Zlevel);
		    GRAFIX->drawPoint_PointDistance(IntersectionPnts[0], IntersectionPnts[1], IntersectionPnts[2], IntersectionPnts[3], this->mposition.getX(), this->mposition.getY(), this->doubledimesion(), this->getCDimension(), this->getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0006", __FILE__, __FUNCSIG__); }
}

void DimRunOutMeasurement::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		CalculateMeasurement();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0007", __FILE__, __FUNCSIG__); }
}

void DimRunOutMeasurement::UpdateMeasurement()
{
	CalculateMeasurement();
}

DimBase* DimRunOutMeasurement::Clone(int n)
{
	try
	{
		DimRunOutMeasurement* Cdim = new DimRunOutMeasurement();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0008", __FILE__, __FUNCSIG__); return NULL;}
}

DimBase* DimRunOutMeasurement::CreateDummyCopy()
{
	try
	{
		DimRunOutMeasurement* Cdim = new DimRunOutMeasurement(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0009", __FILE__, __FUNCSIG__); }
}

void DimRunOutMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
	     this->ParentsShape.clear();
		 DimRunOutMeasurement *tmpdim = (DimRunOutMeasurement *)(Cdim);
		 for(list<Shape*>::iterator it1 = tmpdim->ParentsShape.begin(); it1 != tmpdim->ParentsShape.end(); it1++)
		 {
			  this->ParentsShape.push_back((*it1));
		 }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimRunOutMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimRunOutMeasurement"<<endl;
		for(list<Shape*>::iterator it1 = x.ParentsShape.begin(); it1 != x.ParentsShape.end(); it1++)
		{
			os <<"ParentShape:"<< (*it1)->getId() << endl;
		}
		os<<"EndDimRunOutMeasurement"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimRunOutMeasurement& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimRunOutMeasurement")
		{
		    while(Tagname!=L"EndDimRunOutMeasurement")
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
		            if(Tagname==L"ParentShape")
					{
						x.ParentsShape.push_back((Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())]);
					}
					if (x.MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_AXIALRUNOUT)
					{
						if (x.ParentsShape.size() == 1)
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						else
							x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
					}
				}
			}
	     	x.UpdateMeasurement();
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
		return is;	
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0012", __FILE__, __FUNCSIG__); return is; }
}
