#include "StdAfx.h"
#include "DimConeAngularity.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Line.h"
#include "..\Helper\Helper.h"
#include "..\Engine\RCadApp.h"

DimConeAngularity::DimConeAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0001", __FILE__, __FUNCSIG__); }
}

DimConeAngularity::DimConeAngularity(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0002", __FILE__, __FUNCSIG__); }
}

DimConeAngularity::~DimConeAngularity()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0003", __FILE__, __FUNCSIG__); }
}

void DimConeAngularity::init()
{
	try
	{
		AngularityAngle = 0;
		Land = 500;
		this->MeasureAsGDnTMeasurement(true);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimConeAngularity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
	    if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY)
			dimcnt = perpendicularityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY)
			dimcnt = angularityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY)
			dimcnt = coaxilityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM)
			dimcnt = parallelismNo++;
		
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimConeAngularity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
				GRAFIX->drawAngularityCylinder(&DimSelectionLine[0], &ConeParam[0], &ConeEndPoints[0], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 0, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
				GRAFIX->drawAngularityCylinder(&DimSelectionLine[0], &ConeParam[0], &ConeEndPoints[0], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 2, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
				GRAFIX->drawAngularityCylinder(&DimSelectionLine[0], &ConeParam[0], &ConeEndPoints[0], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 1, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0006", __FILE__, __FUNCSIG__); }
}

void DimConeAngularity::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0007", __FILE__, __FUNCSIG__); }
}

void DimConeAngularity::SetLandAndAngle(double angle, double land)
{
	try
	{
		AngularityAngle = angle;
		Land = land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0008", __FILE__, __FUNCSIG__); }
}

void DimConeAngularity::CalculateMeasurement(Shape *s1, Shape *s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		((Cone*)ParentShape1)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape1)->getEndPoints(&ConeEndPoints[0]);
	
		double datumEndPnts[6] = {0}, otherParm[8] = {0}, dis = 0;

     	switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
			{
				if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					((Plane*)ParentShape2)->getParameters(&otherParm[3]);	
					dis = RMATH3DOBJECT->Angularity_Line_wrt_Plane(&ConeParam[0], &otherParm[3], AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
				{
					((Cylinder*)ParentShape2)->getParameters(&otherParm[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(ConeParam, otherParm,  AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CONE)
				{
					((Cone*)ParentShape2)->getParameters(&otherParm[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(ConeParam, otherParm,  AngularityAngle, Land);
				}
				else 
				{
					((Line*)ParentShape2)->getParameters(&otherParm[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(ConeParam, otherParm,  AngularityAngle, Land);
				}
				this->setDimension(dis);
				break;
			}
			case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
			{
				if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CONE)
				{
					((Cone*)ParentShape2)->getParameters(&otherParm[0]);
			    }
		    	else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
				{
					((Cylinder*)ParentShape2)->getParameters(&otherParm[0]);
			    }
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::LINE3D || ParentShape2->ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
				{
					((Line*)ParentShape2)->getParameters(&otherParm[0]);
				}
				dis = RMATH3DOBJECT->Coaxiality(ConeEndPoints, otherParm, Land);
				this->setDimension(dis);
				break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0009", __FILE__, __FUNCSIG__); }
}

void DimConeAngularity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
} 

DimBase* DimConeAngularity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY)
			myname = _T("ConPer");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY)
			myname = _T("ConAng");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY)
			myname = _T("ConCoax");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM)
			myname = _T("ConPar");
		DimConeAngularity* Cdim = new DimConeAngularity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0010", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimConeAngularity::CreateDummyCopy()
{
	try
	{
		DimConeAngularity* Cdim = new DimConeAngularity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0011", __FILE__, __FUNCSIG__); return NULL; }
}

void DimConeAngularity::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->AngularityAngle = ((DimConeAngularity*)Cdim)->AngularityAngle;
		this->Land = ((DimConeAngularity*)Cdim)->Land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0012", __FILE__, __FUNCSIG__); }
}

int DimConeAngularity::angularityno = 0;
int DimConeAngularity::coaxilityno = 0;
int DimConeAngularity::perpendicularityno = 0;
int DimConeAngularity::parallelismNo = 0;
void DimConeAngularity::reset()
{
	angularityno = 0;
	coaxilityno = 0;
	perpendicularityno = 0;
	parallelismNo = 0;
}

wostream& operator<<(wostream& os, DimConeAngularity& x)
{
	try
	{

		os << (*static_cast<DimBase*>(&x));
		os<<"DimConeAngularity"<<endl;
		os <<"AngularityAngle:"<< x.AngularityAngle << endl;
		os <<"Land:"<< x.Land << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimConeAngularity"<<endl;	
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0013", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimConeAngularity& x)
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
			if(Tagname==L"DimConeAngularity")
			{
				while(Tagname!=L"EndDimConeAngularity")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0014", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimConeAngularity& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCNANG0014", __FILE__, __FUNCSIG__);}
}