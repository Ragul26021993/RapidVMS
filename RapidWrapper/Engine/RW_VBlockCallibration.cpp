//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Engine\FocusFunctions.h"
#include "..\MAINDll\Engine\CalibrationCalculation.h"
#include "..\MAINDll\Shapes\Line.h"
#include "..\MAINDll\Helper\Helper.h"
#include "RW_VBlockCallibration.h"

///Default constructor.. Initialise the deafault settings..
RWrapper::RW_VBlockCallibration::RW_VBlockCallibration()
{
	 try
	 {
		this->VBlockCalInstance = this; this->PauseCallibration = false;
		this->UpperlineFlag = false; this->LowerLineFlag = false; this->AngleBisectorLineFlag = false; this->CallibrationDoneFlag = false;
		this->UpperLineId = 0; this->LowerLineId = 0; this->AngleBisectorLineId = 0; this->Cradius = 0; this->Axis_Zvalue = 0;
		this->Axis_X1value = 0; this->Axis_X2value = 0; this->IdealRadius = 0; this->new_CylinderRadius = 0; this->newZ_Value = 0; this->Axis_Y1value = 0; this->Axis_Y2value = 0;
		this->HolderCalibration = false; this->FixtureCylinderRadius = 0; this->CylinderOverlapDist = 0;
		this->TargetValue = gcnew cli::array<double>(4);
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0001", ex); }
}

//Destructor.. Release the memory..
RWrapper::RW_VBlockCallibration::~RW_VBlockCallibration()
{
	try
	{
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0002", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_VBlockCallibration^ RWrapper::RW_VBlockCallibration::MYINSTANCE()
{
	return VBlockCalInstance;
}

void RWrapper::RW_VBlockCallibration::SetCylinderRadius(double dia, bool holderType, double fixedCylinderDia, double CylinderOverlap)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)				// changese for inch mode...
		{
				dia = dia * 25.4;
				fixedCylinderDia = fixedCylinderDia * 25.4;
				CylinderOverlap = CylinderOverlap * 25.4;
		}
		for(int i = 0; i < 4; i++)															// save cordinates x,y,z,r
			this->TargetValue[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		this->PauseCallibration = false;
		this->HolderCalibration = holderType;
		this->FixtureCylinderRadius = fixedCylinderDia / 2;
		this->CylinderOverlapDist = CylinderOverlap / 2;
		IdealRadius = dia / 2;
		create_Line(true);																	// call the function to create line at edge
		if(!RWrapper::RW_MainInterface::MYINSTANCE()->MachineCNCStatus)
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);								// make machine mode to cnc(1) if it is 0
		//setting x,y,z,r(for lower line) coordinates in target... its  like a point which is diameteric far from current point downward
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - 2 * IdealRadius, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		for(int i = 0; i < 4; i++)
			this->TargetValue[i] = target[i];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::VBLOCK_CALLIBRATION_LINE);	
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0001", ex); }
}

void RWrapper::RW_VBlockCallibration::create_Line(bool firstLine)							// creating an line 
{
	try
	{
		bool idpresent = false;
		for(RC_ITER i = MAINDllOBJECT->getShapesList().getList().begin(); i != MAINDllOBJECT->getShapesList().getList().end(); i++)
		{
			BaseItem* item = &(*i->second);
			Shape* currentShape = (Shape*)item;
			if(firstLine)
			{
				if(UpperLineId == currentShape->getId())
				{
					idpresent = true;
					break;
				}
			}
			else
			{
				if(LowerLineId == currentShape->getId())
				{
					idpresent = true;
					break;
				}
			}
		}
		if(idpresent)
		{
			MAINDllOBJECT->ClearShapeSelections();
			if(firstLine)
				MAINDllOBJECT->selectShape(UpperLineId, false);
			else
				MAINDllOBJECT->selectShape(LowerLineId, false);
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			CALBCALCOBJECT->SetCurrentSelectedLine(CShape);
		}
		else
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(firstLine)
			{
				UpperlineFlag = true;
				UpperLineId = CShape->getId();
			}
			else
			{
				LowerLineFlag = true;
				LowerLineId = CShape->getId();
			}
			CALBCALCOBJECT->SetCurrentSelectedLine(CShape);
		}
		CALBCALCOBJECT->GrabPointsToSelectedLine(1);											//grabing points  on edge
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0004", ex); }
}

