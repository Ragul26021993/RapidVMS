#include "StdAfx.h"
#include "ZoomInOutHandler.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\RCadApp.h"
//Constructor..///
//Set the rectangle type..//
ZoomInOutHandler::ZoomInOutHandler(bool zoomin)
{
	try
	{
		setMaxClicks(2);
		Zoomin_outflag = zoomin;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0001", __FILE__, __FUNCSIG__); }
} 

//Destructor..//
ZoomInOutHandler::~ZoomInOutHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0002", __FILE__, __FUNCSIG__); }
}

//Handle Mouse down..///
void ZoomInOutHandler::LmouseDown()
{
	try
	{
		switch(MAINDllOBJECT->getCurrentWindow())
		{
		case 1:
			if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN && MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			else if(MAINDllOBJECT->RcadWindow3DMode())
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			break;
		case 2:
			if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DXFZOOMIN && MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DXFZOOMOUT)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			break;
		default:
			setClicksDone(getClicksDone() - 1);
			return;
			break;
		}

		if(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).PanMode)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(getClicksDone() == 1)
		{
			point[0].set(pt->getX(), pt->getY(), pt->getZ());
			Mpoint[0].set(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).GetMouseDown().x,MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).GetMouseDown().y);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0003", __FILE__, __FUNCSIG__); }
}

void ZoomInOutHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0004", __FILE__, __FUNCSIG__); }
}

//Draw the rectangle.....//
void ZoomInOutHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 0) return;
		if(this->getClicksDone() == 1)
			GRAFIX->drawRectangle(point[0].getX(), point[0].getY(), getClicksValue(1).getX(), getClicksValue(1).getY());
		//RCADGRAFIX->drawRectangle(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0005", __FILE__, __FUNCSIG__); }
}

void ZoomInOutHandler::LmouseUp(double x, double y)
{
	try
	{
		this->LmouseDown_x(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0006", __FILE__, __FUNCSIG__); }
}


void ZoomInOutHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0007", __FILE__, __FUNCSIG__); }
}

//Max clcik...
void ZoomInOutHandler::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		point[1].set(pt->getX(), pt->getY(), pt->getZ());
		Mpoint[1].set(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).GetMouseDown().x,MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).GetMouseDown().y);

		double Zoomarea = abs((point[0].getX() - point[1].getX()) * (point[0].getY() - point[1].getY()));
		double RcadSize = (MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getViewDim().x * MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getViewDim().y);
		if((Zoomarea/RcadSize) < 0.01) return;
		if((Zoomarea/RcadSize) > 1) return;
		if(Zoomin_outflag)
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).changeZoom_Box(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX() * abs(2 * Zoomarea/RcadSize), (Mpoint[0].getX() + Mpoint[1].getX())/2, (Mpoint[0].getY() + Mpoint[1].getY())/2);
		else
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).changeZoom_Box(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX() * abs(RcadSize/(2 * Zoomarea)), (Mpoint[0].getX() + Mpoint[1].getX())/2, (Mpoint[0].getY() + Mpoint[1].getY())/2);
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMIN || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMOUT)
			MAINDllOBJECT->DXFShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RECTMH0008", __FILE__, __FUNCSIG__); }
}