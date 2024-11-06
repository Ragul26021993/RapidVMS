#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "RW_OverlapImgWin.h"
#include "..\MAINDLL\Engine\OverlapImgWin.h"

RWrapper::RW_OverlapImgWin^ RWrapper::RW_OverlapImgWin::MyInstance()
{
	
	if(RW_OverlapImgWinInstance == nullptr)
	{
		RW_OverlapImgWinInstance = gcnew RW_OverlapImgWin();
	}
	return RW_OverlapImgWinInstance;
}

RWrapper::RW_OverlapImgWin::RW_OverlapImgWin()
{
	try
	{
	}
	catch(Exception^ ex){}
}

RWrapper::RW_OverlapImgWin::~RW_OverlapImgWin()
{
	try
	{

	}
	catch(Exception^ ex){}
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinLeftMouseDown(double x, double y)
{
	OVERLAPIMAGEWINOBJECT->OnLeftMouseDown_OverlapImgWin(x, y);	
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinRightMouseDown(double x, double y)
{
	OVERLAPIMAGEWINOBJECT->OnRightMouseDown_OverlapImgWin(x, y);
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinMiddleMouseDown(double x, double y)
{
	OVERLAPIMAGEWINOBJECT->OnMiddleMouseDown_OverlapImgWin(x, y);
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinLeftMouseUp(double x, double y)
{
	OVERLAPIMAGEWINOBJECT->OnLeftMouseUp_OverlapImgWin(x, y);
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinRightMouseUp(double x, double y)
{
	OVERLAPIMAGEWINOBJECT->OnRightMouseUp_OverlapImgWin(x, y);
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinMiddleMouseUp(double x, double y)
{
	OVERLAPIMAGEWINOBJECT->OnMiddleMouseUp_OverlapImgWin(x, y);
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinMouseMove(double x, double y)
{
	OVERLAPIMAGEWINOBJECT->OnMouseMove_OverlapImgWin(x, y);
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinMouseWheel(int evalue)
{
	OVERLAPIMAGEWINOBJECT->OnMouseWheel_OverlapImgWin(evalue);
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinMouseEnter()
{
	OVERLAPIMAGEWINOBJECT->OnMouseEnter_OverlapImgWin();
}

void RWrapper::RW_OverlapImgWin::OnOverlapImgWinMouseLeave()
{
	OVERLAPIMAGEWINOBJECT->OnMouseLeave_OverlapImgWin();
}

void RWrapper::RW_OverlapImgWin::UpdateGraphics()
{
	OVERLAPIMAGEWINOBJECT->update_OverlapImgWinGraphics();
}

void RWrapper::RW_OverlapImgWin::ClearAll()
{
	OVERLAPIMAGEWINOBJECT->CloseWindow();	
}