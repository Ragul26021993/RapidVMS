#include "StdAfx.h"
#include "DimRadius2D.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimRadius2D::DimRadius2D(TCHAR* myname):DimBase(genName(myname))
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0001", __FILE__, __FUNCSIG__); }
}

DimRadius2D::DimRadius2D(bool simply):DimBase(false)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0002", __FILE__, __FUNCSIG__); }
}

DimRadius2D::~DimRadius2D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimRadius2D::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		radiusno++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(radiusno, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimRadius2D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double endp[8];
		switch(ParentShape1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CIRCLE:
				GRAFIX->drawDiametere3D(&center[0], ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::SHAPETYPE::ARC:
				GRAFIX->drawArc_Radius(&center[0], ((Circle*)ParentShape1)->Radius(),  &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, ((Circle*)ParentShape1)->Startangle(), ((Circle*)ParentShape1)->Sweepangle(), &LastMPositionX, &LastMPositionY, getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0005", __FILE__, __FUNCSIG__); }
}

void DimRadius2D::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D;
		double cirParam[7] = {0}, radius = (((Circle*)s1)->Radius());
    	((Circle*)ParentShape1)->getCenter()->FillDoublePointer(center, 3);
		((Circle*)ParentShape1)->getParameters(cirParam);
		RMATH3DOBJECT->TransformationMatrix_Dia_Circle(cirParam, &TransformationMatrix[0], &MeasurementPlane[0]);
		setDimension(radius);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0006", __FILE__, __FUNCSIG__); }
}

void DimRadius2D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimRadius2D::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimRadius2D::Clone(int n)
{
	try
	{
		DimRadius2D* Cdim = new DimRadius2D();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimRadius2D::CreateDummyCopy()
{
	try
	{
		DimRadius2D* Cdim = new DimRadius2D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimRadius2D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimRadius2D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<< "DimRadius2D"<<endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os<< "EndDimRadius2D"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimRadius2D& x)
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
			if(Tagname==L"DimRadius2D")
			{
			while(Tagname!=L"EndDimRadius2D")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimRadius2D& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD0011", __FILE__, __FUNCSIG__); }
}