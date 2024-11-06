#include "StdAfx.h"
#include "AutoScanSurface.h"
#include "SinglePoint.h"
#include "..\Engine\RCadApp.h"

AutoScanSurface::AutoScanSurface(map<int, double> selectedPnts):ShapeWithList(genName(_T("AFS")))
{
	this->ShapeType = RapidEnums::SHAPETYPE::AUTOFOCUSSURFACE;
	SelectedPoints = NULL;
	SelectedPoints = new double[selectedPnts.size()];
	for(int i = 0; i < selectedPnts.size(); i++)
		SelectedPoints[i] = selectedPnts[i];
	SelectedPointsCnt = selectedPnts.size() / 3;
	SurfacePoints = NULL;
	TriangleList = NULL;
	TriangleCnt = NumberOfRows = NumberOfCols = 0;
	double* RectendPoitnsX = new double[SelectedPointsCnt];
	double* RectendPoitnsY = new double[SelectedPointsCnt];
	center[0] = 0;
	center[1] = 0;
	center[2] = 0;
	for(int i = 0; i < SelectedPointsCnt; i++)
	{
		RectendPoitnsX[i] = SelectedPoints[3 * i];
		RectendPoitnsY[i] = SelectedPoints[3 * i + 1];
		center[0] += SelectedPoints[3 * i];
		center[1] += SelectedPoints[3 * i + 1];
		center[2] += SelectedPoints[3 * i + 2];
	}
	if(SelectedPointsCnt > 0)
	{
    	center[0] /= SelectedPointsCnt;
     	center[1] /= SelectedPointsCnt;
		center[2] /= SelectedPointsCnt;
	}
	extentPnts[0] = RMATH2DOBJECT->minimum(RectendPoitnsX, SelectedPointsCnt);
	extentPnts[1] = RMATH2DOBJECT->minimum(RectendPoitnsY, SelectedPointsCnt);
	extentPnts[2] = RMATH2DOBJECT->maximum(RectendPoitnsX, SelectedPointsCnt);
	extentPnts[3] = RMATH2DOBJECT->maximum(RectendPoitnsY, SelectedPointsCnt);
	delete [] RectendPoitnsX; 
	delete [] RectendPoitnsY;
	this->IsValid(true);
	
}

AutoScanSurface::AutoScanSurface():ShapeWithList(genName(_T("AFS")))
{
	this->ShapeType = RapidEnums::SHAPETYPE::AUTOFOCUSSURFACE;
	SelectedPoints = NULL;
	SurfacePoints = NULL;
	TriangleList = NULL;
	TriangleCnt = NumberOfRows = NumberOfCols = 0;
	this->IsValid(true);
}

AutoScanSurface::~AutoScanSurface()
{
	if(SelectedPoints != NULL) {delete [] SelectedPoints; SelectedPoints = NULL;}
	if(SurfacePoints != NULL) {delete [] SurfacePoints; SurfacePoints = NULL;}
	if(TriangleList != NULL) {free(TriangleList); TriangleList = NULL;}
}

bool AutoScanSurface::CreateSurface(double width, double height)
{
	try
	{
		double* RectendPoitnsX = new double[SelectedPointsCnt];
	    double* RectendPoitnsY = new double[SelectedPointsCnt];
		for(int i = 0; i < SelectedPointsCnt; i++)
		{
			RectendPoitnsX[i] = SelectedPoints[3 * i];
			RectendPoitnsY[i] = SelectedPoints[3 * i + 1];
		}	
		extentPnts[0] = RMATH2DOBJECT->minimum(RectendPoitnsX, SelectedPointsCnt);
		extentPnts[1] = RMATH2DOBJECT->minimum(RectendPoitnsY, SelectedPointsCnt);
		extentPnts[2] = RMATH2DOBJECT->maximum(RectendPoitnsX, SelectedPointsCnt);
		extentPnts[3] = RMATH2DOBJECT->maximum(RectendPoitnsY, SelectedPointsCnt);
		delete [] RectendPoitnsX; 
		delete [] RectendPoitnsY;

     	double NextPosZ = -1000;
		double ScanWidth = extentPnts[2] - extentPnts[0];
		double ScanHeight = extentPnts[3] - extentPnts[1];
		NumberOfRows = (int)ceil(ScanHeight/height);
		NumberOfCols = (int)ceil(ScanWidth/width);
		int TotalNoOfCells = NumberOfRows * NumberOfCols, PointCnt = 0;
		if(SurfacePoints != NULL) {delete [] SurfacePoints; SurfacePoints = NULL;}
		SurfacePoints = new double[TotalNoOfCells * 3];
		for(int i = 0; i < NumberOfCols; i++)
		{
			for(int j = 0; j < NumberOfRows; j++)
			{
				if(i % 2 != 0)
				{
					SurfacePoints[PointCnt++] = extentPnts[0] + i * width + width/2;
					SurfacePoints[PointCnt++] = extentPnts[1] + (NumberOfRows - j - 1) * height + height/2;
				}
				else
				{
					SurfacePoints[PointCnt++] = extentPnts[0] + i * width  + width/2;
					SurfacePoints[PointCnt++] = extentPnts[1] + j * height + height/2;
				}
				SurfacePoints[PointCnt++] = NextPosZ;
			}	
		}
		BESTFITOBJECT->getZcoordInTriangleList(SurfacePoints, TotalNoOfCells, TriangleList, TriangleCnt, 0.3, 1.5);	
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0014", __FILE__, __FUNCSIG__); }
}

TCHAR* AutoScanSurface::genName(const TCHAR* prefix)
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

bool AutoScanSurface::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		/*	MAINDllOBJECT->StlHighlightedPointFlag = false;
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
			}*/
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0001", __FILE__, __FUNCSIG__); return false;}
}

