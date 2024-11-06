#include "StdAfx.h"
#include "RTextHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
RTextHandler::RTextHandler()
{
	try
	{
		Validflag = false;
		setMaxClicks(1);
		Textfsize = 20;
		runningPartprogramValid = false;
		MAINDllOBJECT->SetStatusCode("RTextHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0001", __FILE__, __FUNCSIG__); }
}

//Destructors....//
RTextHandler::~RTextHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0002", __FILE__, __FUNCSIG__); }
}

//Handle the mousedown event...//
void RTextHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mousemove....
void RTextHandler::mouseMove()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone());
		if(getClicksDone() < 1)
			location.set(pt->getX(), pt->getY(), pt->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0004", __FILE__, __FUNCSIG__); }
}

//Draw the text..//
void RTextHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(Validflag)
			GRAFIX->drawString(location.getX(), location.getY(), 0, 0, (char*)CurrentText.c_str(), WPixelWidth * Textfsize, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0005", __FILE__, __FUNCSIG__); }
}


void RTextHandler::SetCurrentText(char* Ctext)
{
	try
	{
		CurrentText = (const char*)Ctext;
		Validflag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0006", __FILE__, __FUNCSIG__); }
}

void RTextHandler::MouseScroll(bool flag)
{
	try
	{
		if(flag)
		{
			if(Textfsize < 60) Textfsize += 1;
		}
		else
		{
			if(Textfsize > 10) Textfsize -= 1;
		}
		MAINDllOBJECT->update_VideoGraphics();
		MAINDllOBJECT->update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0007", __FILE__, __FUNCSIG__); }
}


void RTextHandler::EscapebuttonPress()
{
	try
	{
		if(getClicksDone() > 0)
		{
			Validflag = false;
			CurrentText = "";
			MAINDllOBJECT->deleteForEscape();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0008", __FILE__, __FUNCSIG__); }
}

//
void RTextHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentText == "")
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Vector* pt = &getClicksValue(0);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		//location.set(pt->getX(), pt->getY(), pt->getZ());
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				((ShapeWithList*)RTextShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}
		RTextShape = new RText();
		RTextShape->TextSize(Textfsize);
		RTextShape->SetCurrentText(CurrentText);
		AddShapeAction::addShape(RTextShape);
		AddPointAction::pointAction((ShapeWithList*)RTextShape, baseaction);
		Validflag = false;
		CurrentText = "";			
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0009", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...///
void RTextHandler::PartProgramData()
{
	try
	{
		RTextShape = (RText*)(MAINDllOBJECT->getShapesList().selectedItem());
		RTextShape->IsValid(false);
		this->CurrentText = RTextShape->getText();
		this->Textfsize = RTextShape->TextSize();
		setMaxClicks(1);
		runningPartprogramValid = true;
		Validflag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RTEXTMH0010", __FILE__, __FUNCSIG__); }
}