#include "stdafx.h"
#include "Cone.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

Cone::Cone(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

Cone::Cone(bool simply):ShapeWithList(false)
{
	init();
}

Cone::~Cone()
{
}

void Cone::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::CONE;
		this->Radius1(0); this->Radius2(0); this->ApexAngle(0);
		this->dir_l(0); this->dir_m(0); this->dir_n(1);
		this->MinInscribeApexAng(0); this->MaxCircumApexAng(0); this->Conicality(0); this->Length(0);
		this->phi_max(2*M_PI); this->phi_min(0);
		this->ShapeAs3DShape(true); this->IdCone(true);
		this->UseLightingProperties(true);
		for (int i = 0; i < 8; i++)
			this->prevguess[i] = 0;
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center1), 0), 0);
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center2), 1), 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0001", __FILE__, __FUNCSIG__); }
}

TCHAR* Cone::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0002", __FILE__, __FUNCSIG__); return name; }
}

void Cone::UpdateBestFit()
{
	try
	{
		this->IsValid(false);
		int totcount = this->PointsList->Pointscount();
		if (totcount < 6) return;
		// update pts array with original points ............
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
		updateCone(totcount);
		ProbePointCalculations(totcount);
		calculateAttributes();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0003", __FILE__, __FUNCSIG__); }
}

 bool Cone::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0003", __FILE__, __FUNCSIG__); return false;}
}

