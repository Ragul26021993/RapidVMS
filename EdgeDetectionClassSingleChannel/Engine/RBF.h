// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RBF_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RBF_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef RBF_EXPORTS
#define RBF_API __declspec(dllexport)
#else
#define RBF_API __declspec(dllimport)
#endif

#include <list>
#include <fstream>

using namespace std;



struct RBF_API Pt_3D
{
	double x;
	double y;
	double z;
	int pt_index;
	Pt_3D operator =(Pt_3D pt_3d)
	{
		x = pt_3d.x;
		y = pt_3d.y;
		z = pt_3d.z;
		pt_index = pt_3d.pt_index;
		return *this;
	}

	bool operator ==(Pt_3D pt_3d)
	{
		if (pt_3d.pt_index == pt_index)
			return true;
		else 
			return false;
	}
};

struct RBF_API Line_3D
{
	Pt_3D pt_1;
	Pt_3D pt_2;
	bool isBoundary;
	bool isFinalBoundary;
	int line_index;
	Line_3D operator =(Line_3D L1)
	{
		pt_1 = L1.pt_1;
		pt_2 = L1.pt_2;
		isBoundary = L1.isBoundary;
		isFinalBoundary = L1.isFinalBoundary;
		line_index = L1.line_index;
		return *this;
	}

	bool operator ==(Line_3D L1)
	{
		if (((L1.pt_1 == pt_1) && (L1.pt_2 == pt_2)) || ((L1.pt_2 == pt_1) && (L1.pt_1 == pt_2)))
			return true;
		else
			return false;
	}
};

struct RBF_API Triangle_3D
{
	Line_3D line_1;
	Line_3D line_2;
	Line_3D line_3;
	int triangle_index;
	Triangle_3D operator =(Triangle_3D T1)
	{
		line_1 = T1.line_1;
		line_2 = T1.line_2;
		line_3 = T1.line_3;
		triangle_index = T1.triangle_index;
		return *this;
	}

	bool operator ==(Triangle_3D T1)
	{
		if (((T1.line_1 == line_1) && (T1.line_2 == line_2) && (T1.line_3 == line_3)) ||
				((T1.line_1 == line_2) && (T1.line_2 == line_3) && (T1.line_3 == line_1)) ||
				((T1.line_1 == line_3) && (T1.line_2 == line_1) && (T1.line_3 == line_2)) ||
				((T1.line_1 == line_1) && (T1.line_2 == line_3) && (T1.line_3 == line_2)) ||
				((T1.line_1 == line_3) && (T1.line_2 == line_2) && (T1.line_3 == line_1)) ||
				((T1.line_1 == line_2) && (T1.line_2 == line_1) && (T1.line_3 == line_3)) )
			return true;
		else
			return false;
	}
};

typedef list <Pt_3D> Pts_3DList;
typedef list <Line_3D> Line_3DList;
typedef list <Triangle_3D> Triangle_3DList;

struct RBF_API Pt
{
	double x;
	double y;
	int pt_index;
	bool operator ==(Pt pt)
	{
		if (pt.pt_index == pt_index)
			return true;
		else 
			return false;
	}
};

struct RBF_API intPt
{
	int x;
	int y;
};

struct RBF_API LineArc
{
	Pt start_pt;
	Pt end_pt;
	Pt mid_pt;
	double slope;
	double intercept;
	double length;
};

struct RBF_API LineArc_Circle
{
	double Center_X;
	double Center_Y;
	double radius;
};

enum RBF_API refobj
{
	reference,
	obj
};

enum RBF_API direction
{
	rightwards,
	leftwards,
	top,
	bottom
};

enum Shapes_3D
{
	nullshape,
	plane,
	cylinder,
	cone,
	sphere
};

enum Curves_2D
{
	nullcurve,
	line,
	arc
};

typedef list <Pt> PtsList;
typedef list <LineArc> LineArcList;
typedef list <double> dbllist;
typedef list <BYTE*> IMGLIST;
typedef list <intPt> intPtsList;

struct RBF_API Surface_3D
{
	double answer[10];
	Pts_3DList pts_3d_list;
	Shapes_3D shape;
	int shape_index;
};

struct RBF_API Curve_2D
{
	double answer[6];
	PtsList pts_list;
	Curves_2D curve;
	int curve_index;
};

