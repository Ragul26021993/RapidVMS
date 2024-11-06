#pragma once
#include "stdafx.h"
#include <process.h>
#include<stdio.h>
#include "ThreadCalcFunctions.h"
#include "..\Engine\RCadApp.h"
#include "..\Measurement\DimThread.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Handlers\MouseHandler.h"
#include "..\Engine\ThreadCalcFunctions.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\ShapeWithList.h"

//semaphores - the graphics thread doesn't loop unnecessarily they wait till next SetEvent..
HANDLE Thread_Semaphore;

//Handle Error callback from Graphcis dll.....
void ThreadGraphicsError(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}


//Thread for the graphics on video
void ThreadGraphics_Thread(void *anything)
{
	try
	{
		//Create a semaphore for Thread graphics thread
		Thread_Semaphore = CreateEvent(NULL, TRUE, FALSE, _T("ThreadGraphics"));
		//The graphics thread starts here
		while(MAINDllOBJECT->ThreadRunning)
		{
			try
			{
				THREADCALCOBJECT->Draw_ThreadWindowGraphics();
				WaitForSingleObject(Thread_Semaphore, INFINITE); 
				ResetEvent(Thread_Semaphore);
			}
			catch(...)
			{ 
				THREADCALCOBJECT->DontUpdatethreadGraphics(false); THREADCALCOBJECT->Thread_GraphicsUpdating = false;
				MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0001", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		_endthread();
	}
	catch(...)
	{ 
		THREADCALCOBJECT->DontUpdatethreadGraphics(false); THREADCALCOBJECT->Thread_GraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0002", __FILE__, __FUNCSIG__); 
		_endthread();
	}
}

//Constructor..
ThreadCalcFunctions::ThreadCalcFunctions()
{
	try
	{
		ThCnt = 0;
		ClearAllThread(false); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
ThreadCalcFunctions::~ThreadCalcFunctions()
{
	try
	{
		update_ThreadGraphics();
		Wait_ThreadGraphicsUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0003", __FILE__, __FUNCSIG__); }
}


//Thread left moouse down..
void ThreadCalcFunctions::OnLeftMouseDown_Thread(double x, double y)
{
	try
	{
		RWindow& ThreadWindow = MAINDllOBJECT->getWindow(4);
		ThreadWindow.mouseDown(x, y);
		if(nearestThreadShape != NULL)
		{
			SelectThreadShape(nearestThreadShape->getId());
			ThreadShape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0005", __FILE__, __FUNCSIG__); }
}

//Thread window right mouse down..
void ThreadCalcFunctions::OnRightMouseDown_Thread(double x, double y)
{
	try
	{
		if(MAINDllOBJECT->getCurrentHandler()->getClicksDone() == 0)
		{
			CurrentSelectedThreadAction->IncrementShapePointer();
			ThreadShape_Updated();
			MAINDllOBJECT->SetStatusCode("ThreadCalcFunctions01");
		}
		else
			MAINDllOBJECT->getCurrentHandler()->RmouseDown(MAINDllOBJECT->getWindow(0).getPointer().x, MAINDllOBJECT->getWindow(0).getPointer().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0006", __FILE__, __FUNCSIG__); }
}

//Thread window middle mouse down..
void ThreadCalcFunctions::OnMiddleMouseDown_Thread(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0007", __FILE__, __FUNCSIG__); }
}

//Thread window left mouse up..
void ThreadCalcFunctions::OnLeftMouseUp_Thread(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0008", __FILE__, __FUNCSIG__); }
}

//Thread window right mouse up..
void ThreadCalcFunctions::OnRightMouseUp_Thread(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0009", __FILE__, __FUNCSIG__); }
}

//Thread window middle mouse up..
void ThreadCalcFunctions::OnMiddleMouseUp_Thread(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0010", __FILE__, __FUNCSIG__); }
}

//Thread window mouse move..
void ThreadCalcFunctions::OnMouseMove_Thread(double x, double y)
{
	try
	{
		RWindow& ThreadWindow = MAINDllOBJECT->getWindow(4);
		ThreadWindow.mouseMove(x, y);
		double CurrentMousePosX = ThreadWindow.getPointer().x, CurrentMousePosY = ThreadWindow.getPointer().y;
		this->ThreadShapeHighlighted(false);
		Shape* nshape = NULL; 	
		if(CurrentSelectedThreadAction != NULL)
			nshape = HELPEROBJECT->getNearestShapeOnVideo(CurrentSelectedThreadAction->ShapeCollection_ThreadWindow, CurrentMousePosX, CurrentMousePosY, SNAPDIST * ThreadWindow.getUppX());
		if(nshape != NULL)
		{
			this->ThreadShapeHighlighted(true);
			if(this->nearestThreadShape != NULL)
				this->nearestThreadShape->HighlightedFormouse(false);
			this->nearestThreadShape = nshape;
			this->nearestThreadShape->HighlightedFormouse(true);
			this->ThreadGraphics_Updated(true);
		}	
		else if(this->nearestThreadShape != NULL)
		{
			this->nearestThreadShape->HighlightedFormouse(false);
			this->nearestThreadShape = NULL;
			this->ThreadGraphics_Updated(true);
		}
		update_ThreadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0011", __FILE__, __FUNCSIG__); }
}

//Thread window mouse scroll..
void ThreadCalcFunctions::OnMouseWheel_Thread(int evalue)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0012", __FILE__, __FUNCSIG__); }
}

//Thread window mouse Enter..
void ThreadCalcFunctions::OnMouseEnter_Thread()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0013", __FILE__, __FUNCSIG__); }
}

//Thread window mouse leave..
void ThreadCalcFunctions::OnMouseLeave_Thread()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0014", __FILE__, __FUNCSIG__); }
}

//Initialise OpenGl windows..
void ThreadCalcFunctions::InitialiseThreadOGlWindow(HWND handle, int width, int height)
{
	try 
	{
		RWindow& ThreadWindow = MAINDllOBJECT->getWindow(4);
		ThreadWindow.init(handle, 13, 13, 0.75, width, height);
		ThGraphics = new RGraphicsWindow();
		ThGraphics->GraphicsDLLError = &ThreadGraphicsError;
		ThGraphics->addWindow(ThreadWindow.getHandle(), ThreadWindow.getWinDim().x, ThreadWindow.getWinDim().y, 0.2f, 0.2f, 0.2f);
		ThreadWindow.WindowPanned(true);
		ThreadWindow.WindowResized(true);
		Draw_ThreadWindowGraphics();
		_beginthread(&ThreadGraphics_Thread, 0, NULL);
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0015", __FILE__, __FUNCSIG__); }
}

//Add new Thread Measurement..
void ThreadCalcFunctions::AddNewThreadMeasurement(bool TopSurfaceFlatStatus, bool RootSurfaceFlatStatus)
{
	try
	{
		this->TopSurfaceFlat(TopSurfaceFlatStatus);
		this->RootSurfaceFlat(RootSurfaceFlatStatus);
		this->ThreadMeasurementMode(true);
		DimBase* Cdim = new DimThread(_T("Thread"));
		((DimThread*)Cdim)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
		AddThreadMeasureAction::addDim(Cdim);
		CurrentSelectedThreadAction = ((DimThread*)Cdim)->PointerToAction;
		CurrentSelectedThreadAction->IncludeMajorMinorDia(IncludeMajorMinroDiaMeasure());
		CurrentSelectedThreadAction->ExternalThreadMeasurement(ExternalThreadMeasure());
		CurrentSelectedThreadAction->TopSurfaceFlat(this->TopSurfaceFlat());
		CurrentSelectedThreadAction->RootSurfaceFlat(this->RootSurfaceFlat());
		InitialiseThreadShapes(3, this->TopSurfaceFlat(), this->RootSurfaceFlat());
		MAINDllOBJECT->selectMeasurement(Cdim->getId(),false);
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0016", __FILE__, __FUNCSIG__); }
}

//Initialise thread shapes..
void ThreadCalcFunctions::InitialiseThreadShapes(int threadcount, bool TopSurfaceFlatStatus, bool RootSurfaceFlatStatus, bool PartProgramLoad)
{
	try
	{
		if(CurrentSelectedThreadAction == NULL) return;
		this->TopSurfaceFlat(TopSurfaceFlatStatus);
		this->RootSurfaceFlat(RootSurfaceFlatStatus);
		int ShapeCnt = 0;
		CurrentSelectedThreadAction->ResetAllActions(PartProgramLoad);
		CurrentSelectedThreadAction->ThreadCount(threadcount);
		CurrentSelectedThreadAction->TopSurfaceFlat(TopSurfaceFlatStatus);
		CurrentSelectedThreadAction->RootSurfaceFlat(RootSurfaceFlatStatus);
		int ActId = CurrentSelectedThreadAction->getId();
		DontUpdatethreadGraphics(true);
		MAINDllOBJECT->dontUpdateGraphcis = true;
		int ThreadGap = 800/threadcount;
		int x1, y1, x2, y2, x3, y3;
		int ThreadGap1 = ThreadGap + ThreadGap;
		int Xstart = -1500;
		int YBottom = 400, YTop = 1100;
		int Gap = 20;
		for(int i = 0; i < threadcount; i++)
		{
			x1 = Xstart + 2 * i * ThreadGap1; y1 = YBottom;
			x2 = Xstart + ThreadGap +  2 * i * ThreadGap1; y2 = YTop;
			CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
			CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);

			if(this->TopSurfaceFlat())
			{
				ShapeCnt++;
				x1 = Xstart + ThreadGap +  2 * i * ThreadGap1; y1 = YTop;
				x2 = Xstart + ThreadGap1 + 2 * i * ThreadGap1; y3 = YTop;
				CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
				CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);
			}
			else
			{
				ShapeCnt++;
				x1 = Xstart + ThreadGap +  2 * i * ThreadGap1; y1 = YTop;
				x2 = x1 + ThreadGap/2; y2 = YTop + ThreadGap/5;
				x3 = Xstart + ThreadGap1 + 2 * i * ThreadGap1; y3 = YTop;
				CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, true), false);
				CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, false), false);
			}

			ShapeCnt++;
			x1 = Xstart + ThreadGap1 + 2 * i * ThreadGap1; y1 = YTop;
			x2 = Xstart + ThreadGap1 + ThreadGap + 2 * i * ThreadGap1; y2 = YBottom;
			CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
			CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);
			if(i != threadcount - 1)
			{
				if(this->RootSurfaceFlat())
				{
					ShapeCnt++;
					x1 = Xstart + ThreadGap1 + ThreadGap + 2 * i * ThreadGap1; y1 = YBottom;
					x2 = Xstart + 2 * (i + 1) * ThreadGap1; y3 = YBottom;
					CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
					CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);
				}
				else
				{
					ShapeCnt++;
					x1 = Xstart + ThreadGap1 + ThreadGap + 2 * i * ThreadGap1; y1 = YBottom;
					x2 = x1 + ThreadGap/2; y2 = YBottom - ThreadGap/5;
					x3 = Xstart + 2 * (i + 1) * ThreadGap1; y3 = YBottom;
					CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, true), false);
					CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, false), false);
				}
			}
			ShapeCnt++;
		}
		for(int i = 0; i < threadcount; i++)
		{
			x1 = Xstart + 2 * i * ThreadGap1; y1 = -YBottom;
			x2 = Xstart + ThreadGap +  2 * i * ThreadGap1; y2 = -YTop;
			CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
			CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);

			if(this->TopSurfaceFlat())
			{
				ShapeCnt++;
				x1 = Xstart + ThreadGap +  2 * i * ThreadGap1; y1 = -YTop;
				x2 = Xstart + ThreadGap1 + 2 * i * ThreadGap1; y3 = -YTop;
				CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
				CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);
			}
			else
			{
				ShapeCnt++;
				x1 = Xstart + ThreadGap +  2 * i * ThreadGap1; y1 = -YTop;
				x2 = x1 + ThreadGap/2; y2 = -(YTop + ThreadGap/5);
				x3 = Xstart + ThreadGap1 + 2 * i * ThreadGap1; y3 = -YTop;
				CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, true), false);
				CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, false), false);
			}

			ShapeCnt++;
			x1 = Xstart + ThreadGap1 + 2 * i * ThreadGap1; y1 = -YTop;
			x2 = Xstart + ThreadGap1 + ThreadGap + 2 * i * ThreadGap1; y2 = -YBottom;
			CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
			CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);
			if(i != threadcount - 1)
			{
				if(this->RootSurfaceFlat())
				{
					ShapeCnt++;
					x1 = Xstart + ThreadGap1 + ThreadGap + 2 * i * ThreadGap1; y1 = -YBottom;
					x2 = Xstart + 2 * (i + 1) * ThreadGap1; y3 = -YBottom;
					CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, true), false);
					CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(LineForThread(x1, y1, x2, y2, ShapeCnt, ActId, false), false);
				}
				else
				{
					ShapeCnt++;
					x1 = Xstart + ThreadGap1 + ThreadGap + 2 * i * ThreadGap1; y1 = -YBottom;
					x2 = x1 + ThreadGap/2; y2 = -(YBottom - ThreadGap/5);
					x3 = Xstart + 2 * (i + 1) * ThreadGap1; y3 = -YBottom;
					CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, true), false);
					CurrentSelectedThreadAction->ShapeCollection_Actions.addItem(ArcForThread(x1 + Gap, y1, x2, y2, x3 - Gap, y3, ShapeCnt, ActId, false), false);
				}
			}
			ShapeCnt++;
		}
		if(!PartProgramLoad)
			CreateShapes_MainCollection();
		CurrentSelectedThreadAction->SelectTheFirstShape();
		DontUpdatethreadGraphics(false);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->UpdateShapesChanged();
		ThreadShape_Updated();
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; DontUpdatethreadGraphics(false); MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0017", __FILE__, __FUNCSIG__); }
}

