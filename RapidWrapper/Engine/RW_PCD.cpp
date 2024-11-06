#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Shapes\RPoint.h"
#include "..\MAINDLL\Shapes\PCDCircle.h"
#include "..\MAINDLL\Engine\PartProgramFunctions.h"

RWrapper::RW_PCD::RW_PCD()
{
	try
	{
		this->pcd = this;
		this->PCDlist = gcnew ArrayList;
		this->PCDShapeCollection = gcnew System::Collections::Hashtable;
		this->AddShapeMemberNameList = gcnew System::Collections::Generic::List<System::String^>; 
		this->PCDMemberNameList = gcnew System::Collections::Generic::List<System::String^>; 
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0001", ex); }
}

RWrapper::RW_PCD::~RW_PCD()
{
}

RWrapper::RW_PCD^ RWrapper::RW_PCD::MYINSTANCE()
{
	return pcd;
}

void RWrapper::RW_PCD::NewClicked()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())return;
		ClearLastPCDValues();
		for(RC_ITER i = MAINDllOBJECT->getShapesList().getList().begin(); i != MAINDllOBJECT->getShapesList().getList().end(); i++)
		{
			String^ _shapeName = gcnew System::String(((Shape*)(*i).second)->getModifiedName());
			if(((Shape*)(*i).second)->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || ((Shape*)(*i).second)->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				this->PCDShapeCollection->Add(_shapeName, ((Shape*)(*i).second)->getId());
				AddShapeMemberNameList->Add(_shapeName);
			}
		}
		MAINDllOBJECT->AddNewPcdCircle();
		PCDMeasureSelectionChanged::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0002", ex); }
}

void RWrapper::RW_PCD::PCD_MeasurementClicked()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())return;
		if(MAINDllOBJECT->CurrentPCDShape == NULL)
		{
			 MAINDllOBJECT->SetStatusCode("RW_PCD01");
			return;
		}
		MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::PCDMEASURE_HANDLER);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0003", ex); }
}

void RWrapper::RW_PCD::Circle_Added(System::String^ CName)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())return;
		this->PCDMemberNameList->Add(CName);
		this->AddShapeMemberNameList->Remove(CName);
		MAINDllOBJECT->AddCircleToCurrentPCD((int)this->PCDShapeCollection[CName]);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0004", ex); }
}

void RWrapper::RW_PCD::Circle_Remove(System::String^ CName)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())return;
		this->PCDMemberNameList->Remove(CName);
		this->AddShapeMemberNameList->Add(CName);
		MAINDllOBJECT->RemoveCircleFromCurrentPCD((int)this->PCDShapeCollection[CName]);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0005", ex); }
}

void RWrapper::RW_PCD::ClearLastPCDValues()
{
	try
	{
		PCDShapeCollection->Clear();
		PCDlist->Clear();
		AddShapeMemberNameList->Clear();
		PCDMemberNameList->Clear();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0006", ex); }
}

void RWrapper::RW_PCD::GetSelectedPCDParameters()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())return;
		if(MAINDllOBJECT->CurrentPCDShape == NULL)return;
		ClearLastPCDValues();
		PCDCircle* CShape = (PCDCircle*)MAINDllOBJECT->CurrentPCDShape;
		/*for each(Shape* CSh in CShape->PCDAllShapeCollection)
		{
			System::String^ _shapeName = gcnew System::String(CSh->getModifiedName());
			this->PCDShapeCollection->Add(_shapeName, CSh->getId());
			AddShapeMemberNameList->Add(_shapeName);
		}
		for each(Shape* CSh in CShape->PCDParentCollection)
		{
			System::String^ _shapeName = gcnew System::String(CSh->getModifiedName());
			PCDMemberNameList->Add(_shapeName);
			AddShapeMemberNameList->Remove(_shapeName);
		}*/
		PCDMeasureSelectionChanged::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0007", ex); }
}

