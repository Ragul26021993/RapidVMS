// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RAPIDCAMCONTROL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RAPIDCAMCONTROL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef RAPIDCAMCONTROL_EXPORTS
#define RAPIDCAMCONTROL_API __declspec(dllexport)
#else
#define RAPIDCAMCONTROL_API __declspec(dllimport)
#endif
#include "stdafx.h"

#pragma warning(disable : 4995)

extern "C"
{	//Camera Controls
	RAPIDCAMCONTROL_API bool Initialise(HWND hwndApp, int CamLabel, int ColorKey);
	RAPIDCAMCONTROL_API bool SetVideoWindow(int vWidth, int vHeight, int FrameRate, bool SetAutoExposure, double distortionFactor, long offset_x, long offset_y);
	RAPIDCAMCONTROL_API bool SetVideoWindowEx(int vWidth, int vHeight, int SrcLeft, int SrcTop, int FinalWidth, int FinalHeight, int FrameRate,
												bool SetAutoExposure, double distortionFactor, long offset_x, long offset_y, bool CompressVideoFrame);
	RAPIDCAMCONTROL_API bool SetVideoWindowEx2(int vWidth, int vHeight, int SrcLeft, int SrcTop, int DisplayWidth, int DisplayHeight, double FractionToDisplay, double FrameRate,
												bool SetAutoExposure, double distortionFactor, long offset_x, long offset_y);

	RAPIDCAMCONTROL_API void CameraClose();
	RAPIDCAMCONTROL_API void Preview();
	RAPIDCAMCONTROL_API void StopPreview();
	RAPIDCAMCONTROL_API int GetCameraProperty(int propID);
	RAPIDCAMCONTROL_API void SetCameraProperty(int propID, int propValue);
	RAPIDCAMCONTROL_API void GetVideoDialog();
	RAPIDCAMCONTROL_API void GetCameraFormatDialog();
	RAPIDCAMCONTROL_API void FlipHorizontal();
	RAPIDCAMCONTROL_API void FlipVertical();
	RAPIDCAMCONTROL_API void SetTransKey(int TransColor);
	RAPIDCAMCONTROL_API void SetGraphicsOpacity(int opacity);
	RAPIDCAMCONTROL_API int CurrentCamera();
	RAPIDCAMCONTROL_API bool Set_Exposure(int Value);
	RAPIDCAMCONTROL_API bool SetRadialImageCorrectionParams(double distortionFactor, long offset_x, long offset_y);

	//Get Image and part and other properties...
 	RAPIDCAMCONTROL_API void GetImage(BYTE* pImageBuffer, bool FullFrame);
 	RAPIDCAMCONTROL_API bool Get_Variance_Values(double* P);
	RAPIDCAMCONTROL_API bool GetAverageImage(double* AverageImage, int FrameCt);
	RAPIDCAMCONTROL_API bool GetImageNow_byte(BYTE *pImage, bool FullFrame);
	RAPIDCAMCONTROL_API bool GetImageNow_byte_1bpp(int channel, BYTE* pImage, bool FullFrame);
	RAPIDCAMCONTROL_API bool GetImageNow_double_1bpp(int channel, double* pImage, bool FullFrame);
	RAPIDCAMCONTROL_API bool GetAverageImage_double_1bpp(int channel, double* AverageImage, bool FullFrame, int FrameCt);

	//Graphics Controls
	RAPIDCAMCONTROL_API void AddGraphics(); //Mix the bitmap to show graphics with video
	RAPIDCAMCONTROL_API void ClearGraph(); //Clear the bitmap of all graphics
	RAPIDCAMCONTROL_API void DisableGraphics(); //Stop showing grpahics, and show only pure video
	RAPIDCAMCONTROL_API void SetGraph24(BYTE* lpBits); //Set all bits in the graphics directly.
	RAPIDCAMCONTROL_API void SetGraph32(BYTE* lpBits); //Set all bits in the graphics directly.
	RAPIDCAMCONTROL_API void EnableGraphics();//Enable showing grpahics

	//Focus Meter Functions ------
	//default value for normalized is true and pixgap is 2.
	RAPIDCAMCONTROL_API bool GetFocusMetric(int* pR, double* pFocus, int N, bool normalized, int pixgap);
	RAPIDCAMCONTROL_API void GetFocusMetric2(int* pR, double* pFocus, int N);
	RAPIDCAMCONTROL_API bool FocusScan_Region(int* pR, int AnalHalfWidth, int StepSize, double* Ans);
	RAPIDCAMCONTROL_API bool GetFocusMetric_Profile(int* pR, double* pFocus, int N);
	RAPIDCAMCONTROL_API bool GetFocusMetric_noNorm(int* pR, double* pFocus, int N);
	
	RAPIDCAMCONTROL_API void Set_ImBufferMode(bool LookatImage);

	//Error Handling.
	RAPIDCAMCONTROL_API	void (*RapidCamError)(char* RapidCamerrorcode);
}