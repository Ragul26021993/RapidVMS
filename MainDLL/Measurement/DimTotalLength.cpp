#include "StdAfx.h"
#include "DimTotalLength.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\ShapeWithList.h"
#include"..\Engine\PointCollection.h"
#include"..\Shapes\SinglePoint.h"
#include"..\Engine\PointCollection.h"

DimTotalLength::DimTotalLength(TCHAR* myname, std::list<Shape*> ShapeColl, int MeasureType):DimBase(genName(myname))
{
	try
	{
		init();
		this->Measurement_Type = MeasureType;
		for(std::list<Shape*>::iterator item = ShapeColl.begin(); item != ShapeColl.end(); item++)
		{
			Shape* Cshape = *item;
			this->ParentShapeCollection.push_back(Cshape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0001", __FILE__, __FUNCSIG__); }
}

DimTotalLength::DimTotalLength(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0002", __FILE__, __FUNCSIG__); }
}

DimTotalLength::~DimTotalLength()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0003", __FILE__, __FUNCSIG__); }
}

void DimTotalLength::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIMTOTAL_LENGTH;
		this->ParentShapeCollection.clear();
		center[0] = 0; center[1] = 0; center[2] = 0;
		this->Measurement_Type = 0;
		this->ArcBestFitType = MAINDllOBJECT->ArcBestFitType_Dia;
	    this->PointsToBundle = MAINDllOBJECT->BestFit_PointsToBundle;
		IsValid(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimTotalLength::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		distno++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(distno, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimTotalLength::CalculateMeasurement(bool CalculateAgain)
{
	try
	{		
		if(CalculateAgain)
		{
			double PerimeterofSh = 0;
			for each(ShapeWithList* Cpshape in this->ParentShapeCollection)
			{
				if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
					PerimeterofSh += ((Line*)Cpshape)->Length();
				else if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
					PerimeterofSh += ((Circle*)Cpshape)->Length();
				else if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				{
					int PtsCount = Cpshape->PointsList->Pointscount();
					if(PtsCount < 2) return;
					int SkipCnt = 0, TmpCnt = 0;
					if(PtsCount > 300)
						SkipCnt = PtsCount / 300;
					double PtsArray[600] = {0};
					int PtsCnt = 0;
					for(PC_ITER SptItem = Cpshape->PointsList->getList().begin(); SptItem != Cpshape->PointsList->getList().end(); SptItem++)
					{
						if(TmpCnt >= SkipCnt)
						{
							TmpCnt = 0;
							SinglePoint* Spt = (*SptItem).second;
							double ProjectedPt[3] = {Spt->X, Spt->Y, Spt->Z};
							findProjectedPoint(ArcBestFitType, this->PointsToBundle, SptItem, Cpshape->PointsList, ProjectedPt);
							PtsArray[PtsCnt++] = ProjectedPt[0]; PtsArray[PtsCnt++] = ProjectedPt[1];
						}
						else
							TmpCnt++;
					}	
					double Plength = 0;
					BESTFITOBJECT->getLengthofShape(PtsArray, PtsCnt / 2, &Plength, 1, false);
					PerimeterofSh += Plength;
				}
			}
			if(this->Measurement_Type == 0)
				this->setDimension(PerimeterofSh);
			else
				this->setDimension(PerimeterofSh / M_PI);
		}		
		int pointsCnt = 0, MinimumPtsCnt = 250;
		for each(ShapeWithList* Cpshape in this->ParentShapeCollection)
		{
			if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				pointsCnt++;
			else if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				pointsCnt++;
			else if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				int PtsCnt = Cpshape->PointsList->Pointscount();
				if(PtsCnt > MinimumPtsCnt)
					pointsCnt += MinimumPtsCnt;
				else
					pointsCnt += Cpshape->PointsList->Pointscount();
			}
		}
		double* temporaryPoints = new double[pointsCnt * 2];
		int Cnt = 0;
		for(std::list<Shape*>::iterator item = this->ParentShapeCollection.begin(); item != this->ParentShapeCollection.end(); item++)
		{
			Shape* Cpshape = *item;
			if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				temporaryPoints[Cnt++] = ((Line*)Cpshape)->getMidPoint()->getX();
				temporaryPoints[Cnt++] = ((Line*)Cpshape)->getMidPoint()->getY();
			}
			else if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				temporaryPoints[Cnt++] = ((Circle*)Cpshape)->getMidPoint()->getX();
				temporaryPoints[Cnt++] = ((Circle*)Cpshape)->getMidPoint()->getY();
			}
			else if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				int TmpCnt = 0, PtsCnt = ((ShapeWithList*)Cpshape)->PointsList->Pointscount();
			 	int SkipCnt = 0;
				if(PtsCnt > MinimumPtsCnt)
					SkipCnt = PtsCnt / MinimumPtsCnt + 1;
				for(PC_ITER SptItem = ((ShapeWithList*)Cpshape)->PointsList->getList().begin(); SptItem != ((ShapeWithList*)Cpshape)->PointsList->getList().end(); SptItem++)
				{
					if(TmpCnt >= SkipCnt)
					{
						TmpCnt = 0;
						SinglePoint* Spt = (*SptItem).second;
						temporaryPoints[Cnt++] = Spt->X;
						temporaryPoints[Cnt++] = Spt->Y;
					}
					else
					{
						TmpCnt++;
					}			
				}
			}
		}		
		double point2[2] = {mposition.getX(), mposition.getY()};
		RMATH2DOBJECT->Nearestpoint_point(Cnt/2, temporaryPoints, &point2[0], &center[0]);
		delete [] temporaryPoints;
		RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(center, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0006", __FILE__, __FUNCSIG__); }
}

void DimTotalLength::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(center, 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0007", __FILE__, __FUNCSIG__); }
}

void DimTotalLength::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement();
}

void DimTotalLength::UpdateMeasurement()
{
	CalculateMeasurement(false);
}

DimBase* DimTotalLength::Clone(int n)
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimTotalLength::CreateDummyCopy()
{
	try
	{
		DimTotalLength* Cdim = new DimTotalLength(false);
		Cdim->ArcBestFitType = this->ArcBestFitType;
		Cdim->PointsToBundle = this->PointsToBundle;
		Cdim->Measurement_Type = this->Measurement_Type;
		for(std::list<Shape*>::iterator item = this->ParentShapeCollection.begin(); item != this->ParentShapeCollection.end(); item++)
		{
			Shape* Cpshape = *item;
			Cdim->ParentShapeCollection.push_back(Cpshape);
		}
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimTotalLength::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->ParentShapeCollection.clear();
		for(std::list<Shape*>::iterator item = ((DimTotalLength*)Cdim)->ParentShapeCollection.begin(); item != ((DimTotalLength*)Cdim)->ParentShapeCollection.end(); item++)
		{
			Shape* Cpshape = *item;
			this->ParentShapeCollection.push_back(Cpshape);
		}
		this->ArcBestFitType = ((DimTotalLength*)Cdim)->ArcBestFitType;
		this->PointsToBundle = ((DimTotalLength*)Cdim)->PointsToBundle;
		this->Measurement_Type = ((DimTotalLength*)Cdim)->Measurement_Type;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimTotalLength& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimTotalLength"<<endl;
		os <<"Measurement_Type:"<< x.Measurement_Type << endl;
		os <<"PointsToBundle:"<< x.PointsToBundle << endl;
		os <<"ArcBestFitType"<< x.ArcBestFitType << endl;
		os <<"ParentShapeCollId:"<< x.ParentShapeCollection.size() << endl;
		for(std::list<Shape*>::iterator item = x.ParentShapeCollection.begin(); item != x.ParentShapeCollection.end(); item++)
		{
			Shape* Cpshape = *item;
			os << Cpshape->getId() << endl;
		}
		os <<"EndDimTotalLength"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimTotalLength& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		int n = 0;
		if(Tagname==L"DimTotalLength")
		{
		while(Tagname!=L"EndDimTotalLength")
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
					if(Tagname==L"ParentShapeCollId")
					{
						n = atoi((const char*)(Val).c_str());
						for(int i = 0; i < n; i++)
						{
							int Id = 0;
							is >> Id;
							x.ParentShapeCollection.push_back((Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[Id]);
						}
					}	
					else if(Tagname == L"Measurement_Type")
					{
						x.Measurement_Type = atoi((const char*)(Val).c_str());
					}
					else if(Tagname == L"PointsToBundle")
					{
						x.PointsToBundle = atoi((const char*)(Val).c_str());
					}
					else if(Tagname == L"ArcBestFitType")
					{
						if(Val=="0")
							x.ArcBestFitType = false;						
						else
							x.ArcBestFitType = true;
					}
				}
			}
			x.UpdateMeasurement();
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
		
		return is;		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTLENGTH0012", __FILE__, __FUNCSIG__); return is; }
}

void DimTotalLength::findProjectedPoint(bool ArcBestFit, int BestFitPts, map<int,SinglePoint*>::iterator SptItem, PointCollection *PtsList, double* ProjectedPt)
{
	try
	{
		double Distance = 0;
		int PtsCount = BestFitPts * 2 + 1;
		double TempPoint[3] = {((*SptItem).second)->X, ((*SptItem).second)->Y, ((*SptItem).second)->Z};
		if(PtsCount < 3 || PtsCount > PtsList->Pointscount() - 1) return;
		double *PtsArray = new double[PtsCount * 2];
		int count = BestFitPts;
		for(int i = 0; i < count; i++)
		{
			if(SptItem == PtsList->getList().begin())
			{
				SptItem = PtsList->getList().end();
			}
			SptItem--;
		}
		for(int i = 0; i < PtsCount; i++)
		{
			if(SptItem == PtsList->getList().end())
			{
				SptItem = PtsList->getList().begin();
			}
			PtsArray[2 * i] = ((*SptItem).second)->X; PtsArray[2 * i + 1] = ((*SptItem).second)->Y;
			SptItem++;
		}
		if(ArcBestFit)
		{
			double ans[3] = {0}, angles[2] = {0};
			if(BESTFITOBJECT->Circle_BestFit(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc()))
			{
				RMATH2DOBJECT->ArcBestFitAngleCalculation(PtsArray, PtsCount, &ans[0], ans[2], &angles[0]);
				RMATH2DOBJECT->Projection_Of_Point_OnArc(ans, ans[2], angles[0], angles[0] + angles[1], TempPoint, ProjectedPt);	
			}
			else
			{
				BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
				RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], TempPoint, ProjectedPt);
			}
		}
		else
		{
			double ans[2] = {0};
			BESTFITOBJECT->Line_BestFit_2D(PtsArray, PtsCount, &ans[0], true, 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(ans[0], ans[1], TempPoint, ProjectedPt);
		}	
		delete [] PtsArray;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCDevH0014", __FILE__, __FUNCSIG__);}
}