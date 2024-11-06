//Generic Module.. Can be used for any application which uses OpenlGL: Window Settings
//Contains: 
//1 OpenGl initialization
//2 Window settings.. viewport.. readpixels..

#include <string>
#include "..\RapidGraphics.h"

class RapidGraphics_API RGraphicsWindow
{
private:
#pragma region Private Variables
	//Handle for device context..
	HDC dc;
	HWND handle;
	HGLRC rc;
	//Flag value used to rotate the viewport..
	bool flagValue;
	double TotalRotated = 0.0;
	//Variable to hold the graphics rotate angle...//
	double RotateAngle;
#pragma endregion 

public:
#pragma region Constructor
	//Constructor.../////Destructor//
	RGraphicsWindow();
	~RGraphicsWindow();
#pragma endregion

#pragma region Error Handling.
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	void (*GraphicsDLLError) (char* Graphicserrorcode, char* GraphicsFileName, char* GraphicsFunName);
#pragma endregion

#pragma region Opengl Initialization
	//************************************************************************************************//
	//Initialize OpenGl window..//
	HGLRC addWindow(HWND handle, double width, double height, float r, float g, float b);
	//Select the current window to be rendered..//
	void SelectWindow();
	//render the current window
	void render(bool swapbuffers = true); 
	//Set the window view port....//
	void setViewEx(double left, double right, double bottom, double top);
	//Set the window view port and pixel width, center position of the window...
	void setView(double left, double right, double bottom, double top, double upp, double cx, double cy, bool inverted = false);
	//resize the current window.. reset the viewport..
	void resize(UINT32 width, UINT32 height, UINT32 srcL = 0, UINT32 srcR = 0);
	//Read pixels used to save the Rwindow image...//
	void readPixels(void* destination, UINT32 camW = -1, UINT32 camH = -1);
	void readPixelsDigitalZoom(void* destination);
	void glReadVideoPixels(void* destination);
	//Read the image pixels..//
	BYTE* readPixelsEx();
	//Clear the current window..//
	void clearScreen();
	//Rotate the graphics....//
	void RotateGraphics(double a, double x, double y, double z, double px, double py, double pz, double *MMatrix);
	void RotateGrWindow(double *MMatrix);
	//********************************************************************************************************************// 
#pragma endregion


#pragma region Graphics Rotation & translation fuction
	//***********************************************************************************************//
	//Set and get the flag for UCS rotate....//
	bool getRotate();
	//Set the Rotate flag...
	void getRotate(bool);
	//Rotate the graphics...//
	void RotateGL();
	//Set the Rotate angle...//
	void RotateGL(double angle);
	//Translate the graphics to (x,y,z)...
	void translateMatrix(double x, double y, double z);
	//Straighten Graphics
	void UndoAllRotations();
	//************************************************************************************************//
#pragma endregion 

};