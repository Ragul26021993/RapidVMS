#include "stdafx.h"
#include "CloudComparisionHandler.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\CloudComparisionAction.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\ProfileScanAction.h"
#include "..\Engine\PointCollection.h"
#include "..\Shapes\SinglePoint.h"
#include "..\DXF\DXFExpose.h"
#include "..\Shapes\RPoint.h"

//Constructor..//
CloudComparisionHandler::CloudComparisionHandler()
{
	try
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CLOUDCOMPARISION_HANDLER;
		runningPartprogramValid = false;
		MaxInnerPtsAllowed = 700; MaxOuterPtsAllowed = 1500;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
CloudComparisionHandler::~CloudComparisionHandler()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0002", __FILE__, __FUNCSIG__); }
}

//Mousemove....//
void CloudComparisionHandler::mouseMove()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0003", __FILE__, __FUNCSIG__); }
}

//Handle the left mousedown....//
void CloudComparisionHandler::LmouseDown()
{
	try{}
	catch(...){  MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0004", __FILE__, __FUNCSIG__); }
}

//Draw the parallel arc..//
void CloudComparisionHandler::draw(int windowno, double WPixelWidth)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0005", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::EscapebuttonPress()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0006", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data.....//
void CloudComparisionHandler::PartProgramData()
{
	try
	{
		std::list<int> ShapeIdlist;
		bool ArcBestFit = false;
		ShapeWithList* ParentShape1 = NULL, *ParentShape2 = NULL, *ParentShape3 = NULL;
		int ParentShpId[2] = {0}, numberOfPts = 0, SkipPts1 = 0, SkipPts2 = 0;
		ParentShape3 = ((CloudComparisionAction*)PPCALCOBJECT->getCurrentAction())->GetThirdParentShape();
		if(ParentShape3 != NULL)
		{
			double angle = 0;
			ArcBestFit = ((CloudComparisionAction*)PPCALCOBJECT->getCurrentAction())->GetCloudComparisionParam(&ShapeIdlist, &angle, ParentShpId, &numberOfPts);
			ParentShape1 = (ShapeWithList*)MAINDllOBJECT->getShapesList().getList()[ParentShpId[0]];
			ParentShape2 = (ShapeWithList*)MAINDllOBJECT->getShapesList().getList()[ParentShpId[1]];
			if(ParentShape1->PointsList->Pointscount() > MaxInnerPtsAllowed)
				SkipPts1 = ParentShape1->PointsList->Pointscount() / MaxInnerPtsAllowed;
			if(ParentShape1 != NULL && ParentShape2 != NULL)
				UpdateMeasurementFor_Delphi_Component(angle, ParentShape1, ParentShape2, ParentShape3, ArcBestFit, numberOfPts, SkipPts1, ShapeIdlist);
		}
		else if(((CloudComparisionAction*)PPCALCOBJECT->getCurrentAction())->GetClosedLoopFlag())
		{
			int MeasureCount = 0;
			ArcBestFit = ((CloudComparisionAction*)PPCALCOBJECT->getCurrentAction())->GetCloudComparisionParam(&ShapeIdlist, &MeasureCount, ParentShpId, &numberOfPts);
			bool SurfaceOn = ((CloudComparisionAction*)PPCALCOBJECT->getCurrentAction())->GetSurfaceLightFlag();
			ParentShape1 = (ShapeWithList*)MAINDllOBJECT->getShapesList().getList()[ParentShpId[0]];
			ParentShape2 = (ShapeWithList*)MAINDllOBJECT->getShapesList().getList()[ParentShpId[1]];
			std::list<int> Idlist;
			Idlist.push_back(ParentShape1->getId());
			Idlist.push_back(ParentShape2->getId());
			MAINDllOBJECT->selectShape(&Idlist);
			SkipPts1 = ParentShape1->PointsList->Pointscount() / MaxInnerPtsAllowed;
			SkipPts2 = ParentShape2->PointsList->Pointscount() / MaxOuterPtsAllowed;
			UpdateDeviationParamFor3MComponent(SurfaceOn, MeasureCount, SkipPts1, SkipPts2, ParentShape1, ParentShape2, ArcBestFit, numberOfPts, ShapeIdlist);		
		}
		else
		{
			double interval = 0;
			ArcBestFit = ((CloudComparisionAction*)PPCALCOBJECT->getCurrentAction())->GetCloudComparisionParam(&ShapeIdlist, &interval, ParentShpId, &numberOfPts);
			ParentShape1 = (ShapeWithList*)MAINDllOBJECT->getShapesList().getList()[ParentShpId[0]];
			ParentShape2 = (ShapeWithList*)MAINDllOBJECT->getShapesList().getList()[ParentShpId[1]];
			std::list<int> Idlist;
			Idlist.push_back(ParentShape1->getId());
			Idlist.push_back(ParentShape2->getId());
			MAINDllOBJECT->selectShape(&Idlist);
			SkipPts1 = ParentShape1->PointsList->Pointscount() / MaxInnerPtsAllowed;
			SkipPts2 = ParentShape2->PointsList->Pointscount() / MaxOuterPtsAllowed;
			UpdateDeviationParam(interval, SkipPts1, SkipPts2, ParentShape1, ParentShape2, ArcBestFit, numberOfPts, ShapeIdlist);			
		}
		PPCALCOBJECT->partProgramReached();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0007", __FILE__, __FUNCSIG__); }
}

//Set the  parallel arc parameters...
void CloudComparisionHandler::LmaxmouseDown()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0008", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::CloudptComparatorParm(double interval, bool ArcBestFit, int NumberOfPts)
{
	try
	{
		ShapeWithList* ParentShape1, *ParentShape2;
		ParentShape1 = NULL; ParentShape2 = NULL;
		bool ShapePresent = false;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{				
				if(ParentShape1 == NULL)
					ParentShape1 = CShape;
				else if(ParentShape2 == NULL)
				{
					ParentShape2 = CShape;
					ShapePresent = true;
					break;
				}
			}
		}
		int SkipPts1 = 0, SkipPts2 = 0;
		if(ShapePresent && ParentShape1->PointsList->Pointscount() > 1 && ParentShape2->PointsList->Pointscount() > 1)
		{
			SkipPts1 = ParentShape1->PointsList->Pointscount() / MaxInnerPtsAllowed;
			SkipPts2 = ParentShape2->PointsList->Pointscount() / MaxOuterPtsAllowed;
			std::list<Shape*> LineIdList;
			if(CreateMeasurement(interval, ParentShape1, ParentShape2, ArcBestFit, NumberOfPts, SkipPts1, SkipPts2, &LineIdList))
			{
				CloudComparisionAction::SetCloudCompareParam(interval, ParentShape1, ParentShape2, ArcBestFit, NumberOfPts, LineIdList);
			}
		}
		else 
		{
			//message box please select two cloud points shape for this tool..
			MAINDllOBJECT->ShowMsgBoxString("CloudComparisionHandler01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0009", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::CloudptComparatorParm(int MeasureCount, bool ArcBestFit, int NumberOfPts)
{
	try
	{
		ShapeWithList* ParentShape1, *ParentShape2;
		ParentShape1 = NULL; ParentShape2 = NULL;
		bool ShapePresent = false;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{				
				if(ParentShape1 == NULL)
					ParentShape1 = CShape;
				else if(ParentShape2 == NULL)
				{
					ParentShape2 = CShape;
					ShapePresent = true;
					break;
				}
			}
		}
		bool SurfaceOn = false;

		for each(RAction* Caction in  ParentShape1->PointAtionList)
		{
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)Caction)->getFramegrab();
				SurfaceOn = fb->SurfaceON;
				break;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				SurfaceOn = ((ProfileScanAction*)Caction)->GetSurfaceLifghtInfo();
				break;
			}
		}
		
		int SkipPts1 = 0, SkipPts2 = 0;
		if(ShapePresent && ParentShape1->PointsList->Pointscount() > 1 && ParentShape2->PointsList->Pointscount() > 1)
		{
			SkipPts1 = ParentShape1->PointsList->Pointscount() / MaxInnerPtsAllowed;
			SkipPts2 = ParentShape2->PointsList->Pointscount() / MaxOuterPtsAllowed;
			std::list<Shape*> LineIdList;
			if(CreateMeasurementFor3MComponent(SurfaceOn, MeasureCount, ParentShape1, ParentShape2, ArcBestFit, NumberOfPts, SkipPts1, SkipPts2, &LineIdList))
			{
				CloudComparisionAction::SetCloudCompareParam(SurfaceOn, MeasureCount, ParentShape1, ParentShape2, ArcBestFit, NumberOfPts, LineIdList);
			}
		}
		else 
		{
			//message box please select two cloud points shape for this tool..
			MAINDllOBJECT->ShowMsgBoxString("CloudComparisionHandler01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0009", __FILE__, __FUNCSIG__); }
}

bool CloudComparisionHandler::CreateMeasurement(double interval, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, int SkipPts1, int SkipPts2, std::list<Shape*>* LineIdList)
{
	try
	{
		bool MinDistPresent = false, MaxDistPresent = false, firstTime = true, returnFlag = false;
		double Point1[3] = {0}, Point2[3] = {0}, MinDist = 999999, MaxDist = 0, MinDist_Point[6] = {0}, MaxDist_Point[6] = {0}, TmpMinDist = 999999, EndPt[6] = {0};	
		if(interval != 0)
		{
			int TmpCnt1 = 0;
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				SinglePoint* Spt1 = (*SptItem1).second;
				if(TmpCnt1 >= SkipPts1)
				{
					TmpCnt1 = 0;
					Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;	
					findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
				}
				else
				{
					TmpCnt1++;
					continue;
				}
				if(firstTime)
					firstTime = false;
				else
				{
					if(MaxDist < TmpMinDist)
					{
						MaxDistPresent = true;
						MaxDist = TmpMinDist;
						MaxDist_Point[0] = EndPt[0]; MaxDist_Point[1] = EndPt[1]; MaxDist_Point[2] = EndPt[2];
						MaxDist_Point[3] = EndPt[3]; MaxDist_Point[4] = EndPt[4]; MaxDist_Point[5] = EndPt[5];		
					}
				}
				int TmpCnt2 = 0;
				TmpMinDist = 999999;
				for(PC_ITER SptItem2 = ParentShape2->PointsList->getList().begin(); SptItem2 != ParentShape2->PointsList->getList().end(); SptItem2++)
				{
					SinglePoint* Spt2 = (*SptItem2).second;
					if(abs(Point1[0] - Spt2->X) > TmpMinDist && abs(Point1[1] - Spt2->Y) > TmpMinDist) continue;
					if(TmpCnt2 >= SkipPts2)
					{
						TmpCnt2 = 0;
						Point2[0] = Spt2->X; Point2[1] = Spt2->Y; Point2[2] = Spt2->Z;
						findIntersectionPoint(ArcBestFit, NumberOfPts, SptItem2, ParentShape2->PointsList, Point2, Point1);
					}
					else
					{
						TmpCnt2++;
						continue;
					}
					double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
					if(MinDist > dist)
					{
						MinDistPresent = true;
						MinDist = dist;
						MinDist_Point[0] = Point1[0]; MinDist_Point[1] = Point1[1]; MinDist_Point[2] = Point1[2];
						MinDist_Point[3] = Point2[0]; MinDist_Point[4] = Point2[1]; MinDist_Point[5] = Point1[2];
					}
					if(TmpMinDist > dist)
					{
						TmpMinDist = dist;
						EndPt[0] = Point1[0]; EndPt[1] = Point1[1]; EndPt[2] = Point1[2];
						EndPt[3] = Point2[0]; EndPt[4] = Point2[1]; EndPt[5] = Point1[2];
					}
				}
			}
			if(MinDistPresent)
			{
				returnFlag = true;
				CreateLineMeasureMent(MinDist_Point, &MinDist_Point[3], ParentShape1, ParentShape2, LineIdList, true, 1);
			}

			if(MaxDistPresent)
			{
				CreateLineMeasureMent(MaxDist_Point, &MaxDist_Point[3], ParentShape1, ParentShape2, LineIdList, true, 2);
			}
				
		    firstTime = true;
			double LastPt[3] = {0};
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				SinglePoint* Spt1 = (*SptItem1).second;
				Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;
				if(firstTime)
					firstTime = false;
				else
				{
					if(RMATH2DOBJECT->Pt2Pt_distance(Point1, LastPt) < interval) continue;
				}
				findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
				double dist1 = 999999, EndPt[3] = {0};
				for(PC_ITER SptItem2 = ParentShape2->PointsList->getList().begin(); SptItem2 != ParentShape2->PointsList->getList().end(); SptItem2++)
				{
					SinglePoint* Spt2 = (*SptItem2).second;
					if(abs(Point1[0] - Spt2->X) > dist1 && abs(Point1[1] - Spt2->Y) > dist1) continue;

					Point2[0] = Spt2->X; Point2[1] = Spt2->Y; Point2[2] = Spt2->Z;
					findIntersectionPoint(ArcBestFit, NumberOfPts, SptItem2, ParentShape2->PointsList, Point2, Point1);
					double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
					if(dist1 > dist)
					{
						dist1 = dist;
						EndPt[0] = Point2[0]; EndPt[1] = Point2[1]; EndPt[2] = Point2[2];
					}	
				}
				LastPt[0] = Point1[0]; LastPt[1] = Point1[1]; LastPt[2] = Point1[2];
				//create measurement...
				CreateLineMeasureMent(Point1, EndPt, ParentShape1, ParentShape2, LineIdList);
			}
		}
		else
		{
			int noOfLines = 0;
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				if(noOfLines > 500) break;
				SinglePoint* Spt1 = (*SptItem1).second;
				Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;
				double dist1 = 999999, EndPt[3] = {0};
				for(PC_ITER SptItem2 = ParentShape2->PointsList->getList().begin(); SptItem2 != ParentShape2->PointsList->getList().end(); SptItem2++)
				{
					SinglePoint* Spt2 = (*SptItem2).second;
					Point2[0] = Spt2->X; Point2[1] = Spt2->Y; Point2[2] = Spt2->Z;
					double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
					if(dist1 > dist)
					{
						dist1 = dist;
						EndPt[0] = Point2[0]; EndPt[1] = Point2[1]; EndPt[2] = Point2[2];
					}
				}
				//create measurement...
				noOfLines++;
				CreateLineMeasureMent(Point1, EndPt, ParentShape1, ParentShape2, LineIdList);
				returnFlag = true;
			}
		}
		return returnFlag;
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0010", __FILE__, __FUNCSIG__); return false;}
}

