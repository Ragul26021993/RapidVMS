//Class Used for ImageGridProgram....
#pragma once
namespace RWrapper
{
	public ref class RW_ImageGridProgram
	{
		static RW_ImageGridProgram^ ImageCalInstance; //hold the instance..
		int TotalRows, TotalColumns, CellCount, listCount;
		System::Collections::Generic::List<System::Double>^ CellPosition;
		System::Collections::Generic::List<System::Int32>^ ActionIdList;
		System::String^ currentStoragePath;

	public:
		delegate void PartProgramBuildEventHandler();
		event PartProgramBuildEventHandler^ CallPartProgramBuildEvent;
		RW_ImageGridProgram(); //Default constructor..
		~RW_ImageGridProgram(); //Destructor.. Clear the memory..
		static RW_ImageGridProgram^ MYINSTANCE(); //Expose the class instance..
		void CreatePathListForImageAction(int NoOfRows, int NoOfColumn, double RowGap, double ColumnGap);
		void Continue_PauseImageAction(bool ContinueFlag);
		void StopImageAction();
		void ClearAll();
		void Goto_NextCellPosition(bool ContinueFlag);
		void SetStoragePath(System::String^ folderPath);
	};
}

//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!