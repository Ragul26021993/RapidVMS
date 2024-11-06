#include "StdAfx.h"
#include "Helper.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\IsectPt.h"
#include "..\Engine\CalibrationCalculation.h"
#include "..\Actions\ActionHeaders.h"
#include "..\Shapes\ShapeHeaders.h"
#include "..\Measurement\MeasurementHeaders.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\DXF\DXFExpose.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "..\Engine\PartProgramFunctions.h"

//#include "..\Actions\AddDimAction.h"
//#include "..\Actions\LineArcAction.h"
// Get the nearest shape..
// Get the nearest measurement...
// Draw the snap rectangle.....///
// Move the selected shape collection..//
// Draw the exrension Line for Implicit intersection point...//
// Smart Measurement Mode Calculations.. Select..Update etc..//

   EdgeDetectionSingleChannel::ScanDirection LastDir = EdgeDetectionSingleChannel::ScanDirection::Upwards;

HelperClass::HelperClass()
{
	try
	{
		isFirstFrame = false;
		MultiFeatureScan = false;
		AutoProfileScanEnable = false;
		StartProfileScan = false;
		profileScanEndPtPassed = false;
		PickEndPoint_ProfileScan = false;
		profile_scan_running = false;
		profile_scan_failed = false;
		profile_scan_stopped = false;
		profile_scan_paused = false;
		this->ProfileScanLineArcAction = NULL;
		CurrentProfScanMode = RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_NOTHING;
		profile_scan_auto_thread = false;
		Scan_Complete_Profile = false;
		skip_cont_constraint = false;
		SptCount = 0;
		ProbeMeasurelist = NULL;
		ProfileScanJumpParam = -1;
		SnapPt_Pointer = NULL;
		SnapPtForFixture = false;
		IsFixtureCallibration = false;
		RotationScanFlag = false;
		profileScanPixelPt[0] = 0;
		profileScanPixelPt[1] = 0;
		profileScanPixelPt[2] = 0;
		profileScanPixelPt[3] = 0;
		SurfaceAlgoType = 1;
		SurfaceAlgoParam[0] = 5; SurfaceAlgoParam[1] = 40; SurfaceAlgoParam[2] = 200; SurfaceAlgoParam[3] = 40; SurfaceAlgoParam[4] = 1;
		AutoScanDone = false;
		CamSizeRatio = 1.0;
		//CamSizeRatioH = 1.0;
		/*profileScanPixelPt[2] = MAINDllOBJECT->getWindow(0).getWinDim().x;
		profileScanPixelPt[3] = MAINDllOBJECT->getWindow(0).getWinDim().y;*/		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0001", __FILE__, __FUNCSIG__); }
}

HelperClass::~HelperClass()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0002", __FILE__, __FUNCSIG__); }
}

Shape* HelperClass::getNearestShapeOnVideo(RCollectionBase& shapes, double x, double y, double snapdist, bool includeselected)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getNearestShapeOnVideo", __FUNCSIG__);
	try
	{
		Vector c1(x, y);
		if(includeselected)//Include selected shapes also...
		{
			for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
			{
				Shape* Cshape = ((Shape*)(*item).second);
				if((Cshape->ShapeAs3DShape() && Cshape->ShapeType != RapidEnums::SHAPETYPE::SPLINE) || Cshape->IsHidden() || !Cshape->IsValid()) continue;
				if((Cshape->Normalshape()) && (MAINDllOBJECT->getCurrentUCS().UCSMode() == 4))
				{
				  double TransformM[16] = {0};
				  RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 4, 1, TransformM);
				  if(Cshape->Shape_IsInWindow(c1, snapdist, TransformM))
					 return Cshape;
				}
				else
				{
				  if(Cshape->Shape_IsInWindow(c1, snapdist))
					return Cshape;
				};
			}
			return NULL;
		}
		else
		{
			for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
			{
				Shape* Cshape = ((Shape*)(*item).second);
				if((Cshape->ShapeAs3DShape() && Cshape->ShapeType != RapidEnums::SHAPETYPE::SPLINE) || Cshape->IsHidden() || !Cshape->IsValid() || Cshape->selected()) continue;
				if((Cshape->Normalshape()) && (MAINDllOBJECT->getCurrentUCS().UCSMode() == 4))
				{
				  double TransformM[16] = {0};
				  RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 4, 1, TransformM);
				  if(Cshape->Shape_IsInWindow(c1, snapdist, TransformM))
					 return Cshape;
				}
				else
				{
				  if(Cshape->Shape_IsInWindow(c1, snapdist))
					return Cshape;
				}
			}
			return NULL;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0003", __FILE__, __FUNCSIG__); return NULL; }
}

//Function to get the nearest shape.. For mouse move..// //pass the collection of shapes... mouse point(x, y)...
Shape* HelperClass::getNearestShape(RCollectionBase& shapes, double x, double y, double snapdist, bool includeselected)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getNearestShape", __FUNCSIG__);
	try
	{
		Vector c1(x, y);
		list<Shape*> TempShapeList;
		if(includeselected)//Include selected shapes also...
		{
			for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
			{
				Shape* Cshape = ((Shape*)(*item).second);
				if((!Cshape->Normalshape()) && (!MAINDllOBJECT->RcadWindow3DMode()) && (Cshape->ShapeType == RapidEnums::SHAPETYPE::PLANE)) continue;
				if(!Cshape->IsHidden() && Cshape->IsValid() && Cshape->Shape_IsInWindow(c1, snapdist))
					TempShapeList.push_back(Cshape);
			}
		}
		else
		{
			for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
			{
				Shape* Cshape = ((Shape*)(*item).second);
				if(!Cshape->IsHidden() && !Cshape->selected() && Cshape->IsValid() && Cshape->Shape_IsInWindow(c1, snapdist))
					TempShapeList.push_back(Cshape);
			}
		}
		Shape *Cshape = NULL;
		double currentZ = -99999;
		if(TempShapeList.size() > 0)
		{
			for(list<Shape*>::iterator It = TempShapeList.begin(); It != TempShapeList.end(); It++)
			{
				Shape * tShape = (*It);
				if(tShape->ShapeType == RapidEnums::SHAPETYPE::INTERSECTIONSHAPE)
				{
					Cshape = tShape;
					break;
				}
				else if(abs(tShape->GetZlevel() - currentZ) < 0.0001)
				{
				   if(ShapePreference(tShape->ShapeType) > ShapePreference(Cshape->ShapeType))
				   { 
					   currentZ = tShape->GetZlevel();
					   Cshape = tShape;
				   }
				}
				else if(tShape->GetZlevel() > currentZ)
				{
					currentZ = tShape->GetZlevel();
					Cshape = *It;
				}
			}
		}
		return Cshape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0004", __FILE__, __FUNCSIG__); return NULL; }
}

//Function to get the nearest shape.. For mouse move..// //pass the collection of shapes... Perpendicular line passing through mp to the view plane..
Shape* HelperClass::getNearestShape(RCollectionBase& shapes, double* Sline, double snapdist, bool includeselected)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getNearestShape1", __FUNCSIG__);
	try
	{
		list<Shape*> TempShapeList;
		if(includeselected)//Include selected shapes also...
		{
			for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
			{
				Shape* Cshape = ((Shape*)(*item).second);
				if((!Cshape->Normalshape()) && (!MAINDllOBJECT->RcadWindow3DMode()) && (Cshape->ShapeType == RapidEnums::SHAPETYPE::PLANE)) continue;
				if(!Cshape->IsHidden() && Cshape->IsValid() && Cshape->Shape_IsInWindow(Sline, snapdist))
				   TempShapeList.push_back(Cshape);
			}	
		}
		else
		{
			for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
			{
				Shape* Cshape = ((Shape*)(*item).second);
				if(!Cshape->IsHidden() && !Cshape->selected() && Cshape->IsValid() && Cshape->Shape_IsInWindow(Sline, snapdist))
					TempShapeList.push_back(Cshape);
			}
		}
		if(TempShapeList.size() > 0)
		{
			return getHighestZLevelShape(&TempShapeList);
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0005", __FILE__, __FUNCSIG__); return NULL;}
}

Shape* HelperClass::getHighestZLevelShape(list<Shape*> *TempShapeList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getHighestZLevelShape", __FUNCSIG__);
	try
	{
		Shape * SelectedShape = NULL;
		double intersectPt[6] = {0}, TransformMatrix[16] = {0}, Zval[2] = {0},  currentZ = -99999;
		int count = 0;
		MAINDllOBJECT->getWindow(1).getTransformMatrixfor3Drotate(TransformMatrix);
		for(list<Shape*>::iterator It = TempShapeList->begin(); It != TempShapeList->end(); It++)
		{
			if((*It)->ShapeType == RapidEnums::SHAPETYPE::INTERSECTIONSHAPE)
			{
				SelectedShape = *It;
				break;
			}
			(*It)->GetSelectedPointOnShape(intersectPt, &count);
			for(int i = 0; i < count; i++)
			{ 
				double pt[4] = {intersectPt[i*3], intersectPt[i*3 + 1], intersectPt[i*3 + 2], 0}, temp[4] = {0};
				int s1[2] = {4, 4}, s2[2] = {4, 1};
				RMATH2DOBJECT->MultiplyMatrix1(TransformMatrix, s1, pt, s2, temp);
				Zval[i] = temp[2];
				if(abs(Zval[i] - currentZ) < 0.0001)
				{
				   if(ShapePreference((*It)->ShapeType) > ShapePreference(SelectedShape->ShapeType))
				   { 
					   currentZ = Zval[i];
					   SelectedShape = *It;
				   }
				}
				else if(Zval[i] > currentZ)
				{
				  currentZ = Zval[i];
				  SelectedShape = *It;
				}
			}
		}
		if(SelectedShape == NULL)
			SelectedShape = *(TempShapeList->begin());
		return SelectedShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0117", __FILE__, __FUNCSIG__); return NULL; }
}

int HelperClass::ShapePreference(RapidEnums::SHAPETYPE stype)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "ShapePreference", __FUNCSIG__);
	switch(stype)
	{
		case RapidEnums::SHAPETYPE::RPOINT:
		case RapidEnums::SHAPETYPE::RPOINT3D:
		case RapidEnums::SHAPETYPE::RELATIVEPOINT:
			return 50;
		case RapidEnums::SHAPETYPE::LINE:
		case RapidEnums::SHAPETYPE::LINE3D:
			return 49;
		case RapidEnums::SHAPETYPE::CIRCLE:
		case RapidEnums::SHAPETYPE::CIRCLE3D:
		case RapidEnums::SHAPETYPE::ARC:
		case RapidEnums::SHAPETYPE::ARC3D:
			return 48;
		case RapidEnums::SHAPETYPE::ELLIPSE3D:
		case RapidEnums::SHAPETYPE::PARABOLA3D:
		case RapidEnums::SHAPETYPE::HYPERBOLA3D:
			return 47;
		default:
			return 0;
	
	}
}

double HelperClass::GetZlevelOfRcadWindow(RCollectionBase& shapes, double* Sline)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetZlevelOfRcadWindow", __FUNCSIG__);
	try
	{
		double WinW = MAINDllOBJECT->getWindow(1).getViewDim().x/2;
		double WinH = MAINDllOBJECT->getWindow(1).getViewDim().y/2;
		double snapdist = 0;
		if(WinW > WinH)
			snapdist = WinH;
		else
			snapdist = WinW;
		list<Shape*> TempShapeList;
		for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
		{
			Shape* Cshape = ((Shape*)(*item).second);
			if(Cshape->Normalshape() && Cshape->IsValid() && Cshape->WithinCirclularView(Sline, snapdist))
				TempShapeList.push_back(Cshape);
		}
		double TotalZ = 0, CurrentZlevel = 0;
		for each(Shape* Csh in TempShapeList)
		{
			TotalZ += Csh->GetZlevel();
		}
		if(TempShapeList.size() > 0)
			CurrentZlevel = TotalZ/((int)TempShapeList.size());
		return CurrentZlevel;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0006", __FILE__, __FUNCSIG__); return 0;}
}

Shape* HelperClass::getNearestShape(std::list<Shape*> ShapeList,double x, double y, double snapdist)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getNearestShape2", __FUNCSIG__);
	try
	{
		Vector c1;
		c1.set(x, y);
		for each(Shape* sh in ShapeList)
		{
			if(sh->Shape_IsInWindow(c1,snapdist))
				return sh;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0007", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* HelperClass::getNearestThreadShape(std::list<Shape*> ShapeList1, std::list<Shape*> ShapeList2,double x, double y, double snapdist, bool flag)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getNearestThreadShape", __FUNCSIG__);
	try
	{
		Vector c1;
		c1.set(x, y);
		if(!flag)
		{
			for each(Shape* sh in ShapeList1)
			{
				if(sh->Shape_IsInWindow(c1,snapdist))
					return sh;
			}
		}
		else
		{		
			for each(Shape* sh in ShapeList2)
			{
				if(sh->Shape_IsInWindow(c1,snapdist))
					return sh;
			}
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* HelperClass::getNearestmeasure(RCollectionBase& measure, double x, double y, int windowno)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getNearestmeasure", __FUNCSIG__);
	try
	{
		if(MAINDllOBJECT->RcadWindow3DMode())
		{
			double  SelectionLine[6];
			DimBase* selectedDim = NULL;
			for(RC_ITER item = measure.getList().begin(); item != measure.getList().end(); item++)
			{
				DimBase* dim = (DimBase*)(*item).second;
				MAINDllOBJECT->getWindow(1).CalculateSelectionLine(SelectionLine);
				if(dim->Measure_IsInWindow(SelectionLine))
				{
					if(selectedDim == NULL)
						selectedDim = dim;
					else if (selectedDim->DistanceFromMousePoint > dim->DistanceFromMousePoint)
						selectedDim = dim;
				}
			}
			if(selectedDim != NULL)
			return selectedDim;
		}
		else
		{
			for(RC_ITER item = measure.getList().begin(); item != measure.getList().end(); item++)
			{
				DimBase* dim = (DimBase*)(*item).second;
				if(dim->Measure_IsInWindow(x, y, windowno))
					return dim;
			}	
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0009", __FILE__, __FUNCSIG__); return NULL;}
}

//Move the shape collection...//
void HelperClass::moveShapeCollection(std::list<Shape*> shapes, Vector &v, bool isNudge, double angle)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "moveShapeCollection", __FUNCSIG__);
	try
	{
		double transform[9];
		double p1[3] = {0, 0, 0}, p2[3] = {v.getX(), v.getY(), v.getZ()};
		if(isNudge)
		{
			Vector TranslateVector;
			TranslateVector.set(p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]);
			for each(Shape* item in shapes)
			item->Translate(TranslateVector);
		}
		else
		{
			Vector GotoOrigin;
			GotoOrigin.set(-p2[0], -p2[1]);
			Vector GotoRotationPts;
			GotoRotationPts.set(p2[0], p2[1]);
			RMATH2DOBJECT->TransformM_RotateAboutPoint(&p2[0], angle, &transform[0]);
			transform[2] = 0; transform[5] = 0;
			for each(Shape* item in shapes)
			item->Translate(GotoOrigin);

			for each(Shape* item in shapes)
			item->Transform(transform);

			for each(Shape* item in shapes)
			item->Translate(GotoRotationPts);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); }
}

//Functions to get Entity Extents of Rcad window..
bool HelperClass::GetRcadWindowEntityExtents(double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetRcadWindowEntityExtents", __FUNCSIG__);
	try
	{
		double PointsColl[12] = {0};
		bool CheckFlag1 = GetSelectedShapeExtents(MAINDllOBJECT->getShapesList(), &PointsColl[0], &PointsColl[2]);
		bool CheckFlag3 = false, CheckFlag2 = false; //GetMeasurementExtents(MAINDllOBJECT->getDimList(), &PointsColl[4], &PointsColl[6]);
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::AUTOFOCUSSCANHANDLER)
		{
			CheckFlag3 = ((AutoFocusScanHandler*)MAINDllOBJECT->getCurrentHandler())->GetEnclosedRectanglefor2dMode(&PointsColl[8], &PointsColl[10]);
		}

		if(CheckFlag1 && CheckFlag2 && CheckFlag3)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 6, 2, Lefttop, Rightbottom);
			return true;
		}
		if(CheckFlag1 && CheckFlag2)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 4, 2, Lefttop, Rightbottom);
			return true;
		}
		if(CheckFlag1 && CheckFlag3)
		{
			double tmpPnts[8] = {0};
			for(int i = 0; i < 4; i++)
			{
				tmpPnts[i] = PointsColl[i];
				tmpPnts[4 + i] = PointsColl[8 + i];
			}
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&tmpPnts[0], 4, 2, Lefttop, Rightbottom);
			return true;
		}
		else if(CheckFlag1)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 2, 2, Lefttop, Rightbottom);
			return true;
		}
		else if(CheckFlag3)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[8], 2, 2, Lefttop, Rightbottom);
			return true;
		}
		else
			return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); return false; }
}

//Functions to get Entity Extents of Dxf window..
bool HelperClass::GetDxfWindowEntityExtents(double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetDxfWindowEntityExtents", __FUNCSIG__);
	try
	{
		double PointsColl[8] = {0};
		bool CheckFlag1 = GetSelectedShapeExtents(DXFEXPOSEOBJECT->CurrentDXFShapeList, &PointsColl[0], &PointsColl[2]);
		bool CheckFlag2 = GetMeasurementExtents(DXFEXPOSEOBJECT->CurrentDXFMeasureList, &PointsColl[4], &PointsColl[6]);
		if(CheckFlag1 && CheckFlag2)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 4, 2, Lefttop, Rightbottom);
			return true;
		}
		else if(CheckFlag1)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 2, 2, Lefttop, Rightbottom);
			return true;
		}
		else
			return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); return false; }
}

bool HelperClass::GetSelectedShapeExtents(RCollectionBase& shapes, double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetSelectedShapeExtents", __FUNCSIG__);
	try
	{
		list<Shape*> TempShapeList;
		for(RC_ITER i = shapes.getList().begin(); i != shapes.getList().end(); i++)
		{
			Shape* Cshape = (Shape*)(*i).second;
			if (Cshape == MAINDllOBJECT->ReferenceDotShape || Cshape == MAINDllOBJECT->SecondReferenceDotShape) continue;
			if(!Cshape->Normalshape()) continue;
			TempShapeList.push_back(Cshape);			
		}
		bool CheckFlag = GetSelectedShapeExtents(TempShapeList, Lefttop, Rightbottom);
		return CheckFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::GetSelectedShapeExtents(std::list<Shape*> shapes, double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetSelectedShapeExtents1", __FUNCSIG__);
	try
	{
		double* PointsColl = (double*)calloc((int)shapes.size() * 6, sizeof(double));
		ZeroMemory(PointsColl, (int)shapes.size() * 6);
		int PtCnt = 0;
		bool CheckFlag = false;
		for each(Shape* Csh in shapes)
		{
			ShapeWithList* Cshape = (ShapeWithList*)Csh;
			if(!Cshape->IsValid()) continue;
			bool AddedExtentsFlag = Cshape->GetShapeExtents(PointsColl + PtCnt, PointsColl + PtCnt + 3);
			if(AddedExtentsFlag)
				PtCnt += 6;
		}
		if(PtCnt > 0)
		{
			CheckFlag = true;
			RMATH2DOBJECT->GetTopLeftAndRightBottom(PointsColl, PtCnt/3, 3, Lefttop, Rightbottom); 
			double RcadWupp = MAINDllOBJECT->getWindow(1).getUppX();
			if(((Rightbottom[0] - Lefttop[0]) < 0.001) && ((Lefttop[1] - Rightbottom[1]) < 0.001)) 
				CheckFlag = false;
		}
		free(PointsColl);
		return CheckFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0015", __FILE__, __FUNCSIG__); return false; }
}

bool HelperClass::GetMeasurementExtents(RCollectionBase& Measurements, double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetMeasurementExtents", __FUNCSIG__);
	try
	{
		if(Measurements.count() < 1) return false;
		double* PointsColl = (double*)calloc((int)Measurements.count() * 4, sizeof(double));
		ZeroMemory(PointsColl, (int)Measurements.count() * 4);
		int PtCnt = 0;
		for(RC_ITER DimItem = Measurements.getList().begin(); DimItem != Measurements.getList().end(); DimItem++)
		{
			DimBase* Cdim = (DimBase*)(*DimItem).second;
			if(Cdim->ChildMeasurementType())continue;
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH || Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD) continue;
			Cdim->getLeftTopnRightBtm(PointsColl + PtCnt, PointsColl + PtCnt + 2);
			PtCnt += 4;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(PointsColl, PtCnt/2, 2, Lefttop, Rightbottom); 
		free(PointsColl);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::GetMeasurementExtents(std::list<DimBase*> Measurements, double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetMeasurementExtents1", __FUNCSIG__);
	try
	{
		if(Measurements.size() < 1) return false;
		double* PointsColl = (double*)calloc((int)Measurements.size() * 4, sizeof(double));
		int PtCnt = 0;
		for each(DimBase* Cdim in Measurements)
		{
			if(Cdim->ChildMeasurementType())continue;
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH || Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD) continue;
			Cdim->getLeftTopnRightBtm(PointsColl + PtCnt, PointsColl + PtCnt + 2);
			PtCnt += 4;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(PointsColl, PtCnt/2, 2, Lefttop, Rightbottom); 
		free(PointsColl);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::GetRcadWindowEntityExtentsfor3DMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetRcadWindowEntityExtentsfor3DMode", __FUNCSIG__);
	try
	{
		double PointsColl[8] = {0};
		bool CheckFlag2 = false, CheckFlag1 = GetSelectedShapeExtents3DMode(&transformMatrix[0], MAINDllOBJECT->getShapesList(), &PointsColl[0], &PointsColl[2]);
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::AUTOFOCUSSCANHANDLER)
		{
			CheckFlag2 = ((AutoFocusScanHandler*)MAINDllOBJECT->getCurrentHandler())->GetEnclosedRectanglefor3dMode(&transformMatrix[0], &PointsColl[4], &PointsColl[6]);
		}
		if(CheckFlag1 && CheckFlag2)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 4, 2, Lefttop, Rightbottom);
			return true;
		}
		else if(CheckFlag1)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[0], 2, 2, Lefttop, Rightbottom);
			return true;
		}
		else if(CheckFlag2)
		{
			RMATH2DOBJECT->GetTopLeftAndRightBottom(&PointsColl[4], 2, 2, Lefttop, Rightbottom);
			return true;
		}
		else
			return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); return false; }
}

bool HelperClass::GetSelectedShapeExtents3DMode(double *transformMatrix, RCollectionBase& shapes, double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetSelectedShapeExtents3DMode", __FUNCSIG__);
	try
	{
		list<Shape*> TempShapeList;
		for(RC_ITER i = shapes.getList().begin(); i != shapes.getList().end(); i++)
		{
			Shape* Cshape = (Shape*)(*i).second;
			if(!Cshape->Normalshape()) continue;
			TempShapeList.push_back(Cshape);			
		}
		bool CheckFlag = GetSelectedShapeExtents3DMode(&transformMatrix[0], TempShapeList, Lefttop, Rightbottom);
		return CheckFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0013", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::GetSelectedShapeExtents3DMode(double *transformMatrix, std::list<Shape*> shapes, double *Lefttop, double *Rightbottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetSelectedShapeExtents3DMode1", __FUNCSIG__);
	try
	{
		double* PointsColl = (double*)calloc((int)shapes.size() * 6, sizeof(double));
		int PtCnt = 0;
		bool CheckFlag = false;
		//double LeftValue[2], RghtValue[2];
		for each(Shape* Csh in shapes)
		{
			ShapeWithList* Cshape = (ShapeWithList*)Csh;
			if(!Cshape->IsValid()) continue;
			bool AddedExtentsFlag = Cshape->GetEnclosedRectanglefor3dMode(&transformMatrix[0], PointsColl + PtCnt, PointsColl + PtCnt + 3);
			if(AddedExtentsFlag)
				PtCnt += 6;
		}
		if(PtCnt > 0)
		{
			CheckFlag = true;
			RMATH2DOBJECT->GetTopLeftAndRightBottom(PointsColl, PtCnt/3, 3, Lefttop, Rightbottom); 
			double RcadWupp = MAINDllOBJECT->getWindow(1).getUppX();
			if((Rightbottom[0] - Lefttop[0]) < 0.001) 
				CheckFlag = false;
		}
		free(PointsColl);
		return CheckFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0015", __FILE__, __FUNCSIG__); return false; }
}

void HelperClass::RectangleEnclosing_Arcfor3DMode(double *ArcDcs, double *center, double *endpt1, double *endpt2, double radius, double startang, double endangle, double *Lefttop, double *RightBottom)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "RectangleEnclosing_Arcfor3DMode", __FUNCSIG__);
	try
	{
		double PtX[30] = {0},PtY[30] = {0};
		int Cnt = 0;	
		PtX[Cnt] = endpt1[0];
		PtY[Cnt++] = endpt1[1];	
		double radVector[3], perVector[3], pts[6] = {endpt1[0], endpt1[1], endpt1[2], center[0], center[1], center[2]};
		
		RMATH3DOBJECT->DCs_TwoEndPointsofLine(&pts[0], &radVector[0]);
		//RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&radVector[0], &ArcDcs[0], &perVector[0]);
		perVector[0] = ArcDcs[1]*radVector[2] - ArcDcs[2]*radVector[1];
		perVector[1] = ArcDcs[2]*radVector[0] - ArcDcs[0]*radVector[2];
		perVector[2] = ArcDcs[0]*radVector[1] - ArcDcs[1]*radVector[0];
		for (double tempang =  0 ; tempang < 360; tempang += 15)
		{
			if(RMATH2DOBJECT->AngleIsinArcorNot(startang, endangle, tempang * M_PI / 180))
			{
				if (endangle > startang)
				{
					PtX[Cnt] = *center + radius * ((radVector[0])*cos(tempang * M_PI / 180 - startang) + (perVector[0])*sin(tempang * M_PI / 180 - startang));
					PtY[Cnt++] = *(center + 1) + radius * ((radVector[1])*cos(tempang * M_PI / 180 - startang) + (perVector[1])*sin(tempang * M_PI / 180 - startang));
				}
				else
				{
					PtX[Cnt] = *center + radius * ((radVector[0])*cos(tempang * M_PI / 180 - startang) - (perVector[0])*sin(tempang * M_PI / 180 - startang));
					PtY[Cnt++] = *(center + 1) + radius * ((radVector[1])*cos(tempang * M_PI / 180 - startang) - (perVector[1])*sin(tempang * M_PI / 180 - startang));
				}
			}
		}
		
		PtX[Cnt] = *endpt2;
		PtY[Cnt++] = *(endpt2 + 1);
		*Lefttop = RMATH2DOBJECT->minimum(&PtX[0], Cnt);
		*(Lefttop + 1) = RMATH2DOBJECT->maximum(&PtY[0], Cnt);
		*RightBottom = RMATH2DOBJECT->maximum(&PtX[0], Cnt);
		*(RightBottom + 1) = RMATH2DOBJECT->minimum(&PtY[0], Cnt);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0016", __FILE__, __FUNCSIG__); }
}

