#include "StdAfx.h"
#include "DimDiameter3D.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Sphere.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimDiameter3D::DimDiameter3D(TCHAR* myname):DimBase(genName(myname))
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0001", __FILE__, __FUNCSIG__); }
}

DimDiameter3D::DimDiameter3D(bool simply):DimBase(false)
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0002", __FILE__, __FUNCSIG__); }
}

DimDiameter3D::~DimDiameter3D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimDiameter3D::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA2D0004", __FILE__, __FUNCSIG__); }
} 

void DimDiameter3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0005", __FILE__, __FUNCSIG__); }
}

void DimDiameter3D::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D;
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
		setDimension(radius * 2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0006", __FILE__, __FUNCSIG__); }
}

void DimDiameter3D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimDiameter3D::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimDiameter3D::Clone(int n)
{
	try
	{
		DimDiameter3D* Cdim = new DimDiameter3D();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimDiameter3D::CreateDummyCopy()
{
	try
	{
		DimDiameter3D* Cdim = new DimDiameter3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimDiameter3D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimDiameter3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimDiameter3D"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;		
		os<<"EndDimDiameter3D"<<endl;	
		return os;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimDiameter3D& x)
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
			if(Tagname==L"DimDiameter3D")
			{
				while(Tagname!=L"EndDimDiameter3D")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimDiameter3D& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0011", __FILE__, __FUNCSIG__);}
}