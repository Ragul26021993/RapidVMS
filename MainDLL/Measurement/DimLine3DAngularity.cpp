#include "StdAfx.h"
#include "DimLine3DAngularity.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimLine3DAngularity::DimLine3DAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0001", __FILE__, __FUNCSIG__); }
}

DimLine3DAngularity::DimLine3DAngularity(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0002", __FILE__, __FUNCSIG__); }
}

void DimLine3DAngularity::init()
{
	try
	{
		AngularityAngle = 0;
		Land = 500;
		this->MeasureAsGDnTMeasurement(true);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0003", __FILE__, __FUNCSIG__); }
}

DimLine3DAngularity::~DimLine3DAngularity()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimLine3DAngularity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM)
			dimcnt = parallelityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY)
			dimcnt = perpendicularityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY)
			dimcnt = angularityno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimLine3DAngularity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
				GRAFIX->drawAngularity3D(&DimSelectionLine[0], LineMidPt[0], LineMidPt[1], LineMidPt[2], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 0, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
				GRAFIX->drawAngularity3D(&DimSelectionLine[0], LineMidPt[0], LineMidPt[1], LineMidPt[2], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 1, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
				GRAFIX->drawAngularity3D(&DimSelectionLine[0], LineMidPt[0], LineMidPt[1], LineMidPt[2], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 2, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0006", __FILE__, __FUNCSIG__); }
}

void DimLine3DAngularity::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement((Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentShape1, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0007", __FILE__, __FUNCSIG__); }
}

void DimLine3DAngularity::SetLandAndAngle(double angle, double land)
{
	try
	{
		AngularityAngle = angle;
		Land = land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0008", __FILE__, __FUNCSIG__); }
}

void DimLine3DAngularity::CalculateMeasurement(Shape *s1, Shape *s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		double Line1Param[6] = {0}, RefShape[8] = {0}, dis = 0;
		((Line*)ParentShape1)->getParameters(&Line1Param[0]);
		((Line*)ParentShape1)->getMidPoint()->FillDoublePointer(&LineMidPt[0], 3);
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
	    	case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
			{
				if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					((Plane*)ParentShape2)->getParameters(&RefShape[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Plane(Line1Param, RefShape,  AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
				{
					((Cylinder*)ParentShape2)->getParameters(&RefShape[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(Line1Param, RefShape,  AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CONE)
				{
					((Cone*)ParentShape2)->getParameters(&RefShape[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(Line1Param, RefShape,  AngularityAngle, Land);
				}
				else 
				{
					((Line*)ParentShape2)->getParameters(&RefShape[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(Line1Param, RefShape,  AngularityAngle, Land);
				}
				this->setDimension(dis);
				break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0009", __FILE__, __FUNCSIG__); }
}

void DimLine3DAngularity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLine3DAngularity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM)
			myname = _T("l3dPar");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY)
			myname = _T("l3dPer");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY)
			myname = _T("l3dAng");
		DimLine3DAngularity* Cdim = new DimLine3DAngularity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0010", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLine3DAngularity::CreateDummyCopy()
{
	try
	{
		DimLine3DAngularity* Cdim = new DimLine3DAngularity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0011", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLine3DAngularity::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->AngularityAngle = ((DimLine3DAngularity*)Cdim)->AngularityAngle;
		this->Land = ((DimLine3DAngularity*)Cdim)->Land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0012", __FILE__, __FUNCSIG__); }
}


int DimLine3DAngularity::angularityno = 0;
int DimLine3DAngularity::parallelityno = 0;
int DimLine3DAngularity::perpendicularityno = 0;
void DimLine3DAngularity::reset()
{
	angularityno = 0;
	parallelityno = 0;
	perpendicularityno = 0;
}

wostream& operator<<(wostream& os, DimLine3DAngularity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimLine3DAngularity"<<endl;
		os <<"AngularityAngle:"<< x.AngularityAngle << endl;
		os <<"Land:"<< x.Land << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimLine3DAngularity"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0013", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLine3DAngularity& x)
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
			if(Tagname==L"DimLine3DAngularity")
			{
			while(Tagname!=L"EndDimLine3DAngularity")
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
						if(Tagname==L"AngularityAngle")
						{
							x.AngularityAngle=atof((const char*)(Val).c_str());
						}
						 else if(Tagname==L"Land")
						{
							x.Land =atof((const char*)(Val).c_str());
						}		
						 else if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 =(Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}		
						 else if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 =(Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}		
					 }
				}
				HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
				x.UpdateMeasurement();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0014", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLine3DAngularity& x)
{
	try
	{
		int n;
		is >> x.AngularityAngle;
		is >> x.Land;
		is >> n;
		x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 = (Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE3DANG0014", __FILE__, __FUNCSIG__);}
}