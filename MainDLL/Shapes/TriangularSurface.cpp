#include "StdAfx.h"
#include "TriangularSurface.h"
#include "SinglePoint.h"
#include "..\Engine\RCadApp.h"

TriangularSurface::TriangularSurface(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

TriangularSurface::TriangularSurface(bool simply):ShapeWithList(false)
{
	init();
}

void TriangularSurface::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::TRIANGULARSURFACE;
		this->ShapeAs3DShape(true);
		this->UseLightingProperties(true);
		TriangleCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0001", __FILE__, __FUNCSIG__); }
}

TriangularSurface::~TriangularSurface()
{
	TriangleCollection.clear();
	ClearIntersectionPtsMemory();
}

TCHAR* TriangularSurface::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0002", __FILE__, __FUNCSIG__); return name; }
}

bool TriangularSurface::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		if(MAINDllOBJECT->CheckStlHighlightedPt)
		{
			MAINDllOBJECT->StlHighlightedPointFlag = false;
			for(std::list<Triangle>::iterator TriangleShapeitr = TriangleCollection.begin(); TriangleShapeitr != TriangleCollection.end(); TriangleShapeitr++)
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(corner1.getX(), corner1.getY(), TriangleShapeitr->point1[0], TriangleShapeitr->point1[1]) < Tolerance)
				{
					ReturnFlag = true;
					MAINDllOBJECT->StlHighlightedPointFlag = true;
					MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point1[0], TriangleShapeitr->point1[1], TriangleShapeitr->point1[2]);
					break;
				}
				else if(RMATH2DOBJECT->Pt2Pt_distance(corner1.getX(), corner1.getY(), TriangleShapeitr->point2[0], TriangleShapeitr->point2[1]) < Tolerance)
				{
					ReturnFlag = true;
					MAINDllOBJECT->StlHighlightedPointFlag = true;
					MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point2[0], TriangleShapeitr->point2[1], TriangleShapeitr->point2[2]);
					break;
				}
				else if(RMATH2DOBJECT->Pt2Pt_distance(corner1.getX(), corner1.getY(), TriangleShapeitr->point3[0], TriangleShapeitr->point3[1]) < Tolerance)
				{
					ReturnFlag = true;
					MAINDllOBJECT->StlHighlightedPointFlag = true;
					MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point3[0], TriangleShapeitr->point3[1], TriangleShapeitr->point3[2]);
					break;
				}
			}
		}
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0001", __FILE__, __FUNCSIG__); return false;}
}

bool TriangularSurface::Shape_IsInWindow( Vector& corner1,Vector& corner2 )
{
	return false;
}

bool TriangularSurface::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		if(MAINDllOBJECT->CheckStlHighlightedPt)
		{
			MAINDllOBJECT->StlHighlightedPointFlag = false;
			for(std::list<Triangle>::iterator TriangleShapeitr = TriangleCollection.begin(); TriangleShapeitr != TriangleCollection.end(); TriangleShapeitr++)
			{
				if(RMATH3DOBJECT->Distance_Point_Line(TriangleShapeitr->point1, SelectionLine) < Tolerance)
				{
					ReturnFlag = true;
					MAINDllOBJECT->StlHighlightedPointFlag = true;
					MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point1[0], TriangleShapeitr->point1[1], TriangleShapeitr->point1[2]);
					break;
				}
				else if(RMATH3DOBJECT->Distance_Point_Line(TriangleShapeitr->point2, SelectionLine) < Tolerance)
				{
					ReturnFlag = true;
					MAINDllOBJECT->StlHighlightedPointFlag = true;
					MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point2[0], TriangleShapeitr->point2[1], TriangleShapeitr->point2[2]);
					break;
				}
				else if(RMATH3DOBJECT->Distance_Point_Line(TriangleShapeitr->point3, SelectionLine) < Tolerance)
				{
					ReturnFlag = true;
					MAINDllOBJECT->StlHighlightedPointFlag = true;
					MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point3[0], TriangleShapeitr->point3[1], TriangleShapeitr->point3[2]);
					break;
				}
			}
		}
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0001", __FILE__, __FUNCSIG__); return false;}
}

