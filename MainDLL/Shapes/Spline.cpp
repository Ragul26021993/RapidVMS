#include "StdAfx.h"
#include "Spline.h"
#include "SinglePoint.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PointCollection.h"
#include "..\DXF\DXFExpose.h"

//declare private variable......

double* basisCoeffArray;
std::map<double, double*> basisCoefficientList;
std::map<double, double*> TmpCoefficientList;

Spline::Spline(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

Spline::Spline(bool simply):ShapeWithList(false)
{
	init();
}

Spline::~Spline()
{
	try
	{	
		if(this->KnotsArray != NULL)
		{
			delete [] KnotsArray;
			this->KnotsArray = NULL;
		}
		if(this->CtrPtsArray != NULL)
		{
			delete [] CtrPtsArray;
			this->CtrPtsArray = NULL;
		}
		ClearAllMemory();
		delete ControlPointsList;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0001", __FILE__, __FUNCSIG__);}
}

void Spline::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::SPLINE;
		this->ShapeAs3DShape(true);
		this->IsValid(true);
		this->ClosedSpline = false;
		this->DrawSpline = true;
		this->framgrabShape = true;
		this->Spline_degree = 3; this->Spline_cpts = 0; this->Spline_knots = 0;
		basisCoeffArray = NULL;
		this->KnotsArray = NULL;
		this->CtrPtsArray = NULL;
		ControlPointsList = new PointCollection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0002", __FILE__, __FUNCSIG__);}
}

int Spline::Lshapenumber = 0;

TCHAR* Spline::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		Lshapenumber++;
		_itot_s(Lshapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINE0003", __FILE__, __FUNCSIG__); return name; }
}

void Spline::ClearAllMemory()
{
	try
	{
		while(pointsOnCurve.size() != 0)
		{
			std::list<SinglePoint*>::iterator itr = pointsOnCurve.begin();
			SinglePoint* Spt = *itr;
			pointsOnCurve.remove(Spt);
			delete Spt;
		}
		pointsOnCurve.clear();
		CleartmpMemory(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0004", __FILE__, __FUNCSIG__);}
}

bool Spline::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		if(this->framgrabShape)
		{
			for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				if(Tolerance >= RMATH2DOBJECT->Pt2Pt_distance(corner1.getX(), corner1.getY(), Spt->X, Spt->Y))
				{
					ReturnFlag = true;
					break;
				}
			}
		}
		else
		{
			for(std::list<SinglePoint*>::iterator itr = pointsOnCurve.begin(); itr != pointsOnCurve.end(); itr++)
			{	
				if(Tolerance >= RMATH2DOBJECT->Pt2Pt_distance(corner1.getX(), corner1.getY(), (*itr)->X, (*itr)->Y))
				{
					ReturnFlag = true;
					break;
				}
			}
		}
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0005", __FILE__, __FUNCSIG__); return false;}
}

bool Spline::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	try
	{
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0006", __FILE__, __FUNCSIG__); return false;}
}

bool Spline::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0007", __FILE__, __FUNCSIG__); return false;}
}

void Spline::Translate(Vector& Position)
{
	try
	{
		for(PC_ITER SptItem = this->ControlPointsList->getList().begin(); SptItem != this->ControlPointsList->getList().end(); SptItem++)
		{	
			SinglePoint* Spt = (*SptItem).second;
			Spt->X += Position.getX(); Spt->Y += Position.getY(); Spt->Z += Position.getZ();
		}
		this->UpdateBestFit();
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0008", __FILE__, __FUNCSIG__); }
}

void Spline::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		if(this->DrawSpline)
		{	
			if(this->CtrPtsArray == NULL)
				this->UpdateBestFit();
			if(this->ControlPointsList->Pointscount() > 3 && this->CtrPtsArray != NULL)
				GRAFIX->drawSpline(this->CtrPtsArray, this->ControlPointsList->Pointscount(), this->KnotsArray);
			/*for(std::list<SinglePoint*>::iterator itr = pointsOnCurve.begin(); itr != pointsOnCurve.end(); itr++)
			{		
				double pt[3] = {(*itr)->X, (*itr)->Y, (*itr)->Z};
				GRAFIX->drawPoints(pt, 1, false, 3, FGPOINTSIZE);
			}*/
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0009", __FILE__, __FUNCSIG__); }
}

void Spline::drawGDntRefernce()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline00010", __FILE__, __FUNCSIG__); }
}

