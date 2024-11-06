#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\ShapeWithList.h"
#include "PointFromShapeHandler.h"
#include "..\Actions\RAction.h"
#include "..\Actions\AddShapePointsAction.h"
#include "..\Engine\PointCollection.h"

#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

AddShapePointsAction *newAction;
PointFromShapeHandler::PointFromShapeHandler()
{
	try
	{
		setMaxClicks(1);
		DrawShape = NULL;
		ActionAdded = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0001", __FILE__, __FUNCSIG__); }
}

PointFromShapeHandler::PointFromShapeHandler(bool PartProgRunning)
{
	try
	{
		runningPartprogramValid = PartProgRunning;
		ActionAdded = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0001", __FILE__, __FUNCSIG__); }
}

PointFromShapeHandler::~PointFromShapeHandler()
{
	try
	{

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0002", __FILE__, __FUNCSIG__); }
}

void PointFromShapeHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0002a", __FILE__, __FUNCSIG__); }
}

void PointFromShapeHandler::mouseMove()
{
}

void PointFromShapeHandler::draw(int windowno, double WPixelWidth)
{
}

void PointFromShapeHandler::LmouseUp(double x, double y)
{
}

void PointFromShapeHandler::LmaxmouseDown()
{
	try
	{
		PointCollection PtsColl;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				AddShapePointsAction *Myaction = (AddShapePointsAction*)(PPCALCOBJECT->getCurrentAction());
				ShapeWithList *ParentShape;
				for each(Shape* Shp in Myaction->AddedShapesId)
				{
					if(Shp != NULL)
					{
						ParentShape =(ShapeWithList*)Shp;
						for(PC_ITER SptItem = ParentShape->PointsList->getList().begin(); SptItem != ParentShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;				
							double ValueX = Spt->X, ValueY = Spt->Y, ValueZ = Spt->Z;				
							PtsColl.Addpoint(new SinglePoint(ValueX, ValueY,ValueZ));				   
						}
					}					
				}
				runningPartprogramValid = false;
				DrawShape->ResetCurrentPointsList();
				DrawShape->AddPoints(&PtsColl);
				return;
			}	
		} 
		else
		{
			this->DrawShape = NULL;
		    if((ShapeWithList*)(MAINDllOBJECT->getShapesList().getSelected().getList().size() < 2)) return;
			
			for(RC_ITER i = MAINDllOBJECT->getShapesList().getSelected().getList().begin(); i != MAINDllOBJECT->getShapesList().getSelected().getList().end(); i++)
			{
				ShapeWithList *TempselectedShape = (ShapeWithList*)(*i).second;
				if(TempselectedShape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && TempselectedShape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(TempselectedShape->PointsList->Pointscount() == 0)
				{
					this->DrawShape = TempselectedShape;
					break;
				}
			}

			if(this->DrawShape == NULL) return;

			for(RC_ITER i = MAINDllOBJECT->getShapesList().getSelected().getList().begin(); i !=MAINDllOBJECT->getShapesList().getSelected().getList().end(); i++)
			{			
				ShapeWithList *TempselectedShape = (ShapeWithList*)(*i).second;
				if(TempselectedShape->getId() == this->DrawShape->getId()) continue;
				if(TempselectedShape->PointsList->SelectedPointsCnt > 0)
				{
					for(PC_ITER SptItem = TempselectedShape->PointsList->getList().begin(); SptItem != TempselectedShape->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;	
						if(Spt->IsSelected)
						{
							double ValueX = Spt->X, ValueY = Spt->Y, ValueZ = Spt->Z;				
							PtsColl.Addpoint(new SinglePoint(ValueX, ValueY,ValueZ));
						}	   
					}
				}
				else
				{
					for(PC_ITER SptItem = TempselectedShape->PointsList->getList().begin(); SptItem != TempselectedShape->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;				
						double ValueX = Spt->X, ValueY = Spt->Y, ValueZ = Spt->Z;				
						PtsColl.Addpoint(new SinglePoint(ValueX, ValueY,ValueZ));				   
					}
				}
			} 
	  		if(DrawShape != NULL)
			{
				if(!ActionAdded)
				{
					ActionAdded = true;
					newAction = new AddShapePointsAction();
					newAction->setShape(DrawShape);
				}
				//newAction->AddedShapesId.clear();
				//DrawShape->ResetCurrentPointsList();
				DrawShape->AddPoints(&PtsColl);
				
				for(RC_ITER i = MAINDllOBJECT->getShapesList().getSelected().getList().begin(); i !=MAINDllOBJECT->getShapesList().getSelected().getList().end(); i++)
				{					 
					ShapeWithList *TempselectedShape = (ShapeWithList*)(*i).second;	
					if(TempselectedShape->getId() == this->DrawShape->getId()) continue;
					if(TempselectedShape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && TempselectedShape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
					newAction->AddedShapesId.push_back(TempselectedShape);				
				}			 
				MAINDllOBJECT->addAction(newAction);
			}
			else
			{
				return;
			}	  
		   MAINDllOBJECT->ClearShapeSelections();	 
	   }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0007", __FILE__, __FUNCSIG__); MAINDllOBJECT->ClearShapeSelections();}
}

void PointFromShapeHandler::RmouseDown(double x, double y)
{
}

void PointFromShapeHandler::MouseScroll(bool flag)
{
}

void PointFromShapeHandler::EscapebuttonPress()
{
	try
	{
		MAINDllOBJECT->getShapesList().clearSelection();
		MAINDllOBJECT->getDimList().clearSelection();
		MAINDllOBJECT->SetStatusCode("Cleared Selection");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0008", __FILE__, __FUNCSIG__); }
}

void PointFromShapeHandler::PartProgramData()
{
	try
	{
		AddShapePointsAction *Myaction = (AddShapePointsAction*)(PPCALCOBJECT->getCurrentAction());
		DrawShape = (ShapeWithList*)Myaction->getShape();
		if(DrawShape != NULL)
			LmaxmouseDown();
		resetClicks();
		PPCALCOBJECT->partProgramReached();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0009", __FILE__, __FUNCSIG__); }
}