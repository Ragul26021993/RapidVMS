#include "stdafx.h"
#include "Conics3D.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\PartProgramFunctions.h"

Conics3D::Conics3D(TCHAR* myname):ShapeWithList(genName(myname))
{
	this->ShapeType = RapidEnums::SHAPETYPE::ELLIPSE3D;
	init();
}

Conics3D::Conics3D(bool simply):ShapeWithList(false)
{
	this->ShapeType = RapidEnums::SHAPETYPE::ELLIPSE3D;
	init();
}

Conics3D::Conics3D(RapidEnums::SHAPETYPE stype):ShapeWithList(false)
{
	TCHAR* myname;
	this->ShapeType = stype;
	if (stype == RapidEnums::SHAPETYPE::ELLIPSE3D)
	{
		myname = _T("EL");
		this->eccentricity ( 0.5);
		this->startAngle (0); this->endAngle (2 * M_PI);
	}
	else if (stype == RapidEnums::SHAPETYPE::PARABOLA3D)
	{
		myname = _T("PB");
		this->eccentricity ( 1 );
		this->startAngle (0); this->endAngle (2 * M_PI);
	}
	else
	{
		myname = _T("HB");
		this->eccentricity ( 1.5 );
		this->startAngle(M_PI/2); this->endAngle(3*M_PI/2);
	}
	genName(myname);
	init();
}

Conics3D::Conics3D(TCHAR* myname, RapidEnums::SHAPETYPE stype):ShapeWithList(genName(myname))
{
	this->ShapeType = stype;
	if (stype == RapidEnums::SHAPETYPE::ELLIPSE3D)
	{
		this->eccentricity ( 0.5);
		this->startAngle (0); this->endAngle (2 * M_PI);
	}
	else if (stype == RapidEnums::SHAPETYPE::PARABOLA3D)
	{
		this->eccentricity ( 1 );
		this->startAngle (0); this->endAngle (2 * M_PI);
	}
	else
	{
		this->eccentricity ( 1.5 );
		this->startAngle(M_PI/2); this->endAngle(3*M_PI/2);
	}
	init();
}

Conics3D::~Conics3D()
{
}

