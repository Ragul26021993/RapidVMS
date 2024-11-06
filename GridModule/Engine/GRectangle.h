//Class for rectangle shape..//
#include "dllmain.h"
#include<string>

using namespace std;
class GRIDMODULE_API GRectangle
{
	static int Rnumber;
	double topleft[3], topright[3], bottomleft[3], bottomright[3], center[3];
	bool isvalid, isselected;
	int VisitCount, itemid;
	std::string Rname;
	std::string ImagePath;
public:
	GRectangle();
	~GRectangle();

	void SetLeftTop(double x, double y, double z);
	void SetBottomRight(double x, double y, double z);
	bool Ishiglighted(double x, double y);
	void CalculateEndpoints();
	double* GetLeftTop();
	double* GetBottomRight();
	double* GetRightTop();
	double* GetBottomLeft();
	double* GetCenter();
	bool valid();
	void valid(bool flag);
	bool selected();
	void selected(bool flag);
	void IncrementVcount();
	int GetVisitCount();
	int getId();
	void SetName(std::string st);
	std::string GetName();
	static void reset();
	std::string GetImagePath();
	void SetImagePath(std::string);
};	