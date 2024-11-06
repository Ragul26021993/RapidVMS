#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Shapes\Line.h"
#include "../MainDll/Measurement/MeasurementHeaders.h"
#include "RW_DelphiCamModule.h"
#include <iostream>
#include <fstream> 

double Point3DForZPositionZValue = 0.0, InMINRAD = 0.0, InMAXRAD = 0.0;			//added by vj on 11/04/2014, as per new measurement requirement for z position in tpliner 
double DelphiTVS_Cir_Cen[3] = { 0 };

RWrapper::RW_MeasureParameter::RW_MeasureParameter()
{
	try
	{
		 this->DimParamInstance = this;	 
		 this->CurrentGradeValue = "";

		 this->DelphiMeasureMentListTable = gcnew System::Data::DataTable();
		 /*this->DelphiMeasureMentListTable->Columns->Add("Measure Type");
		 this->DelphiMeasureMentListTable->Columns->Add("Measure Name");*/
		 this->DelphiMeasureMentListTable->Columns->Add("ValueTag");
		 this->DelphiMeasureMentListTable->Columns->Add("Value");
		 this->DelphiMeasureMentListTable->Columns->Add("GradeTag");
		 this->DelphiMeasureMentListTable->Columns->Add("Grade");

		 this->MeasureMentListTableWithNT = gcnew System::Data::DataTable();
		 this->MeasureMentListTableWithNT->Columns->Add("Measure Type");
		 this->MeasureMentListTableWithNT->Columns->Add("Measure Name");
		 this->MeasureMentListTableWithNT->Columns->Add("Value");
		 this->MeasureMentListTableWithNT->Columns->Add("Nominal");
		 this->MeasureMentListTableWithNT->Columns->Add("USL");
		 this->MeasureMentListTableWithNT->Columns->Add("LSL");
		 this->MeasureMentListTableWithNT->Columns->Add("MeasureID");
		 
		 this->SelectedMeasureIDList = gcnew System::Collections::Generic::List<System::Int32>;
		 this->Measure_EntityID = 0;
		 this->MeasureParameter_ArrayList = gcnew System::Collections::Generic::List<System::Collections::ArrayList^>;

		 this->HobMeasurements = gcnew cli::array<Double>(4);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDIMPRD0001", ex); }
}

RWrapper::RW_MeasureParameter::~RW_MeasureParameter()
{
}

RWrapper::RW_MeasureParameter^ RWrapper::RW_MeasureParameter::MYINSTANCE()
{
	return DimParamInstance;
}

System::Data::DataTable^ RWrapper::RW_MeasureParameter::GetMeasurementTable()
{
	try
	{
		System::Collections::ArrayList^ MeasureList = GetTotal_MeasurementList();
		this->MeasureMentListTable->Rows->Clear();
		for(int i = 0; i < MeasureList->Count/3; i++)
			this->MeasureMentListTable->Rows->Add(MeasureList[3 * i + 1], MeasureList[3 * i], MeasureList[3 * i + 2]);
		return MeasureMentListTable;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0004", ex); return MeasureMentListTable; }
}

System::Data::DataTable^ RWrapper::RW_MeasureParameter::GetMeasurementTableWithDelphi()
{
	try
	{
		System::Collections::ArrayList^ MeasureList = DelphiAngleMeasurementList();
		this->DelphiMeasureMentListTable->Rows->Clear();
		/*for(int i = 0; i < MeasureList->Count/4; i++)
			this->DelphiMeasureMentListTable->Rows->Add(MeasureList[4 * i], MeasureList[4 * i + 1], MeasureList[4 * i + 2], MeasureList[4 * i + 3]);*/
		for(int i = 0; i < MeasureList->Count/4; i++)
		{
			this->DelphiMeasureMentListTable->Rows->Add("Value", MeasureList[4 * i + 2], "Grade", MeasureList[4 * i + 3]);
			//this->DelphiMeasureMentListTable->Rows->Add("Grade", MeasureList[4 * i + 3]);
		}
		return DelphiMeasureMentListTable;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0006", ex); return DelphiMeasureMentListTable; }
}

System::Data::DataTable^ RWrapper::RW_MeasureParameter::GetMeasurementTableWithNT()
{
	try
	{
		System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureListWithNT = GetTotal_MeasurementListWithNT();
		this->MeasureMentListTableWithNT->Rows->Clear();
		for each(System::Collections::ArrayList^ MeasureList in MeasureListWithNT)
			this->MeasureMentListTableWithNT->Rows->Add(MeasureList[0], MeasureList[1], MeasureList[2], MeasureList[3], MeasureList[4], MeasureList[5], "", MeasureList[6]);
		return MeasureMentListTableWithNT;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0004", ex); return MeasureMentListTableWithNT; }
}

void RWrapper::RW_MeasureParameter::SetDimensionForExcel(System::String^ dimName, System::String^ dimTYpe, System::String^ dimValue, System::Collections::ArrayList^ MeasureDimNameValue)
{
	try
	{	 
		 MeasureDimNameValue->Add(dimName);
		 MeasureDimNameValue->Add(dimTYpe);
		 MeasureDimNameValue->Add(dimValue);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0020", ex); }
 }

void RWrapper::RW_MeasureParameter::SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, System::String^ dimValue, double Nominal, double usl, double lsl, int MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT)
{
	try
	{	 
		 System::Collections::ArrayList^ MValueList = gcnew System::Collections::ArrayList();
		 MValueList->Add(dimName);
		 MValueList->Add(dimTYpe);
		 MValueList->Add(dimValue);
		 if(AngleMeasure)
		 {
			 System::String^ CNominal = Convert_AngleFormat(Nominal, measureType, MFormat, true);
			 System::String^ Cusl = Convert_AngleFormat(usl, measureType, MFormat, true);
			 System::String^ Clsl = Convert_AngleFormat(lsl, measureType, MFormat, true);
			 MValueList->Add(CNominal);
			 MValueList->Add(Cusl);
			 MValueList->Add(Clsl);
		 }
		 else
		 {
			MValueList->Add(Nominal);
			MValueList->Add(usl);
			MValueList->Add(lsl);
		 }
		 MValueList->Add(MId);
		 MeasureDimNameValueWithNT->Add(MValueList);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0020", ex); }
}

void RWrapper::RW_MeasureParameter::SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, System::String^ dimValue, double Nominal, double usl, double lsl, System::String^ MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT)
 {
	 try
	 {
		  
		 System::Collections::ArrayList^ MValueList = gcnew System::Collections::ArrayList();
		 MValueList->Add(dimName);
		 MValueList->Add(dimTYpe);
		 MValueList->Add(dimValue);
		 if(AngleMeasure)
		 {
			 System::String^ CNominal = Convert_AngleFormat(Nominal, measureType, MFormat, true);
			 System::String^ Cusl = Convert_AngleFormat(usl, measureType, MFormat, true);
			 System::String^ Clsl = Convert_AngleFormat(lsl, measureType, MFormat, true);
			 MValueList->Add(CNominal);
			 MValueList->Add(Cusl);
			 MValueList->Add(Clsl);
		 }
		 else
		 {
			MValueList->Add(Nominal);
			MValueList->Add(usl);
			MValueList->Add(lsl);
		 }
		 MValueList->Add(MId);
		 MeasureDimNameValueWithNT->Add(MValueList);
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0020", ex); }
}

void RWrapper::RW_MeasureParameter::SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, int dimValue, double Nominal, double usl, double lsl, System::String^ MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT)
{ 
	try
	{
		 System::Collections::ArrayList^ MValueList = gcnew System::Collections::ArrayList();
		 MValueList->Add(dimName);
		 MValueList->Add(dimTYpe);
		 MValueList->Add(dimValue);
		 if(AngleMeasure)
		 {
			 System::String^ CNominal = Convert_AngleFormat(Nominal, measureType, MFormat, true);
			 System::String^ Cusl = Convert_AngleFormat(usl, measureType, MFormat, true);
			 System::String^ Clsl = Convert_AngleFormat(lsl, measureType, MFormat, true);
			 MValueList->Add(CNominal);
			 MValueList->Add(Cusl);
			 MValueList->Add(Clsl);
		 }
		 else
		 {
			MValueList->Add(Nominal);
			MValueList->Add(usl);
			MValueList->Add(lsl);
		 }
		 MValueList->Add(MId);
		 MeasureDimNameValueWithNT->Add(MValueList);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0020", ex); }
}

void RWrapper::RW_MeasureParameter::SetDimensionForExcelWithNT(System::String^ dimName, System::String^ dimTYpe, int dimValue, double Nominal, double usl, double lsl, int MId, bool AngleMeasure, int measureType, System::String^ MFormat, System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT)
{
	try
	{	 
		 System::Collections::ArrayList^ MValueList = gcnew System::Collections::ArrayList();
		 MValueList->Add(dimName);
		 MValueList->Add(dimTYpe);
		 MValueList->Add(dimValue);
		 if(AngleMeasure)
		 {
			 System::String^ CNominal = Convert_AngleFormat(Nominal, measureType, MFormat, true);
			 System::String^ Cusl = Convert_AngleFormat(usl, measureType, MFormat, true);
			 System::String^ Clsl = Convert_AngleFormat(lsl, measureType, MFormat, true);
			 MValueList->Add(CNominal);
			 MValueList->Add(Cusl);
			 MValueList->Add(Clsl);
		 }
		 else
		 {
			MValueList->Add(Nominal);
			MValueList->Add(usl);
			MValueList->Add(lsl);
		 }
		 MValueList->Add(MId);
		 MeasureDimNameValueWithNT->Add(MValueList);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0020", ex); }
}

System::Collections::ArrayList^ RWrapper::RW_MeasureParameter::GetTotal_MeasurementList()
{
	try
	{
		System::String^ MeasureFormat = "0.0";
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		System::Collections::ArrayList^ MeasureDimNameValue = gcnew System::Collections::ArrayList();
		
		for(RC_ITER CucsItem = MAINDllOBJECT->getUCSList().getList().begin(); CucsItem != MAINDllOBJECT->getUCSList().getList().end(); CucsItem++)
		{
			UCS* CurrentUcs = (UCS*)(*CucsItem).second;
			RCollectionBase& myDimCollection = CurrentUcs->getDimensions();
			for(RC_ITER CdimItem = myDimCollection.getList().begin(); CdimItem != myDimCollection.getList().end(); CdimItem++)
			{
				DimBase* Cdim = (DimBase*)(*CdimItem).second;
				if(!Cdim->IsValid()) continue;
				if(Cdim->ChildMeasurementType()) continue;
				if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
				{
					DimPointCoordinate* CdimPt = (DimPointCoordinate*)Cdim;
					SetDimensionForExcel(gcnew System::String(CdimPt->DimPointX->getModifiedName()), "Center", Microsoft::VisualBasic::Strings::Format(Cdim->ParentPoint1->getX(), MeasureFormat), MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimPt->DimPointY->getModifiedName()), "Center", Microsoft::VisualBasic::Strings::Format(Cdim->ParentPoint1->getY(), MeasureFormat), MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimPt->DimPointZ->getModifiedName()), "Center", Microsoft::VisualBasic::Strings::Format(Cdim->ParentPoint1->getZ(), MeasureFormat), MeasureDimNameValue);
				}
				else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
				{
					DimThread* CdimTh = (DimThread*)(*CdimItem).second;
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdMajordia->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdMajordia->ChildMeasureValue(), MeasureFormat), MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdMinordia->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdMinordia->ChildMeasureValue(), MeasureFormat), MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdEffdiaG->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdEffdiaG->ChildMeasureValue(), MeasureFormat), MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdEffdiaM->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdEffdiaM->ChildMeasureValue(), MeasureFormat), MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdPitch->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdPitch->ChildMeasureValue(), MeasureFormat), MeasureDimNameValue);
					System::String^ ThreadAngle = Convert_AngleFormat(CdimTh->DimThrdAngle->ChildMeasureValue(), CMeasureMode, MeasureFormat, true);
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdAngle->getModifiedName()), "RW_Thread", ThreadAngle, MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdTopRadius->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdTopRadius->ChildMeasureValue(), MeasureFormat), MeasureDimNameValue);
					SetDimensionForExcel(gcnew System::String(CdimTh->DimThrdRootRadius->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdRootRadius->ChildMeasureValue(), MeasureFormat), MeasureDimNameValue);
				}
				else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
					SetDimensionForExcel(gcnew System::String(Cdim->getModifiedName()), "Depth", Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat), MeasureDimNameValue);
				else
				{
					if(Cdim->DistanceMeasurement())
						SetDimensionForExcel(gcnew System::String(Cdim->getModifiedName()), "Distance", Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat), MeasureDimNameValue);
					else
					{
						System::String^ Angle_Value = Convert_AngleFormat(Cdim->getDimension(), CMeasureMode, MeasureFormat, false);
						SetDimensionForExcel(gcnew System::String(Cdim->getModifiedName()), "Angle", Angle_Value, MeasureDimNameValue);
					}
				}
			}
		}
		return MeasureDimNameValue;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025", ex); }
}

