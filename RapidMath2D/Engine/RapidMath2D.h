//Class Does all the Mamthematical calculations used in Rapid-I 5.0 //General library
//The basic Shapes used in Rapid-I 5.0 are circle, arc, point, lines(Finite line, Infinte line, Ray)
//This class mainly include 
// * All type of measurement(Normal/ Linear) calculations between the Shapes...
// * All Derived Shape Parameter calcualtions...
// * Shape to Shape/ Other Shape intersection Points calcuilations..
// * Tranforamtion matrix calculation.. One point, 2 Point, 2 line, 1point1line etc...
// * Nearest/Farthest Point to the given Shape...
// * IsInwindow calcualations to all the shapes..(To select the shapes inside a rectangle)
// * Distance pf Mouse point to the Shapes.. to Select/Highlight the Shape...
// * Point on Shapes...
// * FrameGrab Calculations(Enclosing rectangle...)
// * Matrix Operations: Addition, Subtraction, Multiplication etc...
// * Functions to calculate the Extension line to Show the virtual snap points...
// 
//Measurements like 
// * Point to Point Distance..(Normal/ Linear)
// * Point to other shape distaces
// * Similarly all combination of shapes..// etc..


#ifdef RAPIDMATH2D_EXPORTS
#define RapidMath2D_API  __declspec(dllexport)
#else
#define RapidMath2D_API  __declspec(dllimport)
#endif

#define IsEqual(x, y, z) (abs(x - y) < z ? 1 : 0)
#define IsGreater(x, y, z) ((x - y) > z ? 1 : 0)
#define IsGreaterOrEqual(x, y, z) ((((x - y) > z) || (abs(x - y) < z))  ? 1 : 0)
//#define IsInBetween(x, a, b) (((x > a && x < b) || (x > b && x < a)) ? 1 : 0)

#define IsInBetween(x, a, b, z) (((IsGreaterOrEqual(x, a, z) && IsGreaterOrEqual(b, x, z)) || (IsGreaterOrEqual(x, b, z) && IsGreaterOrEqual(a, x, z))) ? 1 : 0)

class RapidMath2D_API RapidMath2D
{
public:
	RapidMath2D();//Constructor..//
	~RapidMath2D();//Destructor..//

#pragma region Error Handling.
	// To Handle the error generated...
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	void (*RMath2DError)(char* Rmaterrorcode, char* RmathFileName, char* RmatFunName);
#pragma endregion

