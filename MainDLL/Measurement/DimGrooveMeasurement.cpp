#include "StdAfx.h"
#include "DimGrooveMeasurement.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\SinglePoint.h"
#include "..\Engine\PointCollection.h"

DimGrooveMeasurement::DimGrooveMeasurement(TCHAR* myname, RapidEnums::MEASUREMENTTYPE ctype):DimBase(genName(myname, ctype))
{
	try
	{
		init(ctype);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0001", __FILE__, __FUNCSIG__); }
}

DimGrooveMeasurement::DimGrooveMeasurement(bool simply, RapidEnums::MEASUREMENTTYPE ctype):DimBase(false)
{
	try
	{
		init(ctype);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0002", __FILE__, __FUNCSIG__); }
}

void DimGrooveMeasurement::init(RapidEnums::MEASUREMENTTYPE ctype)
{
 try
	{
		this->MeasurementType = ctype;
	    if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH)
		{
			Vector1 = new Vector();
			TempLine1 = new Line(false);
			TempLine1->IsValid(false);
			TempLine2 = NULL;
			Vector2 = NULL;
		}
		else if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH)
		{
			Vector1 = new Vector();
			Vector2 = new Vector();
			TempLine1 = NULL;
			TempLine2 = NULL;
		}
		else if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE)
		{
			TempLine1 = new Line(false);
			TempLine1->IsValid(false);
			TempLine2 = new Line(false);
			TempLine2->IsValid(false);
			Vector1 = NULL; Vector2 = NULL;
		}
		WidthType = false; IsValid(false);
	   }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0001", __FILE__, __FUNCSIG__); }
}

DimGrooveMeasurement::~DimGrooveMeasurement()
{
	try
	{
	   	if(TempLine1 != NULL)
			delete(TempLine1);
		if(TempLine2 != NULL)
			delete(TempLine2);
		if(Vector1 != NULL)
			delete(Vector1);
		if(Vector2 != NULL)
			delete(Vector2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimGrooveMeasurement::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE ctype)
{
	try
	{
		this->MeasurementType = ctype;
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH)
			dimcnt = grovdpthcnt++;
		else if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH)
			dimcnt = grovwidthcnt++;
		else if(ctype == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE)
			dimcnt = grovangcnt++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimGrooveMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{    
		if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() < 1)
		{
			IsValid(false);
			return;
		}
		if(MeasurementType  == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH)
		{  
			GRAFIX->drawLine(CornerPnts[2],CornerPnts[3], CornerPnts[4], CornerPnts[5]);
			double mousePnt[2] = {(CornerPnts[0] + CornerPnts[2])/2, (CornerPnts[1] + CornerPnts[3])/2};
			GRAFIX->drawPoint_FinitelineDistance(Vector1->getX(), Vector1->getY(), ((Line*)TempLine1)->Angle(), ((Line*)TempLine1)->Intercept(), &CornerPnts[2], &CornerPnts[4], mousePnt[0] + 0.2, mousePnt[1] + 0.2, doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Vector1->getZ());
		}
		else if(MeasurementType  == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH)
		{
			double mousePnt[2] = {(Vector1->getX() + Vector2->getX())/2, (Vector1->getY() + Vector2->getY())/2};
			GRAFIX->drawPoint_PointDistance(Vector1->getX(), Vector1->getY(), Vector2->getX(), Vector2->getY(),  mousePnt[0] + 0.2, mousePnt[1] + 0.2, doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Vector1->getZ());
		}
		else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE)
		{
			GRAFIX->drawLine(CornerPnts[0],CornerPnts[1], CornerPnts[2], CornerPnts[3]);
			GRAFIX->drawLine(CornerPnts[4],CornerPnts[5], CornerPnts[6], CornerPnts[7]);
			double angle1 = RMATH2DOBJECT->ray_angle(CornerPnts[4], CornerPnts[5], CornerPnts[6], CornerPnts[7]);
			double angle2 = RMATH2DOBJECT->ray_angle(CornerPnts[2], CornerPnts[3], CornerPnts[0], CornerPnts[1]);
			double Midangle1 = (angle1 + angle2)/2;
			double Angle1Pos[2] = {CornerPnts[6] + 0.5 * cos(Midangle1), CornerPnts[7] + 0.5 * sin(Midangle1)};
		   	GRAFIX->drawFiniteline_FinitelineAngle(((Line*)TempLine1)->Angle(), ((Line*)TempLine1)->Intercept(), &CornerPnts[0], &CornerPnts[2], ((Line*)TempLine2)->Angle(), ((Line*)TempLine2)->Intercept(), &CornerPnts[4], &CornerPnts[6],  Angle1Pos[0], Angle1Pos[1], getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, ((Line*)TempLine1)->getPoint1()->getZ());
     	}
	  
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0005", __FILE__, __FUNCSIG__); }
}

