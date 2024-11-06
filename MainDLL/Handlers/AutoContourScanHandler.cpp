#include "StdAfx.h"
#include "AutoContourScanHandler.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Shapes\AutoContourLine.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\FocusFunctions.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Helper\Helper.h"

AutoContourScanHandler::AutoContourScanHandler()
{
	setMaxClicks(2);
	FRow = FCol = FWidth = FHeight = 0;
	scanning = false;
	failed = false;
	dirfactor = 1;
	rotateOnMouseMove = false;
	PointOfRotation = NULL;
	FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::AUTOCONTOURSCANMODE;
	MAINDllOBJECT->ShowAutoContourWindow(true);
}

AutoContourScanHandler::~AutoContourScanHandler()
{
	MAINDllOBJECT->ShowAutoContourWindow(false);
	ClearAll();
}

void AutoContourScanHandler::LmouseDown()
{
	if(rotateOnMouseMove)
	{
		rotateOnMouseMove = false;			
	}
	if(MAINDllOBJECT->getCurrentWindow() == 1 && getClicksDone() == 1)
	{
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			rotateOnMouseMove = true;	
			PointOfRotation = new Vector(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
		}
		setClicksDone(0);
		return;
	}
	if(MAINDllOBJECT->getCurrentWindow() != 0)
	{
		setClicksDone(0);
		return;
	}
	pointCollection[0] = getClicksValue(0).getX();
	pointCollection[1] = getClicksValue(0).getY();
	pointCollection[2] = getClicksValue(0).getZ();
}

void AutoContourScanHandler::RmouseDown(double x, double y)
{
}

void AutoContourScanHandler::mouseMove()
{
	if(rotateOnMouseMove)
	{
		double currentShapeCenter[3] = {0};
		GetSelectedShapeCenter(currentShapeCenter);
		currentShapeCenter[2] = PointOfRotation->getZ();
		double newMatrix[9] = {0};
		CalculateMatrixesForRotation(currentShapeCenter, newMatrix);
		for each(AutoContourLine *curl in AutoContourLineCollection)
		{
			if(!(curl->selected())) continue;
			curl->Translate(Vector(-PointOfRotation->getX(), -PointOfRotation->getY(), -PointOfRotation->getZ()));
			curl->Transform(newMatrix);
			curl->Translate(Vector(PointOfRotation->getX(), PointOfRotation->getY(), PointOfRotation->getZ()));
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
}	

void AutoContourScanHandler::draw(int windowno, double WPixelWidth)
{
	try
	{		
		for each(AutoContourLine *contourline in AutoContourLineCollection)
		{
			contourline->drawCurrentShape(windowno, WPixelWidth);
		}
		if(getClicksDone() == 1)
		{
			pointCollection[3] = getClicksValue(getClicksDone()).getX();
			pointCollection[4] = getClicksValue(getClicksDone()).getY();
			pointCollection[5] = getClicksValue(getClicksDone()).getZ();
			GRAFIX->drawLine_3D(pointCollection[0], pointCollection[1], pointCollection[2], pointCollection[3], pointCollection[4], pointCollection[5]);
		}		
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOFOCUSCALC002", __FILE__, __FUNCSIG__); 
	}
}

void AutoContourScanHandler::LmaxmouseDown()
{
	if(MAINDllOBJECT->getCurrentWindow() != 0)
	{
		setClicksDone(getMaxClicks() - 1);
		return;
	}
	pointCollection[3] = getClicksValue(1).getX();
	pointCollection[4] = getClicksValue(1).getY();
	pointCollection[5] = getClicksValue(1).getZ();
	AutoContourLineCollection.push_back(new AutoContourLine(pointCollection));
	setClicksDone(0);
}

void AutoContourScanHandler::EscapebuttonPress()
{
	if(!scanning)
	{
		ClearAll();
	}
}

bool AutoContourScanHandler::StartAutoFocus()
{
	try
	{
		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) || (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)) return false;
		FCol = FOCUSCALCOBJECT->focusRectangleColumn;
		FRow = FOCUSCALCOBJECT->focusRectangleRow; 
		FWidth =  FOCUSCALCOBJECT->focusRectangleWidth * HELPEROBJECT->CamSizeRatio;
		FHeight = FOCUSCALCOBJECT->focusRectangleWidth * HELPEROBJECT->CamSizeRatio;
		Fgap = FOCUSCALCOBJECT->focusRectangleGap;
		if(Fgap == 0) Fgap = 1;
		if(AutoContourLineCollection.empty()) return false;
		/*for each(AutoContourLine *CountourLine in AutoContourLineCollection)
		{
		}*/
		for each(AutoContourLine *contourline in AutoContourLineCollection)
		{
			contourline->CreateRegions(560);
		}
		FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::AUTOCONTOURSCANMODE;
		FOCUSCALCOBJECT->FocusOnStatus(true);
		AutoContourLineIter = AutoContourLineCollection.begin();
		(*AutoContourLineIter)->RegionIter = (*AutoContourLineIter)->RegionList.begin();
		if(MAINDllOBJECT->getSelectedShapesList().getList().empty())
		{
			AddShapeAction::addShape(new CloudPoints());
			currentshape = (ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second);
		}
		else
			currentshape = (ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second);
		
		if(FOCUSCALCOBJECT->Cvalue != NULL) { free(FOCUSCALCOBJECT->Cvalue); FOCUSCALCOBJECT->Cvalue = NULL;}
		FOCUSCALCOBJECT->Cvalue = (double*)calloc((*(*AutoContourLineIter)->RegionIter)->Cell + 40, sizeof(double));
		if(FOCUSCALCOBJECT->Rectptr != NULL){ free(FOCUSCALCOBJECT->Rectptr);  FOCUSCALCOBJECT->Rectptr = NULL;}
			FOCUSCALCOBJECT->Rectptr = (*(*AutoContourLineIter)->RegionIter)->GetRectPointer((*(*AutoContourLineIter)->RegionIter)->Cell + 40, 40, &(*AutoContourLineIter)->LineParam[3], MAINDllOBJECT->getWindow(0).getUppX(), Fgap); 
		FOCUSCALCOBJECT->setRowColumn(1, (*(*AutoContourLineIter)->RegionIter)->Cell, Fgap, FWidth, FHeight);
		/*FOCUSCALCOBJECT->setContourScanWidth((*RegionIter)-> + FWidth, (*RegionIter)->Row + FHeight);*/
		FOCUSCALCOBJECT->AFSGotoPosition.SetValues((*(*AutoContourLineIter)->RegionIter)->DroPos[0], (*(*AutoContourLineIter)->RegionIter)->DroPos[1], (*(*AutoContourLineIter)->RegionIter)->DroPos[2]);
		FOCUSCALCOBJECT->GotoFocusScanPosition();
		scanning = true;
		return true;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOCONTOURSCAN001", __FILE__, __FUNCSIG__); 
		return false;
	}
}

