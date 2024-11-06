#include "StdAfx.h"
#include "DimCylinderToCylinderDistance.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"

DimCylinderToCylinderDistance::DimCylinderToCylinderDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0002", __FILE__, __FUNCSIG__); }
}

DimCylinderToCylinderDistance::~DimCylinderToCylinderDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0003", __FILE__, __FUNCSIG__); }
}

void DimCylinderToCylinderDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0004", __FILE__, __FUNCSIG__); }
}

void DimCylinderToCylinderDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(CylinderMType)
	{
		case CYLINDERMEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CYLINDERMEASURETYPE::DISTANCEONLY:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
		case CYLINDERMEASURETYPE::DISTANCEANDANGLE:
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

void DimCylinderToCylinderDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(CylinderMType)
		{
			case CYLINDERMEASURETYPE::ANGLEONLY:
				GRAFIX->drawCylinder_CylinderAngle3D(&Cylinder1Param[0], &Cylinder1EndPts[0], &Cylinder2Param[0], &Cylinder2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case CYLINDERMEASURETYPE::DISTANCEONLY:
				GRAFIX->drawCylinder_CylinderDistance3D(&Cylinder1Param[0], &Cylinder1EndPts[0], &Cylinder2Param[0], &Cylinder2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);					
				break;
			case CYLINDERMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
					GRAFIX->drawCylinder_CylinderAngle3D(&Cylinder1Param[0], &Cylinder1EndPts[0], &Cylinder2Param[0], &Cylinder2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				else
					GRAFIX->drawCylinder_CylinderDistance3D(&Cylinder1Param[0], &Cylinder1EndPts[0], &Cylinder2Param[0], &Cylinder2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType() - 1, &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);						
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0006", __FILE__, __FUNCSIG__); }
}

void DimCylinderToCylinderDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double angle, Cy1MidPt[3];
		this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCYLINDERDISTANCE;
		this->DistanceMeasurement(true);
		((Cylinder*)ParentShape1)->getParameters(&Cylinder1Param[0]);
		((Cylinder*)ParentShape1)->getEndPoints(&Cylinder1EndPts[0]);
		((Cylinder*)ParentShape2)->getParameters(&Cylinder2Param[0]);
		((Cylinder*)ParentShape2)->getEndPoints(&Cylinder2EndPts[0]);
		for(int i = 0; i < 3; i++) Cy1MidPt[i] = (Cylinder1EndPts[i] + Cylinder1EndPts[3 + i]) / 2;
		if(RMATH3DOBJECT->Checking_Intersection_Line_Line(&Cylinder1Param[0], &Cylinder2Param[0]))
		{
			CylinderMType = CYLINDERMEASURETYPE::ANGLEONLY;
			TotalNormalmeasureType(1);
		}
		else if(RMATH3DOBJECT->Checking_Parallel_Line_to_Line(&Cylinder1Param[0], &Cylinder2Param[0]))
		{
			CylinderMType = CYLINDERMEASURETYPE::DISTANCEONLY;
			TotalNormalmeasureType(3);
		}
		else
		{
			CylinderMType = CYLINDERMEASURETYPE::DISTANCEANDANGLE;
			TotalNormalmeasureType(4);
		}
		switch(CylinderMType)
		{
			case CYLINDERMEASURETYPE::ANGLEONLY:
				this->DistanceMeasurement(false);
				RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&Cylinder1Param[0], &Cylinder2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				angle = RMATH3DOBJECT->Angle_Line_Line(&Cylinder1Param[0], &Cylinder2Param[0], MeasurementPlane, DimSelectionLine, Cy1MidPt);
				this->setAngularDimension(angle);
				break;
			case CYLINDERMEASURETYPE::DISTANCEONLY:
				 angle = RMATH3DOBJECT->Distance_Cylinder_Cylinder(&Cylinder1Param[0], &Cylinder2Param[0], NormalmeasureType());
				 setDimension(angle);
				 RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&Cylinder1Param[0], &Cylinder2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				break;
			case CYLINDERMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
				{
					RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&Cylinder1Param[0], &Cylinder2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					angle = RMATH3DOBJECT->Angle_Line_Line(&Cylinder1Param[0], &Cylinder2Param[0], MeasurementPlane, DimSelectionLine, Cy1MidPt);
					this->DistanceMeasurement(false);
					this->setAngularDimension(angle);
				}
				else
				{
					angle = RMATH3DOBJECT->Distance_Cylinder_Cylinder(&Cylinder1Param[0], &Cylinder2Param[0], NormalmeasureType() - 1);
					RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&Cylinder1Param[0], &Cylinder2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
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

void DimCylinderToCylinderDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0008", __FILE__, __FUNCSIG__); }
}

void DimCylinderToCylinderDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimCylinderToCylinderDistance::Clone(int n)
{
	try
	{
		DimCylinderToCylinderDistance* Cdim = new DimCylinderToCylinderDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCylinderToCylinderDistance::CreateDummyCopy()
{
	try
	{
		DimCylinderToCylinderDistance* Cdim = new DimCylinderToCylinderDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCylinderToCylinderDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimCylinderToCylinderDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCylinderToCylinderDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimCylinderToCylinderDistance"<<endl;	
		return os;			
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCylinderToCylinderDistance& x)
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
			if(Tagname==L"DimCylinderToCylinderDistance")
			{
				while(Tagname!=L"EndDimCylinderToCylinderDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCylinderToCylinderDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0013", __FILE__, __FUNCSIG__); }
}