////Line for the thread graphics..
//Shape* ThreadCalcFunctions::LineForThread(double x1, double y1, double x2, double y2, int id, int Actid, bool forthreadwindow)
//{
//	try
//	{
//		if(forthreadwindow)
//		{
//			Line* myLine  = new Line();
//			myLine->ShapeType = RapidEnums::SHAPETYPE::LINE;
//			myLine->LineType = RapidEnums::LINETYPE::FINITELINE;
//			myLine->ShapeForThreadMeasurement(true);	
//			myLine->Normalshape(true);
//			myLine->IsValid(true);
//			AddShapeAction::addShape(myLine, false);
//		}
//		Shape* Cshape = new Line(false);
//		((Line*)Cshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
//		RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
//		item--;
//		Shape* shp = (Shape*)(*item).second;
//		Cshape->setId(shp->getId());
//		Cshape->UcsId(shp->UcsId());
//		Cshape->ShapeForThreadMeasurement(true);
//		Cshape->selected(false, false);
//		Cshape->IsValid(forthreadwindow);
//		if(forthreadwindow)
//		{
//			PointCollection Pointlist;
//			Pointlist.Addpoint(new SinglePoint(x1, y1, 0));
//			Pointlist.Addpoint(new SinglePoint(x2, y2, 0));
//			((ShapeWithList*)Cshape)->AddPoints(&Pointlist);
//			Cshape->ShapeThickness(2);
//		}
//		return Cshape;
//	}
//	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0018", __FILE__, __FUNCSIG__); return NULL; }
//}

