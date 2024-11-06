#pragma once
namespace RWrapper 
{
	public ref class RW_OverlapImgWin
	{
	private:		
		static RW_OverlapImgWin^ RW_OverlapImgWinInstance = nullptr;
		RW_OverlapImgWin();
		~RW_OverlapImgWin();
	public :
		static RW_OverlapImgWin^ MyInstance();
#pragma region overlap image window Mouse Event Handling
	 void OnOverlapImgWinLeftMouseDown(double x, double y); 
	 void OnOverlapImgWinRightMouseDown(double x, double y); 
	 void OnOverlapImgWinMiddleMouseDown(double x, double y); 
	 void OnOverlapImgWinLeftMouseUp(double x, double y); 
	 void OnOverlapImgWinRightMouseUp(double x, double y); 
	 void OnOverlapImgWinMiddleMouseUp(double x, double y); 
	 void OnOverlapImgWinMouseMove(double x, double y);
	 void OnOverlapImgWinMouseWheel(int evalue);
	 void OnOverlapImgWinMouseEnter();
	 void OnOverlapImgWinMouseLeave();
	 
	 void UpdateGraphics();
	 void ClearAll();
#pragma endregion
	};
}
//Created by Shweytank Mishra.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!