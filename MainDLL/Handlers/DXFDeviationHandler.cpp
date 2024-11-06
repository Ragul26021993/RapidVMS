#include "stdafx.h"
#include "DXFDeviationHandler.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\SinglePoint.h"
#include "..\Measurement\DimBase.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\DXF_DeviationAction.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
DXFDeviationHandler::DXFDeviationHandler()
{
	try
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DXF_DEVIATION_HANDLER;
		runningPartprogramValid = false;
		this->SptCollection = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
DXFDeviationHandler::~DXFDeviationHandler()
{
	try
	{
		delete SptCollection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0002", __FILE__, __FUNCSIG__); }
}

//Mousemove....//
void DXFDeviationHandler::mouseMove()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0003", __FILE__, __FUNCSIG__); }
}

//Handle the left mousedown....//
void DXFDeviationHandler::LmouseDown()
{
	try{}
	catch(...){  MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0004", __FILE__, __FUNCSIG__); }
}

//Draw the parallel arc..//
void DXFDeviationHandler::draw(int windowno, double WPixelWidth)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0005", __FILE__, __FUNCSIG__); }
}

void DXFDeviationHandler::EscapebuttonPress()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0006", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data.....//
void DXFDeviationHandler::PartProgramData()
{
	try
	{
		std::list<int> SelectedShapeIdlist;
		std::list<int> LineIdlist;
		double tolerance = 0, interval = 0, UpperCutoff = 0;
		int IntervalType = 0;
		bool ClosedLoop = false, TwoD_Deviation = false;
		((DXF_DeviationAction*)PPCALCOBJECT->getCurrentAction())->GetDXF_DeviationParam(&SelectedShapeIdlist, &LineIdlist, &UpperCutoff, &tolerance, &interval, &IntervalType, &ClosedLoop, &TwoD_Deviation);
		MAINDllOBJECT->ClearShapeSelections();
		MAINDllOBJECT->selectShape(&SelectedShapeIdlist);
		if(interval != 0)
		{
			CreateSortedPts_ShapeIdRelation(TwoD_Deviation);
			CreateSortedPtsList(TwoD_Deviation, interval);
		}
		UpdateDXFDeviationParam(UpperCutoff, LineIdlist, tolerance, interval, IntervalType, ClosedLoop, TwoD_Deviation);
		MAINDllOBJECT->ClearShapeSelections();
		PPCALCOBJECT->partProgramReached();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0007", __FILE__, __FUNCSIG__); }
}

//Set the  parallel arc parameters...
void DXFDeviationHandler::LmaxmouseDown()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0008", __FILE__, __FUNCSIG__); }
}

void DXFDeviationHandler::DXFDeviationData(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation)
{
	try
	{
		std::list<int> ShapeIdlist;
		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			ShapeIdlist.push_back(CShape->getId());
		}
		if(interval != 0)
		{
			CreateSortedPts_ShapeIdRelation(TwoD_Deviation);
			CreateSortedPtsList(TwoD_Deviation, interval);
		}
		if(CalculateDeviation(UpperCutoff, tolerance, interval, IntervalType, ClosedLoop, TwoD_Deviation))
		{
			DXF_DeviationAction::SetDXF_DeviationParam(UpperCutoff, tolerance, interval, IntervalType, ClosedLoop, TwoD_Deviation, &ShapeIdlist, CreatedShapeidlist, createdMeasureIdlist);
			HELPEROBJECT->MultiFeatureScan = false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0010", __FILE__, __FUNCSIG__); }
}

void DXFDeviationHandler::CreateSortedPts_ShapeIdRelation(bool TwoD_Deviation)
{
	try
	{	
		int totalptsCount = 0, SkipPts = 0;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				totalptsCount += CShape->PointsList->Pointscount();
			}
		}
		if(totalptsCount > 1000)
			SkipPts = totalptsCount / 600;
		SptCollection = new ShapeId_PointsCollection();
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				int TmpCnt = 0;
				for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
				{
					if(SkipPts <= TmpCnt)
					{
						SinglePoint* Spt = (*SptItem).second;
						SinglePoint* Spt1 = new SinglePoint(Spt->X, Spt->Y, Spt->Z);
						Spt1->PtID = CShape->getId();
						SptCollection->Addpoint(Spt1);
						TmpCnt = 0;
					}
					else
						TmpCnt++;
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0011", __FILE__, __FUNCSIG__); }
}

//modify sorting algorithm..
void DXFDeviationHandler::CreateSortedPtsList(bool TwoD_Deviation, double interval)
{
	try
	{
		if(SptCollection->PtsList.size() < 1) return;
		bool Firsttime = true;
		SinglePoint* FirstPoint = new SinglePoint();
		int mapId = 0;
		double dist = 0;
		for(std::map<int, SinglePoint*>::iterator itr = SptCollection->PtsList.begin(); itr != SptCollection->PtsList.end(); itr++)
		{
			if(Firsttime)
			{
				Firsttime = false;
				FirstPoint->X = (*itr).second->X; FirstPoint->Y = (*itr).second->Y; FirstPoint->Z = (*itr).second->Z; FirstPoint->PtID = (*itr).second->PtID; mapId = (*itr).first;
				break;
			}
			else
			{
				if(FirstPoint->X > (*itr).second->X)
				{
					FirstPoint->X = (*itr).second->X; FirstPoint->Y = (*itr).second->Y; FirstPoint->Z = (*itr).second->Z; FirstPoint->PtID = (*itr).second->PtID; mapId = (*itr).first;
				}
			}
		}
		bool SortAgain = false;
		int Cnt = 0, newMapId = 0; dist = 0;
		std::map<int, SinglePoint*> tempPtslist;
		tempPtslist[Cnt++] = FirstPoint;
		SinglePoint* Spt2 = SptCollection->PtsList[mapId];
		SptCollection->PtsList.erase(mapId);
		delete Spt2;
		SinglePoint* newFirstPt = new SinglePoint();
		double ShortestPt[3] = {FirstPoint->X, FirstPoint->Y, FirstPoint->Z};
		while(SptCollection->PtsList.size() != 0)
		{
			Firsttime = true;
			SinglePoint* Spt1 = new SinglePoint();
			for(std::map<int, SinglePoint*>::iterator itr = SptCollection->PtsList.begin(); itr != SptCollection->PtsList.end(); itr++)
			{
				if(Firsttime)
				{
					dist = RMATH2DOBJECT->Pt2Pt_distance((*itr).second->X, (*itr).second->Y, ShortestPt[0], ShortestPt[1]);
					Firsttime = false;
					Spt1->X = (*itr).second->X; Spt1->Y = (*itr).second->Y; Spt1->Z = (*itr).second->Z; Spt1->PtID = (*itr).second->PtID; mapId = (*itr).first;
				}
				else
				{
					double dist1 = RMATH2DOBJECT->Pt2Pt_distance((*itr).second->X, (*itr).second->Y, ShortestPt[0], ShortestPt[1]);
					if(dist > dist1)
					{
						dist = dist1;
						Spt1->X = (*itr).second->X; Spt1->Y = (*itr).second->Y; Spt1->Z = (*itr).second->Z; Spt1->PtID = (*itr).second->PtID; mapId = (*itr).first;
					}
					else if(dist - dist1 > interval)
					{
						newFirstPt->X = (*itr).second->X; newFirstPt->Y = (*itr).second->Y; newFirstPt->Z = (*itr).second->Z; newFirstPt->PtID = (*itr).second->PtID; newMapId = (*itr).first;
						SortAgain = true;
					}
				}
			}
			SinglePoint* Spt3 = SptCollection->PtsList[mapId];
			tempPtslist[Cnt++] = Spt1;
			ShortestPt[0] = Spt1->X; ShortestPt[1] = Spt1->Y; ShortestPt[2] = Spt1->Z;
			SptCollection->PtsList.erase(mapId);
			delete Spt3;
		}	

		//sort again with newfirstpoint............
		Cnt = 0;
		if(SortAgain)
		{
			SptCollection->SortedPtsList[Cnt++] = newFirstPt;
			SinglePoint* Spt2 = tempPtslist[mapId];
			ShortestPt[0] = Spt2->X; ShortestPt[1] = Spt2->Y; ShortestPt[2] = Spt2->Z;
			tempPtslist.erase(mapId);
			delete Spt2;
			while(tempPtslist.size() != 0)
			{
				Firsttime = true;
				SinglePoint* Spt1 = new SinglePoint();
				for(std::map<int, SinglePoint*>::iterator itr = tempPtslist.begin(); itr != tempPtslist.end(); itr++)
				{
					if(Firsttime)
					{
						dist = RMATH2DOBJECT->Pt2Pt_distance((*itr).second->X, (*itr).second->Y, ShortestPt[0], ShortestPt[1]);
						Firsttime = false;
						Spt1->X = (*itr).second->X; Spt1->Y = (*itr).second->Y; Spt1->Z = (*itr).second->Z; Spt1->PtID = (*itr).second->PtID; mapId = (*itr).first;
					}
					else
					{
						double dist1 = RMATH2DOBJECT->Pt2Pt_distance((*itr).second->X, (*itr).second->Y, ShortestPt[0], ShortestPt[1]);
						if(dist > dist1)
						{
							dist = dist1;
							Spt1->X = (*itr).second->X; Spt1->Y = (*itr).second->Y; Spt1->Z = (*itr).second->Z; Spt1->PtID = (*itr).second->PtID; mapId = (*itr).first;
						}
					}
				}
				SinglePoint* Spt3 = tempPtslist[mapId];
				SptCollection->SortedPtsList[Cnt++] = Spt1;
				ShortestPt[0] = Spt1->X; ShortestPt[1] = Spt1->Y; ShortestPt[2] = Spt1->Z;
				tempPtslist.erase(mapId);
				delete Spt3;
			}
		}
		else
		{
			delete newFirstPt;
			while(tempPtslist.size() != 0)
			{
				std::map<int, SinglePoint*>::iterator itr = tempPtslist.begin();
				SptCollection->SortedPtsList[Cnt++] = (*itr).second;
				tempPtslist.erase((*itr).first);
			}
		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0010", __FILE__, __FUNCSIG__); }
}

bool DXFDeviationHandler::CalculateDeviation(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation)
{
	try
	{
		int NoOfLines = 0;
		RCollectionBase& ShpCollection = MAINDllOBJECT->getShapesList();
		if(interval != 0)
		{
			double PreviousPtX[2] = {0};
			bool firstintPt = true;
			int TmpCnt = 0, TmpCnt1 = 0;
			for(std::map<int, SinglePoint*>::iterator itr = SptCollection->SortedPtsList.begin(); itr != SptCollection->SortedPtsList.end(); itr++)
			{
				double Pnts[3] = {(*itr).second->X, (*itr).second->Y, (*itr).second->Z}, intersectPts[3] = {0};
				TmpCnt1++;
				if(!firstintPt)
				{
					if(IntervalType == 0)
					{
						if(!(abs(PreviousPtX[0] - Pnts[0]) >= interval))
							continue;
					}
					else if(IntervalType == 1)
					{
						if(!(abs(PreviousPtX[1] - Pnts[1]) >= interval))
							continue;
					}
					else if(IntervalType == 2)
					{
						if(!(RMATH2DOBJECT->Pt2Pt_distance(PreviousPtX, Pnts) >= interval))
							continue;
					}
				}		
				TmpCnt++;
				SinglePoint* DeviationPt = new SinglePoint();
				findAveragePoint(Pnts, interval / 2, &SptCollection->SortedPtsList, DeviationPt);
				double AvgPnt[3] = {DeviationPt->X, DeviationPt->Y, DeviationPt->Z};
				Shape* Cshape = NULL;
				bool ReturnFlag = false, CutOffFlag = false;
				Cshape = DXFEXPOSEOBJECT->getNearestShape_2D(AvgPnt, intersectPts, &ReturnFlag);
				if(abs(UpperCutoff) < 0.00001)
					CutOffFlag = true;
				else
				{
					if(UpperCutoff > RMATH2DOBJECT->Pt2Pt_distance(AvgPnt, intersectPts))
						CutOffFlag = true;
				}
				if(ReturnFlag && CutOffFlag)
				{
					firstintPt = false;
					if(IntervalType == 0)							
						PreviousPtX[0] = Pnts[0];
					else if(IntervalType == 1)	
						PreviousPtX[1] = Pnts[1];
					else if(IntervalType == 2)
					{
						PreviousPtX[0] = Pnts[0]; PreviousPtX[1] = Pnts[1];
					}
					if(NoOfLines >= 1000) return true;
					bool negativeMeasurement = false;
					if(ClosedLoop)
					{
						if(DXFEXPOSEOBJECT->getNumberOfIntersectionPoints(ShpCollection, AvgPnt) % 2 != 0)
							negativeMeasurement = true;
					}
					NoOfLines++;
					//create deviation line...
					Create2DLine_WithoutAction(AvgPnt, intersectPts);
					RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
					item--;
					ShapeWithList* Shp = (ShapeWithList*)((*item).second);
					CreatedShapeidlist.push_back(Shp);
					Cshape->addChild(Shp);
					Shp->addParent(Cshape);
					Shape* CloudShape = ((Shape*)MAINDllOBJECT->getShapesList().getList()[DeviationPt->PtID]);
					CloudShape->addChild(Shp);
					Shp->addParent(CloudShape);
					//create measurement............
					DimBase* dim = NULL;
					dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
					dim->addParent(Shp);
					Shp->addMChild(dim);
					dim->IsValid(true);
					dim->IsNegativeMeasurement(negativeMeasurement);
					dim->UpdateMeasurement();
					MAINDllOBJECT->getDimList().addItem((BaseItem*)dim);
					createdMeasureIdlist.push_back(dim);
				}
				delete DeviationPt;
			}
		}
		else
		{
			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
			{
				ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
				if(!CShape->Normalshape()) continue;
				if(!CShape->selected()) continue;
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				{
					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;
						double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
						Shape* CurrentSelShape = NULL;
						bool ReturnFlag = false, CutOffFlag = false;
						CurrentSelShape = DXFEXPOSEOBJECT->getNearestShape_2D(Pts, intersectPts, &ReturnFlag);
						if(abs(UpperCutoff) < 0.00001)
							CutOffFlag = true;
						else
						{
							if(UpperCutoff > RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts))
								CutOffFlag = true;
						}
						if(ReturnFlag && CutOffFlag)
						{
							if(NoOfLines >= 1000) return true;		
							bool negativeMeasurement = false;
							if(ClosedLoop)
							{
								if(DXFEXPOSEOBJECT->getNumberOfIntersectionPoints(ShpCollection, Pts) % 2 != 0)
									negativeMeasurement = true;
							}
							NoOfLines++;
							Create2DLine_WithoutAction(Pts, intersectPts);
							RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
							item--;
							ShapeWithList* Shp = (ShapeWithList*)((*item).second);
							CreatedShapeidlist.push_back(Shp);
							CurrentSelShape->addChild(Shp);
							Shp->addParent(CurrentSelShape);
							CShape->addChild(Shp);
							Shp->addParent(CShape);
							//create measurement............
							DimBase* dim = NULL;
							dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
							dim->addParent(Shp);
							Shp->addMChild(dim);
							dim->IsValid(true);
							dim->IsNegativeMeasurement(negativeMeasurement);
							dim->UpdateMeasurement();
							MAINDllOBJECT->getDimList().addItem((BaseItem*)dim);
							createdMeasureIdlist.push_back(dim);
						}					
					}
				 }
			}
		}
		MAINDllOBJECT->UpdateShapesChanged();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0013", __FILE__, __FUNCSIG__); return false;}
}

bool DXFDeviationHandler::getNearestIntersectionpts(Shape* Current_Shape, double* Pnts, double* IntersectionPt)
{
	try
	{
		if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			double intersectpoints[2] = {0}, endpts[4] = {0};
			endpts[0] = ((Line*)Current_Shape)->getPoint1()->getX(); endpts[1] = ((Line*)Current_Shape)->getPoint1()->getY(); endpts[2] = ((Line*)Current_Shape)->getPoint2()->getX(); endpts[3] = ((Line*)Current_Shape)->getPoint2()->getY();
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)Current_Shape)->Angle(), ((Line*)Current_Shape)->Intercept(), Pnts, intersectpoints);		
			if(!DXFEXPOSEOBJECT->intersectionPointOnLine(endpts[0], endpts[1], endpts[2], endpts[3], intersectpoints[0], intersectpoints[1]))					
			{					
				double dist1 = 0, dist2 = 0;
				dist1 = RMATH2DOBJECT->Pt2Pt_distance(Pnts[0], Pnts[1], endpts[0], endpts[1]);
				dist2 = RMATH2DOBJECT->Pt2Pt_distance(Pnts[0], Pnts[1], endpts[2], endpts[3]);				
				if(dist1 < dist2)
				{
					intersectpoints[0] = endpts[0]; intersectpoints[1] = endpts[1];
				}
				else
				{
					intersectpoints[0] = endpts[2]; intersectpoints[1] = endpts[3];
				}
			}				
			IntersectionPt[0] = intersectpoints[0];
			IntersectionPt[1] = intersectpoints[1];
			IntersectionPt[2] = Pnts[2];
		}
		else if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::ARC || Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		{
			double center[2] = {((Circle*)Current_Shape)->getCenter()->getX(), ((Circle*)Current_Shape)->getCenter()->getY()};
			double slope, intercept, intersectpoints[2], endpt1[2] = {((Circle*)Current_Shape)->getendpoint1()->getX(), ((Circle*)Current_Shape)->getendpoint1()->getY()}, endpt2[2] = {((Circle*)Current_Shape)->getendpoint2()->getX(), ((Circle*)Current_Shape)->getendpoint2()->getY()};	
			RMATH2DOBJECT->TwoPointLine(center, Pnts, &slope, &intercept);
			if(DXFEXPOSEOBJECT->calculateIntersectionPtOnCircle(Pnts, Current_Shape, slope, intercept, &intersectpoints[0]))
			{	
				IntersectionPt[0] = intersectpoints[0];
				IntersectionPt[1] = intersectpoints[1];
				IntersectionPt[2] = Pnts[2];
			}
			else
				return false;
		}
		else
			return false;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0014", __FILE__, __FUNCSIG__); return false;}
}

