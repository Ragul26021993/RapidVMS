#include "StdAfx.h"
#include "DeleteShapesAction.h"
#include "AddPointAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\RelatedShape.h"
#include "../Measurement/DimBase.h"
#include "../Measurement/DimPointToLineDistance.h"
#include "../Measurement/DimPointToCircleDistance.h"
#include "../Measurement/DimPointToPointDistance.h"
#include "..\Engine\IsectPt.h"
#include "..\Engine\FocusFunctions.h"

DeleteShapesAction::DeleteShapesAction():RAction(_T("DelShapes"))
{
	try
	{
		delshapes = new RCollectionBase(false);
		delMeasurement = new RCollectionBase(false);
		this->CurrentActionType = RapidEnums::ACTIONTYPE::DELETESHAPEACTION;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0001", __FILE__, __FUNCSIG__); }
}

DeleteShapesAction::~DeleteShapesAction()
{
	try
	{
		delshapes->deleteAll();
		delete delshapes;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0002", __FILE__, __FUNCSIG__); }
}

bool DeleteShapesAction::execute()
{
	try
	{
		RCollectionBase& selshapes = MAINDllOBJECT->getSelectedShapesList();
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		while(selshapes.count() != 0)
		{
			RC_ITER i = selshapes.getList().begin();
			((Shape*)((*i).second))->IsValid(false);
			for each(BaseItem* act in ((ShapeWithList*)((*i).second))->PointAtionList)
		    	((AddPointAction*)act)->ActionStatus(false);
			Shape* Shp = (Shape*)((*i).second);
			deleteRelatedShape(Shp);
			if(Shp->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE)
				FOCUSCALCOBJECT->DepthDatumList.remove((*i).second->getId());
			delshapes->addItem((BaseItem*)Shp, false);
			rshapes.removeItem((BaseItem*)Shp);
		}
		for(RC_ITER It = delshapes->getList().begin(); It != delshapes->getList().end(); It++)
		{
			ShapeWithList *csh = (ShapeWithList*)(It->second);
			for each(BaseItem* act in csh->PointAtionList)
		        MAINDllOBJECT->getActionsHistoryList().notifyRemove(act);
			if(csh->CurrentShapeAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyRemove(csh->CurrentShapeAddAction);
		}
		for(RC_ITER It = delMeasurement->getList().begin(); It != delMeasurement->getList().end(); It++)
		{
			DimBase *cdim = (DimBase*)(It->second);
			if(cdim->CurrentDimAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyRemove(cdim->CurrentDimAddAction);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0003", __FILE__, __FUNCSIG__); return false; }
}

bool DeleteShapesAction::redo()
{
	try
	{
		RCollectionBase& selshapes = MAINDllOBJECT->getSelectedShapesList();
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		while (selshapes.count() != 0)
		{
			RC_ITER i = selshapes.getList().begin();
			ShapeWithList *CurrentShape = (ShapeWithList*)((*i).second);
			CurrentShape->IsValid(false);

			for each(BaseItem* act in CurrentShape->PointAtionList)
				((AddPointAction*)act)->ActionStatus(false);
			deleteRelatedShape((Shape*)((*i).second));
			if(CurrentShape->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE)
				FOCUSCALCOBJECT->DepthDatumList.remove(CurrentShape->getId());
			delshapes->addItem((CurrentShape), false);
			rshapes.removeItem(CurrentShape);
		}
		for(RC_ITER It = delshapes->getList().begin(); It != delshapes->getList().end(); It++)
		{
			ShapeWithList *csh = (ShapeWithList*)(It->second);
			for each(BaseItem* act in csh->PointAtionList)
		        MAINDllOBJECT->getActionsHistoryList().notifyRemove(act);
			if(csh->CurrentShapeAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyRemove(csh->CurrentShapeAddAction);
		}
		for(RC_ITER It = delMeasurement->getList().begin(); It != delMeasurement->getList().end(); It++)
		{
			DimBase *cdim = (DimBase*)(It->second);
			if(cdim->CurrentDimAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyRemove(cdim->CurrentDimAddAction);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0004", __FILE__, __FUNCSIG__); return false; }
}

void DeleteShapesAction::undo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		MAINDllOBJECT->getShapesList().clearSelection();
		for (RC_ITER i = delshapes->getList().begin(); i!=delshapes->getList().end(); i++)
		{
			ShapeWithList* Csh = (ShapeWithList*)((*i).second);
			Csh->IsValid(true);
			rshapes.addItem(Csh);
			for(list<BaseItem*>::iterator shapeiterator = Csh->getParents().begin();shapeiterator != Csh->getParents().end(); shapeiterator++)
			{
			   ((Shape*)(*shapeiterator))->addChild(Csh);
			}
			for each(BaseItem* act in Csh->PointAtionList)
				((AddPointAction*)act)->ActionStatus(true);
			if(Csh->selected())
				MAINDllOBJECT->getSelectedShapesList().addItem(Csh, false);
			for(list<BaseItem*>::iterator shapeiterator = Csh->getChild().begin();shapeiterator != Csh->getChild().end(); shapeiterator++)
			{
				ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
				if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
				{
					if(((Circle*)Cshape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
					{
						((PCDCircle*)Cshape)->AddParentShapes(Csh);
						continue;
					}
				}
			}
			if(((Shape*)((*i).second))->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE)
				FOCUSCALCOBJECT->DepthDatumList.push_back((*i).second->getId());
		}
		rshapes.notifySelection();
		for(RC_ITER It = delshapes->getList().begin(); It != delshapes->getList().end(); It++)
		{
			ShapeWithList *csh = (ShapeWithList*)(It->second);
			for each(BaseItem* act in csh->PointAtionList)
		        MAINDllOBJECT->getActionsHistoryList().notifyAdd(act);
			if(csh->CurrentShapeAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyAdd(csh->CurrentShapeAddAction);
		}
		delshapes->clear(false);
		RCollectionBase& mshapes = MAINDllOBJECT->getDimList();
		for (RC_ITER i = delMeasurement->getList().begin(); i!=delMeasurement->getList().end(); i++)
		{
			DimBase* Cdim = (DimBase*)((*i).second);
			Cdim->IsValid(true);
			mshapes.addItem(Cdim);
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
			if((*i).second->selected())
				MAINDllOBJECT->getSelectedDimList().addItem((*i).second, false);
			/*DimBase* Cdim = (DimBase*)((*i).second);
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
			{
				for(list<BaseItem*>::iterator Pshape = Cdim->getParents().begin();Pshape != Cdim->getParents().end(); Pshape++)
				{				
					if(((Shape*)(*Pshape))->getMchild().size() == 0)
					{
						for each(RAction* Caction in  ((ShapeWithList*)(*Pshape))->PointAtionList)
							((AddPointAction*)Caction)->deactivateAction();
						MAINDllOBJECT->getShapesList().addItem((BaseItem*)(*Pshape));
						MAINDllOBJECT->DepthDatumList.push_back((*Pshape)->getId());
					}
				}
			}*/
		}
		mshapes.notifySelection();
		for(RC_ITER It = delMeasurement->getList().begin(); It != delMeasurement->getList().end(); It++)
		{
			DimBase *cdim = (DimBase*)(It->second);
			if(cdim->CurrentDimAddAction != NULL) 
				MAINDllOBJECT->getActionsHistoryList().notifyAdd(cdim->CurrentDimAddAction);
		}
		delMeasurement->clear(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0005", __FILE__, __FUNCSIG__); }
}

//Deletes the related shapes.... Go through a loop to delete all the derived shapes..//
void DeleteShapesAction::deleteRelatedShape(Shape* s)
{
	try
	{
		if(s == NULL)return;
		if(s->getMchild().size() > 0)deleteRelatedMeasure(s);
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		for(list<BaseItem*>::iterator shapeiterator = s->getParents().begin();shapeiterator != s->getParents().end(); shapeiterator++)
		{
		    ((Shape*)(*shapeiterator))->RemoveChild(s);
		}
		for(list<BaseItem*>::iterator shapeiterator = s->getGroupChilds().begin();shapeiterator != s->getGroupChilds().end(); shapeiterator++)
		{
			ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
			if(rshapes.getList().find(Cshape->getId()) == rshapes.getList().end()) return;
			BaseItem* CSh = rshapes.getList().at(Cshape->getId());
			for each(BaseItem* act in Cshape->PointAtionList)
				((AddPointAction*)act)->ActionStatus(false);
			Cshape->IsValid(false);
			if(Cshape->getGroupChilds().size() > 0) 
				deleteRelatedShape(Cshape);
			if(Cshape->getChild().size() > 0) 
				deleteRelatedShape(Cshape);
			if(Cshape->getMchild().size() > 0)
				deleteRelatedMeasure(Cshape);
			delshapes->addItem(Cshape, false);
			rshapes.removeItem(Cshape);
		}
 		for(list<BaseItem*>::iterator shapeiterator = s->getChild().begin();shapeiterator != s->getChild().end(); shapeiterator++)
		{
			
			ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
		    if(rshapes.getList().find(Cshape->getId()) == rshapes.getList().end()) return;
			BaseItem* CSh = rshapes.getList().at(Cshape->getId());
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				if(((Circle*)Cshape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
				{
					((PCDCircle*)Cshape)->RemoveParentShapes(s);
					continue;
				}
			}
			for each(BaseItem* act in Cshape->PointAtionList)
				((AddPointAction*)act)->ActionStatus(false);
			Cshape->IsValid(false);
			if(Cshape->getGroupChilds().size() > 0) 
				deleteRelatedShape(Cshape);
			if(Cshape->getChild().size() > 0) 
				deleteRelatedShape(Cshape);
			if(Cshape->getMchild().size() > 0)
				deleteRelatedMeasure(Cshape);
			delshapes->addItem(Cshape, false);
			rshapes.removeItem(Cshape);
		}
		for(list<BaseItem*>::iterator shapeiterator = s->getucsCopyChild().begin();shapeiterator != s->getucsCopyChild().end(); shapeiterator++)
		{
			ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
		    if(rshapes.getList().find(Cshape->getId()) == rshapes.getList().end()) return;
			BaseItem* CSh = rshapes.getList().at(Cshape->getId());
			for each(BaseItem* act in Cshape->PointAtionList)
				((AddPointAction*)act)->ActionStatus(false);
			Cshape->IsValid(false);
			if(Cshape->getGroupChilds().size() > 0) 
				deleteRelatedShape(Cshape);
			if(Cshape->getChild().size() > 0) 
				deleteRelatedShape(Cshape);
			if(Cshape->getMchild().size() > 0)
				deleteRelatedMeasure(Cshape);
			delshapes->addItem(Cshape, false);
			rshapes.removeItem(Cshape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0006", __FILE__, __FUNCSIG__); }
}


//Deletes the related measurements...//
void DeleteShapesAction::deleteRelatedMeasure(Shape* s)
{
	try
	{
		if(s == NULL)return;
		RCollectionBase& mshapes = MAINDllOBJECT->getDimList();
		for(list<BaseItem*>::iterator shapeiterator = s->getMchild().begin();shapeiterator != s->getMchild().end(); shapeiterator++)
		{
			DimBase* Cdim = (DimBase*)(*shapeiterator);
			Cdim->IsValid(false);
			/*if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
			{
				for(list<BaseItem*>::iterator Pshape = Cdim->getParents().begin();Pshape != Cdim->getParents().end(); Pshape++)
				{
					if(((Shape*)(*Pshape))->getMchild().size() == 0)
					{
						for each(RAction* Caction in  ((ShapeWithList*)(*Pshape))->PointAtionList)
							((AddPointAction*)Caction)->deactivateAction();
						MAINDllOBJECT->getShapesList().removeItem((BaseItem*)(*Pshape));
						MAINDllOBJECT->DepthDatumList.remove((*Pshape)->getId());
					}
				}
			}*/
			delMeasurement->addItem(Cdim, false);
			mshapes.removeItem(Cdim);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0007", __FILE__, __FUNCSIG__); }
}