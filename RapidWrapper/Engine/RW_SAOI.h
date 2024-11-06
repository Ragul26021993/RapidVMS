//Class Used for SAOI..
//All the button actions, event handling from / to the front end for the SAOI GRID..
#pragma once
namespace RWrapper 
{
public ref class RW_SAOIParameter
{
	int TopLeftX, TopLeftY , CellCount;
	double CellWidth, CellHeight,GridWidth, GridHeight;
	bool firstTime;
	static RW_SAOIParameter^ SAOIParam; //hold the instance..
	bool DisplayGrid;
public:
	RW_SAOIParameter(); //Default constructor..
	~RW_SAOIParameter(); //Destructor.. Clear the memory..
	double xvalue , yvalue;
	static RW_SAOIParameter^ MYINSTANCE(); //Expose the class instance..
	void InitialilseGrid(double TopLeftX, double TopLeftY, double GridWidth, double GridHeight, double CellWidth, double CellHeight);
	void IncrementGridSelection();
	void StartCNC();
	void ClearGrid();
	void ShowHideGrid();
	void SetTopLeftAccToGetPointer(double x ,double y);
	void (CALLBACK *SetFrontEndValue)();
	void MachineGotoDefaultPosition();
};
}

//Created by Shweytank
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!