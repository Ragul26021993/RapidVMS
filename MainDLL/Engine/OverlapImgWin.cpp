//Include the header files...//
#include "stdafx.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <process.h>
#include "OverlapImgWin.h"
#include "..\Shapes\Shape.h"
#include "..\Measurement\DimBase.h"
#include "..\Handlers\MouseHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\MainDLL.h"
#include <map>
#include "UCS.h"

//Handle Error callback from Graphcis dll.....
void GraphicsErrorHandler1(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

HANDLE Win_Semaphore;
//Static Instance..
OverlapImgWin* OverlapImgWin::_myinstance = 0;
OverlapImgWin* OverlapImgWin::getMyInstance()
{
	if(_myinstance==NULL)
	{
		_myinstance = new OverlapImgWin();			
	}
	return _myinstance;
}

OverlapImgWin::OverlapImgWin()
{
	try
	{
		this->IsThreadRunning=true;
		ThCnt = 0;
		ClearAllThread(false);
		LoadFirstTime=true;
		Columncount=0;
		Rowcount=0;
		iswindowInitialized=false;
		WinTopX = -2;
		WinTopY = 2;
	}
	catch(...){}
}

OverlapImgWin::~OverlapImgWin()
{	
	 IsThreadRunning = false;
	 update_OverlapImgWinGraphics();
	 Wait_OverlapImgWinGraphicsUpdate();
	 ResetEvent(Win_Semaphore);
	 CloseHandle(Win_Semaphore);
}

void OverlapImgWin::OnLeftMouseDown_OverlapImgWin(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0001", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnRightMouseDown_OverlapImgWin(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0002", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnMiddleMouseDown_OverlapImgWin(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0003", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnLeftMouseUp_OverlapImgWin(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0004", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnRightMouseUp_OverlapImgWin(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0005", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnMiddleMouseUp_OverlapImgWin(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0006", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnMouseMove_OverlapImgWin(double x, double y)
{
	try
	{ 
	MAINDllOBJECT->SetCurrentWindow(3);
	MAINDllOBJECT-> getWindow(3).mouseMove(x, y);
	update_OverlapImgWinGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0007", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnMouseWheel_OverlapImgWin(int evalue)
{
	try
	{
		Wait_OverlapImgWinGraphicsUpdate();
		MAINDllOBJECT->SetCurrentWindow(3);
		MAINDllOBJECT->getWindow(3).changeZoom(evalue>0?false:true);	
		update_OverlapImgWinGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0008", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnMouseEnter_OverlapImgWin()
{
	try{
	MAINDllOBJECT->SetCurrentWindow(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW0009", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::OnMouseLeave_OverlapImgWin()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OIW00010", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::InitialiseOverlapImgOGlWindow(HWND handle, int width, int height)
{
	try
	{		
			RWindow& OverlapImgWindow = MAINDllOBJECT->getWindow(3);
			OverlapImgWindow.init(handle, 0.01, 0.01, 0.94, width, height);		
			//Graphics Initialization....//
			OvImgGraphics = new RGraphicsWindow(); 
			OvImgGraphics->GraphicsDLLError = &GraphicsErrorHandler1;
			//Add the window...
			OvImgGraphics->addWindow(OverlapImgWindow.getHandle(), OverlapImgWindow.getWinDim().x, OverlapImgWindow.getWinDim().y, 0.0f, 0.0f, 0.0f);
			GRAFIX->CreateNewFont(OverlapImgWindow.getHandle(), "Tahoma");
			OverlapImgWindow.WindowPanned(true);	
			this->IsThreadRunning=true;
			Draw_OverlapImgWinWindowGraphics();
			StartOverlapImgWinThread();
			iswindowInitialized=true;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0015", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::Draw_OverlapImgWinWindowGraphics()
{
	try
	{
		GraphicsUpdating = true;
		double WindowUpp;
		RWindow& OverlapImgWindow = MAINDllOBJECT->getWindow(3);

		WindowUpp = OverlapImgWindow.getUppX();		
		OvImgGraphics->SelectWindow();
		
		if(OverlapImgWindow.WindowPanned())
		{
			double ext[4];
			OverlapImgWindow.getExtents(ext);
			OvImgGraphics->setView(ext[0], ext[1], ext[2], ext[3], OverlapImgWindow.getUppX(), OverlapImgWindow.getCenter().x, OverlapImgWindow.getCenter().y);
			GRAFIX->SetWindowSettings(WindowUpp, OverlapImgWindow.getCenter().x,OverlapImgWindow.getCenter().y, 3);

			OverlapImgWindow.WindowPanned(false);
		}
		if(ThreadGraphics_Updated())
		{				
			buildThreadShapeList(_imagepath,284,177,1,1);
			ThreadGraphics_Updated(false);			
		}		
		/*MAINDllOBJECT->getWindow(3).ZoomToExtents(3);*/
		/*_imagepath[0]="D:\\SuperImposeImage\\0.bmp";
		_imagepath[1]="D:\\SuperImposeImage\\1.bmp";
		_imagepath[2]="D:\\SuperImposeImage\\2.bmp";
		_imagepath[3]="D:\\SuperImposeImage\\3.bmp";
		_imagepath[4]="D:\\SuperImposeImage\\4.bmp";
		_imagepath[5]="D:\\SuperImposeImage\\5.bmp";*/
		/*_imagepath[11]="D:\\SuperImposeImage\\6.bmp";
		_imagepath[10]="D:\\SuperImposeImage\\7.bmp";
		_imagepath[9]="D:\\SuperImposeImage\\8.bmp";
		_imagepath[8]="D:\\SuperImposeImage\\9.bmp";
		_imagepath[7]="D:\\SuperImposeImage\\10.bmp";
		_imagepath[6]="D:\\SuperImposeImage\\11.bmp";
		_imagepath[17]="D:\\SuperImposeImage\\12.bmp";
		_imagepath[16]="D:\\SuperImposeImage\\13.bmp";
		_imagepath[15]="D:\\SuperImposeImage\\14.bmp";
		_imagepath[14]="D:\\SuperImposeImage\\15.bmp";
		_imagepath[13]="D:\\SuperImposeImage\\16.bmp";
		_imagepath[12]="D:\\SuperImposeImage\\17.bmp";*/
		GRAFIX->DrawVideoTextureBackGround(_imagepath,800,600,Rowcount,Columncount);
		//GRAFIX->DrawVideoTextureBackGround(_imagepath,800,600,1,6);

		/*OvImgGraphics->translateMatrix(0.0, 0.0, -400);
		GRAFIX->drawFixedAxisBackgroundOverlapImgWin(MAINDllOBJECT->getCurrentUCS().getWindow(0).getViewDim().x,MAINDllOBJECT->getCurrentUCS().getWindow(0).getViewDim().y, -OverlapImgWindow.getpt().x, -OverlapImgWindow.getpt().y);*/
		
		GRAFIX->CalldedrawGlList(16);
				
		OvImgGraphics->render();
		GraphicsUpdating = false;
	}
	catch(...)
	{
		OvWinMouseFlag = false; GraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("OVIMGWIN0001", __FILE__, __FUNCSIG__); 
	}
}

void OverlapImgWin::StartOverlapImgWinThread()
{
	_beginthread(&Graphics_Thread, 0, NULL);
}

void Graphics_Thread(void *anything)
{
	try
	{
		//Create a semaphore for Thread graphics thread
		Win_Semaphore = CreateEvent(NULL, TRUE, FALSE, _T("OVIMGWINGRAPHICS"));
		//The graphics thread starts here
		while(OVERLAPIMAGEWINOBJECT->IsThreadRunning)
		{
			try
			{
				OVERLAPIMAGEWINOBJECT->Draw_OverlapImgWinWindowGraphics();
				WaitForSingleObject(Win_Semaphore, INFINITE); 
				ResetEvent(Win_Semaphore);
			}
			catch(...)
			{ 
				OVERLAPIMAGEWINOBJECT->DontUpdateGraphics(false); OVERLAPIMAGEWINOBJECT->GraphicsUpdating = false;
				MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0001", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		_endthread();
	}
	catch(...)
	{ 
		OVERLAPIMAGEWINOBJECT->DontUpdateGraphics(false); OVERLAPIMAGEWINOBJECT->GraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0002", __FILE__, __FUNCSIG__); 
		_endthread();
	}
}

void OverlapImgWin::buildThreadShapeList(map<int ,std::string> imagepath , int bpwidth, int bpheight, int Rowcount , int Columncount)
{
	try
	{
		GRAFIX->DeleteGlList(16);
		GRAFIX->CreateNewGlList(16);
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0024", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::update_OverlapImgWinGraphics()
{
	try
	{ 
		if(DontUpdateGraphics()) return;
		if(GraphicsUpdating) return;
		SetEvent(Win_Semaphore); 
		Sleep(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0026", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::Wait_OverlapImgWinGraphicsUpdate()
{
	ThCnt = 0;
	while(GraphicsUpdating)
	{
		ThCnt++;
	}
}

void OverlapImgWin::ClearAllThread(bool updategraphics)
{
	try
	{
		this->ThreadGraphics_Updated(true);
		this->DontUpdateGraphics(false);
		this->GraphicsUpdating = false;		
		this->nearestThreadShape = NULL;
		if(updategraphics) 
			update_OverlapImgWinGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THFN0027", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::CloseWindow()
{
	this->IsThreadRunning=false;
	Wait_OverlapImgWinGraphicsUpdate();
	ResetEvent(Win_Semaphore);
	CloseHandle(Win_Semaphore);
	MAINDllOBJECT->getWindow(3).ResetAll();	
	_imagepath.clear();
	Rowcount=0;
	Columncount = 0;
	delete OvImgGraphics;
}

//Calculating Center of window during DRO changes..
void OverlapImgWin::SetcenterScreen_OvImgWin(double x, double y,double z)
{
	try
	{
		MAINDllOBJECT->getWindow(3).centerCam(x - MAINDllOBJECT->getWindow(3).getpt().x - MAINDllOBJECT->getWindow(3).getHomediff().x, y - MAINDllOBJECT->getWindow(3).getpt().y - MAINDllOBJECT->getWindow(3).getHomediff().y);
		MAINDllOBJECT->getWindow(3).WindowPanned(true);
		update_OverlapImgWinGraphics();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OVIMGWIN0005", __FILE__, __FUNCSIG__); }
}

void OverlapImgWin::GridImageCaptured(map<int ,std::string> Imagepath ,double row, double column)
{
	_imagepath=Imagepath;
	Rowcount=row;
	Columncount = column;
	ThreadGraphics_Updated(true);
	update_OverlapImgWinGraphics();	
}

void OverlapImgWin::GetWindowEntityExtents(double *Lefttop, double *Rightbottom)
{
	try
	{
		Lefttop[0] = WinTopX;
		Lefttop[1] = WinTopY;
		double Ratio = (double)800 / 600 ;
		double width =2*abs(WinTopX)/Columncount , height = width/Ratio;	
		Rightbottom[0] = WinTopX + Columncount*width , Rightbottom[1] = WinTopY- Rowcount*height;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("OVIMGWINGRAPHICS", __FILE__, __FUNCSIG__); }
}