bool HelperClass::PointOnHighlightedShape(Shape* s1, double *point1, double *point2, double *rpoint,double *angle, double *intercept)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "PointOnHighlightedShape", __FUNCSIG__);
	try
	{
		double angle1, intercept1;
		switch(s1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				angle1 = ((Line*)s1)->Angle() + M_PI_2;
				RMATH2DOBJECT->Intercept_LinePassing_Point(point1, angle1,&intercept1);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle1,intercept1,point2,rpoint);
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				angle1 = ((Line*)s1)->Angle() + M_PI_2;
				RMATH2DOBJECT->Intercept_LinePassing_Point(point1, angle1,&intercept1);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle1,intercept1,point2,rpoint);
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				angle1 = ((Line*)s1)->Angle() + M_PI_2;
				RMATH2DOBJECT->Intercept_LinePassing_Point(point1, angle1,&intercept1);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle1,intercept1,point2,rpoint);
				break;
			case RapidEnums::SHAPETYPE::ARC:
				angle1 = RMATH2DOBJECT->ray_angle(((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY(), point1[0], point1[1]);
				RMATH2DOBJECT->Intercept_LinePassing_Point(point1, angle1,&intercept1);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle1,intercept1,point2,rpoint);
				break;
			case RapidEnums::SHAPETYPE::CIRCLE:
				angle1 = RMATH2DOBJECT->ray_angle(((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY(), point1[0], point1[1]);
				RMATH2DOBJECT->Intercept_LinePassing_Point(point1, angle1,&intercept1);
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle1,intercept1,point2,rpoint);
				break;
		}
		*angle = angle1;
		*intercept = intercept1;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0016", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::PointOnHighlightedShapeTest(Shape* s1,double *point1, Shape* s2, double* point3)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "PointOnHighlightedShapeTest", __FUNCSIG__);
	try
	{
		switch(s1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::ARC: 
			case RapidEnums::SHAPETYPE::CIRCLE:
				{
					double slope, intercept;
					double center1[2] = {((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY()};
					RMATH2DOBJECT->TwoPointLine(point1, &center1[0], &slope, &intercept);
					switch(s2->ShapeType)
					{
						case RapidEnums::SHAPETYPE::ARC:
						case RapidEnums::SHAPETYPE::CIRCLE:
							{											            
								double intpoint1[4], intpoint2[4];
								int count[2] = {0, 0};							
								double center2[2] = {((Circle*)s2)->getCenter()->getX(), ((Circle*)s2)->getCenter()->getY()};
								RMATH2DOBJECT->Line_arcinter(slope, intercept, &center2[0], ((Circle*)s2)->Radius(), ((Circle*)s2)->Startangle(), ((Circle*)s2)->Sweepangle(), &intpoint1[0], &intpoint2[0], &count[0]);				        
								if(count[0] == 2)
								{
									double dist1 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intpoint1[0]);
									double dist2 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intpoint1[2]);
									if(dist1 < dist2)
									{
										point3[0] = intpoint1[0];
										point3[1] = intpoint1[1];
									}
									else
									{
										point3[0] = intpoint1[2];
										point3[1] = intpoint1[3];
									}
								}
								else if(count[0] == 1)
								{
									point3[0] = intpoint1[0];
									point3[1] = intpoint1[1];
								}
								else
									return false;
							}
							break;							
						/*case RapidEnums::SHAPETYPE::LINE:
							{		
								double intpoint1[2], intpoint2[2];
								int count[2] = {0, 0};	
								double pnt3[2], pnt4[2];
								pnt3[0] = ((Line*)s2)->getPoint1()->getX(); pnt3[1] = ((Line*)s2)->getPoint1()->getY();
								pnt4[0] = ((Line*)s2)->getPoint2()->getX(); pnt4[1] = ((Line*)s2)->getPoint2()->getY();
								RMATH2DOBJECT->Line_finiteline(((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &pnt3[0], &pnt4[0], slope, intercept,  &intpoint1[0], &intpoint2[0], &count[0]);
								if(count[0] == 1)
								{
									point3[0] = intpoint1[0];
									point3[1] = intpoint1[1];
								}
								else if(count[1] == 1)
								{
									point3[0] = intpoint2[0];
									point3[1] = intpoint2[1];
								}
								else
									return false;
							}
							break;*/
						case RapidEnums::SHAPETYPE::XLINE:
						case RapidEnums::SHAPETYPE::XRAY:
						case RapidEnums::SHAPETYPE::LINE:
							{
								double intpoint1[2];							   
								int count = RMATH2DOBJECT->Line_lineintersection(slope, intercept, ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &intpoint1[0]);								
								if(count == 1)
								{
									point3[0] = intpoint1[0];
									point3[1] = intpoint1[1];	
								}
								else
									return false;															
							}
							break;
					 /* case RapidEnums::SHAPETYPE::XRAY:
						  {
							  double intpoint1[2], intpoint2[2];
							  int count[2] = {0, 0};	
							  double c1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()};
							  RMATH2DOBJECT->raylineint(slope, intercept, &c1[0],((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &intpoint1[0],	&intpoint2[0], &count[0]);						
							  if(count[0] == 1)
								{
									point3[0] = intpoint1[0];
									point3[1] = intpoint1[1];
								}
								else if(count[1] == 1)
								{
									point3[0] = intpoint2[0];
									point3[1] = intpoint2[1];
								}
								else
									return false;
						  }
						  break;*/
					}
				}
				break;				
			case RapidEnums::SHAPETYPE::LINE:
			case RapidEnums::SHAPETYPE::XLINE:
			case RapidEnums::SHAPETYPE::XRAY:
				switch(s2->ShapeType)
				{
					case RapidEnums::SHAPETYPE::ARC:
					case RapidEnums::SHAPETYPE::CIRCLE:
						{
							double intrceptOfpLine;
							RMATH2DOBJECT->Intercept_LinePassing_Point(point1, ((Line*)s1)->Angle() + M_PI_2 , &intrceptOfpLine);
							double intpoint1[4], intpoint2[4];
								int count[2] = {0, 0};							
								double center2[2] = {((Circle*)s2)->getCenter()->getX(), ((Circle*)s2)->getCenter()->getY()};
								RMATH2DOBJECT->Line_arcinter(((Line*)s1)->Angle() + M_PI_2 , intrceptOfpLine, &center2[0], ((Circle*)s2)->Radius(), ((Circle*)s2)->Startangle(), ((Circle*)s2)->Sweepangle(), &intpoint1[0], &intpoint2[0], &count[0]);				        
								if(count[0] == 2)
								{
									double dist1 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intpoint1[0]);
									double dist2 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intpoint1[2]);
									if(dist1 < dist2)
									{
										point3[0] = intpoint1[0];
										point3[1] = intpoint1[1];
									}
									else
									{
										point3[0] = intpoint1[2];
										point3[1] = intpoint1[3];
									}
								}
								else if(count[0] == 1)
								{
									point3[0] = intpoint1[0];
									point3[1] = intpoint1[1];
								}
								else
									return false;
						}
						break;
					case RapidEnums::SHAPETYPE::LINE:
					case RapidEnums::SHAPETYPE::XLINE:
					case RapidEnums::SHAPETYPE::XRAY:
						double intpoint1[2], intpoint2[2];
						int count[2] = {0, 0};	
						double intrceptOfpLine;
						RMATH2DOBJECT->Intercept_LinePassing_Point(point1, ((Line*)s1)->Angle() + M_PI_2 , &intrceptOfpLine);
						//RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)s1)->Angle() + M_PI_2, intrceptOfpLine, point2, point3);
						RMATH2DOBJECT->raylineint(((Line*)s2)->Angle(),((Line*)s2)->Intercept(), point1, ((Line*)s1)->Angle() + M_PI_2, intrceptOfpLine,  &intpoint1[0], &intpoint2[0], &count[0]);
						 if(count[0] == 1)
								{
									point3[0] = intpoint1[0];
									point3[1] = intpoint1[1];
								}
								else if(count[1] == 1)
								{
									point3[0] = intpoint2[0];
									point3[1] = intpoint2[1];
								}
								else
									return false;
						break;
				}
				break;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0016", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::PerpendiculaIntersectiononLine(Shape* s1,double *point,double *rpoint)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "PerpendiculaIntersectiononLine", __FUNCSIG__);
	try
	{
		switch(s1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)s1)->Angle(),((Line*)s1)->Intercept(),point,rpoint);
				return true;
			case RapidEnums::SHAPETYPE::XLINE:
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)s1)->Angle(),((Line*)s1)->Intercept(),point,rpoint);
				return true;
			case RapidEnums::SHAPETYPE::XRAY:
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)s1)->Angle(),((Line*)s1)->Intercept(),point,rpoint);
				return true;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0017", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::IntersectionPointOnHighlightedLine(Shape* s1,double angle, double intercept, double *rpoint)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "IntersectionPointOnHighlightedLine", __FUNCSIG__);
	try
	{
		double angle1, intercept1;
		switch(s1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::LINE:
				angle1 = ((Line*)s1)->Angle();
				intercept1 = ((Line*)s1)->Intercept();
				RMATH2DOBJECT->Line_lineintersection(angle,intercept,angle1,intercept1,rpoint);
				break;
			case RapidEnums::SHAPETYPE::XLINE:
				angle1 = ((Line*)s1)->Angle();
				intercept1 = ((Line*)s1)->Intercept();
				RMATH2DOBJECT->Line_lineintersection(angle,intercept,angle1,intercept1,rpoint);
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				angle1 = ((Line*)s1)->Angle();
				intercept1 = ((Line*)s1)->Intercept();
				RMATH2DOBJECT->Line_lineintersection(angle,intercept,angle1,intercept1,rpoint);
				break;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0018", __FILE__, __FUNCSIG__); return false; }
}

bool HelperClass::IntersectionPointOnHighlightedCircle(Shape* s1, double pointX, double pointY, double angle, double intercept, double *rpoint)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "IntersectionPointOnHighlightedCircle", __FUNCSIG__);
	try
	{
		double Center[2], intersection[4];
		switch(s1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
				Center[0] = ((Circle*)s1)->getCenter()->getX();
				Center[1] = ((Circle*)s1)->getCenter()->getY();
				RMATH2DOBJECT->Line_circleintersection(angle,intercept,&Center[0],((Circle*)s1)->Radius(),&intersection[0]);
				if(RMATH2DOBJECT->Pt2Pt_distance(intersection[0], intersection[1], pointX, pointY) < RMATH2DOBJECT->Pt2Pt_distance(intersection[2], intersection[3], pointX, pointY))
				{
					*rpoint = intersection[0];
					*(rpoint + 1) = intersection[1];
				}
				else
				{
					*rpoint = intersection[2];
					*(rpoint + 1) = intersection[3];
				}
				break;
			case RapidEnums::SHAPETYPE::ARC:
				Center[0] = ((Circle*)s1)->getCenter()->getX();
				Center[1] = ((Circle*)s1)->getCenter()->getY();
				RMATH2DOBJECT->Line_circleintersection(angle,intercept,&Center[0],((Circle*)s1)->Radius(),&intersection[0]);
				if(RMATH2DOBJECT->Pt2Pt_distance(intersection[0], intersection[1], pointX, pointY) < RMATH2DOBJECT->Pt2Pt_distance(intersection[2], intersection[3], pointX, pointY))
				{
					*rpoint = intersection[0];
					*(rpoint + 1) = intersection[1];
				}
				else
				{
					*rpoint = intersection[2];
					*(rpoint + 1) = intersection[3];
				}
				break;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0019", __FILE__, __FUNCSIG__); return false; }
}

bool HelperClass::ChecktheParallelism(Shape* s1, Shape* s2)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "ChecktheParallelism", __FUNCSIG__);
	try
	{
		if(!((s1->ShapeType == RapidEnums::SHAPETYPE::LINE || s1->ShapeType == RapidEnums::SHAPETYPE::XLINE) && 
			(s2->ShapeType == RapidEnums::SHAPETYPE::LINE || s2->ShapeType == RapidEnums::SHAPETYPE::XLINE)))
			return false;
		double S1_angle = ((Line*)s1)->Angle(), S2_angle = ((Line*)s2)->Angle();
		RMATH2DOBJECT->Angle_FirstScndQuad(&S1_angle);
		RMATH2DOBJECT->Angle_FirstScndQuad(&S2_angle);
		if(abs(S1_angle - S2_angle) > 0.00001)
			return false;
		else
			return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0020", __FILE__, __FUNCSIG__); }
}

bool HelperClass::CheckLinePerpendicularity(Shape* s1, Shape* s2)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CheckLinePerpendicularity", __FUNCSIG__);
	try
	{
		if(!((s1->ShapeType == RapidEnums::SHAPETYPE::LINE || s1->ShapeType == RapidEnums::SHAPETYPE::XLINE) && 
			(s2->ShapeType == RapidEnums::SHAPETYPE::LINE || s2->ShapeType == RapidEnums::SHAPETYPE::XLINE)))
			return false;
		double S1_angle = ((Line*)s1)->Angle(), S2_angle = ((Line*)s2)->Angle();
		RMATH2DOBJECT->Angle_FirstScndQuad(&S1_angle);
		RMATH2DOBJECT->Angle_FirstScndQuad(&S2_angle);
		if(abs(abs(S1_angle - S2_angle) - M_PI_2) > 0.0001)
			return false;
		else
			return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0020", __FILE__, __FUNCSIG__); }
}

void HelperClass::AddGDnTReference(DimBase* dim, Shape* s)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddGDnTReference", __FUNCSIG__);
	try{AddGDnTReferenceDatum(dim, s);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0021", __FILE__, __FUNCSIG__); }
}

bool HelperClass::CheckShapeListCloseLoop(std::list<Shape*> *ShapeList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CheckShapeListCloseLoop", __FUNCSIG__);
	try
	{
		if(ShapeList->size() < 2) return false;
		Shape* CFirstShape = (*ShapeList->begin());
		list<Shape*>::iterator Shiter = ShapeList->end();
		Shiter--;
		Shape* CLastShape = (*Shiter);
		Vector FShEndp1, FShEndp2, EShEndp1, EShEndp2;
		if(CFirstShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			FShEndp1.set(((Line*)CFirstShape)->getPoint1()->getX(), ((Line*)CFirstShape)->getPoint1()->getY(), ((Line*)CFirstShape)->getPoint1()->getZ());
			FShEndp2.set(((Line*)CFirstShape)->getPoint2()->getX(), ((Line*)CFirstShape)->getPoint2()->getY(), ((Line*)CFirstShape)->getPoint2()->getZ());
		}
		else
		{
			FShEndp1.set(((Circle*)CFirstShape)->getendpoint1()->getX(), ((Circle*)CFirstShape)->getendpoint1()->getY(), ((Circle*)CFirstShape)->getendpoint1()->getZ());
			FShEndp2.set(((Circle*)CFirstShape)->getendpoint2()->getX(), ((Circle*)CFirstShape)->getendpoint2()->getY(), ((Circle*)CFirstShape)->getendpoint2()->getZ());
		}
		if(CLastShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			EShEndp1.set(((Line*)CLastShape)->getPoint1()->getX(), ((Line*)CLastShape)->getPoint1()->getY(), ((Line*)CLastShape)->getPoint1()->getZ());
			EShEndp2.set(((Line*)CLastShape)->getPoint2()->getX(), ((Line*)CLastShape)->getPoint2()->getY(), ((Line*)CLastShape)->getPoint2()->getZ());
		}
		else
		{
			EShEndp1.set(((Circle*)CLastShape)->getendpoint1()->getX(), ((Circle*)CLastShape)->getendpoint1()->getY(), ((Circle*)CLastShape)->getendpoint1()->getZ());
			EShEndp2.set(((Circle*)CLastShape)->getendpoint2()->getX(), ((Circle*)CLastShape)->getendpoint2()->getY(), ((Circle*)CLastShape)->getendpoint2()->getZ());
		}
		if((FShEndp1.operator==(EShEndp1)) || (FShEndp1.operator==(EShEndp2)))
			return true;
		else if((FShEndp2.operator==(EShEndp1)) || (FShEndp2.operator==(EShEndp2)))
			return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0022", __FILE__, __FUNCSIG__); }
}

double HelperClass::CalculateAreaofShapeList(std::list<Shape*> *ShapeList)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CalculateAreaofShapeList", __FUNCSIG__);
	try
	{
		//Added on March 22 2011... Area Calculation...
		int TotalShapeCnt = (int)ShapeList->size();
		double *ShapeParam = (double*)calloc(TotalShapeCnt  * 9, sizeof(double));
		bool *Shapetype = (bool*)calloc(TotalShapeCnt, sizeof(bool));
		int Cnt = 0, flagCnt = 0;
		for each(Shape* Cpshape in *ShapeList)
		{
			switch(Cpshape->ShapeType)
			{
			case RapidEnums::SHAPETYPE::LINE:
				ShapeParam[Cnt++] = ((Line*)Cpshape)->getPoint1()->getX();
				ShapeParam[Cnt++] = ((Line*)Cpshape)->getPoint1()->getY();
				ShapeParam[Cnt++] = ((Line*)Cpshape)->getPoint2()->getX();
				ShapeParam[Cnt++] = ((Line*)Cpshape)->getPoint2()->getY();
				ShapeParam[Cnt++] = 0;
				ShapeParam[Cnt++] = 0;
				ShapeParam[Cnt++] = ((Line*)Cpshape)->Angle();
				ShapeParam[Cnt++] = ((Line*)Cpshape)->Intercept();
				ShapeParam[Cnt++] = ((Line*)Cpshape)->Length();
				Shapetype[flagCnt++] = true;
				break;
			case RapidEnums::SHAPETYPE::ARC:
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->getendpoint1()->getX();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->getendpoint1()->getY();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->getendpoint2()->getX();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->getendpoint2()->getY();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->getCenter()->getX();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->getCenter()->getY();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->Startangle();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->Sweepangle();
				ShapeParam[Cnt++] = ((Circle*)Cpshape)->Radius();
				Shapetype[flagCnt++] = false;
				break;
			}
		}
		double TArea = RMATH2DOBJECT->AreaOfPolygon(&ShapeParam[0], &Shapetype[0], TotalShapeCnt);
		free(ShapeParam);
		free(Shapetype);
		return TArea;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0023", __FILE__, __FUNCSIG__); }
}

double HelperClass::CalculatePerimeterOfShapeList(std::list<Shape*> *ShapeList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CalculatePerimeterOfShapeList", __FUNCSIG__);
	try
	{
		double PerimeterofSh = 0;
		for each(Shape* Cpshape in *ShapeList)
		{
			if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				PerimeterofSh += ((Line*)Cpshape)->Length();
			else
				PerimeterofSh += ((Circle*)Cpshape)->Length();
		}
		return PerimeterofSh;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0024", __FILE__, __FUNCSIG__); return 0; }
}

void HelperClass::AddContinuityRaltion_DxfShapes(std::list<Shape*> *ShapeList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddContinuityRaltion_DxfShapes", __FUNCSIG__);
	try
	{
		Vector Sh1Endp1, Sh1Endp2, Sh2Endp1, Sh2Endp2;
		std::list<Shape*> TempLineArcShapeList;
		for each(Shape* Csh in *ShapeList)
		{
			if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				PointCollection PtColl;
				PtColl.Addpoint(new SinglePoint(((Line*)Csh)->getPoint1()->getX(), ((Line*)Csh)->getPoint1()->getY(), ((Line*)Csh)->getPoint1()->getZ()));
				PtColl.Addpoint(new SinglePoint(((Line*)Csh)->getPoint2()->getX(), ((Line*)Csh)->getPoint2()->getY(), ((Line*)Csh)->getPoint2()->getZ()));
				((Line*)Csh)->AddPoints(&PtColl);	
			}
			else if(Csh->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				PointCollection PtColl;
				PtColl.Addpoint(new SinglePoint(((Circle*)Csh)->getendpoint1()->getX(), ((Circle*)Csh)->getendpoint1()->getY(), ((Circle*)Csh)->getendpoint1()->getZ()));
				PtColl.Addpoint(new SinglePoint(((Circle*)Csh)->getendpoint2()->getX(), ((Circle*)Csh)->getendpoint2()->getY(), ((Circle*)Csh)->getendpoint2()->getZ()));
				PtColl.Addpoint(new SinglePoint(((Circle*)Csh)->getMidPoint()->getX(), ((Circle*)Csh)->getMidPoint()->getY(), ((Circle*)Csh)->getMidPoint()->getZ()));
				((Circle*)Csh)->AddPoints(&PtColl);	
			}
			TempLineArcShapeList.push_back(Csh);
		}
		for each(Shape* CFtshape in *ShapeList)
		{
			if(CFtshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				Sh1Endp1.set(*((Line*)CFtshape)->getPoint1());
				Sh1Endp2.set(*((Line*)CFtshape)->getPoint2());
			}
			else
			{
				Sh1Endp1.set(*((Circle*)CFtshape)->getendpoint1());
				Sh1Endp2.set(*((Circle*)CFtshape)->getendpoint2());
			}
			for each(Shape* CShape in TempLineArcShapeList)
			{
				if(CShape->getId() == CFtshape->getId())continue;
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					Sh2Endp1.set(*((Line*)CShape)->getPoint1());
					Sh2Endp2.set(*((Line*)CShape)->getPoint2());
				}
				else
				{
					Sh2Endp1.set(*((Circle*)CShape)->getendpoint1());
					Sh2Endp2.set(*((Circle*)CShape)->getendpoint2());
				}
				bool NearestFlag = false;
				if((Sh1Endp1.operator==(Sh2Endp1)) || (Sh1Endp1.operator==(Sh2Endp2)))
					NearestFlag = true;
				else if((Sh1Endp2.operator==(Sh2Endp1)) || (Sh1Endp2.operator==(Sh2Endp2)))
					NearestFlag = true;
				if(NearestFlag)
				{
					CShape->ShapeAsfasttrace(true);
					CFtshape->ShapeAsfasttrace(true);
					CShape->addFgNearShapes(CFtshape);
					CFtshape->addFgNearShapes(CShape);
				}
			}
		}

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); }
}

void HelperClass::AddFgShapeRelationShip(RCollectionBase& shapes, Shape* CFtshape)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddFgShapeRelationShip", __FUNCSIG__);
	try
	{
		Vector Sh1Endp1, Sh1Endp2, Sh2Endp1, Sh2Endp2;
		if(CFtshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			Sh1Endp1.set(*((Line*)CFtshape)->getPoint1());
			Sh1Endp2.set(*((Line*)CFtshape)->getPoint2());
		}
		else
		{
			Sh1Endp1.set(*((Circle*)CFtshape)->getendpoint1());
			Sh1Endp2.set(*((Circle*)CFtshape)->getendpoint2());
		}
		for(RC_ITER item = shapes.getList().begin(); item != shapes.getList().end(); item++)
		{
			Shape* Cshape = ((Shape*)(*item).second);
			if(CFtshape->getId() == Cshape->getId()) continue;
			if(Cshape->ShapeAsfasttrace())
			{
				if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					Sh2Endp1.set(*((Line*)Cshape)->getPoint1());
					Sh2Endp2.set(*((Line*)Cshape)->getPoint2());
				}
				else
				{
					Sh2Endp1.set(*((Circle*)Cshape)->getendpoint1());
					Sh2Endp2.set(*((Circle*)Cshape)->getendpoint2());
				}
				bool NearestFlag = false;
				if((Sh1Endp1.operator==(Sh2Endp1)) || (Sh1Endp1.operator==(Sh2Endp2)))
					NearestFlag = true;
				else if((Sh1Endp2.operator==(Sh2Endp1)) || (Sh1Endp2.operator==(Sh2Endp2)))
					NearestFlag = true;
				if(NearestFlag)
				{
					Cshape->addFgNearShapes(CFtshape);
					CFtshape->addFgNearShapes(Cshape);
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); }
}

void HelperClass::CalculateAreaOfShapeUsingOneShot(Shape* CShape)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CalculateAreaOfShapeUsingOneShot", __FUNCSIG__);
	try
	{
		
		PointCollection* CPointsList = ((ShapeWithList*)CShape)->PointsList;

		list<Shape*> CurrentShapeLst;
		int PointsCount = (int)CPointsList->Pointscount();
		int Step = 12;
		for(int i = 0; i < PointsCount; i++)
		{
			PointCollection PtColl;
			Shape* nshape = new Line(false);
			int Stindex = i, endindex = i + Step;
			if(endindex >= PointsCount)
				endindex = 0;
			SinglePoint* tempPoint1 = CPointsList->getList()[Stindex];
			SinglePoint* tempPoint2 = CPointsList->getList()[endindex];
			PtColl.Addpoint(new SinglePoint(tempPoint1->X, tempPoint1->Y, tempPoint1->Z));
			PtColl.Addpoint(new SinglePoint(tempPoint2->X, tempPoint2->Y, tempPoint2->Z));
			((ShapeWithList*)nshape)->AddPoints(&PtColl);
			CurrentShapeLst.push_back(nshape);
			i += Step - 1;
		}
		for each (Shape* CSh in CurrentShapeLst)
			((Perimeter*)CShape)->AddShape(CSh);
		//((ShapeWithList*)CShape)->UpdateShape();

		//list<LineArc> LineArcColl;
		//list<Pt> AllPointsCollection;
		//map<int, Pt> AllPointsColl;
		//map<int, Pt> AllPointsTempCollection;
		//Pt toppoint; bool flag = true;
		//int Cnt = 0;
		//for(PC_ITER SptItem = CPointsList->getList().begin(); SptItem != CPointsList->getList().end(); SptItem++)
		//{
		//	SinglePoint* Spt = (*SptItem).second;
		//	Pt newpt;
		//	newpt.x = Spt->X; newpt.y = Spt->Y;
		//	newpt.pt_index = Cnt;
		//	if(flag)
		//	{ 
		//		flag = false;
		//		toppoint.x = newpt.x; toppoint.y = newpt.y; 
		//		toppoint.pt_index = newpt.pt_index;
		//	}
		//	else
		//	{
		//		if(newpt.y > toppoint.y)
		//		{
		//			toppoint.x = newpt.x; toppoint.y = newpt.y; 
		//			toppoint.pt_index = newpt.pt_index;
		//		}
		//	}
		//	AllPointsColl[Cnt] = newpt;
		//	Cnt++;
		//}
		//Cnt = 0;
		//Pt FisrtPt;
		//FisrtPt.x = toppoint.x; FisrtPt.y = toppoint.y;
		//FisrtPt.pt_index = 0;
		////toppoint.pt_index = 0;
		//int TotalCnt = AllPointsColl.size();
		//AllPointsTempCollection[0] = FisrtPt;
		//AllPointsCollection.push_back(FisrtPt);
		//AllPointsColl.erase(toppoint.pt_index);
		//
		//for(int i = 0; i < TotalCnt - 1; i++)
		//{
		//	Pt tempPoint = AllPointsTempCollection[Cnt];
		//	Pt CurrentPt;
		//	double dist = 0, mindist = 0; flag = true;
		//	int C_index;
		//	for(map<int, Pt>::iterator Pitem = AllPointsColl.begin(); Pitem != AllPointsColl.end(); Pitem++)
		//	{
		//		Pt Cpt = (*Pitem).second;
		//		double tempdist = 0;
		//		tempdist += pow((tempPoint.x - Cpt.x), 2);  
		//		tempdist += pow((tempPoint.y - Cpt.y), 2);  
		//		dist = sqrt(tempdist);
		//		if(flag)
		//		{ 
		//			mindist = dist; flag = false;
		//			CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
		//			C_index = Cpt.pt_index;
		//		}
		//		else
		//		{
		//			if(mindist > dist)
		//			{
		//				mindist = dist;
		//				CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
		//				C_index = Cpt.pt_index;
		//			}
		//		}
		//	}
		//	Cnt++;
		//	CurrentPt.pt_index = Cnt;
		//	AllPointsTempCollection[Cnt] = CurrentPt;
		//	AllPointsCollection.push_back(CurrentPt);
		//	AllPointsColl.erase(C_index);
		//}

		//list<Shape*> CurrentShapeLst;
		//int PointsCount = (int)AllPointsTempCollection.size();
		//for(int i = 0; i < PointsCount; i++)
		//{
		//	PointCollection PtColl;
		//	Shape* nshape = new Line(false);
		//	int Stindex = i, endindex = i + 12;
		//	if(endindex >= PointsCount)
		//		endindex = 0;
		//	Pt tempPoint1 = AllPointsTempCollection[Stindex];
		//	Pt tempPoint2 = AllPointsTempCollection[endindex];
		//	PtColl.Addpoint(new SinglePoint(tempPoint1.x, tempPoint1.y));
		//	PtColl.Addpoint(new SinglePoint(tempPoint2.x, tempPoint2.y));
		//	((ShapeWithList*)nshape)->AddPoints(&PtColl);
		//	CurrentShapeLst.push_back(nshape);
		//	i += 11;
		//}

		//
		//for each (Shape* CSh in CurrentShapeLst)
		//	((Perimeter*)CShape)->AddShape(CSh);
		////((ShapeWithList*)CShape)->ResetCurrentParameters();
		//((ShapeWithList*)CShape)->UpdateShape();
		
		//double tolerance = 0.02; // getWindow(0).getUppX() / 10000;
		//fnLinesAndArcs(&AllPointsCollection, &LineArcColl, tolerance);
		//map<int, Shape*> CurrentShapeLst;
		//Shape* newShape = NULL;
		//Shape* LastShape = NULL;
		//list<LineArc>::iterator Temp = LineArcColl.begin();
		//list<LineArc>::iterator Temp1 = LineArcColl.end();
		//Temp1--;
		//int FistShIndex = (*Temp).start_pt.pt_index;
		//int LastShIndex = (*Temp1).start_pt.pt_index;
		//int ShapeCount = LineArcColl.size();
		//int TempShapeCount = 0;
		//for each(LineArc Cla in LineArcColl)
		//{
		//	PointCollection PtColl;
		//	int start_index = Cla.start_pt.pt_index;
		//	int end_index = Cla.end_pt.pt_index;
		//	if(abs(Cla.slope) < 0.02) Cla.slope = 0;
		//	Pt tempPoint = AllPointsTempCollection[start_index];
		//	PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	tempPoint = AllPointsTempCollection[end_index];
		//	if(TempShapeCount == ShapeCount - 1)
		//		tempPoint = AllPointsTempCollection[FistShIndex];
		//	PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	if(Cla.slope == 0) // Add new Line..
		//	{
		//		newShape = new Line(false);
		//	}
		//	else
		//	{
		//		newShape = new Circle(false, RapidEnums::SHAPETYPE::ARC);
		//		((Circle*)newShape)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
		//		tempPoint = AllPointsTempCollection[(int)(start_index + end_index)/2];
		//		PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	}
		//	Cla.mid_pt.pt_index = newShape->getId();
		//	CurrentShapeLst[Cla.mid_pt.pt_index] = newShape;
		//	newShape->ShapeAsfasttrace(true);
		//	newShape->ShapeForPerimeter(true);
		//	//AddShapeAction::addShape(newShape);
		//	if(LastShape != NULL)
		//	{
		//		newShape->addFgNearShapes(LastShape);
		//		LastShape->addFgNearShapes(newShape);
		//	}
		//	if(TempShapeCount == ShapeCount - 1)
		//	{
		//		Shape* Csh = (*CurrentShapeLst.begin()).second;
		//		Csh->addFgNearShapes(newShape);
		//		newShape->addFgNearShapes(Csh);
		//	}
		//	((ShapeWithList*)newShape)->AddPoints(&PtColl);
		//	LastShape = newShape;
		//	TempShapeCount++;
		//}

		//LastShape = NULL;
		//for each(LineArc Cla in LineArcColl)
		//{
		//	PointCollection PtColl;
		//	int start_index = Cla.start_pt.pt_index;
		//	int end_index = Cla.end_pt.pt_index;
		//	if(abs(Cla.slope) < 0.02) Cla.slope = 0;
		//	for(int i = start_index; i <= end_index; i++)
		//	{
		//		Pt tempPoint = AllPointsTempCollection[i];
		//		PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	}
		//	LastShape = CurrentShapeLst[Cla.mid_pt.pt_index];
		//	((ShapeWithList*)LastShape)->AddPoints(&PtColl);
		//}

		//

		//for(map<int, Shape*>::iterator ShItem = CurrentShapeLst.begin(); ShItem != CurrentShapeLst.end(); ShItem++)
		//{
		//	Shape* CSh = (*ShItem).second;
		//	((Perimeter*)CShape)->AddShape(CSh);
		//}
		//((ShapeWithList*)CShape)->ResetCurrentParameters();
		//((ShapeWithList*)CShape)->UpdateBestFit();
		//
		//MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025a", __FILE__, __FUNCSIG__); }
}

bool HelperClass::GetAverageImageValue(int StartX, int StartY, int Length, int width, double* AverageValue)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetAverageImageValue", __FUNCSIG__);
	try
	{		
		bool CheckImageGetFlag = false;
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{				
			CheckImageGetFlag = MAINDllOBJECT->SetZoomedImageInEdgeDetection();					
		}
		else
		{
			CheckImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection();
		}
			
		if(!CheckImageGetFlag)
		{
			MAINDllOBJECT->SetStatusCode("HelperClass01");
			MAINDllOBJECT->SetAndRaiseErrorMessage("FGBGrab Error", "FrameGrab Get Image Error: Failed to get Image From camera!", __FUNCSIG__);
			return false;
		}
		int Box[4] = {StartX, StartY, Length, width};
		AverageValue[0] = EDSCOBJECT->CalculateAvgForRectangle(Box);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::DoCurrentFrameGrab_Area(FramegrabBase* currentAction, double* Area, bool SetImage)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "DoCurrentFrameGrab_Area", __FUNCSIG__);
	try
	{
		if(SetImage)
		{
			bool CheckImageGetFlag = true;
			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
					MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
					MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
					CheckImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection();
			}
			else
			{
				if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
					MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
					MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
					CheckImageGetFlag = MAINDllOBJECT->SetZoomedImageInEdgeDetection();
				else
					CheckImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection();
			}
			if(!CheckImageGetFlag)
			{
				MAINDllOBJECT->SetStatusCode("HelperClass01");
				MAINDllOBJECT->SetAndRaiseErrorMessage("FGBGrab Error", "FrameGrab Get Image Error: Failed to get Image From camera!", __FUNCSIG__);
				return false;
			}
		}
		RapidEnums::RAPIDFGACTIONTYPE CFGtype = currentAction->FGtype;
		int CFGdirection = currentAction->FGdirection;
		double CFGWidth = currentAction->FGWidth;

		double p1[2] = {currentAction->myPosition[0].getX(), currentAction->myPosition[0].getY()};
		double p2[2] = {currentAction->myPosition[1].getX(), currentAction->myPosition[1].getY()};
		double p3[2] = {currentAction->myPosition[2].getX(), currentAction->myPosition[2].getY()};
		int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y;

		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM) && MAINDllOBJECT->DigitalZoomModeForOneShot())
		{
			for(int i = 0; i < 2; i++)
			{
				p1[i] = p1[i] * 2;
				p2[i] = p2[i] * 2;
				p3[i] = p3[i] * 2;
			}
			wwidth = wwidth * 2;
			wheight = wheight * 2;
		}
		int RectFGArray[5]; 
		double CircularArray[6];
		//According to the type calculate the points(grab the points: edge)
		if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB)
		{
			RMATH2DOBJECT->CircleParameters_FG(&p1[0], &p2[0], CFGWidth, &CircularArray[0]);
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB)
		{
			p1[1] = wheight - p1[1]; p2[1] = wheight - p2[1]; p3[1] = wheight - p3[1];
			RMATH2DOBJECT->ArcParameters_FG(&p1[0], &p2[0], &p3[0], CFGWidth, &CircularArray[0]);
			CircularArray[1] = wheight - CircularArray[1];
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE)
		{
			RectFGArray[0] = min(p1[0], p2[0]); RectFGArray[1] = min(p1[1], p2[1]);
			RectFGArray[2] = max(p1[0], p2[0]) - RectFGArray[0];
			RectFGArray[3] = max(p1[1], p2[1]) - RectFGArray[1];
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
		{
			RectFGArray[0] = p1[0]; RectFGArray[1] = p1[1];
			RectFGArray[2] = p2[0]; RectFGArray[3] = p2[1];
			RectFGArray[4] = CFGWidth * 2;
		}
		try
		{		
			if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB)
				Area[1] = EDSCOBJECT->CalculateWhiteAreaStraightRect(&RectFGArray[0], &Area[0]);
			else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
				Area[1] = EDSCOBJECT->CalculateWhiteAreaAngularRect(&RectFGArray[0], &Area[0]);
			else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB)
				Area[1] = EDSCOBJECT->CalculateWhiteAreaCurvedBox(&CircularArray[0], &Area[0]);
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0004", __FILE__, __FUNCSIG__); return false; }
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025b", __FILE__, __FUNCSIG__); return false; }
}

