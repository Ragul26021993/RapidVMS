#include "stdafx.h"
#include "SnapPtManager.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "RCadApp.h"
#include "..\Helper\General.h"
#include "math.h"
#include "IsectPt.h"
#include "SnapPointCollection.h"
#include "RCollectionBase.h"
#include "..\DXF\DXFExpose.h"
#define _USE_MATH_DEFINES
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
SnapPtManager::SnapPtManager(RCollectionBase* shapes)
{
	try
	{
		this->CurrentUCSShapeList = shapes;
		this->CurrentUCSShapeList->addListener(this);
		Parent2 = Parent1 = NULL;
		addedshape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
SnapPtManager::~SnapPtManager()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0002", __FILE__, __FUNCSIG__); }
}



//Itemadded event .. calculate the intersection points...//
void SnapPtManager::itemAdded(BaseItem *item, Listenable* sender)
{
	try
	{
		//return;
		if(DXFEXPOSEOBJECT->LoadDXF) return;
		addedshape = (Shape*)item;
		if(((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::PLANE || ((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS || ((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::DUMMY || ((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE || ((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::SPHERE || ((Shape*)item)->ShapeType == RapidEnums::SHAPETYPE::CYLINDER) return;
		calcIntPts(item);	
		if(addedshape->IsValid())
			addedshape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0003", __FILE__, __FUNCSIG__); }
}

//Item changed event... Update the intersection points..//
void SnapPtManager::itemChanged(BaseItem *baseitem, Listenable* sender)
{
	try
	{
		//return;
		if (PPCALCOBJECT->PPDontUpdateSnapPts()) return;
		if(DXFEXPOSEOBJECT->LoadDXF) return;
		if(!PPCALCOBJECT->IsPartProgramRunning() && addedshape != NULL && addedshape->getId() == baseitem->getId() && ((Shape*)baseitem)->ShapeType != RapidEnums::SHAPETYPE::DUMMY)
		{
			calcIntPts(baseitem);
			addedshape = NULL;
			return;
		}	
		ShapeWithList* s = (ShapeWithList*)baseitem;
		if(!s->IsValid()) 
			return;
		if(s->ShapeType == RapidEnums::SHAPETYPE::PLANE || s->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS || s->ShapeType == RapidEnums::SHAPETYPE::DUMMY || s->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE || s->ShapeType == RapidEnums::SHAPETYPE::SPHERE || s->ShapeType == RapidEnums::SHAPETYPE::CYLINDER) return;
		//Iterating through all the shapes in the current UCS
		RCollectionBase& shapes = *(this->CurrentUCSShapeList);
		//Calculate intesection point with the other shapes...//
		for(RC_ITER i = shapes.getList().begin(); i != shapes.getList().end(); i++)
		{
			if(!((ShapeWithList*)(*i).second)->IsValid()) continue;
			if (baseitem != (*i).second && ((ShapeWithList*)(*i).second)->ShapeType != RapidEnums::SHAPETYPE::RPOINT) //avoiding computing intersection point with itself
			{

				createIntPts((ShapeWithList*)baseitem, (ShapeWithList*)(*i).second);
				UpdateIntersectionWithshapes((ShapeWithList*)baseitem, (ShapeWithList*)(*i).second);
			}
		}
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0004", __FILE__, __FUNCSIG__); }
}

//handle the itemremoved... Erase the snap points...//
void SnapPtManager::itemRemoved(BaseItem *item, Listenable* sender)
{
	try
	{
		ShapeWithList* s = (ShapeWithList*)item;
		for(ISN_ITER i = s->IsectPointsList->getList().begin(); i != s->IsectPointsList->getList().end(); i++)
		{
			IsectPt* Ipt = (IsectPt*)((*i).second);
			IsectPtList.ErasePoint(Ipt->IsnPtId);
			if(Ipt->getParent1()->getId() != s->getId())
				((ShapeWithList*)Ipt->getParent1())->IsectPointsList->ErasePoint(Ipt->IsnPtId);
			if(Ipt->getParent2()->getId() != s->getId())
				((ShapeWithList*)Ipt->getParent2())->IsectPointsList->ErasePoint(Ipt->IsnPtId);
		}
		s->IsectPointsList->deleteAll();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0005", __FILE__, __FUNCSIG__); }
}

void SnapPtManager::selectionChanged(Listenable* sender)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0006", __FILE__, __FUNCSIG__); }
}



//Calculate the intersection point with the other shape.. 1st time...//
//Add the intersection points to the snap point collection..//
void SnapPtManager::calcIntPts(BaseItem *item)
{
	try
	{
		ShapeWithList* s = dynamic_cast<ShapeWithList*>(item);
		if(s->ShapeType == RapidEnums::SHAPETYPE::RPOINT || 
			s->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS || s->ShapeType == RapidEnums::SHAPETYPE::DUMMY || 
			s->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE || 
			s->ShapeType == RapidEnums::SHAPETYPE::SPHERE  
			) return;
			//s->ShapeType == RapidEnums::SHAPETYPE::PLANE || 
			// || s->ShapeType == RapidEnums::SHAPETYPE::CYLINDER
	
		//Iterating through all the shapes in the current UCS
		RCollectionBase& shapes = *(this->CurrentUCSShapeList);
		
		//Calculate the intersection point of shape with other shapes...//
		for(RC_ITER i = shapes.getList().begin(); i != shapes.getList().end(); i++)
		{
			if(!((ShapeWithList*)(*i).second)->IsValid()) continue;
			if (item != (*i).second && ((ShapeWithList*)(*i).second)->ShapeType != RapidEnums::SHAPETYPE::RPOINT) //avoiding computing intersection point with itself
			{
				createIntPts((ShapeWithList*)item, (ShapeWithList*)(*i).second);
				CopyPoints((ShapeWithList*)item, (ShapeWithList*)(*i).second);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0007", __FILE__, __FUNCSIG__); }
}

//Calculates the intersection
void SnapPtManager::createIntPts(ShapeWithList *s1, ShapeWithList *s2)
{
	try
	{
		SCount[0] = 0; SCount[1] = 0;
		for(int i = 0; i < 4; i++)
		{
			ans[i] = 0; ans1[i] = 0;
		}
		//ensure both are well formed and have enough points
		if (!s1->IsValid() || !s2->IsValid())return;

		//just take the two shapes, find out the number of intersection points 
		//if there are any. then create the point and attach the shapes.
		//***************************************************************************************//
		//***********************************SAME SHAPES****************************************//
		//**************************************************************************************//

		if (compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::XLINE))
				SCount[0] = RMATH2DOBJECT->Line_lineintersection(((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &ans[0]);
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, 
			RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::LINE))
		{
			double p1[2] = {((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY()};
			double p2[2] = {((Line*)s1)->getPoint2()->getX(), ((Line*)s1)->getPoint2()->getY()};
			double p3[2] = {((Line*)s2)->getPoint1()->getX(), ((Line*)s2)->getPoint1()->getY()};
			double p4[2] = {((Line*)s2)->getPoint2()->getX(), ((Line*)s2)->getPoint2()->getY()};
			RMATH2DOBJECT->Finiteline_line(&p1[0], &p2[0], &p3[0], &p4[0],((Line*)s1)->Angle(), ((Line*)s1)->Intercept(),((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &ans[0],&ans1[0], &SCount[0]);
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::ARC))
		{
			double p1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
			double p2[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
			RMATH2DOBJECT->arc_arcintersection(&p1[0],((Circle*)s1)->Radius(), ((Circle*)s1)->Startangle(), ((Circle*)s1)->Sweepangle(), &p2[0],((Circle*)s2)->Radius(),((Circle*)s2)->Startangle(), ((Circle*)s2)->Sweepangle(), &ans[0], &ans1[0],&SCount[0]);
		}

		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::CIRCLE))
		{	
			double c1c[2] = {((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY()};
			double c2c[2] = {((Circle*)s2)->getCenter()->getX(), ((Circle*)s2)->getCenter()->getY()};
			SCount[0] = RMATH2DOBJECT->Circle_circleintersection(&c1c[0], &c2c[0], ((Circle*)s1)->Radius(),  ((Circle*)s2)->Radius(), &ans[0]);
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::XRAY))
		{
			double	a1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()},
					a2[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()};
			RMATH2DOBJECT->ray_rayintersec(&a1[0], ((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &a2[0], ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(),&ans[0], &ans1[0], &SCount[0]);
		}
		//*************************************************************************************//
		//*********************XLINE WITH OTHER DIFFERENT SHAPES.******************************//
		//************************************************************************************//
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::LINE))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double	c1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()},
						c2[2] = {((Line*)s1)->getPoint2()->getX(), ((Line*)s1)->getPoint2()->getY()};
				RMATH2DOBJECT->Line_finiteline(((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &c1[0], &c2[0],((Line*)s1)->Angle(), ((Line*)s1)->Intercept(),&ans[0],&ans1[0], &SCount[0]);
			}
			else
			{
				double	c1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()},
						c2[2] = {((Line*)s2)->getPoint2()->getX(), ((Line*)s2)->getPoint2()->getY()};
				RMATH2DOBJECT->Line_finiteline(((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &c1[0], &c2[0],((Line*)s2)->Angle(), ((Line*)s2)->Intercept(),&ans[0],&ans1[0], &SCount[0]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::XRAY))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{
				double	c1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()};
				RMATH2DOBJECT->raylineint(((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &c1[0], ((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &ans[0], &ans1[0],&SCount[0]);
			}
			else
			{
				double	c1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()};
				RMATH2DOBJECT->raylineint(((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &c1[0], ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &ans[0], &ans1[0],&SCount[0]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, 
			RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::ARC))
		{
			 if (s1->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				double	a2[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
				RMATH2DOBJECT->Line_arcinter(((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &a2[0],((Circle*)s1)->Radius() , ((Circle*)s1)->Startangle(),((Circle*)s1)->Sweepangle(), &ans[0], &ans1[0], &SCount[0]);
			}
			else
			{
				double	a2[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
				RMATH2DOBJECT->Line_arcinter(((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &a2[0],((Circle*)s2)->Radius() , ((Circle*)s2)->Startangle(),((Circle*)s2)->Sweepangle(), &ans[0], &ans1[0], &SCount[0]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::CIRCLE))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				double	p1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
				SCount[0] = RMATH2DOBJECT->Line_circleintersection(((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &p1[0], ((Circle*)s1)->Radius(), &ans[0]);
			}
			else
			{
				double	p1[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
				SCount[0] = RMATH2DOBJECT->Line_circleintersection(((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &p1[0], ((Circle*)s2)->Radius(), &ans[0]);
			}
		}
		//******************************************************************************************************//
		//*********************************LINE WITH OTHER DIFFERENT SHAPES*************************************//
		//****************************************EXCEPT XLINE*************************************************//
		//******************************************************************************************************//
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::XRAY))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{
				double	a1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()},
						b1[2] = {((Line*)s2)->getPoint2()->getX(),((Line*)s2)->getPoint2()->getY()},
						a2[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()};
				RMATH2DOBJECT->ray_finitelineint(&a1[0], &b1[0], ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &a2[0],((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &ans[0], &ans1[0],&SCount[0]);
			}
			else
			{
				double	a1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()},
						b1[2] = {((Line*)s1)->getPoint2()->getX(),((Line*)s1)->getPoint2()->getY()},
						a2[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()};
				RMATH2DOBJECT->ray_finitelineint(&a1[0], &b1[0],((Line*)s1)->Angle(), ((Line*)s1)->Intercept(),  &a2[0],((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &ans[0], &ans1[0],&SCount[0]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, 
			RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::LINE))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				double	a1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()},
						b1[2] = {((Line*)s2)->getPoint2()->getX(),((Line*)s2)->getPoint2()->getY()},
						a2[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
				RMATH2DOBJECT->Finiteline_circle(&a1[0],&b1[0], ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(),  &a2[0], ((Circle*)s1)->Radius(), &ans[0], &ans1[0], &SCount[0]);
			}
			else
			{
				double	a1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()},
						b1[2] = {((Line*)s1)->getPoint2()->getX(),((Line*)s1)->getPoint2()->getY()},
						a2[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
				RMATH2DOBJECT->Finiteline_circle(&a1[0], &b1[0],((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &a2[0], ((Circle*)s2)->Radius(), &ans[0], &ans1[0], &SCount[0]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, 
			RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::LINE))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				double	a1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()},
						b1[2] = {((Line*)s2)->getPoint2()->getX(),((Line*)s2)->getPoint2()->getY()},
						a2[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
				RMATH2DOBJECT->Finiteline_arcinter(&a1[0], &b1[0], ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &a2[0], ((Circle*)s1)->Radius(), ((Circle*)s1)->Startangle(), ((Circle*)s1)->Sweepangle(), &ans[0], &ans1[0],&SCount[0]);
			}
			else
			{
				double	a1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()},
						b1[2] = {((Line*)s1)->getPoint2()->getX(),((Line*)s1)->getPoint2()->getY()},
						a2[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
				RMATH2DOBJECT->Finiteline_arcinter(&a1[0], &b1[0], ((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &a2[0], ((Circle*)s2)->Radius(), ((Circle*)s2)->Startangle(), ((Circle*)s2)->Sweepangle(), &ans[0], &ans1[0],&SCount[0]);
			}
		}
		//**********************************************************************************************//
		//*********************************XRAY WITH DIFFERENT SHAPES**********************************//
		//*********************************EXCEPT XLINE AND LINE***************************************//
		//*********************************************************************************************//
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::CIRCLE))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				double	e1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()},
						a1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
				RMATH2DOBJECT->ray_circleint(&a1[0], ((Circle*)s1)->Radius(), &e1[0], ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &ans[0], &ans1[0],&SCount[0]);
			}
			else
			{
				double	e1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()},
						a1[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
				RMATH2DOBJECT->ray_circleint(&a1[0], ((Circle*)s2)->Radius(), &e1[0], ((Line*)s1)->Angle(),  ((Line*)s1)->Intercept(), &ans[0], &ans1[0],&SCount[0]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::ARC))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				double	e1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()},
						a1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
				RMATH2DOBJECT->ray_arcinter(&e1[0], ((Line*)s2)->Angle(), ((Line*)s2)->Intercept(), &a1[0], ((Circle*)s1)->Radius(), ((Circle*)s1)->Startangle(), ((Circle*)s1)->Sweepangle(), &ans[0], &ans1[0], &SCount[0]);
			}
			else
			{
				double	e1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()},
						a1[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
				RMATH2DOBJECT->ray_arcinter(&e1[0], ((Line*)s1)->Angle(), ((Line*)s1)->Intercept(), &a1[0], ((Circle*)s2)->Radius(), ((Circle*)s2)->Startangle(), ((Circle*)s2)->Sweepangle(), &ans[0], &ans1[0], &SCount[0]);
			}
		}
		//************************************************************************************************************//
		//*****************************************CIRCLE TO ARC INTERSECTION*****************************************//
		//************************************************************************************************************//
		
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType,
			RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::ARC))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				double	e1[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()},
						a1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
				RMATH2DOBJECT->Circle_arcintersection(&e1[0], ((Circle*)s2)->Radius(), &a1[0], ((Circle*)s1)->Radius(), ((Circle*)s1)->Startangle(), ((Circle*)s1)->Sweepangle(), &ans[0], &ans1[0], &SCount[0]);
			}
			else
			{
				double	e1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()},
						a1[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
				RMATH2DOBJECT->Circle_arcintersection(&e1[0], ((Circle*)s1)->Radius(), &a1[0], ((Circle*)s2)->Radius(), ((Circle*)s2)->Startangle(), ((Circle*)s2)->Sweepangle(), &ans[0],&ans1[0],&SCount[0]);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0008", __FILE__, __FUNCSIG__); }
}



IsectPtCollection* SnapPtManager::getIsectPts()
{
	return &this->IsectPtList;
}

//Retuns the current intersection parents....//
Shape* SnapPtManager::getParent1()
{
	return Parent1;
}

Shape* SnapPtManager::getParent2()
{
	return Parent2;
}

void SnapPtManager::SetParents(Shape* s1, Shape* s2)
{
	Parent1 = s1;
	Parent2 = s2;
}

//According to the intersetion point count create the intersection point object and set the parameters....//
void SnapPtManager::CopyPoints( ShapeWithList* s1, ShapeWithList* s2)
{
	try
	{
		if (SCount[0] > 0)
		{
			for (int n = 0; n < SCount[0]; n++)
			{
				IsectPt* ispt = new IsectPt(s1, s2);
				if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
					ispt->set(ans[n*2], ans[n*2 + 1], MAINDllOBJECT->getCurrentDRO().getZ());
				else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
					ispt->set(ans[n*2], ans[n*2 + 1], MAINDllOBJECT->getCurrentDRO().getX());
				else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
					ispt->set(ans[n*2], ans[n*2 + 1], MAINDllOBJECT->getCurrentDRO().getY());
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
					ispt->set(ans[n * 2], -ans[n * 2 + 1], MAINDllOBJECT->getCurrentDRO().getX());
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
					ispt->set(ans[n * 2], -ans[n * 2 + 1], MAINDllOBJECT->getCurrentDRO().getY());
				getIsectPts()->Addpoint(ispt);
			}
		}
		if (SCount[1] > 0)
		{
			for (int n = 0; n < SCount[1]; n++)
			{
				IsectPt* ispt = new IsectPt(s1, s2);
				if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
					ispt->set(ans1[n*2], ans1[n*2 + 1],MAINDllOBJECT->getCurrentDRO().getZ());
				else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
					ispt->set(ans1[n*2], ans1[n*2 + 1],MAINDllOBJECT->getCurrentDRO().getX());
				else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
					ispt->set(ans1[n*2], ans1[n*2 + 1],MAINDllOBJECT->getCurrentDRO().getY());
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
					ispt->set(ans1[n * 2], -ans1[n * 2 + 1], MAINDllOBJECT->getCurrentDRO().getX());
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
					ispt->set(ans1[n * 2], -ans1[n * 2 + 1], MAINDllOBJECT->getCurrentDRO().getY());
				ispt->SetIntersectionType(true);
				getIsectPts()->Addpoint(ispt);
			}
		}	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0018", __FILE__, __FUNCSIG__); }
}

//Added on 03/05/10... to update the intersection point with the shapes when the shape is updated...
//Update/ create new / remove....///Go through the all child points of the shape... 
//Compare the parents of new intersection point and old intersection points child points
//If matches.. Update the point, else add the new point... if not exits... remove the point...
void SnapPtManager::UpdateIntersectionWithshapes(ShapeWithList* s1, ShapeWithList* s2)
{
	try
	{
		ISN_ITER item = s1->IsectPointsList->getList().begin(); 
		bool flag = false, flag1 = false, flag2 = false;
		int Ccnt = 0;
		while (item != s1->IsectPointsList->getList().end())
		{
			Ccnt++;
			//pass the key of each item to erase it
			IsectPt* i = (IsectPt*)(*item).second;
			if(i->getParent1() != NULL && i->getParent2() != NULL)
			{
				if(compareboth<int>(i->getParent1()->getId(), i->getParent2()->getId(),s1->getId(), s2->getId()))
				{
					if(SCount[0] == 0 && SCount[1] == 0)
					{
						int id = i->IsnPtId;
						getIsectPts()->ErasePoint(id);
						PPCALCOBJECT->IntersectionPointMissed(true);
						s1->IsectPointsList->ErasePoint(id);
						s2->IsectPointsList->ErasePoint(id);
						RCIT it  =  i->getRelatedDim().begin();
						while(it != i->getRelatedDim().end())
						{
							DimBase* dim = (DimBase*)(*it);
							MAINDllOBJECT->getDimList().removeItem(dim);
							//s1->getMchild().remove(dim);
							it++;
						}
						i->getRelatedDim().clear();
						MAINDllOBJECT->Measurement_updated();
						item = s1->IsectPointsList->getList().begin();
						flag = true;
					}
					else if(SCount[0] > 0 || SCount[1] > 0)
					{
						if(SCount[0] > 0)
						{
							i->set(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
							i->SetIntersectionType(false);
							flag2 = true;
						}
						if(SCount[1] > 0)
						{
							if(!flag2)
							{
								i->set(ans1[0], ans1[1], MAINDllOBJECT->getCurrentDRO().getZ());
								i->SetIntersectionType(true);
							}
							else
							{
								bool secondptt = false;
								item++;
								while (item != s1->IsectPointsList->getList().end())
								{
									IsectPt* i = (IsectPt*)(*item).second;
									if(i->getParent1() != NULL && i->getParent2() != NULL)
									{
										if(compareboth<int>(i->getParent1()->getId(), i->getParent2()->getId(),s1->getId(), s2->getId()))
										{
											i->set(ans1[0], ans1[1], MAINDllOBJECT->getCurrentDRO().getZ());
											i->SetIntersectionType(true);
											secondptt = true;
										}
									}
									item++;
								}
								if(item == s1->IsectPointsList->getList().end() && !secondptt)
								{
									IsectPt* ispt = new IsectPt(s1, s2);
									ispt->set(ans1[0], ans1[1], MAINDllOBJECT->getCurrentDRO().getZ());
									ispt->SetIntersectionType(true);
									getIsectPts()->Addpoint(ispt);
								}
								flag2 = false;
							}
						}
						flag1 = true;
					}
					if(SCount[0] > 1 || SCount[1] > 1)
					{
						bool secondpt = false;
						item++;				
						while (item != s1->IsectPointsList->getList().end())
						{
							IsectPt* i = (IsectPt*)(*item).second;
							if(i->getParent1() != NULL && i->getParent2() != NULL)
							{
								if(compareboth<int>(i->getParent1()->getId(), i->getParent2()->getId(),s1->getId(), s2->getId()))
								{
									if(SCount[0] > 1)
									{
										i->set(ans[2], ans[3], MAINDllOBJECT->getCurrentDRO().getZ());
										i->SetIntersectionType(false);
									}
									else
									{
										i->set(ans1[2], ans1[3], MAINDllOBJECT->getCurrentDRO().getZ());
										i->SetIntersectionType(true);
									}
									secondpt = true;
								}
							}
							item++;
						}
						if(item == s1->IsectPointsList->getList().end() && !secondpt)
						{
							IsectPt* ispt = new IsectPt(s1, s2);
							if(SCount[0] > 1)
							{
								ispt->set(ans[2], ans[3], MAINDllOBJECT->getCurrentDRO().getZ());
								ispt->SetIntersectionType(false);
								getIsectPts()->Addpoint(ispt);
							}
							else
							{
								ispt->set(ans1[2], ans1[3], MAINDllOBJECT->getCurrentDRO().getZ());
								ispt->SetIntersectionType(true);
								getIsectPts()->Addpoint(ispt);
							}
						}
					
					}
					else if(flag1)
					{
						if(item != s1->IsectPointsList->getList().end())
						{
							item++;
							if(item != s1->IsectPointsList->getList().end())
							{
								IsectPt* ii = (IsectPt*)(*item).second;
								if(ii->getParent1() != NULL && ii->getParent2() != NULL)
								{
									if((compareboth<int>(ii->getParent1()->getId(), ii->getParent2()->getId(),s1->getId(), s2->getId())))
									{
										int id = (*item).second->IsnPtId;
										getIsectPts()->ErasePoint(id);
										PPCALCOBJECT->IntersectionPointMissed(true);
										s1->IsectPointsList->getList().erase(id);
										s2->IsectPointsList->getList().erase(id);
										RCIT it  =  ii->getRelatedDim().begin();
										while(it != ii->getRelatedDim().end())
										{
											DimBase* dim = (DimBase*)(*it);
											MAINDllOBJECT->getDimList().removeItem(dim);
											//s1->getMchild().remove(dim);
											it++;
										}
										ii->getRelatedDim().clear();
										MAINDllOBJECT->Measurement_updated();
										item = s1->IsectPointsList->getList().begin();
										item++;
									}
								}
							}
						}
					}
					else
					{
						if(!flag && item != s1->IsectPointsList->getList().end())
							item++;
						flag = false;
					}
				}
				else 
					item++;
			}
			else
				item++;
		}
		if(s1->IsectPointsList->getList().size() == 0 || !flag1)
			CopyPoints(s1, s2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0019", __FILE__, __FUNCSIG__); }
}



//Calculates the nearest point to the mouse point... if the distance is less than the snapdistance then return the snap point
Vector* SnapPtManager::getNearestPoint(double x, double y, double snapdistance, double z, bool considerz)
{
	try
	{
		Vector* CPt1 = NULL;
		Vector* CPt_tmp = NULL;
		Vector* CPt2 = NULL;
		bool found_intpt = false;
		int prev_parent1_id = -1;
		int prev_parent2_id = -1;
		//compare the mouse point with all intersection points..//
		//get the intersection point with minimum shape id's - ie the first of the intersection points.
		for(ISN_ITER item = getIsectPts()->getList().begin(); item != getIsectPts()->getList().end(); item++)
		{
			CPt_tmp = (Vector*)item->second;
			if(abs(CPt_tmp->getX() - x) <= snapdistance && abs(CPt_tmp->getY() - y) <= snapdistance)
			{
				found_intpt = true;
				if (prev_parent1_id == -1)
				{
					Parent1 = item->second->getParent1();
					Parent2 = item->second->getParent2();
					prev_parent1_id = Parent1->getId();
					prev_parent2_id = Parent2->getId();
					CPt1 = CPt_tmp;
				}

				if (((item->second->getParent1()->getId() < prev_parent1_id) && (item->second->getParent2()->getId() < prev_parent2_id)) ||
					((item->second->getParent1()->getId() < prev_parent2_id) && (item->second->getParent2()->getId() < prev_parent1_id)))
				{
					Parent1 = item->second->getParent1();
					Parent2 = item->second->getParent2();
					prev_parent1_id = Parent1->getId();
					prev_parent2_id = Parent2->getId();
					CPt1 = CPt_tmp;
				}
			}
		}
		if (found_intpt == false)
			CPt1 = NULL;
		bool parents_related = false;
		//check if the parents are related to each other.  If yes, prioritize them lower than the snap points
		if (CPt1 != NULL)
		{
			for each(Shape* Csh in Parent1->getParents())
			{
				if (Csh->getId() == Parent2->getId())
				{
					parents_related = true;
					break;
				}
			}
			if (parents_related == false)
			{
				for each(Shape* Csh in Parent2->getParents())
				{
					if (Csh->getId() == Parent1->getId())
					{
						parents_related = true;
						break;
					}
				}
			}
		}
		Shape* csh1 = Parent1;
		Shape* csh2 = Parent2;
		//snap points from shape collection	
		CPt2 = getNearestSnapPoint(*CurrentUCSShapeList, x, y, snapdistance, false, z, considerz);
		if ((CPt2 == NULL) && (CPt1 != NULL))
		{
			Parent1 = csh1;
			Parent2 = csh2;
			return CPt1;
		}
		else
		{
			if ((CPt1 != NULL) && (CPt2 != NULL))
			{
				//if parents are not related, then if one of the parents is same as parent of snap point, choose intersection point over snap point.
				//if (((Parent1 == csh1) || (Parent1 == csh2)) && (parents_related == false))
				//{
				//	Parent1 = csh1;
				//	Parent2 = csh2;
				//	return CPt1;
				//}
				//if parent of snap point is having id greater than parent of intersection points, choose intersection point over snap point.
				if ((Parent1->getId() > csh1->getId()) && (Parent1->getId() > csh2->getId()))
				{
					Parent1 = csh1;
					Parent2 = csh2;
					return CPt1;
				}
				//in other cases choose snap point
			}
			return CPt2;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0009", __FILE__, __FUNCSIG__); return NULL;}
}


Vector* SnapPtManager::getNearestSnapPoint(std::list<Shape*> shapes, double x, double y, double snapdistance)
{
	try
	{
		Vector* CPt = NULL;
		Vector* CPt_tmp = NULL;
		int prev_parent_id = -1;
		bool found_snappt = false;
		for each(Shape* CShape in shapes)
		{
			if(!CShape->IsValid() && CShape->ShapeType != RapidEnums::SHAPETYPE::PERIMETER) continue;
			if(!CShape->Normalshape()) continue;
			SnapPointCollection* Spcoll = ((ShapeWithList*)CShape)->SnapPointList;
			for(SPC_ITER item = Spcoll->getList().begin(); item != Spcoll->getList().end(); item++)
			{
				CPt_tmp = (*item).second->getPt();
				if(abs(CPt_tmp->getX() - x) <= snapdistance && abs(CPt_tmp->getY() - y) <= snapdistance)
				{
					Parent2 = NULL;
					found_snappt = true;
					if (prev_parent_id == -1)
					{
						Parent1 = item->second->getParent();
						prev_parent_id = Parent1->getId();
						CPt = CPt_tmp;
					}

					if (item->second->getParent()->getId() < prev_parent_id) 
					{
						Parent1 = item->second->getParent();
						prev_parent_id = Parent1->getId();
						CPt = CPt_tmp;
					}
				}
			}
		}
		if (found_snappt)
			return CPt;
		else
			return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0015", __FILE__, __FUNCSIG__); return NULL; }
} 


//Calculates the nearest point to the mouse point... if the distance is less than the snapdistance then return the snap point
Vector* SnapPtManager::getNearestSnapPoint(RCollectionBase& shapes, double x, double y, double snapdistance, bool ShapePasted, double z, bool considerz)
{ 
	try
	{
		bool ShowExplicitIntersectionPTsBool = MAINDllOBJECT->ShowExplicitIntersectionPoints();
		bool IsPPRBool = PPCALCOBJECT->IsPartProgramRunning();
		if( ShowExplicitIntersectionPTsBool || IsPPRBool)
		{
			AllSnapPoints.clear();
			Vector* CPt = NULL;
			int prev_parent_id = -1;
			bool found_snappt = false;
			for(RC_ITER Sitem = shapes.getList().begin(); Sitem != shapes.getList().end(); Sitem++)
			{
				Shape* CShape = (Shape*)Sitem->second;
				if(!CShape->Normalshape()) continue;
				if(!CShape->IsValid() && CShape->ShapeType != RapidEnums::SHAPETYPE::PERIMETER) continue;
				if(ShapePasted && CShape->selected()) continue;
				SnapPointCollection* Spcoll = ((ShapeWithList*)CShape)->SnapPointList;
				for(SPC_ITER item = Spcoll->getList().begin(); item != Spcoll->getList().end(); item++)
				{
					if(!item->second->IsValid)continue;
					Vector* CPt_tmp = NULL;
					CPt_tmp = item->second->getPt();
					if(abs(CPt_tmp->getX() - x) <= snapdistance && abs(CPt_tmp->getY() - y) <= snapdistance)
					{
						if(!considerz || abs(CPt_tmp->getZ() - z) <= snapdistance)
						{
							found_snappt = true;
							Parent2 = NULL;
							SnapPtId = item->second->SPtId;
							if (prev_parent_id == -1)
							{
								Parent1 = item->second->getParent();
								prev_parent_id = Parent1->getId();
								CPt = CPt_tmp;
							}

							if (item->second->getParent()->getId() < prev_parent_id) 
							{
								Parent1 = item->second->getParent();
								prev_parent_id = Parent1->getId();
								CPt = CPt_tmp;
							}
						}
					}
					/*if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER_ENDPT)
					{
						if(abs(CPt_tmp->getX() - x) <= snapdistance && abs(CPt_tmp->getY() - y) <= snapdistance)
						{
							Parent1 = item->second->getParent();
							Parent2 = NULL;
							AllSnapPoints.push_back(item->second);
						}
					}
					else
					{
						if(abs(CPt_tmp->getX() - x) <= snapdistance && abs(CPt_tmp->getY() - y) <= snapdistance)
						{
							found_snappt = true;
							Parent2 = NULL;
							if (prev_parent_id == -1)
							{
								Parent1 = item->second->getParent();
								prev_parent_id = Parent1->getId();
								CPt = CPt_tmp;
							}

							if (item->second->getParent()->getId() < prev_parent_id) 
							{
								Parent1 = item->second->getParent();
								prev_parent_id = Parent1->getId();
								CPt = CPt_tmp;
							}
						}
					}*/
				}
			}
			if(AllSnapPoints.size() > 1) 
				return (*AllSnapPoints.begin())->getPt();
			else 
			{
				if (found_snappt)
					return CPt;
				else
					return NULL;
			}
		}
		else 
			return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0010", __FILE__, __FUNCSIG__); return NULL;}
}

Vector* SnapPtManager::getNearestSnapPoint(RCollectionBase& shapes, double* Sline, double snapdistance, bool ShapePasted)
{
	try
	{
		Vector* CPt = NULL;
		for(RC_ITER Sitem = shapes.getList().begin(); Sitem != shapes.getList().end(); Sitem++)
		{
			Shape* Cshape = (Shape*)Sitem->second;
			if(!Cshape->Normalshape()) continue;
			if(!Cshape->IsValid() && Cshape->ShapeType != RapidEnums::SHAPETYPE::PERIMETER) continue;
			if(ShapePasted && Cshape->selected()) continue;
			SnapPointCollection* Spcoll = ((ShapeWithList*)Cshape)->SnapPointList;
			for(SPC_ITER item = Spcoll->getList().begin(); item != Spcoll->getList().end(); item++)
			{
				if(!item->second->IsValid)continue;
				CPt = item->second->getPt();
				double pt[3] = {CPt->getX(), CPt->getY(), CPt->getZ()};
				double dist = RMATH3DOBJECT->Distance_Point_Line(&pt[0], Sline);
				if(abs(dist) <= snapdistance)
				{
					Parent1 = item->second->getParent();
					Parent2 = NULL;
					SnapPtId = item->second->SPtId;
					return CPt;
				}
			}
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0011", __FILE__, __FUNCSIG__); return NULL; }
}


Vector* SnapPtManager::getNearestSnapPointUCS(RCollectionBase& shapes, double* TransformMatrix, double x, double y, double snapdistance, bool ShapePasted)
{
	try
	{
		Vector* CPt = NULL;
		double TransformM[16] = {0};
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		RMATH2DOBJECT->OperateMatrix4X4(TransformMatrix, 4, 1, TransformM);
		for(RC_ITER Sitem = shapes.getList().begin(); Sitem != shapes.getList().end(); Sitem++)
		{
			Shape* CShape = (Shape*)Sitem->second;
			if(!CShape->Normalshape()) continue;
			if(!CShape->IsValid() && CShape->ShapeType != RapidEnums::SHAPETYPE::PERIMETER) continue;
			if(ShapePasted && CShape->selected()) continue;
			SnapPointCollection* Spcoll = ((ShapeWithList*)CShape)->SnapPointList;
			for(SPC_ITER item = Spcoll->getList().begin(); item != Spcoll->getList().end(); item++)
			{
				if(!item->second->IsValid)continue;
				CPt = item->second->getPt();
				double temp1[4], temp2[4] = {CPt->getX(),CPt->getY(), CPt->getZ(), 1}; 
				RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp2, Order2, temp1);
				if(abs(temp1[0] - x) <= snapdistance && abs(temp1[1] - y) <= snapdistance)
				{
					Parent1 = item->second->getParent();
					Parent2 = NULL;
					SnapPtId = item->second->SPtId;
					return CPt;
				}
			}
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0011", __FILE__, __FUNCSIG__); return NULL; }
}



Vector* SnapPtManager::getNearestIntersectionPointOnShape(ShapeWithList* CurrentShape, double x, double y, double snapdistance, bool ShapePasted)
{
	try
	{
		if(ShapePasted && CurrentShape->selected()) return NULL;
		Vector* CPt = NULL;
		if(MAINDllOBJECT->ShowExplicitIntersectionPoints() || PPCALCOBJECT->IsPartProgramRunning())
		{
			for(ISN_ITER item = CurrentShape->IsectPointsList->getList().begin(); item != CurrentShape->IsectPointsList->getList().end(); item++)
			{
				IsectPt *ips = item->second;
				CPt = (Vector*)ips;
				if(abs(CPt->getX() - x) <= snapdistance && abs(CPt->getY() - y) <= snapdistance)
				{
					Parent1 = item->second->getParent1();
					Parent2 = item->second->getParent2();
					SnapPtId = item->second->IsnPtId;
					if(ips->GetIntersectionType())
					{
						if(MAINDllOBJECT->ShowImplicitIntersectionPoints())
						{
							MAINDllOBJECT->CurrentExtensionFlag = true;
							return CPt;
						}
						else if(PPCALCOBJECT->IsPartProgramRunning())
							return CPt;
						else
							return NULL;
					}
					else
						return CPt;
				}
			}
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0012", __FILE__, __FUNCSIG__); return NULL; }
}

Vector* SnapPtManager::getNearestIntersectionPointOnShape(ShapeWithList* CurrentShape, double* Sline, double snapdistance, bool ShapePasted)
{
	try
	{
		if(ShapePasted && CurrentShape->selected()) return NULL;
		Vector* CPt = NULL;
		if(MAINDllOBJECT->ShowExplicitIntersectionPoints() || PPCALCOBJECT->IsPartProgramRunning())
		{
			for(ISN_ITER item = CurrentShape->IsectPointsList->getList().begin(); item != CurrentShape->IsectPointsList->getList().end(); item++)
			{
				IsectPt *ips = item->second;
				CPt = (Vector*)ips;
				double pt[3] = {CPt->getX(), CPt->getY(), CPt->getZ()};
				double dist = RMATH3DOBJECT->Distance_Point_Line(&pt[0], Sline);
				if(dist <= snapdistance)
				{
					Parent1 = ips->getParent1();
					Parent2 = ips->getParent2();
					if(ips->GetIntersectionType())
					{
						if(MAINDllOBJECT->ShowImplicitIntersectionPoints())
						{
							MAINDllOBJECT->CurrentExtensionFlag = true;
							return CPt;
						}
						else if(PPCALCOBJECT->IsPartProgramRunning())
							return CPt;
						else
							return NULL;
					}
					else
						return CPt;
				}
			}
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0013", __FILE__, __FUNCSIG__); return NULL; }
}

Vector* SnapPtManager::getNearestIntersectionPointOnShapeUCS(ShapeWithList* CurrentShape, double* TransformMatrix, double x, double y, double snapdistance, bool ShapePasted)
{
	try
	{
		if(ShapePasted && CurrentShape->selected()) return NULL;
		Vector* CPt = NULL;
		double TransformM[16] = {0};
		int Order1[2] = {4, 4}, Order2[2] = {4, 1};
		RMATH2DOBJECT->OperateMatrix4X4(TransformMatrix, 4, 1, TransformM);
		if(MAINDllOBJECT->ShowExplicitIntersectionPoints() || PPCALCOBJECT->IsPartProgramRunning())
		{
			for(ISN_ITER item = CurrentShape->IsectPointsList->getList().begin(); item != CurrentShape->IsectPointsList->getList().end(); item++)
			{
				IsectPt *ips = item->second;
				CPt = (Vector*)ips;
				double temp1[4], temp2[4] = {CPt->getX(),CPt->getY(), CPt->getZ(), 1}; 
				RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp2, Order2, temp1);
				if(abs(temp1[0] - x) <= snapdistance && abs(temp1[1] - y) <= snapdistance)
				{
					Parent1 = item->second->getParent1();
					Parent2 = item->second->getParent2();
					if(ips->GetIntersectionType())
					{
						if(MAINDllOBJECT->ShowImplicitIntersectionPoints())
						{
							MAINDllOBJECT->CurrentExtensionFlag = true;
							return CPt;
						}
						else if(PPCALCOBJECT->IsPartProgramRunning())
							return CPt;
						else
							return NULL;
					}
					else
						return CPt;
				}
			}
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISPTMGR0012", __FILE__, __FUNCSIG__); return NULL; }
}
