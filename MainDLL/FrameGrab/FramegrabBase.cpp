//Include Required header files...
#pragma once
#include "stdafx.h"
#include "FramegrabBase.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"
#include "..\Handlers_FrameGrab\FrameGrabHandler.h"

//Default Constructor..
FramegrabBase::FramegrabBase():BaseItem(_T("FG"))
{
	try
	{
		init();
		fgNumber++;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0001", __FILE__, __FUNCSIG__); }
}

//Constructor with type of fg...//
FramegrabBase::FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE fg):BaseItem(genName(_T("FG")))
{
	try
	{
		FGtype = fg; 
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0002", __FILE__, __FUNCSIG__); }
}


FramegrabBase::FramegrabBase(FramegrabBase* OriginalFb):BaseItem(genName(_T("FG")))   // copy constructor dont-intialize any variable here... use init
{
	try
	{
		init();
		fgNumber++;
		FGWidth = OriginalFb->FGWidth; noofpts = 0; noofptstaken_build = 0; ChannelNo = OriginalFb->ChannelNo;
		MaskFactor = OriginalFb->MaskFactor; Binary_LowerFactor = OriginalFb->Binary_LowerFactor; Binary_UpperFactor = OriginalFb->Binary_UpperFactor; PixelThreshold = OriginalFb->PixelThreshold; MaskingLevel = OriginalFb->MaskingLevel;
		camProperty = OriginalFb->camProperty; lightProperty = OriginalFb->lightProperty;
		magLevel = OriginalFb->magLevel;
		ProfileON = OriginalFb->ProfileON; SurfaceON = OriginalFb->SurfaceON; UseDigitalZoom = OriginalFb->UseDigitalZoom;
		FGdirection = OriginalFb->FGdirection; CucsId = OriginalFb->CucsId;
		ProbeDirection = OriginalFb->ProbeDirection;
		ProbePosition = OriginalFb->ProbePosition;
		FGtype = OriginalFb->FGtype;
		CurrentWindowNo = OriginalFb->CurrentWindowNo; Surfacre_FrameGrabtype = OriginalFb->Surfacre_FrameGrabtype;
		Pointer_SnapPt = OriginalFb->Pointer_SnapPt;
		DerivedShape = OriginalFb->DerivedShape; ReferenceAction(false); DroMovementFlag = OriginalFb->DroMovementFlag;
		ActionForTwoStepHoming(false); ActionFinished(false);
		PointDRO = OriginalFb->PointDRO;
		for(int i = 0; i < 3; i++)
			points[i] = OriginalFb->points[i];
		for(int i = 0; i < 5; i++)
			myPosition[i] = OriginalFb->myPosition[i];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0002a", __FILE__, __FUNCSIG__); }
}

void FramegrabBase::init()
{
	try
	{
		UseDigitalZoom = false;
		FGWidth = 100; noofpts = 0; noofptstaken_build = 0; ChannelNo = 1; MaskFactor = 5; Binary_LowerFactor = 40; Binary_UpperFactor = 200; PixelThreshold = 40; MaskingLevel = 1;
		camProperty = "58FF0080010003"; lightProperty = "0000000000000000";
		magLevel = "67X";
		FGdirection = 0; CucsId = 0;
		ProbeDirection = RapidEnums::PROBEDIRECTION::XLEFT2RIGHT;
		ProbePosition = RapidEnums::PROBEPOSITION::LEFT;
		CurrentWindowNo = 0; Surfacre_FrameGrabtype = 0;
		Pointer_SnapPt = NULL;
		DerivedShape = false; ReferenceAction(false); DroMovementFlag = true;
		ActionForTwoStepHoming(false); ActionFinished(false);
		PartprogrmActionPassStatus(false);
		isLineArcFirstPtAction(false);
		DontCheckProjectionType(false);
		Auto_FG_AssistedMode = MAINDllOBJECT->Force_AutoFG_InAssistedMode();
		//for (int i = 0; i < 4; i++)
		//	CG_Params[i] = -1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0002a", __FILE__, __FUNCSIG__); }
}

//Destructor .. Clear the memory..
FramegrabBase::~FramegrabBase()
{
	PointActionIdList.clear();
}

//Fg base name...//
TCHAR* FramegrabBase::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		fgNumber++;
		TCHAR shapenumstr[10];
		_itot_s(fgNumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0003", __FILE__, __FUNCSIG__); return name; }
}

//Get the camera property.,..//
char* FramegrabBase::getCamProperties()
{
	return (char*)camProperty.c_str();
}

//Get the current light intensity..//
char* FramegrabBase::getLightIntensity()
{
	return (char*)lightProperty.c_str();
}

std::string FramegrabBase::getCurrentCrosshairTypeName()
{
	try
	{
		std::string PPstepActn = "Default";
		if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE)
			PPstepActn = "Normal_CH";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
			PPstepActn = "Flexible_CH";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB)
			PPstepActn = "ARC_Framegrab";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB)
			PPstepActn = "CIRCLE_Framegrab";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB)
			PPstepActn = "RECT_Framegrab";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES)
			PPstepActn = "RECT_Framegrab";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
			PPstepActn = "ARECT_Framegrab";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE)
			PPstepActn = "FREXT_Framegrab";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK)
			PPstepActn = "CLICK_Framegrab";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
			PPstepActn = "Probe_Point";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH)
			PPstepActn = "Focus_Depth";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO)
			PPstepActn = "Focus_Depth";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN)
			PPstepActn = "Focus_Scan";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN)
			PPstepActn = "Contour_Scan";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX)
			PPstepActn = "Focus_Depth_2bx";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
			PPstepActn = "Focus_Depth_4bx";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::SCANCROSSHAIR)
			PPstepActn = "ScanMode";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::LIVESCANCROSSHAIR)
			PPstepActn = "LiveScanMode";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::SELECTPOINTCROSSHAIR)
			PPstepActn = "SelectPoint";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::IDMEASUREMENT)
			PPstepActn = "IdMeasure";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ODMEASUREMENT)
			PPstepActn = "OdMeasure";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::DETECT_ALLEDGES)
			PPstepActn = "AllEdges";
		else if(this->FGtype == RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG)
			PPstepActn = "ContourFrameGrab";
	
		return ((char*)PPstepActn.c_str());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0004", __FILE__, __FUNCSIG__); return ""; }
}



