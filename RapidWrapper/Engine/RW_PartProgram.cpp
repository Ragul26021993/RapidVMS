#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Shapes\RPoint.h"
#include "..\MAINDLL\DXF\DXFExpose.h"
#include "..\MAINDLL\Actions\AddImageAction.h"
#include "..\MAINDLL\Engine\FocusFunctions.h"
#include "..\MAINDll\Engine\ThreadCalcFunctions.h"
#include "..\MAINDll\Engine\PartProgramFunctions.h"
#include "..\MAINDLL\Actions\CircularInterPolationAction.h"
#include "..\MAINDLL\Actions\LinearInterPolationAction.h"
#include "..\MAINDLL\Actions\ProfileScanAction.h"
#include "..\MAINDLL\Shapes\Line.h"
#include "..\MainDLL\Helper\Helper.h"

#pragma warning( disable : 4267 )  

bool rotateimage = true;
double PrevTarget[4];

void CALLBACK FinishedTwoStepManualHoming()
{
	try
	{
		if(!MAINDllOBJECT->AutomaticTwoStepHoming())
		{
			//if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			//	DROInstance->ActivateCNC();
			if(RWrapper::RW_PartProgram::MYINSTANCE()->ProgramMode)
				RWrapper::RW_CNC::MYINSTANCE()->Activate_CNCModeDRO();
			//RWrapper::RW_PartProgram::MYINSTANCE()->HandlePartprogram();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WR_PPC001", ex); }
}

void CALLBACK FinishedProbeManualAction()
{
	try
	{
		if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			DROInstance->ActivateCNC();
		if(RWrapper::RW_PartProgram::MYINSTANCE()->ProgramMode)
			RWrapper::RW_CNC::MYINSTANCE()->Activate_CNCModeDRO();
		RWrapper::RW_PartProgram::MYINSTANCE()->HandlePartprogram();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WR_PPC002", ex); }
}

void CALLBACK FinishedFirstPtActionIdentification()
{
	try
	{
		RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_ContinueBuild(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WR_PPC003", ex); }
}

void CALLBACK StopPartProgram()
{
	try
	{
		RWrapper::RW_PartProgram::MYINSTANCE()->Abort_PartProgram();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WR_PPC004", ex); }
}

void CALLBACK RectangleExtentForImageAction()
{
	try
	{
		RWrapper::RW_PartProgram::MYINSTANCE()->SaveImageRectangleExtent();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WR_PPC005", ex); }
}

RWrapper::RW_PartProgram::RW_PartProgram()
{
	try
	{
		//Load all partprogram and report file names in partprogram panel textboxes...//
		//this->PointOfRotationFlag = false;
		this->SaveReferencePtMode = false;
		this->CorrectRotationalError = false;
		this->NumberOfCommandSend = 0;
		this->InterPolationCommandCount = 0;
		this->BatchOfCommandCompleted = true;
		 this->PPInstance = this;
		 this->ReturnTohomePosition = false;
		 this->AutoFocusDuringPartprogram = false;
		 this->ProgramMode = false;
		 //this->CorrectionValue = gcnew System::Collections::Generic::List<System::Double>();
		 this->FixtureReferenceAngle = 0;
		 this->RunStepsOneByOne = false;

		 this->ProgramStepTable = gcnew System::Data::DataTable();
		 this->ProgramStepTable->Columns->Add("S No");
		 this->ProgramStepTable->Columns->Add("Shape");
		 this->ProgramStepTable->Columns->Add("Type");
		 this->ProgramStepTable->Columns->Add("Steps");
		 this->ProgramStepTable->Columns->Add("Points");
		 this->ProgramStepTable->Columns->Add("Status");
		 this->ProgramStepTable->Columns->Add("Id");
		 this->PartProgramPaused = false;
		 this->RunningFailedAction = false;
		 this->CNC_Assisted_Mode = MAINDllOBJECT->CNCAssisted_FG();
		 ImageFilePath = "";
		 HomePosition = (double*)calloc(4, sizeof(double));
		 this->ReferenceModeF = false;
		 PPCALCOBJECT->FinishedTwoStepHomingMode = &FinishedTwoStepManualHoming;
		 PPCALCOBJECT->AbortPartProgram = &StopPartProgram;
		 PPCALCOBJECT->FinishedFirstPointActionIdentification = &FinishedFirstPtActionIdentification;
		 PPCALCOBJECT->FinishedFirstPointActionForProbe = &FinishedProbeManualAction;
		 PPCALCOBJECT->CreateRectangleExtentForImage = &RectangleExtentForImageAction;
		 ResetGridProgramCount();
		 this->SameCommand = 0;
		 MAINDllOBJECT->PrevLightCommand = "aaa";
		 MAINDllOBJECT->PrevCameraSetting = "bbb";
		 for (int i = 0; i < 4; i++)
			 PrevTarget[i] = -10000;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0001", ex); }
}

RWrapper::RW_PartProgram::~RW_PartProgram()
{
}

RWrapper::RW_PartProgram^ RWrapper::RW_PartProgram::MYINSTANCE()
{  
	  return PPInstance;
}

void RWrapper::RW_PartProgram::SaveCurrentDROValue()
{
	for(int i = 0; i < 4; i++)
		HomePosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
}

void RWrapper::RW_PartProgram::UpdateProgramStepsTable()
{
	try
	{
		int StepsCount = 0;
		ProgramStepTable->Rows->Clear();
		System::String ^PPstepCnt = "", ^PPShapeName = "", ^PPShapeTypeName = "", ^PPstepActn = "", ^PPNoofPts = "", ^PPstepStatus = "", ^PPfgId = "";
		Shape *Cshape, *LastCshape;
		FramegrabBase *Cframegrab, *LastCframegrab;

		for(RC_ITER ppCAction = PPCALCOBJECT->getPPActionlist().getList().begin(); ppCAction != PPCALCOBJECT->getPPActionlist().getList().end(); ppCAction++)
		{
			RAction* Caction = (RAction*)(*ppCAction).second;
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				Shape* Cshape = ((AddPointAction*)Caction)->getShape();
				FramegrabBase* Cframegrab = ((AddPointAction*)Caction)->getFramegrab();
				if(StepsCount > 1)
				{
					if(Cshape->ShapeAsfasttrace() && LastCshape->ShapeAsfasttrace())
					{
						if(Cshape->getId() != LastCshape->getId())
						{
							LastCshape = Cshape; LastCframegrab = Cframegrab;
							continue;
						}
					}
				}
				StepsCount++;
				PPstepCnt = System::Convert::ToString(StepsCount);
				PPShapeName = gcnew System::String(Cshape->getModifiedName());
				PPShapeTypeName = gcnew System::String((char*)Cshape->getCurrentShapeTypeName().c_str());
				if(PPShapeTypeName == "Default" || PPShapeTypeName == "Dummy")
					continue;
				PPstepActn = gcnew System::String((char*)Cframegrab->getCurrentCrosshairTypeName().c_str());
				PPNoofPts = System::Convert::ToString((int)Cframegrab->noofpts);
				PPfgId = System::Convert::ToString(Cframegrab->getId());
				ProgramStepTable->Rows->Add(PPstepCnt, PPShapeName, PPShapeTypeName, PPstepActn, PPNoofPts, PPstepStatus, PPfgId);
				LastCshape = Cshape; LastCframegrab = Cframegrab;
			}
		}
		UpdateProgramStep::raise();
		//Sleep(100);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0019", ex); }
}

void RWrapper::RW_PartProgram::UpdateProgramStepsTableForEdit()
{
	try
	{
		int StepsCount = 0;
		ProgramStepTable->Rows->Clear();
		System::String ^PPstepCnt = "", ^PPShapeName = "", ^PPShapeTypeName = "", ^PPstepActn = "", ^PPNoofPts = "", ^PPstepStatus = "", ^PPfgId = "";
		Shape *Cshape, *LastCshape;
		FramegrabBase *Cframegrab, *LastCframegrab;

		for(RC_ITER ppCAction = MAINDllOBJECT->getActionsHistoryList().getList().begin(); ppCAction != MAINDllOBJECT->getActionsHistoryList().getList().end(); ppCAction++)
		{
			RAction* Caction = (RAction*)(*ppCAction).second;
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				Shape* Cshape = ((AddPointAction*)Caction)->getShape();
				FramegrabBase* Cframegrab = ((AddPointAction*)Caction)->getFramegrab();
				if(StepsCount > 1)
				{
					if(Cshape->ShapeAsfasttrace() && LastCshape->ShapeAsfasttrace())
					{
						if(Cshape->getId() != LastCshape->getId())
						{
							LastCshape = Cshape; LastCframegrab = Cframegrab;
							continue;
						}
					}
				}
				StepsCount++;
				PPstepCnt = System::Convert::ToString(StepsCount);
				PPShapeName = gcnew System::String(Cshape->getModifiedName());
				PPShapeTypeName = gcnew System::String((char*)Cshape->getCurrentShapeTypeName().c_str());
				if(PPShapeTypeName == "Default" || PPShapeTypeName == "Dummy")
					continue;
				PPstepActn = gcnew System::String((char*)Cframegrab->getCurrentCrosshairTypeName().c_str());
				PPNoofPts = System::Convert::ToString((int)Cframegrab->noofpts);
				PPfgId = System::Convert::ToString(Caction->getId());
				ProgramStepTable->Rows->Add(PPstepCnt, PPShapeName, PPShapeTypeName, PPstepActn, PPNoofPts, PPstepStatus, PPfgId);
				LastCshape = Cshape; LastCframegrab = Cframegrab;
			}
		}
		UpdateProgramStep::raise();
		Sleep(100);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0019A", ex); }
}

void RWrapper::RW_PartProgram::UpdateCurrentPPStepStatus(int Fgid, int PointCnt, bool status)
{
	try
	{
		for(int i = 0; i < ProgramStepTable->Rows->Count; i++)
		{
			if(Fgid == System::Convert::ToInt32(ProgramStepTable->Rows[i]->default["Id"]))
			{
				if(status)
					this->ProgramStepTable->Rows[i]->default["Status"] = "Success";
				else
					this->ProgramStepTable->Rows[i]->default["Status"] = "Failed";
				this->ProgramStepTable->Rows[i]->default["Points"] = System::Convert::ToString(PointCnt);
				break;
			}
		}
		UpdateProgramStep::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0019", ex); }
}

 void RWrapper::RW_PartProgram::PartProgram_EditAlign(System::String^ BtnName)
 {
	 try
	 {
		 if(BtnName == "1 Point")
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PPALIGNMENT_1PT);
		 if(BtnName == "2 Points")
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PPALIGNMENT_2PT);
		 if(BtnName == "1 Point 1 Line")
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PPALIGNMENT_1PT1LINE);
		  else if(BtnName == "Reset Align")
			DXFEXPOSEOBJECT->ResetDxfAlignment();
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0019", ex); }
 }

bool RWrapper::RW_PartProgram::PartProgram_Build(System::String^ FilePath)
{
	try
	{
		MAINDllOBJECT->SetStatusCode("RW_PartProgram19");
		MAINDllOBJECT->AddAction_PPEditMode = false;
		MAINDllOBJECT->ShowHideWaitCursor(1);
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("PartProgram_Build", "Build PartProgram", "PartProgram Tab");
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return false;
		}
		System::IO::StreamWriter^ tst = gcnew System::IO::StreamWriter(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath,"\\ProgramLog\\ProgramDetails.txt"),true);
		tst->WriteLine("Program build:");
		tst->WriteLine("Program Name:  " + FilePath);
		tst->WriteLine("Time and Date:  " + Microsoft::VisualBasic::DateAndTime::Now.ToString());
		tst->WriteLine("");
		tst->Close();
		BuildFilePath = FilePath;
		if (PPCALCOBJECT->CreateProfileLineArcFrameGrabs())
		{
			PartProgram_ContinueBuild(true);
			return true;		
		}
		else
		{
			MAINDllOBJECT->ShowHideWaitCursor(2);
			return false;
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0019a", ex); return false;}
}

void RWrapper::RW_PartProgram::PartProgram_ContinueBuild(bool same_thread)
{
	try
	{
		PPCALCOBJECT->buildpp((char*)(void*)Marshal::StringToHGlobalAnsi(BuildFilePath).ToPointer());
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			System::String^ TempStr = BuildFilePath;
			System::String^ TempStr1 = TempStr->Substring(0, TempStr->Length - 5);
			System::String^ ImFilePath;
			//if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			//	ImFilePath = TempStr1 + "_OneShotImage.dat";
			//else
			ImFilePath = TempStr1 + "_OneShotImage.jpg"; // .bmp"; // .bin"; // 
			RWrapper::RW_MainInterface::MYINSTANCE()->SaveImageForOneShot(ImFilePath);
		}
		//copy first action image, if it exists, and save with appropriate name in appropriate folder.  This will be called during program run and displayed in dxf window.
		System::String^ FirstPtActionImPath = gcnew System::String((char*) MAINDllOBJECT->currentPath.c_str()); 
		FirstPtActionImPath += "\\FirstPointActionImage.jpg";

		System::String^ tmpStr = BuildFilePath;
		System::String^ tmpStr1 = tmpStr->Substring(0, tmpStr->Length - 5);
		System::String^ TgtFilePath = tmpStr1 + "_FirstPointActionImage.jpg";
		System::IO::FileInfo^ RefImageinfo = gcnew System::IO::FileInfo(FirstPtActionImPath);
		if(RefImageinfo->Exists)
			System::IO::File::Copy(FirstPtActionImPath, TgtFilePath, true);
		
		//save rcad window image after doing zoom to extents and save with another appropriate name in appropriate folder.  This will be called during program preview for
		//mouse hover and displayed in a small pop-up window.
		System::String^ ProgPreviewImage = tmpStr1 + "_ProgramPreviewImage.jpg";
		if(MAINDllOBJECT->RcadZoomExtents())
			MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
		//MAINDllOBJECT->WriteStringToFile("PartProgram_ContinueBuild");
		if(PPCALCOBJECT->ExportPointsToCsvInPP())
		{			
			DXFEXPOSEOBJECT->CreatePointCsvFile((char*)(void*)Marshal::StringToHGlobalAnsi(BuildFilePath).ToPointer());
			DXFEXPOSEOBJECT->WriteToPointCsvFile((char*)(void*)Marshal::StringToHGlobalAnsi(BuildFilePath).ToPointer());
		}
		if(PPCALCOBJECT->ExportParamToCsvInPP())
		{			
			DXFEXPOSEOBJECT->CreateShapesParamCsvFile((char*)(void*)Marshal::StringToHGlobalAnsi(BuildFilePath).ToPointer());
			DXFEXPOSEOBJECT->WriteToShapesParamCsvFile((char*)(void*)Marshal::StringToHGlobalAnsi(BuildFilePath).ToPointer());
		}
		MAINDllOBJECT->ClearShapeMeasureSelections();
		RWrapper::RW_MainInterface::MYINSTANCE()->SaveRcadWindowImage(ProgPreviewImage, false);
		if (!same_thread)
			BuildProgramCompleted::raise();
		MAINDllOBJECT->ShowHideWaitCursor(2);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0008", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_Load(System::String^ FilePath)
{	
	try
	{
		MAINDllOBJECT->ShowHideWaitCursor(1);
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("PartProgram_Load", "Load PartProgram", "Program Name List");
		PPCALCOBJECT->PPAlignEditFlag(false);
		this->OneShot_RefImage_Loaded = false;
		PPCALCOBJECT->IntersectionPointMissed(false);
		if(MAINDllOBJECT->getActionsHistoryList().getList().size() > 2)
		{
			if(MAINDllOBJECT->DemoMode() || MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Load01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
			{
				LoadProgram(FilePath);
			}
			else
				RWrapper::RW_MainInterface::MYINSTANCE()->InternalClearAll(2);
		}
		else
		{
			LoadProgram(FilePath);
		}
		MAINDllOBJECT->ShowHideWaitCursor(2);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0009", ex); }
}

void RWrapper::RW_PartProgram::LoadProgram(System::String^ FilePath)
{
	try
	{
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			System::String^ TempStr = FilePath;
			System::String^ TempStr1 = TempStr->Substring(0, TempStr->Length - 5);
			ImageFilePath = TempStr1 + "_OneShotImage.jpg";
			//this->LoadedRefImage_OneShot = false;

			System::IO::FileInfo^ RefImageinfo = gcnew System::IO::FileInfo(ImageFilePath);
			if(!RefImageinfo->Exists)
			{
				ImageFilePath = TempStr1 + "_OneShotImage.bmp";
				RefImageinfo = gcnew System::IO::FileInfo(ImageFilePath);
				if (!RefImageinfo->Exists)
				{
					ImageFilePath = TempStr1 + "_OneShotImage.bin";
					RefImageinfo = gcnew System::IO::FileInfo(ImageFilePath);
					if (!RefImageinfo->Exists)
					{
						MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramLoadProgram01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
						RWrapper::RW_MainInterface::MYINSTANCE()->InternalClearAll(2);
						return;
					}
				}
			}
		}
		std::wifstream ifile;
		ifile.open((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer());
		if(ifile.fail())
		{
			 ifile.close();
			 MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramLoadProgram02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			 RWrapper::RW_MainInterface::MYINSTANCE()->InternalClearAll(2);
			 return;
		}
		ifile.close();
		//System::Windows::Forms::MessageBox::Show("Got the pointer to File", "Rapid-I");
		//Reset All the UCS... and Other panel properties//
		RWrapper::RW_MainInterface::MYINSTANCE()->InternalClearAll(1);
		//System::Windows::Forms::MessageBox::Show("Finished Internal Clear", "Rapid-I");
		Sleep(50);
		PPCALCOBJECT->ProgramMadeUsingVblockAxis(false);
		PPCALCOBJECT->ProgramMadeUsingReferenceDot(false);
		PPCALCOBJECT->ProgramMadeUsingFirstShapeAsReference(false);
		//Load the Parprogram...
		PPCALCOBJECT->loadpp((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer());
		//System::Windows::Forms::MessageBox::Show("Finished Loading Program", "Rapid-I");

		PPCALCOBJECT->SetActionIdForNogoMeasurement();
		//PPCALCOBJECT->ArrangeForShortestPath();
		PPCALCOBJECT->ProgramRunningForFirstTime(true);
		PPCALCOBJECT->PartprogramLoaded(true);
		MAINDllOBJECT->changeUCS(0);
		Sleep(20);
		UpdateProgramStepsTable();
		LoadedFilePath = FilePath;
		
		if(PPCALCOBJECT->ProgramMeasureUnitType == RapidEnums::RAPIDUNITS::MM)
			UpdateMeasureUnit::raise("mm");
		else
			UpdateMeasureUnit::raise("Inches");
		if(MAINDllOBJECT->RcadZoomExtents())
			MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
		System::IO::StreamWriter^ tst = gcnew System::IO::StreamWriter(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath,"\\ProgramLog\\ProgramDetails.txt"),true);
		tst->WriteLine("Program load:");
		tst->WriteLine("Program Name:  " + FilePath);
		tst->WriteLine("Time and Date:  " + Microsoft::VisualBasic::DateAndTime::Now.ToString());
		tst->WriteLine("");
		tst->Close();
		//System::Windows::Forms::MessageBox::Show("Finished Settings Init and Log Writing", "Rapid-I");

		//load first action image, if it exists
		System::String^ tmpStr = FilePath;
		System::String^ tmpStr1 = tmpStr->Substring(0, tmpStr->Length - 5);
		System::String^ FirstPtActionImPath = tmpStr1 + "_FirstPointActionImage.jpg";
		System::IO::FileInfo^ FirstPtActionImageinfo = gcnew System::IO::FileInfo(FirstPtActionImPath);
		if(FirstPtActionImageinfo->Exists && !PPCALCOBJECT->IntersectionPointMissed())
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->RaiseSavedImageEvent(FirstPtActionImPath);
		}
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			bool Checkflag = RWrapper::RW_PartProgram::MYINSTANCE()->LoadReferecneImage_OneShot(ImageFilePath);
			if (!Checkflag)
			{
				MAINDllOBJECT->SetStatusCode("RW_PartProgram03");
				System::Windows::Forms::MessageBox::Show("Did not load Ref Image Properly", "Rapid-I");
			}
				//return;
		}

		//System::Windows::Forms::MessageBox::Show("Got First Image and stored in HDD", "Rapid-I");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0010", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_Edit(System::String^ FilePath)
{
	try
	{
		MAINDllOBJECT->SetStatusCode("RW_PartProgram20");
		MAINDllOBJECT->AddAction_PPEditMode = false;
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(!PPCALCOBJECT->PartprogramLoaded())
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Edit01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(PPCALCOBJECT->ProgramMadeUsingReferenceDot())
		{
			if(!PPCALCOBJECT->ReferenceDotIstakenAsHomePosition())
			{
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Edit02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				return;
			}
		}
		if(PPCALCOBJECT->ProgramMadeUsingVblockAxis())
		{
			if(!PPCALCOBJECT->VblockAxisAsReference())
			{
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Edit02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				return;
			}
		}
		bool referencedotFlag = PPCALCOBJECT->ReferenceDotIstakenAsHomePosition();
		RWrapper::RW_MainInterface::MYINSTANCE()->InternalClearAll(1);
		PPCALCOBJECT->ReferenceDotIstakenAsHomePosition(referencedotFlag);
		PPCALCOBJECT->Editpp((char*)(void*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer());
		UpdateProgramStepsTableForEdit();
		System::IO::StreamWriter^ tst = gcnew System::IO::StreamWriter(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath,"\\ProgramLog\\ProgramDetails.txt"),true);
		tst->WriteLine("Program Edit:");
		tst->WriteLine("Program Name:  " + FilePath);
		tst->WriteLine("Time and Date:  " + Microsoft::VisualBasic::DateAndTime::Now.ToString());
		tst->WriteLine("");
		tst->Close();
		RWrapper::RW_Enum::PROGRAM_PARAMETER PartProgramParam = RWrapper::RW_Enum::PROGRAM_PARAMETER(PPCALCOBJECT->ResetActionParameter());
		if(PartProgramParam != RWrapper::RW_Enum::PROGRAM_PARAMETER::PP_NULL)
			RaisePP_ParamResetEvent(PartProgramParam);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0011", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_Run()
{
	try
	{
		if (PPCALCOBJECT->IsPartProgramRunning())
		{
			if (PPCALCOBJECT->ProgramMadeUsingTwoStepHoming() && PPCALCOBJECT->IsPartProgramRunning() && PPCALCOBJECT->FinishedManualTwoStepAlignment() || PartProgramPaused)
			{//	this->HandlePartprogram();
			//else if (PartProgramPaused)
			//{
				PartProgramPaused = false;
				HandlePartprogram();
			}
			return;
		}
		MeasurementCount = 0;
		RowCount = 0;
		ColumnCount = 0;
		CurrentCount = 0;
		MAINDllOBJECT->IsPartProgramPause = false;
		RWrapper::RW_PartProgram::MYINSTANCE()->NumberOfCommandSend = 0;
		this->SaveReferencePtMode = false;
		//this->PointOfRotationFlag = false;
		this->FixtureReferenceAngle = 0;
		PartProgramPaused = false;
		//PrevLightCommand = "";
		for (int j = 0; j < 4; j++)
			PrevTarget[j] = -10000;
		PPCALCOBJECT->RefreshImageForEdgeDetection = true;

		//Reset Last Lighting and CamSettings sent
		MAINDllOBJECT->PrevLightCommand = "aaa";
		MAINDllOBJECT->PrevCameraSetting = "bbb";

		MAINDllOBJECT->SetStatusCode("RW_PartProgram17");
		if(RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag)
		{
			//RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList->Clear();
			//this->CorrectionValue->Clear();
			//for(int i = 0; i < 3 * (RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns * RWrapper::RW_DBSettings::MYINSTANCE()->GridRowcnt - 1); i++)
			//	this->CorrectionValue->Add(0);
			//RWrapper::RW_PartProgram::MYINSTANCE()->ReadCorrectionValueForGrid();
			this->CorrectRotationalError = true;
			if (!RWrapper::RW_FixtureCalibration::MYINSTANCE()->FixtureCalibValues_Loaded)
				RWrapper::RW_FixtureCalibration::MYINSTANCE()->ReadCorrectionValues();
		}
		//else
		//	RWrapper::RW_PartProgram::MYINSTANCE()->ResetGridProgramCount();
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("PartProgram_Run", "Run PartProgram", "PartProgram Tab");
		if(!PPCALCOBJECT->PartprogramLoaded())
		{
			RunIsnotValid::raise();
			MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(!PPCALCOBJECT->ReferenceDotIstakenAsHomePosition())
		{
			if(PPCALCOBJECT->ProgramMadeUsingFirstShapeAsReference())
			{
				RunIsnotValid::raise();
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				return;
			}
		}
		
		if(!PPCALCOBJECT->VblockAxisAsReference())
		{
			if(PPCALCOBJECT->ProgramMadeUsingVblockAxis())
			{
				RunIsnotValid::raise();
				//change message for vblock axis...
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				return;
			}
		}

		if(PPCALCOBJECT->UseImageComparision() || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			if (MAINDllOBJECT->CameraConnected())
			{

				if (SendCurrentStepLightCommand(PPCALCOBJECT->ppFirstgrab->getLightIntensity()) == 2)
					RWrapper::RW_CNC::MYINSTANCE()->ConvertLightValueToInt(gcnew System::String(PPCALCOBJECT->ppFirstgrab->getLightIntensity()));
				
				std::string currCamSetting(PPCALCOBJECT->ppFirstgrab->getCamProperties());
				if (MAINDllOBJECT->PrevCameraSetting != currCamSetting)
					MAINDllOBJECT->SetCamProperty(PPCALCOBJECT->ppFirstgrab->getCamProperties());
				MAINDllOBJECT->PrevCameraSetting = currCamSetting;
				//Sleep(100); //was 1000 before
			}
			if(!PPCALCOBJECT->ProgramDoneForIdorOdMeasurement())
			{
				if (!this->SingleComponentinOneShot)
				{
					int Cnt = PPCALCOBJECT->GetAlltheImages();
					if (Cnt == 0)
					{
						MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
						return;
					}
				}
				//if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
				//{
				//	PartProgramFinished();
				//	return;
				//}
				//else
				//{
					bool Checkflag = OneShotpartprogramRun();
					if (!Checkflag)
					{
						//PartProgram_GridRun();
						//if(PPCALCOBJECT->CurrentProramCount >= PPCALCOBJECT->TotalNoOfImages)
						//	RunIsnotValid::raise();
						if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
							PartProgramFinished();
						else
							MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run05", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
						return;
					}
				//}
			}
		}
		System::IO::StreamWriter^ tst = gcnew System::IO::StreamWriter(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath,"\\ProgramLog\\ProgramDetails.txt"),true);
		tst->WriteLine("Program Run:");
		tst->WriteLine("");
		tst->Close();
		PPCALCOBJECT->UpdateFistFramegrab(false);
		for(int i = 0; i < 4; i++)
			HomePosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		this->ProgramMode = RWrapper::RW_CNC::MYINSTANCE()->getCNCMode();
		MAINDllOBJECT->RcadGraphicsMoveWithDRO(false);
		if(PPCALCOBJECT->ProgramMadeUsingTwoReferenceDot())
		{
			double Point1[2] = {PPCALCOBJECT->HomePosition.getX(), PPCALCOBJECT->HomePosition.getY()}, Point2[2] = {PPCALCOBJECT->HomePosition_2nd.getX(), PPCALCOBJECT->HomePosition_2nd.getY()};
			double Angle1 = RMATH2DOBJECT->ray_angle(Point1, Point2);
			Point1[0] = PPCALCOBJECT->PPHomePosition.getX(); Point1[1] = PPCALCOBJECT->PPHomePosition.getY();
			Point2[0] = PPCALCOBJECT->PPHomePosition_2nd.getX(); Point2[1] = PPCALCOBJECT->PPHomePosition_2nd.getY();
			double Angle2 = RMATH2DOBJECT->ray_angle(Point1, Point2);
			FixtureReferenceAngle = Angle1 - Angle2;
			if (FixtureReferenceAngle < 0.000000001) FixtureReferenceAngle = 0; //If angle = 10^-9 rad, then we will have an error of 0.5um for a length of 500 mm, which is the largest we can travel in a 4030. So anything less than this shall be 0!
		}
		else if(PPCALCOBJECT->ProgramMadeUsingRefLine())
		{
			if(MAINDllOBJECT->ReferenceLineShape != NULL)
			{
				FixtureReferenceAngle = ((Line*)MAINDllOBJECT->ReferenceLineShape)->Angle() - PPCALCOBJECT->ReferenceLineAngle;
				if(FixtureReferenceAngle > M_PI_2)
					FixtureReferenceAngle -= M_PI;
				else if(FixtureReferenceAngle < -M_PI_2)
					FixtureReferenceAngle += M_PI;
			}
		}
		StartPartProgramRun();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0012", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_GridRun()
{
	try
	{
		this->CorrectRotationalError = true;
		if(this->SaveReferencePtMode && !PPCALCOBJECT->PartProgramAlignmentIsDone())
		{
			PPCALCOBJECT->ReferencePtAsHomePos(true);
		}
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("PartProgram_GridRun", "GridRun PartProgram", "PartProgram Tab");
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			if(PPCALCOBJECT->CurrentProramCount >= PPCALCOBJECT->TotalNoOfImages)
			{ 
				PPCALCOBJECT->ClearImageListMemory();
				//StartPartProgramRun();
				return;
			}
			//bool Checkflag = OneShotpartprogramRun();
			//if(!Checkflag)
			//{
			//	PartProgram_GridRun();
			//	return;
			//}
		}
		StartPartProgramRun();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0013", ex); }
}

bool RWrapper::RW_PartProgram::OneShotpartprogramRun()
{
	try
	{
		PPCALCOBJECT->CurrentProramCount = PPCALCOBJECT->CurrentProramCount + 1;
		bool Checkflag = false;
		if(PPCALCOBJECT->UseImageComparision())
		{
			System::String^ tmpStr = LoadedFilePath;
			System::String^ tmpStr1 = tmpStr->Substring(0, tmpStr->Length - 5);
			System::String^ FirstPtActionImPath = tmpStr1 + "_FirstPointActionImage.jpg";
			Checkflag = RWrapper::RW_PartProgram::MYINSTANCE()->LoadReferecneImage_OneShot(FirstPtActionImPath);
			//System::Windows::Forms::MessageBox::Show("Got into Image Comparison Mode", "Rapid-I");

		}
		else
		{
			if (!this->OneShot_RefImage_Loaded)
				Checkflag = RWrapper::RW_PartProgram::MYINSTANCE()->LoadReferecneImage_OneShot(ImageFilePath);
			else
				Checkflag = true;
		}
		if(!Checkflag)
		{
			MAINDllOBJECT->SetStatusCode("RW_PartProgram03");
			return false;
		}

		if (this->SingleComponentinOneShot)
			Checkflag = PPCALCOBJECT->SetObjectImage_OneShot();
		else
			Checkflag = PPCALCOBJECT->SetObjImage_OneShot();

		if(!Checkflag)
		{
			MAINDllOBJECT->SetStatusCode("RW_PartProgram04");
			return false;
		}

		if(RWrapper::RW_DBSettings::MYINSTANCE()->PartialmageModeForOneShot)
		{
			Checkflag = PPCALCOBJECT->Calculate_BestMatch();
			if(!Checkflag)
			{
				MAINDllOBJECT->SetStatusCode("RW_PartProgram05");
				return false;
			}
		}
		else
		{
			//we have already calculated CG when loading ref image. So lets do only for object image and save time
			int AlignParams[6] = { 0 };
			Checkflag = PPCALCOBJECT->RedefineOneShot_AboutCG(false, AlignParams);
			System::IO::StreamWriter^ tst = gcnew System::IO::StreamWriter(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath, "\\ProgramLog\\ProgramDetails.txt"), true);
			tst->WriteLine("Align Params:");
			for (int jj = 0; jj < 6; jj++)
				tst->Write(AlignParams[jj] + ", ");
			tst->WriteLine("");
			tst->Close();

			if(!Checkflag)
			{
				if (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
					MAINDllOBJECT->SetStatusCode("RW_PartProgram06");
				return false;
			}
			
			Checkflag = PPCALCOBJECT->CalculateImage_RotateAngle();
			if(!Checkflag)
			{
				MAINDllOBJECT->SetStatusCode("RW_PartProgram07");
				return false;
			}
		}
		return true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0014", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_Pause()
{
	try
	{
		if(MAINDllOBJECT->Continuous_ScanMode)
			MAINDllOBJECT->ContinuePauseRotationalScan(true);
		PartProgramPaused = true;
		MAINDllOBJECT->SetStatusCode("RW_PartProgram16");
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("PartProgram_Pause", "Pause PartProgram", "PartProgram Tab");
		if(!BatchOfCommandCompleted)
		{
			DROInstance->PauseCurrentcommand();
			return;
		}
		this->ProgramMode = RWrapper::RW_CNC::MYINSTANCE()->getCNCMode();
		/*if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			DROInstance->ToManual();*/
		if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0015", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_Continue()
{
	try
	{
		if(MAINDllOBJECT->Continuous_ScanMode)
			MAINDllOBJECT->ContinuePauseRotationalScan(false);
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("PartProgram_Continue", "Continue PartProgram", "PartProgram Tab");
		/*if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			DROInstance->ToManual();*/
		if(!BatchOfCommandCompleted)
		{
			DROInstance->ResumeCurrentCommand();
			return;
		}	
		if(this->ProgramMode)
		{   
			if ((MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::DSP) && (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP))
			{
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				return;
			}
		}
		if(PPCALCOBJECT->getCurrentAction()== NULL)
		{
			PartProgramFinished();
			return;
		}
		
		HandlePartprogram();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0016", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_Stop()
{
	try
	{
		if(MAINDllOBJECT->Continuous_ScanMode)
			MAINDllOBJECT->ContinuePauseRotationalScan(true);
		MAINDllOBJECT->IsPartProgramPause = false;
		PartProgramPaused = false;
		MAINDllOBJECT->SetStatusCode("RW_PartProgram18");
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("PartProgram_Stop", "Stop PartProgram", "PartProgram Tab");
		PPCALCOBJECT->PPDontUpdateSnapPts(false);
		if(ReferenceModeF)
		{
			PPCALCOBJECT->ReferencePtAsHomePos(true);
			ReferenceModeF = false;
		}
		//ResetGridProgramCount();
		//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		this->ProgramMode = false;
		MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		PPCALCOBJECT->DrawPartprogramArrow(false);
		PPCALCOBJECT->IsPartProgramRunning(false);
		PPCALCOBJECT->FirstMousedown_Partprogram_Manual(false);
		PPCALCOBJECT->FirstMousedown_Partprogram(false);
		PPCALCOBJECT->StartingPointofPartprogram(true);
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0017", ex); }
}

void RWrapper::RW_PartProgram::PartProgram_HomePosition()
{
	try
	{
		double target[4] = {HomePosition[0], HomePosition[1], HomePosition[2], HomePosition[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0018", ex); }
}

void RWrapper::RW_PartProgram::Handle_Program_btnClicks(System::String^ Str)
 {
	 try
	 {
		 RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("Handle_Program_btnClicks", Str + "Button clicks for PartProgram", "PartProgram Tab");
		 if(Str == "Reference Dot")
		 {
			 if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHTGAUGE_VISION)
			 {
				if(MAINDllOBJECT->ReferenceDotShape != NULL) return;
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER3D);
				 ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
				 MAINDllOBJECT->ReferenceDotShape = CShape;
				 PointCollection PtColl;
				 PtColl.Addpoint(new SinglePoint(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]));
				 CShape->AddPoints(&PtColl);
				 MAINDllOBJECT->SetShapeForReferenceDot();
			 }
			 else
			 {
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram11");
				 if(PPCALCOBJECT->SelectedShapesForTwoStepHoming())
				 {
					MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramHandle_Program_btnClicks01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					MAINDllOBJECT->SetStatusCode("RW_PartProgram08");
					return;
				 }
				 if(PPCALCOBJECT->IsPartProgramRunning())	
				 {
					MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
					return;
				 }
				 if(!MAINDllOBJECT->CameraConnected()) return;
				 if(MAINDllOBJECT->ReferenceDotShape != NULL) return;
				 if (MAINDllOBJECT->CurrentCameraType == 3)
				 {
					 //std::string RefDotCamSettings = "000000FF800180A0808014";
					 MAINDllOBJECT->SetCamProperty(MAINDllOBJECT->RefDotCamSettings); // RefDotCamSettings);
				 }
				 PPCALCOBJECT->CalculateReferenceDotPosition();

				 if (!RWrapper::RW_DBSettings::MYINSTANCE()->FiveXLensePresent)
				 {
					 RWrapper::RW_FocusDepth::MYINSTANCE()->SetSelectedAxis(2);
					 FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::HOMING_MODE;
					 int focusBoxsize = 250;
					 //focusBoxsize = RW_DBSettings::MYINSTANCE()->RefDotDia_in_mm * HELPEROBJECT->CamSizeRatio / MAINDllOBJECT->getWindow(0).getUppX();
					 RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(1, 1, 0, focusBoxsize, focusBoxsize, 0.6, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed); //RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan
					 FOCUSCALCOBJECT->FocusOnStatus(false);
					 //FOCUSCALCOBJECT->drawFocusRectangels();
					 RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(4);
				 }
				 else
				 {
					 FOCUSCALCOBJECT->FocusStatusFlag(true);
					 PPCALCOBJECT->SetReferenceDotPosition();
					 //MAINDllOBJECT->ShowHideWaitCursor(2);
					 MAINDllOBJECT->update_VideoGraphics(false);
					 MAINDllOBJECT->update_RcadGraphics(false);
				 }
			 }
		 }
		 else if(Str == "Reference Sphere")
		 {
			 if(MAINDllOBJECT->ReferenceDotShape != NULL) return;
			 MAINDllOBJECT->CreateReferenceSphere();
		 }
		 else if(Str == "2nd Reference Dot")
		 {
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram11");
			 if(PPCALCOBJECT->SelectedShapesForTwoStepHoming())
			 {
				 MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramHandle_Program_btnClicks01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram08");
				 return;
			 }
			 if(PPCALCOBJECT->IsPartProgramRunning())	
			 {
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
				 return;
			 }
			 if(!MAINDllOBJECT->CameraConnected()) return;
			 if(MAINDllOBJECT->ReferenceDotShape == NULL) return;
			 if(MAINDllOBJECT->SecondReferenceDotShape != NULL) return;

			 if (MAINDllOBJECT->CurrentCameraType == 3)
			 {
				 std::string RefDotCamSettings = "000000FF800180A0808014";
				 MAINDllOBJECT->SetCamProperty(RefDotCamSettings);
			 }

			 PPCALCOBJECT->CalculateSecondReferenceDotPosition();

			 if (!RWrapper::RW_DBSettings::MYINSTANCE()->FiveXLensePresent)
			 {
				 FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND;
				 RWrapper::RW_FocusDepth::MYINSTANCE()->SetSelectedAxis(2);
				 RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(1, 1, 0, 80, 80, 0.6, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed); //RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan
				 FOCUSCALCOBJECT->FocusOnStatus(false);
				 RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(4);
			 }
			 else
			 {
				 FOCUSCALCOBJECT->FocusStatusFlag(true);
				 PPCALCOBJECT->SetReferenceDotPosition();
				 MAINDllOBJECT->ShowHideWaitCursor(2);
				 MAINDllOBJECT->update_VideoGraphics(false);
				 MAINDllOBJECT->update_RcadGraphics(false);
			 }
		 }
		 else if(Str == "Reference Line")
		 {
			 if(MAINDllOBJECT->ReferenceLineShape != NULL) return;
			 MAINDllOBJECT->CreateRefernceLineShape();
		 }
		 else if (Str == "Probe Position")
		 {
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram10");
			 if (PPCALCOBJECT->IsPartProgramRunning())
			 {
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
				 return;
			 }
			 if (MAINDllOBJECT->CurrentProbeType == RapidEnums::PROBETYPE::SIMPLE_PROBE)
			 {
			 }
			 else
			 {
				 RWrapper::RW_DRO::MYINSTANCE()->SelectProbePointSide = false;
				 //RWrapper::RW_DRO::MYINSTANCE()->RaiseTouchedProbePostion(); 
			 }
			 MAINDllOBJECT->NotedownProbePath();
			 Console::Beep(600, 500);
		 }
		 else if(Str == "Save Video Image")
		 {
			 if(PPCALCOBJECT->IsPartProgramRunning()) return;		 
			 if(MAINDllOBJECT->CameraConnected())
			 {
				 RWrapper::RW_DRO::MYINSTANCE()->RaiseCameraClickSoundEvent();
				 MAINDllOBJECT->NotedDownImagePickPoint();
				 RC_ITER item = MAINDllOBJECT->getActionsHistoryList().getList().end();
				 item--;
				 RAction* MyAction = (RAction*)((*item).second);
				if(MyAction->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
				{	
					int Image_Count = ((AddImageAction*)MyAction)->ImageCount;
					System::String^ filePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "//Image//" + System::Convert::ToString(Image_Count) + ".jpg";
					RWrapper::RW_MainInterface::MYINSTANCE()->SaveVideoWindowImage(filePath, false, false);
				}
			 }
		 }
		 else if(Str == "Pause Position")
		 {
			 if(PPCALCOBJECT->IsPartProgramRunning())	
			 {
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
				return;
			 }
			 MAINDllOBJECT->AddPauseAfterAction();
		 }
		 else if(Str == "Return to Home")
		 {
			  MAINDllOBJECT->SetStatusCode("RW_PartProgram12");
			 this->ReturnTohomePosition = !this->ReturnTohomePosition;
		 }
		 else if(Str == "Part Program Home")
		 {
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram14");
		 }
		 else if(Str == "Auto Focus During Program")
		 {
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram13");
			this->AutoFocusDuringPartprogram = !this->AutoFocusDuringPartprogram;
		 }
		 else if(Str == "Grid Run")
		 {
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram15");
			 RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag = !RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag;
			 //if(!RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag)
				// RWrapper::RW_PartProgram::MYINSTANCE()->ResetGridProgramCount();
		 }
		 else if(Str == "Single Step Run")
		 {
			 RunStepsOneByOne = !RunStepsOneByOne;
		 }
		 else if(Str == "Sigma Mode")
		 {
			 PPCALCOBJECT->SigmaModeInPP = !PPCALCOBJECT->SigmaModeInPP;
		 }
		 else if(Str == "Use Image comparison")
		 {
			 PPCALCOBJECT->UseImageComparision(!PPCALCOBJECT->UseImageComparision());
		 }
		 else if(Str == "Program 1Point")
		 {
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROGRAM_1PT_ALIGNMENT);
		 }
		 else if(Str == "Program 2Point")
		 {
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROGRAM_2PT_ALIGNMENT);
		 }
		 else if(Str == "Program 1Line 1Point")
		 {
			  MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROGRAM_1LN_1PTALIGNMENT);
		 }
		 else if(Str == "Clear Ref Dot and Sphere")
		 {
			 if(MAINDllOBJECT->ReferenceDotShape != NULL)
			 {
				MAINDllOBJECT->deleteReferenceShape((ShapeWithList*)MAINDllOBJECT->ReferenceDotShape);
				MAINDllOBJECT->ReferenceDotShape = NULL;
				PPCALCOBJECT->ReferenceDotIstakenAsHomePosition(false);
			 }
			 if(MAINDllOBJECT->Vblock_CylinderAxisLine != NULL)
			 {
				 MAINDllOBJECT->deleteReferenceShape((ShapeWithList*)MAINDllOBJECT->Vblock_CylinderAxisLine);
				 MAINDllOBJECT->Vblock_CylinderAxisLine = NULL;
				 PPCALCOBJECT->VblockAxisAsReference(false);
			 }
			 if(MAINDllOBJECT->SecondReferenceDotShape != NULL)
			 {
				 MAINDllOBJECT->deleteReferenceShape((ShapeWithList*)MAINDllOBJECT->SecondReferenceDotShape);
				 MAINDllOBJECT->SecondReferenceDotShape = NULL;
				 PPCALCOBJECT->ReferenceDotIstakenAsSecondHomePos(false);
			 }
			 if(MAINDllOBJECT->ReferenceLineShape != NULL)
			 {
				 MAINDllOBJECT->deleteReferenceShape((ShapeWithList*)MAINDllOBJECT->ReferenceLineShape);
				 MAINDllOBJECT->ReferenceLineShape = NULL;
				 PPCALCOBJECT->ReferenceLineAsRefAngle(false);
			 }
			 MAINDllOBJECT->UpdateShapesChanged();
			 FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOCUSDEPTH_MODE;

		 }
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0024", ex); }
 }

void RWrapper::RW_PartProgram::StartPartProgramRun()
{
	try
	{
		//DROInstance->WritedebugInfo("(',')------------------ProgramRun-----------(',')");
		PPCALCOBJECT->FinishedManualTwoStepAlignment(false);
		//RWrapper::RW_CNC::MYINSTANCE()->Reset_PrevTarget();
		//CNC_Assisted_Mode = MAINDllOBJECT->CNCAssisted_FG();

		if(PPCALCOBJECT->IntersectionPointMissed())
		{
			//LoadProgram(LoadedFilePath);
			if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			{
				if(MAINDllOBJECT->CameraConnected()) 
				{
					if (SendCurrentStepLightCommand(PPCALCOBJECT->ppFirstgrab->getLightIntensity()) == 2)
						RWrapper::RW_CNC::MYINSTANCE()->ConvertLightValueToInt(gcnew System::String(PPCALCOBJECT->ppFirstgrab->getLightIntensity()));

					std::string currCamSetting(PPCALCOBJECT->ppFirstgrab->getCamProperties());
					if (MAINDllOBJECT->PrevCameraSetting != currCamSetting)
						MAINDllOBJECT->SetCamProperty(PPCALCOBJECT->ppFirstgrab->getCamProperties());
					MAINDllOBJECT->PrevCameraSetting = currCamSetting;
				}
			}
			else
			{
				LoadProgram(LoadedFilePath);
				//if (PPCALCOBJECT->ProgramMadeUsingTwoStepHoming() && !MAINDllOBJECT->AutomaticTwoStepHoming()) LoadProgram(LoadedFilePath);
				//please do alignment again...
				if (PPCALCOBJECT->PartProgramAlignmentIsDone())
				{
					MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramFinished03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					Abort_PartProgram();
					return;
				}
			}

			PPCALCOBJECT->UpdateFistFramegrab(false);
			if(!RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag)
			{
				for(int i = 0; i < 4; i++)
					HomePosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			}
			this->ProgramMode = RWrapper::RW_CNC::MYINSTANCE()->getCNCMode();
			MAINDllOBJECT->RcadGraphicsMoveWithDRO(false);
			PPCALCOBJECT->IntersectionPointMissed(false);
		}
		System::String^ pathstring = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Image";
		System::IO::DirectoryInfo^ ImageInfo = gcnew System::IO::DirectoryInfo(pathstring);
		if (ImageInfo->Exists)
		{
			cli::array<String^>^dirs = System::IO::Directory::GetFiles(pathstring);
			for each(System::String^ filename in dirs)
			{
				System::IO::File::Delete(filename);
			}
		}
		else
			System::IO::Directory::CreateDirectory(pathstring);
		
		//PPCALCOBJECT->PPDontUpdateSnapPts(true);
		PPCALCOBJECT->PPDontUpdateSnapPts(false);
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteUserLog("StartPartProgramRun", "GridRun Loaded Finished", "Run_1");

		if(PPCALCOBJECT->BeginPartProgram())
		{			
			rotateimage = true;
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteCommandSendLog("Program Run");
			UpdateProgramStepsTable();
			//If we have done homing of machine, run the program in the same coordinates as it was made. No need to shift it. 
			PPCALCOBJECT->ShiftProgram = !RWrapper::RW_MainInterface::MYINSTANCE()->Do_Homing_At_Startup;

			/*if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			{
				PartProgramFinished();
				return;
			}
			else */
			if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			{
				MAINDllOBJECT->SetZoomedImageInEdgeDetection();
				if(!PPCALCOBJECT->ProgramDoneForIdorOdMeasurement())
					PPCALCOBJECT->RotateFgGraphics_OneShot();
			}
			if(PPCALCOBJECT->ReferencePtAsHomePos())
			{	
				//if grid program is running then do global rotation of action..
				//if(RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag && FixtureReferenceAngle != 0)
				if(FixtureReferenceAngle != 0)
				{
					double RotationPt[2] = {PPCALCOBJECT->PPHomePosition.getX(), PPCALCOBJECT->PPHomePosition.getY()};
					PPCALCOBJECT->RotateActionForGridRun(RotationPt, FixtureReferenceAngle);
				}
				
				if(MAINDllOBJECT->ReferenceDotShape != NULL)
					MAINDllOBJECT->ReferenceDotShape->selected(false);
				
				RWrapper::RW_PartProgram::MYINSTANCE()->ReferenceModeF = true; 
				Vector temp(0, 0, 0);
				PPCALCOBJECT->ShiftShape_ForFirstMouseDown(temp, true);
				if(!this->SaveReferencePtMode)
				{
					FramegrabBase* fg =  PPCALCOBJECT->getFrameGrab();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0] = fg->PointDRO.getX();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1] = fg->PointDRO.getY();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[2] = fg->PointDRO.getZ();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[3] = fg->PointDRO.getR();
				}
				this->SaveReferencePtMode = true;
				PPCALCOBJECT->ReferencePtAsHomePos(false);
			}
			else if(PPCALCOBJECT->PartProgramAlignmentIsDone())
			{
				if(!this->SaveReferencePtMode)
				{
					FramegrabBase* fg =  PPCALCOBJECT->getFrameGrab();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0] = fg->PointDRO.getX();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1] = fg->PointDRO.getY();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[2] = fg->PointDRO.getZ();
					RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[3] = fg->PointDRO.getR();
				}
				this->SaveReferencePtMode = true;
			}
			//PPCALCOBJECT->WriteDebugInfo(); 
			if(PPCALCOBJECT->ProgramMadeUsingTwoStepHoming())
			{
				//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
				if(!MAINDllOBJECT->AutomaticTwoStepHoming())
					 RWrapper::RW_CNC::MYINSTANCE()->Activate_ManualModeDRO();
				//DROInstance->ToManual();
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram09");
			}
			else if(PPCALCOBJECT->FirstProbePointActionFlag)
				RWrapper::RW_CNC::MYINSTANCE()->Activate_ManualModeDRO();
			if(this->CorrectRotationalError)
			{
				this->CorrectRotationalError = false;
				if(RWrapper::RW_FixtureCalibration::MYINSTANCE()->PointOfRotationFlag && MAINDllOBJECT->ReferenceDotShape != NULL)
				{
					FramegrabBase* fg =  PPCALCOBJECT->getFrameGrab();
					PPCALCOBJECT->CopyFirstActionProperties(fg);
					if(PPCALCOBJECT->IsPartProgramRunning())
					{
						Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
						Vector temp(CurrDRO.getX(), CurrDRO.getY(), CurrDRO.getZ(), CurrDRO.getR());
						PPCALCOBJECT->ShiftShape_ForFirstMouseDown(temp, PPCALCOBJECT->ReferencePtAsHomePos());
					}
					//this is used to rotate point of rotation for inner rotation...
					double Theta = RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 1];
					if(SaveReferencePtMode && FixtureReferenceAngle != 0 && Theta != 0)
					{
						double Transform_Matrix[9] = {0};
						Transform_Matrix[0] = cos(FixtureReferenceAngle); Transform_Matrix[1] = -sin(FixtureReferenceAngle); 
						Transform_Matrix[3] = sin(FixtureReferenceAngle); Transform_Matrix[4] = cos(FixtureReferenceAngle);
						Transform_Matrix[8] = 1;				
						double GloballyRotatedPt[2] = { RWrapper::RW_FixtureCalibration::MYINSTANCE()->PointOfRotation[0] + (ColumnCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellWidth) + RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 3],
														RWrapper::RW_FixtureCalibration::MYINSTANCE()->PointOfRotation[1] + RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 2] - (RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellHeight)};
						Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, GloballyRotatedPt[0], GloballyRotatedPt[1]);	
						double RotationPt[2] = {TmpValue.getX() + PPCALCOBJECT->HomePosition.getX(), TmpValue.getY() + PPCALCOBJECT->HomePosition.getY()};
						PPCALCOBJECT->RotateActionForGridRun(RotationPt, Theta);
					}
					else if(Theta != 0)
					{
						double RotationPt[2] = { RWrapper::RW_FixtureCalibration::MYINSTANCE()->PointOfRotation[0], RWrapper::RW_FixtureCalibration::MYINSTANCE()->PointOfRotation[1] }; //{PPCALCOBJECT->HomePosition.getX() + PointOfRotation[0], PPCALCOBJECT->HomePosition.getY() + PointOfRotation[1]};
						RotationPt[0] += (ColumnCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellWidth) + RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 3];
						RotationPt[1] += RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 2] - (RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellHeight);
						PPCALCOBJECT->RotateActionForGridRun(RotationPt, Theta);
					}
				}
				else //if(MAINDllOBJECT->ReferenceDotShape != NULL)
				{
					FramegrabBase* fg =  PPCALCOBJECT->getFrameGrab();
					if (fg->noofpts == 0)
					{
						int a = 0;
					}
					PPCALCOBJECT->CopyFirstActionProperties(fg);

					if(PPCALCOBJECT->IsPartProgramRunning())
					{
						Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
						Vector temp(CurrDRO.getX(), CurrDRO.getY(), CurrDRO.getZ(), CurrDRO.getR());
						PPCALCOBJECT->ShiftShape_ForFirstMouseDown(temp, PPCALCOBJECT->ReferencePtAsHomePos());
					}
				}
			}
			if (RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag)
			{
				int TotalRunCount = RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalRows;
				if (this->MeasurementCount == 0)
					GridRunCountEvent::raise( 1 + "/" + TotalRunCount);
			}
			this->HandlePartprogram();
		}
		else
		{
			PartProgramFinished();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0006", ex); }
}

FramegrabBase* ffg;

void RWrapper::RW_PartProgram::HandlePartprogram()
{
	try
	{
		this->BatchOfCommandCompleted = true;
		PPCALCOBJECT->RefreshImageForEdgeDetection = true;
		if(PPCALCOBJECT->FirstProbePointActionFlag && !PPCALCOBJECT->ActionTobeTransformed) return;
		//To highlight the currently running action in frontend
		if(PPCALCOBJECT->getCurrentAction() == NULL)
			PPCALCOBJECT->getPPActionlist().clearSelection();
		else
			PPCALCOBJECT->getCurrentAction()->selected(true);
		//If the current action is null or finished all the actions then dispaly the message....//
		if(PPCALCOBJECT->getCurrentAction() == NULL)
		{
			if(!PPCALCOBJECT->IsPartProgramRunning()) return;
			PPCALCOBJECT->PPDontUpdateSnapPts(false);
			PPCALCOBJECT->UpdateallSnapPtsForShapes();
			PPCALCOBJECT->ClearAfterProgramFinish();
			//MAINDllOBJECT->SetStatusCode("Please Wait..... Generating Report", 1);
			//Sleep(50);
			PartProgramFinished();
			return;
		}
		//According to the current action call the function from frame work..//
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			//if(MAINDllOBJECT->CameraConnected()) 
			//	SendCurrentStepLightCommand(PPCALCOBJECT->getCurrentActionLightProperty());
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if (fg->noofpts == 0)
			{
				int a = 0;
			}
			if (MAINDllOBJECT->CameraConnected() &&
				MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT &&
				MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
				MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			{
				ChangeZoomSelection(gcnew String(fg->magLevel.c_str()));
			}

			if (SendCurrentStepLightCommand(fg->getLightIntensity()) == 2)
				RWrapper::RW_CNC::MYINSTANCE()->ConvertLightValueToInt(gcnew System::String(fg->getLightIntensity()));

			std::string currCamSetting(fg->getCamProperties());
			if (MAINDllOBJECT->PrevCameraSetting != currCamSetting)
				MAINDllOBJECT->SetCamProperty(fg->getCamProperties());
			MAINDllOBJECT->PrevCameraSetting = currCamSetting;

			//CNC_Assisted_Mode = MAINDllOBJECT->CNCAssisted_FG();
			//Get the current action type...
			/*if(fg->UseDigitalZoom && !MAINDllOBJECT->DigitalZoomIsOn)
				 RWrapper::RW_MainInterface::MYINSTANCE()->GenerateToolBarClick("Tools", "Digital Zoom ON", false);
			else if(MAINDllOBJECT->DigitalZoomIsOn)
				 RWrapper::RW_MainInterface::MYINSTANCE()->GenerateToolBarClick("Tools", "Digital Zoom ON", true);*/

			if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION && !PPCALCOBJECT->ProgramRunningForFirstTime())
			{
				if(fg->ReferenceAction())
				{
					PPCALCOBJECT->IncrementAction();
					HandlePartprogram();
					return;
				}
			}

			if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				if(PPCALCOBJECT->ProgramMadeUsingTwoStepHoming() && PPCALCOBJECT->FinishedManualTwoStepAlignment())
				{
					if(fg->ActionForTwoStepHoming())
					{
						PPCALCOBJECT->IncrementAction();
						//HandlePartprogram();
						RWrapper::RW_DRO::MYINSTANCE()->Set2StepPP_Flag(true);
						return;
					}
				}
				
				if(PPCALCOBJECT->getCurrentPPShape()->IgnoreManualCrosshairSteps())
				{
					if(fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
					{
						PPCALCOBJECT->IncrementAction();
						//HandlePartprogram();
						RWrapper::RW_DRO::MYINSTANCE()->Set2StepPP_Flag(true);
						return;
					}
				}
			}

			if(PPCALCOBJECT->UseImageComparision() && rotateimage)
			{
				rotateimage = false;
				PPCALCOBJECT->RotateFgGraphics_AfterImageMatch();
			}

			//switch (fg->FGtype)
			//{
			//case RapidEnums::RAPIDFGACTIONTYPE::ROTATIONSCAN_FIXEDRECTANGLE:
			//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ANGULARRECTANGLEFRAMEGRAB:
			//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ARCFRAMEGRAB:
			//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_CIRCLEFRAMEGRAB:
			//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_RECTANGLEFRAMEGRAB:
			//	break;
			//default:
			//	PPCALCOBJECT->drawRunpp();
			//	break;
			//}
			if (fg->Auto_FG_AssistedMode && MAINDllOBJECT->CNCAssisted_FG())
				MAINDllOBJECT->CNCAssisted_FG(false);

			PPCALCOBJECT->drawRunpp();
			
			if (PPCALCOBJECT->getCurrentAction() == NULL) return;

			if (fg->CurrentWindowNo != 1 && RWrapper::RW_CNC::MYINSTANCE()->getCNCMode() && 
				(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
					MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT && 
					MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM))
			{
				ffg = fg;	
				RWrapper::RW_PartProgram::MYINSTANCE()->CalculateAndMovetoPosition(0);

				//System::Threading::ThreadStart^ gotoDelegate = gcnew System::Threading::ThreadStart(RWrapper::RW_PartProgram::MYINSTANCE(), &RWrapper::RW_PartProgram::CalculateAndMovetoPosition);
				//System::Threading::Thread^ gotoThread = gcnew System::Threading::Thread( gotoDelegate);
				//gotoThread->Start();

				//PPCALCOBJECT->drawRunpp();
			}
			else
			{
				PPCALCOBJECT->drawRunpp();
				if (fg->CurrentWindowNo == 1 && !fg->DroMovementFlag)
				{
					PPCALCOBJECT->RunCurrentStep();
					RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				}
				else
					PPCALCOBJECT->RunCurrentStep();
			}			
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::PERIMETERACTION)
		{
			PPCALCOBJECT->drawRunpp();
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode() && (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
																MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
																MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM))
			{				
				double target[4] = {fg->PointDRO.getX(),fg->PointDRO.getY(),fg->PointDRO.getZ(), fg->PointDRO.getR()};
				if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
				   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
				double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
				if(fg->CurrentWindowNo == 1 && !fg->DroMovementFlag)
				{
					PPCALCOBJECT->RunCurrentStep();
					RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				}
				else
				{
					if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
						target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];				
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_INCREMENT);
				}
			}
			else
			{
				if(fg->CurrentWindowNo == 1 && !fg->DroMovementFlag)
				{
					PPCALCOBJECT->RunCurrentStep();
					RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				}
				else
					PPCALCOBJECT->RunCurrentStep();
			}	
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
		{
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
			if(PPCALCOBJECT->ProgramMadeUsingTwoStepHoming() && !PPCALCOBJECT->FinishedManualTwoStepAlignment())
			{
				PPCALCOBJECT->IncrementAction();
				//HandlePartprogram();
				RWrapper::RW_DRO::MYINSTANCE()->Set2StepPP_Flag(true);
				return;
			}
			if(!MAINDllOBJECT->OneTouchProbeAlignment)
				PPCALCOBJECT->DoHoming();
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			double target[4] = {fg->PointDRO.getX(),fg->PointDRO.getY(),fg->PointDRO.getZ(), fg->PointDRO.getR()};
			if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
			   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_INCREMENT);
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
		{
			//if(MAINDllOBJECT->CameraConnected()) 
			//	SendCurrentStepLightCommand(PPCALCOBJECT->getCurrentActionLightProperty());
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if (SendCurrentStepLightCommand(fg->getLightIntensity()) == 2)
				RWrapper::RW_CNC::MYINSTANCE()->ConvertLightValueToInt(gcnew System::String(fg->getLightIntensity()));

			std::string currCamSetting(fg->getCamProperties());
			if (MAINDllOBJECT->PrevCameraSetting != currCamSetting)
				MAINDllOBJECT->SetCamProperty(fg->getCamProperties());
			MAINDllOBJECT->PrevCameraSetting = currCamSetting;

			double target[4] = {fg->PointDRO.getX(),fg->PointDRO.getY(),fg->PointDRO.getZ(), fg->PointDRO.getR()};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_SAVEIMAGE);
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::AREA_ACTION)
		{
			PPCALCOBJECT->drawRunpp();
			if(MAINDllOBJECT->CameraConnected()) 
				SendCurrentStepLightCommand(PPCALCOBJECT->getCurrentActionLightProperty());
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			double target[4] = {fg->PointDRO.getX(),fg->PointDRO.getY(),fg->PointDRO.getZ(), fg->PointDRO.getR()};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_ACTION);
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
		{
			bool withinView = false;
			THREADCALCOBJECT->ThreadMeasurementMode(true);
			THREADCALCOBJECT->CurrentSelectedThreadAction = (AddThreadMeasureAction*)PPCALCOBJECT->getCurrentAction();
			if(PPCALCOBJECT->getCurrentThreadAction() == NULL)
			{
				RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_ACTION);
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			}
			else
			{
				
				//if(MAINDllOBJECT->CameraConnected()) 
				//	SendCurrentStepLightCommand(PPCALCOBJECT->getCurrentThreadActionLightProperty());

				FramegrabBase* fg = PPCALCOBJECT->getThreadFrameGrab();
				if (SendCurrentStepLightCommand(PPCALCOBJECT->ppFirstgrab->getLightIntensity()) == 2)
					RWrapper::RW_CNC::MYINSTANCE()->ConvertLightValueToInt(gcnew System::String(fg->getLightIntensity()));

				std::string currCamSetting(fg->getCamProperties());
				if (MAINDllOBJECT->PrevCameraSetting != currCamSetting)
					MAINDllOBJECT->SetCamProperty(fg->getCamProperties());
				MAINDllOBJECT->PrevCameraSetting = currCamSetting;

				PPCALCOBJECT->drawThreadRunpp();
				MAINDllOBJECT->update_VideoGraphics();
				double target[4] = {fg->PointDRO.getX(),fg->PointDRO.getY(),fg->PointDRO.getZ(), fg->PointDRO.getR()};
				double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
				RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_ACTION);
				if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
					target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
				double Zv = abs(target[2] - MAINDllOBJECT->getCurrentDRO().getZ());
				double Rv = abs(target[3] - MAINDllOBJECT->getCurrentDRO().getR());
				bool flag = PPCALCOBJECT->CheckWithinCurrentView();
				if(Zv < 0.03 && Rv < 0.003 && flag)
					withinView = true;
				if(withinView)
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				else
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_ACTION);
			}
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::COPYSHAPEACTION)
		{
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
			PPCALCOBJECT->CopyShapes_DuringProgram();
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ROTATIONACTION)
		{
			PPCALCOBJECT->RotateShapes_DuringProgram();
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::REFLECTIONACTION)
		{
			PPCALCOBJECT->ReflectShapes_DuringProgram();
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
		{
			PPCALCOBJECT->drawRunpp();
			RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetAddPointInfo();
			Vector GotoPos = *((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetCurrentPosition();
			double target[4] = {GotoPos.getX(), GotoPos.getY(), GotoPos.getZ(), RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::CIRCULARINTERPOLATION_CURRENTPOS);
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
		{
			PPCALCOBJECT->drawRunpp();
			RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag = ((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetAddPointInfo();
			Vector GotoPos = *((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetTargetPosition();
			double target[4] = {GotoPos.getX(), GotoPos.getY(), GotoPos.getZ(), RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::LINEARINTERPOLATION);
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
		{
			PPCALCOBJECT->drawRunpp();
			//if (SendCurrentStepLightCommand(((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetLight_Property()) == 2)
			//{
			//	if (MAINDllOBJECT->CameraConnected())
			//		MAINDllOBJECT->SetCamProperty(((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetCam_Property());
			//}
			FramegrabBase *fg = PPCALCOBJECT->getFrameGrab();
			if (SendCurrentStepLightCommand(fg->getLightIntensity()) == 2)
				RWrapper::RW_CNC::MYINSTANCE()->ConvertLightValueToInt(gcnew System::String(fg->getLightIntensity()));

			std::string currCamSetting(fg->getCamProperties());
			if (MAINDllOBJECT->PrevCameraSetting != currCamSetting)
				MAINDllOBJECT->SetCamProperty(fg->getCamProperties());
			MAINDllOBJECT->PrevCameraSetting = currCamSetting;

			Vector GotoPos = *((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetCurrentPosition();
			double target[4] = {GotoPos.getX(), GotoPos.getY(), GotoPos.getZ(), RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_ACTION);
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::TRANSLATIONACTION)
		{
			PPCALCOBJECT->TranslateShapes_DuringProgram();
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION || PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION 
			|| PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
		{
			PPCALCOBJECT->drawRunpp();
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if(fg != NULL)
			{
				if(fg->CurrentWindowNo == 1 && !fg->DroMovementFlag)
				{
					PPCALCOBJECT->RunCurrentStep();
					RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				}
				else
					PPCALCOBJECT->RunCurrentStep();
			}
			else
				PPCALCOBJECT->RunCurrentStep();
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::AUTOALIGNACTION || PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::DEVIATIONACTION|| PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION
			|| PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::MOVESHAPEACTION || PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION
			|| PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION || PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION || PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION
			|| PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION)
		{
			PPCALCOBJECT->drawRunpp();
			PPCALCOBJECT->RunCurrentStep();
		}		
		else 
		{
			PPCALCOBJECT->drawRunpp();
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_ACTION);
			RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0005", ex); }
}

void RWrapper::RW_PartProgram::CalculateAndMovetoPosition(int SpecialStep)
{
	FramegrabBase* fg = ffg;
	bool withinView = false;
	SameCommand = 0;
	TargetReachedCallback CurrentTarget;
	double target[4] = { fg->PointDRO.getX(), fg->PointDRO.getY(), fg->PointDRO.getZ(), fg->PointDRO.getR() };
	double feedrate[4] = { RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3] };
	switch (SpecialStep)
	{
	case 0:
		if (fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH || fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN ||
			fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN || fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX ||
			fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
		{
			if (fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
				target[(int)fg->myPosition[4].getZ()] = target[(int)fg->myPosition[4].getZ()] - fg->myPosition[4].getX();
			else
			{
				FOCUSCALCOBJECT->FocusStartPosition.set(fg->PointDRO);
				target[(int)fg->myPosition[2].getZ()] = target[(int)fg->myPosition[2].getZ()] - fg->myPosition[2].getX();
			}
			CurrentTarget = TargetReachedCallback::FOCUSDEPTH_SPAN_DURINGPROGRAM;
		}
		else if (fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO)
		{
			CurrentTarget = TargetReachedCallback::FOCUSDEPTH_DRO_DURINGPROGRAM;
		}
		else if (fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
		{
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 0)
			{
				double Pstopvalue = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
				if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XRIGHT2LEFT)
					target[0] += Pstopvalue;
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XLEFT2RIGHT)
					target[0] -= Pstopvalue;
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YTOP2BOTTOM)
					target[1] += Pstopvalue;
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YBOTTOM2TOP)
					target[1] -= Pstopvalue;
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZTOP2BOTTOM)
					target[2] += Pstopvalue;
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZBOTTOM2TOP)
					target[2] -= Pstopvalue;
				CurrentTarget = TargetReachedCallback::PARTPROGRAM_PROBEHIT;
			}
			else if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
			{
				int ProbeDirection = 1;
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
				double MoveFwrd = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
				if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XRIGHT2LEFT)
				{
					ProbeDirection = 3; // 4bits of data first two is axis number and next bit rpresent +(0) and final bit for probe point always 1.(0011)
					target[0] -= MoveFwrd;
				}
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XLEFT2RIGHT)
					target[0] += MoveFwrd;
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YTOP2BOTTOM)
				{
					ProbeDirection = 7; // 0111
					target[1] -= MoveFwrd;
				}
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YBOTTOM2TOP)
				{
					ProbeDirection = 5; // 0101
					target[1] += MoveFwrd;
				}
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZTOP2BOTTOM)
				{
					ProbeDirection = 11; // 1011
					target[2] -= MoveFwrd;
				}
				else if (fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZBOTTOM2TOP)
				{
					ProbeDirection = 9; // 1001
					target[2] += MoveFwrd;
				}
				CurrentTarget = TargetReachedCallback::PARTPROGRAM_NOTHING;
				DROInstance->SetProbeDirection(ProbeDirection);
			}
			else
			{
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
				CurrentTarget = TargetReachedCallback::PARTPROGRAM_NOTHING;
				DROInstance->SetProbeDirection(fg->ProbeDirection);
			}
		}
		else if (fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATIONSCAN_FIXEDRECTANGLE || fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ANGULARRECTANGLEFRAMEGRAB ||
			fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ARCFRAMEGRAB || fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_CIRCLEFRAMEGRAB ||
			fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_RECTANGLEFRAMEGRAB)
		{
			//System::Windows::Forms::MessageBox::Show(RWrapper::RW_CNC::MYINSTANCE()->RotaryScanSpan.ToString(), "Rapid-I");
			target[3] -= RWrapper::RW_CNC::MYINSTANCE()->RotaryScanSpan;
			CurrentTarget = TargetReachedCallback::PARTPROGRAM_ACTION;
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTarget);
			break;
		}
		else
			CurrentTarget = TargetReachedCallback::PARTPROGRAM_ACTION;
		if (fg->CurrentWindowNo == 1 && !fg->DroMovementFlag)
		{
			PPCALCOBJECT->RunCurrentStep();
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
			RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
		}
		else
		{
			if (RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
				target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];

			for (int ii = 0; ii < 4; ii++)
			{
				if (abs(target[ii] - PrevTarget[ii]) < 0.001) SameCommand++;
				PrevTarget[ii] = target[ii];
			}

			if (SameCommand == 4)
			{	//Same command, we will simulate target reached!!
				if (PPCALCOBJECT->IsPartProgramRunning())
				{
					PPCALCOBJECT->RefreshImageForEdgeDetection = false;
					//PPCALCOBJECT->drawRunpp();
					PPCALCOBJECT->RunCurrentStep();
					//RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
					//PPCALCOBJECT->partProgramReached();
					return;
				}
			}

			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTarget);
		}
		break;

	case 1:
		CurrentTarget = TargetReachedCallback::ROTARY_AXIS_SCAN;
		fg = PPCALCOBJECT->getFrameGrab();
		//PPCALCOBJECT->drawRunpp();
		target[0] = fg->PointDRO.getX(); target[1] = fg->PointDRO.getY(); target[2] = fg->PointDRO.getZ(); target[3] = fg->PointDRO.getR();
		//System::Windows::Forms::MessageBox::Show(target[0].ToString() + ", " + target[1].ToString() + ", " + target[2].ToString() + ", " + target[3].ToString() + RWrapper::RW_CNC::MYINSTANCE()->RotaryScanSpan.ToString() + ", " + RWrapper::RW_CNC::MYINSTANCE()->RotaryScanSpeed.ToString(), "Rapid-I");
		PPCALCOBJECT->RefreshImageForEdgeDetection = true;
		target[3] += RWrapper::RW_CNC::MYINSTANCE()->RotaryScanSpan;
		feedrate[3] = RWrapper::RW_CNC::MYINSTANCE()->RotaryScanSpeed;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTarget);
		break;

	default:
		break;
	}


}

int RWrapper::RW_PartProgram::SendCurrentStepLightCommand(char* LightProperty)
{
	try
	{
		int Noofcards = RW_DBSettings::MYINSTANCE()->NoOfCards;
		std::string CurrentLightProperty = (const char*)LightProperty;
		//if we have the same lighting command as before, lets not waste time sending the same command again!
		if (MAINDllOBJECT->PrevLightCommand != "" && CurrentLightProperty == MAINDllOBJECT->PrevLightCommand)
			 return 4;

		MAINDllOBJECT->PrevLightCommand = CurrentLightProperty;
		if(CurrentLightProperty == "") return -1;
		if(CurrentLightProperty.length() < 2 * Noofcards)
		{
			for(int i = CurrentLightProperty.length(); i <= 2 * Noofcards; i++)
				CurrentLightProperty = "0" + CurrentLightProperty;
		}
		else if(CurrentLightProperty.length() > 2 * Noofcards)
			CurrentLightProperty = CurrentLightProperty.substr((2 * Noofcards) - CurrentLightProperty.length(), 2 * Noofcards);

		int L_Val = 0; char u_Val[10];
		std::reverse(CurrentLightProperty.begin(), CurrentLightProperty.end());
		std::string OutputLight;
		for (int ii = 0; ii < CurrentLightProperty.length(); ii += 2)
		{
			std::string ss = CurrentLightProperty.substr(ii, 2);
			L_Val = std::stoi(ss, nullptr, 16);
			if (ii / 2 == RW_DBSettings::MYINSTANCE()->Profile_Zone_No || ii / 2 == RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No)
			{
				if (L_Val != 0) L_Val += PL_Offset;
			}
			else
			{
				if (L_Val != 0) L_Val += SF_Offset;
			}
			if (L_Val < 0) L_Val = 0;
			if (L_Val > 63) L_Val = 63;
			_ltoa(L_Val, u_Val, 16);
			ss = u_Val;
			for (int j = ss.length(); j < 2; j++)
				ss = "0" + ss;
			OutputLight = OutputLight + ss;
		}
		std::reverse(OutputLight.begin(), OutputLight.end());
		DROInstance->sendLightCommand((char*)OutputLight.c_str(), true);
		return 2;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0003", ex);
		return 0;
	}
}

void RWrapper::RW_PartProgram::TakeProbePoint()
{
	try
	{
		FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
		double target[4] = {fg->PointDRO.getX(),fg->PointDRO.getY(),fg->PointDRO.getZ(), RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DBSettings::MYINSTANCE()->ProbeSpeed, RWrapper::RW_DBSettings::MYINSTANCE()->ProbeSpeed, RWrapper::RW_DBSettings::MYINSTANCE()->ProbeSpeed, RWrapper::RW_DBSettings::MYINSTANCE()->ProbeSpeed};
		double MoveFwrd = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue + 1;
		if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XRIGHT2LEFT)
			target[0] -= MoveFwrd;
		else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XLEFT2RIGHT)
			target[0] += MoveFwrd;
		else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YTOP2BOTTOM)
			target[1] -= MoveFwrd;
		else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YBOTTOM2TOP)
			target[1] += MoveFwrd;
		else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZTOP2BOTTOM)
			target[2] -= MoveFwrd;
		else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZBOTTOM2TOP)
			target[2] += MoveFwrd;
		RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_NOTHING);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0002", ex); }
}

