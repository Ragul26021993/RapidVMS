#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "SelectionHandler.h"
#include "..\Engine\PointCollection.h"

SelectionHandler::SelectionHandler()
{
	try
	{
		setMaxClicks(2);
		MAINDllOBJECT->SetStatusCode("SelectionHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0001", __FILE__, __FUNCSIG__); }
}

SelectionHandler::~SelectionHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0002", __FILE__, __FUNCSIG__); }
}

//For 1st click set drag = true...
void SelectionHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).PanMode)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT)
		{
			if(MAINDllOBJECT->RcadWindow3DMode())
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0003", __FILE__, __FUNCSIG__); }
}

void SelectionHandler::mouseMove()
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
void SelectionHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 1)
		{
    		if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
			{
				int s1[2] = {4,4}, s2[2] = {4,1};
				double transPosMatrix[16] = {0}, InvTransformMatrix[16] = {0}, TransformMatrix[16] = {0}, points13[8] = {getClicksValue(0).getX(), getClicksValue(0).getY(), 0, 1, getClicksValue(1).getX(), getClicksValue(1).getY(), 0, 1}, endp13[8] = {0}, endp24[8] = {0};
				double points24[8] = {getClicksValue(1).getX(), getClicksValue(0).getY(), 0, 1, getClicksValue(0).getX(), getClicksValue(1).getY(), 0, 1};
				MAINDllOBJECT->getWindow(1).getTransformMatrixfor3Drotate(InvTransformMatrix, 2);
				RMATH3DOBJECT->MultiplyMatrix1(InvTransformMatrix, s1, points13, s2, endp13);
				RMATH3DOBJECT->MultiplyMatrix1(InvTransformMatrix, s1, &points13[4], s2, &endp13[4]);
			    RMATH3DOBJECT->MultiplyMatrix1(InvTransformMatrix, s1, points24, s2, endp24);
				RMATH3DOBJECT->MultiplyMatrix1(InvTransformMatrix, s1, &points24[4], s2, &endp24[4]);
				GRAFIX->SetColor_Double(1, 0, 0);
				GRAFIX->drawRectangle3D(endp13[0], endp13[1], endp13[2], endp24[0], endp24[1], endp24[2], endp13[4], endp13[5], endp13[6], endp24[4], endp24[5], endp24[6]);
			}
	    }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0005", __FILE__, __FUNCSIG__); }
}

void SelectionHandler::LmouseUp(double x, double y)
{
}

void SelectionHandler::LmaxmouseDown()
{
	try
	{
		std::list<int> PtIdlist;
		PtIdlist.clear();
		double point1[2] = {getClicksValue(0).getX(), getClicksValue(0).getY()}, point2[2] = {getClicksValue(1).getX(), getClicksValue(1).getY()};
		double topleft[2] = {0}, bottomRight[2] = {0};
		RMATH2DOBJECT->GetTopLeftNBtnRht(&point1[0], &point2[0], &topleft[0], &bottomRight[0]);
		if(MAINDllOBJECT->ShowHideFGPoints())
		{				
			double invTransformMatrix[16] = {0};
			MAINDllOBJECT->getWindow(1).getTransformMatrixfor3Drotate(invTransformMatrix, 0);
			MAINDllOBJECT->selectFGPointsWithTransformation(Vector(topleft[0], topleft[1], 0), Vector(bottomRight[0], bottomRight[1], 0), &PtIdlist, invTransformMatrix);
		}
		else
		{
			MAINDllOBJECT->selectShapesWindow(Vector(topleft[0], topleft[1], 0), Vector(bottomRight[0], bottomRight[1], 0));
			MAINDllOBJECT->selectMeasurewindow(Vector(topleft[0], topleft[1], 0), Vector(bottomRight[0], bottomRight[1], 0));
		}
		resetClicks();
		MAINDllOBJECT->Shape_Updated();
		if(PtIdlist.size() > 0)
			MAINDllOBJECT->ShowFgPtIdList(&PtIdlist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0007", __FILE__, __FUNCSIG__); }

}

void SelectionHandler::RmouseDown(double x, double y)
{
}

void SelectionHandler::MouseScroll(bool flag)
{
}

void SelectionHandler::EscapebuttonPress()
{
	try
	{
		if(MAINDllOBJECT->ShowHideFGPoints())
		{
			ShapeWithList *Cshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			if(Cshape == NULL) return;
			for(PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
				(*Item).second->IsSelected = false;
			Cshape->PointsList->SelectedPointsCnt = 0;
			MAINDllOBJECT->Shape_Updated();
			MAINDllOBJECT->SetStatusCode("Cleared Selection");
		}
		else
		{
			MAINDllOBJECT->getShapesList().clearSelection();
			MAINDllOBJECT->getDimList().clearSelection();
			MAINDllOBJECT->SetStatusCode("Cleared Selection");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0008", __FILE__, __FUNCSIG__); }
}