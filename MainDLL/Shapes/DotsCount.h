//PolyLine Class.. Like Cloudpoints.. Set of lines is one shape..//
#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>

class MAINDLL_API DotsCount:
	public ShapeWithList
{
private:
	static int shapenumber;
	TCHAR* genName(const TCHAR* prefix);
	
	void init();	
	class DotStruct
	{
	public:
		DotStruct(){};
		~DotStruct()
		{
			while(PtsColl.size() != 0)
			{
				std::list<double*>::iterator itr1 = PtsColl.begin();
				double *PointValue = (*itr1);
				PtsColl.erase(itr1);
				delete [] PointValue;
			}
		};
		std::list<double*> PtsColl;
	};
	
	std::list<DotStruct*> DotsColl;
	void ClearAllMemory();
	void CreateDotsCollection();
	void CreateCircleCollection();
public:
	DotsCount(TCHAR* myname = _T("DC"));
	DotsCount(bool simply);
	~DotsCount();

	unsigned int MinimumPtsCount, PixelCount;
	double FrameArea;
	std::list<double*> CircleCollection;

	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void Translate(Vector& Position);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);
	static void reset();
};	