#include "StdAfx.h"
#include "AutoFocusScanHandler.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\FocusFunctions.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Shapes\AutoScanSurface.h"
#include "..\Helper\Helper.h"
//Default handler .. do nothing.....!!!!

AutoFocusScanHandler::AutoFocusScanHandler()
{
	curSurfacePntsCnt = 0;
	setMaxClicks(2);
	CurrentDir = DirList::ALONGYPOS;
	FRow = FCol = FGap = FWidth = FHeight = 0;
	CurrentCell = 0;
	MaxZ = MinZ = 0;
	conti = true;
	PointOfRotation = NULL;
	MAINDllOBJECT->ShowAutoFocusWindow(true);
	rotateOnMouseMove = false;
}

AutoFocusScanHandler::~AutoFocusScanHandler()
{
	MAINDllOBJECT->ShowAutoFocusWindow(false);
	ClearTmpListptr();
	ClearAll();
}

void AutoFocusScanHandler::LmouseDown()
{
	try
	{
		if(rotateOnMouseMove)
		{
			rotateOnMouseMove = false;
			for each(AutoScanSurface *curs in tmpSurfaceCollection)
			{
				if(BESTFITOBJECT->TriangulateSurface(curs->SelectedPoints, curs->SelectedPointsCnt, &curs->TriangleList,  &curs->TriangleCnt, false))
				{
					SurfaceCollection.push_back(curs);
					curs->UpdateEnclosedRectangle();
					MAINDllOBJECT->getUCS(MAINDllOBJECT->getCurrentUCS().getId())->getShapes().addItem(curs);
					if(curs->selected())
						MAINDllOBJECT->getSelectedShapesList().addItem(curs, false);
					MAINDllOBJECT->getShapesList().notifySelection();				
				}
			}
			tmpSurfaceCollection.clear();
			MAINDllOBJECT->Shape_Updated();
			setClicksDone(0);
			return;
		}
		if(MAINDllOBJECT->getCurrentWindow() == 1 && curSurfacePntsCnt == 0)
		{
			  if(MAINDllOBJECT->highlightedPoint() != NULL)
			  {
				  PointOfRotation = new Vector(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
				  for each(AutoScanSurface *curs in SurfaceCollection)
				  {
					  if(!curs->selected()) continue;
					  AutoScanSurface *tmp = new AutoScanSurface();
					  tmp->CopyShapeParameters(curs);
					  if(BESTFITOBJECT->TriangulateSurface(tmp->SelectedPoints, tmp->SelectedPointsCnt, &tmp->TriangleList,  &tmp->TriangleCnt, false))
					  {
						 tmpSurfaceCollection.push_back(tmp);
					  }
					  else
					  {
						delete tmp;
					  }
					  rotateOnMouseMove = true;
				  }			  
			  }
			  setClicksDone(0);
			  return;
		}
		
		currentSurfacePnts[3 * curSurfacePntsCnt] = getClicksValue(getClicksDone() - 1).getX();
		currentSurfacePnts[3 * curSurfacePntsCnt + 1] = getClicksValue(getClicksDone() - 1).getY();
		currentSurfacePnts[3 * curSurfacePntsCnt + 2] = MAINDllOBJECT->getCurrentDRO().getZ();
		if(curSurfacePntsCnt > 1)
		{
			if(MaxZ < currentSurfacePnts[3 * curSurfacePntsCnt + 2])
				MaxZ = currentSurfacePnts[3 * curSurfacePntsCnt + 2];
			else if(MinZ > currentSurfacePnts[3 * curSurfacePntsCnt + 2])
				MinZ = currentSurfacePnts[3 * curSurfacePntsCnt + 2];
		}
		else if(curSurfacePntsCnt == 1)
		{ 
			if(currentSurfacePnts[2] > currentSurfacePnts[5])
			{
				MaxZ = currentSurfacePnts[2];
				MinZ = currentSurfacePnts[5];
			}
			else
			{
				MaxZ = currentSurfacePnts[5];
				MinZ = currentSurfacePnts[2];
			}
		}
		setClicksDone(0);
		curSurfacePntsCnt++;
		MAINDllOBJECT->ZoomToWindowExtents(1);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC001", __FILE__, __FUNCSIG__); 
	}
}

void AutoFocusScanHandler::RmouseDown(double x, double y)
{
	try
	{
		if(curSurfacePntsCnt < 3) return;
		AutoScanSurface *curSurface = NULL;
		curSurface = new AutoScanSurface(currentSurfacePnts);
		if(BESTFITOBJECT->TriangulateSurface(curSurface->SelectedPoints, curSurfacePntsCnt, &curSurface->TriangleList,  &curSurface->TriangleCnt, false))
		{
			SurfaceCollection.push_back(curSurface);
			curSurface->UpdateEnclosedRectangle();
			MAINDllOBJECT->getUCS(MAINDllOBJECT->getCurrentUCS().getId())->getShapes().addItem(curSurface);
			if(curSurface->selected())
				MAINDllOBJECT->getSelectedShapesList().addItem(curSurface, false);
			MAINDllOBJECT->Shape_Updated();	
		}
		else
		{
			delete curSurface;
			curSurface = NULL;
			MAINDllOBJECT->SetStatusCode("AutoFocusScanHandler01");
		}
		ClearTmpListptr();
		MAINDllOBJECT->ZoomToWindowExtents(1);
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC001b", __FILE__, __FUNCSIG__); 
	}
}

void AutoFocusScanHandler::mouseMove()
{
	try
	{
		if(rotateOnMouseMove)
		{
			double currentShapeCenter[3] = {0};
			GetSelectedShapeCenter(currentShapeCenter);
			currentShapeCenter[2] = PointOfRotation->getZ();
			double newMatrix[9] = {0};
			CalculateMatrixesForRotation(currentShapeCenter, newMatrix);
			for each(AutoScanSurface *curs in tmpSurfaceCollection)
			{
				curs->Translate(Vector(-PointOfRotation->getX(), -PointOfRotation->getY(), -PointOfRotation->getZ()));
				curs->Transform(newMatrix);
				curs->Translate(Vector(PointOfRotation->getX(), PointOfRotation->getY(), PointOfRotation->getZ()));
				curs->UpdateEnclosedRectangle();
			}
			MAINDllOBJECT->Shape_Updated();	
		}
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC001c", __FILE__, __FUNCSIG__);
	}
}	

void AutoFocusScanHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
			if(rotateOnMouseMove)
			{
				  for each(AutoScanSurface *curs in tmpSurfaceCollection)
				  {
					  for(int i = 0; i < curs->TriangleCnt; i++)
					  {
						 GRAFIX->draw_PlanePolygon(&curs->TriangleList[9 * i], 3, true, MAINDllOBJECT->DrawWiredSurface());
					  }
				  }
			}
			else
			{
				double *tmpPnts = NULL;
				tmpPnts = new double[3 * curSurfacePntsCnt];
				for(int i = 0; i < curSurfacePntsCnt - 1; i++)
				{
					GRAFIX->drawLine_3D(currentSurfacePnts[3 * i], currentSurfacePnts[3 * i + 1], currentSurfacePnts[3 * i + 2], currentSurfacePnts[3 * (i + 1) ], currentSurfacePnts[3 * (i + 1) + 1], currentSurfacePnts[3 * (i + 1) + 2]);
					tmpPnts[3 * i] = currentSurfacePnts[3 * i];
					tmpPnts[3 * i + 1] = currentSurfacePnts[3 * i + 1];
					tmpPnts[3 * i + 2] = currentSurfacePnts[3 * i + 2];
				}
				if(curSurfacePntsCnt > 0)
				{
					if(windowno == 0)
					{
						Vector* pt = &getClicksValue(getClicksDone());
						GRAFIX->drawLine_3D(pt->getX(), pt->getY(), pt->getZ(), currentSurfacePnts[3 * (curSurfacePntsCnt - 1)], currentSurfacePnts[3 * (curSurfacePntsCnt - 1) + 1], currentSurfacePnts[3 * (curSurfacePntsCnt - 1) + 2]);
					}
					tmpPnts[3 * (curSurfacePntsCnt - 1)] = currentSurfacePnts[3 * (curSurfacePntsCnt - 1)];
					tmpPnts[3 * (curSurfacePntsCnt - 1) + 1] = currentSurfacePnts[3 * (curSurfacePntsCnt - 1) + 1];
					tmpPnts[3 * (curSurfacePntsCnt - 1) + 2] = currentSurfacePnts[3 * (curSurfacePntsCnt - 1) + 2];
				}
				if(abs(MaxZ - MinZ) < 0.0001)
					GRAFIX->drawPoints(tmpPnts, curSurfacePntsCnt, false);
				else
					GRAFIX->drawPoints(tmpPnts, curSurfacePntsCnt, MinZ, MaxZ, false);
				delete [] tmpPnts;
			}
	
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC002", __FILE__, __FUNCSIG__); 
	}
}