bool HelperClass::DoCurrentFrameGrab_SurfaceOrProfile(FramegrabBase* currentAction, bool SurfaceFG, bool SetImage)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "DoCurrentFrameGrab_SurfaceOrProfile", __FUNCSIG__);
	int ZeroPtTrialNumber = 0;
	try
	{
	StartFG:
		ZeroPtTrialNumber++;
		if(SetImage)
		{
			bool CheckImageGetFlag = true;
			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
					MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
					MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
				{
					CheckImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection(currentAction->ChannelNo);		
				}
			}
			else
			{
				//if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
				//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
				//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
				//{				
				//	CheckImageGetFlag = MAINDllOBJECT->SetZoomedImageInEdgeDetection();					
				//}
				//else
				//{
					CheckImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection(currentAction->ChannelNo);
				//}
			}
			if(!CheckImageGetFlag)
			{
				MAINDllOBJECT->SetStatusCode("HelperClass01");
				MAINDllOBJECT->SetAndRaiseErrorMessage("FGBGrab Error", "FrameGrab Get Image Error: Failed to get Image From camera!", __FUNCSIG__);
				return false;
			}
			/*else
				if (SurfaceFG)
					MessageBox(NULL, L"Got Image for Surface Edge Detection", L"Rapid-I", MB_TOPMOST);*/

		}
		RapidEnums::RAPIDFGACTIONTYPE CFGtype = currentAction->FGtype;
		int CFGdirection = currentAction->FGdirection;
		double CFGWidth = currentAction->FGWidth;

		//if (PPCALCOBJECT->IsPartProgramRunning())
		//{
		//	if (currentAction->CG_Params[0] != -1 && currentAction->CG_Params[1] != -1 && currentAction->CG_Params[2] != -1)
		//	{
		//		//We got align the current image to the one stored in the PP. 
		//		double thisCG_Params[4] = { -1 };
		//		MAINDllOBJECT->Calculate_CurrentFrameDifference(thisCG_Params, 16, PPCALCOBJECT->OneShotImgBinariseVal(), SurfaceFG);
		//		if (thisCG_Params[0] != -1 && thisCG_Params[1] != -1 && thisCG_Params[2] != -1)
		//		{
		//			//We got a match. Now let us shift the frames and rotate them according to the differences...
		//			double diffPix[2] = { currentAction->CG_Params[1] - thisCG_Params[1], currentAction->CG_Params[2] - thisCG_Params[2] };
		//			double diff[2] = { diffPix[0] * MAINDllOBJECT->getWindow(0).getUppX() / this->CamSizeRatio, diffPix[1] * MAINDllOBJECT->getWindow(0).getUppX() / this->CamSizeRatio };
		//			for (int k = 0; k < 3; k++)
		//			{
		//				currentAction->points[k].set(diff[0] + currentAction->points[k].getX(), diff[1] + currentAction->points[k].getY(), currentAction->points[k].getZ());
		//				currentAction->myPosition[k].set(diffPix[0] + currentAction->myPosition[k].getX(), diffPix[1] + currentAction->myPosition[k].getY());
		//			}
		//			//currentAction->points[0].set(diff[0] + currentAction->points[0].getX(), diff[1] + currentAction->points[0].getY(), currentAction->points[0].getZ());
		//			//currentAction->points[1].set(diff[0] + currentAction->points[1].getX(), diff[1] + currentAction->points[1].getY(), currentAction->points[1].getZ());
		//			//currentAction->points[2].set(diff[0] + currentAction->points[2].getX(), diff[1] + currentAction->points[2].getY(), currentAction->points[2].getZ());
		//			//currentAction->myPosition[0].set(diffPix[0] + currentAction->myPosition[0].getX(), diffPix[1] + currentAction->myPosition[0].getY());
		//			//currentAction->myPosition[1].set(diffPix[0] + currentAction->myPosition[1].getX(), diffPix[1] + currentAction->myPosition[1].getY());
		//			//currentAction->myPosition[2].set(diffPix[0] + currentAction->myPosition[2].getX(), diffPix[1] + currentAction->myPosition[2].getY());
		//			double ImageRAngle = currentAction->CG_Params[3] - thisCG_Params[3];
		//			if (ImageRAngle < 0) ImageRAngle += 2 * M_PI;
		//			ImageRAngle = 2 * M_PI - ImageRAngle;
		//			//Transformation Matrix..
		//			double TransformM[9] = { 0 };
		//			//RMATH2DOBJECT->TransformM_RotateAboutPoint(diff, ImageRAngle, &TransformM[0]);
		//			TransformM[2] = 0; TransformM[5] = 0;
		//			double	gP[3] = { 0,0, 0 },
		//				cP[3] = { MAINDllOBJECT->getWindow(0).getWinDim().x * CamSizeRatio / 2, MAINDllOBJECT->getWindow(0).getWinDim().y * CamSizeRatio / 2 , 0 },
		//				ans[3] = { 0,0,0 };
		//
		//			for (int i = 0; i < 3; i++)
		//			{
		//				gP[0] = currentAction->myPosition[i].getX(); gP[1] = currentAction->myPosition[i].getY(); gP[2] = 0; // currentAction->points[i].getZ();
		//				//cP[0] = icentre.x * HELPEROBJECT->CamSizeRatio / 2; cP[1] = icentre.y * HELPEROBJECT->CamSizeRatio / 2; cP[2] = 0;
		//				//cP[0] = 0;// MAINDllOBJECT->getCurrentUCS->UCSPt_B2S.getX();
		//				//cP[1] = 0;// MAINDllOBJECT->getCurrentUCS->UCSPt_B2S.getY(); cP[2] = 0;
		//
		//				RMATH2DOBJECT->Rotate_2D_Pt(gP, cP, ImageRAngle, ans);
		//				currentAction->myPosition[i].set(ans[0], ans[1], 0);
		//				//Vector Temp = MAINDllOBJECT->TransformMultiply(TransformM, ans[0], ans[1], currentAction->points[i].getZ());
		//
		//				//currentAction->points[i].set(Temp);
		//
		//				//currentAction->myPosition[i].set((currentAction->points[i].getX() - cx) / upp + icentre.x / 2, icentre.y / 2 - (currentAction->points[i].getY() - cy) / upp);
		//			}
		//		}
		//	}
		//}
		//else
		//{
		//	if (MAINDllOBJECT->Align_by_CG())
		//	{
		//		MAINDllOBJECT->Calculate_CurrentFrameDifference(currentAction->CG_Params, 16, PPCALCOBJECT->OneShotImgBinariseVal(), SurfaceFG);
		//	}
		//}

		double p1[2] = { currentAction->myPosition[0].getX(), currentAction->myPosition[0].getY() };
		double p2[2] = { currentAction->myPosition[1].getX(), currentAction->myPosition[1].getY() };
		double p3[2] = { currentAction->myPosition[2].getX(), currentAction->myPosition[2].getY() };
		int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y;

		//if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT)) // && MAINDllOBJECT->DigitalZoomModeForOneShot())
		//{
		//	for(int i = 0; i < 2; i++)
		//	{
		//		p1[i] = p1[i] * this->CamSizeRatio;
		//		p2[i] = p2[i] * this->CamSizeRatio;
		//		p3[i] = p3[i] * this->CamSizeRatio;
		//	}
		//	wwidth = wwidth * this->CamSizeRatio; 
		//	wheight = wheight * this->CamSizeRatio;
		//	CFGWidth *= this->CamSizeRatio;
		//}

		int RectFGArray[5]; 
		double CircularArray[6];
		//According to the type calculate the points(grab the points: edge)
		if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_CIRCLEFRAMEGRAB)
		{
			RMATH2DOBJECT->CircleParameters_FG(&p1[0], &p2[0], CFGWidth, &CircularArray[0]);
			//Scale the FG box to get to correct location in actual image which will be larger
			//for (int i = 0; i < 4; i ++) CircularArray[i] *= CamSizeRatio;
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ARCFRAMEGRAB)
		{
			p1[1] = wheight - p1[1]; p2[1] = wheight - p2[1]; p3[1] = wheight - p3[1];
			RMATH2DOBJECT->ArcParameters_FG(&p1[0], &p2[0], &p3[0], CFGWidth, &CircularArray[0]);
			CircularArray[1] = wheight - CircularArray[1];
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE
			 || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_RECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATIONSCAN_FIXEDRECTANGLE)
		{
			RectFGArray[0] = min(p1[0], p2[0]); RectFGArray[1] = min(p1[1], p2[1]);
			RectFGArray[2] = max(p1[0], p2[0]) - RectFGArray[0];
			RectFGArray[3] = max(p1[1], p2[1]) - RectFGArray[1];
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ANGULARRECTANGLEFRAMEGRAB)
		{
			RectFGArray[0] = p1[0]; RectFGArray[1] = p1[1];
			RectFGArray[2] = p2[0]; RectFGArray[3] = p2[1];
			RectFGArray[4] = CFGWidth * 2;
		}
		try
		{
			if(SurfaceFG)
			{
				if (!PPCALCOBJECT->IsPartProgramRunning()) // && currentAction->Surfacre_FrameGrabtype == EdgeDetectionSingleChannel::ComponentType::PeakBased)
				{
					currentAction->MaskFactor = EDSCOBJECT->Type6_MaskFactor; currentAction->Binary_LowerFactor = EDSCOBJECT->Type6_Binary_LowerFactor; 
					currentAction->Binary_UpperFactor = EDSCOBJECT->Type6_Binary_UpperFactor;
					currentAction->PixelThreshold = EDSCOBJECT->Type6_PixelThreshold; currentAction->MaskingLevel = EDSCOBJECT->Type6_MaskingLevel;
					currentAction->DoAverageFiltering = EDSCOBJECT->DoAverageFiltering;
					//MessageBox(NULL, L"Set Default settings", L"Rapid-I", MB_TOPMOST);
				}
				else
				{
					EDSCOBJECT->Type6_MaskFactor = currentAction->MaskFactor; EDSCOBJECT->Type6_Binary_LowerFactor = currentAction->Binary_LowerFactor;
					EDSCOBJECT->Type6_Binary_UpperFactor = currentAction->Binary_UpperFactor;
					EDSCOBJECT->Type6_PixelThreshold = currentAction->PixelThreshold; EDSCOBJECT->Type6_MaskingLevel = currentAction->MaskingLevel;
					EDSCOBJECT->DoAverageFiltering = currentAction->DoAverageFiltering;
				}
				if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
					currentAction->noofpts = EDSCOBJECT->DetectSurfaceEdgeAngularRect(&RectFGArray[0], (bool)CFGdirection, !SetImage, EdgeDetectionSingleChannel::ComponentType(currentAction->Surfacre_FrameGrabtype));
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES)
					currentAction->noofpts = EDSCOBJECT->DetectAllProfileEdges(&RectFGArray[0]);
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB)
				{
					//if(PPCALCOBJECT->IsPartProgramRunning())
					//{
						if(currentAction->Surfacre_FrameGrabtype == EdgeDetectionSingleChannel::ComponentType::PeakBased)
							currentAction->noofpts = EDSCOBJECT->DetectSurfaceEdge_Peak(&RectFGArray[0], currentAction->MaskFactor, currentAction->Binary_LowerFactor, currentAction->Binary_UpperFactor, currentAction->PixelThreshold, currentAction->MaskingLevel, !SetImage); 					
						else
							currentAction->noofpts = EDSCOBJECT->DetectSurfaceEdge(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection(CFGdirection), EdgeDetectionSingleChannel::ComponentType(currentAction->Surfacre_FrameGrabtype));		
					//}
					//else
					//	currentAction->noofpts = EDSCOBJECT->DetectSurfaceEdge(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection(CFGdirection), EdgeDetectionSingleChannel::ComponentType(currentAction->Surfacre_FrameGrabtype));
				}
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB)
					currentAction->noofpts = EDSCOBJECT->DetectSurfaceEdgeCurvedBox(&CircularArray[0], CFGdirection, !SetImage, EdgeDetectionSingleChannel::ComponentType(currentAction->Surfacre_FrameGrabtype));
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK)
					currentAction->noofpts = EDSCOBJECT->DetectEdgeMouseClick(p1[0], p1[1]);
			}
			else
			{
				if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ANGULARRECTANGLEFRAMEGRAB)
				{
					//if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
					//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
					//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)) // || CurrentCamera() == 3)) // && MAINDllOBJECT->DigitalZoomModeForOneShot())
					//	currentAction->noofpts = EDSCOBJECT->DetectEdgeLowFocusAngularRect(&RectFGArray[0], CFGdirection);
					//else
						currentAction->noofpts = EDSCOBJECT->DetectEdgeAngularRect(&RectFGArray[0], (bool)CFGdirection);
				}
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES)
						currentAction->noofpts = EDSCOBJECT->DetectAllProfileEdges(&RectFGArray[0]);
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_RECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATIONSCAN_FIXEDRECTANGLE)
				{
					//if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
					//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
					//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM) // || CurrentCamera() == 3) // && MAINDllOBJECT->DigitalZoomModeForOneShot())
					//	currentAction->noofpts = EDSCOBJECT->DetectEdgeLowFocus(&RectFGArray[0], CFGdirection);
					//else
						currentAction->noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection(CFGdirection));
				}
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ARCFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ROTATION_CIRCLEFRAMEGRAB)
				{
					//if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
					//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
					//	MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)// || CurrentCamera() == 3) // && MAINDllOBJECT->DigitalZoomModeForOneShot())
					//	currentAction->noofpts = EDSCOBJECT->DetectEdgeLowFocusCurvedBox(&CircularArray[0], CFGdirection);
					//else
						currentAction->noofpts = EDSCOBJECT->DetectEdgeCurvedBox(&CircularArray[0], CFGdirection);
				}
				else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK)
					currentAction->noofpts = EDSCOBJECT->DetectEdgeMouseClick(p1[0], p1[1]);
			}
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0004", __FILE__, __FUNCSIG__); return false; }

		//If we are running on 64-bit windows, and we do not get 0 points, there is some connection problem. Lets fix it!
		//if (Is64BitWindows())
		//{
		if (CurrentCamera() == 3 && currentAction->noofpts == 0)
		{
			//double whiteAreaSize = 0, totalAreaSize = 0;
			//int fgbox[4] = { 0, 0, MAINDllOBJECT->currCamWidth, MAINDllOBJECT->currCamHeight }; 
			//whiteAreaSize = EDSCOBJECT->CalculateWhiteAreaStraightRect(fgbox, &totalAreaSize);
			if (EDSCOBJECT->CheckForBlankImage(3))
			//if (whiteAreaSize > 100 && (whiteAreaSize / totalAreaSize) < 0.2)
			{			//Let us reset video and get things in action again!
				MAINDllOBJECT->cameraFormatDialog();
				Sleep(100);
				//We try a max of 3 times! if not, we write an error message and quit trying and return failed
				if (ZeroPtTrialNumber < 3)
					goto StartFG;
				else
				{
					MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025_FG_Fail", __FILE__, __FUNCSIG__);
					return false;
				}
			}
		}
		//		}
		//		//First, let us try to clear memory and try to do edge detection again. 
		//	}
		//}
		//Converting to DRO value.
		if(!HELPEROBJECT->RotationScanFlag)
			AddFrameGabbedPoints(currentAction->noofpts, currentAction->AllPointsList);

		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT)
		{
			if (!MAINDllOBJECT->SigmaModeFlag()) MAINDllOBJECT->SigmaModeFlag(true);
			if (ZeroPtTrialNumber < MAINDllOBJECT->RepeatCount())
			{
				Sleep(100);
				goto StartFG;
			}
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::DoCurrentFrameGrab_IdOdOrFull(FramegrabBase* currentAction)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "DoCurrentFrameGrab_IdOdOrFull", __FUNCSIG__);
	try
	{
		bool CheckImageGetFlag = true;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
				MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
				MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
				CheckImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection();
		}
		else
		{
			if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
				CheckImageGetFlag = MAINDllOBJECT->SetZoomedImageInEdgeDetection();
			else
				CheckImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection();
		}
		if(!CheckImageGetFlag)
		{
			MAINDllOBJECT->SetStatusCode("HelperClass01");
			MAINDllOBJECT->SetAndRaiseErrorMessage("FGBGrab Error", "FrameGrab Get Image Error: Failed to get Image From camera!", __FUNCSIG__);
			return false;
		}

		RapidEnums::RAPIDFGACTIONTYPE CFGtype = currentAction->FGtype;
		int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y;

		RapidEnums::RAPIDPROJECTIONTYPE Cptype = RapidEnums::RAPIDPROJECTIONTYPE(MAINDllOBJECT->getCurrentUCS().UCSProjectionType());
		Vector Cdro = MAINDllOBJECT->getCurrentDRO();
		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM) && MAINDllOBJECT->DigitalZoomModeForOneShot())
		{
			wwidth = wwidth * 2;
			wheight = wheight * 2;
		}
		int RectFGArray[4] = {30, 30, wwidth - 60, wheight - 60};
		try
		{
			if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ODMEASUREMENT)
				currentAction->noofpts = EDSCOBJECT->DetectIDOD(&RectFGArray[0], false);
			else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::IDMEASUREMENT)
				currentAction->noofpts = EDSCOBJECT->DetectIDOD(&RectFGArray[0], true);
			else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::DETECT_ALLEDGES)
				currentAction->noofpts = EDSCOBJECT->DetectAllProfileEdges(&RectFGArray[0]);
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0004", __FILE__, __FUNCSIG__); return false; }
	
		//Converting to DRO value.
		if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::DETECT_ALLEDGES)
			AddAllEdgePointCollection(currentAction->noofpts, currentAction->AllPointsList);
		else
			AddFrameGabbedPoints(currentAction->noofpts, currentAction->AllPointsList);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); return false;}
}

void HelperClass::AddFrameGabbedPoints(int NoOfPoints, PointCollection& CFgPointsList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddFrameGabbedPoints", __FUNCSIG__);
	try
	{
		//double cx = MAINDllOBJECT->getUCS(0)->GetCurrentUCS_DROpostn().getX(), cy = MAINDllOBJECT->getUCS(0)->GetCurrentUCS_DROpostn().getY(); // 
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX() / this->CamSizeRatio, upp_y = MAINDllOBJECT->getWindow(0).getUppY() / this->CamSizeRatio;
		double HalfScreenX = MAINDllOBJECT->getWindow(0).getWinDim().x * this->CamSizeRatio / 2 * upp_x, HalfScreenY = MAINDllOBJECT->getWindow(0).getWinDim().y * this->CamSizeRatio / 2 * upp_y;
		RapidEnums::RAPIDPROJECTIONTYPE Cptype = RapidEnums::RAPIDPROJECTIONTYPE(MAINDllOBJECT->getCurrentUCS().UCSProjectionType());
		Vector Cdro = MAINDllOBJECT->getCurrentDRO();

		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			double Cp_x, Cp_y;
			int Off_x = 0, Off_y = 0;
			if(MAINDllOBJECT->DigitalZoomModeForOneShot())

			{
				upp_x = upp_x /2;
				upp_y = upp_y /2;
				Off_x = MAINDllOBJECT->PixelOffset_X();
				Off_y = MAINDllOBJECT->PixelOffset_Y();
			}
			double Pwx = 0.0, Pwy = 0.0, RadialDistance = 0.0, quadCorr = 0.0;
			for(int n = 0; n < NoOfPoints; n++)
			{
				//if(EDSCOBJECT->DetectedPointsList[n * 2] == -1) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2] < 0) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2 + 1] < 0) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2] > MAINDllOBJECT->currCamWidth) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2 + 1] > MAINDllOBJECT->currCamHeight) continue;

				Cp_x = EDSCOBJECT->DetectedPointsList[n * 2] + Off_x; Cp_y = EDSCOBJECT->DetectedPointsList[n * 2 + 1] + Off_y;
				R_Point CPt;
				if(!CALBCALCOBJECT->CalibrationModeFlag())
				{
					RadialDistance = RMATH2DOBJECT->Pt2Pt_distance(MAINDllOBJECT->PixelWidthX0(), MAINDllOBJECT->PixelWidthY0(), Cp_x, Cp_y);
					if (RadialDistance > MAINDllOBJECT->getWindow(0).getWinDim().x * 8) continue;
					if(MAINDllOBJECT->PixelWidthCorrectionForOneShot() && RadialDistance > MAINDllOBJECT->PW_C0_Rad())
					{
						//((MAINDllOBJECT->getWindow(0).getWinDim().x / 2)+(MAINDllOBJECT->PixelWidthX0())), ((MAINDllOBJECT->getWindow(0).getWinDim().y / 2)+(MAINDllOBJECT->PixelWidthY0())), Cp_x, Cp_y);	//vinod..............
						quadCorr = MAINDllOBJECT->PixelWidthCorrectionQuadraticTerm() * RadialDistance * RadialDistance;
						//double linearCorrX = RadialDistance * //(MAINDllOBJECT->getWindow(0).getWinDim().x / 2 + MAINDllOBJECT->PixelWidthX0()) - Cp_x;
						//double linearCorrY = (MAINDllOBJECT->getWindow(0).getWinDim().y / 2 + MAINDllOBJECT->PixelWidthY0()) - Cp_y;
						 
						Pwx = quadCorr + MAINDllOBJECT->PixelWidthCorrectionLinearTermX() * RadialDistance + upp_x;
						Pwy = quadCorr + MAINDllOBJECT->PixelWidthCorrectionLinearTermY() * RadialDistance + upp_y;
						Cp_x = cx + Cp_x * Pwx + HalfScreenX; Cp_y = cy - Cp_y * Pwy + HalfScreenY;
						if (MAINDllOBJECT->UseLocalisedCorrection) BESTFITOBJECT->correctXYCoord2(&Cp_x, &Cp_y);
						CPt = R_Point(Cp_x - HalfScreenX, Cp_y - HalfScreenY);
					}
					else
					{
						//Cp_x = cx + Cp_x * upp_x + HalfScreenX; Cp_y = cy - Cp_y * upp_y + HalfScreenY;
						//BESTFITOBJECT->correctXYCoord2(&Cp_x, &Cp_y);
						//CPt = R_Point(Cp_x - HalfScreenX, Cp_y - HalfScreenY);
						CPt = R_Point(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y);
					}
				}
				else
					CPt = MAINDllOBJECT->getWindow(0).PixelCalibration_OneShot(Cp_x, Cp_y, MAINDllOBJECT->DigitalZoomModeForOneShot());
				if (CPt.x != CPt.x || CPt.y != CPt.y) continue;
				CFgPointsList.Addpoint(new SinglePoint(CPt.x, CPt.y, Cdro.getZ()));
			}			
		}
		else if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::TIS)
		{
			for (int n = 0; n < NoOfPoints; n++)
			{
				//if (EDSCOBJECT->DetectedPointsList[n * 2] == -1) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2] < 0) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2 + 1] < 0) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2] > MAINDllOBJECT->currCamWidth) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2 + 1] > MAINDllOBJECT->currCamHeight) continue;

				if (MAINDllOBJECT->AAxisHorizontal)
					CFgPointsList.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, Cdro.getY(), cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, Cdro.getR()));
				else
					CFgPointsList.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, Cdro.getZ(), Cdro.getR()));
			}

			//double matix1[9] = {0};
			//int s1[2] = {3,3}, s2[2] = {3,1};
			//RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(-(Cdro.getR() * M_PI), &MAINDllOBJECT->TIS_CAxis[3], matix1);
			//for(int n = 0; n < NoOfPoints; n++)
			//{
			//	if(EDSCOBJECT->DetectedPointsList[n * 2] == -1) continue;
			//	double pnt[3] = {cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, Cdro.getZ()}, finalPnt[3] = {0};
			//	RMATH3DOBJECT->MultiplyMatrix1(matix1, s1, pnt, s2, finalPnt);
			//	CFgPointsList.Addpoint(new SinglePoint(finalPnt[0], finalPnt[2], finalPnt[1]));
			//}
		}
		else
		{
			for (int n = 0; n < NoOfPoints; n++)
			{
				if (EDSCOBJECT->DetectedPointsList[n * 2] < 0) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2 + 1] < 0) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2] > MAINDllOBJECT->currCamWidth) continue;
				if (EDSCOBJECT->DetectedPointsList[n * 2 + 1] > MAINDllOBJECT->currCamHeight) continue;

				if (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3)
				{
					int Order1[2] = { 3, 3 }, Order2[2] = { 3, 1 };
					//for (PC_ITER Item = CFgPointsList.getList().begin(); Item != CFgPointsList.getList().end(); Item++)
					//{
					//	SinglePoint* Spt = Item->second;
					//	double temp1[3], temp2[3] = { Spt->X, Spt->Y, Spt->Z };
					//	RMATH2DOBJECT->MultiplyMatrix1(&MAINDllOBJECT->getCurrentUCS().transform[0], Order1, temp2, Order2, temp1);
					//	Spt->SetValues(temp1[0], temp1[1], temp1[2]);
					//}
					R_Point tempP = MAINDllOBJECT->getCurrentUCS().getWindow(0).CalculateTransformedCoordinates(EDSCOBJECT->DetectedPointsList[n * 2] / this->CamSizeRatio, EDSCOBJECT->DetectedPointsList[n * 2 + 1] / this->CamSizeRatio);
					CFgPointsList.Addpoint(new SinglePoint(tempP.x, tempP.y, MAINDllOBJECT->getCurrentUCS().GetCurrentUCS_DROpostn().getZ(), MAINDllOBJECT->getCurrentUCS().GetCurrentUCS_DROpostn().getR()));
				}
				else
					CFgPointsList.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, Cdro.getZ()));
			}
		}
	/*	else if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 0)
		{
			for(PC_ITER Item = CFgPointsList.getList().begin(); Item != CFgPointsList.getList().end(); Item++)
			{
				SinglePoint* Sp = (*Item).second;
				double temp[3] = {Sp->X - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), Sp->Y - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), Sp->Z - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ()};
				Sp->SetValues(temp[0], temp[1], temp[2]);
			}
		}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); }
}

void HelperClass::AddAllEdgePointCollection(int NoOfPoints, PointCollection& CFgPointsList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddAllEdgePointCollection", __FUNCSIG__);
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX(), upp_y = MAINDllOBJECT->getWindow(0).getUppY();
		list<PointCollection*> TemppointCollection;
		PointCollection PtColl;
		for(int n = 0; n < NoOfPoints; n++)
		{
			CFgPointsList.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, 0.0));
			if(EDSCOBJECT->DetectedPointsList[n * 2] == -1)
			{
				PointCollection* TempPtColl = new PointCollection;
				for(PC_ITER Sitem = PtColl.getList().begin(); Sitem != PtColl.getList().end(); Sitem++)
					TempPtColl->Addpoint((*Sitem).second);
				PtColl.EraseAll();
				TemppointCollection.push_back(TempPtColl);
				continue;
			}
			PtColl.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, 0.0));
		}

		for each(PointCollection *Ptcollns in TemppointCollection)
		{
			/*if(Ptcollns->Pointscount() < 5)
			{
				Ptcollns->deleteAll();
				continue;
			}*/
			Line* Nshape = new Line();
			Nshape->AddPoints(Ptcollns);
			AddShapeAction::addShape(Nshape);
			Ptcollns->deleteAll();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); }
}

void  HelperClass::getPointParam(Vector* CPoint, list<list<double>>* ShpParmPntr)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getPointParam", __FUNCSIG__);
		list<double> TempParam;
		double TempPoint[3]= { CPoint->getX(), CPoint->getY(), CPoint->getZ()};
		for(int i = 0; i < 3; i++) TempParam.push_back(TempPoint[i]);
		(*ShpParmPntr).push_back(TempParam);
}

