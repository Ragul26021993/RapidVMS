#ifdef EDGEDETECTIONCLASSSINGLECHANNEL_EXPORTS
#define EDGEDETECTIONCLASSSINGLECHANNEL __declspec(dllexport)
#else
#define EDGEDETECTIONCLASSSINGLECHANNEL __declspec(dllimport)
#endif

#pragma warning(disable: 4244) 
#include <list>
#include <map>
#include <WinDef.h>
#include <string>

//This is the edge detection class
//It has:
//	1. Method to detect an edge within a fixed rectangle
//	2. Method to detect an edge within an angular rect
//	3. Method to detect an edge within a curved box for arcs
//	4. Method to detect the aurolab lens hole
//	5. Method to calculate the area of white within a specified rectangular region
//  6. Method to get a single edge point in any one direction or no direction specified
//  7. Method to find an edge within a block of specified size (for mouse click edge detection)
//  8. Method for surface edge detection.



class EDGEDETECTIONCLASSSINGLECHANNEL EdgeDetectionSingleChannel
{
	
#pragma region "Global variables, Enumerations and Structures"
private:
	double *iBytes, *PixelVariance, VarianceCutOff, *iBytesCopy, *iBytes_RBChannel;		//A 1D array to store all the pixel values.
	int ImageWidth, ImageHeight;	//variables to store image width, height
	
	double Tolerance;
	bool PixelAveraged; //specifies if the pixel values have been averaged over multiple frames
	int PixelSkipValue; //This value indicates the number of pixels that has to be skipped when fast scanning
	int NoEdgePixelCount; //We count the number of rows/columns that did not give an edge pixel during Edge Travel. Only when this number goes greater than twice our scan range do we call Dust Skip
	int CameraType; //We can choose Edge Travel algorithm according to camera type

public: 
	double DetectedPointsList[20000];
	int BoundryBuffer;
//Surface Edge Detection Properties
public:
	enum NoiseFilter_BestFitType
	{
		LINE_BESTFIT,
		ARC_BESTFIT,
		LINEARC_BESTFIT,
	};

	//Type1 (CoaxialEdge)
	int Type1_LookAheadVal;
	double Type1_BlackTol, Type1_FilterMaskTol;
	double Type1_WhiteTol, Type1_FilterMaskTolB2W;
	//Type2 (SingleWhitePixelEdge)
	int Type2_LookAheadVal, Type2_CurrAbsThreshold, Type2_CurrDiffThreshold, Type2_LookaheadAbsThreshold, Type2_LookaheadDiffThreshold, Type2_MedianFilterBxSize, Type2_AvgFilterBxSize;
	//Type3 (ManualColouredEdge)
	int Type3_LookAheadVal, Type3_BlackTol, Type3_FilterMaskTol;
	int Type3_WhiteTol, Type3_FilterMaskTolB2W;
	//Type4 (FocusBasedEdge)
	int Type4_LookAheadVal, Type4_TextureDiffThreshold, Type4_AcrossEdgeIncr, Type4_AlongEdgeIncr, Type4_AcrossEdgeBxThickness, Type4_AlongEdgeBxThickness;
	//type5 (PlasticComponentEdge)
	int Type5_LocalMaximaExtent, Type5_DiffThreshold, Type5_FilterMaskSize;

