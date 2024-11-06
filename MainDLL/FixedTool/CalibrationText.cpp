#include "stdafx.h"
#include "CalibrationText.h"
#include "..\Engine\RCadApp.h"

// constructor 
CalibrationText::CalibrationText()
{
	mousedownDetail = false ;
	mousedownTable = false;
}

CalibrationText::~CalibrationText()
{
}

void CalibrationText::drawCalibrationText(double x1, double y1,double zoom)
{
	if(!mousedownDetail)
	{
		xDetail = x1;
		yDetail = y1;
	}
	std::string result;
	char numstr[21];
	std::list<string > Tmpstring;
	result = "Machine Number :" + MachineNo ;
	Tmpstring.push_back(result); 
	result = "Customer Name : " + CmpnyName ;
	Tmpstring.push_back(result); 
	result = "Verification done by : " + CalrName ;
	Tmpstring.push_back(result); 
	result = "Date : " + Date ;
	Tmpstring.push_back(result); 
	GRAFIX->InsertText(xDetail , yDetail, 0, false, Tmpstring ,zoom);
	
}

void CalibrationText::drawCalibrationMeasurement(double x1,double y1,double zoomin, double zoomout)
{
	int j =1;
	double avg = 0;
	double max=0;
	double min =10000;
	int count = 0;
	std::string fname = "Dist";
	std::string result;
	std::list<string > Tmpstring;
	Tmpstring .push_back ("Repeat Measure");
	if(!mousedownTable)
	{
		xTable = x1;
		yTable = y1;
	}
	
	for (list<char*>::iterator it = MeasureArray.begin(); it != MeasureArray.end(); it++)
	{
		char numstr[21]; 
		result = fname + itoa(j, numstr, 10);
		result = result + " : " + *it;
		Tmpstring.push_back(result); 
		j+=1;
	}

	for (list<double>::iterator it = MeasurementArray.begin(); it != MeasurementArray.end(); it++)
	{
		if(*it>max)
			max = *it;
		if(*it<min)
			min= *it;
		avg += *it;
		count ++;
	}

	char cd[8];
	string temp;
	RMATH2DOBJECT->Double2String(min, 4, cd); 
	temp = cd ;
	result = "Min   : " + temp ;
	Tmpstring.push_back(result); 
	
	RMATH2DOBJECT->Double2String(max, 4, cd);
	temp = cd ;
	result = "Max  : " + temp ;
	Tmpstring.push_back(result); 
	
	/*for (list<double>::iterator it = MeasurementArray.begin(); it != MeasurementArray.end(); it++)
	{
		avg += *it;
		count ++;
	}*/
	avg /= count;
	double diff= max - min;
	RMATH2DOBJECT->Double2String(diff, 4, cd); 
	temp = cd ;
	result = "Diff   : " + temp ;
	Tmpstring.push_back(result); 
	GRAFIX->InsertText(xTable, yTable, 0, true,Tmpstring ,zoomin);
}