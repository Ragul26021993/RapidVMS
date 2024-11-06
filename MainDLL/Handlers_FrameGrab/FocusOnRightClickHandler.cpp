 #include "StdAfx.h"
#include "FocusOnRightClickHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\FocusFunctions.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include <process.h>
#include "..\Helper\Helper.h"

//Default constuctor..
FocusOnRightClickHandler::FocusOnRightClickHandler()
{
	try
	{
		init();
		setMaxClicks(2);		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
FocusOnRightClickHandler::~FocusOnRightClickHandler()
{
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void FocusOnRightClickHandler::init()
{
	try
	{
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0004", __FILE__, __FUNCSIG__); }
}

void FocusOnRightClickHandler::mouseMove()
{
}

//Left mouse down handling
void FocusOnRightClickHandler::LmouseDown()
{
	try
	{
		setClicksDone(getClicksDone() - 1);
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0006", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void FocusOnRightClickHandler::EscapebuttonPress()
{
	
}

//draw Functions.. draw the flexible crosshair at mouse position..
void FocusOnRightClickHandler::draw(int windowno, double WPixelWidth)
{
}

void FocusOnRightClickHandler::LmaxmouseDown()
{
}

void FocusOnRightClickHandler::MouseScroll(bool flag)
{
}

void FocusOnRightClickHandler::RmouseDown(double x, double y)
{
	try
	{	int correctionX = MAINDllOBJECT->getWindow(0).getWinDim().x / 2;
		int correctionY = MAINDllOBJECT->getWindow(0).getWinDim().y / 2;
		double tmpPoints[2] = {x - MAINDllOBJECT->getCurrentDRO().getX(), y - MAINDllOBJECT->getCurrentDRO().getY()};
		double wupp = MAINDllOBJECT->getWindow(0).getUppX();
		if(MAINDllOBJECT->getSurface())
		{
			FOCUSCALCOBJECT->setRowColumn(1, 1, 0, 40, 40);
			if(FOCUSCALCOBJECT->Cvalue != NULL) { free(FOCUSCALCOBJECT->Cvalue); FOCUSCALCOBJECT->Cvalue = NULL;}
				FOCUSCALCOBJECT->Cvalue = (double*)calloc(1, sizeof(double));
			if(FOCUSCALCOBJECT->Rectptr != NULL){ free(FOCUSCALCOBJECT->Rectptr);  FOCUSCALCOBJECT->Rectptr = NULL;}
				FOCUSCALCOBJECT->Rectptr = (int*)calloc(4, sizeof(int)); 
			int counter = 0;
			FOCUSCALCOBJECT->Rectptr[counter++] = int(tmpPoints[0] / wupp)  + correctionX;
			FOCUSCALCOBJECT->Rectptr[counter++] = correctionY - int(tmpPoints[1] / wupp);
			FOCUSCALCOBJECT->Rectptr[counter++] = FOCUSCALCOBJECT->focusRectangleWidth;
			FOCUSCALCOBJECT->Rectptr[counter++] = FOCUSCALCOBJECT->focusRectangleHeight;
		}
		else if(MAINDllOBJECT->getProfile())
		{
			MAINDllOBJECT->SetImageInEdgeDetection();
			int RectFGArray[4] = {0,0,FOCUSCALCOBJECT->focusRectangleWidth * HELPEROBJECT->CamSizeRatio, FOCUSCALCOBJECT->focusRectangleHeight * HELPEROBJECT->CamSizeRatio}, FrameGrabBox[4] = {100,100,500,400}, pntCnt = 0;
			pntCnt = EDSCOBJECT->DetectAllProfileEdges(FrameGrabBox);
			if(pntCnt <= 0) return;
			double myPoint[2] = {int(tmpPoints[0] / wupp) , int(tmpPoints[1] / wupp) };
			double dist = 10000;
			for(int n = 0; n < pntCnt; n++)
			{
				double tmpPnt[2] = {EDSCOBJECT->DetectedPointsList[n * 2], EDSCOBJECT->DetectedPointsList[n * 2 + 1]};
				if(tmpPnt[0] == -1) continue;
				double tmpDist = RMATH2DOBJECT->Pt2Pt_distance(tmpPnt, myPoint);
				if(tmpDist < dist)
				{
				    dist = tmpDist;
					RectFGArray[0] = tmpPnt[0];
					RectFGArray[1] = tmpPnt[1];
				}
			}
			FOCUSCALCOBJECT->SetAutoFocusRectangle(&RectFGArray[0]);
		}
		MAINDllOBJECT->DoRightClickFocus();
		resetClicks();
	}
	catch(...)
	{
	
	}
}

