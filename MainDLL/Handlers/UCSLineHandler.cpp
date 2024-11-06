#include "stdafx.h"
#include "../Engine/RCadApp.h"
#include "UCSLineHandler.h"
#include "..\Shapes\Line.h"
#include "../Actions/AddUCSAction.h"

//Constructor....//
UCSLineHandle::UCSLineHandle()
{
	try
	{
		setMaxClicks(2);
		valid = false;
		RotateAngle = 0;
		/*MAINDllOBJECT->SetStatusCode("UCS002");*/
		MAINDllOBJECT->SetStatusCode("UCSLineHandle01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
UCSLineHandle::~UCSLineHandle()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0002", __FILE__, __FUNCSIG__); }
}

//Get the line dimension for the new ucs..//
bool UCSLineHandle::getLineDimension( Shape *s )
{
	try
	{
		this->s1 = s;
		bool flag = false;
		switch(s->ShapeType)
		{
			case RapidEnums::SHAPETYPE::XLINE:
			case RapidEnums::SHAPETYPE::XRAY:
			case RapidEnums::SHAPETYPE::LINE:
				RotateAngle = ((Line*)s)->Angle();
				UIntercept = ((Line*)s)->Intercept();
				RMATH2DOBJECT->Angle_FirstFourthQuad(&RotateAngle);
				flag = true;
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0003", __FILE__, __FUNCSIG__); return false;}
}

void UCSLineHandle::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0004", __FILE__, __FUNCSIG__); }
}

void UCSLineHandle::EscapebuttonPress()
{
	try 
	{
		MAINDllOBJECT->SetStatusCode("UCS002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0005", __FILE__, __FUNCSIG__); }
}

///Hnadle the Mouse down event.../
void UCSLineHandle::LmouseDown()
{
	try
	{
		if(getClicksDone() != getMaxClicks() && MAINDllOBJECT->highlightedPoint() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if (MAINDllOBJECT->highlightedPoint() != NULL && getClicksDone() != getMaxClicks())
		{
			//Shape* ts = MAINDllOBJECT->highlightedShape();
			//switch (ts->ShapeType)
			//{
			//case RapidEnums::SHAPETYPE::RPOINT:
			//case RapidEnums::SHAPETYPE::CIRCLE:
			//case RapidEnums::SHAPETYPE::ARC:
			//	this->s0 = ts;
			//	break;
			//case RapidEnums::SHAPETYPE::LINE:
			//	//this->s0 = (Shape*)(((Line*)ts)->SnapPointList[0]);
			//	break;
			//}
			//if (MAINDllOBJECT->highlightedShape())
			//if (baseaction->CurrentWindowNo == 1)
			//	baseaction->DroMovementFlag = false;
			//baseaction->DerivedShape = true;
			setClicksValue(0,MAINDllOBJECT->highlightedPoint());
			Centerpt = MAINDllOBJECT->highlightedPoint();
			MAINDllOBJECT->SetStatusCode("UCS003");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0006", __FILE__, __FUNCSIG__); }
}

//rotate the graphics with respect to line angle..//
void UCSLineHandle::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0007", __FILE__, __FUNCSIG__); }
}

//Max moue click.../
void UCSLineHandle::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getMaxClicks() - 1);
			return;
		}
		if(!getLineDimension(MAINDllOBJECT->highlightedShape()))
		{
			setClicksDone(getMaxClicks() - 1);
			return;
		}
		MAINDllOBJECT->AddingUCSModeFlag = true;
		//Create new UCS.....//
		UCS* nucs = new UCS(_T("UCS"));
		UCS& cucs = MAINDllOBJECT->getCurrentUCS();
		//Set the UCS mode..//
		nucs->UCSMode(2);
		//Set the UCS angle...//
		nucs->UCSangle(RotateAngle);
		nucs->UCSIntercept(UIntercept);
		//UCS center....///
		nucs->UCSPoint.set(getClicksValue(0));
		nucs->SetUCSProjectionType(cucs.UCSProjectionType());
		//Set the parameters and the transformation matrix
		nucs->SetParameters(RotateAngle, &getClicksValue(0));
		////Back transform Matrix..
		//double CenterPt[2] = {0}, UcsAngle = nucs->UCSangle();
		//RMATH2DOBJECT->TransformM_RotateAboutPoint(&CenterPt[0], -UcsAngle, &nucs->transform[0]);
		if(cucs.UCSMode() == 1)
		{
		    nucs->ParentUcsId(cucs.getId());
		    cucs.ChildUcsId(nucs->getId());
		}
		//Add the windows..//
		//nucs->getWindow(0).init(cucs.getWindow(0));
		//nucs->getWindow(1).init(cucs.getWindow(1));
		//nucs->getWindow(2).init(cucs.getWindow(2));
		for (int j = 0; j < 3; j++)
		{
			nucs->getWindow(j).init(cucs.getWindow(j));
			nucs->getWindow(j).WindowNo = j;
		}

		nucs->CenterPt = Centerpt;
		nucs->AxisLine = MAINDllOBJECT->highlightedShape();
		//nucs->AxisLine->addChild(nucs);
		//Add new UCS...//
		AddUCSAction::addUCS(nucs);
		MAINDllOBJECT->AddingUCSModeFlag = false;
		nucs->getWindow(1).homeIt(1);
		MAINDllOBJECT->SetRcadRotateAngle(RotateAngle);
		MAINDllOBJECT->SetVideoRotateAngle(RotateAngle);
		//nucs->SetKeepgraphicsStraightflag(MAINDllOBJECT->KeepGraphicsStflag);
		MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		//Update the graphics...//
		MAINDllOBJECT->updateAll();
		MAINDllOBJECT->SetStatusCode("Finished");
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		MAINDllOBJECT->LineArcUcsCreated();
	}
	catch(...){MAINDllOBJECT->AddingUCSModeFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("UCSLINEMH0008", __FILE__, __FUNCSIG__); }
}