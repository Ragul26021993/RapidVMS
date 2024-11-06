#pragma once
#include "dllmain.h"
#include <map>
#include <list>
# include <iostream>
# include <fstream>
#include "GRectangle.h"
#include<string>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

#define GM_ITER map<int,GRectangle*>::iterator
#define GM_ITER1 std::list<GRectangle*>::iterator

class GRIDMODULE_API GridModule
{
private:
	double GridTop, GridLeft, GridBottom, GridRight;
	double GridWidth, GridHeight, CellWidth, CellHeight;
	bool UserOnlyWH, UseRowColumnsCount, UseTwoCornersWH;
	bool ShowRCHeaders;
	std::string MyGridName;	
	map<int, GRectangle*> allitems;	
	map<int, GRectangle*> RowColumnitems;
	map<int, GRectangle*> ItemsAccordingToEffectiveMovementInColumn;//Increasing order if row is even and descending order when Row is odd
	std::list<GRectangle*> SelectedItemsList;
	std::list<GRectangle*> SortedItemsList;
	GRectangle* SelectedGrect;
	GM_ITER GMitem;
	GM_ITER1 GMSitem, GMSSitem;

	list<string> DXFList;
	list<string>::iterator DXFListIterator;
	char* filename;
	std::string temp;
	char cd[30];
	int  d, sgn;
	ofstream DXFDataWriter;
	//Entry point and exit point of the file
	void entryPoint();
	void exitPoint();
	//Double to string conversion functions
	char* postMinus(double x);
	//Set the common parameters to each shape...///
	void setCommonParameter(char* str,double x1,double x2,double x3);
	//Adds the text parameter...
	void addtextparameter(double ax,double bx,double cx,double width,double height,char* text);

	void AddRectangleParameter(double *point1, double *point2, double *point3, double *point4);
	//Generate the dxf file..//
	bool generateDXFFile(char* filename);

public:
	//map<int,GRectangle*>::iterator GM_ITER;
	GridModule(std::string Gridname = "Grid", bool ShowRowColumnHeaders = false);
	~GridModule();
#pragma region Error Handling.
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	void (*GridModuleError)(char* RMath3Derrorcode, char* RMath3DFileName, char* RMath3DFunName);
#pragma endregion

public: 
	int RowsCount, ColumnsCount;
	void SetLeftTop(double x, double y);
	void SetBottomRight(double x, double y);
	void SetWidth(double width);
	void SetHeight(double height);
	void SetCellWidth(double width);
	void SetCellHeight(double height);
	void SetRows(int rows);
	void SetColumns(int columns);
	void CalculateRectangles(bool EffectiveMovementRowWise = false);
	void SetTheGridCalcualtionType(bool UsingWidthHeight = true, bool UsingRowColumnCounts = false, bool Using2Corners = false);
	map<int,GRectangle*>& getItemList();
	map<int,GRectangle*>& getRCItemList();
	map<int,GRectangle*>& getECItemList();
	std::list<GRectangle*>& getSelectedItemList();
	void IncrementSelection();
	void IncrementaccordingEffectiveMovement();
	void IncrementSelectedSelection();
	void IncrementaccordingSorted();
	void ClearSelection();
	void SetCurrentIterator(map<int,GRectangle*>::iterator d);
	void SetCurrentIteratorOfEffectiveRCMovement(map<int,GRectangle*>::iterator d);
	void SetCurrentIterator(GRectangle* d);
	void SetCurrentIteratorOfEffectiveRCMovement(GRectangle* d);
	void SetSelectedRect(GRectangle* Sel);

	void SortItemList(std::list<GRectangle*> ItemList, double x, double y);

	void AddRemoveToselectedItems(GRectangle* Sel, double x, double y, bool Add = true);

	bool ShowColumnRowHeaders();

	GRectangle* getCurrentHighlightedCell(double x, double y);
	GRectangle* getSmartCurrentHighlightedCell(double x, double y);
	GRectangle* getCurrentSelecteCell();

	void WritetoDXF(char* filename);
};
