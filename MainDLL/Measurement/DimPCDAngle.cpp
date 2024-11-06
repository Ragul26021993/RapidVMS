#include "StdAfx.h"
#include "DimPCDAngle.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimPCDAngle::DimPCDAngle(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		this->DistanceMeasurement(false);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0001", __FILE__, __FUNCSIG__); }
}

DimPCDAngle::DimPCDAngle(bool simply):DimBase(false)
{
	try{ this->DistanceMeasurement(false); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0002", __FILE__, __FUNCSIG__); }
}

DimPCDAngle::~DimPCDAngle()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPCDAngle::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		pcdangle++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(pcdangle, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPCDAngle::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawPCDAngle(PcdCenter[0], PcdCenter[1], angle1, intercept1, angle2, intercept2, mposition.getX(), mposition.getY(), drawOrietation, getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, PcdCenter[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0005", __FILE__, __FUNCSIG__); }
}

void DimPCDAngle::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement((Shape*)sender, ParentShape2, ParentShape3);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentShape1, (Shape*)sender, ParentShape3);
		else if(sender->getId() == ParentShape3->getId())
			this->CalculateMeasurement(ParentShape1, ParentShape2, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0006", __FILE__, __FUNCSIG__); }
}

void DimPCDAngle::CalculateMeasurement(Shape *s1, Shape *s2, Shape *s3)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		this->ParentShape3 = s3;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PCDANGLE;
		double PcdAngle, temp;
		PcdCenter[0] = ((Circle*)ParentShape1)->getCenter()->getX(); PcdCenter[1] = ((Circle*)ParentShape1)->getCenter()->getY(); PcdCenter[2] = ((Circle*)ParentShape1)->getCenter()->getZ();
		ShCenter1[0] = ((Circle*)ParentShape2)->getCenter()->getX(); ShCenter1[1] = ((Circle*)ParentShape2)->getCenter()->getY();
		ShCenter2[0] = ((Circle*)ParentShape3)->getCenter()->getX(); ShCenter2[1] = ((Circle*)ParentShape3)->getCenter()->getY();
		angle1 = RMATH2DOBJECT->ray_angle(PcdCenter[0], PcdCenter[1], ShCenter1[0], ShCenter1[1]);
		angle2 = RMATH2DOBJECT->ray_angle(PcdCenter[0], PcdCenter[1], ShCenter2[0], ShCenter2[1]);
		mouseAngle = RMATH2DOBJECT->ray_angle(PcdCenter[0], PcdCenter[1], mposition.getX(), mposition.getY());
		if(angle1 > angle2)
		{
			temp = angle1; angle1 = angle2; angle2 = temp;
			temp = ShCenter1[0]; ShCenter1[0] = ShCenter2[0]; ShCenter2[0] = temp;
			temp = ShCenter1[1]; ShCenter1[1] = ShCenter2[1]; ShCenter2[1] = temp;
		}
		RMATH2DOBJECT->Intercept_LinePassing_Point(&ShCenter1[0], angle1, &intercept1);
		RMATH2DOBJECT->Intercept_LinePassing_Point(&ShCenter2[0], angle2, &intercept2);
		if(mouseAngle <= angle2 && mouseAngle > angle1)
		{
			PcdAngle = angle2 - angle1;
			drawOrietation = true;
		}
		else
		{
			PcdAngle = angle1 - angle2;
			drawOrietation = false;
		}
		RMATH2DOBJECT->Angle_FourQuad(&PcdAngle);
		this->DistanceMeasurement(false);
		this->setAngularDimension(PcdAngle);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0007", __FILE__, __FUNCSIG__); }
}

void DimPCDAngle::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2, ParentShape3);
} 

DimBase* DimPCDAngle::Clone(int n)
{
	try
	{
		DimPCDAngle* Cdim = new DimPCDAngle();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPCDAngle::CreateDummyCopy()
{
	try
	{
		DimPCDAngle* Cdim = new DimPCDAngle(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPCDAngle::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0010", __FILE__, __FUNCSIG__); }
}

int DimPCDAngle::pcdangle = 0;
void DimPCDAngle::reset()
{
	pcdangle = 0;
}

wostream& operator<<(wostream& os, DimPCDAngle& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimPCDAngle"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"ParentShape3:"<< x.ParentShape3->getId() << endl;
		os <<"EndDimPCDAngle"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPCDAngle& x)
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
			if(Tagname==L"DimPCDAngle")
			{
			while(Tagname!=L"EndDimPCDAngle")
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
						if(Tagname==L"ParentShape3")
						{
							x.ParentShape3 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPCDAngle& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape3 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPCDANG0012", __FILE__, __FUNCSIG__); }
}