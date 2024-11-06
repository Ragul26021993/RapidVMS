#pragma once

using namespace System::Drawing;
using namespace System;
namespace RWrapper 
{

public ref class RW_DrawGraphicsText
{
private:	 
	 static RW_DrawGraphicsText^ DrawGraphicsInstance;
	 System::Windows::Forms::Form^ HiddenDrawWndow;
public:
	 RW_DrawGraphicsText();
	 ~RW_DrawGraphicsText();
	 static RW_DrawGraphicsText^ MYINSTANCE();

public:
	void Draw_NormalString(int Str_FontSize, System::String^ Str_Fonttype, System::String^ Str_FontText, double PosX, double PosY, double PosZ, double angle);
	void Initialise_RapidFontDefaultProperty(RGraphicsFont* fontobject);

};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!