void RWrapper::RW_VBlockCallibration::create_AngleBisector()
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		if(!AngleBisectorLineFlag && UpperlineFlag && LowerLineFlag)
		{
			Shape* ParentShape1 = NULL;
			Shape* ParentShape2 = NULL;
			for(RC_ITER i = MAINDllOBJECT->getShapesList().getList().begin(); i != MAINDllOBJECT->getShapesList().getList().end(); i++)
			{
				BaseItem* item = &(*i->second);
				Shape* currentShape = (Shape*)item;
				if(currentShape->getId() == UpperLineId)
					ParentShape1 = currentShape;
				else if(currentShape->getId() == LowerLineId)
					ParentShape2 = currentShape;
			}
			if(ParentShape1 != NULL && ParentShape2 != NULL)
			{
				AngleBisectorLineFlag = true;
				HELPEROBJECT->Create_AngleBisector(ParentShape1, ParentShape2);	
				Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				AngleBisectorLineId = CShape->getId();
				double Endpts[6] = {0};
				((Line*)CShape)->getEndPoints(Endpts);
				new_CylinderRadius = Cradius = RMATH2DOBJECT->Pt2Line_Dist((Endpts[0] + Endpts[3]) / 2, (Endpts[1] + Endpts[4]) / 2, ((Line*)ParentShape1)->Angle(),((Line*)ParentShape1)->Intercept());
				Axis_Y1value = Endpts[1]; Axis_Y2value = Endpts[4];
				Axis_X1value = Endpts[0]; Axis_X2value = Endpts[3];
				double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], (Endpts[1] + Endpts[4]) / 2, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + IdealRadius, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			    double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
				for(int i = 0; i < 4; i++)
					this->TargetValue[i] = target[i];
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], RWrapper::RW_DRO::MYINSTANCE()->TargetReachedCallback::VBLOCK_CALLIBRATION);
			}
		}
		else
			MAINDllOBJECT->SetStatusCode("RW_VBlockCallibration01");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0005", ex); }
}