void AutoFocusScanHandler::LmaxmouseDown()
{
	   setClicksDone(0);
	   return;
}

void AutoFocusScanHandler::EscapebuttonPress()
{
	ClearTmpListptr();
}

bool AutoFocusScanHandler::StartAutoFocus()
{
	try
	{
		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) || (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)) return false;
		if(SurfaceCollection.empty()) return false;
		FRow = FOCUSCALCOBJECT->focusRectangleRow;
		FCol = FOCUSCALCOBJECT->focusRectangleColumn;
		FGap = FOCUSCALCOBJECT->focusRectangleGap;// *HELPEROBJECT->CamSizeRatio;
		FWidth = FOCUSCALCOBJECT->focusRectangleWidth;// *HELPEROBJECT->CamSizeRatio;
		FHeight = FOCUSCALCOBJECT->focusRectangleHeight;// *HELPEROBJECT->CamSizeRatio;
		double upp = MAINDllOBJECT->getWindow(0).getUppX();// / HELPEROBJECT->CamSizeRatio;
		double wwidth = (FCol * FGap) * upp;
		double wheight = (FRow * FGap) * upp;
		for(list<AutoScanSurface*>::iterator It = SurfaceCollection.begin(); It != SurfaceCollection.end(); It++)
			(*It)->CreateSurface(wwidth, wheight);
		SurfaceIter = SurfaceCollection.begin();
		TotalCell = (*SurfaceIter)->NumberOfRows * (*SurfaceIter)->NumberOfCols;
		ClearTmpListptr();
		while(CurrentCell < TotalCell)
		{
			if((*SurfaceIter)->SurfacePoints[3 * CurrentCell + 2] != -1000)
				break;
			CurrentCell++;
		}
		if((*SurfaceIter)->NumberOfRows == 1) 
			CurrentDir = DirList::ALONGX;
		else if(CurrentCell % ((*SurfaceIter)->NumberOfRows - 1) == 0)
			CurrentDir = DirList::ALONGX;
		else if((*SurfaceIter)->NumberOfCols == 1) 
			CurrentDir = DirList::ALONGYPOS;
		else if(((*SurfaceIter)->NumberOfRows > 1) && (CurrentCell / ((*SurfaceIter)->NumberOfRows - 1)) % 2 == 0)
			CurrentDir = DirList::ALONGYPOS;
		else 
			CurrentDir = DirList::ALONGYNEG;

		currentshape = new CloudPoints();
		AddShapeAction::addShape(currentshape);
		FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::AUTOFOCUSSCANMODE;
		FOCUSCALCOBJECT->FocusOnStatus(true);
		FOCUSCALCOBJECT->setRowColumn(FRow, FCol, FGap, FWidth, FHeight);
		FOCUSCALCOBJECT->AFSGotoPosition.SetValues((*SurfaceIter)->SurfacePoints[3 * CurrentCell], (*SurfaceIter)->SurfacePoints[3 * CurrentCell + 1], (*SurfaceIter)->SurfacePoints[3 * CurrentCell + 2]);
		FOCUSCALCOBJECT->GotoFocusScanPosition();
		return true;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC003", __FILE__, __FUNCSIG__); 
	}
}

