#include "StdAfx.h"
#include "DimTruePosition3D.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimTruePosition3D::DimTruePosition3D(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	init();
}

DimTruePosition3D::DimTruePosition3D(bool simply):DimBase(false)
{
	init();
}

DimTruePosition3D::~DimTruePosition3D()
{
}

void DimTruePosition3D::init()
{
	try
	{
		IsValid(false);
		MeasureAs3DMeasurement(true);
		tpposition[0] = 0;
		tpposition[1] = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0001", __FILE__, __FUNCSIG__); }
}

TCHAR* DimTruePosition3D::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0002", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimTruePosition3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double cen[3] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(), ((Circle*)ParentShape1)->getCenter()->getZ()};
		GRAFIX->draw3DTruePosition(mposition.getX(), mposition.getY(), cen[0], cen[1], ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), &ParentShape2->DatumName, &ParentShape3->DatumName, &ParentShape4->DatumName, getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0003", __FILE__, __FUNCSIG__); }
}

void DimTruePosition3D::CalculateMeasurement(Shape* s1, Shape* s2, Shape* s3, Shape* s4)
{
	try
	{
		list<list<double>> ShapesParam;
		double TransformMatrix[16];
		double cen[4] = {0, 0, 0, 1}, TransCen[4] = {0};
		int Order1[] = {4, 4}, Order2[] = {4, 1};
		this->ParentShape1 = s1; this->ParentShape2 = s2; this->ParentShape3 = s3; this->ParentShape4 = s4;
		HELPEROBJECT->getShapeDimension(s2, &ShapesParam);
		HELPEROBJECT->getShapeDimension(s3, &ShapesParam);
		HELPEROBJECT->getShapeDimension(s4, &ShapesParam);
		HELPEROBJECT->UCSBasePlaneTransformM(&ShapesParam, TransformMatrix, false);
		((Circle*)s1)->getCenter()->FillDoublePointer(&cen[0], 3);
		RMATH2DOBJECT->MultiplyMatrix1(TransformMatrix, Order1, cen, Order2, TransCen);

		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R:
				setDimension(RMATH2DOBJECT->Pt2Pt_distance(TransCen, tpposition));
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D:
				setDimension(2 * RMATH2DOBJECT->Pt2Pt_distance(TransCen, tpposition));
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0004", __FILE__, __FUNCSIG__); }
}

void DimTruePosition3D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2, ParentShape3, ParentShape4);
}

void DimTruePosition3D::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement((Shape*)sender, ParentShape2, ParentShape3, ParentShape4);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentShape1, (Shape*)sender, ParentShape3, ParentShape4);
		else if(sender->getId() == ParentShape3->getId())
			this->CalculateMeasurement(ParentShape1, ParentShape2, (Shape*)sender, ParentShape4);
		else if(sender->getId() == ParentShape4->getId())
			this->CalculateMeasurement(ParentShape1, ParentShape2, ParentShape3, (Shape*)sender);
		CalculateMeasurement(ParentShape1, ParentShape2, ParentShape3, ParentShape4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0004", __FILE__, __FUNCSIG__); }
}

void DimTruePosition3D::SetTPposition(double x, double y)
{
	tpposition[0] = x;
	tpposition[1] = y;
}

int DimTruePosition3D::tpnoG = 0;

void DimTruePosition3D::reset()
{
	tpnoG = 0;
}

DimBase* DimTruePosition3D::Clone(int n)
{
	try
	{
		std::wstring myname;
		myname = _T("TPG3D");
		DimTruePosition3D* Cdim = new DimTruePosition3D((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0005", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimTruePosition3D::CreateDummyCopy()
{
	try
	{
		DimTruePosition3D* Cdim = new DimTruePosition3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0006", __FILE__, __FUNCSIG__); return NULL; }
}

void DimTruePosition3D::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->tpposition[0] = ((DimTruePosition3D*)Cdim)->tpposition[0];
		this->tpposition[1] = ((DimTruePosition3D*)Cdim)->tpposition[1];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0007", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimTruePosition3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimTruePosition3D"<<endl;
		os <<"tpposition0:"<< x.tpposition[0] << endl;
		os <<"tpposition1:"<< x.tpposition[1] << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"ParentShape3:"<< x.ParentShape3->getId() << endl;
		os <<"ParentShape4:"<< x.ParentShape4->getId() << endl;
		os <<"EndDimTruePosition3D"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimTruePosition3D& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimTruePosition3D")
		{
			while(Tagname!=L"EndDimTruePosition3D")
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
					else if(Tagname==L"ParentShape4")
					{
						x.ParentShape4 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape3);
			HELPEROBJECT->AddGDnTReference(&x, x.ParentShape4);
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTP0009", __FILE__, __FUNCSIG__); return is; }
}