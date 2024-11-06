//Include the header files...//
#include "stdafx.h"
#include <string>
#include <dwmapi.h>
#include "RGraphicsWindow.h"
#define _USE_MATH_DEFINES
#include "math.h"

//Window width height..
#define MAX_X 999999
#define MAX_Y 999999
#define MAX_Z 999999

#define MIN_X -999999
#define MIN_Y -999999
#define MIN_Z -999999


//Disable the vSync..disables the framerate restriction.. gives more frame rates...//
//but this may decrease the clarity...!!!!!!!!!!
void setVSync(UINT32 interval = 1)
{
	try
	{
		//API call to disable vertical synchronization..//
		typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
		const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
		if(strstr(extensions, "WGL_EXT_swap_control"))
			return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
		else
		{
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
			if(wglSwapIntervalEXT)
				wglSwapIntervalEXT(interval);
		}
	}
	catch(...){ }
}

RGraphicsWindow::RGraphicsWindow()
{
	this->flagValue = false;
	GraphicsDLLError = NULL;
	this->TotalRotated = 0.0;
}

RGraphicsWindow::~RGraphicsWindow()
{
	try
	{
		//Delete the graphic object and clear the graphics color.....//
		wglDeleteContext(rc);
		glClear(0);
		glClearColor(0,0,0,0);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0002", __FILE__, __FUNCSIG__); }
}

void RGraphicsWindow::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(GraphicsDLLError != NULL)
		GraphicsDLLError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}

// Initialize the opengl window... pass handle, width, height, and background color..//
//vinvert flag decides the presence of video...//
HGLRC RGraphicsWindow::addWindow(HWND handle, double width, double height, float r, float g, float b)
{
	try
	{
		this->handle = handle;
		this->dc = GetDC(handle);
	
		//-----------------------OPENGL initialization code-----------------------
		//set an appropriate pixel format for device context
		PIXELFORMATDESCRIPTOR pfd;
		pfd.nSize = sizeof(pfd);
		//pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL*/;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW |              // support window 
			PFD_SUPPORT_OPENGL |              // support OpenGL 
			PFD_SUPPORT_COMPOSITION |
			PFD_DOUBLEBUFFER;                 // double buffered
		pfd.nVersion = 2;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cAlphaBits = 255;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;
		//pfd.cDepthBits = 255;
		//pfd.cStencilBits = 255;		
		DWORD style = ::GetWindowLong(handle, GWL_STYLE);
		style &= ~WS_OVERLAPPEDWINDOW;
		style |= WS_POPUP;
		::SetWindowLong(handle, GWL_STYLE, style);

		DWM_BLURBEHIND bb = { 0 };
		HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
		bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
		bb.hRgnBlur = hRgn;
		bb.fEnable = TRUE;
		DwmEnableBlurBehindWindow(handle, &bb);


		UINT32 pixelformat = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc, pixelformat, &pfd);
		DescribePixelFormat(dc, pixelformat, sizeof(pfd), &pfd);
		//create the rendering context and make it the current one
		rc = wglCreateContext(dc);
		//-----------------------OPENGL initialization code-----------------------
		wglMakeCurrent(dc, rc);	
		////glEnable(GL_CULL_FACE);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		//
		////glDepthMask(GL_TRUE);
		////set the color used the clear the back buffer - so it becomes background color
		//glClearColor(r, g, b, 0.2f);
		glClearColor(r, g, b, 1.0f);

		//glDisable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		//set the color used the clear the back buffer - so it becomes background color
		
		//GLfloat light_position1[] = {255, 255, 255, 255};
	/*	GLfloat light_position2[] = {Light_Position2[0], Light_Position2[1], Light_Position2[2], Light_Position2[3]};
		GLfloat light_position3[] = {Light_Position3[0], Light_Position3[1], Light_Position3[2], Light_Position3[3]};
		GLfloat light_position4[] = {Light_Position4[0], Light_Position4[1], Light_Position4[2], Light_Position4[3]};
		GLfloat light_position5[] = {Light_Position5[0], Light_Position5[1], Light_Position5[2], Light_Position5[3]};
		GLfloat light_position6[] = {Light_Position6[0], Light_Position6[1], Light_Position6[2], Light_Position6[3]};*/
		//GLfloat mat_diffuse[] = {0, 0, 0, 0};
		//glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
		//glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
	/*	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, mat_diffuse);
		glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, mat_diffuse);
		glLightfv(GL_LIGHT3, GL_POSITION, light_position4);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, mat_diffuse);
		glLightfv(GL_LIGHT4, GL_POSITION, light_position5);
		glLightfv(GL_LIGHT4, GL_DIFFUSE, mat_diffuse);
		glLightfv(GL_LIGHT5, GL_POSITION, light_position6);
		glLightfv(GL_LIGHT5, GL_DIFFUSE, mat_diffuse);*/
		//glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);

	/*	GLfloat mat_ambient[] = { 0.6f, 0.6f, 0.5f, 1.0f};
		GLfloat mat_specular[] = { 1.0f, 0.0f, 0.5f, 1.0f };
		GLfloat mat_shininess[] = { 100.0f };
		GLfloat light_position[] = {0, 0, 10000, 0.0f };
		GLfloat model_ambient[] = { 0.7f, 0.9f, 0.5f, 1.0f };

		glShadeModel(GL_SMOOTH);
		glMaterialfv(GL_FRONT,GL_CLAMP,mat_ambient);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_ambient);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL_FACE);
		glEnable(GL_CULL_FACE);*/


		//set the projection using ortho
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-width/2.0, width/2.0, height/2.0, -height/2.0, MIN_Z, MAX_Z);

		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		//Disable the vertical synchronization..//
		setVSync(0);
		return rc;
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0003", __FILE__, __FUNCSIG__); return NULL; }
}

