#include "StdAfx.h"
#include "CloudPointMeasurementsHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Helper\Helper.h"
#include "..\Measurement\DimSurfaceRoughnessMeasurement.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Shapes\FrameGrabShape.h"

CloudPointMeasurementsHandler::CloudPointMeasurementsHandler()
{
	try
	{
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0001", __FILE__, __FUNCSIG__); }
}

CloudPointMeasurementsHandler::~CloudPointMeasurementsHandler()
{
}

void CloudPointMeasurementsHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).PanMode)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT)
		{
			if(MAINDllOBJECT->RcadWindow3DMode())
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0004", __FILE__, __FUNCSIG__); }
}

void CloudPointMeasurementsHandler::mouseMove()
{
	if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
	{
		PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
	}
}

void CloudPointMeasurementsHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 1)
		{
			if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
			{
			    GRAFIX->SetColor_Double(1, 0, 0);
			    GRAFIX->drawRectangle(PointsDRO[0].getX(), PointsDRO[0].getY(), PointsDRO[1].getX(), PointsDRO[1].getY());
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0006", __FILE__, __FUNCSIG__); }
}

void CloudPointMeasurementsHandler::EscapebuttonPress()
{
	try
	{
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0007", __FILE__, __FUNCSIG__); }
}

void CloudPointMeasurementsHandler::LmaxmouseDown()
{
	try
	{
		ShapeWithList *ShapeToAddPnts = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if (ShapeToAddPnts != NULL) 
		{
		     
			PointsDRO[getMaxClicks() - 1].set(getClicksValue(getMaxClicks() - 1));
  			MAINDllOBJECT->CloudPointsMeasureCallBack();
		}
		resetClicks();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0008", __FILE__, __FUNCSIG__); }
}

void CloudPointMeasurementsHandler::AddMeasurement(list<string> measureList)
{
	try
	{
		CloudPoints *ShapeToAddPnts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
		if (ShapeToAddPnts != NULL) 
		{
	    	double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
			double topleft[2] = {0}, bottomRight[2] = {0};
			RMATH2DOBJECT->GetTopLeftNBtnRht(&point1[0], &point2[0], &topleft[0], &bottomRight[0]);
			PointCollection selectedPoints;
			for(PC_ITER Item = ShapeToAddPnts->PointsList->getList().begin(); Item != ShapeToAddPnts->PointsList->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->Point_IsInWindow(topleft[0], bottomRight[1], bottomRight[0], topleft[1]))
				{
				   selectedPoints.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
		 		}
			}	
			if(selectedPoints.Pointscount() > 10)
			{
				ShapeToAddPnts->PointsForMeasureCnt = selectedPoints.Pointscount();
				if(ShapeToAddPnts->CloudPointsForMeasure != NULL)
				{
					free(ShapeToAddPnts->CloudPointsForMeasure);
					ShapeToAddPnts->CloudPointsForMeasure = NULL;
				}
				ShapeToAddPnts->CloudPointsForMeasure = (double*)malloc(sizeof(double) * 3 * ShapeToAddPnts->PointsForMeasureCnt);
				double* RectendPoitnsX = new double[ShapeToAddPnts->PointsForMeasureCnt];
				double* RectendPoitnsY = new double[ShapeToAddPnts->PointsForMeasureCnt];
				int i = 0, PtCnt = 0;
				for(PC_ITER Spt = selectedPoints.getList().begin(); Spt != selectedPoints.getList().end(); Spt++)
				{
					SinglePoint* spt = (*Spt).second;
					RectendPoitnsX[PtCnt] = spt->X;
					RectendPoitnsY[PtCnt++] = spt->Y;
			    	ShapeToAddPnts->CloudPointsForMeasure[i++] = spt->X;
					ShapeToAddPnts->CloudPointsForMeasure[i++] = spt->Y;
					ShapeToAddPnts->CloudPointsForMeasure[i++] = spt->Z;
				}

				double extentPnts[4] = {0}, tmpPos[2] = {0}, position[3] = {0};
				extentPnts[0] = RMATH2DOBJECT->minimum(RectendPoitnsX, ShapeToAddPnts->PointsForMeasureCnt);
				extentPnts[1] = RMATH2DOBJECT->maximum(RectendPoitnsY, ShapeToAddPnts->PointsForMeasureCnt);
				extentPnts[2] = RMATH2DOBJECT->maximum(RectendPoitnsX, ShapeToAddPnts->PointsForMeasureCnt);
				extentPnts[3] = RMATH2DOBJECT->minimum(RectendPoitnsY, ShapeToAddPnts->PointsForMeasureCnt);

				double length = extentPnts[2] - extentPnts[0], width = extentPnts[1] - extentPnts[3];
				
				delete [] RectendPoitnsX;
				delete [] RectendPoitnsY;

				for each(string str in measureList)
				{
					
					DimBase *Cdim;
					double shiftx = 0, shiftz = 0;
					if(str == "Sq")
					{
						Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ);
						Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ;
						tmpPos[0] = extentPnts[0] + length / 6;
					    tmpPos[1] = extentPnts[1] - width / 6;
						shiftx = -.3;
						shiftz = .4;
					}
					else if(str == "Sa")
					{
						Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA);
						Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA;
						tmpPos[0] = extentPnts[0] + length / 2;
					    tmpPos[1] = extentPnts[1] - width / 6;
						shiftz = .4;
					}
					else if(str == "Sp")
					{			
						Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP);
						Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP;
						tmpPos[0] = extentPnts[0] + length * 5/6;
						tmpPos[1] = extentPnts[1] - width / 6;
						shiftx = .3;
						shiftz = .4;
					}
				    else if(str == "Sv")
					{
						Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV);
						Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV;
						tmpPos[0] = extentPnts[0] + length / 6;
					    tmpPos[1] = extentPnts[1] - width / 2;
						shiftx = -.5;
						shiftz = .6;
					}
					else if(str == "Sz")
					{
						Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ);
						Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ;
						tmpPos[0] = extentPnts[0] + length / 2;
					    tmpPos[1] = extentPnts[1] - width / 2;
						shiftz = -.6;
					}
			    	else if(str == "Ssk")
					{
						Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK);
						Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK;
						tmpPos[0] = extentPnts[0] + length * 5/6;
					    tmpPos[1] = extentPnts[1] - width / 2;
						shiftx = .5;
						shiftz = -.6;
					}
					else if(str == "Sku")
					{
						Cdim = new DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU);
						Cdim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU;
						tmpPos[0] = extentPnts[0] + length / 6;
					    tmpPos[1] = extentPnts[1] - width * 5/6;
						shiftx = -.7;
						shiftz = -.6;
					}
					if(Cdim != NULL)
					{
						Getpostion(ShapeToAddPnts->CloudPointsForMeasure, ShapeToAddPnts->PointsForMeasureCnt, tmpPos, position);
						Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
						Cdim->SetMousePosition(position[0] + shiftx, position[1], position[2] + shiftz + .3);
						((DimSurfaceRoughnessMeasurement*)Cdim)->SetCenter(position);
						double SelectionLine[6] = {position[0] + shiftx, position[1], position[2] + shiftz + .3, 0, 1, 0};
						Cdim->SetSelectionLine(&SelectionLine[0]);
						((DimSurfaceRoughnessMeasurement*)Cdim)->CalculateMeasurement(ShapeToAddPnts);
						ShapeToAddPnts->addMChild(Cdim);
		             	Cdim->addParent(ShapeToAddPnts);
						Cdim->IsValid(true);
						AddDimAction::addDim(Cdim);
				 	}
		        }
				/*FrameGrabShape * cshape = new FrameGrabShape();
				cshape->Points[0].set(PointsDRO[0].getX(), PointsDRO[0].getY(), 0);
				cshape->Points[1].set(PointsDRO[1].getX(), PointsDRO[1].getY(), 0);
				cshape->UpdateBestFit();
				AddShapeAction::addShape(cshape);	*/	
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0009", __FILE__, __FUNCSIG__); }
}

void CloudPointMeasurementsHandler::Getpostion(double *points, int cnt, double *tmpPos, double *pos)
{
	double dist = 1000;
	for(int i = 0; i < cnt; i++)
	{
		double tmpdist = RMATH2DOBJECT->Pt2Pt_distance(tmpPos, &points[3*i]);
    	if(tmpdist < dist)
		{
			pos[0] = points[3*i];
			pos[1] = points[3*i + 1];
			pos[2] = points[3*i + 2];
			dist = tmpdist;
		}
	}
}