#include "StdAfx.h"
#include "FrameGrabHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\ThreadCalcFunctions.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddThreadPointAction.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\AreaShape.h"
#include "..\Shapes\Shape.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"
#include <process.h>
#include "..\Shapes\DotsCount.h"

FrameGrabHandler* thread_fghandler = NULL;
//Constructor..//
FrameGrabHandler::FrameGrabHandler()
{
	try
	{
		ValidFlag = false;
		baseaction = NULL;
		runningPartprogramValid = false;
		DrawTempSFGShapes = false;
		AddSelectedPointToShape_ThreadRunning = false;
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
		CurrentWindow = 0;
		withDirectionScan = false;
		drawscanDir = false;
		memset(scandir, 0, 3 * sizeof(double));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0001", __FILE__, __FUNCSIG__); }
}

//Used for partprogram...//
FrameGrabHandler::FrameGrabHandler(FramegrabBase* fg)
{
	try
	{
		baseaction = fg;
		this->CurrentWindow = baseaction->CurrentWindowNo;
		this->CurrentFGType = baseaction->FGtype;

		runningPartprogramValid = true;
		DrawTempSFGShapes = false;
		ValidFlag = false;
		AddSelectedPointToShape_ThreadRunning = false;
		for(int i = 0; i < 3; i++)
		{
			PointsDRO[i].set(baseaction->points[i]);
			PointsPixel[i].set(baseaction->myPosition[i]);
		}	
		if(MAINDllOBJECT->IsCNCMode())
		{
			setClicksDone(getMaxClicks());
			ValidFlag = true;
		}
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabHandler::~FrameGrabHandler()
{
	try
	{
		//while (AddSelectedPointToShape_ThreadRunning == true) ;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0003", __FILE__, __FUNCSIG__); }
}



//Handle the mouse move......//
void FrameGrabHandler::mouseMove()
{
	try
	{
		if(!MAINDllOBJECT->ProfileScanMode() && MAINDllOBJECT->getSelectedShapesList().count() < 1 && !THREADCALCOBJECT->ThreadMeasurementMode()) return;
		if(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->IsCNCMode() && !MAINDllOBJECT->CNCAssisted_FG()) return;
		this->FG_mouseMove();
		if(getClicksDone() == getMaxClicks() - 1)
			ValidFlag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabHandler::LmouseDown()
{
	try
	{
		if(!MAINDllOBJECT->ProfileScanMode() && MAINDllOBJECT->getSelectedShapesList().count() < 1 && !THREADCALCOBJECT->ThreadMeasurementMode()) return;
		if(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->IsCNCMode() && !MAINDllOBJECT->CNCAssisted_FG()) return;
		if(PPCALCOBJECT->IsPartProgramRunning()) ValidFlag = false;
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			if(getClicksDone() == 1)
				this->CurrentWindow = MAINDllOBJECT->getCurrentWindow();
			else if(this->CurrentWindow != MAINDllOBJECT->getCurrentWindow())
			{
				setClicksDone(getClicksDone() - 1);
	    		return;
			}
		}
		this->FG_LmouseDown();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0005", __FILE__, __FUNCSIG__); }
}

//Handle lmouse up.
void FrameGrabHandler::LmouseUp(double x, double y)
{
	try
	{
		//if(getClicksDone() == getMaxClicks() - 1)
			//this->LmouseDown_x(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0006", __FILE__, __FUNCSIG__); }
}

//Handle Rmouse down..
void FrameGrabHandler::RmouseDown(double x, double y)
{
	try
	{
		if(!MAINDllOBJECT->ProfileScanMode() && MAINDllOBJECT->getSelectedShapesList().count() < 1 && !THREADCALCOBJECT->ThreadMeasurementMode()) return;
		if(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->IsCNCMode()) return;
		this->FG_ChangeDrirection();
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0007", __FILE__, __FUNCSIG__); }
}

void FrameGrabHandler::FG_ChangeScanDirection()
{

}

//Handle middle mouse down..
void FrameGrabHandler::MmouseDown(double x, double y)
{
	try
	{
		if(!MAINDllOBJECT->ProfileScanMode() && MAINDllOBJECT->getSelectedShapesList().count() < 1 && !THREADCALCOBJECT->ThreadMeasurementMode()) return;
		if(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->IsCNCMode()) return;
		Shape* shpe = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(shpe->ApplyMMCCorrection() && !shpe->ScanDirDetected())
			this->FG_ChangeScanDirection();
		if(MAINDllOBJECT->getSurface())
			MAINDllOBJECT->ContextMenuForSfg();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void FrameGrabHandler::LmaxmouseDown()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			ValidFlag = false;
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
			    baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO);
				for(int i = 0; i < 3; i++)
				{
					baseaction->points[i].set(PointsDRO[i]);
					baseaction->myPosition[i].set(PointsPixel[i]);
				}	
				/*if(!MAINDllOBJECT->IsCNCMode() && (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT))
				{
					for(int i = 0; i < 3; i++)
					{
						baseaction->points[i].set(PointsDRO[i]);
						baseaction->myPosition[i].set(PointsPixel[i]);
					}	
				}*/
				if(baseaction->CurrentWindowNo == 1)
			       setAddpointactioninRcad();
				else
				{
				    setAddpointaction();
					if (MAINDllOBJECT->CNCAssisted_FG() && MAINDllOBJECT->RuninCNCMode && !MAINDllOBJECT->IsCNCMode())
					{
						MAINDllOBJECT->ChangeCncMode(1); Sleep(20);
					}
			    	//if (PPCALCOBJECT->RefreshImageForEdgeDetection) 
						PPCALCOBJECT->partProgramReached();
				}
				return;
			}
			else if(MAINDllOBJECT->IsPartProgramPause)
			{
				baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO);
				this->FG_LmaxmouseDown();
				for(int i = 0; i < 3; i++)
				{
					baseaction->points[i].set(PointsDRO[i]);
					baseaction->myPosition[i].set(PointsPixel[i]);
				}
				if(baseaction->CurrentWindowNo == 1)
			       setAddpointactioninRcad();
				else
				   setAddpointaction();
				PPCALCOBJECT->partProgramReached();
				return;
			}
		}
		else
		{
			if(this->CurrentWindow != MAINDllOBJECT->getCurrentWindow())
			{
				setClicksDone(getMaxClicks() - 1);
	    		return;
			}
			ValidFlag = false;
			baseaction = new FramegrabBase(CurrentFGType);
			baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO);
			if(MAINDllOBJECT->CameraConnected())
			{
				baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				baseaction->camProperty = MAINDllOBJECT->getCamSettings();
				baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			baseaction->ProfileON = MAINDllOBJECT->getProfile();
			baseaction->SurfaceON = MAINDllOBJECT->getSurface();
			baseaction->Surfacre_FrameGrabtype = MAINDllOBJECT->SurfaceFgtype();

			baseaction->Auto_FG_AssistedMode = MAINDllOBJECT->Force_AutoFG_InAssistedMode();
			
			this->FG_LmaxmouseDown();
			for(int i = 0; i < 3; i++)
			{
				baseaction->points[i].set(PointsDRO[i]);
				baseaction->myPosition[i].set(PointsPixel[i]);
			}	
			baseaction->CurrentWindowNo = this->CurrentWindow;
			if(baseaction->CurrentWindowNo == 1)
			{
			   	baseaction->DroMovementFlag = false;
				setAddpointactioninRcad();
			}
			else
		    	setAddpointaction();
			if(THREADCALCOBJECT->ThreadMeasurementMode()) return;
			Shape* shpe = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(shpe == NULL) return;
			if(((ShapeWithList*)shpe)->PointsList->Pointscount() == 0) return;
	    	if(shpe->ApplyMMCCorrection() && (!shpe->ScanDirDetected()))
			{
				switch (shpe->ShapeType)
				{
					case RapidEnums::SHAPETYPE::LINE:
					case RapidEnums::SHAPETYPE::LINE3D:
					case RapidEnums::SHAPETYPE::PLANE:
					case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					{
						if(shpe->ShapeType == RapidEnums::SHAPETYPE::PLANE)
						{
							for(int i = 0; i < 3; i++)
							((Plane*)shpe)->ScanDir[i] = scandir[i];					
						}
						else if(shpe->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
						{
							/* for(int i = 0; i < 3; i++)
							((CloudPoints*)currentshape)->ScanDir[i] = dir[i];*/
						}
						else
						{
							for(int i = 0; i < 3; i++)
							((Line*)shpe)->ScanDir[i] = scandir[i];
						}
						shpe->ScanDirDetected(true);
						break;
					}
					case RapidEnums::SHAPETYPE::CIRCLE:
					case RapidEnums::SHAPETYPE::ARC:
					case RapidEnums::SHAPETYPE::CIRCLE3D:
					case RapidEnums::SHAPETYPE::ARC3D:
					{	
						if(CurrentFGType == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB || CurrentFGType == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB)
						{
						    shpe->OuterCorrection(withDirectionScan);
						}
						else
						{
							shpe->OuterCorrection(true);
							/*double point1[3] = (Circle*)shpe)->getCenter();
							if(cenz < ((Circle*)shpe)->PointsList->getList().begin()->second->X)
								shpe->OuterCorrection(true);
							else
								shpe->OuterCorrection(false);
							shpe->ScanDirDetected(true);*/
						}
						shpe->ScanDirDetected(true);
						break;
					}		
		    	}
		    }
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0009", __FILE__, __FUNCSIG__); 
	}
}

//Draw the framegrabs.......//
void FrameGrabHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 0 && this->CurrentWindow == 0)
		{
			if(DrawTempSFGShapes)
			{
				for(PSC_ITER Citem = SurfaceFGShapecollection.begin(); Citem != SurfaceFGShapecollection.end(); Citem++)
				{
					Shape* CShape = (*Citem).second;
					RColor& SColor = CShape->ShapeColor;
					GRAFIX->SetColor_Double(SColor.r, SColor.g, SColor.b);
					CShape->drawShape(0, WPixelWidth);
				}
			}
			if(!ValidFlag) return;
			if(MAINDllOBJECT->getShapesList().selectedItem() != NULL)
			{
				Shape* shpe = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				if(shpe == NULL) return;
	    		if(!shpe->ScanDirDetected() && shpe->ApplyMMCCorrection())
					drawscanDir = true;
				else
					drawscanDir = false;
			}
			else
				drawscanDir = false;
			this->FG_draw(WPixelWidth);
		}
		else if(windowno == 1  && this->CurrentWindow == 1)
		{
			if(PPCALCOBJECT->IsPartProgramRunning() ||(getClicksDone() > 0 && getClicksDone() < getMaxClicks()))
			{
		      this->FG_draw(WPixelWidth, 1);
			}
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0010", __FILE__, __FUNCSIG__); 
	}
}

void FrameGrabHandler::FG_draw(double wupp, int windowno)
{
}

//Esc key press..
void FrameGrabHandler::EscapebuttonPress()
{
	try
	{
		ValidFlag = false;
		resetClicks();
		DrawTempSFGShapes = false;
		//delete temp coll
		while(SurfaceFGShapecollection.size() != 0)
		{
			PSC_ITER i = SurfaceFGShapecollection.begin();
			Shape* Csh = (*i).second;
			SurfaceFGShapecollection.erase(Csh->getId());
			delete Csh;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0011", __FILE__, __FUNCSIG__); }
}

//Increase the width of the framegrab...//
void FrameGrabHandler::MouseScroll(bool flag)
{
	try
	{
		this->FG_MouseScroll(flag, MAINDllOBJECT->getWindow(0).getUppX());
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data....//
void FrameGrabHandler::PartProgramData()
{
	try
	{
		runningPartprogramValid = true;
		ValidFlag = true;
		if (MAINDllOBJECT->IsCNCMode() || this->CurrentWindow == 1)
		{
			if (!MAINDllOBJECT->CNCAssisted_FG())
				//PPCALCOBJECT->partProgramReached;
			//else
				LmaxmouseDown();
			else
			{
				if (MAINDllOBJECT->IsCNCMode())
				{
					MAINDllOBJECT->RuninCNCMode = true;
					MAINDllOBJECT->ChangeCncMode(2);
				}
			}
		}
		else if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			MAINDllOBJECT->update_VideoGraphics();
			Sleep(50);
			LmaxmouseDown();
		}
		else
			resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0013", __FILE__, __FUNCSIG__); }
}

//Set the flag..
void FrameGrabHandler::setDrawFG(FramegrabBase* fb)
{
	try
	{
		baseaction = fb;
		for(int i = 0; i < 3; i++)
		{
			PointsDRO[i].set(baseaction->points[i]);
			PointsPixel[i].set(baseaction->myPosition[i]);
		}	
		FG_UpdateFirstFrameppLoad();
		ValidFlag = true;
		runningPartprogramValid = true;
		setClicksDone(getMaxClicks());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014", __FILE__, __FUNCSIG__); }
}

void FrameGrabHandler::SetAreaAction(Shape* shpe)
{
	try
	{
		bool FrameGrabDone = false;
		if(baseaction->ProfileON)
		{
			double Area[2] = {0};
			FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_Area(baseaction, Area);
			//convert area into mm..
			double uppx = MAINDllOBJECT->getWindow(0).getUppX();
			double uppy = MAINDllOBJECT->getWindow(0).getUppY();
			Area[0] *= uppx * uppy; Area[1] *= uppx * uppy;
			if(FrameGrabDone)
				CurrentShape = shpe;
			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				if(!FrameGrabDone)
				{
					baseaction->PartprogrmActionPassStatus(false);
					return;
				}
				else
				{
					baseaction->PartprogrmActionPassStatus(true);
					baseaction->ActionFinished(true);
					((AreaShape*)shpe)->AddArea(Area[0], Area[1], baseaction, false);
				}
			}
			else
			{
				if(!FrameGrabDone) return;
				((AreaShape*)shpe)->AddArea(Area[0], Area[1], baseaction);
			}				
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014a", __FILE__, __FUNCSIG__); }
}

//Add the framgrab points....//
void FrameGrabHandler::setAddpointaction()
{
	try
	{
		Shape* shpe = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(THREADCALCOBJECT->ThreadMeasurementMode())
			shpe = (ShapeWithList*)THREADCALCOBJECT->CurrentSelectedThreadAction->CurrentSelectedThreadShape;
		if(shpe != NULL)
		{
			if(shpe->ShapeType == RapidEnums::SHAPETYPE::AREA)
			{
				SetAreaAction(shpe);
			}
			else
			{
				bool FrameGrabDone = false;
				if(baseaction->SurfaceON)
				{
					DoSurfaceFramegrab();
				}
				else if(baseaction->ProfileON)
				{
					//MessageBox(NULL, TEXT("Starting to Add Action"), TEXT("Rapid-I"), NULL);
					FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false, PPCALCOBJECT->RefreshImageForEdgeDetection);
					if(PPCALCOBJECT->IsPartProgramRunning())
					{
						if(!FrameGrabDone || baseaction->noofpts == 0)
						{
							baseaction->PartprogrmActionPassStatus(false);
	              			baseaction->ActionFinished(true);
							return;
						}
						else if(baseaction->noofpts < baseaction->noofptstaken_build/15)
							baseaction->PartprogrmActionPassStatus(false);
						else
							baseaction->PartprogrmActionPassStatus(true);
					}
					else
					{
						if(!FrameGrabDone) return;
					}	
					AddSelectedPointToShape();
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0015", __FILE__, __FUNCSIG__); }
}

void FrameGrabHandler::setAddpointactioninRcad()
{
	try
	{
		double wupp = MAINDllOBJECT->getWindow(1).getUppX();
		double width = baseaction->FGWidth * wupp;

		switch(this->CurrentFGType)
		{
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
			{
				double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
				double topleft[2] = {0}, bottomRight[2] = {0};
				RMATH2DOBJECT->GetTopLeftNBtnRht(&point1[0], &point2[0], &topleft[0], &bottomRight[0]);
				MAINDllOBJECT->selectFGPointsFromShapes(Vector(topleft[0], topleft[1], 0), Vector(bottomRight[0], bottomRight[1], 0), baseaction);	
				break;
			}
			case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
			{
				double startangle = 0, endAng = 0, radius, radius1, radius2, cen[2];
				double Points2[2]={(PointsDRO[0].getX() + PointsDRO[1].getX())/2 , (PointsDRO[0].getY() + PointsDRO[1].getY())/2 };
				radius = RMATH2DOBJECT->Pt2Pt_distance(PointsDRO[0].getX(),PointsDRO[0].getY(), Points2[0], Points2[1]);
				radius1 = radius - width - wupp;
				radius2 = radius + width + wupp;
				MAINDllOBJECT->selectFGPointsInCircularFrameFromShapes(radius1, radius2, Points2, &startangle, &endAng, baseaction);
				break;
			}
			case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
			{
				double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()},
					point3[2] = {PointsDRO[2].getX(), PointsDRO[2].getY()};
				double cen[2] = {0}, radius = 0, radius1 = 0, radius2 = 0, startangle = 0, sweepangle = 0, endAng = 0, ep1[4] = {0}, ep2[4] = {0};
				int dir = RMATH2DOBJECT->Arc_3Pt(&point1[0], &point2[0], &point3[0], &cen[0], &radius, &startangle, &sweepangle);
				radius1 = radius + width + wupp;
				radius2 = radius - width - wupp;
				if(dir == 1)
				{
				  double tmp = startangle;
				  startangle = startangle - sweepangle;
				  endAng = tmp;
				}
				else
					endAng = startangle + sweepangle;
				MAINDllOBJECT->selectFGPointsInCircularFrameFromShapes(radius1, radius2, cen, &startangle, &endAng, baseaction);
				break;
			}
			case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
			{
				double point1[2] = {0}, point2[2] = {0};
				if (PointsDRO[0].getY() < PointsDRO[1].getY())
				{
					point1[0] = PointsDRO[0].getX(); point1[1] = PointsDRO[0].getY();
					point2[0] = PointsDRO[1].getX(); point2[1] = PointsDRO[1].getY();
				}
				else
				{
					point1[0] = PointsDRO[1].getX(); point1[1] = PointsDRO[1].getY();
					point2[0] = PointsDRO[0].getX(); point2[1] = PointsDRO[0].getY();
				}
				MAINDllOBJECT->selectFGPointsInAngularRectFrameFromShapes(point1, point2, width, baseaction);
				break;
			}
		}
		ShapeWithList* shape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(shape == NULL) return;
		CurrentShape = shape;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			shape->PointAtionList.push_back(PPCALCOBJECT->getCurrentAction());
			shape->ManagePoint(baseaction, shape->ADD_POINT);
			baseaction->ActionFinished(true);
		}
		else
		{
			if(!baseaction->AllPointsList.getList().empty())
			{
				AddPointAction::pointAction(shape, baseaction);
				shape->IgnoreManualCrosshairSteps(true);
			}
		}
		shape->DoneFrameGrab(true);
		char temp[10];
		itoa(baseaction->noofpts, &temp[0], 10);
		std::string TempNoofpoints = (const char*)temp;
		MAINDllOBJECT->SetStatusCode("FrameGrabHandlerAddSelectedPointToShape02", true, TempNoofpoints);
	}
	catch(...)
	{
	
	}
}

void FrameGrabHandler::AddSelectedPointToShape()
{
	try
	{
		if(THREADCALCOBJECT->ThreadMeasurementMode())
		{
			ShapeWithList* shape = (ShapeWithList*)THREADCALCOBJECT->CurrentSelectedThreadAction->CurrentSelectedThreadShape;
			CurrentShape = shape;
			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				shape->ManagePoint(baseaction, shape->ADD_POINT);
				baseaction->ActionFinished(true);
			}
			else
				AddThreadPointAction::ThreadpointAction(shape, baseaction);
			THREADCALCOBJECT->AddPointsToMainCollectionShape(shape);
			if(baseaction->noofpts > 10)
			{
				Shape* Csh = (Shape*)THREADCALCOBJECT->CurrentSelectedThreadAction->ShapeCollection_ThreadWindow.getList()[shape->getId()];
				Csh->FinishedThreadMeasurement(true);
			}
			THREADCALCOBJECT->ThreadMeasurementModeStarted(true);
			char temp[10];
			itoa(baseaction->noofpts, &temp[0], 10);
			std::string TempNoofpoints = (const char*)temp;
			MAINDllOBJECT->SetStatusCode("FrameGrabHandlerAddSelectedPointToShape01", true, TempNoofpoints);
			MAINDllOBJECT->Shape_Updated();
			//MAINDllOBJECT->update_VideoGraphics();
		}
		else
		{
			ShapeWithList* shape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			if(shape == NULL) return;
			CurrentShape = shape;
			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				shape->PointAtionList.push_back(PPCALCOBJECT->getCurrentAction());
				shape->ManagePoint(baseaction, shape->ADD_POINT);
				baseaction->ActionFinished(true);
				/*if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ROTARY_BOSCH_FI)
				{
					double AngleShift = baseaction->PointDRO.getR() - MAINDllOBJECT->getCurrentDRO().getR();
					AngleShift = AngleShift * 180 * 60 / M_PI;
					if(shape->ShapeType == RapidEnums::SHAPETYPE::LINE)
					{
						double langle = ((Line*)shape)->Angle();
						if(abs(cos(langle)) < 0.9 && abs(cos(langle)) > 0.2)
							AngleShift = AngleShift;
						else
							AngleShift = 0;
					}
					if(abs(AngleShift) > 0.1)
					{
						Vector ShiftPos(AngleShift, 0, 0);
						shape->Translate(ShiftPos);
						shape->notifyAll(ITEMSTATUS::DATACHANGED, shape);
					}
				}*/
			}
			else if(HELPEROBJECT->IsFixtureCallibration)
			{
				HELPEROBJECT->IsFixtureCallibration = false;
				shape->AddPoints(&baseaction->AllPointsList);			
				HELPEROBJECT->CreateFramGrabCallback();
			}
			else
			{
				bool AddNewAction = true;
				if (baseaction->SurfaceON && baseaction->SEDP_List.size() != 1)
					AddNewAction = false;
				if (AddNewAction) 
					AddPointAction::pointAction(shape, baseaction);
				shape->IgnoreManualCrosshairSteps(true);
			}
			shape->DoneFrameGrab(true);
			if(shape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && shape->getId() <= MAINDllOBJECT->ToolAxisLineId) return;
			char temp[10];
			itoa(baseaction->noofpts, &temp[0], 10);
			std::string TempNoofpoints = (const char*)temp;
			MAINDllOBJECT->SetStatusCode("FrameGrabHandlerAddSelectedPointToShape02", true, TempNoofpoints);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014", __FILE__, __FUNCSIG__); }
}

int FrameGrabHandler::SelectSurfaceGrabbedShape(char* ShName)
{
	try
	{
		DrawTempSFGShapes = false;
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		//Update the shape 
		ShapeWithList* SelectedSh = NULL;
		std::string SelShName = (const char*)ShName;
		SelectedSh = (ShapeWithList*)(SurfaceFGShapecollection.begin()->second);
		if(SelectedSh == NULL) return -1;
		baseaction->Surfacre_FrameGrabtype = 5; // SelectedSh->getId();
		baseaction->AllPointsList.deleteAll();
		for(PC_ITER Spt = SelectedSh->PointsList->getList().begin(); Spt != SelectedSh->PointsList->getList().end(); Spt++)
		{
			SinglePoint* Sp = (*Spt).second;
			baseaction->AllPointsList.Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
		}
		SurfaceED_Params* currSED = new SurfaceED_Params(baseaction->MaskFactor, baseaction->Binary_LowerFactor, baseaction->Binary_UpperFactor,
			baseaction->MaskingLevel, baseaction->PixelThreshold, baseaction->DoAverageFiltering);
		baseaction->SEDP_List.push_back(*currSED);
		AddSelectedPointToShape();
		//delete temp coll
		while(SurfaceFGShapecollection.size() != 0)
		{
			PSC_ITER i = SurfaceFGShapecollection.begin();
			Shape* Csh = (*i).second;
			SurfaceFGShapecollection.erase(Csh->getId());
			delete Csh;
		}
		int ss = baseaction->SEDP_List.size();
		return ss;
	}
	catch (...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014", __FILE__, __FUNCSIG__);
		return -1;
	}
}

//Highlight the user selected shape
void FrameGrabHandler::HighlightFGShape(char* ShName)
{
	try
	{
		Shape* SelectedSh;
		std::string SelShName = (const char*)ShName;
		for(PSC_ITER Citem = SurfaceFGShapecollection.begin(); Citem != SurfaceFGShapecollection.end(); Citem++)
		{
			SelectedSh = (*Citem).second;
			std::string SName = (const char*)(*Citem).second->getModifiedName();
			if(SName == SelShName)
			{
				SelectedSh->ShapeColor.set(0, 1, 0);
				SelectedSh->ShapeThickness(2);
			}
			else
			{
				SelectedSh->ShapeColor.set(1, 0, 1);
				SelectedSh->ShapeThickness(1);
			}
		}
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014", __FILE__, __FUNCSIG__); }
}

//Do surface framegrab..
void FrameGrabHandler::DoSurfaceFramegrab()
{
	try
	{
		bool FrameGrabDone = true;
		int TotalSFgTypes = 6;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			////while (AddSelectedPointToShape_ThreadRunning == true) ;
			//baseaction->MaskFactor = baseaction->SEDP_List.front().MaskFactor;
			//baseaction->Binary_LowerFactor = baseaction->SEDP_List.front().Binary_Lower;
			//baseaction->Binary_UpperFactor = baseaction->SEDP_List.front().Binary_Upper;
			//baseaction->MaskingLevel = baseaction->SEDP_List.front().MaskingLevel;
			//baseaction->PixelThreshold = baseaction->SEDP_List.front().Threshold;
			//FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, true);
			//if(!FrameGrabDone) // || baseaction->noofpts == 0)
			//{
			//	baseaction->PartprogrmActionPassStatus(false);
			//	return;
			//}
			//else if (baseaction->noofpts < baseaction->noofptstaken_build / 10)
			//{
				bool GotSufficientPoints = false;
				bool SetFreshImage = true;
				//int count = -1;
				//SurfaceED_Params* Ori_SEDP = new SurfaceED_Params(baseaction->MaskFactor, baseaction->Binary_LowerFactor, baseaction->Binary_UpperFactor, baseaction->MaskingLevel, baseaction->PixelThreshold);
				for each(SurfaceED_Params thisSED in baseaction->SEDP_List)
				{
					//count++;
					//if (count == 0)
					//	SetFreshImage = true;
					//else
					//	SetFreshImage = false;

					baseaction->MaskFactor = thisSED.MaskFactor;
					baseaction->Binary_LowerFactor = thisSED.Binary_Lower;
					baseaction->Binary_UpperFactor = thisSED.Binary_Upper;
					baseaction->MaskingLevel = thisSED.MaskingLevel;
					baseaction->PixelThreshold = thisSED.Threshold;
					baseaction->DoAverageFiltering = thisSED.FilterByAverage;
					
					FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, true, SetFreshImage);
					if (FrameGrabDone && baseaction->noofpts > baseaction->noofptstaken_build / 10)
					{
						//baseaction->PartprogrmActionPassStatus(true);
						//AllAttemptsFailed = false;
						GotSufficientPoints = true;
						break;
					}
					SetFreshImage = false;
				}
				//if (AllAttemptsFailed) 
					baseaction->PartprogrmActionPassStatus(GotSufficientPoints);
			//}
			//else
			//	baseaction->PartprogrmActionPassStatus(true);

			//thread_fghandler = this;
			//_beginthread(&AddSelectedPointToShape_Thread, 0, NULL);
			AddSelectedPointToShape();
		}
		else
		{
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			Shape* TempShape;
			if(CShape == NULL) return;
			std::string ShpNames = "";
			//Call a function to do all types..draw the shapes..
			/*for(int i = 0; i < TotalSFgTypes; i++)
			{*/
				std::string tempshapename = "";
				baseaction->Surfacre_FrameGrabtype = MAINDllOBJECT->SelectedAlgorithm();
				FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, true);
				//If you want you check for the flag!!
							
				//if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
				//{
				//	TempShape = new Circle(false, RapidEnums::SHAPETYPE::CIRCLE);
				//	tempshapename = "Circle";
				//}
				//else if(CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				//{
				//	TempShape = new Circle(false, RapidEnums::SHAPETYPE::ARC);
				//	((Circle*)TempShape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				//	tempshapename = "Arc";
				//}
				//else if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				//{
					TempShape = new CloudPoints(false);
					tempshapename = "CldPts";
				//}
				//else if(CShape->ShapeType == RapidEnums::SHAPETYPE::DOTSCOUNT)
				//{
				//	TempShape = new DotsCount(false);
				//	tempshapename = "DotCnts";
				//}
				//else
				//{
				//	TempShape = new Line(false, RapidEnums::SHAPETYPE::LINE);
				//	tempshapename = "Line";
				//}
				tempshapename = tempshapename;
				ShpNames += tempshapename;
				TempShape->setId(0);
				TempShape->setModifiedName(tempshapename);
				TempShape->ShapeColor.set(1, 0, 1);
				((ShapeWithList*)TempShape)->AddPoints(&baseaction->AllPointsList);
				baseaction->AllPointsList.deleteAll();
				SurfaceFGShapecollection[TempShape->getId()] = TempShape;
	/*		}*/
			//Raise event to select the required one..
			DrawTempSFGShapes = true;
			//Raise event to select the required one..
			MAINDllOBJECT->SEDUserChoice((char*)ShpNames.substr(0, ShpNames.length() - 1).c_str());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014", __FILE__, __FUNCSIG__); }
}

void FrameGrabHandler::RedoAlgo6ForSurfaceFG()
{
	try
	{
		//baseaction->ChannelNo = MAINDllOBJECT->Channel_No();
		if (SurfaceFGShapecollection.size() == 0)
			this->DoSurfaceFramegrab();
		else
			HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, true, false);

		Shape* TempShape = SurfaceFGShapecollection[0];
		((ShapeWithList*)TempShape)->PointsList->deleteAll();
		((ShapeWithList*)TempShape)->PointsListOriginal->deleteAll();
		((ShapeWithList*)TempShape)->AddPoints(&baseaction->AllPointsList);
		baseaction->AllPointsList.deleteAll();
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014", __FILE__, __FUNCSIG__); }
}

//update graphics for program
void FrameGrabHandler::updateGraphicsProgram()
{
	try
	{
		for(int i = 0; i < 3; i++)
		{
			PointsDRO[i].set(baseaction->points[i]);
			PointsPixel[i].set(baseaction->myPosition[i]);
		}	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0014", __FILE__, __FUNCSIG__); }
}

//Thread for the adding selected points to shape 
void AddSelectedPointToShape_Thread(void *anything)
{
	try
	{
		thread_fghandler->AddSelectedPointToShape_ThreadRunning = true;
		thread_fghandler->AddSelectedPointToShape();
		thread_fghandler->AddSelectedPointToShape_ThreadRunning = false;
		_endthread();
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0015", __FILE__, __FUNCSIG__); 
		thread_fghandler->AddSelectedPointToShape_ThreadRunning = false;
		_endthread();
	}
}

//void FrameGrabHandler::Set_PtsandPixels(FramegrabBase* fb)
//{
//	try
//	{
//		baseaction = fb;
//		for (int i = 0; i < 3; i++)
//		{
//			PointsDRO[i].set(baseaction->points[i]);
//			PointsPixel[i].set(baseaction->myPosition[i]);
//		}
//
//	}
//	catch (...)
//	{
//		MAINDllOBJECT->SetAndRaiseErrorMessage("FGMH0016", __FILE__, __FUNCSIG__);
//	}
//}
