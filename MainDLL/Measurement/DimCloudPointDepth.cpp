#include "StdAfx.h"
#include "DimCloudPointDepth.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Engine\RCadApp.h"

DimCloudPointDepth::DimCloudPointDepth(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0001", __FILE__, __FUNCSIG__); }
}

DimCloudPointDepth::DimCloudPointDepth(bool simply):DimBase(false)
{
	try
	{
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0002", __FILE__, __FUNCSIG__); }
}

DimCloudPointDepth::~DimCloudPointDepth()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimCloudPointDepth::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimCloudPointDepth::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(this->ParentPoint1 != NULL)
			GRAFIX->drawPoint_PointDistance(ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint2->getX(), ParentPoint2->getY(), mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm,ParentPoint1->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0005", __FILE__, __FUNCSIG__); }
}

void DimCloudPointDepth::UpdateForParentChange(BaseItem* sender)
{
	if((this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0) && this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

void DimCloudPointDepth::CalculateMeasurement(Shape *s1)
{
	try
	{
		this->ParentShape1 = s1;
		MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CLOUDPOINTDEPTH;
		this->ParentPoint1 = ((CloudPoints*)this->ParentShape1)->getMaxpoint();
		this->ParentPoint2 = ((CloudPoints*)this->ParentShape1)->getMinpoint();
		double dist = this->ParentPoint1->getZ() - this->ParentPoint2->getZ();
		this->setDimension(dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0006", __FILE__, __FUNCSIG__); }
}

void DimCloudPointDepth::UpdateMeasurement()
{
	try
	{
		CalculateMeasurement(ParentShape1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0007", __FILE__, __FUNCSIG__); }
}

DimBase* DimCloudPointDepth::Clone(int n)
{
	try
	{
		DimCloudPointDepth* Cdim = new DimCloudPointDepth();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCloudPointDepth::CreateDummyCopy()
{
	try
	{
		DimCloudPointDepth* Cdim = new DimCloudPointDepth(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCloudPointDepth::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimCloudPointDepth& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCloudPointDepth"<<endl;
		os <<"ParentId:" << x.ParentShape1->getId() << endl;
		os<<"EndDimCloudPointDepth"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCloudPointDepth& x)
{
	try
	{
		int n = 0;
		is >> (*(DimBase*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimCloudPointDepth")
		{
			while(Tagname!=L"EndDimCloudPointDepth")
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
					if(Tagname==L"ParentId")
					{
						x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						x.UpdateMeasurement();
					}
				}
			}
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCLPT0012", __FILE__, __FUNCSIG__); return is; }
}