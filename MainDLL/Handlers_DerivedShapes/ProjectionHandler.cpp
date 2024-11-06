#include "stdafx.h"
#include "ProjectionHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\RPoint.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\AddPointAction.h"

//Constructor...//
ProjectionHandler::ProjectionHandler()
{
	init();	
	setMaxClicks(2);
	/*MAINDllOBJECT->SetStatusCode("CircleTangent2TwoLine01");*/
}

//Destructor..
ProjectionHandler::~ProjectionHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROH0001", __FILE__, __FUNCSIG__); }
}

void ProjectionHandler::ResetShapeHighlighted()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROH0002", __FILE__, __FUNCSIG__); }
}

//Initialise flags..counters..
void ProjectionHandler::init()
{
	try
	{
		runningPartprogramValid = false;
		validflag = false;
		resetClicks();
		ParentShape1 = NULL; 
		PointParent1 = NULL;
		PointParent2 = NULL;
		ParentPoint = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROH0003", __FILE__, __FUNCSIG__); }
}

//
void ProjectionHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL && MAINDllOBJECT->highlightedPoint() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if (MAINDllOBJECT->highlightedShape() != NULL)
		{	
			ParentShape1 = MAINDllOBJECT->highlightedShape();			
		} 	
		else
		{
			ParentPoint = MAINDllOBJECT->highlightedPoint();
			if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL)
		    	PointParent1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1();
			if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2() != NULL)
		    	PointParent2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2();	
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROH0004", __FILE__, __FUNCSIG__); }
}

void ProjectionHandler::mouseMove()
{
}

void ProjectionHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(ParentPoint != NULL)
			GRAFIX->drawPoint(ParentPoint->getX(), ParentPoint->getY(), ParentPoint->getZ(), 8);
		if(ParentShape1 != NULL)
		{
			if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D ||
				ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
				GRAFIX->drawPoint(((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(), ((Circle*)ParentShape1)->getCenter()->getZ(), 8);
			//else if (ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CONE) // || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
			//{
			//	double coneAxis[6] = { 0 };
			//	double Angle = RMATH3DOBJECT->Angle_Line_Line(((Cone)ParentShape1).getAxisLine(&coneAxis[0]), ;
			//}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROH0004a", __FILE__, __FUNCSIG__); }
}

void ProjectionHandler::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void ProjectionHandler::SetAnyData(double *data)
{
}


//Increase/decrease the diameter for ouse scroll event....//
void ProjectionHandler::MouseScroll(bool flag)
{
}

//Handle the partprogram....//
void ProjectionHandler::PartProgramData()
{
	CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
	CurrentShape->UpdateBestFit();
	PPCALCOBJECT->partProgramReached();
}

//Max mouse click..//
void ProjectionHandler::LmaxmouseDown()
{
	/*try
	{*/
	//LmouseDown();
	Shape* Csh = MAINDllOBJECT->highlightedShape();	
			if(MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getMaxClicks() - 1);
				
				return;
			}
			//Shape* Csh = MAINDllOBJECT->highlightedShape();
			
			/*if(ParentShape1 != NULL && ParentShape1->getId() == Csh->getId())
			{
			   setClicksDone(getMaxClicks() - 1);			   
			   return;
			}*/
			/*if(ParentPoint != NULL)
			{
				MAINDllOBJECT->SetAndRaiseErrorMessage("LmaxmouseDown3", __FILE__, __FUNCSIG__);
				if(Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE || Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE3D)
			    {
					CurrentShape = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
	        		((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::PROJECTEDPOINT3D;
					CurrentShape->addParent(Csh);
				 	Csh->addChild(CurrentShape);
					if(PointParent1 != NULL)
					{
					    CurrentShape->addParent(PointParent1);
				    	PointParent1->addChild(CurrentShape); 
					}
					if(PointParent2 != NULL)
					{
					    CurrentShape->addParent(PointParent2);
				    	PointParent2->addChild(CurrentShape); 
					}
					((RPoint*)CurrentShape)->ParentPoint = this->ParentPoint;
					((RPoint*)CurrentShape)->UpdateForParentChange(Csh);
					AddShapeAction::addShape(CurrentShape);
					MAINDllOBJECT->Shape_Updated();
			    }
				else if(Csh->ShapeType ==  RapidEnums::SHAPETYPE::PLANE)
				{
					ParentShape1 = Csh;
					CurrentShape = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
	        		((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::PROJECTEDPOINT3D;
					CurrentShape->addParent(Csh);
					Csh->addChild(CurrentShape);
					if(PointParent1 != NULL)
					{
					    CurrentShape->addParent(PointParent1);
				    	PointParent1->addChild(CurrentShape); 
					}
					if(PointParent2 != NULL)
					{
					    CurrentShape->addParent(PointParent2);
				    	PointParent2->addChild(CurrentShape); 
					}
					((RPoint*)CurrentShape)->ParentPoint = this->ParentPoint;
					CurrentShape->UpdateBestFit();
					AddShapeAction::addShape(CurrentShape);
					MAINDllOBJECT->Shape_Updated();
				}
				else
				{
			    	setClicksDone(getMaxClicks() - 1);
			        return;
				}
			}*/
			else 
			{
				if((ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE3D) && Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					
				  	CurrentShape = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
	        		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::PROJECTIONLINE3D;
					if(Csh->ShapeType != RapidEnums::SHAPETYPE::PLANE)
					{
					  Shape *tmp = Csh;
					  Csh = ParentShape1;
					  ParentShape1 = tmp;
					}
					CurrentShape->addParent(ParentShape1);
					CurrentShape->addParent(Csh);
					ParentShape1->addChild(CurrentShape);
					Csh->addChild(CurrentShape);
					CurrentShape->UpdateBestFit();
					AddShapeAction::addShape(CurrentShape);
					MAINDllOBJECT->Shape_Updated();
			    }
				else if((ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC3D
					|| ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D) && Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					
					//CurrentShape = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
	        		//((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::PROJECTEDPOINT3D;
					CurrentShape = new Circle(_T("C3D"), RapidEnums::SHAPETYPE::CIRCLE3D);
					((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::PROJECTEDCIRCLE;
					if (Csh->ShapeType != RapidEnums::SHAPETYPE::PLANE)
					{
						Shape* tmp = Csh;
						Csh = ParentShape1;
						ParentShape1 = tmp;
					}
					CurrentShape->addParent(ParentShape1);
					ParentShape1->addChild(CurrentShape);
					CurrentShape->addParent(Csh);
					Csh->addChild(CurrentShape);
					CurrentShape->UpdateBestFit();
					AddShapeAction::addShape(CurrentShape);
					MAINDllOBJECT->Shape_Updated();
				}
				else if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::INTERSECTIONSHAPE && Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					
					CurrentShape = new CloudPoints();
	        		((CloudPoints*)CurrentShape)->CloudPointsType = RapidEnums::CLOUDPOINTSTYPE::INTERSECTIONSHAPEPROJECTION;
					if(Csh->ShapeType != RapidEnums::SHAPETYPE::PLANE)
					{
					  Shape *tmp = Csh;
					  Csh = ParentShape1;
					  ParentShape1 = tmp;
					}
					CurrentShape->addParent(ParentShape1);
					CurrentShape->addParent(Csh);
					ParentShape1->addChild(CurrentShape);
					Csh->addChild(CurrentShape);
					CurrentShape->UpdateBestFit();
					AddShapeAction::addShape(CurrentShape);
					setBaseRProperty(0, 0, 0);
	     			baseaction->DroMovementFlag = false;
	    			AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
					MAINDllOBJECT->Shape_Updated();
			    }
				else if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS && Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					
					CurrentShape = new CloudPoints();
	        		((CloudPoints*)CurrentShape)->CloudPointsType = RapidEnums::CLOUDPOINTSTYPE::CLOUDPOINTSPROJECTION;
					if(Csh->ShapeType != RapidEnums::SHAPETYPE::PLANE)
					{
					  Shape *tmp = Csh;
					  Csh = ParentShape1;
					  ParentShape1 = tmp;
					}
					CurrentShape->addParent(ParentShape1);
					CurrentShape->addParent(Csh);
					ParentShape1->addChild(CurrentShape);
					Csh->addChild(CurrentShape);
					AddShapeAction::addShape(CurrentShape);
					MAINDllOBJECT->Shape_Updated();
			    }
				else if (ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CONE && Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					
					//CurrentShape = new Cone(_T("C3D"), RapidEnums::SHAPETYPE::CONE);
					CurrentShape = new Circle(_T("C3D"), RapidEnums::SHAPETYPE::CIRCLE3D);
					((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER;
					if (Csh->ShapeType != RapidEnums::SHAPETYPE::PLANE)
					{
						Shape *tmp = Csh;
						Csh = ParentShape1;
						ParentShape1 = tmp;
					}
					CurrentShape->addParent(ParentShape1);
					CurrentShape->addParent(Csh);
					ParentShape1->addChild(CurrentShape);
					Csh->addChild(CurrentShape);
					CurrentShape->UpdateBestFit();
					AddShapeAction::addShape(CurrentShape);
					MAINDllOBJECT->Shape_Updated();
					
				}
				else
				{
				   setClicksDone(getMaxClicks() - 1); 
				   //return;
				}
			}
			init();
	//}
	//catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROH0006", __FILE__, __FUNCSIG__); }
}