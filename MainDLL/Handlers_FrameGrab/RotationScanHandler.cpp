#include "StdAfx.h"
#include <process.h>
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "RotationScanHandler.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\Line.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "..\Actions\AddPointAction.h"
#include "..\Handlers_SmartMeasures\SM_SL1MeasureHandler.h"
#include "..\Handlers_SmartMeasures\SM_SL3MeasureHandler.h"
#include "..\Handlers_SmartMeasures\SM_RunoutMeasurementHandler.h"
#include "..\Handlers_SmartMeasures\SM_PointAngleMeasurementHandler.h"
#include <iostream>
#include <fstream>

//ofstream pointsFile;

RotationScanHandler::RotationScanHandler()
{
	try
	{	
		this->SavePointsFlag = true;
		for(int i = 0; i < 2000; i++)
			this->PixelCoordinateArray[i] = -1;
		this->slope = 0;
		for(int i = 0; i < 3; i++)
			this->CircleParam_Pixel[i] = 0; 
		StartScanning = false; ScanDirection = 0; this->DrawFramGrab = false; PauseCurrentProgram = false;
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB)
		{
			setMaxClicks(2);
			SelectionShapeType = SELECTIONSHAPETYPE::CIRCULAR;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_Circular.FrameGrab_CurrentWidth;
		}
		else if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB)
		{
			setMaxClicks(3);
			SelectionShapeType = SELECTIONSHAPETYPE::ARC;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_Arc.FrameGrab_CurrentWidth;
		}
		else if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB)
		{
			setMaxClicks(2);
			SelectionShapeType = SELECTIONSHAPETYPE::ANGULARRECTANGLE;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth;
		}
		else if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB)
		{
			setMaxClicks(2);
			SelectionShapeType = SELECTIONSHAPETYPE::RECTANGLE;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth;
		}
		else if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB)
		{
			setMaxClicks(2);
			FixedHeight = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight; 
			SelectionShapeType = SELECTIONSHAPETYPE::FIXEDRECTANGLE;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN01", __FILE__, __FUNCSIG__); }
}

RotationScanHandler::RotationScanHandler(RapidEnums::RAPIDHANDLERTYPE HandlerType)
{
	try
	{	
		this->SavePointsFlag = true; this->DrawFramGrab = false;
		for(int i = 0; i < 2000; i++)
			this->PixelCoordinateArray[i] = -1;
		this->slope = 0;
		for(int i = 0; i < 3; i++)
			this->CircleParam_Pixel[i] = 0; 
		StartScanning = false; ScanDirection = 0; PauseCurrentProgram = false;
		if(HandlerType == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB)
		{
			setMaxClicks(2);
			SelectionShapeType = SELECTIONSHAPETYPE::CIRCULAR;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_Circular.FrameGrab_CurrentWidth;
		}
		else if(HandlerType == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB)
		{
			setMaxClicks(3);
			SelectionShapeType = SELECTIONSHAPETYPE::ARC;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_Arc.FrameGrab_CurrentWidth;
		}
		else if(HandlerType == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB)
		{
			setMaxClicks(2);
			SelectionShapeType = SELECTIONSHAPETYPE::ANGULARRECTANGLE;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth;
		}
		else if(HandlerType == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB)
		{
			setMaxClicks(2);
			SelectionShapeType = SELECTIONSHAPETYPE::RECTANGLE;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth;
		}
		else if(HandlerType == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB)
		{
			setMaxClicks(2);
			FixedHeight = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight; 
			SelectionShapeType = SELECTIONSHAPETYPE::FIXEDRECTANGLE;
			SeletionFrames_Width = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth;
		}
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SMARTMEASUREMODE_HANDLER)
		{
			if(MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::PARALLELRUNOUT || MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::INTERSECTIONRUNOUT)
			{
				this->DrawFramGrab = true;
			}
		}
		else
		{
			//can not use this feature....status message
			MAINDllOBJECT->CurrentStatus("RSCAN02", false, "");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN01", __FILE__, __FUNCSIG__); }
}

RotationScanHandler::~RotationScanHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN02", __FILE__, __FUNCSIG__); }
}

