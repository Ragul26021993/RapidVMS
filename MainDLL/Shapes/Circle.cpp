#include "StdAfx.h"
#include "Circle.h"
#include "Plane.h"
#include "TwoArc.h"
#include "PCDCircle.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

Circle::Circle(TCHAR* myname, RapidEnums::SHAPETYPE Stype):ShapeWithList(genName(myname, Stype))
{
	try
	{
		/*this->ShapeType = Stype;
		if(this->ShapeType ==  RapidEnums::SHAPETYPE::CIRCLE)
			this->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
		else if(this->ShapeType ==  RapidEnums::SHAPETYPE::CIRCLE3D)
			this->CircleType = RapidEnums::CIRCLETYPE::CIRCLE3DCIR;
		else if(this->ShapeType ==  RapidEnums::SHAPETYPE::ARC3D)
			this->CircleType = RapidEnums::CIRCLETYPE::ARC3DCIR;
		else if(this->ShapeType ==  RapidEnums::SHAPETYPE::ARC)
			this->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;*/
		init(Stype);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0001", __FILE__, __FUNCSIG__);}
}

Circle::Circle(TCHAR* myname, bool ForPcd,bool is3D):ShapeWithList(myname)
{
	try
	{
		if(is3D)this->ShapeType = RapidEnums::SHAPETYPE::CIRCLE3D;
		else this->ShapeType = RapidEnums::SHAPETYPE::CIRCLE;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0002", __FILE__, __FUNCSIG__); }
}

Circle::Circle(bool simply, RapidEnums::SHAPETYPE Stype):ShapeWithList(false)
{
	try
	{
		this->ShapeType = Stype;
		if(this->ShapeType ==  RapidEnums::SHAPETYPE::CIRCLE)
			this->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
		init(Stype);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0003", __FILE__, __FUNCSIG__); }
}

Circle::~Circle()
{
}

void Circle::init(){}
void Circle::init(RapidEnums::SHAPETYPE Stype)
{
	try
	{
		this->ShapeType = Stype;
		if (ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
			this->ShapeAs3DShape(true);
		this->added_line_arc_line_pt = false;
		this->Radius(0); this->Startangle(0); this->Sweepangle(0); this->OriginalRadius(0); this->BuildRadius(0);
		this->MinInscribedia(0); this->MaxCircumdia(0); this->Circularity(0); this->Length(0);
		this->IdCircle(true); this->SetAsCriticalShape(false);
		this->dir_l(0); this->dir_m(0); this->dir_n(1);
		this->OffsetDefined = false;
		this->Offset(0);
		VPointer = new Vector(0, 0, 0);
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
				this->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->endpoint1), 0), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->endpoint2), 1), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2), 2);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center), 3), 3);
				MAINDllOBJECT->SetAndRaiseErrorMessage("Circle", __FILE__, __FUNCSIG__);
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				this->CircleType = RapidEnums::CIRCLETYPE::CIRCLE3DCIR;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->endpoint1), 0), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->endpoint2), 1), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2), 2);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center), 3), 3);
				MAINDllOBJECT->SetAndRaiseErrorMessage("Circle3D", __FILE__, __FUNCSIG__);
				break;
			case RapidEnums::SHAPETYPE::ARC:
				this->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->endpoint1), 0), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->endpoint2), 1), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2), 2);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center), 3), 3);
				break;
		}
		/*if(this->ShapeType ==  RapidEnums::SHAPETYPE::CIRCLE3D)
			this->ShapeAs3DShape(true);*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0004", __FILE__, __FUNCSIG__); }
}

TCHAR* Circle::genName(const TCHAR* prefix, RapidEnums::SHAPETYPE Stype)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10]; int Cnt;
		if(strcmp((const char*)prefix, "d") == 0)
		{
			if(Stype == RapidEnums::SHAPETYPE::ARC)
				Cnt = ADXFshapenumber++;
			else
				Cnt = CDXFshapenumber++;
		}
		else
		{
			if(Stype == RapidEnums::SHAPETYPE::ARC) 
				Cnt = Ashapenumber++;
			else
				Cnt = Cshapenumber++;
		}
		Cnt++;
		_itot_s(Cnt, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0005", __FILE__, __FUNCSIG__); return name; }
}

void Circle::setCenter(Vector& t)
{
	center.set(t);
}

void Circle::setendpoint1(Vector& t)
{
	endpoint1.set(t);
}

void Circle::setendpoint2(Vector& t)
{
	endpoint2.set(t);
}

void Circle::setMidPoint(Vector& t)
{
	Midpoint.set(t);
}

Vector* Circle::getCenter()
{
	return &(this->center);
}

Vector* Circle::getendpoint1()
{
	return &(this->endpoint1);
}

Vector* Circle::getendpoint2()
{
	return &(this->endpoint2);
}

Vector* Circle::getMidPoint()
{
	return &(this->Midpoint);
}

void Circle::setCircleParameters(double l, double m, double n, double radius)
{
	dir_l(l); dir_m(m); dir_n(n); Radius(radius);
}

void Circle::getParameters(double* circleparam)
{
	center.FillDoublePointer(circleparam, 3);
	circleparam[3] = dir_l(); circleparam[4] = dir_m(); circleparam[5] = dir_n(); circleparam[6] = Radius();
}

void Circle::getPlaneParameters(double* circlepln)
{
	circlepln[0] = dir_l(); circlepln[1] = dir_m(); circlepln[2] = dir_n(); circlepln[3] = (center.getX() * dir_l() +  center.getY() * dir_m() + center.getZ() * dir_n());
}

bool Circle::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		double mp[2] = {corner1.getX() ,corner1.getY()}, cen[2] = {center.getX(),center.getY()},
			endp1[2] = {endpoint1.getX(), endpoint1.getY()}, endp2[2] = {endpoint2.getX(), endpoint2.getY()};
		bool flag = false, flag1 = false;
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
				flag = RMATH2DOBJECT->MousePt_OnCircle(&cen[0], Radius(), &mp[0], Tolerance);
				break;
			case RapidEnums::SHAPETYPE::ARC:
				flag = RMATH2DOBJECT->MousePt_OnArc(&cen[0], Radius(), Startangle(), Sweepangle(), &endp1[0], &endp2[0], Tolerance, &mp[0], &flag1);
				MAINDllOBJECT->ShapeHighlightedForImplicitSnap(flag1);
				if(MAINDllOBJECT->ShapeHighlightedForImplicitSnap() && MAINDllOBJECT->ShowImplicitIntersectionPoints()) return true;
				else MAINDllOBJECT->ShapeHighlightedForImplicitSnap(false);
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
				flag = Shape_IsInWindow(&SelectionL[0], Tolerance);
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0006", __FILE__, __FUNCSIG__); return false; }
}

bool Circle::Shape_IsInWindow(Vector& corner1,double Tolerance, double* TransformM)
{
	try
	{
		double temp1[4] = {center.getX(), center.getY(), center.getZ(), 1}, tempCen[4], temp2[4] = {Midpoint.getX(), Midpoint.getY(), Midpoint.getZ(), 1}, tempmidpt[4];
		double tmprad, tmpstrtang, tmpsweepang;
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		double mp[2] = {corner1.getX() ,corner1.getY()}, endp1[4] = {endpoint1.getX(), endpoint1.getY(), endpoint1.getZ(), 1},  tempendpnt1[4],
		endp2[4] = {endpoint2.getX(), endpoint2.getY(), endpoint2.getZ(), 1}, tempendpnt2[4];
		RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, endp1, Order2, tempendpnt1);
		RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, endp2, Order2, tempendpnt2);
		RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp2, Order2, tempmidpt);
		RMATH2DOBJECT->Arc_3Pt(tempendpnt1, tempendpnt2, tempmidpt, tempCen, &tmprad, &tmpstrtang, &tmpsweepang);
		RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp1, Order2, tempCen);
		bool flag = false, flag1 = false;
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
				flag = RMATH2DOBJECT->MousePt_OnCircle(& tempCen[0], Radius(), &mp[0], Tolerance);
				break;
			case RapidEnums::SHAPETYPE::ARC:
				flag = RMATH2DOBJECT->MousePt_OnArc(&tempCen[0], Radius(), tmpstrtang, tmpsweepang, &tempendpnt1[0], &tempendpnt2[0], Tolerance, &mp[0], &flag1);
				MAINDllOBJECT->ShapeHighlightedForImplicitSnap(flag1);
				if(MAINDllOBJECT->ShapeHighlightedForImplicitSnap() && MAINDllOBJECT->ShowImplicitIntersectionPoints()) return true;
				else MAINDllOBJECT->ShapeHighlightedForImplicitSnap(false);
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
				flag = Shape_IsInWindow(&SelectionL[0], Tolerance);
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0006", __FILE__, __FUNCSIG__); return false; }
}

bool Circle::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	try
	{
		double p1[2] = {corner1.getX(),corner1.getY()}, p2[2] = {corner2.getX(),corner1.getY()}, p3[2] = {corner2.getX(),corner2.getY()}, p4[2] = {corner1.getX(),corner2.getY()};
		double cen[2] = {center.getX(), center.getY()}, endp1[2] = {endpoint1.getX(),endpoint1.getY()}, endp2[2] = {endpoint2.getX(),endpoint2.getY()};
		bool flag = false;
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
				flag = RMATH2DOBJECT->Circle_isinwindow(&cen[0], Radius(), &p1[0], &p2[0], &p3[0], &p4[0]);
				break;
			case RapidEnums::SHAPETYPE::ARC:
				flag = RMATH2DOBJECT->arc_isinwindow(&cen[0], Radius(), Startangle(), Sweepangle(), &endp1[0], &endp2[0], &p1[0], &p2[0], &p3[0], &p4[0]);
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0007", __FILE__, __FUNCSIG__); return false; }
}

bool Circle::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		double centerPt[3] = {center.getX(),center.getY(),center.getZ()};
		//get the 4th parameter of circle's plane as D
		double D = dir_l() * center.getX() + dir_m() * center.getY() + dir_n() * center.getZ();
		double PlaneOfArc[4] = {dir_l(), dir_m(), dir_n(), D}, Tempangle;
		IntersectnPts[2] = 0;
		Intersects = RMATH3DOBJECT->Intersection_Line_Plane(SelectionLine, PlaneOfArc, IntersectnPts);
		bool flag = false;
		if(Intersects == 1)
		{
			double dist = RMATH3DOBJECT->Distance_Point_Point(centerPt, IntersectnPts);
			if(dist < Radius() + Tolerance && dist > Radius() - Tolerance)
			{
				switch(this->ShapeType)
				{
					case RapidEnums::SHAPETYPE::CIRCLE:
					case RapidEnums::SHAPETYPE::CIRCLE3D:
						flag = true;
						break;
					case RapidEnums::SHAPETYPE::ARC:
						Tempangle = RMATH2DOBJECT->ray_angle(centerPt[0], centerPt[1], IntersectnPts[0], IntersectnPts[1]);
						if(Tempangle >= Startangle() && Tempangle <= Startangle() + Sweepangle())
							flag = true;
						break;
				}
			}				
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0008", __FILE__, __FUNCSIG__); return false; }
}

void Circle::UpdateBestFit()
{
	try
	{
		if (this->ShapeType != RapidEnums::SHAPETYPE::CIRCLE3D && !this->IsDxfShape()) {
			MAINDllOBJECT->SetAndRaiseErrorMessage("UpdateBestFit1", __FILE__, __FUNCSIG__);
			this->SetAsCriticalShape(MAINDllOBJECT->CriticalShapeStatus);
		}
			
		if (/*CircleType == RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER ||*/ CircleType == RapidEnums::CIRCLETYPE::TWOPTCIRCLE || CircleType == RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE || CircleType == RapidEnums::CIRCLETYPE::LINEARCLINE_CIRCLE ||
			CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT || CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE || CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE || CircleType == RapidEnums::CIRCLETYPE::CIRCLE3DCIR || CircleType == RapidEnums::CIRCLETYPE::ARC_TANGENT_2_LINES_CIRCLE
			|| CircleType == RapidEnums::CIRCLETYPE::ARC_ARC_SIBLINGS || CircleType == RapidEnums::CIRCLETYPE::ARC_TAN_TO_ARC || CircleType == RapidEnums::CIRCLETYPE::ARC_TAN_TO_LINE || CircleType == RapidEnums::CIRCLETYPE::CIRCLEFROMCLOUDPNTS)
		{
			MAINDllOBJECT->SetAndRaiseErrorMessage("UpdateBestFit2", __FILE__, __FUNCSIG__);
			updateNormalCircles();
		}
			
		else if(CircleType == RapidEnums::CIRCLETYPE::ARC_INTWOARC && this->getGroupParent().size() > 0)
		{
		   ShapeWithList* Cshape = (ShapeWithList*)(*(this->getGroupParent().begin()));
		   ((TwoArc *)Cshape)->UpdateForSubshapeChange(this);
		}
		else if (CircleType == RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER) {
			MAINDllOBJECT->SetAndRaiseErrorMessage("UpdateBestFit3", __FILE__, __FUNCSIG__);
			updateProjectedCircle(3);
		}
		else
			updateDerivedCircles();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

 bool Circle::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
 {
	 try
	 {
		 int s1[2] = {4, 4}, s2[2] = {4, 1}, Order1[2] = {3, 3}, Order2[2] = {3, 1};
		 double CirParam[7] = {0}, TransMatrix[9] = {0};
		 getParameters(&CirParam[0]);
		 double cent[4] = {center.getX(), center.getY(), center.getZ(), 1}, new_cen[4] = {0};
		 RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &cent[0], &s2[0], &new_cen[0]);
		 switch(this->ShapeType)
		 {
		 case RapidEnums::SHAPETYPE::CIRCLE:
		 case RapidEnums::SHAPETYPE::CIRCLE3D:			
			 RMATH2DOBJECT->RectangleEnclosing_Circle(&new_cen[0], Radius(), &Lefttop[0], &Rightbottom[0]);
			 break;
		 case RapidEnums::SHAPETYPE::ARC:								
		 case RapidEnums::SHAPETYPE::ARC3D:
			 {
				 double  temp_endp1[4] = {0}, temp_endp2[4] = {0}, newDcs[4] = {0}, endpt1[4] = {endpoint1.getX(), endpoint1.getY(), endpoint1.getZ(), 1},
					 endpt2[4] = {endpoint2.getX(), endpoint2.getY(), endpoint2.getZ(), 1}, CircleDc[4] = {CirParam[3], CirParam[4], CirParam[5], 1};
				 RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &endpt1[0], &s2[0], &temp_endp1[0]);
				 RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &endpt2[0], &s2[0], &temp_endp2[0]);					
				 RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &CircleDc[0], &s2[0], &newDcs[0]);
				 HELPEROBJECT->RectangleEnclosing_Arcfor3DMode(&newDcs[0], &new_cen[0], &temp_endp1[0], &temp_endp2[0], Radius(), Startangle(), Startangle() + Sweepangle(), &Lefttop[0], &Rightbottom[0]);			
			 }
			 break;
		 }
		 return true;
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); return false; }
}

