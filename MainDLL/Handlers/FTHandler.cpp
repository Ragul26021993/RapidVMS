#include "StdAfx.h"
#include "FTHandler.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Measurement\DimBase.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\SnapPoint.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Helper\Helper.h"

//Constructor .. set the clicks required...//
FTHandler::FTHandler(bool FtType_Middle)
{
	try
	{
		undoflag = false; CurrentFTType_Mid = FtType_Middle;
		Rflag = false; EditShapeFor_Shiftdrag = false;
		Edit_LastPoint = false;
		cshape = NULL;
		setMaxClicks(4);
		/*MAINDllOBJECT->SetStatusCode("FT001");*/
		if(CurrentFTType_Mid)
		{
			MAINDllOBJECT->SetStatusCode("FTHandler01");
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER_ENDPT;
		}
		else
		{
			MAINDllOBJECT->SetStatusCode("FTHandler02");
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0001", __FILE__, __FUNCSIG__); }
}

FTHandler::FTHandler(bool FtType_Middle, ShapeWithList* csh, bool unflag)
{
	try
	{
		CurrentFTType_Mid = FtType_Middle;EditShapeFor_Shiftdrag = false;
		Rflag = false; valid = false;
		cshape = csh;
		if(cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			Rflag = true;
		undoflag = unflag;
		setMaxClicks(4);
		if(CurrentFTType_Mid)
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER_ENDPT;
		else
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FTHandler::~FTHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move...//
void FTHandler::mouseMove()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone());
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY(), pt->getZ());
		if(EditShapeFor_Shiftdrag)
		{
			for each(RAction* Caction in CurrentActionCollection)
			{
				FramegrabBase* Cfg = ((AddPointAction*)Caction)->getFramegrab();
				Cfg->points[0].set(pt->getX(), pt->getY(), pt->getZ());
				(((AddPointAction*)Caction)->getShape())->Modify_Point((int)*Cfg->PointActionIdList.begin(), pt->getX(), pt->getY(), pt->getZ());
			}
			if(Edit_LastPoint)
			{
				setClicksValue(0, pt->getX(), pt->getY(), pt->getZ());
				firstClick->points[0].set(pt->getX(), pt->getY(), pt->getZ());
				firstClick->AllPointsList.getList()[0]->SetValues(pt->getX(), pt->getY(), pt->getZ());
			}
			MAINDllOBJECT->SelectedShape_Updated();
			return;
		}
		if(this->getClicksDone() == 1)
		{
			double point1[2] = {getClicksValue(0).getX(), getClicksValue(0).getY()},point2[2] = {getClicksValue(1).getX(), getClicksValue(1).getY()}, angle,intercept,length;
			point3[0] = getClicksValue(1).getX();  point3[1] = getClicksValue(1).getY();
			angle = RMATH2DOBJECT->ray_angle(&point1[0], &point2[0]);
			if(MAINDllOBJECT->ShiftON)
				RMATH2DOBJECT->LineForShiftON(angle,&point1[0],&point2[0], &point3[0],&angle, &intercept);
			else
				RMATH2DOBJECT->Intercept_LinePassing_Point(&point1[0], angle,&intercept);
			length = RMATH2DOBJECT->Pt2Pt_distance(&point1[0], &point3[0]);
			if(angle > M_PI) angle -= M_PI;
			MAINDllOBJECT->CurrentShapeProperties(intercept,angle,length,0,0);
		}
		if(this->getClicksDone() == 2 && Rflag)
		{
			if(getClicksValue(1).operator==(getClicksValue(2))) return;
			double tp1[2] = {getClicksValue(0).getX(), getClicksValue(0).getY()}, tp2[2] = {getClicksValue(1).getX(), getClicksValue(1).getY()},
				tp3[2] = {getClicksValue(2).getX(), getClicksValue(2).getY()};
			if(CurrentFTType_Mid)
				RMATH2DOBJECT->Arc_3Pt(&tp1[0], &tp2[0], &tp3[0], &center[0], &radius, &startangle, &sweepangle);
			else
				RMATH2DOBJECT->Arc_3Pt(&tp1[0], &tp3[0], &tp2[0], &center[0], &radius, &startangle, &sweepangle);
			MAINDllOBJECT->CurrentShapeProperties(center[0], center[1],radius,sweepangle,4);
		}
		valid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0004", __FILE__, __FUNCSIG__); }
}

void FTHandler::SettheClickvalue(int i, double x, double y, double z)
{
	try
	{
		setBaseRProperty(x, y, z);
		if(i == 0)
			firstClick = baseaction;
		else if(i == 1)
			rightClick = baseaction;

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down...//
//Need to do.. (add the line for 1st click , if the 2nd clickis is left, add the points to the current shape.
//else delete line and instantiate arc and to action...)//
void FTHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->ShiftON && MAINDllOBJECT->highlightedPoint() != NULL)
		{
			setClicksDone(getClicksDone() - 1);
			list<SnapPoint*> CSpShapeSnapPoint = MAINDllOBJECT->getCurrentUCS().getIPManager().AllSnapPoints;
			MAINDllOBJECT->DeselectAll(false);
			for each(SnapPoint* Csnp in CSpShapeSnapPoint)
			{
				ShapeWithList* Csh = (ShapeWithList*)Csnp->getParent();
				Vector* snappt = Csnp->getPt();
				if(Csh->ShapeForCurrentFastrace())
				{
					MAINDllOBJECT->selectShape(Csh->getId(), true);
					for each(RAction* Caction in Csh->PointAtionList)
					{
						FramegrabBase* Cfg = ((AddPointAction*)Caction)->getFramegrab();
						if(snappt->operator==(Cfg->points[0]))
						{
							CurrentActionCollection.push_back(Caction);
							if(getClicksValue(0).operator==(Cfg->points[0]))
								Edit_LastPoint = true;
						}
					}
					TempClick.SetValues(snappt->getX(), snappt->getY(), snappt->getZ());
				}
			}
			if(CurrentActionCollection.size() > 0)
				EditShapeFor_Shiftdrag = true;
			return;
		}
		valid = false;
		Vector* pt;
		if(MAINDllOBJECT->highlightedPoint() != NULL)
			setClicksValue(getClicksDone() - 1, MAINDllOBJECT->highlightedPoint());
		if(MAINDllOBJECT->ShiftON)
			setClicksValue(getClicksDone() - 1, new Vector(point3[0],point3[1],0 ));
		pt = &getClicksValue(getClicksDone() - 1);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() == 2)
		{
			Rflag = false;
			//add a line with the two points
			if(!undoflag)
			{
				cshape = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
				cshape->ShapeForCurrentFastrace(true);
				cshape->ShapeAsfasttrace(true);
				MAINDllOBJECT->CurrentFTShapeCollection.push_back(cshape);
			}
			else if(cshape != NULL)
			{
				if(cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				{
					MAINDllOBJECT->undoforFT(2);
					Sleep(50);
					cshape = new Line();
					undoflag = false;
				}
			}
			((Line*)cshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
			((Line*)cshape)->FastraceType_Mid(CurrentFTType_Mid);

			if(!undoflag)
			{
				AddPointAction::pointAction((ShapeWithList*)cshape, firstClick);
				AddShapeAction::addShape(cshape);
			}
			cshape->ShapeThickness(ShapeThickness);
			ShapeThickness = 1;
			undoflag = false;
			Sleep(20);
			AddPointAction::pointAction((ShapeWithList*)cshape, baseaction);
			this->setClicksDone(1);
			setClicksValue(0, &getClicksValue(1));
			setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
			firstClick = baseaction;
			HELPEROBJECT->AddFgShapeRelationShip(MAINDllOBJECT->getShapesList(), cshape);
		}
		else if(getClicksDone() == 3)
			LmaxmouseDown();
		else
			firstClick = baseaction;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0006", __FILE__, __FUNCSIG__); }
}


void FTHandler::LmouseUp(double x, double y)
{
	try
	{
		EditShapeFor_Shiftdrag = false;
		Edit_LastPoint = false;
		for each(RAction* Caction in CurrentActionCollection)
		{
			FramegrabBase* Cfg = ((AddPointAction*)Caction)->getFramegrab();
			Shape* Csh = ((AddPointAction*)Caction)->getShape();
			Csh->notifyAll(ITEMSTATUS::DATACHANGED, Csh);
		}
		CurrentActionCollection.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0007", __FILE__, __FUNCSIG__); }
}

//Handle the right mouse down.....
void FTHandler::RmouseDown(double x, double y)
{
	try
	{
		if(MAINDllOBJECT->ControlOn)
		{
			double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
			double upp_x = MAINDllOBJECT->getWindow(0).getUppX(), upp_y = MAINDllOBJECT->getWindow(0).getUppY();
			MAINDllOBJECT->ControlOn = false;
			R_Point Cpt = MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).GetMouseDown();
			MmouseDown((x - cx)/upp_x, (cy - y)/upp_y);
			MAINDllOBJECT->ControlOn = true;
			return;
		}
		if(getClicksDone() == 1)
		{
			MAINDllOBJECT->dontUpdateGraphcis = true;
			Rflag = true;
			valid = false;
			setClicksDone(2);
			setBaseRProperty(getClicksValue(1).getX(), getClicksValue(1).getY(), getClicksValue(1).getZ());
			rightClick = baseaction;
			if(!undoflag)
			{
				cshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				cshape->ShapeForCurrentFastrace(true);
				cshape->ShapeAsfasttrace(true);
				MAINDllOBJECT->CurrentFTShapeCollection.push_back(cshape);
			}
			else if(cshape != NULL)
			{
				if(cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					MAINDllOBJECT->undoforFT(2);
					Sleep(50);
					cshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
					undoflag = false;
				}
			}
			
			if(CurrentFTType_Mid)
				((Circle*)cshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
			else
				((Circle*)cshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
			if(!undoflag)
			{
				AddPointAction::pointAction((ShapeWithList*)cshape, firstClick);
				Sleep(20);
				AddShapeAction::addShape(cshape);
			}
			AddPointAction::pointAction((ShapeWithList*)cshape, rightClick);
			Sleep(20);
			radius = 0; 
			startangle = 0; 
			sweepangle = 0;
			MAINDllOBJECT->dontUpdateGraphcis = false;
		}

	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0008", __FILE__, __FUNCSIG__); }
}


void FTHandler::EscapebuttonPress()
{
	try
	{
		valid = false;
		if(Rflag)
		{
			Rflag = false;
			MAINDllOBJECT->deleteForEscape();
		}
		for each(Shape* CSh in MAINDllOBJECT->CurrentFTShapeCollection)
		{
			CSh->ShapeForCurrentFastrace(false);
			if(CSh->ShapeType ==  RapidEnums::SHAPETYPE::ARC)
				((Circle*)CSh)->calculateAttributes();
		}
		MAINDllOBJECT->CurrentFTShapeCollection.clear();
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0009", __FILE__, __FUNCSIG__); }
}

//draw the fast trace...//
void FTHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		drawFlexibleCh();
		GRAFIX->SetGraphicsLineWidth(ShapeThickness);
		if(EditShapeFor_Shiftdrag)
			GRAFIX->drawLineStipple(TempClick.X, TempClick.Y,  getClicksValue(1).getX(), getClicksValue(1).getY());
		else
		{
			if(valid)
			{
				if(this->getClicksDone() == 1)
				{
					//GRAFIX->drawCrossMark(getClicksValue(0).getX(), getClicksValue(0).getY(), 0);
					GRAFIX->drawLine(getClicksValue(0).getX(), getClicksValue(0).getY(),point3[0], point3[1]);
				}
				else if(this->getClicksDone() == 2 && Rflag)
				{
					//GRAFIX->drawCrossMark(getClicksValue(0).getX(), getClicksValue(0).getY(), 0);
					//GRAFIX->drawCrossMark(getClicksValue(1).getX(), getClicksValue(1).getY(), 0);
					GRAFIX->drawArc(center[0], center[1], radius, startangle, sweepangle, WPixelWidth);
				}
			}
		}
		GRAFIX->SetGraphicsLineWidth(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0010", __FILE__, __FUNCSIG__); }
}

//handle the middle mouse down...
//end the fast trace...//
void FTHandler::MmouseDown(double x, double y)
{
	try
	{
		MAINDllOBJECT->OnLeftMouseDown_Video(int(x), int(y));
		for each(Shape* CSh in MAINDllOBJECT->CurrentFTShapeCollection)
		{
			CSh->ShapeForCurrentFastrace(false);
			if(CSh->ShapeType ==  RapidEnums::SHAPETYPE::ARC)
				((Circle*)CSh)->calculateAttributes();
		}
		MAINDllOBJECT->CurrentFTShapeCollection.clear();
		resetClicks();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0011", __FILE__, __FUNCSIG__); }
}
	

void FTHandler::setPoint(FramegrabBase* actn, int i)
{
	try
	{
		setClicksValue( i, &actn->points[0]);
		if(i == 0)
		{
			baseaction = actn;
			//setBaseRProperty(actn);
			baseaction->AllPointsList.Addpoint(new SinglePoint(baseaction->points[0].getX(), baseaction->points[0].getY(), baseaction->points[0].getZ()));
			firstClick = baseaction;
		}
		else if(i == 1)
		{
			baseaction = actn;
			//setBaseRProperty(actn);
			baseaction->AllPointsList.Addpoint(new SinglePoint(baseaction->points[0].getX(), baseaction->points[0].getY(), baseaction->points[0].getZ()));
			rightClick = baseaction;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0012", __FILE__, __FUNCSIG__); }

}
void FTHandler::setRflag( bool flag)
{
	Rflag = flag;
}

void FTHandler::LmaxmouseDown()
{
	try
	{
		valid = false;
		undoflag = false;
		Vector* pt;
		if(MAINDllOBJECT->highlightedPoint() != NULL)
			setClicksValue(getClicksDone() - 1, MAINDllOBJECT->highlightedPoint());
		pt = &getClicksValue(getClicksDone() - 1);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		Rflag = false;
		//add a line with the two points
		cshape->ShapeThickness(ShapeThickness);
		AddPointAction::pointAction((ShapeWithList*)cshape, baseaction);
		this->setClicksDone(1);
		if(CurrentFTType_Mid)
			setClicksValue( 0, &rightClick->points[0]);
		else
			setClicksValue( 0, &baseaction->points[0]);
		pt = &getClicksValue(0);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		firstClick = baseaction;
		if(CurrentFTType_Mid)
			firstClick->myPosition[0] = rightClick->myPosition[0];
		HELPEROBJECT->AddFgShapeRelationShip(MAINDllOBJECT->getShapesList(), cshape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FTMH0013", __FILE__, __FUNCSIG__); }
}