bool AutoScanSurface::Shape_IsInWindow( Vector& corner1,Vector& corner2 )
{
	return false;
}

bool AutoScanSurface::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		//if(MAINDllOBJECT->CheckStlHighlightedPt)
		//{
		//	MAINDllOBJECT->StlHighlightedPointFlag = false;
		//	for(std::list<Triangle>::iterator TriangleShapeitr = TriangleCollection.begin(); TriangleShapeitr != TriangleCollection.end(); TriangleShapeitr++)
		//	{
		//		if(RMATH3DOBJECT->Distance_Point_Line(TriangleShapeitr->point1, SelectionLine) < Tolerance)
		//		{
		//			ReturnFlag = true;
		//			MAINDllOBJECT->StlHighlightedPointFlag = true;
		//			MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point1[0], TriangleShapeitr->point1[1], TriangleShapeitr->point1[2]);
		//			break;
		//		}
		//		else if(RMATH3DOBJECT->Distance_Point_Line(TriangleShapeitr->point2, SelectionLine) < Tolerance)
		//		{
		//			ReturnFlag = true;
		//			MAINDllOBJECT->StlHighlightedPointFlag = true;
		//			MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point2[0], TriangleShapeitr->point2[1], TriangleShapeitr->point2[2]);
		//			break;
		//		}
		//		else if(RMATH3DOBJECT->Distance_Point_Line(TriangleShapeitr->point3, SelectionLine) < Tolerance)
		//		{
		//			ReturnFlag = true;
		//			MAINDllOBJECT->StlHighlightedPointFlag = true;
		//			MAINDllOBJECT->StlHighlightedPt.set(TriangleShapeitr->point3[0], TriangleShapeitr->point3[1], TriangleShapeitr->point3[2]);
		//			break;
		//		}
		//	}
		//}
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0001", __FILE__, __FUNCSIG__); return false;}
}

