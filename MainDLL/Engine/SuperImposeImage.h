#pragma once
#include "UCS.h"
#include <list>
#include <map>
//#include <vector>
//#include <TCHAR.H>
//#include <iomanip>
//#include <string>
#include "..\GridModule\Engine\GridModule.h"

#define SuperImposeImageObj SuperImposeImage::getMyInstance()

class GridModule;
class GRectangle;

class MAINDLL_API SuperImposeImage
{

private:
	SuperImposeImage();
	~SuperImposeImage();
	static SuperImposeImage* _myinstance;//static Instance of this class.
	int imagecount;
	GM_ITER item;
	map<int, GRectangle*> GList;
	map<int, GRectangle*> allitems;
	map<int ,std::string> imagepath;
	bool maxcolreached;
	std::string FolderPath;
public:
	bool FirstTime;	
	void (CALLBACK *ImageCapturingCompleted) ();
#pragma region Variables Pointer	
	GridModule* MyGrid; // GridModule///
	GRectangle* GridCellShape;
#pragma endregion

	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	static SuperImposeImage* getMyInstance();
	double GridWidth,GridHeight,ColumnsCount,RowsCount;

#pragma region Grid handling

	//Initialise Grid Module...	
	void InitialiseGridModule(double LeftTopX, double LeftTopY, double BottomRightX, double BottomRightY,double CellWidth, double CellHeight);	
	//Increment the grid selection..
	void IncrementGridSelections();
	void SmartIncrementGridSelections();
	
	void GotoDefaultPosition();
	void GotoHomePosition();
	void StartImageCapturingProcess();
	void ClearCurrentGrid();
	void setSmartCurrentHighlitedCell(double x , double y);
	void setCurrentHighlitedCell(double x , double y);
	void GridModuleErrorHandler(char* ecode, char* fname, char* funcname);
	void CaptureImage();
	void SetFolderPath(std::string Folderpath);
	std::string GetFolderPath();
#pragma endregion

#pragma region All Callbacks
	  // callback function to delete pointslogtext file
	 void (CALLBACK *ImagecaptureSound) ();
#pragma region
};
