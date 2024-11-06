#include "stdafx.h"
#include "DeviationHandler.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\DeviationAction.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
DeviationHandler::DeviationHandler()
{
	try
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEVIATION_HANDLER;
		runningPartprogramValid = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
DeviationHandler::~DeviationHandler()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0002", __FILE__, __FUNCSIG__); }
}

//Mousemove....//
void DeviationHandler::mouseMove()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0003", __FILE__, __FUNCSIG__); }
}

//Handle the left mousedown....//
void DeviationHandler::LmouseDown()
{
	try{}
	catch(...){  MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0004", __FILE__, __FUNCSIG__); }
}

//Draw the parallel arc..//
void DeviationHandler::draw(int windowno, double WPixelWidth)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0005", __FILE__, __FUNCSIG__); }
}

void DeviationHandler::EscapebuttonPress()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0006", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data.....//
void DeviationHandler::PartProgramData()
{
	try
	{
		UpdateDeviationParam();
		PPCALCOBJECT->partProgramReached();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0007", __FILE__, __FUNCSIG__); }
}

//Set the  parallel arc parameters...
void DeviationHandler::LmaxmouseDown()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0008", __FILE__, __FUNCSIG__); }
}

void DeviationHandler::SetAnyData(double *data)
{
	try
	{
		//data[3]->deviation with yellow shapes only........
		//data[4]->interval type..........
		std::list<int> Idlist;
		std::list<int> LineId;
		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			Idlist.push_back(CShape->getId());
		}		
		bool addaction = DXFEXPOSEOBJECT->ShowDeviationwithCloudPts(data[0], data[1], data[2], &LineId, data[4], bool(data[5]), bool(data[3]), int(data[6]), bool(data[7]));
		if(addaction)
		{
			Vector parameter;
			parameter.set(data[0], data[1], data[2], data[3], data[4]);
			DeviationAction::SetDeviationParam(parameter, &Idlist, &LineId, bool(data[5]), int(data[6]), bool(data[7]));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0009", __FILE__, __FUNCSIG__); }
}

void DeviationHandler::UpdateDeviationParam()
{
	try
	{
		//data[3]->deviation with yellow shapes only........
		//data[4]->interval type..........right now i am not using this param b/c it is specific to sheasha tools..
		runningPartprogramValid = true;
		std::list<int> Idlist;
		map<int, int> LinIdlist;
		int measureRefrenceAxis = 0;
		bool BestfitArcflag = true;
		Vector Dparameter;
		Dparameter = *((DeviationAction*)PPCALCOBJECT->getCurrentAction())->GetDeviationParam();
		((DeviationAction*)PPCALCOBJECT->getCurrentAction())->getShapeIdList(&Idlist, &BestfitArcflag, &measureRefrenceAxis);
		((DeviationAction*)PPCALCOBJECT->getCurrentAction())->getLineIdList(&LinIdlist);
		bool DeviationType_2D = ((DeviationAction*)PPCALCOBJECT->getCurrentAction())->getdeviationTypeFlag();
		MAINDllOBJECT->ClearShapeSelections();
		MAINDllOBJECT->selectShape(&Idlist);
		double data[5] = {Dparameter.getX(), Dparameter.getY(), Dparameter.getZ(), Dparameter.getR(), Dparameter.getP()};

		if(DeviationType_2D)
		{
			if(data[3] == 0)
			{
				int MapId = 0;
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(CShape == NULL)  return;
					if(!CShape->selected()) continue;			
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						for(RC_ITER pCaction = PPCALCOBJECT->getPPActionlist().getList().begin(); pCaction != PPCALCOBJECT->getPPActionlist().getList().end(); pCaction++)
						{
							if(((RAction*)(*pCaction).second)->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) continue;
							if(((AddPointAction*)(*pCaction).second)->getShape()->getId() != CShape->getId()) continue;
													
							FramegrabBase* Cfgb = ((AddPointAction*)(*pCaction).second)->getFramegrab();
							list<int> tmpList = Cfgb->PointActionIdList;
							if(tmpList.size() < 3) continue;
							double *points = NULL;
							points = new double[tmpList.size() * 2];
							int n = 0;
							double avgZ = 0, ans[3] = {0}, angles[2] = {0}, currentY = Cfgb->PointDRO.getY() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), currentZ = 0;
							for(list<int>::iterator It = tmpList.begin(); It != tmpList.end(); It++)
							{
								SinglePoint* Pt = CShape->PointsList->getList()[*It];
								points[n++] = Pt->X;
								points[n++] = Pt->Y;
								avgZ += Pt->Y;
							}
							avgZ /= tmpList.size();
							if(BestfitArcflag)
							{
								BESTFITOBJECT->Circle_BestFit(points, tmpList.size(), &ans[0], MAINDllOBJECT->SigmaModeFlag(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());					
				        		if(abs(ans[2]) < abs(currentY - ans[0]))
									currentZ = avgZ;
								else
								{
									double tmpz1 = sqrt(pow(ans[2], 2) - pow(currentY - ans[0], 2)) + ans[1];
									double tmpz2 = -sqrt(pow(ans[2], 2) - pow(currentY - ans[0], 2)) + ans[1];
									if(abs(tmpz1 - avgZ) < abs(tmpz2 - avgZ))
										currentZ = tmpz1;
									else
										currentZ = tmpz2;
									if(abs(currentZ - avgZ) > 1) currentZ = avgZ;
								}
							}
							else
							{
								BESTFITOBJECT->Line_BestFit_2D(points, tmpList.size(), &ans[0], MAINDllOBJECT->SigmaModeFlag(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());	
								currentZ = tan(ans[0]) * currentY + ans[1];
								if(abs(currentZ - avgZ) > 1) currentZ = avgZ;
							}
							double intersectPts[3] = {0}, Pts[3] = {currentY, currentZ, Cfgb->PointDRO.getX() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX()};
							if(points != NULL)
							{
								delete [] points;
								points = NULL;
							}
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = DXFEXPOSEOBJECT->getNearestShape_3D(Pts, intersectPts, &ReturnFlag);
							intersectPts[2] = Pts[2];
							if(ReturnFlag)
							{
								if(LinIdlist.size() <= MapId) return;
								int LnID = LinIdlist[MapId];
								Shape* CShape = (Shape*)(MAINDllOBJECT->getShapesList().getList()[LnID]);
								double tolerancelimit = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
								bool positiveFlag = false;
								bool withintolerance = false;
								if(measureRefrenceAxis == 0)
								{
									if(Pts[0] > intersectPts[0])
									{
										positiveFlag = true;
										if(data[0] >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(data[1]) >= tolerancelimit)
											withintolerance = true;
									}
								}
								else if(measureRefrenceAxis == 1)
								{
									if(Pts[1] > intersectPts[1])
									{
										positiveFlag = true;
										if(data[0] >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(data[1]) >= tolerancelimit)
											withintolerance = true;
									}
								}		
								if(withintolerance)
									CShape->ShapeColor.set(1, 1, 1);
								else
									CShape->ShapeColor.set(1, 0, 0);
								if(positiveFlag)
								{
									if(CShape->getMchild().size() > 0)
									{
										list<BaseItem*>::iterator childDimIter = CShape->getMchild().begin();
										DimBase* cdim = (DimBase*)(*childDimIter);
										cdim->IsNegativeMeasurement(false);
									}
								}
								else
								{
									if(CShape->getMchild().size() > 0)
									{
										list<BaseItem*>::iterator childDimIter = CShape->getMchild().begin();
										DimBase* cdim = (DimBase*)(*childDimIter);
										cdim->IsNegativeMeasurement(true);
									}
								}
								((Line*)CShape)->setPoint1(Vector(Pts[0], Pts[1], Pts[2]));
								((Line*)CShape)->setPoint2(Vector(intersectPts[0], intersectPts[1], intersectPts[2]));
								((Line*)CShape)->calculateAttributes();
								MapId++;
							}
						}
					}
				}
			}
			else if(data[3] == 1)
			{
				int MapId = 0;
				bool firstTime = true;
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(CShape == NULL)  return;
					if(!CShape->selected()) continue;	
					if(CShape->IsDxfShape()) continue;
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
					{
						double totDist = 0;
						int ptsCount = 0;
						for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							double dist = RMATH2DOBJECT->Pt2Line_Dist(Spt->X, Spt->Y, ((Line*)CShape)->Angle(), ((Line*)CShape)->Intercept());
							totDist += dist;
							ptsCount++;
						}
						double Straightness = totDist / ptsCount;
						if(data[0] < Straightness)
						{
							MAINDllOBJECT->ShowMsgBoxString("Deviation001", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							PPCALCOBJECT->AbortPartProgram();
							return;
						}
						if(Straightness * 2 < abs(data[1]))
							Straightness *= 2;
						std::list<int> ShapeIdList;
						map<int, double> DistIdMap;
						map<int, SinglePoint*> intersectPtMapList;
	                    for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							if(Straightness < RMATH2DOBJECT->Pt2Line_Dist(Spt->X, Spt->Y, ((Line*)CShape)->Angle(), ((Line*)CShape)->Intercept())) continue;
							double intersectPts[3] = {0, 0, Spt->Z}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = DXFEXPOSEOBJECT->getNearestShape_2D(Pts, intersectPts, &ReturnFlag, false);						
							if(ReturnFlag)
							{
								//check deviation for yellow lines only.......
								if(CurrentSelShape->ShapeColor.r != 1 || CurrentSelShape->ShapeColor.g != 1 || CurrentSelShape->ShapeColor.b != 0) continue;
								bool IdPresent = false;
								for each(int id in ShapeIdList)
								{
									if(CurrentSelShape->getId() == id)
									{
										IdPresent = true;
										break;
									}
								}
								double currentDist = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);								
								if(IdPresent)
								{							
									if(currentDist > DistIdMap[CurrentSelShape->getId()])
									{
										DistIdMap[CurrentSelShape->getId()] = currentDist;
										intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], Pts[0],  Pts[1], Pts[2]);
									
									}
								}
								else
								{
									ShapeIdList.push_back(CurrentSelShape->getId());
									DistIdMap[CurrentSelShape->getId()] = currentDist;
									intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], Pts[0],  Pts[1], Pts[2]);
								
								}
							}
						}
						for each(int id in ShapeIdList)
						{
							if(LinIdlist.size() <= MapId) return;
							int LnID = LinIdlist[MapId];
							Shape* Lineshape = NULL;
							Shape* ParentShape = (Shape*)(MAINDllOBJECT->getShapesList().getList()[id]);
							if(CShape->getChild().size() > 0)
							{
								list<BaseItem*>::iterator ShpItr = CShape->getChild().begin();
								Shape* Shp = (Shape*)(*ShpItr);
								for(list<BaseItem*>::iterator childShpIter = ParentShape->getChild().begin(); childShpIter != ParentShape->getChild().end(); childShpIter++)
								{
									Lineshape = (Shape*)(*childShpIter);
									if(Shp->getId() == Lineshape->getId())
										break;
								}														
							}
							if(Lineshape == NULL) break;
							SinglePoint* Spt = intersectPtMapList[id];
							double intersPt[3] = {Spt->X, Spt->Y, Spt->Pdir}, CPts[3] = {Spt->Z, Spt->R, Spt->Pdir};
							if(firstTime)
							{
								firstTime = false;
								Vector Shift_XY;
								Shift_XY.set(Spt->Z - Spt->X, Spt->R - Spt->Y);
								DXFEXPOSEOBJECT->SetTransVector(Shift_XY, false);
								intersPt[0] = CPts[0]; intersPt[1] = CPts[1];
							}	
							double tolerancelimit = RMATH2DOBJECT->Pt2Pt_distance(CPts, intersPt);
							bool positiveFlag = false;
							bool withintolerance = false;
							if(measureRefrenceAxis == 0)
							{
								if(CPts[0] <= intersPt[0])
								{
									positiveFlag = true;
									if(data[0] >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(data[1]) >= tolerancelimit)
										withintolerance = true;
								}
							}
							else if(measureRefrenceAxis == 1)
							{
								if(CPts[1] > intersPt[1])
								{
									positiveFlag = true;
									if(data[0] >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(data[1]) >= tolerancelimit)
										withintolerance = true;
								}
							}		
							if(withintolerance)
								Lineshape->ShapeColor.set(1, 1, 1);
							else
								Lineshape->ShapeColor.set(1, 0, 0);
							if(positiveFlag)
							{
								if(Lineshape->getMchild().size() > 0)
								{
									list<BaseItem*>::iterator childDimIter = Lineshape->getMchild().begin();
									DimBase* cdim = (DimBase*)(*childDimIter);
									cdim->IsNegativeMeasurement(false);
								}
							}
							else
							{
								if(Lineshape->getMchild().size() > 0)
								{
									list<BaseItem*>::iterator childDimIter = Lineshape->getMchild().begin();
									DimBase* cdim = (DimBase*)(*childDimIter);
									cdim->IsNegativeMeasurement(true);
								}
							}
							((Line*)Lineshape)->setPoint1(Vector(intersPt[0], intersPt[1], intersPt[2]));
							((Line*)Lineshape)->setPoint2(Vector(CPts[0], CPts[1], CPts[2]));
							((Line*)Lineshape)->calculateAttributes();
							MapId++;
						}
					}
				}
			}
		}
		else
		{
			if(data[3] == 0)
			{
				int MapId = 0;
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(CShape == NULL)  return;
					if(!CShape->selected()) continue;			
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = DXFEXPOSEOBJECT->getNearestShape_3D(Pts, intersectPts, &ReturnFlag);
							if(ReturnFlag)
							{
								if(LinIdlist.size() <= MapId) return;
								int LnID = LinIdlist[MapId];
								Shape* CShape = (Shape*)(MAINDllOBJECT->getShapesList().getList()[LnID]);
								double tolerancelimit = RMATH3DOBJECT->Distance_Point_Point(Pts, intersectPts);
								bool positiveFlag = false;
								bool withintolerance = false;
								if(measureRefrenceAxis == 0)
								{
									if(Pts[0] > intersectPts[0])
									{
										positiveFlag = true;
										if(data[0] >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(data[1]) >= tolerancelimit)
											withintolerance = true;
									}
								}
								else if(measureRefrenceAxis == 1)
								{
									if(Pts[1] > intersectPts[1])
									{
										positiveFlag = true;
										if(data[0] >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(data[1]) >= tolerancelimit)
											withintolerance = true;
									}
								}
								if(withintolerance)
									CShape->ShapeColor.set(1, 1, 1);
								else
									CShape->ShapeColor.set(1, 0, 0);
								if(positiveFlag)
								{
									if(CShape->getMchild().size() > 0)
									{
										list<BaseItem*>::iterator childDimIter = CShape->getMchild().begin();
										DimBase* cdim = (DimBase*)(*childDimIter);
										cdim->IsNegativeMeasurement(false);
									}
								}
								else
								{
									if(CShape->getMchild().size() > 0)
									{
										list<BaseItem*>::iterator childDimIter = CShape->getMchild().begin();
										DimBase* cdim = (DimBase*)(*childDimIter);
										cdim->IsNegativeMeasurement(true);
									}
								}
								((Line*)CShape)->setPoint1(Vector(Pts[0], Pts[1], Pts[2]));
								((Line*)CShape)->setPoint2(Vector(intersectPts[0], intersectPts[1], intersectPts[2]));
								((Line*)CShape)->calculateAttributes();
								MapId++;
							}
						}
					}
				}
			}
			else if(data[3] == 1)
			{
				int MapId = 0;
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(CShape == NULL)  return;
					if(!CShape->selected()) continue;			
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						std::list<int> ShapeIdList;
						map<int, double> DistIdMap;
						map<int, SinglePoint*> intersectPtMapList;
						map<int, SinglePoint*> CloudPtsList;
						for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};				
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = DXFEXPOSEOBJECT->getNearestShape_3D(Pts, intersectPts, &ReturnFlag);
							if(ReturnFlag)
							{
								//check deviation for yellow lines only.......
								if(CurrentSelShape->ShapeColor.r != 1 || CurrentSelShape->ShapeColor.g != 1 || CurrentSelShape->ShapeColor.b != 0) continue;
								bool IdPresent = false;
								for each(int id in ShapeIdList)
								{
									if(CurrentSelShape->getId() == id)
									{
										IdPresent = true;
										break;
									}
								}
								double currentDist = RMATH3DOBJECT->Distance_Point_Point(Pts, intersectPts);
								if(currentDist < 0.0001) continue;
								if(IdPresent)
								{							
									if(currentDist > DistIdMap[CurrentSelShape->getId()])
									{
										DistIdMap[CurrentSelShape->getId()] = currentDist;
										intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], intersectPts[2]);
										CloudPtsList[CurrentSelShape->getId()] = new SinglePoint(Pts[0],  Pts[1], Pts[2]);
									
									}
								}
								else
								{
									ShapeIdList.push_back(CurrentSelShape->getId());
									DistIdMap[CurrentSelShape->getId()] = currentDist;
									intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], intersectPts[2]);
									CloudPtsList[CurrentSelShape->getId()] = new SinglePoint(Pts[0],  Pts[1], Pts[2]);
								
								}
							}
						}
						for each(int id in ShapeIdList)
						{
							if(LinIdlist.size() <= MapId) return;
							int LnID = LinIdlist[MapId];
							Shape* CShape = (Shape*)(MAINDllOBJECT->getShapesList().getList()[LnID]);
							SinglePoint* Spt = intersectPtMapList[id];
							SinglePoint* CSpt = CloudPtsList[id];
							double intersPt[3] = {Spt->X, Spt->Y, Spt->Z}, CPts[3] = {CSpt->X, CSpt->Y, CSpt->Z};
							double tolerancelimit = RMATH3DOBJECT->Distance_Point_Point(CPts, intersPt);
							bool positiveFlag = false;
							bool withintolerance = false;
							if(measureRefrenceAxis == 0)
							{
								if(CPts[0] > intersPt[0])
								{
									positiveFlag = true;
									if(data[0] >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(data[1]) >= tolerancelimit)
										withintolerance = true;
								}
							}
							else if(measureRefrenceAxis == 1)
							{
								if(CPts[1] > intersPt[1])
								{
									positiveFlag = true;
									if(data[0] >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(data[1]) >= tolerancelimit)
										withintolerance = true;
								}
							}
							if(withintolerance)
								CShape->ShapeColor.set(1, 1, 1);
							else
								CShape->ShapeColor.set(1, 0, 0);
							if(positiveFlag)
							{
								if(CShape->getMchild().size() > 0)
								{
									list<BaseItem*>::iterator childDimIter = CShape->getMchild().begin();
									DimBase* cdim = (DimBase*)(*childDimIter);
									cdim->IsNegativeMeasurement(false);
								}
							}
							else
							{
								if(CShape->getMchild().size() > 0)
								{
									list<BaseItem*>::iterator childDimIter = CShape->getMchild().begin();
									DimBase* cdim = (DimBase*)(*childDimIter);
									cdim->IsNegativeMeasurement(true);
								}
							}
							((Line*)CShape)->setPoint1(Vector(intersPt[0], intersPt[1], intersPt[2]));
							((Line*)CShape)->setPoint2(Vector(CPts[0], CPts[1], CPts[2]));
							((Line*)CShape)->calculateAttributes();
							MapId++;
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("DevH0010", __FILE__, __FUNCSIG__); 
	}
}