	int Type6_MaskFactor, Type6_Binary_LowerFactor, Type6_Binary_UpperFactor, Type6_PixelThreshold, Type6_MaskingLevel, Type6_ScanRange, Type6_NoiseFilterDistInPixel, Type6SubAlgo_NoiseFilterDistInPixel, Type6JumpThreshold;
	bool ApplySubAlgorithmOfType6, ApplyDirectionalScan, DoAverageFiltering;
	int Pixel_Neighbour, MinimumPointsInEdge;
	double *bd_factor; //a double array of parameter values. A pointer is done so that we dont need to change header again! Allocate in constructor
	bool CorrectforBeamDivergence; // If true, we will correct pixel edge position based on position in image
	//If True We will look for dark-light-dark or light-dark-light pattern before detecting an edge. 
	//KroneckerCentre to true will give the centre and not the edge of the kronecker as the edge
	bool KroneckerEdge, CentreofKronecker;
	int KroneckerThickness;
	NoiseFilter_BestFitType NoiseFilterType;
	//BYTE BinImage[32000000];
	int EdgeFinderPixelRange, LookAheadLimit;
	//Enumeration to specify the type of Box that the user has specified where the edge has to be detected
	double CamSizeRatio;
	double RefDotDiameter;

private:
	enum TypeOfBox
	{
		StraightRect,
		AngularRect,
		CurvedBox
	};

public:
	//Enumeration to specify the direction of scan
	enum ScanDirection
	{
		Rightwards,
		Downwards,
		Leftwards,
		Upwards
	};

	enum ComponentType
	{
		CoaxialEdge,
		SingleWhitePixelEdge,
		ManualColouredEdge,
		FocusBasedEdge,
		PlasticComponentEdge,
		PeakBased,
	};


private:
	//This is a Pt structure to contain the coordinates of a point
	struct Pt
	{
		double X, Y;
		int pt_index;
		int direction;
		double ScanAnglePosition;
		bool operator ==(const Pt &pt) const
		{
			if (pt.pt_index == pt_index)
				return true;
			else 
				return false;
		}
	};

	//struct ArcScanParams
	//{
	//	int ScanDir;
	//	double stAngle;
	//	double EndAngle;
	//};
#pragma endregion

public:

#pragma region "Constructor, Set Image, Set Variance and Binarize Image functions"
	
	//Constructor to set width and height of the image
	EdgeDetectionSingleChannel(int bmWidth, int bmHeight, int ProfileEdgeThickness, int EdgeScanSize);
	
	//To reset the image size
	void ResetImageSize(int bmWidth, int bmHeight);

	//This function must be called immidiately after the instantiation of the class
	//It is used to set the image
	void SetImage(double *PixelArray, bool PixelValuesAveraged, int CurrentCameraNo);
	void SetImage(BYTE *PixelArray, bool PixelValuesAveraged, int CurrentCameraNo);
	void SetImageforDifferentChannel(double *PixelArray);

	//This function is used to set the variance of each averaged pixels on multiple frames
	void SetVariance(double *VarianceArray, double VarianceCutOffVal);

	//This function binarizes the image according to the threshold value passed
	void BinarizeImage(int Threshold);

	//To get the average pixel value of the whole Image
	double GetAvgOfImage();
	//Calculate average for current FG rectangle
	double CalculateAvg(int *Box);
	double CalculateAvgForRectangle(int* Box);
	double *GetImage();
	void SetBeamDivergenceParams(bool EnableBeamCorrection, double *factors);
	void ConvertTo3bpp(BYTE* PixelArray);

#pragma endregion

#pragma region Error Handling.
	// Added by Sathya..
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	void (*EdgeDetectionError)(char* Edscerrorcode, char* EdscFileName, char* EdscFunName);
#pragma endregion

#pragma region "Detect Edge functions for the three type of boundry conditions"
	
	//This function scans in a specified direction for an edge within a straight rectangular area.
	int DetectEdgeStraightRect(int *FrameGrabBox, ScanDirection Scan_Direction);
	
	//This function scans in a specified direction(default being rightwards/downwards for horizontal/vertical scan) for an edge within an angular rect.
	int DetectEdgeAngularRect(int *FrameGrabBox, bool OppositeScan);
	
	//This function scans in a specified direction(default being outwards) for an edge within a curved box.
	int DetectEdgeCurvedBox(double *FrameGrabBox, bool OppositeScan);

#pragma endregion

#pragma region "Function to Get All Edge Points"

