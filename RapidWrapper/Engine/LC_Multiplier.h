#pragma once

namespace RWrapper
{
	ref struct LC_Deviations
	{
		System::Double Position;
		System::Double Deviation;
		System::Double Slope;
	};

	ref class LC_Multiplier
	{
		/*
				Added 14 Jul 2016.
		Localised multiplier implementation. We observe not just count loss/gain, but localised change in linearity between counts and actual distance moved.
		For this, we make a Locations collection with start and end coordinate of this rectangle (not just a fixed grid of pre-defined size).
		Then, we have the local multiplier, based on the distance moved from the start position in the concerned axis. Example: if the entry in the dB is +0.004mm,
		we have a multiplier of 0.004 / (End - start). So if we have moved 3 mm into the region from the start position, we then add 3 * multiplier to get correction value.
		We add this to the net correction and return the correction...

		*/
	private:
		//System::Collections::Generic::List<cli::array<double>^>^ Locations; //A two dimensional matrix with 2 columns (one for Start, one for End) and DependencyRank number of Rows for marking the rectangular location of this correction zone
		//System::Collections::Generic::List<cli::array<double>^>^ LocalMultiplier;

	public:
		int NoOfAxes;
		System::Collections::ArrayList ^Deviations;
		
		LC_Multiplier::LC_Multiplier(int AxisNumber)
		{
			this->NoOfAxes = AxisNumber;
			this->Deviations = gcnew System::Collections::ArrayList;
			for (int i = 0; i < AxisNumber; i++)
			{
				System::Collections::Generic::List<RWrapper::LC_Deviations^> ^Dev = gcnew System::Collections::Generic::List<RWrapper::LC_Deviations^>();
				this->Deviations->Add(Dev);
			}
			
			//this->Locations = gcnew System::Collections::Generic::List<cli::array<double>^>(); // cli::array<double, 3>(NoOfAxes, 3); // hard coded for 2 axes here. start and end for each axis * two axes. 

			//for (int i = 0; i < AxisNumber; i++)
			//{
			//	cli::array<double>^ position_array = gcnew cli::array<double>(2); //extents of this correction object for each axis
			//	this->Locations->Add(position_array);
			//}

			//this->LocalMultiplier = gcnew System::Collections::Generic::List<cli::array<double>^>();  
			//for (int j = 0; j < AxisNumber; j++)
			//{
			//	cli::array<double>^ multiplierArray = gcnew cli::array<double>(2); //one for each axis
			//	this->LocalMultiplier->Add(multiplierArray);
			//}
		}

		LC_Multiplier::~LC_Multiplier()
		{
		}

		void CalculateMultiplierCorrectionParameters() //,  , cli::array<double>^ Deviations) //cli::array<double>^ Position
		{
			//Assign the start and end positions for each axis here.
			for (int j = 0; j < this->NoOfAxes; j++)
			{
				System::Collections::Generic::List<RWrapper::LC_Deviations^> ^Dev = (System::Collections::Generic::List<RWrapper::LC_Deviations^>^)this->Deviations[j];
				//The ith entry in the List contains the slope for correcting in the region of i - 1 to i. So the first entry will have a slope of 0.
				for (int i = 1; i < Dev->Count; i++)
				{
					Dev[i]->Slope = (Dev[i]->Deviation - Dev[i - 1]->Deviation) / (Dev[i]->Position - Dev[i - 1]->Position); 
				}
				//cli::array<double>^ position_array = this->Locations[j]; 
				//position_array[0] = Dev[0]->Position;
				//position_array[1] = Dev[Dev->Count - 1]->Position;
			}
		}

		int GetCorrectionValue(double* CurrentPosition, cli::array<double>^ Correction)
		{
			int IsinWindow = 0; // false;
			try
			{
				//New method with both up ramp and down ramp
				for (int i = 0; i < this->NoOfAxes; i++)
				{
					System::Collections::Generic::List<RWrapper::LC_Deviations^> ^Dev = (System::Collections::Generic::List<RWrapper::LC_Deviations^>^)this->Deviations[i];
					if (CurrentPosition[i] >= Dev[0]->Position && CurrentPosition[i] < Dev[Dev->Count - 1]->Position)
						IsinWindow++; // = true;
				}

				if (IsinWindow != this->NoOfAxes) return -1;
				// We now do local multiplier corrections. 
				for (int j = 0; j < this->NoOfAxes; j++)
				{
					//cli::array<double>^ posArray = this->Locations[i];
					//cli::array<double>^ posMultipler = this->LocalMultiplier[i];
					System::Collections::Generic::List<RWrapper::LC_Deviations^> ^Dev = (System::Collections::Generic::List<RWrapper::LC_Deviations^>^)this->Deviations[j];
					for (int i = 1; i < Dev->Count; i++)
					{
						if (CurrentPosition[j] > Dev[i]->Position) // The current position is in a region to the right. Let us calculate 
							Correction[j] += (Dev[i]->Position - Dev[i - 1]->Position) * Dev[i]->Slope;
						else
						{
							Correction[j] += (CurrentPosition[j] - Dev[i - 1]->Position) * Dev[i]->Slope; 
							break;
						}
					}
				}
			}
			catch (System::Exception^ ex)
			{
				return 0;
			}
			return 1;
		}
	};
}
