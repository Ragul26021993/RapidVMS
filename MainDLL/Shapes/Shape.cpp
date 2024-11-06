#include "StdAfx.h"
#include "Shape.h"
#include "Sphere.h"
#include "Circle.h"
#include "RPoint.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PointCollection.h"
#include "..\Engine\PartProgramFunctions.h"

Shape::Shape(TCHAR* myname):BaseItem(myname)
{
	init();	
}

Shape::Shape(bool simply):BaseItem(_T("Shape"))
{
	init();	
}

Shape::~Shape()
{
}

void Shape::init()
{
	try
	{
		this->ShapeAsfasttrace(false); this->ShowFGpointsflag(false);
		this->Normalshape(true); this->ShapeAsReferenceShape(false);
		this->ShapeThickness(1); this->ShapeAs3DShape(false);
		this->HighlightedFormouse(false); this->ShapeForCurrentFastrace(false);
		this->ShapeForThreadMeasurement(false); this->FinishedThreadMeasurement(false);
		this->ShapeDoneUsingProbe(false); this->ShapeForPerimeter(false);
		this->HighlightedForPCDHighLight(false);
		this->HighlightedForMeasurement(false);
		this->ShapeUsedForTwoStepHoming(false);
		this->ShapeAsRotaryEntity(false);
		this->UseLightingProperties(false);
		this->DoneFrameGrab(false);
		this->IgnoreManualCrosshairSteps(false);
		this->AppliedLineArcSeperation(false);
		this->AppliedLineArcSepOnce(false);
		this->PartialShape(false);
		//this->IsUsedForReferenceDot(false);
		this->PartialShapeDrawMode(0);
		this->UcsId(MAINDllOBJECT->getCurrentUCS().getId());
		this->RefernceDatumForgdnt(0);
		this->DatumName = '0';
		for(int i = 0; i < 3; i++)
		{
			ShapeLeftTop[i] = 0; ShapeRightBottom[i] = 0;
			FgPointLeftTop[i] = 0; FgPointsRightBottom[i] = 0;
			this->IntersectnPts[i] = 0;
		}
		this->Intersects = 0;
		this->IsValid(false);	
		this->IsHidden(false);
		this->IsDxfShape(false);
		this->IsAlignedShape(false);
		this->ProbeCorrection(true);
		this->XOffsetCorrection(false);
		this->YOffsetCorrection(false);
		this->ZOffsetCorrection(false);
		this->ROffsetCorrection(false);
		this->AddActionWhileEdit(true);
		this->SigmaMode(MAINDllOBJECT->SigmaModeFlag());
		this->FlatnessFilterFactor(2);
		this->ApplyMMCCorrection(MAINDllOBJECT->ApplyMMCCorrection());
		this->ApplyFilterCorrection(MAINDllOBJECT->ApplyFilterCorrection());
		this->MMCCorrectionIterationCnt(1);
		this->FilterCorrectionIterationCnt(MAINDllOBJECT->FilterCorrectionIterationCnt());
		this->FilterCorrectionFactor(MAINDllOBJECT->FilterCorrectionFactor());
		this->OuterCorrection(MAINDllOBJECT->OuterCorrection());
		this->ScanDirDetected(false);
		this->ApplyPixelCorrection(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0001", __FILE__, __FUNCSIG__); }
}

void Shape::CopyOriginalProperties(Shape* Cshape)
{
	try
	{
		this->ShapeType = Cshape->ShapeType;
		this->ShapeAsReferenceShape(Cshape->ShapeAsReferenceShape());
		this->ShapeThickness(Cshape->ShapeThickness());
		this->ShapeAsfasttrace(Cshape->ShapeAsfasttrace());
		this->IgnoreManualCrosshairSteps(Cshape->IgnoreManualCrosshairSteps());
		this->AppliedLineArcSeperation(Cshape->AppliedLineArcSeperation());
		this->AppliedLineArcSepOnce(Cshape->AppliedLineArcSepOnce());
		this->Normalshape(Cshape->Normalshape());
		this->ShapeAs3DShape(Cshape->ShapeAs3DShape());
		this->ShapeDoneUsingProbe(Cshape->ShapeDoneUsingProbe());
		this->ShapeForPerimeter(Cshape->ShapeForPerimeter());
		this->ShapeUsedForTwoStepHoming(Cshape->ShapeUsedForTwoStepHoming());
		this->ShapeAsRotaryEntity(Cshape->ShapeAsRotaryEntity());
		this->PartialShape(Cshape->PartialShape());
		this->PartialShapeDrawMode(Cshape->PartialShapeDrawMode());
		this->UcsId(Cshape->UcsId());
		this->IsDxfShape(Cshape->IsDxfShape());
		this->DoNotSaveInPP = Cshape->DoNotSaveInPP;
		this->XOffsetCorrection(Cshape->XOffsetCorrection());
		this->YOffsetCorrection(Cshape->YOffsetCorrection());
		this->ZOffsetCorrection(Cshape->ZOffsetCorrection());
		this->ROffsetCorrection(Cshape->ROffsetCorrection());
		this->CopyShapeParameters(Cshape);
		this->UpdateEnclosedRectangle();
		this->ShapeColor.set(Cshape->ShapeColor.r, Cshape->ShapeColor.g, Cshape->ShapeColor.b);
		((ShapeWithList*)this)->PointsList->deleteAll();
		((ShapeWithList*)this)->PointsListOriginal->deleteAll();
		((ShapeWithList*)this)->PointAtionList.clear();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0002", __FILE__, __FUNCSIG__); }
}

double Shape::GetZlevel()
{
	return 0;
}

void Shape::GetShapeCenter(double *cPoint)
{
}
void Shape::GetSelectedPointOnShape(double *IntersectPt, int *Count)
{
	for(int i = 0; i < this->Intersects * 3; i++)
	   IntersectPt[i] = this->IntersectnPts[i];
	*Count = this->Intersects;
}

bool Shape::WithinCirclularView(double *SelectionLine, double Tolerance)
{
	return false;
}

bool Shape::GetShapeExtents(double *Lefttop, double *Rightbottom)
{
	try
	{
		if(this->ShowFGpointsflag())
		{
			for(int i = 0; i < 3; i++)
			{
				Lefttop[i] = FgPointLeftTop[i];
				Rightbottom[i] = FgPointsRightBottom[i];
			}
		}
		else
		{
			for(int i = 0; i < 3; i++)
			{
				Lefttop[i] = ShapeLeftTop[i];
				Rightbottom[i] = ShapeRightBottom[i];
			}
		}
		//Remove the shape type from the if condition.. once you implement the enclosed rectangle/ cube..!!
		if(ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE || ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D || ShapeType == RapidEnums::SHAPETYPE::PARABOLA3D || ShapeType == RapidEnums::SHAPETYPE::HYPERBOLA3D || ShapeType == RapidEnums::SHAPETYPE::DUMMY || 
			ShapeType == RapidEnums::SHAPETYPE::TWOARC || ShapeType == RapidEnums::SHAPETYPE::TWOLINE || ShapeType == RapidEnums::SHAPETYPE::DEFAULTSHAPE)
			return false;
		else
		{
			/*if(IsEqual(Lefttop[0], Rightbottom[0]) || IsEqual(Lefttop[1], Rightbottom[1]))
				return false;
			else*/
				return true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0003", __FILE__, __FUNCSIG__); }
}

void Shape::UpdateRefelctedShape(Shape *Rshape, double *reflectionSurfaceParam, bool reflectionAlongPlane)
{
	ShapeWithList *cShape = (ShapeWithList*)this;
	if(reflectionAlongPlane)
	{
		for(PC_ITER item = cShape->PointsList->getList().begin(); item != cShape->PointsList->getList().end(); item++)
		{    
			double reflectedPoints[3] = {0}, tmpPoint[3] = {item->second->X, item->second->Y, item->second->Z};
			RMATH3DOBJECT->ReflectedPointsAlongPlane(tmpPoint, 1, reflectionSurfaceParam, reflectedPoints);
			((ShapeWithList*)Rshape)->PointsListOriginal->Addpoint(new SinglePoint(reflectedPoints[0], reflectedPoints[1], reflectedPoints[2]));
			((ShapeWithList*)Rshape)->PointsList->Addpoint(new SinglePoint(reflectedPoints[0], reflectedPoints[ 1], reflectedPoints[2]));
		}
	}
	else
	{
		for(PC_ITER item = cShape->PointsList->getList().begin(); item != cShape->PointsList->getList().end(); item++)
		{     
			double reflectedPoints[3] = {0}, tmpPoint[3] = {item->second->X, item->second->Y, item->second->Z};
			 RMATH3DOBJECT->ReflectedPointsAlongLine(tmpPoint, 1, reflectionSurfaceParam, reflectedPoints);
			((ShapeWithList*)Rshape)->PointsListOriginal->Addpoint(new SinglePoint(reflectedPoints[0], reflectedPoints[1], reflectedPoints[2]));
			((ShapeWithList*)Rshape)->PointsList->Addpoint(new SinglePoint(reflectedPoints[0], reflectedPoints[1], reflectedPoints[2]));
		}
	}
	((ShapeWithList*)Rshape)->DotheShapeUpdate();
}

void Shape::RotateShape(double *rotateM, double * rotationOrigin)
{
	ShapeWithList *cShape = ((ShapeWithList*)this);
	int pointsSize = cShape->PointsList->getList().size();
	cShape->PointsListOriginal->deleteAll();
	double negPostion[3]  = {-rotationOrigin[0], -rotationOrigin[1], -rotationOrigin[2]};
	for(PC_ITER item = cShape->PointsList->getList().begin(); item != cShape->PointsList->getList().end(); item++)
	{
		SinglePoint* Spt = item->second;
		Spt->Translate(negPostion);
		Spt->Transform(rotateM);
		Spt->Translate(rotationOrigin);
		cShape->PointsListOriginal->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
	}
	cShape->DotheShapeUpdate();
}

void Shape::TranslateShape(double *translateDir)
{
	ShapeWithList *cShape = ((ShapeWithList*)this);
	int pointsSize = cShape->PointsList->getList().size();
	cShape->PointsListOriginal->deleteAll();
	for(PC_ITER item = cShape->PointsList->getList().begin(); item != cShape->PointsList->getList().end(); item++)
	{
		SinglePoint* Spt = item->second;
		Spt->Translate(translateDir);
		/*Spt->SetValues(Spt->X, Spt->Y, Spt->Z);*/
		cShape->PointsListOriginal->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
	}
	cShape->DotheShapeUpdate();
}

void Shape::UpdateHomePostionFromReferenceDotShape(bool FirstReferenceDot)
{
	if(FirstReferenceDot)
	{
		if(this->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		   PPCALCOBJECT->HomePosition.set(*((Sphere*)this)->getCenter());
		else if(this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		   PPCALCOBJECT->HomePosition.set(*((Circle*)this)->getCenter());
		else if(this->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
			PPCALCOBJECT->HomePosition.set(*((RPoint*)this)->getPosition());
	}
	else
	{
		if(this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		   PPCALCOBJECT->HomePosition_2nd.set(*((Circle*)this)->getCenter());
	}
}

void Shape::drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM)
{}

bool Shape::Shape_IsInWindow(Vector& corner1, double Tolerance, double* TransformM)
{
  return false;
}

void Shape::drawShape(int windowno, double WPixelWidth)
{
	try
	{
		if(!IsValid()) return;
		if(IsHidden()) return;
		if((windowno == 0) && (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || 
								MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ ||
								MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm ||
								MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm))
		   return;
		if(this->ShowFGpointsflag())
			((ShapeWithList*)this)->drawFGpoints();
		else
		{  
			GRAFIX->SetGraphicsLineWidth(ShapeThickness());
			if (windowno == 0)
			{
				if((this->Normalshape()) && (MAINDllOBJECT->getCurrentUCS().UCSMode() == 4))
				{
					double TransformM[16] = {0};
					RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 4, 1, TransformM);
					this->drawCurrentShape(windowno, WPixelWidth,TransformM);
				}
				else if((this->Normalshape()) && (MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSXZ))
				{
					double TransformM[16] = {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1};
					this->drawCurrentShape(windowno, WPixelWidth,TransformM);
				}
				else if((this->Normalshape()) && (MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGXZ))
				{
					double TransformM[16] = {-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1};
					this->drawCurrentShape(windowno, WPixelWidth,TransformM);
				}
				else if((this->Normalshape()) && (MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSYZ))
				{
					double TransformM[16] = {0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1};
					this->drawCurrentShape(windowno, WPixelWidth,TransformM);
				}
				else if((this->Normalshape()) && (MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGYZ))
				{
					double TransformM[16] = {0, 0, 1, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1};
					this->drawCurrentShape(windowno, WPixelWidth,TransformM);
				}
				else
					this->drawCurrentShape(windowno, WPixelWidth);
			}
			else
			  this->drawCurrentShape(windowno, WPixelWidth);
			GRAFIX->SetGraphicsLineWidth(1);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0003", __FILE__, __FUNCSIG__); }
}

void Shape::drawSelectedPointCrossMark(int windowno, double WPixelWidth)
{
	try
	{    
		if(!IsHidden())
			((ShapeWithList*)this)->drawPointCrossMark(WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0005", __FILE__, __FUNCSIG__); }
}

double Shape::getProbeRadius(int side)
{
	try
	{
		if(MAINDllOBJECT->CurrentProbeType == RapidEnums::PROBETYPE::SIMPLE_PROBE)
		{
			return MAINDllOBJECT->ProbeRadius();
		}
		else if(MAINDllOBJECT->CurrentProbeType != RapidEnums::PROBETYPE::GENERICPROBE)
		{
			switch(side)
			{
				case 0:
					return MAINDllOBJECT->X_AxisProbeRadiusLeft();
					break;
				case 1:
					return MAINDllOBJECT->X_AxisProbeRadiusRight();
					break;
				case 2:
					return MAINDllOBJECT->Y_AxisProbeRadiusBottom();
					break;
				case 3:
					 return MAINDllOBJECT->Y_AxisProbeRadiusTop();
					 break;
				case 4:
					 return MAINDllOBJECT->X_AxisProbeRadiusStrt();
					 break;
			}
		}
		else
		{
			 return MAINDllOBJECT->GetGenericProbeRadius(side);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0005a", __FILE__, __FUNCSIG__); }
}

std::string Shape::getCurrentShapeTypeName()
{
	try
	{
		std::string CShapeName = "Default";
		if(this->ShapeType == RapidEnums::SHAPETYPE::LINE)
			CShapeName = "Line";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			CShapeName = "RayLine";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::XLINE)
			CShapeName = "Line";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
			CShapeName = "Line 3D";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			CShapeName = "Circle";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::ARC)
			CShapeName = "Arc";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
			CShapeName = "Point";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::RTEXT)
			CShapeName = "Text";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::PERIMETER)
			CShapeName = "Perimeter";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::POLYLINE)
			CShapeName = "Polyline";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE)
			CShapeName = "DepthShape";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
			CShapeName = "Point 3D";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			CShapeName = "CloudPoint";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
			CShapeName = "Sphere";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::PLANE)
			CShapeName = "Plane";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
			CShapeName = "Cylinder";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::CONE)
			CShapeName = "Cone";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
			CShapeName = "Circle 3D";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
			CShapeName = "Arc 3D";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
			CShapeName = "Ellipse";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::PARABOLA3D)
			CShapeName = "Parabola";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::HYPERBOLA3D)
			CShapeName = "Hyperbola";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::DUMMY)
			CShapeName = "Dummy";
		else if(this->ShapeType == RapidEnums::SHAPETYPE::DEFAULTSHAPE)
			CShapeName = "Default";
		return ((char*)CShapeName.c_str());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0006", __FILE__, __FUNCSIG__); return ""; }
}

TCHAR Shape::name[10];

wostream& operator<<(wostream& os, Shape& x)
{
	try
	{
		os << "ShapeType:" << x.ShapeType << endl;
		os << "CommonShapes" <<endl;
		os << "ShapeType:" << x.ShapeType << endl;
		os << "Id:" << x.getId()  << endl;
		os << "IsDxfShape:" << x.IsDxfShape() <<endl;		
		os << "IsValid:" << x.IsValid()  << endl;		
		os << "XOffsetCorrection:" << x.XOffsetCorrection() << endl;
		os << "YOffsetCorrection:" << x.YOffsetCorrection() << endl;
		os << "ZOffsetCorrection:" << x.ZOffsetCorrection() << endl;
		os << "ROffsetCorrection:" << x.ROffsetCorrection() << endl;
		os << "ShapeAsReferenceShape:" << x.ShapeAsReferenceShape() << endl;
		os << "ShapeAsfasttrace:" << x.ShapeAsfasttrace() << endl;
		os << "ShapeForPerimeter:" << x.ShapeForPerimeter() << endl;
		os << "IgnoreManualCrosshairSteps:" << x.IgnoreManualCrosshairSteps() << endl;
		os << "AppliedLineArcSeperation:" << x.AppliedLineArcSeperation() << endl;
		os << "AppliedLineArcSepOnce:" << x.AppliedLineArcSepOnce() << endl;
		os << "ShapeUsedForTwoStepHoming:" << x.ShapeUsedForTwoStepHoming() << endl;
		os << "ShapeAsRotaryEntity:" << x.ShapeAsRotaryEntity() << endl;
		os << "ShapeThickness:" << x.ShapeThickness() << endl;
		os << "ShapeDoneUsingProbe:" << x.ShapeDoneUsingProbe() << endl;
		os << "SigmaMode:" << x.SigmaMode() << endl;
		os << "ApplyMMCCorrection:" << x.ApplyMMCCorrection() << endl;
		os << "ApplyFilterCorrection:" << x.ApplyFilterCorrection() << endl;
		os << "MMCCorrectionIterationCnt:" << x.MMCCorrectionIterationCnt() << endl;
		os << "FilterCorrectionIterationCnt:" << x.FilterCorrectionIterationCnt() << endl;
		os << "FilterCorrectionFactor:" << x.FilterCorrectionFactor() << endl;
		os << "OuterCorrection:" << x.OuterCorrection() << endl;
		os << "ScanDirDetected:" << x.ScanDirDetected() << endl;
		os << "AddActionWhileEdit:" << x.AddActionWhileEdit() << endl;
		os << "ShapeColorR:" << x.ShapeColor.r  << endl <<"ShapeColorG:" << x.ShapeColor.g  << endl <<"ShapeColorB:" << x.ShapeColor.b << endl;

		os << "getParents:" << x.getParents().size() << endl;
		for(list<BaseItem*>::iterator shapeiterator = x.getParents().begin();shapeiterator != x.getParents().end(); shapeiterator++)
			os << "getParentsshapeiterator:" << ((Shape*)(*shapeiterator))->getId() << endl;
	
		os << "getFgNearShapes:" << x.getFgNearShapes().size() << endl;
		for(list<BaseItem*>::iterator shapeiterator = x.getFgNearShapes().begin();shapeiterator != x.getFgNearShapes().end(); shapeiterator++)
			os << "getFgNearShapesshapeiterator:" << ((Shape*)(*shapeiterator))->getId() << endl;

		os << "getucsCopyParent:" << x.getucsCopyParent().size() << endl;
		for(list<BaseItem*>::iterator shapeiterator = x.getucsCopyParent().begin();shapeiterator != x.getucsCopyParent().end(); shapeiterator++)
		{
			os << "getucsCopyParentshapeUcsId:" << ((Shape*)(*shapeiterator))->UcsId() << endl;
			os << "getucsCopyParentshapeiterator:" << ((Shape*)(*shapeiterator))->getId() << endl;
		}

		os << "getSiblings:" << x.getSiblings().size() << endl;
		for(list<BaseItem*>::iterator shapeiterator = x.getSiblings().begin();shapeiterator != x.getSiblings().end(); shapeiterator++)
			os << "getSiblingsshapeiterator:" << ((Shape*)(*shapeiterator))->getId() << endl;
		os << "EndCommonShapes" <<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SH0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Shape& x) 
{
	try
	{
		bool flag; int n, mcsParentucsId = 0;
		double r, g, b;		
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"CommonShapes")
			{
				while(Tagname!=L"EndCommonShapes")
				{	
					std::wstring Linestr;				
					is >> Linestr;
					if(is.eof())
					{						
						MAINDllOBJECT->PPLoadSuccessful(false);
						break;
					}
					int ColonIndx = Linestr.find(':');
					if(ColonIndx==-1)
					{
						Tagname=Linestr;
					}
					else
					{
						std::wstring Tag = Linestr.substr(0, ColonIndx);
						std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname=Tag;					
						if(Tagname==L"Id")
						{
							x.setId(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"IsDxfShape")
						{
							if(Val=="0")
							{
								x.IsDxfShape(false);
							}
							else
								x.IsDxfShape(true);
						}
						else if(Tagname==L"IsValid")
						{
							if(Val=="0")
							{
								x.IsValid(false);
							}
							else
								x.IsValid(true);
						}
						else if(Tagname==L"XOffsetCorrection")
						{
							if(Val=="0")
							{
								x.XOffsetCorrection(false);
							}
							else
								x.XOffsetCorrection(true);
						}
						else if(Tagname==L"YOffsetCorrection")
						{
							if(Val=="0")
							{
								x.YOffsetCorrection(false);
							}
							else
								x.YOffsetCorrection(true);
						}
						else if(Tagname==L"ApplyMMCCorrection")
						{
						   if(Val=="0")
							{
								x.ApplyMMCCorrection(false);
							}
							else
								x.ApplyMMCCorrection(true);
						}
						else if(Tagname==L"ApplyFilterCorrection")
						{
							if(Val=="0")
							{
								x.ApplyFilterCorrection(false);
							}
							else
								x.ApplyFilterCorrection(true);
						}
						else if(Tagname==L"MMCCorrectionIterationCnt")
						{
							x.MMCCorrectionIterationCnt(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"FilterCorrectionIterationCnt")
						{
							x.FilterCorrectionIterationCnt(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"FilterCorrectionFactor")
						{
							x.FilterCorrectionFactor(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"OuterCorrection")
						{
							if(Val=="0")
							{
								x.OuterCorrection(false);
							}
							else
								x.OuterCorrection(true);
						}
						else if(Tagname==L"ScanDirDetected")
						{
							if(Val=="0")
							{
								x.ScanDirDetected(false);
							}
							else
								x.ScanDirDetected(true);
						}
						else if(Tagname==L"ZOffsetCorrection")
						{
							if(Val=="0")
							{
								x.ZOffsetCorrection(false);
							}
							else
								x.ZOffsetCorrection(true);
						}
						else if(Tagname==L"ROffsetCorrection")
						{
							if(Val=="0")
							{
								x.ROffsetCorrection(false);
							}
							else
								x.ROffsetCorrection(true);
						}
						else if(Tagname==L"ShapeAsReferenceShape")
						{
							if(Val=="0")
							{
								x.ShapeAsReferenceShape(false);
							}
							else
								x.ShapeAsReferenceShape(true);
						}
						else if(Tagname==L"ShapeForPerimeter")
						{
							if(Val=="0")
							{
								x.ShapeForPerimeter(false);
							}
							else
								x.ShapeForPerimeter(true);
						}
						else if(Tagname==L"ShapeAsfasttrace")
						{
							if(Val=="0")
							{
								x.ShapeAsfasttrace(false);
							}
							else
								x.ShapeAsfasttrace(true);
						}
						else if(Tagname==L"IgnoreManualCrosshairSteps")
						{
							if(Val=="0")
							{
								x.IgnoreManualCrosshairSteps(false);
							}
							else
								x.IgnoreManualCrosshairSteps(true);
						}
						else if(Tagname==L"AppliedLineArcSeperation")
						{
							if(Val=="0")
							{
								x.AppliedLineArcSeperation(false);
							}
							else
								x.AppliedLineArcSeperation(true);
						}
						else if(Tagname==L"AppliedLineArcSepOnce")
						{
							if(Val=="0")
							{
								x.AppliedLineArcSepOnce(false);
							}
							else
								x.AppliedLineArcSepOnce(true);
						}
						else if(Tagname==L"ShapeUsedForTwoStepHoming")
						{
							if(Val=="0")
							{
								x.ShapeUsedForTwoStepHoming(false);
							}
							else
								x.ShapeUsedForTwoStepHoming(true);
						}
						else if(Tagname==L"ShapeAsRotaryEntity")
						{
							if(Val=="0")
							{
								x.ShapeAsRotaryEntity(false);
							}
							else
								x.ShapeAsRotaryEntity(true);
						}
						else if(Tagname==L"ShapeThickness")
						{
							x.ShapeThickness(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"ShapeDoneUsingProbe")
						{
							if(Val=="0")
							{
								x.ShapeDoneUsingProbe(false);
							}
							else
								x.ShapeDoneUsingProbe(true);
						}
						else if(Tagname==L"SigmaMode")
						{
							if(Val=="0")
								x.SigmaMode(false);
							else
								x.SigmaMode(true);
						}
						else if(Tagname==L"AddActionWhileEdit")
						{
							if(Val=="0")
								x.AddActionWhileEdit(false);
							else
								x.AddActionWhileEdit(true);
						}
						else if(Tagname==L"ShapeColorR")
						{
							r=atof((const char*)(Val).c_str());
						}
						else if(Tagname==L"ShapeColorG")
						{
							g=atof((const char*)(Val).c_str());
						}
						else if(Tagname==L"ShapeColorB")
						{
							b=atof((const char*)(Val).c_str());
						}
						else if(Tagname==L"getGroupParent")
						{
						
						}
						else if(Tagname==L"getGroupParentshapeiterator")
						{
						}
						else if(Tagname==L"getParents")
						{
						}
						else if(Tagname==L"getParentsshapeiterator")
						{
							int j =atoi((const char*)(Val).c_str());
							if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(j))
							{
								x.addParent(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]);
								((Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j])->addChild(&x);
							}
						}
						else if(Tagname==L"getFgNearShapes")
						{
						}
						else if(Tagname==L"getFgNearShapesshapeiterator")
						{
							int j =atoi((const char*)(Val).c_str());
							x.FtFgRelationshapes.push_back(j);
						}
						else if(Tagname==L"getucsCopyParent")
						{
						}
						else if(Tagname==L"getucsCopyParentshapeUcsId")
						{
							 mcsParentucsId = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"getucsCopyParentshapeiterator")
						{
							int j =atoi((const char*)(Val).c_str());
							if(MAINDllOBJECT->getUCSList().ItemExists(mcsParentucsId))
							{
								UCS* Rmcs = (UCS*)(MAINDllOBJECT->getUCSList().getList()[mcsParentucsId]);
								if(Rmcs->getShapes().ItemExists(j))
									x.adducsCopyRelation(Rmcs->getShapes().getList()[j]);
							}
						}
						else if(Tagname==L"getSiblings")
						{
						}
						else if(Tagname==L"getSiblingsshapeiterator")
						{
							int j =atoi((const char*)(Val).c_str());
							x.siblingrelationships.push_back(j);
						}
					 }
				}
				x.ShapeColor.set(r, g, b);
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("SH0008", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, Shape& x)
{
	try
	{
		bool flag; int n;
		double r, g, b;
		char Cc;
		wstring flagStr;
		is >> n; x.setId(n);
		is >> flagStr;
		std::string Str = RMATH2DOBJECT->WStringToString(flagStr);
		if(Str == "IsDxfShape")
		{
			is >> flag;		
			x.IsDxfShape(flag);
			is >> flag; x.IsValid(flag);
		}
		else
		{
			if(Str == "0")
				x.IsValid(false);
			else
				x.IsValid(true);
		}
		is >> flagStr;
		Str = RMATH2DOBJECT->WStringToString(flagStr);
		if(Str == "XOffsetCorrection")
		{
			is >> flag;		
			x.XOffsetCorrection(flag);
			is >> flag; x.YOffsetCorrection(flag);
			is >> flag; x.ZOffsetCorrection(flag);
			is >> flag; x.ShapeAsReferenceShape(flag);
		}
		else
		{
			if(Str == "0")
				x.ShapeAsReferenceShape(false);
			else
				x.ShapeAsReferenceShape(true);
		}
		is >> flag; x.ShapeAsfasttrace(flag);
		is >> flag; x.IgnoreManualCrosshairSteps(flag);
		is >> flag; x.AppliedLineArcSeperation(flag);
		is >> flag; x.AppliedLineArcSepOnce(flag);
		is >> flag; x.ShapeUsedForTwoStepHoming(flag);
		is >> flag; x.ShapeAsRotaryEntity(flag);
		is >> r; x.ShapeThickness(r);
		is >> flag; x.ShapeDoneUsingProbe(flag);
		is >> r >> g >> b;
		x.ShapeColor.set(r, g, b);

		is >> n;
		for(int i = 0; i < n; i++)
		{
			int j;                    // for groupparent
			is >> j;
		}

		is >> n;
		for(int i = 0; i < n; i++)
		{
			int j;
			is >> j;
			x.addParent(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]);
			((Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j])->addChild(&x);
		}

		is >> n;
		for(int i = 0; i < n; i++)
		{
			int j;
			is >> j;
			x.FtFgRelationshapes.push_back(j);
		}

		is >> n;
		for(int i = 0; i < n; i++)
		{
			int j;
			is >> j;
			UCS* Rmcs = (UCS*)(*MAINDllOBJECT->getUCSList().getList().begin()).second;
			x.adducsCopyRelation(Rmcs->getShapes().getList()[j]);
		}

		is >> n;
		for(int i = 0; i < n; i++)
		{
			int j;
			is >> j;
			x.siblingrelationships.push_back(j);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("SH0008", __FILE__, __FUNCSIG__);}
}
