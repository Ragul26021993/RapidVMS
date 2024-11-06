#include "StdAfx.h"
#include "DimPointToPointDistance.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\RPoint.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Line.h"
#include "..\Engine\PartProgramFunctions.h"

bool checknear;
DimPointToPointDistance::DimPointToPointDistance(TCHAR* myname, RapidEnums::MEASUREMENTTYPE mtype):DimBase(genName(myname))
{
	try
	{
		this->CurrentStatus = true;
		IsValid(true);
		this->MeasurementType = mtype;
		firstNear = true;
		secondNear = true;
		checknear = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0001", __FILE__, __FUNCSIG__); }
}

DimPointToPointDistance::DimPointToPointDistance(bool simply):DimBase(false)
{
	try
	{
		this->CurrentStatus = true;
		IsValid(true);
		firstNear = true;
		secondNear = true;
		checknear = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0002", __FILE__, __FUNCSIG__); }
}

DimPointToPointDistance::~DimPointToPointDistance()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimPointToPointDistance::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		distno++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(distno, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimPointToPointDistance::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		if(LinearMeasurementMode())
			GRAFIX->drawPoint_PointDistance_Linear(ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint2->getX(), ParentPoint2->getY(), CurrentStatus, mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, ParentPoint1->getZ());
		else
			GRAFIX->drawPoint_PointDistance(ParentPoint1->getX(), ParentPoint1->getY(), ParentPoint2->getX(), ParentPoint2->getY(), mposition.getX(), mposition.getY(), doubledimesion(), getCDimension(), getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm,ParentPoint1->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0005", __FILE__, __FUNCSIG__); }
}

void DimPointToPointDistance::UpdateForParentChange(BaseItem* sender)
{
	if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
	UpdateMeasurement();
}

void DimPointToPointDistance::CalculateMeasurement(Vector *vv1, Vector *vv2)
{
	try
	{
		this->ParentPoint1 = vv1;
		this->ParentPoint2 = vv2;
		double pp1[2] = {ParentPoint1->getX(), ParentPoint1->getY()};
		double pp2[2] = {ParentPoint2->getX(), ParentPoint2->getY()};
		double mp[2] = {mposition.getX(), mposition.getY()}, dis = 0;
		bool NextStatus;
		MeasurementPlane[3] = ParentPoint1->getZ();
		if(this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE)
		{
			if(!PPCALCOBJECT->IsPartProgramRunning() && checknear)
			{
				list<Shape*> parentv1, parentv2;
				IsectPt* firstPnt = NULL, *secondPnt = NULL; 
				bool parentOfv1 = false, parentOfv2 = false;
				for(RC_ITER Sitem = MAINDllOBJECT->getShapesList().getList().begin(); Sitem != MAINDllOBJECT->getShapesList().getList().end(); Sitem++)
				{
		   			Shape* CShape = (Shape*)Sitem->second;
					if(!CShape->IsValid() && !CShape->Normalshape()) continue;
					if(!(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)) continue;
					ISN_ITER item = ((ShapeWithList*)CShape)->IsectPointsList->getList().begin(); 
					while(item  != ((ShapeWithList*)CShape)->IsectPointsList->getList().end())
					{
						IsectPt* i = (IsectPt*)(*item).second;
						if(abs(i->getX() - vv1->getX()) <= .0001 && abs(i->getY() - vv1->getY()) <= 0.0001)
						{
							firstPnt = i;
							parentOfv1 = true;
						}
						if(abs(i->getX() - vv2->getX()) <= .0001 && abs(i->getY() - vv2->getY()) <= 0.0001)
						{
							secondPnt = i;
							parentOfv2 = true;
						}
						if(parentOfv1 && parentOfv2) break;
						item++;
					}
					if(parentOfv1 && parentOfv2) break;
				}
				if(firstPnt != NULL)
				{
					Shape *s1 = NULL, *s2 = NULL;
					s1 = firstPnt->getParent1();
					s2 = firstPnt->getParent2();
					if(s1 != NULL && s2 != NULL)
					{
						if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ARC))
						{
							if(s1->ShapeType != RapidEnums::SHAPETYPE::LINE)
							{
								Shape * tmpshape = s1;
								s1 = s2;
								s2 = tmpshape;
							}
							IsectPt* otherInpnt = NULL; 
							ISN_ITER item = ((ShapeWithList*)s1)->IsectPointsList->getList().begin(); 
							while(item  != ((ShapeWithList*)s1)->IsectPointsList->getList().end())
							{
								IsectPt* i = (IsectPt*)(*item).second;
								if(i->getParent1() != NULL && i->getParent2() != NULL)
								{
									if(compareboth<int>(i->getParent1()->getId(), i->getParent2()->getId(),s1->getId(), s2->getId()))
									{
										if(firstPnt->IsnPtId == i->IsnPtId)
										{
											item++;
											continue;
										}
										otherInpnt = i;
										break;
									}
								}
								item++;
		    				}
							if(otherInpnt != NULL)
							{
								double point1[2] = {((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY()};
								double intrPnts[4] = {firstPnt->getX(), firstPnt->getY(), otherInpnt->getX(), otherInpnt->getY()};
								double dist1 = RMATH2DOBJECT->Pt2Pt_distance(point1, intrPnts);
								double dist2 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intrPnts[2]);
								if(dist1 > dist2)
									firstNear = false;
							}
						}
					}
				}
				if(secondPnt != NULL)
				{
					Shape *s1 = NULL, *s2 = NULL;
					s1 = secondPnt->getParent1();
					s2 = secondPnt->getParent2();
					if(s1 != NULL && s2 != NULL)
					{
						if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ARC))
						{
							if(s1->ShapeType != RapidEnums::SHAPETYPE::LINE)
							{
								Shape * tmpshape = s1;
								s1 = s2;
								s2 = tmpshape;
							}
							IsectPt* otherInpnt = NULL; 
							ISN_ITER item = ((ShapeWithList*)s1)->IsectPointsList->getList().begin(); 
							while(item  != ((ShapeWithList*)s1)->IsectPointsList->getList().end())
							{
								IsectPt* i = (IsectPt*)(*item).second;
								if(i->getParent1() != NULL && i->getParent2() != NULL)
								{
									if(compareboth<int>(i->getParent1()->getId(), i->getParent2()->getId(),s1->getId(), s2->getId()))
									{
										if(secondPnt->IsnPtId == i->IsnPtId)
										{
										   item++;
										   continue;
										}
										otherInpnt = i;
										break;
									}
								}
								item++;
		    				}
							if(otherInpnt != NULL)
							{
								double point1[2] = {((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY()};
								double intrPnts[4] = {secondPnt->getX(), secondPnt->getY(), otherInpnt->getX(), otherInpnt->getY()};
								double dist1 = RMATH2DOBJECT->Pt2Pt_distance(point1, intrPnts);
								double dist2 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intrPnts[2]);
								if(dist1 > dist2)
									secondNear = false;
							}
						 }
					  }
				}
			}
			if(LinearMeasurementMode())
			{
				dis = RMATH2DOBJECT->LPt2Pt_distance(&pp1[0], &pp2[0], CurrentStatus, &NextStatus, &mp[0]);
				CurrentStatus = NextStatus;
			}
			else
				dis = RMATH2DOBJECT->Pt2Pt_distance(&pp1[0], &pp2[0]);
		}
		else if(MAINDllOBJECT->Vblock_CylinderAxisLine != NULL)
		{
			Shape *axisline = MAINDllOBJECT->Vblock_CylinderAxisLine;
			double finalPnts[4] = {0};
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)axisline)->Angle(), ((Line*)axisline)->Intercept(), pp1, finalPnts);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)axisline)->Angle(), ((Line*)axisline)->Intercept(), pp2, &finalPnts[2]);
			dis = RMATH2DOBJECT->Pt2Pt_distance(&finalPnts[0], &finalPnts[2]);
        }
		this->setDimension(dis);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0006", __FILE__, __FUNCSIG__); }
}

