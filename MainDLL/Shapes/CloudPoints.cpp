#include "StdAfx.h"
#include "CloudPoints.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\TriangulationCalc.h"
#include "..\DXF\DXFExpose.h"
#include "TriangularSurface.h"
#include "Plane.h"
#include "IntersectionShape.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"
#include "Plane.h"

CloudPoints::CloudPoints(TCHAR *myname):ShapeWithList(genName(myname))
{
	init();
}

CloudPoints::CloudPoints(bool simply):ShapeWithList(false)
{
	init();
}

CloudPoints::~CloudPoints()
{
	IsValid(false);
	if(AllTrianglelist != NULL)
		free(AllTrianglelist);
	if(PlaneParams != NULL)
		free(PlaneParams);
	if(PlaneMidPointColl != NULL)
		free(PlaneMidPointColl);
	if(CloudPointsForMeasure != NULL)
		free(CloudPointsForMeasure);
	ClearDeviationColorMemory();
	TempShapeCollection.clear();
}

void CloudPoints::init()
{
	this->ShapeType = RapidEnums::SHAPETYPE::CLOUDPOINTS;
	this->ShapeAs3DShape(true);
	this->trianglecount = 0;
	this->AllTrianglelist = NULL;
	this->PlaneParams = NULL;
	this->PlaneMidPointColl = NULL;
	this->UseLightingProperties(true);
	this->CloudPointsForLine = false;
	this->SelectedPtsOnly(false);
	this->toleranceComparision = false;
	this->ToleranceValue = 0;
	this->CloudPointsType = RapidEnums::CLOUDPOINTSTYPE::DEFAULT;
	this->CloudPointsForMeasure = NULL;
	this->PointsForMeasureCnt = 0;
	this->IdealSurface = NULL;
	this->MaximumDistance = 0;
}

TCHAR* CloudPoints::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		if(strcmp((const char*)prefix, "d") == 0)
		{
			DXFshapenumber++;
			_itot_s(DXFshapenumber, shapenumstr, 10, 10);
		}	
		else
		{
			shapenumber++;
			_itot_s(shapenumber, shapenumstr, 10, 10);
		}
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0001", __FILE__, __FUNCSIG__); return name; }
}

bool CloudPoints::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	return false;
}

bool CloudPoints::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool CloudPoints::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void CloudPoints::Translate(Vector& Position)
{
	try
	{
		if(this->SelectedPtsOnly())
		{
			for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				if(Spt->IsSelected)
					Spt->SetValues(Spt->X + Position.getX(), Spt->Y + Position.getY(), Spt->Z + Position.getZ(), Spt->R, Spt->Pdir);
			}
		}
		else
		{
			for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				Spt->SetValues(Spt->X + Position.getX(), Spt->Y + Position.getY(), Spt->Z + Position.getZ(), Spt->R, Spt->Pdir);
			}
		}
		
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		filterptsCnt = 0 ;
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
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0001b", __FILE__, __FUNCSIG__); }
}

Vector* CloudPoints::getMinpoint()
{
	return &(Minpoint);
}

Vector* CloudPoints::getMaxpoint()
{
	return &(Maxpoint);
}

void CloudPoints::CompareWithStlSurface(double tolerance)
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		this->toleranceComparision = true;
		this->ToleranceValue = tolerance;
		this->UpdateBestFit();
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0001c", __FILE__, __FUNCSIG__); MAINDllOBJECT->dontUpdateGraphcis = false; }
}

