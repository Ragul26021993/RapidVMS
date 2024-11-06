#include <math.h>
#include "StdAfx.h"
#include "CamProfile.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Engine\PointCollection.h"
#include "Circle.h"
#include "Line.h"
#include "..\Helper\Helper.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\PartProgramFunctions.h"
#include <string>
int PTsBndl = 0;		//added by vj 11/04/2014
double RX1, RY1, RX2, RY2, Ray_slope_for_cam_profile_shape=0, Ray_intercept_for_cam_profile_shape=0, Delphi_Cam_circle_centre[3], Delphi_Cam_circle_rad = 0.0;			//vinod for drawing nominal shape in camprofile
double DC_Upper_tol = 0.0, DC_Lower_tol = 0.0, DC_angle=0.0, INputMin_Radius = 0.0, INputMax_Radius = 0.0;				//vinod           											
bool Is_ID_Tool = true;    //touch through inner or outer side in tool ....vinod
char CurrentDate[18] = ""; char CurrentTime[18] = ""; char MinDeviation[10] = "";  char MaxDeviation[10] = "";
CamProfile::CamProfile(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

CamProfile::CamProfile(bool simply):ShapeWithList(false)
{
	init();
}

void CamProfile::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::CAMPROFILE;
		this->ShapeAs3DShape(false);
		this->IsValid(false);
		this->MinRadius = 7.905;
		this->OuterCircle = NULL;
		this->RayLine = NULL;
		this->RPoint3dForZposition = NULL;
		this->IdCamProfile(true);
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if(!CShape->Normalshape()) continue;
			if(!CShape->selected()) continue;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{				
				this->OuterCircle = CShape;
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{				
				this->RayLine = CShape;
			}
			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{				
				this->RPoint3dForZposition = CShape;
			}
		}
		for(int i = 0; i < 4; i++)
			CamParameter[i] = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP001", __FILE__, __FUNCSIG__); }
}

CamProfile::~CamProfile()
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP002", __FILE__, __FUNCSIG__); }
}

TCHAR* CamProfile::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		shapenumber++;
		TCHAR shapenumstr[10];
		_itot_s(shapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP003", __FILE__, __FUNCSIG__); return name; }
}

bool CamProfile::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP004", __FILE__, __FUNCSIG__); return false;}
}

bool CamProfile::Shape_IsInWindow( Vector& corner1,Vector& corner2 )
{
	return false;
}

bool CamProfile::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP005", __FILE__, __FUNCSIG__); return false;}
}

