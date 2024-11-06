//AddPoint Handler Class... 
//Inherited from Mouse Handler.. Used to add Points to selected shape using Flexible crosshair..
#pragma once
#include "..\Handlers\MouseHandler.h"

class SelectPointHandler:
	public MouseHandler
{
private:
	bool runningPartprogramValid; //Used in Partprogram
	void init(); //Initialise default settings..
	void AddPointsToSelectedShape(bool runningpp);
	
	friend void ScaninSeparateThread(void *anything);

public:
	SelectPointHandler(); //default constructor..
	SelectPointHandler(FramegrabBase *fb); //Used for already  existing point action .. in Partprogram!
	~SelectPointHandler(); //destructor... clear the memory related..!
	bool ProbeisLoaded, StartScanning;

	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void EscapebuttonPress();
	virtual void PartProgramData();

	void StartContinuousScanning(bool ScanStart);
	void AddScanPointstoShape();
	void StopScanning();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!