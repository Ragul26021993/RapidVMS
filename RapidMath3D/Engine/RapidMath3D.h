#ifdef RAPIDMATH3D_EXPORTS
#define RapidMath3D_API  __declspec(dllexport)
#else
#define RapidMath3D_API  __declspec(dllimport)
#endif
#pragma warning(disable: 4251) 

#include <string>
#include <list>

class RapidMath3D_API RapidMath3D
{
private:
	int PrecisionCt;
	 
public:
	RapidMath3D();
	~RapidMath3D();
public:	 

#pragma region Error Handling.
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	void (*RMath3DError)(char* RMath3Derrorcode, char* RMath3DFileName, char* RMath3DFunName);
#pragma endregion

#pragma region Distance calculations
	bool DirectionCosines(double *DirRatios, double *DirCosines, bool FourthParameter = false);
	double Distance_Point_Point(double *Point1, double *Point2);
	double Distance_Point_Line(double* p, double* L);
	double Distance_Point_Plane(double *point, double *plane);
	double Distance_Point_PlaneAlongSeletedAxis(double *point, double *plane,int selectedAxis);
	double Distance_Point_Plane_with_Dir(double *point, double *plane);
	double Distance_Point_Sphere(double *Point, double *Sphere, int MeasureType);
	double Distance_Point_Sphere_with_Dir(double *Point, double *Sphere);
	double Distance_Point_Cylinder(double *Point, double *Cylinder, int MeasureType);
	double Distance_Point_Cylinder_with_Dir(double *Point, double *Cylinder);
	double Distance_Line_Line(double *Line1, double *Line2);
	double Distance_Line_Plane(double *Line, double *Plane);
	double Distance_Line_Sphere(double *Line, double *Sphere, int MeasureType);
	double Distance_Line_Cylinder(double *Line, double *Cylinder, int MeasureType);
	double Distance_Line_Cone(double *Line, double *Cone, int MeasureType);
	double Distance_Line_Cone(double* Line, double* Cone);
	double Distance_Plane_Plane(double *Plane1, double *Plane2);
	double Distance_Plane_Sphere(double *Plane, double *Sphere, int MeasureType);
	double Distance_Plane_Cylinder(double *Plane, double *Cylinder, int MeasureType);
	double Distance_Sphere_Sphere(double *Sphere1, double *Sphere2, int MeasureType);
	double Distance_Sphere_Cylinder(double *Sphere, double *Cylinder, int MeasureType);
	double Distance_Cylinder_Cylinder(double *Cylinder1, double *Cylinder2, int MeasureType);
	double Distance_Cylinder_Cone(double *Cylinder, double *Cone, int MeasureType);
	double Pt2Circle3D_distance_Type(double *point, double *center, double *dir, double radius, int type);
	double Distance_Point_Cone(double *Point, double *Cone, double *Sf1, double *Sf2, int MeasureType);
	double Distance_Cone_Cone(double *Cone1, double *Cone2);
#pragma endregion
	 
#pragma region Angle Calculations
	 //Function to calculate angle between 2 directions(cosines or ratios). Also an option to get the cos of angle directly
	double Angle_Btwn_2Directions(double *Direction1, double *Direction2, bool Ratios, bool CosOfAngle);
	bool CheckVectorBtwn2Vectors(double *Direction1, double *Direction2, double *checkDir, bool Ratios, bool equalVector = false);
	double Angle_Line_Line(double *Line1, double *Line2);
	double Angle_Line_Plane(double *Line, double *Plane);
	double Angle_Plane_Plane(double *Plane1, double *Plane2);
#pragma endregion

#pragma region Intersection between 2 objects
	bool Intersection_Line_Line(double *Line1, double *Line2, double *IntersectionPt);
	bool Plane_Triangle_Intersection(double* trianglePts, double Value, int Axisno, double* Ans);
	int Intersection_Line_Plane(double *Line, double *Plane, double *IntersectionPt);
	int Intersection_Line_Sphere(double *Line, double *Sphere, double *IntersectionPts);
	int Intersection_Line_elipse(double *Line, double *elipse, double *IntersectionPts);
	int Intersection_Line_Cylinder(double *Line, double *Cylinder, double *IntersectionPts);
	int Intersection_Plane_Plane(double *Plane1, double *Plane2, double *IntersectionLine);
	int Intersection_Plane_Sphere(double *Plane, double *Sphere, double *IntersectionCircle);
	bool Intersection_Plane_Cylinder(double *Plane, double *Cylinder, double *IntersectionCircle);
	int Intersection_Line_Cone(double* Line, double* Cone, double* IntersectionPts);
	bool Intersection_Line_Torus(double* Line, double* Torus, double wupp, int* pt_cnt, double* intersection_pts);
	
#pragma endregion

#pragma region Condition Checkings
	//Function to check if a given point is within the bounds of X, Y and Z extremes of a group of points
	bool Checking_Point_On_LineSegment(double *EndPts, double *Point);
	bool Checking_Point_within_Block(double *Point, double *Points, int NoOfPts);
	bool Checking_Point_on_Line(double *Point, double *Line);
	bool Checking_Point_on_Plane(double *Point, double *Plane);
	bool Checking_Point_on_Sphere(double *Point, double *Sphere);
	bool Checking_Point_on_Cylinder(double *Point, double *Cylinder);
	bool Checking_Line_on_Plane(double *Line, double *Plane);
	bool Checking_Parallel_Line_to_Line(double *Line1, double *Line2);
	bool Checking_Perpendicular_Line_to_Line(double *Line1, double *Line2);
	bool Checking_Intersection_Line_Line(double *Line1, double *Line2);
	bool Checking_Parallel_Line_to_Plane(double *Line, double *Plane);
	bool Checking_Perpendicular_Line_to_Plane(double *Line, double *Plane);
	bool Checking_Parallel_Plane_to_Plane(double *Plane1, double *Plane2);
	bool Checking_Perpendicular_Plane_to_Plane(double *Plane1, double *Plane2);
	bool Checking_Intersection_2Lines(double *Line1, double *Line2Pts, double *IntersectnPt);
	bool Checking_PointIsInTriangle(double *point, double *Vertex1, double *Vertex2, double *Vertex3);
#pragma endregion

#pragma region Creating objects relative to other objects
	//Function to measure the direction cosines of a direction perpendicular to 2 given directions
	bool Create_Perpendicular_Direction_2_2Directions(double *Direction1, double *Direction2, double* PerpendDir);
	bool Create_AngleBisector_Lines(double *Line1, double *Line2, double *Bisector1, double *Bisector2);
	bool Create_AngleBisector_Planes(double *Plane1, double *Plane2, double *Bisector1, double *Bisector2);
	void Create_Parallel_Line_2_Line(double *Point, double *Line, double *CreatedLine, double offset = 0);
	//void Create_Parallel_Line_2_Line(double *Point, double *Line, double *CreatedLine, double offset, double*);
	bool Create_Perpendicular_Line_2_Line(double *Point, double *Line, double *CreatedLine);
	void Create_Perpendicular_Line_2_Plane(double *Point, double *Plane, double *CreatedLine);
	void Create_Parallel_Plane_2_Plane(double *Point, double *Plane, double *CreatedPlane);
	bool Create_Perpendicular_Plane_2_Plane(double *Line, double *Plane, double *CreatedPlane);
	bool Create_Perpendicular_Plane_2_Plane(double *Point1, double *Point2, double *Plane, double *CreatedPlane);
	bool Create_Plane_Line_2_Line(double *Line1, double *Line2, double *CreatedPlane);
	bool Create_Points_On_Line(double* Line, double* Point, double Dist, double* PointsOnLine);

#pragma endregion

#pragma region Equation solving functions
	void SolveEquationRatio(double *eq1, double *eq2, double *solution);
	void SolveEquation(double *eq1, double *eq2, double *solution);
	void SolveEquation(double *eq1, double *eq2, double *eq3, double *solution);
	int Solve_Quadratic_Equation(double *Parameters, double *Answers);
#pragma endregion

#pragma region Projections
	bool Projection_Point_on_Line(double *Point, double *Line, double *PointOfProjectn);
	bool Projection_Point_on_Plane(double *Point, double *Plane, double *PointOfProjectn);
	bool Projection_Line_on_Plane(double *Line, double *Plane, double *LineOfProjectn);
#pragma endregion

#pragma region Unused functions
	//Function to be called for calculating the parameters required for plane to plane angle measurement
	void Measurement_Angle_Plane_Plane(double *Plane1, double *Plane2, double *MousePt, double *Line1, double *Line2, double* TransMousePnt, double *RotX, double *RotY);
	void PlaneRotationAngles(double *Plane, double *RotationAngles);

#pragma region Miscellaneous functions
	bool Points_Of_ClosestDist_Btwn2Lines(double *Line1, double *Line2, double *Points);
	
