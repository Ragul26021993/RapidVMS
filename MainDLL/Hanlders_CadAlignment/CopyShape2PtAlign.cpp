#include "stdafx.h"
#include "CopyShape2PtAlign.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Actions\AddDimAction.h"
#include "..\Measurement\DimBase.h"

CopyShape2PtAlign::CopyShape2PtAlign()
{
	try
	{
		 setMaxClicks(5);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN01", __FILE__, __FUNCSIG__);
	}
}

CopyShape2PtAlign::~CopyShape2PtAlign()
{
	try
	{
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN02", __FILE__, __FUNCSIG__);
	}
}

void CopyShape2PtAlign::SelectPoints_Shapes()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() < 3)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
		       points[getClicksDone() - 1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
		}
		else if(getClicksDone() == 3 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
		       points[getClicksDone() - 1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
		}
		else if(getClicksDone() == 4 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
		       points[getClicksDone() - 1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
			CreateCopyOfShapes_Measure_Action();
			resetClicks();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN03", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::Align_Finished()
{
	try
	{
		resetClicks();	
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN04", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::drawSelectedShapesOndxf()
{
	try
	{
		
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN05", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::drawSelectedShapesOnvideo()
{
	try
	{
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN06", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::Align_mouseMove(double x, double y)
{
	 try
	 {
	 }
	 catch(...)
	 {
		 MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN07", __FILE__, __FUNCSIG__);
	 }
}

void CopyShape2PtAlign::HandlePartProgramData()
{
	try
	{
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN08", __FILE__, __FUNCSIG__);
	}
}

void CopyShape2PtAlign::CreateCopyOfShapes_Measure_Action()
{
	try
	{	
		for(int i = 0; i < 9; i++)
			Transform_Matrix[i] = 0;
		double Point1[3] = {points[0].getX(), points[0].getY(), points[0].getZ()}, Point2[3] = {points[1].getX(), points[1].getY(), points[1].getZ()}, Point3[3] = {points[2].getX(), points[2].getY(), points[2].getZ()}, Point4[3] = {points[3].getX(), points[3].getY(), points[3].getZ()};
		double Angle1 = RMATH2DOBJECT->ray_angle(Point1, Point2);
		double Angle2 = RMATH2DOBJECT->ray_angle(Point3, Point4);
		double AngDiff = Angle2 - Angle1;
		TransPt.set(Point3[0] - Point1[0], Point3[1] - Point1[1]);
		RMATH2DOBJECT->TransformM_RotateAboutPoint(Point3, AngDiff, Transform_Matrix);
		Difference.set(Transform_Matrix[2], Transform_Matrix[5]);
		Transform_Matrix[2] = 0; Transform_Matrix[5] = 0;
		ShapeTypeList.clear();
		MeasurementTypeList.clear();
		for(RC_ITER Item_shape = MAINDllOBJECT->getShapesList().getList().begin(); Item_shape != MAINDllOBJECT->getShapesList().getList().end(); Item_shape++)
		{
			Shape* Cshape = (Shape*)(*Item_shape).second;
			if(!Cshape->Normalshape() || Cshape->getId() < MAINDllOBJECT->ToolAxisLineId + 2) continue;
			if(Cshape->selected())
			{
				ShapeTypeList.push_back(Cshape);
				for(list<BaseItem*>::iterator Measureiterator = Cshape->getMchild().begin(); Measureiterator != Cshape->getMchild().end(); Measureiterator++)
				{
					DimBase* Cdim = (DimBase*)(*Measureiterator);
					bool MeasurementPresent = false;
					for each(DimBase* Measure in MeasurementTypeList)
					{
						if(Measure->getId() == Cdim->getId())
						{
							MeasurementPresent = true;
							break;
						}
					}
					if(!MeasurementPresent)
						MeasurementTypeList.push_back(Cdim);
				}
			}
		}
	    getCoordinateFor_NextShape(true); 
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN09", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::getCoordinateFor_NextShape(bool FirstTime)
{
	try
	{	
		if(FirstTime)
		{
			getShape_Parameter(true);
		}
		else
		{
			MAINDllOBJECT->UpdateShapesChanged();
		}			
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN10", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::getShape_Parameter(bool firsttime)
{
	try
	{
		if(firsttime)
		{	
			int k = ShapeTypeList.size();
			if(k > 0)
			{
				Shapeitr = ShapeTypeList.begin();
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
		else if(Shapeitr == ShapeTypeList.end())
		{
			CreateMeasurement();
			getCoordinateFor_NextShape(false);
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
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN11", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::UpdateDerivedShape()
{
	try
	{
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
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
				Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, base_action->Pointer_SnapPt->getX() + TransPt.getX(), base_action->Pointer_SnapPt->getY() + TransPt.getY());	
				temp.set(TmpValue.getX() + Difference.getX(), TmpValue.getY() + Difference.getY(), base_action->Pointer_SnapPt->getZ());			
				UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
				Vector* TempV = MAINDllOBJECT->getVectorPointer_UCS(&temp, ucs, true);
				MousePt.set(TempV->getX(), TempV->getY(), TempV->getZ());
			}
			else
			{
				if(myshapeList->PointsListOriginal->getList().size() > 0)
				{
					SinglePoint* Spt = (SinglePoint*)(*myshapeList->PointsListOriginal->getList().begin()).second;
					Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, Spt->X + TransPt.getX(), Spt->Y + TransPt.getY());	
					MousePt.set(TmpValue.getX() + Difference.getX(), TmpValue.getY() + Difference.getY(), Spt->Z);
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
				Vector MousePt, temp;
				if(base_action->CurrentWindowNo == 1 && base_action->Pointer_SnapPt != NULL)
				{
					Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, base_action->Pointer_SnapPt->getX() + TransPt.getX(), base_action->Pointer_SnapPt->getY() + TransPt.getY());	
					temp.set(TmpValue.getX() + Difference.getX(), TmpValue.getY() + Difference.getY(), base_action->Pointer_SnapPt->getZ());			
					UCS* ucs = &MAINDllOBJECT->getCurrentUCS();
					Vector* TempV = MAINDllOBJECT->getVectorPointer_UCS(&temp, ucs, true);
					MousePt.set(TempV->getX(), TempV->getY(), TempV->getZ());
				}
				else
				{
					if(myshapeList->PointsListOriginal->getList().size() > 0)
					{
						SinglePoint* Spt = (SinglePoint*)(*myshapeList->PointsListOriginal->getList().begin()).second;
						Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, Spt->X + TransPt.getX(), Spt->Y + TransPt.getY());	
						MousePt.set(TmpValue.getX() + Difference.getX(), TmpValue.getY() + Difference.getY(), Spt->Z);
					}
				}
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

void CopyShape2PtAlign::CreateActionForShapes()
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
				AddPointAction* Caction = (AddPointAction*)(*itr);
				FramegrabBase* baseaction = new FramegrabBase(Caction->getFramegrab());
				baseaction->TranslatePosition(TransPt, ((AddPointAction*)Caction)->getShape()->UcsId());
				baseaction->TrasformthePosition(Transform_Matrix, ((AddPointAction*)Caction)->getShape()->UcsId());
				baseaction->TranslatePosition(Difference, ((AddPointAction*)Caction)->getShape()->UcsId());
				for each(int Id in Caction->getFramegrab()->PointActionIdList)
				{
					SinglePoint* Spt = myshapeList->PointsList->getList()[Id];
					Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, Spt->X + TransPt.getX(), Spt->Y + TransPt.getY());	
					baseaction->AllPointsList.Addpoint(new SinglePoint(TmpValue.getX() + Difference.getX(), TmpValue.getY() + Difference.getY(), Spt->Z, Spt->R, Spt->Pdir));
				}
				AddPointAction::pointAction(Csh, baseaction);
			}
			else
				return;
		}
		Shapeitr++;
		getShape_Parameter(false);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN13", __FILE__, __FUNCSIG__); 
	}
}

void CopyShape2PtAlign::CreateMeasurement()
{
	try
	{
		for each(DimBase* measurement in MeasurementTypeList)
		{
			//create clone of measurement.......
			DimBase* newmeasurement = measurement->Clone(0);
			//translate measurement......
			newmeasurement->TranslatePosition(TransPt);
			newmeasurement->TransformPosition(Transform_Matrix);
			newmeasurement->TranslatePosition(Difference);
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
					Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, measurement->ParentPoint1->getX() +TransPt.getX(), measurement->ParentPoint1->getY() + TransPt.getY());	
					Point1.set(TmpValue.getX() + Difference.getX(), TmpValue.getY() + Difference.getY(), measurement->ParentPoint1->getZ());
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
					Vector TmpValue = MAINDllOBJECT->TransformMultiply(Transform_Matrix, measurement->ParentPoint2->getX() + TransPt.getX(), measurement->ParentPoint2->getY() + TransPt.getY());	
					Point2.set(TmpValue.getX() + Difference.getX(), TmpValue.getY() + Difference.getY(), measurement->ParentPoint2->getZ());
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
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CSP2PTALIGN15", __FILE__, __FUNCSIG__); 
	}
}
