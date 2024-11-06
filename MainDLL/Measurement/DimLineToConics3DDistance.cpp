#include "StdAfx.h"
#include "DimLineToConics3DDistance.h"
#include "..\Shapes\Conics3D.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLineToConics3DDistance::DimLineToConics3DDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0002", __FILE__, __FUNCSIG__); }
}

DimLineToConics3DDistance::~DimLineToConics3DDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0003", __FILE__, __FUNCSIG__); }
}

void DimLineToConics3DDistance::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0004", __FILE__, __FUNCSIG__); }
}

void DimLineToConics3DDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Line_ConicsMType)
	{
		case LINE_CONICSMEASURETYPE ::ANGLE:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case LINE_CONICSMEASURETYPE::DISTANCE:
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

void DimLineToConics3DDistance::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOCONICS3DDISTANCE;
		this->DistanceMeasurement(true);
		if(ParentShape1->ShapeType != RapidEnums::SHAPETYPE::ELLIPSE3D)
		{
			Shape* temp = this->ParentShape2;
			this->ParentShape2 = this->ParentShape1;
			this->ParentShape1 = temp;
		}

		((Conics3D*)ParentShape1)->getParameters(&Conics3DParam[0]);
		((Line*)ParentShape2)->getParameters(&LineParam[0]);
		((Line*)ParentShape2)->getEndPoints(&LineEndPoints[0]);

		double mid[3] = {Conics3DParam[4], Conics3DParam[5], Conics3DParam[6]};

		if(RMATH3DOBJECT->Checking_Parallel_Line_to_Plane(&LineParam[0], &Conics3DParam[0]))
		{
			RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Conics3D(&LineParam[0], &Conics3DParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Distance_Line_Plane(&LineParam[0], &Conics3DParam[0]);
			setDimension(Dist);
			Line_ConicsMType = LINE_CONICSMEASURETYPE::DISTANCE;
		}
		else
		{
			this->DistanceMeasurement(false);
			RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Conics3D(&LineParam[0], &Conics3DParam[0], &TransformationMatrix[0], &MeasurementPlane[0], &ProjectLine);
			double Dist = RMATH3DOBJECT->Angle_Line_Conics3D(&LineParam[0], &LineEndPoints[0], &Conics3DParam[0], MeasurementPlane, &DimSelectionLine[0], ProjectLine);
			this->setAngularDimension(Dist);
			Line_ConicsMType = LINE_CONICSMEASURETYPE::ANGLE;
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0006", __FILE__, __FUNCSIG__); }
}

void DimLineToConics3DDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		GRAFIX->drawFiniteline_Conics3DDistOrAng3D(&LineParam[0], &LineEndPoints[0], &Conics3DParam[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], DistanceMeasurement(), &DimSelectionLine[0], ProjectLine, windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0007", __FILE__, __FUNCSIG__); }
}

void DimLineToConics3DDistance::UpdateForParentChange(BaseItem* sender)
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

void DimLineToConics3DDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLineToConics3DDistance::Clone(int n)
{
	try
	{
		DimLineToConics3DDistance* Cdim = new DimLineToConics3DDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLineToConics3DDistance::CreateDummyCopy()
{
	try
	{
		DimLineToConics3DDistance* Cdim = new DimLineToConics3DDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLineToConics3DDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimLineToConics3DDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToConics3DDistance"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToConics3DDistance"<<endl;
		return os;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToConics3DDistance& x)
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
			if(Tagname==L"DimLineToConics3DDistance")
			{
			while(Tagname!=L"EndDimLineToConics3DDistance")
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
void ReadOldPP(wistream& is, DimLineToConics3DDistance& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2PLANE0013", __FILE__, __FUNCSIG__); }
}