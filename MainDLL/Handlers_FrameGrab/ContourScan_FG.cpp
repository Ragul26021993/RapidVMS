#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "ContourScan_FG.h"
#include "..\Engine\FocusFunctions.h"
#include "..\Actions\AddUCSAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

ContourScan_FG::ContourScan_FG()
{
	try
	{
		this->SelectionShapeType = SELECTIONSHAPETYPE::DEFAULT;
		setMaxClicks(2);
		this->SeletionFrames_Height = 40;
		this->SeletionFrames_Width = 40;
		this->scanning = false;
		this->ShapeToAddPnts = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0001", __FILE__, __FUNCSIG__); }
}

ContourScan_FG::~ContourScan_FG()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0002", __FILE__, __FUNCSIG__); }
}

//For 1st click set drag = true...
void ContourScan_FG::LmouseDown()
{
	try
	{
		/*if(ShapeToAddPnts == NULL) 
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}*/
		this->scanning = false;
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0003", __FILE__, __FUNCSIG__); }
}

void ContourScan_FG::mouseMove()
{
	try
	{
		if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
		{
			PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0004", __FILE__, __FUNCSIG__); }
}

//Draw the rectangle till drag is true... till mouse up..!!
void ContourScan_FG::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 0)
		{
			if(getClicksDone() > 0 || scanning)
			{
				double wupp = WPixelWidth;
				double height = SeletionFrames_Height * wupp;
				if(SelectionShapeType == SELECTIONSHAPETYPE::DEFAULT)
				{
					GRAFIX->SetColor_Double(1, 0, 0);
					double Points[4] = {PointsDRO[0].getX(), PointsDRO[0].getY(), PointsDRO[1].getX(), PointsDRO[1].getY()};
					if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
					{
						if(Points[2] > Points[0])
							Points[2] = Points[0] + height;
						else
							Points[2] = Points[0] - height;
					}
					else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
					{
						if(Points[3] > Points[1])
							Points[3] = Points[1] + height;
						else
							Points[3] = Points[1] - height;
					}
					else
					{
						if(abs(Points[2] - Points[0]) < abs(Points[3] - Points[1]))
						{
							if(Points[2] > Points[0])
								Points[2] = Points[0] + height;
							else
								Points[2] = Points[0] - height;
						}
						else
						{
							if(Points[3] > Points[1])
								Points[3] = Points[1] + height;
							else
								Points[3] = Points[1] - height;
						}
					}
					GRAFIX->translateMatrix(0.0, 0.0, 2.0);
					GRAFIX->SetColor_Double(0, 0, 1);
					GRAFIX->drawRectangle(Points[0], Points[1], Points[2], Points[3]);
					GRAFIX->translateMatrix(0.0, 0.0, -2.0);
					GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
					GRAFIX->drawRectangle(Points[0] + wupp, Points[1] - wupp , Points[2] - wupp, Points[3] + wupp, true);
				}
				else if(SelectionShapeType == SELECTIONSHAPETYPE::CIRCULAR)
				{
					double radius = 0, radius1 = 0, radius2 = 0;
					double Points2[2]={(PointsDRO[0].getX() + PointsDRO[1].getX())/2 , (PointsDRO[0].getY() + PointsDRO[1].getY())/2 };
					radius = RMATH2DOBJECT->Pt2Pt_distance(PointsDRO[0].getX(),PointsDRO[0].getY(), Points2[0], Points2[1]);
					radius1 = radius + height/2;
					radius2 = radius - height/2;
					GRAFIX->translateMatrix(0.0, 0.0, 2.0);
					GRAFIX->SetColor_Double(1, 0, 0);
					GRAFIX->drawCircle(Points2[0], Points2[1], radius1);
					GRAFIX->drawCircle(Points2[0], Points2[1], radius2);
					GRAFIX->translateMatrix(0.0, 0.0, -2.0);
			/*		GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f); 
					GRAFIX->drawFillArcFG(PointsDRO[2].getX(), PointsDRO[2].getY(), radius1 - wupp, radius2 + wupp, 0, 2 * M_PI);*/
				}
				else if(SelectionShapeType == SELECTIONSHAPETYPE::ARC)
				{
					double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()},
						point3[2] = {PointsDRO[2].getX(), PointsDRO[2].getY()};
					double ep1[4] = {0}, ep2[4] = {0}, cen[2] = {0}, radius = 0, radius1 = 0, radius2 = 0, startangle = 0, sweepangle = 0;
					RMATH2DOBJECT->Arc_3Pt(&point1[0], &point2[0], &point3[0], &cen[0], &radius, &startangle, &sweepangle);
					radius1 = radius + height/2;
					radius2 = radius - height/2;
					RMATH2DOBJECT->ArcFrameGrabEndPts(&cen[0], radius1, radius2, startangle, sweepangle, &ep1[0], &ep2[0]);
					GRAFIX->translateMatrix(0.0, 0.0, 2.0);
					GRAFIX->SetColor_Double(1, 0, 0);
					GRAFIX->drawArc(cen[0], cen[1], radius1, startangle, sweepangle, wupp);
					GRAFIX->drawArc(cen[0], cen[1], radius2, startangle, sweepangle, wupp);
					GRAFIX->drawLine(ep1[0], ep1[1], ep1[2], ep1[3]);
					GRAFIX->drawLine(ep2[0], ep2[1], ep2[2], ep2[3]);
					GRAFIX->translateMatrix(0.0, 0.0, -2.0);
					GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);		
					GRAFIX->drawFillArcFG(cen[0], cen[1], radius1 - wupp, radius2 + wupp, startangle, sweepangle);
				}
				/*else if(SelectionShapeType == SELECTIONSHAPETYPE::ANGULARRECTANGLE)
				{
					double pts[14] = {0}, point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
					RMATH2DOBJECT->RectangleFG(&point1[0], &point2[0], height/2, wupp, &pts[0], false);
					GRAFIX->translateMatrix(0.0, 0.0, 2.0);
					GRAFIX->SetColor_Double(1, 0, 0);
					GRAFIX->drawRectangle(pts[0], pts[1], pts[2], pts[3], pts[6], pts[7], pts[4], pts[5]);
					GRAFIX->translateMatrix(0.0, 0.0, -2.0);
				}*/
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0005", __FILE__, __FUNCSIG__); }
}

void ContourScan_FG::LmouseUp(double x, double y)
{
}

void ContourScan_FG::LmaxmouseDown()
{
	try
	{
		/*if(ShapeToAddPnts == NULL) 
		{
			setClicksDone(getMaxClicks() - 1);
			return;
		}*/
		FOCUSCALCOBJECT->ClrearAllFocus();
	
		double wupp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
		int correctionX = MAINDllOBJECT->getWindow(0).getWinDim().x  * HELPEROBJECT->CamSizeRatio / 2;
		int correctionY = MAINDllOBJECT->getWindow(0).getWinDim().y  * HELPEROBJECT->CamSizeRatio / 2;
		int counter = 0;
		bool alongX = true;
		if(!PPCALCOBJECT->IsPartProgramRunning())
			PointsDRO[getMaxClicks() - 1].set(getClicksValue(getMaxClicks() - 1));

		double tmpPoints[6] = {PointsDRO[0].getX() - MAINDllOBJECT->getCurrentDRO().getX(), PointsDRO[0].getY() - MAINDllOBJECT->getCurrentDRO().getY(), PointsDRO[1].getX() - MAINDllOBJECT->getCurrentDRO().getX(), PointsDRO[1].getY() - MAINDllOBJECT->getCurrentDRO().getY(), PointsDRO[2].getX() - MAINDllOBJECT->getCurrentDRO().getX(), PointsDRO[2].getY() - MAINDllOBJECT->getCurrentDRO().getY()};

		if (SelectionShapeType == SELECTIONSHAPETYPE::DEFAULT)
		{
			double startP[2] = { tmpPoints[0], tmpPoints[1] };
			int length = 0;
			if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				length = (int)floor(abs(tmpPoints[3] - tmpPoints[1]) / wupp);
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				length = (int)floor(abs(tmpPoints[2] - tmpPoints[0]) / wupp);
			else
			{
				if (abs(tmpPoints[2] - tmpPoints[0]) < abs(tmpPoints[3] - tmpPoints[1]))
					length = (int)floor(abs(tmpPoints[3] - tmpPoints[1]) / wupp);
				else
					length = (int)floor(abs(tmpPoints[2] - tmpPoints[0]) / wupp);
			}

			if (length < SeletionFrames_Width) return;

			int noOfCell = length - SeletionFrames_Width;

			FOCUSCALCOBJECT->setRowColumn(1, noOfCell, 0, SeletionFrames_Width, SeletionFrames_Height);

			if (FOCUSCALCOBJECT->Cvalue != NULL) { free(FOCUSCALCOBJECT->Cvalue); FOCUSCALCOBJECT->Cvalue = NULL; }
			FOCUSCALCOBJECT->Cvalue = (double*)calloc(noOfCell, sizeof(double));
			if (FOCUSCALCOBJECT->Rectptr != NULL) { free(FOCUSCALCOBJECT->Rectptr);  FOCUSCALCOBJECT->Rectptr = NULL; }
			FOCUSCALCOBJECT->Rectptr = (int*)calloc(noOfCell * 4, sizeof(int));

			if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			{
				if (tmpPoints[2] < tmpPoints[0])
					startP[0] = tmpPoints[0] - SeletionFrames_Height * wupp;
				if (tmpPoints[3] > tmpPoints[1])
					startP[1] = tmpPoints[3];

				for (int j = 0; j < noOfCell; j++)
				{
					FOCUSCALCOBJECT->Rectptr[counter++] = int(startP[0] / wupp + correctionX);
					FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionY - startP[1] / wupp + j);
					FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Width * HELPEROBJECT->CamSizeRatio;
					FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Height * HELPEROBJECT->CamSizeRatio;
				}
				alongX = false;
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			{
				if (tmpPoints[2] < tmpPoints[0])
					startP[0] = tmpPoints[2];
				if (tmpPoints[3] > tmpPoints[1])
					startP[1] = tmpPoints[1] + SeletionFrames_Height * wupp;

				for (int j = 0; j < noOfCell; j++)
				{
					FOCUSCALCOBJECT->Rectptr[counter++] = int(startP[0] / wupp + j + correctionX);
					FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionY - startP[1] / wupp);
					FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Width * HELPEROBJECT->CamSizeRatio;
					FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Height * HELPEROBJECT->CamSizeRatio;
				}
			}
			else
			{
				if (abs(tmpPoints[2] - tmpPoints[0]) < abs(tmpPoints[3] - tmpPoints[1]))
				{
					if (tmpPoints[2] < tmpPoints[0])
						startP[0] = tmpPoints[0] - SeletionFrames_Height * wupp;
					if (tmpPoints[3] > tmpPoints[1])
						startP[1] = tmpPoints[3];

					for (int j = 0; j < noOfCell; j++)
					{
						FOCUSCALCOBJECT->Rectptr[counter++] = int(startP[0] / wupp + correctionX);
						FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionY - startP[1] / wupp + j);
						FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Width * HELPEROBJECT->CamSizeRatio;
						FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Height * HELPEROBJECT->CamSizeRatio;
					}
					alongX = false;
				}
				else
				{
					if (tmpPoints[2] < tmpPoints[0])
						startP[0] = tmpPoints[2];
					if (tmpPoints[3] > tmpPoints[1])
						startP[1] = tmpPoints[1] + SeletionFrames_Height * wupp;

					for (int j = 0; j < noOfCell; j++)
					{
						FOCUSCALCOBJECT->Rectptr[counter++] = int(startP[0] / wupp + j + correctionX);
						FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionY - startP[1] / wupp);
						FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Width * HELPEROBJECT->CamSizeRatio;
						FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Height * HELPEROBJECT->CamSizeRatio;
					}
				}

			}
		}
		else if(SelectionShapeType == SELECTIONSHAPETYPE::CIRCULAR)
		{

			double radius = 0, centerPnt[2] = {((tmpPoints[0] + tmpPoints[2]) / 2), ((tmpPoints[1] + tmpPoints[3]) / 2)};
			radius = RMATH2DOBJECT->Pt2Pt_distance(tmpPoints[0], tmpPoints[1], centerPnt[0], centerPnt[1]);	
			double increamentAngle =  wupp/radius, cAng = 0;
			int noOfCell = int(2 * M_PI / increamentAngle);

			FOCUSCALCOBJECT->setRowColumn(1, noOfCell, 0, SeletionFrames_Width, SeletionFrames_Height);
			if(FOCUSCALCOBJECT->Cvalue != NULL) { free(FOCUSCALCOBJECT->Cvalue); FOCUSCALCOBJECT->Cvalue = NULL;}
				FOCUSCALCOBJECT->Cvalue = (double*)calloc(noOfCell, sizeof(double));
			if(FOCUSCALCOBJECT->Rectptr != NULL){ free(FOCUSCALCOBJECT->Rectptr);  FOCUSCALCOBJECT->Rectptr = NULL;}
				FOCUSCALCOBJECT->Rectptr = (int*)calloc(noOfCell * 4, sizeof(int));	

			for(int i = 0; i < noOfCell; i++)
			{
				double tmpX = centerPnt[0] + radius * cos(cAng);
				double tmpY = centerPnt[1] + radius * sin(cAng);
				FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionX + tmpX/wupp - SeletionFrames_Width/2);
				FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionY - tmpY/wupp - SeletionFrames_Height/2);
				FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Width;
				FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Height;
				cAng += increamentAngle;
			}
		}
		else if(SelectionShapeType == SELECTIONSHAPETYPE::ARC)
		{
			double cen[2] = {0}, radius = 0, startAngle = 0, sweepAngle = 0, cAng = 0;
			RMATH2DOBJECT->Arc_3Pt(&tmpPoints[0], &tmpPoints[2], &tmpPoints[4], &cen[0], &radius, &startAngle, &sweepAngle);
			double increamentAngle = wupp / radius;
			int noOfCell = int(sweepAngle / increamentAngle);

			FOCUSCALCOBJECT->setRowColumn(1, noOfCell, 0, SeletionFrames_Width, SeletionFrames_Height);
			if(FOCUSCALCOBJECT->Cvalue != NULL) { free(FOCUSCALCOBJECT->Cvalue); FOCUSCALCOBJECT->Cvalue = NULL;}
				FOCUSCALCOBJECT->Cvalue = (double*)calloc(noOfCell, sizeof(double));
			if(FOCUSCALCOBJECT->Rectptr != NULL){ free(FOCUSCALCOBJECT->Rectptr);  FOCUSCALCOBJECT->Rectptr = NULL;}
				FOCUSCALCOBJECT->Rectptr = (int*)calloc(noOfCell * 4, sizeof(int)); 

			cAng = startAngle;
			for(int i = 0; i < noOfCell; i++)
			{
				double tmpX = cen[0] + radius * cos(cAng);
				double tmpY = cen[1] + radius * sin(cAng);
				FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionX + tmpX/wupp - SeletionFrames_Width/2);
				FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionY - tmpY/wupp - SeletionFrames_Height/2);
				FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Width;
				FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Height;
				cAng += increamentAngle;
			}
		}
		/*else if(SelectionShapeType == SELECTIONSHAPETYPE::ANGULARRECTANGLE)
		{
			double points[4] = {0};
			if (PointsDRO[0].getY() < PointsDRO[1].getY())
			{
			   points[0] = PointsDRO[0].getX(); points[1] = PointsDRO[0].getY();
			   points[2] = PointsDRO[1].getX(); points[3] = PointsDRO[1].getY();
			}
			else
			{
				points[0] = PointsDRO[1].getX(); points[1] = PointsDRO[1].getY();
				points[2] = PointsDRO[0].getX(); points[3] = PointsDRO[0].getY();
			}
			if(CreateUcs(points))
			{
			
			}
			double moveDir[3] = {points[2] - points[0], points[3] - points[1], 0}, moveDirCos[3] = {0};
			RMATH3DOBJECT->DirectionCosines(moveDir, moveDirCos);
			int diffDist = (int)floor(abs((RMATH2DOBJECT->Pt2Pt_distance(&points[0], &points[2])) / wupp));
			if(diffDist < SeletionFrames_Width) return;
			int noOfCell = diffDist - SeletionFrames_Width + SeletionFrames_Width % 2;
			FOCUSCALCOBJECT->setRowColumn(1, noOfCell, 0, SeletionFrames_Width, SeletionFrames_Height);
			if(FOCUSCALCOBJECT->Cvalue != NULL) { free(FOCUSCALCOBJECT->Cvalue); FOCUSCALCOBJECT->Cvalue = NULL;}
			FOCUSCALCOBJECT->Cvalue = (double*)calloc(noOfCell, sizeof(double));
			if(FOCUSCALCOBJECT->Rectptr != NULL){ free(FOCUSCALCOBJECT->Rectptr);  FOCUSCALCOBJECT->Rectptr = NULL;}
			FOCUSCALCOBJECT->Rectptr = (int*)calloc(noOfCell * 4, sizeof(int)); 
			for(double i = SeletionFrames_Width/2; i < (diffDist - SeletionFrames_Width/2); i++)
			{
				double tmpX = (points[0] - MAINDllOBJECT->getCurrentDRO().getX()) / wupp + i * moveDirCos[0];
				double tmpY = (points[1] - MAINDllOBJECT->getCurrentDRO().getY()) / wupp + i * moveDirCos[1];
				FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionX + int(tmpX) - SeletionFrames_Width/2);
				FOCUSCALCOBJECT->Rectptr[counter++] = int(correctionY - int(tmpY) - SeletionFrames_Height/2);
				FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Width;
				FOCUSCALCOBJECT->Rectptr[counter++] = SeletionFrames_Height; 
			}
		}*/
		if(SelectionShapeType == SELECTIONSHAPETYPE::DEFAULT)
		{
			this->ShapeToAddPnts = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(this->ShapeToAddPnts == NULL)
			{
				if(MAINDllOBJECT->getCurrentUCS().getId() == 0)
				{
					if(alongX )
						MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XZ;
					else 
						MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::YZ;
					UCS::CreateUcs(0, &MAINDllOBJECT->getCurrentUCS().UCSPoint);
				}
				AddShapeAction::addShape(new CloudPoints());
				ShapeToAddPnts = (ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second);
			}
		}
		else
		{
			this->ShapeToAddPnts = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(this->ShapeToAddPnts == NULL)
			{
				AddShapeAction::addShape(new CloudPoints());
				ShapeToAddPnts = (ShapeWithList*)(MAINDllOBJECT->getSelectedShapesList().getList().begin()->second);
			}
		
		}
		MAINDllOBJECT->DoFocusFG();
		resetClicks();	
		this->scanning = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0007", __FILE__, __FUNCSIG__); }

}

