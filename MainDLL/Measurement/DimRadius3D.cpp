#include "StdAfx.h"
#include "DimRadius3D.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"

DimRadius3D::DimRadius3D(TCHAR* myname):DimBase(genName(myname))
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0001", __FILE__, __FUNCSIG__); }
}

DimRadius3D::DimRadius3D(bool simply):DimBase(false)
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0002", __FILE__, __FUNCSIG__); }
}

DimRadius3D::~DimRadius3D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimRadius3D::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimRadius3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(ParentShape1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CYLINDER:
				GRAFIX->drawDiametere3D(&CylinderParam[0], &CylinderEndPoints[0], getCDimension(), getModifiedName(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::SHAPETYPE::SPHERE:
				GRAFIX->drawDiametere3D(&CylinderParam[0], CylinderParam[3] * 2, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				GRAFIX->drawDiametere3D(&CylinderParam[0], CylinderParam[6] * 2, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0005", __FILE__, __FUNCSIG__); }
}

void DimRadius3D::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_RADIUS3D;
		double radius;
		switch(ParentShape1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::CYLINDER:
				radius = (((Cylinder*)ParentShape1)->Radius1());
				((Cylinder*)ParentShape1)->getParameters(&CylinderParam[0]);
				((Cylinder*)ParentShape1)->getEndPoints(&CylinderEndPoints[0]);
				break;
			case RapidEnums::SHAPETYPE::SPHERE:
				radius = (((Sphere*)ParentShape1)->Radius());
				((Sphere*)ParentShape1)->getParameters(&CylinderParam[0]);
				RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(CylinderParam, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				radius = (((Circle*)ParentShape1)->Radius());
				((Circle*)ParentShape1)->getParameters(&CylinderParam[0]);
				RMATH3DOBJECT->TransformationMatrix_Dia_Circle(CylinderParam, &TransformationMatrix[0], &MeasurementPlane[0]);
				break;
		}
		setDimension(radius);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0006", __FILE__, __FUNCSIG__); }
}

void DimRadius3D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimRadius3D::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimRadius3D::Clone(int n)
{
	try
	{
		DimRadius3D* Cdim = new DimRadius3D();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimRadius3D::CreateDummyCopy()
{
	try
	{
		DimRadius3D* Cdim = new DimRadius3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimRadius3D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimRadius3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<< "DimRadius3D"<<endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os<< "EndDimRadius3D"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimRadius3D& x)
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
			if(Tagname==L"DimRadius3D")
			{
				while(Tagname!=L"EndDimRadius3D")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimRadius3D& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMRAD3D0011", __FILE__, __FUNCSIG__); }
}