//Select the current window
void RGraphicsWindow::SelectWindow()
{
	try{ wglMakeCurrent(dc, rc); }
	catch(...){ SetAndRaiseErrorMessage("GRDLL0006", __FILE__, __FUNCSIG__); }
}

//Render the graphics...//
void RGraphicsWindow::render(bool swapbuffers)
{
	try
	{
		if(swapbuffers) SwapBuffers(dc); // draw graphics on Window..
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the buffer...//
		wglMakeCurrent(NULL, NULL);
		glFlush();
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0007", __FILE__, __FUNCSIG__); }
}


//Set window view...(port view)//
void RGraphicsWindow::setViewEx(double left, double right, double bottom, double top)
{
	try
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(left, right, bottom, top, MIN_Z, MAX_Z);
		glMatrixMode(GL_MODELVIEW);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0009", __FILE__, __FUNCSIG__); }
}

//Set the window view port and window properties...//
void RGraphicsWindow::setView(double left, double right, double bottom, double top, double upp, double cx, double cy, bool inverted)
{
	try
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if(inverted)//Invert, if the window contains video...//
			glOrtho(left, right, top, bottom, MIN_Z, MAX_Z);
		else
			glOrtho(left, right, bottom, top, MIN_Z, MAX_Z);
		glMatrixMode(GL_MODELVIEW);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0010", __FILE__, __FUNCSIG__); }
}

//Resize the window.. i.e. reset the view port.......//
void RGraphicsWindow::resize(UINT32 width, UINT32 height, UINT32 srcL, UINT32 srcR)
{
	try
	{
		glViewport(srcL, srcR, width, height); 
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0011", __FILE__, __FUNCSIG__); }
}

//Read the current window pixels... destination pixels will be saved as jpeg..//
void RGraphicsWindow::readPixels(void* destination, UINT32 camW, UINT32 camH)
{	
	try
	{
		int p[4];
		glGetIntegerv(GL_VIEWPORT, p);
		if (camW == -1)
			camW = p[2];
		if (camH == -1)
			camH = p[3];
		//glDisable(GL_DEPTH);
		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_FUNC);
		glReadBuffer(GL_FRONT_FACE);
		GLint xx = (camW - p[2]);
		if (xx & 1 == 1) xx += 1;
		GLint yy = (camH - p[3]);
		if (yy & 1 == 1) yy += 1;

		glReadPixels((GLint)( -xx / 2), (GLint)( -yy/ 2), camW, camH, GL_BGRA_EXT, GL_UNSIGNED_BYTE, destination);
		//glReadPixels(-p[0]/2, -p[1]/2, p[2], p[3], GL_BGRA_EXT, GL_UNSIGNED_BYTE, destination);

	}
	catch(...)
	{ 
		SetAndRaiseErrorMessage("GRDLL0014", __FILE__, __FUNCSIG__); 
	}
}

void RGraphicsWindow::readPixelsDigitalZoom(void* destination)
{
	try
	{
		int p[4];
		glGetIntegerv(GL_VIEWPORT, p);
		glReadBuffer(GL_FRONT_FACE);
		glReadPixels(-p[2]/2, -p[3]/2, 2 * p[2], 2 * p[3], GL_BGRA_EXT, GL_UNSIGNED_BYTE, destination);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0014", __FILE__, __FUNCSIG__); }
}

void RGraphicsWindow::glReadVideoPixels(void* destination)
{
	try
	{
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, 0, 0, 800, 600, 0);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, destination); 
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0014", __FILE__, __FUNCSIG__); }
}

//Read the current window pixels...//
BYTE* RGraphicsWindow::readPixelsEx()
{	
	try 
	{
		int p[4];
		glGetIntegerv(GL_VIEWPORT, p);
		glReadBuffer(GL_FRONT_FACE);
		BYTE* destination = (BYTE*)calloc((p[2]) * (p[3]) * 3 + 1, sizeof(BYTE));
		glReadPixels(0, 0, p[2], p[3], GL_RGB, GL_UNSIGNED_BYTE, destination);
		return destination;
	
	/*	int p[4];
		glGetIntegerv(GL_VIEWPORT, p);
		BYTE* destination = (BYTE*)calloc((p[2] + 1) * (p[3] + 1) * 3, sizeof(BYTE));
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, p[2], p[3], 0);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, destination); 
		return destination;*/
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0015", __FILE__, __FUNCSIG__); return NULL; }
}

//Clear the window color.. to black..
void RGraphicsWindow::clearScreen()
{
	try{ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
	catch(...){ SetAndRaiseErrorMessage("GRDLL0016", __FILE__, __FUNCSIG__); }
}

void RGraphicsWindow::RotateGraphics(double a, double x, double y, double z, double px, double py, double pz, double *MMatrix)
{
	try
	{
		//glMatrixMode(GL_MODELVIEW);
		glGetDoublev(GL_MODELVIEW_MATRIX, MMatrix);
		//glGetDoublev(GL_PROJECTION_MATRIX,PMatrix);
		glLoadIdentity();
		glTranslatef(px, py, pz);
		glRotated(a, x, y, z); //rotate on the x axis
		glTranslatef(-px, -py, -pz);
		//glTranslatef(-MMatrix[12], -MMatrix[13], -MMatrix[14]);
		glMultMatrixd(MMatrix);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0018", __FILE__, __FUNCSIG__); }
}

void RGraphicsWindow::RotateGrWindow(double *MMatrix)
{
	try
	{
		glLoadIdentity();
		glMultMatrixd(MMatrix);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0018", __FILE__, __FUNCSIG__); }
}

//Set and get the rotate flag..//
bool RGraphicsWindow::getRotate()
{
	return flagValue;
}

void RGraphicsWindow::getRotate(bool f)
{
	flagValue = f;
}

//Rotate the current window graphics to rotateangle..//
void RGraphicsWindow::RotateGL()
{
	try
	{
		this->getRotate(false);
		double param[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, param);	
		glLoadIdentity();
		glRotatef((GLfloat)RotateAngle,0,0,1);
		glMultMatrixd(param);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0046", __FILE__, __FUNCSIG__); }	
}

//Set the rotate angle...//
void RGraphicsWindow::RotateGL(double angle)
{
	try
	{
		//glPushMatrix();
		this->RotateAngle = angle * (180.0 / M_PI);
		this->TotalRotated += this->RotateAngle;
		this->getRotate(true);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0047", __FILE__, __FUNCSIG__); }	
}

//Translate the graphics..///
void RGraphicsWindow::translateMatrix(double x, double y, double z)
{
	try{ glTranslated(x, y, z); }
	catch(...){ SetAndRaiseErrorMessage("GRDLL0048", __FILE__, __FUNCSIG__); }	
}

void RGraphicsWindow::UndoAllRotations()
{
	//this->RotateAngle = -this->TotalRotated;
	//this->RotateGL();
	//this->RotateAngle = 0; this->TotalRotated = 0;
	//glPopMatrix();
}