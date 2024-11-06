#include "stdafx.h"
#include "Perimeter.h"
#include "Circle.h"
#include "Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

Perimeter::Perimeter(TCHAR* myname):ShapeWithList(genName(myname))
{
    init();
}

Perimeter::Perimeter(bool flag):ShapeWithList(false)
{
	init();
	this->AddFramgrabPts = flag;
}

Perimeter::~Perimeter()
{
	deleteAllShapes();
}

void Perimeter::deleteAllShapes(bool DeleteShape)
{
	try
	{
		while (PerimeterShapecollection.size() != 0)
		{
			PSC_ITER i = PerimeterShapecollection.begin();
			Shape* Csh = (*i).second;
			PerimeterShapecollection.erase(Csh->getId());
			if(DeleteShape)
				delete Csh;
			else
			{
				((ShapeWithList*)Csh)->PointsList->deleteAll();
				((ShapeWithList*)Csh)->PointsListOriginal->deleteAll();
			}
		}

		while (TempShapecollection.size() != 0)
		{
			PSC_ITER i = TempShapecollection.begin();
			Shape* Csh = (*i).second;
			TempShapecollection.erase(Csh->getId());
			delete Csh;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0001", __FILE__, __FUNCSIG__); }
}

void Perimeter::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::PERIMETER;
		this->TotalArea(0); this->TotalPerimeter(0); this->Diameter(0);
		this->IsValid(true); this->AddFramgrabPts = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0002", __FILE__, __FUNCSIG__); }
}

TCHAR* Perimeter::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		shapenumber++;
		_itot_s(shapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0003", __FILE__, __FUNCSIG__); }
}

void Perimeter::Translate(Vector& Position)
{
	try
	{
		for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
		{
			Shape* Cshape = (*i).second;
			Cshape->Translate(Position);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0004", __FILE__, __FUNCSIG__); }
}

void Perimeter::Transform(double* transform)
{
	try
	{
		for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
		{
			Shape* Cshape = (*i).second;
			Cshape->Transform(transform);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0005", __FILE__, __FUNCSIG__); }
}

void Perimeter::AlignToBasePlane(double* trnasformM)
{
	try
	{
		for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
		{
			Shape* Cshape = (*i).second;
			Cshape->AlignToBasePlane(trnasformM);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0006", __FILE__, __FUNCSIG__); }
}

void Perimeter::GetShapeCenter(double *cPoint)
{
	
}

void Perimeter::UpdateEnclosedRectangle()
{
	try
	{
		PointCollection PtColl;
		for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
		{
			Shape* Cshape = (*i).second;
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double line_endpts[6] = {0};
				((Line*)Cshape)->getEndPoints(line_endpts);
				PtColl.Addpoint(new SinglePoint(line_endpts[0], line_endpts[1], line_endpts[2]));
				PtColl.Addpoint(new SinglePoint(line_endpts[3], line_endpts[4], line_endpts[5]));
			}
			else if(Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				Vector Point1 = *((Circle*)Cshape)->getendpoint1();
				Vector Point2 = *((Circle*)Cshape)->getendpoint2();
				Vector Point3 = *((Circle*)Cshape)->getMidPoint();
				PtColl.Addpoint(new SinglePoint(Point1.getX(), Point1.getY(), Point1.getZ()));
				PtColl.Addpoint(new SinglePoint(Point2.getX(), Point2.getY(), Point2.getZ()));
				PtColl.Addpoint(new SinglePoint(Point3.getX(), Point3.getY(), Point3.getZ()));
			}
		}
		int s1[2] = {4, 4}, s2[2] = {4, 1}, n = PtColl.Pointscount();	
		double *TransformePpoint = NULL;
		TransformePpoint = new double[n * 2 + 1];
		int cnt = 0;
		for( PC_ITER Citem = PtColl.getList().begin(); Citem != PtColl.getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			TransformePpoint[cnt++] = Spt->X;
			TransformePpoint[cnt++] = Spt->Y;
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{
			int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			double TransMatrix[9] = {0},  *Temporary_point = NULL;	
			Temporary_point = new double[n * 3];
			cnt = 0;
			for( PC_ITER Citem = PtColl.getList().begin(); Citem != PtColl.getList().end(); Citem++)
			{
				SinglePoint* Spt = (*Citem).second;
				Temporary_point[cnt++] = Spt->X;
				Temporary_point[cnt++] = Spt->Y;
				Temporary_point[cnt++] = Spt->Z;
			}
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			for(int i = 0; i < n; i++)
				RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, &Temporary_point[3 * i], Order2, &TransformePpoint[2 * i]);	
			delete [] Temporary_point;
		}
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, ShapeLeftTop, ShapeRightBottom);
		delete [] TransformePpoint;
		PtColl.deleteAll();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0007", __FILE__, __FUNCSIG__); }
}

void Perimeter::AddShape(Shape* Csh, bool SetRelation)
{
	try
	{
		PerimeterShapecollection[Csh->getId()] = Csh;
		if(SetRelation && !this->AddFramgrabPts)
		{
			Csh->addGroupParent(this);
			this->addGroupChild(Csh);
			this->UpdateEnclosedRectangle();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0008", __FILE__, __FUNCSIG__); }
}

void Perimeter::RemoveShape(Shape* Csh)
{
	try
	{
		bool ShapePresent = false;
		for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
		{
			Shape* Cshape = (*i).second;
			if(Cshape->getId() == Csh->getId())
			{
				ShapePresent = true;
				break;
			}
		}
		if(ShapePresent)
		{
			PerimeterShapecollection.erase(Csh->getId());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0009", __FILE__, __FUNCSIG__); }
}

void Perimeter::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		for(PSC_ITER Citem = PerimeterShapecollection.begin(); Citem != PerimeterShapecollection.end(); Citem++)
			(*Citem).second->drawShape(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0010", __FILE__, __FUNCSIG__); }
}

void Perimeter::drawGDntRefernce()
{
}

void Perimeter::ResetShapeParameters()
{
	try
	{
		this->TotalArea(0); this->TotalPerimeter(0); this->Diameter(0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI00011", __FILE__, __FUNCSIG__); }
}

Vector* Perimeter::getCenter()
{
	return &(this->Pcenter);
}

bool Perimeter::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		bool ShHighighted = false;
		for(PSC_ITER item = PerimeterShapecollection.begin(); item != PerimeterShapecollection.end(); item++)
		{
			Shape* Cshape = ((Shape*)(*item).second);
			if(Cshape->Shape_IsInWindow(corner1, Tolerance))
			{
				ShHighighted = true;
				break;
			}
		}
		return ShHighighted;
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0011", __FILE__, __FUNCSIG__); return false; }
}

bool Perimeter::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool Perimeter::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
		bool ShHighighted = false;
		for(PSC_ITER item = PerimeterShapecollection.begin(); item != PerimeterShapecollection.end(); item++)
		{
			Shape* Cshape = ((Shape*)(*item).second);
			if(Cshape->Shape_IsInWindow(SelectionLine, Tolerance))
			{
				ShHighighted = true;
				break;
			}
		}
		return ShHighighted;
	}
	catch(...) {MAINDllOBJECT->SetAndRaiseErrorMessage("PERI00012", __FILE__, __FUNCSIG__); return false; }
}

void Perimeter::UpdateBestFit()
{
	try
	{
		if(!this->AddFramgrabPts)
		{
			if(PerimeterShapecollection.size() < 1) return;
		}
		else
		{
			if(this->PointsList->Pointscount() < 2) return;
			deleteAllShapes(true);
			HELPEROBJECT->CalculateAreaOfShapeUsingOneShot(this);
		}
		list<Shape*> TempShapelist;
		int Cnt = 0;
		int pointsCnt = (int)this->PerimeterShapecollection.size();
		double* tempPoints = (double*)malloc(sizeof(double) *  pointsCnt * 2);
		for(PSC_ITER Citem = this->PerimeterShapecollection.begin(); Citem != this->PerimeterShapecollection.end(); Citem++)
		{
			Shape* Cpshape = (*Citem).second;
			TempShapelist.push_back(Cpshape);
			if(Cpshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				tempPoints[Cnt++] = ((Line*)Cpshape)->getMidPoint()->getX();
				tempPoints[Cnt++] = ((Line*)Cpshape)->getMidPoint()->getY();
			}
			else
			{
				tempPoints[Cnt++] = ((Circle*)Cpshape)->getMidPoint()->getX();
				tempPoints[Cnt++] = ((Circle*)Cpshape)->getMidPoint()->getY();
			}
		}
		double ans[3] = {0};
		//Calculate the best fit circle....//
		BESTFITOBJECT->Circle_BestFit(tempPoints, pointsCnt, &ans[0], false);
		free(tempPoints);
		Pcenter.set(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
		
		double TArea = 0;
		bool ClosedLoop = HELPEROBJECT->CheckShapeListCloseLoop(&TempShapelist);
		TArea = HELPEROBJECT->CalculateAreaofShapeList(&TempShapelist);
		this->TotalArea(TArea);
		double ShPerimeter = HELPEROBJECT->CalculatePerimeterOfShapeList(&TempShapelist);
		this->TotalPerimeter(ShPerimeter);
		this->Diameter(ShPerimeter/M_PI);
		this->notifyAll(ITEMSTATUS::DATACHANGED, this);	
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("PERI00013", __FILE__, __FUNCSIG__);
	}
}

bool Perimeter::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		PointCollection PtColl;
		for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
		{
			Shape* Cshape = (*i).second;
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double line_endpts[6] = {0};
				((Line*)Cshape)->getEndPoints(line_endpts);
				PtColl.Addpoint(new SinglePoint(line_endpts[0], line_endpts[1], line_endpts[2]));
				PtColl.Addpoint(new SinglePoint(line_endpts[3], line_endpts[4], line_endpts[5]));
			}
			else if(Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				Vector Point1 = *((Circle*)Cshape)->getendpoint1();
				Vector Point2 = *((Circle*)Cshape)->getendpoint2();
				Vector Point3 = *((Circle*)Cshape)->getMidPoint();
				PtColl.Addpoint(new SinglePoint(Point1.getX(), Point1.getY(), Point1.getZ()));
				PtColl.Addpoint(new SinglePoint(Point2.getX(), Point2.getY(), Point2.getZ()));
				PtColl.Addpoint(new SinglePoint(Point3.getX(), Point3.getY(), Point3.getZ()));
			}
		}
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		int n = PtColl.Pointscount();
		double *TransformePpoint = NULL, *tempPoint = NULL;
		TransformePpoint = new double[(n + 1) * 2];
		tempPoint = new double[n * 4];
		int cnt = 0;
		for( PC_ITER Citem = PtColl.getList().begin(); Citem != PtColl.getList().end(); Citem++)
		{
			SinglePoint* Spt = (*Citem).second;
			tempPoint[cnt++] = Spt->X;
			tempPoint[cnt++] = Spt->Y;
			tempPoint[cnt++] = Spt->Z;
			tempPoint[cnt++] = 1;
		}	
		 for(int i = 0; i < n; i++)
			RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &tempPoint[4 * i], &s2[0], &TransformePpoint[2 * i]);
		RMATH2DOBJECT->GetTopLeftAndRightBottom(&TransformePpoint[0], n, 2, Lefttop, Rightbottom);
		delete [] TransformePpoint;
		delete [] tempPoint;
		PtColl.deleteAll();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0014", __FILE__, __FUNCSIG__); return false; }
}

void Perimeter::UpdateForParentChange(BaseItem* sender)
{
}

Shape* Perimeter::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("Peri");
		else
			myname = _T("dPeri");
		Perimeter* CShape = new Perimeter((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0015", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* Perimeter::CreateDummyCopy()
{
	try
	{
		Perimeter* CShape = new Perimeter();
		if(!this->AddFramgrabPts)
		{
			for (PSC_ITER i = this->PerimeterShapecollection.begin(); i != this->PerimeterShapecollection.end(); i++)
			{
				Shape* Shp = (*i).second;
				CShape->AddShape(Shp->CreateDummyCopy());
			}
		}
		CShape->AddFramgrabPts = this->AddFramgrabPts;
		CShape->TotalArea(this->TotalArea());
		CShape->TotalPerimeter(this->TotalPerimeter());
		CShape->Diameter(this->Diameter());
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0016", __FILE__, __FUNCSIG__); return NULL; }
}

void Perimeter::CopyShapeParameters(Shape* s)
{
	try
	{
		if(!((Perimeter*)s)->AddFramgrabPts)
		{
			for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
			{
				ShapeWithList* Shp = (ShapeWithList*)(*i).second;
				Shp->PointsList->deleteAll();
				Shp->PointsListOriginal->deleteAll();
			}
		
			for (PSC_ITER i = ((Perimeter*)s)->PerimeterShapecollection.begin(); i != ((Perimeter*)s)->PerimeterShapecollection.end(); i++)
			{
				Shape* Shp = (*i).second;
				this->PerimeterShapecollection[Shp->getId()]->CopyShapeParameters(Shp);
			}
		}
		this->AddFramgrabPts = ((Perimeter*)s)->AddFramgrabPts;
		this->TotalArea(((Perimeter*)s)->TotalArea());
		this->TotalPerimeter(((Perimeter*)s)->TotalPerimeter());
		this->Diameter(((Perimeter*)s)->Diameter());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0017", __FILE__, __FUNCSIG__); }
}

int Perimeter::shapenumber = 0;
void Perimeter::reset()
{
    shapenumber = 0;
}

void Perimeter::SetParentChildRelation()
{
	try
	{
		if(!this->AddFramgrabPts)
		{
			for (PSC_ITER i = PerimeterShapecollection.begin(); i != PerimeterShapecollection.end(); i++)
			{
				Shape* Cshape = (*i).second;
				this->addGroupChild(Cshape);
				Cshape->addGroupParent(this);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0018", __FILE__, __FUNCSIG__); }
}

wostream& operator<<( wostream& os, Perimeter& x )
{	
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "Perimeter" << endl;
		os << "TotalArea:" << x.TotalArea() << endl;
		os << "TotalPerimeter:" << x.TotalPerimeter() << endl;
		os << "Diameter:" << x.Diameter() << endl;
		os << "AddFramgrabPts:" << x.AddFramgrabPts <<endl;	
		if(!x.AddFramgrabPts)
		{
			os << "PerimeterShapecollection:" << endl;
			for (PSC_ITER i = x.PerimeterShapecollection.begin(); i != x.PerimeterShapecollection.end(); i++)
			{
				Shape* Shp = (*i).second;
				switch(Shp->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
					os << endl << L"shape" << endl;
					os << (*(Line*)Shp);
					os << endl;
					break;
				case RapidEnums::SHAPETYPE::ARC:
					os << endl << L"shape" << endl;
					os << (*(Circle*)Shp);
					os << endl;
					break;
				}
			}
			os << "EndPerimeterShapecollection" << endl;
		}
		os << "EndPerimeter" << endl;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0019", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, Perimeter& x )
{
	try
	{
		is >> (*(Shape*)&x);
		std::wstring Tagname;
		is >> Tagname;
		if(Tagname==L"Perimeter")
		{
			x.ShapeType = RapidEnums::SHAPETYPE::PERIMETER;
			while(Tagname!=L"EndPerimeter")
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
					if(Tagname==L"TotalArea")
					{
						 x.TotalArea(atof((const char*)(Val).c_str()));
					}
					else if(Tagname==L"TotalPerimeter")
					{
						 x.TotalPerimeter(atof((const char*)(Val).c_str()));
					}
					else if(Tagname==L"Diameter")
					{
						 x.Diameter(atof((const char*)(Val).c_str()));
					}
					else if(Tagname==L"AddFramgrabPts")
					{
						if(Val=="0")
						{
							x.AddFramgrabPts = false;
						}
						else
							x.AddFramgrabPts = true;
					}
					else if(Tagname==L"PerimeterShapecollection")
					{
						while(Tagname!=L"EndPerimeterShapecollection")
						{			
							is >> Linestr;
							if(is.eof())
							{						
								MAINDllOBJECT->PPLoadSuccessful(false);
								break;
							}
						    ColonIndx = Linestr.find(':');
							if(ColonIndx == -1)
								Tagname = Linestr;
							else
								Tagname = Linestr.substr(0, ColonIndx);				
							if(Tagname==L"shape")
							{
								int cn = 0;	
								std::wstring cnstr;
								is >> cnstr;
								ColonIndx = cnstr.find(':');
								if(ColonIndx != -1)
								{			
									TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
									std::string Val1(TagVal.begin(), TagVal.end() );
									cn = atoi((const char*)Val1.c_str());
									RapidEnums::SHAPETYPE temp = RapidEnums::SHAPETYPE(cn);
									Shape *item;
									bool enteringFlag = false;
									switch(temp)
									{
										case RapidEnums::SHAPETYPE::LINE:
											enteringFlag = true;
											item = new Line(_T("pL"), RapidEnums::SHAPETYPE::LINE);
											is >> (*static_cast<Line*>(item));
											break;

										case RapidEnums::SHAPETYPE::ARC:
											enteringFlag = true;
											item = new Circle(_T("pA"), RapidEnums::SHAPETYPE::ARC);
											is >> (*static_cast<Circle*>(item));
											break;
									}
									if(enteringFlag)
									{
										item->UpdateEnclosedRectangle();
										x.AddShape(item, false);
									}	
								}
							}
						}
					}
				}
			}
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
		x.UpdateBestFit();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERI0020", __FILE__, __FUNCSIG__); return is; }
}