void Cone::updateCone(int PointsCount)
{
	try
	{
		double ans[8] = {0}, endpts[6] = {0}, minr, maxr;
		if (prevguess[6] != 0)
		{
			for (int i = 0; i < 8; i++)
				ans[i] = prevguess[i];
		}
		bool flag = BESTFITOBJECT->Cone_BestFit(pts, PointsCount, &ans[0]);
		if (flag)
		{
			for (int i = 0; i < 8; i++)
				prevguess[i] = ans[i];
		}
		HELPEROBJECT->CalculateEndPoints_3DLineBestfit(PointsCount, pts, &ans[0], &endpts[0], &endpts[3], &minr, &maxr);
		if(ApplyFilterCorrection() && PointsCount > 6)
		{
		    int cnt = PointsCount;
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
		    {
				double coneparam[8] = {ans[0], ans[1], ans[2], ans[3], ans[4], ans[5],(ans[6] + minr * tan (ans[7])), (ans[6] + maxr * tan (ans[7]))};
				flag = false;
			    cnt = HELPEROBJECT->FilteredCone(PointsList, coneparam, endpts, FilterCorrectionFactor(), i);
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
				flag = BESTFITOBJECT->Cone_BestFit(filterpts, filterptsCnt, &ans[0]);	
				HELPEROBJECT->CalculateEndPoints_3DLineBestfit(filterptsCnt, filterpts, &ans[0], &endpts[0], &endpts[3], &minr, &maxr);
			}
		}
		if(ApplyMMCCorrection() && ScanDirDetected() && PointsCount > 6)
		{
			int cnt = PointsCount;
    	    for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
		    {
				double coneparam[8] = {ans[0], ans[1], ans[2], ans[3], ans[4], ans[5],(ans[6] + minr * tan (ans[7])), (ans[6] + maxr * tan (ans[7]))};
				flag = false;
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
			    cnt = HELPEROBJECT->OutermostInnerMostCone(PointsList, coneparam, endpts, useValidPointOnly, OuterCorrection());
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
				flag = BESTFITOBJECT->Cone_BestFit(filterpts, filterptsCnt, &ans[0]);	
				HELPEROBJECT->CalculateEndPoints_3DLineBestfit(filterptsCnt, filterpts, &ans[0], &endpts[0], &endpts[3], &minr, &maxr);
		    }
    	}
	
		//if(!flag) return false;
		/*double nval = sqrt(pow(ans[3], 2) + pow(ans[4], 2) + pow(ans[5], 2));
		SetConeProperties(ans[3]/nval, ans[4]/nval, ans[5]/nval);*/
		center1.set(endpts[0], endpts[1], endpts[2]);
		center2.set(endpts[3], endpts[4], endpts[5]);
		Radius1(ans[6] + minr * tan (ans[7])); Radius2(ans[6] + maxr * tan (ans[7]));
		ApexAngle(ans[7]);
		
		double boundaryangles[2] = {0};
		if(PartialShape())
		{
			BESTFITOBJECT->getConeBoundaryAngles(pts, PointsCount, ans, boundaryangles);
			phi_min(boundaryangles[0]);
			phi_max(boundaryangles[1]);
		}
		this->dir_l(ans[3]);
		this->dir_m(ans[4]);
		this->dir_n(ans[5]);
		updateParameters();

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0004", __FILE__, __FUNCSIG__); }
}

void Cone::updateParameters()
{
	try
	{
		this->Length(sqrt(pow(center2.getX() - center1.getX(), 2) + pow(center2.getY() - center1.getY(), 2) + pow(center2.getZ() - center1.getZ(), 2)));
		/*this->dir_l((center2.getX() - center1.getX()) / Length());
		this->dir_m((center2.getY() - center1.getY()) / Length());
		this->dir_n((center2.getZ() - center1.getZ()) / Length());*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0004", __FILE__, __FUNCSIG__); }
}

void Cone::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt >= 6) // && PointsCnt < 10)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForProbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdCone();
		}
		if(ShapeDoneUsingProbe())
		{
			updateCone_UsingProbe();
			updateCone(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0005", __FILE__, __FUNCSIG__); }
}

void Cone::CheckPointIsForProbe()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0006", __FILE__, __FUNCSIG__); }
}

void Cone::CheckIdorOdCone()
{
	try
	{
		int IDCount = 0;
        PointCollection ptCol;
		HELPEROBJECT->GetMcsPointsList(this->PointsListOriginal, &ptCol);
		Vector CenterMcs;
		HELPEROBJECT->GetMcsPoint(&center1, &CenterMcs);
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
			this->IdCone(true);
		else
			this->IdCone(false);
		if(this->ApplyMMCCorrection() && (!this->ScanDirDetected()))
		{
			this->OuterCorrection(!this->IdCone());
			this->ScanDirDetected(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0007", __FILE__, __FUNCSIG__); }
}

void Cone::updateCone_UsingProbe()
{
	try
	{
		double Pt[3];;
		double Pradius = 0;
		double ConeParam[8] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1(),ApexAngle()};
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};
			Pradius = getProbeRadius(Spt->PLR);
			RMATH3DOBJECT->Cone_SurfacePt_for_Probe(&ConeParam[0], &PrPoint[0], Pradius, IdCone(), &Pt[0]);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0008", __FILE__, __FUNCSIG__); }
}

void Cone::UpdateForParentChange(BaseItem* sender)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0009", __FILE__, __FUNCSIG__); }
}

void Cone::calculateAttributes()
{
	try
	{
		int PointsCount = this->PointsList->Pointscount();
		double Npt[3];
		double ConeParam[8] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), ApexAngle()};
		double ang1 = RMATH3DOBJECT->Smallest_Angle_of_Cone(pts, PointsCount, &ConeParam[0], &Npt[0]);
		this->MinInscribeApexAng(ang1);
		double ang2 = RMATH3DOBJECT->Largest_Angle_of_Cone(pts, PointsCount, &ConeParam[0], &Npt[0]);
		this->MaxCircumApexAng(ang2);
		this->Conicality(ang2 - ang1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0010", __FILE__, __FUNCSIG__); }
}

void Cone::drawCurrentShape(int windowno, double WPixelWidth)
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

void Cone::drawGDntRefernce()
{
	try
	{
		if(RefernceDatumForgdnt() > 0)
		{
			double CnParam[8], CnEndpts[6];
			getParameters(&CnParam[0]);
			getEndPoints(&CnEndpts[0]);
			GRAFIX->drawReferenceDatumName_Cone(&CnParam[0], &CnEndpts[0], &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0012", __FILE__, __FUNCSIG__); }
}

void Cone::ResetShapeParameters()
{
	try
	{
		this->Radius1(0); this->Radius2(0); this->ApexAngle(0);
		this->dir_l(0); this->dir_m(0); this->dir_n(1);
		this->MinInscribeApexAng(0); this->MaxCircumApexAng(0); this->Conicality(0); this->Length(0);
		this->phi_max(2*M_PI); this->phi_min(0);
		this->ShapeAs3DShape(true); this->IdCone(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0013", __FILE__, __FUNCSIG__); }
}

bool Cone::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
		return Shape_IsInWindow(&SelectionL[0], Tolerance);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0014", __FILE__, __FUNCSIG__); return false; }
}

bool Cone::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool Cone::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		double ThisCone[8] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), ApexAngle()};
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
		Intersects = RMATH3DOBJECT->Intersection_Line_Cone(SelectionLine, ThisCone, IntersectnPts);
		if(Intersects == 1 || Intersects == 2)
		{
			RMATH3DOBJECT->Projection_Point_on_Line(&IntersectnPts[0], ThisCone, ProjctnPt);
			if(RMATH3DOBJECT->Checking_Point_within_Block(ProjctnPt, ThePoints, 2))
				return true;
			if(Intersects == 2)
			{
				RMATH3DOBJECT->Projection_Point_on_Line(&IntersectnPts[3], ThisCone, ProjctnPt);
				if(RMATH3DOBJECT->Checking_Point_within_Block(ProjctnPt, ThePoints, 2))
					return true;
			}
		}

		double PlaneOfArc1[4] = {dir_l(), dir_m(), dir_n(), dir_l() * center1.getX() +  dir_m() * center1.getY() + dir_n() * center1.getZ()};
		double PlaneOfArc2[4] = {dir_l(), dir_m(), dir_n(), dir_l() * center2.getX() +  dir_m() * center2.getY() + dir_n() * center2.getZ()};
		Intersects = RMATH3DOBJECT->Intersection_Line_Plane(SelectionLine, PlaneOfArc1, IntersectnPts);
		if(Intersects == 1)
		{
			double dist = RMATH3DOBJECT->Distance_Point_Point(ThePoints, IntersectnPts);
			if(dist < Radius1())
				return true;
		}
		Intersects = RMATH3DOBJECT->Intersection_Line_Plane(SelectionLine, PlaneOfArc2, IntersectnPts);
		if(Intersects == 1)
		{
			double dist = RMATH3DOBJECT->Distance_Point_Point(&ThePoints[3], IntersectnPts);
			if(dist < Radius2())
				return true;
		}	
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0015", __FILE__, __FUNCSIG__); return false; }
}

void Cone::SetCenter1(Vector& t)
{
	this->center1.set(t);
}

void Cone::SetCenter2(Vector& t)
{
	this->center2.set(t);
}

void Cone::SetCenters(Vector& c1, Vector& c2)
{
	this->center1.set(c1); this->center1.set(c2);
}

void Cone::SetRadius(double r1,double r2)
{
	Radius1(r1); Radius2(r2);
}

void Cone::SetConeProperties(double l, double m, double n)
{
	dir_l(l); dir_m(m); dir_n(n);
}

Vector* Cone::getCenter1()
{
	return (&this->center1);
}

Vector* Cone::getCenter2()
{
	return (&this->center2);
}

void Cone::getParameters(double* Coneparam)
{
	center1.FillDoublePointer(Coneparam, 3);
	Coneparam[3] = dir_l(); Coneparam[4] = dir_m(); Coneparam[5] = dir_n();
	Coneparam[6] = Radius1();
	Coneparam[7] = Radius2();
}

void Cone::getAxisLine(double *LineParam3D)
{
	center1.FillDoublePointer(LineParam3D, 3);
	LineParam3D[3] = dir_l(); LineParam3D[4] = dir_m(); LineParam3D[5] = dir_n();
}

void Cone::getEndPoints(double* endpoints)
{
	center2.FillDoublePointer(endpoints, 3);
	center1.FillDoublePointer(endpoints + 3, 3);
}

void Cone::getSurfaceLine(double *dir, bool forward, double *SFline, double *SFlineEndpts)
{
	try
	{
		int dirFactr = -1;
		if(forward) dirFactr = 1;
		double ratios[3];
		double cen1[3] = {center1.getX(), center1.getY(), center1.getZ()}, cen2[3] = {center2.getX(), center2.getY(), center2.getZ()};
		for(int i = 0; i < 3; i++)
		{
			SFline[i] = cen1[i] + dirFactr * dir[i] * Radius1();
			SFlineEndpts[i] = SFline[i];
			SFlineEndpts[i + 3] = cen2[i] + dirFactr * dir[i] * Radius2();
			ratios[i] = SFlineEndpts[i+3] - SFlineEndpts[i];
		}
		double norm = sqrt(pow(ratios[0],2) + pow(ratios[1],2) + pow(ratios[2],2));
		SFline[3] = ratios[0]/norm; SFline[4] = ratios[1]/norm; SFline[5] = ratios[2]/norm; 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0016", __FILE__, __FUNCSIG__); }
}

Shape* Cone::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("CL");
		else
			myname = _T("dCL");
		Cone* CShape = new Cone((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		  CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0017", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Cone::CreateDummyCopy()
{
	try
	{
		Cone* CShape = new Cone(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0018", __FILE__, __FUNCSIG__); return NULL; }
}

void Cone::CopyShapeParameters(Shape* s)
{
	try
	{
		Cone* Ccone = (Cone*)s;
		this->center1.set(*Ccone->getCenter1());
		this->center2.set(*Ccone->getCenter2());
		this->Radius1(Ccone->Radius1());
		this->Radius2(Ccone->Radius2());
		this->dir_l(Ccone->dir_l());
		this->dir_m(Ccone->dir_m());
		this->dir_n(Ccone->dir_n());
		this->ApexAngle(Ccone->ApexAngle());
		this->phi_max(Ccone->phi_max());
		this->phi_min(Ccone->phi_min());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0019", __FILE__, __FUNCSIG__); }
}

void Cone::Transform(double* transform)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply(transform,center1.getX(), center1.getY(), center1.getZ());
		center1.set(temp.getX(), temp.getY(), temp.getZ());
		temp = MAINDllOBJECT->TransformMultiply(transform,center2.getX(), center2.getY(), center2.getZ());
		center2.set(temp.getX(), temp.getY(), temp.getZ());
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0020", __FILE__, __FUNCSIG__); }
}

void Cone::Translate(Vector& Position)
{
	center1 += Position;
	center2 += Position;
	this->UpdateEnclosedRectangle();
}

void Cone::AlignToBasePlane(double* trnasformM)
{
}

void Cone::UpdateEnclosedRectangle()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0020a", __FILE__, __FUNCSIG__); }
}

void Cone::GetShapeCenter(double *cPoint)
{
	cPoint[0] = (center1.getX() + center2.getX()) / 2;
	cPoint[1] = (center1.getY() + center2.getY()) / 2;
	cPoint[2] = (center1.getZ() + center2.getZ()) / 2;
}

bool Cone::WithinCirclularView(double *SelectionLine, double Tolerance)
{
	double ThisCone[8] = {center1.getX(), center1.getY(), center1.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), ApexAngle()};
	double dist = RMATH3DOBJECT->Distance_Line_Cone(SelectionLine, ThisCone);
	if(dist < Tolerance) return true;
	return false;
}

double Cone::GetZlevel()
{
	return (center1.getZ() + center2.getZ())/2;
}

int Cone::shapenumber = 0;
void Cone::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, Cone& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "Cone" << endl;
		os << "Center1:values" << endl << (*static_cast<Vector*>(x.getCenter1())) << endl;
		os << "Center2:values" << endl << (*static_cast<Vector*>(x.getCenter2())) << endl;
		os << "Radius1:" << x.Radius1() << endl;
		os << "Radius2:" << x.Radius2() << endl;
		os << "dir_l:" << x.dir_l() << endl;
		os << "dir_m:" << x.dir_m() << endl;
		os << "dir_n:" << x.dir_n() << endl;
		os << "ApexAngle:" << x.ApexAngle() << endl;
		for (int i = 0; i < 8; i++)
		{
			os << "prevguess:" << x.prevguess[i] << endl;
		}
		os << "EndCone" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CN0021", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Cone& x)
{
	try
	{
		int countprevguess=0;
		is >> (*(Shape*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"Cone")
			{
			while(Tagname!=L"EndCone")
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
						else if(Tagname==L"ApexAngle")
						{
							 x.ApexAngle(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"prevguess")
						{
							x.prevguess[countprevguess] = atof((const char*)(Val).c_str());
							countprevguess++;
						}
					}
				}
				x.ShapeType = RapidEnums::SHAPETYPE::CONE;
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CN0022", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, Cone& x)
{
	try
	{
		double d;
		x.ShapeType = RapidEnums::SHAPETYPE::CONE;
		is >> (*static_cast<Vector*>(x.getCenter1()));
		is >> (*static_cast<Vector*>(x.getCenter2()));
		is >> d; x.Radius1(d);
		is >> d; x.Radius2(d);
		is >> d; x.dir_l(d);
		is >> d; x.dir_m(d);
		is >> d; x.dir_n(d);
		is >> d; x.ApexAngle(d);
		for (int i = 0; i < 8; i++)
		{
			is >> d; x.prevguess[i] = d;
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CN0022", __FILE__, __FUNCSIG__); }
}
