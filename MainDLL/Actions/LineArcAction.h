//Class to add the Line Arc action..
#pragma once
#include "RAction.h"
#include "AddPointAction.h"
#include "..\Shapes\CloudPoints.h"

class LineArcAction:
	public RAction, public CollectionListener
{
private: 
	map<int, AddPointAction*> PointActionList; //list of point actions created
	bool closed_loop;
	bool shape_as_fasttrace;
	
public:
	map<int, map<int, FramegrabBase*>> FrameGrabCollection; //FrameGrabCollection
	map<int, Shape*> TempShapeCollection; //TemporaryShapeCollection.
	map<int, Shape*> fgTempCollection; //TemporaryShapeCollection in the order of framegrabs
	bool autothread;
	bool FramegrabactionAdded;
	enum ptscollectiontype
	{
		EDGE,
		PROBE,
		FOCUS,
		CROSSHAIR
	}; 
	ptscollectiontype pttype;
	LineArcAction(bool auto_thread = false);
	virtual ~LineArcAction();

	//Virtual functions..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	//line-arc separation and generation of TempShapeCollection//
	void lineArcSep(Shape* CShape);
	void lineArcSep(PointCollection* PntCollection, double tolerance = 0.003, double max_dist_betn_neighboring_pts = 1000, bool sort = false, double max_radius = 2, double min_radius = 0.01, double min_angle_cutoff = M_PI/36, double noise = 0.009, bool addShapes = true, bool best_fit_curve = false, bool closed_loop = false, bool shape_as_fast_trace = false, bool join_all_pts = false);
	//merge highlighted shapes of TempShapeCollection if they are neighboring..//
	bool mergeShapes(bool line_arc);
	//determine frame grab base collection from TempShapeCollection//
	bool determineFrameGrabs();
	//Add the respective frame grab derived actions to the shapes of TempShapeCollection..//
	void addLineArcAction();
	//create chamfer shapes by merging all intermediate shapes into line or arc apart from the two end shapes..which should be lines
	bool chamferShapes(bool line_arc);
	//Partprogram read/ write...should be taken care by pointAction partprogram read/write//
	bool getCleanShapes(map<int, Shape*> inputshapeseq, map<int, Shape*>* p_outputshapeseq, double max_radius = 0.1, double min_angle_cutoff = M_PI/18, double noise_length = 0.1, double tolerance = 0);

	//iterate through TempShapeCollection and create fasttrace dependencies
	void createfasttraceshapes();

	void RemoveAshapeAndReArrange(map<int, Shape*>* Shapecoll, Shape* CShape);


	//Item added, itemchaged events...//
	virtual void itemAdded(BaseItem* item, Listenable* sender);
	virtual void itemRemoved(BaseItem* item, Listenable* sender);
	virtual void itemChanged(BaseItem* item, Listenable* sender);
	virtual void selectionChanged(Listenable* sender);
	void deleteAll();
};