void CloudPoints::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		if(CloudPointsForLine)
		{
			list<Pt>::iterator Sptnext;
			for(list<Pt>::iterator Spt = AllPointsCollection.begin(); Spt != AllPointsCollection.end(); Spt++)
			{
				Sptnext = Spt;
				Sptnext++;
				if(Sptnext != AllPointsCollection.end())
				{
					double point1[2] = {Spt->x, Spt->y};
					double point2[2] = {Sptnext->x, Sptnext->y};
					GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
				}
			}
		}
		else if(MAINDllOBJECT->TriangulateCpSurface())
		{
			if(trianglecount == 0)
			{
				int totcount = this->PointsList->Pointscount();
				if(this->ShapeAs3DShape())
					GRAFIX->drawPoints(pts, totcount, false, 3, FGPOINTSIZE);
				else
					GRAFIX->drawPoints(pts, totcount, false, 2, FGPOINTSIZE);
			}
			else
			{
				if(MAINDllOBJECT->ShowPictorialView() && abs(this->Maxpoint.getZ() - this->Minpoint.getZ()) > 0.001)
				{
					if(!MAINDllOBJECT->EnableTriangulationLighting)
					{
						for(int i = 0; i < trianglecount; i++)
							GRAFIX->draw_TriangleWithColour(AllTrianglelist + 9 * i, this->Minpoint.getZ(), this->Maxpoint.getZ(), MAINDllOBJECT->AllowTransparencyTo3DShapes(), MAINDllOBJECT->DrawWiredSurface(), true, MAINDllOBJECT->DiscreteColor, MAINDllOBJECT->ColorThreshold);
					}
					else
					{
						for(int i = 0; i < trianglecount; i++)
						{
							GRAFIX->EnableLighting(PlaneMidPointColl[3 * i], PlaneMidPointColl[3 * i + 1], PlaneMidPointColl[3 * i + 2], PlaneParams[4 * i], PlaneParams[4 * i + 1], PlaneParams[4 * i + 2]);
							GRAFIX->draw_TriangleWithColour(AllTrianglelist + 9 * i, this->Minpoint.getZ(), this->Maxpoint.getZ(), MAINDllOBJECT->AllowTransparencyTo3DShapes(), MAINDllOBJECT->DrawWiredSurface(), false);
							GRAFIX->DisableLighting();
						}
					}
				}
				else
				{
					for(int i = 0; i < trianglecount; i++)
					{
						GRAFIX->EnableLighting(PlaneMidPointColl[3 * i], PlaneMidPointColl[3 * i + 1], PlaneMidPointColl[3 * i + 2], PlaneParams[4 * i], PlaneParams[4 * i + 1], PlaneParams[4 * i + 2]);
						GRAFIX->draw_PlanePolygon(AllTrianglelist + 9 * i, 3, MAINDllOBJECT->AllowTransparencyTo3DShapes(), MAINDllOBJECT->DrawWiredSurface());
						GRAFIX->DisableLighting();
					}
				}
			}
		}
		else if(toleranceComparision)
		{
			double* WithinTol = new double[this->WithinToleranceList.size() * 3];
			double* OutOfTol = new double[this->OutOfToleranceList.size() * 3];
			int Cnt = 0;
			for(std::list<SinglePoint*>::iterator itr = this->OutOfToleranceList.begin(); itr != this->OutOfToleranceList.end(); itr++)
			{
				OutOfTol[Cnt++] = (*itr)->X; OutOfTol[Cnt++] = (*itr)->Y; OutOfTol[Cnt++] = (*itr)->Z;
			}
			Cnt = 0;
			for(std::list<SinglePoint*>::iterator itr = this->WithinToleranceList.begin(); itr != this->WithinToleranceList.end(); itr++)
			{
				WithinTol[Cnt++] = (*itr)->X; WithinTol[Cnt++] = (*itr)->Y; WithinTol[Cnt++] = (*itr)->Z;
			}

			GRAFIX->drawPoints(WithinTol, this->WithinToleranceList.size(), false, true, 3, FGPOINTSIZE);
			GRAFIX->drawPoints(OutOfTol, this->OutOfToleranceList.size(), false, false, 3, FGPOINTSIZE);

			delete [] WithinTol;
			delete [] OutOfTol;
		}
		else if(this->PtsListWithColor.size() > 0)
		{
			for each(PtWithColor* TmpPtsColor in PtsListWithColor)
			{
				//GRAFIX->SetColor_Double(TmpPtsColor.R, TmpPtsColor.G, TmpPtsColor.B);
				//GRAFIX->drawPoint(TmpPtsColor.X, TmpPtsColor.Y, TmpPtsColor.Z, FGPOINTSIZE);
				GRAFIX->DrawPointsWithColor(TmpPtsColor->X, TmpPtsColor->Y, TmpPtsColor->Z, TmpPtsColor->R, TmpPtsColor->G, TmpPtsColor->B, FGPOINTSIZE);
			}
		}
		else
		{
			int totcount = this->PointsList->Pointscount();
			if(this->ShapeAs3DShape())
			{
				if(MAINDllOBJECT->ShowPictorialView())
				{
					if(abs(this->Maxpoint.getZ() - this->Minpoint.getZ()) < 0.001)
						GRAFIX->drawPoints(pts, totcount, false, 3, FGPOINTSIZE);
					else
						GRAFIX->drawPoints(pts, totcount, this->Minpoint.getZ(), this->Maxpoint.getZ(), false, MAINDllOBJECT->DiscreteColor, MAINDllOBJECT->ColorThreshold, FGPOINTSIZE);
				}
				else
					GRAFIX->drawPoints(pts, totcount, false, 3, FGPOINTSIZE);	
			}
			else
				GRAFIX->drawPoints(pts, totcount, false, 2, FGPOINTSIZE);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0002", __FILE__, __FUNCSIG__); }
}

void CloudPoints::drawGDntRefernce()
{
}

void CloudPoints::ResetShapeParameters()
{
}