System::Collections::ArrayList^ RWrapper::RW_MeasureParameter::DelphiAngleMeasurementList()
{
	try
	{
		System::String^ MeasureFormat = "0.0";
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		System::Collections::ArrayList^ AngleMeasureDimNameValue = gcnew System::Collections::ArrayList();
		
		for(RC_ITER CUcsitem = MAINDllOBJECT->getUCSList().getList().begin(); CUcsitem != MAINDllOBJECT->getUCSList().getList().end(); CUcsitem++)
		{
			UCS* CurrentUcs = (UCS*)(*CUcsitem).second;
			RCollectionBase& myDimCollection = CurrentUcs->getDimensions();
			for(RC_ITER Dimitem = myDimCollection.getList().begin(); Dimitem != myDimCollection.getList().end(); Dimitem++)
			{
				DimBase* Cdim = (DimBase*)(*Dimitem).second;
				if(!Cdim->IsValid()) continue;
				if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_ROTARYANGLEMEASURE)
				{
					System::String^ MValue = "";
					System::String^ AngGradeValue = "";
					if(MAINDllOBJECT->AngleMeasurementMode() == 0) 
					{
						char AngledegMin[100];
						RMATH2DOBJECT->Radian2Deg_Min_Sec(Cdim->getDimension(), &AngledegMin[0]);
						MValue = gcnew System::String(&AngledegMin[0]);
					}
					else
						MValue = Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat);
					double AngleDec = Cdim->getDimension();
					if(MAINDllOBJECT->AngleMeasurementMode() == 0 || MAINDllOBJECT->AngleMeasurementMode() == 2) 
						AngleDec = System::Math::Abs(Cdim->getDimension() * (180 / System::Math::PI));
					if(AngleDec < RWrapper::RW_DBSettings::MYINSTANCE()->FirstGradeValue || AngleDec > RWrapper::RW_DBSettings::MYINSTANCE()->LastGradevalue)
						AngGradeValue = "None";
					else
					{
						int GradeIndex = -1;
						for each(System::Collections::DictionaryEntry GradeValue in RWrapper::RW_DBSettings::MYINSTANCE()->GradeValueCollection)
						{
							GradeIndex = System::Convert::ToInt32(GradeValue.Key);
							if(AngleDec >= System::Convert::ToDouble(GradeValue.Value))
							{
								if(AngleDec <= System::Convert::ToDouble(RWrapper::RW_DBSettings::MYINSTANCE()->GradeValueEndCollection[GradeIndex]))
								{
									break;
								}
							}
						}
						if(GradeIndex >= 0)
							AngGradeValue = System::Convert::ToString(RWrapper::RW_DBSettings::MYINSTANCE()->GradeLevelCollection[GradeIndex]);
						CurrentGradeValue = AngGradeValue;

						/*double AngleDiff = AngleDec - RWrapper::RW_DBSettings::MYINSTANCE()->FirstGradeValue;
						double AngleIndex = AngleDiff/RWrapper::RW_DBSettings::MYINSTANCE()->GradeStep;
						int AngleIndexInt = (int)AngleIndex;
						double AngleIndexDiff = AngleIndex - AngleIndexInt;
						if(AngleIndexDiff > 0) AngleIndexInt += 1;
						AngGradeValue = System::Convert::ToString(RWrapper::RW_DBSettings::MYINSTANCE()->GradeLevelCollection[AngleIndexInt]);
						CurrentGradeValue = AngGradeValue;*/
					}
					AngleMeasureDimNameValue->Add("Angle");
					AngleMeasureDimNameValue->Add(gcnew System::String(Cdim->getModifiedName()));
					AngleMeasureDimNameValue->Add(MValue);
					AngleMeasureDimNameValue->Add(AngGradeValue);
				}	
			}
		}
		return AngleMeasureDimNameValue;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0027", ex); }
}

