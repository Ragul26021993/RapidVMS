#pragma once
#include "RAction.h"

class Shape;
class MAINDLL_API ProfileScanAction:
	public RAction
{
private:
	bool CompleteProfile, SurfaceON;
	int SkipCount, MaskFactor, Binary_LowerFactor, Binary_UpperFactor, PixelThreshold, MaskingLevel;
	Shape* Cshape;
	Vector FirstPt, SecondPt, CurrentPos;
	Vector* EndPt;
	std::string CamProperty, LightProperty;
public:	
	bool PausableAction;
	ProfileScanAction();
	virtual ~ProfileScanAction();

	//Add and remove the point action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	Vector* GetFirstPt();
	Vector* GetSecondPt();
	Vector* GetEndPt();
	Vector* GetCurrentPosition();
	Shape* getShape();
	bool GetSurfaceLifghtInfo();
	bool GetSurfaceAlgoParam(int *Param);
	int GetPixelCount();
	bool GetProfileInfo();
	string GetCam_Property();
	char* GetLight_Property();
	void SetFirstPt(Vector& pt);
	void SetSecondPt(Vector& pt);
	void SetCurrentPos(Vector& pt);
	void Translate(Vector pt1);
	void Transform(double* Tmatrix);
	static void AddProfileScanAction(Shape* Shp, Vector Pt1, Vector Pt2, Vector* lastPt, Vector currentPosition, bool complete_profile, int PixelSkip, std::string lightsetting, std::string CamSetting, int* SurfaceAlgoProp, bool SurfaceLight);

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, ProfileScanAction&);
	friend wistream& operator>>(wistream&, ProfileScanAction&);
};