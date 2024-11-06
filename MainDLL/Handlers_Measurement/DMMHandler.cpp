#include "StdAfx.h"
#include "DMMHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddPointAction.h"
#include "..\Measurement\DimDigitalMicrometer.h"
#include "..\Shapes\DummyShape.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

DMMHandler::DMMHandler()
{
	try
	{
		setMaxClicks(2);
		init();
		MAINDllOBJECT->SetStatusCode("DMMHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0001", __FILE__, __FUNCSIG__); }
}

DMMHandler::~DMMHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0002", __FILE__, __FUNCSIG__); }
}

void DMMHandler::init()
{
	try
	{
		dim = NULL;
		PointCount = 0;
		Cshape[0] = NULL;
		Cshape[1] = NULL;
		Cshape[2] = NULL;
		this->DmMeasureType = 0;
		this->DmMeasureTypeLinear = MAINDllOBJECT->DMMMeasureTypeLinear();
		this->runningPartprogramValid = false;
		this->MeasureDir = true;
		this->DrawNearMousePoint = true;
		this->FloatFormouseMove = true;
		this->HorizontalMeasure = true;
		this->CheckIntesectFlag = false;
		NoofDec = MAINDllOBJECT->MeasurementNoOfDecimal();
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0003", __FILE__, __FUNCSIG__); }
}



void DMMHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		ClickValue[getClicksDone() - 1].set(pt->getX(), pt->getY(), pt->getZ());
		point1[0] = getClicksValue(0).getX(); point1[1] = getClicksValue(0).getY();
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				FirstAction_DMM = baseaction;
				PointCount++;
				if (Cshape[1] != NULL && Cshape[2] != NULL)
					CheckIntesectFlag = HELPEROBJECT->PointOnHighlightedShapeTest(Cshape[1], &point1[0], Cshape[2], &point3[0]);
				if(CheckIntesectFlag)
					ClickValue[3].set(point3[0], point3[1], MAINDllOBJECT->getCurrentDRO().getZ());
				else
					ClickValue[3].set(point1[0], point1[1], MAINDllOBJECT->getCurrentDRO().getZ());
				PPCALCOBJECT->partProgramReached();

			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 1)
		{
			Cshape[0] = new DummyShape();
			CurrentShape = Cshape[0];
			AddShapeAction::addShape(Cshape[0]);
		}
		setBaseRProperty(point1[0], point1[1], pt->getZ());
		DmMeasureType = 0;
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			if(MAINDllOBJECT->getCurrentWindow() == 1)
				baseaction->DerivedShape = true;
			if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL)
				Cshape[1] = MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1();
		}
		if(MAINDllOBJECT->highlightedShape() != NULL)
			Cshape[1] = MAINDllOBJECT->highlightedShape();
		
		PointCount++;
		MAINDllOBJECT->SetStatusCode("DMM002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0004", __FILE__, __FUNCSIG__); }
}