System::Collections::Generic::List<System::Collections::ArrayList^>^ RWrapper::RW_MeasureParameter::GetTotal_MeasurementListWithNT()
{
	try
	{
		System::String^ MeasureFormat = "0.0";
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT = gcnew System::Collections::Generic::List<System::Collections::ArrayList^>;
		try
		{
			for(RC_ITER CucsItem = MAINDllOBJECT->getUCSList().getList().begin(); CucsItem != MAINDllOBJECT->getUCSList().getList().end(); CucsItem++)
			{
				UCS* CurrentUcs = (UCS*)(*CucsItem).second;
				RCollectionBase& myDimCollection = CurrentUcs->getDimensions();
				for(RC_ITER CdimItem = myDimCollection.getList().begin(); CdimItem != myDimCollection.getList().end(); CdimItem++)
				{
					DimBase* Cdim = (DimBase*)(*CdimItem).second;
					if(!Cdim->IsValid()) continue;
					if(Cdim->ChildMeasurementType()) continue;
					System::String^ MeasId = System::Convert::ToString(Cdim->getId());
					if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
					{
						DimPointCoordinate* CdimPt = (DimPointCoordinate*)Cdim;
						SetDimensionForExcelWithNT(gcnew System::String(CdimPt->DimPointX->getModifiedName()), "Coordinate", System::Convert::ToString(Cdim->ParentPoint1->getX()), CdimPt->DimPointX->NominalValue(), CdimPt->DimPointX->UpperTolerance(), CdimPt->DimPointX->LowerTolerance(), CdimPt->DimPointX->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimPt->DimPointY->getModifiedName()), "Coordinate", System::Convert::ToString(Cdim->ParentPoint1->getY()), CdimPt->DimPointY->NominalValue(), CdimPt->DimPointY->UpperTolerance(), CdimPt->DimPointY->LowerTolerance(), CdimPt->DimPointY->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimPt->DimPointZ->getModifiedName()), "Coordinate", System::Convert::ToString(Cdim->ParentPoint1->getZ()), CdimPt->DimPointZ->NominalValue(), CdimPt->DimPointZ->UpperTolerance(), CdimPt->DimPointZ->LowerTolerance(), CdimPt->DimPointZ->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
					}
					else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					{
						DimThread* CdimTh = (DimThread*)(*CdimItem).second;
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdMajordia->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdMajordia->ChildMeasureValue()), CdimTh->DimThrdMajordia->NominalValue(), CdimTh->DimThrdMajordia->UpperTolerance(), CdimTh->DimThrdMajordia->LowerTolerance(), CdimTh->DimThrdMajordia->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdMinordia->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdMinordia->ChildMeasureValue()), CdimTh->DimThrdMinordia->NominalValue(), CdimTh->DimThrdMinordia->UpperTolerance(), CdimTh->DimThrdMinordia->LowerTolerance(), CdimTh->DimThrdMinordia->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdEffdiaG->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdEffdiaG->ChildMeasureValue()), CdimTh->DimThrdEffdiaG->NominalValue(), CdimTh->DimThrdEffdiaG->UpperTolerance(), CdimTh->DimThrdEffdiaG->LowerTolerance(), CdimTh->DimThrdEffdiaG->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdEffdiaM->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdEffdiaM->ChildMeasureValue()), CdimTh->DimThrdEffdiaM->NominalValue(), CdimTh->DimThrdEffdiaM->UpperTolerance(), CdimTh->DimThrdEffdiaM->LowerTolerance(), CdimTh->DimThrdEffdiaM->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdPitch->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdPitch->ChildMeasureValue()), CdimTh->DimThrdPitch->NominalValue(), CdimTh->DimThrdPitch->UpperTolerance(), CdimTh->DimThrdPitch->LowerTolerance(), CdimTh->DimThrdPitch->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						System::String^ ThreadAngle = Convert_AngleFormat(CdimTh->DimThrdAngle->ChildMeasureValue(), CMeasureMode, MeasureFormat, true);
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdAngle->getModifiedName()), "RW_Thread", ThreadAngle, CdimTh->DimThrdAngle->NominalValue(), CdimTh->DimThrdAngle->UpperTolerance(), CdimTh->DimThrdAngle->LowerTolerance(), CdimTh->DimThrdAngle->getId(), true, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdTopRadius->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdTopRadius->ChildMeasureValue()), CdimTh->DimThrdTopRadius->NominalValue(), CdimTh->DimThrdTopRadius->UpperTolerance(), CdimTh->DimThrdTopRadius->LowerTolerance(), CdimTh->DimThrdTopRadius->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdRootRadius->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdRootRadius->ChildMeasureValue()), CdimTh->DimThrdRootRadius->NominalValue(), CdimTh->DimThrdRootRadius->UpperTolerance(), CdimTh->DimThrdRootRadius->LowerTolerance(), CdimTh->DimThrdRootRadius->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						//SetDimensionForExcelWithNT(gcnew System::String(CdimTh->DimThrdPitchDia->getModifiedName()), "RW_Thread", System::Convert::ToString(CdimTh->DimThrdPitchDia->ChildMeasureValue()), CdimTh->DimThrdPitchDia->NominalValue(), CdimTh->DimThrdPitchDia->UpperTolerance(), CdimTh->DimThrdPitchDia->LowerTolerance(), CdimTh->DimThrdPitchDia->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
					}
					else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
						SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "Depth", System::Convert::ToString(Cdim->getDimension()), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
					else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE)
						SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "Deviation", System::Convert::ToString(Cdim->getDimension()), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);			
					else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT)
						SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "Area", System::Convert::ToString(Cdim->getDimension()), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);							
					else
					{
						if(Cdim->DistanceMeasurement())
						{
							if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT)
							{
								SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "SplineMeasure", gcnew System::String(Cdim->getCDimension()), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
								char ch[10];
								_itoa(Cdim->getId(), ch, 10);
								std::string temp = (const char*)(&ch[0]);
								std::string Idstr = "S" + temp;
								if(Cdim->getDimension() == Cdim->NominalValue())
									SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "SplineMeasure", Cdim->getDimension(), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), gcnew System::String((char*)Idstr.c_str()), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
								else
									SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "SplineMeasure", "-", Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), gcnew System::String((char*)Idstr.c_str()), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
							}
							else
								SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "Distance", System::Convert::ToString(Cdim->getDimension()), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->getId(), false, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						}
						else
						{
							System::String^ Angle_Value = Convert_AngleFormat(Cdim->getDimension(), CMeasureMode, MeasureFormat, false);
							SetDimensionForExcelWithNT(gcnew System::String(Cdim->getModifiedName()), "Angle", Angle_Value, Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->getId(), true, CMeasureMode, MeasureFormat, MeasureDimNameValueWithNT);
						}
					}
				}
			}
		}
		catch(Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025a", ex);
		}
		return MeasureDimNameValueWithNT;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025", ex); }
}

