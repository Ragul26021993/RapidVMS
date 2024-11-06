//Class to add perimeter shape..//
#pragma once
#include "shapewithlist.h"
//derived from the shape with list..//
class MAINDLL_API Perimeter:
	public ShapeWithList
{
private:
	static int shapenumber;
	Vector Pcenter;
	TCHAR* genName(const TCHAR* prefix);
	void init();
	void deleteAllShapes(bool DeleteShape = true);
	bool AddFramgrabPts;
public:
	RapidProperty<double> TotalPerimeter, Diameter, TotalArea;
	map<int, Shape*> PerimeterShapecollection;
	map<int, Shape*> TempShapecollection;
public:
	//Constructor and destructor..//
	Perimeter(TCHAR*myname = _T("Peri"));
	Perimeter(bool FramgrabPts);
	~Perimeter();
	
	//Virtual functions..//
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void Transform(double* transform);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void Translate(Vector& Position);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);

	Vector* getCenter();

	void AddShape(Shape* Csh, bool SetRelation = true);
	void RemoveShape(Shape* Csh);
	static void reset();
	void SetParentChildRelation();
	//Write and Read to/ from the file..///
	friend wostream& operator<<(wostream& os, Perimeter& x);
	friend wistream& operator>>(wistream& is, Perimeter& x);
};