void AutoContourScanHandler::ContinueAutoContourScan()
{
	CalculateNextGotoPosition(true);
}

void AutoContourScanHandler::RotateAutoContourScanPoints(double rotationAngle, int step)
{
	try
	{
		if (PointOfRotation == NULL) 
		{
			return;
		}
		else 
		{
			double angleofrotation = rotationAngle *  M_PI/180;
			numberOfRotation = step;
			double dir[3] = {0, 0, 1}, rotateMatrix[9] = {0};
			int s1[2] = {3,3}, s2[2] = {3,1};
			list<AutoContourLine*> tmpLinesCollection;
			for( int i = 1; i <= numberOfRotation; i++)
			{
				RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(i * angleofrotation, dir, rotateMatrix);
				for each(AutoContourLine *contourline in AutoContourLineCollection)
				{
					AutoContourLine * newline = new AutoContourLine();
					newline->CopyShapeParameters(contourline);
					newline->Translate(Vector(-PointOfRotation->getX(), -PointOfRotation->getY(), -PointOfRotation->getZ()));
					newline->Transform(rotateMatrix);
					newline->Translate(*PointOfRotation);		
					tmpLinesCollection.push_back(newline);
				}
			}
			for each(AutoContourLine *contourline in tmpLinesCollection)
			{
				AutoContourLineCollection.push_back(contourline);
			}
			tmpLinesCollection.clear();
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOCONTOURSCAN003a", __FILE__, __FUNCSIG__);
	}
}

void AutoContourScanHandler::UpdateAutoContourScanPoints(double* Zvalues)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
		int count1 = 0 , count2 = 0, cellsWithInSpan = 0, cells = (*(*AutoContourLineIter)->RegionIter)->Cell;
		double FocusStartZvalue = FOCUSCALCOBJECT->FocusStartPosition.getZ();
		double FocusSpanPos = FocusStartZvalue + FOCUSCALCOBJECT->fspan;
		double FocusSpanNeg = FocusStartZvalue - FOCUSCALCOBJECT->fspan;
		double *FocusXYZWithInSpan = new double[cells * 3];
		int *tmpRectPtr = FOCUSCALCOBJECT->Rectptr;
		PointCollection TempPtCollection;
		for(int i = 0; i < cells; i++)
		{
			if((Zvalues[i] <= FocusSpanPos) && (Zvalues[i] >= FocusSpanNeg))
			{
				double tmpPnt[3] = {cx + (tmpRectPtr[4 * i] + (tmpRectPtr[4 * i + 2]/2)) * upp, cy - (tmpRectPtr[4 * i + 1] + (tmpRectPtr[4 * i + 3]/2)) * upp, -1000};
				if(FOCUSCALCOBJECT->ApplyFocusCorrection())
				{
					double correction = 0;
					if(BESTFITOBJECT->getZError(20, 15, 40, &FOCUSCALCOBJECT->FocusErrCorrectionValue[0],  &FOCUSCALCOBJECT->FocusCalibError[0], tmpRectPtr[4 * i] + 20, tmpRectPtr[4 * i + 1] + 20, &correction))
					   Zvalues[i] -= correction;		  
				}
				FocusXYZWithInSpan[3 * cellsWithInSpan] = tmpPnt[0];
				FocusXYZWithInSpan[3 * cellsWithInSpan + 1] = tmpPnt[1];
				FocusXYZWithInSpan[3 * cellsWithInSpan + 2] = Zvalues[i]- MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ();
				cellsWithInSpan++;
			}
		}
		
		int ind1 = 0, ind2 = 1, ind3 = 2;
		if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
		{
		   ind1 = 1, ind2 = 2, ind3 = 0;
		}
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
		{
		   ind2 = 2, ind3 = 1;
		}

		for(int i = 0; i < cellsWithInSpan; i++)
		{
			TempPtCollection.Addpoint(new SinglePoint(FocusXYZWithInSpan[3 * i + ind1], FocusXYZWithInSpan[3 * i + ind2], FocusXYZWithInSpan[3 * i + ind3]));
		} 
		Line LShape(false);
		LShape.AddPoints(&TempPtCollection);
		LShape.getParameters(LineParam);

		if(TempPtCollection.Pointscount() < 0.6 * cells)
		{
		/*	failed = true;
			FOCUSCALCOBJECT->ChangeCncMode(2);
			if(MAINDllOBJECT->ShowMsgBoxString("AutoContourScanHandler01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
			{
				(*(*AutoContourLineIter)->RegionIter)->DroPos[2] = MAINDllOBJECT->getCurrentDRO().getZ();
			}
			else
			{
				currentshape->AddPoints(&TempPtCollection);*/
				failed = false;
	/*			 
			}
			FOCUSCALCOBJECT->ChangeCncMode(1);*/
		}
		else
		{
			currentshape->AddPoints(&TempPtCollection);
			failed = false;
		}
		delete [] FocusXYZWithInSpan;	
		MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
		CalculateNextGotoPosition();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOCONTOURSCAN002", __FILE__, __FUNCSIG__); }
}

