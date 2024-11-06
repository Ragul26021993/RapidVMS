#include "StdAfx.h"
#include "DimPointCoordinate.h"
#include "DimChildMeasurement.h"
#include "..\Engine\RCollectionBase.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

DimPointCoordinate::DimPointCoordinate(TCHAR* myname):DimBase(genName(myname))
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0001", __FILE__, __FUNCSIG__); }
}

DimPointCoordinate::DimPointCoordinate(bool simply):DimBase(false)
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0002", __FILE__, __FUNCSIG__); }
}

void DimPointCoordinate::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE;
		//Initialise the child measurements... To handle all the conditions.. 
		//i.e. we need all the properties for each measurements..
		std::string DimName = this->getModifiedName();
		DimPointX = AddChildMeasurement(DimName + "_X");
		DimPointY = AddChildMeasurement(DimName + "_Y");
		DimPointZ = AddChildMeasurement(DimName + "_Z");
		this->addMChild(DimPointX);
		this->addMChild(DimPointY);
		this->addMChild(DimPointZ);
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0002", __FILE__, __FUNCSIG__); }
}

DimChildMeasurement* DimPointCoordinate::AddChildMeasurement(std::string str)
{
	try
	{
		DimChildMeasurement* Cdim = new DimChildMeasurement(true);
		Cdim->MeasurementType = this->MeasurementType;
		Cdim->setModifiedName(str);
		Cdim->setOriginalName(RMATH2DOBJECT->StringToWString(str));
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0002", __FILE__, __FUNCSIG__); return NULL; }
}

DimPointCoordinate::~DimPointCoordinate()
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointCoordinate::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		centerno++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(centerno, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0004", __FILE__, __FUNCSIG__); _T("NA"); }
} 

void DimPointCoordinate::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double cpnt[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		GRAFIX->drawDiametere3D(&cpnt[0], 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0005", __FILE__, __FUNCSIG__); }
}

void DimPointCoordinate::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	UpdateMeasurement();
}

void DimPointCoordinate::CalculateMeasurement(Vector *v1)
{
	try
	{
		this->ParentPoint1 = v1;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE;
		int NoofDec = MAINDllOBJECT->MeasurementNoOfDecimal();
		bool flag = false;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			flag = true;
		std::string collection = "(" + ConvertToString(ParentPoint1->getX(), NoofDec, flag) + "," + ConvertToString(ParentPoint1->getY(), NoofDec, flag) + "," + ConvertToString(ParentPoint1->getZ(), NoofDec, flag) + ")";
		this->setDimension((char*)collection.c_str());
		DimPointX->ChildMeasureValue(ParentPoint1->getX());
		DimPointY->ChildMeasureValue(ParentPoint1->getY());
		DimPointZ->ChildMeasureValue(ParentPoint1->getZ());
		double cpnt[3] = {ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint1->getZ()};
		RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(cpnt, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0006", __FILE__, __FUNCSIG__); }
}

std::string DimPointCoordinate::ConvertToString(double d, int NoOfDec, bool inchMode)
{
	try
	{
		int NoofDec = MAINDllOBJECT->MeasurementNoOfDecimal();
		if(abs(d) < 0.00001) d = 0.0;
		if(abs(d) > 10000) return "0.0000";
		if(inchMode)
			d = d / 25.4;
		char cd[30];
		RMATH2DOBJECT->Double2String(d, NoofDec, cd); 
		return cd;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0008", __FILE__, __FUNCSIG__); }
}

void DimPointCoordinate::UpdateMeasurement()
{
	CalculateMeasurement(ParentPoint1);
}

void DimPointCoordinate::AddChildeMeasureToCollection(RCollectionBase& MColl)
{
	try
	{
		MColl.addItem(DimPointX, false); MColl.addItem(DimPointY, false); MColl.addItem(DimPointZ, false);

		DimPointX->ParentMeasureId(this->getId()); DimPointY->ParentMeasureId(this->getId()); DimPointZ->ParentMeasureId(this->getId());

		PChildMeasureIdCollection.push_back(DimPointX->getId());
		PChildMeasureIdCollection.push_back(DimPointY->getId());
		PChildMeasureIdCollection.push_back(DimPointZ->getId());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0008", __FILE__, __FUNCSIG__); }
}

DimBase* DimPointCoordinate::Clone(int n)
{
	try
	{
		DimPointCoordinate* Cdim = new DimPointCoordinate();
		Cdim->CopyOriginalProperties(this);
		Cdim->DimPointX->CopyOriginalProperties(this->DimPointX);
		Cdim->DimPointY->CopyOriginalProperties(this->DimPointY);
		Cdim->DimPointZ->CopyOriginalProperties(this->DimPointZ);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointCoordinate::CreateDummyCopy()
{
	try
	{
		DimPointCoordinate* Cdim = new DimPointCoordinate(false);
		Cdim->CopyOriginalProperties(this);
		//Temp fix to copy the parameters... Can be changed!!
		Cdim->DimPointX->setId(this->DimPointX->getId());
		Cdim->DimPointY->setId(this->DimPointY->getId());
		Cdim->DimPointZ->setId(this->DimPointZ->getId());
		Cdim->DimPointX->CopyOriginalProperties(this->DimPointX);
		Cdim->DimPointY->CopyOriginalProperties(this->DimPointY);
		Cdim->DimPointZ->CopyOriginalProperties(this->DimPointZ);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointCoordinate::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0009", __FILE__, __FUNCSIG__); }
}

int DimPointCoordinate::centerno = 0;
void DimPointCoordinate::reset()
{
	centerno = 0;
}

wostream& operator<<(wostream& os, DimPointCoordinate& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointCoordinate"<<endl;
		os <<"ParentPoint1:value"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os <<"DimChildMeasurementX:value"<<endl<<(*static_cast<DimChildMeasurement*>(x.DimPointX));
		os <<"DimChildMeasurementY:value"<<endl<<(*static_cast<DimChildMeasurement*>(x.DimPointY));
		os <<"DimChildMeasurementZ:value"<<endl<<(*static_cast<DimChildMeasurement*>(x.DimPointZ));
		os<<"EndDimPointCoordinate"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0010", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimPointCoordinate& x)
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
			if(Tagname==L"DimPointCoordinate")
			{
				while(Tagname!=L"EndDimPointCoordinate")
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
						if(Tagname==L"ParentPoint1")
						{
							x.ParentPoint1 = new Vector(0,0,0);
							is >> (*static_cast<Vector*>(x.ParentPoint1));
						}		
						else if(Tagname==L"DimChildMeasurementX")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimPointX));
						}		
						else if(Tagname==L"DimChildMeasurementY")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimPointY));
						}	
						else if(Tagname==L"DimChildMeasurementZ")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimPointZ));
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointCoordinate& x)
{
	try
	{
		x.ParentPoint1 = new Vector(0,0,0);
 		is >> (*static_cast<Vector*>(x.ParentPoint1));
		is >> (*static_cast<DimChildMeasurement*>(x.DimPointX));
		is >> (*static_cast<DimChildMeasurement*>(x.DimPointY));
		is >> (*static_cast<DimChildMeasurement*>(x.DimPointZ));
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0012", __FILE__, __FUNCSIG__); }
}
