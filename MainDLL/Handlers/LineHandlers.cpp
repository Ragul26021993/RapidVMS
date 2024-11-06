#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "LineHandlers.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Line.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
//According to the line type, set the clicks required and set the required flags....//
LineHandlers::LineHandlers(RapidEnums::LINETYPE d)
{
	try
	{
		CurrentLineType = d;
		this->CurrentWindow = -1;
		AddNewLine();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0001", __FILE__, __FUNCSIG__); }
}

LineHandlers::LineHandlers(RapidEnums::LINETYPE d, ShapeWithList* sh, bool flag)
{
	try
	{
		CurrentLineType = d;
		this->CurrentWindow = -1;
		runningPartprogramValid = false;
		setMaxClicks(2);
		myshape = sh;
		valid = flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0002", __FILE__, __FUNCSIG__); }
}

//Used for partprogram...//
LineHandlers::LineHandlers()
{
	try
	{
		ShapeThickness = 1;
		runningPartprogramValid = false;
		this->CurrentWindow = -1;
		CurrentShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		myshape = (ShapeWithList*)CurrentShape;
		CurrentLineType = ((Line*)myshape)->LineType;
		FirstClick = true;
		valid = false;
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0003", __FILE__, __FUNCSIG__); }
}

//Desructor/..////
LineHandlers::~LineHandlers()
{ 
}

void LineHandlers::AddNewLine()
{
	try
	{
		FirstClick = true;
		runningPartprogramValid = false;
		valid = false;
		switch(CurrentLineType)
		{
			case RapidEnums::LINETYPE::FINITELINE:
				myshape = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
				((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
				setMaxClicks(2);
				/*MAINDllOBJECT->SetStatusCode("L001");*/
				MAINDllOBJECT->SetStatusCode("LineHandlers01");
				break;

			case RapidEnums::LINETYPE::INFINITELINE:
				myshape = new Line(_T("XL"), RapidEnums::SHAPETYPE::XLINE);
				((Line*)myshape)->LineType = RapidEnums::LINETYPE::INFINITELINE;
				setMaxClicks(2);
				/*MAINDllOBJECT->SetStatusCode("L005");*/
				MAINDllOBJECT->SetStatusCode("LineHandlers02");
				break;

			case RapidEnums::LINETYPE::RAYLINE:
				myshape = new Line(_T("R"), RapidEnums::SHAPETYPE::XRAY);
				((Line*)myshape)->LineType = RapidEnums::LINETYPE::RAYLINE;
				setMaxClicks(2);
				/*MAINDllOBJECT->SetStatusCode("L003");*/
				MAINDllOBJECT->SetStatusCode("LineHandlers03");
				break;

			case RapidEnums::LINETYPE::FINITELINE3D:
				myshape = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
				((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE3D;
				setMaxClicks(2);
				/*MAINDllOBJECT->SetStatusCode("L001");*/
				MAINDllOBJECT->SetStatusCode("LineHandlers04");
				break;
			case RapidEnums::LINETYPE::INFINITELINE3D:
				myshape = new Line(_T("XL3D"), RapidEnums::SHAPETYPE::XLINE3D);
				((Line*)myshape)->LineType = RapidEnums::LINETYPE::INFINITELINE3D;
				setMaxClicks(2);
				/*MAINDllOBJECT->SetStatusCode("L001");*/
				MAINDllOBJECT->SetStatusCode("LineHandlers04");
				break;
		}
		AddShapeAction::addShape(myshape);
		CurrentShape = myshape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mousemove...//
void LineHandlers::mouseMove()
{
	try
	{
		//if(getClicksDone() == 0)return;
		double intercept = 0, length = 0;
		Vector* pt = &getClicksValue(getClicksDone());
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() == 1)
		{
			valid = true;
			myshape->IsValid(false);
			double point1[3] = {getClicksValue(0).getX(), getClicksValue(0).getY(), MAINDllOBJECT->getCurrentDRO().getZ()};
			point3[0] = pt->getX(); point3[1] = pt->getY(); point3[2] = MAINDllOBJECT->getCurrentDRO().getZ();	
			if(!PPCALCOBJECT->IsPartProgramRunning())
			{
				point2[0] = pt->getX(); point2[1] = pt->getY();
				angle = RMATH2DOBJECT->ray_angle(&point1[0], &point2[0]);
				if(MAINDllOBJECT->ShiftON && MAINDllOBJECT->highlightedShape() != NULL)
				{
					RMATH2DOBJECT->LineForShiftON(angle,&point1[0],&point2[0], &point3[0],&angle, &intercept);
					HELPEROBJECT->IntersectionPointOnHighlightedLine(MAINDllOBJECT->highlightedShape(), angle, intercept, &point3[0]);
				}
				else if(MAINDllOBJECT->ShiftON)
					RMATH2DOBJECT->LineForShiftON(angle,&point1[0],&point2[0], &point3[0],&angle, &intercept);
				else
					RMATH2DOBJECT->Intercept_LinePassing_Point(&point1[0], angle,&intercept);
				switch(CurrentLineType)
				{
					case RapidEnums::LINETYPE::FINITELINE:
						length = RMATH2DOBJECT->Pt2Pt_distance(&point1[0], &point3[0]);
						if(angle > M_PI) angle -= M_PI;
						MAINDllOBJECT->CurrentShapeProperties(intercept,angle,length,0,0);
						((Line*)myshape)->setLineParameters(angle, intercept);
						((Line*)myshape)->setPoint1(Vector(point1[0], point1[1], point1[2]));
						((Line*)myshape)->setPoint2(Vector(point3[0], point3[1], point1[2]));
						break;
					case RapidEnums::LINETYPE::INFINITELINE:
						if(angle > M_PI) angle -= M_PI;
						MAINDllOBJECT->CurrentShapeProperties(intercept,angle,point1[2],0,2);
						break;
					case RapidEnums::LINETYPE::RAYLINE:
						MAINDllOBJECT->CurrentShapeProperties(intercept,angle,point1[2],0,3);
						break;
				}
			}
			else
			{
				if(CurrentLineType == RapidEnums::LINETYPE::FINITELINE && myshape->getMchild().size() > 0 && MAINDllOBJECT->VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS)
				{
					((Line*)myshape)->setPoint1(Vector(point1[0], point1[1], point1[2]));
					((Line*)myshape)->setPoint2(Vector(point3[0], point3[1], point1[2]));					
					((Line*)myshape)->calculateAttributes();
					myshape->notifyAll(ITEMSTATUS::DATACHANGED, myshape);	
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mousedown event..//
void LineHandlers::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				myshape->ManagePoint(baseaction, myshape->ADD_POINT);
				if(!PPCALCOBJECT->ProgramAutoIncrementMode())
					PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 1 && !FirstClick)
		{
			valid = false;
			switch(CurrentLineType)
			{
				case RapidEnums::LINETYPE::FINITELINE:
					myshape = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
					((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
					break;

				case RapidEnums::LINETYPE::INFINITELINE:
					myshape = new Line(_T("XL"), RapidEnums::SHAPETYPE::XLINE);
					((Line*)myshape)->LineType = RapidEnums::LINETYPE::INFINITELINE;
					break;

				case RapidEnums::LINETYPE::RAYLINE:
					myshape = new Line(_T("R"), RapidEnums::SHAPETYPE::XRAY);
					((Line*)myshape)->LineType = RapidEnums::LINETYPE::RAYLINE;
					break;

				case RapidEnums::LINETYPE::FINITELINE3D:
					myshape = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
					((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE3D;
					break;
				case RapidEnums::LINETYPE::INFINITELINE3D:
					myshape = new Line(_T("L3D"), RapidEnums::SHAPETYPE::XLINE3D);
					((Line*)myshape)->LineType = RapidEnums::LINETYPE::INFINITELINE3D;
					break;
			}
			CurrentShape = myshape;
			AddShapeAction::addShape(myshape);
			Sleep(2);
		}
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(baseaction->CurrentWindowNo == 1)
			baseaction->DroMovementFlag = false;
		if(getClicksDone() < getMaxClicks())
			AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
		switch(CurrentLineType)
		{
		case RapidEnums::LINETYPE::FINITELINE3D:
		case RapidEnums::LINETYPE::FINITELINE:
			MAINDllOBJECT->SetStatusCode("L002");
			break;

		case RapidEnums::LINETYPE::INFINITELINE:
			MAINDllOBJECT->SetStatusCode("L006");
			break;

		case RapidEnums::LINETYPE::RAYLINE:
			MAINDllOBJECT->SetStatusCode("L004");
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0006", __FILE__, __FUNCSIG__); }
}

void LineHandlers::RmouseDown(double x,double y)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			if(MAINDllOBJECT->SmartMeasureType != RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE) return;
			if(getClicksDone() == 0)
				AddNewLine();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0007", __FILE__, __FUNCSIG__); }
}

void LineHandlers::draw(int windowno, double WPixelWidth)
{
	try
	{
		drawFlexibleCh();
		if(valid)
		{
			GRAFIX->SetGraphicsLineWidth(ShapeThickness);
			//if(this->getClicksDone() < getMaxClicks() && getClicksDone() >= 1)
			//	GRAFIX->drawPoint(getClicksValue(0).getX(), getClicksValue(0).getY(), 0 );
			switch(CurrentLineType)
			{
			case  RapidEnums::LINETYPE::MULTIPOINTSLINEPARALLEL2LINE:
				GRAFIX->drawLine_3D(getClicksValue(0).getX(),getClicksValue(0).getY(), MAINDllOBJECT->getCurrentDRO().getZ() ,point3[0], point3[1], point3[2]);
				break;
			case  RapidEnums::LINETYPE::FINITELINE:
				GRAFIX->drawLine_3D(getClicksValue(0).getX(),getClicksValue(0).getY(), MAINDllOBJECT->getCurrentDRO().getZ() ,point3[0], point3[1], point3[2]);
				break;
			case  RapidEnums::LINETYPE::INFINITELINE:
				GRAFIX->drawXLineOrXRay(angle, getClicksValue(0).getX(), getClicksValue(0).getY(), WPixelWidth, true, MAINDllOBJECT->getCurrentDRO().getZ());
				break;
			case  RapidEnums::LINETYPE::RAYLINE:
				GRAFIX->drawXLineOrXRay(angle, getClicksValue(0).getX(), getClicksValue(0).getY(), WPixelWidth, false, MAINDllOBJECT->getCurrentDRO().getZ());
				break;
			case RapidEnums::LINETYPE::FINITELINE3D:
				GRAFIX->drawLine_3D(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(0).getZ(), getClicksValue(1).getX(), getClicksValue(1).getY(), getClicksValue(1).getZ());
				break;
			}
			GRAFIX->SetGraphicsLineWidth(1);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0008", __FILE__, __FUNCSIG__); }
}

void LineHandlers::SetAngle(double ang)
{
	angle = ang;
}

//Handle the partprogram data...//
void LineHandlers::PartProgramData()
{
	int num = 0;
	try
	{
		myshape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());
		num++;
		setMaxClicks(2);
		num++;
		switch(myshape->ShapeType)
		{
		case RapidEnums::SHAPETYPE::LINE:
			if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
			{
				num += 10;
				baseaction = PPCALCOBJECT->getFrameGrab();
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				num++;
				myshape->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				num++;
				setClicksValue(getClicksDone(), baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				setClicksDone(getClicksDone() + 1);
				num++;
				return;
			}
			CurrentLineType = RapidEnums::LINETYPE::FINITELINE;
			break;
		case RapidEnums::SHAPETYPE::LINE3D:
			if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
			{
				num += 20;
				baseaction = PPCALCOBJECT->getFrameGrab();
				num++;
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				num++;
				myshape->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				num++;
				setClicksValue(getClicksDone(), baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				setClicksDone(getClicksDone() + 1);
				num++;
				return;
			}
			CurrentLineType = RapidEnums::LINETYPE::FINITELINE3D;
			break;
		case RapidEnums::SHAPETYPE::XLINE3D:
			if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
			{
				num += 30;
				baseaction = PPCALCOBJECT->getFrameGrab();
				num++;
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				num++;
				myshape->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				num++;
				setClicksValue(getClicksDone(), baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				setClicksDone(getClicksDone() + 1);
				num++;
				return;
			}
			CurrentLineType = RapidEnums::LINETYPE::INFINITELINE3D;
			break;
		case RapidEnums::SHAPETYPE::XLINE:
			if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
			{
				num += 40;
				baseaction = PPCALCOBJECT->getFrameGrab();
				num++;
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				num++;
				myshape->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				num++;
				setClicksValue(getClicksDone(), baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				setClicksDone(getClicksDone() + 1);
				num++;
				return;
			}
			CurrentLineType = RapidEnums::LINETYPE::INFINITELINE;
			break;
		case RapidEnums::SHAPETYPE::XRAY:
			if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
			{
				num += 50;
				baseaction = PPCALCOBJECT->getFrameGrab();
				num++;
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				num++;
				myshape->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				num++;
				setClicksValue(getClicksDone(), baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				setClicksDone(getClicksDone() + 1);
				num++;
				return;
			}
			CurrentLineType = RapidEnums::LINETYPE::RAYLINE;
			break;
		}
		num++;
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0009" + std::to_string(num), __FILE__, __FUNCSIG__); }
}

void LineHandlers::EscapebuttonPress()
{
	try
	{
		valid = false;
		FirstClick = false;
		if(getClicksDone() > 0)
		{
			MAINDllOBJECT->deleteShape(); // deleteForEscape();
		}
		switch(CurrentLineType)
		{
		case RapidEnums::LINETYPE::FINITELINE3D:
		case RapidEnums::LINETYPE::FINITELINE:
			MAINDllOBJECT->SetStatusCode("L001");
			break;

		case RapidEnums::LINETYPE::INFINITELINE:
			MAINDllOBJECT->SetStatusCode("L005");
			break;

		case RapidEnums::LINETYPE::RAYLINE:
			MAINDllOBJECT->SetStatusCode("L003");
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0010", __FILE__, __FUNCSIG__); }
}

//Handle the Maximum mouse click...//
void LineHandlers::LmaxmouseDown()
{
	try
	{
		myshape->ShapeThickness(ShapeThickness);
		FirstClick = false;
		Vector pt(point3[0],point3[1],getClicksValue(getMaxClicks() - 1).getZ());
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(pt.getX(), pt.getY(), pt.getZ());
				myshape->ManagePoint(baseaction, myshape->ADD_POINT);
				valid = false;
				if(!PPCALCOBJECT->ProgramAutoIncrementMode())
					PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}
		else
		{
			setBaseRProperty(pt.getX(), pt.getY(), pt.getZ());
			if(baseaction->CurrentWindowNo == 1)
				baseaction->DroMovementFlag = false;
			AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
		}
		valid = false;
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0011", __FILE__, __FUNCSIG__); }
}

void LineHandlers::UpdateRepeatShapeAction(FramegrabBase* fb)
{
	try
	{
		myshape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());
		if(myshape == NULL) return;
		switch(myshape->ShapeType)
		{
		case RapidEnums::SHAPETYPE::LINE:
		case RapidEnums::SHAPETYPE::LINE3D:
		case RapidEnums::SHAPETYPE::XLINE3D:
		case RapidEnums::SHAPETYPE::XLINE:
		case RapidEnums::SHAPETYPE::XRAY:	
			setBaseRProperty(fb->Pointer_SnapPt->getX(), fb->Pointer_SnapPt->getY(), fb->Pointer_SnapPt->getZ());
			break;
		}
		myshape->AddPoints(&baseaction->AllPointsList);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0012", __FILE__, __FUNCSIG__); }
}