void Spline::ResetShapeParameters()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline00011", __FILE__, __FUNCSIG__); }
}

void Spline::UpdateControlPointList()
{
	try
	{
		this->ControlPointsList->deleteAll();
		int PtsCount = this->PointsList->Pointscount(), SkipPtsCount = 0, TmpCount = 0;
		if(PtsCount > 250)
		{
			 SkipPtsCount = PtsCount / 250;
			 TmpCount = SkipPtsCount;
		}
		else if(PtsCount < 4)
			return;
		PointCollection SkippedPtsCollection;
		if(this->IsDxfShape() || PtsCount >= 250)
		{
			PtsList XYPtsCollection;
			for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
			{		
				if(TmpCount >= SkipPtsCount)
				{
					SinglePoint* Spt = (*SptItem).second;
					Pt Point_XY;
					Point_XY.pt_index = Spt->PtID;
					Point_XY.x = Spt->X;
					Point_XY.y = Spt->Y;
					XYPtsCollection.push_back(Point_XY);
					TmpCount = 0;
				}
				else
					TmpCount++;
			}
			BESTFITOBJECT->orderpoints(&XYPtsCollection);
			for each(Pt Point_XY in XYPtsCollection)
			{
				SinglePoint* Spt = new SinglePoint();
				Spt->PtID = Point_XY.pt_index;
				Spt->X = Point_XY.x;
				Spt->Y = Point_XY.y;
				Spt->Z = this->PointsList->getList()[Spt->PtID]->Z;
				SkippedPtsCollection.Addpoint(Spt);
			}
		}
		else
		{
			for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
			{		
				if(TmpCount >= SkipPtsCount)
				{
					SinglePoint* Spt = (*SptItem).second;
					SinglePoint* Spt1 = new SinglePoint();
					Spt1->SetValues(Spt->X, Spt->Y, Spt->Z);
					SkippedPtsCollection.Addpoint(Spt1);
					TmpCount = 0;
				}
				else
					TmpCount++;
			}
		}
		if(this->ClosedSpline)
		{
			SinglePoint* FirstPt = new SinglePoint();
			SinglePoint* Spt2 = SkippedPtsCollection.getList()[SkippedPtsCollection.Pointscount() - 2];
			FirstPt->SetValues(Spt2->X, Spt2->Y, Spt2->Z);
			this->ControlPointsList->Addpoint(FirstPt);
			for(PC_ITER SptItem = SkippedPtsCollection.getList().begin(); SptItem != SkippedPtsCollection.getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				SinglePoint* Spt1 = new SinglePoint();
				Spt1->SetValues(Spt->X, Spt->Y, Spt->Z);
				this->ControlPointsList->Addpoint(Spt1);
			}
			SinglePoint* LastPt = new SinglePoint();
			Spt2 = SkippedPtsCollection.getList()[1];
			LastPt->SetValues(Spt2->X, Spt2->Y, Spt2->Z);
			this->ControlPointsList->Addpoint(LastPt);
		}
		else
		{
			for(PC_ITER SptItem = SkippedPtsCollection.getList().begin(); SptItem != SkippedPtsCollection.getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				SinglePoint* Spt1 = new SinglePoint();
				Spt1->SetValues(Spt->X, Spt->Y, Spt->Z);
				this->ControlPointsList->Addpoint(Spt1);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline00012a", __FILE__, __FUNCSIG__); }
}

void Spline::UpdateBestFit()
{
	try
	{
		if(DXFEXPOSEOBJECT->LoadDXF) return; //|| DXFEXPOSEOBJECT->AlignFlag
		if(this->framgrabShape)
			UpdateControlPointList();
		Spline_cpts = ControlPointsList->Pointscount(); 
		if(this->Spline_cpts < 4 || !this->DrawSpline) return;
		Spline_knots = Spline_degree + Spline_cpts + 1;
		if(this->CtrPtsArray != NULL)
		{
			delete [] CtrPtsArray; 
			CtrPtsArray = NULL;
		}
		this->CtrPtsArray = new double[this->Spline_cpts * 3];
		int n = 0;
		for(PC_ITER Spt = this->ControlPointsList->getList().begin(); Spt != this->ControlPointsList->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			this->CtrPtsArray[n++] = Pt->X;
			this->CtrPtsArray[n++] = Pt->Y;
			this->CtrPtsArray[n++] = Pt->Z;
		}
		
		ClearAllMemory();
		//define knots array for closed and open spline....
		if(this->framgrabShape || !this->IsDxfShape())
		{
			if(this->KnotsArray != NULL)
			{
				delete [] KnotsArray;
				this->KnotsArray = NULL;
			}
			KnotsArray = new double[Spline_knots];
			if(this->ClosedSpline)
			{
				for(int i = 0; i < Spline_knots; i++)
					KnotsArray[i] = i;
			}
			else
			{
				if(Spline_knots > 8)
				{
					for(int i = 4; i < Spline_knots - 4; i++)
						KnotsArray[i] = i - Spline_degree;
				}
				KnotsArray[0] = 0; KnotsArray[1] = 0; KnotsArray[2] = 0; KnotsArray[3] = 0; KnotsArray[Spline_knots - 4] = Spline_cpts - Spline_degree; KnotsArray[Spline_knots - 3] = Spline_cpts - Spline_degree; 
				KnotsArray[Spline_knots - 2] = Spline_cpts - Spline_degree; KnotsArray[Spline_knots - 1] = Spline_cpts - Spline_degree;
			}
		}
		if(!this->framgrabShape)
		{
			double tVar = KnotsArray[3], PtsFactor = KnotsArray[Spline_cpts];
			double dividefactor = (PtsFactor - tVar) / (20 * Spline_cpts);
			//calculate basis coefficient......
			CleartmpMemory(true);
			double* TmpArray;
			for(double t = tVar; t <= PtsFactor; t += dividefactor)
			{
				CleartmpMemory(false);
				bool firstTime = true;
				for(int j = 0; j <= Spline_degree; j++)
				{
					if(firstTime)
					{
						firstTime = false;
						TmpArray = new double[Spline_knots - 1];
						for(int k = 0; k < Spline_knots - 1; k++)
						{
							if(KnotsArray[k] <= t && KnotsArray[k + 1] > t)
								TmpArray[k] = 1;
							else
								TmpArray[k] = 0;
						}
					}
					else
					{
						TmpArray = new double[Spline_cpts];
						for(int k = 0; k < Spline_cpts; k++)
						{
							double Tmp1 = KnotsArray[j + k] - KnotsArray[k], Tmp2 = 0, Tmp3 = 0, Tmp4 = 0, Tmp5 = 0, Tmp6 = 0;
							if(Tmp1 != 0)
							{
								Tmp2 = (t - KnotsArray[k]) * TmpCoefficientList[j - 1][k];
								Tmp5 = Tmp2 / Tmp1;
							}
							Tmp3 = KnotsArray[j + k + 1] - KnotsArray[k + 1];
							if(Tmp3 != 0)
							{
								Tmp4 = (KnotsArray[j + k + 1] - t) * TmpCoefficientList[j - 1][k + 1];
								Tmp6 = Tmp4 / Tmp3;
							}
							TmpArray[k] = Tmp5 + Tmp6;
						}
					}
					if(j == Spline_degree)
					{
						basisCoefficientList[t] = TmpArray;
					}
					else
					{
						TmpCoefficientList[j] = TmpArray;
					}
				}
			}
			CleartmpMemory(false);

			int PtId = 0;
			//create points on Spline curve ........
			for(double t = tVar; t <= PtsFactor; t += dividefactor)
			{
				SinglePoint* Spt = new SinglePoint();
				Spt->PtID = PtId;
				for(int i = 0; i < Spline_cpts; i++)
				{
					SinglePoint* Spt1 = ControlPointsList->getList()[i];
					Spt->X += Spt1->X * basisCoefficientList[t][i];
					Spt->Y += Spt1->Y * basisCoefficientList[t][i];
					Spt->Z += Spt1->Z * basisCoefficientList[t][i];
				}
				PtId++;
				pointsOnCurve.push_back(Spt);
			}
			CleartmpMemory(true);
		}
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline00012", __FILE__, __FUNCSIG__); }
}

void Spline::CleartmpMemory(bool AllMemoryflag)
{
	try
	{
		while(TmpCoefficientList.size() != 0)
		{
			std::map<double, double*>::iterator itr = TmpCoefficientList.begin();
			double* Array_Pts = (*itr).second;
			TmpCoefficientList.erase((*itr).first);
			delete [] Array_Pts;
		}
		TmpCoefficientList.clear();

		if(AllMemoryflag)
		{
			while(basisCoefficientList.size() != 0)
			{
				std::map<double, double*>::iterator itr = basisCoefficientList.begin();
				double* Array_Pts = (*itr).second;
				basisCoefficientList.erase((*itr).first);
				delete [] Array_Pts;
			}
			basisCoefficientList.clear();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0013", __FILE__, __FUNCSIG__); }
}

 bool Spline::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		int n = this->ControlPointsList->Pointscount();
		double *TransformePpoint = NULL, *tempPoint = NULL;
		TransformePpoint = new double[(n + 1) * 2];
		tempPoint = new double[n * 4];
		int cnt = 0;
		for(PC_ITER SptItem = this->ControlPointsList->getList().begin(); SptItem != this->ControlPointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			tempPoint[cnt++] = Spt->X ; tempPoint[cnt++] = Spt->Y; tempPoint[cnt++] = Spt->Z; tempPoint[cnt++] = 1;
		}	
		 for(int i = 0; i < n; i++)
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &tempPoint[4 * i], &s2[0], &TransformePpoint[2 * i]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		delete [] tempPoint;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0014", __FILE__, __FUNCSIG__); return false;}
}

void Spline::UpdateForParentChange(BaseItem* sender)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0015", __FILE__, __FUNCSIG__); }
}

void Spline::Transform(double* transform)
{
	try
	{
		Vector Pt1;		
		for(PC_ITER SptItem = this->ControlPointsList->getList().begin(); SptItem != this->ControlPointsList->getList().end(); SptItem++)
		{	
			SinglePoint* Spt = (*SptItem).second;
			Pt1.set(MAINDllOBJECT->TransformMultiply(transform, Spt->X, Spt->Y, Spt->Z));
			Spt->SetValues(Pt1.getX(), Pt1.getY(), Pt1.getZ());
		}
		this->UpdateBestFit();
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0016", __FILE__, __FUNCSIG__); }
}

void Spline::AlignToBasePlane(double* trnasformM)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0017", __FILE__, __FUNCSIG__); }
}

void Spline::UpdateEnclosedRectangle()
{
	try
	{
		int n = this->ControlPointsList->Pointscount();
		double *TransformePpoint = NULL;
		TransformePpoint = new double[n * 2 + 1];
		int cnt = 0;
		for(PC_ITER SptItem = this->ControlPointsList->getList().begin(); SptItem != this->ControlPointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			TransformePpoint[cnt++] = Spt->X ; TransformePpoint[cnt++] = Spt->Y; 
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0},  *Temporary_point = NULL;	
			Temporary_point = new double[n * 3];
			cnt = 0;
			for(PC_ITER SptItem = this->ControlPointsList->getList().begin(); SptItem != this->ControlPointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				Temporary_point[cnt++] = Spt->X ; Temporary_point[cnt++] = Spt->Y; 
			}
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			for(int i = 0; i < n; i++)
				RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &Temporary_point[3 * i], Order2, &TransformePpoint[2 * i]);	
			delete [] Temporary_point;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, ShapeLeftTop, ShapeRightBottom);
		delete [] TransformePpoint;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0018", __FILE__, __FUNCSIG__); }
}

Shape* Spline::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("S");
		else
			myname = _T("dS");
		Spline* CShape = new Spline((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		{
		    CShape->CopyOriginalProperties(this);
			((Spline*)CShape)->ClosedSpline = this->ClosedSpline;
			((Spline*)CShape)->framgrabShape = this->framgrabShape;
			if(((Spline*)CShape)->KnotsArray != NULL)
			{
				delete [] ((Spline*)CShape)->KnotsArray;
				((Spline*)CShape)->KnotsArray = NULL;
			}
			((Spline*)CShape)->Spline_knots = this->Spline_knots;
			((Spline*)CShape)->KnotsArray = new double[this->Spline_knots];
			for(int i = 0; i < this->Spline_knots; i++)
				((Spline*)CShape)->KnotsArray[i] = this->KnotsArray[i];
			if(this->framgrabShape)
			{
				PointCollection& PtColl = *(((ShapeWithList*)this)->PointsList);
				for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					CShape->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
					CShape->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
				}
				CShape->UpdateShape();
			}
			/*else
			{
				for(PC_ITER SptItem = this->ControlPointsList->getList().begin(); SptItem != this->ControlPointsList->getList().end(); SptItem++)
				{
					SinglePoint* Spt = (*SptItem).second;
					((Spline*)CShape)->ControlPointsList->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
				}
			}*/
		}
		CShape->UpdateBestFit();
		return CShape;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0019", __FILE__, __FUNCSIG__); return NULL;
	}
}

Shape* Spline::CreateDummyCopy()
{
	try
	{
		Spline* CShape = new Spline(false);
		CShape->CopyOriginalProperties(this);
		CShape->ClosedSpline = this->ClosedSpline;
		CShape->framgrabShape = this->framgrabShape;
		if(this->framgrabShape)
		{
			PointCollection& PtColl = *(((ShapeWithList*)this)->PointsList);
			for(PC_ITER Spt = PtColl.getList().begin(); Spt != PtColl.getList().end(); Spt++)
			{
				SinglePoint* Sp = (*Spt).second;
				CShape->PointsListOriginal->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
				CShape->PointsList->Addpoint(new SinglePoint(Sp->X, Sp->Y, Sp->Z, Sp->R, Sp->Pdir));
			}
		}
		/*else
		{
			for(PC_ITER SptItem = this->ControlPointsList->getList().begin(); SptItem != this->ControlPointsList->getList().end(); SptItem++)
			{	
				CShape->ControlPointsList->Addpoint(new SinglePoint((*SptItem).second->X, (*SptItem).second->Y, (*SptItem).second->Z));
			}
		}*/
		CShape->setId(this->getId());
		CShape->UpdateShape();
		return CShape;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0020", __FILE__, __FUNCSIG__); return NULL;
	}
}

void Spline::CopyShapeParameters(Shape* s)
{
	try
	{
		this->ControlPointsList->deleteAll();
		ClearAllMemory();
		Spline *OrgShape = (Spline*)s;
		this->ClosedSpline = OrgShape->ClosedSpline;
		this->framgrabShape = OrgShape->framgrabShape;
		int n = OrgShape->ControlPointsList->Pointscount();
		if(this->KnotsArray != NULL)
		{
			delete [] KnotsArray;
			this->KnotsArray = NULL;
		}
		this->KnotsArray = new double[n + 4];
		for(int i = 0; i < n + 4; i++)
			this->KnotsArray[i] = OrgShape->KnotsArray[i];
		/*if(OrgShape->framgrabShape)
		{			
			this->PointsList->deleteAll();
			this->PointsListOriginal->deleteAll();
			for(PC_ITER SptItem = OrgShape->PointsList->getList().begin(); SptItem != OrgShape->PointsList->getList().end(); SptItem++)
			{	
				this->PointsList->Addpoint(new SinglePoint((*SptItem).second->X, (*SptItem).second->Y, (*SptItem).second->Z));
				this->PointsListOriginal->Addpoint(new SinglePoint((*SptItem).second->X, (*SptItem).second->Y, (*SptItem).second->Z));
			}
		}
		else
		{
			for(PC_ITER SptItem = OrgShape->ControlPointsList->getList().begin(); SptItem != OrgShape->ControlPointsList->getList().end(); SptItem++)
			{	
				this->ControlPointsList->Addpoint(new SinglePoint((*SptItem).second->X, (*SptItem).second->Y, (*SptItem).second->Z));
			}
		}*/
		if(!OrgShape->framgrabShape)
		{
			for(PC_ITER SptItem = OrgShape->ControlPointsList->getList().begin(); SptItem != OrgShape->ControlPointsList->getList().end(); SptItem++)
			{	
				this->ControlPointsList->Addpoint(new SinglePoint((*SptItem).second->X, (*SptItem).second->Y, (*SptItem).second->Z));
			}
		}
		this->UpdateBestFit();
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0021", __FILE__, __FUNCSIG__);
	}
}

wostream& operator<<(wostream& os, Spline& x)
{
	try
	{
		os.precision(16);
		os << (*static_cast<Shape*>(&x));
		os << "Spline" <<endl;
		os << "SplineType:" << x.ClosedSpline << endl;
		os << "framgrabShape:" << x.framgrabShape << endl;
		os << "SplineDegree:" << x.Spline_degree << endl;
		os << "ControlPtsValues:" << x.Spline_cpts << endl;
		for(PC_ITER SptItem = x.ControlPointsList->getList().begin(); SptItem != x.ControlPointsList->getList().end(); SptItem++)
		{	
		    SinglePoint* Spt = (*SptItem).second;
			os << Spt->X << endl;
			os << Spt->Y << endl;
			os << Spt->Z << endl;
		}
		os << "KnotsValues:" << x.Spline_knots << endl;
		for(int i = 0; i < x.Spline_knots; i++)
			os << x.KnotsArray[i] << endl;
		os<< "EndSpline" <<endl;
		return os;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0022", __FILE__, __FUNCSIG__); return os; 
	}
}

wistream& operator>>( wistream& is, Spline& x )
{
	try
	{
		is >> (*(Shape*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"Spline")
		{
			while(Tagname!=L"EndSpline")
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
					if(Tagname==L"SplineType")
					{
						if(Val=="0")
							x.ClosedSpline = false;
						else
							x.ClosedSpline = true;
					}
					else if(Tagname==L"framgrabShape")
					{
						if(Val=="0")
							x.framgrabShape = false;
						else
							x.framgrabShape = true;
					}
					else if(Tagname==L"SplineDegree")
					{
						x.Spline_degree = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"ControlPtsValues")
					{
						x.Spline_cpts = atoi((const char*)(Val).c_str());
						for(int i = 0; i < x.Spline_cpts; i++)
						{
							SinglePoint* Spt = new SinglePoint();
							is >> Spt->X;
							is >> Spt->Y;
							is >> Spt->Z;
							x.ControlPointsList->Addpoint(Spt);
						}
					}
					else if(Tagname==L"KnotsValues")
					{
						x.Spline_knots = atoi((const char*)(Val).c_str());
						if(x.KnotsArray != NULL)
						{
							delete [] x.KnotsArray;
							x.KnotsArray = NULL;
						}
						x.KnotsArray = new double[x.Spline_knots];
						for(int i = 0; i < x.Spline_knots; i++)
						{							
							is >> x.KnotsArray[i];													
						}
					}
				}
			}
		}
		return is;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Spline0023", __FILE__, __FUNCSIG__); return is; }
}