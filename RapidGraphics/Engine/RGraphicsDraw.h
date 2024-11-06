//Class used to Handle the Graphics..
#pragma once
#include "..\RapidGraphics.h"
#include "..\..\RapidMath2D\Engine\RapidMath2D.h"
#include "..\..\RapidMath3D\Engine\RapidMath3D.h"
#include <list>
#include <map>

//Expose Rwindow Graphcis Instance.
#define GRAFIX RGraphicsDraw::getRgdInstance()
//Expose Rapid Math 2d Instance.
#define RMATH2DOBJECT RGraphicsDraw::getRgdInstance()->getRapidMath()
//Expose Rapid math 3D object
#define RMATH3DOBJECT RGraphicsDraw::getRgdInstance()->getRapidMath3D()



#pragma region Structures
//Structure to set the graphics color...//
struct GraphicsColor
{
	double r, g, b, z;
	GraphicsColor(){ r = 1; g = 1; b = 1; z = 1; }
	void set(double r1, double g1, double b1, double z1)
	{
		this->r = r1; this->g = g1; this->b = b1; this->z = z1;
	}
	~GraphicsColor(){}
};

//Structure used to hold the window center and pixelwidth..//
struct GRWindow
{
	double cx, cy, UPP;
	GRWindow(){cx = 0; cy = 0; UPP = 1;}
};
#pragma endregion

class RapidGraphics_API RGraphicsDraw
{
#pragma region Variable declaration
private:
	HDC dc;
	double scalefactor, RotateAngle; //Variable for measurement text parameters...// //Measurement text angle..//
	int wc, cidx, Cwindowno; //window count, //current index
	double CurrentX, CurrentY; // Windono... value for Scale View..
	GRWindow rwindow[5]; // Window Properties..
	//Color properties used for shapes, measurements, crosshair, axis, focus rectangle etc...
	GraphicsColor Crosshair_Color, FlexibleCrosshair_Color, Measure_Color, XAxis_Color, YAxis_Color, Videoback_Color;
	//structure defines the width, height properties of the charaters...
	GLYPHMETRICSFLOAT gms[256];
	std::string fname;
	int PointSize;
    double Bg_Color_Upper[3], Bg_Color_Lower[3];
	double Material_Ambient[4], Material_Specular[4], Material_Diffuse[4];
	double Model_Ambient[4], Light_Position1[4], Light_Position2[4], Light_Position3[4];
	double Light_Position4[4], Light_Position5[4], Light_Position6[4];
	double Material_Shininess;

