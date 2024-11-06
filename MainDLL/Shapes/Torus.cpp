#include "stdafx.h"
#include "Torus.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

Torus::Torus(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

Torus::Torus(bool simply):ShapeWithList(false)
{
	init();
}

Torus::~Torus()
{
}

void Torus::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::TORUS;
		this->Radius1(0); this->Radius2(0);
		this->dir_l(0); this->dir_m(0); this->dir_n(0);
		this->ShapeAs3DShape(true); 
		this->UseLightingProperties(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0001", __FILE__, __FUNCSIG__); }
}

TCHAR* Torus::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0002", __FILE__, __FUNCSIG__); return name; }
}

void Torus::UpdateBestFit()
{
	try
	{
		this->IsValid(false);
		int totcount = this->PointsListOriginal->Pointscount();
		if(totcount < 8) return;
		if(!updateTorus(totcount))
		{
			if(Radius1() > 0)this->IsValid(true);
			else this->IsValid(false);
			return;
		}
		ProbePointCalculations(totcount);
		calculateAttributes();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0003", __FILE__, __FUNCSIG__); }
}

bool Torus::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	return false;
}

bool Torus::updateTorus(int PointsCount)
{
	try
	{
		double ans[8] = {0}; bool flag;
		//flag = BESTFITOBJECT->Torus_BestFit(ans, PointsCount, &ans[0]);
		//if(!flag) return false;
		if(ApplyFilterCorrection() && PointsCount > 4)
		{
		    int cnt = PointsCount;
			for(int i = 0; i < FilterCorrectionIterationCnt(); i++)
		    {
				flag = false;
				cnt = HELPEROBJECT->FilteredTorus(PointsList, ans, FilterCorrectionFactor(), i);
				if(cnt < 8) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * cnt * 3);
				filterptsCnt = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					if(Pt->InValid) continue;
					this->filterpts[3 * filterptsCnt] = Pt->X;
					this->filterpts[3 * filterptsCnt + 1] = Pt->Y;
					this->filterpts[3 * filterptsCnt + 2] = Pt->Z;
					filterptsCnt++;
				}
				//if(!BESTFITOBJECT->Torus_BestFit(filterpts, filterptsCnt, &ans[0])) return false;
				return true;
			}
    	}
		if(ApplyMMCCorrection() && ScanDirDetected() && PointsCount > 4)
		{
			int cnt = PointsCount;
		    for(int i = 0; i < MMCCorrectionIterationCnt(); i++)
		    {
				flag = false;
				bool useValidPointOnly = false;
				if(i > 0 || ApplyFilterCorrection()) {useValidPointOnly = true;}
				cnt = HELPEROBJECT->OutermostInnerMostTorus(PointsList, ans, useValidPointOnly, OuterCorrection());
				if(cnt < 8) break;
				if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
				this->filterpts = (double*)malloc(sizeof(double) * cnt * 3);
				filterptsCnt = 0;
				for(PC_ITER Spt = this->PointsList->getList().begin(); Spt != this->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					if(Pt->InValid) continue;
					this->filterpts[3 * filterptsCnt] = Pt->X;
					this->filterpts[3 * filterptsCnt + 1] = Pt->Y;
					this->filterpts[3 * filterptsCnt + 2] = Pt->Z;
					filterptsCnt++;
				}
				//if(!BESTFITOBJECT->Torus_BestFit(filterpts, filterptsCnt, &ans[0])) return false;
				return true;
			}
		}		
		center.set(ans[0], ans[1], ans[2]);
		dir_l(ans[3]);
		dir_m(ans[4]);
		dir_n(ans[5]);
		Radius1(ans[6]);
		Radius2(ans[7]);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0004", __FILE__, __FUNCSIG__); return false;}
}

