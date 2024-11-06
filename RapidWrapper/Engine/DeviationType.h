#pragma once
namespace RWrapper
{
	ref class DeviationType
	{
	public:
		DeviationType(int axName, int NameofMovingAxis, int MatrixSize);
		~DeviationType();
		int AxisName; //Deviations for this axis
		int MovementAxis; //Deviations happen when moving in this axis
		cli::array<double>^DeviationValues; //Actual Deviation Vaues. 
	private:

	};

	DeviationType::DeviationType(int axName, int NameofMovingAxis, int MatrixSize)
	{
		this->AxisName = axName;
		this->MovementAxis = NameofMovingAxis;
		DeviationValues = gcnew cli::array<double>(MatrixSize);
	}

	DeviationType::~DeviationType()
	{
	}

};
