#pragma once
#include <map>
#include <list>
#include "..\MainDLL.h"
#include "..\Helper\General.h"
#include "..\Shapes\Vector.h"

class DXFCollection;
class Shape;
class ShapeWithList;
class DimBase;
class SinglePoint;
class RCollectionBase;
class RAction;
class DXFWriter;
class PointCollection;
class IGESWriter;
class STLWriter;

class MAINDLL_API DXFExpose
{
	
private:
	map<int, Shape*> ShapeColl1; //Shape collections..// For the DXF shape collection..!
	map<int, Shape*> ShapeColl2; //Shape collections..// For the DXF shape collection..!
	map<int, int> DxfRcadShapeRelation;

	double transform[9], transform3D[16];
	Vector userdefinedpt, spherecentrept, Translate_Vector;
	DXFCollection* Cdxf;

	//Function to export all the points to DXF
	void ExportPointsToDXF(char* filename, RCollectionBase& ShapeCollection);
	//Function to export all the points to text File
	void ExportPointsToText(char* filename, RCollectionBase& ShapeCollection);
	//Function to export shape parameters to csv File
	void ExportPointsToCsv(char* filename, RCollectionBase& ShapeCollection);

	void create_2DLine(double *Point1, double *Point2, bool selected = true, bool whitecolor = true, bool DoNotAddAction = false);
	//bool getExtremePointsforDxfEditedShape(double *LeftMost, double *RightMost);
	bool getExtremePointsforDxfEditedShape(double dist, std::list<double> *CorrectionX, std::list<double> *CorrectionY, std::list<double> *distList, double *LefMost, double *RightMost);
	bool getExtremePointsForCloudPoints(double *LeftMost, double *RightMost);
	void createtemporaryLine(std::list<double> *PtsList, Shape* CurrentSelectShape);
	void ExportDXFFile(DXFWriter* writer, ShapeWithList* Cshape, int ColorCode, bool CloudPointAsPolyLine);
public:
	std::list<Shape*> CurrentDXFShapeList;	//holds all shapes and measurement from DXF file loaded.
	std::list<DimBase*> CurrentDXFMeasureList;	//holds all shapes and measurement from DXF file loaded.
	std::list<RAction*> CurrentDXFNonEditedActionList;

	std::map<int, Shape*> PPDXFShapeListOriginal, PPDXFShapeList;	

	map<int, SinglePoint*> CADcolors;
	bool LoadDXF, AlignFlag, DeviationWithLineOnly, ExportShapeAsZequaltoZero, ExportShapesOfAllUcsToDxf;
	std::string PointsDirectoryPath;
	RapidProperty<bool> AllPointsAsCloudPoint;
	RapidProperty<bool> AlignDxfShapes;
	RapidProperty<bool> DoThreeDAlignment;
	RapidProperty<bool> DxfShapeContinuity;

public:
	DXFExpose();
	~DXFExpose();
	//Delete selected DXF file.
	void deleteDXF(int id);
	//Select DXF file by passing ID.
	void selectDXF(int id);
	//Update DXf list..
	void UpdateForUCS();
	//Edit DXF file for Alignment.
	void editDXF();
	//Save DXF file by getting path.
	void ExportDXFFile(char* filename, bool CloudPointAsPolyLine);
	
	//Function to export all the points to DXF
	void ExportShapePointsToDXF(char* filename, bool OnlySelectedShapes);
	//Function to export all the points to text File
	void ExportShapePointsToText(char* filename, bool OnlySelectedShapes);
	//Function to export shape parameters to csv File
	void ExportShapePointsToCsv(char* filename, bool OnlySelectedShapes);
	
	void ExportShapeParametersToCsv(char* filename, bool OnlySelectedShapes);

	void WriteShapeParametersToCsv(std::wofstream& myfile, ShapeWithList *CShape);

