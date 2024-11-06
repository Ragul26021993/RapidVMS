#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\Shapes\SinglePoint.h"
#include "CloudPoints3DStitching.h"
#include "..\Engine\PointCollection.h"

double TransformMatrix1[9] = {0}, TransformMatrix2[9] = {0}, InverseTransformMatrix1[9] = {0}, origin1[3] = {0}, origin2[3] = {0}; 

CloudPoints3DStitching::CloudPoints3DStitching()
{
	try
	{
		setMaxClicks(2);
	   /* MAINDllOBJECT->SetStatusCode("SelectionHandler01");*/
		RefrenceShape = NULL;
		ShapeTobeAdded = NULL;
		FirstShape = true;
		PointsRefCount = 0;
		PointsObjCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0001", __FILE__, __FUNCSIG__); }
}

CloudPoints3DStitching::~CloudPoints3DStitching()
{
	try
	{
	/*	if(PointsRef != NULL){delete [] PointsRef; PointsRef = NULL;}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0002", __FILE__, __FUNCSIG__); }
}

//For 1st click set drag = true...
void CloudPoints3DStitching::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).PanMode)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT)
		{
			if(MAINDllOBJECT->RcadWindow3DMode())
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		if(MAINDllOBJECT->getSelectedShapesList().getList().empty() || ((Shape*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second))->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
    	RefrenceShape = ((ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second));
        list<int> PtIdlist;
		double yDir[3] = {0}, wupp = MAINDllOBJECT->getWindow(1).getUppX();
		double topleft[3] = {PointsDRO[0].getX() - 40 * wupp, PointsDRO[0].getY() + 40 * wupp, 0}, bottomRight[2] = {PointsDRO[0].getX() + 40 * wupp, PointsDRO[0].getY() - 40 * wupp};
	//	MAINDllOBJECT->selectFGPoints(Vector(topleft[0], topleft[1], 0), Vector(bottomRight[0], bottomRight[1], 0), &PtIdlist, false, NULL);
		if(!PtIdlist.empty())
		{
			double leftBot[3] = {PointsDRO[0].getX() - 40 * wupp, PointsDRO[0].getY() - 40 * wupp, 0}, planeParam[5] = {0};
			MAINDllOBJECT->ShowFgPtIdList(&PtIdlist);
			for each(int id in PtIdlist)
			{
				PointsRef[PointsRefCount++] = RefrenceShape->PointsList->getList()[id]->X;
				PointsRef[PointsRefCount++] = RefrenceShape->PointsList->getList()[id]->Y;
				PointsRef[PointsRefCount++] = RefrenceShape->PointsList->getList()[id]->Z;
			}
			BESTFITOBJECT->Plane_BestFit(PointsRef, PointsRefCount/3, planeParam, MAINDllOBJECT->SigmaModeFlag());
			planeParam[3] = -planeParam[3];
			RMATH3DOBJECT->Projection_Point_on_Plane(leftBot, planeParam, origin1);
			RMATH3DOBJECT->Projection_Point_on_Plane(topleft, planeParam, yDir);
			for(int i = 0; i < 3; i++){yDir[i] -= origin1[i];}
			RMATH3DOBJECT->DirectionCosines(yDir, &TransformMatrix1[3]);
			for(int i = 0; i < 3; i++) {TransformMatrix1[6 + i] = planeParam[i];}
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&TransformMatrix1[3], &TransformMatrix1[6], &TransformMatrix1[0]);
		}
		else
		{
	    	setClicksDone(getClicksDone() - 1);
			return;
		}
	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0003", __FILE__, __FUNCSIG__); }
}

void CloudPoints3DStitching::mouseMove()
{
	try
	{
    	if(getClicksDone() < getMaxClicks())
			PointsDRO[0].set(getClicksValue(getClicksDone()));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0004", __FILE__, __FUNCSIG__); }
}

//Draw the rectangle till drag is true... till mouse up..!!
void CloudPoints3DStitching::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 1)
		{
			double wupp = WPixelWidth;
			if(getClicksDone() < getMaxClicks())
			{
				GRAFIX->SetColor_Double(1, 0, 0);
				GRAFIX->drawRectangle(PointsDRO[0].getX() - 40 * wupp, PointsDRO[0].getY() + 40 * wupp,  PointsDRO[0].getX() + 40 * wupp, PointsDRO[0].getY() - 40 * wupp);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0005", __FILE__, __FUNCSIG__); }
}

void CloudPoints3DStitching::LmouseUp(double x, double y)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0006", __FILE__, __FUNCSIG__); }
}

void CloudPoints3DStitching::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second->getId() != RefrenceShape->getId())
		{
		    ShapeTobeAdded = ((ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second));
		    list<int> PtIdlist;
			double yDir[3] = {0}, wupp = MAINDllOBJECT->getWindow(1).getUppX();
			double topleft[3] = {PointsDRO[0].getX() - 40 * wupp, PointsDRO[0].getY() + 40 * wupp, 0}, bottomRight[2] = {PointsDRO[0].getX() + 40 * wupp, PointsDRO[0].getY() - 40 * wupp};
		//	MAINDllOBJECT->selectFGPoints(Vector(topleft[0], topleft[1], 0), Vector(bottomRight[0], bottomRight[1], 0), &PtIdlist, false, NULL);
			if(!PtIdlist.empty())
			{
				double leftBot[3] = {PointsDRO[0].getX() - 40 * wupp, PointsDRO[0].getY() - 40 * wupp, 0}, planeParam[5] = {0};
				MAINDllOBJECT->ShowFgPtIdList(&PtIdlist);
				for each(int id in PtIdlist)
				{
				  PointsObj[PointsObjCount++] = ShapeTobeAdded->PointsList->getList()[id]->X;
				  PointsObj[PointsObjCount++] = ShapeTobeAdded->PointsList->getList()[id]->Y;
				  PointsObj[PointsObjCount++] = ShapeTobeAdded->PointsList->getList()[id]->Z;	 
				}
				BESTFITOBJECT->Plane_BestFit(PointsObj, PointsObjCount/3, planeParam, MAINDllOBJECT->SigmaModeFlag());
				planeParam[3] = -planeParam[3];
				RMATH3DOBJECT->Projection_Point_on_Plane(leftBot, planeParam, origin2);
				RMATH3DOBJECT->Projection_Point_on_Plane(topleft, planeParam, yDir);
				for(int i = 0; i < 3; i++){yDir[i] -= origin2[i];}
				RMATH3DOBJECT->DirectionCosines(yDir, &TransformMatrix2[3]);
				for(int i = 0; i < 3; i++) {TransformMatrix2[6 + i] = planeParam[i];}
				RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&TransformMatrix2[3], &TransformMatrix2[6], &TransformMatrix2[0]);
			}
			RMATH2DOBJECT->OperateMatrix4X4(TransformMatrix1, 3, 1, InverseTransformMatrix1);
			ShapeTobeAdded->Translate(Vector(-origin2[0], -origin2[1], -origin2[2]));
			ShapeTobeAdded->Transform(TransformMatrix2);
			ShapeTobeAdded->Transform(InverseTransformMatrix1);
			ShapeTobeAdded->Translate(Vector(origin1[0], origin1[1], origin1[2]));
		}
		else
		{
	    	setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0007", __FILE__, __FUNCSIG__); }

}

void CloudPoints3DStitching::RmouseDown(double x, double y)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0016", __FILE__, __FUNCSIG__); }
}

void CloudPoints3DStitching::MouseScroll(bool flag)
{
}

void CloudPoints3DStitching::EscapebuttonPress()
{
	try
	{
		if(MAINDllOBJECT->ShowHideFGPoints())
		{
			ShapeWithList *Cshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			if(Cshape == NULL) return;
			for(PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
				(*Item).second->IsSelected = false;
			Cshape->PointsList->SelectedPointsCnt = 0;
			MAINDllOBJECT->Shape_Updated();
			MAINDllOBJECT->SetStatusCode("Cleared Selection");
		}
		else
		{
			MAINDllOBJECT->getShapesList().clearSelection();
			MAINDllOBJECT->getDimList().clearSelection();
			MAINDllOBJECT->SetStatusCode("Cleared Selection");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0008", __FILE__, __FUNCSIG__); }
}