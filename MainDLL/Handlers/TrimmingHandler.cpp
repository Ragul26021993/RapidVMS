#include "stdafx.h"
#include "TrimmingHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\IsectPtCollection.h"
#include "..\Engine\IsectPt.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Actions\TrimShapeAction.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"


//Constructor..//
TrimmingHandler::TrimmingHandler()
{
	try
	{
		runningPP = false;
		setMaxClicks(2);
		MAINDllOBJECT->SetStatusCode("TrimmingHandler01");
		/*MAINDllOBJECT->SetStatusCode("T001");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
TrimmingHandler::~TrimmingHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0002", __FILE__, __FUNCSIG__); }
}

//Mousemove....//
void TrimmingHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the left mousedown....//
void TrimmingHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(getClicksDone() == 1 && MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
	
		s1 = MAINDllOBJECT->highlightedShape();
		if(!s1->Normalshape())
			return;
		MAINDllOBJECT->dontUpdateGraphcis = true;
		getDimension(s1);
		ISN_ITER item = ((ShapeWithList*)s1)->IsectPointsList->getList().begin(); 
		int cnt = 0;
		if(!MAINDllOBJECT->ShowImplicitIntersectionPoints())
		{
			while (item != ((ShapeWithList*)s1)->IsectPointsList->getList().end())
			{
				IsectPt* i = (IsectPt*)(*item).second;
				if(!i->GetIntersectionType())
					cnt++;
				item++;
			}
		}
		else
			cnt = ((ShapeWithList*)s1)->IsectPointsList->getList().size() * 2;
		item = ((ShapeWithList*)s1)->IsectPointsList->getList().begin(); 
		double minxr;
		bool flag = false, firstpointflag = false, secondptflag = false;
		//double *intrsctnpts = (double*)malloc( sizeof(double) * cnt);
		double intrsctnpts[100];
		cnt = 0;
		while (item != ((ShapeWithList*)s1)->IsectPointsList->getList().end())
		{
			IsectPt* i = (IsectPt*)(*item).second;
			if(!i->GetIntersectionType())
			{
				if(i->getParent1() != NULL && i->getParent2() != NULL)
				{
					intrsctnpts[cnt++] = i->getX();
					intrsctnpts[cnt++] = i->getY();
				}
			}
			item++;
			/*if(MAINDllOBJECT->ShowImplicitIntersectionPoints())
			{
				IsectPt* i = (IsectPt*)(*item).second;
				if(i->getParent1() != NULL && i->getParent2() != NULL)
				{
					intrsctnpts[cnt++] = i->getX();
					intrsctnpts[cnt++] = i->getY();
				}
				else if((((Vector*)i)->operator !=(endp1)) && (((Vector*)i)->operator !=(endp2)))
				{
					intrsctnpts[cnt++] = i->getX();
					intrsctnpts[cnt++] = i->getY();
				}
				item++;
			}
			else
			{
				IsectPt* i = (IsectPt*)(*item).second;
				if(!i->GetIntersectionType())
				{
					if(i->getParent1() != NULL && i->getParent2() != NULL)
					{
						intrsctnpts[cnt++] = i->getX();
						intrsctnpts[cnt++] = i->getY();
					}
					else if((((Vector*)i)->operator !=(endp1)) && (((Vector*)i)->operator !=(endp2)))
					{
						intrsctnpts[cnt++] = i->getX();
						intrsctnpts[cnt++] = i->getY();
					}
				}
				item++;
			}*/
		}
		double cent[2], mp[2];
		double ptonarc[2];
		double firstangle,secondangle,ang,ang1;

		switch(s1->ShapeType)
		{		
			case RapidEnums::SHAPETYPE::LINE:
				if(angle > 1.48 && angle < 1.65)
				{
					for(int i = 0; i < cnt/2; i++)
					{
						if(!flag && ((pt->getY() - intrsctnpts[2 * i + 1]) < 0))
						{
							minxr = pt->getY() - intrsctnpts[2 * i + 1];
							flag = true;
							firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							firstpointflag = true;
						}
						else if(flag)
						{
							if(((pt->getY() - intrsctnpts[2 * i + 1]) < 0) && ((pt->getY() - intrsctnpts[2 * i + 1]) > minxr))
							{
								minxr = pt->getY() - intrsctnpts[2 * i + 1];
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							}
						}
					}
					flag = false;
					for(int i = 0; i < cnt/2; i++)
					{
						if(!flag && ((pt->getY() - intrsctnpts[2 * i + 1]) > 0))
						{
							minxr = pt->getY() - intrsctnpts[2 * i + 1];
							flag = true;
							secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							secondptflag = true;
						}
						else if(flag)
						{
							if(((pt->getY() - intrsctnpts[2 * i + 1]) > 0) && ((pt->getY() - intrsctnpts[2 * i + 1]) < minxr))
							{
								minxr = pt->getY() - intrsctnpts[2 * i + 1];
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							}
						}
					}
				}
				else
				{
					for(int i = 0; i < cnt/2; i++)
					{
						if(!flag && ((pt->getX() - intrsctnpts[2 * i]) < 0))
						{
							minxr = pt->getX() - intrsctnpts[2 * i];
							flag = true;
							firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							firstpointflag = true;
						}
						else if(flag)
						{
							if(((pt->getX() - intrsctnpts[2 * i]) < 0) && ((pt->getX() - intrsctnpts[2 * i]) > minxr))
							{
								minxr = pt->getX() - intrsctnpts[2 * i];
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							}
						}
					}
					flag = false;
					for(int i = 0; i < cnt/2; i++)
					{
						if(!flag && ((pt->getX() - intrsctnpts[2 * i]) > 0))
						{
							minxr = pt->getX() - intrsctnpts[2 * i];
							flag = true;
							secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							secondptflag = true;
						}
						else if(flag)
						{
							if(((pt->getX() - intrsctnpts[2 * i]) > 0) && ((pt->getX() - intrsctnpts[2 * i]) < minxr))
							{
								minxr = pt->getX() - intrsctnpts[2 * i];
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
							}
						}
					}
				}
				if(firstpoint.operator ==(endp1) || firstpoint.operator ==(endp2))
					firstpointflag = false;
				if(secondpt.operator ==(endp1) || secondpt.operator ==(endp2))
					secondptflag = false;
				if(!firstpointflag && !secondptflag)
				{
					MAINDllOBJECT->getShapesList().clearSelection();
					s1->selected(true);
					MAINDllOBJECT->deleteShape();
				}
				else if(firstpointflag && !secondptflag)
				{
					if(angle > 1.48 && angle < 1.65)
					{
						if((firstpoint.getY() - endp1.getY()) > 0)
							((Line*)s1)->setPoint1(Vector(firstpoint.getX(), firstpoint.getY(), ((Line*)s1)->getPoint1()->getZ()));
						else
							((Line*)s1)->setPoint2(Vector(firstpoint.getX(), firstpoint.getY(), ((Line*)s1)->getPoint2()->getZ()));
					}
					else
					{
						if((firstpoint.getX() - endp1.getX()) > 0)
							((Line*)s1)->setPoint1(Vector(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint1()->getZ()));
						else
							((Line*)s1)->setPoint2(Vector(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint2()->getZ()));
					}
					((Line*)s1)->calculateAttributes();
					s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
					TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
				}
				else if(!firstpointflag && secondptflag)
				{
					if(angle > 1.48 && angle < 1.65)
					{
						if((secondpt.getY() - endp1.getY()) < 0)
							((Line*)s1)->setPoint1(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ()));
						else
							((Line*)s1)->setPoint2(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint2()->getZ()));
					}
					else
					{
						if((secondpt.getX() - endp1.getX()) < 0)
							((Line*)s1)->setPoint1(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ()));
						else
							((Line*)s1)->setPoint2(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint2()->getZ()));
					}
					((Line*)s1)->calculateAttributes();
					s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
					TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
				}
				else
				{
					double d1 = RMATH2DOBJECT->Pt2Pt_distance(firstpoint.getX(), firstpoint.getY(), endp1.getX(), endp1.getY());
					double d2 = RMATH2DOBJECT->Pt2Pt_distance(secondpt.getX(), secondpt.getY(), endp1.getX(), endp1.getY());
					if(d1 < d2)
					{
						ShapeWithList* myshape = new Line();
						((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
						setBaseRProperty(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ());
						AddShapeAction::addShape(myshape);
						AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
						myshape->ShapeThickness(1);
						setBaseRProperty(((Line*)s1)->getPoint2()->getX(), ((Line*)s1)->getPoint2()->getY(), ((Line*)s1)->getPoint2()->getZ());
						AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
						((Line*)s1)->setPoint2(Vector(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint2()->getZ()));
						((Line*)s1)->calculateAttributes();
						s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
						TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
					}
					else
					{
						ShapeWithList* myshape = new Line();
						((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
						setBaseRProperty(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint1()->getZ());
						AddShapeAction::addShape(myshape);
						AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
						myshape->ShapeThickness(1);
						setBaseRProperty(((Line*)s1)->getPoint2()->getX(), ((Line*)s1)->getPoint2()->getY(), ((Line*)s1)->getPoint2()->getZ());
						AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
						((Line*)s1)->setPoint2(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint2()->getZ()));
						((Line*)s1)->calculateAttributes();
						s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
						TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
					}
				}
				break;
				case RapidEnums::SHAPETYPE::XLINE:
					if(angle > 1.48 && angle < 1.65)
					{
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getY() - intrsctnpts[2 * i + 1]) < 0))
							{
								minxr = pt->getY() - intrsctnpts[2 * i + 1];
								flag = true;
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								firstpointflag = true;
							}
							else if(flag)
							{
								if(((pt->getY() - intrsctnpts[2 * i + 1]) < 0) && ((pt->getY() - intrsctnpts[2 * i + 1]) > minxr))
								{
									minxr = pt->getY() - intrsctnpts[2 * i + 1];
									firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
						flag = false;
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getY() - intrsctnpts[2 * i + 1]) > 0))
							{
								minxr = pt->getY() - intrsctnpts[2 * i + 1];
								flag = true;
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								secondptflag = true;
							}
							else if(flag)
							{
								if(((pt->getY() - intrsctnpts[2 * i + 1]) > 0) && ((pt->getY() - intrsctnpts[2 * i + 1]) < minxr))
								{
									minxr = pt->getY() - intrsctnpts[2 * i + 1];
									secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
					}
					else
					{
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getX() - intrsctnpts[2 * i]) < 0))
							{
								minxr = pt->getX() - intrsctnpts[2 * i];
								flag = true;
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								firstpointflag = true;
							}
							else if(flag)
							{
								if(((pt->getX() - intrsctnpts[2 * i]) < 0) && ((pt->getX() - intrsctnpts[2 * i]) > minxr))
								{
									minxr = pt->getX() - intrsctnpts[2 * i];
									firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
						flag = false;
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getX() - intrsctnpts[2 * i]) > 0))
							{
								minxr = pt->getX() - intrsctnpts[2 * i];
								flag = true;
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								secondptflag = true;
							}
							else if(flag)
							{
								if(((pt->getX() - intrsctnpts[2 * i]) > 0) && ((pt->getX() - intrsctnpts[2 * i]) < minxr))
								{
									minxr = pt->getX() - intrsctnpts[2 * i];
									secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
					}
					if(firstpoint.operator ==(endp1) || firstpoint.operator ==(endp2))
						firstpointflag = false;
					if(secondpt.operator ==(endp1) || secondpt.operator ==(endp2))
						secondptflag = false;
					if(!firstpointflag && !secondptflag)
					{
						/*MAINDllOBJECT->getShapesList().clearSelection();
						s1->selected(true);
						MAINDllOBJECT->deleteShape();*/
					}
					else if(firstpointflag && !secondptflag)
					{
						s1->ShapeType = RapidEnums::SHAPETYPE::XRAY;
						((Line*)s1)->setPoint1(Vector(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint1()->getZ()));
						double pnt[2] = {pt->getX(),pt->getY()};
						RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle, intercept,&pnt[0], &intrcn[0]); 
						rangle = RMATH2DOBJECT->ray_angle(intrcn[0], intrcn[1], firstpoint.getX(), firstpoint.getY());
						((Line*)s1)->setPoint2(Vector(firstpoint.getX() + 50 * cos(rangle), firstpoint.getY() + 50 * sin(rangle), ((Line*)s1)->getPoint1()->getZ()));
						((Line*)s1)->setLineParameters(rangle, ((Line*)s1)->Intercept());
						s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
						TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
					}
					else if(!firstpointflag && secondptflag)
					{
						s1->ShapeType = RapidEnums::SHAPETYPE::XRAY;
						((Line*)s1)->setPoint1(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ()));
						double pnt[2] = {pt->getX(),pt->getY()};
						RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle, intercept,&pnt[0], &intrcn[0]); 
						rangle = RMATH2DOBJECT->ray_angle(intrcn[0], intrcn[1], secondpt.getX(), secondpt.getY());
						((Line*)s1)->setPoint2(Vector(secondpt.getX() + 50 * cos(rangle), secondpt.getY() + 50 * sin(rangle), ((Line*)s1)->getPoint1()->getZ()));
						((Line*)s1)->setLineParameters(rangle, ((Line*)s1)->Intercept());
						s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
						TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
					}
					else
					{
						s1->ShapeType = RapidEnums::SHAPETYPE::XRAY;
						((Line*)s1)->setPoint1(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ()));
						double pnt[2] = {pt->getX(),pt->getY()};
						RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle, intercept,&pnt[0], &intrcn[0]); 
						rangle = RMATH2DOBJECT->ray_angle(intrcn[0], intrcn[1],secondpt.getX(), secondpt.getY());
						((Line*)s1)->setLineParameters(rangle, ((Line*)s1)->Intercept());
						((Line*)s1)->setPoint2(Vector(secondpt.getX() + 50 * cos(rangle), secondpt.getY() + 50 * sin(rangle), ((Line*)s1)->getPoint1()->getZ()));
						s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
						TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);

						ShapeWithList* myshape = new Line(_T("R"), RapidEnums::SHAPETYPE::XRAY);
						setBaseRProperty(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint1()->getZ());
						AddShapeAction::addShape(myshape);
						AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
						rangle = RMATH2DOBJECT->ray_angle(intrcn[0], intrcn[1],firstpoint.getX(), firstpoint.getY());
						myshape->ShapeThickness(1);
						setBaseRProperty(firstpoint.getX() + 50 * cos(rangle), firstpoint.getY() + 50 * sin(rangle), ((Line*)s1)->getPoint1()->getZ());
						AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
					}
					break;
				case RapidEnums::SHAPETYPE::XRAY:
					if(angle > 1.48 && angle < 1.65)
					{
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getY() - intrsctnpts[2 * i + 1]) < 0))
							{
								minxr = pt->getY() - intrsctnpts[2 * i + 1];
								flag = true;
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								firstpointflag = true;
							}
							else if(flag)
							{
								if(((pt->getY() - intrsctnpts[2 * i + 1]) < 0) && ((pt->getY() - intrsctnpts[2 * i + 1]) > minxr))
								{
									minxr = pt->getY() - intrsctnpts[2 * i + 1];
									firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
						flag = false;
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getY() - intrsctnpts[2 * i + 1]) > 0))
							{
								minxr = pt->getY() - intrsctnpts[2 * i + 1];
								flag = true;
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								secondptflag = true;
							}
							else if(flag)
							{
								if(((pt->getY() - intrsctnpts[2 * i + 1]) > 0) && ((pt->getY() - intrsctnpts[2 * i + 1]) < minxr))
								{
									minxr = pt->getY() - intrsctnpts[2 * i + 1];
									secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
					}
					else
					{
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getX() - intrsctnpts[2 * i]) < 0))
							{
								minxr = pt->getX() - intrsctnpts[2 * i];
								flag = true;
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								firstpointflag = true;
							}
							else if(flag)
							{
								if(((pt->getX() - intrsctnpts[2 * i]) < 0) && ((pt->getX() - intrsctnpts[2 * i]) > minxr))
								{
									minxr = pt->getX() - intrsctnpts[2 * i];
									firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
						flag = false;
						for(int i = 0; i < cnt/2; i++)
						{
							if(!flag && ((pt->getX() - intrsctnpts[2 * i]) > 0))
							{
								minxr = pt->getX() - intrsctnpts[2 * i];
								flag = true;
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								secondptflag = true;
							}
							else if(flag)
							{
								if(((pt->getX() - intrsctnpts[2 * i]) > 0) && ((pt->getX() - intrsctnpts[2 * i]) < minxr))
								{
									minxr = pt->getX() - intrsctnpts[2 * i];
									secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Line*)s1)->getPoint1()->getZ());
								}
							}
						}
					}
					if(firstpoint.operator ==(endp1) || firstpoint.operator ==(endp2))
						firstpointflag = false;
					if(secondpt.operator ==(endp1) || secondpt.operator ==(endp2))
						secondptflag = false;
					if(!firstpointflag && !secondptflag)
					{
						MAINDllOBJECT->getShapesList().clearSelection();
						s1->selected(true);
						MAINDllOBJECT->deleteShape();
					}
					else if(firstpointflag && !secondptflag)
					{
						double d1 = RMATH2DOBJECT->Pt2Pt_distance(firstpoint.getX(), firstpoint.getY(), endp1.getX(), endp1.getY());
						double d2 = RMATH2DOBJECT->Pt2Pt_distance(pt->getX(), pt->getY(), endp1.getX(), endp1.getY());
						if(d2 < d1)
						{
							((Line*)s1)->setPoint1(Vector(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint1()->getZ()));
				
						}
						else
						{
							s1->ShapeType = RapidEnums::SHAPETYPE::LINE;
							((Line*)s1)->setPoint2(Vector(((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY(),((Line*)s1)->getPoint1()->getZ()));
							((Line*)s1)->setPoint1(Vector(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint1()->getZ()));							
						}
						((Line*)s1)->calculateAttributes();
						s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
						TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
					}
					else if(!firstpointflag && secondptflag)
					{
						double d1 = RMATH2DOBJECT->Pt2Pt_distance(secondpt.getX(), secondpt.getY(), endp1.getX(), endp1.getY());
						double d2 = RMATH2DOBJECT->Pt2Pt_distance(pt->getX(), pt->getY(), endp1.getX(), endp1.getY());
						if(d2 < d1)
						{
							((Line*)s1)->setPoint1(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ()));
						}
						else
						{
							s1->ShapeType = RapidEnums::SHAPETYPE::LINE;
							((Line*)s1)->setPoint2(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ()));
						}
						((Line*)s1)->calculateAttributes();
						 s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);
						 TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
					}
					else
					{
						double d1 = RMATH2DOBJECT->Pt2Pt_distance(firstpoint.getX(), firstpoint.getY(), endp1.getX(), endp1.getY());
						double d2 = RMATH2DOBJECT->Pt2Pt_distance(secondpt.getX(), secondpt.getY(), endp1.getX(), endp1.getY());
						if(d1 < d2)
						{
							ShapeWithList* myshape = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
							((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
							setBaseRProperty(firstpoint.getX(), firstpoint.getY(),((Line*)s1)->getPoint1()->getZ());
							AddShapeAction::addShape(myshape);
							AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
							myshape->ShapeThickness(1);
							setBaseRProperty(((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY(), ((Line*)s1)->getPoint1()->getZ());
							AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);

							((Line*)s1)->setPoint1(Vector(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ()));
							((Line*)s1)->setPoint2(Vector(secondpt.getX() + 5 * cos(angle), secondpt.getY() + 5 * sin(angle),((Line*)s1)->getPoint1()->getZ()));
							((Line*)s1)->calculateAttributes();
							s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
							TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
						}
						else
						{
							ShapeWithList* myshape = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
							((Line*)myshape)->LineType = RapidEnums::LINETYPE::FINITELINE;
							setBaseRProperty(secondpt.getX(), secondpt.getY(),((Line*)s1)->getPoint1()->getZ());
							AddShapeAction::addShape(myshape);
							AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
							myshape->ShapeThickness(1);
							setBaseRProperty(((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY(), ((Line*)s1)->getPoint1()->getZ());
							AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
							((Line*)s1)->setPoint1(Vector(firstpoint.getX(), firstpoint.getY(), ((Line*)s1)->getPoint1()->getZ()));
							((Line*)s1)->setPoint2(Vector(firstpoint.getX() + 5 * cos(angle), firstpoint.getY() + 5 * sin(angle),((Line*)s1)->getPoint1()->getZ()));
							((Line*)s1)->calculateAttributes();
							s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
							TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentLineType, s1);
						}
					}
					break;
					case RapidEnums::SHAPETYPE::CIRCLE:
						cent[0] = cen.getX();  cent[1] = cen.getY(); mp[0] = pt->getX(); mp[1] = pt->getY();
						RMATH2DOBJECT->Point_onCircle(&cent[0],radius, &mp[0],&ptonarc[0]);
						ang = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), ptonarc[0], ptonarc[1]);
						if(cnt > 2)
						{
						for(int i = 0; i < cnt/2; i++)
						{
							ang1 = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), intrsctnpts[2 * i], intrsctnpts[2 * i + 1]);
							if(!flag)
							{
								minxr = ang1 -  ang;
								if(minxr < 0) minxr += 2 * M_PI;
								flag = true;
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								firstpointflag = true;
							}
							else if(flag)
							{
								double tempangle = ang1 -  ang;
								if(tempangle < 0) tempangle += 2 * M_PI;
								if(tempangle > minxr)
								{
									minxr = tempangle;
									secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								}
							}
						}
						flag = false;
						for(int i = 0; i < cnt/2; i++)
						{
							ang1 = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), intrsctnpts[2 * i], intrsctnpts[2 * i + 1]);
							if(!flag)
							{
								minxr = ang1 -  ang;
								if(minxr < 0) minxr += 2 * M_PI;
								flag = true;
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								secondptflag = true;
							}
							else if(flag)
							{
								double tempangle = ang1 -  ang;
								if(tempangle < 0) tempangle += 2 * M_PI;
								if(tempangle < minxr)
								{
									minxr = tempangle;
									firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								}
							}
						}
						}
						if(!firstpointflag && !secondptflag)
						{
							MAINDllOBJECT->getShapesList().clearSelection();
							s1->selected(true);
							MAINDllOBJECT->deleteShape();
						}
						else if(firstpointflag && secondptflag)
						{
							firstangle = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), firstpoint.getX(), firstpoint.getY());
							secondangle = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), secondpt.getX(), secondpt.getY());
							
							double midp[2]={(secondpt.getX() + firstpoint.getX())/2, (secondpt.getY() + firstpoint.getY())/2};
							s1->ShapeType = RapidEnums::SHAPETYPE::ARC;
							((Circle*)s1)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
							
							((Circle*)s1)->setendpoint1(Vector(firstpoint.getX(), firstpoint.getY(), ((Circle*)s1)->getendpoint1()->getZ()));

							double midangle = secondangle - firstangle;
							if(midangle < 0)
								midangle += 2 * M_PI;
							((Circle*)s1)->Startangle(firstangle);
							((Circle*)s1)->Sweepangle(midangle);
							midangle = firstangle + midangle/2;
							ptonarc[0] = cent[0] + radius * cos(midangle);
							ptonarc[1] = cent[1] + radius * sin(midangle);
							((Circle*)s1)->getMidPoint()->set(ptonarc[0], ptonarc[1], ((Circle*)s1)->getendpoint1()->getZ());
							((Circle*)s1)->setendpoint2(Vector(secondpt.getX(), secondpt.getY(), ((Circle*)s1)->getendpoint1()->getZ()));
							((Circle*)s1)->calculateAttributes();
							s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);
							TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentCircleType, s1);
						}
						break;
					case RapidEnums::SHAPETYPE::ARC:
						cent[0] = cen.getX();  cent[1] = cen.getY(); mp[0] = pt->getX(); mp[1] = pt->getY();
						RMATH2DOBJECT->Point_onCircle(&cent[0], radius, &mp[0], &ptonarc[0]);
						ang = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), ptonarc[0], ptonarc[1]);
						if(ang < startang) 
							ang += 2 * M_PI;
						for(int i = 0; i < cnt/2; i++)
						{
							ang1 = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), intrsctnpts[2 * i], intrsctnpts[2 * i + 1]);
							if(ang1 < startang) 
								ang1 += 2 * M_PI;
							if(!flag && ((ang -  ang1) > 0))
							{
								minxr = ang -  ang1;
								flag = true;
								firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								firstpointflag = true;
							}
							else if(flag)
							{
								if(((ang -  ang1) > 0) && ((ang -  ang1) < minxr))
								{
									minxr = ang -  ang1;
									firstpoint.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								}
							}
						}
						flag = false;
						for(int i = 0; i < cnt/2; i++)
						{
							ang1 = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), intrsctnpts[2 * i], intrsctnpts[2 * i + 1]);
							if(ang1 < startang) 
								ang1 += 2 * M_PI;
							if(!flag && ((ang -  ang1) < 0))
							{
								minxr = ang -  ang1;
								flag = true;
								secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								secondptflag = true;
							}
							else if(flag)
							{
								if(((ang -  ang1) < 0) && ((ang -  ang1) > minxr))
								{
									minxr = ang -  ang1;
									secondpt.set(intrsctnpts[2 * i], intrsctnpts[2 * i + 1], ((Circle*)s1)->getendpoint1()->getZ());
								}
							}
						}

						if(firstpoint.operator ==(endp1) || firstpoint.operator ==(endp2))
							firstpointflag = false;
						if(secondpt.operator ==(endp1) || secondpt.operator ==(endp2))
							secondptflag = false;
						if(!firstpointflag && !secondptflag)
						{
							MAINDllOBJECT->getShapesList().clearSelection();
							s1->selected(true);
							MAINDllOBJECT->deleteShape();
						}
						else if(firstpointflag && !secondptflag)
						{
							((Circle*)s1)->setendpoint2(Vector(firstpoint.getX(), firstpoint.getY(), ((Circle*)s1)->getendpoint1()->getZ()));
							((Circle*)s1)->calculateAttributes();
							s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);
							TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentCircleType, s1);
						}
						else if(!firstpointflag && secondptflag)
						{							
							((Circle*)s1)->setendpoint1(Vector(secondpt.getX(), secondpt.getY(), ((Circle*)s1)->getendpoint1()->getZ()));
							((Circle*)s1)->calculateAttributes();
							s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);
							TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentCircleType, s1);
						}
						else
						{
							firstangle = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), firstpoint.getX(), firstpoint.getY());
							secondangle = RMATH2DOBJECT->ray_angle(cen.getX(), cen.getY(), secondpt.getX(), secondpt.getY());
							double midp[2]= {(secondpt.getX() + ((Circle*)s1)->getendpoint2()->getX())/2, (secondpt.getY() + ((Circle*)s1)->getendpoint2()->getY())/2};
							ShapeWithList* myshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
							((Circle*)myshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
							setBaseRProperty(secondpt.getX(), secondpt.getY(),((Circle*)s1)->getendpoint1()->getZ());
							AddShapeAction::addShape(myshape);
							AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
							RMATH2DOBJECT->Point_onArc(&cent[0], radius, startang, sweepang, &midp[0], &ptonarc[0]);
							setBaseRProperty(ptonarc[0], ptonarc[1],((Circle*)s1)->getendpoint1()->getZ());
							AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
							myshape->ShapeThickness(((Circle*)s1)->ShapeThickness());
							setBaseRProperty(((Circle*)s1)->getendpoint2()->getX(), ((Circle*)s1)->getendpoint2()->getY(), ((Circle*)s1)->getendpoint2()->getZ());
							AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
							((Circle*)s1)->setendpoint2(Vector(firstpoint.getX(), firstpoint.getY(), ((Circle*)s1)->getendpoint1()->getZ()));
							((Circle*)s1)->calculateAttributes();
							s1->notifyAll(ITEMSTATUS::DATACHANGED, s1);	
							TrimShapeAction::TrimShape(endp1, endp2, CurrentShapeType, CurrentCircleType, s1);
						}
						break;
		}
		resetClicks();
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->update_VideoGraphics();
		MAINDllOBJECT->update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0004", __FILE__, __FUNCSIG__); }
}

