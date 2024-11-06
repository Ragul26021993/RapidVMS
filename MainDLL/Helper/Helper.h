//To highlight the shape / measurements...//
#pragma once
#include "..\MainDLL.h"
#include "..\Engine\RCollectionBase.h"
#include "..\Engine\PointCollection.h"
#include "..\Shapes\Vector.h"
#include "math.h"

class Shape;			//implementing as this way to reduce build time
class DimBase;
class RAction;
class ShapeWithList;
class FramegrabBase;
class Line;
class Circle;
class RPoint;

class MAINDLL_API HelperClass
{
public:
	HelperClass();
	~HelperClass();		

	//Returns the nearest shape...//
	Shape* getNearestShapeOnVideo(RCollectionBase& shapes, double x, double y, double snapdist, bool includeselected = false);
	Shape* getNearestShape(RCollectionBase& shapes, double x, double y, double snapdist, bool includeselected = false);
	Shape* getNearestShape(RCollectionBase& shapes, double* Sline, double snapdist, bool includeselected = false);
	Shape* getNearestShape(std::list<Shape*> ShapeList,double x, double y, double snapdist);
	Shape* getNearestThreadShape(std::list<Shape*> ShapeList1, std::list<Shape*> ShapeList2,double x, double y, double snapdist, bool flag);
	double GetZlevelOfRcadWindow(RCollectionBase& shapes, double* Sline);
	Shape* getHighestZLevelShape( list<Shape*> *TempShapeList);
	int ShapePreference(RapidEnums::SHAPETYPE stype);
	//Returns the nearest measurements....//
	DimBase* getNearestmeasure(RCollectionBase& measure, double x, double y, int windowno);
	
	//Move the shape collections..
	void moveShapeCollection(std::list<Shape*> shapes, Vector &v, bool isNudge, double angle = 0);
	
	//Function to get the exents of selected shapes.              
	bool GetRcadWindowEntityExtents(double *Lefttop, double *Rightbottom);
	bool GetDxfWindowEntityExtents(double *Lefttop, double *Rightbottom);
	bool GetSelectedShapeExtents(RCollectionBase& shapes, double *Lefttop, double *Rightbottom);
	bool GetSelectedShapeExtents(std::list<Shape*> shapes, double *Lefttop, double *Rightbottom);
	bool GetMeasurementExtents(RCollectionBase& Measurements, double *Lefttop, double *Rightbottom);
	bool GetMeasurementExtents(std::list<DimBase*> Measurements, double *Lefttop, double *Rightbottom);
	
	//function to get the extent of line for 3D mode...........
	bool GetRcadWindowEntityExtentsfor3DMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	bool GetSelectedShapeExtents3DMode(double *transformMatrix, RCollectionBase& shapes, double *Lefttop, double *Rightbottom);
	bool GetSelectedShapeExtents3DMode(double *transformMatrix, std::list<Shape*> shapes, double *Lefttop, double *Rightbottom);

		
#pragma region function for zoom to extent with rotation........
	//Function to get the Rectangle Enclosing given arc
	void RectangleEnclosing_Arcfor3DMode(double *ArcDcs, double *center, double *endpt1, double *endpt2, double radius, double startang, double endangle, double *Lefttop, double *RightBottom);
#pragma endregion

	//Used for DMM Handling
	bool PointOnHighlightedShape(Shape* s1,double *point1, double *point2, double *rpoint,double *angle, double *intercept);
	bool PointOnHighlightedShapeTest(Shape* s1,double *point1, Shape* s2, double* point3);
	bool PerpendiculaIntersectiononLine(Shape* s1,double *point,double *rpoint);
	bool IntersectionPointOnHighlightedLine(Shape* s1,double angle, double intercept, double *rpoint);
	bool IntersectionPointOnHighlightedCircle(Shape* s1, double pointX, double pointY, double angle, double intercept, double *rpoint);

