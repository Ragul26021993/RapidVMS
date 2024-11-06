#include "StdAfx.h"
#include "PerimeterHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Perimeter.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Actions\PerimeterAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"
#define _USE_MATH_DEFINES
#include <math.h>

PerimeterHandler::PerimeterHandler()
{
	init();
	MAINDllOBJECT->SetStatusCode("PerimeterHandler01");
}

PerimeterHandler::PerimeterHandler(Shape* ParentShape, int ChildId, bool flag)
{
	try
	{
		setMaxClicks(4);
		undoflag = flag;
		ShapeHighilghted = false; SnapPointHighlighted = false;
	    Edit_LastPoint = false;
		Pshape = (Perimeter*)ParentShape;
		if(undoflag)
		{
			bool ShapePresent = false;
			for(PSC_ITER ShpItr = Pshape->PerimeterShapecollection.begin(); ShpItr != Pshape->PerimeterShapecollection.end(); ShpItr++)
			{
				cshape = (*ShpItr).second;
				if(ChildId == cshape->getId())
				{
					ShapePresent = true;
					break;
				}
			}
			if(ShapePresent)
			{
				if(((ShapeWithList*)cshape)->PointsList->getList().size() == 1)
				{
					Pshape->TempShapecollection[cshape->getId()] = cshape;
					Pshape->RemoveShape(cshape);
				}
			}	
		}
		else
		{
			bool ShapePresent = false;
			for(PSC_ITER ShpItr = Pshape->TempShapecollection.begin(); ShpItr != Pshape->TempShapecollection.end(); ShpItr++)
			{
				cshape = (*ShpItr).second;
				if(ChildId == cshape->getId())
				{
					ShapePresent = true;
					break;
				}
			}
			if(ShapePresent)
			{
				Pshape->AddShape(cshape);
				Pshape->TempShapecollection.erase(cshape->getId());
				/*if(((ShapeWithList*)Cshape)->PointsList->getList().size() == 1)
				{
					Pshape->AddShape(Cshape);
					Pshape->TempShapecollection.erase(Cshape->getId());
				}*/
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0001", __FILE__, __FUNCSIG__); }
}

PerimeterHandler::PerimeterHandler(Shape *s)
{
	Pshape = (Perimeter*)s;
	Pflag = true;
	setMaxClicks(4);
}

PerimeterHandler::~PerimeterHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0002", __FILE__, __FUNCSIG__); }
}

void PerimeterHandler::init()
{
	try
	{
		resetClicks();
		Rflag = false; Pflag = false; undoflag = false;
		runningPartprogramValid = false; valid = false;
		ShapeHighilghted = false; SnapPointHighlighted = false;
	    Edit_LastPoint = false;
		cshape = NULL; Pshape = NULL; HighlightedShape = NULL;
		SelectedShapePointColl.clear();
		point3[0] = 0; point3[1] = 0;
		setMaxClicks(4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0001", __FILE__, __FUNCSIG__); }
}

//For each shape line/arc.. calculate the perimeter and diameter...
void PerimeterHandler::LmouseDown()
{
	try
	{
		valid = false;	
		Vector* pt;
		if(MAINDllOBJECT->highlightedPoint() != NULL)
			setClicksValue(getClicksDone() - 1, MAINDllOBJECT->highlightedPoint());
		if(SnapPointHighlighted)
			setClicksValue(getClicksDone() - 1, &Vector(SnapPointer->X, SnapPointer->Y, SnapPointer->Z));
		pt = &getClicksValue(getClicksDone() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			Pflag = true; Rflag = false;
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				bool incrementAction = false;
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				((ShapeWithList*)cshape)->ManagePoint(baseaction, ((ShapeWithList*)cshape)->ADD_POINT);	
				setClicksDone(1);	
				if(cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{				
					setClicksValue(0, pt->getX(), pt->getY(), pt->getZ());	
					if(((ShapeWithList*)cshape)->PointsList->getList().size() == 2)
						incrementAction = true;
				}
				else if(cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				{
					if(((ShapeWithList*)cshape)->PointsList->getList().size() == 3)
					{
						incrementAction = true;	
						SinglePoint* Spt = ((ShapeWithList*)cshape)->PointsList->getList()[1];
						pt->set(Spt->X, Spt->Y, Spt->Z);
						setClicksValue(0, Spt->X, Spt->Y, Spt->Z);
					}
					else if(((ShapeWithList*)cshape)->PointsList->getList().size() == 2)
					{
						setClicksValue(1, pt->getX(), pt->getY(), pt->getZ());	
						Rflag = true;
						setClicksDone(2);
					}				
				}
				if(incrementAction)
				{
					PPCALCOBJECT->IncrementStepForProbe();
					if(PPCALCOBJECT->getCurrentAction() != NULL)
					{
						if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::PERIMETERACTION)
						{
							cshape = ((PerimeterAction*)PPCALCOBJECT->getCurrentAction())->getShape();																						
							setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
							((ShapeWithList*)cshape)->ManagePoint(baseaction, ((ShapeWithList*)cshape)->ADD_POINT);
						}
					}
				}				
				Pshape->UpdateBestFit();
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(!Pflag)
		{
			if(!PPCALCOBJECT->IsPartProgramRunning())
			{
				Pshape = new Perimeter();
				Pshape->TotalPerimeter(0);
				Pshape->Diameter(0);
				AddShapeAction::addShape(Pshape);
			}
			MAINDllOBJECT->CurrentShapeProperties(0, 0, 0, 0, 5);
		}
		Pflag = true;

		
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() == 2)
		{
			Rflag = false;
			//add a line with the two points
			if(!undoflag)
			{
				cshape = new Line(false);
				cshape->ShapeForPerimeter(true);
			}
			((Line*)cshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
			if(!undoflag)
				PerimeterAction::AddPerimeterAction(Pshape, (ShapeWithList*)cshape, firstClick);
			undoflag = false;
			PerimeterAction::AddPerimeterAction(Pshape, (ShapeWithList*)cshape, baseaction);
			this->setClicksDone(1);
			setClicksValue(0, &getClicksValue(1));
			setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
			firstClick = baseaction;
			//add shape to perimeter shapecollection....
			Pshape->AddShape(cshape);
			Pshape->UpdateBestFit();
		}
		else if(getClicksDone() == 3)
			LmaxmouseDown();
		else
		{
			firstClick = baseaction;
			undoflag = false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0002", __FILE__, __FUNCSIG__); }
}

void PerimeterHandler::LmouseUp(double x, double y)
{
	try
	{
		SelectedShapePointColl.clear();
		Edit_LastPoint = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the right mouse down..
void PerimeterHandler::RmouseDown(double x, double y)
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
				cshape->ShapeForPerimeter(true);
			}
			else if(cshape != NULL)
			{
				if(cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					MAINDllOBJECT->undoforFT(2);
					cshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
					undoflag = false;
				}
			}
			((Circle*)cshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
			if(!undoflag)
				PerimeterAction::AddPerimeterAction(Pshape, (ShapeWithList*)cshape, firstClick);
			PerimeterAction::AddPerimeterAction(Pshape, (ShapeWithList*)cshape, rightClick);	
			MAINDllOBJECT->dontUpdateGraphcis = false;
		}

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0004", __FILE__, __FUNCSIG__); MAINDllOBJECT->dontUpdateGraphcis = false;}
}

//Handle the middle mousedown.. end the perimeter...
void PerimeterHandler::MmouseDown(double x, double y)
{
	try
	{
		MAINDllOBJECT->OnLeftMouseDown_Video(int(x), int(y));
		Pshape->UpdateBestFit();
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				PPCALCOBJECT->partProgramReached();
			}
		}
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mousemove...//
void PerimeterHandler::mouseMove()
{
	try
	{
		if(Pshape == NULL) return;
		Vector* pt = &getClicksValue(getClicksDone());
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY(), pt->getZ());
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
			RMATH2DOBJECT->Arc_3Pt(&tp1[0], &tp2[0], &tp3[0], &center[0], &radius, &startangle, &sweepangle);
		}
		ShapeHighilghted = false; SnapPointHighlighted = false;
		Edit_LastPoint = false;
		HighlightedShape = NULL; SnapPointer = NULL;
		getNearestPoint(Pshape->PerimeterShapecollection, pt->getX(), pt->getY(), MAINDllOBJECT->getWindow(0).getUppX() * SNAPDIST);
		if(SnapPointer != NULL)
		{
			SnapPointHighlighted = true;
		}
		else
		{
			Shape* nshape = getNearestShape(Pshape->PerimeterShapecollection, pt->getX(), pt->getY(), MAINDllOBJECT->getWindow(0).getUppX() * SNAPDIST);
			if(nshape != NULL)
			{
				HighlightedShape = nshape;
				ShapeHighilghted = true;
			}
		}
		valid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0006", __FILE__, __FUNCSIG__); }
}

Shape* PerimeterHandler::getNearestShape(map<int,Shape*> Shapecollection, double x, double y, double snapdist)
{
	try
	{
		Vector c1;
		c1.set(x, y);
		for(PSC_ITER pitem = Shapecollection.begin();  pitem != Shapecollection.end(); pitem++)
		{
			Shape* Csh = (Shape*)(*pitem).second;
			if(Csh->Shape_IsInWindow(c1, snapdist))
				return Csh;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0007", __FILE__, __FUNCSIG__); return NULL; }
}

void PerimeterHandler::getNearestPoint(map<int,Shape*> Shapecollection, double x, double y, double snapdist)
{
	try
	{
		SelectedShapePointColl.clear();
		for(PSC_ITER pitem = Shapecollection.begin();  pitem != Shapecollection.end(); pitem++)
		{
			ShapeWithList* Csh = (ShapeWithList*)((*pitem).second);
			PointCollection* Spcoll = Csh->PointsListOriginal;
			for(PC_ITER item = Csh->PointsListOriginal->getList().begin(); item != Csh->PointsListOriginal->getList().end(); item++)
			{
				SinglePoint* Spt = (*item).second;
				Vector Temp(Spt->X, Spt->Y, Spt->Z);
				if(abs(Spt->X - x) <= snapdist && abs(Spt->Y - y) <= snapdist)
				{
					SnapPointer = Spt;
					SelectedShapePointColl[Csh->getId()] = Spt->PtID;
					if(getClicksValue(0).operator == (Temp))
						Edit_LastPoint = true;
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0008", __FILE__, __FUNCSIG__); }
}

void PerimeterHandler::EscapebuttonPress()
{
	try
	{
		valid = false;
		Pshape->UpdateBestFit();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0009", __FILE__, __FUNCSIG__); }
}

//draw the perimeter..//
void PerimeterHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		drawFlexibleCh();
		if(Pshape == NULL) return;
		if(!valid) return;
		if(this->getClicksDone() == 1)
			GRAFIX->drawLine(getClicksValue(0).getX(), getClicksValue(0).getY(), point3[0], point3[1]);
		else if(this->getClicksDone() == 2 && Rflag)
			GRAFIX->drawArc(center[0], center[1], radius, startangle, sweepangle, WPixelWidth);
		if(ShapeHighilghted)
			drawSnapPointOfShape();
		if(SnapPointHighlighted)
			drawSnapPoint();
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0010", __FILE__, __FUNCSIG__); }
}

void PerimeterHandler::drawSnapPointOfShape()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(HighlightedShape != NULL)
		{
			switch(HighlightedShape->ShapeType)
			{
			case RapidEnums::SHAPETYPE::LINE:
				GRAFIX->drawPoint(((Line*)HighlightedShape)->getPoint1()->getX(), ((Line*)HighlightedShape)->getPoint1()->getY(), 0, 10);
				GRAFIX->drawPoint(((Line*)HighlightedShape)->getPoint2()->getX(), ((Line*)HighlightedShape)->getPoint2()->getY(), 0, 10);
				break;
			case RapidEnums::SHAPETYPE::ARC:
				GRAFIX->drawPoint(((Circle*)HighlightedShape)->getendpoint1()->getX(), ((Circle*)HighlightedShape)->getendpoint1()->getY(), 0, 10);
				GRAFIX->drawPoint(((Circle*)HighlightedShape)->getendpoint2()->getX(), ((Circle*)HighlightedShape)->getendpoint2()->getY(), 0, 10);
				GRAFIX->drawPoint(((Circle*)HighlightedShape)->getMidPoint()->getX(), ((Circle*)HighlightedShape)->getMidPoint()->getY(), 0, 10);
				break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0011", __FILE__, __FUNCSIG__); }
}

void PerimeterHandler::drawSnapPoint()
{
	try
	{
		GRAFIX->SetColor_Double(0, 0, 1);
		GRAFIX->drawPoint(SnapPointer->X, SnapPointer->Y, 0, 10);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0012", __FILE__, __FUNCSIG__); }
}

void PerimeterHandler::PartProgramData()
{
	try
	{
		cshape = ((PerimeterAction*)PPCALCOBJECT->getCurrentAction())->getShape();
		Pshape = (Perimeter*)(*cshape->getGroupParent().begin());
		if(((ShapeWithList*)cshape)->PointsList->getList().size() == 1)
			setClicksDone(1);
		else if(((ShapeWithList*)cshape)->PointsList->getList().size() == 2)
			setClicksDone(2);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0013", __FILE__, __FUNCSIG__); }
}

//Max mouse click
void PerimeterHandler::LmaxmouseDown()
{
	try
	{
		valid = false;
		undoflag = false;
		Vector* pt;
		if(MAINDllOBJECT->highlightedPoint() != NULL)
			setClicksValue(getClicksDone() - 1, MAINDllOBJECT->highlightedPoint());
		if(SnapPointHighlighted)
			setClicksValue(getClicksDone() - 1, &Vector(SnapPointer->X, SnapPointer->Y, SnapPointer->Z));
		pt = &getClicksValue(getClicksDone() - 1);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		Rflag = false;
		//add a line with the two points
		PerimeterAction::AddPerimeterAction(this->Pshape, (ShapeWithList*)cshape, baseaction);
		this->setClicksDone(1);
		setClicksValue( 0, &rightClick->points[0]);
		pt = &getClicksValue(0);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		firstClick = baseaction;
		//add shape to perimeter shapecollection....
		Pshape->AddShape(cshape);
		Pshape->UpdateBestFit();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0014", __FILE__, __FUNCSIG__); }
}

void PerimeterHandler::SettheClickvalue(int i, double x, double y, double z)
{
	try
	{
		setBaseRProperty(x, y, z);
		if(i == 0)
			firstClick = baseaction;
		else if(i == 1)
			rightClick = baseaction;

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERIMH0015", __FILE__, __FUNCSIG__); }
}
