//Include required headerfiles...//
#include "stdafx.h"
#include "DXFExpose.h"
#include "..\Engine\RCadApp.h"
#include "..\Hanlders_CadAlignment\CadAlignHandler.h"
#include "..\Shapes\ShapeHeaders.h"
#include "..\Measurement\MeasurementHeaders.h"
#include "DXFHeaders.h"
#include "..\MAINDLL\Actions\EditCad2PtAction.h"
#include "..\MAINDLL\Actions\Cad1PtAlignAction.h"
#include "..\MAINDLL\Actions\Cad2PtAlignAction.h"
#include "..\MAINDLL\Actions\Cad1Pt1LineAction.h"
#include "..\MAINDLL\Actions\Cad2LineAction.h"
#include "..\MAINDLL\Actions\LineArcAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Actions\EditCadAction.h"
#include "..\Engine\PointCollection.h"
#include <string>	
#include <sstream>

bool Is_ID = true;										
//double Cam_P_Circle_Centre[3]={0};              //11/02/2014 vinod.. for excel report we need here cam profile circle center..
double Cam_P_Circle_Centre[3] = { 0 };              //11/02/2014 vinod.. for excel report we need here cam profile circle center..
double Point3DForZPositionZValue = 0.0, MinRadi = 0.0, MaxRadi = 0.0;			//added by vj on 11/04/2014, as per new measurement requirement for z position in tpliner 

DXFExpose::DXFExpose()
{
	LoadDXF = false;
	this->AllPointsAsCloudPoint(false);
	this->AlignDxfShapes(false);
	this->DoThreeDAlignment(false);
	this->DxfShapeContinuity(false);
	this->AlignFlag = false;
	this->DeviationWithLineOnly = false;
	this->ExportShapeAsZequaltoZero = false;
	this->ExportShapesOfAllUcsToDxf = false;
	this->PointsDirectoryPath = "D:\\Shape Points\\";
}

DXFExpose::~DXFExpose()
{
}

