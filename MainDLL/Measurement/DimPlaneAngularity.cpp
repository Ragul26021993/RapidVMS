#include "StdAfx.h"
#include "DimPlaneAngularity.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimPlaneAngularity::DimPlaneAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0001", __FILE__, __FUNCSIG__); }
}

DimPlaneAngularity::DimPlaneAngularity(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0002", __FILE__, __FUNCSIG__); }
}

void DimPlaneAngularity::init()
{
	try
	{
		AngularityAngle = 0;
		Land = 500;
		this->MeasureAsGDnTMeasurement(true);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0003", __FILE__, __FUNCSIG__); }
}

DimPlaneAngularity::~DimPlaneAngularity()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPlaneAngularity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM)
			dimcnt = parallelityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY)
			dimcnt = perpendicularityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY)
			dimcnt = angularityno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPlaneAngularity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
				GRAFIX->drawAngularity3D(&DimSelectionLine[0], PlaneMidPt[0], PlaneMidPt[1], PlaneMidPt[2], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 0, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
				GRAFIX->drawAngularity3D(&DimSelectionLine[0], PlaneMidPt[0], PlaneMidPt[1], PlaneMidPt[2], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 1, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
				GRAFIX->drawAngularity3D(&DimSelectionLine[0], PlaneMidPt[0], PlaneMidPt[1], PlaneMidPt[2], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 2, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0006", __FILE__, __FUNCSIG__); }
}

void DimPlaneAngularity::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0007", __FILE__, __FUNCSIG__); }
}

void DimPlaneAngularity::SetLandAndAngle(double angle, double land)
{
	try
	{
		AngularityAngle = angle;
		Land = land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0008", __FILE__, __FUNCSIG__); }
}

void DimPlaneAngularity::CalculateMeasurement(Shape *s1, Shape *s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		double Plane1Param[4] = {0}, RefShape[8] = {0}, dis = 0;
		((Plane*)ParentShape1)->getParameters(&Plane1Param[0]);
		double pt1[3], pt2[3];
		((Plane*)ParentShape1)->getPoint1()->FillDoublePointer(&pt1[0], 3);
		((Plane*)ParentShape1)->getPoint3()->FillDoublePointer(&pt2[0], 3);
		PlaneMidPt[0] = (pt1[0] + pt2[0])/2;
		PlaneMidPt[1] = (pt1[1] + pt2[1])/2;
		PlaneMidPt[2] = (pt1[2] + pt2[2])/2;
		switch(this->MeasurementType)
		{
		case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
		case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
		case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
			{
				if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::LINE3D || ParentShape2->ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
				{
					((Line*)ParentShape2)->getParameters(&RefShape[0]);
					 dis = RMATH3DOBJECT->Angularity_Line_wrt_Plane(RefShape, Plane1Param, AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER) 
				{
					((Cylinder*)ParentShape2)->getParameters(&RefShape[0]);
			     	 dis = RMATH3DOBJECT->Angularity_Line_wrt_Plane(RefShape, Plane1Param, AngularityAngle, Land);
				}
				else if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CONE) 
				{
					((Cone*)ParentShape2)->getParameters(&RefShape[0]);
			     	 dis = RMATH3DOBJECT->Angularity_Line_wrt_Plane(RefShape, Plane1Param, AngularityAngle, Land);
				}
				else 
				{
					((Plane*)ParentShape2)->getParameters(&RefShape[0]);
					dis = RMATH3DOBJECT->Angularity_Plane_wrt_Plane(&Plane1Param[0], &RefShape[0],  AngularityAngle, Land);
				}
				this->setDimension(dis);
				break;
			}
		 }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0009", __FILE__, __FUNCSIG__); }
}

void DimPlaneAngularity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
} 

DimBase* DimPlaneAngularity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM)
			myname = _T("PlPar");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY)
			myname = _T("PlPer");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY)
			myname = _T("PlAng");
		DimPlaneAngularity* Cdim = new DimPlaneAngularity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0010", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPlaneAngularity::CreateDummyCopy()
{
	try
	{
		DimPlaneAngularity* Cdim = new DimPlaneAngularity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0011", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPlaneAngularity::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->AngularityAngle = ((DimPlaneAngularity*)Cdim)->AngularityAngle;
		this->Land = ((DimPlaneAngularity*)Cdim)->Land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0012", __FILE__, __FUNCSIG__); }
}

int DimPlaneAngularity::angularityno = 0;
int DimPlaneAngularity::parallelityno = 0;
int DimPlaneAngularity::perpendicularityno = 0;
void DimPlaneAngularity::reset()
{
	angularityno = 0;
	parallelityno = 0;
	perpendicularityno = 0;
}

wostream& operator<<(wostream& os, DimPlaneAngularity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPlaneAngularity"<<endl;
		os <<"AngularityAngle:"<< x.AngularityAngle << endl;
		os <<"Land:"<< x.Land << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimPlaneAngularity"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0013", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPlaneAngularity& x)
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
			if(Tagname==L"DimPlaneAngularity")
			{
			while(Tagname!=L"EndDimPlaneAngularity")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0014", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPlaneAngularity& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPLANEANG0014", __FILE__, __FUNCSIG__); }
}