void CamProfile::Translate(Vector& Position)
{
	try
	{
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP006", __FILE__, __FUNCSIG__);}
}
//vinod jakhar..
void CamProfile::Set_DeiphiCam_Circle_Centre(double x_coord, double y_coord, double z_coord, double ray_slope, double ray_intercept, double DCCRad)			//vinod jakhar
{
	Delphi_Cam_circle_centre[0] = x_coord;
	Delphi_Cam_circle_centre[1] = y_coord;								
	Delphi_Cam_circle_centre[2] = z_coord;
	Delphi_Cam_circle_rad = DCCRad;
	Ray_slope_for_cam_profile_shape=ray_slope;
	Ray_intercept_for_cam_profile_shape=ray_intercept;
}//vinod jakhar..
void CamProfile::Set_DeiphiCam_parameters(double DeiphiCam_Angle, double DeiphiCam_Lower_tol, double DeiphiCam_Upper_tol, bool ToolType, int PtstoBundle, double rayx1, double rayy1, double rayx2, double rayy2, double MinRad, double MaxRad, char* CurDate, char* CurTime)		//vinod
{
	Is_ID_Tool = ToolType;
	DC_angle = DeiphiCam_Angle;
	DC_Upper_tol = DeiphiCam_Upper_tol;
	DC_Lower_tol = DeiphiCam_Lower_tol;
	PTsBndl = PtstoBundle;
	RX1 = rayx1;					//ray's point1x
	RY1 = rayy1;					//ray's point1y
	RX2 = rayx2;					//ray's point2x
	RY2 = rayy2;					//ray's point2y
	INputMin_Radius = MinRad;
	INputMax_Radius = MaxRad;
	strcpy(CurrentDate,CurDate);		//to attach with image
	strcpy(CurrentTime,CurTime);		//to attach with image
}
//vinod jakhar..
void CamProfile::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{	
	    if(Is_ID_Tool && this->IsValid() && this->PointsList->Pointscount() > 0) 
		{	
			double PtsArray[730] = {0}, PtsArray_outer[730] = {0}, PtsArray_inner[730] = {0}, PtsArray_Deviation[730] = {0}, Angle = 0.0;
			int PtsOrder[365] = {0};
			int PtsCount = 0;			
			double Range_offset = 3.0, dist = 0.0;       // drawing full shape...0-30,330-360 and 150-210 degree is a arc, 30-150 and 210-330 is variblle length arc
			//drawing 0 to 30 degree
			for(int i = 0; i <= 30; i++)														// vinod 15/01/2014 drawing nominal shape, and max min limit shapes
			{
					Angle =  Ray_slope_for_cam_profile_shape + ( i * M_PI / 180);
					PtsArray[2 * i] = Delphi_Cam_circle_centre[0] + INputMin_Radius * cos(Angle);										//parameters  for nominal															
					PtsArray[2 * i + 1] = Delphi_Cam_circle_centre[1] + INputMin_Radius * sin(Angle);		

					PtsArray_outer[2 * i] = Delphi_Cam_circle_centre[0] + (INputMin_Radius+Range_offset) *  cos(Angle);					//parameters for outer shape										
					PtsArray_outer[2 * i + 1] = Delphi_Cam_circle_centre[1] + (INputMin_Radius+Range_offset) * sin(Angle);												

					PtsArray_inner[2 * i] = Delphi_Cam_circle_centre[0] + (INputMin_Radius-Range_offset) * cos(Angle);							//parameters for inner 	shape							
					PtsArray_inner[2 * i + 1] = Delphi_Cam_circle_centre[1] + (INputMin_Radius-Range_offset) * sin(Angle);														
				
					PtsOrder[PtsCount++] = PtsCount;
			}//drawing 151 to 210 degree
				for(int i = 151; i <= 210; i++)
				{
					Angle = Ray_slope_for_cam_profile_shape + ( i * M_PI / 180 );					
					PtsArray[2 * i] = Delphi_Cam_circle_centre[0] + INputMax_Radius * cos(Angle);														//parameters  for nominal	
					PtsArray[2 * i + 1] = Delphi_Cam_circle_centre[1] + INputMax_Radius * sin(Angle);														
				
					PtsArray_outer[2 * i] = Delphi_Cam_circle_centre[0] + (INputMax_Radius+Range_offset) * cos(Angle);															
					PtsArray_outer[2 * i + 1] = Delphi_Cam_circle_centre[1] + (INputMax_Radius+Range_offset) * sin(Angle);						//parameters for outer shape								
				
					PtsArray_inner[2 * i] = Delphi_Cam_circle_centre[0] + (INputMax_Radius-Range_offset) * cos(Angle);															
					PtsArray_inner[2 * i + 1] = Delphi_Cam_circle_centre[1] + (INputMax_Radius-Range_offset) * sin(Angle);			//parameters for inner 	shape											
				
					PtsOrder[PtsCount++] = PtsCount;
				}//drawing 331 to 360 degree
				for(int i = 331; i <= 360; i++)
				{
					Angle = Ray_slope_for_cam_profile_shape + ( i * M_PI / 180 );
					PtsArray[2 * i] = Delphi_Cam_circle_centre[0] + INputMin_Radius * cos(Angle);													//parameters  for nominal		
					PtsArray[2 * i + 1] = Delphi_Cam_circle_centre[1] + INputMin_Radius * sin(Angle);														
				
					PtsArray_outer[2 * i] = Delphi_Cam_circle_centre[0] + (INputMin_Radius+Range_offset) * cos(Angle);							//parameters for outer shape								
					PtsArray_outer[2 * i + 1] = Delphi_Cam_circle_centre[1] + (INputMin_Radius+Range_offset) * sin(Angle);														
				
					PtsArray_inner[2 * i] = Delphi_Cam_circle_centre[0] + (INputMin_Radius-Range_offset) * cos(Angle);															
					PtsArray_inner[2 * i + 1] = Delphi_Cam_circle_centre[1] + (INputMin_Radius-Range_offset) * sin(Angle);					//parameters for inner 	shape									
				
					PtsOrder[PtsCount++] = PtsCount;
				}//drawing 31 to 150 degree
				for(int i = 31; i <= 150; i++)
				{
					Angle =  ( i * M_PI / 180 );
					dist = ((INputMin_Radius + INputMax_Radius)/2) - (2 * cos((1.5 * Angle) - (M_PI / 4)));
					Angle+=Ray_slope_for_cam_profile_shape;
					PtsArray[2 * i] = Delphi_Cam_circle_centre[0] + dist * cos(Angle);															//parameters  for nominal
					PtsArray[2 * i + 1] = Delphi_Cam_circle_centre[1] + dist * sin(Angle);														
				
					PtsArray_outer[2 * i] = Delphi_Cam_circle_centre[0] + (dist+Range_offset)* cos(Angle);										//parameters for outer shape					
					PtsArray_outer[2 * i + 1] = Delphi_Cam_circle_centre[1] + (dist+Range_offset) * sin(Angle);														
				
					PtsArray_inner[2 * i] = Delphi_Cam_circle_centre[0] + (dist-Range_offset) * cos(Angle);									//parameters for inner 	shape						
					PtsArray_inner[2 * i + 1] = Delphi_Cam_circle_centre[1] + (dist-Range_offset) * sin(Angle);														
				
					PtsOrder[PtsCount++] = PtsCount;
				}//drawing 221 to 330 degree
				for(int i = 211; i <= 330; i++)
				{
					Angle =  ( i * M_PI / 180 );
					dist = ((INputMin_Radius + INputMax_Radius)/2) - (2 * cos((1.5 * Angle) - (3* M_PI / 4)));
					Angle+=Ray_slope_for_cam_profile_shape;
					PtsArray[2 * i] = Delphi_Cam_circle_centre[0] + dist * cos(Angle);															//parameters  for nominal
					PtsArray[2 * i + 1] = Delphi_Cam_circle_centre[1] + dist * sin(Angle);														
				
					PtsArray_outer[2 * i] = Delphi_Cam_circle_centre[0] + (dist+Range_offset)* cos(Angle);										//parameters for outer shape						
					PtsArray_outer[2 * i + 1] = Delphi_Cam_circle_centre[1] + (dist+Range_offset)* sin(Angle);															
				
					PtsArray_inner[2 * i] = Delphi_Cam_circle_centre[0] + (dist-Range_offset)* cos(Angle);										//parameters for inner shape							
					PtsArray_inner[2 * i + 1] = Delphi_Cam_circle_centre[1] + (dist-Range_offset)* sin(Angle);															
				
					PtsOrder[PtsCount++] = PtsCount;
				}
				//we can use here dxfexpose's noominal.. for future..no need to calculate again..
				GRAFIX->SetColor_Double(0, 0, 1);				//blue
				GRAFIX->DrawCamProfile(PtsArray, PtsCount, PtsOrder);												//draw nominal.......Vinod Jakhar

				GRAFIX->SetColor_Double(1, 0, 0);				//red
				GRAFIX->DrawCamProfile(PtsArray_outer, PtsCount, PtsOrder);											//draw outer shape
			
				GRAFIX->SetColor_Double(1, 0, 0);				//red
				GRAFIX->DrawCamProfile(PtsArray_inner, PtsCount, PtsOrder);											//draw inner shape
				
				PointCollection SortedPtsCollection;
				PtsList XYPtsCollection;
					
				int Tmp_Count = 0;								//point teken for drawing
				for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
				{		
					SinglePoint* Spt = (*SptItem).second;
					Pt Point_XY;
					Point_XY.pt_index = Spt->PtID;
					Point_XY.x = Spt->X;
					Point_XY.y = Spt->Y;
					XYPtsCollection.push_back(Point_XY);		//collecting points in XYPtsCollection
					Tmp_Count++;
				}
				if((Tmp_Count%((int)(360/DC_angle)))==0)					//draw deviation only after all points had taken
				{
					for each(Pt Point_XY in XYPtsCollection)
					{
						SinglePoint* Spt = new SinglePoint();
						Spt->PtID = Point_XY.pt_index;
						Spt->X = Point_XY.x;
						Spt->Y = Point_XY.y;
						Spt->Z = this->PointsList->getList()[Spt->PtID]->Z;		//add each point in SortedPtsCollection
						SortedPtsCollection.Addpoint(Spt);
					}				
					
					int i=0, j = 0;
					double MinDev = 1000.0, MaxDev = 0.0, touch_point_slope = 0.0, deviation_pt_slope_wrt_ray = 0.0, off_set = 0.0, NominalDist = 0.0, ActualDist = 0.0;					
					Angle = 0.0;
					PtsCount=0;
					for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
					{	
						Angle =   i * DC_angle * M_PI / 180;
						double Local_CamParam[4] = {0};						//these are camparameteres for actual distance
						if(DXFEXPOSEOBJECT->GetLocalCamProfileParameter(PTsBndl, (Angle + this->CamParameter[3]), &SortedPtsCollection, this->CamParameter, Local_CamParam))		//collecting Local_camparam
						{
							BESTFITOBJECT->CamProfile_GetDist(this->CamParameter, (Angle + this->CamParameter[3]), &NominalDist);		//localdist is actual
							BESTFITOBJECT->CamProfile_GetDist(Local_CamParam, this->CamParameter, (Angle + this->CamParameter[3]), &ActualDist);		// collecting actual dist
						}
						i++;

						PtsArray_Deviation[j] =  ((*SptItem).second)->X;			//collecting actual touched point
						PtsArray_Deviation[j+1]  =  ((*SptItem).second)->Y;
						
						Ray_slope_for_cam_profile_shape = RMATH2DOBJECT->ray_angle(RX1, RY1, RX2, RY2);			//slope of ray wrt to rayPoint1 0-2PI
						touch_point_slope = RMATH2DOBJECT->ray_angle(RX1, RY1, PtsArray_Deviation[j], PtsArray_Deviation[j+1]);    //slope of touch point wrt to rayPoint1 0-2PI
						deviation_pt_slope_wrt_ray = touch_point_slope - Ray_slope_for_cam_profile_shape;		//calculating touched point's slope wrt ray
						if(deviation_pt_slope_wrt_ray<0)
							deviation_pt_slope_wrt_ray += (2* M_PI); 
						deviation_pt_slope_wrt_ray = (deviation_pt_slope_wrt_ray/M_PI)*180;					//deviation_pt_slope_wrt_ray in degree

						double x_value =PtsArray[(2*(int)deviation_pt_slope_wrt_ray)];
						double y_value =PtsArray[(2*(int)deviation_pt_slope_wrt_ray) + 1];
						x_value -= Delphi_Cam_circle_centre[0];
						y_value -= Delphi_Cam_circle_centre[1];
						x_value /= NominalDist;
						y_value /= NominalDist;
						
						//calculate min and max deviation for export in Rcad image...
						if((abs(ActualDist-NominalDist))<MinDev)
							MinDev = abs(ActualDist-NominalDist);
						if((abs(ActualDist-NominalDist))>MaxDev)
							MaxDev = abs(ActualDist-NominalDist);
						
						//new location of points according to tollerance range
						if((ActualDist-NominalDist)<0)																			//in upper tolrance range
						{
							off_set = (abs(ActualDist-NominalDist)/DC_Upper_tol)*Range_offset;								//represent the deviation in the tolerance range, let given range is represented by a band of Range_offset of 3.0
							PtsArray_Deviation[j] = ((x_value)*(NominalDist + off_set))+Delphi_Cam_circle_centre[0];		//for draw deviated point we add the offset in the touched point according to position of point wrt to ray
							PtsArray_Deviation[j+1] = ((y_value)*(NominalDist + off_set))+Delphi_Cam_circle_centre[1];
						}
						else																										//in lower tolrance range
						{
							off_set = (abs(ActualDist-NominalDist)/DC_Lower_tol)*Range_offset;								//represent the deviation in the tolerance range, let given range is represented by a band of Range_offset of 3.0
							PtsArray_Deviation[j] = ((x_value)*(NominalDist - off_set))+Delphi_Cam_circle_centre[0];		//for draw deviated point we add the offset in the touched point according to position of point wrt to ray
							PtsArray_Deviation[j+1] = ((y_value)*(NominalDist - off_set))+Delphi_Cam_circle_centre[1];
						}				
						PtsOrder[PtsCount++] = PtsCount;
						j+=2;					
					}
					if(j/2*DC_angle==360)																//if this is last point in the rotation then we join this with first point
					{
						PtsArray_Deviation[j] =  PtsArray_Deviation[0];
						PtsArray_Deviation[j+1]  =  PtsArray_Deviation[1];
						PtsOrder[PtsCount++] = PtsCount;
					}
					GRAFIX->SetColor_Double(0, 1, 0);						//deviation in green
					GRAFIX->DrawCamProfile(PtsArray_Deviation, PtsCount, PtsOrder);					//sending paraeter to draw figure 
					
					//28/04/2014 adding date, time, minDev, Maxdev in image... Vinod Jakhar
					GRAFIX->SetColor_Double(1, 0, 0);
					RMATH2DOBJECT->Double2String(MinDev, 4, MinDeviation, false);		//convert double(MinDev) upto 4 decimal to String(MinDeviation 
					RMATH2DOBJECT->Double2String(MaxDev, 4, MaxDeviation, false);		//convert double(MaxDev) upto 4 decimal to String(MaxDeviation)
					std::string dt = "Date:";
					std::string tm = "Time:";
					std::string mn = "Min:";
					std::string mx = "Max:";
					char FullFormetedDate[18] = "";						//to store full date format as "Date: 28/04/2014"
					char FullFormetedTime[18] = "";						//to store full time format as "Time: 04-13 PM"
					char FullFormetedMinDev[18] = "";					//to store full mindev format as "Min: 0.0025"
					char FullFormetedMaxDev[18] = "";					//to store full maxdev format as "max: 0.0139"
					RMATH2DOBJECT->Concat2stringin3rd(dt, (std::string)CurrentDate, FullFormetedDate);//store here full date format as "Date: 28/04/2014"
					RMATH2DOBJECT->Concat2stringin3rd(tm, (std::string)CurrentTime, FullFormetedTime);//store here full time format as "Time: 04-13 PM"
					RMATH2DOBJECT->Concat2stringin3rd(mn, (std::string)MinDeviation, FullFormetedMinDev);//store here mindev format as "Min: 0.0025"
					RMATH2DOBJECT->Concat2stringin3rd(mx, (std::string)MaxDeviation, FullFormetedMaxDev);//store here maxdev format as "max: 0.0139"
						
					double TextHeight = Delphi_Cam_circle_rad/5*(GRAFIX->getTexHeight(FullFormetedDate));	//calculate height here
					//drawStringEdited3D function used for draw string ..
					GRAFIX->drawStringEdited3D(Delphi_Cam_circle_centre[0]-Delphi_Cam_circle_rad, Delphi_Cam_circle_centre[1]-(Delphi_Cam_circle_rad * 0.6) - (TextHeight * 0), Delphi_Cam_circle_centre[2], 0, FullFormetedDate,  20* WPixelWidth);		//print the date in image at the location in 3rd coordinate from delphi center
					GRAFIX->drawStringEdited3D(Delphi_Cam_circle_centre[0]-Delphi_Cam_circle_rad, Delphi_Cam_circle_centre[1]-(Delphi_Cam_circle_rad * 0.6) - (TextHeight * 1), Delphi_Cam_circle_centre[2], 0, FullFormetedTime,  20* WPixelWidth);		//print the time in image at the location in 3rd coordinate from delphi center and below the date
					GRAFIX->drawStringEdited3D(Delphi_Cam_circle_centre[0]-Delphi_Cam_circle_rad, Delphi_Cam_circle_centre[1]-(Delphi_Cam_circle_rad * 0.6) - (TextHeight * 2), Delphi_Cam_circle_centre[2], 0, FullFormetedMinDev,  20* WPixelWidth);		//print the minD in image at the location in 3rd coordinate from delphi center and below the time
					GRAFIX->drawStringEdited3D(Delphi_Cam_circle_centre[0]-Delphi_Cam_circle_rad, Delphi_Cam_circle_centre[1]-(Delphi_Cam_circle_rad * 0.6)- (TextHeight * 3), Delphi_Cam_circle_centre[2], 0, FullFormetedMaxDev,  20* WPixelWidth);		//print the maxD in image at the location in 3rd coordinate from delphi center and below the minD
		
				}
				//older code
			//PtsCount=0;
			//std::map<int, SinglePoint*>::iterator sp;
			//int j = 0, deviation_pt_slope_wrt_ray = 0;
			//double deviation_dist, actual_dist, touch_point_slope, off_set;
			//for(sp = this->PointsList->getList().begin(); sp != this->PointsList->getList().end(); sp++)				//deviation design vinod 26/01/2014
			//{
			//	PtsArray_Deviation[j] =  ((*sp).second)->X;
			//	PtsArray_Deviation[j+1]  =  ((*sp).second)->Y;
			//	deviation_dist= RMATH2DOBJECT->Pt2Pt_distance(PtsArray_Deviation[j], PtsArray_Deviation[j+1], Delphi_Cam_circle_centre[0], Delphi_Cam_circle_centre[1]);
			//	touch_point_slope= RMATH2DOBJECT->AngleofLineWithTwoPtswrtXaxis0To2PI(Delphi_Cam_circle_centre[0], Delphi_Cam_circle_centre[1], PtsArray_Deviation[j], PtsArray_Deviation[j+1]);
			//	
			//	//angle of line(using center_pt,touch_pt) w.r.t. ray                          //vinod    08/02/2014
			//	if((touch_point_slope-Ray_slope_for_cam_profile_shape)<= (M_PI/2))//1st cordinate... theeta
			//		deviation_pt_slope_wrt_ray=(asin(RMATH2DOBJECT->Pt2Line_Dist(PtsArray_Deviation[j], PtsArray_Deviation[j+1], Ray_slope_for_cam_profile_shape, Ray_intercept_for_cam_profile_shape)/deviation_dist) * 180 /M_PI);
			//	else if((touch_point_slope-Ray_slope_for_cam_profile_shape)<= (M_PI))//second  coordinate......pi-theeta
			//		deviation_pt_slope_wrt_ray=180.0-(asin(RMATH2DOBJECT->Pt2Line_Dist(PtsArray_Deviation[j], PtsArray_Deviation[j+1], Ray_slope_for_cam_profile_shape, Ray_intercept_for_cam_profile_shape)/deviation_dist) * 180 /M_PI);
			//	else if((touch_point_slope-Ray_slope_for_cam_profile_shape)<= (3*M_PI/2))//3rd..... pi+theeta
			//		deviation_pt_slope_wrt_ray=180.0+(asin(RMATH2DOBJECT->Pt2Line_Dist(PtsArray_Deviation[j], PtsArray_Deviation[j+1], Ray_slope_for_cam_profile_shape, Ray_intercept_for_cam_profile_shape)/deviation_dist) * 180 /M_PI);
			//	else
			//		deviation_pt_slope_wrt_ray=360.0-(asin(RMATH2DOBJECT->Pt2Line_Dist(PtsArray_Deviation[j], PtsArray_Deviation[j+1], Ray_slope_for_cam_profile_shape, Ray_intercept_for_cam_profile_shape)/deviation_dist) * 180 /M_PI);
			//	
			//	actual_dist= RMATH2DOBJECT->Pt2Pt_distance(PtsArray[2 * deviation_pt_slope_wrt_ray], PtsArray[(2 * deviation_pt_slope_wrt_ray) + 1], Delphi_Cam_circle_centre[0], Delphi_Cam_circle_centre[1]);
			//	if(actual_dist-deviation_dist<0)																			//in upper tolrance range
			//	{
			//		off_set = ((deviation_dist-actual_dist)/DC_Upper_tol)*Range_offset;
			//		PtsArray_Deviation[j] = (((PtsArray[2 * deviation_pt_slope_wrt_ray]-Delphi_Cam_circle_centre[0])/actual_dist)*(actual_dist + off_set))+Delphi_Cam_circle_centre[0];
			//		PtsArray_Deviation[j+1] = (((PtsArray[(2 * deviation_pt_slope_wrt_ray) + 1]-Delphi_Cam_circle_centre[1])/actual_dist)*(actual_dist + off_set))+Delphi_Cam_circle_centre[1];
			//	}
			//	else																										//in lower tolrance range
			//	{
			//		off_set = ((actual_dist-deviation_dist)/DC_Lower_tol)*Range_offset;
			//		PtsArray_Deviation[j] = (((PtsArray[2 * deviation_pt_slope_wrt_ray]-Delphi_Cam_circle_centre[0])/actual_dist)*(actual_dist - off_set))+Delphi_Cam_circle_centre[0];
			//		PtsArray_Deviation[j+1] = (((PtsArray[(2 * deviation_pt_slope_wrt_ray) + 1]-Delphi_Cam_circle_centre[1])/actual_dist)*(actual_dist - off_set))+Delphi_Cam_circle_centre[1];
			//	}

			//	PtsOrder[PtsCount++] = PtsCount;
			//	j+=2;
			//}	
			//if(j/2*DC_angle==360)																//if this is last point in the rotation then we join this with first point
			//{
			//	PtsArray_Deviation[j] =  PtsArray_Deviation[0];
			//	PtsArray_Deviation[j+1]  =  PtsArray_Deviation[1];
			//	PtsOrder[PtsCount++] = PtsCount;
			//}
			//GRAFIX->SetColor_Double(0, 1, 0);
			//GRAFIX->DrawCamProfile(PtsArray_Deviation, PtsCount, PtsOrder);					//sending paraeter to draw figure 
		}
		else
		{
			double PtsArray_Deviation[730] = {0};				// drawing for OD.. tpliner
			int PtsOrder[365] = {0};
			int PtsCount = 0;
			int i;
			
			std::map<int, SinglePoint*>::iterator sp;
			int j = 0, deviation_pt_slope_wrt_ray=0;
			for(sp = this->PointsList->getList().begin(); sp != this->PointsList->getList().end(); sp++)				//vinod  deviation design vinod 08/02/2014
			{
				PtsArray_Deviation[j] =  ((*sp).second)->X;
				PtsArray_Deviation[j+1]  =  ((*sp).second)->Y;				

				PtsOrder[PtsCount++] = PtsCount;
				j+=2;
			}	
			if(j/2*DC_angle==360)																//if this is last point in the rotation then we join this with first point
			{
				PtsArray_Deviation[j] =  PtsArray_Deviation[0];
				PtsArray_Deviation[j+1]  =  PtsArray_Deviation[1];
				PtsOrder[PtsCount++] = PtsCount;	
			}			
			GRAFIX->SetColor_Double(0, 1, 0);
			GRAFIX->DrawCamProfile(PtsArray_Deviation, PtsCount, PtsOrder);		   //sending paraeter to draw figure 
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP007", __FILE__, __FUNCSIG__); }
}

void CamProfile::drawGDntRefernce()
{
}

void CamProfile::ResetShapeParameters()
{
}

void CamProfile::UpdateBestFit()
{
	try
	{
		if(DXFEXPOSEOBJECT->LoadDXF) 
			return;
		this->IsValid(false);
		if(this->PointsList->Pointscount() < 1) return;
		
		if(this->OuterCircle != NULL && this->RayLine != NULL)
		{
			if(this->pts != NULL){free(pts); pts = NULL;}
			this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);

			if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
			this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
			filterptsCnt = 0;
			int PtsCnt = 0, j = 0;
			for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
			{
				SinglePoint* Pt = (*Spt).second;
				this->pts[PtsCnt++] = Pt->X;
				this->pts[PtsCnt++] = Pt->Y;			
				this->filterpts[j++] = Pt->X;
				this->filterpts[j++] = Pt->Y;			
				filterptsCnt++;
			}
			double Answer[4] = {0}, InitialGuess[4] = {0};
			InitialGuess[0] = ((Circle*)this->OuterCircle)->getCenter()->getX(); InitialGuess[1] = ((Circle*)this->OuterCircle)->getCenter()->getY();
			InitialGuess[2] = this->MinRadius; InitialGuess[3] = ((Line*)RayLine)->Angle();
			if(BESTFITOBJECT->CamProfile_BestFit(pts, PtsCnt / 2, InitialGuess, Answer))
			{
				this->IsValid(true);
				for(int i = 0; i < 4; i++)
					this->CamParameter[i] = Answer[i];
				//ProbePointCalculations(this->PointsList->Pointscount());
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP008", __FILE__, __FUNCSIG__); }
}

bool CamProfile::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP009", __FILE__, __FUNCSIG__); return false;}
}

void CamProfile::UpdateForParentChange(BaseItem* sender)
{
}

void CamProfile::UpdateEnclosedRectangle()
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0010", __FILE__, __FUNCSIG__);}
}

void CamProfile::Transform(double* transform)
{
	try
	{
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP011", __FILE__, __FUNCSIG__); }
}

void CamProfile::AlignToBasePlane(double* trnasformM)
{
	try
	{
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP012", __FILE__, __FUNCSIG__); }
}

Shape* CamProfile::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("CP");
		else
			myname = _T("dCP");
		CamProfile* CShape = new CamProfile((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		    CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP013", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* CamProfile::CreateDummyCopy()
{
	try
	{
		CamProfile* CShape = new CamProfile(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP014", __FILE__, __FUNCSIG__); return NULL; }
}

void CamProfile::CopyShapeParameters(Shape* s)
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP015", __FILE__, __FUNCSIG__); }
}

int CamProfile::shapenumber=0;

void CamProfile::reset()
{
	shapenumber = 0;
}

void CamProfile::GetShapeCenter(double *cPoint)
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP016", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, CamProfile& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		
		return os;

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP017", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, CamProfile& x )
{
	try
	{
		is >> (*(Shape*)&x);
		x.ShapeType = RapidEnums::SHAPETYPE::CAMPROFILE;
		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CP018", __FILE__, __FUNCSIG__); return is; }
}

void CamProfile::SetMinRadius(double Radius)
{
	try
	{
		this->MinRadius = Radius;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP019", __FILE__, __FUNCSIG__); }
}

void CamProfile::GetCamProfileParam(double *Parameter)
{
	for(int i = 0; i < 4; i++)
		Parameter[i] = this->CamParameter[i];
}

void CamProfile::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt >= 10 && PointsCnt < 20)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdCamProfile();
		}
		if(ShapeDoneUsingProbe())
		{
			updateCamProfile_UsingPorbe();
			if(this->pts != NULL){free(pts); pts = NULL;}
			this->pts = (double*)malloc(sizeof(double) * this->PointsList->Pointscount() * 2);

			if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
			this->filterpts = (double*)malloc(sizeof(double) * this->PointsList->Pointscount() * 2);
			filterptsCnt = 0;
			int PtsCnt = 0, j = 0;
			for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
			{
				SinglePoint* Pt = (*Spt).second;
				this->pts[PtsCnt++] = Pt->X;
				this->pts[PtsCnt++] = Pt->Y;			
				this->filterpts[j++] = Pt->X;
				this->filterpts[j++] = Pt->Y;			
				filterptsCnt++;
			}
			double Answer[4] = {0}, InitialGuess[4] = {0};
			InitialGuess[0] = ((Circle*)this->OuterCircle)->getCenter()->getX(); InitialGuess[1] = ((Circle*)this->OuterCircle)->getCenter()->getY();
			InitialGuess[2] = this->MinRadius; InitialGuess[3] = ((Line*)RayLine)->Angle();
			this->IsValid(false);
			if(BESTFITOBJECT->CamProfile_BestFit(pts, PtsCnt / 2, InitialGuess, Answer))
			{
				this->IsValid(true);
				for(int i = 0; i < 4; i++)
					this->CamParameter[i] = Answer[i];
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP020", __FILE__, __FUNCSIG__); }
}

void CamProfile::CheckPointIsForPorbe()
{
	try
	{
		if(PointAtionList.size() < 1) return;
		AddPointAction* Caction = (AddPointAction*)(*PointAtionList.begin());
		if(Caction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
		{
			if(this->ProbeCorrection())
				this->ShapeDoneUsingProbe(true);
			else
				this->ShapeDoneUsingProbe(false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP021", __FILE__, __FUNCSIG__); }
}

void CamProfile::CheckIdorOdCamProfile()
{
	try
	{
		int IDCount = 0;
		PointCollection ptCol;
		HELPEROBJECT->GetMcsPointsList(this->PointsListOriginal, &ptCol);
		Vector CenterMcs, CamCenter;
		CamCenter.set(CamParameter[0], CamParameter[1]);
		HELPEROBJECT->GetMcsPoint(&CamCenter, &CenterMcs);
		for(PC_ITER Item = ptCol.getList().begin(); Item != ptCol.getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(Spt->Pdir == 0) //FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT
			{
				if((CenterMcs.getX() - Spt->X) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 1) //FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT
			{
				if((CenterMcs.getX() - Spt->X) < 0)
					IDCount++;
				else
					IDCount--;
			}
			else if(Spt->Pdir == 2) //FramegrabBase::PROBEDIRECTION::YTOP2BOTTOM
			{
				if((CenterMcs.getY() - Spt->Y) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 3) // FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP
			{
				if((CenterMcs.getY() - Spt->Y) < 0)
					IDCount++;
				else
					IDCount--;
			}
			else if(Spt->Pdir == 4) // FramegrabBase::PROBEDIRECTION::ZTOPTOBOTTOM
			{
				if((CenterMcs.getZ() - Spt->Z) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 5) // FramegrabBase::PROBEDIRECTION::ZBOTTOM2TOP
			{
				if((CenterMcs.getZ() - Spt->Z) < 0)
					IDCount++;
				else
					IDCount--;
			}
		}
		ptCol.deleteAll();
		if(IDCount >= 0)
			this->IdCamProfile(true);
		else
			this->IdCamProfile(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP022", __FILE__, __FUNCSIG__); }
}

void CamProfile::updateCamProfile_UsingPorbe()
{
	try
	{
		double Pradius = 0;
		if(this->pts != NULL){free(pts); pts = NULL;}
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		
		double pt_in[2] = {0}, pt_out[2] = {0};
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			Pradius = getProbeRadius(Spt->PLR);
			pt_in[0] = Spt->X; pt_in[1] = Spt->Y; pt_out[0] = Spt->X; pt_out[1] = Spt->Y;
			applyDelphiCamProbeCorrection(pt_in, pt_out, Pradius, this->IdCamProfile());
			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(pt_out[0], pt_out[1], Spt->Z, Spt->R, Spt->Pdir);
		}

		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		filterptsCnt = 0;
		int n = 0, j = 0;
		for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			this->pts[n++] = Pt->X;
			this->pts[n++] = Pt->Y;
			if(Pt->InValid) continue;
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			filterptsCnt++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP023", __FILE__, __FUNCSIG__); }
}

bool CamProfile::applyDelphiCamProbeCorrection(double* pt_in, double* pt_out, double probeRadius, bool inner)
{
	try
	{
		double x_in = pt_in[0]; 
		double y_in = pt_in[1];
		double delta_x = x_in - CamParameter[0];
		double delta_y = y_in - CamParameter[1];
		double probeCorrection[2] = {0};
		int mult = 1;
		if (!inner)
			mult = -1;

		double delta_norm = sqrt(pow(delta_x,2) + pow(delta_y,2));
		if (delta_norm == 0)
			return false;
		double ang = acos (delta_x/delta_norm);
		if (delta_y < 0)
			ang = 2 * M_PI - ang;
		
		double min_radius = CamParameter[2];
		double theta = ang - CamParameter[3];
		double r = 0;
		double xdash = 0;
		double ydash = 0;
		double norm = 0;

		//calculate the normalized tangent vector xdash, ydash.  Normal vector is ydash, -xdash
		if ((theta <= M_PI/6) || (theta >= 11*M_PI/6) || ((theta >= 5*M_PI/6) && (theta <= 7*M_PI/6)))
		{
			xdash = - sin (ang);
			ydash = cos (ang);
		}
		else if (theta < 5*M_PI/6) //this is case when angle is between 30 and 150 degrees
		{
			r = min_radius + 2 - 2 * cos(1.5 * theta - M_PI/4);
			xdash = 3 * sin (1.5 * theta - M_PI/4) * cos (ang) - r * sin (ang);
			ydash = 3 * sin (1.5 * theta - M_PI/4) * sin (ang) + r * cos (ang);
			norm = sqrt(xdash*xdash + ydash*ydash);
			if (norm == 0)
				return false;
			xdash /= norm;
			ydash /= norm;
		}
		else //this is case when angle is between 210 and 330 degrees
		{
			r = min_radius + 2 - 2 * sin(1.5 * theta - M_PI/4);
			xdash = -3 * cos (1.5 * theta - M_PI/4) * cos (ang) - r * sin (ang);
			ydash = -3 * cos (1.5 * theta - M_PI/4) * sin (ang) + r * cos (ang);
			norm = sqrt(xdash*xdash + ydash*ydash);
			if (norm == 0)
				return false;
			xdash /= norm;
			ydash /= norm;
		}

		//calculate probe correction
		probeCorrection[0] = ydash * mult * probeRadius;
		probeCorrection[1] = -xdash * mult * probeRadius;

		//calculate corrected point
		pt_out[0] = x_in + probeCorrection[0];
		pt_out[1] = y_in + probeCorrection[1];
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP023", __FILE__, __FUNCSIG__); return false;}
}
