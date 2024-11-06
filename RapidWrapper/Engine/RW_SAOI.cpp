//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "RW_SAOI.h"
///Default constructor.. Initialise the deafault settings..
RWrapper::RW_SAOIParameter::RW_SAOIParameter()
{
	 try
	 {
		this->CellWidth=0;
		this->CellHeight=0;
		this->TopLeftX=0;
		this->TopLeftY=0;
		this->GridWidth=0;
		this->GridHeight=0;
		this->DisplayGrid=false;
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0001", ex); }
}

//Destructor.. Release the memory..
RWrapper::RW_SAOIParameter::~RW_SAOIParameter()
{
	try
	{
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_SAOIParameter^ RWrapper::RW_SAOIParameter::MYINSTANCE()
{
	return SAOIParam;
}
void RWrapper::RW_SAOIParameter::SetTopLeftAccToGetPointer(double x ,double y)
{
	MAINDllOBJECT->getWindow(2).mouseDown(x, y);
	this->xvalue=MAINDllOBJECT->getWindow(2).getPointer().x;
	this->yvalue=MAINDllOBJECT->getWindow(2).getPointer().y;
}
void RWrapper::RW_SAOIParameter::InitialilseGrid(double TopLeftX, double TopLeftY, double GridWidth, double GridHeight, double CellWidth, double CellHeight)
{
	try
	{
		
	this->TopLeftX=TopLeftX;
	this->TopLeftY=TopLeftY;
	this->GridWidth=GridWidth;
	this->GridHeight=GridHeight;
	this->CellWidth=CellWidth;
	this->CellHeight=CellHeight;
	MAINDllOBJECT->InitialiseGridModule(TopLeftX, TopLeftY,GridWidth,GridHeight,CellWidth,CellHeight);
	this->DisplayGrid=true;
	}
	catch(Exception^ ex)
	{
	RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex);
	}
}
void RWrapper::RW_SAOIParameter::ClearGrid()
{
	try
	{
		MAINDllOBJECT->ClearCurrentGrid();
	}
	catch(Exception^ ex)
	{	RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex);	}
}
void RWrapper::RW_SAOIParameter::IncrementGridSelection()
{
	try{
		MAINDllOBJECT->IncrementGridSelections();}
	catch(Exception^ ex)
	{	RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex);	}
}
void RWrapper::RW_SAOIParameter::StartCNC()
{
	try{
		MAINDllOBJECT->GridCellHighlighted(true);}
	catch(Exception^ ex) 
	{	RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex);	}
}
void RWrapper::RW_SAOIParameter::ShowHideGrid()
{	try
	{
		if(this->DisplayGrid)
		{
		this->DisplayGrid=false;
		MAINDllOBJECT->ShowHideCurrentGrid();
		MAINDllOBJECT->GridCellHighlighted(false);
		}
		else
		{
			this->DisplayGrid=true;
			MAINDllOBJECT->ShowHideCurrentGrid();
		}
	}
catch(Exception^ ex)
	{	RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex);	}
}
void RWrapper::RW_SAOIParameter::MachineGotoDefaultPosition()
{
	try
	{
		MAINDllOBJECT->GotoDefaultPosition();
	}
	catch(Exception^ ex)
	{	RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0002", ex);	}

}