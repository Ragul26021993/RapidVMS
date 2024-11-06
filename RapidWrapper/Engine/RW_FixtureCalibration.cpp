//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "..\MAINDLL\Actions\AddPointAction.h"
#include "..\MAINDll\Helper\Helper.h"
#include "..\MAINDll\Shapes\Line.h"
#include "..\MAINDll\Engine\PartProgramFunctions.h"
#include "RW_DRO.h"

void CALLBACK FramGrabCallbak()
{
	RWrapper::RW_FixtureCalibration::MYINSTANCE()->Create_New_Framgrab(false);
}

void CALLBACK Callibrate_Goto_NextCell()
{
	if(RWrapper::RW_FixtureCalibration::MYINSTANCE()->FixtureAutomatedCallibration)
	{
		RWrapper::RW_FixtureCalibration::MYINSTANCE()->StartAutomatedFixtureCallibration(true);
	}
	else
	{
		RWrapper::RW_FixtureCalibration::MYINSTANCE()->StartFixtureCallibration();
	}
}

///Default constructor.. Initialise the deafault settings..
RWrapper::RW_FixtureCalibration::RW_FixtureCalibration()

{
	 try
	 {
		this->FixtureCalInstance = this; this->PointOfRotationFlag = false; this->FixtureAutomatedCallibration = false;
		this->CellWidth = 0, this->CellHeight = 0, this->TotalRows = 0, this->TotalColumns = 0; this->ReferenceAngle = 0; this->FixtureOrientationAngle = 0;
		this->previousDroValue = gcnew cli::array<double>(2); this->PointOfRotation = gcnew cli::array<double>(2);
		this->CurrentFixureName = "";
		CellPosition = gcnew System::Collections::Generic::List<System::Double>();
		CorrectionList = gcnew System::Collections::Generic::List<System::Double>();
		this->CellCount = 0, this->listCount = 0;
		this->firstTime = true;
		HELPEROBJECT->CreateFramGrabCallback = &FramGrabCallbak;
		MAINDllOBJECT->Fixture_GotoNextCellCallBack = &Callibrate_Goto_NextCell;
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0001", ex); }
}

//Destructor.. Release the memory..
RWrapper::RW_FixtureCalibration::~RW_FixtureCalibration()
{
	try
	{
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex); }
}

bool RWrapper::RW_FixtureCalibration::Check_CallibrationInstance()
{
	if(FixtureCalInstance == nullptr)
		return false;
	else
		return true;
}

