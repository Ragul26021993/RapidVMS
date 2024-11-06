#include "StdAfx.h"
#include "DimSplineMeasurement.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\PointCollection.h"
#include "..\Engine\RCadApp.h"

DimSplineMeasurement::DimSplineMeasurement(TCHAR* myname, RapidEnums::MEASUREMENTTYPE ctype):DimBase(genName(myname, ctype))
{
	try
	{
		this->MeasurementType = ctype;
		TempCircle = new Circle(false);
		TempCircle->IsValid(false);
		EnteredSplineCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0001", __FILE__, __FUNCSIG__); }
}

DimSplineMeasurement::DimSplineMeasurement(bool simply):DimBase(false)
{
	try
	{
		TempCircle = new Circle(false);
		TempCircle->IsValid(false);
		EnteredSplineCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0002", __FILE__, __FUNCSIG__); }
}

DimSplineMeasurement::~DimSplineMeasurement()
{
	try
	{
		delete TempCircle;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimSplineMeasurement::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE ctype)
{
	try
	{
		this->MeasurementType = ctype;
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT)
			dimcnt = splinecnt++;
		else if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMIC)
			dimcnt = splinemic++;
		else if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMCC)
			dimcnt = splinemcc++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimSplineMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(&center[0], 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		//CurrentPointColl->drawAllpoints(5, 0, 1, 0);
		//GRAFIX->SetColor_Double(TempCircle->ShapeColor.r, TempCircle->ShapeColor.g, TempCircle->ShapeColor.b);
		TempCircle->drawShape(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0005", __FILE__, __FUNCSIG__); }
}

void DimSplineMeasurement::CalculateMeasurement(Shape* s1, int userentspcount)
{
	try
	{
		this->NominalValue(userentspcount);
		this->EnteredSplineCount = userentspcount;
		this->ParentShape1 = s1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimSplineMeasurement::ArrangePtsCalculateSplinecount()
{
	try
	{
		if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT)
			CalculateSplineCount();
		else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMIC)
			CalculateSplineMic();
		else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMCC)
			CalculateSplineMcc();
		RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(center, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimSplineMeasurement::CalculateSplineCount()
{
	try
	{
		CurrentPointColl->deleteAll();
		int SplineCount = 0;
		Circle* CShape = (Circle*)ParentShape1;
		double radius = CShape->Radius();
		double MaxRadius = CShape->MaxCircumdia() / 2;
		Vector* CircCenter = CShape->getCenter();
		CShape->getCenter()->FillDoublePointer(&center[0], 3);
		rad = 2 * CShape->Radius();

		if(CShape->PointsList->Pointscount() < 1)
		{
			setDimension(SplineCount);
			return;
		}
		////CShape->PointsList->ArrangePointsInOrder(PointCollection::POINTARRANGEMENTTYPE::NORMAL);
		//PointCollection* CPointColl = &CShape->PointsList;
		//PointCollection TempPointColl;

		////Get points outside the circle
		//for(PC_ITER PtItem = CPointColl->getList().begin(); PtItem != CPointColl->getList().end(); PtItem++)
		//{
		//	SinglePoint* CurrentPt = PtItem->second;
		//	double dist = RMATH2DOBJECT->Pt2Pt_distance(CircCenter->getX(), CircCenter->getY(), CurrentPt->X, CurrentPt->Y);
		//	if(dist > radius && dist < MaxRadius)
		//		TempPointColl.Addpoint(new SinglePoint(CurrentPt->X, CurrentPt->Y, CurrentPt->Z));
		//}

		////Spilt into clusters..get the collection of points
		//double Vwupp = MAINDllOBJECT->getWindow(0).getUppX();
		//PC_ITER PtItem = TempPointColl.getList().begin();
		//SinglePoint* CurrentPoint, *NextPoint;
		//CurrentPoint = PtItem->second;
		//PtItem++;
		//for(;PtItem != TempPointColl.getList().end(); PtItem++)
		//{
		//	NextPoint = PtItem->second;
		//	double dist = RMATH2DOBJECT->Pt2Pt_distance(CurrentPoint->X, CurrentPoint->Y, NextPoint->X, NextPoint->Y);
		//	double DistPixel = dist / Vwupp;
		//	if(DistPixel > 13) SplineCount++;
		//	CurrentPoint = NextPoint;
		//}


		//CurrentPoint = TempPointColl.getList().begin()->second;
		//double dist = RMATH2DOBJECT->Pt2Pt_distance(CurrentPoint->X, CurrentPoint->Y, NextPoint->X, NextPoint->Y);
		//double DistPixel = dist / Vwupp;
		//if(DistPixel > 13) SplineCount++;
		//setDimension(SplineCount);

		PointCollection* CPointColl = CShape->PointsList;
	
		//Spilt into clusters..get the collection of points
		double Vwupp = MAINDllOBJECT->getWindow(0).getUppX();
		bool PointsOutside = false;
		int PointsOutsidecount = 0;
		double PointsOutSideDist = 0;
		for(PC_ITER PtItem = CPointColl->getList().begin(); PtItem != CPointColl->getList().end(); PtItem++)
		{
			SinglePoint* CurrentPoint = PtItem->second;
			double dist = RMATH2DOBJECT->Pt2Pt_distance(CircCenter->getX(), CircCenter->getY(), CurrentPoint->X, CurrentPoint->Y);
			double DistDiffInPixel = (dist - radius) / Vwupp;
			if(DistDiffInPixel < -0.5 && DistDiffInPixel > -30)
			{
				if(PointsOutside)
				{
					if(PointsOutsidecount > 3 && (PointsOutSideDist/PointsOutsidecount) > 0.5)
					{
						PointsOutside = false;
						PointsOutsidecount = 0;
						SplineCount++;
					}
				}
				PointsOutSideDist = 0;
				PointsOutside = false;
				PointsOutsidecount = 0;
			}
			else if(DistDiffInPixel > 0.5 && DistDiffInPixel < 30)
			{
				PointsOutSideDist += DistDiffInPixel;
				PointsOutsidecount++;
				PointsOutside = true;
			}
		}
		setDimensionInteger(SplineCount);
		if(SplineCount == EnteredSplineCount)
			setDimension("Pass");
		else
		{
			if(SplineCount < EnteredSplineCount / 2.0)
				setDimension("No Spline");
			else if(SplineCount > (1.3 * EnteredSplineCount))
				setDimension("Double Broach");
			else
				setDimension("Fail");
		}
		/*if(SplineCount != 28 && SplineCount != 29)
			SplineCount = 0;*/
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimSplineMeasurement::CalculateSplineMic()
{
	try
	{
		CurrentPointColl->deleteAll();
		Circle* CShape = (Circle*)ParentShape1;
		double radius = CShape->Radius();
		Vector* CircCenter = CShape->getCenter();
		double CCenter[2];
		CircCenter->FillDoublePointer(&CCenter[0]);

		if(CShape->PointsList->Pointscount() < 1)
		{
			setDimension(0.0);
			return;
		}
	
		PointCollection* CPointColl = CShape->PointsList;
	
		//Spilt into clusters..get the collection of points which are inside the circle
		double Vwupp = MAINDllOBJECT->getWindow(0).getUppX();
		bool PointsOutside = false, PointsInside = false;
		int PointInsideCount = 0, PointsOutsidecount = 0;
		map<int, int> PointsArrangement;
		for(PC_ITER PtItem = CPointColl->getList().begin(); PtItem != CPointColl->getList().end(); PtItem++)
		{
			SinglePoint* CurrentPoint = PtItem->second;
			double dist = RMATH2DOBJECT->Pt2Pt_distance(CircCenter->getX(), CircCenter->getY(), CurrentPoint->X, CurrentPoint->Y);
			double DistDiffInPixel = (dist - radius) / Vwupp;
			if(DistDiffInPixel < 0 && DistDiffInPixel > -30)
			{
				PointInsideCount++;
				PointsInside = true;
			}
			else if(DistDiffInPixel > 0 && DistDiffInPixel < 30)
			{
				if(PointsInside)
				{
					if(PointInsideCount > 3)
						PointsArrangement[CurrentPoint->PtID - PointInsideCount] = CurrentPoint->PtID;
				}	
				PointsInside = false;
				PointInsideCount = 0;
			}
		}

		//get all nearest point to circle center from that collections
		PointCollection Nearestpointcoll;
		double* temppointcoll;
		for(map<int, int>::iterator ptitr = PointsArrangement.begin(); ptitr != PointsArrangement.end(); ptitr++)
		{
			int StartIndex = ptitr->first;
			int EndIndex = ptitr->second;
			double Nearpoint[2];
			int TotalPointcnt = EndIndex - StartIndex + 1;
			temppointcoll = (double*)malloc(sizeof(double) * TotalPointcnt * 2);
			int ptCount = 0;
			for(int i = StartIndex; i <= EndIndex; i++)
			{
				SinglePoint* CurrentPoint = CPointColl->getList()[i];
				//CurrentPointColl->Addpoint(new SinglePoint(CurrentPoint->X, CurrentPoint->Y));
				temppointcoll[ptCount++] = CurrentPoint->X;
				temppointcoll[ptCount++] = CurrentPoint->Y;
			}
			RMATH2DOBJECT->Nearestpoint_point(TotalPointcnt, temppointcoll, &CCenter[0], &Nearpoint[0]);
			Nearestpointcoll.Addpoint(new SinglePoint(Nearpoint[0], Nearpoint[1], CPointColl->getList().begin()->second->Z));
			free(temppointcoll);
		}
		CurrentPointColl->CopyAllPoints(&Nearestpointcoll);
		if(Nearestpointcoll.Pointscount() < 3)
		{
			setDimension(0.0);
			return;
		}
		//caculate a circle
		TempCircle->ResetCurrentParameters();
		TempCircle->AddPoints(&Nearestpointcoll);
		TempCircle->getCenter()->FillDoublePointer(&center[0], 3);
		rad = TempCircle->MinInscribedia();
		TempCircle->Radius(rad/2);
		setDimension(TempCircle->MinInscribedia());	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimSplineMeasurement::CalculateSplineMcc()
{
	try
	{
		Circle* CShape = (Circle*)ParentShape1;
		double radius = CShape->Radius();
		Vector* CircCenter = CShape->getCenter();
		double CCenter[2];
		CircCenter->FillDoublePointer(&CCenter[0]);

		if(CShape->PointsList->Pointscount() < 1)
		{
			setDimension(0.0);
			return;
		}
	
		PointCollection* CPointColl = CShape->PointsList;
	
		//Spilt into clusters..get the collection of points which are inside the circle
		double Vwupp = MAINDllOBJECT->getWindow(0).getUppX();
		bool PointsOutside = false, PointsInside = false;
		int PointInsideCount = 0, PointsOutsidecount = 0;
		map<int, int> PointsArrangement;
		for(PC_ITER PtItem = CPointColl->getList().begin(); PtItem != CPointColl->getList().end(); PtItem++)
		{
			SinglePoint* CurrentPoint = PtItem->second;
			double dist = RMATH2DOBJECT->Pt2Pt_distance(CircCenter->getX(), CircCenter->getY(), CurrentPoint->X, CurrentPoint->Y);
			double DistDiffInPixel = (dist - radius) / Vwupp;
			if(DistDiffInPixel < 0 && DistDiffInPixel > -30)
			{
				if(PointsOutside)
				{
					if(PointsOutsidecount > 3)
						PointsArrangement[CurrentPoint->PtID - PointsOutsidecount] = CurrentPoint->PtID;
				}
				PointsOutside = false;
				PointsOutsidecount = 0;	
			}
			else if(DistDiffInPixel > 0 && DistDiffInPixel < 30)
			{
				PointsOutsidecount++;
				PointsOutside = true;
			}
		}

		//get all nearest point to circle center from that collections
		PointCollection Farthestpointcoll;
		double* temppointcoll;
		for(map<int, int>::iterator ptitr = PointsArrangement.begin(); ptitr != PointsArrangement.end(); ptitr++)
		{
			int StartIndex = ptitr->first;
			int EndIndex = ptitr->second;
			double Farpoint[2];
			int TotalPointcnt = EndIndex - StartIndex + 1;
			temppointcoll = (double*)malloc(sizeof(double) * TotalPointcnt * 2);
			int ptCount = 0;
			for(int i = StartIndex; i <= EndIndex; i++)
			{
				SinglePoint* CurrentPoint = CPointColl->getList()[i];
				//CurrentPointColl->Addpoint(new SinglePoint(CurrentPoint->X, CurrentPoint->Y));
				temppointcoll[ptCount++] = CurrentPoint->X;
				temppointcoll[ptCount++] = CurrentPoint->Y;
			}
			RMATH2DOBJECT->Farthestpoint_point(TotalPointcnt, temppointcoll, &CCenter[0], &Farpoint[0]);
			Farthestpointcoll.Addpoint(new SinglePoint(Farpoint[0], Farpoint[1], CPointColl->getList().begin()->second->Z));
			free(temppointcoll);
		}
		CurrentPointColl->CopyAllPoints(&Farthestpointcoll);
		if(Farthestpointcoll.Pointscount() < 3)
		{
			setDimension(0.0);
			return;
		}

		//caculate a circle
		TempCircle->ResetCurrentParameters();
		TempCircle->AddPoints(&Farthestpointcoll);
		TempCircle->getCenter()->FillDoublePointer(&center[0], 3);
		rad = TempCircle->MaxCircumdia();
		TempCircle->Radius(rad/2);
		setDimension(TempCircle->MaxCircumdia());	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimSplineMeasurement::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, EnteredSplineCount);
}

void DimSplineMeasurement::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender, EnteredSplineCount);
	ArrangePtsCalculateSplinecount();
}

DimBase* DimSplineMeasurement::Clone(int n)
{
	try
	{
		DimSplineMeasurement* Cdim = new DimSplineMeasurement(this->MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimSplineMeasurement::CreateDummyCopy()
{
	try
	{
		DimSplineMeasurement* Cdim = new DimSplineMeasurement(false);
		Cdim->MeasurementType = this->MeasurementType;
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimSplineMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0009", __FILE__, __FUNCSIG__); }
}

int DimSplineMeasurement::splinecnt = 0;
int DimSplineMeasurement::splinemic = 0;
int DimSplineMeasurement::splinemcc = 0;
void DimSplineMeasurement::reset()
{
	splinecnt = 0;
	splinemic = 0;
	splinemcc = 0;
}

wostream& operator<<( wostream& os, DimSplineMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimSplineMeasurement"<<endl;
		os <<"EnteredSplineCount:"<< x.EnteredSplineCount << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"EndDimSplineMeasurement"<<endl;
		return os;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0010", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimSplineMeasurement& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"DimSplineMeasurement")
			{
				while(Tagname!=L"EndDimSplineMeasurement")
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
						if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}
						else if(Tagname==L"EnteredSplineCount")
						{
							x.EnteredSplineCount = atoi((const char*)(Val).c_str());
						}	
					 }
				}
				x.UpdateMeasurement();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimSplineMeasurement& x)
{
	try
	{
		int n;
		is >> x.EnteredSplineCount;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0011", __FILE__, __FUNCSIG__);  }
}