void Torus::updateParameters()
{
	try
	{
		/*this->Length(sqrt(pow(center2.getX() - center1.getX(), 2) + pow(center2.getY() - center1.getY(), 2) + pow(center2.getZ() - center1.getZ(), 2)));
		this->dir_l((center2.getX() - center1.getX()) / Length());
		this->dir_m((center2.getY() - center1.getY()) / Length());
		this->dir_n((center2.getZ() - center1.getZ()) / Length());*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0004", __FILE__, __FUNCSIG__);}
}

void Torus::ProbePointCalculations(int PointsCnt)
{
	try
	{
		if(PointsCnt == 8)
		{ 
			if(!PPCALCOBJECT->IsPartProgramRunning())
				CheckPointIsForPorbe();
		/*	if(ShapeDoneUsingProbe())
				CheckIdorOdTorus();*/
		}
		if(ShapeDoneUsingProbe())
		{
			updateTorus_UsingPorbe();
			updateTorus(PointsCnt);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0005", __FILE__, __FUNCSIG__); }
}

void Torus::CheckPointIsForPorbe()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0006", __FILE__, __FUNCSIG__); }
}

void Torus::updateTorus_UsingPorbe()
{
	try
	{
		double Pt[3];;
		double Pradius = MAINDllOBJECT->ProbeRadius();
		double TorusParam[8] = {center.getX(), center.getY(), center.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), Radius2()};
		for(PC_ITER Item = this->PointsListOriginal->getList().begin(); Item != this->PointsListOriginal->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double PrPoint[3] = {Spt->X, Spt->Y, Spt->Z};
			RMATH3DOBJECT->Torus_SurfacePt_for_Probe(&TorusParam[0], &PrPoint[0], Pradius, &Pt[0]);
			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(Pt[0], Pt[1], Pt[2], Spt->R, Spt->Pdir);
		}
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
		if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}
		this->filterpts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 2);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0008", __FILE__, __FUNCSIG__); }
}

void Torus::UpdateForParentChange(BaseItem* sender)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0009", __FILE__, __FUNCSIG__); }
}

void Torus::UpdateEnclosedRectangle()
{
	try
	{
      RMATH2DOBJECT->GetTopLeftAndRightBottom(this->pts, this->PointsList->Pointscount(), 3, &ShapeLeftTop[0], &ShapeRightBottom[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0019", __FILE__, __FUNCSIG__); }
}

void Torus::calculateAttributes()
{
	try
	{
		int PointsCount = this->PointsList->Pointscount();
		double Npt[3];
		double TorusParam[8] = {center.getX(), center.getY(), center.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), Radius2()};
	/*	*double dist1 = RMATH3DOBJECT->Nearest_Point_to_Cylinder(pts, PointsCount, &TorusParam[0], &Npt[0]);
		this->MinInscribedia(dist1 + Radius1());
		double dist2 = RMATH3DOBJECT->Farthest_Point_to_Cylinder(pts, PointsCount, &CylinderParam[0], &Npt[0]);*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0010", __FILE__, __FUNCSIG__); }
}

void Torus::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
	  /*  if(HighlightedFormouse())
			GRAFIX->draw_Torus(center.getX(),center.getY(),center.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), Radius2(), MAINDllOBJECT->DrawWiredSurface(), false, WPixelWidth);
		else*/
		 GRAFIX->draw_Torus(center.getX(),center.getY(),center.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), Radius2(), MAINDllOBJECT->DrawWiredSurface(), false, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0011", __FILE__, __FUNCSIG__); }
}

void Torus::drawGDntRefernce()
{

}

void Torus::ResetShapeParameters()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0013", __FILE__, __FUNCSIG__); }
}

bool Torus::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		double SelectionL[6] = {corner1.getX(), corner1.getY(), corner1.getZ(), 0, 0, 1};
		return Shape_IsInWindow(&SelectionL[0], Tolerance);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0014", __FILE__, __FUNCSIG__); return false; }
}

bool Torus::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool Torus::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		/*double ThisTorus[8] = {center.getX(), center.getY(), center.getZ(), dir_l(), dir_m(), dir_n(), Radius1(), Radius2()};
		double IntersectnPts[6], ProjctnPt[3];
		int IntrsectCnt = 0;
		return RMATH3DOBJECT->Intersection_Line_Torus(SelectionLine, ThisTorus,  MAINDllOBJECT->getWindow(2).getUppX(), &IntrsectCnt, IntersectnPts);*/
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0015", __FILE__, __FUNCSIG__); return false; }
}

