#include "StdAfx.h"
#include "DimSphereToSphereDistance.h"
#include "..\Shapes\Sphere.h"

#include "..\Engine\RCadApp.h"

DimSphereToSphereDistance::DimSphereToSphereDistance(TCHAR* myname):DimBase(genName(myname))
{
	init();
}

DimSphereToSphereDistance::DimSphereToSphereDistance(bool simply):DimBase(false)
{
	init();
}

DimSphereToSphereDistance::~DimSphereToSphereDistance()
{
}

void DimSphereToSphereDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0001", __FILE__, __FUNCSIG__); }
}

TCHAR* DimSphereToSphereDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0002", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimSphereToSphereDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double Dist;
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOSPHEREDISTANCE;

		((Sphere*)ParentShape1)->getParameters(&Sphere1Param[0]);
		((Sphere*)ParentShape2)->getParameters(&Sphere2Param[0]);
		Dist = RMATH3DOBJECT->Distance_Sphere_Sphere(&Sphere1Param[0], &Sphere2Param[0], NormalmeasureType());
		this->setDimension(Dist);
		RMATH3DOBJECT->TransformationMatrix_Dist_Spr2Spr(&Sphere1Param[0], &Sphere2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0003", __FILE__, __FUNCSIG__); }
}

void DimSphereToSphereDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawSphere_SphereDistance3D(&Sphere1Param[0], &Sphere2Param[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0004", __FILE__, __FUNCSIG__); }
}

void DimSphereToSphereDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0005", __FILE__, __FUNCSIG__); }
}

void DimSphereToSphereDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimSphereToSphereDistance::Clone(int n)
{
	try
	{
		DimSphereToSphereDistance* Cdim = new DimSphereToSphereDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0006", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimSphereToSphereDistance::CreateDummyCopy()
{
	try
	{
		DimSphereToSphereDistance* Cdim = new DimSphereToSphereDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0007", __FILE__, __FUNCSIG__); return NULL; }
}

void DimSphereToSphereDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0008", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimSphereToSphereDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimSphereToSphereDistance"<<endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os << "ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimSphereToSphereDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0009", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimSphereToSphereDistance& x)
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
			if(Tagname==L"DimSphereToSphereDistance")
			{
			while(Tagname!=L"EndDimSphereToSphereDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0010", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimSphereToSphereDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSPH2SPH0010", __FILE__, __FUNCSIG__); }
}