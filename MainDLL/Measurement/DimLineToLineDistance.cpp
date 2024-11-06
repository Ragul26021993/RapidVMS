#include "StdAfx.h"
#include "DimLineToLineDistance.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"

DimLineToLineDistance::DimLineToLineDistance(bool simply):DimBase(false)
{
	try
	{
		setMeasureName = simply;
		IntCnt = 0; angleZone = -1;
		AngleTypeWithAxis = ANGLEWITHAXIS::DEFAULT;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0002", __FILE__, __FUNCSIG__); }
}

DimLineToLineDistance::~DimLineToLineDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0003", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance::SetMeasureName()
{
	std::string myname;
	char temp[10] = "/0";
	switch(this->MeasurementType)
	{
	    case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE:
		case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINERAYANGLE:
		{
			myname = "Ang";
			angleno++;
			count++;
			myname = myname + itoa(angleno, temp, 10);
			break;
		}
		case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE:
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

void DimLineToLineDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(abs(mposition.getX()) > 500000 || abs(mposition.getY()) > 500000) 
			return;
		double Zlevel = ((Line*)ParentShape1)->getPoint1()->getZ();
		switch(this->MeasurementType)
		{
		    	case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE:
				switch(angletype)
				{
					case ANGLETYPE::LINE_LINE:
						if(abs(angle1 - angle2) > 0.00001)
							GRAFIX->drawFiniteline_FinitelineAngle(angle1, intercept1, &Endpoints[0], &Endpoints[2], angle2, intercept2, &Endpoints[4], &Endpoints[6], mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						else
							GRAFIX->drawFiniteline_FinitelineDistance(&Endpoints[0], &Endpoints[2], &Endpoints[4], &Endpoints[6], angle1, intercept1, angle2, intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
					case ANGLETYPE::XRAY_XRAY:
						if(abs(angle1 - angle2) > 0.00001)
							GRAFIX->drawRay_RayAngle(((Line*)ParentShape1)->Angle(), intercept1, &Endpoints[0], ((Line*)ParentShape2)->Angle(), intercept2, &Endpoints[2], mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						else
							GRAFIX->drawRay_RayDistance(&Endpoints[0],((Line*)ParentShape1)->Angle(),intercept1,&Endpoints[2],((Line*)ParentShape2)->Angle(),intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
					case ANGLETYPE::XLINE_XLINE:
						if(abs(angle1 - angle2) > 0.00001)
							GRAFIX->drawInfiniteline_InfinitelineAngle(angle1, intercept1, angle2,intercept2, mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						else
							GRAFIX->drawInfiniteline_InfinitelineDistance(angle1, intercept1, angle2, intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
					case ANGLETYPE::LINE_XRAY:
						if(abs(angle1 - angle2) > 0.00001)
						{
							if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
								GRAFIX->drawFiniteline_RayAngle(angle1, intercept1, &Endpoints[0], &Endpoints[2], ((Line*)ParentShape2)->Angle(), intercept2, &Endpoints[4], mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
							else
								GRAFIX->drawFiniteline_RayAngle(angle2, intercept2, &Endpoints[2],&Endpoints[4], ((Line*)ParentShape1)->Angle(),intercept1,&Endpoints[0], mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						}
						else
						{
							if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
								GRAFIX->drawFiniteline_RayDistance(&Endpoints[0], &Endpoints[2], angle1, intercept1, &Endpoints[4], ((Line*)ParentShape2)->Angle(), intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
							else 
								GRAFIX->drawFiniteline_RayDistance(&Endpoints[2], &Endpoints[4], angle2, intercept2, &Endpoints[0], ((Line*)ParentShape1)->Angle(), intercept1, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						}
						break;
					case ANGLETYPE::LINE_XLINE:
						switch(AngleTypeWithAxis)
						{
							case ANGLEWITHAXIS::DEFAULT:
								if(abs(angle1 - angle2) > 0.00001)
								{
									if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
										GRAFIX->drawInfiniteline_FinitelineAngle(angle1, intercept1, &Endpoints[0], &Endpoints[2], angle2, intercept2, mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
									else
										GRAFIX->drawInfiniteline_FinitelineAngle(angle2, intercept2, &Endpoints[0], &Endpoints[2], angle1, intercept1, mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
								}
								else
								{
									if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::XLINE)
										GRAFIX->drawInfiniteline_FinitelineDistance(angle2, intercept2, &Endpoints[0], &Endpoints[2], angle1, intercept1, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
									else 
										GRAFIX->drawInfiniteline_FinitelineDistance(angle1, intercept1, &Endpoints[0], &Endpoints[2], angle2, intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
								}
								break;
							case ANGLEWITHAXIS::ANGLE_XAXIS:
								if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
									GRAFIX->drawFiniteline_FinitelineAngle(angle1, intercept1, &Endpoints[0], &Endpoints[2], angle2, Endpoints[1], &Endpoints[0], &Endpoints[0], mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
								else
									GRAFIX->drawFiniteline_FinitelineAngle(angle2, intercept2, &Endpoints[0], &Endpoints[2], angle1, Endpoints[1], &Endpoints[0], &Endpoints[0],  mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
								break;
							case ANGLEWITHAXIS::ANGLE_YAXIS:
								if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
									GRAFIX->drawFiniteline_FinitelineAngle(angle1, intercept1, &Endpoints[0], &Endpoints[2], angle2, Endpoints[0], &Endpoints[0], &Endpoints[0],  mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
								else
									GRAFIX->drawFiniteline_FinitelineAngle(angle2, intercept2, &Endpoints[0], &Endpoints[2], angle1, Endpoints[0], &Endpoints[0], &Endpoints[0],  mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
								break;
						}
						break;
					case ANGLETYPE::XRAY_XLINE:
						if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
							GRAFIX->drawInfiniteline_RayAngle(((Line*)ParentShape1)->Angle(), intercept1, &Endpoints[0], angle2, intercept2, mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						else
							GRAFIX->drawInfiniteline_RayAngle(((Line*)ParentShape2)->Angle(), intercept2, &Endpoints[0], angle1, intercept1, mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
				}
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE:
				switch(angletype)
				{
					case ANGLETYPE::LINE_LINE:	
						GRAFIX->drawFiniteline_FinitelineDistance(&Endpoints[0], &Endpoints[2], &Endpoints[4], &Endpoints[6], angle1, intercept1, angle2, intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
					case ANGLETYPE::XRAY_XRAY:
						GRAFIX->drawRay_RayDistance(&Endpoints[0],((Line*)ParentShape1)->Angle(),intercept1,&Endpoints[2],((Line*)ParentShape2)->Angle(),intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
					case ANGLETYPE::XLINE_XLINE:
						GRAFIX->drawInfiniteline_InfinitelineDistance(angle1, intercept1, angle2, intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
					case ANGLETYPE::LINE_XRAY:
						if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
							GRAFIX->drawFiniteline_RayDistance(&Endpoints[0], &Endpoints[2], angle1, intercept1, &Endpoints[4], ((Line*)ParentShape2)->Angle(), intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						else 
							GRAFIX->drawFiniteline_RayDistance(&Endpoints[2], &Endpoints[4], angle2, intercept2, &Endpoints[0], ((Line*)ParentShape1)->Angle(), intercept1, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break; 
					case ANGLETYPE::LINE_XLINE:
						if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::XLINE)
							GRAFIX->drawInfiniteline_FinitelineDistance(angle2, intercept2, &Endpoints[0], &Endpoints[2], angle1, intercept1, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						else 
							GRAFIX->drawInfiniteline_FinitelineDistance(angle1, intercept1, &Endpoints[0], &Endpoints[2], angle2, intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
					case ANGLETYPE::XRAY_XLINE:
						if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::XLINE)
							GRAFIX->drawInfiniteline_RayDistance(angle1, intercept1, &Endpoints[0], ((Line*)ParentShape2)->Angle(), intercept2, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						else  
							GRAFIX->drawInfiniteline_RayDistance(angle2, intercept2, &Endpoints[0], ((Line*)ParentShape1)->Angle(), intercept1, doubledimesion(), mposition.getX(), mposition.getY(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, Zlevel);
						break;
				}
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0005", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement((Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentShape1, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0006", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance::AngleIntercept(Shape *c_line, double *angle, double *intercept)
{
	try
	{
		Line *Cline = (Line*)c_line;
		if(Cline->Angle() > M_PI) *angle = Cline->Angle() - M_PI;
		else *angle = Cline->Angle();
		*intercept = Cline->Intercept();
		switch(Cline->ShapeType)
		{
		case RapidEnums::SHAPETYPE::LINE:
			Endpoints[IntCnt++] = Cline->getPoint1()->getX();
			Endpoints[IntCnt++] = Cline->getPoint1()->getY();
			Endpoints[IntCnt++] = Cline->getPoint2()->getX();
			Endpoints[IntCnt++] = Cline->getPoint2()->getY();
			break;
		case RapidEnums::SHAPETYPE::XRAY:
			Endpoints[IntCnt++] = Cline->getPoint1()->getX();
			Endpoints[IntCnt++] = Cline->getPoint1()->getY();
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0007", __FILE__, __FUNCSIG__); }
}

void DimLineToLineDistance::CalculateMeasurement(Shape *s1, Shape *s2,bool calculate)
{
	try
	{
		if(calculate)
		{
			IntCnt = 0;
			this->ParentShape1 = s1;
			this->ParentShape2 = s2;
			if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::LINE))
				angletype = ANGLETYPE::LINE_LINE;
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::XRAY))
				angletype = ANGLETYPE::XRAY_XRAY;
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::XLINE))
				angletype = ANGLETYPE::XLINE_XLINE;
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::XRAY))
				angletype = ANGLETYPE::LINE_XRAY;
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::XLINE))
				angletype = ANGLETYPE::LINE_XLINE;
			else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::XLINE))
				angletype = ANGLETYPE::XRAY_XLINE;
			AngleIntercept(ParentShape1, &angle1, &intercept1);
			AngleIntercept(ParentShape2, &angle2, &intercept2);
			RMATH2DOBJECT->Angle_FirstScndQuad(&angle1);
			RMATH2DOBJECT->Angle_FirstScndQuad(&angle2);
		}

		double intersctpt[3] = {0}, cp2[3] = {mposition.getX(), mposition.getY(), 0}, Tempangle;
		if (this->angleZone > 0)
		{
			double cp1[3] = { 0 };
			double tA = angle2 - angle1;
			if (tA < 0) tA += M_PI;
			RMATH2DOBJECT->Line_lineintersection(angle1, intercept1, angle2, intercept2, intersctpt);
			RMATH2DOBJECT->GetPtInTwoLineQuadrant(angle1, tA, this->angleZone, intersctpt, this->Meas_Location_Radius, cp1);
			cp2[0] = cp1[0]; cp2[1] = cp1[1];
			mposition.set(cp1[0], cp1[1], mposition.getZ());
		}
	
		this->DistanceMeasurement(true);
		switch(this->MeasurementType)
		{
		case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE:
			switch(AngleTypeWithAxis)
			{
				case ANGLEWITHAXIS::DEFAULT:
					Tempangle = RMATH2DOBJECT->Line2Line_angle(angle1, intercept1, angle2, intercept2, &cp2[0]);
					break;
				case ANGLEWITHAXIS::ANGLE_XAXIS:
					if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
						Tempangle = RMATH2DOBJECT->Line2Line_angle(angle1, intercept1, angle2, Endpoints[1], &cp2[0]);
					else
						Tempangle = RMATH2DOBJECT->Line2Line_angle(angle1, Endpoints[1], angle2, intercept2, &cp2[0]);
					break;
				case ANGLEWITHAXIS::ANGLE_YAXIS:
					if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
						Tempangle = RMATH2DOBJECT->Line2Line_angle(angle1, intercept1, angle2, Endpoints[0], &cp2[0]);
					else
						Tempangle = RMATH2DOBJECT->Line2Line_angle(angle1, Endpoints[0], angle2, intercept2, &cp2[0]);
					break;
			}
			this->DistanceMeasurement(false);
			this->setAngularDimension(Tempangle);
			if(abs(angle1 - angle2) < 0.00001)
			{
				Tempangle = RMATH2DOBJECT->Line2Line_dis(angle1, intercept1, angle2, intercept2);
				doubledimesion(Tempangle);
			}
			break;
		case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE:
			Tempangle = RMATH2DOBJECT->Line2Line_dis(angle1, intercept1, angle2, intercept2);
			this->setDimension(Tempangle);
			break;	
		case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINERAYANGLE:
			{
			   double dir1[3] = {((Line *)s1)->dir_l() , ((Line *)s1)->dir_m(), ((Line *)s1)->dir_n()};
			   double dir2[3] = {((Line *)s2)->dir_l() , ((Line *)s2)->dir_m(), ((Line *)s2)->dir_n()}, dir3[3] = {0};
			   RMATH3DOBJECT->CrossProductOf2Vectors(dir1, dir2, dir3);
			   Tempangle = RMATH3DOBJECT->Angle_Btwn_2Directions(dir1, dir2, false, false);
			   	this->DistanceMeasurement(false);
			   if(dir3[2] > -0.1)
				   this->setAngularDimension(Tempangle);
			   else
				   this->setAngularDimension(2 * M_PI - Tempangle);
			   break;
			}
		}
		if(setMeasureName)
		{
		    setMeasureName = false;
 	        SetMeasureName();
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0008", __FILE__, __FUNCSIG__); 
	}
}

void DimLineToLineDistance::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
} 

DimBase* DimLineToLineDistance::Clone(int n)
{
	try
	{
		std::wstring myname;
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE)
			myname = _T("Dist");
		else if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE)
			myname = _T("Ang");
		DimLineToLineDistance* Cdim = new DimLineToLineDistance(true);
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0009", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimLineToLineDistance::CreateDummyCopy()
{
	try
	{
		DimLineToLineDistance* Cdim = new DimLineToLineDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0010", __FILE__, __FUNCSIG__); return NULL; }
}

void DimLineToLineDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0011", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, DimLineToLineDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os <<"DimLineToLineDistance"<<endl;
		os <<"AngleTypeWithAxis:"<< x.AngleTypeWithAxis <<  endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"EndDimLineToLineDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0012", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimLineToLineDistance& x)
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
			if(Tagname==L"DimLineToLineDistance")
			{
				while(Tagname!=L"EndDimLineToLineDistance")
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
							//Now let us set up the angle position properly.
							double Line1[2] = { 0 }, Line2[2] = { 0 }, measP[2] = { 0 }, CorrectedMeasPt[2] = { 0 };
							Line1[0] = ((Line*)x.ParentShape1)->Angle(); Line1[1] = ((Line*)x.ParentShape1)->Intercept();
							Line2[0] = ((Line*)x.ParentShape2)->Angle(); Line2[1] = ((Line*)x.ParentShape2)->Intercept();
							measP[0] = x.mposition.getX(); measP[1] = x.mposition.getY();
							x.angleZone = RMATH2DOBJECT->GetZoneOfPtBetweenLines(Line1, Line2, measP, CorrectedMeasPt, &x.Meas_Location_Radius);
							x.mposition.set(CorrectedMeasPt[0], CorrectedMeasPt[1], x.mposition.getZ());
						}
						else if(Tagname==L"AngleTypeWithAxis")
						{
								x.AngleTypeWithAxis = DimLineToLineDistance::ANGLEWITHAXIS(atoi((const char*)(Val).c_str()));
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0013", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimLineToLineDistance& x)
{
	try
	{
		int n;
		is >> n; x.AngleTypeWithAxis = DimLineToLineDistance::ANGLEWITHAXIS(n);
		is >> n;
		x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 = (Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		x.UpdateMeasurement();
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("DIMLINE2LINE0013", __FILE__, __FUNCSIG__); }
}