void AutoContourScanHandler::CalculateNextGotoPosition(bool samePlace)
{
	try
	{
		if(!samePlace)
		{
			conti = false;
			if(!failed)
			{
				if((*AutoContourLineIter)->RegionIter != (*AutoContourLineIter)->RegionList.end())
				{
					conti = true;
					(*AutoContourLineIter)->RegionIter++;
				}

			}
			if((*AutoContourLineIter)->RegionIter == (*AutoContourLineIter)->RegionList.end())
			{
				conti = false;
				if(AutoContourLineIter == AutoContourLineCollection.end())
				{
					scanning = false;
					MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthAFSGotoNextPosition01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					return;
				}
				else
				{
					AutoContourLineIter++;
					if(AutoContourLineIter == AutoContourLineCollection.end())
					{
						scanning = false;
						MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthAFSGotoNextPosition01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
						return;
					}
				   (*AutoContourLineIter)->RegionIter = (*AutoContourLineIter)->RegionList.begin();
				   if((*AutoContourLineIter)->RegionIter == (*AutoContourLineIter)->RegionList.end())
				   {
					   CalculateNextGotoPosition();
					   return;
				   }
				   if(MAINDllOBJECT->getSelectedShapesList().getList().empty())
				   {
						AddShapeAction::addShape(new CloudPoints());
						currentshape = (ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second);
				   }
				   else
				   {
						AddShapeAction::addShape(currentshape->Clone(0, false));
						currentshape = (ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second);
				   }
				}
			}
		}
		double span = 2;
		/*if(AvgZ != 1000 && DirZ != -1000 && conti)
		{
			FOCUSCALCOBJECT->AFSGotoPosition.SetValues((*(*AutoContourLineIter)->RegionIter)->DroPos[0], (*(*AutoContourLineIter)->RegionIter)->DroPos[1], AvgZ + 2 * dirfactor * (DirZ - AvgZ));
			span = 1.4 * abs(DirZ - AvgZ);
			
		}
		else
		{*/
		FOCUSCALCOBJECT->AFSGotoPosition.SetValues((*(*AutoContourLineIter)->RegionIter)->DroPos[0], (*(*AutoContourLineIter)->RegionIter)->DroPos[1], (*(*AutoContourLineIter)->RegionIter)->DroPos[2]);
		/*}*/
		if(conti)
		{
			double Points[6] = {FOCUSCALCOBJECT->AFSGotoPosition.X, FOCUSCALCOBJECT->AFSGotoPosition.Y, FOCUSCALCOBJECT->AFSGotoPosition.Z, 0, 0, 0};
			RMATH3DOBJECT->Projection_Point_on_Line(Points, LineParam, &Points[3]);
			FOCUSCALCOBJECT->AFSGotoPosition.SetValues(Points[0], Points[1], Points[5]);
			span = abs(Points[5] - MAINDllOBJECT->getCurrentDRO().getZ()) + .6;
			if(span > 3) { span = 3;}
		}
		FOCUSCALCOBJECT->changeFocusSpan(span);
		if(FOCUSCALCOBJECT->Cvalue != NULL) { free(FOCUSCALCOBJECT->Cvalue); FOCUSCALCOBJECT->Cvalue = NULL;}
		FOCUSCALCOBJECT->Cvalue = (double*)calloc((*(*AutoContourLineIter)->RegionIter)->Cell + 40, sizeof(double));
		if(FOCUSCALCOBJECT->Rectptr != NULL){ free(FOCUSCALCOBJECT->Rectptr);  FOCUSCALCOBJECT->Rectptr = NULL;}
			FOCUSCALCOBJECT->Rectptr = (*(*AutoContourLineIter)->RegionIter)->GetRectPointer((*(*AutoContourLineIter)->RegionIter)->Cell + 40, 40, &(*AutoContourLineIter)->LineParam[3], MAINDllOBJECT->getWindow(0).getUppX(), Fgap); 
		FOCUSCALCOBJECT->setRowColumn(1, (*(*AutoContourLineIter)->RegionIter)->Cell, Fgap, FWidth, FHeight);
		//FOCUSCALCOBJECT->setContourScanWidth((*(*AutoContourLineIter)->RegionIter)->Col + FWidth, (*(*AutoContourLineIter)->RegionIter)->Row + FHeight);
		FOCUSCALCOBJECT->GotoFocusScanPosition();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOCONTOURSCAN003", __FILE__, __FUNCSIG__); }
}

void AutoContourScanHandler::ClearAll()
{
	try
	{
		if(PointOfRotation != NULL) 
		{
			delete PointOfRotation;
			PointOfRotation = NULL;
		}
		for (AutoContourLineIter = AutoContourLineCollection.begin(); AutoContourLineIter != AutoContourLineCollection.end(); AutoContourLineIter++)
		{
			(*AutoContourLineIter)->ClearRegion();
		}
		scanning = false;
		failed = false;
		dirfactor = 1;
		pointCollection.clear();
		resetClicks();
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOCONTOURSCAN005", __FILE__, __FUNCSIG__);
	}
}

bool AutoContourScanHandler::GetSelectedShapeCenter(double *currentCenter)
{
	try
	{
	int i = 0;
	double tmpCenter[3] = {0};
	for(list<AutoContourLine*>::iterator It = AutoContourLineCollection.begin(); It != AutoContourLineCollection.end(); It++)
	{
		AutoContourLine *tmp = *It;
		tmp->GetShapeCenter(tmpCenter);
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
	catch(...)
	{
	}
}

bool AutoContourScanHandler::CalculateMatrixesForRotation(double *currentShapeCenter, double *rotateMatrix)
{
	try
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
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("AUTOCONTOURSCAN007", __FILE__, __FUNCSIG__);
		return false;
	}
}