void RWrapper::RW_PartProgram::PartProgramFinished()
{
	try
	{
		bool Checkflag = true;
		this->CorrectRotationalError = false;
		MAINDllOBJECT->Derivedmodeflag = false;
		PPCALCOBJECT->DrawPartprogramArrow(false);
		PPCALCOBJECT->IsPartProgramRunning(false);
		PPCALCOBJECT->ProgramRunningForFirstTime(false);
		GenerateProgramReport::raise();
		////System::Windows::Forms::MessageBox::Show("Finished Excel Report Generation", "Rapid-I");

		if(MAINDllOBJECT->HideShapeMeasurementDuringPP)
		{
			MAINDllOBJECT->UpdateShapesChanged();
			MAINDllOBJECT->Measurement_updated();
		}
		MAINDllOBJECT->SetStatusCode("RW_PartProgram01");
		//if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		//	return;

		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM) 
		{
			if (RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag && !RunningFailedAction)
			{
				int TotalRunCount = RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalRows;
				MeasurementCount++;
				ColumnCount++;
				int CurrentRunCount = 0;
				if (ColumnCount >= RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns)
				{
					ColumnCount = 0;
					RowCount++;
				}
				if (RowCount == RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalRows)
				{
					MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramFinished01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_FINISHED);
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
					//ResetGridProgramCount();
					return;
				}
				CurrentRunCount = RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + ColumnCount + 1;
				GridRunCountEvent::raise(CurrentRunCount + " / " + TotalRunCount);
				RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_RERUN);
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			}
			else
			{
				RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_FINISHED);
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			}
			//RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_FINISHED);
			//RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			if (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramFinished02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		}
		else
		{
			if(PPCALCOBJECT->ExportPointsToCsvInPP())
				DXFEXPOSEOBJECT->WriteToPointCsvFile((char*)(void*)Marshal::StringToHGlobalAnsi(LoadedFilePath).ToPointer());
			if(PPCALCOBJECT->ExportParamToCsvInPP())
				DXFEXPOSEOBJECT->WriteToShapesParamCsvFile((char*)(void*)Marshal::StringToHGlobalAnsi(LoadedFilePath).ToPointer());
	
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			if(RWrapper::RW_DBSettings::MYINSTANCE()->GridProgramModeFlag && !RunningFailedAction)
			{
				int TotalRunCount = RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalRows;				
				Sleep(100);
				MeasurementCount++;
				ColumnCount++;
				int CurrentRunCount;
				if(ColumnCount >= RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns)
				{
					ColumnCount = 0;
					RowCount++;					
				}				
				if(RowCount == RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalRows)
				{
					MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramFinished01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					GridRunCountEvent::raise("Done");
					MeasurementCount = 0;
					RowCount = 0;
					ColumnCount = 0;
					CurrentCount = 0;
					if(RWrapper::RW_PartProgram::MYINSTANCE()->ReturnTohomePosition && RW_CNC::MYINSTANCE()->getCNCMode())
					{
						double target[4] = {RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0], RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1], 
											RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[2], RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[3]};
						if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
							  target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
						RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_FINISHED);
					}
					else
					{
						RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_FINISHED);
						RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
					}
					//ResetGridProgramCount();
					if(RWrapper::RW_PartProgram::MYINSTANCE()->ReferenceModeF)
					{
						PPCALCOBJECT->ReferencePtAsHomePos(true);
						RWrapper::RW_PartProgram::MYINSTANCE()->ReferenceModeF = false;
					}
					return;
				}
				CurrentRunCount = RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + ColumnCount + 1;
				GridRunCountEvent::raise(CurrentRunCount + " / " + TotalRunCount);
				//Calculate the next cell start position/ home position..//
				if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				{
					double target[4] = {0};
					if(RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList->Count == 3 * (RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalRows - 1) && RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList->Count > 0)
					{
						double Correction[2] = {(ColumnCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellWidth) + RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 3], 
												 RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 2] - (RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellHeight)};
						if(SaveReferencePtMode && FixtureReferenceAngle != 0)
						{
							double Transform_Matrix[9] = {0};
							Transform_Matrix[0] = cos(FixtureReferenceAngle); Transform_Matrix[1] = -sin(FixtureReferenceAngle); 
							Transform_Matrix[3] = sin(FixtureReferenceAngle); Transform_Matrix[4] = cos(FixtureReferenceAngle);
							Transform_Matrix[8] = 1;							
							Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, Correction[0], Correction[1]);	
							target[0] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0] + TmpValue.getX();
							target[1] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1] + TmpValue.getY();
						}
						else
						{
							double Tmp1 = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0], Tmp2 = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1];
							double Tmp4 = (ColumnCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellWidth), Tmp3 = (RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellHeight);
							target[0] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0] + (ColumnCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellWidth) + RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 3];
							target[1] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1] - (RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellHeight) + RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList[3 * RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + 3 * ColumnCount - 2];
						}
					}
					else
					{
						target[0] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0] + (ColumnCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellWidth);
						target[1] = RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1] - (RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellHeight);
											
					}
					target[2] =	RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[2], RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[3];
					if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
						 target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
					
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_RERUN);
				}
				else
				{
					RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_RERUN);
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				}
			}
			else
			{
				if(RunningFailedAction)
					MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramFinished02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				else
					MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramFinished02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				if(RWrapper::RW_PartProgram::MYINSTANCE()->ReferenceModeF)
				{
					PPCALCOBJECT->ReferencePtAsHomePos(true);
					RWrapper::RW_PartProgram::MYINSTANCE()->ReferenceModeF = false;
				}
				if(RWrapper::RW_PartProgram::MYINSTANCE()->ReturnTohomePosition && RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				{
					double target[4] = {RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[0], RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[1], 
											RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[2], RWrapper::RW_PartProgram::MYINSTANCE()->HomePosition[3]};
					if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
						target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_FINISHED);
				}
				else
				{
					RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_FINISHED);
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				}
			}
		}
		PPCALCOBJECT->StartingPointofPartprogram(true);
		Checkflag = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0007", ex); }
	finally
	{ 
	if (RWrapper::RW_DBSettings::MYINSTANCE()->Gridrerun)
		{
		ResetGridProgramCount();
		}
	}
}