	double MeasureDecValue;

#pragma region Distance Calculations..Between Shapes
	//Calcualtes the Point to Point distance..
	//pointer to two points point(x, y)..returns the distance//
	double Pt2Pt_distance(double *point1, double *point2);
	//point1(x1, y1) and point2(x2, y2) points:: returns the distance//
	double Pt2Pt_distance(double x1, double y1, double x2, double y2); 
	//Linear measurement calcualtions.. X and Y shift between two points..
	double LPt2Pt_distance(double *point1, double *point2, bool CurrentStatus, bool *NextStatus, double *mousept);
	//Linear measurement(X and Y Shifts) with some slope...//
	double LXPt2Pt_distance_angle(double *point1, double *point2, double slope);
	double LYPt2Pt_distance_angle(double *point1, double *point2, double slope);
	//Point to line distance calcualtion..
	//point : x, y, slope and intercept : Function returns the distance.
	double Pt2Line_Dist(double x, double y, double slope, double intercept); 
	//Distance is not absolute.. give +/- distance./
	double Pt2Line_DistSigned(double x, double y, double slope, double intercept); 
	//Line to line distance calcualtion.. Only for Parallel lines..
	//Input >> two line parameters,   result>> dist
	double Line2Line_dis(double L1slope, double intercept1, double L2slope, double intercept2);
	//point to circle distances with respect to type//
	//pointer to the point(x, y), center and radius are the circle parameters
	double Pt2Circle_distance_Type(double *point, double *center, double radius, int type);
	//Circle to Point linear distances.6 distances..//
	//Input: circle, point parameters and type
	double LPt2Circle_distance_Type(double *point, double *center, double radius, int type);
	//Line to Circle Distance w.r.t type..
	//slope and intercept are the line parameters. center and radius are the circle parameters
	double Line2Circle_distance_Type(double slope, double intercept, double *center, double radius, int type);
	//Circle to circle distance calculation
	//center1, center2, radius1, radius2 are the circle parameters.
	double Circle2Circle_distance_Type(double *center1, double *center2, double radius1, double radius2, int type);
	double Circle2Circle_distance_NewType(double *center1, double *center2, double radius1, double radius2, int type1, int type2);
	//Circle to circle Linear measurment calculations... 6 distances...
	//Input: two circle parameters and type.
	double LCircle2Circle_distance_Type(double *center1, double *center2, double radius1, double radius2, int type);
	double AngleBwTwoPtswrtcenter(double x1, double y1, double x2, double y2, double centerx, double centery);		//vinod	AngleBw Two Pts wrt center Pt. 0 to 2PI
	double AngleofLineWithTwoPtswrtXaxis0To2PI(double x1, double y1, double x2, double y2);		//vinod.. Angle of Line With Two Pts wrt Xaxis 0 To 2PI
	//Calculate the zone of a given point between two given lines
	int GetZoneOfPtBetweenLines(double* L1, double* L2, double* p, double* ans, double* Radius);
	void GetPtInTwoLineQuadrant(double Angle1, double Angle2, int Zone, double* Centre, double rad, double* mPt);

#pragma endregion

#pragma region LineAngle Calculation Mousemove
	//Infinite Line Angles with coordinate axis with respect to the Mousepoint(4 angles)//
	//Input: line parameters, mousepoint.
	double Line_slopemousept(double slope, double intercept, double *mousept);
	//Line to line angle calculation. different combination of lines..
	//Input: two line parameters.. mousepoint, result: angle
	double Line2Line_angle(double L1slope, double intercept1, double L2slope, double intercept2, double *mousept);
#pragma endregion 

#pragma region Circle and Arc parameter Calcualtions
	//Input >> point1, point2, point3,   Result >> center, radius...
	bool Circle_2Pt(double *point1, double *point2, double *center, double *radius);
	bool Circle_3Pt(double *point1, double *point2, double *point3, double *center, double *radius);
	//Input >> point1, point2, point3,   Result >> center, radius, startang, sweepang...
	int Arc_3Pt(double *point1, double *point2, double *point3, double *center, double *radius, double *startang, double *sweepang);
	//Arc with Fixed Radius
	bool Arc_FixedDia(double *center, double *point1, double *mousept, double radius, double *startang, double *sweepang, bool Anticlockwise);
	//1 point Circle with Fixed Diameter..
	bool Circle_1Pt_FixedDia(double *point1, double diameter, double *mousept, double *point2);
	//3 Point Circle with Fixed Diameter..
	bool Circle_3Pt_FixedDia(double *point1, double *point2, double radius, double *mousept, double *center, double *point3);
#pragma endregion

#pragma region Angle Conversion Functions
	//Convert from radian degree to decimal degree..
	double Radian2Degree(double radian_val);
	//Convert from radian degree to deg_ min_ sec..
	bool Radian2Deg_Min_Sec(double radian, char* Anglev, int NodigitsDeg = 0);
	//Decimal degree to radian..
	double Degree2Radian(double Decideg);
	//Deg_min_sec to radian conversion..
	double Deg_Min_Sec2Radian(char* degree);
#pragma endregion

#pragma region Double to String, string to string Conversion Functions
	//concat 1st and 2nd string and save in 3rd
	bool Concat2stringin3rd(std::string& tempStr1, std::string& tempStr2, char* tempStr3);
	//Convert from double to String..
	bool Double2String(double doub_value, int NoofDec, char* Conv_Str, bool Check_DecimalSeperateor = true);
	//Convert string to wstring conversion
	std::wstring StringToWString(const std::string& Str);
	//Convert wstring to string covnersion
	std::string WStringToString(const std::wstring& Str);
	//Convet char* to string;
#pragma endregion

#pragma region Matrix Operations
	//inputs:: M1,S1 and M2, S2 are the matrix element and size, Ans is the resultant matrix..
	bool AddMatrix(double *M1, int *S1, double *M2, int *S2, double *Ans);
	//inputs M1,S1 and M2, S2 are the matrix element and size, Ans is the resultant matrix..
	bool SubMatrix(double *M1, int *S1, double *M2, int *S2, double *Ans);
	//Multiply the given matrix by a number... ans is the resultant matrix...
	bool Multiplication_number(double *M1, int *S1, int num, double *Ans);
	//Divide the given matrix by a number.. ans is the resultant matrix...//
	bool Division_number(double *M1, int *S1,int num, double *Ans);
	//Performs the multiplication between two matrices....
	bool MultiplyMatrix1(double *M1, int *S1, double *M2, int *S2, double *Ans);
	//Cholesky Matrix calculation...
	bool Cholesky_matrix(double *M1, int size, double*L1);
	//Transpose of a Matrix..
	bool TransposeMatrix(double *InputMatrix, int size, double *TransposedMatrix);
	//Inverse / determinant of a matrix..
	double OperateMatrix4X4(double* matrix, int Size, int Flag, double* Answer);
	//Load identity matrix..
	bool LoadIdentityMatrix(double* matrix, int size);
#pragma endregion

#pragma region Derived Shape Calculation
	//Parent line paramters, mousepoint and length..//End points are output..//
	void Parallelline(double slope, double intercept, double *mousepoint, double len,  double *endpt1, double *endpt2,double *offset, bool flag, int LinePosition = 0);
	//Parent line parameters, mousepoint, length..//End points are the out puts..//
	void Perpedicularlline(double slope, double intercept, double *mousepoint, double len, double *endpt1, double *endpt2, int LinePosition = 0);
	//line with slope and width..
	void EquiDistPointsOnLine(double slope, double len, double *point, double *endpt1, double *endpt2, int MousePosition = 0);
	//Angle bisector of two lines...//Input >> two line parameters, mouse point, length
	void Angle_bisector(double slope1, double intercept1, double slope2, double intercept2, double *mousepoint, double length, double *endpt1, double *endpt2, int LinePosition = 0); 
	void Angle_bisector(double slope1, double intercept1, double slope2, double intercept2, double *mousepoint, double *slope, double *intercept); 
	//Midpoint of two points...//
	void Mid_point(double *endpt1, double *endpt2, double *midpoint);	
	//Tangent to a circle nearest to the mouse point..//
	void Tangent2aCircle(double *center, double radius, double *point, double lengthoftan, double *endpt1, double *endpt2, int LinePosition = 0);
	//Tangent to a circle parllel to the given line..//
	void Tangent_Parallel(double *center, double radius, double slope, double intercept, double length, double *point, double *endpt1, double *endpt2, int LinePosition = 0);
	//Tangent to a cirlce perpendicular to the given line..
	void Tangent_Perpendicular(double *center, double radius, double slope, double intercept, double length, double *point, double *endpt1, double *endpt2, int LinePosition = 0);
	//Tangent to a cirlce at  given point...//
	void Tangent_ThruPoint(double *center, double radius, double *tanpoint, double *mousept, double length, double *endpt1, double *endpt2, int LinePosition = 0);
	//Parallel tangent to two given circles//
	void Tangent_TwoCircles(double *center1, double radius1, double *center2, double radius2,double length, double *refpoint,double *endpt1,double *endpt2);