//Added on 09/03/2014 by vinod for hobchecker measurements............
System::Collections::Generic::List<System::Collections::ArrayList^>^ RWrapper::RW_MeasureParameter::GetTotal_MeasurementListWithNTHobChecker(int MeasurementType) //, double Radius, double TrueAngle, int NoofPtsfor_FormMeasurement)
{
	try
	{	
		//tx[0] = MAINDllOBJECT->TIS_CAxis[0]; tx[1] = MAINDllOBJECT->TIS_CAxis[1];
		//MAINDllOBJECT->GetC_Axis_XY(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], &tx[0]);
		System::Collections::Generic::List<System::Collections::ArrayList^>^ MeasureDimNameValueWithNT = gcnew System::Collections::Generic::List<System::Collections::ArrayList^>;
		try
		{	
			//Get current shape to which we have added all the points	
			ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			if(!CShape->Normalshape()) return MeasureDimNameValueWithNT;
			int PtsCt = CShape->PointsList->Pointscount();
			double currZ;
			double* runoutPts = (double*)malloc(sizeof(double) * PtsCt * 2);
			int ii = 0;
			double* OriPt = (double*)malloc(sizeof(double) * PtsCt * 4);
			double *RotatedPt = (double*)malloc(sizeof(double) * PtsCt * 4);
			SinglePoint* Sp_Mid; 
			int ProbeDir; 
			if(CShape->PointsList->Pointscount() > 1)// && NoofShapes == TargetShapeNo)
			{						
				PtsList XYPtsCollection;
				for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
				{		
					SinglePoint* Spt = (*SptItem).second;
					currZ = Spt->Z;
					ProbeDir = Spt->Pdir;
					if ((int) (ii / 2) == (int)PtsCt / 2) Sp_Mid = Spt;
					//Un-rotate the point to get the original X, Y and theta values, and for sinusoid runout correction
					RotatedPt[ii * 2] = Spt->X; RotatedPt[ii * 2 + 1] = Spt->Y; RotatedPt[ii * 2 + 2] = Spt->Z; RotatedPt[ii * 2 + 3] = Spt->R;
					//RMATH3DOBJECT->Restore_BC_Tilt(RotatedPt + ii * 2, MAINDllOBJECT->BC_Axis);
					////Collect R vs Y data in a linear array for Best-fit
					//runoutPts[ii] = OriPt[ii * 2 + 3]; //ii++;
					//runoutPts[ii + 1] = OriPt[ii * 2 + 1]; 
					ii += 2;
				}
			}
			double tx[4];
			//RMATH3DOBJECT->GetC_Axis_XY(OriPt[2], MAINDllOBJECT->TIS_CAxis, tx);
			//memcpy(tx, MAINDllOBJECT->TIS_CAxis, 3);
			tx[0] = MAINDllOBJECT->TIS_CAxis[0];
			tx[1] = MAINDllOBJECT->TIS_CAxis[1];
			tx[2] = MAINDllOBJECT->TIS_CAxis[2];

			if(MeasurementType == 1)
			{	
				if(CShape->PointsList->Pointscount() > 1) // CShape->ShapeType == RapidEnums::SHAPETYPE::LINE &&
				{						
					double rX[4] = {0};
					//RMATH3DOBJECT->GetC_Axis_XY(OriPt[2], MAINDllOBJECT->TIS_CAxis, rX);
					//memcpy(rX, MAINDllOBJECT->TIS_CAxis, 3);
					rX[0] = MAINDllOBJECT->TIS_CAxis[0];
					rX[1] = MAINDllOBJECT->TIS_CAxis[1];
					rX[2] = MAINDllOBJECT->TIS_CAxis[2];
					int index = 0; //(int)(PtsCt / 2);
					double tt, r_offset = 10000;
					
					//memcpy(OriPt, RotatedPt, PtsCt * 4);
					for (int i = 0; i < PtsCt; i ++)
						RMATH3DOBJECT->UnRotatePoint_Around_C_Axis(RotatedPt + i * 4, MAINDllOBJECT->BC_Axis, MAINDllOBJECT->TIS_CAxis, OriPt + i * 4); //MAINDllOBJECT->BC_Axis,
						//OriPt[i] = RotatedPt[i];

					for (int j = 0; j < PtsCt; j ++)
					{
						tt = OriPt[4 * j + 1] - rX[1];
						if (tt < r_offset)
						{
							r_offset = tt; index = j;
						}
					}
					double slope = 0.0, intercept = 0.0;
					//if (ProbeDir == 3) 
					//	ProbeDir = 1; 
					//else
					//	ProbeDir = -1;
					ProbeDir = RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[4];

					if (OriPt[4 * index] == rX[0])
					{
						slope = M_PI_2; 
						intercept = rX[0];
					}
					else
					{	
						slope = atan((OriPt[4 * index + 1] + ProbeDir * MAINDllOBJECT->ProbeRadius() - rX[1])/(OriPt[4 * index] - rX[0]));
						intercept = rX[1] - tan(slope) * rX[0];
					}
					if (slope < 0) slope += M_PI;

					Double max = 0, min = 0;
					cli::array<Double>^ FormErrors = gcnew cli::array<Double>(PtsCt);
					int i;
					//for (i = PtsCt - 1; i >= 0; i --)
					for (i = 0; i < PtsCt; i ++)
					{
						OriPt[i * 4 + 1] += ProbeDir * MAINDllOBJECT->ProbeRadius();
						FormErrors[i] = RMATH2DOBJECT->Pt2Line_DistSigned(OriPt[i * 4], OriPt[i * 4 + 1], slope, intercept);	// + MAINDllOBJECT->ProbeRadius()
						if (i == 0)
						{
							max = FormErrors[1]; min = FormErrors[i];
						}
						else
						{
							if (FormErrors[i] > max) max = FormErrors[i];
							if (FormErrors[i] < min) min = FormErrors[i];
						}
						SetDimensionForExcelWithNT("Err-"+System::Convert::ToString(PtsCt - i), "Distance", System::Convert::ToString(FormErrors[i]), 0.0, 0.0, 0.0, i, false, 0.0, System::Convert::ToString(0), MeasureDimNameValueWithNT);
						this->HobMeasurements[0] = abs(max - min);
					}			
				}
			}
			else if (MeasurementType == 2)
			{
				//First let us check if there is runout, and give correction for that!
				//TrueAngle = (2 * M_PI)/ (180 * (PtsCt - 1));				
				//tx[0] = MAINDllOBJECT->BC_Axis[0];
				//tx[1] = MAINDllOBJECT->BC_Axis[1];
				double Angle = M_PI * 2 / RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[6];
				double TrueAngle = Angle;
				double Radius = 0; //RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[15] / 2 - 1.125 * RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[12]; //
				double gash_pitch = RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[13];
				double Move_inZDirection = 0.0, pitch = 0.0;
				double helixangle = atan(gash_pitch / (M_PI * Radius * 2));
				pitch = abs(M_PI * RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[12] * RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[11]);
				double Rotate_AxisForHelicalLead = 0;
				//double *CorrectedPt = (double*)malloc(sizeof(double) * PtsCt);
				double *ToothAngles = (double*)malloc(sizeof(double) * PtsCt);
				cli::array<double>^ AngularPositions = gcnew cli::array<double>(PtsCt);

				double rX[4] = { 0 };

				Move_inZDirection = (pitch / RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[6]) * RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[5];
				if (gash_pitch != 0)
				{
					pitch *= sin(helixangle);
					Move_inZDirection *= sin(helixangle);
					Rotate_AxisForHelicalLead = abs(Move_inZDirection) * 2 * M_PI / gash_pitch;
				}
				Angle -= Rotate_AxisForHelicalLead;
				double RunoutParams[3] = { 0 };

				//Since there is always a runout here, we will compensate for that and neutralise that. 
				//Collect all X-Y values of all the points, calculate Circle Best fit, and calculate angles measurements from this centre, rather than the TIS_CAxis centre
				double dist = 0;
				for (int ii = 0; ii < PtsCt - 1; ii++)
				{
					OriPt[ii * 2] = RotatedPt[ii * 4];
					OriPt[ii * 2 + 1] = RotatedPt[ii * 4 + 1];
				}

				tx[0] = MAINDllOBJECT->TIS_CAxis[0];
				tx[1] = MAINDllOBJECT->TIS_CAxis[1];
				tx[2] = MAINDllOBJECT->TIS_CAxis[2];
				Radius = RMATH2DOBJECT->Pt2Pt_distance(RotatedPt, tx);
				//if (BESTFITOBJECT->Circle_BestFit(OriPt, PtsCt, tx, true, 1000000, true, 0, 1))
				//{
				//	Radius = tx[2];
				//}
				//else
				//{
				//}
				//double tempPt[3] = { 0 };
				//for (int i = 0; i < PtsCt; i++)
				//{
				//	RMATH2DOBJECT->Point_onCircle(tx, Radius, &RotatedPt[i * 4], tempPt);
				//	RotatedPt[i * 4] = tempPt[0];
				//	RotatedPt[i * 4 + 1] = tempPt[1];
				//}

				//for (int i = 0; i < PtsCt - 1; i++)//each(Pt Point_XY in XYPtsCollection)
				//{
				//	////RMATH3DOBJECT->GetC_Axis_XY(RotatedPt[i * 4 + 2], MAINDllOBJECT->TIS_CAxis, tx);
				//	//angle bw 2 consecutive pts wrt to any reference pt. to find nett. error
				//	ToothAngles[i] = RMATH2DOBJECT->AngleBwTwoPtswrtcenter(RotatedPt[i * 4], RotatedPt[i * 4 + 1], RotatedPt[(i + 1) * 4], RotatedPt[(i + 1) * 4 + 1], tx[0], tx[1]);
				//	if (gash_pitch != 0)
				//		ToothAngles[i] -= abs(RotatedPt[(i + 1) * 4 + 2] - RotatedPt[i * 4 + 2]) * 2 * M_PI / gash_pitch;

				//	ToothAngles[i] = abs(ToothAngles[i]);
				//	runoutPts[i * 2] = RotatedPt[i * 4 + 3]; //ii++;
				//	runoutPts[i * 2 + 1] = Radius * (ToothAngles[i] - ToothAngles[0]);
				//}

				//Get deviations from theoretically correct positions for each tooth. 
				for (int i = 0; i < PtsCt; i++)
				{
					AngularPositions[i] = RMATH2DOBJECT->AngleBwTwoPtswrtcenter(RotatedPt[i * 4], RotatedPt[i * 4 + 1], RotatedPt[0], RotatedPt[1], tx[0], tx[1]);
					//if (gash_pitch != 0)
					//	AngularPositions[i] -= abs(RotatedPt[(i + 1) * 4 + 2] - RotatedPt[2]) * 2 * M_PI / gash_pitch;
					AngularPositions[i] -= i * Angle; // M_PI * 2 / RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[6] - Rotate_AxisForHelicalLead);
					runoutPts[i * 2] = RotatedPt[i * 4 + 3]; //ii++;
					runoutPts[i * 2 + 1] = Radius * AngularPositions[i];
				}

				//Now let us do the sinusoid runout correction
				if (BESTFITOBJECT->Sinusoid_BestFit(runoutPts, PtsCt - 1, RunoutParams))
				{
					System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
					System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
					SfilePath = SfilePath + "\\Log\\AxisLog\\GashSpacingPts " + Microsoft::VisualBasic::DateAndTime::Now.ToString("dd-MMM-yyyy") + ".csv";
					System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
					pointsFile->WriteLine(currentTime);
					pointsFile->WriteLine(tx[0] + ", " + tx[1] + ", " + Radius); // tx[2]);
					double delta = 0;
					double MinError = 100, MaxError = 0;
					int MaxIndex = 0, MinIndex = 0;
					cli::array<Double>^ Errors = gcnew cli::array<double>(PtsCt);
					pointsFile->WriteLine(RunoutParams[0] + ", " + RunoutParams[1] + ", " + RunoutParams[2]);
					//for (int ii = 0; ii < PtsCt - 1; ii++)
					//{	//Write the values to function and keep record.
					//	delta = RunoutParams[0] + RunoutParams[2] * cos(runoutPts[ii * 2] - RunoutParams[1]); //
					//	Errors[ii] = runoutPts[ii * 2 + 1] - delta; //
					//	if (Errors[ii] > MaxError)
					//	{
					//		MaxError = Errors[ii];
					//		MaxIndex = ii;
					//	}
					//	if (Errors[ii] < MinError)
					//	{
					//		MinError = Errors[ii];
					//		MinIndex = ii;
					//	}
					//}
					//bool UpdateSinusoid = false;
					//System::Collections::Generic::List<Double>^ filteredPts = gcnew System::Collections::Generic::List<Double>();
					//if (MaxError >= 0.009 || MinError <= -0.009)
					//{
					//	UpdateSinusoid = true;
					//	for (int ii = 0; ii < PtsCt - 1; ii++)
					//	{
					//		if ((ii == MaxIndex && MaxError > 0.010) || (ii == MinIndex && MinError < -0.010))
					//			continue;
					//		else
					//		{
					//			filteredPts->Add(runoutPts[2 * ii]);
					//			filteredPts->Add(runoutPts[2 * ii + 1]);
					//		}
					//	}
					//	double* fP = (double*)malloc(filteredPts->Count * sizeof(double));
					//	for (int jj = 0; jj < filteredPts->Count; jj++)
					//		*(fP++) = filteredPts[jj];
					//	if (UpdateSinusoid && BESTFITOBJECT->Sinusoid_BestFit(fP, filteredPts->Count / 2, RunoutParams))
					//		pointsFile->WriteLine(RunoutParams[0] + ", " + RunoutParams[1] + ", " + RunoutParams[2]);
					//}

					//for (int ii = 0; ii < PtsCt; ii++)
					//{	//Write the values to function and keep record.
					//	delta = RunoutParams[0] + RunoutParams[2] * cos(runoutPts[ii * 2] - RunoutParams[1]); //
					//	runoutPts[ii * 2 + 1] -= delta; //
					//}
					pointsFile->WriteLine();
					pointsFile->Close();
				}

				int i = 0;
				double Error = 0.0, AngleBw2Pts = 0.0;// LastX = 0.0, LastY = 0.0, FirstX = 0.0, FirstY = 0.0;
				cli::array<double> ^HobGashSpacingValues, ^AdjacentGashErrors; //, ^ArrangedSpacingValues, ^HobGashSpacingDiffValues;
				HobGashSpacingValues = gcnew cli::array<double>(PtsCt);
				AdjacentGashErrors = gcnew cli::array<double>(PtsCt);
				Double max = 0, min = 0;
				for (i = 0; i < PtsCt - 1; i++)//each(Pt Point_XY in XYPtsCollection)
				{
					HobGashSpacingValues[i] = runoutPts[i * 2 + 1]; // - runoutPts[1];
					if (i == 0)
					{
						max = HobGashSpacingValues[i]; min = HobGashSpacingValues[i];
					}
					else
					{
						if (HobGashSpacingValues[i] > max) max = HobGashSpacingValues[i];
						if (HobGashSpacingValues[i] < min) min = HobGashSpacingValues[i];
					}
					SetDimensionForExcelWithNT("Error-" + System::Convert::ToString(i), "Distance", System::Convert::ToString(HobGashSpacingValues[i]), 0.0, 0.0, 0.0, i, false, 0.0, System::Convert::ToString(0), MeasureDimNameValueWithNT);
				}
				HobMeasurements[2] = abs(max - min); // Cumulative Pitch error recorded

				max = HobGashSpacingValues[0] - HobGashSpacingValues[1];
				min = max;
				for (int i = 0; i < PtsCt; i++)
				{
					if (i < PtsCt - 1)
						AdjacentGashErrors[i] = HobGashSpacingValues[i] - HobGashSpacingValues[i + 1];
					else
						AdjacentGashErrors[i] = HobGashSpacingValues[i] - HobGashSpacingValues[0];

					if (AdjacentGashErrors[i] > max) max = AdjacentGashErrors[i];
					if (AdjacentGashErrors[i] < min) min = AdjacentGashErrors[i];
				}
				HobMeasurements[1] = max(abs(max), abs(min)); //Adjacent Pitch error noted...
				free(ToothAngles);
			}
			else if(MeasurementType == 3)
			{
				double y0 = OriPt[1]; //(int)(PtsCt / 2) +
				double dist = 0.0, radius0 = 0, xError = 0.0;
				double indexAngle, radius, anglePositionCorr, yCorrection;
				double rX[4] = {0};
				double HelixAngle, actualZ, C_Deviation;
				//array<double> ^FormDeviationValues, ^ArrangedDeviationValues;
				//FormDeviationValues = gcnew cli::array<double>(PtsCt);
				//ArrangedDeviationValues = gcnew cli::array<double>(PtsCt);
				int i;
				RWrapper::PointsToGo^ ProbePath_LstEntity;
				double gash_pitch = RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[13];
				double runout_tilt = 0;

				for (int i = 0; i < PtsCt; i ++)
					RMATH3DOBJECT->UnRotatePoint_Around_C_Axis(RotatedPt + i * 4, MAINDllOBJECT->BC_Axis, MAINDllOBJECT->TIS_CAxis, OriPt + i * 4); //MAINDllOBJECT->BC_Axis,

				for(i = 0; i < PtsCt; i ++)
				{	
					if (gash_pitch == 0)
					{	
						//runout_tilt = (OriPt[4 * i + 2] - OriPt[2]) * tan(MAINDllOBJECT->TIS_CAxis[4]); //RWrapper::RW_DelphiCamModule::MYINSTANCE()->Hob_RunoutParams[4]);
							//y0 = tx[1] - rX[1];
						dist = RotatedPt[4 * i + 1] - RotatedPt[1]; // - runout_tilt;// - y0;
					}
					else
					{
						RMATH3DOBJECT->GetC_Axis_XY(RotatedPt[4 * i + 2] - MAINDllOBJECT->TIS_CAxis[2], MAINDllOBJECT->BC_Axis, tx);
						tx[0] += MAINDllOBJECT->TIS_CAxis[0];
						tx[1] += MAINDllOBJECT->TIS_CAxis[1];
						//Correct for rotational position error
						if (i > 0)
						{	
							//NExt ALgorithm - 28 Jun 14
							//Calculate the radius of each point taken. Take Rad1 of first point. R-R0 is the lead error!
							//Correct for X-position variation! 
							radius  = sqrt(pow((RotatedPt[4 * i] - tx[0]), 2) + pow((RotatedPt[4 * i + 1] - tx[1]), 2));
							xError = OriPt[4 * i] - OriPt[4 * (i - 1)];
							dist = radius - radius0 - xError; // - runout_tilt;
						}
						else
						{
							dist = 0;
							radius0 = sqrt(pow((RotatedPt[0] - tx[0]), 2) + pow((RotatedPt[1] - tx[1]), 2));
						}
					}
					runoutPts[i * 2] = RotatedPt[4 * i + 2];
					runoutPts[i * 2 + 1] = dist;
					//FormDeviationValues[i] = dist;
				}
				double gashLeadLine[6];
				BESTFITOBJECT->Line_BestFit_2D(runoutPts, PtsCt, gashLeadLine, true);
				System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
				System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
				SfilePath = SfilePath + "\\Log\\AxisLog\\GashLeadValues " + Microsoft::VisualBasic::DateAndTime::Now.ToString("dd-MMM-yyyy") + ".csv";
				System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
				pointsFile->WriteLine(currentTime);
				pointsFile->WriteLine(gashLeadLine[0] + ", " + gashLeadLine[1]); // tx[2]);
				pointsFile->Close();
				Double max = 0, min = 0;
				cli::array<Double>^ LeadErrors = gcnew cli::array<Double>(PtsCt);

				for (int i = 0; i < PtsCt; i++)
				{
					LeadErrors[i] = runoutPts[2 * i] * gashLeadLine[0] + gashLeadLine[1];
					if (i == 0)
					{
						max = LeadErrors[i]; min = LeadErrors[i];
					}
					else
					{
						if (LeadErrors[i] > max) max = LeadErrors[i];
						if (LeadErrors[i] < min) min = LeadErrors[i];
					}
					SetDimensionForExcelWithNT("Err-" + System::Convert::ToString(i), "Distance", System::Convert::ToString(LeadErrors[i]), 0.0, 0.0, 0.0, i, false, 0.0, System::Convert::ToString(0), MeasureDimNameValueWithNT);

				}
				HobMeasurements[3] = abs(max - min); //Gash Lead Error
				//Array::Copy(FormDeviationValues, ArrangedDeviationValues, FormDeviationValues->Length - 1);
				//Array::Sort(ArrangedDeviationValues);
				//double totalDeviation = abs(ArrangedDeviationValues[FormDeviationValues->Length - 1] - ArrangedDeviationValues[0]);
				//SetDimensionForExcelWithNT("Lead-Err", "Distance", System::Convert::ToString(totalDeviation), 0.0, 0.0, 0.0, i, false, 0.0, System::Convert::ToString(0), MeasureDimNameValueWithNT);
			}
			free(runoutPts); free(OriPt); free(RotatedPt);
		}
		catch(Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025aA", ex);
		}
		return MeasureDimNameValueWithNT;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025A", ex); }
}


//Added on 29 May 12 by Rahul............
void RWrapper::RW_MeasureParameter::SelectedMeasurementParameter(RCollectionBase* Measurecollselected)
{
	try
	{
		SelectedMeasureIDList->Clear();
		for(RC_ITER CdimItem = Measurecollselected->getList().begin(); CdimItem != Measurecollselected->getList().end(); CdimItem++)
		{
			DimBase* Cdim = (DimBase*)(*CdimItem).second;
			RWrapper::RW_MeasureParameter::MYINSTANCE()->SelectedMeasureIDList->Add(Cdim->getId());
		}
		MeasureParamUpdateEvent::raise(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_SELECTION_CHANGED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025", ex); }
}

//Added on 29 May 12 ..........
void RWrapper::RW_MeasureParameter::RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE Mtype)
{
	try
	{
		MeasureParamUpdateEvent::raise(Mtype);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025", ex); }
}

//Added on 29 May 12 ............
void RWrapper::RW_MeasureParameter::AddMeasurementParameter(BaseItem* item)
{
	try
	{
		System::String^ MeasureFormat = "0.0";
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";

		MeasureParameter_ArrayList->Clear();
		DimBase* Cdim = (DimBase*)(item);
		System::Collections::ArrayList^ MeasureList = gcnew System::Collections::ArrayList;
		if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
		{
			DimPointCoordinate* CdimPt = (DimPointCoordinate*)Cdim;
			MeasureList->Add(CdimPt->getId()), MeasureList->Add(CdimPt->DimPointX->getId()), MeasureList->Add(gcnew System::String(CdimPt->DimPointX->getModifiedName())), MeasureList->Add("Coordinate"), MeasureList->Add(Microsoft::VisualBasic::Strings::Format(CdimPt->ParentPoint1->getX(), MeasureFormat)), MeasureList->Add(CdimPt->DimPointX->NominalValue()), MeasureList->Add(CdimPt->DimPointX->UpperTolerance()), MeasureList->Add(CdimPt->DimPointX->LowerTolerance()), MeasureList->Add(CdimPt->DimPointX->Include_PassFail()), MeasureList->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList);
			System::Collections::ArrayList^ MeasureList1 = gcnew System::Collections::ArrayList;
			MeasureList1->Add(CdimPt->getId()), MeasureList1->Add(CdimPt->DimPointY->getId()), MeasureList1->Add(gcnew System::String(CdimPt->DimPointY->getModifiedName())), MeasureList1->Add("Coordinate"), MeasureList1->Add(Microsoft::VisualBasic::Strings::Format(CdimPt->ParentPoint1->getY(), MeasureFormat)), MeasureList1->Add(CdimPt->DimPointY->NominalValue()), MeasureList1->Add(CdimPt->DimPointY->UpperTolerance()), MeasureList1->Add(CdimPt->DimPointY->LowerTolerance()), MeasureList1->Add(CdimPt->DimPointY->Include_PassFail()), MeasureList1->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList1);
			System::Collections::ArrayList^ MeasureList2 = gcnew System::Collections::ArrayList;
			MeasureList2->Add(CdimPt->getId()), MeasureList2->Add(CdimPt->DimPointZ->getId()), MeasureList2->Add(gcnew System::String(CdimPt->DimPointZ->getModifiedName())), MeasureList2->Add("Coordinate"), MeasureList2->Add(Microsoft::VisualBasic::Strings::Format(CdimPt->ParentPoint1->getZ(), MeasureFormat)), MeasureList2->Add(CdimPt->DimPointZ->NominalValue()), MeasureList2->Add(CdimPt->DimPointZ->UpperTolerance()), MeasureList2->Add(CdimPt->DimPointZ->LowerTolerance()), MeasureList2->Add(CdimPt->DimPointZ->Include_PassFail()), MeasureList2->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList2);
		}
		else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
		{
			DimThread* CdimTh = (DimThread*)(item);	
			MeasureList->Add(CdimTh->DimThrdMajordia->getId()), MeasureList->Add(CdimTh->DimThrdMajordia->getId()), MeasureList->Add(gcnew System::String(CdimTh->DimThrdMajordia->getModifiedName())), MeasureList->Add("RW_Thread"), MeasureList->Add(Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdMajordia->ChildMeasureValue(), MeasureFormat)), MeasureList->Add(CdimTh->DimThrdMajordia->NominalValue()), MeasureList->Add(CdimTh->DimThrdMajordia->UpperTolerance()), MeasureList->Add(CdimTh->DimThrdMajordia->LowerTolerance()), MeasureList->Add(CdimTh->DimThrdMajordia->Include_PassFail()), MeasureList->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList);
			System::Collections::ArrayList^ MeasureList1 = gcnew System::Collections::ArrayList;
			MeasureList1->Add(CdimTh->DimThrdMinordia->getId()), MeasureList1->Add(CdimTh->DimThrdMinordia->getId()), MeasureList1->Add(gcnew System::String(CdimTh->DimThrdMinordia->getModifiedName())), MeasureList1->Add("RW_Thread"), MeasureList1->Add(Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdMinordia->ChildMeasureValue(), MeasureFormat)), MeasureList1->Add(CdimTh->DimThrdMinordia->NominalValue()), MeasureList1->Add(CdimTh->DimThrdMinordia->UpperTolerance()), MeasureList1->Add(CdimTh->DimThrdMinordia->LowerTolerance()), MeasureList1->Add(CdimTh->DimThrdMinordia->Include_PassFail()), MeasureList1->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList1);
			System::Collections::ArrayList^ MeasureList2 = gcnew System::Collections::ArrayList;
			MeasureList2->Add(CdimTh->DimThrdEffdiaG->getId()), MeasureList2->Add(CdimTh->DimThrdEffdiaG->getId()), MeasureList2->Add(gcnew System::String(CdimTh->DimThrdEffdiaG->getModifiedName())), MeasureList2->Add("RW_Thread"), MeasureList2->Add(Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdEffdiaG->ChildMeasureValue(), MeasureFormat)), MeasureList2->Add(CdimTh->DimThrdEffdiaG->NominalValue()), MeasureList2->Add(CdimTh->DimThrdEffdiaG->UpperTolerance()), MeasureList2->Add(CdimTh->DimThrdEffdiaG->LowerTolerance()), MeasureList2->Add(CdimTh->DimThrdEffdiaG->Include_PassFail()), MeasureList2->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList2);
			System::Collections::ArrayList^ MeasureList3 = gcnew System::Collections::ArrayList;
			MeasureList3->Add(CdimTh->DimThrdEffdiaM->getId()), MeasureList3->Add(CdimTh->DimThrdEffdiaM->getId()), MeasureList3->Add(gcnew System::String(CdimTh->DimThrdEffdiaM->getModifiedName())), MeasureList3->Add("RW_Thread"), MeasureList3->Add(Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdEffdiaM->ChildMeasureValue(), MeasureFormat)), MeasureList3->Add(CdimTh->DimThrdEffdiaM->NominalValue()), MeasureList3->Add(CdimTh->DimThrdEffdiaM->UpperTolerance()), MeasureList3->Add(CdimTh->DimThrdEffdiaM->LowerTolerance()), MeasureList3->Add(CdimTh->DimThrdEffdiaM->Include_PassFail()), MeasureList3->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList3);
			System::Collections::ArrayList^ MeasureList4 = gcnew System::Collections::ArrayList;
			MeasureList4->Add(CdimTh->DimThrdPitch->getId()), MeasureList4->Add(CdimTh->DimThrdPitch->getId()), MeasureList4->Add(gcnew System::String(CdimTh->DimThrdPitch->getModifiedName())), MeasureList4->Add("RW_Thread"), MeasureList4->Add(Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdPitch->ChildMeasureValue(), MeasureFormat)), MeasureList4->Add(CdimTh->DimThrdPitch->NominalValue()), MeasureList4->Add(CdimTh->DimThrdPitch->UpperTolerance()), MeasureList4->Add(CdimTh->DimThrdPitch->LowerTolerance()), MeasureList4->Add(CdimTh->DimThrdPitch->Include_PassFail()), MeasureList4->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList4);
			System::String^ ThreadAngle = Convert_AngleFormat(CdimTh->DimThrdAngle->ChildMeasureValue(), CMeasureMode, MeasureFormat, true);
			System::String^ CNominal = Convert_AngleFormat(CdimTh->DimThrdAngle->NominalValue(), CMeasureMode, MeasureFormat, true);
			System::String^ Cusl = Convert_AngleFormat(CdimTh->DimThrdAngle->UpperTolerance(), CMeasureMode, MeasureFormat, true);
			System::String^ Clsl = Convert_AngleFormat(CdimTh->DimThrdAngle->LowerTolerance(), CMeasureMode, MeasureFormat, true);
			System::Collections::ArrayList^ MeasureList5 = gcnew System::Collections::ArrayList;
			MeasureList5->Add(CdimTh->DimThrdAngle->getId()), MeasureList5->Add(CdimTh->DimThrdAngle->getId()), MeasureList5->Add(gcnew System::String(CdimTh->DimThrdAngle->getModifiedName())), MeasureList5->Add("Angle"), MeasureList5->Add(ThreadAngle), MeasureList5->Add(CNominal), MeasureList5->Add(Cusl), MeasureList5->Add(Clsl), MeasureList5->Add(CdimTh->DimThrdAngle->Include_PassFail()), MeasureList5->Add(false);
			MeasureParameter_ArrayList->Add(MeasureList5);
			System::Collections::ArrayList^ MeasureList6 = gcnew System::Collections::ArrayList;
			MeasureList6->Add(CdimTh->DimThrdTopRadius->getId()), MeasureList6->Add(CdimTh->DimThrdTopRadius->getId()), MeasureList6->Add(gcnew System::String(CdimTh->DimThrdTopRadius->getModifiedName())), MeasureList6->Add("RW_Thread"), MeasureList6->Add(Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdTopRadius->ChildMeasureValue(), MeasureFormat)), MeasureList6->Add(CdimTh->DimThrdTopRadius->NominalValue()), MeasureList6->Add(CdimTh->DimThrdTopRadius->UpperTolerance()), MeasureList6->Add(CdimTh->DimThrdTopRadius->LowerTolerance()), MeasureList6->Add(CdimTh->DimThrdTopRadius->Include_PassFail()), MeasureList6->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList6);
			System::Collections::ArrayList^ MeasureList7 = gcnew System::Collections::ArrayList;
			MeasureList7->Add(CdimTh->DimThrdRootRadius->getId()), MeasureList7->Add(CdimTh->DimThrdRootRadius->getId()), MeasureList7->Add(gcnew System::String(CdimTh->DimThrdRootRadius->getModifiedName())), MeasureList7->Add("RW_Thread"), MeasureList7->Add(CdimTh->DimThrdRootRadius->ChildMeasureValue()), MeasureList7->Add(CdimTh->DimThrdRootRadius->NominalValue()), MeasureList7->Add(CdimTh->DimThrdRootRadius->UpperTolerance()), MeasureList7->Add(CdimTh->DimThrdRootRadius->LowerTolerance()), MeasureList7->Add(CdimTh->DimThrdRootRadius->Include_PassFail()), MeasureList7->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList7);
			System::Collections::ArrayList^ MeasureList8 = gcnew System::Collections::ArrayList;
			MeasureList8->Add(CdimTh->DimThrdPitchDia->getId()), MeasureList8->Add(CdimTh->DimThrdPitchDia->getId()), MeasureList8->Add(gcnew System::String(CdimTh->DimThrdPitchDia->getModifiedName())), MeasureList8->Add("RW_Thread"), MeasureList8->Add(Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdPitchDia->ChildMeasureValue(), MeasureFormat)), MeasureList8->Add(CdimTh->DimThrdPitchDia->NominalValue()), MeasureList8->Add(CdimTh->DimThrdPitchDia->UpperTolerance()), MeasureList8->Add(CdimTh->DimThrdPitchDia->LowerTolerance()), MeasureList8->Add(CdimTh->DimThrdPitchDia->Include_PassFail()), MeasureList8->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList8);
		}
		else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
		{
			MeasureList->Clear();
			MeasureList->Add(Cdim->getId()), MeasureList->Add(Cdim->getId()), MeasureList->Add(gcnew System::String(Cdim->getModifiedName())), MeasureList->Add("Depth"), MeasureList->Add(Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat)), MeasureList->Add(Cdim->NominalValue()), MeasureList->Add(Cdim->UpperTolerance()), MeasureList->Add(Cdim->LowerTolerance()), MeasureList->Add(Cdim->Include_PassFail()), MeasureList->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList);
		}
		else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE)
		{
			MeasureList->Clear();
			MeasureList->Add(Cdim->getId()), MeasureList->Add(Cdim->getId()), MeasureList->Add(gcnew System::String(Cdim->getModifiedName())), MeasureList->Add("Deviation"), MeasureList->Add(Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat)), MeasureList->Add(Cdim->NominalValue()), MeasureList->Add(Cdim->UpperTolerance()), MeasureList->Add(Cdim->LowerTolerance()), MeasureList->Add(Cdim->Include_PassFail()), MeasureList->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList);
		}
		else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT)
		{
			MeasureList->Clear();
			MeasureList->Add(Cdim->getId()), MeasureList->Add(Cdim->getId()), MeasureList->Add(gcnew System::String(Cdim->getModifiedName())), MeasureList->Add("Area"), MeasureList->Add(Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat)), MeasureList->Add(Cdim->NominalValue()), MeasureList->Add(Cdim->UpperTolerance()), MeasureList->Add(Cdim->LowerTolerance()), MeasureList->Add(Cdim->Include_PassFail()), MeasureList->Add(true);
			MeasureParameter_ArrayList->Add(MeasureList);
		}
		else
		{
			if(Cdim->DistanceMeasurement())
			{
				if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT)
				{
					MeasureList->Clear();
					MeasureList->Add(Cdim->getId()), MeasureList->Add(Cdim->getId()), MeasureList->Add(gcnew System::String(Cdim->getModifiedName())), MeasureList->Add("SplineMeasure"), MeasureList->Add(gcnew System::String(Cdim->getCDimension())), MeasureList->Add(Cdim->NominalValue()), MeasureList->Add(Cdim->UpperTolerance()), MeasureList->Add(Cdim->LowerTolerance()), MeasureList->Add(Cdim->Include_PassFail()), MeasureList->Add(true);
					MeasureParameter_ArrayList->Add(MeasureList);
				}
				else
				{
					MeasureList->Clear();
					MeasureList->Add(Cdim->getId()), MeasureList->Add(Cdim->getId()), MeasureList->Add(gcnew System::String(Cdim->getModifiedName())), MeasureList->Add("Distance"), MeasureList->Add(Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat)), MeasureList->Add(Cdim->NominalValue()), MeasureList->Add(Cdim->UpperTolerance()), MeasureList->Add(Cdim->LowerTolerance()), MeasureList->Add(Cdim->Include_PassFail()), MeasureList->Add(true);
					MeasureParameter_ArrayList->Add(MeasureList);
				}
			}
			else
			{
				System::String^ Angle_Value = Convert_AngleFormat(Cdim->getDimension(), CMeasureMode, MeasureFormat, false);
				System::String^ CNominal = Convert_AngleFormat(Cdim->NominalValue(), CMeasureMode, MeasureFormat, true);
				System::String^ Cusl = Convert_AngleFormat(Cdim->UpperTolerance(), CMeasureMode, MeasureFormat, true);
				System::String^ Clsl = Convert_AngleFormat(Cdim->LowerTolerance(), CMeasureMode, MeasureFormat, true);
				MeasureList->Clear();
				MeasureList->Add(Cdim->getId()), MeasureList->Add(Cdim->getId()), MeasureList->Add(gcnew System::String(Cdim->getModifiedName())), MeasureList->Add("Angle"), MeasureList->Add(Angle_Value), MeasureList->Add(CNominal), MeasureList->Add(Cusl), MeasureList->Add(Clsl), MeasureList->Add(Cdim->Include_PassFail()), MeasureList->Add(false);
				MeasureParameter_ArrayList->Add(MeasureList);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0025", ex); }
}