	bool ChecktheParallelism(Shape* s1, Shape* s2);
	bool CheckLinePerpendicularity(Shape* s1, Shape* s2);
	void AddGDnTReference(DimBase* dim, Shape* s);
	bool CheckShapeListCloseLoop(std::list<Shape*> *ShapeList);
	double CalculateAreaofShapeList(std::list<Shape*> *ShapeList);
	double CalculatePerimeterOfShapeList(std::list<Shape*> *ShapeList);
	void AddContinuityRaltion_DxfShapes(std::list<Shape*> *ShapeList);
	void AddFgShapeRelationShip(RCollectionBase& shapes, Shape* CFtshape);
	void CalculateAreaOfShapeUsingOneShot(Shape* CShape);

	bool GetAverageImageValue(int StartX, int StartY, int Length, int width, double* AverageValue);
	bool DoCurrentFrameGrab_Area(FramegrabBase* currentAction, double* Area, bool SetImage = true);
	bool DoCurrentFrameGrab_SurfaceOrProfile(FramegrabBase* currentAction, bool SurfaceFG, bool SetImage = true);
	bool DoCurrentFrameGrab_IdOdOrFull(FramegrabBase* currentAction);
	void AddFrameGabbedPoints(int NoOfPoints, PointCollection& CFgPointsList);
	void AddAllEdgePointCollection(int NoOfPoints, PointCollection& CFgPointsList);

	void getPointParam(Vector* CPoint, list<list<double>>* ShpParmPntr);
	bool getShapeDimension(Shape *s, list<list<double>>* ShpParmPntr);
	bool Co_OrdinateSystem_From_ThreeShape(list<list<double>> ShapesParam, double* Plane, double* Line, double* Point);
	bool UCSBasePlaneTransformM(list<list<double>> *ShapesParam, double* TransformMatrix, bool CheckForAxisFlip = true);

	void CalculateEndPoints_3DLineBestfit(int PointsCount, double* Points, double* ThreeDLine, double* EndPt1, double* EndPt2, double* Minr, double* Maxr);

	bool CheckParallismRelationBtn2Lines(Line *line1, Line *line2);
	bool CheckRelationBtn2Lines(Line *line1, Line *line2);

#pragma region Measurement Create Object functions
	//********2D Measurements: All Combinations.. Right click, Normal etc...**********//
	//Handling the Right click on a Point..Point Coordinates, Ordinate..
	DimBase* CreatePointDim(Vector* v1, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z);
	//Handling Right Click on Line Circle/Arc..
	DimBase* CreateRightClickDim2D(Shape* s1, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z, int Spcount = 0);
	//Handling the angularity of line. Concentricity of circle, TP etc..
	DimBase* CreateAngularityDim2D(Shape* s1, Shape* s2, Shape* s3, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Angle, double land, double x, double y, double z);
	//Distance between the points.

	DimBase* CreateTruePositionDim(Vector* v1, Shape* s1, Shape* s2, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Angle, double land, double x, double y, double z);

	DimBase* CreatePointDim2D(Vector* v1, Vector* v2, double x, double y, double z, bool projectionDist = false);
	//Measurement between two shapes...
	DimBase* CreateShapeToShapeDim2D(Shape* s1, Shape* s2, double x, double y, double z, bool AngleWithAxis = true);
	//Measurement between the point and a shape...//
	DimBase* CreatePointToShapeDim2D(Vector* v1, Shape* s1, double x, double y, double z);
	//PCD Measurements
	DimBase* CreatePCDMeasurements(Shape* s1, Shape* s2, Shape* s3, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z);

	//********3D Measurements: All Combinations.. Right click, Normal etc...**********//
	//Right click on line and Plane..
	DimBase* CreateRightClickDim3D(Shape* s1, RapidEnums::MEASUREMENTTYPE CurrentMtype, double x, double y, double z);
	DimBase* Create3DTruePositionForPoint(Vector* V1, Shape* s2, Shape* s3, Shape* s4, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Tposition_X, double Tposition_Y, double x, double y,double z);
	//Handling the angularity of line3D, Cylinder, Plane etc..
	DimBase* CreateAngularityDim3D(Shape* s1, Shape* s2, Shape* s3, Shape* s4, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Angle, double land, double x, double y,double z);
	//Point to Point Distance 3d
	DimBase* CreatePointDim3D(Vector* v1, Vector* v2, double x, double y, double z);
	//Point to shape distance 3D
	DimBase* CreatePointToShapeDim3D(Vector* v1, Shape* s1, double x, double y, double z);
	//Shape to Shape distance 3D
	DimBase* CreateShapeToShapeDim3D(Shape* s1, Shape* s2, double x, double y, double z);
	//Rotary Angle Measure.. Special tool for delphi..
	DimBase* CreateRotaryMeasurement(Shape* s1, Shape* s2, double x, double y, double z);
	//Digital Micrometer Measurement..
	DimBase* CreateDMMDim(Vector* v1, Vector* v2, double x, double y, double z);
	//Add gd&t reference count.
	void AddGDnTReferenceDatum(DimBase* dim, Shape* s);

