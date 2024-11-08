//Contains the graphics drawing functions.. Measurement drawing, crosshair, scancross, 
//Focus rectangles, scaleview...//
#include "stdafx.h"
#include <string>
#include "RGraphicsDraw.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "glut.h"
#include <iostream>

#define MAX_Y 999999
#define MAX_X 999999
#define MIN_Y -999999
#define MIN_X -999999
#define MAX_Z 999999
#define MIN_Z -999999

//Macro for fontlist, used in font creation
#define FONTLIST 255
#define IncrementAngle (M_PI / 180.0)
#define ArrowSize 12.0

static GLuint texture;

//Handle Error callback from Rapid Math......
void RMath2DErrorHandler(char* ecode, char* fname, char* funcname)
{
	GRAFIX->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

//Hnadle the callback from RapidMath 3d...
void RMath3DErrorHandler(char* ecode, char* fname, char* funcname)
{
	GRAFIX->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

RGraphicsDraw::RGraphicsDraw()
{
	try
	{
		this->scalefactor = 100;
		Videoback_Color.set(0.4f, 0.4f, 0.4f, 0);
		FlexibleCrosshair_Color.set(0, 0, 1, 0);
		RGraphicsDrawError = NULL;

		this->RapidMath2DInst = new RapidMath2D();
		this->RapidMath2DInst->RMath2DError = &RMath2DErrorHandler;

		this->RapidMath3DInst = new RapidMath3D();
		this->RapidMath3DInst->RMath3DError = &RMath3DErrorHandler;

		this->SetLight_Position1(-1000.0, 1000.0, 2550.0);
		this->fname = "Tahoma";
		this->Bg_Color_Upper[0] = 0.2;
		this->Bg_Color_Upper[1] = 0.2;
		this->Bg_Color_Upper[2] = 0.2;
		this->Bg_Color_Lower[0] = 0.5;
		this->Bg_Color_Lower[1] = 0.5;
		this->Bg_Color_Lower[2] = 0.5;
		this->PointSize = 3;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0001", __FILE__, __FUNCSIG__); }
}

RGraphicsDraw::~RGraphicsDraw()
{
	try
	{
		glClear(0);
		glClearColor(0, 0, 0, 0);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0002", __FILE__, __FUNCSIG__); }
}

RGraphicsDraw* RGraphicsDraw::RgraphicsInstance = 0;

void RGraphicsDraw::CreateGraphicsDrawInstance()
{
	RgraphicsInstance = new RGraphicsDraw();
}

void RGraphicsDraw::DeleteGraphicsDrawInstance()
{
	try
	{
		delete RgraphicsInstance->RapidMath2DInst; delete RgraphicsInstance->RapidMath3DInst;
		delete RgraphicsInstance;
	}
	catch(...){}
}

RGraphicsDraw* RGraphicsDraw::getRgdInstance()
{
	return RgraphicsInstance;
}

void RGraphicsDraw::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(RGraphicsDrawError != NULL)
		RGraphicsDrawError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}

//Expose instance of RapidMath 2D...
RapidMath2D* RGraphicsDraw::getRapidMath()
{
	return RapidMath2DInst;
}

//Expose instance of RapidMath 3D...
RapidMath3D* RGraphicsDraw::getRapidMath3D()
{
	return RapidMath3DInst;
}


void RGraphicsDraw::SetWindowSettings(double upp,double cx, double cy, int cindex)
{
	try
	{
		cidx = cindex;
		Cwindowno = cindex;
		rwindow[cidx].UPP = upp;
		rwindow[cidx].cx = cx;
		rwindow[cidx].cy = cy;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0003", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawAxes(double wupp, bool XaxisHighlighted, bool YaxisHighlighted)
{
	try
	{
		GLdouble Xaxis_Array[8] = {0, 0, MIN_X * wupp, 0, 0, 0, MAX_X * wupp, 0};
		GLdouble Yaxis_Array[8] = {0, 0, 0, MIN_Y * wupp, 0, 0, 0, MAX_Y * wupp};
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(5, 0x5F5F);
		glColor3d(1, 0, 0);
		unsigned int indices[] = {0, 1, 2, 3};
		if(!XaxisHighlighted)
		{
			glVertexPointer(2, GL_DOUBLE, 0, Xaxis_Array);
			glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, indices);
		}
		if(!YaxisHighlighted)
		{
			glVertexPointer(2, GL_DOUBLE, 0, Yaxis_Array);
			glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, indices);
		}
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_LINE_STIPPLE);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0004", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawAxes_3D(double wupp)
{
	try
	{
		glLineStipple(2,(short) 0x1C47);
		glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glBegin(GL_LINES);
		glVertex3d(-200* wupp, 0 , 0);
		glVertex3d(200 * wupp, 0, 0);
		glColor3d(0, 1, 0);
		glVertex3d(0, -200 * wupp, 0);
		glVertex3d(0, 200 * wupp, 0);
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, -200 * wupp);
		glVertex3d(0, 0, 200 * wupp);
		glEnd();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0005", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawAxes_3DWithArrow(double x, double y, double z, double wupp)
{
	try
	{
		glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glVertex3d(x, y, z);
		glVertex3d(x + 200 * wupp, y, z);
		glColor3d(0, 1, 0);
		glVertex3d(x, y, z);
		glVertex3d(x, y + 200 * wupp, z);
		glColor3d(0, 0, 1);
		glVertex3d(x, y, z);
		glVertex3d(x, y, z + 200 * wupp);
		glEnd();

		glColor3d(1, 0, 0);
		drawConeFor3DAxis(x + 200 * wupp, y, z, x + 230 * wupp, y, z, 10 * wupp, 0);
		glColor3d(0, 1, 0);
		drawConeFor3DAxis(x, y + 200 * wupp, z, x, y + 230 * wupp, z, 10 * wupp, 0);
		glColor3d(0, 0, 1);
		drawConeFor3DAxis(x, y, z + 200 * wupp, x, y, z + 230 * wupp, 10 * wupp, 0);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0006", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFixedAxis(double cx, double cy, double wupp)
{
	try
	{
		glColor3d(1, 0, 0);
		//drawLine(MIN_X * wupp  + rwindow[2].cx - cx, rwindow[2].cy - cy, MAX_X * wupp  + rwindow[2].cx - cx, rwindow[2].cy - cy);
		//drawLine(rwindow[2].cx - cx, MIN_Y * wupp  + rwindow[2].cy - cy, rwindow[2].cx - cx, MAX_Y * wupp  + rwindow[2].cy - cy);
		drawLine(MIN_X * wupp, cy, MAX_X * wupp, cy);
		drawLine(cx, MIN_Y * wupp, cx, MAX_Y * wupp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0007", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFixedAxisBackground(double x, double y, double cx, double cy)
{
	try
	{
		glColor3d(0.5f, 0.5f, 0.5f);
		//drawRectangle(rwindow[2].cx - cx - x / 2, rwindow[2].cy - cy + y / 2, rwindow[2].cx - cx + x / 2, rwindow[2].cy - cy - y / 2, true);
		drawRectangle(cx - x/2, cy + y/2, cx + x/2, cy - y/2, true);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0007", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFixedAxisOverlapImgWin(double cx, double cy, double wupp)
{
	try
	{
		glColor3d(1, 0, 0);
		drawLine(MIN_X * wupp  + rwindow[3].cx - cx, rwindow[3].cy - cy, MAX_X * wupp  + rwindow[3].cx - cx, rwindow[3].cy - cy);
		drawLine(rwindow[3].cx - cx, MIN_Y * wupp  + rwindow[3].cy - cy, rwindow[3].cx - cx, MAX_Y * wupp  + rwindow[3].cy - cy);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0007", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFixedAxisBackgroundOverlapImgWin(double x, double y, double cx, double cy)
{
	try
	{
		glColor3d(0.5f, 0.5f, 0.5f);
		drawRectangle(rwindow[3].cx - cx - x/2, rwindow[3].cy - cy + y/2, rwindow[3].cx - cx + x/2, rwindow[3].cy - cy - y/2, true);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0007a", __FILE__, __FUNCSIG__); }
}


void RGraphicsDraw::drawFlexibleCrossHair(double x, double y, double gap)
{
	try
	{
		GLdouble mix[]={-1000, y - gap, 1000, y + gap, x - gap, -1000, x + gap, 1000};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 3};
		translateMatrix(0.0, 0.0, 101.0);

		glColor3d(FlexibleCrosshair_Color.r, FlexibleCrosshair_Color.g, FlexibleCrosshair_Color.b);
		//glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, indices);
		//Left HOrizontal line
		glBegin(GL_LINES);
		glVertex2d(x - gap, y); //MAX_X, rwindow[0].cy); //
		glVertex2d(MIN_X, y);
		glEnd();

		//Top Vertical Line
		glBegin(GL_LINES);
		glVertex2d(x, y + gap);
		glVertex2d(x, MAX_Y);
		glEnd();

		//Right Horizontal Line
		glBegin(GL_LINES);
		glVertex2d(x + gap, y);
		glVertex2d(MAX_X, y);
		glEnd();

		//Bottom Vertical Line
		glBegin(GL_LINES);
		glVertex2d(x, y - gap); //MAX_Y); //
		glVertex2d(x, MIN_Y);
		glEnd();
		translateMatrix(0.0, 0.0, -101.0);

		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0008", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawTransparentRect(double x, double y, double width, double wupp)
{
	//Draw an "empty" box to hide all graphics around the mouse pick up point
	double point1[2] = { x - width, y + width }, point2[2] = { x + width, y - width };
	translateMatrix(0.0, 0.0, 100.0);
	glColor3d(Videoback_Color.r, Videoback_Color.g, Videoback_Color.b);
	drawRectangle(point1[0], point1[1], point2[0], point2[1], true);
	translateMatrix(0.0, 0.0, -100.0);
}

void RGraphicsDraw::drawCrossHair(bool hatchedmode, bool record, int* offset, bool ShiftUpperVerticalLine)
{
	try
	{
		if (hatchedmode)
		{
			glEnable(GL_LINE_STIPPLE);//for hatched crosshair
			glLineStipple(1, 0x03FF);
		}
		double gap = rwindow[0].UPP;// / 2;
		glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);

		////Left HOrizontal line
		glBegin(GL_LINES);
		glVertex2d(rwindow[0].cx + offset[0] * gap , (rwindow[0].cy + offset[1] * gap)); //MAX_X, rwindow[0].cy); //
		glVertex2d(MIN_X, (rwindow[0].cy + offset[1] * gap));
		glEnd();

		////Top Vertical Line
		glBegin(GL_LINES);
		glVertex2d((rwindow[0].cx + offset[2] * gap)-0.0000079, rwindow[0].cy + offset[3] * gap);
		glVertex2d((rwindow[0].cx + offset[2] * gap)-0.0000079, MAX_Y);
		glEnd();

		////Right Horizontal Line
		glBegin(GL_LINES);
		glVertex2d(rwindow[0].cx + offset[4] * gap, (rwindow[0].cy + offset[5] * gap));
		glVertex2d(MAX_X, (rwindow[0].cy + offset[5] * gap));
		glEnd();

		////Bottom Vertical Line
		glBegin(GL_LINES);
		glVertex2d((rwindow[0].cx + offset[6] * gap)+0, rwindow[0].cy + offset[7] * gap); //MAX_Y); //
		glVertex2d((rwindow[0].cx + offset[6] * gap)+0, MIN_Y);
		glEnd();



		////Left HOrizontal line
		//glBegin(GL_LINES);
		//glVertex2d(rwindow[0].cx - 2 * gap, rwindow[0].cy); //MAX_X, rwindow[0].cy); //
		//glVertex2d(MIN_X, rwindow[0].cy);
		//glEnd();

		////Right Horizontal Line
		//glBegin(GL_LINES);
		//glVertex2d(rwindow[0].cx + gap, rwindow[0].cy);
		//glVertex2d(MAX_X, rwindow[0].cy);
		//glEnd();

		////Bottom Vertical Line
		//glBegin(GL_LINES);
		//glVertex2d(rwindow[0].cx - gap, rwindow[0].cy - 2 * gap); //MAX_Y); //
		//glVertex2d(rwindow[0].cx - gap, MIN_Y);
		//glEnd();

		////Top Vertical Line
		//glBegin(GL_LINES);
		////if (ShiftUpperVerticalLine)
		//{
		//	glVertex2d(rwindow[0].cx - gap, rwindow[0].cy + gap);
		//	glVertex2d(rwindow[0].cx - gap, MAX_Y);
		//}
		////else
		////{
		////	glVertex2d(rwindow[0].cx, rwindow[0].cy + gap);
		////	glVertex2d(rwindow[0].cx, MAX_Y);
		////}
		//glEnd();

		//glColor3d(0.4, 0.4, 0.4); 
		//glColor3d(Videoback_Color.r, Videoback_Color.g, Videoback_Color.b);
		////drawRectangle(rwindow[0].cx - 5, rwindow[0].cy + 5 , rwindow[0].cx + 5, rwindow[0].cy - 5, true);
		//GLdouble pts[] = { rwindow[0].cx - gap, rwindow[0].cy + gap, rwindow[0].cx + gap, rwindow[0].cy + gap,
		//					rwindow[0].cx + gap, rwindow[0].cy + gap, rwindow[0].cx + gap, rwindow[0].cy - gap,
		//					rwindow[0].cx + gap, rwindow[0].cy - gap, rwindow[0].cx - gap, rwindow[0].cy - gap,
		//					rwindow[0].cx - gap, rwindow[0].cy - gap, rwindow[0].cx - gap, rwindow[0].cy + gap };
		//glEnableClientState(GL_VERTEX_ARRAY);
		//glVertexPointer(3, GL_DOUBLE, 0, pts);

		//unsigned int indices[] = { 0, 1, 2, 3, 0 };
		//EnableTransparency();
		//glColor3d(Videoback_Color.r, Videoback_Color.g, Videoback_Color.b);
		//glPushMatrix();
		////glTranslatef(0,0,51);
		//glDrawElements(GL_POLYGON, 5, GL_UNSIGNED_INT, indices);
		//glPopMatrix();
		//glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		//DisableTransparency();
		//glDisableClientState(GL_VERTEX_ARRAY);

		//glBegin(GL_LINES);
		//glVertex2d(rwindow[0].cx - 10* gap, rwindow[0].cy); 
		//glVertex2d(rwindow[0].cx + 10*gap, rwindow[0].cy);
		//glEnd();


		//glBegin(GL_POINTS);
		//glPointSize(4 * gap);
		//glVertex3d(rwindow[0].cx, rwindow[0].cy, 0);
		//glEnd();


		//GLdouble mix[] = { rwindow[0].cx - gap, rwindow[0].cy, MIN_X, rwindow[0].cy, // + rwindow[0].cx
		//	rwindow[0].cx + gap, rwindow[0].cy, MAX_X, rwindow[0].cy, //+ rwindow[0].cx
		//	rwindow[0].cx, rwindow[0].cy - gap, rwindow[0].cx, MIN_Y, // + rwindow[0].cy,
		//	rwindow[0].cx, rwindow[0].cy + gap, rwindow[0].cx, MAX_Y };// +rwindow[0].cy};

		//glEnableClientState(GL_VERTEX_ARRAY);
		//glVertexPointer(2, GL_DOUBLE, 0, mix);
		//unsigned int indices[] = {0, 1, 2, 3, 4, 5, 6, 7};
		//glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		//glPushMatrix();
		//glTranslatef(0,0,-150);
		//glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, indices);
		//glDisableClientState(GL_VERTEX_ARRAY);
		//glPopMatrix();
		if(record)
		{
			CurrentX = rwindow[0].cx;
			CurrentY = rwindow[0].cy;
		}
		if(hatchedmode) glDisable(GL_LINE_STIPPLE);//for hatched crosshair
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0009", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCrossHair(bool hatchedmode, bool record, double angle) //, int* offset)
{
	try
	{
		if(hatchedmode) 
		{
			glEnable(GL_LINE_STIPPLE);//for hatched crosshair
			glLineStipple(1, 0x03FF);
		}
		int offset[8] = { 0 };
		double gap = rwindow[0].UPP; // / 2;
		double cos_angle = cos(angle);
		double sin_angle = sin(angle);
		GLdouble mix[]={rwindow[0].cx + offset[4] * gap, rwindow[0].cy + offset[5] * gap, 0, MAX_X * cos_angle  + rwindow[0].cx + offset[4] * gap, rwindow[0].cy +  MAX_Y * sin_angle + offset[5] * gap, 0,
			rwindow[0].cx + offset[0] * gap, rwindow[0].cy + offset[1] * gap, 0, MIN_X * cos_angle + rwindow[0].cx + offset[0] * gap, rwindow[0].cy +  MIN_Y * sin_angle + offset[1] * gap, 0,
			rwindow[0].cx + offset[6] * gap, rwindow[0].cy + offset[7] * gap, 0, rwindow[0].cx + MAX_X * cos_angle + offset[6] * gap, MIN_Y * sin_angle + rwindow[0].cy + offset[7] * gap, 0,
			rwindow[0].cx + offset[2] * gap, rwindow[0].cy + offset[3] * gap, 0, rwindow[0].cx + MIN_X * cos_angle + offset[2] * gap, MAX_Y * sin_angle + rwindow[0].cy + offset[3] * gap, 0};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 3, 4, 5, 6, 7};
		glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);

		glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		if(record)
		{
			CurrentX = rwindow[0].cx;
			CurrentY = rwindow[0].cy;
		}
		if(hatchedmode) glDisable(GL_LINE_STIPPLE);//for hatched crosshair
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0010", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawXCrossHair(bool hatchedmode, double linewidth)
{
	try
	{
		if(hatchedmode) 
		{
			glEnable(GL_LINE_STIPPLE);//for hatched crosshair
			glLineStipple(1, 0x03FF);
		}
		glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		glLineWidth(linewidth);
		drawLine(MIN_X  + rwindow[0].cx, rwindow[0].cy, MAX_X + rwindow[0].cx, rwindow[0].cy);
		glLineWidth(1);
		if(hatchedmode) glDisable(GL_LINE_STIPPLE);//for hatched crosshair
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0011", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawYCrossHair(bool hatchedmode, double linewidth)
{
	try
	{
		if(hatchedmode) 
		{
			glEnable(GL_LINE_STIPPLE);//for hatched crosshair
			glLineStipple(1, 0x03FF);
		}
		glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		glLineWidth(linewidth);
		drawLine(rwindow[0].cx, MIN_Y + rwindow[0].cy, rwindow[0].cx, MAX_Y + rwindow[0].cy);
		glLineWidth(1);
		if(hatchedmode) glDisable(GL_LINE_STIPPLE);//for hatched crosshair
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0012", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawAngleCrossHair(bool hatchedmode, double angle, double linewidth/*, double minX, double minY*/)
{
	try
	{
		if(hatchedmode) 
		{
			glEnable(GL_LINE_STIPPLE);//for hatched crosshair
			glLineStipple(1, 0x03FF);
		}
		glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		glLineWidth(linewidth);
		double cos_angle = cos(angle), sin_angle = sin(angle);
		/*double tmpdist = (sqrt(pow(minX, 2) + pow(minY, 2)) - minX)/2;
		drawLine(rwindow[0].cx + tmpdist * cos_angle, rwindow[0].cy + tmpdist * sin_angle, rwindow[0].cx +  MAX_X * cos_angle, rwindow[0].cy + MAX_Y * sin_angle);*/
		drawLine(rwindow[0].cx + MIN_X * cos_angle, rwindow[0].cy + MIN_Y * sin_angle, rwindow[0].cx +  MAX_X * cos_angle, rwindow[0].cy + MAX_Y * sin_angle);
		glLineWidth(1);
		if(hatchedmode) glDisable(GL_LINE_STIPPLE);//for hatched crosshair
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0013", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawLineFragment(bool hatchedmode, double x1, double y1, double x2, double y2, double linewidth)
{
	try
	{
		if(hatchedmode) 
		{
			glEnable(GL_LINE_STIPPLE);//for hatched crosshair
			glLineStipple(1, 0x03FF);
		}
		glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		glLineWidth(linewidth);
		drawLine(x1, y1, x2, y2);
		glLineWidth(1);
		if(hatchedmode) glDisable(GL_LINE_STIPPLE);//for hatched crosshair
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0013a", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawScanMode(double x, double y, double width, double wupp, bool hatchedmode, bool flag)
{
	try
	{
		if(hatchedmode) 
		{
			glEnable(GL_LINE_STIPPLE);//for hatched crosshair
			glLineStipple(1, 0x03FF);
		}
		double point1[2] = {x - width, y + width}, point2[2] = {x + width, y - width};
		glColor3d(Videoback_Color.r, Videoback_Color.g, Videoback_Color.b);
		drawRectangle(point1[0] + wupp, point1[1] - wupp , point2[0] - wupp, point2[1] + wupp, true);
		translateMatrix(0.0, 0.0, 2.0);
		if(flag)
			glColor3d(0, 1, 0);
		else
			glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		drawRectangle(point1[0], point1[1], point2[0], point2[1]);
		if(hatchedmode) glDisable(GL_LINE_STIPPLE);//for hatched crosshair
		translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0014", __FILE__, __FUNCSIG__); }
}

//void RGraphicsDraw::drawScaleViewMode(double wupp, int WinWidth, int WinHeight)
//{
//	try
//	{
//		double Win_Width = wupp * WinWidth, Win_Height = wupp * WinHeight;
//		double Wupp_um = wupp * 1000, ScaleView_Gap = 0.2; // in mm
//		double Video_Cx = rwindow[0].cx, Video_Cy = rwindow[0].cy;
//		
//		if(Wupp_um < 3)
//			ScaleView_Gap = 0.1;
//		else if(Wupp_um < 4)
//			ScaleView_Gap = 0.2;
//		else if(Wupp_um < 16)
//			ScaleView_Gap = 0.5;
//		else 
//			ScaleView_Gap = 1;
//
//		int ValueCnt_X = (int)((Win_Width/ScaleView_Gap)/2) + 2, ValueCnt_Y = (int)((Win_Height/ScaleView_Gap)/2) + 2;
//		double Line_Width = 10 * wupp; // pixels * pixelwidth
//		double Text_PosGap = 18 * wupp;
//
//		double Cen_DiffX = Video_Cx - CurrentX, Cen_DiffY = Video_Cy - CurrentY;
//		int XStart_Cnt = (int)(Cen_DiffX/ScaleView_Gap),  YStart_Cnt = (int)(Cen_DiffY/ScaleView_Gap);
//		double XStart_Pos = CurrentX + XStart_Cnt * ScaleView_Gap, YStart_Pos = CurrentY + YStart_Cnt * ScaleView_Gap;
//		double XStart_Value = XStart_Cnt * ScaleView_Gap, YStart_Value = YStart_Cnt * ScaleView_Gap;
//
//		double YPos_LineUp = Video_Cy + Line_Width, YPos_LineBtm = Video_Cy - Line_Width;
//		double XPos_LineRgt = Video_Cx + Line_Width, XPos_LineLft = Video_Cx - Line_Width;
//		double XScale_TxtPos_Y = Video_Cy - Text_PosGap, YScale_TxtPos_X = Video_Cx - Text_PosGap;
//
//		glColor3f(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
//		//draw first value on both axis..
//		//glLineWidth(2);
//		drawLine(XStart_Pos, YPos_LineUp, XStart_Pos, YPos_LineBtm); 
//		drawLine(XPos_LineRgt, YStart_Pos, XPos_LineLft, YStart_Pos);
//		//glLineWidth(1);
//		drawstr_scaleview(XStart_Pos - Text_PosGap, XScale_TxtPos_Y, XStart_Value, wupp);
//		drawstr_scaleview(YScale_TxtPos_X, YStart_Pos - Text_PosGap, YStart_Value, wupp);
//
//		for(int Cnt = 1; Cnt < ValueCnt_X; Cnt++)
//		{
//			double X_PosP = XStart_Pos + Cnt * ScaleView_Gap, X_PosN = XStart_Pos - Cnt * ScaleView_Gap;
//			double Y_PosP = YStart_Pos + Cnt * ScaleView_Gap, Y_PosN = YStart_Pos - Cnt * ScaleView_Gap;
//			double XScale_ValP = XStart_Value + Cnt * ScaleView_Gap, XScale_ValN = XStart_Value - Cnt * ScaleView_Gap;
//			double YScale_ValP = YStart_Value + Cnt * ScaleView_Gap, YScale_ValN = YStart_Value - Cnt * ScaleView_Gap;
//			//draw the line segments
//			//glLineWidth(2);
//			drawLine(X_PosP, YPos_LineUp, X_PosP, YPos_LineBtm);
//			drawLine(X_PosN, YPos_LineUp, X_PosN, YPos_LineBtm);
//			drawLine(XPos_LineRgt, Y_PosP, XPos_LineLft, Y_PosP);
//			drawLine(XPos_LineRgt, Y_PosN, XPos_LineLft, Y_PosN);
//			//glLineWidth(1);
//
//			//Text Positions..
//			double XScale_TxtPos_xP = X_PosP - Text_PosGap, XScale_TxtPos_xN = X_PosN - Text_PosGap;
//			double YScale_TxtPos_yP = Y_PosP - Text_PosGap, YScale_TxtPos_yN = Y_PosN - Text_PosGap;
//			
//			//draw scale value text
//			drawstr_scaleview(XScale_TxtPos_xP, XScale_TxtPos_Y, XScale_ValP, wupp);
//			drawstr_scaleview(XScale_TxtPos_xN, XScale_TxtPos_Y, XScale_ValN, wupp);
//			drawstr_scaleview(YScale_TxtPos_X, YScale_TxtPos_yP, YScale_ValP, wupp);
//			drawstr_scaleview(YScale_TxtPos_X, YScale_TxtPos_yN, YScale_ValN, wupp);
//		}
//	}
//	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0015", __FILE__, __FUNCSIG__); }
//}
//
//void RGraphicsDraw::drawstr_scaleview(double x,double y, double d, double wupp)
//{
//	try
//	{
//		if(abs(d) < 0.00001)
//			d = 0;
//		std::string temp = "";
//		int dp, sgn;
//		char st[10];
//		_fcvt_s(st, 8, d, 1, &dp, &sgn);
//		if(dp == 0) dp = 1;
//		temp = st;
//		if(abs(d) < 1)
//			temp = "0" + temp;
//		temp.insert(dp, ".");
//		if(sgn == 1)
//			temp = "-" + temp;
//		drawString3D(x, y, 0, 0, (char*)temp.c_str(), wupp * 18);
//	}
//	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0016", __FILE__, __FUNCSIG__); }
//}



void RGraphicsDraw::drawScaleViewMode(double wupp, int WinWidth, int WinHeight, bool INCH_Mode)
{
	try
	{
		double Win_Width = wupp * WinWidth, Win_Height = wupp * WinHeight;
		double Wupp_um = wupp * 1000, ScaleView_Gap = 0.2; // in mm
		double Video_Cx = rwindow[0].cx, Video_Cy = rwindow[0].cy;
		int NoOfDecPlaces = 1, TextPosGap_CntX = 18, TextPosGap_CntY = 18;
		double Unit_Factor = 1;
		
		if(INCH_Mode)
		{
			NoOfDecPlaces = 3;
			Unit_Factor = 25.4;
			TextPosGap_CntX = 24;
			if(Wupp_um < 3)
				ScaleView_Gap = 0.2;
			else if(Wupp_um < 4)
				ScaleView_Gap = 0.3;
			else if(Wupp_um < 16)
				ScaleView_Gap = 0.7;
			else 
				ScaleView_Gap = 1.5;
		}
		else
		{
			if(Wupp_um < 3)
				ScaleView_Gap = 0.1;
			else if(Wupp_um < 4)
				ScaleView_Gap = 0.2;
			else if(Wupp_um < 16)
				ScaleView_Gap = 0.5;
			else 
				ScaleView_Gap = 1;
		}
		
		int ValueCnt_X = (int)((Win_Width/ScaleView_Gap)/2) + 2, ValueCnt_Y = (int)((Win_Height/ScaleView_Gap)/2) + 2;
		double Line_Width = 10 * wupp; // pixels * pixelwidth
		double Text_PosGapX = TextPosGap_CntX * wupp, Text_PosGapY = TextPosGap_CntY * wupp;

		double Cen_DiffX = Video_Cx - CurrentX, Cen_DiffY = Video_Cy - CurrentY;
		int XStart_Cnt = (int)(Cen_DiffX/ScaleView_Gap),  YStart_Cnt = (int)(Cen_DiffY/ScaleView_Gap);
		double XStart_Pos = CurrentX + XStart_Cnt * ScaleView_Gap, YStart_Pos = CurrentY + YStart_Cnt * ScaleView_Gap;
		double XStart_Value = XStart_Cnt * ScaleView_Gap / Unit_Factor, YStart_Value = YStart_Cnt * ScaleView_Gap / Unit_Factor;

		double YPos_LineUp = Video_Cy + Line_Width, YPos_LineBtm = Video_Cy - Line_Width;
		double XPos_LineRgt = Video_Cx + Line_Width, XPos_LineLft = Video_Cx - Line_Width;
		double XScale_TxtPos_Y = Video_Cy - Text_PosGapY, YScale_TxtPos_X = Video_Cx - Text_PosGapX;

		glColor3f(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		//draw first value on both axis..
		//glLineWidth(2);
		drawLine(XStart_Pos, YPos_LineUp, XStart_Pos, YPos_LineBtm); 
		drawLine(XPos_LineRgt, YStart_Pos, XPos_LineLft, YStart_Pos);
		//glLineWidth(1);
		drawstr_scaleview(XStart_Pos - Text_PosGapX, XScale_TxtPos_Y, XStart_Value, wupp, NoOfDecPlaces);
		drawstr_scaleview(YScale_TxtPos_X, YStart_Pos - Text_PosGapY, YStart_Value, wupp, NoOfDecPlaces);

		for(int Cnt = 1; Cnt < ValueCnt_X; Cnt++)
		{
			double X_PosP = XStart_Pos + Cnt * ScaleView_Gap, X_PosN = XStart_Pos - Cnt * ScaleView_Gap;
			double Y_PosP = YStart_Pos + Cnt * ScaleView_Gap, Y_PosN = YStart_Pos - Cnt * ScaleView_Gap;
			double XScale_ValP = XStart_Value + Cnt * ScaleView_Gap / Unit_Factor, XScale_ValN = XStart_Value - Cnt * ScaleView_Gap / Unit_Factor;
			double YScale_ValP = YStart_Value + Cnt * ScaleView_Gap / Unit_Factor, YScale_ValN = YStart_Value - Cnt * ScaleView_Gap / Unit_Factor;
			//draw the line segments
			//glLineWidth(2);
			drawLine(X_PosP, YPos_LineUp, X_PosP, YPos_LineBtm);
			drawLine(X_PosN, YPos_LineUp, X_PosN, YPos_LineBtm);
			drawLine(XPos_LineRgt, Y_PosP, XPos_LineLft, Y_PosP);
			drawLine(XPos_LineRgt, Y_PosN, XPos_LineLft, Y_PosN);
			//glLineWidth(1);

			//Text Positions..
			double XScale_TxtPos_xP = X_PosP - Text_PosGapX, XScale_TxtPos_xN = X_PosN - Text_PosGapX;
			double YScale_TxtPos_yP = Y_PosP - Text_PosGapY, YScale_TxtPos_yN = Y_PosN - Text_PosGapY;
			
			//draw scale value text
			drawstr_scaleview(XScale_TxtPos_xP, XScale_TxtPos_Y, XScale_ValP, wupp, NoOfDecPlaces);
			drawstr_scaleview(XScale_TxtPos_xN, XScale_TxtPos_Y, XScale_ValN, wupp, NoOfDecPlaces);
			drawstr_scaleview(YScale_TxtPos_X, YScale_TxtPos_yP, YScale_ValP, wupp, NoOfDecPlaces);
			drawstr_scaleview(YScale_TxtPos_X, YScale_TxtPos_yN, YScale_ValN, wupp, NoOfDecPlaces);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0015", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawstr_scaleview(double x,double y, double d, double wupp, int DecPlaces)
{
	try
	{
		char st[20];
		RMATH2DOBJECT->Double2String(d, DecPlaces, &st[0]);
		drawStringEdited3D(x, y, 0, 0, &st[0], wupp * 18);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0016", __FILE__, __FUNCSIG__); }
}



void RGraphicsDraw::drawXaxis(double x)
{
	try
	{
		GLdouble mix[]={MIN_X * rwindow[cidx].UPP, x, -0.9,MAX_X * rwindow[cidx].UPP, x, -0.9};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1};
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0017", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawYaxis(double x)
{
	try
	{
		GLdouble mix[]={x, MIN_Y * rwindow[cidx].UPP, -0.9, x, MAX_Y * rwindow[cidx].UPP, -0.9};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1};
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0018", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawLiveScanCross(double x, double y, double width, double wupp, bool flag)
{
	try
	{
		double point1[2] = {x - width, y + width}, point2[2] = {x + width, y - width};
		glColor3d(Videoback_Color.r, Videoback_Color.g, Videoback_Color.b);
		translateMatrix(0.0, 0.0, 2.0);
		drawRectangle(point1[0] + wupp, point1[1] - wupp , point2[0] - wupp, point2[1] + wupp, true);
		if(flag)
			glColor3d(0, 1, 0);
		else
			glColor3d(FlexibleCrosshair_Color.r, FlexibleCrosshair_Color.g, FlexibleCrosshair_Color.b);
		drawRectangle(point1[0], point1[1], point2[0], point2[1]);
		translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0014", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawMultiScanCross(double x, double y, double width, double wupp)
{
	try
	{
		double point1[2] = {x - width, y + width}, point2[2] = {x + width, y - width};
		glColor3d(Videoback_Color.r, Videoback_Color.g, Videoback_Color.b);
		translateMatrix(0.0, 0.0, 2.0);
		drawRectangle(point1[0] + wupp, point1[1] - wupp , point2[0] - wupp, point2[1] + wupp, true);
		glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
		drawRectangle(point1[0], point1[1], point2[0], point2[1]);
		translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0014", __FILE__, __FUNCSIG__); }
}


//Set graphics color..//
void RGraphicsDraw::SetColor_Double(double r, double g, double b)
{
	glColor3d(r, g, b);
}

//Set graphics color with transperant..
void RGraphicsDraw::SetColor_Double_Alpha(double r, double g, double b, double alpha)
{
	Material_Diffuse[0] = r; Material_Diffuse[1] = g;
	Material_Diffuse[2] = b; Material_Diffuse[3] = alpha;
	/*Material_Diffuse[0] = 180.0/255.0; Material_Diffuse[1] = 215.0/255.0;
	Material_Diffuse[2] = 255.0/255.0; Material_Diffuse[3] = 1.0;*/
	glColor4d(r, g, b, alpha);
}

//Set graphics color..//
void RGraphicsDraw::SetColor_Int(int r, int g, int b)
{
	glColor3i(r, g, b);
}

//Set measurement color..
void RGraphicsDraw::SetMeasurement_Color(double r, double g, double b)
{
	Measure_Color.set(r, g, b, 0);
}

//Set crosshair color property..//
void RGraphicsDraw::SetCrossHair_Color(double r, double g, double b)
{
	Crosshair_Color.set(r, g, b, 0);
}

//Set flexible ceosshair color...//
void RGraphicsDraw::SetFlexbleCrossHair_Color(double r, double g, double b)
{
	FlexibleCrosshair_Color.set(r, g, b, 0);
}


void RGraphicsDraw::SetMaterial_Ambient(double r, double g, double b, double z)
{
	Material_Ambient[0] = r; Material_Ambient[1] = g;
	Material_Ambient[2] = b; Material_Ambient[3] = z;
}

void RGraphicsDraw::SetMaterial_Specular(double r, double g, double b, double z)
{
	Material_Specular[0] = r; Material_Specular[1] = g;
	Material_Specular[2] = b; Material_Specular[3] = z;
}

void RGraphicsDraw::SetMaterial_Diffuse(double r, double g, double b, double z)
{
	/*Material_Diffuse[0] = r; Material_Diffuse[1] = g;
	Material_Diffuse[2] = b; Material_Diffuse[3] = z;*/
}

void RGraphicsDraw::SetMaterial_Shininess(double z)
{
	Material_Shininess = z;
}

void RGraphicsDraw::SetModel_Ambient(double r, double g, double b, double z)
{
	Model_Ambient[0] = r; Model_Ambient[1] = g;
	Model_Ambient[2] = b; Model_Ambient[3] = z;
}

void RGraphicsDraw::SetLight_Position1(double r, double g, double b, double z)
{
	Light_Position1[0] = r; Light_Position1[1] = g;
	Light_Position1[2] = b; Light_Position1[3] = z;
}

void RGraphicsDraw::SetLight_Position2(double r, double g, double b, double z)
{
	Light_Position2[0] = r; Light_Position2[1] = g;
	Light_Position2[2] = b; Light_Position2[3] = z;
}

void RGraphicsDraw::SetLight_Position3(double r, double g, double b, double z)
{
	Light_Position3[0] = r; Light_Position3[1] = g;
	Light_Position3[2] = b; Light_Position3[3] = z;
}

void RGraphicsDraw::SetLight_Position4(double r, double g, double b, double z)
{
	Light_Position4[0] = r; Light_Position4[1] = g;
	Light_Position4[2] = b; Light_Position4[3] = z;
}

void RGraphicsDraw::SetLight_Position5(double r, double g, double b, double z)
{
	Light_Position5[0] = r; Light_Position5[1] = g;
	Light_Position5[2] = b; Light_Position5[3] = z;
}

void RGraphicsDraw::SetLight_Position6(double r, double g, double b, double z)
{
	Light_Position6[0] = r; Light_Position6[1] = g;
	Light_Position6[2] = b; Light_Position6[3] = z;
}


void RGraphicsDraw::CreateNewGlList(int id)
{
	glNewList(id, GL_COMPILE);
}

void RGraphicsDraw::EndNewGlList()
{
	glEndList();
}

void RGraphicsDraw::DeleteGlList(int id)
{
	glDeleteLists(id, 1);
}

void RGraphicsDraw::CalldedrawGlList(int id)
{
	glCallList(id);
}

void RGraphicsDraw::PushCurrentGraphics()
{
	glPushMatrix();
}

void RGraphicsDraw::PopCurrentGraphics()
{
	glPopMatrix();
}

void RGraphicsDraw::SetGraphicsLineWidth(double width)
{
	glLineWidth(width);
}

void RGraphicsDraw::LoadGraphicsIdentityMatrix()
{
	glLoadIdentity();
}

//setting font name
void RGraphicsDraw::SetFontFamily(std::string FFamilyName)
{
	this->fname = FFamilyName;
}

std::string RGraphicsDraw::GetFontFamilyName()
{
	return fname;
}


void RGraphicsDraw::DrawVideoTextureBackGround(std::map<int, std::string> imagepath, int bpwidth, int bpheight, int Rowcount , int Columncount)
{
	try
	{
		GLuint* texture;
		int NumberofImages = Rowcount*Columncount;
		double Ratio = (double)bpwidth / bpheight ;
		if(Columncount==0 || Rowcount ==0)
			return;
		texture = new GLuint[NumberofImages];
		int Texturecount=0;		
		double leftx =-2, lefty = 2 ;
		double width =2*abs(leftx)/Columncount , height = width/Ratio;

		unsigned char * data;
		data = (unsigned char *)malloc( bpwidth * bpheight * 4 + 54);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glGenTextures( NumberofImages, texture ); 		
		
		for(std::map<int, std::string>::iterator itr= imagepath.begin(); itr!=imagepath.end() ; itr++)
		{
			FILE * file;
			file = fopen( (const char*)((*itr).second).c_str(), "rb" );
			if ( file == NULL )
				continue;				
			fread( data, bpwidth * bpheight * 4 + 54, 1, file );
			fclose( file );

			glBindTexture( GL_TEXTURE_2D, texture[Texturecount] );
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL ); //set texture environment parameters //
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, bpwidth , bpheight , 0,GL_BGRA_EXT, GL_UNSIGNED_BYTE, (data + 54));
			glEnable(GL_TEXTURE_2D);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			/*glLoadIdentity();*/

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

			/*glLoadIdentity();*/
			int j = ((*itr).first) % Columncount, i= ((*itr).first)/Columncount;				
			double x = leftx + j*width , y=lefty- i*height;

			glBegin(GL_QUADS);		
			glTexCoord2f(0.0,1);	glVertex3d(x,y,0.9);
			glTexCoord2f(1.0,1);	glVertex3d(x+width,y,0.9);    
			glTexCoord2f(1.0,0);   	glVertex3d(x+width,y-height,0.9);
			glTexCoord2f(0.0,0);	glVertex3d(x,y-height,0.9);		
		
			glEnd();

			Texturecount++;
		}		
	
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		/*glDeleteTextures( 1, &texture );*/
		glDeleteTextures( NumberofImages, texture );
		delete [] texture;
		free(data);
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::DrawVideoTextureBackGround(unsigned char* pixArray, int bpwidth, int bpheight, char* FPath)
{
	try
	{
		unsigned char * data;
		FILE * file;
		file = fopen( (const char*)FPath, "rb" );
		if ( file == NULL )
			return;
		//data = (unsigned char *)malloc( 3664 * 2748 * 3 );
		//fread( data, 3664 * 2748 * 3, 1, file );
		data = (unsigned char *)malloc( bpwidth * bpheight * 3 + 54);
		fread( data, bpwidth * bpheight * 3 + 54, 1, file );
		fclose( file );


		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glGenTextures( 1, &texture ); //generate the texture with the loaded data
		glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to it’s array
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL ); //set texture environment parameters //
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, bpwidth , bpheight , 0,GL_BGR_EXT, GL_UNSIGNED_BYTE, (data + 54));
		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		
		/*glBindTexture( GL_TEXTURE_2D, texture );
		glBegin(GL_QUADS);
		glTexCoord2f(0.0,1);
		glVertex3i(-1,-1,1);
		glTexCoord2f(1,1);
		glVertex3i(1,-1,1);    
		glTexCoord2f(1,0);
		glVertex3i(1,1,1);
		glTexCoord2f(0,0);
		glVertex3i(-1,1,1);*/

		//glBindTexture( GL_TEXTURE_2D, texture );
		//glTranslatef(0,0,0.1);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0,1);	glVertex3d(-1,1,0.9);
		glTexCoord2f(1.0,1);	glVertex3d(1,1,0.9);    
		glTexCoord2f(1.0,0);   	glVertex3d(1,-1,0.9);
		glTexCoord2f(0.0,0);	glVertex3d(-1,-1,0.9);
		
		glEnd();
		glDisable(GL_TEXTURE_2D);
//		gluScaleImage(GL_RGB, bpwidthn, bpheight, GLenum typein, const void *datain, GLint widthout, GLint heightout, GLenum typeout, void *dataout);
	/*	glRasterPos2i(-1, -1);
		 glDrawPixels(bpwidth, bpheight, GL_RGB, GL_UNSIGNED_BYTE, pixArray);*/
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glDeleteTextures( 1, &texture );

		free(data);
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//dimensioning functions
//draw point to point distance...//
void RGraphicsDraw::drawPoint_PointDistance(double x1, double y1, double x2, double y2, double x3, double y3, double disValue, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double p1[2] = {x1, y1}, p2[2] = {x2, y2}, p3[2] = {x3, y3};
		double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s1 = 0;
		RMATH2DOBJECT->Pt2Ptdis(&p1[0], &p2[0], &p3[0], &DisString[0], disValue,&e1[0], &e2[0], &m1[0], &s1);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
		/*draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine(e1, e2);
		drawarrow(&m1[0], &s1, &e1[2], &e2[2], &disValue, &DisString[0], w_upp);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0020", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_PointDistance_Linear(double x1, double y1, double x2, double y2, bool flag, double x3, double y3, double d, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s2 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double m[2] = {x3, y3}, p1[2] = {x1, y1}, p2[2] = {x2, y2};
		RMATH2DOBJECT->XY_shift(&p1[0], &p2[0], flag, &m[0], &DisString[0], d, &e1[0], &e2[0], &m1[0], &s2);
		e2[0] = p2[0]; e1[0] = p1[0];
		e2[1] = p2[1]; e1[1] = p1[1];
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0021", __FILE__, __FUNCSIG__); }
}

//Draw line to point distance measure....//
void RGraphicsDraw::drawPoint_FinitelineDistance(double x1, double y1, double slope, double intercept, double *endp1, double *endp2, double x, double y, double Dist, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double p1[2] = {x1,y1}, m2[2] = {x, y}, intercept1 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		RMATH2DOBJECT->Point_FinitelineDistance(&p1[0], slope, intercept,endp1,endp2,&intercept1, Dist, &m2[0],&DisString[0],&e1[0], &e2[0],&m1[0],&s2);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], tmp1, tmp2, w_upp);
		/*drawCommonProperties_AllMeasure2D(e1, e2,DisString,m1, m2, s2, intercept1, intercept, Dist, distance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, 0);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0022", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_InfinitelineDistance(double x1, double y1, double slope, double intercept, double x, double y, double Dist, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double p1[2] = {x1,y1}, m2[2] = {x, y}, intercept1 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		RMATH2DOBJECT->Point_InfinitelineDistance(&p1[0], slope, intercept,&intercept1, Dist, &m2[0],&DisString[0],&e1[0], &e2[0],&m1[0],&s2);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], tmp1, tmp2, w_upp);
		/*drawCommonProperties_AllMeasure2D(e1, e2,DisString,m1, m2, s2, intercept1, intercept, Dist, distance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, 0);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0023", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_RayDistance(double x1, double y1, double slope, double intercept, double *endp1, double x, double y, double Dist, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double p1[2] = {x1,y1}, m2[2] = {x, y}, intercept1 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		RMATH2DOBJECT->Point_RayDistance(&p1[0], slope, intercept,endp1, &intercept1, Dist, &m2[0],&DisString[0],&e1[0], &e2[0],&m1[0],&s2);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], tmp1, tmp2, w_upp);
		/*drawCommonProperties_AllMeasure2D(e1, e2,DisString,m1, m2, s2, intercept1, intercept, Dist, distance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, 0);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0024", __FILE__, __FUNCSIG__); }
}




void RGraphicsDraw::drawCircle_FinitelineDistance(double slope, double intercept, double *endp1, double *endp2, double *center, double radius, double x, double y, double Dist, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double m2[2] = {x, y}, intercept1 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[6],e2[6],m1[3],s2;
		RMATH2DOBJECT->Circle_FinitelineDistance(slope, intercept,endp1,endp2,center, radius, &intercept1,  &m2[0],&DisString[0],Dist,&e1[0], &e2[0],&m1[0],&s2, type);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[0], &tmp2[0], w_upp);
		///*drawCommonProperties_AllMeasure2D(e1, e2,DisString,m1, m2, s2, intercept1, intercept, Dist, distance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0025", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCircle_InfinitelineDistance(double slope, double intercept, double *center, double radius, double x, double y, double Dist, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double m2[2] = {x, y}, intercept1 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s2 = 0;
		RMATH2DOBJECT->Circle_InfinitelineDistance(slope, intercept,center, radius, &intercept1,  &m2[0],&DisString[0],Dist,&e1[0], &e2[0],&m1[0],&s2, type);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[0], &tmp2[0], w_upp);
		/*drawCommonProperties_AllMeasure2D(e1, e2, DisString, m1, m2, s2, intercept1, intercept, Dist, distance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0026", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCircle_RayDistance(double slope, double intercept, double *endp1, double *center, double radius, double x, double y, double Dist, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double m2[2] = {x, y}, intercept1 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		RMATH2DOBJECT->Circle_RayDistance(slope, intercept,endp1,center, radius, &intercept1,  &m2[0],&DisString[0],Dist,&e1[0], &e2[0],&m1[0],&s2, type);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[0], &tmp2[0], w_upp);
		/*drawCommonProperties_AllMeasure2D(e1, e2,DisString,m1, m2, s2, intercept1, intercept, Dist, distance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0027", __FILE__, __FUNCSIG__); }
}

//Draw circle to point measurements...//
void RGraphicsDraw::drawCircle_PointDistance(double x1, double y1,double radius, double x2, double y2,double d, double x3, double y3, char* distance, char* MeasureName,int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s2 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double c1[2] = {x2, y2}, m[2] = {x3, y3}, p1[2] = {x1,y1};
		RMATH2DOBJECT->Circle_PointDistance(&p1[0],&c1[0],radius,&m[0],&DisString[0],d,&e1[0], &e2[0],&m1[0],&s2,type);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0028", __FILE__, __FUNCSIG__); }
}

//Draw point to circle linear measurements.. 6 distances...
void RGraphicsDraw::drawCircle_PointDistance_Linear(double x1, double y1,double radius, double x2, double y2,double d, double x3, double y3, char* distance, char* MeasureName, int type, bool fl, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double c1[2] = {x1, y1}, m[3] = {x3, y3, 0}, p1[2] = {x2,y2};
		RMATH2DOBJECT->Circle_PointDistance_Linear(&c1[0],radius,&p1[0], &m[0],&DisString[0],d,&e1[0], &e2[0],&m1[0],&s2,type);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0029", __FILE__, __FUNCSIG__); }
}

//Draw Circle to circle distance...//
void RGraphicsDraw::drawCircle_CircleDistance(double x1, double y1, double radius1, double x2, double y2, double radius2, double distan, double x3, double y3, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{

		double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s2 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double c1[3] = {x1, y1, 0}, c2[3] = {x2, y2, 0}, m[3] = {x3, y3, 0};
		RMATH2DOBJECT->Circle_CircleDistance(&c1[0], &c2[0], radius1, radius2, &m[0], &DisString[0], distan, &e1[0], &e2[0], &m1[0], &s2, type);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0030", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCircle_CircleDistance(double x1, double y1, double radius1, double x2, double y2, double radius2, double distan, double x3, double y3, char* distance, char* MeasureName, int type1, int type2, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s2 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double c1[3] = {x1, y1, 0}, c2[3] = {x2, y2, 0}, m[3] = {x3, y3, 0};
		double dir[3] = {x2-x1, y2-y1, 0}, dircos[3] = {0};
		RMATH3DOBJECT->DirectionCosines(dir, dircos);
		double pt1[2] = {0}, pt2[2] = {0};
		if(type1 == 0)
		{
			for(int i = 0; i < 2; i++)
			{
				pt1[i] = c1[i] + radius1 * dircos[i];
			}
		}
		else if(type1 == 1)
		{
			for(int i = 0; i < 2; i++)
			{
				pt1[i] = c1[i];
			}
		}
		else if(type1 == 2)
		{
			for(int i = 0; i < 2; i++)
			{
				pt1[i] = c1[i] - radius1 * dircos[i];
			}
		}
		if(type2 == 0)
		{
			for(int i = 0; i < 2; i++)
			{
				pt2[i] = c2[i] - radius2 * dircos[i];
			}
		}
		else if(type2 == 1)
		{
			for(int i = 0; i < 2; i++)
			{
				pt2[i] = c2[i];
			}
		}
		else if(type2 == 2)
		{
			for(int i = 0; i < 2; i++)
			{
				pt2[i] = c2[i] + radius2 * dircos[i];
			}
		}
		RMATH2DOBJECT->Pt2Ptdis(pt1,pt2, m, &DisString[0], distan, &e1[0], &e2[0], &m1[0], &s2);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0030", __FILE__, __FUNCSIG__); }
}

//Circle to circle Linear measurement.. X and Y Shifts.. total 6 distances....//
void RGraphicsDraw::drawCircle_CircleDistance_Linear(double x1, double y1, double radius1, double x2, double y2, double radius2, double distan, double x3, double y3, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double c1[2] = {x1, y1}, c2[2] = {x2, y2}, m[2] = {x3, y3};
		RMATH2DOBJECT->Circle_CircleDistance_Linear(&c1[0], &c2[0], radius1, radius2, &m[0], &DisString[0], distan, &e1[0], &e2[0], &m1[0], &s2, type);
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0031", __FILE__, __FUNCSIG__); }
}

//draw Line to Line measurement...//
void RGraphicsDraw::drawInfiniteline_InfinitelineDistance(double slope1, double intercept1, double slope2, double intercept2,double distance, double x, double y, char* strdistance , char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double m[2] = {x, y};
		RMATH2DOBJECT->Infiniteline_InfinitelineDistance(slope1,intercept1,slope2,intercept2,distance,&m[0],&DisString[0],&e1[0],&e2[0],&m1[0],&s2);
	/*	drawCommonProperties_AllMeasure2D(e1, e2, DisString,m1, m, s2, intercept1, intercept2, distance, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, strdistance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0032", __FILE__, __FUNCSIG__); }
}

//draw finite line to finite line distance measurement.....///
void RGraphicsDraw::drawFiniteline_FinitelineDistance( double *endp11,double *endp12,double *endp21,double *endp22, double slope1,double intercept1,double slope2,double intercept2,double distance, double x, double y, char* strdistance , char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double	m[2] = {x, y};
		RMATH2DOBJECT->Finiteline_FinitelineDistance(endp11,endp12,endp21,endp22,slope1,intercept1,slope2,intercept2,distance,&m[0],&DisString[0],&e1[0],&e2[0],&m1[0],&s2);
		/*drawCommonProperties_AllMeasure2D(e1, e2, DisString,m1, m, s2, intercept1, intercept2, distance, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, strdistance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0033", __FILE__, __FUNCSIG__); }
}

//Draw xray to xray measurement...///
void RGraphicsDraw::drawRay_RayDistance(double *endp1,double slope1, double intercept1, double *endp2,double slope2, double intercept2,double distance, double x, double y, char* strdistance , char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double	m[2] = {x, y};
		RMATH2DOBJECT->Ray_RayDistance(endp1,slope1,intercept1,endp2,slope2,intercept2,distance,&m[0],&DisString[0],&e1[0],&e2[0],&m1[0],&s2);
	/*	drawCommonProperties_AllMeasure2D(e1, e2, DisString,m1, m, s2, intercept1, intercept2, distance, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, strdistance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0034", __FILE__, __FUNCSIG__); }
}

//Draw xline to line width measurement...//
void RGraphicsDraw::drawInfiniteline_FinitelineDistance( double slope1, double intercept1,double *endp1, double *endp2,double slope2, double intercept2,double distance, double x, double y, char* strdistance , char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double	m[2] = {x, y};
		RMATH2DOBJECT->Infiniteline_FinitelineDistance(slope1,intercept1,endp1,endp2,slope2,intercept2,distance,&m[0],&DisString[0],&e1[0],&e2[0],&m1[0],&s2);
	/*	drawCommonProperties_AllMeasure2D(e1, e2, DisString,m1, m, s2, intercept1, intercept2, distance, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, strdistance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0035", __FILE__, __FUNCSIG__); }
}

//Draw Xline to Xray widht measurement...//
void RGraphicsDraw::drawInfiniteline_RayDistance(double slope1, double intercept1,double *endp1,double slope2, double intercept2,double distance, double x, double y, char* strdistance , char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0},e2[6] = {0},m1[3] = {0},s2 = 0;
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double	m[2] = {x, y};
		RMATH2DOBJECT->Infiniteline_RayDistance(slope1,intercept1,endp1,slope2,intercept2,distance,&m[0],&DisString[0],&e1[0],&e2[0],&m1[0],&s2);
		/*drawCommonProperties_AllMeasure2D(e1, e2, DisString,m1, m, s2, intercept1, intercept2, distance, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, strdistance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0036", __FILE__, __FUNCSIG__); }
}

//Draw line to xray measurement...//
void RGraphicsDraw::drawFiniteline_RayDistance(double *endp1, double *endp2,double slope1, double intercept1, double *endp3,double slope2, double intercept2,double distance, double x, double y, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s2 = 0;
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double	m[2] = {x, y};
		RMATH2DOBJECT->Finiteline_RayDistance(endp1,endp2,slope1, intercept1,endp3,slope2,intercept2,distance,&m[0],&DisString[0],&e1[0],&e2[0],&m1[0],&s2);
		/*drawCommonProperties_AllMeasure2D(e1, e2, DisString,m1, m, s2, intercept1, intercept2, distance, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);*/
		m1[2] = Zlevel;
		double tmp1[9] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel};
		double tmp2[9] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel};
		draw_MeasurementString2D(m1, DisString, strdistance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawMeasureLine3D(tmp1, tmp2);
		CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0037", __FILE__, __FUNCSIG__); }
}



void RGraphicsDraw::drawFiniteline_FinitelineAngle(double slope1,double intercept1,double *endp11,double *endp12, double slope2,double intercept2,double *endp21,double *endp22,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double m[2] = {x, y};
		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Finiteline_FinitelineAngle(slope1,intercept1,endp11,endp12,slope2,intercept2,endp21,endp22, &m[0], &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
		drawCommonProperties_Angle2D(pt, center, startang, sweepang, radius, slop, DisString, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0038", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawInfiniteline_InfinitelineAngle(double slope1,double intercept1,double slope2,double intercept2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double m[2] = {x, y};
		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Infiniteline_InfinitelineAngle(slope1,intercept1,slope2,intercept2,&m[0], &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
		drawCommonProperties_Angle2D(pt, center, startang, sweepang, radius, slop, DisString, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0040", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawRay_RayAngle(double slope1,double intercept1,double *endp1, double slope2,double intercept2,double *endp2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double m[2] = {x, y};
		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Ray_RayAngle(slope1,intercept1,endp1,slope2,intercept2,endp2,&m[0], &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
		drawCommonProperties_Angle2D(pt, center, startang, sweepang, radius, slop, DisString, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0041", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawInfiniteline_FinitelineAngle(double slope1,double intercept1,double *endp1,double *endp2, double slope2,double intercept2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double m[2] = {x, y};
		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Infiniteline_FinitelineAngle(slope1,intercept1,endp1,endp2,slope2,intercept2,&m[0], &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
		drawCommonProperties_Angle2D(pt, center, startang, sweepang, radius, slop, DisString, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0042", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawInfiniteline_RayAngle(double slope1,double intercept1,double *endp1,double slope2,double intercept2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double m[2] = {x, y};
		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Infiniteline_RayAngle(slope1,intercept1,endp1,slope2,intercept2,&m[0], &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
		drawCommonProperties_Angle2D(pt, center, startang, sweepang, radius, slop, DisString, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0043", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_RayAngle(double slope1,double intercept1,double *endp1,double *endp2, double slope2,double intercept2,double *endp3,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double m[2] = {x, y};
		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Finiteline_RayAngle(slope1,intercept1,endp1,endp2,slope2,intercept2,endp3,&m[0], &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
		drawCommonProperties_Angle2D(pt, center, startang, sweepang, radius, slop, DisString, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0044", __FILE__, __FUNCSIG__); }
}

//Draw the line slope measurement....//
void RGraphicsDraw::drawSlope(double slope, double intercept, double x, double y, double angle1, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, p[6] = {0}, sl[2] = {0}, mp[2] = {x, y}, transM[9] = {1,0,0,0,1,0,0,0,1};
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		RMATH2DOBJECT->LineSlope(slope, intercept, &mp[0], &DisString[0], &radius, &center[0], &startang[0], &sweepang[0], &p[0], &sl[0]);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawArc3D(center[0], center[1], radius, startang[0], sweepang[0], true, Zlevel, transM);
		drawArc3D(center[0], center[1], radius, startang[1], sweepang[1], true, Zlevel, transM);
		p[2] = Zlevel;
		draw_MeasurementString2D(p, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawarrow(&p[2], sl[0], w_upp, Zlevel);
		drawarrow(&p[4], sl[1], w_upp, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0049", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPCDAngle(double centerX, double centerY, double slope1, double intercept1, double slope2, double intercept2, double mouseX, double mouseY, bool drawdir, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
		double mp[2] = {mouseX, mouseY}, center[2] = {centerX, centerY};
		double radius = 0, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->PCDAngleCalculation(slope1, intercept1, slope2, intercept2, &center[0], &mp[0], drawdir, &DisString[0], &radius, &startang[0], &sweepang[0], &pt[0], &slop[0]);
		drawCommonProperties_Angle2D(pt, center, startang, sweepang, radius, slop, DisString, strdistance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0039", __FILE__, __FUNCSIG__); }
}




void RGraphicsDraw::drawDiameter(double x1, double y1, double x2, double y2, double d, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double DisString1[2] = {getTextWidth(distance) * Fscalefactor, getTextWidth(MeasureName) * Fscalefactor};
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		if(DisString1[1] > DisString1[0])DisString[0] = DisString1[1];
		double m[2] = {x2, y2}, p1[2] = {x1, y1}, e1[9], sl2 = 0;
		RMATH2DOBJECT->Circle_diameter(&p1[0], d, &m[0], &DisString[0], &e1[0], &sl2);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		e1[8] = Zlevel;
		double temp[6] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel};
		drawLine_3D(e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel);
		drawLine_3D(e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel);
		DrawArrowCone(&temp[0], &temp[3] , w_upp);
		//if (windowno == 0)
		//	drawString_BackGroud(e1[6], e1[7], 0, 1.2 * max(getTextWidth(distance), getTextWidth(MeasureName)) * Fscalefactor,
		//		1.5 * (getTexHeight(distance) + getTexHeight(MeasureName)) * Fscalefactor, 0, 0, 0);
		draw_MeasurementString2D(&e1[6], DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0058", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawArc_Radius(double *center, double shapeR, double *MouseSelectnLine, double *TransMatrix, double *MeasurementPlane, double start, double sweep, double *lastmpX, double *lastmpY,char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		//double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		//double m[2] = {x2, y2}, p1[2] = {x1, y1}, e1[9], sl2;
		//RMATH2DOBJECT->Arc_radius(&p1[0] ,r, &m[0], start, sweep, &DisString[0], &e1[0], lastmpX, lastmpY, &sl2);
		//glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		//drawLine_3D(e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel);
		//drawLine_3D(e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel);
		//sl2 = RMATH2DOBJECT->ray_angle(&e1[0], &e1[2]);
		//e1[8] = Zlevel;
		//double temp[6] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel};
		//DrawArrowCone(&temp[0], &temp[3] , w_upp);
		//draw_MeasurementString2D(&e1[6], DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);


		double e1[8] =  {0}, MousePos[3] = {0}, InverseTransMatrix[9] = {0}, sl2 = 0;
		double TransCentre[3] = {0}, TransMPos[3] = {0}, FinalEnd[9] = {0}, TextPos[3] = {0};
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);

		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, MousePos, Order2, TransMPos);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, center, Order2, TransCentre);
		
	/*	RMATH2DOBJECT->Circle_diameter(TransCentre, shapeDia, TransMPos, &DisString[0], &e1[0], &s2);*/
		RMATH2DOBJECT->Arc_radius(TransCentre , shapeR, TransMPos, start, sweep, &DisString[0], &e1[0], lastmpX, lastmpY, &sl2);
		/*lastmpX = TransMPos[0];*/

		double TransEnd1[3] = {e1[0], e1[1], TransCentre[2]}, TransEnd2[3] = {e1[2], e1[3], TransCentre[2]},
			   TransEnd3[3] = {e1[4], e1[5], TransCentre[2]}, TransTextPos[3] = {e1[6], e1[7], TransCentre[2]};

		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd1[0], Order2, &FinalEnd[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd2[0], Order2, &FinalEnd[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd3[0], Order2, &FinalEnd[6]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransTextPos[0], Order2, TextPos);

		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(FinalEnd[0], FinalEnd[1], FinalEnd[2], FinalEnd[3], FinalEnd[4], FinalEnd[5]);
		drawLine_3D(FinalEnd[3], FinalEnd[4], FinalEnd[5], FinalEnd[6], FinalEnd[7], FinalEnd[8]);

		//if (windowno == 0)
		//	drawString_BackGroud(TextPos[0], TextPos[1], 0, 1.2 * max(getTextWidth(distance), getTextWidth(MeasureName)) * Fscalefactor,
		//										  1.5 * (getTexHeight(distance) + getTexHeight(MeasureName)) * Fscalefactor, 0, 0, 0);

		//Draw the measurement strings
		draw_MeasurementString3D(TextPos, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);

		//Draw the arrow cones
		double ArrowHeadPt[3] = {FinalEnd[0], FinalEnd[1], FinalEnd[2]};
		double ArrowTailPt[3] = {FinalEnd[3], FinalEnd[4], FinalEnd[5]};
		DrawArrowCone(ArrowHeadPt, ArrowTailPt, w_upp);

	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0059", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawArclength(double x1,double y1,double x2,double y2,double *endp1, double *endp2,double r, double start, double sweep, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[10] = {0},sl[2] = {0};
		double DisString[2] = {getTextWidth(distance) * Fscalefactor , getTexHeight(distance) * Fscalefactor};
		double m[2] = {x2, y2}, p1[2] = {x1, y1}, r1, st[2], sw[2], transM[9] = {1,0,0,0,1,0,0,0,1};
		RMATH2DOBJECT->Arc_Angle(&p1[0], r, start, sweep, endp1, endp2, &m[0], &DisString[0], &e1[0], &st[0], &sw[0], &r1, &sl[0]);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3DStipple(e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel);
		drawLineStipple(e1[6], e1[7], Zlevel, e1[8], e1[9], Zlevel);
		drawArc3D(p1[0], p1[1], r1, st[0], sw[0], true, Zlevel, transM);
		drawArc3D(p1[0], p1[1], r1, st[1], sw[1], true, Zlevel, transM);
		drawarrow(&e1[4], sl[0], w_upp, Zlevel);
		drawarrow(&e1[8], sl[1], w_upp, Zlevel);
		double temp[3] = {e1[0], e1[1], Zlevel};
		draw_MeasurementString2D(temp, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0060", __FILE__, __FUNCSIG__); }
}



//Draw digital micrometer..//
void RGraphicsDraw::drawDigital_Micrometer(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3, double dis, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, bool nearMousePt, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		double e1[8] = {0}, e2[8] = {0}, m1[3] = {0}, s2 = 0;
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double p1[2] = {x, y}, p2[2] = {x1, y1}, p3[2] = {x2, y2};
		RMATH2DOBJECT->Digital_Micrometer(&p1[0], &p2[0], dis, &DisString[0], 30 * w_upp, &e1[0], &e2[0], &m1[0], &s2, nearMousePt);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		m1[2] = Zlevel;
		double tmp1[12] = {e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel, e1[6], e1[7], Zlevel};
		double tmp2[12] = {e2[0], e2[1], Zlevel, e2[2], e2[3], Zlevel, e2[4], e2[5], Zlevel, e2[6], e2[7], Zlevel};
		drawLine_3D(tmp1[0], tmp1[1], tmp1[2], tmp1[3], tmp1[4], tmp1[5]);
		drawLine_3D(tmp2[0], tmp2[1], tmp2[2], tmp2[3], tmp2[4], tmp2[5]);
		drawLine_3D(tmp1[6], tmp1[7], tmp1[8], tmp1[9], tmp1[10], tmp1[11]);
		drawLine_3D(tmp2[6], tmp2[7], tmp2[8], tmp2[9], tmp2[10], tmp2[11]);
		drawLine_3DStipple(x, y, Zlevel, x2, y2, Zlevel);
		drawLine_3DStipple(x1, y1, Zlevel, x3, y3, Zlevel);
		CalculateAndDrawArrowCones(&tmp1[6], &tmp2[6], &tmp1[9], &tmp2[9], w_upp);
		draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		/*	draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		drawarrow(&m1[0], &s2, &e1[4], &e2[4], &dis, &DisString[0], w_upp);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0053", __FILE__, __FUNCSIG__); }
}




void RGraphicsDraw::drawCommonProperties_AllMeasure2D(double e1[], double e2[],double Dis[],double m1[], double m2[],double slopear, double intercept1, double intercept2, double Dist, char* Distance, char* MeasureName, int windowno, double w_upp, double FScaleFactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		draw_MeasurementString2D(m1, Dis, Distance, MeasureName, windowno, FScaleFactor, Lefttop, RightBtm);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLineStipple(e1[0], e1[1], e1[2], e1[3]);
		drawLineStipple(e2[0], e2[1], e2[2], e2[3]);
		drawLineStipple(e1[2], e1[3], e1[4], e1[5]);
		drawLineStipple(e2[2], e2[3], e2[4], e2[5]);
		if(intercept1 < intercept2)
			drawarrow(&m2[0], &slopear, &e1[2], &e2[2], &Dist, &Dis[0], w_upp);
		else
			drawarrow(&m2[0], &slopear, &e2[2], &e1[2], &Dist, &Dis[0], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0050", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_MeasurementString2D(double m1[], double Dis[], char* Distance, char* MeasureName, int windowno, double FScaleFactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double lefttopx = m1[0] - 2 * Dis[0]/3, lefttopy = m1[1] + 3 * Dis[1]/2;
		double rightbottomx = m1[0] + 2 * Dis[0]/3, rightbottomy = m1[1] - 3 * Dis[1]/2;
		Lefttop[0] = lefttopx; Lefttop[1] = lefttopy;
		Lefttop[3] = m1[0]; Lefttop[4] = m1[1]; Lefttop[5] = m1[2];
		RightBtm[0] = rightbottomx; RightBtm[1] = rightbottomy;
		//double StrWidth[2] = {max(getTextWidth(Distance), getTextWidth(MeasureName)), (getTexHeight(Distance) + getTexHeight(MeasureName))};
		//StrWidth[0] = 1.2 * StrWidth[0] * FScaleFactor;
		//StrWidth[1] = 1.5 * StrWidth[1] * FScaleFactor;

		//if (windowno == 0)
		//	drawString_BackGroud(m1[0], m1[1], 0, StrWidth[0], StrWidth[1], 0, 0, 0);

		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		
		drawStringEdited3D(m1[0], m1[1], m1[2], 0, Distance, FScaleFactor);
		drawStringEdited3D1(m1[0], m1[1], m1[2], 0, MeasureName, FScaleFactor);
		//if(windowno == 0)
		//	drawStringWith_BackGroud(m1[0], m1[1], 0, 0, Distance, FScaleFactor, 0, 0, 0);// Measure_Color.r, Measure_Color.g, Measure_Color.b);
		//else
		//	drawString3D(m1[0], m1[1], 0, 0, Distance, FScaleFactor);
		//drawString3D1(m1[0], m1[1], 0, 0, MeasureName, FScaleFactor);*/
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0051", __FILE__, __FUNCSIG__); }
}

//Draw the line segments.. Used for all the measurements...//
void RGraphicsDraw::drawMeasureLine( double e1[], double e2[])
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLineStipple(e1[0], e1[1], e1[2], e1[3]);
		drawLineStipple(e2[0], e2[1], e2[2], e2[3]);
		drawLineStipple(e1[2], e1[3], e1[4], e1[5]);
		drawLineStipple(e2[2],e2[3], e2[4],e2[5]);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0052", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCommonProperties_Angle2D(double pt[], double center[], double startang[], double sweepang[], double radius, double slop[], double DisString[], char* strdistance, char* MeasureName, int windowno, double w_upp, double FScaleFactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		double TransformMatrix[16] = {1,0,0,0,1,0,0,0,1};
		double temp[12] = {pt[2], pt[3], Zlevel, pt[4], pt[5], Zlevel, pt[6], pt[7], Zlevel, pt[8], pt[9], Zlevel};
		drawLine_3DStipple(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
		drawLine_3DStipple(temp[6], temp[7], temp[8], temp[9], temp[10], temp[11]);
		drawArc3D(center[0], center[1], radius, startang[0], sweepang[0], true, Zlevel, TransformMatrix);
		drawArc3D(center[0], center[1], radius, startang[1], sweepang[1], true, Zlevel, TransformMatrix);
		CalculateAndDrawArrowConesAng(&temp[0], &temp[6], slop, TransformMatrix, w_upp);
		double m1[3] = {pt[0], pt[1], Zlevel};
		draw_MeasurementString2D(m1, DisString, strdistance, MeasureName, windowno, FScaleFactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0045", __FILE__, __FUNCSIG__); }
}

//Draw arrow..//
void RGraphicsDraw::drawarrow(double *m1, double *sl, double *x1, double *x2, double *dis, double *space, double w_upp)
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		double p1[2] = {0}, p2[2] = {0}, p3[2] = {0}, p4[2] = {0};
		RMATH2DOBJECT->arrow(m1, *sl, x1, x2, *dis, w_upp * ArrowSize, space, &p1[0], &p2[0], &p3[0], &p4[0]);
		drawTriangle(x1[0], x1[1], p1[0], p1[1], p2[0], p2[1], true);
		drawTriangle(x2[0], x2[1], p3[0], p3[1], p4[0], p4[1], true);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0046", __FILE__, __FUNCSIG__); }
}

//Draw arrow...//
void RGraphicsDraw::drawarrow(double *endpt, double sl, double w_upp, double Zlevel)
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		double p1[2] = {0}, p2[2] = {0}, sl1 = sl + M_PI / 15, sl2 = sl - M_PI / 15;
		RMATH2DOBJECT->arrow1(endpt, sl1, sl2, &p1[0], &p2[0], w_upp * ArrowSize);
		drawTriangle(endpt[0],endpt[1], p1[0], p1[1], p2[0], p2[1], true);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0047", __FILE__, __FUNCSIG__); }
}

//Draw arrow...//
void RGraphicsDraw::drawarrow1(double *endpt,double sl, double w_upp)
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		double p1[2], p2[2], sl1 = sl + M_PI / 15, sl2 = sl - M_PI / 15;
		RMATH2DOBJECT->arrow2(endpt, sl1, sl2, &p1[0], &p2[0], w_upp * ArrowSize);
		drawTriangle(endpt[0],endpt[1], p1[0], p1[1], p2[0], p2[1], true);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0048", __FILE__, __FUNCSIG__); }
}

 


void RGraphicsDraw::translateMatrix(double x, double y, double z)
{
	glTranslated(x, y, z);
}

void RGraphicsDraw::drawArcFGDir(double x,double y, double *p1, double radius, double startangle, double sweepangle)
{
	try
	{
		double x1 = 0,y1 = 0,x2 = 0,y2 = 0;
		double st, endangle = startangle + sweepangle;
		for(st = startangle; st <= endangle - IncrementAngle * 4; st += IncrementAngle)
		{
			x1 = (radius * cos(st)) + x;
			y1 = (radius * sin(st)) + y;
			x2 = (radius * cos(st + IncrementAngle * 4)) + x;
			y2 = (radius * sin(st + IncrementAngle * 4)) + y;
			glBegin(GL_POLYGON);
			glVertex2d(x1,y1);
			glVertex2d(x2,y2);
			glVertex2d(p1[0],p1[1]);
			glEnd();
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0056", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFillArcFG(double x, double y, double radius1, double radius2, double startangle, double sweepangle)
{
	try
	{
		double x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0, x4 = 0, y4 = 0;
		double st = 0, endangle = startangle + sweepangle;
		double Increment_Ang = IncrementAngle / 2;
		//if (sweepangle < IncrementAngle)
		//	Increment_Ang = IncrementAngle / 8;
		//else if (sweepangle < IncrementAngle / 4)
		//	Increment_Ang = IncrementAngle / 4;
		//else 
		int stepNumber = sweepangle / IncrementAngle;
		if (stepNumber < 12)
		{
			if (sweepangle < IncrementAngle)
				Increment_Ang = sweepangle / 12;
			else
				Increment_Ang = IncrementAngle;
		}
		else
			Increment_Ang = IncrementAngle * 4;

		/*if (sweepangle < IncrementAngle * 8)
			Increment_Ang = IncrementAngle;
		else
			Increment_Ang = IncrementAngle * 4;

		for(st = startangle; st <= endangle - Increment_Ang; st += IncrementAngle)
		{
			x1 = (radius1 * cos(st)) + x;
			y1 = (radius1 * sin(st)) + y;
			x2 = (radius1 * cos(st + Increment_Ang)) + x;
			y2 = (radius1 * sin(st + Increment_Ang)) + y;
			x3 = (radius2 * cos(st)) + x;
			y3 = (radius2 * sin(st)) + y;
			x4 = (radius2 * cos(st + Increment_Ang)) + x;
			y4 = (radius2 * sin(st + Increment_Ang)) + y;
			glBegin(GL_POLYGON);
			glVertex2d(x1, y1);
			glVertex2d(x2, y2);
			glVertex2d(x3, y3);
			glVertex2d(x4, y4);
			glVertex2d(x1, y1);
			glEnd();
		}
		for(st = endangle; st >= startangle + Increment_Ang; st -= IncrementAngle)
		{
			x1 = (radius1 * cos(st)) + x;
			y1 = (radius1 * sin(st)) + y;
			x2 = (radius1 * cos(st - Increment_Ang)) + x;
			y2 = (radius1 * sin(st - Increment_Ang)) + y;
			x3 = (radius2 * cos(st)) + x;
			y3 = (radius2 * sin(st)) + y;
			x4 = (radius2 * cos(st - Increment_Ang)) + x;
			y4 = (radius2 * sin(st - Increment_Ang)) + y;
			glBegin(GL_POLYGON);
			glVertex2d(x1, y1);
			glVertex2d(x2, y2);
			glVertex2d(x3, y3);
			glVertex2d(x4, y4);
			glVertex2d(x1, y1);
			glEnd();
			*/

			/*//Draw a direct concave polygon directly
			glBegin(GL_POLYGON);
			for (st = startangle; st < endangle; st += IncrementAngle)
			{
				x1 = (radius1 * cos(st)) + x;
				y1 = (radius1 * sin(st)) + y;
				glVertex2d(x1, y1);
			}
			x1 = (radius1 * cos(endangle)) + x;
			y1 = (radius1 * sin(endangle)) + y;
			glVertex2d(x1, y1);

			for (st = endangle; st > startangle; st -= IncrementAngle)
			{
				x1 = (radius2 * cos(st)) + x;
				y1 = (radius2 * sin(st)) + y;
				glVertex2d(x1, y1);
			}
			x1 = radius2 * cos(startangle) + x;
			y1 = radius2 * sin(startangle) + y;
			glVertex2d(x1, y1);
			x1 = radius1 * cos(startangle) + x;
			y1 = radius1 * sin(startangle) + y;
			glVertex2d(x1, y1);
			glEnd(); */

		glBegin(GL_TRIANGLE_STRIP);
		for (st = startangle; st < endangle; st += IncrementAngle)
		{
			x1 = (radius1 * cos(st)) + x;
			y1 = (radius1 * sin(st)) + y;
			glVertex2d(x1, y1);
			x1 = (radius2 * cos(st)) + x;
			y1 = (radius2 * sin(st)) + y;
			glVertex2d(x1, y1);

		}
		x1 = (radius1 * cos(endangle)) + x;
		y1 = (radius1 * sin(endangle)) + y;
		glVertex2d(x1, y1);
		x1 = radius2 * cos(endangle) + x;
		y1 = radius2 * sin(endangle) + y;
		glVertex2d(x1, y1);
		glEnd();

	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0057", __FILE__, __FUNCSIG__); }
}


//Draw circle .. fill the circle region..//
void RGraphicsDraw::drawFillCircle(double x, double y, double radius1, double radius2)
{
	try
	{
		double x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0, x4 = 0, y4 = 0;
		double cosangle = 0, sinangle = 0, cosangle1 = 0, sinangle1 = 0;
		//Fill the small rectangles in a loop to complete the circle
		for(double angle = 0.0; angle <= (M_PI * 2); angle += IncrementAngle)
		{
			cosangle = cos(angle); sinangle = sin(angle);
			cosangle1 = cos(angle + IncrementAngle * 4); sinangle1 = sin(angle + IncrementAngle * 4);
			x1 = radius1 * cosangle + x; y1 = radius1 * sinangle + y;
			x2 = radius1 * cosangle1 + x; y2 = radius1 * sinangle1 + y;
			x3 = radius2 * cosangle + x; y3 = radius2 * sinangle + y;
			x4 = radius2 * cosangle1 + x; y4 = radius2 * sinangle1 + y;
			
			glBegin(GL_POLYGON);
			glVertex2d(x1, y1);
			glVertex2d(x2, y2);
			glVertex2d(x3, y3);
			glVertex2d(x4, y4);
			glVertex2d(x1, y1);
			glEnd();
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}


void RGraphicsDraw::drawFocusRectangle(double row, double column, double gap, double width, double height, int ShiftInX, int ShiftInY)
{
	try
	{
		double pwidth = width * rwindow[0].UPP;
		double pHeight = height * rwindow[0].UPP;
		double pgap =  gap * rwindow[0].UPP;
		double fillExtend = 0.5 * rwindow[0].UPP;
		double startX = rwindow[0].cx - (/*(column * pwidth) + */((column - 1)* pgap)) / 2 /*+ (pwidth / 2)*/;
		startX += ShiftInX;
		double StartY = rwindow[0].cy + (/*(row * pHeight) +*/ ((row - 1) * pgap)) / 2 /*- (pHeight / 2)*/;
		StartY += ShiftInY;
		for(int i = 0; i < row; i++)
		{
			for(int j = 0; j < column; j++)
			{
				//GLdouble pts[24] = {((startX  + j * (pwidth + pgap)) + pwidth / 2) - fillExtend, (StartY - i * (pHeight + pgap) + pHeight / 2) - fillExtend,-146,
				//				((startX  + j * (pwidth + pgap)) + pwidth / 2) - fillExtend, (StartY - i * (pHeight + pgap) - pHeight / 2) + fillExtend,-146,
				//				 ((startX  + j * (pwidth + pgap)) - pwidth / 2) + fillExtend, (StartY - i * (pHeight + pgap) - pHeight / 2) + fillExtend,-146,
				//				  ((startX  + j * (pwidth + pgap)) - pwidth / 2) + fillExtend, (StartY - i * (pHeight + pgap) + pHeight / 2) - fillExtend,-146,
				//					(startX  + j * (pwidth + pgap)) - pwidth / 2, StartY - i * (pHeight + pgap) - pHeight / 2, -146,
				//					(startX  + j * (pwidth + pgap)) - pwidth / 2, StartY - i * (pHeight + pgap) + pHeight / 2, -146,
				//					 (startX  + j * (pwidth + pgap)) + pwidth / 2, StartY - i * (pHeight + pgap) + pHeight / 2, -146,
				//					  (startX  + j * (pwidth + pgap)) + pwidth / 2, StartY - i * (pHeight + pgap) - pHeight / 2, -146};

				GLdouble pts[24] = {(startX  + j * pgap + pwidth / 2) - fillExtend, (StartY - i * pgap + pHeight / 2) - fillExtend,-146,
				(startX  + j * pgap + pwidth / 2) - fillExtend, (StartY - i * pgap - pHeight / 2) + fillExtend,-146,
					(startX  + j * pgap - pwidth / 2) + fillExtend, (StartY - i * pgap - pHeight / 2) + fillExtend,-146,
					(startX  + j * pgap - pwidth / 2) + fillExtend, (StartY - i * pgap + pHeight / 2) - fillExtend,-146,
					(startX  + j * pgap) - pwidth / 2, (StartY - i * pgap - pHeight / 2), -146,
					(startX  + j * pgap) - pwidth / 2, StartY - i * pgap + pHeight / 2, -146,
						(startX  + j * pgap) + pwidth / 2, StartY - i * pgap + pHeight / 2, -146,
						(startX  + j *  pgap) + pwidth / 2, StartY - i * pgap - pHeight / 2, -146};


				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_DOUBLE, 0, pts);

				unsigned int indices[] = {0, 1, 2, 3, 0};
				unsigned int indices1[] = {4, 5, 6, 7, 4};

				glColor3d(Videoback_Color.r, Videoback_Color.g, Videoback_Color.b);
				glPushMatrix();
				//glTranslatef(0,0,51);
				glDrawElements(GL_POLYGON, 5, GL_UNSIGNED_INT, indices);
				glPopMatrix();
				glColor3d(Crosshair_Color.r, Crosshair_Color.g, Crosshair_Color.b);
				glDrawElements(GL_LINE_LOOP, 5, GL_UNSIGNED_INT, indices1);

				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0061", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPartprogramPathArrow(double x1, double y1, double x2, double y2, double wupp)
{
	try
	{
		double p1[2] = {x1, y1}, p2[2] = {x2, y2}, e1[8] = {0}, e2[6] = {0}, e3[6] = {0};
		double PtDist = RMATH2DOBJECT->Pt2Pt_distance(x1, y1, x2, y2);
		int PtDistPixel = (int) (PtDist / wupp);
		RMATH2DOBJECT->PartProgramDirection(&p1[0], &p2[0], &e1[0], &e2[0], &e3[0], 3 * wupp, 6 * wupp);
		EnableTransparency();
		glColor4d(0.1, 1, 0.1, 0.5);
		//glLineWidth(12);
		//drawLine(p1[0], p1[1], p2[0], p2[1]);
		//glLineWidth(2);
		drawRectangle(e1[0], e1[1], e1[2], e1[3], e1[6], e1[7], e1[4], e1[5], true);
		drawTriangle(e2[0], e2[1], e2[2], e2[3], e2[4], e2[5], true);
		if(PtDistPixel > 200)
			drawTriangle(e3[0], e3[1], e3[2], e3[3], e3[4], e3[5], true);
		//glColor3f(0.2, 1, 0.2);
		glLineWidth(2);
		drawCircle(e2[4], e2[5], 11 * wupp);
		drawCircle(e2[4], e2[5], 4 * wupp);
		glLineWidth(1);
		DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0063", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPartProgramStartPoint(double x1, double y1, double wupp)
{
	try
	{
		double p1[2] = {x1, y1}, e1[16] = {0};
		RMATH2DOBJECT->PartProgramStartingPoint(&p1[0],&e1[0], 8 * wupp, 6 * wupp);
		glLineWidth(2);
		glColor3f(0, 1, 0);
		drawRectangle(e1[0], e1[1], e1[2], e1[3], e1[4], e1[5], e1[6], e1[7]);
		drawLine(e1[8], e1[9], e1[10], e1[11]);
		drawLine(e1[12], e1[13], e1[14], e1[15]);
		glLineWidth(1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0064", __FILE__, __FUNCSIG__); }
}




void RGraphicsDraw::drawReferenceDatumName_Line(double slope, double *point, char* Strname, double w_upp)
{
	try
	{
		Strname[1] = '\0';
		RMATH2DOBJECT->Angle_FirstScndQuad(&slope);
		if(slope == 0)
			drawHorizontalDName(point, Strname, w_upp);
		else if(slope == M_PI_2)
			drawVerticalDName(point, Strname, w_upp);
		else
			drawDName_Slope(slope, point, Strname, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0076", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawReferenceDatumName_Circle(double *center, double radius, char* Strname, double w_upp)
{
	try
	{
		Strname[1] = '\0';
		double point1[2] = {center[0], center[1] - radius};
		drawHorizontalDName(point1, Strname, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0077", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawReferenceDatumName_Line3D(double *Line, double *MidPt, char* Strname, double w_upp)
{
	try
	{
		Strname[1] = '\0';
		double TransMatrix[9] = {1,0,0,0,1,0,0,0,1}, TmpPln[4] = {0,0,1,0}, TmpPt[3] = {MidPt[0] + 10 * Line[3], MidPt[1] + 10 * Line[4], MidPt[2] + 10 * Line[5]};
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(Line, TmpPt, TransMatrix, TmpPln);

		double endp[3] = {MidPt[0] - 8 * w_upp * Line[3], MidPt[1] - 8 * w_upp * Line[4], MidPt[2]  - 8 * w_upp * Line[5]};
		double endp1[3] = {MidPt[0] + 8 * w_upp * Line[3], MidPt[1] + 8 * w_upp * Line[4], MidPt[2]  + 8 * w_upp * Line[5]};
		double PerpendDir[3];
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&Line[3], &TransMatrix[6], PerpendDir);
		if(acos(PerpendDir[0]) > M_PI_2)
		{
			PerpendDir[0] = - PerpendDir[0];
			PerpendDir[1] = - PerpendDir[1];
			PerpendDir[2] = - PerpendDir[2];
		}
		double endp2[3] = {MidPt[0] + 10 * w_upp * PerpendDir[0], MidPt[1] + 10 * w_upp * PerpendDir[1], MidPt[2]  + 10 * w_upp * PerpendDir[2]};
		double endp3[3] = {MidPt[0] + 20 * w_upp * PerpendDir[0], MidPt[1] + 20 * w_upp * PerpendDir[1], MidPt[2]  + 20 * w_upp * PerpendDir[2]};
		drawDName3D(endp, endp1, endp2, endp3, Strname, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0078", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawReferenceDatumName_Plane(double *Plane, double *MidPt, char* Strname, double w_upp)
{
	try
	{
		Strname[1] = '\0';
		double TransMatrix[9] = {1,0,0,0,1,0,0,0,1};
		RMATH3DOBJECT->GetTMatrixForPlane(Plane, TransMatrix);
		double endp[3] = {MidPt[0] - 8 * w_upp * TransMatrix[0], MidPt[1] - 8 * w_upp * TransMatrix[1], MidPt[2]  - 8 * w_upp * TransMatrix[2]};
		double endp1[3] = {MidPt[0] + 8 * w_upp * TransMatrix[0], MidPt[1] + 8 * w_upp * TransMatrix[1], MidPt[2]  + 8 * w_upp * TransMatrix[2]};
		double endp2[3] = {MidPt[0] + 10 * w_upp * Plane[0], MidPt[1] + 10 * w_upp * Plane[1], MidPt[2]  + 10 * w_upp * Plane[2]};
		double endp3[3] = {MidPt[0] + 20 * w_upp * Plane[0], MidPt[1] + 20 * w_upp * Plane[1], MidPt[2]  + 20 * w_upp * Plane[2]};
		drawDName3D(endp, endp1, endp2, endp3, Strname, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0079", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawReferenceDatumName_Sphere(double *Sphere, char* Strname, double w_upp)
{
	try
	{
		Strname[1] = '\0';
		double endp[3] = {Sphere[0] - 8 * w_upp, Sphere[1] - Sphere[3], Sphere[2]};
		double endp1[3] = {Sphere[0] + 8 * w_upp, Sphere[1] - Sphere[3], Sphere[2]};
		double endp2[3] = {Sphere[0], Sphere[1] - Sphere[3] - 10 * w_upp, Sphere[2]};
		double endp3[3] = {Sphere[0], Sphere[1] - Sphere[3] - 20 * w_upp, Sphere[2]};
		drawDName3D(endp, endp1, endp2, endp3, Strname, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0080", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawReferenceDatumName_Cylinder(double *Cylinder, double *CylinderEndPts, char* Strname, double w_upp)
{
	try
	{
		Strname[1] = '\0';
		double TransMatrix[9] = {1,0,0,0,1,0,0,0,1}, TmpPln[4] = {0,0,1,0}, PerpendDir[3] = {0}, CySurfaceLn[6] = {0}, CySurfacePts[6] = {0};
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(&CylinderEndPts[0], &CylinderEndPts[3], TransMatrix, TmpPln);
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&Cylinder[3], &TransMatrix[6], PerpendDir);
		RMATH3DOBJECT->GetCylndrSurfaceLn(Cylinder, CylinderEndPts, PerpendDir, true, CySurfaceLn, CySurfacePts);
		double MidPt[3] = {(CySurfacePts[0] + CySurfacePts[3]) / 2, (CySurfacePts[1] + CySurfacePts[4]) / 2, (CySurfacePts[2] + CySurfacePts[5]) / 2};

		double endp[3] = {MidPt[0] - 8 * w_upp * CySurfaceLn[3], MidPt[1] - 8 * w_upp * CySurfaceLn[4], MidPt[2]  - 8 * w_upp * CySurfaceLn[5]};
		double endp1[3] = {MidPt[0] + 8 * w_upp * CySurfaceLn[3], MidPt[1] + 8 * w_upp * CySurfaceLn[4], MidPt[2]  + 8 * w_upp * CySurfaceLn[5]};
		double endp2[3] = {MidPt[0] - 10 * w_upp * PerpendDir[3], MidPt[1] - 10 * w_upp * PerpendDir[4], MidPt[2]  - 10 * w_upp * PerpendDir[5]};
		double endp3[3] = {MidPt[0] - 20 * w_upp * PerpendDir[3], MidPt[1] - 20 * w_upp * PerpendDir[4], MidPt[2]  - 20 * w_upp * PerpendDir[5]};
		drawDName3D(endp, endp1, endp2, endp3, Strname, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0081", __FILE__, __FUNCSIG__); }
}


void RGraphicsDraw::drawReferenceDatumName_Cone(double *Cone, double *ConeEndPts, char* Strname, double w_upp)
{
	try
	{
		Strname[1] = '\0';
		double TransMatrix[9] = {1,0,0,0,1,0,0,0,1}, TmpPln[4] = {0,0,1,0}, PerpendDir[3] = {0}, CnSurfaceLn[6] = {0}, CnSurfacePts[6] = {0};
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(&ConeEndPts[0], &ConeEndPts[3], TransMatrix, TmpPln);
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&Cone[3], &TransMatrix[6], PerpendDir);
		RMATH3DOBJECT->GetConeSurfaceLn(Cone, ConeEndPts, PerpendDir, true, CnSurfaceLn, CnSurfacePts);
		double MidPt[3] = {(CnSurfacePts[0] + CnSurfacePts[3]) / 2, (CnSurfacePts[1] + CnSurfacePts[4]) / 2, (CnSurfacePts[2] + CnSurfacePts[5]) / 2};

		double endp[3] = {MidPt[0] - 8 * w_upp * CnSurfaceLn[3], MidPt[1] - 8 * w_upp * CnSurfaceLn[4], MidPt[2]  - 8 * w_upp * CnSurfaceLn[5]};
		double endp1[3] = {MidPt[0] + 8 * w_upp * CnSurfaceLn[3], MidPt[1] + 8 * w_upp * CnSurfaceLn[4], MidPt[2]  + 8 * w_upp * CnSurfaceLn[5]};
		double endp2[3] = {MidPt[0] - 10 * w_upp * PerpendDir[3], MidPt[1] - 10 * w_upp * PerpendDir[4], MidPt[2]  - 10 * w_upp * PerpendDir[5]};
		double endp3[3] = {MidPt[0] - 20 * w_upp * PerpendDir[3], MidPt[1] - 20 * w_upp * PerpendDir[4], MidPt[2]  - 20 * w_upp * PerpendDir[5]};
		drawDName3D(endp, endp1, endp2, endp3, Strname, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0128", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawHorizontalDName(double *point, char* Strname, double w_upp)
{
	try
	{
		double endp[2] = {point[0] - 8 * w_upp, point[1]};
		double endp1[2] = {point[0] + 8 * w_upp, point[1]};
		double endp2[2] = {point[0], point[1] - 10 * w_upp};
		double endp3[2] = {point[0], point[1] - 20 * w_upp};
		double endp4[2] = {endp3[0] - 10 * w_upp, endp3[1]}; 
		double endp5[2] = {endp3[0] + 10 * w_upp, endp3[1] - 20 * w_upp};
		double endp6[2] = {endp3[0] - 6.5 * w_upp, endp3[1] - 16.5 * w_upp};
		double scalefact = 20 * w_upp;
		glLineWidth(2);
		glColor3d(1, 1, 0);
		GLdouble mix[] = {endp[0], endp[1], endp1[0], endp1[1], endp2[0], endp2[1]};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 0};
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		drawRectangle(endp4[0], endp4[1], endp5[0], endp5[1]);
		drawLine(endp2[0], endp2[1], endp3[0], endp3[1]);
		drawStringFixed(endp6[0], endp6[1], 0, scalefact,  Strname);
		glLineWidth(1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0082", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawVerticalDName(double *point, char* Strname, double w_upp)
{
	try
	{
		double endp[2] = {point[0], point[1] - 8 * w_upp};
		double endp1[2] = {point[0], point[1] + 8 * w_upp};
		double endp2[2] = {point[0] + 10 * w_upp, point[1]};
		double endp3[2] = {point[0] + 20 * w_upp, point[1]};
		double endp4[2] = {endp3[0], endp3[1] + 10 * w_upp}; 
		double endp5[2] = {endp3[0] + 20 * w_upp, endp3[1] - 10 * w_upp};
		double endp6[2] = {endp3[0] + 3.5 * w_upp, endp3[1] - 6.5 * w_upp};
		double scalefact = 20 * w_upp;
		glLineWidth(2);
		glColor3d(1, 1, 0);
		GLdouble mix[] = {endp[0], endp[1], endp1[0], endp1[1], endp2[0], endp2[1]};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 0};
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		drawRectangle(endp4[0], endp4[1], endp5[0], endp5[1]);
		drawLine(endp2[0], endp2[1], endp3[0], endp3[1]);
		drawStringFixed(endp6[0], endp6[1], 0, scalefact,  Strname);
		glLineWidth(1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0083", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawDName_Slope(double slope, double *point, char* Strname, double w_upp)
{
	try
	{
		double endp[2] = {point[0] - cos(slope)* 8 * w_upp, point[1] - sin(slope) * 8 * w_upp};
		double endp1[2] = {point[0] + cos(slope)* 8 * w_upp, point[1] + sin(slope) * 8 * w_upp};
		double endp2[2] = {point[0] - cos(slope + M_PI_2) * 10 * w_upp, point[1] - sin(slope + M_PI_2) * 10 * w_upp};
		double endp3[2] = {point[0] - cos(slope + M_PI_2) * 20 * w_upp, point[1] - sin(slope + M_PI_2) * 20 * w_upp};
		double endp4[2] = {endp3[0] + 10 * w_upp, endp3[1]};
		double endp5[2] = {endp4[0], endp4[1] + 10 * w_upp}; 
		double endp6[2] = {endp4[0] + 20 * w_upp, endp4[1] - 10 * w_upp};
		double endp7[2] = {endp4[0] + 3.5 * w_upp, endp4[1] - 6.5 * w_upp};
		double scalefact = 20 * w_upp;
		glLineWidth(2);
		glColor3d(1, 1, 0);
		GLdouble mix[] = {endp[0], endp[1], endp1[0], endp1[1], endp2[0], endp2[1]};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 0};
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		drawRectangle(endp5[0], endp5[1], endp6[0], endp6[1]);
		drawLine(endp2[0], endp2[1], endp3[0], endp3[1]);
		drawLine(endp3[0], endp3[1], endp4[0], endp4[1]);
		drawStringFixed(endp7[0], endp7[1], 0, scalefact,  Strname);
		glLineWidth(1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0084", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawDName3D(double *endp, double *endp1, double *endp2, double *endp3, char* Strname, double w_upp)
{
	try
	{
		double endp4[3] = {endp3[0] + 10 * w_upp, endp3[1], endp3[2]};
		double endp5[3] = {endp4[0], endp4[1] + 10 * w_upp, endp3[2]}; 
		double endp6[3] = {endp4[0] + 20 * w_upp, endp4[1] - 10 * w_upp, endp3[2]};
		double endp7[3] = {endp4[0] + 3.5 * w_upp, endp4[1] - 6.5 * w_upp, endp3[2]};
		double scalefact = 20 * w_upp;
		glLineWidth(2);
		glColor3d(1, 1, 0);
		GLdouble mix[] = {endp[0], endp[1], endp[2], endp1[0], endp1[1], endp1[2], endp2[0], endp2[1], endp2[2]};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 0};
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		drawRectangle3D(endp5[0], endp5[1], endp5[2], endp6[0], endp5[1], endp6[2], endp6[0], endp6[1], endp6[2], endp5[0], endp6[1], endp6[2]);
		drawLine_3D(endp2[0], endp2[1], endp2[2], endp3[0], endp3[1], endp3[2]);
		drawLine_3D(endp3[0], endp3[1], endp3[2], endp4[0], endp4[1], endp4[2]);
		drawStringFixed(endp7[0], endp7[1], endp7[2], scalefact,  Strname);
		glLineWidth(1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0085", __FILE__, __FUNCSIG__); }
}




void RGraphicsDraw::drawAngularity(double x, double y, double x1, double y1, char* distance, char* s1n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel)
{
	try
	{
		s1n[1] = '\0';
		std::string temp, temp1, temp2;
		temp = (const char*)distance;
		temp1 = (const char*)s1n;
		if(type == 0)
			temp2 = "    " + temp + "  " + temp1;
		else if(type == 1)
			temp2 = "// " + temp + "  " + temp1;
		else
			temp2 = "    " + temp + "  " + temp1;
		distance = (char*)temp2.c_str();
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[9], s2, m[2] = {x, y}, p1[2] = {x1, y1};
		RMATH2DOBJECT->Circle_diameter(&p1[0], 0, &m[0], &DisString[0], &e1[0], &s2);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(e1[0], e1[1], Zlevel, e1[2], e1[3], Zlevel);
		drawLine_3D(e1[2], e1[3], Zlevel, e1[4], e1[5], Zlevel);
		double pos[3] = {e1[6], e1[7], Zlevel};
		Calculate_MeasureRectangle(pos, &DisString[0], Lefttop, RightBtm);
		if(windowno == 0)
			drawString_BackGroud(e1[6], e1[7], Zlevel, DisString[0], DisString[1], 0, 0, 0);
		e1[8] = Zlevel;
		drawAngularity_MeasureString(&e1[6], (char*)temp2.c_str(), MeasureName, DisString[0], DisString[1], type, Fscalefactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0086", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::Calculate_MeasureRectangle(double *m1, double *Dis, double* Lefttop, double* RightBtm)
{
	try
	{
		double Xshift = Dis[0]/2 + Dis[0]/20, Yshift = Dis[1] + Dis[1]/2;
		Dis[0] = 2 * Xshift; Dis[1] = 2 * Yshift;
		Lefttop[0] = m1[0] - Xshift;
		Lefttop[1] = m1[1] + Yshift;
		Lefttop[3] = m1[0];
		Lefttop[4] = m1[1];
		Lefttop[5] = m1[2];
		RightBtm[0] = m1[0] + Xshift;
		RightBtm[1] = m1[1] - Yshift;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0087", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawAngularity_MeasureString(double *m1, char* Distance, char* MeasureName, double StrWidth, double StrHeight, int type, double Fscalefactor)
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		double glx = StrWidth, gly = StrHeight; 
		double MMatrix[16], TMMatrix[16];
		glPushMatrix();
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(m1[0], m1[1], m1[2]);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, 0, 0);
		drawRectangle(0, -gly/2, glx, gly/2);
		drawLine(glx/6, 0, glx/6, gly/2);
		drawLine(4.8 * glx/6, 0, 4.8 * glx/6, gly/2);
		drawLine(0, 0, glx, 0);
		glLineWidth(2);
		if(type == 0)
		{
			drawLine(glx/64, gly/8, 8 * glx/64, gly/8 + 7 * glx/64);
			drawLine(glx/64, gly/8, 8 * glx/64, gly/8);
		}
		else if(type == 2)
		{
			drawLine(2 * glx/64, gly/8, 9 * glx/64, gly/8);
			drawLine(5.5 * glx/64, gly/8, 5.5 * glx/64, 7 * gly/16);
		}
		else if (type == 3)
		{
			drawLine(2 * glx / 64, gly / 8, 9 * glx / 64, gly / 8 + 7 * glx / 64);
			double endptt[2] = { 9 * glx / 64, gly / 8 + 7 * glx / 64 };
			glColor3d(Measure_Color.r, Measure_Color.g, Measure_Color.b);
			double p1[2], p2[2], sl1 = M_PI_4 + M_PI / 6, sl2 = M_PI_4 - M_PI / 6;
			RMATH2DOBJECT->arrow2(endptt, sl1, sl2, &p1[0], &p2[0], 6 * glx / 64);
			drawTriangle(endptt[0], endptt[1], p1[0], p1[1], p2[0], p2[1], true);
		}
		glLineWidth(1);
		glTranslated(glx/24, gly/12, 0);
		glScaled(Fscalefactor, Fscalefactor, 0);
		glCallLists((GLsizei)strlen(Distance), GL_UNSIGNED_BYTE, Distance);
		glPopMatrix();

		drawString_AngularityName(m1[0], m1[1], m1[2], 0, MeasureName, StrWidth, StrHeight, Fscalefactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0088", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawString_AngularityName(double x, double y, double z, double angle, char *string, double StrWidth, double StrHeight, double Fscalefactor)
{
	try
	{
		double glx = getTextWidth(string) * Fscalefactor, gly = getTexHeight(string) * Fscalefactor; 
		glPushMatrix();
		double MMatrix[16], TMMatrix[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, -gly - gly/16, 0);
		glScaled(Fscalefactor, Fscalefactor, 0);	
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0092", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawConcentricity(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel)
{
	try
	{
		s1n[1] = '\0';
		std::string temp, temp1, temp2;
		temp = (const char*)distance;
		temp1 = (const char*)s1n;
		temp2 = "    " + temp + " " + temp1;
		distance = (char*)temp2.c_str();
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[8], s2, m[2] = {x, y}, p1[2] = {x1, y1};
		RMATH2DOBJECT->Circle_diameter(&p1[0], d, &m[0], &DisString[0], &e1[0], &s2);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(e1[0], e1[1], ZLevel, e1[2], e1[3], ZLevel);
		drawLine_3D(e1[2], e1[3], ZLevel, e1[4], e1[5], ZLevel);
		drawarrow(&e1[0], s2, w_upp, ZLevel);
		double pos[3] = {e1[6], e1[7], ZLevel};
		Calculate_MeasureRectangle(pos, &DisString[0], Lefttop, RightBtm);
		if(windowno == 0)
			drawString_BackGroud(e1[6], e1[7], ZLevel, DisString[0], DisString[1], 0, 0, 0);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawString_Concentricity(e1[6], e1[7], ZLevel, 0, (char*)temp2.c_str(), DisString[0], DisString[1], Fscalefactor);
		drawString_AngularityName(e1[6], e1[7] - DisString[1]/32, ZLevel, 0, MeasureName, DisString[0], DisString[1], Fscalefactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0093", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawString_Concentricity(double x, double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor)
{
	try
	{
		double glx = StrWidth, gly = StrHeight; 
		double MMatrix[16], TMMatrix[16];
		glPushMatrix();
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, 0, 0);
		drawRectangle(0, -gly/2, glx, gly/2);
		drawLine(glx/6, 0, glx/6, gly/2);
		drawLine(4.8 * glx/6, 0, 4.8 * glx/6, gly/2);
		drawLine(0, 0, glx, 0);
		glLineWidth(2);
		//drawPoint(glx/12, gly/4, 0, 3.0);
		drawCircle(glx/12, gly/4, gly/6, false);
		drawCircle(glx/12, gly/4, gly/12, false);
		glLineWidth(1);
		glTranslated(glx/24, gly/12, 0);
		glScaled(Fscalefactor, Fscalefactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0094", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawProfile(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* MeasureName, 
									int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel)
{
	try
	{
		s1n[1] = '\0';
		std::string temp, temp1, temp2;
		temp = (const char*)distance;
		temp1 = (const char*)s1n;
		temp2 = "    " + temp + " " + temp1;
		distance = (char*)temp2.c_str();
		double DisString[2] = { getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor };
		double e1[8], s2, m[2] = { x, y }, p1[2] = { x1, y1 };
		RMATH2DOBJECT->Circle_diameter(&p1[0], d, &m[0], &DisString[0], &e1[0], &s2);
		glColor3d(Measure_Color.r, Measure_Color.g, Measure_Color.b);
		drawLine_3D(e1[0], e1[1], ZLevel, e1[2], e1[3], ZLevel);
		drawLine_3D(e1[2], e1[3], ZLevel, e1[4], e1[5], ZLevel);
		drawarrow(&e1[0], s2, w_upp, ZLevel);
		double pos[3] = { e1[6], e1[7], ZLevel };
		Calculate_MeasureRectangle(pos, &DisString[0], Lefttop, RightBtm);
		if (windowno == 0)
			drawString_BackGroud(e1[6], e1[7], ZLevel, DisString[0], DisString[1], 0, 0, 0);
		glColor3d(Measure_Color.r, Measure_Color.g, Measure_Color.b);
		drawString_Profile(e1[6], e1[7], ZLevel, 0, (char*)temp2.c_str(), DisString[0], DisString[1], Fscalefactor, w_upp);
		drawString_AngularityName(e1[6], e1[7] - DisString[1] / 32, ZLevel, 0, MeasureName, DisString[0], DisString[1], Fscalefactor);
	}
	catch (...){ SetAndRaiseErrorMessage("RGRAPHICS0093", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawString_Profile(double x, double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor, double wr_upp)
{
	try
	{
		double glx = StrWidth, gly = StrHeight;
		double MMatrix[16], TMMatrix[16];
		glPushMatrix();
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, 0, 0);
		drawRectangle(0, -gly / 2, glx, gly / 2);
		drawLine(glx / 6, 0, glx / 6, gly / 2);
		drawLine(4.8 * glx / 6, 0, 4.8 * glx / 6, gly / 2);
		drawLine(0, 0, glx, 0);
		glLineWidth(2);
		//drawPoint(glx/12, gly/4, 0, 3.0);
		//drawCircle(glx / 12, gly / 4, gly / 6, false);
		//drawCircle(glx / 12, gly / 4, gly / 12, false);
		drawArc(glx / 12, gly / 4, gly / 6, 0, M_PI, wr_upp);
		drawLine(glx / 12 - gly / 6, gly / 4, glx / 12 + gly / 6, gly / 4);
		glLineWidth(1);
		glTranslated(glx / 24, gly / 12, 0);
		glScaled(Fscalefactor, Fscalefactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch (...){ SetAndRaiseErrorMessage("RGRAPHICS0094", __FILE__, __FUNCSIG__); }
}


void RGraphicsDraw::drawTruePosition(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* s2n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel)
{
	try
	{
		s1n[1] = '\0'; 
		std::string temp, temp1, temp2, temp3;
		temp = (const char*)distance;
		temp1 = (const char*)s1n;
		if(type == 0)
		{
			s2n[1] = '\0';
			temp2 = (const char*)s2n;
		}
		if(type == 0)
			temp3 = "     " + temp + " " + temp1 + "  " + temp2;
		else
			temp3 = "    " + temp + " " + temp1;
		distance = (char*)temp3.c_str();
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[8] = {0}, s2 = 0, m[2] = {x, y}, p1[2] = {x1, y1};
		RMATH2DOBJECT->Circle_diameter(&p1[0], d, &m[0], &DisString[0], &e1[0], &s2);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(e1[0], e1[1], ZLevel, e1[2], e1[3], ZLevel);
		drawLine_3D(e1[2], e1[3], ZLevel, e1[4], e1[5], ZLevel);
		drawarrow(&e1[0], s2, w_upp, ZLevel);
		double pos[3] = {e1[6], e1[7], ZLevel};
		Calculate_MeasureRectangle(pos, &DisString[0], Lefttop, RightBtm);
		if(windowno == 0)
			drawString_BackGroud(e1[6], e1[7], ZLevel, DisString[0], DisString[1], 0, 0, 0);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		if(type == 0)
			drawString_TruePositionXY(e1[6], e1[7], ZLevel, 0, (char*)temp3.c_str(), DisString[0], DisString[1], Fscalefactor);
		else
			drawString_TruePositionOneAxis(e1[6], e1[7], ZLevel, 0, (char*)temp3.c_str(), DisString[0], DisString[1], Fscalefactor);
		drawString_AngularityName(e1[6], e1[7] - DisString[1]/16, ZLevel, 0, MeasureName, DisString[0], DisString[1], Fscalefactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0095", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw3DTruePosition(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* s2n, char* s3n, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel)
{
	try
	{
		std::string temp, temp1, temp2, temp3, temp4;
		s1n[1] = '\0'; 
		temp = (const char*)distance;
		temp1 = (const char*)s1n;
		s2n[1] = '\0';
		temp2 = (const char*)s2n;
		s3n[1] = '\0';
		temp3 = (const char*)s3n;
		
		temp4 = "     " + temp + " " + temp1 + "  " + temp2 + "  " + temp3;
		
		distance = (char*)temp4.c_str();
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[8] = {0}, s2 = 0, m[2] = {x, y}, p1[2] = {x1, y1};
		
		RMATH2DOBJECT->Circle_diameter(&p1[0], d, &m[0], &DisString[0], &e1[0], &s2);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(e1[0], e1[1], ZLevel, e1[2], e1[3], ZLevel);
		drawLine_3D(e1[2], e1[3], ZLevel, e1[4], e1[5], ZLevel);
		double ArrPt[3] = {e1[0], e1[1], ZLevel}, ArrTail[3] = {e1[2], e1[3], ZLevel};
		DrawArrowCone(&ArrPt[0], ArrTail, w_upp);
		//drawarrow(&e1[0], s2, w_upp, ZLevel);
		double pos[3] = {e1[6], e1[7], ZLevel};
		Calculate_MeasureRectangle(pos, &DisString[0], Lefttop, RightBtm);
		if(windowno == 0)
			drawString_BackGroud(e1[6], e1[7], ZLevel, DisString[0], DisString[1], 0, 0, 0);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawString_TruePositionXYZ(e1[6], e1[7], ZLevel, 0, (char*)temp4.c_str(), DisString[0], DisString[1], Fscalefactor);
		drawString_AngularityName(e1[6], e1[7] - DisString[1]/16, ZLevel, 0, MeasureName, DisString[0], DisString[1], Fscalefactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0095", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawString_TruePositionXY(double x,double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor)
{
	try
	{
		double glx = StrWidth, gly = StrHeight; 
		double MMatrix[16], TMMatrix[16];
		glPushMatrix();
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, 0, 0);
		drawRectangle(0, -gly/2, glx, gly/2);
		drawLine(glx/6, 0, glx/6, gly/2);
		drawLine(4 * glx/6, 0, 4 * glx/6, gly/2);
		drawLine(5 * glx/6, 0, 5 * glx/6, gly/2);
		drawLine(0, 0, glx, 0);
		glLineWidth(2);
		drawPoint(glx/12, gly/4, 0, 3.0);
		drawCircle(glx/12, gly/4, gly/9);
		drawLine(glx/64, gly/4, 9.5 * glx/64, gly/4);
		drawLine(glx/12, gly/16, glx/12, 7 * gly/16);
		glLineWidth(1);
		glTranslated(glx/24, gly/12, 0);
		glScaled(Fscalefactor, Fscalefactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0096", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawString_TruePositionXYZ(double x,double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor)
{
	try
	{
		double glx = StrWidth, gly = StrHeight; 
		double MMatrix[16], TMMatrix[16];
		glPushMatrix();
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, 0, 0);
		drawRectangle(0, -gly/2, glx, gly/2);
		drawLine(glx/6, 0, glx/6, gly/2);
		drawLine(3.4 * glx/6, 0, 3.4 * glx/6, gly/2);
		drawLine(4.27 * glx/6, 0, 4.27 * glx/6, gly/2);
		drawLine(5.14 * glx/6, 0, 5.14 * glx/6, gly/2);
		drawLine(0, 0, glx, 0);
		glLineWidth(2);
		drawPoint(glx/12, gly/4, 0, 3.0);
		drawCircle(glx/12, gly/4, gly/9);
		drawLine(glx/64, gly/4, 9.5 * glx/64, gly/4);
		drawLine(glx/12, gly/16, glx/12, 7 * gly/16);
		glLineWidth(1);
		glTranslated(glx/24, gly/12, 0);
		glScaled(Fscalefactor, Fscalefactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0096", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawString_TruePositionOneAxis(double x,double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor)
{
	try
	{
		double glx = StrWidth, gly = StrHeight; 
		double MMatrix[16], TMMatrix[16];
		glPushMatrix();
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, 0, 0);
		drawRectangle(0, -gly/2, glx, gly/2);
		drawLine(glx/6, 0, glx/6, gly/2);
		drawLine(4.8 * glx/6, 0, 4.8 * glx/6, gly/2);
		drawLine(0, 0, glx, 0);
		glLineWidth(2);
		drawPoint(glx/12, gly/4, 0, 3.0);
		drawCircle(glx/12, gly/4, gly/9);
		drawLine(glx/64, gly/4, 9.5 * glx/64, gly/4);
		drawLine(glx/12, gly/16, glx/12, 7 * gly/16);
		glLineWidth(1);
		glTranslated(glx/24, gly/12, 0);
		glScaled(Fscalefactor, Fscalefactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0097", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_PointDistance3D(double x1,double y1,double z1,double x2,double y2,double z2,double disValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		//Breadth and height of the text
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		//p1: point1, p2: point2
		double p1[3] = {x1, y1, z1}, p2[3] = {x2, y2, z2};
		double InverseTransMatrix[9] = {1,0,0,0,1,0,0,0,1};
		//arrays to get end1, end2, middle, slope of arrow
		double TransformedPt1[3] = {0}, TransformedPt2[3] = {0}, TransformedMousePt[3] = {0}, MousePos[3] = {0}, e1[6] = {0},e2[6] = {0},m1[2] = {0},s1 = 0, e13D[9] = {0}, e23D[9] = {0};

		//Now the intersection of measurement plane and selection line is the mouse pt reqd
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);

		//Now lets transform the 2 points using the matrix

		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, p1, Order2, TransformedPt1);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, p2, Order2, TransformedPt2);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, MousePos, Order2, TransformedMousePt);
		//get all the parameters
		RMATH2DOBJECT->Pt2Ptdis(&TransformedPt1[0], &TransformedPt2[0], &TransformedMousePt[0], &DisString[0],disValue,&e1[0], &e2[0],&m1[0],&s1);

		double end1[9] = {e1[0], e1[1], TransformedPt1[2], e1[2], e1[3], TransformedPt1[2], e1[4], e1[5], TransformedPt1[2]},
			   end2[9] = {e2[0], e2[1], TransformedPt1[2], e2[2], e2[3], TransformedPt1[2], e2[4], e2[5], TransformedPt1[2]},
			   middle[3] = {m1[0], m1[1], TransformedPt1[2]}, TransMiddle[3];
		//get the inverse of the matrix for transforming the points back to original position.
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);
		
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[0], Order2, &e13D[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[3], Order2, &e13D[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[6], Order2, &e13D[6]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[0], Order2, &e23D[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[3], Order2, &e23D[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[6], Order2, &e23D[6]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &middle[0], Order2, &TransMiddle[0]);
		//draw the text string
		draw_MeasurementString3D(TransMiddle, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		//draw the lines
		drawMeasureLine3D(e13D, e23D);
		//draw the arrows
		CalculateAndDrawArrowCones(&e13D[3], &e23D[3], &e13D[6], &e23D[6], w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0103", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_PointDistance3DLinear(double x1,double y1,double z1,double x2,double y2,double z2,double disValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, int type)
{
	try
	{
		double p1[3] = {x1, y1, z1}, p2[3] = {x2, y2, z2}, xyPlane[4] = {0, 0, 1, z2}, projPnt[3] = {0};
		RMATH3DOBJECT->Projection_Point_on_Plane(p1, xyPlane, projPnt);
		if(type == 0 || type == 1)
		{
			double mousepos[3] = {0};
			RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, xyPlane, mousepos);
			drawLine_3DStipple(p1[0], p1[1], p1[2], projPnt[0], projPnt[1], projPnt[2]);
			double e1[6] = {0}, e2[6] = {0}, m1[3] = {0}, s2 = 0;
			double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
			if(type == 0)
			   RMATH2DOBJECT->X_shift(p2, projPnt, mousepos, DisString, disValue, e1, e2, m1, &s2);
			else if(type == 1)
			   RMATH2DOBJECT->Y_shift(p2, projPnt, mousepos, DisString, disValue, e1, e2, m1, &s2);
				
			e2[0] = projPnt[0]; e1[0] = p2[0];
			e2[1] = projPnt[1]; e1[1] = p2[1];
			m1[2] = z2;
			double tmp1[9] = {e1[0], e1[1], z2, e1[2], e1[3], z2, e1[4], e1[5], z2};
			double tmp2[9] = {e2[0], e2[1], z2, e2[2], e2[3], z2, e2[4], e2[5], z2};
			draw_MeasurementString2D(m1, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
			drawMeasureLine3D(tmp1, tmp2);
			CalculateAndDrawArrowCones(&tmp1[3], &tmp2[3], &tmp1[6], &tmp2[6], w_upp);
		}
		/*else if(type == 1)
		{
			double mousepos[3] = {0};
			RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, xyPlane, mousepos);
			drawLine_3DStipple(p1[0], p1[1], p1[2], projPnt[0], projPnt[1], projPnt[2]);
			drawPoint_PointDistance_Linear(projPnt[0], projPnt[1], p2[0], p2[1], false, mousepos[0], mousepos[1], disValue, distance, MeasureName, windowno, w_upp, Fscalefactor, Lefttop, RightBtm, z2);
		}*/
		else
		{
			RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(&p1[0], &projPnt[0], TransMatrix, MeasurementPlane);
			drawLine_3DStipple(p2[0], p2[1], p2[2], projPnt[0], projPnt[1], projPnt[2]);
			drawPoint_PointDistance3D(p1[0], p1[1], p1[2], projPnt[0], projPnt[1], projPnt[2], disValue, distance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
			
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0103", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_FinitelineDistance3D(double *Point, double *endpts, double *Line, double Dist, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		//Breadth and height of the text
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		//p1: point1, p2: point2
		double intercept1 = 0;
		double InverseTransMatrix[9] = {1,0,0,0,1,0,0,0,1};
		//arrays to get end1, end2, middle, slope of arrow
		double TransformedPt[3] = {0}, TransformedEndPt1[3] = {0}, TransformedEndPt2[3] = {0}, TransformedMousePt[3] = {0}, MousePos[3] = {0}, e1[6] = {0},e2[6] = {0},m1[2] = {0},s1 = 0, e13D[9] = {0}, e23D[9] = {0};

		//Now the intersection of measurement plane and selection line is the mouse pt reqd
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);

		//Now lets transform the point and the 2 end points of the line using the matrix

		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, Point, Order2, TransformedPt);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &endpts[0], Order2, TransformedEndPt1);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &endpts[3], Order2, TransformedEndPt2);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, MousePos, Order2, TransformedMousePt);
		//get all the parameters
		double slope, intercept;
		if(TransformedEndPt2[0] - TransformedEndPt1[0] !=0)
		{
			slope = (TransformedEndPt2[1] - TransformedEndPt1[1])/(TransformedEndPt2[0] - TransformedEndPt1[0]);
			intercept = TransformedEndPt1[1] - slope * TransformedEndPt1[0];
			slope = atan(slope);
		}
		else
		{
			slope = M_PI_2;
			intercept = TransformedEndPt1[0];
		}
			
		//RMATH2DOBJECT->Pt2Ptdis(&TransformedPt1[0], &TransformedPt2[0], &TransformedMousePt[0], &DisString[0],disValue,&e1[0], &e2[0],&m1[0],&s1);
		RMATH2DOBJECT->Point_FinitelineDistance(&TransformedPt[0], slope, intercept,TransformedEndPt1,TransformedEndPt2,&intercept1, Dist, &TransformedMousePt[0],&DisString[0],&e1[0], &e2[0],&m1[0],&s1);
		
		double end1[9] = {e1[0], e1[1], TransformedPt[2], e1[2], e1[3], TransformedPt[2], e1[4], e1[5], TransformedPt[2]},
			   end2[9] = {e2[0], e2[1], TransformedPt[2], e2[2], e2[3], TransformedPt[2], e2[4], e2[5], TransformedPt[2]},
			   middle[3] = {m1[0], m1[1], TransformedPt[2]}, TransMiddle[3] = {0};
		//get the inverse of the matrix for transforming the points back to original position.
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);
		
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[0], Order2, &e13D[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[3], Order2, &e13D[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[6], Order2, &e13D[6]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[0], Order2, &e23D[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[3], Order2, &e23D[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[6], Order2, &e23D[6]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &middle[0], Order2, &TransMiddle[0]);
		//draw the text string
		draw_MeasurementString3D(TransMiddle, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		//draw the lines
		drawMeasureLine3D(e13D, e23D);
		
		CalculateAndDrawArrowCones(&e13D[3], &e23D[3], &e13D[0], &e23D[0], w_upp);
		//drawCommonProperties_AllMeasure3D(e1, e2);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0107", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_PlaneDistance3D(double *Point, double *Plane, double *PlnPts, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1}, PlnEdgePoints[6] = {PlnPts[0], PlnPts[1], PlnPts[2], PlnPts[9], PlnPts[10], PlnPts[11]};
		double IntersectnPts[6] = {0};
		
		RMATH3DOBJECT->Intersection_Plane_Plane(Plane, MeasurementPlane, IntersectnLn);

		if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnPts[0], &IntersectnPts[0]))
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnPts[6], &IntersectnPts[3]);
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnPts[3], &IntersectnPts[0]))
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		
		drawPoint_FinitelineDistance3D(Point, IntersectnPts, IntersectnLn, DisValue, distance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0111", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_CylinderDistance3D(double *Point, double *Cylinder, double *EndPts, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double CyLine[6] = {0,0,0,0,0,1}, CyLnPts[6] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Point_Cylinder(Point, Cylinder, EndPts, MeasureType, CyLine, CyLnPts);
		drawPoint_FinitelineDistance3D(Point, CyLnPts, CyLine, DisValue, distance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0112", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_SphereDistance3D(double *Point, double *TheSphere, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double SprPt[3] = {0}; // Variable..!
		RMATH3DOBJECT->MeasureModeCalc_Point_Sphere(Point, TheSphere, MeasureType, SprPt);
		drawPoint_PointDistance3D(Point[0], Point[1], Point[2], SprPt[0], SprPt[1], SprPt[2], DisValue, distance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0113", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_elipseDistance3D(double *Point, double *elipse, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1};
		double IntersectnPts[6] = {0};
		
		RMATH3DOBJECT->Intersection_Plane_Plane(elipse, MeasurementPlane, IntersectnLn);
		RMATH3DOBJECT->Intersection_Line_elipse(IntersectnLn, &elipse[0], &IntersectnPts[3]);
		
		drawPoint_FinitelineDistance3D(Point, IntersectnPts, IntersectnLn, DisValue, distance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0111", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoint_Circle3DDistance3D(double *Point, double *Circle3D, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, int type)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1};
		double IntersectnPts[6] = {0};
		double CircleAsSphere[4] = {Circle3D[0], Circle3D[1], Circle3D[2], Circle3D[6]};
		double CirclePlane[4] = {Circle3D[3], Circle3D[4], Circle3D[5], Circle3D[0] * Circle3D[3] + Circle3D[1] * Circle3D[4] + Circle3D[2] * Circle3D[5]};
		if(type = 0)
		{
			RMATH3DOBJECT->Intersection_Plane_Plane(CirclePlane, MeasurementPlane, IntersectnLn);
			RMATH3DOBJECT->Intersection_Line_Sphere(IntersectnLn, &CircleAsSphere[0], &IntersectnPts[0]);
			drawPoint_FinitelineDistance3D(Point, IntersectnPts, IntersectnLn, DisValue, distance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else if(type = 1)
		{
			double tmpPnt[6] = {0};
			RMATH3DOBJECT->Projection_Point_on_Plane(Point, CirclePlane, &tmpPnt[3]);
			for(int i = 0; i < 3; i++)
				tmpPnt[i] = Point[i];
			GRAFIX->drawStippleLines3D(tmpPnt, 1);
			drawPoint_PointDistance3D(Circle3D[0], Circle3D[1], Circle3D[2], tmpPnt[3], tmpPnt[4], tmpPnt[5], DisValue, distance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0111", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_FinitelineDistance3D(double *Line1, double *endPts1, double *Line2, double *endPts2, double distance, char* strdistance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double PtsOfClosestDist[6] = {0};
		bool FirstPtOnLine = false, SecondPtOnLine = false;
		//if the following statement is true, it means the lines are either skew or intersecting
		if(RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(Line1, Line2, PtsOfClosestDist))
		{
			//if the 1st point is within the span of the 1st finite line, set flag
			if(RMATH3DOBJECT->Checking_Point_within_Block(PtsOfClosestDist, endPts1, 2))
				FirstPtOnLine = true;
			
			//if the 2nd point is within the span of the 2nd finite line, then set flag
			if(RMATH3DOBJECT->Checking_Point_within_Block(&PtsOfClosestDist[3], endPts2, 2))
				SecondPtOnLine = true;
			
			//if both points are not within the span of the 2 respective lines, we need a different approach
			if(!FirstPtOnLine && !SecondPtOnLine)
			{
				double PlaneWith2ndLn[4] = {0}, ProjectedMidPt[3] = {0}, ProjectedPt1[3] = {0}, ProjectedPt2[3] = {0};
				double Ln1MidPt[3] = {(endPts1[0] + endPts1[3]) / 2, (endPts1[1] + endPts1[4]) / 2, (endPts1[2] + endPts1[5]) / 2};

				//Get the plane that contains line2 and parallel to line 1
				RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&Line1[3], &Line2[3], PlaneWith2ndLn);
				PlaneWith2ndLn[3] = PlaneWith2ndLn[0] * Line2[0] + PlaneWith2ndLn[1] * Line2[1] + PlaneWith2ndLn[2] * Line2[2];

				//Now project the mid pt of line 1 to this plane
				RMATH3DOBJECT->Projection_Point_on_Plane(Ln1MidPt, PlaneWith2ndLn, ProjectedMidPt);

				//Get the end pts of the projected line
				double OffsetLength = RMATH3DOBJECT->Distance_Point_Point(&endPts2[0], &endPts2[3]) / 2;
				for(int i = 0 ; i < 3; i++)
				{
					ProjectedPt1[i] = ProjectedMidPt[i] + OffsetLength * Line2[3 + i];
					ProjectedPt2[i] = ProjectedMidPt[i] - OffsetLength * Line2[3 + i];
				}

				//Now draw the dotted lines indicating projections of line 2
				drawLine_3DStipple(endPts2[0], endPts2[1], endPts2[2], ProjectedPt1[0], ProjectedPt1[1], ProjectedPt1[2]);
				drawLine_3DStipple(ProjectedPt1[0], ProjectedPt1[1], ProjectedPt1[2], ProjectedPt2[0], ProjectedPt2[1], ProjectedPt2[2]);
				drawLine_3DStipple(ProjectedPt2[0], ProjectedPt2[1], ProjectedPt2[2], endPts2[3], endPts2[4], endPts2[5]);

				//Now set the points of closest distance to mid pt of line 1 and mid pt of projected line
				PtsOfClosestDist[0] = Ln1MidPt[0];
				PtsOfClosestDist[1] = Ln1MidPt[1];
				PtsOfClosestDist[2] = Ln1MidPt[2];
				PtsOfClosestDist[3] = ProjectedMidPt[0];
				PtsOfClosestDist[4] = ProjectedMidPt[1];
				PtsOfClosestDist[5] = ProjectedMidPt[2];

				//Set the measurement plane and transformation matrix
				RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(&PtsOfClosestDist[0], &PtsOfClosestDist[3], TransMatrix, MeasurementPlane);
			}
			else
			{
				//if 1st pt outside 1st line, draw extension line
				if(!FirstPtOnLine)
				{
					if(abs(PtsOfClosestDist[0] - endPts1[0]) < abs(PtsOfClosestDist[0] - endPts1[3]) ||
						abs(PtsOfClosestDist[1] - endPts1[1]) < abs(PtsOfClosestDist[1] - endPts1[4]) ||
							abs(PtsOfClosestDist[2] - endPts1[2]) < abs(PtsOfClosestDist[2] - endPts1[5]))
					{
						double DrawingPts[6] = {PtsOfClosestDist[0], PtsOfClosestDist[1], PtsOfClosestDist[2], endPts1[0], endPts1[1], endPts1[2]};
						drawLine_3DStipple(DrawingPts[0], DrawingPts[1], DrawingPts[2], DrawingPts[3], DrawingPts[4], DrawingPts[5]);
					}
					else
					{
						double DrawingPts[6] = {PtsOfClosestDist[0], PtsOfClosestDist[1], PtsOfClosestDist[2], endPts1[3], endPts1[4], endPts1[5]};
						drawLine_3DStipple(DrawingPts[0], DrawingPts[1], DrawingPts[2], DrawingPts[3], DrawingPts[4], DrawingPts[5]);
					}
				}
				//if 2nd pt outside 2nd line, draw extension line
				if(!SecondPtOnLine)
				{
					if(abs(PtsOfClosestDist[3] - endPts2[0]) < abs(PtsOfClosestDist[3] - endPts2[3]) ||
						abs(PtsOfClosestDist[4] - endPts2[1]) < abs(PtsOfClosestDist[4] - endPts2[4]) ||
							abs(PtsOfClosestDist[5] - endPts2[2]) < abs(PtsOfClosestDist[5] - endPts2[5]))
					{
						double DrawingPts[6] = {PtsOfClosestDist[3], PtsOfClosestDist[4], PtsOfClosestDist[5], endPts2[0], endPts2[1], endPts2[2]};
						drawLine_3DStipple(DrawingPts[0], DrawingPts[1], DrawingPts[2], DrawingPts[3], DrawingPts[4], DrawingPts[5]);
					}
					else
					{
						double DrawingPts[6] = {PtsOfClosestDist[3], PtsOfClosestDist[4], PtsOfClosestDist[5], endPts2[3], endPts2[4], endPts2[5]};
						drawLine_3DStipple(DrawingPts[0], DrawingPts[1], DrawingPts[2], DrawingPts[3], DrawingPts[4], DrawingPts[5]);
					}
				}
			}
			drawPoint_PointDistance3D(PtsOfClosestDist[0], PtsOfClosestDist[1], PtsOfClosestDist[2], PtsOfClosestDist[3], PtsOfClosestDist[4], PtsOfClosestDist[5], distance, strdistance, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
		//else the lines are parallel
		{
			//double e1[6],e2[6],m1[2],s2;
			double DisString[2] = {getTextWidth(strdistance) * Fscalefactor, getTexHeight(strdistance) * Fscalefactor};
			//double	m[2] = {x, y};
			//SObjectGraphics->Finiteline_FinitelineDistance(endp11,endp12,endp21,endp22,slope1,intercept1,slope2,intercept2,distance,&m[0],&DisString[0],&e1[0],&e2[0],&m1[0],&s2);
			//drawCommonProperties_AllMeasure(e1, e2,DisString,m1, m, s2, intercept1, intercept2, distance, strdistance, MeasureName);


		
			double InverseTransMatrix[9] = {1,0,0,0,1,0,0,0,1};
			//arrays to get end1, end2, middle, slope of arrow
			double TransformedEndPt11[3] = {0}, TransformedEndPt12[3] = {0}, TransformedEndPt21[3] = {0}, TransformedEndPt22[3] = {0}, TransformedMousePt[3] = {0}, MousePos[3] = {0}, e1[6] = {0},e2[6] = {0},m1[2] = {0},s1 = 0, e13D[9] = {0}, e23D[9] = {0};

			//Get the 3D selection line from RCad window
			//RCAD->getWindow(1).CalculateSelectionLine(MouseSelectnLine);
			
			//Now the intersection of measurement plane and selection line is the mouse pt reqd
			RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);

			//Now lets transform the 4 endpoints of the 2 lines using the matrix

			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &endPts1[0], Order2, TransformedEndPt11);
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &endPts1[3], Order2, TransformedEndPt12);
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &endPts2[0], Order2, TransformedEndPt21);
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &endPts2[3], Order2, TransformedEndPt22);
			RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, MousePos, Order2, TransformedMousePt);
			//get all the parameters
			double slope1, intercept1, slope2, intercept2;
			if(TransformedEndPt12[0] - TransformedEndPt11[0] !=0)
			{
				slope1 = (TransformedEndPt12[1] - TransformedEndPt11[1])/(TransformedEndPt12[0] - TransformedEndPt11[0]);
				intercept1 = TransformedEndPt11[1] - slope1 * TransformedEndPt11[0];
			}
			else
			{
				slope1 = M_PI_2;
				intercept1 = TransformedEndPt11[0];
			}

			if(TransformedEndPt22[0] - TransformedEndPt21[0] !=0)
			{
				slope2 = (TransformedEndPt22[1] - TransformedEndPt21[1])/(TransformedEndPt22[0] - TransformedEndPt21[0]);
				intercept2 = TransformedEndPt21[1] - slope2 * TransformedEndPt21[0];
			}
			else
			{
				slope2 = M_PI_2;
				intercept2 = TransformedEndPt21[0];
			}
				
			//SObjectGraphics->Pt2Ptdis(&TransformedPt1[0], &TransformedPt2[0], &TransformedMousePt[0], &DisString[0],disValue,&e1[0], &e2[0],&m1[0],&s1);
			//SObjectGraphics->Point_FinitelineDistance(&TransformedPt[0], slope, intercept,TransformedEndPt1,TransformedEndPt2,&intercept1, Dist, &TransformedMousePt[0],&DisString[0],&e1[0], &e2[0],&m1[0],&s1);
			RMATH2DOBJECT->Finiteline_FinitelineDistance(TransformedEndPt11,TransformedEndPt12,TransformedEndPt21,TransformedEndPt22, slope1, intercept1, slope2, intercept2, distance,&TransformedMousePt[0],&DisString[0],&e1[0], &e2[0],&m1[0],&s1);
			double end1[9] = {e1[0], e1[1], TransformedEndPt11[2], e1[2], e1[3], TransformedEndPt11[2], e1[4], e1[5], TransformedEndPt11[2]},
				   end2[9] = {e2[0], e2[1], TransformedEndPt11[2], e2[2], e2[3], TransformedEndPt11[2], e2[4], e2[5], TransformedEndPt11[2]},
				   middle[3] = {m1[0], m1[1], TransformedEndPt11[2]}, TransMiddle[3] = {0};
			//get the inverse of the matrix for transforming the points back to original position.
			RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);
			
			RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[0], Order2, &e13D[0]);
			RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[3], Order2, &e13D[3]);
			RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[6], Order2, &e13D[6]);
			RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[0], Order2, &e23D[0]);
			RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[3], Order2, &e23D[3]);
			RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[6], Order2, &e23D[6]);
			RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &middle[0], Order2, &TransMiddle[0]);
			//draw the text string
			draw_MeasurementString3D(TransMiddle, DisString, strdistance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
			
			//draw the lines
			//RCADGRAFIX->drawLine_3D(e13D[0], e13D[1], e13D[2], e23D[0], e23D[1], e23D[2]);
			drawMeasureLine3D(e13D, e23D);
			
			CalculateAndDrawArrowCones(&e13D[3], &e23D[3], &e13D[0], &e23D[0], w_upp);
			//drawCommonProperties_AllMeasure3D(e1, e2);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0101", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_FinitelineAngle3D(double *Line1, double *LnPts1, double *Line2, double *LnPts2, double angle, char* angleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double DisString[2] = {getTextWidth(angleChar) * Fscalefactor , getTexHeight(angleChar) * Fscalefactor };
		double Dist = RMATH3DOBJECT->Distance_Point_Point(&LnPts2[0], &LnPts2[3]);
		double Ln1MidPt[3] = {(LnPts1[0] + LnPts1[3]) / 2, (LnPts1[1] + LnPts1[4]) / 2, (LnPts1[2] + LnPts1[5]) / 2};
		bool DoSecondLnProjection = false;
		Dist = Dist / 2;
		
		double InverseTransMatrix[9] = {1,0,0,0,1,0,0,0,1};
		double PrjEndPt2[6] = {0};

		//Project the line2 pts on the measurement plane
		//RMATH3DOBJECT->Projection_Point_on_Plane(&LnPts2[0], MeasurementPlane, &PrjEndPt2[0]);
		//RMATH3DOBJECT->Projection_Point_on_Plane(&LnPts2[3], MeasurementPlane, &PrjEndPt2[3]);
		if(RMATH3DOBJECT->Checking_Point_on_Plane(LnPts2, MeasurementPlane) && RMATH3DOBJECT->Checking_Point_on_Plane(&LnPts2[3], MeasurementPlane))
		{
			DoSecondLnProjection = false;
			for(int i = 0; i < 6; i++)
			{
				PrjEndPt2[i] = LnPts2[i];
			}
		}
		else
		{
			DoSecondLnProjection = true;
			for(int i = 0; i < 3; i++)
			{
				PrjEndPt2[i] = Ln1MidPt[i] + Line2[3 + i] * Dist;
				PrjEndPt2[3 + i] = Ln1MidPt[i] - Line2[3 + i] * Dist;
			}
		}

		//arrays to get end1, end2, middle, slope of arrow
		double TransformedEndPt11[3] = {0}, TransformedEndPt12[3] = {0}, TransformedEndPt21[3] = {0}, TransformedEndPt22[3] = {0}, TransformedMousePt[3] = {0}, MousePos[3] = {0};

		//Now the intersection of measurement plane and selection line is the mouse pt reqd
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);

		//Now lets transform the 4 endpoints of the 2 lines using the matrix

		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &LnPts1[0], Order2, TransformedEndPt11);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &LnPts1[3], Order2, TransformedEndPt12);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &PrjEndPt2[0], Order2, TransformedEndPt21);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &PrjEndPt2[3], Order2, TransformedEndPt22);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, MousePos, Order2, TransformedMousePt);
		//get all the parameters
		double slope1 = 0, intercept1 = 0, slope2 = 0, intercept2 = 0;
		if(abs(TransformedEndPt12[0] - TransformedEndPt11[0]) > 0.00001)
		{
			slope1 = (TransformedEndPt12[1] - TransformedEndPt11[1])/(TransformedEndPt12[0] - TransformedEndPt11[0]);
			intercept1 = TransformedEndPt11[1] - slope1 * TransformedEndPt11[0];
			slope1 = atan(slope1);
		}
		else
		{
			slope1 = M_PI_2;
			intercept1 = TransformedEndPt11[0];
		}

		if(abs(TransformedEndPt22[0] - TransformedEndPt21[0]) > 0.00001)
		{
			slope2 = (TransformedEndPt22[1] - TransformedEndPt21[1])/(TransformedEndPt22[0] - TransformedEndPt21[0]);
			intercept2 = TransformedEndPt21[1] - slope2 * TransformedEndPt21[0];
			slope2 = atan(slope2);
		}
		else
		{
			slope2 = M_PI_2;
			intercept2 = TransformedEndPt21[0];
		}

		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Finiteline_FinitelineAngle(slope1,intercept1,TransformedEndPt11,TransformedEndPt12,slope2,intercept2,TransformedEndPt21,TransformedEndPt22, TransformedMousePt, &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
			
		double end1[6] = {pt[2], pt[3], TransformedEndPt11[2], pt[4], pt[5], TransformedEndPt11[2]},
			   end2[6] = {pt[6], pt[7], TransformedEndPt11[2], pt[8], pt[9], TransformedEndPt11[2]},
			   midP[3] = {pt[0], pt[1], TransformedEndPt11[2]},
			   LineEnd1[6] = {0}, LineEnd2[6] = {0}, MidPt[3] = {0};

		//get the inverse of the matrix for transforming the points back to original position.
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);
		
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[0], Order2, &LineEnd1[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[3], Order2, &LineEnd1[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[0], Order2, &LineEnd2[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[3], Order2, &LineEnd2[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &midP[0], Order2, &MidPt[0]);
		
		//draw the text string
		draw_MeasurementString3D(MidPt, DisString, angleChar, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		
		//Draw the 2 straight lines
		drawLine_3DStipple(LineEnd1[0], LineEnd1[1], LineEnd1[2], LineEnd1[3], LineEnd1[4], LineEnd1[5]);
		drawLine_3DStipple(LineEnd2[0], LineEnd2[1], LineEnd2[2], LineEnd2[3], LineEnd2[4], LineEnd2[5]);
		
		double PrjctnLns[12] = {LnPts2[0], LnPts2[1], LnPts2[2],
								PrjEndPt2[0], PrjEndPt2[1], PrjEndPt2[2],
								PrjEndPt2[3], PrjEndPt2[4], PrjEndPt2[5],
								LnPts2[3], LnPts2[4], LnPts2[5]};
	
		if(DoSecondLnProjection)
		{
			drawLine_3DStipple(PrjctnLns[0], PrjctnLns[1], PrjctnLns[2], PrjctnLns[3], PrjctnLns[4], PrjctnLns[5]);
			drawLine_3DStipple(PrjctnLns[3], PrjctnLns[4], PrjctnLns[5], PrjctnLns[6], PrjctnLns[7], PrjctnLns[8]);
			drawLine_3DStipple(PrjctnLns[6], PrjctnLns[7], PrjctnLns[8], PrjctnLns[9], PrjctnLns[10], PrjctnLns[11]);
		}
	
		drawCommonProperties_Angle3D(pt, center, startang, sweepang, radius, slop, angle, angleChar, MeasureName, TransformedEndPt11[2], InverseTransMatrix);
		
		double PrjLn[6] = {PrjEndPt2[0], PrjEndPt2[1], PrjEndPt2[2], Line2[3], Line2[4], Line2[5]};
		//CalculateAndDrawArrowCones(&e13D[3], &e23D[3]);
		//if(slop[0] >= M_PI) slop[0] -= M_PI;
		//if(slop[1] >= M_PI) slop[1] -= M_PI;
		CalculateAndDrawArrowConesAng(LineEnd1, LineEnd2, slop, TransMatrix, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0101", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_FinitelineAngle3D1(double *Line1, double *LnPts1, double *Line2, double *LnPts2, double angle, char* angleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double DisString[2] = {getTextWidth(angleChar) * Fscalefactor, getTexHeight(angleChar) * Fscalefactor};
			
		double InverseTransMatrix[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
		double PrjEndPt2[6] = {0};

		//Project the line2 pts on the measurement plane
		RMATH3DOBJECT->Projection_Point_on_Plane(&LnPts2[0], MeasurementPlane, &PrjEndPt2[0]);
		RMATH3DOBJECT->Projection_Point_on_Plane(&LnPts2[3], MeasurementPlane, &PrjEndPt2[3]);
		
		//arrays to get end1, end2, middle, slope of arrow
		double TransformedEndPt11[3] = {0}, TransformedEndPt12[3] = {0}, TransformedEndPt21[3] = {0}, TransformedEndPt22[3] = {0}, TransformedMousePt[3] = {0}, MousePos[3] = {0};

		//Now the intersection of measurement plane and selection line is the mouse pt reqd
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);

		//Now lets transform the 4 endpoints of the 2 lines using the matrix

		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &LnPts1[0], Order2, TransformedEndPt11);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &LnPts1[3], Order2, TransformedEndPt12);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &PrjEndPt2[0], Order2, TransformedEndPt21);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &PrjEndPt2[3], Order2, TransformedEndPt22);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, MousePos, Order2, TransformedMousePt);
		//get all the parameters
		double slope1 = 0, intercept1 = 0, slope2 = 0, intercept2 = 0;
		if(abs(TransformedEndPt12[0] - TransformedEndPt11[0]) > 0.00001)
		{
			slope1 = (TransformedEndPt12[1] - TransformedEndPt11[1])/(TransformedEndPt12[0] - TransformedEndPt11[0]);
			intercept1 = TransformedEndPt11[1] - slope1 * TransformedEndPt11[0];
			slope1 = atan(slope1);
		}
		else
		{
			slope1 = M_PI_2;
			intercept1 = TransformedEndPt11[0];
		}

		if(abs(TransformedEndPt22[0] - TransformedEndPt21[0]) > 0.00001)
		{
			slope2 = (TransformedEndPt22[1] - TransformedEndPt21[1])/(TransformedEndPt22[0] - TransformedEndPt21[0]);
			intercept2 = TransformedEndPt21[1] - slope2 * TransformedEndPt21[0];
			slope2 = atan(slope2);
		}
		else
		{
			slope2 = M_PI_2;
			intercept2 = TransformedEndPt21[0];
		}

		double radius = 0, center[2] = {0}, startang[2] = {0}, sweepang[2] = {0}, pt[10] = {0}, slop[4] = {0};
		RMATH2DOBJECT->Finiteline_FinitelineAngle(slope1,intercept1,TransformedEndPt11,TransformedEndPt12,slope2,intercept2,TransformedEndPt21,TransformedEndPt22, TransformedMousePt, &DisString[0], &radius, & center[0], &startang[0], &sweepang[0], &pt[0], &slop[0]);
			
		double end1[6] = {pt[2], pt[3], TransformedEndPt11[2], pt[4], pt[5], TransformedEndPt11[2]},
			   end2[6] = {pt[6], pt[7], TransformedEndPt11[2], pt[8], pt[9], TransformedEndPt11[2]},
			   midP[3] = {pt[0], pt[1], TransformedEndPt11[2]},
			   LineEnd1[6] = {0}, LineEnd2[6] = {0}, MidPt[3] = {0};

		//get the inverse of the matrix for transforming the points back to original position.
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);
		
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[0], Order2, &LineEnd1[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end1[3], Order2, &LineEnd1[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[0], Order2, &LineEnd2[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &end2[3], Order2, &LineEnd2[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &midP[0], Order2, &MidPt[0]);
		
		//draw the text string
		draw_MeasurementString3D(MidPt, DisString, angleChar, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);
		
		//Draw the 2 straight lines
		drawLine_3DStipple(LineEnd1[0], LineEnd1[1], LineEnd1[2], LineEnd1[3], LineEnd1[4], LineEnd1[5]);
		drawLine_3DStipple(LineEnd2[0], LineEnd2[1], LineEnd2[2], LineEnd2[3], LineEnd2[4], LineEnd2[5]);
		
		double PrjctnLns[12] = {LnPts2[0], LnPts2[1], LnPts2[2],
								PrjEndPt2[0], PrjEndPt2[1], PrjEndPt2[2],
								PrjEndPt2[3], PrjEndPt2[4], PrjEndPt2[5],
								LnPts2[3], LnPts2[4], LnPts2[5]};
	
		drawLine_3DStipple(PrjctnLns[0], PrjctnLns[1], PrjctnLns[2], PrjctnLns[3], PrjctnLns[4], PrjctnLns[5]);
		drawLine_3DStipple(PrjctnLns[3], PrjctnLns[4], PrjctnLns[5], PrjctnLns[6], PrjctnLns[7], PrjctnLns[8]);
		drawLine_3DStipple(PrjctnLns[6], PrjctnLns[7], PrjctnLns[8], PrjctnLns[9], PrjctnLns[10], PrjctnLns[11]);
	
		drawCommonProperties_Angle3D(pt, center, startang, sweepang, radius, slop, angle, angleChar, MeasureName, TransformedEndPt11[2], InverseTransMatrix);
		
		double PrjLn[6] = {PrjEndPt2[0], PrjEndPt2[1], PrjEndPt2[2], Line2[3], Line2[4], Line2[5]};
		//CalculateAndDrawArrowCones(&e13D[3], &e23D[3]);
		//if(slop[0] >= M_PI) slop[0] -= M_PI;
		//if(slop[1] >= M_PI) slop[1] -= M_PI;
		CalculateAndDrawArrowConesAng(LineEnd1, LineEnd2, slop, TransMatrix, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0101.1", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_PlaneDistOrAng3D(double *TheLine, double *LinePts, double *ThePlane, double *PlanePts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1}, PlnEdgePoints[6] = {PlanePts[0], PlanePts[1], PlanePts[2], PlanePts[9], PlanePts[10], PlanePts[11]};
		double IntersectnPts[6] = {0}, IntersectnLnPts[6] = {0}, LineForMeasurement[6] = {0,0,0,0,0,1};
		bool IntersectsPlane = false;
		double TheTmpLine[6] = {0,0,0,0,0,1}, TmpLinePts[6] = {0,0,0,0,0,1};
		
		RMATH3DOBJECT->Intersection_Plane_Plane(ThePlane, MeasurementPlane, IntersectnLn);
		RMATH3DOBJECT->Projection_Point_on_Plane(LinePts, ThePlane, IntersectnLnPts);
		RMATH3DOBJECT->Projection_Point_on_Plane(&LinePts[3], ThePlane, &IntersectnLnPts[3]);

		if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[3], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[6], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlanePts[6], &IntersectnPts[3]);
		}
		if(IntersectsPlane)
		{
			LineForMeasurement[0] = IntersectnPts[0];
			LineForMeasurement[1] = IntersectnPts[1];
			LineForMeasurement[2] = IntersectnPts[2];
			LineForMeasurement[3] = IntersectnPts[3];
			LineForMeasurement[4] = IntersectnPts[4];
			LineForMeasurement[5] = IntersectnPts[5];
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = TheLine[i];
				TmpLinePts[i] = LinePts[i];
			}
		}
		else
		{
			double Dist = 0, ProjectedMidPt[3] = {0}, ProjectedPt1[3] = {0}, ProjectedPt2[3] = {0};
			double LnMidPt[3] = {(LinePts[0] + LinePts[3]) / 2, (LinePts[1] + LinePts[4]) / 2, (LinePts[2] + LinePts[5]) / 2};
			double PlnMidPt[3] = {(PlanePts[0] + PlanePts[6]) / 2, (PlanePts[1] + PlanePts[7]) / 2, (PlanePts[2] + PlanePts[8]) / 2};
			int multiplier = 1;

			//Get the distance from line mid pt to plane
			Dist = RMATH3DOBJECT->Distance_Point_Plane(LnMidPt, ThePlane);

			//Set the multiplier according to pt on +ve or -ve side of the plane
			if(LnMidPt[0] * ThePlane[0] + LnMidPt[1] * ThePlane[1] + LnMidPt[2] * ThePlane[2] > ThePlane[3])
				multiplier = 1;
			else
				multiplier = -1;

			//Now get the pt where the projected line must be drawn
			for(int i = 0; i < 3; i++)
				ProjectedMidPt[i] = PlnMidPt[i] + multiplier * ThePlane[i] * Dist;

			//Get the end pts of the projected line
			double OffsetLength = RMATH3DOBJECT->Distance_Point_Point(&LinePts[0], &LinePts[3]) / 2;
			for(int i = 0 ; i < 3; i++)
			{
				ProjectedPt1[i] = ProjectedMidPt[i] + OffsetLength * TheLine[3 + i];
				ProjectedPt2[i] = ProjectedMidPt[i] - OffsetLength * TheLine[3 + i];
			}

			//Now draw the dotted lines indicating projections of line
			double DrawingPts[12] = {LinePts[0], LinePts[1], LinePts[2],
											 ProjectedPt1[0], ProjectedPt1[1], ProjectedPt1[2],
											 ProjectedPt2[0], ProjectedPt2[1], ProjectedPt2[2],
											 LinePts[3], LinePts[4], LinePts[5]};
			drawStippleLines3D(DrawingPts, 3);
						
			//Set the line and line points
			double DashedLine[6] = {0,0,0,0,0,1}, DashedLinePts[6] = {0};
			for(int i = 0; i < 3; i++)
			{
				DashedLine[i] = ProjectedMidPt[i];
				DashedLine[3 + i] = TheLine[3 + i];
				DashedLinePts[i] = ProjectedPt1[i];
				DashedLinePts[3 + i] = ProjectedPt2[i];
			}
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = DashedLine[i];
				TmpLinePts[i] = DashedLinePts[i];
			}
			RMATH3DOBJECT->Projection_Point_on_Plane(DashedLinePts, ThePlane, IntersectnLnPts);
			RMATH3DOBJECT->Projection_Point_on_Plane(&DashedLinePts[3], ThePlane, &IntersectnLnPts[3]);
			LineForMeasurement[0] = IntersectnLnPts[0];
			LineForMeasurement[1] = IntersectnLnPts[1];
			LineForMeasurement[2] = IntersectnLnPts[2];
			LineForMeasurement[3] = IntersectnLnPts[3];
			LineForMeasurement[4] = IntersectnLnPts[4];
			LineForMeasurement[5] = IntersectnLnPts[5];
		}

		if(!IntersectsPlane)
			drawLine_3DStipple((PlanePts[0] + PlanePts[6]) / 2, (PlanePts[1] + PlanePts[7]) / 2, (PlanePts[2] + PlanePts[8]) / 2,
				(LineForMeasurement[0] + LineForMeasurement[3]) / 2, (LineForMeasurement[1] + LineForMeasurement[4]) / 2, (LineForMeasurement[2] + LineForMeasurement[5]) / 2);
		
		if(DistFlag)
			drawFiniteline_FinitelineDistance3D(IntersectnLn, LineForMeasurement, TheTmpLine, TmpLinePts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		else
			drawFiniteline_FinitelineAngle3D1(IntersectnLn, LineForMeasurement, TheTmpLine, TmpLinePts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0104", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_CylinderDistance3D(double *TheLine, double *LnEndPts, double *TheCylinder, double *CylndrEndPts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double CyLine[6] = {0,0,0,0,0,1}, CyLnPts[6] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Line_Cylinder(TheLine, TheCylinder, CylndrEndPts, MeasureType, CyLine, CyLnPts);
		drawFiniteline_FinitelineDistance3D(TheLine, LnEndPts, CyLine, CyLnPts, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0114", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_CylinderAngle3D(double *TheLine, double *LnEndPts, double *TheCylinder, double *CylndrEndPts, double Angle, char* AngleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		drawFiniteline_FinitelineAngle3D(TheLine, LnEndPts, TheCylinder, CylndrEndPts, Angle, AngleChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0115", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_SphereDistance3D(double *TheLine, double *LinePts, double *TheSphere, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double SprPt[3] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Line_Sphere(TheLine, TheSphere, MeasureType, SprPt);
		drawPoint_FinitelineDistance3D( SprPt, LinePts, TheLine, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0116", __FILE__, __FUNCSIG__); }
}

 void RGraphicsDraw::drawFiniteline_Circle3DDistOrAng3D(double *TheLine, double *LinePts, double *Circle3D, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, bool ProjectLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1};
		double IntersectnPts[6] = {0}, IntersectnLnPts[6] = {0}, LineForMeasurement[6] = {0};
		bool IntersectsPlane = false;
		double TheTmpLine[6] = {0,0,0,0,0,1}, TmpLinePts[6] = {0};
		double Circle3DAsSphere[4] = {Circle3D[0], Circle3D[1], Circle3D[2], Circle3D[6]};
		double CirclePln[4] = {Circle3D[3], Circle3D[4], Circle3D[5], (Circle3D[0]*Circle3D[3] + Circle3D[1]*Circle3D[4] + Circle3D[2]*Circle3D[5])};
		RMATH3DOBJECT->Intersection_Plane_Plane(CirclePln, MeasurementPlane, IntersectnLn);
		RMATH3DOBJECT->Projection_Point_on_Plane(LinePts, CirclePln, IntersectnLnPts);
		RMATH3DOBJECT->Projection_Point_on_Plane(&LinePts[3], CirclePln, &IntersectnLnPts[3]);
		int CntIntrsctn =  RMATH3DOBJECT->Intersection_Line_Sphere(IntersectnLn, Circle3DAsSphere, IntersectnPts);
		if (CntIntrsctn == 2)
		IntersectsPlane = true;

		if(IntersectsPlane)
		{
			LineForMeasurement[0] = IntersectnPts[0];
			LineForMeasurement[1] = IntersectnPts[1];
			LineForMeasurement[2] = IntersectnPts[2];
			LineForMeasurement[3] = IntersectnPts[3];
			LineForMeasurement[4] = IntersectnPts[4];
			LineForMeasurement[5] = IntersectnPts[5];
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = TheLine[i];
				TmpLinePts[i] = LinePts[i];
			}
		}
		
		if(ProjectLine)
		{
			double Dist = 0, ProjectedMidPt[3] = {0}, ProjectedPt1[3] = {0}, ProjectedPt2[3] = {0};
			double LnMidPt[3] = {(LinePts[0] + LinePts[3]) / 2, (LinePts[1] + LinePts[4]) / 2, (LinePts[2] + LinePts[5]) / 2};
			double PlnMidPt[3] = {Circle3D[0], Circle3D[1], Circle3D[2]};
			int multiplier = 1;

			//Get the distance from line mid pt to plane
			Dist = RMATH3DOBJECT->Distance_Point_Plane(LnMidPt, CirclePln);

			//Set the multiplier according to pt on +ve or -ve side of the plane
			if(LnMidPt[0] * CirclePln[0] + LnMidPt[1] * CirclePln[1] + LnMidPt[2] * CirclePln[2] > CirclePln[3])
				multiplier = 1;
			else
				multiplier = -1;

			//Now get the pt where the projected line must be drawn
			for(int i = 0; i < 3; i++)
				ProjectedMidPt[i] = PlnMidPt[i] + multiplier * 	CirclePln[i] * Dist;
					//Get the end pts of the projected line
			double OffsetLength = RMATH3DOBJECT->Distance_Point_Point(&LinePts[0], &LinePts[3]) / 2;
			for(int i = 0 ; i < 3; i++)
			{
				ProjectedPt1[i] = ProjectedMidPt[i] + OffsetLength * TheLine[3 + i];
				ProjectedPt2[i] = ProjectedMidPt[i] - OffsetLength * TheLine[3 + i];
			}

			//Now draw the dotted lines indicating projections of line
			double DrawingPts[12] = {LinePts[0], LinePts[1], LinePts[2],
											 ProjectedPt1[0], ProjectedPt1[1], ProjectedPt1[2],
											 ProjectedPt2[0], ProjectedPt2[1], ProjectedPt2[2],
											 LinePts[3], LinePts[4], LinePts[5]};
			drawStippleLines3D(DrawingPts, 3);
						
			//Set the line and line points
			double DashedLine[6], DashedLinePts[6];
			for(int i = 0; i < 3; i++)
			{
				DashedLine[i] = ProjectedMidPt[i];
				DashedLine[3 + i] = TheLine[3 + i];
				DashedLinePts[i] = ProjectedPt1[i];
				DashedLinePts[3 + i] = ProjectedPt2[i];
			}
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = DashedLine[i];
				TmpLinePts[i] = DashedLinePts[i];
			}
			/*LineForMeasurement[0] = IntersectnLnPts[0];
			LineForMeasurement[1] = IntersectnLnPts[1];
			LineForMeasurement[2] = IntersectnLnPts[2];
			LineForMeasurement[3] = IntersectnLnPts[3];
			LineForMeasurement[4] = IntersectnLnPts[4];
			LineForMeasurement[5] = IntersectnLnPts[5];*/
		}
		
		if(DistFlag)
			drawFiniteline_FinitelineDistance3D(IntersectnLn, LineForMeasurement, TheTmpLine, TmpLinePts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		else
			drawFiniteline_FinitelineAngle3D1(IntersectnLn, LineForMeasurement, TheTmpLine, TmpLinePts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0104", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawFiniteline_Conics3DDistOrAng3D(double *TheLine, double *LinePts, double *Conics3D, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, bool ProjectLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
  {
	try
	{
		double dir[6] = {0,0,1,0,0,1}, Cen[3] = {0}, IntersectnLn[6] = {0,0,0,0,0,1}, IntersectnPts[6] = {0}, IntersectnLnPts[6] = {0}, LineForMeasurement[6] = {0}, TheTmpLine[6] = {0,0,0,0,0,1}, TmpLinePts[6] = {0};
		double Max = (2 * Conics3D[13] *Conics3D[16])/(1 - Conics3D[13] * Conics3D[13]);
		RMATH3DOBJECT->Create_Perpendicular_Line_2_Line(&Conics3D[4], &Conics3D[7], &dir[0]);
		double focallgt = (Max/2) - (Conics3D[13] *Conics3D[16])/(1 + Conics3D[13]);
		for(int i = 0; i < 3; i++) Cen[i] = Conics3D[4 + i] + focallgt * dir[3 + i];

		bool IntersectsPlane = false;
		RMATH3DOBJECT->Intersection_Plane_Plane(Conics3D, MeasurementPlane, IntersectnLn);
		int CntIntrsctn =  RMATH3DOBJECT->Intersection_Line_elipse(IntersectnLn, &Conics3D[0], IntersectnPts);
		RMATH3DOBJECT->Projection_Point_on_Plane(&LinePts[3], &Conics3D[0], &IntersectnLnPts[3]);
		RMATH3DOBJECT->Projection_Point_on_Plane(LinePts, &Conics3D[0], IntersectnLnPts);
	
		if (CntIntrsctn == 2)
		IntersectsPlane = true;

		if(IntersectsPlane)
		{
			LineForMeasurement[0] = IntersectnPts[0];
			LineForMeasurement[1] = IntersectnPts[1];
			LineForMeasurement[2] = IntersectnPts[2];
			LineForMeasurement[3] = IntersectnPts[3];
			LineForMeasurement[4] = IntersectnPts[4];
			LineForMeasurement[5] = IntersectnPts[5];
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = TheLine[i];
				TmpLinePts[i] = LinePts[i];
			}
		}
		
		if(ProjectLine)
		{
			double Dist = 0, ProjectedMidPt[3] = {0}, ProjectedPt1[3] = {0}, ProjectedPt2[3] = {0};
			double LnMidPt[3] = {(LinePts[0] + LinePts[3]) / 2, (LinePts[1] + LinePts[4]) / 2, (LinePts[2] + LinePts[5]) / 2};
			double PlnMidPt[3] = {Cen[0], Cen[1], Cen[2]};
			int multiplier = 1;

			//Get the distance from line mid pt to plane
			Dist = RMATH3DOBJECT->Distance_Point_Plane(LnMidPt, Conics3D);

			//Set the multiplier according to pt on +ve or -ve side of the plane
			if(LnMidPt[0] * Conics3D[0] + LnMidPt[1] * Conics3D[1] + LnMidPt[2] * Conics3D[2] > Conics3D[3])
				multiplier = 1;
			else
				multiplier = -1;

			//Now get the pt where the projected line must be drawn
			for(int i = 0; i < 3; i++)
				ProjectedMidPt[i] = PlnMidPt[i] + multiplier * 	Conics3D[i] * Dist;
					//Get the end pts of the projected line
			double OffsetLength = RMATH3DOBJECT->Distance_Point_Point(&LinePts[0], &LinePts[3]) / 2;
			for(int i = 0 ; i < 3; i++)
			{
				ProjectedPt1[i] = ProjectedMidPt[i] + OffsetLength * TheLine[3 + i];
				ProjectedPt2[i] = ProjectedMidPt[i] - OffsetLength * TheLine[3 + i];
			}

			//Now draw the dotted lines indicating projections of line
			double DrawingPts[12] = {LinePts[0], LinePts[1], LinePts[2],
											 ProjectedPt1[0], ProjectedPt1[1], ProjectedPt1[2],
											 ProjectedPt2[0], ProjectedPt2[1], ProjectedPt2[2],
											 LinePts[3], LinePts[4], LinePts[5]};
			drawStippleLines3D(DrawingPts, 3);
						
			//Set the line and line points
			double DashedLine[6] = {0,0,0,0,0,1}, DashedLinePts[6] = {0};
			for(int i = 0; i < 3; i++)
			{
				DashedLine[i] = ProjectedMidPt[i];
				DashedLine[3 + i] = TheLine[3 + i];
				DashedLinePts[i] = ProjectedPt1[i];
				DashedLinePts[3 + i] = ProjectedPt2[i];
			}
			for(int i = 0; i < 6; i++)
			{
				TheTmpLine[i] = DashedLine[i];
				TmpLinePts[i] = DashedLinePts[i];
			}
		
		}
		
		if(DistFlag)
			drawFiniteline_FinitelineDistance3D(IntersectnLn, LineForMeasurement, TheTmpLine, TmpLinePts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		else
			drawFiniteline_FinitelineAngle3D1(IntersectnLn, LineForMeasurement, TheTmpLine, TmpLinePts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0104", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPlane_PlaneDistOrAng3D(double *ThePlane1, double *Plane1Pts, double *ThePlane2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1}, PlnEdgePoints[6] = {Plane2Pts[0], Plane2Pts[1], Plane2Pts[2], Plane2Pts[9], Plane2Pts[10], Plane2Pts[11]};
		double IntersectnPts[6] = {0};
		bool IntersectsPlane = false;
		
		RMATH3DOBJECT->Intersection_Plane_Plane(ThePlane2, MeasurementPlane, IntersectnLn);
		
		if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
		}
		
		if(IntersectsPlane)
		{
			drawFiniteline_PlaneDistOrAng3D(IntersectnLn, IntersectnPts, ThePlane1, Plane1Pts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
		{
			for(int i = 0; i < 3; i++) IntersectnLn[i] = (Plane2Pts[i] + Plane2Pts[6 + i]) / 2;

			if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[0]))
			{
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);			
			}
			else
			{
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[0]);
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
			}
			drawFiniteline_PlaneDistOrAng3D(IntersectnLn, IntersectnPts, ThePlane1, Plane1Pts, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0109", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPlane_CylinderDistOrAng3D(double *ThePlane, double *PlnPts, double *TheCylinder, double *CylinderPts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistOnly, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double CyLine[6] = {0,0,0,0,0,1}, CyLnPts[6] = {0};
		if(DistOnly)
		{
			RMATH3DOBJECT->MeasureModeCalc_Plane_Cylinder(ThePlane, TheCylinder, CylinderPts, MeasureType, CyLine, CyLnPts);
			drawFiniteline_PlaneDistOrAng3D(CyLine, CyLnPts, ThePlane, PlnPts, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, true, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
			drawFiniteline_PlaneDistOrAng3D(TheCylinder, CylinderPts, ThePlane, PlnPts, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, false, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0105", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPlane_SphereDistance3D(double *ThePlane, double *PlnPts, double *TheSphere, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double SprPt[3] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Plane_Sphere(ThePlane, TheSphere, MeasureType, SprPt);
		drawPoint_PlaneDistance3D(SprPt, ThePlane, PlnPts, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0110", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCircle3D_Circle3DDistOrAng3D(double *Circle3D1, double *Circle3D2, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1};
		double IntersectnPts[6] = {0};
		bool IntersectsPlane = false;
		double Circle1AsPlane[4] = {Circle3D1[3], Circle3D1[4], Circle3D1[5], Circle3D1[3] * Circle3D1[0] + Circle3D1[4] * Circle3D1[1] + Circle3D1[5] * Circle3D1[2]};
		double Circle2AsPlane[4] = {Circle3D2[3], Circle3D2[4], Circle3D2[5], Circle3D2[3] * Circle3D2[0] + Circle3D2[4] * Circle3D2[1] + Circle3D2[5] * Circle3D2[2]};
		double Circle1AsSphere[4] = {Circle3D1[0], Circle3D1[1], Circle3D1[2], Circle3D1[6]};
		double Circle2AsSphere[4] = {Circle3D2[0], Circle3D2[1], Circle3D2[2], Circle3D2[6]};

		RMATH3DOBJECT->Intersection_Plane_Plane(Circle2AsPlane, MeasurementPlane, IntersectnLn);
		int CntIntrsctn = RMATH3DOBJECT->Intersection_Line_Sphere(IntersectnLn, Circle2AsSphere, IntersectnPts);
		if(CntIntrsctn == 2)
		{
			bool IntersectsPlane = true;
			drawFiniteline_Circle3DDistOrAng3D(IntersectnLn, IntersectnPts, Circle3D1, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
		{
			for(int i = 0; i < 3; i++) IntersectnLn[i] = Circle3D2[i];
			RMATH3DOBJECT->Intersection_Line_Sphere(IntersectnLn, Circle2AsSphere, IntersectnPts);
			drawFiniteline_Circle3DDistOrAng3D(IntersectnLn, IntersectnPts, Circle3D1, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0109", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawConics3D_Circle3DDistOrAng3D(double *Conics3D, double *Circle3D, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1};
		double IntersectnPts[6] = {0};
		bool IntersectsPlane = false;
		double CircleAsPlane[4] = {Circle3D[3], Circle3D[4], Circle3D[5], Circle3D[3] * Circle3D[0] + Circle3D[4] * Circle3D[1] + Circle3D[5] * Circle3D[2]};
		double CircleAsSphere[4] = {Circle3D[0], Circle3D[1], Circle3D[2], Circle3D[6]};

		RMATH3DOBJECT->Intersection_Plane_Plane(CircleAsPlane, MeasurementPlane, IntersectnLn);
		int CntIntrsctn = RMATH3DOBJECT->Intersection_Line_Sphere(IntersectnLn, CircleAsSphere, IntersectnPts);
		if(CntIntrsctn == 2)
		{
			bool IntersectsPlane = true;
			drawFiniteline_Conics3DDistOrAng3D(IntersectnLn, IntersectnPts, Conics3D, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
		{
			double dir[6] = {0,0,1,0,0,1}, Cen[3] = {0};
			double Max = (2 * Conics3D[13] *Conics3D[16])/(1 - Conics3D[13] * Conics3D[13]);
			RMATH3DOBJECT->Create_Perpendicular_Line_2_Line(&Conics3D[4], &Conics3D[7], &dir[0]);
			double focallgt = (Max/2) - (Conics3D[13] *Conics3D[16])/(1 + Conics3D[13]);
			for(int i = 0; i < 3; i++) Cen[i] = Conics3D[4 + i] + focallgt * dir[3 + i];

			for(int i = 0; i < 3; i++) IntersectnLn[i] = Cen[i];
			RMATH3DOBJECT->Intersection_Line_elipse(IntersectnLn, CircleAsSphere, IntersectnPts);
			drawFiniteline_Conics3DDistOrAng3D(IntersectnLn, IntersectnPts, Conics3D, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0109", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCircle3D_PlaneDistOrAng3D(double *Circle3D, double *ThePlane2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1}, PlnEdgePoints[6] = {Plane2Pts[0], Plane2Pts[1], Plane2Pts[2], Plane2Pts[9], Plane2Pts[10], Plane2Pts[11]};
		double IntersectnPts[6] = {0};
		bool IntersectsPlane = false;
		
		RMATH3DOBJECT->Intersection_Plane_Plane(ThePlane2, MeasurementPlane, IntersectnLn);
		
		if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
		}
		
		if(IntersectsPlane)
		{
			drawFiniteline_Circle3DDistOrAng3D(IntersectnLn, IntersectnPts, Circle3D, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
		{
			for(int i = 0; i < 3; i++) IntersectnLn[i] = (Plane2Pts[i] + Plane2Pts[6 + i]) / 2;

			if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[0]))
			{
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);			
			}
			else
			{
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[0]);
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
			}
			drawFiniteline_Circle3DDistOrAng3D(IntersectnLn, IntersectnPts, Circle3D, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, true, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0109", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawConics3D_PlaneDistOrAng3D(double *Conics3D, double *ThePlane2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1}, PlnEdgePoints[6] = {Plane2Pts[0], Plane2Pts[1], Plane2Pts[2], Plane2Pts[9], Plane2Pts[10], Plane2Pts[11]};
		double IntersectnPts[6] = {0};
		bool IntersectsPlane = false;
		
		RMATH3DOBJECT->Intersection_Plane_Plane(ThePlane2, MeasurementPlane, IntersectnLn);
		
		if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
		}
		else if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[0]))
		{
			IntersectsPlane = true;
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[3]);
			RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);
		}
		
		if(IntersectsPlane)
		{
			drawFiniteline_Conics3DDistOrAng3D(IntersectnLn, IntersectnPts, Conics3D, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
		{
			for(int i = 0; i < 3; i++) IntersectnLn[i] = (Plane2Pts[i] + Plane2Pts[6 + i]) / 2;

			if(RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[0], &IntersectnPts[0]))
			{
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[6], &IntersectnPts[3]);			
			}
			else
			{
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &Plane2Pts[3], &IntersectnPts[0]);
				RMATH3DOBJECT->Checking_Intersection_2Lines(IntersectnLn, &PlnEdgePoints[0], &IntersectnPts[3]);
			}
			drawFiniteline_Conics3DDistOrAng3D(IntersectnLn, IntersectnPts, Conics3D, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, true, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0109", __FILE__, __FUNCSIG__); }

}

void RGraphicsDraw::drawConics3D_Conics3DDistOrAng3D(double *Conics3D1, double *Conics3D2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double IntersectnLn[6] = {0,0,0,0,0,1}, IntersectnPts[6] = {0};
		bool IntersectsPlane = false;
		
		RMATH3DOBJECT->Intersection_Plane_Plane(Conics3D1, MeasurementPlane, IntersectnLn);
		int CntIntrsctn = RMATH3DOBJECT->Intersection_Line_elipse(IntersectnLn, Conics3D1, IntersectnPts);
		if(CntIntrsctn == 2)
		{
			bool IntersectsPlane = true;
			drawFiniteline_Conics3DDistOrAng3D(IntersectnLn, IntersectnPts, Conics3D2, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
		else
		{
			double dir[6] = {0,0,1,0,0,1}, Cen[3] = {0};
			double Max = (2 * Conics3D1[13] *Conics3D1[16])/(1 - Conics3D1[13] * Conics3D1[13]);
			RMATH3DOBJECT->Create_Perpendicular_Line_2_Line(&Conics3D1[4], &Conics3D1[7], &dir[0]);
			double focallgt = (Max/2) - (Conics3D1[13] *Conics3D1[16])/(1 + Conics3D1[13]);
			for(int i = 0; i < 3; i++) Cen[i] = Conics3D1[4 + i] + focallgt * dir[3 + i];

			for(int i = 0; i < 3; i++) IntersectnLn[i] = Cen[i];
			RMATH3DOBJECT->Intersection_Line_elipse(IntersectnLn, Conics3D2, IntersectnPts);
			drawFiniteline_Conics3DDistOrAng3D(IntersectnLn, IntersectnPts, Conics3D2, DistOrAng, DistAngChar, MeasureName, TransMatrix, MeasurementPlane, DistFlag, MouseSelectnLine, false, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0109", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCylinder_CylinderDistance3D(double *TheCylinder1, double *Cylndr1Pts, double *TheCylinder2, double *Cylndr2Pts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{ 
	try
	{
		double Cy1Line[6] = {0,0,0,0,0,1}, Cy2Line[6] = {0,0,0,0,0,1}, Cy1LnPts[6] = {0}, Cy2LnPts[6] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Cylinder_Cylinder(TheCylinder1, Cylndr1Pts, TheCylinder2, Cylndr2Pts, MeasureType, Cy1Line, Cy1LnPts, Cy2Line, Cy2LnPts);
		drawFiniteline_FinitelineDistance3D(Cy1Line, Cy1LnPts, Cy2Line, Cy2LnPts, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0099", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCylinder_CylinderAngle3D(double *TheCylinder1, double *Cylndr1Pts, double *TheCylinder2, double *Cylndr2Pts, double Angle, char* AngleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		drawFiniteline_FinitelineAngle3D(TheCylinder1, Cylndr1Pts, TheCylinder2, Cylndr2Pts, Angle, AngleChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0100", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCylinder_ConeDistance3D(double *TheCylinder, double *CylndrPts, double *TheCone, double *ConePts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{ 
	try
	{
		double CyLine[6] = {0,0,0,0,0,1}, CyLnPts[6] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Line_Cylinder(TheCone, TheCylinder, CylndrPts, MeasureType, CyLine, CyLnPts);
		drawFiniteline_FinitelineDistance3D( CyLine, CyLnPts, TheCone, ConePts, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0099", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawSphere_CylinderDistance3D(double *TheSphere, double *TheCylinder, double *CylndrPts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double SprPt[3] = {0}, CyLine[6] = {0,0,0,0,0,1}, CyLnPt[6] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Sphere_Cylinder(TheSphere, TheCylinder, CylndrPts, MeasureType, SprPt, CyLine, CyLnPt);
		drawPoint_FinitelineDistance3D(SprPt, CyLnPt, CyLine, Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0106", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawSphere_SphereDistance3D(double *TheSphere1, double *TheSphere2, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double Spr1Pt[3] = {0}, Spr2Pt[3] = {0};
		RMATH3DOBJECT->MeasureModeCalc_Sphere_Sphere(TheSphere1, TheSphere2, MeasureType, Spr1Pt, Spr2Pt);
		drawPoint_PointDistance3D(Spr1Pt[0], Spr1Pt[1], Spr1Pt[2], Spr2Pt[0], Spr2Pt[1], Spr2Pt[2], Distance, DistanceChar, MeasureName, TransMatrix, MeasurementPlane, MouseSelectnLine, windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0108", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawDiametere3D(double *center, double shapeDia, char* distance, char* MeasureName, double *MouseSelectnLine, double *TransMatrix, double *MeasurementPlane,int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double e1[8] =  {0}, s2, MousePos[3] = {0}, InverseTransMatrix[9] = {1,0,0,0,1,0,0,0,1};
		double TransCentre[3] = {0}, TransMPos[3] = {0}, FinalEnd[9] = {0}, TextPos[3] = {0};
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);

		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, MousePos, Order2, TransMPos);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, center, Order2, TransCentre);
		
		RMATH2DOBJECT->Circle_diameter(TransCentre, shapeDia, TransMPos, &DisString[0], &e1[0], &s2);

		double TransEnd1[3] = {e1[0], e1[1], TransCentre[2]}, TransEnd2[3] = {e1[2], e1[3], TransCentre[2]},
			   TransEnd3[3] = {e1[4], e1[5], TransCentre[2]}, TransTextPos[3] = {e1[6], e1[7], TransCentre[2]};

		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd1[0], Order2, &FinalEnd[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd2[0], Order2, &FinalEnd[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd3[0], Order2, &FinalEnd[6]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransTextPos[0], Order2, TextPos);

		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(FinalEnd[0], FinalEnd[1], FinalEnd[2], FinalEnd[3], FinalEnd[4], FinalEnd[5]);
		drawLine_3D(FinalEnd[3], FinalEnd[4], FinalEnd[5], FinalEnd[6], FinalEnd[7], FinalEnd[8]);

		//if (windowno == 0)
		//	drawString_BackGroud(TextPos[0], TextPos[1], 0, 1.2 * max(getTextWidth(distance), getTextWidth(MeasureName)) * Fscalefactor,
		//		1.5 * (getTexHeight(distance) + getTexHeight(MeasureName)) * Fscalefactor, 0, 0, 0);

		//Draw the measurement strings
		draw_MeasurementString3D(TextPos, DisString, distance, MeasureName, windowno, Fscalefactor, Lefttop, RightBtm);

		//Draw the arrow cones
		double ArrowHeadPt[3] = {FinalEnd[0], FinalEnd[1], FinalEnd[2]};
		double ArrowTailPt[3] = {FinalEnd[3], FinalEnd[4], FinalEnd[5]};
		DrawArrowCone(ArrowHeadPt, ArrowTailPt, w_upp);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0117", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawDiametere3D(double *cylinder, double *cylinderEndPts, char* distance, char* MeasureName, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double CyMid[3] = {(cylinderEndPts[0] + cylinderEndPts[3]) / 2, (cylinderEndPts[1] + cylinderEndPts[4]) / 2, (cylinderEndPts[2] + cylinderEndPts[5]) / 2};
		//get the measurement Plane
		double TransMatrix[9] = {1,0,0,0,1,0,0,0,1}, MeasurementPlane[4] = {cylinder[3], cylinder[4], cylinder[5], CyMid[0] * cylinder[3] + CyMid[1] * cylinder[4] + CyMid[2] * cylinder[5]};
		//Now the intersection of measurement plane and selection line is the mouse pt reqd
		RMATH3DOBJECT->GetTMatrixForPlane(MeasurementPlane, TransMatrix);
		drawDiametere3D(CyMid, cylinder[6] * 2, distance, MeasureName, MouseSelectnLine, TransMatrix, MeasurementPlane, windowno,  w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0118", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawAxis3D(double *conics3D, double max, double min, double disvalue, char* distance, char* MeasureName, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double measuretype, double* Lefttop, double* RightBtm)
{
	try
	{
		double dir[6] = {0,0,1,0,0,1}, Cen[3] = {0}, pt[6] = {0};
		RMATH3DOBJECT->Create_Perpendicular_Line_2_Line(&conics3D[4], &conics3D[7], &dir[0]);
		double focallgt = (max/2) - (conics3D[13] *conics3D[16])/(1 + conics3D[13]);
		for(int i = 0; i < 3; i++) Cen[i] = conics3D[4 + i] + focallgt * dir[3 + i];
		if(measuretype == 0)
		{
			for(int i = 0; i < 3; i++) 
			{
				pt[i] = Cen[i] + (max/2) * dir[3 + i];
				pt[i + 3] = Cen[i] - (max/2) * dir[3 + i];
			}
		}
		else
		{
			for(int i = 0; i < 3; i++) 
			{
				pt[i] = Cen[i] + (min / 2) * conics3D[10 + i];
				pt[i + 3] = Cen[i] - (min / 2) * conics3D[10 + i];
			}
		}

		double TransformationMatrix[9] = {1,0,0,0,1,0,0,0,1}, MeasurementPlane[4] = {0,0,1,0};
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(&pt[0], &pt[3], &TransformationMatrix[0], &MeasurementPlane[0]);
		drawPoint_PointDistance3D(pt[0], pt[1], pt[2], pt[3], pt[4], pt[5], disvalue, distance, MeasureName, &TransformationMatrix[0], &MeasurementPlane[0], MouseSelectnLine,  windowno, w_upp, Fscalefactor, Lefttop, RightBtm);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0118", __FILE__, __FUNCSIG__); }
}


void RGraphicsDraw::drawAngularity3D(double *MouseSelectnLine, double Midx, double Midy, double Midz, char* distance, char* s1n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		s1n[1] = '\0';
		std::string temp, temp1, temp2;
		temp = (const char*)distance;
		temp1 = (const char*)s1n;
		//if(type == 0)
		//	temp2 = "   " + temp + " " + temp1;
		//else 
		if(type == 1)
			temp2 = "// " + temp + " " + temp1;
		else
			temp2 = "   " + temp + " " + temp1;

		distance = (char*)temp2.c_str();
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double e1[8] = {0}, s2 = 0, MousePos[3] = {0}, p1[2] = {Midx, Midy};
		//get the measurement Plane
		double MeasurementPlane[4] = {0, 0, 1, Midz};
		//Now the intersection of measurement plane and selection line is the mouse pt reqd
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);
		RMATH2DOBJECT->Circle_diameter(&p1[0], 0, &MousePos[0], &DisString[0], &e1[0], &s2);
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(e1[0], e1[1], Midz, e1[2], e1[3], Midz);
		drawLine_3D(e1[2], e1[3], Midz, e1[4], e1[5], Midz);
		//Draw the arrow cone
		double ArrowHeadPt[3] = {e1[0], e1[1], Midz};
		double ArrowTailPt[3] = {e1[2], e1[3], Midz};
		DrawArrowCone(ArrowHeadPt, ArrowTailPt, w_upp);
		double strPos[3] = {e1[6], e1[7], Midz};
		Calculate_MeasureRectangle(strPos, &DisString[0], Lefttop, RightBtm);
		if(windowno == 0)
			drawString_BackGroud(e1[6], e1[7], 0, DisString[0], DisString[1], 0, 0, 0);
		drawAngularity_MeasureString(&strPos[0], (char*)temp2.c_str(), MeasureName, DisString[0], DisString[1], type, Fscalefactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0126", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawAngularityCylinder(double *MouseSelectnLine, double *cylinder, double *cylinderEndPts, char* distance, char* s1n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm)
{
	try
	{
		double e1[8] = {0}, s2 = 0, MousePos[3] = {0}, TransMatrix[9] = {1,0,0,0,1,0,0,0,1}, InverseTransMatrix[9] = {1,0,0,0,1,0,0,0,1};
		double TransCentre[3] = {0}, TransMPos[3] = {0};
		double FinalEnd[9] = {0}, TextPos[3] = {0};
		s1n[1] = '\0';
		std::string temp, temp1, temp2;
		temp = (const char*)distance;
		temp1 = (const char*)s1n;
		if(type != 1)
			temp2 = "   " + temp + " " + temp1;
		else 
			temp2 = "// " + temp + " " + temp1;
		//else
		//	temp2 = "   " + temp + " " + temp1;
		distance = (char*)temp2.c_str();
		double DisString[2] = {getTextWidth(distance) * Fscalefactor, getTexHeight(distance) * Fscalefactor};
		double CyMid[3] = {(cylinderEndPts[0] + cylinderEndPts[3]) / 2, (cylinderEndPts[1] + cylinderEndPts[4]) / 2, (cylinderEndPts[2] + cylinderEndPts[5]) / 2};
		//get the measurement Plane
		double MeasurementPlane[4] = {cylinder[3], cylinder[4], cylinder[5], CyMid[0] * cylinder[3] + CyMid[1] * cylinder[4] + CyMid[2] * cylinder[5]};
		//Now the intersection of measurement plane and selection line is the mouse pt reqd
		RMATH3DOBJECT->Intersection_Line_Plane(MouseSelectnLine, MeasurementPlane, MousePos);
		RMATH3DOBJECT->GetTMatrixForPlane(MeasurementPlane, TransMatrix);
		RMATH2DOBJECT->OperateMatrix4X4(TransMatrix, 3, 1, InverseTransMatrix);

		int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &MousePos[0], Order2, TransMPos);
		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &CyMid[0], Order2, TransCentre);
		
		RMATH2DOBJECT->Circle_diameter(TransCentre, cylinder[6] * 2, TransMPos, &DisString[0], &e1[0], &s2);

		double TransEnd1[3] = {e1[0], e1[1], TransCentre[2]}, TransEnd2[3] = {e1[2], e1[3], TransCentre[2]},
			   TransEnd3[3] = {e1[4], e1[5], TransCentre[2]}, TransTextPos[3] = {e1[6], e1[7], TransCentre[2]};

		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd1[0], Order2, &FinalEnd[0]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd2[0], Order2, &FinalEnd[3]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransEnd3[0], Order2, &FinalEnd[6]);
		RMATH2DOBJECT->MultiplyMatrix1(InverseTransMatrix, Order1, &TransTextPos[0], Order2, TextPos);

		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3D(FinalEnd[0], FinalEnd[1], FinalEnd[2], FinalEnd[3], FinalEnd[4], FinalEnd[5]);
		drawLine_3D(FinalEnd[3], FinalEnd[4], FinalEnd[5], FinalEnd[6], FinalEnd[7], FinalEnd[8]);

		//Draw the arrow cones
		double ArrowHeadPt[3] = {FinalEnd[0], FinalEnd[1], FinalEnd[2]};
		double ArrowTailPt[3] = {FinalEnd[3], FinalEnd[4], FinalEnd[5]};
		DrawArrowCone(ArrowHeadPt, ArrowTailPt, w_upp);
		Calculate_MeasureRectangle(&TextPos[0], &DisString[0], Lefttop, RightBtm);
		if(windowno == 0)
			drawString_BackGroud(TextPos[0], TextPos[1], 0, DisString[0], DisString[1], 0, 0, 0);
		drawAngularity_MeasureString(&TextPos[0], (char*)temp2.c_str(), MeasureName, DisString[0], DisString[1], type, Fscalefactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}


void RGraphicsDraw::drawStippleLines3D(double pts[], int NoOfLns)
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		for(int i = 0; i < NoOfLns; i++)
			drawLine_3DStipple(pts[3 * i], pts[3 * i + 1], pts[3 * i + 2], pts[3 * i + 3], pts[3 * i + 4], pts[3 * i + 5]);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0119", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawMeasureLine3D(double e1[], double e2[])
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawLine_3DStipple(e1[0], e1[1], e1[2], e1[3], e1[4], e1[5]);
		drawLine_3DStipple(e2[0], e2[1], e2[2], e2[3], e2[4], e2[5]);
		drawLine_3DStipple(e1[3], e1[4], e1[5], e1[6], e1[7], e1[8]);
		drawLine_3DStipple(e2[3],e2[4], e2[5],e2[6], e2[7], e2[8]);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0120", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawCommonProperties_Angle3D(double pt[], double center[], double startang[], double sweepang[], double radius, double slop[], double DisString, char* strdistance, char* MeasureName, double ZLevel, double *TransMatrix)
{
	try
	{
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawArc3D(center[0],center[1],radius,startang[0],sweepang[0], true, ZLevel, TransMatrix);
		drawArc3D(center[0],center[1],radius,startang[1],sweepang[1], true, ZLevel, TransMatrix);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0121", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::DrawArrowCone(double *Point1, double *Point1Tail, double w_upp)
{
	try
	{
		double Cosines1[3] = {0}, Ratios1[3] = {Point1Tail[0] - Point1[0], Point1Tail[1] - Point1[1], Point1Tail[2] - Point1[2]};
		double ConeLength  = w_upp * ArrowSize, ConeRadius = w_upp * ArrowSize/4, Cone1Base[3] = {0};
	
		RMATH3DOBJECT->DirectionCosines(Ratios1, Cosines1);
		
		Cone1Base[0] = Point1[0] + Cosines1[0] * ConeLength;
		Cone1Base[1] = Point1[1] + Cosines1[1] * ConeLength;
		Cone1Base[2] = Point1[2] + Cosines1[2] * ConeLength;

		drawConeFor3DAxis(Point1[0], Point1[1], Point1[2], Cone1Base[0], Cone1Base[1], Cone1Base[2], 0 , ConeRadius);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0122", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::CalculateAndDrawArrowConesAng(double *Point1, double *Point2, double *ArrowSlopes, double *TransMatrix, double w_upp)
{
	try
	{
		double ArrowDir1[3] = {0,0,1}, ArrowDir2[3] = {0,0,1};
		double Dir1eq1[4] = {TransMatrix[0], TransMatrix[1], TransMatrix[2], cos(ArrowSlopes[0])};
		double Dir1eq2[4] = {TransMatrix[3], TransMatrix[4], TransMatrix[5], sin(ArrowSlopes[0])};
		double Dir1eq3[4] = {TransMatrix[6], TransMatrix[7], TransMatrix[8], 0};
		double Dir2eq1[4] = {TransMatrix[0], TransMatrix[1], TransMatrix[2], cos(ArrowSlopes[1])};
		double Dir2eq2[4] = {TransMatrix[3], TransMatrix[4], TransMatrix[5], sin(ArrowSlopes[1])};
		double Dir2eq3[4] = {TransMatrix[6], TransMatrix[7], TransMatrix[8], 0};

		RMATH3DOBJECT->SolveEquation(Dir1eq1, Dir1eq2, Dir1eq3, ArrowDir1);
		RMATH3DOBJECT->SolveEquation(Dir2eq1, Dir2eq2, Dir2eq3, ArrowDir2);
	
		//double Cosines1[3], Ratios1[3] = {Point1Tail[0] - Point1[0], Point1Tail[1] - Point1[1], Point1Tail[2] - Point1[2]};
		//double Cosines2[3], Ratios2[3] = {Point2Tail[0] - Point2[0], Point2Tail[1] - Point2[1], Point2Tail[2] - Point2[2]};
		double ConeLength  = w_upp * ArrowSize, ConeRadius = w_upp * ArrowSize/4, Cone1Base[3], Cone2Base[3];

		//RMATH3DOBJECT->DirectionCosines(Ratios1, Cosines1);
		//RMATH3DOBJECT->DirectionCosines(Ratios2, Cosines2);
	
		Cone1Base[0] = Point1[0] + ArrowDir1[0] * ConeLength;
		Cone1Base[1] = Point1[1] + ArrowDir1[1] * ConeLength;
		Cone1Base[2] = Point1[2] + ArrowDir1[2] * ConeLength;

		Cone2Base[0] = Point2[0] + ArrowDir2[0] * ConeLength;
		Cone2Base[1] = Point2[1] + ArrowDir2[1] * ConeLength;
		Cone2Base[2] = Point2[2] + ArrowDir2[2] * ConeLength;

		drawConeFor3DAxis(Point1[0], Point1[1], Point1[2], Cone1Base[0], Cone1Base[1], Cone1Base[2], 0 , ConeRadius);
		drawConeFor3DAxis(Point2[0], Point2[1], Point2[2], Cone2Base[0], Cone2Base[1], Cone2Base[2], 0 , ConeRadius);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0123", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::CalculateAndDrawArrowCones(double *Point1, double *Point2, double *Point1Tail, double *Point2Tail, double w_upp)
{
	try
	{
		double Cosines1[3] = {0,0,1}, Ratios1[3] = {Point1Tail[0] - Point1[0], Point1Tail[1] - Point1[1], Point1Tail[2] - Point1[2]};
		double Cosines2[3] = {0,0,1}, Ratios2[3] = {Point2Tail[0] - Point2[0], Point2Tail[1] - Point2[1], Point2Tail[2] - Point2[2]};
		double ConeLength  = w_upp * ArrowSize, ConeRadius = w_upp * ArrowSize/4, Cone1Base[3], Cone2Base[3];

		RMATH3DOBJECT->DirectionCosines(Ratios1, Cosines1);
		RMATH3DOBJECT->DirectionCosines(Ratios2, Cosines2);
	
		Cone1Base[0] = Point1[0] + Cosines1[0] * ConeLength;
		Cone1Base[1] = Point1[1] + Cosines1[1] * ConeLength;
		Cone1Base[2] = Point1[2] + Cosines1[2] * ConeLength;

		Cone2Base[0] = Point2[0] + Cosines2[0] * ConeLength;
		Cone2Base[1] = Point2[1] + Cosines2[1] * ConeLength;
		Cone2Base[2] = Point2[2] + Cosines2[2] * ConeLength;

		drawConeFor3DAxis(Point1[0], Point1[1], Point1[2], Cone1Base[0], Cone1Base[1], Cone1Base[2], 0 , ConeRadius);
		drawConeFor3DAxis(Point2[0], Point2[1], Point2[2], Cone2Base[0], Cone2Base[1], Cone2Base[2], 0 , ConeRadius);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0124", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_MeasurementString3D(double m1[],double Dis[],char* Distance, char* MeasureName, int windowno, double FScaleFactor, double* Lefttop, double* RightBtm)
{
	try
	{
		Lefttop[0] = m1[0] - 2 * Dis[0]/3; Lefttop[1] = m1[1] + 3 * Dis[1]/2;
		RightBtm[0] = m1[0] + 2 * Dis[0]/3; RightBtm[1] = m1[1] - 3 * Dis[1]/2;
		Lefttop[3] = m1[0]; Lefttop[4] = m1[1]; Lefttop[5] = m1[2];
		glColor3d(Measure_Color.r, Measure_Color.g , Measure_Color.b);
		drawStringEdited3D(m1[0], m1[1], m1[2], 0, Distance, FScaleFactor);
		drawStringEdited3D1(m1[0], m1[1], m1[2], 0, MeasureName, FScaleFactor);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0125", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawProbe_Z(double x, double y, double z)
{
	try
	{
		glColor4f(1.0f, 0.4f, 0.4f, 0.4f);
		EnableTransparency();
		drawSphere_Probe(x, y, z, 0.45);
		DisableTransparency();
		glColor4f(0.0f, 0.5f, 0.2f, 0.4f);
		EnableTransparency();
		drawcylinder_Probe(x, y, z + 0.44, x, y, z + 6, 0.3, 0.3, true, true);
		drawcylinder_Probe(x, y, z + 6, x, y, z + 7, 0.3, 0.6, true, true);
		drawcylinder_Probe(x, y, z + 7, x, y, z + 8, 0.6, 0.6, true, true);
		drawcylinder_Probe(x, y, z + 8, x, y, z + 8.6, 0.8, 0.8, true, true);
		drawcylinder_Probe(x, y, z + 8.6, x, y, z + 9.2, 1.4, 1.5, true, true);
		drawcylinder_Probe(x, y, z + 9.2, x, y, z + 10.2, 1.5, 1.5, true, true);
		DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawProbe_Y(double x, double y, double z)
{
	try
	{
		glColor3f(1, 0, 0);
		drawSphere_Probe(x, y, z, 500);
		//glColor3f(0, 0.5, 0.5);
		drawcylinder_Probe(x, y + 500, z, x, y + 2500, z, 300, 300);
		drawcylinder_Probe(x, y + 2500, z, x, y + 3500, z, 300, 600);
		drawcylinder_Probe(x, y + 3500, z, x, y + 4500, z, 800, 800);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0073", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawcylinder_Probe(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius, double Bradius, bool topdisk, bool bottomdisk)
{
	try
	{
		double vx = x2 - x1, vy = y2 - y1, vz = z2 - z1;
		//handle the degenerate case with an approximation//
		if(vz == 0)
			vz = .00000001;
		double v = sqrt(vx * vx + vy * vy + vz * vz);
		double ax = 57.2957795 * acos(vz/v); 
		if(vz < 0.0) ax = -ax;
		double rx = -vy * vz, ry = vx * vz;

		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
			
		glPushMatrix();
		glTranslatef(x1, y1, z1);
		glRotatef(ax, rx, ry, 0.0);
		gluCylinder(quadric, Tradius, Bradius, v, 40, 1);
		if(topdisk)
		{
			gluQuadricOrientation(quadric,GLU_INSIDE);
			gluDisk(quadric, 0.0, Tradius, 40, 1);
			glTranslatef(0, 0, v);
		}
		if(bottomdisk)
		{
			gluQuadricOrientation(quadric,GLU_OUTSIDE);
			gluDisk(quadric, 0.0, Bradius, 40, 1);
		}
		glPopMatrix();
		gluDeleteQuadric(quadric);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0074", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawSphere_Probe(double x, double y, double z, double radius)
{
	try
	{
		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		glPushMatrix();
		glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);	
		gluSphere(quadric, radius, 40, 40);
		glDisable(GL_BLEND);
		glPopMatrix();
		gluDeleteQuadric(quadric);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0075", __FILE__, __FUNCSIG__); }
}

//Draw single point..//
void RGraphicsDraw::drawPoint(double x, double y, double z, GLfloat pointsize)
{
	try
	{		
		glPointSize(pointsize);
		glBegin(GL_POINTS);
		glVertex3d(x, y, z);
		glEnd();
		glPointSize(1.0f);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawPoints(double *pts, int count, bool AllowTransparency, bool GreenColor, int numberofplane, GLfloat pointsize)
{
	try
	{
		if(GreenColor)
			glColor3f(0, 1, 0);
		else
			glColor3f(1, 0, 0);
		if(AllowTransparency)
			EnableTransparency();
		glPointSize(PointSize);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(numberofplane, GL_DOUBLE, 0, pts);
		glDrawArrays(GL_POINTS, 0, count);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPointSize(1.0f);	//reset point size
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//draw points... array of points..//
void RGraphicsDraw::drawPoints(double *pts, int count, bool AllowTransparency, int numberofplane, GLfloat pointsize)
{
	try
	{
		//Defines the point size...//
		//glPushMatrix();
		if(AllowTransparency)
			EnableTransparency();
		glPointSize(PointSize);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(numberofplane, GL_DOUBLE, 0, pts);
		glDrawArrays(GL_POINTS, 0, count);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPointSize(1.0f);	//reset point size
		if(AllowTransparency)
			DisableTransparency();
		//glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::DrawPointsWithColor(double X, double Y, double Z, double R, double G, double B, GLfloat pointsize)
{
	try
	{
		glPointSize(PointSize);
		glBegin(GL_POINTS);
		glColor3d(R, G, B);
		glVertex3d(X, Y, Z);
		glEnd();
		glPointSize(1.0f);	//reset point size
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RGRAPHICS0127a", __FILE__, __FUNCSIG__);
	}
}

//Draw Points in different colr based on z level........
void RGraphicsDraw::drawPoints(double *pts, int count, double MinZ, double MaxZ, bool AllowTransparency, bool DiscreteColor, double tolerance, GLfloat pointsize)
{
	try
	{
		if(AllowTransparency)
			EnableTransparency();
		glPointSize(PointSize);
		glBegin(GL_POINTS);
		double Z1 = tolerance;
		if(tolerance == 0)
			Z1 = (MaxZ - MinZ) / 6;
		double Z2 = 2 * Z1, Z3 = 3 * Z1, Z4 = 4 * Z1, Z5 = 5 * Z1;
		if(DiscreteColor)
		{
			for(int i = 0; i < count; i++)
			{
				double colorFactor = (pts[3 * i + 2] - MinZ);
				if(colorFactor <= Z1)
					glColor3d(0, 0, 1);
				else if(colorFactor <= Z2)
					glColor3d(0, 0.25, 0.75);
				else if(colorFactor <= Z3)
					glColor3d(0, 0.75, 0.25);
				else if(colorFactor <= Z4)
					glColor3d(0.25, 0.75, 0);
				else if(colorFactor <= Z5)
					glColor3d(0.75, 0.25, 0);
				else
					glColor3d(1, 0, 0);
				glVertex3d(pts[3 * i], pts[3 * i + 1], pts[3 * i + 2]);
			}
		}
		else
		{
			for(int i = 0; i < count; i++)
			{
				double colorFactor = pts[3 * i + 2] - MinZ;
				if(colorFactor <= Z1)
					glColor3d(0, 0, 1);
				else if(colorFactor <= Z2)
					glColor3d(0, ((colorFactor - Z1) / Z1) * 0.25, (1 - 0.25) * ((colorFactor - Z1) / Z1));
				else if(colorFactor <= Z3)
					glColor3d(0, 0.25 + ((colorFactor - Z2) / Z1) * 0.5, 0.75 - (1 - 0.5) * ((colorFactor - Z2) / Z1));
				else if(colorFactor <= Z4)
					glColor3d(((colorFactor - Z3) / Z1) * 0.25, 0.75, 0.25 - (1 - 07.5) * ((colorFactor - Z3) / Z1));
				else if(colorFactor <= Z5)
					glColor3d(0.25 + ((colorFactor - Z4) / Z1) * 0.5, 0.75 - (1 - 0.5) * ((colorFactor - Z4) / Z1), 0);
				else
					glColor3d(1, 0, 0);
				glVertex3d(pts[3 * i], pts[3 * i + 1], pts[3 * i + 2]);
			}
		}
		glEnd();
		glPointSize(1.0f);	//reset point size
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127a", __FILE__, __FUNCSIG__); }
}

//Draw the finite line...//
void RGraphicsDraw::drawLine(double x1, double y1, double x2, double y2)
{
	try
	{
		glBegin(GL_LINES);
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
		glEnd();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//draw dash line... Just enable the stipple mode while drawing the line...//
void RGraphicsDraw::drawLineStipple(double x1, double y1, double x2, double y2, int PixelCount, GLushort Lpattern)
{
	try
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(PixelCount, Lpattern);
		glBegin(GL_LINES);
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//Draw circle with given center and radius....//
void RGraphicsDraw::drawCircle(double x, double y, double radius, bool draw_Center)
{
	try
	{
		//circle with center and radius
		int i = 0;
		GLdouble pts[730] = {0};
		unsigned int ptsorder[365] = {0};
		unsigned int ptscount = 0;
		//Circle is set of line... line loop....
		for(double angle = 0; angle < M_PI * 2; angle += IncrementAngle)
		{
			//if(i >= 720) break;
			ptsorder[ptscount++] = ptscount;
			pts[i++] = (radius * cos(angle)) + x;
			pts[i++] = (radius * sin(angle)) + y;
		}
		ptsorder[ptscount++] = 0;
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, pts);
		glDrawElements(GL_LINE_LOOP, ptscount, GL_UNSIGNED_INT, ptsorder);
		glDisableClientState(GL_VERTEX_ARRAY);
		if(draw_Center) drawPoint(x, y, 0);
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); 
	}
}

//Draw circle with given center and radius....//
void RGraphicsDraw::drawCircleStipple(double x, double y, double radius, int PixelCount, GLushort Lpattern)
{
	try
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(PixelCount, Lpattern);
		int i = 0;
		GLdouble pts[730] = {0};
		unsigned int ptsorder[365] = {0};
		unsigned int ptscount = 0;
		//Circle is set of line... line loop....
		for(double angle = 0; angle < M_PI * 2; angle += IncrementAngle)
		{
			//if(i >= 720) break;
			ptsorder[ptscount++] = ptscount;
			pts[i++] = (radius * cos(angle)) + x;
			pts[i++] = (radius * sin(angle)) + y;
		}
		ptsorder[ptscount++] = 0;
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, pts);
		glDrawElements(GL_LINE_LOOP, ptscount, GL_UNSIGNED_INT, ptsorder);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_LINE_STIPPLE);
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); 
	}
}

// Draw the arc with given parameters...//
void RGraphicsDraw::drawArc(double x, double y, double radius, double startangle, double sweepangle, double wupp)
{
	try
	{
		double incrangle = IncrementAngle;
		if (wupp != 0)
		{
			if (10 * wupp < radius * incrangle)
				incrangle = 10 * wupp / radius;
		}
		if (sweepangle > 360 * incrangle)
		{
			incrangle = sweepangle / 360;
		}
		if(sweepangle < 2 * incrangle)
		{
			double point1[2] = {(radius * cos(startangle)) + x, (radius * sin(startangle)) + y};
			double point2[2] = {(radius * cos(startangle + sweepangle)) + x, (radius * sin(startangle + sweepangle)) + y};
			drawLine(point1[0], point1[1], point2[0], point2[1]);
		}
		else
		{
			double endangle = startangle+sweepangle;
			int i = 0;	
			GLdouble pts[730] = {0};
			unsigned int ptsorder[365] = {0};
			unsigned int ptscount = 0;
			for(double angle = startangle; angle < endangle; angle += incrangle)
			{
				if(i >= 726) break;
				ptsorder[ptscount++] = ptscount;
				pts[i++] = (radius * cos(angle)) + x;
				pts[i++] = (radius * sin(angle)) + y;
			}
			ptsorder[ptscount++] = ptscount;
			pts[i++] = (radius * cos(endangle)) + x;
			pts[i++] = (radius * sin(endangle)) + y;
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_DOUBLE, 0, pts);
			glDrawElements(GL_LINE_STRIP, ptscount, GL_UNSIGNED_INT, ptsorder);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); 
	}
}

// Draw the arc with given parameters...//
void RGraphicsDraw::drawArcStipple(double x, double y, double radius, double startangle, double sweepangle, int PixelCount, GLushort Lpattern)
{
	try
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(PixelCount, Lpattern);
		if(sweepangle < 2 * IncrementAngle)
		{
			double point1[2] = {(radius * cos(startangle)) + x, (radius * sin(startangle)) + y};
			double point2[2] = {(radius * cos(startangle + sweepangle)) + x, (radius * sin(startangle + sweepangle)) + y};
			drawLine(point1[0], point1[1], point2[0], point2[1]);
		}
		else
		{
			double endangle = startangle+sweepangle;
			int i = 0;
			GLdouble pts[730] = {0};
			unsigned int ptsorder[365] = {0};
			unsigned int ptscount = 0;
			for(double angle = startangle; angle < endangle; angle += IncrementAngle)
			{
				if(i >= 726) break;
				ptsorder[ptscount++] = ptscount;
				pts[i++] = (radius * cos(angle)) + x;
				pts[i++] = (radius * sin(angle)) + y;
			}
			ptsorder[ptscount++] = ptscount;
			pts[i++] = (radius * cos(endangle)) + x;
			pts[i++] = (radius * sin(endangle)) + y;
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_DOUBLE, 0, pts);
			glDrawElements(GL_LINE_STRIP, ptscount, GL_UNSIGNED_INT, ptsorder);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glDisable(GL_LINE_STIPPLE);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//Draw the rectangle...//
void RGraphicsDraw::drawRectangle(double x1, double y1, double x2, double y2, bool fill)
{
	drawRectangle(x1, y1, x2, y1, x2, y2, x1, y2, fill);
}

//draw the rectangle...//
void RGraphicsDraw::drawRectangle(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool fill)
{
	try
	{
		GLdouble mix[]={x1, y1, x2, y2, x3, y3, x4, y4};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 3, 0};
		if (fill)glDrawElements(GL_POLYGON, 5, GL_UNSIGNED_INT, indices);
		else glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, bool fill)
{
	try
	{
		GLdouble mix[]={x1, y1, x2, y2, x3, y3};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 0};
		if (fill)glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
		else glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//draw cross mark
void RGraphicsDraw::drawCrossMark(double x, double y, double z, double wupp)
{
	try
	{
		drawLine_3D(x - wupp, y + wupp, z, x + wupp, y - wupp, z);
		drawLine_3D(x - wupp, y - wupp, z, x + wupp, y + wupp, z);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//Draw ray / infinite line...//
void RGraphicsDraw::drawXLineOrXRay(double angle, double x1, double y1, double w_upp, bool Infinite, double Zlevel)
{
	try
	{
		if(Infinite)
		{
			if(angle == 0)
				drawLine_3D(MIN_X * w_upp + x1, y1, Zlevel, MAX_X * w_upp + x1, y1, Zlevel);
			else if(angle == M_PI_2)
				drawLine_3D(x1, MIN_Y * w_upp + y1, Zlevel, x1, MAX_Y * w_upp + y1, Zlevel);
			else
				drawLine_3D(MIN_X * w_upp * cos(angle) + x1, MIN_Y * w_upp * sin(angle) + y1, Zlevel, MAX_X * w_upp * cos(angle) + x1, MAX_Y * w_upp  * sin(angle) + y1, Zlevel);
		}
		else
			drawLine_3D(x1, y1, Zlevel, MAX_X * w_upp * cos(angle) + x1, MAX_Y * w_upp  * sin(angle) + y1, Zlevel);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw3DXLine(double* DirCosines, double x1, double y1, double z1, double w_upp)
{
	try
	{
		drawLine_3D(MIN_X * w_upp * DirCosines[0] + x1, MIN_Y * w_upp * DirCosines[1] + y1, MIN_Z * w_upp * DirCosines[2] + z1, MAX_X * w_upp * DirCosines[0] + x1, MAX_Y * w_upp  * DirCosines[1] + y1, MAX_Z * w_upp * DirCosines[2] + z1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127a", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawXLineOrXRayStipple(double angle, double x1, double y1, double w_upp, bool Infinite, int PixelCount, GLushort Lpattern)
{
	try
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(PixelCount, Lpattern);
		if(Infinite)
		{
			if(angle == 0)
				drawLine(MIN_X * w_upp + x1, y1, MAX_X * w_upp + x1, y1);
			else if(angle == M_PI_2)
				drawLine(x1, MIN_Y * w_upp + y1, x1, MAX_Y * w_upp + y1);
			else
				drawLine(MIN_X * w_upp * cos(angle) + x1, MIN_Y * w_upp * sin(angle) + y1, MAX_X * w_upp * cos(angle) + x1, MAX_Y * w_upp  * sin(angle) + y1);
		}
		else
			drawLine(x1, y1, MAX_X * w_upp * cos(angle) + x1, MAX_Y * w_upp  * sin(angle) + y1);
		glDisable(GL_LINE_STIPPLE);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawXLineThruPoint_Stipple(double x, double y, double w_upp, bool Vertical, GLushort)
{
	try
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(5, 0x5F5F);
		GLdouble Xaxis_Array[8] = {x, y, MIN_X * w_upp, y, x, y, MAX_X * w_upp, y};
		GLdouble Yaxis_Array[8] = {x, y, x, MIN_Y * w_upp, x, y, x, MAX_Y * w_upp};
		unsigned int indices[] = {0, 1, 2, 3};
		if(Vertical)
		{
			glVertexPointer(2, GL_DOUBLE, 0, Yaxis_Array);
			glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, indices);
		}
		else
		{
			glVertexPointer(2, GL_DOUBLE, 0, Xaxis_Array);
			glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, indices);
		}
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_LINE_STIPPLE);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//Draw lines in loop... Used to draw polyline..
void RGraphicsDraw::drawLineloop( double *pts, int cnt,int* lineloop, int numberofPlane )
{
	try
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0,(GLdouble*) pts);
		glDrawElements(GL_LINES, cnt, GL_UNSIGNED_INT, lineloop);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//Draw Spline Shape....
void RGraphicsDraw::drawSpline(double* PtsArray, int ncpts, double* Knots_Array, int degree)
{
	try
	{
		int nknots = ncpts + degree + 1;
		int Order = ncpts;
		if(ncpts > 3)
			Order = 4;
		GLUnurbs * nurbsObject = gluNewNurbsRenderer();
		gluNurbsProperty(nurbsObject, GLU_SAMPLING_TOLERANCE, 5.0);		
		gluBeginCurve(nurbsObject);
		float* knot = new float[nknots];
		for(int i = 0; i < nknots; i++)
			knot[i] = Knots_Array[i];
		float* nurbs_control_points = new float[ncpts * 3];
		for(int i = 0; i < ncpts * 3; i++)
			nurbs_control_points[i] = PtsArray[i];
		gluNurbsCurve(nurbsObject, nknots, knot, 3, nurbs_control_points, Order, GL_MAP1_VERTEX_3);
		gluEndCurve(nurbsObject);	
		gluDeleteNurbsRenderer(nurbsObject);
		delete [] knot;
		delete [] nurbs_control_points;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); 
	}
}

void RGraphicsDraw::drawLine_3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
	try
	{
		glBegin(GL_LINES);
		glVertex3d(x1, y1, z1);
		glVertex3d(x2, y2, z2);
		glEnd();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}
void RGraphicsDraw::drawLine_2D(double x1, double y1, double x2, double y2)
{
	try
	{
		glBegin(GL_LINES);
		glVertex2d(x1, y1);
		glVertex2d(x2, y2);
		glEnd();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawLine_3DStipple(double x1, double y1, double z1, double x2, double y2, double z2, GLushort Lpattern)
{
	try
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(2, Lpattern);
		glBegin(GL_LINES);
		glVertex3d(x1, y1, z1);
		glVertex3d(x2, y2, z2);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

//Draw ray / infinite line 3D...//
void RGraphicsDraw::drawXLineOrXRay3D( double* line, double w_upp, bool Infinite, bool stipple )
{
	try
	{
		double x1 = 0, y1 = 0, z1 = 0, x2 = 0, y2 = 0, z2 = 0;

		x1 = line[0];
		y1 = line[1];
		z1 = line[2];

		x2 = MAX_X * w_upp * line[3] + line[0];
		y2 = MAX_X * w_upp * line[4] + line[1];
		z2 = MAX_X * w_upp * line[5] + line[2];

		if(Infinite)
		{
			x1 = MIN_X * w_upp * line[3] + line[0];
			y1 = MIN_X * w_upp * line[4] + line[1];
			z1 = MIN_X * w_upp * line[5] + line[2];
		}
		if (!stipple)
			drawLine_3D(x1, y1, z1, x2, y2, z2);
		else
			drawLine_3DStipple(x1, y1, z1, x2, y2, z2);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0064", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawArc3D(double x, double y, double radius, double startangle, double sweepangle, bool drawStipple, double ZLevel, double *Transmatrix)
{
	try
	{
		//arc with start and sweep angle and center point
		if(drawStipple)
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(2, 0xAAAA);
		}
		if(sweepangle!=0)
		{
			double endangle = startangle+sweepangle;
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			int i = 0;
			GLdouble pts[730];
			GLdouble pts3D[730];
			unsigned int ptsorder[365];
			unsigned int ptscount = 0;
			for(double angle = startangle; angle < endangle; angle += IncrementAngle)
			{
				ptsorder[ptscount++] = ptscount;
				pts[i++] = (radius * cos(angle)) + x;
				pts[i++] = (radius * sin(angle)) + y;
				pts[i++] = ZLevel;
			}
			ptsorder[ptscount++] = ptscount;
			pts[i++] = (radius * cos(endangle)) + x;
			pts[i++] = (radius * sin(endangle)) + y;
			pts[i++] = ZLevel;

			for(i = 0; i < (int)ptscount; i++)
				RMATH2DOBJECT->MultiplyMatrix1(Transmatrix, Order1, &pts[i * 3], Order2, &pts3D[i * 3]);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_DOUBLE, 0, pts3D);
			glDrawElements(GL_LINE_STRIP, ptscount, GL_UNSIGNED_INT, ptsorder);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		if(drawStipple)
			glDisable(GL_LINE_STIPPLE);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawShapeArc3D(double* Arc3dParams, double StartAng,double SweepAng, double wupp)
{
	try
	{
		double n = Arc3dParams[5];
		double angle = 0;
		double p1 = 0, p2 = 0;

		if (abs(n) != 1)
		{
			p1 = - Arc3dParams[4] / sqrt (pow(Arc3dParams[3],2) + pow(Arc3dParams[4],2));
			p2 = Arc3dParams[3] / sqrt (pow(Arc3dParams[3],2) + pow(Arc3dParams[4],2));	
		}
		else
		{
			p1 = -1;
			p2 = 0;
		}

		angle = acos (n);
		angle = - angle;
		glPushMatrix();
		translateMatrix(Arc3dParams[0],Arc3dParams[1],Arc3dParams[2]);
		glRotatef((GLfloat) 180.0 * angle / M_PI, p1, p2, 0);
		drawArc(0, 0, Arc3dParams[6], StartAng, SweepAng, wupp);	
		glPopMatrix();
	}
	catch(...){SetAndRaiseErrorMessage("GRDLL0061", __FILE__, __FUNCSIG__);}
}

void RGraphicsDraw::drawCircle3D(double* Circle3dParams)
{
	try
	{
		double n = Circle3dParams[5];
		double angle = 0;

		//vector on x,y plane that is axis of rotation to move from xy plane to circle plane
		//double p1, p2;

		//if (abs(n) != 1)
		//{
		//	p1 = - Circle3dParams[4] / sqrt (pow(Circle3dParams[3],2) + pow(Circle3dParams[4],2));
		//	p2 = Circle3dParams[3] / sqrt (pow(Circle3dParams[3],2) + pow(Circle3dParams[4],2));	
		//}
		//else
		//{
		//	//this could be any vector on xy plane because angle is either 0 or PI.
		//	p1 = -1;
		//	p2 = 0;
		//}

		//angle of rotation of plane wrt (p1, p2, 0) axis
	/*	angle = acos (n);
		angle = - angle;
*/
		glPushMatrix();

		
		double CirclePlane[4]={Circle3dParams[3], Circle3dParams[4], Circle3dParams[5], Circle3dParams[3] * Circle3dParams[0] + Circle3dParams[4] * Circle3dParams[1] + Circle3dParams[5] * Circle3dParams[2]};
		double TransMatrix[9];
		RMATH3DOBJECT->GetTMatrixForPlane(CirclePlane, TransMatrix);
		//double param[16]={Circle3dParams[3], Circle3dParams[4], Circle3dParams[5], 0, Circle3dParams[3], Circle3dParams[4], Circle3dParams[5],0,Circle3dParams[3], Circle3dParams[4], Circle3dParams[5], 0, 0, 0, 0, 1};
		double param[16]={TransMatrix[0], TransMatrix[1], TransMatrix[2], 0, TransMatrix[3], TransMatrix[4], TransMatrix[5], 0, TransMatrix[6], TransMatrix[7], TransMatrix[8], 0, 0, 0, 0, 1};
		//glGetDoublev(GL_MODELVIEW_MATRIX, param);	
		//glLoadIdentity();
		translateMatrix(Circle3dParams[0], Circle3dParams[1], Circle3dParams[2]);
		//glRotatef((GLfloat) 180.0 * angle / M_PI, p1, p2, 0);
		glMultMatrixd(param);
		drawCircle(0, 0, Circle3dParams[6]);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0061", __FILE__, __FUNCSIG__); }
}
//draw conic in 3D
void RGraphicsDraw::drawConic3D(double* ConicParams)
{
	try
	{
		//first 4 are conic plane coordinates
		//next 3 are conic focus coordinates
		//next 6 are diretrix line with first 3 being pt on diretrix closest to focus and next 3 being dir cosines of diretrix line
		//next parameter (13) is ecccentricity, 14 and 15 are start and end angles and 16 is distance from focus to diretrix
		double l = ConicParams[0];
		double m = ConicParams[1];
		double n = ConicParams[2];
		double angle = 0;

		//vector on x,y plane that is axis of rotation to move from xy plane to conic plane
		double p1 = 0, p2 = 0;

		if (abs(n) != 1)
		{
			p1 = - ConicParams[1] / sqrt (pow(ConicParams[0],2) + pow(ConicParams[1],2));
			p2 = ConicParams[0] / sqrt (pow(ConicParams[0],2) + pow(ConicParams[1],2));		
		}
		else
		{
			//this could be any vector on xy plane because angle is either 0 or PI.
			p1 = -1;
			p2 = 0;
		}

		//mutually perpendicular unit vectors on conic plane
		double l2, m2, n2, l3, m3, n3;

		//l2, m2, n2 are direction cosines of diretrix (the y-axis of conic plane)
		l2 = ConicParams[10];
		m2 = ConicParams[11];
		n2 = ConicParams[12];

		//l3, m3, n3 are mutually perpendicular to l, m, n and l2, m2, n2 (the x-axis of conic plane)
		l3 = -n * m2 + m * n2;
		m3 = -l * n2 + n * l2;
		n3 = -m * l2 + l * m2;

		double e, fd, startAng, endAng;

		//angle of rotation of plane wrt (p1, p2, 0) axis
		angle = acos (n);

		//y-axis after rotation by above angle would be (p1*p2*(1-n), p2*p2*(1-n) + n, p1*sin(angle))
		double ly = p1 * p2 * (1 - n);
		double my = p2 * p2 * (1 - n) + n;
		double ny = p1 * sin (angle);

		double y_projn_diretrix = l2 * ly + m2 * my + n2 * ny;
		double y_projn_diretrix_perpendicular = l3 * ly + m3 * my + n3 * ny;

		//phi is angle between diretrix and rotated y axis
		double phi = acos (y_projn_diretrix);
		if (y_projn_diretrix_perpendicular < 0)
			phi = 2 * M_PI - phi;
		phi = - phi;
		angle = - angle;
		//populate e, fd, startAng and endAng based on conic params.
		e = ConicParams[13];
		fd = ConicParams[16];
		startAng = ConicParams[14];
		endAng = ConicParams[15];

		glPushMatrix();
		//translate by focus coordinates because above conic is with focus at origin.
		//double param[16];
		//glGetDoublev(GL_MODELVIEW_MATRIX, param);	
		//glLoadIdentity();
		translateMatrix(ConicParams[4],ConicParams[5],ConicParams[6]);
		glRotatef((GLfloat) 180.0 * angle / M_PI, p1, p2, 0);
		glRotatef((GLfloat) 180.0 * phi / M_PI, 0, 0, 1);
		//glMultMatrixd(param);	
		drawConic(e, fd, startAng, endAng);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0062", __FILE__, __FUNCSIG__); }
}
//draw conic in 2d
void RGraphicsDraw::drawConic(double e, double fd, double startAng, double endAng)
{
	try
	{
		int i = 0;
		GLdouble pts[730] = {0};
		unsigned int ptsorder[365] = {0};
		unsigned int ptscount = 0;
		double radius;
		for(double angle = startAng; angle <= endAng; angle += IncrementAngle)
		{
			radius = fd / (1/e - cos (angle));
			ptsorder[ptscount++] = ptscount;
			pts[i++] = (radius * cos(angle));
			pts[i++] = (radius * sin(angle));
		}
		if (endAng == 2 * M_PI)
			ptsorder[ptscount] = 0;
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, pts);
		glDrawElements(GL_LINE_STRIP, ptscount, GL_UNSIGNED_INT, ptsorder);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0063", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawRectangle3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, bool fill)
{
	try
	{
		GLdouble mix[]={x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, mix);
		unsigned int indices[] = {0, 1, 2, 3, 0};
		if (fill)glDrawElements(GL_POLYGON, 5, GL_UNSIGNED_INT, indices);
		else glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawConeFor3DAxis(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius,double Bradius)
{
	try
	{
		double vx = x2 - x1, vy = y2 - y1, vz = z2 - z1;
		//handle the degenerate case with an approximation//
		if(vz == 0)
			vz = 0.00000001;
		double v = sqrt( vx * vx + vy * vy + vz * vz );
		double ax = 57.2957795*acos( vz/v ); 
		if(vz < 0.0) ax = -ax;
		double rx = -vy * vz, ry = vx * vz;
		GLUquadricObj *quadric=gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		glPushMatrix();
		glTranslatef(x1,y1,z1);
		glRotatef(ax, rx, ry, 0.0);
		//draw the cylinder
		gluCylinder(quadric, Tradius, Bradius, v, 40, 1);
		glTranslatef(0, 0, v);
		glPopMatrix();
		gluDeleteQuadric(quadric);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::EnableLighting()
{
	try
	{
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		double light_pos1[] = {Light_Position1[0], Light_Position1[1], Light_Position1[2], Light_Position1[3]};
		int size1[2] = {4, 4}, size2[2] = {4, 1};
		double valb1[4] = {0};
		RMATH2DOBJECT->MultiplyMatrix1(MMatrix, &size1[0], &light_pos1[0], &size2[0], &valb1[0]);
			
		GLfloat light_position1[] = {valb1[0], valb1[1], valb1[2], valb1[3]};
		GLfloat mat_diffuse[] = {Material_Diffuse[0], Material_Diffuse[1], Material_Diffuse[2], Material_Diffuse[3]};
		glShadeModel(GL_SMOOTH);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::EnableLighting(double PosX, double PosY, double PosZ, double l, double m, double n)
{
	try
	{
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		double light_pos1[] = {PosX + Light_Position1[0] * l, PosY + Light_Position1[1] * m, PosZ + Light_Position1[2] * n, 0.0};
		double light_pos2[] = {PosX - Light_Position1[0] * l, PosY - 2 * Light_Position1[1] * m, PosZ - 2 * Light_Position1[2] * n, 0.0};
		int size1[2] = {4, 4}, size2[2] = {4, 1};
		double valb1[4] = {0}, valb2[4] = {0};
		RMATH2DOBJECT->MultiplyMatrix1(MMatrix, &size1[0], &light_pos1[0], &size2[0], &valb1[0]);
		RMATH2DOBJECT->MultiplyMatrix1(MMatrix, &size1[0], &light_pos2[0], &size2[0], &valb2[0]);
			
		//GLfloat light_position1[] = {light_pos1[0], light_pos1[1], light_pos1[2], light_pos1[3]};
		//GLfloat light_position2[] = {light_pos2[0], light_pos2[1], light_pos2[2], light_pos2[3]};
		GLfloat light_position1[] = {valb1[0], valb1[1], valb1[2], valb1[3]};
		GLfloat light_position2[] = {valb2[0], valb2[1], valb2[2], valb2[3]};
		GLfloat mat_diffuse[] = {Material_Diffuse[0], Material_Diffuse[1], Material_Diffuse[2], Material_Diffuse[3]};
		glShadeModel(GL_SMOOTH);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
		glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, mat_diffuse);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::DisableLighting()
{
	try
	{ 
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::EnableTransparency()
{
	try
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::DisableTransparency()
{
	try
	{
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::draw_Sphere(double x, double y, double z, double radius, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{

	/*	glBegin(GL_POINTS);
	
	for(int p = 0; p < 10000; p++) 
	{
		glColor3f(rf(),rf(),rf());
		glVertex3f(2*rf()-1,2*rf()-1,2*rf()-1);
	}
	glEnd();*/

	//glPointSize(5.0);
	//glBegin(GL_POINTS);
	//glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	//for(int i = 0; i < 5; i++) 
	//{
	//	for(int j = 0; j < 5; j++) 
	//	{
	//		for(int k = 0; k < 5; k++) 
	//		{
	//			//glVertex3f(i, j, k);
	//			GLUquadricObj *quadric = gluNewQuadric();
	//			gluQuadricNormals(quadric, GLU_SMOOTH);
	//			gluQuadricDrawStyle(quadric, GLU_LINE);
	//			glPushMatrix();
	//			glTranslatef((GLfloat)i, (GLfloat)j, (GLfloat)k);
	//			gluSphere(quadric, 1, 40, 40);
	//			glPopMatrix();
	//			gluDeleteQuadric(quadric);
	//		}

	//	}
	//}
	//glTranslatef(-(GLfloat)x, -(GLfloat)y, -(GLfloat)z);

	//glEnd();

//		GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
//	   GLfloat ctlpoints[4][4][3];
//	  GLfloat ctrlpoints[4][4][3] = {
//   {{ -1.5, -1.5, 4.0}, { -0.5, -1.5, 2.0}, 
//    {0.5, -1.5, -1.0}, {1.5, -1.5, 2.0}}, 
//   {{ -1.5, -0.5, 1.0}, { -0.5, -0.5, 3.0}, 
//    {0.5, -0.5, 0.0}, {1.5, -0.5, -1.0}}, 
//   {{ -1.5, 0.5, 4.0}, { -0.5, 0.5, 0.0}, 
//    {0.5, 0.5, 3.0}, {1.5, 0.5, 4.0}}, 
//   {{ -1.5, 1.5, -2.0}, { -0.5, 1.5, -2.0}, 
//    {0.5, 1.5, 0.0}, {1.5, 1.5, -1.0}}
//};
//
//	   int u, v;
//   for (u = 0; u < 4; u++) {
//      for (v = 0; v < 4; v++) {
//         ctlpoints[u][v][0] = 3.0*((GLfloat)u - 1.5);
//         ctlpoints[u][v][1] = 3.0*((GLfloat)v - 1.5);
//
//         if ( (u == 1 || u == 2) && (v == 1 || v == 2))
//            ctlpoints[u][v][2] = 2.0;
//         else
//            ctlpoints[u][v][2] = -2.0;
//      }
//   } 
//	   glPushMatrix();
//	   //glRotatef(100.0, 1.,0.,0.);
//	  // glScalef (0.5, 0.5, 0.5);
//	   GLUnurbsObj* theNurb = gluNewNurbsRenderer();
//	   gluBeginSurface(theNurb);
//	   gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
//	   gluNurbsSurface(theNurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0],  4, 4, GL_MAP2_VERTEX_3);
//	   gluEndSurface(theNurb);
//
//	  
//		  glPointSize(5.0);
//		 // glDisable(GL_LIGHTING);
//		  glColor3f(1.0, 1.0, 0.0);
//		  glBegin(GL_POINTS);
//		  for (int i = 0; i < 4; i++) {
//			 for (int j = 0; j < 4; j++) {
//				glVertex3f(ctlpoints[i][j][0], 
//						   ctlpoints[i][j][1], ctlpoints[i][j][2]);
//			 }
//		  }
//		  glEnd();
//		  
//	   glPopMatrix();
	  

		GLuint noofslices = 3 / wupp;
		if(noofslices < 40) noofslices = 40;
		if(noofslices > 120) noofslices = 120;
		//noofslices = 40;
		if(noofslices < 40) noofslices = 40;
		if(AllowTransparency)
			EnableTransparency();
		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		if(Wiredframe)
			gluQuadricDrawStyle(quadric, GLU_LINE);
		else
			gluQuadricDrawStyle(quadric, GLU_FILL);
		glPushMatrix();
		glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
		gluSphere(quadric, radius, noofslices, noofslices);
		glPopMatrix();
		gluDeleteQuadric(quadric);
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::draw_SpherePart(double x, double y, double z, double radius, double MinZPt, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{	
		if(AllowTransparency)
			EnableTransparency();
		//Calculate circles acc. to min Z level..
		double TopZPt = z + radius;
		double ZDiff = TopZPt - MinZPt, Zincrement = 10 * wupp;
		int NofSlices = ZDiff / Zincrement;
		glPushMatrix();
		for(int i = 1; i < NofSlices - 1; i++)
		{
			int PtCnt1 = 0, PtCnt2 = 0;
			GLdouble pts1[1100], pts2[1100];
			double z1val = MinZPt + Zincrement * i, z2val = MinZPt + Zincrement * (i + 1);
			double z1valdiff = z1val - z, z2valdiff = z2val - z;
			double radius1 = sqrt((radius * radius) - (z1valdiff * z1valdiff));
			double radius2 = sqrt((radius * radius) - (z2valdiff * z2valdiff));
			if(i + 1 == NofSlices)
			{
				/*for(double angle = 0; angle < M_PI * 2; angle += IncrementAngle)
				{
					pts1[PtCnt1++] = (radius1 * cos(angle)) + x;
					pts1[PtCnt1++] = (radius1 * sin(angle)) + y;
					pts1[PtCnt1++] = z1val;
				}
				for(int i = 0; i < PtCnt1/3; i++)
				{
					if(Wiredframe)
						glBegin(GL_LINE_LOOP);
					else
						glBegin(GL_POLYGON);
					glVertex3d(x, y, TopZPt);
					glVertex3d(pts1[3 * i], pts1[3 * i + 1], pts1[3 * i + 2]);
					glVertex3d(pts1[3 * i + 3], pts1[3 * i + 4], pts1[3 * i + 5]);
					glVertex3d(x, y, TopZPt);
					glEnd();
				}*/
			}
			else
			{
				for(double angle = 0; angle < M_PI * 2; angle += IncrementAngle)
				{
					pts1[PtCnt1++] = (radius1 * cos(angle)) + x;
					pts1[PtCnt1++] = (radius1 * sin(angle)) + y;
					pts1[PtCnt1++] = z1val;
					pts2[PtCnt2++] = (radius2 * cos(angle)) + x;
					pts2[PtCnt2++] = (radius2 * sin(angle)) + y;
					pts2[PtCnt2++] = z2val;
				}
				glBegin(GL_LINE_LOOP);
				for(int i = 0; i < PtCnt1/3; i++)
				{
					glNormal3d(pts1[3 * i], pts1[3 * i + 1], pts1[3 * i + 2]);
					glVertex3d(pts1[3 * i], pts1[3 * i + 1], pts1[3 * i + 2]);
				}
				glEnd();
				/*for(int i = 0; i < PtCnt1/3; i++)
				{
					if(Wiredframe)
						glBegin(GL_LINE_LOOP);
					else
						glBegin(GL_POLYGON);	
					glVertex3d(pts1[3 * i], pts1[3 * i + 1], pts1[3 * i + 2]);
					glVertex3d(pts1[3 * i + 3], pts1[3 * i + 4], pts1[3 * i + 5]);
					glVertex3d(pts2[3 * i + 3], pts2[3 * i + 4], pts2[3 * i + 5]);
					glVertex3d(pts2[3 * i], pts2[3 * i + 1], pts2[3 * i + 2]);
					glVertex3d(pts1[3 * i], pts1[3 * i + 1], pts1[3 * i + 2]);
					glEnd();
				}*/
			}
			
		}
		glPopMatrix();
		if(AllowTransparency)
			DisableTransparency();
		
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::draw_SpherePartial(double x, double y, double z, double radius, double theta_min, double theta_max, double phi_min, double phi_max, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{	
		if(AllowTransparency)
			EnableTransparency();
		int noofslices = 3 / wupp;
		if(noofslices < 40) noofslices = 40;
		if(noofslices > 120) noofslices = 120;
		double minzpt = radius * cos (theta_max);
		double maxzpt = radius * cos (theta_min);
		double ZDiff = maxzpt - minzpt;
		double Zincrement = ZDiff / noofslices;
		glPushMatrix();
		glTranslatef(x, y, z);
		for(int i = 0; i < noofslices; i++)
		{
			int PtCnt1 = 0, PtCnt2 = 0;
			GLdouble pts1[1100], pts2[1100];
			double z1val = minzpt + Zincrement * i, z2val = minzpt + Zincrement * (i + 1);
			double radius1 = sqrt((radius * radius) - (z1val * z1val));
			double radius2 = sqrt((radius * radius) - (z2val * z2val));
			if ((theta_min == 0) && (i == noofslices - 1))
			{
				radius2 = 0;
				z2val = radius;
			}
			for(double angle = phi_min; angle < phi_max; angle += IncrementAngle)
			{
				pts1[PtCnt1++] = radius1 * cos(angle);
				pts1[PtCnt1++] = radius1 * sin(angle);
				pts1[PtCnt1++] = z1val;
				pts2[PtCnt2++] = radius2 * cos(angle);
				pts2[PtCnt2++] = radius2 * sin(angle);
				pts2[PtCnt2++] = z2val;
			}
			for(int j = 0; j < PtCnt1/3 - 1; j++)
			{
				if(Wiredframe)
					glBegin(GL_LINE_LOOP);
				else
					glBegin(GL_POLYGON);	
				glNormal3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glNormal3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
				glVertex3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
				glNormal3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
				glVertex3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
			//	glNormal3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
				glVertex3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
			//	glNormal3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glEnd();
			}
			if (phi_max - phi_min == 2 * M_PI)
			{
				if(Wiredframe)
					glBegin(GL_LINE_LOOP);
				else
					glBegin(GL_POLYGON);	

				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);
				glVertex3d(pts1[0], pts1[1], pts1[2]);
				glVertex3d(pts2[0], pts2[1], pts2[2]);
				glVertex3d(pts2[PtCnt1 - 3], pts2[PtCnt1 - 2], pts2[PtCnt1 - 1]);
				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);

				glEnd();
			}
		}
		glPopMatrix();
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127a", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::draw_SphereFromPts(double x, double y, double z, double radius, double* pts, int ptscount, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{	
		double tmppt[3] = {0};
		if(AllowTransparency)
			EnableTransparency();
		int noofslices = 40;

		double minzpt = -radius;
		double maxzpt = radius;
		double ZDiff = maxzpt - minzpt;
		double Zincrement = ZDiff / noofslices;

		glPushMatrix();
		glTranslatef(x, y, z);
		for(int i = 0; i < noofslices; i++)
		{
			int PtCnt1 = 0, PtCnt2 = 0;
			GLdouble pts1[1100], pts2[1100];
			double z1val = minzpt + Zincrement * i, z2val = minzpt + Zincrement * (i + 1);
			double radius1 = sqrt((radius * radius) - (z1val * z1val));
			double radius2 = sqrt((radius * radius) - (z2val * z2val));
			if (i == noofslices - 1)
			{
				radius2 = 0;
				z2val = radius;
			}
			for(double angle = 0; angle < 2 * M_PI; angle += 5 * IncrementAngle)
			{
				pts1[PtCnt1++] = radius1 * cos(angle);
				pts1[PtCnt1++] = radius1 * sin(angle);
				pts1[PtCnt1++] = z1val;
				pts2[PtCnt2++] = radius2 * cos(angle);
				pts2[PtCnt2++] = radius2 * sin(angle);
				pts2[PtCnt2++] = z2val;
			}
			bool drawpolygon = false;
			for(int j = 0; j < PtCnt1/3 - 1; j++)
			{
				drawpolygon = false;
				for (int k = 0; k < ptscount; k++)
				{
					tmppt[0] = *(pts + 3*k + 0) - x;
					tmppt[1] = *(pts + 3*k + 1) - y;
					tmppt[2] = *(pts + 3*k + 2) - z;
					if (((abs(tmppt[0] - pts1[3*j]) < Zincrement)
						&& (abs(tmppt[1] - pts1[3*j + 1]) < Zincrement)
						&& (abs(tmppt[2] - pts1[3*j + 2]) < Zincrement)) ||
						((abs(tmppt[0] - pts2[3*j + 3]) < Zincrement)
						&& (abs(tmppt[1] - pts2[3*j + 4]) < Zincrement)
						&& (abs(tmppt[2] - pts2[3*j + 5]) < Zincrement)))
					{
						drawpolygon = true;
						break;
					}
				}
				if (drawpolygon)
				{
					if(Wiredframe)
						glBegin(GL_LINE_LOOP);
					else
						glBegin(GL_POLYGON);	

					glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
					glVertex3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
					glVertex3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
					glVertex3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
					glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);

					glEnd();
				}
			}
			drawpolygon = false;
			for (int k = 0; k < ptscount; k++)
			{
				tmppt[0] = *(pts + 3*k + 0) - x;
				tmppt[1] = *(pts + 3*k + 1) - y;
				tmppt[2] = *(pts + 3*k + 2) - z;
				if (((abs(tmppt[0] - pts1[0]) < Zincrement)
					&& (abs(tmppt[1] - pts1[1]) < Zincrement)
					&& (abs(tmppt[2] - pts1[2]) < Zincrement)) ||
					((abs(tmppt[0] - pts2[PtCnt1 - 3]) < Zincrement)
					&& (abs(tmppt[1] - pts2[PtCnt1 - 2]) < Zincrement)
					&& (abs(tmppt[2] - pts2[PtCnt1 - 1]) < Zincrement)))

				{
					drawpolygon = true;
					break;
				}
			}
			if (drawpolygon)
			{
				if(Wiredframe)
					glBegin(GL_LINE_LOOP);
				else
					glBegin(GL_POLYGON);	

				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);
				glVertex3d(pts1[0], pts1[1], pts1[2]);
				glVertex3d(pts2[0], pts2[1], pts2[2]);
				glVertex3d(pts2[PtCnt1 - 3], pts2[PtCnt1 - 2], pts2[PtCnt1 - 1]);
				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);

				glEnd();
			}
		}
		glPopMatrix();
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127b", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::draw_CylinderPartial(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius, double Bradius, double phi_min, double phi_max, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{
		int noofslices = 3 / wupp;
		if(noofslices < 40) noofslices = 40;
		if(noofslices > 120) noofslices = 120;
		double vx = x2 - x1, vy = y2 - y1, vz = z2 - z1;
		double v = sqrt(vx * vx + vy * vy + vz * vz);
		double ax = 57.2957795 * acos(vz/v); 
		double rx = -vy , ry = vx ;
		double norm = sqrt (vx * vx + vy * vy);
		if (norm != 0)
		{
			rx /= norm; 
			ry /= norm;
		}
		else
		{
			rx = 1;
			ry = 0;
		}
		if(AllowTransparency)
			EnableTransparency();
			
		glPushMatrix();
		glTranslatef(x1, y1, z1);
		glRotatef(ax, rx, ry, 0.0);
		//gluCylinder(quadric, Tradius, Bradius, v, noofslices, 1);
		//replace above call by instead drawing the polygons comprising the partial cylinder between phi_min and phi_max.
		double minzpt = 0;
		double maxzpt = v;
		double ZDiff = maxzpt - minzpt;
		double Zincrement = ZDiff / noofslices;
		for(int i = 0; i < noofslices; i++)
		{
			int PtCnt1 = 0, PtCnt2 = 0;
			GLdouble pts1[1100], pts2[1100];
			double z1val = minzpt + Zincrement * i, z2val = minzpt + Zincrement * (i + 1);
			double radius1 = Tradius + (z1val / v) * (Bradius - Tradius);
			double radius2 = Tradius + (z2val / v) * (Bradius - Tradius);

			for(double angle = phi_min; angle < phi_max; angle += IncrementAngle)
			{
				pts1[PtCnt1++] = radius1 * cos(angle);
				pts1[PtCnt1++] = radius1 * sin(angle);
				pts1[PtCnt1++] = z1val;
				pts2[PtCnt2++] = radius2 * cos(angle);
				pts2[PtCnt2++] = radius2 * sin(angle);
				pts2[PtCnt2++] = z2val;
			}
			for(int j = 0; j < PtCnt1/3 - 1; j++)
			{
				if(Wiredframe)
					glBegin(GL_LINE_LOOP);
				else
					glBegin(GL_POLYGON);	
				glNormal3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glNormal3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
				glVertex3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
				glNormal3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
				glVertex3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
			//	glNormal3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
				glVertex3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
			//	glNormal3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);

				glEnd();
			}
			if (phi_max - phi_min == 2 * M_PI)
			{
				if(Wiredframe)
					glBegin(GL_LINE_LOOP);
				else
					glBegin(GL_POLYGON);	
				glNormal3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);
				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);
				glNormal3d(pts1[0], pts1[1], pts1[2]);
				glVertex3d(pts1[0], pts1[1], pts1[2]);
				glNormal3d(pts2[0], pts2[1], pts2[2]);
				glVertex3d(pts2[0], pts2[1], pts2[2]);
				glNormal3d(pts2[PtCnt1 - 3], pts2[PtCnt1 - 2], pts2[PtCnt1 - 1]);
				glVertex3d(pts2[PtCnt1 - 3], pts2[PtCnt1 - 2], pts2[PtCnt1 - 1]);
				glNormal3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);
				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);

				glEnd();
			}
		}		
		glPopMatrix();
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127a", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_CylinderFromPts(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius, double Bradius, double* pts, int ptscount, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{
		double tmppt[3] = {0};
		double c1 = 0, c2 = 0, c3 = 0;
		int noofslices = 40;
		double vx = x2 - x1, vy = y2 - y1, vz = z2 - z1;
		double v = sqrt(vx * vx + vy * vy + vz * vz);
		double ax = 57.2957795 * acos(vz/v); 
		double cosx = vz/v;
		double sinx = sqrt (1 - cosx * cosx);
		double rx = -vy , ry = vx ;
		double norm = sqrt (vx * vx + vy * vy);
		if (norm != 0)
		{
			rx /= norm; 
			ry /= norm;
		}
		else
		{
			rx = 1;
			ry = 0;
		}
		if(AllowTransparency)
			EnableTransparency();
			
		glPushMatrix();
		glTranslatef(x1, y1, z1);
		glRotatef(ax, rx, ry, 0.0);
		//gluCylinder(quadric, Tradius, Bradius, v, noofslices, 1);
		//replace above call by instead drawing the polygons comprising the partial cylinder between phi_min and phi_max.
		double minzpt = 0;
		double maxzpt = v;
		double ZDiff = maxzpt - minzpt;
		double Zincrement = ZDiff / noofslices;
		for(int i = 0; i < noofslices; i++)
		{
			int PtCnt1 = 0, PtCnt2 = 0;
			GLdouble pts1[1100], pts2[1100];
			double z1val = minzpt + Zincrement * i, z2val = minzpt + Zincrement * (i + 1);
			double radius1 = Tradius + (z1val / v) * (Bradius - Tradius);
			double radius2 = Tradius + (z2val / v) * (Bradius - Tradius);

			for(double angle = 0; angle < 2 * M_PI; angle += 5 * IncrementAngle)
			{
				pts1[PtCnt1++] = radius1 * cos(angle);
				pts1[PtCnt1++] = radius1 * sin(angle);
				pts1[PtCnt1++] = z1val;
				pts2[PtCnt2++] = radius2 * cos(angle);
				pts2[PtCnt2++] = radius2 * sin(angle);
				pts2[PtCnt2++] = z2val;
			}
			bool drawpolygon = false;
			for(int j = 0; j < PtCnt1/3 - 1; j++)
			{
				drawpolygon = false;
				for (int k = 0; k < ptscount; k++)
				{
					tmppt[0] = *(pts + 3*k + 0) - x1;
					tmppt[1] = *(pts + 3*k + 1) - y1;
					tmppt[2] = *(pts + 3*k + 2) - z1;
					c1 = tmppt[0] * rx + tmppt[1] * ry;
					c2 = tmppt[0] * ry - tmppt[1] * rx;
					c3 = tmppt[2] * cosx + c2 * sinx;
					c2 = c2 * cosx - tmppt[2] * sinx;
					tmppt[0] = c1 * rx + c2 * ry;
					tmppt[1] = c1 * ry - c2 * rx;
					tmppt[2] = c3;

					if (((abs(tmppt[0] - pts1[3*j]) < Zincrement)
						&& (abs(tmppt[1] - pts1[3*j + 1]) < Zincrement)
						&& (abs(tmppt[2] - pts1[3*j + 2]) < Zincrement)) ||
						((abs(tmppt[0] - pts2[3*j + 3]) < Zincrement)
						&& (abs(tmppt[1] - pts2[3*j + 4]) < Zincrement)
						&& (abs(tmppt[2] - pts2[3*j + 5]) < Zincrement)))
					{
						drawpolygon = true;
						break;
					}
				}
				if (drawpolygon)
				{
					if(Wiredframe)
						glBegin(GL_LINE_LOOP);
					else
						glBegin(GL_POLYGON);	
					glNormal3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
					glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
					glNormal3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
					glVertex3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
					glNormal3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
					glVertex3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
					glVertex3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
					glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);

					glEnd();
				}
			}
			drawpolygon = false;
			for (int k = 0; k < ptscount; k++)
			{
				tmppt[0] = *(pts + 3*k + 0) - x1;
				tmppt[1] = *(pts + 3*k + 1) - y1;
				tmppt[2] = *(pts + 3*k + 2) - z1;
				c1 = tmppt[0] * rx + tmppt[1] * ry;
				c2 = tmppt[0] * ry - tmppt[1] * rx;
				c3 = tmppt[2] * cosx + c2 * sinx;
				c2 = c2 * cosx - tmppt[2] * sinx;
				tmppt[0] = c1 * rx + c2 * ry;
				tmppt[1] = c1 * ry - c2 * rx;
				tmppt[2] = c3;

				if (((abs(tmppt[0] - pts1[0]) < Zincrement)
					&& (abs(tmppt[1] - pts1[1]) < Zincrement)
					&& (abs(tmppt[2] - pts1[2]) < Zincrement)) ||
					((abs(tmppt[0] - pts2[PtCnt1 - 3]) < Zincrement)
					&& (abs(tmppt[1] - pts2[PtCnt1 - 2]) < Zincrement)
					&& (abs(tmppt[2] - pts2[PtCnt1 - 1]) < Zincrement)))
				{
					drawpolygon = true;
					break;
				}
			}
			if (drawpolygon)
			{
				if(Wiredframe)
					glBegin(GL_LINE_LOOP);
				else
					glBegin(GL_POLYGON);	

				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);
				glVertex3d(pts1[0], pts1[1], pts1[2]);
				glVertex3d(pts2[0], pts2[1], pts2[2]);
				glVertex3d(pts2[PtCnt1 - 3], pts2[PtCnt1 - 2], pts2[PtCnt1 - 1]);
				glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);

				glEnd();
			}
		}
		glPopMatrix();
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127b", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_Cylinder(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius, double Bradius, bool Wiredframe, bool AllowTransparency, bool drawdisk, double wupp)
{
	try
	{
		GLuint noofslices = 3 / wupp;
		if(noofslices < 40) noofslices = 40;
		if(noofslices > 120) noofslices = 120;
		//noofslices = 40;
		double vx = x2 - x1, vy = y2 - y1, vz = z2 - z1;
		//handle the degenerate case with an approximation//
		if(vz == 0)
			vz = 0.00000001;
		double v = sqrt(vx * vx + vy * vy + vz * vz);
		double ax = 57.2957795 * acos(vz/v); 
		if(vz < 0.0) ax = -ax;
		double rx = -vy * vz, ry = vx * vz;
		if(AllowTransparency)
			EnableTransparency();
		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		if(Wiredframe)
			gluQuadricDrawStyle(quadric, GLU_LINE);
		else
			gluQuadricDrawStyle(quadric, GLU_FILL);
			
		glPushMatrix();
		glTranslatef(x1, y1, z1);
		glRotatef(ax, rx, ry, 0.0);
		gluCylinder(quadric, Tradius, Bradius, v, noofslices, 1);
		if(drawdisk)
		{
			gluQuadricOrientation(quadric, GLU_INSIDE);
			gluDisk(quadric, 0.0, Tradius, noofslices, 1);
			glTranslatef(0, 0, v);
			gluQuadricOrientation(quadric, GLU_OUTSIDE);
			gluDisk(quadric, 0.0, Bradius, noofslices, 1);
		}
		glPopMatrix();
		gluDeleteQuadric(quadric);
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_Circledisk(double x1, double y1, double z1, double radius, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{
		GLuint noofslices = 3 / wupp;
		if(noofslices < 40) noofslices = 40;
		if(noofslices > 120) noofslices = 120;
		//noofslices = 40;
		if(AllowTransparency)
			EnableTransparency();
		GLUquadricObj *quadric = gluNewQuadric();
		if(Wiredframe)
			gluQuadricDrawStyle(quadric, GLU_LINE);
		else
			gluQuadricNormals(quadric, GLU_SMOOTH);
		glPushMatrix();
		glTranslatef(x1, y1, z1);
		gluQuadricOrientation(quadric, GLU_INSIDE);
		gluDisk( quadric, 0, radius, noofslices, 1);
		glPopMatrix();
		gluDeleteQuadric(quadric);
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_Plane(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, bool AllowTransparency, bool Wiredframe)
{
	try
	{
		glPushMatrix();
		if(AllowTransparency)
			EnableTransparency();
		if(Wiredframe)
			glBegin(GL_LINE_LOOP);
		else
			glBegin(GL_POLYGON);	
		glVertex3d(x1, y1, z1);
		glVertex3d(x2, y2, z2);
		glVertex3d(x3, y3, z3);
		glVertex3d(x4, y4, z4);
		glVertex3d(x1, y1, z1);
		glEnd();
		if(AllowTransparency)
			DisableTransparency();
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_TriangleWithColour(double* TrianglePts, double MinZ, double MaxZ, bool AllowTransparency, bool Wiredframe, bool WithoutLight, bool DiscreteColor, double tolerance)
{
	try
	{
		glPushMatrix();
		if(AllowTransparency)
			EnableTransparency();
		if(Wiredframe)
		{
			glBegin(GL_LINE_LOOP);
			for(int i = 0; i < 3; i++)
				glVertex3d(TrianglePts[3 * i], TrianglePts[3 * i + 1], TrianglePts[3 * i + 2]);
		}
		else
		{
			double Z1 = tolerance;
			if(tolerance == 0)
				Z1 = (MaxZ - MinZ) / 6;
			double Z2 = 2 * Z1, Z3 = 3 * Z1, Z4 = 4 * Z1, Z5 = 5 * Z1;
			glShadeModel(GL_SMOOTH);
			glBegin(GL_TRIANGLES);
			if(WithoutLight)
			{	
				if(DiscreteColor)
				{
					for(int i = 0; i < 3; i++)
					{
						double colorFactor = (TrianglePts[3 * i + 2] - MinZ);
						if(colorFactor <= Z1)
							glColor3d(0, 0, 1);
						else if(colorFactor <= Z2)
							glColor3d(0, 0.25, 0.75);
						else if(colorFactor <= Z3)
							glColor3d(0, 0.75, 0.25);
						else if(colorFactor <= Z4)
							glColor3d(0.25, 0.75, 0);
						else if(colorFactor <= Z5)
							glColor3d(0.75, 0.25, 0);
						else
							glColor3d(1, 0, 0);
						glVertex3d(TrianglePts[3 * i], TrianglePts[3 * i + 1], TrianglePts[3 * i + 2]);
					}
				}
				else
				{
					for(int i = 0; i < 3; i++)
					{
						double colorFactor = TrianglePts[3 * i + 2] - MinZ;
						if(colorFactor <= Z1)
							glColor3d(0, 0, 1);
						else if(colorFactor <= Z2)
							glColor3d(0, ((colorFactor - Z1) / Z1) * 0.25, (1 - 0.25) * ((colorFactor - Z1) / Z1));
						else if(colorFactor <= Z3)
							glColor3d(0, 0.25 + ((colorFactor - Z2) / Z1) * 0.5, 0.75 - (1 - 0.5) * ((colorFactor - Z2) / Z1));
						else if(colorFactor <= Z4)
							glColor3d(((colorFactor - Z3) / Z1) * 0.25, 0.75, 0.25 - (1 - 07.5) * ((colorFactor - Z3) / Z1));
						else if(colorFactor <= Z5)
							glColor3d(0.25 + ((colorFactor - Z4) / Z1) * 0.5, 0.75 - (1 - 0.5) * ((colorFactor - Z4) / Z1), 0);
						else
							glColor3d(1, 0, 0);
						glVertex3d(TrianglePts[3 * i], TrianglePts[3 * i + 1], TrianglePts[3 * i + 2]);
					}
				}
			}
			else
			{
				if(DiscreteColor)
				{
					double colorFactor = ((TrianglePts[2] + TrianglePts[5] + TrianglePts[8]) / 3 - MinZ);
					if(colorFactor <= Z1)
						SetColor_Double_Alpha(0, 0, 1);
					else if(colorFactor <= Z2)
						SetColor_Double_Alpha(0, 0.25, 0.75);
					else if(colorFactor <= Z3)
						SetColor_Double_Alpha(0, 0.75, 0.25);
					else if(colorFactor <= Z4)
						SetColor_Double_Alpha(0.25, 0.75, 0);
					else if(colorFactor <= Z5)
						SetColor_Double_Alpha(0.75, 0.25, 0);
					else
						SetColor_Double_Alpha(1, 0, 0);
					for(int i = 0; i < 3; i++)
						glVertex3d(TrianglePts[3 * i], TrianglePts[3 * i + 1], TrianglePts[3 * i + 2]);
				}
				else
				{
					double colorFactor = ((TrianglePts[2] + TrianglePts[5] + TrianglePts[8]) / 3 - MinZ);
					if(colorFactor <= Z1)
						SetColor_Double_Alpha(0, 0, 1);
					else if(colorFactor <= Z2)
						SetColor_Double_Alpha(0, ((colorFactor - Z1) / Z1) * 0.25, (1 - 0.25) * ((colorFactor - Z1) / Z1));
					else if(colorFactor <= Z3)
						SetColor_Double_Alpha(0, 0.25 + ((colorFactor - Z2) / Z1) * 0.5, 0.75 - (1 - 0.5) * ((colorFactor - Z2) / Z1));
					else if(colorFactor <= Z4)
						SetColor_Double_Alpha(((colorFactor - Z3) / Z1) * 0.25, 0.75, 0.25 - (1 - 07.5) * ((colorFactor - Z3) / Z1));
					else if(colorFactor <= Z5)
						SetColor_Double_Alpha(0.25 + ((colorFactor - Z4) / Z1) * 0.5, 0.75 - (1 - 0.5) * ((colorFactor - Z4) / Z1), 0);
					else
						SetColor_Double_Alpha(1, 0, 0);
					for(int i = 0; i < 3; i++)
						glVertex3d(TrianglePts[3 * i], TrianglePts[3 * i + 1], TrianglePts[3 * i + 2]);
				}		
			}
		}
		glEnd();
		if(AllowTransparency)
			DisableTransparency();
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_PlanePolygon(double* planepolygonpts, int ptscount, bool AllowTransparency, bool Wiredframe)
{
	try
	{
		glPushMatrix();
		double x[3] = {0};
		if(AllowTransparency)
			EnableTransparency();
		if(Wiredframe)
			glBegin(GL_LINE_LOOP);
		else
			glBegin(GL_POLYGON);	
		for(int i = 0; i < ptscount; i++)
		{
			x[0] = *(planepolygonpts + 3*i);
			x[1] = *(planepolygonpts + 3*i + 1);
			x[2] = *(planepolygonpts + 3*i + 2);
			glVertex3d(x[0], x[1], x[2]);
		}
		glEnd();
		if(AllowTransparency)
			DisableTransparency();
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_PlaneFromPts(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, double* pts, int ptscount, bool AllowTransparency, bool Wiredframe, double wupp)
{
	try
	{
		double tmppt[3] = {0};
		double x0[3] = {x1, y1, z1};
		double v1[3] = {x2 - x1, y2 - y1, z2 - z1};
		double v2[3] = {x4 - x1, y4 - y1, z4 - z1};
		int noofslices = 40;

		if(AllowTransparency)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
		}
		glPushMatrix();

		double v1increment[3] = {v1[0]/noofslices, v1[1]/noofslices, v1[2]/noofslices};
		double v2increment[3] = {v2[0]/noofslices, v2[1]/noofslices, v2[2]/noofslices};
		double diff1 = sqrt(v1increment[0] * v1increment[0] + v1increment[1] * v1increment[1] + v1increment[2] * v1increment[2]);
		double diff2 = sqrt(v2increment[0] * v2increment[0] + v2increment[1] * v2increment[1] + v2increment[2] * v2increment[2]);
		if (diff2 > diff1)
			diff1 = diff2;
		double vcurr[3] = {0}; 
		double vnext[3] = {0};
		for(int i = 0; i < noofslices; i++)
		{
			int PtCnt1 = 0, PtCnt2 = 0;
			GLdouble pts1[1100] = {0}, pts2[1100] = {0};
			for (int a = 0; a < 3; a++)
			{
				vcurr[a] = x0[a] + v1increment[a] * i;
				vnext[a] = x0[a] + v1increment[a] * (i + 1);
			}

			for(int j = 0; j <= noofslices; j++)
			{
				pts1[PtCnt1++] = vcurr[0] + v2increment[0] * j;
				pts1[PtCnt1++] = vcurr[1] + v2increment[1] * j;
				pts1[PtCnt1++] = vcurr[2] + v2increment[2] * j;
				pts2[PtCnt2++] = vnext[0] + v2increment[0] * j;
				pts2[PtCnt2++] = vnext[1] + v2increment[1] * j;
				pts2[PtCnt2++] = vnext[2] + v2increment[2] * j;
			}

			bool drawpolygon = false;
			for(int j = 0; j < PtCnt1/3 - 1; j++)
			{
				drawpolygon = false;
				for (int k = 0; k < ptscount; k++)
				{
					tmppt[0] = *(pts + 3*k + 0) ;
					tmppt[1] = *(pts + 3*k + 1) ;
					tmppt[2] = *(pts + 3*k + 2) ;

					if (((abs(tmppt[0] - pts1[3*j]) < diff1)
						&& (abs(tmppt[1] - pts1[3*j + 1]) < diff1)
						&& (abs(tmppt[2] - pts1[3*j + 2]) < diff1)) ||
						((abs(tmppt[0] - pts2[3*j + 3]) < diff1)
						&& (abs(tmppt[1] - pts2[3*j + 4]) < diff1)
						&& (abs(tmppt[2] - pts2[3*j + 5]) < diff1)))
					{
						drawpolygon = true;
						break;
					}
				}
				if (drawpolygon)
				{
					if(Wiredframe)
						glBegin(GL_LINE_LOOP);
					else
						glBegin(GL_POLYGON);	

					glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
					glVertex3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
					glVertex3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
					glVertex3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
					glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);

					glEnd();
				}
			}
		}

		glPopMatrix();
		if(AllowTransparency)
		{
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::draw_Torus(double x, double y, double z, double l, double m, double n, double outer_radius, double inner_radius, bool Wiredframe, bool AllowTransparency, double wupp)
{
	try
	{	
		int noofslices = 40; //3 / wupp;
		/*if(noofslices < 40) noofslices = 40;
		if(noofslices > 120) noofslices = 120;*/

		double incrangle = 2 * M_PI / noofslices;
		double v = sqrt(l * l + m * m + n * n);
		double ax = 57.2957795 * acos(n/v); 
		double rx = -m , ry = l ;
		double norm = sqrt (l * l + m * m);
		if (norm != 0)
		{
			rx /= norm; 
			ry /= norm;
		}
		else
		{
			rx = 1;
			ry = 0;
		}
		if(AllowTransparency)
			EnableTransparency();
			
		glPushMatrix();
		glTranslatef(x, y, z);
		glRotatef(ax, rx, ry, 0.0);

		for(double angle1 = 0; angle1 < 2 * M_PI; angle1 += incrangle)
		{
			int PtCnt1 = 0, PtCnt2 = 0;
			GLdouble pts1[1100], pts2[1100];
			double c1val_x = outer_radius * cos(angle1), c1val_y = outer_radius * sin (angle1), c2val_x = outer_radius * cos(angle1 + incrangle), c2val_y = outer_radius * sin(angle1 + incrangle);
			
			for(double angle2 = 0; angle2 < 2*M_PI; angle2 += IncrementAngle)
			{
				pts1[PtCnt1++] = c1val_x + inner_radius * cos(angle2) * cos(angle1);
				pts1[PtCnt1++] = c1val_y + inner_radius * cos(angle2) * sin(angle1);
				pts1[PtCnt1++] = inner_radius * sin (angle2);
				pts2[PtCnt2++] = c2val_x + inner_radius * cos(angle2) * cos(angle1 + incrangle);
				pts2[PtCnt2++] = c2val_y + inner_radius * cos(angle2) * sin(angle1 + incrangle);
				pts2[PtCnt2++] = inner_radius * sin (angle2);
			}
			for(int j = 0; j < PtCnt1/3 - 1; j++)
			{
				if(Wiredframe)
					glBegin(GL_LINE_LOOP);
				else
					glBegin(GL_POLYGON);	
				glNormal3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glNormal3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
				glVertex3d(pts1[3 * j + 3], pts1[3 * j + 4], pts1[3 * j + 5]);
				glNormal3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
				glVertex3d(pts2[3 * j + 3], pts2[3 * j + 4], pts2[3 * j + 5]);
			//	glNormal3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
				glVertex3d(pts2[3 * j], pts2[3 * j + 1], pts2[3 * j + 2]);
			//	glNormal3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);
				glVertex3d(pts1[3 * j], pts1[3 * j + 1], pts1[3 * j + 2]);

				glEnd();
			}
			if(Wiredframe)
				glBegin(GL_LINE_LOOP);
			else
				glBegin(GL_POLYGON);	

			glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);
			glVertex3d(pts1[0], pts1[1], pts1[2]);
			glVertex3d(pts2[0], pts2[1], pts2[2]);
			glVertex3d(pts2[PtCnt1 - 3], pts2[PtCnt1 - 2], pts2[PtCnt1 - 1]);
			glVertex3d(pts1[PtCnt1 - 3], pts1[PtCnt1 - 2], pts1[PtCnt1 - 1]);

			glEnd();
		}
		glPopMatrix();
		if(AllowTransparency)
			DisableTransparency();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127a", __FILE__, __FUNCSIG__); }	
}

//Create new font object...//
BOOL RGraphicsDraw::CreateNewFont(HWND handle, char* fontname)
{
	try
	{	
		//return false;
		UINT m_uiListID = glGenLists(FONTLIST);
		if(m_uiListID == 0)
			return FALSE;
		HDC hdc = GetDC(handle);
		//Create the font to displayL"Courier New"
		HFONT newfont;
		HFONT* oldfont;
		BOOL  bresult;
		newfont = CreateFont(-24, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH,  (LPCWSTR)fontname);
		oldfont = (HFONT*)SelectObject(hdc,  newfont);
		bresult = wglUseFontOutlines(hdc, 0, FONTLIST, m_uiListID + FONTLIST, 0.0f, 0.2f, WGL_FONT_POLYGONS, gms);	
		//bresult = wglUseFontBitmaps(hdc, 0, 255, m_uiListID + FONTLIST);
		if(!bresult)return FALSE;		
		SelectObject(hdc, oldfont);
		DeleteObject(&newfont);
		glListBase(m_uiListID + FONTLIST);
		return TRUE;
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0004", __FILE__, __FUNCSIG__); return 0; }
}

//Calculate the string width......//
double RGraphicsDraw::getTextWidth(char *string)
{
	double length = 0;
	try
	{
		if(this->fname == "Tahoma")
		{
		while(*string != 0)
		{
			try
			{
				if(*string == char(176) || *string == char(39) || *string == char(40) || *string == char(41))
					length += 0.15;
				else
					length += gms[*string].gmfCellIncX;
				string++;
			}
			catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
		}
		}

		else if(this->fname== "Times Roman 24")
		{
			while(*string != 0)
			{
				try
				{
					if(*string == char(176) || *string == char(39) || *string == char(40) || *string == char(41))
						length += 0.15;
					else
						length += 0.5;
					string++;
				}
				catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
			}
		}
		else if(this->fname== "Stroke Roman")
		{
			while(*string != 0)
			{
				try
				{
					if(*string == char(176) || *string == char(39) || *string == char(40) || *string == char(41))
						length += 0.15;
					else
						length += 0.4;
					string++;
				}
				catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
			}
		}
		else if(this->fname== "Stroke Mono Roman")
		{
			while(*string != 0)
			{
				try
				{
					if(*string == char(176) || *string == char(39) || *string == char(40) || *string == char(41))
						length += 0.2;
					else
						length += 0.6;
					string++;
				}
				catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
			}
		}
		else //if(this->fname== "Calibri 9")
		{
			while(*string != 0)
			{
				try
				{
					if(*string == char(176) || *string == char(39) || *string == char(40) || *string == char(41))
						length += 0.15;
					else
						length += 0.4;
					string++;
				}
				catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
			}
		}
		return length;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
}

//Calculate the string height.....//
double RGraphicsDraw::getTexHeight(char *string)
{
	double length = 0, templen = 0;
	try
	{
		while(*string != 0)
		{
			try
			{
				if(*string != char(176) && *string != char(39) && *string != char(40) && *string != char(41))
					templen = gms[*string].gmfBlackBoxY; //	templen = 2 * gms[*string].gmfBlackBoxY - gms[*string].gmfptGlyphOrigin.y;
				if(length < templen)
					length = templen;
				string++;
			}
			catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
		}
		if(this->fname== "Times Roman 24")
			return (length + 0.1);
		else
			return length;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
}

double RGraphicsDraw::getYShift(char *string)
{
	double length = 0, templen = 0, YShift = 0;
	try
	{
		while(*string != 0)
		{
			try
			{
				if(*string != char(176) && *string != char(39))
					templen = 2 * gms[*string].gmfBlackBoxY - gms[*string].gmfptGlyphOrigin.y;
				if(length < templen)
				{
					length = templen;
					YShift = gms[*string].gmfBlackBoxY - gms[*string].gmfptGlyphOrigin.y;
				}
				string++;
			}
			catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
		}
		return YShift;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); return length;}
}


void RGraphicsDraw::drawString3D(double x, double y, double z, char *string, double FScaleFactor)
{
	try
	{
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
		glPushMatrix();
		glLoadIdentity();
		glTranslated(x, y, z);
		glTranslated(-glx / 2, gly/6, z);
		glScaled(FScaleFactor, FScaleFactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);			
		glPopMatrix();						
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

// for shape name display edited
void RGraphicsDraw::drawStringEdited3D(double x, double y, double z, char *string, double FScaleFactor)
{
	try
	{
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
		glPushMatrix();
		glLoadIdentity();
		glTranslated(x, y, z);
		const char *c; 
		glRasterPos3f(-glx / 2 - 3, gly/6, z); 
		
		if(fname=="Tahoma")
		{
			glScaled(FScaleFactor, FScaleFactor, 0);
			glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);   //////////
			glDisable(GL_LINE_SMOOTH);
		}
		else
		{
			for (c=string; *c != '\0'; c++)
			{ 
				if(fname == "Calibri 9")
					glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *c);
				else if(fname == "Times Roman 24")
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
				else if(fname == "Helvetica 18")
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); 
			}
		}
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}



void RGraphicsDraw::drawString3D(double x, double y, double z, double angle, char *string, double FScaleFactor)
{
	try
	{
		//glPushMatrix();
		//double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
		//char text[256];              // Holds Our String
		//va_list ap;   
		//va_start(ap, string);                  // Parses The String For Variables
		//vsprintf(text, string, ap);                // And Converts Symbols To Actual Numbers
		//va_end(ap);                     // Results Are Stored In Text
		//glPushAttrib(GL_LIST_BIT);              // Pushes The Display List Bits     ( NEW )
		////glListBase(base - 32);                  // Sets The Base Character to 32    ( NEW 

		//glTranslated(-glx / 2, gly/6, 0);
		//glScaled(FScaleFactor, FScaleFactor, 0);
		//glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		//glPopMatrix();
		//glEnable(GL_TEXTURE_2D);
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
		glPushMatrix();
		//glShadeModel(GL_SMOOTH);                // Enable Smooth Shading
		//glEnable(GL_DEPTH_TEST);                // Enables Depth Testing
		//glDepthFunc(GL_LEQUAL);                 // The Type Of Depth Testing To Do
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice 
		//printf("char: %c, decimal: %d, float: %f, string: %s", 'X', 1618, 1.618, "text");
		//glListBase(15);
		//glEnable(GL_LINE_SMOOTH);
		double MMatrix[16], TMMatrix[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, gly/6, 0);
		glScaled(FScaleFactor, FScaleFactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		//glDisable(GL_LINE_SMOOTH);
		glPopMatrix();
		//glDisable(GL_TEXTURE_2D);

		
//		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
//		glPushMatrix();
//
//	
//	
//		 //glShadeModel(GL_SMOOTH);                // Enable Smooth Shading
//  
//   // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//   // glDepthFunc(GL_LEQUAL);                 // The Type Of Depth Testing To Do
//   // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
//		/*glEnable(GL_BLEND);
//		glEnable(GL_LINE_SMOOTH);
//        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);*/
//		//glShadeModel (GL_FLAT);
//		glLineWidth(2);
//
//       // glEnable(GL_TEXTURE_2D);
//		//glLineWidth(2);
//      //  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//
//   /*     glEnable(GL_TEXTURE_2D);
//        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//*/
//	/*	glEnable(GL_TEXTURE_2D);
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
//		
//		//glPointSize(0.1f);
//		double MMatrix[16], TMMatrix[16];
//		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
//		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
//		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
//		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
//		glTranslated(x, y, z);
//		glMultMatrixd(&TMMatrix[0]);
//		glTranslated(-glx / 2, gly/16, 0);
//		glScaled(FScaleFactor/100, FScaleFactor/100, 0);
//		char *p;
//		for(p = string; *p; p++)
//			glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
//		//glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
//	/*	glDisable(GL_TEXTURE_2D);
//        glDisable(GL_BLEND);*/
//		glDisable(GL_BLEND);
//		glDisable(GL_LINE_SMOOTH);
//		glLineWidth(1);
//		glPopMatrix();
		
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

// edited for measurement distance display
void RGraphicsDraw::drawStringEdited3D(double x, double y, double z, double angle, char *string, double FScaleFactor)
{
	try
	{
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
		glPushMatrix();
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, TMMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};  //////
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, gly/6, 0);
		const char *c; 
		glRasterPos3f(-glx / 9, gly/6, z); 
		if(fname=="Tahoma")
		{
			glScaled(FScaleFactor, FScaleFactor, 0);
			glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);   //////////
			glDisable(GL_LINE_SMOOTH);
		}
		else if(fname == "Calibri 9" || fname == "Times Roman 24" || fname == "Helvetica 18")
		{
			for (c=string; *c != '\0'; c++) 
			{ 
				if(fname == "Calibri 9")
				{
					if(*c==-80)
						glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 176);
					else
						glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *c);
				}
				else if(fname == "Times Roman 24")
				{
					if(*c==-80)
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 176);
					else
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
				}
				else if(fname == "Helvetica 18")
				{
					if(*c==-80)
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 176);
					else
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
				}
			}
		}
		else
		{
			double ScaleFactor = FScaleFactor / 160;
			glScalef(ScaleFactor, ScaleFactor, ScaleFactor);
			for (c=string; *c != '\0'; c++) 
			{ 
				if(fname == "Stroke Mono Roman")
				{
					if(*c==-80)
						glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, 176);
					else
						glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *c);
				}
				else if(fname == "Stroke Roman")
				{
					if(*c==-80)
						glutStrokeCharacter(GLUT_STROKE_ROMAN, 176);
					else
						glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
				}
			}
		}
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}


void RGraphicsDraw::drawStringWith_BackGroud(double x, double y, double z, double angle, char *string, double FScaleFactor, double r, double g, double b)
{
	try
	{
		
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
		glPushMatrix();		
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, TMMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glColor3d(0, 0, 0);
		glTranslated(0, 0, -10);
		drawRectangle(-glx /2 - glx/32, -gly - gly/2 + gly/16, glx /2 + glx/32, gly + gly/2 + gly/16, true);
		glTranslated(0, 0, 10);
		glColor3d(r, g, b);
		glTranslated(-glx / 2, gly/16, 0);
		glScaled(FScaleFactor, FScaleFactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
		
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::drawString_BackGroud(double x, double y, double z, double StrWidth, double StrHeight, double r, double g, double b)
{
	try
	{
		double glx = StrWidth, gly = StrHeight;
		glPushMatrix();
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, TMMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glColor3d(r, g, b);
		glTranslated(0, 0, -1);
		drawRectangle(-glx/2, gly/2, glx/2,-gly/2, true);
		glTranslated(0, 0, 1);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

void RGraphicsDraw::drawString(double x, double y, double z, double angle, char *string, double FScaleFactor, bool drawatmidpoint)
{
	try
	{
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor;
		double Ysh = getYShift(string) * FScaleFactor;
		glPushMatrix();
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, TMMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glRotated(angle, 0, 0, 1);
		if(drawatmidpoint)
			glTranslated(-glx/2, -gly/2 + Ysh, 0);
		else
			glTranslated(0, -gly, 0);
		glScaled(FScaleFactor, FScaleFactor, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::drawString3D1(double x, double y, double z, double angle, char *string, double FScaleFactor)
{
	try
	{
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor; 
		glPushMatrix();
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, TMMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glTranslated(-glx / 2, -gly - gly/6, 0);
		glScaled(FScaleFactor, FScaleFactor, 0);	
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}

// edited for measurement name display
void RGraphicsDraw::drawStringEdited3D1(double x, double y, double z, double angle, char *string, double FScaleFactor)
{
	try
	{
		const char *c; 
		double glx = getTextWidth(string) * FScaleFactor, gly = getTexHeight(string) * FScaleFactor; 
		glPushMatrix();
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, TMMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glRasterPos3f(-glx / 2, -gly - gly/6, 0); 
		if(fname=="Tahoma")
		{
			glTranslated(-glx / 2, -gly - gly/6, 0);
			glScaled(FScaleFactor, FScaleFactor, 0);
			glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);   //////////
			glDisable(GL_LINE_SMOOTH);
		}
		else if(fname == "Calibri 9" || fname == "Times Roman 24" || fname == "Helvetica 18")
		{
			for (c=string; *c != '\0'; c++) 
			{ 
				if(fname == "Calibri 9")
					glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *c);
				else if(fname == "Times Roman 24")
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
				else if(fname == "Helvetica 18")
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
			}
		}
		else
		{
			glTranslated(-glx / 2, -gly - gly/6, 0); 
			double ScaleFactor = FScaleFactor / 160;
			glScalef(ScaleFactor, ScaleFactor, ScaleFactor);
			for (c=string; *c != '\0'; c++) 
			{ 
				if(fname == "Stroke Mono Roman")
					glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *c);
				else if(fname == "Stroke Roman")
					glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
			}
		}
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }	
}


void RGraphicsDraw::drawStringFixed(double x, double y, double z, double scalefact, char *string)
{
	try
	{
		glPushMatrix();
		glTranslated(x, y, z);
		glScaled(scalefact, scalefact, 0);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}


int RGraphicsDraw::next_powerof2(int a)
{
	int rval = 1;
	while(rval < a) rval <<= 1;
	return rval;
}

void RGraphicsDraw::drawStringtexture(BYTE* bpdata, int bpwidth, int bpheight, double Posx, double Posy, double Posz, double angle)
{
	try
	{
		int width = next_powerof2(bpwidth);
		int height = next_powerof2(bpheight);
		int Step = 4;
		GLubyte* expanded_data = new GLubyte[Step * width * height];
		
		for(int j = 0; j < height; j++) 
		{
			for(int i = 0; i < width; i++)
			{
				if(i >= bpwidth || j >= bpheight)
				{
					for(int k = 0; k < Step; k++)
						expanded_data[j * width * Step + i * Step + k] = 0;
				}
				else
				{
					for(int k = 0; k < Step; k++)
						expanded_data[j * width * Step + i * Step + k] = bpdata[j * bpwidth * Step + i * Step + k];
				}
				Light_Position1[0] = i;
			}
			 Light_Position1[1] = j;
		}
		float x = (float)bpwidth / (float)width,
			y = (float)bpheight / (float)height;
		TextXwidth = x; TextYwidth = y; 
	
		//glColor3d(0.4f, 0.4f, 0.4f);
		GLuint Texture;
		glGenTextures(1, &Texture);// Allocate space for texture
		glBindTexture(GL_TEXTURE_2D, Texture); // Set our Tex handle as current
		glTexImage2D(GL_TEXTURE_2D, 0, Step, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, expanded_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		
		//glBindTexture(GL_TEXTURE_2D, Texture);
		//glTranslated(Posx, Posy, Posz);
		//glScaled(x/400, y/400, 1);
		//glScaled(0.001, 0.001, 1);
		/*glTexCoord2d(0, 0); glVertex2f(0, bpheight);
		glTexCoord2d(0, y); glVertex2f(0, 0);
		glTexCoord2d(x, y); glVertex2f(bpwidth, 0);
		glTexCoord2d(x, 0); glVertex2f(bpwidth, bpheight);*/

		glNewList(25, GL_COMPILE);
		glDisable(GL_LIGHTING);
		glPushMatrix();
		//glTranslated(Posx, Posy, Posz);
		glScaled(x, y, 1);
		
		//glShadeModel(GL_SMOOTH);
		glEnable(GL_TEXTURE_2D);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, Texture);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex2f(0, bpheight);
		glTexCoord2d(0, y); glVertex2f(0, 0);
		glTexCoord2d(x, y); glVertex2f(bpwidth, 0);
		glTexCoord2d(x, 0); glVertex2f(bpwidth, bpheight);
		glEnd();
		//glEnable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);


		/*glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0, 1, 0);
		glTexCoord2d(0, y); glVertex3d(0, 0, 0);
		glTexCoord2d(x, y); glVertex3d(1, 0, 0);
		glTexCoord2d(x, 0); glVertex3d(1, 1, 0);
		glEnd();
		
		glBindTexture(GL_TEXTURE_2D, Texture);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0, 1, 1);
		glTexCoord2d(0, y); glVertex3d(0, 0, 1);
		glTexCoord2d(x, y); glVertex3d(1, 0, 1);
		glTexCoord2d(x, 0); glVertex3d(1, 1, 1);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, Texture);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0, 0, 1);
		glTexCoord2d(0, y); glVertex3d(0, 0, 0);
		glTexCoord2d(x, y); glVertex3d(1, 0, 0);
		glTexCoord2d(x, 0); glVertex3d(1, 0, 1);
		glEnd();		
		glBindTexture(GL_TEXTURE_2D, Texture);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0, 1, 1);
		glTexCoord2d(0, y); glVertex3d(0, 1, 0);
		glTexCoord2d(x, y); glVertex3d(1, 1, 0);
		glTexCoord2d(x, 0); glVertex3d(1, 1, 1);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, Texture);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0, 1, 0);
		glTexCoord2d(0, y); glVertex3d(0, 1, 1);
		glTexCoord2d(x, y); glVertex3d(0, 0, 1);
		glTexCoord2d(x, 0); glVertex3d(0, 0, 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, Texture);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(1, 1, 0);
		glTexCoord2d(0, y); glVertex3d(1, 1, 1);
		glTexCoord2d(x, y); glVertex3d(1, 0, 1);
		glTexCoord2d(x, 0); glVertex3d(1, 0, 0);
		glEnd();
		*/
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glEndList();

		//glDeleteTextures(1, &Texture);
		delete[] expanded_data;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::GenerateTextureFromImage(BYTE* bpdata, int bpwidth, int bpheight)
{
	try
	{
		int width = next_powerof2(bpwidth);
		int height = next_powerof2(bpheight);
		int Step = 4;
		GLubyte* expanded_data = new GLubyte[Step * width * height];
		
		for(int j = 0; j < height; j++) 
		{
			for(int i = 0; i < width; i++)
			{
				if(i >= bpwidth || j >= bpheight)
				{
					for(int k = 0; k < Step; k++)
						expanded_data[j * width * Step + i * Step + k] = 0;
				}
				else
				{
					for(int k = 0; k < Step; k++)
						expanded_data[j * width * Step + i * Step + k] = bpdata[j * bpwidth * Step + i * Step + k];
				}
				Light_Position1[0] = i;
			}
			 Light_Position1[1] = j;
		}

		float x = (float)bpwidth / (float)width,
			y = (float)bpheight / (float)height;
		TextXwidth = x; TextYwidth = y; 
		glColor3d(0.4f, 0.4f, 0.4f);
		glGenTextures(1, &CurrentTextureId);// Allocate space for texture
		glBindTexture(GL_TEXTURE_2D, CurrentTextureId); // Set our Tex handle as current
		glTexImage2D(GL_TEXTURE_2D, 0, Step, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, expanded_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		//delete[] expanded_data;
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::RenderTextureInCube(double Posx, double Posy, double Posz, double wupp)
{
	try
	{
		glPushMatrix();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		glTranslated(Posx, Posy, Posz);
		glScaled(wupp, wupp, 1);
		glCallList(25);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0127", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::DrawBackGround(bool is3Dmode)
{
	try
	{
		if(is3Dmode)
		{
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glTranslated(0, 0, 0.1);
			glBegin(GL_QUADS);
			glColor3dv(Bg_Color_Lower); 
			glVertex3i(-1,-1,0.9);
			glVertex3i(1,-1,0.9);    
			glColor3dv(Bg_Color_Upper);  // grey - white
			glVertex3i(1,1,0.9);
			glVertex3i(-1,1,0.9);
			glEnd();
			glPopMatrix();	
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		
		} 
		else
			glClearColor(0, 0, 0, 0);
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0128", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::DrawColorIndicator(double* ZlevelExtent, double* LeftTop, double tolerance, double WPixelWidth)
{
	try
	{
		double RectLefttop[2] = {LeftTop[0] + 5 * WPixelWidth, LeftTop[1] - 5 * WPixelWidth}, RightBottom[2] = {LeftTop[0] + 35 * WPixelWidth, LeftTop[1] - 50 * WPixelWidth};
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
			
		int TextMultipleFactor = 15, YShiftfacor = 15 * WPixelWidth;
		double UpperColor[3] = {1, 0, 0}, LowerColor[3] = {0.75, 0.25, 0};
		double value = tolerance;
		if(tolerance == 0)
			value = (ZlevelExtent[1] - ZlevelExtent[0]) / 6;
		char cd[12] = {"\0"};
		RMATH2DOBJECT->Double2String(5 * value, 4, cd);
		glColor3d(UpperColor[0], UpperColor[1], UpperColor[2]); 
		drawStringFixed(RightBottom[0], RectLefttop[1] - YShiftfacor, 0, WPixelWidth * TextMultipleFactor, cd); 
				
		DrawRectangleWithGradient(RectLefttop, RightBottom, UpperColor, LowerColor);	
		
		RMATH2DOBJECT->Double2String(4 * value, 4, cd);
		drawStringFixed(RightBottom[0], RightBottom[1] - YShiftfacor, 0, WPixelWidth * TextMultipleFactor, cd);

		RectLefttop[1] = RightBottom[1]; RightBottom[1] = RightBottom[1] - 45 * WPixelWidth;
		UpperColor[0] = LowerColor[0]; UpperColor[1] = LowerColor[1]; UpperColor[2] = LowerColor[2];
		LowerColor[0] = 0.25; LowerColor[1] = 0.75; LowerColor[2] = 0;
		DrawRectangleWithGradient(RectLefttop, RightBottom, UpperColor, LowerColor);

		RMATH2DOBJECT->Double2String(3 * value, 4, cd);
		drawStringFixed(RightBottom[0], RightBottom[1] - YShiftfacor, 0, WPixelWidth * TextMultipleFactor, cd);
		
		RectLefttop[1] = RightBottom[1]; RightBottom[1] = RightBottom[1] - 45 * WPixelWidth;
		UpperColor[0] = LowerColor[0]; UpperColor[1] = LowerColor[1]; UpperColor[2] = LowerColor[2];
		LowerColor[0] = 0; LowerColor[1] = 0.75; LowerColor[2] = 0.25;
		DrawRectangleWithGradient(RectLefttop, RightBottom, UpperColor, LowerColor);

		RMATH2DOBJECT->Double2String(2 * value, 4, cd);
		drawStringFixed(RightBottom[0], RightBottom[1] - YShiftfacor, 0, WPixelWidth * TextMultipleFactor, cd);

		RectLefttop[1] = RightBottom[1]; RightBottom[1] = RightBottom[1] - 45 * WPixelWidth;
		UpperColor[0] = LowerColor[0]; UpperColor[1] = LowerColor[1]; UpperColor[2] = LowerColor[2];
		LowerColor[0] = 0; LowerColor[1] = 0.25; LowerColor[2] = 0.75;
		DrawRectangleWithGradient(RectLefttop, RightBottom, UpperColor, LowerColor);
		
		RMATH2DOBJECT->Double2String(value, 4, cd);
		drawStringFixed(RightBottom[0], RightBottom[1] - YShiftfacor, 0, WPixelWidth * TextMultipleFactor, cd);

		RectLefttop[1] = RightBottom[1]; RightBottom[1] = RightBottom[1] - 45 * WPixelWidth;
		UpperColor[0] = LowerColor[0]; UpperColor[1] = LowerColor[1]; UpperColor[2] = LowerColor[2];
		LowerColor[0] = 0; LowerColor[1] = 0; LowerColor[2] = 1;
		DrawRectangleWithGradient(RectLefttop, RightBottom, UpperColor, LowerColor);
		
		RMATH2DOBJECT->Double2String(0.0, 4, cd);
		drawStringFixed(RightBottom[0], RightBottom[1] - YShiftfacor, 0, WPixelWidth * TextMultipleFactor, cd);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0129", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::DrawRectangleWithGradient(double* LeftTop, double* RightBottom, double* UpperColor, double* LowerColor)
{
	try
	{
		glBegin(GL_QUADS);
		glColor3d(UpperColor[0], UpperColor[1], UpperColor[2]); 
		glVertex2d(LeftTop[0], LeftTop[1]);
		glVertex2d(RightBottom[0], LeftTop[1]);	
		glColor3d(LowerColor[0], LowerColor[1], LowerColor[2]);
		glVertex2d(RightBottom[0], RightBottom[1]);
		glVertex2d(LeftTop[0], RightBottom[1]);
		glEnd();
	}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0130", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::SetBackGroundGradient(double r1,double g1, double b1, double r2, double g2, double b2)
{
	try
	{
		Bg_Color_Upper[0] = r1/255;
		Bg_Color_Upper[1] = g1/255;
		Bg_Color_Upper[2] = b1/255;
		Bg_Color_Lower[0] = r2/255;
		Bg_Color_Lower[1] = g2/255;
		Bg_Color_Lower[2] = b2/255;
		}
	catch(...){ SetAndRaiseErrorMessage("RGRAPHICS0130", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::SetFGandCloudPtSize(int PtSize)
{
	this->PointSize = PtSize;
}

void RGraphicsDraw::InsertText(double x,double y, double z, bool flag, std::list<std::string > strings, double FScaleFactor)
{
		glPushMatrix();		
		bool first = true;
		double MMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, TMMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		glGetDoublev(GL_MODELVIEW_MATRIX, &MMatrix[0]);
		MMatrix[3] = 0; MMatrix[7] = 0; MMatrix[11] = 0;
		MMatrix[12] = 0; MMatrix[13] = 0; MMatrix[14] = 0;
		RMATH2DOBJECT->OperateMatrix4X4(&MMatrix[0], 4, 1, &TMMatrix[0]);
		glTranslated(x, y, z);
		glMultMatrixd(&TMMatrix[0]);
		glColor3d(1, 0, 0);
		
		for (std::list <std::string >::iterator it = strings.begin(); it != strings.end(); it++)
	{
		glPushMatrix();
		double glx, gly;
		glTranslatef(-FScaleFactor * 8, - FScaleFactor * 2,0);
		std::string str = *it;
		if(first)
		{
		 glx = getTextWidth(((char*)str.c_str())) * FScaleFactor;
		 gly = getTexHeight(((char*)str.c_str())) * FScaleFactor / 2;
		first = false;
		}
		if(flag==1)
		drawRectangle(- glx/32,- gly/2 + gly/16, glx + glx/32, gly + gly + gly, false);
		glScaled(FScaleFactor, FScaleFactor, 0);
		glCallLists((GLsizei)strlen(((char*)str.c_str())), GL_UNSIGNED_BYTE, ((char*)str.c_str()));
		glPopMatrix();
		glTranslatef(0,-FScaleFactor - FScaleFactor/ 4  ,0);
	}
		glPopMatrix();

}

double getTangentAng(double s, int iter = 100)
{
	   double phi = s;
	   double phi_revised = s;
	   for (int i = 0; i < iter; i++)
	   {
			  phi_revised = atan (phi) + s;
			  if (abs(phi_revised - phi) < 0.0001 * phi)
					 break;
			  phi = phi_revised;
	   }
	   return phi_revised;
}

//draw conic in 3D
void RGraphicsDraw::DrawCircleInvoluteShape(double* Involuteparam)
{
	try
	{
		double startangle , endangle , startradius, endradius , CenterX , CenterY;
		startangle = Involuteparam[4];
		endangle = Involuteparam[5];
		startradius = Involuteparam[6];
		endradius = Involuteparam[7];
		CenterX = Involuteparam[0];
		CenterY = Involuteparam[1];
		glPushMatrix();		
		
		DrawInvolute(startradius, endradius, startangle, endangle , CenterX ,CenterY );
		glPopMatrix();
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0062", __FILE__, __FUNCSIG__); }
}

//draw Circle Involute in 2d
void RGraphicsDraw::DrawInvolute(double startradius, double endradius, double startAng, double endAng ,double CenterX, double CenterY )
{
	try
	{
		int i = 0;
		GLdouble pts[730] = {0};
		unsigned int ptsorder[365] = {0};
		unsigned int ptscount = 0;
		double radius;
		double alpha,R , Phi;
		for(double angle = startAng ; angle <= endAng; angle += IncrementAngle)
		{		
			if(angle-startAng !=0)
			{
				alpha = getTangentAng(angle-startAng);
				R = startradius * alpha;
				radius = R * sqrt(1 + pow(alpha,2));						
				ptsorder[ptscount++] = ptscount;
				Phi = startAng + alpha - atan(alpha);
				pts[i++] = (radius * cos(Phi));
				pts[i++] = (radius * sin(Phi));			
			}
		}
		if (endAng == 2 * M_PI)
			ptsorder[ptscount] = 0;
		translateMatrix(CenterX,CenterY,0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, pts);
		glDrawElements(GL_LINE_STRIP, ptscount, GL_UNSIGNED_INT, ptsorder);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0063", __FILE__, __FUNCSIG__); }
}

void RGraphicsDraw::DrawCamProfile(double *Pts, int PtsCount, int *PtsOrder)
{
	try
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_DOUBLE, 0, Pts);
		glDrawElements(GL_LINE_STRIP, PtsCount, GL_UNSIGNED_INT, PtsOrder);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	catch(...){ SetAndRaiseErrorMessage("GRDLL0063", __FILE__, __FUNCSIG__); }
}

