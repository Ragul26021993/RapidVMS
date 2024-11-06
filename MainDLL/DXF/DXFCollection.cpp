#include "StdAfx.h"
#include "DXFCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
//Constructor...//
DXFCollection::DXFCollection(TCHAR *myname):BaseItem(myname)
{
	try{ dxfsnaps = new SnapPtManager(&(this->ShapeCollection)); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFC0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
DXFCollection::~DXFCollection()
{
	try
	{
		// Delete All Shape, Measurement and Intersection points..
		this->ShapeCollection.deleteAll();
		this->OriginalShapeCollection.deleteAll();
		this->MeasureCollection.deleteAll();
		this->OriginalMeasureCollection.deleteAll();
		this->getIPManager().getIsectPts()->deleteAll();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFC0002", __FILE__, __FUNCSIG__); }
}

//returns the shape collections...//
RCollectionBase& DXFCollection::getShapeCollection()
{
	return this->ShapeCollection;
}

//returns the shape collections...//
RCollectionBase& DXFCollection::getOriginalShapeCollection()
{
	return this->OriginalShapeCollection;
}

//Get the measurement collections...//
RCollectionBase& DXFCollection::getMeasureCollection()
{
	return this->MeasureCollection;
}

//Get the measurement collections...//
RCollectionBase& DXFCollection::getOriginalMeasureCollection()
{
	return this->OriginalMeasureCollection;
}

//get snap point collection...//
SnapPtManager& DXFCollection::getIPManager()
{
	return *(this->dxfsnaps);
}

void DXFCollection::ShapeArrangementForLoop()
{
	try
	{
		double *ShapePts;
		int *ShapeID, NoOfShapesCounter = 0, ShpItr = 0;
		std::list<std::list<int>*> ClusterList;
		std::list<std::list<int>*>::iterator ClusterLstItr;

		ShapePts = (double*)malloc(sizeof(double) * 4 * ShapeCollection.getList().size());
		ShapeID = (int*)malloc(sizeof(int) * ShapeCollection.getList().size());

		for(RC_ITER item = ShapeCollection.getList().begin();	item != ShapeCollection.getList().end(); item++)
		{
			Shape *TmpShape = ((Shape*)(*item).second);
			if(TmpShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				ShapePts[ShpItr * 4] = ((Line*)TmpShape)->getPoint1()->getX();
				ShapePts[ShpItr * 4 + 1] = ((Line*)TmpShape)->getPoint1()->getY();
				ShapePts[ShpItr * 4 + 2] = ((Line*)TmpShape)->getPoint2()->getX();
				ShapePts[ShpItr * 4 + 3] = ((Line*)TmpShape)->getPoint2()->getY();
				ShapeID[ShpItr] = ((Line*)TmpShape)->getId();
				ShpItr++;
			}
			else if(TmpShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				ShapePts[ShpItr * 4] = ((Circle*)TmpShape)->getendpoint1()->getX();
				ShapePts[ShpItr * 4 + 1] = ((Circle*)TmpShape)->getendpoint1()->getY();
				ShapePts[ShpItr * 4 + 2] = ((Circle*)TmpShape)->getendpoint2()->getX();
				ShapePts[ShpItr * 4 + 3] = ((Circle*)TmpShape)->getendpoint2()->getY();
				ShapeID[ShpItr] = ((Circle*)TmpShape)->getId();
				ShpItr++;
			}
		}
		RMATH3DOBJECT->ArrangeDXFShapesInOrder(ShapePts, ShapeID, ShpItr, &ClusterList);
		free(ShapePts);
		free(ShapeID);
	
		for each(std::list<int>* LoopSet in ClusterList)
		{
			list<Shape*> LoopShapesCollections;
			for each(int ID in *LoopSet)
			{
				for(RC_ITER item = ShapeCollection.getList().begin(); item != ShapeCollection.getList().end(); item++)
				{
					Shape* Csh = (Shape*)(*item).second;
					if(Csh->getId() == ID)
					{
						LoopShapesCollections.push_back(Csh);
						break;
					}
				}
			}
			ClosedLoopCollections.push_back(LoopShapesCollections);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFC0004", __FILE__, __FUNCSIG__); }
}