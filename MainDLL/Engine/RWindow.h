// Window class..
// Holds Window properties... 
// Like width, height, camera position, view port.. 
// Pan Shift, zoom factor And Pan, Zoom mode 
// Mouse positions in DRO and pixel coordinates..

#pragma once
#include "..\MainDLL.h"
#include "..\Helper\General.h"
#include "..\RapidGraphics\Engine\RGraphicsWindow.h"

//structor for point....//
//To return the mouse point, window position etc...
struct MAINDLL_API R_Point
{
	double x, y, z;
	R_Point(double PtX = 0, double PtY = 0, double PtZ = 0)
	{
		x = PtX; y = PtY; z = PtZ;
	}
	~R_Point(){	}
};

//Window Class....
class MAINDLL_API RWindow
{
private:
	//Window handle....
	HWND handle;
	//Mouse Positions and Window size in Pixel Coordinates
	UINT32 MmoveX, MmoveY, MdownX, MdownY, WinWidth, WinHeight;
	//Mouse Positions and Window size in DRO coordinates..
	double WMmoveX, WMmoveY, WMdownX, WMdownY, VWinWidth, VWinHeight;
	//Variables used for Panning..
	double LastWMmoveX, LastWMmoveY, PanShiftX, PanShiftY;
	//Variable to Handle the Home Position set... Store the home position Properties..
	double h_camx, h_camy, h_VWinWidth, h_VWinHeight, h_uppX, h_uppY;
	//Variables to handle the Rotation...
	double RotateX, RotateY, TransformationMatrix[16], InverseMatrix[16], TransposeMatrix[16];
	//Flag to control panning window
	bool panning;
	//Center of rotation
	R_Point CenterOfRotation;
	void UpdateCenterScreen(bool updateVideo = true);
	void UpdateCenterScreen_Zoom(UINT32 windowno);
	void UpdateRcadZoomToExtents();
	void UpdateDxfZoomToExtents();
	void UpdateImageWinZoomToExtent();
	void UpdateWindowView(double* LeftTop, double* RightBottom);
public:
	//Camera Center, pixel width, magnification factor etc...
	double camx, camy, uppX, uppY, uppf;
	
	bool PanMode;
	double camz;
	UINT32 WindowNo;
	//intialize the widow with peoperties..//
	RWindow();
	~RWindow();
	void init(RWindow& win);
	void init(RWindow& win, double upp_x, double upp_y, double uppf);
	void init(HWND winhandle, double upp_x, double upp_y, double uppf, UINT32 width, UINT32 height);
	void initVariables();
	//Resize the window...//
	void ResizeWindow(UINT32 winwidth, UINT32 winheight, bool flag = true);
	//Calculate the mouse position w.r.t dro and returns the mouse position..//
	R_Point mouseMove(UINT32 mx, UINT32 my);
	//Mouse down event...//
	void mouseDown(UINT32 mx, UINT32 my);

	//Toggle mode flag..//
	void togglePan();
	//Returns the pan mode flag..//
	bool GetpanMode();
	//Rotate the CAd window..//
	void RotateGraphics(RGraphicsWindow* WndGraphics);
	//Change the zoom property/ pixel width and view port of the window//
	void changeZoom(bool in);
	//Change the zoom properties of the window...//
	void changeZoom_Video(double pixelwidth_x, double pixelwidth_y);
	//Change the zoom properties of the Rcad...//
	void changeZoom_Box(double pixelwidth, double x, double y);
	//Center of the window....//
	R_Point centerCam(double x, double y);
	//Setting Center of rotation....//
	void SetCenterOfRotation(double x, double y, double z);
	//Returns the window extents...//
	void getExtents(double extents[]);

	void GetSelectionLine(double x, double y, double *Answer);

	R_Point CalculateTransformedCoordinates(double mx, double my);
 	//this is the units per pixel
	double getUppX();
	double getUppY();
	double getOriginalUpp();
	//zoom factor
	double getUppf();
	void CalculateSelectionLine(double* Sline);
	void getTransformMatrixfor3Drotate(double* transform, UINT32 cnt = 1);

	void ZoomToExtents(UINT32 windowno);
	void ResetMouseLastPosition();

	void SetMousemoveFor3Dmode();

	R_Point PixelCalibration_OneShot(double mx, double my, bool digitalZoom = false);

	//returns the mouse point, window center, panned position and window view width, height etc..//
	R_Point GetMouseDown();
	R_Point GetLastMouseDown();
	R_Point GetMouseMove();
	R_Point getCam();
	R_Point getCenter();
	R_Point getViewDim();
	R_Point getWinDim();
	R_Point getPointer(bool flag = false);
	R_Point getpt();
	R_Point getHomediff();
	R_Point getRtranslate();
	HWND getHandle();
	R_Point getLastWinMouse();

	RapidProperty<bool> maxed;
	RapidProperty<bool> WindowPanned;
	RapidProperty<bool> WindowResized;
	RapidProperty<bool> WindowHidden;

	//Set the home position of the window...//
	void SetTransFormationMatrix(double* matrix);
	void ResetPan();
	void homeIt(UINT32 windowno);
	void DontMoveWithDro(UINT32 windowno);
	void ResetAll();
};