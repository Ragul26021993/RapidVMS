#include "StdAfx.h"
#include "DimCylinderAngularity.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimCylinderAngularity::DimCylinderAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0001", __FILE__, __FUNCSIG__); }
}

DimCylinderAngularity::DimCylinderAngularity(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0002", __FILE__, __FUNCSIG__); }
}

DimCylinderAngularity::~DimCylinderAngularity()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0003", __FILE__, __FUNCSIG__); }
}

void DimCylinderAngularity::init()
{
	try
	{
		AngularityAngle = 0;
		Land = 500;
		this->MeasureAsGDnTMeasurement(true);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimCylinderAngularity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY)
			dimcnt = perpendicularityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY)
			dimcnt = angularityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY)
			dimcnt = coaxilityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM)
			dimcnt = parallelismNo++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimCylinderAngularity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
				GRAFIX->drawAngularityCylinder(&DimSelectionLine[0], &CylinderParam[0], &CylinderEndPoints[0], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 0, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
				GRAFIX->drawAngularityCylinder(&DimSelectionLine[0], &CylinderParam[0], &CylinderEndPoints[0], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 2, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
				GRAFIX->drawAngularityCylinder(&DimSelectionLine[0], &CylinderParam[0], &CylinderEndPoints[0], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 1, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0006", __FILE__, __FUNCSIG__); }
}


void DimCylinderAngularity::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0007", __FILE__, __FUNCSIG__); }
}

void DimCylinderAngularity::SetLandAndAngle(double angle, double land)
{
	try
	{
		AngularityAngle = angle;
		Land = land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0008", __FILE__, __FUNCSIG__); }
}

void DimCylinderAngularity::CalculateMeasurement(Shape *s1, Shape *s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		((Cylinder*)ParentShape1)->getParameters(&CylinderParam[0]);
		((Cylinder*)ParentShape1)->getEndPoints(&CylinderEndPoints[0]);
		double otherParm[8] = {0}, dis = 0;
		
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
			{
				if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					((Plane*)ParentShape2)->getParameters(&otherParm[3]);	
					dis = RMATH3DOBJECT->Angularity_Line_wrt_Plane(&CylinderParam[0], &otherParm[3], AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
				{
					((Cylinder*)ParentShape2)->getParameters(&otherParm[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(CylinderParam, otherParm,  AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CONE)
				{
					((Cone*)ParentShape2)->getParameters(&otherParm[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(CylinderParam, otherParm,  AngularityAngle, Land);
				}
				else 
				{
					((Line*)ParentShape2)->getParameters(&otherParm[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Line(CylinderParam, otherParm,  AngularityAngle, Land);
				}
				this->setDimension(dis);
				break;
			}
			case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
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
				dis = RMATH3DOBJECT->Coaxiality(CylinderEndPoints, otherParm, Land);
				this->setDimension(dis);
				break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0009", __FILE__, __FUNCSIG__); }
}

void DimCylinderAngularity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
} 

DimBase* DimCylinderAngularity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY)
			myname = _T("CylPer");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY)
			myname = _T("CylAng");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY)
			myname = _T("CylCoax");
	    else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM)
			myname = _T("CylPar");
		DimCylinderAngularity* Cdim = new DimCylinderAngularity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0010", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCylinderAngularity::CreateDummyCopy()
{
	try
	{
		DimCylinderAngularity* Cdim = new DimCylinderAngularity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0011", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCylinderAngularity::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->AngularityAngle = ((DimCylinderAngularity*)Cdim)->AngularityAngle;
		this->Land = ((DimCylinderAngularity*)Cdim)->Land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0012", __FILE__, __FUNCSIG__); }
}

int DimCylinderAngularity::angularityno = 0;
int DimCylinderAngularity::coaxilityno = 0;
int DimCylinderAngularity::perpendicularityno = 0;
int DimCylinderAngularity::parallelismNo = 0;
void DimCylinderAngularity::reset()
{
	angularityno = 0;
	coaxilityno = 0;
	perpendicularityno = 0;
	parallelismNo = 0;
}

wostream& operator<<(wostream& os, DimCylinderAngularity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCylinderAngularity"<<endl;
		os <<"AngularityAngle:"<< x.AngularityAngle << endl;
		os <<"Land:"<< x.Land << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimCylinderAngularity"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0013", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimCylinderAngularity& x)
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
			if(Tagname==L"DimCylinderAngularity")
			{
				while(Tagname!=L"EndDimCylinderAngularity")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0014", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCylinderAngularity& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCYANG0014", __FILE__, __FUNCSIG__); }
}