	//This function copies the specified number of edge points from the class list to the pointer received as a parameter
	bool GetAllEdgePoints(double *PointsListInDoubleFormat, int NumberOfPointsToCopy);
	void ImageAnalysis(int *Box, int masksize, int filterCnt, int PixelThreshold, std::list<double> *ptList);
#pragma endregion

#pragma region "Public function to Get a single edge point in a specified direction or no direction(straight rect only)"
	
	//This function gets a single edge point in a specified direction of scan
	bool GetEdgePoint(int *FrameGrabBox, ScanDirection Scan_Direction, bool SingleLineScan, double *DetectedPoint);

	//This function gets a single edge point within a rect area.
	bool GetEdgePoint(int *FrameGrabBox, bool SingleLineScan, double *DetectedPoint);

#pragma endregion

#pragma region "White Area Calculation"
	
	//This function calculates the value of white area within a rectangular area
	double CalculateWhiteAreaStraightRect(int *FrameGrabBox, double *TotalArea);

	//This function calculates the value of white area within a angular rect area
	double CalculateWhiteAreaAngularRect(int *FrameGrabBox, double *TotalArea);

	//This function calculates the value of white area within a curved box area
	double CalculateWhiteAreaCurvedBox(double *FrameGrabBox, double *TotalArea);

	bool CheckForBlankImage(int BinDegree);

#pragma endregion	
	
#pragma region "Function to find Auro Lab lens hole"
	
	//This function detects the hole on the AuroLab lens
	int FindHole(ScanDirection Scan_Direction);

#pragma endregion

#pragma region "Function to find an edge within a block of specified size"
	
	//This function scans within a block of 20 * 20 pixels for an edge and traverses along it for the whole image
	int DetectEdgeMouseClick(int X, int Y);
#pragma endregion

#pragma region "Function for surface edge detection"
	
	//This function scans for an edge point within a straight rect in the specified direction
	int DetectSurfaceEdge(int *FrameGrabBox, ScanDirection Scan_Direction, ComponentType Component_Type = PlasticComponentEdge, bool dirGiven = false);
	
	int DetectSurfaceEdge(int *FrameGrabBox, ScanDirection Scan_Direction, ComponentType Component_Type, int startR, int startC);

	int DetectSurfaceEdge_Peak(int *FrameGrabBox, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int PixelThreshold, int MaskingLevel, bool RedoImagePreProcess);

	int DetectSurfaceEdge_Peak(int *FrameGrabBox, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int PixelThreshold, int MaskingLevel, ScanDirection lastDir, int startR, int startC);

	//This function scans in a specified direction(default being rightwards/downwards for horizontal/vertical scan) for a surface edge within an angular rect.
	int DetectSurfaceEdgeAngularRect(int *FrameGrabBox, bool OppositeScan, bool RedoImagePreProcess, ComponentType Component_Type = PlasticComponentEdge);
	
	//This function scans in a specified direction(default being outwards) for a surface edge within a curved box.
	int DetectSurfaceEdgeCurvedBox(double *FrameGrabBox, bool OppositeScan, bool RedoImagePreProcess, ComponentType Component_Type = PlasticComponentEdge);

#pragma endregion

#pragma region "Function for reference dot finding"
	
	//This function scans for the reference dot
	int Analyze_RotatingDot(bool BinariseImage);

#pragma endregion

#pragma region Function to perform profile edge detection on lower resolution edges

	//Public function to detect profile edge when the resolution of the edge is low (for rectangular box)
	int DetectEdgeLowFocus(int *FrameGrabBox, int Scan_Direction);

	//Public function to detect profile edge when the resolution of the edge is low (for angular rect box)
	int DetectEdgeLowFocusAngularRect(int *FrameGrabBox, bool OppositeScan);

	//Public function to detect profile edge when the resolution of the edge is low (for arc rect box)
	int DetectEdgeLowFocusCurvedBox(double *FrameGrabBox, bool OppositeScan);

#pragma endregion

#pragma region Function to detect all profile edges within the specified rectangle

