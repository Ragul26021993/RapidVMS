#include "Stdafx.h"
#include "RW_MainInterface.h"

void CALLBACK DrawNormalString(int fontsize, char* fonttype, char* fontstring, double PosX, double PosY, double PosZ, double angle)
{
	RWrapper::RW_DrawGraphicsText::MYINSTANCE()->Draw_NormalString(fontsize, gcnew System::String(fonttype), gcnew System::String(fontstring), PosX, PosY, PosZ, angle);
}

void CALLBACK InitialiseDefault_Rfont(RGraphicsFont* fontobject)
{
	RWrapper::RW_DrawGraphicsText::MYINSTANCE()->Initialise_RapidFontDefaultProperty(fontobject);
}

RWrapper::RW_DrawGraphicsText::RW_DrawGraphicsText()
{
	try
	{
		this->DrawGraphicsInstance = this;	
		MAINDllOBJECT->RaiseDrawNormalString = &DrawNormalString;
		MAINDllOBJECT->InitaliseDefaultFontList = &InitialiseDefault_Rfont;
		//Initialise_RapidFontDefaultProperty();
		//Draw_NormalString(12, "Tahoma", "Test", 0, 0, 0, 0);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRGH0001", ex); }
}

RWrapper::RW_DrawGraphicsText::~RW_DrawGraphicsText()
{
}

RWrapper::RW_DrawGraphicsText^ RWrapper::RW_DrawGraphicsText::MYINSTANCE()
{
	return DrawGraphicsInstance;
}

void RWrapper::RW_DrawGraphicsText::Draw_NormalString(int Str_FontSize, System::String^ Str_Fonttype, System::String^ Str_FontText, double PosX, double PosY, double PosZ, double angle)
{
	try
	{
	
		System::Windows::Forms::Label^ HiddenDrawWndow = gcnew System::Windows::Forms::Label;
		HiddenDrawWndow->BackColor = System::Drawing::Color::Transparent;
		System::Drawing::Graphics^ Str_graphics = System::Drawing::Graphics::FromHwnd(HiddenDrawWndow->Handle);
		System::Drawing::SizeF^ Str_size = Str_graphics->MeasureString(Str_FontText, gcnew System::Drawing::Font(Str_Fonttype, Str_FontSize, System::Drawing::FontStyle::Regular));
		System::Drawing::Bitmap^ Str_Bitmap = gcnew System::Drawing::Bitmap(Str_size->Width, Str_size->Height);
        Str_graphics = System::Drawing::Graphics::FromImage(Str_Bitmap);
		Str_graphics->TextRenderingHint = System::Drawing::Text::TextRenderingHint::AntiAlias;
		Str_graphics->DrawString(Str_FontText, gcnew System::Drawing::Font(Str_Fonttype, Str_FontSize, System::Drawing::FontStyle::Regular), gcnew System::Drawing::SolidBrush(Color::Green), 0, 0);

		//RWrapper::RW_MainInterface::MYINSTANCE()->LoadReferecneImage_OneShot("D:\\Test.jpg");
		//System::Drawing::Bitmap^ Str_Bitmap = gcnew System::Drawing::Bitmap("D:\\Test.jpg");
		Str_Bitmap->Save("D:\\" + Str_FontText +".jpg");
		BYTE* Str_ByteArray = new BYTE[Str_Bitmap->Width * Str_Bitmap->Height * 4];
		System::Drawing::Imaging::BitmapData^ Str_BitmapData;
		cli::array<BYTE>^ iBytes = gcnew cli::array<BYTE>(Str_Bitmap->Width * Str_Bitmap->Height * 4);
		System::Drawing::Rectangle rrect(0, 0, Str_Bitmap->Width, Str_Bitmap->Height);

		System::Drawing::Imaging::PixelFormat testedf = Str_Bitmap->PixelFormat;

		Str_BitmapData = Str_Bitmap->LockBits(rrect, Imaging::ImageLockMode::ReadWrite, Str_Bitmap->PixelFormat);
		int bytes = Str_BitmapData->Stride;  
		System::Runtime::InteropServices::Marshal::Copy(Str_BitmapData->Scan0, iBytes, 0, iBytes->Length);
		Str_Bitmap->UnlockBits(Str_BitmapData);
		for(int counter = 3; counter < iBytes->Length; counter += 4)  
		{
                iBytes[counter] = 255;  
				// iBytes[counter - 1] = 255;  
		}
		for(int i = 0; i < iBytes->Length; i++)
			Str_ByteArray[i] = iBytes[i];
		//GRAFIX->GenerateTextureFromImage(Str_ByteArray, Str_Bitmap->Width, Str_Bitmap->Height);
		GRAFIX->drawStringtexture(Str_ByteArray, Str_Bitmap->Width, Str_Bitmap->Height, PosX, PosY, 0, angle);
		Str_Bitmap->Save("D:\\" + Str_FontText +"1.jpg");
		delete Str_ByteArray;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRGH0001", ex); }
}

void RWrapper::RW_DrawGraphicsText::Initialise_RapidFontDefaultProperty(RGraphicsFont* fontobject)
{
	try
	{
		System::Windows::Forms::Label^ HiddenDrawWndow = gcnew System::Windows::Forms::Label;
		HiddenDrawWndow->BackColor = System::Drawing::Color::Transparent;
		System::Drawing::Graphics^ Str_graphics = System::Drawing::Graphics::FromHwnd(HiddenDrawWndow->Handle);
		std::list<BYTE*> FontList_ByteArray;
		std::list<int> FontList_Widtharray, FontList_heightarray;
		System::String^ Str_Fonttype = "Tahoma";
		int Str_FontSize = 30;
		for(int i = 33; i < 127; i++)
		{
			char c = i;
			System::String^ Str_FontText = gcnew System::String(&c);
			if(Str_FontText == "") Str_FontText = " ";
			System::Drawing::SizeF^ Str_size = Str_graphics->MeasureString(Str_FontText, gcnew System::Drawing::Font(Str_Fonttype, Str_FontSize, System::Drawing::FontStyle::Regular));
			System::Drawing::Bitmap^ Str_Bitmap = gcnew System::Drawing::Bitmap(Str_size->Width, Str_size->Height);
			Str_graphics = System::Drawing::Graphics::FromImage(Str_Bitmap);
			Str_graphics->TextRenderingHint = System::Drawing::Text::TextRenderingHint::AntiAlias;
			Str_graphics->DrawString(Str_FontText, gcnew System::Drawing::Font(Str_Fonttype, Str_FontSize, System::Drawing::FontStyle::Regular), gcnew System::Drawing::SolidBrush(Color::Green), 0, 0);
			Str_Bitmap->Save("D:\\Test\\" + System::Convert::ToString(i) +".jpg");
			BYTE* Str_ByteArray = new BYTE[Str_Bitmap->Width * Str_Bitmap->Height * 4];
			System::Drawing::Imaging::BitmapData^ Str_BitmapData;
			cli::array<BYTE>^ iBytes = gcnew cli::array<BYTE>(Str_Bitmap->Width * Str_Bitmap->Height * 4);
			System::Drawing::Rectangle rrect(0, 0, Str_Bitmap->Width, Str_Bitmap->Height);
			System::Drawing::Imaging::PixelFormat testedf = Str_Bitmap->PixelFormat;
			//Imaging::PixelFormat::Format32bppArgb
			Str_BitmapData = Str_Bitmap->LockBits(rrect, Imaging::ImageLockMode::ReadWrite, Str_Bitmap->PixelFormat);
			System::Runtime::InteropServices::Marshal::Copy(Str_BitmapData->Scan0, iBytes, 0, iBytes->Length);
			Str_Bitmap->UnlockBits(Str_BitmapData);
			for(int counter = 3; counter < iBytes->Length; counter += 4)  
				iBytes[counter] = 255;
			for(int i = 0; i < iBytes->Length; i++)
				Str_ByteArray[i] = iBytes[i];
			FontList_ByteArray.push_back(Str_ByteArray);
			FontList_Widtharray.push_back(Str_Bitmap->Width);
			FontList_heightarray.push_back(Str_Bitmap->Height);
		}
		/*MAINDllOBJECT->dontUpdateGraphcis = true;
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->Wait_DxfGraphicsUpdate();*/
		//Video_Graphics->SelectWindow();
		fontobject->Initialise_RFontlist(&FontList_ByteArray, &FontList_Widtharray, &FontList_heightarray, (char*)Marshal::StringToHGlobalAnsi(Str_Fonttype).ToPointer(), Str_FontSize, false, false, false);
		//MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRGH0001", ex); }
}