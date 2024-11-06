//Class to handle the ImageOverlap Module Flow..
//New thread initialisation..
//Graphics required etc...
//Created on 14/2/2013
#pragma once
#include "RWindow.h"
#include <map>
#define OVERLAPIMAGEWINOBJECT OverlapImgWin::getMyInstance()

class Shape;
class DimBase;

class MAINDLL_API OverlapImgWin
{

private:
	//Constructor ..
	OverlapImgWin();
	//Destructor...
	~OverlapImgWin();
	static OverlapImgWin* _myinstance;
	//Error Handler
	map<int ,std::string> _imagepath ;
	double Columncount,  Rowcount;
	void GraphicsErrorHandler(char* ecode, char* fname, char* funcname);
	bool iswindowInitialized;
	double WinTopX , WinTopY;
public:
	static OverlapImgWin* getMyInstance();
#pragma region All the Flags
	//All the flags used..
	RapidProperty<bool> IncludeMajorMinroDiaMeasure; //Include Major minr dia
	RapidProperty<bool> ExternalThreadMeasure; //External thread measure
	RapidProperty<bool> ThreadMeasurementMode; //Thread Measurement mode
	RapidProperty<bool> ThreadMeasurementModeStarted; //Thread measure mode started
	RapidProperty<bool> ThreadMeasurementModeStartedFromBegin; 
	RapidProperty<bool> ThreadShapeHighlighted;
	RapidProperty<bool> DontUpdateGraphics;
	RapidProperty<bool> ThreadGraphics_Updated;	
	bool OvWinMouseFlag;
	bool IsThreadRunning;
	bool LoadFirstTime;
#pragma endregion

	//To Lock the resource..
	bool GraphicsUpdating;
	int ThCnt;
	friend void Graphics_Thread(void *anything); //Video Graphics...

#pragma region Shape , Graphics
	//Current Thread Shape...
	Shape *nearestThreadShape;
	//Thread Graphics Module Instance..
	RGraphicsWindow* OvImgGraphics;
	//Thread Graphics window..
	/*RWindow& OverlapImgWindow;*/
#pragma endregion

#pragma region Window Mouse Event Handling
	 void OnLeftMouseDown_OverlapImgWin(double x, double y); 
	 void OnRightMouseDown_OverlapImgWin(double x, double y); 
	 void OnMiddleMouseDown_OverlapImgWin(double x, double y); 
	 void OnLeftMouseUp_OverlapImgWin(double x, double y); 
	 void OnRightMouseUp_OverlapImgWin(double x, double y); 
	 void OnMiddleMouseUp_OverlapImgWin(double x, double y); 
	 void OnMouseMove_OverlapImgWin(double x, double y);
	 void OnMouseWheel_OverlapImgWin(int evalue);
	 void OnMouseEnter_OverlapImgWin();
	 void OnMouseLeave_OverlapImgWin();
#pragma endregion

#pragma region Initialisation and Measurement Function
	//Initialise OverlapImage Window..
	void InitialiseOverlapImgOGlWindow(HWND handle, int width, int height);
	//Update Graphics..
	void update_OverlapImgWinGraphics();
	//Wait graphics update..
	void Wait_OverlapImgWinGraphicsUpdate();
	//Render window graphics..
	void Draw_OverlapImgWinWindowGraphics();
	//build thread Graphics..
	void buildThreadShapeList(map<int, std::string> imagepath , int bpwidth, int bpheight, int Rowcount , int Columncount);
	//Clear All
	void ClearAllThread(bool updategraphics = true);
	void StartOverlapImgWinThread();
	//close window
	void CloseWindow();
	//Set center on zoom of the image
	void SetcenterScreen_OvImgWin(double x, double y,double z);
	//Get the image display based on the order at which image is being captured
	void GridImageCaptured(map<int ,std::string> Imagepath ,double column ,double Row);
	// Set the window extent
	void GetWindowEntityExtents(double *Lefttop, double *Rightbottom);
#pragma endregion
};


//Created by Shweytank Mishra.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!