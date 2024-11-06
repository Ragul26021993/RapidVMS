
#pragma once
#include <cmath>

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

class HelperMath
{
public:
	HelperMath(void);
	~HelperMath(void);

	enum Func
	{	Focus,
		Circle_LMA,
		Cylinder,
		Cone,
		ThreeDCircle,
		TwoDConic,
		TwoDParabola,
		Conicoid,
		Paraboloid,
		Torus,
		TwoArc,
		TwoArc_AxisGiven,
		TwoLine,
		TwoLine_AxisGiven,
		LineArcLine,
		LineArcLine_AxisGiven,
		ArcArc,
		ArcArc_ArcGiven,
		LineArc,
		LineArc_LineGiven,
		ArcArcArc,
		Sphere,
		CircleInvolute,
		CamProfile,
		Sinusoid,
	};
	
	bool ShiftaroundCentroid_3D(double* Pts, int PtsCount, double* Centroid);
	bool ShiftaroundCentroid_2D(double* Pts, int PtsCount, double* Centroid);
	double Point_Line_Dist_2D(double* p, double* L);
	double Point_Line_Dist_3D(double* p, double* L);

	int Line_lineintersection_2D( double slope1,double intercept1, double slope2, double intercept2, double *intersectnpt );
	bool Lslope( double *slope );

	void TransposeMatrix(double* matrix, double* Transpose, int Rows, int Columns);
	double OperateMatrix(double* matrix, int Size, int Flag, double* Answer);
	bool MultiplyMatrix(double *M1, int *S1, double *M2, int *S2, double *answer);
	void MultiplyMatrix(double Multiplier, double *Matrix, int *S1);

	bool Get_Jac(Func FunctionName, double* Values, double* Param, int TotalPoints, int NofParams, double* jacobian, double* residual);
	bool LS_BF (	double *Values, double* StartingGuess, 
					int TotalPts, int ConstraintParamIndex, 
					int NoOfParams, int DoNotUpdateIndex, 
					Func Function, double toleranceVal, double* Answer);
	
	bool LS_BF_LM (	double *Values, double* StartingGuess, 
					int TotalPts, int ConstraintParamIndex, 
					int NoOfParams, int DoNotUpdateIndex, 
					Func Function, bool ConstrainAllParams,
					int MaxIterationCt, double toleranceVal, double* Answer);

	double pythag(double a, double b, double c = 0);
	void SVD(double* A, int m, int n, double* diag, double *V);
	
	void HelperMath::Swap(double a, double b);
	void HelperMath::BalanceMatrix(double* M, int Size);
	void HelperMath::Hessenberg_Transform(double* M, int n);
	void HelperMath::H_QR_EigenValues(double* M, int n, double* eigenValues);
	void Calculate_CG(BYTE* image, int width, int height, int *x_CG, int *y_CG, double* Orientation);
	//Calculate the Eigen Values and Eigen Vectors of a given set of points in 2D
	bool Get_Eigen_2D(double* points, int ptsCt, double* eigenValues, double* eigenSlopes);

private:

	double *param, *jacobian, *residual, *jac_temp, *jac_inverse, *jac_residual, *deltaParam;
	int CurrentParameterNumber, CurrentPointsCt;
	void Allocate_LSBF_Memory();//(bool Resize);

};

