#include "StdAfx.h"
#include "DimDiameter2D.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimDiameter2D::DimDiameter2D(TCHAR* myname):DimBase(genName(myname))
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0001", __FILE__, __FUNCSIG__); }
}

DimDiameter2D::DimDiameter2D(bool simply):DimBase(false)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0002", __FILE__, __FUNCSIG__); }
}

DimDiameter2D::~DimDiameter2D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimDiameter2D::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		diano++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(diano, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimDiameter2D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(&center[0], ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0005", __FILE__, __FUNCSIG__); }
}

void DimDiameter2D::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D;
		double radius = (((Circle*)s1)->Radius()), cirParam[7] = {0};
		((Circle*)ParentShape1)->getCenter()->FillDoublePointer(center, 3);
		((Circle*)ParentShape1)->getParameters(cirParam);
		RMATH3DOBJECT->TransformationMatrix_Dia_Circle(cirParam, &TransformationMatrix[0], &MeasurementPlane[0]);
		setDimension(radius * 2);
	}
	catch(...)
	{ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0006", __FILE__, __FUNCSIG__); }
}

void DimDiameter2D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimDiameter2D::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimDiameter2D::Clone(int n)
{
	try
	{
		DimDiameter2D* Cdim = new DimDiameter2D();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimDiameter2D::CreateDummyCopy()
{
	try
	{
		DimDiameter2D* Cdim = new DimDiameter2D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimDiameter2D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimDiameter2D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimDiameter2D"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;		
		os<<"EndDimDiameter2D"<<endl;	
		return os;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0010", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimDiameter2D& x)
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
			if(Tagname==L"DimDiameter2D")
			{
				while(Tagname!=L"EndDimDiameter2D")
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimDiameter2D& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0011", __FILE__, __FUNCSIG__); }
}