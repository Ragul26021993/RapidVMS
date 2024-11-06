#include "StdAfx.h"
#include "RWindow.h"
#include "math.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"
#include "OverlapImgWin.h"

RWindow::RWindow()
{
	initVariables();
}

RWindow::~RWindow()
{
}

//Initialize the Rwindow ... Properties of the windows..//
void RWindow::init(RWindow &win)
{	
	try
	{
		this->init(win.getHandle(), win.getUppX(), win.getUppY(), win.getUppf(), (int)win.getWinDim().x, (int)win.getWinDim().y);
		this->camx = win.getCam().x;
		this->camy = win.getCam().y;
		this->camz = 0;
		this->PanShiftX = win.PanShiftX;
		this->PanShiftY = win.PanShiftY;
		double extn[4];
		win.getExtents(extn);
		this->VWinWidth = abs(extn[1] - extn[0]);
		this->VWinHeight = abs(extn[2] - extn[3]);
		this->WindowNo = win.WindowNo;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0001", __FILE__, __FUNCSIG__); }
}

//Initialize the window with its properties..//
void RWindow::init(HWND winhandle, double upp_x, double upp_y, double uppf, UINT32 width, UINT32 height)
{
	try
	{
		this->handle = winhandle;
		this->uppX = upp_x; this->uppY = upp_y;
		this->uppf = uppf;
		this->h_uppX = uppX; this->h_uppY = uppY;
		//Transformation Matrix...
		RMATH2DOBJECT->LoadIdentityMatrix(&TransformationMatrix[0], 4);
		RMATH2DOBJECT->LoadIdentityMatrix(&InverseMatrix[0], 4);
		RMATH2DOBJECT->LoadIdentityMatrix(&TransposeMatrix[0], 4);
		ResizeWindow(width, height, false);
		//this->WindowNo = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0002", __FILE__, __FUNCSIG__); }
}

void RWindow::init(RWindow& win, double upp_x, double upp_y, double uppf)
{
	try
	{
		this->init(win.getHandle(), upp_x, upp_y, uppf, (int)win.getWinDim().x, (int)win.getWinDim().y);
		this->camx = win.getCam().x;
		this->camy = win.getCam().y;
		double extn[4];
		win.getExtents(extn);
		this->VWinWidth = abs(extn[1] - extn[0]);
		this->VWinHeight = abs(extn[2] - extn[3]);
		//this->WindowNo = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0003", __FILE__, __FUNCSIG__); }
}

void RWindow::initVariables()
{
	try
	{
		this->maxed(false); this->WindowPanned(false); this->WindowResized(false); this->WindowHidden(false);
		this->PanMode = false; this->panning = false; this->camz = 0;
		this->camx = 0; this->camy = 0; this->camz = 0; this->uppX = 0; this->uppY = 0; this->uppf = 0;
		this->MmoveX = 0; this->MmoveY = 0; this->MdownX = 0; this->MdownY = 0; this->WinWidth = 0; this->WinHeight = 0;
		this->WMmoveX = 0; this->WMmoveY = 0; this->WMdownX = 0; this->WMdownY = 0; this->VWinWidth = 0; this->VWinHeight = 0;
		this->LastWMmoveX = 0; this->LastWMmoveY = 0; this->PanShiftX = 0; this->PanShiftY = 0;  
		this->h_camx = 0; this->h_camy = 0; this->h_VWinWidth = 0; this->h_VWinHeight = 0;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0004", __FILE__, __FUNCSIG__); }
}

//setting params, camera positions, calc viewport dims etc..
void RWindow::ResizeWindow( UINT32 width, UINT32 height, bool flag)
{
	try
	{
		this->WinWidth = width;
		this->WinHeight = height;
		double w = width * uppX;
		double h = height * uppY;
		if(VWinWidth != 0)
		{
			this->camx = (camx + VWinWidth/2) * (h/VWinHeight);
			this->camy = (camy - VWinHeight/2) * (h/VWinHeight);
		}
		else
		{
			this->camx = camx - w/2;
			this->camy = camy + h/2;
		}
		this->VWinWidth = w;
		this->VWinHeight = h;
		this->h_VWinWidth = width * h_uppX;
		this->h_VWinHeight = height * h_uppY;
		
		this->WindowResized(true);
		this->WindowPanned(true);
		if(flag)
		{
			this->maxed(!this->maxed());
			MAINDllOBJECT->updateAll();
			UpdateCenterScreen();
		}
		else
			MAINDllOBJECT->UpdateGraphicsOf3Window();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0005", __FILE__, __FUNCSIG__); }
}

//Rcad/ dxf window zoom in/out for mouse scroll..
void RWindow::changeZoom(bool in)
{
	try
	{
		if(this->panning || MAINDllOBJECT->DXFMouseFlag || MAINDllOBJECT->RCADMouseFlag)
			return;
		double LastUppx = uppX, LastUppy = uppY;
		double lastmx1 = this->WMmoveX;// this->camx + (MmoveX * LastUppx);
		double lastmy1 = this->WMmoveY; // this->camy - (MmoveY * LastUppy);
		if(in)
		{
			if(MAINDllOBJECT->ControlOn && MAINDllOBJECT->ShiftON)
				this->uppX /= (uppf + 0.04);
			else if(MAINDllOBJECT->ControlOn)
				this->uppX /= uppf / 2;
			else
				this->uppX /= uppf;
		}
		else 
		{
			if(MAINDllOBJECT->ControlOn && MAINDllOBJECT->ShiftON)
				this->uppX *= (uppf + 0.04);
			else if(MAINDllOBJECT->ControlOn)
				this->uppX *= uppf / 2;
			else
				this->uppX *= uppf;
		}
		if(this->uppX > 500) this->uppX = 500;
		if(this->uppX < 0.00005) this->uppX = 0.00005;
		this->uppY = this->uppX;

		//if(MAINDllOBJECT->getCurrentWindow() == 1)
		//	MAINDllOBJECT->centerScreen_Update(MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), 0);
		//else
		//	MAINDllOBJECT->centerScreen_Update(0, 0, 0);
		VWinWidth = uppX * WinWidth;
		VWinHeight = uppX * WinHeight;
		//double lastmx2 = this->camx + MmoveX * uppX; // -this->VWinWidth / 2 + (MmoveX * uppX); // -h_camx;
		//double lastmy2 = this->camy - MmoveY * uppY; // this->VWinHeight / 2 - (MmoveY * uppY); // -h_camy;
		R_Point mmx = this->CalculateTransformedCoordinates(MmoveX, MmoveY);

		this->PanShiftX += mmx.x - lastmx1;// lastmx2 - lastmx1;
		this->PanShiftY += mmx.y - lastmy1;// lastmy2 - lastmy1;
		if (this->WindowNo == 1 || this->WindowNo == 2)
		{
			this->camx -= mmx.x - lastmx1;//lastmx2 - lastmx1;
			this->camy -= mmx.y - lastmy1;//lastmy2 - lastmy1;
		}

		if(MAINDllOBJECT->getCurrentWindow() == 1)
		{
			MAINDllOBJECT->Entity_UpdatedOnRcad(false);
			UpdateCenterScreen_Zoom(1);
		}
		else if(MAINDllOBJECT->getCurrentWindow() == 2)
		{
			MAINDllOBJECT->DXFMeasurement_Updated(false);
			MAINDllOBJECT->PartProgramShape_updated(false);
			UpdateCenterScreen_Zoom(2);
		}
		else if(MAINDllOBJECT->getCurrentWindow() == 3)
		{
			UpdateCenterScreen_Zoom(3);
		}
		this->LastWMmoveX = MmoveX; // this->camx + (MmoveX * this->uppX);
		this->LastWMmoveY = MmoveY; // this->camy - (MmoveY * this->uppY);
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0006", __FILE__, __FUNCSIG__); }
}

//this function will be called only for the video window
void RWindow::changeZoom_Video(double pixelwidth_x, double pixelwidth_y)
{
	try
	{
		this->uppX = pixelwidth_x;
		this->uppY = pixelwidth_y;
		this->VWinWidth = uppX * WinWidth;
		this->VWinHeight = uppY * WinHeight;
		//this->camx = -VWinWidth/2;
		//this->camy = VWinHeight/2;
		this->WindowPanned(true);
		UpdateCenterScreen();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0007", __FILE__, __FUNCSIG__); }
}

//Change the zoom of the Rcad for rectangle...//
void RWindow::changeZoom_Box(double pixelwidth, double x, double y)
{
	try
	{
		MAINDllOBJECT->DXFMouseFlag = true;
		MAINDllOBJECT->RCADMouseFlag = true;
		this->MmoveX = x; this->MmoveY = y;
		double LastUppx = uppX, LastUppy = uppY;		
		this->uppX = pixelwidth;
		if(this->uppX > 500) this->uppX = 500;
		if(this->uppX < 0.00005) this->uppX = 0.00005;
		this->uppY = this->uppX;
		
		if(MAINDllOBJECT->getCurrentWindow() == 1)
			MAINDllOBJECT->centerScreen_Update(MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), 0);
		else
			MAINDllOBJECT->centerScreen_Update(0, 0, 0);

		double lastmx1 = this->camx + (MmoveX * LastUppx);
		double lastmy1 = this->camy - (MmoveY * LastUppy);
		VWinWidth = uppX * WinWidth;
		VWinHeight = uppY * WinHeight;
		double lastmx2 = -this->VWinWidth/2 + (MmoveX * uppX) - h_camx;
		double lastmy2 = this->VWinHeight/2 - (MmoveY * uppY) - h_camy;
		this->PanShiftX = lastmx2 - lastmx1;
		this->PanShiftY = lastmy2 - lastmy1;

		if(MAINDllOBJECT->getCurrentWindow() == 1)
		{
			MAINDllOBJECT->Entity_UpdatedOnRcad(false);
			UpdateCenterScreen_Zoom(1);
		}
		else if(MAINDllOBJECT->getCurrentWindow() == 2)
		{
			MAINDllOBJECT->DXFMeasurement_Updated(false);
			MAINDllOBJECT->PartProgramShape_updated(false);
			UpdateCenterScreen_Zoom(2);
		}
		this->LastWMmoveX = this->camx + (MmoveX * this->uppX);
		this->LastWMmoveY = this->camy - (MmoveY * this->uppY);
		
		MAINDllOBJECT->DXFMouseFlag = false;
		MAINDllOBJECT->RCADMouseFlag = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0008", __FILE__, __FUNCSIG__); }
}

//Calculate the DRO positions....
R_Point RWindow::mouseMove( UINT32 mx, UINT32 my)
{
	try
	{
		this->MmoveX = mx;

		this->MmoveY = my;
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			R_Point CPt = PixelCalibration_OneShot(mx, my);
			this->WMmoveX = CPt.x;
			this->WMmoveY = CPt.y;
		}
		else
		{
			R_Point mmx = this->CalculateTransformedCoordinates(mx, my);
			this->WMmoveX = mmx.x;
			this->WMmoveY = mmx.y;
		}

		if(this->panning)
		{
			double panX = (this->LastWMmoveX - mx) * this->uppX, panY = (my - this->LastWMmoveY) * this->uppY; // this->WMmoveY;
			if (this->LastWMmoveX == 0) panX = 0;
			if (this->LastWMmoveY == 0) panY = 0;
			
			//if (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3)
			//{
			//	double rr = sqrt(panX * panX + panY * panY);
			//	double theta = 0;
			//	if (panX == 0)
			//	{
			//		if (panY == 0)
			//			theta = 0;
			//		else if (panY > 0)
			//			theta = M_PI_2;
			//		else
			//			theta = 3 * M_PI_2;
			//	}
			//	else
			//	{
			//		theta = atan(panY / panX);
			//		if (panX < 0) theta += M_PI;
			//		if (panY < 0 && panX > 0) theta += 2 * M_PI;
			//	}
			//	panX = rr * cos(theta + MAINDllOBJECT->getCurrentUCS().UCSangle());
			//	panY = rr * sin(theta + MAINDllOBJECT->getCurrentUCS().UCSangle());
			//}
			this->camx += panX;
			this->camy += panY;
			this->PanShiftX -= panX; // this->WMmoveX - this->LastWMmoveX;
			this->PanShiftY -= panY; // this->WMmoveY - this->LastWMmoveY;
			this->WindowPanned(true);
		}
		this->LastWMmoveX = mx; // this->WMmoveX;
		this->LastWMmoveY = my; // this->WMmoveY;
		return(R_Point(this->WMmoveX, this->WMmoveY));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0009", __FILE__, __FUNCSIG__); }
}

//Mouse down event on window...//
void RWindow::mouseDown(UINT32 mx, UINT32 my)
{
	try
	{
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			R_Point CPt = PixelCalibration_OneShot(mx, my);
			this->WMmoveX = CPt.x;
			this->WMmoveY = CPt.y;
		}
		else
		{
			R_Point mmx = this->CalculateTransformedCoordinates(mx, my);
			this->WMmoveX = mmx.x;
			this->WMmoveY = mmx.y;
		}
		this->MdownX = mx;
		this->MdownY = my;
		this->WMdownX = this->WMmoveX;
		this->WMdownY = this->WMmoveY;
		this->RotateX = mx;
		this->RotateY = my;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0010", __FILE__, __FUNCSIG__); }
}

void RWindow::ResetMouseLastPosition()
{
	this->MmoveX = this->WinWidth / 2;
	this->MmoveY = this->WinHeight / 2;
}

void RWindow::SetMousemoveFor3Dmode()
{
	this->MmoveX = this->RotateX + 22;
	this->MmoveY = this->RotateY + 7;
}

R_Point RWindow::PixelCalibration_OneShot(double mx, double my, bool digitalZoom)
{
	try
	{
		double Shift_X, Shift_Y, CWx, CWy;
		double M_x = mx, M_y = my;
		double XPos_Right = MAINDllOBJECT->XPositionStart_Right() + this->WinWidth / 2;
		double XPos_Left = this->WinWidth / 2 - MAINDllOBJECT->XPositionStart_Left();
		double YPos_Btm = MAINDllOBJECT->YPositionStart_Bottom() + this->WinHeight / 2;
		double YPos_Top = this->WinHeight / 2 - MAINDllOBJECT->YPositionStart_Top();
		double Shift_XRightGap = this->WinWidth - XPos_Right;
		double Shift_XLeftGap = XPos_Left;
		double Shift_YBtmGap = this->WinHeight - YPos_Btm;
		double Shift_YTopGap = YPos_Top;
		if(mx > XPos_Right) 
		{
			Shift_X = mx - XPos_Right;
			M_x = mx + Shift_X * ((MAINDllOBJECT->XMultiplyFactor_Right() - 1)  * (Shift_X/Shift_XRightGap));
		}
		if(mx < XPos_Left) 
		{
			Shift_X = XPos_Left - mx;
			M_x = mx - Shift_X * ((MAINDllOBJECT->XMultiplyFactor_Left() - 1)  * (Shift_X/Shift_XLeftGap));
		}

		if(my > YPos_Btm) 
		{
			Shift_Y = my - YPos_Btm;
			M_y = my + Shift_Y * ((MAINDllOBJECT->YMultiplyFactor_Bottom() - 1)  * (Shift_Y/Shift_YBtmGap));
		}
		if(my < YPos_Top) 
		{
			Shift_Y = YPos_Top - my;
			M_y = my - Shift_Y * ((MAINDllOBJECT->YMultiplyFactor_Top() - 1)  * (Shift_Y/Shift_YTopGap));
		}

		//if(MAINDllOBJECT->PixelWidthCorrectionForOneShot())
		//{
		//	//double RadialDistance = RMATH2DOBJECT->Pt2Pt_distance(((this->WinWidth / 2)+(MAINDllOBJECT->PixelWidthX0())), ((this->WinHeight / 2)+(MAINDllOBJECT->PixelWidthY0())), mx, my);	//vinod...........
		//	//double Pwx = MAINDllOBJECT->PixelWidthCorrectionLinearTermX() * RadialDistance + MAINDllOBJECT->PixelWidthCorrectionQuadraticTerm() * RadialDistance * RadialDistance + uppX;
		//	//double Pwy = MAINDllOBJECT->PixelWidthCorrectionLinearTermY() * RadialDistance + MAINDllOBJECT->PixelWidthCorrectionQuadraticTerm() * RadialDistance * RadialDistance + uppY;
		//	//if(digitalZoom)
		//	//{
		//	//	CWx = this->camx + (M_x * Pwx / 2);
		//	//	CWy = this->camy - (M_y * Pwy / 2);
		//	//}
		//	//else
		//	//{
		//	//	CWx = this->camx + (M_x * Pwx);
		//	//	CWy = this->camy - (M_y * Pwy);
		//	//}
		//}
		//else
		{
			if(digitalZoom)
			{
				CWx = this->camx + (M_x * this->uppX / 2);
				CWy = this->camy - (M_y * this->uppY / 2);
			}
			else
			{
				CWx = this->camx + (M_x * this->uppX);
				CWy = this->camy - (M_y * this->uppY);
			}
		}
		
		return R_Point(CWx, CWy);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0010", __FILE__, __FUNCSIG__); }
}

//Toggle the pan mode...//
void RWindow::togglePan()
{
	this->panning = !(this->panning);
	this->LastWMmoveX = 0; this->LastWMmoveY = 0;
	if (this->WindowNo == 1)
		this->PanShiftX = 0; this->PanShiftY = 0;
}

//Rotate the Rcad Window......///
void RWindow::RotateGraphics(RGraphicsWindow* WndGraphics)
{
	try
	{	
		if(abs(MmoveX - RotateX) > abs(MmoveY - RotateY))
		{
			if(MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				WndGraphics->RotateGraphics((MmoveX - RotateX)/2, 0, 1, 0, CenterOfRotation.x, CenterOfRotation.y, CenterOfRotation.z, &TransformationMatrix[0]);
			else if(MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				WndGraphics->RotateGraphics((MmoveX - RotateX)/2, 0, 1, 0, CenterOfRotation.x, CenterOfRotation.z, CenterOfRotation.y, &TransformationMatrix[0]);
			else if(MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				WndGraphics->RotateGraphics((MmoveX - RotateX)/2, 0, 1, 0, CenterOfRotation.y, CenterOfRotation.z, CenterOfRotation.x, &TransformationMatrix[0]);
			else if (MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				WndGraphics->RotateGraphics((MmoveX - RotateX) / 2, 0, 1, 0, CenterOfRotation.x, -CenterOfRotation.z, CenterOfRotation.y, &TransformationMatrix[0]);
			else if (MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				WndGraphics->RotateGraphics((MmoveX - RotateX) / 2, 0, 1, 0, CenterOfRotation.y, -CenterOfRotation.z, CenterOfRotation.x, &TransformationMatrix[0]);
		}
		else
		{
			if(MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				WndGraphics->RotateGraphics((MmoveY - RotateY)/2, 1, 0, 0, CenterOfRotation.x, CenterOfRotation.y, CenterOfRotation.z, &TransformationMatrix[0]);
			else if(MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				WndGraphics->RotateGraphics((MmoveY - RotateY)/2, 1, 0, 0, CenterOfRotation.x, CenterOfRotation.z, CenterOfRotation.y, &TransformationMatrix[0]);
			else if(MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				WndGraphics->RotateGraphics((MmoveY - RotateY)/2, 1, 0, 0, CenterOfRotation.y, CenterOfRotation.z, CenterOfRotation.x, &TransformationMatrix[0]);
			else if (MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				WndGraphics->RotateGraphics((MmoveY - RotateY) / 2, 1, 0, 0, CenterOfRotation.x, -CenterOfRotation.z, CenterOfRotation.y, &TransformationMatrix[0]);
			else if (MAINDllOBJECT->ProjectionType == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				WndGraphics->RotateGraphics((MmoveY - RotateY) / 2, 1, 0, 0, CenterOfRotation.y, -CenterOfRotation.z, CenterOfRotation.x, &TransformationMatrix[0]);
		}
	    RotateX = MmoveX;
		RotateY = MmoveY;
		//camz = camz - TransformationMatrix[14];
		RMATH2DOBJECT->TransposeMatrix(&TransformationMatrix[0], 4, &TransposeMatrix[0]);
		RMATH2DOBJECT->OperateMatrix4X4(&TransposeMatrix[0], 4, 1, &InverseMatrix[0]);
		MAINDllOBJECT->UpdateShapesChanged();
		MAINDllOBJECT->RotateGrafix(false);
		//MAINDllOBJECT->Measurement_updated();
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0011", __FILE__, __FUNCSIG__); }
}

//returns the mouse position with respect to the window...
R_Point RWindow::getPointer(bool flag)
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 1 && MAINDllOBJECT->RcadWindow3DMode())
		{
			if(flag)
				return R_Point(this->WMmoveX, this->WMmoveY);
			else
			{
				double e1[4] = {TransformationMatrix[0], TransformationMatrix[1], TransformationMatrix[2], WMmoveX};
				double e2[4] = {TransformationMatrix[4], TransformationMatrix[5], TransformationMatrix[6], WMmoveY};
				double e3[4] = {TransformationMatrix[8], TransformationMatrix[9], TransformationMatrix[10], 0};
				double ans[4] = {0};
				RMATH3DOBJECT->SolveEquation(e1, e2, e3, ans);
				//double Cx = (WMmoveX * TransformationMatrix[5] - WMmoveY * TransformationMatrix[4]) / (TransformationMatrix[0] * TransformationMatrix[5] - TransformationMatrix[1] * TransformationMatrix[4]);
			    //double Cy = (WMmoveY - Cx * TransformationMatrix[1])/TransformationMatrix[5];
				//return R_Point(Cx, Cy);
				return R_Point(ans[0], ans[1]);
			}		
		}	
		else
		{
			//if(MAINDllOBJECT->getCurrentUCS().UCSMode() > 1 && MAINDllOBJECT->getCurrentUCS().UCSMode() != 4 &&  (MAINDllOBJECT->getCurrentWindow() == 0 || (MAINDllOBJECT->getCurrentWindow() == 1 && !MAINDllOBJECT->getCurrentUCS().KeepGraphicsStraightMode())))
			//{
			//	//double radius = 
			//	Vector temp = MAINDllOBJECT->TransformMultiply(&MAINDllOBJECT->getCurrentUCS().transform[0], WMmoveX, WMmoveY);
			//	return R_Point(temp.getX(),temp.getY());
			//}
			//else
				return R_Point(this->WMmoveX, this->WMmoveY);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0012", __FILE__, __FUNCSIG__); return R_Point(0, 0); }
}

void RWindow::CalculateSelectionLine(double* Sline)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		double TransformedPt[4] = {0}, CmousePt[4] = {WMmoveX, WMmoveY, 0, 1};
		RMATH2DOBJECT->MultiplyMatrix1(&InverseMatrix[0], &s1[0], &CmousePt[0], &s2[0], &TransformedPt[0]);
		for(UINT32 i = 0; i < 3; i++)
		{
			Sline[i] = TransformedPt[i];
			Sline[3 + i] = TransposeMatrix[8 + i];
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0013", __FILE__, __FUNCSIG__); }
}

//returns transformation matrix........
void RWindow::getTransformMatrixfor3Drotate(double* transform, UINT32 cnt)
{
	if(cnt == 0)
	{
		for(UINT32 i = 0; i <= 15; i++)
			transform[i] = TransposeMatrix[i];
	}
	else if (cnt == 1)
	{
	    for(UINT32 i = 0; i <= 15; i++)
			transform[i] = TransformationMatrix[i];
	}
	else if (cnt == 2)
	{
	    for(UINT32 i = 0; i <= 15; i++)
			transform[i] = InverseMatrix[i];
	}
}

//Returns the center of the camera... center of the window...//
R_Point RWindow::getCam()
{
	return R_Point(this->camx, this->camy);
}

//Returns the current Unit per pixel of the window....
double RWindow::getUppX()
{
	return this->uppX;
}

double RWindow::getUppY()
{
	return this->uppY;
}


//returns the original pixel width of the window..//
double RWindow::getOriginalUpp()
{
	return this->h_uppX;
}

//Multiplying factor for zoom in/out..///
double RWindow::getUppf()
{
	return this->uppf;
}

//Returns the window view settings...//
R_Point RWindow::getViewDim()
{
	return R_Point(this->VWinWidth,this->VWinHeight);
}

//Get the window width and height property...//
R_Point RWindow::getWinDim()
{
	return R_Point(this->WinWidth, this->WinHeight);
}

//Returns the window handle..//
HWND RWindow::getHandle()
{
	return this->handle;
}

//Returns the window handle...//
void RWindow::getExtents(double extents[])
{
	try
	{
		//if (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3)
		//{
		//	extents[0] = this->camx;
		//	extents[1] = this->camx + this->VWinWidth * cos(MAINDllOBJECT->getCurrentUCS().UCSangle()) + this->VWinHeight * abs(sin(MAINDllOBJECT->getCurrentUCS().UCSangle()));
		//	extents[2] = this->camy - (this->VWinWidth * abs(sin(MAINDllOBJECT->getCurrentUCS().UCSangle())) + this->VWinHeight * cos(MAINDllOBJECT->getCurrentUCS().UCSangle()));
		//	extents[3] = this->camy;
		//}
		//else
		{
			extents[0] = this->camx;
			extents[1] = this->camx + this->VWinWidth;
			extents[2] = this->camy - this->VWinHeight;
			extents[3] = this->camy;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0014", __FILE__, __FUNCSIG__); }
}

//function to relocate camera, so that the 
//parameters passed become the center of the camera
R_Point RWindow::centerCam(double x, double y)
{
	try
	{
		//if (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3)
		//{
		//	//Vector* pp = new Vector(x, y);
		//	//Vector temp = MAINDllOBJECT->TransformMultiply(&MAINDllOBJECT->getCurrentUCS().transform[0], -((this->VWinWidth) / 2) + x, (this->VWinHeight) / 2 + y);
		//	////return R_Point(temp.getX(), temp.getY());

		//	//this->camx = temp.getX();//x - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX() + -((this->VWinWidth) / 2); //cos(MAINDllOBJECT->getCurrentUCS().UCSangle()) * this->VWinWidth / 2;
		//	//this->camy = temp.getY();// y - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY() + (this->VWinHeight) / 2; //+ sin(MAINDllOBJECT->getCurrentUCS().UCSangle()) * this->VWinHeight / 2;
		//	//If UCS is rotated, we need an aligned rectangle that contains the rotated video size rectangle 
		//	double rWidth = this->VWinWidth * cos(MAINDllOBJECT->getCurrentUCS().UCSangle()) + this->VWinHeight * abs(sin(MAINDllOBJECT->getCurrentUCS().UCSangle()));
		//	double rHeight = this->VWinWidth * abs(sin(MAINDllOBJECT->getCurrentUCS().UCSangle())) + this->VWinHeight * cos(MAINDllOBJECT->getCurrentUCS().UCSangle());
		//	//Put the camera at the top left of this rectangle. 
		//	this->camx = x - rWidth / 2;
		//	this->camy = y + rHeight / 2;
		//}
		//else
		//if (this->WindowNo != 1) // || this->DontMoveWithDro == false)
		//{
			this->camx = -((this->VWinWidth) / 2) + x;
			this->camy = (this->VWinHeight) / 2 + y;
		//}
		//else 
		//{

		//	if ((MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3) && !MAINDllOBJECT->getCurrentUCS().KeepGraphicsStraightMode())
		//	{
		//		double rad = sqrt(this->VWinWidth * this->VWinWidth + this->VWinHeight * this->VWinHeight) / 2;
		//		double phi = atan(this->VWinHeight / this->VWinWidth);
		//		//double delX = 0, delY = 0;
		//		this->camx = rad * cos(phi + MAINDllOBJECT->getCurrentUCS().UCSangle());// +h_camx; // -this->VWinWidth;// / 2;
		//		this->camy = rad * sin(phi + MAINDllOBJECT->getCurrentUCS().UCSangle());// +h_camy; // +this->VWinHeight; // / 2;
		//	}
		//	else
		//	{
		//		this->camx = -((this->VWinWidth) / 2); // +this->PanShiftX; // +x;
		//		this->camy = (this->VWinHeight) / 2; // +this->PanShiftY; // +y;
		//	}
		//}
		return (R_Point(camx, camy));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0015", __FILE__, __FUNCSIG__); }
} 

void RWindow::SetCenterOfRotation(double x, double y, double z)
{
	try
	{
		CenterOfRotation.x = x;
		CenterOfRotation.y = y;
		CenterOfRotation.z = z;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0015a", __FILE__, __FUNCSIG__); }
}

//get the center position of the camera
R_Point RWindow::getCenter()
{
	try
	{
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3 )
		{
		//	//Vector temp = MAINDllOBJECT->TransformMultiply(&MAINDllOBJECT->getCurrentUCS().transform[0], this->camx + this->VWinWidth/2, this->camy - this->VWinHeight/2);
		//	//return R_Point(this->camx - cos(MAINDllOBJECT->getCurrentUCS().UCSangle()) * this->VWinWidth / 2, this->camy + sin(MAINDllOBJECT->getCurrentUCS().UCSangle()) * this->VWinHeight / 2);
		//	// MAINDllOBJECT->getCurrentUCS().UCSPoint.getX() + this->camx + this->VWinWidth / 2, MAINDllOBJECT->getCurrentUCS().UCSPoint.getY() - this->VWinHeight / 2);
		//	//double rWidth = this->VWinWidth * cos(MAINDllOBJECT->getCurrentUCS().UCSangle()) + this->VWinHeight * abs(sin(MAINDllOBJECT->getCurrentUCS().UCSangle()));
		//	//double rHeight = this->VWinWidth * abs(sin(MAINDllOBJECT->getCurrentUCS().UCSangle())) + this->VWinHeight * cos(MAINDllOBJECT->getCurrentUCS().UCSangle());
		//	
		//	//double radius = (sqrt(this->VWinWidth * this->VWinWidth + this->VWinHeight * this->VWinHeight)) / 2;
		//	//double theta = MAINDllOBJECT->getCurrentUCS().UCSangle();// - atan(this->VWinHeight / this->VWinWidth);
		//	//return R_Point(this->camx + radius * cos(theta), this->camy - radius * sin(theta));
			if (this->WindowNo == 0)
				return R_Point(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY());
			else if (MAINDllOBJECT->getCurrentUCS().KeepGraphicsStraightMode())
				return R_Point(this->camx + this->VWinWidth / 2, this->camy - this->VWinHeight / 2);
			else
			{
				double rad = sqrt(this->VWinWidth * this->VWinWidth + this->VWinHeight * this->VWinHeight) / 2;
				double phi = atan(this->VWinHeight / this->VWinWidth);
				return R_Point( this->camx - rad * cos(phi + MAINDllOBJECT->getCurrentUCS().UCSangle()), 
								this->camy + rad * sin(phi + MAINDllOBJECT->getCurrentUCS().UCSangle()));
			}
		}
		else
			return R_Point(this->camx + this->VWinWidth/2, this->camy - this->VWinHeight/2);
				
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0016", __FILE__, __FUNCSIG__); return R_Point(0, 0); }
}

//Returns the displacement of center for pan/ zoom...
R_Point RWindow::getpt()
{
	return R_Point(PanShiftX, PanShiftY);
}

//Returns the last mouse move...//
R_Point RWindow::getLastWinMouse()
{
	return R_Point(this->MmoveX, this->MmoveY);
}

R_Point RWindow::getHomediff()
{
	return R_Point(this->h_camx, this->h_camy);
}

R_Point RWindow::getRtranslate()
{
	return R_Point(this->TransformationMatrix[12], this->TransformationMatrix[13], this->TransformationMatrix[14]);
}

//Reuturns the pan mode...///
bool RWindow::GetpanMode()
{
	return(this->panning);
}

//Returns the Current mouse down...//
R_Point RWindow::GetMouseDown()
{
	return R_Point(this->MdownX, this->MdownY);
}

R_Point RWindow::GetLastMouseDown()
{
	return R_Point(this->WMdownX, this->WMdownY);
}

R_Point RWindow::GetMouseMove()
{
	return R_Point(this->MmoveX, this->MmoveY);
}

//Reset the window pan position..
void RWindow::ResetPan()
{
	try
	{
		RotateX = 0;
		RotateY = 0;
		PanShiftX = 0;
		PanShiftY = 0;
		WindowPanned(true);
		UpdateCenterScreen();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0017", __FILE__, __FUNCSIG__); }
}

//Set the home of the window.....//
void RWindow::homeIt(UINT32 windowno)
{
	try
	{
		RotateX = 0;
		RotateY = 0;
		PanShiftX = 0;
		PanShiftY = 0;
		camz = 0;
		//UCS *ucs = &(MAINDllOBJECT->getCurrentUCS());
		//Vector UcsPt = MAINDllOBJECT->getCurrentUCS().UCSPoint;
		//Vector UcsDiff;
		//if(windowno == 1)
		//{
		//	double ux = UcsPt.getX() + MAINDllOBJECT->getCurrentUCS().currentDroX();
		//	double uy = UcsPt.getY() + MAINDllOBJECT->getCurrentUCS().currentDroY();
		//	double uz = UcsPt.getZ() + MAINDllOBJECT->getCurrentUCS().currentDroZ();
		//	UcsDiff.set(ux, uy, uz);
		//}
		//if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		//}
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getY(); // - UcsDiff.getY();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getZ(); // - UcsDiff.getZ();
		//}
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX(); // - UcsDiff.getX();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getZ(); // - UcsDiff.getZ();
		//}
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getY(); // - UcsDiff.getY();
		//	h_camy =  -MAINDllOBJECT->getCurrentDRO().getZ(); //UcsDiff.getZ()
		//}
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX(); // -UcsDiff.getX();
		//	h_camy = -MAINDllOBJECT->getCurrentDRO().getZ(); //UcsDiff.getZ() 
		//}
		VWinWidth = h_VWinWidth;
		VWinHeight = h_VWinHeight;
		ResetAll();
		uppX = h_uppX;
		uppY = h_uppY;
		WindowPanned(true);
		if (windowno != 1)
			UpdateCenterScreen();
	
		if (windowno == 1)
			this->centerCam(0, 0);
		else if (windowno == 2)
			this->centerCam(MAINDllOBJECT->getCurrentUCS().GetCurrentUCS_DROpostn().getX(), MAINDllOBJECT->getCurrentUCS().GetCurrentUCS_DROpostn().getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0018", __FILE__, __FUNCSIG__); }
}

void RWindow::DontMoveWithDro(UINT32 windowno)
{
	try
	{
		Vector UcsPt = MAINDllOBJECT->getCurrentUCS().UCSPoint;
		Vector UcsDiff;
		//this->WindowNo = windowno;
		if(windowno == 1)
		{
			double ux = UcsPt.getX() + MAINDllOBJECT->getCurrentUCS().currentDroX();
			double uy = UcsPt.getY() + MAINDllOBJECT->getCurrentUCS().currentDroY();
			double uz = UcsPt.getZ() + MAINDllOBJECT->getCurrentUCS().currentDroZ();
			UcsDiff.set(ux, uy, uz);
		}
		//if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		//}
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getZ() - UcsDiff.getZ();
		//}
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getZ() - UcsDiff.getZ();
		//}
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		//	h_camy = UcsDiff.getZ() - MAINDllOBJECT->getCurrentDRO().getZ();
		//}
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		//	h_camy = UcsDiff.getZ() - MAINDllOBJECT->getCurrentDRO().getZ();
		//}

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0018", __FILE__, __FUNCSIG__); }
}

void RWindow::SetTransFormationMatrix(double* matrix)
{
	try
	{
		for(UINT32 i = 0; i < 16; i++)
			TransformationMatrix[i] = matrix[i];
		RMATH2DOBJECT->TransposeMatrix(&TransformationMatrix[0], 4, &TransposeMatrix[0]);
		RMATH2DOBJECT->OperateMatrix4X4(&TransposeMatrix[0], 4, 1, &InverseMatrix[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0019", __FILE__, __FUNCSIG__); }
}

//Reset all matrices
void RWindow::ResetAll()
{
	try
	{
		RMATH2DOBJECT->LoadIdentityMatrix(&TransformationMatrix[0], 4);
		RMATH2DOBJECT->LoadIdentityMatrix(&TransposeMatrix[0], 4);
		RMATH2DOBJECT->LoadIdentityMatrix(&InverseMatrix[0], 4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0019", __FILE__, __FUNCSIG__); }
}

void RWindow::UpdateCenterScreen(bool updateVideo)
{
	try
	{
		if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), updateVideo);
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX(), updateVideo);
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY(), updateVideo);
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX(), updateVideo);
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY(), updateVideo);

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0020", __FILE__, __FUNCSIG__); }
}

void RWindow::UpdateCenterScreen_Zoom(UINT32 windowno)
{
	try
	{
		if(windowno == 1)
		{
			if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				MAINDllOBJECT->SetcenterScreen_Rcad(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				MAINDllOBJECT->SetcenterScreen_Rcad(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX());
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				MAINDllOBJECT->SetcenterScreen_Rcad(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY());
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				MAINDllOBJECT->SetcenterScreen_Rcad(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX());
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				MAINDllOBJECT->SetcenterScreen_Rcad(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY());
		}
		else if(windowno == 2)
		{
			if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				MAINDllOBJECT->SetcenterScreen_Dxf(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				MAINDllOBJECT->SetcenterScreen_Dxf(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX());
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				MAINDllOBJECT->SetcenterScreen_Dxf(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY());
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				MAINDllOBJECT->SetcenterScreen_Dxf(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX());
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				MAINDllOBJECT->SetcenterScreen_Dxf(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY());
		}
		else if(windowno ==3)
		{			
			//OVERLAPIMAGEWINOBJECT->SetcenterScreen_OvImgWin(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
			OVERLAPIMAGEWINOBJECT->SetcenterScreen_OvImgWin(0, 0, 0);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0020", __FILE__, __FUNCSIG__); }
}

void RWindow::ZoomToExtents(UINT32 windowno)
{
	if(windowno == 1)
		UpdateRcadZoomToExtents();
	else if(windowno == 2)
		UpdateDxfZoomToExtents();
	else if(windowno ==3)
	{
		UpdateImageWinZoomToExtent();		
	}
	Sleep(20);
	MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	MAINDllOBJECT->Wait_DxfGraphicsUpdate();
	Sleep(20);
}

void RWindow::UpdateRcadZoomToExtents()
{
	try
	{
		//if ((MAINDllOBJECT->getShapesList().count() <= MAINDllOBJECT->FundamentalShapes()))
		//{
		//	if ((MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::AUTOFOCUSSCANHANDLER))
		//		return;
		//}
		if(PPCALCOBJECT->PartprogramisLoadingEditMode() || PPCALCOBJECT->PartprogramisLoading()) return;
		double LeftTop[2] = {-VWinWidth / 2, VWinHeight / 2}, RightBottom[2] = {VWinWidth/ 2, -VWinHeight / 2};
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->dontUpdateGraphcis = true;
		if(MAINDllOBJECT->RcadWindow3DMode())
		{
			if(!HELPEROBJECT->GetRcadWindowEntityExtentsfor3DMode(&TransposeMatrix[0], &LeftTop[0], &RightBottom[0]))
			{
				MAINDllOBJECT->dontUpdateGraphcis = false;
				return;
			}
		}
		else
		{
			if(!HELPEROBJECT->GetRcadWindowEntityExtents(&LeftTop[0], &RightBottom[0]))
			{
				MAINDllOBJECT->dontUpdateGraphcis = false;
				return;
			}
		}
		
		//Vector UcsPt = MAINDllOBJECT->getCurrentUCS().UCSPoint;
		//Vector UcsDiff;
		////PanShiftX = 0; PanShiftY = 0;
		//double ux = UcsPt.getX() + MAINDllOBJECT->getCurrentUCS().currentDroX();
		//double uy = UcsPt.getY() + MAINDllOBJECT->getCurrentUCS().currentDroY();
		//double uz = UcsPt.getZ() + MAINDllOBJECT->getCurrentUCS().currentDroZ();
		//UcsDiff.set(ux, uy, uz);

		//if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		//}
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getZ() - UcsDiff.getZ();
		//}
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		//	h_camy = MAINDllOBJECT->getCurrentDRO().getZ() - UcsDiff.getZ();
		//}
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		//	h_camy = UcsDiff.getZ() - MAINDllOBJECT->getCurrentDRO().getZ();
		//}
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		//{
		//	h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		//	h_camy = UcsDiff.getZ() - MAINDllOBJECT->getCurrentDRO().getZ();
		//}
		UpdateWindowView(&LeftTop[0], &RightBottom[0]);
		//if ((MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3) && !MAINDllOBJECT->getCurrentUCS().KeepGraphicsStraightMode())
		//{
		//	std::wofstream myfile;
		//	std::string wext = "D:\\windowpts.csv";
		//	myfile.open(wext, ios::app);
		//	myfile << LeftTop[0] << ", " << LeftTop[1] << ", " << RightBottom[0] << ", " << RightBottom[1] << endl;
		//	myfile.close();
		//}
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->UpdateShapesChanged(false);
		//UpdateCenterScreen(false);		
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0020", __FILE__, __FUNCSIG__); }
}

void RWindow::UpdateDxfZoomToExtents()
{
	try
	{
		if(DXFEXPOSEOBJECT->CurrentDXFShapeList.size() < 1) return;
		double LeftTop[2] = {0, h_VWinHeight}, RightBottom[2] = {h_VWinWidth, 0};
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_DxfGraphicsUpdate();
		MAINDllOBJECT->dontUpdateGraphcis = true;
		if(!HELPEROBJECT->GetDxfWindowEntityExtents(&LeftTop[0], &RightBottom[0]))
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return;
		}
		/*Vector UcsPt = MAINDllOBJECT->getCurrentUCS().UCSPoint;
		Vector UcsDiff;
		double ux = UcsPt.getX() + MAINDllOBJECT->getCurrentUCS().currentDroX();
		double uy = UcsPt.getY() + MAINDllOBJECT->getCurrentUCS().currentDroY();
		double uz = UcsPt.getZ() + MAINDllOBJECT->getCurrentUCS().currentDroZ();
		UcsDiff.set(ux, uy, uz);
		h_camx = MAINDllOBJECT->getCurrentDRO().getX() - UcsDiff.getX();
		h_camy = MAINDllOBJECT->getCurrentDRO().getY() - UcsDiff.getY();
		*/
		h_camx = 0; h_camy = 0;
		UpdateWindowView(&LeftTop[0], &RightBottom[0]);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->DXFShape_Updated(false);
		MAINDllOBJECT->DXFMeasurement_Updated(false);	
		//PanShiftX += MAINDllOBJECT->getCurrentDRO().getX(); PanShiftY += MAINDllOBJECT->getCurrentDRO().getY();
		//UpdateCenterScreen();
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0021", __FILE__, __FUNCSIG__); }
}

void RWindow::UpdateImageWinZoomToExtent()
{
		double LeftTop[2] = {0, h_VWinHeight}, RightBottom[2] = {h_VWinWidth, 0};
		OVERLAPIMAGEWINOBJECT->Wait_OverlapImgWinGraphicsUpdate();		
		OVERLAPIMAGEWINOBJECT->DontUpdateGraphics(true);
		OVERLAPIMAGEWINOBJECT->GetWindowEntityExtents(&LeftTop[0], &RightBottom[0]);
		h_camx = 0; h_camy = 0;
		UpdateWindowView(&LeftTop[0], &RightBottom[0]);
		OVERLAPIMAGEWINOBJECT->DontUpdateGraphics(false);
		UpdateCenterScreen();
}

void RWindow::UpdateWindowView(double* LeftTop, double* RightBottom)
{
	try
	{

		uppX = h_uppX;
		VWinWidth = h_VWinWidth;
		VWinHeight = h_VWinHeight;
		if(!MAINDllOBJECT->RcadWindow3DMode())
		{
			RotateX = 0; RotateY = 0;
		}	
		PanShiftX = 0; PanShiftY = 0;
		
		double WiniewDim[4]; getExtents(&WiniewDim[0]);
		double MVWinWidth = RightBottom[0] - LeftTop[0], MVWinHeight = LeftTop[1] - RightBottom[1];
		double XScaleFactor = MVWinWidth/VWinWidth;
		double YScaleFactor = MVWinHeight/VWinHeight;
		if(XScaleFactor > YScaleFactor)
			this->uppX *= XScaleFactor * 1.05;
		else
			this->uppX *= YScaleFactor * 1.05;
		if(this->uppX > 500) 
			this->uppX = 500;
		if(this->uppX < 0.00005) 
			this->uppX = 0.00005;
		this->uppY = this->uppX;
		VWinWidth = uppX * WinWidth;
		VWinHeight = uppY * WinHeight;
		//double CurrentCenter[2] = {camx + VWinWidth/2, camy - VWinHeight/2};
		//double ModifiedCenter[2] = {LeftTop[0] + MVWinWidth /2, LeftTop[1] - MVWinHeight/2};
		PanShiftX = (LeftTop[0] + RightBottom[0]) / 2 - this->camx; PanShiftY = (LeftTop[1] + RightBottom[1]) / 2 - this->camy; // -= ModifiedCenter[1];
		this->camx = (LeftTop[0] + RightBottom[0]) / 2 -  VWinWidth / 2;
		this->camy  = (LeftTop[1] + RightBottom[1]) / 2 + VWinHeight / 2;
		WindowPanned(true);
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0022", __FILE__, __FUNCSIG__); }
}

void RWindow::GetSelectionLine(double x, double y, double *Answer)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		double TransformedPt[4] = {0}, CmousePt[4] = {x, y, 0, 1};
		RMATH2DOBJECT->MultiplyMatrix1(&InverseMatrix[0], &s1[0], &CmousePt[0], &s2[0], &TransformedPt[0]);
		for(UINT32 i = 0; i < 3; i++)
		{
			Answer[i] = TransformedPt[i];
			Answer[3 + i] = TransposeMatrix[8 + i];
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWIN0023", __FILE__, __FUNCSIG__); }
}

R_Point RWindow::CalculateTransformedCoordinates(double mx, double my)
{
	double x = 0, y = 0;

	if (!this->panning && (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3))
	{
		double dx = (mx - this->WinWidth / 2) * this->uppX, dy = (this->WinHeight / 2 - my) * this->uppY;
		double radius = sqrt(dx * dx + dy * dy);
		double theta = 0;
		if (dx == 0)
		{
			if (dy == 0)
				theta = 0;
			else if (dy > 0)
				theta = M_PI_2;
			else
				theta = 3 * M_PI_2;
		}
		else
		{
			theta = atan(dy / dx);
			if (dx < 0) theta += M_PI;
			if (dy < 0 && dx > 0) theta += 2 * M_PI;
		}
		if (this->WindowNo != 1)
		{
			x = this->getCenter().x + radius * cos(theta - MAINDllOBJECT->getCurrentUCS().UCSangle());
			y = this->getCenter().y + radius * sin(theta - MAINDllOBJECT->getCurrentUCS().UCSangle());
		}
		else
		{
			double originX = -this->camx / this->uppX;
			double originY = this->camy / this->uppY;
			double dx = (mx - originX) * this->uppX, dy = (originY - my) * this->uppY;
			double radius = sqrt(dx * dx + dy * dy);
			double theta = 0;
			if (dx == 0)
			{
				if (dy == 0)
					theta = 0;
				else if (dy > 0)
					theta = M_PI_2;
				else
					theta = 3 * M_PI_2;
			}
			else
			{
				theta = atan(dy / dx);
				if (dx < 0) theta += M_PI;
				if (dy < 0 && dx > 0) theta += 2 * M_PI;
			}
			if (MAINDllOBJECT->getCurrentUCS().KeepGraphicsStraightMode())
			{
				x = this->camx + mx * this->uppX;
				y = this->camy - my * this->uppY;
			}
			else
			{
				x = radius * cos(theta - MAINDllOBJECT->getCurrentUCS().UCSangle());
				y = radius * sin(theta - MAINDllOBJECT->getCurrentUCS().UCSangle());
			}
		}
	}
	else
	{
		x = this->camx + mx * this->uppX;
		y = this->camy - my * this->uppY;
	}
	return R_Point(x, y);
}



