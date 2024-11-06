#include "StdAfx.h"
#include "RW_MainInterface.h"

RapidDimListener::RapidDimListener()
{
}

RapidDimListener::~RapidDimListener()
{
}

//Measurement Added
void RapidDimListener::itemAdded(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_MeasureParameter::MYINSTANCE()->AddMeasurementParameter(item);
		RWrapper::RW_MeasureParameter::MYINSTANCE()->RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_ADDED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDIML0001", ex);	}
}

//Measurement Removed
void RapidDimListener::itemRemoved(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_MeasureParameter::MYINSTANCE()->Measure_EntityID = item->getId();
		RWrapper::RW_MeasureParameter::MYINSTANCE()->RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_REMOVED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDIML0002", ex);	}
}

//Measurement Modified
void RapidDimListener::itemChanged(BaseItem* item, Listenable* sender)
{
	try
	{
		DimBase* Cdim = (DimBase*)item;
		if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
			RWrapper::RW_Thread::MYINSTANCE()->GetSelectedThreadParameters();
		else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
			RWrapper::RW_FocusDepth::MYINSTANCE()->Update_FocusDepthValues();
		RWrapper::RW_MeasureParameter::MYINSTANCE()->AddMeasurementParameter(item);
		RWrapper::RW_MeasureParameter::MYINSTANCE()->RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_UPDATE);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDIML0003", ex);	}
}

//Measurement Selection Changed
void RapidDimListener::selectionChanged(Listenable* sender)
{
	try
	{		
		RWrapper::RW_MeasureParameter::MYINSTANCE()->SelectedMeasureIDList->Clear();
		RCollectionBase& Measurecollselected = MAINDllOBJECT->getSelectedDimList();		
		if(Measurecollselected.count() == 1)
		{
			RC_ITER i = Measurecollselected.getList().begin();
			DimBase* Cdim = (DimBase*)((*i).second);
			RWrapper::RW_MeasureParameter::MYINSTANCE()->SelectedMeasureIDList->Add(Cdim->getId());
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				RWrapper::RW_FocusDepth::MYINSTANCE()->Update_FocusDepthValues();
			else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
				RWrapper::RW_Thread::MYINSTANCE()->GetSelectedThreadParameters();
		}
		else
		{
			for(RC_ITER i = Measurecollselected.getList().begin(); i != Measurecollselected.getList().end(); i++)
			{
				DimBase* Cdim = (DimBase*)((*i).second);
				RWrapper::RW_MeasureParameter::MYINSTANCE()->SelectedMeasureIDList->Add(Cdim->getId());
			}
		}
	   RWrapper::RW_MeasureParameter::MYINSTANCE()->RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_SELECTION_CHANGED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDIML0004", ex);	}
}