//Draw the parallel arc..//
void TrimmingHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0005", __FILE__, __FUNCSIG__); }
}

void TrimmingHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0006", __FILE__, __FUNCSIG__); }
}

//Get the arc dimension...
//If the shape is not arc then return false....//
bool TrimmingHandler::getDimension( Shape *s )
{
	try
	{
		bool flag = false;
		switch(s->ShapeType)
		{
		case RapidEnums::SHAPETYPE::CIRCLE:
		case RapidEnums::SHAPETYPE::ARC:
			cen.set(*((Circle*)s)->getCenter());
			radius = ((Circle*)s)->Radius();
			endp1.set(*((Circle*)s)->getendpoint1());
			endp2.set(*((Circle*)s)->getendpoint2());
			startang = ((Circle*)s)->Startangle();
			sweepang = ((Circle*)s)->Sweepangle();
			CurrentShapeType = s->ShapeType;
			CurrentCircleType = ((Circle*)s)->CircleType;
			flag = true;
			break;

		case RapidEnums::SHAPETYPE::XLINE:
		case RapidEnums::SHAPETYPE::XRAY:
		case RapidEnums::SHAPETYPE::LINE:
			angle = ((Line*)s)->Angle();
			intercept = ((Line*)s)->Intercept();
			endp1.set(*((Line*)s)->getPoint1());
			endp2.set(*((Line*)s)->getPoint2());
			CurrentShapeType = s->ShapeType;
			CurrentLineType = ((Line*)s)->LineType;
			flag = true;
			break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0007", __FILE__, __FUNCSIG__); return false; }
}

//Handle the partprogram data.....//
void TrimmingHandler::PartProgramData()
{
	try
	{
		s2 = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		s2->IsValid(false);
		list<BaseItem*>::iterator pShape;
		pShape = s2->getParents().begin();
		s1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*pShape++)->getId()];
		getDimension(s1);
		setClicksDone(getMaxClicks() - 1);
		runningPP = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the max click...
//Set the  parallel arc parameters...
void TrimmingHandler::LmaxmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THMH0009", __FILE__, __FUNCSIG__); }
}