void RWrapper::RW_FixtureCalibration::Close_CallibrationWindow()
{
	try
	{
		if(FixtureCalInstance == nullptr) return;
		RWrapper::RW_FixtureCalibration::MYINSTANCE()->ClearAll();
		delete FixtureCalInstance;
		FixtureCalInstance = nullptr;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0003", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_FixtureCalibration^ RWrapper::RW_FixtureCalibration::MYINSTANCE()
{
	if(FixtureCalInstance == nullptr)
		FixtureCalInstance = gcnew RW_FixtureCalibration();
	return FixtureCalInstance;
}

void RWrapper::RW_FixtureCalibration::StartFixtureCallibration()
{
	try
	{
		Fixture_RferencePointEvent::raise(0);
		if(firstTime)
		{
			this->PointOfRotationFlag = false;
			//this->CellWidth = RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellWidth, this->CellHeight = RWrapper::RW_DBSettings::MYINSTANCE()->GridRowgap,
			//this->TotalRows = RWrapper::RW_FixtureCalibration::MYINSTANCE()->Tota, this->TotalColumns = RWrapper::RW_DBSettings::MYINSTANCE()->GridColumnCnt;
			this->CellPosition->Clear();
			this->CorrectionList->Clear();
			this->firstTime = false; listCount = 0;
			/*double target[2] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1]};*/
			double target[2] = {HELPEROBJECT->SnapPt_Pointer->getX(), HELPEROBJECT->SnapPt_Pointer->getY()};
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
			{
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					//this->ReferenceAngle = ((Line*)CShape)->Angle();
					double Point1[3] = {((Line*)CShape)->getPoint1()->getX(), ((Line*)CShape)->getPoint1()->getY(), ((Line*)CShape)->getPoint1()->getZ()};
					double Point2[3] = {((Line*)CShape)->getPoint2()->getX(), ((Line*)CShape)->getPoint2()->getY(), ((Line*)CShape)->getPoint2()->getZ()};
					this->ReferenceAngle = RMATH2DOBJECT->ray_angle(Point1, Point2);
				}
			}
			if(this->TotalColumns > 0 && this->TotalRows > 0)
			{
				for(int i = 0; i < TotalRows; i++)
				{				
					if(i > 0)
					{
						target[1] = target[1] - CellHeight;	
					}
					for(int j = 0; j < TotalColumns; j++)
					{
						if(j > 0 || i > 0)
						{
							if(j > 0)
								target[0] = target[0] + CellWidth;
							else
								target[0] = HELPEROBJECT->SnapPt_Pointer->getX();
								//target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
							CellPosition->Add(target[0]);
							CellPosition->Add(target[1]);		
						}
					}
				}
				if(MAINDllOBJECT->ReferenceDotShape != NULL)
				{
					PointOfRotationFlag = true;
					this->PointOfRotation[0] = HELPEROBJECT->SnapPt_Pointer->getX() - PPCALCOBJECT->HomePosition.getX();
					this->PointOfRotation[1] = HELPEROBJECT->SnapPt_Pointer->getY() - PPCALCOBJECT->HomePosition.getY();
				}
				double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
				previousDroValue[0] = CellPosition[listCount++], previousDroValue[1] = CellPosition[listCount++];
				double Target[4] = {previousDroValue[0], previousDroValue[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};								
				CellCount++;
				if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&Target[0], &feedrate[0], TargetReachedCallback::FIXTURE_CALIBRATION);	
			}
			else
				return;
		}
		else
		{
			if(CellCount < TotalColumns * TotalRows)
			{
				if(CellCount > 0)
				{
					CorrectionList->Add(HELPEROBJECT->SnapPt_Pointer->getX() - previousDroValue[0]);
					CorrectionList->Add(HELPEROBJECT->SnapPt_Pointer->getY() - previousDroValue[1]);
					Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
					if(CShape != NULL)
					{
						if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
						{
							double Point1[3] = {((Line*)CShape)->getPoint1()->getX(), ((Line*)CShape)->getPoint1()->getY(), ((Line*)CShape)->getPoint1()->getZ()};
							double Point2[3] = {((Line*)CShape)->getPoint2()->getX(), ((Line*)CShape)->getPoint2()->getY(), ((Line*)CShape)->getPoint2()->getZ()};
							double Angle = RMATH2DOBJECT->ray_angle(Point1, Point2) - this->ReferenceAngle;
							CorrectionList->Add(Angle);
						}
						else
							CorrectionList->Add(0);
					}
					else
						CorrectionList->Add(0);
					if(CellCount == TotalColumns * TotalRows - 1)
					{
						RWrapper::RW_FixtureCalibration::MYINSTANCE()->writeCorrectionValues();
						MAINDllOBJECT->ShowMsgBoxString("RW_FixtureCalibration01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
						return;
					}
				}
				double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
				previousDroValue[0] = CellPosition[listCount++], previousDroValue[1] = CellPosition[listCount++];
				double Target[4] = {previousDroValue[0], previousDroValue[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};								
				CellCount++;
				if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&Target[0], &feedrate[0], TargetReachedCallback::FIXTURE_CALIBRATION);										
			}
			else
				RWrapper::RW_FixtureCalibration::MYINSTANCE()->writeCorrectionValues();	
		}
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0004", ex); }
}

void RWrapper::RW_FixtureCalibration::writeCorrectionValues()
{
	try
	{
		const char* tempchar = (const char*)(Marshal::StringToHGlobalAnsi(CurrentFixureName)).ToPointer();
		System::IO::FileInfo^ CalibrationFileInfo = gcnew System::IO::FileInfo(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Database\\FixtureCalibDetails.txt"); // + CurrentFixureName);
		//if(CalibrationFileInfo->Exists)
		//	CalibrationFileInfo->Delete();
		//const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
		//std::string temporaryFixtureName = temporarychar;
		//std::wofstream myfile;
		//std::string filename = temporaryFixtureName + "\\Database\\" + tempchar;
		//myfile.open(filename.c_str());
		//myfile.precision(15);
		System::IO::StreamWriter^ myfile = gcnew System::IO::StreamWriter(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Database\\FixtureCalibDetails.txt", true);
		myfile->WriteLine(this->CurrentFixureName);
		myfile->WriteLine("CalibrationValue");; // << endl;
		if(this->PointOfRotationFlag)
		{
			myfile->WriteLine("PointOfRotation"); // << endl;
			myfile->WriteLine(this->PointOfRotation[0]); // << endl;
			myfile->WriteLine(this->PointOfRotation[1]); // << endl;
		}
		
		for each(double Cvalue in CorrectionList)
			myfile->WriteLine(Cvalue);// << endl;
		myfile->WriteLine("EndCalibration"); // << endl;
		myfile->WriteLine();
		myfile->Close();
		//RWrapper::RW_FixtureCalibration::MYINSTANCE()->ClearAll();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0006", ex); }
}

void RWrapper::RW_FixtureCalibration::ReadCorrectionValues()
{
	System::IO::StreamReader^ sr;
	try
	{
		this->FixtureCalibValues_Loaded = false;
		sr = gcnew System::IO::StreamReader(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Database\\FixtureCalibDetails.txt");
		int count = 0;
		Double currentValue = 0;
		System::Collections::Generic::List<String^>^ FixtureData = gcnew System::Collections::Generic::List<String^>();
		bool GotCurrentFixtureValues = false;
		while (!sr->EndOfStream)
		{
			if (!GotCurrentFixtureValues)
			{
				if (sr->ReadLine() == this->CurrentFixureName) GotCurrentFixtureValues = true;
			}
			else
			{
				FixtureData->Add(sr->ReadLine());
				if (FixtureData[FixtureData->Count - 1] == "EndCalibration") break;
			}
		}
		if (FixtureData[0] != "CalibrationValue") return;
		if (FixtureData[1] == "PointOfRotation")
		{
			this->PointOfRotationFlag = true; 
			this->PointOfRotation[0] = System::Double::Parse(FixtureData[2]);
			this->PointOfRotation[1] = System::Double::Parse(FixtureData[3]);
			FixtureData->RemoveRange(0, 4);
		}
		else
			this->PointOfRotationFlag = false;

		for (int i = 0; i < FixtureData->Count - 1; i++)
		{
			if (System::Double::TryParse(FixtureData[i], currentValue))
			{
				this->CorrectionList->Add(currentValue);
			}
		}
		this->FixtureCalibValues_Loaded = true;
		sr->Close();
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0006a", ex);
		
	}
}

void RWrapper::RW_FixtureCalibration::UpdateFixureDetails_Frontend(System::String^ name, int rcount, int ccount, double rgap, double cgap)
{
	try
	{
		//this->CurrentFixureName = name;
		this->TotalRows = rcount;
		this->TotalColumns = ccount;
		this->CellHeight = rgap;
		this->CellWidth = cgap;
		this->ReadCorrectionValues();
	}
	catch (Exception^ ex) { 
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0011a", ex);
	}
}


void RWrapper::RW_FixtureCalibration::ClearAll()
{
	try
	{
		this->FixtureAutomatedCallibration = false;
		this->PointOfRotationFlag = false;
		MAINDllOBJECT->AutomatedMeasureModeFlag(false);
		HELPEROBJECT->SnapPtForFixture = false;
		HELPEROBJECT->IsFixtureCallibration = false;
		//if(this->CellPosition->Count > 0)
		//	this->CellPosition->Clear();
		//if(this->CellPosition->Count > 0)
		//	this->CorrectionList->Clear();
		this->listCount = 0;
		this->CellCount = 0; 
		//this->listCount = 0; this->CellHeight = 0; this->TotalColumns = 0;
		this->firstTime = true; 
		//this->CellWidth = 0; this->TotalRows = 0; this->ReferenceAngle = 0; this->FixtureOrientationAngle = 0;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0007", ex); }
}

void RWrapper::RW_FixtureCalibration::getFixtureName(System::String^ FixtureName)
{
	try
	{
		this->CurrentFixureName = FixtureName;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0008", ex); }
}

void RWrapper::RW_FixtureCalibration::SetFixtureCallibrationMode()
{
	try
	{
		MAINDllOBJECT->AutomatedMeasureModeFlag(true);
		FixtureAutomatedCallibration = true;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0009", ex); }
}

void RWrapper::RW_FixtureCalibration::StartAutomatedFixtureCallibration(bool IncrementCellCount)
{
	try
	{
		if(firstTime)
		{
			if(HELPEROBJECT->SnapPt_Pointer == NULL) return;
			previousDroValue[0] = HELPEROBJECT->SnapPt_Pointer->getX(); previousDroValue[1] = HELPEROBJECT->SnapPt_Pointer->getY();
			Vector* snpPt = HELPEROBJECT->SnapPt_Pointer;
			
			MAINDllOBJECT->AutomatedMeasureModeFlag(false);
			//this->CellWidth = RWrapper::RW_DBSettings::MYINSTANCE()->GridColumngap, this->CellHeight = RWrapper::RW_DBSettings::MYINSTANCE()->GridRowgap,
			//this->TotalRows = RWrapper::RW_DBSettings::MYINSTANCE()->GridRowcnt, this->TotalColumns = RWrapper::RW_DBSettings::MYINSTANCE()->GridColumnCnt;
			this->CellPosition->Clear();
			this->CorrectionList->Clear();
			this->firstTime = false;
			double target[2] = {0, 0};
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
			{
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
					this->ReferenceAngle = ((Line*)CShape)->Angle();
			}
			if(MAINDllOBJECT->ReferenceDotShape != NULL)
			{
				PointOfRotationFlag = true;
				this->PointOfRotation[0] = HELPEROBJECT->SnapPt_Pointer->getX() - PPCALCOBJECT->HomePosition.getX();
				this->PointOfRotation[1] = HELPEROBJECT->SnapPt_Pointer->getY() - PPCALCOBJECT->HomePosition.getY();
			}
			if(this->TotalColumns > 0 && this->TotalRows > 0)
			{
				for(int i = 0; i < TotalRows; i++)
				{				
					if(i > 0)
					{
						target[1] = target[1] - CellHeight;	
					}
					for(int j = 0; j < TotalColumns; j++)
					{
						if(j > 0 || i > 0)
						{
							if(j > 0)
								target[0] = target[0] + CellWidth;
							CellPosition->Add(target[0]);
							CellPosition->Add(target[1]);		
						}
					}
				}				
			}
			else
				return;
		}
		get_Shape_Parameter(true);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0010", ex); }
}

//get shape from list of shapes.......
void RWrapper::RW_FixtureCalibration::get_Shape_Parameter(bool firstShape)
{
	try
	{
		if(firstShape)
		{	
			if(CellCount == TotalColumns * TotalRows - 1)
			{
				RWrapper::RW_FixtureCalibration::MYINSTANCE()->writeCorrectionValues();
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->ShowMsgBoxString("RW_FixtureCalibration01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
				return;
			}		
			for each(Shape* Shp in MAINDllOBJECT->Automated_ShapeTypeList)
			{
				((ShapeWithList*)Shp)->PointsList->deleteAll();
				((ShapeWithList*)Shp)->PointsListOriginal->deleteAll();
				Vector Trans(CellPosition[CellCount * 2], CellPosition[CellCount * 2 + 1]);
				Shp->Translate(Trans);
			}		
			int k = MAINDllOBJECT->Automated_ShapeTypeList.size();
			if(k > 0)
			{
				HELPEROBJECT->Shapeitr = MAINDllOBJECT->Automated_ShapeTypeList.begin();	
				Create_New_Framgrab(true);
			}	
		}
		else if(HELPEROBJECT->Shapeitr == MAINDllOBJECT->Automated_ShapeTypeList.end())
		{			
			CellCount++;
			if(CellCount == 1)
			{
				previousDroValue[0] += CellPosition[(CellCount - 1) * 2];
				previousDroValue[1] += CellPosition[(CellCount - 1) * 2 + 1];
			}
			else
			{
				previousDroValue[0] += CellPosition[(CellCount - 1) * 2] - CellPosition[(CellCount - 2) * 2];
				previousDroValue[1] += CellPosition[(CellCount - 1) * 2 + 1] - CellPosition[(CellCount - 2) * 2 + 1];
			}
			////target[0] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0] + (ColumnCount * RWrapper::RW_DBSettings::MYINSTANCE()->GridColumngap) + CorrectionValue[3 * RowCount * RWrapper::RW_DBSettings::MYINSTANCE()->GridColumnCnt + 3 * ColumnCount - 3];
			////target[1] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1] - (RowCount * RWrapper::RW_DBSettings::MYINSTANCE()->GridRowgap) + CorrectionValue[3 * RowCount * RWrapper::RW_DBSettings::MYINSTANCE()->GridColumnCnt + 3 * ColumnCount - 2];
			//int currentCol = CellCount / (TotalRows * 2);
			//int currentRow = (CellCount % (TotalRows * 2)) / 2;
			//previousDroValue[0] = HELPEROBJECT->SnapPt_Pointer

			if(HELPEROBJECT->SnapPt_Pointer == NULL)
			{
				Vector Temp(previousDroValue[0], previousDroValue[1]);
				UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
				HELPEROBJECT->SnapPt_Pointer = MAINDllOBJECT->getVectorPointer_UCS(&Temp, ucs, true);
			}
			if(HELPEROBJECT->SnapPt_Pointer == NULL) 
				return;
			CorrectionList->Add(HELPEROBJECT->SnapPt_Pointer->getX() - previousDroValue[0]);
			CorrectionList->Add(HELPEROBJECT->SnapPt_Pointer->getY() - previousDroValue[1]);
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
			{
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					double Angle = this->ReferenceAngle - ((Line*)CShape)->Angle();
					//if (Angle > M_PI_2) Angle -= M_PI;
					CorrectionList->Add(Angle);
				}
				else
					CorrectionList->Add(0);
			}
			else
				CorrectionList->Add(0);
			previousDroValue[0] = HELPEROBJECT->SnapPt_Pointer->getX(); previousDroValue[1] = HELPEROBJECT->SnapPt_Pointer->getY();
			//Repeat same process for next cellcount...
			StartAutomatedFixtureCallibration(false);
		}
		else
		{
			Create_New_Framgrab(true);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWFixCal0011", __FILE__, __FUNCSIG__); }
}

//create new framgrab for selected shape......
void RWrapper::RW_FixtureCalibration::Create_New_Framgrab(bool firstFramgrab)
{
	try
	{
		ShapeWithList* myshapeList = (ShapeWithList*)(*HELPEROBJECT->Shapeitr);
		if(!myshapeList->selected())
		{
			std::list<int> PtIdlist;
			PtIdlist.push_back(myshapeList->getId());
			MAINDllOBJECT->selectShape(&PtIdlist);
		}		
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		double Target[4] = {CellPosition[CellCount * 2], CellPosition[CellCount * 2 + 1], 0, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};								
		if(firstFramgrab)
		{
			int j = myshapeList->PointAtionList.size();
			if(j > 0)
			{
				HELPEROBJECT->Framgrabitr = myshapeList->PointAtionList.begin(); 
				//get framgrab dro current position.......
				Vector &curDroPos = ((AddPointAction*)(*HELPEROBJECT->Framgrabitr))->getFramegrab()->PointDRO;
				Target[0] += curDroPos.getX(); Target[1] += curDroPos.getY(); Target[2] += curDroPos.getZ(); 
				if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&Target[0], &feedrate[0], TargetReachedCallback::FIXTURE_CALIBRATION_FRAMGRAB);
			}
		}
		else
		{
			HELPEROBJECT->Framgrabitr++;
			if(HELPEROBJECT->Framgrabitr == myshapeList->PointAtionList.end())
			{
				HELPEROBJECT->Shapeitr++;
				get_Shape_Parameter(false);
				return;
			}
			else
			{
				Vector &curDroPos = ((AddPointAction*)(*HELPEROBJECT->Framgrabitr))->getFramegrab()->PointDRO;
				Target[0] += curDroPos.getX(); Target[1] += curDroPos.getY(); Target[2] += curDroPos.getZ(); 
				if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&Target[0], &feedrate[0], TargetReachedCallback::FIXTURE_CALIBRATION_FRAMGRAB);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWFixCal0012", __FILE__, __FUNCSIG__); }
}

void RWrapper::RW_FixtureCalibration::get_FramGrab_Parameter()
{
	try
	{
		Vector difference(CellPosition[CellCount * 2], CellPosition[CellCount * 2 + 1], 0);
		HELPEROBJECT->getFramGrabParameter(difference);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWFixCal0013", __FILE__, __FUNCSIG__); }
}

void RWrapper::RW_FixtureCalibration::TakeReferencePoint_FixtureCallibration()
{
	try
	{
		HELPEROBJECT->SnapPtForFixture = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RWFixCal0014", __FILE__, __FUNCSIG__); }
}