	bool Calculate_StandardForm_Line(double *LineEndPt1, double *LineEndPt2, double *LineStdForm);
	double OperateMatrix4X4(double* matrix, int Size, int Flag, double* Answer);
	double RoundDecimal(double valu, int decPlace);
#pragma endregion
	 
#pragma region Transformation matrix and related calculations
	 //Point to point measurement
	 void GetTMatrixForPlane(double *Plane, double *TransMatrix);
	 void TransformationMatrix_Dist_Pt2Pt(double *Point1, double *Point2, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Pt2Ln(double *Point, double *Line, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Pt2Pln(double *Point, double *Plane, double *PlnMidPt, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Pt2Spr(double *Point, double *Sphere, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Pt2Cylndr(double *Point, double *Cylinder, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Ln2Ln(double *Line1, double *Line2, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Ln2Pln(double *Line, double *Plane, double *PlanePts, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
	 void TransformationMatrix_Dist_Ln2Spr(double *Line, double *Sphere, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Ln2Cylndr(double *Line, double *Cylinder, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Pln2Pln(double *Plane1, double *Plane2, double *Pln1MidPt, double *Pln2MidPt, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Pln2Spr(double *Plane, double *Sphere, double *PlnMidPt, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Pln2Cylndr(double *Plane, double *PlanePts, double *Cylinder, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
	 void TransformationMatrix_Dist_Spr2Spr(double *Sphere1, double *Sphere2, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Spr2Cylndr(double *Sphere, double *Cylinder, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Cylndr2Cylndr(double *Cylinder1, double *Cylinder2, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Angle_Ln2Ln(double *Line1, double *Line2, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Angle_Ln2Pln(double *Line, double *Plane, double *PlanePts, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
	 void TransformationMatrix_Angle_Ln2Cylndr(double *Line, double *Cylinder, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Angle_Pln2Pln(double *Plane1, double *Plane2, double *PtOfRef, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Angle_Pln2Cylndr(double *Plane, double *PlanePts, double *Cylinder, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
	 void TransformationMatrix_Angle_Cylndr2Cylndr(double *Cylinder1, double *Cylinder2, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dist_Ln2Cir3D(double *Line, double *Circle3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
	 void TransformationMatrix_Angle_Ln2Cir3D(double *Line, double *Circle3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
     void TransformationMatrix_Dist_Ln2Conics3D(double *Line, double *Conics3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
	 void TransformationMatrix_Angle_Ln2Conics3D(double *Line, double *Conics3D, double *Matrix, double *MeasurementPlane, bool *ProjectLine);
	 void TransformationMatrix_Dia_Circle(double *circleParam, double *Matrix, double *MeasurementPlane);
	 void TransformationMatrix_Dia_Sphere(double *sphereParam, double *selLine, double *Matrix, double *MeasurementPlane);
#pragma endregion

#pragma region Toggle calculations for sphere and cylinder measurements
	 //Functions to calculate the parameters for sphere/cylinder measure type toggling
	 void MeasureModeCalc_Point_Sphere(double *Point, double *Sphere, int MeasureType, double *SprSurfacePt);
	 void MeasureModeCalc_Point_Cylinder(double *Point, double *Cylinder, double *CylndrEndPts, int MeasureType, double *CylndrSurfaceLn, double *SurfaceLnEndPts);
	 void MeasureModeCalc_Line_Sphere(double *Line, double *Sphere, int MeasureType, double *SprSurfacePt);
	 void MeasureModeCalc_Line_Cylinder(double *Line, double *Cylinder, double *CylndrEndPts, int MeasureType, double *CylndrSurfaceLn, double *SurfaceLnEndPts);
	 void MeasureModeCalc_Plane_Sphere(double *Plane, double *Sphere, int MeasureType, double *SprSurfacePt);
	 void MeasureModeCalc_Plane_Cylinder(double *Plane, double *Cylinder, double *CylndrEndPts, int MeasureType, double *CylndrSurfaceLn, double *SurfaceLnEndPts);
	 void MeasureModeCalc_Sphere_Sphere(double *Sphere1, double *Sphere2, int MeasureType, double *Spr1SurfacePt, double *Spr2SurfacePt);
	 void MeasureModeCalc_Sphere_Cylinder(double *Sphere, double *Cylinder, double *CylndrEndPts, int MeasureType, double *SprSurfacePt, double *CylndrSurfaceLn, double *SurfaceLnEndPts);
	 void MeasureModeCalc_Cylinder_Cylinder(double *Cylinder1, double *Cylndr1EndPts, double *Cylinder2, double *Cylndr2EndPts, int MeasureType, double *Cylndr1SurfaceLn, double *SurfaceLn1EndPts, double *Cylndr2SurfaceLn, double *SurfaceLn2EndPts);
#pragma endregion

#pragma region sphere/cylinder surface point/line
	 void GetSprSurfacePt(double *Sphere, double *Direction, bool Forward, double *SurfacePt);
	 void GetCylndrSurfaceLn(double *Cylinder, double *CylndrEndPts, double *Direction, bool Forward, double *SurfaceLn, double *SurfaceLnPts);
	 void GetConeSurfaceLn(double* Cone, double* ConeEndPts, double* Direction, bool Forward, double* SurfaceLn, double* SurfaceLnPts);
#pragma endregion

#pragma region Angle calculations according to quadrant of mouse position
	 //Angle calculation functions according to the mouse position
	 double Angle_Line_Line(double *Line1, double *Line2, double *MeasurementPlane, double *MouseSelectionLn, double *Line1MidPt);
	 double Angle_Line_Line(double *Line1, double *Line2, double *MeasurementPlane, double *MouseSelectionLn);
	 double Angle_Line_Plane(double *Line,double *LinePts, double *Plane,double * PlanePts, double *MeasurementPlane, double *MouseSelectionLn, bool ProjectLine);
	 double Angle_XLine_Plane(double *XLineDirratio, double *PlaneDircosine);		//for xline ..................vinod..
	 double Angle_Line_Circle3D(double *Line, double *endpnts, double *Circle3D, double *MeasurementPlane, double *MouseSelectionLn, bool ProjectLine);
	 double Angle_Plane_Plane(double *Plane1, double *Plane2, double *MeasurementPlane, double *MouseSelectionLn);
     double Angle_Line_Conics3D(double *Line, double *endpnts, double *Conics3D, double *MeasurementPlane, double *MouseSelectionLn, bool ProjectLine);
#pragma endregion

#pragma region Functions to calculate nearest and farthest points from specified objects
	 double Nearest_Point_to_Point(double *PointsList, int NoOfPts, double *Point, double *NearestPt);
	 double Nearest_Point_to_Line(double *PointsList, int NoOfPts, double *Line, double *NearestPt);
	 double Nearest_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *NearestPt);
	 double Nearest_Point_to_Sphere(double *PointsList, int NoOfPts, double *Sphere, double *NearestPt);
	 double Nearest_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *NearestPt);
	 double Farthest_Point_to_Point(double *PointsList, int NoOfPts, double *Point, double *FarthestPt);
	 double Farthest_Point_to_Line(double *PointsList, int NoOfPts, double *Line, double *FarthestPt);
	 double Farthest_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *FarthestPt);
	 double Farthest_Point_to_Sphere(double *PointsList, int NoOfPts, double *sphere, double *FarthestPt);
	 double Farthest_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *FarthestPt);
	 double Least_DirectionalDist_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *NearestPt);
	 double Least_DirectionalDist_Point_to_Sphere(double *PointsList, int NoOfPts, double *Sphere, double *NearestPt);
	 double Least_DirectionalDist_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *NearestPt);
	 double Most_DirectionalDist_Point_to_Plane(double *PointsList, int NoOfPts, double *Plane, double *NearestPt);
	 double Most_DirectionalDist_Point_to_Sphere(double *PointsList, int NoOfPts, double *Sphere, double *NearestPt);
	 double Most_DirectionalDist_Point_to_Cylinder(double *PointsList, int NoOfPts, double *Cylinder, double *NearestPt);
	 double Smallest_Angle_of_Cone(double* PointsList, int NoOfPts, double* Cone, double* Pt);
	 double Largest_Angle_of_Cone(double* PointsList, int NoOfPts, double* Cone, double* Pt);
#pragma endregion

#pragma region functions to calculate GD&T values in threeD
	 double Straightness(double *PointsList, int NoOfPts, double *Line);
	 double FilteredStraightness(double *PointsList, int NoOfPts, double *Line, int filterfactor);
	 double Angularity_Line_wrt_Line(double *Line, double *RefLine, double Angle, double Land);
	 double Flatness(double *PointsList, int NoOfPts, double *Plane);
	 double FilteredFlatness(double *PointsList, int NoOfPts, double *Plane, int filteredfactor);
	 double Angularity_Plane_wrt_Plane(double *Plane, double *RefPlane, double Angle, double Land);
	 double Cylindricity(double *PointsList, int NoOfPts, double *Cylinder);
	 double FilteredCylindricity(double *PointsList, int NoOfPts, double *Cylinder, int filterfactor);
	 double Conicality(double* PointsList, int NoOfPts, double* Cone);
	 double Coaxiality(double *curLinePnts, double *refLine, double Land);
	 double Angularity_Line_wrt_Plane(double *Line, double *Plane, double Angle, double Land);
#pragma endregion

#pragma region functions to add/subtract distances radially to cylinder/plane/sphere for the purpose of probe
       void Plane_SurfacePt_for_Probe(double *Plane, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt);
       void Sphere_SurfacePt_for_Probe(double *SphereCentrePt, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt);
       void Cylinder_SurfacePt_for_Probe(double *CylinderAxisLine, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt);
	   void Torus_SurfacePt_for_Probe(double *TorusParam, double *ProbePt, double ProbeRadius, double *SurfacePt);
	   void Cone_SurfacePt_for_Probe(double *ConeParam, double *ProbePt, double ProbeRadius, bool InnerSurface, double *SurfacePt);
#pragma endregion

#pragma region functions for dxf Shape loop Arrangement
	   void ArrangeDXFShapesInOrder(double *ShapePts, int *ShapeID, int NoOfShapes, std::list<std::list<int>*> *ClusterList);
	   void GetDXFClosedLoopFlipState(double *ShapePts, int NoOfShapes, bool *FlipStateArray);
#pragma endregion

#pragma region Base plane allignment function
	   void TransformationMatrix_BasePlaneAllignment(double *BasePlane, double *BaseLine, double *BasePoint, double *OtherPlane, double *OtherLine, double *OtherPoint, double *TransMatrix, double *BaseOrigin, double *OtherOrigin, bool usingFrameGrab);
#pragma endregion

#pragma region Mirroring function
	   void Mirror_Pt_wrt_Plane(double *Point, double *Plane, double *MirrorPoint);
#pragma endregion

#pragma region IGES Alignment
	   // function to calculate transformation matrix for 1 Line alignment...........
	  void TransformationMatrix_IGESAlignment_1Line(double *igesline1,double *userline1, double *userpt1, double *transformMatrix);

	   // function to calculate transformation matrix for pointline alignment...........
	  void TransformationMatrix_IGESAlignment_1Pt1Line(double *igesline1, double *igespt1, double *userline1, double *userpt1, double *transformMatrix);

	  // function to calculate direction cosines of line if end points are given........point1 = x1, y1, z1, x2, y2, z2......
	  void DCs_TwoEndPointsofLine(double *point1, double * ans);
	  // function to calculate determinant of 3*3 matrix....
	  double Determinant_3x3Matrix(double *matrix);
#pragma endregion


#pragma region Shortest Path algorithm for Partprogram 
	void ShortestPathForPP(double *PPLocationPts, int *PtID, int NoOfPts, int *ArrangedIDs);
#pragma endregion

#pragma region function for circular path movement intermediate points
	bool Circle_3Pt_3D(double* P, double* Ans);
	bool GetIntermediateCirclePts(double* currentpos, double* intpt, double* target, int noofintstops, double* intermediatePts);
#pragma endregion
	void MousePntsForLineIntersection(double *endPnts1, double *endPnts2, double *IntersectPnts, double *mousePnts, double *MeasurePoints);
	void ReflectedPointsAlongPlane(double *points, int pointsSize, double *reflectionSurfaceParam, double *reflectedPoints);
	void ReflectedPointsAlongLine(double *points, int pointsSize, double *reflectionSurfaceParam, double *reflectedPoints);

	bool MultiplyMatrix1(double *M1, int *S1, double *M2, int *S2, double *answer);
	void Intersection_Cylinder_Cone(double *cylinderParam, double *coneParam, double *cylinderCenters, double *coneCenters, double phi_min, double phi_max, double minIncrementFactor, double incrementFactor, double wupp, std::list<double> *IntersectionPoint);
	void Intersection_Sphere_Cone(double *sphereParam, double *coneParam, double *coneCenters, double phi_min, double phi_max, double minIncrementFactor, double incrementFactor, double wupp, std::list<double> *IntersectionPoint, double *avgZ);
	bool Intersection_Cylinder_Plane(double *cylinderParam, double *cylinderCenters, double *planeParams, double *planeCenter, double phi_min, double phi_max, double wupp, std::list<double> *IntersectionPoint);
	bool Intersection_Cone_Plane(double *coneParam, double *coneCenters, double *planeParams, double *planeCenter, double phi_min, double phi_max, double wupp, std::list<double> *IntersectionPoint, bool chkInBlock = true);
	void RotationAround3DAxisThroughOrigin(double angleToRotate, double *axisDir, double *rMatrix);
	void CrossProductOf2Vectors(double *vect1, double *vect2, double *resultVect);
	void MovePointInGivenDirection(double *pnt, double *dir, double dist, double *movdPnt);

#pragma region Finding outermostCloudPoints from another CloudPoint Shape 
	void LeftMostCloudPoints(double *pnts, int NoOfPts, double *line, std::list<double> *tmpList, int noOfCordinate = 3);
	void RightMostCloudPoints(double *pnts, int NoOfPts, double *line, std::list<double> *tmpList, int noOfCordinate = 3);
    void TopMostCloudPoints(double *pnts, int NoOfPts, double *line, std::list<double> *tmpList, int noOfCordinate = 3);
	void BottomMostCloudPoints(double *pnts, int NoOfPts, double *line, std::list<double> *tmpList, int noOfCordinate = 3);
#pragma endregion

#pragma region Rotary Axis Functions -Hob and TIS
	void RotatePoint_Around_C_Axis(double *OriginalPt, double* Axis, double* Centre, double* RotatedPt); //
	void RotatePoint_Around_C_Axis_BC(double *OriginalPt, double* Axis, double* RotatedPt);
	void UnRotatePoint_Around_C_Axis_BC(double* RotatedPt, double* Axis, double* OriginalPt);
	void GetC_Axis_XY(double ZPos, double* Axis, double *Ans);
	void UnRotatePoint_Around_C_Axis(double* RotatedPt, double* Axis, double* Centre, double* OriginalPt); //
	void Correct_BC_Tilt(double* Point, double* BC_Param);
	void Restore_BC_Tilt(double* Point, double* BC_Param);
	void Rotate_Point_2D(double* OriginalPt, double* Axis, double * RotatedPt, double Angle);

#pragma endregion

};