void DimPointToPointDistance::UpdateMeasurement()
{
	if(PPCALCOBJECT->IsPartProgramRunning() && checknear && (this->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE))
	{
        	double snapdistance = MAINDllOBJECT->getWindow(0).getUppX() * SNAPDIST;
			IsectPt* firstPnt = NULL, *secondPnt = NULL; 
			bool parentOfv1 = false, parentOfv2 = false;
			for(RC_ITER Sitem = MAINDllOBJECT->getShapesList().getList().begin(); Sitem != MAINDllOBJECT->getShapesList().getList().end(); Sitem++)
			{
		   		Shape* CShape = (Shape*)Sitem->second;
				if(!CShape->IsValid() && !CShape->Normalshape()) continue;
				if(!(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)) continue;
			    ISN_ITER item = ((ShapeWithList*)CShape)->IsectPointsList->getList().begin(); 
				while(item != ((ShapeWithList*)CShape)->IsectPointsList->getList().end())
				{
					IsectPt* i = (IsectPt*)(*item).second;
					if(abs(i->getX() - ParentPoint1->getX()) <= snapdistance && abs(i->getY() - ParentPoint1->getY()) <= snapdistance)
					{
						firstPnt = i;
						parentOfv1 = true;
					}
					if(abs(i->getX() - ParentPoint2->getX()) <= snapdistance && abs(i->getY() - ParentPoint2->getY()) <= snapdistance)
				    {
						secondPnt = i;
						parentOfv2 = true;
					}
					if(parentOfv1 && parentOfv2) break;
					item++;
				}
				if(parentOfv1 && parentOfv2) break;
			}
			if(firstPnt != NULL)
			{
				Shape *s1 = NULL, *s2 = NULL;
				s1 = firstPnt->getParent1();
				s2 = firstPnt->getParent2();
				if(s1 != NULL && s2 != NULL)
				{
					if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ARC))
					{
					    if(s1->ShapeType != RapidEnums::SHAPETYPE::LINE)
					    {
							Shape * tmpshape = s1;
							s1 = s2;
							s2 = tmpshape;
					    }
						IsectPt* otherInpnt = NULL; 
						ISN_ITER item = ((ShapeWithList*)s1)->IsectPointsList->getList().begin(); 
						while(item  != ((ShapeWithList*)s1)->IsectPointsList->getList().end())
						{
							IsectPt* i = (IsectPt*)(*item).second;
							if(i->getParent1() != NULL && i->getParent2() != NULL)
							{
								if(compareboth<int>(i->getParent1()->getId(), i->getParent2()->getId(),s1->getId(), s2->getId()))
								{
									if(firstPnt->IsnPtId == i->IsnPtId)
									{
										item++;
										continue;
									}
									otherInpnt = i;
									break;
								}
							}
							item++;
		    			}
						if(otherInpnt != NULL)
						{
							double point1[2] = {((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY()};
							double intrPnts[4] = {firstPnt->getX(), firstPnt->getY(), otherInpnt->getX(), otherInpnt->getY()};
						    double dist1 = RMATH2DOBJECT->Pt2Pt_distance(point1, intrPnts);
							double dist2 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intrPnts[2]);
							if((firstNear && (dist1 > dist2)) || (!firstNear && (dist1 < dist2)))
								ParentPoint1 = otherInpnt;
						}
					}
				}
			}
			if(secondPnt != NULL)
			{
				Shape *s1 = NULL, *s2 = NULL;
				s1 = secondPnt->getParent1();
				s2 = secondPnt->getParent2();
				if(s1 != NULL && s2 != NULL)
				{
					if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::ARC))
					{
					    if(s1->ShapeType != RapidEnums::SHAPETYPE::LINE)
					    {
							Shape * tmpshape = s1;
							s1 = s2;
							s2 = tmpshape;
					    }
						IsectPt* otherInpnt = NULL; 
						ISN_ITER item = ((ShapeWithList*)s1)->IsectPointsList->getList().begin(); 
						while(item  != ((ShapeWithList*)s1)->IsectPointsList->getList().end())
						{
							IsectPt* i = (IsectPt*)(*item).second;
							if(i->getParent1() != NULL && i->getParent2() != NULL)
							{
								if(compareboth<int>(i->getParent1()->getId(), i->getParent2()->getId(),s1->getId(), s2->getId()))
								{
									if(secondPnt->IsnPtId == i->IsnPtId)
									{
										item++;
										continue;
									}
									otherInpnt = i;
									break;
								}
							}
							item++;
		    			}
						if(otherInpnt != NULL)
						{
							double point1[2] = {((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY()};
							double intrPnts[4] = {secondPnt->getX(), secondPnt->getY(), otherInpnt->getX(), otherInpnt->getY()};
						    double dist1 = RMATH2DOBJECT->Pt2Pt_distance(point1, intrPnts);
							double dist2 = RMATH2DOBJECT->Pt2Pt_distance(point1, &intrPnts[2]);
					    	if((secondNear && (dist1 > dist2)) || (!secondNear && (dist1 < dist2)))
								ParentPoint2 = otherInpnt;
						}
					 }
			      }
			}
	}
	CalculateMeasurement(ParentPoint1, ParentPoint2);
}