bool HelperClass::getShapeDimension(Shape *s, list<list<double>>* ShpParmPntr)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getShapeDimension", __FUNCSIG__);
	try
	{   
		double TempLine[8] = {0}, TempPoint[4] = {0}, TempPlane[4] = {0};
		list<double> TempParam;
		bool flag = false;
		switch(s->ShapeType)
		{
			case RapidEnums::SHAPETYPE::XLINE:
			case RapidEnums::SHAPETYPE::XLINE3D:
			case RapidEnums::SHAPETYPE::XRAY:
			case RapidEnums::SHAPETYPE::LINE:
			case RapidEnums::SHAPETYPE::LINE3D:
				((Line*)s)->getParameters(TempLine);
				for(int i = 0; i < 6; i++) TempParam.push_back(TempLine[i]);
				flag = true;
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
			case RapidEnums::SHAPETYPE::CIRCLE:
				TempParam.push_back(((Circle*)s)->getCenter()->getX());
				TempParam.push_back(((Circle*)s)->getCenter()->getY());
				TempParam.push_back(((Circle*)s)->getCenter()->getZ());
				flag = true;
				break;
			case RapidEnums::SHAPETYPE::CYLINDER:
				((Cylinder*)s)->getParameters(TempLine);
				for(int i = 0; i < 6; i++) TempParam.push_back(TempLine[i]);
				flag = true;
				break;
			case RapidEnums::SHAPETYPE::CONE:
				((Cone*)s)->getParameters(TempLine);
				for(int i = 0; i < 6; i++) TempParam.push_back(TempLine[i]);
				flag = true;
				break;
			case RapidEnums::SHAPETYPE::PLANE:
				((Plane*)s)->getParameters(TempPlane);
				for(int i = 0; i < 4; i++) TempParam.push_back(TempPlane[i]);
				flag = true;
				break;
			case RapidEnums::SHAPETYPE::SPHERE:
				((Sphere*)s)->getParameters(TempPoint);
				for(int i = 0; i < 3; i++) TempParam.push_back(TempPoint[i]);
				flag = true;
				break;
			case RapidEnums::SHAPETYPE::RPOINT3D:
			case RapidEnums::SHAPETYPE::RPOINT:
				((RPoint*)s)->getPosition()->FillDoublePointer(&TempPoint[0], 3);
				for(int i = 0; i < 3; i++) TempParam.push_back(TempPoint[i]);
				flag = true;
				break;
		}
		if (flag)
		{
			(*ShpParmPntr).push_back(TempParam);
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0003", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::Co_OrdinateSystem_From_ThreeShape(list<list<double>> ShapesParam, double* CurrentPlane, double* CurrentLine, double* CurrentPoint)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "Co_OrdinateSystem_From_ThreeShape", __FUNCSIG__);
	try
	{  
		int SelctNum[3] = {0};
		list<list<double>>::iterator FrstSelectn, TempSelectn;
		if (ShapesParam.size() > 2)
		{
			TempSelectn = FrstSelectn = ShapesParam.begin();
			for(int i = 0; i < 3; i++, TempSelectn++) 
			{
				switch(TempSelectn->size())
				{
					case 3:
						SelctNum[0]++;
						break;
					case 6:
						SelctNum[1]++;
						break;
					case 4:
						SelctNum[2]++;
						break;
				}
			}
		}
		else
			return false;

		if (SelctNum[0] == 0 && SelctNum[1] == 0 && SelctNum[2] == 3)
		{      
			double Pln2[4] = {0}, Pln3[4] = {0};
			list<double>::iterator It1 = FrstSelectn->begin();
			for (int i = 0; i < 4; i++, It1++) CurrentPlane[i] = *It1;
			list<double>::iterator It2 = (++FrstSelectn)->begin();
			for (int i = 0; i < 4; i++, It2++) Pln2[i] = *It2;
			list<double>::iterator It3 = (++FrstSelectn)->begin();
			for (int i = 0; i < 4; i++, It3++) Pln3[i] = *It3;
				
			if(RMATH3DOBJECT->Intersection_Plane_Plane(CurrentPlane, Pln2, CurrentLine) == 2)
			{
				if(RMATH3DOBJECT->Intersection_Line_Plane(CurrentLine, Pln3, CurrentPoint) == 1)
				   return true;
			}
			return false;
		}
		else if (SelctNum[0] == 0 && SelctNum[1] == 1 && SelctNum[2] == 2) 
		{
			double Pln2[4] = {0}, TempLine[6] = {0}; 
			double* temp = CurrentPlane;
			for (int i = 0; i < 3; i++, FrstSelectn++)
			{
				if(FrstSelectn->size() == 4)
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 4; i++,p++) temp[i] = *p;
					temp = Pln2;
				}
				else
				{
				list<double>::iterator p = FrstSelectn->begin();
				for (int i = 0; i < 6; i++, p++) TempLine[i] = *p;
				}
			}
			if(RMATH3DOBJECT->Intersection_Plane_Plane(CurrentPlane, Pln2, CurrentLine) == 2)
			{
				if(RMATH3DOBJECT->Intersection_Line_Line(TempLine, CurrentLine, CurrentPoint))
					 return true;
				else if(RMATH3DOBJECT->Intersection_Line_Plane(TempLine, CurrentPlane, CurrentPoint))
					 return true;
				else if(RMATH3DOBJECT->Intersection_Line_Plane(TempLine, Pln2, CurrentPoint))
					 return true;
			}
			return false;
		}
		
		else if (SelctNum[0] == 0 && SelctNum[1] == 2 && SelctNum[2] == 1) 
		{
			double TempLine1[6] = {0}, TempLine2[6] = {0}, ProjctnLn[6] = {0}; 
			double* temp = TempLine1;
			for (int i = 0; i < 3; i++, FrstSelectn++)
			{
				if(FrstSelectn->size() == 6)
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 6; i++, p++) temp[i] = *p;
					temp = TempLine2;
				}
				else
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 4; i++,p++) CurrentPlane[i] = *p;
				}
			}
			if(RMATH3DOBJECT->Projection_Line_on_Plane(TempLine1, CurrentPlane, CurrentLine))
			{
				if(RMATH3DOBJECT->Projection_Line_on_Plane(TempLine2, CurrentPlane, ProjctnLn))
				{
					if(RMATH3DOBJECT->Intersection_Line_Line(CurrentLine, ProjctnLn, CurrentPoint))
						return true;
				}
			}
			return false;
		}
		
		else if (SelctNum[0] == 0 && SelctNum[1] == 3 && SelctNum[2] == 0) 
		{
			double TempLine2[6] = {0}, TempLine3[6] = {0};
			list<double>::iterator It1 = FrstSelectn->begin();
			for (int i = 0; i < 6; i++, It1++) CurrentLine[i] = *It1; FrstSelectn++;
			list<double>::iterator It2 = FrstSelectn->begin();
			for (int i = 0; i < 6; i++, It2++) TempLine2[i] = *It2; FrstSelectn++;
			list<double>::iterator It3 = FrstSelectn->begin();
			for (int i = 0; i < 6; i++, It3++) TempLine3[i] = *It3;
			if(RMATH3DOBJECT->Create_Plane_Line_2_Line(CurrentLine, TempLine2, CurrentPlane))
			{
				if(RMATH3DOBJECT->Intersection_Line_Plane(TempLine3, CurrentPlane, CurrentPoint))
					return true;
				else if(RMATH3DOBJECT->Intersection_Line_Line(CurrentLine, TempLine2, CurrentPoint)) 
					return true;
			}
			else if(RMATH3DOBJECT->Create_Plane_Line_2_Line(CurrentLine, TempLine3, CurrentPlane))
			{
				if(RMATH3DOBJECT->Intersection_Line_Plane(TempLine2, CurrentPlane, CurrentPoint))
					return true;
				else if(RMATH3DOBJECT->Intersection_Line_Line(CurrentLine, TempLine3, CurrentPoint)) 
					return true;
			}
			else if(RMATH3DOBJECT->Create_Plane_Line_2_Line(TempLine2, TempLine3, CurrentPlane))
			{
				if(RMATH3DOBJECT->Intersection_Line_Plane(CurrentLine, CurrentPlane, CurrentPoint))
				{
					for (int i = 0; i < 6; i++, It3++) CurrentLine[i] = TempLine2[i];
					return true;
				}
				else if(RMATH3DOBJECT->Intersection_Line_Line(TempLine2, TempLine3, CurrentPoint)) 
				{
					for (int i = 0; i < 6; i++, It3++) CurrentLine[i] = TempLine2[i];
					return true;
				}
			}
			return false;
		}

		else if (SelctNum[0] == 1 && SelctNum[1] == 2 && SelctNum[2] == 0) 
		{
			double TempLine2[6] = {0}; 
			double* temp = CurrentLine;
			for (int i = 0; i < 3; i++, FrstSelectn++)
			{
				if(FrstSelectn->size() == 6) 
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 6; i++, p++) temp[i] = *p;
					temp = TempLine2;
				}
				else
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 3; i++,p++) CurrentPoint[i] = *p;
				}
			}
			if (RMATH3DOBJECT->Create_Plane_Line_2_Line(CurrentLine, TempLine2, CurrentPlane))  
				return true;
			return false;
		}
		else if (SelctNum[0] == 1 && SelctNum[1] == 0 && SelctNum[2] == 2) 
		{
			double Pln2[4] = {0};
			double* temp = CurrentPlane;
			for (int i = 0; i < 3; i++, FrstSelectn++)
			{
				if(FrstSelectn->size() == 4) 
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 4; i++, p++) temp[i] = *p;
					temp = Pln2;
				}
				else
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 3; i++,p++) CurrentPoint[i] = *p;
				}
			}
			if(RMATH3DOBJECT->Intersection_Plane_Plane(CurrentPlane, Pln2, CurrentLine) == 2)
			   return true;
			return false;
		}
		else if (SelctNum[0] == 2 && SelctNum[1] == 1 && SelctNum[2] == 0)
		{       
			double Pnts[6] = {0}, TempLine2[6] = {0};
			int j = 0;
			for (int i = 0; i < 3; i++, FrstSelectn++)
			{
				if(FrstSelectn->size() == 3) 
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 3; i++, p++, j++) Pnts[j] = *p;
				}
				else
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 6; i++,p++) CurrentLine[i] = *p;
				}
			}
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(Pnts, &TempLine2[3]);
			for(int i = 0; i < 3; i++) TempLine2[i] = CurrentPoint[i] = Pnts[i];
			if (RMATH3DOBJECT->Create_Plane_Line_2_Line(CurrentLine, TempLine2, CurrentPlane))
				return true;
		}
		else if (SelctNum[0] == 2 && SelctNum[1] == 0 && SelctNum[2] == 1) 
		{
			double Pnts[6] = {0}, tmpLine[6] = {0};
			int j = 0;
			for (int i = 0; i < 3; i++, FrstSelectn++)
			{
				if(FrstSelectn->size() == 3) 
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 3; i++, p++, j++) Pnts[j] = *p;
				}
				else
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 4; i++,p++) CurrentPlane[i] = *p;
				}
			}
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(Pnts, &tmpLine[3]);
			for(int i = 0; i < 3; i++) {tmpLine[i] = Pnts[i];}
			RMATH3DOBJECT->Projection_Line_on_Plane(tmpLine, CurrentPlane, CurrentLine);
			RMATH3DOBJECT->Projection_Point_on_Plane(Pnts, CurrentPlane, CurrentPoint);
			return true;
		}
		else if (SelctNum[0] == 3 && SelctNum[1] == 0 && SelctNum[2] == 0) 
		{
			double TempLine2[6] = {0}, Pnts12[6] = {0}, Pnts13[6] = {0};
			list<double>::iterator It1 = FrstSelectn->begin();
			for (int i = 0; i < 3; i++, It1++) Pnts12[i] = Pnts13[i] = *It1; FrstSelectn++;
			list<double>::iterator It2 = FrstSelectn->begin();
			for (int i = 0; i < 3; i++, It2++) Pnts12[3 + i] = *It2; FrstSelectn++;
			list<double>::iterator It3 = FrstSelectn->begin();
			for (int i = 0; i < 3; i++, It3++) Pnts13[3 + i] = *It3;
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(Pnts12, &CurrentLine[3]);
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(Pnts13, &TempLine2[3]);
			for(int i = 0; i < 3; i++)
			{
				CurrentPoint[i] = CurrentLine[i] = Pnts12[i];
				TempLine2[i] = Pnts13[i];
			}
			if(RMATH3DOBJECT->Create_Plane_Line_2_Line(CurrentLine, TempLine2, CurrentPlane))
			   return true;
		}

		else if (SelctNum[0] == 1 && SelctNum[1] == 1 && SelctNum[2] == 1) 
		{
			double tmpPoint[3] = { 0 }, tempLine[6] = { 0 };
			for (int i = 0; i < 3; i++, FrstSelectn++)
			{
				if(FrstSelectn->size() == 3) 
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 3; i++, p++) tmpPoint[i] = *p;
				}
				else if (FrstSelectn->size() == 4)
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 4; i++,p++) CurrentPlane[i] = *p;
				}
				else 
				{
					list<double>::iterator p = FrstSelectn->begin();
					for (int i = 0; i < 6; i++,p++) tempLine[i] = *p;
				}
			}
			RMATH3DOBJECT->Projection_Point_on_Plane(tmpPoint, CurrentPlane, CurrentPoint);
			//Modified and added by R A N 31 Mar 2015. Std Base Plane Alignment from Pt Line and Plane was NOT projecting the chosen line onto the plane. 
			RMATH3DOBJECT->Projection_Line_on_Plane(tempLine, CurrentPlane, CurrentLine);
			return true;
		}
		return false;
	}
	catch(...){MAINDllOBJECT->AddingUCSModeFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0009", __FILE__, __FUNCSIG__); }
}

bool HelperClass::UCSBasePlaneTransformM(list<list<double>>* ShapesParam, double* TransformMatrix, bool CheckForAxisFlip)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "UCSBasePlaneTransformM", __FUNCSIG__);
	try
	{
		double CurrentPlane[4] = {0}, CurrentLine[6] = {0}, CurrentPoint[3] = {0};
		double Baseplane[4] = {0, 0, 1, 0}, Baseline[6] = {0, 0, 0, 1, 0, 0}, BasePoint[3] = {0, 0, 0}, BaseOrigin[3], OtherOrigin[3] = {0}, TempMatrix[16] = {0};	
		if(Co_OrdinateSystem_From_ThreeShape(*ShapesParam, CurrentPlane, CurrentLine, CurrentPoint))
		{
			double transmatrix[16] = {0};
			for (int i = 0; i < 3; i++) 
			{
				transmatrix[4*i + i] = 1;
				transmatrix[4*i + 3] = -CurrentPoint[i];
			}
			transmatrix[15] = 1; 
			int s1[2] = {4,4}, s2[2] = {4,4};
			if(!MAINDllOBJECT->UCSSelectedLineDirAlongX() && CheckForAxisFlip)
			{
		/*		if(Baseline[3] * CurrentLine[3] + Baseline[4] * CurrentLine[4] + Baseline[5] * CurrentLine[5] < 0)
				{
					for(int i = 0; i < 3; i ++) {CurrentLine[3 + i] = -1 * CurrentLine[3 + i];}
				}
			}
			else
			{*/
				double negX[3] = {-1, 0 , 0}, tempX[3] = {0};
			/*	if(negX[0] * CurrentLine[3] + negX[1] * CurrentLine[4] + negX[2] * CurrentLine[5] < 0)
				{
					for(int i = 0; i < 3; i ++) {CurrentLine[3 + i] = -1 * CurrentLine[3 + i];}
				}*/
				RMATH3DOBJECT->CrossProductOf2Vectors(&CurrentLine[3], CurrentPlane, tempX);
				for(int i = 0; i < 3; i++)
				{ 
					CurrentLine[i] = CurrentPoint[i];
					CurrentLine[3 + i] = tempX[i];
				}
			}
			RMATH3DOBJECT->TransformationMatrix_BasePlaneAllignment(CurrentPlane, CurrentLine, CurrentPoint, Baseplane, Baseline, BasePoint, TempMatrix, BaseOrigin, OtherOrigin, true);
			RMATH2DOBJECT->MultiplyMatrix1(TempMatrix, s1, transmatrix, s2, TransformMatrix);
			return true;
		}
		else
			return false;
	}
	catch(...){MAINDllOBJECT->AddingUCSModeFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPLANEMH0010", __FILE__, __FUNCSIG__); }
}

