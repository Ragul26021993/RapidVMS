#include "StdAfx.h"
#include "MouseHandler.h"
#include "..\Shapes\Shape.h"
#include "..\Framegrab\FramegrabBase.h"
#include "..\Engine\RCadApp.h"
#include "DefaultHandler.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor......
double Temp_raxis_value=0.0;		//vinod 15/02/2014 . . to set Temp_raxis_value bcz we need to display it also, and inbuilt fns implemented for x,y,z only			
MouseHandler::MouseHandler()
{
	try
	{
		ShapeThickness = 1;
		clicksdone = 0; maxclicks = 0; PreventDoublePt = true;
		CurrentShape = NULL; CurrentdrawHandler = NULL;
		FinishedCurrentDrawing = false; AlignmentModeFlag = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
MouseHandler::~MouseHandler()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0002", __FILE__, __FUNCSIG__); }
}

//Reset the current handler clickdone count
void MouseHandler::resetClicks()
{
	try{ this->clicksdone = 0; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0003", __FILE__, __FUNCSIG__); }
}

//Maximum clicks used for an entity.....///
void MouseHandler::setMaxClicks(int clicks)
{
	try{ this->maxclicks = clicks; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0004", __FILE__, __FUNCSIG__); }
}

//Set the current handle click done...
void MouseHandler::setClicksDone(int clicks)
{
	try{ this->clicksdone = clicks; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0005", __FILE__, __FUNCSIG__); }
}

//Returns the maximum clicks..
int MouseHandler::getMaxClicks()
{
	return this->maxclicks;
}

//Returns the clickdone
int MouseHandler::getClicksDone()
{
	return this->clicksdone;
}

//Set the x, y, z of click value..
void MouseHandler::setClicksValue(int i, Vector* pt)
{
	try{ clicks[i].set(*pt); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0006", __FILE__, __FUNCSIG__); }
}

//Set the x, y, z of the click value.
void MouseHandler::setClicksValue(int i, double x, double y, double z)
{
	try{ clicks[i].set(x, y, z); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0007", __FILE__, __FUNCSIG__); }
}

void MouseHandler::raxis_valueSet(double r)			//vinod 15/02/2014 . . to set Temp_raxis_value bcz we need to display it also, and inbuilt fns implemented for x,y,z only				
{
	try{ Temp_raxis_value=r; }						//this is Temp_raxis_value used in setBaseRProperty fn to display r value also in baseaction->points[0].set(....)									
	catch(...){}
}
//set the click and add to action...//
void MouseHandler::setBaseRProperty(double x, double y, double z, bool Probeflag)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
			baseaction = PPCALCOBJECT->getFrameGrab();
		else
		{
			if(Probeflag || MAINDllOBJECT->ProbeEnableFlag)
			{
				MAINDllOBJECT->ProbeEnableFlag = false;
				baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT);
			}
			else
				baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE);
			baseaction->CurrentWindowNo = MAINDllOBJECT->getCurrentWindow();
			if(MAINDllOBJECT->getCurrentWindow() == 1 && MAINDllOBJECT->highlightedPoint() != NULL)
			{
				if(CurrentShape != NULL)
				{
					baseaction->Pointer_SnapPt = MAINDllOBJECT->highlightedPoint();
					baseaction->DerivedShape = true;
					bool PresentFlag1 = false;
					if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL)
					{
						for each(Shape* Csh in CurrentShape->getParents())
						{
							if(Csh->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->getId())
							{
								PresentFlag1 = true;
								break;
							}
						}
						if(!PresentFlag1)CurrentShape->addParent(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1());
						PresentFlag1 = false;
						for each(Shape* Csh in MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->getChild())
						{
							if(Csh->getId() == CurrentShape->getId())
							{
								PresentFlag1 = true;
								break;
							}
						}
						if(!PresentFlag1)MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->addChild(CurrentShape);
					}
				
					PresentFlag1 = false;
					if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2() != NULL)
					{
						for each(Shape* Csh in CurrentShape->getParents())
						{
							if(Csh->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->getId())
							{
								PresentFlag1 = true;
								break;
							}
						}
						if(!PresentFlag1)CurrentShape->addParent(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2());
						PresentFlag1 = false;
						for each(Shape* Csh in MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->getChild())
						{
							if(Csh->getId() == CurrentShape->getId())
							{
								PresentFlag1 = true;
								break;
							}
						}
						if(!PresentFlag1)MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->addChild(CurrentShape);
					}
				}
			}
		}
		if(MAINDllOBJECT->CameraConnected())
		{		
			baseaction->camProperty = MAINDllOBJECT->getCamSettings();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}
		baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
		if(Probeflag)
			baseaction->PointDRO.set(PPCALCOBJECT->MachineDro_ProbeTouch);
		else
			baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO);
		baseaction->DontCheckProjectionType(true);
		baseaction->ProfileON = MAINDllOBJECT->getProfile();
		baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		// Getting DRO position of Mouse Click........
		baseaction->points[0].set(x, y, z, Temp_raxis_value);		//vinod 16/02/2014 . . to set Temp_raxis_value bcz we need to display it also, and inbuilt fns implemented for x,y,z only
		Temp_raxis_value=0.0;										//again reset value of this to 0.0.
		//baseaction->allPoints.push_back(&baseaction->points[0]);
		baseaction->AllPointsList.Addpoint(new SinglePoint(baseaction->points[0].getX(), baseaction->points[0].getY(), baseaction->points[0].getZ(), baseaction->points[0].getR(), baseaction->points[0].getP()));
		// Getting Pixel position of Mouse Click.....
		baseaction->myPosition[0].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
		if(PPCALCOBJECT->IsPartProgramRunning())
			baseaction->ActionFinished(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0008", __FILE__, __FUNCSIG__); }
}

//draw the flexible crosshair for mouse move..
void MouseHandler::drawFlexibleCh()
{
	try
	{
		GRAFIX->SetColor_Double(0, 0, 1);
		if(MAINDllOBJECT->MouseOnVideo() && MAINDllOBJECT->DefaultCrosshairMode())
			GRAFIX->drawFlexibleCrossHair(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY());
		GRAFIX->SetColor_Double(1, 0, 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0010", __FILE__, __FUNCSIG__); }
}

//Handle left mouse down..
bool MouseHandler::LmouseDown_x(double x, double y, double z, double R, bool ConsiderZ, bool dontcheckucs) 
{
	try
	{
		bool MaximumClick = false;
		//bool IsprogramRunning = PPCALCOBJECT->IsPartProgramRunning();
		//if(!IsprogramRunning)
		//	MAINDllOBJECT->dontUpdateGraphcis = true;

		// Prevent taking points for anything within the tolerance width of a given point! by R A N 05 Sep 2018.
		//Vector* currPt = new Vector(x, y, z, R);
		double ptResolution = MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX();
		ptResolution = ptResolution / 2.0;

		if (PreventDoublePt)
		{
			PreventDoublePt = false;
			for (int j = 0; j < this->clicksdone; j++)
			{
				if (abs(this->clicks[j].getX() - x) < ptResolution && abs(this->clicks[j].getY() - y) < ptResolution) return false;
			}
		}

		if(ConsiderZ)
		{
			if(dontcheckucs || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				setMouseClick(x, y, z, dontcheckucs);
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				setMouseClick(x, z, y, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				setMouseClick(y, z, x, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				setMouseClick(x, -z, y, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				setMouseClick(y, -z, x, dontcheckucs);
		}
		else
		{
			if(dontcheckucs || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				setMouseClick(x, y, MAINDllOBJECT->getCurrentDRO().getZ(), dontcheckucs);
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				setMouseClick(x, MAINDllOBJECT->getCurrentDRO().getZ(), y, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				setMouseClick(y, MAINDllOBJECT->getCurrentDRO().getZ(), x, dontcheckucs);	
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				setMouseClick(x, -MAINDllOBJECT->getCurrentDRO().getZ(), y, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				setMouseClick(y, -MAINDllOBJECT->getCurrentDRO().getZ(), x, dontcheckucs);
		}
		clicksdone++;
		if(clicksdone >= maxclicks)
		{	
			clicksdone = 0;
			this->LmaxmouseDown();
			MaximumClick = true;
			ShapeThickness = 1;
		}
		else 
		{
			this->LmouseDown();
			if(clicksdone == 0)
				MaximumClick = true;
		}
		//if(!IsprogramRunning)
		//{
		//	MAINDllOBJECT->dontUpdateGraphcis = false;
		//	int w = MAINDllOBJECT->getCurrentWindow();
		//	MAINDllOBJECT->UpdateMousePosition(MAINDllOBJECT->getWindow(w).getLastWinMouse().x, MAINDllOBJECT->getWindow(w).getLastWinMouse().y, w);
		//}
		return MaximumClick;
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("MH0011", __FILE__, __FUNCSIG__); return false; }
}

//hanlde mouse move....//
void MouseHandler::mouseMove_x(double x, double y, double z , bool ConsiderZ, bool dontcheckucs)
{
	try
	{
		if(ConsiderZ)
		{
			if(dontcheckucs || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				setMouseClick(x, y, z, dontcheckucs);
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				setMouseClick(x, z, y, dontcheckucs);
			else  if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				setMouseClick(y, z, x, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				setMouseClick(x, -z, y, dontcheckucs);
			else  if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				setMouseClick(y, -z, x, dontcheckucs);
		}
		else
		{
			if(dontcheckucs || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				setMouseClick(x, y, MAINDllOBJECT->getCurrentDRO().getZ(), dontcheckucs);
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				setMouseClick(x, MAINDllOBJECT->getCurrentDRO().getZ(), y, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				setMouseClick(y, MAINDllOBJECT->getCurrentDRO().getZ(), x, dontcheckucs);	
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				setMouseClick(x, -MAINDllOBJECT->getCurrentDRO().getZ(), y, dontcheckucs);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				setMouseClick(y, -MAINDllOBJECT->getCurrentDRO().getZ(), x, dontcheckucs);
		}
		this->mouseMove();

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0011", __FILE__, __FUNCSIG__); }
}

//Left mouse down in Pixel coordinates..
void MouseHandler::L_mouseDownPixel(double x, double y)
{
	try{
		L_pixeldown.set(x, y); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0012", __FILE__, __FUNCSIG__); }
}

void MouseHandler::setMouseClick(double x, double y, double z, bool dontChceckStatus)
{
	if(!dontChceckStatus)
	{
		if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSXY)
		{
		    this->clicks[clicksdone].set(x, y, z);	
		}
	    else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSYZ)
		{
			this->clicks[clicksdone].set(z, x, y);	
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGYZ)
		{
			this->clicks[clicksdone].set(z, -x, y);
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSXZ)
		{
			this->clicks[clicksdone].set(x, z, y);
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGXZ)
		{
			this->clicks[clicksdone].set(-x, z, y);
		}
	}
	else
		this->clicks[clicksdone].set(x, y, z);
}

//Return the current left mouse down..pixel coordinates..
Vector& MouseHandler::L_mouseDownPixel()
{
	return L_pixeldown;
}

//Returns the click value..
Vector& MouseHandler::getClicksValue(int i)
{
	return clicks[i];
}

//Handle Left Mouse up..
void MouseHandler::LmouseUp(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0013", __FILE__, __FUNCSIG__); }
}

//Mouse click maximum
void MouseHandler::LmaxmouseDown()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0014", __FILE__, __FUNCSIG__); }
}

//Handle the Right mouse up
void MouseHandler::RmouseUp(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0015", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down...
void MouseHandler::RmouseDown(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0016", __FILE__, __FUNCSIG__); }
}

//Handle the middle mouse down..
void MouseHandler::MmouseDown(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0017", __FILE__, __FUNCSIG__); }
}

//Handle the middle mouse up..
void MouseHandler::MmouseUp(double x, double y)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0018", __FILE__, __FUNCSIG__); }
}

//handle the mouse scroll event..
void MouseHandler::MouseScroll(bool flag)
{
	try
	{
		if(flag) ShapeThickness += 0.2;
		else { if(ShapeThickness > 1) ShapeThickness -= 0.2; }
		MAINDllOBJECT->update_VideoGraphics(); MAINDllOBJECT->update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0019", __FILE__, __FUNCSIG__); }
}

//Handle the key up event..
void MouseHandler::keyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	try
	{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0020", __FILE__, __FUNCSIG__); }
}

//Handle the key up down..
void MouseHandler::keyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0021", __FILE__, __FUNCSIG__); }
}

//Handle partprogram data set..
void MouseHandler::PartProgramData()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0022", __FILE__, __FUNCSIG__); }
}

//Change the internal handler.. For Smart Measurements..
void MouseHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0023", __FILE__, __FUNCSIG__); }
}

//Set data from the front end..
void MouseHandler::SetAnyData(double *data)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0024", __FILE__, __FUNCSIG__); }
}