void RotationScanHandler::LmouseDown()
{
	try
	{
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
		PointsPixel[getClicksDone() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN03", __FILE__, __FUNCSIG__); }
}

void RotationScanHandler::mouseMove()
{
	try
	{
		if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
		{
			PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN04", __FILE__, __FUNCSIG__); }
}

void RotationScanHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 0)
		{
			if((getClicksDone() > 0 && getClicksDone() < getMaxClicks()) || DrawFramGrab)
			{
				double wupp = WPixelWidth;
				double width = SeletionFrames_Width * wupp;
				if(SelectionShapeType == SELECTIONSHAPETYPE::RECTANGLE)
				{
					double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
					double pts[6] = {0};
					GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
					GRAFIX->translateMatrix(0.0, 0.0, -140);
					GRAFIX->drawRectangle(point1[0] + wupp, point1[1] - wupp , point2[0] - wupp, point2[1] + wupp, true);
					GRAFIX->SetColor_Double(0, 0, 1);
					GRAFIX->drawRectangle(point1[0], point1[1], point2[0], point2[1]);
					RMATH2DOBJECT->RectangleFGDirection(&point1[0], &point2[0], ScanDirection, wupp, &pts[0]);
					GRAFIX->drawTriangle(pts[0], pts[1], pts[2], pts[3], pts[4], pts[5], true);
					GRAFIX->translateMatrix(0.0, 0.0, 140);
				}
				else if(SelectionShapeType == SELECTIONSHAPETYPE::FIXEDRECTANGLE)
				{
					Vector Temp(MAINDllOBJECT->getCurrentDRO());
					PointsDRO[0].set(Temp.getX() - SeletionFrames_Width * wupp/2,  Temp.getY() + FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
					PointsDRO[1].set(Temp.getX() + SeletionFrames_Width * wupp/2,  Temp.getY() - FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
					double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
					double pts[6] = {0};
					GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
					GRAFIX->translateMatrix(0.0, 0.0, -140);
					GRAFIX->drawRectangle(point1[0] + wupp, point1[1] - wupp , point2[0] - wupp, point2[1] + wupp, true);
					GRAFIX->SetColor_Double(0, 0, 1);
					GRAFIX->drawRectangle(point1[0], point1[1], point2[0], point2[1]);
					RMATH2DOBJECT->RectangleFGDirection(&point1[0], &point2[0], ScanDirection, wupp, &pts[0]);
					GRAFIX->drawTriangle(pts[0], pts[1], pts[2], pts[3], pts[4], pts[5], true);
					GRAFIX->translateMatrix(0.0, 0.0, 140.0);
				}
				else if(SelectionShapeType == SELECTIONSHAPETYPE::CIRCULAR)
				{
					double radius = 0, radius1 = 0, radius2 = 0, cen[2] = {0};
					double Wwid1 = 35 * wupp, Wwid2 = 2 * wupp;
					PointsDRO[2].set( (PointsDRO[0].getX() + PointsDRO[1].getX())/2 , (PointsDRO[0].getY() + PointsDRO[1].getY())/2, PointsDRO[0].getZ());
					radius = RMATH2DOBJECT->Pt2Pt_distance(PointsDRO[0].getX(), PointsDRO[0].getY(), PointsDRO[2].getX(), PointsDRO[2].getY());
					radius1 = radius + width + wupp;
					radius2 = radius - width - wupp;
					cen[0] = PointsDRO[2].getX(); cen[1] = PointsDRO[2].getY();
					GRAFIX->SetColor_Double(0, 0, 1);
					if(!bool(ScanDirection))
					{
						double pt1[2] = {cen[0] + radius1 + Wwid1, cen[1]}, pt2[2] = {cen[0] + radius1, cen[1] + Wwid1}, pt3[2] = {cen[0] + radius1, cen[1] - Wwid1};
						GRAFIX->drawTriangle(pt1[0], pt1[1], pt2[0], pt2[1], pt3[0], pt3[1], true);
					}
					else
					{
						double pt1[2] = {cen[0] + radius2 - Wwid1, cen[1]}, pt2[2] = {cen[0] + radius2 - Wwid2, cen[1] + Wwid1}, pt3[2] = {cen[0] + radius2 - Wwid2, cen[1] - Wwid1};
						GRAFIX->drawTriangle(pt1[0], pt1[1], pt2[0], pt2[1], pt3[0], pt3[1], true);
					}
					GRAFIX->drawCircle(PointsDRO[2].getX(), PointsDRO[2].getY(), radius1);
					GRAFIX->drawCircle(PointsDRO[2].getX(), PointsDRO[2].getY(), radius2);
					/*GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
					GRAFIX->translateMatrix(0.0, 0.0, -140);
					GRAFIX->drawCircle(PointsDRO[2].getX(), PointsDRO[2].getY(), radius - width, radius + width);
					GRAFIX->translateMatrix(0.0, 0.0, 140);*/
				}
				else if(SelectionShapeType == SELECTIONSHAPETYPE::ARC && (getClicksDone() > 1 || DrawFramGrab))
				{
					double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()},
						point3[2] = {PointsDRO[2].getX(), PointsDRO[2].getY()};
					double pts[14], pts1[14], cen[2], radius, radius1, radius2,startangle, sweepangle, ep1[4], ep2[4], p1[4], p2[4];
		
					RMATH2DOBJECT->Arc_3Pt(&point1[0], &point2[0], &point3[0], &cen[0], &radius, &startangle, &sweepangle);
					radius1 = radius + width + wupp;
					radius2 = radius - width - wupp;								
					
					GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
					GRAFIX->translateMatrix(0.0, 0.0, -140);
					RMATH2DOBJECT->ArcFrameGrabEndPts(&cen[0], radius1, radius2, startangle, sweepangle, &ep1[0], &ep2[0]);
					GRAFIX->drawFillArcFG(cen[0], cen[1], radius1 - wupp, radius2 + wupp, startangle, sweepangle);
					GRAFIX->translateMatrix(0.0, 0.0, 140);
					
					GRAFIX->translateMatrix(0.0, 0.0, 2.0);
					GRAFIX->SetColor_Double(0, 0, 1);
					double Wwid1 = 35 * wupp, Wwid2 = 2 * wupp;
					if(!bool(ScanDirection))
					{
					
						double angle = RMATH2DOBJECT->ray_angle(ep1[0], ep1[1], ep2[0], ep2[1]);
						double midpoint[2] = {cen[0] + (radius1) *  cos(startangle + sweepangle/2), cen[1] + (radius1) *  sin(startangle + sweepangle/2)};
						double midpoint1[2] = {cen[0] + (radius1 + Wwid1) * cos(startangle + sweepangle/2), cen[1] + (radius1 + Wwid1) * sin(startangle + sweepangle/2)};
						double pt1[2] = {midpoint[0] + Wwid1 * cos(angle),midpoint[1] + Wwid1 * sin(angle)}, pt2[2] = {midpoint[0] - Wwid1 * cos(angle),midpoint[1] - Wwid1 * sin(angle)};
						GRAFIX->drawTriangle(midpoint1[0], midpoint1[1], pt1[0], pt1[1],pt2[0], pt2[1], true);
					}
					else
					{
						double angle = RMATH2DOBJECT->ray_angle(ep1[0], ep1[1], ep2[0], ep2[1]);
						double midpoint[2] = {cen[0] + (radius2 - Wwid2) *  cos(startangle + sweepangle/2), cen[1] + (radius2 - Wwid2) *  sin(startangle + sweepangle/2)};
						double midpoint1[2] = {cen[0] + (radius2 - Wwid1) *  cos(startangle + sweepangle/2), cen[1] + (radius2 - Wwid1) *  sin(startangle + sweepangle/2)};
						double pt1[2] = {midpoint[0] + Wwid1 * cos(angle),midpoint[1] + Wwid1 * sin(angle)}, pt2[2] = {midpoint[0] - Wwid1 * cos(angle),midpoint[1] - Wwid1 * sin(angle)};
						GRAFIX->drawTriangle(midpoint1[0], midpoint1[1], pt1[0], pt1[1],pt2[0], pt2[1], true);
					}
					GRAFIX->drawArc(cen[0], cen[1], radius1, startangle, sweepangle, wupp);
					GRAFIX->drawArc(cen[0], cen[1], radius2, startangle, sweepangle, wupp);
					GRAFIX->drawLine(ep1[0], ep1[1], ep1[2], ep1[3]);
					GRAFIX->drawLine(ep2[0], ep2[1], ep2[2], ep2[3]);
					GRAFIX->translateMatrix(0.0, 0.0, -2.0);
				}
				else if(SelectionShapeType == SELECTIONSHAPETYPE::ANGULARRECTANGLE)
				{
					double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
					double pts[14] = {0}, pts1[14] = {0};
					RMATH2DOBJECT->RectangleFG(&point1[0], &point2[0], width, wupp, &pts[0], bool(ScanDirection));
					RMATH2DOBJECT->RectangleFG(&point1[0], &point2[0], width - wupp, wupp, &pts1[0], bool(ScanDirection));
					GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
					GRAFIX->translateMatrix(0.0, 0.0, -140);
					GRAFIX->drawRectangle(pts1[0], pts1[1], pts1[2], pts1[3], pts1[6], pts1[7], pts1[4], pts1[5], true);
					GRAFIX->translateMatrix(0.0, 0.0, 2.0);
					GRAFIX->SetColor_Double(0, 0, 1);
					GRAFIX->drawTriangle(pts[8], pts[9], pts[10], pts[11], pts[12], pts[13], true);
					GRAFIX->drawRectangle(pts[0], pts[1], pts[2], pts[3], pts[6], pts[7], pts[4], pts[5]);
					GRAFIX->translateMatrix(0.0, 0.0, 140.0);
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN05", __FILE__, __FUNCSIG__); }
}

void RotationScanHandler::LmouseUp(double x, double y)
{
}

void RotationScanHandler::LmaxmouseDown()
{
	try
	{
		double wupp = MAINDllOBJECT->getWindow(1).getUppX();
		double width = SeletionFrames_Width * wupp;
		ShapeWithList *ShapeToAddPnts = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if (ShapeToAddPnts != NULL) 
		{
			 if(PPCALCOBJECT->IsPartProgramRunning())
			 {
				baseaction = PPCALCOBJECT->getFrameGrab();
				PointsDRO[0].set(baseaction->points[0]);
				PointsDRO[1].set(baseaction->points[1]);
				PointsDRO[2].set(baseaction->points[2]);
				PointsPixel[0].set(baseaction->myPosition[0]);
				PointsPixel[1].set(baseaction->myPosition[1]);
				PointsPixel[2].set(baseaction->myPosition[2]);
				ScanDirection = baseaction->FGdirection;
				this->SeletionFrames_Width = baseaction->FGWidth;
				if(SelectionShapeType == SELECTIONSHAPETYPE::FIXEDRECTANGLE)
				{
					SeletionFrames_Width = abs(PointsPixel[0].getX() - PointsPixel[1].getX()); 
					FixedHeight = abs(PointsPixel[0].getY() - PointsPixel[1].getY());
				}
				this->DrawFramGrab = true;
				//MessageBox(NULL, L"Came to ProgramData Step", L"Rapid-I", MB_TOPMOST);

				MAINDllOBJECT->ShowRotationScanWindow(1); // 3);
				//MAINDllOBJECT->UpdateShapesChanged();
			 }
			 else
			 {
				PointsDRO[getMaxClicks() - 1].set(getClicksValue(getMaxClicks() - 1));
				PointsPixel[getMaxClicks() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
				UsePreviousFramGrabForScanning();
			}
			if(SelectionShapeType == SELECTIONSHAPETYPE::ARC)
			{
				double ans[3] = {0};
				double point1[3] = {PointsPixel[0].getX(), PointsPixel[0].getY(), 0}, point2[3] = {PointsPixel[1].getX(), PointsPixel[1].getY(), 0},
					point3[3] = {PointsPixel[2].getX(), PointsPixel[2].getY(), 0}, cen[3] = {0}, radius = 0, startangle = 0, sweepangle = 0;
				RMATH2DOBJECT->Arc_3Pt(&point1[0], &point2[0], &point3[0], &cen[0], &radius, &startangle, &sweepangle);
				this->CircleParam_Pixel[0] = cen[0]; this->CircleParam_Pixel[1] = cen[1]; this->CircleParam_Pixel[2] = radius; 
			}
			else if(SelectionShapeType == SELECTIONSHAPETYPE::CIRCULAR)
			{
				this->CircleParam_Pixel[0] = int((PointsPixel[0].getX() + PointsPixel[1].getX()) / 2); this->CircleParam_Pixel[1] = int((PointsPixel[0].getY() + PointsPixel[1].getY()) / 2);
				this->CircleParam_Pixel[2] = int(RMATH2DOBJECT->Pt2Pt_distance(PointsPixel[0].getX(), PointsPixel[0].getY(), (double)CircleParam_Pixel[0], (double)CircleParam_Pixel[1]));
			}
			 SetScanningTypeEnum();
		}
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN06", __FILE__, __FUNCSIG__); }

}

void RotationScanHandler::RmouseDown(double x, double y)
{
	try
	{
		switch(SelectionShapeType)
		{
		case SELECTIONSHAPETYPE::ANGULARRECTANGLE:
		case SELECTIONSHAPETYPE::ARC:
		case SELECTIONSHAPETYPE::CIRCULAR:
			if(ScanDirection < 1)
				ScanDirection++;
			else 
				ScanDirection = 0;
			break;
		case SELECTIONSHAPETYPE::RECTANGLE:
		case SELECTIONSHAPETYPE::FIXEDRECTANGLE:
			if(ScanDirection < 3)
				ScanDirection++;
			else 
				ScanDirection = 0;
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN07", __FILE__, __FUNCSIG__); }
}

void RotationScanHandler::MouseScroll(bool flag)
{
	if(flag)
	{
		switch(SelectionShapeType)
		{
		case SELECTIONSHAPETYPE::ANGULARRECTANGLE:
			if(SeletionFrames_Width < MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_MaxWidth) SeletionFrames_Width += 5;
			break;
		case SELECTIONSHAPETYPE::ARC:
			if(SeletionFrames_Width < MAINDllOBJECT->FrameGrab_Arc.FrameGrab_MaxWidth) SeletionFrames_Width += 5;
			break;
		case SELECTIONSHAPETYPE::CIRCULAR:
			if(SeletionFrames_Width < MAINDllOBJECT->FrameGrab_Circular.FrameGrab_MaxWidth) SeletionFrames_Width += 5;
			break;
		case SELECTIONSHAPETYPE::FIXEDRECTANGLE:
			int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y;
			double wupp = MAINDllOBJECT->getWindow(0).getUppX();
			int IncGap = 20;
			if(ScanDirection == 0 || ScanDirection == 2)
			{
				SeletionFrames_Width = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth; 
				if(SeletionFrames_Width < MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MaxWidth) SeletionFrames_Width += IncGap;
				MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth = SeletionFrames_Width;
			}
			else
			{
				FixedHeight = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight; 
				if(FixedHeight < MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MaxHeight) FixedHeight += IncGap;
				MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight = FixedHeight;
			}
			Vector Temp = MAINDllOBJECT->getCurrentDRO().getX();
			PointsPixel[0].set(wwidth/2 - SeletionFrames_Width/2, wheight/2 - FixedHeight/2);
			PointsPixel[1].set(wwidth/2 + SeletionFrames_Width/2, wheight/2 + FixedHeight/2);
			PointsDRO[0].set(Temp.getX() - SeletionFrames_Width * wupp/2,  Temp.getY() + FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
			PointsDRO[1].set(Temp.getX() + SeletionFrames_Width * wupp/2,  Temp.getY() - FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
			break;
		}
	}
	else 
	{
		switch(SelectionShapeType)
		{
		case SELECTIONSHAPETYPE::ANGULARRECTANGLE:
			if(SeletionFrames_Width > MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_MinWidth) SeletionFrames_Width -= 5;
			break;
		case SELECTIONSHAPETYPE::ARC:
			if(SeletionFrames_Width > MAINDllOBJECT->FrameGrab_Arc.FrameGrab_MinWidth) SeletionFrames_Width -= 5;
			break;
		case SELECTIONSHAPETYPE::CIRCULAR:
			if(SeletionFrames_Width > MAINDllOBJECT->FrameGrab_Circular.FrameGrab_MinWidth) SeletionFrames_Width -= 5;
			break;
		case SELECTIONSHAPETYPE::FIXEDRECTANGLE:
			double wupp = MAINDllOBJECT->getWindow(0).getUppX();
			int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y;
			int IncGap = 20;
			if(ScanDirection == 0 || ScanDirection == 2)
			{
				SeletionFrames_Width = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth; 
				if(SeletionFrames_Width > MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MinHeight) SeletionFrames_Width -= IncGap;
				MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth = SeletionFrames_Width;
			}
			else
			{
				FixedHeight = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight; 
				if(FixedHeight > MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MinHeight) FixedHeight -= IncGap;
				MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight = FixedHeight;
			}
			Vector Temp = MAINDllOBJECT->getCurrentDRO().getX();
			PointsPixel[0].set(wwidth/2 - SeletionFrames_Width/2, wheight/2 - FixedHeight/2);
			PointsPixel[1].set(wwidth/2 + SeletionFrames_Width/2, wheight/2 + FixedHeight/2);
			PointsDRO[0].set(Temp.getX() - SeletionFrames_Width * wupp/2,  Temp.getY() + FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
			PointsDRO[1].set(Temp.getX() + SeletionFrames_Width * wupp/2,  Temp.getY() - FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
			break;
		}
	}
}

void RotationScanHandler::EscapebuttonPress()
{
	try
	{
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN08", __FILE__, __FUNCSIG__); }
}

void RotationScanHandler::PartProgramData()
{
	try
	{
		LmaxmouseDown();
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN09", __FILE__, __FUNCSIG__); 
	}
}

void RotationScanHandler::RotationScanPoints()
{
	try
	{
		if(HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false))
		{
			if(baseaction->noofpts > 0)
			{
				MAINDllOBJECT->dontUpdateGraphcis = true;
				MAINDllOBJECT->Wait_VideoGraphicsUpdate();
				MAINDllOBJECT->Wait_RcadGraphicsUpdate();
				MAINDllOBJECT->Wait_DxfGraphicsUpdate();
				UpdatePixelCoordinate(baseaction->noofpts);
				if(UpdatePointsMapList(baseaction->noofpts))		//updating points with farthest points.
					AddPointsToSelectedShape();
				MAINDllOBJECT->dontUpdateGraphcis = false;
				MAINDllOBJECT->UpdateShapesChanged();
			}
		}
		else
			Sleep(20);

	}
	catch(...)
	{
	   MAINDllOBJECT->dontUpdateGraphcis = false;
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN10", __FILE__, __FUNCSIG__); 
	   
	}
}

void RotationScanHandler::StartRotationalScanning(bool ScanStart)
{
	try
	{
		StartScanning = ScanStart;
		this->SavePointsFlag = ScanStart;
		

		if(StartScanning)
		{
			HELPEROBJECT->RotationScanFlag = ScanStart;
			PtsSructureList.clear();
			//pointsFile.open("D:\\rotaryscanpts.csv");
			//Begin Scanning Thread.................
			_beginthread(&ContinuousScanIndifferentThread, 0, this);
		}
		else
		{

		}
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN10", __FILE__, __FUNCSIG__); 
	}
}

void RotationScanHandler::CotninuePauseRotationalScanning(bool Pauseflag)
{
	try
	{
		PauseCurrentProgram = Pauseflag;
		if(Pauseflag)
			MAINDllOBJECT->ShowRotationScanWindow(3);
		/*StartRotationalScanning(!Pauseflag);*/
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN10A", __FILE__, __FUNCSIG__); 
	}
}

bool RotationScanHandler::UpdatePointsMapList(int NoOfPoints)
{
	try
	{
		bool ReturnFlag = false;
		if(ScanningEdgeAxis == SCANNINGTYPE::XAXIS_UPPEREDGE)
		{
			if(PtsSructureList.size() > 0)
			{
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					if(PtsSructureList.find(PixelCoordinateArray[n * 2]) == PtsSructureList.end())
					{
						PtsStruct TmpStruct;
						TmpStruct.Index = PixelCoordinateArray[n * 2]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2 + 1];
						TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
						PtsSructureList[TmpStruct.Index] = TmpStruct;
						ReturnFlag = true;
					}
					else
					{
						if(PixelCoordinateArray[n * 2 + 1] < PtsSructureList[PixelCoordinateArray[n * 2]].Rvalue)
						{
							PtsSructureList[PixelCoordinateArray[n * 2]].Rvalue = PixelCoordinateArray[n * 2 + 1];
							PtsSructureList[PixelCoordinateArray[n * 2]].X = EDSCOBJECT->DetectedPointsList[n * 2];
							PtsSructureList[PixelCoordinateArray[n * 2]].Y = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
							ReturnFlag = true;
						}
					}
				}
			}
			else
			{
				ReturnFlag = true;
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					PtsStruct TmpStruct;
					TmpStruct.Index = PixelCoordinateArray[n * 2]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2 + 1];
					TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
					PtsSructureList[TmpStruct.Index] = TmpStruct;
				}
			}
		}
		else if(ScanningEdgeAxis == SCANNINGTYPE::XAXIS_LOWEREDGE)
		{
			if(PtsSructureList.size() > 0)
			{
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					if(PtsSructureList.find(PixelCoordinateArray[n * 2]) == PtsSructureList.end())
					{
						PtsStruct TmpStruct;
						TmpStruct.Index = PixelCoordinateArray[n * 2]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2 + 1];
						TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
						PtsSructureList[TmpStruct.Index] = TmpStruct;
						ReturnFlag = true;
					}
					else
					{
						if(PixelCoordinateArray[n * 2 + 1] > PtsSructureList[PixelCoordinateArray[n * 2]].Rvalue)
						{
							PtsSructureList[PixelCoordinateArray[n * 2]].Rvalue = PixelCoordinateArray[n * 2 + 1];
							PtsSructureList[PixelCoordinateArray[n * 2]].X = EDSCOBJECT->DetectedPointsList[n * 2];
							PtsSructureList[PixelCoordinateArray[n * 2]].Y = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
							ReturnFlag = true;
						}
					}
				}
			}
			else
			{
				ReturnFlag = true;
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					PtsStruct TmpStruct;
					TmpStruct.Index = PixelCoordinateArray[n * 2]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2 + 1];
					TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
					PtsSructureList[TmpStruct.Index] = TmpStruct;
				}
			}
		}
		else if(ScanningEdgeAxis == SCANNINGTYPE::YAXIS_UPPEREDGE)
		{
			if(PtsSructureList.size() > 0)
			{
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					if(PtsSructureList.find(PixelCoordinateArray[n * 2 + 1]) == PtsSructureList.end())
					{
						PtsStruct TmpStruct;
						TmpStruct.Index = PixelCoordinateArray[n * 2 + 1]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2];
						TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2 + 1]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2];
						PtsSructureList[TmpStruct.Index] = TmpStruct;
						ReturnFlag = true;
					}
					else
					{
						if(PixelCoordinateArray[n * 2] > PtsSructureList[PixelCoordinateArray[n * 2 + 1]].Rvalue)
						{
							PtsSructureList[PixelCoordinateArray[n * 2 + 1]].Rvalue = PixelCoordinateArray[n * 2];
							PtsSructureList[PixelCoordinateArray[n * 2 + 1]].X = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
							PtsSructureList[PixelCoordinateArray[n * 2 + 1]].Y = EDSCOBJECT->DetectedPointsList[n * 2];
							ReturnFlag = true;
						}
					}
				}
			}
			else
			{
				ReturnFlag = true;
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					PtsStruct TmpStruct;
					TmpStruct.Index = PixelCoordinateArray[n * 2 + 1]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2];
					TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2 + 1]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2];
					PtsSructureList[TmpStruct.Index] = TmpStruct;
				}
			}
		}
		else if(ScanningEdgeAxis == SCANNINGTYPE::YAXIS_LOWEREDGE)
		{
			if(PtsSructureList.size() > 0)
			{
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					if(PtsSructureList.find(PixelCoordinateArray[n * 2 + 1]) == PtsSructureList.end())
					{
						PtsStruct TmpStruct;
						TmpStruct.Index = PixelCoordinateArray[n * 2 + 1]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2];
						TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2 + 1]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2];
						PtsSructureList[TmpStruct.Index] = TmpStruct;
						ReturnFlag = true;
					}
					else
					{
						if(PixelCoordinateArray[n * 2] < PtsSructureList[PixelCoordinateArray[n * 2 + 1]].Rvalue)
						{
							PtsSructureList[PixelCoordinateArray[n * 2 + 1]].Rvalue = PixelCoordinateArray[n * 2];
							PtsSructureList[PixelCoordinateArray[n * 2 + 1]].X = EDSCOBJECT->DetectedPointsList[n * 2 + 1];
							PtsSructureList[PixelCoordinateArray[n * 2 + 1]].Y = EDSCOBJECT->DetectedPointsList[n * 2];
							ReturnFlag = true;
						}
					}
				}
			}
			else
			{
				ReturnFlag = true;
				for(int n = 0; n < NoOfPoints; n++)
				{
					if(PixelCoordinateArray[n * 2] == -1) continue;
					PtsStruct TmpStruct;
					TmpStruct.Index = PixelCoordinateArray[n * 2 + 1]; TmpStruct.Rvalue = PixelCoordinateArray[n * 2];
					TmpStruct.X = EDSCOBJECT->DetectedPointsList[n * 2 + 1]; TmpStruct.Y = EDSCOBJECT->DetectedPointsList[n * 2];
					PtsSructureList[TmpStruct.Index] = TmpStruct;
				}
			}
		}
		return ReturnFlag;
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN11", __FILE__, __FUNCSIG__); 
	   return false;
	}
}