void AutoFocusScanHandler::ContinueAutoFocus()
{
	CalculateNextGotoPosition(true);
}

void AutoFocusScanHandler::UpdateAutoFocusScanPoints(double* Zvalues)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
		int count1 = 0 , count2 = 0, cellsWithInSpan = 0, cells = FRow * FCol;
		double FocusStartZvalue = FOCUSCALCOBJECT->FocusStartPosition.getZ();
		double FocusSpanPos = FocusStartZvalue + FOCUSCALCOBJECT->fspan;
		double FocusSpanNeg = FocusStartZvalue - FOCUSCALCOBJECT->fspan;
		double *FocusXYZWithInSpan = new double[cells * 3];
		double ZDevAvg = 0, PlaneParam[5] = {0};
		int *tmpRectPtr = FOCUSCALCOBJECT->Rectptr;
		AvgZ = 0, DirZ = 0;
		PointCollection TempPtCollection;
		for(int i = 0; i < cells; i++)
		{
			if((Zvalues[i] <= FocusSpanPos) && (Zvalues[i] >= FocusSpanNeg))
			{
				double tmpPnt[3] = {cx + (tmpRectPtr[4 * i] + (tmpRectPtr[4 * i + 2]/2)) * upp, cy - (tmpRectPtr[4 * i + 1] + (tmpRectPtr[4 * i + 3]/2)) * upp, -1000};
				BESTFITOBJECT->getZcoordInTriangleList(tmpPnt, 1, (*SurfaceIter)->TriangleList, (*SurfaceIter)->TriangleCnt, 0.4, 0);
				if(tmpPnt[2] == -1000) continue;
				if(FOCUSCALCOBJECT->ApplyFocusCorrection())
				{
					double correction = 0;
					if(BESTFITOBJECT->getZError(FOCUSCALCOBJECT->CalCol(), FOCUSCALCOBJECT->CalRow(), FOCUSCALCOBJECT->CalWidth(), FOCUSCALCOBJECT->FocusErrCorrectionValue, 
						FOCUSCALCOBJECT->FocusCalibError, tmpRectPtr[4 * i] + FOCUSCALCOBJECT->focusRectangleWidth * HELPEROBJECT->CamSizeRatio / 2, 
						tmpRectPtr[4 * i + 1] + FOCUSCALCOBJECT->focusRectangleHeight * HELPEROBJECT->CamSizeRatio / 2, &correction))
						Zvalues[i] -= correction;		  
				}
				FocusXYZWithInSpan[3 * cellsWithInSpan] = tmpPnt[0];
				FocusXYZWithInSpan[3 * cellsWithInSpan + 1] = tmpPnt[1];
				FocusXYZWithInSpan[3 * cellsWithInSpan + 2] = Zvalues[i];
				cellsWithInSpan++;
				switch(CurrentDir)
				{
					case DirList::ALONGX:
					{
						if((i % FCol) == FCol - 1)
						{
							if(Zvalues[i - int(FCol/2)] < 200)
							{
								AvgZ += Zvalues[i - int(FCol/2)];
								count1++;
							}
							if(Zvalues[i] < 200)
							{
								DirZ += Zvalues[i];
								count2++;
							}
						}
					}
					case DirList::ALONGYPOS:
					{
						if(i < FCol)
						{
							if(Zvalues[FCol * int(FRow/2) + i] < 200)
							{
								AvgZ += Zvalues[FCol * int(FRow/2) + i];
								count1++;
							}
							if(Zvalues[i] < 200)
							{
								DirZ += Zvalues[i];
								count2++;
							}
						}
				   }
				   case DirList::ALONGYNEG:
				   {
					   if(i >= (cells - FCol - 1) && i < cells)
					   {
							 if(Zvalues[i - (FCol * (int(FRow/2) - 1))] < 200)
							 {
								 AvgZ += Zvalues[i - (FCol * (int(FRow/2) - 1))];
								 count1++;
							 }
							 if(Zvalues[i] < 200)
							 {
								 DirZ += Zvalues[i];
								 count2++;
							 }
					   }
					}	
				}
			}
		}

		if(count1 > 0)
			AvgZ /= count1;
		if(count2 > 0)
			DirZ /= count2;
		else
			DirZ = AvgZ;

		if(cellsWithInSpan > 3)
		   FOCUSCALCOBJECT->FilterFocus(FocusXYZWithInSpan, cellsWithInSpan, FocusSpanPos, FocusSpanNeg, PlaneParam,  &ZDevAvg);	
		for(int i = 0; i < cellsWithInSpan; i++)
		{
			if(cellsWithInSpan < 4 || (abs(RMATH3DOBJECT->Distance_Point_Plane(&FocusXYZWithInSpan[3 * i], &PlaneParam[0])) <= (FOCUSCALCOBJECT->NoiseFilterFactor() * ZDevAvg)))
				TempPtCollection.Addpoint(new SinglePoint(FocusXYZWithInSpan[3 * i], FocusXYZWithInSpan[3 * i + 1], FocusXYZWithInSpan[3 * i + 2] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ()));
		}
		currentshape->AddPoints(&TempPtCollection);
		delete [] FocusXYZWithInSpan;
		MAINDllOBJECT->ZoomToWindowExtents(1);
		CalculateNextGotoPosition();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC004", __FILE__, __FUNCSIG__); }
}

