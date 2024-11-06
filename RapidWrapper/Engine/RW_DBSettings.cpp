#include "stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\DXF\DXFExpose.h"
#include "..\MAINDLL\Engine\FocusFunctions.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "..\MAINDLL\Engine\PartProgramFunctions.h"
#include "RW_DelphiCamModule.h"
#include "RW_SphereCalibration.h"

//#using "E:\\CTPL\\Sources\\Build3600\\Builds\\x64\\Debug\\CalibrationModule.dll"
//#using "D:\\Rapid-i\\git\\rapid-i\\Builds\\x64\\Debug\\CalibrationModule.dll"
//#using "E:\\SourceLatest\\SourceLatest\\Builds\\x64\\Debug\\CalibrationModule.dll"

RWrapper::RW_DBSettings::RW_DBSettings()
{
	try
	{
		this->rcadSettings = this;
		this->RotartAxisPresent = false;
		this->_Buttontype = gcnew System::Collections::Generic::List<System::Int32>;
		this->_ButtonToolTip = gcnew System::Collections::Generic::List<System::String^>;
		this->_ButtonBaseName = gcnew System::Collections::Generic::List<System::String^>;
		this->_ButtonImage = gcnew System::Collections::Generic::List<System::Windows::Controls::Image^>;
		this->_ButtonCheckedProperty = gcnew System::Collections::Generic::List<System::Boolean>;
		this->ButtonToolTip = "";
		this->LangaugeType = "English";
		this->ThreeDSoftwareMode = false;
		this->AutoZoomInOutMode = false;
		this->GridProgramModeFlag = false;
		this->Gridrerun = false;
		this->PartialmageModeForOneShot = false;
		this->CurrentMachineNumber = "";
		this->LangCid = 9;
		InitializeRcadSettings();
		this->MyTimer = gcnew System::Windows::Forms::Timer();
		this->MyTimer->Enabled = false;
		this->MyTimer->Interval = 2500;
		this->MyTimer->Tick += gcnew EventHandler(this, &RWrapper::RW_DBSettings::RestoreTheCrash);
		this->R_Last_Value = 0;
		this->CheckForTargetReach = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0001", ex); }
}

RWrapper::RW_DBSettings::~RW_DBSettings()
{
}

RWrapper::RW_DBSettings^ RWrapper::RW_DBSettings::MYINSTANCE()
{
	 return rcadSettings;
}

//Initialize all the arraylist.. and database...//
void RWrapper::RW_DBSettings::InitializeRcadSettings()
{
	try
	{
		this->MagNameList = gcnew System::Collections::Generic::List<System::String^>;		
		this->R_DistList = gcnew System::Collections::Generic::List<System::Double>;	
		this->R_PixelWidth = gcnew System::Collections::Generic::List<System::Double>;
		this->R_ZoomLevel = gcnew System::Collections::Generic::List<System::Int32>;
		this->PixelWidthList_X = gcnew System::Collections::Hashtable();
		this->PixelWidthList_Y = gcnew System::Collections::Hashtable();
		this->DigitalZoomPixel_X = gcnew System::Collections::Hashtable();
		this->DigitalZoomPixel_Y = gcnew System::Collections::Hashtable();
		this->ElvnXProfileMode = gcnew System::Collections::Hashtable();
		this->Grade1LevelCollection = gcnew System::Collections::Hashtable();
		this->Grade1ValueCollection = gcnew System::Collections::Hashtable();
		this->Grade1ValueEndCollection = gcnew System::Collections::Hashtable();
		this->Grade2ValueEndCollection = gcnew System::Collections::Hashtable();
		this->Grade2ValueCollection = gcnew System::Collections::Hashtable();
		this->Grade2LevelCollection = gcnew System::Collections::Hashtable();
		this->SavedLightSettings_NameList = gcnew  System::Collections::Generic::List<System::String^>; 
		GetValuesFromDatabase(true);

		this->con = gcnew System::Data::OleDb::OleDbConnection(RWrapper::RW_MainInterface::DBConnectionString);
		this->con->Open();
		createToolButtonInterface();
		GetSavedLightSettings();
		//Get the CAD colors from the Data base and store that in framework....//
		this->cmd = gcnew System::Data::OleDb::OleDbCommand("SELECT * FROM CADColors ORDER BY CADColorNumber",this->con);
		this->reader = this->cmd->ExecuteReader();
		while(reader->Read())
		{
			DXFEXPOSEOBJECT->SetCADColors(this->reader->GetInt16(0),this->reader->GetInt32(1),this->reader->GetInt32(2),this->reader->GetInt32(3));
		}
		delete cmd;
		/*this->cmd = gcnew System::Data::OleDb::OleDbCommand("SELECT * FROM GradeChart ORDER BY ID",this->con);
		this->reader = this->cmd->ExecuteReader();
		int Id = 1;
		while(reader->Read())
		{
			this->Grade1ValueCollection[Id] = this->reader->GetValue(1);
			this->Grade1LevelCollection[Id] = this->reader->GetValue(3);
			Id++;
		}
		delete cmd;
		this->cmd = gcnew System::Data::OleDb::OleDbCommand("SELECT * FROM GradeChart2 ORDER BY ID",this->con);
		this->reader = this->cmd->ExecuteReader();
		Id = 1;
		while(reader->Read())
		{
			this->Grade2ValueCollection[Id] = this->reader->GetValue(1);
			this->Grade2LevelCollection[Id] = this->reader->GetValue(3);
			Id++;
		}
		delete cmd;*/

		this->cmd = gcnew System::Data::OleDb::OleDbCommand("SELECT * FROM GradeChart ORDER BY ID",this->con);
		this->reader = this->cmd->ExecuteReader();
		while(reader->Read())
		{
			this->Grade1ValueCollection[this->reader->GetValue(0)] = this->reader->GetValue(1);
			this->Grade1ValueEndCollection[this->reader->GetValue(0)] = this->reader->GetValue(2);
			this->Grade1LevelCollection[this->reader->GetValue(0)] = this->reader->GetValue(3);
		}
		delete cmd;
		this->cmd = gcnew System::Data::OleDb::OleDbCommand("SELECT * FROM GradeChart2 ORDER BY ID",this->con);
		this->reader = this->cmd->ExecuteReader();
		
		while(reader->Read())
		{
			this->Grade2ValueCollection[this->reader->GetValue(0)] = this->reader->GetValue(1);
			this->Grade2ValueEndCollection[this->reader->GetValue(0)] = this->reader->GetValue(2);
			this->Grade2LevelCollection[this->reader->GetValue(0)] = this->reader->GetValue(3);
		}
		delete cmd;

	}
	catch(Exception^ ex)
	{ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0002", ex); }
}

//This funtion will create the database interface for the toolstripbuttons..//
void RWrapper::RW_DBSettings::createToolButtonInterface()
{
	try
	{
		this->_toolStripButtonToolTipCommand = gcnew System::Data::OleDb::OleDbCommand("SELECT * FROM ToolStripButton ORDER BY Frequency DESC",this->con);
		this->_toolStripButtonAdapter = gcnew System::Data::OleDb::OleDbDataAdapter("SELECT * FROM ToolStripButton ORDER BY Frequency DESC",RWrapper::RW_MainInterface::DBConnectionString);
		this->_toolStripButtonSet = gcnew System::Data::DataSet();
		this->_toolStripButtonAdapter->Fill(this->_toolStripButtonSet, "ToolStripButton");
		this->_toolStripCommandBuilder = gcnew System::Data::OleDb::OleDbCommandBuilder(this->_toolStripButtonAdapter);
		this->_toolStripCommandBuilder->QuotePrefix = "[";
		this->_toolStripCommandBuilder->QuoteSuffix = "]";
		this-> _toolStripButtonImageCommand = gcnew System::Data::OleDb::OleDbCommand("SELECT ButtonIcon FROM ToolStripButton ORDER BY Frequency DESC",this->con);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0003", ex); }
}

void RWrapper::RW_DBSettings::GetSavedLightSettings()
{
	try
	{
		this->cmd = gcnew System::Data::OleDb::OleDbCommand("SELECT * FROM LightingSetting",this->con);
		this->reader = this->cmd->ExecuteReader();
		while(reader->Read())
		{
			this->SavedLightSettings_NameList->Add(this->reader->GetString(1));
		}
		reader->Close();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0003", ex); }
}

void RWrapper::RW_DBSettings::SaveLightSettings(System::String^ LName, cli::array<int, 1>^ lightvalue)
{
	try
	{
		if(this->SavedLightSettings_NameList->Contains(LName))
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsSaveLightSettings01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(LName == "")
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsSaveLightSettings02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		this->con->Open();
		System::String^ CmdStr = "Insert into LightingSetting (LightName,zone1,zone2,zone3,zone4,zone5,zone6,zone7,zone8)values('" + LName + "','" + lightvalue[0] + "','" + lightvalue[1] + "','" + lightvalue[2] + "','" + lightvalue[3] + "','" + lightvalue[4] + "','" + lightvalue[5] + "','" + 0 + "','" + 0 + "')";
		this->cmd = gcnew System::Data::OleDb::OleDbCommand(CmdStr,this->con);
		this->cmd->ExecuteNonQuery();
		this->con->Close();
		this->SavedLightSettings_NameList->Add(LName);
	}
	 catch(Exception^ ex){ this->con->Close(); RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0004", ex); }  
}

void RWrapper::RW_DBSettings::UpdateSelectedLightValue(System::String^ LName)
{
	 try
	 {
		if(RWrapper::RW_CNC::MYINSTANCE()->IsLightUpdating())
			return;
		if(!this->SavedLightSettings_NameList->Contains(LName))
			return;
		this->con->Open();
		System::String^ CmdStr = "SELECT * FROM LightingSetting where LightName=" + "'" + LName + "'";
		this->cmd = gcnew System::Data::OleDb::OleDbCommand(CmdStr,this->con);
		this->reader = this->cmd->ExecuteReader();
		this->reader->Read();
		cli::array<int, 1>^ lightvalue = {0, 0, 0, 0, 0, 0, 0, 0};
		for(int i = 0; i < 8; i++)
			lightvalue[i] = System::Convert::ToInt32(this->reader->GetValue(2 + i));
		this->con->Close();
		RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);	
	 }
	 catch(Exception^ ex){ this->con->Close(); RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0005", ex); }    
}

void RWrapper::RW_DBSettings::DeleteSelectedLightValue(System::String^ LName)
{
	try
	{
		if(!this->SavedLightSettings_NameList->Contains(LName))
			return;
		this->SavedLightSettings_NameList->Remove(LName);
		this->con->Open();
		System::String^ CmdStr = "DELETE * FROM LightingSetting where LightName=" + "'" + LName + "'";
		this->cmd = gcnew System::Data::OleDb::OleDbCommand(CmdStr,this->con);
		this->cmd->ExecuteNonQuery();
		this->con->Close();	
	}
	catch(Exception^ ex){ this->con->Close(); RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0006", ex); }    
}

//Returns the cursor according to the name... To be displayed on Video picturebox//
System::IO::Stream^ RWrapper::RW_DBSettings::GetCursor(System::String^ IconName, System::String^ toolAlignment)
{
	try
	{
		this->_toolButtonReader = this->_toolStripButtonToolTipCommand->ExecuteReader();
		this->imageReader = _toolStripButtonImageCommand->ExecuteReader(System::Data::CommandBehavior::SequentialAccess);
		delete rawdata;
		while(_toolButtonReader->Read())
		{
			System::String^ CursorName = _toolButtonReader->GetString(1);
			System::String^ Alignment = _toolButtonReader->GetString(2);
			imageReader->Read();
			if(CursorName == IconName && Alignment == toolAlignment)
			{
				rawdata = gcnew cli::array<unsigned char>(900000);
				//Fill the byte array with the image
				INT64 retval = imageReader->GetBytes(0,0,rawdata,0,rawdata->Length);
			}
		}
		imageReader->Close();
		_toolButtonReader->Close();
		System::IO::Stream^ tst = gcnew System::IO::MemoryStream(rawdata);
		return tst;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0008", ex); }  
}

System::Windows::Controls::Image^ RWrapper::RW_DBSettings::getOneImage_RunTime(System::String^ baseName, System::String^ toolAlignment, int width, int height)
{
	try
	{	
		if(con->State == ConnectionState::Closed)
			this->con->Open();
		this->_toolButtonReader = this->_toolStripButtonToolTipCommand->ExecuteReader();
		this->imageReader = _toolStripButtonImageCommand->ExecuteReader(System::Data::CommandBehavior::SequentialAccess);
		System::Windows::Controls::Image^ img = gcnew System::Windows::Controls::Image;
		while(_toolButtonReader->Read())
		{
			System::String^ btnName = _toolButtonReader->GetString(1);
			System::String^ alignment = _toolButtonReader->GetString(2);
			imageReader->Read();
			if(btnName == baseName && alignment == toolAlignment)
			{
				ButtonToolTip = _toolButtonReader->GetString(LangCid);
				rawdata = gcnew cli::array<unsigned char>(900000);
				INT64 retval = imageReader->GetBytes(0, 0, rawdata, 0, rawdata->Length);
				System::Windows::Media::Imaging::BitmapImage^bmp = gcnew System::Windows::Media::Imaging::BitmapImage;
				bmp->CacheOption = System::Windows::Media::Imaging::BitmapCacheOption::OnLoad;
				bmp->BeginInit();
				bmp->StreamSource = gcnew System::IO::MemoryStream(rawdata);
				bmp->EndInit();
				img->Source=bmp;
				img->Width = width;
				img->Height = height;
			}
			delete rawdata;
		}
		imageReader->Close();
		_toolButtonReader->Close();
		this->con->Close();
		return img;
	}
	catch(Exception^ ex){ this->con->Close(); RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0009", ex); }    
}

System::Collections::Generic::List<System::Windows::Controls::Image^>^ RWrapper::RW_DBSettings::getImageList(System::String^ toolAlignment, int width, int height)
{
	try
	{	
		if(con->State == ConnectionState::Closed)
			this->con->Open();
		this->_toolButtonReader = this->_toolStripButtonToolTipCommand->ExecuteReader();
		this->imageReader = this->_toolStripButtonImageCommand->ExecuteReader(System::Data::CommandBehavior::SequentialAccess);
		this->_Buttontype->Clear();
		this->_ButtonBaseName->Clear();
		this->_ButtonToolTip->Clear();
		this->_ButtonImage->Clear();
		this->_ButtonCheckedProperty->Clear();
		int _rowNumber = 0;
		while(_toolButtonReader->Read())
		{
			System::String^ alignment = _toolButtonReader->GetString(2);
			imageReader->Read();
			if(alignment == toolAlignment)
			{
				System::String^ BbaseName = _toolButtonReader->GetString(1);
				if(CompanyName != "Customised Technologies (P) Ltd")
				{
					bool ThreeDBtn = Convert::ToBoolean(_toolButtonReader->GetValue(8));
					bool DisableBtn = Convert::ToBoolean(_toolButtonReader->GetValue(6));
					if(DisableBtn)
						continue;
					if(alignment == "RCAD Toolbar")
					{
						if(MachineType != "Rotary" && BbaseName == "RotaryMeasure")
							continue;
					}
					if(alignment == "Measure")
					{
						if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
							MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
							MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
						{
							if(BbaseName == "OD Measure" || BbaseName == "ID Measure")
								continue;
						}
					}
					if(!ThreeDSoftwareMode)
					{
						if(ThreeDBtn) continue;
					}
				}

				this->_ButtonToolTip->Add(_toolButtonReader->GetString(LangCid));
				this->_Buttontype->Add(Convert::ToInt32(_toolButtonReader->GetValue(5)));
				this->_ButtonCheckedProperty->Add(Convert::ToBoolean(_toolButtonReader->GetValue(7)));
				this->_ButtonBaseName->Add(_toolButtonReader->GetString(1));
				// Fill the byte array with the image
				rawdata = gcnew cli::array<unsigned char>(900000);
				INT64 retval = imageReader->GetBytes(0,0,rawdata,0,rawdata->Length);
				// converting byte array into the image of type Windows.media.imaging (written by avinash on 11 oct 2009)
				System::Windows::Controls::Image^ img = gcnew System::Windows::Controls::Image;
				System::Windows::Media::Imaging::BitmapImage^ bmp = gcnew System::Windows::Media::Imaging::BitmapImage;
				bmp->CacheOption = System::Windows::Media::Imaging::BitmapCacheOption::OnLoad;
				bmp->BeginInit();
				bmp->StreamSource = (gcnew System::IO::MemoryStream(rawdata));
				bmp->EndInit();
				img->Source = bmp;
				img->Width = width;
				img->Height = height;
				this->_ButtonImage->Add(img);
			}
			_rowNumber += 1;
			delete rawdata;
		}
		imageReader->Close();
		_toolButtonReader->Close();
		this->con->Close();
		return _ButtonImage;
	}
	catch(Exception^ ex){ this->con->Close(); RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0010", ex); }    
}

System::Collections::Generic::List<System::String^>^ RWrapper::RW_DBSettings::getButtonBaseNameList()
{
	return _ButtonBaseName;
}

System::Collections::Generic::List<System::String^>^ RWrapper::RW_DBSettings::getToolTipList()
{
	return _ButtonToolTip;
}

System::Collections::Generic::List<System::Int32>^ RWrapper::RW_DBSettings::getButtonTypeList()
{
	return _Buttontype;
}

System::Collections::Generic::List<System::Boolean>^ RWrapper::RW_DBSettings::getButtonCheckedPropertyList()
{
	return _ButtonCheckedProperty;
}

System::Collections::Generic::List<System::String^>^ RWrapper::RW_DBSettings::GetMagnificationList()
{
	return MagNameList;
}

void RWrapper::RW_DBSettings::UpdateVideoPixelWidthForDigitalZoom(System::String^ _magLevel, bool CurrentState)
{
	try
	{	
		if(CurrentState)																				//if true call zoom off... vinod
		{
			double Pixel_X = System::Convert::ToDouble(PixelWidthList_X[_magLevel]) / 2;
			double Pixel_Y = System::Convert::ToDouble(PixelWidthList_Y[_magLevel]) / 2;
			EDSCOBJECT->RefDotDiameter = (this->RefDotDia_in_mm * M_PI) / Pixel_X;
			MAINDllOBJECT->RCadApp::DigitalZoomON(Pixel_X, Pixel_Y);									//caling zoom off ,previously it caals same fn zoom on.. for off click, so it didnt work .. vinod
		}
		else
		{
			double Pixel_X = System::Convert::ToDouble(PixelWidthList_X[_magLevel]);
			double Pixel_Y = System::Convert::ToDouble(PixelWidthList_Y[_magLevel]);
			EDSCOBJECT->RefDotDiameter = (this->RefDotDia_in_mm * M_PI) / Pixel_X;
			MAINDllOBJECT->DigitalZoomOff(Pixel_X, Pixel_Y);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

// storing the pixel width based on selected magnification level
void RWrapper::RW_DBSettings::UpdateVideoPixelWidth(System::String ^_magLevel)
{
	try
	{
		double multiplier = 1.0;
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::PROBE_ONLY || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER) return;

		if(MAINDllOBJECT->DigitalZoomIsOn)
		{
			UpdateVideoPixelWidthForDigitalZoom(_magLevel, true);
		}
		else if (_magLevel->StartsWith("D"))
		{
			multiplier = double::Parse(_magLevel->Substring(1));
			System::String^ MagL = gcnew String(MAINDllOBJECT->LastSelectedMagLevel.c_str());
			if (MagL == "") MagL = "67X";
			double Pixel_X = System::Convert::ToDouble(PixelWidthList_X[MagL]) * multiplier;
			double Pixel_Y = System::Convert::ToDouble(PixelWidthList_Y[MagL]) * multiplier;
			if (Pixel_X == 0) Pixel_X = 0.003;
			if (Pixel_Y == 0) Pixel_Y = 0.003;
			EDSCOBJECT->RefDotDiameter = (this->RefDotDia_in_mm * M_PI) / Pixel_X;
			MAINDllOBJECT->SetVideoPixelWidth(Pixel_X, Pixel_Y);
		}
		else
		{
			UpdateVideoPixelWidthForDigitalZoom(_magLevel, false);
			if (RWrapper::RW_MainInterface::MYINSTANCE()->DigitalZoomLevel != 0.0)
				multiplier *= RWrapper::RW_MainInterface::MYINSTANCE()->DigitalZoomLevel;
			bool last11xstatus = ElevenXProfilePresent;
			ElevenXProfilePresent = System::Convert::ToBoolean(ElvnXProfileMode[_magLevel]);
			double Pixel_X = System::Convert::ToDouble(PixelWidthList_X[_magLevel]) * multiplier;
			double Pixel_Y = System::Convert::ToDouble(PixelWidthList_Y[_magLevel]) * multiplier;
			if (Pixel_X == 0) Pixel_X = 0.003;
			if (Pixel_Y == 0) Pixel_Y = 0.003;
			EDSCOBJECT->RefDotDiameter = (this->RefDotDia_in_mm * M_PI) / Pixel_X;
			MAINDllOBJECT->SetVideoPixelWidth(Pixel_X, Pixel_Y);
			if(last11xstatus != ElevenXProfilePresent)
				RWrapper::RW_CNC::MYINSTANCE()->UpdateLight_ForZoomChange();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

void RWrapper::RW_DBSettings::UpdateVideoUpp_DROCallback(double currRvalue)
{
	try
	{
		double NewPixel_X, NewPixel_Y;
		if(!RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		{
			double StartPixel_X = System::Convert::ToDouble(PixelWidthList_X[MagNameList[0]]);
			double EndPixel_X = System::Convert::ToDouble(PixelWidthList_X[MagNameList[MagNameList->Count - 1]]);
			double StartPixel_Y = System::Convert::ToDouble(PixelWidthList_Y[MagNameList[0]]);
			double EndPixel_Y = System::Convert::ToDouble(PixelWidthList_Y[MagNameList[MagNameList->Count - 1]]);
			double DiffPixel_X = StartPixel_X - EndPixel_X;
			double DiffPixel_Y = StartPixel_Y - EndPixel_Y;
			NewPixel_X = EndPixel_X + DiffPixel_X * currRvalue;
			NewPixel_Y = EndPixel_Y + DiffPixel_Y * currRvalue;
		}
		else
		{
	/*		if(CheckForTargetReach)
			{
			   if(!(((R_Last_Value - R_Lower_Correction) < currRvalue) && (currRvalue < (R_Last_Value + R_Upper_Correction))))
			   {
				   double newtarget = R_Last_Value;
					if(currRvalue < R_Last_Value)
						newtarget -= R_Lower_Correction;
					else
						newtarget += R_Upper_Correction;
					double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], newtarget};
					double feedrate[4] = {5, 5, 5, 5};
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
					Sleep(30);
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_ZOOMINOUTMODE);
					RWrapper::RW_MainInterface::MYINSTANCE()->StartHideWaitStatusMessage(9 * 1000);
					RWrapper::RW_MainInterface::MYINSTANCE()->ShowWaitStatusMessage("Please wait... Camera Zooming", 10);
					DROInstance->ResetLookingSymbol();
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
					return;
			   }
			}
			CheckForTargetReach = false;*/
			double CurrentPixel;
			int CurrentZ;
			GetZoomLevelFromRValue(currRvalue, &CurrentZ);
			if(GetPixelWidthFromRValue(currRvalue, &CurrentPixel)) 
			{
				NewPixel_X = CurrentPixel;
				NewPixel_Y = CurrentPixel;
			}
			else
			{
			 if( currRvalue > R_DistList[(R_DistList->Count) - 1])
			 {
				 NewPixel_X = System::Convert::ToDouble(R_PixelWidth[R_PixelWidth->Count - 1]);
				 NewPixel_Y = System::Convert::ToDouble(R_PixelWidth[R_PixelWidth->Count - 1]);
			 }
			 else if(currRvalue < R_DistList[0])
			 {
				 NewPixel_X = System::Convert::ToDouble(R_PixelWidth[0]);
				 NewPixel_Y = System::Convert::ToDouble(R_PixelWidth[0]);
			 }
			 else
				 return;
			}
		}
		EDSCOBJECT->RefDotDiameter = (this->RefDotDia_in_mm * M_PI) / NewPixel_X;
		MAINDllOBJECT->SetVideoPixelWidth(NewPixel_X, NewPixel_Y);

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

void RWrapper::RW_DBSettings::UpdateCameraZoomLevel(System::String^ _magLevel)
{
	try 
	{
		double Rcount;
		if(!RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		{
			double StartPixel = System::Convert::ToDouble(PixelWidthList_X[MagNameList[0]]);
			double EndPixel = System::Convert::ToDouble(PixelWidthList_X[MagNameList[MagNameList->Count - 1]]);
			double CurrentPixel = System::Convert::ToDouble(PixelWidthList_X[_magLevel]);
			Rcount = 1 - ((StartPixel - CurrentPixel) / (StartPixel - EndPixel));
		}
		else
		{
			ElevenXProfilePresent = System::Convert::ToBoolean(ElvnXProfileMode[_magLevel]);

			int CurrentMagvalue = System::Convert::ToInt32((*_magLevel).Split('X')[0]);
			if (RWrapper::RW_MainInterface::MYINSTANCE()->TwoXLensFitted) CurrentMagvalue /= 2;

			if(!GetRValueFromZoomLevel(&Rcount, System::Convert::ToInt32(CurrentMagvalue))) return;
			R_Last_Value = Rcount;

			if(_magLevel == "11X")
			{
				Rcount -= 10;
				R_Last_Value = 0;
			}
		}
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], Rcount};
		double feedrate[4] = {5, 5, 5, 5};
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		Sleep(30);
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_ZOOMINOUTMODE);
		RWrapper::RW_MainInterface::MYINSTANCE()->StartHideWaitStatusMessage(9 * 1000);
		RWrapper::RW_MainInterface::MYINSTANCE()->ShowWaitStatusMessage("Please wait... Camera Zooming In"+ _magLevel, 10, false);
		DROInstance->ResetLookingSymbol();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

bool RWrapper::RW_DBSettings::GetRValueFromPixelWidth(double* Rcount, double _pixelWidth)
{
	try 
	{
			  double pixelW1 = -1, pixelW2 = -1, R1 = -1, R2 = -1;
			  for (int i = 0; i < (R_PixelWidth->Count - 1); i++)
			  {
					 if ((R_PixelWidth[i] >= _pixelWidth) && (_pixelWidth >= R_PixelWidth[i+1]))
					 {
						   pixelW1 = R_PixelWidth[i];
						   pixelW2 = R_PixelWidth[i+1];
						   R1 = R_DistList[i];
						   R2 = R_DistList[i+1];
						   break;
					 }
			  }
			  if (pixelW1 < 0)
					 return false;
			  if (pixelW1 == pixelW2)
					 return false;
			  *Rcount = R1 + ((R2 - R1)/(pixelW2 - pixelW1)) * (_pixelWidth - pixelW1);
			  return true;

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

bool RWrapper::RW_DBSettings::GetRValueFromZoomLevel(double* Rcount, int zoomLevel)
{
	try 
	{
			  double zoomLevel1 = -1, zoomLevel2 = -1, R1 = -1, R2 = -1;
			  for (int i = 0; i < (R_ZoomLevel->Count - 1); i++)
			  {
					 if ((R_ZoomLevel[i] <= zoomLevel) && (zoomLevel <= R_ZoomLevel[i+1]))
					 {
						   zoomLevel1 = R_ZoomLevel[i];
						   zoomLevel2 = R_ZoomLevel[i+1];
						   R1 = R_DistList[i];
						   R2 = R_DistList[i+1];
						   break;
					 }
			  }
			  if (zoomLevel1 < 0)
					 return false;
			  if (zoomLevel1 == zoomLevel2)
					 return false;
			  *Rcount = R1 + ((R2 - R1)/(zoomLevel2 - zoomLevel1)) * (zoomLevel - zoomLevel1);
			  return true;

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

bool RWrapper::RW_DBSettings::GetPixelWidthFromRValue(double Rcount, double *_pixelWidth)
{
	try 
	{
			  double pixelW1 = -1, pixelW2 = -1, R1 = -1, R2 = -1;
			  if (Rcount > R_DistList[(R_DistList->Count) - 1])
			  {
					pixelW1 = R_PixelWidth[(R_DistList->Count) - 2];
					pixelW2 = R_PixelWidth[(R_DistList->Count) - 1];
					R1 = R_DistList[(R_DistList->Count) - 2];
					R2 = R_DistList[(R_DistList->Count) - 1]; 
			  }
			  else if (Rcount < R_DistList[0])
			  {
					pixelW1 = R_PixelWidth[0];
					pixelW2 = R_PixelWidth[1];
					R1 = R_DistList[0];
					R2 = R_DistList[1]; 
			  }
			  else
			  {
				  for (int i = 0; i < ((R_DistList->Count) - 1); i++)
				  {
						 if ((R_DistList[i] <= Rcount) && (Rcount <= R_DistList[i+1]))
						 {
							   pixelW1 = R_PixelWidth[i];
							   pixelW2 = R_PixelWidth[i+1];
							   R1 = R_DistList[i];
							   R2 = R_DistList[i+1];
							   break;
						 }
				  }
			  }
			  if (R1 < 0)
					 return false;
			  if (R1 == R2)
					 return false;
			  *_pixelWidth = pixelW1 + ((pixelW2 - pixelW1)/(R2 - R1)) * (Rcount - R1);
			  return true;

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

bool RWrapper::RW_DBSettings::GetZoomLevelFromRValue(double Rcount, int *zoomLevel)
{
	try 
	{
			  double zoomLevel1 = -1, zoomLevel2 = -1, R1 = -1, R2 = -1;
			  if (Rcount > R_DistList[(R_DistList->Count) - 1])
			  {
					zoomLevel1 = R_ZoomLevel[(R_ZoomLevel->Count) - 2];
					zoomLevel2 = R_ZoomLevel[(R_ZoomLevel->Count) - 1];
					R1 = R_DistList[(R_DistList->Count) - 2];
					R2 = R_DistList[(R_DistList->Count) - 1]; 
			  }
			  else if (Rcount < R_DistList[0])
			  {
					zoomLevel1 = R_ZoomLevel[0];
					zoomLevel2 = R_ZoomLevel[1];
					R1 = R_DistList[0];
					R2 = R_DistList[1]; 
			  }
			  else
			  {
				  for (int i = 0; i < ((R_DistList->Count) - 1); i++)
				  {
						if ((R_DistList[i] <= Rcount) && (Rcount <= R_DistList[i+1]))
						{
							zoomLevel1 = R_ZoomLevel[i];
							zoomLevel2 = R_ZoomLevel[i+1];
							R1 = R_DistList[i];
							R2 = R_DistList[i+1];
							break;
						}
				  }
			  }
			  if (R1 < 0)
					 return false;
			  if (R1 == R2)
					 return false;
			  *zoomLevel = (int)(zoomLevel1 + ((zoomLevel2 - zoomLevel1)/(R2 - R1)) * (Rcount - R1));
			  return true;

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0007", ex); }  
}

//Reads the value from the Rapidsettings database... Need to Modify..Split!!
void RWrapper::RW_DBSettings::GetValuesFromDatabase(bool InitialCall)
{
		CalibrationModule::DB ^GetValues = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		MagNameList->Clear();
		PixelWidthList_X->Clear();
		PixelWidthList_Y->Clear();
		ElvnXProfileMode->Clear();
		//Read the value from data base values.. Settings values...//
		cli::array<System::String^>^ TableNames = { "UserSettings", "MeasurementSettings", "LightingSettings", "MachineDetails", "MagLevels", "MagnifierSettings", "FixtureDetails", "DXFSettings",
													"CurrentMachine", "LanguageTypeLookup", "VisionSettings","CameraAxisValue", "ProbeSettings", "ProbAssemblyTypeDetails", "FeatureLicenseInfo",
													"ZeroingOffset", "DefaultCamSettings" }; // , "TIS_Settings"
		GetValues->FillTableRange(TableNames);	

		System::Data::DataTable^ dt = GetValues->GetTable("CurrentMachine");
		cli::array<System::Data::DataRow^>^ currMachineTable = dt->Select("C_Id >= 0", "C_Id ASC");
		System::String^ MachineNo = currMachineTable[currMachineTable->Length - 1]["MachineNo"]->ToString();

		//System::Windows::MessageBox::Show(MachineNo);
		//System::String^ MachineNo = System::Convert::ToString(GetValues->GetRecord["CurrentMachine",GetValues->GetRowCount("CurrentMachine")-1,"MachineNo"]);
		CurrentMachineNumber = MachineNo;
		
		NoOfCards = System::Convert::ToInt32(GetValues->Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]->default["LightValue"]);
		try
		{
			Profile_Zone_No = System::Convert::ToInt32(GetValues->Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]->default["Profile_Zone_No"]);
			Profile11X_Zone_No = System::Convert::ToInt32(GetValues->Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]->default["Profile11X_Zone_No"]);
			Aux_Zone_No = System::Convert::ToInt32(GetValues->Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]->default["Aux_Zone_No"]);
			CoAx_ZoneNo = System::Convert::ToInt32(GetValues->Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]->default["CoAx_ZoneNo"]);
		}
		catch (Exception^ Ex)
		{
			Profile_Zone_No = 4;
			Aux_Zone_No = 5;
			CoAx_ZoneNo = 6;
			Profile11X_Zone_No = 7;
		}
		FOCUSCALCOBJECT->FocusActiveWidth(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["DTol"]));
		FOCUSCALCOBJECT->MaxMinFocusRatioCutoff(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["MaxMinFocusRatioCutoff"]));
		FOCUSCALCOBJECT->MaxMinZRatioCutoff(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["MaxMinZRatioCutoff"]));
		FOCUSCALCOBJECT->NoiseFilterFactor(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["NoiseFilterFactor"]));
		FOCUSCALCOBJECT->GrainFactor(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["GrainFactor"]));
		FOCUSCALCOBJECT->SigmaActiveWdthRatioCutoff(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SigmaActiveWdthRatioCutoff"]));
		MAINDllOBJECT->KeepGraphicsStraightflag(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ShowGraphicsStraight"]));
		MAINDllOBJECT->ShowNameTagFlag(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ShowNameTaginVideo"]));
		PPCALCOBJECT->DoShortestPath(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["UseOptimisedPath"]));
		DXFEXPOSEOBJECT->DeviationWithLineOnly = System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ShwDeviationForLnsOnly"]);
		PPCALCOBJECT->ExportPointsToCsvInPP(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ExportShpPtsToCSVDuringPP"]));
		PPCALCOBJECT->ExportParamToCsvInPP(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ExportShpParamsToCSVDuringPP"]));
		MAINDllOBJECT->ShowPictorialView(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["CloudPtsContourColouring"]));		
		MAINDllOBJECT->RcadBackGround_White = System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["InvertRcadImgBackColour"]);		
		MAINDllOBJECT->MinimumPointsInEdge(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", CurrentMachineNumber)[0]->default["ProfileScanCutoffPoint"]));
		MAINDllOBJECT->SkipPixelWidth(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ProfileScanRectHorizontalBorderBuffer"]));
		MAINDllOBJECT->SkipPixelHeight(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ProfileScanRectHorizontalBorderBuffer"]));
		MAINDllOBJECT->PixelNeighbour(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ProfileScanNeighbourPtDistTolerance"]));
		MAINDllOBJECT->ProfileScanMoveToNextFrameFactor = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ProfileScanMoveToNextFrameFraction"]);
		MAINDllOBJECT->BestFit_PointsToBundle = System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["BestFitPointsBundleForDia"]);
		MAINDllOBJECT->ArcBestFitType_Dia = System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ArcBestFitTypeForDia"]);
		MAINDllOBJECT->FilterCorrectionFactor(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["FilterFactor"]));
		MAINDllOBJECT->FilterCorrectionIterationCnt(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["FilterIterationFactor"]));
		MAINDllOBJECT->UseAvImage(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["UseAverageImage"]));
		MAINDllOBJECT->NoOfFramesToAverage(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["ImageCountForAveraging"]));
		MAINDllOBJECT->RepeatCount(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["RepeatCount"]));
		MAINDllOBJECT->SelectedAlgorithm(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", CurrentMachineNumber)[0]->default["SelectedAlgorithm"]));
		MAINDllOBJECT->CurrentMCSType = RapidEnums::MCSTYPE(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", CurrentMachineNumber)[0]->default["MCSType"]));
		try
		{
			MAINDllOBJECT->CNCAssisted_FG(System::Convert::ToBoolean(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["CNCAssisted_FG"]));
			MAINDllOBJECT->Force_AutoFG_InAssistedMode(!MAINDllOBJECT->CNCAssisted_FG());
		}
		catch (const std::exception&)
		{

		}
		//System::Windows::MessageBox::Show("2");

		RWrapper::RW_MainInterface::MYINSTANCE()->Do_Homing_At_Startup = System::Convert::ToBoolean(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["AutoHoming_at_Start"]);

		MAINDllOBJECT->FloatDXFwithMouseMove = !System::Convert::ToBoolean(GetValues->Select_ChildControls("DXFSettings", "MachineNo", MachineNo)[0]->default["DoNotFloatDXFwithMouseMove"]);
		
		MAINDllOBJECT->Dots_PointsMinCount = System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["MinimumPointsRequiredToDetermineShape"]);
		MAINDllOBJECT->Dots_PixelCount = System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["MinimumPixelDifferenceBetweenShapes"]);

		DXFEXPOSEOBJECT->AllPointsAsCloudPoint(System::Convert::ToBoolean(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["AllPointAsCloudPoints"]));
		DXFEXPOSEOBJECT->DxfShapeContinuity(System::Convert::ToBoolean(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["MaintainDxfShpContinuity"]));
		DXFEXPOSEOBJECT->ExportShapeAsZequaltoZero = System::Convert::ToBoolean(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["ExportDxfShapesAtZZero"]);
		MAINDllOBJECT->UseMultiThreads_Surfacebuild(System::Convert::ToBoolean(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["OptimiseSurfaceBuild"]));
		MAINDllOBJECT->TriangleStepLength(System::Convert::ToInt32(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["SurfaceStepLength"]));
		MAINDllOBJECT->PointClusterSize(System::Convert::ToInt32(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["SurfacePtClustSize"]));
		MAINDllOBJECT->CloudPtdistance(System::Convert::ToDouble(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["SurfacePtGap"]));
		MAINDllOBJECT->BestFitSurface_pts_around_xy_plane(System::Convert::ToBoolean(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["CloudPtsAroundXYPlane"]));
		MAINDllOBJECT->BestFitSurface_PtsFactor(System::Convert::ToInt32(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["BestFitSurfaceTolerance"]));
		DXFEXPOSEOBJECT->ExportShapesOfAllUcsToDxf = System::Convert::ToBoolean(GetValues->Select_ChildControls("DXFSettings","MachineNo", MachineNo)[0]->default["ExportAllShpsToSingleUCS"]);

		double Tmp_CameraDistortionFactor = MAINDllOBJECT->CameraDistortionFactor;
		long  Tmp_CamXOffSet = MAINDllOBJECT->CamXOffSet, Tmp_CamYOffSet = MAINDllOBJECT->CamYOffSet;
		MAINDllOBJECT->CamXOffSet = System::Convert::ToInt64(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["CamDistortionOffSetX"]);
		MAINDllOBJECT->CamYOffSet = System::Convert::ToInt64(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["CamDistortionOffSetY"]);
		MAINDllOBJECT->CameraDistortionFactor = System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["CamDistortionFactor"]);
		if(Tmp_CameraDistortionFactor != MAINDllOBJECT->CameraDistortionFactor || Tmp_CamXOffSet != MAINDllOBJECT->CamXOffSet || Tmp_CamYOffSet != MAINDllOBJECT->CamYOffSet)
		{
			MAINDllOBJECT->ResetCameraDistortionParameter(MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet);
		}

		MAINDllOBJECT->PixelOffset_X(System::Convert::ToInt32(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["Xoffset"]));
		MAINDllOBJECT->PixelOffset_Y(System::Convert::ToInt32(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["Yoffset"]));
		MAINDllOBJECT->XPositionStart_Right(System::Convert::ToInt32(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["XPositionStartRight"]));
		MAINDllOBJECT->XPositionStart_Left(System::Convert::ToInt32(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["XPositionStartLeft"]));
		MAINDllOBJECT->YPositionStart_Top(System::Convert::ToInt32(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["YPositionStartTop"]));
		MAINDllOBJECT->YPositionStart_Bottom(System::Convert::ToInt32(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["YPositionStartBottom"]));
		MAINDllOBJECT->XMultiplyFactor_Right(System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["XMultiplyFactorRight"]));
		MAINDllOBJECT->XMultiplyFactor_Left(System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["XMultiplyFactorLeft"]));
		MAINDllOBJECT->YMultiplyFactor_Top(System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["YMultiplyFactorTop"]));
		MAINDllOBJECT->YMultiplyFactor_Bottom(System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["YMultiplyFactorBottom"]));
		//MAINDllOBJECT->Align_by_CG(System::Convert::ToBoolean(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["Align_by_CG"]));
		//System::Windows::MessageBox::Show("3");

		System::String^ LenseType = System::Convert::ToString(GetValues->Select_ChildControls("VisionSettings","MachineNo", MachineNo)[0]->default["CamLenseType"]);
		if(LenseType == "1X")
			MAINDllOBJECT->CurrentLenseType = RapidEnums::RAPIDLENSETYPE::LENSE_1X;
		else if(LenseType == "2X")
			MAINDllOBJECT->CurrentLenseType = RapidEnums::RAPIDLENSETYPE::LENSE_2X;
		else if(LenseType == "5X")
			MAINDllOBJECT->CurrentLenseType = RapidEnums::RAPIDLENSETYPE::LENSE_5X;
		
		MAINDllOBJECT->SamplePtsCutOff = System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["WriteToHardDiskPtsLimit"]);
		MAINDllOBJECT->VideoFrameRate(System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["FrameRate"]));
		MAINDllOBJECT->AutomaticTwoStepHoming(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["TwoStepHoming"]));
		RWrapper::RW_MainInterface::MYINSTANCE()->Delay_AfterTargetReached = System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["DelayAfterTargetReached"]);
		RWrapper::RW_MainInterface::MYINSTANCE()->DealyinSec = System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["AccessoryLenses"]);
		RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["MultiLevelZoomMachine"]);

		MAINDllOBJECT->R_Axis_ForMeasurement = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]->default["RotaryAxisForMeasurement"]);
		//System::Windows::MessageBox::Show("3");

		//RWrapper::RW_MainInterface::MYINSTANCE()->SetMachineType(System::Convert::ToString(GetValues->GetRecord["MachineDetails",GetValues->GetRowCount("MachineDetails")-1,"MachineType"]), false);
		cli::array<System::Data::DataRow^>^ MagRows = GetValues->Select_ChildControls("MagLevels","MachineNo",MachineNo);
		for(int i = 0 ;i < MagRows->Length; i++)
		{
			bool CheckHidden = System::Convert::ToBoolean(MagRows[i]->default["Hidden"]);
			if(!CheckHidden)
			{
				System::String^ MaglevelX = System::Convert::ToString(MagRows[i]->default["MagLevelValue"]);
				this->MagNameList->Add(MaglevelX);
				this->PixelWidthList_X[MaglevelX] = MagRows[i]->default["PixelWidth"];
				this->PixelWidthList_Y[MaglevelX] = MagRows[i]->default["PixelWidthY"];
				this->DigitalZoomPixel_X[MaglevelX] = MagRows[i]->default["DigitalZoomPixel_X"];
				this->DigitalZoomPixel_Y[MaglevelX] = MagRows[i]->default["DigitalZoomPixel_Y"];
				this->ElvnXProfileMode[MaglevelX] = System::Convert::ToBoolean(MagRows[i]->default["ElvnXProfileLight"]);
			}
		}

		DROStyle = System::Convert::ToString(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["DROStyle"]);
		CompanyName = System::Convert::ToString(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["CompanyName"]);
		CompanyLocation = System::Convert::ToString(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["Location"]);
		SoftwareVersion = System::Convert::ToString(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["StartVersionNo"]);
		RWrapper::RW_MainInterface::MYINSTANCE()->AllowStepImport = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["AllowStepFileImport"]);
		RWrapper::RW_MainInterface::MYINSTANCE()->UseLookAhead = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["UseLookAhead"]);
		//System::Windows::MessageBox::Show("4");

		Camera_Type = System::Convert::ToString(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["CameraType"]);
		MAINDllOBJECT->DigitalZoomModeForOneShot(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["UseDigitalZoominOneShot"]));
		NoofMachineAxis = System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["NoOfAxes"]);
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		{
			this->R_Upper_Correction = System::Convert::ToDouble(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["R_Upper_Correction"]);
			this->R_Lower_Correction = System::Convert::ToDouble(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["R_Lower_Correction"]);
			cli::array<System::Data::DataRow^>^ CamAxisRows = GetValues->Select_ChildControls("CameraAxisValue","MachineNo",MachineNo);
			for(int i = 0 ;i < CamAxisRows->Length; i++)
			{
				this->R_DistList->Add(System::Convert::ToDouble(CamAxisRows[i]->default["AxisValue_R"]));
				this->R_PixelWidth->Add(System::Convert::ToDouble(CamAxisRows[i]->default["PixelValue_R"]));
				this->R_ZoomLevel->Add(System::Convert::ToInt32(CamAxisRows[i]->default["R_ZoomLevel"]));
			}
		}
		MeasureNoOfDec = System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["MesureNoOfDec"]);
		PPCALCOBJECT->OneShotTolPer(System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["OneShotImageTolerance"]));
		PPCALCOBJECT->OneShotTargetTolPer(System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["OneShotImageMatchTargetTolerance"]));
		PPCALCOBJECT->OneShotImgBoundryWidth(System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["OneShotImageBoundryWidth"]));
		PPCALCOBJECT->OneShotImgBinariseVal(System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["OneShotImageBinariseValue"]));
		PPCALCOBJECT->NoOfFeatures(GetValues->GetRowCount("FeatureLicenseInfo"));
		MAINDllOBJECT->MeasurementNoOfDecimal(MeasureNoOfDec);
		RMATH2DOBJECT->MeasureDecValue = pow(10.0, -MeasureNoOfDec);
		MachineType = System::Convert::ToString(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["MachineType"]);
		//System::Windows::MessageBox::Show("5");

		if(MachineType == "FullCNC")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::FULLCNC;
		else if(MachineType == "AutoFocus")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::AUTOFOCUS;
		else if(MachineType == "Horizontal")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::HORIZONTAL;
		else if(MachineType == "Rotary Del_FI")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::ROTARY_DEL_FI;
		else if(MachineType == "Rotary Bosch_FI")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::ROTARY_BOSCH_FI;
		else if(MachineType == "One Shot" || MachineType == "One_Shot")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::ONE_SHOT;
		else if(MachineType == "Normal As One Shot")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT;
		else if(MachineType == "DSP")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::DSP;
		else if(MachineType == "Magnifier DSP")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::MAGNIFIER_DSP;
		else if(MachineType == "Horizontal DSP")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP;
		else if(MachineType == "ProbeOnly")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::PROBE_ONLY;
		else if(MachineType == "HeightGauge")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE;
		else if(MachineType == "Contour Tracer")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::HEIGHTGAUGE_VISION;
		else if(MachineType == "HobChecker")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::HOBCHECKER;
		else if(MachineType == "HeightGauge Vision")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::HEIGHTGAUGE_VISION;
		else if(MachineType == "Contour Tracer")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::TOUCHTYPE_CONTOURTRACER;
		else if(MachineType == "TIS")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::TIS;
		else if (MachineType == "OnlineSystem")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM;
		else if (MachineType == "VisionHeadOnly")
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::VISIONHEADONLY;

		bool EnableOneshotFeatures = System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["EnableOneshotFeatures"]);		
		if(EnableOneshotFeatures)
			MAINDllOBJECT->CurrentMahcineType = RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT;
		//System::Windows::MessageBox::Show("7");

		MAINDllOBJECT->StylusPolarity(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["Polarity"]));
		System::String^ GAxisMovement = System::Convert::ToString(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["GAxisMovement"]);
		MAINDllOBJECT->CurrentStylusType = RapidEnums::STYLUSTYPE::ALONG_ZAXIS;
		if (GAxisMovement == "Stylus Along X-Axis")
			MAINDllOBJECT->CurrentStylusType = RapidEnums::STYLUSTYPE::ALONG_XAXIS;
		else if(GAxisMovement == "Stylus Along Y-Axis")
			MAINDllOBJECT->CurrentStylusType = RapidEnums::STYLUSTYPE::ALONG_YAXIS;
		else if(GAxisMovement == "Stylus Along Z-Axis")
			MAINDllOBJECT->CurrentStylusType = RapidEnums::STYLUSTYPE::ALONG_ZAXIS;
		

		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ROTARY_BOSCH_FI)
		{
			RotartAxisPresent = true;
			NoofMachineAxis = 4;
		}
		else if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ROTARY_DEL_FI || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::TIS)
		{
			MachineType = "Rotary";
			NoofMachineAxis = 4;
			//gcnew RWrapper::RW_DelphiCamModule();
			try
			{
				MAINDllOBJECT->Rotary_YaxisOffset(System::Convert::ToDouble(GetValues->Select_ChildControls("ZeroingOffset", "MachineNo", MachineNo)[0]->default["OffestY"]));
			}
			catch(Exception^ ex){}
		}

		if(NoofMachineAxis == 4 && MachineType != "Rotary")
			RotartAxisPresent = true;
		else
			RotartAxisPresent = false;
		MachineTypeEnum = (int)MAINDllOBJECT->CurrentMahcineType;
		AuxilaryLightPresent = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["AuxiliaryLighting"]);
		CoAxialLightPresent = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["CoAxialLighting"]);
		FiveXLensePresent = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["FiveXLensePresent"]);
		MAINDllOBJECT->IsProbePresent(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["ProbePresent"]));
		ThreeDSoftwareMode = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["ThreedSoftware"]);
		AutoZoomInOutMode = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["AutoZoomInOut"]);
		PartialmageModeForOneShot = System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["OneShotPartialImageEnabled"]);
		//System::Windows::MessageBox::Show("8");

		MAINDllOBJECT->FullWindowDigitalZoom(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]->default["DoubleImageEnabled"]));
		MAINDllOBJECT->RefDot_Diameter(System::Convert::ToDouble(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["RefDot_Diameter"]));

		if (MAINDllOBJECT->IsProbePresent())
		{
			//gcnew RWrapper::RW_SphereCalibration();
			ThreeDSoftwareMode = true;
		}
		MAINDllOBJECT->DefaultCrosshairMode(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["DefaultFlexibleCrosshair"]));
		MAINDllOBJECT->HideGraphicsOnVideoSettingsMode(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["HideGraphicsOnVideo"]));
		MAINDllOBJECT->SaveCrossHairWithImage(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["SaveCrossHairWithImage"]));
		MAINDllOBJECT->TriangulateCpSurface(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["CloudPointsAsSurface"]));
		MAINDllOBJECT->ApplyLineArcSeperationMode(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["UseLineArcFitting"]));
		MAINDllOBJECT->OutlierFilteringForLineArc(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["OutlierFilteringForLineOrArc"]));
		//MAINDllOBJECT->TriangleStepLength(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["IgnorePtPercent"]));
		double ColorR = System::Convert::ToDouble(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["CrosshairColorR"]);
		double ColorG = System::Convert::ToDouble(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["CrosshairColorG"]);
		double ColorB = System::Convert::ToDouble(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["CrosshairColorB"]);
		MAINDllOBJECT->SetCrosshair_Color(ColorR/255.0,  ColorG/255.0,  ColorB/255.0);
		
		MAINDllOBJECT->OneTouchProbeAlignment = System::Convert::ToBoolean(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["UseProbeOneTouchAlignment"]);
		MAINDllOBJECT->MasterProbeRadius(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["MasterProbeDia"])/2);
		//ProbeStopValue = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeOffsetValue"]);
		//ProbeSpeed = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeSpeed"]);
		MAINDllOBJECT->ProbeRadius(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeDiameter"])/2);
		System::String^ ProbeType = System::Convert::ToString(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeType"]);		
		//System::Windows::MessageBox::Show("9");

		MAINDllOBJECT->UseGotoPointsforProbeHitPartProg(System::Convert::ToBoolean(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["UseGotoPointsforPobeHitPartProg"]));
		if(ProbeType == "NormalProbe")
		{
			MAINDllOBJECT->CurrentProbeType = RapidEnums::PROBETYPE::SIMPLE_PROBE;
		}
		else if(ProbeType == "TProbeAlongX")
		{
			MAINDllOBJECT->CurrentProbeType = RapidEnums::PROBETYPE::X_AXISTPROBE;
			MAINDllOBJECT->X_AxisProbeRadiusLeft(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongXLeftDia"])/2);
			MAINDllOBJECT->X_AxisProbeRadiusRight(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongXRightDia"])/2);
			MAINDllOBJECT->X_AxisProbeRadiusStrt(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongXStrtDia"])/2);
			MAINDllOBJECT->X_AxisProbeArmLengthX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongXOffsetX"]));
			MAINDllOBJECT->X_AxisProbeArmLengthY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongXOffsetY"]));
			MAINDllOBJECT->X_AxisProbeArmLengthZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongXOffsetZ"]));
			MAINDllOBJECT->StarStraightOffsetX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeXStrtOffsetX"]));
			MAINDllOBJECT->StarStraightOffsetY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeXStrtOffsetY"]));
			MAINDllOBJECT->StarStraightOffsetZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeXStrtOffsetZ"]));
		}
		else if(ProbeType == "TProbeAlongY")
		{
			MAINDllOBJECT->CurrentProbeType = RapidEnums::PROBETYPE::Y_AXISTPROBE;
			MAINDllOBJECT->Y_AxisProbeRadiusBottom(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongYBackDia"])/2);
			MAINDllOBJECT->Y_AxisProbeRadiusTop(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongYFrontDia"])/2);
			MAINDllOBJECT->X_AxisProbeRadiusStrt(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongYStrtDia"])/2);
			MAINDllOBJECT->StarStraightOffsetX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeStrtOffsetX"]));
			MAINDllOBJECT->StarStraightOffsetY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeStrtOffsetY"]));
			MAINDllOBJECT->StarStraightOffsetZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeStrtOffsetZ"]));
			MAINDllOBJECT->Y_AxisProbeArmLengthX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongYOffsetX"]));
			MAINDllOBJECT->Y_AxisProbeArmLengthY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongYOffsetY"]));
			MAINDllOBJECT->Y_AxisProbeArmLengthZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["TProbeAlongYOffsetZ"]));
		   
		}
		else if(ProbeType == "StarProbe")
		{
			MAINDllOBJECT->CurrentProbeType = RapidEnums::PROBETYPE::STARPROBE;
			MAINDllOBJECT->X_AxisProbeRadiusLeft(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeLeftDia"])/2);
			MAINDllOBJECT->X_AxisProbeRadiusRight(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeRightDia"])/2);
			MAINDllOBJECT->X_AxisProbeRadiusStrt(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeStrtDia"])/2);
			MAINDllOBJECT->Y_AxisProbeRadiusBottom(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeBackDia"])/2);
			MAINDllOBJECT->Y_AxisProbeRadiusTop(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeFrontDia"])/2);
			MAINDllOBJECT->StarProbeRightOffsetX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeRightOffsetX"]));
			MAINDllOBJECT->StarProbeRightOffsetY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeRightOffsetY"]));
			MAINDllOBJECT->StarProbeRightOffsetZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeRightOffsetZ"]));
			MAINDllOBJECT->StarProbeBackOffsetX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeBackOffsetX"]));
			MAINDllOBJECT->StarProbeBackOffsetY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeBackOffsetY"]));
			MAINDllOBJECT->StarProbeBackOffsetZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeBackOffsetZ"]));
			MAINDllOBJECT->StarProbeFrontOffsetX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeFrontOffsetX"]));
			MAINDllOBJECT->StarProbeFrontOffsetY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeFrontOffsetY"]));
			MAINDllOBJECT->StarProbeFrontOffsetZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeFrontOffsetZ"]));
			MAINDllOBJECT->StarStraightOffsetX(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeStrtOffsetX"]));
			MAINDllOBJECT->StarStraightOffsetY(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeStrtOffsetY"]));
			MAINDllOBJECT->StarStraightOffsetZ(System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["StarProbeStrtOffsetZ"]));
		}
		else if(ProbeType == "Generic Probe")
		{
			System::String^ CurrentGenericProbeType = System::Convert::ToString(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["CurrentGenericProbeType"]);		
			MAINDllOBJECT->CurrentProbeType = RapidEnums::PROBETYPE::GENERICPROBE;
			int id = 0, rowCount = GetValues->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + MachineNo + "' AND ProbType = '" + CurrentGenericProbeType + "'")->Length;
			double offsetx = 0, offsety = 0, offsetz = 0, radius = 0;
			for(int i = 0; i < rowCount; i++)
			{
				id = System::Convert::ToInt32(GetValues->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + MachineNo + "' AND ProbType = '" + CurrentGenericProbeType + "'")[i]->default["ID"]);
				offsetx = System::Convert::ToDouble(GetValues->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + MachineNo + "' AND ProbType = '" + CurrentGenericProbeType + "'")[i]->default["OffsetX"]);
				offsety = System::Convert::ToDouble(GetValues->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + MachineNo + "' AND ProbType = '" + CurrentGenericProbeType + "'")[i]->default["OffsetY"]);
				offsetz =  System::Convert::ToDouble(GetValues->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + MachineNo + "' AND ProbType = '" + CurrentGenericProbeType + "'")[i]->default["OffsetZ"]);
				radius =  System::Convert::ToDouble(GetValues->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + MachineNo + "' AND ProbType = '" + CurrentGenericProbeType + "'")[i]->default["Radius"]);
				MAINDllOBJECT->SetGenericProbeOffset(id, offsetx, offsety, offsetz, radius);
			}
		}
		MAINDllOBJECT->ProbeOffsetX = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeOffsetX"]);
		MAINDllOBJECT->ProbeOffsetY = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeOffsetY"]);
		MAINDllOBJECT->ProbeOffsetZ = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeOffsetZ"]);
		
		//MAINDllOBJECT->ProbeOffsetX = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["GageOffsetX"]);
		//MAINDllOBJECT->ProbeOffsetY = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["GageOffsetY"]);
		//MAINDllOBJECT->ProbeOffsetZ = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["GageOffsetZ"]);

		MAINDllOBJECT->DemoMode(System::Convert::ToBoolean(GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0]->default["DemoMode"]));
		System::String^ TempDrive = System::Convert::ToString(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SelectProgramDrive"]);

		//System::Windows::MessageBox::Show("10");

		MAINDllOBJECT->RotaryCalibrationIntercept(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["RotaryInterceptValue"]));
		MAINDllOBJECT->RotaryCalibrationAngleValue(System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["RotaryAngleDiff"]));
		
		if (GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["RotaryFixtureOffset"] == DBNull::Value)
			MAINDllOBJECT->RotaryFixtureOffset(0);
		else
			MAINDllOBJECT->RotaryFixtureOffset(System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["RotaryFixtureOffset"]));
		//System::Double radiff = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["RotaryAngleDiff"]);
		//System::Windows::MessageBox::Show(radiff.ToString());
		//System::Double raintercept = MAINDllOBJECT->RotaryCalibrationIntercept();
		//System::Windows::MessageBox::Show(raintercept.ToString());
		/*double anglevalue = 0, rangleval = 0;
		System::String^ RPosition = System::Convert::ToString(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["RotaryAngleDiff"]);
		array<System::String^>^ RPositionArray = RPosition->Split(System::Char(','));
		for(int i = 0; i < RPositionArray->Length; i++)
			anglevalue += (System::Math::Abs(System::Convert::ToDouble(RPositionArray[i])))/(System::Math::Pow(60, i));
		if(RPosition->Contains("-"))
			anglevalue = -anglevalue;
		rangleval = anglevalue* (System::Math::PI/180);
		MAINDllOBJECT->RotaryCalibrationAngleValue(rangleval);*/
		System::String^ UCSBasePlnLnRefAxis = System::Convert::ToString(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["UCSBasePlnLnRefAxis"]);
		if(UCSBasePlnLnRefAxis == "XAxis")
		{
			 MAINDllOBJECT->UCSSelectedLineDirAlongX(true);
		}
		else
		{
			 MAINDllOBJECT->UCSSelectedLineDirAlongX(false);
		}
		if(InitialCall)
		{
			SpeedX = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedX"]);
			SpeedY = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedY"]);
			SpeedZ = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedZ"]);
			SpeedR = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedR"]);
		}
		SpeedMinValue = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["MinimumValue"]);
		SpeedMaxValue = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["MaximumValue"]);
		SpeedStepValue = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["StepValue"]);
		SpeedStepValue = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["StepValue"]);
		LangaugeType = System::Convert::ToString(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["LanguageType"]);
		DefaultMagnificationLevel = System::Convert::ToString(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["DefaultMagLevel"]);
		LangCid = System::Convert::ToInt32(GetValues->Select_ChildControls("LanguageTypeLookup","LanguageType", LangaugeType)[0]->default["ColumnId"]);

		FOCUSCALCOBJECT->ApplyFocusCorrection(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ApplyFocusCorrection"]));
		FOCUSCALCOBJECT->AutoAdjustLighting(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["AutoAdjustLighting"]));
		FOCUSCALCOBJECT->CalRow(System::Convert::ToInt32(GetValues->Select_ChildControls("MeasurementSettings","MachineNo", MachineNo)[0]->default["NumberOfRows_FocusCalib"]));
		FOCUSCALCOBJECT->CalCol(System::Convert::ToInt32(GetValues->Select_ChildControls("MeasurementSettings","MachineNo", MachineNo)[0]->default["NumberOfColumns_FocusCalib"])); 
		FOCUSCALCOBJECT->CalWidth(System::Convert::ToInt32(GetValues->Select_ChildControls("MeasurementSettings","MachineNo", MachineNo)[0]->default["CellWidth_FocusCalib"]));
		FOCUSCALCOBJECT->ApplyPixelCorrection(System::Convert::ToBoolean(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["ApplyPixelAveraging"]));
		FOCUSCALCOBJECT->PixelCorrectionFactor(System::Convert::ToInt32(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["PixelAveragingValue"]));
		
		//System::Windows::MessageBox::Show("11");

		
		bool Presentflag = false;
		cli::array<System::IO::DriveInfo^,1>^ DirectoryList =  System::IO::DriveInfo::GetDrives();
		for each(System::IO::DriveInfo^ D in DirectoryList)
		{
			if(D->Name == TempDrive)
			{
				Presentflag = true;
				break;
			}
		}
		if(Presentflag)
		{
			ProgramPath = TempDrive + "Programs";
			ReportPath = TempDrive + "Reports";
			TemplatesPath = TempDrive + "Templates";
		}
		else
		{
			ProgramPath = "C:\\Programs";
			ReportPath = "C:\\Reports";
			TemplatesPath = "C:\\Templates";
		}

		RWrapper::RW_FixtureCalibration::MYINSTANCE()->CurrentFixureName = System::Convert::ToString(GetValues->Select_ChildControls("UserSettings","MachineNo", MachineNo)[0]->default["DefaultFixtureName"]);
		cli::array<System::Data::DataRow^>^ FixureRows = GetValues->Select_ChildControls("FixtureDetails","MachineNo", MachineNo);
		for(int i = 0 ;i < FixureRows->Length; i++)
		{
			System::String^ CheckString = System::Convert::ToString(FixureRows[i]->default["FixtureName"]);
			if(CheckString == RWrapper::RW_FixtureCalibration::MYINSTANCE()->CurrentFixureName)
			{
				RWrapper::RW_FixtureCalibration::MYINSTANCE()->UpdateFixureDetails_Frontend(CheckString, 
					System::Convert::ToInt32(GetValues->Select_ChildControls("FixtureDetails","MachineNo", MachineNo)[i]->default["No_of_Rows"]),
					System::Convert::ToInt32(GetValues->Select_ChildControls("FixtureDetails","MachineNo", MachineNo)[i]->default["No_of_Columns"]),
					System::Convert::ToDouble(GetValues->Select_ChildControls("FixtureDetails","MachineNo", MachineNo)[i]->default["RowGap"]),
					System::Convert::ToDouble(GetValues->Select_ChildControls("FixtureDetails","MachineNo", MachineNo)[i]->default["ColumnGap"]));
			}
		}
		if(!InitialCall)
			MAINDllOBJECT->CalculateCloudPtZlevelExtent();
		//System::Windows::MessageBox::Show("12");

		//We now read the default camera setttings for both profile light and surface light. 
		//If this machine is normal machine with 2M camera, we set the hard coded default values - 
		MAINDllOBJECT->SurfaceCamSettings_default = "8080808001004C";
		MAINDllOBJECT->ProfileCamSettings_default = "58FF008001004C";
		MAINDllOBJECT->RCam3_D_GainSettings = "0002020202";
		MAINDllOBJECT->RefDotCamSettings = MAINDllOBJECT->ProfileCamSettings_default;
		//Now let us see if we have any entries for this machine type - 
		cli::array<System::Data::DataRow^>^ CamSettingsValues = GetValues->Select_ChildControls("DefaultCamSettings", "MachineNo", MachineNo);
		cli::array<System::Data::DataRow^>^ CamSettingsRCam3 = GetValues->Select_ChildControls("DefaultCamSettings", "MachineNo", "RC3");
		const char* temporarychar;
		if (System::Environment::Is64BitOperatingSystem)
		{
			MAINDllOBJECT->SurfaceCamSettings_default = (const char*)(Marshal::StringToHGlobalAnsi(CamSettingsRCam3[0]["SurfaceLightSettings"]->ToString())).ToPointer();
			MAINDllOBJECT->ProfileCamSettings_default = (const char*)(Marshal::StringToHGlobalAnsi(CamSettingsRCam3[0]["ProfileLightSettings"]->ToString())).ToPointer();
		}
		if (CamSettingsValues->Length > 0)
		{
			System::Data::DataRow^ latestValue = CamSettingsValues[CamSettingsValues->Length - 1];
			if (System::Environment::Is64BitOperatingSystem && latestValue["SurfaceLightSettings"]->ToString()->Length < 20)
				temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(CamSettingsRCam3[0]["SurfaceLightSettings"]->ToString())).ToPointer();
			else
				temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(latestValue["SurfaceLightSettings"]->ToString())).ToPointer();
			MAINDllOBJECT->SurfaceCamSettings_default = temporarychar;

			if (System::Environment::Is64BitOperatingSystem && latestValue["ProfileLightSettings"]->ToString()->Length < 20)
				temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(CamSettingsRCam3[0]["ProfileLightSettings"]->ToString())).ToPointer();
			else
				temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(latestValue["ProfileLightSettings"]->ToString())).ToPointer();
			MAINDllOBJECT->ProfileCamSettings_default = temporarychar;

			temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(latestValue["DigitalGain_RCam3"]->ToString())).ToPointer();
			if (temporarychar != "")	
				MAINDllOBJECT->RCam3_D_GainSettings = temporarychar;
			System::String^ tempValue = latestValue["RefDotCamSettings"]->ToString()->Trim();

			temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(tempValue)).ToPointer();
			if (temporarychar != "")
				MAINDllOBJECT->RefDotCamSettings = temporarychar;
		}
		//System::Windows::MessageBox::Show("13");
		//Setup the 11X Profile light intensity level for main profile light...
		RWrapper::RW_DBSettings::MYINSTANCE()->ProfileLightLevelFor11X = 1;
		RWrapper::RW_DBSettings::MYINSTANCE()->ProfileLightLevelFor11X = System::Convert::ToInt32(GetValues->Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]->default["ProfileLightLevelFor11X"]);

		//Read Cross-Hair offset values and set them up
		GetValues->FillTable("CrossHairOffsets");
		cli::array<System::Data::DataRow^>^ XHairOffsetValues = GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo);
		int ccct = 0;
		if (XHairOffsetValues->Length > 0)
		{
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Left_Normal_X"]);
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Left_Normal_Y"]);
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Top_Normal_X"]);
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Top_Normal_Y"]);
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Right_Normal_X"]);
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Right_Normal_Y"]);
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Bottom_Normal_X"]);
			MAINDllOBJECT->Offset_Normal[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Bottom_Normal_Y"]);

			ccct = 0;
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Left_Skewed_X"]);
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Left_Skewed_Y"]);
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Top_Skewed_X"]);
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Top_Skewed_Y"]);
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Right_Skewed_X"]);
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Right_Skewed_Y"]);
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Bottom_Skewed_X"]);
			MAINDllOBJECT->Offset_Skewed[ccct++] = System::Convert::ToInt32(GetValues->Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo)[0]->default["Bottom_Skewed_Y"]);
		}


		//Finilize the database reading module...//
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0011", ex);
	}
	GetValues->FinalizeClass();
}