void TriangularSurface::Translate(Vector& Position)
{
	try
	{
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{	
		    TShapeitr->point1[0] += Position.getX(); TShapeitr->point1[1] += Position.getY(); TShapeitr->point1[2] += Position.getZ();
			TShapeitr->point2[0] += Position.getX(); TShapeitr->point2[1] += Position.getY(); TShapeitr->point2[2] += Position.getZ();
			TShapeitr->point3[0] += Position.getX(); TShapeitr->point3[1] += Position.getY(); TShapeitr->point3[2] += Position.getZ();
		}
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0003", __FILE__, __FUNCSIG__);}
}

void TriangularSurface::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{			
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			double trianglePts[9] = {TShapeitr->point1[0], TShapeitr->point1[1], TShapeitr->point1[2], TShapeitr->point2[0], TShapeitr->point2[1], TShapeitr->point2[2], TShapeitr->point3[0], TShapeitr->point3[1], TShapeitr->point3[2]};
			double PlaneMidPointColl[3] = {(TShapeitr->point1[0] + TShapeitr->point2[0] + TShapeitr->point3[0]) / 3, (TShapeitr->point1[1] + TShapeitr->point2[1] + TShapeitr->point3[1]) / 3,(TShapeitr->point1[2] + TShapeitr->point2[2] + TShapeitr->point3[2]) / 3};
			GRAFIX->EnableLighting(PlaneMidPointColl[0], PlaneMidPointColl[1], PlaneMidPointColl[2], TShapeitr->dirCosine[0], TShapeitr->dirCosine[1], TShapeitr->dirCosine[2]);
			GRAFIX->SetColor_Double(0, 0, 1);
			GRAFIX->draw_PlanePolygon(trianglePts, 3, MAINDllOBJECT->AllowTransparencyTo3DShapes(), MAINDllOBJECT->DrawWiredSurface());
			GRAFIX->DisableLighting();
		}

		if(MAINDllOBJECT->CheckStlHighlightedPt)
		{
			for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
			{
				GRAFIX->drawPoints(TShapeitr->point1, 1, false, 3, FGPOINTSIZE);
				GRAFIX->drawPoints(TShapeitr->point2, 1, false, 3, FGPOINTSIZE);
				GRAFIX->drawPoints(TShapeitr->point3, 1, false, 3, FGPOINTSIZE);
			}
		}
		
		/*for(std::list<std::list<TriangleIntersectPts*>*>::iterator ListIter1 = List_List_PtsArray.begin(); ListIter1 != List_List_PtsArray.end(); ListIter1++)
		{
			std::list<TriangleIntersectPts*>* List_PtsArray = (*ListIter1);
			for(std::list<TriangleIntersectPts*>::iterator ListIter2 = List_PtsArray->begin(); ListIter2 != List_PtsArray->end(); ListIter2++)
			{
				TriangleIntersectPts* PtsArray = (*ListIter2);	
				GRAFIX->drawPoints(PtsArray->Line_Pts, 2, false, 3, FGPOINTSIZE);
			}
		}*/

		/*for(std::list<TriangleIntersectPts*>::iterator ListIter2 = LineCollection.begin(); ListIter2 != LineCollection.end(); ListIter2++)
		{
			TriangleIntersectPts* PtsArray = (*ListIter2);	
			GRAFIX->drawLine_3D(PtsArray->Line_Pts[0], PtsArray->Line_Pts[1], PtsArray->Line_Pts[2], PtsArray->Line_Pts[3], PtsArray->Line_Pts[4], PtsArray->Line_Pts[5]);
		}*/


		/*for(std::list<std::list<TriangleIntersectPts*>*>::iterator ListIter1 = PolyLineCollection.begin(); ListIter1 != PolyLineCollection.end(); ListIter1++)
		{
			std::list<TriangleIntersectPts*>* List_PtsArray = (*ListIter1);
			for(std::list<TriangleIntersectPts*>::iterator ListIter2 = List_PtsArray->begin(); ListIter2 != List_PtsArray->end(); ListIter2++)
			{
				TriangleIntersectPts* PtsArray = (*ListIter2);	
				GRAFIX->drawPoints(PtsArray->Line_Pts, 2, false, 3, FGPOINTSIZE);
			}
		}*/

		for(std::list<SinglePoint*>::iterator itr = SolidGeometryPtsList.begin(); itr != SolidGeometryPtsList.end(); itr++)
		{		
			double pt[3] = {(*itr)->X, (*itr)->Y, (*itr)->Z};
			GRAFIX->drawPoints(pt, 1, false, 3, FGPOINTSIZE);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0004", __FILE__, __FUNCSIG__); }
}

void TriangularSurface::drawGDntRefernce()
{
}

void TriangularSurface::ResetShapeParameters()
{
}

void TriangularSurface::UpdateBestFit()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0006", __FILE__, __FUNCSIG__); }
}

