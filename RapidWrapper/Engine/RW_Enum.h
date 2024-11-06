
//Header File to Hold All enum type for Wrapper...............
#pragma once

namespace RWrapper 
{
	namespace RW_Enum
	{

#pragma region MessageBox Enumeration Type
		public enum struct  RW_MSGBOXTYPE
		{
			MSG_YES_NO,
			MSG_OK,
			MSG_OK_CANCEL,
			MSG_YES_NO_CANCEL,	
		};

		public enum struct  RW_MSGBOXICONTYPE
		{
			MSG_EXCLAMATION,
			MSG_ERROR,
			MSG_INFORMATION,
			MSG_NONE,
			MSG_QUESTION,
			MSG_STOP,
			MSG_WARNING,
		};
#pragma endregion

#pragma region Window And Cursor Enumeration Type
		public enum struct RW_WINDOWTYPE
		{
			VIDEO_WINDOW,
			RCAD_WINDOW,
			DXF_WINDOW,
		};

		public enum struct RW_CURSORTYPE
		{
			RW_CURSOR_DEFAULT,
			RW_CURSOR_CROSS,
			RW_CURSOR_ARCFG,	  
			RW_CURSOR_CIRCLEFG,
			RW_CURSOR_ANGRECTFG,
			RW_CURSOR_RECTFG,
			RW_CURSOR_NULL,
			RW_CURSOR_TRIM,
			RW_CURSOR_PANHOLD,		
			RW_CURSOR_PANRELEASE,		
		};
#pragma endregion

#pragma region ShapeParameter Enumeration Type
		public enum struct RW_SHAPEPARAMTYPE
		{
			SHAPE_ADDED,
			SHAPE_SELECTION_CHANGED,
			SHAPE_POINTS_UPDATE,
			SHAPE_TABLE_UPDATE,
			SHAPE_REMOVED,
			SHAPE_UPDATE,
			SHAPE_HIDE_UNHIDE,
		};
#pragma endregion


#pragma region ActionParameter Enumeration Type
		public enum struct RW_ACTIONPARAMTYPE
		{
			ACTION_ADDED,
			ACTION_REMOVED,
			ACTION_UPDATE,
			ACTION_SELECTION_CHANGED,
		};
#pragma endregion

		
#pragma region UCSParameter Type Enumeration 
		public enum struct RW_UCSPARAMETER_TYPE
		{
			UCS_ADDED,
			UCS_SELECTION_CHANGED,
			UCS_REMOVED,
			CHANGE_DXF_LIST,
			UCS_ROTATE_ANGLE,
		};
#pragma endregion

		
#pragma region DXFParameter Type Enumeration 
		public enum struct RW_DXFPARAMETER_TYPE
		{			
			DXF_ADDED,
			DXF_REMOVED,
			DXF_SELECTION_CHANGED,
		};

#pragma region MeasurementParameter Type Enumeration 
		public enum struct RW_MEASUREPARAM_TYPE
		{			
			MEASUREMENT_ADDED,
			MEASUREMENT_REMOVED,
			MEASUREMENT_SELECTION_CHANGED,
			MEASUREMENT_UPDATE,
			MEASUREMENT_TABLE_UPDATE,
		};
#pragma endregion

#pragma region Enum for PartProgram parameter
	public enum struct PROGRAM_PARAMETER
	{
		PP_NULL,
		DEVIATION,
		RELATIVE_PTOFFSET,
		DEVIATION_RELATIVE_PT,
	};
#pragma endregion
	}

	public enum struct ClassNames
	{
		AutoCalibration,
		L_Block,
		DelphiCam,
		SphereCalibration,
		V_Block_Calib,
		Fixture_Calib,
		ImageGridProgram,
		SuperImposeImage,
	};
}

//Created by Rahul Singh.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!