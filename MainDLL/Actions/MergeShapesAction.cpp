#include "StdAfx.h"
#include "MergeShapesAction.h"
#include "LineArcAction.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

MergeShapesAction::MergeShapesAction(bool ln_arc, bool cl_lp, bool shp_as_ft):RAction(_T("MerShapes"))
{
	try
	{
		cshape = NULL;
		line_arc = ln_arc;
		closed_loop = cl_lp;
		shape_as_fasttrace = shp_as_ft;
		this->CurrentActionType = RapidEnums::ACTIONTYPE::MERGESHAPEACTION;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0001", __FILE__, __FUNCSIG__); }
}

MergeShapesAction::~MergeShapesAction()
{
	try
	{
		Shape* csh;
		for (unsigned int i = 0; i < delshapes.size(); i++)
		{
			csh = delshapes[i];
			delete csh;
		}
		delshapes.clear();
		cshape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0002", __FILE__, __FUNCSIG__); }
}

bool MergeShapesAction::execute()
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->dontUpdateGraphcis = true;
		RCollectionBase& selshapes = MAINDllOBJECT->getSelectedShapesList();
		RCollectionBase& Sshapes = MAINDllOBJECT->getShapesList();
		if (HELPEROBJECT->ProfileScanLineArcAction == NULL)
			return false;
		int cnt = (((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->TempShapeCollection).size();
		for (int i =0; i < cnt; i++)
		{
			((((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->TempShapeCollection)[i])->IsValid(false);
			((((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->TempShapeCollection)[i])->ShapeAsfasttrace(false);
		}
		bool found_shape = false;
		map<int, int> int_array;
		map<int, int> int_array2;
		int a = 0;
		Shape* tmpShape;	
		int index = -1;
		map <int, Shape*> fgTempCollection2;

		if (selshapes.count() == 0) 
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return false;
		}

		for (RC_ITER shape_iter = selshapes.getList().begin(); shape_iter != selshapes.getList().end(); shape_iter++)
		{
			tmpShape = (Shape*) ((*shape_iter).second);
			found_shape = false;
			for (int i = 0; i < cnt; i++)
			{
				if ((((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->TempShapeCollection)[i] != tmpShape) 
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
				if ((((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection)[i] == tmpShape)
				{
					index = i;
					break;
				}
			}
			if (index == -1)
			{
				fgTempCollection2[b] = (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection)[i];
				b++;
			}
		}

		int max_index = int_array[0];
		int min_index = int_array[0];
		int max_index1 = -1;
		int min_index1 = 0;
		int size = int_array.size();
		for (a = 0; a < size; a++)
		{
			if (int_array[a] > max_index)
				max_index = int_array[a];
			if (int_array[a] < min_index)
				min_index = int_array[a];
		}
		//if selected shapes are not neighboring, return false
		if (max_index - min_index + 1 != size ) 
		{
			if (!closed_loop)
			{
				MAINDllOBJECT->dontUpdateGraphcis = false;
				return false;
			}
			else
			{			
				b = 0;
				for (int i = 0; i < cnt; i++)
				{
					for (unsigned int a = 0; a < int_array.size(); a++)
					{
						if ( i == int_array[a])
						{
							int_array2[b] = i;
							b++;
							break;
						}
					}
				}
				for (unsigned int i = 0; i < int_array2.size(); i++)
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

		map <int, Shape*> TempShapeCollection2;
		if (min_index < min_index1)
			min_index = min_index1;
		for (int i = max_index1 + 1; i < min_index; i++)
		{
			TempShapeCollection2[i - max_index1 - 1] = (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection)[i];
		}
		(((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection).clear();
		Shape* newshape;
		if (line_arc)
		{
			newshape = new Line();
		}
		else
		{
			newshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
			((Circle*)newshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
		}
		cshape = newshape;
		TempShapeCollection2[min_index - max_index1 - 1] = newshape;
		for (int i = max_index + 1; i < cnt; i++)
		{
			TempShapeCollection2[i - size - max_index1] = (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection)[i];
		}
		Sshapes.addItem(newshape);
		int z = 0;
		for (int i = min_index; i <= max_index; i++)
		{
			((ShapeWithList*) newshape)->AddPoints(((ShapeWithList*)((((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection)[i]))->PointsList);
			delshapes[z] = (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection)[i];
			z++;
		}
		//merge shapes till max_index1 to take care of closed loop case.
		for (int i = 0; i < max_index1 + 1; i++)
		{
			((ShapeWithList*) newshape)->AddPoints(((ShapeWithList*)((((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection)[i]))->PointsList);
			delshapes[z] = (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection)[i];
			z++;
		}
		while(selshapes.count() != 0)
		{
			RC_ITER i = selshapes.getList().begin();
			((Shape*)((*i).second))->IsValid(false);
			for each(BaseItem* act in ((ShapeWithList*)((*i).second))->PointAtionList)
				((AddPointAction*)act)->ActionStatus(false);
			Sshapes.removeItem(((*i).second));
		}

		for (unsigned int i = 0; i < fgTempCollection2.size(); i++)
		{
			(((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection)[i] = fgTempCollection2[i];
		}
		fgTempCollection2.clear();
		(((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection)[cnt - size] = newshape;

		//now populate TempShapeCollection back with merged shape list
		(((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection).clear();
		cnt = TempShapeCollection2.size();
		for (int i = 0; i < cnt; i++)
		{
			(((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection)[i] = TempShapeCollection2[i];
		}
		newshape->IsValid(false);

		if (shape_as_fasttrace)
			((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->createfasttraceshapes();
		MAINDllOBJECT->selectShape(cshape->getId(), false);
		for (int i =0; i < cnt; i++)
		{
			((((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->TempShapeCollection)[i])->IsValid(true);
		}
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->Shape_Updated();
		return true;
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0003", __FILE__, __FUNCSIG__); return false; }
}

bool MergeShapesAction::redo()
{
	try
	{
		if (execute())
			return true;
		else 
			return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0004", __FILE__, __FUNCSIG__); return false; }
}

void MergeShapesAction::undo()
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->dontUpdateGraphcis = true;
		RCollectionBase& Sshapes = MAINDllOBJECT->getShapesList();
		RCollectionBase& rActions = MAINDllOBJECT->getActionsHistoryList();

		if (HELPEROBJECT->ProfileScanLineArcAction == NULL)
			return;
		int cnt = ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.size();
		for (int i =0; i < cnt; i++)
		{
			((((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->TempShapeCollection)[i])->IsValid(false);
		}
		bool found_shape = false;
		int index_tempcoll = -1;
		map <int, Shape*> fgTempCollection2;

		if (cshape == NULL) 
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return;
		}

		found_shape = false;
		for (int i = 0; i < cnt; i++)
		{
			if (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection[i] != cshape) 
				continue;
			else
			{
				found_shape = true;
				index_tempcoll = i;
				break;
			}
		}
		//if added shape not in temporary collection return doing nothing
		if (found_shape == false) 
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return;
		}

		int b = 0;
		for (int i = 0; i < cnt; i++)
		{
			if (((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection[i] != cshape)
			{
				fgTempCollection2[b] = ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection[i];
				b++;
			}
		}

		map <int, Shape*> TempShapeCollection2;

		int i = 0;
		for (i = 0; i < index_tempcoll; i++)
		{
			TempShapeCollection2[i] = ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection[i];
		}
		((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection.clear();

		//clear existing selection before updating shapes in selected items list
		Sshapes.clearSelection();
		//add the deleted shapes to main collection, tempshapecollection and fgtempcollection
		for (unsigned int z = 0; z < delshapes.size(); z++)
		{
			TempShapeCollection2[i] = delshapes[z];
			TempShapeCollection2[i]->IsValid(false);
			Sshapes.addItem(TempShapeCollection2[i]);
			fgTempCollection2[b] = delshapes[z];
			(delshapes[z])->selected(false);
			MAINDllOBJECT->selectShape((delshapes[z])->getId(), true);
			i++;
			b++;
		}
		
		//clear deleted shapes list because they have been added to main collection
		delshapes.clear();

		//now add rest of shapes from temp collection to the list.
		for (int j = index_tempcoll + 1; j < cnt; j++)
		{
			TempShapeCollection2[i] = ((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection[j];
			i++;
		}
		
		//populate fgTempCollection
		for (unsigned int i = 0; i < fgTempCollection2.size(); i++)
			((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection[i] = fgTempCollection2[i];
		fgTempCollection2.clear();

		//populate TempShapeCollection 
		((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.clear();
		cnt = TempShapeCollection2.size();
		for (int i = 0; i < cnt; i++)
			((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection[i] = TempShapeCollection2[i];

		//remove the added shape since we have undone operation now.
		cshape->IsValid(false);
		for each(BaseItem* act in (((ShapeWithList*)(cshape))->PointAtionList))
			rActions.removeItem(act, true);
		Sshapes.removeItem(cshape, true);
		
		//update cshape to NULL since it contains nothing now
		cshape = NULL;	
		if (shape_as_fasttrace)
			((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->createfasttraceshapes();
		for (int i =0; i < cnt; i++)
		{
			((((LineArcAction*)(HELPEROBJECT->ProfileScanLineArcAction))->TempShapeCollection)[i])->IsValid(true);
		}
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDSH0005", __FILE__, __FUNCSIG__); }
}