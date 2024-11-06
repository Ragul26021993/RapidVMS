#include "StdAfx.h"
#include "TrimShapeAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"

TrimShapeAction::TrimShapeAction():RAction(_T("TrimShapes"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::TRIMSHAPEACTION;
}

TrimShapeAction::~TrimShapeAction()
{
}

bool TrimShapeAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TRMSHP0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool TrimShapeAction::redo()
{
	try
	{
		Shape* Shp = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeId];
		if(Shp->ShapeType == RapidEnums::SHAPETYPE::LINE || Shp->ShapeType == RapidEnums::SHAPETYPE::XLINE || Shp->ShapeType == RapidEnums::SHAPETYPE::XRAY)
		{
			Shp->ShapeType = CurrentShapeType;
			((Line*)Shp)->LineType = CurrentLineType;
			((Line*)Shp)->setPoint1(Vector(CurrentEndPt1.getX(), CurrentEndPt1.getY(), CurrentEndPt1.getZ()));
			((Line*)Shp)->setPoint2(Vector(CurrentEndPt2.getX(), CurrentEndPt2.getY(), CurrentEndPt2.getZ()));
			((Line*)Shp)->calculateAttributes();
		}
		else if(Shp->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Shp->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			((Circle*)Shp)->ShapeType = CurrentShapeType;
			((Circle*)Shp)->CircleType = CurrentCircleType;
			double firstangle = RMATH2DOBJECT->ray_angle(((Circle*)Shp)->getCenter()->getX(), ((Circle*)Shp)->getCenter()->getY(), CurrentEndPt1.getX(), CurrentEndPt1.getY());
			double secondangle = RMATH2DOBJECT->ray_angle(((Circle*)Shp)->getCenter()->getX(), ((Circle*)Shp)->getCenter()->getY(), CurrentEndPt2.getX(), CurrentEndPt2.getY());
			double midangle = secondangle - firstangle;
			if(midangle < 0)
				midangle += 2 * M_PI;
			((Circle*)Shp)->Startangle(firstangle);
			((Circle*)Shp)->Sweepangle(midangle);
			midangle = firstangle + midangle/2;
			double ptonarc[2] = {((Circle*)Shp)->getCenter()->getX() + ((Circle*)Shp)->Radius() * cos(midangle), ((Circle*)Shp)->getCenter()->getY() + ((Circle*)Shp)->Radius() * sin(midangle)};
			((Circle*)Shp)->getMidPoint()->set(ptonarc[0], ptonarc[1]);
			((Circle*)Shp)->setendpoint1(Vector(CurrentEndPt1.getX(), CurrentEndPt1.getY(), CurrentEndPt1.getZ()));
			((Circle*)Shp)->setendpoint2(Vector(CurrentEndPt2.getX(), CurrentEndPt2.getY(), CurrentEndPt2.getZ()));
			((Circle*)Shp)->calculateAttributes();
		}
		Shp->notifyAll(ITEMSTATUS::DATACHANGED, Shp);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TRMSHP0002", __FILE__, __FUNCSIG__); return false; }
}

void TrimShapeAction::undo()
{
	try
	{
		Shape* Shp = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeId];
		if(Shp->ShapeType == RapidEnums::SHAPETYPE::LINE || Shp->ShapeType == RapidEnums::SHAPETYPE::XLINE || Shp->ShapeType == RapidEnums::SHAPETYPE::XRAY)
		{
			Shp->ShapeType = PreviousShapeType;
			((Line*)Shp)->LineType = PreviousLineType;
			((Line*)Shp)->setPoint1(Vector(PreviousEndPt1.getX(), PreviousEndPt1.getY(), PreviousEndPt1.getZ()));
			((Line*)Shp)->setPoint2(Vector(PreviousEndPt2.getX(), PreviousEndPt2.getY(), PreviousEndPt2.getZ()));
			((Line*)Shp)->calculateAttributes();
		}
		else if(Shp->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Shp->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			((Circle*)Shp)->ShapeType = PreviousShapeType;
			((Circle*)Shp)->CircleType = PreviousCircleType;
			double firstangle = RMATH2DOBJECT->ray_angle(((Circle*)Shp)->getCenter()->getX(), ((Circle*)Shp)->getCenter()->getY(), PreviousEndPt1.getX(), PreviousEndPt1.getY());
			double secondangle = RMATH2DOBJECT->ray_angle(((Circle*)Shp)->getCenter()->getX(), ((Circle*)Shp)->getCenter()->getY(), PreviousEndPt2.getX(), PreviousEndPt2.getY());
			double midangle = secondangle - firstangle;
			if(midangle < 0)
				midangle += 2 * M_PI;
			((Circle*)Shp)->Startangle(firstangle);
			((Circle*)Shp)->Sweepangle(midangle);
			midangle = firstangle + midangle/2;
			double ptonarc[2] = {((Circle*)Shp)->getCenter()->getX() + ((Circle*)Shp)->Radius() * cos(midangle), ((Circle*)Shp)->getCenter()->getY() + ((Circle*)Shp)->Radius() * sin(midangle)};
			((Circle*)Shp)->getMidPoint()->set(ptonarc[0], ptonarc[1]);
			((Circle*)Shp)->setendpoint1(Vector(PreviousEndPt1.getX(), PreviousEndPt1.getY(), PreviousEndPt1.getZ()));
			((Circle*)Shp)->setendpoint2(Vector(PreviousEndPt2.getX(), PreviousEndPt2.getY(), PreviousEndPt2.getZ()));
			((Circle*)Shp)->calculateAttributes();
		}
		Shp->notifyAll(ITEMSTATUS::DATACHANGED, Shp);
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TRMSHP0003", __FILE__, __FUNCSIG__); }
}

void TrimShapeAction::TrimShape(Vector &PrevEpoint1, Vector &PrevEpoint2, RapidEnums::SHAPETYPE PreSType, RapidEnums::LINETYPE PreLtype, Shape* currentShape)
{
	try
	{
		TrimShapeAction* tsa = new TrimShapeAction();
		tsa->PreviousEndPt1 = PrevEpoint1;
		tsa->PreviousEndPt2 = PrevEpoint2;
		tsa->PreviousShapeType = PreSType;
		tsa->PreviousLineType = PreLtype;
		tsa->ShapeId = currentShape->getId();
		tsa->CurrentShapeType = currentShape->ShapeType;
		tsa->CurrentLineType = ((Line*)currentShape)->LineType;
		tsa->CurrentEndPt1.set(((Line*)currentShape)->getPoint1()->getX(), ((Line*)currentShape)->getPoint1()->getY(), ((Line*)currentShape)->getPoint1()->getZ());
		tsa->CurrentEndPt2.set(((Line*)currentShape)->getPoint2()->getX(), ((Line*)currentShape)->getPoint2()->getY(), ((Line*)currentShape)->getPoint2()->getZ());				
		MAINDllOBJECT->addAction(tsa);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TRMSHP0004", __FILE__, __FUNCSIG__); }
}

void TrimShapeAction::TrimShape(Vector &PrevEpoint1, Vector &PrevEpoint2, RapidEnums::SHAPETYPE PreSType, RapidEnums::CIRCLETYPE preCtype, Shape* currentShape)
{
	try
	{
		TrimShapeAction* tsa = new TrimShapeAction();
		tsa->PreviousEndPt1 = PrevEpoint1;
		tsa->PreviousEndPt2 = PrevEpoint2;
		tsa->PreviousShapeType = PreSType;
		tsa->PreviousCircleType = preCtype;
		tsa->ShapeId = currentShape->getId();
		tsa->CurrentShapeType = currentShape->ShapeType;
		tsa->CurrentCircleType = ((Circle*)currentShape)->CircleType;
		tsa->CurrentEndPt1.set(((Circle*)currentShape)->getendpoint1()->getX(), ((Circle*)currentShape)->getendpoint1()->getY(), ((Circle*)currentShape)->getendpoint1()->getZ());
		tsa->CurrentEndPt2.set(((Circle*)currentShape)->getendpoint2()->getX(), ((Circle*)currentShape)->getendpoint2()->getY(), ((Circle*)currentShape)->getendpoint2()->getZ());	
		MAINDllOBJECT->addAction(tsa);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TRMSHP0005", __FILE__, __FUNCSIG__); }
}