bool RotationScanHandler::SetScanningTypeEnum()
{
	try
	{
		switch(SelectionShapeType)
		{
			case SELECTIONSHAPETYPE::ANGULARRECTANGLE:
				{
					double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
					slope = RMATH2DOBJECT->ray_angle(point2, point1);
					switch(ScanDirection)
					{
						case 0:
							{
								if((slope < 3 * M_PI_4 && slope >= M_PI_4) || (slope >= 5 * M_PI_4 && slope < 7 * M_PI_4))
								{
									if(abs(PointsPixel[0].getX() - PointsPixel[1].getX()) > abs(PointsPixel[0].getY() - PointsPixel[1].getY()))
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
									else
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
								}
								else
								{
									if(abs(PointsPixel[0].getX() - PointsPixel[1].getX()) > abs(PointsPixel[0].getY() - PointsPixel[1].getY()))
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
									else
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
								}
							}
							break;
						case 1:
							{
								if((slope < 3 * M_PI_4 && slope >= M_PI_4) || (slope >= 5 * M_PI_4 && slope < 7 * M_PI_4))
								{
									if(abs(PointsPixel[0].getX() - PointsPixel[1].getX()) > abs(PointsPixel[0].getY() - PointsPixel[1].getY()))
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
									else
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
								}
								else 
								{
									if(abs(PointsPixel[0].getX() - PointsPixel[1].getX()) > abs(PointsPixel[0].getY() - PointsPixel[1].getY()))
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
									else
										ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
								}
							}
							break;
					}
				}
				break;
				case SELECTIONSHAPETYPE::ARC:
				{
					double point1[2] = {(double)PointsPixel[0].getX(), (double)PointsPixel[0].getY()}, point2[2] = {(double)PointsPixel[2].getX(), (double)PointsPixel[2].getY()};
					slope = RMATH2DOBJECT->ray_angle(point2, point1);
					switch(ScanDirection)
					{
					case 0:
						if((slope < M_PI_4 * 3 && slope >= 0) || (slope < 2 * M_PI && slope >  M_PI_4 * 7))
							ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
						else
							ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
						break;
					case 1:
						if((slope < M_PI_4 * 3 && slope >= 0) || (slope < 2 * M_PI && slope >  M_PI_4 * 7))
							ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
						else
							ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
						break;
					}
				}
				break;
			case SELECTIONSHAPETYPE::CIRCULAR:
				switch(ScanDirection)
				{
				case 0:
					ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
					break;
				case 1:
					ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
					break;
				}
				break;
			case SELECTIONSHAPETYPE::RECTANGLE:
			case SELECTIONSHAPETYPE::FIXEDRECTANGLE:
				{
					switch(ScanDirection)
					{
					case 0:
						ScanningEdgeAxis = SCANNINGTYPE::YAXIS_UPPEREDGE;
						break;
					case 1:
						ScanningEdgeAxis = SCANNINGTYPE::XAXIS_LOWEREDGE;
						break;
					case 2:
						ScanningEdgeAxis = SCANNINGTYPE::YAXIS_LOWEREDGE;
						break;
					case 3:
						ScanningEdgeAxis = SCANNINGTYPE::XAXIS_UPPEREDGE;
						break;
					}
				}
				break;
		}
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN11", __FILE__, __FUNCSIG__); 
	   return false;
	}
}