void DMMHandler::mouseMove()
{
	try
	{
		if(getClicksDone() == 0)return;
		Vector* pt = &getClicksValue(getClicksDone());
		if(PointCount != 0)
		{
			CalculateDigitalMicrometer(pt->getX(), pt->getY(), pt->getZ());
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0005", __FILE__, __FUNCSIG__); }
}



void DMMHandler::draw(int windowno, double WPixelWidth)
{
	try
	{		
         ClickValue[1].set(point3[0], point3[1], MAINDllOBJECT->getCurrentDRO().getZ());        		
		double FScalefact;
		if(windowno == 0) FScalefact = 20;
		else FScalefact = 20;
		double TxtLeftTop_Rcad[6] = {0}, TxtRightBtm_Rcad[3]= {0};
		if(PointCount != 0)
		{
			if(DrawNearMousePoint)
				GRAFIX->drawDigital_Micrometer(point1[0], point1[1], point3[0], point3[1], getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY(),  DMMvalue, &cd[0], "duM", windowno, WPixelWidth, FScalefact * WPixelWidth, true, &TxtLeftTop_Rcad[0], &TxtRightBtm_Rcad[0], MAINDllOBJECT->getCurrentDRO().getZ());
			else
				GRAFIX->drawDigital_Micrometer(point1[0], point1[1], point3[0], point3[1], getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY(), DMMvalue, &cd[0], "duM", windowno, WPixelWidth, FScalefact * WPixelWidth, false, &TxtLeftTop_Rcad[0], &TxtRightBtm_Rcad[0], MAINDllOBJECT->getCurrentDRO().getZ());	
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0006", __FILE__, __FUNCSIG__); }
}


void DMMHandler::CalculateDigitalMicrometer(double x, double y, double z)
{
	try
	{
		double angle, intercept;
		//point1[0] = getClicksValue(0).getX(); point1[1] = getClicksValue(0).getY();
		point2[0] = x; point2[1] = y; point3[0] = x; point3[1] = y;
		angle = RMATH2DOBJECT->ray_angle(&point1[0], &point2[0]);
		if(MAINDllOBJECT->LinearModeFlag())
		{
			switch(DmMeasureTypeLinear)
			{
				case 0:
					RMATH2DOBJECT->LineHorizontal_Vertical(angle,&point1[0],&point2[0], &point3[0],&angle, &intercept, true);
					if(FloatFormouseMove)
					{
						RMATH2DOBJECT->Point_PerpenIntrsctn_Line(M_PI_2, point1[0], &point2[0], &point1[0]);
						point3[0] = x; point3[1] = y;
					}
					break;
				case 1:
					RMATH2DOBJECT->LineHorizontal_Vertical(angle,&point1[0],&point2[0], &point3[0],&angle, &intercept, false);
					if(FloatFormouseMove)
					{
						RMATH2DOBJECT->Point_PerpenIntrsctn_Line(0, point1[1], &point2[0], &point1[0]);
						point3[0] = x; point3[1] = y;
					}
					break;
				case 2:
					break;
			}
		}
		else
		{
			if(MAINDllOBJECT->ShiftON && MAINDllOBJECT->highlightedShape() != NULL)
			{
				RMATH2DOBJECT->LineForShiftON(angle,&point1[0],&point2[0], &point3[0],&angle, &intercept);
				HELPEROBJECT->IntersectionPointOnHighlightedLine(MAINDllOBJECT->highlightedShape(), angle, intercept, &point3[0]);
			}
			else if(MAINDllOBJECT->ShiftON)
				RMATH2DOBJECT->LineForShiftON(angle,&point1[0],&point2[0], &point3[0],&angle, &intercept);
			else if(Cshape[1] != NULL && MAINDllOBJECT->highlightedShape() != NULL)
			{
				switch(DmMeasureType)
				{
					case 0:
						if(Cshape[1] != MAINDllOBJECT->highlightedShape())
							HELPEROBJECT->PointOnHighlightedShapeTest(Cshape[1], &point1[0], MAINDllOBJECT->highlightedShape(), &point3[0]);
						else
						{
							HELPEROBJECT->PointOnHighlightedShape(Cshape[1], &point1[0],&point2[0],&point3[0],&angle, &intercept);
						    HELPEROBJECT->IntersectionPointOnHighlightedLine(MAINDllOBJECT->highlightedShape(), angle, intercept, &point3[0]);
						}						
						break;
					case 1:
						switch(Cshape[1]->ShapeType)
						{
							case RapidEnums::SHAPETYPE::XLINE:
							case RapidEnums::SHAPETYPE::XRAY:
							case RapidEnums::SHAPETYPE::LINE:
								HELPEROBJECT->PerpendiculaIntersectiononLine(Cshape[1],&point2[0],&point1[0]);
								break;
							case RapidEnums::SHAPETYPE::ARC:
							case RapidEnums::SHAPETYPE::CIRCLE:
								HELPEROBJECT->PointOnHighlightedShape(MAINDllOBJECT->highlightedShape(), &point1[0],&point2[0],&point3[0],&angle, &intercept);
								HELPEROBJECT->IntersectionPointOnHighlightedCircle(MAINDllOBJECT->highlightedShape(), point2[0], point2[1], angle, intercept, &point3[0]);
								break;
						}
						break;
					case 2:
						switch(Cshape[1]->ShapeType)
						{
							case RapidEnums::SHAPETYPE::XLINE:
							case RapidEnums::SHAPETYPE::XRAY:
							case RapidEnums::SHAPETYPE::LINE:
								HELPEROBJECT->PointOnHighlightedShape(MAINDllOBJECT->highlightedShape(), &point1[0],&point2[0],&point3[0],&angle, &intercept);
								HELPEROBJECT->IntersectionPointOnHighlightedCircle(MAINDllOBJECT->highlightedShape(), point2[0], point2[1], angle, intercept, &point3[0]);
								break;
						}
						break;
					case 3:
						break;
				}
			}
			else if(Cshape[1] != NULL)
			{
				switch(DmMeasureType)
				{
					case 0:
						HELPEROBJECT->PointOnHighlightedShape(Cshape[1], &point1[0],&point2[0],&point3[0],&angle, &intercept);						
						break;
					case 1:
						HELPEROBJECT->PerpendiculaIntersectiononLine(Cshape[1],&point2[0],&point1[0]);
						break;
					case 2:
						break;
					case 3:
						break;
				}
			}
			else if(MAINDllOBJECT->highlightedShape() != NULL)
			{
				switch(DmMeasureType)
				{
					case 0:
					case 2:
						HELPEROBJECT->PointOnHighlightedShape(MAINDllOBJECT->highlightedShape(), &point1[0],&point2[0],&point3[0],&angle, &intercept);
						HELPEROBJECT->IntersectionPointOnHighlightedCircle(MAINDllOBJECT->highlightedShape(), point2[0], point2[1], angle, intercept, &point3[0]);
						break;
					case 1:
					case 3:
						break;
				}
			}
		}
		DMMvalue = RMATH2DOBJECT->Pt2Pt_distance(point1[0], point1[1], point3[0], point3[1]);	
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			DMMvalue = DMMvalue / 25.4;
		RMATH2DOBJECT->Double2String(DMMvalue, NoofDec, cd); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0007", __FILE__, __FUNCSIG__); }
}



void DMMHandler::keyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	 try
	 {
		 int code = nChar;
		 if(code == 18)
		 {
			if(MAINDllOBJECT->ShiftON)
				DrawNearMousePoint = !DrawNearMousePoint;
			else if(MAINDllOBJECT->ControlOn)
				FloatFormouseMove = !FloatFormouseMove;
			else
				MeasureDir = !MeasureDir;
			mouseMove();
		 }
		 RMATH2DOBJECT->MeasurementDirection = MeasureDir;
		 MAINDllOBJECT->update_VideoGraphics(true);
		 MAINDllOBJECT->update_RcadGraphics();
	 }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0008", __FILE__, __FUNCSIG__); }
}

void DMMHandler::EscapebuttonPress()
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0009", __FILE__, __FUNCSIG__); }
}


void DMMHandler::PartProgramData()
{
	try
	{
		if(getClicksDone() == 1 && CheckIntesectFlag)
		{
			runningPartprogramValid = true;
			this->LmouseDown_x(ClickValue[3].getX(), ClickValue[3].getY());
			return;
		}
		Cshape[0] = (DummyShape*)(MAINDllOBJECT->getShapesList().selectedItem());
		list<BaseItem*>::iterator mydim = Cshape[0]->getMchild().begin();
		dim = (DimBase*)(*mydim);
		dim->IsValid(false);
		setMaxClicks(2);
		list<BaseItem*>::iterator ptr = Cshape[0]->getParents().begin(); 
	    int k = Cshape[0]->getParents().size(); 
		if(k > 0)
			Cshape[1] = (Shape*)(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()]);
		
		if (k > 1)
		{
			ptr++;
			Cshape[2] = (Shape*)(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()]);
		}
		baseaction = PPCALCOBJECT->getFrameGrab();
		FirstAction_DMM = baseaction;
		if(baseaction->CurrentWindowNo == 1)
		{
			double mp[2] = {0};
			if(baseaction->Pointer_SnapPt != NULL)
				baseaction->Pointer_SnapPt->FillDoublePointer(&mp[0]);
			point1[0] = mp[0]; point1[1] = mp[1];
			setBaseRProperty(point1[0], point1[1], MAINDllOBJECT->getCurrentDRO().getZ());
			runningPartprogramValid = false;
			PointCount++;
			CheckIntesectFlag = HELPEROBJECT->PointOnHighlightedShapeTest(Cshape[1], &point1[0], Cshape[2], &point3[0]);
			if(CheckIntesectFlag)
				ClickValue[3].set(point3[0], point3[1], MAINDllOBJECT->getCurrentDRO().getZ());
			else
				ClickValue[3].set(point1[0], point1[1], MAINDllOBJECT->getCurrentDRO().getZ());
			ClickValue[0].set(point1[0], point1[1], 0);
			CheckIntesectFlag = true;
			setClicksDone(1);
			PPCALCOBJECT->partProgramReached();
			//return;
		}
		runningPartprogramValid = true;
	}
	catch (...) {
		MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0010", __FILE__, __FUNCSIG__);
		runningPartprogramValid = true;
	}
}

void DMMHandler::LmaxmouseDown()
{
	try
	{
		PointCount = 0;
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		ClickValue[getMaxClicks() - 1].set(pt->getX(), pt->getY(), pt->getZ());
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				FirstAction_DMM->points[0].set(point1[0], point1[1], MAINDllOBJECT->getCurrentDRO().getZ());
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				((DimDigitalMicrometer*)dim)->SetFirstOriginalPoint1(ClickValue[0].getX(), ClickValue[0].getY(), ClickValue[0].getZ());
				((DimDigitalMicrometer*)dim)->SetFirstOriginalPoint2(ClickValue[1].getX(), ClickValue[1].getY(), ClickValue[1].getZ());
				((ShapeWithList*)Cshape[0])->ManagePoint(FirstAction_DMM, ShapeWithList::ADD_POINT);
				((ShapeWithList*)Cshape[0])->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				((DimDigitalMicrometer*)dim)->CalculateMeasurement(&ClickValue[0], &ClickValue[1]); // &FirstAction_DMM->points[0], &baseaction->points[0]);
				dim->IsValid(true);
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}	
		baseaction->points[0].set(point1[0], point1[1], pt->getZ());
		if(Cshape[1] != NULL)		
			Cshape[0]->addParent(Cshape[1]);
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL)
				Cshape[0]->addParent(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1());
		}
		else if(MAINDllOBJECT->highlightedShape() != NULL)
			Cshape[0]->addParent(MAINDllOBJECT->highlightedShape());
		Cshape[1] = NULL;
        AddPointAction::pointAction((ShapeWithList*)Cshape[0], baseaction);
		FirstAction_DMM = baseaction;
		setBaseRProperty(point3[0], point3[1], pt->getZ());
		AddPointAction::pointAction((ShapeWithList*)Cshape[0], baseaction);
		dim = HELPEROBJECT->CreateDMMDim(&FirstAction_DMM->points[0], &baseaction->points[0], pt->getX(), pt->getY(), pt->getZ());
		dim->MeasurementDirection(MeasureDir);
		((DimDigitalMicrometer*)dim)->SetFirstOriginalPoint1(ClickValue[0].getX(), ClickValue[0].getY(), ClickValue[0].getZ());
		((DimDigitalMicrometer*)dim)->SetFirstOriginalPoint2(ClickValue[1].getX(), ClickValue[1].getY(), ClickValue[1].getZ());
		dim->SetMousePosition((point1[0] + point3[0])/2, (point1[1] + point3[1])/2, 0, false);
		//((DimDigitalMicrometer*)dim)->DrawneartheMousePosition = DrawNearMousePoint;
		dim->addParent((BaseItem*)Cshape[0]);
		Cshape[0]->addMChild(dim);
		AddDimAction::addDim(dim);
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
		//MAINDllOBJECT->Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0011", __FILE__, __FUNCSIG__); }
}

