#include "StdAfx.h"
#include "DimTruePosition3DPoint.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimTruePosition3DPoint::DimTruePosition3DPoint(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	init();
}

DimTruePosition3DPoint::DimTruePosition3DPoint(bool simply):DimBase(false)
{
	init();
}

DimTruePosition3DPoint::~DimTruePosition3DPoint()
{
}

void DimTruePosition3DPoint::init()
{
	try
	{
		IsValid(false);
		MeasureAs3DMeasurement(true);
		tpposition[0] = 0;
		tpposition[1] = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0001", __FILE__, __FUNCSIG__); }
}

TCHAR* DimTruePosition3DPoint::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt = 0;
		dimcnt = tpnoG++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0002", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimTruePosition3DPoint::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->draw3DTruePosition(mposition.getX(), mposition.getY(), ParentPoint1->getX(), ParentPoint1->getY(), 0, getCDimension(), &ParentShape1->DatumName, &ParentShape2->DatumName, &ParentShape3->DatumName, getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, ParentPoint1->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0003", __FILE__, __FUNCSIG__); }
}

void DimTruePosition3DPoint::CalculateMeasurement(Vector* v1, Shape* s1, Shape* s2, Shape* s3)
{
	try
	{
		list<list<double>> ShapesParam;
		double TransformMatrix[16];
		double cen[4] = {v1->getX(), v1->getY(), v1->getZ(), 1}, TransCen[4] = {0};
		int Order1[] = {4, 4}, Order2[] = {4, 1};
		this->ParentPoint1 = v1;
		this->ParentShape1 = s1; this->ParentShape2 = s2; this->ParentShape3 = s3;
		HELPEROBJECT->getShapeDimension(s1, &ShapesParam);
		HELPEROBJECT->getShapeDimension(s2, &ShapesParam);
		HELPEROBJECT->getShapeDimension(s3, &ShapesParam);
		HELPEROBJECT->UCSBasePlaneTransformM(&ShapesParam, TransformMatrix, false);
		RMATH2DOBJECT->MultiplyMatrix1(TransformMatrix, Order1, cen, Order2, TransCen);

		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R:
				setDimension(RMATH2DOBJECT->Pt2Pt_distance(TransCen, tpposition));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D:
				setDimension(2 * RMATH2DOBJECT->Pt2Pt_distance(TransCen, tpposition));
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0004", __FILE__, __FUNCSIG__); }
}

void DimTruePosition3DPoint::UpdateMeasurement()
{
	CalculateMeasurement(ParentPoint1, ParentShape1, ParentShape2, ParentShape3);
}

void DimTruePosition3DPoint::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement(ParentPoint1, (Shape*)sender, ParentShape2, ParentShape3);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentPoint1, ParentShape1, (Shape*)sender, ParentShape3);
		else if(sender->getId() == ParentShape3->getId())
			this->CalculateMeasurement(ParentPoint1, ParentShape1, ParentShape2, (Shape*)sender);
		else 
			this->CalculateMeasurement(ParentPoint1, ParentShape1, ParentShape2, ParentShape3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0005", __FILE__, __FUNCSIG__); }
}

void DimTruePosition3DPoint::SetTPposition(double x, double y)
{
	tpposition[0] = x;
	tpposition[1] = y;
}

int DimTruePosition3DPoint::tpnoG = 0;

void DimTruePosition3DPoint::reset()
{
	tpnoG = 0;
}

DimBase* DimTruePosition3DPoint::Clone(int n)
{
	try
	{
		std::wstring myname;
		myname = _T("TPG3D");
		DimTruePosition3DPoint* Cdim = new DimTruePosition3DPoint((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0006", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimTruePosition3DPoint::CreateDummyCopy()
{
	try
	{
		DimTruePosition3DPoint* Cdim = new DimTruePosition3DPoint(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0007", __FILE__, __FUNCSIG__); return NULL; }
}

void DimTruePosition3DPoint::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->tpposition[0] = ((DimTruePosition3DPoint*)Cdim)->tpposition[0];
		this->tpposition[1] = ((DimTruePosition3DPoint*)Cdim)->tpposition[1];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0008", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimTruePosition3DPoint& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimTruePosition3DPoint"<<endl;
		os <<"tpposition0:"<< x.tpposition[0] << endl;
		os <<"tpposition1:"<< x.tpposition[1] << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"ParentShape3:"<< x.ParentShape3->getId() << endl;
		os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os <<"EndDimTruePosition3DPoint"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0009", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimTruePosition3DPoint& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimTruePosition3DPoint")
		{
			while(Tagname!=L"EndDimTruePosition3DPoint")
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
					else if(Tagname==L"ParentPoint1")
					{
						x.ParentPoint1 = new Vector(0,0,0);
						is >> (*static_cast<Vector*>(x.ParentPoint1));
						UCS* ucs = MAINDllOBJECT->getUCS(x.ParentShape1->UcsId());
						x.ParentPoint1 = MAINDllOBJECT->getVectorPointer_UCS(x.ParentPoint1, ucs, true);
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
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape1);
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape3);
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP3DPT0010", __FILE__, __FUNCSIG__); return is; }
}