void CloudPoints::UpdateBestFit()
{
	try
	{
		if(DXFEXPOSEOBJECT->LoadDXF) 
			return;
		IsValid(true);
		if(CloudPointsForLine)
		{
			CreateLines();
		}
		else if(this->CloudPointsType == RapidEnums::CLOUDPOINTSTYPE::INTERSECTIONSHAPEPROJECTION)
		{
			if(this->getParents().empty()) return;
			ShapeWithList *parentShape1 = (ShapeWithList*)(*(this->getParents().begin()));
			list<BaseItem*>::iterator item = this->getParents().begin();
			Plane *parentShape2 = (Plane*)(*(++item));
			double planeParam[4] = {0};
			parentShape2->getParameters(planeParam);
			int pointCnt = ((IntersectionShape*) parentShape1)->pntscount;
			double *intersctPts = ((IntersectionShape*) parentShape1)->intersectPts;
			double projpnt[3] = {0};
			this->PointsList->deleteAll();
			this->PointsListOriginal->deleteAll();
			if(this->pts != NULL){free(pts); pts = NULL;}
				this->pts = (double*)malloc(sizeof(double) * pointCnt * 3);
			if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * pointCnt * 3);
			filterptsCnt = 0; 
			for(int j = 0; j < pointCnt; j++)
			{
				RMATH3DOBJECT->Projection_Point_on_Plane(&intersctPts[3 * j], planeParam, projpnt);
				this->PointsList->Addpoint(new SinglePoint(projpnt[0], projpnt[1], projpnt[2]));
				this->PointsListOriginal->Addpoint(new SinglePoint(projpnt[0], projpnt[1], projpnt[2]));
				pts[3 * j] = projpnt[0];
				pts[3 * j + 1] = projpnt[1];
				pts[3 * j + 2] = projpnt[2];
				filterpts[3 * j] = projpnt[0];
				filterpts[3 * j + 1] = projpnt[1];
				filterpts[3 * j + 2] = projpnt[2];
				filterptsCnt++;
			}
			intersctPts = NULL;
			this->notifyAll(ITEMSTATUS::DATACHANGED, this);
			MAINDllOBJECT->UpdateShapesChanged();
		}
		else if(this->CloudPointsType == RapidEnums::CLOUDPOINTSTYPE::INTERSECTIONSHAPEPROJECTION)
		{
			if(this->getParents().empty()) return;
			ShapeWithList *parentShape1 = (ShapeWithList*)(*(this->getParents().begin()));
			list<BaseItem*>::iterator item = this->getParents().begin();
			Plane *parentShape2 = (Plane*)(*(++item));
			double planeParam[4] = {0};
			parentShape2->getParameters(planeParam);
			int pointCnt = ((IntersectionShape*) parentShape1)->pntscount, j = 0, n = 0;
			double projpnt[3] = {0};
			this->PointsList->deleteAll();
			this->PointsListOriginal->deleteAll();
			if(this->pts != NULL){free(pts); pts = NULL;}
				this->pts = (double*)malloc(sizeof(double) * pointCnt * 3);
			if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * pointCnt * 3);
			filterptsCnt = 0;
			for(map<int, SinglePoint*>::iterator spt = parentShape1->PointsList->getList().begin(); spt != parentShape1->PointsList->getList().end() ; spt++)
			{
				double curP[3] = {spt->second->X, spt->second->Y, spt->second->Z};
				RMATH3DOBJECT->Projection_Point_on_Plane(curP, planeParam, projpnt);
				this->PointsList->Addpoint(new SinglePoint(projpnt[0], projpnt[1], projpnt[2]));
				this->PointsListOriginal->Addpoint(new SinglePoint(projpnt[0], projpnt[1], projpnt[2]));
				pts[j++] = projpnt[0];
				pts[j++] = projpnt[1];
				pts[j++] = projpnt[2];
				filterpts[n++] = projpnt[0];
				filterpts[n++] = projpnt[1];
				filterpts[n++] = projpnt[2];
				filterptsCnt++;
			}
			this->notifyAll(ITEMSTATUS::DATACHANGED, this);
			MAINDllOBJECT->UpdateShapesChanged();
		}
		else if(MAINDllOBJECT->TriangulateCpSurface())
		{
			int ptscount = this->PointsList->Pointscount();
			if(ptscount >= 3)
			{
				/*DWORD d1, d2, d3;
				d1 = GetTickCount();*/
				if(AllTrianglelist != NULL) free(AllTrianglelist);
				TriangulationCalc TCObject(MAINDllOBJECT->UseMultiThreads_Surfacebuild(), MAINDllOBJECT->PointClusterSize());
				trianglecount = TCObject.Triangulate3DPointCollection(*PointsList, &AllTrianglelist, MAINDllOBJECT->CloudPtdistance());
				/*d2 = GetTickCount();
				d3 = d2 - d1;
				d3++;*/
			}
			if(trianglecount > 0)
			{
				if(PlaneParams != NULL) free(PlaneParams);
				if(PlaneMidPointColl != NULL) free(PlaneMidPointColl);
				PlaneParams = (double*)calloc(trianglecount * 4, sizeof(double));
				PlaneMidPointColl = (double*)calloc(trianglecount * 3, sizeof(double));
				for(int i = 0; i < trianglecount; i++)
				{
					BESTFITOBJECT->Plane_3Pt_2(AllTrianglelist + 9 * i, PlaneParams + 4 * i);
					PlaneMidPointColl[3 * i] = (AllTrianglelist[9 * i] + AllTrianglelist[9 * i + 3] + AllTrianglelist[9 * i + 6])/3;
					PlaneMidPointColl[3 * i + 1] = (AllTrianglelist[9 * i + 1] + AllTrianglelist[9 * i + 4] + AllTrianglelist[9 * i + 7])/3;
					PlaneMidPointColl[3 * i + 2] = (AllTrianglelist[9 * i + 2] + AllTrianglelist[9 * i + 5] + AllTrianglelist[9 * i + 8])/3;
				}
			}
		}
		else if(this->toleranceComparision)
		{
			/*DWORD d1, d2, d3;
			d1 = GetTickCount();*/
			this->WithinToleranceList.clear(); this->OutOfToleranceList.clear();
			for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				this->OutOfToleranceList.push_back(Spt);
			}
			for(RC_ITER item = MAINDllOBJECT->getShapesList().getList().begin(); item != MAINDllOBJECT->getShapesList().getList().end(); item++)
			{
				Shape* Cshape = (Shape*)((*item).second);
				if(Cshape->ShapeType == RapidEnums::SHAPETYPE::TRIANGULARSURFACE)
				{
					int PtsCount = this->OutOfToleranceList.size();
					std::list<SinglePoint*>::iterator itr = this->OutOfToleranceList.begin();
					for(int i = 0; i < PtsCount; i++)
					{
						double Point[3] = {(*itr)->X, (*itr)->Y, (*itr)->Z};
						bool EraseItem = false;
						for(((TriangularSurface*)Cshape)->TShapeitr = ((TriangularSurface*)Cshape)->TriangleCollection.begin(); ((TriangularSurface*)Cshape)->TShapeitr != ((TriangularSurface*)Cshape)->TriangleCollection.end(); ((TriangularSurface*)Cshape)->TShapeitr++)
						{
							double dist = ((TriangularSurface*)Cshape)->TShapeitr->point1[0] * ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[0] + ((TriangularSurface*)Cshape)->TShapeitr->point1[1] * ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[1] + ((TriangularSurface*)Cshape)->TShapeitr->point1[2] * ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[2];
							double PlaneParam[4] = {((TriangularSurface*)Cshape)->TShapeitr->dirCosine[0], ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[1], ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[2], dist};
							if(RMATH3DOBJECT->Distance_Point_Plane(Point, PlaneParam) < this->ToleranceValue)
							{
								double ProjectedPt[3] = {0};
								RMATH3DOBJECT->Projection_Point_on_Plane(Point, PlaneParam, ProjectedPt);
								if(RMATH3DOBJECT->Checking_PointIsInTriangle(ProjectedPt, ((TriangularSurface*)Cshape)->TShapeitr->point1, ((TriangularSurface*)Cshape)->TShapeitr->point2, ((TriangularSurface*)Cshape)->TShapeitr->point3))
								{
									this->WithinToleranceList.push_back(*itr);
									EraseItem = true;
									break;
								}
							}
						}
						std::list<SinglePoint*>::iterator itr1 = itr;
						itr++;
						if(EraseItem)
							this->OutOfToleranceList.erase(itr1);	
					}
				}
			}
			/*d2 = GetTickCount();
			d3 = d2 - d1;
			d3++;*/
		}
		else
		{
			MAINDllOBJECT->dontUpdateGraphcis = true;
			DWORD d1, d2, d3;
			d1 = GetTickCount();
			CalculateColorWithRespectToSurface();
			d2 = GetTickCount();
			d3 = d2 - d1;
			MAINDllOBJECT->dontUpdateGraphcis = false;
			MAINDllOBJECT->Shape_Updated(false);
			MAINDllOBJECT->Wait_VideoGraphicsUpdate();
			MAINDllOBJECT->Wait_RcadGraphicsUpdate();
			MAINDllOBJECT->update_VideoGraphics(true);
			MAINDllOBJECT->update_RcadGraphics(true);

			//Trial to see eigenvectors and eigenangles for a given set of points

		}
		if(MAINDllOBJECT->ShowPictorialView() && !MAINDllOBJECT->TriangulateCpSurface())
		{
			MAINDllOBJECT->CalculateCloudPtZlevelExtent();
		}
		if(this->ShapeAs3DShape())
			CalculateZLevel();
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("CP0002a", __FILE__, __FUNCSIG__); }
}

 bool CloudPoints::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		int n = this->PointsList->Pointscount();
		double *TransformePpoint = NULL, *tempPoint = NULL;
		TransformePpoint = new double[(n + 1) * 2];
		tempPoint = new double[n * 4];
		int cnt = 0;
		for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			tempPoint[cnt++] = Spt->X;
			tempPoint[cnt++] = Spt->Y;
			tempPoint[cnt++] = Spt->Z;
			tempPoint[cnt++] = 1;
		}	
		 for(int i = 0; i < n; i++)
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &tempPoint[4 * i], &s2[0], &TransformePpoint[2 * i]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		delete [] tempPoint;
		return true;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CP0002a", __FILE__, __FUNCSIG__); return false;
	}
}

