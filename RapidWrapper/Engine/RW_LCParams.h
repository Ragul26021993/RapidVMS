#pragma once
//#include <math.h>
#include "RW_CorrectionParams.h"

namespace RWrapper
{

	ref class RW_LCParams
	{
	private:
		//cli::array<double, 2> ^CorrectionParameters;
		//cli::array<double> ^Positions;

	public:
		RW_LCParams(int AxisNo);
		~RW_LCParams();

		int AxisNumber;
		bool IsRotaryAxis;
		System::Collections::Generic::List<CorrectionParams ^>^ CorrectionParameterList;

		void SetCorrectionParams(cli::array<System::Data::DataRow ^>^ RawValuesFromTable); // int RawValuesFromTable);
		cli::array<double>^ GetCorrectedPosition(cli::array<double>^ CurrentPosition); //
	};
};
