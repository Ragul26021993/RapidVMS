#include "StdAfx.h"
#include "DimLineToCircle3DDistance.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

DimLineToCircle3DDistance::DimLineToCircle3DDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0002", __FILE__, __FUNCSIG__); }
}

DimLineToCircle3DDistance::~DimLineToCircle3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0003", __FILE__, __FUNCSIG__); }
}

void DimLineToCircle3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0004", __FILE__, __FUNCSIG__); }
}

void DimLineToCircle3DDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(CLMeasuretype)
	{
		case CIRCLE3DLINEMESURETYPE ::ANGLE:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case CIRCLE3DLINEMESURETYPE::DISTANCE:
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

void DimLineToCircle3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1; this->ParentShape2 = ps2;
		if((ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D) || (ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CIRCLE))
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOCIRCLE3DDISTANCE;
		((Circle*)ParentShape1)->getParameters(&Circle3D[0]);
	 	double Circle3DAsPln[4];
		((Circle*)ParentShape1)->getPlaneParameters(&Circle3DAsPln[0]);
	    
		((Line*)ParentShape2)->getParameters(&LineParam[0]);
		((Line*)ParentShape2)->getEndPoints(&LineEndPoints[0]);
		if(((Line*)ParentShape2)->LineType == RapidEnums::LINETYPE::INFINITELINE)
		{
		     for(int i = 0; i < 3; i++)
			 {
				 LineParam[i] = LineParam[i] / pow(10.0, 5);
				 LineEndPoints[0 + i] /= pow(10.0, 5);
				 LineEndPoints[3 + i] /= pow(10.0, 5);
			 }
		}	
		if(NormalmeasureType() == 0)
		{
			RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Cir3D(&LineParam[0],Circle3D, &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			if(RMATH3DOBJECT->Checking_Parallel_Line_to_Plane(&LineParam[0], &Circle3D[3]))
			{
				distanceType(true);
				double Dist = RMATH3DOBJECT->Distance_Line_Plane(&LineParam[0], &Circle3DAsPln[0]);
				setDimension(Dist);
				CLMeasuretype = CIRCLE3DLINEMESURETYPE::DISTANCE;
			}
			else
			{
				this->DistanceMeasurement(false);
				distanceType(false);
				double Dist = RMATH3DOBJECT->Angle_Line_Circle3D(&LineParam[0], &LineEndPoints[0], &Circle3D[0], MeasurementPlane, &DimSelectionLine[0], ProjectLine);	
				this->setAngularDimension(Dist);
				CLMeasuretype = CIRCLE3DLINEMESURETYPE::ANGLE;
			}
		}
		else if(NormalmeasureType() == 1)
		{
		    distanceType(true);
			RMATH3DOBJECT->TransformationMatrix_Dist_Pt2Ln(Circle3D, &LineParam[0], &TransformationMatrix[0], &MeasurementPlane[0]);
			double Dist = RMATH3DOBJECT->Distance_Point_Line(Circle3D, &LineParam[0]);
			setDimension(Dist);
			CLMeasuretype = CIRCLE3DLINEMESURETYPE::DISTANCE;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0006", __FILE__, __FUNCSIG__); }
}

void DimLineToCircle3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(NormalmeasureType() == 0)
		   GRAFIX->drawFiniteline_Circle3DDistOrAng3D(&LineParam[0], &LineEndPoints[0],Circle3D, doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], distanceType(), &DimSelectionLine[0], ProjectLine, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
		else
	    	GRAFIX->drawPoint_FinitelineDistance3D(&Circle3D[0], &LineEndPoints[0], &LineParam[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0007", __FILE__, __FUNCSIG__); }
}

void DimLineToCircle3DDistance::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0008", __FILE__, __FUNCSIG__); }
}

void DimLineToCircle3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLineToCircle3DDistance::Clone(int n)
{
	try
	{
		DimLineToCircle3DDistance* Cdim = new DimLineToCircle3DDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLineToCircle3DDistance::CreateDummyCopy()
{
	try
	{
		DimLineToCircle3DDistance* Cdim = new DimLineToCircle3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLineToCircle3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimLineToCircle3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToCircle3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToCircle3DDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToCircle3DDistance& x)
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
			if(Tagname==L"DimLineToCircle3DDistance")
			{
			while(Tagname!=L"EndDimLineToCircle3DDistance")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLineToCircle3DDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0013", __FILE__, __FUNCSIG__); }
}