bool TriangularSurface::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		int n = this->TriangleCollection.size() * 3;
		double *TransformePpoint = NULL, *tempPoint = NULL;
		TransformePpoint = new double[(n + 1) * 2];
		tempPoint = new double[n * 4];
		int cnt = 0;
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			tempPoint[cnt++] = TShapeitr->point1[0]; tempPoint[cnt++] = TShapeitr->point1[1]; tempPoint[cnt++] = TShapeitr->point1[2]; tempPoint[cnt++] = 1;
			tempPoint[cnt++] = TShapeitr->point2[0]; tempPoint[cnt++] = TShapeitr->point2[1]; tempPoint[cnt++] = TShapeitr->point2[2]; tempPoint[cnt++] = 1;
			tempPoint[cnt++] = TShapeitr->point3[0]; tempPoint[cnt++] = TShapeitr->point3[1]; tempPoint[cnt++] = TShapeitr->point3[2]; tempPoint[cnt++] = 1;
		}	
		 for(int i = 0; i < n; i++)
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &tempPoint[4 * i], &s2[0], &TransformePpoint[2 * i]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		delete [] tempPoint;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0007", __FILE__, __FUNCSIG__); return false;}
}

void TriangularSurface::UpdateForParentChange(BaseItem* sender)
{
}

void TriangularSurface::UpdateEnclosedRectangle()
{
	try
	{
		int n = this->TriangleCollection.size() * 3;
		double *TransformePpoint = NULL;
		TransformePpoint = new double[n * 2 + 1];
		int cnt = 0;
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			TransformePpoint[cnt++] = TShapeitr->point1[0]; TransformePpoint[cnt++] = TShapeitr->point1[1]; 
			TransformePpoint[cnt++] = TShapeitr->point2[0]; TransformePpoint[cnt++] = TShapeitr->point2[1]; 
			TransformePpoint[cnt++] = TShapeitr->point3[0]; TransformePpoint[cnt++] = TShapeitr->point3[1];
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0},  *Temporary_point = NULL;	
			Temporary_point = new double[n * 3];
			cnt = 0;
			for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
			{
				Temporary_point[cnt++] = TShapeitr->point1[0]; Temporary_point[cnt++] = TShapeitr->point1[1]; Temporary_point[cnt++] = TShapeitr->point1[2]; 
				Temporary_point[cnt++] = TShapeitr->point2[0]; Temporary_point[cnt++] = TShapeitr->point2[1]; Temporary_point[cnt++] = TShapeitr->point2[2]; 
				Temporary_point[cnt++] = TShapeitr->point3[0]; Temporary_point[cnt++] = TShapeitr->point3[1]; Temporary_point[cnt++] = TShapeitr->point3[2];
			}
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			for(int i = 0; i < n; i++)
				RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &Temporary_point[3 * i], Order2, &TransformePpoint[2 * i]);	
			delete [] Temporary_point;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, ShapeLeftTop, ShapeRightBottom);
		delete [] TransformePpoint;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0008", __FILE__, __FUNCSIG__);}
}

void TriangularSurface::Transform(double* transform)
{
	try
	{
		Vector Pt1, Pt2, Pt3, Pt4;
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{	
		    Pt1.set(MAINDllOBJECT->TransformMultiply(transform, TShapeitr->point1[0], TShapeitr->point1[1], TShapeitr->point1[2]));
			Pt2.set(MAINDllOBJECT->TransformMultiply(transform, TShapeitr->point2[0], TShapeitr->point2[1], TShapeitr->point2[2]));
			Pt3.set(MAINDllOBJECT->TransformMultiply(transform, TShapeitr->point3[0], TShapeitr->point3[1], TShapeitr->point3[2]));
			Pt4.set(MAINDllOBJECT->TransformMultiply(transform, TShapeitr->dirCosine[0], TShapeitr->dirCosine[1], TShapeitr->dirCosine[2]));			
			TShapeitr->point1[0] = Pt1.getX(); TShapeitr->point1[1] = Pt1.getY(); TShapeitr->point1[2] = Pt1.getZ();
			TShapeitr->point2[0] = Pt2.getX(); TShapeitr->point2[1] = Pt2.getY(); TShapeitr->point2[2] = Pt2.getZ();
			TShapeitr->point3[0] = Pt3.getX(); TShapeitr->point3[1] = Pt3.getY(); TShapeitr->point3[2] = Pt3.getZ();
			TShapeitr->dirCosine[0] = Pt4.getX(); TShapeitr->dirCosine[1] = Pt4.getY(); TShapeitr->dirCosine[2] = Pt4.getZ();
		}
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0009", __FILE__, __FUNCSIG__); }
}

void TriangularSurface::AlignToBasePlane(double* trnasformM)
{
	try
	{
		Vector Pt1, Pt2, Pt3, Pt4;
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			Pt1.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, TShapeitr->point1[0], TShapeitr->point1[1], TShapeitr->point1[2]));
			Pt2.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, TShapeitr->point2[0], TShapeitr->point2[1], TShapeitr->point2[2]));
			Pt3.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, TShapeitr->point3[0], TShapeitr->point3[1], TShapeitr->point3[2]));
			Pt4.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, TShapeitr->dirCosine[0], TShapeitr->dirCosine[1], TShapeitr->dirCosine[2]));			
			TShapeitr->point1[0] = Pt1.getX(); TShapeitr->point1[1] = Pt1.getY(); TShapeitr->point1[2] = Pt1.getZ();
			TShapeitr->point2[0] = Pt2.getX(); TShapeitr->point2[1] = Pt2.getY(); TShapeitr->point2[2] = Pt2.getZ();
			TShapeitr->point3[0] = Pt3.getX(); TShapeitr->point3[1] = Pt3.getY(); TShapeitr->point3[2] = Pt3.getZ();
			TShapeitr->dirCosine[0] = Pt4.getX(); TShapeitr->dirCosine[1] = Pt4.getY(); TShapeitr->dirCosine[2] = Pt4.getZ();
		}
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0009", __FILE__, __FUNCSIG__); }
}

