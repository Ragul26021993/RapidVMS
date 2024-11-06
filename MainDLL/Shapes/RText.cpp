#include "StdAfx.h"
#include "RText.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PointCollection.h"

RText::RText(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

RText::RText(bool simply):ShapeWithList(false)
{
	init();
}

RText::~RText()
{
}

void RText::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::RTEXT;
		this->TextWidth(0); this->TextHeight(0); this->TextAngle(0);
		this->TextSize(20); this->TextString = "";
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->LeftMiddle), 0), 0);
		this->SnapPointList->Addpoint(new SnapPoint(this, &(this->RightMiddle), 1), 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0001", __FILE__, __FUNCSIG__); }
}

TCHAR* RText::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		if(strcmp((const char*)prefix, "d") == 0)
		{
			DXFshapenumber++;
			_itot_s(DXFshapenumber, shapenumstr, 10, 10);
		}	
		else
		{
			shapenumber++;
			_itot_s(shapenumber, shapenumstr, 10, 10);
		}
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0002", __FILE__, __FUNCSIG__); return NULL; }
}

bool RText::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	try
	{
		bool flag = false;
		double point1[4] = {LeftTop.getX(), LeftTop.getY(), RightTop.getX(), RightTop.getY()};
		double point2[4] = {RightBottom.getX(), RightBottom.getY(), LeftBottom.getX(), LeftBottom.getY()};
		double mp[2] = {corner1.getX(), corner1.getY()};
		flag = RMATH2DOBJECT->PtisinRectangle(&point1[0], &point1[2], &point2[0], &point2[2], &mp[0]); 
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0003", __FILE__, __FUNCSIG__); }

	//try{ return (RMATH2DOBJECT->Ptisinwindow(corner1.getX(), corner1.getY(), TextPosition.getX() - TextWidth()/2, TextPosition.getY() - TextHeight()/2, TextPosition.getX() + TextWidth()/2, TextPosition.getY() + TextHeight()/2)); }
	//catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0003", __FILE__, __FUNCSIG__); }
}

bool RText::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

bool RText::Shape_IsInWindow(Vector& corner1,Vector& corner2)
{
	return false;
}

void RText::Translate(Vector& Position)
{
	TextPosition += Position;
}

void RText::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		GRAFIX->drawString(TextPosition.getX(), TextPosition.getY(), 0, TextAngle(), (char*)TextString.c_str(), WPixelWidth * TextSize(), true);
		if(this->HighlightedFormouse() && windowno == 0)
		{
			GRAFIX->SetColor_Double(0, 1, 0);
			GRAFIX->drawPoint(LeftMiddle.getX(), LeftMiddle.getY(), 0, 8);
			GRAFIX->drawPoint(RightMiddle.getX(), RightMiddle.getY(), 0, 8);
			//RCADGRAFIX->drawRectangle(LeftTop.getX(), LeftTop.getY(), RightTop.getX(), RightTop.getY(), RightBottom.getX(), RightBottom.getY(), LeftBottom.getX(), LeftBottom.getY());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0004", __FILE__, __FUNCSIG__); }
}

void RText::drawGDntRefernce()
{
}

void RText::ResetShapeParameters()
{
}

void RText::UpdateBestFit()
{
	int totcount = this->PointsList->Pointscount();
	if (totcount < 1) return;
	calculateAttributes();
	this->IsValid(true);
}

bool RText::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	return false;
}

void RText::RotateText(double x, double y)
{
	double angle;
	angle = RMATH2DOBJECT->ray_angle(pts[0], pts[1], x, y);
	if(MAINDllOBJECT->highlightedPoint()->operator== (RightMiddle))
	{
		this->TextAngle(angle * 180 / M_PI);
	}
	else
	{
		angle = angle - M_PI;
		if(angle < 0) angle += 2 * M_PI;
		this->TextAngle(angle * 180 / M_PI);
	}
	this->calculateAttributes();
}

void RText::calculateAttributes()
{
	SetTextPosition(Vector(pts[0], pts[1], MAINDllOBJECT->getCurrentDRO().getZ()));
	double angle = this->TextAngle() * M_PI/180;
	double endp1[2], endp2[2], temppoint[2];
	
	double cosangle = TextWidth() * cos(angle)/2, sinangle = TextWidth() * sin(angle)/2;
	double cosangle1 = TextHeight() * cos(angle + M_PI_2)/2, sinangle1 = TextHeight() * sin(angle + M_PI_2)/2;

	endp1[0] = pts[0] + cosangle; endp1[1] = pts[1] + sinangle;
	endp2[0] = pts[0] - cosangle; endp2[1] = pts[1] - sinangle;
	RightMiddle.set(endp1[0], endp1[1], MAINDllOBJECT->getCurrentDRO().getZ()); 
	LeftMiddle.set(endp2[0], endp2[1], MAINDllOBJECT->getCurrentDRO().getZ());


	temppoint[0] = endp1[0] + cosangle1; temppoint[1] = endp1[1] + sinangle1;
	LeftTop.set(temppoint[0], temppoint[1], MAINDllOBJECT->getCurrentDRO().getZ());
	temppoint[0] = endp1[0] - cosangle1; temppoint[1] = endp1[1] - sinangle1;
	LeftBottom.set(temppoint[0], temppoint[1], MAINDllOBJECT->getCurrentDRO().getZ());

	temppoint[0] = endp2[0] + cosangle1; temppoint[1] = endp2[1] + sinangle1;
	RightTop.set(temppoint[0], temppoint[1], MAINDllOBJECT->getCurrentDRO().getZ());
	temppoint[0] = endp2[0] - cosangle1; temppoint[1] = endp2[1] - sinangle1;
	RightBottom.set(temppoint[0], temppoint[1], MAINDllOBJECT->getCurrentDRO().getZ());
}

