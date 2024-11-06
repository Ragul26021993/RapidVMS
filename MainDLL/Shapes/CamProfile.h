
#pragma once
#include "shapewithlist.h"

class MAINDLL_API CamProfile:
	public ShapeWithList
{
private:
	static int shapenumber;
	TCHAR* genName(const TCHAR* prefix);
	void init();	
	double CamParameter[4];
	double MinRadius;
	ShapeWithList *OuterCircle;
	ShapeWithList *RayLine;
	ShapeWithList *RPoint3dForZposition;

	RapidProperty<bool> IdCamProfile;
	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void CheckIdorOdCamProfile();
	void updateCamProfile_UsingPorbe();
	bool applyDelphiCamProbeCorrection(double* pt_in, double* pt_out, double probeRadius, bool inner = true);

public:
	CamProfile(TCHAR* myname = _T("DC"));
	CamProfile(bool simply);
	~CamProfile();

	void SetMinRadius(double Radius);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void Translate(Vector& Position);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);
	static void reset();
	static void Set_DeiphiCam_Circle_Centre(double x_coord, double y_coord, double z_coord, double ray_angle, double ray_intercept, double DCCRad);		//vinod
	static void Set_DeiphiCam_parameters(double DeiphiCam_Angle, double DeiphiCam_Lower_tol, double DeiphiCam_Upper_tol, bool ToolType, int PtstoBundle, double rayx1, double rayy1, double rayx2, double rayy2, double MinRad, double MaxRad, char* CurDate, char* CurTime);		//vinod
	void GetCamProfileParam(double *Parameter);
};	