//Line for the thread graphics..
Shape* ThreadCalcFunctions::LineForThread(double x1, double y1, double x2, double y2, int id, int Actid, bool forthreadwindow)
{
	try
	{
		Shape* Cshape = new Line(false);
		((Line*)Cshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
		Cshape->setId(id);
		Cshape->UcsId(Actid);
		Cshape->ShapeForThreadMeasurement(true);
		Cshape->selected(false, false);
		Cshape->IsValid(forthreadwindow);
		if(forthreadwindow)
		{
			PointCollection Pointlist;
			Pointlist.Addpoint(new SinglePoint(x1, y1, 0));
			Pointlist.Addpoint(new SinglePoint(x2, y2, 0));
			((ShapeWithList*)Cshape)->AddPoints(&Pointlist);
			Cshape->ShapeThickness(2);
		}
		return Cshape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0018", __FILE__, __FUNCSIG__); return NULL; }
}

//Arc for thread shape..
Shape* ThreadCalcFunctions::ArcForThread(double x1, double y1, double x2, double y2, double x3, double y3, int id, int Actid, bool forthreadwindow)
{
	try
	{
		Shape* Cshape = new Circle(false, RapidEnums::SHAPETYPE::ARC);
		((Circle*)Cshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
		Cshape->setId(id);
		Cshape->UcsId(Actid);
		Cshape->ShapeForThreadMeasurement(true);
		Cshape->selected(false, false);
		if(forthreadwindow)
		{
			PointCollection Pointlist;
			Pointlist.Addpoint(new SinglePoint(x1, y1, 0));
			Pointlist.Addpoint(new SinglePoint(x2, y2, 0));
			Pointlist.Addpoint(new SinglePoint(x3, y3, 0));		
			((ShapeWithList*)Cshape)->AddPoints(&Pointlist);
			Cshape->ShapeThickness(2);
		}
		Cshape->IsValid(forthreadwindow);
		return Cshape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0020", __FILE__, __FUNCSIG__); return NULL; }
}

//Select thread shape..
void ThreadCalcFunctions::SelectThreadShape(int id)
{
	try
	{
		CurrentSelectedThreadAction->SelecttheCurrentPointer(id);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0022", __FILE__, __FUNCSIG__); }
}

//Update thread grpahics..
void ThreadCalcFunctions::ThreadShape_Updated()
{
	try
	{
		if(!ThreadGraphics_Updated())
			this->ThreadGraphics_Updated(true);
		update_ThreadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0023", __FILE__, __FUNCSIG__); }
}

//build thread graphics list..
void ThreadCalcFunctions::buildThreadShapeList()
{
	try
	{
		RWindow& ThreadWindow = MAINDllOBJECT->getWindow(4);
		GRAFIX->DeleteGlList(13);
		GRAFIX->CreateNewGlList(13);
		if(CurrentSelectedThreadAction != NULL)
		{
			for(RC_ITER item = CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.getList().begin(); item != CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.getList().end(); item++)
			{
				Shape* Cshape = (Shape*)(*item).second;
				if(Cshape->HighlightedFormouse())
					GRAFIX->SetColor_Double(100.0/255.0, 147.0/255.0, 237.0/255.0);
				else if(Cshape->selected())
					GRAFIX->SetColor_Double(1, 0, 1);
				else if(Cshape->FinishedThreadMeasurement())
					GRAFIX->SetColor_Double(0, 1, 0);
				else
					GRAFIX->SetColor_Double(1, 1, 1);
				//Cshape->drawShape(0, ThreadWindow.getUppX());
				Cshape->drawShape(4, ThreadWindow.getUppX());
			}
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0024", __FILE__, __FUNCSIG__); }
}

//Calculate thread measure parameters..
void ThreadCalcFunctions::CalculateThreadParameters()
{
	try
	{
		((DimThread*)CurrentSelectedThreadAction->getDim())->CalculateMeasurement();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0025", __FILE__, __FUNCSIG__); }
}

//Current thread measurement.,
DimBase* ThreadCalcFunctions::CurrentThreadMeasure()
{
	if(CurrentSelectedThreadAction != NULL)
		return CurrentSelectedThreadAction->getDim();
	else
		return NULL;
}

//Update Thread Graphics..
void ThreadCalcFunctions::update_ThreadGraphics()
{
	try
	{ 
		if(DontUpdatethreadGraphics()) return;
		if(Thread_GraphicsUpdating) return;
		SetEvent(Thread_Semaphore); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0026", __FILE__, __FUNCSIG__); }
}

//Render thread window graphics..
void ThreadCalcFunctions::Draw_ThreadWindowGraphics()
{
	try
	{
		RWindow& ThreadWindow = MAINDllOBJECT->getWindow(4);
		Thread_GraphicsUpdating = true;
		ThGraphics->SelectWindow();
		if(ThreadWindow.WindowPanned())
		{
			double ext[4];
			ThreadWindow.getExtents(ext);
			ThGraphics->setView(ext[0], ext[1], ext[2], ext[3], ThreadWindow.getUppX(), ThreadWindow.getCenter().x, ThreadWindow.getCenter().y);
			ThreadWindow.WindowPanned(false);
			if(ThreadWindow.WindowResized())
			{
				ThGraphics->resize((int)ThreadWindow.getWinDim().x, (int)ThreadWindow.getWinDim().y);
				ThreadWindow.WindowResized(false);
			}
		}
		if(ThreadGraphics_Updated())
		{
			buildThreadShapeList();
			ThreadGraphics_Updated(false);
		}
		GRAFIX->CalldedrawGlList(13);
		ThGraphics->render();
		Thread_GraphicsUpdating = false;
	}
	catch(...){ Thread_GraphicsUpdating = false; MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0001", __FILE__, __FUNCSIG__); }
}

//Wait Thread graphics update..
void ThreadCalcFunctions::Wait_ThreadGraphicsUpdate()
{
	ThCnt = 0;
	while(Thread_GraphicsUpdating)
	{
		ThCnt++;
	}
}

//Clear All
void ThreadCalcFunctions::ClearAllThread(bool updategraphics)
{
	try
	{ 
		this->TopSurfaceFlat(false);
		this->RootSurfaceFlat(false);
		this->IncludeMajorMinroDiaMeasure(true);
		this->ExternalThreadMeasure(true);
		this->ThreadMeasurementMode(false);
		this->ThreadMeasurementModeStarted(false);
		this->ThreadMeasurementModeStartedFromBegin(false);
		
		this->ThreadShapeHighlighted(false);
		this->ThreadGraphics_Updated(true);
		this->DontUpdatethreadGraphics(false);
		this->Thread_GraphicsUpdating = false;

		this->CurrentSelectedThreadAction = NULL;
		this->nearestThreadShape = NULL;
		if(updategraphics) 
			update_ThreadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0027", __FILE__, __FUNCSIG__); }
}

void ThreadCalcFunctions::CreateShapes_MainCollection()
{
	try
	{
		ThreadShape_MainCollection_Relation.clear();
		for(RC_ITER i = CurrentSelectedThreadAction->ShapeCollection_Actions.getList().begin(); i != CurrentSelectedThreadAction->ShapeCollection_Actions.getList().end(); i++)
		{
			Shape* Threadshape = (Shape*)((*i).second);
			Shape* Cshape = NULL;
			if(Threadshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				Cshape = new Line(_T("L"));
				((Line*)Cshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
				Cshape->UcsId(MAINDllOBJECT->getCurrentUCS().getId());
				Cshape->ShapeForThreadMeasurement(true);
				Cshape->selected(false);
				Cshape->IsValid(true);
			}
			else
			{
				Cshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				((Circle*)Cshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				Cshape->UcsId(MAINDllOBJECT->getCurrentUCS().getId());
				Cshape->ShapeForThreadMeasurement(true);
				Cshape->selected(false);
				Cshape->IsValid(true);
			}
			MAINDllOBJECT->getShapesList().addItem(Cshape);
			this->ThreadShape_MainCollection_Relation[Threadshape->getId()] = Cshape->getId();
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0028", __FILE__, __FUNCSIG__);
	}
}

void ThreadCalcFunctions::AddPointsToMainCollectionShape(ShapeWithList* shape, bool AddShape)
{
	try
	{
		ShapeWithList* Cshape = NULL;
		for(std::map<int, int>::iterator itr = THREADCALCOBJECT->ThreadShape_MainCollection_Relation.begin(); itr != THREADCALCOBJECT->ThreadShape_MainCollection_Relation.end(); itr++)
		{
			int Main_Coll_ShapeId = (*itr).second;
			if(shape->getId() == (*itr).first)
			{
				if(MAINDllOBJECT->getShapesList().ItemExists(Main_Coll_ShapeId)) 
				{
					Cshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().getList()[Main_Coll_ShapeId];
					break;
				}
			}
		}
		if(AddShape && Cshape == NULL)
		{
			if(shape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				Cshape = new Line(_T("L"));
				((Line*)Cshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
				Cshape->UcsId(MAINDllOBJECT->getCurrentUCS().getId());
				Cshape->ShapeForThreadMeasurement(true);
				Cshape->selected(false);
				Cshape->IsValid(true);
				MAINDllOBJECT->getShapesList().addItem(Cshape);
				this->ThreadShape_MainCollection_Relation[shape->getId()] = Cshape->getId();
			}
		}
		if(Cshape != NULL)
		{
			Cshape->PointsListOriginal->deleteAll();
			Cshape->PointsList->deleteAll();
			PointCollection PtColl;
			for(PC_ITER Item = shape->PointsListOriginal->getList().begin(); Item != shape->PointsListOriginal->getList().end(); Item++)
			{     
				SinglePoint* Sp = (*Item).second;
				PtColl.Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR));
			}			
			Cshape->AddPoints(&PtColl);
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0029", __FILE__, __FUNCSIG__);
	}
}