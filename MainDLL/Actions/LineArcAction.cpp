#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "LineArcAction.h"
#include "MergeShapesAction.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Handlers_SmartMeasures\SM_Chamfer_LineMeasureHandler.h"
#include "..\Handlers_SmartMeasures\SM_Chamfer_ArcMeasureHandler.h"
#include "..\Actions\AddShapeAction.h"

LineArcAction::LineArcAction(bool auto_thread):RAction(_T("AddLineArc"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDLINEARCPFACTION;
	this->FramegrabactionAdded = false;
	this->autothread = auto_thread;
	this->closed_loop = false;
	this->shape_as_fasttrace = false;
	this->pttype = ptscollectiontype::EDGE;
	MAINDllOBJECT->getShapesList().addListener(this);
}

LineArcAction::~LineArcAction()
{	
	try
	{ 
		for (int i =0; i < FrameGrabCollection.size(); i++)
			FrameGrabCollection[i].clear();
		FrameGrabCollection.clear();
		TempShapeCollection.clear(); 
		fgTempCollection.clear();
		PointActionList.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0001", __FILE__, __FUNCSIG__); }
}

bool LineArcAction::execute()
{
	try
	{
		ShapeWithList* shape;
		map<int, FramegrabBase*> baseFG;
		int cnt = fgTempCollection.size();
		if (FrameGrabCollection.size() != cnt) return false;
		for (int i = 0; i < cnt; i++)
		{
			shape = (ShapeWithList*) fgTempCollection[i];
			baseFG = FrameGrabCollection[i];
			for (int j = 0; j < baseFG.size(); j++)
			{
				if(shape != NULL && baseFG[j]->AllPointsList.Pointscount() > 0)
				{
					shape->ManagePoint(baseFG[j], shape->ADD_POINT);		
				}
			}
		}
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0002", __FILE__, __FUNCSIG__); return false; }
}

bool LineArcAction::redo()
{
	try
	{
		ShapeWithList* shape;
		map<int, FramegrabBase*> baseFG;
		int cnt = fgTempCollection.size();
		if (FrameGrabCollection.size() != cnt) return false;
		int k = 0;
		for (int i = 0; i < cnt; i++)
		{
			shape = (ShapeWithList*) fgTempCollection[i];
			baseFG = FrameGrabCollection[i];
			for (int j = 0; j < baseFG.size(); j++)
			{
				shape->Modify_Points(baseFG[j], shape->ADD_POINT);	
				if (k >= PointActionList.size()) continue;
				shape->PointAtionList.push_back(PointActionList[k]);
				k++;
			}
		}
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0003", __FILE__, __FUNCSIG__); return false; }
}

void LineArcAction::undo()
{
	try
	{
		ShapeWithList* shape;
		map<int, FramegrabBase*> baseFG;
		int cnt = fgTempCollection.size();
		if (FrameGrabCollection.size() != cnt) return;
		int k = 0;
		for (int i = 0; i < cnt; i++)
		{
			shape = (ShapeWithList*)fgTempCollection[i];
			baseFG = FrameGrabCollection[i];	
			for (int j = 0; j < baseFG.size(); j++)
			{
				shape->Modify_Points(baseFG[j], shape->REMOVE_POINT);	
				if (k >= PointActionList.size()) continue;
				shape->PointAtionList.remove(PointActionList[k]);
				k++;
			}
		}
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0004", __FILE__, __FUNCSIG__); }
}

void LineArcAction::lineArcSep(Shape* CShape)
{
	try
	{
		ShapeWithList* CldShape = (ShapeWithList*)CShape;
		//allow below variables to be modifiable through settings
		double max_radius = 2;
		double min_radius = 0.01;
		double min_angle_cutoff = M_PI/18;
		double tolerance = 0.003; //number of pixel widths that we are allowing the point to deviate from the line or arc
		double noise = 0.009; //number of tolerance-levels (tolerance factor times pixel width) being considered as "noise" - shapes less than this length will be merged with other shapes.
		double max_dist_betn_pts = 1000;
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
		{
			max_radius = 2;
			min_radius = 0.05;
		}
		lineArcSep(CldShape->PointsList, tolerance, max_dist_betn_pts, true, max_radius, min_radius, min_angle_cutoff, noise );
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0008", __FILE__, __FUNCSIG__); }
}

void LineArcAction::lineArcSep(PointCollection* PntCollection, double tolerance, double max_dist_betn_pts, bool sort, double max_radius, double min_radius, double min_angle_cutoff, double noise, bool addShapes, bool best_fit_curve, bool closed_lp, bool shp_as_fast_trc, bool join_all_pts )
{
	try
	{
		this->closed_loop = closed_lp;
		this->shape_as_fasttrace = shp_as_fast_trc;
		Shape* newShape;
		list<Pt> AllPointsCollection;
		map<int, Pt> AllPointsColl;
		map<int, Pt> AllPointsTempCollection;
		map<int, Shape*> TempShapeCollection2;


		int Cnt = 0; bool flag = true; 	Pt leftmostpoint; 
		if (sort)
		{
			for(PC_ITER SptItem = PntCollection->getList().begin(); SptItem != PntCollection->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				Pt newpt;
				newpt.x = Spt->X; newpt.y = Spt->Y;
				newpt.pt_index = Cnt;
				if(flag)
				{ 
					flag = false;
					leftmostpoint.x = newpt.x; leftmostpoint.y = newpt.y; 
					leftmostpoint.pt_index = newpt.pt_index;
				}
				else
				{
					if(newpt.x < leftmostpoint.x)
					{
						leftmostpoint.x = newpt.x; leftmostpoint.y = newpt.y; 
						leftmostpoint.pt_index = newpt.pt_index;
					}
				}
				AllPointsColl[Cnt] = newpt;
				Cnt++;
			}
			Cnt = 0;
			Pt FisrtPt;
			FisrtPt.x = leftmostpoint.x; FisrtPt.y = leftmostpoint.y;
			FisrtPt.pt_index = 0;
			int TotalCnt = AllPointsColl.size();
			AllPointsTempCollection[0] = FisrtPt;
			AllPointsCollection.push_back(FisrtPt);
			AllPointsColl.erase(leftmostpoint.pt_index);
		
			for(int i = 0; i < TotalCnt - 1; i++)
			{
				Pt tempPoint = AllPointsTempCollection[Cnt];
				Pt CurrentPt;
				double dist = 0, mindist = 0; flag = true;
				int C_index;
				for(map<int, Pt>::iterator Pitem = AllPointsColl.begin(); Pitem != AllPointsColl.end(); Pitem++)
				{
					Pt Cpt = (*Pitem).second;
					double tempdist = 0;
					tempdist += pow((tempPoint.x - Cpt.x), 2);  
					tempdist += pow((tempPoint.y - Cpt.y), 2);  
					dist = sqrt(tempdist);
					if(flag)
					{ 
						mindist = dist; flag = false;
						CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
						C_index = Cpt.pt_index;
					}
					else
					{
						if(mindist > dist)
						{
							mindist = dist;
							CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
							C_index = Cpt.pt_index;
						}
					}
				}
				Cnt++;
				CurrentPt.pt_index = Cnt;
				AllPointsTempCollection[Cnt] = CurrentPt;
				AllPointsCollection.push_back(CurrentPt);
				AllPointsColl.erase(C_index);
			}
		}
		else
		{
			for(PC_ITER SptItem = PntCollection->getList().begin(); SptItem != PntCollection->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				Pt newpt;
				newpt.x = Spt->X; newpt.y = Spt->Y;
				newpt.pt_index = Cnt;
				AllPointsTempCollection[Cnt] = newpt;
				AllPointsCollection.push_back(newpt);
				Cnt++;
			}
		}
		int i = 0;
		int step = -1;
		if (join_all_pts)
		{
			list<Pt>::iterator pt_iter_next;
			for (list<Pt>::iterator pt_iter = AllPointsCollection.begin(); pt_iter != AllPointsCollection.end(); pt_iter++)
			{
				PointCollection PtColl;
				pt_iter_next = pt_iter;
				pt_iter_next++;
				if (pt_iter_next != AllPointsCollection.end())
				{
					PtColl.Addpoint(new SinglePoint(pt_iter->x, pt_iter->y, MAINDllOBJECT->getCurrentDRO().getZ()));
					PtColl.Addpoint(new SinglePoint(pt_iter_next->x, pt_iter_next->y, MAINDllOBJECT->getCurrentDRO().getZ()));
					newShape = new Line();
					TempShapeCollection[i] = newShape;
					((ShapeWithList*)newShape)->AddPoints(&PtColl);
					i++;
				}
			}
		}
		else
		{
			if (best_fit_curve == false)
			{
				list<LineArc> LineArcColl;
			
				if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
				{
					step = 1;
				}
				BESTFITOBJECT->fnLinesAndArcs(&AllPointsCollection, &LineArcColl, tolerance, max_radius, min_radius, step, max_dist_betn_pts);
				//Convert to Rapid-I shape type and add to the tempShape Collection
				for (list<LineArc>::iterator linearc_iter = LineArcColl.begin(); linearc_iter != LineArcColl.end(); linearc_iter++)
				{
					PointCollection PtColl;
					int start_index = linearc_iter->start_pt.pt_index;
					int end_index = linearc_iter->end_pt.pt_index;
					for(int j = start_index; j <= end_index; j++)
					{
						PtColl.Addpoint(new SinglePoint(AllPointsTempCollection[j].x , AllPointsTempCollection[j].y, MAINDllOBJECT->getCurrentDRO().getZ()));
					}
					if(abs(linearc_iter->slope) < 0.02) linearc_iter->slope = 0;
					if(linearc_iter->slope == 0) // Add new Line..
					{
						newShape = new Line();
					}
					else
					{
						newShape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
						((Circle*)newShape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}
					TempShapeCollection2[i] = newShape;
					((ShapeWithList*)newShape)->AddPoints(&PtColl);
					i++;
				}
				LineArcColl.clear();
			}
			else
			{
				Curve_2DList curveLst;
				double* pts = (double*) malloc(2*sizeof(double) * AllPointsCollection.size());
				PtsList::iterator ptr_iter = AllPointsCollection.begin();
				for (int i = 0; i < AllPointsCollection.size(); i++)
				{
					*(pts + 2 * i) = ptr_iter->x;
					*(pts + 2 * i + 1) = ptr_iter->y;
					ptr_iter++;
				}
				step = 1;
				if (AllPointsCollection.size() > 100)
					step = AllPointsCollection.size()/100;
				if (step > 5)
					step = 5;
				if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
				{
					step = 1;
				}
				BESTFITOBJECT->BestFitCurve(pts, AllPointsCollection.size(), tolerance, tolerance, &curveLst, step);
				/*Convert to Rapid-I shape type and add to the tempShape Collection*/
				Curve_2DList::iterator curvelst_iter;
				int curve_index = 0;
				for (curvelst_iter = curveLst.begin(); curvelst_iter != curveLst.end(); curvelst_iter++)
				{
					PointCollection PtColl;
					curve_index = curvelst_iter->curve_index; 
					PtsList pts_list = curvelst_iter->pts_list;
					for (PtsList::iterator pts_iter = pts_list.begin(); pts_iter != pts_list.end(); pts_iter++)
					{
						PtColl.Addpoint(new SinglePoint(pts_iter->x , pts_iter->y, MAINDllOBJECT->getCurrentDRO().getZ()));
					}
					if(curvelst_iter->curve == Curves_2D::line) // Add new Line..
					{
						newShape = new Line();
					}
					else 
					{
						newShape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
						((Circle*)newShape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
					}

					TempShapeCollection2[curve_index] = newShape;
				
					((ShapeWithList*)newShape)->AddPoints(&PtColl);

					curve_index++;
				}
				curveLst.clear();
				free (pts);
			}

			if (!getCleanShapes(TempShapeCollection2, &TempShapeCollection, max_radius, min_angle_cutoff, noise, tolerance))
			{
				return;
			}
			//for (int i = 0; i < TempShapeCollection2.size(); i++)
			//	delete TempShapeCollection2[i];
			//TempShapeCollection2.clear(); //Added by Sathya on 7 March 2012
		
			//Changed by Sathya on 7 March 2012
			i = 0;
			while(TempShapeCollection2.size() != 0)
			{
				Shape* CShape = TempShapeCollection2.begin()->second;
				TempShapeCollection2.erase(i);
				delete CShape;
				i++;
			}
		}
			
		if(addShapes)
		{
			for (int i = 0; i < TempShapeCollection.size(); i++)
			{
				if (this->autothread == false)
					AddShapeAction::addShape(TempShapeCollection[i], false);
				fgTempCollection[i] = TempShapeCollection[i];
			}
		}
		if (this->shape_as_fasttrace)
			createfasttraceshapes();
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0010", __FILE__, __FUNCSIG__); }
}

void LineArcAction::createfasttraceshapes()
{
	try
	{
		double startpoint[2] = {0};
		double endpoints[4] = {0};
		double prevendpoints[4] = {0};
		double d1, d2, d3, d4, mind;

		for (int i =0; i < TempShapeCollection.size(); i++)
		{
			TempShapeCollection[i]->ShapeAsfasttrace(false);	
		}
		for (int i =0; i < TempShapeCollection.size(); i++)
		{
			(TempShapeCollection[i])->clearFgNearShapes();
			((ShapeWithList*)(TempShapeCollection[i]))->UpdateShape();
		}
		for (int i =0; i < TempShapeCollection.size(); i++)
		{
			memcpy(prevendpoints, endpoints, 4*sizeof(double));
			memset(endpoints, 0, 4*sizeof(double));
			if (TempShapeCollection[i]->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				((Line*)TempShapeCollection[i])->getPoint1()->FillDoublePointer(endpoints);
				((Line*)TempShapeCollection[i])->getPoint2()->FillDoublePointer(endpoints + 2);
			}
			else
			{
				((Circle*)TempShapeCollection[i])->getendpoint1()->FillDoublePointer(endpoints);
				((Circle*)TempShapeCollection[i])->getendpoint2()->FillDoublePointer(endpoints + 2);
			}
			d1 = RMATH2DOBJECT->Pt2Pt_distance(endpoints[0], endpoints[1], prevendpoints[0], prevendpoints[1]);
			d2 = RMATH2DOBJECT->Pt2Pt_distance(endpoints[2], endpoints[3], prevendpoints[0], prevendpoints[1]);
			d3 = RMATH2DOBJECT->Pt2Pt_distance(endpoints[0], endpoints[1], prevendpoints[2], prevendpoints[3]);
			d4 = RMATH2DOBJECT->Pt2Pt_distance(endpoints[2], endpoints[3], prevendpoints[2], prevendpoints[3]);
			mind = d1;
			if (d2 < mind)
				mind = d2;
			if (d3 < mind)
				mind = d3;
			if (d4 < mind)
				mind = d4;

			if (i > 0)
			{
				if (TempShapeCollection[i]->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					if (mind == d1)
					{
						((Line*)TempShapeCollection[i])->setPoint1(Vector(prevendpoints[0], prevendpoints[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[2]; startpoint[1] = prevendpoints[3];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Line*)TempShapeCollection[i])->setPoint2(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
					else if (mind == d2)
					{
						((Line*)TempShapeCollection[i])->setPoint2(Vector(prevendpoints[0], prevendpoints[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[2]; startpoint[1] = prevendpoints[3];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Line*)TempShapeCollection[i])->setPoint1(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
					else if (mind == d3)
					{
						((Line*)TempShapeCollection[i])->setPoint1(Vector(prevendpoints[2], prevendpoints[3], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[0]; startpoint[1] = prevendpoints[1];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Line*)TempShapeCollection[i])->setPoint2(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
					else
					{
						((Line*)TempShapeCollection[i])->setPoint2(Vector(prevendpoints[2], prevendpoints[3], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[0]; startpoint[1] = prevendpoints[1];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Line*)TempShapeCollection[i])->setPoint1(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
				}
				else
				{
					if (mind == d1)
					{
						((Circle*)TempShapeCollection[i])->setendpoint1(Vector(prevendpoints[0], prevendpoints[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[2]; startpoint[1] = prevendpoints[3];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Circle*)TempShapeCollection[i])->setendpoint2(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
					else if (mind == d2)
					{
						((Circle*)TempShapeCollection[i])->setendpoint2(Vector(prevendpoints[0], prevendpoints[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[2]; startpoint[1] = prevendpoints[3];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Circle*)TempShapeCollection[i])->setendpoint1(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
					else if (mind == d3)
					{
						((Circle*)TempShapeCollection[i])->setendpoint1(Vector(prevendpoints[2], prevendpoints[3], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[0]; startpoint[1] = prevendpoints[1];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Circle*)TempShapeCollection[i])->setendpoint2(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
					else
					{
						((Circle*)TempShapeCollection[i])->setendpoint2(Vector(prevendpoints[2], prevendpoints[3], MAINDllOBJECT->getCurrentDRO().getZ()));
						if (i == 1)
						{
							startpoint[0] = prevendpoints[0]; startpoint[1] = prevendpoints[1];
						}
						if ((i == TempShapeCollection.size()-1) && (this->closed_loop))
						{
							((Circle*)TempShapeCollection[i])->setendpoint1(Vector(startpoint[0], startpoint[1], MAINDllOBJECT->getCurrentDRO().getZ()));
						}
					}
				}
			}	

			TempShapeCollection[i]->ShapeAsfasttrace(true);
			if (i < TempShapeCollection.size() - 1)
			{
				TempShapeCollection[i]->addFgNearShapes(TempShapeCollection[i+1]);
				TempShapeCollection[i+1]->addFgNearShapes(TempShapeCollection[i]);
			}
			if (i == TempShapeCollection.size()-1)
			{
				if (this->closed_loop)
				{
					TempShapeCollection[TempShapeCollection.size()-1]->addFgNearShapes(TempShapeCollection[0]);
					TempShapeCollection[0]->addFgNearShapes(TempShapeCollection[TempShapeCollection.size()-1]);
				}
			}
		}
		for (int i =0; i < TempShapeCollection.size(); i++)
		{
			((ShapeWithList*)(TempShapeCollection[i]))->UpdateShape();
		}
	}
	catch(...) {  MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0010", __FILE__, __FUNCSIG__); }
}

bool LineArcAction::mergeShapes(bool line_arc)
{
	try
	{
		if (this->autothread) return false;
		if(FramegrabactionAdded) return false;
		
		MergeShapesAction* msa = new MergeShapesAction(line_arc, closed_loop, shape_as_fasttrace);
		MAINDllOBJECT->addAction(msa);
		//RCollectionBase& selshapes = MAINDllOBJECT->getSelectedShapesList();
		//RCollectionBase& Sshapes = MAINDllOBJECT->getShapesList();
		//int cnt = TempShapeCollection.size();
		//bool found_shape = false;
		//map <int, int> int_array;
		//map <int, int> int_array2;
		//int a = 0;
		//Shape* tmpShape;	
		//int index = -1;
		//map <int, Shape*> fgTempCollection2;

		//if (selshapes.count() == 0) 
		//{
		//	MAINDllOBJECT->dontUpdateGraphcis = false;
		//	return false;
		//}

		//for (RC_ITER shape_iter = selshapes.getList().begin(); shape_iter != selshapes.getList().end(); shape_iter++)
		//{
		//	tmpShape = (Shape*) ((*shape_iter).second);
		//	found_shape = false;
		//	for (int i = 0; i < cnt; i++)
		//	{
		//		if (TempShapeCollection[i] != tmpShape) 
		//			continue;
		//		else
		//		{
		//			found_shape = true;
		//			int_array[a] = i;
		//			break;
		//		}
		//	}
		//	//if selected shape not in temporary collection return false
		//	if (found_shape == false) 
		//	{
		//		MAINDllOBJECT->dontUpdateGraphcis = false;
		//		return false;
		//	}
		//	a++;
		//}
		//int b = 0;
		//for (int i = 0; i < cnt; i++)
		//{
		//	index = -1;
		//	for (RC_ITER shape_iter = selshapes.getList().begin(); shape_iter != selshapes.getList().end(); shape_iter++)
		//	{
		//		tmpShape = (Shape*) ((*shape_iter).second);
		//		if (fgTempCollection[i] == tmpShape)
		//		{
		//			index = i;
		//			break;
		//		}
		//	}
		//	if (index == -1)
		//	{
		//		fgTempCollection2[b] = fgTempCollection[i];
		//		b++;
		//	}
		//}

		//int max_index = int_array[0];
		//int min_index = int_array[0];
		//int max_index1 = -1;
		//int min_index1 = 0;
		//int size = int_array.size();
		//for (int a = 0; a < size; a++)
		//{
		//	if (int_array[a] > max_index)
		//		max_index = int_array[a];
		//	if (int_array[a] < min_index)
		//		min_index = int_array[a];
		//}
		////if selected shapes are not neighboring, return false
		//if (max_index - min_index + 1 != size ) 
		//{
		//	if (!this->closed_loop)
		//	{
		//		MAINDllOBJECT->dontUpdateGraphcis = false;
		//		return false;
		//	}
		//	else
		//	{			
		//		int b = 0;
		//		for (int i = 0; i < cnt; i++)
		//		{
		//			for (int a = 0; a < int_array.size(); a++)
		//			{
		//				if ( i == int_array[a])
		//				{
		//					int_array2[b] = i;
		//					b++;
		//					break;
		//				}
		//			}
		//		}
		//		for (int i = 0; i < int_array2.size(); i++)
		//		{
		//			if (int_array2[i] != i)
		//			{
		//				max_index1 = i - 1;
		//				min_index1 = int_array2[i];
		//				break;
		//			}
		//		}
		//		//this is to check for case where the neighboring shapes are across the ends of the closed loop
		//		if (max_index1 + cnt - min_index1 + 1 != size)
		//		{
		//			MAINDllOBJECT->dontUpdateGraphcis = false;
		//			return false;
		//		}
		//	}
		//}

		//map <int, Shape*> TempShapeCollection2;
		//if (min_index < min_index1)
		//	min_index = min_index1;
		//for (int i = max_index1 + 1; i < min_index; i++)
		//{
		//	TempShapeCollection2[i - max_index1 - 1] = TempShapeCollection[i];
		//}
		//fgTempCollection.clear();
		//Shape* newshape;
		//if (line_arc)
		//{
		//	newshape = new Line();
		//}
		//else
		//{
		//	newshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
		//	((Circle*)newshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
		//}
		//TempShapeCollection2[min_index - max_index1 - 1] = newshape;
		//AddShapeAction::addShape(newshape, false);
		//for (int i = min_index; i <= max_index; i++)
		//{
		//	((ShapeWithList*) newshape)->AddPoints(&((ShapeWithList*)TempShapeCollection[i])->PointsList);
		//	//Sshapes.removeItem(TempShapeCollection[i]);
		//}
		////merge shapes till max_index1 to take care of closed loop case.
		//for (int i = 0; i < max_index1 + 1; i++)
		//{
		//	((ShapeWithList*) newshape)->AddPoints(&((ShapeWithList*)TempShapeCollection[i])->PointsList);
		//}

		//for (int i = max_index + 1; i < cnt; i++)
		//{
		//	TempShapeCollection2[i - size - max_index1] = TempShapeCollection[i];
		//}
		//
		//for (int i = 0; i < fgTempCollection2.size(); i++)
		//	fgTempCollection[i] = fgTempCollection2[i];
		//fgTempCollection2.clear();
		//fgTempCollection[cnt - size] = newshape;

		//Sleep(20);
		//MAINDllOBJECT->Wait_SaveActionsWorkSpace();
		//Sleep(10);
		//MAINDllOBJECT->deleteForEscape();
		////now populate TempShapeCollection back with merged shape list
		//TempShapeCollection.clear();
		//cnt = TempShapeCollection2.size();
		//for (int i = 0; i < cnt; i++)
		//	TempShapeCollection[i] = TempShapeCollection2[i];

		return true;
	}
	catch(...) {MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0009", __FILE__, __FUNCSIG__); return false;}
}

bool LineArcAction::determineFrameGrabs()
{
	try
	{
		//populate FrameGrabCollection using TempShapeCollection
		//for each shape, get mid point on the shape and length of shape.
		//create a framegrab centered at the mid point  
		//(angular rect at same slope if line and circular rect with same radius if arc).
		//the extremeties of the framegrab would be at distance from the mid point of 70% of 
		//minimum of distance of video extreme point along shape and actual extreme point of shape
		//from the mid point.
		//if length of shape is greater than 1.5 times length of first framegrab, create two more 
		//framegrabs by moving along shape to 70% of distance of extreme points from mid point.
		//Using each of these positions as mid points, create framegrabs similar to first.
		//Associate these framegrabs (one or three in number) with the given shape.
		//The FrameGrabCollection created would be a list of such lists for the entire shape collection
		if (this->autothread) return false;
		map<int, FramegrabBase*> baseFGColl;
		FramegrabBase* baseFG;
		int cnt = fgTempCollection.size();
		bool camconn = MAINDllOBJECT->CameraConnected();
		bool profon = MAINDllOBJECT->getProfile();
		bool surfon = MAINDllOBJECT->getSurface();
		int surffgtype = MAINDllOBJECT->SurfaceFgtype();
		std::string lightprop;
		std::string camprop;
		RapidEnums::RAPIDFGACTIONTYPE CurrentFGType;
		Vector* mid_pt;
		Vector extr_mid_pt[2];
		Vector pts[3];
		Vector pixel_pts[3];
		bool need_3_fg;
		double length = 0;
		double trunc_shape_length = 0;
		double windim = MAINDllOBJECT->getWindow(0).getWinDim().y * MAINDllOBJECT->getWindow(0).getUppY();
		double uppx = MAINDllOBJECT->getWindow(0).getUppX();
		double uppy = MAINDllOBJECT->getWindow(0).getUppY();
		double angle = 0;
		double Midangle = 0;
		double angle1, angle2;
		double extrangle1, extrangle2;
		double y_win = MAINDllOBJECT->getWindow(0).getWinDim().y;
		double x_win = MAINDllOBJECT->getWindow(0).getWinDim().x;

		if (camconn)
		{
			lightprop = MAINDllOBJECT->getLightProperty();
			camprop = MAINDllOBJECT->getCamSettings();
		}
		Shape* Cshape;
		for (int i =0; i < cnt; i++)
		{
			baseFGColl.clear();
			Cshape = fgTempCollection[i];
			if(Cshape->getMchild().size() > 0 || Cshape->getChild().size() > 0)
			{
				if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
				{
					if (this->pttype == LineArcAction::ptscollectiontype::EDGE)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB;
					else if (this->pttype == LineArcAction::ptscollectiontype::FOCUS)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH;
					else if (this->pttype == LineArcAction::ptscollectiontype::PROBE)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT;
					else if (this->pttype == LineArcAction::ptscollectiontype::CROSSHAIR)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR;
					mid_pt = ((Circle*) (Cshape))->getMidPoint();
					length = ((Circle*) (Cshape))->Length();
				}
				else
				{
					if (this->pttype == LineArcAction::ptscollectiontype::EDGE)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB;
					else if (this->pttype == LineArcAction::ptscollectiontype::FOCUS)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH;
					else if (this->pttype == LineArcAction::ptscollectiontype::PROBE)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT;
					else if (this->pttype == LineArcAction::ptscollectiontype::CROSSHAIR)
						CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR;
					mid_pt = ((Line*) (Cshape))->getMidPoint();
					length = ((Line*) (Cshape))->Length();
				}
				if (windim < length)
					trunc_shape_length = windim;
				else
					trunc_shape_length = length;
				if (Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
					if(1.5 * trunc_shape_length > length && 2 * length > windim)
						trunc_shape_length /= 4;

				if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
				{	
					//70% of length on either side of mid point
					angle = trunc_shape_length * 0.5 * 0.7 / length;
					Midangle = ((Circle*) (Cshape))->Startangle() + ((Circle*) (Cshape))->Sweepangle()/2;
					angle1 = Midangle + angle * ((Circle*) (Cshape))->Sweepangle();
					pts[0].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(angle1), 
									((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(angle1), ((Circle*) (Cshape))->getCenter()->getZ());
					angle2 = Midangle - angle * ((Circle*) (Cshape))->Sweepangle();
					pts[1].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(angle2), 
									((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(angle2), ((Circle*) (Cshape))->getCenter()->getZ());
					pts[2].set(*mid_pt);

					pixel_pts[0].set((((Circle*) (Cshape))->Radius() * cos(angle1) - ((Circle*) (Cshape))->Radius() * cos(Midangle)) / uppx + x_win/2,
										-(((Circle*) (Cshape))->Radius() * sin(angle1) - ((Circle*) (Cshape))->Radius() * sin(Midangle)) / uppy + y_win/2);
					pixel_pts[1].set((((Circle*) (Cshape))->Radius() * cos(angle2) - ((Circle*) (Cshape))->Radius() * cos(Midangle)) / uppx + x_win/2,
										-(((Circle*) (Cshape))->Radius() * sin(angle2) - ((Circle*) (Cshape))->Radius() * sin(Midangle)) / uppy + y_win/2);
					pixel_pts[2].set(x_win/2, y_win/2);
				}
				else
				{
					//70% of length on either side
					pts[0].set(mid_pt->getX() - cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7,
								mid_pt->getY() - sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7, mid_pt->getZ());
					pts[1].set(mid_pt->getX() + cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7,
								mid_pt->getY() + sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7, mid_pt->getZ());
					pts[2].set(0, 0, 0);

					pixel_pts[0].set(- cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppx + x_win/2, 
										 sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppy + y_win/2);
					pixel_pts[1].set( cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppx + x_win/2,
										-sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppy + y_win/2);
					pixel_pts[2].set(x_win/2, y_win/2);
				}
				baseFG = new FramegrabBase(CurrentFGType);
				baseFG->PointDRO.set(*mid_pt);
				if(camconn)
				{
					baseFG->lightProperty = lightprop;
					baseFG->camProperty = camprop;
					baseFG->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
				}
				baseFG->ProfileON = profon;
				baseFG->SurfaceON = surfon;
				baseFG->Surfacre_FrameGrabtype = surffgtype;
				for(int j = 0; j < 3; j++)
				{
					baseFG->points[j].set(pts[j]);
					baseFG->myPosition[j].set(pixel_pts[j]);
				}	
				baseFGColl[0] = baseFG;
				need_3_fg = false;
				if (length > 1.5 * trunc_shape_length) need_3_fg = true;
				if (need_3_fg)
				{
					//get two more framegrabs at each of the extremes.
					if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
					{
						angle = (length - trunc_shape_length) * 0.5 * 0.7 / length;
						extrangle1 = Midangle + angle * ((Circle*) (Cshape))->Sweepangle();
						extr_mid_pt[0].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(extrangle1), 
										((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(extrangle1), ((Circle*) (Cshape))->getCenter()->getZ());
						extrangle2 = Midangle - angle * ((Circle*) (Cshape))->Sweepangle();
						extr_mid_pt[1].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(extrangle2), 
										((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(extrangle2), ((Circle*) (Cshape))->getCenter()->getZ());
					}
					else
					{
						//modifying this so that the end points are at extreme frames - May 20 2012
						extr_mid_pt[0].set(mid_pt->getX() - cos(((Line*) (Cshape))->Angle()) * (length - trunc_shape_length) * 0.5,
								mid_pt->getY() - sin(((Line*) (Cshape))->Angle()) * (length - trunc_shape_length) * 0.5, mid_pt->getZ());
						extr_mid_pt[1].set(mid_pt->getX() + cos(((Line*) (Cshape))->Angle()) * (length - trunc_shape_length) * 0.5,
								mid_pt->getY() + sin(((Line*) (Cshape))->Angle()) * (length - trunc_shape_length) * 0.5, mid_pt->getZ());
					}
					baseFG = new FramegrabBase(CurrentFGType);
					baseFG->PointDRO.set(extr_mid_pt[0]);
					if(camconn)
					{
						baseFG->lightProperty = lightprop;
						baseFG->camProperty = camprop;
						baseFG->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
					}
					baseFG->ProfileON = profon;
					baseFG->SurfaceON = surfon;
					baseFG->Surfacre_FrameGrabtype = surffgtype;
					if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
					{	
						//70% of length on either side of mid point
						angle = trunc_shape_length * 0.5 * 0.7 / length;
						angle1 = extrangle1 + angle * ((Circle*) (Cshape))->Sweepangle();
						pts[0].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(angle1), 
										((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(angle1), ((Circle*) (Cshape))->getCenter()->getZ());
						angle2 = extrangle1 - angle * ((Circle*) (Cshape))->Sweepangle();
						pts[1].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(angle2), 
										((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(angle2), ((Circle*) (Cshape))->getCenter()->getZ());
						pts[2].set(extr_mid_pt[0]);

						pixel_pts[0].set((((Circle*) (Cshape))->Radius() * cos(angle1) - ((Circle*) (Cshape))->Radius() * cos(extrangle1)) / uppx + x_win/2,
											-(((Circle*) (Cshape))->Radius() * sin(angle1) - ((Circle*) (Cshape))->Radius() * sin(extrangle1)) / uppy + y_win/2);
						pixel_pts[1].set((((Circle*) (Cshape))->Radius() * cos(angle2) - ((Circle*) (Cshape))->Radius() * cos(extrangle1)) / uppx + x_win/2,
											-(((Circle*) (Cshape))->Radius() * sin(angle2) - ((Circle*) (Cshape))->Radius() * sin(extrangle1)) / uppy + y_win/2);
						pixel_pts[2].set(x_win/2, y_win/2);
					}
					else
					{
						//70% of length on either side
						pts[0].set(extr_mid_pt[0].getX() - cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7,
									extr_mid_pt[0].getY() - sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7, extr_mid_pt[0].getZ());
						pts[1].set(extr_mid_pt[0].getX() + cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7,
									extr_mid_pt[0].getY() + sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7, extr_mid_pt[0].getZ());
						pts[2].set(0, 0, 0);

						pixel_pts[0].set(- cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppx + x_win/2, 
											 sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppy + y_win/2);
						pixel_pts[1].set( cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppx + x_win/2,
											-sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppy + y_win/2);
						pixel_pts[2].set(x_win/2, y_win/2);
					}
					for(int j = 0; j < 3; j++)
					{
						baseFG->points[j].set(pts[j]);
						baseFG->myPosition[j].set(pixel_pts[j]);
					}	
					baseFGColl[1] = baseFG;

					baseFG = new FramegrabBase(CurrentFGType);
					//get other extreme of fgTempCollection[i]
					baseFG->PointDRO.set(extr_mid_pt[1]);
					if(camconn)
					{
						baseFG->lightProperty = lightprop;
						baseFG->camProperty = camprop;
						baseFG->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
					}
					baseFG->ProfileON = profon;
					baseFG->SurfaceON = surfon;
					baseFG->Surfacre_FrameGrabtype = surffgtype;
					if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
					{	
						//70% of length on either side of mid point
						angle = trunc_shape_length * 0.5 * 0.7 / length;
						angle1 = extrangle2 + angle * ((Circle*) (Cshape))->Sweepangle();
						pts[0].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(angle1), 
										((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(angle1), ((Circle*) (Cshape))->getCenter()->getZ());
						angle2 = extrangle2 - angle * ((Circle*) (Cshape))->Sweepangle();
						pts[1].set(((Circle*) (Cshape))->getCenter()->getX() + ((Circle*) (Cshape))->Radius() * cos(angle2), 
										((Circle*) (Cshape))->getCenter()->getY() + ((Circle*) (Cshape))->Radius() * sin(angle2), ((Circle*) (Cshape))->getCenter()->getZ());
						pts[2].set(extr_mid_pt[1]);

						pixel_pts[0].set((((Circle*) (Cshape))->Radius() * cos(angle1) - ((Circle*) (Cshape))->Radius() * cos(extrangle2)) / uppx + x_win/2,
											-(((Circle*) (Cshape))->Radius() * sin(angle1) - ((Circle*) (Cshape))->Radius() * sin(extrangle2)) / uppy + y_win/2);
						pixel_pts[1].set((((Circle*) (Cshape))->Radius() * cos(angle2) - ((Circle*) (Cshape))->Radius() * cos(extrangle2)) / uppx + x_win/2,
											-(((Circle*) (Cshape))->Radius() * sin(angle2) - ((Circle*) (Cshape))->Radius() * sin(extrangle2)) / uppy + y_win/2);
						pixel_pts[2].set(x_win/2, y_win/2);
					}
					else
					{
						//70% of length on either side
						pts[0].set(extr_mid_pt[1].getX() - cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7,
									extr_mid_pt[1].getY() - sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7, extr_mid_pt[1].getZ());
						pts[1].set(extr_mid_pt[1].getX() + cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7,
									extr_mid_pt[1].getY() + sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7, extr_mid_pt[1].getZ());
						pts[2].set(0, 0, 0);

						pixel_pts[0].set(- cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppx + x_win/2, 
											 sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppy + y_win/2);
						pixel_pts[1].set( cos(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppx + x_win/2,
											-sin(((Line*) (Cshape))->Angle()) * trunc_shape_length * 0.5 * 0.7 / uppy + y_win/2);
						pixel_pts[2].set(x_win/2, y_win/2);
					}
					for(int j = 0; j < 3; j++)
					{
						baseFG->points[j].set(pts[j]);
						baseFG->myPosition[j].set(pixel_pts[j]);
					}	
					baseFGColl[2] = baseFG;
				}
			}
			FrameGrabCollection[i] = baseFGColl;
		}
		return true;
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0006", __FILE__, __FUNCSIG__); return false;}
}

void LineArcAction::addLineArcAction()
{
	try
	{
		if (this->autothread) return;
		int k = 0;
		ShapeWithList* s;
		map<int, FramegrabBase*> v;
		int cnt = fgTempCollection.size();
		if (FrameGrabCollection.size() != cnt) return;
		for (int i = 0; i < cnt; i++)
		{
			s = (ShapeWithList*) fgTempCollection[i];
			v = FrameGrabCollection[i];
			for (int j = 0; j < v.size(); j++)
			{
				if(!(v[j]->isLineArcFirstPtAction()))
				{
					AddPointAction *action = new AddPointAction(s, v[j]);
					action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDPOINTACTION;
					action->ActionStatus(true);
					s->PointAtionList.push_back(action);
				
					MAINDllOBJECT->addAction(action);
					PointActionList[k] = action;
					k++;
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0006", __FILE__, __FUNCSIG__); }
}

bool LineArcAction::chamferShapes(bool line_arc)
{
	try
	{
		//get selected shapes.  check for first and last shapes to be lines.  If not return false.
		//check for continuity of shapes.  if not return false.
		//merge all intermediate shapes (2nd to last but one) as either line or arc based on flag passed.
		//create SM_Chamfer_LineMeasureHandler or SM_Chamfer_ArcMeasureHandler object, populate the 3 parent shapes of this object
		//using the three shapes (first, last and the above merged shape) and call the function AddDistanceMeasurement.

		if (this->autothread) return false;
		if(FramegrabactionAdded) return false;
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->dontUpdateGraphcis = true;
		RCollectionBase& selshapes = MAINDllOBJECT->getSelectedShapesList();
		RCollectionBase& Sshapes = MAINDllOBJECT->getShapesList();
		int cnt = TempShapeCollection.size();
		bool found_shape = false;
		map <int, int> int_array;
		map <int, int> int_array2;
		int a = 0;
		Shape* tmpShape;	
		int index = -1;
		map <int, Shape*> fgTempCollection2;
		
		for (RC_ITER shape_iter = selshapes.getList().begin(); shape_iter != selshapes.getList().end(); shape_iter++)
		{
			tmpShape = (Shape*) ((*shape_iter).second);
			found_shape = false;
			for (int i = 0; i < cnt; i++)
			{
				if (TempShapeCollection[i] != tmpShape) 
					continue;
				else
				{
					found_shape = true;
					int_array[a] = i;
					break;
				}
			}
			//if selected shape not in temporary collection return false
			if (found_shape == false) 
			{
				MAINDllOBJECT->dontUpdateGraphcis = false;
				return false;
			}
			a++;
		}
		int b = 0;
		for (int i = 0; i < cnt; i++)
		{
			index = -1;
			for (RC_ITER shape_iter = selshapes.getList().begin(); shape_iter != selshapes.getList().end(); shape_iter++)
			{
				tmpShape = (Shape*) ((*shape_iter).second);
				if (fgTempCollection[i] == tmpShape)
				{
					index = i;
				}
				else continue;
			}
			if (index == -1)
			{
				fgTempCollection2[b] = fgTempCollection[i];
				b++;
			}
		}

		int max_index = int_array[0];
		int min_index = int_array[0];
		int max_index1 = -1;
		int min_index1 = 0;
		int size = int_array.size();
		for (int a = 0; a < size; a++)
		{
			if (int_array[a] > max_index)
				max_index = int_array[a];
			if (int_array[a] < min_index)
				min_index = int_array[a];
		}
		//if selected shapes are not neighboring, return false
		if (max_index - min_index + 1 != size ) 
		{
			if (!this->closed_loop)
			{
				MAINDllOBJECT->dontUpdateGraphcis = false;
				return false;
			}
			else
			{			
				int b = 0;
				for (int i = 0; i < cnt; i++)
				{
					for (int a = 0; a < int_array.size(); a++)
					{
						if ( i == int_array[a])
						{
							int_array2[b] = i;
							b++;
							break;
						}
					}
				}
				for (int i = 0; i < int_array2.size(); i++)
				{
					if (int_array2[i] != i)
					{
						max_index1 = i - 1;
						min_index1 = int_array2[i];
						break;
					}
				}
				//this is to check for case where the neighboring shapes are across the ends of the closed loop
				if (max_index1 + cnt - min_index1 + 1 != size)
				{
					MAINDllOBJECT->dontUpdateGraphcis = false;
					return false;
				}
			}
		}

		map <int, Shape*> inputshapeseq;
		map <int, Shape*> outputshapeseq;
		if (min_index < min_index1)
			min_index = min_index1;
		for (int i = min_index; i <= max_index; i++)
		{
			inputshapeseq[i-min_index] = TempShapeCollection[i];
		}
		for (int i = 0; i <= max_index1; i++)
		{
			inputshapeseq[max_index - min_index + i] = TempShapeCollection[i];
		}
		//pass shapes from min_index to max_index (inputshapeseq) and get a "cleaner" sequence of shapes (outputshapeseq)
		if (!getCleanShapes(inputshapeseq, &outputshapeseq))
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return false;
		}

		int outputseqsize = outputshapeseq.size();
		//if first and last shapes are not lines, return false
		if (outputshapeseq[0]->ShapeType != RapidEnums::SHAPETYPE::LINE) 
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return false;
		}
		if (outputshapeseq[outputseqsize-1]->ShapeType != RapidEnums::SHAPETYPE::LINE) 
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return false;
		}

		map <int, Shape*> TempShapeCollection2;

		for (int i = max_index1 + 1; i < min_index; i++)
		{
			TempShapeCollection2[i - max_index1 - 1] = TempShapeCollection[i];
		}

		//Added by Sathya on 7 March 2012
		Shape* CShape1 = outputshapeseq[0];
		Shape* CShape3 = outputshapeseq[outputseqsize - 1];
		outputshapeseq.erase(0); outputshapeseq.erase(outputseqsize - 1);
		
		//get the first of the chamfer shapes - line into the TempShapeCollection2 and fgTempCollection2
		AddShapeAction::addShape(CShape1, false);
		TempShapeCollection2[min_index - max_index1 - 1] = CShape1;
		fgTempCollection2[b] = CShape1;
		b++;

		//get the second of the chamfer shapes - line or arc into the TempShapeCollection2 and fgTempCollection2
		Shape* CShape2;
		if(line_arc)
			CShape2 = new Line();
		else
		{
			CShape2 = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
			((Circle*)CShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
		}
		TempShapeCollection2[min_index - max_index1] = CShape2;
		AddShapeAction::addShape(CShape2, false);
		/*PointCollection PtColl;
		for (int i = 1; i < outputseqsize -1; i++)
		{
			PtColl = ((ShapeWithList*) outputshapeseq[i])->PointsList;
			((ShapeWithList*) newshape)->AddPoints(&PtColl);
		}*/

		//Changed by Sathya o 7 March 2012 .. .create a copy / pass that list..It will create copy in shape
		//If we delete intermediate shapes then its pointcollection will be deleted.. eventhough PtColl will have collection null objects.
		//Since it is local, it will try to delete memory at the end of this function.. debug assertion bcoz of delete of null pointer!
		for (int i = 1; i < outputseqsize -1; i++)
			((ShapeWithList*)CShape2)->AddPoints(((ShapeWithList*)outputshapeseq[i])->PointsList);
		
		//Added by Sathya on 7 March 2012
		int tempcnt = 1;
		while(outputshapeseq.size() != 0)
		{
			Shape* CShape = outputshapeseq.begin()->second;
			outputshapeseq.erase(tempcnt);
			delete CShape;
			tempcnt++;
		}



		fgTempCollection2[b] = CShape2;
		b++;

		//get the third (last) of the chamfer shapes - line into the TempShapeCollection2 and fgTempCollection2
		AddShapeAction::addShape(CShape3, false);
		TempShapeCollection2[min_index + 1 - max_index1] = CShape3;
		fgTempCollection2[b] = CShape3;
		b++;

		for (int i = max_index + 1; i < cnt; i++)
		{
			TempShapeCollection2[i + 2 - size - max_index1] = TempShapeCollection[i];
		}
		
		//populate fgTempCollection back
		fgTempCollection.clear();
		for (int i = 0; i < fgTempCollection2.size(); i++)
			fgTempCollection[i] = fgTempCollection2[i];
		fgTempCollection2.clear();

		//remove shapes from main collection that have been replaced with chamfered shapes
		/*for (int i = min_index; i <= max_index; i++)
			Sshapes.removeItem(TempShapeCollection[i]);*/

		//Changed by Sathya on 7 March 2012
		Sleep(20);
		MAINDllOBJECT->Wait_SaveActionsWorkSpace();
		Sleep(10);
		MAINDllOBJECT->deleteShape();// deleteForEscape();


		//now populate TempShapeCollection back with merged shape list
		TempShapeCollection.clear();
		cnt = TempShapeCollection2.size();
		for (int i = 0; i < cnt; i++)
			TempShapeCollection[i] = TempShapeCollection2[i];

		if (this->shape_as_fasttrace)
			createfasttraceshapes();
		/*MouseHandler* p_mousehandler = NULL;
		if (line_arc)
		{
			p_mousehandler = new SM_Chamfer_LineMeasureHandler();
			((SM_Chamfer_LineMeasureHandler*) p_mousehandler)->call_chamfer_dist_measurement(outputshapeseq[0], newshape, outputshapeseq[outputseqsize - 1]);
		}
		else
		{
			p_mousehandler = new SM_Chamfer_ArcMeasureHandler();
			((SM_Chamfer_ArcMeasureHandler*) p_mousehandler)->call_chamfer_dist_measurement(outputshapeseq[0], newshape, outputshapeseq[outputseqsize - 1]);
		}*/ 

		
		//Changed by Sathya on 7 March 2012
		if(line_arc)
		{
			SM_Chamfer_LineMeasureHandler p_mousehandler(false);
			p_mousehandler.call_chamfer_dist_measurement(CShape1, CShape2, CShape3);
		}
		else
		{
			SM_Chamfer_ArcMeasureHandler p_mousehandler(false);
			p_mousehandler.call_chamfer_dist_measurement(CShape1, CShape2, CShape3);
		}
		inputshapeseq.clear();
		outputshapeseq.clear();
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->Shape_Updated();
		return true;
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0007", __FILE__, __FUNCSIG__); return false;}
}

bool LineArcAction::getCleanShapes(map<int, Shape*> inputshapeseq, map<int, Shape*>* p_outputshapeseq, double max_radius, double min_angle_cutoff, double noise_length, double tolerance)
{
	try
	{
		int cnt = inputshapeseq.size();
		Shape* Cshape;
		Shape* Cshape2;
		double radius = 0;

		map<int, Shape*> Shapecollection2;
		double length = 0;
		double length_0 = 0;
		int k = 0;
		int j = 0;
		Shape* newshape;
		Shape* tmpshape;

		//merge successive shapes where one of them is less than the noise_length
		for(int i = 0; i < cnt - 1; i++)
		{
			Cshape = inputshapeseq[i];

			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				length_0 = ((Circle*) Cshape)->Length();
			else
				length_0 = ((Line*) Cshape)->Length();
			j = i + 1;
			Cshape2 = inputshapeseq[j];

			if (Cshape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
				length = ((Circle*) Cshape2)->Length();
			else
				length = ((Line*) Cshape2)->Length();
			
			if (((length_0 < noise_length)||(Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC))
				&&((length < noise_length)||(Cshape2->ShapeType == RapidEnums::SHAPETYPE::ARC)))
			{
				newshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				((Circle*)newshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
			}
			else
			{
				if ((length_0 > noise_length)&&(Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC))
				{
					newshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
					((Circle*)newshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				}
				else
					newshape = new Line();
			}

			((ShapeWithList*)newshape)->AddPoints((((ShapeWithList*) Cshape)->PointsList));
			Cshape = newshape;

			while ((length < noise_length)||(length_0 < noise_length))
			{
				((ShapeWithList*) Cshape)->AddPoints((((ShapeWithList*) Cshape2)->PointsList));
				if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
					length_0 = ((Circle*) Cshape)->Length();
				else
					length_0 = ((Line*) Cshape)->Length();
				i++;
				j++;
				if (j == cnt) break;
				Cshape2 = inputshapeseq[j];
				
				if (Cshape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
					length = ((Circle*) Cshape2)->Length();
				else
					length = ((Line*) Cshape2)->Length();
			}
			if (tolerance !=0 )
			{
				if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				{
					tmpshape = new Line();
					((ShapeWithList*) tmpshape)->AddPoints((((ShapeWithList*) Cshape)->PointsList));
					if (((Line*)tmpshape)->Straightness() > tolerance)
					{
						delete tmpshape;
					}
					else
					{
						delete Cshape;
						Cshape = tmpshape;
					}
				}
			}
			Shapecollection2[k] = Cshape;
			k++;
		}

		if (j == cnt - 1)
		{
			Cshape = inputshapeseq[cnt - 1];

			newshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
			((Circle*)newshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;

			((ShapeWithList*)newshape)->AddPoints((((ShapeWithList*) Cshape)->PointsList));
			Cshape = newshape;

			Shapecollection2[k] = Cshape;
			k++;
		}

		cnt = Shapecollection2.size();
		int m = 0;
		//treat arcs greater than the max_radius as straight lines.
		for(int i = 0; i < cnt; i++)
		{
			Cshape = Shapecollection2[i];
			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				radius = ((Circle*) Cshape)->Radius();
				if (radius > max_radius)
				{
					Cshape2 = new Line();

					((ShapeWithList*)Cshape2)->AddPoints((((ShapeWithList*) Cshape)->PointsList));
					Cshape = Cshape2;
				}
			}
			(*p_outputshapeseq)[m] = Cshape;
			m++;
		}

		Shapecollection2.clear();
		cnt = p_outputshapeseq->size();
		k = 0;
		j = 0;
		//successive lines with slopes falling less than min_angle_cutoff away are treated as a single line
		for(int i = 0; i < cnt - 1; i++)
		{
			Cshape = (*p_outputshapeseq)[i];
			j = i + 1;
			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double slope = ((Line*) Cshape)->Angle();
				Cshape2 = (*p_outputshapeseq)[j];
				while (Cshape2->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					double slope2 = ((Line*) Cshape2)->Angle();
					double diff_slope = abs(slope - slope2);
					diff_slope -= int(diff_slope/M_PI) * M_PI;
					if (diff_slope > M_PI_2)
						diff_slope = M_PI - diff_slope;
					if (diff_slope > min_angle_cutoff) 
						break;

					((ShapeWithList*) Cshape)->AddPoints((((ShapeWithList*) Cshape2)->PointsList));
					i++;
					j++;
					if (j == cnt) break;
					Cshape2 = (*p_outputshapeseq)[j];
					slope = ((Line*) Cshape)->Angle();
				}
			}
			Shapecollection2[k] = Cshape;
			k++;
		}
		if (j == cnt - 1)
		{
			Shapecollection2[k] = (*p_outputshapeseq)[cnt  - 1];
			k++;
		}
		p_outputshapeseq->clear();
		for (int i = 0; i < Shapecollection2.size(); i++)
		{
			(*p_outputshapeseq)[i] = Shapecollection2[i];
		}
		Shapecollection2.clear();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEARC0007", __FILE__, __FUNCSIG__); return false;}
}


void LineArcAction::RemoveAshapeAndReArrange(map<int, Shape*>* Shapecoll, Shape* CShape)
{
	try
	{
		bool ShapeExists = false;
		int Tempcnt = 0;
		for(map<int, Shape*>::iterator itemSh = Shapecoll->begin(); itemSh != Shapecoll->end(); itemSh++)
		{
			Shape* CSh = itemSh->second;
			if(CSh->getId() == CShape->getId())
			{
				ShapeExists = true;
				break;
			}
			Tempcnt++;
		}
		if(ShapeExists)
		{
			Shapecoll->erase(Tempcnt);
			list<Shape*> tempShapecoll;
			for(map<int, Shape*>::iterator itemSh = Shapecoll->begin(); itemSh != Shapecoll->end(); itemSh++)
				tempShapecoll.push_back(itemSh->second);
			Tempcnt = 0;
			Shapecoll->clear();
			for each(Shape* CSh in tempShapecoll)
			{
				(*Shapecoll)[Tempcnt] = CSh;
				Tempcnt++;
			}
		}		
	}
	catch(...){}
}

void LineArcAction::itemAdded(BaseItem* item, Listenable* sender)
{
}
void LineArcAction::itemRemoved(BaseItem* item, Listenable* sender)
{
	try
	{
		RemoveAshapeAndReArrange(&TempShapeCollection, (Shape*)item);
		RemoveAshapeAndReArrange(&fgTempCollection, (Shape*)item);
	}
	catch(...){}
}
void LineArcAction::itemChanged(BaseItem* item, Listenable* sender)
{
}
void LineArcAction::selectionChanged(Listenable* sender)
{
}
void LineArcAction::deleteAll()
{
	try
	{
		int i = 0;
		while(TempShapeCollection.size() != 0)
		{
			Shape* CShape = TempShapeCollection.begin()->second;
			TempShapeCollection.erase(i);
			delete CShape;
			i++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0010", __FILE__, __FUNCSIG__); }
}