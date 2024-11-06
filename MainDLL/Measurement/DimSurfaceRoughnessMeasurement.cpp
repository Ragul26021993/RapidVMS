#include "StdAfx.h"
#include "DimSurfaceRoughnessMeasurement.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Engine\PointCollection.h"
#include "..\Engine\RCadApp.h"

DimSurfaceRoughnessMeasurement::DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(Mtype))
{
}

DimSurfaceRoughnessMeasurement::DimSurfaceRoughnessMeasurement(bool simply):DimBase(false)
{
}

DimSurfaceRoughnessMeasurement::~DimSurfaceRoughnessMeasurement()
{
}

TCHAR* DimSurfaceRoughnessMeasurement::genName(RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		const TCHAR* prefix;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ)
			prefix = _T("Sq");
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA)
			prefix = _T("Sa");
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP)
			prefix = _T("Sp");
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV)
			prefix = _T("Sv");
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ)
			prefix = _T("Sz");
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK)
			prefix = _T("Ssk");
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU)
			prefix = _T("Sku");
		_tcscpy_s(name, 20, prefix);
		roughnesscnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(roughnesscnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSR0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimSurfaceRoughnessMeasurement::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawDiametere3D(&center[0], 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSR0005", __FILE__, __FUNCSIG__); }
}

void DimSurfaceRoughnessMeasurement::CalculateMeasurement(Shape* s1, bool calculate)
{
	try
	{
		this->ParentShape1 = s1;
		CloudPoints *curShape = ((CloudPoints*)this->ParentShape1);
		if(calculate)
		{
			double *points = NULL, ans[4] = {0}, dist = 0;
			int cnt = 0;
			if(curShape->PointsForMeasureCnt == 0)
			{
			   cnt = curShape->PointsListOriginal->Pointscount();
			   points = curShape->pts;
			}
			else
			{
			   cnt = curShape->PointsForMeasureCnt;
	     	   points = curShape->CloudPointsForMeasure;
			}
			BESTFITOBJECT->Plane_BestFit(points, cnt, &ans[0], MAINDllOBJECT->SigmaModeFlag());
			ans[3] = -ans[3];
			if(ans[2] < 0)	{for(int i =0; i < 4; i++){ans[i] = -ans[i];}}

			if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ)
			{
				double tmpdist = 0;
				for(int i = 0; i < cnt; i++)
				{
					tmpdist = RMATH3DOBJECT->Distance_Point_Plane(&points[3 * i], ans);
					dist += tmpdist * tmpdist;
				} 
				dist /= cnt;
				dist = sqrt(dist);
			}
			else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA)
			{
				double tmpdist = 0;
				for(int i = 0; i < cnt; i++)
				{
					tmpdist = RMATH3DOBJECT->Distance_Point_Plane(&points[3 * i], ans);
					dist += abs(tmpdist);
				} 
				dist /= cnt;
			}
			else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP)
			{
				double tmpdist = 0;
				dist = -1000;
				for(int i = 0; i < cnt; i++)
				{
					tmpdist = RMATH3DOBJECT-> Distance_Point_Plane_with_Dir(&points[3 * i], ans);
					if(dist < tmpdist) dist = tmpdist; 
				} 
			}
			else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV)
			{
				double tmpdist = 0;
				dist = 1000;
				for(int i = 0; i < cnt; i++)
				{
					tmpdist = RMATH3DOBJECT-> Distance_Point_Plane_with_Dir(&points[3 * i], ans);
					if(dist > tmpdist) dist = tmpdist; 
				} 
			}
			else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ)
			{
				dist = RMATH3DOBJECT->Flatness(points, cnt, ans);
			}	
			else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK)
			{
	    		double tmpdist = 0, tmpdist1 = 0, tmpdist2 = 0;
				for(int i = 0; i < cnt; i++)
				{
					tmpdist = RMATH3DOBJECT->Distance_Point_Plane_with_Dir(&points[3 * i], ans);
					tmpdist1 += pow(tmpdist,3); 
					tmpdist2 += tmpdist * tmpdist;
				} 
				tmpdist1 /= cnt;
				tmpdist2 /= cnt;
				tmpdist2 = sqrt(tmpdist2);
				if(tmpdist2 == 0)
					tmpdist2 = 1;
				dist = tmpdist1 / pow(tmpdist2,3);
			
			}	
			else if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU)
			{
			   double tmpdist = 0, tmpdist1 = 0, tmpdist2 = 0;
				for(int i = 0; i < cnt; i++)
				{
					tmpdist = RMATH3DOBJECT->Distance_Point_Plane_with_Dir(&points[3 * i], ans);
					tmpdist1 += pow(tmpdist,4); 
					tmpdist2 += tmpdist * tmpdist;
				} 
				tmpdist1 /= cnt;
				tmpdist2 /= cnt;
				if(tmpdist2 == 0)
					tmpdist2 = 1;
				dist = tmpdist1 / pow(tmpdist2,2);
			}
			setDimension(dist);
	    }
		RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(center, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSR0006", __FILE__, __FUNCSIG__); }
}

void DimSurfaceRoughnessMeasurement::SetCenter(double *tcenter)
{
	center[0] = tcenter[0];
	center[1] = tcenter[1];
	center[2] = tcenter[2];
}

void DimSurfaceRoughnessMeasurement::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, false);
}

void DimSurfaceRoughnessMeasurement::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	CalculateMeasurement((Shape*)sender);
}

DimBase* DimSurfaceRoughnessMeasurement::Clone(int n)
{
	try
	{
		DimSurfaceRoughnessMeasurement* Cdim = new DimSurfaceRoughnessMeasurement(this->MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSR0007", __FILE__, __FUNCSIG__); return NULL; }
}

int DimSurfaceRoughnessMeasurement::roughnesscnt = 0;

void DimSurfaceRoughnessMeasurement::reset()
{
	roughnesscnt = 0;
}

DimBase* DimSurfaceRoughnessMeasurement::CreateDummyCopy()
{
	try
	{
		DimSurfaceRoughnessMeasurement* Cdim = new DimSurfaceRoughnessMeasurement(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSR0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimSurfaceRoughnessMeasurement::CopyMeasureParameters(DimBase* Cdim)
{
}

wostream& operator<<( wostream& os, DimSurfaceRoughnessMeasurement& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimSurfaceRoughnessMeasurement"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;		
		os<<"EndDimSurfaceRoughnessMeasurement"<<endl;	
		return os;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSR0010", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, DimSurfaceRoughnessMeasurement& x)
{
	try
	{
		is >> (*(DimBase*)&x);			
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"DimSurfaceRoughnessMeasurement")
		{
			while(Tagname!=L"EndDimSurfaceRoughnessMeasurement")
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
		return is;
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("DIMSR0011", __FILE__, __FUNCSIG__); return is; }
}
