#include "stdafx.h"
#include "AutoAlignHandler.h"
#include "..\Actions\AutoAlignAction.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

//Constructor...
AutoAlignHandler::AutoAlignHandler()
{
	try
	{
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0001", __FILE__, __FUNCSIG__); }
}

AutoAlignHandler::AutoAlignHandler(bool SelectedShapes, int alignment_mode)
{
	try
	{
		setMaxClicks(1);
		DoAutoAlignment(SelectedShapes, alignment_mode);
		if(HELPEROBJECT->MultiFeatureScan)
		{
			if(MAINDllOBJECT->ShowMsgBoxString("AutoAlign0001", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
			{
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER;
				std::list<int> tmpIdList;
				for each(int id in HELPEROBJECT->MultiFeatureShapeList)
				{
					tmpIdList.push_back(id);
				}
				tmpIdList.erase(tmpIdList.begin());
				MAINDllOBJECT->selectShape(&tmpIdList);
				MAINDllOBJECT->DerivedShapeCallback();
			}
			else
			{
				MAINDllOBJECT->DeviationCallBack();
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
AutoAlignHandler::~AutoAlignHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0002", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void AutoAlignHandler::SelectPoints_Shapes()
{
	try
	{
		setClicksDone(getClicksDone() - 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0003", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void AutoAlignHandler::Align_Finished()
{
	try
	{		
		resetClicks();				
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0004", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void AutoAlignHandler::drawSelectedShapesOndxf()
{
	try
	{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0005", __FILE__, __FUNCSIG__); }
}

void AutoAlignHandler::drawSelectedShapeOnRCad()
{
	try
	{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0006", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void AutoAlignHandler::drawSelectedShapesOnvideo()
{
	try
	{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0007", __FILE__, __FUNCSIG__); }
}

 void AutoAlignHandler::Align_mouseMove(double x, double y)
{
	 try
	 { }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0008", __FILE__, __FUNCSIG__); }
}

 void AutoAlignHandler::HandlePartProgramData()
{
	try
	{
		int mode = ((AutoAlignAction*)PPCALCOBJECT->getCurrentAction())->getAlignmentMode();
		bool selectedFlag = ((AutoAlignAction*)PPCALCOBJECT->getCurrentAction())->getshapeSelectionFlag();
		std::list<int> Idlist;
		((AutoAlignAction*)PPCALCOBJECT->getCurrentAction())->getShapeIdList(&Idlist);
		//clear shape selection...
		MAINDllOBJECT->ClearShapeSelections();
		//select shapes for auoalignment........
		MAINDllOBJECT->selectShape(&Idlist);
		//do alignment.........
		DoAutoAlignment(selectedFlag, mode);
		PPCALCOBJECT->partProgramReached();
	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0009", __FILE__, __FUNCSIG__); }
}

 void AutoAlignHandler::DoAutoAlignment(bool SelectedShapes, int alignment_mode)
{
	try
	{
		std::list<int> Idlist;
		Curve_2DList Dxf_CurveList;
		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!(CShape->ShapeType == RapidEnums::SHAPETYPE::ARC || CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)) continue;
			Curve_2D Dxf_Curve2D;
			PtsList Dxf_pts_list;
			Idlist.push_back(CShape->getId());
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				Dxf_Curve2D.curve = Curves_2D::arc;
				Dxf_Curve2D.answer[0] = ((Circle*)CShape)->getCenter()->getX();
				Dxf_Curve2D.answer[1] = ((Circle*)CShape)->getCenter()->getY();
				Dxf_Curve2D.answer[2] = ((Circle*)CShape)->Radius();
				Dxf_Curve2D.answer[3] = ((Circle*)CShape)->Length();
				Dxf_Curve2D.answer[4] = ((Circle*)CShape)->Startangle();
				Dxf_Curve2D.answer[5] = ((Circle*)CShape)->Sweepangle();

				Pt Arc_StartPt;
				Arc_StartPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getendpoint1()->getX();
				Arc_StartPt.y = ((Circle*)CShape)->getendpoint1()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);

				Pt Arc_MidPt;
				Arc_MidPt.pt_index = 0;
				Arc_MidPt.x = ((Circle*)CShape)->getMidPoint()->getX();
				Arc_MidPt.y = ((Circle*)CShape)->getMidPoint()->getY();
				Dxf_pts_list.push_back(Arc_MidPt);

				Pt Arc_EndPt;
				Arc_EndPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getendpoint2()->getX();
				Arc_StartPt.y = ((Circle*)CShape)->getendpoint2()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				Dxf_Curve2D.curve = Curves_2D::line;
				Dxf_Curve2D.answer[0] = tan(((Line*)CShape)->Angle());
				Dxf_Curve2D.answer[1] = ((Line*)CShape)->Intercept();
				Dxf_Curve2D.answer[2] = ((Line*)CShape)->Length();

				Pt Line_StartPt;
				Line_StartPt.pt_index = 0;
				Line_StartPt.x = ((Line*)CShape)->getPoint1()->getX();
				Line_StartPt.y = ((Line*)CShape)->getPoint1()->getY();
				Dxf_pts_list.push_back(Line_StartPt);

				Pt Line_EndPt;
				Line_EndPt.pt_index = 0;
				Line_EndPt.x = ((Line*)CShape)->getPoint2()->getX();
				Line_EndPt.y = ((Line*)CShape)->getPoint2()->getY();
				Dxf_pts_list.push_back(Line_EndPt);
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				Dxf_Curve2D.curve = Curves_2D::arc;
				Dxf_Curve2D.answer[0] = ((Circle*)CShape)->getCenter()->getX();
				Dxf_Curve2D.answer[1] = ((Circle*)CShape)->getCenter()->getY();
				Dxf_Curve2D.answer[2] = ((Circle*)CShape)->Radius();
				Dxf_Curve2D.answer[3] = ((Circle*)CShape)->Length();
				Dxf_Curve2D.answer[4] = 0;
				Dxf_Curve2D.answer[5] = 2 * M_PI;

				Pt Arc_StartPt;
				Arc_StartPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getCenter()->getX() + ((Circle*)CShape)->Radius();
				Arc_StartPt.y = ((Circle*)CShape)->getCenter()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);

				Pt Arc_MidPt;
				Arc_MidPt.pt_index = 0;
				Arc_MidPt.x = ((Circle*)CShape)->getCenter()->getX() - ((Circle*)CShape)->Radius();
				Arc_MidPt.y = ((Circle*)CShape)->getCenter()->getY();
				Dxf_pts_list.push_back(Arc_MidPt);

				Pt Arc_EndPt;
				Arc_EndPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getCenter()->getX() + ((Circle*)CShape)->Radius();
				Arc_StartPt.y = ((Circle*)CShape)->getCenter()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);
			}
			Dxf_Curve2D.pts_list = Dxf_pts_list;
			Dxf_Curve2D.curve_index = 0;
			Dxf_CurveList.push_back(Dxf_Curve2D);
		}

		double *Pts_Array = NULL;
		int PtsCnt = 0;

		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS && !CShape->IsDxfShape())
				PtsCnt += CShape->PointsListOriginal->Pointscount();
		}

		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS && CShape->IsDxfShape() && CShape->PointsList->Pointscount() > 0)
			{
				int TmpCount = 0, SkipPtsCount = 0;
				if(CShape->PointsList->Pointscount() > 180)
					SkipPtsCount = CShape->PointsList->Pointscount() / 100;
				PtsList XYPtsCollection;
				for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
				{		
					if(TmpCount >= SkipPtsCount)
					{
						SinglePoint* Spt = (*SptItem).second;
						Pt Point_XY;
						Point_XY.pt_index = Spt->PtID;
						Point_XY.x = Spt->X;
						Point_XY.y = Spt->Y;
						XYPtsCollection.push_back(Point_XY);
						TmpCount = 0;
					}
					else
						TmpCount++;
				}
				BESTFITOBJECT->orderpoints(&XYPtsCollection);
				Idlist.push_back(CShape->getId());
				bool firstTime = true;
				double LastPt[2] = {0};
				for each(Pt Point_XY in XYPtsCollection)
				{
					if(firstTime)
					{
						LastPt[0] = Point_XY.x;
						LastPt[1] = Point_XY.y;
						firstTime = false;
					}
					else
					{
						double Tmp_Pt2[2] = {Point_XY.x, Point_XY.y}, Line_Param[3] = {0};
						if(HELPEROBJECT->GetLineParameter(LastPt, Tmp_Pt2, Line_Param))
						{
							Curve_2D Dxf_Curve2D;
							PtsList Dxf_pts_list;
							Dxf_Curve2D.curve = Curves_2D::line;
							Dxf_Curve2D.answer[0] = tan(Line_Param[1]);
							Dxf_Curve2D.answer[1] = Line_Param[2];
							Dxf_Curve2D.answer[2] = Line_Param[0];

							Pt Line_StartPt;
							Line_StartPt.pt_index = 0;
							Line_StartPt.x = LastPt[0];
							Line_StartPt.y = LastPt[1];
							Dxf_pts_list.push_back(Line_StartPt);

							Pt Line_EndPt;
							Line_EndPt.pt_index = 0;
							Line_EndPt.x = Tmp_Pt2[0];
							Line_EndPt.y = Tmp_Pt2[1];
							Dxf_pts_list.push_back(Line_EndPt);

							Dxf_Curve2D.pts_list = Dxf_pts_list;
							Dxf_Curve2D.curve_index = 0;
							Dxf_CurveList.push_back(Dxf_Curve2D);
						}
						LastPt[0] = Point_XY.x;
						LastPt[1] = Point_XY.y;
					}
				}
			}
		}

		//if no of points greater than 0 then assign memory ot points array..........
		if(PtsCnt > 0)
			Pts_Array = new double[2 * PtsCnt];
		else
			return;

		int indx = 0;
		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS && !CShape->IsDxfShape())
			{			
				Idlist.push_back(CShape->getId());
				for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
				{
					SinglePoint* Spt = (*SptItem).second;
					Pts_Array[indx++] = Spt->X;
					Pts_Array[indx++] = Spt->Y;
				}		
			}
		}			

		double Shift_X = 0, Shift_Y = 0, R_Theta = 0, RotationPt[2] = {0};
		//DWORD d1, d2, d3;
		//d1 = GetTickCount();
		bool AlignmentFlag = BESTFITOBJECT->BFalignment_2dpts_with_lines_arcs(Pts_Array, PtsCnt, Dxf_CurveList, &Shift_X, &Shift_Y, &R_Theta, &RotationPt[0], MAINDllOBJECT->BestFitAlignment_Resolution, MAINDllOBJECT->BestFitAlignment_SampleSize, alignment_mode);
		//d2 = GetTickCount();
		//d3= d2-d1;
		if(Pts_Array != NULL)
			delete [] Pts_Array;
		if(!AlignmentFlag)return;
		//create action for alignment if partprogram is not running........
		if(!PPCALCOBJECT->IsPartProgramRunning())
			AutoAlignAction::SetAutoAlignAction(&Idlist, SelectedShapes, alignment_mode);
		R_Theta = -R_Theta;	Shift_X = -Shift_X;	Shift_Y = -Shift_Y;
		double RotateMatrix[9] = {0};
		RotateMatrix[0] = cos(R_Theta); RotateMatrix[1] = -sin(R_Theta); 
		RotateMatrix[3] = sin(R_Theta); RotateMatrix[4] = cos(R_Theta); 
		RotateMatrix[8] = 1;

		Vector translate;
		translate.set(Shift_X, Shift_Y);

		Vector GotoOrigin;
		GotoOrigin.set(-RotationPt[0], -RotationPt[1]);

		Vector GotoRotationPts;
		GotoRotationPts.set(RotationPt[0], RotationPt[1]);

		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->IsDxfShape()) continue;
			if(SelectedShapes)
			{
				if(!CShape->selected()) continue;
			}
			switch(alignment_mode)
			{
			case 0://translate and rotate both...		
				CShape->Translate(GotoOrigin);
				CShape->Transform(RotateMatrix);
				CShape->Translate(GotoRotationPts);
				CShape->Translate(translate);
				break;
			case 1://translate only..
				CShape->Translate(translate);
				break;
			case 2://rotate only...
				CShape->Translate(GotoOrigin);
				CShape->Transform(RotateMatrix);
				CShape->Translate(GotoRotationPts);
				break;
			}
		}
		//}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0010", __FILE__, __FUNCSIG__); }
}

 