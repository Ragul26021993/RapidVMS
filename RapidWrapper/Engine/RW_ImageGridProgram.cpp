//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Actions\AddImageAction.h"

///Default constructor.. Initialise the deafault settings..
RWrapper::RW_ImageGridProgram::RW_ImageGridProgram()
{
	 try
	 {
		 this->ImageCalInstance = this;
		 this->TotalRows = 0, this->TotalColumns = 0; this->CellCount = 0, this->listCount = 0;
		 this->CellPosition = gcnew System::Collections::Generic::List<System::Double>();
		 this->ActionIdList = gcnew System::Collections::Generic::List<System::Int32>();
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWImg0001", ex); }
}

//Destructor.. Release the memory..
RWrapper::RW_ImageGridProgram::~RW_ImageGridProgram()
{
	try
	{
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_ImageGridProgram^ RWrapper::RW_ImageGridProgram::MYINSTANCE()
{
	return ImageCalInstance;
}

//create list for image path.....
void RWrapper::RW_ImageGridProgram::CreatePathListForImageAction(int NoOfRows, int NoOfColumn, double RowGap, double ColumnGap)
{
	 try
	 {
		 this->ClearAll();
		this->TotalRows = NoOfRows; this->TotalColumns = NoOfColumn;
		this->CellPosition->Clear();
		this->ActionIdList->Clear();
		double target[2] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1]};
		if(this->TotalColumns > 0 && this->TotalRows > 0)
		{
			for(int i = 0; i < TotalRows; i++)
			{				
				if(i > 0)
				{
					target[1] = target[1] - RowGap;	
				}
				for(int j = 0; j < TotalColumns; j++)
				{
					if(j > 0 || i > 0)
					{
						if(j > 0)
							target[0] = target[0] + ColumnGap;
						else
							target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
						CellPosition->Add(target[0]);
						CellPosition->Add(target[1]);		
					}
				}
			}
			this->Goto_NextCellPosition(false);
		}
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWImg0002", ex); }
}

void RWrapper::RW_ImageGridProgram::Continue_PauseImageAction(bool ContinueFlag)
{
	 try
	 {
		if(ContinueFlag)
			this->Goto_NextCellPosition(true);
		else
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWImg0003", ex); }
}

void RWrapper::RW_ImageGridProgram::StopImageAction()
{
	 try
	 {
		 bool deleteAction = false;
		//delete all image action....
		for each(int ActionId in this->ActionIdList)
		{
			if(MAINDllOBJECT->getActionsHistoryList().ItemExists(ActionId))
			{
				BaseItem* ActionItem = MAINDllOBJECT->getActionsHistoryList().getList()[ActionId];
				MAINDllOBJECT->getActionsHistoryList().removeItem(ActionItem, true, true);
				deleteAction = true;
			}
		}
		if(deleteAction)
			MAINDllOBJECT->ImageAction_Count = 0;
		 RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		 this->ClearAll();
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWImg0004", ex); }
}

void RWrapper::RW_ImageGridProgram::ClearAll()
{
	try
	{
		 this->TotalRows = 0, this->TotalColumns = 0; this->CellCount = 0, this->listCount = 0;
		 this->ActionIdList->Clear();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWImg0005", ex); }
}

void RWrapper::RW_ImageGridProgram::Goto_NextCellPosition(bool ContinueFlag)
{
	try
	{
		if (ContinueFlag && this->CellCount != 0)
		{
			this->CellCount--;
			this->listCount -= 2;
			if (CellCount < TotalColumns * TotalRows - 1)
			{
				double feedrate[4] = { RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3] };
				double Target[4] = { CellPosition[listCount++], CellPosition[listCount++], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
				this->CellCount++;
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&Target[0], &feedrate[0], TargetReachedCallback::IMAGEGRID_PROGRAM);
			}
		}
		else
		{
			Sleep(50);
			if (MAINDllOBJECT->CameraConnected())
			{
				RWrapper::RW_DRO::MYINSTANCE()->RaiseCameraClickSoundEvent();
				MAINDllOBJECT->NotedDownImagePickPoint();
				RC_ITER item = MAINDllOBJECT->getActionsHistoryList().getList().end();
				item--;
				RAction* MyAction = (RAction*)((*item).second);
				ActionIdList->Add(MyAction->getId());
				int Image_Count = ((AddImageAction*)MyAction)->ImageCount;
				//System::String^ filePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "//Image//" + System::Convert::ToString(Image_Count) + ".jpg";
				RWrapper::RW_MainInterface::MYINSTANCE()->SaveVideoWindowImage(this->currentStoragePath + "\\" + System::Convert::ToString(Image_Count) + ".jpg", false, false);
				if (CellCount < TotalColumns * TotalRows - 1)
				{
					double feedrate[4] = { RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3] };
					double Target[4] = { CellPosition[listCount++], CellPosition[listCount++], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
					this->CellCount++;
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&Target[0], &feedrate[0], TargetReachedCallback::IMAGEGRID_PROGRAM);
				}
				else if (CellCount == TotalColumns * TotalRows - 1)
				{
					//clear action List and then call partprogram build....
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
					this->ActionIdList->Clear();
					CallPartProgramBuildEvent::raise();
					return;
				}
			}
			else
			{
				//Please connect camera first....
			}
		}
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWImg0006", ex);
	}
}

void RWrapper::RW_ImageGridProgram::SetStoragePath(System::String^ folderPath)
{
	this->currentStoragePath = folderPath;
}

