#include "StdAfx.h"
#include "DimCircle3DToCircle3DDistance.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimCircle3DToCircle3DDistance::DimCircle3DToCircle3DDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0002", __FILE__, __FUNCSIG__); }
}

DimCircle3DToCircle3DDistance::~DimCircle3DToCircle3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0003", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToCircle3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0004", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToCircle3DDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Circle3DMtype)
	{
		case CIRCLE3DMEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CIRCLE3DMEASURETYPE::DISTANCEONLY:
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

void DimCircle3DToCircle3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double Dist = 0;
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCIRCLE3DDISTANCE;
		this->DistanceMeasurement(true);
	    ((Circle*)ParentShape1)->getParameters(Circle3D1);
	    ((Circle*)ParentShape2)->getParameters(Circle3D2);
		double Circle1AsPlane[4] = {Circle3D1[3], Circle3D1[4], Circle3D1[5], Circle3D1[3] * Circle3D1[0] + Circle3D1[4] * Circle3D1[1] + Circle3D1[5] * Circle3D1[2]};
		double Circle2AsPlane[4] = {Circle3D2[3], Circle3D2[4], Circle3D2[5], Circle3D2[3] * Circle3D2[0] + Circle3D2[4] * Circle3D2[1] + Circle3D2[5] * Circle3D2[2]};
		if(NormalmeasureType() == 0)
		{
			if(RMATH3DOBJECT->Checking_Parallel_Plane_to_Plane(&Circle1AsPlane[0], &Circle2AsPlane[0]))
			{
				Dist = RMATH3DOBJECT->Distance_Plane_Plane(&Circle1AsPlane[0], &Circle2AsPlane[0]);
			}
			else
			{
				this->DistanceMeasurement(false);
				RMATH3DOBJECT->TransformationMatrix_Angle_Pln2Pln(&Circle1AsPlane[0], &Circle2AsPlane[0], &Circle3D1[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				Dist = RMATH3DOBJECT->Angle_Plane_Plane(&Circle1AsPlane[0], &Circle2AsPlane[0], MeasurementPlane, DimSelectionLine);
				this->setAngularDimension(Dist);
				Circle3DMtype = CIRCLE3DMEASURETYPE::ANGLEONLY;
				if(setMeasureName)
				{
					setMeasureName = false;
 					SetMeasureName();
				}
				return;
			}
		}
		else if(NormalmeasureType() == 1)
		{
		   Dist = RMATH3DOBJECT->Pt2Circle3D_distance_Type(Circle3D1, Circle3D2, Circle2AsPlane, Circle3D2[6], 0);
		}
		else if(NormalmeasureType() == 2)
		{
		   Dist = RMATH3DOBJECT->Pt2Circle3D_distance_Type(Circle3D1, Circle3D2, Circle2AsPlane, Circle3D2[6], 2);
		}
		Circle3DMtype = CIRCLE3DMEASURETYPE::DISTANCEONLY;
		RMATH3DOBJECT->TransformationMatrix_Dist_Pln2Pln(&Circle1AsPlane[0], &Circle2AsPlane[0], &Circle3D1[0], &Circle3D2[0], &TransformationMatrix[0], &MeasurementPlane[0]);
        setDimension(Dist);	
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0006", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToCircle3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
	    if (NormalmeasureType() == 0)
		    GRAFIX->drawCircle3D_Circle3DDistOrAng3D(&Circle3D1[0], &Circle3D2[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], DistanceMeasurement(), &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	    else if(NormalmeasureType() == 1)
			GRAFIX->drawPoint_PointDistance3D(Circle3D1[0], Circle3D1[1], Circle3D1[2], Circle3D2[0], Circle3D2[1], Circle3D2[2], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	    else if(NormalmeasureType() == 2)
			GRAFIX->drawPoint_Circle3DDistance3D(&Circle3D1[0], &Circle3D2[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0007", __FILE__, __FUNCSIG__); }
}


void DimCircle3DToCircle3DDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0008", __FILE__, __FUNCSIG__); }
}

void DimCircle3DToCircle3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimCircle3DToCircle3DDistance::Clone(int n)
{
	try
	{
		DimCircle3DToCircle3DDistance* Cdim = new DimCircle3DToCircle3DDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimCircle3DToCircle3DDistance::CreateDummyCopy()
{
	try
	{
		DimCircle3DToCircle3DDistance* Cdim = new DimCircle3DToCircle3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimCircle3DToCircle3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimCircle3DToCircle3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimCircle3DtoCircle3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimCircle3DtoCircle3DDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimCircle3DToCircle3DDistance& x)
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
			if(Tagname==L"DimCircle3DtoCircle3DDistance")
			{
			while(Tagname!=L"EndDimCircle3DtoCircle3DDistance")
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimCircle3DToCircle3DDistance& x)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPL2PL0013", __FILE__, __FUNCSIG__); }
}