void RotationScanHandler::AddPointsToSelectedShape(bool AddAction)
{
	try
	{
		ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(CShape == NULL) return;
		if(this->SavePointsFlag)
		{
			this->SavePointsFlag = false;
			LastFramePointCollection.deleteAll();
			LastFramePointCollection.EraseAll();
			for(PC_ITER Item = CShape->PointsList->getList().begin(); Item != CShape->PointsList->getList().end(); Item++)
			{     
				  SinglePoint* Spt = Item->second;
				  LastFramePointCollection.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
			}
		}
		CShape->deleteAllPoints();
		baseaction->AllPointsList.deleteAll();
		baseaction->PointActionIdList.clear();

		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio, upp_y = MAINDllOBJECT->getWindow(0).getUppY() / HELPEROBJECT->CamSizeRatio;
		Vector Cdro = MAINDllOBJECT->getCurrentDRO();

		if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			if(ScanningEdgeAxis == SCANNINGTYPE::XAXIS_UPPEREDGE || ScanningEdgeAxis == SCANNINGTYPE::XAXIS_LOWEREDGE)
			{
				for(std::map<int, PtsStruct>::iterator itr = PtsSructureList.begin(); itr != PtsSructureList.end(); itr++)
				{
					PtsStruct TmpValue = (*itr).second;
					double X = TmpValue.X, Y = TmpValue.Y;
					baseaction->AllPointsList.Addpoint(new SinglePoint(cx + X * upp_x, cy - Y * upp_y, Cdro.getZ()));
				}
			}
			else
			{
				for(std::map<int, PtsStruct>::iterator itr = PtsSructureList.begin(); itr != PtsSructureList.end(); itr++)
				{
					PtsStruct TmpValue = (*itr).second;
					double Y = TmpValue.X, X = TmpValue.Y;
					baseaction->AllPointsList.Addpoint(new SinglePoint(cx + X * upp_x, cy - Y * upp_y, Cdro.getZ()));
				}
			}
			baseaction->noofpts = PtsSructureList.size();
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			for(PC_ITER Item = baseaction->AllPointsList.getList().begin(); Item != baseaction->AllPointsList.getList().end(); Item++)
			{     
				  SinglePoint* Spt = Item->second;
				  double temp1[3], temp2[3] = {Spt->X, Spt->Y, Spt->Z}; 
				  RMATH2DOBJECT->MultiplyMatrix1(&MAINDllOBJECT->getCurrentUCS().transform[0], Order1, temp2, Order2, temp1);
				  Spt->SetValues(temp1[0], temp1[1], temp1[2]);
			}
		}
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			CShape->AddPoints(&LastFramePointCollection);
			CShape->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
		}
		else
		{
			if(AddAction)
			{
				UpdateFramGrabProperties();
				CShape->AddPoints(&LastFramePointCollection);
				AddPointAction::pointAction(CShape, baseaction);
			}
			else
			{
				CShape->AddPoints(&LastFramePointCollection);
				CShape->AddPoints(&baseaction->AllPointsList);
			}
		}
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN10", __FILE__, __FUNCSIG__); 
	}
}

