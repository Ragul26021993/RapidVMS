#include "StdAfx.h"
#include "Pline.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PointCollection.h"

Pline::Pline(TCHAR* myname):ShapeWithList(genName(myname))
{
	this->ShapeType = RapidEnums::SHAPETYPE::POLYLINE;
}

Pline::Pline(bool simply):ShapeWithList(false)
{
	this->ShapeType = RapidEnums::SHAPETYPE::POLYLINE;
}

void Pline::init()
{
	this->SnapPointList->Addpoint(new SnapPoint(this, &(this->point1), 0), 0);
}

Pline::~Pline()
{
}

TCHAR* Pline::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLINE0001", __FILE__, __FUNCSIG__); return name; }
}
bool Pline::Shape_IsInWindow(Vector& corner1,double Tolerance)
{
	return false;
}

bool Pline::Shape_IsInWindow( Vector& corner1,Vector& corner2 )
{
	return false;
}

bool Pline::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void Pline::Translate(Vector& Position)
{
	try
	{
		for(PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			Spt->SetValues(Spt->X + Position.getX(), Spt->Y + Position.getY(), Spt->Z + Position.getZ(), Spt->R, Spt->Pdir);
		}
		
		if(this->pts != NULL){free(pts); pts = NULL;}
		this->pts = (double*)malloc(sizeof(double) * this->PointsListOriginal->Pointscount() * 3);
		if(this->filterpts != NULL){ free(filterpts); filterpts = NULL;}
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
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLN0001", __FILE__, __FUNCSIG__);}
}

void Pline::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		int totalcnt = this->PointsList->Pointscount();
		int *a = NULL;
		a = (int*)malloc(sizeof(int) * totalcnt * 2);
		int i = totalcnt;
		for(i = totalcnt; i > 0; i--)
		{
			*(a + 2 * i - 1) = i;
			*(a + 2 * i - 2) = i - 1;
		}
		double* TmpPts = NULL;
		TmpPts = (double*)malloc(sizeof(double) * totalcnt * 2);
		for(PC_ITER Item = this->PointsList->getList().begin(); Item != this->PointsList->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			pts[i++] = Spt->X; pts[i++] = Spt->Y;
		}
		GRAFIX->drawLineloop(pts, 2 *(totalcnt - 1), a, 2);
		if(a != NULL){free(a); a = NULL;}
		if(TmpPts != NULL){free(TmpPts); TmpPts = NULL;}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLINE0002", __FILE__, __FUNCSIG__); }
}

void Pline::drawGDntRefernce()
{
}

void Pline::ResetShapeParameters()
{
}

void Pline::UpdateBestFit()
{
}
bool Pline::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		int n = this->PointsList->Pointscount();
		double *TransformePpoint = NULL, *tempPoint = NULL;
		TransformePpoint = new double[(n + 1) * 2];
		tempPoint = new double[n * 4];
		int cnt = 0;
		for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			tempPoint[cnt++] = Spt->X;
			tempPoint[cnt++] = Spt->Y;
			tempPoint[cnt++] = Spt->Z;
			tempPoint[cnt++] = 1;
		}	
		 for(int i = 0; i < n; i++)
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &tempPoint[4 * i], &s2[0], &TransformePpoint[2 * i]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		delete [] tempPoint;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CP0002a", __FILE__, __FUNCSIG__); return false;}
}

void Pline::UpdateForParentChange(BaseItem* sender)
{
}

void Pline::UpdateEnclosedRectangle()
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1}, n = this->PointsList->Pointscount();	
		double *TransformePpoint = NULL;
		TransformePpoint = new double[n * 2 + 1];
		int cnt = 0;
		for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			TransformePpoint[cnt++] = Spt->X;
			TransformePpoint[cnt++] = Spt->Y;
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0},  *Temporary_point = NULL;	
			Temporary_point = new double[n * 3];
			cnt = 0;
			for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				Temporary_point[cnt++] = Spt->X;
				Temporary_point[cnt++] = Spt->Y;
				Temporary_point[cnt++] = Spt->Z;
			}
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			for(int i = 0; i < n; i++)
				RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &Temporary_point[3 * i], Order2, &TransformePpoint[2 * i]);	
			delete [] Temporary_point;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, ShapeLeftTop, ShapeRightBottom);
		delete [] TransformePpoint;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLN0003", __FILE__, __FUNCSIG__);}
}


void Pline::Transform(double* transform)
{
	try
	{
		for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			Vector temp = MAINDllOBJECT->TransformMultiply(transform, Spt->X, Spt->Y, Spt->Z);
			SinglePoint* Spt1 =  this->PointsList->getList()[Spt->PtID];
			Spt1->SetValues(temp.getX(), temp.getY(), temp.getZ(), Spt->R, Spt->Pdir);
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
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLINE0004", __FILE__, __FUNCSIG__); }
}

void Pline::AlignToBasePlane(double* trnasformM)
{
}

void Pline::setPoint1( Vector d)
{
	point1 = d;
}

Vector* Pline::getPoint1()
{
	return(&this->point1);
}

Shape* Pline::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("PL");
		else
			myname = _T("dPL");
		Pline* CShape = new Pline((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		{
		    CShape->CopyOriginalProperties(this);
			PointCollection& PtColl = *(((ShapeWithList*)this)->PointsList);
			for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
			{
				SinglePoint* Sp = (*Spt).second;
				CShape->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
				CShape->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
			}
			CShape->UpdateShape();
		}
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLINE0004", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Pline::CreateDummyCopy()
{
	try
	{
		Pline* CShape = new Pline(false);
		CShape->CopyOriginalProperties(this);
		PointCollection& PtColl = *(((ShapeWithList*)this)->PointsList);
		for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
		{
			SinglePoint* Sp = (*Spt).second;
			CShape->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
			CShape->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
		}
		CShape->UpdateShape();
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLINE0005", __FILE__, __FUNCSIG__); return NULL; }
}

void Pline::CopyShapeParameters(Shape* )
{
}

int Pline::shapenumber=0;
void Pline::reset()
{
	shapenumber = 0;
}
