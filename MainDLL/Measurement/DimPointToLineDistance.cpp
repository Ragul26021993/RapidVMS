#include "StdAfx.h"
#include "DimPointToLineDistance.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

DimPointToLineDistance::DimPointToLineDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0001", __FILE__, __FUNCSIG__); }
}

DimPointToLineDistance::DimPointToLineDistance(bool simply):DimBase(false)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0002", __FILE__, __FUNCSIG__); }
}

DimPointToLineDistance::~DimPointToLineDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToLineDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToLineDistance::CalculateMeasurement(Shape *s1, Vector *pp1)
{
	try
	{
		if (pp1 == nullptr) pp1 = new Vector(0, 0, 0);

		this->ParentPoint1 = pp1;
		this->ParentShape1 = s1;
		double p[2] = {ParentPoint1->getX(), ParentPoint1->getY()};
		double slope = ((Line*)ParentShape1)->Angle(), intercept = ((Line*)ParentShape1)->Intercept(), dis;
		disttype = Distancetype((int)ParentShape1->ShapeType);
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE;
		dis = RMATH2DOBJECT->Pt2Line_Dist(p[0], p[1], slope, intercept);
		this->setDimension(dis);
	}
	catch(std::exception &ex)
	{
		std::string tempString = __FUNCSIG__;
		tempString.append(ex.what());
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0005", __FILE__, tempString);
	}
}

void DimPointToLineDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{			
	try
	{
		Line* line = (Line*)ParentShape1;
		double slope = line->Angle(), intercept = line->Intercept();
		double point[4], Zlevel = line->getPoint1()->getZ();
		switch(disttype)
		{
			case Distancetype::PT_LINE:
				point[0] = line->getPoint1()->getX(); point[1] = line->getPoint1()->getY(); point[2] = line->getPoint2()->getX(); point[3] = line->getPoint2()->getY();
				GRAFIX->drawPoint_FinitelineDistance(ParentPoint1->getX(), ParentPoint1->getY(), slope, intercept, &point[0], &point[2], mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
				break;
			case Distancetype::PT_XRAY:
				point[0] = line->getPoint1()->getX(); point[1] = line->getPoint1()->getY();
				GRAFIX->drawPoint_RayDistance(ParentPoint1->getX(), ParentPoint1->getY(), slope, intercept, &point[0], mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
				break;
			case Distancetype::PT_XLINE:
				GRAFIX->drawPoint_InfinitelineDistance(ParentPoint1->getX(), ParentPoint1->getY(), slope, intercept, mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
				break;
		}
	}
	catch(std::exception &ex)
	{
		std::string tempString = __FUNCSIG__;
		tempString.append(ex.what());
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0006", __FILE__, tempString);
	}
		
		/*...)
	{ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0006", __FILE__, __FUNCSIG__); }*/
}

void DimPointToLineDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			ParentShape1 = (Shape*)sender;
		CalculateMeasurement(ParentShape1, ParentPoint1);
	}
	catch(std::exception &ex)
	{
		std::string tempString = __FUNCSIG__;
		tempString.append(ex.what());
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0007", __FILE__, tempString);
	}
		/*...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0007", __FILE__, __FUNCSIG__); }*/
}

void DimPointToLineDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentPoint1);	
}

DimBase* DimPointToLineDistance::Clone(int n)
{
	try
	{
		DimPointToLineDistance* Cdim = new DimPointToLineDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0008", __FILE__, __FUNCSIG__); return NULL;}
}

DimBase* DimPointToLineDistance::CreateDummyCopy()
{
	try
	{
		DimPointToLineDistance* Cdim = new DimPointToLineDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0009", __FILE__, __FUNCSIG__); }
}

void DimPointToLineDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0010", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToLineDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToLineDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		if (x.ParentPoint1 != nullptr)
			os <<"ParentPoint1:values"<<endl<<(*static_cast<Vector*>(x.ParentPoint1)) << endl;
		else
		{
			Vector* vv = new Vector(0, 0, 0);
			os << "ParentPoint1:values" << endl << (*static_cast<Vector*>(vv)) << endl;
		}
	}
	catch (std::exception &ex)
	{
		std::string tempString = __FUNCSIG__;
		tempString.append(ex.what());
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0011", __FILE__, tempString);
	}
	os << "EndDimPointToLineDistance" << endl;
	return os;

	//catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToLineDistance& x)
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
			if (Tagname == L"DimPointToLineDistance")
			{
				while (Tagname != L"EndDimPointToLineDistance")
				{
					std::wstring Linestr;
					is >> Linestr;
					if (is.eof())
					{
						MAINDllOBJECT->PPLoadSuccessful(false);
						break;
					}
					int ColonIndx = Linestr.find(':');
					if (ColonIndx == -1)
					{
						Tagname = Linestr;
					}
					else
					{
						std::wstring Tag = Linestr.substr(0, ColonIndx);
						std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname = Tag;
						if (Tagname == L"ParentPoint1")
						{
							x.ParentPoint1 = new Vector(0, 0, 0);
							try
							{
								is >> (*static_cast<Vector*>(x.ParentPoint1));
							}
							catch (...)
							{
								MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0012a", __FILE__, __FUNCSIG__);
							}
						}
						else if (Tagname == L"ParentShape1")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToLineDistance& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.ParentPoint1 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint1));
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2LINE0012", __FILE__, __FUNCSIG__);  }
}