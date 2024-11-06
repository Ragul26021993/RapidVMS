#include "StdAfx.h"
#include "DimTruePositionPoint.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimTruePositionPoint::DimTruePositionPoint(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	init();
}

DimTruePositionPoint::DimTruePositionPoint(bool simply):DimBase(false)
{
	init();
}

DimTruePositionPoint::~DimTruePositionPoint()
{
}

void DimTruePositionPoint::init()
{
	try
	{
		IsValid(false);
		tpposition[0] = 0;
		tpposition[1] = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0001", __FILE__, __FUNCSIG__); }
}

TCHAR* DimTruePositionPoint::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt = 0;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R || Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D)
			dimcnt = tpnoG++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX)
			dimcnt = tpnoX++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY)
			dimcnt = tpnoY++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0002", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimTruePositionPoint::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double cen[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
				GRAFIX->drawTruePosition(mposition.getX(), mposition.getY(), cen[0], cen[1], 0, getCDimension(), &ParentShape1->DatumName, &ParentShape2->DatumName, getModifiedName(), 0, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
				GRAFIX->drawTruePosition(mposition.getX(), mposition.getY(), cen[0], cen[1], 0, getCDimension(), &ParentShape1->DatumName, &ParentShape2->DatumName, getModifiedName(), 1, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);
			
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
				GRAFIX->drawTruePosition(mposition.getX(), mposition.getY(), cen[0], cen[1], 0, getCDimension(), &ParentShape1->DatumName, &ParentShape2->DatumName, getModifiedName(), 2, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);			
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0003", __FILE__, __FUNCSIG__); }
}

void DimTruePositionPoint::CalculateMeasurement(Vector* v1, Shape* s1, Shape* s2)
{
	try
	{
		this->ParentPoint1 = v1;
		this->ParentShape1 = s1; this->ParentShape2 = s2; 
		double cen[2] = {v1->getX(), v1->getY()}, CShape1_intercept = 0, CShape2_intercept = 0, CShape1_intersectPt[2] = {0}, CShape2_intersectPt[2] = {0}, newCenter[2] = {0};
		Line* CShape1 = (Line*)ParentShape1; Line* CShape2 = (Line*)ParentShape2;
		if(CShape1->Angle() > M_PI_2 - 0.0001 && CShape1->Angle() < M_PI_2 + 0.0001)
			CShape1_intercept = cen[0];
		else
			CShape1_intercept = cen[1] - tan(CShape1->Angle()) * cen[0];
		if(CShape2->Angle() > M_PI_2 - 0.0001 && CShape2->Angle() < M_PI_2 + 0.0001)
			CShape2_intercept = cen[0];
		else
			CShape2_intercept = cen[1] - tan(CShape2->Angle()) * cen[0];
		RMATH2DOBJECT->Line_lineintersection(CShape1->Angle(), CShape1_intercept, CShape2->Angle(), CShape2->Intercept(), CShape2_intersectPt);
		RMATH2DOBJECT->Line_lineintersection(CShape2->Angle(), CShape2_intercept, CShape1->Angle(), CShape1->Intercept(), CShape1_intersectPt);
		newCenter[0] = RMATH2DOBJECT->Pt2Pt_distance(cen, CShape2_intersectPt);
		newCenter[1] = RMATH2DOBJECT->Pt2Pt_distance(cen, CShape1_intersectPt);
		if(CShape2_intersectPt[0] - cen[0] > 0)
			newCenter[0] = -newCenter[0];
		if(CShape1_intersectPt[1] - cen[1] > 0)
			newCenter[1] = -newCenter[1];
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
				setDimension(RMATH2DOBJECT->Pt2Pt_distance(newCenter, tpposition));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
				setDimension(2 * RMATH2DOBJECT->Pt2Pt_distance(newCenter, tpposition));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
				setDimension(abs(newCenter[0] - tpposition[0]));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
				setDimension(abs(newCenter[1] - tpposition[1]));
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0004", __FILE__, __FUNCSIG__); }
}

void DimTruePositionPoint::UpdateMeasurement()
{
	CalculateMeasurement(ParentPoint1, ParentShape1, ParentShape2);
}

void DimTruePositionPoint::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement(ParentPoint1, (Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentPoint1, ParentShape1, (Shape*)sender);
		else 
			this->CalculateMeasurement(ParentPoint1, ParentShape1, ParentShape2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0004", __FILE__, __FUNCSIG__); }
}

void DimTruePositionPoint::SetTPposition(double x, double y)
{
	tpposition[0] = x;
	tpposition[1] = y;
}

int DimTruePositionPoint::tpnoG = 0;
int DimTruePositionPoint::tpnoX = 0;
int DimTruePositionPoint::tpnoY = 0;
void DimTruePositionPoint::reset()
{
	tpnoG = 0;
	tpnoX = 0;
	tpnoY = 0;
}

DimBase* DimTruePositionPoint::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R || this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D)
			myname = _T("TPG");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX)
			myname = _T("TPX");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY)
			myname = _T("TPY");
		DimTruePositionPoint* Cdim = new DimTruePositionPoint((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0005", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimTruePositionPoint::CreateDummyCopy()
{
	try
	{
		DimTruePositionPoint* Cdim = new DimTruePositionPoint(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0006", __FILE__, __FUNCSIG__); return NULL; }
}

void DimTruePositionPoint::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->tpposition[0] = ((DimTruePositionPoint*)Cdim)->tpposition[0];
		this->tpposition[1] = ((DimTruePositionPoint*)Cdim)->tpposition[1];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0007", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimTruePositionPoint& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimTruePositionPoint"<<endl;
		os <<"tpposition0:"<< x.tpposition[0] << endl;
		os <<"tpposition1:"<< x.tpposition[1] << endl;
		os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimTruePositionPoint"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimTruePositionPoint& x)
{
	try
	{
		is >> (*(DimBase*)&x);	
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimTruePositionPoint")
		{
			while(Tagname!=L"EndDimTruePositionPoint")
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
					else if(Tagname==L"ParentShape2")
					{
						x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
					}	
					else if(Tagname==L"ParentPoint1")
					{
						x.ParentPoint1 = new Vector(0,0,0);
						is >> (*static_cast<Vector*>(x.ParentPoint1));
					}	
					else if(Tagname==L"tpposition0")
					{
						x.tpposition[0] = atof((const char*)(Val).c_str());
					}	
					else if(Tagname==L"tpposition1")
					{
						x.tpposition[1] = atof((const char*)(Val).c_str());
					}	
					}
			}
			x.UpdateMeasurement();
			switch(x.MeasurementType)
			{
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
				HELPEROBJECT->AddGDnTReference(&x, x.ParentShape1);
				HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
				HELPEROBJECT->AddGDnTReference(&x, x.ParentShape1);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
				HELPEROBJECT->AddGDnTReference(&x, x.ParentShape1);
				break;
			}
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);			
		}	
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0009", __FILE__, __FUNCSIG__); return is; }
}
