#include "StdAfx.h"
#include "IntersectionShape.h"
#include "Plane.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Engine\SnapPointCollection.h"

IntersectionShape::IntersectionShape(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

IntersectionShape::IntersectionShape(bool simply):ShapeWithList(false)
{
	init();
}

IntersectionShape::~IntersectionShape()
{
	if(intersectPts != NULL) 
		free(intersectPts);
	intersectPts = NULL;
}

void IntersectionShape::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::INTERSECTIONSHAPE;
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->centerOfGravity), 3), 3);
		intersectPts = NULL;
		pntscount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0001", __FILE__, __FUNCSIG__); }
}

TCHAR* IntersectionShape::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10]; int Cnt;
		Cnt = shapenumber++;
		Cnt++;
		_itot_s(Cnt, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0002", __FILE__, __FUNCSIG__); return name; }
}

bool IntersectionShape::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	for(int i = 0; i < pntscount; i++)
	{
		double pnt[6] = {intersectPts[3 * i], intersectPts[3 * i + 1], intersectPts[3 * i + 2], corner1.getX(),corner1.getY(), corner1.getZ()};
        if(RMATH2DOBJECT->Pt2Pt_distance(pnt, &pnt[3]) <= Tolerance)	
	      return true;
	}
	return false;
}

bool IntersectionShape::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	try{ return false; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0003", __FILE__, __FUNCSIG__); }
}

bool IntersectionShape::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	for(int i = 0; i < pntscount; i++)
	{
		double pnt[3] = {intersectPts[3 * i], intersectPts[3 * i + 1], intersectPts[3 * i + 2]};
        if(RMATH3DOBJECT->Distance_Point_Line(pnt, SelectionLine) <= Tolerance)	
	      return true;
	}
	return false;
}

void IntersectionShape::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		GRAFIX->drawPoints(intersectPts, pntscount, false, 3, FGPOINTSIZE);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0004", __FILE__, __FUNCSIG__); }
}

void IntersectionShape::ResetShapeParameters()
{
}