void Conics3D::init()
{
	try
	{
		best_fit_succeeded = false;
		memset(best_fit_answer,0,sizeof(double)*14);
		this->dir_l(0); this->dir_m(0); this->dir_n(1); this->D(0);
		this->focus.set(0,0,0);
		Diretrix[0] = -1;
		for (int i =1; i<5; i++)
		{
			Diretrix[i] = 0;
		}
		Diretrix[5] = 1;
		dist_focus_diretrix (1);
		this->ShapeAs3DShape(true); this->IdConic(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON001", __FILE__, __FUNCSIG__); }
}

TCHAR* Conics3D::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		shapenumber++;
		_itot_s(shapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON002", __FILE__, __FUNCSIG__); return name; }
}

void Conics3D::UpdateBestFit()
{
	try
	{
		this->IsValid(false);
		int totcount = this->PointsList->Pointscount();
		if (totcount < 5) return;
		updateConic(totcount);
		ProbePointCalculations(totcount);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON003", __FILE__, __FUNCSIG__); }
}

 bool Conics3D::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
 {
	 try
	 {
		 return false;
	 }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON003", __FILE__, __FUNCSIG__); }
}


void Conics3D::updateConic(int PointsCount)
{
	try
	{
		//initialize with 0
		double ans[14] = {0};
		int i = 0;
		RBF::Conic conictype;

		if (this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
			conictype = RBF::Conic::Ellipse;
		else if (this->ShapeType == RapidEnums::SHAPETYPE::PARABOLA3D)
			conictype = RBF::Conic::Parabola;
		else
			conictype = RBF::Conic::Hyperbola;

		//if we have a previous valid best fit answer, use this as initial guess params, else proceed with 0 values in answer
		if ((best_fit_answer[0] != 0) || (best_fit_answer[1] != 0) || (best_fit_answer[2] != 0) )
			memcpy(ans,best_fit_answer,sizeof(double)*14);

		//check for return flag and return if false
		best_fit_succeeded = BESTFITOBJECT->ThreeDConic_BestFit(pts, PointsCount, &ans[0], conictype);
		if (!best_fit_succeeded)
			return;

		//store answer returned in successful best fit for next iteration if required
		memcpy(best_fit_answer,ans,sizeof(double)*14);

		//plane of conic
		dir_l(ans[0]); dir_m(ans[1]); dir_n(ans[2]); D(ans[3]);

		focus.set(ans[4], ans[5], ans[6]);

		//dir cosines of diretrix in 3, 4 and 5 elements of diretrix
		for (i = 7; i < 10; i++)
			Diretrix[i - 4] = ans[i];

		//point on diretrix closest to focus in 0, 1 and 2 elements of diretrix
		for (i = 10; i < 13; i++)
			Diretrix[i - 10] = ans[i];
	
		//eccentricity of conic
		eccentricity(ans[13]);

		//distance of diretrix from focus
		double dist = RMATH3DOBJECT->Distance_Point_Point(&ans[4], &ans[10]);

		dist_focus_diretrix(dist);

		//ray from diretrix to focus
		double ray1[3] = {ans[4] - ans[10], ans[5] - ans[11], ans[6] - ans[12]};
		double dir1[3];
		RMATH3DOBJECT->DirectionCosines(ray1, dir1);

		//Calculate 2nd Focal point for ellipse..
		if (this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
		{
			double Vertex[3] = {(eccentricity() * Diretrix[0] + focus.getX()) / (eccentricity() + 1), (eccentricity() * Diretrix[1] + focus.getY()) / (eccentricity() + 1), (eccentricity() * Diretrix[2] + focus.getZ()) / (eccentricity() + 1)};
			double dist1 = RMATH3DOBJECT->Distance_Point_Point(Vertex, &ans[4]);
			double MajorAxis_A = dist1 / (1 - eccentricity());
			focus2.set(2 * dir1[0] * MajorAxis_A * eccentricity() + focus.getX(), 2 * dir1[1] * MajorAxis_A * eccentricity() + focus.getY(), 2 * dir1[2] * MajorAxis_A * eccentricity() + focus.getZ());
		}

		//ray along diretrix
		double dir2[3] = {ans[7], ans[8], ans[9]};
		double dir3[3];
		double cos_angle;
		double angle;
		double projn;
		double min_angle = 2 * M_PI;
		double max_angle = 0;

		for(int i = 0; i < PointsCount; i++)
		{
			//get angle between ray from focal point to point and 
			//ray from diretrix point closest to focus to focal point.
			//get this by taking inverse cos of dot product of dir cosines of the two rays.
			//determine sign of angle based on sign of projection of the first ray above along diretrix.
			//find min and max of the angles and call these as end points of the conic (start angle and end angle).
			//if min and max are 0 and 2PI, then this is a closed curve (ie ellipse).
			double ray3[3] = {*(pts + 3*i) - ans[4], *(pts + 3*i + 1) - ans[5], *(pts + 3*i + 2) - ans[6]};
			RMATH3DOBJECT->DirectionCosines(ray3, dir3);
			cos_angle = 0;
			for (int i=0; i<3; i++)
				cos_angle += dir1[i] * dir3[i];
			angle = acos (cos_angle);
			projn = 0;
			for (int i=0; i<3; i++)
				projn += dir2[i] * dir3[i];
			if (projn < 0 )
				angle = 2 * M_PI - angle;
			if (angle < min_angle)
			{
				min_angle = angle;
				if (cos_angle > (1 / eccentricity()))
					min_angle = acos ((1 / eccentricity())) + 0.1;
			}
			if (angle > max_angle)
			{
				max_angle = angle;
				if (cos_angle > (1 / eccentricity()))
					max_angle = 2 * M_PI - acos ((1 / eccentricity())) - 0.1;
			}
		}
		startAngle(min_angle);
		endAngle(max_angle);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON004", __FILE__, __FUNCSIG__); }
}

void Conics3D::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt >= 5) // && PointsCnt < 10)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForProbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdConic();
		}
		if(ShapeDoneUsingProbe())
		{
			updateConic_UsingProbe();
			updateConic(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON005", __FILE__, __FUNCSIG__); }
}

void Conics3D::CheckPointIsForProbe()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON006", __FILE__, __FUNCSIG__); }
}

void Conics3D::CheckIdorOdConic()
{
	try
	{
		int IDCount = 0;
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(Spt->Pdir == 0) //FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT
			{
				if((focus.getX() - Spt->X) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 1) //FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT
			{
				if((focus.getX() - Spt->X) < 0)
					IDCount++;
				else
					IDCount--;
			}
			else if(Spt->Pdir == 2) //FramegrabBase::PROBEDIRECTION::YTOP2BOTTOM
			{
				if((focus.getY() - Spt->Y) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 3) // FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP
			{
				if((focus.getY() - Spt->Y) < 0)
					IDCount++;
				else
					IDCount--;
			}
		}
		if(IDCount > 0)
			this->IdConic(true);
		else
			this->IdConic(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON007", __FILE__, __FUNCSIG__); }
}


void Conics3D::updateConic_UsingProbe()
{
	try
	{
		double Pt[3];;
		double Pradius = MAINDllOBJECT->ProbeRadius();
		int mulfactor = -1;
		if (IdConic())
			mulfactor = 1;
		double ConicParam[17];
		//first 4 are conic plane coordinates
		//next 3 are conic focus coordinates
		//next 6 are diretrix line with first 3 being pt on diretrix closest to focus and next 3 being dir cosines of diretrix line
		//next parameter (13) is ecccentricity, 14 and 15 are start and end angles and 16 is distance from focus to diretrix
		this->getParameters(ConicParam);
		//ray's are direted lines and the corresponding dir's are the normalized unit vectors along the rays
		double ray1[3], dir1[3], dir2[3], ray3[3], dir3[3], ray4[3], dir4[3];
		//get dir cosines of perpendicular to diretrix on conic plane.
		for (int i=0; i<3; i++)
			ray3[i] = ConicParam[4+i] - ConicParam[7+i];
		RMATH3DOBJECT->DirectionCosines(ray3, dir3);

		double cos_angle, angle, projn;
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};
			//find e_r, the unit vector from focus to probe point
			//find e_theta as cross product of unit vector perpendicular to plane and e_r
			//for a point on conic which is at an angular coordinate of theta (angle between ray from
			//focus to point and angle of ray from diretrix perpendicular to diretrix through focus),
			//outward normal to conic on plane of conic is e_r ( 1/e - cos theta) + e_theta (sin theta).
			//use this to determine corrections to probe point.

			//get e_r first
			for (int i = 0; i < 3; i++)
				ray1[i] = PrPoint[i] - ConicParam[4+i];
			RMATH3DOBJECT->DirectionCosines(ray1, dir1);

			//get e_theta now.
			dir2[0] = ConicParam[1] * dir1[2] - ConicParam[2] * dir1[1];
			dir2[1] = ConicParam[2] * dir1[0] - ConicParam[0] * dir1[2];
			dir2[2] = ConicParam[0] * dir1[1] - ConicParam[1] - dir1[0];
			
			//get theta, polar coordinate of probe point wrt diretrix pt to focal pt line, next
			cos_angle = 0;
			for (int i=0; i<3; i++)
				cos_angle += dir1[i] * dir3[i];
			angle = acos (cos_angle);
			//get projection of point along directrix to determine sign of angle
			projn = 0;
			for (int i=0; i<3; i++)
				projn += dir1[i] * ConicParam[10 + i];
			if (projn < 0 )
				angle = 2 * M_PI - angle;

			//outward normal calculation now
			for (int i=0; i < 3; i++)
				ray4[i] = dir1[i] * ( 1/ConicParam[13] - cos_angle) + dir2[i] * sin (angle);
			RMATH3DOBJECT->DirectionCosines(ray4, dir4);

			for (int i= 0; i < 3; i++)
				Pt[i] = PrPoint[i] + Pradius * mulfactor * dir4[i];

			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(Pt[0], Pt[1], Pt[2], Spt->R, Spt->Pdir);
		}

		if(this->pts != NULL){free(pts); pts = NULL;}
		    this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
	    	this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		filterptsCnt = 0;
		int n = 0, j = 0;
		for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			this->pts[n++] = Pt->X;
			this->pts[n++] = Pt->Y;
			this->pts[n++] = Pt->Z;
			if(Pt->InValid) continue;
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			this->filterpts[j++] = Pt->Z;
			filterptsCnt++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON008", __FILE__, __FUNCSIG__); }
}

void Conics3D::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		//do nothing for now
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON009", __FILE__, __FUNCSIG__); }
}

void Conics3D::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		double ConicParams[17];
		//first 4 are conic plane coordinates
		//next 3 are conic focus coordinates
		//next 6 are diretrix line with first 3 being pt on diretrix closest to focus and next 3 being dir cosines of diretrix line
		//next parameter (13) is ecccentricity, 14 and 15 are start and end angles and 16 is distance from focus to diretrix
		getParameters(ConicParams);
		//drawCurrentShape complete conic if ellipse, else only from start to end angle.
		if ((ConicParams[13] < 1) && (this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D))
		{
			ConicParams[14] = 0;
			ConicParams[15] = 2 * M_PI;
		}
		GRAFIX->drawConic3D(ConicParams);
		GRAFIX->drawPoint(focus.getX(), focus.getY(),focus.getZ());
		if (this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
			GRAFIX->drawPoint(focus2.getX(), focus2.getY(),focus2.getZ());
		GRAFIX->drawXLineOrXRay3D(Diretrix, WPixelWidth, true, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON011", __FILE__, __FUNCSIG__); }
}

void Conics3D::drawGDntRefernce()
{
	try
	{
		//do nothing for now
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON012", __FILE__, __FUNCSIG__); }
}

bool Conics3D::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
		return Shape_IsInWindow(&SelectionL[0], Tolerance);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON014", __FILE__, __FUNCSIG__); return false; }
}

bool Conics3D::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool Conics3D::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		double ThisConic[17];
		//first 4 are conic plane coordinates
		//next 3 are conic focus coordinates
		//next 6 are diretrix line with first 3 being pt on diretrix closest to focus and next 3 being dir cosines of diretrix line
		//next parameter (13) is ecccentricity, 14 and 15 are start and end angles and 16 is distance from focus to diretrix
		getParameters(ThisConic);
		double ProjctnPt[3];
		double PlaneOfArc[4] = {dir_l(), dir_m(), dir_n(), D()};

		//find intersection point of selection line with plane of conic
		Intersects = RMATH3DOBJECT->Intersection_Line_Plane(SelectionLine, PlaneOfArc, IntersectnPts);
		
		if (Intersects != 1) return false;

		double ray1[3], dir1[3], ray2[3], dir2[3];
		//get dir cosines of perpendicular to diretrix on conic plane.
		for (int i=0; i<3; i++)
			ray1[i] = ThisConic[4+i] - ThisConic[7+i];
		RMATH3DOBJECT->DirectionCosines(ray1, dir1);
		//get dir cosines of ray from focus to intersection point
		for (int i=0; i<3; i++)
			ray2[i] = IntersectnPts[i] - ThisConic[4+i];
		RMATH3DOBJECT->DirectionCosines(ray2, dir2);
		//find cos of angle of ray from focus to intersection point w.r.t. diretrix to focus ray
		double cos_angle = 0;
		for (int i=0; i<3; i++)
			cos_angle += dir1[i] * dir2[i];
		//find radial distance of conic at this angle using formula f_d/(1/e - cos theta),
		//where f_d is distance from diretrix to focus, e is eccentricity.
		double den_factor = 1/ThisConic[13] - cos_angle;

		if (den_factor <=0 ) return false;

		double conic_radial_dist = ThisConic[16] /den_factor;
		double radial_dist = RMATH3DOBJECT->Distance_Point_Point(IntersectnPts, &ThisConic[4]);

		//if this radial distance is within tolerance from distance between intersection point and
		//focal point, return true, else false.
		if (abs(conic_radial_dist - radial_dist) < Tolerance)
			return true;

		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON015", __FILE__, __FUNCSIG__); return false; }
}

//first 4 are conic plane coordinates
//next 3 are conic focus coordinates
//next 6 are diretrix line with first 3 being pt on diretrix closest to focus and next 3 being dir cosines of diretrix line
//next parameter (13) is ecccentricity, 14 and 15 are start and end angles and 16 is distance from focus to diretrix
void Conics3D::getParameters(double* Conicparam)
{
	//get conic parameters
	Conicparam[0] = dir_l(); Conicparam[1] = dir_m(); Conicparam[2] = dir_n(); Conicparam[3] = D();
	Conicparam[4] = focus.getX(); Conicparam[5] = focus.getY(); Conicparam[6] = focus.getZ();
	for (int i = 0; i < 6; i++)
	{
		Conicparam[7+i] = Diretrix[i];
	}
	Conicparam[13] = eccentricity(); Conicparam[14] = startAngle(); Conicparam[15] = endAngle();
	Conicparam[16] = dist_focus_diretrix();
}

//first 4 are conic plane coordinates
//next 3 are conic focus coordinates
//next 6 are diretrix line with first 3 being pt on diretrix closest to focus and next 3 being dir cosines of diretrix line
//next parameter (13) is ecccentricity, 14 and 15 are start and end angles and 16 is distance from focus to diretrix
void Conics3D::setParameters(double* Conicparam)
{
	//set conic parameters
	dir_l(Conicparam[0]); dir_m(Conicparam[1]); dir_n(Conicparam[2]); D(Conicparam[3]);
	focus.set(Conicparam[4], Conicparam[5], Conicparam[6]);
	for (int i = 0; i < 6; i++)
	{
		Diretrix[i] = Conicparam[7+i];
	}
	eccentricity(Conicparam[13]); startAngle(Conicparam[14]); endAngle(Conicparam[15]);
	dist_focus_diretrix(Conicparam[16]);
}

void Conics3D::ResetShapeParameters()
{
	try
	{
		//set conic parameters to default
		dir_l(0); dir_m(0); dir_n(1); D(0); focus.set(0,0,0);
		Diretrix[0] = -1;
		for (int i =1; i<5; i++)
		{
			Diretrix[i] = 0;
		}
		Diretrix[5] = 1;
		if (this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
		{
			eccentricity (0.5); startAngle(0); endAngle(2 * M_PI);
		}
		else if (this->ShapeType == RapidEnums::SHAPETYPE::PARABOLA3D)
		{
			eccentricity (1); startAngle(0); endAngle(2 * M_PI);
		}
		else
		{
			eccentricity (1.5); startAngle(M_PI/2); endAngle(3*M_PI/2);
		}
		dist_focus_diretrix (1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0020", __FILE__, __FUNCSIG__); }
}

Shape* Conics3D::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
		{
			if (this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
				myname = _T("CLE");
			else if (this->ShapeType == RapidEnums::SHAPETYPE::PARABOLA3D)
				myname = _T("CLP");
			else
				myname = _T("CLH");
		}
		else
		{
			if (this->ShapeType == RapidEnums::SHAPETYPE::ELLIPSE3D)
				myname = _T("dCLE");
			else if (this->ShapeType == RapidEnums::SHAPETYPE::PARABOLA3D)
				myname = _T("dCLP");
			else
				myname = _T("dCLH");
		}
		Conics3D* CShape = new Conics3D((TCHAR*) myname.c_str(), this->ShapeType);
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON017", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Conics3D::CreateDummyCopy()
{
	try
	{
		Conics3D* CShape = new Conics3D(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON018", __FILE__, __FUNCSIG__); return NULL; }
}

void Conics3D::CopyShapeParameters(Shape* s)
{
	try
	{
		double ConicParams[17];
		((Conics3D*) s)->getParameters(ConicParams);
		this->setParameters(ConicParams);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON019", __FILE__, __FUNCSIG__); }
}

void Conics3D::Transform(double* transform)
{
	try
	{
		//do nothing for now
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON020", __FILE__, __FUNCSIG__); }
}

void Conics3D::Translate(Vector& Position)
{
	//do nothing for now
}

void Conics3D::AlignToBasePlane(double* trnasformM)
{
}

void Conics3D::UpdateEnclosedRectangle()
{
}

void Conics3D::GetShapeCenter(double *cPoint)
{
	cPoint[0] = focus.getX();
	cPoint[1] = focus.getY();
	cPoint[2] = focus.getZ();
}

int Conics3D::shapenumber = 0;

void Conics3D::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, Conics3D& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os<< "Conisc3d" <<endl;
		os << "ShapeType:" << x.ShapeType << endl;
		double ConicParams[17];
		x.getParameters(ConicParams);
		for (int i = 0; i < 17; i++)
		{
			os << "ConicParams:" << ConicParams[i] << endl;
		}
		os<< "EndConisc3d" <<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON021", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Conics3D& x)
{
	try
	{
		int countConicParams=0;
		double ConicParams[17];
		is >> (*(Shape*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"Conisc3d")
			{
			while(Tagname!=L"EndConisc3d")
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
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname=Tag;					
						if(Tagname==L"ShapeType")
						{
							x.ShapeType = RapidEnums::SHAPETYPE(atoi((const char*)(Val).c_str()));						
						}
						else if(Tagname==L"ConicParams")
						{
							ConicParams[countConicParams] = atof((const char*)(Val).c_str());
							countConicParams++;
						}
					}
				}		
				x.setParameters(ConicParams);
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}				
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CON022", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, Conics3D& x)
{
	try
	{
		int n; double d;
		is >> n;
		x.ShapeType = RapidEnums::SHAPETYPE(n);
		double ConicParams[17];
		for (int i = 0; i < 17; i++)
		{
			is >> d; ConicParams[i] = d;
		}
		x.setParameters(ConicParams);
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CON022", __FILE__, __FUNCSIG__); }
}