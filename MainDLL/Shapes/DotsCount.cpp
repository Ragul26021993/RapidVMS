#include "StdAfx.h"
#include "DotsCount.h"
#include "..\DXF\DXFExpose.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PointCollection.h"
#include "..\Actions\AddPointAction.h"

DotsCount::DotsCount(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

DotsCount::DotsCount(bool simply):ShapeWithList(false)
{
	init();
}

void DotsCount::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::DOTSCOUNT;
		this->ShapeAs3DShape(false);
		this->UseLightingProperties(false);
		this->MinimumPtsCount = MAINDllOBJECT->Dots_PointsMinCount; this->PixelCount = MAINDllOBJECT->Dots_PixelCount;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0001", __FILE__, __FUNCSIG__); }
}

DotsCount::~DotsCount()
{
	try
	{
		this->ClearAllMemory();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0001a", __FILE__, __FUNCSIG__); }
}

TCHAR* DotsCount::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0002", __FILE__, __FUNCSIG__); return name; }
}

bool DotsCount::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0001", __FILE__, __FUNCSIG__); return false;}
}

bool DotsCount::Shape_IsInWindow( Vector& corner1,Vector& corner2 )
{
	return false;
}

bool DotsCount::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		bool ReturnFlag = false;
		
		return ReturnFlag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0001", __FILE__, __FUNCSIG__); return false;}
}

