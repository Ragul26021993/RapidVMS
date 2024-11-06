#include "StdAfx.h"
#include "AreaShape.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AreaShapeAction.h"

AreaShape::AreaShape():ShapeWithList(L"Area")
{
	init();
}

AreaShape::AreaShape(bool whiteRegion):ShapeWithList(L"Area")
{
	init();
	this->WhiteArea = whiteRegion;
}

AreaShape::~AreaShape()
{
}

void AreaShape::init()
{
	this->ShapeType = RapidEnums::SHAPETYPE::AREA;
	this->IsValid(true);
	this->WhiteArea = false;
	this->firsttime = true;
	this->TotArea = 0;
	this->AreaList.clear();
}

void AreaShape::AddArea(double TotalRegion, double WhiteRegion, FramegrabBase* currentAction, bool AddAction)
{
	try
	{
		if(this->firsttime)
		{
			this->firsttime = false;
			this->Pcenter = currentAction->PointDRO;
		}
		double Areavalue = WhiteRegion;
		if(!this->WhiteArea)
			Areavalue = TotalRegion - WhiteRegion;
		this->AreaList.push_back(Areavalue);
		this->TotArea = 0;
		for each(double value in this->AreaList)
			this->TotArea += value;
		if(AddAction)
			AreaShapeAction::AddAreaShapeDroValue(this, currentAction, Areavalue);
		this->notifyAll(ITEMSTATUS::DATACHANGED, this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area003", __FILE__, __FUNCSIG__); }
}

void AreaShape::UpdateArea(double areaValue)
{
	try
	{		
		this->AreaList.push_back(areaValue);
		this->TotArea = 0;
		for each(double value in this->AreaList)
			this->TotArea += value;
		this->notifyAll(ITEMSTATUS::DATACHANGED, this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area003", __FILE__, __FUNCSIG__); }
}

void AreaShape::UpdateArea()
{
	try
	{		
		if(this->AreaList.size() > 0)
		{
			std::list<double>::iterator itr = this->AreaList.end();	
			itr--;
			this->AreaList.erase(itr);
			this->TotArea = 0;
			for each(double value in this->AreaList)
				this->TotArea += value;
			this->notifyAll(ITEMSTATUS::DATACHANGED, this);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area003", __FILE__, __FUNCSIG__); }
}

Vector* AreaShape::getCenter()
{
	return &(this->Pcenter);
}

bool AreaShape::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	return false;
}

bool AreaShape::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool AreaShape::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void AreaShape::Translate(Vector& Position)
{
}

void AreaShape::drawCurrentShape(int windowno, double WPixelWidth)
{
}

void AreaShape::drawGDntRefernce()
{
}

void AreaShape::ResetShapeParameters()
{
}

void AreaShape::UpdateBestFit()
{
	IsValid(true);
}

 bool AreaShape::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{return false;
}

void AreaShape::UpdateForParentChange(BaseItem* sender)
{
}

void AreaShape::Transform(double* transform)
{
}

void AreaShape::AlignToBasePlane(double* trnasformM)
{
}

void AreaShape::UpdateEnclosedRectangle()
{
}

Shape* AreaShape::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		AreaShape* CShape = new AreaShape();
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area001", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* AreaShape::CreateDummyCopy()
{
	try
	{
		AreaShape* CShape = new AreaShape(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area002", __FILE__, __FUNCSIG__); return NULL; }
}

void AreaShape::CopyShapeParameters(Shape* s)
{
	try
	{
		this->TotArea = ((AreaShape*)s)->TotArea;
		this->Pcenter = ((AreaShape*)s)->Pcenter;
		this->WhiteArea = ((AreaShape*)s)->WhiteArea;
		this->AreaList.clear();
		for each(double value in ((AreaShape*)s)->AreaList)
			this->AreaList.push_back(value);
		}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area005", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, AreaShape& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "AreaShape" << endl;
		os << "TotalArea:" << x.TotArea << endl;
		os << "WhiteArea:" << x.WhiteArea << endl;
		os << "center:value"<< endl << (*static_cast<Vector*>(x.getCenter())) << endl;
		os << "EndAreaShape" << endl;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area003", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, AreaShape& x )
{
	try
	{
		is >> (*(Shape*)&x);
		std::wstring Tagname;
		is >> Tagname;
		if(Tagname==L"AreaShape")
		{
			x.ShapeType = RapidEnums::SHAPETYPE::AREA;
			while(Tagname!=L"EndAreaShape")
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
						 x.TotArea = atof((const char*)(Val).c_str());
					}
					else if(Tagname==L"WhiteArea")
					{
						if(Val=="0")
							x.WhiteArea = false;
						else
							x.WhiteArea = true;
					}
					else if(Tagname==L"center")
					{
						is >> (*static_cast<Vector*>(x.getCenter()));
					}
				}
			}
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Area004", __FILE__, __FUNCSIG__); return is; }
}