Shape* TriangularSurface::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("TL");
		else
			myname = _T("dTL");
		TriangularSurface* CShape = new TriangularSurface((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		    CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0010", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* TriangularSurface::CreateDummyCopy()
{
	try
	{
		TriangularSurface* CShape = new TriangularSurface(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0011", __FILE__, __FUNCSIG__); return NULL; }
}

void TriangularSurface::CopyShapeParameters(Shape* s)
{
	try
	{
		this->TriangleCollection.clear();
		TriangularSurface *OrgShape = (TriangularSurface*)s;
		for(TShapeitr = OrgShape->TriangleCollection.begin(); TShapeitr != OrgShape->TriangleCollection.end(); TShapeitr++)
		{
			double TrianglePts[12] = {TShapeitr->point1[0], TShapeitr->point1[1], TShapeitr->point1[2], TShapeitr->point2[0], TShapeitr->point2[1],
				TShapeitr->point2[2], TShapeitr->point3[0] ,TShapeitr->point3[1], TShapeitr->point3[2], TShapeitr->dirCosine[0] ,TShapeitr->dirCosine[1], TShapeitr->dirCosine[2]};
			this->AddNewTriangle(TrianglePts);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0012", __FILE__, __FUNCSIG__); }
}

int TriangularSurface::shapenumber=0;

void TriangularSurface::reset()
{
	shapenumber = 0;
}

void TriangularSurface::GetShapeCenter(double *cPoint)
{
	try
	{
		double Pt[3] = {0};
		int Cnt = 0;
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			Cnt++;
			Pt[0] += (TShapeitr->point1[0] + TShapeitr->point2[0] + TShapeitr->point3[0] / 3);
			Pt[1] += (TShapeitr->point1[1] + TShapeitr->point2[1] + TShapeitr->point3[1] / 3);
			Pt[2] += (TShapeitr->point1[2] + TShapeitr->point2[2] + TShapeitr->point3[2] / 3);
		}
		cPoint[0] = Pt[0] / Cnt;
		cPoint[1] = Pt[1] / Cnt;
		cPoint[2] = Pt[2] / Cnt;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0013", __FILE__, __FUNCSIG__); }
}

void TriangularSurface::AddNewTriangle(double* Parameter)
{
	try
	{
		TriangleCount++;
		Triangle Tshape;
		Tshape.point1[0] = Parameter[0]; Tshape.point1[1] = Parameter[1]; Tshape.point1[2] = Parameter[2];
		Tshape.point2[0] = Parameter[3]; Tshape.point2[1] = Parameter[4]; Tshape.point2[2] = Parameter[5];
		Tshape.point3[0] = Parameter[6]; Tshape.point3[1] = Parameter[7]; Tshape.point3[2] = Parameter[8];
		Tshape.dirCosine[0] = Parameter[9]; Tshape.dirCosine[1] = Parameter[10]; Tshape.dirCosine[2] = Parameter[11];
		TriangleCollection.push_back(Tshape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0014", __FILE__, __FUNCSIG__); }
}

void TriangularSurface::CreateIntersectionPts_Triangle(double interval, int Axisno)
{
	try
	{
		ClearIntersectionPtsMemory();
		if(TriangleCollection.size() < 1) return;
		if(interval <= 0) return;

		IntersectInterval = interval;
		TShapeitr = TriangleCollection.begin();
		MinimumZ = TShapeitr->point1[Axisno];
		MaximumZ = TShapeitr->point1[Axisno];
		
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			if(MinimumZ > TShapeitr->point1[Axisno])
				MinimumZ = TShapeitr->point1[Axisno];
			if(MaximumZ < TShapeitr->point1[Axisno])
				MaximumZ = TShapeitr->point1[Axisno];
			if(MinimumZ > TShapeitr->point2[Axisno])
				MinimumZ = TShapeitr->point2[Axisno];
			if(MaximumZ < TShapeitr->point2[Axisno])
				MaximumZ = TShapeitr->point2[Axisno];
			if(MinimumZ > TShapeitr->point3[Axisno])
				MinimumZ = TShapeitr->point3[Axisno];
			if(MaximumZ < TShapeitr->point3[Axisno])
				MaximumZ = TShapeitr->point3[Axisno];
		}
		CurrentZ = MinimumZ + 0.001;
		GetSurfaceIntersectPointCollection();
		
		/*ClearIntersectionPtsMemory();
		if(TriangleCollection.size() < 1) return;
		if(interval <= 0) return;

		double MinZ = 0, MaxZ = 0;
		TShapeitr = TriangleCollection.begin();
		MinZ = TShapeitr->point1[Axisno];
		MaxZ = TShapeitr->point1[Axisno];
		
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			if(MinZ > TShapeitr->point1[Axisno])
				MinZ = TShapeitr->point1[Axisno];
			if(MaxZ < TShapeitr->point1[Axisno])
				MaxZ = TShapeitr->point1[Axisno];
			if(MinZ > TShapeitr->point2[Axisno])
				MinZ = TShapeitr->point2[Axisno];
			if(MaxZ < TShapeitr->point2[Axisno])
				MaxZ = TShapeitr->point2[Axisno];
			if(MinZ > TShapeitr->point3[Axisno])
				MinZ = TShapeitr->point3[Axisno];
			if(MaxZ < TShapeitr->point3[Axisno])
				MaxZ = TShapeitr->point3[Axisno];
		}
				
		for(double Value = MinZ; Value <= MaxZ; Value+= interval)
		{		
			std::list<TriangleIntersectPts*>* List_PtsArray = new std::list<TriangleIntersectPts*>();
			for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
			{
				double trianglePts[9] = {TShapeitr->point1[0], TShapeitr->point1[1], TShapeitr->point1[2], TShapeitr->point2[0], TShapeitr->point2[1], 
					TShapeitr->point2[2], TShapeitr->point3[0], TShapeitr->point3[1], TShapeitr->point3[2]};
				if(CheckTriangleIntersectionwithPlane(trianglePts, Value, Axisno))
				{
					double Answer[6] = {0};
					if(RMATH3DOBJECT->Plane_Triangle_Intersection(trianglePts, Value, Axisno, Answer))
					{
						TriangleIntersectPts* PtsArray = new TriangleIntersectPts();
						PtsArray->Line_Pts[0] = Answer[0]; PtsArray->Line_Pts[1] = Answer[1]; PtsArray->Line_Pts[2] = Answer[2];
						PtsArray->Line_Pts[3] = Answer[3]; PtsArray->Line_Pts[4] = Answer[4]; PtsArray->Line_Pts[5] = Answer[5];
						List_PtsArray->push_back(PtsArray);
					}
				}
			}
			if(List_PtsArray->size() > 0)
				List_List_PtsArray.push_back(List_PtsArray);
			else
				delete List_PtsArray;
		}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0015", __FILE__, __FUNCSIG__); }
}

bool TriangularSurface::CheckTriangleIntersectionwithPlane(double* PtsArray, double Value, int Axisno)
{
	try
	{
		if(Value > PtsArray[Axisno] && Value > PtsArray[Axisno + 3] && Value > PtsArray[Axisno + 6]) return false;
		if(Value < PtsArray[Axisno] && Value < PtsArray[Axisno + 3] && Value < PtsArray[Axisno + 6]) return false;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0016", __FILE__, __FUNCSIG__); return false;}
}

void TriangularSurface::ClearIntersectionPtsMemory()
{
	try
	{
		ClearLinecollectionMemory();
		ClearPolyLineMemory();
		ClearSolidGemotrypointsMemory();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0017", __FILE__, __FUNCSIG__);}
}

void TriangularSurface::GetSurfaceIntersectPointCollection()
{
	try
	{
		ClearLinecollectionMemory();
		if(CurrentZ > MaximumZ + 0.001) return;	
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			double trianglePts[9] = {TShapeitr->point1[0], TShapeitr->point1[1], TShapeitr->point1[2], TShapeitr->point2[0], TShapeitr->point2[1], 
				TShapeitr->point2[2], TShapeitr->point3[0], TShapeitr->point3[1], TShapeitr->point3[2]};
			if(CheckTriangleIntersectionwithPlane(trianglePts, CurrentZ, 2))
			{
				double Answer[6] = {0};
				if(RMATH3DOBJECT->Plane_Triangle_Intersection(trianglePts, CurrentZ, 2, Answer))
				{
					TriangleIntersectPts* PtsArray = new TriangleIntersectPts();
					PtsArray->Line_Pts[0] = Answer[0]; PtsArray->Line_Pts[1] = Answer[1]; PtsArray->Line_Pts[2] = Answer[2];
					PtsArray->Line_Pts[3] = Answer[3]; PtsArray->Line_Pts[4] = Answer[4]; PtsArray->Line_Pts[5] = Answer[5];
					LineCollection.push_back(PtsArray);
				}
			}
		}	
		CreatePolyLineCollection();
		CreatePointsforSolidgeometry();
		CurrentZ += IntersectInterval;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0018", __FILE__, __FUNCSIG__);}
}

void TriangularSurface::CreatePolyLineCollection()
{
	try
	{	
		ClearPolyLineMemory();
		if(LineCollection.size() < 1) return;
		bool Closedloop = false;
		std::list<TriangleIntersectPts*>* List_PtsArray = new std::list<TriangleIntersectPts*>();
		std::list<TriangleIntersectPts*>::iterator ListIter2 = LineCollection.begin();
		TriangleIntersectPts* PtsArray = (*ListIter2);
		LineCollection.remove(PtsArray);
		List_PtsArray->push_back(PtsArray);
		//delete PtsArray;
		while(LineCollection.size() != 0)
		{
			Closedloop = false;
			double Last_endPt[3] = {0};
			if(List_PtsArray->size() > 0)
			{
				std::list<TriangleIntersectPts*>::iterator ListIter2 = List_PtsArray->end();
				ListIter2--;
				TriangleIntersectPts* LinePtsArray = (*ListIter2);	
				Last_endPt[0] = LinePtsArray->Line_Pts[3]; Last_endPt[1] = LinePtsArray->Line_Pts[4]; Last_endPt[2] = LinePtsArray->Line_Pts[5];
			}
			bool foundNeighbourLine = false;
			for(std::list<TriangleIntersectPts*>::iterator ListIter = LineCollection.begin(); ListIter != LineCollection.end(); ListIter++)
			{
				TriangleIntersectPts* PtsArray1 = (*ListIter);	
				int Cnt = CheckNeighbourLines(Last_endPt, PtsArray1->Line_Pts);
				if(Cnt == 1)
				{
					foundNeighbourLine = true;
					LineCollection.remove(PtsArray1);
					List_PtsArray->push_back(PtsArray1);	
					break;
				}
				else if(Cnt == 2)
				{
					//reorder of line pts..
					TriangleIntersectPts* Lpts = new TriangleIntersectPts();
					Lpts->Line_Pts[0] = PtsArray1->Line_Pts[3]; Lpts->Line_Pts[1] = PtsArray1->Line_Pts[4]; Lpts->Line_Pts[2] = PtsArray1->Line_Pts[5];
					Lpts->Line_Pts[3] = PtsArray1->Line_Pts[0]; Lpts->Line_Pts[4] = PtsArray1->Line_Pts[1]; Lpts->Line_Pts[5] = PtsArray1->Line_Pts[2];
					List_PtsArray->push_back(Lpts);	
					foundNeighbourLine = true;
					LineCollection.remove(PtsArray1);
					delete PtsArray1;
					break;
				}
			}
			if(foundNeighbourLine)
			{
				std::list<TriangleIntersectPts*>::iterator itr = List_PtsArray->begin();
				TriangleIntersectPts* LinePtsArray = (*itr);	
				double firstPt[3] = {LinePtsArray->Line_Pts[0], LinePtsArray->Line_Pts[1], LinePtsArray->Line_Pts[2]};		
				std::list<TriangleIntersectPts*>::iterator itr1 = List_PtsArray->end();
				itr1--;
				TriangleIntersectPts* LineArray = (*itr1);	
				double endPt[3] = {LineArray->Line_Pts[3], LineArray->Line_Pts[4], LineArray->Line_Pts[5]};
				if(abs(firstPt[0] - endPt[0]) < 0.0001 && abs(firstPt[1] - endPt[1]) < 0.0001 && abs(firstPt[2] - endPt[2]) < 0.0001)
				{
					Closedloop = true;
					PolyLineCollection.push_back(List_PtsArray);
					if(LineCollection.size() > 0)
					{
						List_PtsArray = new std::list<TriangleIntersectPts*>();
						std::list<TriangleIntersectPts*>::iterator itr2 = LineCollection.begin();
						TriangleIntersectPts* Lpts = (*itr2);
						LineCollection.remove(Lpts);
						List_PtsArray->push_back(Lpts);
					}
				}
			}
			else
			{
				//if we wont get closed loop then raise a message
				MAINDllOBJECT->ShowMsgBoxString("Deviation001", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				break;
			}
		}
		if(!Closedloop)
		{
			while(List_PtsArray->size() != 0)
			{
				std::list<TriangleIntersectPts*>::iterator ListIter2 = List_PtsArray->begin();
				TriangleIntersectPts* PtsArray = (*ListIter2);			
				List_PtsArray->remove(PtsArray);
				delete PtsArray;
			}
			List_PtsArray->clear();
			delete List_PtsArray;
		}
	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0019", __FILE__, __FUNCSIG__);}
}

int TriangularSurface::CheckNeighbourLines(double* pt, double* endpts)
{
	try
	{
		if(abs(pt[0] - endpts[0]) < 0.0001 && abs(pt[1] - endpts[1]) < 0.0001 && abs(pt[2] - endpts[2]) < 0.0001)
			return 1;
		else if(abs(pt[0] - endpts[3]) < 0.0001 && abs(pt[1] - endpts[4]) < 0.0001 && abs(pt[2] - endpts[5]) < 0.0001)
			return 2;
		else
			return 0;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0020", __FILE__, __FUNCSIG__);  return 0;}
}

void TriangularSurface::CreatePointsforSolidgeometry()
{
	try
	{
		ClearSolidGemotrypointsMemory();
		double MinX = 0, MaxX = 0, MinY = 0, MaxY = 0, Pointsgap = 0.3;
		bool firstTime = true;
		for(std::list<std::list<TriangleIntersectPts*>*>::iterator ListIter1 = PolyLineCollection.begin(); ListIter1 != PolyLineCollection.end(); ListIter1++)
		{
			std::list<TriangleIntersectPts*>* List_PtsArray = (*ListIter1);
			for(std::list<TriangleIntersectPts*>::iterator ListIter2 = List_PtsArray->begin(); ListIter2 != List_PtsArray->end(); ListIter2++)
			{
				TriangleIntersectPts* PtsArray = (*ListIter2);	
				if(firstTime)
				{
					firstTime = false;
					MinX = PtsArray->Line_Pts[0]; MaxX = PtsArray->Line_Pts[0]; MinY = PtsArray->Line_Pts[1]; MaxY = PtsArray->Line_Pts[1];
				}
				if(MinX > PtsArray->Line_Pts[0])
					MinX = PtsArray->Line_Pts[0];
				if(MinX > PtsArray->Line_Pts[3])
					MinX = PtsArray->Line_Pts[3];
				if(MaxX < PtsArray->Line_Pts[0])
					MaxX = PtsArray->Line_Pts[0];
				if(MaxX < PtsArray->Line_Pts[3])
					MaxX = PtsArray->Line_Pts[3];
				if(MinY > PtsArray->Line_Pts[1])
					MinY = PtsArray->Line_Pts[1];
				if(MinY > PtsArray->Line_Pts[4])
					MinY = PtsArray->Line_Pts[4];
				if(MaxY < PtsArray->Line_Pts[1])
					MaxY = PtsArray->Line_Pts[1];
				if(MaxY < PtsArray->Line_Pts[4])
					MaxY = PtsArray->Line_Pts[4];
			}
		}	
		//MinX += 0.001, MaxX -= 0.001, MinY += 0.001, MaxY -= 0.001;
		for(double row = MinX; row <= MaxX; row += Pointsgap)
		{
			for(double column = MinY; column <= MaxY; column += Pointsgap)	
			{
				double Pt[3] = {row, column, CurrentZ};
				if(GetNumber_Of_Intersection(Pt) % 2 != 0)
					SolidGeometryPtsList.push_back(new SinglePoint(Pt[0], Pt[1], Pt[2]));			
			}
		}	
		for(double column = MinY; column <= MaxY; column += Pointsgap)	
		{
			double Pt[3] = {MaxX, column, CurrentZ};
			if(GetNumber_Of_Intersection(Pt) % 2 != 0)
				SolidGeometryPtsList.push_back(new SinglePoint(Pt[0], Pt[1], Pt[2]));		
		}
		double Pt[3] = {MaxX, MaxY, CurrentZ};
		if(GetNumber_Of_Intersection(Pt) % 2 != 0)
			SolidGeometryPtsList.push_back(new SinglePoint(Pt[0], Pt[1], Pt[2]));
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0020", __FILE__, __FUNCSIG__);}
}

int TriangularSurface::GetNumber_Of_Intersection(double* Pts)
{
	try
	{
		int NoOfintersection = 0;
		for(std::list<std::list<TriangleIntersectPts*>*>::iterator ListIter1 = PolyLineCollection.begin(); ListIter1 != PolyLineCollection.end(); ListIter1++)
		{
			std::list<TriangleIntersectPts*>* List_PtsArray = (*ListIter1);
			for(std::list<TriangleIntersectPts*>::iterator ListIter2 = List_PtsArray->begin(); ListIter2 != List_PtsArray->end(); ListIter2++)
			{
				TriangleIntersectPts* PtsArray = (*ListIter2);
				if((Pts[1] - PtsArray->Line_Pts[1] > 0.0001 && Pts[1] - PtsArray->Line_Pts[4] > 0.0001) || (Pts[1] - PtsArray->Line_Pts[1] < 0.0001 && Pts[1] - PtsArray->Line_Pts[4] < 0.0001)) continue;
				double angle = 0, initercept = 0;
				if(abs(PtsArray->Line_Pts[0] - PtsArray->Line_Pts[3]) > 0.0001)
				{
					angle = (atan((PtsArray->Line_Pts[4] - PtsArray->Line_Pts[1])/(PtsArray->Line_Pts[3] - PtsArray->Line_Pts[0])));
					initercept = PtsArray->Line_Pts[1] - tan(angle) * PtsArray->Line_Pts[0];
				}
				else
				{
					angle = M_PI_2;
					initercept = PtsArray->Line_Pts[0];
				}
				if(angle < 0) 
					angle += M_PI;
				else if(angle > M_PI)
					angle -= M_PI;
				int Count[2] = {0};
				double intpt1[2] = {0}, intpt2[2] = {0};
				if(RMATH2DOBJECT->ray_finitelineint(PtsArray->Line_Pts, &PtsArray->Line_Pts[3], angle, initercept, Pts, 0, Pts[1], intpt1, intpt2, Count) == 1)
					NoOfintersection++;
			}
		}
		return NoOfintersection;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0022", __FILE__, __FUNCSIG__);return 0;}
}

void TriangularSurface::ClearLinecollectionMemory()
{
	try
	{	
		while(LineCollection.size() != 0)
		{
			std::list<TriangleIntersectPts*>::iterator ListIter2 = LineCollection.begin();
			TriangleIntersectPts* PtsArray = (*ListIter2);		
			LineCollection.remove(PtsArray);
			delete PtsArray;
		}
		LineCollection.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0023", __FILE__, __FUNCSIG__);}
}

void TriangularSurface::ClearPolyLineMemory()
{
	try
	{	
		while(PolyLineCollection.size() != 0)
		{
			std::list<std::list<TriangleIntersectPts*>*>::iterator ListIter1 = PolyLineCollection.begin();
			std::list<TriangleIntersectPts*>* List_PtsArray = (*ListIter1);
			while(List_PtsArray->size() != 0)
			{
				std::list<TriangleIntersectPts*>::iterator ListIter2 = List_PtsArray->begin();
				TriangleIntersectPts* PtsArray = (*ListIter2);			
				List_PtsArray->remove(PtsArray);
				delete PtsArray;
			}
			List_PtsArray->clear();
			delete List_PtsArray;
			PolyLineCollection.remove(List_PtsArray);
		}
		PolyLineCollection.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0024", __FILE__, __FUNCSIG__);}
}

void TriangularSurface::ClearSolidGemotrypointsMemory()
{
	try
	{	
		while(SolidGeometryPtsList.size() != 0)
		{
			std::list<SinglePoint*>::iterator itr = SolidGeometryPtsList.begin();
			SinglePoint* Spt = *itr;
			SolidGeometryPtsList.remove(Spt);
			delete Spt;
		}
		SolidGeometryPtsList.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0025", __FILE__, __FUNCSIG__);}
}