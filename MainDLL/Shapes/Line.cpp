#include "StdAfx.h"
#include "Line.h"
#include "TwoLine.h"
#include "TwoArc.h"
#include "Plane.h"
#include "Cylinder.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

Line::Line(TCHAR* myname, RapidEnums::SHAPETYPE Stype):ShapeWithList(genName(myname, Stype))
{
	init(Stype);
}

Line::Line(bool simply, RapidEnums::SHAPETYPE Stype):ShapeWithList(false)
{
	init(Stype);
}

Line::~Line()
{
}

void Line::init(){}
void Line::init(RapidEnums::SHAPETYPE Stype)
{
	try
	{
		this->ShapeType = Stype;
		if(ShapeType == RapidEnums::SHAPETYPE::LINE3D || ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
			this->ShapeAs3DShape(true);
		this->Angle(0); this->Intercept(0); this->Length(0); this->BuildAngle(0);
		this->Straightness(0); this->Offset(0); this->AddIntercept(false); this->SetAsCriticalShape(false);
		this->FilteredStraightness(0); 
		this->dir_l(0); this->dir_m(0); this->dir_n(0);
		this->LinePosition(0);
		this->DefinedOffset(false);
		this->DefinedLength(false);
		this->EndPointsFixed(false);
		this->FastraceType_Mid(true);
		this->ScanDirDetected(false);
		VPointer = new Vector(0, 0, 0);
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				//MAINDllOBJECT->SetAndRaiseErrorMessage("Line", "initWithShape", __FUNCSIG__);
				this->LineType = RapidEnums::LINETYPE::FINITELINE;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point1), 0), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point2), 1), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2), 2);
				MAINDllOBJECT->SetAndRaiseErrorMessage("Line", __FILE__, __FUNCSIG__);
				break;
			case RapidEnums::SHAPETYPE::LINE3D:
				this->LineType = RapidEnums::LINETYPE::FINITELINE3D;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point1), 0), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point2), 1), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2), 2);
				MAINDllOBJECT->SetAndRaiseErrorMessage("Line3D", __FILE__, __FUNCSIG__);
				break;
			case RapidEnums::SHAPETYPE::XLINE3D:
				this->LineType = RapidEnums::LINETYPE::INFINITELINE3D;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point1), 0), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point2), 1), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2), 2);
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				this->LineType = RapidEnums::LINETYPE::RAYLINE;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point1), 0, true), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point2), 1, false), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2, false), 2);
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				this->LineType = RapidEnums::LINETYPE::INFINITELINE;
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point1), 0, false), 0);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point2), 1, false), 1);
				this->SnapPointList->Addpoint(new SnapPoint(this, &(this->Midpoint), 2, false), 2);
				break;
		}
		this->ProbeCorrectionShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0001", __FILE__, __FUNCSIG__); }
}

TCHAR* Line::genName(const TCHAR* prefix, RapidEnums::SHAPETYPE Stype)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10]; int Cnt;
		if(strcmp((const char*)prefix, "d") == 0)
		{
			if(Stype == RapidEnums::SHAPETYPE::LINE)
				Cnt = LDXFshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::LINE3D)
				Cnt = L3dDXFshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::XLINE3D)
				Cnt = XL3dDXFshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::XLINE)
				Cnt = XLDXFshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::XRAY)
				Cnt = RDXFshapenumber++;
		}
		else
		{
			if(Stype == RapidEnums::SHAPETYPE::LINE)
				Cnt = Lshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::LINE3D)
				Cnt = L3dshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::XLINE3D)
				Cnt = XL3dshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::XLINE)
				Cnt = XLshapenumber++;
			else if(Stype == RapidEnums::SHAPETYPE::XRAY)
				Cnt = Rshapenumber++;
		}
		Cnt++;
		_itot_s(Cnt, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0002", __FILE__, __FUNCSIG__); return name; }
}

bool Line::Shape_IsInWindow(Vector& corner1,double Tolerance)
{
	try
	{
		double endp1[2] = {point1.getX(), point1.getY()}, endp2[2] = {point2.getX(), point2.getY()}, mp[2] = {corner1.getX(), corner1.getY()};
		bool flag = false, flag1 = false;
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				flag = RMATH2DOBJECT->MousePt_OnFiniteline(&endp1[0], &endp2[0], Angle(), Intercept(), &mp[0], Tolerance, &flag1);
				MAINDllOBJECT->ShapeHighlightedForImplicitSnap(flag1);
				if(MAINDllOBJECT->ShapeHighlightedForImplicitSnap() && MAINDllOBJECT->ShowImplicitIntersectionPoints()) return true;
				else MAINDllOBJECT->ShapeHighlightedForImplicitSnap(false);
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				flag = RMATH2DOBJECT->MousePt_OnInfiniteline(Angle(), Intercept(), &mp[0], Tolerance);
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				flag = RMATH2DOBJECT->MousePt_OnRay(&endp1[0], Angle(), Intercept(), &mp[0], Tolerance);
				break;
			case RapidEnums::SHAPETYPE::LINE3D:
			case RapidEnums::SHAPETYPE::XLINE3D:
				{
					double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
					flag = Shape_IsInWindow(&SelectionL[0], Tolerance);
				}
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0003", __FILE__, __FUNCSIG__); return false; }
}

bool Line::Shape_IsInWindow(Vector& corner1, double Tolerance, double* TransformM)
{
try
	{
		double tempSlope, tempIntercept, temp1[4] = {point1.getX(), point1.getY(), point1.getZ(), 1},  temp2[4], temp3[4] = {point2.getX(), point2.getY(), point2.getZ(), 1},  temp4[4]; 
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp1, Order2, temp2);
		RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp3, Order2, temp4);
		RMATH2DOBJECT->TwoPointLine(temp2, temp4, &tempSlope, &tempIntercept);
		double mp[2] = {corner1.getX(), corner1.getY()};
		bool flag = false, flag1 = false;
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				flag = RMATH2DOBJECT->MousePt_OnFiniteline(&temp2[0], &temp4[0], tempSlope, tempIntercept, &mp[0], Tolerance, &flag1);
				MAINDllOBJECT->ShapeHighlightedForImplicitSnap(flag1);
				if(MAINDllOBJECT->ShapeHighlightedForImplicitSnap() && MAINDllOBJECT->ShowImplicitIntersectionPoints()) return true;
				else MAINDllOBJECT->ShapeHighlightedForImplicitSnap(false);
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				flag = RMATH2DOBJECT->MousePt_OnInfiniteline(Angle(), Intercept(), &mp[0], Tolerance);
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				flag = RMATH2DOBJECT->MousePt_OnRay(&temp2[0], Angle(), Intercept(), &mp[0], Tolerance);
				break;
			case RapidEnums::SHAPETYPE::LINE3D:
			case RapidEnums::SHAPETYPE::XLINE3D:
				{
					double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
					flag = Shape_IsInWindow(&SelectionL[0], Tolerance);
				}
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0003", __FILE__, __FUNCSIG__); return false; }
}

bool Line::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	try
	{
		double p1[2] = {corner1.getX(), corner1.getY()}, p2[2] = {corner2.getX(), corner1.getY()};
		double p3[2] = {corner2.getX(), corner2.getY()}, p4[2] = {corner1.getX(), corner2.getY()};
		double endp1[2] = {point1.getX(), point1.getY()}, endp2[2] = {point2.getX(), point2.getY()};
		bool flag = false;
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				flag = RMATH2DOBJECT->Fline_isinwindow(&endp1[0], &endp2[0], Angle(), Intercept(), &p1[0], &p2[0], &p3[0], &p4[0]);
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				flag = RMATH2DOBJECT->Inline_isinwindow(Angle(), Intercept(), &p1[0], &p2[0], &p3[0], &p4[0]);
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				flag = RMATH2DOBJECT->ray_isinwindow(&endp1[0], Angle(),Intercept(), &p1[0], &p2[0], &p3[0], &p4[0]);
				break;
			case RapidEnums::SHAPETYPE::LINE3D:
			case RapidEnums::SHAPETYPE::XLINE3D:
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0004", __FILE__, __FUNCSIG__); return false; }
}

bool Line::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		double TheLine[6] = {point1.getX(), point1.getY(), point1.getZ()};
		double dist, LnEndPoints[6] = {point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ()};
		double Ratios[3] = {point1.getX() - point2.getX(), point1.getY() - point2.getY(), point1.getZ() - point2.getZ()};
		RMATH3DOBJECT->DirectionCosines(&Ratios[0], &TheLine[3]);
		dist = RMATH3DOBJECT->Distance_Line_Line(SelectionLine, &TheLine[0]);
		if(dist <= Tolerance)
		{
			double PtsOfClosestDist[6];
			if(RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(TheLine, SelectionLine, PtsOfClosestDist))
			{
				RMATH3DOBJECT->Intersection_Line_Line(TheLine, SelectionLine, IntersectnPts);
				this->Intersects = 1;
				if(this->ShapeType == RapidEnums::SHAPETYPE::LINE3D || this->ShapeType == RapidEnums::SHAPETYPE::LINE)
					return RMATH3DOBJECT->Checking_Point_within_Block(PtsOfClosestDist, LnEndPoints, 2);
				else
					return true;
			}			
		}	
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0005", __FILE__, __FUNCSIG__); return false; }
}


void Line::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				{
					GRAFIX->drawLine_3D(point1.getX(), point1.getY(),  point1.getZ(), point2.getX(), point2.getY(), point2.getZ());
					if(this->HighlightedFormouse() && LineType == RapidEnums::LINETYPE::TANGENT_THRU_POINT)
					ExtensionForDerivedline();
				}
				break;
			case RapidEnums::SHAPETYPE::LINE3D:
				{
					GRAFIX->drawLine_3D(point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ());
					double mainP[3] = {point1.getX(), point1.getY(), point1.getZ()};
					double tailP[3] = {point2.getX(), point2.getY(), point2.getZ()};
					GRAFIX->DrawArrowCone(mainP, tailP, WPixelWidth);
				}
				break;
			case RapidEnums::SHAPETYPE::XLINE3D:
				{
					double DirCosine[3] = {dir_l(), dir_m(), dir_n()};
					GRAFIX->draw3DXLine(DirCosine, (point1.getX() + point2.getX())/2, (point1.getY() + point2.getY())/2, (point1.getZ() + point2.getZ())/2, WPixelWidth);
					double mainP[3] = {point1.getX(), point1.getY(), point1.getZ()};
					double tailP[3] = {point2.getX(), point2.getY(), point2.getZ()};
					GRAFIX->DrawArrowCone(mainP, tailP, WPixelWidth);
				}
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				{
					double lineparam[6] = {0};
					getParameters(lineparam);
				/*	GRAFIX->drawXLineOrXRay3D(lineparam, WPixelWidth, true, false);*/
					GRAFIX->drawXLineOrXRay(Angle(), (point1.getX() + point2.getX())/2, (point1.getY() + point2.getY())/2, WPixelWidth, true, point1.getZ());
					break; 
				}
			case RapidEnums::SHAPETYPE::XRAY:
				{
					double lineparam[6] = {0};
					getParameters(lineparam);
					GRAFIX->drawXLineOrXRay(Angle(), point1.getX(), point1.getY(), WPixelWidth, false, point1.getZ());
					//GRAFIX->drawXLineOrXRay3D(lineparam, WPixelWidth, false, false);
					break;
				}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0006", __FILE__, __FUNCSIG__); }
}

