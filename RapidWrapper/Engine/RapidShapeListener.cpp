#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Shapes\Circle.h"

RapidShapeListener::RapidShapeListener()
{
}

RapidShapeListener::~RapidShapeListener()
{
}

//Shape added  //Modified by Rahul on 19 may 12..........
void RapidShapeListener::itemAdded(BaseItem* item, Listenable* sender)
{
	try
	{
		if(((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::DUMMY) return;
		if(((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE)
		{
			RWrapper::RW_MainInterface::EntityName = gcnew System::String(item->getOriginalName());
			RWrapper::RW_ShapeParameter::MYINSTANCE()->Shape_EntityID = item->getId();
			RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(1);
		}
		else
			RWrapper::RW_ShapeParameter::MYINSTANCE()->Shape_EntityID = item->getId();
			
		RWrapper::RW_ShapeParameter::MYINSTANCE()->SetShapeParamForFrontend(((Shape*)item));
		RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_ADDED);
		MAINDllOBJECT->UpdateCenterOfRotation();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RSHAPEL0001", ex); }
}

//Shape Removed. //Modified by Rahul on 19 may 12..........
void RapidShapeListener::itemRemoved(BaseItem* item, Listenable* sender)
{
	try
	{
		if(((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::DUMMY) return;
		if(((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE)
		{
			RWrapper::RW_MainInterface::EntityName = gcnew System::String(item->getOriginalName());
			RWrapper::RW_ShapeParameter::MYINSTANCE()->Shape_EntityID = item->getId();
			RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(2);
		}
		else
			RWrapper::RW_ShapeParameter::MYINSTANCE()->Shape_EntityID = item->getId();
		RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_REMOVED);
		RWrapper::RW_ShapeParameter::MYINSTANCE()->UpdateSelectedShape_PointsTable();
		MAINDllOBJECT->UpdateCenterOfRotation();
		if(((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			MAINDllOBJECT->CalculateCloudPtZlevelExtent();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RSHAPEL0002", ex); }	
}

//Shape Parameter Changed //Modified by Rahul on 19 may 12..........
void RapidShapeListener::itemChanged(BaseItem* item, Listenable* sender)
{
	try
	{
		if(((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::TRIANGULARSURFACE) return;
		RWrapper::RW_ShapeParameter::MYINSTANCE()->UpdateSelectedShape_PointsTable();
		RWrapper::RW_ShapeParameter::MYINSTANCE()->SetShapeParamForFrontend(((Shape*)item));
		RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_UPDATE);
		MAINDllOBJECT->UpdateCenterOfRotation();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RSHAPEL0003", ex); }
}

//Shape Selection Changed  //Modified by Rahul on 19 may 12..........
void RapidShapeListener::selectionChanged(Listenable* sender)
{
	try
	{
		RWrapper::RW_ShapeParameter::MYINSTANCE()->SelectedShapeIDList->Clear();
		RCollectionBase& Shapecollselected = MAINDllOBJECT->getSelectedShapesList();
		if(Shapecollselected.count() < 1)
			RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_SELECTION_CHANGED);
		else if(Shapecollselected.count() == 1)
		{
			Shape* CShape = (Shape*)Shapecollselected.getList().begin()->second;
			if(CShape->ShapeType != RapidEnums::SHAPETYPE::DUMMY && CShape->ShapeType != RapidEnums::SHAPETYPE::DEPTHSHAPE)
			{
				RWrapper::RW_ShapeParameter::MYINSTANCE()->SelectedShapeIDList->Add(CShape->getId());
				RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_SELECTION_CHANGED);
			}
			else
				RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_SELECTION_CHANGED);
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				if(((Circle*)CShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
				{
					RWrapper::RW_PCD::MYINSTANCE()->GetSelectedPCDParameters();
				}
			}
		}
		else
		{
			for(RC_ITER i = Shapecollselected.getList().begin(); i != Shapecollselected.getList().end(); i++)
			{
				if(((Shape*)((*i).second))->ShapeType != RapidEnums::SHAPETYPE::DUMMY && ((Shape*)((*i).second))->ShapeType != RapidEnums::SHAPETYPE::DEPTHSHAPE)
					RWrapper::RW_ShapeParameter::MYINSTANCE()->SelectedShapeIDList->Add((*i->second).getId());
			}
			RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_SELECTION_CHANGED);
		}
		RWrapper::RW_ShapeParameter::MYINSTANCE()->UpdateSelectedShape_PointsTable();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RSHAPEL0004", ex); }
}