void AutoFocusScanHandler::RotateAutoFocusScanSurfaces(double rotationAngle, int step)
{
	try
	{
		if (PointOfRotation == NULL || rotateOnMouseMove)
		{
			return;
		}
		else
		{
			 double angleofrotation = rotationAngle *  M_PI/180;
			 numberOfRotation = step;
			 double dir[3] = {0, 0, 1}, rotateMatrix[9] = {0};
			 int s1[2] = {3,3}, s2[2] = {3,1};
			 list<AutoScanSurface*> tmpSurfaceCollection;
			 for( int i = 1; i <= numberOfRotation; i++)
			 {
				 RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(i * angleofrotation, dir, rotateMatrix);
				 for each(AutoScanSurface *sur in SurfaceCollection)
				 {
					  map<int, double> tmpsurfacePnts;
					  for(int i = 0; i < sur->SelectedPointsCnt; i++)
					  {
						 double tmp[3] = {sur->SelectedPoints[3 * i], sur->SelectedPoints[3 * i + 1], sur->SelectedPoints[3 * i + 2]}, tmp2[3] = {0}, origin[3] = {PointOfRotation->getX(), PointOfRotation->getY(), PointOfRotation->getZ()};
						 for(int j = 0; j < 3; j++) { tmp[j] -= origin[j];}
						 RMATH3DOBJECT->MultiplyMatrix1(rotateMatrix, s1, tmp, s2, tmp2);
						 for(int j = 0; j < 3; j++) { tmp2[j] += origin[j];}
						 tmpsurfacePnts[3*i] = tmp2[0];
						 tmpsurfacePnts[3*i + 1] = tmp2[1];
						 tmpsurfacePnts[3*i + 2] = tmp2[2];
					  }
					  AutoScanSurface *curSurface = NULL;
					  curSurface = new AutoScanSurface(tmpsurfacePnts);
					  if(BESTFITOBJECT->TriangulateSurface(curSurface->SelectedPoints, curSurface->SelectedPointsCnt, &curSurface->TriangleList,  &curSurface->TriangleCnt, false))
					  {
						 tmpSurfaceCollection.push_back(curSurface);
					  }
					  else
					  {
						 delete curSurface;
						 curSurface = NULL;
					  }
				   }
				}
				for each(AutoScanSurface *sur in tmpSurfaceCollection)
				{
					SurfaceCollection.push_back(sur);
					sur->UpdateEnclosedRectangle();
					MAINDllOBJECT->getUCS(MAINDllOBJECT->getCurrentUCS().getId())->getShapes().addItem(sur);
					if(sur->selected())
						MAINDllOBJECT->getSelectedShapesList().addItem(sur, false);
					MAINDllOBJECT->getShapesList().notifySelection();
					MAINDllOBJECT->Shape_Updated();
				}
				tmpSurfaceCollection.clear();
		}
	}
	catch(...)
	{
		 MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC003b", __FILE__, __FUNCSIG__);
	}
}