void RText::UpdateForParentChange(BaseItem* sender)
{
}

void RText::UpdateEnclosedRectangle()
{
	LeftTop.FillDoublePointer(&ShapeLeftTop[0]);
	RightBottom.FillDoublePointer(&ShapeRightBottom[0]);
}

void RText::SetCurrentText(std::string& txt)
{
	try
	{
		this->TextString.assign(txt.c_str());
		this->TextWidth(GRAFIX->getTextWidth((char*)TextString.c_str()) * MAINDllOBJECT->getWindow(0).getUppX() * TextSize());
		this->TextHeight(GRAFIX->getTexHeight((char*)TextString.c_str()) * MAINDllOBJECT->getWindow(0).getUppX() * TextSize());
		this->TextAngle(0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0005", __FILE__, __FUNCSIG__); }
}

Vector* RText::getPoint()
{
	return &TextPosition;
}

Vector* RText::getLeftTopPoint()
{
	return &LeftTop;
}

Vector* RText::getRightBtmPoint()
{
	return &RightBottom;
}

std::string RText::getText()
{
	return TextString;
}

void RText::SetTextPosition(Vector d)
{
	TextPosition = d;
}

Shape* RText::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("T");
		else
			myname = _T("dT");
		RText* CShape = new RText((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
		   CShape->CopyOriginalProperties(this);
		return CShape;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0006", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* RText::CreateDummyCopy()
{
	try
	{
		RText* CShape = new RText(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0007", __FILE__, __FUNCSIG__); return NULL; }
}

void RText::CopyShapeParameters(Shape* s)
{
	try
	{
		this->TextPosition.set(((RText*)s)->getPoint()->getX(), ((RText*)s)-> getPoint()->getY(), ((RText*)s)-> getPoint()->getZ());
		this->TextString = ((RText*)s)->getText();
		this->TextAngle(((RText*)s)->TextAngle());
		this->TextWidth(((RText*)s)->TextWidth());
		this->TextHeight(((RText*)s)->TextHeight());
		this->TextSize(((RText*)s)->TextSize());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0008", __FILE__, __FUNCSIG__); }
}



void RText::Transform(double* transform)
{
	SetTextPosition(MAINDllOBJECT->TransformMultiply(transform, TextPosition.getX(), TextPosition.getY()));
}

void RText::AlignToBasePlane(double* trnasformM)
{
}

int RText::shapenumber = 0;
int RText::DXFshapenumber = 0;
void RText::reset()
{
	shapenumber = 0;
	DXFshapenumber = 0;
}

wostream& operator<<(wostream& os, RText& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		os << "RText" << endl;
		os << "Point:values" << endl << (*static_cast<Vector*>(x.getPoint())) << endl;
		os << "TextAngle:" << x.TextAngle() << endl;
		os << "TextWidth:" << x.TextWidth() << endl;
		os << "TextHeight:" << x.TextHeight() << endl;
		os << "TextSize:" << x.TextSize() << endl;
		os << "Text:" << x.getText().c_str() << endl;
		os << "EndRText" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0009", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, RText& x )
{
	try
	{
		wchar_t pp[255];
		is >> (*(Shape*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"RText")
			{
			x.ShapeType = RapidEnums::SHAPETYPE::RTEXT;
			while(Tagname!=L"EndRText")
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
						if(Tagname==L"Point")
						{
							is >> (*static_cast<Vector*>(&x.TextPosition));
						}
						else if(Tagname==L"TextAngle")
						{
							x.TextAngle(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"TextWidth")
						{
							x.TextWidth(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"TextHeight")
						{
							x.TextHeight(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"TextSize")
						{
							x.TextSize(atof((const char*)(Val).c_str()));
						}
						else if(Tagname==L"Text")
						{
							is.getline(pp, 250);
							const wchar_t* p = 0;
							p = TagVal.c_str();
							for(int i = 0; i < (int)TagVal.length(); i++)
								x.TextString.append((char*)(p + i));
							TagVal = (wstring)&pp[0];
							for(int i = 0; i < (int)TagVal.length(); i++)
								x.TextString.append((char*)&pp[i]);
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0010", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, RText& x)
{
	try
	{
		wstring name;
		char buffer[256];
		double d;
		std::streamsize tst = 300;
		wchar_t pp[255];
		int n;
		bool flag;
		x.ShapeType = RapidEnums::SHAPETYPE::RTEXT;
		is >> (*static_cast<Vector*>(&x.TextPosition));
		is >> d; x.TextAngle(d);
		is >> d; x.TextWidth(d);
		is >> d; x.TextHeight(d);
		is >> d; x.TextSize(d);
		is >> name;
		is.getline(pp, 250);
		const wchar_t* p = 0;
		p = name.c_str();
		for(int i = 0; i < (int)name.length(); i++)
			x.TextString.append((char*)(p + i));
		name = (wstring)&pp[0];
		for(int i = 0; i < (int)name.length(); i++)
			x.TextString.append((char*)&pp[i]);
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("TEXT0010", __FILE__, __FUNCSIG__); }
}