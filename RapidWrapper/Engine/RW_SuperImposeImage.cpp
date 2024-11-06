//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "RW_SuperImposeImage.h"
#include "..\MAINDLL\Engine\SuperImposeImage.h"

void CALLBACK CaptureImageSound()
{
	try
	{			
		RWrapper::RW_DRO::MYINSTANCE()->RaiseCameraClickSoundEvent();
	}
	catch(Exception^ ex){ }
}

RWrapper::RW_SuperImposeImage::RW_SuperImposeImage()
{	
	 try
	 {
		this->SuperImposeParam = this;
		this->CellWidth=0;
		this->CellHeight=0;
		this->TopLeftX=0;
		this->TopLeftY=0;
		this->GridWidth=0;
		this->GridHeight=0;
		this->BottomRightX=0;
		this->BottomRightY=0;	
		this->FirstPointSet=false;
		this->BothPointSet = false;
		SuperImposeImageObj->ImagecaptureSound = &CaptureImageSound;
	 }
	 catch(Exception^ ex)
	 {
		 RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0001", ex); 
	 }
}

//Destructor.. Release the memory..
RWrapper::RW_SuperImposeImage::~RW_SuperImposeImage()
{
	try
	{
		ClearGrid();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0002", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_SuperImposeImage^ RWrapper::RW_SuperImposeImage::MYINSTANCE()
{
	return SuperImposeParam;
}

void RWrapper::RW_SuperImposeImage::SetTopLeft(double x ,double y)
{
	if(FirstPointSet)
	{
		BothPointSet=true;
	}
	FirstPointSet=true;
	this->TopLeftX=MAINDllOBJECT->getCurrentDRO().getX();
	this->TopLeftY=MAINDllOBJECT->getCurrentDRO().getY();
}

void RWrapper::RW_SuperImposeImage::SetBottomRight(double x ,double y)
{
	if(FirstPointSet)
	{
		BothPointSet=true;
	}
	FirstPointSet=true;

	this->BottomRightX=MAINDllOBJECT->getCurrentDRO().getX();
	this->BottomRightY=MAINDllOBJECT->getCurrentDRO().getY();
}

void RWrapper::RW_SuperImposeImage::InitialiseGrid()
{
	double upp = MAINDllOBJECT->getWindow(0).getUppX();
	try
	{		
		if(BothPointSet)
		{
			double _BottomRightX = this->BottomRightX + ((MAINDllOBJECT->getWindow(0).getWinDim().x)/2)*upp;
			double _BottomRightY = this->BottomRightY - ((MAINDllOBJECT->getWindow(0).getWinDim().y)/2)*upp;

			double _TopLeftX= this->TopLeftX - ((MAINDllOBJECT->getWindow(0).getWinDim().x)/2)*upp;
			double _TopLeftY= this->TopLeftY + ((MAINDllOBJECT->getWindow(0).getWinDim().y)/2)*upp;

		CellWidth=(MAINDllOBJECT->getWindow(0).getWinDim().x)*upp;
		CellHeight=(MAINDllOBJECT->getWindow(0).getWinDim().y)*upp;

		SuperImposeImageObj->InitialiseGridModule(_TopLeftX, _TopLeftY,_BottomRightX,_BottomRightY,CellWidth,CellHeight);
	  /*SuperImposeImageObj->StartImageCapturingProcess();*/		
		}
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0003", ex);
	}
}

void RWrapper::RW_SuperImposeImage::StartImageCapturingInGrid()
{
	try
	{	
		SuperImposeImageObj->StartImageCapturingProcess();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0006", ex);	
	}
}

void RWrapper::RW_SuperImposeImage::ClearGrid()
{
	try
	{
		SuperImposeImageObj->ClearCurrentGrid();
	}
	catch(Exception^ ex)
	{	
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0007", ex);	
	}
}

void RWrapper::RW_SuperImposeImage::IncrementGridSelection()
{
	try
	{
		SuperImposeImageObj->IncrementGridSelections();
	}
	catch(Exception^ ex)
	{	
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0008", ex);	
	}
}

void RWrapper::RW_SuperImposeImage::StartCNC()
{
	try
	{
		MAINDllOBJECT->GridCellHighlighted(true);
	}
	catch(Exception^ ex) 
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0009", ex);	
	}
}

void RWrapper::RW_SuperImposeImage::MachineGotoHomePosition()
{
	try
	{
		SuperImposeImageObj->GotoHomePosition();
	}
	catch(Exception^ ex)
	{	
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSIMPOIMG0010", ex);	
	}	
}

void RWrapper::RW_SuperImposeImage::SetFolderPath(System::String^ FolderPath)
{	
	SuperImposeImageObj->SetFolderPath((char*)Marshal::StringToHGlobalAnsi(FolderPath).ToPointer());
}