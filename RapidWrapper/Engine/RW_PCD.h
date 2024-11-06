#pragma once

namespace RWrapper 
{
public ref class RW_PCD
{
public:
	delegate void PCDEventHandler();

private:
	 //Instance of RW_PCD.
	 static RW_PCD^ pcd;
	 System::Collections::Hashtable^ PCDShapeCollection;
	 System::Collections::Generic::List<System::Int32>^ SelectedPCDShapes;
	 System::Collections::Generic::List<System::String^>^ AllShapeNameList;
	 System::Collections::ArrayList^ PCDlist;
	 	
	 //Calcualte all values, RW_PCD dia, offset and angles
	 void CalculateAllvalues();
	 void CalcualteAlloffsetValues();
	 void CalcualteAllAngles();
	 void CalculatePCDAngles(Shape* PCdShape, Shape* CShape1, Shape* CShape2);
	 //Function to get the shape parameters
	 void GettheCenter(Shape* sh, double* center);
	
public:
	 System::Collections::Generic::List<System::String^>^ AddShapeMemberNameList;
	 System::Collections::Generic::List<System::String^>^ PCDMemberNameList;
	
	 event PCDEventHandler^ PCDMeasureSelectionChanged;

	 RW_PCD();
	 ~RW_PCD();
	 static RW_PCD^ MYINSTANCE();

	 void NewClicked();
	 void PCD_MeasurementClicked();
	 void Circle_Added(System::String^ CName);
	 void Circle_Remove(System::String^ CName);
	 System::Collections::ArrayList^ GetPCDMeasureList();
	 void ClearLastPCDValues();
	 void GetSelectedPCDParameters();

	 void ResetAllPCD();
};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!