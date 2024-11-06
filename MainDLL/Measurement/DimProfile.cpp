#include "StdAfx.h"
#include "DimProfile.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\RPoint.h"

DimProfile::DimProfile(TCHAR* myname) :DimBase(genName(myname))
{
	try{}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0001", __FILE__, __FUNCSIG__); }
}

DimProfile::DimProfile(bool simply) :DimBase(false)
{
	try{}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0002", __FILE__, __FUNCSIG__); }
}

DimProfile::~DimProfile()
{
	try{}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimProfile::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		ProfileCnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(ProfileCnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0004", __FILE__, __FUNCSIG__); return _T("NA"); }
}

void DimProfile::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double cen[3] = { ((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(), ((Circle*)ParentShape1)->getCenter()->getZ() };
		GRAFIX->drawProfile(mposition.getX(), mposition.getY(), ((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(), ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), &ParentShape2->DatumName, getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);
	}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0005", __FILE__, __FUNCSIG__); }
}


void DimProfile::CalculateMeasurement(Shape* s1, Shape* s2, Shape* s3, Shape* s4, Shape* refShape)
{
	try
	{

		this->ParentShape1 = s1; this->ParentShape2 = s2;
		this->ParentShape3 = s3; this->ParentShape4 = s4;

		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_PROFILE;
		double cen1[2] = { ((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY() };
		double cen2[2] = { 0 };
		//Check the type of shape for which we want to calculate profile...
		if (ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		{
			//We have an arc or circle. We need to add all the points taken for the shape to a new circle/arc with the ideal parameters of Reference Shape
			//And then calculate the circularity of that. 
			Circle* tempCircle = new Circle(_T("tempp1"), RapidEnums::SHAPETYPE::CIRCLE);
			tempCircle->CircleType = RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER;
			Vector* idealCentre = ((Circle*)refShape)->getCenter();
			((ShapeWithList*)tempCircle)->PointsList->Addpoint(new SinglePoint(idealCentre->getX(), idealCentre->getY(), idealCentre->getZ(), idealCentre->getR()));
			tempCircle->IsHidden(true);
			tempCircle->AddPoints(((ShapeWithList*)s1)->PointsList);
			setDimension(tempCircle->Circularity());
		}
		else if (ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::XLINE || ParentShape1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
		{
			//We have Line. We need to add all the points taken for the shape to a new Line with the ideal parameters of Reference Shape
			//And then calculate the Straightness of that. 
			double* ptsofthisLine = (double*)malloc(((ShapeWithList*)s1)->PointsList->Pointscount() * sizeof(double) * 2);
			int n = 0;
			for (PC_ITER Spt = ((ShapeWithList*)s1)->PointsList->getList().begin(); Spt != ((ShapeWithList*)s1)->PointsList->getList().end(); Spt++)
			{
				if (((*Spt).second)->InValid) continue;
				SinglePoint* Pt = (*Spt).second;
				ptsofthisLine[n++] = Pt->X;
				ptsofthisLine[n++] = Pt->Y;
			}
			setDimension(RMATH2DOBJECT->StraightNessofLine(n / 2, ptsofthisLine, ((Line*)ReferenceShape)->Angle(), ((Line*)ReferenceShape)->Intercept()));
		}
	}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0006", __FILE__, __FUNCSIG__); }
}

void DimProfile::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2, ParentShape3, ParentShape4, ReferenceShape);
}

void DimProfile::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if (this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if (sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement((Shape*)sender, ParentShape2, ParentShape3, ParentShape4, ReferenceShape);
		else if (sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentShape1, (Shape*)sender, ParentShape3, ParentShape4, ReferenceShape);
	}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0007", __FILE__, __FUNCSIG__); }
}

DimBase* DimProfile::Clone(int n)
{
	try
	{
		DimProfile* Cdim = new DimProfile(_T("Pf"));
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimProfile::CreateDummyCopy()
{
	try
	{
		DimProfile* Cdim = new DimProfile(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimProfile::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0010", __FILE__, __FUNCSIG__); }
}

int DimProfile::ProfileCnt = 0;
void DimProfile::reset()
{
	ProfileCnt = 0;
}

wostream& operator<<(wostream& os, DimProfile& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os << "DimProfile" << endl;
		os << "ParentShape1:" << x.ParentShape1->getId() << endl;
		os << "ParentShape2:" << x.ParentShape2->getId() << endl;
		os << "EndDimProfile" << endl;
		return os;
	}
	catch (...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DimProfile& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		if (MAINDllOBJECT->IsOldPPFormat())
		{
			ReadOldPP(is, x);
		}
		else
		{
			std::wstring Tagname;
			is >> Tagname;
			if (Tagname == L"DimProfile")
			{
				while (Tagname != L"EndDimProfile")
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
						if (Tagname == L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}
						else if (Tagname == L"ParentShape2")
						{
							x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch (...){ MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimProfile& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
		x.UpdateMeasurement();
	}
	catch (...){ MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0012", __FILE__, __FUNCSIG__); }
}