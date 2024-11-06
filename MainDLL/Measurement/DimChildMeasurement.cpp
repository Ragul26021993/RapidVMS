#include "StdAfx.h"
#include "DimChildMeasurement.h"
#include "..\Engine\RCadApp.h"

DimChildMeasurement::DimChildMeasurement(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0001", __FILE__, __FUNCSIG__); }
}


DimChildMeasurement::~DimChildMeasurement()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0002", __FILE__, __FUNCSIG__); }
}

void DimChildMeasurement::init()
{
	try
	{
		this->ChildMeasurementType(true);
		this->ChildMeasureValue(0);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0003", __FILE__, __FUNCSIG__); }
}

void DimChildMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0006", __FILE__, __FUNCSIG__); }
}

void DimChildMeasurement::UpdateForParentChange(BaseItem* sender)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0007", __FILE__, __FUNCSIG__); }
}

void DimChildMeasurement::UpdateMeasurement()
{
}

DimBase* DimChildMeasurement::Clone(int n)
{
	try
	{
		DimChildMeasurement* Cdim = new DimChildMeasurement(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimChildMeasurement::CreateDummyCopy()
{
	try
	{
		DimChildMeasurement* Cdim = new DimChildMeasurement(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimChildMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		DimChildMeasurement* dim = (DimChildMeasurement*)Cdim;
		this->ChildMeasureValue(dim->ChildMeasureValue());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0010", __FILE__, __FUNCSIG__); }
}


wostream& operator<<( wostream& os, DimChildMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimChildMeasurement"<<endl;
		os <<"ChildMeasureValue:"<< x.ChildMeasureValue() << endl;
		os<<"EndDimChildMeasurement"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimChildMeasurement& x)
{
	try
	{
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	std::wstring Tagname ;
			is>>Tagname;
			is >> (*(DimBase*)&x);			
			is >> Tagname;
			if(Tagname==L"DimChildMeasurement")
			{
			while(Tagname!=L"EndDimChildMeasurement")
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
						if(Tagname==L"ChildMeasureValue")
						{
							x.ChildMeasureValue(atof((const char*)(Val).c_str()));
						}					 			
					 }
				}
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimChildMeasurement& x)
{
	try
	{
		double d;
		is >> d;
		is >> (*(DimBase*)&x);
		is >> d; x.ChildMeasureValue(d);		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCHILD0013", __FILE__, __FUNCSIG__); }
}