#pragma once
namespace RWrapper 
{
public ref class RW_MeasureParameter
{	 
	 System::Data::DataTable^ MeasureMentListTable;
	 System::Data::DataTable^ DelphiMeasureMentListTable;
	 System::Data::DataTable^ MeasureMentListTableWithNT;
	 static RW_MeasureParameter^ DimParamInstance;
	 

public:
	//delegate for measurement related event handling.......................
	delegate void MeasurementParameterEventHandler(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE MeasureType);

	event MeasurementParameterEventHandler^ MeasureParamUpdateEvent;
	
	 RW_MeasureParameter();
	 ~RW_MeasureParameter();
	 static RW_MeasureParameter^ MYINSTANCE();
	 
	 
	 //variable declaration
	 System::Data::DataTable^ MeasurementParamTable;
	 System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureParameter_ArrayList;
	 System::Collections::Generic::List<System::Int32>^ SelectedMeasureIDList;
	 int Measure_EntityID;
	 System::String^ CurrentGradeValue;

	 System::Collections::ArrayList^ GetTotal_MeasurementList();
	 System::Collections::ArrayList^ DelphiAngleMeasurementList();
	 System::Collections::Generic::List<System::Collections::ArrayList^>^ GetTotal_MeasurementListWithNT();
	 System::Collections::Generic::List<System::Collections::ArrayList^>^ GetTotal_MeasurementListWithNTHobChecker(int MeasurementType); //, double Radius, double TrueAngle, int NoofPtsfor_FormMeasurement);

	 System::Data::DataTable^ GetMeasurementTable();
	 System::Data::DataTable^ GetMeasurementTableWithDelphi();
	 System::Data::DataTable^ GetMeasurementTableWithNT();

	 cli::array<Double>^ HobMeasurements;

	 void SetDimensionForExcel(System::String^ dimName, System::String^ dimTYpe, System::String^ dimValue, System::Collections::ArrayList^ MeasureDimNameValue);
	 void SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, System::String^ dimValue, double Nominal, double usl, double lsl, int MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT);
	 void SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, int dimValue, double Nominal, double usl, double lsl, int MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT);
	 void SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, System::String^ dimValue, double Nominal, double usl, double lsl, System::String^ MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT);	
	 void SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, int dimValue, double Nominal, double usl, double lsl, System::String^ MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT);

	 void SelectMeasurement(int Measureid);
	 void SelectMeasurement(cli::array<int, 1>^ IdList);
	 void DeleteSelectedMesurement();
	 void RenameMeasurement(System::String^ Str);
	 void RenameMeasurement(System::String^ Str, int MeasureId);
	 void Change_MeasurementFontSize(int Fsize);
	 void SetMeasureNominalTolerance(double NVal, double usl, double lsl);
	 void SetMeasureNominalTolerance(double NVal, double usl, double lsl, int MeasureId);
	 void SetMeasureIncludePF(bool IncludePF, int MeasureId);
	 void SetMeasurementUnit(int type);
	 void SetAngleMeasureMode(int type);
	 void ZPositionof3DpointforTVSCircleCen(double ZPos, double DTVS_Cir_cenX, double DTVS_Cir_cenY, double DTVS_Cir_cenZ, double InMinRad, double InMaxRad);	//vinod.. 16/04/2014

	 void AddMeasurementParameter(BaseItem* item);
	 void RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE Mtype);
	 void SelectedMeasurementParameter(RCollectionBase* Measurecollselected);
	 void SetMeasurementParamForTable();
	 int CurrentAngleMeasureMode();
	 System::String^ Convert_AngleFormat(double Ang_val, int measureType, System::String^ MFormat, bool ConvertToDecimalAlso);
};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!