void RWrapper::RW_DBSettings::UpdateProbeApproachGap_Speed(double ApproachGap, double Speed)
{
	try
	{
		ProbeStopValue = ApproachGap;
		ProbeSpeed = Speed;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0011b", ex); }    
}


void RWrapper::RW_DBSettings::DeleteOldCalibrationSettings()
{
	try
	{
		if(con->State == ConnectionState::Closed)
			this->con->Open();
		this->cmd = gcnew System::Data::OleDb::OleDbCommand("DELETE * FROM CalibrationSettings Where MachineNo='" + System::Convert::ToString(CurrentMachineNumber) + "'", this->con);
		this->cmd->ExecuteNonQuery();
		this->con->Close();
	}
	catch(Exception^ ex){ this->con->Close(); RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0012", ex); }  
}

void RWrapper::RW_DBSettings::DeleteOldFocusCalibrationSettings()
{
	try
	{
		if(con->State == ConnectionState::Closed)
			this->con->Open();
		this->cmd = gcnew System::Data::OleDb::OleDbCommand("DELETE * FROM FocusScanCalibration Where MachineNo='" + System::Convert::ToString(CurrentMachineNumber) + "'", this->con);
		this->cmd->ExecuteNonQuery();
		this->con->Close();
	}
	catch(Exception^ ex){ this->con->Close(); RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0013", ex); }
}

void RWrapper::RW_DBSettings::UpdateCalibrationSettings_CurrentMachine(System::Collections::ArrayList^ MeasureValues, int TotalCell)
{
	CalibrationModule::DB^ DB_settings = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		DeleteOldCalibrationSettings();
		DB_settings->FillTable("CalibrationSettings");
		cli::array<System::Data::DataRow^>^ Settings_Rows = gcnew cli::array<System::Data::DataRow^>(TotalCell);
		for(int i = 0; i < TotalCell; i++)
			Settings_Rows[i] = DB_settings->GetTable("CalibrationSettings")->NewRow();
		
		for(int i = 0; i < TotalCell; i++)
		{
			Settings_Rows[i]->default["MachineNo"] = MeasureValues[8 * i];
			Settings_Rows[i]->default["MachineType_Size"] = MeasureValues[8 * i + 1];
			Settings_Rows[i]->default["SlipGaugeLength"] = MeasureValues[8 * i + 2];
			Settings_Rows[i]->default["NextCellGap"] = MeasureValues[8 * i + 3];
			Settings_Rows[i]->default["RowCount"] = MeasureValues[8 * i + 4];
			Settings_Rows[i]->default["ColumnCount"] = MeasureValues[8 * i + 5];
			Settings_Rows[i]->default["Correction_X"] = MeasureValues[8 * i + 6];
			Settings_Rows[i]->default["Correction_Y"] = MeasureValues[8 * i + 7];
		}
		for(int i = 0; i < TotalCell; i++)
			DB_settings->GetTable("CalibrationSettings")->Rows->Add(Settings_Rows[i]);
		DB_settings->Update_Table("CalibrationSettings");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0014", ex); }
	DB_settings->FinalizeClass();
}

void RWrapper::RW_DBSettings::UpdateSphereCalibrationSettings_CurrentMachine(int probetype, int CurrentSide, double ProbeRadius, double *CalibratedOffset)
{
	CalibrationModule::DB^ DB_settings = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		DeleteOldFocusCalibrationSettings();
		DB_settings->FillTable("ProbeSettings");
		int cnt = DB_settings->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + CurrentMachineNumber + "'")->Length;
		if(cnt > 0)
		{
			System::Data::DataRow^ Settings_Rows = DB_settings->Select_ChildControlsMulti("ProbeSettings", "MachineNo = '" + CurrentMachineNumber + "'")[0];
			switch(probetype)
			{
				case 1:
				{
					Settings_Rows->default["ProbeDiameter"] = ProbeRadius;
					break;
				}
				case 2:
				{
					if(CurrentSide == 1)
						Settings_Rows->default["TProbeAlongXLeftDia"] = ProbeRadius;
					else if (CurrentSide == 2)
					{
						Settings_Rows->default["TProbeAlongXRightDia"] = ProbeRadius;
						Settings_Rows->default["TProbeAlongXOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["TProbeAlongXOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["TProbeAlongXOffsetZ"] = CalibratedOffset[2];
					}
					else
					{
						Settings_Rows->default["TProbeAlongXStrtDia"] = ProbeRadius;
						Settings_Rows->default["TProbeXStrtOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["TProbeXStrtOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["TProbeXStrtOffsetZ"] = CalibratedOffset[2];

					}
					break;
				}
				case 3:
				{
					if(CurrentSide == 3)
						Settings_Rows->default["TProbeAlongYFrontDia"] = ProbeRadius;
					else if (CurrentSide == 4)
					{
						Settings_Rows->default["TProbeAlongYBackDia"] = ProbeRadius;
						Settings_Rows->default["TProbeAlongYOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["TProbeAlongYOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["TProbeAlongYOffsetZ"] = CalibratedOffset[2];
					}
					else
					{
						Settings_Rows->default["TProbeAlongYStrtDia"] = ProbeRadius;
						Settings_Rows->default["TProbeStrtOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["TProbeStrtOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["TProbeStrtOffsetZ"] = CalibratedOffset[2];
					}
					break;
				}
				case 4:
				{
					Settings_Rows->default["ProbeDiameter"] = ProbeRadius;
					if(CurrentSide == 1)
						Settings_Rows->default["StarProbeLeftDia"] = ProbeRadius;
					else if(CurrentSide == 2)
					{
						Settings_Rows->default["StarProbeRightDia"] = ProbeRadius;
						Settings_Rows->default["StarProbeRightOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["StarProbeRightOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["StarProbeRightOffsetZ"] = CalibratedOffset[2];
					}
					else if(CurrentSide == 3)
					{
						Settings_Rows->default["StarProbeFrontDia"] = ProbeRadius;
						Settings_Rows->default["StarProbeFrontOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["StarProbeFrontOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["StarProbeFrontOffsetZ"] = CalibratedOffset[2];
					}
					else if(CurrentSide == 4)
					{
						Settings_Rows->default["StarProbeBackDia"] = ProbeRadius;
						Settings_Rows->default["StarProbeBackOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["StarProbeBackOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["StarProbeBackOffsetZ"] = CalibratedOffset[2];
					}
					else // if (CurrentSide == 5)
					{
						Settings_Rows->default["StarProbeStrtDia"] = ProbeRadius;
						Settings_Rows->default["StarProbeStrtOffsetX"] = CalibratedOffset[0];
						Settings_Rows->default["StarProbeStrtOffsetY"] = CalibratedOffset[1];
						Settings_Rows->default["StarProbeStrtOffsetZ"] = CalibratedOffset[2];
					}

					break;
				}
			}
			DB_settings->Update_Table("ProbeSettings");
		}
		this->GetValuesFromDatabase(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0015", ex); }
	DB_settings->FinalizeClass();
}

void RWrapper::RW_DBSettings::UpdateFocusCalibrationSettings_CurrentMachine(System::Collections::ArrayList^ MeasureValues, int TotalCell)
{
	CalibrationModule::DB^ DB_settings = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		DeleteOldFocusCalibrationSettings();
		DB_settings->FillTable("FocusScanCalibration");
		cli::array<System::Data::DataRow^>^ Settings_Rows = gcnew cli::array<System::Data::DataRow^>(TotalCell);
		for(int i = 0; i < TotalCell; i++)
			Settings_Rows[i] = DB_settings->GetTable("FocusScanCalibration")->NewRow();
		
		for(int i = 0; i < TotalCell; i++)
		{
			Settings_Rows[i]->default["MachineNo"] = MeasureValues[6 * i];
			Settings_Rows[i]->default["CellLeftX"] = MeasureValues[6 * i + 1];
			Settings_Rows[i]->default["CellLeftY"] = MeasureValues[6 * i + 2];
			Settings_Rows[i]->default["CellWidth"] = MeasureValues[6 * i + 3];
			Settings_Rows[i]->default["CellHeight"] = MeasureValues[6 * i + 4];
			Settings_Rows[i]->default["Correction_Z"] = MeasureValues[6 * i + 5];
		}
		for(int i = 0; i < TotalCell; i++)
			DB_settings->GetTable("FocusScanCalibration")->Rows->Add(Settings_Rows[i]);
		DB_settings->Update_Table("FocusScanCalibration");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0015", ex); }
	DB_settings->FinalizeClass();
}

void RWrapper::RW_DBSettings::UpdateFocusCalibrationGridSettings_CurrentMachine(System::Collections::ArrayList^ CorrectionValues)
{
	CalibrationModule::DB^ DB_settings = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		DB_settings->FillTable("MeasurementSettings");
		System::Data::DataRow^ Measure_Row = DB_settings->Select_ChildControls("MeasurementSettings","MachineNo", CurrentMachineNumber)[0];
		Measure_Row->default["NumberOfRows_FocusCalib"] = CorrectionValues[0];
		Measure_Row->default["NumberOfColumns_FocusCalib"] = CorrectionValues[1];
		Measure_Row->default["CellWidth_FocusCalib"] = CorrectionValues[2];
		Measure_Row->default["CellHeight_FocusCalib"] = CorrectionValues[3];
		DB_settings->Update_Table("MeasurementSettings");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0015", ex); }
	DB_settings->FinalizeClass();
}

void RWrapper::RW_DBSettings::UpdateCrosshairOffsets()
{
	CalibrationModule::DB^ DB_settings = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		DB_settings->FillTable("CrossHairOffsets");
		cli::array<System::Data::DataRow^>^ XHairOffsetValues = DB_settings->Select_ChildControls("CrossHairOffsets", "MachineNo", CurrentMachineNumber);
		System::Data::DataRow^ thisRow;
		if (XHairOffsetValues->Length == 0)
			thisRow = DB_settings->GetTable("CrossHairOffsets")->NewRow();
		else
			thisRow = XHairOffsetValues[XHairOffsetValues->Length - 1];
		thisRow["MachineNo"] = CurrentMachineNumber;
		int index = 2;
		//Load the current NOrmal offset values
		for (int j = 0; j < 8; j++)
			thisRow[index++] = MAINDllOBJECT->Offset_Normal[j];
		//Load skewed offset values
		for (int j = 0; j < 8; j++)
			thisRow[index++] = MAINDllOBJECT->Offset_Skewed[j];

		if (XHairOffsetValues->Length == 0)
			DB_settings->GetTable("CrossHairOffsets")->Rows->Add(thisRow);

		DB_settings->Update_Table("CrossHairOffsets");
	}
	catch (Exception^ ex) { RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0015xh", ex); }
	DB_settings->FinalizeClass();

}

void RWrapper::RW_DBSettings::UpdateCalibrationSettings()
{
	try
	{
		System::String^ LastMachineNo = CurrentMachineNumber;
		RapidEnums::RAPIDMACHINETYPE PreviousType = MAINDllOBJECT->CurrentMahcineType;
		System::String^ PreviousCtype = Camera_Type;
		int PLangId = LangCid;
		GetValuesFromDatabase(false);
		RWrapper::RW_DRO::MYINSTANCE()->GetDROSettingsValues(false);
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::PROBE_ONLY) return;
		UpdateSurfaceFGSettings();
		RWrapper::RW_DRO::MYINSTANCE()->GetLocalisedCalibrationDetails();
		RWrapper::RW_DRO::MYINSTANCE()->GetFocusCalibrationDetails();
		//System::Windows::MessageBox::Show(CurrentMachineNumber);

		if(LastMachineNo != CurrentMachineNumber)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsUpdateCalibrationSettings01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(PreviousType != MAINDllOBJECT->CurrentMahcineType && (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT && PreviousType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT))
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsUpdateCalibrationSettings02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(PreviousCtype != Camera_Type)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsUpdateCalibrationSettings03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(PLangId != LangCid)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsUpdateCalibrationSettings04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		System::String^ Str = RWrapper::RW_MainInterface::MYINSTANCE()->LastSelectedMagLevel;
		if(MagNameList->Contains(Str))
		{
			UpdateVideoPixelWidth(Str);
		}
		else
		{
			//MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsUpdateCalibrationSettings05", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		}
		MAINDllOBJECT->MeasurementNoOfDecimal(RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec);
		MAINDllOBJECT->RecalculateAndUpdateShapes(RapidEnums::SHAPETYPE::CLOUDPOINTS);
		MAINDllOBJECT->SetImageSize_ED();
		MAINDllOBJECT->Shape_Updated(false);
		MAINDllOBJECT->UpdateAllMeasurements();
		if(MAINDllOBJECT->UseLocalisedCorrection && MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
		{
			MAINDllOBJECT->SetStatusCode("RW_DBSettings01");
			MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsUpdateCalibrationSettings06", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0015", ex); }
}

void RWrapper::RW_DBSettings::UpdateSurfaceFGSettings()
{
	CalibrationModule::DB^ GetValues = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER || EDSCOBJECT == NULL) return;
		GetValues->FillTable("SurfaceFGSettings");
		GetValues->FillTable("OneShotParams");
		GetValues->FillTable("VisionSettings");

		if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HOBCHECKER || EDSCOBJECT != NULL) 
		{	
			EDSCOBJECT->Type1_LookAheadVal = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T1LookAheadValue"]);
			EDSCOBJECT->Type1_BlackTol = System::Convert::ToDouble(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T1BlackTolerance"]);
			EDSCOBJECT->Type1_WhiteTol = System::Convert::ToDouble(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T1WhiteTolerance"]);
			EDSCOBJECT->Type1_FilterMaskTol = System::Convert::ToDouble(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T1FilterMaskTolerance"]);
			EDSCOBJECT->Type1_FilterMaskTolB2W = System::Convert::ToDouble(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T1FilterMaskToleranceB2W"]);
			EDSCOBJECT->Type2_LookAheadVal = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T2LookAheadValue"]);
			EDSCOBJECT->Type2_CurrAbsThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T2CurrAbsThreshold"]);
			EDSCOBJECT->Type2_CurrDiffThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T2CurrDiffThreshold"]);
			EDSCOBJECT->Type2_LookaheadAbsThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T2LookaheadAbsThreshold"]);
			EDSCOBJECT->Type2_LookaheadDiffThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T2LookaheahDiffThreshold"]);
			EDSCOBJECT->Type2_MedianFilterBxSize = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T2MedianFilterBoxSize"]);
			EDSCOBJECT->Type2_AvgFilterBxSize = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T2AverageFilterBoxSize"]);
			EDSCOBJECT->Type3_LookAheadVal = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T3LookAheadValue"]);
			EDSCOBJECT->Type3_BlackTol = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T3BlackTolerance"]);
			EDSCOBJECT->Type3_WhiteTol = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T3WhiteTolerance"]);
			EDSCOBJECT->Type3_FilterMaskTol = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T3FilterMaskTolerance"]);
			EDSCOBJECT->Type3_FilterMaskTolB2W = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T3FilterMaskToleranceB2W"]);
			EDSCOBJECT->Type4_LookAheadVal = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T4LookAheadValue"]);
			EDSCOBJECT->Type4_TextureDiffThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T4TextureDiffThreshold"]);
			EDSCOBJECT->Type4_AcrossEdgeIncr = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T4AcrossEdgeIncrement"]);
			EDSCOBJECT->Type4_AlongEdgeIncr = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T4AlongEdgeIncrement"]);
			EDSCOBJECT->Type4_AcrossEdgeBxThickness = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T4AcrossEdgeBoxThickness"]);
			EDSCOBJECT->Type4_AlongEdgeBxThickness = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T4AlongEdgeBoxThickness"]);
			EDSCOBJECT->Type5_LocalMaximaExtent = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T5LocalMaximaExtent"]);
			EDSCOBJECT->Type5_DiffThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T5DiffThreshold"]);
			EDSCOBJECT->Type5_FilterMaskSize = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T5FilterMaskSize"]);
			EDSCOBJECT->Type6_MaskFactor = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6MaskFactor"]);
			EDSCOBJECT->Type6_Binary_LowerFactor = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6LowerBinaryFactor"]);
			EDSCOBJECT->Type6_Binary_UpperFactor = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6UpperBinaryFactor"]);
			EDSCOBJECT->Type6_PixelThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6PixelThreshold"]);
			EDSCOBJECT->Type6_MaskingLevel = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6MaskingLevel"]);
			EDSCOBJECT->Type6_ScanRange = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6PixelScanRange"]);
			EDSCOBJECT->Pixel_Neighbour = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6PixelNeighbour"]);
			EDSCOBJECT->ApplySubAlgorithmOfType6 = System::Convert::ToBoolean(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6ApplySubAlgo"]);
			EDSCOBJECT->Type6_NoiseFilterDistInPixel = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6CutoffPixels"]);
			EDSCOBJECT->ApplyDirectionalScan = System::Convert::ToBoolean(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6ApplyDirectionalScan"]);
			EDSCOBJECT->Type6JumpThreshold = System::Convert::ToInt32(GetValues->Select_ChildControls("SurfaceFGSettings","MachineNo", CurrentMachineNumber)[0]->default["T6JumpThreshold"]);
			EDSCOBJECT->CorrectforBeamDivergence = false;
			this->RefDotDia_in_mm = System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings", "MachineNo", CurrentMachineNumber)[0]->default["RefDotDia"]);

			//if (MAINDllOBJECT-> ==  MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			cli::array<System::Data::DataRow^>^ OneShotParamList = GetValues->Select_ChildControls("OneShotParams", "MachineNo", CurrentMachineNumber);
			if (OneShotParamList->Length > 0)
			{
				double bdf[6] = { 0 };
				for (int i = 0; i < 6; i++)
				{
					EDSCOBJECT->bd_factor[i] = System::Convert::ToDouble(OneShotParamList[0]->default[i + 9]); //
				}
				EDSCOBJECT->CorrectforBeamDivergence = System::Convert::ToBoolean(OneShotParamList[0]->default["EnableBD"]);
				MAINDllOBJECT->PixelWidthCorrectionQuadraticTerm(System::Convert::ToDouble(OneShotParamList[0]->default[3]));
				MAINDllOBJECT->PixelWidthCorrectionLinearTermX(System::Convert::ToDouble(OneShotParamList[0]->default[4]));
				MAINDllOBJECT->PixelWidthCorrectionLinearTermY(System::Convert::ToDouble(OneShotParamList[0]->default[5]));
				MAINDllOBJECT->PW_C0_Rad(System::Convert::ToDouble(OneShotParamList[0]->default[6]));
				MAINDllOBJECT->PixelWidthX0(System::Convert::ToDouble(OneShotParamList[0]->default[7]));
				MAINDllOBJECT->PixelWidthY0(System::Convert::ToDouble(OneShotParamList[0]->default[8]));
				RWrapper::RW_PartProgram::MYINSTANCE()->SingleComponentinOneShot = System::Convert::ToBoolean(OneShotParamList[0]->default["SingleComponentinOneShot"]);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0015b", ex); }
	GetValues->FinalizeClass();
}

void RWrapper::RW_DBSettings::UpdateGradeSelection(int Index)
{
	if(Index == 0)
	{
		GradeValueCollection = Grade1ValueCollection;
		GradeValueEndCollection = Grade1ValueEndCollection;
		GradeLevelCollection = Grade1LevelCollection;
	}
	else
	{
		GradeValueCollection = Grade2ValueCollection;
		GradeValueEndCollection = Grade2ValueEndCollection;
		GradeLevelCollection = Grade2LevelCollection;
	}
	bool FirstTime = true;
	for each(System::Collections::DictionaryEntry GradeValue in GradeValueCollection)
	{
		if(FirstTime)
		{
			FirstGradeValue = System::Convert::ToDouble(GradeValue.Value);
			FirstTime = false;
		}
		else
		{
			if(System::Convert::ToDouble(GradeValue.Value) < FirstGradeValue)
				FirstGradeValue = System::Convert::ToDouble(GradeValue.Value);
		}
	}

	FirstTime = true;
	for each(System::Collections::DictionaryEntry GradeValue in GradeValueEndCollection)
	{
		if(FirstTime)
		{
			LastGradevalue = System::Convert::ToDouble(GradeValue.Value);
			FirstTime = false;
		}
		else
		{
			if(System::Convert::ToDouble(GradeValue.Value) > LastGradevalue)
				LastGradevalue = System::Convert::ToDouble(GradeValue.Value);
		}
	}
	/*FirstGradeValue = System::Convert::ToDouble(GradeValueCollection[1]); 
	double ScndValue = System::Convert::ToDouble(GradeValueCollection[2]); 
	LastGradevalue = System::Convert::ToDouble(GradeValueCollection[GradeValueCollection->Count]);
	GradeStep = ScndValue - FirstGradeValue;*/
}

//Finilise the Rcadsettings.... close the data base connection..//
void RWrapper::RW_DBSettings::FinaliseAll()
{
	try
	{
		 this->con->Close();	
		 MAINDllOBJECT->AddingUCSModeFlag = false;
		 if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE && MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::PROBE_ONLY)
		 {
			 MAINDllOBJECT->Draw_VideoWindowGraphics();
		 }
		 MAINDllOBJECT->Draw_DxfWindowGraphics();
		 MAINDllOBJECT->Draw_RcadWindowGraphics();
		 FOCUSCALCOBJECT->StartFocusRelatedThrread();
		 System::IO::FileInfo^ Test = gcnew System::IO::FileInfo(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath, "\\ActionSave.rppx"));
		 if(Test->Exists)
			 this->MyTimer->Enabled = true;
		 const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
		 std::string FilePath = temporarychar;
		 FilePath = FilePath + "\\Database";
		 wifstream ifile;			
				ifile.open(FilePath + "\\FocusDelay.txt");	
		 if(!ifile.fail())
		 { 
			int delayinms = 0;
			ifile >> delayinms;
			if(delayinms > 0)
				FOCUSCALCOBJECT->StartFocusDelayThrread(delayinms);
		 }
		 ifile.close();
		 if(MAINDllOBJECT->UseLocalisedCorrection && MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
		 {
			 MAINDllOBJECT->SetStatusCode("RW_DBSettings01");
			 MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsFinaliseAll01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		 }
		/* if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE && RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus)
			 RWrapper::RW_AutoCalibration::MYINSTANCE()->Handle_HomeClicked(true, true, true);*/
		 /*if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::TIS)
		 {
			 MAINDllOBJECT->SetStatusCode("RW_DBSettings02");
			 MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsFinaliseAll02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		 }*/
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0016", ex); }
}

void RWrapper::RW_DBSettings::SetCameraToCompleteZoom_Exeload()
{
	try
	{
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		{
			ElevenXProfilePresent = System::Convert::ToBoolean(ElvnXProfileMode[RWrapper::RW_MainInterface::MYINSTANCE()->LastSelectedMagLevel]);
			RWrapper::RW_MainInterface::MYINSTANCE()->SetCameraToCompleteZoom_Exeload();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0016b", ex); }
}

void RWrapper::RW_DBSettings::RestoreTheCrash(System::Object^ sender, System::EventArgs^ e)
{
	try
	{
		 this->MyTimer->Enabled = false;
		 System::IO::FileInfo^ Test = gcnew System::IO::FileInfo(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath, "\\ActionSave.rppx"));
		 if(Test->Exists)
		 {
			 if(MAINDllOBJECT->ShowMsgBoxString("RW_DBSettingsRestoreTheCrash01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
			 {
				 PPCALCOBJECT->Editpp((char*)(void*)Marshal::StringToHGlobalAnsi(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath, "\\ActionSave.rppx")).ToPointer());
				 double MCSPosition[4] = {PPCALCOBJECT->SavedCurrentMachineDRO.getX(), PPCALCOBJECT->SavedCurrentMachineDRO.getY(), PPCALCOBJECT->SavedCurrentMachineDRO.getZ(), PPCALCOBJECT->SavedCurrentMachineDRO.getR()};
				 DROInstance->setMCSPosition(&MCSPosition[0]);
			 }
		 }
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWRDB0017", ex); }
}