	//Angle Measurements..
	DimBase* CreateAngleMeasurment(Shape* CShape1, Shape* CShape2, bool AddAction = true);
	void SetAngleMeasurementPosition(DimBase* CDim, double OffsetPos = 0);

	//Create Radius Measurement..
	DimBase* CreateRadiusMeasurement(Shape* CShape);
	void SetRadiusMeasurementPosition(DimBase* CDim);

	//Point to point linear w.r.t line
	DimBase* CreatePt2PtDist_WrtLine(Shape* CShape, Shape* v1Parent1, Shape* v1Parent2, Shape* v2Parent1, Shape* v2Parent2);
	DimBase* CreateLinetoLine_Dist(Shape* CShape1, Shape* CShape2, bool AddAction = true);

	void AddShapeMeasureRelationShip(Shape* CShape, DimBase* Cdim);
#pragma endregion

	void ClearAllFlags();
	
#pragma region Profile Scan

	RAction* ProfileScanLineArcAction;

	//Add line arc action .
	void AddLineArcAction_ProfileScan(bool auto_thread = false);
		//Merge Shapes..
	void MergeShapesOfLineArcAction(bool mergeasLine);
	void Chamfer_Line_Arc(bool linearc);

	void AddtheProfileScanShape();
	//profile scan method and profile scan points collection
	void ProfileScan(bool chk_for_cont = true);
	void ProfileScanContinue();

	bool CheckCurrentFocus_ProfileScan(int noOfPts = 400);
	bool skip_cont_constraint;
	void ProfileScanFinished(bool CompleteProfile = true);
	Vector* ProfileLastPtVector;
	int profileScanPtsCnt, SurfaceAlgoType, SurfaceAlgoParam[5];
	double profileScanPixelPt[4];
	bool profileScanEndPtPassed, PickEndPoint_ProfileScan;
	SinglePoint ProfileScanStart_Pt;
	SinglePoint ProfileScanPrevStart_Pt;
	SinglePoint ProfileScanEnd_Pt;
	SinglePoint ProfileScanPrevEnd_Pt;
	Vector ProfileScanFirstPt;
	Vector ProfileScanSecondPt;
	Vector ProfileScanDroPt;
	bool isFirstFrame, StartProfileScan, MultiFeatureScan, AutoProfileScanEnable;
	std::list<int> MultiFeatureShapeList;
	bool profile_scan_running;
	bool profile_scan_failed, profile_scan_ended, profile_scan_stopped, profile_scan_paused;
	int ProfileScanJumpParam;
	double CamSizeRatio; //W, CamSizeRatioH;
	RapidEnums::RAPIDPROFILESCANMODE CurrentProfScanMode;

	PointCollection CurrFramePointslist;
	PointCollection PrevFramePointslist;
	PointCollection tmpPointslist;
	int profile_scan_iter;
	int first_frame_pts_cnt;
	bool profile_scan_auto_thread;
	bool Scan_Complete_Profile;
	bool AutoScanDone;
	ShapeWithList* CurrentProfileCpShape;
	std::list<std::list<double>*>* ProbeMeasurelist;
	void GetMcsPointsList(PointCollection *UcsList, PointCollection *McsList);
	void GetMcsPoint(Vector *UcsList, Vector *McsList);
#pragma endregion

