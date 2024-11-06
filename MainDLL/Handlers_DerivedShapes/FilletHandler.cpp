#include "stdafx.h"
#include "FilletHandler.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
FilletHandler::FilletHandler()
{
	try
	{
		init();
		setMaxClicks(3);
		MAINDllOBJECT->SetStatusCode("FilletHandler03");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
FilletHandler::~FilletHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0002", __FILE__, __FUNCSIG__); }
}

void FilletHandler::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape1 != NULL)
			ParentShape1->HighlightedForMeasurement(false);
		if(ParentShape2 != NULL)
			ParentShape2->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20001", __FILE__, __FUNCSIG__); }
}

//Initialise the settings..
void FilletHandler::init()
{
	try
	{
		runningPartprogramValid = false;
		ParentShape1 = NULL; ParentShape2 = NULL;
		FilletShape = NULL;
		resetClicks();
		CurrentType = 0; minimumdia = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mosuemove..
void FilletHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down... Select the shapes..
void FilletHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE || Csh->ShapeType ==  RapidEnums::SHAPETYPE::ARC)
		{
			if(getClicksDone() == 1)
				ParentShape1 = Csh;
			else
			{
				if(ParentShape1->getId() == Csh->getId()) ////Wrong proceedure...
				{
					setClicksDone(getClicksDone() - 1);
					return;
				}
				ParentShape2 = Csh;
			}
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 2)
		{
			if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE && ParentShape2->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				CurrentType = 0;
				getLineDimensions(ParentShape1, 0); getLineDimensions(ParentShape2, 1);
			}
			else if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC && ParentShape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				CurrentType = 1;
				getArcDimensions(ParentShape1, 0); getArcDimensions(ParentShape2, 1);
				double d;
				d = RMATH2DOBJECT->Pt2Pt_distance(&Acenter1[0], &Acenter2[0]);
				if(d > (Aradius1 + Aradius2))
					minimumdia = d - (Aradius1 + Aradius2);
			}
			else if(compareboth<int>(ParentShape1->ShapeType, ParentShape2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ARC))
			{
				CurrentType = 2;
				if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
				{
					Shape* temp = ParentShape1;
					ParentShape1 = ParentShape2;
					ParentShape2 = temp;
				}
				getLineDimensions(ParentShape1, 0); getArcDimensions(ParentShape2, 0);
				double d;
				d = RMATH2DOBJECT->Pt2Line_Dist(Acenter1[0], Acenter1[1], Langle1, Lintercept1);
				if(d > Aradius1)
					minimumdia = d - Aradius1;
			}
			MAINDllOBJECT->DerivedShapeCallback();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0005", __FILE__, __FUNCSIG__); }
}

