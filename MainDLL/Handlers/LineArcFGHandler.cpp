#include "StdAfx.h"
#include "LineArcFGHandler.h"
#include "..\Actions\AddPointAction.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\LineArcAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Default Constructor..//
LineArcFGHandler::LineArcFGHandler(map<int, AddPointAction*> ptactcoll)
{
	try
	{
		valid = false;
		pointactioncoll = ptactcoll;
		init();
		valid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...// Release Memory..
LineArcFGHandler::~LineArcFGHandler()
{
	try
	{
		AddPointAction* action;
		int z = pointactioncoll.size();
		for (int i = 0; i < z; i++)
		{
			action = pointactioncoll[z - i];
			pointactioncoll.erase(z - i);
			if (!((action->getFramegrab())->isLineArcFirstPtAction()))
				delete action;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0004", __FILE__, __FUNCSIG__); }
}

//Initiallise Default Settings.. Flags, Mouse clicks required etc..
void LineArcFGHandler::init()
{
	try
	{
		valid = false;
		resetClicks();
		fgHighlighted = false;
		selectedPointAction = NULL;
		highlightedPointAction = NULL;
		setMaxClicks(1);
		linearcfirstptactiondone = false;
		valid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the left mouse down..
void LineArcFGHandler::LmouseDown()
{
	try
	{

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0007", __FILE__, __FUNCSIG__); }
}

//Handle mouse move
void LineArcFGHandler::mouseMove()
{
	try
	{
		if (!valid) return;
		if (linearcfirstptactiondone)
			return;
		Vector* pt = &getClicksValue(getClicksDone());
		fgHighlighted = false; 
		highlightedPointAction = NULL;
		double snapdist;
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			snapdist = MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX();
		}
		else
		{
			snapdist = MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX() * SNAPDIST;
		}

		AddPointAction* npa = getNearestPA(pt->getX(), pt->getY(),  snapdist);
		if(npa != NULL)
		{
			highlightedPointAction = npa;
			fgHighlighted = true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0009", __FILE__, __FUNCSIG__); }
}

//Check is there any framegrab near to the mouse position..
AddPointAction* LineArcFGHandler::getNearestPA(double x, double y, double snapdist)
{
	try
	{
		FramegrabBase* Cfg = NULL;
		double pt[2] = {x, y};
		double pt_proj[2] = {0};
		bool flag1;
		double endpt1[2];
		double endpt2[2];
		double midpt[2];
		for(int i = 0; i < pointactioncoll.size(); i++)
		{
			Cfg = pointactioncoll[i]->getFramegrab();
			(Cfg->points[0]).FillDoublePointer(endpt1);
			(Cfg->points[1]).FillDoublePointer(endpt2);
			(Cfg->points[2]).FillDoublePointer(midpt);
			if (Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
			{
				double slope = 0;
				double intercept = 0;
				if ((Cfg->points[1]).getX() - (Cfg->points[0]).getX() == 0)
				{
					slope = M_PI_2;
					if ((Cfg->points[1]).getY() > (Cfg->points[0]).getY())
						slope = - M_PI_2;
					intercept = (Cfg->points[0]).getX();
				}
				else
				{
					slope = atan(((Cfg->points[1]).getY() - (Cfg->points[0]).getY())/((Cfg->points[1]).getX() - (Cfg->points[0]).getX()));
					intercept = (Cfg->points[0]).getY() - tan (slope) * (Cfg->points[0]).getX();
				}

				if (RMATH2DOBJECT->Pt2Line_Dist(x, y, slope, intercept) < Cfg->FGWidth)
				{
					RMATH2DOBJECT->Point_PerpenIntrsctn_Line(slope, intercept, pt, pt_proj);
					if (RMATH2DOBJECT->MousePt_OnFiniteline(endpt1, endpt2, slope, intercept, pt_proj, snapdist, &flag1))
						return (pointactioncoll[i]);
				}
			}
			else
			{
				double center[2], radius, startang, sweepang;
				RMATH2DOBJECT->Arc_3Pt(endpt1, endpt2, midpt, center, &radius, &startang, &sweepang);
				if (RMATH2DOBJECT->Pt2Circle_distance_Type(pt, center, radius, 0) < Cfg->FGWidth)
				{
					RMATH2DOBJECT->Point_onCircle(center, radius, pt, pt_proj);
					if (RMATH2DOBJECT->MousePt_OnArc(center, radius, startang, sweepang, endpt1, endpt2, snapdist, pt_proj, &flag1))
						return (pointactioncoll[i]);
				}
			}
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0006", __FILE__, __FUNCSIG__); return NULL; }
}
//Handle draw..
void LineArcFGHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(!valid) return;
		FramegrabBase* Cfg = NULL;
		FramegrabBase* Cfghigh = NULL;
		FramegrabBase* Cfgsel = NULL;
		if (selectedPointAction != NULL)
		{
			Cfgsel = selectedPointAction->getFramegrab();
			Cfghigh = Cfgsel;
		}
		else if (highlightedPointAction != NULL)
			Cfghigh = highlightedPointAction->getFramegrab();
		for(int i = 0; i < pointactioncoll.size(); i++)
		{
			Cfg = pointactioncoll[i]->getFramegrab();
			if (Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
			{
				FrameGrabAngularRect cfgrect(Cfg);
				if (Cfghigh != NULL)
				{
					if (Cfg == Cfghigh)
					{
						if (Cfgsel == NULL)
							cfgrect.fgangrect_draw(3*WPixelWidth);
						else
							cfgrect.fgangrect_draw(3*WPixelWidth, true);
					}
					else
						cfgrect.fgangrect_draw(WPixelWidth);
				}
				else
					cfgrect.fgangrect_draw(WPixelWidth);
			}
			else
			{
				FrameGrabArc cfgarc(Cfg);
				if (Cfghigh != NULL)
				{
					if (Cfg == Cfghigh)
					{
						if (Cfgsel == NULL)
							cfgarc.fgarc_draw(3*WPixelWidth);
						else
							cfgarc.fgarc_draw(3*WPixelWidth, true);
					}
					else
						cfgarc.fgarc_draw(WPixelWidth);
				}
				else
					cfgarc.fgarc_draw(WPixelWidth);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0010", __FILE__, __FUNCSIG__); }
}

//Hanlde the Left mouse down(Max mouse down...)
void LineArcFGHandler::LmaxmouseDown()
{
	try
	{
		if (linearcfirstptactiondone)
			return;
		if (highlightedPointAction == NULL)
		{
			setClicksDone(0);
			return;
		}
		//first add the selected framegrab point action
		selectedPointAction = highlightedPointAction;
		selectedPointAction->CurrentActionType = RapidEnums::ACTIONTYPE::ADDPOINTACTION;
		selectedPointAction->ActionStatus(true);
		(selectedPointAction->getShape())->PointAtionList.push_back(selectedPointAction);
		//set this flag to true so that the point action does not get added again during addLineArcAction() below
		(selectedPointAction->getFramegrab())->isLineArcFirstPtAction(true);
		MAINDllOBJECT->addAction(selectedPointAction);

		//now add remaining framegrab point actions created by line-arc action
		for(RC_ITER ActItem = MAINDllOBJECT->getActionsHistoryList().getList().begin(); ActItem != MAINDllOBJECT->getActionsHistoryList().getList().end(); ActItem++)
		{
			RAction* CurrentAction = (RAction*)((*ActItem).second);
			if(!CurrentAction->ActionStatus()) continue;
			if(CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDLINEARCPFACTION)
			{
				if(!((LineArcAction*)CurrentAction)->FramegrabactionAdded)
				{
					((LineArcAction*)CurrentAction)->addLineArcAction();
					((LineArcAction*)CurrentAction)->FramegrabactionAdded = true;
				}
			}
		}
		linearcfirstptactiondone = true;
		PPCALCOBJECT->FinishedFirstPointActionIdentification();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the Escape button Press.. delete the current drawing..reset the modes..
void LineArcFGHandler::EscapebuttonPress()
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARCFGMH0013", __FILE__, __FUNCSIG__); }
}