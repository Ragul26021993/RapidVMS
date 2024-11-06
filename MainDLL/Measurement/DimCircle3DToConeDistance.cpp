#include "StdAfx.h"
#include "DimCircle3DToConeDistance.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Cone.h"
#include "..\Engine\RCadApp.h"

DimCircle3DToConeDistance::DimCircle3DToConeDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0002", __FILE__, __FUNCSIG__); }
}

DimCircle3DToConeDistance::~DimCircle3DToConeDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0003", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToConeDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0004", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToConeDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Circle3DConeMtype)
	{
		case CIRCLE3DCONEMEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CIRCLE3DCONEMEASURETYPE::DISTANCEONLY:
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

void DimCircle3DToConeDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(NormalmeasureType() == 0)
			GRAFIX->drawFiniteline_Circle3DDistOrAng3D(&ConeParam[0], &ConeEndPts[0], &Circle3DParam[0],  doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], false, &DimSelectionLine[0], ProjectLine, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if(NormalmeasureType() == 1)
			GRAFIX->drawPoint_FinitelineDistance3D( &Circle3DParam[0], SfEndPnts1, SfLine1, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if(NormalmeasureType() == 2)
			GRAFIX->drawPoint_FinitelineDistance3D( &Circle3DParam[0], ConeEndPts, ConeParam, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else if(NormalmeasureType() == 3)
			GRAFIX->drawPoint_FinitelineDistance3D( &Circle3DParam[0], SfEndPnts2, SfLine2, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0006", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToConeDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCONEDISTANCE;
		this->DistanceMeasurement(true);

		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::CONE)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
	    ((Circle*)ParentShape2)->getParameters(&Circle3DParam[0]);
		((Cone*)ParentShape1)->getParameters(&ConeParam[0]);
		((Cone*)ParentShape1)->getEndPoints(&ConeEndPts[0]);
		((Circle*)ParentShape2)->getPlaneParameters(&Circle3DAsPln[0]);
		if(NormalmeasureType() == 0)
		{
			this->DistanceMeasurement(false);
			RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cir3D(&ConeParam[0], &Circle3DParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Angle_Line_Circle3D(&ConeParam[0], &ConeEndPts[0], &Circle3DParam[0], &MeasurementPlane[0], DimSelectionLine, ProjectLine);
			this->setAngularDimension(Dist);
			Circle3DConeMtype = CIRCLE3DCONEMEASURETYPE::ANGLEONLY;
		}
		else 
		{
			double dir[4];
			((Cone*)ParentShape1)->getParameters(&ConeParam[0]);
			((Cone*)ParentShape1)->getEndPoints(&ConeEndPts[0]);
			RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Ln(&Circle3DParam[0], &ConeParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(&ConeParam[3], &MeasurementPlane[0], &dir[0]);
			((Cone*)ParentShape1)->getSurfaceLine(&dir[0], true, &SfLine1[0], &SfEndPnts1[0]);
			((Cone*)ParentShape1)->getSurfaceLine(&dir[0], false, &SfLine2[0], &SfEndPnts2[0]);
			double Dist = RMATH3DOBJECT->Distance_Point_Cone(&Circle3DParam[0], &ConeParam[0], &SfLine1[0], &SfLine2[0], NormalmeasureType());
			this->setDimension(Dist);
			Circle3DConeMtype = CIRCLE3DCONEMEASURETYPE::DISTANCEONLY;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0007", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToConeDistance::UpdateForParentChange(BaseItem* sender)
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

void DimCircle3DToConeDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimCircle3DToConeDistance::Clone(int n)
{
	try
	{
		DimCircle3DToConeDistance* Cdim = new DimCircle3DToConeDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCircle3DToConeDistance::CreateDummyCopy()
{
	try
	{
		DimCircle3DToConeDistance* Cdim = new DimCircle3DToConeDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCircle3DToConeDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os,DimCircle3DToConeDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimCircle3DtoConeDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimCircle3DtoConeDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCircle3DToConeDistance& x)
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
			if(Tagname==L"DimCircle3DtoConeDistance")
			{
				while(Tagname!=L"EndDimCircle3DtoConeDistance")
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
void ReadOldPP(wistream& is, DimCircle3DToConeDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2CY0013", __FILE__, __FUNCSIG__); }
}