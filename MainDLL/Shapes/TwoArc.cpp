#include "stdafx.h"
#include "TwoArc.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

double AvgSweepAngleOfBoth = 0.0;			//avg Sweep Angle for two Arc... vinod
TwoArc::TwoArc(TCHAR* myname):ShapeWithList(genName(myname))
{
	try
	{
	    init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0001", __FILE__, __FUNCSIG__);}
}

TwoArc::TwoArc(bool simply):ShapeWithList(false)
{
	init();
}

TwoArc::~TwoArc()
{

}

void TwoArc::init()
{
	try
	{
	    this->ShapeType = RapidEnums::SHAPETYPE::TWOARC;
		this->Angle(0);
		this->Radius1(0);
		this->Radius2(0);
		this->ShapeAs3DShape(false); 
		this->UseLightingProperties(false);
		this->firstTime = true;
		this->ReflectionLine = NULL;
		this->Arc1 = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
		this->Arc1->CircleType = RapidEnums::CIRCLETYPE::ARC_INTWOARC;
		this->Arc2 = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
	    this->Arc2->CircleType = RapidEnums::CIRCLETYPE::ARC_INTWOARC;
        this->AddReflectionLine = true; 
		this->IsValid(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0002", __FILE__, __FUNCSIG__); }
}

TCHAR* TwoArc::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0003", __FILE__, __FUNCSIG__); return name; }
}

void TwoArc::UpdateBestFit()
{
	try
	{
		int totcount = this->PointsListOriginal->Pointscount();
		if(totcount < 6) return;
		if(ReflectionLine == NULL)
		{
			if(PPCALCOBJECT->IsPartProgramRunning())
			   AssignParamForGroupShapes();
			else
			{
				ReflectionLine = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
				ReflectionLine->LineType = RapidEnums::LINETYPE::LINEINTWOARC;
			}
		}
		if(!updateTwoArc(totcount))
		{
			if(Radius1() > 0)this->IsValid(true);
			else IsValid(false);
			return;
		}
		else
		{
		   this->notifyAll(ITEMSTATUS::DATACHANGED, this);	
		   Arc1->IsValid(true);
		   Arc1->notifyAll(ITEMSTATUS::DATACHANGED, Arc1);	
		   Arc2->IsValid(true);
		   Arc2->notifyAll(ITEMSTATUS::DATACHANGED, Arc2);	
		   ReflectionLine->IsValid(true);
		   ReflectionLine->notifyAll(ITEMSTATUS::DATACHANGED, ReflectionLine);	
		   ProbePointCalculations(totcount);
	       if(firstTime) addGroupShapes();
		}
		IsValid(true);
   }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0004", __FILE__, __FUNCSIG__); }
}

bool TwoArc::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	return false;
}

bool TwoArc::updateTwoArc(int PointsCount)
{
	try
	{   
		double arcAngles[2], ans[5] = {0}, arcCenters[4], reflectionLineEndPts[4], endpnts[4];
		double Dir[3], ZDir1[3] = {0, 0, 1},  intercept;
		bool bestFitFlag;  Vector tmpV;
		
		
		if(AddReflectionLine)
			bestFitFlag = BESTFITOBJECT->TwoArc_BestFit(this->pts, PointsCount, &ans[0]);
		else
		{
			ans[0] = ReflectionLine->getMidPoint()->getX(); ans[1] = ReflectionLine->getMidPoint()->getY(); ans[2] = ReflectionLine->Angle();
			bestFitFlag = BESTFITOBJECT->TwoArc_BestFit(this->pts, PointsCount, &ans[0],true);
		}
		
		if(this->ApplyFilterCorrection() && PointsCount > 6)								//is there any need for fitering the points...?
		{
			int cnt = PointsCount;
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
			{
				double Arc1Param[7] = {0}, Arc2Param[7] = {0};
				this->Arc1->getParameters(&Arc1Param[0]);
				this->Arc2->getParameters(&Arc2Param[0]);
				
		 
				double twoarcparam[] = {Arc1Param[0], Arc1Param[1], Arc1Param[6], Arc2Param[0], Arc2Param[1], Arc2Param[6]};	//(x1,y1,radius1,x2,y2,radius2)
				cnt = HELPEROBJECT->FilteredTwoArc(this->PointsList, twoarcparam, FilterCorrectionFactor(), i);			//for filtering points ...... vinod
				if(cnt < 7) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		        filterptsCnt = 0; 
				int n = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					if(((*Spt).second)->InValid) continue;
					SinglePoint* Pt = (*Spt).second;
					filterpts[n++] = Pt->X;
					filterpts[n++] = Pt->Y;
					filterptsCnt++;
				}
				if(AddReflectionLine)
					 bestFitFlag = BESTFITOBJECT->TwoArc_BestFit(this->filterpts, filterptsCnt, &ans[0]);
				else
				{
					ans[0] = ReflectionLine->getMidPoint()->getX(); ans[1] = ReflectionLine->getMidPoint()->getY(); ans[2] = ReflectionLine->Angle();
					bestFitFlag = BESTFITOBJECT->TwoArc_BestFit(this->pts, PointsCount, &ans[0],true);
				}
			}
		}
			
	
		if(!bestFitFlag) return false;
		this->Center.set(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
		this->Angle(ans[2]);
		this->Radius1(ans[3]);
		this->Radius2(ans[4]);
		double reflectionLineDir[3] = {cos(Angle()), sin(Angle()), 0};
		if(AddReflectionLine)
		{
			this->ReflectionLine->Angle(ans[2]);
			RMATH2DOBJECT->Intercept_LinePassing_Point(ans, ans[2], &intercept);
			this->ReflectionLine->Intercept(intercept);
			RMATH2DOBJECT->EquiDistPointsOnLine(Angle(), 1, ans, reflectionLineEndPts, &reflectionLineEndPts[2], 0);
			tmpV.set(reflectionLineEndPts[0], reflectionLineEndPts[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->ReflectionLine->setPoint1(tmpV);
			tmpV.set(reflectionLineEndPts[2], reflectionLineEndPts[3], MAINDllOBJECT->getCurrentDRO().getZ()); 
			this->ReflectionLine->setPoint2(tmpV);
		}
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(ZDir1, reflectionLineDir, Dir);
		for(int i = 0; i < 2; i++)
		{
			arcCenters[i] = ans[i] + Radius1() * Dir[i];
			arcCenters[2 + i] = ans[i] - Radius1() * Dir[i];
		}
		int PointsCnt1 = 0, PointsCnt2 = 0;
		double *tmppts1 = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		double *tmppts2 = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);


		for(int i = 0; i < this->PointsListOriginal->Pointscount(); i++)
		{
		   if((RMATH2DOBJECT->Pt2Pt_distance(pts + 2 * i, arcCenters)) < (RMATH2DOBJECT->Pt2Pt_distance(pts + 2 * i, &arcCenters[2])))
		   {
			   tmppts1[PointsCnt1++] = pts[2 * i];
               tmppts1[PointsCnt1++] = pts[2 * i + 1];
		   }
		   else
		   {
			   tmppts2[PointsCnt2++] = pts[2 * i];
               tmppts2[PointsCnt2++] = pts[2 * i + 1];
		   }
		}
		SetSubArcParam(tmppts1, &arcCenters[0], &ans[4], PointsCnt1, this->Arc1);
		SetSubArcParam(tmppts2, &arcCenters[2], &ans[4], PointsCnt2, this->Arc2);
     	ReflectionLine->calculateAttributes();
	    free(tmppts1); tmppts1 = NULL;
		free(tmppts2); tmppts2 = NULL;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0005", __FILE__, __FUNCSIG__); return false;}
}

void TwoArc::addGroupShapes()
{
	try
	{
		if(AddReflectionLine)
		{
		    addGroupChild(ReflectionLine);
		}
		addGroupChild(Arc1);  
		addGroupChild(Arc2);  
		ReflectionLine->addGroupParent(this);
	 	Arc1->addGroupParent(this); 
		Arc2->addGroupParent(this);  

		this->ChildsId.push_back(ReflectionLine->getId());
		this->ChildsId.push_back(Arc1->getId());
		this->ChildsId.push_back(Arc2->getId());
		if(AddReflectionLine)
	        AddShapeAction::addShape(ReflectionLine, false);
		AddShapeAction::addShape(Arc1, false);
		AddShapeAction::addShape(Arc2, false);
		firstTime = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0006", __FILE__, __FUNCSIG__);}
}

void TwoArc::updateParameters()
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0006", __FILE__, __FUNCSIG__);}
}

void TwoArc::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt == 6)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				CheckIdorOdTwoArc();
		}
		if(ShapeDoneUsingProbe())
		{
			updateTwoArc_UsingPorbe();
			updateTwoArc(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0005", __FILE__, __FUNCSIG__); }
}

void TwoArc::CheckPointIsForPorbe()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0007", __FILE__, __FUNCSIG__); }
}

void TwoArc::CheckIdorOdTwoArc()
{
	try
	{
		SinglePoint* Spt = (SinglePoint*)(*PointsList->getList().begin()).second;
		double *tmpCenter, tempPoint[2] = {Spt->X, Spt->Y}, center1[2] = {Arc1->getCenter()->getX(), Arc1->getCenter()->getY()},  center2[2] = {Arc2->getCenter()->getX(), Arc2->getCenter()->getY()};
		if((RMATH2DOBJECT->Pt2Pt_distance(center1, tempPoint)) < (RMATH2DOBJECT->Pt2Pt_distance(center2, tempPoint)))
			tmpCenter = center1;
		else
			tmpCenter = center2;
		
		if(Spt->Pdir == 0) //FramegrabBase::PROBEDIRECTION::XRIGHT2LEFT
		{
			if((tmpCenter[0] - tempPoint[0]) < 0)
				IdCircle(false);
			else
				IdCircle(true);
		}
		else if(Spt->Pdir == 1) //FramegrabBase::PROBEDIRECTION::XLEFT2RIGHT
		{
			if((tmpCenter[0] - tempPoint[0]) < 0)
				IdCircle(true);
			else
				IdCircle(false);
		}
		else if(Spt->Pdir == 2) //FramegrabBase::PROBEDIRECTION::YTOP2BOTTOM
		{
			if((tmpCenter[1] - tempPoint[1]) < 0)
				IdCircle(false);
			else
				IdCircle(true);
		}
		else if(Spt->Pdir == 3) // FramegrabBase::PROBEDIRECTION::YBOTTOM2TOP
		{
			if((tmpCenter[1] - tempPoint[1]) < 0)
				IdCircle(true);
			else
				IdCircle(false);
		}
		else if(Spt->Pdir == 4) 
		{
			IdCircle(false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0014", __FILE__, __FUNCSIG__); }

}

void TwoArc::updateTwoArc_UsingPorbe()
{
	try
	{
		double Pt[3], Pradius;
		if(IdCircle())
			Pradius = MAINDllOBJECT->ProbeRadius();
		else
			Pradius = -MAINDllOBJECT->ProbeRadius();
		double rangle, PtX, PtY;
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double *tmpCenter, tempPoint[2] = {Spt->X, Spt->Y}, center1[2] = {Arc1->getCenter()->getX(), Arc1->getCenter()->getY()},  center2[2] = {Arc2->getCenter()->getX(), Arc2->getCenter()->getY()};
		    if((RMATH2DOBJECT->Pt2Pt_distance(center1, tempPoint)) < (RMATH2DOBJECT->Pt2Pt_distance(center2, tempPoint)))
			  tmpCenter = center1;
		    else
			  tmpCenter = center2;
			rangle = RMATH2DOBJECT->ray_angle(tmpCenter[0], tmpCenter[1], tempPoint[0],tempPoint[1]);
			PtX = tempPoint[0] + cos(rangle) * Pradius;
			PtY = tempPoint[1] + sin(rangle) * Pradius;
			SinglePoint* Spt1 =  this->PointsListOriginal->getList()[Spt->PtID];
			Spt1->SetValues(PtX, PtY, Spt->Z, Spt->R, Spt->Pdir);
		}
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
	    this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		filterptsCnt = 0;
		int n = 0, j = 0;
		for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0008", __FILE__, __FUNCSIG__); }
}

void TwoArc::UpdateForParentChange(BaseItem* sender)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0009", __FILE__, __FUNCSIG__); }
}

void TwoArc::UpdateEnclosedRectangle()
{
	try
	{
		if(Arc1 != NULL && Arc2 != NULL && ReflectionLine != NULL)
		{
		   Arc1->UpdateEnclosedRectangle();
		   Arc2->UpdateEnclosedRectangle();
		   ReflectionLine->UpdateEnclosedRectangle();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0019", __FILE__, __FUNCSIG__); }
}

void TwoArc::GetShapeCenter(double *cPoint)
{
	cPoint[0] = Center.getX();
	cPoint[1] = Center.getY();
	cPoint[2] = Center.getZ();
}

void TwoArc::SetSubArcParam(double *tmpPnts, double *center, double *radius, int pointsCount, Circle* CurrentArc)    //vinod we use this fn to again set 1st arc's sweepangle to avg sweepangle
{
	try
	{
		if(pointsCount < 6) return;
	    double arcAngles[2], endpnts[4];
		Vector tmpV;
	    if((pointsCount / 2) == 3)
		{
			RMATH2DOBJECT->Arc_3Pt(tmpPnts, tmpPnts + 4, tmpPnts + 2, center, radius, arcAngles, &arcAngles[1]);
		}
		else
		{
			RMATH2DOBJECT->ArcBestFitAngleCalculation(tmpPnts, pointsCount / 2, center, *radius, arcAngles);				//18/02/2014
		}		
		RMATH2DOBJECT->ArcEndPoints(&center[0], *radius, arcAngles[0], arcAngles[1], endpnts);
		tmpV.set(center[0], center[1], MAINDllOBJECT->getCurrentDRO().getZ());
		CurrentArc->setCenter(tmpV);
		CurrentArc->Radius(*radius);
		tmpV.set(endpnts[0],endpnts[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
		CurrentArc->setendpoint1(tmpV);
		tmpV.set(endpnts[2],endpnts[3], MAINDllOBJECT->getCurrentDRO().getZ()); 
		CurrentArc->setendpoint2(tmpV);		
		CurrentArc->Startangle(arcAngles[0]); 
		CurrentArc->Sweepangle(arcAngles[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0010a", __FILE__, __FUNCSIG__); }
}
void TwoArc::UpdateForSubshapeChange(Shape *childShape)
{
	try
	{
		if(!this->getGroupChilds().empty())
		{
			if(this->pts != NULL){free(pts); pts = NULL;}
				this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
			if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
			filterptsCnt = 0;
		    int n = 0, j = 0;
			this->PointsListOriginal->CopyAllPoints(((ShapeWithList *)childShape)->PointsList);
			((ShapeWithList *)childShape)->PointsList->deleteAll();
		
			for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
			{
				SinglePoint* Pt = (*Spt).second;
				this->pts[n++] = Pt->X;
				this->pts[n++] = Pt->Y;
				this->filterpts[j++] = Pt->X;
				this->filterpts[j++] = Pt->Y;
				filterptsCnt++;
			}
			UpdateBestFit();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0010", __FILE__, __FUNCSIG__); }
}

void TwoArc::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
	 	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0011", __FILE__, __FUNCSIG__); }
}

void TwoArc::drawGDntRefernce()
{

}

void TwoArc::ResetShapeParameters()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0013", __FILE__, __FUNCSIG__); }
}

bool TwoArc::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
	   	return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0014", __FILE__, __FUNCSIG__); return false; }
}

bool TwoArc::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool TwoArc::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void TwoArc::SetCenter(Vector& t)
{
	this->Center.set(t);
}

void TwoArc::SetRadius(double r1,double r2)
{
	Radius1(r1); Radius2(r2);
}

void TwoArc::SetSlope(double ang)
{
	Angle(ang);
}

Vector* TwoArc::getCenter()
{
	return (&this->Center);
}

void TwoArc::getParameters(double* TwoArcparam)
{
	Center.FillDoublePointer(TwoArcparam, 2);
	TwoArcparam[2] = Angle();
	TwoArcparam[3] = Radius1(), TwoArcparam[4] = Radius2();
}

Shape* TwoArc::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("TAR");
		else
			myname = _T("dTAR");
		TwoArc* CShape = new TwoArc((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		    CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0017", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* TwoArc::CreateDummyCopy()
{
	try
	{
		TwoArc* CShape = new TwoArc(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0018", __FILE__, __FUNCSIG__); return NULL; }
}

void TwoArc::CopyShapeParameters(Shape* s)
{
	try
	{
		this->Center.set(*((TwoArc*)s)->getCenter());
		this->Radius1(((TwoArc*)s)->Radius1());
		this->Radius2(((TwoArc*)s)->Radius2());
		this->Angle(((TwoArc*)s)->Angle());
		this->AddReflectionLine = AddReflectionLine;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0019", __FILE__, __FUNCSIG__); }
}

void TwoArc::Transform(double* transform)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply(transform, Center.getX(), Center.getY(), Center.getY());
		Center.set(temp.getX(), temp.getY(), temp.getZ());
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0020", __FILE__, __FUNCSIG__); }
}

void TwoArc::Translate(Vector& Position)
{
	Center += Position;
	this->UpdateEnclosedRectangle();
}

void TwoArc::AlignToBasePlane(double* trnasformM)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, Center.getX(), Center.getY(), Center.getZ());
		Center.set(temp.getX(), temp.getY(), temp.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0020", __FILE__, __FUNCSIG__); }
}

void TwoArc::AssignParamForGroupShapes()
{
	try
	{
		if(this->ChildsId.size() > 2)
		{
			list<int>::iterator It = this->ChildsId.begin();  
			this->ReflectionLine = (Line *)MAINDllOBJECT->getShapesList().getList()[*It];  It++;
			this->Arc1 = (Circle *)MAINDllOBJECT->getShapesList().getList()[*It]; It++;
			this->Arc2 = (Circle *)MAINDllOBJECT->getShapesList().getList()[*It];
			if(AddReflectionLine)
			{
				addGroupChild(ReflectionLine);
			}
			addGroupChild(Arc1);  
			addGroupChild(Arc2);  
			ReflectionLine->addGroupParent(this);
	 		Arc1->addGroupParent(this); 
			Arc2->addGroupParent(this);  
			this->firstTime = false;
		}
   }
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0007", __FILE__, __FUNCSIG__); 
	}
}


int TwoArc::shapenumber = 0;
void TwoArc::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, TwoArc& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "TwoArc" <<endl;
		os << "Center:values" << endl << (*static_cast<Vector*>(x.getCenter())) << endl;
		os << "Radius1:" << x.Radius1() << endl;
		os << "Radius2:" << x.Radius2() << endl;
		os << "Angle:" << x.Angle() << endl;
		os << "AddReflectionLine:" << x.AddReflectionLine << endl;
		os << "ChildsIdsize:" << x.ChildsId.size() << endl;
		for(list<int>::iterator It = x.ChildsId.begin(); It != x.ChildsId.end(); It++)
			os << "ChildsId:" << *It << endl;
		os << "EndTwoArc" <<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0021", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, TwoArc& x)
{
	try
	{
		is >> (*(Shape*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"TwoArc")
			{
			x.ShapeType = RapidEnums::SHAPETYPE::TWOARC;
			while(Tagname!=L"EndTwoArc")
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
						if(Tagname==L"Center")
						{
							is >> (*static_cast<Vector*>(x.getCenter()));
						}
						else if(Tagname==L"Radius1")
						{
							 x.Radius1(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"Radius2")
						{
							 x.Radius2(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"Angle")
						{
							 x.Angle(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"AddReflectionLine")
						{
							 if(Val=="0")
								 x.AddReflectionLine=false;
							 else
								 x.AddReflectionLine=true;
						}
						else if(Tagname==L"ChildsIdsize"){						 
						}
						else if(Tagname==L"ChildsId")
						{
							  x.ChildsId.push_back(atoi((const char*)(Val).c_str()));
						}
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CY0022", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, TwoArc& x)
{
	try
	{
		double d; int n, j, k;
		x.ShapeType = RapidEnums::SHAPETYPE::TWOARC;
		is >> (*static_cast<Vector*>(x.getCenter()));
		is >> d; x.Radius1(d);
		is >> d; x.Radius2(d);
		is >> d; x.Angle(d);
		is >> x.AddReflectionLine;
		is >> j;
		for(int i = 0; i < j; i++)
		{
			is >> k; x.ChildsId.push_back(k);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CY0022", __FILE__, __FUNCSIG__); }
}