typedef list <Surface_3D> Surface_3DList;
typedef list <Curve_2D> Curve_2DList;

	class RBF_API RBF
	{
		public:
			enum Conic
			{
				Ellipse,
				Parabola,
				Hyperbola
			} conictype;

			RBF(void);
			~RBF(void);

#pragma region Error Handling.
			void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
			void (*RBFError)(char* RBFerrorcode, char* RBFFileName, char* RBFFunName);
#pragma endregion

			double Tolerance;
			double dist_cutoff;

			//filterdir = -1 implies no filtration direction assumed.  0, 1, 2, 3 implies filtration done to get right extreme line, left extreme line, top extreme line or bottom extreme line.
			//iter = 1 by default.  else iter will iterate for that many times as specified for the outer line filtration.
			bool Line_BestFit_2D(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision = 1000000.00, bool remove_outliers = false, int filterdir = -1, int iter = 1);
			bool FG_Line_BestFit_2D(double* pts, int PtsCount, double* answer, bool SigmaMode, bool start_pt_constraint, bool end_pt_constraint, double* start_pt, double* end_pt);
			bool ParallelLine_BestFit_2D(double* ParentLine, double* pts, int PtsCount, double* answer);

			bool Circle_3Pt(double* P, double* Ans);

			//outer = 0 means normal best fit circle (default).
			//outer = 1 means outermost best fit circle.
			//outer = -1 means innermost best fit circle.
			//iter = 1 by default - else iter will iterate for that many times as specified for the outer/inner circle filtration
			//if outer != 0, then answer[4] must be the size of answer array passed.
			bool Circle_BestFit(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision = 1000000.00, bool remove_outliers = false, int outer = 0, int iter = 1);

			//initGuess and answer params will have 2 params for center, 1 param for min radius, 1 param for major dia angle.
			//if center_frozen is true, only 3rd and 4th params will be updated in answer, first 2 will be same as initGuess.
			bool CamProfile_BestFit(double* pts, int PtsCount, double* initGuess, double* answer, bool center_frozen = false);

			//get the distance at a given angle for a given Cam Profile
			bool CamProfile_GetDist(double* CamParam, double ang, double* dist);

			//find the intersection of local Cam Profile with the ray angle drawn from the global Cam Profile center
			//get distance of this intersection point from global Cam Profile center
			//works so long as CamParamLocal and CamParamGlobal are close (first order approximation done)
			bool CamProfile_GetDist(double* CamParamLocal, double* CamParamGlobal, double ang, double* dist);

			//outer = 0 means normal best fit circle with center (default).
			//outer = 1 means outermost best fit circle with center.
			//outer = -1 means innermost best fit circle with center.
			//iter = 1 by default - else iter will iterate for that many times as specified for the outer/inner circle filtration
			bool CircleWithCenter_BestFit(double* pts, int PtsCount, double* answer, bool remove_outliers = false, int outer = 0, int iter = 1);

			bool FG_Circle_BestFit(double* pts, int PtsCount, double* answer, bool start_pt_constraint, bool end_pt_constraint, double* start_pt, double* end_pt);
			
			bool Ellipse_BestFit(double* pts, int PtsCount, double* answer);

			//The pts will be in 3D
			//Cylinder params = x,y,z, l,m,n, rad ( in that order in the array)
			bool Cylinder_BestFit(double* pts, int PtsCount, double* answer, double precision = 1000000.00, bool remove_outliers = false, bool axial_bestfit = false);

			//The pts will be in 3D
			//Cone params = x,y,z (a pt on cone axis), l,m,n (direction cosines of cone axis), 
			//s (radius of cone at perpendicular plane passing through x,y,z) ang (apex angle of cone) in the same order in the array
			bool Cone_BestFit(double* pts, int PtsCount, double* answer, double precision = 1000000.00);

			//The pts will be in 3D
			//3D Circle params = x,y,z (center of circle), l,m,n (direction cosines of plane of circle), 
			//r (radius of circle) in the same order in the array
			bool ThreeDCircle_BestFit(double* pts, int PtsCount, double* answer);

			//answer will be an array with 14 parameters.  
			//The first 4 will be the best fit plane parameters and the remaining 10 will be from the ThreeDConic:
			//first 3 denoting coordinates of focus lying on best fit plane, 
			//next 3 the dir cosines of diretrix on the best fit plane,
			//7th, 8th and 9th will be a point on diretrix closest to focus, 
			//10th the eccentricity of conic.
			//TwoDConic would require only 6 parameters to define: first 2 denote 2D coordinates of focus on best fit plane,
			//third denotes 2D slope of diretrix on the best fit plane,
			//fourth and fifth the 2D point on diretrix closest to focus, sixth the eccentricity.
			bool ThreeDConic_BestFit(double* pts, int PtsCount, double* answer, Conic ConicShape);

			//answer will be an array with 7 parameters.  
			//first 3 denoting coordinates of focus, 
			//next 3 will be a point on diretrix plane closest to focus, 
			//last would be the eccentricity of conicoid.
			bool Conicoid_BestFit(double* pts, int PtsCount, double* answer, Conic ConicShape);

			//answer will be array of 8 parameters: first 3 will be the projection of center of the smaller arc/circle that rotates around an axis on the axis.
			//next 3 will be the dir cosines of the axis.  7th is the distance of center of arc from axis (the larger circle).  8th is the radius of the arc.
			bool Torus_BestFit(double* pts, int PtsCount, double* answer, double* IdealRadius, double* CentreCoods);

			//this is a two dimentional section of a torus.  the answer will be array of 5 parameters: first 2 will be the projection of center of the smaller arc/circle 
			//on the reflection axis.  Third will be the slope of the reflection axis.  4th is the distance of center of arc from axis (the larger circle).  
			//5th is the radius of the smaller arc/circle.
			//if axis_given is true, we need to pass answer[0], answer[1] and answer[2] to the function.
			bool TwoArc_BestFit(double* pts, int PtsCount, double* answer, bool axis_given = false);

			//the answer will be array of 6 parameters: first 3 will be a point on first line closest to origin and slope of first line 
			//next three will be a point on second line closest to origin and slope of second line.  
			//if axis_given is true, we need to pass a point on axis and slope of axis as first three parameters of answer.
			bool TwoLine_BestFit(double* pts, int PtsCount, double* answer, bool axis_given = false);

			//get best fit line-arc-line for set of points such that both lines are tangential to arc at its two ends.
			//the parameters would be ang1, intercept1, ang2, intercept2, x_center, y_center and radius for the line-arc-line group
			//if initguess flag is true, take initial guess values from array passed.
			//if linesgiven flag is true, answer[0], answer[1], answer[2] and answer[3] will be the passed with init guess values for center of arc on axis, slope of axis and radius of arc.
			bool LineArcLine_BestFit(double* pts, int PtsCount, double* answer, bool initguess = false, bool lines_given = false);

			//best fit pair of arcs that are tangential to each other.  when arc_given is true, first arc parameters (center, radius) are not modified and only second arc parameters
			//are modified to get the best fit.  the answer will consist of eight parameters, 3 each for the two arcs plus the intersection point of the two arcs.  initial guess values
			//are passed through answer.
			bool ArcArc_BestFit(double* pts, int PtsCount, double* answer, bool arc_given);

			//best fit arc-arc-arc combo that are tangential to each other.  the answer will consist of 13 parameters, 3 for each of the arcs and two intersection points.  
			//initial guess values are passed through answer.
			bool ArcArcArc_BestFit(double* pts, int PtsCount, double* answer);

			//best fit line-arc pair that are tangential to each other.  when line_given is true, line params are not modified and only arc parameters
			//are modified to get the best fit.  the answer will consist of four parameters, 2 for line and 2 for center of the arc (radius would be distance from center to line).  
			//initial guess values are passed through answer.
			bool LineArc_BestFit(double* pts, int PtsCount, double* answer, bool line_given);

			//involute best fit
			//answer will consist of center, radius and initial angle of involute
			bool CircleInvolute_BestFit(double* pts, int PtsCount, double* answer);

			bool Line_BestFit_3D(double* pts, int PtsCount, double* answer);
			bool Plane_3Pt_2(double* P, double* Ans);
			bool Plane_3Pt(double* P, double* Ans);
			//upperLower = 0 means no constraint on points.  upperLower = 1 means redo best fit with upper points alone. upperLower = 2 means redo best fit with lower points alone.
			//iter indicates the number of times we will perform this iteration of best fit plane
			bool Plane_BestFit(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision = 1000000.00, int upperLower = 0, int iter = 1);

			//need to pass in pts_out array with sufficient memory (equal to size of pts_in array to be safe)
			//pts_out will be the filtered points and PtsCount_out will contain the count of filtered points.  upperLower = 1 means upper filtration; = 2 means lower filtration.
			//return of false with *PtsCount_out = -1 indicates it failed to get even simple best fit plane (values in answer to be disregarded here).
			//return of false with non-negative value of *PtsCount_out means it failed to get suitable filtered plane but succeeded in getting normal best fit plane (values in answer).
			bool FilteredPlane_BestFit(double* pts_in, int PtsCount_in, double* pts_out, int* PtsCount_out, double* answer, bool SigmaMode, double precision = 1000000.00, int upperLower = 1);
			bool Intersection_Line_Line(double* line1, double* line2, double* intpt);
			bool Circle_3Pt_3D(double* P, double* Ans);
			bool Sphere_4Pt(double* P, double* Ans);
			bool Sphere_BestFit(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision = 1000000.00);
			
			bool CalculateFocus_old(double* pts, int TotalPts, double ActiveWidth, double* answer);
			bool CalculateFocus(double* pts, int TotalPts, double ActiveWidth, double* answer, double MaxMinFocusRatio_cutoff = 1.25, double MaxMinZRatio_cutoff = 5, double SigmaActiveWidthRatio_cutoff = 0.5, double FocusSpanRatio_cutoff = 0.5, double minslope = 0, double maxslope = 100, double z_jump = 0, double minFocusminCutoffRatio = 1);
			
			//last two params are for optional skew angle correction for X-Z and Y-Z respectively
			void CompensateErrors(double* P, double Angle, double* Ans, double AngleXZ = 0, double AngleYZ = 0);

			bool GetFocusMetric(double* Image, int ImageWidth, int* pR, double* pFocus, int N);
		
			//Pts holds list of points that need to be separated into lines and arcs.
			//SecondOrderslopes is a list of lines where lines with zero slope depict lines and non-zero slope depict arcs.
			int fnLinesAndArcs(PtsList* , LineArcList* , double, double max_radius = 50, double min_radius = 0.01, int step = -1, double max_dist_betn_neighboring_pts = 1000);
			int fnLinesAndArcsN(PtsList* , LineArcList* , double, int );
			bool fnLinesAndArcsThread(PtsList* ptr_Pts, LineArcList* ptr_SecondOrderslopes, double* p_tolerance, int* thread_sep_flag, double max_radius, double min_radius);
			//pass in 2-d points array and get length of this set of points by internally using "line-arc" separation
			//default value of step = -1 implies function will automatically calculate points to step through for calculating entity of smallest length
			//a value of step passed in implies the number of points to use for calculating entity of smallest length
			//sorted = true assumes that points are already sorted. sorted = false will call a routine to internally sort the points passed before calculating length.
			bool getLengthofShape(double* pts, int PtsCount, double* p_length, int step = -1, bool sorted = true);

			// function that will return two tangents to circles C1 and C2 respectively that are distance "dist" apart and on same direction w.r.t. two centers.
			// dist passed has to be positive if Tangent1 intersects C2 and negative if it does not intersect C2
			bool fnparallel_tangent_2_arcs (LineArc_Circle, LineArc_Circle, double, double, double, double, LineArc*, LineArc*);
			bool chkPtInSlopesLine(Pt chk_pt, LineArc chk_line, double curr_s, double tolerance);
			LineArc getLine(PtsList* ptr_Pts, double cutoff);

			bool getUpdatedPolygonLines(Pt new_pt, LineArcList* ptr_lines);
			bool getPolygonLines(PtsList*, LineArcList*);
			void updateLine(Pt start_pt, Pt end_pt, LineArc* ptr_Line);

			//actual_width = slip gauge width being used for calibration
			//num_Cell_X = number of cells along X-axis (= field of view x-length / slip gauge width)
			//num_Cell_Y = number of cells along Y-axis (= field of view y-length / slip gauge width)
			//dbl_array = array of doubles having the length along x, y and 45 deg of the slip gauge at each of the grid points (size of array = 3 * num_Cell_X * num_Cell_Y)
			//err_array = array of doubles having the error values along x and y at each of the grid points (size of array = 2 * (num_Cell_X + 1) * (num_Cell_Y + 1)
			//the sign of the error values populated in err_array is such that they need to be added to raw values to get corrected values.
			int fncalibration2(double actual_width, int num_Cell_X, int num_Cell_Y, double* dbl_array, double* err_array );

			//actual_width, num_Cell_Y, num_Cell_Y are having same meaning as in previous function
			//err_array is the array of doubles having error values along x and y passed in to the function.
			int setErrorCorrectionsArray2 (double actual_width, int num_Cell_X, int num_Cell_Y, double* err_array );

			//x, y position is passed in and corrected value is passed out in same pointer variables.
			//getRawFromCorrected is usually false.  If we want to retrieve the raw values based on values that were already corrected, set this flag to true.
			int correctXYCoord2 (double* x, double* y, bool getRawFromCorrected = false);

			//this function is used for focus scan calibration
			//x_num: number of cells along x axis (default 20)
			//y_num: number of cells along y axis (default 15)
			//R_num: number of R values for which calibration values are stored (default 1)
			//R_array: sorted array of R values in ascending order for which calibration has been done (ignored when R_num = 1)
			//cell_width: width/height of each box
			//z_err_matrix: calibration values at x_num * y_num * R_num positions
			//z_calib_array: used for additional proportional correction along x and y (not used - default value of 1,1,0,0)
			//x, y, R: actual value of x, y and R for which correction is required.  (R value ignored when R_num = 1)
			//*z_err: contains finally computed calibration correction.
			bool getZError (int x_num, int y_num, double cell_width, double* z_err_matrix, double* z_calib_array, double x, double y, double* z_err, int R_num = 1, double* R_array = NULL, double R = 0);

			//actual_width = slip gauge width being used for calibration
			//num_Cell_X = number of cells along X-axis (= field of view x-length / slip gauge width)
			//num_Cell_Y = number of cells along Y-axis (= field of view y-length / slip gauge width)
			//dbl_array = array of doubles having the length along x and y of the slip gauge at each of the grid points (size of array = 2 * num_Cell_X * num_Cell_Y)
			//err_array = array of doubles having the error values along x and y at each of the grid points (size of array = 2 * (num_Cell_X + 1) * (num_Cell_Y + 1)
			//the sign of the error values populated in err_array is such that they need to be added to raw values to get corrected values.
			bool fnOneShotCalibration(double actual_width, int num_Cell_X, int num_Cell_Y, double* dbl_array, double* err_array);		

			//caller needs to set ref image, obj image, redefineimagesaboutCG and getmaximagematch to get orientation and shift of obj wrt ref.
			//alternatively caller needs to set ref image, obj image and call getbestimagematch to get the orientation and shift of obj wrt ref.
			//first sequence is preferred if ref and obj are completely within field of view.
			//second sequence is preferred if ref and/or object is only partially in field of view and we are trying to get shift and orientation
			//by getting best image match through auto-correlation.
			//in second sequence, it may be preferable to also call blankpartimage on ref and obj before calling getbestimagematch with suitable parameters.

			// set reference image as array of 0 and 1 based on input pixel array being > or <= 127.  Also update area A_ref.
			// input pixelArray needs to be of size 2*M_ARRAY*2*N_ARRAY
			bool setOneShotImage(BYTE* pixelArray, int width, int height, bool singlechannel, int deadPixelWidth = 10, int binarizingCutoff = 200, int scale = 1, bool recalculateLRdeadPixWidth = false, bool ThisisObjectImage = true, bool IsProfileImage = true);

			// set object image as array of 0 and 1 based on input pixel array being being > or <= 127.  Also update area A_obj.
			// input pixelArray needs to be of size 2*M_ARRAY*2*N_ARRAY
			bool setObjImage (BYTE* pixelArray, int width, int height, bool singlechannel, int deadPixelWidth = 10, int binarizingCutoff = 200, int scale = 1, bool recalculateLRdeadPixWidth = false);

			// get pixel array as array of 255 and 0 based on reference image being 0 or 1. 
			// pixelArray retrieved needs to be of size 2*M_ARRAY*2*N_ARRAY
			bool getRefImage (BYTE* pixelArray, int width, int height, bool singlechannel);

			// get pixel array as array of 255 and 0 based on object image being 0 or 1.
			// pixelArray retrieved needs to be of size 2*M_ARRAY*2*N_ARRAY
			bool getObjImage (BYTE* pixelArray, int width, int height, bool singlechannel);

			// update reference and object image arrays with origin as center of gravity of the figures.
			// If area mismatch above tolerance limit return with error code of 1.  If "both" is set to true,
			// do for both arrays.  Else only for object image (assumption is that it is already done for reference)
			// if successful, return with success code of 0.  return the x and y coordinates of ref and obj.
			int RedefineImagesAboutCG(bool both, int* p_xy, bool MatchExactly, int tol_error = 5);
			int RedefineImagesAboutCentroid (bool both, int* p_xy, bool exactmatch);

			// get angle for which the image match is maximum.  For this angle, also set object image 
			// in terms of coordinate frame rotated by this angle and update A_ref_obj with the maximum
			// value of match obtained.  If this match area mismatch with reference area is above threshold,
			// return error code of 1, else success code of 0.
			// tol_error_ll should ideally be a figure less than tol_error passed to RedefineImagesAboutCG.
			// this gives the limit of tolerance error to which match is attempted.  But function will return success even if best match
			// falls only within tol_error and not within tol_error_ll.  If -1 is passed, tol_error_ll is internally set equal to tol_error.
			int getMaxImageMatch (double* p_Theta, bool MatchExactly, bool singlechannel, int tol_error_ll = -1);
			int getBestImageMatchAng (double* p_Theta, bool exactmatch, bool singlechannel, int tol_error_ll = -1);

			int getBestImageMatch (int* xy, double* p_Theta, bool singlechannel, bool highaccuracymode = true);
			bool blankPartImage (refobj img, direction dir, double percentage, bool singlechannel);

			bool getIrreducibleComponents (BYTE* pixelArray, int width, int height, bool singlechannel, IMGLIST* p_imageLst, intPtsList* p_edgeptslist, int deadPixelWidth = 10, int binarizingCutoff = 200);

			//Use CG to align current frame to Part program frame
			bool get_CGParams(BYTE* image, int w, int h, bool singleCh, double* cgParams, int deadPixW = 10, int binCutOff = 200, int sc = 1, bool ProfileImage = true);


			//ptslist is an array of 3-d points of size ptscount, ie an array of doubles of size 3*ptscount.  
			//trianglelist is a pointer to an array of 3-d triangles of size trianglescount, ie an array of doubles of size 9*trianglescount.
			bool TriangulateSurface(double* ptslist, int ptscount, double** trianglelist, int* trianglescount, bool triangleLengthUpperCutoff = true, int step = 1, int MaxDistToAvgDist = 2 );
			//returns true if the two lines intersect between their end points, else returns false
			bool line_intersect(Line_3D line1, Line_3D line2, double precision);
			bool line_intersect_any_other_line(Line_3D line1, Line_3DList line_list, double precision);

			//returns true if we get a best fit surface with least squares variation less than precision passed.  Surfaces tried out are the 4 fundamental
			//3d Shapes - plane, cylinder, cone, sphere.  Returns the one with least of the least squares variation
			bool BestFitSurface(double* pts, int PtsCount, double precision, Surface_3DList* p_surface_list, int step = 1, bool pts_around_xy_plane = true);
			bool pt_in_shape(Shapes_3D shape, double* Answer, Pt_3D pt, double deviation);

			//best fit curve along same lines (alternate to line-arc sep)
			bool BestFitCurve(double* pts, int PtsCount, double precision_line, double precision_arc, Curve_2DList* p_curve_list, int step = 1);
			bool pt_in_curve(Curves_2D curve, double* Answer, Pt pt, double deviation);
			bool orderpoints(PtsList* ptr_ptslist, double* p_avgDist = NULL, int ptIndex = -1);

			bool twoDpt_inThreeDTriangleLst(double* pt, double* trianglelist, int trianglescount, int* triangle_index, double* z_coord, double dir_n_min_cutoff, double maxdist_from_nbr_pt);
			bool chk_in_triangle(double* pt, double* triangle, int triangle_index, double* min_d, int* pt_index);
			void area_triangle(double* v1, double* v2, double* area);
			bool getZcoordInTriangleList(double* pts, int ptscount, double* trianglelist, int trianglescount, double dir_n_min_cutoff = 0.5, double maxdist_from_nbr_pt = 0.2);

			//functions for returning max, min parameters of 3 d shapes with points - will be needed for rendering them
			bool getSphereBoundaryAngles(double* pts, int ptscount, double* sphereparam, double* boundaryangles);
			bool getConeBoundaryAngles(double* pts, int ptscount, double* coneparam, double* boundaryangles);

			//dir would take values from 0 to 5.  Decreasing X is 0, Increasing X is 1, Decreasing Y is 2, Increasing Y is 3, Decreasing Z is 4, Increasing Z is 5
			bool getProbeCorrectedPt(double* pts_array, int ptscount, bool threeD, double probe_dia, int dir, double* pt_in, double* pt_out, double upp);

			//function to return shift and rotation of pts list wrt line-arc list for min deviation.  
			//alignment mode == 0 implies we want to rotate and translate points to get best fit
			//alignment mode == 1 implies we don't want to rotate points and only want to translate them to get best fit
			//alignment mode == 2 is when we don't want to translate points and only rotate them about their CG to find best fit
			bool BFalignment_2dpts_with_lines_arcs(double* pts_array, int ptscount, Curve_2DList curve_list, double* shift_x, double* shift_y, double* p_theta, double* rotn_origin, double resolution = 0.01, int sample_size = 50, int alignment_mode = 0);
			double get_squared_dist(double* pt, Curve_2D curve);
			double get_min_squared_dist(double* pt, Curve_2DList curve_list);
			double get_min_squared_dist(double* pt, double* pts_list, int ptscount);
			bool getRotnAngleForPtsMatch(double* pts, int ptscount, Curve_2DList curve_list, double* rot_abt, double* p_theta, double* variance, double full_span_angle, double resolution, int l_step, int ang_step);
			
			//function to align 3D points passed in as pts_ref and pts_obj.  shift_obj has x,y,z coordinates of shift vector by which pts_obj need to be shifted
			//p_theta is angle by which pts_obj need to be rotated around y-axis passing through CG of pts_obj.
			//p_phi is angle by which pts_obj need to be subsequently rotated around z-axis passing through CG of pts_obj.
			bool BFalignment_3D_pts(double* pts_ref, int pts_ref_cnt, double* pts_obj, int pts_obj_cnt, double* shift_obj, double* p_theta, double* p_phi, double resolution = 0.01, int sample_size = 50);

			//generate arc tangent to line 
			//pass one point and slope of line.  
			//pass incoming arc params - center, radius and the end point farther away from line which will remain unchanged.
			//arc output will be sent with redefined parameters for center and radius.  
			//The end points of this arc will be the end point passed in and the point where line and arc touch each other but these are not sent back to caller
			bool ArcTangentToLine(double* line, double* arc_in, double* arc_out);
			//generate arc tangent to arc
			//pass first arc parameters - center, radius and end point which is near second arc.
			//pass second arc parameters - center, radius and the end point which is farther from 1st arc.
			//send back recalculated second arc parameters - center and radius.  
			//One of end points of 2nd arc will be same as the end point of first arc passed and the other end point would be calculated at the ray of second end point but these are not sent back to caller.
			bool ArcTangentToArc(double* arc_in_first, double* arc_in_second, double* arc_out_second);

			//delphi cam module probe positions and goto points relative vectors in a 4-d double array
			//ray_angle is angle of ray from center to constructed point on slot of cam profile
			//theta is the angle from ray at which the probe position and go-to point are required
			//pts[0], pts[1] give relative position vector of probe position wrt center
			//pts[2], pts[3] give relative position vector of probe point wrt center
			bool RetrieveDelphiCamPts(double ray_angle, double theta, double min_radius, double* pts);

			//Best fit Sinusoid for runout corrections
			//First written for Hob Checker. WE see some kind of internal runout inside the between centres. 
			//For Hob Checker, the rotary C axis values are the "X" axis, and Y touch points are the "Y" axis. 
			//So pts array will be theta0, Y0, Theta1, Y1 ......
			bool Sinusoid_BestFit(double* pts, int PtsCount, double* answer);
	};