	#pragma region outermost shapes related functions
	int OutermostInnerMostCone(PointCollection *pointlist, double *coneParam, double *coneCenters, bool usevalidPointonly, bool outermost = true);
	int OutermostInnerMostCylinder(PointCollection *pointlist, double *cylinderParam, double *cylinderCenters, bool usevalidPointonly, bool outermost = true);
	int OutermostInnerMostCircle(PointCollection *pointlist, double *circleparam, bool usevalidPointonly, bool outermost = true);
	int OutermostInnerMostSphere(PointCollection *pointlist, double *sphereparam, bool usevalidPointonly, bool outermost = true);
	int OutermostInnerMostCircle3D(PointCollection *pointlist, double *circleparam, bool usevalidPointonly, bool outermost = true);
	int OutermostInnerMostPlane(PointCollection *pointlist, double *planeparam, bool usevalidPointonly, double *dir);
	int OutermostInnerMostLine3D(PointCollection *pointlist, double *lineparam, bool usevalidPointonly, double *dir);
	int OutermostInnerMostLine(PointCollection *pointlist, double *lineparam, bool usevalidPointonly, double *dir);
	int OutermostInnerMostTorus(PointCollection *pointlist, double *torusparam, bool usevalidPointonly, bool outermost);

#pragma endregion

#pragma region filter shapes related functions
	int FilteredCone(PointCollection *pointlist, double *coneParam, double *coneCenters, int multiFactor, int iterationCnt);
	int FilteredCylinder(PointCollection *pointlist, double *cylinderParam, double *cylinderCenters, int multiFactor, int iterationCnt);
	int FilteredCircle(PointCollection *pointlist, double *circleparam, int multiFactor, int iterationCnt);
	int FilteredTwoArc(PointCollection *pointlist, double *twoarcparam, int multiFactor, int iterationCnt);
	int FilteredSphere(PointCollection *pointlist, double *sphereparam, int multiFactor, int iterationCnt);
	int FilteredTorus(PointCollection *pointlist, double *torusparam, int multiFactor, int iterationCnt);
	int FilteredCircle3D(PointCollection *pointlist, double *circleparam, int multiFactor, int iterationCnt);
	int FilteredPlane(PointCollection *pointlist, double *planeparam, int multiFactor, int iterationCnt);
	int FilteredLine3D(PointCollection *pointlist, double *lineparam, int multiFactor, int iterationCnt);
	int FilteredLine(PointCollection *pointlist, double *lineparam, int multiFactor, int iterationCnt);
	
#pragma endregion

	void AddMeasurementForDelphiComponent(Line *pline, Circle *PCircle, RPoint *ppoint);

#pragma region  Automatic identical measurement type
	// write function to Auto measurement of identical shapes.........
	void AutoMeasurementFor_IdenticalShapes(std::list<double> *cord_list);
	bool ShowGraphicalRepresentaionforDeviation(double radius, double tolerance, bool showMeasurement);
	//get framgrab parameter.....
	void get_FramGrabParameter();
	//get Approach distance point for probe......
	void getApproachDistPoint(double *InitialPt, double *EndPt, double Approach_Dist, double *ans);
	void Create_AngleBisector(Shape* Shp1, Shape* Shp2);
	void create_3DLine(double *Point1, double *Point2, bool selected = true, bool whitecolor = true, bool DontSaveShapeInPartProgram = false);
	void Create2DLine(double *Point1, double *Point2, bool selected = true, bool whitecolor = true, bool DontSaveShapeInPartProgram = false);
	void CreateToolAxis(double *Point1, double *Point2);
	//dist, angle and intercept...
	bool GetLineParameter(double *Point1, double *point2, double *LineParam);
	bool RotationScanFlag;
	std::list<Shape*>::iterator  Shapeitr;
	std::list<BaseItem*>::iterator Framgrabitr;
	void getFramGrabParameter(Vector diff);
	void (CALLBACK * CreateFramGrabCallback)();
	
	Vector* SnapPt_Pointer;
	bool SnapPtForFixture, IsFixtureCallibration;
private:
	PointCollection Coord_points;
	list<BaseItem*> ActionList;
	int SptCount;	
	map<int, int> AutomeasureShape_relation;
	//get coordinate from list of coordinate......
	void getCoordinateFor_NextShape();
	//get shape from list of shapes.......
	void getShape_Parameter(bool firsttime);
	//create new framgrab for selected shape......
	void create_newFramgrab(bool firsttime);
#pragma endregion


};
