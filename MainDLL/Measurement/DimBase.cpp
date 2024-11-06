///Measurement base class...//
#include "StdAfx.h"
#include "DimBase.h"
#include "..\Engine\RCadApp.h"

DimBase::DimBase(TCHAR* dimname):BaseItem(dimname)
{
	try{ init(); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0001", __FILE__, __FUNCSIG__); }
}

DimBase::DimBase(bool simply):BaseItem(_T("Measure"))
{
	try{ init(); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0002", __FILE__, __FUNCSIG__); }
}

DimBase::~DimBase()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0003", __FILE__, __FUNCSIG__); }
}

void DimBase::init()
{
	try
	{
		strcpy_s(this->cdimension, 50, "");
		this->MeasureColor.set(0, 1, 0);
		this->MeasurementDirection(true);
		this->ContainsLinearMeasurementMode(false); this->LinearMeasurementMode(false);
		this->MeasureAs3DMeasurement(false); this->MeasureAsGDnTMeasurement(false);
		this->HighlightedForMouseMove(false); this->DistanceMeasurement(true);
		this->ChildMeasurementType(false); this->IsNegativeMeasurement(false);
		this->DistanceFromMousePoint = 0;
		
		this->Include_PassFail(false);
		this->MeasurementUsedForPPAbort(false);
		this->AddActionWhileEdit(true);
		this->NominalValue(0); this->UpperTolerance(0); this->LowerTolerance(0);
		this->NormalmeasureType(0); this->LinearmeasureType(0); this->TotalNormalmeasureType(1);
		this->VideoFont_ScaleFactor(20); this->RcadFont_ScaleFactor(20);
		this->IsValid(false);
		this->CurrentDimAddAction = NULL;

		RMATH2DOBJECT->LoadIdentityMatrix(&TransformationMatrix[0], 3);
		RMATH2DOBJECT->LoadIdentityMatrix(&MeasurementPlane[0], 2);
		this->ParentShape1 = NULL; this->ParentShape2 = NULL; this->ParentShape3 = NULL; this->ParentShape4 = NULL;
		this->ParentPoint1 = NULL; this->ParentPoint2 = NULL;
		for(int i = 0; i < 6; i++)
			this->DimSelectionLine[i] = 0;
		for(int i = 0; i < 3; i++)
		{
			TxtLftTop_Rcad[i] = 0; TxtRghtBtm_Rcad[i] = 0;
			TxtLftTop_Video[i] = 0; TxtRghtBtm_Video[i] = 0;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0004", __FILE__, __FUNCSIG__); }
}

TCHAR* DimBase::genName(const TCHAR* dimname)
{
	try
	{
		_tcscpy_s(name, 10, dimname);
		count++;
		TCHAR shapenumstr[10];
		_itot_s(count, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0005", __FILE__, __FUNCSIG__); return _T("NA");}
}


void DimBase::SetMousePosition(double x, double y, double z, bool UpdateMeasure)
{
	try
	{
		this->mposition.set(x, y, z);
		if(UpdateMeasure)
			this->UpdateMeasurement();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0006", __FILE__, __FUNCSIG__); }
}

void DimBase::SetSelectionLine(double *Sline, bool UpdateMeasure)
{
	try
	{
		for(int i = 0; i < 6; i++)
			DimSelectionLine[i] = Sline[i];
		if(UpdateMeasure)
			this->UpdateMeasurement();
		this->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0007", __FILE__, __FUNCSIG__); }
}

Vector* DimBase::getMousePosition()
{
	return &(this->mposition);
}

void DimBase::getLeftTopnRightBtm(double* Lefttop, double* RightBtm)
{
	try
	{
		Lefttop[0] = TxtLftTop_Rcad[0]; Lefttop[1] = TxtLftTop_Rcad[1];
		RightBtm[0] = TxtRghtBtm_Rcad[0]; RightBtm[1] = TxtRghtBtm_Rcad[1];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0007", __FILE__, __FUNCSIG__); }
}



void DimBase::TranslatePosition(Vector& Position)
{
	try
	{
		double SlineParam[6] = {this->DimSelectionLine[0] + Position.getX(), this->DimSelectionLine[1] + Position.getY(), this->DimSelectionLine[2] + Position.getZ(), this->DimSelectionLine[3], this->DimSelectionLine[4], this->DimSelectionLine[5]};
		this->mposition += Position;
		this->SetSelectionLine(SlineParam, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0007a", __FILE__, __FUNCSIG__); }
}

void DimBase::TransformPosition(double* transform)
{
	try
	{
		this->mposition.set(MAINDllOBJECT->TransformMultiply(transform, mposition.getX(), mposition.getY()));
		//double SlineParam[6] = {this->DimSelectionLine[0], this->DimSelectionLine[1], this->DimSelectionLine[2], this->DimSelectionLine[3], this->DimSelectionLine[4], this->DimSelectionLine[5]};
		//Vector TempVector;
		//TempVector.set(MAINDllOBJECT->TransformMultiply(transform, SlineParam[0], SlineParam[1]));
		//SlineParam[0] = TempVector.getX(); SlineParam[1] = TempVector.getY(); 
		//this->SetSelectionLine(SlineParam, true);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0007b", __FILE__, __FUNCSIG__);
	}
}

void DimBase::BasePlaneAlignment(double* transform)
{
	this->mposition.set(MAINDllOBJECT->TransformMultiply(transform, mposition.getX(), mposition.getY(), mposition.getZ()));
}


void DimBase::setDimension(double d, bool RadianType)
{
	try
	{
		if(this->IsNegativeMeasurement())
		{
			if(d > 0)
				d = -d;
		}
		int NoofDec = MAINDllOBJECT->MeasurementNoOfDecimal();
		if(abs(d) < 0.00001) d = 0.0;
		if(RadianType)
			this->doubledimesion(d);
		if(DistanceMeasurement() && MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			d = d / 25.4;
		this->dimension = d;
		char* cd = getCDimension();
		RMATH2DOBJECT->Double2String(d, NoofDec, cd); 
		setDimension(cd);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0008", __FILE__, __FUNCSIG__); }
}

void DimBase::setDimensionInteger(int d)
{
	try
	{
		if(abs(d) < 0.00001) d = 0.0;
		if(abs(d) > 10000) return;
		this->doubledimesion(d);
		this->dimension = d;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0008", __FILE__, __FUNCSIG__); }
}

void DimBase::setDimension(char* cd)
{
	try
	{
		strcpy(cdimension, cd);
		/*std::string temp = "ø";
		switch(type)
		{
			case MeasurementType::DIM_DIAMETER2D:
				strcat((char*)temp.c_str(), cdimension);
				strcpy(cdimension, temp.c_str());
				break;
			case MeasurementType::DIM_RADIUS2D:
				temp = "R";
				strcat((char*)temp.c_str(), cdimension);
				strcpy(cdimension, temp.c_str());
				break;
		
		}
		strcat(cdimension,this->getModifiedName());*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0009", __FILE__, __FUNCSIG__); }
}

void DimBase::setAngularDimension(double angle)
{
	try
	{
		double Tempangle = 0;
		char AngledegMin[50];
		int AngMtype = MAINDllOBJECT->AngleMeasurementMode();
		if(AngMtype == 0) 
		{
			this->setDimension(angle);
			RMATH2DOBJECT->Radian2Deg_Min_Sec(angle, &AngledegMin[0]);
			this->setDimension(&AngledegMin[0]);
		}
		else if(AngMtype == 1)
		{
			Tempangle = RMATH2DOBJECT->Radian2Degree(angle);
			this->setDimension(Tempangle, false);
		}
		else if(AngMtype == 2)
		{
			this->setDimension(angle);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0008", __FILE__, __FUNCSIG__); }
}

double DimBase::getDimension()
{
	return dimension;
}

char* DimBase::getCDimension()
{
	return this->cdimension;
}

double* DimBase::getMeasurementPlane()
{
	return this->MeasurementPlane;
}

void DimBase::ChangeMeasurementType()
{
	try
	{
		int Lmtype = LinearmeasureType(), Nmtype = NormalmeasureType();
		if(this->LinearMeasurementMode() && this->ContainsLinearMeasurementMode())
		{
			Lmtype++;
			if(Lmtype == TotalLinearmeasureType()) Lmtype = 0;
			LinearmeasureType(Lmtype);
		}
		else
		{
			Nmtype++;
			if(Nmtype == TotalNormalmeasureType()) Nmtype = 0;
			NormalmeasureType(Nmtype);
		}
		MeasurementTypeChanged();
		UpdateMeasurement();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0010", __FILE__, __FUNCSIG__); }
}

void DimBase::UpdateMeasurementColor()
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
		{
			if(this->getDimension() > this->NominalValue())
			{
				if(MAINDllOBJECT->MeasurementcolorInGradient)
				{
					double ColorFactor = 1;
					if(this->UpperTolerance() != 0)
						ColorFactor = abs((this->getDimension() - this->NominalValue()) / this->UpperTolerance());
					if(ColorFactor > 1)
						this->MeasureColor.set(1, 0, 0);
					else
						this->MeasureColor.set(ColorFactor, 1 - ColorFactor, 0);
				}
				else
				{
					if(this->getDimension() > this->NominalValue() + this->UpperTolerance())
						this->MeasureColor.set(1, 0, 0);
					else
						this->MeasureColor.set(0, 1, 0);
				}
			}
			else if(this->getDimension() < this->NominalValue())
			{
				if(MAINDllOBJECT->MeasurementcolorInGradient)
				{
					double ColorFactor = 1;
					if(this->LowerTolerance() != 0)
						ColorFactor = abs((this->NominalValue() - this->getDimension()) / this->LowerTolerance());
					if(ColorFactor > 1)
						this->MeasureColor.set(1, 0, 0);
					else
						this->MeasureColor.set(ColorFactor, 1 - ColorFactor, 0);
				}
				else
				{
					if(this->getDimension() < this->NominalValue() + this->LowerTolerance())
						this->MeasureColor.set(1, 0, 0);
					else
						this->MeasureColor.set(0, 1, 0);
				}
			}
			else
				this->MeasureColor.set(0, 1, 0);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0010a", __FILE__, __FUNCSIG__); }
}

void DimBase::MeasurementTypeChanged()
{
}

void DimBase::CopyOriginalProperties(DimBase* Cdim, bool updatemeasure)
{
	int lineNumber = 0;
	try
	{
		lineNumber++;
		this->MeasurementType = Cdim->MeasurementType;
		lineNumber++;
		this->MeasureColor.set(Cdim->MeasureColor.r, Cdim->MeasureColor.g, Cdim->MeasureColor.b);
		lineNumber++;
		this->MeasurementDirection(Cdim->MeasurementDirection());
		lineNumber++;
		this->ContainsLinearMeasurementMode(Cdim->ContainsLinearMeasurementMode());
		lineNumber++;
		this->LinearMeasurementMode(Cdim->LinearMeasurementMode());
		lineNumber++;
		this->MeasureAs3DMeasurement(Cdim->MeasureAs3DMeasurement());
		lineNumber++;
		this->MeasureAsGDnTMeasurement(Cdim->MeasureAsGDnTMeasurement());
		lineNumber++;
		this->NormalmeasureType(Cdim->NormalmeasureType());
		lineNumber++;
		this->LinearmeasureType(Cdim->LinearmeasureType());
		lineNumber++;
		this->TotalNormalmeasureType(Cdim->TotalNormalmeasureType());
		lineNumber++;
		this->VideoFont_ScaleFactor(Cdim->VideoFont_ScaleFactor());
		lineNumber++;
		this->RcadFont_ScaleFactor(Cdim->RcadFont_ScaleFactor());
		lineNumber++;
		this->DistanceMeasurement(Cdim->DistanceMeasurement());
		lineNumber++;
		this->ChildMeasurementType(Cdim->ChildMeasurementType());
		lineNumber++;

		this->Include_PassFail(Cdim->Include_PassFail());
		lineNumber++;
		this->NominalValue(Cdim->NominalValue());
		lineNumber++;
		this->UpperTolerance(Cdim->UpperTolerance());
		lineNumber++;
		this->LowerTolerance(Cdim->LowerTolerance());
		lineNumber++;

		this->mposition.set(Cdim->mposition);
		lineNumber++;
		for(int i = 0; i < 6; i++)
			this->DimSelectionLine[i] = Cdim->DimSelectionLine[i];
		lineNumber++;
		this->ParentShape1 = Cdim->ParentShape1; this->ParentShape2 = Cdim->ParentShape2; this->ParentShape3 = Cdim->ParentShape3; this->ParentShape4 = Cdim->ParentShape4;
		lineNumber++;
		this->ParentPoint1 = Cdim->ParentPoint1; this->ParentPoint2 = Cdim->ParentPoint2;
		lineNumber++;
		this->CopyMeasureParameters(Cdim);
		lineNumber++;
		this->IsValid(true);
		lineNumber++;
		if(updatemeasure) this->UpdateMeasurement();
		lineNumber++;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0014_" + std::to_string(lineNumber), __FILE__, __FUNCSIG__); }
}

void DimBase::drawMeasurement(int windowno, double WPixelWidth)
{
	try
	{
		if(!IsValid()) return;
		if((MAINDllOBJECT->getCurrentUCS().UCSMode() == 4) && windowno == 0) return;
		if((windowno == 0) && (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || 
								MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || 
								MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm ||
								MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm))
		   return;
		double FScalefact, Left_Top[6] = {0}, Right_Btm[3] = {0};
		if(windowno == 0) FScalefact = VideoFont_ScaleFactor() * WPixelWidth;
		else FScalefact = RcadFont_ScaleFactor() * WPixelWidth;
		drawCurrentMeasure(windowno, WPixelWidth, FScalefact, &Left_Top[0], &Right_Btm[0]);
		if(windowno == 0)
		{
			TxtLftTop_Video[0] = Left_Top[0]; TxtLftTop_Video[1] = Left_Top[1]; TxtLftTop_Video[2] = Left_Top[2];
			TxtRghtBtm_Video[0] = Right_Btm[0]; TxtRghtBtm_Video[1] = Right_Btm[1]; TxtRghtBtm_Video[2] = Right_Btm[2];
		}
		else
		{
			TxtLftTop_Rcad[0] = Left_Top[0]; TxtLftTop_Rcad[1] = Left_Top[1]; TxtLftTop_Rcad[2] = Left_Top[2], TxtLftTop_Rcad[3] = 1;
			TxtRghtBtm_Rcad[0] = Right_Btm[0]; TxtRghtBtm_Rcad[1] = Right_Btm[1]; TxtRghtBtm_Rcad[2] = Right_Btm[2], TxtRghtBtm_Rcad[3] = 1;
			TxtMid_Rcad[0] = Left_Top[3]; TxtMid_Rcad[1] = Left_Top[4]; TxtMid_Rcad[2] = Left_Top[5];
			TextHalfLength = RMATH3DOBJECT->Distance_Point_Point(TxtLftTop_Rcad, TxtRghtBtm_Rcad)/2;
		}
	}
	//catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0011", __FILE__, __FUNCSIG__); }
	catch (std::exception &ex)
	{
		std::string tempString = __FUNCSIG__;
		tempString.append(ex.what());
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0011", __FILE__, tempString);
	}
}

bool DimBase::Measure_IsInWindow(double x, double y, int windowno)
{
	try
	{
		if(MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD || MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH) return false;
		if(windowno == 0)
			return RMATH2DOBJECT->Ptisinwindow(x, y, TxtLftTop_Video[0], TxtRghtBtm_Video[1], TxtRghtBtm_Video[0], TxtLftTop_Video[1]);
		else
		   return RMATH2DOBJECT->Ptisinwindow(x, y, TxtLftTop_Rcad[0], TxtRghtBtm_Rcad[1], TxtRghtBtm_Rcad[0], TxtLftTop_Rcad[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0012", __FILE__, __FUNCSIG__); return false;}
}

bool DimBase::Measure_IsInWindow(Vector& corner1, Vector& corner2)
{
	try
	{
	/*	double p1[2] = {corner1.getX(),corner1.getY()}, p2[2] = {corner2.getX(),corner1.getY()}, p3[2] = {corner2.getX(),corner2.getY()}, p4[2] = {corner1.getX(),corner2.getY()},
				leftp[2] = {TxtLeftTopR[0], TxtLeftTopR[1]}, rightp[2] = {TxtRightBtmR[0], TxtLeftTopR[1]},leftb[2] = {TxtLeftTopR[0], TxtRightBtmR[1]},rightb[2] = {TxtRightBtmR[0], TxtRightBtmR[1]};
		return(RMATH2DOBJECT->Rect_isinwindow(&leftp[0],&rightp[0],&rightb[0],&leftb[0],&p1[0],&p2[0],&p3[0],&p4[0]));*/
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0013", __FILE__, __FUNCSIG__); return false; }
}

bool DimBase::Measure_IsInWindow(double *Sline)
{
	try
	{
		DistanceFromMousePoint = RMATH3DOBJECT->Distance_Point_Line(TxtMid_Rcad, Sline);
		double tmpDist = RMATH3DOBJECT->Distance_Point_Point(TxtLftTop_Rcad, TxtRghtBtm_Rcad);
		if (DistanceFromMousePoint < (tmpDist/2)) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0013", __FILE__, __FUNCSIG__); return false; }
}


int DimBase::count = 0;
int DimBase::distno = 0;
int DimBase::angleno = 0;
int DimBase::diano = 0;
int DimBase::radiusno = 0;
TCHAR DimBase::name[20];
void DimBase::reset()
{
	count = 0; distno = 0; angleno = 0;
	diano = 0; radiusno = 0;
}


wostream& operator<<(wostream& os, DimBase& x)
{
	try
	{
		wstring originalName ; string ModifiedName;
		originalName = x.getOriginalName();
		ModifiedName = x.getModifiedName();
		std::wstring currentModifiedName(ModifiedName.begin(), ModifiedName.end());
		os << "MeasurementType:" << x.MeasurementType << endl;
		os << "CommonMeasurement" << endl;
		os << "MeasurementType:" << x.MeasurementType << endl;
		os << "ID:" << x.getId() << endl;
		os << "IsValid:" <<x.IsValid() << endl;
		if(originalName==currentModifiedName)
		{
			os << "Name:" << x.getOriginalName() << endl;
		}
		else os<< "Name:" <<currentModifiedName<<endl;		
		os << "IsNegativeMeasurement:" << x.IsNegativeMeasurement() << endl;
		os << "LinearMeasurementMode:" << x.LinearMeasurementMode() << endl;		
		os << "MeasurementDirection:" << x.MeasurementDirection() << endl;
		os << "NormalmeasureType:" << x.NormalmeasureType() << endl;
		os << "LinearmeasureType:" <<  x.LinearmeasureType() << endl;
		os << "Include_PassFail:" << x.Include_PassFail() << endl;		
		os << "AddActionWhileEdit:" << x.AddActionWhileEdit() << endl;
		os << "NominalValue:" << x.NominalValue() << endl;
		os << "UpperTolerance:" << x.UpperTolerance() << endl;
		os << "LowerTolerance:" << x.LowerTolerance() << endl;
		os << "MeasurementUsedForPPAbort:" << x.MeasurementUsedForPPAbort() << endl;		
		if(x.IsValid()) os << "mposition:values" << endl << (*static_cast<Vector*>(&x.mposition)) << endl;		
		for(int i = 0; i < 6; i++)
			os << "DimSelectionLine"<<i<<":" << x.DimSelectionLine[i] << endl;
		
		os << "ParentsSize:" << x.getParents().size() << endl;
		for(list<BaseItem*>::iterator shapeiterator = x.getParents().begin();shapeiterator != x.getParents().end(); shapeiterator++)
			os << "shapeiteratorId:"<< ((Shape*)(*shapeiterator))->getId() << endl;
		os << "MeasureColorR:" << x.MeasureColor.r << endl << "MeasureColorG:" << x.MeasureColor.g << endl<< "MeasureColorB:" << x.MeasureColor.b << endl;
		os << "EndCommonMeasurement" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0015", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DimBase& x)
{
	try
	{
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}
		else
		{
			int n;
			wstring name;		
			std::wstring Tagname ;
			is >> Tagname;		
			if(Tagname==L"CommonMeasurement")
			{
				while(Tagname!=L"EndCommonMeasurement")
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
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname=Tag;										
						if(Tagname==L"MeasurementType")
						{
							x.MeasurementType = RapidEnums::MEASUREMENTTYPE(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname == L"ID")
						{
							x.setId(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname == L"IsValid")
						{
							if(Val=="0")
								x.IsValid(false);
							else
								x.IsValid(true);
						}
						else if(Tagname == L"Name")
						{
							name=RMATH2DOBJECT->StringToWString((Val).c_str());
							x.setOriginalName(name);
						}
						else if(Tagname == L"IsNegativeMeasurement")
						{
							if(Val=="0")
								x.IsNegativeMeasurement(false);	
							else
								x.IsNegativeMeasurement(true);	
						}
						else if(Tagname == L"LinearMeasurementMode")
						{
							if(Val=="0")
								x.LinearMeasurementMode(false);
							else
								x.LinearMeasurementMode(true);
						}
						else if(Tagname == L"MeasurementDirection")
						{
							if(Val=="0")					
								x.MeasurementDirection(false);	
							else						
								x.MeasurementDirection(true);
						}
						else if(Tagname == L"AddActionWhileEdit")
						{
							if(Val=="0")					
								x.AddActionWhileEdit(false);	
							else						
								x.AddActionWhileEdit(true);
						}
						else if(Tagname == L"NormalmeasureType")
						{
							x.NormalmeasureType(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname == L"LinearmeasureType")
						{
							x.LinearmeasureType(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname == L"Include_PassFail")
						{
							if(Val=="0")
								x.Include_PassFail(false);	
							else		
								x.Include_PassFail(true);
						}
						else if(Tagname == L"NominalValue")
						{
							x.NominalValue(atof((const char*)(Val).c_str()));
						}
						else if(Tagname == L"UpperTolerance")
						{
							x.UpperTolerance(atof((const char*)(Val).c_str()));
						}
						else if(Tagname == L"LowerTolerance")
						{
							x.LowerTolerance(atof((const char*)(Val).c_str()));
						}
						else if(Tagname == L"MeasurementUsedForPPAbort")
						{
							if(Val=="0")
								x.MeasurementUsedForPPAbort(false);	
							else						
								x.MeasurementUsedForPPAbort(true);
						}
						else if(Tagname == L"mposition")
						{
							if(x.IsValid())is >> (*static_cast<Vector*>(&x.mposition));
						}
						else if(Tagname == L"DimSelectionLine0")
						{
							x.DimSelectionLine[0] =atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"DimSelectionLine1")
						{
							x.DimSelectionLine[1] =atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"DimSelectionLine2")
						{
							x.DimSelectionLine[2] =atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"DimSelectionLine3")
						{
							x.DimSelectionLine[3] =atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"DimSelectionLine4")
						{
							x.DimSelectionLine[4] =atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"DimSelectionLine5")
						{
							x.DimSelectionLine[5] =atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"ParentsSize")
						{
						}
						else if(Tagname == L"shapeiteratorId")
						{					
							int j =atoi((const char*)(Val).c_str());
							x.addParent((Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]);
							((Shape*)(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]))->addMChild(&x);						
						}
						else if(Tagname == L"MeasureColorR")
						{
							x.MeasureColor.r=atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"MeasureColorG")
						{
							x.MeasureColor.g=atof((const char*)(Val).c_str());
						}
						else if(Tagname == L"MeasureColorB")
						{
							x.MeasureColor.b=atof((const char*)(Val).c_str());
						}	
					}
				}
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}		
		return is;
	}
	catch(...)
	{
		MAINDllOBJECT->PPLoadSuccessful(false);
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMBS0016", __FILE__, __FUNCSIG__); return is; 
	}
}
void ReadOldPP(wistream& is, DimBase& x)
{
try
	{
		int n; bool flag; double mpos;
		wstring name;
		is >> n;
		x.MeasurementType = RapidEnums::MEASUREMENTTYPE(n);
		is >> n;
		x.setId(n);
		is >> flag;
		x.IsValid(flag);
		is >> name;
		x.setOriginalName(name);
		is >> name;
		std::string Str = RMATH2DOBJECT->WStringToString(name);
		if(Str == "IsNegativeMeasurement")
		{
			is >> flag;		
			x.IsNegativeMeasurement(flag);
			is >> flag; x.LinearMeasurementMode(flag);
		}
		else
		{
			if(Str == "1")
				x.LinearMeasurementMode(true);
			else
				x.LinearMeasurementMode(false);
		}
		
		is >> flag; x.MeasurementDirection(flag);
		is >> n; x.NormalmeasureType(n);
		is >> n; x.LinearmeasureType(n);
		is >> flag; x.Include_PassFail(flag);
		is >> mpos; x.NominalValue(mpos);
		is >> mpos; x.UpperTolerance(mpos);
		is >> mpos; x.LowerTolerance(mpos);
		is >> flag; x.MeasurementUsedForPPAbort(flag);
		if(x.IsValid())is >> (*static_cast<Vector*>(&x.mposition));
		for(int i = 0; i < 6; i++)
		{
			is >> mpos; x.DimSelectionLine[i] = mpos;
		}
		is >> n;
		for(int i = 0; i < n; i++)
		{
			int j;
			is >> j;
			x.addParent((Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]);
			((Shape*)(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]))->addMChild(&x);
		}
		is >> x.MeasureColor.r >> x.MeasureColor.g >> x.MeasureColor.b;
	}
	catch(...)
	{
		MAINDllOBJECT->PPLoadSuccessful(false);
		MAINDllOBJECT->SetAndRaiseErrorMessage("OLDDIMBS0016", __FILE__, __FUNCSIG__);
	}
}