void Circle::updateNormalCircles()
{
	try
	{
		int totcount = this->PointsListOriginal->Pointscount();
		switch(CircleType)
		{
			case RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER:
				updateCircleWithCenter(totcount);
				break;
			case RapidEnums::CIRCLETYPE::TWOPTCIRCLE:
			case RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE:
				update2Pt3PtCircle(totcount);
				break;
			case RapidEnums::CIRCLETYPE::CIRCLE3DCIR:
			case RapidEnums::CIRCLETYPE::CIRCLEFROMCLOUDPNTS:
				update3DCircle(totcount);
				break;
			case RapidEnums::CIRCLETYPE::PCDCIRCLE:
				((PCDCircle*)this)->CalculatePcdCircle();
				break;
			case RapidEnums::CIRCLETYPE::ARC_ENDPOINT:
			case RapidEnums::CIRCLETYPE::ARC_MIDDLE:
			case RapidEnums::CIRCLETYPE::LINEARCLINE_CIRCLE:
			case RapidEnums::CIRCLETYPE::ARC_TANGENT_2_LINES_CIRCLE:
				if(ShapeAsfasttrace())
					updateFtFgArc(totcount);
				else
					updateArc(totcount);
				break;
			case RapidEnums::CIRCLETYPE::ARC_ARC_SIBLINGS:
				updateSiblingArcs(totcount);
				break;
			case RapidEnums::CIRCLETYPE::ARC_TAN_TO_ARC:
				updateArcTanToArc(totcount);
				break;
			case RapidEnums::CIRCLETYPE::ARC_TAN_TO_LINE:
				updateArcTanToLine(totcount);
				break;
			case RapidEnums::CIRCLETYPE::PROJECTEDCIRCLE:
				updateProjectedCircle(totcount);
				break;
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateDerivedCircles()
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);
		int totcount = this->PointsListOriginal->Pointscount();
		if(totcount < 1) return;
		if(totcount == 1)
			updateDerivedCircleAcc_Type();
		else
			CalculateCircleBestFit(totcount);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateDerivedCircleAcc_Type()
{
	try
	{
		bool flag = false;
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			for each(RAction* Caction in PointAtionList)
			{
				AddPointAction* CptAction = (AddPointAction*)Caction;
				if(CptAction->getFramegrab()->CurrentWindowNo == 1)
				{
					if(CptAction->getFramegrab()->Pointer_SnapPt != NULL)
					{
						flag = true;
						SinglePoint* Spt = this->PointsList->getList()[(int)(*CptAction->getFramegrab()->PointActionIdList.begin())];
						Spt->SetValues(CptAction->getFramegrab()->Pointer_SnapPt->getX(), CptAction->getFramegrab()->Pointer_SnapPt->getY(), CptAction->getFramegrab()->Pointer_SnapPt->getZ());
						Spt = this->PointsListOriginal->getList()[(int)(*CptAction->getFramegrab()->PointActionIdList.begin())];
						Spt->SetValues(CptAction->getFramegrab()->Pointer_SnapPt->getX(), CptAction->getFramegrab()->Pointer_SnapPt->getY(), CptAction->getFramegrab()->Pointer_SnapPt->getZ());
					}
				}
			}
		}
		else
		{
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if(fg == NULL) return;
			if(fg->CurrentWindowNo == 1 && fg->Pointer_SnapPt != NULL) flag = true;
			//added logic on May 17 to handle derived shape update in PP
			for(RC_ITER curraction = PPCALCOBJECT->getPPActionlist().getList().begin(); curraction != PPCALCOBJECT->getPPActionlist().getList().end(); curraction++)
			{
				RAction* action = (RAction*)(*curraction).second;
				if(!action->ActionStatus()) continue;
				if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					if(this->getId() != ((AddPointAction*)action)->getShape()->getId())
						continue;
					
					if(((AddPointAction*)action)->getFramegrab()->CurrentWindowNo == 1)
					{
						if(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt != NULL)
						{
							flag = true;
							SinglePoint* Spt = this->PointsList->getList()[(int)(*(((AddPointAction*)action)->getFramegrab()->PointActionIdList.begin()))];
							Spt->SetValues(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getX(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getY(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getZ());
							Spt = this->PointsListOriginal->getList()[(int)(*(((AddPointAction*)action)->getFramegrab()->PointActionIdList.begin()))];
							Spt->SetValues(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getX(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getY(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getZ());
							break;
						}
					}
				}
			}
		}
		SinglePoint* Spt = (SinglePoint*)(*PointsListOriginal->getList().begin()).second;
		double MousePt[2] = {Spt->X, Spt->Y};
		if(CircleType == RapidEnums::CIRCLETYPE::PARALLELARCCIR)
		{
			updateParallelArc(&MousePt[0], flag);
		}
		else
		{
			if(CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_2CIRCLES_IN)
				updateTanCircleto2CirclesIn(&MousePt[0], flag);
			else if(CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_2CIRCLES_OUT)
				updateTanCircleto2CirclesOut(&MousePt[0],flag);
			else if(CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_CIRCLE)
				updateTanCircletoCircle(&MousePt[0], flag);
			else if(CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_2LINES)
				updateTanCircleto2Lines(&MousePt[0], flag);
			else if(CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_LINECIRCLE)
				updateTanCircletoLineCircle(&MousePt[0], flag);
			else if(CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_TRIANG_IN)
				updateCircleInsideTriangle();
			else if(CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_TRIANG_OUT)
				updateCircleOutSideTriangle();
		}
		calculateAttributes();
		this->IsValid(true);
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__);
	}
}

void Circle::updateCircleWithCenter(int PointsCnt)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);
		if(PointsCnt < 1) return;
		SinglePoint* Spt = (*PointsList->getList().begin()).second;
		center.set(Vector(Spt->X, Spt->Y, Spt->Z));
		if (PointsCnt == 1)
		{
			double rad = Radius();
			Radius(rad);
		}
		else if (PointsCnt == 2)
		{
			Radius(RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], pts[2], pts[3]));	
		}
		else
		{
			double answer[3] = {Spt->X, Spt->Y, 0};
			if (!BESTFITOBJECT->CircleWithCenter_BestFit(pts + 2, PointsCnt, answer, MAINDllOBJECT->OutlierFilteringForLineArc()))
			{
				if(Radius() > 0)
					this->IsValid(true);
				else 
					this->IsValid(false);
				return;
			}
			Radius(answer[2]);
		}
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::update2Pt3PtCircle(int PointsCnt)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);
		if(PointsCnt < 2) return;
		else if(CircleType == RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE && PointsCnt < 3) return;
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);

		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		filterptsCnt = 0;

		int n = 0, j = 0;
		for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			this->pts[n++] = Pt->X;
			this->pts[n++] = Pt->Y;
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			filterptsCnt++;
		}
		CalculateCircleBestFit(PointsCnt);
		ProbePointCalculations(PointsCnt);
		this->calculateAttributes();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0010", __FILE__, __FUNCSIG__); }
}

void Circle::update3DCircle(int PointsCnt)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);
		if(PointsCnt < 3) return;
		double ans[7] = {0};
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);

		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		filterptsCnt = 0;
		int n = 0, j = 0;
		for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			this->pts[n++] = Pt->X;
			this->pts[n++] = Pt->Y;
			this->pts[n++] = Pt->Z;
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			this->filterpts[j++] = Pt->Z;
			filterptsCnt++;
		}
		if (!BESTFITOBJECT->ThreeDCircle_BestFit(pts, PointsCnt, &ans[0]))
		{
			if(Radius() > 0)
				this->IsValid(true);
			else 
				this->IsValid(false);
			return;
		}
		if(this->ApplyFilterCorrection() && PointsCnt > 3)
		{
			bool flag = false;
			int cnt = PointsCnt;
			for(int i = 0; i < this->FilterCorrectionIterationCnt(); i++)
			{
				cnt = HELPEROBJECT->FilteredCircle3D(this->PointsList, ans, FilterCorrectionFactor(), i);
				if(cnt < 4) break;
				if(this->filterpts != NULL){ free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				this->filterptsCnt = 0;
				int n = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					if(((*Spt).second)->InValid) continue;
					SinglePoint* Pt = (*Spt).second;
					filterpts[n++] = Pt->X;
					filterpts[n++] = Pt->Y;
					filterpts[n++] = Pt->Z;
					filterptsCnt++;
				}
				flag = BESTFITOBJECT->ThreeDCircle_BestFit(filterpts, filterptsCnt, &ans[0]);
				if (!flag)
				{
					if(Radius() > 0)
						this->IsValid(true);
					else 
						this->IsValid(false);
					return;
				}
			}
		}
		if(ApplyMMCCorrection() && ScanDirDetected() && PointsCnt > 3)
		{
			bool flag = false;
			int cnt = PointsCnt;
			for(int i = 0; i < this->MMCCorrectionIterationCnt(); i++)
			{
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
				cnt = HELPEROBJECT->OutermostInnerMostCircle3D(this->PointsList, ans, useValidPointOnly, OuterCorrection());
				if(cnt < 4) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				this->filterptsCnt = 0;
				int n = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					if(((*Spt).second)->InValid) continue;
					SinglePoint* Pt = (*Spt).second;
					filterpts[n++] = Pt->X;
					filterpts[n++] = Pt->Y;
					filterpts[n++] = Pt->Z;
					filterptsCnt++;
				}
				flag = BESTFITOBJECT->ThreeDCircle_BestFit(filterpts, filterptsCnt, &ans[0]);
				if (!flag)
				{
					if(Radius() > 0)
						this->IsValid(true);
					else 
						this->IsValid(false);
					return;
				}
			}
		}
		this->center.set(ans[0], ans[1], ans[2]);
		this->Radius(ans[6]); 
		this->dir_l(ans[3]); this->dir_m(ans[4]); this->dir_n(ans[5]);
		ProbePointCalculations(PointsCnt);
		this->calculateAttributes();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateArc3D(int PointsCnt)
{
	try
	{
		if(PointsCnt < 3) return;
		double endpts[4], angles[2], ans[7] = {0};
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		
		if(this->filterpts != NULL){ free(filterpts); filterpts = NULL;}
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		filterptsCnt = 0;
		int n = 0, j = 0;
		for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			this->pts[n++] = Pt->X;
			this->pts[n++] = Pt->Y;
			this->pts[n++] = Pt->Z;
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			this->filterpts[j++] = Pt->Z;
			filterptsCnt++;
		}
		if (!BESTFITOBJECT->ThreeDCircle_BestFit(pts, PointsCnt, &ans[0]))
		{
			if(Radius() > 0)
				this->IsValid(true);
			else 
				this->IsValid(false);
			return;
		}
		this->center.set(ans[0], ans[1], ans[2]);
		this->Radius(ans[6]); 
		this->dir_l(ans[3]); this->dir_m(ans[4]); this->dir_n(ans[5]);
		ProbePointCalculations(PointsCnt);
		this->calculateAttributes();
		this->IsValid(true);
		double TransMatrix[9],InverseMatrix[9];
		double MeasurementPlane[4] = {ans[3], ans[4], ans[5], ans[0] * ans[3] + ans[1] * ans[4] + ans[2] * ans[5]};
		void GetTMatrixForPlane(double *Plane, double *TransMatrix);
		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		double* point3Dcol = (double*)malloc(sizeof(double) * PointsCnt * 2);
		double point3D[3];
		double StartPoint3D[3], EndPoint3D[3];
		
		for(int i = 0; i < PointsCnt; i++)
		{
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, pts + 3 * i, Order2, point3D);
			point3Dcol[2 * i] = point3D[0];
			point3Dcol[2 * i + 1] = point3D[1];
		}
		double center3D[3] = {ans[0], ans[1], ans[2]};
		double center2D[2];
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1,center3D, Order2, point3D);
		center2D[0] = point3D[0];
		center2D[1] = point3D[1];
		RMATH2DOBJECT->ArcBestFitAngleCalculation(&point3Dcol[0], PointsCnt, &center2D[0], ans[6], &angles[0]);
		RMATH2DOBJECT->ArcEndPoints(&center2D[0], ans[6], angles[0], angles[1], &endpts[0]);
		double StartPoint2D[3] = {endpts[0],endpts[1],0};
		double EndPoint2D[3] = {endpts[2],endpts[3],0};
		this->Startangle(angles[0]); 
		this->Sweepangle(angles[1]);
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix,3,1,InverseMatrix);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1,StartPoint2D, Order2, StartPoint3D);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1,EndPoint2D, Order2, EndPoint3D);
		this->endpoint1.set(StartPoint3D[0], StartPoint3D[1], StartPoint3D[2]);
		this->endpoint2.set(EndPoint3D[0], EndPoint3D[1], EndPoint3D[2]);
		free(point3Dcol);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0011", __FILE__, __FUNCSIG__); }
}

