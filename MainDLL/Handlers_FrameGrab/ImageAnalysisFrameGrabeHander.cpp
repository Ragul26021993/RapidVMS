#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "ImageAnalysisFrameGrabeHander.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Shapes\CloudPoints.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"

ImageAnalysisFrameGrabeHander::ImageAnalysisFrameGrabeHander()
{
	try
	{
		setMaxClicks(2);
		this->ShapeToAddPnts = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		FilterCount = 5;
		MatrixSize = 5;
		ThresholdPixel = 45;
		if(this->ShapeToAddPnts == NULL)
		{
			AddShapeAction::addShape(new CloudPoints());
			ShapeToAddPnts = (ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0001", __FILE__, __FUNCSIG__); }
}

ImageAnalysisFrameGrabeHander::~ImageAnalysisFrameGrabeHander()
{
}

//For 1st click set drag = true...
void ImageAnalysisFrameGrabeHander::LmouseDown()
{
	try
	{
		if(ShapeToAddPnts == NULL) 
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0003", __FILE__, __FUNCSIG__); }
}

void ImageAnalysisFrameGrabeHander::mouseMove()
{
	try
	{
		if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
		{
			PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0004", __FILE__, __FUNCSIG__); }
}

//Draw the rectangle till drag is true... till mouse up..!!
void ImageAnalysisFrameGrabeHander::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 0)
		{
			if(getClicksDone() > 0)
			{
				double wupp = WPixelWidth;
				GRAFIX->SetColor_Double(1, 0, 0);
				double Points[4] = {getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY()};
				GRAFIX->drawRectangle(Points[0], Points[1], Points[2], Points[3]);				
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0005", __FILE__, __FUNCSIG__); }
}

void ImageAnalysisFrameGrabeHander::LmouseUp(double x, double y)
{
}

void ImageAnalysisFrameGrabeHander::LmaxmouseDown()
{
	try
	{
		if(ShapeToAddPnts == NULL) 
		{
			setClicksDone(getMaxClicks() - 1);
			return;
		}	
		double wupp = MAINDllOBJECT->getWindow(0).getUppX();
		int correctionX = MAINDllOBJECT->getWindow(0).getWinDim().x / 2;
		int correctionY = MAINDllOBJECT->getWindow(0).getWinDim().y / 2;
		PointsDRO[getMaxClicks() - 1].set(getClicksValue(getMaxClicks() - 1));
		double tmpPoints[4] = {PointsDRO[0].getX() - MAINDllOBJECT->getCurrentDRO().getX(), PointsDRO[0].getY() - MAINDllOBJECT->getCurrentDRO().getY(), PointsDRO[1].getX() - MAINDllOBJECT->getCurrentDRO().getX(), PointsDRO[1].getY() - MAINDllOBJECT->getCurrentDRO().getY()};
		int pointsPixel[4] = {0};
		pointsPixel[0] = int(tmpPoints[0]/wupp + correctionX);
		pointsPixel[2] = int(correctionY - tmpPoints[1]/wupp);
		pointsPixel[1] = int(tmpPoints[2]/wupp + correctionX);
		pointsPixel[3] = int(correctionY - tmpPoints[3]/wupp);
		list<double> ptList;
		MAINDllOBJECT->SetImageInEdgeDetection();
		EDSCOBJECT->ImageAnalysis(pointsPixel, 7, 6, 60, &ptList);
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX(), upp_y = MAINDllOBJECT->getWindow(0).getUppY();
		int NoOfPoints = ptList.size();
		PointCollection ptcol;
		double x = 0;
		SinglePoint *lastPoint = NULL; 
		for(list<double>::iterator It = ptList.begin(); It != ptList.end(); It++)
		{
			x = *It; It++;
			if(It == ptList.end()) break;
			SinglePoint *Spt = new SinglePoint(cx + x * upp_x, cy - *It * upp_y, 0);
			//if(lastPoint != NULL)
			//{
			//	if(RMATH2DOBJECT->Pt2Pt_distance(lastPoint->X, lastPoint->Y, Spt->X, Spt->Y) < 20)
				  ptcol.Addpoint(Spt);
				/*else if(ptcol.Pointscount() > 1)
				{
					((ShapeWithList*)ShapeToAddPnts)->AddPoints(&ptcol);
				}
			}
			if(lastPoint == NULL)
				lastPoint = Spt;*/
		}
		((ShapeWithList*)ShapeToAddPnts)->AddPoints(&ptcol);
		MAINDllOBJECT->ZoomToWindowExtents(1);
		resetClicks();	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0007", __FILE__, __FUNCSIG__); }

}

void ImageAnalysisFrameGrabeHander::RmouseDown(double x, double y)
{
}

void ImageAnalysisFrameGrabeHander::MouseScroll(bool flag)
{
}

void ImageAnalysisFrameGrabeHander::EscapebuttonPress()
{
	resetClicks();
}

void ImageAnalysisFrameGrabeHander::PartProgramData()
{
}

