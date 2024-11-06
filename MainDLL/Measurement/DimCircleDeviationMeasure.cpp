#include "StdAfx.h"
#include "DimCircleDeviationMeasure.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Engine\PointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

DimCircleDeviationMeasure::DimCircleDeviationMeasure(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE;
		TempCircle = new Circle(false);
		TempCircle->IsValid(true);
		this->PtCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0001", __FILE__, __FUNCSIG__); }
}

DimCircleDeviationMeasure::DimCircleDeviationMeasure(bool simply):DimBase(false)
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE;
		TempCircle = new Circle(false);
		TempCircle->IsValid(true);
		this->PtCount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0002", __FILE__, __FUNCSIG__); }
}

DimCircleDeviationMeasure::~DimCircleDeviationMeasure()
{
	try
	{
		delete TempCircle;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimCircleDeviationMeasure::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0005", __FILE__, __FUNCSIG__); return _T("NA");}
} 

void DimCircleDeviationMeasure::ClearPointList()
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0005", __FILE__, __FUNCSIG__); return;}
}

void DimCircleDeviationMeasure::createtemporaryLine(double Pt1_X, double Pt1_Y, double Pt2_X, double Pt2_Y, int PtId)
{
	try
	{
		Line* myLine  = new Line();
		myLine->ShapeType = RapidEnums::SHAPETYPE::LINE;
		myLine->LineType = RapidEnums::LINETYPE::FINITELINE;
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(Pt1_X, Pt1_Y, 0));
		PtColl.Addpoint(new SinglePoint(Pt2_X, Pt2_Y, 0));
		myLine->AddPoints(&PtColl);		
		myLine->Normalshape(true);	
		myLine->IsValid(true);
		myLine->ShapeColor.set(1,1,1);
		this->ParentShape1->addChild(myLine);
		myLine->addParent(this->ParentShape1);
		if(DrawCount == 1 || DrawCount == 0) 
		{
			this->ParentShape2->addChild(myLine);
		    myLine->addParent(this->ParentShape2);
		}
		AddShapeAction::addShape(myLine);
		ShapePtRelation[PtId] = myLine->getId();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0005", __FILE__, __FUNCSIG__); return;}
}

