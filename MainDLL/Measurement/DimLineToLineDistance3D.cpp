#include "StdAfx.h"
#include "DimLineToLineDistance3D.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLineToLineDistance3D::DimLineToLineDistance3D(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0002", __FILE__, __FUNCSIG__); }
}

DimLineToLineDistance3D::~DimLineToLineDistance3D()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0003", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance3D::init()
{
	try
	{
		this->MeasureAs3DMeasurement(true);
		NormalmeasureType(0);
		TotalNormalmeasureType(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0004", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance3D::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(Line3DMtype)
	{
		case LINE3DMEASURETYPE::ANGLEONLY:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case LINE3DMEASURETYPE::DISTANCEONLY:
		{
			myname = "Dist";
			distno++;
			count++;
			myname = myname + itoa(distno, temp, 10);
			break;
		}
		case LINE3DMEASURETYPE::DISTANCEANDANGLE:
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

void DimLineToLineDistance3D::CalculateMeasurement(Shape *ps1, Shape *ps2)
{
	try
	{
		double Dist, Ln1MidPt[3];
		this->ParentShape1 = ps1;
		this->ParentShape2 = ps2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE3D;
	
		this->DistanceMeasurement(true);

		((Line*)ParentShape1)->getParameters(&Line1Param[0]);
		((Line*)ParentShape1)->getEndPoints(&Line1EndPoints[0]);
		((Line*)ParentShape2)->getParameters(&Line2Param[0]);
		((Line*)ParentShape2)->getEndPoints(&Line2EndPoints[0]);
		for(int i = 0; i < 3; i++) Ln1MidPt[i] = (Line1EndPoints[i] + Line1EndPoints[3 + i]) / 2;
				
		if(RMATH3DOBJECT->Checking_Intersection_Line_Line(&Line1Param[0], &Line2Param[0]))
		{
			Line3DMtype = LINE3DMEASURETYPE::ANGLEONLY;
			TotalNormalmeasureType(1);
		}
		else if(RMATH3DOBJECT->Checking_Parallel_Line_to_Line(&Line1Param[0], &Line2Param[0]))
		{
			Line3DMtype = LINE3DMEASURETYPE::DISTANCEONLY;
			TotalNormalmeasureType(1);
		}
		else
		{
			Line3DMtype = LINE3DMEASURETYPE::DISTANCEANDANGLE;
			TotalNormalmeasureType(2);
		}
		switch(Line3DMtype)
		{
			case LINE3DMEASURETYPE::ANGLEONLY:
				this->DistanceMeasurement(false);
				RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Ln(&Line1Param[0], &Line2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				Dist = RMATH3DOBJECT->Angle_Line_Line(&Line1Param[0], &Line2Param[0], MeasurementPlane, &DimSelectionLine[0], Ln1MidPt);
				this->setAngularDimension(Dist);
				break;
			case LINE3DMEASURETYPE::DISTANCEONLY:
				 Dist = RMATH3DOBJECT->Distance_Line_Line(&Line1Param[0], &Line2Param[0]);
				 RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Ln(&Line1Param[0], &Line2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
				 setDimension(Dist);
				break;
			case LINE3DMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
				{
					this->DistanceMeasurement(false);
					RMATH3DOBJECT->TransformationMatrix_Angle_Ln2Ln(&Line1Param[0], &Line2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
					Dist = RMATH3DOBJECT->Angle_Line_Line(&Line1Param[0], &Line2Param[0], MeasurementPlane, &DimSelectionLine[0], Ln1MidPt);
					this->setAngularDimension(Dist);
				}
				else
				{
					 Dist = RMATH3DOBJECT->Distance_Line_Line(&Line1Param[0], &Line2Param[0]);
					 RMATH3DOBJECT->TransformationMatrix_Dist_Ln2Ln(&Line1Param[0], &Line2Param[0], &TransformationMatrix[0], &MeasurementPlane[0]);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0006", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance3D::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		switch(Line3DMtype)
		{
			case LINE3DMEASURETYPE::ANGLEONLY:
				GRAFIX->drawFiniteline_FinitelineAngle3D(&Line1Param[0], &Line1EndPoints[0], &Line2Param[0], &Line2EndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case LINE3DMEASURETYPE::DISTANCEONLY:
				GRAFIX->drawFiniteline_FinitelineDistance3D(&Line1Param[0], &Line1EndPoints[0], &Line2Param[0], &Line2EndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
			case LINE3DMEASURETYPE::DISTANCEANDANGLE:
				if(NormalmeasureType() == 0)
					GRAFIX->drawFiniteline_FinitelineAngle3D(&Line1Param[0], &Line1EndPoints[0], &Line2Param[0], &Line2EndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				else
					GRAFIX->drawFiniteline_FinitelineDistance3D(&Line1Param[0], &Line1EndPoints[0], &Line2Param[0], &Line2EndPoints[0], doubledimesion(), getCDimension(), getModifiedName(), &TransformationMatrix[0], &MeasurementPlane[0], &DimSelectionLine[0], windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0007", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance3D::UpdateForParentChange(BaseItem* sender)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0008", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance3D::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

DimBase* DimLineToLineDistance3D::Clone(int n)
{
	try
	{
		DimLineToLineDistance3D* Cdim = new DimLineToLineDistance3D(_T("Dist"));
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLineToLineDistance3D::CreateDummyCopy()
{
	try
	{
		DimLineToLineDistance3D* Cdim = new DimLineToLineDistance3D(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLineToLineDistance3D::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0011", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance3D::MeasurementTypeChanged()
{
	if(Line3DMtype == LINE3DMEASURETYPE::DISTANCEANDANGLE)
	{
		if(NormalmeasureType() == 0)
			distno--;
		else
			angleno--;
        SetMeasureName();
	}
}

wostream& operator<<( wostream& os, DimLineToLineDistance3D& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToLineDistance3D"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToLineDistance3D"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToLineDistance3D& x)
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
			if(Tagname==L"DimLineToLineDistance3D")
			{
			while(Tagname!=L"EndDimLineToLineDistance3D")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLineToLineDistance3D& x)
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE3D0013", __FILE__, __FUNCSIG__); }
}