void Line::drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM)
{
	try
	{ 
		double temp1[4] = {point1.getX(), point1.getY(), point1.getZ(), 1},  temp2[4], temp3[4] = {point2.getX(), point2.getY(), point2.getZ(), 1},  temp4[4]; 
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransFormM, Order1, temp1, Order2, temp2);
		RMATH2DOBJECT->MultiplyMatrix1(TransFormM, Order1, temp3, Order2, temp4);
		switch(this->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
			case RapidEnums::SHAPETYPE::LINE3D:
			case RapidEnums::SHAPETYPE::XLINE3D:
				GRAFIX->drawLine_3D(temp2[0], temp2[1], temp2[2], temp4[0], temp4[1], temp4[2]);
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				GRAFIX->drawXLineOrXRay(Angle(), (temp2[0] + temp4[0])/2, (temp2[1] + temp4[1])/2, WPixelWidth, true, temp2[2]);
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				GRAFIX->drawXLineOrXRay(Angle(), temp2[0], temp2[1], WPixelWidth, false, temp2[2]);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0006", __FILE__, __FUNCSIG__); }
}

void Line::drawGDntRefernce()
{
	try
	{
		if(RefernceDatumForgdnt() > 0)
		{
			if(ShapeType == RapidEnums::SHAPETYPE::LINE3D || ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
			{
				double LineParam[6], midpt[3];
				getParameters(&LineParam[0]);
				Midpoint.FillDoublePointer(&midpt[0], 3);
				GRAFIX->drawReferenceDatumName_Line3D(&LineParam[0], &midpt[0], &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
			}
			else
			{
				if(!Normalshape())
				{
					double midp[2] = {100 * MAINDllOBJECT->getWindow(1).getUppX() * cos(Angle()), 100 * MAINDllOBJECT->getWindow(1).getUppX() * sin(Angle())};
					GRAFIX->drawReferenceDatumName_Line(Angle(), &midp[0], &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
				}
				else
				{
					double midp[2] = {Midpoint.getX(), Midpoint.getY()};
					GRAFIX->drawReferenceDatumName_Line(Angle(), &midp[0], &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0007", __FILE__, __FUNCSIG__); }
}

void Line::ResetShapeParameters()
{
	try
	{
		this->Angle(0); this->Intercept(0); this->Length(0);
		this->Straightness(0); this->Offset(0); this->AddIntercept(false);
		this->FilteredStraightness(0); 
		this->dir_l(0); this->dir_m(0); this->dir_n(0);
		this->LinePosition(0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0008", __FILE__, __FUNCSIG__); }
}

void Line::ExtensionForDerivedline()
{
	try
	{
		double pt1[2] = {point1.getX(), point1.getY()}, pt2[2] = {point2.getX(), point2.getY()};
		double topleft[2], bottomRight[2];
		RMATH2DOBJECT->GetTopLeftNBtnRht(&pt1[0], &pt2[0], &topleft[0], &bottomRight[0]);
		bool drawExtention = !RMATH2DOBJECT->Ptisinwindow(VPointer->getX(), VPointer->getY(), topleft[0], bottomRight[1], bottomRight[0], topleft[1]);
		if(drawExtention)
		{
			if(RMATH2DOBJECT->Pt2Pt_distance(VPointer->getX(), VPointer->getY(), point1.getX(), point1.getY()) < RMATH2DOBJECT->Pt2Pt_distance(VPointer->getX(), VPointer->getY(), point2.getX(), point2.getY()))
				GRAFIX->drawLineStipple(VPointer->getX(), VPointer->getY(), point1.getX(), point1.getY());
			else
				GRAFIX->drawLineStipple(VPointer->getX(), VPointer->getY(), point2.getX(), point2.getY());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0008", __FILE__, __FUNCSIG__); }
}

void Line::UpdateBestFit()
{
	try
	{
		if(this->ShapeType != RapidEnums::SHAPETYPE::LINE3D && this->ShapeType != RapidEnums::SHAPETYPE::XLINE3D && !this->IsDxfShape())
			this->SetAsCriticalShape(MAINDllOBJECT->CriticalShapeStatus);
		if(LineType == RapidEnums::LINETYPE::FINITELINE || LineType == RapidEnums::LINETYPE::INFINITELINE || LineType == RapidEnums::LINETYPE::RAYLINE || LineType == RapidEnums::LINETYPE::FINITELINE3D || LineType == RapidEnums::LINETYPE::LINEFROMCLOUDPNTS)
			updateNormalLines();
		else if(LineType == RapidEnums::LINETYPE::INTERSECTIONLINE3D)
			UpdateIntersectionLine3D();
		else if(LineType == RapidEnums::LINETYPE::PROJECTIONLINE3D)
			UpdateProjectionLine3D();
		else if (LineType == RapidEnums::LINETYPE::LINEINTWOARC && this->getGroupParent().size() > 0)
		{
			ShapeWithList* Cshape = (ShapeWithList*)(*(this->getGroupParent().begin()));
			((TwoArc *)Cshape)->UpdateForSubshapeChange(this);
		} 
		else if (LineType == RapidEnums::LINETYPE::LINEINTWOLINE && this->getGroupParent().size() > 0)
		{
		   ShapeWithList* Cshape = (ShapeWithList*)(*(this->getGroupParent().begin()));
		   ((TwoLine*)Cshape)->UpdateForSubshapeChange(this);
		}
		else
			updateDerivedLines();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); }
}

 bool Line::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		double LinePt1[4] = {0}, LinePt2[4] = {0}, endp1[4] = {point1.getX(), point1.getY(), point1.getZ(), 1}, endp2[4] = {point2.getX(), point2.getY(), point2.getZ(), 1};		
		RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &endp1[0], &s2[0], &LinePt1[0]);
		RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &endp2[0], &s2[0], &LinePt2[0]);
		RMATH2DOBJECT->RectangleEnclosing_Line(&LinePt1[0], &LinePt2[0], &Lefttop[0], &Rightbottom[0]);	
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); return false; }
}

void Line::updateNormalLines()
{
	try
	{
		if(!this->IsDxfShape() && !PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);
		int totcount = this->PointsListOriginal->Pointscount();
		if(totcount < 2) return;
		switch(this->LineType)
		{
			case RapidEnums::LINETYPE::FINITELINE:
			case RapidEnums::LINETYPE::LINEINTWOARC:
			case RapidEnums::LINETYPE::LINEINTWOLINE:
				if(ShapeAsfasttrace())
					updateFtFgline(totcount);
				else
					updateFiniteLine(totcount);
				break;
			case RapidEnums::LINETYPE::FINITELINE3D:
				update3DLine(totcount);
				break;
			case RapidEnums::LINETYPE::INFINITELINE:
			case RapidEnums::LINETYPE::LINEFROMCLOUDPNTS:
				updateFiniteLine(totcount);
				break;
			case RapidEnums::LINETYPE::RAYLINE:
				updateRayLine(totcount);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); }
}

void Line::updateDerivedLines()
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
			this->IsValid(false);			
		if(LineType == RapidEnums::LINETYPE::LINEPERPENDICULAR2LINEONPLANE)
		{
			updateLinePerpendicular2LineOnPlane();
			return;
		}
		else if(LineType == RapidEnums::LINETYPE::PARALLELLINE3D || LineType == RapidEnums::LINETYPE::PERPENDICULARLINE3D)
		{
			UpdateDerivedLine3D();
			return;
		}
		else if(LineType == RapidEnums::LINETYPE::REFLECTEDLINE)
		{
			UpdateReflectedLine();
			return;
		}
		int totcount = this->PointsListOriginal->Pointscount();
		if(totcount < 1) return;
		if(LineType == RapidEnums::LINETYPE::MULTIPOINTSLINEPARALLEL2LINE)
			updateMultiPointsLineParallel2Line();
		else if(totcount == 1)
			updateDerivedLineAcc_Type();		
		else
			updateDerivedFiniteLine(totcount);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); }
}

void Line::updateDerivedLineAcc_Type()
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
			if(DefinedOffset()) flag = false;
			else flag = true;
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
		double MousePt[2] = {Spt->X, Spt->Y}, EndPt1[2], EndPt2[2];
		if(LineType == RapidEnums::LINETYPE::PARALLEL_LINEL)
			updateParallelLine(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::PERPENDICULAR_LINEL)
			updatePerpendicularLine(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::ANGLE_BISECTORL)
			updateAngleBisector(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::TANGENT_ON_CIRCLEL)
			updateTangentOnCircle(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::PARALLEL_TANGENTL)
			updateParallelTangent(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::PERPENDICULAR_TANGENTL)
			updatePerpendiculartanget(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::TANGENT_THRU_POINT)
			updateTangentthruPoint(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::TANGENT_ON_2_CIRCLE)
			updateTangentOn2Circle(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::ONEPT_ANGLELINE)
			updateOnePtAngleLine(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::ONEPT_OFFSETLINE)
			updateOnePtAngleLine(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		else if(LineType == RapidEnums::LINETYPE::PARALLEL_TANGENT2TWOCIRLE)
			updateParallelTangent2Circle(&MousePt[0], &EndPt1[0], &EndPt2[0], flag);
		if(this->IsDxfShape() && this->PointsListOriginal->Pointscount() >= 2)
		{
			PC_ITER Spt = this->PointsListOriginal->getList().begin();
			point1.set(pts[0], pts[1], Spt->second->Z);
			Spt++;
			point2.set(pts[2], pts[3], Spt->second->Z);
		}
		else if(this->PointsListOriginal->Pointscount() >= 1)
		{
			point1.set(EndPt1[0], EndPt1[1], Spt->Z);
			point2.set(EndPt2[0], EndPt2[1], Spt->Z);
		}
		else
		{
			point1.set(EndPt1[0], EndPt1[1], MAINDllOBJECT->getCurrentDRO().getZ());
			point2.set(EndPt2[0], EndPt2[1], MAINDllOBJECT->getCurrentDRO().getZ());
		}
		calculateAttributes();
	}
	catch(std::exception &ex)
	{ 

		MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, ex.what()); 
	}
}

void Line::updateLinePerpendicular2LineOnPlane()
{
	try
	{
		double actnPnts[3] = {0}, EndPnts[6] = {0};
		int cnt = 0;
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			for each(RAction* Caction in this->PointAtionList)
			{
				if(cnt >= 3) break;
				FramegrabBase *fg = ((AddPointAction*)Caction)->getFramegrab();
				if(fg->CurrentWindowNo == 1)
				{
					if(fg->Pointer_SnapPt != NULL)
					{
						actnPnts[cnt++] = fg->Pointer_SnapPt->getX();
						actnPnts[cnt++] = fg->Pointer_SnapPt->getY();
						actnPnts[cnt++] = fg->Pointer_SnapPt->getZ();
					}
					else
					{
						actnPnts[cnt++] = fg->points[0].getX();
						actnPnts[cnt++] = fg->points[0].getY();
						actnPnts[cnt++] = fg->points[0].getZ();
					}
				}
			}
		}
		else
		{
			for(RC_ITER curraction = PPCALCOBJECT->getPPActionlist().getList().begin(); curraction != PPCALCOBJECT->getPPActionlist().getList().end(); curraction++)
			{
				if(cnt >= 3) break;
				RAction* action = (RAction*)(*curraction).second;
				if(!action->ActionStatus()) continue;
				if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					if(this->getId() != ((AddPointAction*)action)->getShape()->getId())
						continue;		
					FramegrabBase *fg = ((AddPointAction*)action)->getFramegrab();
					if(fg->CurrentWindowNo == 1)
					{
						if(fg->Pointer_SnapPt != NULL)
						{
							actnPnts[cnt++] = fg->Pointer_SnapPt->getX();
							actnPnts[cnt++] = fg->Pointer_SnapPt->getY();
							actnPnts[cnt++] = fg->Pointer_SnapPt->getZ();
						}
						else
						{
							actnPnts[cnt++] = fg->points[0].getX();
							actnPnts[cnt++] = fg->points[0].getY();
							actnPnts[cnt++] = fg->points[0].getZ();
						}
					}
				}
			}
		}
		list<BaseItem*>::iterator Iter = this->getParents().end();
		Plane *parentPlane = (Plane*)(*(--Iter));
		Line *parentLine = (Line*)(*(--Iter));
		double linePnts[6] = {0}, lineParam[6] = {0}, planeParam[4] = {0}, tmpDir[3] = {0}, midPnts[3] = {0}, endPnts[6] = {0}, parentLength = 0;
		parentLine->getParameters(lineParam);
		parentLine->getEndPoints(linePnts);
		parentPlane->getParameters(planeParam);
		parentLength = RMATH3DOBJECT->Distance_Point_Point(linePnts, &linePnts[3]);
		if(parentLength < 1) parentLength = 1;
		if(RMATH3DOBJECT->Checking_Perpendicular_Line_to_Plane(lineParam, planeParam)) return;
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&lineParam[3], planeParam, tmpDir);
		RMATH3DOBJECT->Projection_Point_on_Plane(actnPnts, planeParam, midPnts);
		for(int i = 0; i < 3; i ++)
		{
		   endPnts[i] = midPnts[i] + parentLength * tmpDir[i];
		   endPnts[3 + i] = midPnts[i] - parentLength * tmpDir[i]; 
		}
		point1.set(endPnts[0], endPnts[1], endPnts[2]);
		point2.set(endPnts[3], endPnts[4], endPnts[5]);
		this->calculateAttributes();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE009b", __FILE__, __FUNCSIG__); }
}

