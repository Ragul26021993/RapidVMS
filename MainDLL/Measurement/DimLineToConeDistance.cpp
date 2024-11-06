#include "StdAfx.h"
#include "DimLineToConeDistance.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"

DimLineToConeDistance::DimLineToConeDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0002", __FILE__, __FUNCSIG__); }
}

DimLineToConeDistance::~DimLineToConeDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0003", __FILE__, __FUNCSIG__); }
}

void DimLineToConeDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
		LinearmeasureType(0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0004", __FILE__, __FUNCSIG__); }
}

void DimLineToConeDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(LineConeMtype)
	{
		case LINECONEMEASURETYPE ::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case LINECONEMEASURETYPE::DISTANCEONLY:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
		case LINECONEMEASURETYPE::DISTANCEANDANGLE:
		{
			if(NormalmeasureType() == 0)
			{
				myname = "Ang";
				angleno++;
				count++;
				myname = myname + itoa(angleno, temp, 10);
				break;
			}
			else
			{
				myname = "Dist";
				distno++;
				count++;
				myname = myname + itoa(distno, temp, 10);
				break;
			}
			break;	
		}
	}
	 setModifiedName(myname);
}

void DimLineToConeDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double Dist, LnMidPt[3];
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOCONEDISTANCE;
		this->DistanceMeasurement(true);
		if(ParentShape2->ShapeType != RapidEnums::SHAPETYPE::CONE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		((Line*)ParentShape1)->getParameters(&LineParam[0]);
		((Line*)ParentShape1)->getEndPoints(&LineEndPoints[0]);
		((Cone*)ParentShape2)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape2)->getEndPoints(&ConeEndPoints[0]);
		for(int i = 0; i < 3; i++) LnMidPt[i] = (LineEndPoints[i] + LineEndPoints[3 + i]) / 2;
				
		if(RMATH3DOBJECT->Checking_Intersection_Line_Line(&LineParam[0], &ConeParam[0]))
		{
			LineConeMtype = LINECONEMEASURETYPE::ANGLEONLY;
			TotalNormalmeasureType(1);
		}
		else if(RMATH3DOBJECT->Checking_Parallel_Line_to_Line(&LineParam[0], &ConeParam[0]))
		{
			LineConeMtype = LINECONEMEASURETYPE::DISTANCEONLY;
			TotalNormalmeasureType(1);
		}
		else
		{
			LineConeMtype = LINECONEMEASURETYPE::DISTANCEANDANGLE;
			TotalNormalmeasureType(2);
		}
		switch(LineConeMtype)
		{
			case LINECONEMEASURETYPE::ANGLEONLY:
				this->DistanceMeasurement(false);
				RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&LineParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				Dist = RMATH3DOBJECT->Angle_Line_Line(&LineParam[0], &ConeParam[0], MeasurementPlane, DimSelectionLine, LnMidPt);
				this->setAngularDimension(Dist);
				break;
			case LINECONEMEASURETYPE::DISTANCEONLY:
				 Dist = RMATH3DOBJECT->Distance_Line_Cone(&LineParam[0], &ConeParam[0]);
				 RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&LineParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				 setDimension(Dist);
				break;
			case LINECONEMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
				{
					RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&LineParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					Dist = RMATH3DOBJECT->Angle_Line_Line(&LineParam[0], &ConeParam[0], MeasurementPlane, DimSelectionLine, LnMidPt);
					this->DistanceMeasurement(false);
					this->setAngularDimension(Dist);
				}
				else
				{
					Dist = RMATH3DOBJECT->Distance_Line_Cone(&LineParam[0], &ConeParam[0]);
					RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&LineParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					setDimension(Dist);
				}
				break;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0006", __FILE__, __FUNCSIG__); }
}

void DimLineToConeDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(LineConeMtype)
		{
			case LINECONEMEASURETYPE::ANGLEONLY:
				GRAFIX->drawFiniteline_CylinderAngle3D(&LineParam[0], &LineEndPoints[0], &ConeParam[0], &ConeEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case LINECONEMEASURETYPE::DISTANCEONLY:
				GRAFIX->drawFiniteline_CylinderDistance3D(&LineParam[0], &LineEndPoints[0], &ConeParam[0], &ConeEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case LINECONEMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
					GRAFIX->drawFiniteline_CylinderAngle3D(&LineParam[0], &LineEndPoints[0], &ConeParam[0], &ConeEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				else
					GRAFIX->drawFiniteline_CylinderDistance3D(&LineParam[0], &LineEndPoints[0], &ConeParam[0], &ConeEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType() - 1, &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0007", __FILE__, __FUNCSIG__); }
}

void DimLineToConeDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0008", __FILE__, __FUNCSIG__); }
}

void DimLineToConeDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLineToConeDistance::Clone(int n)
{
	try
	{
		 DimLineToConeDistance* Cdim = new  DimLineToConeDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase*  DimLineToConeDistance::CreateDummyCopy()
{
	try
	{
		DimLineToConeDistance* Cdim = new DimLineToConeDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void  DimLineToConeDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimLineToConeDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToConeDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToConeDistance"<<endl;
		return os;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToConeDistance& x)
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
			if(Tagname==L"DimLineToConeDistance")
			{
			while(Tagname!=L"EndDimLineToConeDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLineToConeDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0013", __FILE__, __FUNCSIG__); }
}