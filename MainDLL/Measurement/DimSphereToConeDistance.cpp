#include "StdAfx.h"
#include "DimSphereToConeDistance.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"

DimSphereToConeDistance::DimSphereToConeDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2SPH0001", __FILE__, __FUNCSIG__); }
}

DimSphereToConeDistance::DimSphereToConeDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0002", __FILE__, __FUNCSIG__); }
}

DimSphereToConeDistance::~DimSphereToConeDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0003", __FILE__, __FUNCSIG__); }
}

void DimSphereToConeDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimSphereToConeDistance::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		distno++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(distno, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimSphereToConeDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawFiniteline_SphereDistance3D(&ConeParam[0], &ConeEndPts[0], &SphereParam[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0006", __FILE__, __FUNCSIG__); }
}

void DimSphereToConeDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCONEDISTANCE;
		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CONE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		((Cone*)ParentShape1)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape1)->getEndPoints(&ConeEndPts[0]);
		((Sphere*)ParentShape2)->getParameters(&SphereParam[0]);
		double Dist = RMATH3DOBJECT->Distance_Line_Sphere(&ConeParam[0], &SphereParam[0], NormalmeasureType());
		setDimension(Dist);
		RMATH3DOBJECT->TransformationMatrix_Dist_Spr2Cylndr(&SphereParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0007", __FILE__, __FUNCSIG__); }
}

void DimSphereToConeDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			CalculateMeasurement((Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			CalculateMeasurement(ParentShape1, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0008", __FILE__, __FUNCSIG__); }
}

void DimSphereToConeDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimSphereToConeDistance::Clone(int n)
{
	try
	{
		DimSphereToConeDistance* Cdim = new DimSphereToConeDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimSphereToConeDistance::CreateDummyCopy()
{
	try
	{
		DimSphereToConeDistance* Cdim = new DimSphereToConeDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimSphereToConeDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimSphereToConeDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os << "DimSphereToConeDistance"<<endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os << "ParentShape2:"<< x.ParentShape2->getId() << endl;
		os << "EndDimSphereToConeDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimSphereToConeDistance& x)
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
			if(Tagname==L"DimSphereToConeDistance")
			{
			while(Tagname!=L"EndDimSphereToConeDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimSphereToConeDistance& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0013", __FILE__, __FUNCSIG__);  }
}