void IntersectionShape::UpdateForParentChange(BaseItem* sender)
{
	try
	{
	//	UpdateBestFit();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0006", __FILE__, __FUNCSIG__); }
}

Shape* IntersectionShape::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("IS");
		else
			myname = _T("dIS");
		IntersectionShape* CShape = new IntersectionShape((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0007", __FILE__, __FUNCSIG__); return NULL; }
}

void IntersectionShape::GetShapeCenter(double *cPoint)
{
	cPoint[0] = centerOfGravity.getX();
	cPoint[1] = centerOfGravity.getY();
	cPoint[2] = centerOfGravity.getZ();
}

Vector* IntersectionShape::getCenterOfGravity()
{
	return &centerOfGravity;
}

Shape* IntersectionShape::CreateDummyCopy()
{
	try
	{
		IntersectionShape* CShape = new IntersectionShape(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0008", __FILE__, __FUNCSIG__); return NULL; }
}

void IntersectionShape::CopyShapeParameters(Shape* s)
{
	try
	{
		if(intersectPts != NULL) { free(intersectPts); intersectPts = NULL;}
		pntscount = 0;
		IntersectionShape* Csph = (IntersectionShape*)s;
		this->centerOfGravity.set(*Csph->getCenterOfGravity());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINT0009", __FILE__, __FUNCSIG__); }
}

void IntersectionShape::getIntersectionPoints(Shape *ParentShape1, Shape *ParentShape2)
{
	try
	{
      if(intersectPts != NULL){free(intersectPts); intersectPts = NULL;}
      list<double> pointonShape, intersectionPoint;
      double increamentFactor = 0.09, avgZ = 0;

      if(compareboth<int>(ParentShape1->ShapeType, ParentShape2->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::CYLINDER))
      {
    	   double planeParam[4] = {0}, planeCenter[3] = {0}, cylinderParam[7] = {0};
		   double ThePoints[6] = {((Cylinder*)ParentShape1)->getCenter1()->getX(), ((Cylinder*)ParentShape1)->getCenter1()->getY(), ((Cylinder*)ParentShape1)->getCenter1()->getZ(), ((Cylinder*)ParentShape1)->getCenter2()->getX(), ((Cylinder*)ParentShape1)->getCenter2()->getY(), ((Cylinder*)ParentShape1)->getCenter2()->getZ()};
		   ((Cylinder*)ParentShape1)->getParameters(cylinderParam);
		   ((Plane*)ParentShape2)->GetShapeCenter(planeCenter);
		   ((Plane*)ParentShape2)->getParameters(planeParam);
		   RMATH3DOBJECT->Intersection_Cylinder_Plane(cylinderParam, ThePoints, planeParam, planeCenter, 0, 2 * M_PI, MAINDllOBJECT->getWindow(1).getUppX(), &intersectionPoint);
	   }
	   else if(compareboth<int>(ParentShape1->ShapeType, ParentShape2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::CYLINDER))
	   {
		   double cylinderLength = 0, coneLength = 0, cylinderParam[7] = {0}, coneParam[8] = {0};
		   ((Cylinder*)ParentShape1)->getParameters(cylinderParam);
		   ((Cone*)ParentShape2)->getParameters(coneParam);
		   double cylinderCenters[6] = {((Cylinder*)ParentShape1)->getCenter1()->getX(), ((Cylinder*)ParentShape1)->getCenter1()->getY(), ((Cylinder*)ParentShape1)->getCenter1()->getZ(), ((Cylinder*)ParentShape1)->getCenter2()->getX(), ((Cylinder*)ParentShape1)->getCenter2()->getY(), ((Cylinder*)ParentShape1)->getCenter2()->getZ()};
	       double coneCenters[6] = {((Cone*)ParentShape2)->getCenter1()->getX(), ((Cone*)ParentShape2)->getCenter1()->getY(), ((Cone*)ParentShape2)->getCenter1()->getZ(), ((Cone*)ParentShape2)->getCenter2()->getX(), ((Cone*)ParentShape2)->getCenter2()->getY(), ((Cone*)ParentShape2)->getCenter2()->getZ()};
		   RMATH3DOBJECT->Intersection_Cylinder_Cone(cylinderParam, coneParam, cylinderCenters, coneCenters, 0, 2 * M_PI, 0.005, 0.3, MAINDllOBJECT->getWindow(1).getUppX(), &intersectionPoint);
	   }
       else if(compareboth<int>(ParentShape1->ShapeType, ParentShape2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::PLANE))
	   {
           double ProjctnPt[3] = {0}, shiftedPoint[3] = {0}, intersectionPnts[6] = {0}, ang = 0, currentline[6] = {0}, rotationOrigin[6] ={0}, coneParam[8] = {0}, planeCenter[3] = {0}, planeParam[4] = {0};
		   double ThePoints[6] = {((Cone*)ParentShape1)->getCenter1()->getX(), ((Cone*)ParentShape1)->getCenter1()->getY(), ((Cone*)ParentShape1)->getCenter1()->getZ(), ((Cone*)ParentShape1)->getCenter2()->getX(), ((Cone*)ParentShape1)->getCenter2()->getY(), ((Cone*)ParentShape1)->getCenter2()->getZ()};
		   int s1[2] = {3,3}, s2[2] = {3,1};
		   ((Cone*)ParentShape1)->getParameters(coneParam);
		   ((Plane*)ParentShape2)->GetShapeCenter(planeCenter);
		   ((Plane*)ParentShape2)->getParameters(planeParam);
		   RMATH3DOBJECT->Intersection_Cylinder_Plane(coneParam, ThePoints, planeParam, planeCenter, 0, 2 * M_PI, MAINDllOBJECT->getWindow(1).getUppX(), &intersectionPoint);
	   }
	   else if(compareboth<int>(ParentShape1->ShapeType, ParentShape2->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::SPHERE))
	   {
		   double cylinderLength = 0, coneLength = 0, sphereParam[4] = {0}, coneParam[8] = {0};
		   ((Cone*)ParentShape1)->getParameters(coneParam);
		   ((Sphere*)ParentShape2)->getParameters(sphereParam);
		   double coneCenters[6] = {((Cone*)ParentShape1)->getCenter1()->getX(), ((Cone*)ParentShape1)->getCenter1()->getY(), ((Cone*)ParentShape1)->getCenter1()->getZ(), ((Cone*)ParentShape1)->getCenter2()->getX(), ((Cone*)ParentShape1)->getCenter2()->getY(), ((Cone*)ParentShape1)->getCenter2()->getZ()};
		   RMATH3DOBJECT->Intersection_Sphere_Cone(sphereParam, coneParam, coneCenters, 0, 2 * M_PI, 0.005, 0.01, MAINDllOBJECT->getWindow(1).getUppX(), &intersectionPoint, &avgZ);
	   }
	   else
	   {
		  return;
	   }
	   intersectPts = new double[intersectionPoint.size()];
	   pntscount = 0;
	   wifstream ifile;
	   ifile.open("E:\\IntersectionShapeOption.txt");
		int factor = -1;
		if(!ifile.fail())
		{
			ifile >> factor;
		}
		ifile.close();
		avgZ /= (intersectionPoint.size() / 3);
		if(factor == -1)
		{
		   for(list<double>::iterator It = intersectionPoint.begin(); It != intersectionPoint.end();)
		   {
	   		   intersectPts[pntscount++] = *It++;
			   intersectPts[pntscount++] = *It++;
			   intersectPts[pntscount++] = *It++;
		   }
		}
		else if(factor == 0)
		{
		   for(list<double>::iterator It = intersectionPoint.begin(); It != intersectionPoint.end();)
		   {
			   double tmp[3] = {0};
			   tmp[0] = *It++;
			   tmp[1] = *It++;
			   tmp[2] = *It++;
			   if(tmp[2] > avgZ)
			   {
				   intersectPts[pntscount++] = tmp[0];
			       intersectPts[pntscount++] = tmp[1];
			       intersectPts[pntscount++] = tmp[2];
			   }
		   }
		}
		else if(factor == 1)
		{
		   for(list<double>::iterator It = intersectionPoint.begin(); It != intersectionPoint.end();)
		   {
			   double tmp[3] = {0};
			   tmp[0] = *It++;
			   tmp[1] = *It++;
			   tmp[2] = *It++;
			   if(tmp[2] < avgZ)
			   {
				   intersectPts[pntscount++] = tmp[0];
			       intersectPts[pntscount++] = tmp[1];
			       intersectPts[pntscount++] = tmp[2];
			   }
		   }
		}
       pntscount /= 3;
	   double center[3] = {0};
	   if(pntscount > 0)
	   {
		   for(int j = 0; j < pntscount; j++)
		   {
			 center[0] += intersectPts[3 * j];
			 center[1] += intersectPts[3 * j + 1];
			 center[2] += intersectPts[3 * j + 2];
		   }
		   center[0] /= pntscount;
		   center[1] /= pntscount;
		   center[2] /= pntscount;
		   centerOfGravity.set(center[0], center[1], center[2]);
	   }
	   this->IsValid(true);
	}
	catch(...)
	{
	    return;
	}
}

int IntersectionShape::shapenumber = 0;

void IntersectionShape::reset()
{
	shapenumber = 0;
}

void IntersectionShape::Transform(double* transform)
{
}

void IntersectionShape::UpdateBestFit()
{
	try
	{
	    if(this->getParents().size() > 1)
		{
			list<BaseItem*>::iterator It = this->getParents().begin();
			Shape *parent1 = (Shape*)(*It);
			It++;
			Shape *parent2 = (Shape*)(*It);
			getIntersectionPoints(parent1, parent2);
			this->notifyAll(ITEMSTATUS::DATACHANGED, this);
		}
	}
	catch(...)
	{
	
	}
}

void IntersectionShape::drawGDntRefernce()
{

}

void IntersectionShape::Translate(Vector& Position)
{
	centerOfGravity += Position;
}

void IntersectionShape::AlignToBasePlane(double* trnasformM)
{
}

void IntersectionShape::UpdateEnclosedRectangle()
{
}

bool IntersectionShape::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
    return true;
}

wostream& operator<<( wostream& os, IntersectionShape& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "IntersectionShape" << endl;
		os << "CenterOfGravity:value" << endl << (*static_cast<Vector*>(&x.centerOfGravity)) << endl;
		os << "EndIntersectionShape" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("INS0019", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>( wistream& is, IntersectionShape& x )
{
	try
	{
		is >> (*(Shape*)&x);
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"IntersectionShape")
		{
	    	while(Tagname!=L"EndIntersectionShape")
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
					if(Tagname==L"CenterOfGravity")
					{
						is >> (*static_cast<Vector*>(x.getCenterOfGravity()));		
					}
				}
			}
		}		
		return is;
	}
	catch(...){ MAINDllOBJECT->PPLoadSuccessful(false);	MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0020", __FILE__, __FUNCSIG__); return is; }

}
