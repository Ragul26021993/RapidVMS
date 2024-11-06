#include "stdafx.h"
#include "Cylinder.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

Cylinder::Cylinder(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

Cylinder::Cylinder(bool simply):ShapeWithList(false)
{
	init();
}

Cylinder::~Cylinder()
{
}

void Cylinder::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::CYLINDER;
		this->Radius1(0); this->Radius2(0);
		this->dir_l(0); this->dir_m(0); this->dir_n(0);
		this->MinInscribedia(0); this->MaxCircumdia(0); this->Cylindricity(0); this->Length(0);
		this->FilteredCylindricity(0);
		this->phi_max(2*M_PI); this->phi_min(0);
		this->ShapeAs3DShape(true); this->IdCylinder(true);
		this->UseLightingProperties(true);
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center1), 0), 0);
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center2), 1), 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0001", __FILE__, __FUNCSIG__); }
}

TCHAR* Cylinder::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		shapenumber++;
		_itot_s(shapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0002", __FILE__, __FUNCSIG__); return name; }
}

void Cylinder::UpdateBestFit()
{
	try
	{
		this->IsValid(false);
		int totcount = this->PointsListOriginal->Pointscount();
		if(totcount < 5) return;
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
		if(!updateCylinder(totcount))
		{
			if(Radius1() > 0)this->IsValid(true);
			else this->IsValid(false);
			return;
		}
		ProbePointCalculations(totcount);
		calculateAttributes();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0003", __FILE__, __FUNCSIG__); }
}

 bool Cylinder::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		double cent1[4] = {center1.getX(), center1.getY(), center1.getZ(), 1}, cent2[4] = {center2.getX(), center2.getY(), center2.getZ(), 1};
		double  endp1[4] = {0}, endp2[4] = {0}, Lefttop1[4] = {0},  Rightbottom1[4] = {0},  Lefttop2[4] = {0}, Rightbottom2[4] = {0};
		RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &cent1[0], &s2[0], &endp1[0]);
		RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &cent2[0], &s2[0], &endp2[0]);
		RMATH2DOBJECT->RectangleEnclosing_Circle(&endp1[0], Radius1(), &Lefttop1[0], &Rightbottom1[0]);
		RMATH2DOBJECT->RectangleEnclosing_Circle(&endp2[0], Radius2(), &Lefttop2[0], &Rightbottom2[0]);
		double PtX[4] = {Lefttop1[0],  Rightbottom1[0],  Lefttop2[0], Rightbottom2[0]}, PtY[4] = {Lefttop1[1],  Rightbottom1[1],  Lefttop2[1], Rightbottom2[1]}; 
		*Lefttop = RMATH2DOBJECT->minimum(&PtX[0], 4);
		*(Lefttop + 1) = RMATH2DOBJECT->maximum(&PtY[0], 4);
		*Rightbottom = RMATH2DOBJECT->maximum(&PtX[0], 4);
		*(Rightbottom + 1) = RMATH2DOBJECT->minimum(&PtY[0], 4);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0003", __FILE__, __FUNCSIG__); return false;}
}

