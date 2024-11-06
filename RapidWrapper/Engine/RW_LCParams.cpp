#include "StdAfx.h"
#include "RW_LCParams.h"
#include <math.h>
#include "RW_MainInterface.h"

#define PI 3.1415926535897932384626433832795

RWrapper::RW_LCParams::RW_LCParams(int AxisNo)
{
	//LCParams = this;
	this->AxisNumber = AxisNo;
}

//RWrapper::RW_LCParams^ RWrapper::RW_LCParams::MYINSTANCE()
//{
//	return LCParams;
//}

RWrapper::RW_LCParams::~RW_LCParams()
{

}

void RWrapper::RW_LCParams::SetCorrectionParams(cli::array<System::Data::DataRow ^>^ RawValuesFromTable) //int RawValuesFromTable) //
{
	try
	{
		//Get the total entries we need. then we can instantiate the array properly
		int NumberofRegions = RawValuesFromTable->Length;
		this->CorrectionParameterList = gcnew System::Collections::Generic::List<RWrapper::CorrectionParams^>(); // (NumberofRegions + 1);
		//Make a Zero valued starting entry. 
		RWrapper::CorrectionParams^ CurrentParam = gcnew RWrapper::CorrectionParams(this->AxisNumber);
		CurrentParam = gcnew RWrapper::CorrectionParams(this->AxisNumber);
		//CurrentParam->IsStraightnessCorrection = true;
		CurrentParam->IsRotaryAxis = Convert::ToBoolean(RawValuesFromTable[0]["IsRotaryAxis"]);
		//Store the last Straightness correction param instance. 
		RWrapper::CorrectionParams^ LastStraightnessParam = CurrentParam;
		this->CorrectionParameterList->Add(CurrentParam);

		//this->CorrectionParameterList[0] = currParam;
		//Now iterate through the Datarow array we got from dB and calculate the corrections. 
		for (int i = 0; i < RawValuesFromTable->Length; i++)
		{
			CurrentParam = gcnew RWrapper::CorrectionParams(this->AxisNumber);
			CurrentParam->Position = Convert::ToDouble(RawValuesFromTable[i]["Position"]);
			//CurrentParam->IsStraightnessCorrection = Convert::ToBoolean(RawValuesFromTable[i]["IsStraightnessCorrection"]);
			//if (this->CorrectionParameterList[i + 1])
			CurrentParam->IsRotaryAxis = Convert::ToBoolean(RawValuesFromTable[i]["IsRotaryAxis"]);
			CurrentParam->CorrectionValue_Axial = Convert::ToDouble(RawValuesFromTable[i]["CorrectionValue_Axial"]);
			CurrentParam->CorrectionValue_Orthogonal = Convert::ToDouble(RawValuesFromTable[i]["CorrectionValue_Orthogonal"]);
			//Calculate the parameters now
			//if (!CurrentParam->IsStraightnessCorrection)
				CurrentParam->CalculateCorrectionParams(this->CorrectionParameterList[i]); //We will use the previous value to correct for axial slope only. 
			//else
			//{
			//	//We use the previously stored straightness value. The orthogonal correction starts at the end of the previous region, 
			//	//NOT at a location where we have localised encoder error. This way we can have any number of localised encoder error entries, and our 
			//	//orthogonal corrections will be continuous and proper
			//	CurrentParam->CalculateCorrectionParams(LastStraightnessParam);
			//	LastStraightnessParam = CurrentParam;
			//}
			this->CorrectionParameterList->Add(CurrentParam); //Add it to the collection
		}
		//All correction values and parameters are loaded and set. Now we are ready to do corrections. 

	}
	catch (System::Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRLCO0001", ex);
	}
}

cli::array<double>^ RWrapper::RW_LCParams::GetCorrectedPosition(cli::array<double>^ CurrentPosition) //int CurrentPosition) //
{
	
	cli::array<double>^ correctedPosition = gcnew cli::array<double>(4); // CurrentPosition->Length);
	Array::Copy(CurrentPosition, correctedPosition, CurrentPosition->Length);
	try
	{
		if (!this->IsRotaryAxis)
		{
			bool DocorrectionNow = false;	
			int OrthoAxisNumber = 1;
			if (AxisNumber = 1) OrthoAxisNumber = 0;

			for (int i = 0; i < this->CorrectionParameterList->Count; i++)
			{
				if (CurrentPosition[OrthoAxisNumber] >= 0 && 
					CurrentPosition[OrthoAxisNumber] < this->CorrectionParameterList[i]->Location_Start) continue;

				if (CurrentPosition[AxisNumber] < this->CorrectionParameterList[i]->Position || (i == this->CorrectionParameterList->Count - 1 && CurrentPosition[AxisNumber] > this->CorrectionParameterList[i]->Position))
				{
					if (i == 0)
						DocorrectionNow = true;
					//Check to see in whichregion its in. We cannot only do one less than. A position in first region will be there in ALL regions then
					else if (i > 0 && CurrentPosition[AxisNumber] > this->CorrectionParameterList[i - 1]->Position)
						DocorrectionNow = true;
					else
						DocorrectionNow = false;
					if (DocorrectionNow)
					{
						//Do Axial Correction first. This is done even if its only encoder error. Orthogonal compensation comes only when we do straightness correction
						correctedPosition[AxisNumber] -= this->CorrectionParameterList[i]->CorrectionLineAxial_0 * CurrentPosition[AxisNumber] + this->CorrectionParameterList[i]->CorrectionLineAxial_1;
						//if (this->CorrectionParameterList[i]->IsStraightnessCorrection)
						//{
							//Orthogonal Correction will return the amount of error to compensate. We simply add to the appropriate axis based on which axis we are working on!
							double OrthogonalCompensation = this->CorrectionParameterList[i]->CorrectionLineOrthogonal_0 * CurrentPosition[AxisNumber] + this->CorrectionParameterList[i]->CorrectionLineOrthogonal_1;
							if (AxisNumber == 0)
								correctedPosition[1] -= OrthogonalCompensation;
							else
								correctedPosition[0] -= OrthogonalCompensation;
							//DoneStraightnessCorrection = true;
						//}
					}
				}
			}
		}
	}
	catch (System::Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRLCO0002", ex);
	}
	return correctedPosition;
}