	//Public function to detect profile edges
	int DetectAllProfileEdges(int *FrameGrabBox, int jump = 2);

#pragma endregion

#pragma region Function to detect ID or OD of an o-ring

	//Public function to detect ID/OD
	int DetectIDOD(int *FrameGrabBox, bool ID);

#pragma endregion

private:
	
#pragma region "Private functions used by detect edge functions that get a single edge point for the 3 kinds of boundry conditions"
	
	//This function gets a single edge point within a straight rect in a specified direction of scan
	bool GetSingleEdgePointStraightRect(int *Box, ScanDirection Scan_Direction, struct Pt *DetectedPoint, bool SingleLineScan = false);
	
	//This function gets a single edge point within an angular rect in a specified direction of scan(default being rightwards/downwards)
	bool GetSingleEdgePointAngularRect(int *Box, bool OppositeScan, struct Pt *DetectedPoint, ScanDirection *Scan_Direction);
	bool GetSingleEdgePointAngularRect2(int *Box, bool OppositeScan, struct Pt *DetectedPoint, ScanDirection *Scan_Direction);

	void GetEdgePointAngularPosition(struct Pt *Point, struct Pt ReferencePt);

	bool GetEdgePtNearGivenPtAngRect_H(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, double SinTH, double CosTH, Pt OriginPt, Pt *DetectedPoint, int Width, int Height, int yScanDir);
	bool GetEdgePtNearGivenPtAngRect_V(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, double SinTH, double CosTH, Pt OriginPt, Pt *DetectedPoint, int Width, int Height, int xScanDir);

	bool GetEdgePtNearGivenPtStraightRect_H(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, Pt *DetectedPoint, int* Box, int yScanDir);
	bool GetEdgePtNearGivenPtStraightRect_V(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, Pt *DetectedPoint, int *Box, int xScanDir);

	int GetEdgePtNearGivenPtCurvedBox(double Rad, double Theta, int windowWidth, int windowHeight, Pt *DetectedPoint, double* Box, int ScanDir, bool OppScan);
	//bool GetEdgePtNearGivenPtCurvedBox_V(double Theta, int windowTheta, int RadialWidth, Pt *DetectedPoint, double* Box, int xScanDir);

	bool AddPointToList(int* AxisPosition, struct Pt thisPt, std::list<struct Pt> *PointsList, bool ScanPositiveDir, struct Pt TravelRefPt);
	//This function gets a single edge point within a curved box in a specified direction of scan(default being outwards)
	bool GetSingleEdgePointCurvedBox_Modified(double *Box, bool OppositeScan, struct Pt *DetectedPoint, ScanDirection *Scan_Direction);
	bool GetSingleEdgePointCurvedBox2(double *Box, int Width, int Height, bool OppositeScan, struct Pt *DetectedPoint, int ScanDIr);
	bool GetSingleEdgePointCurvedBox(double *Box, bool OppositeScan, struct Pt *DetectedPoint);



	//This function checks if a point is within the specified angular rect or not
	//It is used by the GetSingleEdgePointAngularRect and TraverseEdgeAngularRect functions
	int CheckPointWithinAngularRect(int X, int Y, double SinTh, double CosTh, Pt OriginPt, int Width, int Height, bool ApplyBoundryBuffer = true);
	int CheckPointWithinAngularRect(int X, int Y, double SinTh, double CosTh, Pt OriginPt, int Width, int Height, struct Pt* transformedPt);
	int CheckPointWithinStraightRect(int X, int Y, int* Box);
	
	//This function checks if a point is within the specified curved box or not
	//It is used by the GetSingleEdgePointCurvedBox and TraverseEdgeCurvedBox functions
	bool CheckPointWithinCurvedBox(int X, int Y, double Theta1, double Theta2, double R1, double R2, Pt OriginPoint, bool ApplyBoundryBuffer = true);
	int CheckPointWithinCurvedBox2(int X, int Y, double Theta1, double Theta2, double R1, double R2, Pt OriginPoint, Pt* CurrScanPos);
	void IncrementCurvedBoxPosition(int *X, int *Y, Pt OriPt, Pt* cScanPos, int IncreaseBy = 1);