	//Circle tangtial to two given lines...//
	void Circle_Tangent2TwoLines(double slope1, double intercept1, double slope2, double intercept2, double *radius, double *point, double *center);
	//Circle Inside a triangle..
	void Circle_InsideATriangle(double slope1, double intercept1, double slope2, double intercept2, double slope3, double intercept3, double *circleParam);
	//Circle outside a triangle..
	void Circle_Circum2Triangle(double slope1, double intercept1, double slope2, double intercept2, double slope3, double intercept3, double *circleParam);
	//circle tangential to given circle... with mousepoint..
	void Circle_Tangent2Circle(double *center, double radius, double *mousept, double *cen, double *radius1);
	//Circle tangential to other circle with given radius....//
	void Circle_Tangent2CircleFixedDia(double *center, double radius, double radius1, double *mousept, double *center1);
	//Tangential circles
	void Circle_Tangent2Circles(double *center1, double radius1, double *center2, double radius2, double *mousept, double *radius_circle, double *center);
	void Circle_Tangent2Circles_Outside(double *center1, double radius1, double *center2, double radius2, double *mousept, double *radius_circle, double *center);
	void Circle_Tangent2Circles_Inside(double *center1, double radius1, double *center2, double radius2, double *mousept, double radius_circle, double *center);
	void Circle_Tangent2LineCircle(double *center, double radius, double slope, double intercept, double *mousept, double radius_circle, double *rcenter);
	//Fillet for lines ..
	bool FilletForTwoLines(double slope1, double intercept1, double *endpt11, double *endpt12, double slope2, double intercept2, double *endpt21, double *endpt22, double radius, double *center);
	//Fillet for arcs
	bool FilletForTwoArcs(double *center1, double radius1, double startang1, double sweepang1, double *center2, double radius2, double startang2, double sweepang2, double radius, double *Arc1Points, double *Arc2Points, double *FilletArc);
	bool InternalFilletArctoArc(double *center1, double radius1, double startang1, double sweepang1, double *center2, double radius2, double startang2, double sweepang2, double *tancenter, double tanradius, double *Arc1Points, double *Arc2Points, double *FilletArc);
	//Fillet for line and arc..
	bool FilletForLineArc(double slope1, double intercept1, double *endpt11, double *endpt12, double *center1, double radius1, double startang1, double sweepang1, double radius, double *Arc1Points, double *FilletArc);
	bool InternalFilletLinetoArc(double slope1, double intercept1, double *endpt11, double *endpt12, double *center1, double radius1, double startang1, double sweepang1, double *tancenter, double tanradius, double *Arc1Points, double *FilletArc); 
	//Check whether circle inside other circle..
	bool CircleInsideAnotherCircle(double *center1, double radius1, double *center2, double radius2);
#pragma endregion

#pragma region Nearest/Farthest Point Calculation
	//Nearest point to given point from set of points..//
	double Nearestpoint_point(int totalpoints, double *setofpoints, double *point, double *nearestpoint);
	//Nearest point to given line from set of points...
	double Nearestpoint_line(int totalpoints, double *setofpoints, double slope, double intercept, double *nearestpoint);
	//Nearest Point to circle from points collection..
	double Nearestpoint_circle(int totalpoints, double *setofpoints, double *center, double radius, double *nearestpoint);
	//Farthest Point to a given point from set of points..
	double Farthestpoint_point(int totalpoints, double *setofpoints, double *point, double *farthestpoint);
	//Farthest point to a given line from  set of points...//
	double Farthestpoint_line(int totalpoints, double *setofpoints, double slope, double intercept, double *farthestpoint);
	//Farthest point to cirlcle from set of points
	double Farthestpoint_circle(int totalpoints, double *setofpoints, double *center, double radius, double *farthestpoint);
#pragma endregion

#pragma region Shape to Shape Intersection Calculations

#pragma region Line to line intersections
	//Infinite line to Infinie line intersection point..
	int Line_lineintersection(double slope1, double intercept1, double slope2, double intercept2, double *intersectnpt);
	//Infinite line to finite line intersection...
	int Line_finiteline(double slope1, double intecept1, double *endpt1, double *endpt2, double slopeofline, double intercept2, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Ray to Infinite line intersection point...//
	int raylineint(double slopeofline, double intercept, double *point, double slopeofray, double interceptofray, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Ray to finite line intersection point....
	int ray_finitelineint(double *endpt1, double *endpt2, double slopeofline, double intercept, double *point, double slopeofray, double interceptofray, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Ray to ray intersection point.....
	int ray_rayintersec(double *point, double slopeofray, double intercept, double *point1, double slopeofray1, double intercept1, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Finite line to finite line intersection point...
	int Finiteline_line(double *endpt1, double *endpt2, double *endpt3, double *endpt4, double slope1, double intercept1, double slope2, double intercept2, double *intersectnpt1, double *intersectnpt2, int *count);
#pragma endregion

#pragma region Line to Circle, Arc intersections
	//Infinite line to circle intersection point...
	int Line_circleintersection(double slope, double intercept, double *center, double radius, double *intersectnpt);
	//Finite line to circle intersection point....
	int Finiteline_circle(double *endpt1, double *endpt2, double *center, double radius, double *intersectnpt, int *Count);
	int Finiteline_circle(double *endpt1, double *endpt2, double slope1, double intercept1, double *center, double radius, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Ray to circle intersection point...
	int ray_circleint(double *center, double radius, double *point, double slopeofray, double intercept, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Infinite line to Arc intersection point...//
	int Line_arcinter(double slope, double intercept, double *center, double radius, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Finite line to Arc intersection...
	int Finiteline_arcinter(double *endpt1, double *endpt2, double slope, double intercept, double *center, double radius, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count);
	//Ray to arc intersection...
	int ray_arcinter(double *point, double slopeofray, double intercept, double *center, double radius, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count);
#pragma endregion

#pragma region Cirlce to Circle, Arc intersections
	//circle to circle intersection point...
	int Circle_circleintersection(double *center1, double *center2, double radius1, double radius2, double *intersectnpt);
	//Circle to arc intersection point...
	int Circle_arcintersection(double *center1, double radius1, double *center2, double radius2, double startang, double sweepang, double *intersectnpt1, double *intersectnpt2, int *Count );
	//Arc to arc intersection point...
	int arc_arcintersection(double *center1, double radius1, double startang1, double sweepang1, double *center2, double radius2, double startang2, double sweepang2, double *intersectnpt1, double *intersectnpt2, int *Count);
#pragma endregion
#pragma endregion

#pragma region ExtensionLine Calculations
	//Functions to calculate the points to draw the extension lines for intersection points//
	//Finite Line to Fininte line intersection..
	int Finitelineto_Finiteline(double *point1, double *point2, double *point3, double *point4, double *point, double *points);
	//Arc to arc intersection..//
	int Arcto_Arc(double *center1, double radius1, double *center2, double radius2, double *endpoints1, double *endpoints2, double *point, double *points);
	//Ray to ray itersection...//
	int rayto_ray(double *pt1, double slope1, double *pt2, double slope2, double *point, double *points);
	//Inifinite line to finite line intersection ...
	int Lineto_finiteline(double *pt1, double *pt2, double *point, double *points);
	//Finite line to ray intersection..
	int Finitelineto_ray(double *pt1, double *pt2, double *pt, double slope, double *point, double *points);
	//Finite line to circle intersection...//
	int Finitelineto_Circle(double *pt1, double *pt2, double *center, double radius, double *point, double *points);
	//Finite Line to arc intersection... 
	int Finitelineto_arc(double *pt1, double *pt2, double *center, double radius, double *angles, double *point, double *points);
	//Ray to arc intesection...//
	int Rayto_Arc(double *pt, double slope, double *center, double radius, double *angles, double *point, double *points);
	//Finite line to intersection..//
	int Infinitelineto_Arc(double *center, double radius, double *angles, double *point, double *points);
	//Circle to arc intersection...//
	int Circleto_arc(double *center, double radius, double *angles, double *point, double *points);
#pragma endregion

#pragma region Functions to Check Shape Highlight/ Shape Is in Window
	//Functions to highlight the shapes according to the mouse position.....//	
	//Mouse point to line distance...
	bool MousePt_OnFiniteline(double *endpt1, double *endpt2, double slope, double intercept, double *point, double sp, bool *ShapeforImplicit);
	//Mouse point to Ray distance...
	bool MousePt_OnRay(double *pt, double slope, double intercept, double *point, double sp);
	//Mouse point to Infinite line distance...
	bool MousePt_OnInfiniteline(double slope, double intercept, double *point, double sp);
	//Mouse point to circle distance..
	bool MousePt_OnCircle(double *center, double radius, double *point, double sp);
	//Mouse to arc distance...
	bool MousePt_OnArc(double *center, double radius, double startang, double sweepang, double *endpt1, double *endpt2, double sp, double *Mousepoint, bool *ShapeforImplicit);	
	//Point is in Rectangle window...
	bool PtisinTriangle(double *point, double *Vertex1, double *Vertex2, double *Vertex3);
	bool Ptisinwindow(double x, double y, double minx, double miny, double maxx, double maxy);
	bool PtisinCube(double x, double y, double z, double minx, double miny, double minz, double maxx, double maxy, double maxz);
	bool Ptisinwindow(double *point, double *corner1, double *corner2);
	bool PtisinwindowRounded(double x, double y, double *corner1, double *corner2);
	//Circle is in Rectangle window...
	bool Circle_isinwindow(double *center, double radius, double *point1, double *point2, double *point3, double *point4);
	//Infinte line is in Rectangle window..
	bool Inline_isinwindow(double slope, double intercept, double *point1, double *point2, double *point3, double *point4);
	//Ray is in window..//
	bool ray_isinwindow(double *pt,double slope, double intercept, double *point1, double *point2, double *point3, double *point4);
	//Finite line is in Rectangular window..//
	bool Fline_isinwindow(double *endp1, double *endp2, double slope, double intercept, double *point1, double *point2, double *point3, double *point4);
	//Arc is in Rectangular window..//
	bool arc_isinwindow(double *center, double radius, double startang, double sweepang, double *endp1, double *endp2, double *point1, double *point2, double *point3, double *point4);
	//Rectangle is in Rectangle window
	bool Rect_isinwindow(double *p1, double *p2, double *p3, double *p4, double *point1, double *point2, double *point3, double *point4);
	//Rectangle in any orientation..
	bool PtisinRectangle(double *point1, double *point2, double *point3, double *point4, double *mousept);
	//Intersection function , used internally , to check the is in window...//
	bool Finiteline_circle1(double *endpt1, double *endpt2, double slope, double intercept, double *center, double radius);
	bool Line_finiteline1(double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2);
	bool ray_finitelineint1(double *point, double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2);
	bool Finiteline_line1(double *endpt1, double *endpt2, double slope1, double intercept1, double *endpt3, double *endpt4, double slope2, double intercept2);
	bool Finiteline_arcinter1(double *center, double radius, double startang, double sweepang, double *endpt1,double *endpt2, double slope, double intercept);
#pragma endregion
	
#pragma region Statistics
	//Statistics calculations...//
	void statistics(int n, double *values, double *mean, double *median, double *variance, double *stddev);
	//Sxx and Sxy values...//
	void Sxxvalue(int n, double *values, double *Sxx);
	void Sxyvalue(int n, double *valuesX, double *valuesY, double *Sxy);
#pragma endregion

#pragma region Ray Angle Functions.
	//calculate angle in 360 coordinates system.. between two points..
	double ray_angle(double *point1, double *point2);
	double ray_angle(double x1, double y1, double x2, double y2);
	double InterceptOfline(double slope, double *point);
#pragma endregion

#pragma region Area Calculations.
	//Added on 22 March 2011...
	double AreaOfPolygon(double *ShapeParam, bool *Shapetype, int totalnoofshapes);
	double AreaofTriangle(double *point1, double *point2, double *point3);
	//Area of the arc... sector..
	double AreaofSector(double angle, double radius);
	//Area of portion of the circle ..
	double AreaofPartofCircle(double *ShapeParam);
	//Swap the end points for area.. If required...!
	void SwapEndpointsForArea(double *ShapeParam, int totalnoofshapes);
#pragma endregion
	
#pragma region Framegrab and End Point Calculations
	//Arc framegrab calculations...
	void ArcFrameGrabEndPts(double *center, double radius1, double radius2, double startang, double sweepang, double *endpt1, double *endpt2);
	//Arc FG end point calcualtions..//
	void ArcEndPoints_Fill(double *point1, double *point2, double width, double *endpt1, double *endpt2);
	//Functions to calcualte the circle parameters for FG//
	void CircleParameters_FG(double *point1, double *point2, int width, double *C_array);
	//Functions to calcualte the arc parameters for FG//
	void ArcParameters_FG(double *point1, double *point2, double *mousept, int space, double *C_array);
	//Caculations for rectangle frame grab...
	void RectangleFG(double *point1, double *point2, double sp, double wup, double *points, bool flag = true);
	void RectangleFGDirection(double *pt1, double *pt2, int direction, double wup, double *points);
	//Arc end points calculations with start angle and sweep angle...//
	void ArcEndPoints(double *center, double radius, double startangle, double sweepangle, double *endpts);
	//Arc best fit.. Angles calculation
	void ArcBestFitAngleCalculation(double *points, int size, double *center, double radius, double *angles);
	//Line end points calculations after the frame grab//
	void LineEndPoints(double *points, int size, double *endpt1, double *endpt2, double angle);
	//Function to calculate the minimum value in a array...
	double minimum(double *values, int n);
	//Function to calculate the maximum value in a array..
	double maximum(double *values, int n);
#pragma endregion

#pragma region Enclosing Rectangle Calculations
	//Function to get the Rectangle Enclosing given Circle
	void RectangleEnclosing_Circle(double *center, double radius, double *Lefttop, double *RightBottom);
	//Function to get the Rectangle Enclsoing for line
	void RectangleEnclosing_Line(double *point1, double *point2, double *Lefttop, double *RightBottom);
	//Function to get the Rectangle Enclosing given arc
	void RectangleEnclosing_Arc(double *center, double *endpt1, double *endpt2, double radius, double startang, double endangle, double *Lefttop, double *RightBottom);
	
	bool AngleIsinArcorNot(double startangle, double endangle, double angle);
	//Get left top and right bottom of two points
	void GetTopLeftNBtnRht(double *point1, double *point2, double *Lefttop, double *RightBottom);
	//get left top and right bottom of set of points..
	void GetTopLeftAndRightBottom(double* Points, int Pointscount, int pointsize, double* LeftTop, double* RightBottom);
#pragma endregion
	
#pragma region Transformation Matrix For Alignment.
	//Transformation Matrix for One Point Alignment.
	void TransformM_OnePtAlign(double *point1, double *point2, double *TransformMatrix);
	//Function to calculate transformation matrix to Rotate about a Point..
	void TransformM_RotateAboutPoint(double *point, double angle, double *TransformMatrix);
	//Transformation Matrix for Two Point Alignment.
	void TransformM_TwoPointAlign(double *point1, double* point2, double *point3, double *point4, double *TransformMatrix, double *RotatedAngle, double *ShiftedPt);
	//Transformation Matrix for One Point One Line Alignment.
	void TransformM_PointLineAlign(double *point1, double angle1, double intercept1, double *point2, double angle2, double intercept2, double *TransformMatrix, double *RotatedAngle, double *ShiftedPt);
	//Transformation Matrix for Two Line Alignment.
	void TransformM_TwoLineAlign(double* Line1Param, double* Line2Param, double* Line3Param, double* Line4Param, double *TransformMatrix, double *RotatedAngle, double *ShiftedPt);
	//Rotate a 2D Point about an axis point by a given angle
	void Rotate_2D_Pt_Origin(double* givenPt, double* centre, double RotateAngle, double *answer);
	void Rotate_2D_Pt(double* givenPt, double* centre, double RotateAngle, double *answer);
	void UnRotate_2D_Pt(double* givenPt, double* centre, double RotateAngle, double *answer);

#pragma endregion

#pragma region Angle Conversion In Quadrant
	//Angle between  0 to 180//
	void Angle_FirstScndQuad(double *slope);
	//Angle between -90 to +90//
	void Angle_FirstFourthQuad(double *slope); 
	//Angle between to 0 to 360//
	void Angle_FourQuad(double *slope); 
	//Arc Angle difference between two angles ( 0 to 360)
	void ArcAngleDiff(double angle1, double angle2, double *angle);
#pragma endregion

#pragma region Line, Circle Related Functions
	//Perpendicular intersection of Point on Line
	void Point_PerpenIntrsctn_Line(double slope, double intercept, double *point, double *intersectn);
	//Farthest point to given line on both sides and Distance between them.
	double StraightNessofLine(int totalpoints, double *setofpoints, double slope, double intercept);
	double FilteredStraightNessofLine(int totalpoints, double *setofpoints, double slope, double intercept, int filterFactor);
	//Line Calculation for Shift ON..
	void LineForShiftON(double angle, double *endpt1, double *endpt2, double *rpoint, double *rangle, double *rintercept);
	//Horizontal or Vertical calculation  for Digital Micrometer Measurement..
	void LineHorizontal_Vertical(double angle, double *pt1, double *pt2, double *rpoint, double *rangle, double *rintercept, bool Horizon = true);	
	//Line Angularity Calculation.
	void AngularityCalculation(double angle1, double angle2, double land, double referenceangle, double *angularity);
	//Function to calculate the intercept of the line
	void Intercept_LinePassing_Point(double *pt, double slope, double *intercept);
	//Slope and intercept of the two point line..//
	void TwoPointLine(double *point1, double *point2, double *slope, double *intercept);
	//Point on Circle....//
	void Point_onCircle(double *center, double radius, double *mousept, double *point);
	//Point on Arc calculations..
	bool Point_onArc(double *center, double radius, double startangle, double sweepangle, double *mousept, double *point);
	bool Projection_Of_Point_OnArc(double* Center, double Radius, double StartAngle, double EndAngle, double* Point, double* Answer);
	void Projection_Of_Point_Online(double* ProjectablePt, double TargetLineSlope, double targetLineIntercept, double* projectedPt);
	void ReflectPointsAlongLine(double *OriginalPoints, int PtsCnt, double slope, double Intercept, double* ReflectedPts);
#pragma endregion



#pragma region Graphics Drawing Related Calculations.
	
	bool MeasurementDirection;
#pragma region Distance Calculations
	//Calculate point to point distance graphics...
	//Main function: does the calculations for point to point distance..
	//Whatever the distance (measurement except angle).. it finally comes into point to point distance...
	//Input.. point1, point2, mousepoint, *space(width and height of the text), dis: distance between the points..//
	//Output: endpt1, endpt2 ==> are the array to hold the graphics line endpoints...,
	//midpt is the point to draw string(midpt of string), slopear: slope of the arrow...//
	bool Pt2Ptdis(double *point1, double *point2, double *mousept,double *space, double dis,double *endpt1, double *endpt2,double *midpt,double *slopear);
	//Cacualte point to line distance graphics...//
	bool Point_FinitelineDistance(double *point,double slope,double intercept,double *endp1, double *endp2, double *intercept1,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear);
	bool Point_InfinitelineDistance(double *point,double slope,double intercept, double *intercept1,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear);
	bool Point_RayDistance(double *point,double slope,double intercept,double *endp1, double *intercept1,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear);
	
	//function to draw the digital micrometer//
	void Digital_Micrometer(double *point1, double *point2, double dis, double* space, double wupp, double *endpt1, double *endpt2, double* midpt, double *slopear, bool nearMousePt);
	void EditDigitalMicrometer(double *point1, double *point2, double *mousept, double *endpt1, double *endpt2);
#pragma endregion

#pragma region Line To Line Distance Calculations
	//Line to line distance calculations..6 combinations...///
	bool Infiniteline_InfinitelineDistance(double slope1,double intercept1,double slope2,double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear);
	//Finite line to finite line distance ...
	bool Finiteline_FinitelineDistance(double *endp1,double *endp2,double *endp3,double *endp4, double slope1,double intercept1,double slope2,double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear); 
	//Ray to ray distance..//
	bool Ray_RayDistance(double *point1,double slope1, double intercept1, double *point2,double slope2,double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear);
	//Line to finite line distance..//
	bool Infiniteline_FinitelineDistance(double slope1,double intercept1,double *endp1,double *endp2,double slope2, double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear);
	//Line to ray distance...
	bool Infiniteline_RayDistance(double slope1,double intercept1,double *point,double slope2,double intercept2,double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear);
	//finite line to ray distance..//
	bool Finiteline_RayDistance(double *endp1,double *endp2, double slope1, double intercept1, double *point,double slope2,double intercept2, double dis,double *mousept,double *space,double *endpt1, double *endpt2,double *midpt,double *slopear); 
#pragma endregion

#pragma region Line To Line Angle Calculations
	bool Finiteline_FinitelineAngle(double slope1, double intercept1, double *endpt11, double *endpt12, double slope2, double intercept2, double *endpt21, double *endpt22,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear);
	bool Infiniteline_InfinitelineAngle(double slope1, double intercept1, double slope2, double intercept2, double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear);
	bool Ray_RayAngle(double slope1, double intercept1, double *endpt1,double slope2, double intercept2, double *endpt2,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear);
	bool Infiniteline_FinitelineAngle(double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2, double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear);
	bool Infiniteline_RayAngle(double slope1, double intercept1, double *endpt1,double slope2, double intercept2,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear);
	bool Finiteline_RayAngle(double slope1, double intercept1, double *endpt1, double *endpt2, double slope2, double intercept2, double *endpt3,double *mousept,double *space, double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear);
	//line slope modified//
	bool LineSlope(double slope1, double intercept1,double *mousept,double *space,double *radius ,double *center,double *startang, double *sweepang, double *points,double *slopear);

	bool PCDAngleCalculation(double slope1, double intercept1, double slope2, double intercept2, double *center, double *mousepoint, bool drawdir,double *space,double *radius, double *startang, double *sweepang, double *points, double *slopear);
#pragma endregion
	
#pragma region Linear Distances..!
	//Linear distances to circle //
	//Circle to circle distance....//
	bool Circle_CircleDistance_Linear(double *center1,double *center2,double radius1, double radius2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type );
	//circle to point distance..//
	bool Circle_PointDistance_Linear(double *center, double radius,double *point,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type );
	//Linear measurement..//
	bool XY_shift(double *point1,double *point2, bool CurrentStatus, double *mousept, double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear);
#pragma endregion

#pragma region Dia, Radius Measurements
	// Circle radius measurement calculations...//
	bool Circle_radius(double *center,double radius,double *mousept,double *endpt1,double *slopear);
	//Circle diameter measurement calculations..//
	bool Circle_diameter(double *center,double dia,double *mousept,double *space,double *endpt1,double *slopear);
	//Arc radius calculation..
	bool Arc_radius(double *center,double radius,double *mousept,double startang,double sweepang,double *space,double *endpt1, double *LastmpX, double *LastmpY, double *slopear);
	//Arc angle displaing..//
	bool Arc_Angle(double *center, double radius, double startang, double sweepang, double *endp1, double *endp2, double *mousept, double *space, double *points, double *startang1, double *sweepang1, double *r1, double *slopear);
#pragma endregion

#pragma region Circle and Arc Distances..!
	//circle distances modified:: Calcualtion with respect mouse point (only one distance at a time): Synchronization between rapidmath and graphics...//
	bool Circle_PointDistance(double *point,double *center,double radius,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type);
	//Circle to circle distance...//
	bool Circle_CircleDistance(double *center1, double *center2,double radius1,double radius2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type);
#pragma endregion

#pragma region Circle to Line distances
	bool Circle_FinitelineDistance(double slope, double intercept, double *endp1, double *endp2,double *center, double radius, double *intercept1,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type);
	bool Circle_InfinitelineDistance(double slope, double intercept, double *center, double radius, double *intercept1,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type);
	bool Circle_RayDistance(double slope, double intercept, double *endp1,double *center, double radius, double *intercept1,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear, int type);
#pragma endregion

#pragma region Others

	//Calculates the end points of the arrow...//
	bool arrow(double *mousept, double slopear, double *endpt1, double *endpt2, double dis, double Awidth, double *space, double *pt12, double *pt13, double *pt22, double *pt23);
	//partprogram direction//(arrow)
	bool PartProgramDirection(double *point,double *point1,double *rectendpts, double *arrowendpts, double *MidArrowendpts, double width, double height);
	//Partprogram starting point...//
	bool PartProgramStartingPoint(double *point, double *endpoints, double width, double length);
	//-----------------------------------------------------------------------------------------------------//
#pragma endregion

#pragma region Internal Functions
public:
	bool Pt2ptdis1(double *endpt1,double *endpt2,double *space,double dis,double *slope, double *midpt,double *endpt11,double *endpt21);
	bool Pt2ptdis2(double *endpt1,double *endpt2,double *space,double dis,double *slope, double *midpt,double *endpt11,double *endpt21);
	//Arrow functions In different orientations..//
	bool arrow1(double *endpt1,double slope1, double slope2, double *pt12, double *pt13, double w = 40);
	bool arrow2(double *endpt2,double slope1, double slope2, double *pt22, double *pt23, double w = 40);
	
	bool X_shift(double *point1,double *point2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear);
	bool Y_shift(double *point1,double *point2,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear);

private:
	//Points on circle : display the measurement according to mouse point..//
	bool circle_points(double *center,double radius,double slope,double *pt1,double *pt2,double *pt3,double *pt4);
	//Slope and intercept of the line joining points: Used in many functions..//
	bool slopesintercept(double *point1,double *point2,double *mousept,double *slope, double *intercept, double *slope1);
	
	//Linear measurements..//
	//Linear measurement with some angle...//
	bool X_shiftangle(double *point1,double *point2,double slope,double intercept,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear);
	bool Y_shiftangle(double *point1,double *point2,double slope,double intercept,double *mousept,double *space,double dis,double *endpt1, double *endpt2,double *midpt,double *slopear);

	void swaptwopoints(double **point1,double **point2);
	void swaptwopoints(double *point1,double *point2);
	//Get the absolute arc angle..//
	void arcfun(double* temp);
	//Intersection point is the result
	bool intersection(double slope, double intercept, double *point, double *intersectn);
	
	void SwapTwoValues(double* value1, double* value2);
	bool swap1(double *point1,double *point2,double *r1,double *r2);
	bool swap2(double *point1,double *point2);
	bool Lcircle_Points( double *center,double radius,double *pt1,double *pt2,double *pt3,double *pt4 );

	
	double RoundDecimal(double valu, int decPlace);

	bool PtonCircle_Type(double *point,double *center,double radius, int type, double *rpoint);
	bool PtonArc_Type(double *point,double *center,double radius, double startang,double sweepang,int type,double *rpoint);
#pragma endregion

#pragma endregion
public:
	 void SortDoubleArray(double *numbarArray, int low = 0, int length = 0);
	 void SwapArrayElement(double *numbarArray, int first, int second);
	 void Median_Double_Array(double *numbarArray, int ArrayLength, double* median);
	 int GetTransformedProbeDirection(double *transform, int PreviousDir);
	 void PointOnArcForWidth(double *cpoint, double slope, double intercept, double* center, double radius, double startAngle, double swapAngle, double *resultpoints, int *nearestSide, double *midPnts, bool checknearest = false);
};   

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!