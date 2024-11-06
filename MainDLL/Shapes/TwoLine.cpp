#include "stdafx.h"
#include "TwoLine.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

TwoLine::TwoLine(TCHAR* myname):ShapeWithList(genName(myname))
{
	try
	{
	    init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0001", __FILE__, __FUNCSIG__);}
}

TwoLine::TwoLine(bool simply):ShapeWithList(false)
{
	init();
}

TwoLine::~TwoLine()
{
}

void TwoLine::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::TWOLINE;
		this->LineAngle(0.0); this->Angle(0.0);
		this->ShapeAs3DShape(false); 
		this->UseLightingProperties(false);
		this->AddReflectionLine = true;
		this->ReflectionLine = NULL;
		this->Line1 = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
		this->Line1->LineType = RapidEnums::LINETYPE::LINEINTWOLINE;
		this->Line2 = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
		this->Line2->LineType = RapidEnums::LINETYPE::LINEINTWOLINE;
		this->firstTime = true;
		this->IsValid(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0002", __FILE__, __FUNCSIG__); }
}

TCHAR* TwoLine::genName(const TCHAR* prefix)
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

void TwoLine::UpdateBestFit()
{
	try
	{
		int totcount = this->PointsListOriginal->Pointscount();
		if(ReflectionLine == NULL)
		{
			if(PPCALCOBJECT->IsPartProgramRunning())
			   AssignParamForGroupShapes();
			else
			{
				ReflectionLine = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
				ReflectionLine->LineType = RapidEnums::LINETYPE::LINEINTWOLINE;
			}
		}
		if(totcount < 4) return;
		if(!updateTwoLine(totcount))
		{
			if(PerpendicularDistance() > 0)this->IsValid(true);
			else this->IsValid(false);
			return;
		}
		else
		{
			Line1->IsValid(true);
			Line1->notifyAll(ITEMSTATUS::DATACHANGED, Line1);	
		    Line2->IsValid(true);
			Line2->notifyAll(ITEMSTATUS::DATACHANGED, Line2);	
		    ReflectionLine->IsValid(true);
			ReflectionLine->notifyAll(ITEMSTATUS::DATACHANGED, ReflectionLine);	
			this->notifyAll(ITEMSTATUS::DATACHANGED, this);	
		}
		ProbePointCalculations(totcount);
		if(firstTime) addGroupShapes();
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0004", __FILE__, __FUNCSIG__); }
}

bool TwoLine::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	return false;
}

void TwoLine::UpdateForParentChange(BaseItem* sender)
{}

bool TwoLine::updateTwoLine(int PointsCount)
{
	try
	{
		double angle1, angle2, ans[6] = {0}, intercepts[3], tempPoint1[2], tempPoint2[2];
		bool bestFitFlag;  Vector tempV;
		if(AddReflectionLine)
			bestFitFlag = BESTFITOBJECT->TwoLine_BestFit(this->pts, PointsCount, &ans[0]);
		else
		{
		    ans[0] = ReflectionLine->getMidPoint()->getX(); ans[1] = ReflectionLine->getMidPoint()->getY(); ans[2] = ReflectionLine->Angle(), intercepts[0] = ReflectionLine->Intercept();
		    bestFitFlag = BESTFITOBJECT->TwoLine_BestFit(this->pts, PointsCount, &ans[0],true);
		}
		if(!bestFitFlag) return false;
		angle1 = ans[2];	
		angle2 = ans[5];
		RMATH2DOBJECT->Intercept_LinePassing_Point(ans, angle1, &intercepts[1]);
		RMATH2DOBJECT->Intercept_LinePassing_Point(&ans[3], angle2, &intercepts[2]);
		double angOfAxis = 0;
		int PointsCnt1 = 0, PointsCnt2 = 0;
		double *tmppts1 = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		double *tmppts2 = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		for(int i = 0; i < this->PointsListOriginal->Pointscount(); i++)
		{
		   if((RMATH2DOBJECT->Pt2Line_Dist(pts[2 * i], pts[2 * i + 1], angle1, intercepts[1])) < (RMATH2DOBJECT->Pt2Line_Dist(pts[2 * i], pts[2 * i + 1], angle2, intercepts[2])))
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
		double endp[4] = {0};
		if(PointsCnt1 > 0)
		{
			RMATH2DOBJECT->LineEndPoints(tmppts1, PointsCnt1 / 2, &endp[0], &endp[2], angle1);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle1, intercepts[1], &endp[0], &tempPoint1[0]);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle1, intercepts[1], &endp[2], &tempPoint2[0]);
	  
			this->Line1->setLineParameters(angle1, intercepts[1]);
			tempV.set(tempPoint1[0], tempPoint1[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->Line1->setPoint1(tempV);
			tempV.set(tempPoint2[0], tempPoint2[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
			this->Line1->setPoint2(tempV);
			Line1->calculateAttributes();
		}
		if(PointsCnt2 > 0)
		{
			RMATH2DOBJECT->LineEndPoints(tmppts2, PointsCnt2 / 2, &endp[0], &endp[2], angle2);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle2, intercepts[2], &endp[0], &tempPoint1[0]);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle2, intercepts[2], &endp[2], &tempPoint2[0]);
			this->Line2->setLineParameters(angle2, intercepts[2]);
			tempV.set(tempPoint1[0], tempPoint1[1], MAINDllOBJECT->getCurrentDRO().getZ());
			this->Line2->setPoint1(tempV);
			tempV.set(tempPoint2[0], tempPoint2[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
			this->Line2->setPoint2(tempV);
			Line2->calculateAttributes();
			if(AddReflectionLine)
			{
				double intersectPnt[2] = {0}, parentP[4] = {0}, axisLinePnts[4] = {0};
				Line1->getPoint1()->FillDoublePointer(parentP);
				Line1->getPoint2()->FillDoublePointer(&parentP[2]);
				axisLinePnts[0] = (parentP[0] + parentP[2]) / 2;
				axisLinePnts[1] = (parentP[1] + parentP[3]) / 2;
				Line2->getPoint1()->FillDoublePointer(parentP);
				Line2->getPoint2()->FillDoublePointer(&parentP[2]);
				axisLinePnts[2] = (parentP[0] + parentP[2]) / 2;
				axisLinePnts[3] = (parentP[1] + parentP[3]) / 2;
				axisLinePnts[0] = (axisLinePnts[0] + axisLinePnts[2]) / 2;
				axisLinePnts[1] = (axisLinePnts[1] + axisLinePnts[3]) / 2;
				if(RMATH2DOBJECT->Line_lineintersection(angle1, intercepts[1], angle2, intercepts[2], intersectPnt) != 0)
				{
					double tmpang1 = (ans[2] + ans[5]) / 2, tmpang2 = tmpang1 + M_PI_2;
					double tmpInter[2] = {0}, tmpPnt;
					RMATH2DOBJECT->Intercept_LinePassing_Point(intersectPnt, tmpang1, tmpInter);
					RMATH2DOBJECT->Intercept_LinePassing_Point(intersectPnt, tmpang2, &tmpInter[1]);
					 if((RMATH2DOBJECT->Pt2Line_Dist(axisLinePnts[0], axisLinePnts[1], tmpang1, tmpInter[0])) < (RMATH2DOBJECT->Pt2Line_Dist(axisLinePnts[0], axisLinePnts[1], tmpang2, tmpInter[1])))
					 {
						 angOfAxis = tmpang1;
						 intercepts[0] = tmpInter[0];
					 }
					 else
					 {
					     angOfAxis = tmpang2;
						 intercepts[0] = tmpInter[1];
					 }
				}
				else
				{
					angOfAxis = angle1;
					intercepts[0] = (intercepts[1] + intercepts[2]) / 2;
				}
				this->Angle(angOfAxis);
		        this->LineAngle(angOfAxis - ans[2]);
				this->ReflectionLine->setLineParameters(angOfAxis, intercepts[0]);
				RMATH2DOBJECT->EquiDistPointsOnLine(angOfAxis, (Line1->Length() + Line2->Length()) / 2, axisLinePnts, tempPoint1, tempPoint2, 0);
				tempV.set(tempPoint1[0], tempPoint1[1], MAINDllOBJECT->getCurrentDRO().getZ());
				this->ReflectionLine->setPoint1(tempV);
				tempV.set(tempPoint2[0], tempPoint2[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
				this->ReflectionLine->setPoint2(tempV);
				ReflectionLine->calculateAttributes();
			}
		}
		free(tmppts1); tmppts1 = NULL;
		free(tmppts2); tmppts2 = NULL;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0005", __FILE__, __FUNCSIG__); return false;}
}

void TwoLine::addGroupShapes()
{
	try
	{
		if(AddReflectionLine)
		{
		    this->addGroupChild(ReflectionLine);
		}
		this->addGroupChild(Line1);  
		this->addGroupChild(Line2);  
		this->ReflectionLine->addGroupParent(this);
	    this->Line1->addGroupParent(this);  
		this->Line2->addGroupParent(this);  
		this->ChildsId.push_back(ReflectionLine->getId());
		this->ChildsId.push_back(Line1->getId());
		this->ChildsId.push_back(Line2->getId());
		if(AddReflectionLine)
		   AddShapeAction::addShape(this->ReflectionLine, false);
		AddShapeAction::addShape(this->Line1, false);
		AddShapeAction::addShape(this->Line2, false);
		firstTime = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0006", __FILE__, __FUNCSIG__);}
}

void TwoLine::updateParameters()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR0006", __FILE__, __FUNCSIG__);}
}

void TwoLine::ProbePointCalculations(int PointsCnt)
{
	try
	{
		int shapeNum;
		if(PointsCnt == 4)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
			if(ShapeDoneUsingProbe())
				shapeNum = CheckIdorOdTwoLine();
		}
		if(ShapeDoneUsingProbe())
		{
			updateTwoLine_UsingPorbe(shapeNum);
			updateTwoLine(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0005", __FILE__, __FUNCSIG__); }
}

void TwoLine::CheckPointIsForPorbe()
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

int TwoLine::CheckIdorOdTwoLine()
{
	try
	{
		SinglePoint* Spt = (SinglePoint*)(*PointsList->getList().begin()).second;
		double dist1 = RMATH2DOBJECT->Pt2Line_DistSigned(Spt->X, Spt->Y, Line1->Angle(), Line1->Intercept());
		double dist2 = RMATH2DOBJECT->Pt2Line_DistSigned(Spt->X, Spt->Y, Line2->Angle(), Line2->Intercept());
		IdTwoLine(dist1 * dist2 < 0);
		if(dist1 < dist2) return 1;
		else return 2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TAR00017", __FILE__, __FUNCSIG__); }
}

void TwoLine::updateTwoLine_UsingPorbe(int i)
{
	try
	{
		double Pt[3], Pradius;
		if(IdTwoLine())
			Pradius = MAINDllOBJECT->ProbeRadius();
		else
			Pradius = -MAINDllOBJECT->ProbeRadius();
		double rangle, PtX, PtY;
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double *tmpCenter, tempPoint[2] = {Spt->X, Spt->Y};
		    if(i == 1)
			   rangle = Line1->Angle() + M_PI_2;
	        else if(i == 2)
			   rangle = Line2->Angle() + M_PI_2;
		    if(rangle > M_PI) rangle -= M_PI;
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
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			filterptsCnt++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0008", __FILE__, __FUNCSIG__); }
}

void TwoLine::UpdateEnclosedRectangle()
{
	try
	{
      if(Line1 != NULL && Line2 != NULL && ReflectionLine != NULL)
	  {
		  Line1->UpdateEnclosedRectangle();
		  Line2->UpdateEnclosedRectangle();
		  ReflectionLine->UpdateEnclosedRectangle();
	  }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0019", __FILE__, __FUNCSIG__); }
}

void TwoLine::GetShapeCenter(double *cPoint)
{
	cPoint[0] = Center.getX();
	cPoint[1] = Center.getY();
	cPoint[2] = Center.getZ();
}

void TwoLine::SetSubLineParam(double* points, double angle, double intercept, int pointsCount, Line *currentLine)
{
}

void TwoLine::UpdateForSubshapeChange(Shape *childShape)
{
	try
	{
		if(!this->getGroupChilds().empty())
		{
			this->PointsListOriginal->CopyAllPoints(((ShapeWithList *)childShape)->PointsList);
			((ShapeWithList *)childShape)->PointsList->deleteAll();
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
				this->filterpts[j++] = Pt->X;
				this->filterpts[j++] = Pt->Y;
				filterptsCnt++;
			}
			UpdateBestFit();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0010", __FILE__, __FUNCSIG__); }
}

void TwoLine::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{	 	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0011", __FILE__, __FUNCSIG__); }
}

void TwoLine::drawGDntRefernce()
{
}

void TwoLine::ResetShapeParameters()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0013", __FILE__, __FUNCSIG__); }
}

bool TwoLine::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
	  return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0014", __FILE__, __FUNCSIG__); return false; }
}

bool TwoLine::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool TwoLine::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void TwoLine::SetCenter(Vector& t)
{
	this->Center.set(t);
}

Vector* TwoLine::getCenter()
{
	return (&this->Center);
}

void TwoLine::getParameters(double* twoLineparam)
{
	Center.FillDoublePointer(twoLineparam, 3);
	twoLineparam[3] = Angle();
	twoLineparam[4] = LineAngle();
}

Shape* TwoLine::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("TL");
		else
			myname = _T("dTL");
		TwoLine* CShape = new TwoLine((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0017", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* TwoLine::CreateDummyCopy()
{
	try
	{
		TwoLine* CShape = new TwoLine(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0018", __FILE__, __FUNCSIG__); return NULL; }
}

void TwoLine::CopyShapeParameters(Shape* s)
{
	try
	{
		this->Center.set(*((TwoLine*)s)->getCenter());
		this->Angle(((TwoLine*)s)->Angle());
		this->LineAngle(((TwoLine*)s)->LineAngle());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0019", __FILE__, __FUNCSIG__); }
}

void TwoLine::Transform(double* transform)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply(transform, Center.getX(), Center.getY(), Center.getY());
		Center.set(temp.getX(), temp.getY(),Center.getZ());
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0020", __FILE__, __FUNCSIG__); }
}

void TwoLine::Translate(Vector& Position)
{
	Center += Position;
	this->UpdateEnclosedRectangle();
}

void TwoLine::AlignToBasePlane(double* trnasformM)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, Center.getX(), Center.getY(), Center.getZ());
		Center.set(temp.getX(), temp.getY(), temp.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0020", __FILE__, __FUNCSIG__); }
}

void TwoLine::AssignParamForGroupShapes()
{
	try
	{
		if(this->ChildsId.size() > 2)
		{
			list<int>::iterator It = this->ChildsId.begin();  
			this->ReflectionLine = (Line *)MAINDllOBJECT->getShapesList().getList()[*It];  It++;
			this->Line1 = (Line *)MAINDllOBJECT->getShapesList().getList()[*It]; It++;
			this->Line2 = (Line *)MAINDllOBJECT->getShapesList().getList()[*It];
			if(AddReflectionLine)
			{
				this->addGroupChild(ReflectionLine);
			}
			this->addGroupChild(Line1);  
			this->addGroupChild(Line2);  
			this->ReflectionLine->addGroupParent(this);
			this->Line1->addGroupParent(this);  
			this->Line2->addGroupParent(this);  
			this->firstTime = false;
		}
   }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0007", __FILE__, __FUNCSIG__); }
}


int TwoLine::shapenumber = 0;
void TwoLine::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, TwoLine& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "TwoLine" << endl;
		os << "Center:values" << endl << (*static_cast<Vector*>(x.getCenter())) << endl;
		os << "Angle:" << x.Angle() << endl;
		os << "LineAngle:" << x.LineAngle() << endl;
		os << "AddReflectionLine:" << x.AddReflectionLine << endl;
		os << "ChildSize: " << x.ChildsId.size() << endl;
		for(list<int>::iterator It = x.ChildsId.begin(); It != x.ChildsId.end(); It++)
			os << "ChildsId:" << *It << endl;
		os << "EndTwoLine" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0021", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, TwoLine& x)
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
			if(Tagname==L"TwoLine")
			{
			x.ShapeType = RapidEnums::SHAPETYPE::TWOLINE;
			while(Tagname!=L"EndTwoLine")
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
						else if(Tagname==L"Angle")
						{
							 x.Angle(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"Angle")
						{
							 x.LineAngle(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"AddReflectionLine")
						{
							 if(Val=="0")
								 x.AddReflectionLine=false;
							 else
								 x.AddReflectionLine=true;
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("CY0022", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, TwoLine& x)
{
	try
	{
		double d; int j,k;
		x.ShapeType = RapidEnums::SHAPETYPE::TWOLINE;
		is >> (*static_cast<Vector*>(x.getCenter()));
		is >> d; x.Angle(d);
		is >> d; x.LineAngle(d);
		is >> x.AddReflectionLine;
	    is >> j;
		for(int i = 0; i < j; i++)
		{
			is >> k; x.ChildsId.push_back(k);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("CY0022", __FILE__, __FUNCSIG__); }
}