//Main Class to handle the Fixed Shapes...
//Mainly holds all the fixed shape and interacts with the front end through wrapper..
#pragma once 
#include "..\MainDLL.h"
#include <list>
#include <map>
#define FG_ITER map<int,FixedShape*>::iterator
using namespace std;

class FixedShape;

class MAINDLL_API FixedToolCollection
{
private:
	FixedShape* CurrentFixedShape; //Current Selected Fixed shape..
	HANDLE _mutex; 
	bool IsValid;
public:
	map<int, FixedShape*> FixedShapeCollection; // Fixed shape collections..
	//Constrcutor and destructor..//
	FixedToolCollection();
	~FixedToolCollection();
	//RapidToggleProperty<bool> HatcedFixedGraphics;
	//Add Fixed Shapes..
	void AddFixedShapes(int _fixedMode, int Sid, bool dontClear = false); 
	//Set the current fixed shape parameter..
    void setToolParameter(double _first, double _second, double _third, void* anyThing);
	//Setting parameter for Fixed text, Circle and Markers.
	void setFixedToolParameter(double x, double y, double z, void *AnyThing);
	//Change the font size of current selected text..
	void ChangeFixedTextSize(bool increase);
	//Toggle hatched property of the selected fixed shape
	void ToggleFixedShpHatched();
	//Change the color..
	void setFixedToolColor(double r, double g, double b);
	//Sets the center cam as per DRO move...//
	void SetWindowCenter(double x, double y);
	//Select the fixed shape...
	void selectFixedTool(int id);
	//Draw all special tools....
    void drawAll(); 
	//Remove the selected fixed shape..//
	void RemoveSelected();
	//Clear all the fixed tool shapes..//
	void clearFixedTool(bool clearAll = true);
	//Current Selected shape type..
	int CurrentSelectedFixedshape();
	//Hatched fixed shape..
	bool FixedHatched();

	bool FixedNonClearable();

	//Current circle type..
	bool FixedCircleType();
	//Expose Radius for Fixed Selected circle.
	double FixedRadius();
	//Expose Text for Fixed Selected Text.
	char* FixedString();
	//Expose Current Shape X position.
	double FixedgetX();
	//Expose Current Shape Y position.
	double FixedgetY();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!