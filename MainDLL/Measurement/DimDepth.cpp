#include "StdAfx.h"
#include "DimDepth.h"
#include "..\Shapes\Shape.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

DimDepth::DimDepth(TCHAR* myname):DimBase(genName(myname))
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0001", __FILE__, __FUNCSIG__); }
}

DimDepth::DimDepth(bool simply):DimBase(false)
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0002", __FILE__, __FUNCSIG__); }
}

DimDepth::~DimDepth()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0002", __FILE__, __FUNCSIG__); }
}

void DimDepth::init()
{
	try
	{
		this->TwoBoxDepthMeasure(false);
		this->CurrentDMeasure(0);
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DEPTH;
		this->ValueZ1(0);
		this->ValueZ2(0);
		this->DepthValue(0);
		this->setDimension(DepthValue());
		this->dummyShapesType[0] = 0;
		this->dummyShapesType[1] = 0;
		IsValid(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimDepth::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		depthcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(depthcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimDepth::SetShapes(Shape *s1, Shape *s2)
{
	try
	{
		IsValid(false);
		ValueZ1(0); ValueZ2(0); DepthValue(0);
		this->setDimension(DepthValue());
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_DEPTH;
		if(TwoBoxDepthMeasure())
		{
			if(this->ParentShape1 == NULL)return;
			if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() < 2) return;

			if(this->CurrentDMeasure() == 0)
			{
				ValueZ1((((ShapeWithList*)ParentShape1)->PointsList->getList()[0])->Z);
				ValueZ2((((ShapeWithList*)ParentShape1)->PointsList->getList()[1])->Z);
			}
			else
			{
				ValueZ1((((ShapeWithList*)ParentShape1)->PointsList->getList()[2])->Z);
				ValueZ2((((ShapeWithList*)ParentShape1)->PointsList->getList()[3])->Z);
			}
			IsValid(true);
		}
		else
		{
			if(this->ParentShape1 == NULL && this->ParentShape2 == NULL) return;
			if(this->ParentShape1 != NULL)
			{
				if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() != 0)
					ValueZ1(getZFromDiffShapes(ParentShape1, dummyShapesType[0]));
				else
					ValueZ1(0);
			}
			if(this->ParentShape2 != NULL)
			{
				if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() != 0)
					ValueZ2(getZFromDiffShapes(ParentShape2, dummyShapesType[1]));
				else
					ValueZ2(0);
			}
			if(this->ParentShape1 != NULL && this->ParentShape2 != NULL)
				IsValid(true);
		}
		DepthValue(abs(ValueZ1() - ValueZ2()));
		this->setDimension(DepthValue());
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0005", __FILE__, __FUNCSIG__); }
}

void DimDepth::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0006", __FILE__, __FUNCSIG__); }
}

void DimDepth::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->TwoBoxDepthMeasure())
		{
			if(ParentShape1 == NULL) return;
			if(sender->getId() == ParentShape1->getId())
				SetShapes((Shape*)sender, NULL);
		}
		else
		{
			if(this->ParentShape1 == NULL && this->ParentShape2 == NULL) return;
			if(ParentShape1 != NULL)
			{
				if(((Shape*)sender)->getId() == ParentShape1->getId())
					SetShapes((Shape*)sender, ParentShape2);
			}
			if(ParentShape2 != NULL)
			{
				if(((Shape*)sender)->getId() == ParentShape2->getId())
					SetShapes(ParentShape1, (Shape*)sender);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0007", __FILE__, __FUNCSIG__); }
}

void DimDepth::UpdateMeasurement()
{
	SetShapes(ParentShape1, ParentShape2);
}

DimBase* DimDepth::Clone(int n)
{
	try
	{
		DimDepth* Cdim = new DimDepth();
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimDepth::CreateDummyCopy()
{
	try
	{
		DimDepth* Cdim = new DimDepth(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0009", __FILE__, __FUNCSIG__); return NULL; }
}

void DimDepth::CopyMeasureParameters(DimBase* Cdim)
{
	try
	{
		DimDepth* dim = (DimDepth*)Cdim;
		this->ValueZ1(dim->ValueZ1());
		this->ValueZ2(dim->ValueZ2());
		this->DepthValue(dim->DepthValue());
		this->TwoBoxDepthMeasure(dim->TwoBoxDepthMeasure());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0010", __FILE__, __FUNCSIG__); }
}

int DimDepth::depthcnt = 0;
void DimDepth::reset()
{
	depthcnt = 0;
}

double DimDepth::getZFromDiffShapes(Shape *cShape, int type)
{
	try
	{
	   if(((ShapeWithList*)cShape)->PointsList->getList().empty()) return 0;
	   if(type == 3) return (((ShapeWithList*)cShape)->PointsList->getList().begin()->second->Z);
	   FramegrabBase *Cbaseaction = NULL;
	   if(PPCALCOBJECT->IsPartProgramRunning())
	   {
		   for(RC_ITER pCaction = PPCALCOBJECT->getPPActionlist().getList().begin(); pCaction != PPCALCOBJECT->getPPActionlist().getList().end(); pCaction++)
		   {
				if(((RAction*)(*pCaction).second)->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) continue;
				if(((AddPointAction*)(*pCaction).second)->getShape()->getId() != cShape->getId()) continue;						
				Cbaseaction = ((AddPointAction*)(*pCaction).second)->getFramegrab();
		   }
		   if(Cbaseaction == NULL) return 0;
	   }
	   else
		   Cbaseaction = ((AddPointAction*)(*((ShapeWithList*)cShape)->PointAtionList.begin()))->getFramegrab();
	   int count = 0;
	   double avgZ = 0, *pnts = NULL, targetP[6] = {0}, curP[3] = {Cbaseaction->PointDRO.getX() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), Cbaseaction->PointDRO.getY() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), 0};
	   pnts = new double[((ShapeWithList*)cShape)->PointsList->getList().size() * 3];
	   for(map<int,SinglePoint*>::iterator It = ((ShapeWithList*)cShape)->PointsList->getList().begin(); It != ((ShapeWithList*)cShape)->PointsList->getList().end(); It++)
	   {
		  pnts[count++] = (*It).second->X;
		  pnts[count++] = (*It).second->Y;
		  pnts[count++] = (*It).second->Z;
		  avgZ += (*It).second->Z;
	   }
	   count /= 3;
	   if(count == 0) 
	   {
		   if( pnts != NULL){ delete [] pnts;  pnts = NULL;}
		   return 0;
	   }
	   avgZ /= count;
	   if(type == 0)
	   {
			double  param[5] = {0}; 
			if(count == 3) //Call the 3 point Best fit..
				BESTFITOBJECT->Plane_3Pt_2(pnts, &param[0]);
			else	// If the point size is greater than 3...
				BESTFITOBJECT->Plane_BestFit(pnts, count, &param[0], MAINDllOBJECT->SigmaModeFlag());
			param[3] = -1 * param[3];
			double curZ = RMATH3DOBJECT->Distance_Point_PlaneAlongSeletedAxis(curP, param, 2);
			double projectPt[3] = { 0 };
			if (RMATH3DOBJECT->Projection_Point_on_Plane(curP, param, projectPt))
				curZ = projectPt[2];

			if( pnts != NULL){ delete [] pnts;  pnts = NULL;}
			return curZ;
	   }
	   else if(type == 1)
	   {
			 double param[4] = {0}, currentline[6] = {curP[0], curP[1], 0, 0, 0, 1}; 
			 bool flag = false;
			if(count == 4)
				flag = BESTFITOBJECT->Sphere_4Pt(pnts, &param[0]);
			else
				flag = BESTFITOBJECT->Sphere_BestFit(pnts, count, &param[0], MAINDllOBJECT->SigmaModeFlag());
			if(!flag) return avgZ;
			int intpnt = RMATH3DOBJECT->Intersection_Line_Sphere(currentline, param, targetP);
			if(intpnt == 0)
			   return avgZ;
			else if(intpnt == 1)
			   return targetP[2];
			else if(intpnt == 2)
			{
				if(abs(avgZ - targetP[2]) < abs(avgZ - targetP[5]))
					return targetP[2];
				else
					return targetP[5];
			}
	   }
	   else if(type == 2)
	   {
		   double cylParam[7] = {0}, endpnts[6] = {0}, minr = 0, maxr = 0, currentline[6] = {curP[0], curP[1], 0, 0, 0, 1};
		   bool flag = BESTFITOBJECT->Cylinder_BestFit(pnts, count, &cylParam[0], 1000000, MAINDllOBJECT->OutlierFilteringForLineArc());
		   if(!flag) return 0;
		   HELPEROBJECT->CalculateEndPoints_3DLineBestfit(count, pnts, &cylParam[0], &endpnts[0], &endpnts[3], &minr, &maxr);
		   int intpnt = RMATH3DOBJECT->Intersection_Line_Cylinder(currentline, cylParam, targetP);
		   if(intpnt == 0)
			   return avgZ;
		   else if(intpnt == 1)
		   {
				double ProjctnPt[3] = {0};
				RMATH3DOBJECT->Projection_Point_on_Line(targetP, cylParam, ProjctnPt);
				if(RMATH3DOBJECT->Checking_Point_within_Block(ProjctnPt, endpnts, 2))
				{
					return targetP[2];
				}
		   }
		   else if(intpnt == 2)
		   {
				list<double> possibleZ;
				for(int k = 0; k < intpnt; k++)
				{
					double ProjctnPt[3] = {0};
					RMATH3DOBJECT->Projection_Point_on_Line(&targetP[3 * k], cylParam, ProjctnPt);
					if(RMATH3DOBJECT->Checking_Point_within_Block(ProjctnPt, cylParam, 2))
					{
						possibleZ.push_back(targetP[3*k + 2]);
					}
				}
				if(possibleZ.empty())
					return avgZ;
				else if(possibleZ.size() == 1)
					return *(possibleZ.begin());
				else if(possibleZ.size() == 2)
				{
					list<double>::iterator It =  possibleZ.begin();
					double dist1 = *It++, dist2 = *It;
					if(abs(avgZ - dist1) < abs(avgZ - dist2))
						return dist1;
					else
						return dist2;
				}
		   }
	   }
	   if( pnts != NULL){ delete [] pnts;  pnts = NULL;}
	   return 0;
	}
	catch(...)
	{
		  return 0;
	}
}

wostream& operator<<( wostream& os, DimDepth& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimDepth"<<endl;
		os <<"ValueZ1:"<< x.ValueZ1() << endl;
		os <<"ValueZ2:"<< x.ValueZ2() << endl;
		os <<"TwoBoxDepthMeasure:"<< x.TwoBoxDepthMeasure() << endl;
		os <<"CurrentDMeasure:"<< x.CurrentDMeasure() << endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		if(!x.TwoBoxDepthMeasure())
			os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os <<"dummyShapesType0:"<< x.dummyShapesType[0] << endl;
		os <<"dummyShapesType1:"<< x.dummyShapesType[1] << endl;
		os<<"EndDimDepth"<<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimDepth& x)
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
			if(Tagname==L"DimDepth")
			{
				while(Tagname!=L"EndDimDepth")
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
						if(Tagname==L"ValueZ1")
						{
							x.ValueZ1(atoi((const char*)(Val).c_str()));
						}		
						else if(Tagname==L"ValueZ2")
						{
							x.ValueZ2(atoi((const char*)(Val).c_str()));
						}	
						else if(Tagname==L"TwoBoxDepthMeasure")
						{
							if(Val=="0")
							{
								x.TwoBoxDepthMeasure(false);				
							}
							else		
							{
								x.TwoBoxDepthMeasure(true);							
							}
						}	
						else if(Tagname==L"CurrentDMeasure")
						{
								x.CurrentDMeasure(atoi((const char*)(Val).c_str()));
						}	
						else if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}	
						else if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}	
						else if(Tagname==L"dummyShapesType0")
						{
							x.dummyShapesType[0] = atoi((const char*)(Val).c_str());
						}	
						else if(Tagname==L"dummyShapesType1")
						{
							x.dummyShapesType[1] = atoi((const char*)(Val).c_str());
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
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimDepth& x)
{
	try
	{
		bool f; double d; int n;
		is >> d; x.ValueZ1(d);
		is >> d; x.ValueZ2(d);
		is >> f; x.TwoBoxDepthMeasure(f);
		is >> n; x.CurrentDMeasure(n);
		is >> n;
		x.ParentShape1 =(Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		if(!x.TwoBoxDepthMeasure())
		{
			is >> n;
			x.ParentShape2 = (Shape*) MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		}
		x.DepthValue(abs(x.ValueZ1() -  x.ValueZ2()));
		is >> x.dummyShapesType[0];
		is >> x.dummyShapesType[1];
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMDPTH0012", __FILE__, __FUNCSIG__); }
}