void RWrapper::RW_MeasureParameter::SetMeasurementParamForTable()
{
	try
	{
		SelectedMeasureIDList->Clear();
		System::String^ MeasureFormat = "0.0";
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";

		System::Data::DataTable^ DTtable = gcnew System::Data::DataTable();
		DTtable->Columns->Add("ParentMeasureID");
		DTtable->Columns->Add("ChildMeasureID");
		DTtable->Columns->Add("Measure Type");
		DTtable->Columns->Add("Measure Name");
		DTtable->Columns->Add("Value");
		DTtable->Columns->Add("Nominal");
		DTtable->Columns->Add("USL");
		DTtable->Columns->Add("LSL");
		DTtable->Columns->Add("Critical Measurement");
		DTtable->Columns->Add("Distance Mode");
		

		RCollectionBase& myDimCollection = MAINDllOBJECT->getDimList();
		for(RC_ITER CdimItem = myDimCollection.getList().begin(); CdimItem != myDimCollection.getList().end(); CdimItem++)
		{
			DimBase* Cdim = (DimBase*)(*CdimItem).second;
			if(Cdim->selected())
				SelectedMeasureIDList->Add(Cdim->getId());
	
			if(Cdim->ChildMeasurementType()) continue;
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
			{
				DimPointCoordinate* CdimPt = (DimPointCoordinate*)Cdim;
				DTtable->Rows->Add(CdimPt->getId(), CdimPt->DimPointX->getId(), gcnew System::String(CdimPt->DimPointX->getModifiedName()), "Coordinate", Microsoft::VisualBasic::Strings::Format(Cdim->ParentPoint1->getX(), MeasureFormat), CdimPt->DimPointX->NominalValue(), CdimPt->DimPointX->UpperTolerance(), CdimPt->DimPointX->LowerTolerance(), CdimPt->DimPointX->Include_PassFail(), true);
				DTtable->Rows->Add(CdimPt->getId(), CdimPt->DimPointY->getId(), gcnew System::String(CdimPt->DimPointY->getModifiedName()), "Coordinate", Microsoft::VisualBasic::Strings::Format(Cdim->ParentPoint1->getY(), MeasureFormat), CdimPt->DimPointY->NominalValue(), CdimPt->DimPointY->UpperTolerance(), CdimPt->DimPointY->LowerTolerance(), CdimPt->DimPointY->Include_PassFail(), true);
				DTtable->Rows->Add(CdimPt->getId(), CdimPt->DimPointZ->getId(), gcnew System::String(CdimPt->DimPointZ->getModifiedName()), "Coordinate", Microsoft::VisualBasic::Strings::Format(Cdim->ParentPoint1->getZ(), MeasureFormat), CdimPt->DimPointZ->NominalValue(), CdimPt->DimPointZ->UpperTolerance(), CdimPt->DimPointZ->LowerTolerance(), CdimPt->DimPointZ->Include_PassFail(), true);
			}
			else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
			{
				DimThread* CdimTh = (DimThread*)(*CdimItem).second;
				DTtable->Rows->Add(CdimTh->DimThrdMajordia->getId(), CdimTh->DimThrdMajordia->getId(), gcnew System::String(CdimTh->DimThrdMajordia->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdMajordia->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdMajordia->NominalValue(), CdimTh->DimThrdMajordia->UpperTolerance(), CdimTh->DimThrdMajordia->LowerTolerance(), CdimTh->DimThrdMajordia->Include_PassFail(), true);
				DTtable->Rows->Add(CdimTh->DimThrdMinordia->getId(), CdimTh->DimThrdMinordia->getId(), gcnew System::String(CdimTh->DimThrdMinordia->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdMinordia->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdMinordia->NominalValue(), CdimTh->DimThrdMinordia->UpperTolerance(), CdimTh->DimThrdMinordia->LowerTolerance(), CdimTh->DimThrdMinordia->Include_PassFail(), true);
				DTtable->Rows->Add(CdimTh->DimThrdEffdiaG->getId(), CdimTh->DimThrdEffdiaG->getId(), gcnew System::String(CdimTh->DimThrdEffdiaG->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdEffdiaG->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdEffdiaG->NominalValue(), CdimTh->DimThrdEffdiaG->UpperTolerance(), CdimTh->DimThrdEffdiaG->LowerTolerance(), CdimTh->DimThrdEffdiaG->Include_PassFail(), true);
				DTtable->Rows->Add(CdimTh->DimThrdEffdiaM->getId(), CdimTh->DimThrdEffdiaM->getId(), gcnew System::String(CdimTh->DimThrdEffdiaM->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdEffdiaM->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdEffdiaM->NominalValue(), CdimTh->DimThrdEffdiaM->UpperTolerance(), CdimTh->DimThrdEffdiaM->LowerTolerance(), CdimTh->DimThrdEffdiaM->Include_PassFail(), true);
				DTtable->Rows->Add(CdimTh->DimThrdPitch->getId(), CdimTh->DimThrdPitch->getId(), gcnew System::String(CdimTh->DimThrdPitch->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdPitch->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdPitch->NominalValue(), CdimTh->DimThrdPitch->UpperTolerance(), CdimTh->DimThrdPitch->LowerTolerance(), CdimTh->DimThrdPitch->Include_PassFail(), true);
				System::String^ ThreadAngle = Convert_AngleFormat(CdimTh->DimThrdAngle->ChildMeasureValue(), CMeasureMode, MeasureFormat, true);
				System::String^ CNominal = Convert_AngleFormat(CdimTh->DimThrdAngle->NominalValue(), CMeasureMode, MeasureFormat, true);
				System::String^ Cusl = Convert_AngleFormat(CdimTh->DimThrdAngle->UpperTolerance(), CMeasureMode, MeasureFormat, true);
				System::String^ Clsl = Convert_AngleFormat(CdimTh->DimThrdAngle->LowerTolerance(), CMeasureMode, MeasureFormat, true);
				DTtable->Rows->Add(CdimTh->DimThrdAngle->getId(), CdimTh->DimThrdAngle->getId(), gcnew System::String(CdimTh->DimThrdAngle->getModifiedName()), "Angle", ThreadAngle, CNominal, Cusl, Clsl, CdimTh->DimThrdAngle->Include_PassFail(), false);
				DTtable->Rows->Add(CdimTh->DimThrdTopRadius->getId(), CdimTh->DimThrdTopRadius->getId(), gcnew System::String(CdimTh->DimThrdTopRadius->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdTopRadius->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdTopRadius->NominalValue(), CdimTh->DimThrdTopRadius->UpperTolerance(), CdimTh->DimThrdTopRadius->LowerTolerance(), CdimTh->DimThrdTopRadius->Include_PassFail(), true);
				DTtable->Rows->Add(CdimTh->DimThrdRootRadius->getId(), CdimTh->DimThrdRootRadius->getId(), gcnew System::String(CdimTh->DimThrdRootRadius->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdRootRadius->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdRootRadius->NominalValue(), CdimTh->DimThrdRootRadius->UpperTolerance(), CdimTh->DimThrdRootRadius->LowerTolerance(), CdimTh->DimThrdRootRadius->Include_PassFail(), true);
				DTtable->Rows->Add(CdimTh->DimThrdPitchDia->getId(), CdimTh->DimThrdPitchDia->getId(), gcnew System::String(CdimTh->DimThrdPitchDia->getModifiedName()), "RW_Thread", Microsoft::VisualBasic::Strings::Format(CdimTh->DimThrdPitchDia->ChildMeasureValue(), MeasureFormat), CdimTh->DimThrdPitchDia->NominalValue(), CdimTh->DimThrdPitchDia->UpperTolerance(), CdimTh->DimThrdPitchDia->LowerTolerance(), CdimTh->DimThrdPitchDia->Include_PassFail(), true);
			}
			else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				DTtable->Rows->Add(Cdim->getId(), Cdim->getId(), gcnew System::String(Cdim->getModifiedName()), "Depth", Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->Include_PassFail(), true);
			else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE)
				DTtable->Rows->Add(Cdim->getId(), Cdim->getId(), gcnew System::String(Cdim->getModifiedName()), "Deviation", Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->Include_PassFail(), true);
			else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT)
				DTtable->Rows->Add(Cdim->getId(), Cdim->getId(), gcnew System::String(Cdim->getModifiedName()), "Area", Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->Include_PassFail(), true);
			else
			{
				if(Cdim->DistanceMeasurement())
				{
					if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT)
						DTtable->Rows->Add(Cdim->getId(), Cdim->getId(), gcnew System::String(Cdim->getModifiedName()), "SplineMeasure", gcnew System::String(Cdim->getCDimension()), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->Include_PassFail(), true);
					else
						DTtable->Rows->Add(Cdim->getId(), Cdim->getId(), gcnew System::String(Cdim->getModifiedName()), "Distance", Microsoft::VisualBasic::Strings::Format(Cdim->getDimension(), MeasureFormat), Cdim->NominalValue(), Cdim->UpperTolerance(), Cdim->LowerTolerance(), Cdim->Include_PassFail(), true);
				}
				else
				{
					System::String^ Angle_Value = Convert_AngleFormat(Cdim->getDimension(), CMeasureMode, MeasureFormat, false);
					System::String^ CNominal = Convert_AngleFormat(Cdim->NominalValue(), CMeasureMode, MeasureFormat, true);
					System::String^ Cusl = Convert_AngleFormat(Cdim->UpperTolerance(), CMeasureMode, MeasureFormat, true);
					System::String^ Clsl = Convert_AngleFormat(Cdim->LowerTolerance(), CMeasureMode, MeasureFormat, true);
					DTtable->Rows->Add(Cdim->getId(), Cdim->getId(), gcnew System::String(Cdim->getModifiedName()), "Angle", Angle_Value, CNominal, Cusl, Clsl, Cdim->Include_PassFail(), false);
				}
			}
		}
		delete MeasurementParamTable;
		MeasurementParamTable = DTtable;
		RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_TABLE_UPDATE);
		RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_SELECTION_CHANGED);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0041b", ex);
	}
}

void RWrapper::RW_MeasureParameter::SelectMeasurement(int Measureid)
 {
	 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 MAINDllOBJECT->selectMeasurement(Measureid, MAINDllOBJECT->ControlOn);
 }

void RWrapper::RW_MeasureParameter::SelectMeasurement(cli::array<int, 1>^ IdList)
 {
	  MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	  MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	  std::list<int> PtIdlist;
	  for each(System::Int32^ Cvalue in IdList)
	  {
		  int temp = (int)Cvalue;
		  PtIdlist.push_back(temp);
	  }
	  MAINDllOBJECT->selectMeasurement(&PtIdlist);
 }

void RWrapper::RW_MeasureParameter::DeleteSelectedMesurement()
 {
	 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 MAINDllOBJECT->deleteMeasurement();
 }

void RWrapper::RW_MeasureParameter::RenameMeasurement(System::String^ Str)
{
	try
	{
		DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().selectedItem();
		Cdim->setModifiedName((char*)(void*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
		if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
		{
			DimPointCoordinate* dimPt = (DimPointCoordinate*)Cdim;
			System::String^ Str1 = Str + "_X";
			dimPt->DimPointX->setModifiedName((char*)(void*)Marshal::StringToHGlobalAnsi(Str1).ToPointer());
			Str1 = Str + "_Y";
			dimPt->DimPointY->setModifiedName((char*)(void*)Marshal::StringToHGlobalAnsi(Str1).ToPointer());
			Str1 = Str + "_Z";
			dimPt->DimPointZ->setModifiedName((char*)(void*)Marshal::StringToHGlobalAnsi(Str1).ToPointer());
			RWrapper::RW_MeasureParameter::MYINSTANCE()->AddMeasurementParameter(((BaseItem*)Cdim));
			RWrapper::RW_MeasureParameter::MYINSTANCE()->RaiseMeasurementEvent(RWrapper::RW_Enum::RW_MEASUREPARAM_TYPE::MEASUREMENT_UPDATE);
		}
		MAINDllOBJECT->Measurement_updated();	
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_MeasureParameter::RenameMeasurement(System::String^ Str, int MeasureId)
{
	try
	{
		DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().getList()[MeasureId];
		Cdim->setModifiedName((char*)(void*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
		MAINDllOBJECT->Measurement_updated();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_MeasureParameter::Change_MeasurementFontSize(int Fsize)
{
	try
	{
		MAINDllOBJECT->SeMeasurementFontSize(Fsize);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_MeasureParameter::SetMeasureNominalTolerance(double NVal, double usl, double lsl)
{
	try
	{
		DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().selectedItem();
		if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT) return;
		if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE && Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD &&
			Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_DEPTH && Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE &&
			Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT)
		{
			if(!Cdim->DistanceMeasurement())
			{
				int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
				if(CMeasureMode == 1)
				{
					NVal = NVal * M_PI / 180;
					usl = usl * M_PI / 180;
					lsl = lsl * M_PI / 180;
				}
			}
		}
		Cdim->NominalValue(NVal);
		Cdim->UpperTolerance(usl);
		Cdim->LowerTolerance(lsl);
		Cdim->UpdateMeasurementColor();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_MeasureParameter::SetMeasureNominalTolerance(double NVal, double usl, double lsl, int MeasureId)
{
	try
	{
		DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().getList()[MeasureId];
		if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT) return;
		if (Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE && Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD &&
			Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_DEPTH && Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE &&
			Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT)
		{
			if (!Cdim->DistanceMeasurement())
			{
				int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
				if (CMeasureMode == 1)
				{
					NVal = NVal * M_PI / 180;
					usl = usl * M_PI / 180;
					lsl = lsl * M_PI / 180;
				}
			}
		}
		Cdim->NominalValue(NVal);
		Cdim->UpperTolerance(usl);
		Cdim->LowerTolerance(lsl);
		Cdim->UpdateMeasurementColor();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_MeasureParameter::SetMeasureIncludePF(bool IncludePF, int MeasureId)
{
	try
	{
		DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().getList()[MeasureId];
		Cdim->Include_PassFail(IncludePF);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_MeasureParameter::SetMeasurementUnit(int type)
{
	try
	{
		MAINDllOBJECT->SetMeasurementUnit(RapidEnums::RAPIDUNITS(type));
		RWrapper::RW_MainInterface::MYINSTANCE()->RefreshDRO();
		RWrapper::RW_MeasureParameter::MYINSTANCE()->SetMeasurementParamForTable();
		RWrapper::RW_ShapeParameter::MYINSTANCE()->SetShapeParamForTable();
		RWrapper::RW_ShapeParameter::MYINSTANCE()->UpdateSelectedShape_PointsTable();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_MeasureParameter::SetAngleMeasureMode(int type)
{
	try
	{
		MAINDllOBJECT->SetAngleMode(type);
		RWrapper::RW_MainInterface::MYINSTANCE()->RefreshDRO();
		RWrapper::RW_MeasureParameter::MYINSTANCE()->SetMeasurementParamForTable();
		RWrapper::RW_ShapeParameter::MYINSTANCE()->SetShapeParamForTable();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

int RWrapper::RW_MeasureParameter::CurrentAngleMeasureMode()
{
	return MAINDllOBJECT->AngleMeasurementMode();
}

void RWrapper::RW_MeasureParameter::ZPositionof3DpointforTVSCircleCen(double ZPos, double DTVS_Cir_cenX, double DTVS_Cir_cenY, double DTVS_Cir_cenZ, double InMinRad, double InMaxRad)		//vinod.. 16/04/2014 we need here the Z position of top surface of tool  
{
	Point3DForZPositionZValue = ZPos;
	DelphiTVS_Cir_Cen[0] = DTVS_Cir_cenX;
	DelphiTVS_Cir_Cen[1] = DTVS_Cir_cenY;
	DelphiTVS_Cir_Cen[2] = DTVS_Cir_cenZ;
	InMINRAD = InMinRad;
	InMAXRAD = InMaxRad;
}

System::String^ RWrapper::RW_MeasureParameter::Convert_AngleFormat(double Ang_val, int measureType, System::String^ MFormat, bool ConvertToDecimalAlso)
{
	try
	{
		char AngledegMin[100];
		double angle;
		System::String^ Converted_Angle = "0.0000";
		if(ConvertToDecimalAlso)
		{
			if(measureType == 0) 
			{
				RMATH2DOBJECT->Radian2Deg_Min_Sec(Ang_val, &AngledegMin[0]);
				Converted_Angle = gcnew System::String(&AngledegMin[0]);
			}
			else if(measureType == 1)
			{
				angle = RMATH2DOBJECT->Radian2Degree(Ang_val);
				Converted_Angle = Microsoft::VisualBasic::Strings::Format(angle, MFormat);
			}
			else if(measureType == 2)
				Converted_Angle = Microsoft::VisualBasic::Strings::Format(Ang_val, MFormat);
		}
		else
		{
			if(measureType == 0) 
			{
				RMATH2DOBJECT->Radian2Deg_Min_Sec(Ang_val, &AngledegMin[0]);
				Converted_Angle = gcnew System::String(&AngledegMin[0]);
			}
			else
				Converted_Angle = Microsoft::VisualBasic::Strings::Format(Ang_val, MFormat);
		}
		return Converted_Angle;
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
		return "0.0000";
	}
}
