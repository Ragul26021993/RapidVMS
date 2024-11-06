#include "StdAfx.h"
#include "DeleteMeasureAction.h"
#include "DeleteShapesAction.h"
#include "..\Handlers_Measurement\DimensionHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Measurement\MeasurementHeaders.h"
#include "..\Engine\IsectPt.h"
#include "..\Engine\FocusFunctions.h"
#include "AddPointAction.h"

DeleteMeasureAction::DeleteMeasureAction():RAction((TCHAR*)"DelMeasures")
{
	try
	{
		this->CurrentActionType = RapidEnums::ACTIONTYPE::DELETEMEASUREACTION;
		delmeasure = new RCollectionBase(false); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDM0001", __FILE__, __FUNCSIG__); }
}

DeleteMeasureAction::~DeleteMeasureAction()
{
	try
	{
		delmeasure->deleteAll();
		delete delmeasure;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDM0002", __FILE__, __FUNCSIG__); }
}

bool DeleteMeasureAction::execute()
{
	try
	{
		SelectParentMeasurements(MAINDllOBJECT->getSelectedDimList());
		RCollectionBase& selmeasure = MAINDllOBJECT->getSelectedDimList();
		RCollectionBase& rmeasure= MAINDllOBJECT->getDimList();
		while(selmeasure.count() != 0)
		{
			RC_ITER i = selmeasure.getList().begin();
			DimBase *Cdim = (DimBase*)((*i).second);
			for(list<BaseItem*>::iterator shapeiterator = Cdim->getParents().begin();shapeiterator != Cdim->getParents().end(); shapeiterator++)
			{
				((Shape*)(*shapeiterator))->getMchild().remove(Cdim);
				if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DIGITALMICROMETER)
				{
					for each(RAction* Caction in  ((ShapeWithList*)(*shapeiterator))->PointAtionList)
						((AddPointAction*)Caction)->ActionStatus(false);
					MAINDllOBJECT->getShapesList().removeItem((BaseItem*)(*shapeiterator));
				}
				else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				{
					if(((Shape*)(*shapeiterator))->getMchild().size() == 0)
					{
						for each(RAction* Caction in  ((ShapeWithList*)(*shapeiterator))->PointAtionList)
							((AddPointAction*)Caction)->ActionStatus(false);
						MAINDllOBJECT->getShapesList().removeItem((BaseItem*)(*shapeiterator));
						FOCUSCALCOBJECT->DepthDatumList.remove((*shapeiterator)->getId());
					}
				}
			}
			if(Cdim->MeasureAsGDnTMeasurement())
			{
 				int rfgd = Cdim->ParentShape2->RefernceDatumForgdnt();
				if(rfgd != 0)rfgd--;
				Cdim->ParentShape2->RefernceDatumForgdnt(rfgd);
				if(Cdim->ParentShape3 != NULL)
				{
					rfgd = Cdim->ParentShape3->RefernceDatumForgdnt();
					if(rfgd != 0)rfgd--;
					Cdim->ParentShape3->RefernceDatumForgdnt(rfgd);
				}
			}
			delmeasure->addItem(Cdim, false);
			for(RC_ITER It = delmeasure->getList().begin(); It != delmeasure->getList().end(); It++)
			{
				DimBase *cdim = (DimBase*)(It->second);
				if(cdim->CurrentDimAddAction != NULL) 
					MAINDllOBJECT->getActionsHistoryList().notifyRemove(cdim->CurrentDimAddAction);
			}
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				FOCUSCALCOBJECT->DepthMeasureList.remove(Cdim->getId());
			rmeasure.removeItem(Cdim);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDM0003", __FILE__, __FUNCSIG__); return false;}
}

bool DeleteMeasureAction::redo()
{
	try
	{
		SelectParentMeasurements(MAINDllOBJECT->getSelectedDimList());
		RCollectionBase& selmeasure = MAINDllOBJECT->getSelectedDimList();
		RCollectionBase& rmeasure = MAINDllOBJECT->getDimList();
		while (selmeasure.count() != 0)
		{
			RC_ITER i = selmeasure.getList().begin();
			DimBase *Cdim = (DimBase*)((*i).second);
			for(list<BaseItem*>::iterator shapeiterator = Cdim->getParents().begin();shapeiterator != Cdim->getParents().end(); shapeiterator++)
			{
				((Shape*)(*shapeiterator))->getMchild().remove(Cdim);
				if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DIGITALMICROMETER)
				{
				    for each(RAction* Caction in  ((ShapeWithList*)(*shapeiterator))->PointAtionList)
						((AddPointAction*)Caction)->ActionStatus(false);
					MAINDllOBJECT->getShapesList().removeItem((BaseItem*)(*shapeiterator));
				}
				else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				{
					if(((Shape*)(*shapeiterator))->getMchild().size() == 0)
					{
						for each(RAction* Caction in  ((ShapeWithList*)(*shapeiterator))->PointAtionList)
							((AddPointAction*)Caction)->ActionStatus(false);
						MAINDllOBJECT->getShapesList().removeItem((BaseItem*)(*shapeiterator));
						FOCUSCALCOBJECT->DepthDatumList.remove((*shapeiterator)->getId());
					}
				}
			}
			if(Cdim->MeasureAsGDnTMeasurement())
			{
				int rfgd = Cdim->ParentShape2->RefernceDatumForgdnt();
				if(rfgd != 0)rfgd--;
				Cdim->ParentShape2->RefernceDatumForgdnt(rfgd);
				if(Cdim->ParentShape3 != NULL)
				{
					rfgd = Cdim->ParentShape3->RefernceDatumForgdnt();
					if(rfgd != 0)rfgd--;
					Cdim->ParentShape3->RefernceDatumForgdnt(rfgd);
				}
			}
			delmeasure->addItem(Cdim, false);
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				FOCUSCALCOBJECT->DepthMeasureList.remove(Cdim->getId());
			rmeasure.removeItem(Cdim);
		}
		MAINDllOBJECT->Measurement_updated();
		for(RC_ITER It = delmeasure->getList().begin(); It != delmeasure->getList().end(); It++)
		{
			DimBase *cdim = (DimBase*)(It->second);
			if(cdim->CurrentDimAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyRemove(cdim->CurrentDimAddAction);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDM0004", __FILE__, __FUNCSIG__); return false; }
}

void DeleteMeasureAction::undo()
{
	try
	{
		RCollectionBase& rmeasure = MAINDllOBJECT->getDimList();
		for (RC_ITER i = delmeasure->getList().begin(); i!= delmeasure->getList().end(); i++)
		{
			DimBase *Cdim = (DimBase*)((*i).second);
			for(list<BaseItem*>::iterator shapeiterator = Cdim->getParents().begin();shapeiterator != Cdim->getParents().end(); shapeiterator++)
			{
				((Shape*)(*shapeiterator))->addMChild(Cdim);
				if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DIGITALMICROMETER)
				{
					for each(RAction* Caction in  ((ShapeWithList*)(*shapeiterator))->PointAtionList)
						((AddPointAction*)Caction)->ActionStatus(true);
					MAINDllOBJECT->getShapesList().addItem((BaseItem*)(*shapeiterator));
				}
				else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				{
					if(((Shape*)(*shapeiterator))->getMchild().size() == 1)
					{
						for each(RAction* Caction in  ((ShapeWithList*)(*shapeiterator))->PointAtionList)
						((AddPointAction*)Caction)->ActionStatus(true);
						MAINDllOBJECT->getShapesList().addItem((BaseItem*)(*shapeiterator));
						FOCUSCALCOBJECT->DepthDatumList.push_back((*shapeiterator)->getId());
					}
				}
			}
			if(Cdim->MeasureAsGDnTMeasurement())
			{
				int rfgd = Cdim->ParentShape2->RefernceDatumForgdnt();
				rfgd++;
				Cdim->ParentShape2->RefernceDatumForgdnt(rfgd);
				if(Cdim->ParentShape3 != NULL)
				{
					rfgd = Cdim->ParentShape3->RefernceDatumForgdnt();
					rfgd++;
					Cdim->ParentShape3->RefernceDatumForgdnt(rfgd);
				}
			}
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				FOCUSCALCOBJECT->DepthMeasureList.push_back(Cdim->getId());
			rmeasure.addItem(Cdim);	
			Cdim->selected(false, false);
			switch(Cdim->MeasurementType)
			{
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D:
					if(!Cdim->ChildMeasurementType())
					{
						Cdim->ParentPoint1 = MAINDllOBJECT->getVectorPointer(Cdim->ParentPoint1);
						Cdim->ParentPoint2 = MAINDllOBJECT->getVectorPointer(Cdim->ParentPoint2);
						if(Cdim->ParentPoint1->IntersectionPoint)
							((IsectPt*)Cdim->ParentPoint1)->SetRelatedDim(Cdim);
						if(Cdim->ParentPoint2->IntersectionPoint)
							((IsectPt*)Cdim->ParentPoint2)->SetRelatedDim(Cdim);
					}
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONICS3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
					if(!Cdim->ChildMeasurementType())
					{
						Cdim->ParentPoint1 = MAINDllOBJECT->getVectorPointer(Cdim->ParentPoint1);
						if(Cdim->ParentPoint1->IntersectionPoint)
							((IsectPt*)Cdim->ParentPoint1)->SetRelatedDim(Cdim);
					}
					break;
			}
		}
		for(RC_ITER It = delmeasure->getList().begin(); It != delmeasure->getList().end(); It++)
		{
			DimBase *cdim = (DimBase*)(It->second);
			if(cdim->CurrentDimAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyAdd(cdim->CurrentDimAddAction);
		}
		delmeasure->clear(false);
		MAINDllOBJECT->Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDM0005", __FILE__, __FUNCSIG__); }
}

void DeleteMeasureAction::SelectParentMeasurements(RCollectionBase &Measures)
{
	try
	{
		list<DimBase*> TempMeasureList;
		for(RC_ITER i = Measures.getList().begin(); i!= Measures.getList().end(); i++)
		{
			DimBase* Cdim = (DimBase*)((*i).second);
			TempMeasureList.push_back(Cdim);			
		}
		MAINDllOBJECT->getDimList().multiselect(true);
		for each(DimBase* Cdim in TempMeasureList)
		{
			if(Cdim->ChildMeasurementType())
			{
				DimChildMeasurement* CdimChld = (DimChildMeasurement*)Cdim;
				if(MAINDllOBJECT->getDimList().ItemExists(CdimChld->ParentMeasureId()))
				{
					DimBase* CdimCur = (DimBase*)MAINDllOBJECT->getDimList().getList()[CdimChld->ParentMeasureId()];
					if(CdimCur->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
					{
						DimPointCoordinate* CdimPt = (DimPointCoordinate*)CdimCur;
						CdimPt->selected(true);
						for each(int Mid in CdimPt->PChildMeasureIdCollection)
						{
							if(MAINDllOBJECT->getDimList().ItemExists(Mid))
				            {  
								DimBase* dimCur = (DimBase*)MAINDllOBJECT->getDimList().getList()[Mid];
								dimCur->selected(true);
							}
						}
					}
					else if(CdimCur->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					{
						DimThread* CdimThrd = (DimThread*)CdimCur;
						CdimThrd->selected(true);
						for each(int Mid in CdimThrd->TChildMeasureIdCollection)
						{
							if(MAINDllOBJECT->getDimList().ItemExists(Mid))
				            { 
								DimBase* dimCur = (DimBase*)MAINDllOBJECT->getDimList().getList()[Mid];
								dimCur->selected(true);
							}
						}
					}
				}
			}
			else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
			{
				DimPointCoordinate* CdimPt = (DimPointCoordinate*)Cdim;
				CdimPt->selected(true);
				for each(int Mid in CdimPt->PChildMeasureIdCollection)
				{
					if(MAINDllOBJECT->getDimList().ItemExists(Mid))
				    { 
						DimBase* dimCur = (DimBase*)MAINDllOBJECT->getDimList().getList()[Mid];
						dimCur->selected(true);
					}
				}
			}
		}
		MAINDllOBJECT->getDimList().multiselect(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDM0005", __FILE__, __FUNCSIG__); }
}