void CloudPoints::UpdateForParentChange(BaseItem* sender)
{
}

void CloudPoints::Transform(double* transform)
{
	try
	{
		if(this->SelectedPtsOnly())
		{
			for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				if(Spt->IsSelected)
				{
					Vector temp = MAINDllOBJECT->TransformMultiply(transform, Spt->X, Spt->Y, Spt->Z);
					SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
					Spt1->SetValues(temp.getX(), temp.getY(), temp.getZ(), Spt->R, Spt->Pdir);
				}
			 }
		}
		else
		{
			for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				Vector temp = MAINDllOBJECT->TransformMultiply(transform, Spt->X, Spt->Y, Spt->Z);
				SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
				Spt1->SetValues(temp.getX(), temp.getY(), temp.getZ(), Spt->R, Spt->Pdir);
			 }	
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
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0002b", __FILE__, __FUNCSIG__); }
}

void CloudPoints::AlignToBasePlane(double* trnasformM)
{
}

void CloudPoints::UpdateEnclosedRectangle()
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1}, n = this->PointsList->Pointscount();	
		double *TransformePpoint = NULL;
		TransformePpoint = new double[n * 2 + 1];
		int cnt = 0;
		for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			TransformePpoint[cnt++] = Spt->X;
			TransformePpoint[cnt++] = Spt->Y;
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0},  *Temporary_point = NULL;	
			Temporary_point = new double[n * 3];
			cnt = 0;
			for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				Temporary_point[cnt++] = Spt->X;
				Temporary_point[cnt++] = Spt->Y;
				Temporary_point[cnt++] = Spt->Z;
			}
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			for(int i = 0; i < n; i++)
				RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &Temporary_point[3 * i], Order2, &TransformePpoint[2 * i]);	
			delete [] Temporary_point;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, ShapeLeftTop, ShapeRightBottom);
		delete [] TransformePpoint;
		for(int i = 0; i < 3; i++)
		{
			FgPointLeftTop[i] = ShapeLeftTop[i];
			FgPointsRightBottom[i] = ShapeRightBottom[i];
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0003a", __FILE__, __FUNCSIG__);}
}

