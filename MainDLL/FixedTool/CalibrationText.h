#pragma once
#include "..\MainDLL.h"
#include <list>
class CalibrationText
{
	//Variable declarations...//
private:
	double xDetail, yDetail,xTable,yTable, min, max,avg;
public :
     bool mousedownDetail, mousedownTable;
	std::string MachineNo, CmpnyName, CalrName, Date;
	 std::list<char*> MeasureArray;
	 std::list<double> MeasurementArray;
	CalibrationText();
	~CalibrationText();
	void drawCalibrationText(double x, double y,double zoom); 
	void drawCalibrationMeasurement(double x,double y,double zoomin, double zoomout);
	void calculation();
};