	RapidMath2D* RapidMath2DInst;
	RapidMath3D* RapidMath3DInst;

#pragma endregion

#pragma region Constructor and Destructor
public:
	RGraphicsDraw();
	~RGraphicsDraw();	
	static RGraphicsDraw* RgraphicsInstance;//static Instance of this class.
	static void CreateGraphicsDrawInstance();
	static void DeleteGraphicsDrawInstance();
	static RGraphicsDraw* getRgdInstance();
	RapidMath2D* getRapidMath();
	//Expose instance of RapidMath 3D...
	RapidMath3D* getRapidMath3D();
#pragma endregion

	
#pragma region Error Handling.
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	void (*RGraphicsDrawError) (char* Graphicserrorcode, char* GraphicsFileName, char* GraphicsFunName);
#pragma endregion

#pragma region Cross hair/Axis, Color and Window Settings Functions..

#pragma region Crosshair Related Functions
	void SetWindowSettings(double upp, double cx, double cy, int cindex);
	void drawAxes(double wupp, bool XaxisHighlighted, bool YaxisHighlighted); //draw the axes...//
	void drawAxes_3D(double wupp);
	void drawAxes_3DWithArrow(double x, double y, double z, double wupp); //draw the 3d Axes...//
	void drawFixedAxis(double cx, double cy, double wupp);
	void drawFixedAxisBackground(double x, double y, double cx, double cy);
	void drawFixedAxisOverlapImgWin(double cx, double cy, double wupp);
	void drawFixedAxisBackgroundOverlapImgWin(double x, double y, double cx, double cy);
	void drawFlexibleCrossHair(double x, double y, double gap = 0.0);
	void drawCrossHair(bool hatchedmode, bool record, int* offset, bool ShiftUpperVerticalLine = false);
	void drawCrossHair(bool hatchedmode, bool record, double angle); // , int* offset);
	void drawXCrossHair(bool hatchedmode, double linewidth = 2);
	void drawYCrossHair(bool hatchedmode, double linewidth = 2);
	void drawAngleCrossHair(bool hatchedmode, double angle, double linewidth = 2/*, double minX =20, double minY = 20*/);
	void drawLineFragment(bool hatchedmode, double x1, double y1, double x2, double y2, double linewidth = 2);
	void drawScanMode(double x,double y, double width, double wupp, bool hatchedmode, bool flag);
	void drawScaleViewMode(double wupp, int WinWidth, int WinHeight, bool INCH_Mode);
	void drawstr_scaleview(double x, double y, double d, double wupp, int DecPlaces);
	void drawXaxis(double x);
	void drawYaxis(double x);
	void drawLiveScanCross(double x, double y, double width, double wupp, bool flag);
	void drawMultiScanCross(double x, double y, double width, double wupp);
	void drawTransparentRect(double x, double y, double width, double wupp);

#pragma endregion

#pragma region Set Graphics Color Properties
	void SetColor_Double(double r = 1, double g = 1, double b = 1);
	void SetColor_Double_Alpha(double r = 1, double g = 1, double b = 1, double alpha = 0.0);
	void SetColor_Int(int r = 255, int g = 255, int b = 255);
	void SetMeasurement_Color(double r = 1, double g = 1, double b = 0);
	void SetCrossHair_Color(double r, double g, double b);
	void SetFlexbleCrossHair_Color(double r, double g, double b);

