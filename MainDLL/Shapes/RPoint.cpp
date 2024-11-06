#include "StdAfx.h"
#include "RPoint.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Circle.h"
#include "Line.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\PartProgramFunctions.h"

RPoint::RPoint(TCHAR* myname, RapidEnums::SHAPETYPE Stype):ShapeWithList(genName(myname, Stype))
{
	init(Stype);
}

RPoint::RPoint(bool simply, RapidEnums::SHAPETYPE Stype):ShapeWithList(false)
{
	init(Stype);
}

RPoint::~RPoint()
{
}

void RPoint::init(RapidEnums::SHAPETYPE Stype)
{
	try
	{
		this->ShapeType = Stype;
		this->PointType =  RPoint::RPOINTTYPE::POINT;
		if(ShapeType == RapidEnums::SHAPETYPE::RPOINT3D || ShapeType == RapidEnums::SHAPETYPE::RELATIVEPOINT)
			this->ShapeAs3DShape(true);
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->position), 0), 0);
		this->IsValid(false);
		RelativeOffsetX = 0;
		RelativeOffsetY = 0;
		RelativeOffsetZ = 0;
		if(ShapeType == RapidEnums::SHAPETYPE::RELATIVEPOINT && !PPCALCOBJECT->IsPartProgramRunning())
		{
			this->PointType = RPOINTTYPE::RELATIVEPOINT3D;
			MAINDllOBJECT->DerivedShapeCallback();
		}
		this->ParentPoint = NULL;
		this->ProbeCorrectionShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0001", __FILE__, __FUNCSIG__); }
}

TCHAR* RPoint::genName(const TCHAR* prefix, RapidEnums::SHAPETYPE Stype)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10]; int Cnt;
		if(strcmp((const char*)prefix, "d") == 0)
		{
			if(Stype == RapidEnums::SHAPETYPE::RPOINT)
				Cnt = DXFshapenumber++;
			else
				Cnt = DXFshapenumber3D++;
		}
		else
		{
			if(Stype == RapidEnums::SHAPETYPE::RPOINT)
				Cnt = shapenumber++;
			else
				Cnt = shapenumber3D++;
		}
		Cnt++;
		_itot_s(Cnt, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0002", __FILE__, __FUNCSIG__); return name; }
}

bool RPoint::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	return false;
}

bool RPoint::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	try{ return (RMATH2DOBJECT->Ptisinwindow(position.getX(), position.getY(),corner1.getX(), corner2.getY(), corner2.getX(), corner1.getY())); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0003", __FILE__, __FUNCSIG__); }
}

bool RPoint::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void RPoint::Translate(Vector& Position)
{
	position += Position;
	this->UpdateEnclosedRectangle();
}


void RPoint::AlignToBasePlane(double* trnasformM)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, position.getX(), position.getY(), position.getZ());
		setpoint(Vector(temp.getX(), temp.getY(), temp.getZ()));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0003", __FILE__, __FUNCSIG__); }
}

void RPoint::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		GRAFIX->drawPoint(position.getX(), position.getY(), position.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0004", __FILE__, __FUNCSIG__); }
}