void Circle::updateArc(int PointsCnt)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);
		if(PointsCnt < 3) return;
		double ans[4] = {0}, endpts[4] = {0}, angles[2] = {0};
		if (CircleType == RapidEnums::CIRCLETYPE::ARC_TANGENT_2_LINES_CIRCLE) 
		{
			updateArcTangentToTwoLines(&PointsCnt, ans);
		}
		else
		{
			if(this->pts != NULL){free(pts); pts = NULL;}
			this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
			if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
			this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
			filterptsCnt = 0;
			int n = 0, j = 0;
			for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
			{
				SinglePoint* Pt = (*Spt).second;
				this->pts[n++] = Pt->X;
				this->pts[n++] = Pt->Y;
				this->filterpts[j++] = Pt->X;
				this->filterpts[j++] = Pt->Y;
				filterptsCnt++;
			}
			if(PointsCnt == 3)
			{
				if ((CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE) || (CircleType == RapidEnums::CIRCLETYPE::ARC_ARC_SIBLINGS) 
					|| (CircleType == RapidEnums::CIRCLETYPE::ARC_TAN_TO_ARC) || (CircleType == RapidEnums::CIRCLETYPE::ARC_TAN_TO_LINE))
					RMATH2DOBJECT->Arc_3Pt(pts, pts + 2, pts + 4, &ans[0], &ans[2], &angles[0], &angles[1]);
				else if((CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT) || (CircleType == RapidEnums::CIRCLETYPE::LINEARCLINE_CIRCLE))
					RMATH2DOBJECT->Arc_3Pt(pts, pts + 4, pts + 2, &ans[0], &ans[2], &angles[0], &angles[1]);
			}
			else
			{
				if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
				{
					if(!BESTFITOBJECT->Circle_BestFit(pts, PointsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
						return;
				}
				else
				{
					if(!BESTFITOBJECT->Circle_BestFit(pts, PointsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
						return;
				}
				RMATH2DOBJECT->ArcBestFitAngleCalculation(pts, PointsCnt, &ans[0], ans[2], &angles[0]);
			}
			if(ApplyFilterCorrection() && PointsCnt > 3)
			{
				int cnt = PointsCnt;
				for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
				{
					double circleparam[4] = {ans[0], ans[1], 0, ans[2]};
					cnt = HELPEROBJECT->FilteredCircle(this->PointsList, circleparam, FilterCorrectionFactor(), i);
					if(cnt < 4) break;
					if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
					this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
					filterptsCnt = 0;
					int n = 0;
					for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
					{
						if(((*Spt).second)->InValid) continue;
						SinglePoint* Pt = (*Spt).second;
						filterpts[n++] = Pt->X;
						filterpts[n++] = Pt->Y;
						filterptsCnt++;
					}
					if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					{
						if(!BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
							return;
					}
					else
					{
						if(!BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
							return;
					}
					RMATH2DOBJECT->ArcBestFitAngleCalculation(filterpts, filterptsCnt, &ans[0], ans[2], &angles[0]);
				}
			}
			if(ApplyMMCCorrection() && ScanDirDetected() && PointsCnt > 3)
			{
				int cnt = PointsCnt;
				for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
				{
					double circleparam[4] = {ans[0], ans[1], 0, ans[2]};
					bool useValidPointOnly = false;
					if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
					cnt = HELPEROBJECT->OutermostInnerMostCircle(this->PointsList, circleparam, useValidPointOnly, OuterCorrection());
					if(cnt < 4) break;
					if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
					this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
					this->filterptsCnt = 0;
					int n = 0;
					for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
					{
						if(((*Spt).second)->InValid) continue;
						SinglePoint* Pt = (*Spt).second;
						filterpts[n++] = Pt->X;
						filterpts[n++] = Pt->Y;
						filterptsCnt++;
					}
					if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					{
						if(!BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
							return;
					}
					else
					{
						if(!BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
							return;
					}
					RMATH2DOBJECT->ArcBestFitAngleCalculation(filterpts, filterptsCnt, &ans[0], ans[2], &angles[0]);
				}
			}
			this->center.set(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->Radius(ans[2]); this->OriginalRadius(ans[2]);
			this->Startangle(angles[0]); this->Sweepangle(angles[1]);
			this->ProbePointCalculations(PointsCnt);
			double ans1[3] = {center.getX(), center.getY(), Radius()};
			RMATH2DOBJECT->ArcEndPoints(&ans1[0], ans1[2], angles[0], angles[1], &endpts[0]);
			this->endpoint1.set(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->endpoint2.set(endpts[2],endpts[3], MAINDllOBJECT->getCurrentDRO().getZ());		
			calculateAttributes();
		}
		this->IsValid(true); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0011", __FILE__, __FUNCSIG__); }
}

void Circle::updateFtFgArc(int PointsCnt)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);
		if(PointsCnt < 3) return;
		if(PointsCnt > 3) CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
		list<BaseItem*>::iterator psh;
		Shape *CShape1 = NULL, *CShape2 = NULL;
		bool start_pt_constraint = false, end_pt_constraint = false;
		Vector Sh1Endp1, Sh1Endp2, Sh2Endp1, Sh2Endp2, StartPoint, Endpoint;
		if(PointsCnt == 3)
		{
			this->IsValid(false);
			double tp[4], ce[2], rad, startang, sweepang;
			if(CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE)
				RMATH2DOBJECT->Arc_3Pt(pts, pts + 2, pts + 4,&ce[0], &rad, &startang, &sweepang);
			else if(CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT)
				RMATH2DOBJECT->Arc_3Pt(pts, pts + 4, pts + 2,&ce[0], &rad, &startang, &sweepang);
			Radius(rad); Startangle(startang); Sweepangle(sweepang);
			RMATH2DOBJECT->ArcEndPoints(&ce[0], Radius(), Startangle(), Sweepangle(), &tp[0]);
			PC_ITER item = this->PointsList->getList().begin();
			SinglePoint* Spt = (*item).second;
			this->endpoint1.set(tp[0], tp[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->endpoint2.set(tp[2], tp[3], MAINDllOBJECT->getCurrentDRO().getZ());
			this->center.set(ce[0], ce[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->calculateAttributes();
			this->IsValid(true);
			return;
		}
		else
		{
			if(this->getFgNearShapes().size() == 0)
			{
				updateArc(PointsCnt);
				return;
			}
			else if(this->getFgNearShapes().size() == 1)
			{
				psh = this->getFgNearShapes().begin();
				CShape1 = (Shape*)(*psh);
				if(CShape1->DoneFrameGrab()) start_pt_constraint = true;
				if(CShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					Sh1Endp1.set(*((Line*)CShape1)->getPoint1());
					Sh1Endp2.set(*((Line*)CShape1)->getPoint2());
				}
				else
				{
					Sh1Endp1.set(*((Circle*)CShape1)->getendpoint1());
					Sh1Endp2.set(*((Circle*)CShape1)->getendpoint2());
				}
				if((Sh1Endp1.operator==(endpoint1)) || (Sh1Endp1.operator==(endpoint2)))
					StartPoint.set(Sh1Endp1);
				else if((Sh1Endp2.operator==(endpoint1)) || (Sh1Endp2.operator==(endpoint2)))
					StartPoint.set(Sh1Endp2);
			}
			else
			{
				psh = this->getFgNearShapes().begin();
				CShape1 = (Shape*)(*psh);
				psh++; CShape2 = (Shape*)(*psh);
				if(CShape1->DoneFrameGrab()) start_pt_constraint = true;
				if(CShape2->DoneFrameGrab()) end_pt_constraint = true;
		
				if(CShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					Sh1Endp1.set(*((Line*)CShape1)->getPoint1());
					Sh1Endp2.set(*((Line*)CShape1)->getPoint2());
				}
				else
				{
					Sh1Endp1.set(*((Circle*)CShape1)->getendpoint1());
					Sh1Endp2.set(*((Circle*)CShape1)->getendpoint2());
				}
				if(CShape2->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					Sh2Endp1.set(*((Line*)CShape2)->getPoint1());
					Sh2Endp2.set(*((Line*)CShape2)->getPoint2());
				}
				else
				{
					Sh2Endp1.set(*((Circle*)CShape2)->getendpoint1());
					Sh2Endp2.set(*((Circle*)CShape2)->getendpoint2());
				}
				if((Sh1Endp1.operator==(endpoint1)) || (Sh1Endp1.operator==(endpoint2)))
					StartPoint.set(Sh1Endp1);
				else if((Sh1Endp2.operator==(endpoint1)) || (Sh1Endp2.operator==(endpoint2)))
					StartPoint.set(Sh1Endp2);
				if((Sh2Endp1.operator==(endpoint1)) || (Sh2Endp1.operator==(endpoint2)))
					Endpoint.set(Sh2Endp1);
				else if((Sh2Endp2.operator==(endpoint1)) || (Sh2Endp2.operator==(endpoint2)))
					Endpoint.set(Sh2Endp2);
			}
		}
		if(!start_pt_constraint && !end_pt_constraint)
		{
			PointCollection PtColl1, PtColl2;
			Vector Shape2NearPt, Shape1NearPt;
			if(this->getFgNearShapes().size() == 1)
			{
				if(StartPoint.operator==(endpoint1))
				{
					if(endpoint1.operator==(Sh1Endp1))
					{
						PtColl2.Addpoint(new SinglePoint(Sh1Endp2.getX(), Sh1Endp2.getY(), Sh1Endp2.getZ()));
						Shape2NearPt.set(Sh1Endp1);
					}
					else
					{
						PtColl2.Addpoint(new SinglePoint(Sh1Endp1.getX(), Sh1Endp1.getY(), Sh1Endp1.getZ()));
						Shape2NearPt.set(Sh1Endp2);
					}
					if(CShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						Vector* Cmidpt = ((Circle*)CShape1)->getMidPoint();
						PtColl2.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
						((Circle*)CShape1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}
				}
				else
				{
					if(endpoint2.operator==(Sh1Endp1))
					{
						PtColl2.Addpoint(new SinglePoint(Sh1Endp2.getX(), Sh1Endp2.getY(), Sh1Endp2.getZ()));
						Shape2NearPt.set(Sh1Endp1);
					}
					else
					{
						PtColl2.Addpoint(new SinglePoint(Sh1Endp1.getX(), Sh1Endp1.getY(), Sh1Endp1.getZ()));
						Shape2NearPt.set(Sh1Endp2);
					}
					if(CShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						Vector* Cmidpt = ((Circle*)CShape1)->getMidPoint();
						PtColl2.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
						((Circle*)CShape1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}
				}
			}
			else
			{
				if(StartPoint.operator==(endpoint1))
				{
					if(endpoint2.operator==(Sh2Endp1))
					{
						PtColl2.Addpoint(new SinglePoint(Sh2Endp2.getX(), Sh2Endp2.getY(), Sh2Endp2.getZ()));
						Shape2NearPt.set(Sh2Endp1);
					}
					else
					{
						PtColl2.Addpoint(new SinglePoint(Sh2Endp1.getX(), Sh2Endp1.getY(), Sh2Endp1.getZ()));
						Shape2NearPt.set(Sh2Endp2);
					}
					if(CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						Vector* Cmidpt = ((Circle*)CShape2)->getMidPoint();
						PtColl2.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
						((Circle*)CShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}
				}
				else
				{
					if(endpoint1.operator==(Sh2Endp1))
					{
						PtColl2.Addpoint(new SinglePoint(Sh2Endp2.getX(), Sh2Endp2.getY(), Sh2Endp2.getZ()));
						Shape2NearPt.set(Sh2Endp1);
					}
					else
					{
						PtColl2.Addpoint(new SinglePoint(Sh2Endp1.getX(), Sh2Endp1.getY(), Sh2Endp1.getZ()));
						Shape2NearPt.set(Sh2Endp2);
					}
					if(CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						Vector* Cmidpt = ((Circle*)CShape2)->getMidPoint();
						PtColl2.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
						((Circle*)CShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}
				}

				if(Endpoint.operator==(endpoint1))
				{
					if(endpoint2.operator==(Sh1Endp1))
					{
						PtColl1.Addpoint(new SinglePoint(Sh1Endp2.getX(), Sh1Endp2.getY(), Sh1Endp2.getZ()));
						Shape1NearPt.set(Sh1Endp1);
					}
					else
					{
						PtColl1.Addpoint(new SinglePoint(Sh1Endp1.getX(), Sh1Endp1.getY(), Sh1Endp1.getZ()));
						Shape1NearPt.set(Sh1Endp2);
					}
					if(CShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						Vector* Cmidpt = ((Circle*)CShape1)->getMidPoint();
						PtColl1.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
						((Circle*)CShape1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}
				}
				else
				{
					if(endpoint1.operator==(Sh1Endp1))
					{
						PtColl1.Addpoint(new SinglePoint(Sh1Endp2.getX(), Sh1Endp2.getY(), Sh1Endp2.getZ()));
						Shape1NearPt.set(Sh1Endp1);
					}
					else
					{
						PtColl1.Addpoint(new SinglePoint(Sh1Endp1.getX(), Sh1Endp1.getY(), Sh1Endp1.getZ()));
						Shape1NearPt.set(Sh1Endp2);
					}
					if(CShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						Vector* Cmidpt = ((Circle*)CShape1)->getMidPoint();
						PtColl1.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
						((Circle*)CShape1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}
				}
			}

			updateArc(PointsCnt);

			if(this->getFgNearShapes().size() == 1)
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), endpoint1.getX(), endpoint1.getY()) < RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), endpoint2.getX(), endpoint2.getY()))
					PtColl2.Addpoint(new SinglePoint(endpoint1.getX(), endpoint1.getY(), endpoint1.getZ()));
				else
					PtColl2.Addpoint(new SinglePoint(endpoint2.getX(), endpoint2.getY(), endpoint2.getZ()));
				((ShapeWithList*)CShape1)->ResetCurrentPointsList();
				((ShapeWithList*)CShape1)->AddPoints(&PtColl2);
			}
			else
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), endpoint1.getX(), endpoint1.getY()) < RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), endpoint2.getX(), endpoint2.getY()))
				{
					PtColl2.Addpoint(new SinglePoint(endpoint1.getX(), endpoint1.getY(), endpoint1.getZ()));
					PtColl1.Addpoint(new SinglePoint(endpoint2.getX(), endpoint2.getY(), endpoint2.getZ()));
				}
				else
				{
					PtColl2.Addpoint(new SinglePoint(endpoint2.getX(), endpoint2.getY(), endpoint2.getZ()));
					PtColl1.Addpoint(new SinglePoint(endpoint1.getX(), endpoint1.getY(), endpoint1.getZ()));
				}
				((ShapeWithList*)CShape1)->ResetCurrentPointsList();
				((ShapeWithList*)CShape1)->AddPoints(&PtColl1);
				((ShapeWithList*)CShape2)->ResetCurrentPointsList();
				((ShapeWithList*)CShape2)->AddPoints(&PtColl2);
			}
		}
		else if(start_pt_constraint && end_pt_constraint)
		{
			this->IsValid(false);
			double ans[3] = {0};
			double stpt[2] = {StartPoint.getX(), StartPoint.getY()}, endpt[2] = {Endpoint.getX(), Endpoint.getY()};
			BESTFITOBJECT->FG_Circle_BestFit(pts, this->PointsList->Pointscount(), &ans[0], start_pt_constraint, end_pt_constraint, &stpt[0], &endpt[0]);
			this->center.set(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->Radius(ans[2]);
			this->calculateAttributes();
			this->IsValid(true);
		}
		else
		{
			Vector CurrentPoint;
			this->IsValid(false);
			double ans[3] = {0}, endpts[4], angles[2];
			double stpt[2] = {StartPoint.getX(), StartPoint.getY()}, endpt[2] = {Endpoint.getX(), Endpoint.getY()};
			BESTFITOBJECT->FG_Circle_BestFit(pts, PointsList->Pointscount(), &ans[0], start_pt_constraint, end_pt_constraint, &stpt[0], &endpt[0]);
			this->center.set(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->Radius(ans[2]);
			if(start_pt_constraint)
			{
				if(this->getFgNearShapes().size() == 1)
				{
					pts[0] = StartPoint.getX(); pts[1] = StartPoint.getY(); 
					CurrentPoint.set(StartPoint);
					RMATH2DOBJECT->ArcBestFitAngleCalculation(pts, PointsList->Pointscount(), &ans[0], ans[2], &angles[0]);
					RMATH2DOBJECT->ArcEndPoints(&ans[0], ans[2], angles[0], angles[1], &endpts[0]);
				}
				else
				{
					PointCollection PtColl;
					if(CShape2 != NULL)
					{
					
						if(StartPoint.operator==(endpoint1))
						{
							if(endpoint2.operator==(Sh2Endp1))
								PtColl.Addpoint(new SinglePoint(Sh2Endp2.getX(), Sh2Endp2.getY(), Sh2Endp2.getZ()));
							else
								PtColl.Addpoint(new SinglePoint(Sh2Endp1.getX(), Sh2Endp1.getY(), Sh2Endp1.getZ()));
							if(CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
							{
								Vector* Cmidpt = ((Circle*)CShape2)->getMidPoint();
								PtColl.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
								((Circle*)CShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
							}
						}
						else
						{
							if(endpoint1.operator==(Sh2Endp1))
								PtColl.Addpoint(new SinglePoint(Sh2Endp2.getX(), Sh2Endp2.getY(), Sh2Endp2.getZ()));
							else
								PtColl.Addpoint(new SinglePoint(Sh2Endp1.getX(), Sh2Endp1.getY(), Sh2Endp1.getZ()));
							if(CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
							{
								Vector* Cmidpt = ((Circle*)CShape2)->getMidPoint();
								PtColl.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
								((Circle*)CShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
							}
						}
					}
					pts[0] = StartPoint.getX(); pts[1] = StartPoint.getY(); 
					CurrentPoint.set(StartPoint);
					RMATH2DOBJECT->ArcBestFitAngleCalculation(pts, PointsList->Pointscount(), &ans[0], ans[2], &angles[0]);
					RMATH2DOBJECT->ArcEndPoints(&ans[0], ans[2], angles[0], angles[1], &endpts[0]);

					if(RMATH2DOBJECT->Pt2Pt_distance(endpts[0], endpts[1], StartPoint.getX(), StartPoint.getY()) < RMATH2DOBJECT->Pt2Pt_distance(endpts[2], endpts[3], StartPoint.getX(), StartPoint.getY()))				
						PtColl.Addpoint(new SinglePoint(endpts[2], endpts[3], StartPoint.getZ()));
					else
						PtColl.Addpoint(new SinglePoint(endpts[0], endpts[1], StartPoint.getZ()));
					if(CShape2 != NULL)
					{
						((ShapeWithList*)CShape2)->ResetCurrentPointsList();
						((ShapeWithList*)CShape2)->AddPoints(&PtColl);
					}
				}
			}
			else
			{
				if(this->getFgNearShapes().size() == 1)
				{
					pts[0] = Endpoint.getX(); pts[1] = Endpoint.getY(); 
					CurrentPoint.set(Endpoint);
					RMATH2DOBJECT->ArcBestFitAngleCalculation(pts, PointsList->Pointscount(), &ans[0], ans[2], &angles[0]);
					RMATH2DOBJECT->ArcEndPoints(&ans[0], ans[2], angles[0], angles[1], &endpts[0]);
				}
				else
				{
					PointCollection PtColl;
					if(CShape1 != NULL)
					{
						if(Endpoint.operator==(endpoint1))
						{
							if(endpoint2.operator==(Sh1Endp1))
								PtColl.Addpoint(new SinglePoint(Sh1Endp2.getX(), Sh1Endp2.getY(), Sh1Endp2.getZ()));
							else
								PtColl.Addpoint(new SinglePoint(Sh1Endp1.getX(), Sh1Endp1.getY(), Sh1Endp1.getZ()));
							if(CShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
							{
								Vector* Cmidpt = ((Circle*)CShape1)->getMidPoint();
								PtColl.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
								((Circle*)CShape1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
							}
						}
						else
						{
							if(endpoint1.operator==(Sh1Endp1))
								PtColl.Addpoint(new SinglePoint(Sh1Endp2.getX(), Sh1Endp2.getY(), Sh1Endp2.getZ()));
							else
								PtColl.Addpoint(new SinglePoint(Sh1Endp1.getX(), Sh1Endp1.getY(), Sh1Endp1.getZ()));
							if(CShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
							{
								Vector* Cmidpt = ((Circle*)CShape1)->getMidPoint();
								PtColl.Addpoint(new SinglePoint(Cmidpt->getX(), Cmidpt->getY(), Cmidpt->getZ()));
								((Circle*)CShape1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
							}
						}
					}
					pts[0] = Endpoint.getX(); pts[1] = Endpoint.getY(); 
					CurrentPoint.set(Endpoint);
					RMATH2DOBJECT->ArcBestFitAngleCalculation(pts, PointsList->Pointscount(), &ans[0], ans[2], &angles[0]);
					RMATH2DOBJECT->ArcEndPoints(&ans[0], ans[2], angles[0], angles[1], &endpts[0]);

					if(RMATH2DOBJECT->Pt2Pt_distance(endpts[0], endpts[1], Endpoint.getX(), Endpoint.getY()) < RMATH2DOBJECT->Pt2Pt_distance(endpts[2], endpts[3], Endpoint.getX(), Endpoint.getY()))				
						PtColl.Addpoint(new SinglePoint(endpts[2], endpts[3], Endpoint.getZ()));
					else
						PtColl.Addpoint(new SinglePoint(endpts[0], endpts[1], Endpoint.getZ()));
					if(CShape1 != NULL)
					{
						((ShapeWithList*)CShape1)->ResetCurrentPointsList();
						((ShapeWithList*)CShape1)->AddPoints(&PtColl);
					}
				}
			}
			double angle1 = RMATH2DOBJECT->ray_angle(center.getX(), center.getY(), endpts[0], endpts[1]);
			double angle2 = RMATH2DOBJECT->ray_angle(center.getX(), center.getY(), endpts[2], endpts[3]);
			double angle3 = RMATH2DOBJECT->ray_angle(center.getX(), center.getY(), CurrentPoint.getX(), CurrentPoint.getY());
			double tempangle1, tempangle2;
			if(angle3 > angle1) tempangle1 = angle1 - angle3;
			else tempangle1 = angle3 - angle1;
			tempangle1 += 2 * M_PI;
			if(angle3 > angle2) tempangle2 = angle2 - angle3;
			else tempangle2 = angle3 - angle2;
			tempangle2 += 2 * M_PI;
			
			this->endpoint1.set(CurrentPoint.getX(), CurrentPoint.getY(), CurrentPoint.getZ());
			if(tempangle1 < tempangle2)
				this->endpoint2.set(endpts[0], endpts[1], MAINDllOBJECT->getCurrentDRO().getZ());
			else
				this->endpoint2.set(endpts[2], endpts[3], MAINDllOBJECT->getCurrentDRO().getZ());

			double tp[4], ce[2], rad, startang, sweepang;
			double tempAngle = RMATH2DOBJECT->ray_angle(center.getX(), center.getY(), Midpoint.getX(), Midpoint.getY());
			double tempPoints[6] = {endpoint1.getX(), endpoint1.getY(), center.getX() + Radius() * cos(tempAngle), center.getY() + Radius() * sin(tempAngle), endpoint2.getX(), endpoint2.getY()};
			RMATH2DOBJECT->Arc_3Pt(&tempPoints[0], &tempPoints[4], &tempPoints[2], &ce[0], &rad, &startang, &sweepang);
			Radius(rad); Startangle(startang); Sweepangle(sweepang);
			RMATH2DOBJECT->ArcEndPoints(&ce[0], Radius(), Startangle(), Sweepangle(), &tp[0]);
			this->endpoint1.set(tp[0], tp[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->endpoint2.set(tp[2], tp[3], MAINDllOBJECT->getCurrentDRO().getZ());

			calculateAttributes();
			this->IsValid(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); }
}

void Circle::CalculateCircleBestFit(int PointsCnt)
{
	try
	{
		double ans[4] = {0}, zLevel = MAINDllOBJECT->getCurrentDRO().getZ();
		if(!this->PointsList->getList().empty())
			zLevel = this->PointsList->getList().begin()->second->Z;
		if(PointsCnt == 2) 
			RMATH2DOBJECT->Circle_2Pt(pts, pts + 2, &ans[0], &ans[2]);
		else if (PointsCnt == 3)
		{
			//Check if the points give a proper circle. Else, we shall not assign values to the circle parameters! 
			if (!RMATH2DOBJECT->Circle_3Pt(pts, pts + 2, pts + 4, &ans[0], &ans[2])) return;
		}
		else 
		{
			if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
				BESTFITOBJECT->Circle_BestFit(pts, PointsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
			else
				BESTFITOBJECT->Circle_BestFit(pts, PointsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
		}
		if(this->ApplyFilterCorrection() && PointsCnt > 3)
		{
			int cnt = PointsCnt;
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
			{
				double circleparam[4] = {ans[0], ans[1], 0, ans[2]};
				cnt = HELPEROBJECT->FilteredCircle(this->PointsList, circleparam, FilterCorrectionFactor(), i);
				if(cnt < 4) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
				filterptsCnt = 0; 
				int n = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					if(((*Spt).second)->InValid) continue;
					SinglePoint* Pt = (*Spt).second;
					filterpts[n++] = Pt->X;
					filterpts[n++] = Pt->Y;
					filterptsCnt++;
				}
				if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				else
					BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
			}
		}
		if(this->ApplyMMCCorrection() && ScanDirDetected() && PointsCnt > 3)
		{
			int cnt = PointsCnt;
			for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
			{
				double circleparam[4] = {ans[0], ans[1], 0, ans[2]};
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
				cnt = HELPEROBJECT->OutermostInnerMostCircle(this->PointsList, circleparam, useValidPointOnly, OuterCorrection());
				if(cnt < 4) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
				filterptsCnt = 0; 
				int n = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					if(((*Spt).second)->InValid) continue;
					SinglePoint* Pt = (*Spt).second;
					filterpts[n++] = Pt->X;
					filterpts[n++] = Pt->Y;
					filterptsCnt++;
				}
				if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				else
					BESTFITOBJECT->Circle_BestFit(filterpts, filterptsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
			}
		}
		this->center.set(ans[0], ans[1], zLevel);
		this->Radius(ans[2]); this->OriginalRadius(ans[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0010", __FILE__, __FUNCSIG__); }
}

void Circle::updateParallelArc(double* Mousept, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentSh = (Circle*)(*ptr);
		double cent[2] = {ParentSh->getCenter()->getX(), ParentSh->getCenter()->getY()}, tp[4];
		double rad = RMATH2DOBJECT->Pt2Pt_distance(cent[0], cent[1], Mousept[0], Mousept[1]);
		Radius(rad);
		RMATH2DOBJECT->ArcEndPoints(&cent[0], Radius(),ParentSh->Startangle(),ParentSh->Sweepangle(), &tp[0]);
		setCenter(Vector(cent[0],cent[1], ParentSh->getCenter()->getZ()));
		Startangle(ParentSh->Startangle());
		Sweepangle(ParentSh->Sweepangle());
		setendpoint1(Vector(tp[0], tp[1], center.getZ()));
		setendpoint2(Vector(tp[2], tp[3], center.getZ()));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}
	
void Circle::updateTanCircleto2CirclesIn(double* Mousept, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentSh1 = (Circle*)(*ptr--);
		Circle* ParentSh2 = (Circle*)(*ptr);
		double cen1[2] = {ParentSh1->getCenter()->getX(), ParentSh1->getCenter()->getY()}, cent[2];
		double radius1 = ParentSh1->Radius();
		double cen2[2] = {ParentSh2->getCenter()->getX(), ParentSh2->getCenter()->getY()};
		double radius2 = ParentSh2->Radius(), circle_rad = Radius();
		bool OneCircleInsideOther = RMATH2DOBJECT->CircleInsideAnotherCircle(&cen1[0], radius1, &cen2[0], radius2);
		if(OneCircleInsideOther)
			RMATH2DOBJECT->Circle_Tangent2Circles_Inside(&cen1[0], radius1, &cen2[0], radius2, Mousept, circle_rad, &cent[0]);
		else
			RMATH2DOBJECT->Circle_Tangent2Circles(&cen1[0], radius1, &cen2[0], radius2, Mousept, &circle_rad, &cent[0]);
		setCenter(Vector(cent[0], cent[1], 0));
		Radius(circle_rad);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}
	
void Circle::updateTanCircleto2CirclesOut(double* Mousept, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentSh1 = (Circle*)(*ptr--);
		Circle* ParentSh2 = (Circle*)(*ptr);
		double cen1[2] = {ParentSh1->getCenter()->getX(), ParentSh1->getCenter()->getY()}, cent[2];
		double radius1 = ParentSh1->Radius();
		double cen2[2] = {ParentSh2->getCenter()->getX(), ParentSh2->getCenter()->getY()};
		double radius2 = ParentSh2->Radius(), circle_rad = Radius();
		RMATH2DOBJECT->Circle_Tangent2Circles_Outside(&cen1[0], radius1, &cen2[0], radius2, Mousept, &circle_rad, &cent[0]);
		setCenter(Vector(cent[0], cent[1], 0));
		Radius(circle_rad);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateTanCircletoCircle(double* Mousept, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentSh = (Circle*)(*ptr);
		double cen[2] = {ParentSh->getCenter()->getX(), ParentSh->getCenter()->getY()}, cent[2];
		double rad = Radius();
		if(flag)
			RMATH2DOBJECT->Circle_Tangent2Circle(&cen[0], ParentSh->Radius(), Mousept, &cent[0], &rad);
		else
			RMATH2DOBJECT->Circle_Tangent2CircleFixedDia(&cen[0], ParentSh->Radius(), rad, Mousept, &cent[0]);
		Radius(rad);
		setCenter(Vector(cent[0], cent[1], 0));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateTanCircleto2Lines(double* Mousept, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Line* ParentSh1 = (Line*)(*ptr--);
		Line* ParentSh2 = (Line*)(*ptr);
		double ParAngle1 = ParentSh1->Angle(), ParIntercept1 = ParentSh1->Intercept(), cent[2];
		double ParAngle2 = ParentSh2->Angle(), ParIntercept2 = ParentSh2->Intercept();
		double rad = Radius();
		RMATH2DOBJECT->Circle_Tangent2TwoLines(ParAngle1, ParIntercept1, ParAngle2, ParIntercept2, &rad, Mousept, &cent[0]);
		Radius(rad);
		setCenter(Vector(cent[0], cent[1], 0));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateTanCircletoLineCircle(double* Mousept, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentCircle = (Circle*)(*ptr--);
		Line* ParentLine = (Line*)(*ptr);
		double cen[2] = {ParentCircle->getCenter()->getX(), ParentCircle->getCenter()->getY()}, cent[2];
		double radius1 = ParentCircle->Radius();
		double ParAngle = ParentLine->Angle(), ParIntercept = ParentLine->Intercept();
		RMATH2DOBJECT->Circle_Tangent2LineCircle(&cen[0], ParentCircle->Radius(), ParAngle, ParIntercept, Mousept, Radius(), &cent[0]);
		setCenter(Vector(cent[0], cent[1], 0));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateCircleInsideTriangle()
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Line* ParentShape1 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		Line* ParentShape2 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		Line* ParentShape3 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];

		bool CheckFlag1 = HELPEROBJECT->ChecktheParallelism(ParentShape1, ParentShape2);
		bool CheckFlag2 = HELPEROBJECT->ChecktheParallelism(ParentShape2, ParentShape3);
		bool CheckFlag3 = HELPEROBJECT->ChecktheParallelism(ParentShape3, ParentShape1);
		if(!(CheckFlag1 && CheckFlag2 && CheckFlag3))
		{
			double Circle_Param[3] = {0};
			double angle1 = ParentShape1->Angle(), intercept1 = ParentShape1->Intercept();
			double angle2 = ParentShape2->Angle(), intercept2 = ParentShape2->Intercept();
			double angle3 = ParentShape3->Angle(), intercept3 = ParentShape3->Intercept();
			RMATH2DOBJECT->Circle_InsideATriangle(angle1, intercept1, angle2, intercept2, angle3, intercept3, &Circle_Param[0]);
			setCenter(Vector(Circle_Param[0], Circle_Param[1], 0));
			Radius(Circle_Param[2]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::updateCircleOutSideTriangle()
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Line* ParentShape1 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		Line* ParentShape2 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		Line* ParentShape3 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];

		bool CheckFlag1 = HELPEROBJECT->ChecktheParallelism(ParentShape1, ParentShape2);
		bool CheckFlag2 = HELPEROBJECT->ChecktheParallelism(ParentShape2, ParentShape3);
		bool CheckFlag3 = HELPEROBJECT->ChecktheParallelism(ParentShape3, ParentShape1);
		if(!(CheckFlag1 && CheckFlag2 && CheckFlag3))
		{
			double Circle_Param[3] = {0};
			double angle1 = ParentShape1->Angle(), intercept1 = ParentShape1->Intercept();
			double angle2 = ParentShape2->Angle(), intercept2 = ParentShape2->Intercept();
			double angle3 = ParentShape3->Angle(), intercept3 = ParentShape3->Intercept();
			RMATH2DOBJECT->Circle_Circum2Triangle(angle1, intercept1, angle2, intercept2, angle3, intercept3, &Circle_Param[0]);
			setCenter(Vector(Circle_Param[0], Circle_Param[1], 0));
			Radius(Circle_Param[2]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0009", __FILE__, __FUNCSIG__); }
}

void Circle::ProbePointCalculations(int PointsCnt)
{
	try
	{
		//if(PointAtionList.size() < 1) return;
		if(this->ShapeForThreadMeasurement()) return;
		if(this->ShapeForPerimeter()) return;
		if(this->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE) return;
		double ans[7] = {0};
		if(PointsCnt >= 3) // && PointsCnt < 10)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdCircle();
		}
		if(ShapeDoneUsingProbe())
		{
			updateCircle_UsingPorbe();
			if(this->CircleType == RapidEnums::CIRCLETYPE::CIRCLE3DCIR)
			{	
				if (!BESTFITOBJECT->ThreeDCircle_BestFit(pts, PointsCnt, &ans[0]))
				{
					if(Radius() > 0)
						this->IsValid(true);
					else 
						this->IsValid(false);
					return;
				}
				if(this->ApplyFilterCorrection() && PointsCnt > 3)
				{
					bool flag = false;
					int cnt = PointsCnt;
					for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
					{
						cnt = HELPEROBJECT->FilteredCircle3D(this->PointsList, ans, FilterCorrectionFactor(), i);
						if(cnt < 4) break;
						if(this->filterpts != NULL){ free(filterpts); filterpts = NULL;}
						this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
						this->filterptsCnt = 0;
						int n = 0;
						for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
						{
							if(((*Spt).second)->InValid) continue;
							SinglePoint* Pt = (*Spt).second;
							filterpts[n++] = Pt->X;
							filterpts[n++] = Pt->Y;
							filterpts[n++] = Pt->Z;
							filterptsCnt++;
						}
						flag = BESTFITOBJECT->ThreeDCircle_BestFit(filterpts, filterptsCnt, &ans[0]);
						if (!flag)
						{
							if(Radius() > 0)
								this->IsValid(true);
							else 
								this->IsValid(false);
							return;
						}
					}
				}
				if(ApplyMMCCorrection() && ScanDirDetected() && PointsCnt > 3)
				{
					bool flag = false;
					int cnt = PointsCnt;
					for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
					{
						bool useValidPointOnly = false;
						if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
						cnt = HELPEROBJECT->OutermostInnerMostCircle3D(this->PointsList, ans, useValidPointOnly, OuterCorrection());
						if(cnt < 4) break;
						if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
						this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
						this->filterptsCnt = 0;
						int n = 0;
						for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
						{
							if(((*Spt).second)->InValid) continue;
							SinglePoint* Pt = (*Spt).second;
							filterpts[n++] = Pt->X;
							filterpts[n++] = Pt->Y;
							filterpts[n++] = Pt->Z;
							filterptsCnt++;
						}
						flag = BESTFITOBJECT->ThreeDCircle_BestFit(filterpts, filterptsCnt, &ans[0]);
						if (!flag)
						{
							if(Radius() > 0)
								this->IsValid(true);
							else 
								this->IsValid(false);
							return;
						}
					}
				}
				this->center.set(ans[0], ans[1], ans[2]);
				this->Radius(ans[6]); 
				this->dir_l(ans[3]); this->dir_m(ans[4]); this->dir_n(ans[5]);
			}
			else
				CalculateCircleBestFit(PointsCnt);
			/*if(this->CircleType == RapidEnums::CIRCLETYPE::CIRCLE3DCIR)
			{	
				BESTFITOBJECT->ThreeDCircle_BestFit(pts, PointsCnt, &ans[0]);
				this->center.set(ans[0], ans[1], ans[2]);
				this->Radius(ans[6]); 
				this->dir_l(ans[3]); this->dir_m(ans[4]); this->dir_n(ans[5]);
			}
			else
			{
				if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					BESTFITOBJECT->Circle_BestFit(pts, PointsCnt, &ans[0], true);
				else
					BESTFITOBJECT->Circle_BestFit(pts, PointsCnt, &ans[0], SigmaMode());
				double zLevel = MAINDllOBJECT->getCurrentDRO().getZ();
				if(!this->PointsList->getList().empty())
					 zLevel = this->PointsList->getList().begin()->second->Z;
				this->center.set(ans[0], ans[1], zLevel);
				this->Radius(ans[2]);
			}*/
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0012", __FILE__, __FUNCSIG__); }
}

void Circle::CheckPointIsForPorbe()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0013", __FILE__, __FUNCSIG__); }
}

void Circle::CheckIdorOdCircle()
{
	try
	{
		int IDCount = 0;
		PointCollection ptCol;
		HELPEROBJECT->GetMcsPointsList(this->PointsListOriginal, &ptCol);
		Vector CenterMcs;
		HELPEROBJECT->GetMcsPoint(&center, &CenterMcs);
		for(PC_ITER Item = ptCol.getList().begin(); Item != ptCol.getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(Spt->Pdir == 0) //FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT
			{
				if((CenterMcs.getX() - Spt->X) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 1) //FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT
			{
				if((CenterMcs.getX() - Spt->X) < 0)
					IDCount++;
				else
					IDCount--;
			}
			else if(Spt->Pdir == 2) //FramegrabBase::PROBEDIRECTION::YTOP2BOTTOM
			{
				if((CenterMcs.getY() - Spt->Y) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 3) // FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP
			{
				if((CenterMcs.getY() - Spt->Y) < 0)
					IDCount++;
				else
					IDCount--;
			}
			else if(Spt->Pdir == 4) // FramegrabBase::PROBEDIRECTION::ZTOPTOBOTTOM
			{
				if((CenterMcs.getZ() - Spt->Z) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 5) // FramegrabBase::PROBEDIRECTION::ZBOTTOM2TOP
			{
				if((CenterMcs.getZ() - Spt->Z) < 0)
					IDCount++;
				else
					IDCount--;
			}
		}
		ptCol.deleteAll();
		if(IDCount >= 0)
			this->IdCircle(true);
		else
			this->IdCircle(false);
		if(this->ApplyMMCCorrection() && (!this->ScanDirDetected()))
		{
			this->OuterCorrection(!this->IdCircle());
			this->ScanDirDetected(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0014", __FILE__, __FUNCSIG__); }
}

void Circle::updateCircle_UsingPorbe()
{
	try
	{
		double Pradius = 0;
		if(this->pts != NULL){free(pts); pts = NULL;}
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		if(this->CircleType == RapidEnums::CIRCLETYPE::CIRCLE3DCIR)
		{
			double Pt[3] = {0};
			for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};
				Pradius = getProbeRadius(Spt->PLR);
				if(!IdCircle())  Pradius = -Pradius; 
				double dir[3] = {Spt->X - this->center.getX(), Spt->Y - this->center.getY(), Spt->Z - this->center.getZ()};
				double dir_cosines[3] = {0, 0, 1};
				RMATH3DOBJECT->DirectionCosines(dir, dir_cosines);
				for (int i =0; i< 3; i++)
				{
					Pt[i] = PrPoint[i] + dir_cosines[i] * Pradius;
				}
				SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
				Spt1->SetValues(Pt[0], Pt[1], Pt[2], Spt->R, Spt->Pdir);
			}

			this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
			this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
			filterptsCnt = 0;
			int n = 0, j = 0;
			for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
			{
				SinglePoint* Pt = (*Spt).second;
				this->pts[n++] = Pt->X;
				this->pts[n++] = Pt->Y;
				this->pts[n++] = Pt->Z;
				if(Pt->InValid) continue;
				this->filterpts[j++] = Pt->X;
				this->filterpts[j++] = Pt->Y;
				this->filterpts[j++] = Pt->Z;
				filterptsCnt++;
			}
		}
		else
		{
			double rangle, PtX, PtY;
			for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				rangle = RMATH2DOBJECT->ray_angle(center.getX(), center.getY(), Spt->X, Spt->Y);
				Pradius = getProbeRadius(Spt->PLR);
				if(!IdCircle()) Pradius = -Pradius; 
				PtX = Spt->X + cos(rangle) * Pradius;
				PtY = Spt->Y + sin(rangle) * Pradius;
				SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
				Spt1->SetValues(PtX, PtY, Spt->Z, Spt->R, Spt->Pdir);
			}

			this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
			this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
			filterptsCnt = 0;
			int n = 0, j = 0;
			for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
			{
				SinglePoint* Pt = (*Spt).second;
				this->pts[n++] = Pt->X;
				this->pts[n++] = Pt->Y;
				if(Pt->InValid) continue;
				this->filterpts[j++] = Pt->X;
				this->filterpts[j++] = Pt->Y;
				filterptsCnt++;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0015", __FILE__, __FUNCSIG__); }
}

void Circle::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(CircleType == RapidEnums::CIRCLETYPE::CIRCLE3DCIR) return;
		if(CircleType == RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER || CircleType == RapidEnums::CIRCLETYPE::TWOPTCIRCLE || CircleType == RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE || 
			CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT || CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE)
		{
			//modified on May 29 2012 (to handle update to dependent shapes in PP)
			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				if (this->PointsList->Pointscount() > 0)
				{
					int cnt = this->PointsList->Pointscount();
					int i = 0;
					for(RC_ITER curraction = PPCALCOBJECT->getPPActionlist().getList().begin(); curraction != PPCALCOBJECT->getPPActionlist().getList().end(); curraction++)
					{
						RAction* action = (RAction*)(*curraction).second;
						if(!action->ActionStatus()) continue;
						if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
						{
							if(this->getId() != ((AddPointAction*)action)->getShape()->getId())
								continue;
					
							if(((AddPointAction*)action)->getFramegrab()->CurrentWindowNo == 1)
							{
								if(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt != NULL)
								{
									i++;
									SinglePoint* Spt = this->PointsList->getList()[(int)(*(((AddPointAction*)action)->getFramegrab()->PointActionIdList.begin()))];
									Spt->SetValues(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getX(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getY(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getZ());
									Spt = this->PointsListOriginal->getList()[(int)(*(((AddPointAction*)action)->getFramegrab()->PointActionIdList.begin()))];
									Spt->SetValues(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getX(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getY(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getZ());
									if (i == cnt) 
										break;
								}
							}
						}
					}
					this->UpdateShape();
				}
			}
			else
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
		}
		else if(CircleType == RapidEnums::CIRCLETYPE::CIRCLEFROMCLOUDPNTS)
		{
			 this->PointsList->deleteAll();
			 this->PointsListOriginal->deleteAll();
			 PointCollection PtColl;
			 ShapeWithList *CShape = (ShapeWithList*)sender;
			 for(PC_ITER item = CShape->PointsList->getList().begin(); item != CShape->PointsList->getList().end(); item++)
			 {
				 SinglePoint* Spt = item->second;
				 PtColl.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
			 }
			 this->AddPoints(&PtColl);
		}
		else if(CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
			((PCDCircle*)this)->CalculatePcdCircle();
		else if(CircleType == RapidEnums::CIRCLETYPE::LINEARCLINE_CIRCLE)
			updateLineArcLineShape();
		else if(CircleType == RapidEnums::CIRCLETYPE::ARC_TANGENT_2_LINES_CIRCLE)
			this->UpdateBestFit();
		else if(CircleType ==  RapidEnums::CIRCLETYPE::PROJECTEDCIRCLE)
			this->updateNormalCircles();
		else
			updateDerivedCircles();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0016", __FILE__, __FUNCSIG__); }
}

void Circle::updateArcTangentToTwoLines(int* p_PointsCnt, double* ans)
{
	try
	{
		this->IsValid(false);
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Shape* Csh1 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		Shape* Csh3 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		Csh1->IsValid(false);
		Csh3->IsValid(false);
		//call function to do line-arc-line update

		double line1[2] = {((Line*) Csh1)->Angle(), ((Line*) Csh1)->Intercept()};
		double line2[2] = {((Line*) Csh3)->Angle(), ((Line*) Csh3)->Intercept()};

		double circle[3] = {0};

		if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
			BESTFITOBJECT->Circle_BestFit(pts, (*p_PointsCnt), circle, true);
		else
			BESTFITOBJECT->Circle_BestFit(pts, (*p_PointsCnt), circle, SigmaMode());


		//populate answer[0], answer[1] with projection of center of arc on angle bisector, answer[2] with slope of angle bisector and answer[3] with radius of arc - these are init guess values.

		double answer[4] = {0};
		double intercept = 0;
		RMATH2DOBJECT->Angle_bisector(line1[0], line1[1], line2[0], line2[1], circle, &answer[2], &intercept);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(answer[2], intercept, circle, answer);
		answer[3] = circle[2];

		BESTFITOBJECT->LineArcLine_BestFit(pts,(*p_PointsCnt),answer, true, true);

		ans[0] = answer[0];
		ans[1] = answer[1];
		ans[2] = answer[3];

		double linearc_pt1[2] = {0};
		double linearc_pt2[2] = {0};
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(line1[0], line1[1], answer, linearc_pt1);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(line2[0], line2[1], answer, linearc_pt2);

		//update radius as below 
		ans[2] = RMATH2DOBJECT->Pt2Line_Dist(answer[0], answer[1], line1[0], line1[1]) ;
		double d = RMATH2DOBJECT->Pt2Line_Dist(answer[0], answer[1], line2[0], line2[1]);
		if (d > ans[2])
			ans[2] = d;

		double intpt[2] = {0};
		RMATH2DOBJECT->Line_lineintersection(line1[0], line1[1], line2[0], line2[1], intpt);
		double radialmidpt[2] = {0};
		double z = sqrt(pow(intpt[0] - answer[0], 2) + pow(intpt[1] - answer[1],2));
		for (int i = 0; i < 2; i++)
			radialmidpt[i] = answer[i] + (intpt[i] - answer[i]) * ans[2] / z;

		double pts_on_arc[6] = {linearc_pt1[0], linearc_pt1[1], radialmidpt[0], radialmidpt[1], linearc_pt2[0], linearc_pt2[1]};
		double angles[2], endpts[4];
		RMATH2DOBJECT->ArcBestFitAngleCalculation(pts_on_arc, 3, &ans[0], ans[2], &angles[0]);
		this->center.set(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
		this->Radius(ans[2]); this->OriginalRadius(ans[2]);
		this->Startangle(angles[0]); this->Sweepangle(angles[1]);
		/*ProbePointCalculations(*p_PointsCnt);*/
		double ans1[3] = {center.getX(), center.getY(), Radius()};
		RMATH2DOBJECT->ArcEndPoints(&ans1[0], ans1[2], angles[0], angles[1], &endpts[0]);
		this->endpoint1.set(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ());
		this->endpoint2.set(endpts[2],endpts[3], MAINDllOBJECT->getCurrentDRO().getZ());		
		calculateAttributes();

		bool flag1 = false;
		double tempendpt[4];
		PointCollection PtColl;
		if (!(RMATH2DOBJECT->MousePt_OnArc(ans, ans[2], this->Startangle(), this->Sweepangle(), &tempendpt[0], &tempendpt[2], MAINDllOBJECT->getWindow(0).getUppX(), linearc_pt1, &flag1)))
		{
		//if (!added_line_arc_line_pt)
		//{
			//added_line_arc_line_pt = true;
			PtColl.Addpoint(new SinglePoint(linearc_pt1[0], linearc_pt1[1], center.getZ()));
			*p_PointsCnt = *p_PointsCnt + 1;
			this->AddPoints(&PtColl);
		}
		if (!(RMATH2DOBJECT->MousePt_OnArc(ans, ans[2], this->Startangle(), this->Sweepangle(), &tempendpt[0], &tempendpt[2], MAINDllOBJECT->getWindow(0).getUppX(), linearc_pt2, &flag1)))
		{
			PtColl.deleteAll();
			PtColl.Addpoint(new SinglePoint(linearc_pt2[0], linearc_pt2[1], center.getZ()));
			*p_PointsCnt = *p_PointsCnt + 1;
			this->AddPoints(&PtColl);
		}

		double line_endpts[6];
		((Line*) Csh1)->getEndPoints (line_endpts);
		((Line*) Csh1)->EndPointsFixed(true);
		//if (!(RMATH2DOBJECT->MousePt_OnFiniteline(line_endpts, line_endpts+3, line1[0], line1[1], linearc_pt1, MAINDllOBJECT->getWindow(0).getUppX(), &flag1)))
		//{
			double dis1 = RMATH2DOBJECT->Pt2Pt_distance(line_endpts[0], line_endpts[1], linearc_pt1[0], linearc_pt1[1]);
			double dis2 = RMATH2DOBJECT->Pt2Pt_distance(line_endpts[3], line_endpts[4], linearc_pt1[0], linearc_pt1[1]);
			if (dis2 > dis1)
			{
				if (dis1 > RMATH2DOBJECT->MeasureDecValue)
				{
					((Line*) Csh1)->setPoint1(Vector(linearc_pt1[0], linearc_pt1[1], MAINDllOBJECT->getCurrentDRO().getZ()));
					((Line*) Csh1)->setPoint2(Vector(line_endpts[3], line_endpts[4], MAINDllOBJECT->getCurrentDRO().getZ()));
				}
			}
			else
			{
				if (dis2 > RMATH2DOBJECT->MeasureDecValue)
				{
					((Line*) Csh1)->setPoint1(Vector(line_endpts[0], line_endpts[1], MAINDllOBJECT->getCurrentDRO().getZ()));
					((Line*) Csh1)->setPoint2(Vector(linearc_pt1[0], linearc_pt1[1], MAINDllOBJECT->getCurrentDRO().getZ()));
				}
			}
		//}

		((Line*) Csh3)->getEndPoints (line_endpts);
		((Line*) Csh3)->EndPointsFixed(true);
		//if (!(RMATH2DOBJECT->MousePt_OnFiniteline(line_endpts, line_endpts+3, line2[0], line2[1], linearc_pt2, MAINDllOBJECT->getWindow(0).getUppX(), &flag1)))
		//{
			dis1 = RMATH2DOBJECT->Pt2Pt_distance(line_endpts[0], line_endpts[1], linearc_pt2[0], linearc_pt2[1]);
			dis2 = RMATH2DOBJECT->Pt2Pt_distance(line_endpts[3], line_endpts[4], linearc_pt2[0], linearc_pt2[1]);
			if (dis2 > dis1)
			{
				if (dis1 > RMATH2DOBJECT->MeasureDecValue)
				{
					((Line*) Csh3)->setPoint1(Vector(linearc_pt2[0], linearc_pt2[1], MAINDllOBJECT->getCurrentDRO().getZ()));
					((Line*) Csh3)->setPoint2(Vector(line_endpts[3], line_endpts[4], MAINDllOBJECT->getCurrentDRO().getZ()));
				}
			}
			else
			{
				if (dis2 > RMATH2DOBJECT->MeasureDecValue)
				{
					((Line*) Csh3)->setPoint1(Vector(line_endpts[0], line_endpts[1], MAINDllOBJECT->getCurrentDRO().getZ()));
					((Line*) Csh3)->setPoint2(Vector(linearc_pt2[0], linearc_pt2[1], MAINDllOBJECT->getCurrentDRO().getZ()));
				}
			}
		//}
		this->IsValid(true);
		Csh1->IsValid(true);
		Csh3->IsValid(true);
	}
	catch(...)
	{
		 MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0015", __FILE__, __FUNCSIG__);
	}
}

void Circle::updateBestFitArc()
{
	try
	{
		int ptscnt = this->PointsList->Pointscount();
		if (ptscnt < 3) return;
		this->updateArc(ptscnt);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0015a", __FILE__, __FUNCSIG__);
	}
}

void Circle::updateSiblingArcs(int PointsCnt)
{
	try
	{
		if (PointsCnt < 3)
			return;
		if (this->getSiblings().size() != 1)
		{
			updateArc(PointsCnt);
			return;
		}
		Circle* arc2 = (Circle*) (this->getSiblings().front());
		if (arc2->getSiblings().size() != 1)
		{
			updateArc(PointsCnt);
			return;
		}
		if (arc2->getSiblings().front()->getId() != this->getId())
		{
			updateArc(PointsCnt);
			return;
		}
		if (arc2->PointsList->Pointscount() < 3)
		{
			updateArc(PointsCnt);
			return;
		}
		updateArc(PointsCnt);
		if (arc2->PointsList->Pointscount() < 3)
			return;
		arc2->updateBestFitArc();
		int totptscnt = PointsCnt + arc2->PointsList->Pointscount();
		double* totpts = (double*) malloc (2*totptscnt*sizeof(double));
		double answer[8] = {0};

		//set initial guess values
		this->getCenter()->FillDoublePointer(answer);
		answer[2] = this->Radius();
		arc2->getCenter()->FillDoublePointer(answer + 3);
		answer[5] = arc2->Radius();
		double intpt1[2] = {0};
		double intpt2[2] = {0};
		//int cnt;
		//RMATH2DOBJECT->arc_arcintersection(answer, answer[2], this->Startangle(), this->Sweepangle(), answer + 3, answer[5], arc2->Startangle(), arc2->Sweepangle(), intpt1, intpt2, &cnt);

		double pt1[2] = {0}; double pt2[2] = {0}; double pt3[2] = {0}; double pt4[2] = {0};
		this->getendpoint1()->FillDoublePointer(pt1);
		this->getendpoint2()->FillDoublePointer(pt2);
		arc2->getendpoint1()->FillDoublePointer(pt3);
		arc2->getendpoint2()->FillDoublePointer(pt4);

		double dist, dist2;
		//if (cnt == 0)
		//{
			dist = RMATH2DOBJECT->Pt2Pt_distance(pt1, pt3);
			double* endpt1 = pt2;
			double* endpt2 = pt4;
			intpt1[0] = (pt1[0] + pt3[0])/2; intpt1[1] = (pt1[1] + pt3[1])/2;
			dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt1, pt4);
			if (dist > dist2)
			{
				dist = dist2;
				intpt1[0] = (pt1[0] + pt4[0])/2; intpt1[1] = (pt1[1] + pt4[1])/2;
				endpt1 = pt2;
				endpt2 = pt3;
			}
			dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt2, pt3);
			if (dist > dist2)
			{
				dist = dist2;
				intpt1[0] = (pt2[0] + pt3[0])/2; intpt1[1] = (pt2[1] + pt3[1])/2;
				endpt1 = pt1;
				endpt2 = pt4;
			}
			dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt2, pt4);
			if (dist > dist2)
			{
				dist = dist2;
				intpt1[0] = (pt2[0] + pt4[0])/2; intpt1[1] = (pt2[1] + pt4[1])/2;
				endpt1 = pt1;
				endpt2 = pt3;
			}
		//}
		//if (cnt != 2)
		//{
			answer[6] = intpt1[0]; answer[7] = intpt1[1];
		//}
		//else
		//{
		//	answer[6] = (intpt1[0] + intpt2[0])/2;
		//	answer[7] = (intpt1[1] + intpt2[1])/2;
		//}

		memcpy(totpts, pts, 2*PointsCnt*sizeof(double));
		memcpy(totpts + 2*PointsCnt, arc2->pts, 2*(arc2->PointsList->Pointscount())*sizeof(double));

		//for arc given true
		//dist = RMATH2DOBJECT->Pt2Pt_distance(pt1, pt3);
		//answer[6] = pt1[0]; answer[7] = pt1[1];
		//dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt1, pt4);
		//if (dist2 < dist)
		//{
		//	dist = dist2;
		//}
		//dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt2, pt3);
		//if (dist2 < dist)
		//{
		//	dist = dist2;
		//	answer[6] = pt2[0]; answer[7] = pt2[1];
		//}
		//dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt2, pt4);
		//if (dist2 < dist)
		//{
		//	dist = dist2;
		//	answer[6] = pt2[0]; answer[7] = pt2[1];
		//}
		if (!BESTFITOBJECT->ArcArc_BestFit(totpts, totptscnt, answer, false))
		{
			double answer2[8] = {0};
			memcpy(answer2, answer + 3, 3*sizeof(double));
			memcpy(answer2 + 3, answer, 3*sizeof(double));
			memcpy(answer2 + 6, answer + 6, 2*sizeof(double));
			if (!BESTFITOBJECT->ArcArc_BestFit(totpts, totptscnt, answer2, false))
			{
				free(totpts);
				return;
			}
			memcpy(answer, answer2 + 3, 3*sizeof(double));
			memcpy(answer + 3, answer2, 3*sizeof(double));
			memcpy(answer + 6, answer2 + 6, 2*sizeof(double));
		}
		dist = RMATH2DOBJECT->Pt2Pt_distance(endpt1, answer + 6);
		dist2 = RMATH2DOBJECT->Pt2Pt_distance(endpt2, answer + 6);
		double dist3 = RMATH2DOBJECT->Pt2Pt_distance(intpt1, answer + 6);
		if ((dist < dist3)||(dist2 < dist3))
		{
			double answer2[8] = {0};
			memcpy(answer2, answer + 3, 3*sizeof(double));
			memcpy(answer2 + 3, answer, 3*sizeof(double));
			memcpy(answer2 + 6, answer + 6, 2*sizeof(double));
			if (!BESTFITOBJECT->ArcArc_BestFit(totpts, totptscnt, answer2, false))
			{
				free(totpts);
				return;
			}
			memcpy(answer, answer2 + 3, 3*sizeof(double));
			memcpy(answer + 3, answer2, 3*sizeof(double));
			memcpy(answer + 6, answer2 + 6, 2*sizeof(double));
			dist = RMATH2DOBJECT->Pt2Pt_distance(endpt1, answer + 6);
			dist2 = RMATH2DOBJECT->Pt2Pt_distance(endpt2, answer + 6);
			dist3 = RMATH2DOBJECT->Pt2Pt_distance(intpt1, answer + 6);
			if ((dist < dist3)||(dist2 < dist3))
			{
				free(totpts);
				return;
			}
		}
		
		//set current arc parameters
		this->center.set(answer[0], answer[1], MAINDllOBJECT->getCurrentDRO().getZ());
		this->Radius(answer[2]); this->OriginalRadius(answer[2]);
		dist = RMATH2DOBJECT->Pt2Pt_distance(pt1, answer + 6);
		dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt2, answer + 6);
		
		if (dist < dist2)
		{
			this->endpoint1.set(answer[6], answer[7], MAINDllOBJECT->getCurrentDRO().getZ());
			double prevstartangle = this->Startangle();
			double currstartangle = RMATH2DOBJECT->ray_angle(answer, answer + 6);
			this->Startangle(currstartangle);
			double sweepangle = this->Sweepangle();
			this->Sweepangle(sweepangle - currstartangle + prevstartangle);
			double endpts[2] = {0};
			endpts[0] = answer[0] + this->Radius() * cos(this->Startangle() + this->Sweepangle());
			endpts[1] = answer[1] + this->Radius() * sin(this->Startangle() + this->Sweepangle());
			this->endpoint2.set(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ());
		}
		else
		{
			this->endpoint2.set(answer[6], answer[7], MAINDllOBJECT->getCurrentDRO().getZ());
			double prevendangle = this->Startangle() + this->Sweepangle();
			double currendangle = RMATH2DOBJECT->ray_angle(answer, answer + 6);
			if (currendangle < this->Startangle())
				currendangle += 2 * M_PI;
			double sweepangle = this->Sweepangle();
			this->Sweepangle(sweepangle + currendangle - prevendangle);
			double endpts[2] = {0};
			endpts[0] = answer[0] + this->Radius() * cos(this->Startangle());
			endpts[1] = answer[1] + this->Radius() * sin(this->Startangle());
			this->endpoint1.set(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ());
		}
		calculateAttributes();

		//now set sibling arc params
		arc2->setCenter(Vector(answer[3], answer[4], MAINDllOBJECT->getCurrentDRO().getZ()));
		arc2->Radius(answer[5]); arc2->OriginalRadius(answer[5]);
		dist = RMATH2DOBJECT->Pt2Pt_distance(pt3, answer + 6);
		dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt4, answer + 6);
		
		if (dist < dist2)
		{
			arc2->setendpoint1(Vector(answer[6], answer[7]));
			double prevstartangle = arc2->Startangle();
			double currstartangle = RMATH2DOBJECT->ray_angle(answer + 3, answer + 6);
			arc2->Startangle(currstartangle);
			double sweepangle = arc2->Sweepangle();
			arc2->Sweepangle(sweepangle - currstartangle + prevstartangle);
			double endpts[2] = {0};
			endpts[0] = answer[3] + arc2->Radius() * cos(arc2->Startangle() + arc2->Sweepangle());
			endpts[1] = answer[4] + arc2->Radius() * sin(arc2->Startangle() + arc2->Sweepangle());
			arc2->setendpoint2(Vector(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ()));
		}
		else
		{
			arc2->setendpoint2(Vector(answer[6], answer[7], MAINDllOBJECT->getCurrentDRO().getZ()));
			double prevendangle = arc2->Startangle() + arc2->Sweepangle();
			double currendangle = RMATH2DOBJECT->ray_angle(answer + 3, answer + 6);
			if (currendangle < arc2->Startangle())
				currendangle += 2 * M_PI;
			double sweepangle = arc2->Sweepangle();
			arc2->Sweepangle(sweepangle + currendangle - prevendangle);
			double endpts[2] = {0};
			endpts[0] = answer[3] + arc2->Radius() * cos(arc2->Startangle());
			endpts[1] = answer[4] + arc2->Radius() * sin(arc2->Startangle());
			arc2->setendpoint1(Vector(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ()));
		}
		arc2->calculateAttributes();

		free(totpts);
		return;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0029", __FILE__, __FUNCSIG__);
	}
}

void Circle::updateArcTanToArc(int PointsCnt)
{
	try
	{
		if (PointsCnt < 3)return;
		if (this->getParents().size() != 1)
		{
			updateArc(PointsCnt);
			return;
		}
		Circle* arc1 = (Circle*) (this->getParents().front());

		if (arc1->PointsList->Pointscount() < 3)
		{
			updateArc(PointsCnt);
			return;
		}
		updateArc(PointsCnt);
		//arc1->updateBestFitArc();

		double arc_in_first[5] = {0};
		double arc_in_second[5] = {0};
		//set params to pass in to function
		this->getCenter()->FillDoublePointer(arc_in_second);
		arc_in_second[2] = this->Radius();
		arc1->getCenter()->FillDoublePointer(arc_in_first);
		arc_in_first[2] = arc1->Radius();
		double pt1[2] = {0}; double pt2[2] = {0}; double pt3[2] = {0}; double pt4[2] = {0};
		this->getendpoint1()->FillDoublePointer(pt1);
		this->getendpoint2()->FillDoublePointer(pt2);
		arc1->getendpoint1()->FillDoublePointer(pt3);
		arc1->getendpoint2()->FillDoublePointer(pt4);

		double dist, dist2;
		bool pt1_nearby_endpt = true;

		dist = RMATH2DOBJECT->Pt2Pt_distance(pt1, pt3);
		arc_in_first[3] = pt3[0]; arc_in_first[4] = pt3[1];
		arc_in_second[3] = pt2[0]; arc_in_second[4] = pt2[1];
		dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt1, pt4);
		if (dist > dist2)
		{
			dist = dist2;
			arc_in_first[3] = pt4[0]; arc_in_first[4] = pt4[1];
			arc_in_second[3] = pt2[0]; arc_in_second[4] = pt2[1];
			pt1_nearby_endpt = true;
		}
		dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt2, pt3);
		if (dist > dist2)
		{
			dist = dist2;
			arc_in_first[3]  = pt3[0]; arc_in_first[4] = pt3[1];
			arc_in_second[3] = pt1[0]; arc_in_second[4] = pt1[1];
			pt1_nearby_endpt = false;
		}
		dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt2, pt4);
		if (dist > dist2)
		{
			dist = dist2;
			arc_in_first[3]  = pt4[0]; arc_in_first[4] = pt4[1];
			arc_in_second[3] = pt1[0]; arc_in_second[4] = pt1[1];
			pt1_nearby_endpt = false;
		}

		double arc_out_second[3] = {0};
		if (!BESTFITOBJECT->ArcTangentToArc(arc_in_first, arc_in_second, arc_out_second))
		{
			return;
		}
		
		//set current arc parameters
		this->center.set(arc_out_second[0], arc_out_second[1], MAINDllOBJECT->getCurrentDRO().getZ());
		this->Radius(arc_out_second[2]); this->OriginalRadius(arc_out_second[2]);
		
		if (pt1_nearby_endpt)
		{
			this->endpoint1.set(arc_in_first[3], arc_in_first[4], MAINDllOBJECT->getCurrentDRO().getZ());
			double currstartangle = RMATH2DOBJECT->ray_angle(arc_out_second, arc_in_first + 3);
			this->Startangle(currstartangle);
			double currendangle = RMATH2DOBJECT->ray_angle(arc_out_second, arc_in_second + 3);
			if (currendangle < currstartangle)
				currendangle += 2 * M_PI;
			this->Sweepangle(currendangle - currstartangle);
			double endpts[2] = {0};
			endpts[0] = arc_out_second[0] + this->Radius() * cos(currendangle);
			endpts[1] = arc_out_second[1] + this->Radius() * sin(currendangle);
			this->endpoint2.set(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ());
		}
		else
		{
			this->endpoint2.set(arc_in_first[3], arc_in_first[4], MAINDllOBJECT->getCurrentDRO().getZ());
			double currstartangle = RMATH2DOBJECT->ray_angle(arc_out_second, arc_in_second + 3);
			this->Startangle(currstartangle);
			double currendangle = RMATH2DOBJECT->ray_angle(arc_out_second, arc_in_first + 3);
			if (currendangle < currstartangle)
				currendangle += 2 * M_PI;
			this->Sweepangle(currendangle - currstartangle);
			double endpts[2] = {0};
			endpts[0] = arc_out_second[0] + this->Radius() * cos(currstartangle);
			endpts[1] = arc_out_second[1] + this->Radius() * sin(currstartangle);
			this->endpoint1.set(endpts[0],endpts[1], MAINDllOBJECT->getCurrentDRO().getZ());
		}
		calculateAttributes();

		return;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0029", __FILE__, __FUNCSIG__);
	}
}

void Circle::updateArcTanToLine(int PointsCnt)
{
	try
	{
		if (PointsCnt < 3)
			return;
		if (this->getParents().size() != 1)
		{
			updateArc(PointsCnt);
			return;
		}
		Line* line1 = (Line*) (this->getParents().front());

		if (line1->PointsList->Pointscount() < 2)
		{
			updateArc(PointsCnt);
			return;
		}

		updateArc(PointsCnt);

		double line_param[3] = {0};
		double arc_param[5] = {0};
		//set params to pass in to function
		this->getCenter()->FillDoublePointer(arc_param);
		arc_param[2] = this->Radius();
		line1->getPoint1()->FillDoublePointer(line_param);
		line_param[2] = line1->Angle();

		double pt1[2] = {0}; double pt2[2] = {0}; 
		this->getendpoint1()->FillDoublePointer(pt1);
		this->getendpoint2()->FillDoublePointer(pt2);

		double dist, dist2;
		bool pt1_nearby_endpt = true;

		dist = RMATH2DOBJECT->Pt2Line_Dist(pt1[0], pt1[1], line1->Angle(), line1->Intercept());
		arc_param[3] = pt2[0]; arc_param[4] = pt2[1];
		dist2 = RMATH2DOBJECT->Pt2Line_Dist(pt2[0], pt2[1], line1->Angle(), line1->Intercept());
		if (dist > dist2)
		{
			arc_param[3] = pt1[0]; arc_param[4] = pt1[1];
			pt1_nearby_endpt = false;
		}

		double arc_out[3] = {0};
		if (!BESTFITOBJECT->ArcTangentToLine(line_param, arc_param, arc_out))
		{
			return;
		}
		
		//set current arc parameters
		this->center.set(arc_out[0], arc_out[1], MAINDllOBJECT->getCurrentDRO().getZ());
		this->Radius(arc_out[2]); this->OriginalRadius(arc_out[2]);
		
		double endpt_on_line[2] = {0};
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(line1->Angle(), line1->Intercept(), arc_out, endpt_on_line);
		if (pt1_nearby_endpt)
		{
			this->endpoint1.set(endpt_on_line[0], endpt_on_line[1], MAINDllOBJECT->getCurrentDRO().getZ());
			double currstartangle = RMATH2DOBJECT->ray_angle(arc_out, endpt_on_line);
			this->Startangle(currstartangle);
			double currendangle = RMATH2DOBJECT->ray_angle(arc_out, pt2);
			if (currendangle < currstartangle)
				currendangle += 2 * M_PI;
			this->Sweepangle(currendangle - currstartangle);
			this->endpoint2.set(pt2[0], pt2[1], MAINDllOBJECT->getCurrentDRO().getZ());
		}
		else
		{
			this->endpoint2.set(endpt_on_line[0], endpt_on_line[1], MAINDllOBJECT->getCurrentDRO().getZ());
			double currstartangle = RMATH2DOBJECT->ray_angle(arc_out, pt1);
			this->Startangle(currstartangle);
			double currendangle = RMATH2DOBJECT->ray_angle(arc_out, endpt_on_line);
			if (currendangle < currstartangle)
				currendangle += 2 * M_PI;
			this->Sweepangle(currendangle - currstartangle);
			this->endpoint1.set(pt1[0],pt1[1], MAINDllOBJECT->getCurrentDRO().getZ());
		}
		calculateAttributes();

		return;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0029a", __FILE__, __FUNCSIG__);
	}
}

void Circle::updateProjectedCircle(int PointsCnt)
{
		//if (PointsCnt < 3)	return;
		double ans[7] = {0};
		ShapeWithList* ParentShape = (ShapeWithList*)(*this->getParents().begin());
		if(ParentShape == NULL) return;
		if (!BESTFITOBJECT->ThreeDCircle_BestFit(pts, PointsCnt, &ans[0]))
		{
			if(Radius() > 0)
				this->IsValid(true);
			else 
				this->IsValid(false);
			//return;
		}
		
		this->center.set(ans[0], ans[1], ans[2]);
		this->Radius(ans[6]);		
		this->dir_l(ans[3]); this->dir_m(ans[4]); this->dir_n(ans[5]);		
		//ProbePointCalculations(PointsCnt);
		double planeparam[4] = {0};		
		((Plane*)ParentShape)->getParameters(planeparam);		
		double *pntslocal = new double[PointsCnt];		
		for(int i = 0; i < PointsCnt; i++)
		{
		   RMATH3DOBJECT->Projection_Point_on_Plane(&pts[3 * i], planeparam, &pntslocal[3 * i]);	
		}		
		if (!BESTFITOBJECT->ThreeDCircle_BestFit(pntslocal, PointsCnt, &ans[0]))
		{
			if(Radius() > 0)
				this->IsValid(true);
			else 
				this->IsValid(false);
			//return;
		}
		this->center.set(ans[0], ans[1], ans[2]);
		this->Radius(ans[6]);
		this->dir_l(ans[3]); this->dir_m(ans[4]); this->dir_n(ans[5]);
		this->IsValid(true);
}

void Circle::updateLineArcLineShape()
{
	try
	{
		this->IsValid(false);
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Shape* Csh1 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		Shape* Csh2 = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		Shape* Csh3 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];

		//call function to do line-arc-line update
		PointCollection PtColl;

		if (((ShapeWithList*)Csh2)->PointsList->Pointscount() < 3)
		{
			PtColl.Addpoint(new SinglePoint((((Circle*) Csh2)->getendpoint1())->getX(), (((Circle*) Csh2)->getendpoint1())->getY(), (((Circle*) Csh2)->getendpoint1())->getZ()));
			PtColl.Addpoint(new SinglePoint((((Circle*) Csh2)->getMidPoint())->getX(), (((Circle*) Csh2)->getMidPoint())->getY(), (((Circle*) Csh2)->getMidPoint())->getZ()));
			PtColl.Addpoint(new SinglePoint((((Circle*) Csh2)->getendpoint2())->getX(), (((Circle*) Csh2)->getendpoint2())->getY(), (((Circle*) Csh2)->getendpoint2())->getZ()));
		}
		else
			PtColl.CopyAllPoints(((ShapeWithList*)Csh2)->PointsList);

		double* pts = (double*) malloc(2*sizeof(double)*PtColl.Pointscount());
		memset(pts, 0, 2*sizeof(double)*PtColl.Pointscount());
		int i = 0;
		for(PC_ITER PtItem = PtColl.getList().begin(); PtItem != PtColl.getList().end(); PtItem++)
		{
			SinglePoint* Spt = PtItem->second;
			*(pts + 2*i + 0) = Spt->X;
			*(pts + 2*i + 1) = Spt->Y;
			i++;
		}

		double line1[2] = {((Line*) Csh1)->Angle(), ((Line*) Csh1)->Intercept()};
		double line2[2] = {((Line*) Csh3)->Angle(), ((Line*) Csh3)->Intercept()};

		double circle[3] = {0};
		(((Circle*) Csh2)->getCenter())->FillDoublePointer(circle);
		circle[2] = ((Circle*) Csh2)->Radius();

		//populate answer[0], answer[1] with projection of center of arc on angle bisector, answer[2] with slope of angle bisector and answer[3] with radius of arc - these are init guess values.

		double answer[4] = {0};
		double intercept = 0;
		RMATH2DOBJECT->Angle_bisector(line1[0], line1[1], line2[0], line2[1], circle, &answer[2], &intercept);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(answer[2], intercept, circle, answer);
		answer[3] = circle[2];

		BESTFITOBJECT->LineArcLine_BestFit(pts,PtColl.Pointscount(),answer, true, true);
		free (pts);
		
		double line1_endpt[2];
		double line2_endpt[2];
		double line1_endpts[6];
		((Line*) Csh1)->getEndPoints( line1_endpts);
		double line2_endpts[6];
		((Line*) Csh3)->getEndPoints( line2_endpts);
		double circ_midpt[2];
		((Circle*) Csh2)->getMidPoint()->FillDoublePointer(circ_midpt);
		double linearc_pt1[2] = {0};
		double linearc_pt2[2] = {0};
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(line1[0], line1[1], answer, linearc_pt1);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(line2[0], line2[1], answer, linearc_pt2);
		double arcmidpt[2] = {0};
		RMATH2DOBJECT->Point_onCircle(answer, answer[3], circ_midpt, arcmidpt);

		double line_endpts[6];
		((Line*) Csh1)->getEndPoints (line_endpts);
		bool flag1 = false;
		if (!(RMATH2DOBJECT->MousePt_OnFiniteline(line_endpts, line_endpts+3, line1[0], line1[1], linearc_pt1, MAINDllOBJECT->getWindow(0).getUppX(), &flag1)))
		{
			PtColl.EraseAll();
			PtColl.Addpoint(new SinglePoint(linearc_pt1[0], linearc_pt1[1], line_endpts[2]));
			((Line*)Csh1)->AddPoints(&PtColl);
		}
		((Line*) Csh3)->getEndPoints (line_endpts);
		if (!(RMATH2DOBJECT->MousePt_OnFiniteline(line_endpts, line_endpts+3, line2[0], line2[1], linearc_pt2, MAINDllOBJECT->getWindow(0).getUppX(), &flag1)))
		{
			PtColl.EraseAll();
			PtColl.Addpoint(new SinglePoint(linearc_pt2[0], linearc_pt2[1], line_endpts[2]));
			((Line*)Csh3)->AddPoints(&PtColl);
		}

		this->ResetCurrentPointsList();

		PtColl.EraseAll();
		PtColl.Addpoint(new SinglePoint(linearc_pt1[0], linearc_pt1[1], line_endpts[2]));
		PtColl.Addpoint(new SinglePoint(arcmidpt[0], arcmidpt[1], line_endpts[2]));
		PtColl.Addpoint(new SinglePoint(linearc_pt2[0], linearc_pt2[1], line_endpts[2]));
		this->AddPoints(&PtColl);

		this->notifyAll(ITEMSTATUS::DATACHANGED, this);
		Csh2->IsValid(false);
		this->IsValid(true);
	}
	catch(...)
	{
		 MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0015", __FILE__, __FUNCSIG__);
	}
}

void Circle::calculateAttributes()
{
	try
	{
		if(this->CircleType == RapidEnums::CIRCLETYPE::CIRCLE3DCIR)
		{
			int PointCnt = this->PointsList->Pointscount();
			this->MinInscribedia(Radius() * 2);
			this->MaxCircumdia(Radius() * 2);
			if(PointCnt > 3)
			{
				double Npt[3], Fpt[3], Cpt[3];
				Cpt[0] = this->center.getX();
				Cpt[1] = this->center.getY();
				Cpt[2] = this->center.getZ();
				RMATH3DOBJECT->Nearest_Point_to_Point(this->filterpts, filterptsCnt, &Cpt[0],  &Npt[0]);
				RMATH3DOBJECT->Farthest_Point_to_Point(this->filterpts, filterptsCnt, &Cpt[0],  &Fpt[0]);
				MinInscribedia(2 * RMATH3DOBJECT->Distance_Point_Point(Cpt, Npt));
				MaxCircumdia(2 * RMATH3DOBJECT->Distance_Point_Point(Cpt, Fpt));
			}
			this->Circularity((MaxCircumdia() - MinInscribedia())/2);
		}
		else if (this->CircleType != RapidEnums::CIRCLETYPE::ARC_INTWOARC)
		{
			double sl1, sl2, nearpt[2], farpt[2];
			double tp1[2] = {endpoint1.getX(), endpoint1.getY()}, tp2[2] = {endpoint2.getX(), endpoint2.getY()}, cen[2] = {center.getX(), center.getY()};
			sl1 = RMATH2DOBJECT->ray_angle(&cen[0], &tp1[0]);
			sl2 = RMATH2DOBJECT->ray_angle(&cen[0], &tp2[0]);
			Startangle(sl1);
			if((sl2 - sl1) > 0)
				Sweepangle(sl2 - sl1);
			else
				Sweepangle((sl2 - sl1) + 2 * M_PI);
			double Midangle = Startangle() + Sweepangle()/2;
			this->Midpoint.set(center.getX() + Radius() * cos(Midangle), center.getY() + Radius() * sin(Midangle), (endpoint1.getZ() + endpoint2.getZ())/2);
			this->MinInscribedia(Radius() * 2);
			this->MaxCircumdia(Radius() * 2);
			int PointCnt = this->PointsList->Pointscount();
			if(PointCnt > 3)
			{
				RMATH2DOBJECT->Nearestpoint_point(filterptsCnt, this->filterpts, &cen[0],  &nearpt[0]);
				RMATH2DOBJECT->Farthestpoint_point(filterptsCnt , this->filterpts, &cen[0],  &farpt[0]);
				MinInscribedia(2 * RMATH2DOBJECT->Pt2Pt_distance(cen[0], cen[1], nearpt[0], nearpt[1]));
				MaxCircumdia(2 * RMATH2DOBJECT->Pt2Pt_distance(cen[0], cen[1], farpt[0], farpt[1]));
			}
			this->Circularity((MaxCircumdia() - MinInscribedia())/2);
			this->Length(Sweepangle() * Radius());
		
			if(this->ShapeForCurrentFastrace())
			{
				if(CircleType ==  RapidEnums::CIRCLETYPE::ARC_MIDDLE)
					this->Midpoint.set(pts[4], pts[5], 0);
				else
					this->Midpoint.set(pts[2], pts[3], 0);
			}

			/*if(this->ShapeForCurrentFastrace() || this->ShapeForPerimeter())
			{
				if(CircleType ==  RapidEnums::CIRCLETYPE::ARC_MIDDLE)
					this->Midpoint.set(pts[4], pts[5], 0);
				else
					this->Midpoint.set(pts[2], pts[3], 0);
			}*/
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0017", __FILE__, __FUNCSIG__); }
}

void Circle::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
				{
					double Circle3dparams[7] = {center.getX(), center.getY(), center.getZ(), dir_l(), dir_m(), dir_n(), Radius()};
					GRAFIX->drawCircle3D(Circle3dparams);
					GRAFIX->drawPoint(center.getX(), center.getY(), center.getZ());
					if(this->HighlightedFormouse() && CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_2LINES)
						drawExtensionForTangentialCircle();
					break;
				}
		/*	case RapidEnums::SHAPETYPE::ARC:*/
				/*GRAFIX->drawArc(center.getX(), center.getY(), Radius(), Startangle(), Sweepangle(), WPixelWidth);
				break;*/
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				{
					double Circle3dparams[7] = {center.getX(), center.getY(), center.getZ(), dir_l(), dir_m(), dir_n(), Radius()};
					GRAFIX->drawCircle3D(Circle3dparams);
					GRAFIX->drawPoint(center.getX(), center.getY(), center.getZ());
					if(this->HighlightedFormouse() && CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_2LINES)
						drawExtensionForTangentialCircle();
				}
				break;
			case RapidEnums::SHAPETYPE::ARC:
			case RapidEnums::SHAPETYPE::ARC3D:
				{
					double Arc3dparams[7] = {center.getX(), center.getY(), center.getZ(), dir_l(), dir_m(), dir_n(), Radius()};
					GRAFIX->drawShapeArc3D(Arc3dparams,Startangle(), Sweepangle(), WPixelWidth);
				}
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0018", __FILE__, __FUNCSIG__); }	
}

void Circle::drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM)
{
	try
	{
		double temp1[4] = {center.getX(), center.getY(), center.getZ(), 1}, tempCen[4], temp2[4] = {endpoint1.getX(), endpoint1.getY(), endpoint1.getZ(), 1}, tempendpt1[4]; 
		double temp3[4] = {endpoint2.getX(), endpoint2.getY(), endpoint2.getZ(), 1}, tempendpt2[4], temp4[4] = {Midpoint.getX(), Midpoint.getY(), Midpoint.getZ(), 1}, tempmidpt[4];
		double tmprad, tmpstrtang, tmpsweepang;
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransFormM, Order1, temp2, Order2, tempendpt1);
		RMATH2DOBJECT->MultiplyMatrix1(TransFormM, Order1, temp3, Order2, tempendpt2);
		RMATH2DOBJECT->MultiplyMatrix1(TransFormM, Order1, temp4, Order2, tempmidpt);
		RMATH2DOBJECT->Arc_3Pt(tempendpt1, tempendpt2, tempmidpt, tempCen, &tmprad, &tmpstrtang, &tmpsweepang);
		RMATH2DOBJECT->MultiplyMatrix1(TransFormM, Order1, temp1, Order2, tempCen);
		switch(this->ShapeType)
		{
			/*case RapidEnums::SHAPETYPE::CIRCLE:
				GRAFIX->drawCircle(tempCen[0], tempCen[1], Radius());
				GRAFIX->drawPoint(tempCen[0], tempCen[1], tempCen[2]);*/
		/*	case RapidEnums::SHAPETYPE::ARC:
				GRAFIX->drawArc(tempCen[0], tempCen[1], Radius(), tmpstrtang, tmpsweepang, WPixelWidth);
				break;*/
			case RapidEnums::SHAPETYPE::CIRCLE:
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				{
					double Circle3dparams[7] = {tempCen[0], tempCen[1], tempCen[2], dir_l(), dir_m(), dir_n(), Radius()};
					GRAFIX->drawCircle3D(Circle3dparams);
					GRAFIX->drawPoint(tempCen[0], tempCen[1], tempCen[2]);
				}
				break;
			case RapidEnums::SHAPETYPE::ARC:
			case RapidEnums::SHAPETYPE::ARC3D:
				{
					double Arc3dparams[7] = {tempCen[0], tempCen[1], tempCen[2], dir_l(), dir_m(), dir_n(), Radius()};
					GRAFIX->drawShapeArc3D(Arc3dparams,Startangle(), Sweepangle(), WPixelWidth);
				}
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0018", __FILE__, __FUNCSIG__); }	
}

void Circle::drawGDntRefernce()
{
	try
	{
		if(RefernceDatumForgdnt() > 0)
		{
			double midp[3] = {center.getX(), center.getY(), center.getZ()};
			GRAFIX->drawReferenceDatumName_Circle(&midp[0], Radius(), &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0019", __FILE__, __FUNCSIG__); }
}

void Circle::ResetShapeParameters()
{
	try
	{
		this->Radius(0); this->Startangle(0); this->Sweepangle(0); this->center.set(0, 0, 0);
		this->MinInscribedia(0); this->MaxCircumdia(0); this->Circularity(0); this->Length(0);
		this->dir_l(0); this->dir_m(0); this->dir_n(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0020", __FILE__, __FUNCSIG__); }
}

void Circle::drawExtensionForTangentialCircle()
{
	try
	{
		Shape *Cshape[2];
		list<BaseItem*>::iterator ptr = this->getParents().begin(); 
		Cshape[0] = (Shape*)(*ptr++);
		Cshape[1] = (Shape*)(*ptr);
		double angle[2], intercept[2];
		angle[0] = ((Line*)Cshape[0])->Angle(); angle[1] = ((Line*)Cshape[1])->Angle();
		intercept[0] = ((Line*)Cshape[0])->Intercept(); intercept[1] = ((Line*)Cshape[1])->Intercept();
		double c[2] = {center.getX(), center.getY()}, rad = Radius();
		double Line1point1[2] = {((Line*)Cshape[0])->getPoint1()->getX(), ((Line*)Cshape[0])->getPoint1()->getY()};
		double Line1point2[2] = {((Line*)Cshape[0])->getPoint2()->getX(), ((Line*)Cshape[0])->getPoint2()->getY()};
		double Line2point1[2] = {((Line*)Cshape[1])->getPoint1()->getX(), ((Line*)Cshape[1])->getPoint1()->getY()};
		double Line2point2[2] = {((Line*)Cshape[1])->getPoint2()->getX(), ((Line*)Cshape[1])->getPoint2()->getY()};
		double intersect1[4], intersect2[4], topleft[2], bottomRight[2];
		int Cnt[2];
		bool drawExtensionline[2];
		if(Cshape[0]->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			RMATH2DOBJECT->Finiteline_circle(&Line1point1[0], &Line1point2[0], angle[0], intercept[0], &c[0], rad, &intersect1[0], &intersect2[0], &Cnt[0]);
			if(Cnt[1] == 1) drawExtensionline[0] = true;
			else drawExtensionline[0] = false;
			if(drawExtensionline[0])
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line1point1[0], Line1point1[1]) < RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line1point2[0], Line1point2[1]))
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line1point1[0], Line1point1[1]);
				else
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line1point2[0], Line1point2[1]);
			}
		}
		Cnt[0] = 0; Cnt[1] = 0;
		if(Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			RMATH2DOBJECT->Finiteline_circle(&Line2point1[0], &Line2point2[0], angle[1], intercept[1], &c[0], rad, &intersect1[0], &intersect2[0], &Cnt[0]);
			if(Cnt[1] == 1) drawExtensionline[1] = true;
			else drawExtensionline[1] = false;
			if(drawExtensionline[1])
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line2point1[0], Line2point1[1]) < RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line2point2[0], Line2point2[1]))
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line2point1[0], Line2point1[1]);
				else
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line2point2[0], Line2point2[1]);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0021", __FILE__, __FUNCSIG__); }
}

Shape* Circle::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
		{
			if(this->ShapeType == RapidEnums::SHAPETYPE::ARC)
				myname = _T("A");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
				myname = _T("C");
			if(this->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
				myname = _T("A3D");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
				myname = _T("C3D");
		}
		else
		{
			if(this->ShapeType == RapidEnums::SHAPETYPE::ARC)
				myname = _T("dA");
			else
				myname = _T("dC");
			if(this->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
				myname = _T("dA3D");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
				myname = _T("dC3D");
		}
		Circle* CShape = new Circle((TCHAR*)myname.c_str(), this->ShapeType);
		if(copyOriginalProperty)
		   CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0023", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Circle::CreateDummyCopy()
{
	try
	{
		Circle* CShape = new Circle(false, this->ShapeType);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0024", __FILE__, __FUNCSIG__); return NULL; }
}

void Circle::CopyShapeParameters(Shape* s)
{
	try
	{
		this->center.set(*((Circle*)s)->getCenter());
		this->endpoint1.set(*((Circle*)s)->getendpoint1());
		this->endpoint2.set(*((Circle*)s)->getendpoint2());
		this->Midpoint.set(*((Circle*)s)->getMidPoint());
		this->Radius(((Circle*)s)->Radius());
		this->Startangle(((Circle*)s)->Startangle());
		this->Sweepangle(((Circle*)s)->Sweepangle());
		this->dir_l(((Circle*)s)->dir_l());
		this->dir_m(((Circle*)s)->dir_m());
		this->dir_n(((Circle*)s)->dir_n());
		if(this->getMCSParentShape() != NULL)
		{
			if(this->ShapeType ==  RapidEnums::SHAPETYPE::CIRCLE)
				this->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
			else if(this->ShapeType ==  RapidEnums::SHAPETYPE::CIRCLE3D)
				this->CircleType = RapidEnums::CIRCLETYPE::CIRCLE3DCIR;
			else if(this->ShapeType ==  RapidEnums::SHAPETYPE::ARC3D)
				this->CircleType = RapidEnums::CIRCLETYPE::ARC3DCIR;
			else if(this->ShapeType ==  RapidEnums::SHAPETYPE::ARC)
				this->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
		}
		else
			this->CircleType = ((Circle*)s)->CircleType;
		this->calculateAttributes();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0025", __FILE__, __FUNCSIG__); }
}

void Circle::Transform(double* transform)
{
	try
	{
		setCenter(MAINDllOBJECT->TransformMultiply(transform, center.getX(), center.getY(),  center.getZ()));
		setendpoint1(MAINDllOBJECT->TransformMultiply(transform, endpoint1.getX(), endpoint1.getY(), endpoint1.getZ()));
		setendpoint2(MAINDllOBJECT->TransformMultiply(transform, endpoint2.getX(), endpoint2.getY(), endpoint2.getZ()));
		calculateAttributes();
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0026", __FILE__, __FUNCSIG__); }
}

void Circle::Translate(Vector& Position)
{
	center += Position;
	endpoint1 += Position;
	endpoint2 += Position;
	calculateAttributes();
	this->UpdateEnclosedRectangle();
}

void Circle::AlignToBasePlane(double* trnasformM)
{
	try
	{
		double ans[7] = {0}, endPnts[9] = {0}, tranformPntPnts[10] = {0}, cen[3] = {center.getX(), center.getY(), center.getZ()}, tmpdir[3] = {1, 0, 0}, tmpdir1[3] = {0}, tmpdir2[3] = {0}, cirDir[3] = {dir_l(), dir_m(), dir_n()};
		if(!(RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(cirDir, tmpdir, tmpdir1)))
		{
			 tmpdir[0] = 0;
			 tmpdir[1] = 1;
			 RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(cirDir, tmpdir, tmpdir1);
		}
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(cirDir, tmpdir1, tmpdir2);
		for(int i = 0; i < 3; i++)
		{
			endPnts[i] = cen[i] + Radius() * tmpdir1[i];
			endPnts[3 + i] = cen[i] - Radius() * tmpdir1[i];
			endPnts[6 + i] = cen[i] + Radius() * tmpdir2[i];
		}
		setCenter(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, cen[0], cen[1], cen[2]));
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		for(int i = 0; i < 3; i++)
		{
			double p[4] = {endPnts[3 * i], endPnts[3 * i + 1], endPnts[3 * i + 2], 1};
			RMATH2DOBJECT->MultiplyMatrix1(trnasformM, &s1[0], &p[0], &s2[0], &tranformPntPnts[3 * i]);
		}
		BESTFITOBJECT->ThreeDCircle_BestFit(tranformPntPnts, 3, ans);
		this->dir_l(ans[3]); this->dir_m(ans[4]); this->dir_n(ans[5]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0026", __FILE__, __FUNCSIG__); }
}

void Circle::UpdateEnclosedRectangle()
{
	try
	{
		double cen[3] = {0}, TransMatrix[9] = {0};
		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		center.FillDoublePointer(&cen[0], 3);
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{		 
			 double temp_cen[3] = {cen[0], cen[1], cen[2]};
			 RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			 TransMatrix[2] = 0; TransMatrix[5] = 0;
			 RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_cen, Order2, cen);		
		}
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
			case RapidEnums::SHAPETYPE::CIRCLE3D:				
				RMATH2DOBJECT->RectangleEnclosing_Circle(&cen[0], Radius(), &ShapeLeftTop[0], &ShapeRightBottom[0]);				
				if (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
				{
					////RMATH2DOBJECT->Rotate_2D_Pt(ShapeLeftTop, cen, MAINDllOBJECT->getCurrentUCS().UCSangle(), ShapeLeftTop);
					////RMATH2DOBJECT->Rotate_2D_Pt(ShapeRightBottom, cen, -MAINDllOBJECT->getCurrentUCS().UCSangle(), ShapeRightBottom);
					//double temp_cen[3] = { ShapeLeftTop[0], ShapeLeftTop[1], ShapeLeftTop[2] };
					//double endp1[3] = { 0 }, endp2[3] = { 0 };
					//RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
					//RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_cen, Order2, endp1);
					//for (int i = 0; i < 3; i++)
					//{
					//	ShapeLeftTop[i] = cen[i];
					//	temp_cen[i] = ShapeRightBottom[i]; //Upload the next point
					//}
					//RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_cen, Order2, endp2);
					//RMATH2DOBJECT->RectangleEnclosing_Line(&endp1[0], &endp2[0], &ShapeLeftTop[0], &ShapeRightBottom[0]);
				}
				break;
			case RapidEnums::SHAPETYPE::ARC:
			case RapidEnums::SHAPETYPE::ARC3D:
				{
					double endp1[3] = {0}, endp2[3] = {0};
					endpoint1.FillDoublePointer(&endp1[0], 3); endpoint2.FillDoublePointer(&endp2[0], 3);
					if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
					{		 
						RMATH2DOBJECT->RectangleEnclosing_Circle(&cen[0], Radius(), &ShapeLeftTop[0], &ShapeRightBottom[0]);
						//RMATH2DOBJECT->RectangleEnclosing_Arc(&cen[0], &endp1[0], &endp2[0], Radius(), Startangle() - MAINDllOBJECT->getCurrentUCS().UCSangle(), 
						//	Startangle() - MAINDllOBJECT->getCurrentUCS().UCSangle() + Sweepangle(), &ShapeLeftTop[0], &ShapeRightBottom[0]);

						//RMATH2DOBJECT->Rotate_2D_Pt(ShapeLeftTop, cen, MAINDllOBJECT->getCurrentUCS().UCSangle(), ShapeLeftTop);
						//RMATH2DOBJECT->Rotate_2D_Pt(ShapeRightBottom, cen, -MAINDllOBJECT->getCurrentUCS().UCSangle(), ShapeRightBottom);
						//double temp_endpt1[3] = {endp1[0], endp1[1], endp1[2]}, temp_endpt2[3] = {endp2[0], endp2[1], endp2[2]};
						// RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_endpt1, Order2, endp1);
						// RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_endpt2, Order2, endp2);						
						// RMATH2DOBJECT->RectangleEnclosing_Line(&endp1[0], &endp2[0], &ShapeLeftTop[0], &ShapeRightBottom[0]);
					}
					else
						RMATH2DOBJECT->RectangleEnclosing_Arc(&cen[0], &endp1[0], &endp2[0], Radius(), Startangle(), Startangle() + Sweepangle(), &ShapeLeftTop[0], &ShapeRightBottom[0]);
				}
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0026", __FILE__, __FUNCSIG__); }
}

void Circle::GetShapeCenter(double *cPoint)
{
	cPoint[0] = center.getX();
	cPoint[1] = center.getY();
	cPoint[2] = center.getZ();
}

int Circle::Cshapenumber = 0;
int Circle::Ashapenumber = 0;
int Circle::CDXFshapenumber = 0;
int Circle::ADXFshapenumber = 0;

void Circle::reset()
{
	Cshapenumber = 0; Ashapenumber = 0;
	CDXFshapenumber = 0; ADXFshapenumber = 0;
}

wostream& operator<<(wostream& os, Circle& x)
{
	try
	{
		os.precision(16);
		if(x.ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		{
			os << "ShapeType:" << x.ShapeType << endl;
			os << x.CircleType << endl;
		}
		os << (*static_cast<Shape*>(&x));
		os<< "Circle" <<endl;
		os << "ShapeType:" << x.ShapeType << endl;
		os << "endpoint1:values"<< endl << (*static_cast<Vector*>(x.getendpoint1())) << endl;
		os << "endpoint2:values"<< endl << (*static_cast<Vector*>(x.getendpoint2())) << endl;
		os << "Center:values"<< endl << (*static_cast<Vector*>(x.getCenter())) << endl ;
		os << "Radius:" << x.Radius() << endl;
		os << "Startangle:" << x.Startangle() << endl;
		os << "Sweepangle:" << x.Sweepangle() << endl;
		os << "dir_l:" << x.dir_l() << endl;
		os << "dir_m:" << x.dir_m() << endl;
		os << "dir_n:" << x.dir_n() << endl;
		os << "CircleType:" << x.CircleType << endl;
		os << "SetAsCriticalShape:" << x.SetAsCriticalShape() << endl;
		os << "OffsetDefined:" << x.OffsetDefined << endl;
		os << "Offset:" << x.Offset() << endl;
		os<< "EndCircle" <<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0027", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Circle& x)
{
	try
	{
		is >> (*(Shape*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"Circle")
			{
			while(Tagname!=L"EndCircle")
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
						if(Tagname==L"ShapeType")
						{
							x.ShapeType = RapidEnums::SHAPETYPE(atoi((const char*)(Val).c_str()));						
						}
						else if(Tagname==L"endpoint1")
						{
							is >> (*static_cast<Vector*>(x.getendpoint1()));
						}
						else if(Tagname==L"endpoint2")
						{
							is >> (*static_cast<Vector*>(x.getendpoint2()));
						}
						else if(Tagname==L"Center")
						{
							is >> (*static_cast<Vector*>(x.getCenter()));
						}
						else if(Tagname==L"Radius")
						{
							 x.Radius(atof((const char*)(Val).c_str()));
							 x.BuildRadius(x.Radius());
						}
						else if(Tagname==L"Startangle")
						{
							 x.Startangle(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"Sweepangle")
						{
							 x.Sweepangle(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"dir_l")
						{
							 x.dir_l(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"dir_m")
						{
							 x.dir_m(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"dir_n")
						{
							 x.dir_n(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"CircleType")
						{
							 x.CircleType = RapidEnums::CIRCLETYPE(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"SetAsCriticalShape")
						{
							if(Val=="0")
								x.SetAsCriticalShape(false);
							else
								x.SetAsCriticalShape(true);	
						}
						else if(Tagname==L"FilteredArc")
						{
							if(Val=="0")
								x.ApplyFilterCorrection(false);
							else
								x.ApplyFilterCorrection(true);						 
						}
						else if(Tagname==L"OffsetDefined")
						{
							if(Val=="0")
								x.OffsetDefined = false;
							else
								x.OffsetDefined = true;	
						}
						else if(Tagname==L"Offset")
						{
							 x.Offset(atof((const char*)(Val).c_str()));
						}
					}
				}
				x.calculateAttributes();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0028", __FILE__, __FUNCSIG__); return is; }
}

void ReadOldPP(wistream& is, Circle& x)
{
	try
	{
		int n; double d;
		is >> n;
		x.ShapeType = RapidEnums::SHAPETYPE(n);
		is >> (*static_cast<Vector*>(x.getendpoint1()));
		is >> (*static_cast<Vector*>(x.getendpoint2()));
		is >> (*static_cast<Vector*>(x.getCenter()));
		is >> d; x.Radius(d);
		is >> d; x.Startangle(d);
		is >> d; x.Sweepangle(d);
		is >> d; x.dir_l(d);
		is >> d; x.dir_m(d);
		is >> d; x.dir_n(d);
		is >> n;
		x.CircleType = RapidEnums::CIRCLETYPE(n);
		x.calculateAttributes();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0028", __FILE__, __FUNCSIG__); }
}
