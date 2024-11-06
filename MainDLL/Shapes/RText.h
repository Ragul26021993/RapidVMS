//Class for text shape..//
#pragma once
#include "shapewithlist.h"
#include "vector.h"

class MAINDLL_API RText:
	public ShapeWithList
{
private:
	//Variables for shape number..//
	static int shapenumber;
	static int DXFshapenumber;
	//main point and text to show
	Vector TextPosition;
	std::string TextString;
	TCHAR* genName(const TCHAR* prefix);
	void init();
	Vector LeftMiddle, RightMiddle;
	Vector LeftTop, LeftBottom, RightTop, RightBottom;
public:
	RapidProperty<double> TextWidth, TextHeight, TextSize, TextAngle;
public:
	//Constructors..//
	RText(TCHAR* myname = _T("T"));
	RText(bool simply);
	~RText();

	void SetTextPosition(Vector d);
	void SetCurrentText(std::string& textod);
	Vector* getPoint();
	Vector* getLeftTopPoint();
	Vector* getRightBtmPoint();
	std::string getText();


	void RotateText(double x, double y);
	void calculateAttributes();

	//Virtual Functions//
	virtual void Transform(double* transform);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void Translate(Vector& Position);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();

	static void reset();

	friend wostream& operator<<(wostream& os, RText& x);
	friend wistream& operator>>(wistream& is, RText& x);
	friend void ReadOldPP(wistream& is, RText& x);
};	