void FramegrabBase::TrasformthePosition(double *transform, int ucsId)
{
	try
	{
		Vector Temp;
		UCS* CurrentUCS = MAINDllOBJECT->getUCS(ucsId);
		if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			Temp = MAINDllOBJECT->TransformMultiply(transform, PointDRO.getX(), PointDRO.getY(), PointDRO.getZ());
			PointDRO.set(Temp.getX(), Temp.getY(), PointDRO.getZ()); // Temp.getZ());
		}
		double upp = CurrentUCS->getWindow(0).getUppX();
		double cx = 0, cy = 0;
		double x1, y1;

		if(FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN || 
			FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
			return;

		R_Point icentre = MAINDllOBJECT->getWindow(0).getWinDim();

		if (ucsId == 0)
		{
			
			cx = PointDRO.getX(); // - CurrentUCS->UCSPoint.getX() - CurrentUCS->getWindow(0).getViewDim().x / 2;
			cy = PointDRO.getY(); //- CurrentUCS->UCSPoint.getY() + CurrentUCS->getWindow(0).getViewDim().y / 2;

			for (int i = 0; i < 3; i++)
			{
				Temp = MAINDllOBJECT->TransformMultiply(transform, points[i].getX(), points[i].getY(), points[i].getZ()); //1); //
				points[i].set(Temp.getX(), Temp.getY(), Temp.getZ());
				if (FGtype == RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG) return;
				//x1 = (points[i].getX() - cx) / upp + icentre.x / 2; y1 = (cy - points[i].getY()) / upp + icentre.y / 2;
				//myPosition[i].set(x1, y1);
			}
		}
		else
		{
			cx = PointDRO.getX(); //- CurrentUCS->UCSPoint.getX(), // - CurrentUCS->getWindow(0).getViewDim().x / 2,
			cy = PointDRO.getY(); //- CurrentUCS->UCSPoint.getY();// + CurrentUCS->getWindow(0).getViewDim().y / 2;

			//double dx = 0, dy = 0, x = 0, y = 0, radius = 0, theta = 0;
			//radius = sqrt(cx * cx + cy * cy);
			double	gP[3] = { PointDRO.getX(), PointDRO.getY(), 0}, 
					cP[3] = { CurrentUCS->UCSPoint.getX(), CurrentUCS->UCSPoint.getY(), CurrentUCS->UCSPoint.getZ() },
					ans[3] = { 0,0,0 };
			//RMATH2DOBJECT->Rotate_2D_Pt(gP, cP, CurrentUCS->UCSangle(), ans);
			//cx = ans[0]; cy = ans[1];
			
			for (int i = 0; i < 3; i++)
			{
				gP[0] = points[i].getX(); gP[1] = points[i].getY(); gP[2] = points[i].getZ();
				//cP[0] = icentre.x * HELPEROBJECT->CamSizeRatio / 2; cP[1] = icentre.y * HELPEROBJECT->CamSizeRatio / 2; cP[2] = 0;
				cP[0] = CurrentUCS->UCSPt_B2S.getX(); 
				cP[1] = CurrentUCS->UCSPt_B2S.getY(); cP[2] = 0;
				
				RMATH2DOBJECT->UnRotate_2D_Pt(gP, cP, CurrentUCS->UCSAngle_B2S, ans);
				Temp = MAINDllOBJECT->TransformMultiply(transform, ans[0], ans[1], points[i].getZ());

				points[i].set(Temp);

				myPosition[i].set((points[i].getX() - cx) / upp + icentre.x / 2, icentre.y / 2 - (points[i].getY() - cy) / upp);


				//myPosition[i].set(ans[0], ans[1]);

				//points[i].set(PointDRO.getX() + (myPosition[i].getX() - icentre.x * HELPEROBJECT->CamSizeRatio / 2) * upp, 
				//			  PointDRO.getY() + (icentre.y * HELPEROBJECT->CamSizeRatio / 2 - myPosition[i].getY()) * upp, 
				//			  Temp.getZ());
				//dx = (myPosition[i].getX() - icentre.x * HELPEROBJECT->CamSizeRatio / 2) * upp;
				//dy = (icentre.y * HELPEROBJECT->CamSizeRatio / 2 - myPosition[i].getY()) * upp;
				//radius = sqrt(dx * dx + dy * dy);
				//theta = 0;
				//if (dx == 0)
				//{
				//	if (dy == 0)
				//		theta = 0;
				//	else if (dy > 0)
				//		theta = M_PI_2;
				//	else
				//		theta = 3 * M_PI_2;
				//}
				//else
				//{
				//	theta = atan(dy / dx);
				//	if (dx < 0) theta += M_PI;
				//	if (dy < 0 && dx > 0) theta += 2 * M_PI;
				//}

				//x = cx + radius * cos(theta - CurrentUCS->UCSangle());
				//y = cy + radius * sin(theta - CurrentUCS->UCSangle());
				//points[i].set(x, y, Temp.getZ());
				//dx /= upp; dy /= upp; radius /= upp * HELPEROBJECT->CamSizeRatio;
				//x = icentre.x + radius * cos(theta - CurrentUCS->UCSangle());
				//y = icentre.y + radius * sin(theta - CurrentUCS->UCSangle());
				//myPosition[i].set(x, y);
			}
		}
		//PointDRO.set(Temp.getX(), Temp.getY(), Temp.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0006", __FILE__, __FUNCSIG__); }
}

void FramegrabBase::TrasformthePosition_PlaneAlign(double *transform, int ucsId)
{
	try
	{
		Vector Temp;
		if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT && 
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			Temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(transform, PointDRO.getX(), PointDRO.getY(), PointDRO.getZ());
			PointDRO.set(Temp.getX(), Temp.getY(), Temp.getZ());
		}

		UCS* CurrentUCS = MAINDllOBJECT->getUCS(ucsId);
		double upp = CurrentUCS->getWindow(0).getUppX(), cx = PointDRO.getX() - CurrentUCS->UCSPoint.getX() - CurrentUCS->getWindow(0).getViewDim().x/2, cy = PointDRO.getY() - CurrentUCS->UCSPoint.getY() + CurrentUCS->getWindow(0).getViewDim().y/2;
		double x1, y1;
	
		for(int i = 0; i < 3; i++)
		{
			Temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(transform, points[i].getX(), points[i].getY(), points[i].getZ());
			points[i].set(Temp.getX(), Temp.getY(), Temp.getZ());
			x1 = (points[i].getX() - cx)/upp; y1 = (cy - points[i].getY())/upp;
			myPosition[i].set(x1, y1);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0006", __FILE__, __FUNCSIG__); }
}

void FramegrabBase::TranslatePosition(Vector& diff, int ucsId)
{
	try
	{
		PointDRO += diff;
		UCS* CurrentUCS = MAINDllOBJECT->getUCS(ucsId);
		double upp = CurrentUCS->getWindow(0).getUppX(),
			cx = PointDRO.getX(), // - CurrentUCS->UCSPoint.getX() - CurrentUCS->getWindow(0).getViewDim().x / 2,
			cy = PointDRO.getY(); // - CurrentUCS->UCSPoint.getY() + CurrentUCS->getWindow(0).getViewDim().y / 2;
		double x1, y1;
		R_Point icentre = MAINDllOBJECT->getWindow(0).getWinDim();
		for (int i = 0; i < 3; i++)
		{
			if (ucsId == 0)
			{
				points[i] += diff;
				if (!(FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN ||
					FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX))
				{
					x1 = (points[i].getX() - cx) / upp + icentre.x / 2; y1 = (cy - points[i].getY()) / upp + icentre.y / 2;
					myPosition[i].set(x1, y1);
				}
			}
			else
			{
				x1 = (myPosition[i].getX() - icentre.x / 2) * upp + cx; y1 = (icentre.y / 2 - myPosition[i].getY()) * upp + cy;
				int s1[2] = { 3, 3 }, s2[2] = { 3, 1 };
				double p[3] = { x1, y1, 1 }, ans[3] = { 0 };
				RMATH2DOBJECT->MultiplyMatrix1(&CurrentUCS->transform[0], &s1[0], &p[0], &s2[0], &ans[0]);

				//Vector temp = MAINDllOBJECT->Trans(CurrentUCS->transform, x1, y1, PointDRO.getZ());
				points[i].set(ans[0], ans[1], ans[2]);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0006", __FILE__, __FUNCSIG__); }
}


void FramegrabBase::Transform_2S_Homing(double* transform, int ucsId)
{
	try
	{
		Vector Temp;
		double x, y;
		UCS* CurrentUCS = MAINDllOBJECT->getUCS(ucsId);
		if (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT &&
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT &&
			MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			//Temp = MAINDllOBJECT->TransformMultiply(transform, PointDRO.getX(), PointDRO.getY(), PointDRO.getZ());
			x = transform[0] * (PointDRO.getX() - transform[2]) + transform[1] * (PointDRO.getY() - transform[5]);
			y = transform[3] * (PointDRO.getX() - transform[2]) + transform[4] * (PointDRO.getY() - transform[5]);
			PointDRO.set(x, y, PointDRO.getZ());
		}
			//PointDRO.set(Temp.getX(), Temp.getY(), Temp.getZ());
		double upp = CurrentUCS->getWindow(0).getUppX();
		double cx = 0, cy = 0;
		double x1, y1;

		if (FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN ||
			FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX || FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
			return;

		if (ucsId == 0)
		{
			cx = PointDRO.getX(); // - CurrentUCS->UCSPoint.getX() - CurrentUCS->getWindow(0).getViewDim().x / 2;
			cy = PointDRO.getY(); // - CurrentUCS->UCSPoint.getY() + CurrentUCS->getWindow(0).getViewDim().y / 2;
			for (int i = 0; i < 3; i++)
			{
				x = transform[0] * (points[i].getX() - transform[2]) + transform[1] * (points[i].getY() - transform[5]);
				y = transform[3] * (points[i].getX() - transform[2]) + transform[4] * (points[i].getY() - transform[5]);
				points[i].set(x, y, points[i].getZ());
				if (FGtype == RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG) return;
				x1 = (x - cx) / upp; y1 = (cy - y) / upp;
				myPosition[i].set(x1, y1);
			}
		}
	}
	catch(exception exx)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0006a", __FILE__, __FUNCSIG__);
	}
}



//Initialise static variables.. For the action Name..
int FramegrabBase::fgNumber = 0;
TCHAR FramegrabBase::name[10];
void FramegrabBase::reset()
{
	fgNumber = 0;
}

//write to partprogram file.////
wostream& operator<<(wostream& os, FramegrabBase& fg)
{
	try
	{
		os << "FramgrabBase" << endl;
		os << "Id:" << fg.getId() << endl;
		os << "OriginalName:" << fg.getOriginalName() << endl;
		os << "noofpts:" << fg.noofpts << endl;
		os << "ChannelNo:" << fg.ChannelNo << endl;
		os << "FGdirection:" << fg.FGdirection << endl;			
		os << "Surfacre_FrameGrabtype:" << fg.Surfacre_FrameGrabtype << endl;
		os << "ReferenceAction:" << fg.ReferenceAction() << endl;
		os << "ActionForTwoStepHoming:" << fg.ActionForTwoStepHoming() << endl;				
		os << "PointDRO:values" << endl << (static_cast<Vector>(fg.PointDRO)) << endl;
		os << "FGtype:" << fg.FGtype << endl;
		os << "ProbeDirection:" << fg.ProbeDirection << endl;
		os << "ProbePosition:" << fg.ProbePosition << endl;
		os << "FGWidth:" << fg.FGWidth << endl;
		os << "CamProperties:" << fg.getCamProperties() << endl;
		os << "LightIntensity:" << fg.getLightIntensity() << endl;
		os << "MagLevel:" << fg.magLevel.c_str() << endl;
		os << "ProfileON:" << fg.ProfileON << endl;
		os << "SurfaceON:" << fg.SurfaceON << endl;
		if(fg.SurfaceON)
		{
			for each(SurfaceED_Params SED in fg.SEDP_List)
			{
				os << "MaskFactor:" << SED.MaskFactor << endl;
				os << "Binary_LowerFactor:" << SED.Binary_Lower << endl;
				os << "Binary_UpperFactor:" << SED.Binary_Upper << endl;
				os << "PixelThreshold:" << SED.Threshold << endl;
				os << "MaskingLevel:" << SED.MaskingLevel << endl;
				os << "AverageFilter:" << SED.FilterByAverage << endl;
				os << "MaskingLevel:" << SED.MaskingLevel << endl;
			}
		}
		os << "DroMovementFlag:" << fg.DroMovementFlag << endl;	
		os << "DerivedShape:" << fg.DerivedShape << endl;	
		os << "UseDigitalZoom:" << fg.UseDigitalZoom << endl;
		//if (fg.CG_Params[0] != -1 && fg.CG_Params[1] != -1 && fg.CG_Params[2] != -1)
		//	os << "CG_Params:;" << fg.CG_Params[0] << ";" << fg.CG_Params[1] << ";" << fg.CG_Params[2] << ";" << fg.CG_Params[3] << ";" << endl;

		os << "CurrentWindowNo:" << fg.CurrentWindowNo << endl;

		if(fg.CurrentWindowNo == 1 && fg.DerivedShape)
			os << "Pointer_SnapPt:values" << endl << (static_cast<Vector>(*fg.Pointer_SnapPt)) << endl;		
		os << "FgPoints:values" << endl;
		for(int i = 0; i < 3; i++)
			os << (static_cast<Vector>(fg.points[i])) << endl;
		os << "FgPosition:values" << endl ;
		for(int i = 0; i < 5; i++)
			os << (static_cast<Vector>(fg.myPosition[i])) << endl;
		os << "Auto_FG_AssistedMode:" << fg.Auto_FG_AssistedMode << endl;
		os << "EndFramgrabBase" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0007", __FILE__, __FUNCSIG__); return os; }
}

//Read from the partprogram file...//
wistream& operator>>(wistream& is, FramegrabBase& fg)
{
	try
	{
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,fg);
		}	
		else
		{	
			SurfaceED_Params* currSED;
			std::wstring Tagname ;
			is >> Tagname;
			fg.camProperty = ""; fg.lightProperty = "";
			fg.magLevel = "";
			int FgPointscount=0,FgPositioncount=0;
			if(Tagname==L"FramgrabBase")
			{			
				while(Tagname!=L"EndFramgrabBase")
				{	
					std::wstring Linestr;				
					is >> Linestr;
					if(is.eof())
					{						
						MAINDllOBJECT->PPLoadSuccessful(false);
						break;
					}
					int ColonIndx = Linestr.find(':');
					if(ColonIndx==-1)
					{
						Tagname=Linestr;
					}
					else
					{
						std::wstring Tag = Linestr.substr(0, ColonIndx);
						std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end() );
						Tagname=Tag;
						if(Tagname==L"Id")
						{
							fg.setId(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"OriginalName")
						{
							fg.setOriginalName(TagVal);
						}
						else if(Tagname==L"noofpts")
						{
							fg.noofpts=atoi((const char*)(Val).c_str());
							fg.noofptstaken_build = fg.noofpts;
						}
						else if(Tagname==L"ChannelNo")
						{
							
							fg.ChannelNo = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"MaskFactor")
						{
							if (fg.SurfaceON)
							{
								currSED = new SurfaceED_Params();
							}
							currSED->MaskFactor = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"Binary_LowerFactor")
						{
							currSED->Binary_Lower = atoi((const char*)(Val).c_str()); //fg.Binary_LowerFactor
						}
						else if(Tagname==L"Binary_UpperFactor")
						{
							currSED->Binary_Upper = atoi((const char*)(Val).c_str()); //fg.Binary_UpperFactor =
						}
						else if(Tagname==L"PixelThreshold")
						{
							currSED->Threshold = atoi((const char*)(Val).c_str()); //fg.PixelThreshold =
						}
						else if (Tagname == L"AverageFilter")
						{
							if (Val == "0")
								currSED->FilterByAverage = false;
							else
								currSED->FilterByAverage = true;
						}
						else if(Tagname==L"MaskingLevel")
						{
							currSED->MaskingLevel = atoi((const char*)(Val).c_str()); //fg.MaskingLevel
							fg.SEDP_List.push_back(*currSED);
						}
						else if(Tagname==L"FGdirection")
						{
							fg.FGdirection=atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"Surfacre_FrameGrabtype")
						{
							fg.Surfacre_FrameGrabtype=atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"ReferenceAction")
						{
							if(Val=="0")
								fg.ReferenceAction(false);
							else
								fg.ReferenceAction(true);
						}
						else if(Tagname==L"ActionForTwoStepHoming")
						{
							if(Val=="0")
								fg.ActionForTwoStepHoming(false);
							else
								fg.ActionForTwoStepHoming(true);
						}
						else if(Tagname==L"PointDRO")
						{
							is >> (*static_cast<Vector*>(&fg.PointDRO));
							fg.OriginalDRO.set(fg.PointDRO);
						}
						else if(Tagname==L"FGtype")
						{
							fg.FGtype = RapidEnums::RAPIDFGACTIONTYPE(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"ProbePosition")
						{
							fg.ProbePosition = RapidEnums::PROBEPOSITION(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"ProbeDirection")
						{
							fg.ProbeDirection = RapidEnums::PROBEDIRECTION(atoi((const char*)(Val).c_str()));
							fg.OriginalProbeDirection = fg.ProbeDirection;
						}
						else if(Tagname==L"FGWidth")
						{
							fg.FGWidth=atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"CamProperties")
						{
							fg.camProperty = RMATH2DOBJECT->WStringToString(TagVal);
						}
						else if(Tagname==L"LightIntensity")
						{
							fg.lightProperty = RMATH2DOBJECT->WStringToString(TagVal);
						}
						else if(Tagname==L"MagLevel")
						{
							fg.magLevel = RMATH2DOBJECT->WStringToString(TagVal);
						}
						else if(Tagname==L"ProfileON")
						{
							if(Val=="0")
								fg.ProfileON=false;
							else
								fg.ProfileON=true;
						}
						else if(Tagname==L"SurfaceON")
						{
							if(Val=="0")
								fg.SurfaceON=false;
							else
								fg.SurfaceON=true;
						}
						else if (Tagname == L"Auto_FG_AssistedMode")
						{
							if (Val == "0")
								fg.Auto_FG_AssistedMode = false;
							else
								fg.Auto_FG_AssistedMode = true;
						}
						else if(Tagname==L"DroMovementFlag")
						{
							if(Val=="0")
								fg.DroMovementFlag=false;
							else
								fg.DroMovementFlag=true;
						}
						else if(Tagname==L"DerivedShape")
						{
							if(Val=="0")
								fg.DerivedShape=false;
							else
								fg.DerivedShape=true;
						}
						else if(Tagname==L"UseDigitalZoom")
						{
							if(Val=="0")
								fg.UseDigitalZoom=false;
							else
								fg.UseDigitalZoom=true;
						}
						else if(Tagname==L"CurrentWindowNo")
						{
							fg.CurrentWindowNo=atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"Pointer_SnapPt")
						{
							Vector Temp;
							is >>(*static_cast<Vector*>(&Temp));
							UCS* ucs = MAINDllOBJECT->getUCS(fg.CucsId);
							fg.Pointer_SnapPt = MAINDllOBJECT->getVectorPointer_UCS(&Temp, ucs, true);
						}
						else if(Tagname==L"FgPoints")
						{
							for(int i = 0; i < 3; i++)
							is >> (*static_cast<Vector*>(&fg.points[i]));
						}
						else if(Tagname==L"FgPosition")
						{
							for(int i = 0; i < 5; i++)
							is >> (*static_cast<Vector*>(&fg.myPosition[i]));
						}
						//else if (Tagname == L"CG_Params")
						//{
						//	int jj = 0;
						//	while (jj < 4)
						//	{
						//		int index = Linestr.find(';'); int next_index = Linestr.substr(index + 1).find(';') + index;
						//		std::wstring cgrest = Linestr.substr(index + 1, next_index - index + 1);
						//		std::string cgval(cgrest.begin(), cgrest.end());
						//		fg.CG_Params[jj] = atof((const char*)cgval.c_str());
						//		Linestr = Linestr.substr(next_index);
						//		jj++;
						//	}
						//}
					}
				}
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0008", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, FramegrabBase& fg)
{
	try
	{
		wstring name;
		int n; bool flag;
		fg.camProperty = ""; fg.lightProperty = "";
		is >> n;
		fg.setId(n);
		is >> name;
		fg.setOriginalName(name);
		is >> fg.noofpts;
		fg.noofptstaken_build = fg.noofpts;
		is >> fg.FGdirection;
		is >> fg.Surfacre_FrameGrabtype;
		is >> flag; fg.ReferenceAction(flag);
		is >> flag; fg.ActionForTwoStepHoming(flag);
		is >> (*static_cast<Vector*>(&fg.PointDRO));
		fg.OriginalDRO.set(fg.PointDRO);
		is >> n;
		fg.FGtype = RapidEnums::RAPIDFGACTIONTYPE(n);
		is >> n;
		fg.ProbeDirection = RapidEnums::PROBEDIRECTION(n);
		fg.OriginalProbeDirection = RapidEnums::PROBEDIRECTION(n);
		is >> n;
		fg.ProbePosition = RapidEnums::PROBEPOSITION(n);
		is >> fg.FGWidth;
		is >> name;
		fg.camProperty = RMATH2DOBJECT->WStringToString(name);
		is >> name;
		fg.lightProperty = RMATH2DOBJECT->WStringToString(name);
		is >> fg.ProfileON;
		is >> fg.SurfaceON;
		is >> name;
		std::string Str = RMATH2DOBJECT->WStringToString(name);
		if(Str == "DroMovementFlag")
		{
			is >> fg.DroMovementFlag;		
			is >> fg.DerivedShape;
		}
		else
		{
			if(Str == "1")
				fg.DerivedShape = true;
			else
				fg.DerivedShape = false;
		}
		is >> fg.UseDigitalZoom;
		is >> n;
		fg.CurrentWindowNo = n;
		if(n == 1 && fg.DerivedShape)
		{
			Vector Temp;
			is >>(*static_cast<Vector*>(&Temp));
			UCS* ucs = MAINDllOBJECT->getUCS(fg.CucsId);
			fg.Pointer_SnapPt = MAINDllOBJECT->getVectorPointer_UCS(&Temp, ucs, true);
		}
		for(int i = 0; i < 3; i++)
			is >> (*static_cast<Vector*>(&fg.points[i]));
		for(int i = 0; i < 5; i++)
			is >> (*static_cast<Vector*>(&fg.myPosition[i]));
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("FGB0008", __FILE__, __FUNCSIG__); }
}