	void SetMaterial_Ambient(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetMaterial_Specular(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetMaterial_Diffuse(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetMaterial_Shininess(double z = 0);
	void SetModel_Ambient(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetLight_Position1(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetLight_Position2(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetLight_Position3(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetLight_Position4(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetLight_Position5(double r = 1, double g = 1, double b = 1, double z = 1);
	void SetLight_Position6(double r = 1, double g = 1, double b = 1, double z = 1);
#pragma endregion

#pragma region Set Graphics CallList, Width Properties
	void CreateNewGlList(int id);
	void EndNewGlList();
	void DeleteGlList(int id);
	void CalldedrawGlList(int id);
	void PushCurrentGraphics();
	void PopCurrentGraphics();
	void SetGraphicsLineWidth(double width);
	void LoadGraphicsIdentityMatrix();
#pragma endregion
#pragma region Text Font Family
	void SetFontFamily(std::string FFamilyName);
	std::string GetFontFamilyName();
#pragma endregion
	
#pragma region Set Background Image		
	void DrawVideoTextureBackGround(std::map<int, std::string> imglist, int bpwidth, int bpheight, int RowCount , int ColumnCount);
	void DrawVideoTextureBackGround(unsigned char* pixArray, int bpwidth, int bpheight, char* FPath);
#pragma endregion

#pragma region Set Background Gradient
	void SetBackGroundGradient(double r1,double g1, double b1, double r2, double g2, double b2);
#pragma endregion

#pragma region Set FG and Cloud Point Size
	void SetFGandCloudPtSize(int PointSize);
#pragma endregion
	
#pragma endregion

#pragma region Two_D Measurement Drawing

#pragma region Point To Point Distance Measurements
	void drawPoint_PointDistance(double x1, double y1, double x2, double y2, double x3, double y3, double DisValue, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawPoint_PointDistance_Linear(double x1, double y1, double x2, double y2, bool flag, double x3,double y3, double d, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Point To Line Distance Measurements
	void drawPoint_FinitelineDistance(double x1, double y1, double slope, double intercept, double *endp1, double *endp2, double x, double y, double Dist, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawPoint_InfinitelineDistance(double x1, double y1, double slope, double intercept, double x, double y, double Dist, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawPoint_RayDistance(double x1, double y1, double slope, double intercept, double *endp1, double x, double y, double Dist, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Circle To Line Distance Measurements
	void drawCircle_FinitelineDistance(double slope, double intercept, double *endp1, double *endp2, double *center, double radius, double x, double y, double Dist, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawCircle_InfinitelineDistance(double slope, double intercept, double *center, double radius, double x, double y, double Dist, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawCircle_RayDistance(double slope, double intercept, double *endp1, double *center, double radius, double x, double y, double Dist, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Circle to Circle,Point Measurements
	void drawCircle_PointDistance(double x1, double y1,double radius, double x2, double y2,double distan, double x3, double y3, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawCircle_PointDistance_Linear(double x1, double y1,double radius, double x2, double y2,double d,double x3, double y3, char* distance, char* MeasureName,int type, bool fl, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawCircle_CircleDistance(double x1, double y1, double radius1, double x2, double y2, double radius2, double distan, double x3, double y3, char* distance, char* MeasureName, int type,  int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawCircle_CircleDistance(double x1, double y1, double radius1, double x2, double y2, double radius2, double distan, double x3, double y3, char* distance, char* MeasureName, int type1, int type2, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawCircle_CircleDistance_Linear(double x1, double y1, double radius1, double x2, double y2, double radius2, double distan, double x3, double y3, char* distance, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Line to Line Distance Measurements
	void drawInfiniteline_InfinitelineDistance(double slope1, double intercept1, double slope2, double intercept2,double distance, double x, double y, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawFiniteline_FinitelineDistance(double *endp11,double *endp12,double *endp21,double *endp22, double slope1,double intercept1,double slope2,double intercept2,double distance, double x, double y, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawRay_RayDistance(double *endp1,double slope1, double intercept1, double *endp2,double slope2, double intercept2,double distance, double x, double y, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawInfiniteline_FinitelineDistance(double slope1, double intercept1,double *endp1, double *endp2,double slope2, double intercept2,double distance, double x, double y, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawInfiniteline_RayDistance(double slope1, double intercept1,double *endp1,double slope2, double intercept2,double distance, double x, double y, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawFiniteline_RayDistance(double *endp1, double *endp2,double slope1, double intercept1, double *endp3,double slope2, double intercept2,double distance, double x, double y, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Line to Line Angles Measurements
	void drawFiniteline_FinitelineAngle(double slope1,double intercept1,double *endp11,double *endp12, double slope2,double intercept2,double *endp21,double *endp22,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawInfiniteline_InfinitelineAngle(double slope1,double intercept1,double slope2,double intercept2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawRay_RayAngle(double slope1,double intercept1,double *endp1, double slope2,double intercept2,double *endp2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawInfiniteline_FinitelineAngle(double slope1,double intercept1,double *endp1,double *endp2, double slope2,double intercept2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawInfiniteline_RayAngle(double slope1,double intercept1,double *endp1,double slope2,double intercept2,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawFiniteline_RayAngle(double slope1,double intercept1,double *endp1,double *endp2, double slope2,double intercept2,double *endp3,double x,double y,char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawSlope(double slope, double intercept, double x,double y, double angle1,char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawPCDAngle(double centerX, double centerY, double slope1, double intercept1, double slope2, double intercept2, double mouseX, double mouseY, bool drawdir, char* strdistance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Arc, Circle: Radius, Diameter Measurements
	void drawArclength(double x1,double y1,double x2,double y2,double *endp1, double *endp2,double r, double start, double sweep, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawDiameter(double x1, double y1, double x2, double y2, double d, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawArc_Radius(double *center, double shapeR, double *MouseSelectnLine, double *TransMatrix, double *MeasurementPlane, double start, double sweep, double *lastmpX, double *lastmpY,char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
#pragma endregion

#pragma region Draw Digital Micrometer
	void drawDigital_Micrometer(double x ,double y, double x1, double y1, double x2, double y2, double x3, double y3, double dis, char* distance, char* MeasureName, int windowno, double w_upp, double Fscalefactor, bool nearMousePt, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Draw Common Measurement parameters
	void drawMeasureLine(double e1[], double e2[]);
	void draw_MeasurementString2D(double m1[], double Dis[], char* Distance, char* MeasureName, int windowno, double FScaleFactor, double* Lefttop, double* RightBtm);
	void drawCommonProperties_AllMeasure2D(double e1[], double e2[],double Dis[],double m1[], double m2[],double slopear, double intercept1, double intercept2, double Dist, char* Distance, char* MeasureName, int windowno, double w_upp, double FScaleFactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawCommonProperties_Angle2D(double pt[], double center[], double startang[], double sweepang[], double radius, double slop[], double DisString[], char* strdistance, char* MeasureName, int windowno, double w_upp, double FScaleFactor, double* Lefttop, double* RightBtm, double Zlevel);
#pragma endregion

#pragma region Draw Arrow
	void drawarrow(double *m1,double *sl,double *x1,double *x2,double *dis,double *space, double w_upp);
	void drawarrow(double *endpt,double sl, double w_upp, double Zlevel=0);
	void drawarrow1(double *endpt,double sl, double w_upp);
#pragma endregion

#pragma region FG Direction for Arc, Circle FG Trnaslate Matrix, Focus Program Path
	void translateMatrix(double x, double y, double z);
	void drawArcFGDir(double x, double y, double *p1, double radius, double startangle, double sweepangle);
	void drawFillArcFG(double x, double y, double radius1, double radius2, double startangle, double sweepangle);
	void drawFillCircle(double x, double y, double radius1, double radius2);
	void drawFocusRectangle(double row, double column, double gap, double width, double height, int ShiftInX, int ShiftY);
	void drawPartprogramPathArrow(double x1, double y1, double x2, double y2, double wupp);
	void drawPartProgramStartPoint(double x1, double y1, double wupp);
#pragma endregion

#pragma region Reference Datum For GD&T Measurements
	void drawReferenceDatumName_Line(double slope, double *point, char* Strname, double w_upp);
	void drawReferenceDatumName_Circle(double *center, double radius, char* Strname, double w_upp);
	void drawReferenceDatumName_Line3D(double *Line, double *MidPt, char* Strname, double w_upp);
	void drawReferenceDatumName_Plane(double *Plane, double *MidPt, char* Strname, double w_upp);
	void drawReferenceDatumName_Sphere(double *Sphere, char* Strname, double w_upp);
	void drawReferenceDatumName_Cylinder(double *Cylinder, double *CylinderEndPts, char* Strname, double w_upp);
	void drawReferenceDatumName_Cone(double *Cone, double *ConeEndPts, char* Strname, double w_upp);
	void drawHorizontalDName(double *point, char* Strname, double w_upp);
	void drawVerticalDName(double *point, char* Strname, double w_upp);
	void drawDName_Slope(double slope, double *point, char* Strname, double w_upp);
	void drawDName3D(double *endp, double *endp1, double *endp2, double *endp3, char* Strname, double w_upp);
#pragma endregion

#pragma region Line GD&T Measurements
	void drawAngularity(double x, double y, double x1, double y1, char* distance, char* s1n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double Zlevel);
	void drawAngularity_MeasureString(double *m1, char* Distance, char* MeasureName, double StrWidth, double StrHeight, int type, double Fscalefactor);
	void drawString_AngularityName(double x,double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor);
	void Calculate_MeasureRectangle(double *m1, double *Dis, double* Lefttop, double* RightBtm);
#pragma endregion

#pragma region Circle GD&T Measurements
	void drawConcentricity(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel);
	void drawString_Concentricity(double x, double y, double z, double angle, char * string, double StrWidth, double StrHeight, double Fscalefactor);
	void drawTruePosition(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* s2n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel);
	void draw3DTruePosition(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* s2n, char* s3n, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel);
	void drawString_TruePositionXY(double x, double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor);
	void drawString_TruePositionXYZ(double x, double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor);
	void drawString_TruePositionOneAxis(double x, double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor);
#pragma endregion

	void drawProfile(double x, double y, double x1, double y1, double d, char* distance, char* s1n, char* MeasureName, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, double ZLevel);
	void drawString_Profile(double x, double y, double z, double angle, char* string, double StrWidth, double StrHeight, double Fscalefactor, double wr_upp);

#pragma endregion
	
#pragma region Three_D Measurements Drawing

#pragma region ThreeD Point to Other Shape Measurements	
	void drawPoint_PointDistance3D(double x1, double y1, double z1, double x2, double y2, double z2, double disValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPoint_PointDistance3DLinear(double x1, double y1, double z1, double x2, double y2, double z2, double disValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, int type);
	void drawPoint_FinitelineDistance3D(double *Point, double *endpts, double *Line, double Dist, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPoint_PlaneDistance3D(double *Point, double *Plane, double *PlnPts, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPoint_CylinderDistance3D(double *Point, double *Cylinder, double *EndPts, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPoint_SphereDistance3D(double *Point, double *TheSphere, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPoint_elipseDistance3D(double *Point, double *elipse, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPoint_Circle3DDistance3D(double *Point, double *Circle3D, double DisValue, char* distance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm, int type = 0);
#pragma endregion

#pragma region ThreeD Line to Other Shape Measurements	
	void drawFiniteline_FinitelineDistance3D(double *Line1, double *endPts1, double *Line2, double *endPts2, double distance, char* strdistance, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawFiniteline_FinitelineAngle3D(double *Line1, double *LnPts1, double *Line2, double *LnPts2, double angle, char* angleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawFiniteline_FinitelineAngle3D1(double *Line1, double *LnPts1, double *Line2, double *LnPts2, double angle, char* angleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawFiniteline_PlaneDistOrAng3D(double *TheLine, double *LinePts, double *ThePlane, double *PlanePts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawFiniteline_CylinderDistance3D(double *TheLine, double *LnEndPts, double *TheCylinder, double *CylndrEndPts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawFiniteline_CylinderAngle3D(double *TheLine, double *LnEndPts, double *TheCylinder, double *CylndrEndPts, double Angle, char* AngleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawFiniteline_SphereDistance3D(double *TheLine, double *LinePts, double *TheSphere, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawFiniteline_Circle3DDistOrAng3D(double *TheLine, double *LinePts, double *Circle3D, double DistOrAng, char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, bool ProjectLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
    void drawFiniteline_Conics3DDistOrAng3D(double *TheLine, double *LinePts, double *Conics3D, double DistOrAng, char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, bool ProjectLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
#pragma endregion

#pragma region ThreeD Plane to Other Shape Measurements	
void drawPlane_PlaneDistOrAng3D(double *ThePlane1, double *Pln1Pts, double *ThePlane2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPlane_CylinderDistOrAng3D(double *ThePlane, double *PlnPts, double *TheCylinder, double *CylinderPts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistOnly, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawPlane_SphereDistance3D(double *ThePlane, double *PlnPts, double *TheSphere, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawCircle3D_Circle3DDistOrAng3D(double *Circle3D1, double *Circle3D2, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawConics3D_Circle3DDistOrAng3D(double *Conics3D, double *Circle3D, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawCircle3D_PlaneDistOrAng3D(double *Circle3D, double *ThePlane2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawConics3D_PlaneDistOrAng3D(double *Conics3D, double *ThePlane2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawConics3D_Conics3DDistOrAng3D(double *Conics3D1, double *Conics3D2, double *Plane2Pts, double DistOrAng,char* DistAngChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, bool DistFlag, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
#pragma endregion

#pragma region ThreeD Cylinder, Sphere Measurements	
	void drawCylinder_CylinderDistance3D(double *TheCylinder1, double *Cylndr1Pts, double *TheCylinder2, double *Cylndr2Pts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawCylinder_CylinderAngle3D(double *TheCylinder1, double *Cylndr1Pts, double *TheCylinder2, double *Cylndr2Pts, double Angle, char* AngleChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawCylinder_ConeDistance3D(double *TheCylinder, double *CylndrPts, double *TheCone, double *ConePts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawSphere_CylinderDistance3D(double *TheSphere, double *TheCylinder, double *CylndrPts, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawSphere_SphereDistance3D(double *TheSphere1, double *TheSphere2, double Distance,char* DistanceChar, char* MeasureName, double *TransMatrix, double *MeasurementPlane, int MeasureType, double* MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
#pragma endregion

#pragma region ThreeD Cylinder, Sphere Radius, Dia Measurements
	void drawDiametere3D(double *center, double shapeDia, char* distance, char* MeasureName, double *MouseSelectnLine, double *TransMatrix, double *MeasurementPlane,int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawDiametere3D(double *cylinder, double *cylinderEndPts, char* distance, char* MeasureName, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawAxis3D(double *conics3D, double max, double min, double disvalue, char* distance, char* MeasureName, double *MouseSelectnLine, int windowno, double w_upp, double Fscalefactor, double measuretype, double* Lefttop, double* RightBtm);
#pragma endregion

#pragma region ThreeD GD&T Functions
	void drawAngularity3D(double *MouseSelectnLine, double Midx, double Midy, double Midz, char* distance, char* s1n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
	void drawAngularityCylinder(double *MouseSelectnLine, double *cylinder, double *cylinderEndPts, char* distance, char* s1n, char* MeasureName, int type, int windowno, double w_upp, double Fscalefactor, double* Lefttop, double* RightBtm);
#pragma endregion

#pragma region ThreeD Draw Functions: For the Measurements
	void DrawArrowCone(double *Point1, double *Point1Tail, double w_upp);
	void CalculateAndDrawArrowConesAng(double *Point1, double *Point2, double *ArrowSlopes, double *TransMatrix, double w_upp);
	void CalculateAndDrawArrowCones(double *Point1, double *Point2, double *Point1Tail, double *Point2Tail, double w_upp);
	void drawCommonProperties_Angle3D(double pt[], double center[], double startang[], double sweepang[], double radius, double slop[], double DisString, char* strdistance, char* MeasureName, double ZLevel, double *TransMatrix);
	void draw_MeasurementString3D(double m1[], double Dis[], char* Distance, char* MeasureName, int windowno, double FScaleFactor, double* Lefttop, double* RightBtm);
	void drawStippleLines3D( double pts[], int NoOfLns);
	void drawMeasureLine3D( double e1[], double e2[]);
#pragma endregion

#pragma endregion

#pragma region DrawShapes
	
#pragma region Probe Drawing
	void drawProbe_Z(double x, double y, double z);
	void drawProbe_Y(double x, double y, double z);
	void drawcylinder_Probe(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius,double Bradius, bool topdisk = true, bool bottomdisk = true);
	void drawSphere_Probe(double x, double y, double z, double radius);
#pragma endregion

#pragma region TwoD Shapes
	//Draw a point of default size 5..// size can be userdefined....//
	void drawPoint(double x, double y, double z, GLfloat pointsize = 5.0);
	//Draw points... More than One point//point size can be 5/user defined...//
	void drawPoints(double *pts, int count, bool AllowTransparency, bool GreenColor, int numberofplane = 3, GLfloat pointsize = 5.0);
	void drawPoints(double *pts, int count, bool AllowTransparency, int numberofplane = 3, GLfloat pointsize = 5.0);
	void DrawPointsWithColor(double X, double Y, double Z, double R, double G, double B, GLfloat pointsize = 5.0);
	//Draw Points in different colr based on z level........
	void drawPoints(double *pts, int count, double MinZ, double MaxZ, bool AllowTransparency, bool DiscreteColor = false, double tolerance = 0, GLfloat pointsize = 5.0);
	//Draw the finite line..//
	void drawLine(double x1, double y1, double x2, double y2);
	//Draw the line stipple...//
	void drawLineStipple(double x1, double y1, double x2, double y2, int PixelCount = 2, GLushort Lpattern = 0xAAAA);
	//Draw the circle..//
	void drawCircle(double x, double y, double radius, bool draw_Center = false);
	//draw circle stipple
	void drawCircleStipple(double x, double y, double radius, int PixelCount = 2, GLushort Lpattern = 0xAAAA);
	//Draw the arc...//
	void drawArc(double x, double y, double radius, double startangle, double sweepangle, double wupp = 0);
	//Draw Arc Stipple
	void drawArcStipple(double x, double y, double radius, double startangle, double sweepangle, int PixelCount = 2, GLushort Lpattern = 0xAAAA);
	//Draw the rectangle...//.. using the top and bottom corners..
	void drawRectangle(double x1, double y1, double x2, double y2, bool fill = false);
	//Draw rectangle.. using 4 end points...//
	void drawRectangle(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool fill = false);
	//Draw Triangle.. using 3 end points...//
	void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, bool fill = false);
	// Draw Cross mark..
	void drawCrossMark(double x, double y, double z, double wupp);
	//Draw the ray/ Infinte line....//
	void drawXLineOrXRay(double angle, double x1, double y1, double w_upp, bool Infinite, double Zlevel=0);
	void draw3DXLine(double* DirCosines, double x1, double y1, double z1, double w_upp);
	//Draw the ray/ Infinte line....//
	void drawXLineOrXRayStipple(double angle, double x1, double y1, double w_upp, bool Infinite, int PixelCount = 2, GLushort Lpattern = 0xAAAA);
	//Draw Inifinite Line throug Point and angle
	void drawXLineThruPoint_Stipple(double x1, double y1, double w_upp, bool Vertical, GLushort Lpattern = 0x5F5F);
	//Draw line loop..//
	void drawLineloop(double *pts, int cnt,int* lineloop, int numberofPlane = 3);
#pragma endregion

#pragma region Three_D Shape, Probe	
	//3D draw functions....///
	void drawSpline(double* PtsArray, int ncpts, double* Knots_Array, int degree = 3);
	void drawLine_3D(double x1, double y1, double z1, double x2, double y2, double z2);
	void drawLine_2D(double x1, double y1, double x2, double y2);
	void drawLine_3DStipple(double x1, double y1, double z1, double x2, double y2, double z2, GLushort Lpattern = 0xAAAA);
	void drawXLineOrXRay3D( double* line, double w_upp, bool Infinite, bool stipple );
	void drawArc3D(double x,double y,double radius,double startangle,double sweepangle, bool drawStipple, double ZLevel, double *Transmatrix);
	void drawShapeArc3D(double* Arc3dParams, double StartAng,double SweepAng, double wupp);
	//Draw 3d circle
	void drawCircle3D(double* Circle3dParams);
	//Draw 3d conic
	void drawConic3D(double* ConicParams);
	//draw 2d conic with diretrix along y axis, focus at origin, 
	//eccentricity e, distance of diretrix from focus f_d,
	//start angle of startang and end angle of endang
	void drawConic(double e, double fd, double startAng, double endAng);

	void drawRectangle3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, bool fill = false);
	void drawConeFor3DAxis(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius,double Bradius);
	void EnableLighting(double PosX, double PosY, double PosZ, double l, double m, double n);
	void EnableLighting();
	void DisableLighting();
	void EnableTransparency();
	void DisableTransparency();
	//draw sphere..
	void draw_Sphere(double x, double y, double z, double radius, bool Wiredframe, bool AllowTransparency, double wupp);
	//draw wired sphere...
	void draw_SpherePart(double x, double y, double z, double radius, double MinZPt, bool Wiredframe, bool AllowTransparency, double wupp);
	//draw partial shapes
	void draw_SpherePartial(double x, double y, double z, double radius, double theta_min, double theta_max, double phi_min, double phi_max, bool Wiredframe, bool AllowTransparency, double wupp);
	void draw_SphereFromPts(double x, double y, double z, double radius, double* pts, int ptscount, bool Wiredframe, bool AllowTransparency, double wupp);
	void draw_CylinderPartial(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius, double Bradius, double phi_min, double phi_max, bool Wiredframe, bool AllowTransparency, double wupp);
	void draw_CylinderFromPts(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius, double Bradius, double* pts, int ptscount, bool Wiredframe, bool AllowTransparency, double wupp);
	void draw_PlaneFromPts(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, double* pts, int ptscount, bool AllowTransparency, bool Wiredframe, double wupp);
	void draw_Torus(double x, double y, double z, double l, double m, double n, double outer_radius, double inner_radius, bool WiredFrame, bool AllowTransparency, double wupp);
	
	//Function to draw cylinder...//
	void draw_Cylinder(double x1, double y1, double z1, double x2, double y2, double z2, double Tradius, double Bradius, bool Wiredframe, bool AllowTransparency, bool drawdisk, double wupp);
	//Draw the circular disk... cylinder top...//
	void draw_Circledisk(double x1, double y1, double z1, double radius, bool Wiredframe, bool AllowTransparency, double wupp);
	//Draw the four point plane.../
	void draw_Plane(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, bool AllowTransparency, bool Wiredframe);
	void draw_PlanePolygon(double* planepolygonpts, int ptscount, bool AllowTransparency, bool Wiredframe);
	void draw_TriangleWithColour(double* TrianglePts, double MinZ, double MaxZ, bool AllowTransparency, bool Wiredframe, bool WithoutLight, bool DiscreteColor = false, double tolerance = 0);

#pragma endregion
	
#pragma region Text handling functions
	//Create the new font for the current window....//
	BOOL CreateNewFont(HWND pDrawDC, char* fontname);
	//Text width and height..
	double getTextWidth(char *string);
	double getTexHeight(char *string);
	double getYShift(char *string);

	//draw Strings..
	void drawString(double x,double y, double z, double angle, char *string, double FScaleFactor, bool drawatmidpoint = false);
	void drawStringWith_BackGroud(double x,double y, double z, double angle, char *string, double FScaleFactor, double r, double g, double b);
	void drawString_BackGroud(double x, double y, double z, double StrWidth, double StrHeight, double r, double g, double b);
	void drawString3D(double x,double y, double z, char *string, double FScaleFactor);
	void drawString3D(double x,double y, double z, double angle, char *string, double FScaleFactor);
	void drawString3D1(double x,double y, double z, double angle, char *string, double FScaleFactor);
	void drawStringFixed(double x, double y, double z, double scalefact, char *string);
	void drawStringEdited3D(double x,double y, double z, char *string, double FScaleFactor);
	void drawStringEdited3D(double x,double y, double z, double angle, char *string, double FScaleFactor);
	void drawStringEdited3D1(double x,double y, double z, double angle, char *string, double FScaleFactor);

	int next_powerof2(int a);
	void drawStringtexture(BYTE* bpdata, int bpwidth, int bpheight, double Posx, double Posy, double Posz, double angle);

	GLuint CurrentTextureId;
	float TextXwidth, TextYwidth;
	void GenerateTextureFromImage(BYTE* bpdata, int bpwidth, int bpheight);
	void RenderTextureInCube(double Posx, double Posy, double Posz, double wupp);
	void DrawBackGround(bool is3Dmode);
	void DrawColorIndicator(double* ZlevelExtent, double* LeftTop, double tolerance, double WPixelWidth);
	void DrawRectangleWithGradient(double* LeftTop, double* RightBottom, double* UpperColor, double* LowerColor);
	void InsertText(double x,double y, double z, bool angle, std::list<std::string > string, double FScaleFactor);
	void DrawInvolute(double startradius, double endradius, double startAng, double endAng , double CenterX , double CenterY);
	void DrawCircleInvoluteShape(double* Involuteparam);
	void DrawCamProfile(double *Pts, int PtsCount, int *PtsOrder);
#pragma endregion 

#pragma endregion
};