void RWrapper::RW_VBlockCallibration::Get_ZvalueOfCylinderAxis()
{
	try
	{		
		if(UpperlineFlag && LowerLineFlag && AngleBisectorLineFlag)
		{
			MAINDllOBJECT->ClearShapeSelections();
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOCUS_VBLOCK_CALLIBRATION;
			//RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(1, 1, 0, 40, 40, 2, 0.3);
			RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(1, 1, 0, 40, 40, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed);
			RWrapper::RW_FocusDepth::MYINSTANCE()->FocusClicked();
		}
		else
			MAINDllOBJECT->SetStatusCode("RW_VBlockCallibration02");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0006", ex); }
}

void RWrapper::RW_VBlockCallibration::Handle_AxisLock()
{
	try
	{
		if(PauseCallibration) return;
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		FOCUSCALCOBJECT->AutoAdjustLighting(true);
		RWrapper::RW_FocusDepth::MYINSTANCE()->AFS_ScanCurrentPosition(false);
		FOCUSCALCOBJECT->AutoAdjustLighting(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0007", ex); }
}

void RWrapper::RW_VBlockCallibration::SetFocus_Zvalue()
{
	try
	{
		if(FOCUSCALCOBJECT->FocusStatusFlag())
		{
			newZ_Value = Axis_Zvalue = FOCUSCALCOBJECT->FocusZValue() - Cradius;
			CallibrationDoneFlag = true;
			VblockCallibrationCompletedEvent::raise(true);
			RWrapper::RW_MainInterface::MYINSTANCE()->Update_HelpStatusMessage("VBlockCalibration01", false, "");
		}
		else	
			MAINDllOBJECT->ShowMsgBoxString("RW_VBlockCallibration03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0008", ex); }
}

void RWrapper::RW_VBlockCallibration::ClearAll()
{
	try
	{
		this->PauseCallibration = false;
		this->UpperlineFlag = false; this->LowerLineFlag = false; this->AngleBisectorLineFlag = false;
		this->UpperLineId = 0; this->LowerLineId = 0; this->AngleBisectorLineId = 0; this->IdealRadius = 0;
		RWrapper::RW_MainInterface::MYINSTANCE()->Update_HelpStatusMessage("VBlockCalibration03", false, "");
		VblockCallibrationCompletedEvent::raise(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0009", ex); }
}

void RWrapper::RW_VBlockCallibration::Create_AxisLine(double dia)
{
	try
	{
		new_CylinderRadius = dia / 2;
		if(!CallibrationDoneFlag) return;
		std::list<int> Idlist;
		for(RC_ITER i = MAINDllOBJECT->getShapesList().getList().begin(); i != MAINDllOBJECT->getShapesList().getList().end(); i++)
		{
			BaseItem* item = &(*i->second);
			Shape* currentShape = (Shape*)item;
			if(!currentShape->Normalshape()) continue;
			if(AngleBisectorLineId == currentShape->getId())
			{
				if (AngleBisectorLineId != 0)
				{
					int temp = AngleBisectorLineId;
					Idlist.push_back(temp);
				}	
			}
			else if(UpperLineId == currentShape->getId())
			{
				if (UpperLineId != 0)
				{
					int temp = UpperLineId;
					Idlist.push_back(temp);
				}	
			}
			else if(LowerLineId == currentShape->getId())
			{
				if (LowerLineId != 0)
				{
					int temp = LowerLineId;
					Idlist.push_back(temp);
				}
			}
		}
		MAINDllOBJECT->ClearShapeSelections();
		if(Idlist.size() > 0)
		{
			MAINDllOBJECT->selectShape(&Idlist);
			MAINDllOBJECT->deleteShape();
		}
		double Point1[3] = {Axis_X1value, Axis_Y1value, newZ_Value}, Point2[3] = {Axis_X2value, Axis_Y2value, newZ_Value};
		if(HolderCalibration)
		{
			double MultiplicationFactor = 1;
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)		
				MultiplicationFactor *=25.4;
			double TmpValue1 = pow((FixtureCylinderRadius + (Cradius/MultiplicationFactor)), 2) - pow((FixtureCylinderRadius - CylinderOverlapDist), 2);
			double FixtureCyl_Z = Axis_Zvalue - pow(TmpValue1, 0.5);
			
			TmpValue1 = pow((FixtureCylinderRadius + new_CylinderRadius), 2) - pow((FixtureCylinderRadius - CylinderOverlapDist), 2);
			newZ_Value = FixtureCyl_Z + pow(TmpValue1, 0.5);
			Point1[2] = newZ_Value; Point2[2] = newZ_Value; 
		}
		else
		{
			if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL)
			{
				Point1[1] = 1.4142 * (new_CylinderRadius - Cradius) + Point1[1]; Point2[1] = 1.4142 * (new_CylinderRadius - Cradius) + Point2[1];
			}
			else
			{
				double MultiplicationFactor = 1;
				if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)		
					MultiplicationFactor *=25.4;
				newZ_Value = 1.4142 * (new_CylinderRadius - (Cradius/MultiplicationFactor)) + Axis_Zvalue;
				Point1[2] = newZ_Value; Point2[2] = newZ_Value; 
			}
		}
		
		HELPEROBJECT->CreateToolAxis(Point1, Point2);
		RWrapper::RW_MainInterface::MYINSTANCE()->Update_HelpStatusMessage("VBlockCalibration02", false, "");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0010", ex); }
}

void RWrapper::RW_VBlockCallibration::GotoCommandForVblock(int i)
{
	try
	{
		if(MAINDllOBJECT->Vblock_CylinderAxisLine == NULL) return;
		RWrapper::RW_MainInterface::MYINSTANCE()->MachineCNCStatus = RWrapper::RW_CNC::MYINSTANCE()->getCNCMode();
		if(!RWrapper::RW_MainInterface::MYINSTANCE()->MachineCNCStatus)
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		
		double MultiplicationFactor = 1;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)		
			MultiplicationFactor *=25.4;
		
		switch(i)
		{
		case 0:
			GetXYCoordinate(i, target);
			break;
		case 1:
			GetXYCoordinate(i, target);
			break;
		case 2:
			GetXYCoordinate(i, target);
			break;
		case 3:
			target[2] = newZ_Value + (new_CylinderRadius * MultiplicationFactor);
			break;
		case 4:
			target[2] = newZ_Value;
			break;
		case 5:
			target[0] = Axis_X1value;
			break;
		};
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], RWrapper::RW_DRO::MYINSTANCE()->TargetReachedCallback::VBLOCK_GOTOCOMMAND);			 
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0011", ex); }
}

void RWrapper::RW_VBlockCallibration::GetXYCoordinate(int i, double* XY_Coordinate)
{
	try
	{
		double resultVect[3] = {0, 1, 0}, Vect1[3] = {0, 0, 1}, Vect2[3] = {((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->dir_l(), ((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->dir_m(), ((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->dir_n()};
		if(Vect2[0] < 0)
		{
			Vect2[0] *= -1; Vect2[1] *= -1; Vect2[2] *= -1;
		}
		RMATH3DOBJECT->CrossProductOf2Vectors(Vect1, Vect2, resultVect);
		double TempPoint[3] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]};
		double ProjectedPt[3] = {TempPoint[0], TempPoint[1], TempPoint[2]}; 
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->Angle(), ((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->Intercept(), TempPoint, ProjectedPt);
		
		double MultiplicationFactor = 1;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)		
			MultiplicationFactor *=25.4;

		switch(i)
		{
		case 0:
			XY_Coordinate[0] = resultVect[0] * new_CylinderRadius * MultiplicationFactor + ProjectedPt[0];
			XY_Coordinate[1] = resultVect[1] * new_CylinderRadius * MultiplicationFactor + ProjectedPt[1];
			break;
		case 1:
			XY_Coordinate[0] = ProjectedPt[0];
			XY_Coordinate[1] = ProjectedPt[1];
			break;
		case 2:
			XY_Coordinate[0] = -resultVect[0] * new_CylinderRadius * MultiplicationFactor + ProjectedPt[0];
			XY_Coordinate[1] = -resultVect[1] * new_CylinderRadius * MultiplicationFactor + ProjectedPt[1];
			break;
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0012", ex); }
}

void RWrapper::RW_VBlockCallibration::ContinuePauseVblockCallibration(bool PauseFlag)
{
	try
	{
		this->PauseCallibration = PauseFlag;
		if(PauseFlag)
		{			
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		}
		else
		{
			if(!LowerLineFlag)
			{
				if(!RWrapper::RW_MainInterface::MYINSTANCE()->MachineCNCStatus)
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				double target[4] = {TargetValue[0], TargetValue[1], TargetValue[2], TargetValue[3]};
				double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], RWrapper::RW_DRO::MYINSTANCE()->TargetReachedCallback::VBLOCK_CALLIBRATION_LINE);	
			}
			else if(!CallibrationDoneFlag && AngleBisectorLineFlag && UpperlineFlag && LowerLineFlag)
			{
				Shape* CShape = NULL;
				Shape* ParentShape1 = NULL;
				Shape* ParentShape2 = NULL;
				for(RC_ITER i = MAINDllOBJECT->getShapesList().getList().begin(); i != MAINDllOBJECT->getShapesList().getList().end(); i++)
				{
					BaseItem* item = &(*i->second);
					Shape* currentShape = (Shape*)item;
					if(currentShape->getId() == UpperLineId)
						ParentShape1 = currentShape;
					else if(currentShape->getId() == LowerLineId)
						ParentShape2 = currentShape;
					else if(currentShape->getId() == AngleBisectorLineId)
						CShape = currentShape;
				}
				if(CShape != NULL && ParentShape1 != NULL && ParentShape2 != NULL)
				{
					double Endpts[6] = {0};
					((Line*)CShape)->getEndPoints(Endpts);
					new_CylinderRadius = Cradius = RMATH2DOBJECT->Pt2Line_Dist((Endpts[0] + Endpts[3]) / 2, (Endpts[1] + Endpts[4]) / 2, ((Line*)ParentShape1)->Angle(),((Line*)ParentShape1)->Intercept());
					Axis_Y1value = Endpts[1]; Axis_Y2value = Endpts[4];
					Axis_X1value = Endpts[0]; Axis_X2value = Endpts[3];
					double target[4] = {TargetValue[0], TargetValue[1], TargetValue[2], TargetValue[3]};
					double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
					if(!RWrapper::RW_MainInterface::MYINSTANCE()->MachineCNCStatus)
						RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], RWrapper::RW_DRO::MYINSTANCE()->TargetReachedCallback::VBLOCK_CALLIBRATION);			 
				}
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_VblkCal0013", ex); }
}