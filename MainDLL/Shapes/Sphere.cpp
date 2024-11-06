#include "StdAfx.h"
#include "Sphere.h"
#include "Plane.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

Sphere::Sphere( TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

Sphere::Sphere(bool simply):ShapeWithList(false)
{
	init();
}

Sphere::~Sphere()
{

}

void Sphere::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::SPHERE;
		this->Radius(0); this->MinInscribedia(0); this->MaxCircumdia(0);
		this->theta_max(M_PI); this->theta_min(0); this->phi_max(2*M_PI); this->phi_min(0);
		this->Sphericity(0); this->ShapeAs3DShape(true);
		this->IdSphere(false);
		this->UseLightingProperties(true);
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->center), 3), 3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0001", __FILE__, __FUNCSIG__); }
}

TCHAR* Sphere::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		shapenumber++;
		TCHAR shapenumstr[10];
		_itot_s(shapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0002", __FILE__, __FUNCSIG__); return name; }
}

bool Sphere::Shape_IsInWindow(Vector& corner1,double Tolerance)
{
	try
	{
		double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
		return Shape_IsInWindow(&SelectionL[0], Tolerance);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0003", __FILE__, __FUNCSIG__); }
}

bool Sphere::Shape_IsInWindow(Vector& corner1,Vector& corner2 )
{
	return false;
}

bool Sphere::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		double  CenterPt[3] = {center.getX(), center.getY(), center.getZ()}, sphereParam[4] = {CenterPt[0], CenterPt[1], CenterPt[2], Radius()};
		double dist = RMATH3DOBJECT->Distance_Point_Line(CenterPt, SelectionLine);
		double dist1 = SNAPDIST * MAINDllOBJECT->getWindow(1).getUppX();		
		Intersects= RMATH3DOBJECT->Intersection_Line_Sphere(SelectionLine, sphereParam, IntersectnPts);
		//MAINDllOBJECT->AllowTransparencyTo3DShapes()
		//check for distance of selection line from center of sphere
		//if distance is less than snapdist, then return false.
		//else
		if(dist <= Radius())
		{	
			if(dist <= Radius() / 4)
			{
				if(MAINDllOBJECT->AllowTransparencyTo3DShapes())
				{
					MAINDllOBJECT->UpdateGrafixFor3DSnapPoint(true);
					return false;
				}
			}
			return true;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0004", __FILE__, __FUNCSIG__); return false;}
}

void Sphere::Translate(Vector& Position)
{
	center += Position;
	this->UpdateEnclosedRectangle();
}

void Sphere::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		int totcount = this->PointsList->Pointscount();
		bool allowTransparency = MAINDllOBJECT->AllowTransparencyTo3DShapes();
		if(HighlightedFormouse()) allowTransparency = false;

		if(PartialShape())
		{
			//use a settings option to toggle between the three options below.
			if(PartialShapeDrawMode() == 0)
				GRAFIX->draw_SpherePartial(center.getX(), center.getY(), center.getZ(), Radius(), theta_min(), theta_max(), phi_min(), phi_max(), MAINDllOBJECT->DrawWiredSurface(), allowTransparency, WPixelWidth);
			else if(PartialShapeDrawMode() == 1)
				GRAFIX->draw_SphereFromPts(center.getX(), center.getY(), center.getZ(), Radius(), pts, totcount, MAINDllOBJECT->DrawWiredSurface(), allowTransparency, WPixelWidth);
			else
			{
				GRAFIX->draw_Sphere(center.getX(), center.getY(), center.getZ(), Radius(), MAINDllOBJECT->DrawWiredSurface(), allowTransparency, WPixelWidth);
				GRAFIX->drawPoint(center.getX(), center.getY(), center.getZ(),true);
			}
		}
		else
		{
			GRAFIX->draw_Sphere(center.getX(), center.getY(), center.getZ(), Radius(), MAINDllOBJECT->DrawWiredSurface(), allowTransparency, WPixelWidth);
			GRAFIX->drawPoint(center.getX(), center.getY(), center.getZ(),5);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0005", __FILE__, __FUNCSIG__); }
}

void Sphere::drawGDntRefernce()
{
	try
	{
		if(RefernceDatumForgdnt() > 0)
		{
			double SphParam[4];
			getParameters(&SphParam[0]);
			GRAFIX->drawReferenceDatumName_Sphere(&SphParam[0], &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0006", __FILE__, __FUNCSIG__); }
}

void Sphere::ResetShapeParameters()
{ 
	try
	{
		this->Radius(0); this->MinInscribedia(0); this->MaxCircumdia(0);
		this->theta_max(M_PI); this->theta_min(0); this->phi_max(2*M_PI); this->phi_min(0);
		this->Sphericity(0); this->ShapeAs3DShape(true);
		this->IdSphere(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0007", __FILE__, __FUNCSIG__); }
}

void Sphere::UpdateBestFit()
{
	try
	{
		this->IsValid(false);
		int totcount = this->PointsListOriginal->Pointscount();
		if(totcount < 4) return;
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
		if(!updateSphere(totcount))
		{
			if(Radius() > 0)this->IsValid(true);
			else this->IsValid(false);
			return;
		}
		if(MAINDllOBJECT->MasteProbeCalibrationMode())
			ProbePointCalculations_Calibration(totcount);
		else
			ProbePointCalculations(totcount);
		calculateAttributes();
		IsValid(true);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0008", __FILE__, __FUNCSIG__); }
}

bool Sphere::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		 int s1[2] = {4, 4}, s2[2] = {4, 1};
		 double cent[4] = {center.getX(), center.getY(), center.getZ(), 1}, new_cen[4] = {0};
		RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &cent[0], &s2[0], &new_cen[0]);
		RMATH2DOBJECT->RectangleEnclosing_Circle(&new_cen[0], Radius(), &Lefttop[0], &Rightbottom[0]);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0008", __FILE__, __FUNCSIG__); return false;}
}

void Sphere::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt >= 4) // && PointsCnt < 10)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdSphere();
		}
		if(ShapeDoneUsingProbe())
		{
			updateSphere_UsingPorbe();
			if(!updateSphere(PointsCnt))
			{
				if(Radius() > 0)this->IsValid(true);
				else this->IsValid(false);
				return;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0009", __FILE__, __FUNCSIG__); }
}

void Sphere::ProbePointCalculations_Calibration(int PointsCnt)
{
	try
	{
		if(PointsCnt == 4)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdSphere();
		}
		if(ShapeDoneUsingProbe())
		{
			updateSphere_UsingPorbe_Calibration();
			if(!updateSphere(PointsCnt * 2))
			{
				if(Radius() > 0)this->IsValid(true);
				else this->IsValid(false);
				return;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0009", __FILE__, __FUNCSIG__); }
}

bool Sphere::updateSphere(int PointsCnt)
{
	try
	{
		double ans[4] = {0}; bool flag = false;
		ans[0] = this->center.getX(); ans[1] = this->center.getY(); ans[2] = this->center.getZ(); ans[3] = this->Radius(); 
		if(PointsCnt == 4)
			flag = BESTFITOBJECT->Sphere_4Pt(pts, &ans[0]);
		else
		{
			if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
			{
				flag = BESTFITOBJECT->Sphere_BestFit(pts, PointsCnt, &ans[0], true);
			}
			else
			{
				flag = BESTFITOBJECT->Sphere_BestFit(pts, PointsCnt, &ans[0], SigmaMode());
			}
		}
		if(!flag) return false;
		if(ApplyFilterCorrection() && PointsCnt > 4)
		{
			int cnt = PointsCnt;
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
			{
				flag = false;
				cnt = HELPEROBJECT->FilteredSphere(PointsList, ans, FilterCorrectionFactor(), i);
				if(cnt < 4) return false;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * cnt * 3);
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
				if(cnt == 4)
				{
					flag = BESTFITOBJECT->Sphere_4Pt(filterpts, &ans[0]);
				}
				else if(cnt > 4)
				{
					if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					{
						flag = BESTFITOBJECT->Sphere_BestFit(filterpts, filterptsCnt, &ans[0], true);
					}
					else
					{
						flag = BESTFITOBJECT->Sphere_BestFit(filterpts, filterptsCnt, &ans[0], SigmaMode());
					}
				}
				if(!flag)
				{
					return false;
				}
			}
		}
		if(ApplyMMCCorrection() && ScanDirDetected() && PointsCnt > 4)
		{
			int cnt = PointsCnt;
			for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
			{
				flag = false;
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
				cnt = HELPEROBJECT->OutermostInnerMostSphere(PointsList, ans, useValidPointOnly, OuterCorrection());
				if(cnt < 4) return false;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * cnt * 3);
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
				if(cnt == 4)
					flag = BESTFITOBJECT->Sphere_4Pt(filterpts, &ans[0]);
				else if(cnt > 4)
				{
					if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
					{
						flag = BESTFITOBJECT->Sphere_BestFit(filterpts, filterptsCnt, &ans[0], true);
					}
					else
					{
						flag = BESTFITOBJECT->Sphere_BestFit(filterpts, filterptsCnt, &ans[0], SigmaMode());
					}
				}
				if(!flag)
				{
					return false;
				}
			}
		}		
		center.set(ans[0], ans[1], ans[2]);
		Radius(ans[3]);

		double boundaryangles[4] = {0};
		if(PartialShape())
		{
			if (!BESTFITOBJECT->getSphereBoundaryAngles(pts, PointsCnt, ans, boundaryangles))
				return false;
			theta_min(boundaryangles[0]);
			theta_max(boundaryangles[1]);
			phi_min(boundaryangles[2]);
			phi_max(boundaryangles[3]);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0009", __FILE__, __FUNCSIG__); return false; }
}

void Sphere::CheckPointIsForPorbe()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0010", __FILE__, __FUNCSIG__); }
}

void Sphere::CheckIdorOdSphere()
{
	try
	{
		int IDCount = 0;
		PointCollection ptCol;
		Vector CenterMcs;
		HELPEROBJECT->GetMcsPoint(&center, &CenterMcs);
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
					IDCount--;
				else
					IDCount++;
			}
		}
		ptCol.deleteAll();
		if(IDCount > 0)
			IdSphere(true);
		else
			IdSphere(false);
		if(this->ApplyMMCCorrection() && (!this->ScanDirDetected()))
		{
			this->OuterCorrection(!this->IdSphere());
			this->ScanDirDetected(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0011", __FILE__, __FUNCSIG__); }
}

void Sphere::updateSphere_UsingPorbe()
{
	try
	{
		double Pt[3];;
		double Pradius = 0;
		double SphereParam[4] = {center.getX(), center.getY(), center.getZ(), Radius()};
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};
			Pradius = getProbeRadius(Spt->PLR);
			RMATH3DOBJECT->Sphere_SurfacePt_for_Probe(&SphereParam[0], &PrPoint[0], Pradius, IdSphere(), &Pt[0]);
			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(Pt[0], Pt[1], Pt[2], Spt->R, Spt->Pdir);
		}

		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		filterptsCnt  = 0;
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0012", __FILE__, __FUNCSIG__); }
}

void Sphere::updateSphere_UsingPorbe_Calibration()
{
	try
	{
		/*this->PointsList->deleteAll();
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};
			double MPrPoint[3] = {Spt->X, Spt->Y, Spt->Z - 2 * (Spt->Z - center.getZ())};
			this->PointsList->Addpoint(new SinglePoint(PrPoint[0], PrPoint[1], PrPoint[2]));
			this->PointsList->Addpoint(new SinglePoint(MPrPoint[0], MPrPoint[1], MPrPoint[2]));
		}
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsList->Pointscount() * 3);
		int n = 0;
		for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			this->pts[n++] = Pt->X;
			this->pts[n++] = Pt->Y;
			this->pts[n++] = Pt->Z;
		}*/


		double Pt[3];;
		//double Pradius = MAINDllOBJECT->ProbeRadius();
		double MasterPradius = MAINDllOBJECT->MasterProbeRadius();
		double PlaneShift = 2 * MasterPradius - Radius();
		double SphereParam[4] = {center.getX(), center.getY(), center.getZ(), Radius()};
		Plane* PlaneReference = (Plane*)(*this->getParents().begin());
		double PlaneParam[4];
		PlaneReference->getParameters(&PlaneParam[0]);
		
		if(PlaneParam[2] > 0)
			PlaneParam[3] += PlaneShift;
		else
			PlaneParam[3] -= PlaneShift;
		this->PointsList->deleteAll();
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z}, MPrPoint[3];
			this->PointsList->Addpoint(new SinglePoint(PrPoint[0], PrPoint[1], PrPoint[2]));
			RMATH3DOBJECT->Mirror_Pt_wrt_Plane(&PrPoint[0], &PlaneParam[0], &MPrPoint[0]);
			this->PointsList->Addpoint(new SinglePoint(MPrPoint[0], MPrPoint[1], MPrPoint[2]));
		}
		if(this->pts != NULL){free(pts); pts = NULL;}
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsList->Pointscount() * 3);
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsList->Pointscount() * 3);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0012", __FILE__, __FUNCSIG__); }
}

void Sphere::calculateAttributes()
{
	try
	{
		int PointsCount = this->PointsList->Pointscount();
		double Npt[3] = {0};
		double SphereParam[7] = {center.getX(), center.getY(), center.getZ(), Radius()};
		double dist1 = RMATH3DOBJECT->Nearest_Point_to_Sphere(pts, PointsCount, &SphereParam[0], &Npt[0]);
		this->MinInscribedia(dist1 + Radius());
		double dist2 = RMATH3DOBJECT->Farthest_Point_to_Sphere(pts, PointsCount, &SphereParam[0], &Npt[0]);
		this->MaxCircumdia(dist2 + Radius());
		this->Sphericity(dist2 - dist1);
		//get min Z point..
		MinZpoint.set(pts[0], pts[1], pts[2]);
		for(int i = 1; i < PointsCount; i++)
		{
			if(MinZpoint.getZ() > pts[3 * i + 2])
				MinZpoint.set(pts[3 * i], pts[3 * i + 1], pts[3 * i + 2]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0013", __FILE__, __FUNCSIG__); }
}

void Sphere::UpdateForParentChange(BaseItem* sender)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0014", __FILE__, __FUNCSIG__); }
}

void Sphere::UpdateEnclosedRectangle()
{
	try
	{
		double cen[3] = {0};
		center.FillDoublePointer(&cen[0], 3);
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{		 
			 double temp_cen[3] = {cen[0], cen[1], cen[2]}, TransMatrix[9] = {0};
			 int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			 RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			 RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_cen, Order2, cen);		
		}
		RMATH2DOBJECT->RectangleEnclosing_Circle(&cen[0], Radius(), &ShapeLeftTop[0], &ShapeRightBottom[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0026", __FILE__, __FUNCSIG__); }
}

void Sphere::GetShapeCenter(double *cPoint)
{
	cPoint[0] = center.getX();
	cPoint[1] = center.getY();
	cPoint[2] = center.getZ();
}

void Sphere::Transform(double* transform)
{
	center.set(MAINDllOBJECT->TransformMultiply(transform, center.getX(), center.getY(), center.getZ()));
	this->UpdateEnclosedRectangle();
}

void Sphere::AlignToBasePlane(double* trnasformM)
{
	center.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, center.getX(), center.getY(), center.getZ()));
}

void Sphere::setCenter(Vector d)
{
	center.set(d);
}

Vector* Sphere::getCenter()
{
	return(&this->center);
}

void Sphere::getParameters(double* sphereparam)
{
	 center.FillDoublePointer(sphereparam, 3);
	 sphereparam[3] = Radius();
}

void Sphere::getSurfacePoint(double *dir, bool forward, double *SFPoint)
{
	try
	{
		int dirFactr = -1;
		if(forward) dirFactr = 1;
		SFPoint[0] = center.getX() + dirFactr * dir[0] * Radius();
		SFPoint[1] = center.getY() + dirFactr * dir[1] * Radius();
		SFPoint[2] = center.getZ() + dirFactr * dir[2] * Radius();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0015", __FILE__, __FUNCSIG__); }
}

Shape* Sphere::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("SH");
		else
			myname = _T("dSH");
		Sphere* CShape = new Sphere((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
			CShape->CopyOriginalProperties(this);
		return CShape;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0016", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Sphere::CreateDummyCopy()
{
	try
	{
		Sphere* CShape = new Sphere(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0017", __FILE__, __FUNCSIG__); return NULL; }
}

void Sphere::CopyShapeParameters(Shape* s)
{
	try
	{
		Sphere* Csph = (Sphere*)s;
		this->center.set(*Csph->getCenter());
		this->Radius(Csph->Radius());
		this->theta_max(Csph->theta_max());
		this->theta_min(Csph->theta_min());
		this->phi_max(Csph->phi_max());
		this->phi_min(Csph->phi_min());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0018", __FILE__, __FUNCSIG__); }
}


bool Sphere::WithinCirclularView(double *SelectionLine, double Tolerance)
{
	double CenterPt[3] = {center.getX(), center.getY(), center.getZ()};
	double dist = RMATH3DOBJECT->Distance_Point_Line(CenterPt, SelectionLine);
	double dist1 = dist - Radius();
	if(dist1 < Tolerance) return true;
	return false;
}


double Sphere::GetZlevel()
{
	return center.getZ();
}


int Sphere::shapenumber = 0;
void Sphere::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, Sphere& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "Sphere" << endl;
		os << "Center:value" << endl << (*static_cast<Vector*>(x.getCenter())) << endl;
		os << "Radius:" << x.Radius() << endl;
		os << "EndSphere" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0019", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, Sphere& x )
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
			if(Tagname==L"Sphere")
			{
			while(Tagname!=L"EndSphere")
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
						if(Tagname==L"Center")
						{
							is >> (*static_cast<Vector*>(x.getCenter()));		
						}
						else if(Tagname==L"Radius")
						{
							 x.Radius(atof((const char*)(Val).c_str()));		
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0020", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, Sphere& x)
{
	try
	{
		double d;
		is >> (*static_cast<Vector*>(x.getCenter()));
		is >> d; x.Radius(d);
	}
	catch(...){  MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0020", __FILE__, __FUNCSIG__);}
}
