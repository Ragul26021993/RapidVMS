#include "StdAfx.h"
#include "DimLine.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLine::DimLine(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype):DimBase(genName(myname, Mtype))
{
	try{this->CurrentStatus = true;}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0001", __FILE__, __FUNCSIG__); }
}

DimLine::DimLine(bool simply):DimBase(false)
{
	try{this->CurrentStatus = true;}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0002", __FILE__, __FUNCSIG__); }
}

DimLine::~DimLine()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimLine::genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		int dimcnt;
		if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH)
			dimcnt = distno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS || Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED)
			dimcnt = straightnessno++;
		else if(Mtype == RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE)
			dimcnt = angleno++;
		dimcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(dimcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimLine::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		Line* line = (Line*)ParentShape1;
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH:
				if(LinearMeasurementMode())
					GRAFIX->drawPoint_PointDistance_Linear(line->getPoint1()->getX(), line->getPoint1()->getY(), line->getPoint2()->getX(), line->getPoint2()->getY(), CurrentStatus, mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, line->getPoint1()->getZ());
				else
					GRAFIX->drawPoint_PointDistance(line->getPoint1()->getX(), line->getPoint1()->getY(), line->getPoint2()->getX(), line->getPoint2()->getY(), mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, line->getPoint1()->getZ());
			break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS:
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED:
				{
				double cpnt[3] = {(line->getPoint1()->getX() + line->getPoint2()->getX()) / 2, (line->getPoint1()->getY() + line->getPoint2()->getY()) / 2, (line->getPoint1()->getZ() + line->getPoint2()->getZ()) / 2};
	         	GRAFIX->drawDiametere3D(&cpnt[0], 0, getCDimension(), getModifiedName(), &DimSelectionLine[0], TransformationMatrix, MeasurementPlane, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
			//	GRAFIX->drawPoint_PointDistance(line->getPoint1()->getX(), line->getPoint1()->getY(), line->getPoint2()->getX(), line->getPoint2()->getY(), mposition.getX(), mposition.getY(), line->Length(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, line->getPoint1()->getZ());
			    break;
				}
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE:
				GRAFIX->drawSlope(line->Angle(), line->Intercept(), mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, line->getPoint1()->getZ());	
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0005", __FILE__, __FUNCSIG__); }
}

void DimLine::CalculateMeasurement(Line* line)
{
	try
	{
		this->ParentShape1 = line;
		double t1[2] = {line->getPoint1()->getX(), line->getPoint1()->getY()},
					t2[2] = {line->getPoint2()->getX(), line->getPoint2()->getY()},
					t3[2] = {mposition.getX(), mposition.getY()};
		double slope = line->Angle(), intercept = line->Intercept(), angle;
		this->DistanceMeasurement(true);
		bool NextStatus;
		switch(MeasurementType)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH:
				if(LinearMeasurementMode())
				{
					angle = RMATH2DOBJECT->LPt2Pt_distance(&t1[0], &t2[0], CurrentStatus, &NextStatus, &t3[0]);
					CurrentStatus = NextStatus;
					this->setDimension(angle);
				}
				else
					this->setDimension(line->Length());
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS:
				{
				double cpnt[3] = {(line->getPoint1()->getX() + line->getPoint2()->getX()) / 2, (line->getPoint1()->getY() + line->getPoint2()->getY()) / 2, (line->getPoint1()->getZ() + line->getPoint2()->getZ()) / 2};
	            RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(cpnt, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
				this->setDimension(line->Straightness());
				break;
				}
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED:
				{
				double cpnt[3] = {(line->getPoint1()->getX() + line->getPoint2()->getX()) / 2, (line->getPoint1()->getY() + line->getPoint2()->getY()) / 2, (line->getPoint1()->getZ() + line->getPoint2()->getZ()) / 2};
	            RMATH3DOBJECT->TransformationMatrix_Dia_Sphere(cpnt, DimSelectionLine, &TransformationMatrix[0], &MeasurementPlane[0]);
				this->setDimension(line->FilteredStraightness());
				break;
				}
			case RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE:
				angle = RMATH2DOBJECT->Line_slopemousept(slope, intercept, &t3[0]);
				this->DistanceMeasurement(false);
				this->setAngularDimension(angle);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0006", __FILE__, __FUNCSIG__); }
}

void DimLine::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		CalculateMeasurement((Line*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0007", __FILE__, __FUNCSIG__); }
}

void DimLine::UpdateMeasurement()
{
	CalculateMeasurement((Line*)ParentShape1);
}

DimBase* DimLine::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH)
			myname = _T("Dist");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS || this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED)
			myname = _T("St");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE)
			myname = _T("Ang");
		DimLine* Cdim = new DimLine((TCHAR*)myname.c_str(), MeasurementType);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLine::CreateDummyCopy()
{
	try
	{
		DimLine* Cdim = new DimLine(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLine::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0010", __FILE__, __FUNCSIG__); }
}

int DimLine::straightnessno = 0;
void DimLine::reset()
{
	straightnessno = 0;
}

wostream& operator<<(wostream& os, DimLine& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimLine"<<endl;
		os << "CurrentStatus:"<< x.CurrentStatus << endl;
		os << "ParentShape1Id:"<< x.ParentShape1->getId() << endl;
		os<<"EndDimLine"<<endl;
		/*os << x.CurrentStatus << endl;
		os << x.ParentShape1->getId() << endl;*/
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0011", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, DimLine& x)
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
			if(Tagname==L"DimLine")
			{
				while(Tagname!=L"EndDimLine")
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
						if(Tagname==L"CurrentStatus")
						{
							if(Val=="0")							
								x.CurrentStatus=false;
							else
								x.CurrentStatus=true;
						}
						else if(Tagname==L"ParentShape1Id")
						{				 
							x.ParentShape1 =(Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLine& x)
{
	try
	{
		int n;
		is >> x.CurrentStatus;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE0012", __FILE__, __FUNCSIG__); }
}