void ContinuousScanIndifferentThread(void *anything)
{
	try
	{
		RotationScanHandler* ScanningThread = (RotationScanHandler*)anything;
		if(!ScanningThread->baseaction->SurfaceON)
		{
			if(ScanningThread->baseaction->ProfileON)
			{
				while(ScanningThread->StartScanning)
				{
					ScanningThread->RotationScanPoints();
				}
				ScanningThread->StoppedScanning();
			}
			else
			{
				MAINDllOBJECT->ShowRotationScanWindow(2);
				ScanningThread->DrawFramGrab = false;
				//show message and reset all flags ..
				ScanningThread->StartScanning = false;
				HELPEROBJECT->RotationScanFlag = false;
				//Show status message you cannot use this feature in surface light..
				MAINDllOBJECT->CurrentStatus("RSCAN01", false, "");
			}
		}
		else
		{
			MAINDllOBJECT->ShowRotationScanWindow(2);
			ScanningThread->DrawFramGrab = false;
			//show message and reset all flags ..
			ScanningThread->StartScanning = false;
			HELPEROBJECT->RotationScanFlag = false;
			//Show status message you cannot use this feature in surface light..
			MAINDllOBJECT->CurrentStatus("RSCAN01", false, "");
		}
		_endthread();
	}
	catch(...)
	{
		HELPEROBJECT->RotationScanFlag = false;
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN14", __FILE__, __FUNCSIG__); 
	   _endthread();
	}
}

