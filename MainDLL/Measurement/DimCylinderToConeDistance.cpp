#include "StdAfx.h"
#include "DimCylinderToConeDistance.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"

DimCylinderToConeDistance::DimCylinderToConeDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0002", __FILE__, __FUNCSIG__); }
}

DimCylinderToConeDistance::~DimCylinderToConeDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0003", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConeDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0004", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConeDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(MType)
	{
		case MEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case MEASURETYPE::DISTANCEONLY:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
		case MEASURETYPE::DISTANCEANDANGLE:
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

void DimCylinderToConeDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(MType)
		{   
			case MEASURETYPE::ANGLEONLY:
				GRAFIX->drawCylinder_CylinderAngle3D(&CylinderParam[0], &CylinderEndPts[0], &ConeParam[0], &ConeEndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case MEASURETYPE::DISTANCEONLY:
				GRAFIX->drawCylinder_ConeDistance3D(&CylinderParam[0], &CylinderEndPts[0],&ConeParam[0], &ConeEndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);					
				break;
			case MEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
					GRAFIX->drawCylinder_CylinderAngle3D(&CylinderParam[0], &CylinderEndPts[0], &ConeParam[0], &ConeEndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				else
					GRAFIX->drawCylinder_ConeDistance3D(&CylinderParam[0], &CylinderEndPts[0], &ConeParam[0], &ConeEndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType() - 1, &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);						
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0006", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConeDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double angle, Cy1MidPt[3];
		this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCONEDISTANCE;
		this->DistanceMeasurement(true);
		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CYLINDER)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		((Cone*)ParentShape2)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape2)->getEndPoints(&ConeEndPts[0]);
		((Cylinder*)ParentShape1)->getParameters(&CylinderParam[0]);
		((Cylinder*)ParentShape1)->getEndPoints(&CylinderEndPts[0]);
		for(int i = 0; i < 3; i++) Cy1MidPt[i] = (ConeEndPts[i] + ConeEndPts[3 + i]) / 2;
		this->DistanceMeasurement(false);
		if(RMATH3DOBJECT->Checking_Intersection_Line_Line(&CylinderParam[0],&ConeParam[0]))
		{
			MType = MEASURETYPE::ANGLEONLY;
			TotalNormalmeasureType(1);
		}
		else if(RMATH3DOBJECT->Checking_Parallel_Line_to_Line(&CylinderParam[0], &ConeParam[0]))
		{
		MType = MEASURETYPE::DISTANCEONLY;
			TotalNormalmeasureType(3);
		}
		else
		{
			MType = MEASURETYPE::DISTANCEANDANGLE;
			TotalNormalmeasureType(4);
		}
		switch(MType)
		{
			case MEASURETYPE::ANGLEONLY:
				RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&CylinderParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				angle = RMATH3DOBJECT->Angle_Line_Line(&CylinderParam[0], &ConeParam[0], MeasurementPlane, DimSelectionLine, Cy1MidPt);
				this->DistanceMeasurement(false);
				this->setAngularDimension(angle);		
				break;
			case MEASURETYPE::DISTANCEONLY:
				 angle = RMATH3DOBJECT->Distance_Cylinder_Cone(&CylinderParam[0], &ConeParam[0], NormalmeasureType());
				 setDimension(angle);
				 RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&CylinderParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				break;
			case MEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
				{
					RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&CylinderParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					angle = RMATH3DOBJECT->Angle_Line_Line(&CylinderParam[0], &ConeParam[0], MeasurementPlane, DimSelectionLine, Cy1MidPt);
					this->DistanceMeasurement(false);
					this->setAngularDimension(angle);
				}
				else
				{
					angle = RMATH3DOBJECT->Distance_Cylinder_Cone(&CylinderParam[0], &ConeParam[0], NormalmeasureType() - 1);
					RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&CylinderParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					setDimension(angle);
				}
				break;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0007", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConeDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if((this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0) && this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			CalculateMeasurement((Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			CalculateMeasurement(ParentShape1, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0008", __FILE__, __FUNCSIG__); }
}

void DimCylinderToConeDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimCylinderToConeDistance::Clone(int n)
{
	try
	{
		DimCylinderToConeDistance* Cdim = new DimCylinderToConeDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCylinderToConeDistance::CreateDummyCopy()
{
	try
	{
		DimCylinderToConeDistance* Cdim = new DimCylinderToConeDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCylinderToConeDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimCylinderToConeDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCylinderToConeDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimCylinderToConeDistance"<<endl;	
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCylinderToConeDistance& x)
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
			if(Tagname==L"DimCylinderToConeDistance")
			{
				while(Tagname!=L"EndDimCylinderToConeDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCylinderToConeDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0013", __FILE__, __FUNCSIG__);}
}