DimBase* DimPointToPointDistance::Clone(int n)
{
	try
	{
		DimPointToPointDistance* Cdim = new DimPointToPointDistance();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0007", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimPointToPointDistance::CreateDummyCopy()
{
	try
	{
		DimPointToPointDistance* Cdim = new DimPointToPointDistance(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0008", __FILE__, __FUNCSIG__); return NULL; }
}

void DimPointToPointDistance::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0009", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, DimPointToPointDistance& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimPointToPointDistance"<<endl;		
		os <<"firstNear:"<< x.firstNear << endl;
		os <<"secondNear:"<< x.secondNear << endl;
		os <<"CurrentStatus:"<< x.CurrentStatus << endl;
		os <<"ParentPoint1:value"<<endl<< (*static_cast<Vector*>(x.ParentPoint1)) << endl;
		os <<"ParentPoint2:value"<<endl<< (*static_cast<Vector*>(x.ParentPoint2)) << endl;
		os<<"EndDimPointToPointDistance"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0010", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimPointToPointDistance& x)
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
			if(Tagname==L"DimPointToPointDistance")
			{
			while(Tagname!=L"EndDimPointToPointDistance")
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
						if(Tagname==L"firstNear")
						{
							if((Val)=="0")
							{
								x.firstNear = false;							
							}
							else						
							x.firstNear = true;
						}		
						else if(Tagname==L"secondNear")
						{
							if((Val)=="0")
							{
								x.secondNear = false;							
							}
							else						
							x.secondNear = true;
						}		
						else if(Tagname==L"CurrentStatus")
						{
							if((Val)=="0")
							{
								x.CurrentStatus = false;							
							}
							else						
							x.CurrentStatus = true;
						}	
						else if(Tagname==L"ParentPoint1")
						{
							x.ParentPoint1 = new Vector(0,0,0);
							is >> (*static_cast<Vector*>(x.ParentPoint1));
						}	
						else if(Tagname==L"ParentPoint2")
						{
							x.ParentPoint2 = new Vector(0,0,0);
							is >> (*static_cast<Vector*>(x.ParentPoint2));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0011", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimPointToPointDistance& x)
{
	try
	{
		int n; bool flag;
		wstring flagStr;
		is >> flagStr;
		std::string Str = RMATH2DOBJECT->WStringToString(flagStr);
		if(Str == "CheckNear")
		{
		    is >> x.firstNear;
		    is >> x.secondNear;
		    is >> x.CurrentStatus;
		}
		else
		{
			 x.firstNear = true;
		     x.secondNear = true;
			 checknear = false;
			if(Str == "1")
		    	x.CurrentStatus = true;
			else
				x.CurrentStatus = false;
		}
		x.ParentPoint1 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint1));
		x.ParentPoint2 = new Vector(0,0,0);
		is >> (*static_cast<Vector*>(x.ParentPoint2));
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT2PT0011", __FILE__, __FUNCSIG__);}
}