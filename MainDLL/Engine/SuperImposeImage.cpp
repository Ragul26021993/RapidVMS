#include "StdAfx.h"
#include "SuperImposeImage.h"
#include "..\GridModule\Engine\GridModule.h"
#include "RCadApp.h"
#include "..\MainDLL.h"
#include "OverlapImgWin.h"
SuperImposeImage::SuperImposeImage()
{
	this->FirstTime=true;
	MyGrid=NULL;
}
SuperImposeImage::~SuperImposeImage()
{
	maxcolreached=false;
}

void SuperImposeImage::InitialiseGridModule(double LeftTopX, double LeftTopY, double BottomRightX, double BottomRightY,double CellWidth, double CellHeight)
{  
	try
	{	
		ClearCurrentGrid();
		maxcolreached=false;
		MyGrid = new GridModule("Grid", false);
		MyGrid->SetLeftTop(LeftTopX,LeftTopY);
		MyGrid->SetBottomRight(BottomRightX,BottomRightY);
		MyGrid->SetCellWidth(CellWidth);
		MyGrid->SetCellHeight(CellHeight);
		MyGrid->SetTheGridCalcualtionType(false,false , true);
		MyGrid->CalculateRectangles(true);		
		StartImageCapturingProcess();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SImp002", __FILE__, __FUNCSIG__); }
}


void SuperImposeImage::IncrementGridSelections()
{
	try
	{
		if(MyGrid->getCurrentSelecteCell() == NULL && MyGrid->getSelectedItemList().size() == 0) return;
		if(MyGrid->getSelectedItemList().size() > 0)
		{
			MyGrid->IncrementaccordingSorted();			
			double GotoPos[3] = {*MyGrid->getCurrentSelecteCell()->GetCenter(), *(MyGrid->getCurrentSelecteCell()->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
			MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);
		}
		else
		{
			MyGrid->IncrementSelection();		
			double GotoPos[3] = {*MyGrid->getCurrentSelecteCell()->GetCenter(), *(MyGrid->getCurrentSelecteCell()->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
			MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SIMP0004", __FILE__, __FUNCSIG__); }
}

void SuperImposeImage::SmartIncrementGridSelections()
{
	try
	{
		if(MyGrid->getCurrentSelecteCell() == NULL && MyGrid->getSelectedItemList().size() == 0) return;
		if(MyGrid->getSelectedItemList().size() > 0)
		{
			MyGrid->IncrementaccordingSorted();			
			double GotoPos[3] = {*MyGrid->getCurrentSelecteCell()->GetCenter(), *(MyGrid->getCurrentSelecteCell()->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
			MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);
		}
		else
		{
			MyGrid->IncrementaccordingEffectiveMovement();		
			double GotoPos[3] = {*MyGrid->getCurrentSelecteCell()->GetCenter(), *(MyGrid->getCurrentSelecteCell()->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
			MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SIMP0004", __FILE__, __FUNCSIG__); }
}
	
void SuperImposeImage::GotoDefaultPosition()
{
	try
	{
		if(MyGrid == NULL) return;
		else
		{
			GM_ITER item = MyGrid->getItemList().begin();
			GRectangle* Gsh = ((*item).second);
			setCurrentHighlitedCell(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1));		
			MyGrid->ClearSelection();
			GridCellShape->selected(true);
			GridCellShape->IncrementVcount();
			MyGrid->SetSelectedRect(GridCellShape);
			MyGrid->SetCurrentIterator(GridCellShape);		
			double GotoPos[3] = {*MyGrid->getCurrentSelecteCell()->GetCenter(), *(MyGrid->getCurrentSelecteCell()->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
			MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);
		}
	}
	catch(...){}
}

void SuperImposeImage::GotoHomePosition()
{
	try
	{
		if(MyGrid == NULL) return;
		else
		{
			GM_ITER item = MyGrid->getECItemList().begin();
			GRectangle* Gsh = ((*item).second);
			setSmartCurrentHighlitedCell(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1));		
			MyGrid->ClearSelection();
			GridCellShape->selected(true);
			GridCellShape->IncrementVcount();
			MyGrid->SetSelectedRect(GridCellShape);
			MyGrid->SetCurrentIteratorOfEffectiveRCMovement(GridCellShape);		
			double GotoPos[3] = {*MyGrid->getCurrentSelecteCell()->GetCenter(), *(MyGrid->getCurrentSelecteCell()->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
			MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);
		}
	}
	catch(...){}
}

void SuperImposeImage::ClearCurrentGrid()
{
	if(this->MyGrid!= NULL)
	{
		for(GM_ITER item = MyGrid->getItemList().begin(); item!=MyGrid->getItemList().end(); item++)
		{
			GRectangle* Gsh = ((*item).second);
			delete Gsh;
		}		
		MyGrid->getItemList().clear();
		delete MyGrid;
		MyGrid = NULL;

		imagepath.clear();
		ColumnsCount = 0;
		RowsCount = 0;
		maxcolreached=false;
	}
}

void SuperImposeImage::setSmartCurrentHighlitedCell(double x , double y)
{
	if(MyGrid != NULL)
	{
		GridCellShape = MyGrid->getSmartCurrentHighlightedCell(x,y);
	}	
}

void SuperImposeImage::setCurrentHighlitedCell(double x , double y)
{
	if(MyGrid != NULL)
	{
		GridCellShape = MyGrid->getCurrentHighlightedCell(x,y);
	}	
}

void SuperImposeImage::StartImageCapturingProcess()
{
	try
	{
		//allitems.clear();
		GList.clear();
		imagecount=0;
		//item=MyGridModule->getECItemList().begin();
		//allitems=MyGrid->getItemList();		
		ColumnsCount = MyGrid->ColumnsCount;
		RowsCount = MyGrid->RowsCount;
		int index = 0;
		for(int i =0 ; i < RowsCount ; i++)
		{
			for(int j =0  ; j < ColumnsCount ; j++)
			{
				if(i % 2 == 0)
				{
					GList[index]=MyGrid->getItemList()[i * ColumnsCount + j];						
				}
				else
				{
					GList[index]=MyGrid->getItemList()[(i+1) * ColumnsCount - j - 1];						
				}
				index++;	
			}
		}
		item=GList.begin();		
		FirstTime=true;	
		CaptureImage();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SImp008", __FILE__, __FUNCSIG__);}
}

//Handling grid module error...
void SuperImposeImage::GridModuleErrorHandler(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

//Set and raise the Error happened..
void SuperImposeImage::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(MAINDllOBJECT->MainDllErrorHappened != NULL)
		MAINDllOBJECT->MainDllErrorHappened((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}

void SuperImposeImage::CaptureImage()
{
	std::string ImFilePath = "";
	//std::string FolderPath = "D:\\SuperImposeImage";
	std::string FolderPath = this->FolderPath;	
	GRectangle* Gsh;
	char Buf[5];	

	if(FirstTime)
	{
		FirstTime=false;
		Gsh = ((*item).second);
		double GotoPos[3] = {*Gsh->GetCenter(), *(Gsh->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
		MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);
	}
	else
	{	
		ImFilePath += itoa(imagecount,Buf,10);				
		ImFilePath = FolderPath + "\\";
		ImFilePath+=Buf;
		ImFilePath +=".bmp";	

		Gsh = ((*item).second);
		
		setCurrentHighlitedCell(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1));		
		MyGrid->ClearSelection();
		GridCellShape->selected(true);
		GridCellShape->IncrementVcount();
		MyGrid->SetSelectedRect(GridCellShape);
		MyGrid->SetCurrentIterator(GridCellShape);	
		//MyGridModule->IncrementaccordingEffectiveMovement();

		if(MAINDllOBJECT->CameraConnected())
		{
			MAINDllOBJECT->SaveCameraImage(ImFilePath);		
			ImagecaptureSound();
			Gsh->SetImagePath(ImFilePath);
			this->imagecount++;
			imagepath[Gsh->getId()]=ImFilePath;			
			OVERLAPIMAGEWINOBJECT->GridImageCaptured(imagepath , MyGrid->RowsCount , MyGrid->ColumnsCount);
		}
		this->item++;
		if(item!=GList.end())
		{
			Gsh = ((*item).second);
			double GotoPos[3] = {*Gsh->GetCenter(), *(Gsh->GetCenter() + 1),MAINDllOBJECT->getCurrentDRO().getZ()};
			MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE);	
		}
		else
		{			
			MAINDllOBJECT->getWindow(3).ZoomToExtents(3);
		}
	}	
}

void SuperImposeImage::SetFolderPath(std::string path)
{
	FolderPath = path;
}

std::string SuperImposeImage::GetFolderPath()
{
	return FolderPath;
}
//Static Instance..
SuperImposeImage* SuperImposeImage::_myinstance = 0;

SuperImposeImage* SuperImposeImage::getMyInstance()
{
	if(_myinstance == NULL)
	{
		_myinstance = new SuperImposeImage();
	}
	return _myinstance;
}