bool Cylinder::updateCylinder(int PointsCount)
{
	try
	{
		double ans[7] = {0}, endpts[6] = {0}, minr, maxr;
		if (PointsCount > 5)
		{
			center1.FillDoublePointer(ans,3);
			ans[3] = dir_l(); ans[4] = dir_m(); ans[5] = dir_n();
			ans[6] = Radius1();
		}
		bool flag = BESTFITOBJECT->Cylinder_BestFit(pts, PointsCount, &ans[0], 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
		if(!flag) return false;
		HELPEROBJECT->CalculateEndPoints_3DLineBestfit(PointsCount, pts, &ans[0], &endpts[0], &endpts[3], &minr, &maxr);
		if(ApplyFilterCorrection() && PointsCount > 6)
		{
		    int cnt = PointsCount;
		    for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
		    {
				flag = false;
				cnt = HELPEROBJECT->FilteredCylinder(PointsList, ans, endpts, FilterCorrectionFactor(), i);
				if(cnt < 7) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				filterptsCnt = 0;
				int j = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					if(Pt->InValid) continue;
					this->filterpts[j++] = Pt->X;
					this->filterpts[j++] = Pt->Y;
					this->filterpts[j++] = Pt->Z;
					filterptsCnt++;
		        }
				flag = BESTFITOBJECT->Cylinder_BestFit(filterpts, filterptsCnt, &ans[0], 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				if(!flag){ return false;}
				HELPEROBJECT->CalculateEndPoints_3DLineBestfit(filterptsCnt, filterpts, &ans[0], &endpts[0], &endpts[3], &minr, &maxr);
			}
		}
		if(ApplyMMCCorrection() && ScanDirDetected() && PointsCount > 6)
		{
			int cnt = PointsCount;
		    for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
		    {
				flag = false;
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
			    cnt = HELPEROBJECT->OutermostInnerMostCylinder(PointsList, ans, endpts, useValidPointOnly, OuterCorrection());
				if(cnt < 7) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				filterptsCnt = 0;
				int j = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					if(Pt->InValid) continue;
					this->filterpts[j++] = Pt->X;
					this->filterpts[j++] = Pt->Y;
					this->filterpts[j++] = Pt->Z;
					filterptsCnt++;
		        }
				flag = BESTFITOBJECT->Cylinder_BestFit(filterpts, filterptsCnt, &ans[0], 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				if(!flag) { return false; }
				HELPEROBJECT->CalculateEndPoints_3DLineBestfit(filterptsCnt, filterpts, &ans[0], &endpts[0], &endpts[3], &minr, &maxr);
			}
		}
	   /*double nval = sqrt(pow(ans[3], 2) + pow(ans[4], 2) + pow(ans[5], 2));
		SetCylinderProperties(ans[3]/nval, ans[4]/nval, ans[5]/nval);*/
		center1.set(endpts[0], endpts[1], endpts[2]);
		center2.set(endpts[3], endpts[4], endpts[5]);
		Radius1(ans[6]); Radius2(ans[6]);

		double boundaryangles[2] = {0};
		if(PartialShape())
		{
			if(!BESTFITOBJECT->getConeBoundaryAngles(filterpts, filterptsCnt, ans, boundaryangles))
				return false;
			phi_min(boundaryangles[0]);
			phi_max(boundaryangles[1]);
		}
		this->dir_l(ans[3]);
		this->dir_m(ans[4]);
		this->dir_n(ans[5]);
		updateParameters();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0004", __FILE__, __FUNCSIG__); return false;}
}

void Cylinder::updateParameters()
{
	try
	{
		this->Length(sqrt(pow(center2.getX() - center1.getX(), 2) + pow(center2.getY() - center1.getY(), 2) + pow(center2.getZ() - center1.getZ(), 2)));
		/*this->dir_l((center2.getX() - center1.getX()) / Length());
		this->dir_m((center2.getY() - center1.getY()) / Length());
		this->dir_n((center2.getZ() - center1.getZ()) / Length());*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0004", __FILE__, __FUNCSIG__);}
}

void Cylinder::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt >= 5) // && PointsCnt < 10)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdCylinder();
		}
		if(ShapeDoneUsingProbe())
		{
			updateCylinder_UsingPorbe();
			updateCylinder(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0005", __FILE__, __FUNCSIG__); }
}

void Cylinder::CheckPointIsForPorbe()
{
	try
	{
		if(PointAtionList.size() < 1) return;
		this->ShapeDoneUsingProbe(false);
		if(((RAction*)(*PointAtionList.begin()))->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			AddPointAction* Caction = (AddPointAction*)(*PointAtionList.begin());
			if(Caction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
			{
				if(this->ProbeCorrection())
					this->ShapeDoneUsingProbe(true);
				else
					this->ShapeDoneUsingProbe(false);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0006", __FILE__, __FUNCSIG__); }
}

void Cylinder::CheckIdorOdCylinder()
{
	try
	{
		int IDCount = 0;
		PointCollection ptCol;
		Vector CenterMcs;
		HELPEROBJECT->GetMcsPoint(&center1, &CenterMcs);
		HELPEROBJECT->GetMcsPointsList(this->PointsListOriginal, &ptCol);
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
			else if(Spt->Pdir == 4) 
			{				
				if(Spt->Z > CenterMcs.getZ())
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 5) 
			{				
				if(Spt->Z > CenterMcs.getZ())
					IDCount++;
				else
					IDCount--;
			}
	    }
		ptCol.deleteAll();
		if(IDCount > 0)
			this->IdCylinder(true);
		else
			this->IdCylinder(false);
		if(this->ApplyMMCCorrection() && (!this->ScanDirDetected()))
		{
			this->OuterCorrection(!this->IdCylinder());
			this->ScanDirDetected(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0007", __FILE__, __FUNCSIG__); }
}

void Cylinder::updateCylinder_UsingPorbe()
{
	try
	{
		double Pt[3];;
		double Pradius = 0;
		double CylinderParam[7] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1()};
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};
			Pradius = getProbeRadius(Spt->PLR);
			RMATH3DOBJECT->Cylinder_SurfacePt_for_Probe(&CylinderParam[0], &PrPoint[0], Pradius, IdCylinder(), &Pt[0]);
			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(Pt[0], Pt[1], Pt[2], Spt->R, Spt->Pdir);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0008", __FILE__, __FUNCSIG__); }
}

void Cylinder::UpdateForParentChange(BaseItem* sender)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0009", __FILE__, __FUNCSIG__); }
}

void Cylinder::calculateAttributes()
{
	try
	{
		int PointsCount = this->PointsList->Pointscount();
		double Npt[3];
		double CylinderParam[7] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1()};
		double dist1 = RMATH3DOBJECT->Nearest_Point_to_Cylinder(pts, PointsCount, &CylinderParam[0], &Npt[0]);
		this->MinInscribedia(dist1 + Radius1());
		double dist2 = RMATH3DOBJECT->Farthest_Point_to_Cylinder(pts, PointsCount, &CylinderParam[0], &Npt[0]);
		this->MaxCircumdia(dist2 + Radius1());
		this->Cylindricity(RMATH3DOBJECT->Cylindricity(pts, PointsCount, &CylinderParam[0]));
		this->FilteredCylindricity(RMATH3DOBJECT->FilteredCylindricity(pts, PointsCount, &CylinderParam[0], this->FlatnessFilterFactor()));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0010", __FILE__, __FUNCSIG__); }
}

void Cylinder::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		int totcount = this->PointsList->Pointscount();
		bool allowTransparency = MAINDllOBJECT->AllowTransparencyTo3DShapes();
		if(HighlightedFormouse()) allowTransparency = false;

		GRAFIX->drawLine_3DStipple(center1.getX(), center1.getY(), center1.getZ(), center2.getX(), center2.getY(), center2.getZ());
		if(PartialShape())
		{
			//use a settings option to toggle between the three options below.
			if(PartialShapeDrawMode() == 0)
				GRAFIX->draw_CylinderPartial(center1.getX(), center1.getY(), center1.getZ(), center2.getX(), center2.getY(), center2.getZ(), Radius1(), Radius2(), phi_min(), phi_max(), MAINDllOBJECT->DrawWiredSurface(), allowTransparency, WPixelWidth);
			else if(PartialShapeDrawMode() == 1)
				GRAFIX->draw_CylinderFromPts(center1.getX(), center1.getY(), center1.getZ(), center2.getX(), center2.getY(), center2.getZ(), Radius1(), Radius2(), pts, totcount, MAINDllOBJECT->DrawWiredSurface(), allowTransparency, WPixelWidth);
			else
				GRAFIX->draw_Cylinder(center1.getX(), center1.getY(), center1.getZ(), center2.getX(), center2.getY(), center2.getZ(), Radius1(), Radius2(), MAINDllOBJECT->DrawWiredSurface(), allowTransparency, false, WPixelWidth);
		}
		else
		{
			GRAFIX->draw_Cylinder(center1.getX(), center1.getY(), center1.getZ(), center2.getX(), center2.getY(), center2.getZ(), Radius1(), Radius2(), MAINDllOBJECT->DrawWiredSurface(), allowTransparency, MAINDllOBJECT->DrawCylinderdisks(), WPixelWidth);
			GRAFIX->drawPoint(center1.getX(), center1.getY(), center1.getZ(),5);
			GRAFIX->drawPoint(center2.getX(), center2.getY(), center2.getZ(),5);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0011", __FILE__, __FUNCSIG__); }
}

void Cylinder::drawGDntRefernce()
{
	try
	{
		if(RefernceDatumForgdnt() > 0)
		{
			double CyParam[7], CyEndpts[6];
			getParameters(&CyParam[0]);
			getEndPoints(&CyEndpts[0]);
			GRAFIX->drawReferenceDatumName_Cylinder(&CyParam[0], &CyEndpts[0], &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0012", __FILE__, __FUNCSIG__); }
}

void Cylinder::ResetShapeParameters()
{
	try
	{
		this->Radius1(0); this->Radius2(0);
		this->dir_l(0); this->dir_m(0); this->dir_n(0);
		this->MinInscribedia(0); this->MaxCircumdia(0); this->Cylindricity(0); this->Length(0);
		this->phi_max(2*M_PI); this->phi_min(0);
		this->FilteredCylindricity(0);
		this->ShapeAs3DShape(true); this->IdCylinder(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0013", __FILE__, __FUNCSIG__); }
}

bool Cylinder::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
		return Shape_IsInWindow(&SelectionL[0], Tolerance);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0014", __FILE__, __FUNCSIG__); return false; }
}

bool Cylinder::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool Cylinder::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		double ThisCylinder[7] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1()};
		double ThePoints[6] = {center1.getX(), center1.getY(), center1.getZ(), center2.getX(), center2.getY(), center2.getZ()};
		double ProjctnPt[3] = {0}, dist1 = 0, dist2 = 0;
		dist1 = RMATH3DOBJECT->Distance_Point_Line(ThePoints, SelectionLine);
		dist2 = RMATH3DOBJECT->Distance_Point_Line(&ThePoints[3], SelectionLine);
		if((dist1 <= Radius1() / 4) || (dist2 <= Radius2() / 4))
		{
			if(MAINDllOBJECT->AllowTransparencyTo3DShapes())
			{
				MAINDllOBJECT->UpdateGrafixFor3DSnapPoint(true);
				return false;
			}
		}
		this->Intersects = RMATH3DOBJECT->Intersection_Line_Cylinder(SelectionLine, ThisCylinder, IntersectnPts);
		if(Intersects == 1 || Intersects == 2)
		{
			RMATH3DOBJECT->Projection_Point_on_Line(&IntersectnPts[0], ThisCylinder, ProjctnPt);
			if(RMATH3DOBJECT->Checking_Point_within_Block(ProjctnPt, ThePoints, 2))
				return true;
			if(Intersects == 2)
			{
				RMATH3DOBJECT->Projection_Point_on_Line(&IntersectnPts[3], ThisCylinder, ProjctnPt);
				if(RMATH3DOBJECT->Checking_Point_within_Block(ProjctnPt, ThePoints, 2))
					return true;
			}
		}

		double PlaneOfArc1[4] = {dir_l(), dir_m(), dir_n(), dir_l() * center1.getX() +  dir_m() * center1.getY() + dir_n() * center1.getZ()};
		double PlaneOfArc2[4] = {dir_l(), dir_m(), dir_n(), dir_l() * center2.getX() +  dir_m() * center2.getY() + dir_n() * center2.getZ()};
		Intersects = RMATH3DOBJECT->Intersection_Line_Plane(SelectionLine, PlaneOfArc1, IntersectnPts);
		if(Intersects == 1)
		{
			double dis1 = RMATH3DOBJECT->Distance_Point_Point(ThePoints, IntersectnPts);
			if(dist1 < Radius1())
				return true;
		}
		Intersects = RMATH3DOBJECT->Intersection_Line_Plane(SelectionLine, PlaneOfArc2, IntersectnPts);
		if(Intersects == 1)
		{
			double dis1 = RMATH3DOBJECT->Distance_Point_Point(&ThePoints[3], IntersectnPts);
			if(dist1 < Radius1())
				return true;
		}	
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0015", __FILE__, __FUNCSIG__); return false; }
}

void Cylinder::SetCenter1(Vector& t)
{
	this->center1.set(t);
}

void Cylinder::SetCenter2(Vector& t)
{
	this->center2.set(t);
}

void Cylinder::SetCenters(Vector& c1, Vector& c2)
{
	this->center1.set(c1); this->center1.set(c2);
}

void Cylinder::SetRadius(double r1,double r2)
{
	Radius1(r1); Radius2(r2);
}

void Cylinder::SetCylinderProperties(double l, double m, double n)
{
	dir_l(l); dir_m(m); dir_n(n);
}

Vector* Cylinder::getCenter1()
{
	return (&this->center1);
}

Vector* Cylinder::getCenter2()
{
	return (&this->center2);
}

void Cylinder::getParameters(double* cylinderparam)
{
	center1.FillDoublePointer(cylinderparam, 3);
	cylinderparam[3] = dir_l(); cylinderparam[4] = dir_m(); cylinderparam[5] = dir_n();
	cylinderparam[6] = Radius1();
}

void Cylinder::getAxisLine(double *LineParam3D)
{
	center1.FillDoublePointer(LineParam3D, 3);
	LineParam3D[3] = dir_l(); LineParam3D[4] = dir_m(); LineParam3D[5] = dir_n();
}


void Cylinder::getEndPoints(double* endpoints)
{
	center2.FillDoublePointer(endpoints, 3);
	center1.FillDoublePointer(endpoints + 3, 3);
}

void Cylinder::getSurfaceLine(double *dir, bool forward, double *SFline, double *SFlineEndpts)
{
	try
	{
		int dirFactr = -1;
		if(forward) dirFactr = 1;
		double cen1[3] = {center1.getX(), center1.getY(), center1.getZ()}, cen2[3] = {center2.getX(), center2.getY(), center2.getZ()};
		for(int i = 0; i < 3; i++)
		{
			SFline[i] = cen1[i] + dirFactr * dir[i] * Radius1();
			SFlineEndpts[i] = SFline[i];
			SFlineEndpts[i + 3] = cen2[i] + dirFactr * dir[i] * Radius1();
		}
		SFline[3] = dir_l(); SFline[4] = dir_m(); SFline[5] = dir_n(); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0016", __FILE__, __FUNCSIG__); }
}

Shape* Cylinder::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("CL");
		else
			myname = _T("dCL");
		Cylinder* CShape = new Cylinder((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		    CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0017", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Cylinder::CreateDummyCopy()
{
	try
	{
		Cylinder* CShape = new Cylinder(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0018", __FILE__, __FUNCSIG__); return NULL; }
}

void Cylinder::CopyShapeParameters(Shape* s)
{
	try
	{
		Cylinder* Ccyl = (Cylinder*)s;
		this->center1.set(*Ccyl->getCenter1());
		this->center2.set(*Ccyl->getCenter2());
		this->Radius1(Ccyl->Radius1());
		this->Radius2(Ccyl->Radius2());
		this->dir_l(Ccyl->dir_l());
		this->dir_m(Ccyl->dir_m());
		this->dir_n(Ccyl->dir_n());
		this->phi_max(Ccyl->phi_max());
		this->phi_min(Ccyl->phi_min());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0019", __FILE__, __FUNCSIG__); }
}

void Cylinder::Transform(double* transform)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply(transform, center1.getX(), center1.getY(), center1.getZ());
		center1.set(temp.getX(), temp.getY(),  temp.getZ());
		temp = MAINDllOBJECT->TransformMultiply(transform, center2.getX(), center2.getY(), center2.getZ());
		center2.set(temp.getX(), temp.getY(),  temp.getZ());
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0020", __FILE__, __FUNCSIG__); }
}

void Cylinder::Translate(Vector& Position)
{
	center1 += Position;
	center2 += Position;
	this->UpdateEnclosedRectangle();
}

void Cylinder::AlignToBasePlane(double* trnasformM)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, center1.getX(), center1.getY(), center1.getZ());
		center1.set(temp.getX(), temp.getY(), temp.getZ());
		temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, center2.getX(), center2.getY(), center2.getZ());
		center2.set(temp.getX(), temp.getY(), temp.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0020", __FILE__, __FUNCSIG__); }
}

void Cylinder::UpdateEnclosedRectangle()
{
	try
	{
		double cen1[3] = {0}, cen2[3] = {0}, PointsColl[8] = {0};
		center1.FillDoublePointer(&cen1[0], 3); center2.FillDoublePointer(&cen2[0], 3);
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0}, temp_cen1[3] = {cen1[0], cen1[1], cen1[2]}, temp_cen2[3] = {cen2[0], cen2[1], cen2[2]};
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_cen1, Order2, cen1);
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_cen2, Order2, cen2);		
		}
		RMATH2DOBJECT->RectangleEnclosing_Circle(&cen1[0], Radius1(), &PointsColl[0], &PointsColl[2]);	
		RMATH2DOBJECT->RectangleEnclosing_Circle(&cen2[0], Radius2(), &PointsColl[4], &PointsColl[6]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 4, 2, &ShapeLeftTop[0], &ShapeRightBottom[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0020a", __FILE__, __FUNCSIG__); }
}

void Cylinder::GetShapeCenter(double *cPoint)
{
	cPoint[0] = (center1.getX() + center2.getX()) / 2;
	cPoint[1] = (center1.getY() + center2.getY()) / 2;
	cPoint[2] = (center1.getZ() + center2.getZ()) / 2;
}


bool Cylinder::WithinCirclularView(double *SelectionLine, double Tolerance)
{
	double ThisCylinder[7] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1()};
	double dist = RMATH3DOBJECT->Distance_Line_Cylinder(SelectionLine, ThisCylinder, 1);
	if(dist < Tolerance) return true;
	return false;
}

double Cylinder::GetZlevel()
{
	return (center1.getZ() + center2.getZ())/2;
}

int Cylinder::shapenumber = 0;
void Cylinder::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, Cylinder& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "Cylinder" << endl;
		os << "Center1:values" << endl << (*static_cast<Vector*>(x.getCenter1())) << endl;
		os << "Center2:values" << endl << (*static_cast<Vector*>(x.getCenter2())) << endl;
		os << "Radius1:" << x.Radius1() << endl;
		os << "Radius2:" << x.Radius2() << endl;
		os << "dir_l:" << x.dir_l() << endl;
		os << "dir_m:" << x.dir_m() << endl;
		os << "dir_n:" << x.dir_n() << endl;
		os << "EndCylinder" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0021", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Cylinder& x)
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
			if(Tagname==L"Cylinder")
			{
			while(Tagname!=L"EndCylinder")
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
						else if(Tagname==L"Center1")
						{
							is >> (*static_cast<Vector*>(x.getCenter1()));
						}
						else if(Tagname==L"Center2")
						{
							is >> (*static_cast<Vector*>(x.getCenter2()));
						}
						else if(Tagname==L"Radius1")
						{
							 x.Radius1(atof((const char*)(Val).c_str()));
						}	
						else if(Tagname==L"Radius2")
						{
							 x.Radius2(atof((const char*)(Val).c_str()));
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
					}
				}
				x.ShapeType = RapidEnums::SHAPETYPE::CYLINDER;
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CY0022", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, Cylinder& x)
{
	try
	{
		double d;
		x.ShapeType = RapidEnums::SHAPETYPE::CYLINDER;
		is >> (*static_cast<Vector*>(x.getCenter1()));
		is >> (*static_cast<Vector*>(x.getCenter2()));
		is >> d; x.Radius1(d);
		is >> d; x.Radius2(d);
		is >> d; x.dir_l(d);
		is >> d; x.dir_m(d);
		is >> d; x.dir_n(d);
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CY0022", __FILE__, __FUNCSIG__); }
}