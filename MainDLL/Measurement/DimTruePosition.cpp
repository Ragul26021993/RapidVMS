#include "StdAfx.h"
#include "DimTruePosition.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimTruePosition::DimTruePosition(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	init();
}

DimTruePosition::DimTruePosition(bool simply):DimBase(false)
{
	init();
}

DimTruePosition::~DimTruePosition()
{
}

void DimTruePosition::init()
{
	try
	{
		IsValid(false);
		tpposition[0] = 0;
		tpposition[1] = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0001", __FILE__, __FUNCSIG__); }
}

TCHAR* DimTruePosition::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt = 0;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R || Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D)
			dimcnt = tpnoG++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX)
			dimcnt = tpnoX++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY)
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

void DimTruePosition::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double cen[3] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(), ((Circle*)ParentShape1)->getCenter()->getZ()};
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
				GRAFIX->drawTruePosition(mposition.getX(), mposition.getY(), cen[0], cen[1], ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), &ParentShape2->DatumName, &ParentShape3->DatumName, getModifiedName(), 0, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
				GRAFIX->drawTruePosition(mposition.getX(), mposition.getY(), cen[0], cen[1], ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), &ParentShape2->DatumName, &ParentShape3->DatumName, getModifiedName(), 1, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);
			
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
				GRAFIX->drawTruePosition(mposition.getX(), mposition.getY(), cen[0], cen[1], ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), &ParentShape2->DatumName, &ParentShape3->DatumName, getModifiedName(), 2, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);			
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0003", __FILE__, __FUNCSIG__); }
}

void DimTruePosition::CalculateMeasurement(Shape* s1, Shape* s2, Shape* s3)
{
	try
	{
		this->ParentShape1 = s1; this->ParentShape2 = s2; this->ParentShape3 = s3;
		double cen[2] = {0}, CShape1_intercept = 0, CShape2_intercept = 0, CShape1_intersectPt[2] = {0}, CShape2_intersectPt[2] = {0}, newCenter[2] = {0};
		((Circle*)ParentShape1)->getCenter()->FillDoublePointer(&cen[0]);
		Line* CShape1 = (Line*)ParentShape2; Line* CShape2 = (Line*)ParentShape3;
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
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
				setDimension(RMATH2DOBJECT->Pt2Pt_distance(newCenter, tpposition));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
				setDimension(2 * RMATH2DOBJECT->Pt2Pt_distance(newCenter, tpposition));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
				setDimension(abs(newCenter[0] - tpposition[0]));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
				setDimension(abs(newCenter[1] - tpposition[1]));
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0004", __FILE__, __FUNCSIG__); }
}

void DimTruePosition::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2, ParentShape3);
}

void DimTruePosition::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement((Shape*)sender, ParentShape2, ParentShape3);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentShape1, (Shape*)sender, ParentShape3);
		else if(sender->getId() == ParentShape3->getId())
			this->CalculateMeasurement(ParentShape1, ParentShape2, (Shape*)sender);
		CalculateMeasurement(ParentShape1, ParentShape2, ParentShape3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0004", __FILE__, __FUNCSIG__); }
}

void DimTruePosition::SetTPposition(double x, double y)
{
	tpposition[0] = x;
	tpposition[1] = y;
}

int DimTruePosition::tpnoG = 0;
int DimTruePosition::tpnoX = 0;
int DimTruePosition::tpnoY = 0;
void DimTruePosition::reset()
{
	tpnoG = 0;
	tpnoX = 0;
	tpnoY = 0;
}

DimBase* DimTruePosition::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R || this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D)
			myname = _T("TPG");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX)
			myname = _T("TPX");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY)
			myname = _T("TPY");
		DimTruePosition* Cdim = new DimTruePosition((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0005", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimTruePosition::CreateDummyCopy()
{
	try
	{
		DimTruePosition* Cdim = new DimTruePosition(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0006", __FILE__, __FUNCSIG__); return NULL; }
}

void DimTruePosition::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->tpposition[0] = ((DimTruePosition*)Cdim)->tpposition[0];
		this->tpposition[1] = ((DimTruePosition*)Cdim)->tpposition[1];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0007", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimTruePosition& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimTruePosition"<<endl;
		os <<"tpposition0:"<< x.tpposition[0] << endl;
		os <<"tpposition1:"<< x.tpposition[1] << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"ParentShape3:"<< x.ParentShape3->getId() << endl;
		os <<"EndDimTruePosition"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimTruePosition& x)
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
			if(Tagname==L"DimTruePosition")
			{
				while(Tagname!=L"EndDimTruePosition")
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
						else if(Tagname==L"ParentShape3")
						{
							x.ParentShape3 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
					HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
					HELPEROBJECT->AddGDnTReference(&x, x.ParentShape3);
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
					HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
					HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
					break;
				}
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}		
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0009", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimTruePosition& x)
{
	try
	{
		int n;
		is >> x.tpposition[0];
		is >> x.tpposition[1];
		is >> n; x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n; x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n; x.ParentShape3 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
		switch(x.MeasurementType)
		{
		case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
		case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape3);
			break;
		case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
			break;
		case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
			break;
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0009", __FILE__, __FUNCSIG__); }
}