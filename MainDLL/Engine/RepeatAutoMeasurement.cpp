#include "StdAfx.h"
#include "RepeatAutoMeasurement.h"
#include "RCadApp.h"
#include "..\Shapes\Vector.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Handlers_FrameGrab\FrameGrabArc.h"
#include "..\Handlers_FrameGrab\FlexibleCrossHairHandler.h"
#include "..\Handlers_FrameGrab\FrameGrabCircle.h"
#include "..\Handlers_FrameGrab\FrameGrabAngularRect.h"
#include "..\Handlers_FrameGrab\FrameGrabFixedRectangle.h"
#include "..\Handlers_FrameGrab\FrameGrabEdgeMouseClick.h"
#include "..\Handlers_FrameGrab\FrameGrabRectangle.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\ShapeHeaders.h"


RepeatAutoMeasurement* RepeatAutoMeasurement::_RepeatAutoMeasureInstance = 0;

RepeatAutoMeasurement* RepeatAutoMeasurement::GetRepeatAutoMeasureInstance()
{
	if(_RepeatAutoMeasureInstance == NULL)
		_RepeatAutoMeasureInstance = new RepeatAutoMeasurement();
	return _RepeatAutoMeasureInstance;
}

RepeatAutoMeasurement::RepeatAutoMeasurement()
{
	try
	{
		this->initializeVariable();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0001", __FILE__, __FUNCSIG__); }
}

RepeatAutoMeasurement::~RepeatAutoMeasurement()
{
}

void RepeatAutoMeasurement::initializeVariable()
{
	try
	{
		this->TakeReferencePointFlag = false; this->PtOfRotationFlag = false; this->AutoFocusEnable = false;
		this->NumberOfSteps = 0; this->StepsCount = 0; this->StepAngle = 0;
		pointOfRotation[0] = 0; pointOfRotation[1] = 0; pointOfRotation[2] = 0;
		LastTargetSend[0] = 0; LastTargetSend[1] = 0; LastTargetSend[2] = 0;
		transformMatrix[0] = 0; transformMatrix[1] = 0; transformMatrix[2] = 0;
		transformMatrix[3] = 0; transformMatrix[4] = 0; transformMatrix[5] = 0;
		transformMatrix[6] = 0; transformMatrix[7] = 0; transformMatrix[8] = 1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0002", __FILE__, __FUNCSIG__); }
}

bool RepeatAutoMeasurement::CheckRepeatAutoMeasureInstance()
{
	if(_RepeatAutoMeasureInstance == NULL)
		return false;
	else 
		return true;
}

void RepeatAutoMeasurement::DeleteInstance()
{
	try
	{
		if(_RepeatAutoMeasureInstance != NULL)
		{
			delete _RepeatAutoMeasureInstance;
			_RepeatAutoMeasureInstance = NULL;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0003", __FILE__, __FUNCSIG__); }
}
	
void RepeatAutoMeasurement::ClearAll()
{
	try
	{
		this->initializeVariable();
		ResetParam_CircularRepeatMeasure(0);
		ResetParam_CircularRepeatMeasure(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0004", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::GetNextShape()
{
	try
	{
		if(this->StepsCount == this->NumberOfSteps)
		{
			MAINDllOBJECT->UpdateShapesChanged();
			MAINDllOBJECT->setHandler(new DefaultHandler());
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			MAINDllOBJECT->Automated_ShapeTypeList.clear();
			MAINDllOBJECT->Automated_MeasurementTypeList.clear();
			this->PtOfRotationFlag = false;
			ClearAll();
			ResetParam_CircularRepeatMeasure(3);
			return;
		}
		if(this->NumberOfSteps > 0)
			getShape_Parameter(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0005", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::StartAutoMeasuremnet(double Angle, int Steps, bool AutoFocus)
{
	try
	{
		if(MAINDllOBJECT->AutomatedMeasureModeFlag())
		{
			if(this->PtOfRotationFlag)
			{
				ResetParam_CircularRepeatMeasure(0);
				MAINDllOBJECT->AutomatedMeasureModeFlag(false);
				this->NumberOfSteps = Steps;
				this->StepAngle = Angle;
				this->AutoFocusEnable = AutoFocus;
				this->StepsCount = 1;
				GetNextShape();
			}
			else
			{
				MAINDllOBJECT->ShowMsgBoxString("RepeatAutoMeasurement001", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
			}
		}
		else
		{
			MAINDllOBJECT->ShowMsgBoxString("RepeatAutoMeasurement002", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0006", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::getShape_Parameter(bool firsttime)
{
	try
	{
		if(firsttime)
		{	
			int k = MAINDllOBJECT->Automated_ShapeTypeList.size();
			if(k > 0)
			{
				Shapeitr = MAINDllOBJECT->Automated_ShapeTypeList.begin();
				Shape* Cshape = (Shape*)(*Shapeitr);
				Shape* newshape = Cshape->Clone(0);
				AddShapeAction::addShape(newshape);
				ShapeMeasure_relation[Cshape->getId()] = newshape->getId();

				double R_Theta = StepAngle * StepsCount;
				transformMatrix[0] = cos(R_Theta); transformMatrix[1] = -sin(R_Theta); transformMatrix[2] = 0;
				transformMatrix[3] = sin(R_Theta); transformMatrix[4] = cos(R_Theta);  transformMatrix[5] = 0;
				transformMatrix[6] = 0; transformMatrix[7] = 0; transformMatrix[8] = 1;
				
				Vector GotoOrigin, ShiftPt;
				GotoOrigin.set(-pointOfRotation[0], -pointOfRotation[1], -pointOfRotation[2]);
				ShiftPt.set(pointOfRotation[0], pointOfRotation[1], pointOfRotation[2]);

				newshape->Translate(GotoOrigin);
				newshape->Transform(transformMatrix);
				newshape->Translate(ShiftPt);

				create_newFramgrab(true);
			}	
		}
		else if(Shapeitr == MAINDllOBJECT->Automated_ShapeTypeList.end())
		{
			StepsCount++;
			CreateMeasurement();
			GetNextShape();
		}
		else
		{		
			if((ShapeWithList*)(*Shapeitr)->getParents().size() > 0)
				UpdateDerivedShape();
			else
			{
				Shape* Cshape = (Shape*)(*Shapeitr);
				Shape* newshape = Cshape->Clone(0);
				AddShapeAction::addShape(newshape);
				ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
				create_newFramgrab(true);
			}			
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0007", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::create_newFramgrab(bool firsttime)
{
	try
	{
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		if(firsttime)
		{
			int j = myshapeList->PointAtionList.size();
			if(j > 0)
			{
				ActionList = myshapeList->PointAtionList;
				Framgrabitr = ActionList.begin(); 
				RAction* Currentaction = static_cast<RAction*>((*Framgrabitr));
				if(Currentaction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) return;
				Vector &curDroPos = ((AddPointAction*)(*Framgrabitr))->getFramegrab()->PointDRO;
				Vector temp, temp1;
				temp1.set(curDroPos.getX() - pointOfRotation[0], curDroPos.getY() - pointOfRotation[1], curDroPos.getZ() - pointOfRotation[2]);
				temp = MAINDllOBJECT->TransformMultiply(transformMatrix, temp1.getX(), temp1.getY(), temp1.getZ());
				LastTargetSend[0] = temp.getX() + pointOfRotation[0]; LastTargetSend[1] = temp.getY() + pointOfRotation[1]; LastTargetSend[2] = temp.getZ() + pointOfRotation[2];
				if(AutoFocusEnable)
					MAINDllOBJECT->MoveMachineToRequiredPos(LastTargetSend[0], LastTargetSend[1], LastTargetSend[2], false, RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO_AUTOFOCUS);
				else
					MAINDllOBJECT->MoveMachineToRequiredPos(LastTargetSend[0], LastTargetSend[1], LastTargetSend[2], false, RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO);
			}
		}
		else
		{
			Framgrabitr++;
			if(Framgrabitr == ActionList.end())
			{
				Shapeitr++;
				getShape_Parameter(false);
				return;
			}
			else
			{
				Vector &curDroPos = ((AddPointAction*)(*Framgrabitr))->getFramegrab()->PointDRO;
				Vector temp, temp1;
				temp1.set(curDroPos.getX() - pointOfRotation[0], curDroPos.getY() - pointOfRotation[1], curDroPos.getZ() - pointOfRotation[2]);
				temp = MAINDllOBJECT->TransformMultiply(transformMatrix, temp1.getX(), temp1.getY(), temp1.getZ());
				LastTargetSend[0] = temp.getX() + pointOfRotation[0]; LastTargetSend[1] = temp.getY() + pointOfRotation[1]; LastTargetSend[2] = temp.getZ() + pointOfRotation[2];
				if(AutoFocusEnable)
					MAINDllOBJECT->MoveMachineToRequiredPos(LastTargetSend[0], LastTargetSend[1], LastTargetSend[2], false, RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO_AUTOFOCUS);
				else
					MAINDllOBJECT->MoveMachineToRequiredPos(LastTargetSend[0], LastTargetSend[1], LastTargetSend[2], false, RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0008", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::get_FramGrabParameter()
{
	try
	{
		RapidEnums::RAPIDFGACTIONTYPE CurrentFGType;
		bool FrameGrabDone = false;
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		Shape *s = (Shape*)myshapeList;
		AddPointAction* Caction = (AddPointAction*)(*Framgrabitr);
		CurrentFGType = Caction->getFramegrab()->FGtype;
		FramegrabBase* baseaction = new FramegrabBase(Caction->getFramegrab());
		Vector GotoOrigin, ShiftPt;
		GotoOrigin.set(-pointOfRotation[0], -pointOfRotation[1], -pointOfRotation[2]);
		baseaction->TranslatePosition(GotoOrigin, ((AddPointAction*)Caction)->getShape()->UcsId());
		baseaction->TrasformthePosition(this->transformMatrix, ((AddPointAction*)Caction)->getShape()->UcsId());
		ShiftPt.set(pointOfRotation[0], pointOfRotation[1], pointOfRotation[2]);
		baseaction->TranslatePosition(ShiftPt, ((AddPointAction*)Caction)->getShape()->UcsId());
		switch(CurrentFGType)
		{
		case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
			MAINDllOBJECT->setHandler(new FrameGrabArc(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
			MAINDllOBJECT->setHandler(new FrameGrabAngularRect(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MOUSECLICK_GRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			MAINDllOBJECT->setHandler(new FrameGrabEdgeMouseClick(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
			MAINDllOBJECT->setHandler(new FrameGrabCircle(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
			MAINDllOBJECT->setHandler(new FrameGrabRectangle(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
			MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
			MAINDllOBJECT->setHandler(new FlexibleCrossHairHandler(baseaction));
			break;
		}
		MAINDllOBJECT->update_VideoGraphics();
		Sleep(100);
		
		ShapeWithList* Csh = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(Csh != NULL)
		{
			//grab points from the surface....
			FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false, true);
			if(FrameGrabDone)//add points to the shape..........
				AddPointAction::pointAction(Csh, baseaction);
		}
		MAINDllOBJECT->setHandler(new DefaultHandler());
		MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		create_newFramgrab(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0009", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::ContinueAutoMeasurement()
{
	try
	{
		if(AutoFocusEnable)
			MAINDllOBJECT->MoveMachineToRequiredPos(LastTargetSend[0], LastTargetSend[1], LastTargetSend[2], false, RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO_AUTOFOCUS);
		else
			MAINDllOBJECT->MoveMachineToRequiredPos(LastTargetSend[0], LastTargetSend[1], LastTargetSend[2], false, RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0010", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::UpdateDerivedShape()
{
	try
	{
		if(ShapeMeasure_relation.size() < 1)	return;
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		int j = myshapeList->PointAtionList.size();
		if(j == 0)
		{
			Shape* Cshape = (Shape*)(*Shapeitr);
			Shape* newshape = Cshape->Clone(0);
			AddShapeAction::addShape(newshape);
			ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
			list<BaseItem*>::iterator ptr = Cshape->getParents().begin();
			while(ptr != Cshape->getParents().end())
			{
				Shape* PShape = (Shape*)(*ptr);
				Shape* newParentShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[PShape->getId()]];
				newParentShape->addChild(newshape);
				newshape->addParent(newParentShape);
				ptr++;
			}
		}
		else if(j == 1)
		{
			ActionList = myshapeList->PointAtionList;
			Framgrabitr = ActionList.begin(); 
			RAction* Currentaction = static_cast<RAction*>((*Framgrabitr));
			if(Currentaction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) return;
			AddPointAction* Caction = (AddPointAction*)(*Framgrabitr);
			FramegrabBase* base_action = Caction->getFramegrab();
			Vector MousePt, temp, temp1, temp2, temp3;
			if(base_action->CurrentWindowNo == 1 && base_action->Pointer_SnapPt != NULL)
			{
				temp2.set(base_action->Pointer_SnapPt->getX(), base_action->Pointer_SnapPt->getY(), base_action->Pointer_SnapPt->getZ());			
				temp1.set(temp2.getX() - pointOfRotation[0], temp2.getY() - pointOfRotation[1], temp2.getZ() - pointOfRotation[2]);
				temp = MAINDllOBJECT->TransformMultiply(transformMatrix, temp1.getX(), temp1.getY(), temp1.getZ());
				temp3.set(temp.getX() + pointOfRotation[0], temp.getY() + pointOfRotation[1], temp.getZ() + pointOfRotation[2]);
				UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
				Vector* TempV = MAINDllOBJECT->getVectorPointer_UCS(&temp3, ucs, true);
				MousePt.set(TempV->getX(), TempV->getY(), TempV->getZ());
			}
			else
			{
				if(myshapeList->PointsListOriginal->getList().size() > 0)
				{
					SinglePoint* Spt = (SinglePoint*)(*myshapeList->PointsListOriginal->getList().begin()).second;
					temp2.set(Spt->X, Spt->Y, Spt->Z);
					temp1.set(temp2.getX() - pointOfRotation[0], temp2.getY() - pointOfRotation[1], temp2.getZ() - pointOfRotation[2]);
					temp = MAINDllOBJECT->TransformMultiply(transformMatrix, temp1.getX(), temp1.getY(), temp1.getZ());
					MousePt.set(temp.getX() + pointOfRotation[0], temp.getY() + pointOfRotation[1], temp.getZ() + pointOfRotation[2]);
				}
			}
			Shape* Cshape = (Shape*)(*Shapeitr);
			Shape* newshape = Cshape->Clone(0);
			AddShapeAction::addShape(newshape);
			ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
			MAINDllOBJECT->getCurrentHandler()->setBaseRProperty(MousePt.getX(), MousePt.getY(), MousePt.getZ());
			list<BaseItem*>::iterator ptr = Cshape->getParents().begin();
			while(ptr != Cshape->getParents().end())
			{
				Shape* PShape = (Shape*)(*ptr);
				Shape* newParentShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[PShape->getId()]];
				newParentShape->addChild(newshape);
				newshape->addParent(newParentShape);
				ptr++;
			}
			if(base_action->CurrentWindowNo == 1)
			{
				MAINDllOBJECT->getCurrentHandler()->baseaction->CurrentWindowNo = 1;
				MAINDllOBJECT->getCurrentHandler()->baseaction->DroMovementFlag = false;
			}		
			AddPointAction::pointAction((ShapeWithList*)newshape, MAINDllOBJECT->getCurrentHandler()->baseaction);
		}
		else if(j == 2)
		{
			ActionList = myshapeList->PointAtionList;
			Shape* Cshape = (Shape*)(*Shapeitr);
			Shape* newshape = Cshape->Clone(0);
			AddShapeAction::addShape(newshape);
			ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
			for(Framgrabitr = ActionList.begin(); Framgrabitr != ActionList.end(); Framgrabitr++)
			{
				RAction* Currentaction = static_cast<RAction*>((*Framgrabitr));
				if(Currentaction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) return;
				AddPointAction* Caction = (AddPointAction*)(*Framgrabitr);
				FramegrabBase* base_action = Caction->getFramegrab();
				Vector MousePt, temp, temp1, temp2, temp3;
				temp2.set(base_action->points[0].getX(), base_action->points[0].getY(), base_action->points[0].getZ());
				temp1.set(temp2.getX() - pointOfRotation[0], temp2.getY() - pointOfRotation[1], temp2.getZ() - pointOfRotation[2]);
				temp = MAINDllOBJECT->TransformMultiply(transformMatrix, temp1.getX(), temp1.getY(), temp1.getZ());
				MousePt.set(temp.getX() + pointOfRotation[0], temp.getY() + pointOfRotation[1], temp.getZ() + pointOfRotation[2]);
				MAINDllOBJECT->getCurrentHandler()->setBaseRProperty(MousePt.getX(), MousePt.getY(), MousePt.getZ());
				if(base_action->CurrentWindowNo == 1)
				{
					MAINDllOBJECT->getCurrentHandler()->baseaction->CurrentWindowNo = 1;
					MAINDllOBJECT->getCurrentHandler()->baseaction->DroMovementFlag = false;
				}
				AddPointAction::pointAction((ShapeWithList*)newshape, MAINDllOBJECT->getCurrentHandler()->baseaction, false);
			}
			list<BaseItem*>::iterator ptr = Cshape->getParents().begin();
			while(ptr != Cshape->getParents().end())
			{
				Shape* PShape = (Shape*)(*ptr);
				Shape* newParentShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[PShape->getId()]];
				newParentShape->addChild(newshape);
				newshape->addParent(newParentShape);
				ptr++;
			}
		}
		else
			return;
		Shapeitr++;
		getShape_Parameter(false);
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0011", __FILE__, __FUNCSIG__); }
}

void RepeatAutoMeasurement::CreateMeasurement()
{
	int num = 0;
	try
	{
		for each(DimBase* measurement in MAINDllOBJECT->Automated_MeasurementTypeList)
		{
			//create clone of measurement.......
			DimBase* newmeasurement = measurement->Clone(0);
			num++;
			//Rotate measurement...
			Vector GotoOrigin, ShiftPt;
			GotoOrigin.set(-pointOfRotation[0], -pointOfRotation[1], -pointOfRotation[2]);
			num++;
			newmeasurement->TranslatePosition(GotoOrigin);
			newmeasurement->TransformPosition(this->transformMatrix);
			num++;
			ShiftPt.set(pointOfRotation[0], pointOfRotation[1], pointOfRotation[2]);
			newmeasurement->TranslatePosition(ShiftPt);			
			num++;

			Shape *oldPShape1 = NULL, *oldPShape2 = NULL, *oldPShape3 = NULL, *oldPShape4 = NULL;
			Vector *OldParentPoint1 = NULL, *OldParentPoint2 = NULL;
			int Shapecount = 0, PointCount = 0;
			num++;
			if(measurement->ParentShape1 != NULL)
			{
				oldPShape1 = measurement->ParentShape1;
				Shapecount++;
				num++;
				if(measurement->ParentShape2 != NULL)
				{
					oldPShape2 = measurement->ParentShape2;
					Shapecount++;
					if(measurement->ParentShape3 != NULL)
					{
						oldPShape3 = measurement->ParentShape3;
						Shapecount++;
						if (measurement->ParentShape4 != NULL)
						{
							oldPShape4 = measurement->ParentShape4;
							Shapecount++;
						}
					}
				}
			}

			num++;
			if(measurement->getParents().size() > Shapecount)
			{
				Vector Point1, Point2;
				num += 50;
				if(measurement->ParentPoint1 != NULL)
				{
					PointCount++; num++;
					Vector TempPt1, TempPt2;
					TempPt1.set(measurement->ParentPoint1->getX() + GotoOrigin.getX(), measurement->ParentPoint1->getY() + GotoOrigin.getY(), measurement->ParentPoint1->getZ() + GotoOrigin.getZ());
					TempPt2 = MAINDllOBJECT->TransformMultiply(this->transformMatrix, TempPt1.getX(), TempPt1.getY());
					Point1.set(TempPt2.getX() + pointOfRotation[0], TempPt2.getY()  + pointOfRotation[1], TempPt1.getZ()  + pointOfRotation[2]);
					UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
					OldParentPoint1 = MAINDllOBJECT->getVectorPointer_UCS(&Point1, ucs, true);
					if(OldParentPoint1 == NULL)
					{
						OldParentPoint1 = new Vector(Point1.getX(), Point1.getY(), Point1.getZ());
						//OldParentPoint1->set(Point1.getX(), Point1.getY(), Point1.getZ());
					}
				}

				if(measurement->ParentPoint2 != NULL)
				{
					PointCount++; num += 10;
					Vector TempPt1, TempPt2;
					TempPt1.set(measurement->ParentPoint2->getX() + GotoOrigin.getX(), measurement->ParentPoint2->getY() + GotoOrigin.getY(), measurement->ParentPoint2->getZ() + GotoOrigin.getZ());
					TempPt2 = MAINDllOBJECT->TransformMultiply(this->transformMatrix, TempPt1.getX(), TempPt1.getY());
					Point1.set(TempPt2.getX() + pointOfRotation[0], TempPt2.getY()  + pointOfRotation[1], TempPt1.getZ()  + pointOfRotation[2]);
					UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
					OldParentPoint2 = MAINDllOBJECT->getVectorPointer_UCS(&Point2, ucs, true);
					if(OldParentPoint2 == NULL)
					{
						OldParentPoint2 = new Vector(Point2.getX(), Point2.getY(), Point2.getZ());
						//OldParentPoint2->set(Point2.getX(), Point2.getY(), Point2.getZ());
					}
				}
			}

			if(Shapecount == 4)
			{
				num += 400;
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
				newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
				newmeasurement->ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape3->getId()]];
				newmeasurement->ParentShape4 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape4->getId()]];
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape3, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape4, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);
			}
			else if(Shapecount == 3)
			{
				num += 300;
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
				newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
				newmeasurement->ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape3->getId()]];
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape3, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);
			}
			else if (Shapecount == 2)
			{
				num += 200;
				if (PointCount == 1)
				{
					num++;
					newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
					newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
					Shape* Cshape = NULL;
					for (list<BaseItem*>::iterator itr = measurement->getParents().begin(); itr != measurement->getParents().end(); itr++)
					{
						Shape* PShape = (Shape*)(*itr);
						if (PShape->getId() != newmeasurement->ParentShape1->getId() && PShape->getId() != newmeasurement->ParentShape2->getId())
						{
							Cshape = PShape;
							break;
						}
					}
					newmeasurement->ParentPoint1 = OldParentPoint1;
					HELPEROBJECT->AddShapeMeasureRelationShip(Cshape, newmeasurement);
					HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
					HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
					newmeasurement->UpdateMeasurement();
					AddDimAction::addDim(newmeasurement);
				}
				else // if(Shapecount == 2)
				{
					num += 5;
					newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
					newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
					HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
					HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
					newmeasurement->UpdateMeasurement();
					AddDimAction::addDim(newmeasurement);
				}
			}
			else if (Shapecount == 1)
			{
				num += 100;
				if (PointCount == 1)
				{
					num++;
					newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
					Shape* Cshape = NULL;
					for (list<BaseItem*>::iterator itr = measurement->getParents().begin(); itr != measurement->getParents().end(); itr++)
					{
						Shape* PShape = (Shape*)(*itr);
						if (PShape->getId() != newmeasurement->ParentShape1->getId())
						{
							Cshape = PShape;
							break;
						}
					}
					newmeasurement->ParentPoint1 = OldParentPoint1;
					HELPEROBJECT->AddShapeMeasureRelationShip(Cshape, newmeasurement);
					HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
					newmeasurement->UpdateMeasurement();
					AddDimAction::addDim(newmeasurement);
				}
				else
				{
					num += 5;
					newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
					HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
					newmeasurement->UpdateMeasurement();
					AddDimAction::addDim(newmeasurement);
				}
			}
			else if(PointCount == 2)
			{
				num += 600;
				Shape *Cshape1 = NULL, *Cshape2 = NULL;
				for(list<BaseItem*>::iterator itr = measurement->getParents().begin(); itr != measurement->getParents().end(); itr++)
				{
					if(Cshape1 == NULL)
					{
						Cshape1 = (Shape*)(*itr);
					}
					else
					{
						Cshape2 = (Shape*)(*itr);
						break;
					}
				}
				num++;
				newmeasurement->ParentPoint1 = OldParentPoint1;
				newmeasurement->ParentPoint2 = OldParentPoint2;
				num++;
				HELPEROBJECT->AddShapeMeasureRelationShip(Cshape1, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(Cshape2, newmeasurement);
				num++;
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);	
				num++;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0012_" + std::to_string(num), __FILE__, __FUNCSIG__); }
}