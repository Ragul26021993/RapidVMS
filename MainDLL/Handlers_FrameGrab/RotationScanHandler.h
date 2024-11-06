#pragma once
#include "..\Handlers\MouseHandler.h"

class RotationScanHandler :
	public MouseHandler
{
private:
	bool StartScanning, PauseCurrentProgram;
	int SeletionFrames_Width, ScanDirection, FixedHeight;
	double CircleParam_Pixel[3];
	enum SELECTIONSHAPETYPE
	{
		ANGULARRECTANGLE,
		ARC,
		RECTANGLE,
		CIRCULAR,
		FIXEDRECTANGLE,
	}SelectionShapeType;
	Vector PointsDRO[3];
	Vector PointsPixel[3];

	enum SCANNINGTYPE
	{
		XAXIS_UPPEREDGE,
		XAXIS_LOWEREDGE,
		YAXIS_UPPEREDGE,
		YAXIS_LOWEREDGE,
	}ScanningEdgeAxis;

	struct PtsStruct
	{
		int Index, Rvalue, X, Y;
	};
	std::map<int, PtsStruct> PtsSructureList;
	PointCollection LastFramePointCollection;
	double slope;
	bool SavePointsFlag;
	double PixelCoordinateArray[20000];
	bool UpdatePointsMapList(int NoOfPoints);
	bool SetScanningTypeEnum();
	void AddPointsToSelectedShape(bool AddAction = false);
	friend void ContinuousScanIndifferentThread(void *anything);
	void UpdateFramGrabProperties();
	void UpdatePixelCoordinate(int NoOfPoints);
	void StoppedScanning();
	void RotationScanPoints();
public:
	bool DrawFramGrab;

	RotationScanHandler();
	RotationScanHandler(RapidEnums::RAPIDHANDLERTYPE HandlerType);
	~RotationScanHandler();

	//Virtual functions..//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void LmouseUp(double x,double y);
	virtual void RmouseDown(double x, double y);
	virtual void MouseScroll(bool flag);
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	void StartRotationalScanning(bool ScanStart);
	void CotninuePauseRotationalScanning(bool Pauseflag);
	void UsePreviousFramGrabForScanning();
};