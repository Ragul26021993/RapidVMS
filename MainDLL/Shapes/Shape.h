//Shapes base class
//Inherits from drawable, itembase and related shape..//
#pragma once
//Include the headers files..//
#include "Vector.h"
#include "..\Engine\BaseItem.h"
#include "..\MainDLL.h"
#include "..\Helper\General.h"
#include "RelatedShape.h"

class MAINDLL_API Shape:
	public BaseItem, public RelatedShape
{

private:
	void init();

protected:
	static TCHAR name[10]; //Shape Name..//
	//To hold the enclosed rectangle of shape..No need to calculate for all shapes during a shape update.!
	double ShapeLeftTop[3], ShapeRightBottom[3], FgPointLeftTop[3], FgPointsRightBottom[3];

	//Virtual function.. draw: handles graphics function call for the shapes..
	virtual void drawCurrentShape(int windowno, double WPixelWidth) = 0;
	virtual void drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM);
	double IntersectnPts[6];
	int Intersects;
	RapidProperty<bool> SigmaMode;
public:
	//Constructor....
	Shape(TCHAR* myname);
	Shape(bool simply);
	virtual ~Shape();
	
//Virtual functions...
	//Function to move the shape..//
	virtual void Transform(double* transform) = 0;
	//Function to check the mousepointer distance to the shape...
	virtual bool Shape_IsInWindow(Vector& corner1, double Tolerance) = 0;
	virtual bool Shape_IsInWindow(Vector& corner1, double Tolerance, double* TransformM);
	//Function to check whether the shape inside the rectangle...
	virtual bool Shape_IsInWindow(Vector& corner1, Vector& corner2) = 0;
	//Function to check the shape is in window.. near to mouse position
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance) = 0;
	//To get the clone of the shape...//
	virtual Shape* Clone(int n, bool copyOriginalProperty = true) = 0;
	//To get the dummy Copy of the shape...//
	virtual Shape* CreateDummyCopy() = 0;
	//To copy the parameterised shape to current shape..//
	virtual void CopyShapeParameters(Shape*) = 0 ;
	//To update the Shape..
	virtual void UpdateBestFit() = 0;
	//draw gdnt Reference..
	virtual void drawGDntRefernce() = 0;
	//Function to traslate the shapes...
	virtual void Translate(Vector& Position) = 0;
	//To reset the shape parameters..
	virtual void ResetShapeParameters() = 0;
	//To transform Shapes in 3D.
	virtual void AlignToBasePlane(double* trnasformM) = 0;
	//To Update the Enclosed rectangle/ enclosed cube(for 3D)
	virtual void UpdateEnclosedRectangle() = 0;
	//To Check whether the shape is within view..
	virtual bool WithinCirclularView(double *SelectionLine, double Tolerance);
	//To get the Z level of the shape...
	virtual double GetZlevel();
	virtual void GetShapeCenter(double *cPoint);
	virtual void GetSelectedPointOnShape(double *IntersectPt, int *Count);
	//Function to Copy the original Parameters of the Passed shape to the called shape..
	void CopyOriginalProperties(Shape* Cshape);

	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom) = 0;
	//Retruns the shape extents..
	bool GetShapeExtents(double *Lefttop, double *Rightbottom);
	virtual void UpdateRefelctedShape(Shape *Rshape, double *reflectionSurfaceParam, bool reflectionAlongPlane);
	void RotateShape(double *rotateM, double * rotationOrigin);
	void TranslateShape(double *translateDir);
	void UpdateHomePostionFromReferenceDotShape(bool FirstReferenceDot = true);
	//draw Shape..
	void drawShape(int windowno, double WPixelWidth);
	void drawSelectedPointCrossMark(int windowno, double WPixelWidth);
	//returns shape Name..
	std::string getCurrentShapeTypeName();

	double getProbeRadius(int side);

	list<int> FtFgRelationshapes;
	list<int> siblingrelationships;
	
	RapidEnums::SHAPETYPE ShapeType;
//Properties.. Set and get Types...
	RapidProperty<bool> IsValid; // Valid/ Not
	RapidProperty<bool> ShapeAsfasttrace;
	RapidProperty<bool> ShowFGpointsflag;
	RapidProperty<bool> Normalshape;
	RapidProperty<bool> ShapeAsReferenceShape;
	RapidProperty<bool> ShapeAs3DShape;
	RapidProperty<bool> HighlightedFormouse;
	RapidProperty<bool> HighlightedForMeasurement;
	RapidProperty<bool> HighlightedForPCDHighLight;
	RapidProperty<bool> ShapeForCurrentFastrace;
	RapidProperty<bool> ShapeForThreadMeasurement;
	RapidProperty<bool> FinishedThreadMeasurement;
	RapidProperty<bool> ShapeDoneUsingProbe;
	RapidProperty<bool> ShapeForPerimeter;
	RapidProperty<bool> ShapeUsedForTwoStepHoming;
	RapidProperty<bool> ShapeAsRotaryEntity;
	RapidProperty<bool> DoneFrameGrab; // For the FG continuity..
	RapidProperty<bool> AppliedLineArcSeperation; //AppliedLineArcSeperation...
	RapidProperty<bool> AppliedLineArcSepOnce; //flag required to prevent dolinearcseparation function to go into infinite loop
	RapidProperty<bool> IgnoreManualCrosshairSteps; //Set to true Whenever Framegrab is done for the shape. To skip steps during partprogram..
	RapidProperty<bool> UseLightingProperties;
	RapidProperty<bool> PartialShape;
	RapidProperty<int> PartialShapeDrawMode;
	RapidProperty<int> RefernceDatumForgdnt;
	RapidProperty<int> FlatnessFilterFactor;
	RapidProperty<int> UcsId;
	RapidProperty<double> ShapeThickness; // To set and get the thickness of shape..
	RapidProperty<bool> IsHidden; 
	RapidProperty<bool> IsDxfShape;
	RapidProperty<bool> IsAlignedShape; 
	//RapidProperty<bool> IsUsedForReferenceDot;
	RapidProperty<bool> ProbeCorrection;
	RapidProperty<bool> XOffsetCorrection;
	RapidProperty<bool> YOffsetCorrection;
	RapidProperty<bool> ZOffsetCorrection;
	RapidProperty<bool> ROffsetCorrection;
	RapidProperty<bool> AddActionWhileEdit;
	RapidProperty<bool> ApplyMMCCorrection;
	RapidProperty<bool> ApplyFilterCorrection;
	RapidProperty<int> MMCCorrectionIterationCnt;
	RapidProperty<int> FilterCorrectionIterationCnt;
	RapidProperty<int> FilterCorrectionFactor;
	RapidProperty<bool> OuterCorrection;
	RapidProperty<bool> MMCSelectected;
	RapidProperty<bool> ScanDirDetected;
	RapidProperty<bool> ApplyPixelCorrection;

	RColor ShapeColor; // To set and get the Color property of shape..
	char DatumName;

	//Write and Read the common Properties of a Shape..
	friend wostream& operator<<(wostream& os, Shape& x);
	friend wistream& operator>>(wistream& is, Shape& x); 
	friend void ReadOldPP(wistream& is, Shape& x);
};