void DMMHandler::RmouseDown(double x, double y)
{
	try
	{
		if(MAINDllOBJECT->LinearModeFlag())
		{
			DmMeasureTypeLinear = MAINDllOBJECT->DMMMeasureTypeLinear();
			if(DmMeasureTypeLinear < 2)DmMeasureTypeLinear++;
			else DmMeasureTypeLinear = 0;
			MAINDllOBJECT->DMMMeasureTypeLinear(DmMeasureTypeLinear);
			//FloatFormouseMove = !FloatFormouseMove; // As Told by Narayanan Sir on 26 May 2011.. Modified as per the requiremnt.. Default float with mouse!
			mouseMove();
		}
		else
		{
			if(Cshape[1] != NULL)
			{
				switch(Cshape[1]->ShapeType)
				{
					case RapidEnums::SHAPETYPE::LINE:
					case RapidEnums::SHAPETYPE::XLINE:
					case RapidEnums::SHAPETYPE::XRAY:
						if(DmMeasureType < 3) DmMeasureType++;
						else DmMeasureType = 0;
						break;
					case RapidEnums::SHAPETYPE::ARC:
					case RapidEnums::SHAPETYPE::CIRCLE:
						if(DmMeasureType < 2) DmMeasureType++;
						else DmMeasureType = 0;
						break;
				}
			}
			else
			{
				if(DmMeasureType < 3) DmMeasureType++;
				else DmMeasureType = 0;
			}
		}
		MAINDllOBJECT->update_VideoGraphics();
		MAINDllOBJECT->update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DMMMH0012", __FILE__, __FUNCSIG__); }
}