	//Returns true if we have to horizontal Scan else returns false
	int GetScanDirectionforCurvedBox(int X, int Y, Pt Centre, bool OppositeScan);

#pragma endregion	
	
#pragma region "Traverse edge functions for all 3 boundry conditions"
	
	//This function acts as a gateway for the next 3 functions for traversing the edge within a particular boundry conditions. 
	//It is called by the detect edge functions. 
	bool TraverseEdge(TypeOfBox BoxType, void *Box, std::list<struct Pt> *PointsList);
	
	//This traverse edge function is for the straight rect boundry condition in particular
	bool TraverseEdgeStraightRect(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir);
	bool TraverseEdgeStraightRect2(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir);
	bool TraverseEdgeStraightRect3(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir, bool ScanOpposite = false);
	
	//This traverse edge function is for the angular rect boundry condition in particular
	bool TraverseEdgeAngularRect(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir);
	bool TraverseEdgeAngularRect2(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir, struct Pt travRefPt);
	int TraverseEdgeAngularRect3(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir, struct Pt travRefPt);
	bool IncrementPositionAngularRect(int* X, int* Y, double SinTh, double CosTh, Pt OriginPt, struct Pt CurrentLocation, int ShiftBy);

	//This traverse edge function is for the curved box boundry condition in particular
	//bool TraverseEdgeCurvedBox(double *Box, std::list<struct Pt> *PointsList, ScanDirection SDir);
	bool TraverseEdgeCurvedBox(double *Box, std::list<struct Pt> *PointsList);
	bool TraverseEdgeCurvedBox1(double *Box, std::list<struct Pt> *PointsList, bool OppScan);
	bool TraverseEdgeCurvedBox2(double *Box, std::list<struct Pt> *PointsList, bool OppScan);

	//This function Skips the dust/irregularities for the traverse edge functions
	bool DustSkip(Pt Point1, Pt Point2, Pt *Pnt1, Pt *Pnt2);
	
	//This function scans horizontally, detects the edge, then decides whether to put this point into the list or not
	bool ScanHorizontalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool UpperRow, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd, int ScanDir);
	int ScanHorizontalAndInsertToList(int *X, int Y, Pt *EdgePoint); //, bool *EdgeLoopBack, Pt PtAtOtherEnd);
	
	//This function scans vertically, detects the edge, then decides whether to put this point into the list or not
	bool ScanVerticalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool LeftColumn, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd, int ScanDir);
	int ScanVerticalAndInsertToList(int X, int *Y, Pt *EdgePoint); // , bool *EdgeLoopBack, Pt PtAtOtherEnd);

	//This function scans horizontally, detects the edge, then decides whether to put this point into the list Old Algorithm
	bool ScanHorizontalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool UpperRow, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd);

	//This function scans vertically, detects the edge, then decides whether to put this point into the list or not Old Algorithm
	bool ScanVerticalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool LeftColumn, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd);


#pragma endregion	
	
#pragma region "The pin point edge, check hole and get single edge point in block functions"
	
	//This function is used by many functions to accurately pin point the edge within the confines of 4 pixels.
	double PinPointEdge(int x, int y, double *pixels);
	double PinPointEdge1(double *pixels);
	double PinPointEdge2(double *pixels);

	//This function is used to check if the edge point is properly on the edge or is noise
	bool CheckEdgePointQuality(Pt* EdgePt);

	//This function does beam divergence correction and adds the points to the exposed double array
	int AddEdgePtsToCollection(std::list<struct Pt> PtsList);

	//Correct pixel position from PinPointEdge for beam divergence
	double CorrectEdgePoint(int x, int y, double *pixels);
	
	//This function is used by the FindHole function to check if the detected edge is infact the hole that we are looking for
	bool CheckHole(std::list<Pt> *PointsList, int *NeglectHeight, ScanDirection Scan_Direction);

	//This function checks and gets one edge point within a block of specified size
	bool GetSingleEdgePointInBlock(int X, int Y, Pt *DetectedPoint);

	//Get the polar coordinates of a given edge point wrt to the centre of the beam
	//Ans points to 4-sized int array. The first two will be the point proximate to centre, and the last two will be the farther points from the beam
	void GetRadialPts(double X, double Y, int gap, int* Ans);
	
	//Convert XY cood to polar (R, theta)
	void GetPolarCoordinates(double X, double Y, double* Ans);