void AutoFocusScanHandler::CalculateNextGotoPosition(bool currentPlace)
{
	try
	{
		if(!currentPlace)
		{
			if(CurrentCell >= TotalCell - 1)
			{
				SurfaceIter++;
				if(SurfaceIter == SurfaceCollection.end())
				{
					MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthAFSGotoNextPosition01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					return;
				}
				CurrentCell = -1;
				TotalCell = (*SurfaceIter)->NumberOfRows * (*SurfaceIter)->NumberOfCols;
				conti = true;
			}
			CurrentCell++;
		}
		while(CurrentCell < TotalCell)
		{
			if((*SurfaceIter)->SurfacePoints[3 * CurrentCell + 2] != -1000)
				break;
			conti = false;
			CurrentCell++;
		}

		if(CurrentCell == TotalCell)
		{
			conti = false;
			CalculateNextGotoPosition();
			return;
		}
		if((*SurfaceIter)->NumberOfRows == 1)
		   CurrentDir = DirList::ALONGYPOS;
		else if(CurrentCell % ((*SurfaceIter)->NumberOfRows - 1 ) == 0)
		   CurrentDir = DirList::ALONGX;
		else if((CurrentCell / ((*SurfaceIter)->NumberOfRows - 1)) % 2 == 0)
		   CurrentDir = DirList::ALONGYPOS;
		else
		   CurrentDir = DirList::ALONGYNEG;

		double spt[3] = {(*SurfaceIter)->SurfacePoints[3 * CurrentCell], (*SurfaceIter)->SurfacePoints[3 * CurrentCell + 1], (*SurfaceIter)->SurfacePoints[3 * CurrentCell + 2]};
		double upp = MAINDllOBJECT->getWindow(0).getUppX(); // / HELPEROBJECT->CamSizeRatio;
		double halfWidth =  ((FWidth + (FCol - 1) * FGap) * upp) / 2;
		double halfHeight = ((FHeight + (FRow - 1) * FGap) * upp) / 2;

		double cornerpoint[12] = {spt[0] - halfWidth, spt[1] + halfHeight, -1000, spt[0] + halfWidth, spt[1] + halfHeight, -1000,
						spt[0] + halfWidth, spt[1] - halfHeight, -1000, spt[0] - halfWidth, spt[1] - halfHeight, -1000};

		BESTFITOBJECT->getZcoordInTriangleList(cornerpoint, 4, (*SurfaceIter)->TriangleList, (*SurfaceIter)->TriangleCnt);
		int cnt = 0;
		double diff = 1, span = 0, zarray[4] = {0};
		for(int i = 0; i < 4; i++)
		{
			if(cornerpoint[3 * i + 2] != -1000)
			  zarray[cnt++] = cornerpoint[3 * i + 2];
		}
		if(cnt > 1)
		{
			double maxZ = RMATH2DOBJECT->maximum(zarray, cnt);
			double minZ = RMATH2DOBJECT->minimum(zarray, cnt);
			diff = abs(maxZ - minZ);
			if(diff > 4) diff = 4;
		}
	/*	if(conti && AvgZ != 0)
		{
			FOCUSCALCOBJECT->AFSGotoPosition.SetValues(spt[0], spt[1], AvgZ + 2 * (DirZ - AvgZ));
			span = diff + 1.5;
		}
		else*/
		{
			if(currentPlace)
				FOCUSCALCOBJECT->AFSGotoPosition.SetValues(spt[0], spt[1], MAINDllOBJECT->getCurrentDRO().getZ());
			else
				FOCUSCALCOBJECT->AFSGotoPosition.SetValues(spt[0], spt[1], spt[2]);
			span = diff + 2;
		}
		conti = true;
		FOCUSCALCOBJECT->changeFocusSpan(span);
		FOCUSCALCOBJECT->GotoFocusScanPosition();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC002", __FILE__, __FUNCSIG__); }
}

bool AutoFocusScanHandler::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		if(curSurfacePntsCnt == 0) return false;
		int s1[2] = {4, 4}, s2[2] = {4, 1}, size = 0;
		double *TransformePpoint = NULL;
		TransformePpoint = new double[(curSurfacePntsCnt + 1) * 2];
		for(int i = 0; i < curSurfacePntsCnt; i++)
		{
			double tempPoint[4] = {0};
			tempPoint[0] = currentSurfacePnts[3 * i];
			tempPoint[1] = currentSurfacePnts[3 * i + 1];
			tempPoint[2] = currentSurfacePnts[3 * i + 2];
			tempPoint[3] = 1;
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], tempPoint, &s2[0], &TransformePpoint[2 * size]);
			size++;		
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(TransformePpoint, size, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		return true;
	}
	catch(...)
	{
		return false;
	}
}