void DXFDeviationHandler::findAveragePoint(double* points, double radius, std::map<int, SinglePoint*>* ArrangedPtsList, SinglePoint* DeviationPt)
{
	try
	{
		SinglePoint* tempPt = new SinglePoint();
		int count = 0;
		PointCollection ptColl;
		for(std::map<int, SinglePoint*>::iterator itr = ArrangedPtsList->begin(); itr != ArrangedPtsList->end(); itr++)
		{
			SinglePoint* Spt = (*itr).second;
			double temporaryPt[3] = {Spt->X, Spt->Y, Spt->Z};
			if(abs(points[0] - temporaryPt[0]) <= radius && abs(points[1] - temporaryPt[1]) <= radius)
			{
				tempPt->X += temporaryPt[0]; tempPt->Y += temporaryPt[1]; tempPt->Z += temporaryPt[2];
				count++;
			}				
		}
		tempPt->X = tempPt->X / count; tempPt->Y = tempPt->Y / count; tempPt->Z = tempPt->Z / count;
		bool firsttime = true;
		double dist = 0;
		for(std::map<int, SinglePoint*>::iterator itr = ArrangedPtsList->begin(); itr != ArrangedPtsList->end(); itr++)
		{
			if(firsttime)
			{
				firsttime = false;
				dist = RMATH2DOBJECT->Pt2Pt_distance((*itr).second->X, (*itr).second->Y, tempPt->X, tempPt->Y);
				DeviationPt->X = (*itr).second->X; DeviationPt->Y = (*itr).second->Y; DeviationPt->Z = (*itr).second->Z; DeviationPt->PtID = (*itr).second->PtID;
			}
			else
			{
				double dist1 = RMATH2DOBJECT->Pt2Pt_distance((*itr).second->X, (*itr).second->Y, tempPt->X, tempPt->Y);
				if(dist1 < dist)
				{
					dist = dist1;
					DeviationPt->X = (*itr).second->X; DeviationPt->Y = (*itr).second->Y; DeviationPt->Z = (*itr).second->Z; DeviationPt->PtID = (*itr).second->PtID;
				}
			}
		}
		delete tempPt;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0015", __FILE__, __FUNCSIG__); }
}

void DXFDeviationHandler::Create2DLine_WithoutAction(double *Point1, double *Point2)
{
	try
	{
		Line* myLine  = new Line();
		myLine->ShapeType = RapidEnums::SHAPETYPE::LINE;
		myLine->LineType = RapidEnums::LINETYPE::FINITELINE;
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(Point1[0], Point1[1], Point1[2]));
		PtColl.Addpoint(new SinglePoint(Point2[0], Point2[1], Point2[2]));
		myLine->AddPoints(&PtColl);		
		myLine->Normalshape(true);	
		myLine->IsValid(true);	
		MAINDllOBJECT->getShapesList().addItem((BaseItem*)myLine, false);
		//AddShapeAction::addShape(myLine, selected);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0016", __FILE__, __FUNCSIG__); }
}

void DXFDeviationHandler::UpdateDXFDeviationParam(double UpperCutoff, std::list<int> LineIdlist, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation)
{
	try
	{
		std::list<int> ShapeIdList;
		RCollectionBase& ShpCollection = MAINDllOBJECT->getSelectedShapesList();
		if(interval != 0)
		{
			double PreviousPtX[2] = {0};
			bool firstintPt = true;
			for(std::map<int, SinglePoint*>::iterator itr = SptCollection->SortedPtsList.begin(); itr != SptCollection->SortedPtsList.end(); itr++)
			{
				if(LineIdlist.size() < 1) break;
				double Pnts[3] = {(*itr).second->X, (*itr).second->Y, (*itr).second->Z}, intersectPts[3] = {0};
				if(!firstintPt)
				{
					if(IntervalType == 0)
					{
						if(!(abs(PreviousPtX[0] - Pnts[0]) >= interval))
							continue;
					}
					else if(IntervalType == 1)
					{
						if(!(abs(PreviousPtX[1] - Pnts[1]) >= interval))
							continue;
					}
					else if(IntervalType == 2)
					{
						if(!(RMATH2DOBJECT->Pt2Pt_distance(PreviousPtX, Pnts) >= interval))
							continue;
					}
				}		
				SinglePoint* DeviationPt = new SinglePoint();
				findAveragePoint(Pnts, interval / 2, &SptCollection->SortedPtsList, DeviationPt);
				double AvgPnt[3] = {DeviationPt->X, DeviationPt->Y, DeviationPt->Z};
				Shape* Cshape = NULL;
				bool ReturnFlag = false, CutOffFlag = false;
				Cshape = DXFEXPOSEOBJECT->getNearestShape_2D(AvgPnt, intersectPts, &ReturnFlag);				
				if(abs(UpperCutoff) < 0.00001)
					CutOffFlag = true;
				else
				{
					if(UpperCutoff > RMATH2DOBJECT->Pt2Pt_distance(AvgPnt, intersectPts))
						CutOffFlag = true;
				}
				if(ReturnFlag && CutOffFlag)
				{
					firstintPt = false;
					if(IntervalType == 0)							
						PreviousPtX[0] = Pnts[0];
					else if(IntervalType == 1)	
						PreviousPtX[1] = Pnts[1];
					else if(IntervalType == 2)
					{
						PreviousPtX[0] = Pnts[0]; PreviousPtX[1] = Pnts[1];
					}				
					bool negativeMeasurement = false;
					if(ClosedLoop)
					{
						if(DXFEXPOSEOBJECT->getNumberOfIntersectionPoints(ShpCollection, AvgPnt) % 2 != 0)
							negativeMeasurement = true;
					}
					
					std::list<int>::iterator itr = LineIdlist.begin();
					int LineId = *itr;
					LineIdlist.remove(LineId);
					UpdatedPts PtsStrucure;
					PtsStrucure.EndPts[0] = AvgPnt[0]; PtsStrucure.EndPts[1] = AvgPnt[1]; PtsStrucure.EndPts[2] = AvgPnt[2]; PtsStrucure.EndPts[3] = intersectPts[0]; PtsStrucure.EndPts[4] = intersectPts[1]; PtsStrucure.EndPts[5] = intersectPts[2];
					PtsStrucure.negativeMeasurement = negativeMeasurement;
					UpdatedPtsList[LineId] = PtsStrucure;
					
				}
				delete DeviationPt;
			}	
		}
		else
		{
			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
			{
				ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
				if(!CShape->Normalshape()) continue;
				if(!CShape->selected()) continue;
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				{
					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;
						double intersectPts[3] = {0}, Pnts[3] = {Spt->X, Spt->Y, Spt->Z};
						Shape* CurrentSelShape = NULL;
						bool ReturnFlag = false, CutOffFlag = false;
						CurrentSelShape = DXFEXPOSEOBJECT->getNearestShape_2D(Pnts, intersectPts, &ReturnFlag);
						if(abs(UpperCutoff) < 0.00001)
							CutOffFlag = true;
						else
						{
							if(UpperCutoff > RMATH2DOBJECT->Pt2Pt_distance(Pnts, intersectPts))
								CutOffFlag = true;
						}
						if(ReturnFlag && CutOffFlag)
						{	
							bool negativeMeasurement = false;
							if(ClosedLoop)
							{
								if(DXFEXPOSEOBJECT->getNumberOfIntersectionPoints(ShpCollection, Pnts) % 2 != 0)
									negativeMeasurement = true;
							}
							std::list<int>::iterator itr = LineIdlist.begin();
							int LineId = *itr;
							LineIdlist.remove(LineId);
							UpdatedPts PtsStrucure;
							PtsStrucure.EndPts[0] = Pnts[0]; PtsStrucure.EndPts[1] = Pnts[1]; PtsStrucure.EndPts[2] = Pnts[2]; PtsStrucure.EndPts[3] = intersectPts[0]; PtsStrucure.EndPts[4] = intersectPts[1]; PtsStrucure.EndPts[5] = intersectPts[2];
							PtsStrucure.negativeMeasurement = negativeMeasurement;
							UpdatedPtsList[LineId] = PtsStrucure;	
						}					
					}
				 }
			}
		}
		//update all lines
		for(std::map<int, UpdatedPts>::iterator mapItr = UpdatedPtsList.begin(); mapItr != UpdatedPtsList.end(); mapItr++)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0017", __FILE__, __FUNCSIG__); }
}

void DXFDeviationHandler::UpdateMeasurementAfterProgramRun()
{
	try
	{
		for(RC_ITER pCaction = PPCALCOBJECT->getPPArrangedActionlist().getList().begin(); pCaction != PPCALCOBJECT->getPPArrangedActionlist().getList().end(); pCaction++)
		{
			if(((RAction*)(*pCaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION)
			{
				std::list<int> SelectedShapeIdlist;
				std::list<int> LineIdlist;
				double tolerance = 0, interval = 0, UpperCutoff = 0;
				int IntervalType = 0;
				bool ClosedLoop = false, TwoD_Deviation = false;
				((DXF_DeviationAction*)(*pCaction).second)->GetDXF_DeviationParam(&SelectedShapeIdlist, &LineIdlist, &UpperCutoff, &tolerance, &interval, &IntervalType, &ClosedLoop, &TwoD_Deviation);
				MAINDllOBJECT->ClearShapeSelections();
				MAINDllOBJECT->selectShape(&SelectedShapeIdlist);
				if(interval != 0)
				{
					CreateSortedPts_ShapeIdRelation(TwoD_Deviation);
					CreateSortedPtsList(TwoD_Deviation, interval);
				}
				UpdateDXFDeviationParam(UpperCutoff, LineIdlist, tolerance, interval, IntervalType, ClosedLoop, TwoD_Deviation);
				MAINDllOBJECT->ClearShapeSelections();
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevH0016", __FILE__, __FUNCSIG__); }
}