#include "StdAfx.h"
#include "Plane.h"
#include "Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PointCollection.h"
#include "..\Engine\FocusFunctions.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

Plane::Plane(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

Plane::Plane(bool simply):ShapeWithList(false)
{
	init();
}

Plane::~Plane()
{
	delete planePolygonPts;
}

void Plane::init()
{
	try
	{
		this->dir_a(0); this->dir_b(0); this->dir_c(1); this->dir_d(0);
		this->Flatness(0); this->FilteredFlatness(0);
		this->ShapeType = RapidEnums::SHAPETYPE::PLANE;
		this->ShapeAs3DShape(true); this->AddIntercept(false);
		this->planePolygon = false; 
		this->Offset(0);
		this->UseLightingProperties(true);
		this->ScanDirDetected(false);
		this->PlaneType = RapidEnums::PLANETYPE::SIMPLE_PLANE;
		planePolygonPts = new PointCollection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0001", __FILE__, __FUNCSIG__); }
}

TCHAR* Plane::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0002", __FILE__, __FUNCSIG__); return name; }
}

bool Plane::Shape_IsInWindow(Vector& corner1,double Tolerance)
{
	try
	{
		double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
		return Shape_IsInWindow(&SelectionL[0], Tolerance);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0003", __FILE__, __FUNCSIG__); return false; }
}

bool Plane::Shape_IsInWindow( Vector& corner1,Vector& corner2 )
{
	return false;
}

bool Plane::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		double ThisPlane[4] = {this->dir_a(), this->dir_b(), this->dir_c(), this->dir_d()};
		double CornerPoints[12] = {point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ(), point3.getX(), point3.getY(), point3.getZ(), point4.getX(), point4.getY(), point4.getZ()};
		Intersects = RMATH3DOBJECT->Intersection_Line_Plane(SelectionLine, ThisPlane, IntersectnPts);
		if(Intersects == 1)
		{
			return RMATH3DOBJECT->Checking_Point_within_Block(IntersectnPts, CornerPoints, 4);
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0004", __FILE__, __FUNCSIG__); return false; }
}

void Plane::Translate(Vector& Position)
{
	point1 += Position; point2 += Position;
	point3 += Position; point4 += Position;
	this->UpdateEnclosedRectangle();
}

void Plane::AlignToBasePlane(double* trnasformM)
{
	try
	{
		/*point1.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, point1.getX(), point1.getY(), point1.getZ()));
		point2.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, point2.getX(), point2.getY(), point2.getZ()));
		point3.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, point3.getX(), point3.getY(), point3.getZ()));
		point4.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, point4.getX(), point4.getY(), point4.getZ()));*/
	}
	catch(...){}
}

void Plane::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		if(this->Normalshape())
			drawNormalShapes(windowno, WPixelWidth);
		else
			drawAxisShapes(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0005", __FILE__, __FUNCSIG__); }
}

void Plane::drawAxisShapes(int windowno, double WPixelWidth)
{
	try
	{
		if((MAINDllOBJECT->RcadWindow3DMode()) && (!MAINDllOBJECT->SaveImageFlag))
		{
			if(windowno != 1) return;
			SinglePoint* Spt1 = this->PointsList->getList()[0];
			SinglePoint* Spt2 = this->PointsList->getList()[1];
			SinglePoint* Spt3 = this->PointsList->getList()[2];
			SinglePoint* Spt4 = this->PointsList->getList()[3];
			point1.set(Spt1->X * WPixelWidth, Spt1->Y * WPixelWidth, Spt1->Z * WPixelWidth);
			point2.set(Spt2->X * WPixelWidth, Spt2->Y * WPixelWidth, Spt2->Z * WPixelWidth);
			point3.set(Spt3->X * WPixelWidth, Spt3->Y * WPixelWidth, Spt3->Z * WPixelWidth);
			point4.set(Spt4->X * WPixelWidth, Spt4->Y * WPixelWidth, Spt4->Z * WPixelWidth);
			GRAFIX->draw_Plane(point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ(), point3.getX(), point3.getY(), point3.getZ(), point4.getX(), point4.getY(), point4.getZ(), true, false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0006", __FILE__, __FUNCSIG__); }
}
	
void Plane::drawNormalShapes(int windowno, double WPixelWidth)
{
	try
	{
		int totcount = this->PointsList->Pointscount();
		double* x = NULL;
		int n = planePolygonPts->Pointscount();
		bool allowTransparency = MAINDllOBJECT->AllowTransparencyTo3DShapes();
		if(HighlightedFormouse()) allowTransparency = false;
		int l = 0;
		if(n != 0)
		{
			x = (double*) malloc(n*sizeof(double)*3);
			for(PC_ITER Spt = this->planePolygonPts->getList().begin(); Spt != this->planePolygonPts->getList().end(); Spt++)
			{
				*(x + l) = Spt->second->X; l++;
				*(x + l) = Spt->second->Y; l++;
				*(x + l) = Spt->second->Z; l++;
				if(n == l / 3) break;
			}
		}
		else if (point1.getX() != 0 || point2.getX() != 0 || point3.getX() != 0 || point4.getX() != 0)
		{
			if(totcount < 1)
			{
				n = 4;
				x = (double*) malloc(4*sizeof(double)*3);
				Vector tmpArray[4] = {point1, point2, point3, point4};
				for (int i = 0; i < 4; i++)
				{
					*(x + l) = tmpArray[i].getX(); l++;
					*(x + l) = tmpArray[i].getY(); l++;
					*(x + l) = tmpArray[i].getZ(); l++;
				}
			}
		}
		if(!HighlightedFormouse())
		{
			double planeparam[3] = {dir_a(), dir_b(), dir_c()};
			GRAFIX->EnableLighting(MidPoint.getX(), MidPoint.getY(), MidPoint.getZ(), planeparam[0], planeparam[1], planeparam[2]);
		}
		if(PartialShape())
		{
			//use a settings option to toggle between the three options below.
			if(PartialShapeDrawMode() == 0)
				GRAFIX->draw_Plane(point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ(), point3.getX(), point3.getY(), point3.getZ(), point4.getX(), point4.getY(), point4.getZ(), allowTransparency, MAINDllOBJECT->DrawWiredSurface());
			else if(PartialShapeDrawMode() == 1)
				GRAFIX->draw_PlaneFromPts(point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ(), point3.getX(), point3.getY(), point3.getZ(), point4.getX(), point4.getY(), point4.getZ(), pts, totcount, allowTransparency, MAINDllOBJECT->DrawWiredSurface(), WPixelWidth);
			else 
			{
				if(totcount < 50)
					GRAFIX->draw_PlanePolygon(x, n, allowTransparency, MAINDllOBJECT->DrawWiredSurface());
				else
					GRAFIX->draw_Plane(point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ(), point3.getX(), point3.getY(), point3.getZ(), point4.getX(), point4.getY(), point4.getZ(), allowTransparency, MAINDllOBJECT->DrawWiredSurface());
			}
		}
		else
		{
			if(totcount < 50)
				GRAFIX->draw_PlanePolygon(x, n, allowTransparency, MAINDllOBJECT->DrawWiredSurface());
			else
				GRAFIX->draw_Plane(point1.getX(), point1.getY(), point1.getZ(), point2.getX(), point2.getY(), point2.getZ(), point3.getX(), point3.getY(), point3.getZ(), point4.getX(), point4.getY(), point4.getZ(), allowTransparency, MAINDllOBJECT->DrawWiredSurface());
		}
		if(!HighlightedFormouse())
			GRAFIX->DisableLighting();
		if(x != NULL )	{free(x);x = NULL;}	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0006", __FILE__, __FUNCSIG__); }
}

void Plane::drawGDntRefernce()
{
	try
	{
		if(RefernceDatumForgdnt() > 0)
		{
			double PlaneParam[6], pt1[3], pt2[3];
			getParameters(&PlaneParam[0]);
			point1.FillDoublePointer(&pt1[0], 3);
			point3.FillDoublePointer(&pt2[0], 3);
			double PlaneMidPt[3] = {(pt1[0] + pt2[0])/2, (pt1[1] + pt2[1])/2, (pt1[2] + pt2[2])/2};
			GRAFIX->drawReferenceDatumName_Plane(&PlaneParam[0], &PlaneMidPt[0], &DatumName, MAINDllOBJECT->getWindow(1).getUppX());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0006", __FILE__, __FUNCSIG__); }
}

void Plane::ResetShapeParameters()
{	
	try
	{
		this->dir_a(0); this->dir_b(0); this->dir_c(1); this->dir_d(0);
		this->Flatness(0); this->FilteredFlatness(0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0006a", __FILE__, __FUNCSIG__); }
}

void Plane::UpdateBestFit()
{
	try
	{
		if(PlaneType == RapidEnums::PLANETYPE::PERPENDICULARPLANE || PlaneType == RapidEnums::PLANETYPE::PARALLELPLANE || PlaneType == RapidEnums::PLANETYPE::PLANEPERPENDICULAR2LINE)
		{
			updateDerivedPlane();
		}
		int totcount = this->PointsList->Pointscount();
		if (totcount < 3) return;

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
		updatePlane(totcount);
		ProbePointCalculations(totcount);
		if(!this->ShapeDoneUsingProbe())
		{
			double tmpdir[3] = {0, 0, 1}, planedir[3] = {dir_a(), dir_b(), dir_c()};
			if(RMATH3DOBJECT->Angle_Btwn_2Directions(tmpdir, planedir, false, true) < 0)
				SetPlaneParameters(-1 * dir_a(), -1 * dir_b(), -1 * dir_c(), -1 * dir_d());
		}
		else if(this->AddIntercept())
		{
			SetPlaneParameters(-1 * dir_a(), -1 * dir_b(), -1 * dir_c(), -1 * dir_d());
		}
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0007", __FILE__, __FUNCSIG__); }
}

void Plane::updateDerivedPlane()
{
	try
	{
		this->IsValid(false);
		double actnPnts[9] = {0}, planePnts[12] = {0};
		int cnt = 0;
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			for each(RAction* Caction in this->PointAtionList)
			{
				if(cnt >= 9) break;
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
				if(cnt >= 9) break;
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
		if(PlaneType == RapidEnums::PLANETYPE::PERPENDICULARPLANE)
		{
			list<BaseItem*>::iterator Iter = this->getParents().end();
			Plane *parentPlane = (Plane*)(*(--Iter));
			if(parentPlane == NULL) return;
			this->PointsListOriginal->deleteAll();
			this->PointsList->deleteAll();
			double length = 0.5, curPlnDir[3] = {0}, parentParam[4] = {0}, tmpCos[3] = {0}, projectedPoints[6] = {0}, tmpCos2[3] = {0};
			parentPlane->getParameters(parentParam);
			RMATH3DOBJECT->Projection_Point_on_Plane(actnPnts, parentParam, projectedPoints);
			RMATH3DOBJECT->Projection_Point_on_Plane(&actnPnts[3], parentParam, &projectedPoints[3]);
			if(abs(RMATH3DOBJECT->Distance_Point_Point(projectedPoints, &projectedPoints[3])) < 0.001) return; 
			double tmpDir[3] = {projectedPoints[0] - projectedPoints[3], projectedPoints[1] - projectedPoints[4], projectedPoints[2] - projectedPoints[5]};
			RMATH3DOBJECT->DirectionCosines(tmpDir, tmpCos);
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(parentParam, tmpCos, curPlnDir);
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(curPlnDir, tmpCos, tmpDir);
			double midPnt[3] = {(projectedPoints[0] + projectedPoints[3]) / 2, (projectedPoints[1] + projectedPoints[4]) / 2, (projectedPoints[2] + projectedPoints[5]) / 2};
			double tmpDir2[3] = {actnPnts[6] - midPnt[0], actnPnts[7] - midPnt[1], actnPnts[8] - midPnt[2]};
			RMATH3DOBJECT->DirectionCosines(tmpDir2, tmpCos2);
			double dist = RMATH3DOBJECT->Distance_Point_Point(&actnPnts[6], midPnt);
			for(int i = 0; i < 3; i++)
			{
				planePnts[i] = (actnPnts[i] + tmpDir[i] * length) + dist * tmpCos2[i];
				planePnts[3 + i] = (actnPnts[i] - tmpDir[i] * length) + dist * tmpCos2[i];
				planePnts[6 + i] = (actnPnts[3 + i] + tmpDir[i] * length) + dist * tmpCos2[i];
				planePnts[9 + i] = (actnPnts[3 + i] - tmpDir[i] * length) + dist * tmpCos2[i];
			}
		}
		else if(PlaneType == RapidEnums::PLANETYPE::PARALLELPLANE)
		{
			double parentEndPnts[12] = {0}, parentParam[4] = {0}, curD = 0;
			this->PointsListOriginal->deleteAll();
			this->PointsList->deleteAll();
			list<BaseItem*>::iterator Iter = this->getParents().end();
			Plane *parentPlane = (Plane*)(*(--Iter));
			parentPlane->getParameters(parentParam);
			parentPlane->getEndPoints(parentEndPnts);
			curD = parentParam[0] * actnPnts[0] + parentParam[1] * actnPnts[1] + parentParam[2] * actnPnts[2];
			if (this->DefinedOffset())
				parentParam[3] += Offset();
			else
				parentParam[3] = curD;

			for(int i = 0; i < 4; i++)
				RMATH3DOBJECT->Projection_Point_on_Plane(&parentEndPnts[3 *i], parentParam, &planePnts[3 *i]);
		}
		else if(PlaneType == RapidEnums::PLANETYPE::PLANEPERPENDICULAR2LINE)
		{
			double parentParam[6] = {0};
			this->PointsListOriginal->deleteAll();
			this->PointsList->deleteAll();
			list<BaseItem*>::iterator Iter = this->getParents().end();
			Line *parentLine = (Line*)(*(--Iter));
			parentLine->getParameters(parentParam);
			double dir[3] = {1,0,0}, dir1[3] = {0}, dir2[3] = {0};
			if(abs(RMATH3DOBJECT->Angle_Btwn_2Directions(dir, &parentParam[3], false, true)) > .9)
			{
				dir[0] = 0;
				dir[1] = 1;
			}
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(dir, &parentParam[3], dir1);
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(dir1, &parentParam[3], dir2); 
			double length = parentLine->Length();
			if(length == 0) length = 1;
			for(int i = 0; i < 3; i++) 
			{
				planePnts[i] = actnPnts[i] + length * dir1[i];
				planePnts[3 + i] = actnPnts[i] + length * dir2[i];
				planePnts[6 + i] = actnPnts[i] - length * dir1[i];
				planePnts[9 + i] = actnPnts[i] - length * dir2[i];
			}
		}
		this->PointsList->Addpoint(new SinglePoint(planePnts[0], planePnts[1], planePnts[2]));
		this->PointsList->Addpoint(new SinglePoint(planePnts[3], planePnts[4], planePnts[5]));
		this->PointsList->Addpoint(new SinglePoint(planePnts[6], planePnts[7], planePnts[8]));
		this->PointsList->Addpoint(new SinglePoint(planePnts[9], planePnts[10], planePnts[11]));
		this->PointsListOriginal->CopyAllPoints(this->PointsList);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0009", __FILE__, __FUNCSIG__); }
}

void Plane::updatePlane(int totcount)
{
	try
	{
		if (totcount < 3) return;
		double ans[5] = {0, 0, 1, 0, 0}, PPoints[3] = {0};
		double LeftTop[3] = {0},RightBottom[3] = {0}, Leftbottom[3] = {0}, RightTop[3] = {0};
		bool sigmamode = false;
		if(PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->SigmaModeInPP)
			sigmamode = true;
		else
			sigmamode = this->SigmaMode();
		if(totcount == 3) //Call the 3 point Best fit..
			BESTFITOBJECT->Plane_3Pt_2(pts, &ans[0]);
		else	// If the point size is greater than 3...
			BESTFITOBJECT->Plane_BestFit(pts, totcount, &ans[0], sigmamode);
		
		if(ApplyFilterCorrection() && totcount > 5)
		{
			double Plane_Parameter[4] = {ans[0], ans[1], ans[2], ans[3] * (-1)};
			int cnt = totcount;
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
			{
				cnt = HELPEROBJECT->FilteredPlane(PointsList, Plane_Parameter, FilterCorrectionFactor(), i);
				if(cnt < 7) break;
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
				BESTFITOBJECT->Plane_BestFit(filterpts, filterptsCnt, &ans[0], sigmamode);
			}
		}
		if(ApplyMMCCorrection() && ScanDirDetected() && totcount > 5)
		{
			double Plane_Parameter[4] = {ans[0], ans[1], ans[2], ans[3] * (-1)};
			int cnt = totcount;
			for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
			{
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
				cnt = HELPEROBJECT->OutermostInnerMostPlane(PointsList, Plane_Parameter, useValidPointOnly, ScanDir);
				if(cnt < 7) break;
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
				BESTFITOBJECT->Plane_BestFit(filterpts, filterptsCnt, &ans[0], sigmamode);
			}
		}

		ans[3] = -1 * ans[3];
		//ans[1] = -1 * ans[1];
		double planeparam[4] = {0, 0, 1, 0};
		getParameters(planeparam);
		//this will make sure we don't drawCurrentShape polygon from scratch each time
		//if ((abs(planeparam[0] - ans[0]) < 0.1) && (abs(planeparam[1] - ans[1]) < 0.1) && (abs(planeparam[2] - ans[2]) < 0.1 ))
		//	planePolygon = true;
		//else
		planePolygon = false;
		SetPlaneParameters(ans[0], ans[1], ans[2], ans[3]);
		//moving this before plane polygon calculation to ensure it is not getting skipped due to any catch situation
		double Pl[4] = {dir_a(), dir_b(), dir_c(), dir_d()};
		this->Flatness(RMATH3DOBJECT->Flatness(pts, totcount, &Pl[0]));
		this->FilteredFlatness(RMATH3DOBJECT->FilteredFlatness(pts, totcount, &Pl[0], FlatnessFilterFactor()));
		double Cplane[4] = {ans[0], ans[1], ans[2], ans[3]};
		double TranformationMatrix[9] = {0}, InverseMatrix[9] = {0};
		RMATH3DOBJECT->GetTMatrixForPlane(&Cplane[0], &TranformationMatrix[0]);

		int s1[2] = {3, 3}, s2[2] = {3, 1};
		double TransformedPt[4] = {0}, CmousePt[4] = {0, 0, 0, 1};
		double MinX = 0, MinY = 0, MaxX = 0, MaxY = 0;
		PtsList planePts;
		for(int i = 0; i < totcount; i++)
		{
			CmousePt[0] = pts[3 * i]; CmousePt[1] = pts[3 * i + 1]; CmousePt[2] = pts[3 * i + 2];
			RMATH2DOBJECT->MultiplyMatrix1(&TranformationMatrix[0], &s1[0], &CmousePt[0], &s2[0], &TransformedPt[0]);
			if (i == totcount - 1)
			{
				new_pt.x = TransformedPt[0];
				new_pt.y = TransformedPt[1];
			}
			Pt tmpPt;

			tmpPt.x = TransformedPt[0];
			tmpPt.y = TransformedPt[1];

			planePts.push_back(tmpPt);

			if(i == 0)
			{
				MaxX = MinX = TransformedPt[0];
				MaxY = MinY = TransformedPt[1];
			}
			else
			{
				if(TransformedPt[0] < MinX)
					MinX = TransformedPt[0];
				if(TransformedPt[1] < MinY)
					MinY = TransformedPt[1];
				if(TransformedPt[0] > MaxX)
					MaxX = TransformedPt[0];
				if(TransformedPt[1] > MaxY)
					MaxY = TransformedPt[1];
			}
		}
		if(FOCUSCALCOBJECT->CurrentFocusType != RapidEnums::RAPIDFOCUSMODE::CALIBRATION_MODE )
		{
			if (totcount < 50)
			{
				if (planePolygon)
				{
					if (!BESTFITOBJECT->getUpdatedPolygonLines(new_pt, &planePolygonLines))
						return;
				}
				else
				{
					if (!BESTFITOBJECT->getPolygonLines(&planePts, &planePolygonLines))
					{
						planePts.clear();
						planePolygonLines.clear();
						return;
					}
					planePolygon = true;
				}
				planePts.clear();
				planePolygonPts->deleteAll();
				for (LineArcList::iterator i = planePolygonLines.begin(); i != planePolygonLines.end(); i++)
				{
					planePolygonPts->Addpoint(new SinglePoint(i->start_pt.x, i->start_pt.y, ans[3]));
				}
			}

			Leftbottom[0] = MinX; Leftbottom[1] = MinY; Leftbottom[2] = ans[3];
			LeftTop[0] = MinX; LeftTop[1] = MaxY; LeftTop[2] = ans[3];
			RightTop[0] = MaxX; RightTop[1] = MaxY; RightTop[2] = ans[3];
			RightBottom[0] = MaxX; RightBottom[1] = MinY; RightBottom[2] = ans[3];

			RMATH2DOBJECT->OperateMatrix4X4(&TranformationMatrix[0], 3, 1, &InverseMatrix[0]);

			RMATH2DOBJECT->MultiplyMatrix1(&InverseMatrix[0], &s1[0], &Leftbottom[0], &s2[0], &TransformedPt[0]);point1.set(TransformedPt[0], TransformedPt[1], TransformedPt[2]);
			RMATH2DOBJECT->MultiplyMatrix1(&InverseMatrix[0], &s1[0], &LeftTop[0], &s2[0], &TransformedPt[0]);point2.set(TransformedPt[0], TransformedPt[1], TransformedPt[2]);
			RMATH2DOBJECT->MultiplyMatrix1(&InverseMatrix[0], &s1[0], &RightTop[0], &s2[0], &TransformedPt[0]);point3.set(TransformedPt[0], TransformedPt[1], TransformedPt[2]);
			RMATH2DOBJECT->MultiplyMatrix1(&InverseMatrix[0], &s1[0], &RightBottom[0], &s2[0], &TransformedPt[0]);point4.set(TransformedPt[0], TransformedPt[1], TransformedPt[2]);
		
			MidPoint.set((point1.getX() + point3.getX())/2, (point1.getY() + point3.getY())/2, (point1.getZ() + point3.getZ())/2);

			if (totcount < 50)
			{
				double x[3] = {0};
				for (int i = 0; i < planePolygonPts->Pointscount(); i++)
				{
					x[0] = planePolygonPts->getList()[i]->X;
					x[1] = planePolygonPts->getList()[i]->Y;
					x[2] = planePolygonPts->getList()[i]->Z;

					RMATH2DOBJECT->MultiplyMatrix1(&InverseMatrix[0], &s1[0], x, &s2[0], &TransformedPt[0]);
					planePolygonPts->getList()[i]->X = TransformedPt[0];
					planePolygonPts->getList()[i]->Y = TransformedPt[1];
					planePolygonPts->getList()[i]->Z = TransformedPt[2];
				}
			}
		}
		IsValid(true);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0007A", __FILE__, __FUNCSIG__); 
	}
}

bool Plane::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		 int s1[2] = {4, 4}, s2[2] = {4, 1};
		int n = this->PointsList->Pointscount();
		double *TransformePpoint = NULL, *tempPoint = NULL;
		TransformePpoint = new double[(n + 1) * 2];
		tempPoint = new double[n * 4];
		int Cnt = 0;
		for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			tempPoint[Cnt++] = Spt->X;
			tempPoint[Cnt++] = Spt->Y;
			tempPoint[Cnt++] = Spt->Z;
			tempPoint[Cnt++] = 1;
		}		
		 for(int i = 0; i < n; i++)
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &tempPoint[4 * i], &s2[0], &TransformePpoint[2 * i]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		delete [] tempPoint;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0006", __FILE__, __FUNCSIG__); return false; }
}


void Plane::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt >= 3) // && PointsCnt < 10)
		{
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();	
			if(ShapeDoneUsingProbe())
				CheckAddOrSubtractIntercept();
		}
		if(ShapeDoneUsingProbe())
		{
			updatePlane_UsingPorbe();
			updatePlane(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0015", __FILE__, __FUNCSIG__); }
}

void Plane::CheckPointIsForPorbe()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0016", __FILE__, __FUNCSIG__); }
}

void Plane::CheckAddOrSubtractIntercept()
{
	try
	{
		int IDCount = 0;
		double DirCosine[3] = {0};
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			int type = (int)Spt->Pdir;
			switch(type)
			{
			case 0:
				DirCosine[0] = -1; DirCosine[1] = 0; DirCosine[2] = 0;
				break;
			case 1:
				DirCosine[0] = 1; DirCosine[1] = 0; DirCosine[2] = 0;
				break;
			case 2:
				DirCosine[0] = 0; DirCosine[1] = -1; DirCosine[2] = 0;
				break;
			case 3:
				DirCosine[0] = 0; DirCosine[1] = 1; DirCosine[2] = 0;
				break;
			case 4:
				DirCosine[0] = 0; DirCosine[1] = 0; DirCosine[2] = -1;
				break;
			case 5:
				DirCosine[0] = 0; DirCosine[1] = 0; DirCosine[2] = 1;
				break;
			}

			if((DirCosine[0] * this->dir_a() + DirCosine[1] * this->dir_b() + DirCosine[2] * this->dir_c()) < 0)
				IDCount--;
			else
				IDCount++;
		}
		if(IDCount > 0)
			this->AddIntercept(true);
		else
			this->AddIntercept(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0017", __FILE__, __FUNCSIG__); }
}

// Added on 28 May 12 ...........
void Plane::updatePlane_UsingPorbe()
{
	try
	{
		double Pradius = 0;
		int multiplyfactor = -1;
		if(this->AddIntercept()) multiplyfactor = 1;

		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double SurfacePt[3] = {0};
			Pradius = getProbeRadius(Spt->PLR);
			SurfacePt[0] = Spt->X + multiplyfactor * Pradius * this->dir_a();
			SurfacePt[1] = Spt->Y + multiplyfactor * Pradius * this->dir_b();
			SurfacePt[2] = Spt->Z + multiplyfactor * Pradius * this->dir_c();
			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(SurfacePt[0], SurfacePt[1], SurfacePt[2], Spt->R, Spt->Pdir);
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
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0018", __FILE__, __FUNCSIG__); 
	}
}

void Plane::UpdateForParentChange(BaseItem* sender)
{
	if(PlaneType == RapidEnums::PLANETYPE::PERPENDICULARPLANE || PlaneType == RapidEnums::PLANETYPE::PARALLELPLANE || PlaneType == RapidEnums::PLANETYPE::PLANEPERPENDICULAR2LINE)
	{
		UpdateBestFit();
	}
}

void Plane::Transform(double* transform)
{
	try
	{
		point1.set(MAINDllOBJECT->TransformMultiply(transform,point1.getX(),point1.getY(),point1.getZ()));
		point2.set(MAINDllOBJECT->TransformMultiply(transform,point2.getX(),point2.getY(),point2.getZ()));
		point3.set(MAINDllOBJECT->TransformMultiply(transform,point3.getX(),point3.getY(),point3.getZ()));
		point4.set(MAINDllOBJECT->TransformMultiply(transform,point4.getX(),point4.getY(),point4.getZ()));
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0008", __FILE__, __FUNCSIG__); }
}

void Plane::GetShapeCenter(double *cPoint)
{
	cPoint[0] = (point1.getX() + point3.getX()) / 2;
	cPoint[1] = (point1.getY() + point3.getY()) / 2;
	cPoint[2] = (point1.getZ() + point3.getZ()) / 2;
}

void Plane::UpdateEnclosedRectangle()
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1}, n = this->PointsList->Pointscount();	
		double *TransformePpoint = NULL;
		TransformePpoint = new double[n * 2 + 1];
		int Cnt = 0;
		for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			TransformePpoint[Cnt++] = Spt->X;
			TransformePpoint[Cnt++] = Spt->Y;
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0},  *Temporary_point = NULL;	
			Temporary_point = new double[n * 3];
			Cnt = 0;
			for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				Temporary_point[Cnt++] = Spt->X;
				Temporary_point[Cnt++] = Spt->Y;
				Temporary_point[Cnt++] = Spt->Z;
			}
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			for(int i = 0; i < n; i++)
				RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &Temporary_point[3 * i], Order2, &TransformePpoint[2 * i]);	
			delete [] Temporary_point;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, ShapeLeftTop, ShapeRightBottom);
		delete [] TransformePpoint;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0008a", __FILE__, __FUNCSIG__); }
}


Shape* Plane::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("PL");
		else
			myname = _T("dPL");
		Plane* CShape = new Plane((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		   CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0008", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Plane::CreateDummyCopy()
{
	try
	{
		Plane* CShape = new Plane(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0009", __FILE__, __FUNCSIG__); return NULL; }
}


void Plane::CopyShapeParameters(Shape* s)
{
	try
	{
		this->point1.set(*((Plane*)s)->getPoint1());
		this->point2.set(*((Plane*)s)->getPoint2());
		this->point3.set(*((Plane*)s)->getPoint3());
		this->point4.set(*((Plane*)s)->getPoint4());
		this->dir_a(((Plane*)s)->dir_a());
		this->dir_b(((Plane*)s)->dir_b());
		this->dir_c(((Plane*)s)->dir_c());
		this->dir_d(((Plane*)s)->dir_d());
		this->PointsList->deleteAll();
		this->PointsListOriginal->deleteAll();
		for (LineArcList::iterator i = ((Plane*)s)->planePolygonLines.begin(); i != ((Plane*)s)->planePolygonLines.end(); i++)
		{
			this->planePolygonLines.push_back(*i);
		}
		if(((Plane*)s)->planePolygonPts->getList().size() > 0)
			this->AddPoints(((Plane*)s)->planePolygonPts);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0010", __FILE__, __FUNCSIG__); }
}

void Plane::SetPlanePoints(Vector& v1, Vector& v2, Vector& v3, Vector& v4)
{
	point1.set(v1); point2.set(v2); point3.set(v3); point4.set(v4);
}

void Plane::setPoint1(Vector& t)
{
	point1.set(t);
}

void Plane::setPoint2(Vector& t)
{
	point2.set(t);
}

void Plane::setPoint3(Vector& t)
{
	point3.set(t);
}

void Plane::setPoint4(Vector& t)
{
	point4.set(t);
}

Vector* Plane::getPoint1()
{
	return &(this->point1);
}

Vector* Plane::getPoint2()
{
	return &(this->point2);
}

Vector* Plane::getPoint3()
{
	return &(this->point3);
}

Vector* Plane::getPoint4()
{
	return &(this->point4);
}

Vector* Plane::getMidPoint()
{
	return &(this->MidPoint);
}

void Plane::SetPlaneParameters(double a, double b, double c, double d)
{
	dir_a(a); dir_b(b);	dir_c(c);dir_d(d);
}

void Plane::getParameters(double* planeparam)
{
	planeparam[0] = dir_a(); planeparam[1] = dir_b(); planeparam[2] = dir_c(); planeparam[3] = dir_d();
}

void Plane::getEndPoints(double* endpoints)
{
	point1.FillDoublePointer(endpoints, 3);
	point2.FillDoublePointer(endpoints + 3, 3);
	point3.FillDoublePointer(endpoints + 6, 3);
	point4.FillDoublePointer(endpoints + 9, 3);
}


bool Plane::WithinCirclularView(double *SelectionLine, double Tolerance)
{
	double ThisPlane[4] = {dir_a(), dir_b(), dir_c(), dir_d()};
	double dist = RMATH3DOBJECT->Distance_Line_Plane(SelectionLine, ThisPlane);
	if(dist < Tolerance) return true;
	return false;
}


double Plane::GetZlevel()
{
	return (point1.getZ() + point3.getZ())/2;
}


int Plane::shapenumber=0;


void Plane::decrement()
{
	shapenumber--;
}

void Plane::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, Plane& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "Plane" << endl;
		os << "Point1:values" << endl << (*static_cast<Vector*>(x.getPoint1())) << endl;
		os << "Point2:values" << endl << (*static_cast<Vector*>(x.getPoint2())) << endl;
		os << "Point3:values" << endl << (*static_cast<Vector*>(x.getPoint3())) << endl;
		os << "Point4:values" << endl << (*static_cast<Vector*>(x.getPoint4())) << endl;
		os << "dir_a:" << x.dir_a() << endl;
		os << "dir_b:" << x.dir_b() << endl;
		os << "dir_c:" << x.dir_c() << endl;
		os << "dir_d:" << x.dir_d() << endl;
		os << "PlaneType:" << x.PlaneType << endl;
		os << "DefinedOffset:" << x.DefinedOffset() << endl;
		os << "Offset:" << x.Offset() << endl;
		os << "EndPlane" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, Plane& x )
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
			if(Tagname==L"Plane")
			{
				x.ShapeType = RapidEnums::SHAPETYPE::PLANE;
				while(Tagname!=L"EndPlane")
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
						if(Tagname==L"Point1")
						{
							is >> (*static_cast<Vector*>(x.getPoint1()));				
						}
						else if(Tagname==L"Point2")
						{
							is >> (*static_cast<Vector*>(x.getPoint2()));					
						}
						else if(Tagname==L"Point3")
						{
							is >> (*static_cast<Vector*>(x.getPoint3()));					
						}
						else if(Tagname==L"Point4")
						{
							is >> (*static_cast<Vector*>(x.getPoint4()));					
						}
						else if(Tagname==L"dir_a")
						{
							x.dir_a(atof((const char*)(Val).c_str()));					
						}
						else if(Tagname==L"dir_b")
						{
							x.dir_b(atof((const char*)(Val).c_str()));				
						}
						else if(Tagname==L"dir_c")
						{
							x.dir_c(atof((const char*)(Val).c_str()));					
						}
						else if(Tagname==L"dir_d")
						{
							x.dir_d(atof((const char*)(Val).c_str()));					
						}
						else if(Tagname==L"PlaneType")
						{
							x.PlaneType = RapidEnums::PLANETYPE(atoi((const char*)(Val).c_str()));			
						}
						else if (Tagname == L"DefinedOffset")
						{
							if (Val == "0")
								x.DefinedOffset(false);
							else
								x.DefinedOffset(true);
						}
						else if (Tagname == L"Offset")
						{
							x.Offset(atof((const char*)(Val).c_str()));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, Plane& x)
{
	try
	{
		int n;
		double d;
		x.ShapeType = RapidEnums::SHAPETYPE::PLANE;
		is >> (*static_cast<Vector*>(x.getPoint1()));
		is >> (*static_cast<Vector*>(x.getPoint2()));
		is >> (*static_cast<Vector*>(x.getPoint3()));
		is >> (*static_cast<Vector*>(x.getPoint4()));
		is >> d; x.dir_a(d);
		is >> d; x.dir_b(d);
		is >> d; x.dir_c(d);
		is >> d; x.dir_d(d);
		is >> n; x.PlaneType = RapidEnums::PLANETYPE(n);
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("PLANE0012", __FILE__, __FUNCSIG__); }
}