void AutoScanSurface::Translate(Vector& Position)
{
	try
	{
		for(int i = 0; i < TriangleCnt; i++)
		{	
			for(int j = 0; j < 3; j++)
			{
		       this->TriangleList[9 * i + 3 * j] += Position.getX(); 
		       this->TriangleList[9 * i + 3 * j + 1] += Position.getY(); 
		       this->TriangleList[9 * i + 3 * j + 2] += Position.getZ(); 
			}
		}
		for(int i = 0; i < SelectedPointsCnt; i++)
		{
			this->SelectedPoints[3 * i] += Position.getX();
			this->SelectedPoints[3 * i + 1] += Position.getY();
			this->SelectedPoints[3 * i + 2] += Position.getZ();
		}
		for ( int i = 0; i < 2; i++)
		{
			this->extentPnts[2 * i] += Position.getX();
			this->extentPnts[2 * i + 1] += Position.getY();
		}
	
		this->center[0] += Position.getX();
		this->center[1] += Position.getY();
		this->center[2] += Position.getZ();
		//this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0003", __FILE__, __FUNCSIG__);}
}

void AutoScanSurface::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{			
		for(int i = 0; i < TriangleCnt; i++)
		{
			GRAFIX->draw_PlanePolygon(&TriangleList[9 * i], 3, true, MAINDllOBJECT->DrawWiredSurface());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0004", __FILE__, __FUNCSIG__); }
}

void AutoScanSurface::drawGDntRefernce()
{
}

void AutoScanSurface::ResetShapeParameters()
{
}

void AutoScanSurface::UpdateBestFit()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0006", __FILE__, __FUNCSIG__); }
}

bool AutoScanSurface::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1}, n = TriangleCnt * 3, cnt = 0;
		double *TransformePpoint = NULL, *tempPoint = NULL;
		TransformePpoint = new double[n * 2 + 2];
		tempPoint = new double[n * 4];
		for(int i = 0; i < TriangleCnt; i++)
		{
			tempPoint[cnt++] = TriangleList[9 * i]; tempPoint[cnt++] =TriangleList[9 * i + 1]; tempPoint[cnt++] = TriangleList[9 * i + 2]; tempPoint[cnt++] = 1;
			tempPoint[cnt++] = TriangleList[9 * i + 3]; tempPoint[cnt++] = TriangleList[9 * i + 4]; tempPoint[cnt++] = TriangleList[9 * i + 5]; tempPoint[cnt++] = 1;
			tempPoint[cnt++] = TriangleList[9 * i + 6]; tempPoint[cnt++] = TriangleList[9 * i + 7]; tempPoint[cnt++] = TriangleList[9 * i + 8]; tempPoint[cnt++] = 1;
		}	
		for(int i = 0; i < n; i++)
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], s1, &tempPoint[4 * i], s2, &TransformePpoint[2 * i]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		delete [] tempPoint;
		return true;
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("TL0007", __FILE__, __FUNCSIG__);
		return false;
	}
}

