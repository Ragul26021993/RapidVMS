#include "StdAfx.h"
#include "DimensionHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Shapes\RelatedShape.h"
#include "..\Shapes\PCDCircle.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Cylinder.h"
#include "..\Measurement\DimCircleDeviationMeasure.h"
#include "..\Engine\IsectPt.h"
#include "..\Helper\Helper.h"
#include "..\Measurement\DimTotalLength.h"

DimensionHandler::DimensionHandler()
{
	try
	{
		setMaxClicks(3);
		WrongProceedureCnt = 0;
		init();
		CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DEFAULTMEASURE;
		CurrentPCdCircle = MAINDllOBJECT->CurrentPCDShape;
		MAINDllOBJECT->SetStatusCode("DimensionHandler12");
		if(MAINDllOBJECT->PCDMeasureModeFlag())
			MAINDllOBJECT->SetStatusCode("DimensionHandler13");
		}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0001", __FILE__, __FUNCSIG__); }
}

DimensionHandler::~DimensionHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0002", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::init()
{
	try
	{
		dim = NULL;
		ShapeCount = 0; PointCount = 0;
		for(int Cnt = 0; Cnt < 2; Cnt++)
		{
			Related_Pt1[Cnt] = NULL;
			Related_Pt2[Cnt] = NULL;
			Cshape[Cnt] = NULL;
			vPointer[Cnt] = NULL;
		}
		Cshape[2] = NULL; Cshape[3] = NULL;
		this->CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKNOTHING;
		this->RightClick = false;
		this->MeasureDir = true;
		resetClicks();
		setMaxClicks(3);
		CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DEFAULTMEASURE;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0003", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::LmouseDown()
{
	try
	{
		//if(MAINDllOBJECT->highlightedShape() != NULL) //&& MAINDllOBJECT->highlightedShape()->IsUsedForReferenceDot())
		//{
		//  setClicksDone(getClicksDone() - 1);
		//  return;
		//}
		if(MAINDllOBJECT->PCDMeasureModeFlag())
			Handle_MouseDownForPcdMeasure();
		else
		{
			Vector* pt = &getClicksValue(getClicksDone() - 1);
			if(RightClick)
			{
				Handle_RightClickMode(pt->getX(), pt->getY(), pt->getZ());
				return;
			}
			if(MAINDllOBJECT->highlightedShape() == NULL && MAINDllOBJECT->highlightedPoint() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D || 
				CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D)
			{	
				if (!CompareShapesFor3DTP())
				{
					setClicksDone(getClicksDone() - 1);
					return;
				}
			}
			if(MAINDllOBJECT->highlightedPoint() != NULL)
			{
				vPointer[PointCount++] =  MAINDllOBJECT->highlightedPoint();
				if(PointCount == 1)
				{
					Related_Pt1[0] = MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1();
					Related_Pt1[1] =  MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2();
				}
				else
				{
					Related_Pt2[0] = MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1();
					Related_Pt2[1] =  MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2();
				}
			}
			else if(MAINDllOBJECT->highlightedShape() != NULL)
			{
				Cshape[ShapeCount++] = MAINDllOBJECT->highlightedShape();
				Cshape[ShapeCount - 1]->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			if(getClicksDone() == 2 && CurrentMeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R && CurrentMeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D 
				&& CurrentMeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R && CurrentMeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D)
			{	
				CompareSelectedEntities(pt->getX(), pt->getY(), pt->getZ());
			}
			if(getClicksDone() == 3 && (CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D 
				|| CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D))
			{	
				CompareSelectedEntities(pt->getX(), pt->getY(), pt->getZ());
				//Add3DTPGDnTMeasurement(pt->getX(), pt->getY(), pt->getZ());
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0004", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::Handle_MouseDownForPcdMeasure()
{
	try
	{
		CurrentPCdCircle = MAINDllOBJECT->CurrentPCDShape;
		if(CurrentPCdCircle == NULL)
		{
			MAINDllOBJECT->SetStatusCode("DimensionHandler01");
			setClicksDone(getClicksDone() - 1);
			return;
		}
		bool Cliked_OnRightShape = false;
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Cshape[ShapeCount++] = MAINDllOBJECT->highlightedShape();		
		Cliked_OnRightShape = CheckShapeSelected_PCD(Cshape[ShapeCount - 1]);
		if(!Cliked_OnRightShape)
		{
			ShapeCount--;
			setClicksDone(getClicksDone() - 1);
			if(ShapeCount == 0)
			{
				ResetShapeHighlighted();
				init();
				MAINDllOBJECT->SetStatusCode("PCDMeasureHandler03");
			}
			else 
				MAINDllOBJECT->SetStatusCode("PCDMeasureHandler03");
			return;
		}
		else
		{
			Cshape[ShapeCount - 1]->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
			if(ShapeCount == 2)
			{
				if(Cshape[0]->getId() == Cshape[1]->getId()) // Add PCD Offset Distance Measurement
				{
					dim = HELPEROBJECT->CreatePCDMeasurements(CurrentPCdCircle, Cshape[0], NULL, RapidEnums::MEASUREMENTTYPE::DIM_PCDOFFSET, pt->getX(), pt->getY(), pt->getZ());
					AddShapeMeasureRelationShip(CurrentPCdCircle);
					AddShapeMeasureRelationShip(Cshape[0]);
				}
				else // Add PCD Angle Measurement..
				{
					dim = HELPEROBJECT->CreatePCDMeasurements(CurrentPCdCircle, Cshape[0], Cshape[1], RapidEnums::MEASUREMENTTYPE::DIM_PCDANGLE, pt->getX(), pt->getY(), pt->getZ());
					AddShapeMeasureRelationShip(CurrentPCdCircle);
					AddShapeMeasureRelationShip(Cshape[0]);
					AddShapeMeasureRelationShip(Cshape[1]);
				}
				MAINDllOBJECT->SetStatusCode("PCDMeasureHandler04");
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0004", __FILE__, __FUNCSIG__); }
}


void DimensionHandler::mouseMove()
{
	try
	{
		if(getClicksDone() == 0)return;
		Vector* pt = &getClicksValue(getClicksDone());
		if(dim != NULL)
		{
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(CSline);
			dim->SetSelectionLine(&CSline[0]);
			dim->SetMousePosition(pt->getX(), pt->getY(), pt->getZ());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0005", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		GRAFIX->SetMeasurement_Color(1, 1, 0);
		if(windowno == 0 && MAINDllOBJECT->ShowMeasurementOnVideo())
			drawSelectedShapesOnVideo(WPixelWidth);
		else if(windowno == 1)
			drawSelectedShapesOnRcad(WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0006", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::drawSelectedShapesOnVideo(double wupp)
{
	try
	{
		if(getClicksDone() > 0)
		{
			GRAFIX->SetColor_Double(0, 1, 0);
			for(int Cnt = 0; Cnt < 2; Cnt++)
			{
				if(vPointer[Cnt] != NULL)
					GRAFIX->drawPoint(vPointer[Cnt]->getX(), vPointer[Cnt]->getY(), vPointer[Cnt]->getZ(), 8);
			}
			if(dim != NULL && !dim->MeasureAs3DMeasurement())
				dim->drawMeasurement(0, wupp);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0007", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::drawSelectedShapesOnRcad(double wupp)
{
	try
	{
		if(getClicksDone() > 0)
		{
			GRAFIX->SetColor_Double(0, 1, 0);
			for(int Cnt = 0; Cnt < 2; Cnt++)
			{
				if(vPointer[Cnt] != NULL)
					GRAFIX->drawPoint(vPointer[Cnt]->getX(), vPointer[Cnt]->getY(), vPointer[Cnt]->getZ(), 8);
			}
			if(dim != NULL)
				dim->drawMeasurement(1, wupp);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0008", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::keyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	 try
	 {
		 int code = nChar;
		 if(code == 18)
			MeasureDir = !MeasureDir;
		 RMATH2DOBJECT->MeasurementDirection = MeasureDir;
		 if(dim != NULL)
			 dim->MeasurementDirection(MeasureDir);
		 MAINDllOBJECT->update_VideoGraphics(true);
		 MAINDllOBJECT->update_RcadGraphics(true);
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0009", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("DimensionHandler02");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0010", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::LmaxmouseDown()
{
	try
	{
		AddDimAction::addDim(dim);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0011", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::ResetShapeHighlighted()
{
	try
	{
		if(Cshape[0] != NULL)
			Cshape[0]->HighlightedForMeasurement(false);
		if(Cshape[1] != NULL)
			Cshape[1]->HighlightedForMeasurement(false);
		if(Cshape[2] != NULL)
			Cshape[2]->HighlightedForMeasurement(false);
		if(Cshape[3] != NULL)
			Cshape[3]->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0011", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::RmouseDown(double x, double y)
{
	try
	{
		if(MAINDllOBJECT->PCDMeasureModeFlag()) return;
		//if(MAINDllOBJECT->highlightedShape() != NULL) //&& MAINDllOBJECT->highlightedShape()->IsUsedForReferenceDot())
		//  return;
		if(getClicksDone() == getMaxClicks() - 1)
		{
			dim->ChangeMeasurementType();
			return;
		}
		if(MAINDllOBJECT->highlightedPoint() == NULL && MAINDllOBJECT->highlightedShape() == NULL)
			return;
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			vPointer[PointCount++] =  MAINDllOBJECT->highlightedPoint();
			if(PointCount == 1)
			{
				Related_Pt1[0] = MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1();
				Related_Pt1[1] =  MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2();
			}
			else
			{
				Related_Pt2[0] = MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1();
				Related_Pt2[1] =  MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2();
			}
		}
		else if(MAINDllOBJECT->highlightedShape() != NULL)
		{
			ResetShapeHighlighted();
			 init();
			Cshape[ShapeCount++] = MAINDllOBJECT->highlightedShape();
			Cshape[ShapeCount - 1]->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		RightClick = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0012", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::Handle_RightClickMode(double x, double y, double z)
{
	try
	{
		if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DEFAULTMEASURE) 
		{
			WrongProceedureFollowed();
			return;
		}
		if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE) 
		{
			if((MAINDllOBJECT->highlightedShape() != NULL) && (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::ARC || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CIRCLE))
			{
				MAINDllOBJECT->DerivedShapeCallback();
				DimBase* Cdim = NULL;
				Cdim = new DimCircleDeviationMeasure(true);
				Cdim->MeasurementType = CurrentMeasurementType;
				if(Cshape[0]->getId() == MAINDllOBJECT->highlightedShape()->getId())
				{
					((DimCircleDeviationMeasure*)Cdim)->CalculateMeasurement(Cshape[0]);
					dim = Cdim;
					dim->IsValid(true);
				}
				else
				{
					((DimCircleDeviationMeasure*)Cdim)->CalculateMeasurement(Cshape[0], MAINDllOBJECT->highlightedShape());
					dim = Cdim;
					dim->IsValid(true);
					AddShapeMeasureRelationShip(MAINDllOBJECT->highlightedShape());
				}
				AddShapeMeasureRelationShip(Cshape[0]);			
				LmaxmouseDown();
				MAINDllOBJECT->Measurement_updated();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		switch(CurrentRClickType)
		{
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONPOINT:
				dim = HELPEROBJECT->CreatePointDim(vPointer[0], CurrentMeasurementType, x, y, z);
				AddShapeMeasureRelationShip(Related_Pt1[0]);
				AddShapeMeasureRelationShip(Related_Pt1[1]);
				break;
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONPERIMETER:
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONLINE:
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCIRCLE:
				dim = HELPEROBJECT->CreateRightClickDim2D(Cshape[0], CurrentMeasurementType, x, y, z);
				AddShapeMeasureRelationShip(Cshape[0]);
				break;
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONLINE3D:
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONPLANE:
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCYLINDER:
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONSPHERE:
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCIRCLE3D:
			case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCLOUDPOINTS:
				dim = HELPEROBJECT->CreateRightClickDim3D(Cshape[0], CurrentMeasurementType, x, y, z);
				AddShapeMeasureRelationShip(Cshape[0]);
				break;
		}		
		CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DEFAULTMEASURE;
		RightClick = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0013", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::CompareSelectedEntities(double x, double y, double z)
{
	try
	{
		if(ShapeCount == 4)
			Add3DTPGDnTMeasurement(x, y, z, false);
		else if(ShapeCount == 3 && PointCount == 1)
			Add3DTPGDnTMeasurement(x, y, z, true);
		else if(ShapeCount == 2 && PointCount == 1)
			AddTPGDnTMeasurementForPoint(x, y, z);
		else if(ShapeCount == 3)
			AddTPGDnTMeasurement(x, y, z);
		else if(ShapeCount == 2)
			AddMeasurement_TwoShapes(x, y, z);
		else if(ShapeCount == 1 && PointCount == 1)
			AddMeasurement_OneShapeOnePoint(x, y, z);
		else
			AddMeasurement_TwoPoints(x, y, z);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0014", __FILE__, __FUNCSIG__); }
}

bool DimensionHandler::CompareShapesFor3DTP()
{
	try
	{
		if((MAINDllOBJECT->highlightedShape() != NULL))
        {   
			for(int i = 0; i < ShapeCount; i++)
			{
		     	if(MAINDllOBJECT->highlightedShape()->getId() == Cshape[i]->getId())
					return false;
			}
			list<list<double>> ShapesParam;
			if(!(HELPEROBJECT->getShapeDimension(MAINDllOBJECT->highlightedShape(), &ShapesParam)))
				return false;
		}
		else if (MAINDllOBJECT->highlightedPoint() != NULL)
		{
		    if (MAINDllOBJECT->highlightedPoint() == vPointer[0])
				return false;
		}
		else
		{
		    return false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0014", __FILE__, __FUNCSIG__); return false;}
}

void DimensionHandler::AddMeasurement_TwoShapes(double x, double y, double z)
{
	try
	{
		if(CurrentRClickType != RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKNOTHING)
		{
			bool ContinueExecution = false;
			switch(CurrentRClickType)
			{
				case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONLINE:
					ContinueExecution = AddMeasurement_RightClickOnLine();
					break;
				case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCIRCLE:
					ContinueExecution = AddMeasurement_RightClickOnCircle();
					break;
				case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONLINE3D:
					ContinueExecution = AddMeasurement_RightClickOnLine3D();
					break;
				case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCYLINDER:
					ContinueExecution = AddMeasurement_RightClickOnCylinder();
					break;
				case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONPLANE:
					ContinueExecution = AddMeasurement_RightClickOnPlane();
					break;
				case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONSPHERE:
					ContinueExecution = AddMeasurement_RightClickOnSphere();
					break;
				case RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCONE:
					ContinueExecution = AddMeasurement_RightClickOnCone();
					break;
			}
			if(!ContinueExecution)
			{
				WrongProceedureFollowed();
				return;
			}
		}
		else
		{
			bool Do2DMeasurement = Check2DMeasurement_TwoShapes();
			if(Do2DMeasurement)
				dim = HELPEROBJECT->CreateShapeToShapeDim2D(Cshape[0], Cshape[1], x, y, z);
			else
				dim = HELPEROBJECT->CreateShapeToShapeDim3D(Cshape[0], Cshape[1], x, y, z);
			AddShapeMeasureRelationShip(Cshape[0]);
			if(Cshape[0]->getId() != Cshape[1]->getId())
				AddShapeMeasureRelationShip(Cshape[1]);
			
		}
		ShapeCount = PointCount  = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0015", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::AddMeasurement_OneShapeOnePoint(double x, double y, double z)
{
	try
	{
		if(CurrentRClickType != RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKNOTHING)
		{
			bool ContinueExecution = false;
			WrongProceedureFollowed();
		}
		else
		{
			bool Do2DMeasurement = Check2DMeasurement_OneShapeOnePoint();
			if(Do2DMeasurement)
				dim = HELPEROBJECT->CreatePointToShapeDim2D(vPointer[0], Cshape[0], x, y, z);
			else
				dim = HELPEROBJECT->CreatePointToShapeDim3D(vPointer[0], Cshape[0], x, y, z);
			if(((Vector*)vPointer[0])->IntersectionPoint)
				((IsectPt*)vPointer[0])->SetRelatedDim(dim);
			AddShapeMeasureRelationShip(Related_Pt1[0]);
			AddShapeMeasureRelationShip(Related_Pt1[1]);
			AddShapeMeasureRelationShip(Cshape[0]);
			ShapeCount = PointCount  = 0;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0016", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::AddMeasurement_TwoPoints(double x, double y, double z)
{
	try
	{
		if(getClicksValue(0) == getClicksValue(1))
		{
			dim = HELPEROBJECT->CreatePointDim(vPointer[0], RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE, x, y, z);
			if(((Vector*)vPointer[0])->IntersectionPoint)
				((IsectPt*)vPointer[0])->SetRelatedDim(dim);
			AddShapeMeasureRelationShip(Related_Pt1[0]);
			AddShapeMeasureRelationShip(Related_Pt1[1]);
		}
		else
		{
			bool Do2DMeasurement = Check2DMeasurement_TwoPoints();
			if(Do2DMeasurement)
				dim = HELPEROBJECT->CreatePointDim2D(vPointer[0], vPointer[1], x, y, z);
			else
				dim = HELPEROBJECT->CreatePointDim3D(vPointer[0], vPointer[1], x, y, z);
			if(((Vector*)vPointer[0])->IntersectionPoint)
				((IsectPt*)vPointer[0])->SetRelatedDim(dim);
			if(((Vector*)vPointer[1])->IntersectionPoint)
				((IsectPt*)vPointer[1])->SetRelatedDim(dim);
			AddShapeMeasureRelationShip(Related_Pt1[0]);
			AddShapeMeasureRelationShip(Related_Pt1[1]);
			AddShapeMeasureRelationShip(Related_Pt2[0]);
			AddShapeMeasureRelationShip(Related_Pt2[1]);
		}		
		ShapeCount = PointCount  = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0017", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(dim);
			dim->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0018", __FILE__, __FUNCSIG__); }
}

bool DimensionHandler::AddMeasurement_RightClickOnLine()
{
	try
	{
		if(PointCount == 1)
		{
			ClickedonWrongPoint();
			MAINDllOBJECT->SetStatusCode("DimensionHandler03");
			return false;
		}
		else if(Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE)
			Add2DGDnTMeasurement();
		else
		{
			ClickedonWrongShape();
			MAINDllOBJECT->SetStatusCode("DimensionHandler03");
			return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0019", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::AddMeasurement_RightClickOnCircle()
{
	try
	{
		if(PointCount == 1)
		{

			ClickedonWrongPoint();
			MAINDllOBJECT->SetStatusCode("DimensionHandler04");
			return false;
		}
		else if (Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::ARC || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::RPOINT || 
			Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			Add2DGDnTMeasurement();
		else
		{
			ClickedonWrongShape();
			MAINDllOBJECT->SetStatusCode("DimensionHandler04");
			return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0020", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::AddMeasurement_RightClickOnLine3D()
{
	try
	{
		if(PointCount == 1)
		{
			ClickedonWrongPoint();
			MAINDllOBJECT->SetStatusCode("DimensionHandler05");
			return false;
		}
		else if(Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CONE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::PLANE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
			Add3DGDnTMeasurement();
		else
		{
			ClickedonWrongShape();
			MAINDllOBJECT->SetStatusCode("DimensionHandler05");
			return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0021", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::AddMeasurement_RightClickOnPlane()
{
	try
	{
		if(PointCount == 1)
		{
			ClickedonWrongPoint();
			MAINDllOBJECT->SetStatusCode("DimensionHandler05");
			return false;
		}
		else if(Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::PLANE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CONE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
			Add3DGDnTMeasurement();
		else
		{
			ClickedonWrongShape();
			MAINDllOBJECT->SetStatusCode("DimensionHandler05");
			return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0022", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::AddMeasurement_RightClickOnCylinder()
{
	try
	{
		if(PointCount == 1)
		{
			ClickedonWrongPoint();
			MAINDllOBJECT->SetStatusCode("DimensionHandler06");
			return false;
		}
		else if((CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY) && (Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CONE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CYLINDER))
			Add3DGDnTMeasurement();
		else if((CurrentMeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY) && (Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CONE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CYLINDER || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::PLANE))
			Add3DGDnTMeasurement();
		else
		{
			ClickedonWrongShape();
			MAINDllOBJECT->SetStatusCode("DimensionHandler06");
			return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0023", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::AddMeasurement_RightClickOnSphere()
{
	try
	{
		if(PointCount == 1)
		{
			ClickedonWrongPoint();
			MAINDllOBJECT->SetStatusCode("DimensionHandler07");
			return false;
		}
		else if(Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
			Add3DGDnTMeasurement();
		else
		{
			ClickedonWrongShape();
			MAINDllOBJECT->SetStatusCode("DimensionHandler07");
			return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0024", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::AddMeasurement_RightClickOnCone()
{
try
	{
		if(PointCount == 1)
		{
			ClickedonWrongPoint();
			return false;
		}
		else if((CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY) && (Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CONE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CYLINDER))
			Add3DGDnTMeasurement();
	    else if((CurrentMeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY) && (Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::PLANE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CONE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::CYLINDER))
			Add3DGDnTMeasurement();
		else
		{
			ClickedonWrongShape();
			return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0023con", __FILE__, __FUNCSIG__); return false; }
}

void DimensionHandler::ClickedonWrongPoint()
{
	try
	{
		PointCount = 0;
		vPointer[0] = NULL;
		setClicksDone(getClicksDone() - 1);		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0025", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::ClickedonWrongShape()
{
	try
	{
		ShapeCount--;
		if(Cshape[1] != NULL)
		{
			Cshape[1]->HighlightedForMeasurement(false);
			Cshape[1] = NULL;
			MAINDllOBJECT->Shape_Updated();
		}
		setClicksDone(getClicksDone() - 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0026", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::WrongProceedureFollowed()
{
	try
	{
		//return;
		//WrongProceedureCnt++;
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("DimensionHandler08");
		/*if(WrongProceedureCnt % 2 == 0)
			MAINDllOBJECT->SetStatusCode("Sorry! Wrong Procedure.. Please Follow the Procedure..!", 1);
		else
			MAINDllOBJECT->SetStatusCode("Sorry Boss!! Please Follow the Procedure!", 1);*/

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0027", __FILE__, __FUNCSIG__); }
}

bool DimensionHandler::Check2DMeasurement_TwoShapes()
{
	try
	{
		bool Do2DMeasurement = true;
		if(Cshape[0]->ShapeAs3DShape() || Cshape[1]->ShapeAs3DShape())
			Do2DMeasurement = false;
		return Do2DMeasurement;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0028", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::Check2DMeasurement_OneShapeOnePoint()
{
	try
	{
		bool Do2DMeasurement = true;
		Shape* Related_Shape[2] = {Related_Pt1[0], Related_Pt1[1]};
		if(Cshape[0]->ShapeAs3DShape())
			Do2DMeasurement = false;
		for(int i = 0; i < 2; i++)
		{
			if(Related_Shape[i] != NULL)
			{
				if(Related_Shape[i]->ShapeAs3DShape())
				{
					Do2DMeasurement = false;
					break;
				}
			}
		}
		return Do2DMeasurement;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0029", __FILE__, __FUNCSIG__); return false; }
}
	
bool DimensionHandler::Check2DMeasurement_TwoPoints()
{
	try
	{
		bool Do2DMeasurement = true;
		Shape* Related_Shape[4] = {Related_Pt1[0], Related_Pt1[1], Related_Pt2[0], Related_Pt2[1]};
		for(int i = 0; i < 4; i++)
		{
			if(Related_Shape[i] != NULL)
			{
				if(Related_Shape[i]->ShapeAs3DShape())
				{
					Do2DMeasurement = false;
					break;
				}
			}
		}
		return Do2DMeasurement;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0030", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::CheckShapeSelected_PCD(Shape* CShape)
{
	try
	{
		bool ShapePresent = false;
		for each(Shape* Csh in ((PCDCircle*)CurrentPCdCircle)->PCDParentCollection)
		{
			if(Csh->getId() == CShape->getId()) 
			{
				ShapePresent = true;
				break;
			}
		}
		return ShapePresent;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0007", __FILE__, __FUNCSIG__); }
}

bool DimensionHandler::SetCurrentMeasurementTypeForRightClick(RapidEnums::MEASUREMENTTYPE Cmeasuretype, int type)
{
	try
	{
		CurrentMeasurementType = Cmeasuretype;
		bool flag = false;
		switch(type)
		{
			case 0:
				flag = RightClickOnPoint();
				break;
			case 1:
				flag = RightClickOnLine();
				break;
			case 2:
				flag = RightClickOnCircle();
				break;
			case 3:
				flag = RightClickOnLine3D();
				break;
			case 4:
				flag = RightClickOnPlane();
				break;
			case 5:
				flag = RightClickOnCylinder();
				break;
			case 6:
				flag = RightClickOnSphere();
				break;
			case 7:
				flag = RightClickOnPerimeter();
				break;
			case 8:
				flag = RightClickOnCone();
				break;
			case 9:
				flag = RightClickOnCircle3D();
				break;
			case 10:
			    flag = RightClickOnCloudPoints();
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0031", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnPoint()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONPOINT;
		if(PointCount == 1)
		{
			setClicksDone(1);
			if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R)
			{
				MAINDllOBJECT->TruePositionMode = true;
				MAINDllOBJECT->DerivedShapeCallback();
			}
			return true;
		}
		else
		{
			WrongProceedureFollowed();
			return false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0032", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnLine()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONLINE;
		switch(CurrentMeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					MAINDllOBJECT->DerivedShapeCallbackWithPara(((Line*)Cshape[0])->Length());
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PROFILE:
				if (ShapeCount == 1)
				{
					setClicksDone(1);
					//MAINDllOBJECT->DerivedShapeCallbackWithPara(((Line*)Cshape[0])->Length());
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;

				break;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0033", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnCircle()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCIRCLE;
		if(ShapeCount == 1)
		{
			setClicksDone(1);
			if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R)
			{
				MAINDllOBJECT->TruePositionMode = true;
				MAINDllOBJECT->DerivedShapeCallback();
			}
			/*else if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE)
				MAINDllOBJECT->DerivedShapeCallback();*/
			else if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY_D)
			{
				RightClick = false;
				MAINDllOBJECT->SetStatusCode("DimensionHandler09");
			}
			return true;
		}
		else
		{
			WrongProceedureFollowed();
			return false;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0034", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnCircle3D()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCIRCLE3D;
		if(ShapeCount == 1)
		{
			setClicksDone(1);
			if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R)
			{
				MAINDllOBJECT->TruePositionMode = true;
				MAINDllOBJECT->DerivedShapeCallback();
			}
			else if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R)
			{
				MAINDllOBJECT->TruePositionMode = true;
				MAINDllOBJECT->DerivedShapeCallback();
			}
			return true;
		}
		else
		{
			WrongProceedureFollowed();
			return false;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0034", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnCloudPoints()
{
	   CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCLOUDPOINTS;
		if(ShapeCount == 1)
		{
			setClicksDone(1);
			MAINDllOBJECT->SetStatusCode("DimensionHandler09");
			return true;
		}
		else
		{
			WrongProceedureFollowed();
			return false;
		}
		return false;
}

bool DimensionHandler::RightClickOnLine3D()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONLINE3D;
		switch(CurrentMeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					MAINDllOBJECT->DerivedShapeCallbackWithPara(((Line*)Cshape[0])->Length());
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0035", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnPlane()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONPLANE;
		switch(CurrentMeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS:
            case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					MAINDllOBJECT->DerivedShapeCallbackWithPara(0);
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_AXIALRUNOUT:
				if (ShapeCount == 1)
				{
					setClicksDone(1);
					RightClick = false;
					MAINDllOBJECT->SetStatusCode("DimensionHandler09");
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;

		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0036", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnCylinder()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCYLINDER;
		switch(CurrentMeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					MAINDllOBJECT->DerivedShapeCallbackWithPara(((Cylinder*)Cshape[0])->Length());
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0037", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnSphere()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONSPHERE;
		if(ShapeCount == 1)
		{
			setClicksDone(1);
			return true;
		}
		else
		{
			WrongProceedureFollowed();
			return false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0038", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnPerimeter()
{
	try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONPERIMETER;
		if(ShapeCount == 1)
		{
			setClicksDone(1);
			return true;
		}
		else
		{
			WrongProceedureFollowed();
			return false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0038", __FILE__, __FUNCSIG__); return false; }
}

bool DimensionHandler::RightClickOnCone()
{
try
	{
		CurrentRClickType = RapidEnums::CURRENTRIGHTCLICKEDSHAPE::RIGHTCLICKONCONE;
		switch(CurrentMeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CONICALITY:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
				if(ShapeCount == 1)
				{
					setClicksDone(1);
					MAINDllOBJECT->DerivedShapeCallbackWithPara(((Cone*)Cshape[0])->Length());
					return true;
				}
				else
				{
					WrongProceedureFollowed();
					return false;
				}
				break;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0037", __FILE__, __FUNCSIG__); return false; }
}

void DimensionHandler::SetAnyData(double *data)
{
	try
	{
		//add measurement action to base class, measurement should be valid = true, set parent child relationship............
		//add measurement type & values to measureparameter and set values for excel..........
		//For part program add measurement function to edit, load, save............
		if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE)
		{
			DimBase* Cdim = NULL;
			Cdim = new DimCircleDeviationMeasure();
			Cdim->MeasurementType = CurrentMeasurementType;
			((DimCircleDeviationMeasure*)Cdim)->CalculateMeasurement(Cshape[0], data);
			dim = Cdim;
			dim->IsValid(true);
			AddShapeMeasureRelationShip(Cshape[0]);
			LmaxmouseDown();
			MAINDllOBJECT->Measurement_updated();

		}
		else
		{
			if(data[1] == 0)
			{
				double datumEndPnts[6] = {0};
		    	if(Cshape[0]->ShapeType == RapidEnums::SHAPETYPE::CONE)
				   ((Cone*)Cshape[0])->getEndPoints(&datumEndPnts[0]);
		    	else if(Cshape[0]->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
			       ((Cylinder*)Cshape[0])->getEndPoints(&datumEndPnts[0]);
				else if(Cshape[0]->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
			       ((Line*)Cshape[0])->getEndPoints(&datumEndPnts[0]);
				data[1] = RMATH3DOBJECT->Distance_Point_Point(datumEndPnts, &datumEndPnts[3]);
			}
			AngularityAngle = data[0];
			AngularityLand = data[1];
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				AngularityAngle = data[0]/25.4;
			RightClick = false;
			MAINDllOBJECT->SetStatusCode("DimensionHandler10");
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0039", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::Add2DGDnTMeasurement()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		dim = HELPEROBJECT->CreateAngularityDim2D(Cshape[0], Cshape[1], NULL, CurrentMeasurementType, AngularityAngle, AngularityLand,  pt->getX(), pt->getY(), pt->getZ());
		AddShapeMeasureRelationShip(Cshape[0]);
		AddShapeMeasureRelationShip(Cshape[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0040", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::AddTPGDnTMeasurement(double x, double y, double z)
{
	try
	{
		if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D 
			||CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY)
		{
			if(!((Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XRAY)
				&& (Cshape[2]->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape[2]->ShapeType == RapidEnums::SHAPETYPE::XLINE || Cshape[2]->ShapeType == RapidEnums::SHAPETYPE::XRAY)))
			{
				ResetShapeHighlighted();
				init();
				//MAINDllOBJECT->SetStatusCode("DimensionHandler11");
				return;
			}
		}
		else
		{
			if(!HELPEROBJECT->CheckLinePerpendicularity(Cshape[1], Cshape[2]))
			{
				ResetShapeHighlighted();
				init();
				MAINDllOBJECT->SetStatusCode("DimensionHandler11");
				return;
			}
		}	
		dim = HELPEROBJECT->CreateAngularityDim2D(Cshape[0], Cshape[1], Cshape[2], CurrentMeasurementType, AngularityLand, AngularityAngle, x, y, z);
		AddShapeMeasureRelationShip(Cshape[0]);
		AddShapeMeasureRelationShip(Cshape[1]);
		AddShapeMeasureRelationShip(Cshape[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0040", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::AddTPGDnTMeasurementForPoint(double x, double y, double z)
{
	try
	{
		if(CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D 
			||CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX || CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY)
		{
			if(!((Cshape[0]->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape[0]->ShapeType == RapidEnums::SHAPETYPE::XLINE || Cshape[0]->ShapeType == RapidEnums::SHAPETYPE::XRAY)
				&& (Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XLINE || Cshape[1]->ShapeType == RapidEnums::SHAPETYPE::XRAY)))
			{
				ResetShapeHighlighted();
				init();
	    		return;
			}
		}
		else
		{
			ResetShapeHighlighted();
			init();
			MAINDllOBJECT->SetStatusCode("DimensionHandler11");
			return;
		}	
		dim = HELPEROBJECT->CreateTruePositionDim(vPointer[0], Cshape[0], Cshape[1], CurrentMeasurementType, AngularityLand, AngularityAngle, x, y, z);
		if(((Vector*)vPointer[0])->IntersectionPoint)
			((IsectPt*)vPointer[0])->SetRelatedDim(dim);
		AddShapeMeasureRelationShip(Related_Pt1[0]);
		AddShapeMeasureRelationShip(Related_Pt1[1]);
		AddShapeMeasureRelationShip(Cshape[0]);
		AddShapeMeasureRelationShip(Cshape[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0040", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::Add3DTPGDnTMeasurement(double x, double y, double z, bool ForPoint)
{
	try
	{
		if(ForPoint)
		{
			dim = HELPEROBJECT->Create3DTruePositionForPoint(vPointer[0], Cshape[0], Cshape[1], Cshape[2], CurrentMeasurementType, AngularityLand, AngularityAngle, x, y, z);
			if(((Vector*)vPointer[0])->IntersectionPoint)
				((IsectPt*)vPointer[0])->SetRelatedDim(dim);
			AddShapeMeasureRelationShip(Related_Pt1[0]);
			AddShapeMeasureRelationShip(Related_Pt1[1]);
			AddShapeMeasureRelationShip(Cshape[0]);
			AddShapeMeasureRelationShip(Cshape[1]);
			AddShapeMeasureRelationShip(Cshape[2]);
		}
		else
		{
			dim = HELPEROBJECT->CreateAngularityDim3D(Cshape[0], Cshape[1], Cshape[2], Cshape[3], CurrentMeasurementType, AngularityLand, AngularityAngle, x, y, z);
			AddShapeMeasureRelationShip(Cshape[0]);
			AddShapeMeasureRelationShip(Cshape[1]);
			AddShapeMeasureRelationShip(Cshape[2]);
			AddShapeMeasureRelationShip(Cshape[3]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0040", __FILE__, __FUNCSIG__); }
}

//bool DimensionHandler::CheckTPGDntShapes()
//{
//	try
//	{
//		if(compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::LINE) ||
//			compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::XLINE) ||
//			compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::XLINE))
//		{
//			if(HELPEROBJECT->CheckLinePerpendicularity(Cshape[1], Cshape[2]))
//				return true;
//			else
//			{
//				MAINDllOBJECT->SetStatusCode("Please Select two Perendicular lines as Reference", 1);
//				return false;
//			}
//		}
//		else if(compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::CIRCLE) ||
//				compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::CIRCLE) ||
//				compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ARC) || 
//				compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::ARC))
//		{
//			return true;
//		}
//		else if(compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::CIRCLE) ||
//			compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::ARC) ||
//			compareboth(Cshape[1]->ShapeType, Cshape[2]->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::ARC))
//		{
//			return true;
//		}
//		else
//			return false;
//	}
//	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0040", __FILE__, __FUNCSIG__); }
//}

void DimensionHandler::Add3DGDnTMeasurement()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		dim = HELPEROBJECT->CreateAngularityDim3D(Cshape[0], Cshape[1], NULL, NULL, CurrentMeasurementType, AngularityAngle, AngularityLand, pt->getX(), pt->getY(), pt->getZ());
		AddShapeMeasureRelationShip(Cshape[0]);
		AddShapeMeasureRelationShip(Cshape[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0041", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::SetTrueCenter(double x, double y, int en, bool forPoint)
{
	try
	{
		if(forPoint)
		{
			if(en == 0)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R;
			else if(en == 1)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D;
			else if(en == 2)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX;
			else if(en == 3)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY;
		}
		else
		{
			if(en == 0)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R;
			else if(en == 1)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D;
			else if(en == 2)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX;
			else if(en == 3)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY;
		}
		RightClick = false;
		AngularityLand = x; AngularityAngle = y;
		setClicksDone(0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0042", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::SetTrueCenter3D(double x, double y, int en, bool forPoint)
{
	try
	{
		if(forPoint)
		{
			if(en == 0)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R;
			else if(en == 1)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D;
		}
		else
		{
			if(en == 0)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R;
			else if(en == 1)
				CurrentMeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D;
		}
		setMaxClicks(4);
		RightClick = false;
		AngularityLand = x; AngularityAngle = y;
		setClicksDone(0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0042", __FILE__, __FUNCSIG__); }
}

void DimensionHandler::MeasurementForHighlighted_Shape(int MeasureType, bool AutomateFixMeasure)
{
	try
	{
		double ShapeCenter[3] = {0};
		int Cnt = 0;
		std::list<Shape*> TempShapeList;
		for(RC_ITER item = MAINDllOBJECT->getShapesList().getList().begin(); item !=   MAINDllOBJECT->getShapesList().getList().end(); item++)
		{
			Shape* Cshpe = (Shape*)(*item).second;
			if(!Cshpe->Normalshape()) continue;
			if(!Cshpe->selected()) continue;
			if(Cshpe->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshpe->ShapeType == RapidEnums::SHAPETYPE::ARC || Cshpe->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				TempShapeList.push_back(Cshpe);
				double centerPoints[3] = {0};
				Cshpe->GetShapeCenter(centerPoints);				
				ShapeCenter[0] += centerPoints[0];
				ShapeCenter[1] += centerPoints[1];
				ShapeCenter[2] += centerPoints[2];
				Cnt++;
			}
		}
		ShapeCenter[0] = ShapeCenter[0] / Cnt; ShapeCenter[1] = ShapeCenter[1] / Cnt; ShapeCenter[2] = ShapeCenter[2] / Cnt;
		if(TempShapeList.size() < 1) return;

		this->dim = new DimTotalLength(_T("Dia"), TempShapeList, MeasureType);
		((DimTotalLength*)this->dim)->CalculateMeasurement();
		this->dim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
		for each(Shape* Cpshape in TempShapeList)
		{
			AddShapeMeasureRelationShip(Cpshape);
		}
		if(AutomateFixMeasure)
		{
			DWORD d1 = GetTickCount();
			int Cnt = d1 % 20;
			if(Cnt == 0)
				Cnt = 2;
			std::list<Shape*>::iterator itr = TempShapeList.begin();
			ShapeWithList* Shp = (ShapeWithList*)(*itr);
			int PtsCnt = Shp->PointsList->Pointscount() / Cnt, StepCnt = 0;
			double MousePos[3] = {ShapeCenter[0], ShapeCenter[1], ShapeCenter[2]};
			for(PC_ITER SptItem = Shp->PointsList->getList().begin(); SptItem != Shp->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				if(PtsCnt == StepCnt)
				{
					double X = 3, Y = 3;
					if(Spt->X - ShapeCenter[0] < 0)
						X = -3;
					if(Spt->Y - ShapeCenter[1] < 0)
						Y = -3;
					MousePos[0] = Spt->X + X; MousePos[1] = Spt->Y + Y; MousePos[2] = Spt->Z;
					break;
				}
				StepCnt++;
			}
			this->dim->SetMousePosition(MousePos[0], MousePos[1], MousePos[2]);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).GetSelectionLine(MousePos[0], MousePos[1], SelectionLine);
			this->dim->SetSelectionLine(&SelectionLine[0]);
			setClicksDone(3);
			AddDimAction::addDim(dim);
			ResetShapeHighlighted();
			init();
			MAINDllOBJECT->SetStatusCode("Finished");
		}
		else
		{
			this->dim->SetMousePosition(ShapeCenter[0], ShapeCenter[1], ShapeCenter[2]);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).GetSelectionLine(ShapeCenter[0], ShapeCenter[1], SelectionLine);
			this->dim->SetSelectionLine(&SelectionLine[0]);		
			setClicksDone(2);
		}
		
	}
	catch(...)
	{
	     MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0043A", __FILE__, __FUNCSIG__); 
	}
}

void DimensionHandler::HighlightShapeForMeasurement(int id)
{
	try
	{
		 Shape *cShp = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
		 /*if(cShp->IsUsedForReferenceDot())
		 {
		    setClicksDone(getClicksDone() - 1);
		    return;
		 }*/
		 if(getClicksDone() < 2)
		 {
			  if(MAINDllOBJECT->PCDMeasureModeFlag()) return;
			  Cshape[ShapeCount++] = cShp;
			  Cshape[ShapeCount - 1]->HighlightedForMeasurement(true);
			  MAINDllOBJECT->Shape_Updated();
			  setClicksDone(getClicksDone() + 1);
			  if(getClicksDone() == 2)
			  {	
				   double centerPoints[6] = {0};
				   Cshape[0]->GetShapeCenter(&centerPoints[0]);
				   Cshape[1]->GetShapeCenter(&centerPoints[3]);
				   CompareSelectedEntities((centerPoints[0] + centerPoints[3]) / 2, (centerPoints[1] + centerPoints[4]) / 2, (centerPoints[2] + centerPoints[5]) / 2);
			  }
	    }
	    else
    	{
			 ResetShapeHighlighted();
			 init();
		}
	}
	catch(...)
	{
	     MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0043", __FILE__, __FUNCSIG__); 
	}
}

void DimensionHandler::HighlightShapeRightClickForMeasurement(int id)
{
	Shape *cShp = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
	/*if(cShp->IsUsedForReferenceDot())
	{
		setClicksDone(getClicksDone() - 1);
		return;
	}*/
	ResetShapeHighlighted();
	init();
    MAINDllOBJECT->SetRightClickOnShape(cShp);
	Cshape[ShapeCount++] = cShp;
	Cshape[ShapeCount - 1]->HighlightedForMeasurement(true);
	MAINDllOBJECT->Shape_Updated();
	RightClick = true;
}