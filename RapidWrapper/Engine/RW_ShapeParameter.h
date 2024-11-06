#pragma once
namespace RWrapper 
{
public ref class RW_ShapeParameter
{
public:
	
	delegate void ShapeParameterEventHandler(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE SType);
	//modified on 30 May 12..............
	delegate void SelectFgPtEventHandler(System::Collections::Generic::List<System::Int32>^ IDList);

private:	 
	 static RW_ShapeParameter^ ShParamInstance;
	 System::Collections::ArrayList^ ShapePropertiesArray;
	 
public:
	
	  event ShapeParameterEventHandler^ ShapeParameterUpdate;
	  event SelectFgPtEventHandler^ ShowFgPtListEvent;

	  int Shape_EntityID;
	  //array list for shapes Parameter.........
	 static System::Collections::ArrayList^ ShapeParameter_ArrayList;
	 System::Collections::Generic::List<System::Int32>^ SelectedShapeIDList;
	 //Points list of the current selected shape
	 System::Data::DataTable^ ShapeParameterTable;
	 //Tables for all shape properties
	 System::Data::DataTable^ ShapePointsTable;
public:
	 RW_ShapeParameter();
	 ~RW_ShapeParameter();
	 static RW_ShapeParameter^ MYINSTANCE();

#pragma region Shape Events, Parameter Updates
	 void GetTotal_ShapeList();
	 void GetShapeList();
	 void SetShapeParam(char* SName, char* Stype, double clicks, char* Ucs);
	 void SetShapeValues(double *m1);
	 void SetShapeParameters(System::String^ lbl1txt, System::String^ lbl2txt, System::String^ lbl3txt, System::String^ lbl4txt, System::String^ txt1txt,String^ txt2txt, System::String^ txt3txt, System::String^ txt4txt);	
	 void SetShapeProperties(Shape* currentShape);
	 void SetShapeParamForTable();
	 void ResetShapePointsTable();
	 void RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE ShapeType);
	 void UpdateSelectedShape_PointsTable();
	 void SetShapeParamForFrontend(Shape* currentShape);
#pragma endregion

#pragma region Select, Delete Entity, Entity List
	 void SelectShape(int Shapeid);
	 void SelectShape(cli::array<int, 1>^ IdList);
	 void SelectPointsList(cli::array<int, 1>^ IdList);

	 void DeleteSelectedShape();
	 void DeleteSelectedPointsList();
	 void GotoSelectedPtPosition();
	 void RenameShape(System::String^ Str);
	 void SetTextValue_TextShape(System::String^ Str);
	 void ShowFgPointsEvent(System::Collections::Generic::List<System::Int32>^ IDList);
#pragma endregion

};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!