#pragma endregion

#pragma region "Helper function for surface edge detection"
	//Ankit Fastner and Rishab Carbide components
	void DetectSurfaceEdgeHelper(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, int LookAhead, int BlackTolerence, int FilteringMaskTolerence, int WhiteTolerence , int FilteringMaskTolerence2 );
	
	//Ankit Fastner and Rishab Carbide components
	void DetectSurfaceEdgeBasic(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, int LookAhead, int DiffCutoff);

	void DetectSurfaceEdgeAngularRectHelper(int *Box, bool OppositeScan, std::list<struct Pt> *PointsList);
	//General Surface edge detection for Angular rect
	void DetectSurfaceEdgeAngularRectHelper(int *Box, bool OppositeScan, std::list<struct Pt> *PointsList, int LookAhead, int BlackTolerence, int FilteringMaskTolerence);

	//Function to get the points in original coordinate system from the transformed coordinate system
	void GetOriginalCoordinates(double XDash, double YDash, double XOrigin, double YOrigin, double CosTh, double SinTh, double *X, double *Y);

	//General Surface edge detection for curved box
	void DetectSurfaceEdgeCurvedBoxHelper(double *Box, bool OppositeScan, std::list<struct Pt> *PointsList, int LookAhead, int BlackTolerence, int FilteringMaskTolerence);
	
	//Function to get the points in original coordinate system from the radial coordinate system
	void GetOriginalCoordinates(double XOrigin, double YOrigin, double Radius, double Theta, double *X, double *Y);
	
	//pixel masking to filter out unwanted edge points
	bool FilteringMask(int XPt, int YPt, int WhiteThreshold);
	bool FilteringMask(int XPt, int YPt, int WhiteThreshold,bool BlackToWhiteMovement);
	

	void GetMedian(int *Box , ScanDirection Scan_Direction , int Range ,std::list<int>* MedianPixels);

	void DetectSurfaceEdgeHelper1(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList);
	
	//Brakes India component
	void DetectSurfaceEdgeHelper2(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList);

	//To divide the image into grids of specified size and getting the 'FocusMetric' values for each block into an array
	void GetTextureArray(int *Box, int BlockWidth, int BlockHeight, int HIncrementer, int VIncrementer, int *NoOfRows, int *NoOfCols, double *TextureArray);

	//Sathya's function added on 31/3/11. Good for plastic components
	void DetectSurfaceEdgeHelper3(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, double diffThreshold = 3.0, int MaskSize = 7);

	//For Ankit's new component edge(Titanium Component-using median filtering)
	void DetectSurfaceEdgeHelper4(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList);

	//Function to find the median filter value with a n pixel mask
	double GetMedianFilterValue(int X, int Y, int MaskSize, bool GetAvg = false);

	//Function to filter out points from list using line best fit method
	void FilterOutPtsFromList(std::list<struct Pt> *PointsList);

	//Function to get best fit line for last 10 points and checking if the new point is close enough to this line
	bool BestFitFiltering(std::list<struct Pt> *PointsList, Pt NewPt);

	//Function to calculate the Point to line distance//
	bool Pt2Line_Dist(double *point, double slope, double intercept, double *distance);

	void DetectSurfaceEdgeNormalising(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList);
#pragma endregion

