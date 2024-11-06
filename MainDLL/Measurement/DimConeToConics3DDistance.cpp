#include "StdAfx.h"
#include "DimConeToConics3DDistance.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Conics3D.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"

DimConeToConics3DDistance::DimConeToConics3DDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0002", __FILE__, __FUNCSIG__); }
}

DimConeToConics3DDistance::~DimConeToConics3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0003", __FILE__, __FUNCSIG__); }
}

void DimConeToConics3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0004", __FILE__, __FUNCSIG__); }
}

void DimConeToConics3DDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(ConeConicsMtype)
	{
		case CONE_CONICSMEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CONE_CONICSMEASURETYPE::DISTANCEONLY:
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

void DimConeToConics3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(NormalmeasureType() == 0)
			GRAFIX->drawFiniteline_Conics3DDistOrAng3D( &ConeParam[0], &ConeEndPts[0], &Conics3DParam[0],  doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], false, &DimSelectionLine[0], ProjectLine, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if(NormalmeasureType() == 1)
			GRAFIX->drawPoint_FinitelineDistance3D( &Conics3DParam[4], SfEndPnts1, SfLine1, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if(NormalmeasureType() == 2)
			GRAFIX->drawPoint_FinitelineDistance3D( &Conics3DParam[4], ConeEndPts, ConeParam, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if(NormalmeasureType() == 3)
			GRAFIX->drawPoint_FinitelineDistance3D( &Conics3DParam[4], SfEndPnts2, SfLine2, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0006", __FILE__, __FUNCSIG__); }
}

void DimConeToConics3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONETOCONICS3DDISTANCE;
		this->DistanceMeasurement(true);

		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CONE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
	    ((Conics3D*)ParentShape2)->getParameters(&Conics3DParam[0]);
		((Cone*)ParentShape1)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape1)->getEndPoints(&ConeEndPts[0]);
		if(NormalmeasureType() == 0)
		{
			this->DistanceMeasurement(false);
			RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Conics3D(&ConeParam[0], &Conics3DParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Angle_Line_Conics3D(&ConeParam[0], &ConeEndPts[0], &Conics3DParam[0], &MeasurementPlane[0], DimSelectionLine, ProjectLine);
			this->setAngularDimension(Dist);
			ConeConicsMtype = CONE_CONICSMEASURETYPE::ANGLEONLY;
		}
		else 
		{
			double dir[4];
			RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Ln(&Conics3DParam[4], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&ConeParam[3], &MeasurementPlane[0], &dir[0]);
			((Cone*)ParentShape1)->getSurfaceLine(&dir[0], true, &SfLine1[0], &SfEndPnts1[0]);
			((Cone*)ParentShape1)->getSurfaceLine(&dir[0], false, &SfLine2[0], &SfEndPnts2[0]);
			double Dist = RMATH3DOBJECT->Distance_Point_Cone(&Conics3DParam[4], &ConeParam[0], &SfLine1[0], &SfLine2[0], NormalmeasureType());
			this->setDimension(Dist);
			ConeConicsMtype = CONE_CONICSMEASURETYPE::DISTANCEONLY;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0007", __FILE__, __FUNCSIG__); }
}

void DimConeToConics3DDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0008", __FILE__, __FUNCSIG__); }
}

void DimConeToConics3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimConeToConics3DDistance::Clone(int n)
{
	try
	{
		DimConeToConics3DDistance* Cdim = new DimConeToConics3DDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimConeToConics3DDistance::CreateDummyCopy()
{
	try
	{
		DimConeToConics3DDistance* Cdim = new DimConeToConics3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimConeToConics3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os,DimConeToConics3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimConeToConics3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimConeToConics3DDistance"<<endl;	
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimConeToConics3DDistance& x)
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
			if(Tagname==L"DimConeToConics3DDistance")
			{
				while(Tagname!=L"EndDimConeToConics3DDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__); return is;}
}
void ReadOldPP(wistream& is, DimConeToConics3DDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__);}
}