System::Collections::ArrayList^ RWrapper::RW_PCD::GetPCDMeasureList()
{
	try
	{
		PCDlist->Clear();
		if(MAINDllOBJECT->CurrentPCDShape == NULL) return PCDlist;
		if(!MAINDllOBJECT->CurrentPCDShape->IsValid()) return PCDlist;
		CalculateAllvalues();
		return PCDlist;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0008", ex); }
}

//Calculate all values to generate excel report..//
void RWrapper::RW_PCD::CalculateAllvalues()
{
	try
	{
		//Add the RW_PCD diameter to the list...//
		PCDCircle* CShape = (PCDCircle*)MAINDllOBJECT->CurrentPCDShape;
		this->PCDlist->Add(gcnew System::String(CShape->getModifiedName()) + "_Dia");
		this->PCDlist->Add("");
		this->PCDlist->Add(CShape->Radius() / 500);
		CalcualteAlloffsetValues();
		CalcualteAllAngles();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0009", ex); }
}

//Calculate all offset values..
//Offset values of all RW_PCD members with the RW_PCD circle..//
void RWrapper::RW_PCD::CalcualteAlloffsetValues()
{
	try
	{
		System::String^ MeasureFormat = "0.0";
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		//Calcualte the Offset of RW_PCD member circles with the RW_PCD circle(Difference between the centers...)
		PCDCircle* CShape = (PCDCircle*)MAINDllOBJECT->CurrentPCDShape;
		double PcdCenter[2] = {CShape->getCenter()->getX(), CShape->getCenter()->getY()};
		double PcdRadius = CShape->Radius();
		for each(Shape* CSh in CShape->PCDParentCollection)
		{
			double ShCenter[2] = {((Circle*)CSh)->getCenter()->getX(), ((Circle*)CSh)->getCenter()->getY()};
			double OffsetDist = RMATH2DOBJECT->Pt2Pt_distance(PcdCenter[0], PcdCenter[1], ShCenter[0], ShCenter[1]);
			OffsetDist = PcdRadius - OffsetDist;
			if(abs(OffsetDist) < 0.00001) OffsetDist = 0;
			System::String^ Str = Microsoft::VisualBasic::Strings::Format(OffsetDist, MeasureFormat);
			this->PCDlist->Add("Offset" + "_"  +  gcnew System::String(CSh->getModifiedName()));
			this->PCDlist->Add("");
			this->PCDlist->Add(Str);	
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0010", ex); }
}

//Caculate all RW_PCD angles..
//Angle between RW_PCD members with respect to the center of the RW_PCD circle...//
//the angle is with respect to center of the RW_PCD circle... angle between the line joining the center of the RW_PCD wiht the center of circle1 and 
//.. line joining the center of RW_PCD with the line joining the center of Circle2;
void RWrapper::RW_PCD::CalcualteAllAngles()
{
	try
	{
		PCDCircle* CShape = (PCDCircle*)MAINDllOBJECT->CurrentPCDShape;
		double PcdCenter[2] = {CShape->getCenter()->getX(), CShape->getCenter()->getY()};
		Shape *CShape1, *CShape2;
		bool flag = true;
		for each(Shape* CSh in CShape->PCDParentCollection)
		{
			if(flag)
			{
				flag = false;
				CShape1 = CSh;
			}
			else
			{
				CShape2 = CSh;
				CalculatePCDAngles(CShape, CShape1, CShape2);
				CShape1 = CShape2;
			}
		}
		CalculatePCDAngles(CShape, CShape1, (Shape*)(*CShape->PCDParentCollection.begin()));
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0011", ex); }
}

void RWrapper::RW_PCD::CalculatePCDAngles(Shape* PCdShape, Shape* CShape1, Shape* CShape2)
{
	try
	{
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		System::String^ MeasureFormat = "0.0";
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		double PcdCenter[2] = {((Circle*)PCdShape)->getCenter()->getX(), ((Circle*)PCdShape)->getCenter()->getY()};
		double ShCenter1[2] = {((Circle*)CShape1)->getCenter()->getX(), ((Circle*)CShape1)->getCenter()->getY()};
		double ShCenter2[2] = {((Circle*)CShape2)->getCenter()->getX(), ((Circle*)CShape2)->getCenter()->getY()};
		double angle1 = RMATH2DOBJECT->ray_angle(PcdCenter[0], PcdCenter[1], ShCenter1[0], ShCenter1[1]);
		double angle2 = RMATH2DOBJECT->ray_angle(PcdCenter[0], PcdCenter[1], ShCenter2[0], ShCenter2[1]);
		double AngleDiff = angle2 - angle1;
		RMATH2DOBJECT->Angle_FourQuad(&AngleDiff);
		System::String^ PCDAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(AngleDiff, CMeasureMode, MeasureFormat, true);
		this->PCDlist->Add("Angle" + gcnew System::String(CShape1->getModifiedName()) + "_" + gcnew System::String(CShape2->getModifiedName()));
		this->PCDlist->Add("");
		this->PCDlist->Add(PCDAngle);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0012", ex); }
}

//Returns the circle/arc/point center values...//
void RWrapper::RW_PCD::GettheCenter(Shape* sh, double* center)
{
	try
	{
		if(sh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || sh->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			center[0] = ((Circle*)sh)->getCenter()->getX();
			center[1] = ((Circle*)sh)->getCenter()->getY();
		}
		else if(sh->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
		{
			center[0] = ((RPoint*)sh)->getPosition()->getX();
			center[1] = ((RPoint*)sh)->getPosition()->getY();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPCD0013", ex); }
}

void RWrapper::RW_PCD::ResetAllPCD()
{
	PCDMeasureSelectionChanged::raise();
}