void Line::updateMultiPointsLineParallel2Line()
{
	try
	{
		int PointsCnt = this->PointsListOriginal->getList().size();
		if(PointsCnt < 2) return;
		Line *parentLine = NULL;
		parentLine = (Line*)(*this->getParents().begin());
		if(parentLine == NULL)
		{
			if(MAINDllOBJECT->Vblock_CylinderAxisLine != NULL)
				parentLine = (Line*)(MAINDllOBJECT->Vblock_CylinderAxisLine);
			else return;
		}
		double ans[2] = {0}, p1[2] = {0}, p2[2] = {0}, p3[4] = {0}, ang = parentLine->Angle();
		BESTFITOBJECT->ParallelLine_BestFit_2D(&ang, pts, PointsCnt, &ans[0]);
		RMATH2DOBJECT->LineEndPoints(pts, PointsCnt, &p1[0], &p2[0], ans[0]);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &p3[0]);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &p3[2]);
		if (!EndPointsFixed())
		{
			PC_ITER Spt = this->PointsListOriginal->getList().begin();
			point1.set(p3[0], p3[1], Spt->second->Z);
			point2.set(p3[2], p3[3], Spt->second->Z);
		}
		this->Angle(ans[0]);
		this->Intercept(ans[1]);
		this->calculateAttributes();
		this->IsValid(true);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009b", __FILE__, __FUNCSIG__);
	}
}

void Line::UpdateDerivedLine3D()
{
	try
	{
		this->IsValid(false);
		double actnPnts[6] = {0},  endPnts[6] = {0};
		int cnt = 0;
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			for each(RAction* Caction in this->PointAtionList)
			{
				if(cnt >= 6) break;
				FramegrabBase *fg = ((AddPointAction*)Caction)->getFramegrab();
				if(fg->CurrentWindowNo == 1)
				{
					if(fg->Pointer_SnapPt != NULL)
					{
						actnPnts[cnt++] = fg->Pointer_SnapPt->getX();
						actnPnts[cnt++] = fg->Pointer_SnapPt->getY();
						actnPnts[cnt++] = fg->Pointer_SnapPt->getZ();
					}
					else
					{
						actnPnts[cnt++] = fg->points[0].getX();
						actnPnts[cnt++] = fg->points[0].getY();
						actnPnts[cnt++] = fg->points[0].getZ();
					}
				}
			}
		}
		else
		{
			for(RC_ITER curraction = PPCALCOBJECT->getPPActionlist().getList().begin(); curraction != PPCALCOBJECT->getPPActionlist().getList().end(); curraction++)
			{
				if(cnt >= 6) break;
				RAction* action = (RAction*)(*curraction).second;
				if(!action->ActionStatus()) continue;
				if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					if(this->getId() != ((AddPointAction*)action)->getShape()->getId())
						continue;		
					FramegrabBase *fg = ((AddPointAction*)action)->getFramegrab();
					if(fg->CurrentWindowNo == 1)
					{
						if(fg->Pointer_SnapPt != NULL)
						{
							actnPnts[cnt++] = fg->Pointer_SnapPt->getX();
							actnPnts[cnt++] = fg->Pointer_SnapPt->getY();
							actnPnts[cnt++] = fg->Pointer_SnapPt->getZ();
						}
						else
						{
							actnPnts[cnt++] = fg->points[0].getX();
							actnPnts[cnt++] = fg->points[0].getY();
							actnPnts[cnt++] = fg->points[0].getZ();
						}
					}
				}
			}
		}
		if(LineType == RapidEnums::LINETYPE::PARALLELLINE3D)
		{
			//list<BaseItem*>::iterator Iter = this->getParents(); // .end();
			Shape* sp;
			for each (Shape* iter in this->getParents())
			{
				if (iter == NULL) return;
				sp = (Shape*)iter;
			}
			if (sp == NULL) return;
			double parentParam[6] = {0};
			if (sp->ShapeType == RapidEnums::SHAPETYPE::CYLINDER || sp->ShapeType == RapidEnums::SHAPETYPE::CONE)
			{
				Cylinder* Cyl = (Cylinder*)sp;
				Cyl->getAxisLine(parentParam);
			}
			else
			{
				Line *parentPLine = (Line*)sp; //(Line*)(*(--Iter));
				parentPLine->getParameters(parentParam);
			}
			if (DefinedOffset())
			{
				double perpendLine[6] = { 0 };
				RMATH3DOBJECT->Projection_Point_on_Line(actnPnts, parentParam, &actnPnts[3]);
				RMATH3DOBJECT->Create_Perpendicular_Line_2_Line(actnPnts, parentParam, perpendLine);
				double PtsonLine[6] = { 0 };
				RMATH3DOBJECT->Create_Points_On_Line(perpendLine, &actnPnts[3], this->Offset(), PtsonLine);
				double ParallelLine[6] = { 0 };
				if (RMATH3DOBJECT->Distance_Point_Point(actnPnts, PtsonLine) < RMATH3DOBJECT->Distance_Point_Point(actnPnts, &PtsonLine[3]))
					memcpy(ParallelLine, &PtsonLine[0], 3 * sizeof(double));
				else
					memcpy(ParallelLine, &PtsonLine[3], 3 * sizeof(double));
				memcpy(&ParallelLine[3], &parentParam[3], 3 * sizeof(double));
				RMATH3DOBJECT->Create_Points_On_Line(ParallelLine, ParallelLine, this->Length() / 2, endPnts);
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					endPnts[i] = actnPnts[i] + 3 * parentParam[3 + i];
					endPnts[3 + i] = actnPnts[i] - 3 * parentParam[3 + i];
				}
			}
		}
		else if(LineType == RapidEnums::LINETYPE::PERPENDICULARLINE3D)
		{
			list<BaseItem*>::iterator Iter = this->getParents().end();
			Line *parentPLine = (Line*)(*(--Iter));
			if(parentPLine == NULL) return;
			double parentParam[6] = {0}, tmpPlane[4] = {0};
			parentPLine->getParameters(parentParam);
			for(int i = 0; i < 3; i++)
			{
				tmpPlane[i] = parentParam[3 + i];
				tmpPlane[3] += tmpPlane[i] * actnPnts[i];
			}
			RMATH3DOBJECT->Projection_Point_on_Plane(actnPnts, tmpPlane, endPnts);
			RMATH3DOBJECT->Projection_Point_on_Plane(&actnPnts[3], tmpPlane, &endPnts[3]);
		}
		point1.set(endPnts[0], endPnts[1], endPnts[2]);
		point2.set(endPnts[3], endPnts[4], endPnts[5]);
		this->IsValid(true);
		calculateAttributes();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); }
}

