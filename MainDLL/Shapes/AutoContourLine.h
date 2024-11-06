//PolyLine Class.. Like Cloudpoints.. Set of lines is one shape..//
#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>


class MAINDLL_API AutoContourLine:
	public ShapeWithList
{
	static int shapenumber;
	TCHAR* genName(const TCHAR* prefix);

	public:
			
	struct Region
	{
		Region(double *strtPos, double *droPos, int cell)
		{
			Cell = cell;
			DroPos[0] = droPos[0];
			DroPos[1] = droPos[1];
			DroPos[2] = droPos[2];
			StrtPos[0] = strtPos[0];
			StrtPos[1] = strtPos[1];
			StrtPos[2] = strtPos[2];
		}

		~Region()
		{}

		int *GetRectPointer(int length, int seletionFrames_Width, double *moveDirCos, double wupp, int gap)
		{
			int counter = 0;
			int *tmp = new int[Cell * 4];
			for(double i = seletionFrames_Width/2; i < (length - seletionFrames_Width/2); i= i + gap)
			{
				double tmpX = (StrtPos[0] - DroPos[0]) / wupp + i * moveDirCos[0];
				double tmpY = (StrtPos[1] - DroPos[1]) / wupp + i * moveDirCos[1];
				tmp[counter++] = int(400 + int(tmpX) - seletionFrames_Width/2);
				tmp[counter++] = int(300 - int(tmpY) - seletionFrames_Width/2);
				tmp[counter++] = seletionFrames_Width;
				tmp[counter++] = seletionFrames_Width; 
			}	   
			this->Cell = counter / 4;
			return tmp;
		}

		double DroPos[3], StrtPos[3];
		int Cell;
	};	

	double *SelectedPoints, extentPnts[4], center[3], LineParam[6];
	int SelectedPointsCnt, NumberOfRegions;
	AutoContourLine();
	AutoContourLine(map<int, double> selectedPnts);
	~AutoContourLine();
	list<Region*> RegionList;
	list<Region*>::iterator RegionIter;
	bool CreateRegions(int length);
	void ClearRegion();
	//Virtual functions../;/
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);

	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void Translate(Vector& Position);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	void reset();
	void GetShapeCenter(double *cPoint);

		//Write and Read to/ from the file..///
	friend wostream& operator<<(wostream& os, AutoContourLine& x);
	friend wistream& operator>>(wistream& is, AutoContourLine& x);
};	