bool AutoFocusScanHandler::GetEnclosedRectanglefor2dMode(double *Lefttop, double *Rightbottom)
{
	try
	{
		if(curSurfacePntsCnt == 0) return false;
		int cnt = 0;
		double *extentPointCol = NULL;
		extentPointCol = new double[curSurfacePntsCnt * 2];
		for(int i = 0; i < curSurfacePntsCnt; i++)
		{
			extentPointCol[cnt++] = currentSurfacePnts[3 * i];
			extentPointCol[cnt++] = currentSurfacePnts[3 * i + 1];
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(extentPointCol, cnt / 2, 2, Lefttop, Rightbottom);
		delete [] extentPointCol;
		return true;
	}
	catch(...)
	{
		return false;
	}
}

void AutoFocusScanHandler::ExportSurfacePoints(char* filename)
{
	std::wofstream myfile;
	myfile.open(filename);
	if(!myfile.is_open()) return;
	for(list<AutoScanSurface*>::iterator It = SurfaceCollection.begin(); It != SurfaceCollection.end(); It++)
	{
	   myfile << "Surface" << endl;
	   myfile << (*It)->SelectedPointsCnt << endl;
	   for(int i = 0; i < (*It)->SelectedPointsCnt; i++)
	   {
		  myfile << (*It)->SelectedPoints[i] << endl;
	   }
	}
	myfile << "End" << endl;
	myfile.close();
}

void AutoFocusScanHandler::ImportSurfacePoint(char* filename)
{
	std::wifstream myfile;
	myfile.open(filename);
	if(!myfile.is_open()) return;
	ClearTmpListptr();
	ClearAll();
	wstring flagStr;
	myfile >> flagStr;
	std::string Str = RMATH2DOBJECT->WStringToString(flagStr);
	while(Str != "End")
	{
		if(Str == "Surface")
		{
			myfile >> curSurfacePntsCnt;
			for(int i = 0; i < curSurfacePntsCnt; i++)
			{
			  myfile >> currentSurfacePnts[3 * i];
			  myfile >> currentSurfacePnts[3 * i + 1];
			  myfile >> currentSurfacePnts[3 * i + 2];
			}
			SurfaceCollection.push_back(new AutoScanSurface(currentSurfacePnts));
			ClearTmpListptr();
			myfile >> flagStr;
			Str = RMATH2DOBJECT->WStringToString(flagStr);
		}
		else
			break;
	}
	myfile.close();
}

void AutoFocusScanHandler::ClearTmpListptr()
{
	 currentSurfacePnts.clear();
	 curSurfacePntsCnt = 0;
	 CurrentCell = 0;
	 for(list<AutoScanSurface*>::iterator it = tmpSurfaceCollection.begin(); it != tmpSurfaceCollection.end(); it++)
	 {
		MAINDllOBJECT->getUCS(MAINDllOBJECT->getCurrentUCS().getId())->getShapes().removeItem((*it), false);
		delete *it;
	 }
	 tmpSurfaceCollection.clear();
	 rotateOnMouseMove = false;
}

void AutoFocusScanHandler::ClearAll()
{
	try
	{
		drawSurface = false;
		for(list<AutoScanSurface*>::iterator it = SurfaceCollection.begin(); it != SurfaceCollection.end(); it++)
		{
			MAINDllOBJECT->getUCS(MAINDllOBJECT->getCurrentUCS().getId())->getShapes().removeItem((*it), false);
			delete *it;
		}
		SurfaceCollection.clear();
	}
	catch(...)
	{	
	}
}

bool AutoFocusScanHandler::GetSelectedShapeCenter(double *currentCenter)
{
	int i = 0;
	double tmpCenter[3] = {0};
	for(list<AutoScanSurface*>::iterator It = tmpSurfaceCollection.begin(); It != tmpSurfaceCollection.end(); It++)
	{
		AutoScanSurface *curSurface = *It;
		curSurface->GetShapeCenter(tmpCenter);
		for(int k = 0; k < 3; k++) {currentCenter[k] += tmpCenter[k];}
		i++;
	}	
	if(i > 0)
	{
		for(int j = 0; j < 3; j++) {currentCenter[j] /= i;}
		return true;
	}
	return false;
}

bool AutoFocusScanHandler::CalculateMatrixesForRotation(double *currentShapeCenter, double *rotateMatrix)
{
		int s1[2] = {3,3}, s2[2] = {3,3}, s3[2] = {3,1};
		double mSelectionLine[6] = {0}, targetPosition[3] = {0}, targetDir[3] = {0}, currentDir[3] = {0}; 
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(mSelectionLine);
		double RotationPlane[4] = {0, 0, 1, PointOfRotation->getZ()}, RotationOrigin[3] = {PointOfRotation->getX(), PointOfRotation->getY(), PointOfRotation->getZ()};
		RMATH3DOBJECT->Intersection_Line_Plane(mSelectionLine, RotationPlane, targetPosition);
		for(int i = 0; i < 3; i++)
		{
		   targetDir[i] = targetPosition[i] - RotationOrigin[i];
		   currentDir[i] = currentShapeCenter[i] - RotationOrigin[i];
		}
		double Angle = RMATH3DOBJECT->Angle_Btwn_2Directions(targetDir, currentDir, true, false);
		double vectorDir[3] = {(currentDir[1] * targetDir[2] - currentDir[2] * targetDir[1]), (currentDir[2] * targetDir[0] - currentDir[0] * targetDir[2]), (currentDir[0] * targetDir[1] - currentDir[1] * targetDir[0])};
		RMATH3DOBJECT->DirectionCosines(vectorDir, vectorDir);
		if(RMATH3DOBJECT->Angle_Btwn_2Directions(vectorDir, RotationPlane, false, true) < 0)
			Angle *= -1;
		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(Angle, RotationPlane, rotateMatrix);
		return true;
}