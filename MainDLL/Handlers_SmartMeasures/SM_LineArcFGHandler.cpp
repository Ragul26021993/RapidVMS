#include "StdAfx.h"
#include "SM_LineArcFGHandler.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\ProfileScanAction.h"
#include "..\Helper\Helper.h"

//Constructor..//
SM_LineArcFGHandler::SM_LineArcFGHandler()
{
	try
	{
		if(MAINDllOBJECT->getSelectedShapesList().count() > 0)
		{
			setMaxClicks(0);
		}
		else
		{
			finalShape = true;
			setMaxClicks(2);
		}
		HELPEROBJECT->ProfileScanLineArcAction = NULL;
		ValidFlag = false;
		this->sort_pts_for_linearcsep = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0001", __FILE__, __FUNCSIG__); }
}

SM_LineArcFGHandler::~SM_LineArcFGHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0002", __FILE__, __FUNCSIG__); }
}



//Handle the mouse move......//
void SM_LineArcFGHandler::mouseMove()
{
	try
	{
		if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
		{
			PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
			PointsPixel[getMaxClicks() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
		}
		if(getClicksDone() == getMaxClicks() - 1)
			ValidFlag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void SM_LineArcFGHandler::LmouseDown()
{
	try
	{
		if(getClicksDone() == 1)
			RectDirection = 0;
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
		PointsPixel[getClicksDone() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y, MAINDllOBJECT->getCurrentDRO().getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0004", __FILE__, __FUNCSIG__); }
}

//Handle lmouse up.
void SM_LineArcFGHandler::LmouseUp(double x, double y)
{
	try
	{

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0005", __FILE__, __FUNCSIG__); }
}

//Handle Rmouse down..
void SM_LineArcFGHandler::RmouseDown(double x, double y)
{
	try
	{  
		ChangeDirection();
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0006", __FILE__, __FUNCSIG__); }
}

void SM_LineArcFGHandler::AddpointCollFromCloudPnt()
{
	try
	{  
		this->sort_pts_for_linearcsep = true;
		RapidEnums::RAPIDFGACTIONTYPE currfgtype;
		setMaxClicks(0);
		ShapeWithList *tmpShape = ((ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem());
		for(RC_ITER ActItem = MAINDllOBJECT->getActionsHistoryList().getList().begin(); ActItem != MAINDllOBJECT->getActionsHistoryList().getList().end(); ActItem++)
		{
			RAction* CurrentAction = (RAction*)((*ActItem).second);
			if (CurrentAction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) continue;

			if (tmpShape->getId() == ((AddPointAction*) CurrentAction)->getShape()->getId())
			{
				currfgtype = ((AddPointAction*) CurrentAction)->getFramegrab()->FGtype;
				break;
			}
		}
		
	    for(PC_ITER Spt = tmpShape->PointsList->getList().begin(); Spt != tmpShape->PointsList->getList().end(); Spt++)
		    TempPointColl.Addpoint(new SinglePoint(Spt->second->X, Spt->second->Y, Spt->second->Z, Spt->second->R, Spt->second->Pdir));
		HELPEROBJECT->ProfileScanLineArcAction = new LineArcAction();
		switch (currfgtype)
		{
			case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
			case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
			case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
			case RapidEnums::RAPIDFGACTIONTYPE::DETECT_ALLEDGES:
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
			case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES:
				((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->pttype = LineArcAction::ptscollectiontype::EDGE;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN:
			case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH:
			case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX:
			case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX:
			case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO:
			case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN:
				((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->pttype = LineArcAction::ptscollectiontype::FOCUS;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT:
				((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->pttype = LineArcAction::ptscollectiontype::PROBE;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
				((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->pttype = LineArcAction::ptscollectiontype::CROSSHAIR;
				break;
		}
		finalShape = false;
		CalculateDiffShapes(0.003, 2, 0.01, M_PI/36, 0.009, 1, false, false, false);

		MAINDllOBJECT->update_VideoGraphics();
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->LineArcCallback(0.003, 2, 0.01, M_PI/36, 0.009, 1, false, false, false);

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0007", __FILE__, __FUNCSIG__); }
}

//Handle middle mouse down..
void  SM_LineArcFGHandler::MmouseDown(double x, double y)
{
	try
	{  
		if(MAINDllOBJECT->getSurface())
			MAINDllOBJECT->ContextMenuForSfg();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0008", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void  SM_LineArcFGHandler::LmaxmouseDown()
{
	try
	{
		ValidFlag = false;
		AddpointCollectionFG(MAINDllOBJECT->getWindow(0).getUppX());
		HELPEROBJECT->ProfileScanLineArcAction = new LineArcAction();
		finalShape = false;
		CalculateDiffShapes(0.003, 2, 0.01, M_PI/36, 0.009, 1000, false, false, false);
		MAINDllOBJECT->LineArcCallback(0.003, 2, 0.01, M_PI/36, 0.009, 1000, false, false, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0009", __FILE__, __FUNCSIG__); }
}


//Draw the framegrabs.......//
void SM_LineArcFGHandler::draw(int windowno, double WPixelWidth)
{
	try
	{   
		if(!finalShape)
		{
			if(((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.size() > 0)
			{
			  for(map<int, Shape*>::iterator It = ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.begin(); It != ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.end(); It++)
					It->second->drawShape(windowno, WPixelWidth);
			}
		}
		if(windowno == 0)
		{
			if(!ValidFlag) return;
			double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
			double pts[6];
			GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
			GRAFIX->drawRectangle(point1[0] + WPixelWidth, point1[1] - WPixelWidth , point2[0] - WPixelWidth, point2[1] + WPixelWidth, true);
			GRAFIX->translateMatrix(0.0, 0.0, 2.0);
			GRAFIX->SetColor_Double(0, 0, 1);
			GRAFIX->drawRectangle(point1[0], point1[1], point2[0], point2[1]);
			RMATH2DOBJECT->RectangleFGDirection(&point1[0], &point2[0], RectDirection, WPixelWidth, &pts[0]);
			GRAFIX->drawTriangle(pts[0], pts[1], pts[2], pts[3], pts[4], pts[5], true);
			GRAFIX->translateMatrix(0.0, 0.0, -2.0);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0010", __FILE__, __FUNCSIG__); }
}

//Esc key press..
void SM_LineArcFGHandler::EscapebuttonPress()
{
	try
	{
		ValidFlag = false;
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0011", __FILE__, __FUNCSIG__); }
}


void SM_LineArcFGHandler::MouseScroll(bool flag)
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0012", __FILE__, __FUNCSIG__); }
}

void SM_LineArcFGHandler::AddpointCollectionFG(double WPixelWidth)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX(),upp_y = MAINDllOBJECT->getWindow(0).getUppY();
		bool FrameGrabDone = false;
		MAINDllOBJECT->SetImageInEdgeDetection();
		
		int RectFGArray[4] = {(PointsDRO[0].getX() - cx)/ upp_x, (cy - PointsDRO[0].getY())/upp_y, 
								abs(PointsDRO[0].getX() - PointsDRO[1].getX())/upp_x, abs(PointsDRO[0].getY() - PointsDRO[1].getY())/upp_y};
		int noofpts = EDSCOBJECT->DetectAllProfileEdges(&RectFGArray[0]);
		for(int n = 0; n < noofpts; n++)
		{
			if(!((EDSCOBJECT->DetectedPointsList[n * 2] == -1) && (EDSCOBJECT->DetectedPointsList[n * 2 + 1] == -1)))
				TempPointColl.Addpoint(new SinglePoint((cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x), (cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y), PointsDRO[0].getZ()));
		}
	}	
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0013", __FILE__, __FUNCSIG__); }
}

bool SM_LineArcFGHandler::CalculateDiffShapes(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts)
{
	try
	{
			((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->lineArcSep(&TempPointColl, tolerance, max_dist_betn_pts, this->sort_pts_for_linearcsep, max_radius, min_radius, min_angle_cutoff, noise, false, false, closed_loop, shape_as_fast_trace, join_all_pts);
	        return (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.size() > 0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0014", __FILE__, __FUNCSIG__); }
}


void SM_LineArcFGHandler::AddFinalShapes(bool PerimeterShapeFlag)
{
	try
	{
	    finalShape = true;
		MAINDllOBJECT->getActionsHistoryList().addItem(((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction));
		//delete selected shapes..
		MAINDllOBJECT->deleteShape();
		((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection.clear();
		int i = 0;
		for(map<int, Shape*>::iterator It = ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.begin(); It != ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.end(); It++)
		{
			AddShapeAction::addShape(It->second, false);
			((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection[i] = It->second;
			i++;
		}
		FinishedCurrentDrawing = true;
	    MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0015", __FILE__, __FUNCSIG__); }	
}  


void SM_LineArcFGHandler::ClearLineArc()
{
	try
	{
 		if(HELPEROBJECT->ProfileScanLineArcAction != NULL)
		{	
			((LineArcAction *)HELPEROBJECT->ProfileScanLineArcAction)->deleteAll();
			HELPEROBJECT->ProfileScanLineArcAction = NULL;
		}
		delete HELPEROBJECT->ProfileScanLineArcAction;
		FinishedCurrentDrawing = true;
	    MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0016", __FILE__, __FUNCSIG__); }
}

void SM_LineArcFGHandler::ChangeDirection()
{
	try
	{
		if(RectDirection < 3) RectDirection++;
		else RectDirection = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_LAFH0017", __FILE__, __FUNCSIG__); }
}