void RWrapper::RW_PartProgram::ResetGridProgramCount()
{
	try
	{
		this->CorrectRotationalError = false;
		this->ColumnCount = 0;
		this->RowCount = 0;
		this->MeasurementCount = 0;
		//this->CorrectionValue->Clear();
		this->FixtureReferenceAngle = 0;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_PartProgram::Rerun_Program()
{
	/*int CurrentCount = RowCount * RWrapper::RW_FixtureCalibration::MYINSTANCE()->TotalColumns + ColumnCount + 1;
	wofstream ofile;
	ofile.open("D:SphereCalibration.txt", ios::app);
	ofile << CurrentCount << endl;
	ofile.close();*/
	Sleep(1000);
	RuntheProgramAgain::raise();
}

void RWrapper::RW_PartProgram::Abort_PartProgram()
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Stop();
		RunIsnotValid::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0012", ex); }
}

System::String^ RWrapper::RW_PartProgram::GetFeatureLIst()
{
	try
	{
		return gcnew System::String(PPCALCOBJECT->getFeatureList().c_str());
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0012", ex); }
}

void RWrapper::RW_PartProgram::ContinuePartProgram_AfterUserInteraction()
{
	try
	{
		PPCALCOBJECT->partProgramReached();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0013", ex); }
}

//void RWrapper::RW_PartProgram::ReadCorrectionValueForGrid()
//{
//	try
//	{
//		System::String^ fixtureName = RWrapper::RW_DBSettings::MYINSTANCE()->CurrentFixureName;
//		System::IO::FileInfo^ CalibrationFileInfo = gcnew System::IO::FileInfo(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath +"\\Database\\" + fixtureName);
//		if(CalibrationFileInfo->Exists)
//		{
//			RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList->Clear();
//			//this->CorrectionValue->Clear();
//			const char* tempchar = (const char*)(Marshal::StringToHGlobalAnsi(fixtureName)).ToPointer();
//			const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
//			std::string temporaryFixtureName = temporarychar;
//			std::string filename = temporaryFixtureName + "\\Database\\" + tempchar;	
//			std::wstring buffer;
//			std::wifstream ifile;
//			ifile.open(filename.c_str());
//			ifile.precision(15);
//			ifile >> buffer;
//			if(buffer == L"CalibrationValue")
//			{
//				while(!ifile.eof())
//				{
//					try
//					{
//						ifile >> buffer;
//						if(buffer == L"EndCalibration")
//							break;
//						else if(buffer == L"PointOfRotation")
//						{
//							this->PointOfRotationFlag = true;
//							ifile >> buffer;
//							const wchar_t *str = buffer.c_str();
//							double Cvalue = _wtof(str);		
//							this->PointOfRotation[0] = Cvalue;
//							ifile >> buffer;
//							str = buffer.c_str();
//							Cvalue = _wtof(str);	
//							this->PointOfRotation[1] = Cvalue;
//						}
//						else
//						{
//							const wchar_t *str = buffer.c_str();
//							double Cvalue = _wtof(str);					
//							RWrapper::RW_FixtureCalibration::MYINSTANCE()->CorrectionList->Add(Cvalue);
//							//CorrectionValue->Add(Cvalue);
//						}
//					}
//					catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0010", ex); }
//				}
//				ifile.close();
//				//this->CorrectRotationalError = true;
//			}
//		}
//	}
//	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0011", ex); }
//}

void RWrapper::RW_PartProgram::RaisePP_ParamResetEvent(RWrapper::RW_Enum::PROGRAM_PARAMETER type)
{
	try
	{
		PP_ParamResetEvent::raise(type);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025", ex); }
}

void RWrapper::RW_PartProgram::SetToleranceValueForDeviation(double Uppertolerance, double Lowertolerance)
{
	try
	{
		PPCALCOBJECT->SetDeviationTolerance(Uppertolerance, Lowertolerance);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0026", ex); }
}

void RWrapper::RW_PartProgram::SetOffsetForRelativePoint(double offset_X, double offset_Y, double offset_Z)
 {
	try
	{
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->Normalshape())
				{
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::RELATIVEPOINT)
					{
						((RPoint*)Cshape)->SetOffsets(offset_X, offset_Y, offset_Z);
						break;
					}
				}
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0027", ex); }
}

void RWrapper::RW_PartProgram::RunFailedAction()
{
	try
	{
		bool FoundAction = false;
		while(PPCALCOBJECT->PPrerunActionList != PPCALCOBJECT->getPPActionlist().getList().end())
		{
			if(PPCALCOBJECT->PPrerunActionList->second == NULL)
			{
			   RunningFailedAction = false;
			   PPCALCOBJECT->getPPActionlist().clearSelection();
			   PPCALCOBJECT->PPDontUpdateSnapPts(false);
			   PPCALCOBJECT->UpdateallSnapPtsForShapes();
			   PPCALCOBJECT->ClearAfterProgramFinish();
			   PartProgramFinished();
			   break;
			}
			RAction* Caction = (RAction*)(*PPCALCOBJECT->PPrerunActionList).second;
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				PPCALCOBJECT->SetCurrentAction(Caction);
				HandlePartprogram();
				break;
			}
			for each(int id in FailedActionIdList)
			{
			  if((id == Caction->getId()))
			  {
				 PPCALCOBJECT->SetCurrentAction(Caction);
				 PPCALCOBJECT->IsPartProgramRunning(true);
				 HandlePartprogram();
				 FoundAction = true;
				 break;
			  }
			}
			if(FoundAction) break;
			PPCALCOBJECT->PPrerunActionList++;
		}

		if(FoundAction)
		{
			if(PPCALCOBJECT->PPrerunActionList == PPCALCOBJECT->getPPActionlist().getList().end())
			{
				RunningFailedAction = false;
				PPCALCOBJECT->getPPActionlist().clearSelection();
				PPCALCOBJECT->PPDontUpdateSnapPts(false);
				PPCALCOBJECT->UpdateallSnapPtsForShapes();
				PPCALCOBJECT->ClearAfterProgramFinish();
				PartProgramFinished();
			}
		}
		
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0031", ex);
	}
}

void RWrapper::RW_PartProgram::GetActionListOfOutTolMeasure()
{
	try
	{
		FailedActionIdList->Clear();
		std::list<int> tempList;
		PPCALCOBJECT->GetActionListInFailedMeasure(&tempList);
		for(list<int>::iterator It = tempList.begin(); It != tempList.end(); It++)
			FailedActionIdList->Add(*It);
		RunningFailedAction = true;
		PPCALCOBJECT->PPrerunActionList = PPCALCOBJECT->getPPActionlist().getList().begin();
		RunFailedAction();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0031", ex);
	}
}

void RWrapper::RW_PartProgram::SetPointsDirectoryPath(System::String^ PointsDirectPath)
{
	DXFEXPOSEOBJECT->PointsDirectoryPath = (char*)(void*)Marshal::StringToHGlobalAnsi(PointsDirectPath).ToPointer();
	DXFEXPOSEOBJECT->PointsDirectoryPath += "\\";
}

 bool RWrapper::RW_PartProgram::SendCommandInBatch()
 {
	 try
	 {
		 BatchOfCommandCompleted = true; 
		 std::map<int, DigitalReadOut::BatchCommandTarget> BatchTargetPos; //= new std::map<int, DigitalReadOut::BatchCommandTarget>;
		 double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		 double target[4] = {0};
		 this->NumberOfCommandSend = 0;
		 this->InterPolationCommandCount = 0;
		 bool firstCommand = true, firstTime = true;
		 Shape *Cshape1;
		 Cshape1 = NULL;
		 TargetReachedCallback CurrentTarget;
		 for(int i = 1; i < 20; i++)
		 {
			 DigitalReadOut::BatchCommandTarget TargetPos;
			 int Probe_direction = 0;
			  //action incremented ...
			 if(firstTime)
			 {
				 PPCALCOBJECT->IncrementToCurrentAction();
				 firstTime = false;
			 }
			 else
				PPCALCOBJECT->IncrementActionForBatchOfCommand();
			 if(PPCALCOBJECT->getCurrentAction(true) == NULL)
			 {
				 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
				 break;
			 }
			 if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode() && 
				 (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
					 MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
					 MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM))
			 {
				 if(PPCALCOBJECT->getCActionType(true) == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				 {
					 if( ((AddPointAction*)PPCALCOBJECT->getCurrentAction(true))->getShape()->ShapeUsedForTwoStepHoming() || 
						 ((AddPointAction*)PPCALCOBJECT->getCurrentAction(true))->getShape()->ShapeAsReferenceShape() )
					 {
						 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
						 break;
					 }
					 FramegrabBase* fg = PPCALCOBJECT->getFrameGrab(true);
					 for each(int ActionId in PPCALCOBJECT->LastActionIdList_OffsetAction)
					 {
						 if(PPCALCOBJECT->getCurrentAction(true)->getId() == ActionId)
						 {
							 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
							 break;
						 }
					 }
					 if(((AddPointAction*)PPCALCOBJECT->getCurrentAction(true))->PausableAction)
					 {
						 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
						 break;
					 }
					 if(((AddPointAction*)PPCALCOBJECT->getCurrentAction(true))->CriticalAction && !fg->PartprogrmActionPassStatus())
					 {
						 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
						 break;
					 }
					 if(Cshape1 == NULL)
					 {
						 Cshape1 = ((AddPointAction*)PPCALCOBJECT->getCurrentAction(true))->getShape();
						 if(Cshape1->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
							 PPCALCOBJECT->ChangeUCSAction_DuringProgram(Cshape1->UcsId());
						 if(!Cshape1->selected())
							 MAINDllOBJECT->selectShape(Cshape1->getId(), false);
					 }
					 else
					 {
						 if(Cshape1->getId() != ((AddPointAction*)PPCALCOBJECT->getCurrentAction(true))->getShape()->getId())
						 {
							 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
							 break;
						 }
					 }
					 target[0] = fg->PointDRO.getX(); target[1] = fg->PointDRO.getY(); target[2] = fg->PointDRO.getZ(); target[3] = fg->PointDRO.getR();
					 if(fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
					 {
						 if(firstCommand)
						 {
							 firstCommand = false;
							 CurrentTarget = TargetReachedCallback::PARTPROGRAM_NOTHING;
						 }
						 double MoveFwrd = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
						 if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XRIGHT2LEFT)
						 {
							 Probe_direction = 3; // 4bits of data first two is axis number and next bit rpresent +(0) and final bit for probe point always 1.(0011)
							 target[0] -= MoveFwrd;
						 }
						 else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::XLEFT2RIGHT)
						 {
							 Probe_direction = 1;
							 target[0] += MoveFwrd;
						 }
						 else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YTOP2BOTTOM)
						 {
							 Probe_direction = 7; // 0111
							 target[1] -= MoveFwrd;
						 }
						 else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::YBOTTOM2TOP)
						 {
							 Probe_direction = 5; // 0101
							 target[1] += MoveFwrd;
						 }
						 else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZTOP2BOTTOM)
						 {
							 Probe_direction = 11; // 1011
							 target[2] -= MoveFwrd;
						 }
						 else if(fg->ProbeDirection == RapidEnums::PROBEDIRECTION::ZBOTTOM2TOP)
						 {
							 Probe_direction = 9; // 1001
							 target[2] += MoveFwrd;
						 }
						 TargetPos.Target_Feedrate[0] = target[0]; TargetPos.Target_Feedrate[1] = target[1]; TargetPos.Target_Feedrate[2] = target[2]; TargetPos.Target_Feedrate[3] = target[3]; 
						 TargetPos.Target_Feedrate[4] = feedrate[0]; TargetPos.Target_Feedrate[5] = feedrate[1]; TargetPos.Target_Feedrate[6] = feedrate[2]; TargetPos.Target_Feedrate[7] = feedrate[3]; 
						 TargetPos.LinearInterPolation = true; TargetPos.ProbeDir = Probe_direction;
					 }
					 else
					 {
						 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
						 break;
					 }
				 }
				 else if(PPCALCOBJECT->getCActionType(true) == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
				 {
					 if(((AddProbePathAction*)PPCALCOBJECT->getCurrentAction(true))->PausableAction)
					 {
						 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
						 break;
					 }
					 if(firstCommand)
					 {
						 firstCommand = false;
						 CurrentTarget = TargetReachedCallback::PARTPROGRAM_INCREMENT;
					 }
					 FramegrabBase* fg = PPCALCOBJECT->getFrameGrab(true);
					 target[0] = fg->PointDRO.getX(); target[1] = fg->PointDRO.getY(); target[2] = fg->PointDRO.getZ(); target[3] = fg->PointDRO.getR();
					 if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
						 target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];	
					 if(!MAINDllOBJECT->OneTouchProbeAlignment && NumberOfCommandSend == 0)
						 PPCALCOBJECT->DoHoming();
					 TargetPos.Target_Feedrate[0] = target[0]; TargetPos.Target_Feedrate[1] = target[1]; TargetPos.Target_Feedrate[2] = target[2]; TargetPos.Target_Feedrate[3] = target[3]; 
					 TargetPos.Target_Feedrate[4] = feedrate[0]; TargetPos.Target_Feedrate[5] = feedrate[1]; TargetPos.Target_Feedrate[6] = feedrate[2]; TargetPos.Target_Feedrate[7] = feedrate[3];
					 TargetPos.LinearInterPolation = true; TargetPos.ProbeDir = 0;
				 }
				 else if(PPCALCOBJECT->getCActionType(true) == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
				 {
					 if(((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->PausableAction)
					 {
						 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
						 break;
					 }
					 if(((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetShapeInfo())
					 {
						 if(Cshape1 == NULL)
						 {
							 Cshape1 = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->getShape();
							 if(Cshape1->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
								 PPCALCOBJECT->ChangeUCSAction_DuringProgram(Cshape1->UcsId());
							 if(!Cshape1->selected())
								 MAINDllOBJECT->selectShape(Cshape1->getId(), false);
						 }
						 else
						 {
							 if(Cshape1->getId() != ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->getShape()->getId())
							 {
								 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
								 break;
							 }
						 }
					 }
					 if(firstCommand)
					 {
						 firstCommand = false;
						 CurrentTarget = TargetReachedCallback::CIRCULARINTERPOLATION;
						 PPCALCOBJECT->drawRunpp();
						 RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetAddPointInfo();
					 }
					 Vector Center = *((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetCenter();
					 Vector NormalVctor = *((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetNormalVector();
					 int hops = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetHopsCount();
					 double SweeepAngle = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetSweepAngle();
					 double Radius =  ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetRadius();
					 TargetPos.Target_Feedrate[0] = Center.getX(); TargetPos.Target_Feedrate[1] = Center.getY(); TargetPos.Target_Feedrate[2] = Center.getZ(); TargetPos.Target_Feedrate[3] = NormalVctor.getX(); 
					 TargetPos.Target_Feedrate[4] = NormalVctor.getY(); TargetPos.Target_Feedrate[5] = NormalVctor.getZ(); TargetPos.Target_Feedrate[6] = Radius; TargetPos.Target_Feedrate[7] = feedrate[0];
					 TargetPos.LinearInterPolation = false; TargetPos.NumberOfHops = hops; TargetPos.SweepAngle = SweeepAngle;
					 InterPolationCommandCount++;
				 }
				 else if(PPCALCOBJECT->getCActionType(true) == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
				 {
					 if(((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->PausableAction)
					 {
						 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
						 break;
					 }
					 if(((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetShapeInfo())
					 {
						 if(Cshape1 == NULL)
						 {
							 Cshape1 = ((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->getShape();
							  if(Cshape1->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
								 PPCALCOBJECT->ChangeUCSAction_DuringProgram(Cshape1->UcsId());
							 if(!Cshape1->selected())
								 MAINDllOBJECT->selectShape(Cshape1->getId(), false);
						 }
						 else
						 {
							 if(Cshape1->getId() != ((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->getShape()->getId())
							 {
								 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
								 break;
							 }
						 }
					 }
					 if(firstCommand)
					 {
						 firstCommand = false;
						 CurrentTarget = TargetReachedCallback::LINEARINTERPOLATION;
						 PPCALCOBJECT->drawRunpp();
						 RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag = ((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetAddPointInfo();
					 }
					 Vector GotoPos = *((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction(true))->GetTargetPosition();
					 double target[4] = {GotoPos.getX(), GotoPos.getY(), GotoPos.getZ(), RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
					 double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
					 TargetPos.Target_Feedrate[0] = target[0]; TargetPos.Target_Feedrate[1] = target[1]; TargetPos.Target_Feedrate[2] = target[2]; TargetPos.Target_Feedrate[3] = target[3]; 
					 TargetPos.Target_Feedrate[4] = feedrate[0]; TargetPos.Target_Feedrate[5] = feedrate[1]; TargetPos.Target_Feedrate[6] = feedrate[2]; TargetPos.Target_Feedrate[7] = feedrate[3];
					 TargetPos.LinearInterPolation = true; TargetPos.ProbeDir = 0;
					 InterPolationCommandCount++;
				 }
				 else
				 {  
					 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
					 break;
				 }
				 int NumberOfCommandSend = 0;
				 BatchTargetPos[NumberOfCommandSend] = TargetPos;
				 NumberOfCommandSend++;
			 }
			 else
			 {
				 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
				 break;
			 }
		 }
		 if(NumberOfCommandSend > 1)
		 {
			 BatchOfCommandCompleted = false;
			 RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(CurrentTarget);
			 //send batch of command....
			 DROInstance->GotoForBatchOfCommand(BatchTargetPos, NumberOfCommandSend);	
			 return true;
		 }
		 else
		 {
			 for(int i = NumberOfCommandSend - 1; i >= 0; i--)
			 {
				 PPCALCOBJECT->IncrementActionForBatchOfCommand(false);
			 }
			 NumberOfCommandSend = 0;
			 InterPolationCommandCount = 0;
			 return false;
		 }
	 }
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0032", ex); 
		return false;
	}
}

void RWrapper::RW_PartProgram::SaveImageRectangleExtent()
{
	try
	{
		bool firstTime = true;
		double MIN_X = 0, MIN_Y = 0, MAX_X = 0, MAX_Y = 0;
		for(RC_ITER item = MAINDllOBJECT->getActionsHistoryList().getList().begin(); item != MAINDllOBJECT->getActionsHistoryList().getList().end(); item++)
		{
			RAction* MyAction = (RAction*)((*item).second);
			if(!MyAction->ActionStatus()) continue;
			if(MyAction->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fg = ((AddImageAction*)MyAction)->getFramegrab();
				Vector temp = fg->PointDRO;
				if(firstTime)
				{
					firstTime = false;
					MIN_X = temp.getX(); MAX_X = temp.getX();
					MIN_Y = temp.getY(); MAX_Y = temp.getY();
				}
				else
				{
					if(temp.getX() < MIN_X)
						MIN_X = temp.getX();
					if(temp.getY() < MIN_Y)
						MIN_Y = temp.getY();
					if(temp.getX() > MAX_X)
						MAX_X = temp.getX();
					if(temp.getY() > MAX_Y)
						MAX_Y = temp.getY();
				}
			}
		}
		std::map<int, double*> ImageExtent;
		double DivideFactor_X = MAX_X - MIN_X;
		double DivideFactor_Y = MAX_Y - MIN_Y;
		for(RC_ITER item = MAINDllOBJECT->getActionsHistoryList().getList().begin(); item != MAINDllOBJECT->getActionsHistoryList().getList().end(); item++)
		{
			RAction* MyAction = (RAction*)((*item).second);
			if(!MyAction->ActionStatus()) continue;
			if(MyAction->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fg = ((AddImageAction*)MyAction)->getFramegrab();
				Vector temp = fg->PointDRO;
				double* TmpValue = new double[2];
				TmpValue[0] = 0.5; TmpValue[1] = 0.5;
				if(DivideFactor_X != 0)
				{
					TmpValue[0] = (temp.getX() - MIN_X) / DivideFactor_X;
				}
				if(DivideFactor_Y != 0)
				{
					TmpValue[1] = (temp.getY() - MIN_Y) / DivideFactor_Y;
				}
				ImageExtent[((AddImageAction*)MyAction)->ImageCount] = TmpValue;
			}
		 }
		const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
		std::string filePath = temporarychar;
		filePath += "//Image//Image.txt";
		std::wofstream myfile;
		myfile.open(filePath.c_str());
		myfile.precision(15);
		map<int, double*>::iterator itr;
		int NoOfImages = ImageExtent.size();
		while(ImageExtent.size() != 0)
		{
			itr = ImageExtent.begin();
			double* TmpPtr = (*itr).second;
			int Im_Count = (*itr).first;
			System::String^ Str = System::Convert::ToString(Im_Count) + ":" + System::Convert::ToString(TmpPtr[0]) + ":" + System::Convert::ToString(TmpPtr[1]);
			const char* tempchar = (const char*)(Marshal::StringToHGlobalAnsi(Str).ToPointer());
			myfile << tempchar << endl;
			ImageExtent.erase(Im_Count);
			delete [] TmpPtr;
		}
		myfile.close();
		MoveImageEvent::raise(NoOfImages);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0033", ex); 
	}
}

bool RWrapper::RW_PartProgram::GetDelphiPPSuccessStatus()
{
	try
	{
		return PPCALCOBJECT->CalculateDelphiPPSuccessStatus();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0033", ex); 
		return false;
	}
}


bool RWrapper::RW_PartProgram::LoadReferecneImage_OneShot(System::String^ FileName)
{
	try
	{
		cli::array<BYTE> ^iBytes;
		bool flag = false;
		if (FileName->EndsWith("bmp") || FileName->EndsWith("jpg"))
		{
			System::Drawing::Bitmap^ bm = gcnew System::Drawing::Bitmap(FileName);
			System::Drawing::Imaging::BitmapData^ bmd;
			iBytes = gcnew cli::array<BYTE>(bm->Width * bm->Height * 3);
			System::Drawing::Rectangle rrect(0, 0, bm->Width, bm->Height);
			bmd = bm->LockBits(rrect, Imaging::ImageLockMode::ReadWrite, Imaging::PixelFormat::Format24bppRgb);
			System::Runtime::InteropServices::Marshal::Copy(bmd->Scan0, iBytes, 0, iBytes->Length);
			bm->UnlockBits(bmd);
			for (int i = 0; i < iBytes->Length; i++)
				RW_MainInterface::MYINSTANCE()->image_pixels[i] = iBytes[i];
			flag = PPCALCOBJECT->SetRefImage_OneShot(RW_MainInterface::MYINSTANCE()->image_pixels, iBytes->Length, 3);//&pixels[0]
		}
		else if (FileName->EndsWith("bin"))
		{
			//iBytes = gcnew cli::array<BYTE>(MAINDllOBJECT->currCamWidth * MAINDllOBJECT->currCamHeight);  
			//System::IO::StreamReader^ oneS_RefFile = gcnew System::IO::StreamReader(FileName);
			//cli::array<String^>^ ByteValues = oneS_RefFile->ReadToEnd()->Split(',');

			System::IO::FileStream^ fs = gcnew System::IO::FileStream(FileName, System::IO::FileMode::Open);
			//System::IO::BinaryReader^ br = gcnew System::IO::BinaryReader(fs);
			System::IO::StreamReader^ br = gcnew System::IO::StreamReader(fs);


			//cli::array<unsigned char>^ ByteBlock = br->ReadBytes(MAINDllOBJECT->currCamWidth * MAINDllOBJECT->currCamHeight * HELPEROBJECT->CamSizeRatio * HELPEROBJECT->CamSizeRatio);
			cli::array<wchar_t>^ ByteBlock = gcnew cli::array<wchar_t>(MAINDllOBJECT->currCamWidth * MAINDllOBJECT->currCamHeight * HELPEROBJECT->CamSizeRatio * HELPEROBJECT->CamSizeRatio);
			int ff = br->ReadBlock(ByteBlock, 0, MAINDllOBJECT->currCamWidth * MAINDllOBJECT->currCamHeight * HELPEROBJECT->CamSizeRatio * HELPEROBJECT->CamSizeRatio);
			//System::Runtime::InteropServices::Marshal::Copy(ByteBlock, 0, IntPtr((void *)RWrapper::pixels[0]), ByteBlock->Length);
			int j = 0;
			for (int i = 0; i < ByteBlock->Length; i++)
			{
				RW_MainInterface::MYINSTANCE()->image_pixels[j++] = ByteBlock[i];
				//pixels[j++] = ByteBlock[i];
				//pixels[j++] = ByteBlock[i];
			}

			//RWrapper::RW_MainInterface::MYINSTANCE()->RGBArray2Image(pixels, MAINDllOBJECT->currCamWidth, MAINDllOBJECT->currCamHeight, "E:\\REfImage_1.bmp", false, false);
			flag = PPCALCOBJECT->SetRefImage_OneShot(RW_MainInterface::MYINSTANCE()->image_pixels, ByteBlock->Length, 1);//&pixels[0]
			fs->Close();
		}
		this->OneShot_RefImage_Loaded = flag;
		return flag;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0029b", ex); return false;
	}
}

bool RWrapper::RW_PartProgram::SetLightChangeValue(bool ForSurface, int changeValue)
{
	try
	{
		if (ForSurface)
			this->SF_Offset = changeValue;
		else
			this->PL_Offset = changeValue;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0030", ex); return false;
	}
}
