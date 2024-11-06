// EdgeDetectionClassSingleChannel.cpp : Defines the exported functions for the DLL application.
//

#include "..\stdafx.h"
#define _USE_MATH_DEFINES
#include <Windows.h>
#include<math.h>
#include<list>
#include <map>
#include<stdio.h>
#include "EdgeDetectionClassSingleChannel.h"
#include "RBF.h"
using namespace std;
#include <iostream>
#include <fstream>
#include <string>	
#include "..\ArcScanParams.h"

RBF *BFit = new RBF();
//int DoubleEdgeCt;

#pragma region "Constructor, Set Image and Set Variance"
	
	//Constructor to set the width and height of the image
	EdgeDetectionSingleChannel::EdgeDetectionSingleChannel(int bmWidth, int bmHeight, int ProfileEdgeThickness, int EdgeScanSize)
	{
		//SetAndRaiseErrorMessage("EDCSC0001", "EdgeDetectionSingleChannel", __FUNCSIG__);
		ImageWidth = bmWidth;
		ImageHeight = bmHeight;
		//BoundryBuffer = 6;
		Tolerance = 100;
		PixelSkipValue = 3;
		EdgeDetectionError = NULL;

		Type1_BlackTol = 50;
		Type1_FilterMaskTol= 50;
		
		Type1_WhiteTol = 60;
		Type1_FilterMaskTolB2W = 50;

		Type3_WhiteTol = 30;
		Type3_FilterMaskTolB2W = 30;

		MinimumPointsInEdge = 100;
		Type6_ScanRange = 20;
		ApplySubAlgorithmOfType6 = true;
		Pixel_Neighbour = 0;
		Type6_NoiseFilterDistInPixel = 15;
		Type6JumpThreshold = 20;
		ApplyDirectionalScan = false;
		Type6SubAlgo_NoiseFilterDistInPixel = 3;
		DoAverageFiltering = true;

		KroneckerEdge = false; CentreofKronecker = false;
		KroneckerThickness = 10;

		EdgeFinderPixelRange = ProfileEdgeThickness; 
		LookAheadLimit = EdgeScanSize;
		PixelSkipValue = EdgeFinderPixelRange;

		//BoundryBuffer = EdgeFinderPixelRange * 2;
		BoundryBuffer = max(EdgeFinderPixelRange, LookAheadLimit) + 1;

		//We start with a two-factor linear correction for beam divergence
		bd_factor = (double*)malloc(sizeof(double) * 6);
		ZeroMemory(bd_factor, 6 * sizeof(double));
		CamSizeRatio = 1.0;
		//ifstream bdfile;
		//bdfile.open("bd.txt");
		//char xx[20];
		//if (!bdfile)
		//{	bdfile.getline(xx, 4);
		//	//bdfile >> xx;
		//	}

		////bdfile>>bd_factor[0];
		////bdfile>>bd_factor[1];
		////bdfile>>bd_factor[2];
		////bdfile>>bd_factor[3];
		//bdfile.close();
	}

	//To reset the image size
	void EdgeDetectionSingleChannel::ResetImageSize(int bmWidth, int bmHeight)
	{
		ImageWidth = bmWidth;
		ImageHeight = bmHeight;
	}

	/*This function must be called immidiately after the instantiation of the class
	It is used to set the image*/
	void EdgeDetectionSingleChannel::SetImage(double *PixelArray, bool PixelValuesAveraged, int CurrentCameraNo)
	{
		try
		{
			iBytes = PixelArray;
			this->CameraType = CurrentCameraNo;
			if (CurrentCameraNo == 3)
			//	iBytes = PixelArray;
			//else
			{
				
				//int stIndex = 0, ct = 0;
				//iBytes = (double *)malloc(ImageWidth * ImageHeight * sizeof(double) / 4);
				//for (int j = 0; j < ImageHeight; j += 2)
				//{
				//	stIndex = j * ImageWidth;
				//	for (int i = 0; i < ImageWidth; i += 2)
				//	{
				//		iBytes[ct++] = PixelArray[stIndex + i];
				//	}
				//}
				//EdgeFinderPixelRange = 5; LookAheadLimit = 15;
				//BoundryBuffer = max(EdgeFinderPixelRange, LookAheadLimit) + 1;
				//ImageWidth = ImageWidth / 2; 
				//ImageHeight = ImageHeight / 2; // >> 1;
			}
			PixelAveraged = PixelValuesAveraged;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0001", __FILE__, __FUNCSIG__); }
	}

	void EdgeDetectionSingleChannel::SetImage(BYTE *PixelArray, bool PixelValuesAveraged, int CurrentCameraNo)
	{
		this->CameraType = CurrentCameraNo; PixelAveraged = PixelValuesAveraged;
		int totalsize = ImageWidth * ImageHeight;
		for (int i = 0; i < totalsize; i++)
			iBytes[i] = (double)PixelArray[i];
	}

	void EdgeDetectionSingleChannel::SetImageforDifferentChannel(double *PixelArray)
	{
		try
		{
			iBytes_RBChannel = PixelArray;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0001a", __FILE__, __FUNCSIG__); }
	}

	//This function is used to set the variance of each averaged pixels on multiple frames
	void EdgeDetectionSingleChannel::SetVariance(double *VarianceArray, double VarianceCutOffVal)
	{
		try
		{
			PixelVariance = VarianceArray;
			VarianceCutOff = VarianceCutOffVal;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0002", __FILE__, __FUNCSIG__); }
	}

	//This function binarizes the image according to the threshold value passed
	void EdgeDetectionSingleChannel::BinarizeImage(int Threshold)
	{
		try
		{
			iBytesCopy = iBytes;
			iBytes = (double *)malloc(ImageWidth * ImageHeight * sizeof(double));
			for(int i = 0; i < ImageHeight; i++)
				for(int j = 0; j < ImageWidth; j++)
					if(iBytesCopy[i * ImageWidth + j] <= Threshold) iBytes[i * ImageWidth + j] = 0;
					else iBytes[i * ImageWidth + j] = 255;

			int MaskThickness = 2, whitecnt;
			int FilterMaskVal = (int)((MaskThickness * 2 + 1) * (MaskThickness * 2 + 1) * 0.6);
			for(int i = MaskThickness; i < ImageHeight - MaskThickness; i++)
				for(int j = MaskThickness; j < ImageWidth - MaskThickness; j++)
				{
					whitecnt = 0;
					if(iBytes[i * ImageWidth + j] == 0)
					{
						for(int itr1 = -MaskThickness; itr1 <= MaskThickness; itr1++)
							for(int itr2 = -MaskThickness; itr2 <= MaskThickness; itr2++)
								if(iBytes[(i + itr1) * ImageWidth + j + itr2] == 255)
									whitecnt++;
						if(whitecnt > FilterMaskVal) iBytes[i * ImageWidth + j] = 255;
					}
				}

			free(iBytes);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0003", __FILE__, __FUNCSIG__); }
	}

	//To get the average pixel value of the whole Image
	double EdgeDetectionSingleChannel::GetAvgOfImage()
	{
		try
		{
			double avg = 0;
			int imageSize = ImageWidth * ImageHeight;
			for (int i = 0; i < imageSize; i++) avg += iBytes[i];
			avg /= imageSize;
			return avg;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0003", __FILE__, __FUNCSIG__); return 0;}
	}

	double *EdgeDetectionSingleChannel::GetImage()
	{
		SetAndRaiseErrorMessage("EDCSC0009", "GetImage", __FUNCSIG__);
		return iBytes;
	}

	void EdgeDetectionSingleChannel::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
	{
		if(EdgeDetectionError != NULL)
			EdgeDetectionError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
	}

	void EdgeDetectionSingleChannel::SetBeamDivergenceParams(bool EnableBeamCorrection, double *factors)
	{
		CorrectforBeamDivergence = EnableBeamCorrection;
		if (!EnableBeamCorrection) return;
		memcpy(bd_factor, factors, 6 * sizeof(double));
	}

#pragma endregion

//#pragma region "Edge Detection in any of the 4 directions"

	//This function copies the specified number of edge points from the class list to the pointer received as a parameter
	bool EdgeDetectionSingleChannel::GetAllEdgePoints(double *PointsListInDoubleFormat, int NumberOfPointsToCopy)
	{
		try
		{	
			//if number of pts asked for is greater than the numbr detected, then return false
			if(NumberOfPointsToCopy > 10000) return false;

			//Now copy all the points from the class list to the user list
			memcpy((void*)PointsListInDoubleFormat, (void*)DetectedPointsList, NumberOfPointsToCopy * sizeof(double) * 2);
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0003", __FILE__, __FUNCSIG__); return false; }
	}
		
	//----------------------------------------------------------------------
	//The 3 Public functions for straight rect, angular rect and curved box respectively.
	//----------------------------------------------------------------------

	//This function detects an edge in the specified direction for the straight rectangular area
	int EdgeDetectionSingleChannel::DetectEdgeStraightRect(int *FrameGrabBox, ScanDirection Scan_Direction)
	{	
		try
		{
			//DWORD dwStart = timeGet
			struct Pt Point;
			std::list<struct Pt> PointsList;
			int i = 0;
			int Box[4] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3]};

			/*Box[1] = ImageHeight - Box[1] - Box[3];

			if(Scan_Direction == Upwards) Scan_Direction = Downwards;
			else if(Scan_Direction == Downwards) Scan_Direction = Upwards;*/
			
			//Get one single point from where edge traversal can start
			if(!GetSingleEdgePointStraightRect(Box, Scan_Direction, &Point)) return 0;
			
			PointsList.push_back(Point);	//Put this one point into the list
			//and call traverse edge function
			if (!TraverseEdgeStraightRect3(Box, &PointsList, Scan_Direction)) return 0;
			//if (!TraverseEdgeStraightRect(Box, &PointsList, Scan_Direction)) return 0;
			//if(!TraverseEdge(StraightRect, Box, &PointsList)) return 0;
			//Now put all the points in the linear array after correction if required...
			//DWORD dwfin = timeGet
			return AddEdgePtsToCollection(PointsList);			
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0004", __FILE__, __FUNCSIG__); return -1; }
	}
	
	//This function detects an edge in the specified direction for the angular rectangular area
	int EdgeDetectionSingleChannel::DetectEdgeAngularRect(int *FrameGrabBox, bool OppositeScan)
	{
		SetAndRaiseErrorMessage("EDCSC0013", "DetectEdgeAngularRect", __FUNCSIG__);
		try
		{
			struct Pt Point, TravelReferencePt;
			std::list<struct Pt> PointsList;
			int Box[5] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3], FrameGrabBox[4]};

			/*Box[1] = ImageHeight - 1 - Box[1];
			Box[3] = ImageHeight - 1 - Box[3];*/
			
			//Get one single point from where edge traversal can start
			ScanDirection sd;

			//if(!GetSingleEdgePointAngularRect2(Box, OppositeScan, &Point, &sd)) return 0;
			if (!GetSingleEdgePointAngularRect2(Box, OppositeScan, &Point, &sd)) return 0;
			//Let us get the Reference point with which to get the travel direction properly. 
			switch (sd)
			{
			case EdgeDetectionSingleChannel::Rightwards:
				TravelReferencePt.X = -1000;
				if (Box[2] < Box[0])
					TravelReferencePt.Y = Box[3]; // +Box[3]) / 2;
				else
					TravelReferencePt.Y = Box[1]; // +Box[3]) / 2;
				break;
			case EdgeDetectionSingleChannel::Downwards:
				TravelReferencePt.Y = -1000;
				//TravelReferencePt.X = (Box[0] + Box[2]) / 2;
				if (Box[2] < Box[0])
					TravelReferencePt.X = Box[2]; // +Box[3]) / 2;
				else
					TravelReferencePt.X = Box[0]; // +Box[3]) / 2;
				break;
			case EdgeDetectionSingleChannel::Leftwards:
				TravelReferencePt.X = ImageWidth + 1000;
				//TravelReferencePt.Y = (Box[1] + Box[3]) / 2;
				if (Box[2] < Box[0])
					TravelReferencePt.Y = Box[3]; // +Box[3]) / 2;
				else
					TravelReferencePt.Y = Box[1]; // +Box[3]) / 2;
				break;

				break;
			case EdgeDetectionSingleChannel::Upwards:
				TravelReferencePt.Y = ImageHeight + 1000;
				//TravelReferencePt.X = (Box[0] + Box[2]) / 2;
				if (Box[2] < Box[0])
					TravelReferencePt.X = Box[2]; // +Box[3]) / 2;
				else
					TravelReferencePt.X = Box[0]; // +Box[3]) / 2;
				break;
			default:
				break;
			}
			//GetEdgePointAngularPosition(&Point, TravelReferencePt);
			PointsList.push_back(Point);	//Put this one point into the list
			////and call traverse edge function
			////if (!TraverseEdge(AngularRect, Box, &PointsList)) return 0;
			int Status = TraverseEdgeAngularRect3(Box, &PointsList, sd, TravelReferencePt);
			//std::string msg = std::to_string(Status); // +"  of max: " + std::to_string(BoxLength);
			//wchar_t tt[40];
			//swprintf(tt, L"%d", Status);
			//MessageBox(NULL, tt, NULL, NULL);

			//MessageBox(NULL, (LPCWSTR)msg.c_str(), L"Rapid-I", MB_TOPMOST);

			if (Status < 0)
			{
				SetAndRaiseErrorMessage("EDSC0005   " + std::to_string(Status), __FILE__, __FUNCSIG__);
				return 0;
			}
			//Now put all the points in the linear array after correction if required...
			return AddEdgePtsToCollection(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0005", __FILE__, __FUNCSIG__); return -1; }
	}

	void EdgeDetectionSingleChannel::GetEdgePointAngularPosition(struct Pt *Point, struct Pt ReferencePt)
	{
		SetAndRaiseErrorMessage("EDCSC0014", "GetEdgePointAngularPosition", __FUNCSIG__);
		if (Point->X == ReferencePt.X)
		{
			if (Point->Y > ReferencePt.Y) Point->ScanAnglePosition = M_PI_2;
			else Point->ScanAnglePosition = 3 * M_PI_2;
		}
		else
		{
			Point->ScanAnglePosition = atan((Point->Y - ReferencePt.Y) / (Point->X - ReferencePt.X));
			if (Point->ScanAnglePosition < 0) Point->ScanAnglePosition += M_PI;
			if (Point->X < ReferencePt.X && Point->Y < ReferencePt.Y) Point->ScanAnglePosition += M_PI;
			if (Point->X > ReferencePt.X && Point->Y < ReferencePt.Y) Point->ScanAnglePosition += M_PI;
		}
		//int ans = 1, count = 0;
		//for (int i = 1; i < 10; i++)
		//{
		//	count++;
		//	if (i == 5)
		//	{
		//		ans = i; i = 8;
		//	}
		//}
	}

	
	//This function detects an edge in the specified direction for the curved Box area
	int EdgeDetectionSingleChannel::DetectEdgeCurvedBox(double *FrameGrabBox, bool OppositeScan)
	{
		SetAndRaiseErrorMessage("EDCSC0015", "DetectEdgeCurvedBox", __FUNCSIG__);
		try
		{
			struct Pt Point;
			std::list<struct Pt> PointsList;
			//DoubleEdgeCt = 0;
			double Box[6] = { FrameGrabBox[0], FrameGrabBox[1], FrameGrabBox[2], FrameGrabBox[3], FrameGrabBox[4] , FrameGrabBox[5] };
			//double cs_Box[6] = { FrameGrabBox[0] * this->CamSizeRatio, FrameGrabBox[1] * this->CamSizeRatio, FrameGrabBox[2] * this->CamSizeRatio, 
			//					 FrameGrabBox[3] * this->CamSizeRatio, FrameGrabBox[4] , FrameGrabBox[5] };

			//if (this->CameraType == 3)
			//{
				//if (!GetSingleEdgePointCurvedBox2(cs_Box, 20, 20, OppositeScan, &Point, 1)) return 0;

				//PointsList.push_back(Point);	//Put this one point into the list
				//////and call traverse edge function
				////if (!TraverseEdge(CurvedBox, Box, &PointsList)) return 0;
				////if (!TraverseEdgeCurvedBox(Box, &PointsList, sd)) return 0;
				//if (!TraverseEdgeCurvedBox1(Box, &PointsList, OppositeScan)) return 0;
				if (!TraverseEdgeCurvedBox2(Box, &PointsList, OppositeScan)) return 0;
			//	//wchar_t tt[40];
			//	//swprintf(tt, L"%d", DoubleEdgeCt);
			//	//MessageBox(NULL, tt,  L"Rapid-I", MB_TOPMOST);
			//}
			//else
			//{
			//	ScanDirection sd;
			//	if (!GetSingleEdgePointCurvedBox(Box, OppositeScan, &Point)) return 0;

			//	PointsList.push_back(Point);	//Put this one point into the list
			//									//////and call traverse edge function
			//									//if (!TraverseEdge(CurvedBox, Box, &PointsList)) return 0;
			//									//if (!TraverseEdgeCurvedBox(Box, &PointsList, sd)) return 0;
			//	if (!TraverseEdgeCurvedBox(Box, &PointsList)) return 0;

			//}
			//Now put all the points in the linear array after correction if required...
			return AddEdgePtsToCollection(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0006", __FILE__, __FUNCSIG__); return -1; }
	}
	
	//----------------------------------------------------------------------
	//The 3 private functions that get a single edge point in straight rect, angular rect and curved box respectively. The other 2 are the helper functions
	//----------------------------------------------------------------------

	//This function scans for an edge point within a straight rect in the specified direction
	bool EdgeDetectionSingleChannel::GetSingleEdgePointStraightRect(int *Box, ScanDirection Scan_Direction, struct Pt *DetectedPoint, bool SingleLineScan)	   //box(0,0,800,600)
	{
		SetAndRaiseErrorMessage("EDCSC0016", "GetSingleEdgePointStraightRect", __FUNCSIG__);
		try
		{
			double pixels[40] = {0};
			int XIterator, YIterator,  LookAheadIndex, i;
			double var, pixelDiff;
			struct Pt Point;
			bool ScnDirection = true;														//setting window boundaries to scan within window 
			int LeftBoundry = Box[0] * this->CamSizeRatio + BoundryBuffer;
			int RightBoundry = Box[0] * this->CamSizeRatio + Box[2] * this->CamSizeRatio - BoundryBuffer - 1;
			int TopBoundry = Box[1] * this->CamSizeRatio + BoundryBuffer;
			int BottomBoundry = Box[1] * this->CamSizeRatio + Box[3] * this->CamSizeRatio - BoundryBuffer - 1;
			int PixelArrayIndex;
			
			switch(Scan_Direction)
			{
			case Rightwards:
				YIterator = TopBoundry; // Box[1] + Box[3] / 2; //Lets start from middle row //
				//Rightward Scanning starts here i.e scanning from left to right starting from middle row till the end, 
				//and then again from middle row till the start in the opposite direction.
				for(int j = 0; j < 2; j++)	//2 loops: 1st for going from middle to last and 2nd for moving from middle to first row 
				{
					while(YIterator >= TopBoundry && YIterator <= BottomBoundry)
					{
						for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							pixelDiff = abs(iBytes[PixelArrayIndex] - iBytes[(PixelArrayIndex + EdgeFinderPixelRange)]);
							if(pixelDiff < Tolerance)
								continue;

							//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
							//for this purpose, first it must be determined till where this checking must be carried out
							if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
								//the checking can continue for 'look ahead' number of pixels
								LookAheadIndex = LookAheadLimit;
							else
								//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
								LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
							//This is where the actual checking happens the range for which has been determined in the previous if statement
							for(i = EdgeFinderPixelRange; i <=  LookAheadIndex; i++)
							{	pixelDiff = abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]);
								if(pixelDiff < Tolerance)
								{
									//its not a legitimate edge as there is no continuity of transition
									//hence jump to the point where the check failed and continue searching for edge from this point forward
									XIterator = XIterator + i - PixelSkipValue;
									goto last1;
								}
							}
							//copy the 4 pixels where edge has been detected into 'pixels' array
							for(int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[PixelArrayIndex + j];
						
							var = PinPointEdge(XIterator, YIterator, pixels);
							if(var == -1)
								// :( No edge detected
								goto last1;
						
							//Ah!! we have an edge
							Point.X = XIterator + var;
							Point.Y = YIterator;
							//Put this into the pointer and return true
							*DetectedPoint = Point;
							return true;
						
	last1:					i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
						}
						if(SingleLineScan) return false;	//this means that the previous outer while loop iterates only once
						if(ScnDirection) YIterator++;	//when scanning from middle to last row
						else YIterator--;	//when scanning from middle to first row
					}
					if(j == 0)
					{
						ScnDirection = false; //change the direction
						YIterator = Box[1] + Box[3] / 2 - 1; //We need to start from middle again
					}
				}
				break;
			case Leftwards:
				//Note: To undersatnd the following statements, read the comments in rightward scanning section,
				//they are similar(but not exactly same)
				YIterator = TopBoundry; // Box[1] + Box[3] / 2;
				//Leftward Scanning starts here i.e scanning from right to left starting from 1st row
				for(int j = 0; j < 2; j++)
				{
					while(YIterator >= TopBoundry && YIterator <= BottomBoundry)
					{
						for(XIterator = RightBoundry; XIterator >= LeftBoundry; XIterator -= PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							pixelDiff = abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - EdgeFinderPixelRange)]);
							if(pixelDiff < Tolerance)
								continue;

							if((XIterator - LookAheadLimit) >= BoundryBuffer)
								LookAheadIndex = LookAheadLimit;
							else
								LookAheadIndex = XIterator - BoundryBuffer;
						
							for(i = EdgeFinderPixelRange; i <=  LookAheadIndex; i++)
							{	pixelDiff = abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]);
								if(pixelDiff < Tolerance)
								{
									XIterator = XIterator - i + PixelSkipValue;
									goto last2;
								}
							}
							for(int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[(PixelArrayIndex - j)];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							if(var == -1) 
								goto last2;
						
							Point.X = XIterator - var;
							Point.Y = YIterator;
							
							*DetectedPoint = Point;
							return true;

	last2:					i = 1; //the dummy statement
						}
						if(SingleLineScan) return false;	//this means that the previous outer while loop iterates only once
						if(ScnDirection) YIterator++;
						else YIterator--;
					}
					if(j == 0)
					{
						ScnDirection = false;
						YIterator = Box[1] + Box[3] / 2 - 1; 
					}
				}
				break;
			case Downwards:
				//Note: To undersatnd the following statements, read the comments in rightward scanning section,
				//they are similar(not exactly same)
				XIterator = LeftBoundry;// Box[0] + Box[2] / 2;																	//start scanning from middle
				//Downward Scanning starts here i.e scanning from top to bottom starting from 1st column
				for(int j = 0; j < 2; j++)
				{
					while(XIterator >= LeftBoundry && XIterator <= RightBoundry)
					{
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + EdgeFinderPixelRange) * ImageWidth)]) < Tolerance)
								continue;

							if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
						
							for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
							{	pixelDiff = abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]);
								if(pixelDiff < Tolerance)
								{
									YIterator = YIterator + i /ImageWidth - PixelSkipValue;
									goto last3;
								}
							}
							for(int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							if(var == -1) 
								goto last3;
						
							Point.X = XIterator;
							Point.Y = YIterator + var;
							*DetectedPoint = Point;
							return true;

	last3:					i = 1;	//the dummy statement
						}
						if(SingleLineScan) return false;	//this means that the previous outer while loop iterates only once
						if(ScnDirection) XIterator++;
						else XIterator--;
					}
					if(j == 0)
					{
						ScnDirection = false;
						XIterator = Box[0] + Box[2] / 2 - 1; 
					}
				}
				break;
			case Upwards:
				//Note: To undersatnd the following statements, read the comments in rightward scanning section,
				//they are similar(not exactly same)
				XIterator = LeftBoundry; // Box[0] + Box[2] / 2;
				//Upward Scanning starts here i.e scanning from bottom to top starting from 1st column
				for(int j = 0; j < 2; j++)
				{
					while(XIterator >= LeftBoundry && XIterator <= RightBoundry)
					{
						for(YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator -= PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							pixelDiff = abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator - EdgeFinderPixelRange) * ImageWidth)]);
							if(pixelDiff < Tolerance)
								continue;

							if((YIterator - LookAheadLimit) >= BoundryBuffer)
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;
						
							for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
							{	pixelDiff = abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]);
								if(pixelDiff < Tolerance)
								{
									YIterator = YIterator - i / ImageWidth + PixelSkipValue;
									goto last4;
								}
							}
							for(int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[(XIterator + (YIterator - j) * ImageWidth)];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							if(var == -1) 
								goto last4;
						
							Point.X = XIterator;
							Point.Y = YIterator - var;
							*DetectedPoint = Point;
							return true;

	last4:					i = 1; //the dummy statement
						}
						if(SingleLineScan) return false;	//this means that the previous outer while loop iterates only once
						if(ScnDirection) XIterator++;
						else XIterator--;
					}
					if(j == 0)
					{
						ScnDirection = false;
						XIterator = Box[0] + Box[2] / 2 - 1; 
					}
				}
				break;
			}

			return false; //Alas!! we searched the whole area but couldn't find a single edge point
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0007", __FILE__, __FUNCSIG__); return false; }
	}
	
	//This function scans for an edge point within an angular rect in the specified direction
	bool EdgeDetectionSingleChannel::GetSingleEdgePointAngularRect(int *Box, bool OppositeScan, struct Pt *DetectedPoint, ScanDirection *Scan_Direction)
	{
		SetAndRaiseErrorMessage("EDCSC0017", "GetSingleEdgePointAngularRect", __FUNCSIG__);
		try
		{
			Pt Point1, Point2, OriginPoint, Point;
			int Width = Box[4], Height, XIterator, YIterator;
			double CosTh, SinTh;// , pixelDiff;
			double pixels[40] = {0};
			int LookAheadIndex, i;
			double var;
			//ScanDirection Scan_Direction; //This one is for determining which direction to scan
			int PixelArrayIndex;
			int DirSign = 1;

			Point1.X = Box[0];
			Point1.Y = Box[1];
			Point2.X = Box[2];
			Point2.Y = Box[3];

			Height = sqrt(pow((Point2.X - Point1.X), 2) + pow((Point2.Y - Point1.Y), 2)); //height of the box is the dist between the 2 points

			if(abs(Point1.X - Point2.X) > abs(Point1.Y - Point2.Y))
			{
				//Perform Vertical scan
				if(!OppositeScan)
				{
					if(Point2.X > Point1.X)
						*Scan_Direction = Downwards;
					else
						*Scan_Direction = Upwards;
				}
				else 
				{
					if(Point2.X > Point1.X)
						*Scan_Direction = Upwards;
					else
						*Scan_Direction = Downwards;
				}
			}
			else
			{
				//Perform Horizontal scan
				if(!OppositeScan)
				{
					if(Point2.Y > Point1.Y)
						*Scan_Direction = Leftwards;
					else
						*Scan_Direction = Rightwards;
				}
				else 
				{
					if(Point2.Y > Point1.Y)
						*Scan_Direction = Rightwards;
					else
						*Scan_Direction = Leftwards;
				}
			}


			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if(Box[2] < Box[0])
			{	
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}

			if (*Scan_Direction == Leftwards || *Scan_Direction == Rightwards)
			{
				if (Point1.Y > Point2.Y) DirSign = -1;
			}
			else
			{

			}

			CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X), 2) + pow((Point2.Y - Point1.Y), 2));
			SinTh = (Point2.Y - Point1.Y) / sqrt(pow((Point2.X - Point1.X), 2) + pow((Point2.Y - Point1.Y), 2));

			//if (Point1.X < Point2.X)
			//{

				XIterator = (Point1.X + Point2.X) / 2; // (BoundryBuffer + 5) * CosTh;
				YIterator = (Point1.Y + Point2.Y) / 2; // (BoundryBuffer + 5) * SinTh;
			//}
			//else
			//{
			//	XIterator = Point2.X + (BoundryBuffer + 5) * CosTh;
			//	YIterator = Point2.Y + (BoundryBuffer + 5) * SinTh;
			//}

			////Recast Cos and sin theta for rearranged points
			//CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X), 2) + pow((Point2.Y - Point1.Y), 2));
			//SinTh = (Point1.Y - Point2.Y) / sqrt(pow((Point2.X - Point1.X), 2) + pow((Point2.Y - Point1.Y), 2));

			//The left top corner of the rect is taken as the origin for the transformation
			OriginPoint.X = Point1.X; // -Width / 2 * SinTh;
			OriginPoint.Y = Point1.Y; // -Width / 2 * CosTh;

			////Start from the mid point of the rect
			//XIterator = Point1.X; // (Point1.X + Point2.X) / 2;
			//YIterator = Point1.Y; // (Point1.Y + Point2.Y) / 2;

			switch(*Scan_Direction)
			{
			case Rightwards:
				//if (Point1.X < Point2.X)
				//{
				//	XIterator = Point1.X + BoundryBuffer;
				//	YIterator = Point1.Y;
				//}
				//else
				//{
				//	XIterator = Point2.X + BoundryBuffer;
				//	YIterator = Point2.Y;
				//}
				//Go to the left end of the box so that the scanning can start from there
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height)) XIterator--;
				XIterator++;
				
				//Now scan from left end to right end
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + EdgeFinderPixelRange)]) < Tolerance)
						goto last1;

					//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
					//for this purpose, first it must be determined till where this checking must be carried out
					if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
						//the checking can continue for 'look ahead' number of pixels
						LookAheadIndex = LookAheadLimit;
					else
						//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
						LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
					 
					//This is where the actual checking happens the range for which has been determined in the previous if statement
					for(i = EdgeFinderPixelRange; i <= LookAheadIndex; i++)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
						{
							//its not a legitimate edge as there is no continuity of transition
							//hence jump to the point where the check failed and continue searching for edge from this point forward
							XIterator = XIterator + i - PixelSkipValue; 
							goto last1;
						}
						//copy the 4 pixels where edge has been detected into 'pixels' array
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[PixelArrayIndex + j];
			
					//var = PinPointEdge(pixels);
					var = PinPointEdge(XIterator, YIterator, pixels);
					if(var == -1) 
						goto last1;
					 
					Point.X = XIterator + var;
					Point.Y = YIterator;
					Point.direction = -1;
					if(pixels[0] > pixels[EdgeFinderPixelRange]) Point.direction = 1;

					//We have a point, return true
					*DetectedPoint = Point;
					return true;
					 
	last1:			XIterator += 2;
				}
				break;

			case Leftwards:

				//Go to the right end of the box so that the scanning can start from there
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height)) XIterator++;
				XIterator--;
				//Now scan right to left
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					double pixelDiff = abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - EdgeFinderPixelRange)]);
					if(pixelDiff < Tolerance)
						goto last2;

					if((XIterator - LookAheadLimit) >= BoundryBuffer)
						LookAheadIndex = LookAheadLimit;
					else
						LookAheadIndex = XIterator - BoundryBuffer;
						
					for(i = EdgeFinderPixelRange; i <=  LookAheadIndex; i++)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]) < Tolerance)
						{
							XIterator = XIterator - i + PixelSkipValue;
							goto last2;
						}
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[PixelArrayIndex - j];
				
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
					if(var == -1) 
						goto last2;
						
					Point.X = XIterator - var;
					Point.Y = YIterator;
					Point.direction = -1;
					if(pixels[0] > pixels[EdgeFinderPixelRange]) Point.direction = 1;

					*DetectedPoint = Point;
					return true;

	last2:			XIterator -= 2; 
				}
				break;

			case Downwards:

				//Go to the top end of the box so that the scanning can start from there
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height)) YIterator--;
				YIterator++;
				//Now scan up to down
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + EdgeFinderPixelRange) * ImageWidth)]) < Tolerance)
						goto last3;
					if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
						LookAheadIndex = LookAheadLimit * ImageWidth;
					else
						LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
						
					for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
						{
							YIterator = YIterator + i /ImageWidth - PixelSkipValue;
							goto last3;
						}
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
				
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
					if(var == -1) 
						goto last3;
					  
					Point.X = XIterator;
					Point.Y = YIterator + var;
					Point.direction = -3;
					if(pixels[0] > pixels[EdgeFinderPixelRange]) Point.direction = 3;

					*DetectedPoint = Point;
					return true;

	last3:			YIterator += 2;
				}
				break;

			case Upwards:

				//Go to the bottom end of the box so that the scanning can start from there
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height)) YIterator++;
				YIterator--;
				//Now scan from down to up
				while(CheckPointWithinAngularRect(XIterator, YIterator, SinTh, CosTh, OriginPoint, Width, Height))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator - EdgeFinderPixelRange) * ImageWidth)]) < Tolerance)
						goto last4;

					if((YIterator - LookAheadLimit) >= BoundryBuffer)
						LookAheadIndex = LookAheadLimit * ImageWidth;
					else
						LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;
					  
					for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]) < Tolerance)
						{
							YIterator = YIterator - i / ImageWidth + PixelSkipValue;
							goto last4;
						}
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[(XIterator + (YIterator - j) * ImageWidth)];
				
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
					if(var == -1) 
						goto last4;
					   
					Point.X = XIterator;
					Point.Y = YIterator - var;
					Point.direction = -4;
					if(pixels[0] > pixels[EdgeFinderPixelRange]) Point.direction = 4;
					*DetectedPoint = Point;
					return true;

	last4:			YIterator -= 2; 
				}
			}
			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0008", __FILE__, __FUNCSIG__); return false;}
	}
	
	bool EdgeDetectionSingleChannel::GetSingleEdgePointAngularRect2(int *Box, bool OppositeScan, struct Pt *DetectedPoint, ScanDirection *Scan_Direction)
	{
		SetAndRaiseErrorMessage("EDCSC0018", "GetSingleEdgePointAngularRect2", __FUNCSIG__);
		try
		{
			Pt StPt, EndPt, OriginPoint;
			int Width = Box[4] * this->CamSizeRatio, Height, X, Y;
			double CosTh, SinTh;// , pixelDiff;
			//double pixels[40] = { 0 };
			//int LookAheadIndex;// , currentIndex;
			//double var;
			//ScanDirection Scan_Direction; //This one is for determining which direction to scan
			//int PixelArrayIndex;
			int DirSign = 1;

			StPt.X = Box[0] * this->CamSizeRatio;
			StPt.Y = Box[1] * this->CamSizeRatio;
			EndPt.X = Box[2] * this->CamSizeRatio;
			EndPt.Y = Box[3] * this->CamSizeRatio;

			Height = sqrt(pow((EndPt.X - StPt.X), 2) + pow((EndPt.Y - StPt.Y), 2)); //height of the box is the dist between the 2 points

			if (abs(StPt.X - EndPt.X) > abs(StPt.Y - EndPt.Y))
			{
				//Perform Vertical scan
				if (!OppositeScan)
				{
					if (EndPt.X > StPt.X)
						*Scan_Direction = Downwards;
					else
						*Scan_Direction = Upwards;
				}
				else
				{
					if (EndPt.X > StPt.X)
						*Scan_Direction = Upwards;
					else
						*Scan_Direction = Downwards;
				}
			}
			else
			{
				//Perform Horizontal scan
				if (!OppositeScan)
				{
					if (EndPt.Y > StPt.Y)
						*Scan_Direction = Leftwards;
					else
						*Scan_Direction = Rightwards;
				}
				else
				{
					if (EndPt.Y > StPt.Y)
						*Scan_Direction = Rightwards;
					else
						*Scan_Direction = Leftwards;
				}
			}


			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if (Box[2] < Box[0])
			{
				StPt.X = Box[2] * this->CamSizeRatio;
				StPt.Y = Box[3] * this->CamSizeRatio;
				EndPt.X = Box[0] * this->CamSizeRatio;
				EndPt.Y = Box[1] * this->CamSizeRatio;
			}

			if (*Scan_Direction == Leftwards || *Scan_Direction == Rightwards)
			{
				if (StPt.Y > EndPt.Y) DirSign = -1;
			}
			else
			{
				if (StPt.X > EndPt.X) DirSign = -1;
			}

			CosTh = (EndPt.X - StPt.X) / sqrt(pow((EndPt.X - StPt.X), 2) + pow((EndPt.Y - StPt.Y), 2));
			SinTh = (EndPt.Y - StPt.Y) / sqrt(pow((EndPt.X - StPt.X), 2) + pow((EndPt.Y - StPt.Y), 2));

			X = StPt.X + 2 * CosTh;
			Y = StPt.Y + 2 * SinTh;
			//XIterator = (StPt.X + EndPt.X) / 2; // (BoundryBuffer + 5) * CosTh;
			//YIterator = (StPt.Y + EndPt.Y) / 2; // (BoundryBuffer + 5) * SinTh;
												//The left top corner of the rect is taken as the origin for the transformation
			OriginPoint.X = StPt.X; // -Width / 2 * SinTh;
			OriginPoint.Y = StPt.Y; // -Width / 2 * CosTh;
			bool GotPt = false;	
			int ScanDirMult = 1;
			int scanBoxSize = 20;
			int y_StepSize = scanBoxSize * abs(SinTh), x_StepSize = 20 * abs(CosTh);
			if (y_StepSize < 1) y_StepSize = 1;
			if (x_StepSize < 1) x_StepSize = 1;

			int pos_st = 0, pos_end = 0;
			switch (*Scan_Direction)
			{
			case Rightwards:
			case Leftwards:
				//First let us fix the Reference pt that will be used for Checking direction of travel and ensure that there is no return in the same region
				//EdgeTravelReferencePt. = (StPt.X + EndPt.X) / 2 - 1000; EdgeTravelReferencePt.y = 

				//X -= Width / 2;
				//for (int j = 0; j < 2; j++)
				//{
				//	for (int i = 0; i < Width / 2; i++)
				//	{
				//		GotPt = GetEdgePtNearGivenPtAngRect_H(X + i * ScanDirMult, Y, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, DirSign);
				//		if (GotPt) break;
				//		else GotPt = GetEdgePtNearGivenPtAngRect_H(X + i * ScanDirMult, Y, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, -1 * DirSign);
				//		if (GotPt) break;
				//	}
				//	ScanDirMult *= -1;
				//}
				pos_st = StPt.Y; pos_end = EndPt.Y;
				if (StPt.Y > EndPt.Y)
				{
					pos_st = EndPt.Y; pos_end = StPt.Y;
					for (Y = pos_end - 2; Y > pos_st + 2; Y -= y_StepSize) //- y_StepSize / 2 //y_StepSize / 2
					{
						for (X = StPt.X - Width / 2 + x_StepSize / 2; X < StPt.X + Width / 2 - x_StepSize / 2; X += x_StepSize)
						{
							GotPt = GetEdgePtNearGivenPtAngRect_H(X, Y, *Scan_Direction, scanBoxSize, scanBoxSize, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, DirSign);
							if (GotPt) return GotPt;
						}
					}
				}
				else
				{
					for (Y = pos_st + 2; Y < pos_end - 2; Y += y_StepSize) // + y_StepSize / 2 //y_StepSize / 2
					{
						for (X = StPt.X - Width / 2 + x_StepSize / 2; X < StPt.X + Width / 2 - x_StepSize / 2; X += x_StepSize)
						{
							GotPt = GetEdgePtNearGivenPtAngRect_H(X, Y, *Scan_Direction, scanBoxSize, scanBoxSize, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, DirSign);
							if (GotPt) return GotPt;
						}
					}
				}
				return GotPt;
				break;

			case Upwards:
			case Downwards:
				//for (int j = 0; j < 2; j++)
				//{
				//	for (int i = 0; i < Width / 2; i++)
				//	{
				//		GotPt = GetEdgePtNearGivenPtAngRect_V(X, Y + i * ScanDirMult, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, DirSign);
				//		if (GotPt) break;
				//		else GotPt = GetEdgePtNearGivenPtAngRect_V(X, Y + i * ScanDirMult, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, -1 * DirSign);
				//		if (GotPt) break;
				//	}
				//	ScanDirMult *= -1;
				//}

				//return GotPt;
				for (X = StPt.X + 2; X < EndPt.X - 2; X += x_StepSize) //x_StepSize / 2 // x_StepSize / 2
				{
					for (Y = StPt.Y - Width / 2 + y_StepSize; Y < StPt.Y + Width / 2 - y_StepSize; Y += y_StepSize)
					{
						GotPt = GetEdgePtNearGivenPtAngRect_V(X, Y, *Scan_Direction, scanBoxSize, scanBoxSize, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, DirSign);
						if (GotPt) return GotPt;
					}

				}
				break;

				//for (int j = 0; j < Width; j++)
				//{
				//	currentIndex = (Y + j) * ImageWidth + X;
				//	for (int i = 0; i < Width; i+= EdgeFinderPixelRange)
				//	{
				//		//Check for jump in color between current pixel and pixel edgefinder step away.
				//		if (abs(iBytes[currentIndex + i] - iBytes[currentIndex + i + EdgeFinderPixelRange]) > Tolerance)
				//		{
				//			int cCt = 0;
				//			//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
				//			for (int k = 0; k <= EdgeFinderPixelRange; k++)
				//			{
				//				if (abs(iBytes[currentIndex + i] - iBytes[currentIndex + i + EdgeFinderPixelRange + k]) > Tolerance)
				//					cCt++;
				//			}
				//			if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
				//			{
				//				if (CheckPointWithinAngularRect(X + i, Y + j, SinTh, CosTh, OriginPoint, Width, Height))
				//				{	
				//					//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
				//					int xx = X + i;
				//					int GotPt = ScanHorizontalAndInsertToList(&xx, Y + j, DetectedPoint);
				//					if (GotPt == 1)
				//					//DetectedPoint->X = X + i;
				//					//DetectedPoint->Y = Y + j;
				//						return true;
				//				}
				//			}
				//		}
				//	}
				//}

			//case Leftwards:
			//	//X += Width / 2;
			//	//return GetEdgePtNearGivenPtAngRect_H(X, Y, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, 1);
			//	GotPt = GetEdgePtNearGivenPtAngRect_H(X, Y, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, -1);
			//	if (!GotPt)
			//		GotPt = GetEdgePtNearGivenPtAngRect_H(X, Y, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, 1);
			//	return GotPt;

			//	//for (int j = 0; j < Width; j++)
			//	//{
			//	//	currentIndex = (Y + j) * ImageWidth + X;
			//	//	for (int i = 0; i < Width; i += EdgeFinderPixelRange)
			//	//	{
			//	//		//Check for jump in color between current pixel and pixel edgefinder step away.
			//	//		if (abs(iBytes[currentIndex - i] - iBytes[currentIndex - i - EdgeFinderPixelRange]) > Tolerance)
			//	//		{
			//	//			int cCt = 0;
			//	//			//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
			//	//			for (int k = 0; k <= EdgeFinderPixelRange; k++)
			//	//			{
			//	//				if (abs(iBytes[currentIndex - i] - iBytes[currentIndex - i - EdgeFinderPixelRange - k]) > Tolerance)
			//	//					cCt++;
			//	//			}
			//	//			if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
			//	//			{
			//	//				if (CheckPointWithinAngularRect(X - i, Y + j, SinTh, CosTh, OriginPoint, Width, Height))
			//	//				{
			//	//					//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
			//	//					DetectedPoint->X = X - i;
			//	//					DetectedPoint->Y = Y + j;
			//	//					return true;
			//	//				}
			//	//			}
			//	//		}
			//	//	}
			//	//}

			//	break;

			//case Downwards:
			//	//Y -= Width / 2;
			//	return GetEdgePtNearGivenPtAngRect_V(X, Y, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height);

			//	//for (int j = 0; j < Width; j++) //X scan
			//	//{
			//	//	for (int i = 0; i < Width; i += EdgeFinderPixelRange) //Y Scan
			//	//	{
			//	//		currentIndex = (Y + i) * ImageWidth + X + j;

			//	//		//Check for jump in color between current pixel and pixel edgefinder step away.
			//	//		if (abs(iBytes[currentIndex] - iBytes[currentIndex + EdgeFinderPixelRange * ImageWidth]) > Tolerance)
			//	//		{
			//	//			int cCt = 0;
			//	//			//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
			//	//			for (int k = 0; k <= EdgeFinderPixelRange; k++)
			//	//			{
			//	//				if (abs(iBytes[currentIndex] - iBytes[currentIndex + (EdgeFinderPixelRange + k) * ImageWidth]) > Tolerance)
			//	//					cCt++;
			//	//			}
			//	//			if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
			//	//			{
			//	//				int EdgeLocation = CheckPointWithinAngularRect(X + j, Y + i, SinTh, CosTh, OriginPoint, Width, Height);
			//	//				if (EdgeLocation == 0)
			//	//				{
			//	//					//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
			//	//					DetectedPoint->X = X + j;
			//	//					DetectedPoint->Y = Y + i;
			//	//					return true;
			//	//				}
			//	//			}
			//	//		}
			//	//	}
			//	//}

			//	break;

			//case Upwards:
			//	//Y += Width / 2;
			//	return GetEdgePtNearGivenPtAngRect_V(X, Y, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height);

			//	for (int j = 0; j < Width; j++) //X scan
			//	{
			//		for (int i = 0; i < Width; i += EdgeFinderPixelRange) //Y Scan
			//		{
			//			currentIndex = (Y - i) * ImageWidth + X + j;

			//			//Check for jump in color between current pixel and pixel edgefinder step away.
			//			if (abs(iBytes[currentIndex] - iBytes[currentIndex - EdgeFinderPixelRange * ImageWidth]) > Tolerance)
			//			{
			//				int cCt = 0;
			//				//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
			//				for (int k = 0; k <= EdgeFinderPixelRange; k++)
			//				{
			//					if (abs(iBytes[currentIndex] - iBytes[currentIndex - (EdgeFinderPixelRange + k) * ImageWidth]) > Tolerance)
			//						cCt++;
			//				}
			//				if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
			//				{
			//					int EdgeLocation = CheckPointWithinAngularRect(X + j, Y - i, SinTh, CosTh, OriginPoint, Width, Height);
			//					if (EdgeLocation == 0)
			//					{
			//						//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
			//						DetectedPoint->X = X + j;
			//						DetectedPoint->Y = Y - i;
			//						return true;
			//					}
			//				}
			//			}
			//		}
			//	}

			}
			return false;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0008", __FILE__, __FUNCSIG__); return false; }
	}


	bool EdgeDetectionSingleChannel::GetEdgePtNearGivenPtAngRect_H(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, double SinTH, double CosTH, Pt OriginPt, Pt *DetectedPoint, int Width, int Height, int yScanDir)
	{
		SetAndRaiseErrorMessage("EDCSC0019", "GetEdgePtNearGivenPtAngRect_H", __FUNCSIG__);
		try 
		{
			int currentIndex = 0;
			int inc_dir = 1;
			if (sd == Leftwards) inc_dir = -1;
			//DetectedPoint->X = -1; DetectedPoint->Y = -1;
			//First let us try above the current line. 
			//for (int k = 0; k < 2; k++)
			//{
			if (X < windowWidth / 2) Y = windowWidth / 2 + 1;
			if (Y < windowHeight / 2) Y = windowHeight / 2 + 1;

			for (int j = 0; j < windowHeight / 2; j++)
			{
				currentIndex = (Y + j * yScanDir) * ImageWidth + X - inc_dir * windowWidth / 2;
				for (int i = 0; i < windowWidth; i += EdgeFinderPixelRange)
				{
					//Check for jump in color between current pixel and pixel edgefinder step away.
					if (abs(iBytes[currentIndex + i * inc_dir] - iBytes[currentIndex + (i + EdgeFinderPixelRange) * inc_dir]) > Tolerance)
					{
						int cCt = 0;
						//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
						for (int k = 0; k <= EdgeFinderPixelRange; k++)
						{
							if (abs(iBytes[currentIndex + i * inc_dir] - iBytes[currentIndex + (i + EdgeFinderPixelRange + k) * inc_dir]) > Tolerance)
								cCt++;
						}
						if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
						{
							if (CheckPointWithinAngularRect(X - inc_dir * windowWidth / 2 + i * inc_dir, Y + j * yScanDir, SinTH, CosTH, OriginPt, Width, Height) == 0)
							{
								//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
								int xx = X - inc_dir * windowWidth / 2 + i * inc_dir;
							ShiftAndGetPt:
								int GotPt = ScanHorizontalAndInsertToList(&xx, Y + j * yScanDir, DetectedPoint);
								if (GotPt == 1)
									return true;
								else if (GotPt == -3)
								{
									xx += inc_dir; goto ShiftAndGetPt;
								}
							}
						}
					}
				}
			}
			//	yScanDir *= -1;
			//}
		}
		catch (...)
		{

		}
		return false;
	}

	bool EdgeDetectionSingleChannel::GetEdgePtNearGivenPtAngRect_V(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, double SinTH, double CosTH, Pt OriginPt, Pt *DetectedPoint, int Width, int Height, int xScanDir)
	{
		SetAndRaiseErrorMessage("EDCSC0020", "GetEdgePtNearGivenPtAngRect_V", __FUNCSIG__);
		try {
			int currentIndex = 0;
			int inc_dir = 1;
			if (sd == Upwards) inc_dir = -1;
			//DetectedPoint->X = -1; DetectedPoint->Y = -1;
			if (X < windowWidth / 2) Y = windowWidth / 2 + 1;
			if (Y < windowHeight / 2) Y = windowHeight / 2 + 1;

			for (int j = 0; j < windowWidth / 2; j++) //X scan
			{
				for (int i = 0; i < windowHeight; i += EdgeFinderPixelRange) //Y Scan
				{
					currentIndex = (Y - inc_dir * windowHeight / 2 + i * inc_dir) * ImageWidth + X + j * xScanDir;

					//Check for jump in color between current pixel and pixel edgefinder step away.
					if (abs(iBytes[currentIndex] - iBytes[currentIndex + EdgeFinderPixelRange * ImageWidth * inc_dir]) > Tolerance)
					{
						int cCt = 0;
						//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
						for (int k = 0; k <= EdgeFinderPixelRange; k++)
						{
							if (abs(iBytes[currentIndex] - iBytes[currentIndex + (EdgeFinderPixelRange + k) * ImageWidth * inc_dir]) > Tolerance)
								cCt++;
						}
						if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
						{
							int EdgeLocation = CheckPointWithinAngularRect(X + j * xScanDir, Y + i * inc_dir - inc_dir * windowHeight / 2, SinTH, CosTH, OriginPt, Width, Height);
							if (EdgeLocation == 0)
							{
								//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
								int yy = Y + i * inc_dir - inc_dir * windowHeight / 2;
							ShiftAndGetPt:
								int GotPt = ScanVerticalAndInsertToList(X + j * xScanDir, &yy, DetectedPoint);
								if (GotPt == 1)
									return true;
								else if (GotPt == -3)
								{
									yy += inc_dir; goto ShiftAndGetPt;
								}
							}
						}
					}
				}
			}
		}
		catch (...)
		{

		}
		return false;
	}

	bool EdgeDetectionSingleChannel::GetEdgePtNearGivenPtStraightRect_H(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, Pt *DetectedPoint, int* Box, int yScanDir)
	{
		SetAndRaiseErrorMessage("EDCSC0021", "GetEdgePtNearGivenPtStraightRect_H", __FUNCSIG__);
		try
		{
			int currentIndex = 0;
			int inc_dir = 1;
			if (sd == Leftwards) inc_dir = -1;
			for (int j = 0; j < windowHeight / 2; j++)
			{
				currentIndex = (Y + j * yScanDir) * ImageWidth + X;
				for (int i = -windowWidth / 2; i < windowWidth / 2; i += EdgeFinderPixelRange)
				{
					//Check for jump in color between current pixel and pixel edgefinder step away.
					if (abs(iBytes[currentIndex + i * inc_dir] - iBytes[currentIndex + (i + EdgeFinderPixelRange) * inc_dir]) > Tolerance)
					{
						int cCt = 0;
						//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
						for (int k = 0; k <= EdgeFinderPixelRange; k++)
						{
							if (abs(iBytes[currentIndex + i * inc_dir] - iBytes[currentIndex + (i + EdgeFinderPixelRange + k) * inc_dir]) > Tolerance)
								cCt++;
						}
						if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
						{
							if (CheckPointWithinStraightRect(X + i * inc_dir, Y + j * yScanDir, Box) == 0)
							{
								//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
								int xx = X + i * inc_dir;
							ShiftAndGetPt:
								int GotPt = ScanHorizontalAndInsertToList(&xx, Y + j * yScanDir, DetectedPoint);
								if (GotPt == 1)
									return true;
								else if (GotPt == -3)
								{
									xx += inc_dir; goto ShiftAndGetPt;
								}
							}
						}
					}
				}
			}
		}
		catch (...)
		{
		}
		return false;
	}

	bool EdgeDetectionSingleChannel::GetEdgePtNearGivenPtStraightRect_V(int X, int Y, ScanDirection sd, int windowWidth, int windowHeight, Pt *DetectedPoint, int *Box, int xScanDir)
	{
		SetAndRaiseErrorMessage("EDCSC0022", "GetEdgePtNearGivenPtStraightRect_V", __FUNCSIG__);
		try {
			int currentIndex = 0;
			int inc_dir = 1;
			if (sd == Upwards) inc_dir = -1;

			for (int j = 0; j < windowWidth / 2; j++) //X scan
			{
				for (int i = -windowHeight / 2; i < windowHeight / 2; i += EdgeFinderPixelRange) //Y Scan
				{
					currentIndex = (Y + i * inc_dir) * ImageWidth + X + j * xScanDir;

					//Check for jump in color between current pixel and pixel edgefinder step away.
					if (abs(iBytes[currentIndex] - iBytes[currentIndex + EdgeFinderPixelRange * ImageWidth * inc_dir]) > Tolerance)
					{
						int cCt = 0;
						//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
						for (int k = 0; k <= EdgeFinderPixelRange; k++)
						{
							if (abs(iBytes[currentIndex] - iBytes[currentIndex + (EdgeFinderPixelRange + k) * ImageWidth * inc_dir]) > Tolerance)
								cCt++;
						}
						if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
						{
							int EdgeLocation = CheckPointWithinStraightRect(X + j * xScanDir, Y + i * inc_dir, Box);
							if (EdgeLocation == 0)
							{
								//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
								int yy = Y + i * inc_dir;
							ShiftAndGetPt:
								int GotPt = ScanVerticalAndInsertToList(X + j * xScanDir, &yy, DetectedPoint);
								if (GotPt == 1)
									return true;
								else if (GotPt == -3)
								{
									yy += inc_dir; goto ShiftAndGetPt;
								}
							}
						}
					}
				}
			}
		}
		catch (...)
		{

		}
		return false;

	}

	int EdgeDetectionSingleChannel::GetEdgePtNearGivenPtCurvedBox(double Rad, double Theta, int windowWidth, int windowHeight, Pt *DetectedPoint, double* Box, int ScanDir, bool OppScan)
	{
		SetAndRaiseErrorMessage("EDCSC0023", "GetEdgePtNearGivenPtCurvedBox", __FUNCSIG__);
		try
		{
			int currentIndex = 0;
			int inc_dir = 1;
			ScanDirection sd = Rightwards;
			if (Theta > 2 * M_PI) Theta -= (2 * M_PI);

			if (Theta >= M_PI_4 && Theta < 3 * M_PI_4)
				if (!OppScan) sd = Upwards;
				else sd = Downwards;
			else if (Theta >= 3 * M_PI_4 && Theta < 5 * M_PI_4)
				if (!OppScan) sd = Leftwards;
				else sd = Rightwards;
			else if (Theta >= 5 * M_PI_4 && Theta < 7 * M_PI_4)
				if (!OppScan) sd = Downwards;
				else sd = Upwards;
			else
			{
				if (!OppScan) sd = Rightwards;
				else sd = Leftwards;
			}
			int X, Y, st = -windowWidth / 2, ed = windowWidth / 2, inc = EdgeFinderPixelRange;
			X = Box[0] + Rad * cos(Theta);
			Y = Box[1] - Rad * sin(Theta);
			if (X < windowWidth / 2) X = windowWidth / 2 + 1;
			if (X > ImageWidth - windowWidth / 2) X = ImageWidth - windowWidth / 2 - 1;
			if (Y < windowHeight / 2) Y = windowHeight / 2 + 1;
			if (Y > ImageHeight - windowHeight / 2) Y = ImageHeight - windowHeight / 2 - 1;

			Pt OriPt, cScanPos; OriPt.X = Box[0]; OriPt.Y = Box[1];
			int i = 0;

			if (sd == Rightwards || sd == Leftwards)
			{
				if (sd == Leftwards)
				{
					inc_dir = -1;
					st = windowWidth / 2;
					ed = -windowWidth / 2;
					inc = -EdgeFinderPixelRange;
				}
				for (int j = 0; j < windowHeight / 2; j++)
				{
					currentIndex = (Y + j * ScanDir) * ImageWidth + X;
					//for (int i = st; i < ed; i += inc)
					i = st;
					while (i != ed)
					{
						//Check for jump in color between current pixel and pixel edgefinder step away.
						if (abs(iBytes[currentIndex + i] - iBytes[currentIndex + i + inc]) > Tolerance)
						{
							int cCt = 0;
							//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
							for (int k = 0; k <= EdgeFinderPixelRange; k++)
							{
								if (abs(iBytes[currentIndex + i] - iBytes[currentIndex + i + inc + k * inc_dir]) > Tolerance)
									cCt++;
							}
							if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
							{
								int edgeType = CheckPointWithinCurvedBox2(X + i, Y + j * ScanDir, Box[4], Box[5], Box[2], Box[3], OriPt, &cScanPos);
								if (edgeType == 0)
								{
									//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
									int xx = X + i + inc_dir * EdgeFinderPixelRange;
								ShiftAndGetPt:
									int GotPt = ScanHorizontalAndInsertToList(&xx, Y + j * ScanDir, DetectedPoint);
									if (GotPt == 1)
									{
										if (DetectedPoint->X > 3 || DetectedPoint->Y > 3 || DetectedPoint->X < ImageWidth - 3 || DetectedPoint->Y < ImageHeight - 3)
											return 0;
										else
											return -1;
									}
									else if (GotPt == -3)
									{
										xx -= inc_dir; goto ShiftAndGetPt;
									}
								}
								else if (edgeType == 1)
									return 1;
							}
						}
						i += inc;
					}
				}
			}
			else
			{
				st = -windowHeight / 2; ed = windowHeight / 2;
				if (sd == Upwards)
				{
					inc_dir = -1;
					st = windowHeight / 2;
					ed = -(windowHeight / 2);
					inc = -EdgeFinderPixelRange;
				}

				for (int j = 0; j < windowWidth / 2; j++) //X scan
				{
					//for (int i = st; i < ed; i += inc) //Y Scan
					i = st;
					while (i != ed)
					{
						currentIndex = (Y + i) * ImageWidth + X + j * ScanDir;

						//Check for jump in color between current pixel and pixel edgefinder step away.
						if (abs(iBytes[currentIndex] - iBytes[currentIndex + EdgeFinderPixelRange * ImageWidth * inc_dir]) > Tolerance)
						{
							int cCt = 0;
							//We found a change. Now let us check that it is consistent for the next edgefinderRange number of pixels
							for (int k = 0; k <= EdgeFinderPixelRange; k++)
							{
								if (abs(iBytes[currentIndex] - iBytes[currentIndex + (EdgeFinderPixelRange + k) * ImageWidth * inc_dir]) > Tolerance)
									cCt++;
							}
							if (cCt > EdgeFinderPixelRange - 2) // we are allowed a max of two pixels of wrong color...
							{
								int EdgeLocation = CheckPointWithinCurvedBox2(X + j * ScanDir, Y + i, Box[4], Box[5], Box[2], Box[3], OriPt, &cScanPos);
								if (EdgeLocation == 0)
								{
									//We have an edge within one EdgeFinderRange. Lets take the point coordinates and return. 
									int yy = Y + i + EdgeFinderPixelRange * inc_dir;
								ShiftAndGetPtV:
									int GotPt = ScanVerticalAndInsertToList(X + j * ScanDir, &yy, DetectedPoint);
									if (GotPt == 1)
									{
										if (DetectedPoint->X > 3 || DetectedPoint->Y > 3 || DetectedPoint->X < ImageWidth - 3 || DetectedPoint->Y < ImageHeight - 3)
											return 0;
										else
											return 0;
									}
									else if (GotPt == -3)
									{
										yy -= inc_dir; goto ShiftAndGetPtV;
									}
								}
								else if (EdgeLocation == 1)
									return 1;
							}
						}
						i += inc;
					}
				}
			}
		}
		catch (...)
		{
		}
		return -1;
	}

	
	//This function checks if a point is within the angular rect or not and returns a bool value appropriately
	int EdgeDetectionSingleChannel::CheckPointWithinAngularRect(int X, int Y, double SinTh, double CosTh, Pt OriginPt, int Width, int Height, bool ApplyBoundryBuffer)
	{
		SetAndRaiseErrorMessage("EDCSC0024", "CheckPointWithinAngularRect", __FUNCSIG__);
		try
		{
			//transformation formula: X' = (X - OriginX)cos(a) - (Y - OriginY)sin(a)
			//						  Y' = (X - OriginX)sin(a) + (Y - OriginY)cos(a)
			//But here the direction of Y axis is reversed in pixel coordinates(Due to which, whatever angle we get, we reverse the sign, i.e. we make 'a' as '-a')
			//Also we always transform by an angle of (90 - a). Hence we get the following formula
			//double XTransformed = (X - OriginPt.X) * SinTh + (Y - OriginPt.Y) * CosTh;
			//double YTransformed = (OriginPt.X - X) * CosTh + (Y - OriginPt.Y) * SinTh;
			if (X < 5 || Y < 5) return -1;
			if (ImageWidth - X < 5 || ImageHeight - Y < 5) return -1;

			double XTransformed = (X - OriginPt.X) * CosTh + (Y - OriginPt.Y) * SinTh;
			double YTransformed = (X - OriginPt.X) * SinTh + (OriginPt.Y - Y) * CosTh;
			
			if (XTransformed < 0) return -1;
			if (XTransformed >= Height) return 1;
			if (YTransformed > Width / 2) return 2;
			if (YTransformed < -Width / 2) return -2;
			return 0;
			//int TmpBoundryBuffer = 0;
			//if(ApplyBoundryBuffer) TmpBoundryBuffer = BoundryBuffer;


			////if(XTransformed >= TmpBoundryBuffer && XTransformed < Width - TmpBoundryBuffer && YTransformed >= TmpBoundryBuffer && YTransformed < Height - TmpBoundryBuffer)
			//if(XTransformed > 0 && XTransformed < Height && abs(YTransformed) <= Width / 2)
			////if(XTransformed >= 0 && XTransformed < Width && YTransformed >=0 && YTransformed < Height)
			//	//If transformed points within bounds, return true
			//	return true;

			//return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0009", __FILE__, __FUNCSIG__); return false; }
	}
	
	int EdgeDetectionSingleChannel::CheckPointWithinAngularRect(int X, int Y, double SinTh, double CosTh, Pt OriginPt, int Width, int Height, struct Pt* transformedPt)
	{
		SetAndRaiseErrorMessage("EDCSC0025", "CheckPointWithinAngularRect", __FUNCSIG__);
		transformedPt->X = (X - OriginPt.X) * CosTh + (Y - OriginPt.Y) * SinTh;
		transformedPt->Y = (X - OriginPt.X) * SinTh + (OriginPt.Y - Y) * CosTh;

		if (transformedPt->X < 0) return -1;
		if (transformedPt->X >= Height) return 1;
		if (transformedPt->Y > Width / 2) return 2;
		if (transformedPt->Y < -Width / 2) return -2;
		return 0;
	}

	int EdgeDetectionSingleChannel::CheckPointWithinStraightRect(int X, int Y, int* Box)
	{
		SetAndRaiseErrorMessage("EDCSC0026", "CheckPointWithinStraightRect", __FUNCSIG__);
		if (X < Box[0] * this->CamSizeRatio) return -1;
		if (Y < Box[1] * this->CamSizeRatio) return 1;
		if (X > this->CamSizeRatio * (Box[0] + Box[2])) return 2;
		if (Y > this->CamSizeRatio * (Box[1] + Box[3])) return -2;
		return 0;
	}
	//This function scans for an edge point within a curved box in the specified direction
	bool EdgeDetectionSingleChannel::GetSingleEdgePointCurvedBox(double *Box, bool OppositeScan, struct Pt *DetectedPoint)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "GetSingleEdgePointCurvedBox", __FUNCSIG__);
		try
		{
			Pt OriginPoint, Point;
			int XIterator, YIterator;
			double pixels[4] = { 0 };
			int LookAhead = 10, LookAheadIndex, i;
			double var, R1, R2, Theta1, Theta2, Theta;
			ScanDirection Scan_Direction;
			int PixelArrayIndex;

			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];

			Theta = (Theta1 + Theta2) / 2;
			if (Theta > 2 * M_PI) Theta -= (2 * M_PI);

			if (Theta >= M_PI_4 && Theta < 3 * M_PI_4)
				if (!OppositeScan) Scan_Direction = Upwards;
				else Scan_Direction = Downwards;
			else if (Theta >= 3 * M_PI_4 && Theta < 5 * M_PI_4)
				if (!OppositeScan) Scan_Direction = Leftwards;
				else Scan_Direction = Rightwards;
			else if (Theta >= 5 * M_PI_4 && Theta < 7 * M_PI_4)
				if (!OppositeScan) Scan_Direction = Downwards;
				else Scan_Direction = Upwards;
			else
				if (!OppositeScan) Scan_Direction = Rightwards;
				else Scan_Direction = Leftwards;

				switch (Scan_Direction)
				{
				case Rightwards:

					XIterator = OriginPoint.X + (R1 + R2) / 2 * cos(Theta);
					YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin(Theta);
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) XIterator--;
					XIterator++;
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + 3)]) < Tolerance)
							goto last1;

						//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
						//for this purpose, first it must be determined till where this checking must be carried out
						if ((XIterator + LookAhead) < ImageWidth - BoundryBuffer)
							//the checking can continue for 'look ahead' number of pixels
							LookAheadIndex = LookAhead;
						else
							//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
							LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;

						//This is where the actual checking happens the range for which has been determined in the previous if statement
						for (i = 4; i <= LookAheadIndex; i++)
							if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
							{
								//its not a legitimate edge as there is no continuity of transition
								//hence jump to the point where the check failed and continue searching for edge from this point forward
								XIterator = XIterator + i - PixelSkipValue;
								goto last1;
							}
						//copy the 4 pixels where edge has been detected into 'pixels' array
						for (int j = 0; j <= 3; j++)
							pixels[j] = iBytes[PixelArrayIndex + j];

						//var = PinPointEdge(pixels);
						var = PinPointEdge(XIterator, YIterator, pixels);
						if (var == -1)
							goto last1;

						Point.X = XIterator + var;
						Point.Y = YIterator;
						*DetectedPoint = Point;
						return true;

					last1:			XIterator += 2;
					}
					break;

				case Leftwards:

					XIterator = OriginPoint.X + (R1 + R2) / 2 * cos((Theta1 + Theta2) / 2.0);
					YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin((Theta1 + Theta2) / 2.0);
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) XIterator++;
					XIterator--;
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - 3)]) < Tolerance)
							goto last2;

						if ((XIterator - LookAhead) >= BoundryBuffer)
							LookAheadIndex = LookAhead;
						else
							LookAheadIndex = XIterator - BoundryBuffer;

						for (i = 4; i <= LookAheadIndex; i++)
							if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]) < Tolerance)
							{
								XIterator = XIterator - i + PixelSkipValue;
								goto last2;
							}
						for (int j = 0; j <= 3; j++)
							pixels[j] = iBytes[PixelArrayIndex - j];

						//var = PinPointEdge(pixels);
						var = PinPointEdge(XIterator, YIterator, pixels);
						if (var == -1)
							goto last2;

						Point.X = XIterator - var;
						Point.Y = YIterator;
						*DetectedPoint = Point;
						return true;

					last2:			XIterator -= 2;
					}
					break;

				case Downwards:

					XIterator = OriginPoint.X + (R1 + R2) / 2 * cos((Theta1 + Theta2) / 2.0);
					YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin((Theta1 + Theta2) / 2.0);
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) YIterator--;
					YIterator++;
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if (abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + 3) * ImageWidth)]) < Tolerance)
							goto last3;
						if ((YIterator + LookAhead) < ImageHeight - BoundryBuffer)
							LookAheadIndex = LookAhead * ImageWidth;
						else
							LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;

						for (i = 4 * ImageWidth; i <= LookAheadIndex; i += ImageWidth)
							if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
							{
								YIterator = YIterator + i / ImageWidth - PixelSkipValue;
								goto last3;
							}
						for (int j = 0; j <= 3; j++)
							pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];

						//var = PinPointEdge(pixels);
						var = PinPointEdge(XIterator, YIterator, pixels);

						if (var == -1)
							goto last3;

						Point.X = XIterator;
						Point.Y = YIterator + var;
						*DetectedPoint = Point;
						return true;

					last3:			YIterator += 2;
					}
					break;

				case Upwards:

					XIterator = OriginPoint.X + (R1 + R2) / 2 * cos((Theta1 + Theta2) / 2.0);
					YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin((Theta1 + Theta2) / 2.0);
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) YIterator++;
					YIterator--;
					while (CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if (abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator - 3) * ImageWidth)]) < Tolerance)
							goto last4;

						if ((YIterator - LookAhead) >= BoundryBuffer)
							LookAheadIndex = LookAhead * ImageWidth;
						else
							LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;

						for (i = 4 * ImageWidth; i <= LookAheadIndex; i += ImageWidth)
							if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]) < Tolerance)
							{
								YIterator = YIterator - i / ImageWidth + PixelSkipValue;
								goto last4;
							}
						for (int j = 0; j <= 3; j++)
							pixels[j] = iBytes[(XIterator + (YIterator - j) * ImageWidth)];

						//var = PinPointEdge(pixels);
						var = PinPointEdge(XIterator, YIterator, pixels);
						if (var == -1)
							goto last4;

						Point.X = XIterator;
						Point.Y = YIterator - var;
						*DetectedPoint = Point;
						return true;

					last4:			YIterator -= 2;
					}
				}

				return false;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0010", __FILE__, __FUNCSIG__); return false; }
	}


	bool EdgeDetectionSingleChannel::GetSingleEdgePointCurvedBox_Modified(double *Box, bool OppositeScan, struct Pt *DetectedPoint, ScanDirection *Scan_Direction)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "GetSingleEdgePointCurvedBox_Modified", __FUNCSIG__);
		try
		{
			Pt OriginPoint, Point;
			int XIterator, YIterator;
			double pixels[40] = {0};
			int  LookAheadIndex, i;
			double var, R1, R2, Theta1, Theta2, Theta;
			//ScanDirection Scan_Direction;
			int PixelArrayIndex;
			
			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];
			
			Theta = (Theta1 + Theta2) / 2;
			if(Theta > 2 * M_PI) Theta -= (2 * M_PI);

			if(Theta >= M_PI_4 && Theta < 3 * M_PI_4)
				if(! OppositeScan) *Scan_Direction = Upwards;
				else *Scan_Direction = Downwards;
			else if(Theta >= 3 * M_PI_4 && Theta < 5 * M_PI_4)
				if(! OppositeScan) *Scan_Direction = Leftwards;
				else *Scan_Direction = Rightwards;
			else if(Theta >= 5 * M_PI_4 && Theta < 7 * M_PI_4)
				if(! OppositeScan) *Scan_Direction = Downwards;
				else *Scan_Direction = Upwards;
			else
				if(! OppositeScan) *Scan_Direction = Rightwards;
				else *Scan_Direction = Leftwards;

			switch(*Scan_Direction)
			{
			case Rightwards:

				XIterator = OriginPoint.X + (R1 + R2) / 2 * cos(Theta);
				YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin(Theta);
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) XIterator--;
				XIterator++;
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + EdgeFinderPixelRange)]) < Tolerance)
						goto last1;

					//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
					//for this purpose, first it must be determined till where this checking must be carried out
					if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
						//the checking can continue for 'look ahead' number of pixels
						LookAheadIndex = LookAheadLimit;
					else
						//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
						LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
					 
					//This is where the actual checking happens the range for which has been determined in the previous if statement
					for(i = EdgeFinderPixelRange; i <= LookAheadIndex; i++)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
						{
							//its not a legitimate edge as there is no continuity of transition
							//hence jump to the point where the check failed and continue searching for edge from this point forward
							XIterator = XIterator + i - PixelSkipValue; 
							goto last1;
						}
						//copy the 4 pixels where edge has been detected into 'pixels' array
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[PixelArrayIndex + j];
			
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);					
					if(var == -1) 
						goto last1;

					Point.X = XIterator + var;
					Point.Y = YIterator;
					*DetectedPoint = Point;
					return true;
					 
	last1:			XIterator +=2;
				}
				break;

			case Leftwards:
			
				XIterator = OriginPoint.X + (R1 + R2) / 2 * cos(Theta);// (Theta1 + Theta2) / 2.0);
				YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin(Theta); // (Theta1 + Theta2) / 2.0);
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) XIterator++;
				XIterator--;
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - EdgeFinderPixelRange)]) < Tolerance)
						goto last2;

					if((XIterator - LookAheadLimit) >= BoundryBuffer)
						LookAheadIndex = LookAheadLimit;
					else
						LookAheadIndex = XIterator - BoundryBuffer;
							
					for(i = 4; i <=  LookAheadIndex; i++)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]) < Tolerance)
						{
							XIterator = XIterator - i + PixelSkipValue;
							goto last2;
						}
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[PixelArrayIndex - j];
					
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
					if(var == -1) 
						goto last2;
							
					Point.X = XIterator - var;
					Point.Y = YIterator;
					*DetectedPoint = Point;
					return true;

	last2:			XIterator -= 2; 
				}
				break;

			case Downwards:

				XIterator = OriginPoint.X + (R1 + R2) / 2 * cos(Theta);// (Theta1 + Theta2) / 2.0);
				YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin(Theta);//(Theta1 + Theta2) / 2.0);
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) YIterator--;
				YIterator++;
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + EdgeFinderPixelRange) * ImageWidth)]) < Tolerance)
						goto last3;
					if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
						LookAheadIndex = LookAheadLimit * ImageWidth;
					else
						LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
							
					for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
						{
							YIterator = YIterator + i /ImageWidth - PixelSkipValue;
							goto last3;
						}
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
					
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);

					if(var == -1) 
						goto last3;
						  
					Point.X = XIterator;
					Point.Y = YIterator + var;
					*DetectedPoint = Point;
					return true;

	last3:			YIterator += 2;
				}
				break;

			case Upwards:

				XIterator = OriginPoint.X + (R1 + R2) / 2 * cos(Theta);//(Theta1 + Theta2) / 2.0);
				YIterator = OriginPoint.Y - (R1 + R2) / 2 * sin(Theta);//(Theta1 + Theta2) / 2.0);
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint)) YIterator++;
				YIterator--;
				while(CheckPointWithinCurvedBox(XIterator, YIterator, Theta1, Theta2, R1, R2, OriginPoint))
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator - EdgeFinderPixelRange) * ImageWidth)]) < Tolerance)
						goto last4;

					if((YIterator - LookAheadLimit) >= BoundryBuffer)
						LookAheadIndex = LookAheadLimit * ImageWidth;
					else
						LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;
						  
					for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]) < Tolerance)
						{
							YIterator = YIterator - i / ImageWidth + PixelSkipValue;
							goto last4;
						}
					for(int j = 0; j <= EdgeFinderPixelRange; j++)
						pixels[j] = iBytes[(XIterator + (YIterator - j) * ImageWidth)];
					
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
					if(var == -1) 
						goto last4;
						   
					Point.X = XIterator;
					Point.Y = YIterator - var;
					*DetectedPoint = Point;
					return true;

	last4:			YIterator -= 2; 
				}
			}

			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0010", __FILE__, __FUNCSIG__); return false; }
	}
	
	bool EdgeDetectionSingleChannel::GetSingleEdgePointCurvedBox2(double *Box, int Width, int Height, bool OppositeScan, struct Pt *DetectedPoint, int ScanDir)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "GetSingleEdgePointCurvedBox2", __FUNCSIG__);
		double* pixels = new double[40];
		bool answer = false;
		try
		{
			Pt OriginPoint;// , Point;
			//int XIterator, YIterator;
			ZeroMemory(pixels, 40 * sizeof(double));
			//int   i; //LookAheadIndex,
			double R1, R2, Theta1, Theta2, Theta;//var,
			//ScanDirection Scan_Direction;
			//int PixelArrayIndex;

			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];
			ScanDirection sdir = Rightwards;
			//Theta = (Theta1 + Theta2) / 2;
			Theta = Theta1 + 10 * 2 / (R1 + R2);



			//if (Theta > 2 * M_PI) Theta -= (2 * M_PI);

			//if (Theta >= M_PI_4 && Theta < 3 * M_PI_4)
			//	if (!OppositeScan) sdir = Upwards;
			//	else sdir = Downwards;
			//else if (Theta >= 3 * M_PI_4 && Theta < 5 * M_PI_4)
			//	if (!OppositeScan) sdir = Leftwards;
			//	else sdir = Rightwards;
			//else if (Theta >= 5 * M_PI_4 && Theta < 7 * M_PI_4)
			//	if (!OppositeScan) sdir = Downwards;
			//	else sdir = Upwards;
			//else
			//{
			//	if (!OppositeScan) sdir = Rightwards;
			//	else sdir = Leftwards;
			//}
			int GotPt = -1000;
			int ScanDirMult = 1;
			double Rad = Box[2] + Width / 2;
			double Theta_Inc = 1 / Rad;
			//int X = OriginPoint.X + Rad * cos(Theta);
			//int Y = OriginPoint.Y + Rad * sin(Theta);
			//int ScanBox[] = { X, Y, Width, Width };

			//switch (sdir)
			//{
			//case Rightwards:
			//case Leftwards:
			//	//First let us fix the Reference pt that will be used for Checking direction of travel and ensure that there is no return in the same region
			//	//EdgeTravelReferencePt. = (StPt.X + EndPt.X) / 2 - 1000; EdgeTravelReferencePt.y = 

			//	//X -= Width / 2;
				//for (int j = 0; j < 2; j++)


			//while (Rad > R1 && Rad < R2)
			//{
			//	for (int i = 0; i < Width / 2; i++)
			//	{
			//		//GotPt = GetEdgePtNearGivenPtStraightRect_H(X + i * ScanDirMult, Y, sdir, Width, Width, DetectedPoint, ScanBox, ScanDir);
			//		//Theta += 1 / Rad;
			//		GotPt = GetEdgePtNearGivenPtCurvedBox(Rad, Theta + i * Theta_Inc, Width, Width, DetectedPoint, Box, 1, OppositeScan);
			//		if (GotPt == 0)  return true; // break;
			//		//else GotPt = GetEdgePtNearGivenPtStraightRect_H(X + i * ScanDirMult, Y, sdir, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, -1 * DirSign);
			//		//if (GotPt) break;
			//	}
			//	Rad += Width;
			//}

			for (double ct = Theta + Theta_Inc; ct < Theta2 - Theta_Inc; ct += Theta_Inc)
			{
				for (Rad = R1 + Width / 2; Rad < R2 - Width / 2; Rad += Width)
				{
					GotPt = GetEdgePtNearGivenPtCurvedBox(Rad, ct, Width, Width, DetectedPoint, Box, 1, OppositeScan);
					if (GotPt == 0)
					{
						answer = true; break;
					}
				}
			}
			//return GotPt;
		//	break;

		//case Upwards:

		//case Downwards:
		//	for (int j = 0; j < 2; j++)
		//	{
		//		for (int i = 0; i < Width / 2; i++)
		//		{
		//			GotPt = GetEdgePtNearGivenPtStraightRect_V(X, Y + i * ScanDirMult, sdir, Width, Width, DetectedPoint, ScanBox, ScanDir);
		//			if (GotPt) return true;
		//			//else GotPt = GetEdgePtNearGivenPtAngRect_V(X, Y + i * ScanDirMult, *Scan_Direction, Width, Width, SinTh, CosTh, OriginPoint, DetectedPoint, Width, Height, -1 * DirSign);
		//			//if (GotPt) break;
		//		}
		//		ScanDirMult *= -1;
		//	}

			//return GotPt;
			//break;

			//return false;
		//}
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0010", __FILE__, __FUNCSIG__); answer = false; } // return false;
		delete pixels;
		return answer;
	}

	//This function checks if the given coordinate values are within the curved box confines
	bool EdgeDetectionSingleChannel::CheckPointWithinCurvedBox(int X, int Y, double Theta1, double Theta2, double R1, double R2, Pt OriginPoint, bool ApplyBoundryBuffer)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double Distance = sqrt(pow((OriginPoint.X - X),2)+pow(OriginPoint.Y - Y,2));
			double Theta = acos((double)(X - OriginPoint.X) / Distance);
			int TmpBoundryBuffer = 0;
			if(ApplyBoundryBuffer) TmpBoundryBuffer = BoundryBuffer;

			if(Y > OriginPoint.Y) Theta = 2 * M_PI - Theta;

			if(Distance >= (R1 + TmpBoundryBuffer) && Distance < (R2 - TmpBoundryBuffer))
			{
				if(Theta > (Theta1) && Theta < Theta2)
					return true;

				if(Theta2 > 2 * M_PI)
				{
					if(Theta < (Theta2 - 2 * M_PI)) return true;
					else return false;
				}
			}

			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0011", __FILE__, __FUNCSIG__); return false; }
	}
	
	int EdgeDetectionSingleChannel::CheckPointWithinCurvedBox2(int X, int Y, double Theta1, double Theta2, double R1, double R2, Pt OriginPoint, Pt* CurrScanPos)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double Distance = sqrt(pow((OriginPoint.X - X), 2) + pow(OriginPoint.Y - Y, 2));
			double Theta = acos((double)(X - OriginPoint.X) / Distance);
			CurrScanPos->X = Distance; 

			//if (Y > OriginPoint.Y) Theta = 2 * M_PI - Theta;
			//double Theta = 0;
			if ((int)OriginPoint.X == X)
			{
				if (Y < (int)OriginPoint.Y)
					Theta = M_PI_2;
				else
					Theta = M_PI_2 * 3;
			}
			else
			{
				Theta = atan((Y - OriginPoint.Y) / (OriginPoint.X - X));
				if (X < (int)OriginPoint.X && Y <= (int)OriginPoint.Y)
					Theta += M_PI;
				else if (X < (int)OriginPoint.X && Y >= (int)OriginPoint.Y)
					Theta += M_PI;
				else if (X > (int)OriginPoint.X && Y >= (int)OriginPoint.Y)
					Theta += 2 * M_PI;
			}
			if (Theta2 > 2 * M_PI && Theta > 0 && Theta < M_PI_2)
				Theta += 2 * M_PI;

			CurrScanPos->Y = Theta;
			//if (Theta2 > 2 * M_PI)
			//{
			//	if (Theta > (Theta2 - 2 * M_PI)) return 1;
			//}
			if (Distance > R2) return 2;
			if (Distance < R1) return -2;
			if (Theta > Theta2) return 1;
			if (Theta < Theta1) return -1;
			return 0;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0011a", __FILE__, __FUNCSIG__); return -10; }

	}


	int EdgeDetectionSingleChannel::GetScanDirectionforCurvedBox(int X, int Y, Pt Centre, bool OppositeScan)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		bool DoVerticalScan = false;
		double Theta = 0;
		if (Centre.X == X)
		{
			if (Y < Centre.Y)
				Theta = M_PI_2;
			else
				Theta = M_PI_2 * 3;
		}
		else
		{
			Theta = atan((Y - Centre.Y) / (Centre.X - X));
			if (X < Centre.X && Y <= Centre.Y)
				Theta += M_PI;
			else if (X < Centre.X && Y >= Centre.Y)
				Theta += M_PI;
			else if (X > Centre.X && Y >= Centre.Y)
				Theta += 2 * M_PI;
		}

		if (Theta > 2 * M_PI) Theta -= (2 * M_PI);

		int Sdir = 0;

		if (Theta >= M_PI_4 && Theta < 3 * M_PI_4)
			if (!OppositeScan) Sdir = 3;
			else Sdir = 1;
		else if (Theta >= 3 * M_PI_4 && Theta < 5 * M_PI_4)
			if (!OppositeScan) Sdir = 2;
			else Sdir = 0;
		else if (Theta >= 5 * M_PI_4 && Theta < 7 * M_PI_4)
			if (!OppositeScan) Sdir = 1;
			else Sdir = 3;
		else
		{
			if (!OppositeScan) Sdir = 0;
			else Sdir = 2;
		}

		return Sdir;
		//if (Theta < M_PI_4 || Theta > 7 * M_PI_4 || (Theta > 3 * M_PI_4 && Theta < 5 * M_PI_4))
		//	if (OppositeScan) return 2; else return 0;
		//	//return true; //We need to do horizontal scan
		//else
		//	if (OppositeScan) return 3; else return 1;
		//return false; // we need to do vertical scan
	}



	//-----------------------------------------------------------------------
	//The traverse edge function that calls one of the 3 traverse edge function according to the "BoxType" parameter passed
	//-----------------------------------------------------------------------

	//..22This function is used to call the appropriate traverse edge function according to the type of box
	bool EdgeDetectionSingleChannel::TraverseEdge(TypeOfBox BoxType, void *Box, std::list<struct Pt> *PointsList)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			switch(BoxType)
			{
			case StraightRect:
				return TraverseEdgeStraightRect((int *)Box, PointsList, ScanDirection::Rightwards);
			case AngularRect:
				return TraverseEdgeAngularRect((int *)Box, PointsList, ScanDirection::Rightwards);			
			case CurvedBox:
				return TraverseEdgeCurvedBox((double *)Box, PointsList); // , ScanDirection::Rightwards );
			}
			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0012", __FILE__, __FUNCSIG__); return false; }
	}
	


	//----------------------------------------------------------------------------------
	//The 3 traverse edge functions which when passed with a single edge point traverse along the edge in straight rect, angular rect and curved box respectively.
	//----------------------------------------------------------------------------------

	//This function receives a single point(which is already in the Points list), then using this point, it traverses
	//along the edge, while adding detected edge points into the list in the process. The boundry is a straight rect
	bool EdgeDetectionSingleChannel::TraverseEdgeStraightRect(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int X, Y, XPrevious, YPrevious, X_Break, Y_Break;
			Pt PrvToPrvPt, PtAtOtherEnd;
			bool TraverseDirection = true, EdgeLoopBack = false;
			bool ScanInPrevLine = true, DoHorizontalScan = true, TryBothLines = false, DoVerticalScan = false;

			int LeftBoundry = Box[0] + BoundryBuffer;									//set boundaries
			int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
			int TopBoundry = Box[1] + BoundryBuffer;
			int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
			
			X = PointsList->begin()->X + 0.5;
			Y = PointsList->begin()->Y + 0.5;
			XPrevious = 0;
			YPrevious = 0;
			PrvToPrvPt.X = 0;
			PrvToPrvPt.Y = 0;
			PtAtOtherEnd.X = X;
			PtAtOtherEnd.Y = Y;
			NoEdgePixelCount = 0;

			int sd = 1;

			if (SDir == Rightwards || SDir == Leftwards)
			{
				if (SDir == Leftwards) sd = -1;
				if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else return false;
			}
			else
			{
				if (SDir == Upwards) sd = -1;

				if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else
				{
					if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
						SDir = Rightwards;
					else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
						SDir = Leftwards;
					else 
						return false;
				}
			}

			int MaxPtsPossible = ImageWidth * ImageHeight / EdgeFinderPixelRange; //We will consume a minimum of Edge thickness number of pixels for one edge pt

			for(int DirectionCount = 0; DirectionCount < 2; DirectionCount++)
			{
				int PointsCount = 0;
				while(X <= RightBoundry && X >= LeftBoundry && Y <= BottomBoundry && Y >= TopBoundry)
				{
					PointsCount++;
					if(PointsCount > MaxPtsPossible) 
						break;

					if (SDir == Rightwards || SDir == Leftwards)
					{
						DoHorizontalScan = true; DoVerticalScan = false;
						//if (YPrevious == Y) TryBothLines = true;
						if (YPrevious <= Y) ScanInPrevLine = false; else ScanInPrevLine = true;
					}
					else
					{
						DoHorizontalScan = false; DoVerticalScan = true;
						//if (XPrevious == X) TryBothLines = true;
						if (XPrevious <= X) ScanInPrevLine = false; else ScanInPrevLine = true;
					}
					
					if (DoHorizontalScan)
					{
						if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, ScanInPrevLine, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
							continue;
						if (TryBothLines)
							if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, !ScanInPrevLine, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						//Horizontal Scan has failed. Let us try the vertical scan now
						if (NoEdgePixelCount < 2 * EdgeFinderPixelRange + 1)
						{
							if (NoEdgePixelCount == 1)
							{
								X_Break = X; Y_Break = Y;
							}
							else
								continue; //While.
						}
						else
						{
							DoVerticalScan = true;
							X = X_Break; Y = Y_Break - 1;
							//continue;
						}
					}
					if (EdgeLoopBack) return true;

					if(DoVerticalScan)
					{
						if (SDir == Rightwards || SDir == Leftwards) DoVerticalScan = false;

						if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, ScanInPrevLine, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
							continue;
						if (TryBothLines)
							if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, !ScanInPrevLine, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						if (EdgeLoopBack) return true;

						//Vertical Scan Failed. Let us try horizontal Scan now
						if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, ScanInPrevLine, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
							continue;
						if (TryBothLines)
							if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, !ScanInPrevLine, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
					}
					if (EdgeLoopBack) return true;

					//if(YPrevious == Y)
					//{
					//	if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//		continue;
					//	else if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//		continue;
					//}
					//else
					//{
					//	if(YPrevious < Y) // == Y - 1)
					//	{
					//		if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//			continue;
					//	}
					//	else if(YPrevious > Y) // == Y + 1)
					//	{
					//		if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//			continue;
					//	}
					//}
					//if (EdgeLoopBack) break; // return true;

					//if(XPrevious == X)
					//{
					//	if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//		continue;
					//	else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//		continue;
					//}
					//else
					//{
					//	if(XPrevious < X) // == X - 1)
					//	{
					//		if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//			continue;
					//	}
					//	else if(XPrevious > X) // == X + 1)
					//	{
					//		if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
					//			continue;
					//	}
					//}
					//if (EdgeLoopBack) break; // return true;
					if (SDir == Rightwards || SDir == Leftwards)
					{

					}

					//Even if we did not get an edge, we continue the scan on the next row/column until we have a contiguous set of 2 * EdgeRange number of pixels. Then we call dust skip
					if (NoEdgePixelCount < 2 * EdgeFinderPixelRange)
						continue;

					Pt Pnt1, Pnt2;
					std::list<Pt>::iterator Itr;
					if(PointsList->size() >= 15)
					{
						if(TraverseDirection)
						{
							Itr = PointsList->end();
							for(int j = 1; j<= 15; j++) Itr--;
							
							if(!DustSkip(*(--(PointsList->end())), *(Itr), &Pnt1, &Pnt2)) break;
						}
						else
						{
							Itr = PointsList->begin();
							for(int j = 1; j<= 14; j++) Itr++;
							
							if(!DustSkip(*(PointsList->begin()), *(Itr), &Pnt1, &Pnt2)) break;
						}
					}
					else
					{
						if(TraverseDirection)
						{
							if(!DustSkip(*(--(PointsList->end())), *(PointsList->begin()), &Pnt1, &Pnt2)) break;
						}
						else
						{
							if(!DustSkip(*(PointsList->begin()), *(--(PointsList->end())), &Pnt1, &Pnt2)) break;
						}
					}
					
					if(Pnt1.X > RightBoundry || Pnt1.X < LeftBoundry || Pnt1.Y > BottomBoundry || Pnt1.Y < TopBoundry) break;
					if(Pnt2.X > RightBoundry || Pnt2.X < LeftBoundry || Pnt2.Y > BottomBoundry || Pnt2.Y < TopBoundry) break;
					
					if(TraverseDirection)
					{
						PointsList->push_back(Pnt1);
						PointsList->push_back(Pnt2);
					}
					else
					{
						PointsList->push_front(Pnt1);
						PointsList->push_front(Pnt2);
					}
					NoEdgePixelCount = 0;

					X = Pnt2.X + 0.5;
					Y = Pnt2.Y + 0.5;
					XPrevious = Pnt1.X + 0.5;
					YPrevious = Pnt1.Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}

				if(DirectionCount == 0)
				{
					TraverseDirection = false;
					EdgeLoopBack = false;
					PtAtOtherEnd.X = (--(PointsList->end()))->X + 0.5;
					PtAtOtherEnd.Y = (--(PointsList->end()))->Y + 0.5; 
					NoEdgePixelCount = 0;

					X = PointsList->begin()->X + 0.5;
					Y = PointsList->begin()->Y + 0.5;
					XPrevious = (++(PointsList->begin()))->X + 0.5;
					YPrevious = (++(PointsList->begin()))->Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}
			}
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0013", __FILE__, __FUNCSIG__); return false; }
	}

	bool EdgeDetectionSingleChannel::TraverseEdgeStraightRect2(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int X, Y, X_break, Y_Break;
			Pt PtAtOtherEnd, CurrentEdgePt, P1;
			bool EdgeLoopBack = false;
			bool DoHorizontalScan = true, DoVerticalScan = false;

			X = PointsList->begin()->X + 0.5;
			Y = PointsList->begin()->Y + 0.5;
			PtAtOtherEnd.X = X;
			PtAtOtherEnd.Y = Y;
			P1.X = X; P1.Y = Y;
			NoEdgePixelCount = 0;

			int Left = Box[0];
			int Right = Box[0] + Box[2];
			int Top = Box[1];
			int Bottom = Box[1] + Box[3];
			int ShiftCounter = 0, EdgeDetectedType = 0, PtCounter = 0;

			if (SDir == Rightwards || SDir == Leftwards)
			{
				DoHorizontalScan = true; DoVerticalScan = false;
			}
			else
			{
				DoHorizontalScan = false; DoVerticalScan = true;
			}

			while (X >= Left && X < Right && Y >= Top && Y < Bottom)
			{
				while (DoHorizontalScan)
				{
					EdgeDetectedType = ScanHorizontalAndInsertToList(&X, Y, &CurrentEdgePt); //, &EdgeLoopBack, PtAtOtherEnd);

					if (X < Left || X >= Right || Y < Top || Y >= Bottom) break;

					if (EdgeDetectedType == 1)
					{
						if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
							//We got back to starting edge point.
							return true;
						//We got an edgepoint. Lets add it
						PointsList->push_back(CurrentEdgePt);
						PtCounter++;
						//Now we check slope of edge. If current pt and 10th point before this pt give a slope less than 45 deg, we switch to vertical scan.
						if (PtCounter >= 10)
						{
							if (abs(P1.Y - CurrentEdgePt.Y) < abs(P1.X - CurrentEdgePt.X))
							{
								DoVerticalScan = true; DoHorizontalScan = false;
								Y = Y - EdgeFinderPixelRange;
							}
							P1 = CurrentEdgePt; PtCounter = 0;
						}
						if (P1.Y > CurrentEdgePt.Y)
							Y--;
						else
							Y++;
						//Y++; //Got an edge. Go to the next row
					}
					else if (EdgeDetectedType == -2)
					{
						//We got an island pixel row. So just skip this and move to next row
						if (P1.Y > CurrentEdgePt.Y)
							Y--;
						else
							Y++; 
						DoHorizontalScan = true;
					}
					else if (EdgeDetectedType == -10)
					{
						// Some error happened. So let us quit
						return true;
					}
					else
					{
						//Let us now search in the vicinity
						if (NoEdgePixelCount > 0 && NoEdgePixelCount <= 2 * EdgeFinderPixelRange)
						{
							int shift_dir = -1;
							if (NoEdgePixelCount == 1) //First miss. Note the break points
							{
								X_break = X; Y_Break = Y;
							}
								// We have tried a number of times in one direction. So go to the other side of the starting position
							if (ShiftCounter >= EdgeFinderPixelRange) shift_dir = 1;
							
							ShiftCounter++;
							//Shift by one range. 
							int xx = X + shift_dir * EdgeFinderPixelRange;
							//Check if we are within scanning area
							if (xx >= Left && xx < Right)
							{
								//Continue scan in same direction...
								DoHorizontalScan = true;
								X = xx;
							}
						}
						else
						{	
							Y = Y_Break - EdgeFinderPixelRange; NoEdgePixelCount = 0;
							X = CurrentEdgePt.X + 0.5; Y = CurrentEdgePt.Y + 0.5;
							DoVerticalScan = true; DoHorizontalScan = false;
						}
					}
				}
				while (DoVerticalScan)
				{
					EdgeDetectedType = ScanVerticalAndInsertToList(X, &Y, &CurrentEdgePt); //, &EdgeLoopBack, PtAtOtherEnd);
					
					if (X < Left || X >= Right || Y < Top || Y >= Bottom) break;

					if (EdgeDetectedType == 1)
					{
						if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
							return false;
						PointsList->push_back(CurrentEdgePt);
						PtCounter++;
						//Now we check slope of edge. If current pt and 10th point before this pt give a slope less than 45 deg, we switch to vertical scan.
						if (PtCounter >= 10)
						{
							if (abs(P1.Y - CurrentEdgePt.Y) > abs(P1.X - CurrentEdgePt.X))
							{
								DoHorizontalScan = true; DoVerticalScan = false;
								X = X - EdgeFinderPixelRange;
							}
							P1 = CurrentEdgePt; PtCounter = 0;
						}
						if (P1.X > CurrentEdgePt.X)
							X--;
						else
							X++;

					}
					else if (EdgeDetectedType == -2)
					{
						if (P1.X > CurrentEdgePt.X)
							X--;
						else
							X++;
						DoVerticalScan = true;
					}
					else if (EdgeDetectedType == -10)
					{
						//Some error happened. So break everything and quit
						return true;
					}
					else
					{
						if (NoEdgePixelCount > 0 && NoEdgePixelCount <= 2 * EdgeFinderPixelRange)
						{
							int shift_dir = -1;
							if (NoEdgePixelCount == 1)
							{
								X_break = X; Y_Break = Y;
							}
							if (ShiftCounter >= EdgeFinderPixelRange) shift_dir = 1;
							
							ShiftCounter++;
							int yy = Y + shift_dir * EdgeFinderPixelRange;

							if (yy >= Top && yy < Bottom)
							{
								DoVerticalScan = true;
								Y = yy;
							}
						}
						else
						{
							X = X_break - EdgeFinderPixelRange; NoEdgePixelCount = 0;
							X = CurrentEdgePt.X + 0.5; Y = CurrentEdgePt.Y + 0.5;
							DoHorizontalScan = true; DoVerticalScan = false;
						}
					}
				}
			}

			//if (DoHorizontalScan)
			//{
			//	//if (SDir)
			//	for (int yy = Y + 1; yy < Box[3]; yy++)
			//	{
			//		if (!ScanHorizontalAndInsertToList(&X, yy, PointsList, &EdgeLoopBack, PtAtOtherEnd))
			//		{//We did not get a point in horizontal scan. So let us now try a vertical scan
			//			ScanVerticalAndInsertToList(X, &yy, PointsList, &EdgeLoopBack, PtAtOtherEnd);
			//			//If we still do not get a point, we just move on to the next column
			//		}
			//	}
			//	DoVerticalScan = true;
			//}

			//if (DoVerticalScan)
			//{
			//	for (int xx = X + 1; xx < Box[2]; xx++)
			//	{
			//		if (!ScanVerticalAndInsertToList(xx, &Y, PointsList, &EdgeLoopBack, PtAtOtherEnd))
			//		{//We did not get a point in Vertical scan. So let us now try a horizontal scan
			//			ScanHorizontalAndInsertToList(&xx, Y, PointsList, &EdgeLoopBack, PtAtOtherEnd);
			//			//If we still do not get point, we just move on to the next row
			//		}
			//	}
			//}
			return true;
		}
		catch (...)
		{
			SetAndRaiseErrorMessage("EDSC0013a", __FILE__, __FUNCSIG__); return false;
		}

	}

	bool EdgeDetectionSingleChannel::TraverseEdgeStraightRect3(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir, bool ScanOpposite)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int X, Y, EdgeDetectedType = 0, PtCounter = 0, EdgePtPositionFlag = 0, BoxLength = 0; //X_break, Y_Break,
			Pt PtAtOtherEnd, CurrentEdgePt, P1, PrevEdgePt;
			bool EdgeLoopBack = false, FailedInOtherDirection = false;
			bool DoHorizontalScan = true, DoVerticalScan = false, DefaultVerticalScan = false, ScanOrientationSwitched = false;
			int horizontal_ScanDir = 1, Vert_ScanDir = 1, currScanDir = 0, tempScanDir = 0;

			X = PointsList->begin()->X;
			Y = PointsList->begin()->Y;
			PtAtOtherEnd.X = X; P1.X = X;
			PtAtOtherEnd.Y = Y; P1.Y = Y;
			//if (ScanOpposite)
			//{
			//	std::list<Pt>::iterator Itr = PointsList->end();
			//	PtAtOtherEnd.X = PointsList->end()->X;
			//	PtAtOtherEnd.Y = PointsList->end()->Y;
			//}
			CurrentEdgePt.X = X; CurrentEdgePt.Y = Y;
			PrevEdgePt.X = X; PrevEdgePt.Y = Y;
			P1.X = X; P1.Y = Y;
			NoEdgePixelCount = 0;
			int stP = 0, eP = 0;
		
			if (SDir == Upwards) Vert_ScanDir = -1;
			if (SDir == Leftwards) horizontal_ScanDir = -1;

			if (SDir == Downwards || SDir == Upwards)
			{ 
				if (ScanOpposite) horizontal_ScanDir = -1;
				//We start from the Left of the box and travel to the right
				DefaultVerticalScan = true;
				DoVerticalScan = true; DoHorizontalScan = false;
				BoxLength = Box[2] * 2 * this->CamSizeRatio;
				X+= horizontal_ScanDir; //Shift by 1 pixel to start scan Since we always take stPt as the smaller X, a vertical scan means we will start from the left end of the box
				stP = Y; 
			}
			else
			{
				if (ScanOpposite) Vert_ScanDir = -1;
				//We travel from the top of the box to the bottom
				Y += Vert_ScanDir; //Shift by 1 pixel up or down to start scan...
				DoVerticalScan = false; DoHorizontalScan = true;
				BoxLength = Box[3] * 2 * this->CamSizeRatio;
				stP = X; 
			}

			//Ensure we are within the box + plus get transformed coordinate to start scan correctly....
			if (CheckPointWithinStraightRect(X, Y, Box) != 0) return false;
			bool TakeThisPt = false;	
			
			//We go uniformly from starting point to end point. 
			//while (CheckPointWithinStraightRect(X, Y, Box) == 0)
			for (int currPos = 0; currPos < BoxLength; currPos++)
			{
				if (DoHorizontalScan)
				{
					if (GetEdgePtNearGivenPtStraightRect_H(X, Y, SDir, 20, 20, &CurrentEdgePt, Box, Vert_ScanDir))
					{
CheckAndAddPt:
						if (CheckPointWithinStraightRect(CurrentEdgePt.X, CurrentEdgePt.Y, Box) == 0)
						{
							if (PointsList->size() == 1 || ScanOrientationSwitched)
							{

								tempScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
								if (tempScanDir != 0) currScanDir = tempScanDir;
								TakeThisPt = true; ScanOrientationSwitched = false;
							}

							//Check if we are back to first scanned pixel! We got back to starting edge point.
							if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
								return true;

							if (CurrentEdgePt.X > ImageWidth - 10) 
								return true;
							if (CurrentEdgePt.Y > ImageHeight - 10) 
								return true;

							//Check for same sign of scan direction.. 
							tempScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
							horizontal_ScanDir = CurrentEdgePt.X - PrevEdgePt.X;
							if (horizontal_ScanDir == 0) 
							{
								Pt lastPt, prevToPrevPt;
								std::list<Pt>::iterator Itr;
								Itr = PointsList->end(); 
								if (PointsList->size() < 2) horizontal_ScanDir = 1;
								else
								{
									while (horizontal_ScanDir == 0) // || Itr == PointsList->begin())
									{
										Itr--; lastPt = *Itr;
										if (Itr == PointsList->begin()) break;
										Itr--; prevToPrevPt = *Itr;
										horizontal_ScanDir = 1; 
										if (lastPt.X < prevToPrevPt.X) horizontal_ScanDir = -1;
										if (Itr == PointsList->begin()) break;
									}
									if (horizontal_ScanDir == 0) horizontal_ScanDir = 1;
								}
							}
							horizontal_ScanDir = horizontal_ScanDir / abs(horizontal_ScanDir);

							if ((tempScanDir ^ currScanDir) >= 0)
							{
								//We are travelling in correct direction. Let us take the point...
								TakeThisPt = true;
							}
							if (tempScanDir == 0) TakeThisPt = false;

							if (TakeThisPt)
							{
								PointsList->push_back(CurrentEdgePt);
								PtCounter++;
								if (PtCounter >= 15)
								{
									if (abs(P1.Y - CurrentEdgePt.Y) < abs(P1.X - CurrentEdgePt.X))
									{
										DoVerticalScan = true; DoHorizontalScan = false; ScanOrientationSwitched = true;
										Y = Y - EdgeFinderPixelRange;
									}
									P1 = CurrentEdgePt; PtCounter = 0;
								}
								//Jump to the new edge position and increment Y in correct direction for next scan
								Y = CurrentEdgePt.Y + Vert_ScanDir; X = CurrentEdgePt.X + horizontal_ScanDir;
								PrevEdgePt = CurrentEdgePt;
							}
							else
							{
								Y += Vert_ScanDir;
							}
						}
						else
						{
							if (DoVerticalScan)
								X += horizontal_ScanDir;
							else
								Y += Vert_ScanDir;
							//return true;
						}
					}
					else
					{
						if (ScanOrientationSwitched)
						{
							if (Y < this->CamSizeRatio * (Box[1] + Box[3]) - BoundryBuffer)
								Y += Vert_ScanDir;
							else
							{
								if (DoVerticalScan)
									X += horizontal_ScanDir;
								else
									Y += Vert_ScanDir;
								//	return true;
							}
						}
						else if (!DefaultVerticalScan)
						{
							int bb[4] = { X / 2 - 20, (Y + 1 - BoundryBuffer) / 2, 80, 20 };
							if (GetSingleEdgePointStraightRect(bb, SDir, &CurrentEdgePt))
							{
								goto CheckAndAddPt;
							}
							else
							{
								DoVerticalScan = true; DoHorizontalScan = false;
								ScanOrientationSwitched = true;
							}
						}
						else
						{	
							if (DoVerticalScan)
								X += horizontal_ScanDir;
							else
								Y += Vert_ScanDir;

							
							//switch (SDir)
							//{
							//case EdgeDetectionSingleChannel::Rightwards:
							//case EdgeDetectionSingleChannel::Leftwards:
							//	Y += Vert_ScanDir;
							//	break;
							//case EdgeDetectionSingleChannel::Downwards:
							//case EdgeDetectionSingleChannel::Upwards:
							//	X += horizontal_ScanDir;
							//	break;
							//default:
							//	break;
							//}
						}

					}
				}

				if (DoVerticalScan)
				{
					if (GetEdgePtNearGivenPtStraightRect_V(X, Y, SDir, 20, 20, &CurrentEdgePt, Box, horizontal_ScanDir))
					{
						if (CheckPointWithinStraightRect(CurrentEdgePt.X, CurrentEdgePt.Y, Box) == 0)
						{
							if (PointsList->size() == 1 || ScanOrientationSwitched)
							{
								tempScanDir = CurrentEdgePt.X - PrevEdgePt.X;
								if (tempScanDir != 0) currScanDir = tempScanDir;
								TakeThisPt = true; ScanOrientationSwitched = false;
							}
							//Check if we are back to first scanned pixel! We got back to starting edge point.
							if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
								return true;

							if (CurrentEdgePt.X > ImageWidth - 10) return true;
							if (CurrentEdgePt.Y > ImageHeight - 10) return true;

							//Check for same sign of scan direction.. 
							tempScanDir = CurrentEdgePt.X - PrevEdgePt.X;
							Vert_ScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
							if (Vert_ScanDir == 0) 
							{
								Pt lastPt, prevToPrevPt;
								std::list<Pt>::iterator Itr;
								Itr = PointsList->end();
								if (PointsList->size() < 2) Vert_ScanDir = 1;
								else
								{
									while (Vert_ScanDir == 0)
									{
										Itr--; lastPt = *Itr;
										if (Itr == PointsList->begin()) break;
										Itr--; prevToPrevPt = *Itr;
										Vert_ScanDir = 1;
										if (lastPt.Y < prevToPrevPt.Y) Vert_ScanDir = -1;
										if (Itr == PointsList->begin()) break;
									}
								}
							}
							Vert_ScanDir = Vert_ScanDir / abs(Vert_ScanDir);
							
							if ((tempScanDir ^ currScanDir) >= 0)
							{
								//We are travelling in correct direction. Let us take the point...
								TakeThisPt = true;
							}
							if (tempScanDir == 0) TakeThisPt = false;

							if (TakeThisPt)
							{
								PointsList->push_back(CurrentEdgePt);
								PtCounter++;
								if (PtCounter >= 15)
								{
									if (abs(P1.Y - CurrentEdgePt.Y) > abs(P1.X - CurrentEdgePt.X))
									{
										DoVerticalScan = false; DoHorizontalScan = true;
										X = X - EdgeFinderPixelRange;
									}
									P1 = CurrentEdgePt; PtCounter = 0;
								}
								//Jump to the new edge position and increment Y in correct direction for next scan
								Y = CurrentEdgePt.Y; X = CurrentEdgePt.X + horizontal_ScanDir;
								PrevEdgePt = CurrentEdgePt;
							}
							else
							{
								X += horizontal_ScanDir;
							}
						}
						else
						{
							if (DoVerticalScan)
								X += horizontal_ScanDir;
							else
								Y += Vert_ScanDir;
							if (CheckPointWithinStraightRect(X, Y, Box) != 0) return true;
							//return true;
						}
					}
					else
					{
						if (ScanOrientationSwitched)
						{
							if (X > this->CamSizeRatio * Box[0] && X < this->CamSizeRatio * (Box[0] + Box[2]) + BoundryBuffer)
								X += horizontal_ScanDir;
							else
							{
								if (DoVerticalScan)
									X += horizontal_ScanDir;
								else
									Y += Vert_ScanDir;
								if (CheckPointWithinStraightRect(X, Y, Box) != 0) return true;
								//return true;
							}
						}
						else if (!DefaultVerticalScan)
						{
							int bb[4] = { (X + horizontal_ScanDir - BoundryBuffer) / 2, Y + 20 * Vert_ScanDir, 20, 80 };
							if (GetSingleEdgePointStraightRect(bb, SDir, &CurrentEdgePt))
							{
								goto CheckAndAddPt;
							}
							else
							{
								DoVerticalScan = false; DoHorizontalScan = true;
								ScanOrientationSwitched = true;
							}
						}
						else
						{
							if (DoVerticalScan)
								X += horizontal_ScanDir;
							else
								Y += Vert_ScanDir;
							if (CheckPointWithinStraightRect(X, Y, Box) != 0) return true;
							//return true;
						}
					}
				}

				if (CheckPointWithinStraightRect(X, Y, Box) != 0) 
					return true;
			}
			return true;
		}
		catch (...)
		{
			return false;
		}

	}


	/*bool EdgeDetectionSingleChannel::TraverseEdgeStraightRect3(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir)
	//{
	//	try
	//	{
	//		int X, Y, X_break, Y_Break, EdgeDetectedType = 0, PtCounter = 0, EdgePtPositionFlag = 0, BoxLength = 0;
	//		Pt PtAtOtherEnd, CurrentEdgePt, P1, PrevEdgePt;
	//		bool EdgeLoopBack = false;
	//		bool DoHorizontalScan = true, DoVerticalScan = false, DefaultVerticalScan = false, ScanOrientationSwitched = false;
	//		int horizontal_ScanDir = 1, Vert_ScanDir = 1, currScanDir = 0, tempScanDir = 0;

	//		X = PointsList->begin()->X;
	//		Y = PointsList->begin()->Y;
	//		PtAtOtherEnd.X = X; P1.X = X;
	//		PtAtOtherEnd.Y = Y; P1.Y = Y;
	//		CurrentEdgePt.X = X; CurrentEdgePt.Y = Y;
	//		PrevEdgePt.X = X; PrevEdgePt.Y = Y;
	//		P1.X = X; P1.Y = Y;
	//		NoEdgePixelCount = 0;

	//		if (SDir == Upwards) Vert_ScanDir = -1;
	//		if (SDir == Leftwards) horizontal_ScanDir = -1;

	//		if (SDir == Downwards || SDir == Upwards)
	//		{
	//			//We start from the Left of the box and travel to the right
	//			DefaultVerticalScan = true;
	//			DoVerticalScan = true; DoHorizontalScan = false;
	//			BoxLength = Box[2];
	//			X += horizontal_ScanDir; //Shift by 1 pixel to start scan Since we always take stPt as the smaller X, a vertical scan means we will start from the left end of the box
	//		}
	//		else
	//		{
	//			//We travel from the top of the box to the bottom
	//			Y += Vert_ScanDir; //Shift by 1 pixel up or down to start scan...
	//			DoVerticalScan = false; DoHorizontalScan = true;
	//			BoxLength = Box[3];
	//		}

	//		//Ensure we are within the box + plus get transformed coordinate to start scan correctly....
	//		if (CheckPointWithinStraightRect(X, Y, Box) != 0) return false;
	//		bool TakeThisPt = false;

	//		//We go uniformly from starting point to end point. 
	//		while (CheckPointWithinStraightRect(X, Y, Box) == 0)
	//		{
	//			if (DoHorizontalScan)
	//			{
	//				EdgeDetectedType = ScanHorizontalAndInsertToList(&X, Y, &CurrentEdgePt);
	//				switch (EdgeDetectedType)
	//				{
	//				case 1: //We got an edge point
	//					if (PointsList->size() == 1 || ScanOrientationSwitched)
	//					{

	//						tempScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
	//						if (tempScanDir != 0) currScanDir = tempScanDir;
	//						TakeThisPt = true; ScanOrientationSwitched = false;
	//					}

	//					//Check if we are back to first scanned pixel! We got back to starting edge point.
	//					if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
	//						return true;

	//					//Check for same sign of scan direction.. 
	//					tempScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
	//					horizontal_ScanDir = CurrentEdgePt.X - PrevEdgePt.X;
	//					if (horizontal_ScanDir != 0) horizontal_ScanDir = horizontal_ScanDir / abs(horizontal_ScanDir);
	//					if ((tempScanDir ^ currScanDir) >= 0)
	//					{
	//						//We are travelling in correct direction. Let us take the point...
	//						TakeThisPt = true;
	//					}
	//					if (tempScanDir == 0) TakeThisPt = false;

	//					if (TakeThisPt)
	//					{
	//						PointsList->push_back(CurrentEdgePt);
	//						PtCounter++;
	//						if (PtCounter >= 15)
	//						{
	//							if (abs(P1.Y - CurrentEdgePt.Y) < abs(P1.X - CurrentEdgePt.X))
	//							{
	//								DoVerticalScan = true; DoHorizontalScan = false; ScanOrientationSwitched = true;
	//								Y = Y - EdgeFinderPixelRange;
	//							}
	//							P1 = CurrentEdgePt; PtCounter = 0;
	//						}
	//						//Jump to the new edge position and increment Y in correct direction for next scan
	//						Y = CurrentEdgePt.Y + Vert_ScanDir; X = CurrentEdgePt.X + horizontal_ScanDir;
	//						PrevEdgePt = CurrentEdgePt;
	//					}
	//					else
	//					{
	//						Y += Vert_ScanDir;
	//					}

	//					break;

	//				case -2: //Double Grad. Skip this row
	//					Y += Vert_ScanDir;
	//					break;

	//				case -3: //We have a pixel gradient. We need to shift to the left and scan again.
	//					X--;
	//					break;

	//				case -10:
	//					return true;
	//					break;

	//				case -1: //DId not get a point...

	//					if (GetEdgePtNearGivenPtStraightRect_H(X, Y, SDir, 20, 20, &CurrentEdgePt, Box, Vert_ScanDir))
	//					{
	//						if (CheckPointWithinStraightRect(CurrentEdgePt.X, CurrentEdgePt.Y, Box) == 0)
	//						{
	//							if (PointsList->size() == 1 || ScanOrientationSwitched)
	//							{
	//								tempScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
	//								if (tempScanDir != 0) currScanDir = tempScanDir;
	//								ScanOrientationSwitched = false;
	//							}

	//							tempScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
	//							//if (horizontal_ScanDir != 0) horizontal_ScanDir = horizontal_ScanDir / abs(horizontal_ScanDir);
	//							if ((tempScanDir ^ currScanDir) >= 0 || ((int)(CurrentEdgePt.X - PrevEdgePt.X) ^ horizontal_ScanDir) >= 0)
	//							{
	//								PointsList->push_back(CurrentEdgePt);
	//								PtCounter++;
	//								//Jump to the new edge position and increment Y in correct direction for next scan
	//								Y = CurrentEdgePt.Y + Vert_ScanDir; X = CurrentEdgePt.X + horizontal_ScanDir;
	//								PrevEdgePt = CurrentEdgePt;
	//							}
	//							else
	//							{
	//								Y += Vert_ScanDir;
	//							}
	//							horizontal_ScanDir = (CurrentEdgePt.X - PrevEdgePt.X);
	//							if (horizontal_ScanDir != 0) horizontal_ScanDir = horizontal_ScanDir / abs(horizontal_ScanDir);
	//						}
	//						else
	//						{
	//							if (!DefaultVerticalScan)
	//							{
	//								DoVerticalScan = true; DoHorizontalScan = false;
	//								ScanOrientationSwitched = true;
	//							}
	//						}
	//					}
	//					else
	//					{
	//						DoVerticalScan = true; DoHorizontalScan = false;
	//						ScanOrientationSwitched = true;
	//					}

	//				default:
	//					break;
	//				}
	//			}

	//			if (DoVerticalScan)
	//			{
	//				EdgeDetectedType = ScanVerticalAndInsertToList(X, &Y, &CurrentEdgePt);
	//				switch (EdgeDetectedType)
	//				{
	//				case 1: //We got an edge point
	//					if (PointsList->size() == 1 || ScanOrientationSwitched)
	//					{
	//						tempScanDir = CurrentEdgePt.X - PrevEdgePt.X;
	//						if (tempScanDir != 0) currScanDir = tempScanDir;
	//						TakeThisPt = true; ScanOrientationSwitched = false;
	//					}
	//					//Check if we are back to first scanned pixel! We got back to starting edge point.
	//					if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
	//						return true;

	//					//Check for same sign of scan direction.. 
	//					tempScanDir = CurrentEdgePt.X - PrevEdgePt.X;
	//					Vert_ScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
	//					if (Vert_ScanDir != 0) Vert_ScanDir = Vert_ScanDir / abs(Vert_ScanDir);

	//					if ((tempScanDir ^ currScanDir) >= 0)
	//					{
	//						//We are travelling in correct direction. Let us take the point...
	//						TakeThisPt = true;
	//					}
	//					if (tempScanDir == 0) TakeThisPt = false;

	//					if (TakeThisPt)
	//					{
	//						PointsList->push_back(CurrentEdgePt);
	//						PtCounter++;
	//						if (PtCounter >= 15)
	//						{
	//							if (abs(P1.Y - CurrentEdgePt.Y) > abs(P1.X - CurrentEdgePt.X))
	//							{
	//								DoVerticalScan = false; DoHorizontalScan = true;
	//								X = X - EdgeFinderPixelRange;
	//							}
	//							P1 = CurrentEdgePt; PtCounter = 0;
	//						}
	//						//Jump to the new edge position and increment Y in correct direction for next scan
	//						Y = CurrentEdgePt.Y; X = CurrentEdgePt.X + horizontal_ScanDir;
	//						PrevEdgePt = CurrentEdgePt;
	//					}
	//					else
	//					{
	//						X += horizontal_ScanDir;
	//					}
	//					break;

	//				case -2: //Double Grad. Skip this row
	//					X += horizontal_ScanDir;
	//					break;

	//				case -3: //We have a pixel gradient. We need to shift to the left and scan again.
	//					Y--;
	//					break;

	//				case -10:
	//					return true;
	//					break;

	//				case -1: //DId not get a point...

	//					if (GetEdgePtNearGivenPtStraightRect_V(X, Y, SDir, 20, 20, &CurrentEdgePt, Box, horizontal_ScanDir))
	//					{
	//						if (CheckPointWithinStraightRect(CurrentEdgePt.X, CurrentEdgePt.Y, Box) == 0)
	//						{
	//							if (PointsList->size() == 1 || ScanOrientationSwitched)
	//							{
	//								tempScanDir = CurrentEdgePt.X - PrevEdgePt.X;
	//								if (tempScanDir != 0) currScanDir = tempScanDir;
	//								ScanOrientationSwitched = false;
	//							}

	//							if ((tempScanDir ^ currScanDir) >= 0 || ((int)(CurrentEdgePt.Y - PrevEdgePt.Y) ^ Vert_ScanDir) >= 0)
	//							{
	//								PointsList->push_back(CurrentEdgePt);
	//								PtCounter++;
	//								//Jump to the new edge position and increment Y in correct direction for next scan
	//								Y = CurrentEdgePt.Y; X = CurrentEdgePt.X + horizontal_ScanDir;
	//								PrevEdgePt = CurrentEdgePt;
	//							}
	//							else if (tempScanDir == 0)
	//							{
	//								if (CurrentEdgePt.Y - PrevEdgePt.Y == 0)
	//								{
	//									if (!DefaultVerticalScan)
	//									{
	//										DoVerticalScan = false; DoHorizontalScan = true;
	//										ScanOrientationSwitched = true;
	//									}
	//								}
	//							}
	//							else
	//							{
	//								X += horizontal_ScanDir;
	//							}
	//							Vert_ScanDir = CurrentEdgePt.Y - PrevEdgePt.Y;
	//							if (Vert_ScanDir != 0) Vert_ScanDir = Vert_ScanDir / abs(Vert_ScanDir);
	//						}
	//						else
	//						{
	//							if (!DefaultVerticalScan)
	//							{
	//								DoVerticalScan = false; DoHorizontalScan = true;
	//								ScanOrientationSwitched = true;
	//							}
	//						}
	//					}
	//					else
	//					{
	//						DoVerticalScan = false; DoHorizontalScan = true;
	//						ScanOrientationSwitched = true;
	//					}

	//				default:
	//					break;
	//				}

	//			}
	//		}
	//	}
	//	catch (...)
	//	{
	//		return false;
	//	}

	}
	*/


	//This function is similar to the above function, except that it operates within a boundry of an angular rect
	bool EdgeDetectionSingleChannel::TraverseEdgeAngularRect(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int Width = Box[4], Height, X, Y, XPrevious, YPrevious, X_Break, Y_Break;
			Pt Point1, Point2, OriginPoint, PrvToPrvPt, PtAtOtherEnd;
			bool TraverseDirection = true, EdgeLoopBack = false;
			double CosTh, SinTh;
			bool DoHorizontalScan = true, DoVerticalScan = false;

			if(Box[2] > Box[0])
			{
				Point1.X = Box[0];
				Point1.Y = Box[1];
				Point2.X = Box[2];
				Point2.Y = Box[3];
			}
			else
			{
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}
			Height = sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));

			CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X), 2) + pow((Point2.Y - Point1.Y), 2));
			SinTh = (Point2.Y - Point1.Y) / sqrt(pow((Point2.X - Point1.X), 2) + pow((Point2.Y - Point1.Y), 2));

			OriginPoint.X = Point1.X; // -Width / 2 * SinTh;
			OriginPoint.Y = Point1.Y; // -Width / 2 * CosTh;



			X = PointsList->begin()->X + 0.5;
			Y = PointsList->begin()->Y + 0.5;
			XPrevious = 0;
			YPrevious = 0;
			PrvToPrvPt.X = 0;
			PrvToPrvPt.Y = 0;
			PtAtOtherEnd.X = X;
			PtAtOtherEnd.Y = Y;

			//if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			//else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			//else if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			//else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			//else return false;
			int sd = 1;
			if (SDir == Rightwards || SDir == Leftwards)
			{
				bool UpperR = false;
				DoHorizontalScan = true; DoVerticalScan = false;
				if (Point1.Y > Point2.Y) UpperR = true;
				if (SDir == Leftwards) sd = -1;
				if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, UpperR, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, !UpperR, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else return false;
			}
			else
			{
				if (SDir == Upwards) sd = -1;
				DoHorizontalScan = false; DoVerticalScan = true;
				if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
				else return false;
			}

			int MaxPtsPossible = ImageWidth * ImageHeight / EdgeFinderPixelRange; //We will consume a minimum of Edge thickness number of pixels for one edge pt
			for(int DirectionCount = 0; DirectionCount < 2; DirectionCount++)
			{
				int PointsCount = 0;
				while(CheckPointWithinAngularRect(X, Y, SinTh, CosTh, OriginPoint, Width, Height))
				{
					PointsCount++;
					if(PointsCount > MaxPtsPossible) 
						break;
					if (DoHorizontalScan) // (SDir == Leftwards || SDir == Rightwards)
					{
						if (YPrevious == Y)
						{
							if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
							else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						}
						else
						{
							if (YPrevious < Y) // == Y - 1)
							{
								if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
									continue;
							}
							else if (YPrevious > Y) // == Y + 1)
							{
								if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
									continue;
							}
						}
						if (EdgeLoopBack) return true;
						//We did not get a point in horizontal Scan. Let us try in vertical scan. 
						if (NoEdgePixelCount < 2 * EdgeFinderPixelRange + 1)
						{
							if (NoEdgePixelCount == 1)
							{
								X_Break = X; Y_Break = Y;
							}
							else
								continue; //While.
						}
						else
						{
							DoVerticalScan = true;
							X = X_Break; Y = Y_Break - 1;
							//continue;
						}
					}

					if (DoVerticalScan)
					{
						//If we were originally to do a horizontal scan, let us revert to that state. 
						if (SDir == Leftwards || SDir == Rightwards) DoVerticalScan = false;

						if (XPrevious == X)
						{
							if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
							else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						}
						else
						{
							if (XPrevious < X) // == X - 1)
							{
								if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
									continue;
							}
							else if (XPrevious > X) // == X + 1)
							{
								if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
									continue;
							}
						}
						if (EdgeLoopBack) return true;
						if (NoEdgePixelCount < 2 * EdgeFinderPixelRange + 1)
						{
							if (NoEdgePixelCount == 1)
							{
								X_Break = X; Y_Break = Y;
							}
							else
								continue; //While.
						}
						else
						{
							if (SDir == Leftwards || SDir == Rightwards) DoHorizontalScan = true;
							X = X_Break - 1; Y = Y_Break;
							//continue;
						}

					}

					//Even if we did not get an edge, we continue the scan on the next row/column until we have a contiguous set of 2 * EdgeRange number of pixels. Then we call dust skip
					if (NoEdgePixelCount < 2 * EdgeFinderPixelRange + 1)
						continue;

					Pt Pnt1, Pnt2;
					std::list<Pt>::iterator Itr;
					if(PointsList->size() >= 15)
					{
						if(TraverseDirection)
						{
							Itr = PointsList->end();
							for(int j = 1; j<= 15; j++) Itr--;
							
							if(!DustSkip(*(--(PointsList->end())), *(Itr), &Pnt1, &Pnt2)) break;
						}
						else
						{
							Itr = PointsList->begin();
							for(int j = 1; j<= 14; j++) Itr++;
							
							if(!DustSkip(*(PointsList->begin()), *(Itr), &Pnt1, &Pnt2)) break;
						}
					}
					else
					{
						if(TraverseDirection)
						{
							if(!DustSkip(*(--(PointsList->end())), *(PointsList->begin()), &Pnt1, &Pnt2)) break;
						}
						else
						{
							if(!DustSkip(*(PointsList->begin()), *(--(PointsList->end())), &Pnt1, &Pnt2)) break;
						}
					}
					
					if(!CheckPointWithinAngularRect(Pnt1.X, Pnt1.Y, SinTh, CosTh, OriginPoint, Width, Height)) break;
					if(!CheckPointWithinAngularRect(Pnt2.X, Pnt2.Y, SinTh, CosTh, OriginPoint, Width, Height)) break;
					
					if(TraverseDirection)
					{
						PointsList->push_back(Pnt1);
						PointsList->push_back(Pnt2);
					}
					else
					{
						PointsList->push_front(Pnt1);
						PointsList->push_front(Pnt2);
					}

					X = Pnt2.X + 0.5;
					Y = Pnt2.Y + 0.5;
					XPrevious = Pnt1.X + 0.5;
					YPrevious = Pnt1.Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}

				if(DirectionCount == 0)
				{
					TraverseDirection = false;
					EdgeLoopBack = false;
					PtAtOtherEnd.X = (--(PointsList->end()))->X + 0.5;
					PtAtOtherEnd.Y = (--(PointsList->end()))->Y + 0.5; 
				
					X = PointsList->begin()->X + 0.5;
					Y = PointsList->begin()->Y + 0.5;
					XPrevious = (++(PointsList->begin()))->X + 0.5;
					YPrevious = (++(PointsList->begin()))->Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}
			}
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0014", __FILE__, __FUNCSIG__); return false; }
	}

	bool EdgeDetectionSingleChannel::TraverseEdgeAngularRect2(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir, struct Pt travRefPt)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int X, Y, X_break, Y_Break, EdgeDetectedType = 0, PtCounter = 0, EdgePtPositionFlag = 0;
			Pt PtAtOtherEnd, StPt, EndPt, OriginPoint, CurrentEdgePt, P1;
			bool EdgeLoopBack = false;
			bool DoHorizontalScan = true, DoVerticalScan = false;
			int horizontal_ScanDir = 1;
			double angleDelta = 0;
			bool ScanInPositiveDir = true;

			X = PointsList->begin()->X + 0.5;
			Y = PointsList->begin()->Y + 0.5;
			PtAtOtherEnd.X = X; P1.X = X;
			PtAtOtherEnd.Y = Y; P1.Y = Y;
			CurrentEdgePt.X = X; CurrentEdgePt.Y = Y;
			NoEdgePixelCount = 0;
			//PointsList->pop_back();

			//int Left = Box[0];
			//int Right = Box[0] + Box[2];
			//int Top = Box[1];
			//int Bottom = Box[1] + Box[3];
			int ShiftCounter = 0;

			if (Box[2] > Box[0])
			{
				StPt.X = Box[0];
				StPt.Y = Box[1];
				EndPt.X = Box[2];
				EndPt.Y = Box[3];
			}
			else
			{
				StPt.X = Box[2];
				StPt.Y = Box[3];
				EndPt.X = Box[0];
				EndPt.Y = Box[1];
				
			}
			if (EndPt.Y < StPt.Y) horizontal_ScanDir = -1;
			double Height = sqrt(pow((EndPt.X - StPt.X), 2) + pow((EndPt.Y - StPt.Y), 2));
			double CosTh = (EndPt.X - StPt.X) / Height;
			double SinTh = (EndPt.Y - StPt.Y) / Height;
			int Width = Box[4];

			OriginPoint.X = StPt.X; // -Width / 2 * SinTh;
			OriginPoint.Y = StPt.Y; // -Width / 2 * CosTh;

			//Get Angular position of start and end pts. We use this as a criterion to ensure we progress in our scan and complete without going into infinite loops
			GetEdgePointAngularPosition(&StPt, travRefPt);
			GetEdgePointAngularPosition(&EndPt, travRefPt);
			if (StPt.ScanAnglePosition == 0 && EndPt.ScanAnglePosition > 3 * M_PI_2)
				StPt.ScanAnglePosition = 2 * M_PI;
			//Check the overall direction of scan we need to be doing.... this is simply based on st and end position
			angleDelta = EndPt.ScanAnglePosition - StPt.ScanAnglePosition;
			if (angleDelta < 0) ScanInPositiveDir = false;

			//Now set up if we should default to vertical or horizontal scan. 
			if (SDir == Rightwards || SDir == Leftwards)
			{
				DoHorizontalScan = true; DoVerticalScan = false;
			}
			else
			{
				DoHorizontalScan = false; DoVerticalScan = true;
			}
			bool BreakTraversal = false;

			while (!BreakTraversal)//X >= StPt.X && X < Right && Y >= Top && Y < Bottom)
			{
				if (DoHorizontalScan)
				{
					EdgeDetectedType = ScanHorizontalAndInsertToList(&X, Y, &CurrentEdgePt); //, &EdgeLoopBack, PtAtOtherEnd);

					if (EdgeDetectedType == 1)
					{
				GotEdge_Horizontal:
						EdgePtPositionFlag = CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, OriginPoint, Width, (int)Height);
						if (EdgePtPositionFlag == 0) //Edge Point within angular rect
						{//break;

							if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
								//We got back to starting edge point.
								return true;
							AddPointToList(&Y, CurrentEdgePt, PointsList, ScanInPositiveDir, travRefPt);
							PtCounter++;
							if (PtCounter >= 10)
							{
								if (abs(P1.Y - CurrentEdgePt.Y) < abs(P1.X - CurrentEdgePt.X))
								{
									DoVerticalScan = true; DoHorizontalScan = false;
									Y = Y - EdgeFinderPixelRange;
								}
								P1 = CurrentEdgePt; PtCounter = 0;
							}

						}
						else
						{
							if (EdgePtPositionFlag == -1)
							{
								//We got a point to the left of the box. So we shift to the right and then scan again
								CurrentEdgePt = *(--PointsList->end());
								X = CurrentEdgePt.X; Y = CurrentEdgePt.Y;
								//if (P1.Y >= CurrentEdgePt.Y) Y += EdgeFinderPixelRange;
								//else Y -= EdgeFinderPixelRange;
								if (ScanInPositiveDir) Y += EdgeFinderPixelRange;
								else Y -= EdgeFinderPixelRange;
							}
							else if (EdgePtPositionFlag == 1)
							{
								//We have reached the end of the box. We can now terminate. 
								BreakTraversal = true;
							}
							else if (abs(EdgePtPositionFlag) == 2)
							{
								//We have gotten a point that is likely to be noise and its too far from the edge. So we ignore this, increment Y and move on...
								CurrentEdgePt = *(--PointsList->end()); 
								if (P1.Y > CurrentEdgePt.Y) Y--;
								else Y++;
							}
						}
					}
					else if (EdgeDetectedType == -2)
					{
						//We got an island pixel row. So just skip this and move to next row
						if (ScanInPositiveDir) Y--;  // (P1.Y > CurrentEdgePt.Y)
						else Y++;
						DoHorizontalScan = true;
					}
					else if (EdgeDetectedType == -10)
					{
						// Some error happened. So let us quit
						return true;
					}
					else if (EdgeDetectedType == -3)
					{
						if (P1.X > CurrentEdgePt.X)	X--;
						else X++;
					}
					else
					{
						//Let us now search in the vicinity
						if (NoEdgePixelCount > 0 && NoEdgePixelCount <= 2 * EdgeFinderPixelRange && PointsList->size() < EdgeFinderPixelRange)
						{
							int shift_dir = -1;
							if (NoEdgePixelCount == 1) //First miss. Note the break points
							{
								X_break = X; Y_Break = Y;
							}
							// We have tried a number of times in one direction. So go to the other side of the starting position
							if (ShiftCounter >= EdgeFinderPixelRange) shift_dir = 1;

							ShiftCounter++;
							//Shift by one range. 
							int xx = X + shift_dir * EdgeFinderPixelRange;
							//Check if we are within scanning area
							//if (xx >= Left && xx < Right)
							//{
								//Continue scan in same direction...
								DoHorizontalScan = true;
								X = xx;
							//}
						}
						else
						{
							//Y = Y_Break - EdgeFinderPixelRange; NoEdgePixelCount = 0;
							//X = CurrentEdgePt.X + 0.5; Y = CurrentEdgePt.Y + 0.5;
							//DoVerticalScan = true; DoHorizontalScan = false;
							ScanDirection ssd = Rightwards;
							int xx = X - 5 * EdgeFinderPixelRange;
							Pt LastPt = *(--(PointsList->end()));
							if (P1.X > LastPt.X)
							{
								ssd = Leftwards; xx = X + 5 * EdgeFinderPixelRange;
							}

							//We are unable to get a point. So let us do a scan within a small window nearby
							if (!GetEdgePtNearGivenPtAngRect_H(xx, Y, ssd, 10 * EdgeFinderPixelRange, 5 * EdgeFinderPixelRange, SinTh, CosTh, OriginPoint, &CurrentEdgePt, Width, Height, horizontal_ScanDir))
							{
								//EdgePtPositionFlag = CheckPointWithinAngularRect(X, Y, SinTh, CosTh, OriginPoint, Width, (int)Height);
								//if (EdgePtPositionFlag != 0)
								//{
								Pt NextP1, NextP2;
								if (!DustSkip(P1, CurrentEdgePt, &NextP1, &NextP2))
								{
									//We couldnt get a point with DustSkip. So let us switch to vertical scan
									DoVerticalScan = true; DoHorizontalScan = false;
								}
								else
								{
									//We skipped and got points. Let is now evaluate the direction and proceed forward.
									P1 = NextP1; CurrentEdgePt = NextP2;
									GetEdgePointAngularPosition(&P1, travRefPt);
									GetEdgePointAngularPosition(&CurrentEdgePt, travRefPt);
									Y = P1.Y;
									AddPointToList(&Y, P1, PointsList, ScanInPositiveDir, travRefPt);
									Y = CurrentEdgePt.Y; 
									AddPointToList(&Y, CurrentEdgePt, PointsList, ScanInPositiveDir, travRefPt);

									//PointsList->push_back(P1); PointsList->push_back(NextP2); 
									PtCounter = 0;
									X = CurrentEdgePt.X; 
									//if (P1.Y > CurrentEdgePt.Y) Y--;
									//else Y++;
								}
							}
							else
							{
								X = CurrentEdgePt.X; Y = CurrentEdgePt.Y;
								goto GotEdge_Horizontal;
							}
						}
					}
				}

				if (DoVerticalScan)
				{
					EdgeDetectedType = ScanVerticalAndInsertToList(X, &Y, &CurrentEdgePt); //, &EdgeLoopBack, PtAtOtherEnd);

					//if (X < Left || X >= Right || Y < Top || Y >= Bottom) break;

					if (EdgeDetectedType == 1)
					{
		GotEdge_Vertical:
						EdgePtPositionFlag = CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, OriginPoint, Width, (int)Height);
						if (EdgePtPositionFlag == 0)
						{
							if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
								return false;
							PointsList->push_back(CurrentEdgePt);
							PtCounter++;
							//Now we check slope of edge. If current pt and 10th point before this pt give a slope less than 45 deg, we switch to vertical scan.
							if (PtCounter >= 10)
							{
								if (abs(P1.Y - CurrentEdgePt.Y) > abs(P1.X - CurrentEdgePt.X))
								{
									DoHorizontalScan = true; DoVerticalScan = false;
									X = X - EdgeFinderPixelRange;
								}
								P1 = CurrentEdgePt; PtCounter = 0;
							}
							X++;
						}
						else if (EdgePtPositionFlag == -1)
						{
							CurrentEdgePt = *(--PointsList->end());
							X = CurrentEdgePt.X + EdgeFinderPixelRange;
							Y = CurrentEdgePt.Y;
						}
						else if (EdgePtPositionFlag == 1)
						{
							//We have reached the end of the box. We can now terminate. 
							BreakTraversal = true;
						}
						else if (abs(EdgePtPositionFlag) == 2)
						{
							//We have gotten a point that is likely to be noise and its too far from the edge. So we ignore this, increment Y and move on...
							CurrentEdgePt = *(--PointsList->end());
							X++;
						}
					}
					else if (EdgeDetectedType == -2)
					{
						X++; DoVerticalScan = true;
					}
					else if (EdgeDetectedType == -10)
					{
						//Some error happened. So break everything and quit
						return true;
					}
					else if (EdgeDetectedType == -3)
					{
						if (P1.Y > CurrentEdgePt.Y)	Y--;
						else Y++;
					}
					else
					{
						if (NoEdgePixelCount > 0 && NoEdgePixelCount <= 2 * EdgeFinderPixelRange && PointsList->size() < 5)
						{
							int shift_dir = -1;
							if (NoEdgePixelCount == 1)
							{
								X_break = X; Y_Break = Y;
							}
							if (ShiftCounter >= EdgeFinderPixelRange) shift_dir = 1;

							ShiftCounter++;
							int yy = Y + shift_dir * EdgeFinderPixelRange;

							//if (yy >= Top && yy < Bottom)
							//{
								DoVerticalScan = true;
								Y = yy;
							//}
						}
						else
						{
							//X = X_break - EdgeFinderPixelRange; NoEdgePixelCount = 0;
							//X = CurrentEdgePt.X + 0.5; Y = CurrentEdgePt.Y + 0.5;
							//DoHorizontalScan = true; DoVerticalScan = false;
							//X++;
							ScanDirection ssd = Downwards;
							int yy = Y - 5 * EdgeFinderPixelRange;
							if (P1.Y > CurrentEdgePt.Y)
							{
								ssd = Upwards; yy = Y + 5 * EdgeFinderPixelRange;
							}

							if (!GetEdgePtNearGivenPtAngRect_V(X, yy, ssd, 5 * EdgeFinderPixelRange, 10 * EdgeFinderPixelRange, SinTh, CosTh, OriginPoint, &CurrentEdgePt, Width, Height, horizontal_ScanDir))
							{	//EdgePtPositionFlag = CheckPointWithinAngularRect(X, Y, SinTh, CosTh, OriginPoint, Width, (int)Height);
								//if (EdgePtPositionFlag != 0)
								Pt NextP1, NextP2;
								if (!DustSkip(P1, CurrentEdgePt, &NextP1, &NextP2))
								{
									//We couldnt get a point with DustSkip. So let us switch to vertical scan
									DoVerticalScan = false; DoHorizontalScan = true;
								}
								else
								{
									//We skipped and got points. Let is now evaluate the direction and proceed forward.
									P1 = NextP1; CurrentEdgePt = NextP2;
									PointsList->push_back(P1); PointsList->push_back(NextP2); PtCounter = 0;
									X = CurrentEdgePt.X; Y = CurrentEdgePt.Y;
									if (P1.X > CurrentEdgePt.X) X--;
									else X++;
								}
							}
							else
							{
								Y = CurrentEdgePt.Y; X = CurrentEdgePt.X;
								goto GotEdge_Vertical;
							}
						}
					}
				}
			}


				//	if (DoHorizontalScan)
				//	{
				//		PointsList->push_back(CurrentEdgePt);
				//		Y += horizontal_ScanDir; //Got an edge. Go to the next row
				//	}
				//	else
				//	{
				//		//Let us now search in the vicinity
				//		if (NoEdgePixelCount > 0 && NoEdgePixelCount <= 2 * EdgeFinderPixelRange)
				//		{
				//			int shift_dir = -1;
				//			if (NoEdgePixelCount == 1) //First miss. Note the break points
				//			{
				//				X_break = X; Y_Break = Y;
				//			}
				//			// We have tried a number of times in one direction. So go to the other side of the starting position
				//			if (ShiftCounter >= EdgeFinderPixelRange) shift_dir = 1;

				//			ShiftCounter++;
				//			//Shift by one range. 
				//			int xx = X + shift_dir * EdgeFinderPixelRange;
				//			//Check if we are within scanning area
				//			//if (CheckPointWithinAngularRect(xx, Y, SinTh, CosTh, OriginPoint, Width, (int)Height))
				//			//{
				//				//Continue scan in same direction...
				//				DoHorizontalScan = true;
				//				X = xx;
				//			//}
				//			//else
				//			//{
				//			//	Y = Y_Break - EdgeFinderPixelRange;
				//			//	DoVerticalScan = true;
				//			//}
				//		}
				//		else
				//		{
				//			Y = Y_Break - EdgeFinderPixelRange; NoEdgePixelCount = 0;
				//			DoVerticalScan = true;
				//		}
				//	}
				//}

			//	while (DoVerticalScan)
			//	{
			//		DoVerticalScan = ScanVerticalAndInsertToList(X, &Y, &CurrentEdgePt); //, &EdgeLoopBack, PtAtOtherEnd);

			//		//if (X < Left || X >= Right || Y < Top || Y >= Bottom) break;
			//		//if (!CheckPointWithinAngularRect(X, Y, SinTh, CosTh, OriginPoint, Width, (int)Height)) break;
			//		if (!CheckPointWithinAngularRect(X, Y, SinTh, CosTh, OriginPoint, Width, (int)Height))
			//		{
			//		//	PointsList->pop_back();
			//			if (NoEdgePixelCount > 0 && !DoVerticalScan)
			//			{
			//				X = X_break; Y = Y_Break; NoEdgePixelCount = 0;
			//				DoHorizontalScan = true;
			//			}
			//			break;
			//		}

			//		if (DoVerticalScan)
			//		{
			//			PointsList->push_back(CurrentEdgePt);
			//			X++;
			//		}
			//		else
			//		{
			//			if (NoEdgePixelCount > 0 && NoEdgePixelCount <= 2 * EdgeFinderPixelRange)
			//			{
			//				int shift_dir = -1;
			//				if (NoEdgePixelCount == 1)
			//				{
			//					X_break = X; Y_Break = Y;
			//				}
			//				if (ShiftCounter >= EdgeFinderPixelRange) shift_dir = 1;

			//				ShiftCounter++;
			//				int yy = Y + shift_dir * EdgeFinderPixelRange;

			//				//if (CheckPointWithinAngularRect(X, yy, SinTh, CosTh, OriginPoint, Width, (int)Height)) // (yy >= Top && yy < Bottom)
			//				//{
			//					DoVerticalScan = true;
			//					Y = yy;
			//				//}
			//				//else
			//				//{
			//				//	X = X_break - EdgeFinderPixelRange;
			//				//	DoHorizontalScan = true;
			//				//}
			//			}
			//			else
			//			{
			//				X = X_break - EdgeFinderPixelRange; NoEdgePixelCount = 0;
			//				DoHorizontalScan = true;
			//			}
			//		}
			//	}
			//}

			return true;
		}
		catch (...)
		{
			SetAndRaiseErrorMessage("EDSC0013a", __FILE__, __FUNCSIG__); return false;
		}

	}

	int EdgeDetectionSingleChannel::TraverseEdgeAngularRect3(int *Box, std::list<struct Pt> *PointsList, ScanDirection SDir, struct Pt travRefPt)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int X, Y, EdgeDetectedType = 0, PtCounter = 0, EdgePtPositionFlag = 0, BoxLength = 0;
			Pt PtAtOtherEnd, StPt, EndPt, CurrentEdgePt, P1, currScanPosition, prevScanPosition;
			bool EdgeLoopBack = false;
			bool DoHorizontalScan = true, DoVerticalScan = false, DefaultVerticalScan = false;
			int horizontal_ScanDir = 1, SearchBoxSize = 20;
			//bool ReturnValue = false;


			//Get box limits and directions...
			if (Box[2] > Box[0])
			{
				StPt.X = Box[0] * this->CamSizeRatio;
				StPt.Y = Box[1] * this->CamSizeRatio;
				EndPt.X = Box[2] * this->CamSizeRatio;
				EndPt.Y = Box[3] * this->CamSizeRatio;
			}
			else
			{
				StPt.X = Box[2] * this->CamSizeRatio;
				StPt.Y = Box[3] * this->CamSizeRatio;
				EndPt.X = Box[0] * this->CamSizeRatio;
				EndPt.Y = Box[1] * this->CamSizeRatio;
			}
			X = PointsList->begin()->X; //StPt.X; //
			Y = PointsList->begin()->Y; //StPt.Y; // 
			PtAtOtherEnd.X = X; P1.X = X;
			PtAtOtherEnd.Y = Y; P1.Y = Y;
			CurrentEdgePt.X = X; CurrentEdgePt.Y = Y;
			NoEdgePixelCount = 0;

			if (StPt.X < SearchBoxSize / 2 || EndPt.X < SearchBoxSize / 2 || X < SearchBoxSize / 2) return 0;
			if (StPt.Y < SearchBoxSize / 2 || EndPt.Y < SearchBoxSize / 2 || Y < SearchBoxSize / 2) return 0;
			if (StPt.X > ImageWidth - SearchBoxSize / 2 || EndPt.X > ImageWidth - SearchBoxSize / 2 || X > ImageWidth - SearchBoxSize / 2) return 0;
			if (StPt.Y > ImageHeight - SearchBoxSize / 2 || EndPt.Y > ImageHeight - SearchBoxSize / 2 || Y > ImageHeight - SearchBoxSize / 2) return 0;

			if (EndPt.Y < StPt.Y) horizontal_ScanDir = -1;
			double Height = sqrt(pow((EndPt.X - StPt.X), 2) + pow((EndPt.Y - StPt.Y), 2));
			double CosTh = (EndPt.X - StPt.X) / Height;
			double SinTh = (EndPt.Y - StPt.Y) / Height;
			int Width = Box[4] * this->CamSizeRatio;
			BoxLength = (int)Height;
			
			if (SDir == Downwards || SDir == Upwards)
			{
				DefaultVerticalScan = true;
				DoVerticalScan = true; DoHorizontalScan = false;
				X++; //Shift by 1 pixel to start scan Since we always take stPt as the smaller X, a vertical scan means we will start from the left end of the box
			}
			else
			{
				Y += horizontal_ScanDir; //Shift by 1 pixel up or down to start scan...
				DoVerticalScan = false; DoHorizontalScan = true;
			}
			int currentPosition = 0;
			//currentPosition = CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition);
			//X = StPt.X + (currScanPosition.X + 1) * CosTh + currScanPosition.Y * SinTh;
			//Y = StPt.Y + (currScanPosition.X + 1) * SinTh - currScanPosition.Y * CosTh;

			//Ensure we are within the box + plus get transformed coordinate to start scan correctly....
			for (int jj = 1; jj < 2 * BoxLength; jj++)
			{
				currentPosition = CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition);
				if (currScanPosition.X > 0) break;
				IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, jj);
				//switch (currentPosition)
				//{
				//case 0:
				//	//We got a proper point within the box. So lets move to the next step
				//	jj = 100;
				//	break;
				//case -1:
				//	//if (currScanPosition.X > -7)
				//	//{
				//		int inc = 1;
				//		while (CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) != 0)
				//		{
				//			IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, inc++);
				//			if (currScanPosition.X > 0) break;
				//		}

				//		//if (DoHorizontalScan)
				//		//	Y += horizontal_ScanDir;
				//		//else
				//		//	X += horizontal_ScanDir;
				//	//}
				//	break;

				//default:
				//	return -100; // false;
				//	break;
				//}
			}

			prevScanPosition = currScanPosition;
			int ScanUpperLimit = 2 * BoxLength;
			bool Re_arrived_On_Edge = false;

			//We go uniformly from starting point to end point. 
			for (currentPosition = currScanPosition.X; currentPosition < ScanUpperLimit; currentPosition++)
			{
				//if (currentPosition < 0) return false;
				if (currentPosition < 0)
				{

					if (PointsList->size() > 0)
					{
						Pt LastPt = *(--PointsList->end());
						X = LastPt.X; Y = LastPt.Y;
						int inc = 1;
						while (CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
						{
							IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, inc++);
							if (currScanPosition.X > prevScanPosition.X) break;
						}

						//return 101; // true;
					}
					else return -101; // false;
				}

				//Check if we have to do horizontal scan
				if (DoHorizontalScan)
				{
					EdgeDetectedType = ScanHorizontalAndInsertToList(&X, Y, &CurrentEdgePt);
					switch (EdgeDetectedType)
					{
					case 1: //We got an edge point
						//Check if its within the angular rect
						EdgePtPositionFlag = CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition);
						switch (EdgePtPositionFlag)
						{
						case 0: //We are within the box
							//Check if we are back to first scanned pixel! We got back to starting edge point.
							if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
								return 800; // true;
							//Are we moving in positive direction within angularrect? 
							if (currScanPosition.X > prevScanPosition.X)
							{//We are OK. So let us add to the list
								PointsList->push_back(CurrentEdgePt);
								//ScanPtsList.push_back(currScanPosition);
								//currentPosition = currScanPosition.X - 1;
								prevScanPosition = currScanPosition;

								PtCounter++;
								//Now we check edge slope. if it goes beyond 45 degrees, we switch to vertical scan. 
								if (PtCounter >= 15)
								{
									if (abs(P1.Y - CurrentEdgePt.Y) < abs(P1.X - CurrentEdgePt.X))
									{
										DoVerticalScan = true; DoHorizontalScan = false;
										Y = Y - EdgeFinderPixelRange;
									}
									P1 = CurrentEdgePt; PtCounter = 0;
								}
							}
							else
							{
								//The gotten point is making us travel in opposite direction.... we just proceed until we reach end of box...
							}
							//Now increment Y axis to next row...
							Y = CurrentEdgePt.Y + horizontal_ScanDir; X = CurrentEdgePt.X;
							break;

						case 1: //We have traveled outside the box length. so we are finished!!
							return 200; // true;

						case -1: //We are to the left of the box so traveling in wrong direction. Let us see search for the last good point.
							//First check if last good point is within the boundary buffer of right edge ... 

							if (BoxLength - prevScanPosition.X <= BoundryBuffer)
							{
								return 103; // true;
							}
							else
							{
								//Let us increment in Y axis and try again....
								while (currScanPosition.X < 5)
								{
									IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 2);
									CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition); // != 0 && (currScanPosition.X - 1) <= prevScanPosition.X
								}

								//currentPosition = currScanPosition.X;
							}
							break;

						case 2: //We are outside the box
						case -2:
							//X = CurrentEdgePt.X;// currScanPosition.X;
							Y += horizontal_ScanDir;
							//IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 1);
							//currentPosition--;
							//return true;
							break;
						default:
							break;
						}
						break;

					case -2: //We got two gradients within our scan area. So we skip this row
						Y += horizontal_ScanDir;
						//currentPosition--;
						break;

					case -1: //Couldnt find an edge. SO let us do dust skip and try again. 
						//Pt NextP1, NextP2;
						//for (int jj = 0; jj < EdgeFinderPixelRange; jj++)
						//{

						//}
						//for (int xx = X - Width / 2; xx < X + Width / 2; xx += SearchBoxSize)
						for (int xx = X; xx > X - Width / 2; xx -= SearchBoxSize)
						{
							if (GetEdgePtNearGivenPtAngRect_H(xx, Y, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
							{
								if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
								{
									Re_arrived_On_Edge = true; X = xx; break;
								}
							}
						}

						if (!Re_arrived_On_Edge)
						{
							for (int xx = X + SearchBoxSize / 2; xx < X + Width / 2; xx += SearchBoxSize)
							{
								if (GetEdgePtNearGivenPtAngRect_H(xx, Y, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
								{
									if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
									{
										Re_arrived_On_Edge = true; X = xx; break;
									}
								}
							}
						}

						if (Re_arrived_On_Edge)
						{
							Re_arrived_On_Edge = false;
							if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
							{
								//Pt LastPt = *(--PointsList->end());
								if (currScanPosition.X > prevScanPosition.X)
								{
									//we got a good point. Let us add to the collection and proceed forward...
									PointsList->push_back(CurrentEdgePt);
									prevScanPosition = currScanPosition;
									X = CurrentEdgePt.X; Y = CurrentEdgePt.Y + horizontal_ScanDir;
									//currentPosition = currScanPosition.X - 1;
									//DoHorizontalScan = true; DoVerticalScan = false;
									PtCounter++;
								}
								else
								{
									Pt init = currScanPosition;
									bool GotPt = false;
									for (int k = 1; k <= SearchBoxSize; k++)
									{
										IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, init, k);
										if (GetEdgePtNearGivenPtAngRect_H(X, Y, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
										{
											if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
											{
												if (currScanPosition.X > prevScanPosition.X)
												{
													//we got a good point. Let us add to the collection and proceed forward...
													PointsList->push_back(CurrentEdgePt);
													//currentPosition = currScanPosition.X - 1;
													X = CurrentEdgePt.X; Y = CurrentEdgePt.Y + horizontal_ScanDir;
													prevScanPosition = currScanPosition; GotPt = true;
													PtCounter++;
													break;
												}
											}
										}
									}
									if (!GotPt)
									{
										DoVerticalScan = true; DoHorizontalScan = false;
										//currentPosition--;
									}
								}
							}
							else
							{
								if (!DefaultVerticalScan)
								{
									DoVerticalScan = true; DoHorizontalScan = false;
									//currentPosition--;
								}
							}
						}
						else
						{
							if (GetEdgePtNearGivenPtAngRect_V(X, Y, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
							{
								DoVerticalScan = true; DoHorizontalScan = false;
								//currentPosition--;
							}
							else
							{
								//SearchBoxSize += 20;
								Y += horizontal_ScanDir;
								//if (SearchBoxSize > Width / 2)
								//{
								//	SearchBoxSize = 20; Y = CurrentEdgePt.Y + horizontal_ScanDir; X = CurrentEdgePt.X + 1;
								//	//return 301;// true;
								//}
							}
						}
						break;

					case -10: //Error occurred during scan
						return -200; // true;

					case -3: //We have a pixel gradient. We need to shift to the left and scan again.
						X--;
						//currentPosition--; //Ensure we do not lose a point because we are redoing the scan!
						break;

					default:
						break;
					}
				}
				if (DoVerticalScan)
				{
					EdgeDetectedType = ScanVerticalAndInsertToList(X, &Y, &CurrentEdgePt);
					switch (EdgeDetectedType)
					{
					case 1: //We got an edge point
						//Check if its within the angular rect
						EdgePtPositionFlag = CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition);
						switch (EdgePtPositionFlag)
						{
						case 0: //We are within the box
								//Check if we are back to first scanned pixel! We got back to starting edge point.
							if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
								return 300; // true;
							//Are we moving in positive direction within angularrect? 
							if (currScanPosition.X > prevScanPosition.X)
							{//We are OK. So let us add to the list
								PointsList->push_back(CurrentEdgePt);
								//ScanPtsList.push_back(currScanPosition);
								//currentPosition = currScanPosition.X - 1;
								prevScanPosition = currScanPosition;

								PtCounter++;
								//Now we check edge slope. if it goes beyond 45 degrees, we switch to vertical scan. 
								if (PtCounter >= 15)
								{
									if (abs(P1.Y - CurrentEdgePt.Y) < abs(P1.X - CurrentEdgePt.X))
									{
										DoVerticalScan = true; DoHorizontalScan = false;
										Y = Y - EdgeFinderPixelRange;
									}
									P1 = CurrentEdgePt; PtCounter = 0;
								}
							}
							else
							{
								//The gotten point is making us travel in opposite direction.... //we just proceed until we reach end of box...
							}
							//Now increment X axis to next column...
							X++;
							break;

						case 1: //We have traveled outside the box length. so we are finished!!
							return 805; // true;

						case -1: //We are to the left of the box so traveling in wrong direction. Let us see search for the last good point.
								 //First check if last good point is within the boundary buffer of right edge ... 

							if (BoxLength - prevScanPosition.X <= BoundryBuffer)
							{
								return 105; // true;
							}
							else
							{
								//Let us increment by one pixel and continue....
								//X++;
								//IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 1);
								while (currScanPosition.X < 5)
								{
									IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 2);
									CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition); // != 0 && (currScanPosition.X - 1) <= prevScanPosition.X
								}

								//currentPosition = currScanPosition.X;
							}
							break;

						case 2: //We are outside the box
						case -2:
							//return true;
							X++;
							//X = CurrentEdgePt.X + 1; Y = CurrentEdgePt.Y;
							//IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 1);
							//currentPosition--;
							break;
						default:
							break;
						}
						break;

					case -2: //We got two gradients within our scan area. So we skip this row
						X++;
						//while (CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
						//{
						//	IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 1);
						//	if (currScanPosition.X > prevScanPosition.X) break;
						//}
						//	return 140;
						//currentPosition--;
						//currentPosition = currScanPosition.X - 1;
						break;

					case -1: //Couldnt find an edge. SO let us do dust skip and try again. 
						
						//for (int yy = Y - Width / 2; yy < Y + Width / 2; yy += SearchBoxSize)
						for (int yy = Y; yy > Y - Width / 2; yy -= SearchBoxSize)
						{
							if (GetEdgePtNearGivenPtAngRect_V(X, yy, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
							{
								if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
								{
									Re_arrived_On_Edge = true; Y = yy; break;
								}
							}
						}

						if (!Re_arrived_On_Edge)
						{
							for (int yy = Y + SearchBoxSize / 2; yy < Y + Width / 2; yy += SearchBoxSize)
							{
								if (GetEdgePtNearGivenPtAngRect_V(X, yy, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
								{
									if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
									{
										Re_arrived_On_Edge = true; Y = yy; break;
									}
								}
							}
						}
						
						//if (GetEdgePtNearGivenPtAngRect_V(X, Y, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
						if (Re_arrived_On_Edge)
						{
							Re_arrived_On_Edge = false;
							if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
							{
								//Pt LastPt = *(--PointsList->end());
								if (currScanPosition.X > prevScanPosition.X)
								{
									//we got a good point. Let us add to the collection and proceed forward...
									PointsList->push_back(CurrentEdgePt);
									X = CurrentEdgePt.X + horizontal_ScanDir; Y = CurrentEdgePt.Y;
									//currentPosition = currScanPosition.X - 1;
									//DoHorizontalScan = true; DoVerticalScan = false;
									prevScanPosition = currScanPosition;
									PtCounter++;
								}
								else // if (currScanPosition.X == prevScanPosition.X)
								{
									Pt init = currScanPosition;
									bool GotPt = false;
									for (int k = 1; k <= SearchBoxSize; k++)
									{
										//X = StPt.X + (initX + k) * CosTh + initY * SinTh;
										//Y = StPt.Y + (initX + k) * SinTh - initY * CosTh;
										IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, init, k);
										if (GetEdgePtNearGivenPtAngRect_V(X, Y, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
										{
											if (CheckPointWithinAngularRect(CurrentEdgePt.X, CurrentEdgePt.Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
											{
												if (currScanPosition.X > prevScanPosition.X)
												{
													//we got a good point. Let us add to the collection and proceed forward...
													PointsList->push_back(CurrentEdgePt);
													//currentPosition = currScanPosition.X - 1;
													X = CurrentEdgePt.X + horizontal_ScanDir; Y = CurrentEdgePt.Y;
													prevScanPosition = currScanPosition; GotPt = true;
													PtCounter++;
													break;
												}
											}
										}
									}
									if (!GotPt)
									{
										if (!DefaultVerticalScan)
										{
											DoHorizontalScan = true; DoVerticalScan = false;
										}
										else
										{
											X++; // currentPosition--;
										}
									}
								}
								//else
								//{
								//	if (!DefaultVerticalScan)
								//	{
								//		DoHorizontalScan = true; DoVerticalScan = false;
								//	}
								//	else
								//	{
								//		//X = StPt.X + (prevScanPosition.X + 1) * CosTh + prevScanPosition.Y * SinTh;
								//		//Y = StPt.Y + (prevScanPosition.X + 1) * SinTh - prevScanPosition.Y * CosTh;
								//		//IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, prevScanPosition, 1);
								//		//while (CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
								//		//{
								//		//	IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 1);
								//		//	if (currScanPosition.X > prevScanPosition.X) break;
								//		//}
								//		X++; currentPosition--;
								//	}
								//	//currentPosition--;
								//}
							}
							else
							{								//return true;
								if (!DefaultVerticalScan)
								{
									DoHorizontalScan = true; DoVerticalScan = false;
									//currentPosition--;
								}
								else
									return 310; // true;
							}
						}
						else
						{
							if (GetEdgePtNearGivenPtAngRect_H(X, Y, SDir, SearchBoxSize, SearchBoxSize, SinTh, CosTh, StPt, &CurrentEdgePt, Width, BoxLength, horizontal_ScanDir))
							{
								DoHorizontalScan = true; DoVerticalScan = false;
								//currentPosition--;
							}
							else
							{
								//while (CheckPointWithinAngularRect(X, Y, SinTh, CosTh, StPt, Width, BoxLength, &currScanPosition) == 0)
								//{
								//	IncrementPositionAngularRect(&X, &Y, SinTh, CosTh, StPt, currScanPosition, 1);
								//	if (currScanPosition.X > prevScanPosition.X) break;
								//}
								//SearchBoxSize += 20;
								X++;
								//currentPosition = currScanPosition.X - 1;
								//if (SearchBoxSize > Width / 2) // return 320; // true;
								//{
								//	SearchBoxSize = 20;
								//	X = CurrentEdgePt.X + 1; Y = CurrentEdgePt.Y + horizontal_ScanDir;
								//}
							}
						}

						break;

					case -10: //Error occurred during scan
						return -300; // true;

					case -3: //We have a pixel gradient. We need to shift to the left and scan again.
						X++;
						//currentPosition--; //Ensure we do not lose a point because we are redoing the scan!
						break;

					default:
						break;
					}
				}

			}
	//FinishScan:
	//		std::string msg = std::to_string(currentPosition) + "  of max: " + std::to_string(BoxLength);
	//		MessageBox(NULL, (LPCWSTR)msg.c_str(), L"Rapid-I", MB_TOPMOST);
			return 1000;
		}
		catch (...)
		{
			return -1000;
		}
	}

	bool EdgeDetectionSingleChannel::IncrementPositionAngularRect(int* X, int* Y, double SinTh, double CosTh, Pt OriginPt, struct Pt CurrentLocation, int ShiftBy)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			*X = OriginPt.X + (CurrentLocation.X + ShiftBy) * CosTh + CurrentLocation.Y * SinTh;
			*Y = OriginPt.Y + (CurrentLocation.X + ShiftBy) * SinTh - CurrentLocation.Y * CosTh;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}


	bool EdgeDetectionSingleChannel::AddPointToList(int* AxisPosition, struct Pt thisPt, std::list<struct Pt> *PointsList, bool ScanPositiveDir, struct Pt TravelRefPt)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		GetEdgePointAngularPosition(&thisPt, TravelRefPt);
		Pt LastPt = *(PointsList->end()--);
		double angleDelta = thisPt.ScanAnglePosition - LastPt.ScanAnglePosition;
		if (ScanPositiveDir)
		{
			(*AxisPosition)--;
			//If we got a point in the opposite direction. So let us quit and continue....
			if (angleDelta < 0) return false;
		}
		else
		{
			(*AxisPosition)++;
			if (angleDelta > 0) return false;
		}
		//We got an edgepoint. Lets add it
		PointsList->push_back(thisPt);
		////Increment according to 
		//if (ScanInPositiveDir) Y--;
		//else Y++;
		return true;
	}

	
	//This function is similar to the above function, except that it operates within a boundry of a curved box
//	bool EdgeDetectionSingleChannel::TraverseEdgeCurvedBox(double *Box, std::list<struct Pt> *PointsList, ScanDirection SDir)
//	{
//		//try
//		//{
//		//	int X, Y, XPrevious, YPrevious;
//		//	Pt OriginPoint, PrvToPrvPt, PtAtOtherEnd;
//		//	bool TraverseDirection = true, EdgeLoopBack = false;
//		//	double R1, R2, Theta1, Theta2;
//
//		//	OriginPoint.X = Box[0];
//		//	OriginPoint.Y = Box[1];
//		//	R1 = Box[2];
//		//	R2 = Box[3];
//		//	Theta1 = Box[4];
//		//	Theta2 = Box[5];
//		//	
//		//	X = PointsList->begin()->X + 0.5;
//		//	Y = PointsList->begin()->Y + 0.5;
//		//	XPrevious = 0;
//		//	YPrevious = 0;
//		//	PrvToPrvPt.X = 0;
//		//	PrvToPrvPt.Y = 0;
//		//	PtAtOtherEnd.X = X;
//		//	PtAtOtherEnd.Y = Y;
//
//		//	//if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
//		//	//else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
//		//	//else if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
//		//	//else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
//		//	//else return false;
//		//	int sd = 1;
//		//	bool DoHorizontalScan = GetScanDirectionforCurvedBox(X, Y, OriginPoint);
//
//		//	if (DoHorizontalScan) // SDir == Rightwards || SDir == Leftwards)
//		//	{
//		//		if (SDir == Leftwards) sd = -1;
//		//		if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
//		//		else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
//		//		else return false;
//		//	}
//		//	else
//		//	{
//		//		if (SDir == Upwards) sd = -1;
//		//		if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
//		//		else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
//		//		else return false;
//		//	}
//
//		//	int MaxPtsPossible = ImageWidth * ImageHeight / EdgeFinderPixelRange; //We will consume a minimum of Edge thickness number of pixels for one edge pt
//
//		//	for(int DirectionCount = 0; DirectionCount < 2; DirectionCount++)
//		//	{
//		//		int PointsCount = 0;
//		//		while(CheckPointWithinCurvedBox(X, Y, Theta1, Theta2, R1, R2, OriginPoint))
//		//		{
//		//			PointsCount++;
//		//			if(PointsCount > MaxPtsPossible)
//		//				break;
//		//			DoHorizontalScan = GetScanDirectionforCurvedBox(X, Y, OriginPoint);
//		//			if (DoHorizontalScan)
//		//			{
//		//				if (YPrevious == Y)
//		//				{
//		//					if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//						continue;
//		//					else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//						continue;
//		//				}
//		//				else
//		//				{
//		//					if (YPrevious < Y) // == Y - 1)
//		//					{
//		//						if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//							continue;
//		//					}
//		//					else// if(YPrevious == Y + 1)
//		//					{
//		//						if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//							continue;
//		//					}
//		//				}
//		//				if (EdgeLoopBack) return true;
//		//			}
//		//			else
//		//			{
//		//				if (XPrevious == X)
//		//				{
//		//					if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//						continue;
//		//					else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//						continue;
//		//				}
//		//				else
//		//				{
//		//					if (XPrevious < X) // == X - 1)
//		//					{
//		//						if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//							continue;
//		//					}
//		//					else // if(XPrevious == X + 1)
//		//					{
//		//						if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
//		//							continue;
//		//					}
//		//				}
//		//				if (EdgeLoopBack) return true;
//		//			}
//
//		//			if (NoEdgePixelCount < 2 * EdgeFinderPixelRange)
//		//				continue;
//
//		//			Pt Pnt1, Pnt2;
//		//			std::list<Pt>::iterator Itr;
//		//			if(PointsList->size() >= 15)
//		//			{
//		//				if(TraverseDirection)
//		//				{
//		//					Itr = PointsList->end();
//		//					for(int j = 1; j<= 15; j++) Itr--;
//		//					
//		//					if(!DustSkip(*(--(PointsList->end())), *(Itr), &Pnt1, &Pnt2)) break;
//		//				}
//		//				else
//		//				{
//		//					Itr = PointsList->begin();
//		//					for(int j = 1; j<= 14; j++) Itr++;
//		//					
//		//					if(!DustSkip(*(PointsList->begin()), *(Itr), &Pnt1, &Pnt2)) break;
//		//				}
//		//			}
//		//			else
//		//			{
//		//				if(TraverseDirection)
//		//				{
//		//					if(!DustSkip(*(--(PointsList->end())), *(PointsList->begin()), &Pnt1, &Pnt2)) break;
//		//				}
//		//				else
//		//				{
//		//					if(!DustSkip(*(PointsList->begin()), *(--(PointsList->end())), &Pnt1, &Pnt2)) break;
//		//				}
//		//			}
//		//			
//		//			if(!CheckPointWithinCurvedBox(Pnt1.X, Pnt1.Y, Theta1, Theta2, R1, R2, OriginPoint)) break;
//		//			if(!CheckPointWithinCurvedBox(Pnt2.X, Pnt2.Y, Theta1, Theta2, R1, R2, OriginPoint)) break;
//		//			
//		//			if(TraverseDirection)
//		//			{
//		//				PointsList->push_back(Pnt1);
//		//				PointsList->push_back(Pnt2);
//		//			}
//		//			else
//		//			{
//		//				PointsList->push_front(Pnt1);
//		//				PointsList->push_front(Pnt2);
//		//			}
//		//			NoEdgePixelCount = 0;
//
//		//			X = Pnt2.X + 0.5;
//		//			Y = Pnt2.Y + 0.5;
//		//			XPrevious = Pnt1.X + 0.5;
//		//			YPrevious = Pnt1.Y + 0.5;
//		//			PrvToPrvPt.X = 0;
//		//			PrvToPrvPt.Y = 0;
//		//		}
//
//		//		if(DirectionCount == 0)
//		//		{
//		//			TraverseDirection = false;
//		//			EdgeLoopBack = false;
//		//			PtAtOtherEnd.X = (--(PointsList->end()))->X + 0.5;
//		//			PtAtOtherEnd.Y = (--(PointsList->end()))->Y + 0.5; 
//		//		
//		//			X = PointsList->begin()->X + 0.5;
//		//			Y = PointsList->begin()->Y + 0.5;
//		//			XPrevious = (++(PointsList->begin()))->X + 0.5;
//		//			YPrevious = (++(PointsList->begin()))->Y + 0.5;
//		//			PrvToPrvPt.X = 0;
//		//			PrvToPrvPt.Y = 0;
//		//		}
//		//	}
//		//	return true;
//		//}
//	//catch(...){ SetAndRaiseErrorMessage("EDSC0015", __FILE__, __FUNCSIG__); return false; }
//return true;
//
//	}

	bool EdgeDetectionSingleChannel::TraverseEdgeCurvedBox(double *Box, std::list<struct Pt> *PointsList)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int X, Y, XPrevious, YPrevious;
			Pt OriginPoint, PrvToPrvPt, PtAtOtherEnd;
			bool TraverseDirection = true, EdgeLoopBack = false;
			double R1, R2, Theta1, Theta2;

			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];

			X = PointsList->begin()->X + 0.5;
			Y = PointsList->begin()->Y + 0.5;
			XPrevious = 0;
			YPrevious = 0;
			PrvToPrvPt.X = 0;
			PrvToPrvPt.Y = 0;
			PtAtOtherEnd.X = X;
			PtAtOtherEnd.Y = Y;

			if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd));
			else return false;

			for (int DirectionCount = 0; DirectionCount < 2; DirectionCount++)
			{
				int PointsCount = 0;
				while (CheckPointWithinCurvedBox(X, Y, Theta1, Theta2, R1, R2, OriginPoint))
				{
					PointsCount++;
					if (PointsCount > 1500)
						break;
					if (YPrevious == Y)
					{
						if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
							continue;
						else if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
							continue;
					}
					else
					{
						if (YPrevious == Y - 1)
						{
							if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
								continue;
						}
						else if (YPrevious == Y + 1)
						{
							if (ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
								continue;
						}
					}
					if (EdgeLoopBack) return true;

					if (XPrevious == X)
					{
						if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
							continue;
						else if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
							continue;
					}
					else
					{
						if (XPrevious == X - 1)
						{
							if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
								continue;
						}
						else if (XPrevious == X + 1)
						{
							if (ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd))
								continue;
						}
					}
					if (EdgeLoopBack) return true;

					Pt Pnt1, Pnt2;
					std::list<Pt>::iterator Itr;
					if (PointsList->size() >= 15)
					{
						if (TraverseDirection)
						{
							Itr = PointsList->end();
							for (int j = 1; j <= 15; j++) Itr--;

							if (!DustSkip(*(--(PointsList->end())), *(Itr), &Pnt1, &Pnt2)) break;
						}
						else
						{
							Itr = PointsList->begin();
							for (int j = 1; j <= 14; j++) Itr++;

							if (!DustSkip(*(PointsList->begin()), *(Itr), &Pnt1, &Pnt2)) break;
						}
					}
					else
					{
						if (TraverseDirection)
						{
							if (!DustSkip(*(--(PointsList->end())), *(PointsList->begin()), &Pnt1, &Pnt2)) break;
						}
						else
						{
							if (!DustSkip(*(PointsList->begin()), *(--(PointsList->end())), &Pnt1, &Pnt2)) break;
						}
					}

					if (!CheckPointWithinCurvedBox(Pnt1.X, Pnt1.Y, Theta1, Theta2, R1, R2, OriginPoint)) break;
					if (!CheckPointWithinCurvedBox(Pnt2.X, Pnt2.Y, Theta1, Theta2, R1, R2, OriginPoint)) break;

					if (TraverseDirection)
					{
						PointsList->push_back(Pnt1);
						PointsList->push_back(Pnt2);
					}
					else
					{
						PointsList->push_front(Pnt1);
						PointsList->push_front(Pnt2);
					}

					X = Pnt2.X + 0.5;
					Y = Pnt2.Y + 0.5;
					XPrevious = Pnt1.X + 0.5;
					YPrevious = Pnt1.Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}

				if (DirectionCount == 0)
				{
					TraverseDirection = false;
					EdgeLoopBack = false;
					PtAtOtherEnd.X = (--(PointsList->end()))->X + 0.5;
					PtAtOtherEnd.Y = (--(PointsList->end()))->Y + 0.5;

					X = PointsList->begin()->X + 0.5;
					Y = PointsList->begin()->Y + 0.5;
					XPrevious = (++(PointsList->begin()))->X + 0.5;
					YPrevious = (++(PointsList->begin()))->Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}
			}
			return true;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0015", __FILE__, __FUNCSIG__); return false; }
	}

	
	bool EdgeDetectionSingleChannel::TraverseEdgeCurvedBox1(double *Box, std::list<struct Pt> *PointsList, bool OppScan)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int X, Y, EdgeDetectedType = 0, PtCounter = 0, EdgePtPositionFlag = 0, BoxLength = 0;
			Pt PtAtOtherEnd,  CurrentEdgePt, P1, currScanPosition, prevScanPosition, OriginPt; //StPt, EndPt,
			bool EdgeLoopBack = false;
			bool DoHorizontalScan = true, DoVerticalScan = false, DefaultVerticalScan = false;
			int horizontal_ScanDir = 1, SearchBoxSize = 20;

			double R1, R2, Theta1, Theta2;

			OriginPt.X = Box[0];
			OriginPt.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];
			//MessageBox(NULL, L"Entered Curved Scan", L"Rapid-I", MB_TOPMOST);

			X = PointsList->begin()->X;
			Y = PointsList->begin()->Y;
			PtAtOtherEnd.X = X; P1.X = X;
			PtAtOtherEnd.Y = Y; P1.Y = Y;
			CurrentEdgePt.X = X; CurrentEdgePt.Y = Y;
			NoEdgePixelCount = 0;

			ScanDirection SDir = (ScanDirection)GetScanDirectionforCurvedBox(X, Y, OriginPt, OppScan);
			
			int currentPosition = 0;
			int MaxPtsNumber = 2 * R2 * (Theta2 - Theta1);
			int inc = 0;

			//Ensure we are within the box + plus get transformed coordinate to start scan correctly....
			for (int jj = 0; jj < SearchBoxSize; jj++)
			{

				currentPosition = CheckPointWithinCurvedBox2(X, Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition);
				switch (currentPosition)
				{
				case 0:
					//We got a proper point within the box. So lets move to the next step
					jj = 100;
					break;
				case -1:
					while (currScanPosition.Y - Theta1 > (-SearchBoxSize / currScanPosition.X))
					{
						IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
						currentPosition = CheckPointWithinCurvedBox2(X, Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition);
						inc++;
						if (inc > MaxPtsNumber) 
						{
							//MessageBox(NULL, L"Could not get a start point within box", L"Rapid-I", MB_TOPMOST);
							return false;
						}
						//currScanPosition.Y += 1 / currScanPosition.X;
						//X = OriginPt.X + currScanPosition.X * cos(currScanPosition.Y); // +1 / currScanPosition.X);
						//Y = OriginPt.Y - currScanPosition.X * sin(currScanPosition.Y); // +1 / currScanPosition.X);
					}
					break;

				case 1:


				default:
					//MessageBox(NULL, L"Did not start Scan Cycle", L"Rapid-I", MB_TOPMOST);
					return false;
					break;
				}
			}

			bool IncrementPosition = false;	
			prevScanPosition = currScanPosition;

			Pt LastPt, EarlierPt; 
			std::list<Pt>::iterator Itr;
			int X_Dir = 0, Y_Dir = 0;
			int scannedEdgeType = -1000;
			int startrad = R1, endRad = R2;
			int scandir = 1;
			
			if (SDir == Downwards || SDir == Upwards)
			{
				if (currScanPosition.Y > M_PI && currScanPosition.Y < M_PI * 2) X_Dir = 1;
				else X_Dir = -1;
			}
			else
			{
				if ((currScanPosition.Y > 0 && currScanPosition.Y < M_PI_2) ||
					(currScanPosition.Y > 3 * M_PI_2 && currScanPosition.Y < 5 * M_PI_2)) Y_Dir = 1;
				else Y_Dir = -1;
			}
			IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);

			//We go uniformly from starting point to end point. 
			for (currentPosition = 0; currentPosition < MaxPtsNumber; currentPosition++)
			{
				if (abs(X) > ImageWidth || abs(Y) > ImageHeight)
				{
					//MessageBox(NULL, L"current position is outside image !", L"Rapid-I", MB_TOPMOST);
					return true;
				}
				if (currScanPosition.Y < Theta1)
				{
					//DoubleEdgeCt++;
					if (PointsList->size() > 0)
					{
						Pt LastPt = *(--PointsList->end());
						X = LastPt.X; Y = LastPt.Y;
						//int inc = 1;
						while (CheckPointWithinCurvedBox2(X, Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition) == 0)
						{
							IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
							if (currScanPosition.Y > prevScanPosition.Y) break;
						}
						//return true;
					}
					else
					{
						//MessageBox(NULL, L"Went in wrong direction without taking point", L"Rapid-I", MB_TOPMOST);
						return false;
					}
				}

				SDir = (ScanDirection)GetScanDirectionforCurvedBox(X, Y, OriginPt, OppScan);
				if (SDir == Rightwards || SDir == Leftwards)
					EdgeDetectedType = ScanHorizontalAndInsertToList(&X, Y, &CurrentEdgePt);
				else
					EdgeDetectedType = ScanVerticalAndInsertToList(X, &Y, &CurrentEdgePt);

				switch (EdgeDetectedType)
				{
				case 1: //We got an edge point
					//Check if its within the angular rect
					EdgePtPositionFlag = CheckPointWithinCurvedBox2(X, Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition);
					switch (EdgePtPositionFlag)
					{
					case 0: //We are within the box
							//Check if we are back to first scanned pixel! We got back to starting edge point.
						if (abs(CurrentEdgePt.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(CurrentEdgePt.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
						{
							//MessageBox(NULL, L"Got same as start point !", L"Rapid-I", MB_TOPMOST);
							return true;
						}
						//Are we moving in positive direction within CurvedBox? 
						if (currScanPosition.Y > prevScanPosition.Y)
						{//We are OK. So let us add to the list
							PointsList->push_back(CurrentEdgePt);
							//currentPosition++;
							PtCounter++;
							prevScanPosition = currScanPosition;
							LastPt = CurrentEdgePt;
							Itr = --PointsList->end();
							//currScanPosition.Y += 1 / currScanPosition.X;
							if (PtCounter > 6)
							{
								PtCounter = 0;
								for (int j = 1; j <= 5; j++) Itr--;
								EarlierPt = *(Itr);
								X_Dir = (LastPt.X - EarlierPt.X);
								if (X_Dir != 0) X_Dir /= abs(X_Dir);
								Y_Dir = (LastPt.Y - EarlierPt.Y);
								if (Y_Dir != 0)	Y_Dir /= abs(Y_Dir);
							}
						}
						else
						{
							//if (currScanPosition.Y == prevScanPosition.Y)
							//currScanPosition.Y += 2 / currScanPosition.X;
							//else
							//{
								//Pt PrevToLastPt = *(Itr);

							switch (SDir)
							{
							case EdgeDetectionSingleChannel::Rightwards:
							case EdgeDetectionSingleChannel::Leftwards:
								//if (Y_Dir == 0)
								//	IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
								//else
								Y += Y_Dir;
								break;
							case EdgeDetectionSingleChannel::Downwards:
							case EdgeDetectionSingleChannel::Upwards:
								//if (X_Dir == 0)
								//	IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
								//else
								X += X_Dir;
								break;
							default:
								break;
							}
							continue;
							//}
						}
						//Now increment Y axis to next row...
						//Y = CurrentEdgePt.Y + horizontal_ScanDir; X = CurrentEdgePt.X;
						IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
						//currentPosition--; //Ensure we do not lose a point because we are redoing the scan!
						break;

					case 1: //We have traveled outside the box length. so we are finished!!
						//MessageBox(NULL, L"Finished Scanning !", L"Rapid-I", MB_TOPMOST);
						return true;

					case -1: //We are to the left of the box so traveling in wrong direction. Let us see search for the last good point.
							 //First check if last good point is within the boundary buffer of right edge ... 

							//Let us increment in Y axis and try again....
							//Y += horizontal_ScanDir;
						//IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
						//currentPosition--;
						//}
						//break;
					case 2: //We are outside the box
					case -2:
					default:
						//return true;
						IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
						goto SearchForEdge;
						//break;
						break;
					}
					break;

				case -2: //We got two gradients within our scan area. So we skip this row
					//Y += horizontal_ScanDir;
					IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
					goto SearchForEdge;
					//currentPosition--;
					break;

				case -1: //Couldnt find an edge. So let us do dust skip and try again. 
			SearchForEdge:
					IncrementPosition = false;

					//if (GetEdgePtNearGivenPtCurvedBox(currScanPosition.X, currScanPosition.Y, SearchBoxSize, SearchBoxSize, &CurrentEdgePt, Box, horizontal_ScanDir, OppScan))
					//{
					//	if (CheckPointWithinCurvedBox2(CurrentEdgePt.X, CurrentEdgePt.Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition) == 0)
					//	{
					//		if (currScanPosition.Y > prevScanPosition.Y)
					//		{
					//			//we got a good point. Let us add to the collection and proceed forward...
					//			PointsList->push_back(CurrentEdgePt);
					//			//X = CurrentEdgePt.X; Y = CurrentEdgePt.Y + horizontal_ScanDir;
					//			prevScanPosition = currScanPosition;
					//			IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
					//			//currentPosition++; // = currScanPosition.X - 1;
					//			LastPt = CurrentEdgePt;
					//			Itr = --PointsList->end();
					//			if (PointsList->size() > 6)
					//			{
					//				for (int j = 1; j <= 5; j++) Itr--;
					//				EarlierPt = *(Itr);
					//				X_Dir = (LastPt.X - EarlierPt.X);
					//				if (X_Dir != 0) X_Dir /= abs(X_Dir);
					//				Y_Dir = (LastPt.Y - EarlierPt.Y);
					//				if (Y_Dir != 0)	Y_Dir /= abs(Y_Dir);
					//			}
					//		}
					//		else
					//		{
					//			currScanPosition.Y += SearchBoxSize / (2 * currScanPosition.X);
					//			IncrementPosition = true;
					//		}
					//	}
					//	else
					//	{								//return true;
					//		//currScanPosition.Y += 1 / currScanPosition.X;
					//		IncrementPosition = true;
					//	}
					//}
					//else
					//{
						//SearchBoxSize += 10;
						//Y += horizontal_ScanDir;
						if (OppScan)
						{
							startrad = R2; endRad = R1; scandir = -1;
						}
						currScanPosition.X = startrad;
						scannedEdgeType = -1000;
						while (abs(currScanPosition.X - endRad) > SearchBoxSize)
						{
							currScanPosition.X += scandir * (SearchBoxSize / 2);
							scannedEdgeType = GetEdgePtNearGivenPtCurvedBox(currScanPosition.X, currScanPosition.Y, SearchBoxSize, SearchBoxSize, &CurrentEdgePt, Box, horizontal_ScanDir, OppScan);
							if (scannedEdgeType == 0)
							{
								int rT = CheckPointWithinCurvedBox2(CurrentEdgePt.X, CurrentEdgePt.Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition);
								if (rT == 0)
								{
									if (currScanPosition.Y > prevScanPosition.Y)
									{
										//we got a good point. Let us add to the collection and proceed forward...
										PointsList->push_back(CurrentEdgePt);
										//X = CurrentEdgePt.X; Y = CurrentEdgePt.Y + horizontal_ScanDir;
										prevScanPosition = currScanPosition;
										IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
										//currentPosition++; // = currScanPosition.X - 1;
										LastPt = CurrentEdgePt;
										Itr = --PointsList->end();
										if (PointsList->size() > 6)
										{
											for (int j = 1; j <= 5; j++) Itr--;
											EarlierPt = *(Itr);
											X_Dir = (LastPt.X - EarlierPt.X);
											if (X_Dir != 0) X_Dir /= abs(X_Dir);
											Y_Dir = (LastPt.Y - EarlierPt.Y);
											if (Y_Dir != 0)	Y_Dir /= abs(Y_Dir);
										}
									}
									else
									{
										//X = LastPt.X, Y = LastPt.Y;
										CheckPointWithinCurvedBox2(X, Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition);
										//currScanPosition.Y += SearchBoxSize / (1 * currScanPosition.X);
										IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
									}
								}
								else if (rT == 1)
									return true;
								else if (rT == -1)
								{
									//currScanPosition.Y += SearchBoxSize / (1 * currScanPosition.X);
									if (PointsList->size() > 5)
									{
										Itr = --PointsList->end();
										EarlierPt = *(Itr);
										X = EarlierPt.X, Y = EarlierPt.Y;
										CheckPointWithinCurvedBox2(X, Y, Theta1, Theta2, R1, R2, OriginPt, &currScanPosition);
									}
									else
									{
										X = PtAtOtherEnd.X; Y = PtAtOtherEnd.Y;
									}
									IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition, 5);
								}
								else
									IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
							}
							else if (scannedEdgeType == 1)
								return true;
							//else continue
						}
						//currScanPosition.Y += SearchBoxSize / (1 * currScanPosition.X);
						IncrementPosition = true;
						if (currScanPosition.Y > 4 * M_PI)
						{
							//MessageBox(NULL, L"Current angular position went beyond 2.pi", L"Rapid-I", MB_TOPMOST);
							return true;
						}
						//if (SearchBoxSize > (R2 - R1) / 4) return true;
					//}
					if (IncrementPosition) IncrementCurvedBoxPosition(&X, &Y, OriginPt, &currScanPosition);
					break;

				case -10: //Error occurred during scan
				
					//MessageBox(NULL, L"Checking of point gave an error condition!", L"Rapid-I", MB_TOPMOST);
					return true;

				case -3: //We have a pixel gradient. We need to shift to the left and scan again.
					switch (SDir)	
					{
					case EdgeDetectionSingleChannel::Rightwards:
						X--;
						break;
					case EdgeDetectionSingleChannel::Downwards:
						Y--;
						break;
					case EdgeDetectionSingleChannel::Leftwards:
						X++;
						break;
					case EdgeDetectionSingleChannel::Upwards:
						Y--;
						break;
					default:
						break;
					}
					//currentPosition--; //Ensure we do not lose a point because we are redoing the scan!
					break;

				default:
					break;
				}
			}
			//wchar_t tt[40];
			//swprintf(tt, L"%d", currentPosition);
			//MessageBox(NULL, tt,  L"Rapid-I", MB_TOPMOST);

			//MessageBox(NULL, (LPCWSTR)msg.c_str(), L"Rapid-I", MB_TOPMOST);
		}
		catch (...)
		{
			//MessageBox(NULL, L"Got an error situation", L"Rapid-I", MB_TOPMOST);
			return false;
		}
	}

	bool EdgeDetectionSingleChannel::TraverseEdgeCurvedBox2(double *Box, std::list<struct Pt> *PointsList, bool OppScan)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int PtCounter = 0;// , incDir;
			Pt StPt, EndPt, OriginPt, BeginPt, PolarPt, cOriPt; //PtAtOtherEnd,
			int SearchBoxSize = 20;
			double R1, R2, Theta1, Theta2, BoxThetaSpan, R;
			//double startTheta, endTheta;
			//bool Scanin2Steps = false;
			int NumberofScanSteps = 1;
			ArcScanParams *currScanP;

			std::list<struct Pt> TempList;
			std::list<ArcScanParams> ScanStates;

			OriginPt.X = Box[0];
			OriginPt.Y = Box[1];
			cOriPt.X = OriginPt.X * this->CamSizeRatio;
			cOriPt.Y = OriginPt.Y * this->CamSizeRatio;
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];
			int RectBox[5] = { 0 };
			double ArcBox[6] = { 0 };
			for (int ii = 0; ii < 6; ii++) ArcBox[ii] = Box[ii];

			//MessageBox(NULL, L"Entered Curved Scan", L"Rapid-I", MB_TOPMOST);
			R = (R1 + R2) / 2;

			//BeginPt = *(PointsList->begin());

			ScanDirection SDir; 
			if (Theta1 < M_PI)
			{
				currScanP = new ArcScanParams(-1);
				currScanP->endAngle = Theta1; 
				if (Theta2 <= M_PI)
				{
					currScanP->stAngle = Theta2; ScanStates.push_back(*currScanP);
				}
				else if (Theta2 <= 2 * M_PI)
				{
					currScanP->stAngle = M_PI; ScanStates.push_back(*currScanP);
					currScanP = new ArcScanParams(1);
					currScanP->stAngle = M_PI;
					currScanP->endAngle = Theta2; ScanStates.push_back(*currScanP);
				}
				else
				{
					currScanP->stAngle = M_PI; ScanStates.push_back(*currScanP);
					currScanP = new ArcScanParams(1);
					currScanP->stAngle = M_PI;
					currScanP->endAngle = 2 * M_PI; ScanStates.push_back(*currScanP);
					currScanP = new ArcScanParams(-1);
					currScanP->stAngle = Theta2;
					currScanP->endAngle = 2 * M_PI; ScanStates.push_back(*currScanP);
				}
			}
			else
			{
				currScanP = new ArcScanParams(1);
				currScanP->stAngle = Theta1;
				if (Theta2 <= 2 * M_PI)
				{
					currScanP->endAngle = Theta2; ScanStates.push_back(*currScanP);
				}
				else if (Theta2 <= 3 * M_PI)
				{
					currScanP->endAngle = 2 * M_PI; ScanStates.push_back(*currScanP);
					currScanP = new ArcScanParams(-1);
					currScanP->stAngle = Theta2;
					currScanP->endAngle = 2 * M_PI; ScanStates.push_back(*currScanP);
				}
				else
				{
					currScanP->endAngle = 2 * M_PI; ScanStates.push_back(*currScanP);
					currScanP = new ArcScanParams(-1);
					currScanP->stAngle = 3 * M_PI;
					currScanP->endAngle = 2 * M_PI; ScanStates.push_back(*currScanP);
					currScanP = new ArcScanParams(1);
					currScanP->stAngle = 3 * M_PI;
					currScanP->endAngle = Theta2; ScanStates.push_back(*currScanP);
				}

				//startTheta = Theta1; incDir = 1;
				//if (Theta2 <= M_PI * 2)
				//	endTheta = Theta2;
				//else
				//	endTheta = 2 * M_PI; Scanin2Steps = true;
			}

			BoxThetaSpan = 2 * acos(1 - SearchBoxSize / (2 * R * this->CamSizeRatio));
			if (BoxThetaSpan > M_PI / 8.0) BoxThetaSpan = M_PI / 8.0;

			std::list<Pt>::iterator itr;
			std::list<ArcScanParams>::iterator paramItr;

			int ans = 0;
			RectBox[4] = abs(R2 - R1);// *this->CamSizeRatio;
			ArcScanParams thisScanparam;
			double endAng = 0;
			//TempList.push_back(BeginPt);
			////This will do the whole thing except for the last section that is a multiple of boxthetaspan to theta2
			//for (double ctheta = Theta1 + BoxThetaSpan; ctheta < Theta2 + BoxThetaSpan; ctheta += BoxThetaSpan)
			for (paramItr = ScanStates.begin(); paramItr != ScanStates.end(); paramItr++)
			{
				thisScanparam = *paramItr;
				double ctheta = thisScanparam.stAngle;
				if (thisScanparam.ScanDir == 1)
				{
					//for (double ctheta = thisScanparam.stAngle; ctheta < thisScanparam.endAngle - BoxThetaSpan; ctheta += BoxThetaSpan)
					//while (ctheta < thisScanparam.endAngle)
					for (ctheta = thisScanparam.stAngle; ctheta < thisScanparam.endAngle; ctheta += BoxThetaSpan) 
					{
						StPt.X = OriginPt.X + R * cos(ctheta); StPt.Y = OriginPt.Y - R * sin(ctheta); // + SearchBoxSize / R); // + SearchBoxSize / R
						endAng = ctheta + BoxThetaSpan;
						if (ctheta < 3 * M_PI_2 && endAng >  3 * M_PI_2)
						{
							endAng = 3 * M_PI_2;
							ctheta = 3 * M_PI_2 - BoxThetaSpan;
						}
						if (endAng > thisScanparam.endAngle) endAng = thisScanparam.endAngle;

						EndPt.X = OriginPt.X + R * cos(endAng); EndPt.Y = OriginPt.Y - R * sin(endAng);
						//if (StPt.X <= EndPt.X)
						//{
						RectBox[0] = StPt.X; RectBox[1] = StPt.Y; RectBox[2] = EndPt.X; RectBox[3] = EndPt.Y;
						//}
						//else
						//{
						//	RectBox[0] = EndPt.X; RectBox[1] = EndPt.Y; RectBox[2] = StPt.X; RectBox[3] = StPt.Y;
						//}
						//ctheta += BoxThetaSpan;

						if (GetSingleEdgePointAngularRect2(RectBox, OppScan, &BeginPt, &SDir))
						{
							TempList.push_front(BeginPt);

							//SDir = (ScanDirection)GetScanDirectionforCurvedBox(BeginPt.X, BeginPt.Y, OriginPt, OppScan);
							ans += TraverseEdgeAngularRect3(RectBox, &TempList, SDir, EndPt);

							for (itr = TempList.begin(); itr != TempList.end(); itr++)
							{
								//if (CheckPointWithinCurvedBox2((int)(itr->X), (int)(itr->Y), Theta1, Theta2, R1 * this->CamSizeRatio, R2 * this->CamSizeRatio, cOriPt, &PolarPt) == 0)
									PointsList->push_back(*itr);
							}
						}
						TempList.clear();
					}
				}
				else
				{
					
					//for (double ctheta = thisScanparam.stAngle; ctheta > thisScanparam.endAngle + BoxThetaSpan; ctheta -= BoxThetaSpan)
					//while (ctheta > thisScanparam.endAngle)
					for (ctheta = thisScanparam.stAngle; ctheta > thisScanparam.endAngle; ctheta-= BoxThetaSpan)
					{
						StPt.X = OriginPt.X + R * cos(ctheta); StPt.Y = OriginPt.Y - R * sin(ctheta); // - SearchBoxSize / R
						endAng = ctheta - BoxThetaSpan; 
						if (ctheta > M_PI_2 && endAng < M_PI_2)
						{
							endAng = M_PI_2; 
							ctheta = M_PI_2 + BoxThetaSpan;
						}
						if (endAng < thisScanparam.endAngle) endAng = thisScanparam.endAngle; //check for the last section. adjust for the remaining size
						EndPt.X = OriginPt.X + R * cos(endAng); EndPt.Y = OriginPt.Y - R * sin(endAng);
						RectBox[0] = StPt.X; RectBox[1] = StPt.Y; RectBox[2] = EndPt.X; RectBox[3] = EndPt.Y;
						//ctheta -= BoxThetaSpan;

						if (GetSingleEdgePointAngularRect2(RectBox, OppScan, &BeginPt, &SDir))
						{
							TempList.push_front(BeginPt);

							ans += TraverseEdgeAngularRect3(RectBox, &TempList, SDir, EndPt);
							int pointsnumber = TempList.size();
							for (itr = TempList.begin(); itr != TempList.end(); itr++)
							{
								//if (CheckPointWithinCurvedBox2((int)(itr->X), (int)(itr->Y), Theta1, Theta2, R1 * this->CamSizeRatio, R2 * this->CamSizeRatio, cOriPt, &PolarPt) == 0)
									PointsList->push_front(*itr);
							}
						}
						TempList.clear();
					}
				}
			}
			//Lets complete the same for the last section.
			//RectBox[0] = RectBox[2]; RectBox[1] = RectBox[3];
			//RectBox[2] = OriginPt.X + R * cos(Theta2); RectBox[3] = OriginPt.Y - R * sin(Theta2);
			//SDir = (ScanDirection)GetScanDirectionforCurvedBox(EndPt.X, EndPt.Y, OriginPt, OppScan);
			//int ans = TraverseEdgeAngularRect3(RectBox, PointsList, SDir, PtAtOtherEnd);
			if (ans > 0) return true; else return false;
		}
		catch (...)
		{
			return false;
		}
	}


	void EdgeDetectionSingleChannel::IncrementCurvedBoxPosition(int *X, int *Y, Pt OriPt, Pt *cScanPos, int IncreaseBy)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		int xx = *X, yy = *Y;
		try
		{
			double curAngle = cScanPos->Y;
			while (xx == *X && yy == *Y)
			{
				curAngle += IncreaseBy / cScanPos->X;
				xx = OriPt.X + cScanPos->X * cos(curAngle);
				yy = OriPt.Y - cScanPos->X * sin(curAngle);
			}
			cScanPos->Y = curAngle;
		}
		catch (...)
		{
		}
		*X = xx; *Y = yy;
	}

	//--------------------------------------------------------------------------------------------------
	//the dust skip function which can be called by any 1 of the 3 traverse edge functions when it encounters some dust due to which it cannot traverse further
	//--------------------------------------------------------------------------------------------------

	//This function skips the dust/irregularities that is hindering the edge traversal(for all the 3 edge traversals)
	bool EdgeDetectionSingleChannel::DustSkip(Pt Point1, Pt Point2, Pt *Pnt1, Pt *Pnt2)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double pixels[40] = {0};
			int XIterator, YIterator,  LookAheadIndex, i;
			double var;
			struct Pt Point;
			bool OneEdgeDetected = false;
			int LeftBoundry, RightBoundry, TopBoundry, BottomBoundry, ScanLength = 20, ScanWidth = 15;
			int PixelArrayIndex;
			
			ScanLength = 2 * LookAheadLimit; // (int)(20.0 * (double)EdgeFinderPixelRange / 3.0);
			ScanWidth = (int)(5.0 * (double)EdgeFinderPixelRange);

			//if (x1-x2) lesser than (y1-y2), then we need to do horizontal scan)
			if(abs(Point1.X - Point2.X) < abs(Point1.Y - Point2.Y))
				//if y1 > y2, then we need to go from row to row in the downward direction
				//if (Point1.Y > Point2.Y)
				if (Point1.Y < Point2.Y)
				{
					LeftBoundry = Point1.X + 0.5 - ScanWidth / 2; RightBoundry = Point1.X + 0.5 + ScanWidth / 2;
					TopBoundry = Point1.Y + 1 + 0.5; BottomBoundry = Point1.Y + ScanLength + 0.5;
					if (LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for (YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator--)
						for (XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + EdgeFinderPixelRange)]) < Tolerance)
								continue;

							//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
							//for this purpose, first it must be determined till where this checking must be carried out
							if ((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
								//the checking can continue for 'look ahead' number of pixels
								LookAheadIndex = LookAheadLimit;
							else
								//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
								LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;

							//This is where the actual checking happens the range for which has been determined in the previous if statement
							for (i = EdgeFinderPixelRange; i <= LookAheadIndex; i++)
								if (abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									//its not a legitimate edge as there is no continuity of transition
									//hence jump to the point where the check failed and continue searching for edge from this point forward
									XIterator = XIterator + i - PixelSkipValue;
									goto last1;
								}

							//copy the 4 pixels where edge has been detected into 'pixels' array
							for (int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[PixelArrayIndex + j];

							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);

							if (var == -1)
								goto last1;

							Point.X = XIterator + var;
							Point.Y = YIterator;
							if (!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else
							{
								*Pnt1 = Point;
								if (abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > EdgeFinderPixelRange - 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > EdgeFinderPixelRange - 1) return false;
								else return true;
							}

						last1:					i = 1;
						}
				}
						
				else
				//we need to go from row to row in the upward direction
				{
					LeftBoundry = Point1.X + 0.5 - ScanWidth / 2; RightBoundry = Point1.X + 0.5 + ScanWidth / 2;
					TopBoundry = Point1.Y - ScanLength + 0.5; BottomBoundry = Point1.Y - 1 + 0.5;
					if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
						for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + EdgeFinderPixelRange)]) < Tolerance)
								continue;

							//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
							//for this purpose, first it must be determined till where this checking must be carried out
							if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
								//the checking can continue for 'look ahead' number of pixels
								LookAheadIndex = LookAheadLimit;
							else
								//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
								LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
							//This is where the actual checking happens the range for which has been determined in the previous if statement
							for(i = EdgeFinderPixelRange; i <=  LookAheadIndex; i++)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									//its not a legitimate edge as there is no continuity of transition
									//hence jump to the point where the check failed and continue searching for edge from this point forward
									XIterator = XIterator + i - PixelSkipValue;
									goto last2;
								}

							//copy the 4 pixels where edge has been detected into 'pixels' array
							for(int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[PixelArrayIndex + j];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							if(var == -1) 
								goto last2;
						
							Point.X = XIterator + var;
							Point.Y = YIterator;
							if(!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else
							{
								*Pnt1 = Point;
								if(abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > EdgeFinderPixelRange - 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > EdgeFinderPixelRange - 1) return false;
								else return true;
							}
												
	last2:					i = 1;
						}
				}
			else
			//Vertical scan
				if(Point1.X < Point2.X)
				//if x1 > x2, then we need to go from column to column in the rightward direction
				{
					LeftBoundry = Point1.X + 0.5 + 1; RightBoundry = Point1.X + 0.5 + ScanLength;
					TopBoundry = Point1.Y - ScanWidth / 2 + 0.5; BottomBoundry = Point1.Y + ScanWidth / 2 + 0.5;
					if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for(XIterator = RightBoundry; XIterator >= LeftBoundry; XIterator--)
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + EdgeFinderPixelRange) * ImageWidth)]) < Tolerance)
								continue;

							if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
						
							for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									YIterator = YIterator + i /ImageWidth - PixelSkipValue;
									goto last3;
								}
							for(int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							if(var == -1) 
								goto last3;
						
							Point.X = XIterator;
							Point.Y = YIterator + var;
							if(!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else
							{
								*Pnt1 = Point;
								if(abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > EdgeFinderPixelRange - 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > EdgeFinderPixelRange - 1) return false;
								else return true;
							}

	last3:					i = 1;
						}
				}
				else
				{
					LeftBoundry = Point1.X + 0.5 - ScanLength; RightBoundry = Point1.X + 0.5 - 1;
					TopBoundry = Point1.Y - ScanWidth / 2 + 0.5; BottomBoundry = Point1.Y + ScanWidth / 2 + 0.5;
					if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + EdgeFinderPixelRange) * ImageWidth)]) < Tolerance)
								continue;

							if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
						
							for(i = EdgeFinderPixelRange * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									YIterator = YIterator + i / ImageWidth - PixelSkipValue;
									goto last4;
								}
							for(int j = 0; j <= EdgeFinderPixelRange; j++)
								pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							
							if(var == -1) 
								goto last4;
						
							Point.X = XIterator;
							Point.Y = YIterator + var;
							if(!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else
							{
								*Pnt1 = Point;
								if(abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > EdgeFinderPixelRange - 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > EdgeFinderPixelRange - 1) return false;
								else return true;
							}

	last4:					i = 1;
						}
				}
			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0016", __FILE__, __FUNCSIG__); return false; }
	}
	


	//-------------------------------------------------------------------------
	//The two functions used by the traverse edge functions. The 3rd function to pin point an edge within a span of 4 pixels using sub-pixeling
	//-------------------------------------------------------------------------

	//This function finds an edge point either on the top or the bottom row of the currently detected edge point
	//Then it checks whether the new point has already been detected, if so, it rejects that point
	//It returns true or false according to the detection
	bool EdgeDetectionSingleChannel::ScanHorizontalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool UpperRow, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd, int ScanDir)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double PinPointDist;
			int PinPointDistRounded;
			double pixels[40] = { 0 };
			struct Pt Point;
			int ShiftLeftCt = 0; 
			int dirSign = 1;
			int yindex = 0;
			int scd = 1;
			bool DoneBothSides = false, GotEdge = true;
			if (UpperRow) dirSign = -1;

			yindex = (*Y + dirSign) * ImageWidth;

			while (ShiftLeftCt < 2 * EdgeFinderPixelRange + 1)
			{
				for (int j = 0; j <= EdgeFinderPixelRange; j++)
					pixels[j] = iBytes[(*X - ShiftLeftCt * scd - (EdgeFinderPixelRange - 1) + j + yindex)];

				PinPointDist = PinPointEdge1(pixels);
				PinPointDistRounded = PinPointDist + 0.5;
				if (PinPointDist <= 0) // PinPointDistRounded == 0)
				{
					//*X -= 1; // ScanDir;
					ShiftLeftCt++;
					if (ShiftLeftCt == 2 * EdgeFinderPixelRange + 1)
					{
						if (DoneBothSides) break;
						ShiftLeftCt = 0; scd = -1;
						DoneBothSides = true;
					}
					//PinPointDistRounded = 1;
				}
				else
					break;
			}

			if (PinPointDist == -1) 
			{
				GotEdge = false;
				goto SetForNextScan;
			}

			if (abs(*X + PinPointDistRounded - PrvToPrvPt->X) < 2 && abs(*Y - PrvToPrvPt->Y) < 2) // EdgeFinderPixelRange - 1)
			{
				GotEdge = false;
				goto SetForNextScan;
			}

			Point.X = *X - 1 + PinPointDist;

			Point.Y = *Y + dirSign;

			//int GradPts[4];
			//GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
			int grad = pixels[0] - pixels[EdgeFinderPixelRange]; //iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];

			if (grad > 0)
			{
				if (*X > bd_factor[0])
					Point.direction = 1;
				else
					Point.direction = -1;
			}
			else
			{
				if (*X > bd_factor[0])
					Point.direction = -1;
				else
					Point.direction = 1;
			}

			if (abs(Point.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(Point.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
			{
				
				*EdgeLoopBack = true;
				return false;
			}

			if (TraverseDirection) PointsList->push_back(Point);
			else PointsList->push_front(Point);

		SetForNextScan:

			PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
			*XPrevious = *X; *YPrevious = *Y;

			*X = *X - 1 + PinPointDistRounded;
			*Y = *Y + dirSign;
			//If we did not get edge, return false. 
			if (!GotEdge)
			{
				NoEdgePixelCount++;
				return false;
			}

			NoEdgePixelCount = 0;
			return true;
			//}
			//else
			//{
			//	for(int j = 0; j <= EdgeFinderPixelRange; j++)
			//		pixels[j] = iBytes[(*X - (EdgeFinderPixelRange - 1) + j + (*Y + 1) * ImageWidth)];

			//	PinPointDist = PinPointEdge1(pixels);
			//	PinPointDistRounded = PinPointDist + 0.5;
			//	if (PinPointDistRounded == 0) PinPointDistRounded = 1;

			//	if(PinPointDist == -1) return false;
			//	if(abs(*X + PinPointDistRounded - PrvToPrvPt->X) < EdgeFinderPixelRange - 1 && abs(*Y - PrvToPrvPt->Y) < EdgeFinderPixelRange - 1) return false;

			//	Point.X = *X - 1 + PinPointDist;
			//	Point.Y = *Y + 1;
			//	
			//	int GradPts[4];
			//	GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
			//	int grad = pixels[0] - pixels[4];

			//	if (grad > 0)
			//	{
			//		if (*X > bd_factor[0])
			//			Point.direction = 1;
			//		else 
			//			Point.direction = -1;
			//	}
			//	else
			//	{
			//		if (*X > bd_factor[0])
			//			Point.direction = -1;
			//		else 
			//			Point.direction = 1;
			//	}

			//	if(abs(Point.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(Point.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
			//	{
			//		*EdgeLoopBack = true;
			//		return false;
			//	}

			//	if(TraverseDirection) PointsList->push_back(Point);
			//	else PointsList->push_front(Point);

			//	PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
			//	*XPrevious = *X; *YPrevious = *Y;
			//	*X = *X - 1 + PinPointDistRounded; *Y = *Y + 1;
			//	return true;
			//}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0017", __FILE__, __FUNCSIG__); return false; }
	}
	
	int EdgeDetectionSingleChannel::ScanHorizontalAndInsertToList(int *X, int Y, Pt *EdgePoint) //, bool *EdgeLoopBack, Pt PtAtOtherEnd)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double PinPointDist = -1;
			int PinPointDistRounded = 0;
			double pixels[40] = { 0 };
			//struct Pt Point;
			int ShiftLeftCt = 0;
			//int dirSign = 1;
			int currStartIndex = 0;
			int scd = 1;
			bool DoneBothSides = false, GotEdge = true;
			int shiftPos = 0;
			int gradUpCt = 0, gradDownCt = 0, GotEdgeCt = 0;
			if (Y < 5) return -21;
			if (*X < 5) return -20;

			currStartIndex = Y * ImageWidth + *X - EdgeFinderPixelRange;

			for (int j = 0; j <= 2 * EdgeFinderPixelRange + 1; j++)
			{
				pixels[j] = iBytes[j + currStartIndex];
				//Check for island pixel. If its there, simply fail this row
				if (j > 0)
				{
					if (pixels[j] - pixels[j - 1] > Tolerance)
						gradUpCt++;
					else if (pixels[j - 1] - pixels[j] > Tolerance)
						gradDownCt++;
				}
			}

			if (gradUpCt == 0 && gradDownCt == 0)
			{
				GotEdge = false;
				goto SetForNextScan;
			}

			if ((gradUpCt == 1 && gradDownCt == 0) || (gradUpCt == 0 && gradDownCt == 1))
			{
				//We do not have an island pixel. So let us get the actual pixel position
				for (shiftPos = 0; shiftPos <= EdgeFinderPixelRange; shiftPos++)
				{
					PinPointDist = PinPointEdge1(&pixels[shiftPos]);
					PinPointDistRounded = PinPointDist + 0.5;
					if (PinPointDist > -1.0) GotEdgeCt++; //We get a one pixel edge. so let us show that we need to scan again by moving to the left...
					//We got an edge pixel. So lets move on...
					if (PinPointDist > 0 && PinPointDist < EdgeFinderPixelRange - 1.5) break;
				}
			}
			else
				return -2;
			
			if (PinPointDist == -1)
			{
			//We did not get a pixel. 
				GotEdge = false;
				goto SetForNextScan;
			}

			EdgePoint->X = *X + PinPointDist + 2 + shiftPos - EdgeFinderPixelRange;
			EdgePoint->Y = Y;
			*X += PinPointDistRounded + 2 + shiftPos - EdgeFinderPixelRange;

			//int GradPts[4];
			//GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
			int grad = pixels[shiftPos] - pixels[EdgeFinderPixelRange + shiftPos]; //iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];
			if (grad > 0)
			{
				if (*X > bd_factor[0])
					EdgePoint->direction = 1;
				else
					EdgePoint->direction = -1;

			}
			else
			{
				if (*X > bd_factor[0])
					EdgePoint->direction = -1;
				else
					EdgePoint->direction = 1;
			}

			//if (TraverseDirection) PointsList->push_back(Point);
			//else PointsList->push_front(Point);

		SetForNextScan:


			//If we did not get edge, return false. 
			if (!GotEdge)
			{
				NoEdgePixelCount++;
				if (GotEdgeCt > 0)
				{

					return -3;
				}
				return -1;
			}
			NoEdgePixelCount = 0;
			return 1;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0017", __FILE__, __FUNCSIG__); return -10; }

	}


	int EdgeDetectionSingleChannel::ScanVerticalAndInsertToList(int X, int *Y, Pt *EdgePoint) //, bool *EdgeLoopBack, Pt PtAtOtherEnd)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double PinPointDist = -1;
			int PinPointDistRounded;
			double pixels[40] = { 0 };
			//struct Pt Point;
			int ShiftLeftCt = 0;
			//int dirSign = 1;
			int currStartIndex = 0;
			int scd = 1;
			bool DoneBothSides = false, GotEdge = true;
			int shiftPos = 0, gradUpCt = 0, gradDownCt = 0, GotEdgeCt = 0;
			//if (UpperRow) dirSign = -1;

			currStartIndex = (*Y - EdgeFinderPixelRange) * ImageWidth + X;

			for (int j = 0; j <= 2 * EdgeFinderPixelRange + 1; j++)
			{
				pixels[j] = iBytes[j * ImageWidth + currStartIndex];
				if (j > 0)
				{
					if (pixels[j] - pixels[j - 1] > Tolerance)
						gradUpCt++;
					else if (pixels[j - 1] - pixels[j] > Tolerance)
						gradDownCt++;
				}
			}

			if (gradUpCt == 0 && gradDownCt == 0)
			{
				GotEdge = false;
				goto SetForNextScan;
			}

			if ((gradUpCt == 1 && gradDownCt == 0) || (gradUpCt == 0 && gradDownCt == 1))
			{

				for (shiftPos = 0; shiftPos <= EdgeFinderPixelRange; shiftPos++)
				{
					PinPointDist = PinPointEdge1(&pixels[shiftPos]);
					PinPointDistRounded = PinPointDist + 0.5;
					if (PinPointDist > -1.0) GotEdgeCt++;
					//We got an edge pixel. So lets move on...
					if (PinPointDist > 0 && PinPointDist < EdgeFinderPixelRange - 1.5) break;
				}
			}
			else
				return -2;


			if (PinPointDist == -1)
			{
				GotEdge = false;
				goto SetForNextScan;
			}

			EdgePoint->X = X;
			EdgePoint->Y = *Y + PinPointDist + 2 + shiftPos - EdgeFinderPixelRange;
			*Y += PinPointDistRounded + 2 + shiftPos - EdgeFinderPixelRange;

			//int GradPts[4];
			//GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
			int grad = pixels[0] - pixels[EdgeFinderPixelRange]; //iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];

			if (grad > 0)
			{
				if (X > bd_factor[0])
					EdgePoint->direction = 1;
				else
					EdgePoint->direction = -1;
			}
			else
			{
				if (X > bd_factor[0])
					EdgePoint->direction = -1;
				else
					EdgePoint->direction = 1;
			}

			//if (TraverseDirection) PointsList->push_back(Point);
			//else PointsList->push_front(Point);
			//PointsList->push_back(Point);

		SetForNextScan:


			//If we did not get edge, return false. 
			if (!GotEdge)
			{
				NoEdgePixelCount++;
				if (GotEdgeCt > 0)	return -3;
				return -1;
			}
			NoEdgePixelCount = 0;
			return 1;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0017", __FILE__, __FUNCSIG__); return false; }

	}



	//This function finds an edge point either on the left or the right column of the currently detected edge point
	//Then it checks whether the new point has already been detected, if so, it rejects that point
	//It returns true or false according to the detection
	bool EdgeDetectionSingleChannel::ScanVerticalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool LeftColumn, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd, int ScanDir)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double PinPointDist;
			int PinPointDistRounded;
			double pixels[5] = {0};
			struct Pt Point;

			int ShiftUpCt = 0;
			int dirSign = 1;
			int xindex = 0;
			int scd = 1;
			bool DoneBothSides = false, GotEdge = true;
			if (LeftColumn) dirSign = -1;

			xindex = *X + dirSign;

			while (ShiftUpCt <= 2 * EdgeFinderPixelRange + 1)
			{
				for (int j = 0; j <= EdgeFinderPixelRange; j++)
					pixels[j] = iBytes[(xindex + (*Y - ShiftUpCt * scd - (EdgeFinderPixelRange - 1) + j) * ImageWidth)];

				PinPointDist = PinPointEdge1(pixels);
				PinPointDistRounded = PinPointDist + 0.5;
				if ( PinPointDist<= 0) // PinPointDistRounded == 0)
				{
					//*Y -= 1; //ScanDir
					ShiftUpCt++;
					if (ShiftUpCt == 2 * EdgeFinderPixelRange + 1 && PinPointDist == -1)
					{
						if (DoneBothSides) break;
						ShiftUpCt = 0; scd = -1;
						DoneBothSides = true;
					}
				}
				else
					break;

			}

			if (PinPointDist == -1)
			{
				GotEdge = false;
				goto SetForNextScan;
				//return false;
			}
			if (abs(*X - PrvToPrvPt->X) < 2 && abs(*Y + PinPointDistRounded - PrvToPrvPt->Y) < 2) // return false; //
			{
				GotEdge = false;
				goto SetForNextScan;
			}
			Point.X = *X + dirSign;
			Point.Y = *Y - 1 + PinPointDist;

			int grad = pixels[0] - pixels[4];

			if (grad > 0)
			{
				if (*Y > bd_factor[1])
					Point.direction = 1;
				else
					Point.direction = -1;
			}
			else
			{
				if (*Y > bd_factor[1])
					Point.direction = -1;
				else
					Point.direction = 1;
			}

			if (abs(Point.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(Point.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
			{
				*EdgeLoopBack = true;
				return false;
			}

			if (TraverseDirection) PointsList->push_back(Point);
			else PointsList->push_front(Point);

		SetForNextScan:
			PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
			*XPrevious = *X; *YPrevious = *Y;
			*X = *X + dirSign; 
			*Y = *Y - 1 + PinPointDistRounded;

			//If we did not get edge, return false. 
			if (!GotEdge)
			{
				NoEdgePixelCount++;
				return false;
			}
			NoEdgePixelCount = 0;
			return true;


			//if(LeftColumn)
			//{
			//	/*for(int j = 0; j <= 3; j++)
			//		pixels[j] = iBytes[(*X - 1 + (*Y - 1 + j) * ImageWidth)];*/
			//	
			//	for(int j = 0; j <= EdgeFinderPixelRange; j++)
			//		pixels[j] = iBytes[(*X - 1 + (*Y - (EdgeFinderPixelRange - 1) + j) * ImageWidth)];

			//	PinPointDist = PinPointEdge1(pixels);
			//	PinPointDistRounded = PinPointDist + 0.5;
			//	if (PinPointDistRounded == 0) PinPointDistRounded = 1;

			//	if(PinPointDist == -1) return false;
			//	if(abs(*X - PrvToPrvPt->X) < 1 && abs(*Y + PinPointDistRounded - PrvToPrvPt->Y) < 1) return false; //EdgeFinderPixelRange - 1
			//
			//	Point.X = *X - 1;
			//	Point.Y = *Y - 1 + PinPointDist;
			//	//Point.direction = -3;
			//	//if (pixels[0] > pixels[3]) Point.direction = 3;
			//	
			//	int GradPts[4];
			//	GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
			//	//int grad = iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];
			//	int grad = pixels[0] - pixels[4];

			//	if (grad > 0)
			//	{
			//		if (*Y > bd_factor[1])
			//			Point.direction = 1;
			//		else 
			//			Point.direction = -1;
			//	}
			//	else
			//	{
			//		if (*Y > bd_factor[1])
			//			Point.direction = -1;
			//		else 
			//			Point.direction = 1;
			//	}
			//	
			//	//Check for radial change in white to black for correcting for beam divergence
			//	if (CorrectforBeamDivergence)
			//	{	
			//		
			//	}

			//	if(abs(Point.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(Point.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
			//	{
			//		*EdgeLoopBack = true;
			//		return false;
			//	}
			//	/*for each(Pt Pnt in *PointsList)
			//	{
			//		if(Pnt.X == Point.X && Pnt.Y == Point.Y)
			//		{
			//			*EdgeLoopBack = true;
			//			return false;
			//		}
			//	}*/
			//	if(TraverseDirection) PointsList->push_back(Point);
			//	else PointsList->push_front(Point);

			//	PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
			//	*XPrevious = *X; *YPrevious = *Y;
			//	*X = *X - 1; *Y = *Y - 1 + PinPointDistRounded;
			//	return true;
			//}
			//else
			//{
			//	/*for(int j = 0; j <= 3; j++)
			//		pixels[j] = iBytes[(*X + 1 + (*Y - 1 + j) * ImageWidth)];*/

			//	for(int j = 0; j <= EdgeFinderPixelRange; j++)
			//		pixels[j] = iBytes[(*X + 1 + (*Y - (EdgeFinderPixelRange - 1) + j) * ImageWidth)];

			//	PinPointDist = PinPointEdge1(pixels);
			//	PinPointDistRounded = PinPointDist + 0.5;
			//	if (PinPointDistRounded == 0) PinPointDistRounded = 1;

			//	if(PinPointDist == -1) return false;
			//	//it was *X + 1 instead of *X - 1. was that a bug all this time?
			//	if(abs(*X - PrvToPrvPt->X) < 1 && abs(*Y + PinPointDistRounded - PrvToPrvPt->Y) < 1) return false;

			//	Point.X = *X + 1;
			//	Point.Y = *Y - 1 + PinPointDist;
			//	//Point.direction = -4;
			//	//if (pixels[0] > pixels[3]) Point.direction = 4;
			//	int GradPts[4];
			//	GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
			//	//int grad = iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];
			//	int grad = pixels[0] - pixels[4];

			//	/*if (abs(grad) < Tolerance) 
			//		Point.direction = 0;
			//	else*/ 
			//	if (grad > 0)
			//	{
			//		if (*Y > bd_factor[1])
			//			Point.direction = 1;
			//		else 
			//			Point.direction = -1;
			//	}
			//	else
			//	{
			//		if (*Y > bd_factor[1])
			//			Point.direction = -1;
			//		else 
			//			Point.direction = 1;
			//	}

			//	if(abs(Point.X - PtAtOtherEnd.X) < EdgeFinderPixelRange && abs(Point.Y - PtAtOtherEnd.Y) < EdgeFinderPixelRange && PointsList->size() > LookAheadLimit)
			//	{
			//		*EdgeLoopBack = true;
			//		return false;
			//	}
			//	/*for each(Pt Pnt in *PointsList)
			//	{
			//		if(Pnt.X == Point.X && Pnt.Y == Point.Y)
			//		{
			//			*EdgeLoopBack = true;
			//			return false;
			//		}
			//	}*/
			//	if(TraverseDirection) PointsList->push_back(Point);
			//	else PointsList->push_front(Point);

			//	PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
			//	*XPrevious = *X; *YPrevious = *Y;
			//	*X = *X + 1; *Y = *Y - 1 + PinPointDistRounded;
			//	return true;
			//}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0018", __FILE__, __FUNCSIG__); return false; }
	}

	//This function finds an edge point either on the top or the bottom row of the currently detected edge point
	//Then it checks whether the new point has already been detected, if so, it rejects that point
	//It returns true or false according to the detection
	bool EdgeDetectionSingleChannel::ScanHorizontalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool UpperRow, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double PinPointDist;
			int PinPointDistRounded;
			double pixels[5] = { 0 };
			struct Pt Point;

			if (UpperRow)
			{
				/*for(int j = 0; j <= 3; j++)
				pixels[j] = iBytes[(*X - 1 + j + (*Y - 1) * ImageWidth)];*/

				for (int j = 0; j <= 4; j++)
					pixels[j] = iBytes[(*X - 2 + j + (*Y - 1) * ImageWidth)];

				PinPointDist = PinPointEdge1(pixels);
				PinPointDistRounded = PinPointDist + 0.5;

				if (PinPointDist == -1) return false;
				if (abs(*X - 1 + PinPointDistRounded - PrvToPrvPt->X) < 2 && abs(*Y - 1 - PrvToPrvPt->Y) < 2) return false;

				Point.X = *X - 1 + PinPointDist;
				Point.Y = *Y - 1;
				//Point.direction = -1;
				//if (pixels[0] > pixels[3]) Point.direction = 1;

				int GradPts[4];
				GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
				int grad = pixels[0] - pixels[4]; //iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];

												  /*if (abs(grad) < Tolerance)
												  Point.direction = 0;
												  else*/
				if (grad > 0)
				{
					if (*X > bd_factor[0])
						Point.direction = 1;
					else
						Point.direction = -1;
				}
				else
				{
					if (*X > bd_factor[0])
						Point.direction = -1;
					else
						Point.direction = 1;
				}

				if (abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
				{
					*EdgeLoopBack = true;
					return false;
				}
				/*for each(Pt Pnt in *PointsList)
				{
				if(Pnt.X == Point.X && Pnt.Y == Point.Y)
				{
				*EdgeLoopBack = true;
				return false;
				}
				}*/
				if (TraverseDirection) PointsList->push_back(Point);
				else PointsList->push_front(Point);

				PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
				*XPrevious = *X; *YPrevious = *Y;
				*X = *X - 1 + PinPointDistRounded; *Y = *Y - 1;
				return true;
			}
			else
			{
				/*for(int j = 0; j <= 3; j++)
				pixels[j] = iBytes[(*X - 1 + j + (*Y + 1) * ImageWidth)];*/

				for (int j = 0; j <= 4; j++)
					pixels[j] = iBytes[(*X - 2 + j + (*Y + 1) * ImageWidth)];

				PinPointDist = PinPointEdge1(pixels);
				PinPointDistRounded = PinPointDist + 0.5;

				if (PinPointDist == -1) return false;
				if (abs(*X - 1 + PinPointDistRounded - PrvToPrvPt->X) < 2 && abs(*Y + 1 - PrvToPrvPt->Y) < 2) return false;

				Point.X = *X - 1 + PinPointDist;
				Point.Y = *Y + 1;

				//if (pixels[0] > pixels[3]) Point.direction = 2;

				int GradPts[4];
				GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
				//int grad = iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];
				int grad = pixels[0] - pixels[4];

				///*if (abs(grad) < Tolerance) 
				//	Point.direction = 0;
				//else*/ if (grad > 0)
				//	Point.direction = 1;
				//else
				//	Point.direction = -1;

				if (grad > 0)
				{
					if (*X > bd_factor[0])
						Point.direction = 1;
					else
						Point.direction = -1;
				}
				else
				{
					if (*X > bd_factor[0])
						Point.direction = -1;
					else
						Point.direction = 1;
				}

				if (abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
				{
					*EdgeLoopBack = true;
					return false;
				}
				/*for each(Pt Pnt in *PointsList)
				{
				if(Pnt.X == Point.X && Pnt.Y == Point.Y)
				{
				*EdgeLoopBack = true;
				return false;
				}
				}*/
				if (TraverseDirection) PointsList->push_back(Point);
				else PointsList->push_front(Point);

				PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
				*XPrevious = *X; *YPrevious = *Y;
				*X = *X - 1 + PinPointDistRounded; *Y = *Y + 1;
				return true;
			}
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0017", __FILE__, __FUNCSIG__); return false; }
	}

	//This function finds an edge point either on the left or the right column of the currently detected edge point
	//Then it checks whether the new point has already been detected, if so, it rejects that point
	//It returns true or false according to the detection
	bool EdgeDetectionSingleChannel::ScanVerticalAndInsertToList(int *X, int *Y, int *XPrevious, int *YPrevious, struct Pt *PrvToPrvPt, bool LeftColumn, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double PinPointDist;
			int PinPointDistRounded;
			double pixels[5] = { 0 };
			struct Pt Point;

			if (LeftColumn)
			{
				/*for(int j = 0; j <= 3; j++)
				pixels[j] = iBytes[(*X - 1 + (*Y - 1 + j) * ImageWidth)];*/

				for (int j = 0; j <= 4; j++)
					pixels[j] = iBytes[(*X - 1 + (*Y - 2 + j) * ImageWidth)];

				PinPointDist = PinPointEdge1(pixels);
				PinPointDistRounded = PinPointDist + 0.5;

				if (PinPointDist == -1) return false;
				if (abs(*X - 1 - PrvToPrvPt->X) < 2 && abs(*Y - 1 + PinPointDistRounded - PrvToPrvPt->Y) < 2) return false;

				Point.X = *X - 1;
				Point.Y = *Y - 1 + PinPointDist;
				//Point.direction = -3;
				//if (pixels[0] > pixels[3]) Point.direction = 3;

				int GradPts[4];
				GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
				//int grad = iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];
				int grad = pixels[0] - pixels[4];

				if (grad > 0)
				{
					if (*Y > bd_factor[1])
						Point.direction = 1;
					else
						Point.direction = -1;
				}
				else
				{
					if (*Y > bd_factor[1])
						Point.direction = -1;
					else
						Point.direction = 1;
				}

				//Check for radial change in white to black for correcting for beam divergence
				if (CorrectforBeamDivergence)
				{

				}

				if (abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
				{
					*EdgeLoopBack = true;
					return false;
				}
				/*for each(Pt Pnt in *PointsList)
				{
				if(Pnt.X == Point.X && Pnt.Y == Point.Y)
				{
				*EdgeLoopBack = true;
				return false;
				}
				}*/
				if (TraverseDirection) PointsList->push_back(Point);
				else PointsList->push_front(Point);

				PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
				*XPrevious = *X; *YPrevious = *Y;
				*X = *X - 1; *Y = *Y - 1 + PinPointDistRounded;
				return true;
			}
			else
			{
				/*for(int j = 0; j <= 3; j++)
				pixels[j] = iBytes[(*X + 1 + (*Y - 1 + j) * ImageWidth)];*/

				for (int j = 0; j <= 4; j++)
					pixels[j] = iBytes[(*X + 1 + (*Y - 2 + j) * ImageWidth)];

				PinPointDist = PinPointEdge1(pixels);
				PinPointDistRounded = PinPointDist + 0.5;

				if (PinPointDist == -1) return false;
				if (abs(*X + 1 - PrvToPrvPt->X) < 2 && abs(*Y - 1 + PinPointDistRounded - PrvToPrvPt->Y) < 2) return false;

				Point.X = *X + 1;
				Point.Y = *Y - 1 + PinPointDist;
				//Point.direction = -4;
				//if (pixels[0] > pixels[3]) Point.direction = 4;
				int GradPts[4];
				GetRadialPts(Point.X, Point.Y, 2, &GradPts[0]);
				//int grad = iBytes[(GradPts[0] + (GradPts[1] - 1) * ImageWidth)] - iBytes[(GradPts[2] + (GradPts[3] - 1) * ImageWidth)];
				int grad = pixels[0] - pixels[4];

				/*if (abs(grad) < Tolerance)
				Point.direction = 0;
				else*/
				if (grad > 0)
				{
					if (*Y > bd_factor[1])
						Point.direction = 1;
					else
						Point.direction = -1;
				}
				else
				{
					if (*Y > bd_factor[1])
						Point.direction = -1;
					else
						Point.direction = 1;
				}

				if (abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
				{
					*EdgeLoopBack = true;
					return false;
				}
				/*for each(Pt Pnt in *PointsList)
				{
				if(Pnt.X == Point.X && Pnt.Y == Point.Y)
				{
				*EdgeLoopBack = true;
				return false;
				}
				}*/
				if (TraverseDirection) PointsList->push_back(Point);
				else PointsList->push_front(Point);

				PrvToPrvPt->X = *XPrevious; PrvToPrvPt->Y = *YPrevious;
				*XPrevious = *X; *YPrevious = *Y;
				*X = *X + 1; *Y = *Y - 1 + PinPointDistRounded;
				return true;
			}
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0018", __FILE__, __FUNCSIG__); return false; }
	}


	//This function receives an array of 4 pixels, finds the coordinate of the edge point and returns the distance(in pixel coordinates) of this point form the leftmost pixel
	//It returns any real value between -0.5 to 2.5 if edge is found, else it returns -1
	double EdgeDetectionSingleChannel::PinPointEdge(int x, int y, double *pixels)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int i,direction;
			//One other point to note is that this function always considers the middle byte value within a pixel for edge detection

			//if difference of pixel value of 1st pixel and 4th pixel less than 100, then its considered as the absence of any edge
			if(abs(pixels[0] - pixels[EdgeFinderPixelRange]) < 100) return -1.0;

			if(pixels[0] > pixels[EdgeFinderPixelRange]) direction = 1;
			//direction 1 means transition from bright to dark region else dark to bright region
			else direction = -1;
			
			for(i = 0;i <= EdgeFinderPixelRange - 1;i++)
			{
				//if the pixel value is between 230 & 60, then the edge is passing right through this pixel
				if(pixels[i] <= 230 && pixels[i] > 60) goto found;
				//if the differnce in values between 2 adjacent pixels is > 170, then the edge is considered to be between the 2 pixels
				if(abs(pixels[i] - pixels[(i + 1)]) > 170) goto found;
				//if (i > 1)
				//{
				//	if (abs(pixels[i] - pixels[(i - 2)]) > 170)
				//	{
				//		double anss = 0;
				//		if (direction == 1)
				//		{

				//		}
				//	}
				//}
			}
			return -1.0;
		found:
			double ans = 0;
			if(direction == 1)
			{
				//We do not consider the edge point if it is more than 3 pixels away
				if((i + (pixels[i] + pixels[(i + 1)]) / 255.0 - 0.5) > (double)EdgeFinderPixelRange - 0.5) 
					return -1.0; //2.5
				ans = (i + (pixels[i] + pixels[(i + 1)]) / 255.0 - 0.5);
				return ans;
			}

			if((i + 1.5 - (pixels[i] + pixels[(i + 1)]) / 255.0) > (double)EdgeFinderPixelRange - 0.5) 
				return -1.0; //2.5
			ans = (i + 1.5 - (pixels[i] + pixels[(i + 1)]) / 255.0);
			return ans;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0019", __FILE__, __FUNCSIG__); return -1; }
	}
	
	double EdgeDetectionSingleChannel::PinPointEdge1(double *pixels)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int i,direction;
			double ans;
			
			//if difference of pixel value of 1st pixel and 5th pixel less than 100, then its considered as the absence of any edge
			if(abs(pixels[0] - pixels[EdgeFinderPixelRange]) < 100) return -1.0;

			if(pixels[0] > pixels[EdgeFinderPixelRange]) direction = 1;
			//direction 1 means transition from bright to dark region else dark to bright region
			else direction = -1;
			
			for(i = 0; i <= EdgeFinderPixelRange; i++)
			{
				//if the pixel value is between 230 & 60, then the edge is passing right through this pixel
				if(pixels[i] <= 230 && pixels[i] > 60) goto found;
				//if the differnce in values between 2 adjacent pixels is > 170, then the edge is considered to be between the 2 pixels
				if(abs(pixels[i] - pixels[(i + 1)]) > 170) goto found;
			}
			return -1.0;
	found:
			if(direction == 1)
			{
				//We do not consider the edge point if it is more than 3 pixels away
				ans = (i + (pixels[i] + pixels[(i + 1)]) / 255.0 - 0.5);
				ans -= 1;
				if(ans >  (double)EdgeFinderPixelRange - 0.5) 
					return -1.0;
				if(ans < -0.67)  //O.5 denotes exactly a 255 to 0 transition in the first to second pixel itself. 0.6 means 230 to 0 transition.
					return -1.0;
				return ans;
			}
			ans = (i + 1.5 - (pixels[i] + pixels[(i + 1)]) / 255.0);
			ans -= 1;
			if(ans > (double)EdgeFinderPixelRange - 0.5) 
				return -1.0;
			if(ans < -0.67) 
				return -1.0;
			return ans;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0019", __FILE__, __FUNCSIG__); return -1; }
	}

	double EdgeDetectionSingleChannel::PinPointEdge2(double *pixels)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int i, direction;
			double ans;

			//if difference of pixel value of 1st pixel and 5th pixel less than 100, then its considered as the absence of any edge
			if (abs(pixels[0] - pixels[EdgeFinderPixelRange]) < 100) return -1.0;

			if (pixels[0] > pixels[EdgeFinderPixelRange]) direction = 1;
			//direction 1 means transition from bright to dark region else dark to bright region
			else direction = -1;

			for (i = 0; i <= EdgeFinderPixelRange; i++)
			{
				//if the pixel value is between 230 & 60, then the edge is passing right through this pixel
				if (pixels[i] <= 230 && pixels[i] > 60) goto found;
				//if the differnce in values between 2 adjacent pixels is > 170, then the edge is considered to be between the 2 pixels
				if (abs(pixels[i] - pixels[(i + 1)]) > 170) goto found;
			}
			return -1.0;
		found:
			if (direction == 1)
			{
				//We do not consider the edge point if it is more than 3 pixels away
				ans = (i + (pixels[i] + pixels[(i + 1)]) / 255.0 - 0.5);
				ans -= 1;
				if (ans >  (double)EdgeFinderPixelRange - 0.5)
					return -1.0;
				if (ans < -0.67)  //O.5 denotes exactly a 255 to 0 transition in the first to second pixel itself. 0.6 means 230 to 0 transition.
					return -1.0;
				return ans;
			}
			ans = (i + 1.5 - (pixels[i] + pixels[(i + 1)]) / 255.0);
			ans -= 1;
			if (ans >(double)EdgeFinderPixelRange - 0.5)
				return -1.0;
			if (ans < -0.67)
				return -1.0;
			return ans;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0019", __FILE__, __FUNCSIG__); return -1; }
	}


	//This function is the same for different framegrab functions
	int EdgeDetectionSingleChannel::AddEdgePtsToCollection(std::list<struct Pt> PtsList)
	{	
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		//Now the list has all the points as Pt structure
		//before sending it, we need to put all the values in a double array in X1 Y1 X2 Y2... fashion
		double shift = 0, dist = 0;	
		int i = 0;
		//bd_factor[0] = ImageWidth/2; bd_factor[1] = ImageHeight/2; bd_factor[2] = 0.0; 
		//bd_factor[3] = 0.01430769; bd_factor[4] = -6.403344; bd_factor[5] = -0.00000347; //0.0000005;//1452423; //4 = -1.8741044 ,,, -7.4964 ,, -8.0964 // 0.001425
		//bd_factor[3] = -0.0092410521 * 0.5; bd_factor[4] = 12.6620126789; bd_factor[5] = 0.0000017547 * 1.2; //0.0000005;//1452423; //4 = -1.8741044 ,,, -7.4964 ,, -8.0964 // 0.001425
		//bd_factor[3] = 3.3396; bd_factor[4] = 1.789; bd_factor[5] = 1200; //0.0000005;//1452423; //4 = -1.8741044 ,,, -7.4964 ,, -8.0964 // 0.001425
		//CorrectforBeamDivergence = false;

		if(PixelAveraged)
		{	//If we are doing averaging over many frames, ensure that we have low variance. Throw away edge points with higher variance
			for each(Pt Pnt in PtsList)
			{	
				if (PixelVariance[(int)(Pnt.X + 0.5) + ((int)(Pnt.Y + 0.5) * ImageWidth)] > VarianceCutOff)	continue;
				//Add the edge point
				DetectedPointsList[2 * i] = Pnt.X;		//X value in even position
				DetectedPointsList[2 * i + 1] = Pnt.Y;	//Y value in odd position
				//DetectedPointsList[2 * i + 1] = ImageHeight - 1 - Pnt.Y;	//Y value in odd position
				//For Oneshot, if we have to correct pixels due to beam divergence, do that here. 
				if (CorrectforBeamDivergence)
				{	//Get the distance of pixel from centre of beam. We have a central area of no divergence. 
					dist = sqrt(pow((Pnt.X - bd_factor[0]), 2) + pow((Pnt.Y - bd_factor[1]), 2)) - bd_factor[2]; //164.753;
					if (dist <= 0)
						shift = 0; // within the central no-divergence area. Hence dont do any correction
					else
						//shift = dist * bd_factor[3] + bd_factor[4];
						//shift = bd_factor[4] + bd_factor[3] * exp(-1 * (dist * dist) / (bd_factor[5] * bd_factor[5]));
					if (Pnt.direction < 0) shift *= -1; //Shift the pixel based on wether we are doing white-to-black or black to white. 
					if (Pnt.direction < 3)
					{
						DetectedPointsList[2 * i] += shift;
						//shift *= -1;
					}
					else
					{
						DetectedPointsList[2 * i + 1] += shift;
						//shift *= -1;
					}
				}
				i++;
				if(i == 10000) return 10000;		
			}
		}
		else
		{
			double angleofPt = 0;
			for each(Pt Pnt in PtsList)
			{	//Add the edge point
				//For Oneshot, if we have to correct pixels due to beam divergence, do that here. 
				if (CorrectforBeamDivergence)
				{	//Get the distance of pixel from centre of beam. We have a central area of no divergence. 
					double PolarXY[2];
					GetPolarCoordinates(Pnt.X, Pnt.Y, &PolarXY[0]);

					//if (PolarXY[0] <= bd_factor[2])
					//	shift = 0; // within the central no-divergence area. Hence dont do any correction
					//else
					//{
						//shift = PolarXY[0] * bd_factor[3] + bd_factor[4];
						//shift = PolarXY[0] * PolarXY[0] * bd_factor[5] + PolarXY[0] * bd_factor[3] + bd_factor[4];
					//shift = bd_factor[3] * exp(-1 * (PolarXY[0] * PolarXY[0]) / (bd_factor[5] * bd_factor[5]));
					//shift += bd_factor[2] * PolarXY[0] + bd_factor[4];
					shift = bd_factor[3] * (PolarXY[0] * PolarXY[0]) + bd_factor[2] * PolarXY[0] + bd_factor[4];
					//}

					//if (Pnt.direction < 0) shift *= -1; //Shift the pixel based on wether we are doing white-to-black or black to white. 
					//if (Pnt.direction > 1 || Pnt.direction < -1) Pnt.direction = 0;
					PolarXY[0] += shift;// * Pnt.direction;
					DetectedPointsList[2 * i] =  bd_factor[0] + PolarXY[0] * cos(PolarXY[1]);
					DetectedPointsList[2 * i + 1] = bd_factor[1] - PolarXY[0] * sin(PolarXY[1]);

				}
				else
				{
					DetectedPointsList[2 * i] = Pnt.X;		//X value in even position
					DetectedPointsList[2 * i + 1] = Pnt.Y;	//Y value in odd position
					//DetectedPointsList[2 * i + 1] = ImageHeight - 1 - Pnt.Y;	//Y value in odd position
				}
				i++;
				if(i == 10000) return 10000;		
			}
		}
		return i;
	}


	//------------------------------------------------------------------------------
	//The function to calculate the percentage of area of white within a box
	//------------------------------------------------------------------------------

	//This function calculates the area of white region within a specified rectangular region
	//It returns the area value in pixel coordinates
	double EdgeDetectionSingleChannel::CalculateWhiteAreaStraightRect(int *FrameGrabBox, double *TotalArea)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int Box[4] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3]};
			
			//Box[1] = ImageHeight - Box[1] - Box[3];

			int InitialX = Box[0], InitialY = Box[1], Width = Box[2], Height = Box[3], BlackCutOff = 35, WhiteCutOff = 245;
			double TotalWhiteArea = 0;
			int LeftBoundry = InitialX, RightBoundry = InitialX + Width - 1, TopBoundry = InitialY, BottomBoundry = InitialY + Height - 1; 
			
			//Go from pixel to pixel checking if it is white(or rather, above the threshold of black)
			//if so, add the ratio to the total sum
			for(int Y = TopBoundry; Y <= BottomBoundry; Y++)
				for(int X = LeftBoundry; X <= RightBoundry; X++)
					if(iBytes[(X + Y * ImageWidth)] > BlackCutOff)
					{
						if(iBytes[(X + Y * ImageWidth)] < WhiteCutOff)
							TotalWhiteArea += (iBytes[(X + Y * ImageWidth)]) / 255.0;
						else
							TotalWhiteArea += 1;
					}
			*TotalArea = Width * Height;       
			return TotalWhiteArea;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0020", __FILE__, __FUNCSIG__); return -1; }
	}
	
	//This function calculates the area of white region within a specified angular rect region
	//It returns the area value in pixel coordinates
	double EdgeDetectionSingleChannel::CalculateWhiteAreaAngularRect(int *FrameGrabBox, double *TotalArea)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "CalculateWhiteAreaAngularRect", __FUNCSIG__);
		try
		{
			//To accomplish this, we will calculate a straight rectangle that encompasses the angular rect
			//Then we shall iterate normally checking if each pixel is withinn the angular rect
			//if it is, consider it for area calculation
			int Box[5] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3], FrameGrabBox[4]};
			Pt Point1, Point2, RectCorners[4];
			int Width = Box[4], Height;
			double CosTh, SinTh, MaxX, MinX, MaxY, MinY;
			
			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if(Box[1] > Box[3])
			{	
				Point1.X = Box[0];
				Point1.Y = Box[1];
				Point2.X = Box[2];
				Point2.Y = Box[3];
			}
			else
			{
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}
			Height = sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2)); //height of the box is the dist between the 2 points

			CosTh = (Point2.X - Point1.X) / Height;
			SinTh = (Point1.Y - Point2.Y) / Height;

			//The left top corner of the rect is taken as the origin for the transformation
			RectCorners[0].X = Point2.X - Width / 2 * SinTh;
			RectCorners[0].Y = Point2.Y - Width / 2 * CosTh;
			RectCorners[1].X = Point2.X + Width / 2 * SinTh;
			RectCorners[1].Y = Point2.Y + Width / 2 * CosTh;
			RectCorners[2].X = Point1.X - Width / 2 * SinTh;
			RectCorners[2].Y = Point1.Y - Width / 2 * CosTh;
			RectCorners[3].X = Point1.X + Width / 2 * SinTh;
			RectCorners[3].Y = Point1.Y + Width / 2 * CosTh;

			MaxX = RectCorners[0].X; MinX = RectCorners[0].X;
			MaxY = RectCorners[0].Y; MinY = RectCorners[0].Y;
			for(int i = 1; i < 4; i++)
			{
				if(MaxX < RectCorners[i].X)
					MaxX = RectCorners[i].X;
				if(MinX > RectCorners[i].X)
					MinX = RectCorners[i].X;
				if(MaxY < RectCorners[i].Y)
					MaxY = RectCorners[i].Y;
				if(MinY > RectCorners[i].Y)
					MinY = RectCorners[i].Y;
			}

			int BlackCutOff = 35, WhiteCutOff = 245, TotalAreaCount = 0;
			double TotalWhiteArea = 0;
			
			//Go from pixel to pixel checking if it is white(or rather, above the threshold of black)
			//if so, add the ratio to the total sum
			for(int Y = MinY; Y <= MaxY; Y++)
				for(int X = MinX; X <= MaxX; X++)
					if(CheckPointWithinAngularRect(X, Y, SinTh, CosTh, RectCorners[0], Width, Height, false))
					{
						TotalAreaCount++;
						if(iBytes[(X + Y * ImageWidth)] > BlackCutOff)
						{
							if(iBytes[(X + Y * ImageWidth)] < WhiteCutOff)
								TotalWhiteArea += (iBytes[(X + Y * ImageWidth)]) / 255.0;
							else
								TotalWhiteArea += 1;
						}
					}
			*TotalArea = TotalAreaCount;       
			return TotalWhiteArea;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0020a", __FILE__, __FUNCSIG__); return -1; }
	}

	//This function calculates the area of white region within a specified curved box region
	//It returns the area value in pixel coordinates
	double EdgeDetectionSingleChannel::CalculateWhiteAreaCurvedBox(double *FrameGrabBox, double *TotalArea)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			//To accomplish this, we will calculate a straight rectangle that encompasses the Curved Box
			//Then we shall iterate normally checking if each pixel is withinn the Curved Box
			//if it is, consider it for area calculation
			int Box[6] = {FrameGrabBox[0], FrameGrabBox[1], FrameGrabBox[2], FrameGrabBox[3], FrameGrabBox[4], FrameGrabBox[5]};
			Pt OriginPoint;
			double R1, R2, Theta1, Theta2, MaxX, MinX, MaxY, MinY;

			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];

			MinX = OriginPoint.X - R2; if(MinX < 0) MinX = 0;
			MaxX = OriginPoint.X + R2; if(MaxX >= ImageWidth) MaxX = ImageWidth - 1;
			MinY = OriginPoint.Y - R2; if(MinY < 0) MinY = 0;
			MaxY = OriginPoint.Y + R2; if(MaxY >= ImageHeight) MaxY = ImageHeight - 1;

			int BlackCutOff = 35, WhiteCutOff = 245, TotalAreaCount = 0;
			double TotalWhiteArea = 0;
			
			//Go from pixel to pixel checking if it is white(or rather, above the threshold of black)
			//if so, add the ratio to the total sum
			for(int Y = MinY; Y <= MaxY; Y++)
				for(int X = MinX; X <= MaxX; X++)
					if(CheckPointWithinCurvedBox(X, Y, Theta1, Theta2, R1, R2, OriginPoint, false))
					{
						TotalAreaCount++;
						if(iBytes[(X + Y * ImageWidth)] > BlackCutOff)
						{
							if(iBytes[(X + Y * ImageWidth)] < WhiteCutOff)
								TotalWhiteArea += (iBytes[(X + Y * ImageWidth)]) / 255.0;
							else
								TotalWhiteArea += 1;
						}
					}
			*TotalArea = TotalAreaCount;       
			return TotalWhiteArea;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0020b", __FILE__, __FUNCSIG__); return -1; }
	}

	bool EdgeDetectionSingleChannel::CheckForBlankImage(int BinDegree)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int BinNumber = 1; 
			if (BinDegree > 7) BinDegree = 7;
			BinNumber = BinNumber << BinDegree;
			int binsize = 8 - BinDegree;
			int* Histogram = (int*)malloc(BinNumber * sizeof(int));
			int PixelBinValue = 0;
			//memset(Histogram, 0, BinNumber);
			for (int i = 0; i < BinNumber; i++)
				Histogram[i] = 0;
				//Go from pixel to pixel checking if it is white(or rather, above the threshold of black)
			//if so, add the ratio to the total sum
			for (int Y = 1; Y <= ImageHeight - 1; Y += 2)
				for (int X = 1; X <= ImageWidth - 1; X += 2)
				{
					PixelBinValue = ((int)iBytes[(X + Y * ImageWidth)]) >> binsize;
					Histogram[PixelBinValue] ++;
				}

			int Non_ZeroCount = 0;
			for (int i = 0; i < BinNumber; i++)
				if (Histogram[i] > 0) Non_ZeroCount++;
			if (Non_ZeroCount == 1)
				return true;
			else
				return false;
		}
		catch (...) { SetAndRaiseErrorMessage("EDSC0020", __FILE__, __FUNCSIG__); return false; }
	}


	
	//----------------------------------------------------------------------
	//the aurolab reference hole detection functions
	//----------------------------------------------------------------------

	//This function, with the help of Check Hole function, finds the small hole that is present on the Aurolab lens
	int EdgeDetectionSingleChannel::FindHole(ScanDirection Scan_Direction)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			Pt Point;    //Used to contain edge point coordinates
			int  LookAheadIndex;
			int XIterator, YIterator, i;
			double pixels[40] = {0};
			double var;
			std::list<Pt> PointsList; //To contain the list of all edge points
			bool TowardsRight = true;          //Scanning from middle to right side
			bool HoleFound = false;                    //True if the hole is found
			int NeglectHeight = 0;
			int LeftBoundry = 30 + BoundryBuffer;
			int RightBoundry = ImageWidth - 31 - BoundryBuffer;
			int TopBoundry = BoundryBuffer;
			int BottomBoundry = ImageHeight - 1 - BoundryBuffer;
			int PixelArrayIndex;

			/*if(Scan_Direction == Upwards) Scan_Direction = Downwards;
			else Scan_Direction = Upwards;*/

			XIterator = ImageWidth / 2;     //Lets start from the middle column..to increase our chances!!
	FindEdge:
			//This outer loop is used to move horizontally from 1 column to another looking for an edge point
			while(XIterator >= LeftBoundry && XIterator <= RightBoundry)
			{
				PointsList.clear();  //Every time a new search for an edge begins, discard the old edge points
				
				switch(Scan_Direction)
				{
			
					case Upwards:
						//In a particular column(defined by the outer loop) this inner loop scans from bottom to top for an edge
						for(YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator -= PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator - 3) * ImageWidth)]) < Tolerance)
								continue;

							//If an edge exists, validate it by checking continuity for the next few pixels
							//The following if statement determines the point till which to check 
							if((YIterator - LookAheadLimit) >= BoundryBuffer)
								//if the point is well inside the upper edge of the bmp picture
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								//else the point till where to check is the edge of picture itself
								LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;
							
							//The following statement actully checks the continuity till a point determined in the previous if statement 
							for(i = 4 * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex - i)]) < Tolerance)
								{
									YIterator = YIterator - i / ImageWidth + PixelSkipValue;
									goto last1;
								}

							//Copy 4 pixels that contains the edge
							for(int j = 0; j <= 3; j++)
							pixels[j] = iBytes[(XIterator + (YIterator - j) * ImageWidth)];

							//pin point the edge within that 4 pixels
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							
							if(var == -1) continue;
							
							Point.X = XIterator;
							Point.Y = YIterator - var;
							PointsList.push_back(Point);
							//Call this function to check if the edge is that of the hole that we are searching for
							HoleFound = CheckHole(&PointsList, &NeglectHeight, Scan_Direction);

							break;
	last1:					i = var + 0.5;//dummy statement to avoid compile time error
						}
						break;

					case Downwards:
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + 3) * ImageWidth)]) < Tolerance)
								continue;

							//If an edge exists, validate it by checking continuity for the next few pixels
							//The following if statement determines the point till which to check 
							if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
								//if the point is well inside the upper edge of the bmp picture
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								//else the point till where to check is the edge of picture itself
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
							
							//The following statement actully checks the continuity till a point determined in the previous if statement 
							for(i = 4 * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									YIterator = YIterator + i /ImageWidth - PixelSkipValue;
									goto last2;
								}

							//Copy 4 pixels that contains the edge
							for(int j = 0; j <= 3; j++)
							pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];

							//pin point the edge within that 4 pixels
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							
							if(var == -1) continue;
							
							Point.X = XIterator;
							Point.Y = YIterator + var;
							PointsList.push_back(Point);
							//Call this function to check if the edge is that of the hole that we are searching for
							HoleFound = CheckHole(&PointsList, &NeglectHeight, Scan_Direction);

							break;
	last2:					i = var + 0.5;
						}
						break;

					default:
						return 0;
				}

				if(HoleFound)
				{
					//This means that the hole has been detected, hence pass the list of edge points that form the hole
					i = 0;
					if(PixelAveraged)
					{
						for each(Pt Pnt in PointsList)
						{
							if(PixelVariance[(int)(Pnt.X + 0.5) + ((int)(Pnt.Y + 0.5) * ImageWidth)] <= VarianceCutOff)
							{
								DetectedPointsList[2 * i] = Pnt.X;		//X value in even position
								DetectedPointsList[2 * i + 1] = Pnt.Y;	//Y value in odd position
								//DetectedPointsList[2 * i + 1] = ImageHeight - 1 - Pnt.Y;	//Y value in odd position
								i++;
								if(i == 10000)
									return 10000;
							}
						}
					}
					else
					{
						for each(Pt Pnt in PointsList)
						{
							DetectedPointsList[2 * i] = Pnt.X;		//X value in even position
							DetectedPointsList[2 * i + 1] = Pnt.Y;	//Y value in odd position
							//DetectedPointsList[2 * i + 1] = ImageHeight - 1 - Pnt.Y;	//Y value in odd position
							i++;
							if(i == 10000)
								return 10000;				
						}
					}
					return i;
				}
				else
					//Else we need to continu scanning
					if(TowardsRight)
						//If scanning towards right, go rightwards 10 steps, we dont need details anyways! So lets not waste time by scanning evry column
						XIterator += 10;
					else
						//If scanning towards left, jump left 10 steps
						XIterator -= 10;                     
			}

			//We are out of the loop. Lets check if we have scanned in both the directions
			if(TowardsRight)
			{
				//We only have scanned from middle to right most. Time to scan leftwards from middle
				TowardsRight = false;
				XIterator = ImageWidth / 2 - 10;
				//Go back to scan one more time, but in the opposite direction
				goto FindEdge;
			}

			//After evrything if we dont find any hole, then forget it and return nothing which says -"Sory mate, i could'nt find the hole"
			return 0;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0021", __FILE__, __FUNCSIG__); return -1; }
	}
	
	//This function checks if the detected edge is infact the required hole or not
	//If it is, it gets all the edge points and returns true, else it returns false
	bool EdgeDetectionSingleChannel::CheckHole(std::list<Pt> *PointsList, int *NeglectHeight, ScanDirection Scan_Direction)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			Pt Point;
			Point.X = 0; Point.Y = 0;

			//If the new edge point is at the same height(+150) of the previously discarded edge, then we do not need this edge either
			if(*NeglectHeight != 0)
				if(Scan_Direction = Upwards)
				{
					if(PointsList->begin()->Y < *NeglectHeight + 150)
						return false;
				}
				else
				{
					if(PointsList->begin()->Y > *NeglectHeight - 150)
						return false;
				}
			
			int Box[4] = {0, 0, ImageWidth, ImageHeight};
			//Call this function to detect the edge through edge traversal method
			TraverseEdge(StraightRect, Box, PointsList);

			//If the number of edges detected is less than 160, then we are not interested
			if(PointsList->size() < 160)
				return false;
			
			//Lets get two points that should be approximately the two ends of a diameter of the hole
			Point = *(PointsList->begin());

			std::list<Pt>::iterator PtListItertr= PointsList->begin();
			for(int i = 0; i < 150; i++, PtListItertr++); 
			Pt Point2 = *PtListItertr;
			//If the distance between these two points are not within 90 and 110, then we do not consider it as the hole
			if(sqrt(pow((Point.X - Point2.X),2) + pow((Point.Y - Point2.Y),2)) > 90 && sqrt(pow((Point.X - Point2.X),2) + pow((Point.Y - Point2.Y),2)) < 110)
				//Lets return true saying that the hole has been found
				return true;
			else if(*NeglectHeight == 0)
			{
				//if the edge is longer than 160 pixels, but not the hole, but the lens edge itself, 
				//then we make sure that further edge points detected at this height is not traversed by setting this variable
				*NeglectHeight = PointsList->begin()->Y;
				switch(Scan_Direction)
				{
					case Upwards:
						//we get the Y value of the highest point(i.e. lowest Y value) as the neglect height
						for each(Pt Point1 in *PointsList)
							if(Point1.Y < *NeglectHeight)
								*NeglectHeight = Point1.Y;
							
					case Downwards:
						//we get the Y value of the lowest point(i.e. highest Y value) as the neglect height
						for each(Pt Point1 in *PointsList)
							if(Point1.Y > *NeglectHeight)
								*NeglectHeight = Point1.Y;
							
				}
			}

			//If all fails, then this is not the hole that we are looking for
			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0022", __FILE__, __FUNCSIG__); return false; }
	}
	


	//-----------------------------------------------------------------------
	//The following 2 functions to get a single edge point
	//-----------------------------------------------------------------------

	//This function is used to get one single edge point by scanning in a particuar specified direction in a rectangular area
	bool EdgeDetectionSingleChannel::GetEdgePoint(int *FrameGrabBox, ScanDirection Scan_Direction, bool SingleLineScan, double *DetectedPoint)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			Pt Point;
			int Box[4] = { FrameGrabBox[0], FrameGrabBox[1], FrameGrabBox[2], FrameGrabBox[3] };
			
			/*Box[1] = ImageHeight - Box[1] - Box[3];

			if(Scan_Direction == Upwards) Scan_Direction = Downwards;
			else if(Scan_Direction == Downwards) Scan_Direction = Upwards;*/
			
			//Call the private function GetSingleEdgePointStraightRect to get an edge in Pt structure format
			int itt = FrameGrabBox[3] / 2;
			if (Scan_Direction == ScanDirection::Downwards)
				itt = FrameGrabBox[2] / 2;

			for (int j = itt; j < itt * 2; j++)
			{
				if (Scan_Direction == ScanDirection::Rightwards)
					Box[1] = FrameGrabBox[1] + j - BoundryBuffer;
				else
					Box[0] = FrameGrabBox[0] + j - BoundryBuffer;

				if (GetSingleEdgePointStraightRect(Box, Scan_Direction, &Point, SingleLineScan))
				{
					//Convert this Pt structure into double array format
					DetectedPoint[0] = Point.X;
					//DetectedPoint[1] = ImageHeight - 1 -Point.Y;
					DetectedPoint[1] = Point.Y;
					return true;
				}
			}
			for (int j = itt; j > 0; j--)
			{
				if (Scan_Direction == ScanDirection::Rightwards)
					Box[1] = FrameGrabBox[1] + j - BoundryBuffer;
				else
					Box[0] = FrameGrabBox[0] + j - BoundryBuffer;

				if (GetSingleEdgePointStraightRect(Box, Scan_Direction, &Point, SingleLineScan))
				{
					//Convert this Pt structure into double array format
					DetectedPoint[0] = Point.X;
					//DetectedPoint[1] = ImageHeight - 1 -Point.Y;
					DetectedPoint[1] = Point.Y;
					return true;
				}
			}

			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0023", __FILE__, __FUNCSIG__); return false; }
	}
	
	//This functions scans in all directions untill it gets a single edge point(in a straight rect area)
	bool EdgeDetectionSingleChannel::GetEdgePoint(int *FrameGrabBox, bool SingleLineScan, double *DetectedPoint)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			Pt Point;
			int Box[4] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3]};

			//Box[1] = ImageHeight - Box[1] - Box[3];

			//Call the private function GetSingleEdgePointStraightRect to get an edge in Pt structure format
			if(GetSingleEdgePointStraightRect(Box, Rightwards, &Point, SingleLineScan));
			else if(GetSingleEdgePointStraightRect(Box, Leftwards, &Point, SingleLineScan));
			else if(GetSingleEdgePointStraightRect(Box, Upwards, &Point, SingleLineScan));
			else if(GetSingleEdgePointStraightRect(Box, Downwards, &Point, SingleLineScan));
			else return false;

			//Convert the detected Pt structure into double array format
			DetectedPoint[0] = Point.X;
			//DetectedPoint[1] = ImageHeight -1 -Point.Y;
			DetectedPoint[1] = Point.Y;
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0024", __FILE__, __FUNCSIG__); return false; }
	}
	


	//--------------------------------------------------------------------------------------------
	//The following 2 functions to detect an edge within a block and traverse along it. A future idea wherein the edge will be detected within the visinity of mouse position
	//--------------------------------------------------------------------------------------------

	//This function scans within a block of 20 * 20 pixels for an edge and traverses along it for the whole image
	int EdgeDetectionSingleChannel::DetectEdgeMouseClick(int X, int Y)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			struct Pt Point;
			std::list<struct Pt> PointsList;
			int i = 0;
			int Box[4] = {10, 10, ImageWidth / this->CamSizeRatio - 10, ImageHeight / this->CamSizeRatio - 10 };
			
			int ScanBoxSize = 120 * this->CamSizeRatio;
			//Box[0] = X * this->CamSizeRatio - ScanBoxSize;
			//Box[1] = Y * this->CamSizeRatio - ScanBoxSize;
			//Box[2] = 2 * ScanBoxSize;
			//Box[3] = 2 * ScanBoxSize;

			if (Box[0] < 0) Box[0] = 0;
			if (Box[1] < 0) Box[1] = 0;

			//Get one single point from where edge traversal can start
			//if(!GetSingleEdgePointInBlock(X, ImageHeight -1 -Y, &Point)) return 0;
			//if(!GetSingleEdgePointInBlock(X * this->CamSizeRatio, Y * this->CamSizeRatio, &Point)) return 0;
			
			if (!GetEdgePtNearGivenPtStraightRect_H(X * this->CamSizeRatio, Y * this->CamSizeRatio, Rightwards, ScanBoxSize, ScanBoxSize, &Point, Box, 1))  //  Y Box, Rightwards, &Point))
				if (!GetEdgePtNearGivenPtStraightRect_V(X * this->CamSizeRatio, Y * this->CamSizeRatio, Downwards, ScanBoxSize, ScanBoxSize, &Point, Box, 1))  //  Y Box, Rightwards, &Point))
					if (!GetEdgePtNearGivenPtStraightRect_H(X * this->CamSizeRatio, Y * this->CamSizeRatio, Leftwards, ScanBoxSize, ScanBoxSize, &Point, Box, 1))  //  Y Box, Rightwards, &Point))
						if (!GetEdgePtNearGivenPtStraightRect_V(X * this->CamSizeRatio, Y * this->CamSizeRatio, Upwards, ScanBoxSize, ScanBoxSize, &Point, Box, 1))  //  Y Box, Rightwards, &Point))
							return 0;

			PointsList.push_back(Point);	//Put this one point into the list
			//and call traverse edge function
			//if (!TraverseEdge(StraightRect, Box, &PointsList)) return 0;
			Box[0] = 0; Box[1] = 0; Box[2] = ImageWidth; Box[3] = ImageHeight;
			if (!TraverseEdgeStraightRect3(Box, &PointsList, Rightwards)) return 0;
			std::list<struct Pt> ReverseList;
			ReverseList.push_back(Point);	//Put this one point into the list

			if(!TraverseEdgeStraightRect3(Box, &ReverseList, Rightwards, true)) return 0;

			for each(Pt Pnt in ReverseList)
			{
				PointsList.push_front(Pnt);
			}
			//Merge the two lists now


			//Now put all the points in the linear array after correction if required...
			return AddEdgePtsToCollection(PointsList);			
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0025", __FILE__, __FUNCSIG__); return -1; }
	}

	//This function checks and gets one edge point within a block of specified size
	bool EdgeDetectionSingleChannel::GetSingleEdgePointInBlock(int X, int Y, Pt *DetectedPoint)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			double pixels[40] = {0};
			int XIterator, YIterator,  LookAheadIndex, i;
			double var;
			struct Pt Point;
			int BlockSize = 20 * this->CamSizeRatio;
			int LeftBoundry = X - BlockSize / 2;
			int RightBoundry = X + BlockSize / 2;
			int TopBoundry = Y - BlockSize / 2;
			int BottomBoundry = Y + BlockSize / 2;
			int PixelArrayIndex;

			if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
				return false;

			//Rightward Scanning starts here
			for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
			{
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += PixelSkipValue)
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[PixelArrayIndex] - iBytes[(PixelArrayIndex + 3)]) < Tolerance)
						continue;

					//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
					//for this purpose, first it must be determined till where this checking must be carried out
					if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
						//the checking can continue for 'look ahead' number of pixels
						LookAheadIndex = LookAheadLimit;
					else
						//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
						LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
				
					//This is where the actual checking happens the range for which has been determined in the previous if statement
					for(i = 4; i <=  LookAheadIndex; i++)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
						{
							//its not a legitimate edge as there is no continuity of transition
							//hence jump to the point where the check failed and continue searching for edge from this point forward
							XIterator = XIterator + i - PixelSkipValue;
							goto last1;
						}

					//copy the 4 pixels where edge has been detected into 'pixels' array
					for(int j = 0; j <= 3; j++)
						pixels[j] = iBytes[PixelArrayIndex + j];
				
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
					
					if(var == -1)
						// :( No edge detected
						goto last1;
				
					//Ah!! we have an edge
					Point.X = XIterator + var;
					Point.Y = YIterator;
					//Put this into the pointer and return true
					*DetectedPoint = Point;
					return true;
				
	last1:			i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
				}
			}
					
			for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
			{
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
				{
					PixelArrayIndex = XIterator + YIterator * ImageWidth;

					//the following if statement searches for a definite edge. If not found, it skips to the next loop
					if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + 3) * ImageWidth)]) < Tolerance)
						continue;

					if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
						LookAheadIndex = LookAheadLimit * ImageWidth;
					else
						LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
				
					for(i = 4 * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
						if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
						{
							YIterator = YIterator + i /ImageWidth - PixelSkipValue;
							goto last3;
						}
					for(int j = 0; j <= 3; j++)
						pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
				
					//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
					
					if(var == -1) 
						goto last3;
				
					Point.X = XIterator;
					Point.Y = YIterator + var;
					*DetectedPoint = Point;
					return true;

	last3:			i = 1;	//the dummy statement
				}
			}
			return false;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0026", __FILE__, __FUNCSIG__); return false; }
	}
	

	//---------------------------------------------------------------------------------------
	//The functions for surface edge detection
	//---------------------------------------------------------------------------------------

	//This function scans for an edge point within a straight rect in the specified direction. It sets the values for the two kinds of components
	int EdgeDetectionSingleChannel::DetectSurfaceEdge(int *FrameGrabBox, ScanDirection Scan_Direction, ComponentType Component_Type, bool dirGiven)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdge", __FUNCSIG__);
		try
		{
			std::list<struct Pt> PointsList;
			int i = 0;
			
			int Box[4] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3]};

			//if the component is "FocusBasedEdge" (eg: Brakes India), then we use the RapidCam functions directly 
			//and hence we do not need to invert the box to account for the inverted image. But otherwise, we need to invert the box.
			//if(Component_Type != FocusBasedEdge)
			//{
			//	//Inverting the box parameters as image is inverted
			//	Box[1] = ImageHeight - Box[1] - Box[3];
			//	
			//	//same way invert the vertical scan directions
			//	if(Scan_Direction == Upwards) Scan_Direction = Downwards;
			//	else if(Scan_Direction == Downwards) Scan_Direction = Upwards;
			//}

							
			int LookAheadValue, BlackTolerence, FilteringMaskTolerence , WhiteTolerence ,FilteringMaskTolerence2;

			if(Component_Type == CoaxialEdge)
			{
				double Avg = CalculateAvg(Box);
				LookAheadValue = Type1_LookAheadVal;
				/*BlackTolerence = (int)(Avg * Type1_BlackTol);
				FilteringMaskTolerence = (int)(Avg * Type1_FilterMaskTol);*/
				/*BlackTolerence=56;
				FilteringMaskTolerence = 56;*/
				BlackTolerence=Type1_BlackTol;
				FilteringMaskTolerence = Type1_FilterMaskTol;
				WhiteTolerence = Type1_WhiteTol;
				FilteringMaskTolerence2 = Type1_FilterMaskTolB2W;
				//LookAheadValue = 20;//15
				//BlackTolerence = 50;//70
				//FilteringMaskTolerence = 50;//70
			}
			else if(Component_Type == ManualColouredEdge)
			{
				LookAheadValue = Type3_LookAheadVal;
				BlackTolerence = Type3_BlackTol;
				FilteringMaskTolerence = Type3_FilterMaskTol;
				WhiteTolerence=Type3_WhiteTol;
				FilteringMaskTolerence2 = Type3_FilterMaskTolB2W;
			}
			
			if(Component_Type == FocusBasedEdge)
			{
				DetectSurfaceEdgeHelper2(Box, Scan_Direction, &PointsList);
			}
			else if(Component_Type == SingleWhitePixelEdge)
			{
				DetectSurfaceEdgeHelper4(Box, Scan_Direction, &PointsList);
			}
			else if(Component_Type == PlasticComponentEdge)
			{
				DetectSurfaceEdgeHelper3(Box, Scan_Direction, &PointsList, Type5_DiffThreshold, Type5_FilterMaskSize);
			}
			else if(Component_Type == PeakBased)
			{
				int FramBox[4] = {FrameGrabBox[0] + BoundryBuffer, FrameGrabBox[0] + FrameGrabBox[2]  - BoundryBuffer - 1, FrameGrabBox[1] + BoundryBuffer, FrameGrabBox[1] + FrameGrabBox[3]  - BoundryBuffer - 1};
				if(FramBox[0] - Type6_MaskFactor / 2 - 2 <= 0)
					FramBox[0] = FramBox[0] + (Type6_MaskFactor / 2);
				if(FramBox[1] + Type6_MaskFactor / 2 + 2 >= ImageWidth)
					FramBox[1] = FramBox[1] - (Type6_MaskFactor / 2);
				if(FramBox[2] - Type6_MaskFactor / 2 - 2 <= 0)
					FramBox[2] = FramBox[2] + (Type6_MaskFactor / 2);
				if(FramBox[3] + Type6_MaskFactor / 2 + 2 >= ImageHeight)
					FramBox[3] = FramBox[3] - (Type6_MaskFactor / 2);
				Masking_Binarization(FramBox, Type6_MaskFactor, Type6_Binary_LowerFactor, Type6_Binary_UpperFactor, Type6_MaskingLevel);
				if(dirGiven)
					DetectSurfaceEdgeWithoutDirection(FramBox, Type6_PixelThreshold, &PointsList, Scan_Direction, true);
				else
					DetectSurfaceEdgeWithoutDirection(FramBox, Type6_PixelThreshold, &PointsList);
			}
			else
			{
				DetectSurfaceEdgeHelper(Box, Scan_Direction, &PointsList, LookAheadValue, BlackTolerence, FilteringMaskTolerence,WhiteTolerence,FilteringMaskTolerence2);
			}
			
			//Now put all the points in the linear array with no correction (there is no beam Divergence in Surface Edge Detection!)
			bool tempCorrectforBeam = CorrectforBeamDivergence;
			CorrectforBeamDivergence = false;
			i = AddEdgePtsToCollection(PointsList);			
			CorrectforBeamDivergence = tempCorrectforBeam;
			return i;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0027", __FILE__, __FUNCSIG__); return -1; }
	}

	int EdgeDetectionSingleChannel::DetectSurfaceEdge(int *FrameGrabBox, ScanDirection Scan_Direction, ComponentType Component_Type, int startR, int startC)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdge1", __FUNCSIG__);
		try
		{
			std::list<struct Pt> ArrangedPointsList1, ArrangedPointsList2;
			int i = 0, FramBox[4] = {FrameGrabBox[0] * this->CamSizeRatio + BoundryBuffer, FrameGrabBox[0] * this->CamSizeRatio + FrameGrabBox[2] * this->CamSizeRatio - BoundryBuffer - 1, 
									 FrameGrabBox[1] * this->CamSizeRatio + BoundryBuffer, FrameGrabBox[1] * this->CamSizeRatio + FrameGrabBox[3] * this->CamSizeRatio - BoundryBuffer - 1};
		
			if(FramBox[0] - Type6_MaskFactor / 2 - 2 <= 0)
				FramBox[0] = FramBox[0] + (Type6_MaskFactor / 2);
			if(FramBox[1] + Type6_MaskFactor / 2 + 2 >= ImageWidth)
				FramBox[1] = FramBox[1] - (Type6_MaskFactor / 2);
			if(FramBox[2] - Type6_MaskFactor / 2 - 2 <= 0)
				FramBox[2] = FramBox[2] + (Type6_MaskFactor / 2);
			if(FramBox[3] + Type6_MaskFactor / 2 + 2 >= ImageHeight)
				FramBox[3] = FramBox[3] - (Type6_MaskFactor / 2);
			Masking_Binarization(FramBox, Type6_MaskFactor, Type6_Binary_LowerFactor, Type6_Binary_UpperFactor, Type6_MaskingLevel);
			DetectSurfaceEdgeHelperPeakPoint(FramBox[0], FramBox[1], FramBox[2], FramBox[3], Type6_PixelThreshold, Scan_Direction, &ArrangedPointsList1, startR, startC);
		/*	if(ArrangedPointsList.size() < MinimumPointsInEdge)
			{*/
			if(Scan_Direction == Rightwards || Scan_Direction == Leftwards)
				Scan_Direction = Upwards;
			else
				Scan_Direction = Rightwards;
			
			DetectSurfaceEdgeHelperPeakPoint(FramBox[0], FramBox[1], FramBox[2], FramBox[3], Type6_PixelThreshold, Scan_Direction, &ArrangedPointsList2, startR, startC);
			/*}*/
			
			//Now put all the points in the linear array with no correction (there is no beam Divergence in Surface Edge Detection!)
			bool tempCorrectforBeam = CorrectforBeamDivergence;
			CorrectforBeamDivergence = false;

			if(ArrangedPointsList1.size() > ArrangedPointsList2.size())
			{
				ArrangeAllPoints2(&ArrangedPointsList1);
				i = AddEdgePtsToCollection(ArrangedPointsList1);			
			}
			else
			{
				ArrangeAllPoints2(&ArrangedPointsList2);
				i = AddEdgePtsToCollection(ArrangedPointsList1);			
			}
			CorrectforBeamDivergence = tempCorrectforBeam;
			
			return i;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0027", __FILE__, __FUNCSIG__); return -1; }
	}

	int EdgeDetectionSingleChannel::DetectSurfaceEdge_Peak(int *FrameGrabBox, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int PixelThreshold, int MaskingLevel, bool RedoImagePreProcess)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdge_Peak", __FUNCSIG__);
		try
		{
			std::list<struct Pt> PointsList;
			int i = 0;	

			int FramBox[4] = {FrameGrabBox[0] * this->CamSizeRatio + BoundryBuffer, FrameGrabBox[0] * this->CamSizeRatio + FrameGrabBox[2] * this->CamSizeRatio - BoundryBuffer - 1, 
							  FrameGrabBox[1] * this->CamSizeRatio + BoundryBuffer, FrameGrabBox[1] * this->CamSizeRatio + FrameGrabBox[3] * this->CamSizeRatio - BoundryBuffer - 1};
			
			if(FramBox[0] - Type6_MaskFactor / 2 - 2 <= 0)
				FramBox[0] = FramBox[0] + (Type6_MaskFactor / 2);
			if(FramBox[1] + Type6_MaskFactor / 2 + 2 >= ImageWidth)
				FramBox[1] = FramBox[1] - (Type6_MaskFactor / 2);
			if(FramBox[2] - Type6_MaskFactor / 2 - 2 <= 0)
				FramBox[2] = FramBox[2] + (Type6_MaskFactor / 2);
			if(FramBox[3] + Type6_MaskFactor / 2 + 2 >= ImageHeight)
				FramBox[3] = FramBox[3] - (Type6_MaskFactor / 2);

			if (RedoImagePreProcess) Masking_Binarization(FramBox, MaskSize, Binary_LowerFactor, Binary_UpperFactor, MaskingLevel);
			DetectSurfaceEdgeWithoutDirection(FramBox, PixelThreshold, &PointsList);
			
			//Now put all the points in the linear array after correction if required...
			return AddEdgePtsToCollection(PointsList);			
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0027a", __FILE__, __FUNCSIG__); return -1; }
	}

	int EdgeDetectionSingleChannel::DetectSurfaceEdge_Peak(int *FrameGrabBox, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int PixelThreshold, int MaskingLevel, ScanDirection lastDir, int startR, int startC)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdge_Peak1", __FUNCSIG__);
		try
		{
			std::list<struct Pt> ArrangedPointsList1, ArrangedPointsList2;
			int i = 0;	
			int FramBox[4] = {FrameGrabBox[0] + BoundryBuffer, FrameGrabBox[0] + FrameGrabBox[2]  - BoundryBuffer - 1, FrameGrabBox[1] + BoundryBuffer, FrameGrabBox[1] + FrameGrabBox[3]  - BoundryBuffer - 1};
			if(FramBox[0] - Type6_MaskFactor / 2 - 2 <= 0)
				FramBox[0] = FramBox[0] + (Type6_MaskFactor / 2);
			if(FramBox[1] + Type6_MaskFactor / 2 + 2 >= ImageWidth)
				FramBox[1] = FramBox[1] - (Type6_MaskFactor / 2);
			if(FramBox[2] - Type6_MaskFactor / 2 - 2 <= 0)
				FramBox[2] = FramBox[2] + (Type6_MaskFactor / 2);
			if(FramBox[3] + Type6_MaskFactor / 2 + 2 >= ImageHeight)
				FramBox[3] = FramBox[3] - (Type6_MaskFactor / 2);


			Masking_Binarization(FramBox, MaskSize, Binary_LowerFactor, Binary_UpperFactor, MaskingLevel);
			DetectSurfaceEdgeHelperPeakPoint(FramBox[0], FramBox[1], FramBox[2], FramBox[3], PixelThreshold, lastDir, &ArrangedPointsList1, startR, startC);
			
			if(lastDir == Rightwards || lastDir == Leftwards)
				lastDir = Upwards;
			else
				lastDir = Rightwards;
			DetectSurfaceEdgeHelperPeakPoint(FramBox[0], FramBox[1], FramBox[2], FramBox[3], PixelThreshold, lastDir, &ArrangedPointsList2, startR, startC);
			
			//Now put all the points in the linear array with no correction (there is no beam Divergence in Surface Edge Detection!)
			bool tempCorrectforBeam = CorrectforBeamDivergence;
			CorrectforBeamDivergence = false;

			if(ArrangedPointsList1.size() > ArrangedPointsList2.size())
			{
				ArrangeAllPoints2(&ArrangedPointsList1);
				i = AddEdgePtsToCollection(ArrangedPointsList1);			
			}
			else
			{
				ArrangeAllPoints2(&ArrangedPointsList2);
				i = AddEdgePtsToCollection(ArrangedPointsList1);			
			}
			CorrectforBeamDivergence = tempCorrectforBeam;
			return i;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0027a", __FILE__, __FUNCSIG__); return -1; }
	}

	//This function detects an edge in the specified direction for the angular rectangular area
	int EdgeDetectionSingleChannel::DetectSurfaceEdgeAngularRect(int *FrameGrabBox, bool OppositeScan, bool RedoImagePreProcess, ComponentType Component_Type)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeAngularRect", __FUNCSIG__);
		try
		{
			std::list<struct Pt> PointsList;
			int i = 0;
			int Box[5] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3], FrameGrabBox[4]};

			for (int i = 0; i < 5; i++)
				Box[i] *= this->CamSizeRatio;
			/*Box[1] = ImageHeight - 1 - Box[1];
			Box[3] = ImageHeight - 1 - Box[3];*/
			
			int LookAheadValue, BlackTolerence, FilteringMaskTolerence;

			if(Component_Type == CoaxialEdge)
			{
				LookAheadValue = 20;
				BlackTolerence = 50;
				FilteringMaskTolerence = 50;
			}
			else
			{
				LookAheadValue = 3;
				BlackTolerence = 150;
				FilteringMaskTolerence = 100;
			}
			
			if (Component_Type == PeakBased)
			{
				if (KroneckerEdge)
				{
					if (Type6_MaskFactor > KroneckerThickness / 2) Type6_MaskFactor = KroneckerThickness / 2;
				}
				if (RedoImagePreProcess) 
					Masking_BinarizationAngRect(Box, Type6_MaskFactor, Type6_Binary_LowerFactor, Type6_Binary_UpperFactor, Type6_MaskingLevel);
				
				DetectSurfaceEdgeAngularRectHelper(Box, OppositeScan, &PointsList);
			}
			else
				DetectSurfaceEdgeAngularRectHelper(Box, OppositeScan, &PointsList, LookAheadValue, BlackTolerence, FilteringMaskTolerence);
			
			//Now put all the points in the linear array with no correction (there is no beam Divergence in Surface Edge Detection!)
			bool tempCorrectforBeam = CorrectforBeamDivergence;
			CorrectforBeamDivergence = false;
			i = AddEdgePtsToCollection(PointsList);			
			CorrectforBeamDivergence = tempCorrectforBeam;
			return i;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0005", __FILE__, __FUNCSIG__); return -1; }
	}
	
	//This function detects an edge in the specified direction for the curved Box area
	int EdgeDetectionSingleChannel::DetectSurfaceEdgeCurvedBox(double *FrameGrabBox, bool OppositeScan, bool RedoImagePreProcess, ComponentType Component_Type)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeCurvedBox", __FUNCSIG__);
		try
		{
			std::list<struct Pt> PointsList;
			int i = 0;
			double Box[6] = { FrameGrabBox[0] * this->CamSizeRatio, FrameGrabBox[1] * this->CamSizeRatio,FrameGrabBox[2] * this->CamSizeRatio,FrameGrabBox[3] * this->CamSizeRatio, FrameGrabBox[4] ,FrameGrabBox[5] };

			int LookAheadValue, BlackTolerence, FilteringMaskTolerence;

			if(Component_Type == CoaxialEdge)
			{
				LookAheadValue = 20;//15
				BlackTolerence = 50;//70
				FilteringMaskTolerence = 50;//70
			}
			else
			{
				LookAheadValue = 3;
				BlackTolerence = 150;
				FilteringMaskTolerence = 100;
			}
			if(Component_Type == PeakBased)
			{
				if (RedoImagePreProcess) Masking_BinarizationCurve(Box, Type6_MaskFactor, Type6_Binary_LowerFactor, Type6_Binary_UpperFactor, Type6_MaskingLevel);
				DetectSurfaceEdgeCurvedBoxHelper(Box, OppositeScan, &PointsList, Type6_PixelThreshold);
			}
			else
				DetectSurfaceEdgeCurvedBoxHelper(Box, OppositeScan, &PointsList, LookAheadValue, BlackTolerence, FilteringMaskTolerence);
			
			//Now put all the points in the linear array with no correction (there is no beam Divergence in Surface Edge Detection!)
			bool tempCorrectforBeam = CorrectforBeamDivergence;
			CorrectforBeamDivergence = false;
			i = AddEdgePtsToCollection(PointsList);			
			CorrectforBeamDivergence = tempCorrectforBeam;
			return i;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0006", __FILE__, __FUNCSIG__); return -1; }
	}
		
	//ankit Fastner and Rishab
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeHelper(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, int LookAhead, int BlackTolerence, int FilteringMaskTolerence , int WhiteTolerence , int FilteringMaskTolerence2)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeHelper", __FUNCSIG__);
		try
		{
			int XIterator, YIterator, LookAheadIndex, i;
			struct Pt Point, prevPt;
			int LeftBoundry = Box[0] + BoundryBuffer;
			int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
			int TopBoundry = Box[1] + BoundryBuffer;
			int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
			int PixelArrayIndex;
			int FirstPixelMedianFilterVal;	
			bool BlackToWhiteMovement = false;
			//if(Scan_Direction == Rightwards || Scan_Direction == Downwards)
			switch(Scan_Direction)
			{
			case Rightwards:
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						FirstPixelMedianFilterVal = GetMedianFilterValue(LeftBoundry,YIterator,9,false);
						for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(FirstPixelMedianFilterVal<109)
							{
								//Movement from Black to White
								if(iBytes[PixelArrayIndex] < WhiteTolerence)
								{
									BlackToWhiteMovement=true;
									continue;
								}
							}
							else
							{
								//Movement from White to Black
								if(iBytes[PixelArrayIndex] > BlackTolerence)
								{
									BlackToWhiteMovement=false;
									continue;
								}
							}

							//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
							//for this purpose, first it must be determined till where this checking must be carried out
							if((XIterator + LookAhead) < ImageWidth - BoundryBuffer)
								//the checking can continue for 'look ahead' number of pixels
								LookAheadIndex = LookAhead;
							else
								//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
								LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
							//This is where the actual checking happens the range for which has been determined in the previous if statement
							for(i = 1; i <=  LookAheadIndex; i++)
								if(FirstPixelMedianFilterVal<109)
								{
									//Movement from Black to White
									if(iBytes[PixelArrayIndex + i] < WhiteTolerence)
									{
										//its not a legitimate edge as there is no continuity of transition
										//hence jump to the point where the check failed and continue searching for edge from this point forward
										XIterator = XIterator + i;
										goto last1;
									}
								}
								else
								{
									//Movement from White to Black
									if(iBytes[PixelArrayIndex + i] > BlackTolerence)
									{
										//its not a legitimate edge as there is no continuity of transition
										//hence jump to the point where the check failed and continue searching for edge from this point forward
										XIterator = XIterator + i;
										goto last1;
									}
								}

						
							if(XIterator == LeftBoundry)
								break;
							if(FirstPixelMedianFilterVal<109)
							{
								//Movement from Black to White
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence2,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}
							else
							{
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}

							//Ah!! we have an edge
							Point.X = XIterator - 0.5;
							Point.Y = YIterator;
							bool gotnewPt = true;
							if (PointsList->size() > 0)
							{
								prevPt = PointsList->back();
								gotnewPt = false;
								if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
									gotnewPt;
							}
							if (gotnewPt)
								PointsList->push_back(Point);
							break;
					
	last1:					i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
						}
					}				
				break;
			case Leftwards:
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						FirstPixelMedianFilterVal = GetMedianFilterValue(RightBoundry,YIterator,9,false);
						for(XIterator = RightBoundry; XIterator >= LeftBoundry; XIterator--)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(FirstPixelMedianFilterVal<109)
								{
									//Movement from Black to White
									if(iBytes[PixelArrayIndex] < WhiteTolerence)
									{
										BlackToWhiteMovement=true;
										continue;
									}
								}
								else
								{
									//Movement from White to Black
									if(iBytes[PixelArrayIndex] > BlackTolerence)
									{
										BlackToWhiteMovement=false;
										continue;
									}
								}

							if((XIterator - LookAhead) >= BoundryBuffer)
								LookAheadIndex = LookAhead;
							else
								LookAheadIndex = XIterator - BoundryBuffer;
					
							for(i = 1; i <=  LookAheadIndex; i++)
								if(FirstPixelMedianFilterVal<109)
								{
									if(iBytes[PixelArrayIndex - i] < BlackTolerence)
									{
										XIterator = XIterator - i;
										goto last2;
									}
								}
								else
								{
									if(iBytes[PixelArrayIndex - i] > BlackTolerence)
									{
										XIterator = XIterator - i;
										goto last2;
									}
								}

							if(XIterator == RightBoundry)
								break;

							if(FirstPixelMedianFilterVal<109)
							{
								//Movement from Black to White
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence2,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}
							else
							{
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}

							Point.X = XIterator + 0.5;
							Point.Y = YIterator;
							bool gotnewPt = true;
							if (PointsList->size() > 0)
							{
								prevPt = PointsList->back();
								gotnewPt = false;
								if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
									gotnewPt;
							}
							if (gotnewPt)
								PointsList->push_back(Point);

							break;

	last2:					i = 1; //the dummy statement
						}
					}			
				break;
			case Downwards:
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						FirstPixelMedianFilterVal = GetMedianFilterValue(XIterator,TopBoundry,9,false);
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(FirstPixelMedianFilterVal<109)
								{
									//Movement from Black to White
									if(iBytes[PixelArrayIndex] < WhiteTolerence)
									{
										BlackToWhiteMovement=true;
										continue;
									}
								}
							else
							{
								//Movement from White to Black
								if(iBytes[PixelArrayIndex] > BlackTolerence)
								{
									BlackToWhiteMovement=false;
									continue;
								}
							}

							if((YIterator + LookAhead) < ImageHeight - BoundryBuffer)
								LookAheadIndex = LookAhead * ImageWidth;
							else
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
					
							for(i = 1 * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
								if(FirstPixelMedianFilterVal<109)
								{
									if(iBytes[PixelArrayIndex + i] < BlackTolerence)
									{
										YIterator = YIterator + i /ImageWidth;
										goto last3;
									}
								}
								else
								{
										if(iBytes[PixelArrayIndex + i] > BlackTolerence)
									{
										YIterator = YIterator + i /ImageWidth;
										goto last3;
									}
								}
						
							if(YIterator == TopBoundry)
								break;

							if(FirstPixelMedianFilterVal<109)
							{
								//Movement from Black to White
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence2,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}
							else
							{
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}

							Point.X = XIterator;
							Point.Y = YIterator - 0.5;
							bool gotnewPt = true;
							if (PointsList->size() > 0)
							{
								prevPt = PointsList->back();
								gotnewPt = false;
								if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
									gotnewPt;
							}
							if (gotnewPt)
								PointsList->push_back(Point);
							break;

	last3:					i = 1;	//the dummy statement
						}					
					}				
				break;
			case Upwards:
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						FirstPixelMedianFilterVal = GetMedianFilterValue(XIterator,BottomBoundry,9,false);
						for(YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator--)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(FirstPixelMedianFilterVal<109)
								{
									//Movement from Black to White
									if(iBytes[PixelArrayIndex] < WhiteTolerence)
									{
										BlackToWhiteMovement=true;
										continue;
									}
								}
							else
							{
								//Movement from White to Black
								if(iBytes[PixelArrayIndex] > BlackTolerence)
								{
									BlackToWhiteMovement=false;
									continue;
								}
							}

							if((YIterator - LookAhead) >= BoundryBuffer)
								LookAheadIndex = LookAhead * ImageWidth;
							else
								LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;
					
							for(i = 1 * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
								if(FirstPixelMedianFilterVal<109)
								{
									if(iBytes[PixelArrayIndex - i] < BlackTolerence)
									{
										YIterator = YIterator - i / ImageWidth;
										goto last4;
									}
								}
								else
								{
									if(iBytes[PixelArrayIndex - i] > BlackTolerence)
									{
										YIterator = YIterator - i / ImageWidth;
										goto last4;
									}
								}
						
							if(YIterator == TopBoundry)
								break;

							if(FirstPixelMedianFilterVal<109)
							{
								//Movement from Black to White
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence2,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}
							else
							{
								if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence,BlackToWhiteMovement))
									//filter the edge points. Only if it passes, its added to the list
									break;
							}

							Point.X = XIterator;
							Point.Y = YIterator + 0.5;
							bool gotnewPt = true;
							if (PointsList->size() > 0)
							{
								prevPt = PointsList->back();
								gotnewPt = false;
								if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
									gotnewPt;
							}
							if (gotnewPt)
								PointsList->push_back(Point);
							break;

	last4:					i = 1; //the dummy statement
						}
					}			
				break;
			}
			FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0028", __FILE__, __FUNCSIG__); }
	}

	void EdgeDetectionSingleChannel::DetectSurfaceEdgeBasic(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, int LookAhead, int DiffCutoff)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeBasic", __FUNCSIG__);
		try
		{
			int XIterator, YIterator, LookAheadIndex, i;
			struct Pt Point;
			int LeftBoundry = Box[0] + BoundryBuffer;
			int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
			int TopBoundry = Box[1] + BoundryBuffer;
			int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
			int PixelArrayIndex;
			int CompareSkip = 10;
			switch(Scan_Direction)
			{
			case Rightwards:
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[PixelArrayIndex + CompareSkip]) < DiffCutoff)
								continue;
							
							//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
							//for this purpose, first it must be determined till where this checking must be carried out
							if((XIterator + LookAhead) < ImageWidth - BoundryBuffer)
								//the checking can continue for 'look ahead' number of pixels
								LookAheadIndex = LookAhead;
							else
								//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
								LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
							//This is where the actual checking happens the range for which has been determined in the previous if statement
							for(i = CompareSkip + 1; i <=  LookAheadIndex; i++)
							{
								//Movement from White to Black
								if(abs(iBytes[PixelArrayIndex + i] - iBytes[PixelArrayIndex]) < DiffCutoff)
								{
									//its not a legitimate edge as there is no continuity of transition
									//hence jump to the point where the check failed and continue searching for edge from this point forward
									XIterator = XIterator + i;
									goto last1;
								}
							}
							
							if(XIterator == LeftBoundry)
								break;
							
							//Ah!! we have an edge
							Point.X = XIterator - 0.5;
							Point.Y = YIterator;
						
							PointsList->push_back(Point);
							break;
					
	last1:					i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
						}
					}				
				break;
			case Leftwards:
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						for(XIterator = RightBoundry; XIterator >= LeftBoundry; XIterator--)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[PixelArrayIndex - CompareSkip]) < DiffCutoff)
								continue;

							if((XIterator - LookAhead) >= BoundryBuffer)
								LookAheadIndex = LookAhead;
							else
								LookAheadIndex = XIterator - BoundryBuffer;
					
							for(i = CompareSkip + 1; i <=  LookAheadIndex; i++)
							{
								if(abs(iBytes[PixelArrayIndex - i] - iBytes[PixelArrayIndex]) < DiffCutoff)
								{
									XIterator = XIterator - i;
									goto last2;
								}
							}

							if(XIterator == RightBoundry)
								break;

							Point.X = XIterator + 0.5;
							Point.Y = YIterator;
						
							PointsList->push_back(Point);
							break;

	last2:					i = 1; //the dummy statement
						}
					}			
				break;
			case Downwards:
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							if(abs(iBytes[PixelArrayIndex] - iBytes[PixelArrayIndex + CompareSkip * ImageWidth]) < DiffCutoff)
								continue;
							
							if((YIterator + LookAhead) < ImageHeight - BoundryBuffer)
								LookAheadIndex = LookAhead * ImageWidth;
							else
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
					
							for(i = (CompareSkip + 1) * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
							{
								if(abs(iBytes[PixelArrayIndex + i] - iBytes[PixelArrayIndex]) < DiffCutoff)
								{
									YIterator = YIterator + i /ImageWidth;
										goto last3;
								}
							}
							
							if(YIterator == TopBoundry)
								break;

							Point.X = XIterator;
							Point.Y = YIterator - 0.5;
						
							PointsList->push_back(Point);
							break;

	last3:					i = 1;	//the dummy statement
						}					
					}				
				break;
			case Upwards:
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						for(YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator--)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							if(abs(iBytes[PixelArrayIndex] - iBytes[PixelArrayIndex - CompareSkip * ImageWidth]) < DiffCutoff)
								continue;

							if((YIterator - LookAhead) >= BoundryBuffer)
								LookAheadIndex = LookAhead * ImageWidth;
							else
								LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;
					
							for(i = (CompareSkip + 1) * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
							{
								if(abs(iBytes[PixelArrayIndex - i] - iBytes[PixelArrayIndex]) < DiffCutoff)
								{
									YIterator = YIterator - i /ImageWidth;
										goto last4;
								}
							}
							
							if(YIterator == TopBoundry)
								break;

							Point.X = XIterator;
							Point.Y = YIterator + 0.5;
						
							PointsList->push_back(Point);
							break;

	last4:					i = 1; //the dummy statement
						}
					}			
				break;
			}
			FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0028", __FILE__, __FUNCSIG__); }
	}

	void EdgeDetectionSingleChannel::DetectSurfaceEdgeAngularRectHelper(int *Box, bool OppositeScan, std::list<struct Pt> *PointsList)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeAngularRectHelper", __FUNCSIG__);
		try
		{
			Pt Point1, Point2, OriginPoint, Point, prevPt;
			int Height, Width = Box[4], XIterator, YIterator, Xoriginal, Yoriginal;
			double Xoriginal_d, Yoriginal_d;
			double CosTh = 0, SinTh = 0, PixelDiff = 0, lastVal = 0;
			int i = 0, PixelArrayIndex = 0, PtId = 0;
			bool gotfirstPoint = false, GotPoint = false;
			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if(Box[1] > Box[3])
			{	
				Point1.X = Box[0];
				Point1.Y = Box[1];
				Point2.X = Box[2];
				Point2.Y = Box[3];
			}
			else
			{
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}
			
			CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));
			SinTh = (Point1.Y - Point2.Y) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));

			//The left top corner of the rect is taken as the origin for the transformation
			OriginPoint.X = Point2.X - Width / 2 * SinTh;
			OriginPoint.Y = Point2.Y - Width / 2 * CosTh;

			Height = sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));

			//To account for the direction change in the range 135 deg to 180 deg
			if ((Point1.X - Point2.X) > (Point1.Y - Point2.Y))
				OppositeScan = !OppositeScan;
			
			if (!OppositeScan)
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					gotfirstPoint = GotPoint = false;
					PixelDiff = 0;
					for(XIterator = 0; XIterator < Width; XIterator++)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;
						if(gotfirstPoint)
						{
							double TmpValue = abs(iBytes[PixelArrayIndex] - lastVal);
							if(TmpValue > PixelDiff && (lastVal < Type6_PixelThreshold || iBytes[PixelArrayIndex] < Type6_PixelThreshold))
							{
								PixelDiff = TmpValue;
								GotPoint = true;
								Point.X = Xoriginal - 0.5; 
								Point.Y = Yoriginal - 0.5;
							}
							lastVal= iBytes[PixelArrayIndex];
						}
						else
						{
						   gotfirstPoint = true;
						   lastVal= iBytes[PixelArrayIndex];
						}		
					}
					if(GotPoint)
					{
						if (PointsList->size() > 0)
						{
							prevPt = PointsList->back();
							if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
								continue;
						}
						Point.pt_index = PtId++;
						PointsList->push_back(Point);
					}
				}			
			}
			else
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					gotfirstPoint = GotPoint = false;
					PixelDiff = 0;
					for(XIterator = Width - 1; XIterator >= 0; XIterator--)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;
						if(gotfirstPoint)
						{
							double TmpValue = abs(iBytes[PixelArrayIndex] - lastVal);
							if(TmpValue > PixelDiff && (lastVal < Type6_PixelThreshold || iBytes[PixelArrayIndex] < Type6_PixelThreshold))
							{
								PixelDiff = TmpValue;
								GotPoint = true;
								Point.X = Xoriginal - 0.5;
								Point.Y = Yoriginal - 0.5; 
							}
							lastVal= iBytes[PixelArrayIndex];
						}
						else
						{
						   gotfirstPoint = true;
						   lastVal= iBytes[PixelArrayIndex];
						}	
					}
					if (GotPoint)
					{
						if (PointsList->size() > 0)
						{
							prevPt = PointsList->back();
							if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
								continue;
						}
						Point.pt_index = PtId++;
						PointsList->push_back(Point);
					}
				}			
			}
			FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0028b", __FILE__, __FUNCSIG__); }
	}


	//General Surface edge detection for Angular rect
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeAngularRectHelper(int *Box, bool OppositeScan, std::list<struct Pt> *PointsList, int LookAhead, int BlackTolerence, int FilteringMaskTolerence)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeAngularRectHelper1", __FUNCSIG__);
		try
		{
			Pt Point1, Point2, OriginPoint, Point, prevPt;
			int Width = Box[4], Height, XIterator, YIterator, Xoriginal, Yoriginal, XoriginalTmp, YoriginalTmp;
			double Xoriginal_d, Yoriginal_d, XoriginalTmp_d, YoriginalTmp_d;
			double CosTh, SinTh;
			int  i;
			int PixelArrayIndex;

			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if(Box[1] > Box[3])
			{	
				Point1.X = Box[0];
				Point1.Y = Box[1];
				Point2.X = Box[2];
				Point2.Y = Box[3];
			}
			else
			{
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}
			Height = sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2)); //height of the box is the dist between the 2 points

			CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));
			SinTh = (Point1.Y - Point2.Y) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));

			//The left top corner of the rect is taken as the origin for the transformation
			OriginPoint.X = Point2.X - Width / 2 * SinTh;
			OriginPoint.Y = Point2.Y - Width / 2 * CosTh;

			//To account for the direction change in the range 135 deg to 180 deg
			if ((Point1.X - Point2.X) > (Point1.Y - Point2.Y))
				OppositeScan = !OppositeScan;
			
			if (!OppositeScan)
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = 0; XIterator < Width; XIterator++)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						for(i = 1; i <= LookAhead; i++)
						{
							GetOriginalCoordinates(XIterator + i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							if(iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackTolerence)
							{
								//its not a legitimate edge as there is no continuity of transition
								//hence jump to the point where the check failed and continue searching for edge from this point forward
								XIterator = XIterator + i;
								goto last1;
							}
						}
						
						if(!FilteringMask(Xoriginal, Yoriginal, FilteringMaskTolerence))
							//filter the edge points. Only if it passes, its added to the list
							break;

						//Ah!! we have an edge
						GetOriginalCoordinates(XIterator - 0.5, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Point.X = Xoriginal_d;
						Point.Y = Yoriginal_d;
						bool gotnewPt = true;
						if (PointsList->size() > 0)
						{
							prevPt = PointsList->back();
							gotnewPt = false;
							if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
								gotnewPt;
						}
						if (gotnewPt)
							PointsList->push_back(Point);
						break;
					
last1:					i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
					}
				}				
			}
			else
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = Width - 1; XIterator >= 0; XIterator--)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						for(i = 1; i <= LookAhead; i++)
						{
							GetOriginalCoordinates(XIterator - i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							if(iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackTolerence)
							{
								XIterator = XIterator - i;
								goto last2;
							}
						}

						if(!FilteringMask(Xoriginal, Yoriginal, FilteringMaskTolerence))
							break;

						GetOriginalCoordinates(XIterator + 0.5, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Point.X = Xoriginal_d;
						Point.Y = Yoriginal_d;
						bool gotnewPt = true;
						if (PointsList->size() > 0)
						{
							prevPt = PointsList->back();
							gotnewPt = false;
							if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
								gotnewPt;
						}
						if (gotnewPt)
							PointsList->push_back(Point);
						break;

last2:					i = 1; //the dummy statement
					}
				}			
			}
			
			//FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0028", __FILE__, __FUNCSIG__); }
	}

	//Function to get the points in original coordinate system from the transformed coordinate system
	void EdgeDetectionSingleChannel::GetOriginalCoordinates(double XDash, double YDash, double XOrigin, double YOrigin, double CosTh, double SinTh, double *X, double *Y)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "GetOriginalCoordinates", __FUNCSIG__);
		try
		{
			// X = X' sin(a) - Y' cos(a) + XOrigin
			// Y = X' cos(a) + Y' sin(a) + YOrigin
			*X = XDash * SinTh - YDash * CosTh + XOrigin;
			*Y = XDash * CosTh + YDash * SinTh + YOrigin;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0029", __FILE__, __FUNCSIG__); }
	}

	//General Surface edge detection for curved box
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeCurvedBoxHelper(double *Box, bool OppositeScan, std::list<struct Pt> *PointsList, int LookAhead, int BlackTolerence, int FilteringMaskTolerence)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeCurvedBoxHelper", __FUNCSIG__);
		try
		{
			Pt OriginPoint, Point, prevPt;
			int RIterator;
			double ThetaIterator;
			double pixels[40] = {0};
			int  i;
			double R1, R2, Theta1, Theta2, IncrementalAngle;
			int Xoriginal, Yoriginal, XoriginalTmp, YoriginalTmp;
			double Xoriginal_d, Yoriginal_d, XoriginalTmp_d, YoriginalTmp_d;
			int PixelArrayIndex;
			
			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];
			IncrementalAngle = 2 / (R1 + R2); //incrementing the angle so that linear increment is one pixel: ang = 1 / ((R1 + R2) / 2)

			
			if (!OppositeScan)
			{
				for(ThetaIterator = Theta1; ThetaIterator < Theta2; ThetaIterator += IncrementalAngle)
				{
					for(RIterator = R1; RIterator < R2; RIterator++)
					{
						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						for(i = 1; i <= LookAhead; i++)
						{
							GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator + i, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							if(iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackTolerence)
							{
								//its not a legitimate edge as there is no continuity of transition
								//hence jump to the point where the check failed and continue searching for edge from this point forward
								RIterator = RIterator + i;
								goto last1;
							}
						}
						
						if(!FilteringMask(Xoriginal, Yoriginal, FilteringMaskTolerence))
							//filter the edge points. Only if it passes, its added to the list
							break;

						//Ah!! we have an edge
						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator - 0.5, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Point.X = Xoriginal_d;
						Point.Y = Yoriginal_d;
						bool gotnewPt = true;
						if (PointsList->size() > 0)
						{
							prevPt = PointsList->back();
							gotnewPt = false;
							if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
								gotnewPt;
						}
						if (gotnewPt)
							PointsList->push_back(Point);
						break;
					
last1:					i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
					}
				}				
			}
			else
			{
				for(ThetaIterator = Theta1; ThetaIterator < Theta2; ThetaIterator += IncrementalAngle)
				{
					for(RIterator = R2; RIterator > R1; RIterator--)
					{
						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						for(i = 1; i <= LookAhead; i++)
						{
							GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator - i, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							if(iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackTolerence)
							{
								RIterator = RIterator - i;
								goto last2;
							}
						}

						if(!FilteringMask(Xoriginal, Yoriginal, FilteringMaskTolerence))
							break;

						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator + 0.5, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Point.X = Xoriginal_d;
						Point.Y = Yoriginal_d;
						bool gotnewPt = true;
						if (PointsList->size() > 0)
						{
							prevPt = PointsList->back();
							gotnewPt = false;
							if (abs(Point.X - prevPt.X) < 0.003 || abs(Point.Y - prevPt.Y) < 0.003)
								gotnewPt;
						}
						if (gotnewPt)
							PointsList->push_back(Point);
						break;

last2:					i = 1; //the dummy statement
					}
				}			
			}
			
			//FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0028", __FILE__, __FUNCSIG__); }
	}

	//Function to get the points in original coordinate system from the radial coordinate system
	void EdgeDetectionSingleChannel::GetOriginalCoordinates(double XOrigin, double YOrigin, double Radius, double Theta, double *X, double *Y)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "GetOriginalCoordinates", __FUNCSIG__);
		*X = XOrigin + Radius * cos(Theta);
		*Y = YOrigin - Radius * sin(Theta);
	}

	//pixel masking to filter out unwanted edge points
	bool EdgeDetectionSingleChannel::FilteringMask(int XPt, int YPt, int WhiteThreshold, bool BlackToWhiteMovement)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "FilteringMask", __FUNCSIG__);
		try
		{
			int MaskSize = 7, WhiteCnt = 0;
			int Left = XPt - MaskSize / 2;
			int Right = Left + MaskSize - 1;
			int Top = YPt - MaskSize / 2;
			int Bottom = Top + MaskSize - 1;

			//each column to column
			for(int X = Left; X <= Right; X++)
			{
				//each row to row
				for(int Y = Top; Y <= Bottom; Y++)
				{
					if(iBytes[X + Y * ImageWidth] > WhiteThreshold)
						//Count the number of pixels that have value more than the threshold
						WhiteCnt++;
				}
			}

			double CntPercentage = WhiteCnt / pow((double)MaskSize,2); //Get the percentage of white pixels
			if(BlackToWhiteMovement)
			{
				if(CntPercentage > 0.85 || CntPercentage < 0.25)
					return false;
			}
			else
			{
				if(CntPercentage > 0.5 || CntPercentage < 0.3)
					return false;
			}
			//if(CntPercentage > 0.7 || CntPercentage < 0.3)
			//if(CntPercentage > 0.6 || CntPercentage < 0.4)
				//if percentage not between 30 and 70, then return false				

			//Percentage nearly 50, so qualify as edge point
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0029", __FILE__, __FUNCSIG__); return false; }
	}
	bool EdgeDetectionSingleChannel::FilteringMask(int XPt, int YPt, int WhiteThreshold)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "FilteringMask", __FUNCSIG__);
		try
		{
			int MaskSize = 7, WhiteCnt = 0;
			int Left = XPt - MaskSize / 2;
			int Right = Left + MaskSize - 1;
			int Top = YPt - MaskSize / 2;
			int Bottom = Top + MaskSize - 1;

			//each column to column
			for(int X = Left; X <= Right; X++)
			{
				//each row to row
				for(int Y = Top; Y <= Bottom; Y++)
				{
					if(iBytes[X + Y * ImageWidth] > WhiteThreshold)
						//Count the number of pixels that have value more than the threshold
						WhiteCnt++;
				}
			}

			double CntPercentage = WhiteCnt / pow((double)MaskSize,2); //Get the percentage of white pixels
			
			if(CntPercentage > 0.5 || CntPercentage < 0.3)
				return false;		
			//if(CntPercentage > 0.7 || CntPercentage < 0.3)
			//if(CntPercentage > 0.6 || CntPercentage < 0.4)
				//if percentage not between 30 and 70, then return false				

			//Percentage nearly 50, so qualify as edge point
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0029", __FILE__, __FUNCSIG__); return false; }
	}

	double EdgeDetectionSingleChannel::CalculateAvgForRectangle(int* Box)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "CalculateAvgForRectangle", __FUNCSIG__);
		try
		{
			int LeftBoundry = Box[0];
			int RightBoundry = Box[0] + Box[2] - 1;
			int TopBoundry = Box[1];
			int BottomBoundry = Box[1] + Box[3] - 1;
			double Avg = 0;
			for(int YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
			{
				for(int XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					Avg += iBytes[XIterator + YIterator * ImageWidth];
				}
			}
			return Avg / (Box[2] * Box[3]);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0029a", __FILE__, __FUNCSIG__); return 0; }
	}

	//Calculate average for current FG rectangle
	double EdgeDetectionSingleChannel::CalculateAvg(int *Box)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "EdgeDetectionSingleChannel", __FUNCSIG__);
		try
		{
			/*int LeftBoundry = Box[0];
			int RightBoundry = Box[0] + Box[2] - 1;
			int TopBoundry = Box[1];
			int BottomBoundry = Box[1] + Box[3] - 1;*/
			int LeftBoundry = 0;
			int RightBoundry = ImageWidth - 1;
			int TopBoundry = 0;
			int BottomBoundry = ImageHeight - 1;
			double Avg = 0;
			for(int YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
			{
				for(int XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					Avg += iBytes[XIterator + YIterator * ImageWidth];
				}
			}
			//return Avg / (Box[2] * Box[3]);
			return Avg / (ImageWidth * ImageHeight);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0029a", __FILE__, __FUNCSIG__); return 0; }
	}
	
	//For Ankit's new component edge
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeHelper1(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeHelper1", __FUNCSIG__);
		try
		{
		int XIterator, YIterator;
		bool FirstBandStart = false, FirstBandEnd = false, SecondBandStart = false, GotEdge = false;
		int FirstBandCount, SecondBandCount, WhiteBandCount;
		double EdgePoint;
		int FirstTolerance = 160, SecondTolerance = 100, WhiteBandTolerance = 160;
		struct Pt Point;
		int LeftBoundry = Box[0] + BoundryBuffer;
		int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
		int TopBoundry = Box[1] + BoundryBuffer;
		int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
		int PixelArrayIndex;
						
		switch(Scan_Direction)
		{
		case Rightwards:
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
				{
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						if(!FirstBandStart)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandStart = true;
								FirstBandCount = 1;
							}														
						}
						else if(!FirstBandEnd)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandCount++;
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance && FirstBandCount >= 10)
							{
								FirstBandEnd = true;
								WhiteBandCount = 1;
							}
							else
							{
								FirstBandStart = false;
							}
						}
						else if(!SecondBandStart)
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								if(WhiteBandCount == 0)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
								else
								{
									SecondBandStart = true;
									SecondBandCount = 1;
									EdgePoint = XIterator - 0.5;
								}
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance)
							{
								WhiteBandCount++;
								if(WhiteBandCount == 4)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
							}
						}
						else
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								SecondBandCount++;
								if(SecondBandCount == 10)
								{
									GotEdge = true;
									break;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
								SecondBandStart = false;
							}
						}
					}
					
					if(GotEdge)
					{
						Point.X = EdgePoint;
						Point.Y = YIterator;
						PointsList->push_back(Point);
						GotEdge = false;
					}

					FirstBandStart = false;
					FirstBandEnd = false;
					SecondBandStart = false;
				}				
			break;
		case Leftwards:
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
				{
					for(XIterator = RightBoundry; XIterator >= LeftBoundry; XIterator--)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						if(!FirstBandStart)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandStart = true;
								FirstBandCount = 1;
							}														
						}
						else if(!FirstBandEnd)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandCount++;
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance && FirstBandCount >= 5)
							{
								FirstBandEnd = true;
								WhiteBandCount = 1;
							}
							else
							{
								FirstBandStart = false;
							}
						}
						else if(!SecondBandStart)
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								if(WhiteBandCount == 0)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
								else
								{
									SecondBandStart = true;
									SecondBandCount = 1;
									EdgePoint = XIterator + 0.5;
								}
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance)
							{
								WhiteBandCount++;
								if(WhiteBandCount == 4)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
							}
						}
						else
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								SecondBandCount++;
								if(SecondBandCount == 5)
								{
									GotEdge = true;
									break;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
								SecondBandStart = false;
							}
						}
					}

					if(GotEdge)
					{
						Point.X = EdgePoint;
						Point.Y = YIterator;
						PointsList->push_back(Point);
						GotEdge = false;
					}

					FirstBandStart = false;
					FirstBandEnd = false;
					SecondBandStart = false;
				}			
			break;
		case Downwards:
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						if(!FirstBandStart)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandStart = true;
								FirstBandCount = 1;
							}														
						}
						else if(!FirstBandEnd)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandCount++;
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance && FirstBandCount >= 5)
							{
								FirstBandEnd = true;
								WhiteBandCount = 1;
							}
							else
							{
								FirstBandStart = false;
							}
						}
						else if(!SecondBandStart)
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								if(WhiteBandCount == 0)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
								else
								{
									SecondBandStart = true;
									SecondBandCount = 1;
									EdgePoint = YIterator - 0.5;
								}
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance)
							{
								WhiteBandCount++;
								if(WhiteBandCount == 4)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
							}
						}
						else
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								SecondBandCount++;
								if(SecondBandCount == 5)
								{
									GotEdge = true;
									break;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
								SecondBandStart = false;
							}
						}
					}

					if(GotEdge)
					{
						Point.X = XIterator;
						Point.Y = EdgePoint;
						PointsList->push_back(Point);
						GotEdge = false;
					}

					FirstBandStart = false;
					FirstBandEnd = false;
					SecondBandStart = false;
				}				
			break;
		case Upwards:
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					for(YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator--)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						if(!FirstBandStart)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandStart = true;
								FirstBandCount = 1;
							}														
						}
						else if(!FirstBandEnd)
						{
							if(iBytes[PixelArrayIndex] < FirstTolerance)
							{
								FirstBandCount++;
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance && FirstBandCount >= 5)
							{
								FirstBandEnd = true;
								WhiteBandCount = 1;
							}
							else
							{
								FirstBandStart = false;
							}
						}
						else if(!SecondBandStart)
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								if(WhiteBandCount == 0)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
								else
								{
									SecondBandStart = true;
									SecondBandCount = 1;
									EdgePoint = YIterator + 0.5;
								}
							}
							else if(iBytes[PixelArrayIndex] > WhiteBandTolerance)
							{
								WhiteBandCount++;
								if(WhiteBandCount == 4)
								{
									FirstBandStart = false;
									FirstBandEnd = false;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
							}
						}
						else
						{
							if(iBytes[PixelArrayIndex] < SecondTolerance)
							{
								SecondBandCount++;
								if(SecondBandCount == 5)
								{
									GotEdge = true;
									break;
								}
							}
							else
							{
								FirstBandStart = false;
								FirstBandEnd = false;
								SecondBandStart = false;
							}
						}
					}

					if(GotEdge)
					{
						Point.X = XIterator;
						Point.Y = EdgePoint;
						PointsList->push_back(Point);
						GotEdge = false;
					}

					FirstBandStart = false;
					FirstBandEnd = false;
					SecondBandStart = false;
				}			
			break;
		}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0030", __FILE__, __FUNCSIG__); }
	}

	//Brakes India component
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeHelper2(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeHelper2", __FUNCSIG__);
		try
		{
			/*
			The difference in focus metric values will be maximum at the edge, and thats how we determine the position of the edge.
			If the edge is vertical(and we need to perform horizontal scan), then the blocks are alligned in this way for comparison:
									
						!edge
				 __x___ ! __x___ 
				|	   |!|	    |	x = BlockThickness
			   y|	   |!|	    |y  y = BlockLength
				|______|!|______|
						!
						!
			
			If the edge is horizontal(and we need to perform vertical scan), then the blocks are alligned in this way for comparison:
					 __y___	
					|	   |
					|	   |x
					|______|
				------------------	
					 __y___
					|	   |
					|	   |x
					|______|


			*/
			int XIterator, YIterator, LookAhead = Type4_LookAheadVal, LookAheadIndex, i;
			struct Pt Point;
			int XIncrementer, YIncrementer;
			int AcrossEdgeIncementer = Type4_AcrossEdgeIncr, AlongEdgeIncrementer = Type4_AlongEdgeIncr; //These 2 vars specify the gap in pixel coordinates between 2 adjacent blocks
			double *TextureArray;
			int PixelArrayIndex;
			double TextureDiff, TextureThreshold = Type4_TextureDiffThreshold;
			int BlockLength = Type4_AlongEdgeBxThickness, BlockThickness = Type4_AcrossEdgeBxThickness;
			int BlockWidth, BlockHeight;
			
			int NoOfRows, NoOfCols;
							
			int LeftBoundry = 0;
			int RightBoundry;
			int TopBoundry = 0;
			int BottomBoundry;

			int BlockCompareJump;

			/*int BoundryBufferH = 10, BoundryBufferV = 30;
			
			int LeftBoundry = Box[0] + BoundryBufferH;
			int RightBoundry = Box[0] + Box[2] - BoundryBufferH - 1;
			int TopBoundry = Box[1] + BoundryBufferV;
			int BottomBoundry = Box[1] + Box[3] - BoundryBufferV - 1;

			char FileString[] = "Z:\\Texture.csv";
			ofstream FileWriter;
			FileWriter.open(FileString);
			for(XIterator = BoundryBufferH; XIterator <= ImageWidth - BoundryBufferH - 1; XIterator += 2)
			{
				TextureDiff = TextureDifference(XIterator, ImageHeight / 2, Horizontal);
				FileWriter << XIterator << ", " << TextureDiff << endl;
			}
			FileWriter.close();
			return;*/

			double MaxTextureDiff = 0;
			int PeakIndex;
			switch(Scan_Direction)
			{
			case Rightwards:			
					XIncrementer = AcrossEdgeIncementer; YIncrementer = AlongEdgeIncrementer;
					BlockWidth = BlockThickness; BlockHeight = BlockLength;
					BlockCompareJump = BlockThickness / AcrossEdgeIncementer;
					TextureArray = (double*)malloc((int)(Box[2] / XIncrementer + 0.5) * (int)(Box[3] / YIncrementer + 0.5) * sizeof(double));

					//Get an array of focus metrics with specified block size
					//And at an interval of "XIncrementer" pixels in the horizontal direction
					//At an interval of "YIncrementer" pixels in the vertical direction
					GetTextureArray(Box, BlockWidth, BlockHeight, XIncrementer, YIncrementer, &NoOfRows, &NoOfCols, TextureArray);

					RightBoundry = NoOfCols - 1;
					BottomBoundry = NoOfRows - 1;

					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						for(XIterator = LeftBoundry; XIterator <= RightBoundry - BlockCompareJump; XIterator++)
						{
							PixelArrayIndex = XIterator + YIterator * NoOfCols;
								
							//Always compare two blocks that do not overlap. But adjacent blocks overlap. Hence the addition of "BlockCompareJump"
							TextureDiff = abs(TextureArray[PixelArrayIndex + BlockCompareJump] - TextureArray[PixelArrayIndex]);
														
							//If the diff value in focus metric exceeds the threshold, then we are near to the edge.
							//Search locally for the next few pixels for a peak in difference value. The peak will most probably be at the edge itself
							if(TextureDiff >= TextureThreshold)
							{
								if(XIterator + LookAhead <= RightBoundry - BlockCompareJump)
									LookAheadIndex = LookAhead;
								else
									LookAheadIndex = RightBoundry - BlockCompareJump - XIterator;
																
								MaxTextureDiff = TextureDiff;
								PeakIndex = XIterator;
								for(i = 1; i <= LookAheadIndex; i++)
								{
									TextureDiff = abs(TextureArray[PixelArrayIndex + i + BlockCompareJump] - TextureArray[PixelArrayIndex + i]);
									if(TextureDiff > MaxTextureDiff)
									{
										MaxTextureDiff = TextureDiff;
										PeakIndex = XIterator + i;
									}
								}								
								
								/* 
								Now once we determine the 2 blocks with highest peak in difference, we need to zero-in on it within 1 pixel.
								The following diagram(when scaning from left to right) explains it
								 ______	______
								|	   |	  |
								|	   o <--(x,y) The required edge point
								|______|______|
								 ^		  ^
								 |        |___
								 |			  (PeakIndex + BlockCompareJump, YIterator)-coordinate in the focus metric array
								(PeakIndex, YIterator)-coordinate in the focus metric array

								*/
								Point.X = Box[0] + PeakIndex * XIncrementer + BlockWidth - 0.5;
								Point.Y = Box[1] + YIterator * YIncrementer + (BlockHeight / 2) - 0.5;
							
								PointsList->push_back(Point);
								break;
							}
						}											
					}
					free(TextureArray);
				break;
			case Leftwards:
					XIncrementer = AcrossEdgeIncementer; YIncrementer = AlongEdgeIncrementer;
					BlockWidth = BlockThickness; BlockHeight = BlockLength;
					BlockCompareJump = BlockThickness / AcrossEdgeIncementer;
					TextureArray = (double*)malloc((int)(Box[2] / XIncrementer + 0.5) * (int)(Box[3] / YIncrementer + 0.5) * sizeof(double));

					//Explained as above
					GetTextureArray(Box, BlockWidth, BlockHeight, XIncrementer, YIncrementer, &NoOfRows, &NoOfCols, TextureArray);

					RightBoundry = NoOfCols - 1;
					BottomBoundry = NoOfRows - 1;

					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						for(XIterator = RightBoundry; XIterator >= LeftBoundry + BlockCompareJump; XIterator--)
						{
							PixelArrayIndex = XIterator + YIterator * NoOfCols;
								
							TextureDiff = abs(TextureArray[PixelArrayIndex - BlockCompareJump] - TextureArray[PixelArrayIndex]);
														
							//Explained as above
							if(TextureDiff >= TextureThreshold)
							{
								if(XIterator - LookAhead >= LeftBoundry + BlockCompareJump)
									LookAheadIndex = LookAhead;
								else
									LookAheadIndex = XIterator - (LeftBoundry + BlockCompareJump);
																
								MaxTextureDiff = TextureDiff;
								PeakIndex = XIterator;
								for(i = 1; i <= LookAheadIndex; i++)
								{
									TextureDiff = abs(TextureArray[PixelArrayIndex - i - BlockCompareJump] - TextureArray[PixelArrayIndex - i]);
									if(TextureDiff > MaxTextureDiff)
									{
										MaxTextureDiff = TextureDiff;
										PeakIndex = XIterator - i;
									}
								}

								/* 
								Now once we determine the 2 blocks with highest peak in difference, we need to zero-in on it within 1 pixel.
								The following diagram(when scaning from right to left) explains it
								 ______	______
								|	   |	  |
								|	   o <--(x,y) The required edge point
								|______|______|
								 ^		  ^
								 |        |___
								 |			  (PeakIndex, YIterator)-coordinate in the focus metric array
								(PeakIndex - BlockCompareJump, YIterator)-coordinate in the focus metric array

								*/
								Point.X = Box[0] + PeakIndex * XIncrementer - 0.5;
								Point.Y = Box[1] + YIterator * YIncrementer + (BlockHeight / 2) - 0.5;
							
								PointsList->push_back(Point);
								break;
							}
						}											
					}
					free(TextureArray);
				break;
			case Downwards:			
					XIncrementer = AlongEdgeIncrementer; YIncrementer = AcrossEdgeIncementer;
					BlockWidth = BlockLength; BlockHeight = BlockThickness;
					BlockCompareJump = BlockThickness / AcrossEdgeIncementer;
					TextureArray = (double*)malloc((int)(Box[2] / XIncrementer + 0.5) * (int)(Box[3] / YIncrementer + 0.5) * sizeof(double));

					//Explained as above
					GetTextureArray(Box, BlockWidth, BlockHeight, XIncrementer, YIncrementer, &NoOfRows, &NoOfCols, TextureArray);

					RightBoundry = NoOfCols - 1;
					BottomBoundry = NoOfRows - 1;

					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						for(YIterator = TopBoundry; YIterator <= BottomBoundry - BlockCompareJump; YIterator++)
						{
							PixelArrayIndex = XIterator + YIterator * NoOfCols;
								
							TextureDiff = abs(TextureArray[PixelArrayIndex + BlockCompareJump * NoOfCols] - TextureArray[PixelArrayIndex]);
														
							//Explained as above
							if(TextureDiff >= TextureThreshold)
							{
								if(YIterator + LookAhead <= BottomBoundry - BlockCompareJump)
									LookAheadIndex = LookAhead;
								else
									LookAheadIndex = BottomBoundry - BlockCompareJump - YIterator;
																
								MaxTextureDiff = TextureDiff;
								PeakIndex = YIterator;
								for(i = 1; i <= LookAheadIndex; i++) 
								{
									TextureDiff = abs(TextureArray[PixelArrayIndex + (i + BlockCompareJump) * NoOfCols] - TextureArray[PixelArrayIndex + i * NoOfCols]);
									if(TextureDiff > MaxTextureDiff)
									{
										MaxTextureDiff = TextureDiff;
										PeakIndex = YIterator + i;
									}
								}

								/* 
								Now once we determine the 2 blocks with highest peak in difference, we need to zero-in on it within 1 pixel.
								The following diagram(when scaning from top to bottom) explains it

									 ______(x,y) The required edge point
								 ___|___	
								|	|   |
								|	v   |<----(XIterator, PeakIndex) -coordinate in the focus metric array
								|___o___|
								|	    |
								|	    |<----(XIterator, PeakIndex + BlockCompareJump) -coordinate in the focus metric array
								|_______|
								
								*/
								Point.X = Box[0] + XIterator * XIncrementer + (BlockWidth / 2) - 0.5;
								Point.Y = Box[1] + PeakIndex * YIncrementer + BlockHeight - 0.5;
							
								PointsList->push_back(Point);
								break;
							}
						}											
					}
					free(TextureArray);
				break;
			case Upwards:
					XIncrementer = AlongEdgeIncrementer; YIncrementer = AcrossEdgeIncementer;
					BlockWidth = BlockLength; BlockHeight = BlockThickness;
					BlockCompareJump = BlockThickness / AcrossEdgeIncementer;
					TextureArray = (double*)malloc((int)(Box[2] / XIncrementer + 0.5) * (int)(Box[3] / YIncrementer + 0.5) * sizeof(double));

					GetTextureArray(Box, BlockWidth, BlockHeight, XIncrementer, YIncrementer, &NoOfRows, &NoOfCols, TextureArray);

					RightBoundry = NoOfCols - 1;
					BottomBoundry = NoOfRows - 1;

					//As explained above
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						for(YIterator = BottomBoundry; YIterator >= TopBoundry + BlockCompareJump; YIterator--)
						{
							PixelArrayIndex = XIterator + YIterator * NoOfCols;
								
							//As explained above
							TextureDiff = abs(TextureArray[PixelArrayIndex - BlockCompareJump * NoOfCols] - TextureArray[PixelArrayIndex]);
														
							if(TextureDiff >= TextureThreshold)
							{
								if(YIterator - LookAhead >= TopBoundry + BlockCompareJump)
									LookAheadIndex = LookAhead;
								else
									LookAheadIndex = YIterator - (TopBoundry + BlockCompareJump);
																
								MaxTextureDiff = TextureDiff;
								PeakIndex = YIterator;
								for(i = 1; i <= LookAheadIndex; i++) 
								{
									TextureDiff = abs(TextureArray[PixelArrayIndex - (i + BlockCompareJump) * NoOfCols] - TextureArray[PixelArrayIndex - i * NoOfCols]);
									if(TextureDiff > MaxTextureDiff)
									{
										MaxTextureDiff = TextureDiff;
										PeakIndex = YIterator - i;
									}
								}

								/* 
								Now once we determine the 2 blocks with highest peak in difference, we need to zero-in on it within 1 pixel.
								The following diagram(when scaning from bottom to top) explains it

									 ______(x,y) The required edge point
								 ___|___	
								|	|   |
								|	v   |<----(XIterator, PeakIndex - BlockCompareJump) -coordinate in the focus metric array
								|___o___|
								|	    |
								|	    |<----(XIterator, PeakIndex) -coordinate in the focus metric array
								|_______|
								
								*/
								Point.X = Box[0] + XIterator * XIncrementer + (BlockWidth / 2) - 0.5;
								Point.Y = Box[1] + PeakIndex * YIncrementer - 0.5;
							
								PointsList->push_back(Point);
								break;
							}
						}											
					}
					free(TextureArray);
				break;
			}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0031", __FILE__, __FUNCSIG__); }
	}
		
	//To divide the image into grids of specified size and getting the 'FocusMetric' values for each block into an array
	void EdgeDetectionSingleChannel::GetTextureArray(int *Box, int BlockWidth, int BlockHeight, int HIncrementer, int VIncrementer, int *NoOfRows, int *NoOfCols, double *TextureArray)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "GetTextureArray", __FUNCSIG__);
		try
		{
			//The following array is to carry the block parameters for each of which the focus metric is to be calculated
			//Each block needs 4 parameters- x, y, width, height.....(x, y of the left top corner of the block)
			int *TextureBlockBoundry = (int*)malloc((int)(Box[2] / HIncrementer + 0.5) * (int)(Box[3] / VIncrementer + 0.5) * 4 * sizeof(int));
			
			int XIterator, YIterator, i = 0, j = 0, k = 0;
			int LeftBoundry = Box[0];
			int RightBoundry = Box[0] + Box[2] - BlockWidth;
			int TopBoundry = Box[1];
			int BottomBoundry = Box[1] + Box[3] - BlockHeight;
											
			for(YIterator = TopBoundry, i = 0; YIterator <= BottomBoundry; YIterator += VIncrementer, i++)
			{
				for(XIterator = LeftBoundry, j = 0; XIterator <= RightBoundry; XIterator += HIncrementer, j++)
				{
					//Array of X, Y, Width, Height for each block in that order
					TextureBlockBoundry[k * 4] = XIterator;			//X value of the left top corner of the block
					TextureBlockBoundry[k * 4 + 1] = YIterator;		//Y value of the left top corner of the block
					TextureBlockBoundry[k * 4 + 2] = BlockWidth;	//Width of the block
					TextureBlockBoundry[k * 4 + 3] = BlockHeight;	//Height of the block
					k++;
				}				
			}

			*NoOfCols = j;
			*NoOfRows = i;
			
			//The following function resides in the rapidcam class. It gets a value that relates to how much is a particular block focused(focus metric)
			//1st parameter- array of block parameters in the order- x1, y1, width1, height1, x2, y2....& so on, where 1 is for 1st block, 2 for 2nd block and so on.
			//2nd parameter- An array with enough space which will be filled by the function with focus metric values for each block in sequence
			//3rd parameter- Total number of blocks for which the focus metric is to be calculated
			BFit->GetFocusMetric(iBytes, ImageWidth, TextureBlockBoundry, TextureArray, k);
			free(TextureBlockBoundry);
			return;			
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0032", __FILE__, __FUNCSIG__); }
	}
	
	//Sathya's function added on 31/3/11. Good for plastic components
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeHelper3(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, double diffThreshold, int MaskSize)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeHelper3", __FUNCSIG__);
		try
		{
			int XIterator, YIterator, LocalMaximaExtent = Type5_LocalMaximaExtent;
			struct Pt Point;
			int LeftBoundry = Box[0] + BoundryBuffer;
			int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
			int TopBoundry = Box[1] + BoundryBuffer;
			int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
			double Sum1, Sum2, Maxvalue = 0;
			double TempXvalue, TempYvalue, Tempavg;
			bool FirstLineFlag = true;
			int LocalMaximaCount = 0;

			switch(Scan_Direction)
			{
			case Rightwards:
			case Leftwards:
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
				{
					FirstLineFlag = true;
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						long line1 = XIterator - (MaskSize - 1)/2 + (YIterator - (MaskSize - 1)/2) * ImageWidth;
						Sum1 = 0; Sum2 = 0;
						for(int i = 0; i < MaskSize; i++)
						{
							for(int n = 0; n < MaskSize; n++)
							{
								Sum1 += iBytes[line1 + n + ImageWidth * i];
								Sum2 += iBytes[line1 + (MaskSize + n) + ImageWidth * i]; 
							}
						}
						Tempavg = abs(Sum1 - Sum2)/(MaskSize * MaskSize);
						if(FirstLineFlag)
						{
							Maxvalue = Tempavg;
							TempXvalue = XIterator + ((MaskSize - 1)/ 2);
							FirstLineFlag = false;
							LocalMaximaCount = 0;
						}
						else
						{
							if(Tempavg > Maxvalue)
							{
								if(Tempavg > diffThreshold)
								{
									LocalMaximaCount = 0;
									Maxvalue = Tempavg;
									TempXvalue = XIterator + ((MaskSize - 1)/ 2);
								}
							}
							else
							{
								if(Maxvalue > diffThreshold)
								{
									LocalMaximaCount++;
									if(LocalMaximaCount >= LocalMaximaExtent)
										break;
								}
							}
						}
					}
					if(Maxvalue > diffThreshold)
					{
						Point.X = TempXvalue;
						Point.Y = YIterator;
						PointsList->push_back(Point);
					}
				}
				break;

			case Upwards:
			case Downwards:
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					FirstLineFlag = true;
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						long line1 = XIterator - (MaskSize -1)/2 + (YIterator - (MaskSize - 1)/2) * ImageWidth;
						Sum1 = 0; Sum2 = 0;
						for(int i = 0; i < MaskSize; i++)
						{
							for(int n = 0; n < MaskSize; n++)
							{
								Sum1 += iBytes[line1 + n + ImageWidth * i];
								Sum2 += iBytes[line1 + n + ImageWidth * (i + MaskSize)]; 
							}
						}
						Tempavg = abs(Sum1 - Sum2)/(MaskSize * MaskSize);
						if(FirstLineFlag)
						{
							Maxvalue = Tempavg;
							TempYvalue = YIterator + MaskSize/2;
							FirstLineFlag = false;
							LocalMaximaCount = 0;
						}
						else
						{
							if(Tempavg > Maxvalue)
							{
								if(Tempavg > diffThreshold)
								{
									LocalMaximaCount = 0;
									Maxvalue = Tempavg;
									TempYvalue = YIterator + MaskSize/2;
								}
							}
							else
							{
								if(Maxvalue > diffThreshold)
								{
									LocalMaximaCount++;
									if(LocalMaximaCount >= LocalMaximaExtent)
										break;
								}
							}
						}
					}
					if(Maxvalue > diffThreshold)
					{
						Point.X = XIterator;
						Point.Y = TempYvalue;
						PointsList->push_back(Point);
					}
				}
				break;
			}
			FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0033", __FILE__, __FUNCSIG__); }
	}
	
	//For Ankit's new component edge(Titanium Component-using median filtering)
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeHelper4(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeHelper4", __FUNCSIG__);
		try
		{
		int XIterator, YIterator;
		struct Pt Point;
		int LeftBoundry = Box[0] + BoundryBuffer;
		int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
		int TopBoundry = Box[1] + BoundryBuffer;
		int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
		double PrevAvgValue, PrevMedianValue, CurrMedianValue, FwdMedianValue;
		//double EdgePixelThreshold = 200, FwdPixelThreshold = 90, Threshold = 80, Threshold2 = 140, Threshold3 = 10;
		double EdgePixelThreshold = Type2_CurrAbsThreshold, FwdPixelThreshold = Type2_LookaheadAbsThreshold, Threshold = Type2_CurrDiffThreshold, Threshold2 = Type2_LookaheadDiffThreshold;
		int LookAhead = Type2_LookAheadVal, Median1FiltrBxSize = Type2_MedianFilterBxSize, Median2FiltrBxSize = Type2_AvgFilterBxSize;
		bool FalseEdge = false;
			
		/*int MedianVal;
		char FileString[] = "Z:\\Texture.csv";
		ofstream FileWriter;
		FileWriter.open(FileString);
		for(XIterator = 250; XIterator <= 550; XIterator += 1)
		{
			MedianVal = abs(GetMedianFilterValue(XIterator - 1, ImageHeight / 2, 3) - GetMedianFilterValue(XIterator, ImageHeight / 2, 3));
			FileWriter << XIterator << ", " << MedianVal << endl;
		}
		FileWriter.close();
		return;*/


		switch(Scan_Direction)
		{
		case Rightwards:
				//Move from one row to the next downwards
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
				{
					//Get the median value for the first pixel of the row
					PrevMedianValue = GetMedianFilterValue(LeftBoundry, YIterator, Median1FiltrBxSize);
					PrevAvgValue = GetMedianFilterValue(LeftBoundry, YIterator, Median2FiltrBxSize, true);
					//Move forward along the row rightwards
					for(XIterator = LeftBoundry + 1; XIterator <= RightBoundry; XIterator++)
					{
						//Get the median value of the next pixel
						CurrMedianValue = GetMedianFilterValue(XIterator, YIterator, Median1FiltrBxSize);

						PrevAvgValue = GetMedianFilterValue(XIterator - 4, YIterator, Median2FiltrBxSize);

						//Compare the two median values
						if(abs(PrevMedianValue - CurrMedianValue) >= Threshold && PrevMedianValue > EdgePixelThreshold)
						{
							if(XIterator + LookAhead > RightBoundry) break;

							//Now we need to check if the difference continues for the next few pixels
							//Only then it is a valid edge
							for(int i = XIterator + 1; i <= XIterator + LookAhead; i++)
							{
								//get the median values for the pixels
								FwdMedianValue = GetMedianFilterValue(i, YIterator, Median2FiltrBxSize, true);

								//Now if the difference is not within threshold,
								//Flag it as an invalid edge and break off from the loop
								if(abs(PrevMedianValue - FwdMedianValue) < Threshold2 || PrevAvgValue <= FwdMedianValue || FwdMedianValue > FwdPixelThreshold)
								{
									FalseEdge = true;
									break;
								}
							}

							//Once out of the loop, check if the edge is validated
							if(!FalseEdge)
							{
								if(PrevMedianValue > CurrMedianValue)
								{
									//Add the valid edge point to the list
									Point.X = XIterator - 0.5;
									Point.Y = YIterator;
									//if(BestFitFiltering(PointsList, Point))
										PointsList->push_back(Point);
									break; //If valid edge point, break from the present row and go to the next row for next point
								}
							}
							FalseEdge = false; //Reset the flag
						}

						//Update the median values
						PrevMedianValue = CurrMedianValue;						
					}					
				}				
			break;
		case Leftwards:
				//Move from one row to the next downwards
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
				{
					//Get the median value for the last pixel of the row
					PrevMedianValue = GetMedianFilterValue(RightBoundry, YIterator, Median1FiltrBxSize);
					PrevAvgValue = GetMedianFilterValue(RightBoundry, YIterator, Median2FiltrBxSize, true);
					//Move forward along the row leftwards
					for(XIterator = RightBoundry - 1; XIterator >= LeftBoundry; XIterator--)
					{
						//Get the median value of the next pixel
						CurrMedianValue = GetMedianFilterValue(XIterator, YIterator, Median1FiltrBxSize);

						PrevAvgValue = GetMedianFilterValue(XIterator + 4, YIterator, Median2FiltrBxSize);

						//Compare the two median values
						if(abs(PrevMedianValue - CurrMedianValue) >= Threshold && PrevMedianValue > EdgePixelThreshold)
						{
							if(XIterator - LookAhead < LeftBoundry) break;

							//Now we need to check if the difference continues for the next few pixels
							//Only then it is a valid edge
							for(int i = XIterator - 1; i >= XIterator - LookAhead; i--)
							{
								//get the median values for the pixels
								FwdMedianValue = GetMedianFilterValue(i, YIterator, Median2FiltrBxSize, true);

								//Now if the difference is not within threshold,
								//Flag it as an invalid edge and break off from the loop
								if(abs(PrevMedianValue - FwdMedianValue) < Threshold2 || PrevAvgValue <= FwdMedianValue || FwdMedianValue > FwdPixelThreshold)
								{
									FalseEdge = true;
									break;
								}
							}
							
							//Once out of the loop, check if the edge is validated
							if(!FalseEdge)
							{
								if(PrevMedianValue > CurrMedianValue)
								{
									//Add the valid edge point to the list
									Point.X = XIterator + 0.5;
									Point.Y = YIterator;
									//if(BestFitFiltering(PointsList, Point))
										PointsList->push_back(Point);
									break; //If valid edge point, break from the present row and go to the next row for next point
								}
							}
							FalseEdge = false; //Reset the flag
						}
						
						//Update the median values
						PrevMedianValue = CurrMedianValue;
					}					
				}			
			break;
		case Downwards:
				//Move from one column to the next rightwards	
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					//Get the median value for the first pixel of the column
					PrevMedianValue = GetMedianFilterValue(XIterator, TopBoundry, Median1FiltrBxSize);
					PrevAvgValue = GetMedianFilterValue(XIterator, TopBoundry, Median2FiltrBxSize, true);
					//Move forward along the column downwards
					for(YIterator = TopBoundry + 1; YIterator <= BottomBoundry; YIterator++)
					{
						//Get the median value of the next pixel
						CurrMedianValue = GetMedianFilterValue(XIterator, YIterator, Median1FiltrBxSize);

						PrevAvgValue = GetMedianFilterValue(XIterator, YIterator - 4, Median2FiltrBxSize);

						//Compare the two median values
						if(abs(PrevMedianValue - CurrMedianValue) >= Threshold && PrevMedianValue > EdgePixelThreshold)
						{
							if(YIterator + LookAhead > BottomBoundry) break;

							//Now we need to check if the difference continues for the next few pixels
							//Only then it is a valid edge
							for(int i = YIterator + 1; i <= YIterator + LookAhead; i++)
							{
								//get the median values for the pixels
								FwdMedianValue = GetMedianFilterValue(XIterator, i, Median2FiltrBxSize, true);

								//Now if the difference is not within threshold,
								//Flag it as an invalid edge and break off from the loop
								if(abs(PrevMedianValue - FwdMedianValue) < Threshold2 || PrevAvgValue <= FwdMedianValue || FwdMedianValue > FwdPixelThreshold)
								{
									FalseEdge = true;
									break;
								}
							}
							
							//Once out of the loop, check if the edge is validated
							if(!FalseEdge)
							{
								if(PrevMedianValue > CurrMedianValue)
								{
									//Add the valid edge point to the list
									Point.X = XIterator;
									Point.Y = YIterator - 0.5;
									//if(BestFitFiltering(PointsList, Point))
										PointsList->push_back(Point);
									break; //If valid edge point, break from the present column and go to the next column for next point
								}
							}
							FalseEdge = false; //Reset the flag
						}
						
						//Update the median values
						PrevMedianValue = CurrMedianValue;												
					}					
				}				
			break;
		case Upwards:
				//Move from one column to the next rightwards
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					//Get the median value for the last pixel of the column
					PrevMedianValue = GetMedianFilterValue(XIterator, BottomBoundry, Median1FiltrBxSize);
					PrevAvgValue = GetMedianFilterValue(XIterator, BottomBoundry, Median2FiltrBxSize, true);
					//Move forward along the column upwards
					for(YIterator = BottomBoundry - 1; YIterator >= TopBoundry; YIterator--)
					{
						//Get the median value of the next pixel
						CurrMedianValue = GetMedianFilterValue(XIterator, YIterator, Median1FiltrBxSize);

						PrevAvgValue = GetMedianFilterValue(XIterator, YIterator + 4, Median2FiltrBxSize);

						//Compare the two median values
						if(abs(PrevMedianValue - CurrMedianValue) >= Threshold && PrevMedianValue > EdgePixelThreshold)
						{
							if(YIterator - LookAhead < TopBoundry) break;

							//Now we need to check if the difference continues for the next few pixels
							//Only then it is a valid edge
							for(int i = YIterator - 1; i >= YIterator - LookAhead; i--)
							{
								//get the median values for the pixels
								FwdMedianValue = GetMedianFilterValue(XIterator, i, Median2FiltrBxSize, true);

								//Now if the difference is not within threshold,
								//Flag it as an invalid edge and break off from the loop
								if(abs(PrevMedianValue - FwdMedianValue) < Threshold2 || PrevAvgValue <= FwdMedianValue || FwdMedianValue > FwdPixelThreshold)
								{
									FalseEdge = true;
									break;
								}
							}
							
							//Once out of the loop, check if the edge is validated
							if(!FalseEdge)
							{
								if(PrevMedianValue > CurrMedianValue)
								{
									//Add the valid edge point to the list
									Point.X = XIterator;
									Point.Y = YIterator + 0.5;
									//if(BestFitFiltering(PointsList, Point))
										PointsList->push_back(Point);
									break; //If valid edge point, break from the present column and go to the next column for next point
								}
							}
							FalseEdge = false; //Reset the flag
						}
						
						//Update the median values
						PrevMedianValue = CurrMedianValue;								
					}					
				}			
			break;
		}
		FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0034", __FILE__, __FUNCSIG__); }
	}
	
	//Function to find the median filter value with a n pixel mask


	//double EdgeDetectionSingleChannel::GetMedianFilterValue(int X, int Y, int MaskSize, bool GetAvg)
	//{
	//	try
	//	{
	//		int LeftEnd = X - MaskSize / 2;
	//		int RightEnd = LeftEnd + MaskSize - 1;
	//		int TopEnd = Y - MaskSize / 2;
	//		int BottomEnd = TopEnd + MaskSize - 1;
	//		double *Array = new double[MaskSize * MaskSize];
	//		double ReturnVal = 0, Sum = 0;
	//		int cnt = 0;

	//		//Put all the pixel values in the mask block into an array
	//		for(int i = LeftEnd; i <= RightEnd; i++)
	//		{
	//			for(int j = TopEnd; j <= BottomEnd; j++)
	//			{
	//				Sum += iBytes[i + j * ImageWidth];
	//				Array[cnt] = iBytes[i + j * ImageWidth];
	//				cnt++;
	//			}
	//		}

	//		if(GetAvg)
	//		{
	//			ReturnVal = Sum / cnt;
	//		}
	//		else
	//		{
	//			double temp = 0;
	//			for(int i = 0; i < cnt; i++)
	//				for(int j = 0; j < cnt - i - 1; j++)
	//					if(Array[j] > Array[j+1])
	//					{
	//						temp = Array[j];
	//						Array[j] = Array[j + 1];
	//						Array[j + 1] = temp;
	//					}

	//			ReturnVal = Array[cnt / 2];
	//		}
	//		
	//		delete [] Array;
	//		return ReturnVal;
	//	}
	//	catch(...){ SetAndRaiseErrorMessage("EDSC0035", __FILE__, __FUNCSIG__); return 0; }
	//}
	double EdgeDetectionSingleChannel::GetMedianFilterValue(int X, int Y, int MaskSize, bool GetAvg)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "GetMedianFilterValue", __FUNCSIG__);
		try
		{
			int LeftEnd = X - MaskSize / 2;
			int RightEnd = LeftEnd + MaskSize - 1;
			int TopEnd = Y - MaskSize / 2;
			int BottomEnd = TopEnd + MaskSize - 1;
			double *Array = new double[MaskSize * MaskSize];
			double ReturnVal = 0, Sum = 0;
			int cnt = 0;

			int p = X + Y*ImageWidth;

			for (int i = -(MaskSize / 2); i <= MaskSize / 2; i++)
			{
				for (int j = -(MaskSize / 2); j <= MaskSize / 2; j++)
				{
					Array[cnt] = iBytes[p + (i * ImageWidth) + j];
					Sum += Array[cnt];
					cnt++;
				}
			}

			if (GetAvg)
			{
				ReturnVal = Sum / cnt;
			}
			else
			{
				double temp = 0;
				//for (int i = 0; i < cnt; i++)
				//{
				//	for (int j = 0; j < cnt - i - 1; j++)
				//	{
				//		if (Array[j] > Array[j + 1])
				//		{
				//			temp = Array[j];
				//			Array[j] = Array[j + 1];
				//			Array[j + 1] = temp;
				//		}
				//	}
				//}
				QuickSort(Array, 0, MaskSize * MaskSize - 1); // (int)(MaskSize / 4), (int)(3 * MaskSize / 4));
				ReturnVal = Array[cnt / 2];
			}

			delete[] Array;
			return ReturnVal;
		}
		catch (...){ SetAndRaiseErrorMessage("EDSC0035", __FILE__, __FUNCSIG__); return 0; }
	}

	void EdgeDetectionSingleChannel::QuickSort(double* arr, int left, int right)
	{		
		//SetAndRaiseErrorMessage("EDCSC0002", "QuickSort", __FUNCSIG__);
		int i = left, j = right;
		double tmp;
		double pivot = arr[(left + right) / 2];

		//     partition 
		while (i <= j) 
		{
			while (arr[i] < pivot)
				i++;
			while (arr[j] > pivot)
				j--;
			if (i <= j) {
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
				i++;
				j--;
			}
		}

		//      /* recursion */
		if (left < j)
			QuickSort(arr, left, j);
		if (i < right)
			QuickSort(arr, i, right);
	}

	//Function to filter out points from list using line best fit method
	void EdgeDetectionSingleChannel::FilterOutPtsFromList(std::list<struct Pt> *PointsList)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "FilterOutPtsFromList", __FUNCSIG__);
		double* PtsArrayForBestFit;
		try
		{
			if (PointsList->size() < 10) return;
			double LnParam[2] = { 0 }, Dist = 0; 
			int i = 0, j = 0, k = 0, NoOfPtsForBestFit = 20; // PointsList->size() / 5, 
			int DistTolerance = 3;
			//if (NoOfPtsForBestFit < 5) NoOfPtsForBestFit = 5;
			PtsArrayForBestFit = (double*)malloc(2 * NoOfPtsForBestFit * sizeof(double));
			memset((void*)PtsArrayForBestFit, 0, 2 * NoOfPtsForBestFit * sizeof(double));
			std::list<Pt>::iterator itr, tmpitr;

			//Start form the start of the list
			for(i = 0, itr = PointsList->begin(); itr != PointsList->end(); i++, itr++)
			{
				//if the points count do not reach a reqd number, keep adding the points into the array
				if(i < NoOfPtsForBestFit)
				{
					PtsArrayForBestFit[i * 2] = (*itr).X;
					PtsArrayForBestFit[i * 2 + 1] = (*itr).Y;
				}
				//When the count crosses a certain number, we have some work to do!!
				else
				{
					//Get the best fit line for the points that have to be filtered
					BFit->Line_BestFit_2D(PtsArrayForBestFit, NoOfPtsForBestFit, LnParam, false);

					//Now lets go through the array one by one examining each point
					for(j = 0; j < NoOfPtsForBestFit; j++)
					{
						//Get the distance of the current point from the line
						Pt2Line_Dist(&PtsArrayForBestFit[j * 2], LnParam[0], LnParam[1], &Dist);
						//if it crosses a certain limit, we need to erase it from the list
						if(Dist > DistTolerance)
						{
							//We need a temp iterator that points to the end of the present bunch of points i.e. pointing to a location previous to
							//the present loop iterator
							tmpitr = itr;
							tmpitr--;
							//Note: We went through the array in the forward direction. But our temporary list iterator points to a point
							//corresponding to the end of the array. So we iterate backwards to reach the desired point on the list
							//Dont worry about the following statement. Dont get your self confused. Its proper. Take pen and paper and
							//check it if you like. In the last loop, when k = j, the loop terminates leaving us right where we want to be.
							for(k = NoOfPtsForBestFit - 1; k > j; k--, tmpitr--);

							//Now we erase the appropriate point from the list
							PointsList->erase(tmpitr);
						}
					}
					//Reset the count
					i = 0;
					//Now we have filtered the previous bunch of points. Its time to move on. The current iterator points to the start of the next
					//set of points to be filtered. Lets add it to the array
					PtsArrayForBestFit[i * 2] = (*itr).X;
					PtsArrayForBestFit[i * 2 + 1] = (*itr).Y;
				}
			}
			free(PtsArrayForBestFit);
		}
		catch (...){ SetAndRaiseErrorMessage("EDSC0036", __FILE__, __FUNCSIG__); free(PtsArrayForBestFit); }
	}

	//Function to get best fit line for last 10 points and checking if the new point is close enough to this line
	bool EdgeDetectionSingleChannel::BestFitFiltering(std::list<struct Pt> *PointsList, Pt NewPt)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "BestFitFiltering", __FUNCSIG__);
		try
		{
			int NoOfPts = 10, DistTolerance = 5;
			double BFitPts[20], LnParam[2], Dist, TempNwPt[2];
			std::list<Pt>::iterator Itr = PointsList->end();
			//Itr--;

			if((int)PointsList->size() >= NoOfPts)
			{
				for(int i = 0; i < NoOfPts; i++)
				{
					Itr--;
					BFitPts[2 * i] = (Itr)->X;
					BFitPts[2 * i + 1] = (Itr)->Y;					
				}

				BFit->Line_BestFit_2D(BFitPts, NoOfPts, LnParam, false);

				TempNwPt[0] = NewPt.X;
				TempNwPt[1] = NewPt.Y;

				Pt2Line_Dist(TempNwPt, LnParam[0], LnParam[1], &Dist);

				if(Dist <= DistTolerance) return true;
				return false;
			}
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0037", __FILE__, __FUNCSIG__); return false; }
	}

	//Function to calculate the Point to line distance//
	bool EdgeDetectionSingleChannel::Pt2Line_Dist(double *point, double slope, double intercept, double *distance)
	{	
		//SetAndRaiseErrorMessage("EDCSC0002", "Pt2Line_Dist", __FUNCSIG__);
		try
		{
			//If the line is vertical//
			if(slope == 0.5 * M_PI || slope == 3 * M_PI_2)  
			{	
				//Distance between the point and line//
				*distance = abs(*point - intercept);
				return true;
			}
			//Distance between the point and line//
			//(mx1 + c - y1) * cos(slope)//
			*distance = abs((tan(slope) * (*point) + intercept - *(point + 1)) * cos(slope));
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0038", __FILE__, __FUNCSIG__); return false; }
	}

	//for bad lighting
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeNormalising(int *Box, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList)
	{
		SetAndRaiseErrorMessage("EDCSC0002", "DetectSurfaceEdgeNormalising", __FUNCSIG__);
		try
		{
		int XIterator, YIterator, LookAheadIndex, i;
		struct Pt Point;
		int LeftBoundry = Box[0] + BoundryBuffer;
		int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
		int TopBoundry = Box[1] + BoundryBuffer;
		int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
		int PixelArrayIndex;
			
		//====================Normalising values======================
		
		double AvgVal = 0;
		double BlackTolerence = 0;
		double BlackThresholdPercentage = 0.6;
		int FilteringMaskTolerence = 70;
		double WhiteThresholdPercentage = 0.6;
		int LookAhead = 10;
		
		ifstream InfoFile;
		InfoFile.open("EDTestInfo.txt",ios::in);
		InfoFile >> BlackThresholdPercentage;
		InfoFile >> WhiteThresholdPercentage;
		InfoFile >> LookAhead;
		InfoFile.close();
		
		for(int j = Box[0]; j < Box[0] + Box[1]; j++)
			for(int i = Box[2]; i < Box[2] + Box[3]; i++)
			{
				AvgVal += iBytes[j * ImageWidth + i];
			}

		AvgVal = AvgVal / (Box[1] * Box[3]);
		BlackTolerence = AvgVal * BlackThresholdPercentage;
		FilteringMaskTolerence = (int)(AvgVal * WhiteThresholdPercentage);
		//============================================================
		 
		switch(Scan_Direction)
		{
		case Rightwards:
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
				{
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
						//for this purpose, first it must be determined till where this checking must be carried out
						if((XIterator + LookAhead) < ImageWidth - BoundryBuffer)
							//the checking can continue for 'look ahead' number of pixels
							LookAheadIndex = LookAhead;
						else
							//checking can happen only till the end of the column, as 'look ahead' number exceeds the column boundry
							LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
						//This is where the actual checking happens the range for which has been determined in the previous if statement
						for(i = 1; i <=  LookAheadIndex; i++)
							if(iBytes[PixelArrayIndex + i] > BlackTolerence)
							{
								//its not a legitimate edge as there is no continuity of transition
								//hence jump to the point where the check failed and continue searching for edge from this point forward
								XIterator = XIterator + i;
								goto last1;
							}
						
						if(XIterator == LeftBoundry)
							break;

						if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence))
							//filter the edge points. Only if it passes, its added to the list
							break;

						//Ah!! we have an edge
						Point.X = XIterator - 0.5;
						Point.Y = YIterator;
						
						PointsList->push_back(Point);
						break;
					
last1:					i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
					}
				}				
			break;
		case Leftwards:
				for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
				{
					for(XIterator = RightBoundry; XIterator >= LeftBoundry; XIterator--)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						if((XIterator - LookAhead) >= BoundryBuffer)
							LookAheadIndex = LookAhead;
						else
							LookAheadIndex = XIterator - BoundryBuffer;
					
						for(i = 1; i <=  LookAheadIndex; i++)
							if(iBytes[PixelArrayIndex - i] > BlackTolerence)
							{
								XIterator = XIterator - i;
								goto last2;
							}

						if(XIterator == RightBoundry)
							break;

						if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence))
							break;

						Point.X = XIterator + 0.5;
						Point.Y = YIterator;
						
						PointsList->push_back(Point);
						break;

last2:					i = 1; //the dummy statement
					}
				}			
			break;
		case Downwards:
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						if((YIterator + LookAhead) < ImageHeight - BoundryBuffer)
							LookAheadIndex = LookAhead * ImageWidth;
						else
							LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
					
						for(i = 1 * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
							if(iBytes[PixelArrayIndex + i] > BlackTolerence)
							{
								YIterator = YIterator + i /ImageWidth;
								goto last3;
							}
						
						if(YIterator == TopBoundry)
							break;

						if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence))
							break;

						Point.X = XIterator;
						Point.Y = YIterator - 0.5;
						
						PointsList->push_back(Point);
						break;

last3:					i = 1;	//the dummy statement
					}					
				}				
			break;
		case Upwards:
				for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
				{
					for(YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator--)
					{
						PixelArrayIndex = XIterator + YIterator * ImageWidth;

						//the following if statement searches for a definite edge. If not found, it skips to the next loop
						if(iBytes[PixelArrayIndex] > BlackTolerence)
							continue;

						if((YIterator - LookAhead) >= BoundryBuffer)
							LookAheadIndex = LookAhead * ImageWidth;
						else
							LookAheadIndex = (YIterator - BoundryBuffer) * ImageWidth;
					
						for(i = 1 * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
							if(iBytes[PixelArrayIndex - i] > BlackTolerence)
							{
								YIterator = YIterator - i / ImageWidth;
								goto last4;
							}
						
						if(YIterator == TopBoundry)
							break;

						if(!FilteringMask(XIterator, YIterator, FilteringMaskTolerence))
							break;

						Point.X = XIterator;
						Point.Y = YIterator + 0.5;
						
						PointsList->push_back(Point);
						break;

last4:					i = 1; //the dummy statement
					}
				}			
			break;
		}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0039", __FILE__, __FUNCSIG__); return; }
	}

	//---------------------------------------------------------------------------------------
	//Profile Edge Detection for lower resolution edges
	//---------------------------------------------------------------------------------------
	
	//Public function to detect profile edge when the resolution of the edge is low (for rectangular box)
	int EdgeDetectionSingleChannel::DetectEdgeLowFocus(int *FrameGrabBox, int Scan_Direction)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			std::list<struct Pt> PointsList;
			int i = 0;
			int Box[4] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3]};

			/*Box[1] = ImageHeight - Box[1] - Box[3];

			if(Scan_Direction == Upwards) Scan_Direction = Downwards;
			else if(Scan_Direction == Downwards) Scan_Direction = Upwards;*/
			
			DetectProfileEdgeLowerResolution(Box, (ScanDirection) Scan_Direction, 10, &PointsList);
			
			////Now the list has all the points as Pt structure
			////before sending it, we need to put all the values in a double array in X1 Y1 X2 Y2... fashion
			//if(PixelAveraged)
			//{
			//	for each(Pt Pnt in PointsList)
			//	{
			//		if(PixelVariance[(int)(Pnt.X + 0.5) + ((int)(Pnt.Y + 0.5) * ImageWidth)] <= VarianceCutOff)
			//		{
			//			DetectedPointsList[2 * i] = Pnt.X;		//X value in even position
			//			DetectedPointsList[2 * i + 1] = Pnt.Y;	//Y value in odd position
			//			//DetectedPointsList[2 * i + 1] = ImageHeight - 1 - Pnt.Y;	//Y value in odd position
			//			i++;
			//			if(i == 10000)
			//				return 10000;
			//		}
			//	}
			//}
			//else
			//{
			//	for each(Pt Pnt in PointsList)
			//	{
			//		DetectedPointsList[2 * i] = Pnt.X;		//X value in even position
			//		DetectedPointsList[2 * i + 1] = Pnt.Y;	//Y value in odd position
			//		//DetectedPointsList[2 * i + 1] = ImageHeight - 1 - Pnt.Y;	//Y value in odd position
			//		i++;
			//		if(i == 10000)
			//			return 10000;				
			//	}
			//}

			//return i;
			return AddEdgePtsToCollection(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0040", __FILE__, __FUNCSIG__); return 0; }
	}

	//Public function to detect profile edge when the resolution of the edge is low (for angular rect box)
	int EdgeDetectionSingleChannel::DetectEdgeLowFocusAngularRect(int *FrameGrabBox, bool OppositeScan)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			std::list<struct Pt> PointsList;
			int i = 0;
			int Box[5] = {FrameGrabBox[0], FrameGrabBox[1], FrameGrabBox[2], FrameGrabBox[3], FrameGrabBox[4]};

			/*Box[1] = ImageHeight - Box[1] - Box[3];

			if(Scan_Direction == Upwards) Scan_Direction = Downwards;
			else if(Scan_Direction == Downwards) Scan_Direction = Upwards;*/
			
			DetectProfileEdgeLowerResolutionAngularRect(Box, OppositeScan, 10, &PointsList);
			
			return AddEdgePtsToCollection(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0040", __FILE__, __FUNCSIG__); return 0; }
	}

	//Public function to detect profile edge when the resolution of the edge is low (for arc rect box)
	int EdgeDetectionSingleChannel::DetectEdgeLowFocusCurvedBox(double *FrameGrabBox, bool OppositeScan)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			std::list<struct Pt> PointsList;
			int i = 0;
			double Box[6] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3], FrameGrabBox[4], FrameGrabBox[5]};

			/*Box[1] = ImageHeight - Box[1] - Box[3];

			if(Scan_Direction == Upwards) Scan_Direction = Downwards;
			else if(Scan_Direction == Downwards) Scan_Direction = Upwards;*/
			
			DetectProfileEdgeLowerResolutionArcRect(Box, OppositeScan, 10, &PointsList);
			
			return AddEdgePtsToCollection(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0040", __FILE__, __FUNCSIG__); return 0; }
	}

	//Function to perform the scan in a rectangular area for the edge
	void EdgeDetectionSingleChannel::DetectProfileEdgeLowerResolution(int *Box, ScanDirection Scan_Direction, int Resolution, std::list<struct Pt> *PointsList)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			int XIterator, YIterator, Incrementer = Resolution / 2;
			double EdgePoint;
			struct Pt Point;
			int LeftBoundry = Box[0] + BoundryBuffer;
			int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
			int TopBoundry = Box[1] + BoundryBuffer;
			int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
			int LookAhead = 5;
			bool FalseEdge = false;
			double *ArrayBlock = (double*)malloc(sizeof(double) * Resolution);
			int BlackCutOff = 80;
			int WhiteCutoff = 175;
			int Diff = 0;

			switch(Scan_Direction)
			{
			case Rightwards:
					//Move from one row to the next downwards
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						//Move forward along the row rightwards
						for(XIterator = LeftBoundry; XIterator <= RightBoundry - 1 - Resolution; XIterator += Incrementer)
						{
							Diff = iBytes[YIterator * ImageWidth + XIterator] - iBytes[YIterator * ImageWidth + XIterator + Resolution - 1];
							//Compare the 2 pixel values which are "Resolution" no. of pixels apart
							if(abs(Diff) >= Tolerance)
							{
								if ( Diff > 0)
								{
									if (iBytes[YIterator * ImageWidth + XIterator + Resolution - 1] > BlackCutOff) 
										continue;
								}
								else
								{
									if (iBytes[YIterator * ImageWidth + XIterator + Resolution - 1] < WhiteCutoff) 
										continue;
								}
								//Now we need to check if the difference continues for the next few pixels
								//Only then it is a valid edge
								for(int i = XIterator + Resolution; i <= XIterator + Resolution - 1 + LookAhead && i <= RightBoundry; i++)
								{
									//Now if the difference is not within threshold,
									//Flag it as an invalid edge and break off from the loop
									if(abs(iBytes[YIterator * ImageWidth + XIterator] - iBytes[YIterator * ImageWidth + i]) < Tolerance)
									{
										FalseEdge = true;
										break;
									}
								}

								//Once out of the loop, check if the edge is validated
								if(!FalseEdge)
								{
									//Copy the pixel values into the array
									for(int i = 0; i < Resolution; i++)
										ArrayBlock[i] = iBytes[YIterator * ImageWidth + XIterator + i];

									//Call the function to pecisely locate the edge point
									if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
									{
										Point.X = XIterator + EdgePoint;
										Point.Y = YIterator;
										if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
										{	
											if (Point.X > bd_factor[0])
												Point.direction = 1;
											else 
												Point.direction = -1;
										}
										else
										{
											if (Point.X > bd_factor[0])
												Point.direction = -1;
											else 
												Point.direction = 1;
										}
										PointsList->push_back(Point); //Put the point into the list
										break;
									}
									//The locating of the edge failed..
									else
										FalseEdge = false; //Reset the flag
								}
								else
								{
									FalseEdge = false; //Reset the flag
								}
							}
						}					
					}				
				break;
			case Leftwards:
					//Move from one row to the next downwards
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
					{
						//Move forward along the row leftwards
						for(XIterator = RightBoundry - 1; XIterator >= LeftBoundry + 1 + Resolution; XIterator -= Incrementer)
						{	
							Diff =iBytes[YIterator * ImageWidth + XIterator] - iBytes[YIterator * ImageWidth + XIterator - Resolution + 1];
							//Compare the 2 pixel values which are "Resolution" no. of pixels apart
							if(abs(Diff) >= Tolerance)
							{
								if ( Diff > 0)
								{
									if (iBytes[YIterator * ImageWidth + XIterator - Resolution + 1] > BlackCutOff) 
										continue;
								}
								else
								{
									if (iBytes[YIterator * ImageWidth + XIterator - Resolution + 1] < WhiteCutoff) 
										continue;
								} 

								//Now we need to check if the difference continues for the next few pixels
								//Only then it is a valid edge
								for(int i = XIterator - Resolution; i >= XIterator - Resolution + 1 - LookAhead && i >= LeftBoundry; i--)
								{
																		//Now if the difference is not within threshold,
									//Flag it as an invalid edge and break off from the loop
									if(abs(iBytes[YIterator * ImageWidth + XIterator] - iBytes[YIterator * ImageWidth + i]) < Tolerance)
									{
										FalseEdge = true;
										break;
									}
								}
							
								//Once out of the loop, check if the edge is validated
								if(!FalseEdge)
								{
									//Copy the pixel values into the array
									for(int i = 0; i < Resolution; i++)
										ArrayBlock[i] = iBytes[YIterator * ImageWidth + XIterator - i];

									//Call the function to pecisely locate the edge point
									if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
									{
										Point.X = XIterator - EdgePoint;
										Point.Y = YIterator;
										
										if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
										{	
											if (Point.X > bd_factor[0])
												Point.direction = 1;
											else 
												Point.direction = -1;
										}
										else
										{
											if (Point.X > bd_factor[0])
												Point.direction = -1;
											else 
												Point.direction = 1;
										}

										PointsList->push_back(Point); //Put the point into the list
										break;
									}
									//The locating of the edge failed..
									else
										FalseEdge = false; //Reset the flag
								}
								else
								{
									FalseEdge = false; //Reset the flag
								}
							}
						}					
					}			
				break;
			case Downwards:
					//Move from one column to the next rightwards	
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						//Move forward along the column downwards
						for(YIterator = TopBoundry + 1; YIterator <= BottomBoundry - 1 - Resolution; YIterator += Incrementer)
						{
							Diff = iBytes[YIterator * ImageWidth + XIterator] - iBytes[(YIterator + Resolution - 1) * ImageWidth + XIterator];
							//Compare the 2 pixel values which are "Resolution" no. of pixels apart
							if(abs(Diff) >= Tolerance)
							{
								if ( Diff > 0)
								{
									if (iBytes[(YIterator + Resolution - 1) * ImageWidth + XIterator] > BlackCutOff) 
										continue;
								}
								else
								{
									if (iBytes[(YIterator + Resolution - 1) * ImageWidth + XIterator] < WhiteCutoff) 
										continue;
								} 
								//Now we need to check if the difference continues for the next few pixels
								//Only then it is a valid edge
								for(int i = YIterator + Resolution; i <= YIterator + Resolution - 1 + LookAhead && i <= BottomBoundry; i++)
								{
									//Now if the difference is not within threshold,
									//Flag it as an invalid edge and break off from the loop
									if(abs(iBytes[YIterator * ImageWidth + XIterator] - iBytes[i * ImageWidth + XIterator]) < Tolerance)
									{
										FalseEdge = true;
										break;
									}
								}
							
								//Once out of the loop, check if the edge is validated
								if(!FalseEdge)
								{
									//Copy the pixel values into the array
									for(int i = 0; i < Resolution; i++)
										ArrayBlock[i] = iBytes[(YIterator + i) * ImageWidth + XIterator];

									//Call the function to pecisely locate the edge point
									if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
									{
										Point.X = XIterator;
										Point.Y = YIterator + EdgePoint;
										if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
										{	
											if (Point.Y > bd_factor[1])
												Point.direction = 1;
											else 
												Point.direction = -1;
										}
										else
										{
											if (Point.Y > bd_factor[1])
												Point.direction = -1;
											else 
												Point.direction = 1;
										}

										PointsList->push_back(Point); //Put the point into the list
										break;
									}
									//The locating of the edge failed..
									else
										FalseEdge = false; //Reset the flag
								}
								else
								{
									FalseEdge = false; //Reset the flag
								}
							}
						}					
					}				
				break;
			case Upwards:
					//Move from one column to the next rightwards
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
					{
						//Move forward along the column upwards
						for(YIterator = BottomBoundry - 1; YIterator >= TopBoundry + 1 + Resolution; YIterator -= Incrementer)
						{
							Diff = iBytes[YIterator * ImageWidth + XIterator] - iBytes[(YIterator - Resolution + 1) * ImageWidth + XIterator];
							//Compare the 2 pixel values which are "Resolution" no. of pixels apart
							if(abs(Diff) >= Tolerance)
							{
								if ( Diff > 0)
								{
									if (iBytes[(YIterator - Resolution + 1) * ImageWidth + XIterator] > BlackCutOff) 
										continue;
								}
								else
								{
									if (iBytes[(YIterator + Resolution - 1) * ImageWidth + XIterator] < WhiteCutoff) 
										continue;
								}
								//Now we need to check if the difference continues for the next few pixels
								//Only then it is a valid edge
								for(int i = YIterator - Resolution; i >= YIterator - Resolution + 1 - LookAhead && i >= TopBoundry; i--)
								{
									//Now if the difference is not within threshold,
									//Flag it as an invalid edge and break off from the loop
									if(abs(iBytes[YIterator * ImageWidth + XIterator] - iBytes[i * ImageWidth + XIterator]) < Tolerance)
									{
										FalseEdge = true;
										break;
									}
								}
							
								//Once out of the loop, check if the edge is validated
								if(!FalseEdge)
								{
									//Copy the pixel values into the array
									for(int i = 0; i < Resolution; i++)
										ArrayBlock[i] = iBytes[(YIterator - i) * ImageWidth + XIterator];

									//Call the function to pecisely locate the edge point
									if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
									{
										Point.X = XIterator;
										Point.Y = YIterator - EdgePoint;
										if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
										{	
											if (Point.Y > bd_factor[1])
												Point.direction = 1;
											else 
												Point.direction = -1;
										}
										else
										{
											if (Point.Y > bd_factor[1])
												Point.direction = -1;
											else 
												Point.direction = 1;
										}

										PointsList->push_back(Point); //Put the point into the list
										break;
									}
									//The locating of the edge failed..
									else
										FalseEdge = false; //Reset the flag
								}
								else
								{
									FalseEdge = false; //Reset the flag
								}
							}
						}					
					}			
				break;
			}
			if(ArrayBlock != NULL)
			{
				free(ArrayBlock);
				ArrayBlock = NULL;
			}
			//FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0041", __FILE__, __FUNCSIG__); return; }
	}

	//Function to perform the scan in a rectangular area for th edge
	void EdgeDetectionSingleChannel::DetectProfileEdgeLowerResolutionAngularRect0(int *Box,  bool OppositeScan, int Resolution, std::list<struct Pt> *PointsList)
	{
		//SetAndRaiseErrorMessage("EDCSC0002", "ResetImageSize", __FUNCSIG__);
		try
		{
			Pt Point1, Point2, OriginPoint, Point;
			double EdgePoint;
			int Width = Box[4], Height, XIterator, YIterator, Xoriginal, Yoriginal, XoriginalTmp, YoriginalTmp;
			double Xoriginal_d, Yoriginal_d, XoriginalTmp_d, YoriginalTmp_d;
			double CosTh, SinTh;
			int LookAhead = 5;
			int PixelArrayIndex;
			bool FalseEdge = false;
			double *ArrayBlock = (double*)malloc(sizeof(double) * Resolution);
			double FirstP[2], SecondP[2];
			int BlackCutOff = 80;
			int WhiteCutoff = 175;
			int Diff = 0;

			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if(Box[1] > Box[3])
			{	
				Point1.X = Box[0];
				Point1.Y = Box[1];
				Point2.X = Box[2];
				Point2.Y = Box[3];
			}
			else
			{
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}
			Height = sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2)); //height of the box is the dist between the 2 points

			CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));
			SinTh = (Point1.Y - Point2.Y) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));

			//The left top corner of the rect is taken as the origin for the transformation
			OriginPoint.X = Point2.X - Width / 2 * SinTh;
			OriginPoint.Y = Point2.Y - Width / 2 * CosTh;

			//To account for the direction change in the range 135 deg to 180 deg
			if ((Point1.X - Point2.X) > (Point1.Y - Point2.Y))
				OppositeScan = !OppositeScan;
			
			if (!OppositeScan)
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = 0; XIterator < Width; XIterator++)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						GetOriginalCoordinates(XIterator + Resolution - 1, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
						XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
						YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
						if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
							continue;
						//Compare the 2 pixel values which are "Resolution" no. of pixels apart
						Diff = iBytes[PixelArrayIndex] - iBytes[XoriginalTmp + YoriginalTmp * ImageWidth];
						if(abs(Diff) < Tolerance)
							continue;
						if ( Diff > 0)
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackCutOff) 
								continue;
						}
						else
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] < WhiteCutoff) 
								continue;
						}
						//Now we need to check if the difference continues for the next few pixels
						//Only then it is a valid edge
						for(int i = XIterator + Resolution; i <= XIterator + Resolution - 1 + LookAhead && i <= Width; i++)
						{
							GetOriginalCoordinates(i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							//Now if the difference is not within threshold,
							//Flag it as an invalid edge and break off from the loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[YoriginalTmp * ImageWidth + XoriginalTmp]) < Tolerance)
							{		
								FalseEdge = true;
								break;
							}
						}

						//Once out of the loop, check if the edge is validated
						if(!FalseEdge)
						{
							//Copy the pixel values into the array
							for(int i = 0; i < Resolution; i++)
							{
								GetOriginalCoordinates(XIterator + i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
								XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
								YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
								ArrayBlock[i] = iBytes[YoriginalTmp * ImageWidth + XoriginalTmp];
								//Get position of first pixel with respect to beam centre
								if (i == 0) GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &FirstP[0]);
							}
							//Get position of last pixel with respect to beam centre
							GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &SecondP[0]);

							//Call the function to precisely locate the edge point
							if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
							{
								//Ah!! we have an edge
								GetOriginalCoordinates(XIterator + EdgePoint * SinTh, YIterator + EdgePoint * CosTh, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
								Point.X = Xoriginal_d;
								Point.Y = Yoriginal_d;
								if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
								{	
									if (Point.X > bd_factor[0])
										Point.direction = 1;
									else 
										Point.direction = -1;
								}
								else
								{
									if (Point.X > bd_factor[0])
										Point.direction = -1;
									else 
										Point.direction = 1;
								}
								PointsList->push_back(Point); //Put the point into the list
								break;
							}
							//The locating of the edge failed..
							else
								FalseEdge = false; //Reset the flag
						}
						else
						{
							FalseEdge = false; //Reset the flag
						}
					}
				}				
			}
			else
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = Width - 1; XIterator >= 0; XIterator--)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						GetOriginalCoordinates(XIterator - Resolution + 1, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
						XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
						YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
						if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
							continue;
						//Compare the 2 pixel values which are "Resolution" no. of pixels apart
						Diff = iBytes[PixelArrayIndex] - iBytes[XoriginalTmp + YoriginalTmp * ImageWidth];
						if(abs(Diff) < Tolerance)
							continue;
						if ( Diff > 0)
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackCutOff) 
								continue;
						}
						else
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] < WhiteCutoff) 
								continue;
						}

						//Now we need to check if the difference continues for the next few pixels
						//Only then it is a valid edge
						for(int i = XIterator - Resolution; i >= XIterator - Resolution + 1 - LookAhead && i >= 0; i--)
						{
							GetOriginalCoordinates(i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							//Now if the difference is not within threshold,
							//Flag it as an invalid edge and break off from the loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[YoriginalTmp * ImageWidth + XoriginalTmp]) < Tolerance)
							{	
									FalseEdge = true;
									break;
							}
						}

						//Once out of the loop, check if the edge is validated
						if(!FalseEdge)
						{
							//Copy the pixel values into the array
							for(int i = 0; i < Resolution; i++)
							{
								GetOriginalCoordinates(XIterator - i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
								XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
								YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
								ArrayBlock[i] = iBytes[YoriginalTmp * ImageWidth + XoriginalTmp];
							}

							//Call the function to pecisely locate the edge point
							if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
							{
								//Ah!! we have an edge
								GetOriginalCoordinates(XIterator - EdgePoint * SinTh, YIterator + EdgePoint * CosTh, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
								Point.X = Xoriginal_d;
								Point.Y = Yoriginal_d;
								if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
								{	
									if (Point.X > bd_factor[0])
										Point.direction = 1;
									else 
										Point.direction = -1;
								}
								else
								{
									if (Point.X > bd_factor[0])
										Point.direction = -1;
									else 
										Point.direction = 1;
								}
								PointsList->push_back(Point); //Put the point into the list
								break;
							}
							//The locating of the edge failed..
							else
								FalseEdge = false; //Reset the flag
						}
						else
						{
							FalseEdge = false; //Reset the flag
						}
					}
				}			
			}
			if(ArrayBlock != NULL)
			{
				free(ArrayBlock);
				ArrayBlock = NULL;
			}
			//FilterOutPtsFromList(PointsList);
		}
		catch(...)
		{
			SetAndRaiseErrorMessage("EDSC0028", __FILE__, __FUNCSIG__);
		}
	}

		//Function to perform the scan in a rectangular area for th edge
	void EdgeDetectionSingleChannel::DetectProfileEdgeLowerResolutionAngularRect(int *Box,  bool OppositeScan, int Resolution, std::list<struct Pt> *PointsList) 
	{
		//SetAndRaiseErrorMessage("EDSC0028", "DetectProfileEdgeLowerResolutionAngularRect", __FUNCSIG__);
		try
		{
			Pt Point1, Point2, OriginPoint, Point;
			double EdgePoint;
			int Width = Box[4], Height, XIterator, YIterator, Xoriginal, Yoriginal, XoriginalTmp, YoriginalTmp;
			double Xoriginal_d, Yoriginal_d, XoriginalTmp_d, YoriginalTmp_d;
			double CosTh, SinTh;
			int LookAhead = 5;
			int PixelArrayIndex;
			bool FalseEdge = false;
			double *ArrayBlock = (double*)malloc(sizeof(double) * Resolution);
			double FirstP[2], SecondP[2];
			int BlackCutOff = 80;
			int WhiteCutoff = 175;
			int Diff = 0;

			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if(Box[1] > Box[3])
			{	
				Point1.X = Box[0];
				Point1.Y = Box[1];
				Point2.X = Box[2];
				Point2.Y = Box[3];
			}
			else
			{
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}
			Height = sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2)); //height of the box is the dist between the 2 points

			double Theta = 0;
			if (Point2.X == Point1.X)
				Theta = M_PI / 2;
			else
				Theta = atan((Point1.Y - Point2.Y) / (Point2.X - Point1.X));

			Theta += M_PI / 2;
			if (Theta < 0) Theta += M_PI;
			
			CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));
			SinTh = (Point1.Y - Point2.Y) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));

			//The left top corner of the rect is taken as the origin for the transformation
			OriginPoint.X = Point2.X - Width / 2 * SinTh;
			OriginPoint.Y = Point2.Y - Width / 2 * CosTh;

			//To account for the direction change in the range 135 deg to 180 deg
			//if ((Point1.X - Point2.X) > (Point1.Y - Point2.Y))
			//	OppositeScan = !OppositeScan;
			
			if (!OppositeScan)
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = 0; XIterator < Width; XIterator++)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						GetOriginalCoordinates(XIterator + Resolution - 1, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
						XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
						YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
						if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
							continue;
						//Compare the 2 pixel values which are "Resolution" no. of pixels apart
						Diff = iBytes[PixelArrayIndex] - iBytes[XoriginalTmp + YoriginalTmp * ImageWidth];
						if(abs(Diff) < Tolerance)
							continue;
						if ( Diff > 0)
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackCutOff) 
								continue;
						}
						else
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] < WhiteCutoff) 
								continue;
						}
						//Now we need to check if the difference continues for the next few pixels
						//Only then it is a valid edge
						for(int i = XIterator + Resolution; i <= XIterator + Resolution - 1 + LookAhead && i <= Width; i++)
						{
							GetOriginalCoordinates(i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							//Now if the difference is not within threshold,
							//Flag it as an invalid edge and break off from the loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[YoriginalTmp * ImageWidth + XoriginalTmp]) < Tolerance)
							{		
								FalseEdge = true;
								break;
							}
						}

						//Once out of the loop, check if the edge is validated
						if(!FalseEdge)
						{
							//Copy the pixel values into the array
							for(int i = 0; i < Resolution; i++)
							{
								GetOriginalCoordinates(XIterator + i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
								XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
								YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
								ArrayBlock[i] = iBytes[YoriginalTmp * ImageWidth + XoriginalTmp];
								//Get position of first pixel with respect to beam centre
								if (i == 0) GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &FirstP[0]);
							}
							//Get position of last pixel with respect to beam centre
							GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &SecondP[0]);

							//Call the function to precisely locate the edge point
							if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
							{
								//Ah!! we have an edge
								GetOriginalCoordinates(XIterator + EdgePoint, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
								Point.X = Xoriginal_d;
								Point.Y = Yoriginal_d;
								if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
								{	
									if (Point.X > bd_factor[0])
										Point.direction = 1;
									else 
										Point.direction = -1;
								}
								else
								{
									if (Point.X > bd_factor[0])
										Point.direction = -1;
									else 
										Point.direction = 1;
								}
								PointsList->push_back(Point); //Put the point into the list
								break;
							}
							//The locating of the edge failed..
							else
								FalseEdge = false; //Reset the flag
						}
						else
						{
							FalseEdge = false; //Reset the flag
						}
					}
				}				
			}
			else
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = Width - 1; XIterator >= 0; XIterator--)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						GetOriginalCoordinates(XIterator - Resolution + 1, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
						XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
						YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
						if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
							continue;
						//Compare the 2 pixel values which are "Resolution" no. of pixels apart
						Diff = iBytes[PixelArrayIndex] - iBytes[XoriginalTmp + YoriginalTmp * ImageWidth];
						if(abs(Diff) < Tolerance)
							continue;
						if ( Diff > 0)
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackCutOff) 
								continue;
						}
						else
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] < WhiteCutoff) 
								continue;
						}

						//Now we need to check if the difference continues for the next few pixels
						//Only then it is a valid edge
						for(int i = XIterator - Resolution; i >= XIterator - Resolution + 1 - LookAhead && i >= 0; i--)
						{
							GetOriginalCoordinates(i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							//Now if the difference is not within threshold,
							//Flag it as an invalid edge and break off from the loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[YoriginalTmp * ImageWidth + XoriginalTmp]) < Tolerance)
							{	
									FalseEdge = true;
									break;
							}
						}

						//Once out of the loop, check if the edge is validated
						if(!FalseEdge)
						{
							//Copy the pixel values into the array
							for(int i = 0; i < Resolution; i++)
							{
								GetOriginalCoordinates(XIterator - i, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &XoriginalTmp_d, &YoriginalTmp_d);
								XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
								YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
								ArrayBlock[i] = iBytes[YoriginalTmp * ImageWidth + XoriginalTmp];
							}

							//Call the function to pecisely locate the edge point
							if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
							{
								//Ah!! we have an edge
								GetOriginalCoordinates(XIterator - EdgePoint, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
								Point.X = Xoriginal_d;
								Point.Y = Yoriginal_d;
								if (ArrayBlock[0] > ArrayBlock[Resolution - 1])
								{	
									if (Point.X > bd_factor[0])
										Point.direction = 1;
									else 
										Point.direction = -1;
								}
								else
								{
									if (Point.X > bd_factor[0])
										Point.direction = -1;
									else 
										Point.direction = 1;
								}
								PointsList->push_back(Point); //Put the point into the list
								break;
							}
							//The locating of the edge failed..
							else
								FalseEdge = false; //Reset the flag
						}
						else
						{
							FalseEdge = false; //Reset the flag
						}
					}
				}			
			}
			if(ArrayBlock != NULL)
			{
				free(ArrayBlock);
				ArrayBlock = NULL;
			}
			//FilterOutPtsFromList(PointsList);
		}
		catch(...)
		{
			SetAndRaiseErrorMessage("EDSC0028", __FILE__, __FUNCSIG__);
		}
	}


	//Function to perform the scan in a rectangular area for th edge
	void EdgeDetectionSingleChannel::DetectProfileEdgeLowerResolutionArcRect(double *Box,  bool OppositeScan, int Resolution, std::list<struct Pt> *PointsList)
	{
		//SetAndRaiseErrorMessage("EDSC0028", "DetectProfileEdgeLowerResolutionArcRect", __FUNCSIG__);
		try
		{
			Pt OriginPoint, Point;
			double EdgePoint;
			int RIterator;
			double ThetaIterator;
			int LookAhead = 5;
			double R1, R2, Theta1, Theta2, IncrementalAngle;
			int Xoriginal, Yoriginal, XoriginalTmp, YoriginalTmp;
			double Xoriginal_d, Yoriginal_d, XoriginalTmp_d, YoriginalTmp_d;
			int PixelArrayIndex;
			bool FalseEdge = false;
			double *ArrayBlock = (double*)malloc(sizeof(double) * Resolution);
			
			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];
			IncrementalAngle = 2 / (R1 + R2); //incrementing the angle so that linear increment is one pixel: ang = 1 / ((R1 + R2) / 2)
			double FirstP[2], SecondP[2];
			
			int BlackCutOff = 80;
			int WhiteCutoff = 175;
			int Diff = 0;

			if (!OppositeScan)
			{
				for(ThetaIterator = Theta1; ThetaIterator < Theta2; ThetaIterator += IncrementalAngle)
				{
					for(RIterator = R1; RIterator < R2; RIterator++)
					{
						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator + Resolution - 1, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
						XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
						YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
						if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
							continue;
						//Compare the 2 pixel values which are "Resolution" no. of pixels apart
						Diff = iBytes[PixelArrayIndex] - iBytes[XoriginalTmp + YoriginalTmp * ImageWidth];
						if(abs(Diff) < Tolerance)
							continue;
						if ( Diff > 0)
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackCutOff) 
								continue;
						}
						else
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] < WhiteCutoff) 
								continue;
						}


						//Now we need to check if the difference continues for the next few pixels
						//Only then it is a valid edge
						for(int i = RIterator + Resolution; i <= RIterator + Resolution - 1 + LookAhead && i <= R2; i++)
						{
							GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, i, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							//Now if the difference is not within threshold,
							//Flag it as an invalid edge and break off from the loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[YoriginalTmp * ImageWidth + XoriginalTmp]) < Tolerance)
							{
								FalseEdge = true;
								break;
							}
						}
						
						//Once out of the loop, check if the edge is validated
						if(!FalseEdge)
						{	
							
							
							//Copy the pixel values into the array
							for(int i = 0; i < Resolution; i++)
							{
								GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator + i, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
								XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
								YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
								ArrayBlock[i] = iBytes[YoriginalTmp * ImageWidth + XoriginalTmp];
								//Get position of first pixel with respect to beam centre
								if (i == 0) GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &FirstP[0]);
							}
							//Get position of last pixel with respect to beam centre
							GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &SecondP[0]);

							//Call the function to pecisely locate the edge point
							if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
							{
								//Ah!! we have an edge
								GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator + EdgePoint, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
								Point.X = XoriginalTmp_d;
								Point.Y = YoriginalTmp_d;
								if (FirstP[0] > SecondP[0] && ArrayBlock[0] > ArrayBlock[Resolution - 1])
									Point.direction = 1;
								else
									Point.direction = -1;
								PointsList->push_back(Point); //Put the point into the list
								break;
							}
							//The locating of the edge failed..
							else
								FalseEdge = false; //Reset the flag
						}
						else
						{
							FalseEdge = false; //Reset the flag
						}
					}
				}				
			}
			else
			{
				for(ThetaIterator = Theta1; ThetaIterator < Theta2; ThetaIterator += IncrementalAngle)
				{
					for(RIterator = R2; RIterator > R1; RIterator--)
					{
						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth || Xoriginal < 0 || Yoriginal >= ImageHeight || Yoriginal < 0)
							continue;
						PixelArrayIndex = Xoriginal + Yoriginal * ImageWidth;

						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator - Resolution + 1, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
						XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
						YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
						if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
							continue;
						//Compare the 2 pixel values which are "Resolution" no. of pixels apart
						Diff = iBytes[PixelArrayIndex] - iBytes[XoriginalTmp + YoriginalTmp * ImageWidth];
						if(abs(Diff) < Tolerance)
							continue;
						if ( Diff > 0)
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] > BlackCutOff) 
								continue;
						}
						else
						{
							if (iBytes[XoriginalTmp + YoriginalTmp * ImageWidth] < WhiteCutoff) 
								continue;
						}
						//Now we need to check if the difference continues for the next few pixels
						//Only then it is a valid edge
						for(int i = RIterator - Resolution; i >= RIterator - Resolution + 1 - LookAhead && i >= R1; i--)
						{
							GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, i, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
							XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
							YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
							if(XoriginalTmp >= ImageWidth || XoriginalTmp < 0 || YoriginalTmp >= ImageHeight || YoriginalTmp < 0)
								break;
							//Now if the difference is not within threshold,
							//Flag it as an invalid edge and break off from the loop
							if(abs(iBytes[PixelArrayIndex] - iBytes[YoriginalTmp * ImageWidth + XoriginalTmp]) < Tolerance)
							{
								FalseEdge = true;
								break;
							}
						}
						
						//Once out of the loop, check if the edge is validated
						if(!FalseEdge)
						{
							//Copy the pixel values into the array
							for(int i = 0; i < Resolution; i++)
							{
								GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator - i, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
								XoriginalTmp = (int)(XoriginalTmp_d + 0.5);
								YoriginalTmp = (int)(YoriginalTmp_d + 0.5);
								ArrayBlock[i] = iBytes[YoriginalTmp * ImageWidth + XoriginalTmp];
								//Get position of first pixel with respect to beam centre
								if (i == 0) GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &FirstP[0]);
							}
							//Get position of last pixel with respect to beam centre
							GetPolarCoordinates(XoriginalTmp, YoriginalTmp, &SecondP[0]);

							//Call the function to pecisely locate the edge point
							if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
							{
								//Ah!! we have an edge
								GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator - EdgePoint, ThetaIterator, &XoriginalTmp_d, &YoriginalTmp_d);
								Point.X = XoriginalTmp_d;
								Point.Y = YoriginalTmp_d;
								
								if (FirstP[0] > SecondP[0] && ArrayBlock[0] > ArrayBlock[Resolution - 1])
									Point.direction = -1;
								else
									Point.direction = 1;

								PointsList->push_back(Point); //Put the point into the list
								break;
							}
							//The locating of the edge failed..
							else
								FalseEdge = false; //Reset the flag
						}
						else
						{
							FalseEdge = false; //Reset the flag
						}
					}
				}			
			}
			if(ArrayBlock != NULL)
			{
				free(ArrayBlock);
				ArrayBlock = NULL;
			}
			//FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0028", __FILE__, __FUNCSIG__); }
	}

	//Function to pin point the edge within a block, for lower resolution of focus
	bool EdgeDetectionSingleChannel::PinPointEdgeWithinBlock0(double *ArrayBlock, int Size, double *Answer)
	{
		try
		{
			bool Reversed = false;
			int Limit = Size / 2;
			double Sum = 0;

			//First check if the array needs to be reversed, cos the array must be in the order white to black
			if(ArrayBlock[0] < ArrayBlock[Size - 1])
			{
				Reversed = true;
				double temp;
				for(int i = 0; i < Limit; i++)
				{
					//Swapping function
					temp = ArrayBlock[i];
					ArrayBlock[i] = ArrayBlock[Size - 1 - i];
					ArrayBlock[Size - 1 - i] = temp;
				}
			}

			//Next we need to do a bit of filtering by making almost white values to complete white..similarly with black.
			for(int i = 0; i < Size; i++)
			{
				if(ArrayBlock[i] > 240) ArrayBlock[i] = 255;
				else if(ArrayBlock[i] < 30) ArrayBlock[i] = 0;
			}

			//Next check if array is in decreasing order...if not, then it means there is some noise...return false
			for(int i = 0; i < Size - 1; i++)
				if(ArrayBlock[i] < ArrayBlock[i + 1]) return false;

			int st = -1; 
			//Now calculate the edge distance
			for(int i = 0; i < Size; i++)
			{
				if (ArrayBlock[i] > 240) continue;
				if (st == -1) st = i;
				if (ArrayBlock[i] < 70) break;
				Sum += ArrayBlock[i];
			}
			
			if(Reversed) *Answer = Size - (st + Sum / 255); // - 0.5; //==> Size - 1 -(Sum / 255 - 0.5)
			else *Answer = st + Sum / 255; // - 0.5; //==> Sum / 255 - 0.5
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0042", __FILE__, __FUNCSIG__); return 0; }
	}

		//Function to pin point the edge within a block, for lower resolution of focus
	bool EdgeDetectionSingleChannel::PinPointEdgeWithinBlock(double *ArrayBlock, int Size, double *Answer)
	{
		try
		{
			bool Reversed = false;
			int Limit = Size / 2;
			double Sum = 0;

			////First check if the array needs to be reversed, cos the array must be in the order white to black
			//if(ArrayBlock[0] < ArrayBlock[Size - 1])
			//{
			//	Reversed = true;
			//	double temp;
			//	for(int i = 0; i < Limit; i++)
			//	{
			//		//Swapping function
			//		temp = ArrayBlock[i];
			//		ArrayBlock[i] = ArrayBlock[Size - 1 - i];
			//		ArrayBlock[Size - 1 - i] = temp;
			//	}
			//}

			////Next we need to do a bit of filtering by making almost white values to complete white..similarly with black.
			//for(int i = 0; i < Size; i++)
			//{
			//	if(ArrayBlock[i] > 240) ArrayBlock[i] = 255;
			//	else if(ArrayBlock[i] < 30) ArrayBlock[i] = 0;
			//}

			//Next check if array is in decreasing order...if not, then it means there is some noise...return false
			//for(int i = 0; i < Size - 1; i++)
			//	if(ArrayBlock[i] < ArrayBlock[i + 1]) return false;

			//Now find out at what position value 128 occurs. We collect each of the pixels that are between 128 +/- 60 in intensity values, 
			//draw a straight line, and then derive where 128 comes
			int intensityRange = 100;
			int MidVal = 128;
			double linePts[20] = {0};
			int Ct = 0;
			for(int i = 0; i < Size; i++)
			{
				if (ArrayBlock[i] > MidVal + intensityRange || ArrayBlock[i] < MidVal - intensityRange) continue;
				linePts[Ct] = i; linePts[Ct + 1] = ArrayBlock[i];
				Ct += 2;
			}
			double LnParam[2] = {0};

			if (Ct < 4) return false;

			BFit->Line_BestFit_2D(linePts, Ct / 2, LnParam, false);
			
			double EdgeVal = (MidVal - LnParam[1]) / tan(LnParam[0]);
			
			if (EdgeVal > Size || EdgeVal < 0) return false;
			
			if(Reversed) *Answer = Size - EdgeVal; //(st + Sum / 255); // - 0.5; //==> Size - 1 -(Sum / 255 - 0.5)
			else *Answer = EdgeVal; // st + Sum / 255; // - 0.5; //==> Sum / 255 - 0.5
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0042", __FILE__, __FUNCSIG__); return 0; }
	}



	//---------------------------------------------------------------------------------------
	//The functions for Reference dot detection
	//---------------------------------------------------------------------------------------

	//Function to find the reference dot
	int EdgeDetectionSingleChannel::Analyze_RotatingDot(bool BinariseImage)
	{	
		try
		{
			int EPCt = 0, thValue = 150;
			
			if (BinariseImage)
			{	
				double avg = 0;
				int imageSize = ImageWidth * ImageHeight;
				for (int i = 0; i < imageSize; i++) avg += iBytes[i];
				avg /= imageSize;
				thValue = 0.6 * avg;
			}
			/*double *SourceImage = (double*)malloc(ImageHeight * ImageWidth * sizeof(double));
			memcpy((void*)&iBytes, (void*)SourceImage, ImageHeight * ImageWidth * sizeof(double));*/
			RefDotThreshold(iBytes, thValue);
			EPCt = RefDotEdges(iBytes);
			//free(SourceImage);
			return EPCt;	
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0043", __FILE__, __FUNCSIG__); return 0; }
	}
	
	//This function sets the image in black and white format according to the threshold value passed
	void EdgeDetectionSingleChannel::RefDotThreshold(double *SourceImage, int threshvalue)
	{
		try
		{
			//Declaring the variables.
			int x, y; 
			int FastIndex;
			
			//comparing the each color of each pixel to Threshold value and make the bitmap as binary bitmap.
			for(y = 0; y < ImageHeight; y++)
			{
				for(x = 0;x < ImageWidth; x++)
				{
					//Assigning Starting Address to FastIndex.
					FastIndex = x + y * ImageWidth;
					//Getting the First color of the each pixel.
					double Color = SourceImage[FastIndex];
					double tColor;
					(Color >= threshvalue)? tColor = 255 : tColor = 0;
					SourceImage[FastIndex] = tColor;
				}
			}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0044", __FILE__, __FUNCSIG__); }
	}
	
	//Function to detect the reference dot edge
	int EdgeDetectionSingleChannel::RefDotEdges(double *SourceImage)
	{
		try
		{
			int x, y, inDex;
			int eX, eY;
			int imageSize = ImageWidth * ImageHeight;
			int *locarray = (int*)malloc(imageSize * sizeof(int));
			
			//Declare the variable.
			int locIndex = 1;
			for( y = 2; y < ImageHeight - 2; y++)
				for( x = 2; x < ImageWidth - 2; x++)
				 {
					 locIndex = 0;
					 //Find the index of the array (i.e.) height * total pixel in one row + width * 3 
					 inDex = y * ImageWidth + x;
					 //Check the condition that the pixel is having the edge color or not.
					 //horizontal comparison
					 eX = *(SourceImage + inDex);
					 eX -= *(SourceImage + inDex + 1);
					 //vertical comparison
					 eY = *(SourceImage + inDex);
					 eY -= *(SourceImage + inDex + ImageWidth);
					 //if(*(SourceImage + inDex + 1) != 175 && (abs(eX) == 255 ||abs(eY) == 255))
					 if(*(SourceImage + inDex) != 175 && (abs(eX) == 255 ||abs(eY) == 255))
					 {
						 SourceImage[inDex] = 175;
						 //Assign the first edge pixel index value to the array.
						 locarray[locIndex] = inDex;
						 //Increment the index.
						 locIndex++;
						 //Declare the array to calculate the near by pixel index of 3x3 matrix.
						 int row[3]= {-1, 0, 1},col[3]= {-ImageWidth, 0, ImageWidth};
						 int y2=0;
						 //Loop runs until getting all the edge pixel of single object.
						 while( y2!=locIndex)
						 {
							 //Loop to find the adjacent pixels.
							 for(int x1 = 0; x1 < 3; x1++)
								 for(int yy = 0; yy < 3; yy++)
								 {
									 int fast1=(locarray[y2] + row[yy] + col[x1]);
									 //Condition to check that the array index value is out of bounds.
									 if(fast1 >= imageSize - 1)break;
									 //Condition to check that the array index value is out of bounds.
									 if(fast1 <= 0) break;
									 //horizontal comparison
									 eX = *(SourceImage + fast1);
									 eX -= *(SourceImage + fast1 + 1);
									 //vertical comparison
									 eY = *(SourceImage + fast1);
									 eY -= *(SourceImage + fast1 + ImageWidth);
									 //Condition to check the color of the pixel.
									 if(*(SourceImage + fast1)!= 175 && (abs(eX) == 255 ||abs(eY) == 255))
									 {
										 //changing the color.
										 SourceImage[fast1] = 175;
										 //Storing the index value.
										 locarray[locIndex] = fast1;
										 locIndex = locIndex + 1;
									 }
								 }
								 //Increment the index.
								 y2 = y2 + 1;
						 }

						 //Checking he condition of the pixel count
						 if(locIndex > RefDotDiameter * this->CamSizeRatio * 0.5) // && locIndex < 2 * RefDotDiameter * this->CamSizeRatio)
						 {
							 inDex = 0;
							 if (locIndex >= 10000) locIndex = 10000;
							 for(int x = 1; x < locIndex; x++)
							 {
								 *(DetectedPointsList + inDex) = int(locarray[x] % ImageWidth);
								 //*(DetectedPointsList + inDex + 1) = ImageHeight - 1 - int(locarray[x] / ImageWidth);
								 *(DetectedPointsList + inDex + 1) = int(locarray[x] / ImageWidth);
								 inDex+=2;
							 }
							 free(locarray);
							 return inDex / 2;
						 }
					 }
				}
			free(locarray);
			return 0;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0045", __FILE__, __FUNCSIG__); return 0; }
	}

	//---------------------------------------------------------------------------------------
	//The functions for detecting all the profile edges within the specified rectangle
	//---------------------------------------------------------------------------------------

	//Public function to detect profile edges
	int EdgeDetectionSingleChannel::DetectAllProfileEdges(int *FrameGrabBox, int jump)
	{
		try
		{
			std::list<struct Pt> PointsList;
			int Box[4] = {FrameGrabBox[0], FrameGrabBox[1] ,FrameGrabBox[2] ,FrameGrabBox[3]};

			DetectAllProfileEdgesHelper(Box, &PointsList);
			if (PointsList.size() == 0) return 0;
			std::list<struct Pt> FilteredPointsList;
			Pt prevPnt;
			bool firsttime = true;
			for each(Pt Pnt in PointsList)
			{
				if (firsttime)
				{
					firsttime = false;
					FilteredPointsList.push_back(Pnt);
					prevPnt = Pnt;
				}
				else
				{
					if (pow((prevPnt.X - Pnt.X),2) + pow((prevPnt.Y - Pnt.Y),2) > jump * jump)
					{
						FilteredPointsList.push_back(Pnt);
						prevPnt = Pnt;
					}
				}
			}
			PointsList.clear();
			for each(Pt Pnt in FilteredPointsList)
				PointsList.push_back(Pnt);
			FilteredPointsList.clear();

			ArrangeAllPoints2(&PointsList);
			
			//Now put all the points in the linear array after correction if required...
			return AddEdgePtsToCollection(PointsList);			
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0046", __FILE__, __FUNCSIG__); return 0; }
	}

	//Helper function to detect all profile edges within a given box
	void EdgeDetectionSingleChannel::DetectAllProfileEdgesHelper(int *Box, std::list<struct Pt> *PointsList)
	{
		try
		{
			double pixels[40] = {0};
			int XIterator, YIterator,  LookAheadIndex, i;
			double var;
			struct Pt Point;
			int LeftBoundry = Box[0] * this->CamSizeRatio + BoundryBuffer;
			int RightBoundry = (Box[0] + Box[2]) * this->CamSizeRatio - BoundryBuffer - 1;
			int TopBoundry = Box[1] * this->CamSizeRatio + BoundryBuffer;
			int BottomBoundry = (Box[1] + Box[3]) * this->CamSizeRatio - BoundryBuffer - 1;
			int PixelArrayIndex;

			for(int itr = 0; itr < 2; itr++)
				switch(itr)
				{
				case 0:
						//Move from one row to the next downwards
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
						{
							//Move forward along the row rightwards
							//for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += PixelSkipValue)
							for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += 2)
							{
								PixelArrayIndex = XIterator + YIterator * ImageWidth;

								//the following if statement searches for a definite edge. If not found, it skips to the next loop
								if(abs(iBytes[PixelArrayIndex] - iBytes[(PixelArrayIndex + 3)]) < Tolerance)
									continue;

								//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
								//for this purpose, first it must be determined till where this checking must be carried out
								if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
									//the checking can continue for 'look ahead' number of pixels
									LookAheadIndex = LookAheadLimit;
								else
									//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
									LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
								//This is where the actual checking happens the range for which has been determined in the previous if statement
								for(i = 4; i <=  LookAheadIndex; i++)
									if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
									{
										//its not a legitimate edge as there is no continuity of transition
										//hence jump to the point where the check failed and continue searching for edge from this point forward
										XIterator = XIterator + i - PixelSkipValue;
										goto last1;
									}

								//copy the 4 pixels where edge has been detected into 'pixels' array
								for(int j = 0; j <= EdgeFinderPixelRange; j++)
									pixels[j] = iBytes[PixelArrayIndex + j];
						
								//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
								
								if(var == -1)
									// :( No edge detected
									goto last1;
						
								//Ah!! we have an edge
								Point.X = XIterator + var;
								Point.Y = YIterator;
								if (CheckPointWithinStraightRect(Point.X, Point.Y, Box) == 0)
									//Put this into the pointer and return true
									PointsList->push_back(Point); //Put the point into the list
								
last1:							i = 1; //this is just a dummy statement put here bcos before a "}" we need a ";", otherwise it gives an error
							}			
						}				
					break;
				case 1:
						//Move from one column to the next rightwards	
						for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
						{
							//Move forward along the column downwards
							//for(YIterator = TopBoundry + 1; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
							for(YIterator = TopBoundry + 1; YIterator <= BottomBoundry; YIterator += 2)
							{
								PixelArrayIndex = XIterator + YIterator * ImageWidth;

								//the following if statement searches for a definite edge. If not found, it skips to the next loop
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + 3) * ImageWidth)]) < Tolerance)
									continue;

								if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
									LookAheadIndex = LookAheadLimit * ImageWidth;
								else
									LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
						
								for(i = 4 * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
									if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
									{
										YIterator = YIterator + i /ImageWidth - PixelSkipValue;
										goto last3;
									}
								for(int j = 0; j <= EdgeFinderPixelRange; j++)
									pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
						
								//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
								
								if(var == -1) 
									goto last3;
						
								Point.X = XIterator;
								Point.Y = YIterator + var;
								if (CheckPointWithinStraightRect(Point.X, Point.Y, Box) == 0)
									PointsList->push_back(Point); //Put the point into the list
								
last3:							i = 1;	//the dummy statement
							}					
						}				
					break;
				}
			//FilterOutPtsFromList(PointsList);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0047", __FILE__, __FUNCSIG__); return; }
	}

	void EdgeDetectionSingleChannel::ArrangeAllPoints2(std::list<struct Pt>* pAllPointsList, int ptIndex, bool constructlist)
	{
		try
		{
			int TotalCnt = pAllPointsList->size();
			if (TotalCnt == 0)
				return;
			bool flag = true;
			std::list<struct Pt> tmpptslist, currPtslist;
			int index = 0;
			//if point index equal to -1, it means this is first call to the function from outside.
			//iterate through points list and assign a unique point index for all points.
			//this will be used in subsequent calls to function from within this function.
			if (ptIndex == -1)
			{
				for (std::list<struct Pt>::iterator pts_iter = pAllPointsList->begin(); pts_iter != pAllPointsList->end(); pts_iter++)
				{
					pts_iter->pt_index = index;
					index++;
				}
			}	
			//save curent points list
			for (std::list<struct Pt>::iterator pts_iter = pAllPointsList->begin(); pts_iter != pAllPointsList->end(); pts_iter++)
			{
				currPtslist.push_back(*pts_iter);
			}
			Pt firstpt;
			std::list<struct Pt>::iterator pt_iter2 = pAllPointsList->begin();

			//if point index not equal to -1 (this means it is a call to function from within), get first point to start ordering points from.
			if (ptIndex != -1)
			{
				for(std::list<struct Pt>::iterator pts_iter1 = pAllPointsList->begin(); pts_iter1 != pAllPointsList->end(); pts_iter1++)
				{
					if (pts_iter1->pt_index == ptIndex)
					{
						pt_iter2 = pts_iter1;
						break;
					}
				}
			}
			firstpt.X = pt_iter2->X;
			firstpt.Y = pt_iter2->Y;
			firstpt.pt_index = pt_iter2->pt_index;

			tmpptslist.push_back(firstpt);
			pAllPointsList->remove(firstpt);
		
			double dist_threshold = 0;
			Pt tempPoint = firstpt;
			
			for(int i = 0; i < TotalCnt - 1; i++)
			{
				Pt CurrentPt;
				double dist = 0, mindist = 0; flag = true;
				for(std::list<struct Pt>::iterator pts_iter = pAllPointsList->begin(); pts_iter != pAllPointsList->end(); pts_iter++)
				{
					double tempdist = 0;
					tempdist += pow((tempPoint.X - pts_iter->X), 2);  
					tempdist += pow((tempPoint.Y - pts_iter->Y), 2);  
					dist = sqrt(tempdist);
					if(flag)
					{ 
						mindist = dist; flag = false;
						CurrentPt.X = pts_iter->X; CurrentPt.Y = pts_iter->Y; CurrentPt.pt_index = pts_iter->pt_index;
					}
					else
					{
						if(mindist > dist)
						{
							mindist = dist;
							CurrentPt.X = pts_iter->X; CurrentPt.Y = pts_iter->Y; CurrentPt.pt_index = pts_iter->pt_index;
						}
					}
				}
				dist_threshold = Pixel_Neighbour;

				//if minimum distance > threshold, it means we have come to end of current set of edge points.
				//here we do one of two things:
				//1. if constructlist is false (which it is to begin with and in every alternate subsequent call),
				//	we use the last point as start point and call same function again - but this time with constructlist as true.
				//2. if constructlist is true (which it will be in second call and in every alternate subsequent call),
				//	we construct the list of points till the last point to add to the arranged points list, use the next point as start point and call the function with constructlist as false.
				//this is done to ensure that we are always starting from one end of a contiguous list of edge points and proceeding till its end in the arranged points list.
				
				//TODO: to count distinct number of contiguous edges, we need to have a static integer variable (initialized to 0 in first call when ptIndex == -1) 
				//that will be incremented everytime the function is called with constructlist as true.

				if (mindist > dist_threshold)
				{
					int pt_index = tempPoint.pt_index;
					if (constructlist)
					{	
						for (std::list<struct Pt>::iterator pts_iter = tmpptslist.begin(); pts_iter != tmpptslist.end(); pts_iter++)
						{
							currPtslist.remove(*pts_iter);
						}
						pt_index = CurrentPt.pt_index;
					}
					else
					{
						tmpptslist.clear();
					}
					pAllPointsList->clear();
					for (std::list<struct Pt>::iterator pts_iter = currPtslist.begin(); pts_iter != currPtslist.end(); pts_iter++)
					{
						pAllPointsList->push_back(*pts_iter);
					}
					// use extreme point's point index to call the orderpoints function
					ArrangeAllPoints2(pAllPointsList, pt_index, !constructlist);
					for (std::list<struct Pt>::iterator pts_iter = pAllPointsList->begin(); pts_iter != pAllPointsList->end(); pts_iter++)
					{
						tmpptslist.push_back(*pts_iter);
					}
					pAllPointsList->clear();
					for (std::list<struct Pt>::iterator pts_iter = tmpptslist.begin(); pts_iter != tmpptslist.end(); pts_iter++)
					{
						pAllPointsList->push_back(*pts_iter);
					}
					tmpptslist.clear();
					currPtslist.clear();
					return;
				}
				tmpptslist.push_back(CurrentPt);
				pAllPointsList->remove(CurrentPt);
				tempPoint = CurrentPt;
			}
			pAllPointsList->clear();

			for (std::list<struct Pt>::iterator pts_iter = tmpptslist.begin(); pts_iter != tmpptslist.end(); pts_iter++)
			{
				pAllPointsList->push_back(*pts_iter);
			}

			tmpptslist.clear();
			currPtslist.clear();
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0048a", __FILE__, __FUNCSIG__); return; }
	}
	//Function to order the points detected
	void EdgeDetectionSingleChannel::ArrangeAllPoints(int *FrameGrabBox, std::list<struct Pt> AllPointsList, std::list<struct Pt> *ArrangedPointsList)
	{
		try
		{
			map<int, Pt> AllPointsColl;
			map<int, Pt> AllPointsTempCollection;
			Pt LeftMost, TopMost, RightMost, BottomMost; bool flag = true;
			//boundary threshold right, top, bottom...
			int HorizontalFactor = (FrameGrabBox[2]) / 16, VerticalFactor = (FrameGrabBox[3]) / 16;
			double BoundryThreshold[3] = {FrameGrabBox[0] + FrameGrabBox[2] - HorizontalFactor, FrameGrabBox[1] + VerticalFactor, FrameGrabBox[1] + FrameGrabBox[3] - VerticalFactor};
			double minDistThreshold = Pixel_Neighbour;
			int Cnt = 0;

			LeftMost.X = TopMost.X = RightMost.X = BottomMost.X = AllPointsList.begin()->X;
			LeftMost.Y = TopMost.Y = RightMost.Y = BottomMost.Y = AllPointsList.begin()->Y;
			LeftMost.pt_index = TopMost.pt_index = RightMost.pt_index = BottomMost.pt_index = AllPointsList.begin()->pt_index;
			for(std::list<struct Pt>::iterator SptItem = AllPointsList.begin(); SptItem != AllPointsList.end(); SptItem++)
			{
				Pt newpt;
				newpt.X = SptItem->X;
				newpt.Y = SptItem->Y;
				newpt.pt_index = Cnt;
				if(newpt.Y < TopMost.Y)
				{
					TopMost.X = newpt.X; TopMost.Y = newpt.Y; 
					TopMost.pt_index = newpt.pt_index;
				}
				if(newpt.Y > BottomMost.Y)
				{
					BottomMost.X = newpt.X; BottomMost.Y = newpt.Y; 
					BottomMost.pt_index = newpt.pt_index;
				}
				if(newpt.X < LeftMost.X)
				{
					LeftMost.X = newpt.X; LeftMost.Y = newpt.Y; 
					LeftMost.pt_index = newpt.pt_index;
				}
				if(newpt.X > RightMost.X)
				{
					RightMost.X = newpt.X; RightMost.Y = newpt.Y; 
					RightMost.pt_index = newpt.pt_index;
				}
				
				AllPointsColl[Cnt] = newpt;
				Cnt++;
			}
			Cnt = 0;
			Pt FisrtPt;

			if (RightMost.X > BoundryThreshold[0])
			{
				FisrtPt.X = RightMost.X; FisrtPt.Y = RightMost.Y;
				FisrtPt.pt_index = 0;
				AllPointsColl.erase(RightMost.pt_index);
			}
			else if(TopMost.Y < BoundryThreshold[1])
			{
				FisrtPt.X = TopMost.X; FisrtPt.Y = TopMost.Y;
				FisrtPt.pt_index = 0;
				AllPointsColl.erase(TopMost.pt_index);
			}
			else if(BottomMost.Y > BoundryThreshold[2])
			{
				FisrtPt.X = BottomMost.X; FisrtPt.Y = BottomMost.Y;
				FisrtPt.pt_index = 0;
				AllPointsColl.erase(BottomMost.pt_index);
			}
			else
			{
				FisrtPt.X = LeftMost.X; FisrtPt.Y = LeftMost.Y;
				FisrtPt.pt_index = 0;
				AllPointsColl.erase(LeftMost.pt_index);
			}
			//toppoint.pt_index = 0;
			int TotalCnt = AllPointsColl.size() + 1;
			AllPointsTempCollection[0] = FisrtPt;
			ArrangedPointsList->push_back(FisrtPt);
			
		
			for(int i = 0; i < TotalCnt - 1; i++)
			{
				Pt tempPoint = AllPointsTempCollection[Cnt];
				Pt CurrentPt;
				double dist = 0, mindist = 0; flag = true;
				int C_index;
				for(map<int, Pt>::iterator Pitem = AllPointsColl.begin(); Pitem != AllPointsColl.end(); Pitem++)
				{
					Pt Cpt = (*Pitem).second;
					double tempdist = 0;
					tempdist = abs(tempPoint.X - Cpt.X);  
					dist = abs(tempPoint.Y - Cpt.Y);
					if (tempdist > dist)
					dist = tempdist;
					if(flag)
					{ 
						mindist = dist; flag = false;
						CurrentPt.X = Cpt.X; CurrentPt.Y = Cpt.Y;
						C_index = Cpt.pt_index;
					}
					else
					{
						if(mindist > dist)
						{
							mindist = dist;
							CurrentPt.X = Cpt.X; CurrentPt.Y = Cpt.Y;
							C_index = Cpt.pt_index;
						}
					}
				}
				Cnt++;
				CurrentPt.pt_index = Cnt;
				AllPointsTempCollection[Cnt] = CurrentPt;
				if (mindist > minDistThreshold)
				{
					Pt DilimiterPt;
					DilimiterPt.X = -1;
					DilimiterPt.Y = -1;
					ArrangedPointsList->push_back(DilimiterPt);
				}
				ArrangedPointsList->push_back(CurrentPt);
				AllPointsColl.erase(C_index);
			}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0048", __FILE__, __FUNCSIG__); return; }
	}

	//---------------------------------------------------------------------------------------
	//The functions for detecting ID and OD of an O-ring
	//---------------------------------------------------------------------------------------

	//Public function to detect ID/OD
	int EdgeDetectionSingleChannel::DetectIDOD(int *FrameGrabBox, bool ID)
	{
		try
		{
			std::list<struct Pt> UnarrangedPointsList, PointsList;
			int Box[4] = {FrameGrabBox[0], FrameGrabBox[1], FrameGrabBox[2], FrameGrabBox[3]};
			
			DetectIDODHelper(Box, 8, &PointsList, ID);
			if(PointsList.size() < 1) return 0;
			if(!TraverseEdgeStraightRectForIDOD(Box, &PointsList)) return 0;

			//Now put all the points in the linear array after correction if required...
			return AddEdgePtsToCollection(PointsList);			
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0049", __FILE__, __FUNCSIG__); return 0; }
	
	}

	//Helper function that detects ID/OD
	void EdgeDetectionSingleChannel::DetectIDODHelper(int *Box, int Resolution, std::list<struct Pt> *PointsList, bool ID)
	{
		try
		{
			int XIterator, YIterator, Incrementer = Resolution / 2;
			double EdgePoint;
			struct Pt Point;
			int LeftBoundry = Box[0] + BoundryBuffer;
			int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
			int TopBoundry = Box[1] + BoundryBuffer;
			int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;
			int LookAhead = 5;
			bool FalseEdge = false;
			double *ArrayBlock = (double*)malloc(sizeof(double) * Resolution);

			int XTotal = 0, YTotal = 0, NumOfPts = 0;
			for(int i = TopBoundry; i <= BottomBoundry; i++)
				for(int j = LeftBoundry; j <= RightBoundry; j++)
				{
					if(iBytes[i * ImageWidth + j] < 80)
					{
						XTotal += j;
						YTotal += i;
						NumOfPts++;
					}
				}

			if(ID)
			{
				LeftBoundry = (int)(XTotal / NumOfPts);
				//RightBoundry = ImageWidth - BoundryBuffer - 1;
				TopBoundry = (int)(YTotal / NumOfPts) - 20;
				BottomBoundry = (int)(YTotal / NumOfPts) + 20;
			}
			else
			{
				TopBoundry = (int)(YTotal / NumOfPts) - 50;
				BottomBoundry = (int)(YTotal / NumOfPts) + 50;
			}
			
			//Move from one row to the next downwards
			for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
			{
				//Move forward along the row rightwards
				for(XIterator = LeftBoundry; XIterator <= RightBoundry - 1 - Resolution; XIterator += Incrementer)
				{
					//Compare the 2 pixel values which are "Resolution" no. of pixels apart
					if(abs(iBytes[YIterator * ImageWidth + XIterator] - iBytes[YIterator * ImageWidth + XIterator + Resolution - 1]) >= Tolerance)
					{
						//Now we need to check if the difference continues for the next few pixels
						//Only then it is a valid edge
						for(int i = XIterator + Resolution; i <= XIterator + Resolution - 1 + LookAhead && i <= RightBoundry; i++)
						{
							//Now if the difference is not within threshold,
							//Flag it as an invalid edge and break off from the loop
							if(abs(iBytes[YIterator * ImageWidth + XIterator] - iBytes[YIterator * ImageWidth + i]) < Tolerance)
							{
								FalseEdge = true;
								break;
							}
						}

						//Once out of the loop, check if the edge is validated
						if(!FalseEdge)
						{
							//Copy the pixel values into the array
							for(int i = 0; i < Resolution; i++)
								ArrayBlock[i] = iBytes[YIterator * ImageWidth + XIterator + i];

							//Call the function to pecisely locate the edge point
							if(PinPointEdgeWithinBlock(ArrayBlock, Resolution, &EdgePoint))
							{
								Point.X = XIterator + EdgePoint;
								Point.Y = YIterator;
								PointsList->push_back(Point); //Put the point into the list
								if(ArrayBlock != NULL)
								{
									free(ArrayBlock);
									ArrayBlock = NULL;
								}
								return;
							}
							//The locating of the edge failed..
							else
								FalseEdge = false; //Reset the flag
						}
						else
						{
							FalseEdge = false; //Reset the flag
						}
					}
				}
			}
			if(ArrayBlock != NULL)
			{
				free(ArrayBlock);
				ArrayBlock = NULL;
			}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0050", __FILE__, __FUNCSIG__); return; }
	}

	//This special case traverse edge function
	bool EdgeDetectionSingleChannel::TraverseEdgeStraightRectForIDOD(int *Box, std::list<struct Pt> *PointsList)
	{
		try
		{
			int X, Y, XPrevious, YPrevious;
			Pt PrvToPrvPt, PtAtOtherEnd;
			bool TraverseDirection = true, EdgeLoopBack = false;

			int LeftBoundry = Box[0] + BoundryBuffer;
			int RightBoundry = Box[0] + Box[2] - BoundryBuffer - 1;
			int TopBoundry = Box[1] + BoundryBuffer;
			int BottomBoundry = Box[1] + Box[3] - BoundryBuffer - 1;

			X = PointsList->begin()->X + 0.5;
			Y = PointsList->begin()->Y + 0.5;
			XPrevious = 0;
			YPrevious = 0;
			PrvToPrvPt.X = 0;
			PrvToPrvPt.Y = 0;
			PtAtOtherEnd.X = X;
			PtAtOtherEnd.Y = Y;
			int sd = 1;
			//if (SDir == Leftwards) sd = -1;

			if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
			else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
			else if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
			else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd));
			else return false;
			
			for(int DirectionCount = 0; DirectionCount < 2; DirectionCount++)
			{
				int PointsCount = 0;
				while(X <= RightBoundry && X >= LeftBoundry && Y <= BottomBoundry && Y >= TopBoundry)
				{
					PointsCount++;
					if(PointsCount > 1500) 
						break;
					if(YPrevious == Y)
					{
						if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
							continue;
						else if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
							continue;
					}
					else
					{
						if(YPrevious == Y - 1)
						{
							if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						}
						else if(YPrevious == Y + 1)
						{
							if(ScanHorizontalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						}
					}
					if(EdgeLoopBack) return true;

					if(XPrevious == X)
					{
						if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
							continue;
						else if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
							continue;
					}
					else
					{
						if(XPrevious == X - 1)
						{
							if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, false, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						}
						else if(XPrevious == X + 1)
						{
							if(ScanVerticalAndInsertToList(&X, &Y, &XPrevious, &YPrevious, &PrvToPrvPt, true, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd, sd))
								continue;
						}
					}
					if(EdgeLoopBack) return true;

					Pt Pnt1, Pnt2;
					std::list<Pt>::iterator Itr;
					if(PointsList->size() >= 15)
					{
						if(TraverseDirection)
						{
							Itr = PointsList->end();
							for(int j = 1; j<= 15; j++) Itr--;
							
							if(!DustSkipIDOD(*(--(PointsList->end())), *(Itr), &Pnt1, &Pnt2, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd)) break;
						}
						else
						{
							Itr = PointsList->begin();
							for(int j = 1; j<= 14; j++) Itr++;
							
							if(!DustSkipIDOD(*(PointsList->begin()), *(Itr), &Pnt1, &Pnt2, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd)) break;
						}
					}
					else
					{
						if(TraverseDirection)
						{
							if(!DustSkipIDOD(*(--(PointsList->end())), *(PointsList->begin()), &Pnt1, &Pnt2, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd)) break;
						}
						else
						{
							if(!DustSkipIDOD(*(PointsList->begin()), *(--(PointsList->end())), &Pnt1, &Pnt2, PointsList, TraverseDirection, &EdgeLoopBack, PtAtOtherEnd)) break;
						}
					}
					if(EdgeLoopBack) return true;
					
					if(Pnt1.X > RightBoundry || Pnt1.X < LeftBoundry || Pnt1.Y > BottomBoundry || Pnt1.Y < TopBoundry) break;
					if(Pnt2.X > RightBoundry || Pnt2.X < LeftBoundry || Pnt2.Y > BottomBoundry || Pnt2.Y < TopBoundry) break;
					
					if(TraverseDirection)
					{
						PointsList->push_back(Pnt1);
						PointsList->push_back(Pnt2);
					}
					else
					{
						PointsList->push_front(Pnt1);
						PointsList->push_front(Pnt2);
					}

					X = Pnt2.X + 0.5;
					Y = Pnt2.Y + 0.5;
					XPrevious = Pnt1.X + 0.5;
					YPrevious = Pnt1.Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}

				if(DirectionCount == 0)
				{
					TraverseDirection = false;
					EdgeLoopBack = false;
					PtAtOtherEnd.X = (--(PointsList->end()))->X + 0.5;
					PtAtOtherEnd.Y = (--(PointsList->end()))->Y + 0.5; 
				
					X = PointsList->begin()->X + 0.5;
					Y = PointsList->begin()->Y + 0.5;
					XPrevious = (++(PointsList->begin()))->X + 0.5;
					YPrevious = (++(PointsList->begin()))->Y + 0.5;
					PrvToPrvPt.X = 0;
					PrvToPrvPt.Y = 0;
				}
			}
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0051", __FILE__, __FUNCSIG__); return false; }
	}

	//This function Skips the dust/irregularities for the special case traverse edge functions for ID/OD detection
	bool EdgeDetectionSingleChannel::DustSkipIDOD(Pt Point1, Pt Point2, Pt *Pnt1, Pt *Pnt2, std::list<struct Pt> *PointsList, bool TraverseDirection, bool *EdgeLoopBack, Pt PtAtOtherEnd)
	{
		try
		{
			double pixels[40] = {0};
			int XIterator, YIterator,  LookAheadIndex, i;
			double var;
			struct Pt Point;
			bool OneEdgeDetected = false, TwoEdgeDetected = false;
			int LeftBoundry, RightBoundry, TopBoundry, BottomBoundry, ScanLength = 5, ScanWidth = 15;
			int PixelArrayIndex;
			std::list<struct Pt> TmpPointsList;
			
			//if (x1-x2) lesser than (y1-y2), then we need to do horizontal scan)
			if(abs(Point1.X - Point2.X) < abs(Point1.Y - Point2.Y))
				//if y1 > y2, then we need to go from row to row in the downward direction
				if(Point1.Y > Point2.Y)
				{
					LeftBoundry = Point1.X + 0.5 - ScanWidth / 2; RightBoundry = Point1.X + 0.5 + ScanWidth / 2;
					TopBoundry = Point1.Y + 1 + 0.5; BottomBoundry = Point1.Y + ScanLength + 0.5;
					if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for(YIterator = BottomBoundry; YIterator >= TopBoundry; YIterator--)
						for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + 3)]) < Tolerance)
								continue;

							//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
							//for this purpose, first it must be determined till where this checking must be carried out
							if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
								//the checking can continue for 'look ahead' number of pixels
								LookAheadIndex = LookAheadLimit;
							else
								//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
								LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
							//This is where the actual checking happens the range for which has been determined in the previous if statement
							for(i = 4; i <= LookAheadIndex; i++)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									//its not a legitimate edge as there is no continuity of transition
									//hence jump to the point where the check failed and continue searching for edge from this point forward
									XIterator = XIterator + i - PixelSkipValue; 
									goto last1;
								}

							//copy the 4 pixels where edge has been detected into 'pixels' array
							for(int j = 0; j <= 3; j++)
								pixels[j] = iBytes[PixelArrayIndex + j];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							
							if(var == -1) 
								goto last1;
						
							Point.X = XIterator + var;
							Point.Y = YIterator;
							
							if(abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
							{
								*EdgeLoopBack = true;
								return true;
							}

							if(!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else if(!TwoEdgeDetected)
							{
								*Pnt1 = Point;
								TwoEdgeDetected = true;
								if(abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > 1) return false;
							}
							else
							{
								TmpPointsList.push_back(Point);
							}
							break;
												
	last1:					i = 1;
						}
				}
						
				else
				//we need to go from row to row in the upward direction
				{
					LeftBoundry = Point1.X + 0.5 - ScanWidth / 2; RightBoundry = Point1.X + 0.5 + ScanWidth / 2;
					TopBoundry = Point1.Y - ScanLength + 0.5; BottomBoundry = Point1.Y - 1 + 0.5;
					if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator++)
						for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + 3)]) < Tolerance)
								continue;

							//edge identified, now it should be checked for legitimacy i.e if the transition continues for the next few pixels
							//for this purpose, first it must be determined till where this checking must be carried out
							if((XIterator + LookAheadLimit) < ImageWidth - BoundryBuffer)
								//the checking can continue for 'look ahead' number of pixels
								LookAheadIndex = LookAheadLimit;
							else
								//checking can happen only till the end of the column, as 'look ahed' number exceeds the column boundry
								LookAheadIndex = ImageWidth - XIterator - BoundryBuffer - 1;
						
							//This is where the actual checking happens the range for which has been determined in the previous if statement
							for(i = 4; i <=  LookAheadIndex; i++)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									//its not a legitimate edge as there is no continuity of transition
									//hence jump to the point where the check failed and continue searching for edge from this point forward
									XIterator = XIterator + i - PixelSkipValue;
									goto last2;
								}

							//copy the 4 pixels where edge has been detected into 'pixels' array
							for(int j = 0; j <= 3; j++)
								pixels[j] = iBytes[PixelArrayIndex + j];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							
							if(var == -1) 
								goto last2;
						
							Point.X = XIterator + var;
							Point.Y = YIterator;

							if(abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
							{
								*EdgeLoopBack = true;
								return true;
							}

							if(!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else if(!TwoEdgeDetected)
							{
								*Pnt1 = Point;
								TwoEdgeDetected = true;
								if(abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > 1) return false;
							}
							else
							{
								TmpPointsList.push_back(Point);
							}
							break;
												
	last2:					i = 1;
						}
				}
			else
			//Vertical scan
				if(Point1.X > Point2.X)
				//if x1 > x2, then we need to go from column to column in the rightward direction
				{
					LeftBoundry = Point1.X + 0.5 + 1; RightBoundry = Point1.X + 0.5 + ScanLength;
					TopBoundry = Point1.Y - ScanWidth / 2 + 0.5; BottomBoundry = Point1.Y + ScanWidth / 2 + 0.5;
					if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for(XIterator = RightBoundry; XIterator >= LeftBoundry; XIterator--)
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + 3) * ImageWidth)]) < Tolerance)
								continue;

							if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
						
							for(i = 4 * ImageWidth; i <=  LookAheadIndex; i += ImageWidth)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									YIterator = YIterator + i /ImageWidth - PixelSkipValue;
									goto last3;
								}
							for(int j = 0; j <= 3; j++)
								pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							
							if(var == -1) 
								goto last3;
						
							Point.X = XIterator;
							Point.Y = YIterator + var;

							if(abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
							{
								*EdgeLoopBack = true;
								return true;
							}

							if(!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else if(!TwoEdgeDetected)
							{
								*Pnt1 = Point;
								TwoEdgeDetected = true;
								if(abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > 1) return false;
							}
							else
							{
								TmpPointsList.push_back(Point);
							}
							break;

	last3:					i = 1;
						}
				}
				else
				{
					LeftBoundry = Point1.X + 0.5 - ScanLength; RightBoundry = Point1.X + 0.5 - 1;
					TopBoundry = Point1.Y - ScanWidth / 2 + 0.5; BottomBoundry = Point1.Y + ScanWidth / 2 + 0.5;
					if(LeftBoundry < BoundryBuffer || RightBoundry > ImageWidth - BoundryBuffer - 1 || TopBoundry < BoundryBuffer || BottomBoundry > ImageHeight - BoundryBuffer - 1)
						return false;
					for(XIterator = LeftBoundry; XIterator <= RightBoundry; XIterator++)
						for(YIterator = TopBoundry; YIterator <= BottomBoundry; YIterator += PixelSkipValue)
						{
							PixelArrayIndex = XIterator + YIterator * ImageWidth;

							//the following if statement searches for a definite edge. If not found, it skips to the next loop
							if(abs(iBytes[(PixelArrayIndex)] - iBytes[(XIterator + (YIterator + 3) * ImageWidth)]) < Tolerance)
								continue;

							if((YIterator + LookAheadLimit) < ImageHeight - BoundryBuffer)
								LookAheadIndex = LookAheadLimit * ImageWidth;
							else
								LookAheadIndex = (ImageHeight - YIterator - BoundryBuffer - 1) * ImageWidth;
						
							for(i = 4 * ImageWidth; i <=  LookAheadIndex; i +=ImageWidth)
								if(abs(iBytes[(PixelArrayIndex)] - iBytes[(PixelArrayIndex + i)]) < Tolerance)
								{
									YIterator = YIterator + i / ImageWidth - PixelSkipValue;
									goto last4;
								}
							for(int j = 0; j <= 3; j++)
								pixels[j] = iBytes[(XIterator + (YIterator + j) * ImageWidth)];
						
							//var = PinPointEdge(pixels);
							var = PinPointEdge(XIterator, YIterator, pixels);
							
							if(var == -1) 
								goto last4;
						
							Point.X = XIterator;
							Point.Y = YIterator + var;

							if(abs(Point.X - PtAtOtherEnd.X) < 3 && abs(Point.Y - PtAtOtherEnd.Y) < 3 && PointsList->size() > 10)
							{
								*EdgeLoopBack = true;
								return true;
							}

							if(!OneEdgeDetected)
							{
								*Pnt2 = Point;
								OneEdgeDetected = true;
							}
							else if(!TwoEdgeDetected)
							{
								*Pnt1 = Point;
								TwoEdgeDetected = true;
								if(abs((int)(Pnt1->X + 0.5) - (int)(Pnt2->X + 0.5)) > 1 || abs((int)(Pnt1->Y + 0.5) - (int)(Pnt2->Y + 0.5)) > 1) return false;
							}
							else
							{
								TmpPointsList.push_back(Point);
							}
							break;

	last4:					i = 1;
						}
				}
			if(!TwoEdgeDetected) return false;

			TmpPointsList.reverse();
			for(std::list<Pt>::iterator itr = TmpPointsList.begin(); itr != TmpPointsList.end(); itr++)
			{
				if(TraverseDirection)
					PointsList->push_back(*itr);
				else
					PointsList->push_front(*itr);
			}
			return true;
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0052", __FILE__, __FUNCSIG__); return false; }
	}

	void EdgeDetectionSingleChannel::Masking_Binarization(int *Box, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int MaskingLevel)
	{
		try
		{
			int ArrayIndex = 0, Row_Iterator = 0, Column_Iterator = 0;
			int LeftBoundry = Box[0], RightBoundry = Box[1], TopBoundry = Box[2], BottomBoundry = Box[3];
			double *TempByteArray = new double[ImageWidth * ImageHeight];
			memcpy(TempByteArray, iBytes, ImageWidth * ImageHeight * sizeof(double));
			for(int i = 0; i < MaskingLevel; i++)
			{
				for(Row_Iterator = TopBoundry; Row_Iterator <= BottomBoundry; Row_Iterator++)
				{
					for(Column_Iterator = LeftBoundry; Column_Iterator <= RightBoundry; Column_Iterator++)
					{
						ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
						TempByteArray[ArrayIndex] = GetMedianFilterValue(Column_Iterator, Row_Iterator, MaskSize, DoAverageFiltering);
					}
				}		
				for(Row_Iterator = TopBoundry; Row_Iterator <= BottomBoundry; Row_Iterator++)
				{
					for(Column_Iterator = LeftBoundry; Column_Iterator <= RightBoundry; Column_Iterator++)
					{
						ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
						if(TempByteArray[ArrayIndex] < Binary_LowerFactor)
							iBytes[ArrayIndex] = 0;
						else if(TempByteArray[ArrayIndex] > Binary_UpperFactor)
							iBytes[ArrayIndex] = 255;
						else
							iBytes[ArrayIndex] = TempByteArray[ArrayIndex];
					}
				}
			}
			delete [] TempByteArray;
		}
		catch(...)
		{
			SetAndRaiseErrorMessage("EDSC0053", __FILE__, __FUNCSIG__);
		}
	}

	void EdgeDetectionSingleChannel::Masking_BinarizationCurve(double *Box, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int MaskingLevel)
	{
		try
		{
			Pt OriginPoint;
			int ArrayIndex = 0, Xoriginal = 0, Yoriginal = 0;
			double pixels[40] = {0};
			double R1, R2, Theta1, Theta2, Xoriginal_d = 0, Yoriginal_d = 0;
				
			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];
			double *TempByteArray = new double[ImageWidth * ImageHeight];
			memcpy(TempByteArray, iBytes, ImageWidth * ImageHeight * sizeof(double));
			double IncrementalAngle = 2 / (R1 + R2); //incrementing the angle so that linear increment is one pixel: ang = 1 / ((R1 + R2) / 2)
			for(int i = 0; i < MaskingLevel; i++)
			{
				for(double ThetaIterator = Theta1; ThetaIterator < Theta2; ThetaIterator += IncrementalAngle)
				{
					for(double RIterator = R1; RIterator < R2; RIterator++)
					{
						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5) - 1;
						Yoriginal = (int)(Yoriginal_d + 0.5) - 1;
						if(Xoriginal >= ImageWidth - MaskSize / 2 || Xoriginal < MaskSize / 2 || Yoriginal >= ImageHeight - MaskSize / 2 || Yoriginal <  MaskSize / 2)
							continue;			
						ArrayIndex = Xoriginal + Yoriginal * ImageWidth;
						TempByteArray[ArrayIndex] = GetMedianFilterValue(Xoriginal, Yoriginal, MaskSize, DoAverageFiltering);
					}
				}	
				for(double ThetaIterator = Theta1; ThetaIterator < Theta2; ThetaIterator += IncrementalAngle)
				{
					for(double RIterator = R1; RIterator < R2; RIterator++)
					{
						GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5) - 1;
						Yoriginal = (int)(Yoriginal_d + 0.5) - 1;
						ArrayIndex = Xoriginal + Yoriginal * ImageWidth;
						if(TempByteArray[ArrayIndex] < Binary_LowerFactor)
							iBytes[ArrayIndex] = 0;
						else if(TempByteArray[ArrayIndex] > Binary_UpperFactor)
							iBytes[ArrayIndex] = 255;
						else
							iBytes[ArrayIndex] = TempByteArray[ArrayIndex];
					}
				}
			}
			delete [] TempByteArray;
		}
		catch(...)
		{
			SetAndRaiseErrorMessage("EDSC0053", __FILE__, __FUNCSIG__);
		}
	}

	void EdgeDetectionSingleChannel::Masking_BinarizationAngRect(int *Box, int MaskSize, int Binary_LowerFactor, int Binary_UpperFactor, int MaskingLevel)
	{
		try
		{
			Pt Point1, Point2, OriginPoint;
			int Height, Width = Box[4], XIterator, YIterator, Xoriginal, Yoriginal;
			double Xoriginal_d, Yoriginal_d;
			double CosTh = 0, SinTh = 0, PixelDiff = 0, lastVal = 0;
			int i = 0, ArrayIndex = 0;
			//The lower point(point with higher Y value in pixel coordinates) is taken as the first point
			if(Box[1] > Box[3])
			{	
				Point1.X = Box[0];
				Point1.Y = Box[1];
				Point2.X = Box[2];
				Point2.Y = Box[3];
			}
			else
			{
				Point1.X = Box[2];
				Point1.Y = Box[3];
				Point2.X = Box[0];
				Point2.Y = Box[1];
			}
			
			CosTh = (Point2.X - Point1.X) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));
			SinTh = (Point1.Y - Point2.Y) / sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2));

			//The left top corner of the rect is taken as the origin for the transformation
			OriginPoint.X = Point2.X - Width / 2 * SinTh;
			OriginPoint.Y = Point2.Y - Width / 2 * CosTh;

			Height = sqrt(pow((Point2.X - Point1.X),2)+pow((Point2.Y - Point1.Y),2)); 

			double *TempByteArray = new double[ImageWidth * ImageHeight];
			memcpy(TempByteArray, iBytes, ImageWidth * ImageHeight * sizeof(double));

			for(int k = 0; k < MaskingLevel; k++)
			{
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = 0; XIterator < Width; XIterator++)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						if(Xoriginal >= ImageWidth - MaskSize / 2 || Xoriginal < MaskSize / 2 || Yoriginal >= ImageHeight - MaskSize / 2 || Yoriginal <  MaskSize / 2)
							continue;			
						ArrayIndex = Xoriginal + Yoriginal * ImageWidth;
						TempByteArray[ArrayIndex] = GetMedianFilterValue(Xoriginal, Yoriginal, MaskSize, DoAverageFiltering);
					}
				}	
				for(YIterator = 0; YIterator < Height; YIterator++)
				{
					for(XIterator = 0; XIterator < Width; XIterator++)
					{
						GetOriginalCoordinates(XIterator, YIterator, OriginPoint.X, OriginPoint.Y, CosTh, SinTh, &Xoriginal_d, &Yoriginal_d);
						Xoriginal = (int)(Xoriginal_d + 0.5);
						Yoriginal = (int)(Yoriginal_d + 0.5);
						ArrayIndex = Xoriginal + Yoriginal * ImageWidth;
						if(TempByteArray[ArrayIndex] < Binary_LowerFactor)
							iBytes[ArrayIndex] = 0;
						else if(TempByteArray[ArrayIndex] > Binary_UpperFactor)
							iBytes[ArrayIndex] = 255;
						else
							iBytes[ArrayIndex] = TempByteArray[ArrayIndex];
					}
				}
			}
			//int j = 0;
			//for (int i = 0; i < ImageWidth * ImageHeight * 3; i++)
			//{
			//	BinImage[j++] = (BYTE)((int)iBytes[i]);
			//	BinImage[j++] = (BYTE)((int)iBytes[i]);
			//	BinImage[j++] = (BYTE)((int)iBytes[i]);
			//}
			delete [] TempByteArray;
		}
		catch(...)
		{
			SetAndRaiseErrorMessage("EDSC0053", __FILE__, __FUNCSIG__);
		}
	}
	
	void EdgeDetectionSingleChannel::DetectSurfaceEdgeWithoutDirection(int *Box, int PixelThreshold, std::list<struct Pt> *PointsList, ScanDirection scan_Dir, bool dirGiven)
	{
		try
		{		
			ScanDirection Scan_Dir = scan_Dir;
			int LeftBoundry = Box[0], RightBoundry = Box[1], TopBoundry = Box[2], BottomBoundry = Box[3];
			if(!dirGiven)
			{
				int ArrayIndex = 0, Row_Iterator = 0, Column_Iterator = 0, NextArrayIndex = 0;
				double FirstRow = 0, LastRow = 0, FirstColumn = 0, LastColumn = 0;
				for(Column_Iterator = LeftBoundry; Column_Iterator < RightBoundry; Column_Iterator++)
				{
					ArrayIndex = Column_Iterator + TopBoundry * ImageWidth;
					NextArrayIndex = Column_Iterator + BottomBoundry * ImageWidth;
					FirstRow += iBytes[ArrayIndex];
					LastRow += iBytes[NextArrayIndex];
				}

				FirstRow = FirstRow / (RightBoundry - LeftBoundry - 1);
				LastRow = LastRow / (RightBoundry - LeftBoundry - 1);

				for(Row_Iterator = TopBoundry; Row_Iterator < BottomBoundry; Row_Iterator++)
				{
					ArrayIndex = LeftBoundry + Row_Iterator * ImageWidth;
					NextArrayIndex = RightBoundry + Row_Iterator * ImageWidth;
					FirstColumn += iBytes[ArrayIndex];
					LastColumn += iBytes[NextArrayIndex];
				}

				FirstColumn = FirstColumn / (BottomBoundry - TopBoundry - 1);
				LastColumn = LastColumn / (BottomBoundry - TopBoundry - 1);

				if(abs(FirstRow - LastRow) < abs(FirstColumn - LastColumn))
				{
					if(FirstColumn > LastColumn)
						Scan_Dir = Rightwards;
					else
						Scan_Dir = Leftwards;
				}
				else
				{
					if(FirstRow > LastRow)
						Scan_Dir = Downwards;
					else
						Scan_Dir = Upwards;
				}
			}
			std::list<struct Pt> PointsList1, PointsList2;
			DetectSurfaceEdgeHelperPeakPoint(LeftBoundry, RightBoundry, TopBoundry, BottomBoundry, PixelThreshold, Scan_Dir, &PointsList1);
			/*if(PointsList->size() < MinimumPointsInEdge)
			{*/
			if(Scan_Dir == Rightwards || Scan_Dir == Leftwards)
				Scan_Dir = Upwards;
			else
				Scan_Dir = Rightwards;
			DetectSurfaceEdgeHelperPeakPoint(LeftBoundry, RightBoundry, TopBoundry, BottomBoundry, PixelThreshold, Scan_Dir, &PointsList2);
			/*}*/
			if(PointsList1.size() > PointsList2.size())
			{
				for each(Pt Pnt in PointsList1)
				{
					PointsList->push_back(Pnt);
				}
			}
			else
			{
				for each(Pt Pnt in PointsList2)
				{
					PointsList->push_back(Pnt);
				}
			}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0054", __FILE__, __FUNCSIG__); }
	}

	void EdgeDetectionSingleChannel::DetectSurfaceEdgeHelperPeakPoint(int LeftBoundry, int RightBoundry, int TopBoundry, int BottomBoundry, int PixelThreshold, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList)
	{
		try
		{		
			struct Pt Point, prevPt;
			bool GotPoint = false;
			double PixelDiff = 0;
			int ArrayIndex = 0, Row_Iterator = 0, Column_Iterator = 0, NextArrayIndex = 0, PtId = 0;
			if(ApplyDirectionalScan)
			{
				switch(Scan_Direction)
				{
				  case Rightwards:
					for(Row_Iterator = TopBoundry; Row_Iterator < BottomBoundry; Row_Iterator++)
					{
						PixelDiff = 0; GotPoint = false;
						for(Column_Iterator = LeftBoundry + 1; Column_Iterator < RightBoundry; Column_Iterator++)
						{
							ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
							NextArrayIndex = ArrayIndex + 1;
							double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
							if(TmpValue > PixelDiff)
							{
								PixelDiff = TmpValue;
								GotPoint = true;
								Point.X = Column_Iterator - 0.5; Point.Y = Row_Iterator;
								if(TmpValue >= Type6JumpThreshold) break;
							}
						}
						if(GotPoint)
						{
							if (PointsList->size() > 0)
							{ // If current point is within 1 pixel of previous point due to some scanning artifacts, we ignore the point
								prevPt = PointsList->back();
								if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
									continue;
							}
							Point.pt_index = PtId++;
							PointsList->push_back(Point);
						}				
					}
					break;
				  case Leftwards:
					for(Row_Iterator = TopBoundry; Row_Iterator < BottomBoundry; Row_Iterator++)
					{
						PixelDiff = 0; GotPoint = false;
						for(Column_Iterator = RightBoundry - 1; Column_Iterator > LeftBoundry; Column_Iterator--)
						{
							ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
							NextArrayIndex = ArrayIndex + 1;
							double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
							if(TmpValue > PixelDiff)
							{
								PixelDiff = TmpValue;
								GotPoint = true;
								Point.X = Column_Iterator - 0.5; Point.Y = Row_Iterator;
								if(TmpValue >= Type6JumpThreshold) break;
							}
						}
						if(GotPoint)
						{
							if (PointsList->size() > 0)
							{
								prevPt = PointsList->back();
								if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
									continue;
							}
							Point.pt_index = PtId++;
							PointsList->push_back(Point);
						}				
					}
					break;
				  case Downwards:
					for(Column_Iterator = LeftBoundry; Column_Iterator < RightBoundry; Column_Iterator++)
					{
						PixelDiff = 0; GotPoint = false;
						for(Row_Iterator = TopBoundry + 1; Row_Iterator < BottomBoundry; Row_Iterator++)
						{
							ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
							NextArrayIndex = Column_Iterator + (Row_Iterator + 1) * ImageWidth;
							double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
							if(TmpValue > PixelDiff)
							{
								PixelDiff = TmpValue;
								GotPoint = true;
								Point.X = Column_Iterator; Point.Y = Row_Iterator - 0.5;
								if(TmpValue >= Type6JumpThreshold) break;
							}
						}
						if(GotPoint)
						{
							if (PointsList->size() > 0)
							{
								prevPt = PointsList->back();
								if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
									continue;
							}
							Point.pt_index = PtId++;
							PointsList->push_back(Point);
						}
					}
					break;
				  case Upwards:
					for(Column_Iterator = LeftBoundry; Column_Iterator < RightBoundry; Column_Iterator++)
					{
						PixelDiff = 0; GotPoint = false;
						for(Row_Iterator = BottomBoundry - 1; Row_Iterator > TopBoundry; Row_Iterator--)
						{
							ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
							NextArrayIndex = Column_Iterator + (Row_Iterator + 1) * ImageWidth;
							double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
							if(TmpValue > PixelDiff)
							{
								PixelDiff = TmpValue;
								GotPoint = true;
								Point.X = Column_Iterator; Point.Y = Row_Iterator - 0.5;
								if(TmpValue >= Type6JumpThreshold) break;
							}
						}
						if(GotPoint)
						{
							if (PointsList->size() > 0)
							{
								prevPt = PointsList->back();
								if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
									continue;
							}
							Point.pt_index = PtId++;
							PointsList->push_back(Point);
						}
					}
					break;
				}
			}
			else
			{
				switch(Scan_Direction)
				{
					case Rightwards:
					case Leftwards:
						for(Row_Iterator = TopBoundry; Row_Iterator < BottomBoundry; Row_Iterator++)
						{
							PixelDiff = Type6JumpThreshold; GotPoint = false;
							for(Column_Iterator = LeftBoundry + 1; Column_Iterator < RightBoundry; Column_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = ArrayIndex + 1;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff && (iBytes[NextArrayIndex] < PixelThreshold || iBytes[ArrayIndex] < PixelThreshold))
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator - 0.5; Point.Y = Row_Iterator;
								}
							}
							if(GotPoint)
							{
								if (PointsList->size() > 0)
								{
									prevPt = PointsList->back();
									if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
										continue;
								}
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
							}				
						}
						break;
					case Downwards:
					case Upwards:
						for(Column_Iterator = LeftBoundry; Column_Iterator < RightBoundry; Column_Iterator++)
						{
							PixelDiff = Type6JumpThreshold; GotPoint = false;
							for(Row_Iterator = TopBoundry + 1; Row_Iterator < BottomBoundry; Row_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = Column_Iterator + (Row_Iterator + 1) * ImageWidth;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff && (iBytes[NextArrayIndex] < PixelThreshold || iBytes[ArrayIndex] < PixelThreshold))
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator; Point.Y = Row_Iterator - 0.5;
								}
							}
							if(GotPoint)
							{

								if (PointsList->size() > 0)
								{
									prevPt = PointsList->back();
									if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
										continue;
								}
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
							}
						}
						break;
				}
			}
			FilterOutPtsFromList(PointsList, Type6_NoiseFilterDistInPixel);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0055", __FILE__, __FUNCSIG__); }
	}

	void EdgeDetectionSingleChannel::DetectSurfaceEdgeCurvedBoxHelper(double *Box, bool OppositeScan, std::list<struct Pt> *PointsList, int PixelThreshold)
	{
		try
		{
			Pt OriginPoint, Point, prevPt;
			bool GotPoint = false, gotfirstPoint = false;
			double PixelDiff = 0;
			int ArrayIndex = 0, PtId = 0;
			double pixels[40] = {0};
			double R, R1, R2, Theta1, Theta2, lastVal = 0, Xoriginal_d = 0, Yoriginal_d = 0;
				
			OriginPoint.X = Box[0];
			OriginPoint.Y = Box[1];
			R1 = Box[2];
			R2 = Box[3];
			Theta1 = Box[4];
			Theta2 = Box[5];

			double IncrementalAngle = 2 / (R1 + R2); //incrementing the angle so that linear increment is one pixel: ang = 1 / ((R1 + R2) / 2)
			for(double ThetaIterator = Theta1; ThetaIterator < Theta2; ThetaIterator += IncrementalAngle)
			{
				PixelDiff = 0; GotPoint = false, gotfirstPoint = false;
				for(double RIterator = R1; RIterator < R2; RIterator++)
				{
					GetOriginalCoordinates(OriginPoint.X, OriginPoint.Y, RIterator, ThetaIterator, &Xoriginal_d, &Yoriginal_d);
					double Xoriginal = (int)(Xoriginal_d + 0.5);
					double Yoriginal = (int)(Yoriginal_d + 0.5);		
					ArrayIndex = Xoriginal + Yoriginal * ImageWidth;
					if(gotfirstPoint)
					{
						double TmpValue = abs(iBytes[ArrayIndex] - lastVal);
						if(TmpValue > PixelDiff && (lastVal < PixelThreshold || iBytes[ArrayIndex] < PixelThreshold))
						{
							PixelDiff = TmpValue;
							GotPoint = true;
							Point.X = Xoriginal - 0.5; Point.Y = Yoriginal;
							R = RIterator;
						}
						lastVal= iBytes[ArrayIndex];
					}
					else
					{
					   gotfirstPoint = true;
					   lastVal= iBytes[ArrayIndex];
					}					
				}
				if(GotPoint)
				{
				/*	R1 = R - 10;
					R2 = R + 10;*/
					if (PointsList->size() > 0)
					{
						prevPt = PointsList->back();
						if (abs(Point.X - prevPt.X) < 1 || abs(Point.Y - prevPt.Y) < 1)
							continue;
					}
					Point.pt_index = PtId++;
					PointsList->push_back(Point);
				}				
			}
			FilterOutPtsFromList(PointsList);
		}
		catch(...)
		{
	
		}
	}

	void EdgeDetectionSingleChannel::DetectSurfaceEdgeHelperPeakPoint(int LeftBoundry, int RightBoundry, int TopBoundry, int BottomBoundry, int PixelThreshold, ScanDirection Scan_Direction, std::list<struct Pt> *PointsList, int startrow, int startcol)
	{
		try
		{		
			struct Pt Point;
			bool GotPoint = false;
			double PixelDiff = 0;
			int notfound = 0, ArrayIndex = 0, Row_Iterator = 0, Column_Iterator = 0, NextArrayIndex = 0, strtr = startrow, strtc = startcol, PtId = 0;		
			if(ApplyDirectionalScan)
			{
				switch(Scan_Direction)
				{
					case Rightwards:
					case Leftwards:
					{						
						for(Row_Iterator = strtr + 1; Row_Iterator < BottomBoundry; Row_Iterator++)
						{
							PixelDiff = 0; GotPoint = false;
							int leftBoundry = strtc - Type6_ScanRange, rightBoundry = strtc + Type6_ScanRange;
							if(leftBoundry < LeftBoundry) leftBoundry = LeftBoundry;
							if(rightBoundry > RightBoundry) rightBoundry = RightBoundry;
							for(Column_Iterator = leftBoundry; Column_Iterator < rightBoundry; Column_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = ArrayIndex + 1;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff)
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator - 0.5; Point.Y = Row_Iterator;
									strtc = Column_Iterator;
									if(TmpValue >= Type6JumpThreshold) break;
								}
							}
							if(GotPoint)
							{
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
								notfound = 0;
							}		
							else
								notfound++;
							if (notfound > Type6_ScanRange) break;
						}
						strtc = startcol;
						for(Row_Iterator = strtr - 1; Row_Iterator > TopBoundry; Row_Iterator--)
						{
							PixelDiff = 0; GotPoint = false;
							int leftBoundry = strtc - Type6_ScanRange, rightBoundry = strtc + Type6_ScanRange;
							if(leftBoundry < LeftBoundry) leftBoundry = LeftBoundry;
							if(rightBoundry > RightBoundry) rightBoundry = RightBoundry;

							for(Column_Iterator = leftBoundry; Column_Iterator < rightBoundry; Column_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = ArrayIndex + 1;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff)
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator - 0.5; Point.Y = Row_Iterator;
									strtc = Column_Iterator;
									if(TmpValue >= Type6JumpThreshold) break;
								}
							}
							if(GotPoint)
							{
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
								notfound = 0;
							}		
							else
								notfound++;
							if (notfound > Type6_ScanRange) break;				
						}
						break;
				  }
				  case Downwards:
				  case Upwards:
				  {
						for(Column_Iterator = strtc + 1; Column_Iterator < RightBoundry; Column_Iterator++)
						{
							PixelDiff = 0; GotPoint = false;
							int topBoundry = strtr - Type6_ScanRange, bottomBoundry = strtr + Type6_ScanRange;
							if(topBoundry < TopBoundry) topBoundry = TopBoundry;
							if(bottomBoundry > BottomBoundry) bottomBoundry = BottomBoundry;
							for(Row_Iterator = topBoundry; Row_Iterator < bottomBoundry; Row_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = Column_Iterator + (Row_Iterator + 1) * ImageWidth;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff)
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator; Point.Y = Row_Iterator - 0.5;
									strtr = Row_Iterator;
									if(TmpValue >= Type6JumpThreshold) break;
								}
							}
							if(GotPoint)
							{
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
								notfound = 0;
							}		
							else
								notfound++;
							if (notfound > Type6_ScanRange) break;
						}

						strtr = startrow;
						for(Column_Iterator = strtc - 1; Column_Iterator > LeftBoundry; Column_Iterator--)
						{
							PixelDiff = 0; GotPoint = false;
							int topBoundry = strtr - Type6_ScanRange, bottomBoundry = strtr + Type6_ScanRange;
							if(topBoundry < TopBoundry) topBoundry = TopBoundry;
							if(bottomBoundry > BottomBoundry) bottomBoundry = BottomBoundry;
							for(Row_Iterator = topBoundry; Row_Iterator < bottomBoundry; Row_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = Column_Iterator + (Row_Iterator + 1) * ImageWidth;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff)
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator; Point.Y = Row_Iterator - 0.5;
									strtr = Row_Iterator;
									if(TmpValue >= Type6JumpThreshold) break;
								}
							}
							if(GotPoint)
							{
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
								notfound = 0;
							}		
							else
							{
								notfound++;
								strtr++;
							}
							if (notfound > Type6_ScanRange) break;
						}
						break;
					}
				}
			}
			else
			{
				switch(Scan_Direction)
				{
					 case Rightwards:
					 case Leftwards:
					  {						
							for(Row_Iterator = strtr + 1; Row_Iterator < BottomBoundry; Row_Iterator++)
							{
								PixelDiff = Type6JumpThreshold; GotPoint = false;
								int leftBoundry = strtc - Type6_ScanRange, rightBoundry = strtc + Type6_ScanRange;
								if(leftBoundry < LeftBoundry) leftBoundry = LeftBoundry;
								if(rightBoundry > RightBoundry) rightBoundry = RightBoundry;

								for(Column_Iterator = leftBoundry; Column_Iterator < rightBoundry; Column_Iterator++)
								{
									ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
									NextArrayIndex = ArrayIndex + 1;
									double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
									if(TmpValue > PixelDiff && (iBytes[NextArrayIndex] < PixelThreshold || iBytes[ArrayIndex] < PixelThreshold))
									{
										PixelDiff = TmpValue;
										GotPoint = true;
										Point.X = Column_Iterator - 0.5; Point.Y = Row_Iterator;
										strtc = Column_Iterator;
									}
								}
								if(GotPoint)
								{
									Point.pt_index = PtId++;
									PointsList->push_back(Point);
									notfound = 0;
								}		
								else
									notfound++;
								if (notfound > Type6_ScanRange) break;
							}

							strtc = startcol;
							for(Row_Iterator = strtr - 1; Row_Iterator > TopBoundry; Row_Iterator--)
							{
								PixelDiff = Type6JumpThreshold; GotPoint = false;
								int leftBoundry = strtc - Type6_ScanRange, rightBoundry = strtc + Type6_ScanRange;
								if(leftBoundry < LeftBoundry) leftBoundry = LeftBoundry;
								if(rightBoundry > RightBoundry) rightBoundry = RightBoundry;

								for(Column_Iterator = leftBoundry; Column_Iterator < rightBoundry; Column_Iterator++)
								{
									ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
									NextArrayIndex = ArrayIndex + 1;
									double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
									if(TmpValue > PixelDiff && (iBytes[NextArrayIndex] < PixelThreshold || iBytes[ArrayIndex] < PixelThreshold))
									{
										PixelDiff = TmpValue;
										GotPoint = true;
										Point.X = Column_Iterator - 0.5; Point.Y = Row_Iterator;
										strtc = Column_Iterator;
									}
								}
								if(GotPoint)
								{
									Point.pt_index = PtId++;
									PointsList->push_back(Point);
									notfound = 0;
								}		
								else
									notfound++;
								if (notfound > Type6_ScanRange) break;				
							}
					  break;
					  }
				  case Downwards:
				  case Upwards:
					  {
						for(Column_Iterator = strtc + 1; Column_Iterator < RightBoundry; Column_Iterator++)
						{
							PixelDiff = Type6JumpThreshold; GotPoint = false;
							int topBoundry = strtr - Type6_ScanRange, bottomBoundry = strtr + Type6_ScanRange;
							if(topBoundry < TopBoundry) topBoundry = TopBoundry;
							if(bottomBoundry > BottomBoundry) bottomBoundry = BottomBoundry;
							for(Row_Iterator = topBoundry; Row_Iterator < bottomBoundry; Row_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = Column_Iterator + (Row_Iterator + 1) * ImageWidth;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff && (iBytes[NextArrayIndex] < PixelThreshold || iBytes[ArrayIndex] < PixelThreshold))
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator; Point.Y = Row_Iterator - 0.5;
									strtr = Row_Iterator;
								}
							}
							if(GotPoint)
							{
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
								notfound = 0;
							}		
							else
								notfound++;
							if (notfound > Type6_ScanRange) break;
						}

						strtr = startrow;
						for(Column_Iterator = strtc - 1; Column_Iterator > LeftBoundry; Column_Iterator--)
						{
							PixelDiff = Type6JumpThreshold; GotPoint = false;
							int topBoundry = strtr - Type6_ScanRange, bottomBoundry = strtr + Type6_ScanRange;
							if(topBoundry < TopBoundry) topBoundry = TopBoundry;
							if(bottomBoundry > BottomBoundry) bottomBoundry = BottomBoundry;
							for(Row_Iterator = topBoundry; Row_Iterator < bottomBoundry; Row_Iterator++)
							{
								ArrayIndex = Column_Iterator + Row_Iterator * ImageWidth;
								NextArrayIndex = Column_Iterator + (Row_Iterator + 1) * ImageWidth;
								double TmpValue = abs(iBytes[ArrayIndex] - iBytes[NextArrayIndex]);
								if(TmpValue > PixelDiff && (iBytes[NextArrayIndex] < PixelThreshold || iBytes[ArrayIndex] < PixelThreshold))
								{
									PixelDiff = TmpValue;
									GotPoint = true;
									Point.X = Column_Iterator; Point.Y = Row_Iterator - 0.5;
									strtr = Row_Iterator;
								}
							}
							if(GotPoint)
							{
								Point.pt_index = PtId++;
								PointsList->push_back(Point);
								notfound = 0;
							}		
							else
							{
								notfound++;
								strtr++;
							}
							if (notfound > Type6_ScanRange) break;
						}
						break;
				   }
				}
			}
			FilterOutPtsFromList(PointsList, Type6_NoiseFilterDistInPixel);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0055", __FILE__, __FUNCSIG__); }
	}

	//call this function when structure has valid ptindex...
	void EdgeDetectionSingleChannel::FilterOutPtsFromList(std::list<struct Pt> *PointsList, int DistTolerance)
	{
		try
		{
			//NoiseFilterType = LINE_BESTFIT;
			//int PtsCnt = PointsList->size(), Cnt = 0;
			//if(PtsCnt < 3) return;
			//double* PtsArrayForBestFit = new double[PtsCnt * 2];
			//double LnParam[2] = {0}, CircleParam[4] = {0}, LnDist = 0, CirDist = 0;
			//std::list<Pt>::iterator itr, TmpItr;
			//
			//for(itr = PointsList->begin(); itr != PointsList->end(); itr++)
			//{
			//	PtsArrayForBestFit[Cnt++] = (*itr).X;
			//	PtsArrayForBestFit[Cnt++] = (*itr).Y;
			//}
			//if(NoiseFilterType == LINEARC_BESTFIT)
			//{
			//	BFit->Line_BestFit_2D(PtsArrayForBestFit, PtsCnt, LnParam, true);
			//	BFit->Circle_BestFit(PtsArrayForBestFit, PtsCnt, &CircleParam[0], true);

			//	std::list<int> LinePtIdList, CirclePtIdList;
			//	for(itr = PointsList->begin(); itr != PointsList->end(); itr++)
			//	{
			//		double TmpValue[2] = {(*itr).X, (*itr).Y};
			//		//pt to line distance...
			//		Pt2Line_Dist(TmpValue, LnParam[0], LnParam[1], &LnDist);
			//		// Pttopt dist...
			//		CirDist = Pt_Pt_Distance(TmpValue, CircleParam);

			//		if(LnDist > DistTolerance)
			//			LinePtIdList.push_back((*itr).pt_index);
			//		if(abs(CirDist - CircleParam[2]) > DistTolerance)
			//			CirclePtIdList.push_back((*itr).pt_index);
			//	}

			//	//delete noise points ....
			//	itr = PointsList->begin();
			//	bool DeletePt = false;		
			//	std::list<int>::iterator IdItr;
			//	if(CirclePtIdList.size() < LinePtIdList.size())
			//	{
			//		while(itr != PointsList->end())
			//		{
			//			DeletePt = false;
			//			int Id = 0, PtId = (*itr).pt_index;
			//			for (IdItr = CirclePtIdList.begin(); IdItr != CirclePtIdList.end(); IdItr++)
			//			{
			//				if(*IdItr == PtId)
			//				{
			//					DeletePt = true;
			//					TmpItr = itr;
			//					break;
			//				}					
			//			}
			//			itr++;
			//			if(DeletePt)
			//			{
			//				PointsList->erase(TmpItr);	
			//				CirclePtIdList.erase(IdItr);
			//			}
			//		}
			//	}
			//	else
			//	{
			//		while(itr != PointsList->end())
			//		{
			//			DeletePt = false;
			//			int Id = 0, PtId = (*itr).pt_index;
			//			for (IdItr = LinePtIdList.begin(); IdItr != LinePtIdList.end(); IdItr++)
			//			{
			//				if(*IdItr == PtId)
			//				{
			//					DeletePt = true;
			//					TmpItr = itr;
			//					break;
			//				}					
			//			}
			//			itr++;
			//			if(DeletePt)
			//			{
			//				PointsList->erase(TmpItr);	
			//				LinePtIdList.erase(IdItr);
			//			}
			//		}
			//	}
			//}
			//else if(NoiseFilterType == LINE_BESTFIT)
			//{
			//	BFit->Line_BestFit_2D(PtsArrayForBestFit, PtsCnt, LnParam, true);
			//	itr = PointsList->begin();
			//	bool DeletePt = false;
			//	while(itr != PointsList->end())
			//	{
			//		DeletePt = false;
			//		double TmpValue[2] = {(*itr).X, (*itr).Y};
			//		Pt2Line_Dist(TmpValue, LnParam[0], LnParam[1], &LnDist);
			//		if(LnDist > DistTolerance)
			//		{
			//			DeletePt = true;
			//			TmpItr = itr;
			//		}
			//		itr++;
			//		if(DeletePt)
			//			PointsList->erase(TmpItr);	
			//	}
			//}
			//else if(NoiseFilterType == NoiseFilter_BestFitType::ARC_BESTFIT)
			//{
			//	BFit->Circle_BestFit(PtsArrayForBestFit, PtsCnt, &CircleParam[0], true);
			//	itr = PointsList->begin();
			//	bool DeletePt = false;
			//	while(itr != PointsList->end())
			//	{
			//		DeletePt = false;
			//		double TmpValue[2] = {(*itr).X, (*itr).Y};
			//		CirDist = Pt_Pt_Distance(TmpValue, CircleParam);
			//		if(abs(CirDist - CircleParam[2]) > DistTolerance)
			//		{
			//			DeletePt = true;
			//			TmpItr = itr;
			//		}
			//		itr++;
			//		if(DeletePt)
			//			PointsList->erase(TmpItr);	
			//	}
			//}						
			//delete [] PtsArrayForBestFit;

			
			NoiseFilterType = LINE_BESTFIT;
			int PointsCnt = PointsList->size(), Cnt = 0;
			if(PointsCnt < 3) return;
			double* PtsArrayForBestFit = new double[PointsCnt * 2];
			
			for(std::list<Pt>::iterator itr = PointsList->begin(); itr != PointsList->end(); itr++)
			{
				PtsArrayForBestFit[Cnt++] = (*itr).X;
				PtsArrayForBestFit[Cnt++] = (*itr).Y;
			}
			if(NoiseFilterType == LINE_BESTFIT)
			{
				double LnParam[2] = {0}, LnDist = 0;
				std::list<Pt>::iterator itr, TmpItr;
				BFit->Line_BestFit_2D(PtsArrayForBestFit, PointsCnt, LnParam, true);
				itr = PointsList->begin();
				bool DeletePt = false;
				while(itr != PointsList->end())
				{
					DeletePt = false;
					double TmpValue[2] = {(*itr).X, (*itr).Y};
					Pt2Line_Dist(TmpValue, LnParam[0], LnParam[1], &LnDist);
					if(LnDist > DistTolerance)
					{
						DeletePt = true;
						TmpItr = itr;
					}
					itr++;
					if(DeletePt)
						PointsList->erase(TmpItr);	
				}
			}						
			delete [] PtsArrayForBestFit;

			Type6SubAlgo_NoiseFilterDistInPixel = 3;
			/*if(Type6SubAlgo_NoiseFilterDistInPixel < 2 || Type6SubAlgo_NoiseFilterDistInPixel > 100)
				Type6SubAlgo_NoiseFilterDistInPixel = 5;*/
			int BestFitPtsCount = 30;
			//if any body wants to change BestFitPtsCount value then he will have to change size of PtsArrayForBestFit array, please be carefull...
			if(PointsList->size() < 3) return;
			if(NoiseFilterType == LINE_BESTFIT)
			{
				int FilterCount = 1;
				if(PointsList->size() > BestFitPtsCount)
					FilterCount = PointsList->size() / BestFitPtsCount;
				std::list<Pt>::iterator itr = PointsList->begin(), itr1, TmpItr;
				double PtsArrayForBestFit[120] = {0};
				for(int i = 0; i < FilterCount; i++)
				{
					itr1 = itr;
					int TmpCount = 0, PtsCnt = 0;
					double LnParam[2] = {0}, LnDist = 0;
					int MaximumPtsCnt = BestFitPtsCount;
					if(i == FilterCount - 1)
						MaximumPtsCnt *= 2;
					while(itr != PointsList->end() && TmpCount < MaximumPtsCnt)
					{
						TmpCount++;
						PtsArrayForBestFit[PtsCnt++] = (*itr).X;
						PtsArrayForBestFit[PtsCnt++] = (*itr).Y;
						itr++;
					}
					if(PtsCnt < 4) break;
					BFit->Line_BestFit_2D(PtsArrayForBestFit, PtsCnt / 2, LnParam, true);
					bool DeletePt = false;
					while(itr1 != PointsList->end() && itr1 != itr)
					{
						DeletePt = false;
						double TmpValue[2] = {(*itr1).X, (*itr1).Y};
						Pt2Line_Dist(TmpValue, LnParam[0], LnParam[1], &LnDist);
						if(LnDist > Type6SubAlgo_NoiseFilterDistInPixel)
						{
							DeletePt = true;
							TmpItr = itr1;
						}
						itr1++;
						if(DeletePt)
							PointsList->erase(TmpItr);	
					}
				}		
			}
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0036", __FILE__, __FUNCSIG__); }
	}

	double EdgeDetectionSingleChannel::Pt_Pt_Distance(double* Pt1, double* Pt2)
	{
		try
		{
			double dis = 0;
			for(int i = 0; i < 2; i ++)
				dis += pow((Pt1[i] - Pt2[i]), 2);  
			return sqrt(dis);
		}
		catch(...){ SetAndRaiseErrorMessage("EDSC0036", __FILE__, __FUNCSIG__); return 0; }
	}

	void EdgeDetectionSingleChannel::ImageAnalysis(int *Box, int masksize, int filterCnt, int PixelThreshold, std::list<double> *ptList)
	{
		 std::list<struct Pt> ArrangedPointsList;
		 struct Pt Point;
		 int LeftBoundry = Box[0], RightBoundry = Box[1], TopBoundry = Box[2], BottomBoundry = Box[3], width = (RightBoundry -  LeftBoundry);
		 double *TempByteArray = new double[ImageWidth * ImageHeight];
		 memcpy(TempByteArray, iBytes, ImageWidth * ImageHeight * sizeof(double));
		 int limit = masksize / 2;
		 double *imagePixel = new double[masksize * masksize], ans = 0;
		 memset(imagePixel, 0, sizeof(double) * masksize * masksize);
		 int s1 = masksize * masksize - 1, cnt = 0;
		 for(int i = 0; i < filterCnt; i++)
		 {		 
			for (int i = TopBoundry; i < BottomBoundry; i++)
			{
				for (int j = LeftBoundry; j < RightBoundry; j++)
				{
					cnt = 0;
					for (int k = -limit; k <= limit; k++)
					{
						for (int l = -limit; l <= limit; l++)
						{
							 if (k == 0 && l == 0) continue;
							 imagePixel[cnt++] = iBytes[(i + k) * 800 + (j + l)];
						}
					}
					ans =  MedianSort(imagePixel, s1);
					TempByteArray[i * 800 + j] = ans;
				}
			} 
		}
		double gKernel1[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 }, val1 = 0, val2 = 0, val3 = 0;
		double gKernel2[9] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
		s1 = 9;
		for (int i = TopBoundry; i < BottomBoundry; i++)
		{
			for (int j = LeftBoundry; j < RightBoundry; j++)
			{
				cnt = 0;
				for (int k = -1; k <= 1; k++)
				{
					for (int l = -1; l <= 1; l++)
					{
					   imagePixel[cnt++] = TempByteArray[(i + k) * 800 + (j + l)];
					}
				}
				val1 = floor(MultiplyMatrix(imagePixel, s1, gKernel1));
				val2 = floor(MultiplyMatrix(imagePixel, s1, gKernel2));
				val3 = floor(sqrt(val1 * val1 + val2 * val2));
				if(val3 > PixelThreshold)
					iBytes[i * 800 + j] = val3;
				else
					iBytes[i * 800 + j] = 0;
					
			}
		}
		delete [] imagePixel;
		delete [] TempByteArray;
		int size = 14 , PtId = 0;
		for(int i = TopBoundry + 10; i < BottomBoundry - size; i += size)
		{
			for(int j = LeftBoundry + 10; j < RightBoundry - size; j += size)
			{
				bool found = false;
				for(int k = i; k < i + size; k++)
				{
					for(int l = j; l < j + size; l++)
					{
						val1 = iBytes[k * 800 + l];
						val2 = iBytes[k * 800 + l - 1];
						if(val2 == 0 && val1 > 0)
						{
							Point.X = l; Point.Y = k;
							Point.pt_index = PtId++;
							ArrangedPointsList.push_back(Point);	
							/*ptList->push_back(l);
							ptList->push_back(k);*/
							found = true;
							break;
						}				
					}
					if(found) break;
				}	
				if(found) continue;
				for(int k = j; k < j + size; k++)
				{
					 for(int l = i; l < i + size; l++)
					 {
						  val1 = iBytes[l * 800 + k];
						  val2 = iBytes[(l - 1) * 800 + k];
						  if(val2 == 0 && val1 > 0)
						  {
							 Point.X = k; Point.Y = l;
							 Point.pt_index = PtId++;
							 ArrangedPointsList.push_back(Point);	
							/* ptList->push_back(k);
							 ptList->push_back(l);*/
							 found = true;
							 break;
						  }
					}
					if(found) break;
				}
			}
		}	
		ArrangeAllPoints2(&ArrangedPointsList);
		for each(Pt Pnt in ArrangedPointsList)
		{
			ptList->push_back(Pnt.X);
			ptList->push_back(Pnt.Y);
		}
	}

	double EdgeDetectionSingleChannel::MultiplyMatrix(double *M1, int S1, double *M2)
	{
		double answer = 0;
		for(int i = 0; i < S1; i++)
		{	
			answer += M1[i] * M2[i];
		}                
		return answer;
	}


	double EdgeDetectionSingleChannel::MedianSort(double *point, int size)
	{
		for(int j = 0; j < size; j++)
		{
			for(int k = 0; k < (size-1); k++)
			{
				if(point[k] > point[k+1])
				{
					double temp = point[k + 1];
					point[k + 1] = point[k];
					point[k] = temp;
				}
			}
		} 
		if(size % 2 == 0)
			return (point[size/2] + point[size/2 + 1])/2;
		else
			return (point[size/2 + 1]);
	}

	void EdgeDetectionSingleChannel::GetRadialPts(double X, double Y, int gap, int* Ans)
	{
		double PolarXY[2]; 
		GetPolarCoordinates(X,Y, &PolarXY[0]);
		//The first two locations will have point coordinates nearest to the centre of beam
		PolarXY[0] -= gap;
		Ans[0] = (int) (PolarXY[0] * cos(PolarXY[1]) + bd_factor[0]);
		Ans[1] = (int) (bd_factor[1] - PolarXY[0] * sin(PolarXY[1]));
		PolarXY[0] += 2 * gap;
		Ans[2] = (int) (PolarXY[0] * cos(PolarXY[1])+ bd_factor[0]);
		Ans[3] = (int) (bd_factor[1] - PolarXY[0] * sin(PolarXY[1]));
	}

	void EdgeDetectionSingleChannel::GetPolarCoordinates(double X, double Y, double *Ans)
	{
		double dist = 0;
		double angleofPt = 0;
		dist = sqrt(pow((X - bd_factor[0]), 2) + pow((Y - bd_factor[1]), 2)); // - ; //164.753;
		if (X == bd_factor[0])
		{	
			angleofPt = M_PI / 2;
			if (Y > bd_factor[1]) angleofPt *= -1;						
		}
		else
		{	
			angleofPt = atan((bd_factor[1] - Y)/(X - bd_factor[0]));
			if (angleofPt < 0)
			{	
				if (Y > bd_factor[1]) 
					angleofPt += 2 * M_PI;
				else
					angleofPt += M_PI;
			}
			else
				if (Y > bd_factor[1]) angleofPt += M_PI;
		}
		Ans[0] = dist;
		Ans[1] = angleofPt;
	}

	void EdgeDetectionSingleChannel::ConvertTo3bpp(BYTE* PixelArray)
	{
		int totalPixelCount = ImageHeight * ImageWidth; 
		int j = 0;
		for (int i = 0; i < totalPixelCount; i++)
		{
			PixelArray[j++] = (BYTE)((int)iBytes[i]);
			PixelArray[j++] = (BYTE)this->iBytes[i];
			PixelArray[j++] = (BYTE)this->iBytes[i];
		}
		
	}

	bool EdgeDetectionSingleChannel::CheckEdgePointQuality(Pt* EdgePt)
	{
		int x = (int)EdgePt->X;
		int y = (int)EdgePt->Y;
		int MaskSize = 5;
		int startJ = y - MaskSize;
		int endJ = y + MaskSize;
		if (startJ < 0) startJ = 0;
		if (endJ > ImageHeight - 1) endJ = ImageHeight - 1;

		int starti = x - MaskSize;
		int endi = x + MaskSize;
		if (starti < 0) starti = 0;
		if (endi > ImageWidth - 1) endi = ImageWidth - 1;
		int WhiteCt = 0;
		int pos = 0;
		int pixelsCt = 0;
		for (int j = startJ; j < endJ; j++)
		{
			pos = j * ImageWidth;
			for (int i = starti; i < endi; i++)
			{
				pixelsCt++;
				if (iBytes[i + pos] > 170) WhiteCt++;
			}
		}
		if ((double)WhiteCt / (double)pixelsCt < 0.4) return false;

		return true;
	}