void RotationScanHandler::UpdateFramGrabProperties()
{
	try
	{
		baseaction->FGdirection = this->ScanDirection;
		baseaction->FGWidth = this->SeletionFrames_Width;
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN15", __FILE__, __FUNCSIG__); 
	}
}

void RotationScanHandler::UpdatePixelCoordinate(int NoOfPoints)
{
	try
	{
		switch(SelectionShapeType)
		{
		case SELECTIONSHAPETYPE::ANGULARRECTANGLE:
			{
				double TransformMatrix[9] = {cos(slope), -sin(slope), 0, sin(slope), cos(slope), 0, 0, 0, 1};
				Vector TmpValue;
				for(int i = 0; i < NoOfPoints; i++)
				{
					TmpValue = MAINDllOBJECT->TransformMultiply(TransformMatrix, EDSCOBJECT->DetectedPointsList[i * 2], EDSCOBJECT->DetectedPointsList[i * 2 + 1]);
					PixelCoordinateArray[i * 2] = (TmpValue.getX()); PixelCoordinateArray[i * 2 + 1] = (TmpValue.getY());
					//if ((int)EDSCOBJECT->DetectedPointsList[i * 2] % 5 == 0)
					//{
					//	//pointsFile << EDSCOBJECT->DetectedPointsList[i * 2] <<  "," << EDSCOBJECT->DetectedPointsList[i * 2 + 1] << ","; 
					//}
				}
				//pointsFile << endl;
			}
			break;
		case SELECTIONSHAPETYPE::ARC:
			{
				double point1[2] = {CircleParam_Pixel[0], CircleParam_Pixel[1]}, point2[2] = {0};
				int Rvalue = 0;
				for(int i = 0; i < NoOfPoints; i++)
				{
					point2[0] = EDSCOBJECT->DetectedPointsList[i * 2]; point2[1] = EDSCOBJECT->DetectedPointsList[i * 2 + 1];
					double TmpAng = RMATH2DOBJECT->ray_angle(point2, point1);
					Rvalue = (int)RMATH2DOBJECT->Pt2Pt_distance(point1, point2);
					PixelCoordinateArray[i * 2] = CircleParam_Pixel[2] * TmpAng / 2; PixelCoordinateArray[i * 2 + 1] = Rvalue;
					//if ((int)EDSCOBJECT->DetectedPointsList[i * 2] % 5 == 0)
					//{
					//	//pointsFile << EDSCOBJECT->DetectedPointsList[i * 2] <<  "," << EDSCOBJECT->DetectedPointsList[i * 2 + 1] << endl;
					//}
				}
			}
			break;
		case SELECTIONSHAPETYPE::CIRCULAR:
			{
				double point1[2] = {CircleParam_Pixel[0], CircleParam_Pixel[1]}, point2[2] = {0};
				int Rvalue = 0;
				for(int i = 0; i < NoOfPoints; i++)
				{
					point2[0] = EDSCOBJECT->DetectedPointsList[i * 2]; point2[1] = EDSCOBJECT->DetectedPointsList[i * 2 + 1];
					double TmpAng = RMATH2DOBJECT->ray_angle(point2, point1);
					Rvalue = EDSCOBJECT->DetectedPointsList[i * 2 + 1];
					PixelCoordinateArray[i * 2] = CircleParam_Pixel[2] * TmpAng / 2; PixelCoordinateArray[i * 2 + 1] = Rvalue;
				}
			}
			break;
		case SELECTIONSHAPETYPE::RECTANGLE:
		case SELECTIONSHAPETYPE::FIXEDRECTANGLE:
			for(int i = 0; i < NoOfPoints; i++)
			{
				PixelCoordinateArray[2 * i] = EDSCOBJECT->DetectedPointsList[2 * i];
				PixelCoordinateArray[2 * i + 1] = EDSCOBJECT->DetectedPointsList[2 * i + 1];
			}
			break;
		}
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN16", __FILE__, __FUNCSIG__); 
	}
}

