#include "StdAfx.h"
#include "DimCylinderToConics3DDistance.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Conics3D.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"

DimCylinderToConics3DDistance::DimCylinderToConics3DDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0002", __FILE__, __FUNCSIG__); }
}

DimCylinderToConics3DDistance::~DimCylinderToConics3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0003", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConics3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0004", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConics3DDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Cylinder_ConicsMType)
	{
		case CYLINDER_CONICSMEASURETYPE::ANGLE:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CYLINDER_CONICSMEASURETYPE::DISTANCE:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
	}
	 setModifiedName(myname);
}

void DimCylinderToConics3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(Cylinder_ConicsMType)
		{
			case CYLINDER_CONICSMEASURETYPE::ANGLE:
		    GRAFIX->drawFiniteline_Conics3DDistOrAng3D(&CylinderParam[0], &CylinderEndPts[0], Conics3DParam, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], false, &DimSelectionLine[0], ProjectLine, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		    break;

			case CYLINDER_CONICSMEASURETYPE::DISTANCE:
			{
				double CyLine[6] = {0}, CyLnPts[6] = {0};
				RMATH3DOBJECT->MeasureModeCalc_Plane_Cylinder(Conics3DParam, CylinderParam, CylinderEndPts, NormalmeasureType(), CyLine, CyLnPts);
				GRAFIX->drawFiniteline_Conics3DDistOrAng3D(CyLine, CyLnPts, Conics3DParam, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], true, &DimSelectionLine[0], ProjectLine, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0006", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConics3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCONICS3DDISTANCE;
		this->DistanceMeasurement(true);

		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ELLIPSE3D)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		 
		((Conics3D*)ParentShape1)->getParameters(&Conics3DParam[0]);
		((Cylinder*)ParentShape2)->getParameters(&CylinderParam[0]);
		((Cylinder*)ParentShape2)->getEndPoints(&CylinderEndPts[0]);

		double mid[3] = {Conics3DParam[4], Conics3DParam[5], Conics3DParam[6]};

		if(RMATH3DOBJECT->Checking_Parallel_Line_to_Plane(&CylinderParam[0], &Conics3DParam[0]))
		{
			Cylinder_ConicsMType = CYLINDER_CONICSMEASURETYPE::DISTANCE;
			TotalNormalmeasureType(3);
			double Dist = RMATH3DOBJECT->Distance_Plane_Cylinder(&Conics3DParam[0], &CylinderParam[0], NormalmeasureType());
			setDimension(Dist);
			RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Conics3D(&CylinderParam[0], &Conics3DParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
		}
		else
		{   Cylinder_ConicsMType = CYLINDER_CONICSMEASURETYPE::ANGLE;
		    TotalNormalmeasureType(1);
			RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Conics3D(&CylinderParam[0], &Conics3DParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Angle_Line_Conics3D(&CylinderParam[0], &CylinderEndPts[0], &Conics3DParam[0], MeasurementPlane, DimSelectionLine, ProjectLine);
			this->DistanceMeasurement(false);
			this->setAngularDimension(Dist);
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0007", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConics3DDistance::UpdateForParentChange(BaseItem* sender)
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

void DimCylinderToConics3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimCylinderToConics3DDistance::Clone(int n)
{
	try
	{
		DimCylinderToConics3DDistance* Cdim = new DimCylinderToConics3DDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCylinderToConics3DDistance::CreateDummyCopy()
{
	try
	{
		DimCylinderToConics3DDistance* Cdim = new DimCylinderToConics3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCylinderToConics3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimCylinderToConics3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCylinderToConics3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimCylinderToConics3DDistance"<<endl;	
		return os;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCylinderToConics3DDistance& x)
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
			if(Tagname==L"DimCylinderToConics3DDistance")
			{
				while(Tagname!=L"EndDimCylinderToConics3DDistance")
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__); return is;}
}
void ReadOldPP(wistream& is, DimCylinderToConics3DDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__); }
}