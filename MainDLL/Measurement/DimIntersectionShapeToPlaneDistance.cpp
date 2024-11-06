#include "StdAfx.h"
#include "DimIntersectionShapeToPlaneDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\IntersectionShape.h"
#include "..\Engine\RCadApp.h"

DimIntersectionShapeToPlaneDistance::DimIntersectionShapeToPlaneDistance(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0001", __FILE__, __FUNCSIG__); }
}

DimIntersectionShapeToPlaneDistance::DimIntersectionShapeToPlaneDistance(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0002", __FILE__, __FUNCSIG__); }
}

DimIntersectionShapeToPlaneDistance::~DimIntersectionShapeToPlaneDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0003", __FILE__, __FUNCSIG__); }
}

void DimIntersectionShapeToPlaneDistance::init()
{
	try
	{
		for(int i = 0; i < 3; i++) {PointToUse[i] = 0;}
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimIntersectionShapeToPlaneDistance::genName(const TCHAR* prefix)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0005", __FILE__, __FUNCSIG__); return _T("NA");}
} 

void DimIntersectionShapeToPlaneDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->GRAFIX->drawPoint_PlaneDistance3D(&PointToUse[0], &PlaneParam[0], &PlaneEndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0006", __FILE__, __FUNCSIG__); }
}

void DimIntersectionShapeToPlaneDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONSHAPETOPLANEDISTANCE;
		this->DistanceMeasurement(true);

		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::INTERSECTIONSHAPE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		double PlnPts[6] = {0};
		((Plane*)ParentShape2)->getParameters(&PlaneParam[0]);
		((Plane*)ParentShape2)->getEndPoints(&PlaneEndPts[0]);

		((Plane*)ParentShape2)->getPoint1()->FillDoublePointer(&PlnPts[0], 3);
		((Plane*)ParentShape2)->getPoint3()->FillDoublePointer(&PlnPts[3], 3);
		double mid[3] = {(PlnPts[0] + PlnPts[3])/2, (PlnPts[1] + PlnPts[4])/2, (PlnPts[2] + PlnPts[5])/2};

		IntersectionShape *intsectShape = (IntersectionShape *)ParentShape1;
		if(NormalmeasureType() == 0)
		{
			RMATH3DOBJECT->Nearest_Point_to_Plane(intsectShape->intersectPts, intsectShape->pntscount, PlaneParam, PointToUse);
		}
		else if(NormalmeasureType() == 1)
		{
	     	RMATH3DOBJECT->Farthest_Point_to_Plane(intsectShape->intersectPts, intsectShape->pntscount, PlaneParam, PointToUse);
		}
		double Dist = RMATH3DOBJECT->Distance_Point_Plane(PointToUse, PlaneParam);
		setDimension(Dist);
		RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Pln(PointToUse, PlaneParam, mid, &TransformationMatrix[0], &MeasurementPlane[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0007", __FILE__, __FUNCSIG__); }
}

void DimIntersectionShapeToPlaneDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			CalculateMeasurement((Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			CalculateMeasurement(ParentShape1, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0008", __FILE__, __FUNCSIG__); }
}

void DimIntersectionShapeToPlaneDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimIntersectionShapeToPlaneDistance::Clone(int n)
{
	try
	{
		DimIntersectionShapeToPlaneDistance* Cdim = new DimIntersectionShapeToPlaneDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimIntersectionShapeToPlaneDistance::CreateDummyCopy()
{
	try
	{
		DimIntersectionShapeToPlaneDistance* Cdim = new DimIntersectionShapeToPlaneDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimIntersectionShapeToPlaneDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimIntersectionShapeToPlaneDistance& x)
{
	try
	{
	
		os << (*static_cast<DimBase*>(&x));
		os<<"DimIntersectionShapeToPlaneDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"PointToUse0:"<< x.PointToUse[0] << endl;
        os <<"PointToUse1:"<< x.PointToUse[1] << endl;
		os <<"PointToUse2:"<< x.PointToUse[2] << endl;
		os<<"EndDimIntersectionShapeToPlaneDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimIntersectionShapeToPlaneDistance& x)
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
			if(Tagname==L"DimIntersectionShapeToPlaneDistance")
			{
				while(Tagname!=L"EndDimIntersectionShapeToPlaneDistance")
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
						else if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}	
						else if(Tagname==L"PointToUse0")
						{
							x.PointToUse[0]=atof((const char*)(Val).c_str());
						}	
						else if(Tagname==L"PointToUse1")
						{
							x.PointToUse[1]=atof((const char*)(Val).c_str());
						}	
						else if(Tagname==L"PointToUse2")
						{
							x.PointToUse[2]=atof((const char*)(Val).c_str());
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__); return is;}
}
void ReadOldPP(wistream& is, DimIntersectionShapeToPlaneDistance& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> x.PointToUse[0];
		is >> x.PointToUse[1];
		is >> x.PointToUse[2];
		x.UpdateMeasurement();		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__); }
}