void HelperClass::CalculateEndPoints_3DLineBestfit(int PointsCount, double* Points, double* ThreeDLine, double* EndPt1, double* EndPt2, double* Minr, double* Maxr)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CalculateEndPoints_3DLineBestfit", __FUNCSIG__);
	try
	{
		double endp1[3] = {0}, endp2[3] = {0}, ProjectionP[3], minr, maxr;
		if(ThreeDLine[3] != 0)
		{
			for(int i = 0; i < PointsCount; i++)
			{
				double r;
				RMATH3DOBJECT->Projection_Point_on_Line(Points + 3 * i, &ThreeDLine[0], &ProjectionP[0]);
				r = (ProjectionP[0] - ThreeDLine[0])/ThreeDLine[3];
				if(i == 0)
				{
					minr = r; maxr = r;
					endp1[0] = ProjectionP[0]; endp1[1] = ProjectionP[1]; endp1[2] = ProjectionP[2];
					endp2[0] = ProjectionP[0]; endp2[1] = ProjectionP[1]; endp2[2] = ProjectionP[2];
				}
				else
				{
					if(minr > r)
					{
						minr = r;
						endp1[0] = ProjectionP[0]; endp1[1] = ProjectionP[1]; endp1[2] = ProjectionP[2];
						
					}
					if(maxr < r)
					{
						maxr = r;
						endp2[0] = ProjectionP[0]; endp2[1] = ProjectionP[1]; endp2[2] = ProjectionP[2];
					}
				}
			}
		}
		else if(ThreeDLine[4] != 0)
		{
			for(int i = 0; i < PointsCount; i++)
			{
				double r;
				RMATH3DOBJECT->Projection_Point_on_Line(Points + 3 * i, &ThreeDLine[0], &ProjectionP[0]);
				r = (ProjectionP[1] - ThreeDLine[1])/ThreeDLine[4];
				if(i == 0)
				{
					minr = r; maxr = r;
					endp1[0] = ProjectionP[0]; endp1[1] = ProjectionP[1]; endp1[2] = ProjectionP[2];
					endp2[0] = ProjectionP[0]; endp2[1] = ProjectionP[1]; endp2[2] = ProjectionP[2];
				}
				else
				{
					if(minr > r)
					{
						minr = r;
						endp1[0] = ProjectionP[0]; endp1[1] = ProjectionP[1]; endp1[2] = ProjectionP[2];
						
					}
					if(maxr < r)
					{
						maxr = r;
						endp2[0] = ProjectionP[0]; endp2[1] = ProjectionP[1]; endp2[2] = ProjectionP[2];
					}
				}
			}
		}
		else
		{
			for(int i = 0; i < PointsCount; i++)
			{
				double r;
				RMATH3DOBJECT->Projection_Point_on_Line(Points + 3 * i, &ThreeDLine[0], &ProjectionP[0]);
				r = (ProjectionP[2] - ThreeDLine[2])/ThreeDLine[5];
				if(i == 0)
				{
					minr = r; maxr = r;
					endp1[0] = ProjectionP[0]; endp1[1] = ProjectionP[1]; endp1[2] = ProjectionP[2];
					endp2[0] = ProjectionP[0]; endp2[1] = ProjectionP[1]; endp2[2] = ProjectionP[2];
				}
				else
				{
					if(minr > r)
					{
						minr = r;
						endp1[0] = ProjectionP[0]; endp1[1] = ProjectionP[1]; endp1[2] = ProjectionP[2];
						
					}
					if(maxr < r)
					{
						maxr = r;
						endp2[0] = ProjectionP[0]; endp2[1] = ProjectionP[1]; endp2[2] = ProjectionP[2];
					}
				}
			}
		}
		*Minr = minr; *Maxr = maxr;
		for(int i = 0; i < 3; i++)
		{
			EndPt1[i] = endp1[i];
			EndPt2[i] = endp2[i];
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0025", __FILE__, __FUNCSIG__); }
}

//Handling the Right click on a Point..Point Coordinates, Ordinate..
DimBase* HelperClass::CreatePointDim(Vector* v1, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreatePointDim", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
				Cdim = new DimPointCoordinate(_T("Center"));
				((DimPointCoordinate*)Cdim)->CalculateMeasurement(v1);
				((DimPointCoordinate*)Cdim)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				Cdim = new DimOrdinate(_T("Pt_X"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimOrdinate*)Cdim)->CalculateMeasurement(v1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				Cdim = new DimOrdinate(_T("Pt_Y"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimOrdinate*)Cdim)->CalculateMeasurement(v1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
				Cdim = new DimOrdinate(_T("Pt_Z"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimOrdinate*)Cdim)->CalculateMeasurement(v1);
				break;
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0001", __FILE__, __FUNCSIG__); return NULL; }
}

//Handling Right Click on Line Circle/Arc..
DimBase* HelperClass::CreateRightClickDim2D(Shape* s1, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z, int Spcount)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateRightClickDim2D", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH:
				Cdim = new DimLine(_T("Dist"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH;
				((DimLine*)Cdim)->CalculateMeasurement((Line*)s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS:
				Cdim = new DimLine(_T("St"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS;
				((DimLine*)Cdim)->CalculateMeasurement((Line*)s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED:
				Cdim = new DimLine(_T("St"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED;
				((DimLine*)Cdim)->CalculateMeasurement((Line*)s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE:
				Cdim = new DimLine(_T("Ang"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE;
				((DimLine*)Cdim)->CalculateMeasurement((Line*)s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D:
				Cdim = new DimDiameter2D(_T("Dia"));
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D;
				((DimDiameter2D*)Cdim)->CalculateMeasurement(s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT:
				Cdim = new DimSplineMeasurement(_T("SpCt"), CurrentMtype);
				((DimSplineMeasurement*)Cdim)->CalculateMeasurement(s1, Spcount);
				((DimSplineMeasurement*)Cdim)->ArrangePtsCalculateSplinecount();
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMIC:
				Cdim = new DimSplineMeasurement(_T("SpMic"), CurrentMtype);
				((DimSplineMeasurement*)Cdim)->CalculateMeasurement(s1, Spcount);
				((DimSplineMeasurement*)Cdim)->ArrangePtsCalculateSplinecount();
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMCC:
				Cdim = new DimSplineMeasurement(_T("SpMcc"), CurrentMtype);
				((DimSplineMeasurement*)Cdim)->CalculateMeasurement(s1, Spcount);
				((DimSplineMeasurement*)Cdim)->ArrangePtsCalculateSplinecount();
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D:
				Cdim = new DimRadius2D(_T("Rad"));
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D;
				((DimRadius2D*)Cdim)->CalculateMeasurement(s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY:
				Cdim = new DimCircularity(_T("Circ"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY;
				((DimCircularity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC:
				Cdim = new DimCircularity(_T("Mic"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC;
				((DimCircularity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC:
				Cdim = new DimCircularity(_T("Mcc"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC;
				((DimCircularity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE:
				Cdim = new DimArcAngle(_T("ArcAng"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE;
				((DimArcAngle*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH:
				Cdim = new DimArcAngle(_T("Arclen"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH;
				((DimArcAngle*)Cdim)->CalculateMeasurement(s1);
				break;	
			case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
				Cdim = new DimPointCoordinate(_T("Center"));
				((DimPointCoordinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				((DimPointCoordinate*)Cdim)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				Cdim = new DimOrdinate(_T("Pt_X"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimOrdinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				Cdim = new DimOrdinate(_T("Pt_Y"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimOrdinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
				Cdim = new DimOrdinate(_T("Pt_Z"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimOrdinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER:
				Cdim = new DimPerimeter_Diameter(_T("P_Dia"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimPerimeter_Diameter*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_PERIMETER:
				Cdim = new DimPerimeter_Diameter(_T("P_Peri"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimPerimeter_Diameter*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_AREA:
				Cdim = new DimPerimeter_Diameter(_T("P_Area"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				((DimPerimeter_Diameter*)Cdim)->CalculateMeasurement(s1);
				break;
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0002", __FILE__, __FUNCSIG__); return NULL; }
}

//Handling the angularity of line. Concentricity of circle, TP etc..
DimBase* HelperClass::CreateAngularityDim2D(Shape* s1, Shape* s2, Shape* s3, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Angle, double land, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateAngularityDim2D", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
				Cdim = new DimAngularity(_T("Par"), CurrentMtype);
				((DimAngularity*)Cdim)->SetLandAndAngle(0, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
				Cdim = new DimAngularity(_T("Per"), CurrentMtype);
				((DimAngularity*)Cdim)->SetLandAndAngle(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
				Cdim = new DimAngularity(_T("Anglty"), CurrentMtype);
				((DimAngularity*)Cdim)->SetLandAndAngle(Angle * M_PI/180, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY_D:
				Cdim = new DimConcentricity(_T("Concen"));
				Cdim->MeasurementType = CurrentMtype;
				((DimConcentricity*)Cdim)->CalculateMeasurement(s1, s2);
				break;			
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
				Cdim = new DimTruePosition(_T("TPG"), CurrentMtype);
				((DimTruePosition*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition*)Cdim)->CalculateMeasurement(s1, s2, s3);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
				Cdim = new DimTruePosition(_T("TPX"), CurrentMtype);
				((DimTruePosition*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition*)Cdim)->CalculateMeasurement(s1, s2, s3);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
				Cdim = new DimTruePosition(_T("TPY"), CurrentMtype);
				((DimTruePosition*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition*)Cdim)->CalculateMeasurement(s1, s2, s3);
				break;
			//case RapidEnums::MEASUREMENTTYPE::DIM_PROFILE:
			//	Cdim = new DimProfile(_T("Pf"));
			//	Cdim->MeasurementType = CurrentMtype;
			//	((DimProfile*)Cdim)->CalculateMeasurement(s1, s2, s3, s4);
			//	break;

		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			AddGDnTReferenceDatum(Cdim, s2);
			if(CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R || CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D)
				AddGDnTReferenceDatum(Cdim, s3);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0003", __FILE__, __FUNCSIG__); return NULL; }
}
	
DimBase* HelperClass::CreateTruePositionDim(Vector* v1, Shape* s1, Shape* s2, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Angle, double land, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateTruePositionDim", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
				Cdim = new DimTruePositionPoint(_T("TPG"), CurrentMtype);
				((DimTruePositionPoint*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePositionPoint*)Cdim)->CalculateMeasurement(v1, s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
				Cdim = new DimTruePositionPoint(_T("TPX"), CurrentMtype);
				((DimTruePositionPoint*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePositionPoint*)Cdim)->CalculateMeasurement(v1, s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
				Cdim = new DimTruePositionPoint(_T("TPY"), CurrentMtype);
				((DimTruePositionPoint*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePositionPoint*)Cdim)->CalculateMeasurement(v1, s1, s2);
				break;
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			AddGDnTReferenceDatum(Cdim, s1);
			if(CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R || CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D)
				AddGDnTReferenceDatum(Cdim, s2);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0002", __FILE__, __FUNCSIG__); return NULL; }
}

//Distance between the points.
DimBase* HelperClass::CreatePointDim2D(Vector* v1, Vector* v2, double x, double y, double z, bool projectionDist)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreatePointDim2D", __FUNCSIG__);
	try
	{
		DimPointToPointDistance* Cdim;
		if(projectionDist)
			Cdim = new DimPointToPointDistance(_T("Dist"), RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS);
		else
			Cdim = new DimPointToPointDistance(_T("Dist"));
		Cdim->CalculateMeasurement(v1, v2);
		Cdim->ContainsLinearMeasurementMode(true);
		Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
		Cdim->SetMousePosition(x, y, z);
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
		Cdim->SetSelectionLine(&SelectionLine[0]);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0004", __FILE__, __FUNCSIG__); return NULL; }
}
	
//Measurement between two shapes...
DimBase* HelperClass::CreateShapeToShapeDim2D(Shape* s1, Shape* s2, double x, double y, double z, bool AngleWithAxis)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateShapeToShapeDim2D", __FUNCSIG__);
	try
	{
		//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "CreateShapeToShapeDim2D", __FUNCSIG__);

		DimBase* Cdim = NULL;
		if(s1 == s2)
		{
			switch(s1->ShapeType)
			{
				case RapidEnums::SHAPETYPE::CIRCLE:
					Cdim = new DimDiameter2D(_T("Dia"));
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D;
					((DimDiameter2D*)Cdim)->CalculateMeasurement(s1);
					break;
				case RapidEnums::SHAPETYPE::ARC:
					Cdim = new DimRadius2D(_T("Rad"));
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D;
					((DimRadius2D*)Cdim)->CalculateMeasurement(s1);
					break;
				case RapidEnums::SHAPETYPE::LINE:
					Cdim = new DimLine(_T("Dist"), RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH);
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH;
					((DimLine*)Cdim)->CalculateMeasurement((Line*)s1);
					break;
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XRAY:
					Cdim = new DimLine(_T("Ang"), RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE);
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE;
					((DimLine*)Cdim)->CalculateMeasurement((Line*)s1);
					break;

				case RapidEnums::SHAPETYPE::PERIMETER:
					Cdim = new DimPerimeter_Diameter(_T("Peri_Dia"), RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER);
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER;
					((DimPerimeter_Diameter*)Cdim)->CalculateMeasurement(s1);
				break;
				
				case RapidEnums::SHAPETYPE::AREA:
					Cdim = new DimAreaMeasurement(_T("Area"));
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT;
					((DimAreaMeasurement*)Cdim)->CalculateMeasurement(s1);
					break;
			}
		}
		else
		{
			if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::XLINE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::XRAY) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE , RapidEnums::SHAPETYPE::LINE) || 
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE , RapidEnums::SHAPETYPE::LINE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY , RapidEnums::SHAPETYPE::LINE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE , RapidEnums::SHAPETYPE::XRAY))
			{
				Cdim = new DimLineToLineDistance(true);
				if(CheckParallismRelationBtn2Lines((Line*)s1, (Line*)s2))
					Cdim->MeasurementType =  RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE;
				else
					Cdim->MeasurementType =  RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE;
				((DimLineToLineDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::CIRCLE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::CIRCLE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::ARC))
			{
				Cdim = new DimCircleToCircleDistance(_T("Dist"));
				Cdim->ContainsLinearMeasurementMode(true);
				((DimCircleToCircleDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::CIRCLE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::CIRCLE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::CIRCLE)||
						compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::ARC) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::ARC) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ARC))
			{
				Cdim = new DimCircleToLineDistance(_T("Dist"));
				((DimCircleToLineDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0005", __FILE__, __FUNCSIG__); return NULL; }
}

//Measurement between the point and a shape...//
DimBase* HelperClass::CreatePointToShapeDim2D(Vector* v1, Shape* s1, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreatePointToShapeDim2D", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		if(s1->ShapeType == RapidEnums::SHAPETYPE::XLINE || s1->ShapeType == RapidEnums::SHAPETYPE::XRAY || s1->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			Cdim = new DimPointToLineDistance(_T("Dist"));
			((DimPointToLineDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		else if(s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || s1->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			Cdim = new DimPointToCircleDistance(_T("Dist"));
			Cdim->ContainsLinearMeasurementMode(true);
			((DimPointToCircleDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0006", __FILE__, __FUNCSIG__); return NULL; }
}
	
//PCD Measurements
DimBase* HelperClass::CreatePCDMeasurements(Shape* s1, Shape* s2, Shape* s3, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreatePCDMeasurements", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_PCDOFFSET:
				Cdim = new DimPCDOffset();
				Cdim->MeasurementType = CurrentMtype;
				((DimPCDOffset*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PCDANGLE:
				Cdim = new DimPCDAngle();
				Cdim->MeasurementType = CurrentMtype;
				((DimPCDAngle*)Cdim)->CalculateMeasurement(s1, s2, s3);
				break;	
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0007", __FILE__, __FUNCSIG__); return NULL; }
}

//Right click on line and Plane..
DimBase* HelperClass::CreateRightClickDim3D(Shape* s1, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateRightClickDim3D", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS:
				Cdim = new DimLine3D(_T("Stl3d"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS;
				((DimLine3D*)Cdim)->CalculateMeasurement((Line*)s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED:
				Cdim = new DimLine3D(_T("Stl3d"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED;
				((DimLine3D*)Cdim)->CalculateMeasurement((Line*)s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS:
				Cdim = new DimFlatness(_T("Flt"));
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS;
				((DimFlatness*)Cdim)->CalculateMeasurement(s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED:
				Cdim = new DimFlatness(_T("Flt"));
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED;
				((DimFlatness*)Cdim)->CalculateMeasurement(s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY:
				Cdim = new DimCylindricity(_T("Cyl"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY;
				((DimCylindricity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED:
				Cdim = new DimCylindricity(_T("Cyl"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED;
				((DimCylindricity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC:
				Cdim = new DimCylindricity(_T("Cymic"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC;
				((DimCylindricity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC:
				Cdim = new DimCylindricity(_T("Cymcc"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC;
				((DimCylindricity*)Cdim)->CalculateMeasurement(s1);
				break;
				
			case RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY:
				Cdim = new DimSphericity(_T("Sph"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY;
				((DimSphericity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC:
				Cdim = new DimSphericity(_T("Sphmcc"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC;
				((DimSphericity*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC:
				Cdim = new DimSphericity(_T("Sphmic"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC;
				((DimSphericity*)Cdim)->CalculateMeasurement(s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D:
				Cdim = new DimDiameter3D(_T("Dia"));
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D;
				((DimDiameter3D*)Cdim)->CalculateMeasurement(s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS3D:
				Cdim = new DimRadius3D(_T("Rad"));
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_RADIUS3D;
				((DimRadius3D*)Cdim)->CalculateMeasurement(s1);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
				Cdim = new DimPointCoordinate(_T("Center"));
				if(s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || s1->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
					((DimPointCoordinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				if(s1->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
					 ((DimPointCoordinate*)Cdim)->CalculateMeasurement(((Sphere*)s1)->getCenter());
				((DimPointCoordinate*)Cdim)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				Cdim = new DimOrdinate(_T("Pt_X"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				if(s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || s1->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
					  ((DimOrdinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				if(s1->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
					  ((DimOrdinate*)Cdim)->CalculateMeasurement(((Sphere*)s1)->getCenter());
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				Cdim = new DimOrdinate(_T("Pt_Y"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				if(s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || s1->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
					  ((DimOrdinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				if(s1->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
					  ((DimOrdinate*)Cdim)->CalculateMeasurement(((Sphere*)s1)->getCenter());
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
				Cdim = new DimOrdinate(_T("Pt_Z"), CurrentMtype);
				Cdim->MeasurementType = CurrentMtype;
				if(s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || s1->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
					  ((DimOrdinate*)Cdim)->CalculateMeasurement(((Circle*)s1)->getCenter());
				if(s1->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
					  ((DimOrdinate*)Cdim)->CalculateMeasurement(((Sphere*)s1)->getCenter());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ:
				Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ;
				((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA:
				Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA;
				((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP:
				Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP;
				((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV:
				Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV;
				((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ:
				Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ;
				((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK:
				Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK;
				((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(s1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU:
				Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU;
				((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(s1);
				break;
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* HelperClass::Create3DTruePositionForPoint(Vector* V1, Shape* s2, Shape* s3, Shape* s4, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Tposition_X, double Tposition_Y, double x, double y,double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "Create3DTruePositionForPoint", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D:
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R:
				Cdim = new DimTruePosition3DPoint(_T("TPG3D"), CurrentMtype);
				((DimTruePosition3DPoint*)Cdim)->SetTPposition(Tposition_X, Tposition_Y);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition3DPoint*)Cdim)->CalculateMeasurement(V1, s2, s3, s4);
				break;
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			AddGDnTReferenceDatum(Cdim, s2);
			if(CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D || CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R)
			{
				AddGDnTReferenceDatum(Cdim, s3);
				AddGDnTReferenceDatum(Cdim, s4);
			}
			return Cdim;
		}
		return NULL;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0008", __FILE__, __FUNCSIG__); return NULL;
	}
}

//Handling the angularity of line3D, Cylinder, Plane etc..
DimBase* HelperClass::CreateAngularityDim3D(Shape* s1, Shape* s2, Shape* s3, Shape* s4, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Angle, double land, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateAngularityDim3D", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
				Cdim = new DimCylinderAngularity(_T("CylPer"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY;
				((DimCylinderAngularity*)Cdim)->SetLandAndAngle(M_PI_2, land);
				((DimCylinderAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
				Cdim = new DimCylinderAngularity(_T("CylAng"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY;
				((DimCylinderAngularity*)Cdim)->SetLandAndAngle(Angle *  M_PI/180, land);
				((DimCylinderAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
				Cdim = new DimCylinderAngularity(_T("CylCoax"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY;
				((DimCylinderAngularity*)Cdim)->SetLandAndAngle(0, land);
				((DimCylinderAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
				Cdim = new DimCylinderAngularity(_T("CylPar"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM;
				((DimCylinderAngularity*)Cdim)->SetLandAndAngle(0, land);
				((DimCylinderAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
				Cdim = new DimPlaneAngularity(_T("PlPar"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM;
				((DimPlaneAngularity*)Cdim)->SetLandAndAngle(0, land);
				((DimPlaneAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
				Cdim = new DimPlaneAngularity(_T("PlPer"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY;
				((DimPlaneAngularity*)Cdim)->SetLandAndAngle(M_PI_2, land);
				((DimPlaneAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
				Cdim = new DimPlaneAngularity(_T("PlAng"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY;
				((DimPlaneAngularity*)Cdim)->SetLandAndAngle(Angle * M_PI/180, land);
				((DimPlaneAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;

			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
				Cdim = new DimLine3DAngularity(_T("l3dPar"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM;
				((DimLine3DAngularity*)Cdim)->SetLandAndAngle(0, land);
				((DimLine3DAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
				Cdim = new DimLine3DAngularity(_T("l3dPer"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY;
				((DimLine3DAngularity*)Cdim)->SetLandAndAngle(M_PI_2, land);
				((DimLine3DAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
				Cdim = new DimLine3DAngularity(_T("l3dAng"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY;
				((DimLine3DAngularity*)Cdim)->SetLandAndAngle(Angle * M_PI/180, land);
				((DimLine3DAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
				Cdim = new DimConeAngularity(_T("ConPer"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY;
				((DimConeAngularity*)Cdim)->SetLandAndAngle(M_PI_2, land);
				((DimConeAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
				Cdim = new DimConeAngularity(_T("ConAng"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY;
				((DimConeAngularity*)Cdim)->SetLandAndAngle(Angle *  M_PI/180, land);
				((DimConeAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
				Cdim = new DimConeAngularity(_T("ConCoax"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY;
				((DimConeAngularity*)Cdim)->SetLandAndAngle(0, land);
				((DimConeAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
				Cdim = new DimConeAngularity(_T("ConPar"), CurrentMtype);
				Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM;
				((DimConeAngularity*)Cdim)->SetLandAndAngle(0, land);
				((DimConeAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R:
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D:
				Cdim = new DimTruePosition3D(_T("TPG3D"), CurrentMtype);
				((DimTruePosition3D*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition3D*)Cdim)->CalculateMeasurement(s1, s2, s3, s4);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_AXIALRUNOUT:
				Cdim = new DimRunOutMeasurement(_T("ARo"), CurrentMtype);
				Cdim->ParentShape1 = s1; Cdim->ParentShape2 = s2;
				((DimRunOutMeasurement*)Cdim)->ParentsShape.push_back(s1);
				((DimRunOutMeasurement*)Cdim)->ParentsShape.push_back(s2);
				((DimRunOutMeasurement*)Cdim)->CalculateMeasurement();
				break;
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			AddGDnTReferenceDatum(Cdim, s2);
			if(CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R || CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D)
			{
				AddGDnTReferenceDatum(Cdim, s3);
				AddGDnTReferenceDatum(Cdim, s4);
			}
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0009", __FILE__, __FUNCSIG__); return NULL; }
}
	
//Point to Point Distance 3d
DimBase* HelperClass::CreatePointDim3D(Vector* v1, Vector* v2, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreatePointDim3D", __FUNCSIG__);
	try
	{
		DimPointToPointDistance3D* Cdim = new DimPointToPointDistance3D(_T("Dist"));
		Cdim->CalculateMeasurement(v1, v2);
		Cdim->ContainsLinearMeasurementMode(true);
		Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
		Cdim->SetMousePosition(x, y, z);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0010", __FILE__, __FUNCSIG__); return NULL; }
}
	
//Point to shape distance 3D
DimBase* HelperClass::CreatePointToShapeDim3D(Vector* v1, Shape* s1, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreatePointToShapeDim3D", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		if(s1->ShapeType == RapidEnums::SHAPETYPE::XLINE || s1->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || s1->ShapeType == RapidEnums::SHAPETYPE::XRAY || s1->ShapeType == RapidEnums::SHAPETYPE::LINE || s1->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
		{
			Cdim = new DimPointToLineDistance3D(_T("Dist"));
			((DimPointToLineDistance3D*)Cdim)->CalculateMeasurement(s1, v1);
		}
		else if(s1->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			Cdim = new DimPointToPlaneDistance(_T("Dist"));
			((DimPointToPlaneDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		else if(s1->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
		{
			Cdim = new DimPointToCylinderDistance(_T("Dist"));
			((DimPointToCylinderDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		else if(s1->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		{
			Cdim = new DimPointToSphereDistance(_T("Dist"));
			((DimPointToSphereDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		else if(s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
		{
			Cdim = new DimPointToCircle3DDistance(_T("Dist"));
			((DimPointToCircle3DDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		else if(s1->ShapeType == RapidEnums::SHAPETYPE::CONE)
		{
			Cdim = new DimPointToConeDistance(_T("Dist"));
			((DimPointToConeDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		else if(s1->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
		{
			Cdim = new DimPointToConics3DDistance(_T("Dist"));
			((DimPointToConics3DDistance*)Cdim)->CalculateMeasurement(s1, v1);
		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0011", __FILE__, __FUNCSIG__); return NULL; }
}
	
//Shape to Shape distance 3D
DimBase* HelperClass::CreateShapeToShapeDim3D(Shape* s1, Shape* s2, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateShapeToShapeDim3D", __FUNCSIG__);
	try
	{
		DimBase* Cdim = NULL;
		if(s1 == s2)
		{
			switch(s1->ShapeType)
			{
				case RapidEnums::SHAPETYPE::CIRCLE3D:
				case RapidEnums::SHAPETYPE::CYLINDER:
				case RapidEnums::SHAPETYPE::SPHERE:
					Cdim = new DimDiameter3D(_T("Dia"));
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D;
					((DimDiameter3D*)Cdim)->CalculateMeasurement(s1);
					break;
				case RapidEnums::SHAPETYPE::LINE3D:
					Cdim = CreatePointDim3D(((Line*)s1)->getPoint1(), ((Line*)s1)->getPoint2(), x, y, z);
					s1->addMChild(Cdim);
					Cdim->addParent(s1);
					break;
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					Cdim = new DimCloudPointDepth(_T("Dist"));
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CLOUDPOINTDEPTH;
					((DimCloudPointDepth*)Cdim)->CalculateMeasurement(s1);
					break;
				case RapidEnums::SHAPETYPE::CONE:
					Cdim = new DimAngle3D(_T("Ang"));
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONEANGLE;
					((DimAngle3D*)Cdim)->CalculateMeasurement(s1);
					break;
				case RapidEnums::SHAPETYPE::ELLIPSE3D:
					Cdim = new DimAxisLength(_T("Dist"));
					Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_AXISLENGTH;
					((DimAxisLength*)Cdim)->CalculateMeasurement(s1);
					break;
			}
		}
		else if(s1->ShapeAs3DShape() || s2->ShapeAs3DShape())
		{
			if((s1->ShapeType == RapidEnums::SHAPETYPE::LINE || s1->ShapeType == RapidEnums::SHAPETYPE::LINE3D || s1->ShapeType == RapidEnums::SHAPETYPE::XLINE || s1->ShapeType == RapidEnums::SHAPETYPE::XRAY || s1->ShapeType == RapidEnums::SHAPETYPE::XLINE3D) &&
				(s1->ShapeType == RapidEnums::SHAPETYPE::LINE || s1->ShapeType == RapidEnums::SHAPETYPE::LINE3D || s1->ShapeType == RapidEnums::SHAPETYPE::XLINE || s1->ShapeType == RapidEnums::SHAPETYPE::XRAY || s1->ShapeType == RapidEnums::SHAPETYPE::XLINE3D))
			{
				Cdim = new DimLineToLineDistance3D(true);
				((DimLineToLineDistance3D*)Cdim)->CalculateMeasurement(s1, s2);
			}
			
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::PLANE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::PLANE) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::PLANE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::PLANE) 
					|| compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::PLANE))
			{
				Cdim = new DimLineToPlaneDistance(true);
				((DimLineToPlaneDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::CYLINDER) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::CYLINDER) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::CYLINDER) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::CYLINDER) 
					|| compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::CYLINDER))
			{
				Cdim = new DimLineToCylinderDistance(true);
				((DimLineToCylinderDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::SPHERE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::SPHERE) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::SPHERE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::SPHERE) 
				|| compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::SPHERE))
			{
				Cdim = new DimLineToSphereDistance(_T("Dist"));
				((DimLineToSphereDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::PLANE))
			{
				Cdim = new DimPlaneToPlaneDistance(true);
				((DimPlaneToPlaneDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::CYLINDER))
			{
				Cdim = new DimPlaneToCylinderDistance(true);
				((DimPlaneToCylinderDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::SPHERE))
			{
				Cdim = new DimPlaneToSphereDistance(_T("Dist"));
				((DimPlaneToSphereDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::CYLINDER))
			{
				Cdim = new DimCylinderToCylinderDistance(true);
				((DimCylinderToCylinderDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::SPHERE))
			{
				Cdim = new DimCylinderToSphereDistance(_T("Dist"));
				((DimCylinderToSphereDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::SPHERE, RapidEnums::SHAPETYPE::SPHERE))
			{
				Cdim = new DimSphereToSphereDistance(_T("Dist"));
				((DimSphereToSphereDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::CIRCLE3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::CIRCLE3D) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::CIRCLE3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::CIRCLE3D) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::CIRCLE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::CIRCLE3D) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::CIRCLE)) 
			{
				Cdim = new DimLineToCircle3DDistance(true);
				((DimLineToCircle3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::CIRCLE3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::CIRCLE))
			{
				Cdim = new DimPlaneToCircle3DDistance(true);
				((DimPlaneToCircle3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE3D, RapidEnums::SHAPETYPE::CIRCLE3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE3D, RapidEnums::SHAPETYPE::CIRCLE))
			{
				Cdim = new DimCircle3DToCircle3DDistance(true);
				((DimCircle3DToCircle3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE3D, RapidEnums::SHAPETYPE::SPHERE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::SPHERE))
			{
				Cdim = new DimSphereToCircle3DDistance(_T("Dist"));
				((DimSphereToCircle3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::CONE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::CONE) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::CONE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::CONE))
			{
				Cdim = new DimLineToConeDistance(true);
				((DimLineToConeDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::PLANE))
			{
				Cdim = new DimPlaneToConeDistance(true);
				((DimPlaneToConeDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::CYLINDER))
			{
				Cdim = new DimCylinderToConeDistance(true);
				((DimCylinderToConeDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::CONE))
			{
				Cdim = new DimConeToConeDistance(true);
				((DimConeToConeDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::SPHERE))
			{
				Cdim = new DimSphereToConeDistance(_T("Dist"));
				((DimSphereToConeDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::CIRCLE3D))
			{
				Cdim = new DimCircle3DToConeDistance(true);
				((DimCircle3DToConeDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ELLIPSE3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::ELLIPSE3D) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::ELLIPSE3D))
			{
				Cdim = new DimLineToConics3DDistance(true);
				((DimLineToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::ELLIPSE3D))
			{
				Cdim = new DimConeToConics3DDistance(true);
				((DimConeToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::HYPERBOLA3D))
			{
				Cdim = new DimConeToConics3DDistance(true);
				((DimConeToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::PARABOLA3D))
			{
				Cdim = new DimConeToConics3DDistance(true);
				((DimConeToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::SPHERE, RapidEnums::SHAPETYPE::ELLIPSE3D))
			{
				Cdim = new DimSphereToConics3DDistance(_T("Dist"));
				((DimSphereToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::SPHERE, RapidEnums::SHAPETYPE::HYPERBOLA3D))
			{
				Cdim = new DimSphereToConics3DDistance(_T("Dist"));
				((DimSphereToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::SPHERE, RapidEnums::SHAPETYPE::PARABOLA3D))
			{
				Cdim = new DimSphereToConics3DDistance(_T("Dist"));
				((DimSphereToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::HYPERBOLA3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::HYPERBOLA3D) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::HYPERBOLA3D))
			{
				Cdim = new DimLineToConics3DDistance(true);
				((DimLineToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::PARABOLA3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE3D, RapidEnums::SHAPETYPE::PARABOLA3D) ||
				compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE3D, RapidEnums::SHAPETYPE::PARABOLA3D))
			{
				Cdim = new DimLineToConics3DDistance(true);
				((DimLineToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::ELLIPSE3D))
			{
				Cdim = new DimPlaneToConics3DDistance(true);
				((DimPlaneToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::HYPERBOLA3D))
			{
				Cdim = new DimPlaneToConics3DDistance(true);
				((DimPlaneToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::PARABOLA3D))
			{
				Cdim = new DimPlaneToConics3DDistance(true);
				((DimPlaneToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::ELLIPSE3D))
			{
				Cdim = new DimCylinderToConics3DDistance(true);
				((DimCylinderToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::HYPERBOLA3D))
			{
				Cdim = new DimCylinderToConics3DDistance(true);
				((DimCylinderToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::PARABOLA3D))
			{
				Cdim = new DimCylinderToConics3DDistance(true);
				((DimCylinderToConics3DDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::INTERSECTIONSHAPE, RapidEnums::SHAPETYPE::PLANE))
			{
				Cdim = new DimIntersectionShapeToPlaneDistance(_T("Dist"));
				((DimIntersectionShapeToPlaneDistance*)Cdim)->CalculateMeasurement(s1, s2);
			}
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::CIRCLE) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::CIRCLE3D) || compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CYLINDER, RapidEnums::SHAPETYPE::ARC))
			{   
				if(s1->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
				{
					Cdim = CreatePointToShapeDim3D(((Circle*)s2)->getCenter(), s1, x, y, z);
				}
				else
				{
					Cdim = CreatePointToShapeDim3D(((Circle*)s1)->getCenter(), s2, x, y, z);
				}
			}
		}	
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).GetSelectionLine(x, y, SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0012", __FILE__, __FUNCSIG__); return NULL; }
}

//Rotary Angle Measure.. Special tool for delphi..
DimBase* HelperClass::CreateRotaryMeasurement(Shape* s1, Shape* s2, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateRotaryMeasurement", __FUNCSIG__);
	try
	{ 
		DimBase* Cdim = NULL;
		Cdim = new DimRotaryAngleMeasure(_T("R_Ang"));
		Cdim->MeasurementType =  RapidEnums::MEASUREMENTTYPE::DIM_ROTARYANGLEMEASURE;
		((DimRotaryAngleMeasure*)Cdim)->CalculateMeasurement(s1, s2);
		Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
		Cdim->SetMousePosition(x, y, z);
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(1).GetSelectionLine(x, y, SelectionLine);
		Cdim->SetSelectionLine(&SelectionLine[0]);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0014", __FILE__, __FUNCSIG__); return NULL; }
}

//Digital Micrometer drawing..
DimBase* HelperClass::CreateDMMDim(Vector* v1, Vector* v2, double x, double y, double z)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateDMMDim", __FUNCSIG__);
	try
	{
		DimDigitalMicrometer* Cdim = new DimDigitalMicrometer(_T("duM"));
		Cdim->SetMousePosition(x, y, z, false);
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).GetSelectionLine(x, y, SelectionLine);
		Cdim->SetSelectionLine(&SelectionLine[0], false);
		Cdim->CalculateMeasurement(v1, v2);
		Cdim->ContainsLinearMeasurementMode(false);
		Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015", __FILE__, __FUNCSIG__); return NULL; }
}

//Function to add GD&T Reference shape..
void HelperClass::AddGDnTReferenceDatum(DimBase* dim, Shape* s)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddGDnTReferenceDatum", __FUNCSIG__);
	try
	{
		dim->MeasureAsGDnTMeasurement(true);
		int rfdt = s->RefernceDatumForgdnt();
		rfdt++;
		s->RefernceDatumForgdnt(rfdt);
		if(rfdt == 1)
		{
			int type = MAINDllOBJECT->ReferenceDatumNameCount_GDT();
			char Cc = char(65 + type);
			type += 1;
			MAINDllOBJECT->ReferenceDatumNameCount_GDT(type);
			s->DatumName = Cc;
			MAINDllOBJECT->Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0013", __FILE__, __FUNCSIG__); }
}

DimBase* HelperClass::CreateAngleMeasurment(Shape* CShape1, Shape* CShape2, bool AddAction)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateAngleMeasurment", __FUNCSIG__);
	try
	{
		DimBase* Cdim = CreateShapeToShapeDim2D(CShape1, CShape2, MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), 0);
		AddShapeMeasureRelationShip(CShape1, Cdim);
		AddShapeMeasureRelationShip(CShape2, Cdim);
		((DimLineToLineDistance*)Cdim)->AngleTypeWithAxis = DimLineToLineDistance::ANGLEWITHAXIS::DEFAULT;
		SetAngleMeasurementPosition(Cdim);
		Cdim->IsValid(true);
		Cdim->UpdateMeasurement();
		if(AddAction)
			AddDimAction::addDim(Cdim);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015", __FILE__, __FUNCSIG__); return NULL; }
}

void HelperClass::SetAngleMeasurementPosition(DimBase* CDim, double OffsetPos)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "SetAngleMeasurementPosition", __FUNCSIG__);
	try
	{
		Line* Cshape1 = (Line*)CDim->ParentShape1;
		Line* Cshape2 = (Line*)CDim->ParentShape2;

		double langle1 = Cshape1->Angle(), langle2 =  Cshape2->Angle(), radius, r_angle1, r_angle2, mid_angle;
		double ptofIntersection[2], L1point1[2], L1point2[2], L2point1[2], L2point2[2];
		Cshape1->getMidPoint()->FillDoublePointer(&ptofIntersection[0]);
		Cshape1->getPoint1()->FillDoublePointer(&L1point1[0]); Cshape1->getPoint2()->FillDoublePointer(&L1point2[0]);
		Cshape2->getPoint1()->FillDoublePointer(&L2point1[0]); Cshape2->getPoint2()->FillDoublePointer(&L2point2[0]);
		if(abs(langle1 - langle2) < 0.00001)
		{
			CDim->SetMousePosition(ptofIntersection[0], ptofIntersection[1], 0, true);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).GetSelectionLine(ptofIntersection[0], ptofIntersection[1], SelectionLine);
			CDim->SetSelectionLine(&SelectionLine[0]);
		}
		else
		{
			RMATH2DOBJECT->Line_lineintersection(Cshape1->Angle(), Cshape1->Intercept(), Cshape2->Angle(), Cshape2->Intercept(), &ptofIntersection[0]);
			if(RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L1point1[0]) > RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L1point2[0]))
				r_angle1 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L1point1[0]);
			else
				r_angle1 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L1point2[0]);
			if(RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L2point1[0]) > RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L2point2[0]))
				r_angle2 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L2point1[0]);
			else
				r_angle2 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L2point2[0]);
			if(abs(r_angle1 - r_angle2) > M_PI)
			{
				if(r_angle1 < r_angle2) r_angle1 += 2 * M_PI;
				else r_angle2 += 2 * M_PI;
			}
			mid_angle = (r_angle1 + r_angle2)/2;
			radius = RMATH2DOBJECT->Pt2Pt_distance(ptofIntersection[0], ptofIntersection[1], Cshape1->getMidPoint()->getX(), Cshape1->getMidPoint()->getY());
			radius += OffsetPos;
			CDim->SetMousePosition(ptofIntersection[0] + radius * cos(mid_angle), ptofIntersection[1] + radius * sin(mid_angle), 0, true);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).GetSelectionLine(ptofIntersection[0] + radius * cos(mid_angle), ptofIntersection[1] + radius * sin(mid_angle), SelectionLine);
			CDim->SetSelectionLine(&SelectionLine[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015", __FILE__, __FUNCSIG__); }
}

DimBase* HelperClass::CreateRadiusMeasurement(Shape* CShape)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateRadiusMeasurement", __FUNCSIG__);
	try
	{
		DimBase* Cdim = CreateRightClickDim2D(CShape, RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D, MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), 0);
		AddShapeMeasureRelationShip(CShape, Cdim);
		Cdim->IsValid(true);
		SetRadiusMeasurementPosition(Cdim);
		Cdim->UpdateMeasurement();
		AddDimAction::addDim(Cdim);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015a", __FILE__, __FUNCSIG__); return NULL; }
}
	
void HelperClass::SetRadiusMeasurementPosition(DimBase* CDim)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "SetRadiusMeasurementPosition", __FUNCSIG__);
	try
	{
		Circle* Cshape = (Circle*)CDim->ParentShape1;
		double midangle = Cshape->Startangle() + Cshape->Sweepangle()/2;
		double radius = Cshape->Radius();
		if(radius == 0 || Cshape->PointsList->Pointscount() < 3) return;
		double center[2] = {Cshape->getCenter()->getX(), Cshape->getCenter()->getY()};
		double ptonarc[2] = {center[0] + radius * cos(midangle), center[1] + radius * sin(midangle)};
		double posshift = 100 * MAINDllOBJECT->getWindow(0).getUppX();
		if(ptonarc[0] < center[0])
		{
			CDim->SetMousePosition(ptonarc[0] - posshift, ptonarc[1], 0);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).GetSelectionLine(ptonarc[0] - posshift, ptonarc[1], SelectionLine);
			CDim->SetSelectionLine(&SelectionLine[0]);
		}
		else
		{
			CDim->SetMousePosition(ptonarc[0] + posshift, ptonarc[1], 0);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).GetSelectionLine(ptonarc[0] + posshift, ptonarc[1], SelectionLine);
			CDim->SetSelectionLine(&SelectionLine[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015b", __FILE__, __FUNCSIG__); }
}

DimBase* HelperClass::CreatePt2PtDist_WrtLine(Shape* CShape, Shape* v1Parent1, Shape* v1Parent2, Shape* v2Parent1, Shape* v2Parent2)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreatePt2PtDist_WrtLine", __FUNCSIG__);
	try
	{
		//Vector *Vpointer1 = NULL, *Vpointer2 = NULL;
		//Vector temp;
		Vector Vpointer1, Vpointer2;
		double IntersecPoint1[2], IntersecPoint2[2];
		Line* CSh1 = (Line*)v1Parent1; Line* CSh2 = (Line*)v1Parent2;
		Line* CSh3 = (Line*)v2Parent1; Line* CSh4 = (Line*)v2Parent2;
		RMATH2DOBJECT->Line_lineintersection(CSh1->Angle(), CSh1->Intercept(), CSh2->Angle(), CSh2->Intercept(), &IntersecPoint1[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh3->Angle(), CSh3->Intercept(), CSh4->Angle(), CSh4->Intercept(), &IntersecPoint2[0]);
		double midpoint[2] = {(IntersecPoint1[0] + IntersecPoint2[0])/2, (IntersecPoint1[1] + IntersecPoint2[1])/2};
		/*temp.set(IntersecPoint1[0], IntersecPoint1[1]);
		Vpointer1 = MAINDllOBJECT->getVectorPointer(&temp);
		temp.set(IntersecPoint2[0], IntersecPoint2[1]);
		Vpointer2 = MAINDllOBJECT->getVectorPointer(&temp);*/
		Vpointer1.set(IntersecPoint1[0], IntersecPoint1[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer2.set(IntersecPoint2[0], IntersecPoint2[1], MAINDllOBJECT->getCurrentDRO().getZ());
		DimPointToPointDistLAngle* Cdim = new DimPointToPointDistLAngle(_T("Dist"));
		Cdim->CalculateMeasurement(&Vpointer1, &Vpointer2, CShape);
		//Cdim->CalculateMeasurement(Vpointer1, Vpointer2, CShape);
		Cdim->SetMousePosition(midpoint[0], midpoint[1], 0);
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(1).GetSelectionLine(midpoint[0], midpoint[1], SelectionLine);
		Cdim->SetSelectionLine(&SelectionLine[0]);
		AddShapeMeasureRelationShip(CShape, Cdim);
		AddShapeMeasureRelationShip(v1Parent1, Cdim);
		AddShapeMeasureRelationShip(v1Parent2, Cdim);
		AddShapeMeasureRelationShip(v2Parent1, Cdim);
		AddShapeMeasureRelationShip(v2Parent2, Cdim);
		Cdim->IsValid(true);
		Cdim->UpdateMeasurement();
		AddDimAction::addDim(Cdim);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* HelperClass::CreateLinetoLine_Dist(Shape* CShape1, Shape* CShape2, bool AddAction)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateLinetoLine_Dist", __FUNCSIG__);
	try
	{
		Vector* v1 = ((Line*)CShape1)->getMidPoint();
		Vector* v2 = ((Line*)CShape2)->getMidPoint();
		DimPointToLineDistance* Cdim = new DimPointToLineDistance(_T("Dist"));
		Cdim->CalculateMeasurement(CShape2, v1);
		AddShapeMeasureRelationShip(CShape1, Cdim);
		AddShapeMeasureRelationShip(CShape2, Cdim);
		double Midpt[3] = {(v1->getX() + v2->getX())/2, (v1->getY() + v2->getY())/2, 0};
		Cdim->SetMousePosition(Midpt[0], Midpt[1], Midpt[2]);
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(1).GetSelectionLine(Midpt[0], Midpt[1], SelectionLine);
		Cdim->SetSelectionLine(&SelectionLine[0]);
		Cdim->IsValid(true);
		Cdim->UpdateMeasurement();
		if(AddAction)
			AddDimAction::addDim(Cdim);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015", __FILE__, __FUNCSIG__); return NULL; }
}

void HelperClass::AddShapeMeasureRelationShip(Shape* CShape, DimBase* Cdim)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddShapeMeasureRelationShip", __FUNCSIG__);
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(Cdim);
			Cdim->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015", __FILE__, __FUNCSIG__); }
}

//Add line arc action .
void HelperClass::AddLineArcAction_ProfileScan(bool auto_thread)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddLineArcAction_ProfileScan", __FUNCSIG__);
	try
	{
		CurrentProfScanMode = RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_LINEARCACT;
		if (!auto_thread)
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		}
		CurrentProfileCpShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(CurrentProfileCpShape == NULL || CurrentProfileCpShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS)
		{
			CurrentProfileCpShape = new CloudPoints();
			CurrentProfileCpShape->ShapeAs3DShape(false);
			AddShapeAction::addShape(CurrentProfileCpShape);
		}
		if(CurrentProfileCpShape->PointsList->Pointscount() < 100)
			isFirstFrame = true;//Set Profile Scan Start Mode..
		
		profile_scan_auto_thread = auto_thread;
		profile_scan_stopped = false;
		profile_scan_paused = false;
		ProfileScan();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP1027", __FILE__, __FUNCSIG__); }
}

void HelperClass::MergeShapesOfLineArcAction(bool mergeasLine)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "MergeShapesOfLineArcAction", __FUNCSIG__);
	try
	{
		if(ProfileScanLineArcAction != NULL)
			((LineArcAction*)ProfileScanLineArcAction)->mergeShapes(mergeasLine);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP1028", __FILE__, __FUNCSIG__); }
}

void HelperClass::Chamfer_Line_Arc(bool linearc)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "Chamfer_Line_Arc", __FUNCSIG__);
	try
	{
		MAINDllOBJECT->SetImageInEdgeDetection();
		int RectFGArray[4] = {0, 0, MAINDllOBJECT->getWindow(0).getWinDim().x, MAINDllOBJECT->getWindow(0).getWinDim().y};
		int noofpts = EDSCOBJECT->DetectAllProfileEdges(&RectFGArray[0]);
		if(noofpts < 200)
		{
			MAINDllOBJECT->ShowMsgBoxString("HelperClassChamfer_Line_Arc01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		PointCollection TempPointColl;

		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX(), upp_y = MAINDllOBJECT->getWindow(0).getUppY();

		for(int n = 0; n < noofpts; n++)
		{
			if(!((EDSCOBJECT->DetectedPointsList[n * 2] == -1) && (EDSCOBJECT->DetectedPointsList[n * 2 + 1] == -1)))
				TempPointColl.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, MAINDllOBJECT->getCurrentDRO().getZ()));
		}
		ProfileScanLineArcAction = new LineArcAction();
		MAINDllOBJECT->getActionsHistoryList().addItem(ProfileScanLineArcAction);
		((LineArcAction*)ProfileScanLineArcAction)->lineArcSep(&TempPointColl);
		list<int> Shapeid;
		for(map<int, Shape*>::iterator It = ((LineArcAction*)ProfileScanLineArcAction)->TempShapeCollection.begin(); It != ((LineArcAction*)ProfileScanLineArcAction)->TempShapeCollection.end(); It++)
			Shapeid.push_back(It->second->getId());
			MAINDllOBJECT->selectShape(&Shapeid);
		if(ProfileScanLineArcAction != NULL)
			((LineArcAction*)ProfileScanLineArcAction)->chamferShapes(linearc);
		MAINDllOBJECT->Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP1026", __FILE__, __FUNCSIG__); }
}

void HelperClass::AddtheProfileScanShape()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddtheProfileScanShape", __FUNCSIG__);
	try
	{
		MAINDllOBJECT->SetStatusCode("HelperClassAddtheProfileScanShape01");
		//Add New Cloud Point Shape..
		CurrentProfScanMode = RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_NORMAL;
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		//CurrentProfileCpShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		//if(CurrentProfileCpShape == NULL || CurrentProfileCpShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS)
		//{
			CurrentProfileCpShape = new CloudPoints();
			AddShapeAction::addShape(CurrentProfileCpShape);
		//}
		//CurrentProfileCpShape->ShapeAs3DShape(false);
		//if(CurrentProfileCpShape->PointsList->Pointscount() < 100)
		//{
			isFirstFrame = true;//Set Profile Scan Start Mode..
			skip_cont_constraint = false;
		//}
		profile_scan_auto_thread = false;
		profile_scan_stopped = false;
		profile_scan_paused = false;
		profile_scan_failed = true;
		skip_cont_constraint = true;
		profile_scan_running = false;
		profile_scan_ended = false;
		profileScanPtsCnt = 0;
		MAINDllOBJECT->SetStatusCode("HelperClass02");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0026", __FILE__, __FUNCSIG__); }
}

void HelperClass::ProfileScanContinue()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "ProfileScanContinue", __FUNCSIG__);
	try
	{
		profile_scan_paused = false;
		//Add New Cloud Point Shape..
		//CurrentProfScanMode = RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_NORMAL;
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		//AddShapeAction::addShape(CurrentProfileCpShape);
		isFirstFrame = false;//Set Profile Scan Start Mode..
		skip_cont_constraint = false;
		//}
		profile_scan_auto_thread = false;
		profile_scan_stopped = false;
		profile_scan_paused = false;
		profile_scan_failed = true;
		skip_cont_constraint = true;
		profile_scan_running = false;
		profile_scan_ended = false;
		profileScanPtsCnt = 0;
	//	ProfileScan();
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0026a", __FILE__, __FUNCSIG__);
	}
}

void HelperClass::ProfileScan(bool chk_for_cont)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "ProfileScan", __FUNCSIG__);
	try
	{
		if (profile_scan_stopped)
		{
			AutoScanDone= false;
			PrevFramePointslist.deleteAll();
			profile_scan_failed = false;
			profile_scan_running = false;
			skip_cont_constraint = false;
			profile_scan_ended = true;
			profile_scan_paused = false;
			profile_scan_stopped = false;
			//MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			ProfileScanFinished(false);
			return;
		}

		EDSCOBJECT->Pixel_Neighbour = MAINDllOBJECT->PixelNeighbour();
		int pixel_neighbour = EDSCOBJECT->Pixel_Neighbour;
		EDSCOBJECT->MinimumPointsInEdge = MAINDllOBJECT->MinimumPointsInEdge();

		
		if (profile_scan_paused)
		{
			if(skip_cont_constraint == true)
			{
				profile_scan_paused = false;
				MAINDllOBJECT->PointsTakenProfileScan(2);
			}
			else
			{
				skip_cont_constraint = true;
				profile_scan_failed = true;
				profile_scan_running = false;
				return;
			}
		}

		profileScanPtsCnt = 0;
		int jump = ProfileScanJumpParam;
		profile_scan_running = true;

		if (jump <= 0)
		{
			if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
				jump = 1;
			else
				jump = 5;
		}

		profile_scan_ended = false;
		bool reversal = false;
		profile_scan_failed = false;
		double edgepoints[20000]={0};
		// this is used for checking for start point falling within limit_pixel_ct to flag end of profile scan
		if (isFirstFrame)
			profile_scan_iter = 1;
		int limit_pixel_ct = 5 * profile_scan_iter ;

		if (limit_pixel_ct > pixel_neighbour) 
			limit_pixel_ct = pixel_neighbour;

		MAINDllOBJECT->SetImageInEdgeDetection();
		int skipWidth = MAINDllOBJECT->SkipPixelWidth(), skipHeight = MAINDllOBJECT->SkipPixelHeight();
		int RectFGArray[4] = {skipWidth, skipHeight, MAINDllOBJECT->getWindow(0).getWinDim().x - (2 * skipWidth),
								MAINDllOBJECT->getWindow(0).getWinDim().y - (2 * skipHeight)};
		
		if (skip_cont_constraint == true)
		{
			RectFGArray[0] = min(profileScanPixelPt[2], profileScanPixelPt[0]); // profileScanPixelPt[0];
			RectFGArray[1] = min(profileScanPixelPt[1], profileScanPixelPt[3]);// profileScanPixelPt[1];
			RectFGArray[2] = abs(max(profileScanPixelPt[2], profileScanPixelPt[0]) - min(profileScanPixelPt[2], profileScanPixelPt[0]));
			RectFGArray[3] = abs(max(profileScanPixelPt[1], profileScanPixelPt[3]) - min(profileScanPixelPt[1], profileScanPixelPt[3])); // abs(profileScanPixelPt[3] - profileScanPixelPt[1]);
			//if (profileScanPixelPt[2] - profileScanPixelPt[0] < 0)
			//	RectFGArray[0] = profileScanPixelPt[2];
			//if (profileScanPixelPt[3] - profileScanPixelPt[1] < 0)
			//	RectFGArray[1] = profileScanPixelPt[3];
			if (RectFGArray[0] < 0)
				RectFGArray[0] = 0;
			if (RectFGArray[1] < 0)
				RectFGArray[1] = 0;
			if (RectFGArray[2] > MAINDllOBJECT->getWindow(0).getWinDim().x)
				RectFGArray[2] = MAINDllOBJECT->getWindow(0).getWinDim().x;
			if (RectFGArray[3] > MAINDllOBJECT->getWindow(0).getWinDim().y)
				RectFGArray[3] = MAINDllOBJECT->getWindow(0).getWinDim().y;
		}
		//FG_min_length is a measure of min number of edge points expected.
		int FG_min_length = RectFGArray[3];
		if (RectFGArray[3] < RectFGArray[2])
			FG_min_length = RectFGArray[2];
		FG_min_length = FG_min_length/ (2 * CamSizeRatio); // only expecting 50 % of rows to give an edge point.

		//for (int j = 0; j < 4; j++)
		//	RectFGArray[j] *= CamSizeRatio;

		int noofpts = 0;
		if (MAINDllOBJECT->getSurface())
		{
			//pixel_neighbour = EDSCOBJECT->Pixel_Neighbour;
			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				if (isFirstFrame) 
				{
					noofpts = EDSCOBJECT->DetectSurfaceEdge_Peak(RectFGArray, SurfaceAlgoParam[0], SurfaceAlgoParam[1], SurfaceAlgoParam[2], SurfaceAlgoParam[3], SurfaceAlgoParam[4], true);
				}
				else
				{
					if(EDSCOBJECT->ApplySubAlgorithmOfType6 && !skip_cont_constraint)
					{
						int startC =  (ProfileScanEnd_Pt.X - MAINDllOBJECT->getWindow(0).getCam().x) * CamSizeRatio / MAINDllOBJECT->getWindow(0).getUppX();
						int startR =  (MAINDllOBJECT->getWindow(0).getCam().y - ProfileScanEnd_Pt.Y) * CamSizeRatio / MAINDllOBJECT->getWindow(0).getUppY();
						noofpts = EDSCOBJECT->DetectSurfaceEdge_Peak(RectFGArray, SurfaceAlgoParam[0], SurfaceAlgoParam[1], SurfaceAlgoParam[2], SurfaceAlgoParam[3], SurfaceAlgoParam[4], LastDir, startR, startC);
					}
					else if(skip_cont_constraint)
					{
						int startC =  (ProfileScanEnd_Pt.X - MAINDllOBJECT->getWindow(0).getCam().x) / MAINDllOBJECT->getWindow(0).getUppX();
						int startR =  (MAINDllOBJECT->getWindow(0).getCam().y - ProfileScanEnd_Pt.Y) / MAINDllOBJECT->getWindow(0).getUppY();
						if((RectFGArray[0] < startC) && ((RectFGArray[0] + RectFGArray[2]) > startC) && (RectFGArray[1] < startR) && ((RectFGArray[1] + RectFGArray[3]) > startR))
							noofpts = EDSCOBJECT->DetectSurfaceEdge_Peak(RectFGArray, SurfaceAlgoParam[0], SurfaceAlgoParam[1], SurfaceAlgoParam[2], SurfaceAlgoParam[3], SurfaceAlgoParam[4], LastDir, startR, startC);
						else
							noofpts = EDSCOBJECT->DetectSurfaceEdge_Peak(RectFGArray, SurfaceAlgoParam[0], SurfaceAlgoParam[1], SurfaceAlgoParam[2], SurfaceAlgoParam[3], SurfaceAlgoParam[4], true);
					}
					else
					   noofpts = EDSCOBJECT->DetectSurfaceEdge_Peak(RectFGArray, SurfaceAlgoParam[0], SurfaceAlgoParam[1], SurfaceAlgoParam[2], SurfaceAlgoParam[3], SurfaceAlgoParam[4], true);
				}
			}
			else
			{
				if (isFirstFrame) 
				{
					noofpts = EDSCOBJECT->DetectSurfaceEdge(RectFGArray, EdgeDetectionSingleChannel::ScanDirection::Rightwards, EdgeDetectionSingleChannel::ComponentType(MAINDllOBJECT->SelectedAlgorithm()));
				}
				else
				{
					if(MAINDllOBJECT->SelectedAlgorithm() == 5 && EDSCOBJECT->ApplySubAlgorithmOfType6 && !skip_cont_constraint)
					{
					   int startC =  (ProfileScanEnd_Pt.X - MAINDllOBJECT->getWindow(0).getCam().x) / MAINDllOBJECT->getWindow(0).getUppX();
					   int startR =  (MAINDllOBJECT->getWindow(0).getCam().y - ProfileScanEnd_Pt.Y) / MAINDllOBJECT->getWindow(0).getUppY();
					   noofpts = EDSCOBJECT->DetectSurfaceEdge(RectFGArray, LastDir, EdgeDetectionSingleChannel::ComponentType(MAINDllOBJECT->SelectedAlgorithm()), startR, startC);
					}
					else if(MAINDllOBJECT->SelectedAlgorithm() == 5 && skip_cont_constraint)
					{
						int startC =  (ProfileScanEnd_Pt.X - MAINDllOBJECT->getWindow(0).getCam().x) * CamSizeRatio / MAINDllOBJECT->getWindow(0).getUppX();
						int startR =  (MAINDllOBJECT->getWindow(0).getCam().y - ProfileScanEnd_Pt.Y) * CamSizeRatio / MAINDllOBJECT->getWindow(0).getUppY();
						if((RectFGArray[0] < startC) && ((RectFGArray[0] + RectFGArray[2]) > startC) && (RectFGArray[1] < startR) && ((RectFGArray[1] + RectFGArray[3]) > startR))
							 noofpts = EDSCOBJECT->DetectSurfaceEdge(RectFGArray, LastDir, EdgeDetectionSingleChannel::ComponentType(MAINDllOBJECT->SelectedAlgorithm()), startR, startC);
						else
							 noofpts = EDSCOBJECT->DetectSurfaceEdge(RectFGArray, LastDir, EdgeDetectionSingleChannel::ComponentType(MAINDllOBJECT->SelectedAlgorithm()), true);
					}
					else
					{
					   noofpts = EDSCOBJECT->DetectSurfaceEdge(RectFGArray, LastDir, EdgeDetectionSingleChannel::ComponentType(MAINDllOBJECT->SelectedAlgorithm()), true);
					}
				}
			}
			if( (noofpts < MAINDllOBJECT->MinimumPointsInEdge()) && (noofpts < FG_min_length) && (!AutoScanDone))
			{
				AutoScanDone = true;
				CurrFramePointslist.deleteAll();
				MAINDllOBJECT->DoFocusProfile();
				return;
			}
		}
		else
		{
			int edgedetectionJmp = jump / 2;
			if (edgedetectionJmp < 2)
				edgedetectionJmp = 2;
			if (edgedetectionJmp > 5)
				edgedetectionJmp = 5;
			noofpts = EDSCOBJECT->DetectAllProfileEdges(&RectFGArray[0], edgedetectionJmp);
		}
		//if (noofpts < 200) //If less than 200 try in other direction
		//	noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Downwards);
		if ( (noofpts < MAINDllOBJECT->MinimumPointsInEdge()) && (noofpts < FG_min_length))
		{
				profile_scan_failed = true;
				profile_scan_running = false;
				CurrFramePointslist.deleteAll();
				MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				MAINDllOBJECT->ChangeCncMode(2);
				return;
		}

		//Convert to DRO Coordinates
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / CamSizeRatio;
		double cm_x = MAINDllOBJECT->getWindow(0).getCenter().x, cm_y = MAINDllOBJECT->getWindow(0).getCenter().y;
		
		memcpy (edgepoints, EDSCOBJECT->DetectedPointsList, 2*(noofpts)*sizeof(double));

		double x_pos = cx + edgepoints[0] * upp;
		double y_pos = cy - edgepoints[1] * upp;
		double x_prev = x_pos;
		double y_prev = y_pos;
		double extremes[4] = {cx, cy, cx + MAINDllOBJECT->getWindow(0).getWinDim().x * CamSizeRatio * upp , cy - MAINDllOBJECT->getWindow(0).getWinDim().y * CamSizeRatio * upp};
		
		int n;
		//empty current frame points list.
		CurrFramePointslist.deleteAll();
		
		//isFirstFrame will be true if profile scan is being started on a new shape.  
		//If so, start with first point of edge and proceed till we reach either the start point
		//or an end point close to frame boundary
		if (isFirstFrame) 
		{
			//set isFirstFrame to false so that subsequent iterations to this function will reach the "else" block
			isFirstFrame = false;

			double x_0 = cx + edgepoints[0] * upp;
			double y_0 = cy - edgepoints[1] * upp;
			if (((!PPCALCOBJECT->IsPartProgramRunning())&&(!profileScanEndPtPassed)) || Scan_Complete_Profile)
			{
				//set start point as first point of edge in first frame
				ProfileScanStart_Pt.X = x_0;
				ProfileScanStart_Pt.Y = y_0;
			}
			x_prev = x_0;
			y_prev = y_0;
			PrevFramePointslist.deleteAll();
			for( n = 0; n < noofpts; n++)
			{
				if ((edgepoints[2*n] == -1) && (edgepoints[2*n + 1] == -1))
					continue;
				x_pos = cx + edgepoints[2*n] * upp;
				y_pos = cy - edgepoints[2*n + 1] * upp;

				if (chk_for_cont) 
				{
					if ((abs(x_pos - x_prev) > pixel_neighbour*upp)||(abs(y_pos - y_prev) > pixel_neighbour*upp))
					{
						ProfileScanEnd_Pt.X = x_prev;
						ProfileScanEnd_Pt.Y = y_prev;
						break;
					}
				}
				x_prev = x_pos;
				y_prev = y_pos;
				//add points to CurrFramePointslist 
				CurrFramePointslist.Addpoint(new SinglePoint(x_pos, y_pos, MAINDllOBJECT->getCurrentDRO().getZ()));
				if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
					MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT || 
					MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
					continue;

				//check if point is away from start point (by 10 pixels or more)
				if ((abs(x_pos - x_0) > upp * 10) || (abs(y_pos - y_0) > upp * 10))
				{
					//if we reach point that falls near extreme of current frame, mark this as end point and break.
					if ((abs(x_pos - extremes[0]) < upp * 10) || (abs(x_pos - extremes[2]) < upp * 10) ||
							(abs(y_pos - extremes[1]) < upp * 10) || (abs(y_pos - extremes[3]) < upp * 10))
					{
						ProfileScanEnd_Pt.X = x_pos;
						ProfileScanEnd_Pt.Y = y_pos;
						break;
					}
				}
			}

			//if n== noofpts, check if last point is close to start point (within 20 pixels).  If yes, set profile_scan_ended to true.
			//else set end point of current frame to last point.
			//Apr 29 2013: make this check more stringent (within limit_pixel_ct which is 5 pixels for first iteration) 
			//because otherwise it ends up detecting spurious profile scan ended.
			if (n>=noofpts)
			{
				if ( (abs(x_pos - ProfileScanStart_Pt.X) < upp * limit_pixel_ct) && (abs(y_pos - ProfileScanStart_Pt.Y) < upp * limit_pixel_ct)
							&& (abs(x_pos - extremes[0]) > upp * limit_pixel_ct) && (abs(x_pos - extremes[2]) > upp * limit_pixel_ct)
							&& (abs(y_pos - extremes[1]) > upp * limit_pixel_ct) && (abs(y_pos - extremes[3]) > upp * limit_pixel_ct))
				{
					profile_scan_ended = true;
				}
				else
				{
					ProfileScanEnd_Pt.X = x_pos;
					ProfileScanEnd_Pt.Y = y_pos;
				}
			}

			double sq_dist1 = pow(profileScanPixelPt[0] * CamSizeRatio - edgepoints[0], 2) + pow(profileScanPixelPt[1] * CamSizeRatio - edgepoints[1],2);
			double sq_dist2 = pow(profileScanPixelPt[0] * CamSizeRatio - edgepoints[2*noofpts-2], 2) + pow(profileScanPixelPt[1] * CamSizeRatio - edgepoints[2*noofpts-1],2);
			if (sq_dist2 < sq_dist1)
			{
				SinglePoint tmppt = ProfileScanStart_Pt;
				ProfileScanStart_Pt = ProfileScanEnd_Pt;
				ProfileScanEnd_Pt = tmppt;
				reversal = true;
			}
			first_frame_pts_cnt = 0;
		}
		//following block of code is reached for profile scan being called in subsequent iterations.
		//here we first identify the point amongst the edge points that is close to previous end point and then 
		//identify subsequent edge points of this frame till we reach either start point or frame boundary.
		else
		{
			profile_scan_iter++;
			//following flag will determine direction along which to traverse along edge once we reach previous edge point.
			int found_backward_match = 0;

			double x_pos_start = cx + edgepoints[0]* upp;
			double y_pos_start  = cy - edgepoints[1] * upp;
			double x_pos_end = cx + edgepoints[2*noofpts-2] * upp;
			double y_pos_end = cy - edgepoints[2*noofpts-1] * upp;
			
			int m = 0;
			bool found_start_in_prev_pts_list = false;
			bool found_end_in_prev_pts_list = false;

			int tmpCount = PrevFramePointslist.Pointscount();
			map<int,SinglePoint*>& map_prev_pts = PrevFramePointslist.getList();

			bool detected_direcn = false;
//			bool break_from_loop = false;
			int n_match = 0;
			for (n = 0; n < noofpts; n++)
			{
				//				break_from_loop = false;
				if ((edgepoints[2 * n] == -1) && (edgepoints[2 * n + 1] == -1))
					continue;
				x_pos = cx + edgepoints[2 * n] * upp;
				y_pos = cy - edgepoints[2 * n + 1] * upp;
				double x_pos_before_chk = x_pos;
				double y_pos_before_chk = y_pos;
				double x_pos_after_chk = x_pos;
				double y_pos_after_chk = y_pos;
				if ((abs(x_pos - ProfileScanEnd_Pt.X) < upp * pixel_neighbour) && (abs(y_pos - ProfileScanEnd_Pt.Y) < upp * pixel_neighbour))
				{
					n_match = n;
					double x_pos_min = x_pos;
					double y_pos_min = y_pos;
					for (int a = 1; a < 2 * pixel_neighbour; a++)
					{
						if (n + a == noofpts)
							break;
						if ((edgepoints[2 * (n + a)] == -1) && (edgepoints[2 * (n + a) + 1] == -1))
							break;
						x_pos = cx + edgepoints[2 * (n + a)] * upp;
						y_pos = cy - edgepoints[2 * (n + a) + 1] * upp;
						if ((abs(x_pos - ProfileScanEnd_Pt.X) <= abs(x_pos_min - ProfileScanEnd_Pt.X)) && (abs(y_pos - ProfileScanEnd_Pt.Y) <= abs(y_pos_min - ProfileScanEnd_Pt.Y)))
						{
							n_match = n + a;
							x_pos_min = x_pos;
							y_pos_min = y_pos;
						}
					}
					x_pos = x_pos_min;
					y_pos = y_pos_min;
					n = n_match;
					//if ((n > 20) && (n < noofpts - 20))
					//{
					int chk = 0;
					x_prev = x_pos;
					y_prev = y_pos;
					for (chk = 1; chk <= n; chk++)
					{
						if ((edgepoints[2 * (n - chk)] == -1) && (edgepoints[2 * (n - chk) + 1] == -1))
						{
							chk--;
							break;
						}
						x_pos_before_chk = cx + edgepoints[2 * (n - chk)] * upp;
						y_pos_before_chk = cy - edgepoints[2 * (n - chk) + 1] * upp;
						if ((abs(x_pos_before_chk - x_prev) > pixel_neighbour*upp) || (abs(y_pos_before_chk - y_prev) > pixel_neighbour*upp))
						{
							chk--;
							break;
						}
						if ((abs(x_pos_before_chk - ProfileScanStart_Pt.X) < pixel_neighbour*upp) && (abs(y_pos_before_chk - ProfileScanStart_Pt.Y) < pixel_neighbour*upp))
						{
							chk--;
							break;
						}
						x_prev = x_pos_before_chk;
						y_prev = y_pos_before_chk;
						//if (chk >= MAINDllOBJECT->MinimumPointsInEdge()/2)
						//if (chk >= noofpts / 2)
						//	break;
					}
					if (chk >= n)
						chk = n;
					//if (chk == 0)
					//	break;
					x_pos_before_chk = cx + edgepoints[2 * (n - chk)] * upp;
					y_pos_before_chk = cy - edgepoints[2 * (n - chk) + 1] * upp;

					int start_z = 0;
					if (profile_scan_iter > 3)
						start_z = MAINDllOBJECT->MinimumPointsInEdge() / 2;
					int detected_z = 0;
					for (int z = start_z; z < tmpCount; z++)
					{
						if ((abs(x_pos_before_chk - map_prev_pts[z]->X) < upp * pixel_neighbour) && (abs(y_pos_before_chk - map_prev_pts[z]->Y) < upp * pixel_neighbour))
						{
							m = 0;
							found_backward_match = 1;
							detected_direcn = true;
							detected_z = z;
							break;
						}
					}

					//if (detected_direcn == false)
					//{
					chk = 0;
					x_prev = x_pos;
					y_prev = y_pos;
					for (chk = 1; chk < noofpts - n; chk++)
					{
						if ((edgepoints[2 * (n + chk)] == -1) && (edgepoints[2 * (n + chk) + 1] == -1))
						{
							chk--;
							break;
						}
						x_pos_after_chk = cx + edgepoints[2 * (n + chk)] * upp;
						y_pos_after_chk = cy - edgepoints[2 * (n + chk) + 1] * upp;
						if ((abs(x_pos_after_chk - x_prev) > pixel_neighbour*upp) || (abs(y_pos_after_chk - y_prev) > pixel_neighbour*upp))
						{
							chk--;
							break;
						}
						if ((abs(x_pos_after_chk - ProfileScanStart_Pt.X) < pixel_neighbour*upp) && (abs(y_pos_after_chk - ProfileScanStart_Pt.Y) < pixel_neighbour*upp))
						{
							chk--;
							break;
						}
						x_prev = x_pos_after_chk;
						y_prev = y_pos_after_chk;
						//if (chk >= MAINDllOBJECT->MinimumPointsInEdge()/2)
						//if (chk >= noofpts / 2)
						//	break;
					}
					if (chk >= noofpts - n - 1)
						chk = noofpts - n - 1;
					//if (chk == 0)
					//	break;
					x_pos_after_chk = cx + edgepoints[2 * (n + chk)] * upp;
					y_pos_after_chk = cy - edgepoints[2 * (n + chk) + 1] * upp;
					for (int z = start_z; z < tmpCount; z++)
					{
						if ((abs(x_pos_after_chk - map_prev_pts[z]->X) < upp * pixel_neighbour) && (abs(y_pos_after_chk - map_prev_pts[z]->Y) < upp * pixel_neighbour))
						{
							//if (detected_direcn == true)
							//	detected_direcn = false;
							//else
							//{
							//	m = noofpts - 1;
							//	found_backward_match = -1;
							//	detected_direcn = true;
							//}
							if ((detected_direcn == true) && (abs(z - detected_z) < MAINDllOBJECT->MinimumPointsInEdge() / 2))
								detected_direcn = false;
							else
							{
								if ((detected_direcn == false) || (z < detected_z))
								{
									m = noofpts - 1;
									found_backward_match = -1;
								}
								detected_direcn = true;
							}
							break;
						}
					}
					//}
					if (detected_direcn == true)
						break;
					chk = chk;
					//if (break_from_loop == true)
					//	break;
				//}
				}
			}

			if ((detected_direcn == false) && ((chk_for_cont == false)||(skip_cont_constraint == true)))
			{
				double sq_dist1 = pow(profileScanPixelPt[0] * CamSizeRatio - edgepoints[0], 2) + pow(profileScanPixelPt[1] * CamSizeRatio - edgepoints[1],2);
				double sq_dist2 = pow(profileScanPixelPt[0] * CamSizeRatio - edgepoints[2*noofpts-2], 2) + pow(profileScanPixelPt[1] * CamSizeRatio - edgepoints[2*noofpts-1],2);
				double start_end_dist = sqrt(pow((x_pos_start - x_pos_end), 2) + pow((y_pos_start - y_pos_end),2));
				if (start_end_dist < upp * 20)
				{
					profile_scan_failed = true;
					profile_scan_running = false;
					//call appropriate handler to indicate profile scan failure
					MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					MAINDllOBJECT->ChangeCncMode(2);
					return;
				}
				int a = 0;
				int b = 0;
				for (a = 0; a < tmpCount; a++)
				{
					if ((abs(x_pos_start - map_prev_pts[a]->X) < upp * pixel_neighbour) && (abs(y_pos_start - map_prev_pts[a]->Y) < upp * pixel_neighbour))
					{
						found_start_in_prev_pts_list = true;
						break;
					}
				}
				for (b = 0; b < tmpCount; b++)
				{
					if ((abs(x_pos_end - map_prev_pts[b]->X) < upp * pixel_neighbour) && (abs(y_pos_end - map_prev_pts[b]->Y) < upp * pixel_neighbour))
					{
						found_end_in_prev_pts_list = true;
						break;
					}
				}

				if ((found_start_in_prev_pts_list) && (!found_end_in_prev_pts_list))
				{
						m = 0;
						found_backward_match = 1;
				}
				else if ((!found_start_in_prev_pts_list) && (found_end_in_prev_pts_list))
				{
						m = noofpts - 1;
						found_backward_match = -1;
				}
				else 
				{
					if (sq_dist1 < sq_dist2)
					{
						m = 0;
						found_backward_match = 1;
					}
					else
					{
						m = noofpts - 1;
						found_backward_match = -1;
					}
				}
				detected_direcn = true;
			}

			if (detected_direcn == false)
			{
				profile_scan_failed = true;
				profile_scan_running = false;
				skip_cont_constraint = true;
				//call appropriate handler to indicate profile scan failure
				MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				MAINDllOBJECT->ChangeCncMode(2);
				return;
			}

			//get CurrFramePointsList
			for (n = 0; n < noofpts; n++ )
			{
				if ((edgepoints[2*n] == -1) && (edgepoints[2*n + 1] == -1))
					continue;
				if ((chk_for_cont == true) && (skip_cont_constraint == false))
				{
					if (n < n_match)
						continue;
				}
				x_pos = cx + edgepoints[2*n] * upp;
				y_pos = cy - edgepoints[2*n + 1] * upp;
				x_prev = x_pos;
				y_prev = y_pos;
				double x_pos_min = x_pos;
				double y_pos_min = y_pos;
				//check if point is near to prev end point within a tolerance . 
				if ((abs(x_pos - ProfileScanEnd_Pt.X) < upp * pixel_neighbour) && (abs(y_pos - ProfileScanEnd_Pt.Y) < upp * pixel_neighbour))
				{
					int k = n;

					//traverse new points in forward direction if match is found behind, else traverse in reverse direction
					while ((k < noofpts) && ( k >= 0)) 
					{
						if ((edgepoints[2*k] == -1) && (edgepoints[2*k + 1] == -1))
						{
							ProfileScanEnd_Pt.X = x_prev;
							ProfileScanEnd_Pt.Y = y_prev;
							break;
						}
						x_pos = cx + edgepoints[2*k] * upp;
						y_pos = cy - edgepoints[2*k + 1] * upp;

						if (chk_for_cont)
						{
							if ((abs(x_pos - x_prev) > pixel_neighbour*upp)||(abs(y_pos - y_prev) > pixel_neighbour*upp))
							{
								ProfileScanEnd_Pt.X = x_prev;
								ProfileScanEnd_Pt.Y = y_prev;
								break;
							}
						}
						x_prev = x_pos;
						y_prev = y_pos;
						if (profile_scan_ended)
						{
							if ((abs(x_pos - ProfileScanStart_Pt.X) <= abs(x_pos_min - ProfileScanStart_Pt.X)) && 
								(abs(y_pos - ProfileScanStart_Pt.Y) <= abs(y_pos_min - ProfileScanStart_Pt.Y)))
							{
								x_pos_min = x_pos;
								y_pos_min = y_pos;
							}
							else
								break;
						}
						//add points to CurrFramePointslist 
						CurrFramePointslist.Addpoint(new SinglePoint(x_pos,y_pos, MAINDllOBJECT->getCurrentDRO().getZ()));
						
						//check if point is near start point (within limit_pixel_ct pixels)
						if (((abs(x_pos - ProfileScanStart_Pt.X) < upp * limit_pixel_ct) && (abs(y_pos - ProfileScanStart_Pt.Y) < upp * limit_pixel_ct)) &&
							(profile_scan_iter > 2))
						{
							profile_scan_ended = true;
							x_pos_min = x_pos;
							y_pos_min = y_pos;
						}
						//if we reach point that falls near extreme of current frame, 
						//mark this as end point and break.
						if ((abs(x_pos - extremes[0]) < upp * 10) || (abs(x_pos - extremes[2]) < upp * 10) ||
								(abs(y_pos - extremes[1]) < upp * 10) || (abs(y_pos - extremes[3]) < upp * 10))
						{
							ProfileScanEnd_Pt.X = x_pos;
							ProfileScanEnd_Pt.Y = y_pos;
							break;
						}
						k += found_backward_match ;
					}

					//if we did not reach edge or starting point it means profile scan ended in between.
					//treat last point as end point and continue
					if ((k>=noofpts)||(k<=-1))
					{
						ProfileScanEnd_Pt.X = x_pos;
						ProfileScanEnd_Pt.Y = y_pos;
					}
					//if we have reached till here, it means valid edge has been detected in current frame.
					//so break and proceed with adding currframepointslist to profile scan shape list
					break;
				}
				//if point not near previous end point, proceed to next point and check again.
			}

			if ((n==noofpts)&&((chk_for_cont)&&(!skip_cont_constraint)))
			{
				profile_scan_failed = true;
				profile_scan_running = false;
				skip_cont_constraint = true;
				//call appropriate handler to indicate profile scan failure
				MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				MAINDllOBJECT->ChangeCncMode(2);
				return;
			}
			//if we did not find previous end point in current frame, go ahead and take all points of edge as part of profile scan.
			if (n==noofpts)
			{
				x_pos = cx + edgepoints[2*m] * upp;
				y_pos = cy - edgepoints[2*m + 1] * upp;
				x_prev = x_pos;
				y_prev = y_pos;
				double x_pos_min = x_pos;
				double y_pos_min = y_pos;
				while ((m < noofpts) && ( m >= 0)) 
				{
					if ((edgepoints[2*m] == -1) && (edgepoints[2*m + 1] == -1))
					{
						m += found_backward_match;
						continue;
					}
					x_pos = cx + edgepoints[2*m] * upp;
					y_pos = cy - edgepoints[2*m + 1] * upp;

					if (chk_for_cont)
					{
						if ((abs(x_pos - x_prev) > pixel_neighbour*upp)||(abs(y_pos - y_prev) > pixel_neighbour*upp))
						{
							ProfileScanEnd_Pt.X = x_prev;
							ProfileScanEnd_Pt.Y = y_prev;
							break;
						}
					}
					x_prev = x_pos;
					y_prev = y_pos;

					//check if point is near start point (within limit_pixel_ct pixels)
					if (profile_scan_ended)
					{
						if ((abs(x_pos - ProfileScanStart_Pt.X) <= abs(x_pos_min - ProfileScanStart_Pt.X)) && 
							(abs(y_pos - ProfileScanStart_Pt.Y) <= abs(y_pos_min - ProfileScanStart_Pt.Y)))
						{
							x_pos_min = x_pos;
							y_pos_min = y_pos;
						}
						else
							break;
					}
					//add points to CurrFramePointslist 
					CurrFramePointslist.Addpoint(new SinglePoint(x_pos, y_pos, MAINDllOBJECT->getCurrentDRO().getZ()));
					if ((abs(x_pos - ProfileScanStart_Pt.X) < upp * limit_pixel_ct) && (abs(y_pos - ProfileScanStart_Pt.Y) < upp * limit_pixel_ct))
					{
						profile_scan_ended = true;
						x_pos_min = x_pos;
						y_pos_min = y_pos;
					}
					//if we reach point that falls near extreme of current frame, 
					//mark this as end point and break.
					if ((abs(x_pos - extremes[0]) < upp * 10) || (abs(x_pos - extremes[2]) < upp * 10) ||
							(abs(y_pos - extremes[1]) < upp * 10) || (abs(y_pos - extremes[3]) < upp * 10))
					{
						ProfileScanEnd_Pt.X = x_pos;
						ProfileScanEnd_Pt.Y = y_pos;
						break;
					}
					m += found_backward_match;
				}
				//if we did not reach edge or starting point it means profile scan ended in between.
				//treat last point as end point and continue
				if ((m>=noofpts)||(m<=-1))
				{
					ProfileScanEnd_Pt.X = x_pos;
					ProfileScanEnd_Pt.Y = y_pos;
				}
			}
		}

		int tmpcount = CurrFramePointslist.Pointscount(); 
		map<int,SinglePoint*>& map_curr_pts = CurrFramePointslist.getList();

		ProfileScanPrevStart_Pt.X = map_curr_pts[0]->X;
		ProfileScanPrevStart_Pt.Y = map_curr_pts[0]->Y;

		int m = 0;
		int found_backward_match = 1;
		int i = 0;
		int i_after_jump = 0;
		if (reversal)
		{
			m = tmpcount - 1;
			found_backward_match = -1;
			ProfileScanPrevStart_Pt.X = map_curr_pts[tmpcount - 1]->X;
			ProfileScanPrevStart_Pt.Y = map_curr_pts[tmpcount - 1]->Y;
		}

		double dist = 0;
		ShapeWithList* Cshape = (ShapeWithList*) CurrentProfileCpShape;
		if(Cshape != NULL)
		{
			tmpPointslist.deleteAll();
			i = m;
			int currshape_ptssize = Cshape->PointsList->Pointscount();
			map<int,SinglePoint*>& currshape_ptslist = Cshape->PointsList->getList();
			bool to_be_included = true;
			SinglePoint* ptr_pt;
			int a = 0;
			int b = 0;
			int shape_forw_match = 1;
			double prevx = 0, prevy = 0;
			double currx = 0, curry = 0;
			double mind = jump * upp;
			double *tmparray = new double[tmpcount * 2];
			int counter = 0;
			while ((i < tmpcount) && ( i >= 0))
			{
				if (i_after_jump == 0)
				{
					tmpPointslist.Addpoint(new SinglePoint(map_curr_pts[i]->X,map_curr_pts[i]->Y, MAINDllOBJECT->getCurrentDRO().getZ()));
					tmparray[counter++] = map_curr_pts[i]->X;
					tmparray[counter++] = map_curr_pts[i]->Y;
					prevx = map_curr_pts[i]->X;
					prevy = map_curr_pts[i]->Y;
				}
				PrevFramePointslist.Addpoint(new SinglePoint(map_curr_pts[i]->X,map_curr_pts[i]->Y, MAINDllOBJECT->getCurrentDRO().getZ()));
				currx = map_curr_pts[i]->X;
				curry = map_curr_pts[i]->Y;
				i += found_backward_match;
				i_after_jump++;
				if ((abs(currx - prevx) > mind) || (abs(curry - prevy) > mind))
					i_after_jump = 0;
			}
			if(counter / 2 > 2)
			{
				double ans[3] = {0};
				BESTFITOBJECT->Line_BestFit_2D(tmparray, counter /2, ans, true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				if(ans[0] < 0)
					ans[0] += M_PI;
				else if(ans[0] > M_PI) 
					ans[0] -= M_PI;
				if(ans[0] > 0.7854 && ans[0] < 2.3562)
					LastDir = EdgeDetectionSingleChannel::ScanDirection::Rightwards;
				else
					LastDir = EdgeDetectionSingleChannel::ScanDirection::Upwards;
			}
			delete [] tmparray;
			Cshape->AddPoints(&tmpPointslist);
		}
		else
		{
			profile_scan_failed = true;
			profile_scan_running = false;
			tmpPointslist.deleteAll();
			PrevFramePointslist.deleteAll();
			CurrFramePointslist.deleteAll();
			//call appropriate handler to indicate profile scan failure
			MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			MAINDllOBJECT->ChangeCncMode(2);
			return;
		}

		if (profile_scan_iter == 1)
			first_frame_pts_cnt = PrevFramePointslist.Pointscount();
		SinglePoint goto_pt;
		
		goto_pt.X = ProfileScanEnd_Pt.X;
		goto_pt.Y = ProfileScanEnd_Pt.Y;
		
		//making profile scan a bit faster by moving end point to end of frame instead of middle of frame
		double curDroX = MAINDllOBJECT->getCurrentDRO().getX();
		double curDroY = MAINDllOBJECT->getCurrentDRO().getY();
		double deltaX = goto_pt.X - curDroX;
		double deltaY = goto_pt.Y - curDroY;
		if ((abs(deltaX) < 10*upp) && (abs(deltaY) < 10 *upp))
		{
			if (deltaX > 0)
				deltaX = 10 * upp;
			else
				deltaX = -10 * upp;
			if (deltaY > 0)
				deltaY = 10 * upp;
			else
				deltaY = -10 * upp;
			goto_pt.X = curDroX + deltaX;
			goto_pt.Y = curDroY + deltaY;
		}
		if(MAINDllOBJECT->getSurface())
		{
			goto_pt.X += deltaX * MAINDllOBJECT->ProfileScanMoveToNextFrameFactor;
			goto_pt.Y += deltaY * MAINDllOBJECT->ProfileScanMoveToNextFrameFactor;
		}
		else
		{
			goto_pt.X += deltaX * MAINDllOBJECT->ProfileScanMoveToNextFrameFactor;
			goto_pt.Y += deltaY * MAINDllOBJECT->ProfileScanMoveToNextFrameFactor;
		}
		//end of changes to make profile scan faster

		//Erase currFramePointslist
		CurrFramePointslist.deleteAll();
		tmpPointslist.deleteAll();

		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			profile_scan_ended = true;

		profile_scan_running = false;

		if (profile_scan_ended)
		{
			profile_scan_paused = false;
			profile_scan_stopped = false;
			PrevFramePointslist.deleteAll();
			//MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			if(profileScanEndPtPassed)
				ProfileScanFinished(false);
			else
				ProfileScanFinished();
			//call appropriate handler to indicate end of profile scan
		}
		else
		{
			 double GotoPos[3] = {goto_pt.X, goto_pt.Y, MAINDllOBJECT->getCurrentDRO().getZ()};
			//if ((abs(MAINDllOBJECT->getCurrentDRO().getX() - goto_pt.X) < 10 * upp) && (abs(MAINDllOBJECT->getCurrentDRO().getY() - goto_pt.Y) < 10 * upp))
			//{
			//	profile_scan_failed = true;
			//	profile_scan_running = false;
			//	skip_cont_constraint = true;
			//	//call appropriate handler to indicate profile scan failure
			//	MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			//	return;
			//}
			/*DWORD d1, d2, d3;
			d1 = GetTickCount();*/
			MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
			/*d2 = GetTickCount();
			d3 = d2 - d1;
			d3 += 1;*/
			
			if(!profile_scan_stopped && !profile_scan_paused)
			{
				AutoScanDone = false;
				MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::PROFILESCANGOTO);
			}//call appropriate CNC move handler using goto_pt as parameter.
		}
		skip_cont_constraint = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0027", __FILE__, __FUNCSIG__); }
}

void HelperClass::ProfileScanFinished(bool CompleteProfile)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "ProfileScanFinished", __FUNCSIG__);
	try
	{
		AutoScanDone = false;
		MAINDllOBJECT->PointsTakenProfileScan(1);
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			if(!AutoProfileScanEnable || MAINDllOBJECT->getSurface())
				MAINDllOBJECT->ShowMsgBoxString("HelperClassProfileScan04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		}			
		if(CurrentProfScanMode == RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_NORMAL)
		{
		}
		else if(CurrentProfScanMode == RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_AUTOTHREAD)
		{
		}
		else if(CurrentProfScanMode == RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_LINEARCACT)
		{
			ProfileScanLineArcAction = new LineArcAction(profile_scan_auto_thread);
			if (!profile_scan_auto_thread)
			{
				MAINDllOBJECT->getActionsHistoryList().addItem(ProfileScanLineArcAction);
			}
			((LineArcAction*)ProfileScanLineArcAction)->lineArcSep(CurrentProfileCpShape);
			//CurrentProfileCpShape->ResetCurrentParameters();
		}
		else // if nothing
		{
		}
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			PPCALCOBJECT->partProgramReached();
		}
		else
		{
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL && CompleteProfile || profileScanEndPtPassed)
			{
				bool SurfaceLightFlag = false;
				if(MAINDllOBJECT->getSurface())
					SurfaceLightFlag = true;
				std::string CamProperty = MAINDllOBJECT->getCamSettings();
				std::string LightProperty = MAINDllOBJECT->getLightProperty();
				int SurfaceAlgoParameter[5] = {EDSCOBJECT->Type6_MaskFactor, EDSCOBJECT->Type6_Binary_LowerFactor, EDSCOBJECT->Type6_Binary_UpperFactor, EDSCOBJECT->Type6_PixelThreshold, EDSCOBJECT->Type6_MaskingLevel};
				ProfileScanAction::AddProfileScanAction(CShape, ProfileScanFirstPt, ProfileScanSecondPt, ProfileLastPtVector, ProfileScanDroPt, CompleteProfile, ProfileScanJumpParam, LightProperty , CamProperty, SurfaceAlgoParameter, SurfaceLightFlag);
				if(MultiFeatureScan)
				{
					this->MultiFeatureShapeList.clear();
					this->MultiFeatureShapeList.push_back(CShape->getId());
					MAINDllOBJECT->CallDxfImport();
				}
				if(AutoProfileScanEnable)
				{
					AutoProfileScanEnable = false;
					MAINDllOBJECT->CreateNextProfileScan();
				}
			}
			profileScanEndPtPassed = false;
			MAINDllOBJECT->ChangeCncMode(2);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0027", __FILE__, __FUNCSIG__); }
}

bool HelperClass::CheckCurrentFocus_ProfileScan(int Number_Pts)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CheckCurrentFocus_ProfileScan", __FUNCSIG__);
	try
	{
		if(MAINDllOBJECT->getSurface()) return false;
		MAINDllOBJECT->SetImageInEdgeDetection();
		//int RectFGArray[4] = {0, 0, MAINDllOBJECT->getWindow(0).getWinDim().x, MAINDllOBJECT->getWindow(0).getWinDim().y};
		int skipWidth = MAINDllOBJECT->SkipPixelWidth(), skipHeight = MAINDllOBJECT->SkipPixelHeight();
		int RectFGArray[4] = { skipWidth, skipHeight, MAINDllOBJECT->getWindow(0).getWinDim().x - (2 * skipWidth),
			MAINDllOBJECT->getWindow(0).getWinDim().y - (2 * skipHeight) };

		int noofpts = 0;
		//noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Rightwards);
		//if(noofpts < Number_Pts) //If less than 400 try in other direction
		//	noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Downwards);	
		//if (noofpts < Number_Pts) //if still less, try the all edge detection algorithm, the one used actually in profile scan
			noofpts = EDSCOBJECT->DetectAllProfileEdges(&RectFGArray[0]);
		if(noofpts < Number_Pts) return false;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0028", __FILE__, __FUNCSIG__); return false;}
}

void HelperClass::ClearAllFlags()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "ClearAllFlags", __FUNCSIG__);
	try
	{
		RotationScanFlag = false;
		SurfaceAlgoType = 1;
		profileScanPtsCnt = 0;
		AutoProfileScanEnable = false;
		MultiFeatureScan = false;
		isFirstFrame = false;
		StartProfileScan = false;
		PickEndPoint_ProfileScan = false;
		profileScanEndPtPassed = false;
		profile_scan_running = false;
		profile_scan_failed = false;
		profile_scan_paused = false;
		profile_scan_stopped = false;
		Scan_Complete_Profile = false;
		this->ProfileScanLineArcAction = NULL;
		CurrentProfScanMode = RapidEnums::RAPIDPROFILESCANMODE::PROFILESCAN_NOTHING;
		SptCount = 0;
		ProfileScanJumpParam = -1;
		profileScanPixelPt[0] = 0;
		profileScanPixelPt[1] = 0;
		profileScanPixelPt[2] = MAINDllOBJECT->getWindow(0).getWinDim().x;
		profileScanPixelPt[3] = MAINDllOBJECT->getWindow(0).getWinDim().y;
		if(ProbeMeasurelist != NULL)
		{
			for each(std::list<double> *measureParam in *ProbeMeasurelist)
			{
				measureParam->clear();
				delete measureParam;
				measureParam = NULL;				
			}
			ProbeMeasurelist->clear();
			delete ProbeMeasurelist;
			ProbeMeasurelist = NULL;
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP1029", __FILE__, __FUNCSIG__); }
}

// write function to Auto measurement of identical shapes.........
void HelperClass::AutoMeasurementFor_IdenticalShapes(std::list<double> *cord_list)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AutoMeasurementFor_IdenticalShapes", __FUNCSIG__);
	try
	{	
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
			getCoordinateFor_NextShape(); 
		}			
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0002", __FILE__, __FUNCSIG__); }
}

//get coordinate from list of coordinate......
void HelperClass::getCoordinateFor_NextShape()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getCoordinateFor_NextShape", __FUNCSIG__);
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
			return;
		}
		if(Coord_points.Pointscount() > 1)
			getShape_Parameter(true);
			
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0003", __FILE__, __FUNCSIG__); }
}

//get shape from list of shapes.......
void HelperClass::getShape_Parameter(bool firsttime)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getShape_Parameter", __FUNCSIG__);
	try
	{
		if(firsttime)
		{	
			int k = MAINDllOBJECT->Automated_ShapeTypeList.size();
			if(k > 0)
			{
				Shapeitr = MAINDllOBJECT->Automated_ShapeTypeList.begin();
				Shape* Cshape = (Shape*)(*Shapeitr);
				Shape* newshape = Cshape->Clone(0);
				AddShapeAction::addShape(newshape);
				AutomeasureShape_relation[Cshape->getId()] = newshape->getId();
				create_newFramgrab(true);
			}	
		}
		else if(Shapeitr == MAINDllOBJECT->Automated_ShapeTypeList.end())
			{
				for each(DimBase* measurement in MAINDllOBJECT->Automated_MeasurementTypeList)
				{
					//create clone of measurement.......
					DimBase* newmeasurement = measurement->Clone(0);
					SinglePoint* CoordPt =  Coord_points.getList()[SptCount];
					SinglePoint* CoordPt1 =  Coord_points.getList()[0];
					Vector difference(CoordPt->X  - CoordPt1->X, CoordPt->Y - CoordPt1->Y, CoordPt->Z - CoordPt1->Z);
					//translate measurement......
					newmeasurement->TranslatePosition(difference);
					Shape *oldPShape1, *oldPShape2, *oldPShape3; 
					int Shapecount = 0;
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
								Shapecount++;
						}
					}
					if(Shapecount == 3)
					{
						newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[AutomeasureShape_relation[oldPShape1->getId()]];
						newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[AutomeasureShape_relation[oldPShape2->getId()]];
						newmeasurement->ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().getList()[AutomeasureShape_relation[oldPShape3->getId()]];
						AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
						AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
						AddShapeMeasureRelationShip(newmeasurement->ParentShape3, newmeasurement);
						newmeasurement->UpdateMeasurement();
						AddDimAction::addDim(newmeasurement);
					}
					else if(Shapecount == 2)
					{
						newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[AutomeasureShape_relation[oldPShape1->getId()]];
						newmeasurement->ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().getList()[AutomeasureShape_relation[oldPShape2->getId()]];
						AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
						AddShapeMeasureRelationShip(newmeasurement->ParentShape2, newmeasurement);
						newmeasurement->UpdateMeasurement();
						AddDimAction::addDim(newmeasurement);
					}
					else if(Shapecount == 1)
					{
						newmeasurement->ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().getList()[AutomeasureShape_relation[oldPShape1->getId()]];
						AddShapeMeasureRelationShip(newmeasurement->ParentShape1, newmeasurement);
						newmeasurement->UpdateMeasurement();
						AddDimAction::addDim(newmeasurement);						
					}
				}
				getCoordinateFor_NextShape();
			}
		else
		{
			Shape* Cshape = (Shape*)(*Shapeitr);
			Shape* newshape = Cshape->Clone(0);
			AddShapeAction::addShape(newshape);
			AutomeasureShape_relation[Cshape->getId()] = newshape->getId();
			create_newFramgrab(true);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0004", __FILE__, __FUNCSIG__); }
}

//create new framgrab for selected shape......
void HelperClass::create_newFramgrab(bool firsttime)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "create_newFramgrab", __FUNCSIG__);
	try
	{
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		SinglePoint* CoordPt =  Coord_points.getList()[SptCount];
		SinglePoint* CoordPt1 =  Coord_points.getList()[0];
		Vector difference(CoordPt->X  - CoordPt1->X, CoordPt->Y - CoordPt1->Y, CoordPt->Z - CoordPt1->Z);
		if(firsttime)
		{
			int j = myshapeList->PointAtionList.size();
			if(j > 0)
			{
				ActionList = myshapeList->PointAtionList;
				Framgrabitr = ActionList.begin(); 
				//get framgrab dro current position.......
				Vector &curDroPos = ((AddPointAction*)(*Framgrabitr))->getFramegrab()->PointDRO;
				//move dro to a perticular position......
				MAINDllOBJECT->MoveMachineToRequiredPos(difference.getX() + curDroPos.getX(), difference.getY() + curDroPos.getY(), difference.getZ() + curDroPos.getZ(), true, RapidEnums::MACHINEGOTOTYPE::AUTOMEASUREMODEGOTO);
			}
		}
		else
		{
			Framgrabitr++;
			if(Framgrabitr == ActionList.end())
			{
				Shapeitr++;
				getShape_Parameter(false);
				return;
			}
			else
			{
				Vector &curDroPos = ((AddPointAction*)(*Framgrabitr))->getFramegrab()->PointDRO;
				MAINDllOBJECT->MoveMachineToRequiredPos(difference.getX() + curDroPos.getX(), difference.getY() + curDroPos.getY(), difference.getZ() + curDroPos.getZ(), true, RapidEnums::MACHINEGOTOTYPE::AUTOMEASUREMODEGOTO);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0005", __FILE__, __FUNCSIG__); }
}

//get framgrab parameter and add to the shape........
void HelperClass::get_FramGrabParameter()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "get_FramGrabParameter", __FUNCSIG__);
	try
	{
		RapidEnums::RAPIDFGACTIONTYPE CurrentFGType;
		bool FrameGrabDone = false;
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		SinglePoint* CoordPt1 =  Coord_points.getList()[0];
		SinglePoint* CoordPt =  Coord_points.getList()[SptCount];
		Vector difference(CoordPt->X  - CoordPt1->X, CoordPt->Y - CoordPt1->Y, CoordPt->Z - CoordPt1->Z);
		AddPointAction* Caction = (AddPointAction*)(*Framgrabitr);
		CurrentFGType = Caction->getFramegrab()->FGtype;
		FramegrabBase* baseaction = new FramegrabBase(Caction->getFramegrab());
		baseaction->TranslatePosition(difference, ((AddPointAction*)Caction)->getShape()->UcsId());
		switch(CurrentFGType)
		{
		case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
			MAINDllOBJECT->setHandler(new FrameGrabArc(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
			MAINDllOBJECT->setHandler(new FrameGrabAngularRect(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MOUSECLICK_GRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			MAINDllOBJECT->setHandler(new FrameGrabEdgeMouseClick(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
			MAINDllOBJECT->setHandler(new FrameGrabCircle(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
			MAINDllOBJECT->setHandler(new FrameGrabRectangle(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
			MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle(baseaction));
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
			MAINDllOBJECT->setHandler(new FlexibleCrossHairHandler(baseaction));
			break;
		}
		MAINDllOBJECT->update_VideoGraphics();
		Sleep(100);
		
		if(MAINDllOBJECT->getShapesList().selectedItem() != NULL)
		{
			ShapeWithList* Csh = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();

			//grab points from the surface....
			FrameGrabDone = DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false, true);
			if(FrameGrabDone)//add points to the shape..........
				AddPointAction::pointAction(Csh, baseaction);
		}
		create_newFramgrab(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0006", __FILE__, __FUNCSIG__); }
}

void HelperClass::getFramGrabParameter(Vector difference)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getFramGrabParameter", __FUNCSIG__);
	try
	{
		RapidEnums::RAPIDFGACTIONTYPE CurrentFGType;
		bool FrameGrabDone = false;
		ShapeWithList* myshapeList = (ShapeWithList*)(*Shapeitr);
		AddPointAction* Caction = (AddPointAction*)(*Framgrabitr);
		CurrentFGType = Caction->getFramegrab()->FGtype;
		FramegrabBase* baseaction = new FramegrabBase(Caction->getFramegrab());		
		baseaction->TranslatePosition(difference, ((AddPointAction*)Caction)->getShape()->UcsId());
		
		switch(CurrentFGType)
		{
		//case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
		//	MAINDllOBJECT->setHandler(new FrameGrabArc(baseaction));
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
		//	MAINDllOBJECT->setHandler(new FrameGrabAngularRect(baseaction));
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MOUSECLICK_GRAB;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		//	MAINDllOBJECT->setHandler(new FrameGrabEdgeMouseClick(baseaction));
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
		//	MAINDllOBJECT->setHandler(new FrameGrabCircle(baseaction));
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
		//	MAINDllOBJECT->setHandler(new FrameGrabRectangle(baseaction));
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
		//	MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle(baseaction));
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
		//	MAINDllOBJECT->setHandler(new FlexibleCrossHairHandler(baseaction));
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES:
		//	MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE;
		//	MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
		//	MAINDllOBJECT->setHandler(new FrameGrabRectangle(baseaction));
		//	break;
		case RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE:
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
			switch(myshapeList->ShapeType)
			{
			case RapidEnums::SHAPETYPE::LINE:
			case RapidEnums::SHAPETYPE::XLINE:
			case RapidEnums::SHAPETYPE::XRAY:
				switch(((Line*)myshapeList)->LineType)
				{
					case RapidEnums::LINETYPE::FINITELINE:
					case RapidEnums::LINETYPE::INFINITELINE:
					case RapidEnums::LINETYPE::RAYLINE:
					case RapidEnums::LINETYPE::LINEINTWOARC:
						MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER;
						MAINDllOBJECT->setHandler(new LineHandlers());
						if(baseaction->CurrentWindowNo == 1)
						{
							((LineHandlers*)(MAINDllOBJECT->getCurrentHandler()))->UpdateRepeatShapeAction(baseaction);
						}
						CreateFramGrabCallback();
						return;
				}
			}
		}
		//MAINDllOBJECT->update_VideoGraphics();
		//Sleep(200);
				
		//grab points from the surface....
		//FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false, true);
		if(FrameGrabDone && baseaction->AllPointsList.Pointscount() > 100)//add points to the shape..........
		{
			myshapeList->AddPoints(&baseaction->AllPointsList);			
			CreateFramGrabCallback();
		}
		else
		{
			//message box or status message to take fram grab ..
			//MAINDllOBJECT->CurrentStatus("Helper001", false, "");
			IsFixtureCallibration = true;
			switch(CurrentFGType)
			{
			case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
				MAINDllOBJECT->setHandler(new FrameGrabArc());
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
				MAINDllOBJECT->setHandler(new FrameGrabAngularRect());
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MOUSECLICK_GRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				MAINDllOBJECT->setHandler(new FrameGrabEdgeMouseClick());
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
				MAINDllOBJECT->setHandler(new FrameGrabCircle());
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				MAINDllOBJECT->setHandler(new FrameGrabRectangle(false));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle());
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				MAINDllOBJECT->setHandler(new FlexibleCrossHairHandler());
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				MAINDllOBJECT->setHandler(new FrameGrabRectangle(true));
				break;
			}
		}	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0005", __FILE__, __FUNCSIG__); }
}

 //get Approach distance point for probe......
void HelperClass::getApproachDistPoint(double *InitialPt, double *EndPt, double Approach_Dist, double *ans)
{ 
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "getApproachDistPoint", __FUNCSIG__);
	   try
	   {
		   double Dcosine[3];	  
		   double pt[6] = {InitialPt[0], InitialPt[1], InitialPt[2], EndPt[0], EndPt[1], EndPt[2]};
		   RMATH3DOBJECT->DCs_TwoEndPointsofLine(pt, Dcosine);
		   double AxisDCs[3];
		   double temp = abs(Dcosine[0]);
		   int Indx = 0, MulFactor = 1;

		   for(int i = 1; i < 3; i++)
		   {
			   if(temp < abs(Dcosine[i]))
			   {
				   temp = abs(Dcosine[i]);
				   Indx = i;
			   }
		   }
		   if(Dcosine[Indx] < 0) MulFactor = -1;
		   ans[0] = EndPt[0]; ans[1] = EndPt[1]; ans[2] = EndPt[2]; ans[3] = EndPt[0]; ans[4] = EndPt[1]; ans[5] = EndPt[2];
		   ans[Indx] = EndPt[Indx] + Approach_Dist * MulFactor;
		   ans[Indx + 3] = EndPt[Indx] - Approach_Dist * MulFactor;
	   }
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0032", __FILE__, __FUNCSIG__); return;}
   }
	
bool HelperClass::ShowGraphicalRepresentaionforDeviation(double Idealradius, double tolerance, bool showMeasurement)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "ShowGraphicalRepresentaionforDeviation", __FUNCSIG__);
	try
	{	
		//deallocate memory if list is not null.......
		if(ProbeMeasurelist != NULL)
		{
			for each(std::list<double> *measureParam in *ProbeMeasurelist)
			{
				measureParam->clear();
				delete measureParam;
				measureParam = NULL;				
			}
			ProbeMeasurelist->clear();
			delete ProbeMeasurelist;
			ProbeMeasurelist = NULL;
		}
		ProbeMeasurelist = new std::list<std::list<double>*>();
		map<int,double> MeasureValues;
		Shape* Cshape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(Cshape->ShapeType != RapidEnums::SHAPETYPE::CYLINDER)
			return false;
		
		double CylinderParam[7] = {((Cylinder*)Cshape)->getCenter1()->getX(), ((Cylinder*)Cshape)->getCenter1()->getY(), ((Cylinder*)Cshape)->getCenter1()->getZ(),
			((Cylinder*)Cshape)->dir_l(), ((Cylinder*)Cshape)->dir_m(), ((Cylinder*)Cshape)->dir_n(), ((Cylinder*)Cshape)->Radius1()};
		//if radius of ideal cylinder is zero then show deviation with best fit cylinder........
		if(Idealradius == 0)
			Idealradius = ((Cylinder*)Cshape)->Radius1();
		if(tolerance == 0)
			tolerance = 0.01;

		((Cylinder*)Cshape)->setModifiedName("BestFitCyl");

		Cylinder* idealCylinder = new Cylinder();
		idealCylinder->Radius1(Idealradius);
		idealCylinder->Radius2(Idealradius);
		idealCylinder->SetCenter1(Vector(((Cylinder*)Cshape)->getCenter1()->getX(), ((Cylinder*)Cshape)->getCenter1()->getY(), ((Cylinder*)Cshape)->getCenter1()->getZ()));
		idealCylinder->SetCenter2(Vector(((Cylinder*)Cshape)->getCenter2()->getX(), ((Cylinder*)Cshape)->getCenter2()->getY(), ((Cylinder*)Cshape)->getCenter2()->getZ()));
		idealCylinder->Length(((Cylinder*)Cshape)->Length());
		idealCylinder->dir_l(((Cylinder*)Cshape)->dir_l());
		idealCylinder->dir_m(((Cylinder*)Cshape)->dir_m());
		idealCylinder->dir_n(((Cylinder*)Cshape)->dir_n());
		idealCylinder->setModifiedName("IdealCyl");
		idealCylinder->IsValid(true);
		Cshape->addChild(idealCylinder);
		idealCylinder->addParent(Cshape);
		AddShapeAction::addShape(idealCylinder, false);

		bool IdCylinder = false;
		int distCount = 0;
		for(PC_ITER Item = ((ShapeWithList*)Cshape)->PointsList->getList().begin(); Item != ((ShapeWithList*)Cshape)->PointsList->getList().end(); Item++)
		{
			distCount += 1;
			SinglePoint* Spt = (*Item).second;
			double ContactPoint[3] = {Spt->X, Spt->Y, Spt->Z}, Pt[3] = {0};
			double dist = RMATH3DOBJECT->Distance_Point_Line(ContactPoint, CylinderParam);
			if(dist < Idealradius)
				IdCylinder = true;
			else
				IdCylinder = false;
			RMATH3DOBJECT->Cylinder_SurfacePt_for_Probe(CylinderParam, ContactPoint, abs(Idealradius - dist) , IdCylinder, &Pt[0]);			
			if(IdCylinder)
			{
				MeasureValues[distCount] = -RMATH3DOBJECT->Distance_Point_Point(Pt, ContactPoint);
				create_3DLine(Pt, ContactPoint, false, false);
			}
			else
			{
				MeasureValues[distCount] = RMATH3DOBJECT->Distance_Point_Point(Pt, ContactPoint);
				create_3DLine(Pt, ContactPoint, false);
			}
			RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
			item--;
			Shape* shpe = (Shape*)(*item).second;
			idealCylinder->addChild(shpe);
			shpe->addParent(idealCylinder);
			if(showMeasurement)
			{
				DimBase* dim = NULL;	
				dim = CreatePointDim3D(((Line*)shpe)->getPoint1(), ((Line*)shpe)->getPoint2(), (((Line*)shpe)->getPoint1()->getX() + ((Line*)shpe)->getPoint2()->getX()) / 2, 
					(((Line*)shpe)->getPoint1()->getY() + ((Line*)shpe)->getPoint2()->getY()) / 2, (((Line*)shpe)->getPoint1()->getZ() + ((Line*)shpe)->getPoint2()->getZ()) / 2);
				double mSelectionLine[6] = {0};
				MAINDllOBJECT->getWindow(1).GetSelectionLine((((Line*)shpe)->getPoint1()->getX() + ((Line*)shpe)->getPoint2()->getX()) / 2, (((Line*)shpe)->getPoint1()->getY() + ((Line*)shpe)->getPoint2()->getY()) / 2, mSelectionLine);
				dim->SetSelectionLine(mSelectionLine);
				dim->addParent(shpe);
				shpe->addMChild(dim);
				dim->IsValid(true);	
				dim->UpdateMeasurement();
				if(IdCylinder)
				{
					char* cd = dim->getCDimension();
					std::string cdimension = (const char*)cd;
					std::string newCdim = "-" + cdimension;
					cd = (char*)newCdim.c_str();
					dim->setDimension(cd);
					dim->setDimension(-dim->getDimension());
				}					
				AddDimAction::addDim(dim);
			}				
		}

		bool firstTime = true;
		double Zvalue = 0;
		std::list<double> *updatedMeasurelist = new std::list<double>();
		distCount = 0;
		int reversecnt = 0;
		//create list of list of double...........
		for(PC_ITER Item = ((ShapeWithList*)Cshape)->PointsListOriginal->getList().begin(); Item != ((ShapeWithList*)Cshape)->PointsListOriginal->getList().end(); Item++)
		{
			distCount += 1;
			SinglePoint* Spt = (*Item).second;
			if(firstTime)
			{
				firstTime = false;
				Zvalue = Spt->Z;
			}
			if(abs(Spt->Z - Zvalue) < 0.1)
			{
				Zvalue = Spt->Z;
				updatedMeasurelist->push_back(MeasureValues[distCount]);
				if(((ShapeWithList*)Cshape)->PointsListOriginal->Pointscount() == distCount)
				{
					if(reversecnt % 2 == 0)
					{
						ProbeMeasurelist->push_back(updatedMeasurelist);
						reversecnt++;
					}
					else
					{
						updatedMeasurelist->reverse();
						ProbeMeasurelist->push_back(updatedMeasurelist);
						reversecnt++;
					}
				}
			}
			else
			{
				if(reversecnt % 2 == 0)
				{
					ProbeMeasurelist->push_back(updatedMeasurelist);
					reversecnt++;
				}
				else
				{
					updatedMeasurelist->reverse();
					ProbeMeasurelist->push_back(updatedMeasurelist);
					reversecnt++;
				}
				updatedMeasurelist = new std::list<double>;
				updatedMeasurelist->push_back(MeasureValues[distCount]);
				Zvalue = Spt->Z;
			}
		}	
		((Cylinder*)Cshape)->PartialShape(true);
		MAINDllOBJECT->AutoProbeMeasureFlag = false;
		MAINDllOBJECT->UpdateShapesChanged();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0033", __FILE__, __FUNCSIG__); return false; }
}

void HelperClass::Create_AngleBisector(Shape* Shp1, Shape* Shp2)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "Create_AngleBisector", __FUNCSIG__);
	try
	{
		double mousePt[3] = {0}, point1[3] = {0}, point2[3] = {0};
		mousePt[0] = (((Line*)Shp1)->getMidPoint()->getX() + ((Line*)Shp2)->getMidPoint()->getX()) / 2;
		mousePt[1] = (((Line*)Shp1)->getMidPoint()->getY() + ((Line*)Shp2)->getMidPoint()->getY()) / 2;
		mousePt[2] = (((Line*)Shp1)->getMidPoint()->getZ() + ((Line*)Shp2)->getMidPoint()->getZ()) / 2;
		RMATH2DOBJECT->Angle_bisector(((Line*)Shp1)->Angle(), ((Line*)Shp1)->Intercept(), ((Line*)Shp2)->Angle(), ((Line*)Shp2)->Intercept(), &mousePt[0], ((Line*)Shp1)->Length(), &point1[0], &point2[0]);
		Line* myLine = new Line();
		myLine->LineType = RapidEnums::LINETYPE::ANGLE_BISECTORL;
		myLine->ShapeType = RapidEnums::SHAPETYPE::LINE;
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(point1[0], point1[1], point1[2]));
		PtColl.Addpoint(new SinglePoint(point2[0], point2[1], point2[2]));
		myLine->AddPoints(&PtColl);		
		myLine->Normalshape(true);	
		myLine->IsValid(true);		
		Shp1->addChild(myLine);
		Shp2->addChild(myLine);
		myLine->addParent(Shp1);
		myLine->addParent(Shp2);
		AddShapeAction::addShape(myLine);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0034", __FILE__, __FUNCSIG__); }
}

void HelperClass::create_3DLine(double *Point1, double *Point2, bool selected, bool whitecolor, bool DontSaveShapeInPartProgram)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "create_3DLine", __FUNCSIG__);
	try
	{
		Line* myLine  = new Line();
		myLine->LineType = RapidEnums::LINETYPE::FINITELINE3D;
		myLine->ShapeType = RapidEnums::SHAPETYPE::LINE3D;
		myLine->ShapeAs3DShape(true);
		if(!whitecolor)
			myLine->ShapeColor.set(1, 0, 0);
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(Point1[0], Point1[1], Point1[2]));
		PtColl.Addpoint(new SinglePoint(Point2[0], Point2[1], Point2[2]));
		myLine->AddPoints(&PtColl);		
		myLine->Normalshape(true);	
		myLine->IsValid(true);
		myLine->DoNotSaveInPP = DontSaveShapeInPartProgram;
		AddShapeAction::addShape(myLine, selected);
		//AddShapeAction::addShape(myLine, selected);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0035", __FILE__, __FUNCSIG__); }
}

void HelperClass::Create2DLine(double *Point1, double *Point2, bool selected, bool whitecolor, bool DontSaveShapeInPartProgram)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "Create2DLine", __FUNCSIG__);
	try
	{
		Line* myLine  = new Line();
		myLine->ShapeType = RapidEnums::SHAPETYPE::LINE;
		myLine->LineType = RapidEnums::LINETYPE::FINITELINE;
		if(!whitecolor)
			myLine->ShapeColor.set(1, 0, 0);
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(Point1[0], Point1[1], Point1[2]));
		PtColl.Addpoint(new SinglePoint(Point2[0], Point2[1], Point2[2]));
		myLine->AddPoints(&PtColl);		
		myLine->Normalshape(true);	
		myLine->IsValid(true);	
		myLine->DoNotSaveInPP = DontSaveShapeInPartProgram;
		AddShapeAction::addShape(myLine, selected);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0036", __FILE__, __FUNCSIG__); }
}

void HelperClass::CreateToolAxis(double *Point1, double *Point2)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CreateToolAxis", __FUNCSIG__);
	try
	{
		if(MAINDllOBJECT->Vblock_CylinderAxisLine == NULL)
		{
			MAINDllOBJECT->Vblock_CylinderAxisLine = new Line(_T("T_Axis"), RapidEnums::SHAPETYPE::XLINE);
			((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->LineType = RapidEnums::LINETYPE::INFINITELINE;
			MAINDllOBJECT->Vblock_CylinderAxisLine->Normalshape(true);	
			MAINDllOBJECT->Vblock_CylinderAxisLine->IsValid(true);	
			MAINDllOBJECT->Vblock_CylinderAxisLine->DoNotSaveInPP = true;
			MAINDllOBJECT->Vblock_CylinderAxisLine->setId(MAINDllOBJECT->ToolAxisLineId);
			MAINDllOBJECT->getCurrentUCS().getShapes().addItem(MAINDllOBJECT->Vblock_CylinderAxisLine);
			PointCollection PtColl;
			PtColl.Addpoint(new SinglePoint(Point1[0], Point1[1], Point1[2]));
			PtColl.Addpoint(new SinglePoint(Point2[0], Point2[1], Point2[2]));
			((ShapeWithList*)MAINDllOBJECT->Vblock_CylinderAxisLine)->AddPoints(&PtColl);		
			//MAINDllOBJECT->Vblock_CylinderAxisLine->notifyAll(ITEMSTATUS::DATACHANGED, MAINDllOBJECT->Vblock_CylinderAxisLine);
			PPCALCOBJECT->VblockAxisAsReference(true);
		}
		else
		{
			((ShapeWithList*)MAINDllOBJECT->Vblock_CylinderAxisLine)->PointsList->deleteAll();
			((ShapeWithList*)MAINDllOBJECT->Vblock_CylinderAxisLine)->PointsListOriginal->deleteAll();
			PointCollection PtColl;
			PtColl.Addpoint(new SinglePoint(Point1[0], Point1[1], Point1[2]));
			PtColl.Addpoint(new SinglePoint(Point2[0], Point2[1], Point2[2]));
			((ShapeWithList*)MAINDllOBJECT->Vblock_CylinderAxisLine)->AddPoints(&PtColl);	
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0037", __FILE__, __FUNCSIG__); }
}

bool HelperClass::CheckParallismRelationBtn2Lines(Line *line1, Line *line2)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CheckParallismRelationBtn2Lines", __FUNCSIG__);
	try
	{
		double param1[6] = {0}, param2[6] = {0};
		line1->getParameters(param1);
		line2->getParameters(param2);
		if(!RMATH3DOBJECT->Checking_Parallel_Line_to_Line(param1, param2)) return false;
		
		if((line1->LineType == RapidEnums::LINETYPE::PARALLEL_LINEL) || (line2->LineType == RapidEnums::LINETYPE::PARALLEL_LINEL))
		{
		   return CheckRelationBtn2Lines(line1, line2);
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0037b", __FILE__, __FUNCSIG__); return false;}
}

bool HelperClass::CheckRelationBtn2Lines(Line *line1, Line *line2)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "CheckRelationBtn2Lines", __FUNCSIG__);
	try
	{
		for(list<BaseItem*>::iterator It = line1->getParents().begin(); It != line1->getParents().end(); It++)
		{
		   if((*It)->getId() == line2->getId()) return true;
		}
		for(list<BaseItem*>::iterator It = line1->getChild().begin(); It != line1->getChild().end(); It++)
		{
		   if((*It)->getId() == line2->getId()) return true;
		}
		for(list<BaseItem*>::iterator It = line1->getParents().begin(); It != line1->getParents().end(); It++)
		{
		   //if(CheckRelationBtn2Lines((Line *)(*It), line2)) return true;
		   if (ChecktheParallelism((Line*)(*It), line2)) return true;
		}
		for(list<BaseItem*>::iterator It = line1->getChild().begin(); It != line1->getChild().end(); It++)
		{
		   //if(CheckRelationBtn2Lines((Line *)(*It), line2)) return true;
		   if (ChecktheParallelism((Line*)(*It), line2)) return true;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0037b", __FILE__, __FUNCSIG__); return false;}
}

void HelperClass::GetMcsPointsList(PointCollection *UcsList, PointCollection *McsList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetMcsPointsList", __FUNCSIG__);
	if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
	{
		for(PC_ITER Item = UcsList->getList().begin(); Item != UcsList->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			McsList->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
		}
	}
	else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
	{
		for(PC_ITER Item = UcsList->getList().begin(); Item != UcsList->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			McsList->Addpoint(new SinglePoint(Spt->Z, Spt->X, Spt->Y, Spt->R, Spt->Pdir));
		}
	}
	else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
	{
		for(PC_ITER Item = UcsList->getList().begin(); Item != UcsList->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			McsList->Addpoint(new SinglePoint(Spt->X, Spt->Z, Spt->Y, Spt->R, Spt->Pdir));
		}
	}
}

void HelperClass::GetMcsPoint(Vector *UcsList, Vector *McsList)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetMcsPoint", __FUNCSIG__);
	if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
	{
		McsList->set(UcsList->getX(), UcsList->getY(), UcsList->getZ(), UcsList->getR(), UcsList->getP());
	}
	else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
	{
		McsList->set(UcsList->getZ(), UcsList->getX(), UcsList->getY(), UcsList->getR(), UcsList->getP());
	}
	else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
	{
		McsList->set(UcsList->getX(), UcsList->getZ(), UcsList->getY(), UcsList->getR(), UcsList->getP());
	}
}

int HelperClass::OutermostInnerMostCone(PointCollection *pointlist, double *coneParam, double *coneCenters, bool usevalidPointonly, bool outermost)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostCone", __FUNCSIG__);
	try
	{
	   double ratio = (coneParam[7] - coneParam[6]) / RMATH3DOBJECT->Distance_Point_Point(coneCenters, &coneCenters[3]);
	   int cnt = 0;
	   if(outermost)
	   {
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(usevalidPointonly && Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
				RMATH3DOBJECT->Projection_Point_on_Line(currentP, coneParam, projctnPt);
				if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, coneCenters, 2))
				{
					double tmph = RMATH3DOBJECT->Distance_Point_Point(projctnPt, coneCenters);
					double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, coneParam));
					double curntr = abs(coneParam[6] + tmph * ratio);
					if(tmpr < curntr)
					{
						Spt->InValid = true;
					}
					else
						cnt++;	
				}
		   }
	   }
	   else
	   {
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
				RMATH3DOBJECT->Projection_Point_on_Line(currentP, coneParam, projctnPt);
				if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, coneCenters, 2))
				{
					double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, coneParam));
					double tmph = RMATH3DOBJECT->Distance_Point_Point(projctnPt, coneCenters);
					double curntr = abs(coneParam[6] + tmph * ratio);
					if(tmpr > curntr)
					{
						Spt->InValid = true;
					}
					else
					   cnt++;
				}
		   }	   
	   }
	   return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::OutermostInnerMostCylinder(PointCollection *pointlist, double *cylinderParam, double *cylinderCenters, bool usevalidPointonly, bool outermost)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostCylinder", __FUNCSIG__);
	try
	{
	   int cnt = 0;
	   if(outermost)
	   {
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(usevalidPointonly && Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
				RMATH3DOBJECT->Projection_Point_on_Line(currentP, cylinderParam, projctnPt);
				if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, cylinderCenters, 2))
				{
					double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, cylinderParam));
					if(tmpr < cylinderParam[6])
					{
						Spt->InValid = true;
						cnt++;
					}
				}
		   }
	   }
	   else
	   {
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
				RMATH3DOBJECT->Projection_Point_on_Line(currentP, cylinderParam, projctnPt);
				if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, cylinderCenters, 2))
				{
					double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, cylinderParam));
					if(tmpr > cylinderParam[6])
					{
						Spt->InValid = true;
						cnt++;
					}
				}
		   }   
	   }
	   return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::OutermostInnerMostCircle(PointCollection *pointlist, double *circleparam, bool usevalidPointonly, bool outermost)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostCircle", __FUNCSIG__);
	try
	{
		int cnt = 0;
		circleparam[2] = 0;
		if(outermost)
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(usevalidPointonly && Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, 0};
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, circleparam));
				if(tmpr < circleparam[3])
				{
					Spt->InValid = true;
				}
				else
					cnt++;
			}	
		}
		else
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, 0};
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, circleparam));
				if(tmpr > circleparam[3])
				{
				   Spt->InValid = true;
				  }
				else
					cnt++;
			}	
		}
		return cnt;
	}
	catch(...)
	{
	   return -1;
	}
}

int HelperClass::OutermostInnerMostSphere(PointCollection *pointlist, double *sphereparam, bool usevalidPointonly, bool outermost)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostSphere", __FUNCSIG__);
	try
	{
		int cnt = 0;
		if(outermost)
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(usevalidPointonly && Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, sphereparam));
				if(tmpr < sphereparam[3])
				{
					Spt->InValid = true;
				}
				else
					cnt++;
			}	
		}
		else
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, sphereparam));
				if(tmpr > sphereparam[3])
				{
					Spt->InValid = true;
				}
				else
					cnt++;
			}			
		}
		return cnt;
	}
	catch(...)
	{
	   return -1;
	}
}

int HelperClass::OutermostInnerMostTorus(PointCollection *pointlist, double *torusparam, bool usevalidPointonly, bool outermost)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostTorus", __FUNCSIG__);
	try
	{
		int cnt = 0;
		double tmpPnt[3] = {0}, torusPlane[4] = {torusparam[3], torusparam[4], torusparam[5], torusparam[3] * torusparam[0] + torusparam[4] * torusparam[1] + torusparam[5] * torusparam[2]};
		if(outermost)
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(usevalidPointonly && Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
				RMATH3DOBJECT->Projection_Point_on_Plane(currentP, torusPlane, tmpPnt);
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(tmpPnt, torusparam));
				if(abs(tmpr - torusparam[6]) < torusparam[7])
				{
					Spt->InValid = true;
				}
				else
					cnt++;
			}	
		}
		else
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
				RMATH3DOBJECT->Projection_Point_on_Plane(currentP, torusPlane, tmpPnt);
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(tmpPnt, torusparam));
				if(abs(tmpr - torusparam[6]) > torusparam[7])
				{
					Spt->InValid = true;
				}
				else
					cnt++;
			}			
		}
		return cnt;
	}
	catch(...)
	{
	   return -1;
	}
}

int HelperClass::OutermostInnerMostCircle3D(PointCollection *pointlist, double *circleparam, bool usevalidPointonly, bool outermost)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostCircle3D", __FUNCSIG__);
	try
	{
		int cnt = 0;
		double circlePlane[4] = {circleparam[3], circleparam[4], circleparam[5], circleparam[0] * circleparam[3] + circleparam[1] * circleparam[4] + circleparam[2] * circleparam[5]};
		if(outermost)
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(usevalidPointonly && Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projectedP[3] ={0};
				RMATH3DOBJECT->Projection_Point_on_Plane(currentP, circlePlane, projectedP);
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(projectedP, circleparam));
				if(tmpr < circleparam[6])
				{
					Spt->InValid = true;
				}
				else
					cnt++;
			}	
		}
		else
		{
			for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->InValid) continue;
				double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projectedP[3] ={0};
				RMATH3DOBJECT->Projection_Point_on_Plane(currentP, circlePlane, projectedP);
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(projectedP, circleparam));
				if(tmpr > circleparam[6])
				{
					Spt->InValid = true;
				}
				else
					cnt++;
			}		
		}
		return cnt;
	}
	catch(...)
	{
	   return -1;
	}
}

int HelperClass::OutermostInnerMostPlane(PointCollection *pointlist, double *planeparam, bool usevalidPointonly, double *scandir)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostPlane", __FUNCSIG__);
	try
	{
		int cnt = 0;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(usevalidPointonly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projectedP[3] = {0};
			RMATH3DOBJECT->Projection_Point_on_Plane(currentP, planeparam, projectedP);
			double dirrat[3] = {currentP[0] - projectedP[0], currentP[1] - projectedP[1], currentP[2] - projectedP[2]}, dir[3] = {0};
			RMATH3DOBJECT->DirectionCosines(dirrat, dir);
			if(RMATH3DOBJECT->Angle_Btwn_2Directions(dir, scandir, false, true) < 0)
			{
			   Spt->InValid = true;
			}
			else
				cnt++;
		}	
		return cnt;
	}
	catch(...)
	{
	  return -1;
	}
}

int HelperClass::OutermostInnerMostLine3D(PointCollection *pointlist, double *lineparam, bool usevalidPointonly, double *scandir)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostLine3D", __FUNCSIG__);
	try
	{
		int cnt = 0;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(usevalidPointonly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projectedP[3] ={0};
			RMATH3DOBJECT->Projection_Point_on_Line(currentP, lineparam, projectedP);
			double dirrat[3] = {currentP[0] - projectedP[0], currentP[1] - projectedP[1], currentP[2] - projectedP[2]}, dir[3] = {0};
			RMATH3DOBJECT->DirectionCosines(dirrat, dir);
			if(RMATH3DOBJECT->Angle_Btwn_2Directions(dir, scandir, false, true) < 0)
			{
				Spt->InValid = true;
			}
			else
				cnt++;
		}	
		return cnt;
	}
	catch(...)
	{
	  return -1;
	}
}

int HelperClass::OutermostInnerMostLine(PointCollection *pointlist, double *lineparam, bool usevalidPointonly, double *scandir)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "OutermostInnerMostLine", __FUNCSIG__);
	try
	{
		int cnt = 0;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(usevalidPointonly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projectedP[3] ={0};
			double dirrat[3] = {currentP[0] - projectedP[0], currentP[1] - projectedP[1], currentP[2] - projectedP[2]}, dir[3] = {0};
			RMATH3DOBJECT->Projection_Point_on_Line(currentP, lineparam, projectedP);		
			RMATH3DOBJECT->DirectionCosines(dirrat, dir);
			if(RMATH3DOBJECT->Angle_Btwn_2Directions(dir, scandir, false, true) < 0)
			{
				Spt->InValid = true;
			}
			else
				cnt++;
		}	
		return cnt;
	}
	catch(...)
	{
	  return -1;
	}
}

int HelperClass::FilteredCone(PointCollection *pointlist, double *coneParam, double *coneCenters, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredCone", __FUNCSIG__);
	try
	{
		double ratio = (coneParam[7] - coneParam[6]) / RMATH3DOBJECT->Distance_Point_Point(coneCenters, &coneCenters[3]), avg = 0;
		int cnt = 0, prevCnt = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
			RMATH3DOBJECT->Projection_Point_on_Line(currentP, coneParam, projctnPt);
			if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, coneCenters, 2))
			{
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, coneParam));
				double tmph = RMATH3DOBJECT->Distance_Point_Point(projctnPt, coneCenters);
				double curntr = abs(coneParam[6] + tmph * ratio);
				avg += abs(curntr - tmpr);
				prevCnt++;
			}
		}
		if(prevCnt > 0)
			avg /= prevCnt;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
			RMATH3DOBJECT->Projection_Point_on_Line(currentP, coneParam, projctnPt);
			if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, coneCenters, 2))
			{
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, coneParam));
				double tmph = RMATH3DOBJECT->Distance_Point_Point(projctnPt, coneCenters);
				double curntr = abs(coneParam[6] + tmph * ratio);
				if(abs(tmpr - curntr) < (multiFactor * avg))
				{
				   Spt->InValid = true;
				}
				else
				{
					Spt->InValid = false;
					cnt++;
				}
			}
		}	   
		return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::FilteredCylinder(PointCollection *pointlist, double *cylinderParam, double *cylinderCenters, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredCylinder", __FUNCSIG__);
	try
	{
		double avg = 0;
		int cnt = 0, prevCnt = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
			RMATH3DOBJECT->Projection_Point_on_Line(currentP, cylinderParam, projctnPt);
			if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, cylinderCenters, 2))
			{
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, cylinderParam));
				avg += abs(cylinderParam[6] - tmpr);
				prevCnt++;
			}
		}
		if(prevCnt > 0)
			avg /= prevCnt;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z}, projctnPt[3] = {0};
			RMATH3DOBJECT->Projection_Point_on_Line(currentP, cylinderParam, projctnPt);
			if(RMATH3DOBJECT->Checking_Point_within_Block(projctnPt, cylinderCenters, 2))
			{
				double tmpr = abs(RMATH3DOBJECT->Distance_Point_Line(currentP, cylinderParam));
				if(abs(tmpr - cylinderParam[6]) > (multiFactor * avg))
				{
					Spt->InValid = true;
				}
				else
				{
					Spt->InValid = false;
					cnt++;
				}
			}
		}	   
		return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::FilteredCircle(PointCollection *pointlist, double *circleparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredCircle", __FUNCSIG__);
	try
	{
		double avg = 0;
		int cnt = 0, prevCnt = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, 0};
			double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, circleparam));
			avg += abs(circleparam[3] - tmpr);
			prevCnt++;
		}
		if(prevCnt > 0)
			avg /= prevCnt;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, 0};
			double tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, circleparam));
			if(abs(tmpr - circleparam[3]) > (multiFactor * avg))
			{
				Spt->InValid = true;
			}
			else
			{
				Spt->InValid = false;
				cnt++;
			}
		}	   
		return cnt;
	}
	catch(...)
	{
		return -1;
	}
}
int HelperClass::FilteredTwoArc(PointCollection *pointlist, double *twoarcparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredTwoArc", __FUNCSIG__);
	try
	{
		double avg1 = 0, avg2 = 0;
		int cnt = 0, prevCnt1 = 0, prevCnt2 = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;																//take a poit from list for calculation
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, 0};
			double tmpr1 = abs(RMATH2DOBJECT->Pt2Pt_distance(currentP[0], currentP[1], twoarcparam[0], twoarcparam[1]));		//distance from centre1..... vinod
			double tmpr2 = abs(RMATH2DOBJECT->Pt2Pt_distance(currentP[0], currentP[1], twoarcparam[3], twoarcparam[4]));		//distance from centre2..... vinod
			if(abs(twoarcparam[2] - tmpr1) < abs(twoarcparam[5] - tmpr2))
			{
				avg1 += abs(twoarcparam[2] - tmpr1);															//all points deviation addition for arc1
				prevCnt1++;																						//pointcount for arc1
			}
			else
			{
				avg2 += abs(twoarcparam[5] - tmpr2);															//all points deviation addition for arc2
				prevCnt2++;																						//twoarcparam== (x1,y1,radius1,x2,y2,radius2)
			}
		}
		if(prevCnt1 > 0)
			avg1 /= prevCnt1;																	//allpoints's deviation's avg1
		if(prevCnt2 > 0)
			avg2 /= prevCnt2;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, 0};
			double tmpr1 = abs(RMATH2DOBJECT->Pt2Pt_distance(currentP[0], currentP[1], twoarcparam[0], twoarcparam[1]));		//distance from centre1..... vinod
			double tmpr2 = abs(RMATH2DOBJECT->Pt2Pt_distance(currentP[0], currentP[1], twoarcparam[3], twoarcparam[4]));		//distance from centre2..... vinod
			if( (abs(tmpr1 - twoarcparam[2]) > (multiFactor * avg1)) && (abs(tmpr2 - twoarcparam[5]) > (multiFactor * avg2)) )		// if point is beyond a certain cutoff distance away from the average discard it otherwise consider it
			{
				Spt->InValid = true;
			}
			else
			{
				Spt->InValid = false;
				cnt++;
			}
		}	   
		return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::FilteredSphere(PointCollection *pointlist, double *sphereparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredSphere", __FUNCSIG__);
	try
	{
		double tmpr = 0, avg = 0;
		int cnt = 0, prevCnt = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, sphereparam));
			avg += abs(sphereparam[3] - tmpr);
			prevCnt++;
		}
		if(prevCnt > 0)
			avg /= prevCnt;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(currentP, sphereparam));
			if(abs(tmpr - sphereparam[3]) > (multiFactor * avg))
			{
				Spt->InValid = true;
			}
			else
			{
				cnt++;
				Spt->InValid = false;
			}
		}	   
		return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::FilteredTorus(PointCollection *pointlist, double *torusparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredTorus", __FUNCSIG__);
	try
	{
		double tmpr = 0, avg = 0;
		int cnt = 0, prevCnt = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		double tmpPnt[3] = {0}, torusPlane[4] = {torusparam[3], torusparam[4], torusparam[5], torusparam[3] * torusparam[0] + torusparam[4] * torusparam[1] + torusparam[5] * torusparam[2]};
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			RMATH3DOBJECT->Projection_Point_on_Plane(currentP, torusPlane, tmpPnt);
			tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(tmpPnt, torusparam));
			avg += abs(torusparam[6] - tmpr);
			prevCnt++;
		}
		if(prevCnt > 0)
			avg /= prevCnt;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			RMATH3DOBJECT->Projection_Point_on_Plane(currentP, torusPlane, tmpPnt);
			tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(tmpPnt, torusparam));
			if(abs(tmpr - torusparam[6]) > (multiFactor * avg))
			{
				Spt->InValid = true;
			}
			else
			{
				cnt++;
				Spt->InValid = false;
			}
		}	   
		return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::FilteredCircle3D(PointCollection *pointlist, double *circleparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredCircle3D", __FUNCSIG__);
	try
	{
		double avg = 0;
		int cnt = 0, prevCnt = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		double tmpr, projctedPnt[3] = {0}, circlePlane[4] = {circleparam[3], circleparam[4], circleparam[5], circleparam[0] * circleparam[3] + circleparam[1] * circleparam[4] + circleparam[2] * circleparam[5]};
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			RMATH3DOBJECT->Projection_Point_on_Plane(currentP, circlePlane, projctedPnt);
			tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(projctedPnt, circleparam));
			avg += abs(circleparam[6] - tmpr);
			prevCnt++;
		}
		if(prevCnt > 0)
			avg /= prevCnt;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			RMATH3DOBJECT->Projection_Point_on_Plane(currentP, circlePlane, projctedPnt);
			tmpr = abs(RMATH3DOBJECT->Distance_Point_Point(projctedPnt, circleparam));
			if(abs(tmpr - circleparam[6]) > (multiFactor * avg))
			{
				Spt->InValid = true;
			}
			else
			{
				Spt->InValid = false;
				cnt++;
			}
		}	   
		return cnt;
	}
	catch(...)
	{
		return -1;
	}
}

int HelperClass::FilteredPlane(PointCollection *pointlist, double *planeparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredPlane", __FUNCSIG__);
	try
	{
		int cnt = 0, prevCnt = 0;
		double avg = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			avg += abs(RMATH3DOBJECT->Distance_Point_Plane(currentP, planeparam));
			prevCnt++;
		}
		if(prevCnt > 0)	{avg /= prevCnt;}
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			if(abs(RMATH3DOBJECT->Distance_Point_Plane(currentP, planeparam)) > (multiFactor * avg))
			{
				Spt->InValid = true;
			}
			else
			{
				Spt->InValid = false;
				cnt++;
			}
		}			
		return cnt;
	}
	catch(...)
	{
	  return -1;
	}
}

int HelperClass::FilteredLine3D(PointCollection *pointlist, double *lineparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredLine3D", __FUNCSIG__);
	try
	{
		int cnt = 0, prevCnt = 0;
		double avg = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			avg += abs(RMATH3DOBJECT->Distance_Point_Line(currentP, lineparam));
			prevCnt++;
		}
		if(prevCnt > 0) {avg /= prevCnt;}
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, Spt->Z};
			if(abs(RMATH3DOBJECT->Distance_Point_Line(currentP, lineparam)) > (multiFactor * avg))
			{
				Spt->InValid = true;
			}
			else
			{
				Spt->InValid = false;
				cnt++;
			}
		}
		return cnt;
	}
	catch(...)
	{
	  return -1;
	}
}

int HelperClass::FilteredLine(PointCollection *pointlist, double *lineparam, int multiFactor, int iterationCnt)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "FilteredLine", __FUNCSIG__);
	try
	{
		int cnt = 0, prevCnt = 0;
		double avg = 0;
		lineparam[2] = 0;
		bool useValidOnly = true;
		if(iterationCnt == 0) useValidOnly = false;
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, 0};
			avg += abs(RMATH3DOBJECT->Distance_Point_Line(currentP, lineparam));
			prevCnt++;
		}
		if(prevCnt > 0)	{avg /= prevCnt;}
		for(PC_ITER Item = pointlist->getList().begin(); Item != pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(useValidOnly && Spt->InValid) continue;
			double currentP[3] = {Spt->X, Spt->Y, 0};
			if(abs(RMATH3DOBJECT->Distance_Point_Line(currentP, lineparam)) > (multiFactor * avg))
			{
				Spt->InValid = true;	
			}
			else
			{
				Spt->InValid = false;
				cnt++;
			}
		}			
		return cnt;
	}
	catch(...)
	{
	  return -1;
	}
}

void HelperClass::AddMeasurementForDelphiComponent(Line *pline, Circle *PCircle, RPoint *ppoint)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "AddMeasurementForDelphiComponent", __FUNCSIG__);
	try
	{
		DimBase *CurrentMeasure = NULL;
		double mdpoint1[2] = {0}, ReferenceCenter[3] = {0};
		Vector temp, *Vpointer1 = NULL, *Vpointer2 = NULL;
		temp.set(ppoint->getPosition()->getX(), ppoint->getPosition()->getY(), ppoint->getPosition()->getZ());
		Vpointer1 = MAINDllOBJECT->getVectorPointer(&temp);
		PCircle->getCenter()->FillDoublePointer(ReferenceCenter, 3);
		temp.set(ReferenceCenter[0], ReferenceCenter[1], ReferenceCenter[2]);
		Vpointer2 = MAINDllOBJECT->getVectorPointer(&temp);
		ppoint->getPosition()->FillDoublePointer(mdpoint1); 
		double mdpoint[2] = {(mdpoint1[0] + ReferenceCenter[0])/2, (mdpoint1[1] + ReferenceCenter[1])/2};
		if(Vpointer1 != NULL && Vpointer2 != NULL)
		{
			CurrentMeasure = CreatePointDim2D(Vpointer1, Vpointer2, mdpoint[0] + 0.2, mdpoint[1] + 0.2, 0);
			PCircle->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(PCircle);
			ppoint->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(ppoint);
			CurrentMeasure->IsValid(true);
			CurrentMeasure->UpdateMeasurement();
			AddDimAction::addDim(CurrentMeasure);
		}

		Line *tmpLine = new Line();
		FramegrabBase* baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE);
		
		baseaction->CurrentWindowNo = 1;
		baseaction->DroMovementFlag = false;
		AddShapeAction::addShape(tmpLine);
		MAINDllOBJECT->setBaseRPropertyNonHighlighted(ppoint->getPosition()->getX(), ppoint->getPosition()->getY(), ppoint->getPosition()->getZ(), false, baseaction, tmpLine);
		AddPointAction::pointAction((ShapeWithList*)tmpLine, baseaction);

		FramegrabBase* baseaction1 = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE);
		baseaction1->CurrentWindowNo = 1;
		baseaction1->DroMovementFlag = false;
		MAINDllOBJECT->setBaseRPropertyNonHighlighted(ReferenceCenter[0], ReferenceCenter[1], ReferenceCenter[2], false, baseaction1, tmpLine);
		AddPointAction::pointAction((ShapeWithList*)tmpLine, baseaction1);

		double midpoint1[2] = {0}, midpoint2[2] = {0};
		pline->getMidPoint()->FillDoublePointer(midpoint1); tmpLine->getMidPoint()->FillDoublePointer(midpoint2);
		double midpoint[2] = {(midpoint1[0] + midpoint2[0])/2, (midpoint1[1] + midpoint2[1])/2};
		CurrentMeasure = new DimLineToLineDistance(_T("Ang"));
		CurrentMeasure->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINERAYANGLE;
		((DimLineToLineDistance*)CurrentMeasure)->CalculateMeasurement(pline, tmpLine);
		CurrentMeasure->SetMousePosition(midpoint[0], midpoint[1], pline->getPoint1()->getZ());
		CurrentMeasure->ContainsLinearMeasurementMode(true);
		CurrentMeasure->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
	
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
		CurrentMeasure->SetSelectionLine(&SelectionLine[0]);
		CurrentMeasure->LinearMeasurementMode(true);
		pline->addMChild(CurrentMeasure);
		CurrentMeasure->addParent(pline);
		tmpLine->addMChild(CurrentMeasure);
		CurrentMeasure->addParent(tmpLine);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0041", __FILE__, __FUNCSIG__);
	}
}

bool HelperClass::GetLineParameter(double *point1, double *point2, double *LineParam)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("HELPER", "GetLineParameter", __FUNCSIG__);
	try
	{
		LineParam[0] = RMATH2DOBJECT->Pt2Pt_distance(point1, point2);
		if(LineParam[0] < 0.0001) return false;
		LineParam[1] = (atan((point2[1] - point1[1])/(point2[0] - point1[0])));
		if(LineParam[1] < 0)
			LineParam[1] = LineParam[1] + M_PI;
		else if(LineParam[1] > M_PI)
			LineParam[1] = LineParam[1] - M_PI;
		
		if(abs(LineParam[1] - M_PI_2) > 0.00058)
		{
			LineParam[2] = point1[1] - tan(LineParam[1]) * point1[0];
		}
		else
		{
			LineParam[1] = M_PI_2;
			LineParam[2] = point1[0];
		}
		return true;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0042", __FILE__, __FUNCSIG__);
		return false;
	}
}
