#include "StdAfx.h"
#include "DimAngle3D.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"

DimAngle3D::DimAngle3D(TCHAR* myname):DimBase(genName(myname))
{
	init();
}

DimAngle3D::DimAngle3D(bool simply):DimBase(false)
{
	init();
}

void DimAngle3D::init()
{
	try
	{
		MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONEANGLE;
		this->DistanceMeasurement(false);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0002", __FILE__, __FUNCSIG__); }
}

DimAngle3D::~DimAngle3D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimAngle3D::genName(const TCHAR* prefix)
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

void DimAngle3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawFiniteline_FinitelineAngle3D(&SfLine1[0], &SfEndPnts1[0], &SfLine2[0], &SfEndPnts2[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth,TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0005", __FILE__, __FUNCSIG__); }
}

void DimAngle3D::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONEANGLE;
		double angle, Tempangle;
		char AngledegMin[100];
	    switch(ParentShape1->ShapeType)
		{
				 case RapidEnums::SHAPETYPE::CONE:
					Cone* ConeSh = (Cone*)ParentShape1;
			    	ConeSh->getParameters(&ConeParam[0]);
					ConeSh->getEndPoints(&ConeEndPoints[0]);
					double dir[4];
	           		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pt(&ConeEndPoints[0], &ConeEndPoints[3], &TransformationMatrix[0], &MeasurementPlane[0]);
					RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&ConeParam[3], &MeasurementPlane[0], &dir[0]);
					ConeSh->getSurfaceLine(&dir[0], true, &SfLine1[0], &SfEndPnts1[0]);
					ConeSh->getSurfaceLine(&dir[0], false, &SfLine2[0], &SfEndPnts2[0]);
				    angle = RMATH3DOBJECT->Angle_Line_Line( &SfLine1[0], &SfLine2[0], MeasurementPlane, &DimSelectionLine[0]);
					this->DistanceMeasurement(false);
					this->setAngularDimension(angle);
					break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0006", __FILE__, __FUNCSIG__); }
}

void DimAngle3D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimAngle3D::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimAngle3D::Clone(int n)
{
	try
	{
		DimAngle3D* Cdim = new DimAngle3D();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimAngle3D::CreateDummyCopy()
{
	try
	{
		DimAngle3D* Cdim = new DimAngle3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimAngle3D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimAngle3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimAngle3D"<<endl;
		os << "ParentShape1ID:" << x.ParentShape1->getId() << endl;
		os<<"EndDimAngle3D"<<endl;
		//os << x.ParentShape1->getId() << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimAngle3D& x)
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
			if(Tagname==L"DimAngle3D")
			{
				while(Tagname!=L"EndDimAngle3D")
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
						if(Tagname==L"ParentShape1ID")
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimAngle3D& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("OLDDIMDIA3D0011", __FILE__, __FUNCSIG__); }
}