//delete the selected dxf...//
void DXFExpose::deleteDXF(int id)
{
	try
	{
		RCollectionBase& Currentdxf =  MAINDllOBJECT->getDXFList();
		DXFCollection *s = (DXFCollection*)Currentdxf.getList()[id];
		Currentdxf.removeItem(s, true);
		std::list<RAction*>::iterator TmpItr, itr = CurrentDXFNonEditedActionList.begin();
		while(itr != CurrentDXFNonEditedActionList.end())
		{
			RAction *act = *(itr);
			TmpItr = itr;
			MAINDllOBJECT->getActionsHistoryList().removeItem(act);
			delete act;
			itr++;
			CurrentDXFNonEditedActionList.erase(TmpItr);	
		}
		CurrentDXFNonEditedActionList.clear();
		UpdateForUCS();
		MAINDllOBJECT->DXFShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0001", __FILE__, __FUNCSIG__); }
}

//Select the dxf according to the id..//
void DXFExpose::selectDXF(int id)
{
	try
	{
		RCollectionBase& Currentdxf =  MAINDllOBJECT->getDXFList();
		Currentdxf.multiselect(true);
		Currentdxf.getList()[id]->selected(!Currentdxf.getList()[id]->selected());
		UpdateForUCS();
		Currentdxf.multiselect(false);
		MAINDllOBJECT->DXFShape_Updated();
		//MAINDllOBJECT->getDXFList().notifySelection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0002", __FILE__, __FUNCSIG__); }
}

void DXFExpose::UpdateForUCS()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning()) return;
		WaitForSingleObject(MAINDllOBJECT->DxfMutex, INFINITE);
		RCollectionBase& Currentdxf =  MAINDllOBJECT->getDXFList();
		CurrentDXFMeasureList.clear();
		CurrentDXFShapeList.clear();
		for(RC_ITER Citem = Currentdxf.getList().begin(); Citem != Currentdxf.getList().end(); Citem++)
		{
			DXFCollection* CDxfitem = ((DXFCollection*)(*Citem).second);
			if(CDxfitem->selected())
			{
				for(RC_ITER CdxfSh = CDxfitem->getShapeCollection().getList().begin(); CdxfSh != CDxfitem->getShapeCollection().getList().end(); CdxfSh++)
					CurrentDXFShapeList.push_back((Shape*)(*CdxfSh).second);
				for(RC_ITER Cdxfdim = CDxfitem->getMeasureCollection().getList().begin(); Cdxfdim != CDxfitem->getMeasureCollection().getList().end(); Cdxfdim++)
					CurrentDXFMeasureList.push_back((DimBase*)(*Cdxfdim).second);
			}
		}
		ReleaseMutex(MAINDllOBJECT->DxfMutex);
	}
	catch(...){ReleaseMutex(MAINDllOBJECT->DxfMutex); MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0002", __FILE__, __FUNCSIG__); }
}
//Edit DXF.. add to main collection (shape/ measurement)..//
void DXFExpose::editDXF()
{
	try
	{
		if(CurrentDXFShapeList.size() == 0)return;
		MAINDllOBJECT->dontUpdateGraphcis = true;
		std::list<Shape*> TempLineArcShapeList;
		int i = 0;
		
		HELPEROBJECT->ProfileScanLineArcAction = new LineArcAction();
		MAINDllOBJECT->addAction(HELPEROBJECT->ProfileScanLineArcAction);
		MAINDllOBJECT->ShowHideWaitCursor(1);
		EditCadAction *newedit = new EditCadAction(MAINDllOBJECT->getCurrentUCS().getId());
		for each(Shape* Csh in CurrentDXFShapeList)
		{
			Shape* CShape = Csh->Clone(1);
			newedit->EditedShapes.push_back(CShape);
			DxfRcadShapeRelation[Csh->getId()] = CShape->getId();
			CShape->IsDxfShape(true);
			CShape->IsAlignedShape(false);
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || CShape->ShapeType == RapidEnums::SHAPETYPE::ARC || CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				
				((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection[i] = CShape;
				((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection[i] = CShape;
				i++;
				TempLineArcShapeList.push_back(CShape);
				if(HELPEROBJECT->MultiFeatureScan)
					HELPEROBJECT->MultiFeatureShapeList.push_back(CShape->getId());
			}

			CShape->UpdateEnclosedRectangle();
			MAINDllOBJECT->getShapesList().addItem(CShape);
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS && !MAINDllOBJECT->TriangulateCpSurface())
				MAINDllOBJECT->CalculateCloudPtZlevelExtent();
		
		/*	list<BaseItem*>::iterator ShIter;
			for(ShIter = Csh->getMchild().begin(); ShIter != Csh->getMchild().end(); ShIter++)
			{
				if(ShapeColl1[(*ShIter)->getId()] != NULL)
					ShapeColl2[(*ShIter)->getId()] = CShape;
				else
					ShapeColl1[(*ShIter)->getId()] = CShape;
			}*/
			
		}
		
		if(DxfShapeContinuity())
			HELPEROBJECT->AddContinuityRaltion_DxfShapes(&TempLineArcShapeList);

		for(list<RAction*>::iterator It = CurrentDXFNonEditedActionList.begin(); It != CurrentDXFNonEditedActionList.end(); It++)
		{
			if((*It)->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
				((Cad1PtAlignAction*)(*It))->EditCurrentAlignment(DxfRcadShapeRelation);
			else if((*It)->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
				((Cad2PtAlignAction*)(*It))->EditCurrentAlignment(DxfRcadShapeRelation);
			else if((*It)->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
				((Cad1Pt1LineAction*)(*It))->EditCurrentAlignment(DxfRcadShapeRelation);
			else if((*It)->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION)
				((Cad2LineAction*)(*It))->EditCurrentAlignment(DxfRcadShapeRelation);
			else continue;
			newedit->EditedAlignMentAction.push_back(*It);
		}
		MAINDllOBJECT->addAction(newedit);
		CurrentDXFNonEditedActionList.clear();
		DxfRcadShapeRelation.clear();
		ResetDxfAlignment();

		MAINDllOBJECT->ShowHideWaitCursor(2);
		MAINDllOBJECT->ZoomToWindowExtents(1);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->DXFShape_Updated(false);
		MAINDllOBJECT->Shape_Updated(false);
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->Wait_DxfGraphicsUpdate();
		MAINDllOBJECT->update_VideoGraphics(true);
		MAINDllOBJECT->update_RcadGraphics(true);
		MAINDllOBJECT->update_DxfGraphics();

	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0003", __FILE__, __FUNCSIG__); }
}

//Load the dxf file.//
void DXFExpose::ImportDXFFile(char* filename, char* DXFFile, int DXFlength) //, bool ImportforFixedGraphicsDXF)
{ 	
	try
	{
		//Dont update the graphics untill load finishes...//
		MAINDllOBJECT->dontUpdateGraphcis = true;
		LoadDXF = true;
		TCHAR *pwc = (TCHAR*) malloc( sizeof(TCHAR) * (DXFlength + 4) );
		mbstowcs( pwc, DXFFile, DXFlength + 1 );

		Cdxf = new DXFCollection(pwc);
		
		DXFReader reader;
		reader.LoadFile(filename);
		
		//if (!ImportforFixedGraphicsDXF)
			MAINDllOBJECT->getCurrentUCS().getDxfCollecion().addItem(Cdxf);

		MAINDllOBJECT->dontUpdateGraphcis = false;
		selectDXF(Cdxf->getId());
		LoadDXF = false;
		free(pwc);
		//MAINDllOBJECT->DXFShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0004", __FILE__, __FUNCSIG__); }
}

//Write the shape parameters to dxf file..//
void DXFExpose::ExportDXFFile(char* filename, bool CloudPointAsPolyLine)
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		//DXF writer instance.///
		DXFWriter writer;
		writer.ResetHandleCount();
		bool SingleUcsExport = false;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			if(SingleUcsExport && !ExportShapesOfAllUcsToDxf)
				break;
			if(!ExportShapesOfAllUcsToDxf)
				Cucs = &MAINDllOBJECT->getCurrentUCS();
			SingleUcsExport = true;
			for(RC_ITER item = Cucs->getShapes().getList().begin(); item != Cucs->getShapes().getList().end(); item++)
			{
				try
				{
					ShapeWithList* Cshape = (ShapeWithList*)(*item).second;
					if(!Cshape->Normalshape()) continue;
					if(!Cshape->IsValid()) continue;
					if(Cshape->PointsList->Pointscount() < 1 && !Cshape->IsDxfShape() && !Cshape->getucsCopyParent().size() > 0) continue;
					int ColorCode = 7;
					double RGBSum = 1000;
					for(map<int, SinglePoint*>::iterator Citem = CADcolors.begin(); Citem != CADcolors.end(); Citem++)
					{
						SinglePoint* Spt = (*Citem).second;
						if(Cshape->ShapeColor.r * 255 == Spt->X && Cshape->ShapeColor.g * 255 == Spt->Y && Cshape->ShapeColor.b * 255 == Spt->Z)
						{
							ColorCode = Spt->PtID;
							break;
						}			
						else
						{
							double TmpValue = abs(Cshape->ShapeColor.r * 255 - Spt->X) + abs(Cshape->ShapeColor.g * 255 - Spt->Y) + abs(Cshape->ShapeColor.b * 255 - Spt->Z);
							if(RGBSum > TmpValue)
							{
								RGBSum = TmpValue;
								ColorCode = Spt->PtID;
							}
						}
					}
					if(ExportShapesOfAllUcsToDxf)
					{
						if(Cucs->UCSMode() == 1)
						{
							UCS *sourceUcs = MAINDllOBJECT->getUCS(Cshape->UcsId());
							UCS *targetUcs = MAINDllOBJECT->getUCS(0);
							double p1[2] = {sourceUcs->UCSPoint.getX(), sourceUcs->UCSPoint.getY()}, p2[2] = {targetUcs->UCSPoint.getX(), targetUcs->UCSPoint.getY()};
							double transformM[9] = {0};
							if(targetUcs->UCSProjectionType() == 1)  
							{
									transformM[1] = 1;   transformM[5] = 1;   transformM[6] = 1; 
							}
							else if(targetUcs->UCSProjectionType() == 0)
							{
									transformM[0] = 1;   transformM[4] = 1;   transformM[8] = 1; 
							}
							else if(targetUcs->UCSProjectionType() == 2) 
							{
									transformM[0] = 1;   transformM[5] = 1;   transformM[7] = 1; 
							}
							Shape* TempCshape = Cshape->Clone(0);
							TempCshape->CopyOriginalProperties(Cshape);
							if(TempCshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
								((CloudPoints*)TempCshape)->CopyPoints(Cshape);													
							Vector tmpv = Vector(p1[0] - p2[0], p1[1] - p2[1], sourceUcs->UCSPoint.getZ() -targetUcs->UCSPoint.getZ());
							TempCshape->Transform(transformM);
							TempCshape->Translate(tmpv);
							ExportDXFFile(&writer, (ShapeWithList*)TempCshape, ColorCode, CloudPointAsPolyLine);
							delete TempCshape;
						}
						else if(Cucs->UCSMode() == 2)
						{
							UCS *sourceUcs = MAINDllOBJECT->getUCS(Cshape->UcsId());
							UCS *targetUcs = MAINDllOBJECT->getUCS(0);
							double p1[2] = {sourceUcs->UCSPoint.getX(), sourceUcs->UCSPoint.getY()}, p2[2] = {targetUcs->UCSPoint.getX(), targetUcs->UCSPoint.getY()};
							double SourceAngle = sourceUcs->UCSangle(), DstAngle = targetUcs->UCSangle(), transformMatrix[9] = {0};
							double SourceIntercept = sourceUcs->UCSIntercept(), DstIntercept = targetUcs->UCSIntercept();
							double RotatedAngle, ShiftedOrigin[2] = { 0 };
							RMATH2DOBJECT->TransformM_PointLineAlign(&p2[0], DstAngle, DstIntercept, &p1[0], SourceAngle, SourceIntercept, &transformMatrix[0], &RotatedAngle, ShiftedOrigin);
							transformMatrix[2] = 0; transformMatrix[5] = 0;
							double transformMParent[9] = {0};
							if(targetUcs->ParentUcsId() != 0)
							{
								if(sourceUcs->UCSProjectionType() == 1)  
								{
									transformMParent[1] = 1;   transformMParent[5] = 1;   transformMParent[6] = 1; 
								}
								else if(sourceUcs->UCSProjectionType() == 0)
								{
									transformMParent[0] = 1;   transformMParent[4] = 1;   transformMParent[8] = 1; 
								}
								else if(sourceUcs->UCSProjectionType() == 2) 
								{
									transformMParent[0] = 1;   transformMParent[5] = 1;   transformMParent[7] = 1; 
								}	
							}
							Shape* TempCshape = Cshape->Clone(0);
							TempCshape->CopyOriginalProperties(Cshape);
							if(TempCshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
								((CloudPoints*)TempCshape)->CopyPoints(Cshape);
							if((targetUcs->ParentUcsId() != 0) && (Cshape->UcsId() == 0))
							{
								Vector tmpv = Vector(-p1[0], -p1[1], -sourceUcs->UCSPoint.getZ());
								TempCshape->Translate(tmpv);
								TempCshape->Transform(transformMParent);
							}
							Vector tmpv = Vector(p1[0] - p2[0], p1[1] - p2[1], sourceUcs->UCSPoint.getZ() -targetUcs->UCSPoint.getZ());
							TempCshape->Transform(transformMatrix);		
							TempCshape->Translate(tmpv);
							ExportDXFFile(&writer, (ShapeWithList*)TempCshape, ColorCode, CloudPointAsPolyLine);
							delete TempCshape;
						}
						else if(Cucs->UCSMode() == 4)
						{
							double transformMatrix_4X4[16] = {0};
							RMATH2DOBJECT->OperateMatrix4X4(&(MAINDllOBJECT->getCurrentUCS().transform[0]), 4, 1, transformMatrix_4X4);
							Shape* TempCshape = Cshape->Clone(0);
							TempCshape->CopyOriginalProperties(Cshape);
							if(TempCshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
								((CloudPoints*)TempCshape)->CopyPoints(Cshape);
							TempCshape->AlignToBasePlane(transformMatrix_4X4);
							ExportDXFFile(&writer, (ShapeWithList*)TempCshape, ColorCode, CloudPointAsPolyLine);
							delete TempCshape;
						}
						else if(Cucs->UCSMode() == 0)
						{
							ExportDXFFile(&writer, Cshape, ColorCode, CloudPointAsPolyLine);
						}
					}
					else
					{
						ExportDXFFile(&writer, Cshape, ColorCode, CloudPointAsPolyLine);
					}
				}
				catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0005", __FILE__, __FUNCSIG__); }
			}
		}
		//Write all the data into file...//
		writer.generateDXFFile(filename);
		MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0006", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ExportDXFFile(DXFWriter* writer, ShapeWithList* Cshape, int ColorCode, bool CloudPointAsPolyLine)
{ 	
	try
	{
		switch(Cshape->ShapeType)
		{
		case RapidEnums::SHAPETYPE::CIRCLE:	
			if(ExportShapeAsZequaltoZero)
				writer->addCircleParameter(((Circle*)Cshape)->getCenter()->getX(), ((Circle*)Cshape)->getCenter()->getY(), 0, ((Circle*)Cshape)->Radius(), ColorCode);
			else
				writer->addCircleParameter(((Circle*)Cshape)->getCenter()->getX(), ((Circle*)Cshape)->getCenter()->getY(), ((Circle*)Cshape)->getCenter()->getZ(), ((Circle*)Cshape)->Radius(), ColorCode);
			break;
		case RapidEnums::SHAPETYPE::LINE:
			if(ExportShapeAsZequaltoZero)
				writer->addLineParameter("LINE", ((Line*)Cshape)->getPoint1()->getX(),((Line*)Cshape)->getPoint1()->getY(), 0,
										((Line*)Cshape)->getPoint2()->getX(),((Line*)Cshape)->getPoint2()->getY(), 0,ColorCode);
			else
				writer->addLineParameter("LINE", ((Line*)Cshape)->getPoint1()->getX(),((Line*)Cshape)->getPoint1()->getY(),((Line*)Cshape)->getPoint1()->getZ(),
										((Line*)Cshape)->getPoint2()->getX(),((Line*)Cshape)->getPoint2()->getY(),((Line*)Cshape)->getPoint2()->getZ(),ColorCode);
			break;
		case RapidEnums::SHAPETYPE::LINE3D:
			writer->addLineParameter("LINE", ((Line*)Cshape)->getPoint1()->getX(),((Line*)Cshape)->getPoint1()->getY(),((Line*)Cshape)->getPoint1()->getZ(),
										((Line*)Cshape)->getPoint2()->getX(),((Line*)Cshape)->getPoint2()->getY(),((Line*)Cshape)->getPoint2()->getZ(),ColorCode);
			break;
		case RapidEnums::SHAPETYPE::CLOUDPOINTS:
			if(MAINDllOBJECT->TriangulateCpSurface())
			{
				if(((CloudPoints*)Cshape)->AllTrianglelist != NULL)
				{
					for(int i = 0; i < ((CloudPoints*)Cshape)->trianglecount; i++)
					{
						writer->addLineParameter("LINE", ((CloudPoints*)Cshape)->AllTrianglelist[6 * i], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 1], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 2], 
							((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 3], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 4], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 5], ColorCode);
					}
				}
			}
			else if(CloudPointAsPolyLine)
				writer->AddPolylineparameters2D(Cshape->PointsList, ColorCode);
			else
			{
				if(ExportShapeAsZequaltoZero && !Cshape->ShapeAs3DShape())
				{
					for(PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
					{
						SinglePoint* Spt = (*Item).second;
						writer->addPointparameter(Spt->X, Spt->Y, 0, ColorCode);
					}
				}
				else
				{
					for(PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
					{
						SinglePoint* Spt = (*Item).second;
						writer->addPointparameter(Spt->X, Spt->Y,Spt->Z, ColorCode);
					}
				}
			}
			break;
		case RapidEnums::SHAPETYPE::ARC:
			{
				double angle1 = ((Circle*)Cshape)->Startangle() + ((Circle*)Cshape)->Sweepangle();
				if(angle1 > 2 * M_PI) angle1 -= 2 * M_PI;
				if(ExportShapeAsZequaltoZero)
					writer->addArcParameter(((Circle*)Cshape)->getCenter()->getX(),((Circle*)Cshape)->getCenter()->getY(), 0,
												((Circle*)Cshape)->Radius(),((Circle*)Cshape)->Startangle(),angle1, ColorCode);
				else
					writer->addArcParameter(((Circle*)Cshape)->getCenter()->getX(),((Circle*)Cshape)->getCenter()->getY(),((Circle*)Cshape)->getCenter()->getZ(),
											((Circle*)Cshape)->Radius(),((Circle*)Cshape)->Startangle(),angle1, ColorCode);
			}
			break;
		case RapidEnums::SHAPETYPE::RPOINT:
			if(ExportShapeAsZequaltoZero && !Cshape->ShapeAs3DShape())
				writer->addPointparameter(((RPoint*)Cshape)->getPosition()->getX(),((RPoint*)Cshape)->getPosition()->getY(), 0, ColorCode);
			else
				writer->addPointparameter(((RPoint*)Cshape)->getPosition()->getX(),((RPoint*)Cshape)->getPosition()->getY(),((RPoint*)Cshape)->getPosition()->getZ(), ColorCode);
			break;
		case RapidEnums::SHAPETYPE::RPOINT3D:
		case RapidEnums::SHAPETYPE::RELATIVEPOINT:
			writer->addPointparameter(((RPoint*)Cshape)->getPosition()->getX(),((RPoint*)Cshape)->getPosition()->getY(),((RPoint*)Cshape)->getPosition()->getZ(), ColorCode);
			break;
		case RapidEnums::SHAPETYPE::XLINE:
		case RapidEnums::SHAPETYPE::XLINE3D:
			{
				double Direction_Ratio[3] = {0}, LineEndPts[6] = {((Line*)Cshape)->getPoint2()->getX(), ((Line*)Cshape)->getPoint2()->getY(), ((Line*)Cshape)->getPoint2()->getZ(),
					((Line*)Cshape)->getPoint1()->getX(), ((Line*)Cshape)->getPoint1()->getY(), ((Line*)Cshape)->getPoint1()->getZ()};
				RMATH3DOBJECT->DCs_TwoEndPointsofLine(LineEndPts, Direction_Ratio);
				writer->addXLineParameter("XLINE",((Line*)Cshape)->getPoint1()->getX(),((Line*)Cshape)->getPoint1()->getY(),((Line*)Cshape)->getPoint1()->getZ(), 
					Direction_Ratio[0], Direction_Ratio[1], Direction_Ratio[2], ColorCode);
			}
			break;
		case RapidEnums::SHAPETYPE::XRAY:
			{
				double Direction_Ratio[3] = {0}, LineEndPts[6] = {((Line*)Cshape)->getPoint2()->getX(), ((Line*)Cshape)->getPoint2()->getY(), ((Line*)Cshape)->getPoint2()->getZ(),
					((Line*)Cshape)->getPoint1()->getX(), ((Line*)Cshape)->getPoint1()->getY(), ((Line*)Cshape)->getPoint1()->getZ()};
				RMATH3DOBJECT->DCs_TwoEndPointsofLine(LineEndPts, Direction_Ratio);
				writer->addRayParameter("RAY",((Line*)Cshape)->getPoint1()->getX(), ((Line*)Cshape)->getPoint1()->getY(), ((Line*)Cshape)->getPoint1()->getZ(),
										Direction_Ratio[0], Direction_Ratio[1], Direction_Ratio[2], ColorCode);
			}
			break;
		case RapidEnums::SHAPETYPE::POLYLINE:
			{
				writer->AddPolylineparameters2D(Cshape->PointsList, ColorCode);
			}
			break;
		case RapidEnums::SHAPETYPE::SPLINE:
			{
				if(((Spline*)Cshape)->ControlPointsList->Pointscount() < 4) break;
				writer->AddSplineParameter(((Spline*)Cshape)->ControlPointsList, ((Spline*)Cshape)->Spline_knots, ((Spline*)Cshape)->KnotsArray, ((Spline*)Cshape)->ClosedSpline, ColorCode);
			}
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0006b", __FILE__, __FUNCSIG__); }
}

//Load the drl drill file.//
void DXFExpose::ImportDRLFile(char* filename,char* DRLFile,int DRLlength)
{ 	
	try
	{
		//Dont update the graphics untill load finishes...//
		MAINDllOBJECT->dontUpdateGraphcis = true;
		LoadDXF = true;
		TCHAR *pwc = (TCHAR*) malloc( sizeof(TCHAR) * (DRLlength + 4) );
		mbstowcs( pwc, DRLFile, DRLlength + 1 );

		Cdxf = new DXFCollection(pwc);
		
		DXFReader reader;
		reader.LoadDrillFile(filename);

		MAINDllOBJECT->getCurrentUCS().getDxfCollecion().addItem(Cdxf);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		selectDXF(Cdxf->getId());
		LoadDXF = false;
		free(pwc);
		//MAINDllOBJECT->DXFShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0006a", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ImportIGESFile(char* filename, char* igesFile, int filelength)
{
	try
	{
		//Dont update the graphics untill load finishes...//
			MAINDllOBJECT->dontUpdateGraphcis = true;
		LoadDXF = true;
		TCHAR *pwc = (TCHAR*) malloc( sizeof(TCHAR) * (filelength + 4) );
		mbstowcs( pwc, igesFile, filelength + 1 );

		Cdxf = new DXFCollection(pwc);
		
		IGESReader reader;
		reader.LoadFile(filename);

		MAINDllOBJECT->getCurrentUCS().getDxfCollecion().addItem(Cdxf);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		selectDXF(Cdxf->getId());
		LoadDXF = false;
		free(pwc);
		//MAINDllOBJECT->DXFShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0004", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ExportIGESFile(char* filename)
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		double angle1;
		//DXF writer instance.///
		IGESWriter writer;
		//Go through the collection of shapes and write aaccording to the shape type..//
		for(RC_ITER item = MAINDllOBJECT->getShapesList().getList().begin(); item !=   MAINDllOBJECT->getShapesList().getList().end(); item++)
		{
			try
			{
				ShapeWithList* Cshape = (ShapeWithList*)(*item).second;
				if(!Cshape->Normalshape()) continue;
				switch(Cshape->ShapeType)
				{
				case RapidEnums::SHAPETYPE::CYLINDER:	
					writer.addCylinderParameter(((Cylinder*)Cshape)->getCenter1()->getX(), ((Cylinder*)Cshape)->getCenter1()->getY(), ((Cylinder*)Cshape)->getCenter1()->getZ(), ((Cylinder*)Cshape)->getCenter2()->getX(),
						((Cylinder*)Cshape)->getCenter2()->getY(), ((Cylinder*)Cshape)->getCenter2()->getZ(), ((Cylinder*)Cshape)->Radius1(), ((Cylinder*)Cshape)->Radius2());
					break;

				case RapidEnums::SHAPETYPE::CONE:
					writer.addConeParameter(((Cone*)Cshape)->getCenter1()->getX(), ((Cone*)Cshape)->getCenter1()->getY(), ((Cone*)Cshape)->getCenter1()->getZ(), ((Cone*)Cshape)->getCenter2()->getX(),
						((Cone*)Cshape)->getCenter2()->getY(), ((Cone*)Cshape)->getCenter2()->getZ(), ((Cone*)Cshape)->Radius1(), ((Cone*)Cshape)->Radius2());
					break;

				case RapidEnums::SHAPETYPE::SPHERE:
					writer.addSphereParameter(((Sphere*)Cshape)->getCenter()->getX(), ((Sphere*)Cshape)->getCenter()->getY(), ((Sphere*)Cshape)->getCenter()->getZ(),((Sphere*)Cshape)->Radius());
					break;

					
				case RapidEnums::SHAPETYPE::PLANE:
					writer.addPlaneParameter(((Plane*)Cshape)->planePolygonPts, ((Plane*)Cshape)->getPoint1()->getX(), ((Plane*)Cshape)->getPoint1()->getY(), ((Plane*)Cshape)->getPoint1()->getZ(),  ((Plane*)Cshape)->getPoint2()->getX (),
						((Plane*)Cshape)->getPoint2()->getY(), ((Plane*)Cshape)->getPoint2()->getZ(), ((Plane*)Cshape)->getPoint3()->getX(), ((Plane*)Cshape)->getPoint3()->getY(), ((Plane*)Cshape)->getPoint3()->getZ());
					//writer.addPlaneParameter(((Plane*)Cshape)->getPoint1()->getX(), ((Plane*)Cshape)->getPoint1()->getY(), ((Plane*)Cshape)->getPoint1()->getZ(), ((Plane*)Cshape)->getPoint2()->getX (),
						//((Plane*)Cshape)->getPoint2()->getY(), ((Plane*)Cshape)->getPoint2()->getZ(), ((Plane*)Cshape)->getPoint3()->getX(), ((Plane*)Cshape)->getPoint3()->getY(), ((Plane*)Cshape)->getPoint3()->getZ());
					break;

				case RapidEnums::SHAPETYPE::LINE3D:
				  writer.addLine3DParameter(((Line*)Cshape)->getPoint1()->getX(), ((Line*)Cshape)->getPoint1()->getY(), ((Line*)Cshape)->getPoint1()->getZ(),
					  ((Line*)Cshape)->getPoint2()->getX(), ((Line*)Cshape)->getPoint2()->getY(), ((Line*)Cshape)->getPoint2()->getZ());
				  break;

				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					if (MAINDllOBJECT->TriangulateCpSurface())
					{
						if (((CloudPoints*)Cshape)->AllTrianglelist != NULL)
						{
							for (int i = 0; i < ((CloudPoints*)Cshape)->trianglecount; i++)
							{
								writer.addLine3DParameter(((CloudPoints*)Cshape)->AllTrianglelist[6 * i], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 1], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 2],
									((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 3], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 4], ((CloudPoints*)Cshape)->AllTrianglelist[6 * i + 5]);
							}
						}
					}
					else
					{
						if (ExportShapeAsZequaltoZero && !Cshape->ShapeAs3DShape())
						{
							for (PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								writer.addPointparameter(Spt->X, Spt->Y, 0);
							}
						}
						else
						{
							for (PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								writer.addPointparameter(Spt->X, Spt->Y, Spt->Z);
							}
						}
					}
					break;
				}
			}
			catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0005", __FILE__, __FUNCSIG__); }
		}
		//Write all the data into file...//
		writer.generateDXFFile(filename);
		MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0006", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ImportSTLFile(char* filename, char* igesFile, int filelength)
{
	try
	{
		//Dont update the graphics untill load finishes...//
			MAINDllOBJECT->dontUpdateGraphcis = true;
		LoadDXF = true;
		TCHAR *pwc = (TCHAR*) malloc( sizeof(TCHAR) * (filelength + 4) );
		mbstowcs( pwc, igesFile, filelength + 1 );

		Cdxf = new DXFCollection(pwc);
		
		StlReader reader;
		reader.LoadFile(filename);

		MAINDllOBJECT->getCurrentUCS().getDxfCollecion().addItem(Cdxf);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		selectDXF(Cdxf->getId());
		LoadDXF = false;
		free(pwc);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0004a", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ExportSTLFile(char* filename)
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		//STL writer instance.///
		STLWriter writer;
		//Go through the collection of shapes and write aaccording to the shape type..//
		for(RC_ITER item = MAINDllOBJECT->getShapesList().getList().begin(); item !=   MAINDllOBJECT->getShapesList().getList().end(); item++)
		{
			try
			{
				ShapeWithList* Cshape = (ShapeWithList*)(*item).second;
				if(!Cshape->Normalshape()) continue;
				switch(Cshape->ShapeType)
				{
				case RapidEnums::SHAPETYPE::TRIANGULARSURFACE:	
					{
						for(((TriangularSurface*)Cshape)->TShapeitr = ((TriangularSurface*)Cshape)->TriangleCollection.begin(); ((TriangularSurface*)Cshape)->TShapeitr != ((TriangularSurface*)Cshape)->TriangleCollection.end(); ((TriangularSurface*)Cshape)->TShapeitr++)
						{
							double SurfaceParam[12] = {((TriangularSurface*)Cshape)->TShapeitr->point1[0], ((TriangularSurface*)Cshape)->TShapeitr->point1[1], ((TriangularSurface*)Cshape)->TShapeitr->point1[2], ((TriangularSurface*)Cshape)->TShapeitr->point2[0],
								((TriangularSurface*)Cshape)->TShapeitr->point2[1], ((TriangularSurface*)Cshape)->TShapeitr->point2[2], ((TriangularSurface*)Cshape)->TShapeitr->point3[0], ((TriangularSurface*)Cshape)->TShapeitr->point3[1],
								((TriangularSurface*)Cshape)->TShapeitr->point3[2], ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[0], ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[1], ((TriangularSurface*)Cshape)->TShapeitr->dirCosine[2]};
							writer.AddSurfaceParameter(SurfaceParam);
						}
					}
					break;
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					if(MAINDllOBJECT->TriangulateCpSurface())
					{
						if(((CloudPoints*)Cshape)->AllTrianglelist != NULL)
						{
							for(int i = 0; i < ((CloudPoints*)Cshape)->trianglecount; i++)
							{
								double PlaneParams[4] = {0};
								BESTFITOBJECT->Plane_3Pt_2(&((CloudPoints*)Cshape)->AllTrianglelist[9 * i], PlaneParams);
								double SurfaceParam[12] = {((CloudPoints*)Cshape)->AllTrianglelist[9 * i], ((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 1], ((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 2],
									((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 3], ((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 4], ((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 5], ((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 6],
									((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 7], ((CloudPoints*)Cshape)->AllTrianglelist[9 * i + 8], PlaneParams[0], PlaneParams[1], PlaneParams[2]};
								writer.AddSurfaceParameter(SurfaceParam);
							}
						}
					}
					break;
				}
			}
			catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0005a", __FILE__, __FUNCSIG__); }
		}
		//Write all the data into file...//
		writer.generateSTLFile(filename);
		MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0006a", __FILE__, __FUNCSIG__); }
}

// Function to import STEP file
void DXFExpose::ImportSTEPFile(char* filename, char* igesFile, int filelength)
{
	//Dont update the graphics untill load finishes...//
		MAINDllOBJECT->dontUpdateGraphcis = true;
		LoadDXF = true;
		TCHAR *pwc = (TCHAR*) malloc( sizeof(TCHAR) * (filelength + 4) );
		mbstowcs( pwc, igesFile, filelength + 1 );

		Cdxf = new DXFCollection(pwc);
		
		/*IGESReader reader;
		reader.LoadFile(filename);*/
		StepReader reader;
		reader.LoadFile(filename);

		MAINDllOBJECT->getCurrentUCS().getDxfCollecion().addItem(Cdxf);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		selectDXF(Cdxf->getId());
		LoadDXF = false;
		free(pwc);
		//MAINDllOBJECT->DXFShape_Updated();
}

//Function to Import NC file for reading CAM file to generate program
void DXFExpose::ImportNCFile(char* filename, char* NCFile, int filelength)
{
	try
	{
		//Dont update the graphics untill load finishes...//
		MAINDllOBJECT->dontUpdateGraphcis = true;
		LoadDXF = true;
		TCHAR *pwc = (TCHAR*)malloc(sizeof(TCHAR) * (filelength + 4));
		mbstowcs(pwc, NCFile, filelength + 1);

		Cdxf = new DXFCollection(pwc);

		DXFReader reader;
		reader.LoadNCFile(filename);

		//Add code here to add all shapes in cdxf to main Shapes collection

		//MAINDllOBJECT->getCurrentUCS().getDxfCollecion().addItem(Cdxf);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		selectDXF(Cdxf->getId());
		LoadDXF = false;
		free(pwc);
		//MAINDllOBJECT->DXFShape_Updated();
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0006a", __FILE__, __FUNCSIG__); }
}


//Function to export all the points to DXF
void DXFExpose::ExportShapePointsToDXF(char* filename, bool OnlySelectedShapes)
{
	try
	{
		if(OnlySelectedShapes)
			ExportPointsToDXF(filename, MAINDllOBJECT->getSelectedShapesList());
		else
			ExportPointsToDXF(filename, MAINDllOBJECT->getShapesList());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0008", __FILE__, __FUNCSIG__); }
}

//Function to export all the points to text File
void DXFExpose::ExportShapePointsToText(char* filename, bool OnlySelectedShapes)
{
	try
	{
		if(OnlySelectedShapes)
			ExportPointsToText(filename, MAINDllOBJECT->getSelectedShapesList());
		else
			ExportPointsToText(filename, MAINDllOBJECT->getShapesList());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0008", __FILE__, __FUNCSIG__); }
}

//Function to export shape parameters to csv File
void DXFExpose::ExportShapePointsToCsv(char* filename, bool OnlySelectedShapes)
{
	try
	{
		if(OnlySelectedShapes)
			ExportPointsToCsv(filename, MAINDllOBJECT->getSelectedShapesList());
		else
			ExportPointsToCsv(filename, MAINDllOBJECT->getShapesList());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0008", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ExportShapeParametersToCsv(char* filename, bool OnlySelectedShapes)
{
	try
	{
		RCollectionBase ShapeCollection;
		if(OnlySelectedShapes)
		   ShapeCollection = MAINDllOBJECT->getSelectedShapesList(); 
		else
		   ShapeCollection = MAINDllOBJECT->getShapesList(); 
		MAINDllOBJECT->dontUpdateGraphcis = true;
		std::wofstream myfile;
		myfile.open(filename);
		for(RC_ITER ShItem = ShapeCollection.getList().begin(); ShItem != ShapeCollection.getList().end(); ShItem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)(*ShItem).second;
			if(!CShape->Normalshape())continue;
			WriteShapeParametersToCsv(myfile, CShape);
		}
		myfile.close();
		MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0008b", __FILE__, __FUNCSIG__); }
}

void DXFExpose::WriteShapeParametersToCsv(std::wofstream& myfile, ShapeWithList *CShape)
{
	try
	{
		switch(CShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
				{
					myfile << endl << "Line : " << CShape->getModifiedName() << endl;
					double param[6] = {0};
					((Line*)CShape)->getParameters(param);
					myfile << "Px" << param[0] << "," << "Py" << param[1] << "," << "Pz" << param[2] << "," << "Ang" << ((Line*)CShape)->Angle() << "," << "Intercept" << ((Line*)CShape)->Intercept() << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::CIRCLE:
				{
					myfile << endl << "Circle : " << CShape->getModifiedName() << endl;
					double param[7] = {0};
					((Circle*)CShape)->getParameters(param);
					myfile << "Cx" << param[0] << "," << "Cy" << param[1] << "," << "Cz" << param[2] << "," << "Radius" << param[6] << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::XLINE:
				{
					myfile << endl << "Xline : " << CShape->getModifiedName() << endl;
					double param[6] = {0};
					((Line*)CShape)->getParameters(param);
					myfile << "Px" << param[0] << "," << "Py" << param[1] << "," << "Pz" << param[2] << "," << "Ang" << ((Line*)CShape)->Angle() << "," << "Intercept" << ((Line*)CShape)->Intercept() << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::XLINE3D:
				{
					myfile << endl << "Xline3D : " << CShape->getModifiedName() << endl;
					double param[6] = {0};
					((Line*)CShape)->getParameters(param);
					myfile << "Px" << param[0] << "," << "Py" << param[1] << "," << "Pz" << param[2] << "," << "Ang" << ((Line*)CShape)->Angle() << "," << "Intercept" << ((Line*)CShape)->Intercept() << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::ARC:
				{
					myfile << endl << "Arc : " << CShape->getModifiedName() << endl;
					double param[7] = {0};
					((Circle*)CShape)->getParameters(param);
					myfile << "Cx" << param[0] << "," << "Cy" << param[1] << "," << "Cz" << param[2] << "," << "Radius" << param[6] << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::XRAY:
				{
					myfile << endl << "Xray : " << CShape->getModifiedName() << endl;
					double param[6] = {0};
					((Line*)CShape)->getParameters(param);
					myfile << "Px" << param[0] << "," << "Py" << param[1] << "," << "Pz" << param[2] << "," << "Ang" << ((Line*)CShape)->Angle() << "," << "Intercept" << ((Line*)CShape)->Intercept() << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::RPOINT:
				{
					myfile << endl << "Point : " << CShape->getModifiedName() << endl;
					double param[3] = {0};
					Vector *tmpV = ((RPoint*)CShape)->getPosition();
					myfile << "Px" << tmpV->getX() << "," << "Py" << tmpV->getY() << "," << "Pz" << tmpV->getZ() << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::RPOINT3D:
				{
					myfile << endl << "Point3D : " << CShape->getModifiedName() << endl;
					double param[3] = {0};
					Vector *tmpV = ((RPoint*)CShape)->getPosition();
					myfile << "Px" << tmpV->getX() << "," << "Py" << tmpV->getY() << "," << "Pz" << tmpV->getZ() << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::LINE3D:
				{
					myfile << endl << "Line3D : " << CShape->getModifiedName() << endl;
					double param[6] = {0};
					((Line*)CShape)->getParameters(param);
					myfile << "Px" << param[0] << "," << "Py" << param[1] << "," << "Pz" << param[2] << "," << "l" << param[3] << "," << "m" << param[4] << "," << "n" << param[5] << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::CYLINDER:
				{
					myfile << endl << "Cylinder : " << CShape->getModifiedName() << endl;
					double param[7] = {0};
					((Cylinder*)CShape)->getParameters(param);
					myfile << "Px"  << ", " << "Py" << ", " << "Pz" << ", " << "L" << ", " << "M" << ", " << "N" << ", " << "Rad" << endl;
					myfile << param[0] << ", " << param[1] << ", " << param[2] << ", " << param[3] << ", " << param[4] << ", " << param[5] << ", " << param[6] << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::SPHERE:
				{
					myfile << endl << "Sphere : " << CShape->getModifiedName() << endl;
					double param[4] = {0};
					((Sphere*)CShape)->getParameters(param);
					myfile << "Cx" << param[0] << "," << "Cy" << param[1] << "," << "Cz" << param[2] << "," << "radius" << param[3] << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::PLANE:
				{
					myfile << endl << "Plane : " << CShape->getModifiedName() << endl;
					double param[4] = {0};
					((Plane*)CShape)->getParameters(param);
					myfile << "l" << param[0] << "," << "m" << param[1] << "," << "n" << param[2] << "," << "d" << param[3] << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::CIRCLE3D:
				{
					myfile << endl << "3D Circle : " << CShape->getModifiedName() << endl;
					double param[7] = {0};
					((Circle*)CShape)->getParameters(param);
					myfile << "Cx" << param[0] << "," << "Cy" << param[1] << "," << "Cz" << param[2] << "," << "l" << param[3] << "," << "m" << param[4] << "," << "n" << param[5] << "," << "radius" << param[6] << endl;
					break;
				}
				case RapidEnums::SHAPETYPE::CONE:
				{
					myfile << endl << "Cone : " << CShape->getModifiedName() << endl;
					double param[8] = {0};
					((Cylinder*)CShape)->getParameters(param);
					myfile << "Px" << param[0] << "," << "Py" << param[1] << "," << "Pz" << param[2] << "," << "l" << param[3] << "," << "m" << param[4] << "," << "n" << param[5] << "," << "radius1" << param[6]<< "," << "radius2" << param[7] << endl;
					break;
				}
			}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0008c", __FILE__, __FUNCSIG__); }
}

//Export points to CAD..
void DXFExpose::ExportPointsToDXF(char* filename, RCollectionBase& ShapeCollection)
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		DXFWriter writer;
	
		for(RC_ITER ShItem = ShapeCollection.getList().begin(); ShItem != ShapeCollection.getList().end(); ShItem++)
		{
			int ColorCode = 7;
			ShapeWithList* CShape = (ShapeWithList*)(*ShItem).second;
			if(!CShape->Normalshape()) continue;
			if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS) ColorCode = 1;
			for(PC_ITER PtItem = CShape->PointsList->getList().begin(); PtItem != CShape->PointsList->getList().end(); PtItem++)
			{
				SinglePoint* Spt = (*PtItem).second;
				writer.addPointparameter(Spt->X, Spt->Y, Spt->Z, ColorCode);
			}
		}
		
		writer.generateDXFFile(filename);
		MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0008", __FILE__, __FUNCSIG__); }
}

//Function to export all the points to text File
void DXFExpose::ExportPointsToText(char* filename, RCollectionBase& ShapeCollection)
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		RapidEnums::RAPIDPROJECTIONTYPE Cptype = RapidEnums::RAPIDPROJECTIONTYPE(MAINDllOBJECT->getCurrentUCS().UCSProjectionType());
		
		std::wofstream myfile;
		myfile.open(filename);
		for(RC_ITER ShItem = ShapeCollection.getList().begin(); ShItem != ShapeCollection.getList().end(); ShItem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)(*ShItem).second;
			if(!CShape->Normalshape())continue;
			switch(CShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
					myfile << endl << "Line : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CIRCLE:
					myfile << endl << "Circle : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::XLINE:
					myfile << endl << "Xline : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::XLINE3D:
					myfile << endl << "Xline3D : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::ARC:
					myfile << endl << "Arc : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::XRAY:
					myfile << endl << "Xray : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::RPOINT:
					myfile << endl << "Point : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::RPOINT3D:
					myfile << endl << "Point3D : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::LINE3D:
					myfile << endl << "Line3D : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CYLINDER:
					myfile << endl << "Cylinder : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::SPHERE:
					myfile << endl << "Sphere : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::PLANE:
					myfile << endl << "Plane : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					myfile << endl << "CloudPoints : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CIRCLE3D:
					myfile << endl << "3D Circle : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CONE:
					myfile << endl << "Cone : " << CShape->getModifiedName() << endl;
					break;
			}
			switch(CShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
				case RapidEnums::SHAPETYPE::CIRCLE:
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XLINE3D:
				case RapidEnums::SHAPETYPE::ARC:
				case RapidEnums::SHAPETYPE::XRAY:
				case RapidEnums::SHAPETYPE::RPOINT:
				case RapidEnums::SHAPETYPE::RPOINT3D:
				case RapidEnums::SHAPETYPE::LINE3D:
				case RapidEnums::SHAPETYPE::CYLINDER:
				case RapidEnums::SHAPETYPE::SPHERE:
				case RapidEnums::SHAPETYPE::PLANE:
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
				case RapidEnums::SHAPETYPE::CIRCLE3D:
				case RapidEnums::SHAPETYPE::CONE:
					{
						myfile << "  X  " << '\t' << '\t' << "  Y  " << '\t' << '\t' << "  Z  " << endl;
						for(PC_ITER PtItem = CShape->PointsList->getList().begin(); PtItem != CShape->PointsList->getList().end(); PtItem++)
						{
							SinglePoint* Spt = (*PtItem).second;
							if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XY)
								myfile << Spt->X << '\t' << '\t' <<  Spt->Y << '\t' << '\t' << Spt->Z << endl;
							else if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZ || Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
								myfile << Spt->Y << '\t' << '\t' <<  Spt->Z << '\t' << '\t' << Spt->X << endl;
							else if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZ || Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
								myfile << Spt->X << '\t' << '\t' <<  Spt->Z << '\t' << '\t' << Spt->Y << endl;
						}
						myfile << endl;
					}
					break;
				case RapidEnums::SHAPETYPE::DOTSCOUNT:
					{
						myfile << "FrameArea " << ((DotsCount*)CShape)->FrameArea << endl;
						myfile << "  Center_X  " << '\t' << '\t' << "  Center_Y  " << '\t' << '\t' << "  Radius  " << endl;
						for each(double* TempPoint in ((DotsCount*)CShape)->CircleCollection)
						{
							myfile << TempPoint[0] << '\t' << '\t' << TempPoint[1] << '\t' << '\t' << TempPoint[2] << endl;
						}
					}
					break;
			}
		}
		myfile.close();
		MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0009", __FILE__, __FUNCSIG__); }
}

//Function to export shape parameters to csv File
void DXFExpose::ExportPointsToCsv(char* filename, RCollectionBase& ShapeCollection)
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		RapidEnums::RAPIDPROJECTIONTYPE Cptype = RapidEnums::RAPIDPROJECTIONTYPE(MAINDllOBJECT->getCurrentUCS().UCSProjectionType());
		
		std::wofstream myfile;
		myfile.open(filename);
		for(RC_ITER ShItem = ShapeCollection.getList().begin(); ShItem != ShapeCollection.getList().end(); ShItem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)(*ShItem).second;
			if(!CShape->Normalshape())continue;
			switch(CShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
					myfile << endl << "Line : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CIRCLE:
					myfile << endl << "Circle : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::XLINE:
					myfile << endl << "Xline : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::XLINE3D:
					myfile << endl << "Xline3D : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::ARC:
					myfile << endl << "Arc : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::XRAY:
					myfile << endl << "Xray : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::RPOINT:
					myfile << endl << "Point : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::RPOINT3D:
					myfile << endl << "Point3D : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::LINE3D:
					myfile << endl << "Line3D : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CYLINDER:
					myfile << endl << "Cylinder : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::SPHERE:
					myfile << endl << "Sphere : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::PLANE:
					myfile << endl << "Plane : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					myfile << endl << "CloudPoints : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CIRCLE3D:
					myfile << endl << "3D Circle : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::CONE:
					myfile << endl << "Cone : " << CShape->getModifiedName() << endl;
					break;
				case RapidEnums::SHAPETYPE::DOTSCOUNT:
					myfile << endl << "DotsCount : " << CShape->getModifiedName() << endl;
					break;
			}
			int Cnt = 1;
			double dia = 0, O_dia = 0, dir_hit = 10;
			switch(CShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XLINE3D:
				case RapidEnums::SHAPETYPE::XRAY:
				case RapidEnums::SHAPETYPE::RPOINT:
				case RapidEnums::SHAPETYPE::RPOINT3D:
				case RapidEnums::SHAPETYPE::LINE3D:
				case RapidEnums::SHAPETYPE::CYLINDER:
				case RapidEnums::SHAPETYPE::SPHERE:
				case RapidEnums::SHAPETYPE::PLANE:
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
				case RapidEnums::SHAPETYPE::CONE:
					{
						Cnt = 1;
						myfile << "S.NO." << "," << "  X  " << ","  << "  Y  " << "," << "  Z  " << "," << "Dir_hit" << endl;
						for(PC_ITER PtItem = CShape->PointsList->getList().begin(); PtItem != CShape->PointsList->getList().end(); PtItem++)
						{
							SinglePoint* Spt = (*PtItem).second;
							if(Spt->Pdir == 0)
								dir_hit = -1;
							else if(Spt->Pdir == 1)
								dir_hit = 1;
							else if(Spt->Pdir == 2)
								dir_hit = -2;
							else if(Spt->Pdir == 3)
								dir_hit = 2;
							else if(Spt->Pdir == 4)
								dir_hit = 3;
							if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XY)
								myfile << Cnt << "," << Spt->X << "," << Spt->Y << "," << Spt->Z << "," << dir_hit << endl;
							else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZ || Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
								myfile << Cnt << "," << Spt->Y << "," << Spt->Z << "," << Spt->X << "," << dir_hit << endl;
							else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZ || Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
								myfile << Cnt << "," << Spt->X << "," << Spt->Z << "," << Spt->Y << "," << dir_hit << endl;
							Cnt++;
						}
						myfile << endl;
					}
					break;
				case RapidEnums::SHAPETYPE::ARC:
				case RapidEnums::SHAPETYPE::CIRCLE:
				case RapidEnums::SHAPETYPE::CIRCLE3D:
					{
						Cnt = 1;
						myfile << "S.NO." << "," << "  X  " << ","  << "  Y  " << "," << "  Z  " << "," << "Dir_hit" << endl;
						for(PC_ITER Item = CShape->PointsList->getList().begin(); Item != CShape->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							if(Spt->Pdir == 0)
								dir_hit = -1;
							else if(Spt->Pdir == 1)
								dir_hit = 1;
							else if(Spt->Pdir == 2)
								dir_hit = -2;
							else if(Spt->Pdir == 3)
								dir_hit = 2;
							else if(Spt->Pdir == 4)
								dir_hit = 3;
							if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XY)
								myfile << Cnt << "," << Spt->X << "," << Spt->Y << "," << Spt->Z << "," << dir_hit << endl;
							else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZ || Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
								myfile << Cnt << "," << Spt->Y << "," << Spt->Z << "," << Spt->X << "," << dir_hit << endl;
							else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZ || Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
								myfile << Cnt << "," << Spt->X << "," << Spt->Z << "," << Spt->Y << "," << dir_hit << endl;
							Cnt++;
						}
						myfile << endl;
						dia = ((Circle*)CShape)->Radius() * 2;
						O_dia = ((Circle*)CShape)->OriginalRadius() * 2;
						double TpC = RMATH2DOBJECT->Pt2Pt_distance(0, 0, ((Circle*)CShape)->getCenter()->getX(), ((Circle*)CShape)->getCenter()->getY());
						myfile << "Diameter" << "," << dia << endl;
						myfile << "Diameter_WithoutCompensation" << "," << O_dia << endl;
						myfile << "CenterX" << "," << ((Circle*)CShape)->getCenter()->getX() << endl;
						myfile << "CenterY" << "," << ((Circle*)CShape)->getCenter()->getY() << endl;
						myfile << "MIC" << "," << ((Circle*)CShape)->MinInscribedia() << endl;
						myfile << "MCC" << "," << ((Circle*)CShape)->MaxCircumdia() << endl;
						myfile << "Circularity" << "," << ((Circle*)CShape)->Circularity() << endl;
						myfile << "TruePosition" << "," << TpC << endl;
						myfile << endl;
					}
					break;
				case RapidEnums::SHAPETYPE::DOTSCOUNT:
					{
						Cnt = 1;
						myfile << "FrameArea " << "," << ((DotsCount*)CShape)->FrameArea << endl;
						myfile << "S.NO." << "," << "  Center_X  " << ","  << "  Center_Y  " << "," << "  Radius  " << endl;
						for each(double* TempPoint in ((DotsCount*)CShape)->CircleCollection)
						{
							myfile << Cnt++ << "," << TempPoint[0] << "," << TempPoint[1] << "," << TempPoint[2] << endl;
						}
					}
					break;
			}
		}
		myfile.close();
		MAINDllOBJECT->dontUpdateGraphcis = false;
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0010", __FILE__, __FUNCSIG__); }
}

//Adds the dxf measurements to the dxf collection...//
void DXFExpose::AddDXFMeasurement(DimBase* Obj)
{
	try
	{
		Obj->IsValid(true);
		this->Cdxf->getMeasureCollection().addItemWithoutId(Obj);
		DimBase* Cdim = Obj->CreateDummyCopy();
		this->Cdxf->getOriginalMeasureCollection().addItem(Cdim, false);
		this->CurrentDXFMeasureList.push_back(Obj);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0011", __FILE__, __FUNCSIG__); }
}

//Adds DXF shapes to current dxf collection.....//
void DXFExpose::AddDXFShapes(Shape* shape)
{
	try
	{
		shape->IsValid(true);
		shape->UpdateEnclosedRectangle();
		this->Cdxf->getShapeCollection().addItemWithoutId(shape);
		Shape* CShape = shape->CreateDummyCopy();
		CShape->UpdateEnclosedRectangle();
		this->Cdxf->getOriginalShapeCollection().addItem(CShape, false);
		CurrentDXFShapeList.push_back(shape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0012", __FILE__, __FUNCSIG__); }
}

//transformation matrix...//
void DXFExpose::TransformMatrix(double *trans, bool DxfAlign)
{
	try
	{
		for(int i = 0; i < 9; i++)
			transform[i] = trans[i];
		if(!DxfAlign)
			EditedCadTransform(transform);
		else
		{
			this->AlignDxfShapes(true);
			this->DoThreeDAlignment(false);
			if(PPCALCOBJECT->PPAlignEditFlag())
				PPCALCOBJECT->TrnasformProgram_TwoStepHoming(&transform[0]);
			else
				CADalignment(&transform[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0013", __FILE__, __FUNCSIG__); }
}

void DXFExpose::SetTransVector(double *Pt1, double *Pt2, bool DxfAlign)
{
	try
	{
		Translate_Vector.set(Pt2[0] - Pt1[0], Pt2[1] - Pt1[1]);
		SetTransVector(Translate_Vector, DxfAlign);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0013a", __FILE__, __FUNCSIG__); }
}

void DXFExpose::SetTransVector(Vector& TransVect, bool DxfAlign)
{
	try
	{
		if (DxfAlign)
		{
			CADTranslate(TransVect); AlignFlag = true;
		}
		else
			EditedCadTranslate(TransVect);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0013b", __FILE__, __FUNCSIG__); }
}

//function to handle 3D transform matrix.
void DXFExpose::TransformMatrix3D_1Line(double *trans, double *translate, double *Dxfpt, bool DxfAlign)
{
	try
	{
		if(DxfAlign)
		{
			for(int i = 0; i < 16; i++)
				transform3D[i] = trans[i];
			userdefinedpt.set(translate[0], translate[1], translate[2]);
			spherecentrept.set(-Dxfpt[0], -Dxfpt[1], -Dxfpt[2]);
			this->AlignDxfShapes(true);
			this->DoThreeDAlignment(true);
			CADalignment(&transform[0]);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
			MAINDllOBJECT->SetStatusCode("Finished");
		}
		else
		{
			userdefinedpt.set(translate[0], translate[1], translate[2]);
			spherecentrept.set(-Dxfpt[0], -Dxfpt[1], -Dxfpt[2]);
			EditedCadAlignment(trans);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
			MAINDllOBJECT->SetStatusCode("Finished");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0013", __FILE__, __FUNCSIG__); }
}

//Align the current  dxf..//
void DXFExpose::AlginTheDXF(bool AlignmentCompleted)
{
	try
	{
		AlignFlag = false;
		if(AlignmentCompleted)
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
			MAINDllOBJECT->SetStatusCode("Finished");
			MAINDllOBJECT->getWindow(2).ZoomToExtents(2);
			return;
		}
		if(AlignDxfShapes())
		{
			AlignDxfShapes(false);
			CADalignment(&transform[0]);							
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
			MAINDllOBJECT->SetStatusCode("Finished");			
		}	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0014", __FILE__, __FUNCSIG__); }
}

//Reset Dxf alignment..
void DXFExpose::ResetDxfAlignment()
{
	try
	{		
		if(PPCALCOBJECT->PPAlignEditFlag())
			PPCALCOBJECT->ResetPP_EditAlign();
		else
		{
			RCollectionBase& Currentdxf =  MAINDllOBJECT->getDXFList();
			for(RC_ITER CItem = Currentdxf.getList().begin(); CItem != Currentdxf.getList().end(); CItem++)
			{
				DXFCollection* Cdxf = (DXFCollection*)(*CItem).second;
				for(RC_ITER Cdxfitem = Cdxf->getShapeCollection().getList().begin(); Cdxfitem != Cdxf->getShapeCollection().getList().end(); Cdxfitem++)
				{
					Shape* Current_Shape = (Shape*)(*Cdxfitem).second;
					Shape* Saved_Cshape = (Shape*)Cdxf->getOriginalShapeCollection().getList()[Current_Shape->getId()];
					if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS || Current_Shape->ShapeType == RapidEnums::SHAPETYPE::POLYLINE)
					{			
						((ShapeWithList*)Current_Shape)->PointsList->deleteAll();
						((ShapeWithList*)Current_Shape)->PointsListOriginal->deleteAll();
						for(PC_ITER pItem = ((ShapeWithList*)Saved_Cshape)->PointsList->getList().begin(); pItem != ((ShapeWithList*)Saved_Cshape)->PointsList->getList().end(); pItem++)
						{
							SinglePoint* Spt = (*pItem).second;
							((ShapeWithList*)Current_Shape)->PointsList->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
							((ShapeWithList*)Current_Shape)->PointsListOriginal->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));							
						}
						if(((ShapeWithList*)Current_Shape)->pts != NULL) {free(((ShapeWithList*)Current_Shape)->pts); ((ShapeWithList*)Current_Shape)->pts = NULL;}
						((ShapeWithList*)Current_Shape)->pts = (double*)malloc(sizeof(double) * ((ShapeWithList*)Current_Shape)->PointsListOriginal->Pointscount() * 3);
						int n = 0;
						for(PC_ITER pItem = ((ShapeWithList*)Current_Shape)->PointsList->getList().begin(); pItem != ((ShapeWithList*)Current_Shape)->PointsList->getList().end(); pItem++)
						{
							SinglePoint* Spt = (*pItem).second;
							((ShapeWithList*)Current_Shape)->pts[n++] = Spt->X;
							((ShapeWithList*)Current_Shape)->pts[n++] = Spt->Y;
							((ShapeWithList*)Current_Shape)->pts[n++] = Spt->Z;
						}
					}
					else if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::SPLINE)
					{
						Current_Shape->CopyOriginalProperties(Saved_Cshape);
						((ShapeWithList*)Current_Shape)->PointsList->deleteAll();
						((ShapeWithList*)Current_Shape)->PointsListOriginal->deleteAll();
						if(((Spline*)Saved_Cshape)->framgrabShape)
						{			
							for(PC_ITER pItem = ((ShapeWithList*)Saved_Cshape)->PointsList->getList().begin(); pItem != ((ShapeWithList*)Saved_Cshape)->PointsList->getList().end(); pItem++)
							{
								SinglePoint* Spt = (*pItem).second;
								((ShapeWithList*)Current_Shape)->PointsList->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
								((ShapeWithList*)Current_Shape)->PointsListOriginal->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));							
							}
						}
						else
						{
							((Spline*)Current_Shape)->ControlPointsList->deleteAll();
							for(PC_ITER SptItem = ((Spline*)Saved_Cshape)->ControlPointsList->getList().begin(); SptItem != ((Spline*)Saved_Cshape)->ControlPointsList->getList().end(); SptItem++)
							{	
								((Spline*)Current_Shape)->ControlPointsList->Addpoint(new SinglePoint((*SptItem).second->X, (*SptItem).second->Y, (*SptItem).second->Z));
							}
						}
					}
					else
						Current_Shape->CopyOriginalProperties(Saved_Cshape);
				}
				for(RC_ITER Cdxfitem = Cdxf->getMeasureCollection().getList().begin(); Cdxfitem != Cdxf->getMeasureCollection().getList().end(); Cdxfitem++)
				{
					DimBase* Current_Dim = (DimBase*)(*Cdxfitem).second;
					DimBase* Saved_Dim = (DimBase*)Cdxf->getOriginalMeasureCollection().getList()[Current_Dim->getId()];
					Current_Dim->CopyOriginalProperties(Saved_Dim);
				}
			}
			while(CurrentDXFNonEditedActionList.size() != 0)
			{
				RAction *act = *(CurrentDXFNonEditedActionList.begin());
				MAINDllOBJECT->getActionsHistoryList().removeItem(act);
				delete act;
			}
			CurrentDXFNonEditedActionList.clear();

			UpdateForUCS();
			MAINDllOBJECT->DXFShape_Updated();

		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0015", __FILE__, __FUNCSIG__); }

}

void DXFExpose::EditedCadAlignment(double *transform)
{
	try
	{
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->IsDxfShape()) continue;
			if(CShape->IsAlignedShape()) continue;
			CShape->Translate(spherecentrept);
			CShape->AlignToBasePlane(transform);
			CShape->Translate(userdefinedpt);
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0015a", __FILE__, __FUNCSIG__); }
}

//Align the selected cad...
void DXFExpose::CADalignment(double *transform, bool flag)
{
	try
	{
		if(this->DoThreeDAlignment())
		{
			for each(Shape* item in CurrentDXFShapeList)
			{
				item->Translate(spherecentrept);
				item->AlignToBasePlane(transform3D);
				item->Translate(userdefinedpt);
			}
			for each(DimBase* item in CurrentDXFMeasureList)
			{
				item->BasePlaneAlignment(transform3D);
				item->UpdateMeasurement();
			}
		}
		else
		{
			for each(Shape* item in CurrentDXFShapeList)
				item->Transform(transform);
			for each(DimBase* item in CurrentDXFMeasureList)
			{
				item->TransformPosition(transform);
				item->UpdateMeasurement();
			}
		}
		if(flag)
		{
			//((CadAlignHandler*)MAINDllOBJECT->getCurrentHandler())->EscapebuttonPress();
			MAINDllOBJECT->DXFShape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0016", __FILE__, __FUNCSIG__); }
}

void DXFExpose::CADTranslate(Vector& TranslateVector)
{
	try
	{
		for each(Shape* item in CurrentDXFShapeList)
			item->Translate(TranslateVector);
		for each(DimBase* item in CurrentDXFMeasureList)
		{
			item->TranslatePosition(TranslateVector);
			item->UpdateMeasurement();
		}
		MAINDllOBJECT->DXFShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0016a", __FILE__, __FUNCSIG__); }
}

void DXFExpose::EditedCadTranslate(Vector& TranslateVector)
{
	try
	{
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->IsDxfShape()) continue;
			if(CShape->IsAlignedShape()) continue;
			CShape->Translate(TranslateVector);
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0016a", __FILE__, __FUNCSIG__); }
}

void DXFExpose::EditedCadTransform(double *transform)
{
	try
	{
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->IsDxfShape()) continue;
			if(CShape->IsAlignedShape()) continue;
			CShape->Transform(transform);
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0016b", __FILE__, __FUNCSIG__); }
}

 void DXFExpose::SetAlignedProperty(bool Aligned)
{
	try
	{
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Shpitem = Cucs->getShapes().getList().begin(); Shpitem != Cucs->getShapes().getList().end(); Shpitem++)
			{
				Shape* CShape = (Shape*)((*Shpitem).second);
				if(CShape->IsDxfShape()) 
					CShape->IsAlignedShape(Aligned);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0016a", __FILE__, __FUNCSIG__); }
}

//Set the CAD colors....//
void DXFExpose::SetCADColors(int cnt, int r, int g, int b)
{
	try
	{
		SinglePoint* Spt = new SinglePoint(r, g, b);
		Spt->PtID = cnt;
		CADcolors[cnt]= Spt; 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0017", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ShowDeviationwithCloudPts()
{
	try
	{	
		//this code is for HAL.....................
		std::list<double> ptslist;
		Shape* CurSelShape = NULL;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(CShape == NULL)  return;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				CurSelShape = CShape;
				int nCount = CShape->PointsList->Pointscount();
				if(nCount > 3)
				{
					for(int i = 0; i < 6; i++)
					{
						SinglePoint* Spt;
						if(i == 0)
							Spt = CShape->PointsList->getList()[3];
						else if(i == 5)
							Spt = CShape->PointsList->getList()[nCount - 3];
						else
							Spt = CShape->PointsList->getList()[(int)((nCount * i) / 5)];
						double intersectPts[3] = {0}, Pts[2] = {Spt->X, Spt->Y};
						Shape* CurrentSelShape = NULL;
						bool ReturnFlag = false;
						CurrentSelShape = getNearestShape_2D(Pts, intersectPts, &ReturnFlag);
						if(ReturnFlag)
						{
							ptslist.push_back(Pts[0]);
							ptslist.push_back(Pts[1]);
							ptslist.push_back(intersectPts[0]);
							ptslist.push_back(intersectPts[1]);
						}		
					}
				}
				else
					return;
			 }
			 else
				 continue;
		}
			createtemporaryLine(&ptslist, CurSelShape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0018", __FILE__, __FUNCSIG__); }
}

void DXFExpose::ShowDeviationwithCloudPts(double tolerance, double interval, int IntervalType, bool ClosedLoop)
{
	try
	{	
		int NoOfLines = 0;
		RCollectionBase& ShpCollection = MAINDllOBJECT->getShapesList();
		double PreviousPtX[2] = {0};
		bool firstintPt = true;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
				{
					SinglePoint* Spt = (*SptItem).second;
					double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
					if(!firstintPt)
					{
						if(IntervalType == 0)
						{
							if(!(abs(PreviousPtX[0] - Pts[0]) >= interval))
								continue;
						}
						else if(IntervalType == 1)
						{
							if(!(abs(PreviousPtX[1] - Pts[1]) >= interval))
								continue;
						}
						else if(IntervalType == 2)
						{
							if(!(RMATH2DOBJECT->Pt2Pt_distance(PreviousPtX, Pts) >= interval))
								continue;
						}
					}
					Shape* CurrentSelShape = NULL;
					bool ReturnFlag = false;
					CurrentSelShape = getNearestShape_2D(Pts, intersectPts, &ReturnFlag);
					if(ReturnFlag)
					{
						firstintPt = false;
						if(IntervalType == 0)							
							PreviousPtX[0] = Pts[0];
						else if(IntervalType == 1)	
							PreviousPtX[1] = Pts[1];
						else if(IntervalType == 2)
						{
							PreviousPtX[0] = Pts[0]; PreviousPtX[1] = Pts[1];
						}
						if(NoOfLines >= 1000) return;
						double dist = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
						if(tolerance < dist && dist > RMATH2DOBJECT->MeasureDecValue)
						{
							bool negativeMeasurement = false;
							if(ClosedLoop)
							{
								if(getNumberOfIntersectionPoints(ShpCollection, Pts) % 2 != 0)
									negativeMeasurement = true;
							}
							NoOfLines++;
							create_2DLine(Pts, intersectPts, false);
							RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
							item--;
							ShapeWithList* Shp = (ShapeWithList*)((*item).second);
							CurrentSelShape->addChild(Shp);
							Shp->addParent(CurrentSelShape);
							CShape->addChild(Shp);
							Shp->addParent(CShape);
							//create measurement............
							DimBase* dim = NULL;
							dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
							dim->addParent(Shp);
							Shp->addMChild(dim);
							dim->IsValid(true);
							dim->IsNegativeMeasurement(negativeMeasurement);
							dim->UpdateMeasurement();
							AddDimAction::addDim(dim);
						}
					}					
				}
			 }
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0018", __FILE__, __FUNCSIG__); }
}

//void DXFExpose::ShowDeviationwithCloudPts(double Uppertolerance, double Lowertolerance, double interval, bool DeviationWithYellowShapes)
//{
//	try
//	{
//		if(DeviationWithYellowShapes)
//		{
//			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
//			{
//				ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
//				if(CShape == NULL)  return;
//				if(!CShape->selected()) continue;				
//				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
//				{
//					int NoOfLines = 0;
//					double PreviousPtX = 0;
//					bool firstintPt = true;
//					std::list<int> ShapeIdList;
//					map<int, double> DistIdMap;
//					map<int, SinglePoint*> intersectPtMapList;
//					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
//					{
//						SinglePoint* Spt = (*SptItem).second;
//						double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
//						if(!firstintPt)
//						{
//							if(!(abs(PreviousPtX - Pts[0]) >= interval))
//								continue;
//						}					
//						if(getNearestShape(Pts, intersectPts))
//						{
//							//check deviation for yellow lines only.......
//							if(Current_Selected_Shape[0]->ShapeColor.r != 1 || Current_Selected_Shape[0]->ShapeColor.g != 1 || Current_Selected_Shape[0]->ShapeColor.b != 0) continue;
//							firstintPt = false;
//							PreviousPtX = Pts[0];
//							bool IdPresent = false;
//							for each(int id in ShapeIdList)
//							{
//								if(Current_Selected_Shape[0]->getId() == id)
//								{
//									IdPresent = true;
//									break;
//								}
//							}
//							double currentDist = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
//							if(currentDist < 0.0001) continue;
//							if(IdPresent)
//							{							
//								if(currentDist > DistIdMap[Current_Selected_Shape[0]->getId()])
//								{
//									DistIdMap[Current_Selected_Shape[0]->getId()] = currentDist;
//									intersectPtMapList[Current_Selected_Shape[0]->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], Pts[0],  Pts[1]);
//								}
//							}
//							else
//							{
//								ShapeIdList.push_back(Current_Selected_Shape[0]->getId());
//								DistIdMap[Current_Selected_Shape[0]->getId()] = currentDist;
//								intersectPtMapList[Current_Selected_Shape[0]->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], Pts[0], Pts[1]);
//							}
//						}				
//					}
//
//					for each(int id in ShapeIdList)
//					{
//						if(NoOfLines >= 1000) return;
//						NoOfLines++;
//						SinglePoint* Spt = intersectPtMapList[id];
//						double intersectPts[2] = {Spt->X, Spt->Y}, Pts[2] = {Spt->Z, Spt->R};
//						double tolerancelimit = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
//						bool positiveFlag = false;
//						bool withintolerance = false;
//						if(Pts[1] > intersectPts[1])
//						{
//							positiveFlag = true;
//							if(Uppertolerance >= tolerancelimit)
//								withintolerance = true;
//						}
//						else
//						{
//							if(abs(Lowertolerance) >= tolerancelimit)
//								withintolerance = true;
//						}
//						if(withintolerance)
//							create_2DLine(Pts, intersectPts, false, true, true);
//						else
//							create_2DLine(Pts, intersectPts, false, false, true);
//						RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
//						item--;
//						ShapeWithList* Shp = (ShapeWithList*)((*item).second);
//						Shape* ParentShpe = (Shape*)(MAINDllOBJECT->getShapesList().getList()[id]);
//						ParentShpe->addChild(Shp);
//						Shp->addParent(ParentShpe);
//						CShape->addChild(Shp);
//						Shp->addParent(CShape);
//						//create measurement............
//						DimBase* dim = NULL;
//						dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
//						dim->addParent(Shp);
//						Shp->addMChild(dim);
//						dim->IsValid(true);
//						dim->DoNotSaveInPP = true;
//						if(!positiveFlag)
//							dim->IsNegativeMeasurement(true);
//						dim->UpdateMeasurement();
//						AddDimAction::addDim(dim);
//					}																								
//				}
//			}
//		}
//		else
//		{	
//			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
//			{
//				ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
//				if(CShape == NULL)  return;
//				if(!CShape->selected()) continue;
//				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
//				{
//					int NoOfLines = 0;
//					double PreviousPtX = 0;
//					bool firstintPt = true;
//					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
//					{
//						SinglePoint* Spt = (*SptItem).second;
//						double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
//						if(!firstintPt)
//						{
//							if(!(abs(PreviousPtX - Pts[0]) >= interval))
//								continue;
//						}					
//						if(getNearestShape(Pts, intersectPts))
//						{
//							PreviousPtX = Pts[0];
//							firstintPt = false;
//							if(NoOfLines >= 1000) return;					
//							double tolerancelimit = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
//							if(tolerancelimit < 0.0001) continue;
//							NoOfLines++;
//							bool positiveFlag = false;
//							bool withintolerance = false;
//							if(Pts[1] > intersectPts[1])
//							{
//								positiveFlag = true;
//								if(Uppertolerance >= tolerancelimit)
//									withintolerance = true;
//							}
//							else
//							{
//								if(abs(Lowertolerance) >= tolerancelimit)
//									withintolerance = true;
//							}
//							intersectPts[2] = Pts[2];
//							if(withintolerance)
//								create_2DLine(Pts, intersectPts, false, true, true);
//							else
//								create_2DLine(Pts, intersectPts, false, false, true);
//							RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
//							item--;
//							ShapeWithList* Shp = (ShapeWithList*)((*item).second);
//							/*Current_Selected_Shape[0]->addChild(Shp);
//							Shp->addParent(Current_Selected_Shape[0]);
//							CShape->addChild(Shp);
//							Shp->addParent(CShape);*/
//							//create measurement............
//							DimBase* dim = NULL;
//							dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
//							dim->DoNotSaveInPP = true;
//							dim->addParent(Shp);
//							Shp->addMChild(dim);
//							dim->IsValid(true);
//							if(!positiveFlag)
//								dim->IsNegativeMeasurement(true);
//							dim->UpdateMeasurement();
//							AddDimAction::addDim(dim, true);
//						}					
//					}
//				}
//			}
//			MAINDllOBJECT->UpdateShapesChanged();
//		}
//	}
//	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0018a", __FILE__, __FUNCSIG__); }
//}


bool DXFExpose::ShowDeviationwithCloudPts(double Uppertolerance, double Lowertolerance, double interval, std::list<int>* LineIdList, int IntervalType, bool TwoDdeviation, bool DeviationWithYellowShapes, int measureRefrenceAxis, bool BestfitArc)
{
	try
	{
		if(TwoDdeviation)
		{
			if(DeviationWithYellowShapes)
			{
				bool firstTime = true;
				int NoOfLines = 0;
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(!CShape->selected()) continue;
					//if(CShape->IsDxfShape()) continue;
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
					{
						double totDist = 0;
						int ptsCount = 0;
						for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							double dist = RMATH2DOBJECT->Pt2Line_Dist(Spt->X, Spt->Y, ((Line*)CShape)->Angle(), ((Line*)CShape)->Intercept());
							totDist += dist;
							ptsCount++;
						}
						double Straightness = totDist / ptsCount;
						if(Uppertolerance < Straightness)
						{
							MAINDllOBJECT->ShowMsgBoxString("Deviation001", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							return false;
						}
						if(Straightness * 2 < Uppertolerance)
							Straightness *= 2;
						double PreviousPtX[2] = {0};
						bool firstintPt = true;
						std::list<int> ShapeIdList;
						map<int, double> DistIdMap;
						map<int, SinglePoint*> intersectPtMapList;
						for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							if(Straightness < RMATH2DOBJECT->Pt2Line_Dist(Spt->X, Spt->Y, ((Line*)CShape)->Angle(), ((Line*)CShape)->Intercept())) continue;
							double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
							if(!firstintPt)
							{
								if(IntervalType == 0)
								{
									if(!(abs(PreviousPtX[0] - Pts[0]) >= interval))
										continue;
								}
								else if(IntervalType == 1)
								{
									if(!(abs(PreviousPtX[1] - Pts[1]) >= interval))
										continue;
								}
								else if(IntervalType == 2)
								{
									if(!(RMATH2DOBJECT->Pt2Pt_distance(PreviousPtX, Pts) >= interval))
										continue;
								}
							}					
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = getNearestShape_2D(Pts, intersectPts, &ReturnFlag, false);
							intersectPts[2] = Pts[2];
							if(ReturnFlag)
							{
								//check deviation for yellow lines only.......
								if(CurrentSelShape->ShapeColor.r != 1 || CurrentSelShape->ShapeColor.g != 1 || CurrentSelShape->ShapeColor.b != 0) continue;
								firstintPt = false;
								if(IntervalType == 0)							
									PreviousPtX[0] = Pts[0];
								else if(IntervalType == 1)	
									PreviousPtX[1] = Pts[1];
								else if(IntervalType == 2)
								{
									PreviousPtX[0] = Pts[0]; PreviousPtX[1] = Pts[1];
								}
								bool IdPresent = false;
								for each(int id in ShapeIdList)
								{
									if(CurrentSelShape->getId() == id)
									{
										IdPresent = true;
										break;
									}
								}
								double currentDist = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
								if(currentDist < 0.0001) continue;
								if(IdPresent)
								{							
									if(currentDist > DistIdMap[CurrentSelShape->getId()])
									{
										DistIdMap[CurrentSelShape->getId()] = currentDist;									
										intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], Pts[0], Pts[1], Pts[2]);
									}
								}
								else
								{
									ShapeIdList.push_back(CurrentSelShape->getId());
									DistIdMap[CurrentSelShape->getId()] = currentDist;			
									intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], Pts[0], Pts[1], Pts[2]);
								}
							}				
						}
						for each(int id in ShapeIdList)
						{
							if(NoOfLines >= 800) return false;
							NoOfLines++;
							SinglePoint* Spt = intersectPtMapList[id];
							double intersectPts[3] = {Spt->X, Spt->Y, Spt->Pdir}, Pts[3] = {Spt->Z, Spt->R, Spt->Pdir};
							if(firstTime)
							{
								firstTime = false;
								Vector Shift_XY;
								Shift_XY.set(Spt->Z - Spt->X, Spt->R - Spt->Y);
								DXFEXPOSEOBJECT->SetTransVector(Shift_XY, false);
								intersectPts[0] = Pts[0]; intersectPts[1] = Pts[1];
								for(RC_ITER pp = MAINDllOBJECT->getActionsHistoryList().getList().begin(); pp != MAINDllOBJECT->getActionsHistoryList().getList().end(); pp++)
								{
									RAction* action = (RAction*)(*pp).second;
									if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
									{
										Vector AlignPos;
										if(((EditCad2PtAction*)action)->getActionCount() == 1)
										{					
											AlignPos = *((EditCad2PtAction*)action)->getPoint1();
											AlignPos += Shift_XY;
											((EditCad2PtAction*)action)->SetPoint1(AlignPos);
										}
										else if(((EditCad2PtAction*)action)->getActionCount() == 2)
										{
											AlignPos = *((EditCad2PtAction*)action)->getPoint2();
											AlignPos += Shift_XY;				
											((EditCad2PtAction*)action)->SetPoint2(AlignPos);
										}	
									}
								}
							}						
							double tolerancelimit = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
							bool positiveFlag = false;
							bool withintolerance = false;
							if(measureRefrenceAxis == 0)
							{
								if(Pts[0] <= intersectPts[0])
								{
									positiveFlag = true;
									if(Uppertolerance >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(Lowertolerance) >= tolerancelimit)
										withintolerance = true;
								}
							}
							else if(measureRefrenceAxis == 1)
							{
								if(Pts[1] > intersectPts[1])
								{
									positiveFlag = true;
									if(Uppertolerance >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(Lowertolerance) >= tolerancelimit)
										withintolerance = true;
								}
							}						
							if(withintolerance)
								create_2DLine(Pts, intersectPts, false, true, false);
							else
								create_2DLine(Pts, intersectPts, false, false, false);
							RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
							item--;
							ShapeWithList* Shp = (ShapeWithList*)((*item).second);
							Shape* ParentShpe = (Shape*)(MAINDllOBJECT->getShapesList().getList()[id]);
							ParentShpe->addChild(Shp);
							Shp->addParent(ParentShpe);
							CShape->addChild(Shp);
							Shp->addParent(CShape);
							LineIdList->push_back(Shp->getId());
							//create measurement............
							DimBase* dim = NULL;
							dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
							dim->addParent(Shp);
							Shp->addMChild(dim);
							dim->IsValid(true);
							if(!positiveFlag)
								dim->IsNegativeMeasurement(true);
							dim->UpdateMeasurement();
							AddDimAction::addDim(dim);
						}																								
					}
				}
			}
			else
			{	
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(!CShape->selected()) continue;
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						int NoOfLines = 0;
						double PreviousPtX[2] = {0};
						bool firstintPt = true;
						for(list<BaseItem*>::iterator SptItem = CShape->PointAtionList.begin(); SptItem != CShape->PointAtionList.end(); SptItem++)
						{
							list<int> tmpList = ((AddPointAction*)(*SptItem))->getFramegrab()->PointActionIdList;
							if(tmpList.size() < 3) continue;
							double *points = NULL;
							points = new double[tmpList.size() * 2];
							int n = 0;
							double avgZ = 0, ans[3] = {0}, angles[2] = {0}, currentY = ((AddPointAction*)(*SptItem))->getFramegrab()->PointDRO.getY() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), currentZ = 0;
							for(list<int>::iterator It = tmpList.begin(); It != tmpList.end(); It++)
							{
								SinglePoint* Pt = CShape->PointsList->getList()[*It];
								points[n++] = Pt->X;
								points[n++] = Pt->Y;
								avgZ += Pt->Y;
							}
							avgZ /= tmpList.size();
							if(BestfitArc)
							{
								BESTFITOBJECT->Circle_BestFit(points, tmpList.size(), &ans[0], MAINDllOBJECT->SigmaModeFlag(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());					
								if(abs(ans[2]) < abs(currentY - ans[0]))
									currentZ = avgZ;
								else
								{
									double tmpz1 = sqrt(pow(ans[2], 2) - pow(currentY - ans[0], 2)) + ans[1];
									double tmpz2 = -sqrt(pow(ans[2], 2) - pow(currentY - ans[0], 2)) + ans[1];
									if(abs(tmpz1 - avgZ) < abs(tmpz2 - avgZ))
										currentZ = tmpz1;
									else
										currentZ = tmpz2;
									if(abs(currentZ - avgZ) > 1) currentZ = avgZ;
								}
							}
							else
							{
								BESTFITOBJECT->Line_BestFit_2D(points, tmpList.size(), &ans[0], MAINDllOBJECT->SigmaModeFlag(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());	
								currentZ = tan(ans[0]) * currentY + ans[1];
								if(abs(currentZ - avgZ) > 1) currentZ = avgZ;
							}
							double intersectPts[3] = {0}, Pts[3] = {currentY, currentZ, ((AddPointAction*)(*SptItem))->getFramegrab()->PointDRO.getX() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX()};
							if(points != NULL)
							{
								delete [] points;
								points = NULL;
							}
							if(!firstintPt)
							{
								if(IntervalType == 0)
								{
									if(!(abs(PreviousPtX[0] - Pts[0]) >= interval))
										continue;
								}
								else if(IntervalType == 1)
								{
									if(!(abs(PreviousPtX[1] - Pts[1]) >= interval))
										continue;
								}
								else if(IntervalType == 2)
								{
									if(!(RMATH2DOBJECT->Pt2Pt_distance(PreviousPtX, Pts) >= interval))
										continue;
								}
							}					
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = getNearestShape_2D(Pts, intersectPts, &ReturnFlag);
							intersectPts[2] = Pts[2];
							if(ReturnFlag)
							{
								if(IntervalType == 0)							
									PreviousPtX[0] = Pts[0];
								else if(IntervalType == 1)	
									PreviousPtX[1] = Pts[1];
								else if(IntervalType == 2)
								{
									PreviousPtX[0] = Pts[0]; PreviousPtX[1] = Pts[1];
								}
								firstintPt = false;
								if(NoOfLines >= 800) return false;					
								double tolerancelimit = RMATH2DOBJECT->Pt2Pt_distance(Pts, intersectPts);
								if(tolerancelimit < 0.0001) continue;
								NoOfLines++;
								bool positiveFlag = false;
								bool withintolerance = false;
								if(measureRefrenceAxis == 0)
								{
									if(Pts[0] > intersectPts[0])
									{
										positiveFlag = true;
										if(Uppertolerance >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(Lowertolerance) >= tolerancelimit)
											withintolerance = true;
									}
								}
								else if(measureRefrenceAxis == 1)
								{
									if(Pts[1] > intersectPts[1])
									{
										positiveFlag = true;
										if(Uppertolerance >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(Lowertolerance) >= tolerancelimit)
											withintolerance = true;
									}
								}
								if(withintolerance)
									create_2DLine(Pts, intersectPts, false, true, false);
								else
									create_2DLine(Pts, intersectPts, false, false, false);
								RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
								item--;
								ShapeWithList* Shp = (ShapeWithList*)((*item).second);
								CurrentSelShape->addChild(Shp);
								Shp->addParent(CurrentSelShape);
								CShape->addChild(Shp);
								Shp->addParent(CShape);
								LineIdList->push_back(Shp->getId());
								//create measurement............
								DimBase* dim = NULL;
								dim = HELPEROBJECT->CreateShapeToShapeDim2D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
								dim->addParent(Shp);
								Shp->addMChild(dim);
								dim->IsValid(true);
								if(!positiveFlag)
									dim->IsNegativeMeasurement(true);
								dim->UpdateMeasurement();
								AddDimAction::addDim(dim);
							}					
						}
					}
				}
			}
		}
		else
		{
			if(DeviationWithYellowShapes)
			{
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(!CShape->selected()) continue;				
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						int NoOfLines = 0;
						double PreviousPtX[2] = {0};
						bool firstintPt = true;
						std::list<int> ShapeIdList;
						map<int, double> DistIdMap;
						map<int, SinglePoint*> intersectPtMapList;
						map<int, SinglePoint*> CloudPtsList;
						for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
							if(!firstintPt)
							{
								if(IntervalType == 0)
								{
									if(!(abs(PreviousPtX[0] - Pts[0]) >= interval))
										continue;
								}
								else if(IntervalType == 1)
								{
									if(!(abs(PreviousPtX[1] - Pts[1]) >= interval))
										continue;
								}
								else if(IntervalType == 2)
								{
									if(!(RMATH2DOBJECT->Pt2Pt_distance(PreviousPtX, Pts) >= interval))
										continue;
								}
							}
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = getNearestShape_3D(Pts, intersectPts, &ReturnFlag);
							if(ReturnFlag)
							{
								//check deviation for yellow lines only.......
								if(CurrentSelShape->ShapeColor.r != 1 || CurrentSelShape->ShapeColor.g != 1 || CurrentSelShape->ShapeColor.b != 0) continue;
								firstintPt = false;
								if(IntervalType == 0)							
									PreviousPtX[0] = Pts[0];
								else if(IntervalType == 1)	
									PreviousPtX[1] = Pts[1];
								else if(IntervalType == 2)
								{
									PreviousPtX[0] = Pts[0]; PreviousPtX[1] = Pts[1];
								}
								bool IdPresent = false;
								for each(int id in ShapeIdList)
								{
									if(CurrentSelShape->getId() == id)
									{
										IdPresent = true;
										break;
									}
								}
								double currentDist = RMATH3DOBJECT->Distance_Point_Point(Pts, intersectPts);
								if(currentDist < 0.0001) continue;
								if(IdPresent)
								{							
									if(currentDist > DistIdMap[CurrentSelShape->getId()])
									{
										DistIdMap[CurrentSelShape->getId()] = currentDist;									
										intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], intersectPts[2]);
										CloudPtsList[CurrentSelShape->getId()] = new SinglePoint(Pts[0], Pts[1], Pts[2]);
									}
								}
								else
								{
									ShapeIdList.push_back(CurrentSelShape->getId());
									DistIdMap[CurrentSelShape->getId()] = currentDist;			
									intersectPtMapList[CurrentSelShape->getId()] = new SinglePoint(intersectPts[0], intersectPts[1], intersectPts[2]);
									CloudPtsList[CurrentSelShape->getId()] = new SinglePoint(Pts[0], Pts[1], Pts[2]);
								}
							}				
						}

						for each(int id in ShapeIdList)
						{
							if(NoOfLines >= 800) return false;
							NoOfLines++;
							SinglePoint* Spt = intersectPtMapList[id];
							SinglePoint* CSpt = CloudPtsList[id];
							double intersectPts[3] = {Spt->X, Spt->Y, Spt->Z}, Pts[3] = {CSpt->X, CSpt->Y, CSpt->Z};
							double tolerancelimit = RMATH3DOBJECT->Distance_Point_Point(Pts, intersectPts);
							bool positiveFlag = false;
							bool withintolerance = false;
							if(measureRefrenceAxis == 0)
							{
								if(Pts[0] > intersectPts[0])
								{
									positiveFlag = true;
									if(Uppertolerance >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(Lowertolerance) >= tolerancelimit)
										withintolerance = true;
								}
							}
							else if(measureRefrenceAxis == 1)
							{
								if(Pts[1] > intersectPts[1])
								{
									positiveFlag = true;
									if(Uppertolerance >= tolerancelimit)
										withintolerance = true;
								}
								else
								{
									if(abs(Lowertolerance) >= tolerancelimit)
										withintolerance = true;
								}
							}
							if(withintolerance)
								create_2DLine(Pts, intersectPts, false, true, false);
							else
								create_2DLine(Pts, intersectPts, false, false, false);
							RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
							item--;
							ShapeWithList* Shp = (ShapeWithList*)((*item).second);
							Shape* ParentShpe = (Shape*)(MAINDllOBJECT->getShapesList().getList()[id]);
							ParentShpe->addChild(Shp);
							Shp->addParent(ParentShpe);
							CShape->addChild(Shp);
							Shp->addParent(CShape);
							LineIdList->push_back(Shp->getId());
							//create measurement............
							DimBase* dim = NULL;
							dim = HELPEROBJECT->CreateShapeToShapeDim3D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
							double mSelectionLine[6] = {0};
							MAINDllOBJECT->getWindow(1).GetSelectionLine((((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2, mSelectionLine);
							dim->SetSelectionLine(mSelectionLine);
							dim->IsValid(true);
							if(!positiveFlag)
								dim->IsNegativeMeasurement(true);
							dim->UpdateMeasurement();
							AddDimAction::addDim(dim, true);
						}																								
					}
				}
			}
			else
			{	
				for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
				{
					ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
					if(!CShape->selected()) continue;
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						int NoOfLines = 0;
						double PreviousPtX[2] = {0};
						bool firstintPt = true;
						for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
						{
							SinglePoint* Spt = (*SptItem).second;
							double intersectPts[3] = {0}, Pts[3] = {Spt->X, Spt->Y, Spt->Z};
							if(!firstintPt)
							{
								if(IntervalType == 0)
								{
									if(!(abs(PreviousPtX[0] - Pts[0]) >= interval))
										continue;
								}
								else if(IntervalType == 1)
								{
									if(!(abs(PreviousPtX[1] - Pts[1]) >= interval))
										continue;
								}
								else if(IntervalType == 2)
								{
									if(!(RMATH2DOBJECT->Pt2Pt_distance(PreviousPtX, Pts) >= interval))
										continue;
								}
							}					
							Shape* CurrentSelShape = NULL;
							bool ReturnFlag = false;
							CurrentSelShape = getNearestShape_3D(Pts, intersectPts, &ReturnFlag);
							if(ReturnFlag)
							{
								if(IntervalType == 0)							
									PreviousPtX[0] = Pts[0];
								else if(IntervalType == 1)	
									PreviousPtX[1] = Pts[1];
								else if(IntervalType == 2)
								{
									PreviousPtX[0] = Pts[0]; PreviousPtX[1] = Pts[1];
								}
								firstintPt = false;
								if(NoOfLines >= 800) return false;					
								double tolerancelimit = RMATH3DOBJECT->Distance_Point_Point(Pts, intersectPts);
								if(tolerancelimit < 0.0001) continue;
								NoOfLines++;
								bool positiveFlag = false;
								bool withintolerance = false;
								if(measureRefrenceAxis == 0)
								{
									if(Pts[0] > intersectPts[0])
									{
										positiveFlag = true;
										if(Uppertolerance >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(Lowertolerance) >= tolerancelimit)
											withintolerance = true;
									}
								}
								else if(measureRefrenceAxis == 1)
								{
									if(Pts[1] > intersectPts[1])
									{
										positiveFlag = true;
										if(Uppertolerance >= tolerancelimit)
											withintolerance = true;
									}
									else
									{
										if(abs(Lowertolerance) >= tolerancelimit)
											withintolerance = true;
									}
								}
								if(withintolerance)
									HELPEROBJECT->create_3DLine(Pts, intersectPts, false, true);
								else
									HELPEROBJECT->create_3DLine(Pts, intersectPts, false, false);
								RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
								item--;
								ShapeWithList* Shp = (ShapeWithList*)((*item).second);
								CurrentSelShape->addChild(Shp);
								Shp->addParent(CurrentSelShape);
								CShape->addChild(Shp);
								Shp->addParent(CShape);
								LineIdList->push_back(Shp->getId());
								//create measurement............
								DimBase* dim = NULL;
								dim = HELPEROBJECT->CreateShapeToShapeDim3D(Shp, Shp, (((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2 + 0.2, (((Line*)Shp)->getPoint1()->getZ() + ((Line*)Shp)->getPoint2()->getZ()) / 2);
								double mSelectionLine[6] = {0};
								MAINDllOBJECT->getWindow(1).GetSelectionLine((((Line*)Shp)->getPoint1()->getX() + ((Line*)Shp)->getPoint2()->getX()) / 2, (((Line*)Shp)->getPoint1()->getY() + ((Line*)Shp)->getPoint2()->getY()) / 2, mSelectionLine);
								dim->SetSelectionLine(mSelectionLine);
								dim->IsValid(true);
								if(!positiveFlag)
									dim->IsNegativeMeasurement(true);
								dim->UpdateMeasurement();
								AddDimAction::addDim(dim, true);
							}					
						}
					}
				}
			}
		}
		MAINDllOBJECT->UpdateShapesChanged();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0018a", __FILE__, __FUNCSIG__); return false; }
}

Shape* DXFExpose::getNearestShape_3D(double* pts, double *IntersectionPt, bool* returnFlag) 
{
	try
	{
		double Distance = 0;
		bool firstTime = true;
		Shape* CurrentSelected_Shape = NULL;
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape()) continue;
			if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::ARC || Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				double center[2] = {((Circle*)Current_Shape)->getCenter()->getX(), ((Circle*)Current_Shape)->getCenter()->getY()};
				double slope, intercept, intersectpoints[3] = {0}, endpt1[2] = {((Circle*)Current_Shape)->getendpoint1()->getX(), ((Circle*)Current_Shape)->getendpoint1()->getY()}, endpt2[2] = {((Circle*)Current_Shape)->getendpoint2()->getX(), ((Circle*)Current_Shape)->getendpoint2()->getY()};	
				RMATH2DOBJECT->TwoPointLine(center, pts, &slope, &intercept);
				if(calculateIntersectionPtOnCircle(pts, Current_Shape, slope, intercept, &intersectpoints[0]))
				{
					double dist = RMATH3DOBJECT->Distance_Point_Point(pts, intersectpoints);
					if(firstTime)
					{		
						firstTime = false;						
						Distance = dist;
						CurrentSelected_Shape = Current_Shape; 
						IntersectionPt[0] = intersectpoints[0];
						IntersectionPt[1] = intersectpoints[1];
						IntersectionPt[2] = intersectpoints[2];
						*returnFlag = true;		
						if(DeviationWithLineOnly)
							*returnFlag = false;
					}
					else
					{
						if(Distance > dist)
						{
							Distance = dist;
							CurrentSelected_Shape = Current_Shape;
							IntersectionPt[0] = intersectpoints[0];
							IntersectionPt[1] = intersectpoints[1];
							IntersectionPt[2] = intersectpoints[2];
							*returnFlag = true;
							if(DeviationWithLineOnly)
								*returnFlag = false;
						}
					}
				}
			}
			else if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double intersectpoints[3] = {0}, dist = 0, endpts[6] = {0};
				endpts[0] = ((Line*)Current_Shape)->getPoint1()->getX(); endpts[1] = ((Line*)Current_Shape)->getPoint1()->getY(); endpts[2] = ((Line*)Current_Shape)->getPoint1()->getZ(); endpts[3] = ((Line*)Current_Shape)->getPoint2()->getX(); endpts[4] = ((Line*)Current_Shape)->getPoint2()->getY(); endpts[5] = ((Line*)Current_Shape)->getPoint2()->getZ();
				double LineParam[6] = {endpts[0], endpts[1], endpts[2], 0, 0, 0};
				RMATH3DOBJECT->DCs_TwoEndPointsofLine(endpts, &LineParam[3]);
				RMATH3DOBJECT->Projection_Point_on_Line(pts, LineParam, intersectpoints);
				if(intersectionPointOnLine(endpts[0], endpts[1], endpts[3], endpts[4], intersectpoints[0], intersectpoints[1]))				
					dist = RMATH3DOBJECT->Distance_Point_Point(pts, intersectpoints);		
				else
				{					
					double dist1 = 0, dist2 = 0;
					dist1 = RMATH3DOBJECT->Distance_Point_Point(pts, &endpts[0]);
					dist2 = RMATH3DOBJECT->Distance_Point_Point(pts, &endpts[3]);				
					if(dist1 < dist2)
					{
						intersectpoints[0] = endpts[0]; intersectpoints[1] = endpts[1]; intersectpoints[2] = endpts[2]; dist = dist1;
					}
					else
					{
						intersectpoints[0] = endpts[3]; intersectpoints[1] = endpts[4]; intersectpoints[2] = endpts[5]; dist = dist2;
					}
				}
				if(firstTime)
				{
					Distance = dist;
					CurrentSelected_Shape = Current_Shape;
					IntersectionPt[0] = intersectpoints[0];
					IntersectionPt[1] = intersectpoints[1];
					IntersectionPt[2] = intersectpoints[2];
					firstTime = false;
					*returnFlag = true;
				}
				else
				{
					if(Distance > dist)
					{
						Distance = dist;
						CurrentSelected_Shape = Current_Shape;
						IntersectionPt[0] = intersectpoints[0];
						IntersectionPt[1] = intersectpoints[1];
						IntersectionPt[2] = intersectpoints[2];
						*returnFlag = true;
					}
				}			
			}
			else
				continue;
		}
		return CurrentSelected_Shape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0019", __FILE__, __FUNCSIG__); return false; }
}

Shape* DXFExpose::getNearestShape_2D(double* pts, double* IntersectionPt, bool* returnFlag, bool finiteLine)
{
	try
	{
		double Distance = 0;
		bool firstTime = true;
		Shape* CurrentSelected_Shape = NULL;
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape()) continue;
			//if(!Current_Shape->IsDxfShape()) continue;
			if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::ARC || Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				double center[2] = {((Circle*)Current_Shape)->getCenter()->getX(), ((Circle*)Current_Shape)->getCenter()->getY()};
				double slope, intercept, intersectpoints[2], endpt1[2] = {((Circle*)Current_Shape)->getendpoint1()->getX(), ((Circle*)Current_Shape)->getendpoint1()->getY()}, endpt2[2] = {((Circle*)Current_Shape)->getendpoint2()->getX(), ((Circle*)Current_Shape)->getendpoint2()->getY()};	
				RMATH2DOBJECT->TwoPointLine(center, pts, &slope, &intercept);
				if(calculateIntersectionPtOnCircle(pts, Current_Shape, slope, intercept, &intersectpoints[0]))
				{
					double dist = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], intersectpoints[0], intersectpoints[1]);
					if(firstTime)
					{		
						firstTime = false;						
						Distance = dist;
						CurrentSelected_Shape = Current_Shape; 
						IntersectionPt[0] = intersectpoints[0];
						IntersectionPt[1] = intersectpoints[1];
						IntersectionPt[2] = pts[2];
						*returnFlag = true;		
						if(DeviationWithLineOnly)
							*returnFlag = false;
					}
					else
					{
						if(Distance > dist)
						{
							Distance = dist;
							CurrentSelected_Shape = Current_Shape;
							IntersectionPt[0] = intersectpoints[0];
							IntersectionPt[1] = intersectpoints[1];
							IntersectionPt[2] = pts[2];
							*returnFlag = true;
							if(DeviationWithLineOnly)
								*returnFlag = false;
						}
					}
				}
			}
			else if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double intersectpoints[2] = {0}, dist = 0, endpts[4] = {0};
				endpts[0] = ((Line*)Current_Shape)->getPoint1()->getX(); endpts[1] = ((Line*)Current_Shape)->getPoint1()->getY(); endpts[2] = ((Line*)Current_Shape)->getPoint2()->getX(); endpts[3] = ((Line*)Current_Shape)->getPoint2()->getY();
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)Current_Shape)->Angle(), ((Line*)Current_Shape)->Intercept(), pts, intersectpoints);
				if(finiteLine)
				{
					if(intersectionPointOnLine(endpts[0], endpts[1], endpts[2], endpts[3], intersectpoints[0], intersectpoints[1]))				
						dist = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], intersectpoints[0], intersectpoints[1]);		
					else
					{					
						double dist1 = 0, dist2 = 0;
						dist1 = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], endpts[0], endpts[1]);
						dist2 = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], endpts[2], endpts[3]);				
						if(dist1 < dist2)
						{
							intersectpoints[0] = endpts[0]; intersectpoints[1] = endpts[1]; dist = dist1;
						}
						else
						{
							intersectpoints[0] = endpts[2]; intersectpoints[1] = endpts[3]; dist = dist2;
						}
					}
					if(firstTime)
					{
						Distance = dist;
						CurrentSelected_Shape = Current_Shape;
						IntersectionPt[0] = intersectpoints[0];
						IntersectionPt[1] = intersectpoints[1];
						IntersectionPt[2] = pts[2];
						firstTime = false;
						*returnFlag = true;
					}
					else
					{
						if(Distance > dist)
						{
							Distance = dist;
							CurrentSelected_Shape = Current_Shape;
							IntersectionPt[0] = intersectpoints[0];
							IntersectionPt[1] = intersectpoints[1];
							IntersectionPt[2] = pts[2];
							*returnFlag = true;
						}
					}
				}
				else
				{
					if(intersectionPointOnLine(endpts[0], endpts[1], endpts[2], endpts[3], intersectpoints[0], intersectpoints[1]))	
					{
						dist = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], intersectpoints[0], intersectpoints[1]);
					}
					else
					{
						double dist1 = 0, dist2 = 0;
						dist1 = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], endpts[0], endpts[1]);
						dist2 = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], endpts[2], endpts[3]);				
						if(dist1 < dist2)
							dist = dist1;					
						else
							dist = dist2;						
					}
					if(firstTime)
					{
						Distance = dist;
						CurrentSelected_Shape = Current_Shape;
						IntersectionPt[0] = intersectpoints[0];
						IntersectionPt[1] = intersectpoints[1];
						IntersectionPt[2] = pts[2];
						firstTime = false;
						*returnFlag = true;
					}
					else
					{
						if(Distance > dist)
						{
							Distance = dist;
							CurrentSelected_Shape = Current_Shape;
							IntersectionPt[0] = intersectpoints[0];
							IntersectionPt[1] = intersectpoints[1];
							IntersectionPt[2] = pts[2];
							*returnFlag = true;
						}
					}
				}
			}
			else if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::POLYLINE)
			{
				for(std::map<int, SinglePoint*>::iterator itr = ((ShapeWithList*)Current_Shape)->PointsList->getList().begin(); itr != ((ShapeWithList*)Current_Shape)->PointsList->getList().end(); itr++)
				{
					double Pnts[2] = {(*itr).second->X, (*itr).second->Y};
					double dist = RMATH2DOBJECT->Pt2Pt_distance(Pnts, pts);
					if(firstTime)
					{		
						CurrentSelected_Shape = Current_Shape;
						firstTime = false;						
						Distance = dist;
						IntersectionPt[0] = Pnts[0];
						IntersectionPt[1] = Pnts[1];
						*returnFlag = true;
					}
					else
					{
						if(Distance > dist)
						{
							CurrentSelected_Shape = Current_Shape;
							Distance = dist;
							IntersectionPt[0] = Pnts[0];
							IntersectionPt[1] = Pnts[1];
							*returnFlag = true;
						}
					}
				}
			}
			else if(Current_Shape->ShapeType == RapidEnums::SHAPETYPE::SPLINE)
			{
				if(((Spline*)Current_Shape)->framgrabShape)
				{
					for(std::map<int, SinglePoint*>::iterator itr = ((ShapeWithList*)Current_Shape)->PointsList->getList().begin(); itr != ((ShapeWithList*)Current_Shape)->PointsList->getList().end(); itr++)
					{
						double Pnts[2] = {(*itr).second->X, (*itr).second->Y};
						double dist = RMATH2DOBJECT->Pt2Pt_distance(Pnts, pts);
						if(firstTime)
						{		
							CurrentSelected_Shape = Current_Shape;
							firstTime = false;						
							Distance = dist;
							IntersectionPt[0] = Pnts[0];
							IntersectionPt[1] = Pnts[1];
							*returnFlag = true;
						}
						else
						{
							if(Distance > dist)
							{
								CurrentSelected_Shape = Current_Shape;
								Distance = dist;
								IntersectionPt[0] = Pnts[0];
								IntersectionPt[1] = Pnts[1];
								*returnFlag = true;
							}
						}
					}
				}
				else
				{
					for(std::list<SinglePoint*>::iterator itr = ((Spline*)Current_Shape)->pointsOnCurve.begin(); itr != ((Spline*)Current_Shape)->pointsOnCurve.end(); itr++)
					{
						double Pnts[2] = {(*itr)->X, (*itr)->Y};
						double dist = RMATH2DOBJECT->Pt2Pt_distance(Pnts, pts);
						if(firstTime)
						{		
							CurrentSelected_Shape = Current_Shape;
							firstTime = false;						
							Distance = dist;
							IntersectionPt[0] = Pnts[0];
							IntersectionPt[1] = Pnts[1];
							*returnFlag = true;
						}
						else
						{
							if(Distance > dist)
							{
								CurrentSelected_Shape = Current_Shape;
								Distance = dist;
								IntersectionPt[0] = Pnts[0];
								IntersectionPt[1] = Pnts[1];
								*returnFlag = true;
							}
						}
					}
				}
			}
			else
				continue;
		}
		return CurrentSelected_Shape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0019a", __FILE__, __FUNCSIG__); return false; }
}

bool DXFExpose::calculateIntersectionPtOnCircle(double* pt, Shape* s1, double slope, double intercept, double* ans)
{
	try
	{
		if(s1->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			Circle* CircShape1 = (Circle*)s1;
			double center1[2] = {CircShape1->getCenter()->getX(), CircShape1->getCenter()->getY()};
			double intpoint1[4] = {0}, intpoint2[4] = {0};
			int count[2];
			RMATH2DOBJECT->Line_arcinter(slope, intercept, &center1[0], ((Circle*)s1)->Radius(), ((Circle*)s1)->Startangle(), ((Circle*)s1)->Sweepangle(), &intpoint1[0], &intpoint2[0], &count[0]);				        
			if(count[0] == 2)
			{
				double dist1 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[0]);
				double dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[2]);
				if(dist1 < dist2)
				{
					ans[0] = intpoint1[0];
					ans[1] = intpoint1[1];
					ans[2] = CircShape1->getCenter()->getZ();
				}
				else
				{
					ans[0] = intpoint1[2];
					ans[1] = intpoint1[3];
					ans[2] = CircShape1->getCenter()->getZ();
				}
			}
			else if(count[0] == 1)
			{
				ans[0] = intpoint1[0];
				ans[1] = intpoint1[1];
				ans[2] = CircShape1->getCenter()->getZ();
			}
			else
			{
				double endpts[4] = {0}, dist1 = 0, dist2 = 0;
				endpts[0] = ((Circle*)s1)->getendpoint1()->getX(); endpts[1] = ((Circle*)s1)->getendpoint1()->getY(); endpts[2] = ((Circle*)s1)->getendpoint2()->getX(); endpts[3] = ((Circle*)s1)->getendpoint2()->getY();				
				dist1 = RMATH2DOBJECT->Pt2Pt_distance(pt[0], pt[1], endpts[0], endpts[1]);
				dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt[0], pt[1], endpts[2], endpts[3]);
				if(dist1 < dist2)
				{
					ans[0] = endpts[0]; ans[1] = endpts[1]; 
					ans[2] = CircShape1->getCenter()->getZ();
				}
				else
				{
					ans[0] = endpts[2]; ans[1] = endpts[3]; 
					ans[2] = CircShape1->getCenter()->getZ();
				}
			}
		}
		else
		{
			Circle* CircShape1 = (Circle*)s1;
			double center1[2] = {CircShape1->getCenter()->getX(), CircShape1->getCenter()->getY()};
			double intpoint1[4];
			int count = RMATH2DOBJECT->Line_circleintersection(slope, intercept, &center1[0], CircShape1->Radius(), &intpoint1[0]);
			if(count == 2)
			{
				double dist1 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[0]);
				double dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[2]);
				if(dist1 < dist2)
				{
					ans[0] = intpoint1[0];
					ans[1] = intpoint1[1];
					ans[2] = CircShape1->getCenter()->getZ();
				}
				else
				{
					ans[0] = intpoint1[2];
					ans[1] = intpoint1[3];
					ans[2] = CircShape1->getCenter()->getZ();
				}
			}
			else if(count == 1)
			{
				ans[0] = intpoint1[0];
				ans[1] = intpoint1[1];
				ans[2] = CircShape1->getCenter()->getZ();
			}
			else
				return false;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0020", __FILE__, __FUNCSIG__); return false; }
}

void DXFExpose::createtemporaryLine(std::list<double> *PtsList, Shape* CurrentSelectShape)
{
	try
	{
		double *LinePts_Array = NULL;
		int n = PtsList->size(), ptsCnt = 0;
		LinePts_Array = new double[n];
		for each(double Cvalue in *PtsList)
			LinePts_Array[ptsCnt++] = Cvalue;
		double maxdist = 0, mindist = 0;
		int maxcnt = 0, mincnt = 0;
		for(int i = 0; i < n / 4; i++)
		{
			double dist= RMATH2DOBJECT->Pt2Pt_distance(LinePts_Array[4 * i], LinePts_Array[(4 * i) + 1], LinePts_Array[(4 * i) + 2], LinePts_Array[(4 * i) + 3]);
			if(mindist > dist)
			{
				mindist = dist;
				mincnt = i;
			}
			if(maxdist < dist)
			{
				maxdist = dist;
				maxcnt = i;
			}
		}

		for(int i = 0; i < n / 4; i++)
		{
			Line* myLine  = new Line();
			myLine->ShapeType = RapidEnums::SHAPETYPE::LINE;
			myLine->LineType = RapidEnums::LINETYPE::FINITELINE;
			PointCollection PtColl;
			PtColl.Addpoint(new SinglePoint(LinePts_Array[4 * i], LinePts_Array[(4 * i) + 1], 0));
			PtColl.Addpoint(new SinglePoint(LinePts_Array[(4 * i) + 2], LinePts_Array[(4 * i) + 3], 0));
			myLine->AddPoints(&PtColl);		
			myLine->Normalshape(true);	
			myLine->IsValid(true);
			CurrentSelectShape->addChild(myLine);
			myLine->addParent(CurrentSelectShape);
			AddShapeAction::addShape(myLine, false);

			if(i == maxcnt || i == mincnt)
			{
				DimBase* dim = NULL;
				RC_ITER item = MAINDllOBJECT->getShapesList().getList().end();
				item--;
				Shape* Cshape = (Shape*)(*item).second;
				dim = HELPEROBJECT->CreateShapeToShapeDim2D(Cshape, Cshape, (LinePts_Array[4 * i] + LinePts_Array[(4 * i) + 2]) / 2, (LinePts_Array[(4 * i) + 1] + LinePts_Array[(4 * i) + 1]) / 2 + 0.2, 0);
				dim->addParent(Cshape);
				Cshape->addMChild(dim);
				dim->IsValid(true);
				dim->UpdateMeasurement();
				AddDimAction::addDim(dim);
			}
		}
		delete [] LinePts_Array;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0020", __FILE__, __FUNCSIG__); }
}

void DXFExpose::create_2DLine(double *Point1, double *Point2, bool selected, bool whitecolor, bool DoNotAddAction)
{
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
		if(DoNotAddAction)
			myLine->DoNotSaveInPP = true;
		AddShapeAction::addShape(myLine, selected, DoNotAddAction);
	

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0020", __FILE__, __FUNCSIG__); }
}

bool DXFExpose::intersectionPointOnLine(double Pt1x, double Pt1y, double Pt2x, double Pt2y, double IntersectPtx, double IntersectPty)
{
	try
	{
		double MinX = 0, MaxX = 0, MinY = 0, MaxY = 0;
		if(Pt1x < Pt2x)
		{
			MinX = Pt1x;
			MaxX = Pt2x;
		}
		else
		{
			MinX = Pt2x;
			MaxX = Pt1x;
		}
		if(Pt1y < Pt2y)
		{
			MinY = Pt1y;
			MaxY = Pt2y;
		}
		else
		{
			MinY = Pt2y;
			MaxY = Pt1y;
		}
		if(MinX <= IntersectPtx && MaxX >= IntersectPtx && MinY <= IntersectPty && MaxY >= IntersectPty)
			return true;
		else
			return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0022", __FILE__, __FUNCSIG__); return false; }
}

void DXFExpose::AutoAlignmentForDxf(std::list<double> *CorrectionX, std::list<double> *CorrectionY, std::list<double> *distList)
{
	try
	{
		double tempPoint1[2] = {0}, tempPoint2[2] = {0}, tempPoint3[2] = {0}, tempPoint4[2] = {0};
		if(getExtremePointsForCloudPoints(tempPoint3, tempPoint4))
		{
			double dist = RMATH2DOBJECT->Pt2Pt_distance(tempPoint3, tempPoint4);
			if(getExtremePointsforDxfEditedShape(dist, CorrectionX, CorrectionY, distList, tempPoint1, tempPoint2))
			{
				double pt1[2] = {tempPoint1[0], tempPoint1[1]}, pt2[2] = {tempPoint3[0], tempPoint3[1]}, transform[9] = {0};
				RMATH2DOBJECT->TransformM_OnePtAlign(&pt1[0], &pt2[0], &transform[0]);
				double Point1[2] = {tempPoint3[0], tempPoint3[1]},
					pt3[2] = {tempPoint3[0], tempPoint3[1]}, pt4[2] = {tempPoint4[0], tempPoint4[1]};
				double  Point2[2] = {0};
				(MAINDllOBJECT->TransformMultiply(transform, tempPoint2[0], tempPoint2[1])).FillDoublePointer(Point2);
				DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);
				double transformMatrix[9] = {0};
				double RotatedAngle, ShiftedOrigin[2] = { 0 };
				RMATH2DOBJECT->TransformM_TwoPointAlign(Point1, Point2, &pt3[0], &pt4[0], &transformMatrix[0], &RotatedAngle, ShiftedOrigin);
				TransformMatrix(&transformMatrix[0]);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0023", __FILE__, __FUNCSIG__); }
}

bool DXFExpose::getExtremePointsforDxfEditedShape(double dist, std::list<double> *CorrectionX, std::list<double> *CorrectionY, std::list<double> *distList, double *LefMost, double *RightMost)
{
	try
	{
		int cnt = 0, CorrectionCountX = 0;
		double temp1[2] = {0, 0}, temp2[2] = {0, 0}, tempdist[2] = {0, 0};
		int totalCnt = distList->size();
		for each(double Cvalue in *distList)
		{
			cnt++;
			if(dist < Cvalue)
			{
				tempdist[1] = Cvalue;
				break;
			}	
			if(totalCnt == cnt)
			{
				tempdist[1] = Cvalue;
				break;
			}
			tempdist[0] = Cvalue;							
		}

		for each(double Cvalue in *CorrectionX)
		{
			CorrectionCountX++;
			if(CorrectionCountX == cnt - 1)
				temp1[0] = Cvalue;
			else if(CorrectionCountX == cnt)
				temp2[0] = Cvalue;
		}
		CorrectionCountX = 0;
		for each(double Cvalue in *CorrectionY)
		{
			CorrectionCountX++;
			if(CorrectionCountX == cnt - 1)
				temp1[1] = Cvalue;
			else if(CorrectionCountX == cnt)
				temp2[1] = Cvalue;
		}
		for each(double Cvalue in *CorrectionX)
		{
			LefMost[0] = Cvalue;
			break;
		}
		for each(double Cvalue in *CorrectionY)
		{
			LefMost[1] = Cvalue;
			break;
		}
		//Linear interpolation.................
		RightMost[0] =  (((dist - tempdist[0]) * (temp2[0] - temp1[0])) / (tempdist[1] - tempdist[0])) + temp1[0];

		RightMost[1] = (((dist - tempdist[0]) * (temp2[1] - temp1[1])) / (tempdist[1] - tempdist[0])) + temp1[1];
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0024", __FILE__, __FUNCSIG__); return false;}
}

bool DXFExpose::getExtremePointsForCloudPoints(double *LeftMost, double *RightMost)
{
	try
	{
		bool firstTime = true, succeccflag = false;
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				succeccflag = true;
				if(CShape->PointsList->getList().size() < 2)
					return false;
				for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
				{
					SinglePoint* Spt = (*SptItem).second;
					if(firstTime)
					{
						firstTime = false;
						LeftMost[0] = Spt->X; 
						LeftMost[1] = Spt->Y; 
						RightMost[0] = Spt->X; 
						RightMost[1] = Spt->Y;
					}
					else
					{
						if(LeftMost[0] > Spt->X)
						{
							LeftMost[0] = Spt->X; LeftMost[1] = Spt->Y;
						}
						if(RightMost[0] < Spt->X)
						{
							RightMost[0] = Spt->X; RightMost[1] = Spt->Y;
						}
					}
				}
				break;
			}
		}		
		return succeccflag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0024", __FILE__, __FUNCSIG__); return false;}
}

//Autoalignment for selected shapes...........
void DXFExpose::AutoAlignmentForSelectedShapes(bool SelectedPt, int alignment_mode)
{
	try
	{
		Curve_2DList Dxf_CurveList;
		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!(CShape->ShapeType == RapidEnums::SHAPETYPE::ARC || CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)) continue;
			Curve_2D Dxf_Curve2D;
			PtsList Dxf_pts_list;
			 
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				Dxf_Curve2D.curve = Curves_2D::arc;
				Dxf_Curve2D.answer[0] = ((Circle*)CShape)->getCenter()->getX();
				Dxf_Curve2D.answer[1] = ((Circle*)CShape)->getCenter()->getY();
				Dxf_Curve2D.answer[2] = ((Circle*)CShape)->Radius();
				Dxf_Curve2D.answer[3] = ((Circle*)CShape)->Length();
				Dxf_Curve2D.answer[4] = ((Circle*)CShape)->Startangle();
				Dxf_Curve2D.answer[5] = ((Circle*)CShape)->Sweepangle();

				Pt Arc_StartPt;
				Arc_StartPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getendpoint1()->getX();
				Arc_StartPt.y = ((Circle*)CShape)->getendpoint1()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);

				Pt Arc_MidPt;
				Arc_MidPt.pt_index = 0;
				Arc_MidPt.x = ((Circle*)CShape)->getMidPoint()->getX();
				Arc_MidPt.y = ((Circle*)CShape)->getMidPoint()->getY();
				Dxf_pts_list.push_back(Arc_MidPt);

				Pt Arc_EndPt;
				Arc_EndPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getendpoint2()->getX();
				Arc_StartPt.y = ((Circle*)CShape)->getendpoint2()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				Dxf_Curve2D.curve = Curves_2D::line;
				Dxf_Curve2D.answer[0] = tan(((Line*)CShape)->Angle());
				Dxf_Curve2D.answer[1] = ((Line*)CShape)->Intercept();
				Dxf_Curve2D.answer[2] = ((Line*)CShape)->Length();

				Pt Line_StartPt;
				Line_StartPt.pt_index = 0;
				Line_StartPt.x = ((Line*)CShape)->getPoint1()->getX();
				Line_StartPt.y = ((Line*)CShape)->getPoint1()->getY();
				Dxf_pts_list.push_back(Line_StartPt);

				Pt Line_EndPt;
				Line_EndPt.pt_index = 0;
				Line_EndPt.x = ((Line*)CShape)->getPoint2()->getX();
				Line_EndPt.y = ((Line*)CShape)->getPoint2()->getY();
				Dxf_pts_list.push_back(Line_EndPt);
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				Dxf_Curve2D.curve = Curves_2D::arc;
				Dxf_Curve2D.answer[0] = ((Circle*)CShape)->getCenter()->getX();
				Dxf_Curve2D.answer[1] = ((Circle*)CShape)->getCenter()->getY();
				Dxf_Curve2D.answer[2] = ((Circle*)CShape)->Radius();
				Dxf_Curve2D.answer[3] = ((Circle*)CShape)->Length();
				Dxf_Curve2D.answer[4] = 0;
				Dxf_Curve2D.answer[5] = 2 * M_PI;

				Pt Arc_StartPt;
				Arc_StartPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getCenter()->getX() + ((Circle*)CShape)->Radius();
				Arc_StartPt.y = ((Circle*)CShape)->getCenter()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);

				Pt Arc_MidPt;
				Arc_MidPt.pt_index = 0;
				Arc_MidPt.x = ((Circle*)CShape)->getCenter()->getX() - ((Circle*)CShape)->Radius();
				Arc_MidPt.y = ((Circle*)CShape)->getCenter()->getY();
				Dxf_pts_list.push_back(Arc_MidPt);

				Pt Arc_EndPt;
				Arc_EndPt.pt_index = 0;
				Arc_StartPt.x = ((Circle*)CShape)->getCenter()->getX() + ((Circle*)CShape)->Radius();
				Arc_StartPt.y = ((Circle*)CShape)->getCenter()->getY();
				Dxf_pts_list.push_back(Arc_StartPt);
			}
			Dxf_Curve2D.pts_list = Dxf_pts_list;
			Dxf_Curve2D.curve_index = 0;
			Dxf_CurveList.push_back(Dxf_Curve2D);
		}

		double *Pts_Array = NULL;
		int PtsCnt = 0;
		bool firsttime = true;
		for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!firsttime) break;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				firsttime = false;
				PtsCnt = CShape->PointsList->SelectedPointsCnt;
				if(PtsCnt < 1)
				{
					PtsCnt = CShape->PointsList->Pointscount();
					Pts_Array = new double[2 * PtsCnt];
					int n = 0;
					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;
						Pts_Array[n++] = Spt->X;
						Pts_Array[n++] = Spt->Y;
					}
				}
				else
				{
					Pts_Array = new double[2 * PtsCnt];
					int n = 0;
					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;
						if(!Spt->IsSelected) continue;
						Pts_Array[n++] = Spt->X;
						Pts_Array[n++] = Spt->Y;
					}
				}
			}
		}
		double Shift_X = 0, Shift_Y = 0, R_Theta = 0, RotationPt[2] = {0};
		bool AlignmentFlag = BESTFITOBJECT->BFalignment_2dpts_with_lines_arcs(Pts_Array, PtsCnt, Dxf_CurveList, &Shift_X, &Shift_Y, &R_Theta, &RotationPt[0], 0, 50, alignment_mode);
		if(!AlignmentFlag) return;

		if(Pts_Array != NULL)
			delete [] Pts_Array;

		//if we want to rotate line arc instead of cloud points then multiply by -1
		R_Theta = -R_Theta;	Shift_X = -Shift_X;	Shift_Y = -Shift_Y;
		//RotationPt[0] = -RotationPt[0]; RotationPt[1] = -RotationPt[1];
		double RotateMatrix[9] = {0};
		RotateMatrix[0] = cos(R_Theta); RotateMatrix[1] = -sin(R_Theta); 
		RotateMatrix[3] = sin(R_Theta); RotateMatrix[4] = cos(R_Theta); 
		RotateMatrix[8] = 1;

		Vector translate;
		translate.set(Shift_X, Shift_Y);

		Vector GotoOrigin;
		GotoOrigin.set(-RotationPt[0], -RotationPt[1]);

		Vector GotoRotationPts;
		GotoRotationPts.set(RotationPt[0], RotationPt[1]);

		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) continue;
			if(SelectedPt)
			{
				if(!CShape->selected()) continue;
			}
			switch(alignment_mode)
			{
			case 0://translate and rotate both...		
				CShape->Translate(GotoOrigin);
				CShape->Transform(RotateMatrix);
				CShape->Translate(GotoRotationPts);
				CShape->Translate(translate);
				break;
			case 1://translate only..
				CShape->Translate(translate);
				break;
			case 2://rotate only...
				CShape->Translate(GotoOrigin);
				CShape->Transform(RotateMatrix);
				CShape->Translate(GotoRotationPts);
				break;
			}
		}



		/*double RotateMatrix[9] = {0};
		RotateMatrix[0] = cos(R_Theta); RotateMatrix[1] = -sin(R_Theta); 
		RotateMatrix[3] = sin(R_Theta); RotateMatrix[4] = cos(R_Theta); 
		RotateMatrix[8] = 1;

		Vector translate;
		translate.set(Shift_X, Shift_Y);

		Vector GotoOrigin;
		GotoOrigin.set(-RotationPt[0], -RotationPt[1]);

		Vector GotoRotationPts;
		GotoRotationPts.set(RotationPt[0], RotationPt[1]);*/

		//for(RC_ITER Shpitem = MAINDllOBJECT->getSelectedShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getSelectedShapesList().getList().end(); Shpitem++)
		//{
		//	Shape* CShape = (Shape*)((*Shpitem).second);
		//	if(!CShape->Normalshape()) continue;
		//	if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
		//	{
		//		((CloudPoints*)CShape)->SelectedPtsOnly(SelectedPt);
		//		switch(alignment_mode)
		//		{
		//		case 0://translate and rotate both...
		//			CShape->Translate(translate);
		//			CShape->Translate(GotoOrigin);
		//			CShape->Transform(RotateMatrix);
		//			CShape->Translate(GotoRotationPts);
		//			break;
		//		case 1://translate only..
		//			CShape->Translate(translate);
		//			break;
		//		case 2://rotate only...
		//			CShape->Translate(GotoOrigin);
		//			CShape->Transform(RotateMatrix);
		//			CShape->Translate(GotoRotationPts);
		//			break;
		//		}	
		//		((CloudPoints*)CShape)->SelectedPtsOnly(false);
		//		CShape->UpdateEnclosedRectangle();
		//	}
		//}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0025", __FILE__, __FUNCSIG__);}
}

int DXFExpose::getNumberOfIntersectionPoints(RCollectionBase& ShapeCollection, double *RayPt)
{
	try
	{
		int NoOfintersection = 0;
		for(RC_ITER Shpitem = ShapeCollection.getList().begin(); Shpitem != ShapeCollection.getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				int Count[2] = {0};
				double intpt1[2] = {0}, intpt2[2] = {0};
				double endPoint1[2] = {((Line*)CShape)->getPoint1()->getX(), ((Line*)CShape)->getPoint1()->getY()};
				double endPoint2[2] = {((Line*)CShape)->getPoint2()->getX(), ((Line*)CShape)->getPoint2()->getY()};
				if(RMATH2DOBJECT->ray_finitelineint(endPoint1, endPoint2, ((Line*)CShape)->Angle(), ((Line*)CShape)->Intercept(), RayPt, 0, RayPt[1], intpt1, intpt2, Count) == 1)
					NoOfintersection++;
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				int Count[2] = {0};
				double intpt1[4] = {0}, intpt2[4] = {0};
				double center[2] = {((Circle*)CShape)->getCenter()->getX(), ((Circle*)CShape)->getCenter()->getY()};
				int intpoints = RMATH2DOBJECT->ray_circleint(center, ((Circle*)CShape)->Radius(), RayPt, 0, RayPt[1], intpt1, intpt2, Count);
				if(intpoints == 1)
					NoOfintersection += 2;
				else if(intpoints == 2 || intpoints == 3)
					NoOfintersection++;
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				int Count[2] = {0};
				double intpt1[4] = {0}, intpt2[4] = {0};
				double center[2] = {((Circle*)CShape)->getCenter()->getX(), ((Circle*)CShape)->getCenter()->getY()};
				int intpoints = RMATH2DOBJECT->ray_arcinter(RayPt, 0, RayPt[1], center, ((Circle*)CShape)->Radius(), ((Circle*)CShape)->Startangle(), ((Circle*)CShape)->Sweepangle(), intpt1, intpt2, Count);				        
				if(intpoints == 1)
					NoOfintersection += 2;
				else if(intpoints == 2 || intpoints == 5)
					NoOfintersection++;
			}
		}
		return NoOfintersection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0026", __FILE__, __FUNCSIG__); return 0; }
}

void DXFExpose::CreatePointCsvFile(std::string filePath)
{
	try
	{
		int UcsCount = 1;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			RCollectionBase& rshapes =  Cucs->getShapes();
			std::string UcsNumber ="", CCount ="", fileName = filePath.substr(12);
			int i = 1;
			char temp[10] = "/0";
			for(RC_ITER It = rshapes.getList().begin(); It !=  rshapes.getList().end(); It++)
			{
				ShapeWithList *cShape = (ShapeWithList*)(It->second);
				if(!cShape->Normalshape()) continue;
				if(cShape->PointsListOriginal->getList().empty()) continue;
				wofstream ofile;
				itoa(i, &temp[0], 10);
				CCount = (const char*)temp;
				itoa(UcsCount, &temp[0], 10);
				UcsNumber = (const char*)temp;
				ofile.open(PointsDirectoryPath + fileName + "Shape" + CCount + "_" + UcsNumber + ".csv");
				ofile << (char*)cShape->getCurrentShapeTypeName().c_str() << endl;
				int pntcnt = 0;
				for(PC_ITER SptItem = cShape->PointsList->getList().begin(); SptItem != cShape->PointsList->getList().end(); SptItem++)
				{
					pntcnt++;
					ofile << "Point" << pntcnt << "_X" << "," ;
					ofile << "Point" << pntcnt << "_Y" << "," ;
					ofile << "Point" << pntcnt << "_Z" << "," ;	
				}
				ofile << endl;
				ofile.close();
				i++;
			}
			UcsCount++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0027", __FILE__, __FUNCSIG__); }
}

void DXFExpose::WriteToPointCsvFile(std::string filePath)
{
	try
	{
		int UcsCount = 1;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			RCollectionBase& rshapes =  Cucs->getShapes();
			std::string UcsNumber = "", CCount = "", fileName = filePath.substr(12);

			//We get the last part of the filepath to get the name of the program.
			std::string tempProgName;
			std::istringstream iss(filePath);
			while (getline(iss, tempProgName, '\\'))
			{
				fileName = tempProgName;
			}
			fileName = fileName.substr(0, fileName.length() - 4);

			int i = 1;
			char temp[10] = "/0";
			for(RC_ITER It = rshapes.getList().begin(); It !=  rshapes.getList().end(); It++)
			{
				ShapeWithList *cShape = (ShapeWithList*)(It->second);
				if(!cShape->Normalshape()) continue;
				if(cShape->PointsListOriginal->getList().empty()) continue;
				//wifstream ifile;
				itoa(i, &temp[0], 10);
				CCount = (const char*)temp;
				itoa(UcsCount, &temp[0], 10);
				UcsNumber = (const char*)temp;
				/*ifile.open(PointsDirectoryPath + fileName + "Shape" + CCount + "_" + UcsNumber  + ".csv");
				if(!ifile.is_open())
				{
					ifile.close();
					break;
				}*/
				wofstream ofile;
				ofile.open(PointsDirectoryPath + fileName + "Shape" + CCount + "_" + UcsNumber + ".csv", std::ios_base::app);
				if(!ofile) break;
				//ofile << "X" << "," << "Y" << "," << "Z" << ",";
				//switch (MAINDllOBJECT->CurrentMahcineType)
				//{
				//case RapidEnums::RAPIDMACHINETYPE::HOBCHECKER:
				//case RapidEnums::RAPIDMACHINETYPE::TIS:
				//	ofile << "C";
				//	break;
				//}
				ofile << endl;
				for(PC_ITER SptItem = cShape->PointsListOriginal->getList().begin(); SptItem != cShape->PointsListOriginal->getList().end(); SptItem++)
				{
					ofile << (*SptItem).second->X << "," ;
					ofile << (*SptItem).second->Y << "," ;
					ofile << (*SptItem).second->Z << "," ;	
					switch (MAINDllOBJECT->CurrentMahcineType)
					{
					case RapidEnums::RAPIDMACHINETYPE::HOBCHECKER:
					case RapidEnums::RAPIDMACHINETYPE::TIS:
						ofile << (*SptItem).second->R;
						break;
					}

				}
				ofile << endl;
				ofile.close();
				i++;
			}
			UcsCount++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0028", __FILE__, __FUNCSIG__); }
}

void DXFExpose::CreateShapesParamCsvFile(std::string filePath)
{
	try
	{
		int UcsCount = 1;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			RCollectionBase& rshapes =  Cucs->getShapes();
			std::string UcsNumber ="", CCount ="", fileName = filePath.substr(12);
			int i = 1;
			char temp[10] = "/0";
			for(RC_ITER It = rshapes.getList().begin(); It !=  rshapes.getList().end(); It++)
			{
				ShapeWithList *cShape = (ShapeWithList*)(It->second);
				if(!cShape->Normalshape()) continue;
				if(cShape->PointsListOriginal->getList().empty()) continue;
				wofstream ofile;
				itoa(i, &temp[0], 10);
				CCount = (const char*)temp;
				itoa(UcsCount, &temp[0], 10);
				UcsNumber = (const char*)temp;
				ofile.open(PointsDirectoryPath + fileName + "ShapeParam" + CCount + "_" + UcsNumber + ".csv");
				ofile << (char*)cShape->getCurrentShapeTypeName().c_str() << endl;
				ofile << endl;
				ofile.close();
				i++;
			}
			UcsCount++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0027", __FILE__, __FUNCSIG__); }
}

void DXFExpose::WriteToShapesParamCsvFile(std::string filePath)
{
	try
	{
		int UcsCount = 1;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			RCollectionBase& rshapes =  Cucs->getShapes();
			std::string UcsNumber = "", CCount = "", fileName = filePath.substr(12);
			int i = 1;
			char temp[10] = "/0";
			for(RC_ITER It = rshapes.getList().begin(); It !=  rshapes.getList().end(); It++)
			{
				ShapeWithList *cShape = (ShapeWithList*)(It->second);
				if(!cShape->Normalshape()) continue;
				if(cShape->PointsListOriginal->getList().empty()) continue;
				//wifstream ifile;
				itoa(i, &temp[0], 10);
				CCount = (const char*)temp;
				itoa(UcsCount, &temp[0], 10);
				UcsNumber = (const char*)temp;
				wofstream ofile;
				ofile.open(PointsDirectoryPath + fileName + "ShapeParam" + CCount + "_" + UcsNumber + ".csv", std::ios_base::app);
				if(!ofile) break;
				WriteShapeParametersToCsv(ofile, cShape);
				ofile.close();
				i++;
			}
			UcsCount++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0028", __FILE__, __FUNCSIG__); }
}
//void DXFExpose::Cam_profile_Circle_Centre(double x, double y, double z, bool Tool_type)			//11/02/2014 vinod.. for excel report we need here cam profile circle center..
//{
//	Cam_P_Circle_Centre[0] = x;
//	Cam_P_Circle_Centre[1] = y;
//	Cam_P_Circle_Centre[2] = z;
//	Is_ID = Tool_type;
//}

void DXFExpose::Cam_profile_Circle_Centre(double x, double y, double z, bool Tool_type, double Point3DForZPosition_Zvalue, double MinR, double MaxR)			//11/02/2014 vinod.. for excel report we need here cam profile circle center..
{
	Cam_P_Circle_Centre[0] = x;
	Cam_P_Circle_Centre[1] = y;
	Cam_P_Circle_Centre[2] = z;
	Is_ID = Tool_type;
	Point3DForZPositionZValue = Point3DForZPosition_Zvalue;
	MinRadi = MinR;
	MaxRadi = MaxR;
}


//vinod GethobCheckerPersonaldatain_CsvFile 22/02/2014
void DXFExpose::GethobCheckerPersonaldatain_CsvFile(int OrderNo, int SerialNo, int ToolID, char* CustomerName, char* Date, char* Operator, int MachineNo, bool IsHobTypeBore, char* AccuracyClass, double leadofGash, char* Module)
{
	try
		{
			wifstream ifile;
					ifile.open(this->PointsDirectoryPath + "GetHobCheckerPersonalData.txt");
					if(ifile)
					{
						ifile.close();
					}
					wofstream ofile;
					ofile.open(this->PointsDirectoryPath + "GethobCheckerPersonalData.csv", std::ios_base::app);
					if(!ofile) return;					
					
						ofile << "OrderNo " << "," << OrderNo << endl;
						ofile << "SerialNo " << "," << SerialNo << endl;
						ofile << "ToolID " << "," << ToolID << endl;
						ofile << "CustomerName " << "," << CustomerName << endl;
						ofile << "Date " << "," << Date << endl;
						ofile << "Operator " << "," << Operator << endl;
						ofile << "MachineNo " << "," << MachineNo << endl;
						ofile << "IsHobTypeBore " << "," << IsHobTypeBore << endl;
						ofile << "AccuracyClass " << "," << AccuracyClass << endl;
						//ofile << "MandrealLength " << "," << MandrealLength << endl;
						ofile << "leadofGash " << "," << leadofGash << endl;
						ofile << "Module " << "," << Module << endl;

					ofile.close();
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0028", __FILE__, __FUNCSIG__); }
}


void DXFExpose::WriteCamProfileDist_CsvFile(double angle, int PtsToBundle)
{
	if(!Is_ID)									//vinod  11/02/2014    excel report for Outer Tool
	{
		try
		{
			double sector_angle = angle;
			angle = angle * M_PI / 180;
			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
			{
				ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
				if(!CShape->Normalshape()) continue;
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CAMPROFILE && CShape->PointsList->Pointscount() > 1)
				{
					int AngleCount = M_PI * 2 / angle;
					wifstream ifile;
					ifile.open(this->PointsDirectoryPath + "CamParameter.txt");
					if(ifile)
					{
						ifile >> PtsToBundle;
						ifile.close();
					}
					wofstream ofile;
					ofile.open(this->PointsDirectoryPath + "CamProfile.csv", std::ios_base::app);
					if(!ofile) return;
					double CamParam[4] = {0};
					((CamProfile*)CShape)->GetCamProfileParam(CamParam);
					double RelativeAngle = 0, Dist = 0, LocalDist = 0;
					bool GetDistance = false;
					PtsList XYPtsCollection;
					int SkipPts = 0, TmpCount = 0;
					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
					{		
						if(TmpCount >= SkipPts)
						{
							SinglePoint* Spt = (*SptItem).second;
							Pt Point_XY;
							Point_XY.pt_index = Spt->PtID;
							Point_XY.x = Spt->X;
							Point_XY.y = Spt->Y;
							XYPtsCollection.push_back(Point_XY);
							TmpCount = 0;
						}
						else
							TmpCount++;
					}
					double dist;
					int i=1;
					for each(Pt Point_XY in XYPtsCollection)
					{
						dist = RMATH2DOBJECT->Pt2Pt_distance(Cam_P_Circle_Centre[0], Cam_P_Circle_Centre[1], Point_XY.x, Point_XY.y);
						ofile << sector_angle * i++ << "," << dist << endl;
					}				
					ofile.close();
				}
			}
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0028", __FILE__, __FUNCSIG__); }
	}
	else													//vinod  11/02/2014    excel report for Inner Tool
	{
		try
		{
			angle = angle * M_PI / 180;
			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
			{
				ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
				if(!CShape->Normalshape()) continue;
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CAMPROFILE && CShape->PointsList->Pointscount() > 1)
				{
					int AngleCount = M_PI * 2 / angle;
					wifstream ifile;
					ifile.open(this->PointsDirectoryPath + "CamParameter.txt");
					if(ifile)
					{
						ifile >> PtsToBundle;
						ifile.close();
					}
					wofstream ofile;
					ofile.open(this->PointsDirectoryPath + "CamProfile.csv", std::ios_base::app);
					if(!ofile) return;
					double CamParam[4] = {0};
					((CamProfile*)CShape)->GetCamProfileParam(CamParam);
					double RelativeAngle = 0, Dist = 0, LocalDist = 0;
					bool GetDistance = false;
					PointCollection SortedPtsCollection;
					PtsList XYPtsCollection;
					int SkipPts = 0, TmpCount = 0;
					if(CShape->PointsList->Pointscount() > 300)
						SkipPts = CShape->PointsList->Pointscount() / 300;
					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
					{		
						if(TmpCount >= SkipPts)
						{
							SinglePoint* Spt = (*SptItem).second;
							Pt Point_XY;
							Point_XY.pt_index = Spt->PtID;
							Point_XY.x = Spt->X;
							Point_XY.y = Spt->Y;
							XYPtsCollection.push_back(Point_XY);
							TmpCount = 0;
						}
						else
							TmpCount++;
					}
					//BESTFITOBJECT->orderpoints(&XYPtsCollection);					//vinod ... it gives  memory error and program abort.
					for each(Pt Point_XY in XYPtsCollection)
					{
						SinglePoint* Spt = new SinglePoint();
						Spt->PtID = Point_XY.pt_index;
						Spt->X = Point_XY.x;
						Spt->Y = Point_XY.y;
						Spt->Z = CShape->PointsList->getList()[Spt->PtID]->Z;
						SortedPtsCollection.Addpoint(Spt);
					}
					for(int i = 0; i < AngleCount; i++)
					{
						RelativeAngle = i * angle;
						double LocalCamParam[4] = {0};
						if(GetLocalCamProfileParameter(PtsToBundle, RelativeAngle + CamParam[3], &SortedPtsCollection, CamParam, LocalCamParam))
						{
							BESTFITOBJECT->CamProfile_GetDist(CamParam, RelativeAngle + CamParam[3], &Dist);
							BESTFITOBJECT->CamProfile_GetDist(LocalCamParam, CamParam, RelativeAngle + CamParam[3], &LocalDist);
							ofile << RelativeAngle * 180 / M_PI << "," << Dist << "," << LocalDist << endl;
						}
					}
					ofile.close();
				}
			}
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0028", __FILE__, __FUNCSIG__); }
	}
}

bool DXFExpose::GetLocalCamProfileParameter(int PtsToBundle, double Angle, PointCollection* PtsList, double* CamParam, double* LocalParam)
{
	try
	{
		double MinDist = 9999, PtDist = 9999;
		double CenterPoint2[3] = {PtDist * cos(Angle) + CamParam[0], PtDist * sin(Angle) + CamParam[1], 0}; 
		PC_ITER SptItem = PtsList->getList().begin();
		bool GetDist = false;
		double Intercept = RMATH2DOBJECT->InterceptOfline(Angle, CamParam);
		for(PC_ITER SptItem1 = PtsList->getList().begin(); SptItem1 != PtsList->getList().end(); SptItem1++)
		{
			SinglePoint* Spt1 = (*SptItem1).second;
			double Point1[3] = {Spt1->X, Spt1->Y, Spt1->Z}, intersectpoints[3] = {0};	
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(Angle, Intercept, Point1, intersectpoints);
			if(!DXFEXPOSEOBJECT->intersectionPointOnLine(CenterPoint2[0], CenterPoint2[1], CamParam[0], CamParam[1], intersectpoints[0], intersectpoints[1])) continue;
			double dist = RMATH2DOBJECT->Pt2Line_Dist(Point1[0], Point1[1], Angle, Intercept);
			if(MinDist > dist)
			{
				GetDist = true;
				MinDist = dist;
				SptItem = SptItem1;
			}
		}
		if(GetDist)
		{
			int BestFitPts = PtsToBundle * 2 + 1;
			double TempPoint[3] = {((*SptItem).second)->X, ((*SptItem).second)->Y, ((*SptItem).second)->Z};
			if(BestFitPts < 3 || BestFitPts > PtsList->Pointscount() - 1) return false;
			double *PtsArray = new double[BestFitPts * 2];
			int count = PtsToBundle;
			for(int i = 0; i < count; i++)
			{
				if(SptItem == PtsList->getList().begin())
				{
					SptItem = PtsList->getList().end();
				}
				SptItem--;
			}
			for(int i = 0; i < BestFitPts; i++)
			{
				if(SptItem == PtsList->getList().end())
				{
					SptItem = PtsList->getList().begin();
				}
				PtsArray[2 * i] = ((*SptItem).second)->X; PtsArray[2 * i + 1] = ((*SptItem).second)->Y;
				SptItem++;
			}
			double InitialGuess[4] = {CamParam[0], CamParam[1], CamParam[2], CamParam[3]}, Answer[4] = {0};
			if(BESTFITOBJECT->CamProfile_BestFit(PtsArray, BestFitPts, InitialGuess, Answer))
			{
				for(int i = 0; i < 4; i++)
					LocalParam[i] = Answer[i];
			}
			else
				GetDist = false;
			delete [] PtsArray;
		}
		return GetDist;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFEX0028", __FILE__, __FUNCSIG__); return false;}
}