void DimCircleDeviationMeasure::setDefaultLineParameter()
{
	try
	{
		for(list<BaseItem*>::iterator shapeiterator = ParentShape1->getChild().begin();shapeiterator != ParentShape1->getChild().end(); shapeiterator++)
		{
			ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				PointCollection PtColl;
				PtColl.Addpoint(new SinglePoint(0, 0, 0));
				PtColl.Addpoint(new SinglePoint(0, 0, 0));
				((Line*)Cshape)->PointsListOriginal->deleteAll();
				((Line*)Cshape)->AddPoints(&PtColl);		
				((Line*)Cshape)->UpdateBestFit();
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0005", __FILE__, __FUNCSIG__); return;}
}

void DimCircleDeviationMeasure::setLineParameter(double Pt1_X, double Pt1_Y, double Pt2_X, double Pt2_Y, int PtId)
{ 
	try
	{
		for(list<BaseItem*>::iterator shapeiterator = ParentShape1->getChild().begin();shapeiterator != ParentShape1->getChild().end(); shapeiterator++)
		{
			ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				if(((Line*)Cshape)->getId() == ShapePtRelation[PtId])
				{
					PointCollection PtColl;
					PtColl.Addpoint(new SinglePoint(Pt1_X, Pt1_Y, 0));
					PtColl.Addpoint(new SinglePoint(Pt2_X, Pt2_Y, 0));
					((Line*)Cshape)->PointsListOriginal->deleteAll();
					((Line*)Cshape)->AddPoints(&PtColl);		
					((Line*)Cshape)->UpdateBestFit();
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0005", __FILE__, __FUNCSIG__); return;}
}

//Assume shape1 as Circle and shape2 as arc..
void DimCircleDeviationMeasure::CalculateMeasurement(Shape *ps1, double *data)
{
	try
	{
		DrawCount = 0;
		CentRadius[0] = data[0], CentRadius[1] = data[1], CentRadius[2] = data[2];
		((Circle*)TempCircle)->setCenter(Vector(CentRadius[0], CentRadius[1], MAINDllOBJECT->getCurrentDRO().getZ()));
		((Circle*)TempCircle)->Radius(CentRadius[2]);
		this->ParentShape2 = TempCircle; this->ParentShape1 = ps1;
		if((ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CIRCLE || ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ARC) && (ParentShape2->ShapeType != RapidEnums::SHAPETYPE::CIRCLE || ParentShape2->ShapeType != RapidEnums::SHAPETYPE::ARC))
		{
			double center1[2] = {((Circle*)ParentShape2)->getCenter()->getX(), ((Circle*)ParentShape2)->getCenter()->getY()};
			for(PC_ITER SptItem = ((Circle*)ParentShape1)->PointsList->getList().begin(); SptItem != ((Circle*)ParentShape1)->PointsList->getList().end(); SptItem++)
			{
				double pt2[2] = {(*SptItem).second->X, (*SptItem).second->Y};
				double intsecPt[2];
				double slope, intercept;	
		         RMATH2DOBJECT->TwoPointLine(center1, pt2, &slope, &intercept);
				if(CalculateIntersectionPtOnCircle(pt2, ParentShape2, slope, intercept, intsecPt))	
				{
					if(!PPCALCOBJECT->IsPartProgramRunning())
					{
						PtCount++;
						createtemporaryLine(pt2[0], pt2[1], intsecPt[0], intsecPt[1], (*SptItem).second->PtID);	
					}
					else
					{
						if(((Circle*)ParentShape1)->PointsList->getList().size() > 2)
							setLineParameter(pt2[0], pt2[1], intsecPt[0], intsecPt[1], (*SptItem).second->PtID);
						else if(((Circle*)ParentShape1)->PointsList->getList().size() == 1)
							continue;
						else
							setDefaultLineParameter();
					}
				}					
			}
			setDimension(0.0);
		}
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0006", __FILE__, __FUNCSIG__); }
}

void DimCircleDeviationMeasure::CalculateMeasurement(Shape *s2, Shape *s1)
{
	try
	{
		DrawCount = 1;
		this->ParentShape1 = s1; this->ParentShape2 = s2;
		CentRadius[0] = ((Circle*)ParentShape2)->getCenter()->getX(), CentRadius[1] = ((Circle*)ParentShape2)->getCenter()->getX(), CentRadius[2] = ((Circle*)ParentShape2)->Radius();		
		if((ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CIRCLE || ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ARC) && (ParentShape2->ShapeType != RapidEnums::SHAPETYPE::CIRCLE || ParentShape2->ShapeType != RapidEnums::SHAPETYPE::ARC))
		{
			double center1[2] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY()};
			for(PC_ITER SptItem = ((Circle*)ParentShape2)->PointsList->getList().begin(); SptItem != ((Circle*)ParentShape2)->PointsList->getList().end(); SptItem++)
			{
				double pt2[2] = {(*SptItem).second->X, (*SptItem).second->Y};
				double intsecPt[2];
				double slope, intercept;	
		         RMATH2DOBJECT->TwoPointLine(center1, pt2, &slope, &intercept);
				if(CalculateIntersectionPtOnCircle(pt2, ParentShape1, slope, intercept, intsecPt))	
				{
					if(!PPCALCOBJECT->IsPartProgramRunning())
					{
						PtCount++;
						createtemporaryLine(pt2[0], pt2[1], intsecPt[0], intsecPt[1], (*SptItem).second->PtID);	
					}
					else
					{
						if(((Circle*)ParentShape1)->PointsList->getList().size() > 2)
							setLineParameter(pt2[0], pt2[1], intsecPt[0], intsecPt[1], (*SptItem).second->PtID);
						else if(((Circle*)ParentShape1)->PointsList->getList().size() == 1)
							continue;
						else
							setDefaultLineParameter();
					}
				}											
			}
			setDimension(0.0);
		}
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0006", __FILE__, __FUNCSIG__); }
}

void DimCircleDeviationMeasure::CalculateMeasurement(Shape *s1)
{
	try
	{
		DrawCount = 2;
		this->ParentShape1 = s1;
		CentRadius[0] = ((Circle*)ParentShape1)->getCenter()->getX(), CentRadius[1] = ((Circle*)ParentShape1)->getCenter()->getX(), CentRadius[2] = ((Circle*)ParentShape1)->Radius();		
		if((ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CIRCLE || ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ARC))
		{
			double center1[2] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY()};		
			for(PC_ITER SptItem = ((Circle*)ParentShape1)->PointsList->getList().begin(); SptItem != ((Circle*)ParentShape1)->PointsList->getList().end(); SptItem++)
			{
				double pt2[2] = {(*SptItem).second->X, (*SptItem).second->Y};
				double intsecPt[2];
				double slope, intercept;	
					RMATH2DOBJECT->TwoPointLine(center1, pt2, &slope, &intercept);
				if(CalculateIntersectionPtOnCircle(pt2, ParentShape1, slope, intercept, intsecPt))	
				{
					if(!PPCALCOBJECT->IsPartProgramRunning())
					{
						PtCount++;
						createtemporaryLine(pt2[0], pt2[1], intsecPt[0], intsecPt[1], (*SptItem).second->PtID);	
					}
					else
					{
						if(((Circle*)ParentShape1)->PointsList->getList().size() > 2)
							setLineParameter(pt2[0], pt2[1], intsecPt[0], intsecPt[1], (*SptItem).second->PtID);
						else if(((Circle*)ParentShape1)->PointsList->getList().size() == 1)
							continue;
						else
							setDefaultLineParameter();
					}
				}																												
			}						
			setDimension(0.0);
		}
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0006", __FILE__, __FUNCSIG__); }
}

bool DimCircleDeviationMeasure::CalculateIntersectionPtOnCircle(double* pt, Shape* s1, double slope, double intercept, double* ans)
{
	try
	{
		Circle* CircShape1 = (Circle*)s1;
		double center1[2] = {CircShape1->getCenter()->getX(), CircShape1->getCenter()->getY()};
		double intpoint1[4];
		int count = RMATH2DOBJECT->Line_circleintersection(slope, intercept, &center1[0], CircShape1->Radius(), &intpoint1[0]);
		if(count == 2)
		{
			double dist1 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[0]);
			double dist2 = RMATH2DOBJECT->Pt2Pt_distance(pt, &intpoint1[2]);
			if(dist1 < dist2)
			{
				ans[0] = intpoint1[0];
				ans[1] = intpoint1[1];
			}
			else
			{
				ans[0] = intpoint1[2];
				ans[1] = intpoint1[3];
			}
		}
		else if(count == 1)
		{
			ans[0] = intpoint1[0];
			ans[1] = intpoint1[1];
		}
		else
			return false;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0007", __FILE__, __FUNCSIG__); return false; }
}

void DimCircleDeviationMeasure::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(DrawCount == 0)
		{
			GRAFIX->SetColor_Double(1, 0, 0);
			((Circle*)TempCircle)->drawCurrentShape(windowno, WPixelWidth);
		}
		MAINDllOBJECT->UpdateShapesChanged();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0007", __FILE__, __FUNCSIG__); }
}

void DimCircleDeviationMeasure::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(DrawCount == 1)
		{
			if(sender->getId() == ParentShape2->getId())
				CalculateMeasurement((Shape*)sender, ParentShape1);
			else if(sender->getId() == ParentShape1->getId())
				CalculateMeasurement(ParentShape2, (Shape*)sender);
		}
		else if(DrawCount == 0)
			CalculateMeasurement((Shape*)sender, &CentRadius[0]);
		else if(DrawCount == 2)
			CalculateMeasurement((Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0008", __FILE__, __FUNCSIG__); }
}

void DimCircleDeviationMeasure::UpdateMeasurement()
{
	if(DrawCount == 0)
		CalculateMeasurement(ParentShape1, &CentRadius[0]);
	else if(DrawCount == 1)
		CalculateMeasurement(ParentShape2, ParentShape1);
	else if(DrawCount == 2)
		CalculateMeasurement(ParentShape1);
}

DimBase* DimCircleDeviationMeasure::Clone(int n)
{
	try
	{
		DimCircleDeviationMeasure* Cdim = new DimCircleDeviationMeasure();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0009", __FILE__, __FUNCSIG__); }
}

DimBase* DimCircleDeviationMeasure::CreateDummyCopy()
{
	try
	{
		DimCircleDeviationMeasure* Cdim = new DimCircleDeviationMeasure(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCircleDeviationMeasure::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->DrawCount = ((DimCircleDeviationMeasure*)Cdim)->DrawCount;
		this->PtCount  = ((DimCircleDeviationMeasure*)Cdim)->PtCount;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimCircleDeviationMeasure& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCircleDevationMeasure"<<endl;
		os <<"DrawCount:"<< x.DrawCount << endl;
		if( x.DrawCount == 0)
		{
			os << x.CentRadius[0] << endl;
			os << x.CentRadius[1] << endl;
			os << x.CentRadius[2] << endl;
			os << x.ParentShape1->getId() << endl;
		}
		else if(x.DrawCount == 1)
		{
			os << x.ParentShape1->getId() << endl;
		    os << x.ParentShape2->getId() << endl;
		}
		else if(x.DrawCount == 2)
			os << x.ParentShape1->getId() << endl;
		os <<"PtCount:"<< x.PtCount << endl;
		for(map<int, int>::iterator ptitr = x.ShapePtRelation.begin(); ptitr != x.ShapePtRelation.end(); ptitr++)
		{
			os << ptitr->first << endl;
			os << ptitr->second << endl;
		}
		os<<"EndDimCircleDevationMeasure"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCircleDeviationMeasure& x)
{
	try
	{
		int n, m, i, pts;
		is >> (*(DimBase*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"DimCircleDevationMeasure")
			{
			while(Tagname!=L"EndDimCircleDevationMeasure")
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
						std::string Val(TagVal.begin(), TagVal.end() );
						Tagname=Tag;												
						if(Tagname==L"DrawCount")
						{
							x.DrawCount = atoi((const char*)(Val).c_str());
							if(x.DrawCount == 0)
							{
								is >> x.CentRadius[0];
								is >> x.CentRadius[1];
								is >> x.CentRadius[2];
								is >> n;
								x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
							}
							else if(x.DrawCount == 1)
							{
								is >> n;
								x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
								is >> n;
								x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
							}
							else if(x.DrawCount == 2)
							{
								is >> n;
								x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
							}
						}		
						else if(Tagname==L"PtCount")
						{
							pts=atoi((const char*)(Val).c_str());
							for(i = 1; i <= pts; i++)
							{
								is >> n;
								is >> m;
								x.ShapePtRelation[n] = m;
							}
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCircleDeviationMeasure& x)
{
	try
	{
		int n, m, i, pts;		
		is >> x.DrawCount;
		if(x.DrawCount == 0)
		{
			is >> x.CentRadius[0];
			is >> x.CentRadius[1];
			is >> x.CentRadius[2];
			is >> n;
			x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		}
		else if(x.DrawCount == 1)
		{
			is >> n;
		    x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
			is >> n;
			x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		}
		else if(x.DrawCount == 2)
		{
		is >> n;
		x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		}
		is >> pts;
		for(i = 1; i <= pts; i++)
		{
			is >> n;
			is >> m;
			x.ShapePtRelation[n] = m;
		}
		x.UpdateMeasurement();		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("CDM0013", __FILE__, __FUNCSIG__); }
}