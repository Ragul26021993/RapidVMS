#include "stdafx.h"
#include "PCDHandler3D.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
PCDHandler3D::PCDHandler3D(bool AddPcdCircle)
{
	try
	{
		setMaxClicks(2);
		if(AddPcdCircle)
			MAINDllOBJECT->AddNewPcdCircle(true);
		/*MAINDllOBJECT->SetStatusCode("PCD01");*/
		MAINDllOBJECT->SetStatusCode("PCDHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
PCDHandler3D::~PCDHandler3D()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0002", __FILE__, __FUNCSIG__); }
}


//Handle the mosuemove..
void PCDHandler3D::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down... Select the shapes..
void PCDHandler3D::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
		Shape* Sh = MAINDllOBJECT->highlightedShape();
		if(Sh->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		{			
			MAINDllOBJECT->AddCircleToCurrentPCD(Sh->getId());	
			setClicksDone(getClicksDone() - 1);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0005", __FILE__, __FUNCSIG__); }
}


//Handle the escape button press..
void PCDHandler3D::EscapebuttonPress()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0006", __FILE__, __FUNCSIG__); }
}

//Handle the right click...
void PCDHandler3D::RmouseDown(double x, double y)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0007", __FILE__, __FUNCSIG__); }
}


//Draw function
void PCDHandler3D::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0008", __FILE__, __FUNCSIG__); }
}