	//Loading DXF file by getting path.
	void ImportDXFFile(char* filename, char* DxfFile, int DXFlength); // , bool ImportforFixedGraphicsDXF);
	//Reading the File and initilize shapes according to that.
	void AddDXFShapes(Shape* Obj);
	//Add DXF measurements...
	void AddDXFMeasurement(DimBase* Obj);
	//Function during Alignment button pressed.
	void AlginTheDXF(bool AlignmentCompleted = false);
	//Reset the Dxf align..
	void ResetDxfAlignment();
	//Function to Handle Transform Matrix.
	void TransformMatrix(double *transform, bool DxfAlign = true);
	//function to set translate vector....
	void SetTransVector(double *Pt1, double *Pt2, bool DxfAlign = true);
	void SetTransVector(Vector& TransVect, bool DxfAlign = true);
	//function to handle 3D transform matrix.
	void TransformMatrix3D_1Line(double *transform, double *translate, double *Dxfpt, bool DxfAlign = true);
	//Function for CAD alignment.
	void CADalignment(double *transform, bool flag = true);
	void EditedCadAlignment(double *transform);
	//function for translate CAD Shapes......
	void CADTranslate(Vector& TranslateVector);
	void EditedCadTranslate(Vector& TranslateVector);
	void EditedCadTransform(double *transform);
	void SetAlignedProperty(bool Aligned = true);
	//Set color for DXF shapes.
	void SetCADColors(int cnt, int r, int g, int b);

	//.drl NC Drill file import
	void ImportDRLFile(char* filename,char* DxfFile,int DXFlength);

	//Import iges File
	void ImportIGESFile(char* filename, char* igesFile, int filelength);
	void ExportIGESFile(char* filename);

	//import STL file..
	void ImportSTLFile(char* filename, char* igesFile, int filelength);
	void ExportSTLFile(char* filename);

	// STEP files
	void ImportSTEPFile(char* filename, char* igesFile, int filelength);

	void ImportNCFile(char* filename, char* NCFile, int fileLength);

	//show dxf deviation with respect to cloud points....
	void ShowDeviationwithCloudPts();
	void ShowDeviationwithCloudPts(double tolerance, double interval, int IntervalType, bool ClosedLoop);
	bool ShowDeviationwithCloudPts(double Uppertolerance, double Lowertolerance, double interval, std::list<int>* LineIdList, int IntervalType, bool TwoDdeviation, bool DeviationWithYellowShapes, int measureRefrenceAxis, bool BestfitArc);
	//function for autoAlignment...........
	void AutoAlignmentForDxf(std::list<double> *CorrectionX, std::list<double> *CorrectionY, std::list<double> *distList);
	//function for autoalignment using rbf function............trans = 0 for rotate and translate both, 1 for translate only, 2 for rotate only...
	void AutoAlignmentForSelectedShapes(bool SelectedPtOnly, int alignment_mode);
	Shape* getNearestShape_3D(double* pts, double* IntersectionPt, bool* returnFlag);
	Shape* getNearestShape_2D(double* pts, double* IntersectionPt, bool* returnFlag, bool finiteLine = true);
	int getNumberOfIntersectionPoints(RCollectionBase& ShapeCollection, double *RayPt);
	bool intersectionPointOnLine(double Pt1x, double Pt1y, double Pt2x, double Pt2y, double IntersectPtx, double IntersectPty);
	bool calculateIntersectionPtOnCircle(double* pt, Shape* s1, double slope, double intercept, double* ans);
	void CreatePointCsvFile(std::string filePath);
	void WriteToPointCsvFile(std::string filePath);
	void CreateShapesParamCsvFile(std::string filePath);
	void WriteToShapesParamCsvFile(std::string filePath);
	void WriteCamProfileDist_CsvFile(double angle, int PtsToBundle);
	void GethobCheckerPersonaldatain_CsvFile(int OrderNo, int SerialNo, int ToolID, char* CustomerName, char* Date, char* Operator, int MachineNo, bool IsHobTypeBore, char* AccuracyClass, double leadofGash, char* Module);   //22/02/2014
	//void Cam_profile_Circle_Centre(double x, double y, double z, bool Tool_type);				//11/02/2014 vinod.. for excel report we need here cam profile circle center..
	void Cam_profile_Circle_Centre(double x, double y, double z, bool Tool_type, double Point3DForZPosition_Zvalue, double MnR, double MaxR);				//11/02/2014 vinod.. for excel report we need here cam profile circle center..
	bool GetLocalCamProfileParameter(int PtsToBundle, double Angle, PointCollection* PtsList, double* CamParam, double* LocalParam);
};


//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