void CloudComparisionHandler::UpdateDeviationParam(double interval, int SkipPts1, int SkipPts2, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, std::list<int> LineIdList)
{
	try
	{
		std::map<int, UpdatedPts> PointsIdList;
		bool MinDistPresent = false, MaxDistPresent = false, firstTime = true;
		double Point1[3] = {0}, Point2[3] = {0}, MinDist = 999999, MaxDist = 0, MinDist_Point[6] = {0}, MaxDist_Point[6] = {0}, TmpMinDist = 999999, EndPt[6] = {0};	
		if(interval != 0)
		{
			int TmpCnt1 = 0;
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				SinglePoint* Spt1 = (*SptItem1).second;
				if(TmpCnt1 >= SkipPts1)
				{
					TmpCnt1 = 0;
					Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;
					findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
				}
				else
				{
					TmpCnt1++;
					continue;
				}
				if(firstTime)
					firstTime = false;
				else
				{
					if(MaxDist < TmpMinDist)
					{
						MaxDistPresent = true;
						MaxDist = TmpMinDist;
						MaxDist_Point[0] = EndPt[0]; MaxDist_Point[1] = EndPt[1]; MaxDist_Point[2] = EndPt[2];
						MaxDist_Point[3] = EndPt[3]; MaxDist_Point[4] = EndPt[4]; MaxDist_Point[5] = EndPt[5];	
					}
				}
				int TmpCnt2 = 0; TmpMinDist = 999999;
				for(PC_ITER SptItem2 = ParentShape2->PointsList->getList().begin(); SptItem2 != ParentShape2->PointsList->getList().end(); SptItem2++)
				{
					SinglePoint* Spt2 = (*SptItem2).second;
					if(TmpCnt2 >= SkipPts2)
					{
						TmpCnt2 = 0;
						Point2[0] = Spt2->X; Point2[1] = Spt2->Y; Point2[2] = Spt2->Z;
						findIntersectionPoint(ArcBestFit, NumberOfPts, SptItem2, ParentShape2->PointsList, Point2, Point1);
					}
					else
					{
						TmpCnt2++;
						continue;
					}
					double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
					if(MinDist > dist)
					{
						MinDistPresent = true;
						MinDist = dist;
						MinDist_Point[0] = Point1[0]; MinDist_Point[1] = Point1[1]; MinDist_Point[2] = Point1[2];
						MinDist_Point[3] = Point2[0]; MinDist_Point[4] = Point2[1]; MinDist_Point[5] = Point1[2];
					}
					if(TmpMinDist > dist)
					{
						TmpMinDist = dist;
						EndPt[0] = Point1[0]; EndPt[1] = Point1[1]; EndPt[2] = Point1[2];
						EndPt[3] = Point2[0]; EndPt[4] = Point2[1]; EndPt[5] = Point1[2];
					}
				}
			}
		
			if(MinDistPresent)
			{
				if(LineIdList.size() > 0)
				{
					std::list<int>::iterator itr = LineIdList.begin();
					int LineId = *itr;
					LineIdList.remove(LineId);
					UpdatedPts LineEndPts;
					LineEndPts.EndPts[0] = MinDist_Point[0]; LineEndPts.EndPts[1] = MinDist_Point[1]; LineEndPts.EndPts[2] = MinDist_Point[2];
					LineEndPts.EndPts[3] = MinDist_Point[3]; LineEndPts.EndPts[4] = MinDist_Point[4]; LineEndPts.EndPts[5] = MinDist_Point[5];
					PointsIdList[LineId] = LineEndPts;
				}
			}
			if(MaxDistPresent)
			{
				if(LineIdList.size() > 0)
				{
					std::list<int>::iterator itr = LineIdList.begin();
					int LineId = *itr;
					LineIdList.remove(LineId);
					UpdatedPts LineEndPts;
					LineEndPts.EndPts[0] = MaxDist_Point[0]; LineEndPts.EndPts[1] = MaxDist_Point[1]; LineEndPts.EndPts[2] = MaxDist_Point[2];
					LineEndPts.EndPts[3] = MaxDist_Point[3]; LineEndPts.EndPts[4] = MaxDist_Point[4]; LineEndPts.EndPts[5] = MaxDist_Point[5];
					PointsIdList[LineId] = LineEndPts;
				}
			}
						
		    firstTime = true;
			double LastPt[3] = {0};
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				if(LineIdList.size() < 1) break;
				SinglePoint* Spt1 = (*SptItem1).second;
				Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;
				if(firstTime)
					firstTime = false;
				else
				{
					if(RMATH2DOBJECT->Pt2Pt_distance(Point1, LastPt) < interval) continue;
				}				
				findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
				double dist1 = 999999, EndPt[3] = {0};
				for(PC_ITER SptItem2 = ParentShape2->PointsList->getList().begin(); SptItem2 != ParentShape2->PointsList->getList().end(); SptItem2++)
				{
					SinglePoint* Spt2 = (*SptItem2).second;
					Point2[0] = Spt2->X; Point2[1] = Spt2->Y; Point2[2] = Spt2->Z;
					findIntersectionPoint(ArcBestFit, NumberOfPts, SptItem2, ParentShape2->PointsList, Point2, Point1);
					double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
					if(dist1 > dist)
					{
						dist1 = dist;
						EndPt[0] = Point2[0]; EndPt[1] = Point2[1]; EndPt[2] = Point2[2];
					}	
				}
				LastPt[0] = Point1[0]; LastPt[1] = Point1[1]; LastPt[2] = Point1[2];

				std::list<int>::iterator itr = LineIdList.begin();
				int LineId = *itr;
				LineIdList.remove(LineId);
				UpdatedPts LineEndPts;
				LineEndPts.EndPts[0] = Point1[0]; LineEndPts.EndPts[1] = Point1[1]; LineEndPts.EndPts[2] = Point1[2];
				LineEndPts.EndPts[3] = EndPt[0]; LineEndPts.EndPts[4] = EndPt[1]; LineEndPts.EndPts[5] = EndPt[2];
				PointsIdList[LineId] = LineEndPts;
			}
		}
		else
		{
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				if(LineIdList.size() < 1) break;
				SinglePoint* Spt1 = (*SptItem1).second;
				Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;
				double dist1 = 999999, EndPt[3] = {0};
				for(PC_ITER SptItem2 = ParentShape2->PointsList->getList().begin(); SptItem2 != ParentShape2->PointsList->getList().end(); SptItem2++)
				{
					SinglePoint* Spt2 = (*SptItem2).second;
					Point2[0] = Spt2->X; Point2[1] = Spt2->Y; Point2[2] = Spt2->Z;
					double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
					if(dist1 > dist)
					{
						dist1 = dist;
						EndPt[0] = Point2[0]; EndPt[1] = Point2[1]; EndPt[2] = Point2[2];
					}
				}
				std::list<int>::iterator itr = LineIdList.begin();
				int LineId = *itr;
				LineIdList.remove(LineId);
				UpdatedPts LineEndPts;
				LineEndPts.EndPts[0] = Point1[0]; LineEndPts.EndPts[1] = Point1[1]; LineEndPts.EndPts[2] = Point1[2];
				LineEndPts.EndPts[3] = EndPt[0]; LineEndPts.EndPts[4] = EndPt[1]; LineEndPts.EndPts[5] = EndPt[2];
				PointsIdList[LineId] = LineEndPts;
			}
		}
		//update all lines created by cloudPts comparision...
		for(std::map<int, UpdatedPts>::iterator mapItr = PointsIdList.begin(); mapItr != PointsIdList.end(); mapItr++)
		{
			UpdatedPts LnPts = (*mapItr).second;
			int LineId = (*mapItr).first;
			Shape* Lineshape = (Shape*)(MAINDllOBJECT->getShapesList().getList()[LineId]);
			((Line*)Lineshape)->setPoint1(Vector(LnPts.EndPts[0], LnPts.EndPts[1], LnPts.EndPts[2]));
			((Line*)Lineshape)->setPoint2(Vector(LnPts.EndPts[3], LnPts.EndPts[4], LnPts.EndPts[2]));		
			((Line*)Lineshape)->calculateAttributes();	
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0011", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::findAveragePoint(double* points, double radius, PointCollection *PtsList, double* DeviationPt)
{
	try
	{
		double tempPt[2] = {0};
		int count = 0;
		DeviationPt[0] = points[0]; DeviationPt[1] = points[1]; DeviationPt[2] = points[2];
		for(PC_ITER itr = PtsList->getList().begin(); itr != PtsList->getList().end(); itr++)
		{
			SinglePoint* Spt = (*itr).second;
			double temporaryPt[2] = {Spt->X, Spt->Y};
			if(abs(points[0] - temporaryPt[0]) <= radius && abs(points[1] - temporaryPt[1]) <= radius)
			{
				tempPt[0] += temporaryPt[0]; tempPt[1] += temporaryPt[1];
				count++;
			}				
		}
		tempPt[0] = tempPt[0] / count; tempPt[1] = tempPt[1] / count;
		double dist = 999999;
		for(PC_ITER itr = PtsList->getList().begin(); itr != PtsList->getList().end(); itr++)
		{
			SinglePoint* Spt = (*itr).second;
			double temporaryPt[2] = {Spt->X, Spt->Y};		
			double dist1 = RMATH2DOBJECT->Pt2Pt_distance(temporaryPt, tempPt);
			if(dist1 < dist)
			{
				dist = dist1;
				DeviationPt[0] = temporaryPt[0]; DeviationPt[1] = temporaryPt[1];
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0012", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::findIntersectionPoint(bool ArcBestFit, int BestFitPts, map<int, SinglePoint*>::iterator SptItem, PointCollection *PtsList, double* ProjectedPt, double* Point1)
{
	try
	{
		double Distance = 0;
		int PtsCount = BestFitPts * 2 + 1;
		double TempPoint[3] = {((*SptItem).second)->X, ((*SptItem).second)->Y, ((*SptItem).second)->Z};
		if(PtsCount < 3 || PtsCount > PtsList->Pointscount() - 1) return;
		double *PtsArray = new double[PtsCount * 2];
		int count = BestFitPts;
		for(int i = 0; i < count; i++)
		{
			if(SptItem == PtsList->getList().begin())
			{
				SptItem = PtsList->getList().end();
			}
			SptItem--;
		}
		for(int i = 0; i < PtsCount; i++)
		{
			if(SptItem == PtsList->getList().end())
			{
				SptItem = PtsList->getList().begin();
			}
			PtsArray[2 * i] = ((*SptItem).second)->X; PtsArray[2 * i + 1] = ((*SptItem).second)->Y;
			SptItem++;
		}
		if(ArcBestFit)
		{
			double ans[3] = {0}, angles[2] = {0}, endpts[4] = {0};
			if(BESTFITOBJECT->Circle_BestFit(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
			{
				RMATH2DOBJECT->ArcBestFitAngleCalculation(PtsArray, PtsCount, &ans[0], ans[2], &angles[0]);
				RMATH2DOBJECT->ArcEndPoints(&ans[0], ans[2], angles[0], angles[1], &endpts[0]);
				calculateIntersectionPtOnArc(Point1, ans, ans[2], angles, endpts, ProjectedPt);	
			}
			else
			{
				double p1[2] = {0}, p2[2] = {0};
				BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());		
				RMATH2DOBJECT->LineEndPoints(PtsArray, PtsCount, &p1[0], &p2[0], ans[0]);		
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &endpts[0]);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &endpts[2]);
				calculateIntersectiLine(Point1, ans[0], ans[1], endpts, ProjectedPt);	
			}
		}
		else
		{
			double ans[2] = {0}, p1[2] = {0}, p2[2] = {0}, endPt[4] = {0};
			BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());		
			RMATH2DOBJECT->LineEndPoints(PtsArray, PtsCount, &p1[0], &p2[0], ans[0]);		
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &endPt[0]);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &endPt[2]);
			calculateIntersectiLine(Point1, ans[0], ans[1], endPt, ProjectedPt);			
		}	
		delete [] PtsArray;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0013", __FILE__, __FUNCSIG__);}
}

void CloudComparisionHandler::findProjectedPoint(bool ArcBestFit, int BestFitPts, map<int,SinglePoint*>::iterator SptItem, PointCollection *PtsList, double* ProjectedPt)
{
	try
	{
		double Distance = 0;
		int PtsCount = BestFitPts * 2 + 1;
		double TempPoint[3] = {((*SptItem).second)->X, ((*SptItem).second)->Y, ((*SptItem).second)->Z};
		if(PtsCount < 3 || PtsCount > PtsList->Pointscount() - 1) return;
		double *PtsArray = new double[PtsCount * 2];
		int count = BestFitPts;
		for(int i = 0; i < count; i++)
		{
			if(SptItem == PtsList->getList().begin())
			{
				SptItem = PtsList->getList().end();
			}
			SptItem--;
		}
		for(int i = 0; i < PtsCount; i++)
		{
			if(SptItem == PtsList->getList().end())
			{
				SptItem = PtsList->getList().begin();
			}
			PtsArray[2 * i] = ((*SptItem).second)->X; PtsArray[2 * i + 1] = ((*SptItem).second)->Y;
			SptItem++;
		}
		if(ArcBestFit)
		{
			double ans[3] = {0}, angles[2] = {0};
			if(BESTFITOBJECT->Circle_BestFit(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
			{
				RMATH2DOBJECT->ArcBestFitAngleCalculation(PtsArray, PtsCount, &ans[0], ans[2], &angles[0]);
				RMATH2DOBJECT->Projection_Of_Point_OnArc(ans, ans[2], angles[0], angles[0] + angles[1], TempPoint, ProjectedPt);	
			}
			else
			{
				BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], TempPoint, ProjectedPt);
			}
		}
		else
		{
			double ans[2] = {0};
			BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], TempPoint, ProjectedPt);
		}	
		delete [] PtsArray;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0014", __FILE__, __FUNCSIG__);}
}

void CloudComparisionHandler::CreateLineMeasureMent(double* Point1, double* Point2, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, std::list<Shape*>* LineIdList, bool whitecolor, int MinDist, ShapeWithList* ParentShape3)
{
	try
	{
		HELPEROBJECT->Create2DLine(Point1, Point2, false, whitecolor, false);
		RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
		item--;
		ShapeWithList* Shp = (ShapeWithList*)((*item).second);
		ParentShape1->addChild(Shp);
		Shp->addParent(ParentShape1);
		ParentShape2->addChild(Shp);
		Shp->addParent(ParentShape2);
		if(ParentShape3 != NULL)
		{
			ParentShape3->addChild(Shp);
			Shp->addParent(ParentShape3);
		}
		LineIdList->push_back(Shp);
		//create measurement............
		DimBase* dim = NULL;
		dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
		dim->addParent(Shp);
		Shp->addMChild(dim);
		dim->IsValid(true);
		if(MinDist == 1)
		{
			std::wstring Tag = RMATH2DOBJECT->StringToWString("Min_Dist");
			dim->setOriginalName(Tag);
		}
		else if(MinDist == 2)
		{
			std::wstring Tag = RMATH2DOBJECT->StringToWString("Max_Dist");
			dim->setOriginalName(Tag);
		}
		dim->UpdateMeasurement();
		AddDimAction::addDim(dim);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0015", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::calculateIntersectionPtOnArc(double* pt, double* Center, double Radius, double* Angles, double* endpts, double* ans)
{
	try
	{
		double slope = 0, intercept = 0, intersectpoints[2] = {0}, dist1 = 0, dist2 = 0;
		RMATH2DOBJECT->TwoPointLine(Center, pt, &slope, &intercept);
		if(slope < 0)
			slope = slope + M_PI;
		else if(slope > M_PI)
			slope = slope - M_PI;
		double intpoint1[4] = {0}, intpoint2[4] = {0};
		int count[2] = {0};
		RMATH2DOBJECT->Line_arcinter(slope, intercept, Center, Radius, Angles[0], Angles[1], &intpoint1[0], &intpoint2[0], &count[0]);				        
		if(count[0] == 2)
		{
			dist1 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[0]);
			dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[2]);
			if(dist1 < dist2)
			{
				ans[0] = intpoint1[0];
				ans[1] = intpoint1[1];
				ans[2] = pt[2];
			}
			else
			{
				ans[0] = intpoint1[2];
				ans[1] = intpoint1[3];
				ans[2] = pt[2];
			}
		}
		else if(count[0] == 1)
		{
			ans[0] = intpoint1[0];
			ans[1] = intpoint1[1];
			ans[2] = pt[2];
		}
		else
		{		
			dist1 = RMATH2DOBJECT->Pt2Pt_distance(pt[0], pt[1], endpts[0], endpts[1]);
			dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt[0], pt[1], endpts[2], endpts[3]);
			if(dist1 < dist2)
			{
				ans[0] = endpts[0]; ans[1] = endpts[1]; 
				ans[2] = pt[2];
			}
			else
			{
				ans[0] = endpts[2]; ans[1] = endpts[3]; 
				ans[2] = pt[2];
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0016", __FILE__, __FUNCSIG__);}
}

void CloudComparisionHandler::calculateIntersectiLine(double* Point1, double Angle, double Intercept, double* endpts, double* ans)
{
	try
	{
		double intersectpoints[2] = {0};
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(Angle, Intercept, Point1, intersectpoints);		
		if(!DXFEXPOSEOBJECT->intersectionPointOnLine(endpts[0], endpts[1], endpts[2], endpts[3], intersectpoints[0], intersectpoints[1]))					
		{					
			double dist1 = 0, dist2 = 0;
			dist1 = RMATH2DOBJECT->Pt2Pt_distance(Point1[0], Point1[1], endpts[0], endpts[1]);
			dist2 = RMATH2DOBJECT->Pt2Pt_distance(Point1[0], Point1[1], endpts[2], endpts[3]);				
			if(dist1 < dist2)
			{
				ans[0] = endpts[0]; ans[1] = endpts[1]; ans[2] = Point1[2];
			}
			else
			{
				ans[0] = endpts[2]; ans[1] = endpts[3];  ans[2] = Point1[2];
			}
		}	
		else
		{
			ans[0] = intersectpoints[0]; ans[1] = intersectpoints[1]; ans[2] = Point1[2];
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0017", __FILE__, __FUNCSIG__);}
}

bool CloudComparisionHandler::CreateMeasurementFor3MComponent(bool SurfaceOnFlag, int MeasurementCount, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, int SkipPts1, int SkipPts2, std::list<Shape*>* LineIdList)
{
	try
	{
		if(MeasurementCount < 2)
			MeasurementCount = 2;
		bool MinDistPresent = false, MaxDistPresent = false, firstTime = true, returnFlag = false;
		double Point1[3] = {0}, Point2[3] = {0}, MinDist = 999999, MaxDist = 0, MinDist_Point[6] = {0}, MaxDist_Point[6] = {0}, TmpMinDist = 999999, EndPt[6] = {0};	
	
		int TmpCnt1 = 0;
		map<int,SinglePoint*>::iterator StartPtItem;
		double Centroid[3] = {0};
		for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
		{
			SinglePoint* Spt1 = (*SptItem1).second;
			Centroid[0] += Spt1->X; Centroid[1] += Spt1->Y; Centroid[2] += Spt1->Z;
			if(TmpCnt1 >= SkipPts1)
			{
				TmpCnt1 = 0;
				Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;	
				findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
			}
			else
			{
				TmpCnt1++;
				continue;
			}
			if(firstTime)
				firstTime = false;
			else
			{
				if(MaxDist < TmpMinDist)
				{
					MaxDistPresent = true;
					MaxDist = TmpMinDist;
					MaxDist_Point[0] = EndPt[0]; MaxDist_Point[1] = EndPt[1]; MaxDist_Point[2] = EndPt[2];
					MaxDist_Point[3] = EndPt[3]; MaxDist_Point[4] = EndPt[4]; MaxDist_Point[5] = EndPt[5];	
				}
			}		
			TmpMinDist = 999999;				
			findLineArcIntersectPoint(ArcBestFit, Point1, NumberOfPts, ParentShape2->PointsList, Point2);
			double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
			if(MinDist > dist)
			{
				MinDistPresent = true;
				MinDist = dist;
				MinDist_Point[0] = Point1[0]; MinDist_Point[1] = Point1[1]; MinDist_Point[2] = Point1[2];
				MinDist_Point[3] = Point2[0]; MinDist_Point[4] = Point2[1]; MinDist_Point[5] = Point1[2];
				StartPtItem = SptItem1;
			}
			if(TmpMinDist > dist)
			{
				TmpMinDist = dist;
				EndPt[0] = Point1[0]; EndPt[1] = Point1[1]; EndPt[2] = Point1[2];
				EndPt[3] = Point2[0]; EndPt[4] = Point2[1]; EndPt[5] = Point1[2];
			}
		}
		Centroid[0] = Centroid[0] / ParentShape1->PointsList->Pointscount(); Centroid[1] = Centroid[1] / ParentShape1->PointsList->Pointscount(); Centroid[2] = Centroid[2] / ParentShape1->PointsList->Pointscount();
		if(MinDistPresent)
		{
			returnFlag = true;
			CreateLineMeasureMent(MinDist_Point, &MinDist_Point[3], ParentShape1, ParentShape2, LineIdList, true, 1);
		}

		if(MaxDistPresent)
		{
			CreateLineMeasureMent(MaxDist_Point, &MaxDist_Point[3], ParentShape1, ParentShape2, LineIdList, true, 2);
		}
				

		int PtsCount = ParentShape1->PointsList->Pointscount();
		double *PtsArray = new double[PtsCount * 2];
		int PtsCnt = 0;
		for(PC_ITER SptItem = ParentShape1->PointsList->getList().begin(); SptItem != ParentShape1->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			PtsArray[PtsCnt++] = Spt->X;
			PtsArray[PtsCnt++] = Spt->Y;
		}
		double Plength = 0;
		if(SurfaceOnFlag)
			BESTFITOBJECT->getLengthofShape(PtsArray, PtsCount, &Plength, -1, false);
		else
			BESTFITOBJECT->getLengthofShape(PtsArray, PtsCount, &Plength, 1, false);
		delete [] PtsArray;


		map<int, SinglePoint*>::iterator TempItem = StartPtItem;
		int AntiClockWiseCount = 0;
		SinglePoint* Spt1 = (*StartPtItem).second;
		for(int i = 0; i < 7; i++)
		{
			TempItem++;
			if(TempItem == ParentShape1->PointsList->getList().end())
			{
				TempItem = ParentShape1->PointsList->getList().begin();
			}
			SinglePoint* Spt2 = (*TempItem).second;
			double LineEndPts1[6] = {Spt1->X, Spt1->Y, Centroid[2], Centroid[0], Centroid[1], Centroid[2]};
			double LineEndPts2[6] = {Spt2->X, Spt2->Y, Centroid[2], Centroid[0], Centroid[1], Centroid[2]};
			double DirCosine1[3] = {0}, DirCosine2[3] = {0};
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(LineEndPts1, DirCosine1);
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(LineEndPts2, DirCosine2);
			double CrossVector = DirCosine1[0] * DirCosine2[1] - DirCosine1[1] * DirCosine2[0];
			if(CrossVector > 0)
				AntiClockWiseCount++;
			else
				AntiClockWiseCount--;
		}	
		bool IncrementPtrFlag = false;
		if(AntiClockWiseCount > 0)
			IncrementPtrFlag = true;

		double deltaLength = Plength / (MeasurementCount + 1);
		int Pre_Count = 0;
		for(int i = 1; i <= MeasurementCount; i++)
		{
			findNextPoint_UsingLength(SurfaceOnFlag, IncrementPtrFlag, StartPtItem, deltaLength * i, ArcBestFit, NumberOfPts, ParentShape1->PointsList, Point1, &Pre_Count);
			double dist1 = 999999, EndPt[3] = {0};
			findLineArcIntersectPoint(ArcBestFit, Point1, NumberOfPts, ParentShape2->PointsList, Point2);
			double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
			if(dist1 > dist)
			{
				dist1 = dist;
				EndPt[0] = Point2[0]; EndPt[1] = Point2[1]; EndPt[2] = Point2[2];
			}
			CreateLineMeasureMent(Point1, EndPt, ParentShape1, ParentShape2, LineIdList);
		}
		return returnFlag;
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0018", __FILE__, __FUNCSIG__); return false;}
}

void CloudComparisionHandler::findNextPoint(double* Center, double* Centroid, double Theta, bool ArcBestFit, int BestFitPts, PointCollection *PtsList, double* NextPt)
{
	try
	{
		if(Theta > 2 * M_PI)
			Theta -= 2 * M_PI;
		map<int, SinglePoint*>::iterator SptItem;
		double dist1 = 999999;
		for(PC_ITER SptItem1 = PtsList->getList().begin(); SptItem1 != PtsList->getList().end(); SptItem1++)
		{
			SinglePoint* Spt1 = (*SptItem1).second;
			double Tmpangle = RMATH2DOBJECT->ray_angle(Centroid[0], Centroid[1], Spt1->X, Spt1->Y);
			if(abs(Tmpangle - Theta) > M_PI_2) continue;
			double Intercept = RMATH2DOBJECT->InterceptOfline(Theta, Centroid);
			double dist = RMATH2DOBJECT->Pt2Line_Dist(Spt1->X, Spt1->Y, Theta, Intercept);
			if(dist1 > dist)
			{
				dist1 = dist;
				SptItem = SptItem1;
				NextPt[0] = Spt1->X; NextPt[1] = Spt1->Y; NextPt[2] = Spt1->Z;
			}
		}
		
		double Distance = 0;
		int PtsCount = BestFitPts * 2 + 1;
		double TempPoint[3] = {((*SptItem).second)->X, ((*SptItem).second)->Y, ((*SptItem).second)->Z};
		if(PtsCount < 3 || PtsCount > PtsList->Pointscount() - 1) return;
		double *PtsArray = new double[PtsCount * 2];
		int count = BestFitPts;
		for(int i = 0; i < count; i++)
		{
			if(SptItem == PtsList->getList().begin())
			{
				SptItem = PtsList->getList().end();
			}
			SptItem--;
		}
		for(int i = 0; i < PtsCount; i++)
		{
			if(SptItem == PtsList->getList().end())
			{
				SptItem = PtsList->getList().begin();
			}
			PtsArray[2 * i] = ((*SptItem).second)->X; PtsArray[2 * i + 1] = ((*SptItem).second)->Y;
			SptItem++;
		}
		if(ArcBestFit)
		{
			double ans[3] = {0}, angles[2] = {0}, endpts[4] = {0};
			if(BESTFITOBJECT->Circle_BestFit(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
			{
				RMATH2DOBJECT->ArcBestFitAngleCalculation(PtsArray, PtsCount, &ans[0], ans[2], &angles[0]);
				RMATH2DOBJECT->ArcEndPoints(&ans[0], ans[2], angles[0], angles[1], &endpts[0]);
				calculateIntersectionPtOnArc(Centroid, ans, ans[2], angles, endpts, NextPt);	
				Center[0] = ans[0];	Center[1] = ans[1]; Center[2] = NextPt[2];
			}
			else
			{
				double p1[2] = {0}, p2[2] = {0};
				BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());		
				RMATH2DOBJECT->LineEndPoints(PtsArray, PtsCount, &p1[0], &p2[0], ans[0]);		
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &endpts[0]);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &endpts[2]);
				calculateIntersectiLine(Centroid, ans[0], ans[1], endpts, NextPt);
			}
		}
		else
		{
			double ans[2] = {0}, p1[2] = {0}, p2[2] = {0}, endPt[4] = {0};
			BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());		
			RMATH2DOBJECT->LineEndPoints(PtsArray, PtsCount, &p1[0], &p2[0], ans[0]);		
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &endPt[0]);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &endPt[2]);
			calculateIntersectiLine(Centroid, ans[0], ans[1], endPt, NextPt);			
		}	
		delete [] PtsArray;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0013", __FILE__, __FUNCSIG__);}
}

void CloudComparisionHandler::UpdateDeviationParamFor3MComponent(bool SurfaceOnFlag, int MeasurementCount, int SkipPts1, int SkipPts2, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, std::list<int> LineIdList)
{
	try
	{
		std::map<int, UpdatedPts> PointsIdList;
		bool MinDistPresent = false, MaxDistPresent = false, firstTime = true;
		double Point1[3] = {0}, Point2[3] = {0}, MinDist = 999999, MaxDist = 0, MinDist_Point[6] = {0}, MaxDist_Point[6] = {0}, TmpMinDist = 999999, EndPt[6] = {0};	
		if(MeasurementCount < 2)
			MeasurementCount = 2;
		int TmpCnt1 = 0;
		double Centroid[3] = {0};
		map<int,SinglePoint*>::iterator StartPtItem;
		for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
		{
			SinglePoint* Spt1 = (*SptItem1).second;
			Centroid[0] += Spt1->X; Centroid[1] += Spt1->Y; Centroid[2] += Spt1->Z;
			if(TmpCnt1 >= SkipPts1)
			{
				TmpCnt1 = 0;
				Point1[0] = Spt1->X; Point1[1] = Spt1->Y; Point1[2] = Spt1->Z;				
				findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
			}
			else
			{
				TmpCnt1++;
				continue;
			}
			if(firstTime)
				firstTime = false;
			else
			{
				if(MaxDist < TmpMinDist)
				{
					MaxDistPresent = true;
					MaxDist = TmpMinDist;
					MaxDist_Point[0] = EndPt[0]; MaxDist_Point[1] = EndPt[1]; MaxDist_Point[2] = EndPt[2];
					MaxDist_Point[3] = EndPt[3]; MaxDist_Point[4] = EndPt[4]; MaxDist_Point[5] = EndPt[5];	
				}
			}
			TmpMinDist = 999999;				
			findLineArcIntersectPoint(ArcBestFit, Point1, NumberOfPts, ParentShape2->PointsList, Point2);
			double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
			if(MinDist > dist)
			{
				MinDistPresent = true;
				MinDist = dist;
				MinDist_Point[0] = Point1[0]; MinDist_Point[1] = Point1[1]; MinDist_Point[2] = Point1[2];
				MinDist_Point[3] = Point2[0]; MinDist_Point[4] = Point2[1]; MinDist_Point[5] = Point1[2];
				StartPtItem = SptItem1;
			}
			if(TmpMinDist > dist)
			{
				TmpMinDist = dist;
				EndPt[0] = Point1[0]; EndPt[1] = Point1[1]; EndPt[2] = Point1[2];
				EndPt[3] = Point2[0]; EndPt[4] = Point2[1]; EndPt[5] = Point1[2];
			}
		}
		
		Centroid[0] = Centroid[0] / ParentShape1->PointsList->Pointscount(); Centroid[1] = Centroid[1] / ParentShape1->PointsList->Pointscount(); Centroid[2] = Centroid[2] / ParentShape1->PointsList->Pointscount();
		if(MinDistPresent)
		{
			if(LineIdList.size() > 0)
			{
				std::list<int>::iterator itr = LineIdList.begin();
				int LineId = *itr;
				LineIdList.remove(LineId);
				UpdatedPts LineEndPts;
				LineEndPts.EndPts[0] = MinDist_Point[0]; LineEndPts.EndPts[1] = MinDist_Point[1]; LineEndPts.EndPts[2] = MinDist_Point[2];
				LineEndPts.EndPts[3] = MinDist_Point[3]; LineEndPts.EndPts[4] = MinDist_Point[4]; LineEndPts.EndPts[5] = MinDist_Point[5];
				PointsIdList[LineId] = LineEndPts;
			}
		}
		if(MaxDistPresent)
		{
			if(LineIdList.size() > 0)
			{
				std::list<int>::iterator itr = LineIdList.begin();
				int LineId = *itr;
				LineIdList.remove(LineId);
				UpdatedPts LineEndPts;
				LineEndPts.EndPts[0] = MaxDist_Point[0]; LineEndPts.EndPts[1] = MaxDist_Point[1]; LineEndPts.EndPts[2] = MaxDist_Point[2];
				LineEndPts.EndPts[3] = MaxDist_Point[3]; LineEndPts.EndPts[4] = MaxDist_Point[4]; LineEndPts.EndPts[5] = MaxDist_Point[5];
				PointsIdList[LineId] = LineEndPts;
			}
		}
					
		int PtsCount = ParentShape1->PointsList->Pointscount();
		double *PtsArray = new double[PtsCount * 2];
		int PtsCnt = 0;
		for(PC_ITER SptItem = ParentShape1->PointsList->getList().begin(); SptItem != ParentShape1->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			PtsArray[PtsCnt++] = Spt->X;
			PtsArray[PtsCnt++] = Spt->Y;
		}
		double Plength = 0;
		BESTFITOBJECT->getLengthofShape(PtsArray, PtsCount, &Plength, 1, false);
		delete [] PtsArray;

		map<int, SinglePoint*>::iterator TempItem = StartPtItem;
		int AntiClockWiseCount = 0;
		SinglePoint* Spt1 = (*StartPtItem).second;
		for(int i = 0; i < 7; i++)
		{
			TempItem++;
			if(TempItem == ParentShape1->PointsList->getList().end())
			{
				TempItem = ParentShape1->PointsList->getList().begin();
			}
			SinglePoint* Spt2 = (*TempItem).second;
			double LineEndPts1[6] = {Spt1->X, Spt1->Y, Centroid[2], Centroid[0], Centroid[1], Centroid[2]};
			double LineEndPts2[6] = {Spt2->X, Spt2->Y, Centroid[2], Centroid[0], Centroid[1], Centroid[2]};
			double DirCosine1[3] = {0}, DirCosine2[3] = {0};
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(LineEndPts1, DirCosine1);
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(LineEndPts2, DirCosine2);
			double CrossVector = DirCosine1[0] * DirCosine2[1] - DirCosine1[1] * DirCosine2[0];
			if(CrossVector > 0)
				AntiClockWiseCount++;
			else
				AntiClockWiseCount--;
		}	
		bool IncrementPtrFlag = false;
		if(AntiClockWiseCount > 0)
			IncrementPtrFlag = true;

		double deltaLength = Plength / (MeasurementCount + 1);
		int Pre_Count = 0;
		for(int i = 1; i <= MeasurementCount; i++)
		{
			findNextPoint_UsingLength(SurfaceOnFlag, IncrementPtrFlag, StartPtItem, deltaLength * i, ArcBestFit, NumberOfPts, ParentShape1->PointsList, Point1, &Pre_Count);
			double dist1 = 999999, EndPt[3] = {0};
			findLineArcIntersectPoint(ArcBestFit, Point1, NumberOfPts, ParentShape2->PointsList, Point2);
			double dist = RMATH2DOBJECT->Pt2Pt_distance(Point1, Point2);
			if(dist1 > dist)
			{
				dist1 = dist;
				EndPt[0] = Point2[0]; EndPt[1] = Point2[1]; EndPt[2] = Point2[2];
			}
			std::list<int>::iterator itr = LineIdList.begin();
			int LineId = *itr;
			LineIdList.remove(LineId);
			UpdatedPts LineEndPts;
			LineEndPts.EndPts[0] = Point1[0]; LineEndPts.EndPts[1] = Point1[1]; LineEndPts.EndPts[2] = Point1[2];
			LineEndPts.EndPts[3] = EndPt[0]; LineEndPts.EndPts[4] = EndPt[1]; LineEndPts.EndPts[5] = EndPt[2];
			PointsIdList[LineId] = LineEndPts;
		}
		//update all lines created by cloudPts comparision...
		for(std::map<int, UpdatedPts>::iterator mapItr = PointsIdList.begin(); mapItr != PointsIdList.end(); mapItr++)
		{
			UpdatedPts LnPts = (*mapItr).second;
			int LineId = (*mapItr).first;
			Shape* Lineshape = (Shape*)(MAINDllOBJECT->getShapesList().getList()[LineId]);
			((Line*)Lineshape)->setPoint1(Vector(LnPts.EndPts[0], LnPts.EndPts[1], LnPts.EndPts[2]));
			((Line*)Lineshape)->setPoint2(Vector(LnPts.EndPts[3], LnPts.EndPts[4], LnPts.EndPts[2]));		
			((Line*)Lineshape)->calculateAttributes();	
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0011", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::findLineArcIntersectPoint(bool ArcBestFit, double* ProjectedPt, int BestFitPts, PointCollection *PtsList, double* NextPt)
{
	try
	{
		map<int, SinglePoint*>::iterator SptItem;
		double dist1 = 999999;
		for(PC_ITER SptItem1 = PtsList->getList().begin(); SptItem1 != PtsList->getList().end(); SptItem1++)
		{
			SinglePoint* Spt1 = (*SptItem1).second;
			double dist = RMATH2DOBJECT->Pt2Pt_distance(Spt1->X, Spt1->Y, ProjectedPt[0], ProjectedPt[1]);
			if(dist1 > dist)
			{
				dist1 = dist;
				SptItem = SptItem1;
				NextPt[0] = Spt1->X; NextPt[1] = Spt1->Y; NextPt[2] = Spt1->Z;
			}
		}
		
		double Distance = 0;
		int PtsCount = BestFitPts * 2 + 1;
		double TempPoint[3] = {((*SptItem).second)->X, ((*SptItem).second)->Y, ((*SptItem).second)->Z};
		if(PtsCount < 3 || PtsCount > PtsList->Pointscount() - 1) return;
		double *PtsArray = new double[PtsCount * 2];
		int count = BestFitPts;
		for(int i = 0; i < count; i++)
		{
			if(SptItem == PtsList->getList().begin())
			{
				SptItem = PtsList->getList().end();
			}
			SptItem--;
		}
		for(int i = 0; i < PtsCount; i++)
		{
			if(SptItem == PtsList->getList().end())
			{
				SptItem = PtsList->getList().begin();
			}
			PtsArray[2 * i] = ((*SptItem).second)->X; PtsArray[2 * i + 1] = ((*SptItem).second)->Y;
			SptItem++;
		}
		
		if(ArcBestFit)
		{
			double ans[3] = {0}, angles[2] = {0}, endpts[4] = {0};
			if(BESTFITOBJECT->Circle_BestFit(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
			{
				RMATH2DOBJECT->ArcBestFitAngleCalculation(PtsArray, PtsCount, &ans[0], ans[2], &angles[0]);
				RMATH2DOBJECT->ArcEndPoints(&ans[0], ans[2], angles[0], angles[1], &endpts[0]);
				calculateIntersectionPtOnArc(ProjectedPt, ans, ans[2], angles, endpts, NextPt);	
			}
			else
			{
				double p1[2] = {0}, p2[2] = {0};
				BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());		
				RMATH2DOBJECT->LineEndPoints(PtsArray, PtsCount, &p1[0], &p2[0], ans[0]);		
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &endpts[0]);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &endpts[2]);
				calculateIntersectiLine(ProjectedPt, ans[0], ans[1], endpts, NextPt);
			}		
		}
		else
		{
			double ans[2] = {0}, p1[2] = {0}, p2[2] = {0}, endpts[4] = {0};
			BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());		
			RMATH2DOBJECT->LineEndPoints(PtsArray, PtsCount, &p1[0], &p2[0], ans[0]);		
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p1[0], &endpts[0]);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], &p2[0], &endpts[2]);
			calculateIntersectiLine(ProjectedPt, ans[0], ans[1], endpts, NextPt);
		}
		delete [] PtsArray;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0013", __FILE__, __FUNCSIG__);}
}

void CloudComparisionHandler::findNextPoint_UsingLength(bool SurfaceOn, bool IncrementPtr, map<int, SinglePoint*>::iterator StartPtItem, double MinLength, bool ArcBestFit, int BestFitPts, PointCollection *PtsList, double* NextPt, int* PreviousCount)
{
	try
	{
		int LengthCnt = *PreviousCount - 1;
		*PreviousCount = 0;
		int PtsCount = PtsList->Pointscount();
		double *PtsArray = new double[2 * PtsCount];
		int PtsCnt = 0;
		double Plength = 0;
		if(IncrementPtr)
		{	
			if(StartPtItem == PtsList->getList().end())
			{
				StartPtItem = PtsList->getList().begin();
			}
			SinglePoint* Spt = (*StartPtItem).second;
			PtsArray[PtsCnt++] = Spt->X;
			PtsArray[PtsCnt++] = Spt->Y;
			for(int i = 0; i < PtsCount; i++)
			{
				(*PreviousCount)++;
				StartPtItem++;
				if(StartPtItem == PtsList->getList().end())
				{
					StartPtItem = PtsList->getList().begin();
				}
				Spt = (*StartPtItem).second;
				PtsArray[PtsCnt++] = Spt->X;
				PtsArray[PtsCnt++] = Spt->Y;
				NextPt[0] = Spt->X; NextPt[1] = Spt->Y; NextPt[2] = Spt->Z;
				Plength = 0;
				if((*PreviousCount) >= LengthCnt)
				{
					if(SurfaceOn)
						BESTFITOBJECT->getLengthofShape(PtsArray, PtsCnt / 2, &Plength, -1, false);
					else
						BESTFITOBJECT->getLengthofShape(PtsArray, PtsCnt / 2, &Plength, 1, false);
				}
				if(MinLength < Plength) 
					break;
	
			}
		}
		else
		{
			if(StartPtItem == PtsList->getList().begin())
			{
				StartPtItem = PtsList->getList().end();
			}
			SinglePoint* Spt = (*StartPtItem).second;
			PtsArray[PtsCnt++] = Spt->X;
			PtsArray[PtsCnt++] = Spt->Y;
			for(int i = 0; i < PtsCount; i++)
			{
				(*PreviousCount)++;
				StartPtItem--;
				if(StartPtItem == PtsList->getList().begin())
				{
					StartPtItem = PtsList->getList().end();
					StartPtItem--;
				}
				Spt = (*StartPtItem).second;
				PtsArray[PtsCnt++] = Spt->X;
				PtsArray[PtsCnt++] = Spt->Y;
				NextPt[0] = Spt->X; NextPt[1] = Spt->Y; NextPt[2] = Spt->Z;
				Plength = 0;
				if((*PreviousCount) >= LengthCnt)
				{
					if(SurfaceOn)
						BESTFITOBJECT->getLengthofShape(PtsArray, PtsCnt / 2, &Plength, -1, false);
					else
						BESTFITOBJECT->getLengthofShape(PtsArray, PtsCnt / 2, &Plength, 1, false);
				}
				if(MinLength < Plength) 
					break;
			}
		}
		delete [] PtsArray;	
		findProjectedPoint(ArcBestFit, BestFitPts, StartPtItem, PtsList, NextPt);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0017", __FILE__, __FUNCSIG__);}
}

void CloudComparisionHandler::Point_CloudptComparatorParm(double Angle, bool ArcBestFit, int NumberOfPts)
{
	try
	{
		ShapeWithList* ParentShape1, *ParentShape2, *ParentShape3;
		ParentShape1 = NULL; ParentShape2 = NULL; ParentShape3 = NULL;
		bool ShapePresent = false;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS && CShape->PointsList->Pointscount() > 0)
			{				
				if(ParentShape1 == NULL)
					ParentShape1 = CShape;
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || CShape->ShapeType == RapidEnums::SHAPETYPE::XLINE || CShape->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{				
				if(ParentShape2 == NULL)
					ParentShape2 = CShape;
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT || CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
			{				
				if(ParentShape3 == NULL)
					ParentShape3 = CShape;
			}
		}
		if(ParentShape1 != NULL && ParentShape2 != NULL && ParentShape3 != NULL)
		{
			std::list<Shape*> LineIdList;
			int SkipPts = 0;
			if(ParentShape1->PointsList->Pointscount() > MaxInnerPtsAllowed)
				SkipPts = ParentShape1->PointsList->Pointscount() / MaxInnerPtsAllowed;
			if(CreateMeasurementFor_Delphi_Component(Angle, ParentShape1, ParentShape2, ParentShape3, ArcBestFit, NumberOfPts, SkipPts, &LineIdList))
			{
				CloudComparisionAction::SetCloudCompareParam(Angle, ParentShape1, ParentShape2, ParentShape3, ArcBestFit, NumberOfPts, LineIdList);
			}
		}
		else 
		{
			//message box please select one cloud point, one line and one point shape for this tool..
			MAINDllOBJECT->ShowMsgBoxString("CloudComparisionHandler02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0018", __FILE__, __FUNCSIG__); }
}

bool CloudComparisionHandler::CreateMeasurementFor_Delphi_Component(double angle, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, ShapeWithList* ParentShape3, bool ArcBestFit, int NumberOfPts, int SkipPts, std::list<Shape*>* LineIdList)
{
	try
	{
		bool returnFlag = false;
		if(angle < 0.36) return returnFlag;
		int MeasureCount = 360 / angle;	
		angle = angle * M_PI / 180;
		double Slope = ((Line*)ParentShape2)->Angle();
		if(Slope > M_PI_2)
			Slope -= M_PI;
		else if(Slope < -M_PI_2)
			Slope += M_PI;
		double RayAngle = 0, PtDist = 9999;
		double CenterPoint[3] = {((RPoint*)ParentShape3)->getPosition()->getX(), ((RPoint*)ParentShape3)->getPosition()->getY(), ((RPoint*)ParentShape3)->getPosition()->getZ()};
		for(int i = 0; i < MeasureCount; i++)
		{
			RayAngle = Slope + angle * i;
			double CenterPoint2[3] = {PtDist * cos(RayAngle) + CenterPoint[0], PtDist * sin(RayAngle) + CenterPoint[1], 0}; 
			double Intercept = RMATH2DOBJECT->InterceptOfline(RayAngle, CenterPoint);
			double dist1 = 999999, EndPt[3] = {0};
			bool CreateMeasure = false;
			int TmpPtsCnt = 0;
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				if(SkipPts > TmpPtsCnt)
				{
					TmpPtsCnt++;
					continue;
				}
				else
					TmpPtsCnt = 0;
				SinglePoint* Spt1 = (*SptItem1).second;
				double Point1[3] = {Spt1->X, Spt1->Y, Spt1->Z}, intersectpoints[3] = {0};	
				findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(RayAngle, Intercept, Point1, intersectpoints);
				if(!DXFEXPOSEOBJECT->intersectionPointOnLine(CenterPoint2[0], CenterPoint2[1], CenterPoint[0], CenterPoint[1], intersectpoints[0], intersectpoints[1])) continue;
				double dist = RMATH2DOBJECT->Pt2Pt_distance(intersectpoints, Point1);
				if(dist1 > dist)
				{
					CreateMeasure = true;
					dist1 = dist;
					EndPt[0] = intersectpoints[0]; EndPt[1] = intersectpoints[1]; EndPt[2] = CenterPoint[2];
				}
			}
			if(CreateMeasure)
			{
				CreateLineMeasureMent(CenterPoint, EndPt, ParentShape1, ParentShape2, LineIdList, false, 0, ParentShape3);
				returnFlag = true;
			}	
		}
		return returnFlag;
	}
	catch(...){ return false; MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0019", __FILE__, __FUNCSIG__); }
}

void CloudComparisionHandler::UpdateMeasurementFor_Delphi_Component(double angle, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, ShapeWithList* ParentShape3, bool ArcBestFit, int NumberOfPts, int SkipPts, std::list<int> LineIdList)
{
	try
	{
		std::map<int, UpdatedPts> PointsIdList;
		if(angle < 0.36) return;
		int MeasureCount = 360 / angle;	
		angle = angle * M_PI / 180;
		double Slope = ((Line*)ParentShape2)->Angle();
		if(Slope > M_PI_2)
			Slope -= M_PI;
		else if(Slope < -M_PI_2)
			Slope += M_PI;
		double RayAngle = 0, PtDist = 9999;
		double CenterPoint[3] = {((RPoint*)ParentShape3)->getPosition()->getX(), ((RPoint*)ParentShape3)->getPosition()->getY(), ((RPoint*)ParentShape3)->getPosition()->getZ()};
		for(int i = 0; i < MeasureCount; i++)
		{
			if(LineIdList.size() < 1) break;
			RayAngle = Slope + angle * i;
			double CenterPoint2[3] = {PtDist * cos(RayAngle) + CenterPoint[0], PtDist * sin(RayAngle) + CenterPoint[1], 0}; 
			double Intercept = RMATH2DOBJECT->InterceptOfline(RayAngle, CenterPoint);
			double dist1 = 999999, EndPt[3] = {0};
			bool CreateMeasure = false;
			int TmpPtsCnt = 0;
			for(PC_ITER SptItem1 = ParentShape1->PointsList->getList().begin(); SptItem1 != ParentShape1->PointsList->getList().end(); SptItem1++)
			{
				if(SkipPts > TmpPtsCnt)
				{
					TmpPtsCnt++;
					continue;
				}
				else
					TmpPtsCnt = 0;
				SinglePoint* Spt1 = (*SptItem1).second;
				double Point1[3] = {Spt1->X, Spt1->Y, Spt1->Z}, intersectpoints[3] = {0};	
				findProjectedPoint(ArcBestFit, NumberOfPts, SptItem1, ParentShape1->PointsList, Point1);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(RayAngle, Intercept, Point1, intersectpoints);
				if(!DXFEXPOSEOBJECT->intersectionPointOnLine(CenterPoint2[0], CenterPoint2[1], CenterPoint[0], CenterPoint[1], intersectpoints[0], intersectpoints[1])) continue;
				double dist = RMATH2DOBJECT->Pt2Pt_distance(intersectpoints, Point1);
				if(dist1 > dist)
				{
					CreateMeasure = true;
					dist1 = dist;
					EndPt[0] = intersectpoints[0]; EndPt[1] = intersectpoints[1]; EndPt[2] = CenterPoint[2];
				}
			}
			if(CreateMeasure)
			{
				std::list<int>::iterator itr = LineIdList.begin();
				int LineId = (*itr);
				LineIdList.remove(*itr);
				UpdatedPts LineEndPts;
				LineEndPts.EndPts[0] = CenterPoint[0]; LineEndPts.EndPts[1] = CenterPoint[1]; LineEndPts.EndPts[2] = CenterPoint[2];
				LineEndPts.EndPts[3] = EndPt[0]; LineEndPts.EndPts[4] = EndPt[1]; LineEndPts.EndPts[5] = EndPt[2];
				PointsIdList[LineId] = LineEndPts;
			}	
		}
		//update all lines created by cloudPts comparision...
		for(std::map<int, UpdatedPts>::iterator mapItr = PointsIdList.begin(); mapItr != PointsIdList.end(); mapItr++)
		{
			UpdatedPts LnPts = (*mapItr).second;
			int LineId = (*mapItr).first;
			Shape* Lineshape = (Shape*)(MAINDllOBJECT->getShapesList().getList()[LineId]);
			((Line*)Lineshape)->setPoint1(Vector(LnPts.EndPts[0], LnPts.EndPts[1], LnPts.EndPts[2]));
			((Line*)Lineshape)->setPoint2(Vector(LnPts.EndPts[3], LnPts.EndPts[4], LnPts.EndPts[2]));		
			((Line*)Lineshape)->calculateAttributes();	
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0020", __FILE__, __FUNCSIG__); }
}