void DimGrooveMeasurement::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimGrooveMeasurement::CalculateDiffGrooveMeasurements(bool maxwidth)
{
	try
	{
		if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() < 1)
		{
			IsValid(false);
			return;
		}
		CalculateGrooveCornerPnts();
		if(MeasurementType  == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH)
			 CalculateGrooveDepth();
		else if(MeasurementType  == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH)
			 CalculateGrooveWidths(maxwidth);
		else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE)
	     	 CalculateGrooveAngle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}


void DimGrooveMeasurement::CalculateGrooveCornerPnts()
{
	   	Line* CShape = (Line*)ParentShape1;
		double slope = CShape->Angle();
		double intercept = CShape->Intercept(), intercept1, intercept2;
		if(CShape->PointsList->Pointscount() < 1)
		{
			setDimension(0.0);
			return;
		}
		double dist = 0, dist1 = 0, dist2 = 0, Upperpnt[2], Lowerpnt[2];
		for(int i = 0; i < CShape->PointsList->Pointscount(); i ++)
		{
			dist = RMATH2DOBJECT->Pt2Line_DistSigned(CShape->pts[2 * i], CShape->pts[2 * i + 1], slope, intercept);
			if(i == 0)
			{
				if(dist > 0)
					dist1 = dist;
				else
					dist2 = dist;
			}
			else
			{
				if(dist > dist1)
				{
					dist1 = dist;
					Upperpnt[0] = CShape->pts[2 * i]; Upperpnt[1] =  CShape->pts[2 * i + 1];
				}
				if(dist < dist2)
				{
					dist2 = dist;
					Lowerpnt[0] = CShape->pts[2 * i]; Lowerpnt[1] =  CShape->pts[2 * i + 1];
				}
			}
		}
		PointCollection* CPointCol = ((Line*)ParentShape1)->PointsList;
		RMATH2DOBJECT->Intercept_LinePassing_Point(Upperpnt, slope, &intercept1);
		RMATH2DOBJECT->Intercept_LinePassing_Point(Lowerpnt, slope, &intercept2);
		SinglePoint* CurrentPoint =  CPointCol->getList().begin()->second;
	    double dis1 = RMATH2DOBJECT->Pt2Line_Dist(CurrentPoint->X, CurrentPoint->Y, slope, intercept1);
		double dis2 = RMATH2DOBJECT->Pt2Line_Dist(CurrentPoint->X, CurrentPoint->Y, slope, intercept2);
		if(dis1 <= dis2)
		   GrovCornrPnts_TwoLines(slope, intercept1, intercept2, dis1);
		else
		   GrovCornrPnts_TwoLines(slope, intercept2, intercept1, dis2);
	}

bool DimGrooveMeasurement::GrovCornrPnts_TwoLines(double slope, double intercept1, double intercept2,  double dis)
{
		firstset = secondset = thirdset = forthset = false;
	    PointCollection* CPointCol = ((Line*)ParentShape1)->PointsList;
		double Vwupp = MAINDllOBJECT->getWindow(0).getUppX(), tempPoints[8] = {0};
	/*	if (dis > Vwupp) Vwupp = dis;*/
	 	for(PC_ITER PtItem = CPointCol->getList().begin(); PtItem != CPointCol->getList().end(); PtItem++)
		{
			SinglePoint* CurrentPoint = PtItem->second;
			if(!firstset)
			{   double dist = RMATH2DOBJECT->Pt2Line_Dist(CurrentPoint->X, CurrentPoint->Y, slope, intercept1);
				double DistDiffInPixel = (dist) / Vwupp;
				if (DistDiffInPixel > 8)
				{
					 tempPoints[0] = CurrentPoint->X;  tempPoints[1] = CurrentPoint->Y;
	     			firstset = true;
				}
			}
			else if(!secondset)
			{   double dist = RMATH2DOBJECT->Pt2Line_Dist(CurrentPoint->X, CurrentPoint->Y, slope, intercept2);
				double DistDiffInPixel = (dist) / Vwupp;
				if((DistDiffInPixel) < 3)
				{
					tempPoints[2] = CurrentPoint->X;  tempPoints[3] = CurrentPoint->Y;
	     			secondset = true;
				}
			}
			else if(!thirdset)
			{   double dist = RMATH2DOBJECT->Pt2Line_Dist(CurrentPoint->X, CurrentPoint->Y, slope, intercept2);
				double DistDiffInPixel = (dist) / Vwupp;
				if((DistDiffInPixel) > 4)
				{
					tempPoints[4] = CurrentPoint->X; tempPoints[5] = CurrentPoint->Y;
	     			thirdset = true;
				}
			}
			else if(!forthset)
			{   double dist = RMATH2DOBJECT->Pt2Line_Dist(CurrentPoint->X, CurrentPoint->Y,  slope, intercept1);
				double DistDiffInPixel = (dist) / Vwupp;
				if((DistDiffInPixel) < 3)
				{
					tempPoints[6] = CurrentPoint->X; tempPoints[7] = CurrentPoint->Y;
	     			forthset = true;
				}
			}
        }
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(slope, intercept1, &tempPoints[0], &CornerPnts[0]);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(slope, intercept1, &tempPoints[6], &CornerPnts[6]);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(slope, intercept2, &tempPoints[2], &CornerPnts[2]);
		RMATH2DOBJECT->Point_PerpenIntrsctn_Line(slope, intercept2, &tempPoints[4], &CornerPnts[4]);
		return forthset;
}