void RPoint::drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM)
{
	try
	{
		double temp1[4] = {position.getX(), position.getY(), position.getZ(), 1}, temp2[4]; 
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransFormM, Order1, temp1, Order2, temp2);
		GRAFIX->drawPoint(temp2[0], temp2[1], temp2[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0004", __FILE__, __FUNCSIG__); }
}

void RPoint::drawGDntRefernce()
{
}

void RPoint::ResetShapeParameters()
{
	try
	{
		RelativeOffsetX = 0;
		RelativeOffsetY = 0;
		RelativeOffsetZ = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0004a", __FILE__, __FUNCSIG__); }
}

void RPoint::UpdateBestFit()
{
	try
	{
		this->IsValid(false);
		if(this->PointType == RPOINTTYPE::PROJECTEDPOINT3D)
		{
			updateProjectedPoints();
			return;
		}
		else if(PointType == RPOINTTYPE::INTERSECTIONPOINT3D)
		{
			updateIntersectionPoint();
			return;
		}
		else if(this->PointType == RPOINTTYPE::RELATIVEPOINT3D)
		{
			updateRelativePoint();
			return;
		}
		else
		{
			int totcount = this->PointsList->Pointscount();
			if(totcount < 1) return;
			if(PointType == RPOINTTYPE::POINT || PointType == RPOINTTYPE::POINT3D)
				updateNormalPoints();
			else
				updateDerivedPoints();
			double ans[2] = {0};
			if(PointType == RPOINTTYPE::POINT || PointType == RPOINTTYPE::POINT3D || PointType == RPOINTTYPE::POINTONLINE || PointType == RPOINTTYPE::POINTONCIRCLE)
			{
				PC_ITER Item = this->PointsList->getList().begin();
				SinglePoint* Spt = (*Item).second;
				setpoint(Vector(Spt->X, Spt->Y, Spt->Z));
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

bool RPoint::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		 int s1[2] = {4, 4}, s2[2] = {4, 1};
		 double PtPosition[4] = {position.getX(), position.getY(), position.getZ(), 1}, transformedPt[4] = {0};
		 RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &PtPosition[0], &s2[0], &transformedPt[0]);
		 Lefttop[0] = transformedPt[0],  Lefttop[1] = transformedPt[1], Rightbottom[0] = transformedPt[0],  Rightbottom[1] = transformedPt[1];
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0004", __FILE__, __FUNCSIG__); return false; }
}

void RPoint::updateRelativePoint()
{
	this->IsValid(false);
	double actnPnts[9] = {0};
	int cnt = 0;
	if(!PPCALCOBJECT->IsPartProgramRunning())
	{
		for each(RAction* Caction in this->PointAtionList)
		{
			if(cnt >= 9) break;
			FramegrabBase *fg = ((AddPointAction*)Caction)->getFramegrab();
			if(fg->CurrentWindowNo == 1 && fg->Pointer_SnapPt != NULL)
			{
				actnPnts[cnt++] = fg->Pointer_SnapPt->getX();
				actnPnts[cnt++] = fg->Pointer_SnapPt->getY();
				actnPnts[cnt++] = fg->Pointer_SnapPt->getZ();
			}
			else
			{
				int i = 0, k = cnt/3;
				for(PC_ITER spt = this->PointsList->getList().begin(); spt != this->PointsList->getList().end(); spt++, i++)
				{
					if(i != k) continue;
					actnPnts[cnt++] = spt->second->X;
					actnPnts[cnt++] = spt->second->Y;
					actnPnts[cnt++] = spt->second->Z;
					break;
				}
			}
		}
	}
	else
	{
		for(RC_ITER curraction = PPCALCOBJECT->getPPActionlist().getList().begin(); curraction != PPCALCOBJECT->getPPActionlist().getList().end(); curraction++)
		{
			if(cnt >= 9) break;
			RAction* action = (RAction*)(*curraction).second;
			if(!action->ActionStatus()) continue;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				if(this->getId() != ((AddPointAction*)action)->getShape()->getId())	continue;		
				FramegrabBase *fg = ((AddPointAction*)action)->getFramegrab();
				if(fg->CurrentWindowNo == 1 && fg->Pointer_SnapPt != NULL)
				{
					actnPnts[cnt++] = fg->Pointer_SnapPt->getX();
					actnPnts[cnt++] = fg->Pointer_SnapPt->getY();
					actnPnts[cnt++] = fg->Pointer_SnapPt->getZ();
				}
				else
				{
					int i = 0, k = cnt/3;
					for(PC_ITER spt = this->PointsList->getList().begin(); spt != this->PointsList->getList().end(); spt++, i++)
					{
						if(i != k) continue;
						actnPnts[cnt++] = spt->second->X;
						actnPnts[cnt++] = spt->second->Y;
						actnPnts[cnt++] = spt->second->Z;
						break;
					}
				}
			}
		}
	}
	if(cnt < 1) return;
	//ProbePointCalculations(this->PointsList->getList().size());
	switch(cnt)
	{ 
		case 3:
			{
				setpoint(Vector(actnPnts[0] + RelativeOffsetX, actnPnts[1] + RelativeOffsetY, actnPnts[2] + RelativeOffsetZ));
			}
			break;
		case 6:
			{
				setpoint(Vector(actnPnts[0] + RelativeOffsetX, actnPnts[1] + RelativeOffsetY, actnPnts[5] + RelativeOffsetZ));
			}
			break;
		case 9:
			{
				setpoint(Vector(actnPnts[0] + RelativeOffsetX, actnPnts[4] + RelativeOffsetY, actnPnts[8] + RelativeOffsetZ));
			}
		break;	
	}
	this->IsValid(true);
}

void RPoint::updateNormalPoints()
{
	try
	{
		ProbePointCalculations(this->PointsList->getList().size());
		PC_ITER Item = this->PointsList->getList().begin();
		SinglePoint* Spt = (*Item).second;
		setpoint(Vector(Spt->X, Spt->Y, Spt->Z));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updateDerivedPoints()
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			for each(RAction* Caction in PointAtionList)
			{
				AddPointAction* CptAction = (AddPointAction*)Caction;
				if(CptAction->getFramegrab()->CurrentWindowNo == 1)
				{
					if(CptAction->getFramegrab()->Pointer_SnapPt != NULL)
					{
						SinglePoint* Spt = this->PointsList->getList()[(int)(*CptAction->getFramegrab()->PointActionIdList.begin())];
						Spt->SetValues(CptAction->getFramegrab()->Pointer_SnapPt->getX(), CptAction->getFramegrab()->Pointer_SnapPt->getY(), CptAction->getFramegrab()->Pointer_SnapPt->getZ());
						Spt = this->PointsListOriginal->getList()[(int)(*CptAction->getFramegrab()->PointActionIdList.begin())];
						Spt->SetValues(CptAction->getFramegrab()->Pointer_SnapPt->getX(), CptAction->getFramegrab()->Pointer_SnapPt->getY(), CptAction->getFramegrab()->Pointer_SnapPt->getZ());
					}
				}
			}
		}
		int PointsCount = this->PointsList->Pointscount();
		if(PointType == RPoint::RPOINTTYPE::NEARESTPOINT)
			updateNearestpoint(PointsCount);
		else if(PointType == RPoint::RPOINTTYPE::FARTHESTPOINT)
			updateFarthestpoint(PointsCount);
		else if(PointType == RPoint::RPOINTTYPE::NEARESTPOINT3D)
			updateNearestpoint3D(PointsCount);
		else if(PointType == RPoint::RPOINTTYPE::FARTHESTPOINT3D)
			updateFarthestpoint3D(PointsCount);
		else if(PointType == RPoint::RPOINTTYPE::MIDPOINT)
			updateMidpoint(PointsCount);
		else if(PointType == RPoint::RPOINTTYPE::POINTONLINE)
			updatePointOnLine(PointsCount);
		else if(PointType == RPoint::RPOINTTYPE::POINTONCIRCLE)
			updatePointOnCircle(PointsCount);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updateProjectedPoints()
{
	if(this->getParents().empty()) return;
	Shape *parentShape1 = (Shape*)(*(this->getParents().begin()));
	if(parentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE || parentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
	{
		double projPnt[3] = {0}, lineParam[6] = {0}, cpoint[3] = {ParentPoint->getX(), ParentPoint->getY(), ParentPoint->getZ()};
		((Line*)parentShape1)->getParameters(lineParam);
		RMATH3DOBJECT->Projection_Point_on_Line(cpoint, lineParam, projPnt);
		setpoint(Vector(projPnt[0], projPnt[1], projPnt[2]));
		this->IsValid(true);
	}
	else if(parentShape1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || parentShape1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || 
		parentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC || parentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Shape* ParentSh = (Shape*)(*ptr);
		if(ParentSh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			double projPnt[3] = {0}, planeParam[4] = {0}, cpoint[3] = {((Circle*)parentShape1)->getCenter()->getX(), ((Circle*)parentShape1)->getCenter()->getY(), ((Circle*)parentShape1)->getCenter()->getZ()};
			((Plane*)ParentSh)->getParameters(planeParam);
			RMATH3DOBJECT->Projection_Point_on_Plane(cpoint, planeParam, projPnt);
			setpoint(Vector(projPnt[0], projPnt[1], projPnt[2]));
			this->IsValid(true);
		}
	}
	else if(parentShape1->ShapeType == RapidEnums::SHAPETYPE::PLANE && ParentPoint != NULL)
	{
		double projPnt[3] = {0}, planeParam[4] = {0}, cpoint[3] = {ParentPoint->getX(), ParentPoint->getY(), ParentPoint->getZ()};
		((Plane*)parentShape1)->getParameters(planeParam);
		RMATH3DOBJECT->Projection_Point_on_Plane(cpoint, planeParam, projPnt);
		setpoint(Vector(projPnt[0], projPnt[1], projPnt[2])); 
		this->IsValid(true);
	}
	return;

}

void RPoint::updateIntersectionPoint()
{
	if(this->getParents().size() == 2)
	{
		double intersectionPnt[3] = {0}, lineParam[6] = {0}, planeParam[6] = {0};
		Shape *parent1, *parent2;
		list<BaseItem*>::iterator It = this->getParents().begin();
		parent1 = (Shape*)(*It++);
		parent2 = (Shape*)(*It);
		if(compareboth<int>(parent1->ShapeType, parent2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::PLANE) || 
			compareboth<int>(parent1->ShapeType, parent2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::PLANE) ||
			compareboth<int>(parent1->ShapeType, parent2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::PLANE))
		{
			if (parent1->ShapeType != RapidEnums::SHAPETYPE::LINE3D && parent1->ShapeType != RapidEnums::SHAPETYPE::XLINE3D)
			{
				Shape* temp = parent1;
				parent1 = parent2;
				parent2 = temp;
			}
			
			((Line*)parent1)->getParameters(lineParam);
			((Plane*)parent2)->getParameters(planeParam);
			RMATH3DOBJECT->Intersection_Line_Plane(lineParam, planeParam, intersectionPnt);
		}
		else
		{
			RMATH2DOBJECT->Line_lineintersection(((Line*)parent1)->Angle(), ((Line*)parent1)->Intercept(), ((Line*)parent2)->Angle(), ((Line*)parent2)->Intercept(), intersectionPnt);
			intersectionPnt[2] = ((Line*)parent1)->getPoint1()->getZ();
		}
		setpoint(Vector(intersectionPnt[0], intersectionPnt[1], intersectionPnt[2]));
		this->IsValid(true);
	}
}

void RPoint::updateMidpoint(int PointsCount)
{
	try
	{
		if(PointsCount < 2) return;
		PC_ITER Item = this->PointsList->getList().begin();
		SinglePoint* Spt1, *Spt2;
		Spt1 = (*Item).second;
		Item++;
		Spt2 = (*Item).second;
		setpoint(Vector((Spt1->X + Spt2->X)/2, (Spt1->Y + Spt2->Y)/2, 0));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updateNearestpoint(int PointsCount)
{
	try
	{
		if(this->getParents().size() < 1) return;
		double ans[2] = {0};
		ShapeWithList* ParentShape = (ShapeWithList*)(*this->getParents().begin());
		if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::LINE || ParentShape->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			RMATH2DOBJECT->Nearestpoint_line(PointsCount, pts, ((Line*)ParentShape)->Angle(),((Line*)ParentShape)->Intercept(), &ans[0]);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || ParentShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			double Cen[2] = {((Circle*)ParentShape)->getCenter()->getX(),((Circle*)ParentShape)->getCenter()->getY()};
			RMATH2DOBJECT->Nearestpoint_circle(PointsCount, pts, &Cen[0], ((Circle*)ParentShape)->Radius(), &ans[0]);
		}
		setpoint(Vector(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ()));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updateFarthestpoint(int PointsCount)
{
	try
	{
		if(this->getParents().size() < 1) return;
		double ans[2] = {0};
		ShapeWithList* ParentShape = (ShapeWithList*)(*this->getParents().begin());
		if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::LINE || ParentShape->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			RMATH2DOBJECT->Farthestpoint_line(PointsCount, pts, ((Line*)ParentShape)->Angle(),((Line*)ParentShape)->Intercept(), &ans[0]);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || ParentShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			double Cen[2] = {((Circle*)ParentShape)->getCenter()->getX(),((Circle*)ParentShape)->getCenter()->getY()};
			RMATH2DOBJECT->Farthestpoint_circle(PointsCount, pts, &Cen[0], ((Circle*)ParentShape)->Radius(), &ans[0]);
		}
		setpoint(Vector(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ()));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updateNearestpoint3D(int PointsCount)
{
	try
	{
		if(this->getParents().size() < 1) return;
		double ans[3] = {0};
		ShapeWithList* ParentShape = (ShapeWithList*)(*this->getParents().begin());
		if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			double planeparam[4] = {0};
			((Plane*)ParentShape)->getParameters(planeparam);
			RMATH3DOBJECT->Nearest_Point_to_Plane(pts, PointsCount, planeparam, ans);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		{
			double sphereParam[4] = {0};
			((Sphere*)ParentShape)->getParameters(sphereParam);
			RMATH3DOBJECT->Nearest_Point_to_Sphere(pts, PointsCount, sphereParam, ans);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
		{
			double lineParam[6] = {0};
			((Line*)ParentShape)->getParameters(lineParam);
			RMATH3DOBJECT->Nearest_Point_to_Line(pts, PointsCount, lineParam, ans);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
		{
			double cylinderParam[7] = {0};
			((Cylinder*)ParentShape)->getParameters(cylinderParam);
			RMATH3DOBJECT->Nearest_Point_to_Cylinder(pts, PointsCount, cylinderParam, ans);
		}
		setpoint(Vector(ans[0], ans[1], ans[2]));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updateFarthestpoint3D(int PointsCount)
{
	try
	{
		if(this->getParents().size() < 1) return;
		double ans[3] = {0};
		ShapeWithList* ParentShape = (ShapeWithList*)(*this->getParents().begin());
		if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			double planeparam[4] = {0};
			((Plane*)ParentShape)->getParameters(planeparam);
			RMATH3DOBJECT->Farthest_Point_to_Plane(pts, PointsCount, planeparam, ans);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		{
			double sphereParam[4] = {0};
			((Sphere*)ParentShape)->getParameters(sphereParam);
			RMATH3DOBJECT->Farthest_Point_to_Sphere(pts, PointsCount, sphereParam, ans);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
		{
			double lineParam[6] = {0};
			((Line*)ParentShape)->getParameters(lineParam);
			RMATH3DOBJECT->Farthest_Point_to_Line(pts, PointsCount, lineParam, ans);
		}
		else if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
		{
			double cylinderParam[7] = {0};
			((Cylinder*)ParentShape)->getParameters(cylinderParam);
			RMATH3DOBJECT->Farthest_Point_to_Cylinder(pts, PointsCount, cylinderParam, ans);
		}
		setpoint(Vector(ans[0], ans[1], ans[2]));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updatePointOnLine(int PointsCount)
{
	try
	{
		if(this->getParents().size() < 1) return;
		ShapeWithList* ParentShape = (ShapeWithList*)(*this->getParents().begin());
		double mp[2] = {position.getX(), position.getY()}, point[2];
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)ParentShape)->Angle(), ((Line*)ParentShape)->Intercept(), &mp[0], &point[0]);
		setpoint(Vector(point[0], point[1], 0));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::updatePointOnCircle(int PointsCount)
{
	try
	{
		if(this->getParents().size() < 1) return;
		ShapeWithList* ParentShape = (ShapeWithList*)(*this->getParents().begin());
		double mp[2] = {position.getX(), position.getY()}, point[2];
		double Cen[2];
		((Circle*)ParentShape)->getCenter()->FillDoublePointer(&Cen[0]);
		RMATH2DOBJECT->Point_onCircle(&Cen[0],((Circle*)ParentShape)->Radius(), &mp[0], &point[0]);
		setpoint(Vector(point[0], point[1], 0));
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0005", __FILE__, __FUNCSIG__); }
}

void RPoint::ProbePointCalculations(int PointsCnt)
{
	try
	{		
		if(PointsCnt > 0)
		{
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				updatePoint_UsingPorbe();
		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0012", __FILE__, __FUNCSIG__);}
}

void RPoint::CheckPointIsForPorbe()
{
	try
	{
		if(PointAtionList.size() < 1) return;
		AddPointAction* Caction = (AddPointAction*)(*PointAtionList.begin());
		if(Caction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
		{
			if(this->ProbeCorrection())
				this->ShapeDoneUsingProbe(true);
			else
				this->ShapeDoneUsingProbe(false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0013", __FILE__, __FUNCSIG__);}
}

void RPoint::updatePoint_UsingPorbe()
{
	try
	{

		PC_ITER Item = this->PointsList->getList().begin();
		for(PC_ITER Original_Item = this->PointsListOriginal->getList().begin(); Original_Item != this->PointsListOriginal->getList().end(); Original_Item++)
		{
			SinglePoint* Spt = (*Original_Item).second;
			double Pradius = getProbeRadius(Spt->PLR);
					
			SinglePoint* Correct_Spt = (*Item).second;

			if(Spt->Pdir == 0) //FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT
			{
				Correct_Spt->SetValues(Spt->X - Pradius, Spt->Y, Spt->Z, Spt->R, Spt->Pdir);
			}
			else if(Spt->Pdir == 1) //FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT
			{
				Correct_Spt->SetValues(Spt->X + Pradius, Spt->Y, Spt->Z, Spt->R, Spt->Pdir);
			}
			else if(Spt->Pdir == 2) //FramegrabBase::PROBEDIRECTION::YTOP2BOTTOM
			{
				Correct_Spt->SetValues(Spt->X, Spt->Y - Pradius, Spt->Z, Spt->R, Spt->Pdir);
			}
			else if(Spt->Pdir == 3) // FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP
			{
				Correct_Spt->SetValues(Spt->X, Spt->Y + Pradius, Spt->Z, Spt->R, Spt->Pdir);
			}
			else if(Spt->Pdir == 4) // ZTOPTOBOTTOM
			{
				Correct_Spt->SetValues(Spt->X, Spt->Y, Spt->Z - Pradius, Spt->R, Spt->Pdir);
			}
			else if(Spt->Pdir == 5)
			{
				Correct_Spt->SetValues(Spt->X, Spt->Y, Spt->Z + Pradius, Spt->R, Spt->Pdir);
			}
			Item++;
		}

		//if(this->ProbeCorrectionShape != NULL && ProbeCorrectionShape->ShapeType == RapidEnums::SHAPETYPE::PLANE && ((ShapeWithList*)ProbeCorrectionShape)->PointsList->Pointscount() >= 3)
		//{
		//	double Pradius = 0, dir_X = ((Plane*)ProbeCorrectionShape)->dir_a(), dir_Y = ((Plane*)ProbeCorrectionShape)->dir_b(), dir_Z = ((Plane*)ProbeCorrectionShape)->dir_c();
		//	int multiplyfactor = -1;
		//	double dist = ((Plane*)ProbeCorrectionShape)->dir_d();
		//	for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		//	{
		//		SinglePoint* Spt = (*Item).second;
		//		double SurfacePt[3] = {0};
		//		Pradius = getProbeRadius(Spt->PLR);
		//		if(dir_X * Spt->X + dir_Y * Spt->Y + dir_Z * Spt->Z - dist < 0)
		//			multiplyfactor = 1;
		//		else
		//			multiplyfactor = -1;
		//		SurfacePt[0] = Spt->X + multiplyfactor * Pradius * dir_X;
		//		SurfacePt[1] = Spt->Y + multiplyfactor * Pradius * dir_Y;
		//		SurfacePt[2] = Spt->Z + multiplyfactor * Pradius * dir_Z;
		//		SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
		//		Spt1->SetValues(SurfacePt[0], SurfacePt[1], SurfacePt[2], Spt->R, Spt->Pdir);
		//	}
		//}
		//else
		//{
		//	PC_ITER Item = this->PointsList->getList().begin();
		//	for(PC_ITER Original_Item = this->PointsListOriginal->getList().begin(); Original_Item != this->PointsListOriginal->getList().end(); Original_Item++)
		//	{
		//		SinglePoint* Spt = (*Original_Item).second;
		//		double Pradius = getProbeRadius(Spt->PLR);
		//			
		//		SinglePoint* Correct_Spt = (*Item).second;

		//		if(Spt->Pdir == 0) //FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT
		//		{
		//			Correct_Spt->SetValues(Spt->X - Pradius, Spt->Y, Spt->Z, Spt->R, Spt->Pdir);
		//		}
		//		else if(Spt->Pdir == 1) //FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT
		//		{
		//			Correct_Spt->SetValues(Spt->X + Pradius, Spt->Y, Spt->Z, Spt->R, Spt->Pdir);
		//		}
		//		else if(Spt->Pdir == 2) //FramegrabBase::PROBEDIRECTION::YTOP2BOTTOM
		//		{
		//			Correct_Spt->SetValues(Spt->X, Spt->Y - Pradius, Spt->Z, Spt->R, Spt->Pdir);
		//		}
		//		else if(Spt->Pdir == 3) // FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP
		//		{
		//			Correct_Spt->SetValues(Spt->X, Spt->Y + Pradius, Spt->Z, Spt->R, Spt->Pdir);
		//		}
		//		else if(Spt->Pdir == 4) // ZTOPTOBOTTOM
		//		{
		//			Correct_Spt->SetValues(Spt->X, Spt->Y, Spt->Z - Pradius, Spt->R, Spt->Pdir);
		//		}
		//		else if(Spt->Pdir == 5)
		//		{
		//			Correct_Spt->SetValues(Spt->X, Spt->Y, Spt->Z + Pradius, Spt->R, Spt->Pdir);
		//		}
		//		Item++;
		//	}
		//}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0014", __FILE__, __FUNCSIG__);}
}

void RPoint::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(PointType == RPOINTTYPE::INTERSECTIONPOINT3D)
		{
			updateIntersectionPoint();
			return;
		}
		else if(this->PointType == RPOINTTYPE::PROJECTEDPOINT3D)
		{
			UpdateBestFit();
			return;
		}
		else if(this->PointType == RPOINTTYPE::RELATIVEPOINT3D)
		{
			updateRelativePoint();
			return;
		}
		if(PPCALCOBJECT->IsPartProgramRunning())return;
		if(PointType == RPOINTTYPE::POINT || PointType == RPOINTTYPE::POINT3D)
		{
			for each(RAction* Caction in PointAtionList)
			{
				if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					AddPointAction* CptAction = (AddPointAction*)Caction;
					if(CptAction->getFramegrab()->CurrentWindowNo == 1)
					{
						SinglePoint* Spt = this->PointsList->getList()[(int)(*CptAction->getFramegrab()->PointActionIdList.begin())];
						Spt->SetValues(CptAction->getFramegrab()->Pointer_SnapPt->getX(), CptAction->getFramegrab()->Pointer_SnapPt->getY(), CptAction->getFramegrab()->Pointer_SnapPt->getZ());
						Spt = this->PointsListOriginal->getList()[(int)(*CptAction->getFramegrab()->PointActionIdList.begin())];
						Spt->SetValues(CptAction->getFramegrab()->Pointer_SnapPt->getX(), CptAction->getFramegrab()->Pointer_SnapPt->getY(), CptAction->getFramegrab()->Pointer_SnapPt->getZ());
					}
				}
			}
			this->UpdateShape();
		}
		else
			updateDerivedPoints();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0006", __FILE__, __FUNCSIG__); }
}

Vector* RPoint::getPosition()
{
	return &position;
}

void RPoint::SetProbeCorrectionShape(Shape* Shp)
{
	try
	{
		ProbeCorrectionShape = Shp;
		this->addParent(ProbeCorrectionShape);
		ProbeCorrectionShape->addChild(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0006a", __FILE__, __FUNCSIG__); }
}

Shape* RPoint::GetProbeCorrectionShape()
{
	return ProbeCorrectionShape;
}

void RPoint::setpoint(Vector d)
{
	position = d;
}

Shape* RPoint::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("P");
		else
			myname = _T("dP");
		RPoint* CShape = new RPoint((TCHAR*)myname.c_str(), RapidEnums::SHAPETYPE::RPOINT);
		if(copyOriginalProperty)
			CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0007", __FILE__, __FUNCSIG__); return NULL; }
}

void RPoint::Transform(double* transform)
{
	Vector temp = MAINDllOBJECT->TransformMultiply(transform,getPosition()->getX(),getPosition()->getY(),getPosition()->getZ());
	setpoint(Vector(temp.getX(), temp.getY(), temp.getZ()));
	this->UpdateEnclosedRectangle();
}

void RPoint::UpdateEnclosedRectangle()
{
	try
	{
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0}, temp_pts[3] = {position.getX(), position.getY(), position.getZ()};					
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);			
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_pts, Order2, &ShapeLeftTop[0]);
			ShapeRightBottom[0] = ShapeLeftTop[0]; ShapeRightBottom[1] = ShapeLeftTop[1]; ShapeRightBottom[2] = ShapeLeftTop[2];
		}
		else
		{
			position.FillDoublePointer(&ShapeLeftTop[0]); 
			position.FillDoublePointer(&ShapeRightBottom[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0008", __FILE__, __FUNCSIG__);}
}

void RPoint::SetOffsets(double x, double y, double z)
{
	try
	{
		RelativeOffsetX = x;
		RelativeOffsetY = y;
		RelativeOffsetZ = z;
	}
	catch(...){}
}


void RPoint::GetShapeCenter(double *cPoint)
{
	cPoint[0] = position.getX();
	cPoint[1] = position.getY();
	cPoint[2] = position.getZ();
}

Shape* RPoint::CreateDummyCopy()
{
	try
	{
		RPoint* CShape = new RPoint(false, this->ShapeType);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0008", __FILE__, __FUNCSIG__); return NULL; }
}

void RPoint::CopyShapeParameters(Shape* s)
{
	try
	{
		this->PointType = ((RPoint*)s)->PointType;
		if(this->getMCSParentShape() != NULL)
		{
			if((this->PointType == RPoint::RPOINTTYPE::INTERSECTIONPOINT3D) || (this->PointType == RPoint::RPOINTTYPE::RELATIVEPOINT3D) || (this->PointType == RPoint::RPOINTTYPE::PROJECTEDPOINT3D))
				this->PointType = RPoint::RPOINTTYPE::POINT3D;
		}
		this->position.set(*((RPoint*)s)->getPosition());
		if(this->PointType == RPoint::RPOINTTYPE::RELATIVEPOINT3D)
		{
			this->RelativeOffsetX = ((RPoint*)s)->RelativeOffsetX;
			this->RelativeOffsetY = ((RPoint*)s)->RelativeOffsetY;
			this->RelativeOffsetZ = ((RPoint*)s)->RelativeOffsetZ;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0009", __FILE__, __FUNCSIG__); }
}

int RPoint::shapenumber = 0;
int RPoint::DXFshapenumber = 0;
int RPoint::shapenumber3D = 0;
int RPoint::DXFshapenumber3D = 0;
void RPoint::reset()
{
	shapenumber = 0;
	DXFshapenumber = 0;
	shapenumber3D = 0;
	DXFshapenumber3D = 0;
}

wostream& operator<<(wostream& os, RPoint& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "Rpoint" << endl;
		os << "Position:values" << endl << (*static_cast<Vector*>(x.getPosition())) << endl;
		os << "ShapeType:"<< x.ShapeType << endl;		
		os << "PointType:" << x.PointType << endl;
		if(x.ProbeCorrectionShape != NULL)
		{
			os << "ProbeCorrectionShape:" << x.ProbeCorrectionShape->getId() << endl;
		}
		if(x.PointType == RPoint::RPOINTTYPE::PROJECTEDPOINT3D && x.ParentPoint != NULL)
			os << "ParentPoint:values" << endl << (*static_cast<Vector*>(x.ParentPoint)) << endl;
		if(x.PointType == RPoint::RPOINTTYPE::RELATIVEPOINT3D)
		{
			os << "RelativeOffsetX:" << x.RelativeOffsetX << endl;
			os << "RelativeOffsetY:" << x.RelativeOffsetY << endl;
			os << "RelativeOffsetZ:" << x.RelativeOffsetZ << endl;
		}
		os << "EndRpoint" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, RPoint& x )
{
	try
	{		
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			is >> (*(Shape*)&x);
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"Rpoint")
			{
			x.ShapeType = RapidEnums::SHAPETYPE::RPOINT;
			while(Tagname!=L"EndRpoint")
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
						if(Tagname==L"Position")
						{
							is >> (*static_cast<Vector*>(&x.position));				
						}
						else if(Tagname==L"ShapeType")
						{
							x.ShapeType = RapidEnums::SHAPETYPE(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"ProbeCorrectionShape")
						{
							int id = atoi((const char*)(Val).c_str());
							x.ProbeCorrectionShape = (Shape*)(MAINDllOBJECT->getUCS(x.UcsId())->getShapes().getList()[id]);
						}
						else if(Tagname==L"PointType")
						{
							x.PointType = RPoint::RPOINTTYPE(atoi((const char*)(Val).c_str()));	
							if(x.PointType ==  RPoint::RPOINTTYPE::RELATIVEPOINT3D)
							{
								x.ShapeType = RapidEnums::SHAPETYPE::RELATIVEPOINT;
							}
						}
						else if(Tagname==L"ParentPoint")
						{
							Vector tmpV;
							is >> (*static_cast<Vector*>(&tmpV));
							x.ParentPoint = MAINDllOBJECT->getVectorPointer(&tmpV);				
						}
						else if(Tagname==L"RelativeOffsetX")
						{
							x.RelativeOffsetX= atof((const char*)(Val).c_str());
						}
						else if(Tagname==L"RelativeOffsetY")
						{
							x.RelativeOffsetY= atof((const char*)(Val).c_str());
						}
						else if(Tagname==L"RelativeOffsetZ")
						{
							x.RelativeOffsetZ= atof((const char*)(Val).c_str());							
						}
					}
				}			
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, RPoint& x)
{
	try
	{
		int n;
		wstring flagStr;
		is >> (*(Shape*)&x);
		x.ShapeType = RapidEnums::SHAPETYPE::RPOINT;
		is >> (*static_cast<Vector*>(&x.position));
		is >> flagStr;
		std::string Str = RMATH2DOBJECT->WStringToString(flagStr);
		if(Str == "ShapeType")
		{
			is >> n;		
			x.ShapeType = RapidEnums::SHAPETYPE(n);
			is >> n; x.PointType = RPoint::RPOINTTYPE(n);
		}
		else
		{
			x.ShapeType = RapidEnums::SHAPETYPE::RPOINT;
			x.PointType = RPoint::RPOINTTYPE(atoi(Str.c_str()));
		}
		if(x.PointType == RPoint::RPOINTTYPE::PROJECTEDPOINT3D)
		{
			Vector tmpV;
			is >> (*static_cast<Vector*>(&tmpV));
			x.ParentPoint = MAINDllOBJECT->getVectorPointer(&tmpV);
		}
		if(x.PointType ==  RPoint::RPOINTTYPE::RELATIVEPOINT3D)
		{
			is >> x.RelativeOffsetX;
			is >> x.RelativeOffsetY;
			is >> x.RelativeOffsetZ;
			x.ShapeType = RapidEnums::SHAPETYPE::RELATIVEPOINT;
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0011", __FILE__, __FUNCSIG__); }
}
