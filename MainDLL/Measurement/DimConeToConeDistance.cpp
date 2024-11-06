#include "StdAfx.h"
#include "DimConeToConeDistance.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"

DimConeToConeDistance::DimConeToConeDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0002", __FILE__, __FUNCSIG__); }
}

DimConeToConeDistance::~DimConeToConeDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0003", __FILE__, __FUNCSIG__); }
}

void DimConeToConeDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0004", __FILE__, __FUNCSIG__); }
}

void DimConeToConeDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(ConeMType)
	{
		case CONEMEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CONEMEASURETYPE::DISTANCEONLY:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
		case CONEMEASURETYPE::DISTANCEANDANGLE:
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

void DimConeToConeDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(ConeMType)
		{
			case CONEMEASURETYPE::ANGLEONLY:
				GRAFIX->drawCylinder_CylinderAngle3D(&Cone1Param[0], &Cone1EndPts[0], &Cone2Param[0], &Cone2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case CONEMEASURETYPE::DISTANCEONLY:
				GRAFIX->drawCylinder_CylinderDistance3D(&Cone1Param[0], &Cone1EndPts[0], &Cone2Param[0], &Cone2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);					
				break;
			case CONEMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
					GRAFIX->drawCylinder_CylinderAngle3D(&Cone1Param[0], &Cone1EndPts[0], &Cone2Param[0], &Cone2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				else
					GRAFIX->drawCylinder_CylinderDistance3D(&Cone1Param[0], &Cone1EndPts[0], &Cone2Param[0], &Cone2EndPts[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], NormalmeasureType() - 1, &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);						
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0006", __FILE__, __FUNCSIG__); }
}


void DimConeToConeDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double angle, angle1, Cy1MidPt[3];
		this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONETOCONEDISTANCE;
		this->DistanceMeasurement(true);
		((Cone*)ParentShape1)->getParameters(&Cone1Param[0]);
		((Cone*)ParentShape1)->getEndPoints(&Cone1EndPts[0]);
		((Cone*)ParentShape2)->getParameters(&Cone2Param[0]);
		((Cone*)ParentShape2)->getEndPoints(&Cone2EndPts[0]);
		for(int i = 0; i < 3; i++) Cy1MidPt[i] = (Cone1EndPts[i] + Cone1EndPts[3 + i]) / 2;
		if(RMATH3DOBJECT->Checking_Intersection_Line_Line(&Cone1Param[0], &Cone2Param[0]))
		{
			ConeMType = CONEMEASURETYPE::ANGLEONLY;
			TotalNormalmeasureType(1);
		}
		else if(RMATH3DOBJECT->Checking_Parallel_Line_to_Line(&Cone1Param[0], &Cone2Param[0]))
		{
			ConeMType = CONEMEASURETYPE::DISTANCEONLY;
			TotalNormalmeasureType(1);
		}
		else
		{
			ConeMType = CONEMEASURETYPE::DISTANCEANDANGLE;
			TotalNormalmeasureType(2);
		}
		switch(ConeMType)
		{
			case CONEMEASURETYPE::ANGLEONLY:
				this->DistanceMeasurement(false);
				RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&Cone1Param[0], &Cone2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				angle = RMATH3DOBJECT->Angle_Line_Line(&Cone1Param[0], &Cone2Param[0], MeasurementPlane, DimSelectionLine, Cy1MidPt);
				this->setAngularDimension(angle);
				break;
			case CONEMEASURETYPE::DISTANCEONLY:
				 angle = RMATH3DOBJECT->Distance_Cone_Cone(&Cone1Param[0], &Cone2Param[0]);
				 setDimension(angle);
				 RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&Cone1Param[0], &Cone2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				break;
			case CONEMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
				{
					RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cylndr(&Cone1Param[0], &Cone2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					angle = RMATH3DOBJECT->Angle_Line_Line(&Cone1Param[0], &Cone2Param[0], MeasurementPlane, DimSelectionLine, Cy1MidPt);
					this->DistanceMeasurement(false);
					this->setAngularDimension(angle);
				}
				else
				{
					angle = RMATH3DOBJECT->Distance_Cone_Cone(&Cone1Param[0], &Cone2Param[0]);
					RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Cylndr(&Cone1Param[0], &Cone2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
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

void DimConeToConeDistance::UpdateForParentChange(BaseItem* sender)
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

void DimConeToConeDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimConeToConeDistance::Clone(int n)
{
	try
	{
		DimConeToConeDistance* Cdim = new DimConeToConeDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimConeToConeDistance::CreateDummyCopy()
{
	try
	{
		DimConeToConeDistance* Cdim = new DimConeToConeDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimConeToConeDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimConeToConeDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimConeToConeDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimConeToConeDistance"<<endl;	
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCY2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimConeToConeDistance& x)
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
			if(Tagname==L"DimConeToConeDistance")
			{
				while(Tagname!=L"EndDimConeToConeDistance")
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
void ReadOldPP(wistream& is, DimConeToConeDistance& x)
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