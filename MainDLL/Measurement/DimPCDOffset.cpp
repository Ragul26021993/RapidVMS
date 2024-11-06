#include "StdAfx.h"
#include "DimPCDOffset.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimPCDOffset::DimPCDOffset(TCHAR* myname):DimBase(genName(myname))
{
	try{IsValid(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0001", __FILE__, __FUNCSIG__); }
}

DimPCDOffset::DimPCDOffset(bool simply):DimBase(false)
{
	try{IsValid(true);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0002", __FILE__, __FUNCSIG__); }
}

DimPCDOffset::~DimPCDOffset()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPCDOffset::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		pcdoffsetcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(pcdoffsetcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPCDOffset::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(&center[0], ((Circle*)ParentShape2)->Radius() * 2, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0005", __FILE__, __FUNCSIG__); }
}

void DimPCDOffset::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0006", __FILE__, __FUNCSIG__); }
}

void DimPCDOffset::CalculateMeasurement(Shape *s1, Shape *s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PCDOFFSET;
		((Circle*)ParentShape2)->getCenter()->FillDoublePointer(center, 3);
		double cirParam[7] = {0}, ShCenter[2] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY()};
		double OffsetDist = RMATH2DOBJECT->Pt2Pt_distance(center[0], center[1], ShCenter[0], ShCenter[1]);
		OffsetDist = ((Circle*)ParentShape1)->Radius() - OffsetDist;
		((Circle*)ParentShape2)->getParameters(cirParam);
		RMATH3DOBJECT->TransformationMatrix_Dia_Circle(cirParam, &TransformationMatrix[0], &MeasurementPlane[0]);
		this->setDimension(OffsetDist);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0007", __FILE__, __FUNCSIG__); }
}

void DimPCDOffset::UpdateMeasurement()
{
	if(!IsValid()) return;
	CalculateMeasurement(ParentShape1, ParentShape2);

}

DimBase* DimPCDOffset::Clone(int n)
{
	try
	{
		DimPCDOffset* Cdim = new DimPCDOffset();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPCDOffset::CreateDummyCopy()
{
	try
	{
		DimPCDOffset* Cdim = new DimPCDOffset(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPCDOffset::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0010", __FILE__, __FUNCSIG__); }
}

int DimPCDOffset::pcdoffsetcnt = 0;
void DimPCDOffset::reset()
{
	pcdoffsetcnt = 0;
}

wostream& operator<<( wostream& os, DimPCDOffset& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPCDOffset"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimPCDOffset"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPCDOffset& x)
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
			if(Tagname==L"DimPCDOffset")
			{
			while(Tagname!=L"EndDimPCDOffset")
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
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname=Tag;												
						if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}		
						if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPCDOffset& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDOFF0012", __FILE__, __FUNCSIG__); }
}