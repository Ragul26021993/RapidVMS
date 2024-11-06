#include "StdAfx.h"
#include "DimPerimeter_Diameter.h"
#include "..\Shapes\Perimeter.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimPerimeter_Diameter::DimPerimeter_Diameter(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0001", __FILE__, __FUNCSIG__); }
}

DimPerimeter_Diameter::DimPerimeter_Diameter(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0002", __FILE__, __FUNCSIG__); }
}

DimPerimeter_Diameter::~DimPerimeter_Diameter()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0003", __FILE__, __FUNCSIG__); }
}

void DimPerimeter_Diameter::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER;
		IsValid(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPerimeter_Diameter::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER)
			dimcnt = pdiacnt++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_PERIMETER)
			dimcnt = ppericnt++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_AREA)
			dimcnt = pareacnt++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPerimeter_Diameter::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		Perimeter* Pshape = (Perimeter*)ParentShape1;
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER:
				this->setDimension(Pshape->Diameter());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_PERIMETER:
				this->setDimension(Pshape->TotalPerimeter());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_AREA:
				this->setDimension(Pshape->TotalArea());
				break;
		}
		double point1[2] = {Pshape->getCenter()->getX(), Pshape->getCenter()->getY()};
		double point2[2] = {mposition.getX(), mposition.getY()};
		double slope = 0, intercept = 0, intrsctpt1[4] = {0}, intrsctpt2[4] = {0};
		int Count[2] = {0};
		RMATH2DOBJECT->TwoPointLine(&point1[0], &point2[0], &slope, &intercept);
		int pointsCnt = (int)Pshape->PerimeterShapecollection.size();
		double* tempPoints = (double*)malloc(sizeof(double) *  pointsCnt * 2);
		int Cnt = 0;
		for(PSC_ITER Citem = Pshape->PerimeterShapecollection.begin(); Citem != Pshape->PerimeterShapecollection.end(); Citem++)
		{
			Shape* Cpshape = (*Citem).second;
			if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double pt1[2] = {((Line*)Cpshape)->getPoint1()->getX(),((Line*)Cpshape)->getPoint1()->getY()};
				double pt2[2] = {((Line*)Cpshape)->getPoint2()->getX(), ((Line*)Cpshape)->getPoint2()->getY()};
				double Langle = ((Line*)Cpshape)->Angle(), Lintercept = ((Line*)Cpshape)->Intercept();
				RMATH2DOBJECT->Line_finiteline(slope, intercept, &pt1[0], &pt2[0], Langle, Lintercept, &intrsctpt1[0], &intrsctpt2[0], &Count[0]);
				if(Count[0] > 0)
				{
					tempPoints[Cnt++] = intrsctpt1[0];
					tempPoints[Cnt++] = intrsctpt1[1];
				}
			}
			else
			{
				double cntr[2] = {((Circle*)Cpshape)->getCenter()->getX(),((Circle*)Cpshape)->getCenter()->getY()};
				double Aparam[3] = {((Circle*)Cpshape)->Radius(), ((Circle*)Cpshape)->Startangle(), ((Circle*)Cpshape)->Sweepangle()};
				RMATH2DOBJECT->Line_arcinter(slope, intercept, &cntr[0], Aparam[0], Aparam[1], Aparam[2], &intrsctpt1[0], &intrsctpt2[0], &Count[0]);
				if(Count[0] > 0)
				{
					tempPoints[Cnt++] = intrsctpt1[0];
					tempPoints[Cnt++] = intrsctpt1[1];
				}
			}
		}
		RMATH2DOBJECT->Nearestpoint_point(Cnt/2, tempPoints, &point2[0], &center[0]);
		RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(center, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
		center[2] = Pshape->getCenter()->getZ();
		free(tempPoints);
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0005", __FILE__, __FUNCSIG__); }
}

void DimPerimeter_Diameter::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(center, 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0006", __FILE__, __FUNCSIG__); }
}

void DimPerimeter_Diameter::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement(ParentShape1);
}

void DimPerimeter_Diameter::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

DimBase* DimPerimeter_Diameter::Clone(int n)
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPerimeter_Diameter::CreateDummyCopy()
{
	try
	{
		DimPerimeter_Diameter* Cdim = new DimPerimeter_Diameter(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPerimeter_Diameter::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0009", __FILE__, __FUNCSIG__); }
}

void DimPerimeter_Diameter::reset()
{
	pdiacnt = 0;
	ppericnt = 0;
	pareacnt = 0;
}
int DimPerimeter_Diameter::pdiacnt = 0;
int DimPerimeter_Diameter::ppericnt = 0;
int DimPerimeter_Diameter::pareacnt = 0;

wostream& operator<<( wostream& os, DimPerimeter_Diameter& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPerimeter_Diameter"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"EndDimPerimeter_Diameter"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPerimeter_Diameter& x)
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
			if(Tagname==L"DimPerimeter_Diameter")
			{
			while(Tagname!=L"EndDimPerimeter_Diameter")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0011", __FILE__, __FUNCSIG__); return is; }
}

void ReadOldPP(wistream& is, DimPerimeter_Diameter& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPERIDIA0011", __FILE__, __FUNCSIG__);}
}