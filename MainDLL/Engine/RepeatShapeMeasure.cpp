#include "StdAfx.h"
#include "RepeatShapeMeasure.h"
#include "RCadApp.h"
#include "..\Shapes\Vector.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\ShapeHeaders.h"


RepeatShapeMeasure* RepeatShapeMeasure::_RSMInstance = 0;

RepeatShapeMeasure* RepeatShapeMeasure::GetRSMInstance()
{
	if(_RSMInstance == NULL)
		_RSMInstance = new RepeatShapeMeasure();
	return _RSMInstance;
}

RepeatShapeMeasure::RepeatShapeMeasure()
{
	try
	{
		this->initializeVariable();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0001", __FILE__, __FUNCSIG__); }
}

RepeatShapeMeasure::~RepeatShapeMeasure()
{
}

void RepeatShapeMeasure::initializeVariable()
{
	try
	{
		Theta = 0;
		Coord_points.deleteAll();
		ShapeMeasure_relation.clear();
		SptCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0002", __FILE__, __FUNCSIG__); }
}

bool RepeatShapeMeasure::CheckRSMInstance()
{
	if(_RSMInstance == NULL)
		return false;
	else 
		return true;
}

void RepeatShapeMeasure::DeleteInstance()
{
	try
	{
		if(_RSMInstance != NULL)
		{
			delete _RSMInstance;
			_RSMInstance = NULL;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0003", __FILE__, __FUNCSIG__); }
}
	
void RepeatShapeMeasure::ClearAll()
{
	try
	{
		this->initializeVariable();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0004", __FILE__, __FUNCSIG__); }
}

void RepeatShapeMeasure::AutoMeasurementFor_IdenticalShapeMeasure(std::list<double> *cord_list)
{
	try
	{	
		for(int i = 0; i < 9; i++)
			Transform_Matrix[i] = 0;
		Transform_Matrix[0] = cos(Theta); Transform_Matrix[1] = -sin(Theta); 
		Transform_Matrix[3] = sin(Theta); Transform_Matrix[4] = cos(Theta);
		Transform_Matrix[8] = 1;
		//get coordinates as collection of points.......
		int PtCount = 0;
		double PtArray[3] = {0};
		for each(double Cvalue in *cord_list)
		{
			PtArray[PtCount++] = Cvalue;
			if(PtCount == 3)
			{
				PtCount = 0;
				Coord_points.Addpoint(new SinglePoint(PtArray[0], PtArray[1], PtArray[2]));
			}
	    }

		if(MAINDllOBJECT->AutomatedMeasureModeFlag())
		{
			MAINDllOBJECT->AutomatedMeasureModeFlag(false);
			ResetParam_RepeatShapeMeasure(0);
		    getCoordinateFor_NextShape(); 
		}			
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0005", __FILE__, __FUNCSIG__); }
}

//get coordinate from list of coordinate......
void RepeatShapeMeasure::getCoordinateFor_NextShape()
{
	try
	{	
		SptCount++;
		if(SptCount == Coord_points.Pointscount())
		{
			MAINDllOBJECT->UpdateShapesChanged();
			MAINDllOBJECT->setHandler(new DefaultHandler());
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			Coord_points.deleteAll();
			MAINDllOBJECT->Automated_ShapeTypeList.clear();
			MAINDllOBJECT->Automated_MeasurementTypeList.clear();
			ResetParam_RepeatShapeMeasure(1);
			return;
		}
	    if(Coord_points.Pointscount() > 1)
			getShape_Parameter(true);
			
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0006", __FILE__, __FUNCSIG__); }
}

//get shape from list of shapes.......
void RepeatShapeMeasure::getShape_Parameter(bool firsttime)
{
	try
	{
		if(firsttime)
		{	
			int k = MAINDllOBJECT->Automated_ShapeTypeList.size();
			if(k > 0)
			{
				Shapeitr = MAINDllOBJECT->Automated_ShapeTypeList.begin();
				if((ShapeWithList*)(*Shapeitr)->getParents().size() > 0)
				{
					UpdateDerivedShape();
				}
				else
				{
					Shape* Cshape = (Shape*)(*Shapeitr);
					Shape* newshape = Cshape->Clone(0);
					AddShapeAction::addShape(newshape);
					ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
					CreateActionForShapes();
				}
			}	
		}
		else if(Shapeitr == MAINDllOBJECT->Automated_ShapeTypeList.end())
		{
			CreateMeasurement();
			getCoordinateFor_NextShape();
		}
		else
		{
			if((ShapeWithList*)(*Shapeitr)->getParents().size() > 0)
			{
				UpdateDerivedShape();
			}
			else
			{
				Shape* Cshape = (Shape*)(*Shapeitr);
				Shape* newshape = Cshape->Clone(0);
				AddShapeAction::addShape(newshape);
				ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
				CreateActionForShapes();
			}
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0007", __FILE__, __FUNCSIG__); }
}

void RepeatShapeMeasure::UpdateDerivedShape()
{
	try
	{
		//if(ShapeMeasure_relation.size() < 1)	return;
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		SinglePoint* CoordPt =  Coord_points.getList()[SptCount];
		SinglePoint* CoordPt1 =  Coord_points.getList()[0];
		Vector difference(CoordPt->X  - CoordPt1->X, CoordPt->Y - CoordPt1->Y, CoordPt->Z - CoordPt1->Z);
		if(Theta != 0)
		{
			Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, difference.getX(), difference.getY());	
			difference.set(TmpValue.getX(), TmpValue.getY(), CoordPt->Z - CoordPt1->Z);
		}
		int j = myshapeList->PointAtionList.size();
		if(j == 0)
		{
			Shape* Cshape = (Shape*)(*Shapeitr);
			Shape* newshape = Cshape->Clone(0);
			AddShapeAction::addShape(newshape);
			ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
			list<BaseItem*>::iterator ptr = Cshape->getParents().begin();
			while(ptr != Cshape->getParents().end())
			{
				Shape* PShape = (Shape*)(*ptr);
				Shape* newParentShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[PShape->getId()]];
				newParentShape->addChild(newshape);
				newshape->addParent(newParentShape);
				ptr++;
			}
		}
		else if(j == 1)
		{
			std::list<BaseItem*>::iterator itr = myshapeList->PointAtionList.begin(); 
			RAction* Currentaction = static_cast<RAction*>((*itr));
			if(Currentaction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) return;
			AddPointAction* Caction = (AddPointAction*)(*itr);
			FramegrabBase* base_action = Caction->getFramegrab();
			Vector MousePt, temp;
			if(base_action->CurrentWindowNo == 1 && base_action->Pointer_SnapPt != NULL)
			{
				temp.set(base_action->Pointer_SnapPt->getX() + difference.getX(), base_action->Pointer_SnapPt->getY() + difference.getY(), base_action->Pointer_SnapPt->getZ() + difference.getZ());			
				UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
				Vector* TempV = MAINDllOBJECT->getVectorPointer_UCS(&temp, ucs, true);
				MousePt.set(TempV->getX(), TempV->getY(), TempV->getZ());
			}
			else
			{
				if(myshapeList->PointsListOriginal->getList().size() > 0)
				{
					SinglePoint* Spt = (SinglePoint*)(*myshapeList->PointsListOriginal->getList().begin()).second;
					MousePt.set(Spt->X + difference.getX(), Spt->Y + difference.getY(), Spt->Z + difference.getZ());
				}
			}
			Shape* Cshape = (Shape*)(*Shapeitr);
			Shape* newshape = Cshape->Clone(0);
			AddShapeAction::addShape(newshape);
			ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
			MAINDllOBJECT->getCurrentHandler()->setBaseRProperty(MousePt.getX(), MousePt.getY(), MousePt.getZ());
			list<BaseItem*>::iterator ptr = Cshape->getParents().begin();
			while(ptr != Cshape->getParents().end())
			{
				Shape* PShape = (Shape*)(*ptr);
				Shape* newParentShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[PShape->getId()]];
				newParentShape->addChild(newshape);
				newshape->addParent(newParentShape);
				ptr++;
			}
			if(base_action->CurrentWindowNo == 1)
			{
				MAINDllOBJECT->getCurrentHandler()->baseaction->CurrentWindowNo = 1;
				MAINDllOBJECT->getCurrentHandler()->baseaction->DroMovementFlag = false;
			}		
			AddPointAction::pointAction((ShapeWithList*)newshape, MAINDllOBJECT->getCurrentHandler()->baseaction);
		}
		else if(j == 2)
		{
			Shape* Cshape = (Shape*)(*Shapeitr);
			Shape* newshape = Cshape->Clone(0);
			AddShapeAction::addShape(newshape);
			ShapeMeasure_relation[Cshape->getId()] = newshape->getId();
			for(std::list<BaseItem*>::iterator itr = myshapeList->PointAtionList.begin(); itr != myshapeList->PointAtionList.end(); itr++)
			{
				RAction* Currentaction = static_cast<RAction*>((*itr));
				if(Currentaction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) return;
				AddPointAction* Caction = (AddPointAction*)(*itr);
				FramegrabBase* base_action = Caction->getFramegrab();
				Vector MousePt;
				MousePt.set(base_action->points[0].getX() + difference.getX(), base_action->points[0].getY() + difference.getY(), base_action->points[0].getZ() + difference.getZ());
				MAINDllOBJECT->getCurrentHandler()->setBaseRProperty(MousePt.getX(), MousePt.getY(), MousePt.getZ());
				if(base_action->CurrentWindowNo == 1)
				{
					MAINDllOBJECT->getCurrentHandler()->baseaction->CurrentWindowNo = 1;
					MAINDllOBJECT->getCurrentHandler()->baseaction->DroMovementFlag = false;
				}
				AddPointAction::pointAction((ShapeWithList*)newshape, MAINDllOBJECT->getCurrentHandler()->baseaction, false);
			}
			list<BaseItem*>::iterator ptr = Cshape->getParents().begin();
			while(ptr != Cshape->getParents().end())
			{
				Shape* PShape = (Shape*)(*ptr);
				Shape* newParentShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[PShape->getId()]];
				newParentShape->addChild(newshape);
				newshape->addParent(newParentShape);
				ptr++;
			}
		}
		else
			return;
		Shapeitr++;
		getShape_Parameter(false);
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0010", __FILE__, __FUNCSIG__); }
}

void RepeatShapeMeasure::CreateActionForShapes()
{
	try
	{
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		ShapeWithList* Csh = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		for(std::list<BaseItem*>::iterator itr = myshapeList->PointAtionList.begin(); itr != myshapeList->PointAtionList.end(); itr++)
		{
			RAction* Currentaction = static_cast<RAction*>((*itr));
			if(Currentaction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				SinglePoint* CoordPt1 =  Coord_points.getList()[0];
				SinglePoint* CoordPt =  Coord_points.getList()[SptCount];
				Vector difference(CoordPt->X  - CoordPt1->X, CoordPt->Y - CoordPt1->Y, CoordPt->Z - CoordPt1->Z);
				if(Theta != 0)
				{
					Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, difference.getX(), difference.getY());	
					difference.set(TmpValue.getX(), TmpValue.getY(), CoordPt->Z - CoordPt1->Z);
				}
				AddPointAction* Caction = (AddPointAction*)(*itr);
				FramegrabBase* baseaction = new FramegrabBase(Caction->getFramegrab());
				baseaction->TranslatePosition(difference, ((AddPointAction*)Caction)->getShape()->UcsId());
				for each(int Id in Caction->getFramegrab()->PointActionIdList)
				{
					SinglePoint* Spt = myshapeList->PointsList->getList()[Id];
					baseaction->AllPointsList.Addpoint(new SinglePoint(Spt->X + difference.getX(), Spt->Y + difference.getY(), Spt->Z + difference.getZ(), Spt->R, Spt->Pdir));
				}
				AddPointAction::pointAction(Csh, baseaction);
			}
			else
				return;
		}
		Shapeitr++;
		getShape_Parameter(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0012", __FILE__, __FUNCSIG__); }
}

void RepeatShapeMeasure::InitialiseNextGrid()
{
	try
	{
		Coord_points.deleteAll();
		ShapeMeasure_relation.clear();
		SptCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0013", __FILE__, __FUNCSIG__); }
}

void RepeatShapeMeasure::SetOrientationOfComponent()
{
	try
	{
		ShapeWithList* Csh = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(Csh != NULL)
		{
			if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				Theta = ((Line*)Csh)->Angle();
				if(Theta > M_PI_2)
					Theta -= M_PI;
				else if(Theta < -M_PI_2)
					Theta += M_PI;
				ResetParam_RepeatShapeMeasure(2);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0013", __FILE__, __FUNCSIG__); }
}

void RepeatShapeMeasure::CreateMeasurement()
{
	try
	{
		for each(DimBase* measurement in MAINDllOBJECT->Automated_MeasurementTypeList)
		{
			//create clone of measurement.......
			DimBase* newmeasurement = measurement->Clone(0);
			SinglePoint* CoordPt =  Coord_points.getList()[SptCount];
		    SinglePoint* CoordPt1 =  Coord_points.getList()[0];
		    Vector difference(CoordPt->X  - CoordPt1->X, CoordPt->Y - CoordPt1->Y, CoordPt->Z - CoordPt1->Z);
			if(Theta != 0)
			{
				Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, difference.getX(), difference.getY());	
				difference.set(TmpValue.getX(), TmpValue.getY(), CoordPt->Z - CoordPt1->Z);
			}
			//translate measurement......
			newmeasurement->TranslatePosition(difference);
			Shape *oldPShape1 = NULL, *oldPShape2 = NULL, *oldPShape3 = NULL, *oldPShape4 = NULL;
			Vector *OldParentPoint1 = NULL, *OldParentPoint2 = NULL;
			int Shapecount = 0, PointCount = 0;
			oldPShape1 = measurement->ParentShape1;
			if(oldPShape1 != NULL)
			{
				Shapecount++;
				oldPShape2 = measurement->ParentShape2;
				if(oldPShape2 != NULL)
				{
					Shapecount++;
					oldPShape3 = measurement->ParentShape3;
					if(oldPShape3 != NULL)
					{
						Shapecount++;
						oldPShape4 = measurement->ParentShape4;
						if(oldPShape4 != NULL)
							Shapecount++;
					}
				}
			}
			if(measurement->getParents().size() > Shapecount)
			{
				Vector Point1, Point2;
				if(measurement->ParentPoint1 != NULL)
				{
					Point1.set(measurement->ParentPoint1->getX() + difference.getX(), measurement->ParentPoint1->getY() + difference.getY(), measurement->ParentPoint1->getZ() + difference.getZ());
					PointCount++;
					UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
					OldParentPoint1 = MAINDllOBJECT->getVectorPointer_UCS(&Point1, ucs, true);
					if(OldParentPoint1 == NULL)
					{
						OldParentPoint1->set(Point1.getX(), Point1.getY(), Point1.getZ());
					}
				}
				if(measurement->ParentPoint2 != NULL)
				{
					Point2.set(measurement->ParentPoint2->getX() + difference.getX(), measurement->ParentPoint2->getY() + difference.getY(), measurement->ParentPoint2->getZ() + difference.getZ());
					PointCount++;
					UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
					OldParentPoint2 = MAINDllOBJECT->getVectorPointer_UCS(&Point2, ucs, true);
					if(OldParentPoint2 == NULL)
					{
						OldParentPoint2->set(Point2.getX(), Point2.getY(), Point2.getZ());
					}
				}
			}
						
			if(Shapecount == 4)
			{
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
                newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
				newmeasurement->ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape3->getId()]];
				newmeasurement->ParentShape4 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape4->getId()]];
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
                HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape3, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape4, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);
			}
			else if(Shapecount == 3)
			{
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
                newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
				newmeasurement->ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape3->getId()]];
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
                HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape3, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);
			}
			else if(Shapecount == 2 && PointCount == 1)
			{
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
                newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
				Shape* Cshape = NULL;
				for(list<BaseItem*>::iterator itr = measurement->getParents().begin(); itr != measurement->getParents().end(); itr++)
				{
					Shape* PShape = (Shape*)(*itr);
					if(PShape->getId() != newmeasurement->ParentShape1->getId() && PShape->getId() != newmeasurement->ParentShape2->getId())
					{
						Cshape = PShape;
						break;
					}
				}
				newmeasurement->ParentPoint1 = OldParentPoint1;
				HELPEROBJECT->AddShapeMeasureRelationShip(Cshape, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);
			}
			else if(Shapecount == 2)
			{
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
                newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape2->getId()]];
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);
			}
			else if(Shapecount == 1)
			{
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
                HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);
			}
			else if(Shapecount == 1 && PointCount == 1)
			{
				newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeMeasure_relation[oldPShape1->getId()]];
				Shape* Cshape = NULL;
				for(list<BaseItem*>::iterator itr = measurement->getParents().begin(); itr != measurement->getParents().end(); itr++)
				{
					Shape* PShape = (Shape*)(*itr);
					if(PShape->getId() != newmeasurement->ParentShape1->getId())
					{
						Cshape = PShape;
						break;
					}
				}
				newmeasurement->ParentPoint1 = OldParentPoint1;
				HELPEROBJECT->AddShapeMeasureRelationShip(Cshape, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);						
			}
			else if(PointCount == 2)
			{
				Shape *Cshape1 = NULL, *Cshape2 = NULL;
				for(list<BaseItem*>::iterator itr = measurement->getParents().begin(); itr != measurement->getParents().end(); itr++)
				{
					if(Cshape1 == NULL)
					{
						Cshape1 = (Shape*)(*itr);
					}
					else
					{
						Cshape2 = (Shape*)(*itr);
						break;
					}
				}
				newmeasurement->ParentPoint1 = OldParentPoint1;
				newmeasurement->ParentPoint2 = OldParentPoint2;
				HELPEROBJECT->AddShapeMeasureRelationShip(Cshape1, newmeasurement);
				HELPEROBJECT->AddShapeMeasureRelationShip(Cshape2, newmeasurement);
				newmeasurement->UpdateMeasurement();
				AddDimAction::addDim(newmeasurement);	
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RSM0014", __FILE__, __FUNCSIG__); }
}