#include "StdAfx.h"
#include "DimDigitalMicrometer.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\SnapPoint.h"

DimDigitalMicrometer::DimDigitalMicrometer(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		this->DrawneartheMousePosition = false;
		this->MeasurementType = RapidEnums::DIM_DIGITALMICROMETER;
		//Vector* vv = new Vector(0,0,0); this->Endpoint1 = vv;
		//vv = new Vector(0, 0, 0); this->Endpoint2 = vv;
		//vv = new Vector(0, 0, 0); this->ClickedPoint1 = vv;
		//vv = new Vector(0, 0, 0); this->ClickedPoint2 = vv;
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0001", __FILE__, __FUNCSIG__); }
}

DimDigitalMicrometer::DimDigitalMicrometer(bool simply):DimBase(false)
{
	try
	{
		this->DrawneartheMousePosition = false;
		this->MeasurementType = RapidEnums::DIM_DIGITALMICROMETER;
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0002", __FILE__, __FUNCSIG__); }
}

DimDigitalMicrometer::~DimDigitalMicrometer()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimDigitalMicrometer::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		digitalumno++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(digitalumno, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimDigitalMicrometer::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(DrawneartheMousePosition)
			GRAFIX->drawDigital_Micrometer(ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint2->getX(), ParentPoint2->getY(), ClickedPoint1->getX(), ClickedPoint1->getY(), ClickedPoint2->getX(), ClickedPoint2->getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, true, Left_Top, Right_Btm, ParentPoint1->getZ());
		else
			GRAFIX->drawDigital_Micrometer(ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint2->getX(), ParentPoint2->getY(),  ClickedPoint1->getX(), ClickedPoint1->getY(), ClickedPoint2->getX(), ClickedPoint2->getY(), doubledimesion(),getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, false, Left_Top, Right_Btm, ParentPoint1->getZ());
		GRAFIX->drawPoint(ClickedPoint1->getX(), ClickedPoint1->getY(), ClickedPoint1->getZ());
		GRAFIX->drawPoint(ClickedPoint2->getX(), ClickedPoint2->getY(), ClickedPoint1->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0005", __FILE__, __FUNCSIG__); }
}

void DimDigitalMicrometer::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	UpdateMeasurement();
}

void DimDigitalMicrometer::CalculateMeasurement(Vector *v1, Vector *v2)
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DIGITALMICROMETER;
		Endpoint1 = new Vector(v1->getX(), v1->getY(), v1->getZ()); 
		ParentPoint1 = Endpoint1;
		Endpoint2 = new Vector(v2->getX(), v2->getY(), v2->getZ()); 
		ParentPoint2 = Endpoint2;		
		double p1[2] = {ParentPoint1->getX(), ParentPoint1->getY()}, p2[2] = {ParentPoint2->getX(), ParentPoint2->getY()};
		double mp[2] = {mposition.getX(), mposition.getY()}, endp1[2], endp2[2], dis;
		dis = RMATH2DOBJECT->Pt2Pt_distance(&p1[0], &p2[0]);
		RMATH2DOBJECT->EditDigitalMicrometer(&p1[0],&p2[0], &mp[0], &endp1[0], &endp2[0]);
		Endpoint1->set(endp1[0], endp1[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
		ParentPoint1 = Endpoint1;
		Endpoint2->set(endp2[0], endp2[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
		ParentPoint2 = Endpoint2;
		this->setDimension(dis);	
	}
	catch (...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0006", __FILE__, __FUNCSIG__);
	}
}

void DimDigitalMicrometer::SetFirstOriginalPoint1(double x, double y, double z)
{
	ClickedPoint1 = new Vector(x, y, z);
}

void DimDigitalMicrometer::SetFirstOriginalPoint2(double x, double y, double z)
{
	ClickedPoint2 = new Vector(x, y, z);
}

void DimDigitalMicrometer::UpdateMeasurement()
{
	CalculateMeasurement(ParentPoint1, ParentPoint2);
}

DimBase* DimDigitalMicrometer::Clone(int n)
{
	try
	{
		DimDigitalMicrometer* Cdim = new DimDigitalMicrometer();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimDigitalMicrometer::CreateDummyCopy()
{
	try
	{
		DimDigitalMicrometer* Cdim = new DimDigitalMicrometer(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimDigitalMicrometer::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		this->ClickedPoint1 = ((DimDigitalMicrometer*)Cdim)->ClickedPoint1;
		this->ClickedPoint2 = ((DimDigitalMicrometer*)Cdim)->ClickedPoint2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0009", __FILE__, __FUNCSIG__); }
}

int DimDigitalMicrometer::digitalumno = 0;
void DimDigitalMicrometer::reset()
{
	digitalumno = 0;
}

wostream& operator<<( wostream& os, DimDigitalMicrometer& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os << "DimDigitalMicrometer" << endl;
		os <<"DrawneartheMousePosition:"<< x.DrawneartheMousePosition << endl;
		os <<"Endpoint1:Values"<<endl<< (*static_cast<Vector*>(x.Endpoint1)) << endl;
		os <<"Endpoint2:Values"<<endl<< (*static_cast<Vector*>(x.Endpoint2)) << endl;
		os <<"ClickedPoint1:Values"<<endl<< (*static_cast<Vector*>(x.ClickedPoint1)) << endl;
		os <<"ClickedPoint2:Values"<<endl<< (*static_cast<Vector*>(x.ClickedPoint2)) << endl;
		os <<"EndDimDigitalMicrometer" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimDigitalMicrometer& x)
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
			if(Tagname==L"DimDigitalMicrometer")
			{
				while(Tagname!=L"EndDimDigitalMicrometer")
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
						if(Tagname==L"DrawneartheMousePosition")
						{
							if(Val=="0")
							{
								x.DrawneartheMousePosition=false;							
							}
							else						
								x.DrawneartheMousePosition=true;
						}		
						else if(Tagname==L"Endpoint1")
						{
							x.Endpoint1 = new Vector(0, 0, 0);
							is >> (*static_cast<Vector*>(x.Endpoint1));
						}	
						else if(Tagname==L"Endpoint2")
						{
							x.Endpoint2 = new Vector(0, 0, 0);
							is >> (*static_cast<Vector*>(x.Endpoint2));
						}	
						else if(Tagname==L"ClickedPoint1")
						{
							x.ClickedPoint1 = new Vector(0, 0, 0);
							is >> (*static_cast<Vector*>(x.ClickedPoint1));
						}	
						else if(Tagname==L"ClickedPoint2")
						{
							x.ClickedPoint2 = new Vector(0, 0, 0);
							is >> (*static_cast<Vector*>(x.ClickedPoint2));
						}						
					}
				}
				x.ParentPoint1 = x.Endpoint1;
				x.ParentPoint2 = x.Endpoint2;
				x.UpdateMeasurement();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimDigitalMicrometer& x)
{
	try
	{
		is >> x.DrawneartheMousePosition;
		is >> (*static_cast<Vector*>(x.Endpoint1));
		is >> (*static_cast<Vector*>(x.Endpoint2));
		is >> (*static_cast<Vector*>(x.ClickedPoint1));
		is >> (*static_cast<Vector*>(x.ClickedPoint2));
		x.ParentPoint1 = x.Endpoint1;
		x.ParentPoint2 = x.Endpoint2;
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDMM0011", __FILE__, __FUNCSIG__);}
}