void DimGrooveMeasurement::CalculateGrooveWidths(bool maxWidth)
{
	try
	{
		WidthType = maxWidth;
		if(maxWidth)
		{
			if (forthset)
			{
				Vector1->set(CornerPnts[0], CornerPnts[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
				Vector2->set(CornerPnts[6], CornerPnts[7], MAINDllOBJECT->getCurrentDRO().getZ());
			}
			else 
			 return;
		}
		else
		{
			if (thirdset)
			{
				 Vector1->set(CornerPnts[2], CornerPnts[3], MAINDllOBJECT->getCurrentDRO().getZ()); 
			     Vector2->set(CornerPnts[4], CornerPnts[5], MAINDllOBJECT->getCurrentDRO().getZ());
			}
			else 
			 return;
		}
		double dist = RMATH2DOBJECT->Pt2Pt_distance(Vector1->getX(), Vector1->getY(), Vector2->getX(), Vector2->getY());
		setDimension(dist); IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimGrooveMeasurement::CalculateGrooveAngle()
{
	try
	{
		double slope, intercept, cp[2];
		if(forthset)
		{
			RMATH2DOBJECT->TwoPointLine(&CornerPnts[0], &CornerPnts[2], &slope, &intercept);
			TempLine1->Angle(slope); TempLine1->Intercept(intercept);
			RMATH2DOBJECT->TwoPointLine(&CornerPnts[4], &CornerPnts[6], &slope, &intercept);
			TempLine2->Angle(slope); TempLine2->Intercept(intercept);
			double Tempangle = RMATH2DOBJECT->Line2Line_angle(TempLine1->Angle(), TempLine1->Intercept(), TempLine2->Angle(), TempLine2->Intercept(), &cp[0]);
			setDimension(Tempangle);  IsValid(true);	
		}
		else 
			return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimGrooveMeasurement::CalculateGrooveDepth()
{
	try
	{
	    if(thirdset)
		{
			Vector1->set(CornerPnts[0], CornerPnts[1],  MAINDllOBJECT->getCurrentDRO().getZ());
			double slope, intercept, dis;
			RMATH2DOBJECT->TwoPointLine(&CornerPnts[2], &CornerPnts[4], &slope, &intercept);
			TempLine1->Angle(slope); TempLine1->Intercept(intercept);
			dis = RMATH2DOBJECT->Pt2Line_Dist(CornerPnts[0], CornerPnts[1], slope, intercept);
			setDimension(dis); IsValid(true);
     	}
	    else
		  return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimGrooveMeasurement::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimGrooveMeasurement::UpdateForParentChange(BaseItem* sender)
{
	CalculateMeasurement((Shape*)sender);
	CalculateDiffGrooveMeasurements(WidthType);
}

DimBase* DimGrooveMeasurement::Clone(int n)
{
	try
	{
		DimGrooveMeasurement* Cdim = new DimGrooveMeasurement(false, this->MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimGrooveMeasurement::CreateDummyCopy()
{
	try
	{
		DimGrooveMeasurement* Cdim = new DimGrooveMeasurement(false, this->MeasurementType);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimGrooveMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0009", __FILE__, __FUNCSIG__); }
}

int DimGrooveMeasurement::grovdpthcnt = 0;
int DimGrooveMeasurement::grovwidthcnt = 0;
int DimGrooveMeasurement::grovangcnt = 0;

void DimGrooveMeasurement::reset()
{
     grovdpthcnt = 0;
     grovwidthcnt = 0;
     grovangcnt= 0;
}

wostream& operator<<( wostream& os, DimGrooveMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimGrooveMeasurement"<<endl;
		os <<"WidthType:"<< x.WidthType << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os<<"EndDimGrooveMeasurement"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0010", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimGrooveMeasurement& x)
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
			if(Tagname==L"DimGrooveMeasurement")
			{
				while(Tagname!=L"EndDimGrooveMeasurement")
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
						if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}		
						else if(Tagname==L"WidthType")
						{
							if(Val=="0")
							{
								x.WidthType=false;							
							}
							else						
							x.WidthType=true;
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
void ReadOldPP(wistream& is, DimGrooveMeasurement& x)
{
	try
	{
		int n;
		is >> x.WidthType;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0011", __FILE__, __FUNCSIG__);}
}