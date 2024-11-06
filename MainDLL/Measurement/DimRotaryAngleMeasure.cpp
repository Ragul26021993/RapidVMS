#include "StdAfx.h"
#include "DimRotaryAngleMeasure.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"

DimRotaryAngleMeasure::DimRotaryAngleMeasure(TCHAR* myname):DimBase(genName(myname))
{
	this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_ROTARYANGLEMEASURE;
	this->DistanceMeasurement(false);
}

DimRotaryAngleMeasure::DimRotaryAngleMeasure(bool simply):DimBase(false)
{
	this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_ROTARYANGLEMEASURE;
	this->DistanceMeasurement(false);
}

DimRotaryAngleMeasure::~DimRotaryAngleMeasure()
{
}

TCHAR* DimRotaryAngleMeasure::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		Rotaryanglecnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(Rotaryanglecnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0001", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimRotaryAngleMeasure::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawFiniteline_FinitelineDistance(&Endpoints[0], &Endpoints[2], &Endpoints[4], &Endpoints[6], angle1, intercept1, angle2, intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, ((Line*)ParentShape1)->getPoint1()->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0002", __FILE__, __FUNCSIG__); }
}

void DimRotaryAngleMeasure::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0003", __FILE__, __FUNCSIG__); }
}

void DimRotaryAngleMeasure::CalculateMeasurement(Shape *s1, Shape *s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		//double Tempangle, Rangle1 = 0, Rangle2 = 0, Dist1 = 0, Dist2 = 0;
		double Tempangle, Rangle1 = 0, Rangle2 = 0;
		/*if(((ShapeWithList*)ParentShape1)->PointAtionList.size() > 0)
		{
			AddPointAction* Cation = (AddPointAction*)(*((ShapeWithList*)ParentShape1)->PointAtionList.begin());
			Rangle1 = Cation->getFramegrab()->PointDRO.getR();
		}
		if(((ShapeWithList*)ParentShape2)->PointAtionList.size() > 0)
		{
			AddPointAction* Cation = (AddPointAction*)(*((ShapeWithList*)ParentShape2)->PointAtionList.begin());
			Rangle2 = Cation->getFramegrab()->PointDRO.getR();
		}*/
		Rangle1 = ((ShapeWithList*)ParentShape1)->RAxisVal;
		Rangle2 = ((ShapeWithList*)ParentShape2)->RAxisVal;

		angle1 = ((Line*)ParentShape1)->Angle();
		intercept1 = ((Line*)ParentShape1)->Intercept();
		angle2 = ((Line*)ParentShape2)->Angle();
		intercept2 = ((Line*)ParentShape2)->Intercept();
		//Dist1 = ((Line*)ParentShape1)->getMidPoint()->getY();
		//Dist2 = ((Line*)ParentShape2)->getMidPoint()->getY();
		/*double Point1[2] = {MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY()};
		Dist3 = RMATH2DOBJECT->Pt2Line_DistSigned(Point1[0], Point1[1], angle1, intercept1);
		Dist4 = RMATH2DOBJECT->Pt2Line_DistSigned(Point1[0], Point1[1], angle2, intercept2);*/
		//Implement the calibration according to the dist...
		//Dist1 = Dist1 * 0.0185 * M_PI/180; // 0.02 degree per um...
		//Dist2 = Dist2 * 0.0185 * M_PI/180;

		//Dist3 = Dist3 * 0.0185 * M_PI/180; // 0.02 degree per um...
		//Dist4 = Dist4 * 0.0185 * M_PI/180;

		//Rangle1 += Dist1;
		//Rangle2 += Dist2;

		Tempangle = abs(abs(Rangle1 - Rangle2) - MAINDllOBJECT->RotaryFixtureOffset());

		//Tempangle += MAINDllOBJECT->RotaryCalibrationValue();
		this->DistanceMeasurement(false);
		this->setAngularDimension(Tempangle);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0004", __FILE__, __FUNCSIG__); }
}

void DimRotaryAngleMeasure::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
} 

DimBase* DimRotaryAngleMeasure::Clone(int n)
{
	try
	{
		DimRotaryAngleMeasure* Cdim = new DimRotaryAngleMeasure();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0005", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimRotaryAngleMeasure::CreateDummyCopy()
{
	try
	{
		DimRotaryAngleMeasure* Cdim = new DimRotaryAngleMeasure(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0006", __FILE__, __FUNCSIG__); return NULL; }
}

void DimRotaryAngleMeasure::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0007", __FILE__, __FUNCSIG__); }
}

int DimRotaryAngleMeasure::Rotaryanglecnt = 0;
void DimRotaryAngleMeasure::reset()
{
	Rotaryanglecnt = 0;
}

wostream& operator<<(wostream& os, DimRotaryAngleMeasure& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<< "DimRotaryAngleMeasure"<<endl;
		os << "ParentShape1:"<< x.ParentShape1->getId() << endl;
		os << "ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<< "EndDimRotaryAngleMeasure"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimRotaryAngleMeasure& x)
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
			if(Tagname==L"DimRotaryAngleMeasure")
			{
			while(Tagname!=L"EndDimRotaryAngleMeasure")
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
						else if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0009", __FILE__, __FUNCSIG__); return is; }
}

void ReadOldPP(wistream& is, DimRotaryAngleMeasure& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 = (Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMROTANG0009", __FILE__, __FUNCSIG__); }
}