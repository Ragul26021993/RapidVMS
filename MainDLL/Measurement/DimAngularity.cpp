#include "StdAfx.h"
#include "DimAngularity.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimAngularity::DimAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{ init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0001", __FILE__, __FUNCSIG__); }
}

DimAngularity::DimAngularity(bool simply):DimBase(false)
{
	try{ init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0002", __FILE__, __FUNCSIG__); }
}

DimAngularity::~DimAngularity()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0003", __FILE__, __FUNCSIG__); }
}

void DimAngularity::init()
{
	try
	{
		AngularityAngle = 0;
		Land = 500;
		this->MeasureAsGDnTMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimAngularity::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM)
			dimcnt = parallelityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY)
			dimcnt = perpendicularityno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY)
			dimcnt = angularityno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0005", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimAngularity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double Midp[3] = {((Line*)ParentShape1)->getMidPoint()->getX(), ((Line*)ParentShape1)->getMidPoint()->getY(), ((Line*)ParentShape1)->getMidPoint()->getZ()};
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
				GRAFIX->drawAngularity(mposition.getX(), mposition.getY(), Midp[0], Midp[1], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 0, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Midp[2]);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
				GRAFIX->drawAngularity(mposition.getX(), mposition.getY(), Midp[0], Midp[1], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 1, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Midp[2]);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
				GRAFIX->drawAngularity(mposition.getX(), mposition.getY(), Midp[0], Midp[1], getCDimension(), &ParentShape2->DatumName, getModifiedName(), 2, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Midp[2]);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0006", __FILE__, __FUNCSIG__); }
}

void DimAngularity::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0007", __FILE__, __FUNCSIG__); }
}

void DimAngularity::SetLandAndAngle(double angle, double land)
{
	try
	{
		AngularityAngle = angle;
		Land = land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0008", __FILE__, __FUNCSIG__); }
}

void DimAngularity::CalculateMeasurement(Shape *s1, Shape *s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		double angle1 = ((Line*)ParentShape1)->Angle(), angle2 = ((Line*)ParentShape2)->Angle(), dis;
		switch(this->MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
				RMATH2DOBJECT->AngularityCalculation(angle1, angle2, Land, AngularityAngle, &dis);
				this->setDimension(dis);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0009", __FILE__, __FUNCSIG__); }
}

void DimAngularity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
} 

DimBase* DimAngularity::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM)
			myname = _T("Par");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY)
			myname = _T("Per");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY)
			myname = _T("Anglty");
		DimAngularity* Cdim = new DimAngularity((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0010", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimAngularity::CreateDummyCopy()
{
	try
	{
		DimAngularity* Cdim = new DimAngularity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0011", __FILE__, __FUNCSIG__); return NULL; }
}

void DimAngularity::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->AngularityAngle = ((DimAngularity*)Cdim)->AngularityAngle;
		this->Land = ((DimAngularity*)Cdim)->Land;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0012", __FILE__, __FUNCSIG__); }
}

int DimAngularity::angularityno = 0;
int DimAngularity::parallelityno = 0;
int DimAngularity::perpendicularityno = 0;
void DimAngularity::reset()
{
	angularityno = 0;
	parallelityno = 0;
	perpendicularityno = 0;
}

wostream& operator<<(wostream& os, DimAngularity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimAngularity"<<endl;
		os <<"AngularityAngle:"<< x.AngularityAngle << endl;
		os <<"Land:"<< x.Land << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimAngularity"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0013", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimAngularity& x)
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
			if(Tagname==L"DimAngularity")
			{
				while(Tagname!=L"EndDimAngularity")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0014", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimAngularity& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMANG0014", __FILE__, __FUNCSIG__);}
}