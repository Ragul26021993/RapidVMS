#include "stdafx.h"
#include "../Engine/RCadApp.h"
#include "..\Shapes\Shape.h"
#include "UCSPlaneHandler.h"
#include "../Actions/AddUCSAction.h"
#include "..\Helper\Helper.h"

//Constructor....//
UCSPlaneHandle::UCSPlaneHandle()
{
	try
	{
		PointSelctNum = 0, ShapSelctNum = 0;
		setMaxClicks(4);
		valid = false;
		MAINDllOBJECT->SetStatusCode("UCSPlaneHandle01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0001", __FILE__, __FUNCSIG__); }
}

UCSPlaneHandle::~UCSPlaneHandle()
{
}

void UCSPlaneHandle::mouseMove()
{
}

void UCSPlaneHandle::EscapebuttonPress()
{
	try 
	{
		MAINDllOBJECT->SetStatusCode("UCS002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0005", __FILE__, __FUNCSIG__); }
}

///Hnadle the Mouse down event.../
void UCSPlaneHandle::LmouseDown()
{
	try
	{
		if((MAINDllOBJECT->highlightedShape() != NULL))
        {   
			for(int i = 0; i < ShapSelctNum; i++)
			{
		     	if(MAINDllOBJECT->highlightedShape()->getId() == Cshape[i]->getId())
			    {
			       MAINDllOBJECT->SetStatusCode("UCS005");
				   setClicksDone(getClicksDone() - 1);
			       return;
				}
			}
			if(!(HELPEROBJECT->getShapeDimension(MAINDllOBJECT->highlightedShape(), &ShapesParam)))
			{
				setClicksDone(getClicksDone() - 1);
			    return;
			}
		    Cshape[ShapSelctNum] = MAINDllOBJECT->highlightedShape();
			ShapSelctNum ++;
		}
		else if (MAINDllOBJECT->highlightedPoint() != NULL)
		{
			for(int i = 0; i < PointSelctNum; i++)
			{
		     	if (MAINDllOBJECT->highlightedPoint() == Cpoint[i])
			    {
				   MAINDllOBJECT->SetStatusCode("UCS006");
				   setClicksDone(getClicksDone() - 1);
			       return;
				}
			}
			Cpoint[PointSelctNum] = MAINDllOBJECT->highlightedPoint(); 
			HELPEROBJECT->getPointParam(Cpoint[PointSelctNum], &ShapesParam);
			PointSelctNum++;
		}
		else
		{
		    setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 3)
		{
			if(HELPEROBJECT->UCSBasePlaneTransformM(&ShapesParam, TransformMatrix))
			{
		        MAINDllOBJECT->AddingUCSModeFlag = true;
				UCS& cucs = MAINDllOBJECT->getCurrentUCS();
				UCS* nucs = new UCS(_T("UCS"));
				nucs->UCSMode(4);
				for(int i = 0; i < 16; i++) nucs->transform[i] = TransformMatrix[i];
				for (int i = 0; i < ShapSelctNum; i++) nucs->ShapesCol[i] = Cshape[i];
				for (int i = 0; i < PointSelctNum; i++) nucs->PointsCol[i] = Cpoint[i];
				nucs->SetUCSProjectionType(cucs.UCSProjectionType());
				nucs->getWindow(0).init(cucs.getWindow(0));
				nucs->getWindow(1).init(cucs.getWindow(1));
				nucs->getWindow(2).init(cucs.getWindow(2));
				AddUCSAction::addUCS(nucs);
				MAINDllOBJECT->AddingUCSModeFlag = false;
				nucs->getWindow(1).homeIt(1);
					
			    MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
				MAINDllOBJECT->updateAll();
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
			}
			else
			{
			    MAINDllOBJECT->SetStatusCode("UCS007");
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0006", __FILE__, __FUNCSIG__); }
}


void UCSPlaneHandle::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0007", __FILE__, __FUNCSIG__); }
}

//Max moue click.../
void UCSPlaneHandle::LmaxmouseDown()
{
	try
	{
		
	}
	catch(...){MAINDllOBJECT->AddingUCSModeFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0008", __FILE__, __FUNCSIG__); }
}

void UCSPlaneHandle::AddShape(map<int, Shape*> ShapesForBasePlane)
{
	try
	{
		list<list<double>>* ShpParmPntr = &ShapesParam;
		for(map<int, Shape*>::iterator ShpIt = ShapesForBasePlane.begin(); ShpIt != ShapesForBasePlane.end(); ShpIt++)
		{
			if(HELPEROBJECT->getShapeDimension(ShpIt->second, ShpParmPntr))
			{
				for(int i = 0; i < ShapSelctNum; i++)
				{
		     	   if (ShpIt->second->getId() == Cshape[i]->getId()) return;
				}
				Cshape[ShapSelctNum] = ShpIt->second; ShapSelctNum ++;
			}
		}
		memset(TransformMatrix, 0 , 16* sizeof(double));
		if(HELPEROBJECT->UCSBasePlaneTransformM(&ShapesParam, TransformMatrix))
		{
		    MAINDllOBJECT->AddingUCSModeFlag = true;
			UCS& cucs = MAINDllOBJECT->getCurrentUCS();
			UCS* nucs = new UCS(_T("UCS"));
			nucs->UCSMode(4);
			for(int i = 0; i < 16; i++) nucs->transform[i] = TransformMatrix[i];
			for (int i = 0; i < ShapSelctNum; i++) nucs->ShapesCol[Cshape[i]->getId()] = Cshape[i];
			for (int i = 0; i < PointSelctNum; i++) nucs->PointsCol[i] = Cpoint[i];
			nucs->UCSPoint.set(TransformMatrix[3], TransformMatrix[7], TransformMatrix[11]);
			nucs->SetUCSProjectionType(cucs.UCSProjectionType());
			nucs->getWindow(0).init(cucs.getWindow(0));
			nucs->getWindow(1).init(cucs.getWindow(1));
			nucs->getWindow(2).init(cucs.getWindow(2));
			AddUCSAction::addUCS(nucs);
			MAINDllOBJECT->AddingUCSModeFlag = false;
			nucs->getWindow(1).homeIt(1);
					
			MAINDllOBJECT->updateAll();
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		}
		else
		{
			MAINDllOBJECT->SetStatusCode("UCS007");
		}
	}
	catch(...){MAINDllOBJECT->AddingUCSModeFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0008", __FILE__, __FUNCSIG__); }
}

