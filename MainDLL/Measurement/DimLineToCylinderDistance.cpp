#include "StdAfx.h"
#include "DimLineToCylinderDistance.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"

DimLineToCylinderDistance::DimLineToCylinderDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0002", __FILE__, __FUNCSIG__); }
}

DimLineToCylinderDistance::~DimLineToCylinderDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0003", __FILE__, __FUNCSIG__); }
}

void DimLineToCylinderDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
		LinearmeasureType(0);
		TotalLinearmeasureType(6);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0004", __FILE__, __FUNCSIG__); }
}

void DimLineToCylinderDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(LineCylinderMtype)
	{
		case LINECYLINDERMEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case LINECYLINDERMEASURETYPE::DISTANCEONLY:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
		case LINECYLINDERMEASURETYPE::DISTANCEANDANGLE:
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

void DimLineToCylinderDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double Dist, LnMidPt[3];
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOCYLINDERDISTANCE;
		this->DistanceMeasurement(true);
		if(ParentShape2->ShapeType != RapidEnums::SHAPETYPE::CYLINDER)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		((Line*)ParentShape1)->getParameters(&LineParam[0]);
		((Line*)ParentShape1)->getEndPoints(&LineEndPoints[0]);
		((Cylinder*)ParentShape2)->getParameters(&CylinderParam[0]);
		((Cylinder*)ParentShape2)->getEndPoints(&CylinderEndPoints[0]);
		for(int i = 0; i < 3; i++) LnMidPt[i] = (LineEndPoints[i] + LineEndPoints[3 + i]) / 2;
				
		if(RMATH3DOBJECT->Checking_Intersection_Line_Line(&LineParam[0], &CylinderParam[0]))
		{
			LineCylinderMtype = LINECYLINDERMEASURETYPE::ANGLEONLY;
			TotalNormalmeasureType(1);
		}
		else if(RMATH3DOBJECT->Checking_Parallel_Line_to_Line(&LineParam[0], &CylinderParam[0]))
		{
			LineCylinderMtype = LINECYLINDERMEASURETYPE::DISTANCEONLY;
			TotalNormalmeasureType(3);
		}
		else
		{
			LineCylinderMtype = LINECYLINDERMEASURETYPE::DISTANCEANDANGLE;
			TotalNormalmeasureType(4);
		}
		switch(LineCylinderMtype)
		{
			case LINECYLINDERMEASURETYPE::ANGLEONLY:
				this->DistanceMeasurement(false);
				RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&LineParam[0], &CylinderParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				Dist = RMATH3DOBJECT->Angle_Line_Line(&LineParam[0], &CylinderParam[0], MeasurementPlane, DimSelectionLine, LnMidPt);
				this->setAngularDimension(Dist);
				break;
			case LINECYLINDERMEASURETYPE::DISTANCEONLY:
				 Dist = RMATH3DOBJECT->Distance_Line_Cylinder(&LineParam[0], &CylinderParam[0], NormalmeasureType());
				 RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&LineParam[0], &CylinderParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				 setDimension(Dist);
				break;
			case LINECYLINDERMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
				{
					this->DistanceMeasurement(false);
					RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&LineParam[0], &CylinderParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					Dist = RMATH3DOBJECT->Angle_Line_Line(&LineParam[0], &CylinderParam[0], MeasurementPlane, DimSelectionLine, LnMidPt);
					this->setAngularDimension(Dist);
				}
				else
				{
					Dist = RMATH3DOBJECT->Distance_Line_Cylinder(&LineParam[0], &CylinderParam[0], NormalmeasureType() - 1);
					RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&LineParam[0], &CylinderParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
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

void DimLineToCylinderDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(LineCylinderMtype)
		{
			case LINECYLINDERMEASURETYPE::ANGLEONLY:
				GRAFIX->drawFiniteline_CylinderAngle3D(&LineParam[0], &LineEndPoints[0], &CylinderParam[0], &CylinderEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case LINECYLINDERMEASURETYPE::DISTANCEONLY:
				GRAFIX->drawFiniteline_CylinderDistance3D(&LineParam[0], &LineEndPoints[0], &CylinderParam[0], &CylinderEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case LINECYLINDERMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
					GRAFIX->drawFiniteline_CylinderAngle3D(&LineParam[0], &LineEndPoints[0], &CylinderParam[0], &CylinderEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				else
					GRAFIX->drawFiniteline_CylinderDistance3D(&LineParam[0], &LineEndPoints[0], &CylinderParam[0], &CylinderEndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType() - 1, &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0007", __FILE__, __FUNCSIG__); }
}

void DimLineToCylinderDistance::UpdateForParentChange(BaseItem* sender)
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

void DimLineToCylinderDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLineToCylinderDistance::Clone(int n)
{
	try
	{
		DimLineToCylinderDistance* Cdim = new DimLineToCylinderDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLineToCylinderDistance::CreateDummyCopy()
{
	try
	{
		DimLineToCylinderDistance* Cdim = new DimLineToCylinderDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLineToCylinderDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimLineToCylinderDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToCylinderDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToCylinderDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToCylinderDistance& x)
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
			if(Tagname==L"DimLineToCylinderDistance")
			{
				while(Tagname!=L"EndDimLineToCylinderDistance")
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

void ReadOldPP(wistream& is, DimLineToCylinderDistance& x)
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