#pragma once
namespace RWrapper 
{
public ref class RW_SuperImposeImage
{
	double TopLeftX, TopLeftY, CellCount ,BottomRightX , BottomRightY;
	double CellWidth, CellHeight,GridWidth, GridHeight;	
private:
	static RW_SuperImposeImage^ SuperImposeParam; //hold the instance..
	bool FirstPointSet;
	bool BothPointSet;	
public:
	RW_SuperImposeImage(); //Default constructor..
	~RW_SuperImposeImage(); //Destructor.. Clear the memory...
	
	static RW_SuperImposeImage^ MYINSTANCE(); //Expose the class instance..		
	void InitialiseGrid();
	void IncrementGridSelection();
	void StartCNC();
	void ClearGrid();
	void SetTopLeft(double x ,double y);
	void SetBottomRight(double x ,double y);
	void StartImageCapturingInGrid();	
	void MachineGotoHomePosition();
	void SetFolderPath(System::String^ FolderPath);
};
}

//Created by Shweytank
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!