void Torus::SetCenter(Vector& t)
{
	this->center.set(t);
}

void Torus::SetRadius(double r1,double r2)
{
	Radius1(r1); Radius2(r2);
}

void Torus::SetTorusProperties(double l, double m, double n)
{
	dir_l(l); dir_m(m); dir_n(n);
}

Vector* Torus::getCenter()
{
	return (&this->center);
}

void Torus::GetShapeCenter(double *cPoint)
{
	cPoint[0] = center.getX();
	cPoint[1] = center.getY();
	cPoint[2] = center.getZ();
}

void Torus::getParameters(double* Torusparam)
{
	center.FillDoublePointer(Torusparam, 3);
	Torusparam[3] = dir_l(); Torusparam[4] = dir_m(); Torusparam[5] = dir_n();
	Torusparam[6] = Radius1(), Torusparam[7] = Radius2();
}

Shape* Torus::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("TR");
		else
			myname = _T("dTR");
		Torus* CShape = new Torus((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		    CShape->CopyOriginalProperties(this);
	 	return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0017", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Torus::CreateDummyCopy()
{
	try
	{
		Torus* CShape = new Torus(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0018", __FILE__, __FUNCSIG__); return NULL; }
}

void Torus::CopyShapeParameters(Shape* s)
{
	try
	{
		this->center.set(*((Torus*)s)->getCenter());
		this->Radius1(((Torus*)s)->Radius1());
		this->Radius2(((Torus*)s)->Radius2());
		this->dir_l(((Torus*)s)->dir_l());
		this->dir_m(((Torus*)s)->dir_m());
		this->dir_n(((Torus*)s)->dir_n());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0019", __FILE__, __FUNCSIG__); }
}

void Torus::Transform(double* transform)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply(transform, center.getX(), center.getY(), center.getY());
		center.set(temp.getX(), temp.getY(),center.getZ());
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0020", __FILE__, __FUNCSIG__); }
}

void Torus::Translate(Vector& Position)
{
	center += Position;
	this->UpdateEnclosedRectangle();
}

void Torus::AlignToBasePlane(double* trnasformM)
{
	try
	{
		Vector temp = MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, center.getX(), center.getY(), center.getZ());
		center.set(temp.getX(), temp.getY(), temp.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TR0020", __FILE__, __FUNCSIG__); }
}


int Torus::shapenumber = 0;
void Torus::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, Torus& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "Torus" << endl;
		os << "Center:Values" << endl <<(*static_cast<Vector*>(x.getCenter())) << endl;
		os << "Radius1:" << x.Radius1() << endl;
		os << "Radius2:" << x.Radius2() << endl;
		os << "dir_l:" << x.dir_l() << endl;
		os << "dir_m:" << x.dir_m() << endl;
		os << "dir_n:" << x.dir_n() << endl;
		os << "EndTorus" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CY0021", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Torus& x)
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
			if(Tagname==L"Torus")
			{
			x.ShapeType = RapidEnums::SHAPETYPE::TORUS ; 
			while(Tagname!=L"EndTorus")
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
						else if(Tagname==L"dir_l")
						{
							 x.dir_l(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"dir_m")
						{
							 x.dir_m(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"dir_n")
						{
							 x.dir_n(atof((const char*)(Val).c_str()));
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
void ReadOldPP(wistream& is, Torus& x)
{
	try
	{
		double d;
		x.ShapeType = RapidEnums::SHAPETYPE::TORUS;
		is >> (*static_cast<Vector*>(x.getCenter()));
		is >> d; x.Radius1(d);
		is >> d; x.Radius2(d);
		is >> d; x.dir_l(d);
		is >> d; x.dir_m(d);
		is >> d; x.dir_n(d);
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("CY0022", __FILE__, __FUNCSIG__);}
}