void Line::UpdateReflectedLine()
{
	try
	{
		if(this->getParents().size() < 2) return;
		this->IsValid(true);
		list<BaseItem*>::iterator ptr = this->getParents().begin();
		Line *parentLine1 = (Line*)(*ptr);
		ptr++;
		Line *parentLine2 = (Line*)(*ptr);
		double OriginalPoint[6] = {0}, ReflectedPts[6] = {0};
		parentLine2->getEndPoints(OriginalPoint);
		RMATH2DOBJECT->ReflectPointsAlongLine(OriginalPoint, 2, parentLine1->Angle(), parentLine1->Intercept(), ReflectedPts); 
		this->point1.set(ReflectedPts[0], ReflectedPts[1], ReflectedPts[2]);
		this->point2.set(ReflectedPts[3], ReflectedPts[4], ReflectedPts[5]);
		this->calculateAttributes();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0012", __FILE__, __FUNCSIG__); }
}

void Line::updateFiniteLine(int PointsCnt)
{
	try
	{
		if(PointsCnt == 2)
		{
			if(this->IsDxfShape() && this->PointsListOriginal->Pointscount() >= 2)
			{
				PC_ITER Spt = this->PointsListOriginal->getList().begin();
				this->point1.set(pts[0], pts[1], Spt->second->Z);
				Spt++;
				this->point2.set(pts[2], pts[3], Spt->second->Z);
			}
			else if(this->PointsListOriginal->Pointscount() >= 0)
			{
				PC_ITER Spt = this->PointsListOriginal->getList().begin();
				this->point1.set(pts[0], pts[1], Spt->second->Z);
				this->point2.set(pts[2], pts[3], Spt->second->Z);
			}
			else
			{
				this->point1.set(pts[0], pts[1], MAINDllOBJECT->getCurrentDRO().getZ());
				this->point2.set(pts[2], pts[3], MAINDllOBJECT->getCurrentDRO().getZ());
			}
		}
		else
			CalculateLineBestFit(PointsCnt, false);
		//Calculate the line parameters..//
		this->calculateAttributes();
		ProbePointCalculations(PointsCnt);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0010", __FILE__, __FUNCSIG__); }
}

void Line::updateDerivedFiniteLine(int PointsCnt)
{
	try
	{
		if(this->ShapeAs3DShape())
		{
			if(PointsCnt == 2)
			{
				point1.set(pts[0], pts[1], pts[2]);
				point2.set(pts[3], pts[4], pts[5]);
			}
			else
				CalculateLineBestFit3D(PointsCnt);
		}
		else
		{
			if(PointsCnt == 2)
			{
				if(this->IsDxfShape() && this->PointsListOriginal->Pointscount() >= 2)
				{
					PC_ITER Spt = this->PointsListOriginal->getList().begin();
					point1.set(pts[0], pts[1], Spt->second->Z);
					Spt++;
					point2.set(pts[2], pts[3], Spt->second->Z);
				}
				else
				{
					point1.set(pts[0], pts[1], MAINDllOBJECT->getCurrentDRO().getZ());
					point2.set(pts[2], pts[3], MAINDllOBJECT->getCurrentDRO().getZ());
				}
			}
			else
				CalculateLineBestFit(PointsCnt, true);
		}
		this->calculateAttributes();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0010", __FILE__, __FUNCSIG__); }
}

void Line::updateRayLine(int PointsCnt)
{
	try
	{
		point1.set(pts[0], pts[1], MAINDllOBJECT->getCurrentDRO().getZ());
		point2.set(pts[2], pts[3], MAINDllOBJECT->getCurrentDRO().getZ());
		double angle = RMATH2DOBJECT->ray_angle(pts, pts + 2);
		this->Angle(angle);
		this->Intercept(pts[1] - tan(Angle()) * pts[0]);
		if(this->Angle() == M_PI_2 || this->Angle() == 3 * M_PI_2)
			this->Intercept(pts[0]);
		this->dir_l(cos(Angle()));
		this->dir_m(sin(Angle()));
		this->dir_n(0);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0012", __FILE__, __FUNCSIG__); }
}

void Line::update3DLine(int PointsCnt)
{
	try
	{
		if(PointsCnt == 2)
		{
			point1.set(pts[0], pts[1], pts[2]);
			point2.set(pts[3], pts[4], pts[5]);
		}
		else
		{
			//Calculate the best fit here...!
			CalculateLineBestFit3D(PointsCnt);
		}
		this->calculateAttributes();
		ProbePointCalculations_3DLine(PointsCnt);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0013", __FILE__, __FUNCSIG__); }
}

void Line::updateFtFgline(int PointsCnt)
{
	try
	{
		int totcount = this->PointsListOriginal->Pointscount();
		if (totcount < 2) return;
		list<BaseItem*>::iterator psh;
		Shape *CShape1 = NULL, *CShape2 = NULL;
		bool start_pt_constraint = false, end_pt_constraint = false;
		Vector Sh1Endp1, Sh1Endp2, Sh2Endp1, Sh2Endp2, StartPoint, Endpoint;
		if(PointsCnt == 2)
		{
			if(this->IsDxfShape() && this->PointsListOriginal->Pointscount() >= 2)
			{
				PC_ITER Spt = this->PointsListOriginal->getList().begin();
				point1.set(pts[0], pts[1], Spt->second->Z);
				Spt++;
				point2.set(pts[2], pts[3], Spt->second->Z);
			}
			else
			{
				point1.set(pts[0], pts[1], MAINDllOBJECT->getCurrentDRO().getZ());
				point2.set(pts[2], pts[3], MAINDllOBJECT->getCurrentDRO().getZ());
			}
			this->calculateAttributes();
			this->IsValid(true);
			return;
		}
		else
		{
			if(this->getFgNearShapes().size() == 0)
			{
				updateFiniteLine(PointsCnt);
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
				if((Sh1Endp1.operator==(point1)) || (Sh1Endp1.operator==(point2)))
					StartPoint.set(Sh1Endp1);
				else if((Sh1Endp2.operator==(point1)) || (Sh1Endp2.operator==(point2)))
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
				if((Sh1Endp1.operator==(point1)) || (Sh1Endp1.operator==(point2)))
					StartPoint.set(Sh1Endp1);
				else if((Sh1Endp2.operator==(point1)) || (Sh1Endp2.operator==(point2)))
					StartPoint.set(Sh1Endp2);
				if((Sh2Endp1.operator==(point1)) || (Sh2Endp1.operator==(point2)))
					Endpoint.set(Sh2Endp1);
				else if((Sh2Endp2.operator==(point1)) || (Sh2Endp2.operator==(point2)))
					Endpoint.set(Sh2Endp2);
			}
		}
		if(!start_pt_constraint && !end_pt_constraint)
		{
			PointCollection PtColl1, PtColl2;
			Vector Shape2NearPt, Shape1NearPt;
			if(this->getFgNearShapes().size() == 1)
			{
				if(StartPoint.operator==(point1))
				{
					if(point1.operator==(Sh1Endp1))
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
					if(point2.operator==(Sh1Endp1))
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
				if(StartPoint.operator==(point1))
				{
					if(point2.operator==(Sh2Endp1))
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
					if(point1.operator==(Sh2Endp1))
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
				if(Endpoint.operator==(point1))
				{
					if(point2.operator==(Sh1Endp1))
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
					if(point1.operator==(Sh1Endp1))
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

			updateFiniteLine(PointsCnt);

			
			if(this->getFgNearShapes().size() == 1)
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), point1.getX(), point1.getY()) < RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), point2.getX(), point2.getY()))
					PtColl2.Addpoint(new SinglePoint(point1.getX(), point1.getY(), point1.getZ()));
				else
					PtColl2.Addpoint(new SinglePoint(point2.getX(), point2.getY(), point2.getZ()));
				((ShapeWithList*)CShape1)->ResetCurrentPointsList();
				((ShapeWithList*)CShape1)->AddPoints(&PtColl2);
			}
			else
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), point1.getX(), point1.getY()) < RMATH2DOBJECT->Pt2Pt_distance(Shape2NearPt.getX(), Shape2NearPt.getY(), point2.getX(), point2.getY()))
				{
					PtColl2.Addpoint(new SinglePoint(point1.getX(), point1.getY(), point1.getZ()));
					PtColl1.Addpoint(new SinglePoint(point2.getX(), point2.getY(), point2.getZ()));
				}
				else
				{
					PtColl2.Addpoint(new SinglePoint(point2.getX(), point2.getY(), point2.getZ()));
					PtColl1.Addpoint(new SinglePoint(point1.getX(), point1.getY(), point1.getZ()));
				}
				((ShapeWithList*)CShape1)->ResetCurrentPointsList();
				((ShapeWithList*)CShape1)->AddPoints(&PtColl1);
				((ShapeWithList*)CShape2)->ResetCurrentPointsList();
				((ShapeWithList*)CShape2)->AddPoints(&PtColl2);
			}

		}
		else if(start_pt_constraint && end_pt_constraint)
		{
			return;
		}
		else
		{
			double ans[2] = {0}, p1[2], p2[2], p3[4];
			double stpt[2] = {StartPoint.getX(), StartPoint.getY()}, endpt[2] = {Endpoint.getX(), Endpoint.getY()};
			if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
				BESTFITOBJECT->FG_Line_BestFit_2D(pts, PointsCnt, &ans[0], true, start_pt_constraint, end_pt_constraint, &stpt[0], &endpt[0]);
			else
				BESTFITOBJECT->FG_Line_BestFit_2D(pts, PointsCnt, &ans[0], SigmaMode(), start_pt_constraint, end_pt_constraint, &stpt[0], &endpt[0]);
			//calculate the minimum and maximum point in the list..
			RMATH2DOBJECT->LineEndPoints(pts, PointsCnt, &p1[0], &p2[0], ans[0]);
		
			//Calculate the end points..//
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &p3[0]);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &p3[2]);
			if(start_pt_constraint)
			{
				if(this->getFgNearShapes().size() == 1)
				{
					point1.set(StartPoint.getX(), StartPoint.getY(), StartPoint.getZ());
					if(RMATH2DOBJECT->Pt2Pt_distance(p3[0], p3[1], StartPoint.getX(), StartPoint.getY()) < RMATH2DOBJECT->Pt2Pt_distance(p3[2], p3[3], StartPoint.getX(), StartPoint.getY()))				
						point2.set(p3[2], p3[3], MAINDllOBJECT->getCurrentDRO().getZ());
					else
						point2.set(p3[0], p3[1], MAINDllOBJECT->getCurrentDRO().getZ());
				}
				else
				{
					PointCollection PtColl;
					if(CShape2 != NULL)
					{
						if(StartPoint.operator==(point1))
						{
							if(point2.operator==(Sh2Endp1))
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
							if(point1.operator==(Sh2Endp1))
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
					point1.set(StartPoint.getX(), StartPoint.getY(), StartPoint.getZ());
					if(RMATH2DOBJECT->Pt2Pt_distance(p3[0], p3[1], StartPoint.getX(), StartPoint.getY()) < RMATH2DOBJECT->Pt2Pt_distance(p3[2], p3[3], StartPoint.getX(), StartPoint.getY()))				
						point2.set(p3[2], p3[3], MAINDllOBJECT->getCurrentDRO().getZ());
					else
						point2.set(p3[0], p3[1], MAINDllOBJECT->getCurrentDRO().getZ());
					if(CShape2 != NULL)
					{
						PtColl.Addpoint(new SinglePoint(point2.getX(), point2.getY(), point2.getZ()));
						((ShapeWithList*)CShape2)->ResetCurrentPointsList();
						((ShapeWithList*)CShape2)->AddPoints(&PtColl);
					}
				}
			}
			else
			{
				if(this->getFgNearShapes().size() == 1)
				{
					point1.set(Endpoint.getX(), Endpoint.getY(), MAINDllOBJECT->getCurrentDRO().getZ());
					if(RMATH2DOBJECT->Pt2Pt_distance(p3[0], p3[1], Endpoint.getX(), Endpoint.getY()) < RMATH2DOBJECT->Pt2Pt_distance(p3[2], p3[3], Endpoint.getX(), Endpoint.getY()))				
						point2.set(p3[2], p3[3], MAINDllOBJECT->getCurrentDRO().getZ());
					else
						point2.set(p3[0], p3[1], MAINDllOBJECT->getCurrentDRO().getZ());
				}
				else
				{
					PointCollection PtColl;
					if(CShape1 != NULL)
					{
						if(Endpoint.operator==(point1))
						{
							if(point2.operator==(Sh1Endp1))
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
							if(point1.operator==(Sh1Endp1))
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
					point1.set(Endpoint.getX(), Endpoint.getY(), MAINDllOBJECT->getCurrentDRO().getZ());
					if(RMATH2DOBJECT->Pt2Pt_distance(p3[0], p3[1], Endpoint.getX(), Endpoint.getY()) < RMATH2DOBJECT->Pt2Pt_distance(p3[2], p3[3], Endpoint.getX(), Endpoint.getY()))				
						point2.set(p3[2], p3[3], MAINDllOBJECT->getCurrentDRO().getZ());
					else
						point2.set(p3[0], p3[1], MAINDllOBJECT->getCurrentDRO().getZ());
					if(CShape1 != NULL)
					{
						PtColl.Addpoint(new SinglePoint(point2.getX(), point2.getY(), point2.getZ(), MAINDllOBJECT->getCurrentDRO().getZ()));
						((ShapeWithList*)CShape1)->ResetCurrentPointsList();
						((ShapeWithList*)CShape1)->AddPoints(&PtColl);
					}
				}
			}
			calculateAttributes();
			this->IsValid(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); }
}

void Line::CalculateLineBestFit(int PointsCnt, bool IsderivedLine)
{
	try
	{
		
		double zLevel =  MAINDllOBJECT->getCurrentDRO().getZ(), ans[2] = {0}, p1[2] = {0}, p2[2] = {0}, p3[4] = {0}, angle = Angle();
		if(!this->PointsList->getList().empty())
			zLevel = this->PointsList->getList().begin()->second->Z;
		//Calculate the line bestfit..//
		if(IsderivedLine)
		{
			BESTFITOBJECT->ParallelLine_BestFit_2D(&angle, pts, PointsCnt, &ans[0]);
		}
		else
		{
			if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
				BESTFITOBJECT->Line_BestFit_2D(pts, PointsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
			else
				BESTFITOBJECT->Line_BestFit_2D(pts, PointsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
			RMATH2DOBJECT->LineEndPoints(pts, PointsCnt, &p1[0], &p2[0], ans[0]);
		}
		if(ApplyFilterCorrection() && PointsCnt > 2)
		{
			int cnt = PointsCnt;
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
			{
				double lineparam[6] = {p1[0], p1[1], 0, cos(ans[0]), sin(ans[0]), 0};
				cnt = HELPEROBJECT->FilteredLine(this->PointsList, lineparam, FilterCorrectionFactor(), i);
				if(cnt < 3) break;
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
					BESTFITOBJECT->Line_BestFit_2D(filterpts, filterptsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				else
					BESTFITOBJECT->Line_BestFit_2D(filterpts, filterptsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				RMATH2DOBJECT->LineEndPoints(filterpts, filterptsCnt, &p1[0], &p2[0], ans[0]);
			}
		}
		if(this->ApplyMMCCorrection() && ScanDirDetected() && PointsCnt > 2)
		{
			int cnt = PointsCnt;
			for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
			{
				double lineparam[6] = {p1[0], p1[1], 0, cos(ans[0]), sin(ans[0]), 0};
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
				cnt = HELPEROBJECT->OutermostInnerMostLine(this->PointsList, lineparam, useValidPointOnly, ScanDir);
				if(cnt < 3) break;   
				int n = 0;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
				filterptsCnt = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					if(((*Spt).second)->InValid) continue;
					SinglePoint* Pt = (*Spt).second;
					filterpts[n++] = Pt->X;
					filterpts[n++] = Pt->Y;
					filterptsCnt++;
				}
				if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					BESTFITOBJECT->Line_BestFit_2D(filterpts, filterptsCnt, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				else
					BESTFITOBJECT->Line_BestFit_2D(filterpts, filterptsCnt, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				RMATH2DOBJECT->LineEndPoints(filterpts, filterptsCnt, &p1[0], &p2[0], ans[0]);
			}
		}
		//Calculate the end points..//
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &p3[0]);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &p3[2]);
	
		//Set the endpoint s according to the projection type..//
		if (!EndPointsFixed())
		{
			if(this->IsDxfShape() && this->PointsListOriginal->Pointscount() >= 2)
			{
				PC_ITER Spt = this->PointsListOriginal->getList().begin();
				point1.set(p3[0], p3[1], Spt->second->Z);
				Spt++;
				point2.set(p3[2], p3[3], Spt->second->Z);
			}
			else
			{
				point1.set(p3[0], p3[1], zLevel);
				point2.set(p3[2], p3[3], zLevel);
			}
		}
		this->Angle(ans[0]);
		this->Intercept(ans[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::CalculateLineBestFit3D(int PointsCnt)
{
	try
	{
		double ans[6] = {0}, endp1[3] = {0}, endp2[3] = {0}, minr = 0, maxr = 0;
		//Calculate the line bestfit..//
		bool GotBestFit = BESTFITOBJECT->Line_BestFit_3D(pts, PointsCnt, &ans[0]);
		HELPEROBJECT->CalculateEndPoints_3DLineBestfit(PointsCnt, pts, &ans[0], &endp1[0], &endp2[0], &minr, &maxr);
		if(ApplyFilterCorrection() && PointsCnt > 2)
		{
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
			{
				double lineparam[6] = {endp1[0], endp1[1], endp1[2], 0, 0, 0};
				double len = sqrt(pow(endp1[0] - endp2[0], 2) + pow(endp1[1] - endp2[1], 2) + pow(endp1[2] - endp2[2], 2));	
				lineparam[3] = (endp1[0] - endp2[0]) / len;
				lineparam[4] = (endp1[1] - endp2[1]) / len;
				lineparam[5] = (endp1[2] - endp2[2]) / len;
				int cnt = HELPEROBJECT->FilteredLine3D(this->PointsList, lineparam, FilterCorrectionFactor(), i);
				if(cnt < 3) break;				
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				filterptsCnt = 0;
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
				BESTFITOBJECT->Line_BestFit_3D(filterpts, filterptsCnt, &ans[0]);
				HELPEROBJECT->CalculateEndPoints_3DLineBestfit(filterptsCnt, filterpts, &ans[0], &endp1[0], &endp2[0], &minr, &maxr);	
			}
		}
		if(ApplyMMCCorrection() && ScanDirDetected() && PointsCnt > 2)
		{
			int cnt = PointsCnt;
			for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
			{
				double lineparam[6] = {endp1[0], endp1[1], endp1[2], 0, 0, 0};
				double len = sqrt(pow(endp1[0] - endp2[0], 2) + pow(endp1[1] - endp2[1], 2) + pow(endp1[2] - endp2[2], 2));	
				lineparam[3] = (endp1[0] - endp2[0]) / len;
				lineparam[4] = (endp1[1] - endp2[1]) / len;
				lineparam[5] = (endp1[2] - endp2[2]) / len;
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
				cnt = HELPEROBJECT->OutermostInnerMostLine3D(this->PointsList, lineparam, useValidPointOnly, ScanDir);
				if(cnt < 3) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				filterptsCnt = 0;
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
				BESTFITOBJECT->Line_BestFit_3D(filterpts, filterptsCnt, &ans[0]);
				HELPEROBJECT->CalculateEndPoints_3DLineBestfit(filterptsCnt, filterpts, &ans[0], &endp1[0], &endp2[0], &minr, &maxr);
			}
		}
		//Set the endpoint s according to the projection type..//
		point1.set(endp1[0], endp1[1], endp1[2]);
		point2.set(endp2[0], endp2[1], endp2[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updateParallelLine(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); ptr--;
		Line* ParentSh = (Line*)(*ptr);
		double ParAngle = ParentSh->Angle(), ParIntercept = ParentSh->Intercept();
		double pt1[2], pt2[2], Offst = Offset();
		RMATH2DOBJECT->Parallelline(ParAngle, ParIntercept, Mousept, Length(), EndPt1, EndPt2, &Offst, !flag, LinePosition());
		Offset(Offst);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updatePerpendicularLine(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); ptr--;
		Line* ParentSh = (Line*)(*ptr);
		double ParAngle = ParentSh->Angle(), ParIntercept = ParentSh->Intercept();
		double pt1[2], pt2[2];
		RMATH2DOBJECT->Perpedicularlline(ParAngle, ParIntercept, Mousept, Length(), EndPt1, EndPt2, LinePosition());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updateAngleBisector(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Line* ParentSh1 = (Line*)(*ptr--);
		Line* ParentSh2 = (Line*)(*ptr);
		double ParAngle1 = ParentSh1->Angle(), ParIntercept1 = ParentSh1->Intercept();
		double ParAngle2 = ParentSh2->Angle(), ParIntercept2 = ParentSh2->Intercept();
		SinglePoint* Spt = (SinglePoint*)(*PointsListOriginal->getList().begin()).second;
		double mp[2] = {Spt->X, Spt->Y}, pt1[2], pt2[2];
		RMATH2DOBJECT->Angle_bisector(ParAngle1, ParIntercept1, ParAngle2, ParIntercept2, &mp[0], Length(), EndPt1, EndPt2, LinePosition());
		RAxisVal = (((ShapeWithList*)ParentSh1)->RAxisVal + ((ShapeWithList*)ParentSh2)->RAxisVal) / 2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::UpdateIntersectionLine3D()
{
	try
	{
		IsValid(false);
		double Plane1Param[4], Plane2Param[4], Plane1EndPoints[12], Plane2EndPoints[12], IntersectionLine[6], projectionPnts[24], EndPt[6];
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Plane* ParentSh1 = (Plane*)(*ptr--);
		Plane* ParentSh2 = (Plane*)(*ptr);
		((Plane*)ParentSh1)->getParameters(&Plane1Param[0]);
		((Plane*)ParentSh2)->getParameters(&Plane2Param[0]);
		((Plane*)ParentSh1)->getEndPoints(&Plane1EndPoints[0]);
		((Plane*)ParentSh2)->getEndPoints(&Plane2EndPoints[0]);
		 RMATH3DOBJECT->Intersection_Plane_Plane(&Plane1Param[0], &Plane2Param[0], IntersectionLine);
		 for(int i = 0; i < 4; i++)
		 {
			   RMATH3DOBJECT->Projection_Point_on_Line(&Plane1EndPoints[3 * i], IntersectionLine, &projectionPnts[3 * i]);
			   RMATH3DOBJECT->Projection_Point_on_Line(&Plane2EndPoints[3 * i], IntersectionLine, &projectionPnts[12 + 3 * i]);
		 }
		 double Minr, Maxr;
		 HELPEROBJECT->CalculateEndPoints_3DLineBestfit(8, projectionPnts, IntersectionLine, EndPt, &EndPt[3], &Minr, &Maxr);
		 point1.set(EndPt[0], EndPt[1], EndPt[2]);
		 point2.set(EndPt[3], EndPt[4], EndPt[5]);
		 dir_l(IntersectionLine[3]);
		 dir_m(IntersectionLine[4]);
		 dir_n(IntersectionLine[5]);
		 Midpoint.set((point1.getX() + point2.getX())/2, (point1.getY() + point2.getY())/2, (point1.getZ() + point2.getZ())/2);
		 IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::UpdateProjectionLine3D()
{
	try
	{
		
		if(this->getParents().size() < 2) return;
		list<BaseItem*>::iterator ptr = this->getParents().begin();
		Shape *parentshp1 = NULL, *parentshp2 = NULL;
		parentshp1 = (Shape*)(*ptr++);		
		parentshp2 = (Shape*)(*ptr);
		double projPnts[6] = {0}, planeParam[4] = {0}, cpoints[6] = {0};
		((Line*)parentshp1)->getEndPoints(cpoints);
		((Plane*)parentshp2)->getParameters(planeParam);
		RMATH3DOBJECT->Projection_Point_on_Plane(&cpoints[0], planeParam, &projPnts[0]);
		RMATH3DOBJECT->Projection_Point_on_Plane(&cpoints[3], planeParam, &projPnts[3]);
		this->setPoint1(Vector(projPnts[0], projPnts[1], projPnts[2]));
		this->setPoint2(Vector(projPnts[3], projPnts[4], projPnts[5]));
		this->calculateAttributes();
		IsValid(true);
	}
	catch(...)
	{
		 MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014a", __FILE__, __FUNCSIG__); 
	}
}

void Line::updateTangentOnCircle(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentSh = (Circle*)(*ptr);
		double cen[2] = {ParentSh->getCenter()->getX(), ParentSh->getCenter()->getY()};
		double radius = ParentSh->Radius(), pt1[2], pt2[2];
		RMATH2DOBJECT->Tangent2aCircle(&cen[0], radius, Mousept, Length(), EndPt1, EndPt2, LinePosition());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updateParallelTangent(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentCircle = (Circle*)(*ptr--);
		Line* ParentLine = (Line*)(*ptr);
		double cen[2] = {ParentCircle->getCenter()->getX(), ParentCircle->getCenter()->getY()};
		double radius = ParentCircle->Radius(), pt1[2], pt2[2];
		double ParAngle = ParentLine->Angle(), ParIntercept = ParentLine->Intercept();
		RMATH2DOBJECT->Tangent_Parallel(&cen[0], radius, ParAngle, ParIntercept, Length(), Mousept, EndPt1, EndPt2, LinePosition());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updatePerpendiculartanget(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentCircle = (Circle*)(*ptr--);
		Line* ParentLine = (Line*)(*ptr);
		double cen[2] = {ParentCircle->getCenter()->getX(), ParentCircle->getCenter()->getY()};
		double radius = ParentCircle->Radius(), pt1[2], pt2[2];
		double ParAngle = ParentLine->Angle(), ParIntercept = ParentLine->Intercept();
		RMATH2DOBJECT->Tangent_Perpendicular(&cen[0], radius, ParAngle, ParIntercept, Length(), Mousept, EndPt1, EndPt2, LinePosition());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updateTangentOn2Circle(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentCircle1 = (Circle*)(*ptr--);
		Circle* ParentCircle2 = (Circle*)(*ptr);
		double cen1[3] = {ParentCircle1->getCenter()->getX(), ParentCircle1->getCenter()->getY(), 0};
		double cen2[3] = {ParentCircle2->getCenter()->getX(), ParentCircle2->getCenter()->getY(), 0};
		double radius1 = ParentCircle1->Radius(), radius2 = ParentCircle2->Radius(), pt1[2], pt2[2];
		RMATH2DOBJECT->Tangent_TwoCircles(&cen1[0], radius1, &cen2[0], radius2, Length(), Mousept, EndPt1, EndPt2);
		if(!DefinedLength())
		{
			double lineparam[6] = {EndPt1[0], EndPt1[1], 0, 0, 0, 0}, dir[3] = {EndPt2[0] - EndPt1[0], EndPt2[1] - EndPt1[1], 0};
			RMATH3DOBJECT->DirectionCosines(dir, &lineparam[3]);
			double endpnts[6] = {0};
			RMATH3DOBJECT->Projection_Point_on_Line(cen1, lineparam, endpnts);
			RMATH3DOBJECT->Projection_Point_on_Line(cen2, lineparam, &endpnts[3]);
			EndPt1[0] = endpnts[0];
			EndPt1[1] = endpnts[1];
			EndPt2[0] = endpnts[3];
			EndPt2[1] = endpnts[4];
			Length(RMATH2DOBJECT->Pt2Pt_distance(EndPt1[0], EndPt1[1], EndPt2[0], EndPt2[1]));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updateTangentthruPoint(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Circle* ParentCircle = (Circle*)(*ptr);
		double cen[2] = {ParentCircle->getCenter()->getX(), ParentCircle->getCenter()->getY()};
		double radius = ParentCircle->Radius(), pt1[2], pt2[2];
		double VPt[2] = {VPointer->getX(), VPointer->getY()};
		RMATH2DOBJECT->Tangent_ThruPoint(&cen[0], radius, &VPt[0], Mousept, Length(), EndPt1, EndPt2, LinePosition());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updateOnePtAngleLine(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
		list<BaseItem*>::iterator ptr = this->getParents().end(); 
		ptr--;
		Line* ParentSh = (Line*)(*ptr);
		double ParAngle = ParentSh->Angle(), pt1[2], pt2[2];
		RMATH2DOBJECT->EquiDistPointsOnLine(ParAngle + Offset(), Length(), Mousept, EndPt1, EndPt2, LinePosition());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::updateParallelTangent2Circle(double* Mousept, double* EndPt1, double* EndPt2, bool flag)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0014", __FILE__, __FUNCSIG__); }
}

void Line::ProbePointCalculations_3DLine(int PointsCnt)
{
	try
	{
		if(this->ShapeForThreadMeasurement()) return;
		if(this->ShapeForPerimeter()) return;
		if(PointsCnt >= 2) // && PointsCnt < 10)
		{
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();	
		}
		if(ShapeDoneUsingProbe())
		{
			update3DLine_UsingPorbe();
			CalculateLineBestFit3D(PointsCnt);
			this->calculateAttributes();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0015a", __FILE__, __FUNCSIG__); }
}

void Line::update3DLine_UsingPorbe()
{
	try
	{
		double PtX, PtY, PtZ, Pradius = 0;
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			Pradius = getProbeRadius(Spt->PLR);
			if(Spt->Pdir == 0)
			{
				PtX = Spt->X - Pradius; PtY = Spt->Y; PtZ = Spt->Z;
			}
			else if(Spt->Pdir == 1)
			{
				PtX = Spt->X + Pradius; PtY = Spt->Y; PtZ = Spt->Z;
			}
			else if(Spt->Pdir == 2)
			{
				PtX = Spt->X; PtY = Spt->Y - Pradius; PtZ = Spt->Z;
			}
			else if(Spt->Pdir == 3)
			{
				PtX = Spt->X; PtY = Spt->Y + Pradius; PtZ = Spt->Z;
			}
			if(Spt->Pdir == 4)
			{
				PtX = Spt->X; PtY = Spt->Y; PtZ = Spt->Z - Pradius;
			}
			else if(Spt->Pdir == 5)
			{
				PtX = Spt->X; PtY = Spt->Y; PtZ = Spt->Z + Pradius;
			}
			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(PtX, PtY, PtZ, Spt->R, Spt->Pdir);
		}
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0018", __FILE__, __FUNCSIG__); }
}

void Line::ProbePointCalculations(int PointsCnt)
{
	try
	{
		//if(PointAtionList.size() < 1) return;
		if(this->ShapeForThreadMeasurement()) return;
		if(this->ShapeForPerimeter()) return;
		if(PointsCnt >= 2) // && PointsCnt < 10)
		{
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();	
			if(ShapeDoneUsingProbe())
				CheckAddOrSubtractIntercept();
		}
		if(ShapeDoneUsingProbe())
		{
			updateLine_UsingPorbe();
			CalculateLineBestFit(PointsCnt, false);
			this->calculateAttributes();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0015", __FILE__, __FUNCSIG__); }
}

void Line::CheckPointIsForPorbe()
{
	try
	{
		if(PointAtionList.size() < 1) return;
		AddPointAction* Caction = (AddPointAction*)(*PointAtionList.begin());
		if(Caction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
		{
			if(this->ProbeCorrection())
			{
				this->ShapeDoneUsingProbe(true);
			}
			else
				this->ShapeDoneUsingProbe(false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0016", __FILE__, __FUNCSIG__); }
}

void Line::CheckAddOrSubtractIntercept()
{
	try
	{
		int IDCount = 0;
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(this->Angle() <= M_PI_2)
			{
				if(Spt->Pdir == 0 || Spt->Pdir == 3) //FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP , FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT //
					IDCount++;
				else if(Spt->Pdir == 1 || Spt->Pdir == 2)
					IDCount--;
			}
			else
			{
				if(Spt->Pdir == 1 || Spt->Pdir == 3) //FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP , FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT //
					IDCount++;
				else if(Spt->Pdir == 0 || Spt->Pdir == 2)
					IDCount--;
			}
		}
		if(IDCount > 0)
			this->AddIntercept(true);
		else
			this->AddIntercept(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0017", __FILE__, __FUNCSIG__); }
}

void Line::updateLine_UsingPorbe()
{
	try
	{

		double rangle, PtX, PtY, PtZ, Pradius = 0;
		rangle = this->Angle() + M_PI_2;
		if(rangle > M_PI) rangle -= M_PI;
		if(AddIntercept())
		{
			for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				Pradius = getProbeRadius(Spt->PLR);
				if(Spt->Pdir == 4)  // Z-axis moving down//
				{
					PtX = Spt->X; PtY = Spt->Y; PtZ = Spt->Z - Pradius;
				}
				else if(Spt->Pdir == 5)
				{
					PtX = Spt->X; PtY = Spt->Y;	PtZ = Spt->Z + Pradius;
				}
				else
				{
					PtX = Spt->X + cos(rangle) * Pradius;
					PtY = Spt->Y + sin(rangle) * Pradius;
					PtZ = Spt->Z;
				}
				SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
				Spt1->SetValues(PtX, PtY, PtZ, Spt->R, Spt->Pdir);
			}
		}
		else
		{
			for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				Pradius = getProbeRadius(Spt->PLR);
				if(Spt->Pdir == 4)  // Z-axis moving down//
				{
					PtX = Spt->X; PtY = Spt->Y; PtZ = Spt->Z - Pradius;
				}
				else if(Spt->Pdir == 5)
				{
					PtX = Spt->X; PtY = Spt->Y;	PtZ = Spt->Z + Pradius;
				}
				else
				{
					PtX = Spt->X - cos(rangle) * Pradius;
					PtY = Spt->Y - sin(rangle) * Pradius;
					PtZ = Spt->Z;
				}
				SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
				Spt1->SetValues(PtX, PtY, PtZ, Spt->R, Spt->Pdir);
			}
		}
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
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
		if(this->ApplyMMCCorrection() && (!this->ScanDirDetected()))
		{
			switch((int)(this->PointsListOriginal->getList().begin()->second->Pdir))
			{
			   case 0:
				 this->ScanDir[0] = 1;
				 break;
			   case 1:
				 this->ScanDir[0] = -1;
				 break;
			   case 2:
				 this->ScanDir[1] = 1;
				 break;
			   case 3:
				 this->ScanDir[1] = -1;
				 break;
			   case 4:
				 this->ScanDir[2] = 1;
				 break;
			   case 5:
				 this->ScanDir[2] = -1;
				 break;
			}
			this->ScanDirDetected(true);
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
		//	double rangle, PtX, PtY, PtZ, Pradius = 0;
		//	rangle = this->Angle() + M_PI_2;
		//	if(rangle > M_PI) rangle -= M_PI;
		//	if(AddIntercept())
		//	{
		//		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		//		{
		//			SinglePoint* Spt = (*Item).second;
		//			Pradius = getProbeRadius(Spt->PLR);
		//			if(Spt->Pdir == 4)  // Z-axis moving down//
		//			{
		//				PtX = Spt->X; PtY = Spt->Y; PtZ = Spt->Z - Pradius;
		//			}
		//			else if(Spt->Pdir == 5)
		//			{
		//				PtX = Spt->X; PtY = Spt->Y;	PtZ = Spt->Z + Pradius;
		//			}
		//			else
		//			{
		//				PtX = Spt->X + cos(rangle) * Pradius;
		//				PtY = Spt->Y + sin(rangle) * Pradius;
		//				PtZ = Spt->Z;
		//			}
		//			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
		//			Spt1->SetValues(PtX, PtY, PtZ, Spt->R, Spt->Pdir);
		//		}
		//	}
		//	else
		//	{
		//		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		//		{
		//			SinglePoint* Spt = (*Item).second;
		//			Pradius = getProbeRadius(Spt->PLR);
		//			if(Spt->Pdir == 4)  // Z-axis moving down//
		//			{
		//				PtX = Spt->X; PtY = Spt->Y; PtZ = Spt->Z - Pradius;
		//			}
		//			else if(Spt->Pdir == 5)
		//			{
		//				PtX = Spt->X; PtY = Spt->Y;	PtZ = Spt->Z + Pradius;
		//			}
		//			else
		//			{
		//				PtX = Spt->X - cos(rangle) * Pradius;
		//				PtY = Spt->Y - sin(rangle) * Pradius;
		//				PtZ = Spt->Z;
		//			}
		//			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
		//			Spt1->SetValues(PtX, PtY, PtZ, Spt->R, Spt->Pdir);
		//		}
		//	}
		//}
		//if(this->pts != NULL){free(pts); pts = NULL;}
		//this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		//int n = 0;
		//for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
		//{
		//	SinglePoint* Pt = (*Spt).second;
		//	this->pts[n++] = Pt->X;
		//	this->pts[n++] = Pt->Y;
		//}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0018", __FILE__, __FUNCSIG__); }
}

void Line::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(LineType == RapidEnums::LINETYPE::FINITELINE || LineType == RapidEnums::LINETYPE::INFINITELINE || LineType == RapidEnums::LINETYPE::RAYLINE || 
			LineType == RapidEnums::LINETYPE::FINITELINE3D)
		{
			//modified this to handle dependent shapes - May 29 2012 
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
									try
									{
										SinglePoint* Spt = this->PointsList->getList()[(int)(*(((AddPointAction*)action)->getFramegrab()->PointActionIdList.begin()))];
										Spt->SetValues(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getX(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getY(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getZ());
										Spt = this->PointsListOriginal->getList()[(int)(*(((AddPointAction*)action)->getFramegrab()->PointActionIdList.begin()))];
										Spt->SetValues(((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getX(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getY(), ((AddPointAction*)action)->getFramegrab()->Pointer_SnapPt->getZ());
									}
									catch (...)
									{
										MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0019", __FILE__, __FUNCSIG__);
									}
									if (i == cnt) break;
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
							if(this->PointsList->Pointscount() < 1) continue;
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
		else if(LineType == RapidEnums::LINETYPE::INTERSECTIONLINE3D)
		{
		   UpdateIntersectionLine3D();
		}
		else if(LineType == RapidEnums::LINETYPE::PROJECTIONLINE3D)
			UpdateProjectionLine3D();
		else if(LineType == RapidEnums::LINETYPE::LINEFROMCLOUDPNTS)
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
		else
			updateDerivedLines();
	}
	catch(...) //std::exception &ex)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0019", __FILE__, __FUNCSIG__); //ex.what()); //
	}
}

void Line::setPoint1(Vector& t)
{
	point1.set(t);
}

void Line::setPoint2(Vector& t)
{
	point2.set(t);
}

void Line::setMidPoint(Vector& t)
{
	Midpoint.set(t);
}
  
Vector* Line::getPoint1()
{
	return &(this->point1);
}

Vector* Line::getPoint2()
{
	return &(this->point2);
}

Vector* Line::getMidPoint()
{
	return &(this->Midpoint);
}

void Line::setLineParameters(double l, double m, double n)
{
	dir_l(l); dir_m(m); dir_n(n);
}

void Line::setLineParameters(double ang, double intrcpt)
{
	Angle(ang); Intercept(intrcpt);
}

void Line::calculateAttributes()
{
	try
	{
		this->Length(sqrt(pow(this->point1.getX() - this->point2.getX(), 2) + pow(this->point1.getY() - this->point2.getY(), 2) + pow(this->point1.getZ() - this->point2.getZ(), 2)));	
		if(!(this->LineType == RapidEnums::LINETYPE::LINEINTWOLINE  && this->LineType == RapidEnums::LINETYPE::LINEINTWOARC))
		{
			this->Angle(atan((this->point2.getY() - this->point1.getY())/(this->point2.getX() - this->point1.getX())));
			if(this->LineType != RapidEnums::LINETYPE::RAYLINE)
			{
				//Added on 16/02/09... angle should be between 0 to 180...//
				if(this->Angle() < 0) Angle(Angle() + M_PI);
				else if(this->Angle() > M_PI) Angle(Angle() - M_PI);
			}
			if(abs(this->Angle() - M_PI_2) > 0.00058)
			{
				this->Intercept(this->point1.getY() - tan(this->Angle()) * this->point1.getX());
			}
			else
			{
				this->Angle(M_PI_2);
				this->Intercept(this->point1.getX());
			}
			/*if(abs(this->point2.getX() - this->point1.getX()) > 0.0001)
			{
				this->Angle(atan((this->point2.getY() - this->point1.getY())/(this->point2.getX() - this->point1.getX())));
				this->Intercept(this->point1.getY() - tan(this->Angle()) * this->point1.getX());
			}
			else
			{
				this->Angle(M_PI_2);
				this->Intercept(this->point1.getX());
			}*/
		}
		if(this->LineType != RapidEnums::LINETYPE::RAYLINE)
		{
			//Added on 16/02/09... angle should be between 0 to 180...//
			if(this->Angle() < 0) Angle(Angle() + M_PI);
			else if(this->Angle() > M_PI) Angle(Angle() - M_PI);
		}
		this->dir_l((point2.getX() - point1.getX()) / Length());
		this->dir_m((point2.getY() - point1.getY()) / Length());
		this->dir_n((point2.getZ() - point1.getZ()) / Length());
		this->Midpoint.set((point1.getX() + point2.getX())/2, (point1.getY() + point2.getY())/2, (point1.getZ() + point2.getZ())/2);
		if(this->ShapeAs3DShape())
		{
			double lineparam[6] = {point1.getX(), point1.getY(), point1.getZ(), dir_l(), dir_m(), dir_n()};
			this->Straightness(RMATH3DOBJECT->Straightness(this->pts, this->PointsList->Pointscount(), &lineparam[0]));
			this->FilteredStraightness(RMATH3DOBJECT->FilteredStraightness(this->pts, this->PointsList->Pointscount(), &lineparam[0], FlatnessFilterFactor()));
		}
		else
		{
			this->Straightness(RMATH2DOBJECT->StraightNessofLine(this->PointsList->Pointscount(), this->pts, Angle(),Intercept()));
			this->FilteredStraightness(RMATH2DOBJECT->FilteredStraightNessofLine(this->PointsList->Pointscount(), this->pts, Angle(),Intercept(), FlatnessFilterFactor()));
		}
		if(this->LineType == RapidEnums::LINETYPE::ANGLE_BISECTORL)
		{
			if(this->getParents().size() < 2)
				return;
			list<BaseItem*>::iterator ptr;
			Shape *Tmpparent1, *Tmpparent2;
			ptr = this->getParents().begin();
			Tmpparent1 = (Shape*)(*ptr);
			ptr++;
			Tmpparent2 = (Shape*)(*ptr);
			RAxisVal = (((ShapeWithList*)Tmpparent1)->RAxisVal + ((ShapeWithList*)Tmpparent2)->RAxisVal) / 2;
		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0020", __FILE__, __FUNCSIG__); }
}

Shape* Line::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
		{
			if(this->ShapeType == RapidEnums::SHAPETYPE::LINE)
				myname = _T("L");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
				myname = _T("L3D");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
				myname = _T("XL3D");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::XLINE)
				myname = _T("XL");
			else
				myname = _T("R");
		}
		else
		{
			if(this->ShapeType == RapidEnums::SHAPETYPE::LINE)
				myname = _T("dL");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
				myname = _T("dL3D");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
				myname = _T("XdL3D");
			else if(this->ShapeType == RapidEnums::SHAPETYPE::XLINE)
				myname = _T("dXL");
			else
				myname = _T("dR");
		}
		Line* CShape = new Line((TCHAR*)myname.c_str(), this->ShapeType);
		if(copyOriginalProperty)
		   CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0021", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Line::CreateDummyCopy()
{
	try
	{
		Line* CShape = new Line(false, this->ShapeType);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0022", __FILE__, __FUNCSIG__); return NULL; }
}

void Line::CopyShapeParameters(Shape* s)
{
	try
	{
		Line *OrgShape = (Line*)s;
		if(this->getMCSParentShape() != NULL)
		{
			if(this->ShapeType == RapidEnums::SHAPETYPE::LINE)
				this->LineType = RapidEnums::LINETYPE::FINITELINE;
			else if(this->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
				this->LineType = RapidEnums::LINETYPE::FINITELINE3D;
			else if(this->ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
				this->LineType = RapidEnums::LINETYPE::INFINITELINE3D;
			else if(this->ShapeType == RapidEnums::SHAPETYPE::XLINE)
				this->LineType = RapidEnums::LINETYPE::RAYLINE;
			else if(this->ShapeType == RapidEnums::SHAPETYPE::XRAY)
				this->LineType = RapidEnums::LINETYPE::INFINITELINE;
		}
		else
			this->LineType = OrgShape->LineType;
		this->LinePosition(OrgShape->LinePosition());
		this->setPoint1(*(OrgShape->getPoint1()));
		this->setPoint2(*(OrgShape->getPoint2()));
		UCS* ucs = MAINDllOBJECT->getUCS(MAINDllOBJECT->getCurrentUCS().getId());
		Vector* Vpt = MAINDllOBJECT->getVectorPointer_UCS(((Line*)s)->VPointer, ucs);
		if(Vpt != NULL)
			this->VPointer = Vpt;
		this->calculateAttributes();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0023", __FILE__, __FUNCSIG__); }
}

void Line::Transform(double* transform)
{
	try
	{
		setPoint1(MAINDllOBJECT->TransformMultiply(transform, getPoint1()->getX(), getPoint1()->getY(), getPoint1()->getZ()));
		setPoint2(MAINDllOBJECT->TransformMultiply(transform, getPoint2()->getX(), getPoint2()->getY(), getPoint2()->getZ()));
		calculateAttributes();
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0024", __FILE__, __FUNCSIG__); }
}

void Line::Translate(Vector& Position)
{
	point1 += Position;
	point2 += Position;
	calculateAttributes();
	this->UpdateEnclosedRectangle();
}

void Line::AlignToBasePlane(double* trnasformM)
{
	try
	{
		setPoint1(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, point1.getX(), point1.getY(), point1.getZ()));
		setPoint2(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, point2.getX(), point2.getY(), point2.getZ()));
		calculateAttributes();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0024", __FILE__, __FUNCSIG__); }
}


void Line::UpdateEnclosedRectangle()
{
	try
	{
		double endp1[3] = {0}, endp2[3] = {0};
		point1.FillDoublePointer(&endp1[0], 3); point2.FillDoublePointer(&endp2[0], 3);
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			 int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			 double TransMatrix[9] = {0}, temp_endpt1[3] = {point1.getX(), point1.getY(), point1.getZ()}, temp_endpt2[3] = {point2.getX(), point2.getY(), point2.getZ()};
			 RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			 TransMatrix[2] = 0; TransMatrix[5] = 0;
			 RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_endpt1, Order2, endp1);
			 RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_endpt2, Order2, endp2);		
		}
		RMATH2DOBJECT->RectangleEnclosing_Line(&endp1[0], &endp2[0], &ShapeLeftTop[0], &ShapeRightBottom[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0024", __FILE__, __FUNCSIG__); }
}

void Line::GetShapeCenter(double *cPoint)
{
	cPoint[0] = Midpoint.getX();
	cPoint[1] = Midpoint.getY();
	cPoint[2] = Midpoint.getZ();
}

void Line::getParameters(double* lineparam)
{
	point1.FillDoublePointer(lineparam, 3);
	lineparam[3] = dir_l(); lineparam[4] = dir_m(); lineparam[5] = dir_n();
}

void Line::getEndPoints(double* endpoints)
{
	point1.FillDoublePointer(endpoints, 3);
	point2.FillDoublePointer(endpoints + 3, 3);
}

void Line::SetProbeCorrectionShape(Shape* Shp)
{
	try
	{
		ProbeCorrectionShape = Shp;
		this->addParent(ProbeCorrectionShape);
		ProbeCorrectionShape->addChild(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0028", __FILE__, __FUNCSIG__); }
}

Shape* Line::GetProbeCorrectionShape()
{
	return ProbeCorrectionShape;
}

int Line::Lshapenumber = 0;
int Line::XLshapenumber = 0;
int Line::Rshapenumber = 0;
int Line::LDXFshapenumber = 0;
int Line::XLDXFshapenumber = 0;
int Line::RDXFshapenumber = 0;
int Line::L3dshapenumber = 0;
int Line::L3dDXFshapenumber = 0;
int Line::XL3dshapenumber = 0;
int Line::XL3dDXFshapenumber = 0;

void Line::decrement()
{
	XLshapenumber--;
}

void Line::reset()
{
	Lshapenumber = 0; XLshapenumber = 0; Rshapenumber = 0; XL3dshapenumber = 0;
	LDXFshapenumber = 0; XLDXFshapenumber = 0; RDXFshapenumber = 0;
	L3dshapenumber = 0; L3dDXFshapenumber = 0; XL3dDXFshapenumber = 0;

}

wostream& operator<<(wostream& os, Line& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os<< "Line" << endl;
		os << "ShapeType:" << x.ShapeType << endl;
		os << "Point1:value"<< endl << (*static_cast<Vector*>(x.getPoint1())) << endl;
		os << "Point2:value"<< endl << (*static_cast<Vector*>(x.getPoint2())) << endl;
		os << "Intercept:" << x.Intercept() << endl;
		os << "Angle:" << x.Angle() << endl;
		os << "SetAsCriticalShape:" << x.SetAsCriticalShape() << endl;
		os << "Offset:" << x.Offset() << endl;
		os << "Length:" << x.Length() << endl;
		os << "DefinedOffset:" << x.DefinedOffset() << endl;
		os << "RAxisVal:" << x.RAxisVal << endl;
		os << "DefinedLength:" << x.DefinedLength() << endl;
		os << "LineType:" << x.LineType << endl;
		os << "LinePosition:" << x.LinePosition() << endl;
		if(x.ProbeCorrectionShape != NULL)
		{
			os << "ProbeCorrectionShape:" << x.ProbeCorrectionShape->getId() << endl;
		}
		if(x.LineType == RapidEnums::LINETYPE::TANGENT_THRU_POINT)
			os << "VPointer:value"<< endl << (*static_cast<Vector*>(x.VPointer)) << endl;
		os<< "EndLine" <<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0025", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Line& x)
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
			if(Tagname==L"Line")
			{
			while(Tagname!=L"EndLine")
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
						else if(Tagname==L"Point1")
						{
							is >> (*static_cast<Vector*>(x.getPoint1()));
						}
						else if(Tagname==L"Point2")
						{
							is >> (*static_cast<Vector*>(x.getPoint2()));
						}
						else if(Tagname==L"Intercept")
						{
							x.Intercept(atof((const char*)(Val).c_str()));						
						}
						else if(Tagname==L"Angle")
						{
							 x.Angle(atof((const char*)(Val).c_str()));
							 x.BuildAngle(x.Angle());
						}
						else if(Tagname==L"SetAsCriticalShape")
						{
							if(Val=="0")
								x.SetAsCriticalShape(false);
							else
								x.SetAsCriticalShape(true);	
						}
						else if(Tagname==L"Offset")
						{
							x.Offset(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"Length")
						{
							 x.Length(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"DefinedOffset")
						{
							if(Val=="0")
								x.DefinedOffset(false);
							else
								x.DefinedOffset(true);						 
						}
						else if(Tagname==L"RAxisVal")
						{
							x.RAxisVal = atof((const char*)(Val).c_str());
						}
						else if(Tagname==L"FilteredLine")
						{
							if(Val=="0")
								x.ApplyFilterCorrection(false);
							else
								x.ApplyFilterCorrection(true);						 
						}
						else if(Tagname==L"DefinedLength")
						{
							 if(Val=="0")
								x.DefinedLength(false);
							else
								x.DefinedLength(true);	
						}
						else if(Tagname==L"LineType")
						{
							x.LineType = RapidEnums::LINETYPE(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"LinePosition")
						{
							x.LinePosition(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"ProbeCorrectionShape")
						{
							int id = atoi((const char*)(Val).c_str());
							x.ProbeCorrectionShape = (Shape*)(MAINDllOBJECT->getUCS(x.UcsId())->getShapes().getList()[id]);
						}
						else if(Tagname==L"VPointer")
						{
							is >> (*static_cast<Vector*>(x.VPointer));
						}
						else if(Tagname==L"OutermostLine")
						{
							 if(Val=="0")
								x.ApplyMMCCorrection(false);
							else
								x.ApplyMMCCorrection(true);	
						}
						else if(Tagname==L"OutermostSide")
						{
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0026", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, Line& x)
{
	try
	{
		wstring name;
		int n; double d; bool flag;
		is >> n;
		x.ShapeType = RapidEnums::SHAPETYPE(n);
		is >> (*static_cast<Vector*>(x.getPoint1()));
		is >> (*static_cast<Vector*>(x.getPoint2()));
		is >> d; x.Intercept(d);
		is >> d; x.Angle(d);
		is >> d; x.Offset(d);
		is >> d; x.RAxisVal = d;
		is >> d; x.Length(d);
		is >> flag; x.DefinedOffset(flag);
		is >> flag; x.DefinedLength(flag);
		is >> n; x.LineType = RapidEnums::LINETYPE(n);
		is >> n; x.LinePosition(n);
		if(x.LineType == RapidEnums::LINETYPE::TANGENT_THRU_POINT)
		{
			is >> (*static_cast<Vector*>(x.VPointer));
		}
		x.calculateAttributes();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0026", __FILE__, __FUNCSIG__); }
}
