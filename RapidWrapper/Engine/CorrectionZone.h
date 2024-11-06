#pragma once
#include "DeviationType.h"

namespace RWrapper
{
	ref class CorrectionZone
	{
		/* --------------------------------------------------------------
		The class to hold the correction parameters for a given axis for a particular zone. 
		zone is a particular cell in the whole measuring travel of the machine. 
		The machine's complete travel is divided into a regular, rectangular grid, and each cell is 
		represented as a zone. This class holds the correction parameters for each zone. 

		Corrections work on a piece-wise linear method. All corrections given are assumed to change linearly with motion
		A given axis can have corrections due to motion in all the other axes of the machine (remember, the movement in one axis 
		may be produced by innacuracy in another axis. This situation is not encoded and hence is unknown. This module is there 
		to esitmate this error, and give compensation....

		
		--------------------------------------------------------------- */
	public:
		//Properties
		System::String^ Index; //a string constructed to label which
		int AxisLabel; //index of the axis in the currentposition array
		int DependencyRank; //The number of axes whose movement will cause a change in this axis. 
		//int X_ZoneNumber; //The number of zones horizontal for the area of movement.
		//int Y_ZoneNumber; //The number of zones vertical for the area of movement. 

		/* The total number of Correction Parameters will be 4 sets of 2 numbers (slope & intercept) per zone per axis */
		/* Since there will be one instance for each axis, if we do only for X and Y, we will get 8 double numbers per 
		zone for correction */
		System::Collections::Generic::List<cli::array<double, 2>^>^ CorrectionParameters; //A two dimensional matrix with 2 columns (one for slope, one for intercept) and DependencyRank number of Rows
		//cli::array<double, 2>^ CorrectionParameters_Y; //A two dimensional matrix with 2 columns (one for slope, one for intercept) and DependencyRank number of Rows
		//cli::array<double>^ Deviations;
		//cli::array<double>^ AxisAngles;

		//Methods
		CorrectionZone(int AxisL, int Dependency)
		{
			this->AxisLabel = AxisL;
			this->DependencyRank = Dependency;
			//this->X_ZoneNumber = Columns;
			//this->Y_ZoneNumber = Rows;
			this->CorrectionParameters = gcnew System::Collections::Generic::List<cli::array<double, 2>^>();
			//We have a set of 2 lines per movement axis - one for X axis, and one for Y axis
			for (int j = 0; j < Dependency; j++)
			{
				this->CorrectionParameters->Add(gcnew cli::array<double, 2>(Dependency, 2));
			}
			//this->Deviations = gcnew cli::array<double>(4); //Allow for 3 axis angles - XY, XZ and YZ; in that order.
			//this->AxisAngles = gcnew cli::array<double>(3); //Allow for 3 axis angles - XY, XZ and YZ; in that order.
		}

		~CorrectionZone()
		{

		}

		//bool CalculateCorrectionParameters(double GridSize, int DependencyAxis, double CurrentDeviation, double PreviousDeviation, double Position, int PrincipalAxis) // cli::array<double>^ Deviations) //System::Collections::Generic::List<DeviationType^>^ Deviations)
		bool CalculateCountCorrectionParameters(cli::array<double>^ GridSize, cli::array<double>^ CurrentDeviation, cli::array<double>^ PreviousDeviation) //, double Position, int PrincipalAxis) // cli::array<double>^ Deviations) //System::Collections::Generic::List<DeviationType^>^ Deviations)
		{
			//if (Deviations->Length != 2 * this->DependencyRank) return false;
			cli::array<System::String^, 1> ^splitter = { ":" };
			cli::array<System::String^> ^Indices = this->Index->Split(splitter, 2, System::StringSplitOptions::None);
			if (Indices->Length != this->DependencyRank) return false;
			try
			{			
				//We calculate the correction parameters for each axis....
				//int j = 0;
				//for (int i = 0; i < DependencyRank; i++)
				//{
					//Calculate slope - Deviations will give the previous cell's deviation as well as current one. Grid size in this reference axis will be the length over which we need the slope
				int k = 0;
				for (int i = 0; i < DependencyRank; i++)
				{
					cli::array<double, 2>^ currentparameters = this->CorrectionParameters[i];
					for (int j = 0; j < 2; j++)
					{
						currentparameters[j, 0] = CurrentDeviation[k] / GridSize[j]; //- PreviousDeviation[k]
						currentparameters[j, 1] = PreviousDeviation[k] - currentparameters[j, 0] * int::Parse(Indices[j]) * GridSize[j];
						k++;
					}
				}
				//////currentparameters[DependencyAxis, 0] = (CurrentDeviation - PreviousDeviation) / GridSize;    //Deviations[DependencyAxis + 1] - Deviations[2 * i]) / GridSizes[i];
					// Intercept has to be calculated according to the position. Position is simply the 1-based index of this axis multiplied by the grid size....
				////////currentparameters[DependencyAxis, 1] = CurrentDeviation - currentparameters[DependencyAxis, 0] * Position; //Deviations[2 * i + 1] - this->CorrectionParameters[i, 0] * (int::Parse(Index->Substring(i, 1)) * GridSizes[i]);
				return true;
			}
			catch (System::Exception^ Ex)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("CZ001", Ex);
				return false;
			}
		}


		cli::array<double>^ GetCorrectionValue(double* CurrentPosition, cli::array<double>^ gSize, cli::array<int>^ GridPos) //cli::array<double>^
		{
			//First Check if currentposition is within the area of this cell 
			cli::array<double>^ Correction = { 0,0,0,0 }; // gcnew cli::array<double>(4);

			//cli::array<double>^ correctedPosition = gcnew cli::array<double>(4); // CurrentPosition->Length);
			//System::Array::Copy(CurrentPosition, correctedPosition, CurrentPosition->Length);
			try
			{
				//Initially making a variable to store the corrections to verify all is going well
				//double Correction = 0; // 
				for (int j = 0; j < this->DependencyRank; j++)
				{
					cli::array<double, 2>^ currentparameters = this->CorrectionParameters[j];
					for (int i = 0; i < DependencyRank; i++)
					{
						Correction[j] += currentparameters[i, 0] * CurrentPosition[i] + currentparameters[i, 1];
						//Sum all the corrections for this axis
						//correctedPosition[AxisLabel] += Corrections[i];
					}
				}

				////Now do axis angle corrections (localised). 
				//double cpos1 = (CurrentPosition[1] - GridPos[1] * gSize[0]);
				//double cpos2 = CurrentPosition[2];
				//Correction[0] += System::Math::Sin(AxisAngles[0]) * cpos1 + sin(AxisAngles[1]) * cpos2;
				//Correction[1] -= (1 - System::Math::Cos(AxisAngles[0])) * cpos1 + sin(AxisAngles[2]) * cpos2;
				//Correction[2] -= (1 - System::Math::Cos(AxisAngles[1]) * System::Math::Cos(AxisAngles[2])) * cpos2;
				//return Correction;

			}
			catch (System::Exception^ Ex)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("CZ002", Ex);
			}
			return Correction;
		}
	};

	ref struct RegionalDeviations
	{
		System::String^ index;
		int pos_x = -1;
		int pos_y = -1;
		double d_xx = 0;
		double d_xy = 0;
		double d_yx = 0;
		double d_yy = 0;

		cli::array<double>^ ToDoubleArray()
		{
			cli::array<double>^ deviationsarray = { d_xx, d_xy, d_yx, d_yy };
			return deviationsarray;
		}
	};
}
