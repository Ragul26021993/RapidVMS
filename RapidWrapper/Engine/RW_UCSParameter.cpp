#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\DXF\DXFExpose.h"

void CALLBACK RW_WindowCursorChanged(int Wtype, int Ctype)
{
	RWrapper::RW_UCSParameter::MYINSTANCE()->UpdateCursor_AccordingToWindowNo(RWrapper::RW_Enum::RW_WINDOWTYPE(Wtype), RWrapper::RW_Enum::RW_CURSORTYPE(Ctype));
}

RWrapper::RW_UCSParameter::RW_UCSParameter()
{
	try
	{
		 this->UcsParamInstance = this;	
		 this->SelectingDxfMode = false;
		 MAINDllOBJECT->WindowCursorChanged = &RW_WindowCursorChanged;

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDIMPRD0001", ex); }
}

RWrapper::RW_UCSParameter::~RW_UCSParameter()
{
}

RWrapper::RW_UCSParameter^ RWrapper::RW_UCSParameter::MYINSTANCE()
{
	return UcsParamInstance;
}

void RWrapper::RW_UCSParameter::SelectDXF(int id)
{
	if(SelectingDxfMode) return;
	DXFEXPOSEOBJECT->selectDXF(id);
}

void RWrapper::RW_UCSParameter::DeleteDXF(int id)
{
	DXFEXPOSEOBJECT->deleteDXF(id);
}

void RWrapper::RW_UCSParameter::SelectUCS(int UCSid)
{
	 try
	 {
		 if(MAINDllOBJECT->AddingUCSModeFlag)
		   return;
		 //if (UCSid != 0)
		 //{
			// MAINDllOBJECT->changeUCS(0, true);
			// Sleep(1000);
		 //}
		 MAINDllOBJECT->changeUCS(UCSid, true);
	     RWrapper::RW_MainInterface::MYINSTANCE()->RefreshDRO();	
	 }
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0046", ex);
	}
}

void RWrapper::RW_UCSParameter::InitialilseGrid(double LeftTopX, double LeftTopY, double GridWidth, double GridHeight, double CellWidth, double CellHeight)
{
	MAINDllOBJECT->InitialiseGridModule(LeftTopX, LeftTopY,GridWidth,GridHeight,CellWidth,CellHeight);
}

void RWrapper::RW_UCSParameter::RaiseUCSChangedEvents(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE UcsType)
{
	UcsUpdateEvent::raise(UcsType);
}

void RWrapper::RW_UCSParameter::RaiseDXFChangedEvents(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE dType)
{
	DxfUpdateEvent::raise(dType);
}

void RWrapper::RW_UCSParameter::UpdateCursor_AccordingToWindowNo(RWrapper::RW_Enum::RW_WINDOWTYPE windowno, RWrapper::RW_Enum::RW_CURSORTYPE Ctype)
{
	RapidCursorChanged::raise(windowno, Ctype);
}