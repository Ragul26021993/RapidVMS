#pragma once
#include "StdAfx.h"
#include "ShapeWithList.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\IsectPtCollection.h"
#include "..\Engine\PointCollection.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\FocusFunctions.h"
#include "..\Engine\CalibrationCalculation.h"
#include "..\Helper\Helper.h"
#include <process.h>
#include "..\Engine\PartProgramFunctions.h"
#include "Line.h"
#include "Circle.h"

ShapeWithList::ShapeWithList(TCHAR* myname):Shape(myname)
{
	init();
}

ShapeWithList::ShapeWithList(bool simply):Shape(false)
{
	init();
}

ShapeWithList::~ShapeWithList()
{
	try
	{
		if(pts != NULL){free(pts); pts = NULL;}
		if(filterpts != NULL){free(filterpts); filterpts = NULL;}
		IsectPointsList->EraseAll();
		delete SnapPointList;
		delete PointsList;
		delete PointsListOriginal;
		delete RemovedPointsList;
		delete RemovedPointsListOriginal;
		delete IsectPointsList;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0002", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::init()
{
	this->pts = NULL;
	this->filterpts = NULL;
	this->SkipPtscount = 0;
	this->Stepcount = 0;
	this->RAxisVal = 0;
	SnapPointList = new SnapPointCollection();
	PointsList = new PointCollection();
	PointsListOriginal = new PointCollection();
	RemovedPointsList = new PointCollection();
	RemovedPointsListOriginal = new PointCollection();
	IsectPointsList = new IsectPtCollection();
	CurrentShapeAddAction = NULL;
}

void ShapeWithList::ManagePoint(FramegrabBase* CurrentFG, PointOperations pointops)
{
	try
	{	
		/*if(this->ShapeType == RapidEnums::SHAPETYPE::SPLINE && CurrentFG->AllPointsList.Pointscount() > 1)
		{
			MAINDllOBJECT->SetStatusCode("ShapeWithList001");
			return;
		}*/
		if(this->SkipPtscount < this->Stepcount)
		{
			for(int i = 0; i < this->Stepcount; i++)
			{
				if(this->SkipPtscount == this->Stepcount || CurrentFG->AllPointsList.Pointscount() < 1) break;
				writePointsToHarddisc(CurrentFG);
				this->SkipPtscount++;
			}
		}
		if(CurrentFG->AllPointsList.Pointscount() < 1) return;
		if(this->SkipPtscount == this->Stepcount)
		{
			this->SkipPtscount = 0;
			switch(pointops)
			{
				case ADD_POINT:	//Add the points to the list..//
					if((MAINDllOBJECT->getCurrentUCS().UCSMode() == 4) && (CurrentFG->CurrentWindowNo == 0))
					{
						 int Order1[2] = {4, 4}, Order2[2] = {4, 1};
						 for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
						 {
							  SinglePoint* Sp = (*Spt).second;
							  double temp1[4], temp2[4] = {Sp->X, Sp->Y, Sp->Z, 1}; 
							  RMATH2DOBJECT->MultiplyMatrix1(&MAINDllOBJECT->getCurrentUCS().transform[0], Order1, temp2, Order2, temp1);
							  this->PointsListOriginal->Addpoint(new SinglePoint(temp1[0], temp1[1], temp1[2], 0, Sp->Pdir, Sp->PLR));
							  this->PointsList->Addpoint(new SinglePoint(temp1[0], temp1[1], temp1[2], 0, Sp->Pdir, Sp->PLR));
							  CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
						 }
					}
					else
					{
						if(this->ShapeAs3DShape())
						{
							 if(CurrentFG->DontCheckProjectionType() || CurrentFG->CurrentWindowNo == 1 || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY || (CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG) || (CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
								|| (CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN) || (CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH) || CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RCADPOINTS_FG ||
								(CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX) || (CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN) || (CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO))
							 {
								for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							 }
							 else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
							 {
								for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Z, Sp->Y, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Z, Sp->Y, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							 }
							 else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
							 {
								for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->Y, Sp->Z, Sp->X, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->Y, Sp->Z, Sp->X, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							 }
							 else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
							 {
								 for (PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								 {
									 SinglePoint* Sp = (*Spt).second;
									 this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, -Sp->Z, Sp->Y, Sp->R, Sp->Pdir, Sp->PLR));
									 this->PointsList->Addpoint(new SinglePoint(Sp->X, -Sp->Z, Sp->Y, Sp->R, Sp->Pdir, Sp->PLR));
									 CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								 }
							 }
							 else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
							 {
								 for (PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								 {
									 SinglePoint* Sp = (*Spt).second;
									 this->PointsListOriginal->Addpoint(new SinglePoint(Sp->Y, -Sp->Z, Sp->X, Sp->R, Sp->Pdir, Sp->PLR));
									 this->PointsList->Addpoint(new SinglePoint(Sp->Y, -Sp->Z, Sp->X, Sp->R, Sp->Pdir, Sp->PLR));
									 CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								 }
							 }
						}
						else
						{
							if(CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG || 
								CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT || 
								CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN || 
								CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH || 
								CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RCADPOINTS_FG ||
								CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX || 
								CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN || 
								CurrentFG->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO)
							{
								double Zlevel = Get2DShapeZLevel(this->PointsListOriginal, &CurrentFG->AllPointsList, 2);
								for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Zlevel, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Zlevel, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							}
							else if (CurrentFG->DontCheckProjectionType() || 
									 CurrentFG->CurrentWindowNo == 1 || 
									 MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
							{
								for (PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							}
							else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
							{
								double Ylevel = Get2DShapeZLevel(this->PointsListOriginal, &CurrentFG->AllPointsList, 1);
								
								for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Z, Ylevel, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Z, Ylevel, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							}
							else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
							{
								double Xlevel = Get2DShapeZLevel(this->PointsListOriginal, &CurrentFG->AllPointsList, 0);
								
								for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->Y, Sp->Z, Xlevel, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->Y, Sp->Z, Xlevel, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							}
							else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
							{
								double Ylevel = Get2DShapeZLevel(this->PointsListOriginal, &CurrentFG->AllPointsList, 1);

								for (PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, -Sp->Z, Ylevel, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->X, -Sp->Z, Ylevel, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							}
							else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
							{
								double Xlevel = Get2DShapeZLevel(this->PointsListOriginal, &CurrentFG->AllPointsList, 0);

								for (PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
								{
									SinglePoint* Sp = (*Spt).second;
									this->PointsListOriginal->Addpoint(new SinglePoint(Sp->Y, -Sp->Z, Xlevel, Sp->R, Sp->Pdir, Sp->PLR));
									this->PointsList->Addpoint(new SinglePoint(Sp->Y, -Sp->Z, Xlevel, Sp->R, Sp->Pdir, Sp->PLR));
									CurrentFG->PointActionIdList.push_back(this->PointsListOriginal->index - 1);
								}
							}
						}
					}
					CurrentFG->noofpts = CurrentFG->AllPointsList.getList().size();
					if(PPCALCOBJECT->IsPartProgramRunning() && CurrentFG->FGtype != RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
					{
						if(CurrentFG->noofpts == 0)
							CurrentFG->PartprogrmActionPassStatus(false);
						else if(CurrentFG->noofpts < CurrentFG->noofptstaken_build/15)
							CurrentFG->PartprogrmActionPassStatus(false);
					}
					CurrentFG->AllPointsList.deleteAll();
					if(this->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && this->getId() <= MAINDllOBJECT->ToolAxisLineId)
					{
						for each(BaseItem* act in this->PointAtionList)
						{
							((RAction*)act)->DontSaveInPartProgram(true);
							//MAINDllOBJECT->getActionsHistoryList().removeItem(act, true, true);
						}
						//this->PointAtionList.clear();
					}
					break;
				case REMOVE_POINT:	// Remove the points from the list...//
					for(PC_ITER Spt = CurrentFG->AllPointsList.getList().begin(); Spt != CurrentFG->AllPointsList.getList().end(); Spt++)
						this->PointsListOriginal->deletePoint((*Spt).second->PtID);
					break;
			}
			RAxisVal = CurrentFG->PointDRO.getR();
			DotheShapeUpdate();
			if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ROTARY_DEL_FI)
			{
				if(this->ShapeType == RapidEnums::SHAPETYPE::LINE || this->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
				{
					double RotaryAxisYvalue = MAINDllOBJECT->getCurrentDRO().getY();
					if (MAINDllOBJECT->ReferenceDotShape != NULL)
					{
						RotaryAxisYvalue = ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getY() + MAINDllOBJECT->Rotary_YaxisOffset();
					}
					Vector* mp = ((Line*)this)->getMidPoint();
					double Deviation = (((Line*)this)->getMidPoint()->getY() - RotaryAxisYvalue) * MAINDllOBJECT->RotaryCalibrationAngleValue() + MAINDllOBJECT->RotaryCalibrationIntercept();
					RAxisVal = RAxisVal + Deviation;
				}
			}
		}
		if(!MAINDllOBJECT->DoNotSamplePts)
			this->GetDownSampledPoints();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0003", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::Modify_Points(FramegrabBase* CurrentFG, PointOperations pointops)
{
	try
	{	
		switch(pointops)
		{
			case ADD_POINT:	//Add the points to the list..//
				for each(int Ptid in  CurrentFG->PointActionIdList)
				{
					this->PointsListOriginal->SetPoint(this->RemovedPointsListOriginal->getList()[Ptid], Ptid);
					this->PointsList->SetPoint(this->RemovedPointsList->getList()[Ptid], Ptid);
					this->RemovedPointsListOriginal->ErasePoint(Ptid);
					this->RemovedPointsList->ErasePoint(Ptid);
				}
				break;
			
			case REMOVE_POINT:	// Remove the points from the list...//
				for each(int Ptid in  CurrentFG->PointActionIdList)
				{
					this->RemovedPointsList->SetPoint(this->PointsList->getList()[Ptid], Ptid);
					this->RemovedPointsListOriginal->SetPoint(this->PointsListOriginal->getList()[Ptid], Ptid);
					this->PointsListOriginal->ErasePoint(Ptid);
					this->PointsList->ErasePoint(Ptid);
				}
				break;
		}
		DotheShapeUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0004", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::AddPoints(PointCollection* PtsList)
{
	try
	{	
		/*if(this->ShapeType == RapidEnums::SHAPETYPE::SPLINE && PtsList->Pointscount() > 1)
		{
			MAINDllOBJECT->SetStatusCode("ShapeWithList001");
			return;
		}*/
		if(this->SkipPtscount < this->Stepcount)
		{
			for(int i = 0; i < this->Stepcount; i++)
			{
				if(this->SkipPtscount == this->Stepcount || PtsList->Pointscount() < 1) break;
				writePointsToHarddisc(PtsList);
				this->SkipPtscount++;
			}
		}
		if(PtsList->Pointscount() < 1) return;
		if(this->SkipPtscount == this->Stepcount)
		{
			this->SkipPtscount = 0;
			if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 4)
			{
				int Order1[2] = {4, 4}, Order2[2] = {4, 1};
				for(PC_ITER Item = PtsList->getList().begin(); Item != PtsList->getList().end(); Item++)
				{     
					  SinglePoint* Sp = (*Item).second;
					  double temp1[4], temp2[4] = {Sp->X, Sp->Y, Sp->Z, 1}; 
					  RMATH2DOBJECT->MultiplyMatrix1(&MAINDllOBJECT->getCurrentUCS().transform[0], Order1, temp2, Order2, temp1);
					  int ProbeDir = RMATH2DOBJECT->GetTransformedProbeDirection(&MAINDllOBJECT->getCurrentUCS().transform[0], Sp->Pdir);
					  this->PointsListOriginal->Addpoint(new SinglePoint(temp1[0], temp1[1], temp1[2], 0, ProbeDir, Sp->PLR));
					  this->PointsList->Addpoint(new SinglePoint(temp1[0], temp1[1], temp1[2], 0, ProbeDir, Sp->PLR));
				}
			}
			else
			{
				if(this->ShapeAs3DShape())
				{
					for(PC_ITER Item = PtsList->getList().begin(); Item != PtsList->getList().end(); Item++)
					{
						SinglePoint* Sp = (*Item).second;
						this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR));
						this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR));
					}
				}
				else
				{
					double Zlevel = 0;
					if(this->PointsListOriginal->getList().empty())
					{
						if(!PtsList->getList().empty())
							Zlevel = PtsList->getList().begin()->second->Z;
					}
					else
						Zlevel = this->PointsListOriginal->getList().begin()->second->Z;
					for(PC_ITER Spt = PtsList->getList().begin(); Spt !=  PtsList->getList().end(); Spt++)
					{
						SinglePoint* Sp = (*Spt).second;
						this->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Zlevel, Sp->R, Sp->Pdir, Sp->PLR));
						this->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Zlevel, Sp->R, Sp->Pdir, Sp->PLR));
					}
				}
			}
			DotheShapeUpdate();
		}
		if(!MAINDllOBJECT->DoNotSamplePts)
			this->GetDownSampledPoints();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0005", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::Delete_Points(list<int> *PtIdList)
{
	try
	{	
		for each(int Ptid in  *PtIdList)
		{
			this->PointsListOriginal->deletePoint(Ptid);
			this->PointsList->deletePoint(Ptid);
		}
		DotheShapeUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0006", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::Modify_Point(int id, double x, double y, double z)
{
	try
	{
		SinglePoint* Spt = this->PointsListOriginal->getList()[id];
		SinglePoint* Spt1 = this->PointsList->getList()[id];
		if(this->ShapeAs3DShape())
		{
			Spt->SetValues(x, y, z);
			Spt1->SetValues(x, y, z);
		}
		else
		{
			Spt->SetValues(x, y, 0);
			Spt1->SetValues(x, y, z);
		}
		UpdateShape(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0007", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::UpdateShape(bool notifyall)
{
	try
	{
		if(this->pts != NULL){free(pts); pts = NULL;}
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		//if(this->pts != NULL){delete pts; pts = NULL;}
		if (this->PointsListOriginal->Pointscount() > 0)
		{
			if(this->ApplyPixelCorrection())
			   FOCUSCALCOBJECT->DoAverageOfNearByPoints(this);
			else if(this->ShapeAs3DShape())
			{
				this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
				filterptsCnt = 0;
				int n = 0, j = 0;
				for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					this->pts[n++] = Pt->X;
					this->pts[n++] = Pt->Y;
					this->pts[n++] = Pt->Z;
					this->filterpts[j++] = Pt->X;
					this->filterpts[j++] = Pt->Y;
					this->filterpts[j++] = Pt->Z;
					filterptsCnt++;				
				}
							
			}
			else
			{
				this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
				//this->pts = new double[this->PointsListOriginal->Pointscount() * 2];
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
				filterptsCnt = 0;
				int n = 0, j = 0;
				for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					this->pts[n++] = Pt->X;
					this->pts[n++] = Pt->Y;
					this->filterpts[j++] = Pt->X;
					this->filterpts[j++] = Pt->Y;
					filterptsCnt++;
				}
			}
		}
		this->UpdateBestFit();
		//if(this->IsUsedForReferenceDot())
		if(this->getId() == MAINDllOBJECT->ToolAxisLineId - 1)
			this->UpdateHomePostionFromReferenceDotShape();
		this->CalculateEnclosedRectangle();
		if(notifyall && this->getGroupParent().empty() && this->getGroupChilds().empty())
			this->notifyAll(ITEMSTATUS::DATACHANGED, this);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0008", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::CalculateEnclosedRectangle()
{
	try
	{
		if(ShapeAs3DShape())
			RMATH2DOBJECT->GetTopLeftAndRightBottom(this->pts, this->PointsList->Pointscount(), 3, &FgPointLeftTop[0], &FgPointsRightBottom[0]);
		else
			RMATH2DOBJECT->GetTopLeftAndRightBottom(this->pts, this->PointsList->Pointscount(), 2, &FgPointLeftTop[0], &FgPointsRightBottom[0]);
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0008", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::ResetCurrentParameters()
{
	try
	{
		deleteAllPoints();
		this->PointAtionList.clear();
		this->ResetShapeParameters();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0009", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::ResetCurrentPointsList()
{
	try
	{
		deleteAllPoints();
		this->ResetShapeParameters();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0010", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::deleteAllPoints()
{
	try
	{
		this->IsValid(false);
		this->PointsListOriginal->deleteAll();
		this->PointsList->deleteAll();
		this->RemovedPointsList->deleteAll();
		if(pts != NULL){free(pts); pts = NULL;}
		if(filterpts != NULL){free(filterpts); filterpts = NULL;}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0011", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::drawFGpoints()
{
	try
	{
		int selectedcnt = this->PointsList->SelectedPointsCnt;
		int totcount = this->PointsList->Pointscount();
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX(), upp_y = MAINDllOBJECT->getWindow(0).getUppY();
		double HalfScreenX = MAINDllOBJECT->getWindow(0).getWinDim().x / 2 * upp_x, HalfScreenY = MAINDllOBJECT->getWindow(0).getWinDim().y / 2 * upp_y;
		if(totcount < 1) return;
		if(selectedcnt > 0)
		{
			double* pts1  = (double*)malloc(sizeof(double) * selectedcnt * 3);
			double* pts2  = (double*)malloc(sizeof(double) * (totcount - selectedcnt) * 3);
			int i = 0, j = 0;
			for(PC_ITER Item = this->PointsList->getList().begin(); Item != this->PointsList->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->InValid) 			
				{
					continue;
				}
				if(Spt->IsSelected)
				{
					pts1[i++] = Spt->X;
					pts1[i++] = Spt->Y;
					pts1[i++] = Spt->Z;
					if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && !CALBCALCOBJECT->CalibrationModeFlag() && MAINDllOBJECT->UseLocalisedCorrection)
					{
						pts1[i - 3] += HalfScreenX; pts1[i - 2] += HalfScreenY;
						BESTFITOBJECT->correctXYCoord2(&pts1[i - 3], &pts1[i - 2], true);
						pts1[i - 3] -= HalfScreenX; pts1[i - 2] -= HalfScreenY;
					}
				}
				else
				{
					pts2[j++] = Spt->X;
					pts2[j++] = Spt->Y;
					pts2[j++] = Spt->Z;
					if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && !CALBCALCOBJECT->CalibrationModeFlag() && MAINDllOBJECT->UseLocalisedCorrection)
					{
						pts2[i - 3] += HalfScreenX; pts2[i - 2] += HalfScreenY;
						BESTFITOBJECT->correctXYCoord2(&pts2[i - 3], &pts2[i - 2], true);
						pts2[i - 3] -= HalfScreenX; pts2[i - 2] -= HalfScreenY;
					}
				}				
			}
			GRAFIX->SetColor_Double(1, 0, 0);
			GRAFIX->drawPoints(pts1, selectedcnt, false, 3, FGPOINTSIZE);
			GRAFIX->SetColor_Double(1, 0, 1);
			GRAFIX->drawPoints(pts2, totcount - selectedcnt, false, 3, FGPOINTSIZE);
			free(pts1); free(pts2);
		}
		else
		{
			double* tmppts  = (double*)malloc(sizeof(double) *  totcount * 3);
			GRAFIX->SetColor_Double(1, 0, 1);
			if(this->ShapeAs3DShape())
			{
				memcpy(tmppts, pts, sizeof(double) * totcount * 3);
				if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && !CALBCALCOBJECT->CalibrationModeFlag() && MAINDllOBJECT->UseLocalisedCorrection)
				{
					for(int i = 0; i < totcount; i++)
					{
						tmppts[i * 3] += HalfScreenX; tmppts[i * 3 + 1] += HalfScreenY;
						BESTFITOBJECT->correctXYCoord2(&tmppts[i * 3], &tmppts[i * 3 + 1], true);
						tmppts[i * 3] -= HalfScreenX; tmppts[i * 3 + 1] -= HalfScreenY;
					}
				}
				GRAFIX->drawPoints(tmppts, totcount, false, 3, FGPOINTSIZE);
			}
			else
			{
				memcpy(tmppts, pts, sizeof(double) * totcount * 2);
				if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && !CALBCALCOBJECT->CalibrationModeFlag() && MAINDllOBJECT->UseLocalisedCorrection)
				{
					for(int i = 0; i < totcount; i++)
					{
						tmppts[i * 2] += HalfScreenX; tmppts[i * 2 + 1] += HalfScreenY;
						BESTFITOBJECT->correctXYCoord2(&tmppts[i * 2], &tmppts[i * 2 + 1], true);
						tmppts[i * 2] -= HalfScreenX; tmppts[i * 2 + 1] -= HalfScreenY;
					}
				}
				GRAFIX->drawPoints(tmppts, totcount, false, 2, FGPOINTSIZE);
			}
			free(tmppts);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0012", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::drawPointCrossMark(double wupp)
{
	try
	{
		for each(RAction* CAction in PointAtionList)
		{
			if(CAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				AddPointAction* CPtAction = (AddPointAction*)CAction;
				FramegrabBase* CFg = CPtAction->getFramegrab();
				if(CFg->PointActionIdList.size() > 0)
				{
					if(this->PointsList->Pointscount() < 1) continue;
					int Tmp_Indx = (int)*CFg->PointActionIdList.begin();
					if(this->PointsList->getList().find(Tmp_Indx) == this->PointsList->getList().end()) continue;
					SinglePoint* Spt = this->PointsList->getList()[Tmp_Indx];
					if(CFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || CFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR ||
						CFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT || CFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::SCANCROSSHAIR ||
						CFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::LIVESCANCROSSHAIR || CFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::SELECTPOINTCROSSHAIR)
						GRAFIX->drawCrossMark(Spt->X, Spt->Y, Spt->Z, 5 * wupp);
				}
			}
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0013", __FILE__, __FUNCSIG__);
	}
}

void ShapeWithList::DotheShapeUpdate()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(this->PointsList->Pointscount() > 10)
			{
				if((AppliedLineArcSeperation()) && (!(this->AppliedLineArcSepOnce())))
					DoLineArcSeperation();
				else
					UpdateShape();
			}
			else
				UpdateShape();
		}
		else
		{
			if(this->PointsList->Pointscount() > 10)
			{
				if((ShapeType == RapidEnums::SHAPETYPE::LINE || ShapeType == RapidEnums::SHAPETYPE::ARC || ShapeType == RapidEnums::SHAPETYPE::CIRCLE || ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) && MAINDllOBJECT->ApplyLineArcSeperationMode() && (!(this->AppliedLineArcSepOnce())))
					DoLineArcSeperation();
				else
					UpdateShape();
			}
			else
				UpdateShape();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0013", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::DoLineArcSeperation()
{
	try
	{
		this->AppliedLineArcSeperation(true);
		list<LineArc> LineArcColl;
		list<Pt> AllPointsCollection;
		map<int, Pt> AllPointsColl;
		map<int, Pt> AllPointsTempCollection;
		map<int, int> TempPointMap;
		Pt leftpoint; bool flag = true;
		//bool iscloudpts = false;
		//if (this->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
		//	iscloudpts = true;
		int Cnt = 0;
		for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			Pt newpt;
			newpt.x = Spt->X; newpt.y = Spt->Y;
			newpt.pt_index = Spt->PtID;
			if(flag)
			{ 
				flag = false;
				leftpoint.x = newpt.x; leftpoint.y = newpt.y; 
				leftpoint.pt_index = newpt.pt_index;
			}
			else
			{
				if(newpt.x < leftpoint.x)
				{
					leftpoint.x = newpt.x; leftpoint.y = newpt.y; 
					leftpoint.pt_index = newpt.pt_index;
				}
			}
			AllPointsColl[Cnt] = newpt;
			Cnt++;
		}
		Cnt = 0;
		Pt FisrtPt;
		FisrtPt.x = leftpoint.x; FisrtPt.y = leftpoint.y;
		FisrtPt.pt_index = 0;
		int TotalCnt = AllPointsColl.size();
		AllPointsTempCollection[0] = FisrtPt;
		TempPointMap[0] = leftpoint.pt_index;
		AllPointsCollection.push_back(FisrtPt);
		AllPointsColl.erase(leftpoint.pt_index);
		
		for(int i = 0; i < TotalCnt - 1; i++)
		{
			Pt tempPoint = AllPointsTempCollection[Cnt];
			Pt CurrentPt;
			double dist = 0, mindist = 0; flag = true;
			int C_index;
			for(map<int, Pt>::iterator Pitem = AllPointsColl.begin(); Pitem != AllPointsColl.end(); Pitem++)
			{
				Pt Cpt = (*Pitem).second;
				double tempdist = 0;
				tempdist += pow((tempPoint.x - Cpt.x), 2);  
				tempdist += pow((tempPoint.y - Cpt.y), 2);  
				dist = sqrt(tempdist);
				if(flag)
				{ 
					mindist = dist; flag = false;
					CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
					C_index = Cpt.pt_index;
				}
				else
				{
					if(mindist > dist)
					{
						mindist = dist;
						CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
						C_index = Cpt.pt_index;
					}
				}
			}
			Cnt++;
			TempPointMap[Cnt] = CurrentPt.pt_index;
			CurrentPt.pt_index = Cnt;
			AllPointsTempCollection[Cnt] = CurrentPt;
			AllPointsCollection.push_back(CurrentPt);
			AllPointsColl.erase(C_index);
		}
		double tolerance = 2.5*MAINDllOBJECT->getWindow(0).getUppX();
		BESTFITOBJECT->fnLinesAndArcs(&AllPointsCollection, &LineArcColl, tolerance);
		LineArc* CurrentLineArc;
		int MaxPointcount = 0; bool FirstFlag = true;
		int Maxstart_index = 0, Maxend_index = 0;
		//bool CurrentShape = false;
		//if(this->ShapeType == RapidEnums::SHAPETYPE::LINE)
		//	CurrentShape = false;
		//else
		//	CurrentShape = true;
		for (list<LineArc>::iterator linearc_iter = LineArcColl.begin(); linearc_iter != LineArcColl.end(); linearc_iter++)
		{
			int start_index = linearc_iter->start_pt.pt_index;
			int end_index = linearc_iter->end_pt.pt_index;
			int PtCont = end_index - start_index + 1;
			//if(abs(linearc_iter->slope) < 0.02) linearc_iter->slope = 0;
			//bool TempFlag = true;
			//if(linearc_iter->slope == 0)TempFlag = false;
			//if (iscloudpts)
			//	CurrentShape = TempFlag;
			if(FirstFlag) // && (TempFlag == CurrentShape))
			{
				FirstFlag = false;
				MaxPointcount = PtCont;
				/*CurrentLineArc = linearc_iter;*/
				Maxstart_index = start_index;
				Maxend_index = end_index;
			}
			else
			{
				if(MaxPointcount < PtCont) // && (TempFlag == CurrentShape))
				{
					MaxPointcount = PtCont;
					/*CurrentLineArc = linearc_iter;*/
					Maxstart_index = start_index;
					Maxend_index = end_index;
				}
			}
		}
		PointCollection PtColl;
		if(Maxstart_index != Maxend_index)
		{
			for(int i = Maxstart_index; i <= Maxend_index; i++)
			{
				Pt tempPoint = AllPointsTempCollection[i];
				SinglePoint* CSpt = new SinglePoint(tempPoint.x, tempPoint.y, MAINDllOBJECT->getCurrentDRO().getZ());
				//CSpt->PtID = TempPointMap[i];
				PtColl.Addpoint(CSpt);
			}
		}
		this->AppliedLineArcSepOnce(true);
		this->deleteAllPoints();
		for each(BaseItem* bi in PointAtionList)
		{
			RAction* Cact = (RAction*)(bi);
			if(Cact->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				AddPointAction* CAction = (AddPointAction*)Cact;
				CAction->getFramegrab()->PointActionIdList.clear();
			}
		}
		AddPoints(&PtColl);
		this->AppliedLineArcSepOnce(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0014", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::TransformShapePoints(double* Transform)
{
   try
   {
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		for(PC_ITER Item = this->PointsList->getList().begin(), Item2 = this->PointsListOriginal->getList().begin(); Item != this->PointsList->getList().end(); Item++, Item2++)
		{     
				SinglePoint* Sp = (*Item).second;
				double temp1[4], temp2[4] = {Sp->X, Sp->Y, Sp->Z, 1}; 
				RMATH2DOBJECT->MultiplyMatrix1(Transform, Order1, temp2, Order2, temp1);
				Item->second->SetValues(temp1[0], temp1[1], temp1[2],  temp1[3]);
				Item2->second->SetValues(temp1[0], temp1[1], temp1[2],  temp1[3]);
		}
   }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0015", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::writePointsToHarddisc(FramegrabBase* CurrentFG)
{
	try
	{
		std::list<int> PointsIdListforAction;
		if(CurrentFG->AllPointsList.Pointscount() > 1)
		{
			std::list<int> PointsIdList;
			int cnt = 0;
			for( PC_ITER item = CurrentFG->AllPointsList.getList().begin(); item != CurrentFG->AllPointsList.getList().end(); item++)
			{													
				if(cnt < CurrentFG->AllPointsList.Pointscount() - 1)
				{
					cnt++;
					item++;
				}				
				PointsIdList.push_back((*item).second->PtID);					
				cnt++;
			}
			
		
			for( PC_ITER Pts = CurrentFG->AllPointsList.getList().begin(); Pts != CurrentFG->AllPointsList.getList().end(); Pts++)
			{
				int pid = (*Pts).second->PtID;
				bool ptsIdPresent = false;
				for each(int id in PointsIdList)
				{
					if(id == pid)
					{
						ptsIdPresent = true;
						break;
					}
				}
				if(!ptsIdPresent)
					PointsIdListforAction.push_back(pid);
			}
		}
		else if(CurrentFG->AllPointsList.Pointscount() == 1)
		{
			PC_ITER item = CurrentFG->AllPointsList.getList().begin();
			PointsIdListforAction.push_back((*item).second->PtID);	
		}

		std::string filename, filePath;
		std::wofstream PointCollectionWriter;
		char test[50];
		_itoa(this->getId(), test, 10);
		filename = (const char*)(test);
		filePath = MAINDllOBJECT->currentPath + "\\PointsLog\\" + filename +".txt";
		PointCollectionWriter.open(filePath.c_str(), std::ios_base::app);
		for each(int id in PointsIdListforAction)
		{
			SinglePoint *Spt = CurrentFG->AllPointsList.getList()[id];
			PointCollectionWriter << Spt->X << endl;
			PointCollectionWriter << Spt->Y << endl;
			PointCollectionWriter << Spt->Z << endl;
		}					
		PointCollectionWriter.close();
		for each(int id in PointsIdListforAction)
			CurrentFG->AllPointsList.deletePoint(id);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0016", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::writePointsToHarddisc(PointCollection* PtsList)
{
	try
	{
		std::list<int> PointsIdListforAction;
		if(PtsList->Pointscount() > 1)
		{
			std::list<int> PointsIdList;
			int cnt = 0;
			for( PC_ITER item = PtsList->getList().begin(); item != PtsList->getList().end(); item++)
			{													
				if(cnt < PtsList->Pointscount() - 1)
				{
					cnt++;
					item++;
				}				
				PointsIdList.push_back((*item).second->PtID);					
				cnt++;
			}
			
		
			for( PC_ITER Pts = PtsList->getList().begin(); Pts != PtsList->getList().end(); Pts++)
			{
				int pid = (*Pts).second->PtID;
				bool ptsIdPresent = false;
				for each(int id in PointsIdList)
				{
					if(id == pid)
					{
						ptsIdPresent = true;
						break;
					}
				}
				if(!ptsIdPresent)
					PointsIdListforAction.push_back(pid);
			}
		}
		else if(PtsList->Pointscount() == 1)
		{
			PC_ITER item = PtsList->getList().begin();
			PointsIdListforAction.push_back((*item).second->PtID);	
		}

		std::string filename, filePath;
		std::wofstream PointCollectionWriter;
		char test[50];
		_itoa(this->getId(), test, 10);
		filename = (const char*)(test);
		filePath = MAINDllOBJECT->currentPath + "\\PointsLog\\" + filename +".txt";
		PointCollectionWriter.open(filePath.c_str(), std::ios_base::app);
		for each(int id in PointsIdListforAction)
		{
			SinglePoint *Spt = PtsList->getList()[id];
			PointCollectionWriter << Spt->X << endl;
			PointCollectionWriter << Spt->Y << endl;
			PointCollectionWriter << Spt->Z << endl;
		}					
		PointCollectionWriter.close();
		for each(int id in PointsIdListforAction)
			PtsList->deletePoint(id);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0016", __FILE__, __FUNCSIG__); }
}

void ShapeWithList::GetDownSampledPoints()
{
	try
	{
		//if u want to change pts count value(20000) than u have to change in RCADapp also SavePointsToHardDisc....//
		if(PointsList->Pointscount() >= MAINDllOBJECT->SamplePtsCutOff)
		{		
			Stepcount = Stepcount + 1;
			std::list<int> PointsIdList;
			std::list<int> PointsIdListforAction;
			int cnt = 0;
			for( PC_ITER item = this->PointsList->getList().begin(); item != this->PointsList->getList().end(); item++)
			{							
				if(cnt < this->PointsList->Pointscount() - 1)
				{
					PointsIdListforAction.push_back((*item).second->PtID);
					cnt++;
					item++;
				}				
				PointsIdList.push_back((*item).second->PtID);					
				cnt++;
			}
										
			std::string filename, filePath;
			std::wofstream PointCollectionWriter;
			char test[50];
			_itoa(this->getId(), test, 10);
			filename = (const char*)(test);
			filePath = MAINDllOBJECT->currentPath + "\\PointsLog\\" + filename +".txt";
			PointCollectionWriter.open(filePath.c_str(), std::ios_base::app);
			for each(int id in PointsIdListforAction)
			{
				SinglePoint *Spt = this->PointsList->getList()[id];
				PointCollectionWriter << Spt->X << endl;
				PointCollectionWriter << Spt->Y << endl;
				PointCollectionWriter << Spt->Z << endl;
			}					
			PointCollectionWriter.close();
			MAINDllOBJECT->selectFGPoints(&PointsIdListforAction);
			MAINDllOBJECT->deleteFGPoints();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHW0017", __FILE__, __FUNCSIG__); }
}

double ShapeWithList::Get2DShapeZLevel(PointCollection *Pointcol1, PointCollection *PointCol2, int ordinate)
{
	double Zlevel = -1000;
	if(ordinate == 2)
	{
		if(!Pointcol1->getList().empty())
		{
			for(PC_ITER Spt = Pointcol1->getList().begin(); Spt != Pointcol1->getList().end(); Spt++)
			{
				if(Zlevel == -1000)
					Zlevel = Spt->second->Z;
				else if(Spt->second->Z > Zlevel)
					Zlevel = Spt->second->Z;
			}
		}
		if(!PointCol2->getList().empty())
		{
			for(PC_ITER Spt = PointCol2->getList().begin();  Spt != PointCol2->getList().end(); Spt++)
			{
				if(Zlevel == -1000)
					Zlevel = Spt->second->Z;
				else if(Spt->second->Z > Zlevel)
					Zlevel = Spt->second->Z;
			}
		}
		else if(Zlevel == -1000)
			Zlevel = MAINDllOBJECT->getCurrentDRO().getZ();
	}
	else if(ordinate == 1)
	{
		if(!Pointcol1->getList().empty())
		{
			for(PC_ITER Spt = Pointcol1->getList().begin(); Spt != Pointcol1->getList().end(); Spt++)
			{
				if(Zlevel == -1000)
					Zlevel = Spt->second->Y;
				else if(Spt->second->Y > Zlevel)
					Zlevel = Spt->second->Y;
			}
		}
		if(!PointCol2->getList().empty())
		{
			for(PC_ITER Spt = PointCol2->getList().begin();  Spt != PointCol2->getList().end(); Spt++)
			{
				if(Zlevel == -1000)
					Zlevel = Spt->second->Y;
				else if(Spt->second->Y > Zlevel)
					Zlevel = Spt->second->Y;
			}
		}
		else if(Zlevel == -1000)
			Zlevel = MAINDllOBJECT->getCurrentDRO().getY();
	}
	else
	{
		if(!Pointcol1->getList().empty())
		{
			for(PC_ITER Spt = Pointcol1->getList().begin(); Spt != Pointcol1->getList().end(); Spt++)
			{
				if(Zlevel == -1000)
					Zlevel = Spt->second->X;
				else if(Spt->second->X > Zlevel)
					Zlevel = Spt->second->X;
			}
		}
		if(!PointCol2->getList().empty())
		{
			for(PC_ITER Spt = PointCol2->getList().begin();  Spt != PointCol2->getList().end(); Spt++)
			{
				if(Zlevel == -1000)
					Zlevel = Spt->second->X;
				else if(Spt->second->X > Zlevel)
					Zlevel = Spt->second->X;
			}
		}
		else if(Zlevel == -1000)
			Zlevel = MAINDllOBJECT->getCurrentDRO().getX();
	}
	return Zlevel;									
}