void CloudPoints::GetShapeCenter(double *cPoint)
{
	if(!PointsList->getList().empty())
	{
		cPoint[0] = 0;
		cPoint[1] = 0;
		cPoint[2] = 0;
		for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			cPoint[0] += Spt->X;
			cPoint[1] += Spt->Y;
			cPoint[2] += Spt->Z;
		}
		cPoint[0] /= PointsList->getList().size();
		cPoint[1] /= PointsList->getList().size();
		cPoint[2] /= PointsList->getList().size();
		/*cPoint[0] = PointsList->getList().begin()->second->X;
		cPoint[1] = PointsList->getList().begin()->second->Y;
		cPoint[2] = PointsList->getList().begin()->second->Z;*/
	}
}

Shape* CloudPoints::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("CP");
		else
			myname = _T("dCP");
		CloudPoints* CShape = new CloudPoints((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		{
			CShape->CopyOriginalProperties(this);
			if(DXFEXPOSEOBJECT->AllPointsAsCloudPoint())
			{
				PointCollection& PtColl = *(((ShapeWithList*)this)->PointsList);
				for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					CShape->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
					CShape->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
				}
				CShape->UpdateShape();
			}
		}
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0003", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* CloudPoints::CreateDummyCopy()
{
	try
	{
		CloudPoints* CShape = new CloudPoints(false);
		CShape->CopyOriginalProperties(this);
		if(DXFEXPOSEOBJECT->AllPointsAsCloudPoint())
		{
			PointCollection& PtColl = *(((ShapeWithList*)this)->PointsList);
			for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
			{
				SinglePoint* Sp = (*Spt).second;
				CShape->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
				CShape->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
			}
			CShape->UpdateShape();
		}
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0004", __FILE__, __FUNCSIG__); return NULL; }
}

void CloudPoints::CopyShapeParameters(Shape* s)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0004", __FILE__, __FUNCSIG__); }

}

void CloudPoints::CopyPoints(Shape* s)
{
	try
	{
			PointCollection& PtColl = *(((ShapeWithList*)s)->PointsList);
			for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
			{
				SinglePoint* Sp = (*Spt).second;
				this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
				this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
			}
			this->UpdateShape();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0004b", __FILE__, __FUNCSIG__); }
}

void CloudPoints::CreateLines()
{
	try
	{
		if(AllPointsCollection.size() > 0)
			AllPointsCollection.clear();
		PointCollection& PtColl = *(((ShapeWithList*)this)->PointsList);
		map<int, Pt> AllPointsColl;
		map<int, Pt> AllPointsTempCollection;
		int Cnt = 0; bool flag = true; 	Pt leftmostpoint; 
		for(PC_ITER SptItem = PtColl.getList().begin(); SptItem != PtColl.getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			Pt newpt;
			newpt.x = Spt->X; newpt.y = Spt->Y;
			newpt.pt_index = Cnt;
			if(flag)
			{ 
				flag = false;
				leftmostpoint.x = newpt.x; leftmostpoint.y = newpt.y; 
				leftmostpoint.pt_index = newpt.pt_index;
			}
			else
			{
				if(newpt.x < leftmostpoint.x)
				{
					leftmostpoint.x = newpt.x; leftmostpoint.y = newpt.y; 
					leftmostpoint.pt_index = newpt.pt_index;
				}
			}
			AllPointsColl[Cnt] = newpt;
			Cnt++;
		}
		Cnt = 0;
		Pt FisrtPt;
		FisrtPt.x = leftmostpoint.x; FisrtPt.y = leftmostpoint.y;
		FisrtPt.pt_index = 0;
		int TotalCnt = AllPointsColl.size();
		AllPointsTempCollection[0] = FisrtPt;
		AllPointsCollection.push_back(FisrtPt);
		AllPointsColl.erase(leftmostpoint.pt_index);
		
		for(int i = 0; i < TotalCnt - 1; i++)
		{
			Pt tempPoint = AllPointsTempCollection[Cnt];
			Pt CurrentPt;
			double dist = 0, mindist = 0; flag = true;
			int C_index;
			for(map<int, Pt>::iterator Pitem = AllPointsColl.begin(); Pitem != AllPointsColl.end(); Pitem++)
			{
				Pt Cpt = (*Pitem).second;
				double tempdist = 0;
				tempdist += pow((tempPoint.x - Cpt.x), 2);  
				tempdist += pow((tempPoint.y - Cpt.y), 2);  
				dist = sqrt(tempdist);
				if(flag)
				{ 
					mindist = dist; flag = false;
					CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
					C_index = Cpt.pt_index;
				}
				else
				{
					if(mindist > dist)
					{
						mindist = dist;
						CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
						C_index = Cpt.pt_index;
					}
				}
			}
			Cnt++;
			CurrentPt.pt_index = Cnt;
			AllPointsTempCollection[Cnt] = CurrentPt;
			AllPointsCollection.push_back(CurrentPt);
			AllPointsColl.erase(C_index);
		}
		/*for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
		{
			SinglePoint* Sp = (*Spt).second;
			TempPointColl.Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R));
		}
		((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->lineArcSep(&TempPointColl, 1, true, 2, 0.01, M_PI/36, 3, false, false, false, false, true);*/

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0004", __FILE__, __FUNCSIG__); }
}


int CloudPoints::shapenumber = 0;
int CloudPoints::DXFshapenumber = 0;
void CloudPoints::reset()
{
	shapenumber = 0;
	DXFshapenumber = 0;
}

void CloudPoints::CalculateZLevel()
{
	try
	{
		if(this->PointsList->Pointscount() < 1) return;
		SinglePoint* Spt = (*this->PointsList->getList().begin()).second;
		this->Minpoint.set(Spt->X, Spt->Y, Spt->Z);
		this->Maxpoint.set(Spt->X, Spt->Y, Spt->Z);
		for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
		{
			Spt = (*SptItem).second;
			if(this->Minpoint.getZ() > Spt->Z)
			{
				this->Minpoint.set(Spt->X, Spt->Y, Spt->Z);
			}
			if(this->Maxpoint.getZ() < Spt->Z)
			{
				this->Maxpoint.set(Spt->X, Spt->Y, Spt->Z);
			}
		}
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("CP0004a", __FILE__, __FUNCSIG__); 
	}
}

wostream& operator<<(wostream& os, CloudPoints& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "CloudPoints" << endl;
		os << "CloudPointsForLine:" << x.CloudPointsForLine << endl;
		os << "CloudPointsType:" << x.CloudPointsType  << endl;
		os << "Minpoint:values"<< endl << (*static_cast<Vector*>(x.getMinpoint())) << endl;
		os << "Maxpoint:values"<< endl << (*static_cast<Vector*>(x.getMaxpoint())) << endl;
		os << "EndCloudPoints" << endl;
		return os;

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0005", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, CloudPoints& x )
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
			if(Tagname==L"CloudPoints")
			{
			while(Tagname!=L"EndCloudPoints")
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
						if(Tagname==L"CloudPointsForLine")
						{
							if(Val=="0")
							{
							x.CloudPointsForLine=false;
							}
							else
								x.CloudPointsForLine=true;
							x.ShapeType = RapidEnums::SHAPETYPE::CLOUDPOINTS;
						}
						else if(Tagname==L"CloudPointsType")
						{
							x.CloudPointsType = RapidEnums::CLOUDPOINTSTYPE(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"Minpoint")
						{
							is >> (*static_cast<Vector*>(x.getMinpoint()));
						}
						else if(Tagname==L"Maxpoint")
						{
							is >> (*static_cast<Vector*>(x.getMaxpoint()));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CP0006", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, CloudPoints& x)
{
	try
	{
		is >> x.CloudPointsForLine;
		x.ShapeType = RapidEnums::SHAPETYPE::CLOUDPOINTS;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CP0006", __FILE__, __FUNCSIG__); }
}

void CloudPoints::CalculateColorWithRespectToSurface()
{
	try
	{
		ClearDeviationColorMemory();
		if(IdealSurface == NULL) return;
		bool IdealSurfaceDeleted = true;
		for(RC_ITER Item_shape = MAINDllOBJECT->getShapesList().getList().begin(); Item_shape != MAINDllOBJECT->getShapesList().getList().end(); Item_shape++)
		{
			Shape* Cshape = (Shape*)(*Item_shape).second;
			if(Cshape->getId() == IdealSurface->getId())
			{
				IdealSurfaceDeleted = false;
				break;
			}
		}
		if(IdealSurfaceDeleted)
		{
			IdealSurface = NULL;
			return;
		}
		this->MaximumDistance = 0;
		if(IdealSurface->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
		{
			double CylinderParam[6] = {((Cylinder*)IdealSurface)->getCenter1()->getX(), ((Cylinder*)IdealSurface)->getCenter1()->getY(), ((Cylinder*)IdealSurface)->getCenter1()->getZ(), ((Cylinder*)IdealSurface)->dir_l(), ((Cylinder*)IdealSurface)->dir_m(), ((Cylinder*)IdealSurface)->dir_n()};
			double CylinderRadius = ((Cylinder*)IdealSurface)->Radius1();
			for(PC_ITER item = this->PointsList->getList().begin(); item != this->PointsList->getList().end(); item++)
			{
				SinglePoint* Spt = (*item).second;
				double TempPoint[3] = {Spt->X, Spt->Y, Spt->Z};
				PtWithColor* TmpPtsColor = new PtWithColor();
				TmpPtsColor->Dist = abs(RMATH3DOBJECT->Distance_Point_Line(TempPoint, CylinderParam) - CylinderRadius);
				if(this->MaximumDistance < TmpPtsColor->Dist)
					this->MaximumDistance  = TmpPtsColor->Dist;
				TmpPtsColor->X = Spt->X; TmpPtsColor->Y = Spt->Y; TmpPtsColor->Z = Spt->Z;
				PtsListWithColor.push_back(TmpPtsColor);
			}
			
		}
		else if(IdealSurface->ShapeType == RapidEnums::SHAPETYPE::CONE)
		{
			double ConeParam[6] = {((Cone*)IdealSurface)->getCenter1()->getX(), ((Cone*)IdealSurface)->getCenter1()->getY(), ((Cone*)IdealSurface)->getCenter1()->getZ(), ((Cone*)IdealSurface)->dir_l(), ((Cone*)IdealSurface)->dir_m(), ((Cone*)IdealSurface)->dir_n()};
			double ApexAngle = ((Cone*)IdealSurface)->ApexAngle(), ConeRadius = ((Cone*)IdealSurface)->Radius1();
			for(PC_ITER item = this->PointsList->getList().begin(); item != this->PointsList->getList().end(); item++)
			{
				SinglePoint* Spt = (*item).second;
				double TempPoint[3] = {Spt->X, Spt->Y, Spt->Z}, ProjectionPoint[3] = {0}, ConeAxialProjectionPoint[3] = {0}, DirRatios[3] = {0}, DirectionCosine[3] = {0};
				RMATH3DOBJECT->Projection_Point_on_Line(TempPoint, ConeParam, ProjectionPoint);
				double radius = RMATH3DOBJECT->Distance_Point_Point(TempPoint, ProjectionPoint);
				//to find point on surface of cone.....
				for(int i = 0; i < 3; i++)
					ConeAxialProjectionPoint[i] = ProjectionPoint[i] + radius * tan(ApexAngle) * ConeParam[3 + i];
				PtWithColor* TmpPtsColor = new PtWithColor();
				TmpPtsColor->Dist = RMATH3DOBJECT->Distance_Point_Point(TempPoint, ConeAxialProjectionPoint);
				if(this->MaximumDistance < TmpPtsColor->Dist)
					this->MaximumDistance  = TmpPtsColor->Dist;
				TmpPtsColor->X = Spt->X; TmpPtsColor->Y = Spt->Y; TmpPtsColor->Z = Spt->Z;
				PtsListWithColor.push_back(TmpPtsColor);
			}
		}
		else if(IdealSurface->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		{
			double SphereCenter[3] = {((Sphere*)IdealSurface)->getCenter()->getX(), ((Sphere*)IdealSurface)->getCenter()->getY(), ((Sphere*)IdealSurface)->getCenter()->getZ()};
			double SphereRadius = ((Sphere*)IdealSurface)->Radius();
			for(PC_ITER item = this->PointsList->getList().begin(); item != this->PointsList->getList().end(); item++)
			{
				SinglePoint* Spt = (*item).second;
				double TempPoint[3] = {Spt->X, Spt->Y, Spt->Z};
				PtWithColor* TmpPtsColor = new PtWithColor();
				TmpPtsColor->Dist = abs(RMATH3DOBJECT->Distance_Point_Point(TempPoint, SphereCenter) - SphereRadius);
				if(this->MaximumDistance < TmpPtsColor->Dist)
					this->MaximumDistance  = TmpPtsColor->Dist;
				TmpPtsColor->X = Spt->X; TmpPtsColor->Y = Spt->Y; TmpPtsColor->Z = Spt->Z;
				PtsListWithColor.push_back(TmpPtsColor);
			}
		}
		else if(IdealSurface->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			double PlaneParam[4] = {((Plane*)IdealSurface)->dir_a(), ((Plane*)IdealSurface)->dir_b(), ((Plane*)IdealSurface)->dir_c(), ((Plane*)IdealSurface)->dir_d()};			
			for(PC_ITER item = this->PointsList->getList().begin(); item != this->PointsList->getList().end(); item++)
			{
				SinglePoint* Spt = (*item).second;
				double TempPoint[3] = {Spt->X, Spt->Y, Spt->Z};
				PtWithColor* TmpPtsColor = new PtWithColor();
				TmpPtsColor->Dist = RMATH3DOBJECT->Distance_Point_Plane(TempPoint, PlaneParam);
				if(this->MaximumDistance < TmpPtsColor->Dist)
					this->MaximumDistance  = TmpPtsColor->Dist;
				TmpPtsColor->X = Spt->X; TmpPtsColor->Y = Spt->Y; TmpPtsColor->Z = Spt->Z;
				PtsListWithColor.push_back(TmpPtsColor);
			}
		}
		else if(IdealSurface->ShapeType == RapidEnums::SHAPETYPE::TRIANGULARSURFACE)
		{
			for(PC_ITER item = this->PointsList->getList().begin(); item != this->PointsList->getList().end(); item++)
			{
				SinglePoint* Spt = (*item).second;
				double TempPoint[3] = {Spt->X, Spt->Y, Spt->Z};
				PtWithColor* TmpPtsColor = new PtWithColor();
				TmpPtsColor->Dist = 99999;
				bool FoundPoint = false;
				for(((TriangularSurface*)IdealSurface)->TShapeitr = ((TriangularSurface*)IdealSurface)->TriangleCollection.begin(); ((TriangularSurface*)IdealSurface)->TShapeitr != ((TriangularSurface*)IdealSurface)->TriangleCollection.end(); ((TriangularSurface*)IdealSurface)->TShapeitr++)
				{
					double dist = ((TriangularSurface*)IdealSurface)->TShapeitr->point1[0] * ((TriangularSurface*)IdealSurface)->TShapeitr->dirCosine[0] + ((TriangularSurface*)IdealSurface)->TShapeitr->point1[1] * ((TriangularSurface*)IdealSurface)->TShapeitr->dirCosine[1] + ((TriangularSurface*)IdealSurface)->TShapeitr->point1[2] * ((TriangularSurface*)IdealSurface)->TShapeitr->dirCosine[2];
					double PlaneParam[4] = {((TriangularSurface*)IdealSurface)->TShapeitr->dirCosine[0], ((TriangularSurface*)IdealSurface)->TShapeitr->dirCosine[1], ((TriangularSurface*)IdealSurface)->TShapeitr->dirCosine[2], dist};
					double ProjectedPt[3] = {0};
					RMATH3DOBJECT->Projection_Point_on_Plane(TempPoint, PlaneParam, ProjectedPt);
					if(RMATH3DOBJECT->Checking_PointIsInTriangle(ProjectedPt, ((TriangularSurface*)IdealSurface)->TShapeitr->point1, ((TriangularSurface*)IdealSurface)->TShapeitr->point2, ((TriangularSurface*)IdealSurface)->TShapeitr->point3))
					{
						double TempDist = RMATH3DOBJECT->Distance_Point_Plane(TempPoint, PlaneParam);
						if(TmpPtsColor->Dist > TempDist)
						{
							FoundPoint = true;
							TmpPtsColor->Dist = TempDist;
							TmpPtsColor->X = Spt->X; TmpPtsColor->Y = Spt->Y; TmpPtsColor->Z = Spt->Z;
							if(this->MaximumDistance < TmpPtsColor->Dist)
								this->MaximumDistance = TmpPtsColor->Dist;
						}
					}
				}
				if(FoundPoint)
					PtsListWithColor.push_back(TmpPtsColor);
				else
					delete TmpPtsColor;
			}
		}
		if(PtsListWithColor.size() > 0)
		{
			UpdateColorInfo();
		}
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("CP0007", __FILE__, __FUNCSIG__); }
}

void CloudPoints::UpdateColorInfo()
{
	try
	{
		if(this->MaximumDistance == 0) return;
		double Z1 = this->MaximumDistance / 3;
		double Z2 = 2 * Z1;
		for each(PtWithColor* TmpPtsColor in PtsListWithColor)
		{
			double colorFactor = TmpPtsColor->Dist;
			if(colorFactor <= Z1)
			{
				TmpPtsColor->R = 0; TmpPtsColor->G = 1; TmpPtsColor->B = 0;
			}
			else if(colorFactor <= Z2)
			{
				TmpPtsColor->R = (Z2 - colorFactor) / Z2; TmpPtsColor->G = ((colorFactor - Z1) / Z1); TmpPtsColor->B = 0;
			}
			else
			{
				TmpPtsColor->R = 1; TmpPtsColor->G = 0; TmpPtsColor->B = 0;
			}
		}

		/*double Z2 = 2 * Z1, Z3 = 3 * Z1, Z4 = 4 * Z1, Z5 = 5 * Z1;
		for each(PtWithColor* TmpPtsColor in PtsListWithColor)
		{
			double colorFactor = TmpPtsColor->Dist;
			if(colorFactor <= Z1)
			{
				TmpPtsColor->R = 0; TmpPtsColor->G = 0; TmpPtsColor->B = 1;
			}
			else if(colorFactor <= Z2)
			{
				TmpPtsColor->R = 0; TmpPtsColor->G = ((colorFactor - Z1) / Z1) * 0.25; TmpPtsColor->B = (1 - 0.25) * ((colorFactor - Z1) / Z1);
			}
			else if(colorFactor <= Z3)
			{
				TmpPtsColor->R = 0; TmpPtsColor->G = 0.25 + ((colorFactor - Z2) / Z1) * 0.5; TmpPtsColor->B = 0.75 - (1 - 0.5) * ((colorFactor - Z2) / Z1);
			}
			else if(colorFactor <= Z4)
			{
				TmpPtsColor->R = ((colorFactor - Z3) / Z1) * 0.25; TmpPtsColor->G = 0.75; TmpPtsColor->B = 0.25 - (1 - 07.5) * ((colorFactor - Z3) / Z1);
			}
			else if(colorFactor <= Z5)
			{
				TmpPtsColor->R = 0.25 + ((colorFactor - Z4) / Z1) * 0.5; TmpPtsColor->G = 0.75 - (1 - 0.5) * ((colorFactor - Z4) / Z1); TmpPtsColor->B = 0;
			}
			else
			{
				TmpPtsColor->R = 1; TmpPtsColor->G = 0; TmpPtsColor->B = 0;
			}
		}*/
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CP0008", __FILE__, __FUNCSIG__);
	}
}

void CloudPoints::UpdatePointsColorWRTDeviation()
{
	try
	{
		this->IdealSurface = NULL;
		for(RC_ITER Item_shape = MAINDllOBJECT->getShapesList().getList().begin(); Item_shape != MAINDllOBJECT->getShapesList().getList().end(); Item_shape++)
		{
			Shape* Cshape = (Shape*)(*Item_shape).second;
			if(!Cshape->Normalshape()) continue;
			if(Cshape->selected() && (Cshape->ShapeType == RapidEnums::SHAPETYPE::CYLINDER || Cshape->ShapeType == RapidEnums::SHAPETYPE::CONE || Cshape->ShapeType == RapidEnums::SHAPETYPE::SPHERE || Cshape->ShapeType == RapidEnums::SHAPETYPE::PLANE || Cshape->ShapeType == RapidEnums::SHAPETYPE::TRIANGULARSURFACE))
			{
				this->IdealSurface = (ShapeWithList*)Cshape;
				break;
			}
		}
		this->UpdateBestFit();
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CP0009", __FILE__, __FUNCSIG__);
	}
}

void CloudPoints::ClearDeviationColorMemory()
{
	try
	{
		while(PtsListWithColor.size() != 0)
		{
			std::list<PtWithColor*>::iterator itr1 = PtsListWithColor.begin();
			PtWithColor* PointValue = (*itr1);
			PtsListWithColor.erase(itr1);
			delete PointValue;
		}
		PtsListWithColor.clear();
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CP0010", __FILE__, __FUNCSIG__);
	}
}