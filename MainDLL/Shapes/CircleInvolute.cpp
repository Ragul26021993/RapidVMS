#include "StdAfx.h"
#include "CircleInvolute.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

CircleInvolute::CircleInvolute(TCHAR* myname):ShapeWithList(genName(myname))
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CircleInvolute0001", __FILE__, __FUNCSIG__);}
}

CircleInvolute::CircleInvolute(bool simply):ShapeWithList(false)
{
	init();
}

CircleInvolute::~CircleInvolute()
{
}

void CircleInvolute::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::CIRCLEINVOLUTE;
		this->startAngle(0);
		this->endAngle(0);
		this->startRadius(0);
		this->endRadius(0);
		this->ShapeAs3DShape(false); 
		this->UseLightingProperties(false);
		this->firstTime = true;		
		this->IsValid(false);
		this->IdInvolute(false);
	}
	catch(...){}
}

TCHAR* CircleInvolute::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CircleInvolute0002", __FILE__, __FUNCSIG__); return name; }
}

void CircleInvolute::UpdateBestFit()
{
	try
	{
		this->IsValid(false);
		int totcount = this->PointsList->Pointscount();
		if (totcount < 5) return;
		if(updateCircleInvolute(totcount))
		{
			DrawCircleInvolute = true;
			this->IsValid(true);
		}
		ProbePointCalculations(totcount);
		this->IsValid(true);
   }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CircleInvolute0003", __FILE__, __FUNCSIG__); }
}

bool CircleInvolute::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CircleInvolute0004", __FILE__, __FUNCSIG__); }
}

bool CircleInvolute::updateCircleInvolute(int PointsCount)
{
	try
	{
		double ans[4] = {0};		
		BESTFITOBJECT->CircleInvolute_BestFit(pts,PointsCount,&ans[0]);		
		this->Center.set(ans[0],ans[1],0);		
		CircleRadius(ans[2]);		
		
		double dir1[3] = {1,0,0};//Direction cosine of x axis;
		double dir3[3];		
		double centerpoint[3]={ans[0],ans[1],0};
		double max_dist=0 , min_dist;
		startRadius(ans[2]);
		bool firstpoint = true;

		for(int i = 0; i < PointsCount; i++)
		{
			double cos_angle=0;
			double angle;

			double ray[3]={*(pts + 2*i)-ans[0],*(pts + 2*i + 1)-ans[1],0};			
			double pointpos[2] = {*(pts + 2*i) , *(pts + 2*i + 1)};

			double dist = RMATH2DOBJECT->Pt2Pt_distance(pointpos[0] , pointpos[1] , centerpoint[0] , centerpoint[1]);

			RMATH3DOBJECT->DirectionCosines(ray, dir3);
			if(firstpoint)
			{
				min_dist=dist;
				max_dist=dist;				
				for(int j=0;j<3;j++)
					cos_angle+=dir1[j]*dir3[j];
				angle = acos (cos_angle);				
				startAngle(ans[2]);
				endAngle(angle);
				firstpoint=false;
			}
			else
			{
				if(dist>max_dist)
				{
					max_dist=dist;		
					for(int j=0;j<3;j++)
						cos_angle+=dir1[j]*dir3[j];
					angle = acos (cos_angle);
					endAngle(angle);
					endRadius(max_dist);		
				}
				/*if(dist<min_dist)
				{
					min_dist=dist;
					for(int j=0;j<3;j++)
						cos_angle+=dir1[j]*dir3[j];
					angle = acos (cos_angle);
					//startAngle(angle);
					startRadius(ans[2]);
				}*/
			}
		}		
		return true;
	}
	catch(exception ex){ MAINDllOBJECT->SetAndRaiseErrorMessage("CircleInvolute0005", __FILE__, __FUNCSIG__); }
}