void ContourScan_FG::RmouseDown(double x, double y)
{
	try
	{
			if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
			{
				if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				{
					if(SelectionShapeType == SELECTIONSHAPETYPE::DEFAULT)
						SelectionShapeType = (SELECTIONSHAPETYPE)0;
					else
						SelectionShapeType = (SELECTIONSHAPETYPE)((int)SelectionShapeType + 1);
					if(SelectionShapeType == SELECTIONSHAPETYPE::ARC)
						setMaxClicks(3);
					else
						setMaxClicks(2);
				}
			}
			else
			{
				ShapeToAddPnts = NULL;
				ShapeToAddPnts = MAINDllOBJECT->AddShapeForRightClick(true);
			}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SHMH0016", __FILE__, __FUNCSIG__); }
}

void ContourScan_FG::MouseScroll(bool flag)
{
}

void ContourScan_FG::EscapebuttonPress()
{
	resetClicks();
}

void ContourScan_FG::PartProgramData()
{
	try
	{
	   FramegrabBase *Cbaseaction = PPCALCOBJECT->getFrameGrab();
	   this->PointsDRO[0].set(Cbaseaction->points[0]);
	   this->PointsDRO[1].set(Cbaseaction->points[1]);
	   this->PointsDRO[2].set(Cbaseaction->points[2]);
	   this->SelectionShapeType = (ContourScan_FG::SELECTIONSHAPETYPE)((int)Cbaseaction->myPosition[0].getX());
	   this->SeletionFrames_Width = (int)Cbaseaction->myPosition[0].getY();
	   setClicksDone(1);
	   LmaxmouseDown();
	}
	catch(...)
	{
	}
}

bool ContourScan_FG::CreateUcs(double *points)
{
	if(PPCALCOBJECT->IsPartProgramRunning()) return false;
	if(MAINDllOBJECT->getCurrentUCS().UCSMode() != 0) return false; 
	double UIntercept= 0, ratio[3] = {points[2] - points[0], points[3] - points[1], 0}, cosine[3] = {0}, xaxis[3] = {1,0,0}, yaxis[3] = {0,1,0};
	RMATH3DOBJECT->DirectionCosines(ratio, cosine);
	double angle = RMATH3DOBJECT->Angle_Btwn_2Directions(cosine, xaxis, false, false);
	if(abs(angle) < 0.0001) return false;
	RMATH2DOBJECT->Angle_FirstFourthQuad(&angle);
	RMATH2DOBJECT->Intercept_LinePassing_Point(points, angle, &UIntercept);
	MAINDllOBJECT->AddingUCSModeFlag = true;
	UCS* nucs = new UCS(_T("UCS"));
	UCS& cucs = MAINDllOBJECT->getCurrentUCS();
	nucs->UCSMode(2);
	nucs->UCSPoint = cucs.UCSPoint;
	nucs->SetUCSProjectionType(RapidEnums::RAPIDPROJECTIONTYPE::XZ);
	nucs->UCSangle(angle);
	nucs->UCSIntercept(UIntercept);
	double centerPt[3] = {0};
	RMATH2DOBJECT->TransformM_RotateAboutPoint(centerPt, -nucs->UCSangle(), &nucs->transform[0]);
	nucs->getWindow(0).init(cucs.getWindow(0));
	nucs->getWindow(1).init(cucs.getWindow(1));
	nucs->getWindow(2).init(cucs.getWindow(2));
	nucs->CenterPt = cucs.CenterPt;
	AddUCSAction::addUCS(nucs);
	nucs->getWindow(1).homeIt(1);
	MAINDllOBJECT->SetRcadRotateAngle(angle);
	MAINDllOBJECT->SetVideoRotateAngle(angle);
	MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
	MAINDllOBJECT->updateAll();
	MAINDllOBJECT->AddingUCSModeFlag = false;
	cucs.getShapes().removeItem(ShapeToAddPnts, false);
	ShapeToAddPnts->UcsId(nucs->getId());
	nucs->getShapes().addItem(ShapeToAddPnts, true);
	MAINDllOBJECT->selectShape(ShapeToAddPnts->getId(),false);
	return true;
}