#pragma once
namespace RWrapper 
{
public ref class RW_UCSParameter
{
public:
	delegate void UCSParamEventHandler(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE UcsType);
	delegate void DXFParamEventHandler(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE DxfType);
	delegate void CursorTypeEventHandler(RWrapper::RW_Enum::RW_WINDOWTYPE windowno, RWrapper::RW_Enum::RW_CURSORTYPE Ctype);

private:
	static RW_UCSParameter^ UcsParamInstance;
public:
	 bool SelectingDxfMode;
	  //Current Video Cursor.. according to the tool type..
	 System::Windows::Input::Cursor ^VideoCursor, ^RcadCursor, ^DxfCursor, ^PartprogramCursor;

	  event UCSParamEventHandler^ UcsUpdateEvent;
	 event DXFParamEventHandler^ DxfUpdateEvent;
	 event CursorTypeEventHandler^ RapidCursorChanged;
public:

	 RW_UCSParameter();
	 ~RW_UCSParameter();
	 static RW_UCSParameter^ MYINSTANCE();

	 void SelectDXF(int id);
	 void DeleteDXF(int id);
	 void SelectUCS(int UCSid);
	 void InitialilseGrid(double LeftTopX, double LeftTopY, double GridWidth, double GridHeight, double CellWidth, double CellHeight);

	 void RaiseUCSChangedEvents(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE UcsType);
	 void RaiseDXFChangedEvents(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE dType);
	 void UpdateCursor_AccordingToWindowNo(RWrapper::RW_Enum::RW_WINDOWTYPE windowno, RWrapper::RW_Enum::RW_CURSORTYPE Ctype);

};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!