void CircleInvolute::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt >= 5)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForProbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdCircleInvolute();
		}
		if(ShapeDoneUsingProbe())
		{
			updateCircleInvolute_UsingPorbe();
			updateCircleInvolute(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CircleInvolute0005", __FILE__, __FUNCSIG__); }
}

void CircleInvolute::CheckPointIsForProbe()
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

void CircleInvolute::CheckIdorOdCircleInvolute()
{
	try
	{
		int IDCount = 0;
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(Spt->Pdir == 0) //FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT
			{
				if((Center.getX() - Spt->X) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 1) //FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT
			{
				if((Center.getX() - Spt->X) < 0)
					IDCount++;
				else
					IDCount--;
			}
			else if(Spt->Pdir == 2) //FramegrabBase::PROBEDIRECTION::YTOP2BOTTOM
			{
				if((Center.getY() - Spt->Y) < 0)
					IDCount--;
				else
					IDCount++;
			}
			else if(Spt->Pdir == 3) // FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP
			{
				if((Center.getY() - Spt->Y) < 0)
					IDCount++;
				else
					IDCount--;
			}
		}
		if(IDCount > 0)
			this->IdInvolute(true);
		else
			this->IdInvolute(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON007", __FILE__, __FUNCSIG__); }
}

void CircleInvolute::updateCircleInvolute_UsingPorbe()
{
	try
	{
		double Pt[3];
		double Pradius = MAINDllOBJECT->ProbeRadius();
		int mulfactor = -1;
		if (IdInvolute())
			mulfactor = 1;
		double Involuteparam[8];	

		this->getParameters(Involuteparam);		

		double ray1[3], dir1[3] = {1,0,0}, dir2[3] , dist;

		double cos_angle, angle, projn;

		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};

			for (int i = 0; i < 3; i++)
				ray1[i] = PrPoint[i] - Involuteparam[i];
			RMATH3DOBJECT->DirectionCosines(ray1, dir2);
			angle = RMATH2DOBJECT->ray_angle(Involuteparam[0], Involuteparam[1], PrPoint[0],PrPoint[1]);

			/*cos_angle = 0;
			for (int i=0; i<3; i++)
				cos_angle += dir1[i] * dir2[i];
			angle = acos (cos_angle);*/
			
			//outward normal calculation now			
			for (int i= 0; i < 3; i++)
				Pt[i] = PrPoint[i] + Pradius * mulfactor * cos(angle);

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
			if(Pt->InValid)continue;
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			this->filterpts[j++] = Pt->Z;
			filterptsCnt++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON008", __FILE__, __FUNCSIG__); }
}

void CircleInvolute::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON011", __FILE__, __FUNCSIG__); }
}

void CircleInvolute::drawCurrentShape(int windowno, double WPixelWidth)
{
	if(this->DrawCircleInvolute)
	{
		double Involuteparam[8];		
		getParameters(Involuteparam);
		
		GRAFIX->DrawCircleInvoluteShape(Involuteparam);
		GRAFIX->drawPoint(Center.getX(), Center.getY(),Center.getZ());
	}
}

void CircleInvolute::drawGDntRefernce()
{
}

void CircleInvolute::getParameters(double* Involuteparam)
{
	//get Involute parameters	
	Involuteparam[0] = this->Center.getX(); Involuteparam[1] = this->Center.getY(); Involuteparam[2] = 0;
	Involuteparam[3] = CircleRadius(); 
	Involuteparam[4] = startAngle();  Involuteparam[5] = endAngle();
	Involuteparam[6] = startRadius(); Involuteparam[7] = endRadius();
}

void CircleInvolute::setParameters(double* Involuteparam)
{
	//set conic parameters
	Center.set(Involuteparam[0],Involuteparam[1],Involuteparam[2]);
	CircleRadius(Involuteparam[3]); 
	startAngle(Involuteparam[4]); endAngle(Involuteparam[5]);
	startRadius(Involuteparam[6]); endRadius(Involuteparam[7]);
}

bool CircleInvolute::Shape_IsInWindow(Vector& corner1,double Tolerance)
{
	return false;
}

bool CircleInvolute::Shape_IsInWindow(Vector& corner1,Vector& corner2)
{
	return false;
}

bool CircleInvolute::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

Shape* CircleInvolute::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
		{			
			myname = _T("CIV");
		}
		else
		{		
			myname = _T("DCIV");
		}
		CircleInvolute* CShape = new CircleInvolute((TCHAR*) myname.c_str());
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON017", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* CircleInvolute::CreateDummyCopy()
{
	try
	{
		CircleInvolute* CShape = new CircleInvolute(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0018", __FILE__, __FUNCSIG__); return NULL; }
}

void CircleInvolute::CopyShapeParameters(Shape* s)
{
	try
	{
		double Involuteparam[8];
		((CircleInvolute*) s)->getParameters(Involuteparam);
		this->setParameters(Involuteparam);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CON019", __FILE__, __FUNCSIG__); }
}

void CircleInvolute::Translate(Vector& Position)
{
	//do nothing for now
}

void CircleInvolute::Transform(double* transform)
{
}

void CircleInvolute::ResetShapeParameters()
{
	try
	{
	this->Center.set(0,0,0);
	CircleRadius(2);
	startAngle(0);  endAngle(M_PI);
	startRadius(2); endRadius(2*M_PI);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0020", __FILE__, __FUNCSIG__); }
}

void CircleInvolute::AlignToBasePlane(double* TrnasformM)
{
}
void CircleInvolute::UpdateEnclosedRectangle()
{
}

void CircleInvolute::GetShapeCenter(double *cPoint)
{
	cPoint[0] = Center.getX();
	cPoint[1] = Center.getY();
	cPoint[2] = 0;
}

int CircleInvolute::shapenumber = 0;
void CircleInvolute::reset()
{
	shapenumber = 0;
}