void RotationScanHandler::StoppedScanning()
{
	try
	{
		HELPEROBJECT->RotationScanFlag = false;
		//stop scanning ...
		this->DrawFramGrab = false;
		MAINDllOBJECT->dontUpdateGraphcis = true;
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->Wait_DxfGraphicsUpdate();

		//pointsFile.close();

		AddPointsToSelectedShape(true);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->UpdateShapesChanged();
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(!PauseCurrentProgram)
				PPCALCOBJECT->partProgramReached();
			else
			{
				ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
				if(CShape != NULL)
					CShape->deleteAllPoints();
			}
		}
		else
		{
			if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SMARTMEASUREMODE_HANDLER)
			{
				if(MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINESWITHAXIS)
						((SM_SL1MeasureHandler *)MAINDllOBJECT->getCurrentHandler())->RotationStopedAddMeasure();
				else if(MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_4LINESWITHAXIS)
						((SM_SL3MeasureHandler *)MAINDllOBJECT->getCurrentHandler())->RotationStopedAddMeasure();
				else if(MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::PARALLELRUNOUT || MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::INTERSECTIONRUNOUT)
				{
					 this->DrawFramGrab = true;
					 ((SM_RunoutMeasurementHandler *)MAINDllOBJECT->getCurrentHandler())->RotationStopedAddMeasure();
				}
				else if(MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::POINTANGLE)
						((SM_PointAngleMeasurementHandler *)MAINDllOBJECT->getCurrentHandler())->RotationStopedAddMeasure();
			}
		}
		PauseCurrentProgram = false;
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN17", __FILE__, __FUNCSIG__); 
	}
}

