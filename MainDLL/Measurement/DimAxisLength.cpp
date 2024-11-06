#include "StdAfx.h"
#include "DimAxisLength.h"
#include "..\Shapes\Conics3D.h"
#include "..\Engine\RCadApp.h"

DimAxisLength::DimAxisLength(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		this->TotalNormalmeasureType(2);
		this->MeasureAs3DMeasurement(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0001", __FILE__, __FUNCSIG__); }
}

DimAxisLength::DimAxisLength(bool simply):DimBase(false)
{
	try{this->MeasureAs3DMeasurement(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0002", __FILE__, __FUNCSIG__); }
}

DimAxisLength::~DimAxisLength()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimAxisLength::genName(const TCHAR* prefix)
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

void DimAxisLength::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawAxis3D(&Conics3DParam[0], Max, Min, doubledimesion(), getCDimension(), getModifiedName(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, NormalmeasureType(), Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0005", __FILE__, __FUNCSIG__); }
}

void DimAxisLength::CalculateMeasurement(Shape* s1)
{
	try
	{
		this->ParentShape1 = s1;
		MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_AXISLENGTH;
		double Dist;
		switch(ParentShape1->ShapeType)
		{
			case RapidEnums::SHAPETYPE::ELLIPSE3D:
				((Conics3D*)ParentShape1)->getParameters(&Conics3DParam[0]);
				Max = (2 * Conics3DParam[13] *Conics3DParam[16])/(1 - Conics3DParam[13] * Conics3DParam[13]);
				Min = sqrt((Max * Max) * (1 - Conics3DParam[13] * Conics3DParam[13]));
				if (NormalmeasureType() == 0)
				{
					setDimension(Max);
					Dist = Max;
				}
				else 
				{
					setDimension(Min);
					Dist = Min;
				}
			break;
		}
		setDimension(Dist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0006", __FILE__, __FUNCSIG__); }
}

void DimAxisLength::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1);
}

void DimAxisLength::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimAxisLength::Clone(int n)
{
	try
	{
		DimAxisLength* Cdim = new DimAxisLength();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimAxisLength::CreateDummyCopy()
{
	try
	{
		DimAxisLength* Cdim = new DimAxisLength(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimAxisLength::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimAxisLength& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimAxisLength"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os<<"EndDimAxisLength"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimAxisLength& x)
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
			if(Tagname==L"DimAxisLength")
			{
			while(Tagname!=L"EndDimAxisLength")
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
						if(Tagname==L"ParentShape1")
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0011", __FILE__, __FUNCSIG__); return is;}
}
void ReadOldPP(wistream& is, DimAxisLength& x)
{
	try
	{
		int n;		
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDIA3D0011", __FILE__, __FUNCSIG__);}
}