void FilletHandler::getLineDimensions(Shape* Csh, int cnt)
{
	try
	{
		if(cnt == 0)
		{
			Langle1 = ((Line*)Csh)->Angle();
			Lintercept1 = ((Line*)Csh)->Intercept();
			((Line*)Csh)->getPoint1()->FillDoublePointer(&LPoints11[0]);
			((Line*)Csh)->getPoint2()->FillDoublePointer(&LPoints12[0]);
		}
		else
		{
			Langle2 = ((Line*)Csh)->Angle();
			Lintercept2 = ((Line*)Csh)->Intercept();
			((Line*)Csh)->getPoint1()->FillDoublePointer(&LPoints21[0]);
			((Line*)Csh)->getPoint2()->FillDoublePointer(&LPoints22[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0006", __FILE__, __FUNCSIG__); }
}

void FilletHandler::getArcDimensions(Shape* Csh, int cnt)
{
	try
	{
		if(cnt == 0)
		{
			((Circle*)Csh)->getCenter()->FillDoublePointer(&Acenter1[0]);
			Aradius1 = ((Circle*)Csh)->Radius();
			Aangle1[0] = ((Circle*)Csh)->Startangle();
			Aangle1[1] = ((Circle*)Csh)->Sweepangle();
			((Circle*)Csh)->getendpoint1()->FillDoublePointer(&APoints11[0]);
			((Circle*)Csh)->getendpoint2()->FillDoublePointer(&APoints12[0]);
		}
		else
		{
			((Circle*)Csh)->getCenter()->FillDoublePointer(&Acenter2[0]);
			Aradius2 = ((Circle*)Csh)->Radius();
			Aangle2[0] = ((Circle*)Csh)->Startangle();
			Aangle2[1] = ((Circle*)Csh)->Sweepangle();
			((Circle*)Csh)->getendpoint1()->FillDoublePointer(&APoints11[0]);
			((Circle*)Csh)->getendpoint2()->FillDoublePointer(&APoints12[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0007", __FILE__, __FUNCSIG__); }
}

void FilletHandler::CalculateFilletForTwoLine()
{
	try
	{
		double FilletPoints[6];
		bool flag = RMATH2DOBJECT->FilletForTwoLines(Langle1, Lintercept1, &LPoints11[0], &LPoints12[0], Langle2, Lintercept2, &LPoints21[0], &LPoints22[0], radius, &FilletPoints[0]);
		if(flag)
		{
			PointCollection TempPtColl;
			if(radius != 0)
			{
				Shape* Csh = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				((Circle*)Csh)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				TempPtColl.Addpoint(new SinglePoint(FilletPoints[0], FilletPoints[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
				TempPtColl.Addpoint(new SinglePoint(FilletPoints[2], FilletPoints[3], ((Circle*)ParentShape1)->getCenter()->getZ()));
				TempPtColl.Addpoint(new SinglePoint(FilletPoints[4], FilletPoints[5], ((Circle*)ParentShape1)->getCenter()->getZ()));
				((ShapeWithList*)Csh)->AddPoints(&TempPtColl);
				AddShapeAction::addShape(Csh);
			}
			TempPtColl.deleteAll();
			((ShapeWithList*)ParentShape1)->ResetCurrentParameters();
			TempPtColl.Addpoint(new SinglePoint(LPoints11[0], LPoints11[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(LPoints12[0], LPoints12[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
			((ShapeWithList*)ParentShape1)->AddPoints(&TempPtColl);

			TempPtColl.deleteAll();
			((ShapeWithList*)ParentShape2)->ResetCurrentParameters();
			TempPtColl.Addpoint(new SinglePoint(LPoints21[0], LPoints21[1], ((Circle*)ParentShape2)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(LPoints22[0], LPoints22[1], ((Circle*)ParentShape2)->getCenter()->getZ()));
			((ShapeWithList*)ParentShape2)->AddPoints(&TempPtColl);
		}
		else
			MAINDllOBJECT->SetStatusCode("FilletHandler01");
		ResetShapeHighlighted();
		init();
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0008", __FILE__, __FUNCSIG__); }
}

void FilletHandler::CalculateFilletForTwoArc()
{
	try
	{
		double FilletArc[6], Acr1Points[6], Acr2Points[6];
		bool flag = RMATH2DOBJECT->FilletForTwoArcs(&Acenter1[0], Aradius1, Aangle1[0], Aangle1[1],  &Acenter2[0], Aradius2, Aangle2[0], Aangle2[1], radius, &Acr1Points[0], &Acr2Points[0], &FilletArc[0]);
		if(flag)
		{
			PointCollection TempPtColl;
			if(radius != 0)
			{
				Shape* Csh = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				((Circle*)Csh)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				TempPtColl.Addpoint(new SinglePoint(FilletArc[0], FilletArc[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
				TempPtColl.Addpoint(new SinglePoint(FilletArc[2], FilletArc[3], ((Circle*)ParentShape1)->getCenter()->getZ()));
				TempPtColl.Addpoint(new SinglePoint(FilletArc[4], FilletArc[5], ((Circle*)ParentShape1)->getCenter()->getZ()));
				((ShapeWithList*)Csh)->AddPoints(&TempPtColl);
				AddShapeAction::addShape(Csh);
			}
			TempPtColl.deleteAll();
			((ShapeWithList*)ParentShape1)->ResetCurrentParameters();
			((Circle*)ParentShape1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
			TempPtColl.Addpoint(new SinglePoint(Acr1Points[0], Acr1Points[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(Acr1Points[2], Acr1Points[3], ((Circle*)ParentShape1)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(Acr1Points[4], Acr1Points[5], ((Circle*)ParentShape1)->getCenter()->getZ()));
			((ShapeWithList*)ParentShape1)->AddPoints(&TempPtColl);

			TempPtColl.deleteAll();
			((ShapeWithList*)ParentShape2)->ResetCurrentParameters();
			((Circle*)ParentShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
			TempPtColl.Addpoint(new SinglePoint(Acr2Points[0], Acr2Points[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(Acr2Points[2], Acr2Points[3], ((Circle*)ParentShape1)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(Acr2Points[4], Acr2Points[5], ((Circle*)ParentShape1)->getCenter()->getZ()));
			((ShapeWithList*)ParentShape2)->AddPoints(&TempPtColl);
		}
		else
			MAINDllOBJECT->SetStatusCode("FilletHandler01");
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0009", __FILE__, __FUNCSIG__); }
}

void FilletHandler::CalculateFilletForOneLineOneArc()
{
	try
	{
		double Acr1Points[6], FilletPoints[6];
		bool flag = RMATH2DOBJECT->FilletForLineArc(Langle1, Lintercept1, &LPoints11[0], &LPoints12[0], &Acenter1[0], Aradius1, Aangle1[0], Aangle1[1], radius, &Acr1Points[0], &FilletPoints[0]);
		if(flag)
		{
			PointCollection TempPtColl;
			if(radius != 0)
			{
				Shape* Csh = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				((Circle*)Csh)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				TempPtColl.Addpoint(new SinglePoint(FilletPoints[0], FilletPoints[1], ((Circle*)ParentShape2)->getCenter()->getZ()));
				TempPtColl.Addpoint(new SinglePoint(FilletPoints[2], FilletPoints[3], ((Circle*)ParentShape2)->getCenter()->getZ()));
				TempPtColl.Addpoint(new SinglePoint(FilletPoints[4], FilletPoints[5], ((Circle*)ParentShape2)->getCenter()->getZ()));
				((ShapeWithList*)Csh)->AddPoints(&TempPtColl);
				AddShapeAction::addShape(Csh);
			}
			TempPtColl.deleteAll();
			((ShapeWithList*)ParentShape2)->ResetCurrentParameters();
			((Circle*)ParentShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
			TempPtColl.Addpoint(new SinglePoint(Acr1Points[0], Acr1Points[1], ((Circle*)ParentShape2)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(Acr1Points[2], Acr1Points[3], ((Circle*)ParentShape2)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(Acr1Points[4], Acr1Points[5], ((Circle*)ParentShape2)->getCenter()->getZ()));
			((ShapeWithList*)ParentShape2)->AddPoints(&TempPtColl);

			TempPtColl.deleteAll();
			((ShapeWithList*)ParentShape1)->ResetCurrentParameters();
			TempPtColl.Addpoint(new SinglePoint(LPoints11[0], LPoints11[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
			TempPtColl.Addpoint(new SinglePoint(LPoints12[0], LPoints12[1], ((Circle*)ParentShape1)->getCenter()->getZ()));
			((ShapeWithList*)ParentShape1)->AddPoints(&TempPtColl);
		}
		else
			MAINDllOBJECT->SetStatusCode("FilletHandler01");
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0010", __FILE__, __FUNCSIG__); }
}

//Set the defined diameter...
void FilletHandler::SetAnyData(double *data)
{
	try
	{
		double d = data[0];
		radius = d / 2;
		if(CurrentType == 0)
			CalculateFilletForTwoLine();
		else if(CurrentType == 1)
		{
			//radius = minimumdia / 2;
			if(d < minimumdia)
			{
				MAINDllOBJECT->SetStatusCode("FilletHandler02");
				MAINDllOBJECT->DerivedShapeCallback();
				ResetShapeHighlighted();
				init();
			}
			else
				CalculateFilletForTwoArc();
		}
		else if(CurrentType == 2)
		{
			//radius = minimumdia / 2;
			if(d < minimumdia)
			{
				MAINDllOBJECT->SetStatusCode("FilletHandler02");
				MAINDllOBJECT->DerivedShapeCallback();
				ResetShapeHighlighted();
				init();
			}
			else
				CalculateFilletForOneLineOneArc();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0011", __FILE__, __FUNCSIG__); }
}

//Draw the fillet arc..//
void FilletHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the escape button press..
void FilletHandler::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0013", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data.....//
void FilletHandler::PartProgramData()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0014", __FILE__, __FUNCSIG__); }
}

//Handle the max click...
//Set the  parallel arc parameters...
void FilletHandler::LmaxmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0015", __FILE__, __FUNCSIG__); }
}