void RotationScanHandler::UsePreviousFramGrabForScanning()
{
	try
	{
		if(SelectionShapeType == SELECTIONSHAPETYPE::RECTANGLE)
			baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::ROTATION_RECTANGLEFRAMEGRAB);
		else if(SelectionShapeType == SELECTIONSHAPETYPE::CIRCULAR)
			baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::ROTATION_CIRCLEFRAMEGRAB);
		else if(SelectionShapeType == SELECTIONSHAPETYPE::ARC)
			baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ARCFRAMEGRAB);
		else if(SelectionShapeType == SELECTIONSHAPETYPE::ANGULARRECTANGLE)
			baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ANGULARRECTANGLEFRAMEGRAB);
		else if(SelectionShapeType == SELECTIONSHAPETYPE::FIXEDRECTANGLE)
			baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::ROTATIONSCAN_FIXEDRECTANGLE);
		baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO);
		baseaction->CurrentWindowNo = MAINDllOBJECT->getCurrentWindow();
		if(MAINDllOBJECT->CameraConnected())
		{
			baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
			baseaction->camProperty = MAINDllOBJECT->getCamSettings();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}
		baseaction->ProfileON = MAINDllOBJECT->getProfile();
		baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		for(int i = 0; i < 3; i++)
		{
			baseaction->points[i].set(PointsDRO[i]);
			baseaction->myPosition[i].set(PointsPixel[i]);
		}	
		baseaction->FGdirection = this->ScanDirection;
		baseaction->FGWidth = this->SeletionFrames_Width;
		MAINDllOBJECT->ShowRotationScanWindow(0);
		this->DrawFramGrab = true;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RSCAN18", __FILE__, __FUNCSIG__); 
	}
}