//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Actions\CircularInterPolationAction.h"
#include "..\MAINDll\Engine\PartProgramFunctions.h"
#include "..\MAINDLL\Actions\LinearInterPolationAction.h"
#include "..\MAINDLL\Shapes\Line.h"
#include "..\MAINDLL\Shapes\Circle.h"
#include "..\MAINDLL\Shapes\Vector.h"

///Default constructor.. Initialise the deafault settings..
RWrapper::RW_CircularInterPolation::RW_CircularInterPolation()
{
	 try
	 {
		 this->CircularInterPolationInstance = this;
		 this->HeightGaugeMax = 0; this->HeightGaugeMin = 0; this->CommandCount = 0; this->StartPosition = 0;
		 this->TargetPos = gcnew cli::array<double>(3);
		 this->IntermediatePos = gcnew cli::array<double>(3);
		 this->IntPtsArray = gcnew cli::array<double>(9);
		 this->InterFlag = false; this->TargetFlag = false; this->AddPointsFlag = false; this->LineArcFlag = false;
		 this->CloudPts_HeightGauge = new PointCollection();
		 this->PathPtsColl = gcnew System::Collections::Generic::List<PathPts^>();
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0001", ex); }
}

//Destructor.. Release the memory..
RWrapper::RW_CircularInterPolation::~RW_CircularInterPolation()
{
	try
	{
		this->CloudPts_HeightGauge->deleteAll();
		delete this->CloudPts_HeightGauge;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0002", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_CircularInterPolation^ RWrapper::RW_CircularInterPolation::MYINSTANCE()
{
	return CircularInterPolationInstance;
}

void RWrapper::RW_CircularInterPolation::SetIntermediate_TargetPosition(bool Intermediateflag)
{
	try
	{
		if(Intermediateflag)
		{
			this->InterFlag = true;
			this->IntermediatePos[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
			this->IntermediatePos[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
			this->IntermediatePos[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		}
		else
		{
			this->TargetFlag = true;
			this->TargetPos[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
			this->TargetPos[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
			this->TargetPos[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0003", ex); }
}

void RWrapper::RW_CircularInterPolation::StartCircularInterpolation(double Sweepangle, int NumberOfHops)
{
	try
	{
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus("RW_LinearInterPolation01", RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, false, "");
			return;
		}
		this->HeightGaugeMax = 0; HeightGaugeMin = 0;
		this->CloudPts_HeightGauge->deleteAll();
		if(InterFlag && TargetFlag)
		{
			double CircleParam[7] = {0}, CirclePts[9] = {0};		
			CirclePts[0] = IntermediatePos[0]; CirclePts[1] = IntermediatePos[1]; CirclePts[2] = IntermediatePos[2]; CirclePts[3] = TargetPos[0]; CirclePts[4] = TargetPos[1];
			CirclePts[5] = TargetPos[2]; CirclePts[6] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
			CirclePts[7] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1]; CirclePts[8] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
			if(!RMATH3DOBJECT->Circle_3Pt_3D(CirclePts, CircleParam))
			{
				//message please take proper intermediate and target position
				RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus("RW_CircularInterPolation01", RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, false, "");
				return;
			}			
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
				CircularInterPolationAction::AddCircularInterPolationAction(CShape, CircleParam, &CircleParam[3], &CirclePts[6], Sweepangle, NumberOfHops, this->AddPointsFlag);
			else
				CircularInterPolationAction::AddCircularInterPolationAction(CircleParam, &CircleParam[3], &CirclePts[6], Sweepangle, NumberOfHops);
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::CIRCULARINTERPOLATION);
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			DROInstance->GotoCommandWithCircularInterpolation(CircleParam, Sweepangle, RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], NumberOfHops);
		}
		else
		{
			//please first select intermediate and target position...
			RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus("RW_CircularInterPolation02", RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, false, "");
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0003", ex); }
}

void RWrapper::RW_CircularInterPolation::ClearAll()
{
	try
	{
		this->HeightGaugeMax = 0; this->HeightGaugeMin = 0;  this->CommandCount = 0; this->StartPosition = 0;
		 this->InterFlag = false; this->TargetFlag = false; this->AddPointsFlag = false; this->LineArcFlag = false;
		 this->CloudPts_HeightGauge->deleteAll();
		 this->PathPtsColl->Clear();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0004", ex); }
}

void RWrapper::RW_CircularInterPolation::deleteTempCloudPtsColl()
{
	this->CloudPts_HeightGauge->deleteAll();
}

void RWrapper::RW_CircularInterPolation::SetAddPointFlag_HeightGauge(bool flag)
{
	try
	{
		this->AddPointsFlag = flag;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0005", ex); }
}

void RWrapper::RW_CircularInterPolation::StopAddingPtstoSelectedShapes(bool DisableAddPoint)
{
	try
	{
		if(DisableAddPoint)
		{
			this->AddPointsFlag = false;
			DisableAddpPointEvent::raise();
		}
		ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(CShape != NULL)
			CShape->AddPoints(CloudPts_HeightGauge);			
		this->CloudPts_HeightGauge->deleteAll();
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0006", ex); }
}

void RWrapper::RW_CircularInterPolation::AddPointToSelectedCloudPoints(double x, double y, double z)
{
	try
	{
		Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(CShape != NULL)
		{
			if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				this->CloudPts_HeightGauge->Addpoint(new SinglePoint(x, y, z));
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
				this->CloudPts_HeightGauge->Addpoint(new SinglePoint(x, z, y));
			else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
				this->CloudPts_HeightGauge->Addpoint(new SinglePoint(y, z, x));	
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				this->CloudPts_HeightGauge->Addpoint(new SinglePoint(x, -z, y));
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				this->CloudPts_HeightGauge->Addpoint(new SinglePoint(y, -z, x));
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0007", ex); }
}

void RWrapper::RW_CircularInterPolation::Handle_CircularGotoCommand()
{
	try
	{
		this->HeightGaugeMax = 0; HeightGaugeMin = 0;
		this->CloudPts_HeightGauge->deleteAll();
		this->AddPointsFlag = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetAddPointInfo();
		Vector Center = *((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetCenter();
		Vector NormalVctor = *((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetNormalVector();
		int hops = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetHopsCount();
		double SweeepAngle = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetSweepAngle();
		double Radius =  ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetRadius();
		double CircleParam[7] = {Center.getX(), Center.getY(), Center.getZ(), NormalVctor.getX(), NormalVctor.getY(), NormalVctor.getZ(), Radius};
		RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::CIRCULARINTERPOLATION);
		DROInstance->GotoCommandWithCircularInterpolation(CircleParam, SweeepAngle, RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], hops);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0008", ex);	}
}

void RWrapper::RW_CircularInterPolation::AddPathPoint(int PtNumber)
{
	try
	{
		this->IntPtsArray[3 * (PtNumber - 1)] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
		this->IntPtsArray[3 * (PtNumber - 1) + 1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
		this->IntPtsArray[3 * (PtNumber - 1) + 2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0009", ex);	}
}

void RWrapper::RW_CircularInterPolation::AddLine_CircularPath(bool CircularFlag)
{
	try
	{
		PathPts^ Ptstructure = gcnew PathPts;
		Ptstructure->Circular = CircularFlag;
		if(CircularFlag)
		{
			for(int i = 3; i < 9; i++)
				Ptstructure->PtsArray[i - 3] = this->IntPtsArray[i];
		}
		else
		{
			for(int i = 3; i < 6; i++)
				Ptstructure->PtsArray[i - 3] = this->IntPtsArray[i];
		}
		this->PathPtsColl->Add(Ptstructure);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0010", ex);	}
}

void RWrapper::RW_CircularInterPolation::ResetPath()
{
	try
	{
		this->PathPtsColl->Clear();
		this->CommandCount = 0;	this->StartPosition = 0;
		this->HeightGaugeMax = 0; this->HeightGaugeMin = 0;
		this->LineArcFlag = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0011", ex);	}
}

void RWrapper::RW_CircularInterPolation::CreateInterPolationCommands(bool DxfShapeFlag, bool LineArc)
{
	try
	{		
		this->LineArcFlag = LineArc;
		this->CommandCount = 0;
		if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_ZAXIS)
		{
			this->StartPosition = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
		}
		else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_YAXIS)
		{
			this->StartPosition = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
		}
		else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_XAXIS)
		{
			this->StartPosition = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
		}		
		if(DxfShapeFlag)
		{
			CreateGotoCommandList();
		}
		GotoCommandForNextShape(true);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0011", ex);	}
}

void RWrapper::RW_CircularInterPolation::GotoCommandForNextShape(bool firsttime)
{
	try
	{
		this->HeightGaugeMax = 0; this->HeightGaugeMin = 0;
		if(this->CommandCount == this->PathPtsColl->Count)
		{
			StopAddingPtstoSelectedShapes(true);
			return;
		}
		if(this->PathPtsColl[CommandCount]->Circular)
		{
			double CirclePts[9] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2],
				PathPtsColl[CommandCount]->PtsArray[3], PathPtsColl[CommandCount]->PtsArray[4], PathPtsColl[CommandCount]->PtsArray[5], 
				PathPtsColl[CommandCount]->PtsArray[0], PathPtsColl[CommandCount]->PtsArray[1], PathPtsColl[CommandCount]->PtsArray[2]};
			this->CommandCount++;
			if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_ZAXIS)
			{
				CirclePts[2] = this->StartPosition; CirclePts[5] = this->StartPosition; CirclePts[8] = this->StartPosition;
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_YAXIS)
			{
				CirclePts[1] = this->StartPosition; CirclePts[4] = this->StartPosition; CirclePts[7] = this->StartPosition;
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_XAXIS)
			{
				CirclePts[0] = this->StartPosition; CirclePts[3] = this->StartPosition; CirclePts[6] = this->StartPosition;
			}
			
			double CircleParam[7] = {0}, SAngle[2] = {0};
			int Cnt = RMATH2DOBJECT->Arc_3Pt(&CirclePts[0], &CirclePts[3], &CirclePts[6], &CircleParam[0], &CircleParam[6], &SAngle[0], &SAngle[1]);
			if(Cnt == 2)
				SAngle[1] *= -1;
			else if(Cnt == 0)
			{
				//show message box....
				ResetPath();
			}
			int MultiplyFactor = 1;
			if(MAINDllOBJECT->StylusPolarity())
				MultiplyFactor = -1;
			if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_ZAXIS)
			{
				CircleParam[3] = 0; CircleParam[4] = 0; CircleParam[5] = 1 * MultiplyFactor;
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_YAXIS)
			{
				CircleParam[3] = 0; CircleParam[4] = 1 * MultiplyFactor; CircleParam[5] = 0;
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_XAXIS)
			{
				CircleParam[3] = 1 * MultiplyFactor; CircleParam[4] = 0; CircleParam[5] = 0;
			}
			int NumberOfHops = 20;
			int CalculatedHops = int(System::Math::Abs(SAngle[2] * CirclePts[6]));
			if(CalculatedHops > 20 && CalculatedHops < 100)
				NumberOfHops = CalculatedHops;
			else if(CalculatedHops > 100)
				NumberOfHops = 100;
			if(this->LineArcFlag)
			{
				if(!firsttime)
					StopAddingPtstoSelectedShapes(false);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER);
			}
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
				CircularInterPolationAction::AddCircularInterPolationAction(CShape, CircleParam, &CircleParam[3], &CirclePts[0], SAngle[1], NumberOfHops, this->AddPointsFlag);
			else
				CircularInterPolationAction::AddCircularInterPolationAction(CircleParam, &CircleParam[3], &CirclePts[0], SAngle[1], NumberOfHops);
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::TRACEPATH_INTERPOLATION);
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			DROInstance->GotoCommandWithCircularInterpolation(CircleParam, SAngle[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], NumberOfHops);
		}
		else
		{
			double Target[4] = {PathPtsColl[CommandCount]->PtsArray[0], PathPtsColl[CommandCount]->PtsArray[1], PathPtsColl[CommandCount]->PtsArray[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			this->CommandCount++;
			if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_ZAXIS)
			{
				Target[2] = this->StartPosition;
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_YAXIS)
			{
				Target[1] = this->StartPosition;
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_XAXIS)
			{
				Target[0] = this->StartPosition;
			}
			if(this->LineArcFlag)
			{
				if(!firsttime)
					StopAddingPtstoSelectedShapes(false);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
			}
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
				LinearInterPolationAction::AddLinearInterPolationAction(CShape, Target, RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag);
			else
				LinearInterPolationAction::AddLinearInterPolationAction(Target);
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::TRACEPATH_INTERPOLATION);
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			DROInstance->GotoCommand(Target, feedrate);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0012", ex);	}
}

void RWrapper::RW_CircularInterPolation::CreateGotoCommandList()
{
	try
	{
		this->PathPtsColl->Clear();	this->CommandCount = 0;
		std::list<int> ShapeIdList;
		double Point1[2] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1]};
		ShapeWithList* CurrentShape = NULL;
		double dist = 0, NextPt[2] = {0};
		bool firsttime = true;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->IsDxfShape()) continue;
			if(CShape->ShapeType != RapidEnums::SHAPETYPE::LINE && CShape->ShapeType != RapidEnums::SHAPETYPE::ARC) continue;
			double endpt1[2] = {0}, endpt2[2] = {0};
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				endpt1[0] = ((Line*)CShape)->getPoint1()->getX(); endpt1[1] = ((Line*)CShape)->getPoint1()->getY();
				endpt2[0] = ((Line*)CShape)->getPoint2()->getX(); endpt2[1] = ((Line*)CShape)->getPoint2()->getY();			
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				endpt1[0] = ((Circle*)CShape)->getendpoint1()->getX(); endpt1[1] = ((Circle*)CShape)->getendpoint1()->getY();
				endpt2[0] = ((Circle*)CShape)->getendpoint2()->getX(); endpt2[1] = ((Circle*)CShape)->getendpoint2()->getY();
			}
			double d1 = RMATH2DOBJECT->Pt2Pt_distance(Point1, endpt1);
			double d2 = RMATH2DOBJECT->Pt2Pt_distance(Point1, endpt2);
			if(firsttime)
			{
				firsttime = false;
				if(d1 > d2)
				{
					dist = d2;
					NextPt[0] = endpt1[0]; NextPt[1] = endpt1[1]; 
				}
				else
				{
					dist = d1;
					NextPt[0] = endpt2[0]; NextPt[1] = endpt2[1]; 
				}
				CurrentShape = CShape;
			}
			else
			{
				if(d1 > d2)
				{
					if(dist > d2)
					{
						dist = d2;
						NextPt[0] = endpt1[0]; NextPt[1] = endpt1[1]; 
						CurrentShape = CShape;
					}
				}
				else
				{
					if(dist > d1)
					{
						dist = d1;
						NextPt[0] = endpt2[0]; NextPt[1] = endpt2[1]; 
						CurrentShape = CShape;
					}
				}
			}
		}
		if(CurrentShape != NULL)
		{
			AddPathtoList(CurrentShape, NextPt);
			for(int i = 0; i < MAINDllOBJECT->getShapesList().getList().size(); i++)
			{
				bool ReturnFlag = false;
				ShapeIdList.push_back(CurrentShape->getId());
				ShapeWithList* Cshp = GetNextShape(CurrentShape, ShapeIdList, NextPt, &ReturnFlag);
				if(Cshp != NULL && ReturnFlag)
				{
					CurrentShape = Cshp;
					AddPathtoList(CurrentShape, NextPt);
				}
				else
					break;
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0011", ex);	}
}

void RWrapper::RW_CircularInterPolation::AddPathtoList(ShapeWithList* CurrentShape, double* NextPt)
{
	try
	{
		PathPts^ Ptstructure = gcnew PathPts;
		if(CurrentShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			Ptstructure->Circular = false;
			Ptstructure->PtsArray[0] = NextPt[0]; Ptstructure->PtsArray[1] = NextPt[1]; Ptstructure->PtsArray[2] = this->StartPosition;
		}
		else if(CurrentShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			Ptstructure->Circular = true;
			Ptstructure->PtsArray[3] = NextPt[0]; Ptstructure->PtsArray[4] = NextPt[1]; Ptstructure->PtsArray[5] = this->StartPosition;
			Ptstructure->PtsArray[0] = ((Circle*)CurrentShape)->getMidPoint()->getX(); Ptstructure->PtsArray[1] = ((Circle*)CurrentShape)->getMidPoint()->getY(); Ptstructure->PtsArray[2] = this->StartPosition;
		}
		this->PathPtsColl->Add(Ptstructure);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0011", ex);	}
}

ShapeWithList* RWrapper::RW_CircularInterPolation::GetNextShape(ShapeWithList* CurrentShape, std::list<int> IdList, double* NextPt, bool* returnFlag)
{
	try
	{
		ShapeWithList* Cshpe = NULL;
		*returnFlag = false;
		for(list<BaseItem*>::iterator shapeiterator = CurrentShape->getFgNearShapes().begin(); shapeiterator != CurrentShape->getFgNearShapes().end(); shapeiterator++)
		{
			ShapeWithList* Pshp = (ShapeWithList*)(*shapeiterator);
			bool IdPresent = false;
			for each(int Id in IdList)
			{
				if(Pshp->getId() == Id)
				{
					IdPresent = true;
					break;
				}
			}
			if(IdPresent) continue;
			if(Pshp->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				if(System::Math::Abs(((Line*)Pshp)->getPoint1()->getX() - NextPt[0]) < 0.0001 && System::Math::Abs(((Line*)Pshp)->getPoint1()->getY() - NextPt[1]) < 0.0001)
				{		
					NextPt[0] = ((Line*)Pshp)->getPoint2()->getX(); NextPt[1] = ((Line*)Pshp)->getPoint2()->getY();	
					Cshpe = Pshp;
					*returnFlag = true;
					break;
				}
				else if(System::Math::Abs(((Line*)Pshp)->getPoint2()->getX() - NextPt[0]) < 0.0001 && System::Math::Abs(((Line*)Pshp)->getPoint2()->getY() - NextPt[1]) < 0.0001)
				{
					NextPt[0] = ((Line*)Pshp)->getPoint1()->getX(); NextPt[1] = ((Line*)Pshp)->getPoint1()->getY();
					Cshpe = Pshp;
					*returnFlag = true;
					break;
				}
			}
			else if(Pshp->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				if(System::Math::Abs(((Circle*)Pshp)->getendpoint1()->getX() - NextPt[0]) < 0.0001 && System::Math::Abs(((Circle*)Pshp)->getendpoint1()->getY() - NextPt[1]) < 0.0001)
				{					
					NextPt[0] = ((Circle*)Pshp)->getendpoint2()->getX(); NextPt[1] = ((Circle*)Pshp)->getendpoint2()->getY();
					Cshpe = Pshp;
					*returnFlag = true;
					break;
				}
				else if(System::Math::Abs(((Circle*)Pshp)->getendpoint2()->getX() - NextPt[0]) < 0.0001 && System::Math::Abs(((Circle*)Pshp)->getendpoint2()->getY() - NextPt[1]) < 0.0001)
				{
					NextPt[0] = ((Circle*)Pshp)->getendpoint1()->getX(); NextPt[1] = ((Circle*)Pshp)->getendpoint1()->getY();
					Cshpe = Pshp;
					*returnFlag = true;
					break;
				}
			}
		}
		return Cshpe;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0011", ex);	return NULL;}
}