void DotsCount::Translate(Vector& Position)
{
	try
	{
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0003", __FILE__, __FUNCSIG__);}
}

void DotsCount::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{			
		/*int totcount = this->PointsList->Pointscount();
		if(totcount > 0)
		{
			GRAFIX->drawPoints(pts, totcount, false, 2, FGPOINTSIZE);		
		}*/
		for each(double* TempPoint in CircleCollection)
		{
			double Circle3dparams[7] = {TempPoint[0], TempPoint[1], MAINDllOBJECT->getCurrentDRO().getZ(), 0, 0, 1, TempPoint[2]};
			GRAFIX->drawCircle3D(Circle3dparams);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0004", __FILE__, __FUNCSIG__); }
}

void DotsCount::drawGDntRefernce()
{
}

void DotsCount::ResetShapeParameters()
{
}

void DotsCount::UpdateBestFit()
{
	try
	{
		if(DXFEXPOSEOBJECT->LoadDXF) 
			return;
		IsValid(true);
		this->CreateDotsCollection();
		this->FrameArea = 0;
		for(list<BaseItem*>::iterator SptItem = this->PointAtionList.begin(); SptItem != this->PointAtionList.end(); SptItem++)
		{
			FramegrabBase* Fb = ((AddPointAction*)(*SptItem))->getFramegrab();
			if(Fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB || Fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE || Fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES)
			{
				double p1[2] = {Fb->myPosition[0].getX(), Fb->myPosition[0].getY()};
				double p2[2] = {Fb->myPosition[1].getX(), Fb->myPosition[1].getY()};
				double LeftTop[2] = {min(p1[0], p2[0]), min(p1[1], p2[1])};
				double width = max(p1[0], p2[0]) - LeftTop[0];
				double Height = max(p1[1], p2[1]) - LeftTop[1];
				this->FrameArea += width * Height * MAINDllOBJECT->getWindow(0).getUppX() * MAINDllOBJECT->getWindow(0).getUppY();
			}
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
			//this->pts[n++] = Pt->Z;
			this->filterpts[j++] = Pt->X;
			this->filterpts[j++] = Pt->Y;
			//this->filterpts[j++] = Pt->Z;
			filterptsCnt++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0006", __FILE__, __FUNCSIG__); }
}

bool DotsCount::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0007", __FILE__, __FUNCSIG__); return false;}
}

void DotsCount::UpdateForParentChange(BaseItem* sender)
{
}

void DotsCount::UpdateEnclosedRectangle()
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
		for(int i = 0; i < 3; i++)
		{
			FgPointLeftTop[i] = ShapeLeftTop[i];
			FgPointsRightBottom[i] = ShapeRightBottom[i];
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0008", __FILE__, __FUNCSIG__);}
}

void DotsCount::Transform(double* transform)
{
	try
	{
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0009", __FILE__, __FUNCSIG__); }
}

void DotsCount::AlignToBasePlane(double* trnasformM)
{
	try
	{
		this->UpdateEnclosedRectangle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0009", __FILE__, __FUNCSIG__); }
}

Shape* DotsCount::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("DC");
		else
			myname = _T("dDC");
		DotsCount* CShape = new DotsCount((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		    CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0010", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* DotsCount::CreateDummyCopy()
{
	try
	{
		DotsCount* CShape = new DotsCount(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0011", __FILE__, __FUNCSIG__); return NULL; }
}

void DotsCount::CopyShapeParameters(Shape* s)
{
	try
	{
		this->MinimumPtsCount = ((DotsCount*)s)->MinimumPtsCount;
		this->PixelCount = ((DotsCount*)s)->PixelCount;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0012", __FILE__, __FUNCSIG__); }
}

int DotsCount::shapenumber=0;

void DotsCount::reset()
{
	shapenumber = 0;
}

void DotsCount::GetShapeCenter(double *cPoint)
{
	try
	{
		if(!PointsList->getList().empty())
		{
			cPoint[0] = 0;
			cPoint[1] = 0;
			cPoint[2] = 0;
			for( PC_ITER Citem = this->PointsList->getList().begin(); Citem != this->PointsList->getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				cPoint[0] += Spt->X;
				cPoint[1] += Spt->Y;
				cPoint[2] += Spt->Z;
			}
			cPoint[0] /= PointsList->getList().size();
			cPoint[1] /= PointsList->getList().size();
			cPoint[2] /= PointsList->getList().size();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0013", __FILE__, __FUNCSIG__); }
}

void DotsCount::ClearAllMemory()
{
	try
	{
		while(DotsColl.size() != 0)
		{
			std::list<DotStruct*>::iterator itr = DotsColl.begin();
			DotStruct* MyStruct = (*itr);
			DotsColl.erase(itr);
			delete MyStruct;
		}
		DotsColl.clear();

		while(CircleCollection.size() != 0)
		{
			std::list<double*>::iterator itr = CircleCollection.begin();
			double* newPoint = (*itr);
			CircleCollection.erase(itr);
			delete [] newPoint;
		}
		CircleCollection.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0014", __FILE__, __FUNCSIG__); }
}

void DotsCount::CreateDotsCollection()
{
	try
	{
		double toleranceLimit = this->PixelCount * MAINDllOBJECT->getWindow(0).getUppX();
		this->ClearAllMemory();
		std::list<double*> TempPointsColl;
		for(PC_ITER SptItem = this->PointsList->getList().begin(); SptItem != this->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			double* TempPoint = new double[3]; 
			TempPoint[0] = Spt->X;  TempPoint[1] = Spt->Y; TempPoint[2] = Spt->Z;
			TempPointsColl.push_back(TempPoint);
		}
		std::list<DotStruct*> TempDotsColl;
		while(TempPointsColl.size() != 0)
		{
			std::list<double*>::iterator itr = TempPointsColl.begin();
			bool CreateDotStruct = true;
			double* newPoint = (*itr);
			for(std::list<DotStruct*>::iterator itr1 = TempDotsColl.begin(); itr1 != TempDotsColl.end(); itr1++)
			{
				DotStruct* MyDotStruct = (*itr1);
				bool PointsBelongtoThisDot = false;
				for each(double* TempPoint in MyDotStruct->PtsColl)
				{
					if(abs(TempPoint[0] - newPoint[0]) > toleranceLimit || abs(TempPoint[1] - newPoint[1]) > toleranceLimit) continue;
					if(RMATH2DOBJECT->Pt2Pt_distance(newPoint, TempPoint) < toleranceLimit)
					{
						PointsBelongtoThisDot = true;
						break;
					}
				}
				if(PointsBelongtoThisDot)
				{
					double* PtValue = new double[3];
					PtValue[0] = newPoint[0];  PtValue[1] = newPoint[1]; PtValue[2] = newPoint[2];
					MyDotStruct->PtsColl.push_back(PtValue);
					CreateDotStruct = false;
					break;
				}
			}
			if(CreateDotStruct)
			{
				DotStruct* MyDotStruct = new DotStruct();
				double* PtValue = new double[3];
				PtValue[0] = newPoint[0];  PtValue[1] = newPoint[1]; PtValue[2] = newPoint[2];
				MyDotStruct->PtsColl.push_back(PtValue);
				TempDotsColl.push_back(MyDotStruct);
			}
			TempPointsColl.erase(itr);
			delete [] newPoint;
		}

		//update dot collection which wiil have minimum points per dot defined by user...
		for(std::list<DotStruct*>::iterator itr1 = TempDotsColl.begin(); itr1 != TempDotsColl.end(); itr1++)
		{
			DotStruct* MyDotStruct = (*itr1);
			if(MyDotStruct->PtsColl.size() > MinimumPtsCount)
			{
				DotStruct* MyDotStruct1 = new DotStruct();
				for each(double* TempPoint in MyDotStruct->PtsColl)
				{
					double* PtValue = new double[3];
					PtValue[0] = TempPoint[0];  PtValue[1] = TempPoint[1]; PtValue[2] = TempPoint[2];
					MyDotStruct1->PtsColl.push_back(PtValue);
				}				
				DotsColl.push_back(MyDotStruct1);
			}
		}

		while(TempDotsColl.size() != 0)
		{
			std::list<DotStruct*>::iterator itr = TempDotsColl.begin();
			DotStruct* MyStruct = (*itr);
			TempDotsColl.erase(itr);
			delete MyStruct;
		}
		TempDotsColl.clear();
		CreateCircleCollection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0015", __FILE__, __FUNCSIG__); }
}

void DotsCount::CreateCircleCollection()
{
	try
	{
		for(std::list<DotStruct*>::iterator itr = DotsColl.begin(); itr != DotsColl.end(); itr++)
		{
			DotStruct* MyDotStruct = (*itr);
			int PtsCount = MyDotStruct->PtsColl.size();
			double* PtsArray = new double[PtsCount * 2];
			int Cnt = 0;
			for each(double* TempPoint in MyDotStruct->PtsColl)
			{
				PtsArray[Cnt++] = TempPoint[0];  PtsArray[Cnt++] = TempPoint[1]; 
			}	
			double ans[4] = {0};
			if(!BESTFITOBJECT->Circle_BestFit(PtsArray, PtsCount, &ans[0], SigmaMode(), 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
			{
				for(int i = 0; i < PtsCount; i++)
				{
					ans[0] += PtsArray[2 * i];
					ans[1] += PtsArray[2 * i + 1];
				}
				ans[0] = ans[0] / PtsCount;
				ans[1] = ans[1] / PtsCount;
				for(int i = 0; i < PtsCount; i++)
				{
					double dist = RMATH2DOBJECT->Pt2Pt_distance(ans, &PtsArray[2 * i]);
					if(dist > ans[2])
						ans[2] = dist;
				}
			}
			double angles[2] = {0};
			RMATH2DOBJECT->ArcBestFitAngleCalculation(PtsArray, PtsCount, &ans[0], ans[2], &angles[0]);
			delete [] PtsArray;
			if(angles[1] > M_PI)
			{
				double* CircleParam = new double[3];
				CircleParam[0] = ans[0]; CircleParam[1] = ans[1]; CircleParam[2] = ans[2];
				CircleCollection.push_back(CircleParam);
			}
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0016", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, DotsCount& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "DotsCount" << endl;
		os << "MinimumPtsCount:" << x.MinimumPtsCount << endl;
		os << "PixelCount:" << x.PixelCount  << endl;
		os << "EndDotsCount" << endl;
		return os;

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DC0017", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DotsCount& x )
{
	try
	{
		is >> (*(Shape*)&x);
		x.ShapeType = RapidEnums::SHAPETYPE::DOTSCOUNT;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DotsCount")
		{
			while(Tagname!=L"EndDotsCount")
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
					if(Tagname==L"MinimumPtsCount")
					{
						x.MinimumPtsCount = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"PixelCount")
					{
						x.PixelCount = atoi((const char*)(Val).c_str());
					}
				}
			}
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DC0018", __FILE__, __FUNCSIG__); return is; }
}