#pragma region Helper functions for lower resolution profile edge detection

	//Function to perform the scan in a rectangular area for th edge
	void DetectProfileEdgeLowerResolution(int *Box, ScanDirection Scan_Direction, int Resolution, std::list<struct Pt> *PointsList);

	//Function to perform the scan in a rectangular area for th edge
	void DetectProfileEdgeLowerResolutionAngularRect(int *Box,  bool OppositeScan, int Resolution, std::list<struct Pt> *PointsList);
	void DetectProfileEdgeLowerResolutionAngularRect0(int *Box,  bool OppositeScan, int Resolution, std::list<struct Pt> *PointsList);

	//Function to perform the scan in a rectangular area for th edge
	void DetectProfileEdgeLowerResolutionArcRect(double *Box,  bool OppositeScan, int Resolution, std::list<struct Pt> *PointsList);

	//Function to pin point the edge within a block, for lower resolution of focus
	bool PinPointEdgeWithinBlock(double *ArrayBlock, int Size, double *Answer);
	bool PinPointEdgeWithinBlock0(double *ArrayBlock, int Size, double *Answer);


#pragma endregion

#pragma region "Helper Functions for reference dot finding"
	
	//This function sets the image in black and white format according to the threshold value passed
	void RefDotThreshold(double *SourceImage, int threshvalue);

	//Function to detect the reference dot edge
	int RefDotEdges(double *SourceImage);

#pragma endregion

#pragma region "Helper Functions for detecting all the profile edges within the box"

	//Helper function to detect all profile edges within a given box
	void DetectAllProfileEdgesHelper(int *Box, std::list<struct Pt> *PointsList);

	//Function to order the points detected
	void ArrangeAllPoints(int *Box, std::list<struct Pt> AllPointsList, std::list<struct Pt> *ArrangedPointsList);
	void ArrangeAllPoints2(std::list<struct Pt>* pAllPointsList, int ptIndex = -1, bool constructlist = false);
#pragma endregion

#pragma region "Helper Function to detect ID or OD of an o-ring"

	//Helper function that detects ID/OD
	void DetectIDODHelper(int *Box, int Resolution, std::list<struct Pt> *PointsList, bool ID);

	//This special case traverse edge function
	bool TraverseEdgeStraightRectForIDOD(int *Box, std::list<struct Pt> *PointsList);

	//This function Skips the dust/irregularities for the special case traverse edge functions for ID/OD detection
	bool DustSkipIDOD(Pt Point1, Pt Point2, Pt *Pnt1, Pt *Pnt2, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd);

#pragma endregion

#pragma region "Function related to algorithm6 for surface edge detection"

	// function to increase the quality of image..
	void Masking_Binarization(int *Box, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int MaskingLevel);

	void Masking_BinarizationCurve(double *Box, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int MaskingLevel);

	void Masking_BinarizationAngRect(int*Box, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int MaskingLevel);
	
	void DetectSurfaceEdgeHelperPeakPoint(int LeftBoundry, int RightBoundry, int TopBoundry, int BottomBoundry, int PixelThreshold, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList);

	void DetectSurfaceEdgeHelperPeakPoint(int LeftBoundry, int RightBoundry, int TopBoundry, int BottomBoundry, int PixelThreshold, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, int startrow, int startcol);

   void DetectSurfaceEdgeWithoutDirection(int *Box, int PixelThreshold, std::list<struct Pt> *PointsList, ScanDirection Scan_Dir = Rightwards, bool dirGiven = false);

	void FilterOutPtsFromList(std::list<struct Pt> *PointsList, int DistTolerance);

	double Pt_Pt_Distance(double* Pt1, double* Pt2);

	void DetectSurfaceEdgeCurvedBoxHelper(double *Box, bool OppositeScan, std::list<struct Pt> *PointsList, int PixelThreshold);

	double MedianSort(double *point, int size);

	double MultiplyMatrix(double *M1, int S1, double *M2);

	void QuickSort(double* arr, int left, int right);

#pragma endregion

};