void AutoScanSurface::UpdateEnclosedRectangle()
{
	try
	{
		/*int n = TriangleCnt * 3, cnt = 0;
		double *TransformePpoint = NULL;
		TransformePpoint = new double[n * 3];
	
		for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		{
			TransformePpoint[cnt++] = TShapeitr->point1[0]; TransformePpoint[cnt++] = TShapeitr->point1[1]; 
			TransformePpoint[cnt++] = TShapeitr->point2[0]; TransformePpoint[cnt++] = TShapeitr->point2[1]; 
			TransformePpoint[cnt++] = TShapeitr->point3[0]; TransformePpoint[cnt++] = TShapeitr->point3[1];
		}*/

		//if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		//{
		//	int Order1[2] = {3, 3}, Order2[2] = {3, 1};
		//	double TransMatrix[9] = {0},  *Temporary_point = NULL;	
		//	Temporary_point = new double[n * 3];
		//	cnt = 0;
		//	for(TShapeitr = TriangleCollection.begin(); TShapeitr != TriangleCollection.end(); TShapeitr++)
		//	{
		//		Temporary_point[cnt++] = TShapeitr->point1[0]; Temporary_point[cnt++] = TShapeitr->point1[1]; Temporary_point[cnt++] = TShapeitr->point1[2]; 
		//		Temporary_point[cnt++] = TShapeitr->point2[0]; Temporary_point[cnt++] = TShapeitr->point2[1]; Temporary_point[cnt++] = TShapeitr->point2[2]; 
		//		Temporary_point[cnt++] = TShapeitr->point3[0]; Temporary_point[cnt++] = TShapeitr->point3[1]; Temporary_point[cnt++] = TShapeitr->point3[2];
		//	}
		//	RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
		//	for(int i = 0; i < n; i++)
		//		RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &Temporary_point[3 * i], Order2, &TransformePpoint[2 * i]);	
		//	delete [] Temporary_point;
		//}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TriangleList[0], TriangleCnt * 3,  3, ShapeLeftTop, ShapeRightBottom);
		//delete [] TransformePpoint;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0008", __FILE__, __FUNCSIG__);}
}

void AutoScanSurface::Transform(double* transform)
{
	try
	{
		Vector tmpV;
		for(int i = 0; i < TriangleCnt; i++)
		{
			tmpV = MAINDllOBJECT->TransformMultiply(transform, TriangleList[9 * i], TriangleList[9 * i + 1], TriangleList[9 * i + 2]);
			this->TriangleList[9 * i] = tmpV.getX();
			this->TriangleList[9 * i + 1] = tmpV.getY();
			this->TriangleList[9 * i + 2] = tmpV.getZ();
			tmpV = MAINDllOBJECT->TransformMultiply(transform, TriangleList[9 * i + 3], TriangleList[9 * i + 4], TriangleList[9 * i + 5]);
			this->TriangleList[9 * i + 3] = tmpV.getX();
			this->TriangleList[9 * i + 4] = tmpV.getY();
			this->TriangleList[9 * i + 5] = tmpV.getZ();
			tmpV = MAINDllOBJECT->TransformMultiply(transform, TriangleList[9 * i + 6], TriangleList[9 * i + 7], TriangleList[9 * i + 8]);
			this->TriangleList[9 * i + 6] = tmpV.getX();
			this->TriangleList[9 * i + 7] = tmpV.getY();
			this->TriangleList[9 * i + 8] = tmpV.getZ();
		}
		for(int i = 0; i < SelectedPointsCnt; i++)
		{
			tmpV = MAINDllOBJECT->TransformMultiply(transform, SelectedPoints[3 * i], SelectedPoints[3 * i + 1], 0);
			this->SelectedPoints[3 * i] = tmpV.getX();
			this->SelectedPoints[3 * i + 1] = tmpV.getY();
    	}
		for ( int i = 0; i < 2; i++)
		{
			tmpV = MAINDllOBJECT->TransformMultiply(transform, extentPnts[2 * i], extentPnts[2 * i + 1], 0);
			this->extentPnts[2 * i] = tmpV.getX();
			this->extentPnts[2 * i + 1] = tmpV.getY();
		}
		tmpV = MAINDllOBJECT->TransformMultiply(transform, center[0], center[1], center[2]);
		this->center[0] = tmpV.getX();
		this->center[1] = tmpV.getY();
		this->center[2] = tmpV.getZ();
		/*Vector Pt1, Pt2, Pt3, Pt4;
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
		this->UpdateEnclosedRectangle();*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0009", __FILE__, __FUNCSIG__); }
}

void AutoScanSurface::AlignToBasePlane(double* trnasformM)
{
	try
	{
		/*Vector Pt1, Pt2, Pt3, Pt4;
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
		this->UpdateEnclosedRectangle();*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0009", __FILE__, __FUNCSIG__); }
}

Shape* AutoScanSurface::Clone(int n, bool copyOriginalProperty)
{
	try
	{
	/*	std::wstring myname;
		if(n == 0)
			myname = _T("TL");
		else
			myname = _T("dTL");
		AutoScanSurface* CShape = new AutoScanSurface((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		    CShape->CopyOriginalProperties(this);*/
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0010", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* AutoScanSurface::CreateDummyCopy()
{
	try
	{
		/*AutoScanSurface* CShape = new AutoScanSurface(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());*/
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0011", __FILE__, __FUNCSIG__); return NULL; }
}

void AutoScanSurface::CopyShapeParameters(Shape* s)
{
	try
	{
		this->TriangleCnt = ((AutoScanSurface*)s)->TriangleCnt;
		if(this->TriangleList != NULL) {delete [] this->TriangleList; this->TriangleList = NULL;}
		this->TriangleList = new double[((AutoScanSurface*)s)->TriangleCnt * 9];
		for(int i = 0; i < ((AutoScanSurface*)s)->TriangleCnt; i++)
		{
			for(int j = 0; j < 9; j++)
			{
				this->TriangleList[9 * i + j] = ((AutoScanSurface*)s)->TriangleList[9 * i + j];
			}
		}
		this->SelectedPointsCnt = ((AutoScanSurface*)s)->SelectedPointsCnt;
	    if(this->SelectedPoints != NULL) {delete [] this->SelectedPoints; this->SelectedPoints = NULL;}
		this->SelectedPoints = new double[((AutoScanSurface*)s)->SelectedPointsCnt * 3];
		for(int i = 0; i < ((AutoScanSurface*)s)->SelectedPointsCnt; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				this->SelectedPoints[3 * i + j] = ((AutoScanSurface*)s)->SelectedPoints[3 * i + j];
			}
		}
		for ( int i = 0; i < 4; i++)
		{
		   this->extentPnts[i] =  ((AutoScanSurface*)s)->extentPnts[i];
		}
		this->center[0] =  ((AutoScanSurface*)s)->center[0];
		this->center[1] =  ((AutoScanSurface*)s)->center[1];
		this->center[2] =  ((AutoScanSurface*)s)->center[2];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TL0012", __FILE__, __FUNCSIG__); }
}

int AutoScanSurface::shapenumber=0;


void AutoScanSurface::reset()
{
	shapenumber = 0;
}

void AutoScanSurface::GetShapeCenter(double *cPoint)
{
	  cPoint[0] = this->center[0];
	  cPoint[1] = this->center[1];
	  cPoint[2] = this->center[2];	
}

void AutoScanSurface::UpdateForParentChange(BaseItem* sender)
{
}
