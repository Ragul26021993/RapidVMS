#pragma once

namespace RWrapper
{
	ref class CorrectionParams
	{
	public:
		int AxisNo;
		double Position;
		double CorrectionValue_Axial;
		double CorrectionValue_Orthogonal;
		double Location_Start;
		double Location_End;
		//double OrthogonalCorrection;
		//bool IsStraightnessCorrection;
		bool IsRotaryAxis;
		double CorrectionLineAxial_0, CorrectionLineAxial_1;
		double CorrectionLineOrthogonal_0, CorrectionLineOrthogonal_1;

		CorrectionParams(int AxisLabel)
		{
			this->AxisNo = AxisLabel;
			//this->CorrectionLineAxial = gcnew cli::array<double>(2);
			//this->CorrectionLineOrthogonal = gcnew cli::array<double>(2);
		}

		~CorrectionParams()
		{
		}

		void CalculateCorrectionParams(CorrectionParams^ PreviousRegion)
		{
			//First Correct for extra/less movement along this axis itself...
			//Calculate slope with respect to previous value. 
			this->CorrectionLineAxial_0 = (this->CorrectionValue_Axial - PreviousRegion->CorrectionValue_Axial) / (this->Position - PreviousRegion->Position);
			//Calculate intercept - (y1 - slope * x1)
			this->CorrectionLineAxial_1 = this->CorrectionValue_Axial - this->CorrectionLineAxial_0 * this->Position;
			//Now if this is not an encoder error, have a correction in the orthogonal axis as well. 
			//if (this->IsStraightnessCorrection)
			//{
				//We have to add/subtract for travel in the orthogonal axis...
				//double dist = (this->Position - PreviousRegion->Position);
				//double angle = System::Math::Acos(dist / (dist + this->CorrectionValue));
				////Calculate what displacement in orthogonal axis caused the extra movement in this axis!
				//this->OrthogonalCorrection = dist * System::Math::Tan(angle);
				//Calculate slope with respect to previous value. 
				this->CorrectionLineOrthogonal_0 = (this->CorrectionValue_Orthogonal - PreviousRegion->CorrectionValue_Orthogonal) / (this->Position - PreviousRegion->Position);
				//Calculate intercept - (y1 - slope * x1)
				this->CorrectionLineOrthogonal_1 = this->CorrectionValue_Orthogonal - this->CorrectionLineOrthogonal_0 * this->Position;
			//}
		}
	};
}
