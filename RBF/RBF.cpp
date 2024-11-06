// RBF.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <malloc.h>
#include "RBF.h"
#include "HelperMath.h"

#define _USE_MATH_DEFINES
#include <math.h>

HelperMath* hM;
//double XY_AxisAngle = 0;
//double Tolerance;

//global variables needed by calibration functions
double* error_array = NULL;
double calibration_width = 0;
int X_num = 0;
int Y_num =0;
int tol_err = 5;

//global variables needed by auto-orientation detection functions
BYTE* RefImage = NULL;
BYTE* ObjImage = NULL;
double A_ref;
double A_obj;
double A_ref_obj;
BYTE* TmpImage = NULL;
int x_cg_ref =0;
int y_cg_ref =0;
int x_cg_obj =0;
int y_cg_obj =0;
int x_rot_abt = 0;
int y_rot_abt = 0;
int M_ARRAY = 0;
int N_ARRAY = 0;
//For calculating the eigen values for OneShot Orientation calculation
//double X_sq_Sum_ref = 0, Y_sq_Sum_ref = 0, X_Y_Sum_ref = 0;
//double X_sq_Sum_obj = 0, Y_sq_Sum_obj = 0, X_Y_Sum_obj = 0;
double orientation_ref = 0, orientation_obj = 0;

//Global variable needed for angle tolerance for line close to 90 degrees
double angTolerance = 0.0006;

RBF::RBF()
{	Tolerance = 0.0000001;
	hM = new(HelperMath);
	dist_cutoff = 10000;
	RBFError = NULL;
}

RBF::~RBF()
{	
	delete hM;
}

void RBF::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(RBFError != NULL)
		RBFError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}
//================  Calculate shape from minimal pts ===============
// --------------   The 3 pt circle calculator
bool RBF::Circle_3Pt(double* P, double* Ans)
{	
	try
	{
		double m, n, o, p, q, r, s;
		m = *(P + 0) * *(P + 0)  + *(P + 1) * *(P + 1);
		n = *(P + 2) * *(P + 2)  + *(P + 3) * *(P + 3);
		o = *(P + 4) * *(P + 4)  + *(P + 5) * *(P + 5);
		p = *(P + 4) - *(P + 2); q = *(P + 2) - *(P + 0);
		r = *(P + 5) - *(P + 3); s = *(P + 3) - *(P + 1);

		//the points are collinear
		if ( p * s - q * r == 0) return false;

		// Calculating the X and Y co-ods of the centre of circle
		*(Ans + 0) = -((n - o) * s - (m - n) * r) / (2 * ((p * s) - (q * r)));
		*(Ans + 1) = -((n - o) * q - (m - n) * p) / (2 * ((q * r) - (p * s)));
		//calculate radius
		*(Ans + 2) = sqrt( pow((*(P + 0) - *(Ans + 0)), 2) + pow((*(P + 1) - *(Ans + 1)), 2));
		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0001", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::Plane_3Pt_2(double* P, double* Ans)
{
	try
	{
		double v1x = *(P + 3) - *P;
		double v1y = *(P + 4) - *(P + 1);
		double v1z = *(P + 5) - *(P + 2);

		double v2x = *(P + 6) - *P;
		double v2y = *(P + 7) - *(P + 1);
		double v2z = *(P + 8) - *(P + 2);

		//get cross product of v1 and v2
		double w1x = v1y * v2z - v1z * v2y;
		double w1y = v1z * v2x - v1x * v2z;
		double w1z = v1x * v2y - v1y * v2x;

		if ((w1x ==0) && (w1y ==0) && (w1z ==0))
			return false;

		double norm = sqrt ( pow(w1x,2) + pow(w1y,2) + pow(w1z,2));

		w1x = w1x/norm;
		w1y = w1y/norm;
		w1z = w1z/norm;

		double d = - (w1x * *P + w1y * *(P + 1) + w1z * *(P + 2));

		*Ans = w1x;
		*(Ans + 1) = w1y;
		*(Ans + 2) = w1z;
		*(Ans + 3) = d;

		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0002", __FILE__, __FUNCSIG__);  return false;}
}

// --------------   3 Pt Plane Calculator
bool RBF::Plane_3Pt(double* P, double* Ans)
{	
	try
	{
		double Answer[4];
		double* tempMatrix = (double*) malloc(sizeof(double) * 9);
		int i;
		//double *ANsss;
		//A x + B y + C z  + D = 0
		//Coefficient a 
	//A
		for (i = 0; i < 3; i ++)
		{	*(tempMatrix + i * 3 + 0) = *(P + i * 3 + 1);
			*(tempMatrix + i * 3 + 1) = *(P + i * 3 + 2);
			*(tempMatrix + i * 3 + 2) = 1; 
		}
		Answer[0] = hM->OperateMatrix(tempMatrix, 3, 0, NULL);
		//B
		for (i = 0; i < 3; i ++)
		{	*(tempMatrix + i * 3 + 0) = *(P + i * 3);
			*(tempMatrix + i * 3 + 1) = *(P + i * 3 + 2);
			*(tempMatrix + i * 3 + 2) = 1; 
		}
		Answer[1] = hM->OperateMatrix(tempMatrix, 3, 0, NULL);	
		//C
		for (i = 0; i < 3; i ++)
		{	*(tempMatrix + i * 3 + 0) = *(P + i * 3);
			*(tempMatrix + i * 3 + 1) = *(P + i * 3 + 1);
			*(tempMatrix + i * 3 + 2) = 1; 
		}
		Answer[2] =hM->OperateMatrix(tempMatrix, 3, 0, NULL);
		//D
		for (i = 0; i < 3; i ++)
		{	*(tempMatrix + i * 3 + 0) = *(P + i * 3);
			*(tempMatrix + i * 3 + 1) = *(P + i * 3 + 1);
			*(tempMatrix + i * 3 + 2) = *(P + i * 3 + 2); 
		}
		Answer[3] = hM->OperateMatrix(tempMatrix, 3, 0, NULL);
		double sqq = 0;
		//normalise and then set answer
		for (i = 0; i < 3; i ++)
		{	sqq +=Answer[i] * Answer[i];
		}
		sqq = sqrt(sqq);

		for (i = 0; i < 4; i ++)
		{	*(Ans + i) = Answer[i] / sqq;
		}
		free(tempMatrix);
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0003", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::Sphere_4Pt(double* P, double* Ans)
{	
	try
	{
		double* P_Matrix = (double*) malloc(sizeof(double)* 16);
		double Residual[4]; 
		for (int j = 0; j < 4; j ++)//row
		{	Residual[j] = 0;
			for (int i = 0; i < 3; i ++)//column
			{	*(P_Matrix + j * 4 + i) = *(P + j * 3 + i);
				Residual[j] += *(P + j * 3 + i) * *(P + j * 3 + i);
			}
			*(P_Matrix + j * 4 + 3) = 1;
			Residual[j] *= -1;
		}
		double Inverse[16];
		ZeroMemory(&Inverse[0], 4 * sizeof(double));

		double Planarity = hM->OperateMatrix(P_Matrix, 4, 1, &Inverse[0]);
		if (Planarity == 0) return false;
		double tA[4];
		int Size1[2] = {4,4}; 
		int Size2[2] = {4,1}; 
		double tSqr = 0;
		ZeroMemory(&tA[0], 4 * sizeof(double));

		hM->MultiplyMatrix(&Inverse[0], &Size1[0], &Residual[0], &Size2[0], &tA[0]);
		for (int i = 0; i < 3; i ++)
		{	*(Ans + i) = -0.5 * tA[i];
			tSqr += *(Ans + i) * *(Ans + i);
		}
		*(Ans + 3) = sqrt(tSqr - tA[3]);
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0004", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::Circle_3Pt_3D(double* P, double* Ans)
{
	try
	{
		double v1[3] = {*(P+3) - *(P+0), *(P+4) - *(P+1), *(P+5) - *(P+2)};
		double v2[3] = {*(P+6) - *(P+3), *(P+7) - *(P+4), *(P+8) - *(P+5)};

		double w[3] = {v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]};

		double norm = sqrt(pow(w[0],2) + pow(w[1],2) + pow(w[2],2));
		if (norm==0) return false;
		for (int i = 0; i < 3; i++)
			w[i] /= norm;

		double x1[3] = {(*(P+3) + *(P+0))/2, (*(P+4) + *(P+1))/2, (*(P+5) + *(P+2))/2};
		double x2[3] = {(*(P+6) + *(P+3))/2, (*(P+7) + *(P+4))/2, (*(P+8) + *(P+5))/2};

		norm = sqrt(pow(v1[0],2) + pow(v1[1],2) + pow(v1[2],2));
		for (int i = 0; i < 3; i++)
			v1[i] /= norm;

		norm = sqrt(pow(v2[0],2) + pow(v2[1],2) + pow(v2[2],2));
		for (int i = 0; i < 3; i++)
			v2[i] /= norm;

		double l1[3] = {v1[1]*w[2] - v1[2]*w[1], v1[2]*w[0] - v1[0]*w[2], v1[0]*w[1] - v1[1]*w[0]};
		double l2[3] = {v2[1]*w[2] - v2[2]*w[1], v2[2]*w[0] - v2[0]*w[2], v2[0]*w[1] - v2[1]*w[0]};
		double line1[6] = {x1[0], x1[1], x1[2], l1[0], l1[1], l1[2]};
		double line2[6] = {x2[0], x2[1], x2[2], l2[0], l2[1], l2[2]};

		double center[3] = {0};
		if (!Intersection_Line_Line(line1, line2, center))
			return false;

		double radius = hM->pythag(P[0] - center[0], P[1] - center[1], P[2] - center[2]);

		*(Ans +0) = center[0]; *(Ans +1) = center[1]; *(Ans +2) = center[2]; *(Ans +3) = w[0]; *(Ans +4) = w[1]; *(Ans + 5) = w[2]; *(Ans + 6) = radius;
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0005", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::Intersection_Line_Line(double* line1, double* line2, double* center)
{
	try
	{
		//solve for t1*l1[3] - t2*l2[3] = l2[0] - l1[0] and t1*l1[4] - t2*l2[4] = l2[1] - l1[1] and substitute in
		//t1*l1[5] - t2*l2[5] and check if it is falling within a tolerance band of l2[2] - l1[2].  If yes, the intersection point is
		//l1[0] + t1*l1[3], l1[1] + t1*l1[4], l1[2] + t1*l1[5].  if no, return false
		
		double t1 = 0, t2 = 0, chk = 0;
		double det = - line1[3] * line2[4] + line1[4] * line2[3];
		if (abs(det) < Tolerance)
		{
			if (abs(- (line2[0] - line1[0]) * line2[4] + (line2[1] - line1[1]) * line2[3]) > Tolerance)
				return false;
			else
				det = - line1[4] * line2[5] + line1[5] * line2[4];
			if (abs(det) < Tolerance)
			{
				if (abs(- (line2[1] - line1[1]) * line2[5] + (line2[2] - line1[2]) * line2[4]) > Tolerance)
					return false;
				else
					det = - line1[5] * line2[3] + line1[3] * line2[5];
				if (abs(det) < Tolerance)
					return false;
				t1 = (- (line2[2] - line1[2]) * line2[3] + (line2[0] - line1[0]) * line2[5]) / det;
				t2 = (line1[5] * (line2[0] - line1[0]) - line1[3] * (line2[2] - line1[2])) / det;
				chk = t1 * line1[4] - t2 * line2[4] - line2[1] + line1[1];
				if (abs(chk) > Tolerance)
					return false;
				center[0] = line1[0] + t1 * line1[3];
				center[1] = line1[1] + t1 * line1[4];
				center[2] = line1[2] + t1 * line1[5];
				return true;
			}
			t1 = (- (line2[1] - line1[1]) * line2[5] + (line2[2] - line1[2]) * line2[4]) / det;
			t2 = (line1[4] * (line2[2] - line1[2]) - line1[5] * (line2[1] - line1[1])) / det;
			chk = t1 * line1[3] - t2 * line2[3] - line2[0] + line1[0];
			if (abs(chk) > Tolerance)
				return false;
			center[0] = line1[0] + t1 * line1[3];
			center[1] = line1[1] + t1 * line1[4];
			center[2] = line1[2] + t1 * line1[5];
			return true;
		}
		t1 = (- (line2[0] - line1[0]) * line2[4] + (line2[1] - line1[1]) * line2[3]) / det;
		t2 = (line1[3] * (line2[1] - line1[1]) - line1[4] * (line2[0] - line1[0])) / det;

		chk = t1 * line1[5] - t2 * line2[5] - line2[2] + line1[2];
		if (abs(chk) > Tolerance)
			return false;
		center[0] = line1[0] + t1 * line1[3];
		center[1] = line1[1] + t1 * line1[4];
		center[2] = line1[2] + t1 * line1[5];
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0006", __FILE__, __FUNCSIG__); return false;
	}
}

//================  End Calculate Base Shape Functions ================
//=====================================================================


//=======================================================
// ============ " Best Fit functions " ====================

// --------------- " Best Fit Line Function " -----------------------
//     ---------- This function calculates the classical two pt lines + choose the one with the least sum of distances


bool RBF::Line_BestFit_2D(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision, bool remove_outliers, int filterdir, int iter)
{
	try
	{
		int Stepper = 1;
		if (PtsCount > 50) Stepper = (int) PtsCount / 40;
		int LineCt = 1600; // PtsCount * (PtsCount - 1) / 2; 
		double* slopes; double* intercepts;
		slopes = (double*) malloc(sizeof(double) * LineCt);
		intercepts = (double*) malloc(sizeof(double) * LineCt);
		int i = 0, j = 0, Ct = 0;
		for (i = 0; i < 2 * (int)(PtsCount * 0.5); i += 2 * Stepper)
		{	for (j = 2 * (int)(PtsCount * 0.5); j < PtsCount * 2; j += 2 * Stepper)
			{	if (*(pts + i) == *(pts + j))
				{ *(slopes + Ct) = 0.5 * M_PI ; *(intercepts + Ct) = *(pts + i);
				  Ct += 1;
				}
				else
				{ *(slopes + Ct) = atan((*(pts + j + 1) - *(pts + i + 1))/(*(pts + j) - *(pts + i)));
				
					if (abs(abs (*(slopes + Ct)) - 0.5 * M_PI) > angTolerance)
						*(intercepts + Ct)=  *(pts + i + 1) - tan(*(slopes + Ct)) * *(pts + i);
					else
					{
						*(slopes + Ct) = 0.5 * M_PI;
						*(intercepts + Ct)= *(pts + i);
					}
				 Ct += 1;
				}
			}
		}
		//DWORD nn = timeGetTime();

		double min_err = 10000000000;
		double final[2];
		double tL[2]; 
		double ls_err = 0;
		double* tP = (double*) malloc(sizeof(double) * 2);

		for (j = 0; j < Ct; j ++)
		{	ls_err = 0; tL[0] = *(slopes + j); tL[1] = *(intercepts + j);
			for (i = 0; i < PtsCount * 2; i += 2)
			{	for (int k = 0; k < 2; k ++) {tP[k] = *(pts + i + k);}
				double tt = hM->Point_Line_Dist_2D(tP, tL);
				ls_err += tt;	
			}
			if (ls_err < min_err) {min_err = ls_err; final[0] = *(slopes + j); final[1] = *(intercepts + j);}
		}
		*(answer) = final[0]; *(answer + 1) = final[1];
		//	TCHAR ss[100];
		//long tt = (long)timeGetTime() - (long)nn;
		//wsprintf(ss, _T("Ans %d"), tt);
		//MessageBox(hwndVideo, ss, TEXT("RapidCam"), MB_ICONEXCLAMATION);
		if (SigmaMode)
		{	double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
			//Copy the 2D pts into a 3D-esque array for doing proper SVD! take Z to be 0...
			for (i = 0; i < PtsCount; i ++)
			{	*(localPts + i * 3) = *(pts + i * 2);
				*(localPts + i * 3 + 1) = *(pts + i * 2 + 1);
				*(localPts + i * 3 + 2) = 0;
			}
			double Centroid[3], Diag[3], ArrangedDiag[3], valueDecomp[9];
			for (i = 0; i < 3; i ++)
			{	Centroid[i] = 0; Diag[i] = 0; ArrangedDiag[i] = 0;
			}
			for (i = 0; i < 9; i ++) valueDecomp[i] = 0;

			if (hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0]))
			{	hM->SVD(localPts, 3, 3, &Diag[0], &valueDecomp[0]);
				memcpy((void*) &ArrangedDiag[0], (void*) &Diag[0], 3 * sizeof(double));
				double tempD = 0;
				//Sort the diagonal matrix in increasing order into ArrangedDiag
				for (i = 0; i < 3; i ++)
				{	for (j = i + 1; j < 3; j ++)
					{	if (ArrangedDiag[j] < ArrangedDiag[i]) 
						{	tempD = ArrangedDiag[i]; 
							ArrangedDiag[i] = ArrangedDiag[j];
							ArrangedDiag[j] = tempD;
						}
					}
				}
				//Now let us see the different situations
				if (ArrangedDiag[0] == 0)
				{	if (ArrangedDiag[1] == 0)	
					{	//We have a horizontal line
						*(answer) = 0; *(answer + 1) = Centroid[1]; 
					}
					else
					{	
						if (ArrangedDiag[2] == 1.0)
						{	//We have a vertical Line
							*(answer) = M_PI / 2; *(answer + 1) = Centroid[0]; 
						}
						else
						{
							//We have a normal line. No specials. So let us calculate the slopes and intercepts
							int eV;
							for (i = 0; i < 3; i ++)
							{	if (Diag[i] == ArrangedDiag[2]) eV = i;
							}
							*(answer) = atan(valueDecomp[eV + 3] / valueDecomp[eV]);
							if (abs(abs (*(answer)) - 0.5 * M_PI) > angTolerance)
								*(answer + 1) = Centroid[1] - valueDecomp[eV + 3] * Centroid[0] / valueDecomp[eV];
							else
							{
								*(answer) = 0.5 * M_PI;
								*(answer + 1)= Centroid[0];
							}
							
							//*(answer + 2) = atan(valueDecomp[eV + 6] / valueDecomp[eV]);
							//*(answer + 3) = Centroid[2] - valueDecomp[eV + 6] * Centroid[0] / valueDecomp[eV];
						}
					
					}
					ls_err = 0; tL[0] = *(answer); tL[1] = *(answer + 1);
					for (i = 0; i < PtsCount * 2; i += 2)
					{	
						for (int k = 0; k < 2; k ++) 
						{tP[k] = *(pts + i + k);}
						double tt = hM->Point_Line_Dist_2D(tP, tL);
						ls_err += tt;	
					}
					if (min_err < ls_err)
					{
						*(answer) = final[0];
						*(answer + 1) = final[1];
					}
					else
						min_err = ls_err;
				}	
			}
			free(localPts);
		}

		free(slopes); free(intercepts); free(tP);
	
		if ((precision < 1000000) && (!remove_outliers))
		{
			*(answer + 2) = min_err;
			if (min_err > precision)
				return false;
		}
		if (PtsCount == 2) return true;
		if (remove_outliers)
		{
			double err = 2 * min_err/PtsCount;
			double* ptscpy = (double*) malloc(2*PtsCount*sizeof(double));
			memset(ptscpy,0,2*PtsCount*sizeof(double));
			int jj = 0;
			for (i = 0; i < PtsCount * 2; i += 2)
			{
				if (hM->Point_Line_Dist_2D((pts + i), answer) < err)
				{
					*(ptscpy + jj) = *(pts + i); *(ptscpy + jj + 1) = *(pts + i + 1);
					jj += 2;
				}
			}
			int ptscnt = jj/2;
			if (!Line_BestFit_2D(ptscpy, ptscnt, answer, SigmaMode))
			{
				free(ptscpy);
				return false;
			}
			free(ptscpy);
		}
		if ((filterdir != -1) && (iter > 0))
		{
			double* pts_cpy = (double*) malloc(sizeof(double) * PtsCount * 2);
			int k = 0;
			switch (filterdir)
			{
				case 0:
					if (abs(*(answer + 0)) < angTolerance)
					{
						free (pts_cpy);
						return true;
					}
					for (i = 0; i < PtsCount; i ++) 
					{
						if (((abs(abs(*(answer + 0)) - 0.5 * M_PI) < angTolerance) && (*(pts + 2*i) > *(answer + 1))) ||
							((abs(abs(*(answer + 0)) - 0.5 * M_PI) > angTolerance) && (*(pts + 2*i) < ((*(pts + 2*i + 1) - *(answer + 1)) / tan (*(answer + 0))))))
						{
							*(pts_cpy + 2*k) = *(pts + 2*i);
							*(pts_cpy + 2*k + 1) = *(pts + 2*i + 1);
							k++;
						}
					}
					break;
				case 1:
					if (abs(*(answer + 0)) < angTolerance)
					{
						free (pts_cpy);
						return true;
					}
					for (i = 0; i < PtsCount; i ++) 
					{
						if (((abs(abs(*(answer + 0)) - 0.5 * M_PI) < angTolerance) && (*(pts + 2*i) < *(answer + 1))) ||
							((abs(abs(*(answer + 0)) - 0.5 * M_PI) > angTolerance) && (*(pts + 2*i) < ((*(pts + 2*i + 1) - *(answer + 1)) / tan (*(answer + 0))))))
						{
							*(pts_cpy + 2*k) = *(pts + 2*i);
							*(pts_cpy + 2*k + 1) = *(pts + 2*i + 1);
							k++;
						}
					}
					break;
				case 2:
					if (abs(abs(*(answer + 0)) - 0.5 * M_PI) < angTolerance)
					{
						free (pts_cpy);
						return true;
					}
					for (i = 0; i < PtsCount; i ++) 
					{
						if (*(pts + 2*i + 1) > tan(*(answer + 0)) * (*(pts + 2*i)) + *(answer + 1))
						{
							*(pts_cpy + 2*k) = *(pts + 2*i);
							*(pts_cpy + 2*k + 1) = *(pts + 2*i + 1);
							k++;
						}
					}
					break;
				case 3:
					if (abs(abs(*(answer + 0)) - 0.5 * M_PI) < angTolerance)
					{
						free (pts_cpy);
						return true;
					}
					for (i = 0; i < PtsCount; i ++) 
					{
						if (*(pts + 2*i + 1) < tan(*(answer + 0)) * (*(pts + 2*i)) + *(answer + 1))
						{
							*(pts_cpy + 2*k) = *(pts + 2*i);
							*(pts_cpy + 2*k + 1) = *(pts + 2*i + 1);
							k++;
						}
					}
					break;
				default:
					free (pts_cpy);
					return true;
					break;
			}
			iter--;
			if (k < 10)
			{
				free (pts_cpy);
				return true;
			}
			double answer_cpy[2] = {0};
			if (!Line_BestFit_2D(pts_cpy, k, answer_cpy, SigmaMode, precision, remove_outliers, filterdir, iter))
			{
				free (pts_cpy);
				return true;
			}
			free (pts_cpy);
			memcpy(answer,answer_cpy,2*sizeof(double));
		}
		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0007", __FILE__, __FUNCSIG__);  return false;}
}
bool RBF::FG_Line_BestFit_2D(double* pts, int PtsCount, double* answer, bool SigmaMode, bool start_pt_constraint, bool end_pt_constraint, double* start_pt, double* end_pt)
{
	try
	{
		bool status = true;
		double x_val, y_val;
		if ((start_pt_constraint) && (end_pt_constraint))
		{
			if ((start_pt == NULL) || (end_pt == NULL)) return false;
			//line is from start to end pt that we already have.
			y_val = *(end_pt + 1) - *(start_pt + 1);
			x_val = *(end_pt) - *(start_pt);
			if (x_val == 0)
			{
				*answer = M_PI / 2.0;
				*(answer + 1) = *start_pt;
			}
			else
			{
				*answer = atan(y_val/x_val);
				*(answer + 1) = *(start_pt + 1) - *(start_pt) * y_val / x_val;
			}
		}
		else if ((start_pt_constraint) && (!end_pt_constraint))
		{
			if (start_pt == NULL) return false;
			//line is from start pt thru centroid of points continued to x value of last point in list
			double centroid_x = 0;
			double centroid_y = 0;
			for (int i = 0; i < PtsCount; i++)
			{
				centroid_x += *(pts + 2 * i);
				centroid_y += *(pts + 2 * i + 1);
			}
			centroid_x /= PtsCount;
			centroid_y /= PtsCount;
			y_val = centroid_y - *(start_pt + 1);
			x_val = centroid_x - *(start_pt);
			if (x_val == 0)
			{
				*answer = M_PI / 2.0;
				*(answer + 1) = *start_pt;
			}
			else
			{
				*answer = atan(y_val/x_val);
				*(answer + 1) = *(start_pt + 1) - *(start_pt) * y_val / x_val;
			}
		}
		else if ((!start_pt_constraint) && (end_pt_constraint))
		{
			if (end_pt == NULL) return false;
			//line is from centroid of points to end point started from x value of first point in list
			double centroid_x = 0;
			double centroid_y = 0;
			for (int i = 0; i < PtsCount; i++)
			{
				centroid_x += *(pts + 2 * i);
				centroid_y += *(pts + 2 * i + 1);
			}
			centroid_x /= PtsCount;
			centroid_y /= PtsCount;
			y_val = *(end_pt + 1) - centroid_y;
			x_val = *(end_pt) - centroid_x;
			if (x_val == 0)
			{
				*answer = M_PI / 2.0;
				*(answer + 1) = *start_pt;
			}
			else
			{
				*answer = atan(y_val/x_val);
				*(answer + 1) = centroid_y - centroid_x * y_val / x_val;
			}
		}
		else 
		{
			status = Line_BestFit_2D(pts, PtsCount, answer, SigmaMode);
		}
		return status;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0008", __FILE__, __FUNCSIG__);  return false;}
}
bool RBF::ParallelLine_BestFit_2D(double* ParentLine, double* pts, int PtsCount, double* answer)
{
	try
	{
		int Stepper = 1;
		//if (PtsCount > 50) Stepper = (int) PtsCount / 40;
		int LineCt = PtsCount / 2; // PtsCount * (PtsCount - 1) / 2; 
		double* slopes; double* intercepts;
		slopes = (double*) malloc(sizeof(double) * LineCt);
		intercepts = (double*) malloc(sizeof(double) * LineCt);
		int i = 0, j = 0, Ct = 0;
		for (i = 0; i < 2 * (int)(PtsCount * 0.5); i += 2 * Stepper)
		{	*(slopes + Ct) = *(ParentLine + 0);
			if (*(ParentLine) == M_PI/2)
			{	*(intercepts + Ct) = *(pts + i);
			}
			else
			{	 *(intercepts + Ct)=  *(pts + i + 1) - tan(*(slopes + Ct)) * *(pts + i);
			}
			Ct += 1;
		}
		//DWORD nn = timeGetTime();

		double min_err = 10000000000;
		double final[2];
		double tL[2]; 
		double ls_err = 0;
		double* tP = (double*) malloc(sizeof(double) * 2);

		for (j = 0; j < Ct; j ++)
		{	ls_err = 0; tL[0] = *(slopes + j); tL[1] = *(intercepts + j);
			for (i = 0; i < PtsCount * 2; i += 2)
			{	for (int k = 0; k < 2; k ++) {tP[k] = *(pts + i + k);}
				double tt = hM->Point_Line_Dist_2D(tP, tL);
				ls_err += tt;	
			}
			if (ls_err < min_err) {min_err = ls_err; final[0] = *(slopes + j); final[1] = *(intercepts + j);}
		}
		*(answer) = final[0]; *(answer + 1) = final[1];
		//	TCHAR ss[100];
		//long tt = (long)timeGetTime() - (long)nn;
		//wsprintf(ss, _T("Ans %d"), tt);
		//MessageBox(hwndVideo, ss, TEXT("RapidCam"), MB_ICONEXCLAMATION);

		free(slopes); free(intercepts); free(tP);
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0009", __FILE__, __FUNCSIG__); return false;}
}

//Best fit circle - find out one circle of 3pt Circle that will minimise errors
bool RBF::Circle_BestFit(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision, bool remove_outliers, int outer, int iter) 
{
	try
	{
		if (PtsCount < 2)
		{	return false;
		}
		if (PtsCount == 2)
		{	*(answer + 0) = (*(pts + 0) + *(pts + 2))/2;
			*(answer + 1) = (*(pts + 1) + *(pts + 3))/2;
			*(answer + 2) = sqrt( pow((*(pts + 0) - *(answer + 0)), 2) + pow((*(pts + 1) - *(answer + 1)), 2));
			return true;
		}
		int gSize = (int)(PtsCount / 3);
		int TotalCircleCt = 100000; 
		int Stepper = (int) (PtsCount / 20);
		if (Stepper == 0) Stepper = 1;

		//Set one array of double which will hold X,Y,Rad successively of all the circles
		double* Circles = (double*) malloc(sizeof(double)* TotalCircleCt * 3);
		//One counter variable	
		int C_Ct = 0;
		double tP[6];

		//Calculate the circles that are required. 
		if (PtsCount < 30)
		{
			for (int i = 0; i < PtsCount - 2; i += Stepper)
			{
				tP[0] = *(pts + (i<<1)); tP[1] = *(pts + (i<<1) + 1);
				for (int j = i + 1; j < PtsCount - 1; j += Stepper)
				{
					tP[2] = *(pts + (j<<1)); tP[3] = *(pts + (j<<1) + 1);
					for (int k = j + 1; k < PtsCount; k += Stepper)
					{
						tP[4] = *(pts + (k<<1)); tP[5] = *(pts + (k<<1) + 1);
						bool gotCircle = Circle_3Pt(&tP[0], Circles + 3 * C_Ct);
						if (gotCircle) C_Ct++;
						if (C_Ct >= TotalCircleCt) goto Analyse;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < gSize; i += Stepper)
			{
				tP[0] = *(pts + (i<<1)); tP[1] = *(pts + (i<<1) + 1);
				for (int j = gSize; j < 2 * gSize; j += Stepper)
				{
					tP[2] = *(pts + (j<<1)); tP[3] = *(pts + (j<<1) + 1);
					for (int k = 2 * gSize; k < PtsCount; k += Stepper)
					{
						tP[4] = *(pts + (k<<1)); tP[5] = *(pts + (k<<1) + 1);
						bool gotCircle = Circle_3Pt(&tP[0], Circles + 3 * C_Ct);
						if (gotCircle) C_Ct++;
						if (C_Ct >= TotalCircleCt) goto Analyse;
					}
				}
			}
		}
		//Now, let us loop through for each circle, and find out the one with least error
		//First we need an array of double to hold the error values
	Analyse:	double* errorValues = (double*) malloc(sizeof(double)* C_Ct);
		double Min = 10000000; int MinIndex = 0;
		double centre_x = 0, centre_y = 0, radius = 0;
		double* currentErrorValue;
		for (int i = 0; i < C_Ct; i++)
		{
			centre_x = *(Circles + i * 3);
			centre_y = *(Circles + i * 3 + 1);
			radius = *(Circles + i * 3 + 2);
			currentErrorValue = errorValues + i;
			*(errorValues + i) = 0;
			for (int j = 0; j < (PtsCount<<1); j += 2)
			{
				*currentErrorValue += abs(sqrt(pow((*(pts + j) - centre_x), 2) + pow((*(pts + j + 1) -
					centre_y), 2)) - radius);
			}
			if (*currentErrorValue < Min)
			{
				Min = *currentErrorValue; MinIndex = i;
			}
		}
		//Copy the best circle to answer pointer location
		//index 0 = centre x ; 1 = centre y; 2 = radius
		for (int i = 0; i < 3; i ++)
		{	*(answer + i) = *(Circles + MinIndex * 3 + i);
		}

		//If we want geometric best-fit let us carry that one out now - 
		if (SigmaMode)// && PtsCount >= 5)
		{	bool BadAnswer = false;
			double* PP = (double*) malloc(sizeof(double) * PtsCount * 2);
			//Shift the pts so that we ensure that the circle is not at origin. 
			double sX = 0;
			double SG[3], SSG[4], SSG2[4];
			double tempE = pow(*(answer + 0), 2) + pow(*(answer + 1), 2);
			if (tempE < 0.1) sX = 10;
			for (int i = 0; i < 3; i ++) { SG[i] = 0; SSG[i] = 0; SSG2[i] = 0;}
			SSG[3] = 10000;
			SSG2[3] = 10000;
			for (int kk = 0; kk < 2*PtsCount; kk ++) 
			{
				*(PP + kk) = *(pts + kk) + sX;
			}
		
			//1. Trial One - Take 1/R and no shift. See if it works...
			//2. First trial did not work. Now try with -1/R
			//3. Now shift the points by 10 mm and then try again with 1/R
			//4. Now try shifted pts with -1/R
			int jj;
			for (jj = 0; jj <= 1; jj ++)
			{   BadAnswer = false;
				/*if (jj > 2) */
					/*ShiftX = sX;*/
	   //         if (jj == 3) 
				//{	for (int kk = 0; kk < 2 * PtsCount; kk ++) *(PP + kk) = *(pts + kk) + ShiftX;
				//}
				//else
				//{	for (int kk = 0; kk < 2 * PtsCount; kk ++) *(PP + kk) = *(pts + kk);
				//}

				SG[0] = 1 / (2 * *(answer + 2));
				if (jj % 2 == 0)  SG[0] = -SG[0];
				SG[2] = SG[0] * (pow(*(answer + 0) + sX, 2) + pow(*(answer + 1) + sX, 2)) - 1/(4 * SG[0]);
				SG[1] = acos(-2 * SG[0] * (*(answer + 0) + sX) / sqrt(1 + 4 * SG[0] * SG[2]));
				//change made on 28 Dec 2011 to use LS_BF_LM instead of LS_BF.
				hM->LS_BF_LM(PP, &SG[0], PtsCount, 0, 3, -1, hM->Circle_LMA, true, 10000, Tolerance, &SSG[0]);

				if (_isnan(SSG[0]) != 0 || (1 + 4 * SSG[0] * SSG[2]) < 0) BadAnswer = true;
				if(!BadAnswer && abs(abs(SSG[2]) - abs(SG[2])) > 100) BadAnswer = true;
				/*if (!BadAnswer) break;*/
				if ((jj == 0) && (BadAnswer == false))
				{
					memcpy(SSG2, SSG, 4*sizeof(double));
				}
			}
			//4th parameter contains the residue which is the least squares deviation
			if (SSG2[3] < 10000)
			{
				if ((BadAnswer == true) || ((BadAnswer == false) && (SSG2[3] < SSG[3])))
				{
					memcpy(SSG, SSG2, 4*sizeof(double));
					BadAnswer = false;
				}
			}
			if (!BadAnswer)
			{   /*if (jj > 2) ShiftX = sX; else ShiftX = 0;*/
				double RR  = abs(1 / (2 * SSG[0]));
				double x  = sqrt(1 + 4 * SSG[0] * SSG[2]) * cos(SSG[1]) / (-2 * SSG[0]);
				double y  = sqrt(1 + 4 * SSG[0] * SSG[2]) * sin(SSG[1]) / (-2 * SSG[0]);
				*(answer + 0) = x - sX; *(answer + 1) = y - sX; *(answer + 2) = RR;
				double ls_err = 0;
				for (int j = 0; j < 2 * PtsCount; j += 2)
				{	ls_err += abs(sqrt(pow((*(pts + j) - *(answer + 0)), 2) + pow((*(pts + j + 1) - 
															*(answer + 1)), 2)) - *(answer + 2));
				}
				if (ls_err < Min)
					Min = ls_err;
				else
				{
					for (int i = 0; i < 3; i ++)
					{	*(answer + i) = *(Circles + MinIndex * 3 + i);
					}
				}
			}
			free(PP);
		}

		free(Circles); free(errorValues);

		if ((precision < 1000000) && (!remove_outliers))
		{
			*(answer + 3) = Min;
			if (Min > precision)
				return false;
		}
		if (remove_outliers)
		{
			double err = 2 * Min/PtsCount;
			double* ptscpy = (double*) malloc(2 * PtsCount * sizeof(double));
			memset(ptscpy,0,2 * PtsCount * sizeof(double));
			int jj = 0;
			for (int i = 0; i < PtsCount * 2; i += 2)
			{
				if (abs(sqrt(pow((*(pts + i) - *(answer + 0)), 2) + pow((*(pts + i + 1) - 
														*(answer + 1)), 2)) - *(answer + 2)) < err)
				{
					*(ptscpy + jj) = *(pts + i); *(ptscpy + jj + 1) = *(pts + i + 1);
					jj += 2;
				}
			}
			int ptscnt = jj/2;
			if (!Circle_BestFit(ptscpy, ptscnt, answer, SigmaMode))
			{
				free(ptscpy);
				return false;
			}
			free(ptscpy);
		}
		if ((outer != 0) && (iter > 0))
		{
			double* pts_cpy = (double*) malloc(sizeof(double) * PtsCount * 2);
			int k = 0;
			switch (outer)
			{
				case 1:
					for (int i = 0; i < PtsCount; i ++) 
					{
						if (pow((*(pts + i) - *(answer + 0)), 2) + pow((*(pts + i + 1) - *(answer + 1)), 2) - pow(*(answer + 2), 2) > 0)
						{
							*(pts_cpy + (k<<1)) = *(pts + (i<<1));
							*(pts_cpy + (k<<1) + 1) = *(pts + (i<<1) + 1);
							k++;
						}
					}
					break;
				case -1:
					for (int i = 0; i < PtsCount; i ++) 
					{
						if (pow((*(pts + i) - *(answer + 0)), 2) + pow((*(pts + i + 1) - *(answer + 1)), 2) - pow(*(answer + 2), 2) < 0)
						{
							*(pts_cpy + (k<<1)) = *(pts + (i<<1));
							*(pts_cpy + (k<<1) + 1) = *(pts + (i<<1) + 1);
							k++;
						}
					}
					break;
				default:
					free (pts_cpy);
					return true;
					break;
			}
			iter--;
			if (k < 10)
			{
				free (pts_cpy);
				return true;
			}
			double answer_cpy[4] = {0};
			if (!Circle_BestFit(pts_cpy, k, answer_cpy, SigmaMode, precision, remove_outliers, outer, iter))
			{
				free (pts_cpy);
				return true;
			}
			free (pts_cpy);
			memcpy(answer,answer_cpy,4*sizeof(double));
		}
		return true;
	}
	catch(...){SetAndRaiseErrorMessage("RBF0010", __FILE__, __FUNCSIG__); return false;}
}

//Best fit cam profile 
bool RBF::CamProfile_BestFit(double* pts, int PtsCount, double* initGuess, double* answer, bool center_frozen) 
{
	try
	{
		//if (PtsCount < 12)
		//	return false;
		bool LS_BF_successful = hM->LS_BF_LM(pts, initGuess, PtsCount, 2, 4, -1, hM->CamProfile, true, 10000, Tolerance, answer);
		if (LS_BF_successful)
		{
			if (answer[2] < 0)
				answer[2] = - answer[2];
			answer[3] -= int(answer[3] / (2 * M_PI) ) * 2 * M_PI ;
			if (answer[3] < 0)
				answer[3] += 2 * M_PI;
		}
		return LS_BF_successful;
	}
	catch(...){SetAndRaiseErrorMessage("RBF0010.b", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::CamProfile_GetDist(double* CamParam, double ang, double* dist)
{
	try
	{
		ang -= int(ang /( 2 * M_PI) ) * 2 * M_PI ;
		if (ang < 0)
			ang += 2 * M_PI;
		double c = ang - CamParam[3];
		c -= int(c / (2 * M_PI) ) * 2 * M_PI ;
		if (c < 0)
			c += 2 * M_PI;
		double radius = CamParam[2];

		if ((c <= M_PI/6) || (c >=11*M_PI/6))
		{
			*dist = radius;
		}
		else if ((c >= 5*M_PI/6) && (c <=7*M_PI/6))
		{
			*dist = radius + 4;
		}
		else if ((c > M_PI/6) && (c <5*M_PI/6))
		{
			*dist = radius + 2 * (1 - cos(1.5*c - M_PI/4));
		}
		else
		{
			*dist = radius + 2 * (1 - sin(1.5*c - M_PI/4));
		}
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0010.c", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::CamProfile_GetDist(double* CamParamLocal, double* CamParamGlobal, double ang, double* dist)
{
	try
	{
		double dist_gbl = 0;
		if (!CamProfile_GetDist(CamParamGlobal, ang, &dist_gbl))
			return false;
		double x_gbl = dist_gbl * cos (ang) + CamParamGlobal[0];
		double y_gbl = dist_gbl * sin (ang) + CamParamGlobal[1];
		double delta_x_local = x_gbl - CamParamLocal[0];
		double delta_y_local = y_gbl - CamParamLocal[1];
		double delta_norm = sqrt(pow(delta_x_local,2) + pow(delta_y_local,2));
		if (delta_norm == 0)
			return false;
		double ang_local = acos (delta_x_local/delta_norm);
		if (delta_y_local < 0)
			ang_local = 2 * M_PI - ang_local;
		double dist_lcl = 0;
		if (!CamProfile_GetDist(CamParamLocal, ang_local, &dist_lcl))
			return false;
		double x_lcl = dist_lcl * cos (ang_local) + CamParamLocal[0];
		double y_lcl = dist_lcl * sin (ang_local) + CamParamLocal[1];
		double delta_x_gbl = x_lcl - CamParamGlobal[0];
		double delta_y_gbl = y_lcl - CamParamGlobal[1];
		*dist = sqrt(pow(delta_x_gbl,2) + pow(delta_y_gbl,2));
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0010.d", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::CircleWithCenter_BestFit(double* pts, int PtsCount, double* answer, bool remove_outliers, int outer, int iter)
{
	try
	{
		double dist = 0;
		for (int i = 0; i < PtsCount; i++)
		{
			dist += hM->pythag(*(pts + 2*i) - *answer, *(pts + 2*i + 1) - *(answer + 1));
		}
		*(answer + 2) = dist / PtsCount;
		if ( PtsCount > 1 && remove_outliers)
		{
			double err = 0; 
			for (int i = 0; i < PtsCount; i ++)
			{	
				err += abs(hM->pythag(*(pts + 2*i) - *answer, *(pts + 2*i + 1) - *(answer + 1)) - *(answer + 2));
			}
			err = 2 * err/PtsCount;
			double* ptscpy = (double*) malloc(2*PtsCount*sizeof(double));
			memset(ptscpy,0,2*PtsCount*sizeof(double));
			int jj = 0;
			for (int i = 0; i < PtsCount; i ++)
			{
				if (abs(hM->pythag(*(pts + 2*i) - *answer, *(pts + 2*i + 1) - *(answer + 1)) - *(answer + 2)) < err)
				{
					*(ptscpy + 2*jj) = *(pts + 2*i); *(ptscpy + 2*jj + 1) = *(pts + 2*i + 1);
					jj ++;
				}
			}
			int ptscnt = jj;
			if (!CircleWithCenter_BestFit(ptscpy, ptscnt, answer))
			{
				free(ptscpy);
				return false;
			}
			free(ptscpy);
		}
		if ((outer != 0) && (iter > 0))
		{
			double* pts_cpy = (double*) malloc(sizeof(double) * PtsCount * 2);
			int k = 0;
			switch (outer)
			{
				case 1:
					for (int i = 0; i < PtsCount; i ++) 
					{
						if (pow((*(pts + i) - *(answer + 0)), 2) + pow((*(pts + i + 1) - *(answer + 1)), 2) - pow(*(answer + 2), 2) > 0)
						{
							*(pts_cpy + 2*k) = *(pts + 2*i);
							*(pts_cpy + 2*k + 1) = *(pts + 2*i + 1);
							k++;
						}
					}
					break;
				case -1:
					for (int i = 0; i < PtsCount; i ++) 
					{
						if (pow((*(pts + i) - *(answer + 0)), 2) + pow((*(pts + i + 1) - *(answer + 1)), 2) - pow(*(answer + 2), 2) < 0)
						{
							*(pts_cpy + 2*k) = *(pts + 2*i);
							*(pts_cpy + 2*k + 1) = *(pts + 2*i + 1);
							k++;
						}
					}
					break;
				default:
					free (pts_cpy);
					return true;
					break;
			}
			iter--;
			if (k < 10)
			{
				free (pts_cpy);
				return true;
			}
			double answer_cpy[3] = {answer[0], answer[1], 0};
			if (!CircleWithCenter_BestFit(pts_cpy, k, answer_cpy, remove_outliers, outer, iter))
			{
				free (pts_cpy);
				return true;
			}
			free (pts_cpy);
			*(answer + 2) = answer_cpy[2];
		}
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0010a", __FILE__, __FUNCSIG__); 
		return false;
	}
}

bool RBF::FG_Circle_BestFit(double* pts, int PtsCount, double* answer, bool start_pt_constraint, bool end_pt_constraint, double* start_pt, double* end_pt) 
{	
	try
	{
		if ((!start_pt_constraint) && (!end_pt_constraint))
			return Circle_BestFit(pts, PtsCount, answer, false);

		bool both_end_pts_fixed = false;
		double fixed_pt_x, fixed_pt_y, fixed_pt2_x, fixed_pt2_y;
		if ((start_pt_constraint) && (!end_pt_constraint))
		{ 
			if (start_pt == NULL) return false;
			fixed_pt_x = *start_pt;
			fixed_pt_y = *(start_pt + 1);
		}
		else if ((!start_pt_constraint) && (end_pt_constraint))
		{
			if (end_pt == NULL) return false;
			fixed_pt_x = *end_pt;
			fixed_pt_y = *(end_pt + 1);
		}
		else 
		{
			if ((start_pt == NULL) || (end_pt == NULL)) return false;
			both_end_pts_fixed = true;
			fixed_pt_x = *start_pt;
			fixed_pt_y = *(start_pt + 1);
			fixed_pt2_x = *end_pt;
			fixed_pt2_y = *(end_pt + 1);
		}
		int gSize = (int)(PtsCount / 2);
		int TotalCircleCt = 100000; 
		int Stepper = (int) (PtsCount / 20);
		if (Stepper == 0) Stepper = 1;

		//Set one array of double which will hold X,Y,Rad successively of all the circles
		double* Circles = (double*) malloc(sizeof(double)* TotalCircleCt * 3);
		//One counter variable	
		int C_Ct = 0;
		double tP[6];

		if (!both_end_pts_fixed)
		{
			//Calculate the circles that are required. 
			tP[4] = fixed_pt_x; tP[5] = fixed_pt_y;
			if (PtsCount < 30)
			{	for (int i = 0; i < PtsCount - 1; i += Stepper)
				{	tP[0] = *(pts + 2 * i); tP[1] = *(pts + 2 * i + 1);
					for (int j = i + 1; j < PtsCount ; j += Stepper)
					{	tP[2] = *(pts + 2 * j); tP[3] = *(pts + 2 * j + 1);
						bool gotCircle = Circle_3Pt(&tP[0], Circles + 3 * C_Ct);
						if (gotCircle) C_Ct ++;
						if (C_Ct >= TotalCircleCt) goto Analyse;
					}
				}
			}
			else
			{	for (int i = 0; i < gSize; i += Stepper)
				{	tP[0] = *(pts + 2 * i); tP[1] = *(pts + 2 * i + 1);
					for (int j = gSize; j < PtsCount; j += Stepper)
					{	tP[2] = *(pts + 2 * j); tP[3] = *(pts + 2 * j + 1);
						bool gotCircle = Circle_3Pt(&tP[0], Circles + 3 * C_Ct);
						if (gotCircle) C_Ct ++;
						if (C_Ct >= TotalCircleCt) goto Analyse;
					}
				}
			}
		}
		else //if both end points fixed
		{
			//Calculate the circles that are required. 
			tP[4] = fixed_pt_x; tP[5] = fixed_pt_y;
			tP[2] = fixed_pt2_x; tP[3] = fixed_pt2_y;
			for (int i = 0; i < PtsCount; i += Stepper)
			{	tP[0] = *(pts + 2 * i); tP[1] = *(pts + 2 * i + 1);
				bool gotCircle = Circle_3Pt(&tP[0], Circles + 3 * C_Ct);
				if (gotCircle) C_Ct ++;
				if (C_Ct >= TotalCircleCt) goto Analyse;
			}
		}
		//Now, let us loop through for each circle, and find out the one with least error
		//First we need an array of double to hold the error values
	Analyse:	double* errorValues = (double*) malloc(sizeof(double)* C_Ct);
		double Min = 10000000; int MinIndex = 0;
		for (int i = 0 ; i < C_Ct; i ++)
		{	*(errorValues + i) = 0;
			for (int j = 0; j < 2 * PtsCount; j += 2)
			{	*(errorValues + i) += sqrt(abs(pow((*(pts + j) - *(Circles + i * 3 + 0)), 2) + pow((*(pts + j + 1) - 
														*(Circles + i * 3 + 1)), 2) - pow(*(Circles + i * 3 + 2), 2)));
			}
			if (*(errorValues + i) < Min)
			{	Min = *(errorValues + i); MinIndex = i;
			}
		}
		//Copy the best circle to answer pointer location
		//index 0 = centre x ; 1 = centre y; 2 = radius
		for (int i = 0; i < 3; i ++)
		{	*(answer + i) = *(Circles + MinIndex * 3 + i);
		}
	
		free(Circles); free(errorValues);
		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0011", __FILE__, __FUNCSIG__); return false;}
}
bool RBF::Ellipse_BestFit(double* pts, int PtsCount, double* answer)
{	
	try
	{
		double* D1 = (double*) malloc(sizeof(double) * PtsCount * 3); 
		double* D2 = (double*) malloc(sizeof(double) * PtsCount * 3);
		double* D1T = (double*) malloc(sizeof(double) * PtsCount * 3); 
		double* D2T = (double*) malloc(sizeof(double) * PtsCount * 3);

		double S1[9], S2[9], S2T[9];
		double S3[9], T[9], S3Inv[9], C1Inv[9];
		double C1[9] = {0, 0, 2, 0, -1, 0, 2, 0, 0};
		double M[9], tempM[9], tM2[9], EigenV[3];

		//First let us get the matrix of x^2, x.y and y^2 and X, y , 1
		for (int i = 0; i < PtsCount; i ++)
		{	D1[i * 3] = pts[i * 2] * pts[i * 2]; // x^2
			D1[i * 3 + 1] = pts[i * 2] * pts[i * 2 + 1]; //xy
			D1[i * 3 + 2] = pts[i * 2 + 1] * pts[i * 2 + 1];//y^2
		
			D2[i * 3] = pts[i * 2];
			D2[i * 3 + 1] = pts[i * 2 + 1];
			D2[i * 3 + 2] = 1;
		}
		hM->TransposeMatrix(D1, D1T, PtsCount, 3);
		hM->TransposeMatrix(D2, D2T, PtsCount, 3);
		int Size1[2], Size2[2], Size3[2];
		Size1[0] = PtsCount; Size1[1] = 3; 
		Size2[0] = 3; Size2[1] = PtsCount;
		Size3[0] = 3; Size2[1] = 3;

		hM->MultiplyMatrix(D1T, &Size2[0], D1, &Size1[0], &S1[0]);
		hM->MultiplyMatrix(D1T, &Size2[0], D2, &Size1[0], &S2[0]);
		hM->MultiplyMatrix(D2T, &Size2[0], D2, &Size1[0], &S3[0]);

		hM->OperateMatrix(&S3[0], 3, 1, &S3Inv[0]);
		hM->TransposeMatrix(&S2[0], &S2T[0], 3, 3);
		hM->MultiplyMatrix(&S3Inv[0], &Size3[0], &S2T[0], &Size3[0], &T[0]);
		hM->MultiplyMatrix(-1.0, &T[0], &Size3[0]);
		hM->MultiplyMatrix(&S2[0], &Size3[0], &T[0], &Size3[0], &M[0]);
		hM->OperateMatrix(&C1[0], 3, 1, &C1Inv[0]);
		for (int i = 0; i < 9; i ++) M[i] += S1[i];
		hM->MultiplyMatrix(&C1Inv[0], &Size3[0], &M[0], &Size3[0], &tM2[0]);
		for (int i = 0; i < 9; i ++) tempM[i] += tM2[i];
		ZeroMemory(&EigenV[0], 3 * sizeof(double));
		hM->H_QR_EigenValues(&tempM[0], 3, &EigenV[0]);


		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0012", __FILE__, __FUNCSIG__); return false;}
}

//
// The pts array should contain pts in 3D i.e., x1,y1,z1,x2,y2,z2 etc
bool RBF::Plane_BestFit(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision, int upperLower, int iter)
{	
	try
	{
		int PlaneCt = 50000; // PtsCount * (PtsCount - 1) / 2; 
		double* A; double* B; double* C; double* D;
		A = (double*) malloc(sizeof(double) * PlaneCt);
		B = (double*) malloc(sizeof(double) * PlaneCt);
		C = (double*) malloc(sizeof(double) * PlaneCt);
		D = (double*) malloc(sizeof(double) * PlaneCt);
		double tPlane[4]; double P[9];
		int i = 0, j = 0, k= 0, Ct = 0;
		int step = (int) (PtsCount/40);
		if (step ==0 ) step = 1;

		for (i = 0; i < PtsCount - 2; i += step)
		{	for (j = i + 1; j < PtsCount - 1; j += step)
			{	for (k = j + 1; k < PtsCount; k += step)
				{	
					P[0] = *(pts + i * 3); P[1] = *(pts + i  * 3 + 1);  P[2] = *(pts + i  * 3 + 2);
					P[3] = *(pts + j * 3); P[4] = *(pts + j * 3 + 1);  P[5] = *(pts + j * 3 + 2);
					P[6] = *(pts + k * 3); P[7] = *(pts + k * 3 + 1);  P[8] = *(pts + k * 3 + 2);
					if ( Plane_3Pt_2(&P[0], &tPlane[0]))
					{	*(A + Ct) = tPlane[0];	*(B + Ct) = tPlane[1];
						*(C + Ct) = tPlane[2];	*(D + Ct) = tPlane[3];
						Ct += 1;
						if (Ct == PlaneCt) goto AA;
					}
				}
			}
		}

	AA:
	
		double min_err = 10000000000;
		double final[4];
		double ls_err = 0;

		for (j = 0; j < Ct; j ++)
		{	ls_err = 0; 
			for (i = 0; i < PtsCount * 3; i += 3)
			{	ls_err += abs(*(A + j) * *(pts + i) + *(B + j) * *(pts + i + 1) + *(C + j) * *(pts + i + 2) + *(D + j));	
			}
			if (ls_err < min_err) 
			{	min_err = ls_err; final[0] = *(A + j); final[1] = *(B + j); final[2] = *(C + j); final[3] = *(D + j);
			}
		}
		*(answer) = final[0]; *(answer + 1) = final[1]; *(answer + 2) = final[2]; *(answer + 3) = final[3];
		//	TCHAR ss[100];
		//long tt = (long)timeGetTime() - (long)nn;
		//wsprintf(ss, _T("Ans %d"), tt);
		//MessageBox(hwndVideo, ss, TEXT("RapidCam"), MB_ICONEXCLAMATION);
		if (SigmaMode && PtsCount > 10)
		{	double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
			//memcpy((void*) localPts, (void*) pts, sizeof(double) * PtsCount * 3);
			for (i = 0; i < PtsCount * 3; i ++) *(localPts + i) = *(pts + i);

			double Centroid[3], Diag[3], valueDecomp[9];
			ZeroMemory(&Centroid[0], sizeof(double) * 3);
			ZeroMemory(&Diag[0], sizeof(double) * 3);
			ZeroMemory(&valueDecomp[0], sizeof(double) * 9);

			if (hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0]))
			{	hM->SVD(localPts, 3, 3, &Diag[0], &valueDecomp[0]);
				double tempD = 0;
				int eV = 0;
				//Sort the diagonal matrix in increasing order into ArrangedDiag
				double minDiag = Diag[0];
				for (i = 0; i < 3; i ++) if (Diag[i] < minDiag && Diag[i] > 0) {minDiag = Diag[i]; eV = i;}
			
				//for (i = 0; i < 3; i ++) if (Diag[i] == minDiag) eV = i;
			
				double modA = 0; // holds the sqrt(a2 + b2 + c2)
				for (i = 0; i < 3; i ++)
				{	*(answer + i) = valueDecomp[eV + i * 3];
					modA += valueDecomp[eV + i * 3] * valueDecomp[eV + i * 3];
				}
				//Normalise to get direction cosines
				for (i = 0; i < 3; i ++) *(answer + i) /= modA;
				//Get the intercept
				*(answer + 3) = 0;
				for (i = 0; i < 3; i ++) *(answer + 3) -= *(answer + i) * Centroid[i];	
				ls_err = 0; 
				for (i = 0; i < PtsCount * 3; i += 3)
				{	ls_err += abs(*(answer + 0) * *(pts + i) + *(answer + 1) * *(pts + i + 1) + *(answer + 2) * *(pts + i + 2) + *(answer + 3));	
				}
				if (ls_err < min_err) 
					min_err = ls_err; 
				else
				{
					*(answer) = final[0]; *(answer + 1) = final[1]; *(answer + 2) = final[2]; *(answer + 3) = final[3];
				}
			}
			free(localPts);
		}
		free(A); free(B); free(C); free(D);
		if (precision < 1000000)
		{
			*(answer + 4) = min_err;
			if (min_err > precision) 
				return false;
		}
		if ((upperLower != 0)&&(iter > 0))
		{
			double* pts_cpy = (double*) malloc(sizeof(double) * PtsCount * 3);
			memset (pts_cpy, 0, sizeof(double) * PtsCount * 3);
			int k = 0;
			if (upperLower == 1)
			{
				for (i = 0; i < PtsCount; i ++) 
				{
					if (*(answer + 2) * (*(answer + 0) * *(pts + 3*i) + *(answer + 1) * *(pts + 3*i + 1) + *(answer + 2) * *(pts + 3*i + 2) + *(answer + 3)) > 0)
					{
						*(pts_cpy + 3*k) = *(pts + 3*i);
						*(pts_cpy + 3*k + 1) = *(pts + 3*i + 1);
						*(pts_cpy + 3*k + 2) = *(pts + 3*i + 2);
						k++;
					}
				}
			}
			else
			{
				for (i = 0; i < PtsCount; i ++) 
				{
					if (*(answer + 2) * (*(answer + 0) * *(pts + 3*i) + *(answer + 1) * *(pts + 3*i + 1) + *(answer + 2) * *(pts + 3*i + 2) + *(answer + 3)) < 0)
					{
						*(pts_cpy + 3*k) = *(pts + 3*i);
						*(pts_cpy + 3*k + 1) = *(pts + 3*i + 1);
						*(pts_cpy + 3*k + 2) = *(pts + 3*i + 2);
						k++;
					}
				}
			}
			iter--;
			if (k < 10)
			{
				free (pts_cpy);
				return true;
			}
			double answer_cpy[5] = {0};
			if (!Plane_BestFit(pts_cpy, k, answer_cpy, SigmaMode, precision, upperLower, iter))
			{
				free (pts_cpy);
				return true;
			}
			free (pts_cpy);
			memcpy (answer, answer_cpy, 5 * sizeof(double));		
		}
		return true;
	}
	catch(...) { 
		SetAndRaiseErrorMessage("RBF0013", __FILE__, __FUNCSIG__);return false;}
}

bool RBF::FilteredPlane_BestFit(double* pts_in, int PtsCount_in, double* pts_out, int* PtsCount_out, double* answer, bool SigmaMode, double precision, int upperLower)
{	
	try
	{
		if (!Plane_BestFit(pts_in, PtsCount_in, answer, SigmaMode, precision))
		{
			*PtsCount_out = -1;
			return false;
		}
		int k = 0;
		if (upperLower == 1)
		{
			for (int i = 0; i < PtsCount_in; i ++) 
			{
				if (*(answer + 2) * (*(answer + 0) * *(pts_in + 3*i) + *(answer + 1) * *(pts_in + 3*i + 1) + *(answer + 2) * *(pts_in + 3*i + 2) + *(answer + 3)) > 0)
				{
					*(pts_out + 3*k) = *(pts_in + 3*i);
					*(pts_out + 3*k + 1) = *(pts_in + 3*i + 1);
					*(pts_out + 3*k + 2) = *(pts_in + 3*i + 2);
					k++;
				}
			}
		}
		else
		{
			for (int i = 0; i < PtsCount_in; i ++) 
			{
				if (*(answer + 2) * (*(answer + 0) * *(pts_in + 3*i) + *(answer + 1) * *(pts_in + 3*i + 1) + *(answer + 2) * *(pts_in + 3*i + 2) + *(answer + 3)) < 0)
				{
					*(pts_out + 3*k) = *(pts_in + 3*i);
					*(pts_out + 3*k + 1) = *(pts_in + 3*i + 1);
					*(pts_out + 3*k + 2) = *(pts_in + 3*i + 2);
					k++;
				}
			}
		}
		*PtsCount_out = k;
		if (k < 10)
		{
			return false;
		}
		double answer_cpy[5] = {0};
		if (!Plane_BestFit(pts_out, k, answer_cpy, SigmaMode, precision))
		{
			return false;
		}
		memcpy (answer, answer_cpy, 5 * sizeof(double));		
		return true;
	}
	catch(...) { 
		SetAndRaiseErrorMessage("RBF0013.1", __FILE__, __FUNCSIG__);return false;}
}

bool RBF::Line_BestFit_3D(double* pts, int PtsCount, double* answer)
{
	try
	{
		if (PtsCount < 2) return false;
		if (PtsCount == 2)
		{
			*(answer + 0) = *(pts + 0); *(answer + 1) = *(pts + 1); *(answer + 2) = *(pts + 2);
			double dir1 = *(pts + 3) - *(pts + 0);
			double dir2 = *(pts + 4) - *(pts + 1);
			double dir3 = *(pts + 5) - *(pts + 2);
			double mod = sqrt(pow(dir1,2) + pow(dir2,2) + pow(dir3,2));
			if (mod == 0) return false;
			*(answer + 3) = dir1/mod; *(answer + 4) = dir2/mod; *(answer + 5) = dir3/mod;
			return true;
		}

		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
		for (int i = 0; i < PtsCount * 3; i ++) *(localPts + i) = *(pts + i);
		double Centroid[3], Diag[3], valueDecomp[9];
		ZeroMemory(&Centroid[0], sizeof(double) * 3);
		ZeroMemory(&Diag[0], sizeof(double) * 3);
		ZeroMemory(&valueDecomp[0], sizeof(double) * 9);

		if (hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0]))
		{	hM->SVD(localPts, 3, 3, &Diag[0], &valueDecomp[0]);
			double tempD = 0;
			int eV = 0;
				//Sort the diagonal matrix in increasing order into ArrangedDiag
			double maxDiag = Diag[0];
			for (int i = 0; i < 3; i ++) if (Diag[i] > maxDiag && Diag[i] > 0) {maxDiag = Diag[i]; eV = i;}
			
			double modA = 0; // holds the sqrt(a2 + b2 + c2)
			for (int i = 0; i < 3; i ++)
			{	*(answer + 3 + i) = valueDecomp[eV + i * 3];
				modA += valueDecomp[eV + i * 3] * valueDecomp[eV + i * 3];
			}
			if (modA == 0)
			{
				free(localPts);
				return false;
			}
				//Normalise to get direction cosines
			for (int i = 0; i < 3; i ++) *(answer + 3 + i) /= modA;
				//Get the x,y,z coordinates by translating origin by centroid.
			for (int i = 0; i < 3; i ++) *(answer + i) = Centroid[i];			
		}
		free(localPts);

		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0014", __FILE__, __FUNCSIG__); return false;}
}

//The pts will be in 3D
bool RBF::Sphere_BestFit(double* pts, int PtsCount, double* answer, bool SigmaMode, double precision)
{	
	try
	{
		double min_err = 10000000000;
		double final[4];

		if (answer[3] != 0)
		{
			memcpy(final,answer,4*sizeof(double));
			min_err = 0;
			for (int i = 0; i < PtsCount * 3; i += 3)
			{	
				min_err += abs(pow(*(final + 0) - *(pts + i), 2) + pow(*(final + 1) - *(pts + i + 1), 2) + pow(*(final + 2) - *(pts + i + 2), 2)  - pow(*(final + 3), 2));	
			}
		}
		int SphereCt = 50000; 
		double* X; double* Y; double* Z; double* Rad;
		X = (double*) malloc(sizeof(double) * SphereCt);
		Y = (double*) malloc(sizeof(double) * SphereCt);
		Z = (double*) malloc(sizeof(double) * SphereCt);
		Rad = (double*) malloc(sizeof(double) * SphereCt);
		double tSphere[4]; double P[12];
		int i_1 = 0, i_2 = 0, i_3= 0, i_4 = 0, Ct = 0;
		int step = (int) (PtsCount/15);
		if (step ==0 ) step = 1;

		for (i_1 = 0; i_1 < PtsCount - 3; i_1 += step)
		{	
			for (i_2 = i_1 + 1; i_2 < PtsCount - 2; i_2 += step)
			{	
				for (i_3 = i_2 + 1; i_3 < PtsCount -1; i_3 += step)
				{
					for (i_4 = i_3 + 1; i_4 < PtsCount; i_4 += step)
					{
						P[0] = *(pts + i_1 * 3); P[1] = *(pts + i_1 * 3 + 1);  P[2] = *(pts + i_1 * 3 + 2);
						P[3] = *(pts + i_2 * 3); P[4] = *(pts + i_2 * 3 + 1);  P[5] = *(pts + i_2 * 3 + 2);
						P[6] = *(pts + i_3 * 3); P[7] = *(pts + i_3 * 3 + 1);  P[8] = *(pts + i_3 * 3 + 2);
						P[9] = *(pts + i_4 * 3); P[10] = *(pts + i_4 * 3 + 1);  P[11] = *(pts + i_4 * 3 + 2);
						if ( Sphere_4Pt(&P[0], &tSphere[0]))
						{	*(X + Ct) = tSphere[0];	*(Y + Ct) = tSphere[1];
							*(Z + Ct) = tSphere[2];	*(Rad + Ct) = tSphere[3];
							Ct += 1;
							if (Ct == SphereCt) goto AA;
						}
					}
				}
			}
		}

	AA:
	

		double ls_err = 0;

		for (int j = 0; j < Ct; j ++)
		{	ls_err = 0; 
			for (int i = 0; i < PtsCount * 3; i += 3)
			{	ls_err += abs(pow(*(X + j) - *(pts + i), 2) + pow(*(Y + j) - *(pts + i + 1), 2) + pow(*(Z + j) - *(pts + i + 2), 2)  - pow(*(Rad + j), 2));	
			}
			if (ls_err < min_err) {min_err = ls_err; final[0] = *(X + j); final[1] = *(Y + j); final[2] = *(Z + j); final[3] = *(Rad + j);}
		}
		*(answer) = final[0]; *(answer + 1) = final[1]; *(answer + 2) = final[2]; *(answer + 3) = final[3];
		free(X); free(Y); free(Z); free(Rad);

		//if points count > 10 always try for sigma mode best fit.  anyway best fit returned is being compared with above and better best fit returned.
		if (PtsCount > 10)
		{
			bool LS_BF_successful = hM->LS_BF_LM(pts, &final[0], PtsCount, 3, 4, -1, hM->Sphere,true, 10000, Tolerance, &answer[0]);
			if (!LS_BF_successful)
				memcpy(answer, final, 4*sizeof(double));
			if (LS_BF_successful)
			{
				if (answer[3] < 0)
					answer[3] = - answer[3];
				double ls_err2 = 0;
				for (int i = 0; i < PtsCount * 3; i += 3)
				{	ls_err2 += abs(pow(*(answer + 0) - *(pts + i), 2) + pow(*(answer + 1) - *(pts + i + 1), 2) + pow(*(answer + 2) - *(pts + i + 2), 2)  - pow(*(answer + 3), 2));	
				}
				if (ls_err2 < min_err)
					min_err = ls_err2;
				else
					memcpy(answer, final, 4*sizeof(double));
			}
		}
		if (precision < 1000000)
		{
			*(answer + 4) = min_err;
			if (min_err > precision) 
				return false;
		}
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0015", __FILE__, __FUNCSIG__);return false;}
}

//The pts will be in 3D
//Cylinder params = x,y,z, l,m,n, rad ( in that order in the array)
bool RBF::Cylinder_BestFit(double* pts, int PtsCount, double* answer, double precision, bool remove_outliers, bool axial_bestfit)
{
	try
	{
		double Cylinder1[8], Cylinder2[8], Cylinder3[8], initGuess[7];
		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
		double Centroid[3];
		ZeroMemory(&Centroid[0], sizeof(double) * 3);
		ZeroMemory(&Cylinder1[0], sizeof(double) * 8);
		ZeroMemory(&Cylinder2[0], sizeof(double) * 8);
		ZeroMemory(&Cylinder3[0], sizeof(double) * 8);
		ZeroMemory(&initGuess[0], sizeof(double) * 7);

		//memcpy((void*) localPts, (void*)pts, sizeof(double) * PtsCount * 3);
		for (int i = 0; i < PtsCount * 3; i ++) *(localPts + i) = *(pts + i);

		if (!hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0])) 
		{	free(localPts); return false;
		}

		//Set rest of initial guess values
	
		double first3pts[9];
		double first3ptsCircle[7];
		int i =0;

		for (i=0; i<9; i++) first3pts[i] = *(pts+i);

		//get plane direction cosines by passing first 3 points
		while (i <= PtsCount*3)
		{
			if (Circle_3Pt_3D(first3pts, first3ptsCircle)) 
			{	
				i = i - 1;
				break; 
			}
			if (i >= PtsCount*3) break;

			first3pts[6] = *(pts+i);
			first3pts[7] = *(pts+i+1);
			first3pts[8] = *(pts+i+2);
			i += 3;
		}

		//if we failed to get a circle with 3 initial points, return false
		if (i >= PtsCount*3)
		{
			free(localPts);
			return false;
		}
		initGuess[0] = first3ptsCircle[0] - Centroid[0]; initGuess[1] = first3ptsCircle[1] - Centroid[1]; initGuess[2] = first3ptsCircle[2] - Centroid[2];
		initGuess[3] = first3ptsCircle[3]; initGuess[4] = first3ptsCircle[4]; initGuess[5] = first3ptsCircle[5]; initGuess[6] = first3ptsCircle[6];
		
		double res1 = -1, res2 = -1, res3 = -1;
		bool LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 6, 7, -1, hM->Cylinder,true, 10000, Tolerance, &Cylinder1[0]);
		if (!LS_BF_successful)
		{
				double err = 0; 
				for (i = 0; i < PtsCount * 3; i += 3)
				{	
					err += pow(pow(hM->Point_Line_Dist_3D((localPts + i), initGuess),2) - pow(*(initGuess + 6),2),2);
				}
				LS_BF_successful = true;
				memcpy(Cylinder1, initGuess, sizeof(double) * 7);
				Cylinder1[7] = err;
		}
		if (axial_bestfit)
		{
			double line3d[6] = {0};
			if (Line_BestFit_3D(localPts, PtsCount, line3d))
			{
				for (int m = 0; m < 6; m++)
					initGuess[m] = line3d[m];
				initGuess[6] = hM->Point_Line_Dist_3D(localPts, line3d);
				bool LS_BF_successful3 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 6, 7, -1, hM->Cylinder,true, 10000, Tolerance, &Cylinder3[0]);
				if (!LS_BF_successful3)
				{
						double err = 0; 
						for (i = 0; i < PtsCount * 3; i += 3)
						{	
							err += pow(pow(hM->Point_Line_Dist_3D((localPts + i), initGuess),2) - pow(*(initGuess + 6),2),2);
						}
						LS_BF_successful3 = true;
						memcpy(Cylinder3, initGuess, sizeof(double) * 7);
						Cylinder3[7] = err;
				}
				if (LS_BF_successful3)
				{
					res3 = *(Cylinder3 + 7);
					if (LS_BF_successful)
					{
						res1 = *(Cylinder1 + 7);
						if (res3 < res1)
							memcpy(Cylinder1, Cylinder3, sizeof(double) * 8);
					}
					else
					{
						LS_BF_successful = true;
						memcpy(Cylinder1, Cylinder3, sizeof(double) * 8);
					}
				}
			}
		}
		bool LS_BF_successful2 = false;
		if (answer[6] > 0)
		{
			initGuess[0] = answer[0] - Centroid[0]; initGuess[1] = answer[1] - Centroid[1]; initGuess[2] = answer[2] - Centroid[2];
			initGuess[3] = answer[3]; initGuess[4] = answer[4]; initGuess[5] = answer[5]; initGuess[6] = answer[6];
			LS_BF_successful2 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 6, 7, -1, hM->Cylinder,true, 10000, Tolerance, &Cylinder2[0]);
			if (!LS_BF_successful2)
			{
					double err = 0; 
					for (i = 0; i < PtsCount * 3; i += 3)
					{	
						err += pow(pow(hM->Point_Line_Dist_3D((localPts + i), initGuess),2) - pow(*(initGuess + 6),2),2);
					}
					LS_BF_successful2 = true;
					memcpy(Cylinder2, initGuess, sizeof(double) * 7);
					Cylinder2[7] = err;
			}
			if (LS_BF_successful2)
			{
				res2 = *(Cylinder2 + 7);
				if (LS_BF_successful)
				{
					res1 = *(Cylinder1 + 7);
					if (res2 < res1)
						memcpy(Cylinder1, Cylinder2, sizeof(double) * 8);
				}
				else
				{
					LS_BF_successful = true;
					memcpy(Cylinder1, Cylinder2, sizeof(double) * 8);
				}
			}
		}

		free(localPts);

		if (LS_BF_successful)
		{	//Copy answer if successful into answer and return true!!
			double l = Cylinder1[3];
			double m = Cylinder1[4];
			double n = Cylinder1[5];
			double mod = sqrt(pow(l,2) + pow(m,2) + pow(n,2));
			l = l/mod; m = m/mod; n = n/mod;
			Cylinder1[3] = l;
			Cylinder1[4] = m;
			Cylinder1[5] = n;
			memcpy((void*)answer, (void*)&Cylinder1[0], 7 * sizeof(double));
			//Now shift the pt back to original coordinates
			for (int j = 0; j < 3; j ++) *(answer + j) += Centroid[j];

			if ((precision < 1000000) && (!remove_outliers))
			{
				double lsq_dist = 0;
				double sq_dist_from_pt = 0;
				double sq_axial_dist = 0;
				double sq_radial_dist = 0;
				for (int i = 0; i < PtsCount; i ++)
				{
					sq_dist_from_pt = pow (*(pts + 3*i + 0) - *(answer + 0), 2) + pow (*(pts + 3*i + 1) - *(answer + 1), 2)+ pow (*(pts + 3*i + 2) - *(answer + 2), 2);
					sq_axial_dist = pow (((*(pts + 3*i + 0) - *(answer + 0)) * *(answer + 3) + (*(pts + 3*i + 1) - *(answer + 1)) * *(answer + 4) + (*(pts + 3*i + 2) - *(answer + 2)) * *(answer + 5)),2);
					sq_radial_dist = sq_dist_from_pt - sq_axial_dist;
					lsq_dist += abs(sqrt(sq_radial_dist) - *(answer + 6));
				}
				*(answer + 7) = lsq_dist;
				if (lsq_dist > precision) 
					return false;
			}
			if (remove_outliers)
			{
				double err = 0; 
				for (i = 0; i < PtsCount * 3; i += 3)
				{	
					err += abs(hM->Point_Line_Dist_3D((pts + i), answer) - *(answer + 6));
				}
				err = 2 * err/PtsCount;
				double* ptscpy = (double*) malloc(3*PtsCount*sizeof(double));
				memset(ptscpy,0,3*PtsCount*sizeof(double));
				int jj = 0;
				for (i = 0; i < PtsCount * 3; i += 3)
				{
					if (abs(hM->Point_Line_Dist_3D((pts + i), answer) - *(answer + 6))< err)
					{
						*(ptscpy + jj) = *(pts + i); *(ptscpy + jj + 1) = *(pts + i + 1); *(ptscpy + jj + 2) = *(pts + i + 2);
						jj += 3;
					}
				}
				int ptscnt = jj/3;
				if (!Cylinder_BestFit(ptscpy, ptscnt, answer))
				{
					LS_BF_successful = false;
				}
				free(ptscpy);
			}
		}
		return LS_BF_successful;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0016", __FILE__, __FUNCSIG__);return false;}
}

//The pts will be in 3D
//Cone params = x,y,z (a pt on cone axis), l,m,n (direction cosines of cone axis), 
//s (radius of cone at perpendicular plane passing through x,y,z) ang (apex angle of cone) in the same order in the array
bool RBF::Cone_BestFit(double* pts, int PtsCount, double* answer, double precision)
{
	try
	{
		double Cone[9], Cone2[9], initGuess[8];
		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
		double Centroid[3];
		ZeroMemory(&Centroid[0], sizeof(double) * 3);
		ZeroMemory(&Cone[0], sizeof(double) * 9);
		ZeroMemory(&Cone2[0], sizeof(double) * 9);
		ZeroMemory(&initGuess[0], sizeof(double) * 8);
		int i;

		//memcpy((void*) localPts, (void*)pts, sizeof(double) * PtsCount * 3);
		for (i = 0; i < PtsCount * 3; i ++) *(localPts + i) = *(pts + i);

		if (!hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0])) 
		{	free(localPts); return false;
		}

		//Set rest of initial guess values

		double first3pts[9];
		double first3ptsCircle[7];

		for (i=0; i<9; i++) first3pts[i] = *(pts+i);

		//get plane direction cosines by passing first 3 points
		while (i <= PtsCount*3)
		{
			if (Circle_3Pt_3D(first3pts, first3ptsCircle)) 
			{	
				i = i - 1;
				break; 
			}
			if (i >= PtsCount*3) break;

			first3pts[6] = *(pts+i);
			first3pts[7] = *(pts+i+1);
			first3pts[8] = *(pts+i+2);
			i += 3;
		}

		//if we failed to get a plane with 3 initial points, return false
		if (i >= PtsCount*3)
		{
			free(localPts);
			return false;
		}

		initGuess[0] = first3ptsCircle[0] - Centroid[0]; initGuess[1] = first3ptsCircle[1] - Centroid[1]; initGuess[2] = first3ptsCircle[2] - Centroid[2];
		initGuess[3] = first3ptsCircle[3]; initGuess[4] = first3ptsCircle[4]; initGuess[5] = first3ptsCircle[5]; 
		initGuess[6] = first3ptsCircle[6];

		double last3pts[9];
		double last3ptsCircle[7];

		for (i=0; i<9; i++) last3pts[i] = *(pts+ 3*PtsCount - 9 + i);

		//get circle of last 3 points
		if (Circle_3Pt_3D(last3pts, last3ptsCircle)) 
		{	
			double d = sqrt(pow(last3ptsCircle[0] - first3ptsCircle[0],2) + pow(last3ptsCircle[1] - first3ptsCircle[1],2) + pow(last3ptsCircle[2] - first3ptsCircle[2],2));
			double r = abs(last3ptsCircle[6] - first3ptsCircle[6]);
			if (d != 0)
				initGuess[7] = atan(r/d);
		}
		if (initGuess[7] == 0)
			initGuess[7] = M_PI/4;
	
		//Do best-fit and check if successful
		bool LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 6, 8, -1, hM->Cone,true, 10000, Tolerance, &Cone[0]);

		initGuess[3] = -initGuess[3]; initGuess[4] = -initGuess[4]; initGuess[5] = -initGuess[5];

		bool LS_BF_successful2 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 6, 8, -1, hM->Cone,true, 10000, Tolerance, &Cone2[0]);

		double res_size = 1000000;
		double res_size2 = 1000000;

		if (LS_BF_successful)
		{
			res_size = *(Cone + 8);
		}

		if (LS_BF_successful2)
		{
			res_size2 = *(Cone2 + 8);
		}

		if (abs(res_size2) < abs(res_size))
		{
			res_size = res_size2;
			memcpy(Cone,Cone2,sizeof(double)*9);
		}

		if (answer[6]>0)
		{
			double Cone3[9] = {0};
			double Cone4[9] = {0};

			for (int i=0; i< 8; i++)
				initGuess[i] = answer[i];
			for (int i=0; i < 3; i++)
				initGuess[i] -= Centroid[i];
			//Do best-fit and check if successful
			bool LS_BF_successful3 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 6, 8, -1, hM->Cone,true, 10000, Tolerance, &Cone3[0]);

			initGuess[3] = -initGuess[3]; initGuess[4] = -initGuess[4]; initGuess[5] = -initGuess[5];

			bool LS_BF_successful4 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 6, 8, -1, hM->Cone,true, 10000, Tolerance, &Cone4[0]);

			double res_size3 = 1000000;
			double res_size4 = 1000000;

			if (LS_BF_successful3)
			{
				res_size3 = *(Cone3 + 8);
			}

			if (LS_BF_successful4)
			{
				res_size4 = *(Cone4 + 8);
			}

			if (abs(res_size4) < abs(res_size3))
			{
				res_size3 = res_size4;
				memcpy(Cone3,Cone4,sizeof(double)*9);
			}
			
			if (abs(res_size3) < abs(res_size))
			{
				res_size = res_size3;
				memcpy(Cone,Cone3,sizeof(double)*9);
			}
		}

		LS_BF_successful = false;

		if (res_size < 1000000)
		{	
			LS_BF_successful = true;
				//normalize output parameters
			double modA = sqrt(pow(*(Cone + 3), 2) + pow(*(Cone + 4), 2) + pow(*(Cone + 5), 2));
			double	a = *(Cone + 3) / modA;
			double	b = *(Cone + 4) / modA; 
			double	c = *(Cone + 5) / modA;
			*(Cone + 3) = a; *(Cone + 4) = b; *(Cone + 5) = c;

					//bring x,y,z to point closest to origin.
			double x = *Cone;
			double y = *(Cone + 1);
			double z = *(Cone + 2);
			double pt_dot_axis = x*a + y*b + z*c;

			x -= pt_dot_axis * a;
			y -= pt_dot_axis * b;
			z -= pt_dot_axis * c;

			*Cone = x;
			*(Cone + 1) = y;
			*(Cone + 2) = z;

					//normalize apex angle
			double ang = *(Cone + 7);

			ang -= int(ang/(M_PI*2.0));

			if (ang < 0 ) ang += M_PI * 2.0;
			if (ang > M_PI)
			{
				a = -a; b = -b; c = -c;
				*(Cone + 3) = a; *(Cone + 4) = b; *(Cone + 5) = c;
				ang -= M_PI;
			}
			if (ang > M_PI/2.0) 
			{
				a = -a; b = -b; c = -c;
				*(Cone + 3) = a; *(Cone + 4) = b; *(Cone + 5) = c;
				ang = M_PI - ang;
			}

			*(Cone + 7) = ang;

				//normalize base plane radius
			double s = *(Cone + 6);
			if (s < 0)
			{
				a = -a; b = -b; c = -c;
				*(Cone + 3) = a; *(Cone + 4) = b; *(Cone + 5) = c;
				s = -s;
			}
			*(Cone + 6) = s;
		
			//Copy answer if successful into answer and return true!!
			memcpy((void*)answer, (void*)&Cone[0], 8 * sizeof(double));
			//Now shift the pt back to original coordinates
			for (int j = 0; j < 3; j ++) *(answer + j) += Centroid[j];
		}
		free(localPts);
		if (LS_BF_successful)
		{
			if (precision < 1000000)
			{
				double lsq_dist = 0;
				double sq_dist_from_pt = 0;
				double axial_dist = 0;
				double sq_radial_dist = 0;
				double exp_radial_dist = 0;
				for (int i = 0; i < PtsCount; i ++)
				{
					sq_dist_from_pt = pow (*(pts + 3*i + 0) - *(answer + 0), 2) + pow (*(pts + 3*i + 1) - *(answer + 1), 2)+ pow (*(pts + 3*i + 2) - *(answer + 2), 2);
					axial_dist = (*(pts + 3*i + 0) - *(answer + 0)) * *(answer + 3) + (*(pts + 3*i + 1) - *(answer + 1)) * *(answer + 4) + (*(pts + 3*i + 2) - *(answer + 2)) * *(answer + 5);
					sq_radial_dist = sq_dist_from_pt - pow(axial_dist, 2);
					exp_radial_dist = *(answer + 6) + axial_dist * tan(*(answer + 7));
					lsq_dist += abs((sqrt(sq_radial_dist) - exp_radial_dist) * cos(*(answer + 7)));
				}
				*(answer + 8) = lsq_dist;
				if (lsq_dist > precision) 
					return false;
			}
		}
	
		return LS_BF_successful;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0017", __FILE__, __FUNCSIG__); return false;}
}

//The pts will be in 3D
//3D Circle params = x,y,z (center of circle), l,m,n (direction cosines of plane of circle), 
//r (radius of circle) in the same order in the array
bool RBF::ThreeDCircle_BestFit(double* pts, int PtsCount, double* answer)
{
	try
	{
		double ThreeDCircle[8], initGuess[7];
		ZeroMemory(&ThreeDCircle[0], sizeof(double) * 8);
		ZeroMemory(&initGuess[0], sizeof(double) * 7);
		double Min = 10000000;

		if (PtsCount == 3)
		{
			if (!Circle_3Pt_3D(pts, answer))
				return false;
			return true;
		}

		int gSize = (int)(PtsCount / 3);
		int TotalCircleCt = 1000; 
		int Stepper = (int) (PtsCount / 10);
		if (Stepper == 0) Stepper = 1;

		//Set one array of double which will hold answer successively of all the circles
		double* Answers = (double*) malloc(sizeof(double)* TotalCircleCt * 7);
		//One counter variable	
		int C_Ct = 0;
		double tP[9];

		//Calculate the circles that are required. 
		if (PtsCount < 10)
		{	for (int i = 0; i < PtsCount - 2; i += Stepper)
			{	tP[0] = *(pts + 3 * i); tP[1] = *(pts + 3 * i + 1); tP[2] = *(pts + 3 * i + 2);
				for (int j = i + 1; j < PtsCount - 1; j += Stepper)
				{	tP[3] = *(pts + 3 * j); tP[4] = *(pts + 3 * j + 1); tP[5] = *(pts + 3 * j + 2);
					for (int k = j + 1; k < PtsCount; k += Stepper)
					{	tP[6] = *(pts + 3 * k); tP[7] = *(pts + 3 * k + 1); tP[8] = *(pts + 3 * k + 2); 
						bool gotCircle = Circle_3Pt_3D(&tP[0], Answers + 7 * C_Ct);
						if (gotCircle) C_Ct ++;
						if (C_Ct >= TotalCircleCt) goto Analyse;
					}
				}
			}
		}
		else
		{	for (int i = 0; i < gSize; i += Stepper)
			{	tP[0] = *(pts + 3 * i); tP[1] = *(pts + 3 * i + 1); tP[2] = *(pts + 3 * i + 2);
				for (int j = gSize; j < 2 * gSize; j += Stepper)
				{	tP[3] = *(pts + 3 * j); tP[4] = *(pts + 3 * j + 1); tP[5] = *(pts + 3 * j + 2);
					for (int k = 2 * gSize; k < PtsCount; k += Stepper)
					{	tP[6] = *(pts + 3 * k); tP[7] = *(pts + 3 * k + 1); tP[8] = *(pts + 3 * k + 2); 
						bool gotCircle = Circle_3Pt_3D(&tP[0], Answers + 7 * C_Ct);
						if (gotCircle) C_Ct ++;
						if (C_Ct >= TotalCircleCt) goto Analyse;
					}
				}
			}
		}
		if (C_Ct == 0)
		{
			free(Answers);
			return false;
		}
		//Now, let us loop through for each circle, and find out the one with least error
		//First we need an array of double to hold the error values
	Analyse:	double* errorValues = (double*) malloc(sizeof(double)* C_Ct);
		int MinIndex = 0;
		for (int i = 0 ; i < C_Ct; i ++)
		{	*(errorValues + i) = 0;
			for (int j = 0; j < 3 * PtsCount; j += 3)
			{	*(errorValues + i) += abs(sqrt(pow((*(pts + j) - *(Answers + i * 7 + 0)), 2) + 
											pow((*(pts + j + 1) - *(Answers + i * 7 + 1)), 2) + 
											pow((*(pts + j + 2) - *(Answers + i * 7 + 2)), 2)) - 
											*(Answers + i * 7 + 6));
			}
			if (*(errorValues + i) < Min)
			{	Min = *(errorValues + i); MinIndex = i;
			}
		}

		//Copy the best circle to answer pointer location
		//index 0 = centre x ; 1 = centre y; 2 = radius
		for (int i = 0; i < 7; i ++)
		{	*(initGuess + i) = *(Answers + MinIndex * 7 + i);
		}
		free (Answers);
		free (errorValues);	
		//if (PtsCount > 10)
		//{
			//Do best-fit and check if successful
			bool LS_BF_successful = hM->LS_BF_LM(pts, &initGuess[0], PtsCount, 6, 7, -1, hM->ThreeDCircle,true, 10000, Tolerance, &ThreeDCircle[0]);
		
			double err = pow(Min,4);
			if ((!LS_BF_successful) || (err < ThreeDCircle[7]))
				memcpy(ThreeDCircle,initGuess,7*sizeof(double));

					//Normalize the answers. 
			double modA = sqrt(pow(*(ThreeDCircle + 3), 2) + pow(*(ThreeDCircle + 4), 2) + pow(*(ThreeDCircle + 5), 2));
			double a = *(ThreeDCircle + 3) / modA;
			double b = *(ThreeDCircle + 4) / modA; 
			double c = *(ThreeDCircle + 5) / modA;

			*(ThreeDCircle + 3) = a; *(ThreeDCircle + 4) = b; *(ThreeDCircle + 5) = c;
			//Copy answer if successful into answer and return true!!
			memcpy((void*)answer, (void*)&ThreeDCircle[0], 7 * sizeof(double));
		//}
		//else
		//	memcpy((void*)answer, (void*)initGuess, 7 * sizeof(double));

		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0018", __FILE__, __FUNCSIG__); return false;}
}

//answer will be an array with 14 parameters.  
//The first 4 will be the best fit plane parameters and the remaining 10 will be from the ThreeDConic:
//first 3 denoting coordinates of focus lying on best fit plane, 
//next 3 the dir cosines of diretrix on the best fit plane,
//7th, 8th and 9th will be a point on diretrix closest to focus, 
//10th the eccentricity of conic.
//TwoDConic would require only 6 parameters to define: first 2 denote 2D coordinates of focus on best fit plane,
//third denotes 2D slope of diretrix on the best fit plane,
//fourth and fifth the 2D point on diretrix closest to focus, sixth the eccentricity.

bool RBF::ThreeDConic_BestFit(double* pts, int PtsCount, double* answer, Conic ConicShape)
{
	try
	{
		int i = 0;
		double prevAnswer[14] = {0};
		double ThreeDConic[10];
		double TwoDConic[7];
		double TwoDConic2[7];
		double InitGuess[6];
		double Centroid[3];
		ZeroMemory(&Centroid[0], sizeof(double) * 3);
		ZeroMemory(&ThreeDConic[0], sizeof(double) * 10);
		ZeroMemory(&TwoDConic[0], sizeof(double) * 7);
		ZeroMemory(&TwoDConic2[0], sizeof(double) * 7);
		ZeroMemory(&InitGuess[0], sizeof(double) * 6);

		memcpy(prevAnswer,answer,sizeof(double) * 14);

		if (!Plane_BestFit (pts, PtsCount, answer, false))
			return false;
	
		// unit vector along perpendicular to plane
		double l1 = *answer;
		double m1 = *(answer + 1);
		double n1 = *(answer + 2);

		//get max dir cosine of perpendicular to plane for checking right-handedness of coord system determined by focus, diretrix and perpendicular.
		double max_dir = l1;
		if (abs(m1) > abs(max_dir))
			max_dir = m1;
		if (abs(n1) > abs(max_dir))
			max_dir = n1;

		//mutually perpendicular unit vectors on plane
		double l2, m2, n2, l3, m3, n3;

		if (l1 != 0)
		{
			l2 = m1 / sqrt (pow (l1, 2) + pow (m1, 2));
			m2 = - l1 / sqrt (pow (l1, 2) + pow (m1, 2));
		}
		else
		{
			l2 = 1;
			m2 = 0;
		}
		n2 = 0;

		l3 = m1 * n2 - m2 * n1;
		m3 = n1 * l2 - l1 * n2;
		n3 = l1 * m2 - l2 * m1;

		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
		memset(localPts, 0, sizeof(double) * PtsCount * 3);
		//convert to local points with z coordinate as zero and x, y along the l2,m2,n2 and l3,m3,n3 directions
		for ( i = 0; i < PtsCount ; i ++) 
		{
			*(localPts + 3 * i) = *(pts + 3 * i) * l2 + *(pts + 3 * i + 1) * m2 + *(pts + 3 * i + 2) * n2;
			*(localPts + 3 * i + 1) = *(pts + 3 * i) * l3 + *(pts + 3 * i + 1) * m3 + *(pts + 3 * i + 2) * n3;
			*(localPts + 3 * i + 2) = 0;
		}

		if (!hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0])) 
		{	free(localPts); return false;
		}
		//now initial guess parameters are calculated

		//center_pt is the center of list of points for which we are preparing best fit conic.
		double center_pt_x = *(localPts + 3 * int(PtsCount/2));
		double center_pt_y = *(localPts + 3 * int(PtsCount/2) + 1);

		double slope = 0;
	
		//first calculate slope of line joining focus and center point.
		if (center_pt_x !=0) slope = atan (center_pt_y / center_pt_x);
		else slope = M_PI / 2.0;

		//slope of diretrix would be perpendicular to above line.
		slope += M_PI / 2.0;
		slope -= int((slope / M_PI) + 0.5 ) * M_PI ;

		//first two parameters of initial guess are zero because we have shifted conic around centroid.
		InitGuess[2] = slope;
		InitGuess[3] = 2 * center_pt_x;
		InitGuess[4] = 2 * center_pt_y;
		bool constrainallparams = true;
		switch (ConicShape)
		{
		case Ellipse:
			InitGuess[5] = 0.5;
			break;
		case Parabola:
			InitGuess[5] = 1.0;
			constrainallparams = false;
			break;
		case Hyperbola:
			InitGuess[5] = 1.5;
			break;
		}

		bool LS_BF_successful, LS_BF_successful2;
		double res1 = -1, res2 = -1;
		//Do best-fit and check if successful
		if (constrainallparams)
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &InitGuess[0], PtsCount, 5, 6, -1, hM->TwoDConic, true, 10000, Tolerance, &TwoDConic[0]);
			if (LS_BF_successful)
				res1 = TwoDConic[6];
		}
		else
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &InitGuess[0], PtsCount, 2, 5, -1, hM->TwoDParabola,true, 10000, Tolerance, &TwoDConic[0]);
			if (LS_BF_successful)
				res1 = TwoDConic[5];
			TwoDConic[5] = 1;
		}
		if ((prevAnswer[0] != 0) || (prevAnswer[1] != 0) || (prevAnswer[2] != 0))
		{
			InitGuess[0] = prevAnswer[4] * l2 + prevAnswer[5] * m2 + prevAnswer[6] * n2 - Centroid[0];
			InitGuess[1] = prevAnswer[4] * l3 + prevAnswer[5] * m3 + prevAnswer[6] * n3 - Centroid[1];
			double c = prevAnswer[7] * l2 + prevAnswer[8] * m2 + prevAnswer[9] * n2;
			double s = prevAnswer[7] * l3 + prevAnswer[8] * m3 + prevAnswer[9] * n3;
			InitGuess[2] = acos(c);
			if (s < 0)
				InitGuess[2] = M_PI - InitGuess[2];
			InitGuess[3] = prevAnswer[10] * l2 + prevAnswer[11] * m2 + prevAnswer[12] * n2 - Centroid[0];
			InitGuess[4] = prevAnswer[10] * l3 + prevAnswer[11] * m3 + prevAnswer[12] * n3 - Centroid[1];
			InitGuess[5] = prevAnswer[13];
			//Do best-fit and check if successful
			if (constrainallparams)
			{
				LS_BF_successful2 = hM->LS_BF_LM(localPts, &InitGuess[0], PtsCount, 5, 6, -1, hM->TwoDConic, true, 10000, Tolerance, &TwoDConic2[0]);
				if (LS_BF_successful2)
					res2 = TwoDConic2[6];
			}
			else
			{
				LS_BF_successful2 = hM->LS_BF_LM(localPts, &InitGuess[0], PtsCount, 2, 5, -1, hM->TwoDParabola,true, 10000, Tolerance, &TwoDConic2[0]);
				if (LS_BF_successful2)
					res2 = TwoDConic2[5];
				TwoDConic2[5] = 1;
			}
			if (((res1 == -1) && (res2 != -1)) ||
				((res1 != -1) && (res2 != -1) && (res2 < res1)))
			{
				LS_BF_successful = true;
				memcpy(TwoDConic,TwoDConic2,sizeof(double)*7);
			}
		}
		if (LS_BF_successful)
		{
			//shift focal point and diretrix back to orig coord system
			TwoDConic[0] += Centroid[0];
			TwoDConic[1] += Centroid[1];

			TwoDConic[3] += Centroid[0];
			TwoDConic[4] += Centroid[1];

			//3d Conic focal point
			ThreeDConic[0] = TwoDConic[0] * l2 + TwoDConic[1] * l3;
			ThreeDConic[1] = TwoDConic[0] * m2 + TwoDConic[1] * m3;
			ThreeDConic[2] = TwoDConic[0] * n2 + TwoDConic[1] * n3;

			//3d conic diretrix direction cosines
			TwoDConic[2] -= int((TwoDConic[2] / M_PI) + 0.5 ) * M_PI ;

			double d = (TwoDConic[0] - TwoDConic[3]) * cos (TwoDConic[2]) + (TwoDConic[1] - TwoDConic[4]) * sin (TwoDConic[2]);
			TwoDConic[3] += cos (TwoDConic[2]) * d;
			TwoDConic[4] += sin (TwoDConic[2]) * d;

			ThreeDConic[3] = l2 * cos (TwoDConic[2]) + l3 * sin (TwoDConic[2]);
			ThreeDConic[4] = m2 * cos (TwoDConic[2]) + m3 * sin (TwoDConic[2]);
			ThreeDConic[5] = n2 * cos (TwoDConic[2]) + n3 * sin (TwoDConic[2]);

			//3d conic point on diretrix closest to focus
			ThreeDConic[6] = TwoDConic[3] * l2 + TwoDConic[4] * l3;
			ThreeDConic[7] = TwoDConic[3] * m2 + TwoDConic[4] * m3;
			ThreeDConic[8] = TwoDConic[3] * n2 + TwoDConic[4] * n3;

			//3d conic eccentricity
			ThreeDConic[9] = TwoDConic[5];

			//3d conic ray from diretrix to focus
			double ray[3] = {ThreeDConic[0] - ThreeDConic[6], ThreeDConic[1] - ThreeDConic[7], ThreeDConic[2] - ThreeDConic[8]};
			double norm = sqrt (ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]);
			//normalized ray
			for (int i=0; i < 3; i++)
				ray[i] = ray[i] / norm;
			//find cross product of ray with diretrix
			double dir2[3];
			dir2[0] = ray[1] * ThreeDConic[5] - ray[2] * ThreeDConic[4];
			dir2[1] = ray[2] * ThreeDConic[3] - ray[0] * ThreeDConic[5];
			dir2[2] = ray[0] * ThreeDConic[4] - ray[1] * ThreeDConic[3];

			//get max dir cosine of perpendicular to plane
			double max_dir2 = dir2[0];
			if (abs(dir2[1]) > abs(max_dir2))
				max_dir2 = dir2[1];
			if (abs(dir2[2]) > abs(max_dir2))
				max_dir2 = dir2[2];

			//compare relative sign of right-handed perpendicular with original perpendicular to plane
			//and change sign accordingly
			if (max_dir2/max_dir < 0)
			{
				for (int i=0; i < 4; i++)
					*(answer + i) = - *(answer + i);
			}

			//move focus and diretrix point along normal to plane so that they fall on plane of conic
			ThreeDConic[0] -= *(answer + 0) * *(answer + 3);
			ThreeDConic[1] -= *(answer + 1) * *(answer + 3);
			ThreeDConic[2] -= *(answer + 2) * *(answer + 3);

			ThreeDConic[6] -= *(answer + 0) * *(answer + 3);
			ThreeDConic[7] -= *(answer + 1) * *(answer + 3);
			ThreeDConic[8] -= *(answer + 2) * *(answer + 3);

			memcpy ( (void*) (answer + 4), (void*) &ThreeDConic[0], sizeof(double) * 10);
		}

		free(localPts);
		return LS_BF_successful;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0019", __FILE__, __FUNCSIG__); return false;}
}

//answer will be an array with 7 parameters.  
//first 3 denoting coordinates of focus, 
//next 3 will be a point on diretrix plane closest to focus, 
//last would be the eccentricity of conicoid.
bool RBF::Conicoid_BestFit(double* pts, int PtsCount, double* answer, Conic ConicShape)
{
	try
	{
		double Conicoid[8];
		double InitGuess[7];
		double Centroid[3];
		ZeroMemory(&Centroid[0], sizeof(double) * 3);
		ZeroMemory(&Conicoid[0], sizeof(double) * 8);
		ZeroMemory(&InitGuess[0], sizeof(double) * 7);

		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
		memset(localPts, 0, sizeof(double) * PtsCount * 3);
		for (int i = 0; i < PtsCount * 3; i ++) *(localPts + i) = *(pts + i);

		if (!hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0])) 
		{	free(localPts); return false;
		}
		//now initial guess parameters are calculated
	
		//center_pt is the center of list of points (in order in which passed) for which we are preparing best fit conicoid.
		double center_pt_x = *(localPts + 3 * int(PtsCount/2));
		double center_pt_y = *(localPts + 3 * int(PtsCount/2) + 1);
		double center_pt_z = *(localPts + 3 * int(PtsCount/2) + 2);

		bool constrainallparams = true;
		switch (ConicShape)
		{
		case Ellipse:
			InitGuess[0] = 0.5 * center_pt_x; InitGuess[1] = 0.5 * center_pt_y; InitGuess[2] = 0.5 * center_pt_z;
			InitGuess[3] = 2 * center_pt_x; InitGuess[4] = 2 * center_pt_y; InitGuess[5] = 2 * center_pt_z;
			InitGuess[6] = 0.5;
			break;
		case Parabola:
			InitGuess[0] = 0.5 * center_pt_x; InitGuess[1] = 0.5 * center_pt_y; InitGuess[2] = 0.5 * center_pt_z;
			InitGuess[3] = 1.5 * center_pt_x; InitGuess[4] = 1.5 * center_pt_y; InitGuess[5] = 1.5 * center_pt_z;
			InitGuess[6] = 1.0;
			constrainallparams = false;
			break;
		case Hyperbola:
			InitGuess[0] = 0.5 * center_pt_x; InitGuess[1] = 0.5 * center_pt_y; InitGuess[2] = 0.5 * center_pt_z;
			InitGuess[3] = 1.33 * center_pt_x; InitGuess[4] = 1.33 * center_pt_y; InitGuess[5] = 1.33 * center_pt_z;
			InitGuess[6] = 1.5;
			break;
		}

		bool LS_BF_successful;

		//Do best-fit and check if successful
		if (constrainallparams)
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &InitGuess[0], PtsCount, 6, 7, -1, hM->Conicoid, true, 10000, Tolerance, &Conicoid[0]);
		}
		else
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &InitGuess[0], PtsCount, 0, 6, -1, hM->Paraboloid,true, 10000, Tolerance, &Conicoid[0]);
			Conicoid[6] = 1;
		}
		if (LS_BF_successful)
		{
			memcpy((void*)answer, (void*)&Conicoid[0], 10 * sizeof(double));
			//Now shift to original coordinates
			for (int j = 0; j < 3; j ++) 
			{
				*(answer + j) += Centroid[j];
				*(answer + 3 + j) += Centroid[j];
			}
		}
		free(localPts);
		return LS_BF_successful;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0020", __FILE__, __FUNCSIG__); return false;}
}

//answer will be array of 8 parameters: first 3 will be the projection of center of the smaller arc/circle that rotates around an axis on the axis.
//next 3 will be the dir cosines of the axis.  7th is the distance of center of arc from axis (the larger circle).  8th is the radius of the arc.
bool RBF::Torus_BestFit(double* pts, int PtsCount, double* answer, double* IdealRadius, double* CentreCoods)
{
	try
	{
		double torus[9], torus2[9], torus3[9], initGuess[8];
		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 3);
		double Centroid[3];
		int SuccessfulTrialNumber = 0;
		ZeroMemory(&Centroid[0], sizeof(double) * 3);
		ZeroMemory(&torus[0], sizeof(double) * 9);
		ZeroMemory(&torus2[0], sizeof(double) * 9);
		ZeroMemory(&torus3[0], sizeof(double) * 9);
		ZeroMemory(&initGuess[0], sizeof(double) * 8);
		int i;

		for (i = 0; i < PtsCount * 3; i ++) *(localPts + i) = *(pts + i);

		if (!hM->ShiftaroundCentroid_3D(localPts, PtsCount, &Centroid[0])) 
		{	
			free(localPts); return false;
		}

		//Set rest of initial guess values
		double circle3d[7] = {0};

		if (!ThreeDCircle_BestFit(localPts, PtsCount, circle3d))
		{
			free(localPts);
			return false;
		}
		memcpy(initGuess,circle3d,sizeof(double)*7);
		initGuess[7] = circle3d[6]/2;
		
		double res1 = -1, res2 = -1, res3 = -1;
		//Do best-fit and check if successful
		bool LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 7, 8, -1, hM->Torus,true, 10000, Tolerance, &torus[0]);
		initGuess[4] = - initGuess[4];

		bool LS_BF_successful2 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 7, 8, -1, hM->Torus,true, 10000, Tolerance, &torus2[0]);
				
		if (!LS_BF_successful || LS_BF_successful2)
		{	if(torus[8] > torus2[8])
			{	
				SuccessfulTrialNumber = 2;
				memcpy(torus,torus2,sizeof(double)*9);
			}
			else
				SuccessfulTrialNumber = 1;
		}
		initGuess[5] = - initGuess[5]; LS_BF_successful2 = false;
		LS_BF_successful2 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 7, 8, -1, hM->Torus,true, 10000, Tolerance, &torus2[0]);
		if (LS_BF_successful2 && torus[8] > torus2[8])
		{	
			SuccessfulTrialNumber = 3;
			memcpy(torus,torus2,sizeof(double)*9);
		}
		//R A N - Include a better initial guess to get better best-fitting, and hopefully not get stuck in local minima!
		//User will give ideal radii he wants to see. if no ideal radius is given, then we use Circle3D values above. 
		if (IdealRadius[0] != 0) initGuess[6] = IdealRadius[0];
		if (IdealRadius[1] != 0) initGuess[7] = IdealRadius[7];
		if (CentreCoods[0] != 0 && CentreCoods[1] != 0 && CentreCoods[2] != 0)
			memcpy(initGuess, CentreCoods, sizeof(double) * 3);
		// Direction Cosines - always try out donut on the XY. Hence the dc will be 001. We will then try 010 and then 100. 
		//1st torus axis parallel to Z-axis
		initGuess[3] = 0; initGuess[4] = 0; initGuess[5] = 1; LS_BF_successful2 = false;
		LS_BF_successful2 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 7, 8, -1, hM->Torus,true, 10000, Tolerance, &torus2[0]);
		if (LS_BF_successful2 && torus[8] > torus2[8])
		{	
			SuccessfulTrialNumber = 4;
			memcpy(torus,torus2,sizeof(double)*9);
		}
		// Y axis parallel torus
		initGuess[3] = 0; initGuess[4] = 1; initGuess[5] = 0; LS_BF_successful2 = false;
		LS_BF_successful2 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 7, 8, -1, hM->Torus,true, 10000, Tolerance, &torus2[0]);
		if (LS_BF_successful2 && torus[8] > torus2[8])
		{	
			SuccessfulTrialNumber = 5;
			memcpy(torus,torus2,sizeof(double)*9);
		}
		// X axis parallel torus
		initGuess[3] = 0; initGuess[4] = 1; initGuess[5] = 0; LS_BF_successful2 = false;
		LS_BF_successful2 = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 7, 8, -1, hM->Torus,true, 10000, Tolerance, &torus2[0]);
		if (LS_BF_successful2 && torus[8] > torus2[8])
		{	
			SuccessfulTrialNumber = 6;
			memcpy(torus,torus2,sizeof(double)*9);
		}

		//if (LS_BF_successful)
		//	res1 = torus[8];
		//if (LS_BF_successful2)
		//	res2 = torus2[8];
		//if (LS_BF_successful3)
		//	res3 = torus3[8];
		//if ((res2 > -1) && (res2 < res1))
		//{
		//	memcpy(torus, torus2, 9 * sizeof(double));
		//	LS_BF_successful = true;
		//	res1 = res2;
		//}
		//if ((res3 > -1) && (res3 < res1))
		//{
		//	memcpy(torus, torus3, 9 * sizeof(double));
		//	LS_BF_successful = true;
		//	res1 = res3;
		//}
		if (SuccessfulTrialNumber > 0)
		{	
			//normalize dir cosines
			double modA = sqrt(pow(*(torus + 3), 2) + pow(*(torus + 4), 2) + pow(*(torus + 5), 2));
			double	a = *(torus + 3) / modA;
			double	b = *(torus + 4) / modA; 
			double	c = *(torus + 5) / modA;
			*(torus + 3) = a; *(torus + 4) = b; *(torus + 5) = c;
			if (torus[6] < 0)
				torus[6] = - torus[6];
			if (torus[7] < 0)
				torus[7] = - torus[7];

			//Copy answer if successful into answer and return true
			memcpy((void*)answer, (void*)&torus[0], 8 * sizeof(double));
			//Now shift the pt back to original coordinates
			for (int j = 0; j < 3; j ++) *(answer + j) += Centroid[j];
		}
		free(localPts);
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0021", __FILE__, __FUNCSIG__);
		return false;
	}
}
//this is a two dimentional section of a torus.  the answer will be array of 5 parameters: first 2 will be the projection of center of the smaller arc/circle 
//on the reflection axis.  Third will be the slope of the reflection axis.  4th is the distance of center of arc from axis (the larger circle).  
//5th is the radius of the smaller arc/circle.
//if axis_given is true, we need to pass answer[0], answer[1] and answer[2] to the function.
bool RBF::TwoArc_BestFit(double* pts, int PtsCount, double* answer, bool axis_given)
{
	try
	{
		double TwoArc[5], initGuess[5];
		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 2);
		double* Cluster1 = (double*) malloc(sizeof(double) * PtsCount * 2);
		double* Cluster2 = (double*) malloc(sizeof(double) * PtsCount * 2);
		double Centroid[2];
		ZeroMemory(&Centroid[0], sizeof(double) * 2);
		ZeroMemory(&TwoArc[0], sizeof(double) * 5);
		ZeroMemory(&initGuess[0], sizeof(double) * 5);
		int i;
		double maxdev = 0.05;

		if (axis_given == true)
		{
			if (answer[2] < 0)
				answer[2] += 2*M_PI;
			else if (answer[2] >= 2*M_PI)
				answer[2] -= 2*M_PI;
			if (answer[2] >= M_PI)
				answer[2] -= M_PI;
			for (i = 0; i < PtsCount; i ++) 
			{
				*(localPts + 2*i + 0) = (*(pts + 2*i + 0) - answer[0]) * cos (answer[2]) + (*(pts + 2*i + 1) - answer[1]) * sin (answer[2]);
				*(localPts + 2*i + 1) = (*(pts + 2*i + 1) - answer[1]) * cos (answer[2]) - (*(pts + 2*i + 0) - answer[0]) * sin (answer[2]);
			}
			double circle1[3] = {0};

			if (PtsCount > 50)
			{
				//get best fit circle of first 20 points to determine initguess values
				if (!Circle_BestFit(localPts, 20, circle1, true))
				{
					free(localPts); free(Cluster1); free(Cluster2);
					return false;
				}
				//initGuess[0] gives the x-axis value of center in rotated coord frame where axis is along x-axis.
				initGuess[0] = circle1[0];
				//initGuess[1] and initGuess[2] are the outer/inner radii
				initGuess[1] = circle1[1];
				if (initGuess[1] < 0)
					initGuess[1] *= -1;
				initGuess[2] = circle1[2];
			}
			else
			{
				initGuess[0] = 0;
				initGuess[1] = 2;
				initGuess[2] = 1;
			}
		}
		else
		{
			//The conversion of 3D points from the points array to localpts array was wrong. Fixed here. R A N 26 Jul 2019.
			for (i = 0; i < PtsCount; i++)
			{
				localPts[2 * i] = pts[3 * i];
				localPts[2 * i + 1] = pts[3 * i + 1];
			}

			//Next, we now do the initial guess by getting the linear regression line as the partition line for the two clusters of arc points
			double Sx = 0, Sxx = 0, Sy = 0, Sxy = 0;

			for (i = 0; i < PtsCount; i++)
			{
				Sx += localPts[2 * i];
				Sy += localPts[2 * i + 1];
			}
			Sx /= PtsCount; Sy /= PtsCount;

			for (i = 0; i < PtsCount; i++)
			{
				Sxx += (localPts[2 * i] - Sx) * (localPts[2 * i] - Sx);
				Sxy += (localPts[2 * i] - Sx) * (localPts[2 * i + 1] - Sy);
			}
			//double eVal[2] = { 0,0 }, eAngle[2] = { 0,0 };
			//bool gotEigen = hM->Get_Eigen_2D(localPts, PtsCount, eVal, eAngle);
			double partitionLine[2] = { 0,0 };
			//Get the linear regression line. This will be a good way to partition the points into 2 clusters! 
			//Slope  = (n * Sxy - Sx * Sy) / (n * Sxx - Sx * Sx)
			partitionLine[0] = Sxy / Sxx; // ((PtsCount * Sxy - Sx * Sy) / (PtsCount * Sxx - Sx * Sx));
			partitionLine[1] = (Sy - partitionLine[0] * Sx) / PtsCount;
			//Now we partition all points on either side of this line
			int c1 = 0, c2 = 0;
			for (i = 0; i < PtsCount; i++)
			{
				if (localPts[2 * i] * partitionLine[0] + partitionLine[1] >= localPts[2 * i + 1])
				{
					Cluster1[c1++] = localPts[2 * i];
					Cluster1[c1++] = localPts[2 * i + 1];
				}
				else
				{
					Cluster2[c2++] = localPts[2 * i];
					Cluster2[c2++] = localPts[2 * i + 1];
				}
			}

			initGuess[0] = 0;
			initGuess[1] = 0;
			double circle1[3] = { 0 };
			double circle2[3] = { 0 };
			int cnt = 0;

			//We got two clusters now. Check if each cluster has at least 3 points. If not, we dont have a proper sample of points to do TwoArc!
			c1 /= 2; c2 /= 2;
			bool gotCircles[2] = { false, false };
			if (c1 > 2)
				gotCircles[0] = Circle_BestFit(Cluster1, c1, circle1, true);
			if (c2 > 2)
				gotCircles[1] = Circle_BestFit(Cluster2, c2, circle2, true);
			//Check if we did not get either circle. In that case, exit the best fit
			if (!gotCircles[0] || !gotCircles[1])
			{
				free(localPts); free(Cluster1); free(Cluster2);
				return false;
			}

			//--------------------MOdified by R A Narayanan
			//First get the circle of the first 3 points - its most likely that we take these points 
			//on one side before going to the next
			//if (Circle_BestFit(localPts, 3, circle1, true))
			//{	double dev = 0;
			//	int j = 0; 
			//	int c1 = 3, c2 = 0;
			//	cnt = 1;
			//	memcpy(Cluster1, localPts, 6 * sizeof(double));
			//	//if (PtsCount > 20) cnt = int(PtsCount / 20);
			//	for (j = 3; j < PtsCount; j += cnt)
			//	{	//Calculate the deviation of the rest of the points from this circle, add them to a cluster.
			//		dev = abs(sqrt(pow(localPts[2*j] - circle1[0],2) + pow(localPts[2*j + 1] - circle1[1],2)) - circle1[2]);
			//		if (dev < maxdev) // if within a small tolerance of this circle, add to the cluster
			//		{	Cluster1[2 * c1] = localPts[2 * j];
			//			Cluster1[2 * c1 + 1] = localPts[2 * j + 1];
			//			c1 ++; cnt ++;
			//			//Once we add a certain # of points, lets recalculate the circle and update so we get better classification
			//			if (cnt > 10) 
			//			{	Circle_BestFit(Cluster1, c1, circle1, true); cnt = 0;
			//			}					
			//		}
			//		else
			//		{	Cluster2[2 * c2] = localPts[2 * j];
			//			Cluster2[2 * c2 + 1] = localPts[2 * j + 1];
			//			c2 ++;
			//		}
			//	}
			//	//We have "two" clusters of points. Now let us take the bf circle of this cluster, and the rest of the points
			//	if (c2 > 2)	Circle_BestFit(Cluster2, c2, circle2, true);
				//We got initial circle locations. Now let us put into initguess


			if (!hM->ShiftaroundCentroid_2D(localPts, PtsCount, &Centroid[0]))
			{
				free(localPts); free(Cluster1); free(Cluster2);
				return false;
			}
			circle1[0] -= Centroid[0];
			circle1[1] -= Centroid[1];
			circle2[0] -= Centroid[0];
			circle2[1] -= Centroid[1];

			initGuess[0] = (circle1[0] + circle2[0]) / 2;
			initGuess[1] = (circle1[1] + circle2[1]) / 2;
			//The initial slope of the reflection line. it should be perpendicular to the line joining the two centres.
			if (circle2[0] == circle1[0]) //the reflection line is parallel to the Y-axis
			{
				initGuess[2] = 0;
			}
			else
			{
				initGuess[2] = atan((circle2[1] - circle1[1]) / (circle2[0] - circle1[0])) + M_PI_2;
				if (initGuess[2] > M_PI)	initGuess[2] -= M_PI;
			}
			initGuess[3] = sqrt(pow(circle1[0] - circle2[0], 2) + pow(circle1[1] - circle2[1], 2)) / 2;
			initGuess[4] = (circle1[2] + circle2[2]) / 2;

			//}
		//---------------End Modification
			//if (PtsCount > 50)
			//{
			//	//get best fit circle of first 20 points and subsequent 20 points which deviate from this best fit circle to determine initguess values
			//	if (!Circle_BestFit(localPts, 20, circle1, true))
			//	{
			//		free(localPts);
			//		return false;
			//	}
			//	double dev = 0;
			//	int j = 0;
			//	for (j = 20; j < PtsCount; j++)
			//	{
			//		dev = abs(sqrt(pow(localPts[2*j] - circle1[0],2) + pow(localPts[2*j + 1] - circle1[1],2)) - circle1[2]);
			//		if (dev > maxdev)
			//		{
			//			cnt++;
			//			if (cnt >= 20)
			//				break;
			//		}
			//		else
			//			cnt = 0;
			//	}
			//	if (j == PtsCount)
			//	{
			//		circle2[0] = circle1[0]; 
			//		circle2[1] = circle1[1];
			//	}
			//	else
			//	{
			//		if (!Circle_BestFit(localPts + 2*(j - 20), 20, circle2, true))
			//		{
			//			free(localPts);
			//			return false;
			//		}
			//	}
			//	if (circle1[0] == circle2[0])
			//		initGuess[2] = 0;
			//	else
			//	{
			//		initGuess[2] = atan((circle2[1] - circle1[1])/(circle2[0] - circle1[0])) + M_PI_2;
			//		if (initGuess[2] > M_PI)
			//			initGuess[2] -= M_PI;
			//	}
			//	initGuess[0] = (circle1[0] + circle2[0]) / 2;
			//	initGuess[1] = (circle1[1] + circle2[1]) / 2;
			//	initGuess[3] = sqrt(pow(circle1[0] - circle2[0],2) + pow(circle1[1] - circle2[1],2))/2;
			//	initGuess[4] = (circle1[2] + circle2[2])/2;
			//}
			//else
			//{
			//	initGuess[2] = M_PI_2;	
			//	initGuess[3] = 2;
			//	initGuess[4] = 1;
			//}
		}
	
		//Do best-fit and check if successful
		bool LS_BF_successful = false;
		if (axis_given == true)
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 2, 3, -1, hM->TwoArc_AxisGiven,true, 10000, Tolerance, &TwoArc[0]);
		}
		else
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 4, 5, -1, hM->TwoArc,true, 10000, Tolerance, &TwoArc[0]);
		}

		if (LS_BF_successful)
		{	
			if (axis_given == true)
			{
				answer[0] += TwoArc[0] * cos (answer[2]);
				answer[1] += TwoArc[0] * sin (answer[2]);
				if (TwoArc[1] < 0)
					TwoArc[1] = -TwoArc[1];
				if (TwoArc[2] < 0)
					TwoArc[2] = -TwoArc[2];
				answer[3] = TwoArc[1];
				answer[4] = TwoArc[2];
			}
			else
			{
				double slope = TwoArc[2];

				if (slope < 0)
					slope += 2*M_PI;
				slope -= int((slope / M_PI) + 0.5 ) * M_PI ;
				TwoArc[2] = slope;

				if (TwoArc[3] < 0)
					TwoArc[3] = -TwoArc[3];
				if (TwoArc[4] < 0)
					TwoArc[4] = -TwoArc[4];

				//Copy answer if successful into answer and return true
				memcpy((void*)answer, (void*)&TwoArc[0], 5 * sizeof(double));
				//Now shift the pt back to original coordinates
				for (int j = 0; j < 2; j ++) *(answer + j) += Centroid[j];
			}
		}
		free(localPts); free(Cluster1); free(Cluster2);
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0021", __FILE__, __FUNCSIG__);
		return false;
	}
}

//the answer will be array of 6 parameters: first 3 will be a point on first line closest to origin and slope of first line 
//next three will be a point on second line closest to origin and slope of second line. 
//if axis_given is true, we need to pass a point on axis and slope of axis as first three parameters of answer.
bool RBF::TwoLine_BestFit(double* pts, int PtsCount, double* answer, bool axis_given)
{
	try
	{
		double TwoLine[6], initGuess[6];
		double* localPts = (double*) malloc(sizeof(double) * PtsCount * 2);
		double Centroid[2];
		ZeroMemory(&Centroid[0], sizeof(double) * 2);
		ZeroMemory(&TwoLine[0], sizeof(double) * 6);
		ZeroMemory(&initGuess[0], sizeof(double) * 6);
		int i;
		double AxisSlope = 0;
		double AxisPt[2] = {0};
		double maxdev = 0.05;


		if (axis_given == true)
		{
			AxisSlope = answer[2];
			AxisPt[0] = answer[0];
			AxisPt[1] = answer[1];
			for (i = 0; i < PtsCount; i ++) 
			{
				*(localPts + 2*i + 0) = (*(pts + 2*i + 0) - AxisPt[0]) * cos (AxisSlope) + (*(pts + 2*i + 1) - AxisPt[1]) * sin (AxisSlope);
				*(localPts + 2*i + 1) = (*(pts + 2*i + 1) - AxisPt[1]) * cos (AxisSlope) - (*(pts + 2*i + 0) - AxisPt[0]) * sin (AxisSlope);
			}
			double line1[2] = {0};
			if (PtsCount > 50)
			{
				//get best fit line of first 20 points to determine initguess values initGuess[0] and initGuess[1];
				if (!Line_BestFit_2D(localPts, 20, line1, true))
				{
					free(localPts);
					return false;
				}
				if (line1[0] < 0)
					line1[0] += 2 * M_PI;
				if (line1[0] >= M_PI)
					line1[0] -= M_PI;
				//initGuess[0] is signed distance of line pair from origin in rotated coordinate frame.  initGuess[1] is angle made by lines with axis on either side.
				if (line1[0] == M_PI_2)
					initGuess[0] = line1[1];
				else
					initGuess[0] = -line1[1] * cos (line1[0]);
				initGuess[1] = line1[0];
			}
			else
			{
				Pt tmppt;
				PtsList tempptslist;
				for (int i = 0; i < PtsCount; i++)
				{
					tmppt.x = *(localPts + 2 * i); tmppt.y = *(localPts + 2 * i + 1); tmppt.pt_index = i;
					tempptslist.push_back(tmppt);
				}
				orderpoints(&tempptslist);
				PtsList::iterator iter = tempptslist.begin();
				double tmppts[4] = {0};
				tmppts[0] = iter->x;
				tmppts[1] = iter->y;
				iter++;
				tmppts[2] = iter->x;
				tmppts[3] = iter->y;
				tempptslist.clear();
				if (!Line_BestFit_2D(tmppts, 2, line1, false))
				{
					free(localPts);
					return false;
				}
				if (line1[0] < 0)
					line1[0] += 2 * M_PI;
				if (line1[0] >= M_PI)
					line1[0] -= M_PI;
				//initGuess[0] is signed distance of line pair from origin in rotated coordinate frame.  initGuess[1] is angle made by lines with axis on either side.
				if (line1[0] == M_PI_2)
					initGuess[0] = line1[1];
				else
					initGuess[0] = -line1[1] * cos (line1[0]);
				initGuess[1] = line1[0];
			}
			if (initGuess[0] > 0)
			{
				if (initGuess[1] < M_PI_2)
					initGuess[1] = M_PI - initGuess[1];
			}
			else
			{
				if (initGuess[1] > M_PI_2)
					initGuess[1] = M_PI - initGuess[1];
			}
		}
		else
		{
			for (i = 0; i < PtsCount * 2; i ++) *(localPts + i) = *(pts + i);
			if (!hM->ShiftaroundCentroid_2D(localPts, PtsCount, &Centroid[0])) 
			{	
				free(localPts); return false;
			}
			double line1[2] = {0};
			double line2[2] = {0};
			if (PtsCount > 50)
			{
				//get best fit line of first 20 points and subsequent 20 points which deviate from this best fit line to determine initguess values
				if (!Line_BestFit_2D(localPts, 20, line1, true))
				{
					free(localPts);
					return false;
				}
				double dev = 0;
				int j = 0;
				int cnt = 0;
				for (j = 20; j < PtsCount; j++)
				{
					if ((line1[0] == M_PI_2) || (line1[0] == 3 * M_PI_2)) 
						dev = localPts[2*j] - line1[1];
					else
						dev = localPts[2*j + 1] - tan(line1[0])*localPts[2*j] - line1[1];
					if (abs(dev) > maxdev)
					{
						cnt++;
						if (cnt >=20)
						{
							if (!Line_BestFit_2D(localPts, j - 20, line1, true))
							{
								free(localPts);
								return false;
							}
							cnt = 0;
							int k = j - 19;
							for (k = j - 19; k <= j; k++)
							{
								if ((line1[0] == M_PI_2) || (line1[0] == 3 * M_PI_2)) 
									dev = localPts[2*k] - line1[1];
								else
									dev = localPts[2*k + 1] - tan(line1[0])*localPts[2*k] - line1[1];
								if (abs(dev) > maxdev)
								{
									cnt++;
								}
								else
									break;
							}
							if (cnt >= 20)
								break;
							else
								cnt = 0;
						}
					}
					else
						cnt = 0;
				}
				if (j == PtsCount)
				{
					line2[0] = line1[0]; 
					line2[1] = line1[1];
				}
				else
				{
					if (!Line_BestFit_2D(localPts, j-19, line1, true))
					{
						free(localPts);
						return false;
					}
					if (!Line_BestFit_2D(localPts + 2*(j - 19), 20, line2, true))
					{
						free(localPts);
						return false;
					}
					cnt = 0;
					int k = j;
					for (k = j; k < PtsCount; k++)
					{
						if ((line2[0] == M_PI_2) || (line2[0] == 3 * M_PI_2)) 
							dev = localPts[2*k] - line2[1];
						else
							dev = localPts[2*k + 1] - tan(line2[0])*localPts[2*k] - line2[1];
						if (abs(dev) > maxdev)
						{
							cnt++;
							if (cnt >=20)
							{
								if (!Line_BestFit_2D(localPts + 2*(j - 19), k - j + 1, line2, true))
								{
									free(localPts);
									return false;
								}
								cnt = 0;
								int m = k - 19;
								for (m = k - 19; m <= k; m++)
								{
									if ((line2[0] == M_PI_2) || (line2[0] == 3 * M_PI_2)) 
										dev = localPts[2*m] - line2[1];
									else
										dev = localPts[2*m + 1] - tan(line2[0])*localPts[2*m] - line2[1];
									if (abs(dev) > maxdev)
									{
										cnt++;
									}
									else
										break;
								}
								if (cnt >= 20)
								{
									k -= 19;
									break;
								}
								else
									cnt = 0;
							}
						}
						else
							cnt = 0;
					}

					if (!Line_BestFit_2D(localPts + 2*(j - 19), k - j + 19, line2, true))
					{
						free(localPts);
						return false;
					}
				}
				if (line1[0] < 0)
					line1[0] += 2*M_PI;
				line1[0] -= int(line1[0] / M_PI ) * M_PI ;
				if (line1[0] == M_PI_2) 
				{
					initGuess[0] = line1[1];
					initGuess[1] = 0;
				}
				else
				{
					initGuess[0] = -line1[1] * sin(line1[0]) * cos (line1[0]);
					initGuess[1] = line1[1] * cos(line1[0]) * cos (line1[0]);
				}
				initGuess[2] = line1[0];
				if (line2[0] < 0)
					line2[0] += 2*M_PI;
				line2[0] -= int(line2[0] / M_PI ) * M_PI ;
				if (line2[0] == M_PI_2) 
				{
					initGuess[3] = line2[1];
					initGuess[4] = 0;
				}
				else
				{
					initGuess[3] = -line2[1] * sin(line2[0]) * cos (line2[0]);
					initGuess[4] = line2[1] * cos(line2[0]) * cos (line2[0]);
				}
				initGuess[5] = line2[0];
			}
			else
			{
				Pt tmppt;
				PtsList tempptslist;
				for (int i = 0; i < PtsCount; i++)
				{
					tmppt.x = *(localPts + 2 * i); tmppt.y = *(localPts + 2 * i + 1); tmppt.pt_index = i;
					tempptslist.push_back(tmppt);
				}
				orderpoints(&tempptslist);
				PtsList::iterator iter = tempptslist.begin();
				double tmppts[4] = {0};
				tmppts[0] = iter->x;
				tmppts[1] = iter->y;
				iter++;
				tmppts[2] = iter->x;
				tmppts[3] = iter->y;
				if (!Line_BestFit_2D(tmppts, 2, line1, false))
				{
					free(localPts);
					return false;
				}
				if (line1[0] < 0)
					line1[0] += 2*M_PI;
				line1[0] -= int(line1[0] / M_PI ) * M_PI ;
				if (line1[0] == M_PI_2) 
				{
					initGuess[0] = line1[1];
					initGuess[1] = 0;
				}
				else
				{
					initGuess[0] = -line1[1] * sin(line1[0]) * cos (line1[0]);
					initGuess[1] = line1[1] * cos(line1[0]) * cos (line1[0]);
				}
				initGuess[2] = line1[0];
				iter = tempptslist.end();
				iter--;
				tmppts[0] = iter->x;
				tmppts[1] = iter->y;
				iter--;
				tmppts[2] = iter->x;
				tmppts[3] = iter->y;
				if (!Line_BestFit_2D(tmppts, 2, line2, false))
				{
					free(localPts);
					return false;
				}
				if (line2[0] < 0)
					line2[0] += 2*M_PI;
				line2[0] -= int(line2[0] / M_PI ) * M_PI ;
				if (line2[0] == M_PI_2) 
				{
					initGuess[3] = line2[1];
					initGuess[4] = 0;
				}
				else
				{
					initGuess[3] = -line2[1] * sin(line2[0]) * cos (line2[0]);
					initGuess[4] = line2[1] * cos(line2[0]) * cos (line2[0]);
				}
				initGuess[5] = line2[0];
				tempptslist.clear();
			}
		}
	
		//Do best-fit and check if successful
		bool LS_BF_successful = false;
		if (axis_given == true)
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 0, 2, -1, hM->TwoLine_AxisGiven,true, 10000, Tolerance, &TwoLine[0]);
		}
		else
		{
			LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 2, 6, -1, hM->TwoLine,true, 10000, Tolerance, &TwoLine[0]);
		}

		if (LS_BF_successful)
		{	
			if (axis_given == true)
			{
				double ang = TwoLine[1];
				if (ang < 0)
					ang += 2*M_PI;
				ang -= int(ang / M_PI ) * M_PI ;
				if (TwoLine[0] > 0)
				{
					if (ang < M_PI_2)
						ang = M_PI - ang;
				}
				else
				{
					if (ang > M_PI_2)
						ang = M_PI - ang;
				}
				TwoLine[1] = ang;
				answer[0] = AxisPt[0] + TwoLine[0] * sin (TwoLine[1]) * cos (AxisSlope) + TwoLine[0] * cos (TwoLine[1]) * sin (AxisSlope);
				answer[1] = AxisPt[1] + TwoLine[0] * sin (TwoLine[1]) * sin (AxisSlope) - TwoLine[0] * cos (TwoLine[1]) * cos (AxisSlope);
				answer[2] = AxisSlope + TwoLine[1];
				answer[2] -= int( answer[2] / M_PI) * M_PI;
				answer[3] = AxisPt[0] + TwoLine[0] * sin (TwoLine[1]) * cos (AxisSlope) - TwoLine[0] * cos (TwoLine[1]) * sin (AxisSlope);
				answer[4] = AxisPt[1] + TwoLine[0] * sin (TwoLine[1]) * sin (AxisSlope) + TwoLine[0] * cos (TwoLine[1]) * cos (AxisSlope);
				answer[5] = AxisSlope - TwoLine[1];
				answer[5] -= int( answer[5] / M_PI) * M_PI;
			}
			else
			{
				if (TwoLine[2] < 0)
					TwoLine[2] += 2*M_PI;
				TwoLine[2] -= int(TwoLine[2] / M_PI ) * M_PI ;
				if (TwoLine[5] < 0)
					TwoLine[5] += 2*M_PI;
				TwoLine[5] -= int(TwoLine[5] / M_PI ) * M_PI ;
				TwoLine[0] -= (TwoLine[0] * cos (TwoLine[2]) + TwoLine[1] * sin (TwoLine[2])) * cos (TwoLine[2]);
				TwoLine[1] -= (TwoLine[0] * cos (TwoLine[2]) + TwoLine[1] * sin (TwoLine[2])) * sin (TwoLine[2]);
				TwoLine[3] -= (TwoLine[3] * cos (TwoLine[5]) + TwoLine[4] * sin (TwoLine[5])) * cos (TwoLine[5]);
				TwoLine[4] -= (TwoLine[3] * cos (TwoLine[5]) + TwoLine[4] * sin (TwoLine[5])) * sin (TwoLine[5]);
				for (int j = 0; j < 2; j ++) 
				{
					*(TwoLine + j) += Centroid[j];
					*(TwoLine + j + 3) += Centroid[j];
				}
				memcpy((void*)answer, (void*)&TwoLine[0], 6 * sizeof(double));
			}
		}
		free(localPts);
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0022", __FILE__, __FUNCSIG__);
		return false;
	}
}
//get best fit line-arc-line for set of points such that both lines are tangential to arc at its two ends.
//the parameters would be ang1, intercept1, ang2, intercept2, x_center, y_center and radius for the line-arc-line group
//if initguess flag is true, take initial guess values from array passed.
//if linesgiven flag is true, answer[0], answer[1], answer[2] and answer[3] will be the passed with init guess values for center of arc on axis, slope of axis and radius of arc.
bool RBF::LineArcLine_BestFit(double* pts, int PtsCount, double* answer, bool initguess, bool lines_given)
{
	try
	{
		double LineArcLine[5], initGuess[5];
		ZeroMemory(&LineArcLine[0], sizeof(double) * 5);
		ZeroMemory(&initGuess[0], sizeof(double) * 5);
		double* localPts = NULL;
		double line1[2] = {0};
		double line2[2] = {0};
		double circle[3] = {0};

		bool LS_BF_successful = false;
		
		if (lines_given)
		{
			if (PtsCount < 3) 
			{
				return false;
			}
			localPts = (double*) malloc(sizeof(double) * PtsCount * 2);
			for (int i = 0; i < PtsCount; i ++) 
			{
				*(localPts + 2*i + 0) = (*(pts + 2*i + 0) - answer[0]) * cos (answer[2]) + (*(pts + 2*i + 1) - answer[1]) * sin (answer[2]);
				*(localPts + 2*i + 1) = (*(pts + 2*i + 1) - answer[1]) * cos (answer[2]) - (*(pts + 2*i + 0) - answer[0]) * sin (answer[2]);
			}
			//initGuess[0] gives the x-axis value of center in rotated coord frame where axis is along x-axis.
			initGuess[0] = 0;
			//initGuess[1] is radius.
			initGuess[1] = answer[3];
		}
		else
		{
			if (PtsCount < 7) return false;
			if (initguess)
			{
				initGuess[0] = answer[0];

				//intercept is being stored for later comparison with best fit values returned.
				line1[1] = answer[1];

				initGuess[1] = answer[2];

				//intercept is being stored for later comparison with best fit values returned.
				line2[1] = answer[3];

				initGuess[2] = answer[4];
				initGuess[3] = answer[5];
				initGuess[4] = answer[6];
			}
			else
			{
				int pts_line = (PtsCount * 2) / 7;
				if (!Line_BestFit_2D(pts, pts_line, line1, false))
					return false;
				int pts_2nd_line_start_cnt = (PtsCount * 5) / 7;
				if (!Line_BestFit_2D(pts + 2 * pts_2nd_line_start_cnt, pts_line, line2, false))
					return false;
				int pts_circle = (PtsCount * 4) / 7;
				if (!Circle_BestFit(pts + 2 * pts_line, pts_circle, circle, false))
					return false;

				initGuess[0] = line1[0];
				initGuess[1] = line2[0];
				initGuess[2] = circle[0];
				initGuess[3] = circle[1];
				initGuess[4] = circle[2];
			}
		}
		if (lines_given)
			LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 1, 2, -1, hM->LineArcLine_AxisGiven, true, 10000, Tolerance, &LineArcLine[0]);
		else
			LS_BF_successful = hM->LS_BF_LM(pts, &initGuess[0], PtsCount, 4, 5, -1, hM->LineArcLine,true, 10000, Tolerance, &LineArcLine[0]);
		
		if (LS_BF_successful)
		{
			if (lines_given)
			{
				answer[0] += LineArcLine[0] * cos (answer[2]);
				answer[1] += LineArcLine[0] * sin (answer[2]);
				if (LineArcLine[1] < 0)
					LineArcLine[1] = -LineArcLine[1];
				answer[3] = LineArcLine[1];
				free(localPts);
			}
			else
			{
				//update first line parameters in first 2 params of answer.
				*(answer + 0) = LineArcLine[0];
				double ang = LineArcLine[0];
				if (LineArcLine[4] < 0)
					LineArcLine[4] = - LineArcLine[4];
				double v[2] = {0};
				v[0] = LineArcLine[2] - LineArcLine[4] * sin (ang);
				v[1] = LineArcLine[3] + LineArcLine[4] * cos (ang);
				double intercept1;
				if (cos (ang) == 0)
				{
					intercept1 = v[0];
				}
				else
				{
					intercept1 = v[1] - v[0] * tan (ang);
				}
				v[0] = LineArcLine[2] + LineArcLine[4] * sin (ang);
				v[1] = LineArcLine[3] - LineArcLine[4] * cos (ang);
				double intercept2;
				if (cos (ang) == 0)
				{
					intercept2 = v[0];
				}
				else
				{
					intercept2 = v[1] - v[0] * tan (ang);
				}
				if (abs(intercept1 - line1[1]) < abs(intercept2 - line1[1]))
				{
					*(answer + 1) = intercept1;
				}
				else
				{
					*(answer + 1) = intercept2;
				}

				//update last line params in 3rd and 4th param of answer
				*(answer + 2) = LineArcLine[1];
				ang = LineArcLine[1];
				v[0] = LineArcLine[2] - LineArcLine[4] * sin (ang);
				v[1] = LineArcLine[3] + LineArcLine[4] * cos (ang);
				if (cos (ang) == 0)
				{
					intercept1 = v[0];
				}
				else
				{
					intercept1 = v[1] - v[0] * tan (ang);
				}
				v[0] = LineArcLine[2] + LineArcLine[4] * sin (ang);
				v[1] = LineArcLine[3] - LineArcLine[4] * cos (ang);
				if (cos (ang) == 0)
				{
					intercept2 = v[0];
				}
				else
				{
					intercept2 = v[1] - v[0] * tan (ang);
				}
				if (abs(intercept1 - line2[1]) < abs(intercept2 - line2[1]))
				{
					*(answer + 3) = intercept1;
				}
				else
				{
					*(answer + 3) = intercept2;
				}

				//update circle parameters in last 3 params of answer
				*(answer + 4) = LineArcLine[2];
				*(answer + 5) = LineArcLine[3];
				*(answer + 6) = LineArcLine[4];
			}
		}
		
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0023", __FILE__, __FUNCSIG__);
		return false;
	}
}

//best fit pair of arcs that are tangential to each other.  when arc_given is true, first arc parameters (center, radius) are not modified and only second arc parameters
//are modified to get the best fit.  the answer will consist of eight parameters, 3 each for the two arcs plus the intersection point of the two arcs.  initial guess values
//are passed through answer.
bool RBF::ArcArc_BestFit(double* pts, int PtsCount, double* answer, bool arc_given)
{
	try
	{
		double initGuess[6] = {0};
		double* localPts = NULL;
		double c1[2] = {answer[0], answer[1]};
		double c2[2] = {answer[3], answer[4]};
		double cavg[2] = {(c1[0] + c2[0])/2, (c1[1] + c2[1])/2};
		double r1 = answer[2];
		double r2 = answer[5];
		double intpt[2] = {answer[6], answer[7]};
		double rad_vec[2] = {cavg[0] - intpt[0], cavg[1] - intpt[1]};
		double norm = hM->pythag(rad_vec[0], rad_vec[1]);
		if (norm == 0)
			return false;
		for (int i = 0; i < 2; i++)
			rad_vec[i] = rad_vec[i] / norm;
		double ArcArc[6] = {0};
		if (arc_given == false)
		{
			c1[0] = cavg[0] + (r1 - r2)/2 * rad_vec[0];
			c1[1] = cavg[1] + (r1 - r2)/2 * rad_vec[1];
			c2[0] = cavg[0] - (r1 - r2)/2 * rad_vec[0];
			c2[1] = cavg[1] - (r1 - r2)/2 * rad_vec[1];
			r1 = hM->pythag(c1[0] - intpt[0], c1[1] - intpt[1]);
			r2 = hM->pythag(c2[0] - intpt[0], c2[1] - intpt[1]);

			initGuess[0] = c1[0]; initGuess[1] = c1[1]; initGuess[2] = r1; initGuess[3] = r2; initGuess[4] = intpt[0]; initGuess[5] = intpt[1];
		}
		else
		{
			for (int i = 0; i < 2; i++)
				rad_vec[i] = c1[i] - intpt[i];
			norm = hM->pythag(rad_vec[0], rad_vec[1]);
			if (norm == 0)
				return false;
			for (int i = 0; i < 2; i++)
				rad_vec[i] = rad_vec[i] / norm;
			c2[0] = c1[0] - (r1 - r2) * rad_vec[0];
			c2[1] = c1[1] - (r1 - r2) * rad_vec[1];
			r2 = hM->pythag(c2[0] - intpt[0], c2[1] - intpt[1]);
			localPts = (double*) malloc(sizeof(double) * PtsCount * 2);
			for (int i = 0; i < PtsCount; i ++) 
			{
				*(localPts + 2*i + 0) = (*(pts + 2*i + 0) - c1[0]);
				*(localPts + 2*i + 1) = (*(pts + 2*i + 1) - c1[1]);
			}
			initGuess[0] = intpt[0] - c1[0]; initGuess[1] = intpt[1] - c1[1]; initGuess[2] = r2;
		}
		bool LS_BF_successful = false;
		if (arc_given)
			LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 2, 3, -1, hM->ArcArc_ArcGiven, true, 10000, Tolerance, &ArcArc[0]);
		else
			LS_BF_successful = hM->LS_BF_LM(pts, &initGuess[0], PtsCount, 2, 6, -1, hM->ArcArc,true, 10000, Tolerance, &ArcArc[0]); 

		if (LS_BF_successful)
		{
			if (arc_given)
			{
				answer[6] = ArcArc[0] + c1[0];
				answer[7] = ArcArc[1] + c1[1];
				answer[5] = ArcArc[2];
				double d = hM->pythag(ArcArc[0], ArcArc[1]);

				if (d == 0)
				{
					free(localPts);
					return false;
				}
				answer[3] = (d - ArcArc[2])/d * ArcArc[0] + c1[0];
				answer[4] = (d - ArcArc[2])/d * ArcArc[1] + c1[1];

				free(localPts);
			}
			else
			{
				c1[0] = ArcArc[0]; c1[1] = ArcArc[1]; 
				r1 = ArcArc[2]; r2 = ArcArc[3];
				intpt[0] = ArcArc[4]; intpt[1] = ArcArc[5];
				for (int i = 0; i < 2; i++)
					rad_vec[i] = c1[i] - intpt[i];
				norm = hM->pythag(rad_vec[0], rad_vec[1]);
				if (norm == 0)
					return false;
				for (int i = 0; i < 2; i++)
					rad_vec[i] = rad_vec[i] / norm;
			
				c2[0] = c1[0] - (r1 - r2) * rad_vec[0];
				c2[1] = c1[1] - (r1 - r2) * rad_vec[1];
				answer[0] = c1[0]; answer[1] = c1[1]; answer[2] = r1; 
				answer[3] = c2[0]; answer[4] = c2[1]; answer[5] = r2;
				answer[6] = intpt[0]; answer[7] = intpt[1];
			}
		}
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0024", __FILE__, __FUNCSIG__);
		return false;
	}
}
//best fit line-arc pair that are tangential to each other.  when line_given is true, line params are not modified and only arc parameters
//are modified to get the best fit.  the answer will consist of four parameters, 2 for line and 2 for center of the arc (radius would be distance from center to line).  
//initial guess values are passed through answer.
bool RBF::LineArc_BestFit(double* pts, int PtsCount, double* answer, bool line_given)
{
	try
	{
		double initGuess[4] = {0};
		double* localPts = NULL;
		double line[2] = {answer[0], answer[1]};
		double center[2] = {answer[2], answer[3]};
		double LineArc[4] = {0};
		double radius = 0;
		if ((line[0] == M_PI_2) || (line[0] == 3 * M_PI_2))
			radius = abs(center[0] - line[1]);
		else
			radius = abs((tan(line[0]) * center[0] + line[1] - center[1]) * cos(line[0]));
		if (line_given == false)
		{
			initGuess[0] = line[0]; initGuess[1] = line[1]; initGuess[2] = center[0]; initGuess[3] = center[1]; 
		}
		else
		{
			localPts = (double*) malloc(sizeof(double) * PtsCount * 2);	
			for (int i = 0; i < PtsCount; i ++) 
			{
				if ((line[0] == M_PI_2) || (line[0] == 3 * M_PI_2))
				{
					*(localPts + 2*i + 0) = *(pts + 2*i + 1);
					*(localPts + 2*i + 1) = - (*(pts + 2*i + 0) - line[1]);
					initGuess[0] = center[1];
					initGuess[1] = -(center[0] - line[1]);
				}
				else
				{
					*(localPts + 2*i + 0) = (*(pts + 2*i + 0)) * cos (line[0]) + (*(pts + 2*i + 1) - line[1]) * sin (line[0]);
					*(localPts + 2*i + 1) = (*(pts + 2*i + 1) - line[1]) * cos (line[0]) - (*(pts + 2*i + 0) ) * sin (line[0]);
					initGuess[0] = center[0] * cos (line[0]) + (center[1] - line[1]) * sin (line[0]);
					initGuess[1] = (center[1] - line[1]) * cos (line[0]) - center[0] * sin (line[0]);
				}
			}
		}
		bool LS_BF_successful = false;
		if (line_given)
			LS_BF_successful = hM->LS_BF_LM(localPts, &initGuess[0], PtsCount, 0, 2, -1, hM->LineArc_LineGiven, true, 10000, Tolerance, &LineArc[0]);
		else
			LS_BF_successful = hM->LS_BF_LM(pts, &initGuess[0], PtsCount, 2, 4, -1, hM->LineArc,true, 10000, Tolerance, &LineArc[0]); 

		if (LS_BF_successful)
		{
			if (line_given)
			{
				if ((line[0] == M_PI_2) || (line[0] == 3 * M_PI_2))
				{
					answer[2] = line[1] - LineArc[1];
					answer[3] = LineArc[0];
				}
				else
				{
					answer[2] = LineArc[0] * cos (line[0]) - LineArc[1] * sin (line[0]);
					answer[3] = LineArc[0] * sin (line[0]) + LineArc[1] * cos (line[0]) + line[1];
				}
				free(localPts);
			}
			else
			{
				memcpy(answer, LineArc, 4*sizeof(double));
			}
		}
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0025", __FILE__, __FUNCSIG__);
		return false;
	}
}

//best fit arc-arc-arc combo that are tangential to each other.  the answer will consist of 13 parameters, 3 for each of the arcs and two intersection points.  
//initial guess values are passed through answer.
bool RBF::ArcArcArc_BestFit(double* pts, int PtsCount, double* answer)
{
	try
	{
		double initGuess[9] = {0};
		double c1[2] = {answer[0], answer[1]};
		double c2[2] = {answer[3], answer[4]};
		double c3[3] = {answer[6], answer[7]};
		double r1 = answer[2];
		double r2 = answer[5];
		double r3 = answer[8];
		double intpt1[2] = {answer[9], answer[10]};
		double intpt2[2] = {answer[11], answer[12]};
		double rad_vec1[2] = {c2[0] - intpt1[0], c2[1] - intpt1[1]};
		double rad_vec2[2] = {c2[0] - intpt2[0], c2[1] - intpt2[1]};
		double norm = hM->pythag(rad_vec1[0], rad_vec1[1]);
		if (norm == 0)
			return false;
		for (int i = 0; i < 2; i++)
			rad_vec1[i] = rad_vec1[i] / norm;

		norm = hM->pythag(rad_vec2[0], rad_vec2[1]);
		if (norm == 0)
			return false;
		for (int i = 0; i < 2; i++)
			rad_vec2[i] = rad_vec2[i] / norm;

		double ArcArcArc[9] = {0};
		c1[0] = c2[0] + (r1 - r2) * rad_vec1[0];
		c1[1] = c2[1] + (r1 - r2) * rad_vec1[1];

		c3[0] = c2[0] - (r2 - r3) * rad_vec2[0];
		c3[1] = c2[1] - (r2 - r3) * rad_vec2[1];

		initGuess[0] = c2[0]; initGuess[1] = c2[1]; initGuess[2] = r1; initGuess[3] = r2; initGuess[4] = r3; 
		initGuess[5] = intpt1[0]; initGuess[6] = intpt1[1]; initGuess[7] = intpt2[0]; initGuess[8] = intpt2[1];

		bool LS_BF_successful = false;
		LS_BF_successful = hM->LS_BF_LM(pts, &initGuess[0], PtsCount, 3, 9, -1, hM->ArcArcArc,true, 10000, Tolerance, &ArcArcArc[0]); 

		if (LS_BF_successful)
		{
			c2[0] = ArcArcArc[0]; c2[1] = ArcArcArc[1]; 
			r1 = ArcArcArc[2]; r2 = ArcArcArc[3]; r3 = ArcArcArc[4];
			intpt1[0] = ArcArcArc[5]; intpt1[1] = ArcArcArc[6]; intpt2[0] = ArcArcArc[7]; intpt2[1] = ArcArcArc[8];
			for (int i = 0; i < 2; i++)
			{
				rad_vec1[i] = c2[i] - intpt1[i];
				rad_vec2[i] = c2[i] - intpt2[i];
			}
			norm = hM->pythag(rad_vec1[0], rad_vec1[1]);
			if (norm == 0)
				return false;
			for (int i = 0; i < 2; i++)
				rad_vec1[i] = rad_vec1[i] / norm;
			norm = hM->pythag(rad_vec2[0], rad_vec2[1]);
			if (norm == 0)
				return false;
			for (int i = 0; i < 2; i++)
				rad_vec2[i] = rad_vec2[i] / norm;	

			c1[0] = c2[0] + (r1 - r2) * rad_vec1[0];
			c1[1] = c2[1] + (r1 - r2) * rad_vec1[1];

			c3[0] = c2[0] - (r2 - r3) * rad_vec2[0];
			c3[1] = c2[1] - (r2 - r3) * rad_vec2[1];

			answer[0] = c1[0]; answer[1] = c1[1]; answer[2] = r1; 
			answer[3] = c2[0]; answer[4] = c2[1]; answer[5] = r2;
			answer[6] = c3[0]; answer[7] = c3[1]; answer[8] = r3;
			answer[9] = intpt1[0]; answer[10] = intpt1[1];
			answer[11] = intpt2[0]; answer[12] = intpt2[1];
		}
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0026", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::CircleInvolute_BestFit(double* pts, int PtsCount, double* answer)
{
	try
	{
		if (PtsCount < 5)
			return false;
		double initGuess[4] = {0,0,1,0};
		double CircleInvolute[4] = {0};
		if (answer[2] != 0)
			memcpy(initGuess, answer, 4*sizeof(double));
		else
		{
			int step = 1;
			if (PtsCount > 100)
				step = PtsCount/100;
			int localptscount = PtsCount/step;
			double* circlepts = (double*) malloc(2*sizeof(double)*(localptscount - 2));
			memset(circlepts,0,2*sizeof(double)*(localptscount - 2));
			int k = 0;
			double Ans[3] = {0};
			double mincirclept[2] = {0};
			double minR = 0;
			bool first = true;
			for (int i = 0; i < localptscount - 2; i++)
			{
				if (!Circle_3Pt(pts + 2*i*step, Ans)) 
					continue;
				if (first)
				{
					mincirclept[0] = Ans[0];
					mincirclept[1] = Ans[1];
					minR = Ans[2];
					first = false;
				}
				else
				{
					if (Ans[2] < minR)
					{
						mincirclept[0] = Ans[0];
						mincirclept[1] = Ans[1];
						minR = Ans[2];
					}
				}
				memcpy(circlepts + 2*k, Ans, 2*sizeof(double));
				k++;
			}
			if (!Circle_BestFit(circlepts, localptscount - 2, initGuess, true))
			{
				free(circlepts);
				return false;
			}
			double delY = mincirclept[1] - initGuess[1];
			double delX = mincirclept[0] - initGuess[0];
			if (delX == 0)
			{
				if (delY >= 0)
					initGuess[3] = M_PI_2;
				else
					initGuess[3] = 3 * M_PI_2;
			}
			else
			{
				initGuess[3] = atan(delY/delX);
				if (delY < 0)
					initGuess[3] += M_PI;
				if (initGuess[3] < 0)
					initGuess[3] += 2*M_PI;
			}
			free(circlepts);
		}
		bool LS_BF_successful = false;
		LS_BF_successful = hM->LS_BF_LM(pts, &initGuess[0], PtsCount, 2, 4, -1, hM->CircleInvolute,true, 10000, Tolerance, &CircleInvolute[0]); 
		if (LS_BF_successful)
		{
			CircleInvolute[3] -= int(CircleInvolute[3] / (2 * M_PI )) * 2 * M_PI ;
			if (CircleInvolute[3] < 0)
				CircleInvolute[3] += 2 * M_PI;
			memcpy (answer, CircleInvolute, 4 * sizeof(double));
		}
		return LS_BF_successful;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0026.a", __FILE__, __FUNCSIG__);
		return false;
	}
}
//===================  End Best Fit Functions =====================
//=====================================================================

bool RBF::CalculateFocus_old(double* pts, int TotalPtsCount, double ActiveWidth, double* answer)
{
	try
	{
		double StartZ = *(pts);
		int TotalPts = TotalPtsCount * 2;
		double* tP = (double*) calloc(TotalPts, sizeof(double));
		//memcpy((void*)tP, (void*)pts, sizeof(tP));
		int Maxindex = 0; 
		double Max = 0.0;
		int Minindex = 0; 
		double Min = 10000000.0;
		for (int i = 0; i < TotalPts; i += 2)
		{	if (*(pts + i + 1) > Max)
			{	Max = *(pts + i + 1);
				Maxindex = i;
			}
			if (*(pts + i + 1) < Min)
			{	Min = *(pts + i + 1);
				Minindex = i;
			}
		}
	
		//std::wofstream cf;
		//cf.open("D:\\FocusPlot.csv");
	
		//Now shift and normalise input data.
		int j = 0;
		for (int i = 0; i < TotalPts; i += 2)
		{	if (abs(*(pts + i) - *(pts + Maxindex)) < ActiveWidth)
			{	//Shift origin to be within reasonable numbers in Z
				*(tP + j) = *(pts + i) - StartZ;		
				//Normalise the focus values
				*(tP + j + 1) = *(pts + i + 1)/ Max;	
				//cf << *(tP + j) << "," << *(tP + j + 1) << std::endl;
				j += 2;
			}
		}
		//cf.close();

		double StartingGuess[4];
		StartingGuess[0] = 1; StartingGuess[1] = *(pts + Maxindex) - StartZ;
		StartingGuess[2] = 0.15; StartingGuess[3] = Min;// *(tP + Minindex + 1);
		double param[5] = {0};

		//Now call best fit 
		bool gotBF = hM->LS_BF_LM(tP, &StartingGuess[0], j/2, 0, 4, -1, hM->Focus, false, 6000, Tolerance, &param[0]);
		free(tP);
		if (gotBF)
		{	*answer = param[1] + StartZ;
			return true;
		}
		return false;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0027", __FILE__, __FUNCSIG__);return false;}
}

bool RBF::CalculateFocus(double* pts, int TotalPtsCount, double ActiveWidth, double* answer, double MaxMinFocusRatio_cutoff, double MaxMinZRatio_cutoff, double SigmaActiveWidthRatio_cutoff, double FocusSpanRatio_cutoff, double minslope, double maxslope, double z_jump, double minFocusminCutoffRatio)
{
	try
	{
		int TotalPts = TotalPtsCount * 2;
		double* tP = (double*) calloc(TotalPts, sizeof(double));
		int Maxindex = 0; 
		double Max = 0.0;
		int Minindex = 0; 
		double Min = 10000000.0;
		for (int i = 0; i < TotalPts; i += 2)
		{
			if (*(pts + i + 1) > Max)
			{	Max = *(pts + i + 1);
				Maxindex = i;
			}
			if (*(pts + i + 1) < Min)
			{	Min = *(pts + i + 1);
				Minindex = i;
			}
		}
		//added on Jan 11 2012 - if Max is not greater than twice min, the range is too small to do a best fit gaussian, so return false.
		//modified on Feb 25 2012 as below
		if (Max < MaxMinFocusRatio_cutoff * Min) 
			return false;

		//define min_cutoff as below - do best fit gaussian only on pts having focus above min_cutoff
		double min_cutoff = Min + (Max - Min) * FocusSpanRatio_cutoff;

		//Now shift and normalise input data.
		int j = 0;
		double z_max = 0, z_min = 0;
		double f_at_z_max = min_cutoff, f_at_z_min = min_cutoff;
		double diff_z = 0;
		double diff_focus = 0;
		double prev_z = 0;
		double prev_focus = -1;
		bool include_pt = true;
		for (int i = 0; i < TotalPts; i += 2)
		{	
			//added check to only include points having value greater than min_cutoff in tP - Feb 25 2012
			if ((abs(*(pts + i) - *(pts + Maxindex)) < ActiveWidth) && (*(pts + i + 1) >= min_cutoff))
			{	
				if (prev_focus != -1)
				{
					diff_focus = abs(*(pts + i + 1)/ Max - prev_focus);
					diff_z = abs(*(pts + i) - *(pts + Maxindex) - prev_z);
					if ((diff_z < z_jump) || (diff_focus < minslope * diff_z) || (diff_focus > maxslope * diff_z))
						include_pt = false;
					else
						include_pt = true;
				}
				if (include_pt)
				{
					//changed origin to *(pts + Maxindex) instead of startZ - Jan 11 2012
					*(tP + j) = *(pts + i) - *(pts + Maxindex);		
					//Normalise the focus values
					*(tP + j + 1) = *(pts + i + 1)/ Max;	

					if (*(tP + j) > z_max)
					{
						z_max = *(tP + j);
						f_at_z_max = *(tP + j + 1);
					}
					if (*(tP + j) < z_min)
					{
						z_min = *(tP + j);
						f_at_z_min = *(tP + j + 1);
					}
					j += 2;
					prev_focus = *(pts + i + 1)/ Max;
					prev_z = *(pts + i) - *(pts + Maxindex);
				}
			}
		}
		//this means the distribution is skewed and so not a gaussian.
		if ((z_max > MaxMinZRatio_cutoff * abs(z_min)) || (MaxMinZRatio_cutoff * z_max < abs(z_min)))
		{
			if ((f_at_z_max > min_cutoff * minFocusminCutoffRatio) || (f_at_z_min > min_cutoff * minFocusminCutoffRatio))
				return false;
		}

		double StartingGuess[4];
		//modified starting guess as below - Jan 11 2012
		StartingGuess[0] = 1 - Min/Max; StartingGuess[1] = 0;
		StartingGuess[2] = 0.5*(z_max - z_min); StartingGuess[3] = Min/Max;// *(tP + Minindex + 1);
		double param[5];
		for (int i = 0; i < 5; i ++) param[i] = 0;

		//Now call best fit 
		//changed 4th parameter to 1 from 0 and 8th to true from false - Jan 11 2012
		bool gotBF = hM->LS_BF_LM(tP, &StartingGuess[0], j/2, 1, 4, -1, hM->Focus, true, 6000, Tolerance, &param[0]);
		free(tP);
		if (abs(param[2]) > ActiveWidth * SigmaActiveWidthRatio_cutoff)
			return false;
		if (gotBF)
		{	*answer = param[1] + *(pts + Maxindex);
			return true;
		}
		return false;
	}
	catch(...){SetAndRaiseErrorMessage("RBF0028", __FILE__, __FUNCSIG__);return false;}
}

void RBF::CompensateErrors(double* P, double Angle, double* Ans, double AngleXZ, double AngleYZ)
{
	try
	{
		Ans[0] = P[0] + sin(Angle) * P[1] + sin(AngleXZ) * P[2];
		Ans[1] = cos(Angle) * P[1];
		Ans[2] = cos(AngleXZ) * P[2];
		Ans[1] = Ans[1] + sin(AngleYZ) * Ans[2];
		Ans[2] = cos(AngleYZ) * Ans[2];
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0029", __FILE__, __FUNCSIG__); return;}
}

bool RBF::GetFocusMetric(double* Image, int ImageWidth, int* pR, double* pFocus, int N)
{
	try
	{
		double focMet = 0.0; //holds the measure of the contrast
		double toTal = 0.0; //holds the total intensity level of this section
		//int channel = 1;
		int pixelShift = 2;
		int BoxNumber = N * 4; //The start and end pixels of the array of pixel values
		int j = ImageWidth * pixelShift; //Addding j will give x, y+2 pixel's value!
		double *pTemp;

		for (int i = 0; i < BoxNumber; i += 4)
		{	focMet = 0.0; toTal = 0.0;
			for (int y = pR[i + 1] + pixelShift; y < pR[i + 1] + pR[i + 3] - pixelShift; y ++)
			{	for (int x = pR[i] + pixelShift; x < pR[i] + pR[i + 2] - pixelShift; x ++)
				{	pTemp = Image + (x  + y * ImageWidth); //Adding 3 since we want Blue channel
					focMet += abs(*(pTemp)  - *(pTemp - pixelShift)) + abs(*(pTemp)  - *(pTemp + pixelShift));
					focMet += abs(*(pTemp)  - *(pTemp + j)) + abs(*(pTemp)  - *(pTemp - j));
					focMet += abs(*(pTemp)  - *(pTemp + j + pixelShift)) + abs(*(pTemp)  - *(pTemp + j - pixelShift));
					focMet += abs(*(pTemp)  - *(pTemp - j + pixelShift)) + abs(*(pTemp)  - *(pTemp - j - pixelShift));
					toTal += *(pTemp);
				}
			}
			if (toTal != 0) *(pFocus + i / 4) = focMet / toTal;	else *(pFocus + i / 4) = -1.0;
		}
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0030", __FILE__, __FUNCSIG__);return false;}
	
	//return false;
}

//functions needed by line-arc separation function.
LineArc RBF::getLine(PtsList* ptr_Pts, double cutoff)
{
	LineArc tmpLine;
	try
	{
		int ptscount = ptr_Pts ->size();
		double* pts = NULL;
		double* ptr_pts;

		pts = (double*) malloc(2*ptscount*sizeof(double));
		ptr_pts = pts;
		PtsList::iterator pt_iter = ptr_Pts ->begin();
		while (pt_iter != ptr_Pts ->end())
		{
			*ptr_pts = pt_iter->x;
			ptr_pts++;
			*ptr_pts = pt_iter->y;
			ptr_pts++;
			pt_iter++;
		}
		double answer[2] = {0};

		Line_BestFit_2D (pts, ptscount, answer, false);

		Pt start_pt = ptr_Pts ->front();
		Pt end_pt = ptr_Pts ->back();
		if (abs(answer[0]) != M_PI * 0.5)
		{
			if (abs(tan(answer[0])) < cutoff)
				answer[0] = 0;
			start_pt.y = tan(answer[0])*start_pt.x + answer[1];
			end_pt.y = tan(answer[0])*end_pt.x + answer[1];
			if (end_pt.x < start_pt.x)
				answer[0] += M_PI;
		}
		else
		{
			start_pt.x = answer[1];
			end_pt.x = answer[1];
			if (end_pt.y < start_pt.y) 
				answer[0] = - M_PI * 0.5;
		}
		double length = sqrt ( pow ((end_pt.y - start_pt.y), 2 ) + pow ((end_pt.x - start_pt.x), 2 ) );

		tmpLine.start_pt = start_pt;
		tmpLine.end_pt = end_pt;
		tmpLine.slope = answer[0]; 
		tmpLine.intercept = answer[1];
		tmpLine.length = length;

		free (pts);

		return tmpLine;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0031", __FILE__, __FUNCSIG__);return tmpLine;}
}

bool RBF::chkPtInSlopesLine(Pt chk_pt, LineArc chk_line, double curr_s, double tolerance)
{
	try
	{
		double err;
		int n = 0;
		double slope = 0;

		slope = chk_line.slope - int(chk_line.slope / M_PI) * M_PI;

		if (abs(slope) != 0.5 * M_PI)
		{
			err = abs (chk_pt.y - tan (slope) * chk_pt.x - chk_line.intercept);

			//following is because y-axis is actually having a range of 0 to 2PI

			n = (int) ( err/(2 * M_PI));
			err -= n * 2 * M_PI;
			err = abs(err);

			if (err > M_PI)
				err = 2 * M_PI - err;
		}
		else
		{
			err = abs (chk_pt.x - chk_line.intercept);
		}

		err = err * abs(chk_pt.x - curr_s);
		if ( err < tolerance)
			return true;
		else
			return false;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0032", __FILE__, __FUNCSIG__);return false;}
}
//Pts holds list of points that need to be separated into lines and arcs.
//SecondOrderslopes is a list of lines where lines with zero slope depict lines and non-zero slope depict arcs.
int RBF::fnLinesAndArcs(PtsList* ptr_Pts, LineArcList* ptr_SecondOrderslopes, double tolerance, double max_radius, double min_radius, int step, double max_dist_betn_neighboring_pts)
{
	try
	{
		PtsList FirstOrderslopes;
		PtsList FirstOrderslopesStartPts;
		PtsList FirstOrderslopesEndPts;
		PtsList PA;
		int m = 0;
		PtsList::iterator i,j, test_iter;
		int step_size;
		double s = 0;
		double prevslope = 0;
		double lower_cutoff = 0;
		double upper_cutoff = 0;

		if ((step == -1)|| (step == 0))
		{
			step_size = ptr_Pts -> size()/20;

			if (step_size == 0 ) step_size = 1;

			if (step_size > 5 ) step_size = 5;
		}
		else
			step_size = step;

		double x_curr = 0;
		double y_curr = 0;
		double x_next = 0;
		double y_next = 0;
		for (i = ptr_Pts ->begin(); i != ptr_Pts ->end(); advance(i,step_size))
		{
			j = i;
			
			for (m = 0; m <= step_size; m++)
			{ 
				//add points to points list
				x_curr = j->x;
				y_curr = j->y;
				PA.push_back(*j);
				++j;
				if (j == ptr_Pts ->end()) 
					break;
				x_next = j->x;
				y_next = j->y;
				if ((abs(y_next - y_curr) > max_dist_betn_neighboring_pts) || (abs(x_next - x_curr) > max_dist_betn_neighboring_pts)) 
						break;
			}

			//following check done to break from loop if we are within step_size of end of pts list, so that we don't run into debug assertion failure
			test_iter = i;
			for (m = 0; m < step_size; m++)
			{
				test_iter++;
				if (test_iter == ptr_Pts->end()) 
					break;
			}
			if (test_iter == ptr_Pts->end()) 
				break;

			// temporary point to be added to firstorderslopes list. 
			Pt tmppt;

			if (PA.size() < 2) continue;

			LineArc tmpLine = getLine(&PA, 0);
			if (s == 0) 
				prevslope = tmpLine.slope;
	
			//s is total length of path from start including current segment
			s += tmpLine.length;

			//x value is distance covered from start till mid-point of current segment.

			tmppt.x = s - (tmpLine.length * 0.5);

			//y value is slope of this line.

			if (tmpLine.slope >= prevslope)
			{
				tmppt.y = tmpLine.slope - int(((tmpLine.slope - prevslope)/(M_PI)) + 0.5) * M_PI;
			}
			else
			{
				tmppt.y = tmpLine.slope + int(((prevslope - tmpLine.slope)/(M_PI)) + 0.5) * M_PI;
			}

			prevslope = tmppt.y;

			FirstOrderslopes.push_back(tmppt);
			FirstOrderslopesStartPts.push_back(tmpLine.start_pt);
			FirstOrderslopesEndPts.push_back(tmpLine.end_pt);
		
			PA.clear();
		}

		//s is total length of curve.  any radius of curvature higher than this is assumed as a straight line
		//this gives rise to below cutoff value for separating lines from arcs.
		if (max_radius == 50)
			lower_cutoff = 1 / s;
		else
			lower_cutoff = 1 / max_radius;

		upper_cutoff = 1 / min_radius;

		//filter out noise by taking mean slope (instead of median) of successive few lines (ie successive step_size points of FirstOrderslopes list).
	
		dbllist slope;
		dbllist avg_slopes;
		dbllist::iterator dbl_iter;
		dbllist::iterator dbl_iter2;
		avg_slopes.clear();

		for (i = FirstOrderslopes.begin(); i != FirstOrderslopes.end(); i++)
		{
			avg_slopes.push_back( i -> y);
		}

		dbl_iter2 = avg_slopes.begin();

		PtsList::iterator pt_iter = FirstOrderslopesStartPts.begin();
		PtsList::iterator pt_iter2;
		for (i = FirstOrderslopes.begin(); i != FirstOrderslopes.end(); i++)
		{
			slope.clear();
			j = i;
			pt_iter2 = pt_iter;
			double tot = 0;
			for (m = 0; m < step_size; m++)
			{
				slope.push_back(j -> y);
				tot = tot + j -> y;
				x_curr = pt_iter2->x;
				y_curr = pt_iter2->y;
				j++;
				pt_iter2++;
				if (j == FirstOrderslopes.end()) break;
				x_next = pt_iter2->x;
				y_next = pt_iter2->y;
				if ((abs(y_next - y_curr) > max_dist_betn_neighboring_pts) || (abs(x_next - x_curr) > max_dist_betn_neighboring_pts)) break;
			}
			tot = tot/slope.size();
			dbl_iter = dbl_iter2;
			advance (dbl_iter2, slope.size()/2);
			*dbl_iter2 = tot;
			dbl_iter2 = dbl_iter;
			dbl_iter2++;
		}
		dbl_iter2 = avg_slopes.begin();
		for (i = FirstOrderslopes.begin(); i != FirstOrderslopes.end(); i++)
		{
			i -> y = *dbl_iter2;
			dbl_iter2++;
		}

		avg_slopes.clear();
		slope.clear();
	
		//take 2nd order slopes taking step_size at a time of FirstOrderslopes

		i = FirstOrderslopes.begin();
		PtsList::iterator startpt_iter;
		PtsList::iterator endpt_iter;
		startpt_iter = FirstOrderslopesStartPts.begin();
		endpt_iter = FirstOrderslopesEndPts.begin();
		double curr_s = 0;
		PA.clear();
		while (i != FirstOrderslopes.end())
		{
			//start point and end pt of original set of points for current cluster of line or arc
			Pt start_pt;
			Pt end_pt;
		
			//the second order slopes line obtained would be parallel to x-axis if cluster of points belongs to a straight line, 
			//else it would be inclined at an angle to x-axis.
			LineArc tmpLine;
			tmpLine.slope = atan(2 * upper_cutoff);
			PtsList::iterator pt_iter, pt_iter1, pt_iter2;

			int second_order_slope_step = step_size;
			start_pt = *startpt_iter;
			bool jump_encountered = false;
			while (abs(tan(tmpLine.slope)) >= upper_cutoff)
			{
				if (jump_encountered)
				{
					i++;
					startpt_iter++;
					endpt_iter++;
					start_pt = *startpt_iter;
					second_order_slope_step = step_size;
					jump_encountered = false;
				}
				pt_iter = i;
				pt_iter1 = startpt_iter;
				pt_iter2 = endpt_iter;
				PA.clear();
				//take step_size successive points of FirstOrderslopes to get best fit line.
				for (m=0; m<=second_order_slope_step ; m++)
				{
					PA.push_back(*pt_iter);
					curr_s = pt_iter->x;
					end_pt = *pt_iter2;
					x_curr = pt_iter1->x;
					y_curr = pt_iter1->y;
					pt_iter++;
					pt_iter1++;
					pt_iter2++;
					if (m < second_order_slope_step)
					{
						if (pt_iter == FirstOrderslopes.end()) break;
						x_next = pt_iter1->x;
						y_next = pt_iter1->y;
						if ((abs(y_next - y_curr) > max_dist_betn_neighboring_pts) || (abs(x_next - x_curr) > max_dist_betn_neighboring_pts)) 
						{
							jump_encountered = true;
							break;
						}
					}
				} 
				if (PA.size() >= 2)
				{
					tmpLine = getLine(&PA, lower_cutoff);
				}
				second_order_slope_step++;
				if (pt_iter == FirstOrderslopes.end()) break;
			}

			i = pt_iter;
			startpt_iter = pt_iter1;
			endpt_iter = pt_iter2;

			//3 tries are taken of successive points to confirm if new cluster of points belonging to different line or arc has reached.
			int trial = 3;
		
			//keep trying successive points to check if they fall in current line or arc.  When they don't current line or arc is added
			//to the secondorderslopes list of list of points at end.
			while (i != FirstOrderslopes.end())
			{
				x_next = startpt_iter->x;
				y_next = startpt_iter->y;
				if ((abs(y_next - y_curr) > max_dist_betn_neighboring_pts) || (abs(x_next - x_curr) > max_dist_betn_neighboring_pts)) break;
				//check if point falls in best fit line of current cluster and add the corresponding point falling in line to pointlist 
				//comprising line if it is in the line, else not.
				if (chkPtInSlopesLine(*i,tmpLine, curr_s, tolerance)) 
				{
					PA.push_back(*i);
					curr_s = i->x;
					//keep updating end point of current cluster as more and more points get added to current cluster
					end_pt = *endpt_iter;
					x_curr = startpt_iter->x;
					y_curr = startpt_iter->y;
					startpt_iter++;
					endpt_iter++;
					i++;
					trial = 3;
					continue;
				}
				else 
				{
					//if point falls outside of best fit line, check again by considering best fit line of all 
					//points of current cluster not already considered.
					tmpLine = getLine(&PA, lower_cutoff);
					if (chkPtInSlopesLine(*i,tmpLine, curr_s, tolerance))
					{
						PA.push_back(*i);
						curr_s = i->x;
						end_pt = *endpt_iter;
						x_curr = startpt_iter->x;
						y_curr = startpt_iter->y;
						startpt_iter++;
						endpt_iter++;
						i++;
						trial = 3;
						continue;
					}
					else
					{
						/*if point falls outside of best fit line, allow total of 3 trials to take care of "noise" points.*/
						trial -=1;
						if (trial >= 0) 
						{
							PA.push_back(*i);
							curr_s = i->x;
							end_pt = *endpt_iter;
							x_curr = startpt_iter->x;
							y_curr = startpt_iter->y;
							startpt_iter++;
							endpt_iter++;
							i++;
							continue;
						}
						else
						{
							//if three points are outside best fit line, go back by count
							//of 3 for i, and break from inner while loop.
							advance (i,-3);
							advance (startpt_iter, -3);
							advance (endpt_iter, -3);
							//remove last 3 points that have been added to check for "noise".
							PA.pop_back();
							PA.pop_back();
							PA.pop_back();
							curr_s = i->x;

							//below is required so that we get end_pt of previous cluster
							advance (endpt_iter, -1);
							end_pt = *endpt_iter;
							advance (endpt_iter, 1);

							break;
						}
					}
				}
			}

			//end point of original list of points for current cluster of line or arc is stored in end_pt.
		
			if (PA.size() >= 2)
			{
				tmpLine = getLine(&PA, lower_cutoff);
		
				//overwrite the start and end points of tmpLine (which is a line of slopes versus distance) 
				//with the start and end points from original points list

				tmpLine.end_pt = end_pt;
				tmpLine.start_pt = start_pt;
				ptr_SecondOrderslopes->push_back(tmpLine);
			}
			PA.clear();
		}
		FirstOrderslopes.clear();
		FirstOrderslopesStartPts.clear();
		FirstOrderslopesEndPts.clear();
		return 0;
	}
	catch(...){SetAndRaiseErrorMessage("RBF0033", __FILE__, __FUNCSIG__);return 1;}
}

bool RBF::getLengthofShape(double* pts, int PtsCount, double* p_length, int step, bool sorted)
{
	try
	{
		if (step == -1)
		{
			step = PtsCount/20;
			if (step == 0 ) step = 1;
			if (step > 5 ) step = 5;
		}
		int m = 0;
		PtsList tempptslist;
		Pt tmppt;
		for (m = 0; m < PtsCount; m++)
		{
			tmppt.x = *(pts + 2 * m); tmppt.y = *(pts + 2 * m + 1); tmppt.pt_index = m;
			tempptslist.push_back(tmppt);
		}
		double dist_threshold = 0;

		//used to calculate distance threshold
		bool firsttime = true;
		if (sorted == false)
		{
			orderpoints(&tempptslist, &dist_threshold);
			dist_threshold *= 10; //average distance between points times 10 is taken as threshold for distance betn neighboring pts
			firsttime = false; //need not calculate distance threshold again in this case since we have already calculated above.
		}
		PtsList PA;	

		PtsList::iterator i,j, test_iter;
		double s = 0;
		double x_curr = 0;
		double y_curr = 0;
		double x_next = 0;
		double y_next = 0;
		for (i = tempptslist.begin(); i != tempptslist.end(); advance(i,step))
		{
			j = i;
			
			for (m = 0; m <= step; m++)
			{ 
				//add points to points list
				x_curr = j->x;
				y_curr = j->y;
				PA.push_back(*j);
				++j;
				if (j == tempptslist.end()) 
					break;
				x_next = j->x;
				y_next = j->y;
			}

			//following check done to break from loop if we are within step of end of pts list, so that we don't run into debug assertion failure
			test_iter = i;
			for (m = 0; m < step; m++)
			{
				test_iter++;
				if (test_iter == tempptslist.end()) 
					break;
			}
			if (test_iter == tempptslist.end()) 
				break;

			if (PA.size() < 2) continue;

			LineArc tmpLine = getLine(&PA, 0);

			if (firsttime)
			{
				dist_threshold = tmpLine.length * 10;
				firsttime = false;
			}
			if (tmpLine.length < dist_threshold)
			{
				//s is total length of path from start including current segment
				s += tmpLine.length;	
			}
			PA.clear();
		}

		*p_length = s;
		tempptslist.clear();
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0033.b", __FILE__, __FUNCSIG__);
		return false;
	}

}

// function that will return two tangents to circles C1 and C2 respectively that are distance "dist" apart and on same direction w.r.t. two centers.
// dist passed has to be positive if Tangent1 intersects C2 and negative if it does not intersect C2
bool RBF::fnparallel_tangent_2_arcs(LineArc_Circle C1, LineArc_Circle C2, double dist, double line_length, double mouseover_x, double mouseover_y, LineArc* Tangent1, LineArc* Tangent2)
{
	try
	{
		if (C1.Center_X > C2.Center_X)
		{
			LineArc_Circle temp = C1;
			C1 = C2;
			C2 = temp;
		}
		double center_offset = dist + C1.radius - C2.radius;

		double dist_betn_centers = sqrt (pow(C1.Center_X - C2.Center_X,2) + pow(C1.Center_Y - C2.Center_Y,2));

		double alpha;

		//if arc sine cannot be calculated return error.
		if (abs(center_offset) > abs(dist_betn_centers))
			return false;
	
		alpha = asin (center_offset/dist_betn_centers);

		double slope_betn_centers =0;

		if (C1.Center_X - C2.Center_X == 0)
		{
			if (C2.Center_Y - C1.Center_Y >=0) 
				slope_betn_centers = M_PI * 0.5;
			else
				slope_betn_centers = - M_PI * 0.5;
		}
		else
		{
			slope_betn_centers = atan((C1.Center_Y - C2.Center_Y)/(C1.Center_X - C2.Center_X));
		}

		double tangent_slope1 = slope_betn_centers + alpha;
		double tangent_slope2 = slope_betn_centers - alpha;
		double tangent_slope;

		double Tangent1_Pt_X;
		double Tangent1_Pt_Y;

		double Tangent2_Pt_X;
		double Tangent2_Pt_Y;

		double Tangent1_Pt_X_1 = C1.Center_X + (C1.radius * sin (tangent_slope1));
		double Tangent1_Pt_Y_1 = C1.Center_Y - (C1.radius * cos (tangent_slope1));

		double Tangent1_Pt_X_2 = C1.Center_X - (C1.radius * sin (tangent_slope2));
		double Tangent1_Pt_Y_2 = C1.Center_Y + (C1.radius * cos (tangent_slope2));

		double Tangent2_Pt_X_1 = C2.Center_X + (C2.radius * sin (tangent_slope1));
		double Tangent2_Pt_Y_1 = C2.Center_Y - (C2.radius * cos (tangent_slope1));

		double Tangent2_Pt_X_2 = C2.Center_X - (C2.radius * sin (tangent_slope2));
		double Tangent2_Pt_Y_2 = C2.Center_Y + (C2.radius * cos (tangent_slope2));

		double mouse_over_1 = abs((mouseover_y - Tangent1_Pt_Y_1) * cos (tangent_slope1) - (mouseover_x - Tangent1_Pt_X_1) * sin (tangent_slope1));
		double mouse_over_2 = abs((mouseover_y - Tangent1_Pt_Y_2) * cos (tangent_slope2) - (mouseover_x - Tangent1_Pt_X_2) * sin (tangent_slope2));

		//depending on where mouseover point is, choose the nearer tangent lines.
		if (mouse_over_1 <= mouse_over_2)
		{
			Tangent1_Pt_X = Tangent1_Pt_X_1;
			Tangent1_Pt_Y = Tangent1_Pt_Y_1;
			Tangent2_Pt_X = Tangent2_Pt_X_1;
			Tangent2_Pt_Y = Tangent2_Pt_Y_1;
			tangent_slope = tangent_slope1;
		}
		else
		{
			Tangent1_Pt_X = Tangent1_Pt_X_2;
			Tangent1_Pt_Y = Tangent1_Pt_Y_2;
			Tangent2_Pt_X = Tangent2_Pt_X_2;
			Tangent2_Pt_Y = Tangent2_Pt_Y_2;
			tangent_slope = tangent_slope2;
		}

		Tangent1->start_pt.x = Tangent1_Pt_X - (line_length * cos(tangent_slope) * 0.5);
		Tangent1->start_pt.y = Tangent1_Pt_Y - (line_length * sin(tangent_slope) * 0.5);
		Tangent1->end_pt.x = Tangent1_Pt_X + (line_length * cos(tangent_slope) * 0.5);
		Tangent1->end_pt.y = Tangent1_Pt_Y + (line_length * sin(tangent_slope) * 0.5);
		Tangent1->mid_pt.x = Tangent1_Pt_X;
		Tangent1->mid_pt.y = Tangent1_Pt_Y;
		Tangent1->slope = tangent_slope;
		Tangent1->length = line_length;

		Tangent2->start_pt.x = Tangent2_Pt_X - (line_length * cos(tangent_slope) * 0.5);
		Tangent2->start_pt.y = Tangent2_Pt_Y - (line_length * sin(tangent_slope) * 0.5);
		Tangent2->end_pt.x = Tangent2_Pt_X + (line_length * cos(tangent_slope) * 0.5);
		Tangent2->end_pt.y = Tangent2_Pt_Y + (line_length * sin(tangent_slope) * 0.5);
		Tangent2->mid_pt.x = Tangent2_Pt_X;
		Tangent2->mid_pt.y = Tangent2_Pt_Y;
		Tangent2->slope = tangent_slope;
		Tangent2->length = line_length;

		if (abs(tangent_slope) == M_PI * 0.5)
		{
			Tangent1->intercept = Tangent1_Pt_X;

			Tangent2->intercept = Tangent2_Pt_X;
		}
		else
		{
			Tangent1->intercept = Tangent1_Pt_Y - (Tangent1_Pt_X * tan(tangent_slope));

			Tangent2->intercept = Tangent2_Pt_Y - (Tangent2_Pt_X * tan(tangent_slope));
		}

		return true;
	}
	catch (...)
	{
		SetAndRaiseErrorMessage("RBF0034", __FILE__, __FUNCSIG__);
		return false;
	}
}

int RBF::fncalibration2(double actual_width, int num_Cell_X, int num_Cell_Y, double* dbl_array, double* err_array)
{
	try
	{
		//take array of double having measured width in x, y, and 45 degrees and 
		//create error correction in x, y for each of the grid cells.  
		//populate the error correction in an array of dimension 2*(num_Cell_X+1)*(num_Cell_Y+1) 
		//There are totally (num_Cell_X+1) * (num_Cell_Y+1) cells.
		//actual_width is value of the width as per specifications.
	
		double width_x = 0;
		double width_y = 0;
		double width_45 = 0;
		double err_x = 0;
		double err_y = 0;
		double err_45 = 0;
		double err_x_diff = 0;
	
		memset(err_array,0,2*(num_Cell_X+1)*(num_Cell_Y+1)*sizeof(double));

		//error along x-axis is calculated below.  
		for (int i = 1; i < num_Cell_X+1 ; i++)
		{
			width_x = *(dbl_array + 3*(i-1));
			err_x = actual_width - width_x;

			//error at a given x-axis point is calculated by adding previous error to current error.
			*(err_array + 2*i) = *(err_array + 2*(i-1)) + err_x;
		}

		//error along y-axis is calculated below.  this has error x and in y-coordinate. - fix on 29th Nov 2011 to do x correction as well
		for (int i = 1; i < num_Cell_Y+1; i++)
		{
			width_y = *(dbl_array + 3*num_Cell_X*(i-1) + 1);
			err_y = actual_width - width_y;

			//skew error due to non-perpendicularity of x and y axis calculated below - causes error in x value along y axis
			width_x = *(dbl_array + 3*num_Cell_X*(i-1));
			width_45 = *(dbl_array + 3*num_Cell_X*(i-1) + 2);

			err_45 = width_x + width_y - 2 * width_45;

			//error at a given y-axis point is calculated by adding previous error to current error
			*(err_array + 2*(num_Cell_X+1)*i) = *(err_array + 2*(num_Cell_X+1)*(i-1)) + err_45;
			*(err_array + 2*(num_Cell_X+1)*i+1) = *(err_array + 2*(num_Cell_X+1)*(i-1)+1) + err_y;
		}

		//error for intermediate lattice points is calculated by stepping one lower in x and y directions and 
		//adding error within the grid obtained through first 3 elements of dbl_array. 
		//- fix on 29th Nov 2011 to only add x and y errors to intermediate lattice points (ignore 45 deg error because it is already accounted for along y axis)
		for (int i=1; i < num_Cell_Y+1; i++)
		{
			for (int j=1; j < num_Cell_X+1; j++)
			{
				if (i < num_Cell_Y)
				{
					width_x = *(dbl_array + 3*num_Cell_X*i+ 3*(j-1));
				}
				else
				{
					width_x = *(dbl_array + 3*num_Cell_X*(i-1) + 3*(j-1));
				}

				if (j < num_Cell_X)
				{
					width_y = *(dbl_array + 3*num_Cell_X*(i-1) + 3*j + 1);
				}
				else
				{
					width_y = *(dbl_array + 3*num_Cell_X*(i-1) + 3*(j-1) + 1);
				}
			
				err_x = actual_width - width_x;
				err_y = actual_width - width_y;

				*(err_array + 2*(num_Cell_X+1)*i + 2*j) = *(err_array + 2*(num_Cell_X+1)*i + 2*(j-1)) 
														+ err_x;

				*(err_array + 2*(num_Cell_X+1)*i + 2*j +1) = *(err_array + 2*(num_Cell_X+1)*(i-1) + 2*j +1)
														+ err_y;
			}
		}

		//change on 6th Dec 2011.  Use the 45 deg skew along x-axis to calculate relative rotation of adjacent lattice coord system wrt one to the left in x axis.
		//namely 45 deg skew error at a given grid cell minus the x error difference between the points along y axis of the grid cell is equal to y error difference 
		//between adjacent points along x axis due to rotation.
		err_y = 0;
		for (int i = 0; i < num_Cell_X; i++)
		{
			err_x_diff = *(err_array + 2*(num_Cell_X+1) + 2 * i) - *(err_array + 2 * i);
		
			//fix on 6th Dec to also use skew along x axis along with error difference of x between x axis and first parallel line to determine y error along x axis.
			//the difference between the two is explained by a rotation of adjacent grid cell resulting in y error along x axis.
			width_x = *(dbl_array + 3*i);
			width_y = *(dbl_array + 3*i + 1);
			width_45 = *(dbl_array + 3*i + 2);
			err_45 = width_x + width_y - 2 * width_45;
	
			//error in y is cumulatively added to get total y-error along x axis.
			err_y += err_45 - err_x_diff;
			//this error in y added to all points at x = i + 1  for y = j from 0 to num_Cell_Y + 1
			for (int j = 0; j < num_Cell_Y + 1; j++)
			{
				*(err_array + 2 * (num_Cell_X + 1) * j + 2 * (i + 1) + 1) += err_y;
			}
		}

		return 0;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0035", __FILE__, __FUNCSIG__);return 1;}
}
int RBF::setErrorCorrectionsArray2 (double actual_width, int num_Cell_X, int num_Cell_Y, double* err_array)
{
	try
	{
		calibration_width = actual_width;
		X_num = num_Cell_X;
		Y_num = num_Cell_Y;
		if (error_array!=NULL) free (error_array);
		error_array = (double*) malloc (2*(X_num+1)*(Y_num+1)*sizeof(double));
		memset(error_array, 0, 2*(X_num+1)*(Y_num+1)*sizeof(double));
		memcpy(error_array, err_array,2*(X_num+1)*(Y_num+1)*sizeof(double));

		return 0;
	}
	catch(...){ SetAndRaiseErrorMessage("RBF0036", __FILE__, __FUNCSIG__);return 1;}
}
int RBF::correctXYCoord2 (double* x, double* y, bool getRawFromCorrected)
{
	try
	{
		int i_x=0;
		int i_y=0;
		double x_pos =0;
		double y_pos =0;
		double err_x=0;
		double err_y=0;
		double Vx[4]={0};
		double Vy[4]={0};

		//if error corrections array not set, return error code of 1
		if ((X_num == 0) || (Y_num ==0) || (error_array == NULL) || (calibration_width ==0))
			return 1;
		x_pos = *x;
		y_pos = *y;

		//determine grid that we fall in based on x, y coordinates and determine the error values 
		//in x and y directions at the 4 grid points to be used for extrapolation for other x.

		i_x = int(x_pos/calibration_width);
		i_y = int(y_pos/calibration_width);

		if (i_x >= X_num) i_x = X_num - 1;
		if (i_y >= Y_num) i_y = Y_num - 1;

		//this is done only for safety purpose - this condition should never have happened...
		if (i_x <= 0) i_x = 0;
		if (i_y <= 0) i_y = 0;

		Vx[0] = *(error_array + 2*((X_num+1)*i_y + i_x));
		Vx[1] = *(error_array + 2*((X_num+1)*i_y + i_x+1));
		Vx[2] = *(error_array + 2*((X_num+1)*(i_y+1) + i_x+1));
		Vx[3] = *(error_array + 2*((X_num+1)*(i_y+1) + i_x));

		Vy[0] = *(error_array + 2*((X_num+1)*i_y + i_x) + 1);
		Vy[1] = *(error_array + 2*((X_num+1)*i_y + i_x+1) + 1);
		Vy[2] = *(error_array + 2*((X_num+1)*(i_y+1) + i_x+1) + 1);
		Vy[3] = *(error_array + 2*((X_num+1)*(i_y+1) + i_x) + 1);
			
		//linear interpolation of error in x and y based on values at the 4 grid points
		err_x = Vx[0] + (Vx[1] - Vx[0]) * (x_pos - calibration_width * i_x ) / calibration_width
				+ (	(Vx[3] + (Vx[2] - Vx[3]) * (x_pos - calibration_width * i_x ) / calibration_width)
					-(Vx[0] + (Vx[1] - Vx[0]) * (x_pos - calibration_width * i_x ) / calibration_width) )
				* (y_pos - calibration_width * i_y ) / calibration_width;

		err_y = Vy[0] + (Vy[1] - Vy[0]) * (x_pos - calibration_width * i_x ) / calibration_width
				+ (	(Vy[3] + (Vy[2] - Vy[3]) * (x_pos - calibration_width * i_x ) / calibration_width)
					-(Vy[0] + (Vy[1] - Vy[0]) * (x_pos - calibration_width * i_x ) / calibration_width) )
				* (y_pos - calibration_width * i_y ) / calibration_width;

		if (getRawFromCorrected == true)
		{
			err_x = - err_x;
			err_y = - err_y;
		}
		*x = x_pos + err_x;
		*y = y_pos + err_y;

		return 0;
	}
	catch(...){SetAndRaiseErrorMessage("RBF0037", __FILE__, __FUNCSIG__); return 1;}
}

bool RBF::fnOneShotCalibration(double actual_width, int num_Cell_X, int num_Cell_Y, double* dbl_array, double* err_array)
{
	try
	{
		//take array of double having measured width in x and y, and 
		//create error correction in x, y for each of the grid cells.  
		//populate the error correction in an array of dimension 2*(num_Cell_X+1)*(num_Cell_Y+1) 
		//There are totally (num_Cell_X+1) * (num_Cell_Y+1) cells.
		//actual_width is value of the width as per specifications.
	
		double width_x = 0;
		double width_y = 0;
		double err_x = 0;
		double err_y = 0;
	
		memset(err_array,0,2*(num_Cell_X+1)*(num_Cell_Y+1)*sizeof(double));

		//error along x-axis is calculated below.  
		for (int i = 1; i < num_Cell_X+1 ; i++)
		{
			width_x = *(dbl_array + 2*(i-1));
			err_x = actual_width - width_x;

			//error at a given x-axis point is calculated by adding previous error to current error.
			*(err_array + 2*i) = *(err_array + 2*(i-1)) + err_x;
		}

		//error along y-axis is calculated below.
		for (int i = 1; i < num_Cell_Y+1; i++)
		{
			width_y = *(dbl_array + 2*num_Cell_X*(i-1) + 1);
			err_y = actual_width - width_y;

			//error at a given y-axis point is calculated by adding previous error to current error
			*(err_array + 2*(num_Cell_X+1)*i+1) = *(err_array + 2*(num_Cell_X+1)*(i-1)+1) + err_y;
		}

		//error for intermediate lattice points is calculated by stepping one lower in x and y directions and 
		//adding error within the grid obtained through first 2 elements of dbl_array. 
		for (int i=1; i < num_Cell_Y+1; i++)
		{
			for (int j=1; j < num_Cell_X+1; j++)
			{
				if (i < num_Cell_Y)
				{
					width_x = *(dbl_array + 2*num_Cell_X*i+ 2*(j-1));
				}
				else
				{
					width_x = *(dbl_array + 2*num_Cell_X*(i-1) + 2*(j-1));
				}

				if (j < num_Cell_X)
				{
					width_y = *(dbl_array + 2*num_Cell_X*(i-1) + 2*j + 1);
				}
				else
				{
					width_y = *(dbl_array + 2*num_Cell_X*(i-1) + 2*(j-1) + 1);
				}
			
				err_x = actual_width - width_x;
				err_y = actual_width - width_y;

				*(err_array + 2*(num_Cell_X+1)*i + 2*j) = *(err_array + 2*(num_Cell_X+1)*i + 2*(j-1)) 
														+ err_x;

				*(err_array + 2*(num_Cell_X+1)*i + 2*j +1) = *(err_array + 2*(num_Cell_X+1)*(i-1) + 2*j +1)
														+ err_y;
			}
		}

		//get error at mid point of field of view and normalize error array by making this error equal to 0.
		int i_x = int(num_Cell_X/2);
		int i_y = int(num_Cell_Y/2);
		double Vx[4]={0};
		double Vy[4]={0};

		Vx[0] = *(err_array + 2*((num_Cell_X+1)*i_y + i_x));
		Vx[1] = *(err_array + 2*((num_Cell_X+1)*i_y + i_x+1));
		Vx[2] = *(err_array + 2*((num_Cell_X+1)*(i_y+1) + i_x+1));
		Vx[3] = *(err_array + 2*((num_Cell_X+1)*(i_y+1) + i_x));

		Vy[0] = *(err_array + 2*((num_Cell_X+1)*i_y + i_x) + 1);
		Vy[1] = *(err_array + 2*((num_Cell_X+1)*i_y + i_x+1) + 1);
		Vy[2] = *(err_array + 2*((num_Cell_X+1)*(i_y+1) + i_x+1) + 1);
		Vy[3] = *(err_array + 2*((num_Cell_X+1)*(i_y+1) + i_x) + 1);

		//linear interpolation of error in x and y based on values at the 4 grid points
		err_x = Vx[0] + (Vx[1] - Vx[0]) * (num_Cell_X/2 - i_x ) 
				+ (	(Vx[3] + (Vx[2] - Vx[3]) * (num_Cell_X/2 - i_x ))
					-(Vx[0] + (Vx[1] - Vx[0]) * (num_Cell_X/2 - i_x ) ))
				* (num_Cell_Y/2 - i_y );

		err_y = Vy[0] + (Vy[1] - Vy[0]) * (num_Cell_X/2 - i_x ) 
				+ (	(Vy[3] + (Vy[2] - Vy[3]) * (num_Cell_X/2 - i_x )) 
					-(Vy[0] + (Vy[1] - Vy[0]) * (num_Cell_X/2 - i_x )  ))
				* (num_Cell_Y/2 - i_y );

		for (int i=0; i < num_Cell_Y+1; i++)
		{
			for (int j=0; j < num_Cell_X+1; j++)
			{
				*(err_array + 2*(num_Cell_X+1)*i + 2*j) -= err_x;
				*(err_array + 2*(num_Cell_X+1)*i + 2*j +1) -= err_y;
			}
		}
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0035.1", __FILE__, __FUNCSIG__);return false;}
}

bool RBF::getZError (int x_num, int y_num, double cell_width, double* z_err_matrix, double* z_calib_array, double x, double y, double* z_err, int R_num, double* R_array, double R)
{
	try
	{
		//determine R-range we fall in based on passed in R value
		double R_i = 0;
		double R_i_next = 0;
		double alpha = 0;
		int i_r = 0;
		if (R_num > 1)
		{
			R_i = R_array[R_num - 2];
			R_i_next = R_array[R_num - 1];
			i_r = R_num - 2;
			for (int i = 0; i < R_num - 1; i++)
			{
				if (R < R_array[i])
				{
					R_i = R_array[i];
					R_i_next = R_array[i+1];
					i_r = i;
					break;
				}
			}
			if (R_i_next > R_i)
				alpha = (R - R_i)/(R_i_next - R_i);
		}
		//determine grid that we fall in based on x, y coordinates and determine the z error values 
		//at the 4 grid points to be used for extrapolation for other x, y.


		int i_x = int(x/cell_width - 0.5);
		int i_y = int(y/cell_width - 0.5);

		if (i_x == x_num - 1) i_x = x_num - 2;
		if (i_y == y_num - 1) i_y = y_num - 2;

		if (i_x == -1) i_x = 0;
		if (i_y == -1) i_y = 0;

		//error values at the 4 lattice points stored below

		double Vz[4] = {0};

		Vz[0] = *(z_err_matrix + i_r * x_num * y_num + x_num * i_y + i_x);
		Vz[1] = *(z_err_matrix + i_r * x_num * y_num + x_num * i_y + i_x + 1);
		Vz[2] = *(z_err_matrix + i_r * x_num * y_num + x_num * (i_y + 1) + i_x + 1);
		Vz[3] = *(z_err_matrix + i_r * x_num * y_num + x_num * (i_y + 1) + i_x);
		if (R_num > 1)
		{
			double Vz_2[4] = {0};
			Vz_2[0] = *(z_err_matrix + (i_r + 1) * x_num * y_num + x_num * i_y + i_x);
			Vz_2[1] = *(z_err_matrix + (i_r + 1) * x_num * y_num + x_num * i_y + i_x + 1);
			Vz_2[2] = *(z_err_matrix + (i_r + 1) * x_num * y_num + x_num * (i_y + 1) + i_x + 1);
			Vz_2[3] = *(z_err_matrix + (i_r + 1) * x_num * y_num + x_num * (i_y + 1) + i_x);

			Vz[0] = (1 - alpha) * Vz[0] + alpha * Vz_2[0];
			Vz[1] = (1 - alpha) * Vz[1] + alpha * Vz_2[1];
			Vz[2] = (1 - alpha) * Vz[2] + alpha * Vz_2[2];
			Vz[3] = (1 - alpha) * Vz[3] + alpha * Vz_2[3];
		}

		//x and y wrt nearest lattice cell
		double delta_x = 0;
		double delta_y = 0;

		delta_x = x - cell_width * (i_x + 0.5);
		delta_y = y - cell_width * (i_y + 0.5);

		//linear interpolation of z error at intermediate points based on values at the 4 grid points
		//and delta_x, delta_y
		*z_err = Vz[0] + (Vz[1] - Vz[0]) * delta_x  / cell_width
				+ (	Vz[3] + (Vz[2] - Vz[3]) * delta_x / cell_width
					- Vz[0] - (Vz[1] - Vz[0]) * delta_x / cell_width ) * delta_y / cell_width;

		//now apply z_calib_array correction on top of previous correction
		double X_0 = z_calib_array[0];
		double Y_0 = z_calib_array[1];
		double shift_d_x = z_calib_array[2];
		double shift_d_y = z_calib_array[3];

		*z_err += shift_d_x * x / X_0 + shift_d_y * y / Y_0;
		return true;
	}
	catch (...) {SetAndRaiseErrorMessage("RBF0038", __FILE__, __FUNCSIG__); return false;}
}

//following are auto-orientation detection functions

bool RBF::setOneShotImage(BYTE* pixelArray, int width, int height, bool singlechannel, int deadPixelWidth, int binarizingCutoff, int scale, bool recalculateLRdeadPixWidth, bool ThisisObjectImage, bool IsProfileImage)
{
	try
	{
		//A_ref = 0;
		double CG_Dir_Magnitude = 0;
		BYTE* currentImage; 
		int xstep = 2;
		BYTE whiteVal = 2, BlackVal = 0;
		if (!IsProfileImage)
		{	// Added by R A N 24 Jun 2016. For doing OneShot measurements with surface light
			//We expect to have a white component in a black background. 
			whiteVal = 0;
			BlackVal = 2;
		}

		if (scale == 0)	scale = 1;

		if (singlechannel)
			M_ARRAY = width / (2 * scale);
		else
		{
			M_ARRAY = width * 3 / (2 * scale);
			xstep = 3;
		}
		N_ARRAY = height / (2 * scale);

		int arrayLength = 4 * M_ARRAY * N_ARRAY * sizeof(BYTE);

		if (ThisisObjectImage)
		{
			if (ObjImage != NULL) free(ObjImage);
			ObjImage = (BYTE*)malloc(arrayLength);
			currentImage = ObjImage;
		}
		else
		{
			if (RefImage != NULL) free(RefImage);
			RefImage = (BYTE*)malloc(arrayLength);
			currentImage = RefImage;
		}
		
		ZeroMemory(currentImage, arrayLength);
		//memset(RefImage,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));

		BYTE* tem = (BYTE*) malloc(arrayLength);
		//memset(tem,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		ZeroMemory(tem, arrayLength);

		int leftDeadPixWidth = deadPixelWidth;
		int rightDeadPixWidth = deadPixelWidth;
		int startPos = 0, endPos = 0;
		scale = 4;

		//Check to see if there is any fixture in the first row of pixels only. 
		//We will get pixel values jumping from black to white the moment we have 
		//edge of the fixture. We do two scans, one from the left and one from the right
		//This will fail when we have a horizontal fixture !!!
		if (recalculateLRdeadPixWidth)
		{
			startPos = deadPixelWidth * xstep;
			endPos = 2 * M_ARRAY * scale - startPos;
			BYTE* tempImgPos = pixelArray + 2 * deadPixelWidth * M_ARRAY * scale;
			for (int i = startPos; i < endPos; i += xstep)
			{
				if (*(tempImgPos + i) > binarizingCutoff)
				{
					leftDeadPixWidth = i / xstep;
					break;
				}
			}
			for (int i = 2 * M_ARRAY * scale - deadPixelWidth * xstep - xstep; i >=0; i-= xstep)
			{
				if (*(tempImgPos + i) > binarizingCutoff)
				{
					rightDeadPixWidth = (2 * M_ARRAY * scale - i) / xstep - 1;
					break;
				}
			}
		}
		//Now let us scan through the image from relevant areas...
		endPos = 2 * N_ARRAY * scale - deadPixelWidth;
		int thisStepSize = xstep * scale;
		startPos = 2 * M_ARRAY * scale - rightDeadPixWidth * xstep;
		//BYTE* currImgPos;
		int currImgIndex = 0, w_count = 0, b_count = 0;
		//Binarise the image into the tem location
		for (int j = deadPixelWidth; j < endPos; j += scale)
		{
			currImgIndex = 2 * j * M_ARRAY / scale + leftDeadPixWidth / scale;
			for (int i = leftDeadPixWidth * xstep; i < startPos; i += thisStepSize)
			{
				currImgIndex += xstep;
				//i_step += xstep;
				for (int a = 0; a < xstep; a++)
				{
					//Alert: Notice that for profile light images, we have  a black component on a white component
					//This algorithm inverts this! It will not invert for Surface light images since there we expect
					//to see a white component in a black background
					if (*(pixelArray + currImgIndex) > binarizingCutoff)
					{
						*(tem + currImgIndex + a) = BlackVal;
						b_count++;
					}
					else
					{
						*(tem + currImgIndex + a) = whiteVal;
						w_count++;
					}
				}
			}
		}
			//Scan through the whole image now, and denoise in a 5x5 mask by requiring atleast 50%  of pixels to be white...
			//Else its noise, and the refImage will make that pixel black (here BLACK = 0; WHITE = 2)
			//For Surface light components, we have retained white to be white and black to be black. So component is white. 
		endPos = 2 * N_ARRAY - deadPixelWidth / scale;
		startPos = 2 * M_ARRAY - rightDeadPixWidth * xstep / scale;
		int titerator = 0;
		int MaskSize = 5, mLeft = 0;
		mLeft = -1 * (MaskSize - 1);
		int endPosInnerLoop = MaskSize * xstep;
		int LoopCuttoffValue = MaskSize * MaskSize * xstep; //We are doing white as 2, not one. So if all pixels in the mask are white, we will get 2 * square of masksize
		int DeNoiseCount = 1;
		//While cleaning and computing, we generate the A-Transpose A so that we can calculate the eigen vectors and eigen values. 
		double X_sq_Sum = 0, Y_sq_Sum = 0, X_Y_Sum = 0;
		double m_arr_Sq = M_ARRAY * M_ARRAY * 4;
		double n_arr_Sq = N_ARRAY * N_ARRAY * 4;
		double n_m_Prod = M_ARRAY * N_ARRAY * 4;

		for (int dd = 0; dd < DeNoiseCount; dd++)
		{
			CG_Dir_Magnitude = 0;
			for (int j = deadPixelWidth / scale; j < endPos; j++)
			{
				for (int i = leftDeadPixWidth * xstep / scale; i < startPos; i += thisStepSize)
				{
					int k = 0;
					for (int l = mLeft; l < MaskSize; l++)
					{
						titerator = 2 * (j + l) * M_ARRAY + i;
						for (int m = -2 * xstep; m < endPosInnerLoop; m += xstep)
						{
							for (int a = 0; a < xstep; a++)
							{
								k += *(tem + titerator + m + a);
							}
						}
					}
					titerator = 2 * j * M_ARRAY + i;
					if (k > LoopCuttoffValue)
					{
						for (int l = 0; l < xstep; l++)
						{
							*(currentImage + titerator + l) = 2;
						}
					}
					else
					{
						for (int l = 0; l < xstep; l++)
						{
							*(currentImage + titerator + l) = 0;
						}
					}
					//Vector Magnitude / Direction
					CG_Dir_Magnitude += *(currentImage + titerator) * *(currentImage + titerator) * xstep;
				}
			}
			memcpy(tem, currentImage, arrayLength);
		}


		//std::ofstream ifile;
		//std::string filename = "E:\\RefImg_Processed.csv";
		//ifile.open(filename.c_str());

		//for (int jj = 0; jj < height; jj++)
		//{
		//	currImgIndex = jj * width;
		//	for (int ii = 0; ii < width; ii++)
		//	{
		//		ifile << (int)*(currentImage + currImgIndex + ii) << ", ";
		//	}
		//	ifile << endl;
		//}
		//ifile.close();

		//wchar_t ab[100];
		//int j = swprintf_s(ab, L"Ans :  %f\n", CG_Dir_Magnitude);
		//MessageBox(NULL, ab, TEXT("RBF"), NULL);
		int ti = 0, tj = 0;
		if (ThisisObjectImage)
		{
			hM->Calculate_CG(ObjImage, 2 * M_ARRAY, 2 * N_ARRAY, &x_cg_obj, &y_cg_obj, &orientation_obj);
			A_obj = CG_Dir_Magnitude;
		}
		else
		{
			hM->Calculate_CG(RefImage, 2 * M_ARRAY, 2 * N_ARRAY, &x_cg_ref, &y_cg_ref, &orientation_ref);
			A_ref = CG_Dir_Magnitude;
		}
		free (tem);
		return true;
	}
	catch (...)
	{
		SetAndRaiseErrorMessage("RBF0039", __FILE__, __FUNCSIG__); return false;
	}
}
bool RBF::setObjImage (BYTE* pixelArray, int width, int height, bool singlechannel, int deadPixelWidth, int binarizingCutoff, int scale, bool recalculateLRdeadPixWidth)
{
	try
	{
		A_obj = 0;
		int xstep = 1;
		if (scale == 0)
			scale = 1;
		if (!singlechannel)
		{
			xstep = 3;
		}

		if ((M_ARRAY != width * xstep / (2 * scale)) || (N_ARRAY != height / (2 * scale))) 
			return false;
	
		if (ObjImage != NULL) free(ObjImage);

		ObjImage = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memset(ObjImage,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));

		BYTE* tem = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memset(tem,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));

		int leftDeadPixWidth = deadPixelWidth;
		int rightDeadPixWidth = deadPixelWidth;
		if (recalculateLRdeadPixWidth)
		{
			for (int i = deadPixelWidth *xstep; i < 2 * M_ARRAY * scale - deadPixelWidth*xstep; i+= xstep)
			{
				if (*(pixelArray + 2 * deadPixelWidth * M_ARRAY * scale + i) > binarizingCutoff)
				{
					leftDeadPixWidth = i / xstep;
					break;
				}
			}
			for (int i = 2 * M_ARRAY * scale - deadPixelWidth*xstep - xstep; i >=0; i-= xstep)
			{
				if (*(pixelArray + 2 * deadPixelWidth * M_ARRAY * scale + i) > binarizingCutoff)
				{
					rightDeadPixWidth = (2 * M_ARRAY * scale - i) / xstep - 1;
					break;
				}
			}
		}
		for (int j=deadPixelWidth; j<2 * N_ARRAY * scale - deadPixelWidth; j+=scale)
		{ 
			for (int i = leftDeadPixWidth *xstep; i < 2 * M_ARRAY * scale - rightDeadPixWidth*xstep ; i+= xstep * scale)
			{
				for (int a = 0; a < xstep; a++)
				{
					if (*(pixelArray + 2*j*M_ARRAY*scale + i + a) > binarizingCutoff)
						*(tem + 2*j*M_ARRAY/scale + i/scale + a) = 0;
					else
						*(tem + 2*j*M_ARRAY/scale + i/scale + a) = 2;
				}
			}
		}

		for (int j=deadPixelWidth/scale; j<2 * N_ARRAY - deadPixelWidth/scale; j++)
		{ 
			for (int i =leftDeadPixWidth*xstep/scale; i < 2 * M_ARRAY -rightDeadPixWidth*xstep/scale; i += xstep )
			{
				int k = 0;
				for (int l = -2; l < 3; l++)
				{
					for (int m = -2*xstep; m < 3*xstep; m +=xstep)
					{
						for (int a = 0; a < xstep; a++)
						{
							k += *(tem + 2*(j+l)*M_ARRAY + i + m + a);
						}
					}
				}
				if (k > 25 * xstep) 
				{
					for (int l = 0; l< xstep; l++)
					{
						*(ObjImage + 2*j*M_ARRAY + i + l) = 2;
					}
				}
				else
				{
					for (int l = 0; l< xstep; l++)
					{
						*(ObjImage + 2*j*M_ARRAY + i + l) = 0;
					}
				}
			
				A_obj += *(ObjImage + 2*j*M_ARRAY + i) * *(ObjImage + 2*j*M_ARRAY + i) * xstep;
			}
		}
		free (tem);
		return true;
	}
	catch(...) { 
		SetAndRaiseErrorMessage("RBF0040", __FILE__, __FUNCSIG__);
		return false;
	}
}
bool RBF::getRefImage (BYTE* pixelArray, int width, int height, bool singlechannel)
{
	try
	{
		int xstep = 1;

		if (!singlechannel)
		{
			xstep = 3;
		}

		if ((M_ARRAY != width * xstep / 2) || (N_ARRAY != height / 2)) return false;

		int k=0;
		int w = 2 * M_ARRAY;
		int h = 2 * N_ARRAY;

		for (int j = 0; j < h; j++)
		{ 
			for (int i = 0; i < w; i++)
			{
				//pixelArray[k] = 255*(1-*(RefImage + k)/2);
				if (*(RefImage + k) == 2)
					pixelArray[k] = 255;
				else
					pixelArray[k] = 0;
				k++;
			}
		}
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0041", __FILE__, __FUNCSIG__);return false;}
}
bool RBF::getObjImage (BYTE* pixelArray, int width, int height, bool singlechannel)
{
	try
	{
		int xstep = 1;

		if (!singlechannel)
		{
			xstep = 3;
		}

		if ((M_ARRAY != width * xstep / 2) || (N_ARRAY != height / 2)) return false;
		int k=0;
		for (int j=0; j<2 * N_ARRAY; j++)
		{ 
			for (int i=0; i < 2 * M_ARRAY; i++)
			{
				pixelArray[k] = 255*(1-*(ObjImage + k)/2);
				k++;
			}
		}
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0042", __FILE__, __FUNCSIG__);return false;}
}

int RBF::RedefineImagesAboutCG (bool both, int* p_xy, bool MatchExactly, int tol_error)
{
	tol_err = tol_error;
	return RedefineImagesAboutCentroid (both, p_xy, MatchExactly);
}
// Method to redefine image about center of gravity - somewhere close to x=M, y=N but may be slightly off. 
// Array consists of points from x=0 to x=2M and y=0 to y=2N.  Post redefinition, center of gravity
// should be exactly x=M, y=N.  retrieve x and y shifts of obj wrt ref ref
int RBF::RedefineImagesAboutCentroid (bool both, int* p_xy, bool exactmatch)
{
	try
	{
		double M_x =0;
		double M_y =0;
		int tot = 0;
		int t, r, t_2 = 0;
		int width = 2 * M_ARRAY;
		int height = 2 * N_ARRAY;

		// error code of 1 to denote area mismatch error
		if (exactmatch)
		{
			if ((abs(A_obj - A_ref) * 100) > (A_ref * tol_err)) 
				return 1;
		}
		if ((RefImage == NULL) || (ObjImage == NULL)) 
			return 1;

		if (TmpImage != NULL) free(TmpImage);
		int TotalImageLength = 4 * M_ARRAY * N_ARRAY;
		TmpImage = (BYTE*) malloc(TotalImageLength * sizeof(BYTE));
		memset(TmpImage, 0, TotalImageLength * sizeof(BYTE));

		int jj = 0;
		// if "both" set to true, do for reference image as well.
		//if (both)
		//{
		//	hM->Calculate_CG(RefImage, width, height, &x_cg_ref, &y_cg_ref);
		//	r = y_cg_ref - N_ARRAY;
		//	t = x_cg_ref - M_ARRAY;
		//	jj = 0;
		//}
		M_x = 0;
		M_y = 0;
		tot = 0;
		jj = 0;
		//hM->Calculate_CG(ObjImage, width, height, &x_cg_obj, &y_cg_obj);

		//change on Mar 15 2012
		r = y_cg_obj - y_cg_ref;
		t = x_cg_obj - x_cg_ref;

		int x_0 = 0;
		int y_0 = 0;
		jj = 0;
		int stride = 0, stepx = 0;
		// if r and t were exactly 0 (ie no shift), then TmpImage would have been identical to ObjImage
		// when they are not exactly 0, TmpImage gives the ObjImage shifted by this shift in x and y directions
		// so finally the ObjImage will have same CG as RefImage
		for (int j = 0; j < height; j++)
		{
			t_2 = t - 1;
			stride = j * width;
			stepx = r * width;
			for (int i = 0; i < width; i++)
			{
				t_2++;
				if (t_2 < 0) continue;
				if (t_2 >= width) continue;
				if (r < 0) break;
				if (r >= height) break;
				*(TmpImage + stride + i) = *(ObjImage + stepx + t_2);
				jj++;
			}
			r++;
		}

		// x_rot_abt and y_rot_abt are global variables that will be used for rotation in subsequent function
		x_rot_abt = x_cg_ref;
		y_rot_abt = y_cg_ref;

		memcpy(ObjImage, TmpImage,TotalImageLength *sizeof(BYTE));
		memset(TmpImage, 1, TotalImageLength * sizeof(BYTE));	
	
		//std::ofstream ifile;
		//std::string filename = "E:\\ObjImg_Shifted.csv";
		//ifile.open(filename.c_str());
		//int currImgIndex = 0;

		//for (int jj = 0; jj < height; jj++)
		//{
		//	currImgIndex = jj * width;
		//	for (int ii = 0; ii < width; ii++)
		//	{
		//		ifile << (int)*(ObjImage + currImgIndex + ii) << ", ";
		//	}
		//	ifile << endl;
		//}
		//ifile.close();


		//ref and obj cg is passed back
		*p_xy = x_cg_ref;
		*(p_xy + 1) = y_cg_ref;
		*(p_xy + 2) = x_cg_obj;
		*(p_xy + 3) = y_cg_obj;
		*(p_xy + 4) = A_ref;
		*(p_xy + 5) = A_obj;

		// return code of 0 to indicate successful completion of method
		return 0;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0043", __FILE__, __FUNCSIG__);return 1;}
}

int RBF::getMaxImageMatch (double* p_Theta, bool MatchExactly, bool singlechannel, int tol_error_ll)
{

	//	return getBestImageMatchAng(p_Theta, MatchExactly, singlechannel, tol_error_ll);
	*p_Theta = orientation_obj - orientation_ref;
	return 0;
}

int RBF::getBestImageMatchAng (double* p_Theta, bool exactmatch, bool singlechannel, int tol_error_ll)
{
	try
	{
		double theta =0;
		A_ref_obj=0;
		double full_span_angle = 2 * M_PI ;
		double A_match =0;
		int l=0;
		double x_rot =0;
		double y_rot =0;
		double x_rot_0 = 0;
		double y_rot_0 = 0;
		double c =0;
		double s =0;
		int xstep = 1;
		int l_step = 5;
		int ang_step = 16;
		int tol_err_ll = tol_err;

		if (tol_error_ll != -1) tol_err_ll = tol_error_ll;
		if (tol_err_ll > tol_err) tol_err_ll = tol_err;
		//if (exactmatch == false)
		//{
		//	//l_step = 3;
		//	ang_step = 8;
		//}
		if (!singlechannel)
		{
			xstep = 3;
		}
		int scale = M_ARRAY/(400 * xstep);
		if (scale == 0)
			scale = 1;
		if ((RefImage == NULL) || (ObjImage == NULL)) return 1;

		if (TmpImage != NULL) free(TmpImage);

		TmpImage = (BYTE*) malloc(4 * M_ARRAY * N_ARRAY * sizeof(BYTE));

		*p_Theta = 0.0;
		memset(TmpImage, 1, 4 * M_ARRAY * N_ARRAY * sizeof(BYTE));
		double theta_0 = 0;
		double theta_1 = 0;
		double alpha = 0, delta_Angle = 0;
		double ix = 0, iy = 0;
		int pixVal = 0;
		int currImgIndex = 0; // , c_ypos = 0;
		int w = 2 * M_ARRAY, h = 2 * N_ARRAY, ww = 0;
		// 
		// if l_step = 5 and ang_step = 16 (say), what is being done is as follows:
		// in first iteration of outer loop, we are rotating object image in steps of 360/16 and at each rotation, calculating
		// the rotated image and checking its overlap with the reference image.
		// the best of the images (having highest overlap) is picked and in next iteration of outer loop, we are rotating object around
		// this best angle in steps of 2 * 360/16^2 = 360/128.  This is proceeded to a max of 5 iterations so that in principle, we are going down
		// to a lowest possible angle step of 360/(8^5*16).  
		// in between if at any point, we end up getting a match within tolerance specified, we exit loop.

		while (l < l_step)
		{
			delta_Angle = full_span_angle / ang_step;
			theta -= delta_Angle;
			for (int k = 0; k < ang_step; k++)
			{
				theta += delta_Angle;
				c = cos (theta); s = sin (theta);

				//change on Mar 15 2012
				x_rot = -c * (x_rot_abt / xstep + scale) + s * (y_rot_abt + scale) + x_rot_abt / xstep;
				y_rot = -s * (x_rot_abt / xstep + scale) - c * (y_rot_abt + scale) + y_rot_abt;

				for (long j = 0; j < h; j += scale)
				{
					x_rot -= s * scale;
					y_rot += c * scale;

					ix = x_rot;	
					iy = y_rot;
					
					ww = w / xstep;
					//c_ypos = 2 * j * M_ARRAY;
					
					for (long i = 0; i < ww; i += scale)
					{
						x_rot += c * scale;
						y_rot += s * scale;
						if (x_rot < 0) continue;
						if (x_rot >= ww - 0.5) continue;
						if (y_rot < 0) continue;
						if (y_rot >= h - 0.5) continue;

						//currImgIndex = c_ypos + xstep * i;

						//for (int z = 0; z < xstep; z++)
						//{
						//	*(TmpImage + currImgIndex + z) = *(ObjImage + w * (int(y_rot + 0.5)) + xstep * (int(x_rot + 0.5)));
						//}

						pixVal = *(ObjImage + w * (int(y_rot + 0.5)) + xstep * (int(x_rot + 0.5))); // *(TmpImage + currImgIndex);
						A_match += xstep * pixVal * pixVal; // *(*(RefImage + 2 * j*M_ARRAY + xstep*i));
					}
					//x_rot -=2*c*M_ARRAY/xstep;
					//y_rot -=2*s*M_ARRAY/xstep;
					x_rot = ix;
					y_rot = iy;
				}
				if (A_match > A_ref_obj) 
				{
					A_ref_obj = A_match;
					*p_Theta = theta;
				
					if (exactmatch)
					{
						if (abs(A_ref_obj * scale * scale - A_ref )*100 < A_ref * tol_err_ll) break;
					}
				}
				A_match = 0;
			}
			full_span_angle = 2 * full_span_angle / ang_step;
			theta = *p_Theta - full_span_angle / 2;
			l++;
			if (abs(A_ref_obj * scale * scale - A_ref ) * 100 < A_ref * tol_err_ll) break;
		}

		wchar_t ab[100];
		int j = swprintf_s(ab, L"Ans :  %f\n", *p_Theta);
		MessageBox(NULL, ab, TEXT("RBF"), NULL);

		if (exactmatch)
		{
			if (abs(A_ref_obj * scale * scale - A_ref )*100 < A_ref * tol_err)
				return 0;
			else
				return 1;
		}
		else
			return 0;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0044", __FILE__, __FUNCSIG__); return 1;}

}
int RBF::getBestImageMatch (int* xy, double* p_Theta, bool singlechannel, bool highaccuracymode)
{
	try
	{
		//redefine images about centroid and get best image match by rotation first.
		int status = RedefineImagesAboutCentroid (true, xy, false);
		if (status == 1) return status;
		status = getBestImageMatchAng (p_Theta, false, singlechannel);
		if (status == 1) return status;

		if (abs(A_ref_obj-A_ref)*100<A_ref*tol_err) 
			return 0;

		//carry out best fit by nudging along x and y directions to get best auto-correlation.
		int x_nudge =0;
		int y_nudge =0;
		//change on Mar 14 2012
		int full_span_x = M_ARRAY/2;
		int full_span_y = N_ARRAY/2;
		double A_match =A_ref_obj;
		int l=0;
		int x_shift =0;
		int y_shift =0;
		int x_max = 0;
		int y_max = 0;
		int x_0 = 0;
		int y_0 = 0;
		double theta = *p_Theta;

		if (TmpImage != NULL) free(TmpImage);

		TmpImage = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		BYTE* tmp2objimg = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memcpy(tmp2objimg, ObjImage,4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		BYTE* tmp3objimg = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memcpy(tmp3objimg, ObjImage,4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memset(TmpImage,1,4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		while (l<4)
		{
			//change on Mar 14 2012
			y_nudge = y_max - 3*full_span_y/4;
			x_nudge = x_max - 3*full_span_x/4;
			for (int k_y=0; k_y<5;k_y++)
			{
				y_nudge += full_span_y/4;
				for (int k_x=0; k_x<5;k_x++)
				{
					x_nudge += full_span_x/4;
					A_ref_obj = 0;
					for (long j=0; j< 2*N_ARRAY; j++)
					{
						for (long i=0; i< 2*M_ARRAY; i++)
						{
							if (x_nudge + i < 0)
								continue;
							if (x_nudge + i >= 2*M_ARRAY)
								continue;
							if (y_nudge + j < 0)
								break;
							if (y_nudge + j >= 2*N_ARRAY)
								break;
							*(TmpImage + 2*j*M_ARRAY + i) = *(ObjImage + 2* (y_nudge + j) *M_ARRAY + (x_nudge + i));
							if (!highaccuracymode)
								A_ref_obj += (*(TmpImage + 2*j*M_ARRAY + i)) * (*(RefImage + 2*j*M_ARRAY + i));
						}
					}
					//Jun 15 2013 - this takes too long so doing this only for highaccuracy mode...
					if (highaccuracymode)
					{
						memcpy(ObjImage, TmpImage, 4*M_ARRAY*N_ARRAY*sizeof(BYTE));
						x_rot_abt = x_cg_ref - x_nudge;
						y_rot_abt = y_cg_ref - y_nudge;
						if (getBestImageMatchAng (p_Theta, false, singlechannel) == 1)
						{
							free(tmp2objimg);
							return 1;
						}
					}
					if (A_ref_obj > A_match) 
					{
						memcpy(tmp3objimg, TmpImage, 4*M_ARRAY*N_ARRAY*sizeof(BYTE));
						x_max = x_nudge;
						y_max = y_nudge;
						theta = *p_Theta;
						A_match = A_ref_obj;
						if (abs(A_ref_obj-A_ref)*100<A_ref*tol_err) 
							break;
					}
					memcpy(ObjImage, tmp2objimg, 4*M_ARRAY*N_ARRAY*sizeof(BYTE));
				}
				//change on Mar 14 2012
				x_nudge -= 5*full_span_x/4;
				if (abs(A_ref_obj-A_ref)*100<A_ref*tol_err) 
					break;
			}
			if (abs(A_ref_obj-A_ref)*100<A_ref*tol_err) 
				break;
			full_span_x = full_span_x/2;
			x_nudge = x_max ;
			full_span_y = full_span_y/2;
			y_nudge = y_max ;
			l++;
		}
		if (!highaccuracymode)
		{
			memcpy(ObjImage, tmp3objimg, 4*M_ARRAY*N_ARRAY*sizeof(BYTE));
			x_rot_abt = x_cg_ref - x_max;
			y_rot_abt = y_cg_ref - y_max;
			if (getBestImageMatchAng (p_Theta, false, singlechannel) == 1)
			{
				free(tmp2objimg);
				return 1;
			}	
		}
		memcpy(ObjImage, tmp2objimg, 4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		*(xy + 2) += x_max;
		*(xy + 3) += y_max;

		*p_Theta = theta;
	
		free (tmp2objimg);
		free (tmp3objimg);
		return 0;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0045", __FILE__, __FUNCSIG__);return 1;}
}

bool RBF::get_CGParams(BYTE* image, int w, int h, bool singleCh, double* cgParams, int deadPixW, int binCutOff, int sc, bool ProfileImage)
{
	try
	{
		bool ans = false;
		ans = setOneShotImage(image, w, h, singleCh, deadPixW, binCutOff, sc, false, true, ProfileImage);
		cgParams[0] = A_obj; cgParams[1] = x_cg_obj; cgParams[2] = y_cg_obj; cgParams[3] = orientation_obj;
		return true;
	}
	catch (...)
	{
		SetAndRaiseErrorMessage("RBF0045a", __FILE__, __FUNCSIG__); return false;
	}
}


bool RBF::blankPartImage (refobj img, direction dir, double percentage, bool singlechannel)
{
	try
	{
		BYTE* tempImage;

		int xstep = 1;

		if (!singlechannel)
		{
			xstep = 3;
		}
		if (img == reference) tempImage = RefImage;
		else tempImage = ObjImage;

		if (tempImage == NULL) return false;

		switch (dir)
		{
			case rightwards:
				for (int j=0; j<2*N_ARRAY; j++)
				{ 
					for (int i=0; i<xstep*int(2*M_ARRAY*percentage/xstep); i++)
					{
						*(tempImage + 2*j*M_ARRAY + i) = 0;
					}
				}
				break;
			case leftwards:
				for (int j=0; j<2*N_ARRAY; j++)
				{ 
					for (int i=xstep*int(2*M_ARRAY*(1-percentage)/xstep) ; i<2*M_ARRAY; i++)
					{
						*(tempImage + 2*j*M_ARRAY + i) = 0;
					}
				}
				break;
			case top:
				for (int j=0; j<int(2*N_ARRAY*percentage); j++)
				{ 
					for (int i=0 ; i<2*M_ARRAY; i++)
					{
						*(tempImage + 2*j*M_ARRAY + i) = 0;
					}
				}
				break;
			case bottom:
				for (int j=int(2*N_ARRAY*(1-percentage)); j<2*N_ARRAY; j++)
				{ 
					for (int i=0 ; i<2*M_ARRAY; i++)
					{
						*(tempImage + 2*j*M_ARRAY + i) = 0;
					}
				}
				break;
		}
		return true;
	}
	catch(...){SetAndRaiseErrorMessage("RBF0046", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::getIrreducibleComponents (BYTE* pixelArray, int width, int height, bool singlechannel, IMGLIST* p_imageLst, intPtsList* p_edgeptslist, int deadPixelWidth, int binarizingCutoff)
{
	try
	{
		int scale = width/800;
		if (scale == 0)
			scale = 1;
		BYTE* tempImage1;
		BYTE* tempImage2;
		BYTE* compImage;
		BYTE* tmpCompImage;
		int i,j =0;
		int m = 0;
		int n =0;
		bool compfound;
		intPtsList ptslist;
		intPtsList neighboringptslist;
		intPtsList edgeptslist;
		intPt tmppt;
		int xstep = 1;
		intPtsList::iterator iter, iter2;
		intPt neighborpts[4];

		M_ARRAY = width / 2;
		N_ARRAY = height / 2;

		if (!singlechannel)
		{
			xstep = 3;
			M_ARRAY *= xstep;
		}

		//temporary image for storing incoming byte stream converted into 0's and 1's
		tempImage1 = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memset(tempImage1,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));

		//temporary image for performing noise filtering of incoming byte stream before storing in tempImage1
		tempImage2 = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memset(tempImage2,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));

		//get rid of corner pixels because this can contain noise
		for (j=deadPixelWidth*scale; j<2 * N_ARRAY - deadPixelWidth*scale; j++)
		{ 
			for (i = deadPixelWidth*scale*xstep; i < 2 * M_ARRAY - deadPixelWidth*scale*xstep ; i++)
			{
				//use 200 as cut-off for digitizing incoming byte stream into 0's and 1's.
				if (*(pixelArray + 2*j*M_ARRAY + i) > binarizingCutoff)
					*(tempImage2 + 2*j*M_ARRAY + i) = 0;
				else
					*(tempImage2 + 2*j*M_ARRAY + i) = 1;		
			}
		}

		//averaging algorithm to get rid of noise
		for (j=deadPixelWidth*scale; j<2 * N_ARRAY - deadPixelWidth*scale; j++)
		{ 
			for (i =deadPixelWidth*scale*xstep; i < 2 * M_ARRAY -deadPixelWidth*scale*xstep; i += xstep)
			{
				int k = 0;
				for (int l = -2; l < 3; l++)
				{
					for (int p = -2*xstep; p < 3*xstep; p +=xstep)
					{
						k += *(tempImage2 + 2 * (j + l) * M_ARRAY + i + p);
					}
				}
				if (k > 25/2) 
				{
					for (int l = 0; l < xstep; l++)
					{
						*(tempImage1 + 2*j*M_ARRAY + i + l) = 1;
					}
				}
				else
				{
					for (int l = 0; l < xstep; l++)
					{
						*(tempImage1 + 2*j*M_ARRAY + i + l) = 0;
					}
				}
			}
		}
		//free tempImage2 because it has done its job now.  tempImage1 is what will be used in subsequent logic
		free (tempImage2);

		//compImage is assignment of individual pixel points with component numbers - initialized with 0
		compImage = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		memset(compImage,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));

		//number of discrete components found in incoming byte stream is initialized to zero.
		int comp_ct = 0;

		//list of discrete images is initialized to an empty list.
		p_imageLst->clear();
		//edge points list passed back to caller is initialized to empty list.
		p_edgeptslist->clear();

		//points list to consist of all points of an image whose neighbors needs to be checked.
		ptslist.clear();
		//points list consisting of all neighboring points that also belong to image
		neighboringptslist.clear();
		//points list to consist of edge points in each of the discrete images
		edgeptslist.clear();

		while ((m < 2 * M_ARRAY) && (n < 2 * N_ARRAY))
		{
			compfound = false;
			for (j = n; j < 2 * N_ARRAY; j+=scale)
			{
				for (i = 0; i < 2 * M_ARRAY; i += xstep*scale)
				{
					if (j == n)
					{
						if (i < m) continue;
					}
					if ((*(tempImage1 + 2 * j * M_ARRAY + i) == 1) && (*(compImage + 2 * j * M_ARRAY + i) == 0 ))
					{
						comp_ct++;
						//found a new component.  so allocate memory for a new discrete image
						tmpCompImage = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
						memset(tmpCompImage,0,4*M_ARRAY*N_ARRAY*sizeof(BYTE));
					
						for (int l =0; l < xstep*scale; l++)
						{
							for (int z = 0; z < scale; z++)
							{
								*(compImage + 2 * (j+z) * M_ARRAY + i + l) = -comp_ct;
								*(tmpCompImage + 2 * (j+z) * M_ARRAY + i + l) = 1;
							}
						}
						m = i + xstep*scale;
						n = j;

						if (m >= 2 * M_ARRAY)
						{
							m = 0;
							n = n + scale;
						}
						tmppt.x = i;
						tmppt.y = j;
						ptslist.push_back(tmppt);
						edgeptslist.push_back(tmppt);
						compfound = true;
						break;
					}
				}
				if (compfound == true) break;
			}

			//no more components found, so exit
			if (compfound == false ) break;

			//check if we have more points for which we need to check for neighboring points.
			while (ptslist.size() > 0)
			{
				//get all neighboring points that also belong to image in neighboring points list.
				for (iter = ptslist.begin(); iter != ptslist.end(); iter++)
				{
					neighborpts[0].x = iter->x + xstep*scale;
					neighborpts[0].y = iter->y;

					neighborpts[1].x = iter->x - xstep*scale;
					neighborpts[1].y = iter->y;

					neighborpts[2].x = iter->x;
					neighborpts[2].y = iter->y - scale;

					neighborpts[3].x = iter->x;
					neighborpts[3].y = iter->y + scale;

					for (int a = 0; a < 4; a++)
					{
						if ((neighborpts[a].x >= 0) && (neighborpts[a].x < 2 * M_ARRAY) && (neighborpts[a].y >=0) && (neighborpts[a].y < 2 * N_ARRAY))
						{
							if ((*(tempImage1 + 2 * neighborpts[a].y * M_ARRAY + neighborpts[a].x) == 1) && (*(compImage + 2 * neighborpts[a].y * M_ARRAY + neighborpts[a].x) == 0))
							{
								for (int l =0; l < xstep*scale; l++)
								{
									for (int z = 0; z < scale; z++)
									{
										*(compImage + 2 * (neighborpts[a].y + z) * M_ARRAY + neighborpts[a].x + l) = comp_ct;
										*(tmpCompImage + 2 * (neighborpts[a].y + z) * M_ARRAY + neighborpts[a].x + l) = 1;
									}
								}
								tmppt.x = neighborpts[a].x;
								tmppt.y = neighborpts[a].y;
								neighboringptslist.push_back(tmppt);
							}
							if ((*(tempImage1 + 2 * neighborpts[a].y * M_ARRAY + neighborpts[a].x) == 0) && (*(compImage + 2 * iter->y * M_ARRAY + iter->x) == comp_ct))
							{
								for (int l =0; l < xstep*scale; l++)
								{
									for (int z = 0; z < scale; z++)
									{
										*(compImage + 2 * (iter->y + z) * M_ARRAY + iter->x + l) = -comp_ct;
									}
								}
								tmppt.x = iter->x;
								tmppt.y = iter->y;
								edgeptslist.push_back(tmppt);
							}
						}
						else
						{
							if (*(compImage + 2 * iter->y * M_ARRAY + iter->x) == comp_ct)
							{
								for (int l =0; l < xstep * scale; l++)
								{
									for (int z = 0; z < scale; z++)
									{
										*(compImage + 2 * (iter->y + z) * M_ARRAY + iter->x + l) = -comp_ct;
									}
								}
								tmppt.x = iter->x;
								tmppt.y = iter->y;
								edgeptslist.push_back(tmppt);
							}
						}
					}
				}
				//clear existing points list and copy all new neighboring points detected into points list and continue while loop.
				ptslist.clear();
				for (iter2 = neighboringptslist.begin(); iter2 != neighboringptslist.end(); iter2++)
				{
					ptslist.push_back(*iter2);
				}
				neighboringptslist.clear();
			}
			//update component image back to format understood by caller - namely bytes ranging from 0 to 255.
			int A_comp = 0;
			for (j = 0; j < 2 * N_ARRAY; j++)
			{
				for (i = 0; i < 2 * M_ARRAY; i++)
				{
					A_comp += *(tmpCompImage + 2 * j * M_ARRAY + i);
					*(tmpCompImage + 2 * j * M_ARRAY + i) = 255 * (1 - *(tmpCompImage + 2 * j * M_ARRAY + i));
				}
			}
			//add to image list and add to edge pts list.
			if (A_comp > 400 * scale * xstep )
			{
				p_imageLst->push_back(tmpCompImage);

				for (iter2 = edgeptslist.begin(); iter2 != edgeptslist.end(); iter2++)
				{
					p_edgeptslist->push_back(*iter2);
				}
			}
			//clear points list and edge points list to be ready for next component detection.
			ptslist.clear();
			edgeptslist.clear();
		}

		//if (comp_ct == 0)
		//{
		//	tmpCompImage = (BYTE*) malloc(4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		//	memcpy(tmpCompImage,pixelArray,4*M_ARRAY*N_ARRAY*sizeof(BYTE));
		//	p_imageLst->push_back(tmpCompImage);
		//}
		//free memory allocated in function that need not be used by caller
		free (compImage);
		free (tempImage1);

		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0047", __FILE__, __FUNCSIG__);return false;}
}

int RBF::fnLinesAndArcsN(PtsList* ptr_Pts, LineArcList* ptr_SecondOrderslopes, double tolerance, int MaxNumLineArcEntities)
{
	try
	{
		bool foundLineArcEnt = false;
		int NumLineArcEnt = 0;

		while (!foundLineArcEnt)
		{
			ptr_SecondOrderslopes->clear();
			fnLinesAndArcs(ptr_Pts, ptr_SecondOrderslopes, tolerance);
			NumLineArcEnt = ptr_SecondOrderslopes->size();
			if (NumLineArcEnt < MaxNumLineArcEntities)
				foundLineArcEnt = true;
			tolerance *= 2;
		}
		return 0;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0048", __FILE__, __FUNCSIG__);return 1;}
}

bool RBF::fnLinesAndArcsThread(PtsList* ptr_Pts, LineArcList* ptr_SecondOrderslopes, double* p_tolerance, int* thread_sep_flag, double max_radius, double min_radius)
{
	try
	{
		bool foundThreadLineArcList = false;
		int Ct = 0;
		LineArcList::iterator i;
		int lineArcEntities;
		bool isLine;
		int thread_ct;

		//try for max iterations of 100 else break and return false
		while (Ct < 100)
		{
			ptr_SecondOrderslopes->clear();
			lineArcEntities = 0;
			isLine = true;
			thread_ct = 0;
			fnLinesAndArcs(ptr_Pts, ptr_SecondOrderslopes, *p_tolerance, max_radius, min_radius);
			//check first for alternate lines and arcs of thread.
			for (i = ptr_SecondOrderslopes->begin(); i != ptr_SecondOrderslopes->end(); i++)
			{
				lineArcEntities++;
				if (((isLine) && (i->slope != 0)) || ((!isLine) && (i->slope == 0)))
				{
					thread_ct++;
				}
				if (i->slope == 0)
					isLine = true;
				else
					isLine = false;
			}
			if (thread_ct >= lineArcEntities - 2)
			{
				*thread_sep_flag = 0;
				foundThreadLineArcList = true;
				break;
			}
			lineArcEntities = 0;
			isLine = true;
			thread_ct = 0;
			double prev_slope = 0;
			double prev_prev_slope = 0;
			double curr_slope = 0;
			//check now for sequence of lines but with slopes of alternately occurring lines matching within tolerance
			for (i = ptr_SecondOrderslopes->begin(); i != ptr_SecondOrderslopes->end(); i++)
			{
				lineArcEntities++;
				if ((isLine) && (i->slope == 0)) 
				{
					curr_slope = i->intercept;
					if (abs(curr_slope - prev_prev_slope) < *p_tolerance)
						thread_ct++;
					prev_prev_slope = prev_slope;
					prev_slope = curr_slope;
				}
				if (i->slope == 0)
					isLine = true;
				else
					isLine = false;
			}
			if (thread_ct >= lineArcEntities - 2)
			{
				*thread_sep_flag = 1;
				foundThreadLineArcList = true;
				break;
			}
			lineArcEntities = 0;
			isLine = true;
			thread_ct = 0;
			double prev_1_slope = 0;
			double prev_2_slope = 0;
			double prev_3_slope = 0;
			double prev_4_slope = 0;
			curr_slope = 0;
			//check now for sequence of lines but with slopes of every 5th line matching within tolerance
			for (i = ptr_SecondOrderslopes->begin(); i != ptr_SecondOrderslopes->end(); i++)
			{
				lineArcEntities++;
				if ((isLine) && (i->slope == 0)) 
				{
					curr_slope = i->intercept;
					if (abs(curr_slope - prev_4_slope) < *p_tolerance)
						thread_ct++;
					prev_4_slope = prev_3_slope;
					prev_3_slope = prev_2_slope;
					prev_2_slope = prev_1_slope;
					prev_1_slope = curr_slope;
				}
				if (i->slope == 0)
					isLine = true;
				else
					isLine = false;
			}
			if (thread_ct >= lineArcEntities - 2)
			{
				*thread_sep_flag = 2;
				foundThreadLineArcList = true;
				break;
			}
			*p_tolerance *= 2;
			Ct++;
		}
		return foundThreadLineArcList;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0048", __FILE__, __FUNCSIG__);return false;}
}

void RBF::updateLine(Pt start_pt, Pt end_pt, LineArc* ptr_Line)
{
	try
	{
		double x_val, y_val;
		y_val = end_pt.y - start_pt.y;
		x_val = end_pt.x - start_pt.x;
		ptr_Line->start_pt = start_pt;
		ptr_Line->end_pt = end_pt;
		ptr_Line->mid_pt.x = (start_pt.x + end_pt.x)/2;
		ptr_Line->mid_pt.y = (start_pt.y + end_pt.y)/2;
		ptr_Line->length = sqrt ( pow (y_val, 2) + pow (x_val, 2));
		if (x_val == 0)
		{
			ptr_Line->slope = M_PI / 2.0;
			ptr_Line->intercept = start_pt.x;
			if (y_val < 0 )
				ptr_Line->slope = M_PI * 1.5;
		}
		else
		{
			ptr_Line->slope = atan(y_val/x_val);
			ptr_Line->intercept = start_pt.y - start_pt.x * y_val / x_val;
			if (x_val < 0)
				ptr_Line->slope += M_PI;
			if (ptr_Line->slope < 0)
				ptr_Line->slope = 2 * M_PI + ptr_Line->slope;
		}
		return;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0049", __FILE__, __FUNCSIG__);return;}
}

bool RBF::getUpdatedPolygonLines(Pt new_pt, LineArcList* ptr_lines)
{
	try
	{
		int n = ptr_lines->size();

		if (n < 3) return false;

		double maxangle = 0;
		double minangle = 2 * M_PI;
		Pt max_pt;
		Pt min_pt;
		LineArcList::iterator j;
		PtsList::iterator i;
		PtsList pts;
		LineArc tmpLine;

		for (j = ptr_lines->begin(); j != ptr_lines->end(); j++)
		{
			pts.push_back(j->start_pt);
		}

		i = pts.begin();
		min_pt = *i;
		max_pt = *i;
		double prevslope = 0;
		int a = 0;
		for (i = pts.begin(); i != pts.end(); i++)
		{
			updateLine(*i, new_pt, &tmpLine);

			if (tmpLine.slope >= prevslope)
			{
				tmpLine.slope = tmpLine.slope - int(((tmpLine.slope - prevslope)/(M_PI * 2.0)) + 0.5) * M_PI * 2.0;
			}
			else
			{
				tmpLine.slope = tmpLine.slope + int(((prevslope - tmpLine.slope)/(M_PI * 2.0)) + 0.5) * M_PI * 2.0;
			}

			prevslope = tmpLine.slope;
			if (a == 0)
			{
				minangle = tmpLine.slope;
				maxangle = tmpLine.slope;
				min_pt = *i;
				max_pt = *i;
			}
			a++;
			if (tmpLine.slope < minangle)
			{
				minangle = tmpLine.slope;
				min_pt = *i;
			}
			if (tmpLine.slope > maxangle)
			{
				maxangle = tmpLine.slope;
				max_pt = *i;
			}
		}

		if (maxangle - minangle > M_PI)
		{
			//this means point is inside polygon passed,
			//so return without doing anything
			pts.clear();
			return true;
		}

		//line1 and line2 are the two lines from max_pt on either side of existing polygon
		LineArc line1, line2;
		//dir_start and dir_end are the points on existing polygon from which the updated polygon
		//would repeat lines from existing polygon.
		Pt dir_start;
		Pt dir_end;
	
		j = ptr_lines->begin();
		line1 = *j;
		line2 = *j;
	
		for (j = ptr_lines->begin(); j != ptr_lines->end(); j++)
		{
			if ((j->start_pt.x == max_pt.x) && (j->start_pt.y == max_pt.y))
			{
				line1 = *j;
			}
			if ((j->end_pt.x == max_pt.x) && (j->end_pt.y == max_pt.y))
			{
				line2 = *j;
				line2.end_pt = j->start_pt;
				line2.start_pt = j->end_pt;
				line2.slope += M_PI;
				if (line2.slope > 2 * M_PI)
					line2.slope -= 2 * M_PI;
			}
		}

		//get dot product of vector from maxpt to new pt to unit vector along line1 and line2

		double dot_prod1 = ((new_pt.y - max_pt.y) * (line1.end_pt.y - line1.start_pt.y)
							+ (new_pt.x - max_pt.x) * (line1.end_pt.x - line1.start_pt.x)) /line1.length;
		double dot_prod2 = ((new_pt.y - max_pt.y) * (line2.end_pt.y - line2.start_pt.y)
							+ (new_pt.x - max_pt.x) * (line2.end_pt.x - line2.start_pt.x)) /line2.length;

		LineArcList newlinelist;
		if (dot_prod1 > dot_prod2)
		{
			dir_start = min_pt;
			dir_end = max_pt;
			updateLine(max_pt, new_pt, &tmpLine);
			newlinelist.push_back(tmpLine);
			updateLine(new_pt, min_pt, &tmpLine);
			newlinelist.push_back(tmpLine);
		}
		else
		{
			dir_start = max_pt;
			dir_end = min_pt;
			updateLine(min_pt, new_pt, &tmpLine);
			newlinelist.push_back(tmpLine);
			updateLine(new_pt, max_pt, &tmpLine);
			newlinelist.push_back(tmpLine);
		}

		bool addline = false;

		for (j = ptr_lines->begin(); j!= ptr_lines->end(); j++)
		{
			if ((j->start_pt.x == dir_start.x) && (j->start_pt.y == dir_start.y))
				addline = true;
			if ((j->start_pt.x == dir_end.x) && (j->start_pt.y == dir_end.y))
				addline = false;
			if (addline)
				newlinelist.push_back(*j);
		}
		if (addline)
		{
			for (j = ptr_lines->begin(); j!= ptr_lines->end(); j++)
			{
				if ((j->start_pt.x == dir_end.x) && (j->start_pt.y == dir_end.y))
					addline = false;
				if (addline)
					newlinelist.push_back(*j);
			}
		}

		ptr_lines->clear();

		for (j = newlinelist.begin(); j != newlinelist.end(); j++)
		{
			ptr_lines->push_back(*j);
		}

		newlinelist.clear();

		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0050", __FILE__, __FUNCSIG__);return false;}
}

bool RBF::getPolygonLines(PtsList* ptr_pts, LineArcList* ptr_lines)
{
	try
	{
		int n = ptr_pts->size();
		LineArc tmpLine;
		PtsList::iterator i;
		LineArcList::iterator line_iter;
		Pt tmpPt;
		Pt start_pt;
		Pt end_pt;

		if (n < 3) return false;

		if (n == 3)
		{
			ptr_lines->clear();
			for (i = ptr_pts->begin(); i != ptr_pts->end(); i++)
			{
				start_pt = *i;
				i++;
				if (i == ptr_pts->end())
					end_pt = *(ptr_pts->begin());
				else
					end_pt = *i;
				i--;
				updateLine(start_pt, end_pt, &tmpLine);
				ptr_lines->push_back(tmpLine);
			}
			return true;
		}
	
		//store last point before deleting last element of list
		tmpPt = *(ptr_pts->rbegin());
		ptr_pts->pop_back();

		//ptr_pts has all points except last element, so make sure last point is pushed back
		//before returning
		if (getPolygonLines(ptr_pts, ptr_lines) == false)
		{
			ptr_pts->push_back(tmpPt);
			return false;
		}

		double maxangle = 0;
		double minangle = 2 * M_PI;
		Pt max_pt;
		Pt min_pt;
		double prevslope =0;
		int a = 0;
		for (line_iter = ptr_lines->begin(); line_iter != ptr_lines->end(); line_iter++)
		{
			updateLine(line_iter->start_pt, tmpPt, &tmpLine);
			if (tmpLine.slope >= prevslope)
			{
				tmpLine.slope = tmpLine.slope - int(((tmpLine.slope - prevslope)/(M_PI * 2.0)) + 0.5) * M_PI * 2.0;
			}
			else
			{
				tmpLine.slope = tmpLine.slope + int(((prevslope - tmpLine.slope)/(M_PI * 2.0)) + 0.5) * M_PI * 2.0;
			}

			prevslope = tmpLine.slope;
			if (a == 0)
			{
				minangle = tmpLine.slope;
				maxangle = tmpLine.slope;
				min_pt = line_iter->start_pt;
				max_pt = line_iter->start_pt;
			}
			a++;
			if (tmpLine.slope < minangle)
			{
				minangle = tmpLine.slope;
				min_pt = line_iter->start_pt;
			}
			if (tmpLine.slope > maxangle)
			{
				maxangle = tmpLine.slope;
				max_pt = line_iter->start_pt;
			}
		}

		if (maxangle - minangle > M_PI)
		{
			//this means point is inside polygon already retrieved,
			//so return without doing anything
			ptr_pts->push_back(tmpPt);
			return true;
		}

		LineArcList::iterator j;
		//line1 and line2 are the two lines from max_pt on either side of existing polygon
		LineArc line1, line2;
		//dir_start and dir_end are the points on existing polygon from which the updated polygon
		//would repeat lines from existing polygon.
		Pt dir_start;
		Pt dir_end;
	
		j = ptr_lines->begin();
		line1 = *j;
		line2 = *j;
	
		for (j = ptr_lines->begin(); j != ptr_lines->end(); j++)
		{
			if ((j->start_pt.x == max_pt.x) && (j->start_pt.y == max_pt.y))
			{
				line1 = *j;
			}
			if ((j->end_pt.x == max_pt.x) && (j->end_pt.y == max_pt.y))
			{
				line2 = *j;
				line2.end_pt = j->start_pt;
				line2.start_pt = j->end_pt;
				line2.slope += M_PI;
				if (line2.slope > 2 * M_PI)
					line2.slope -= 2 * M_PI;
			}
		}

		//get dot product of vector from maxpt to new pt to unit vector along line1 and line2

		double dot_prod1 = ((tmpPt.y - max_pt.y) * (line1.end_pt.y - line1.start_pt.y)
							+ (tmpPt.x - max_pt.x) * (line1.end_pt.x - line1.start_pt.x)) /line1.length;
		double dot_prod2 = ((tmpPt.y - max_pt.y) * (line2.end_pt.y - line2.start_pt.y)
							+ (tmpPt.x - max_pt.x) * (line2.end_pt.x - line2.start_pt.x)) /line2.length;

		LineArcList newlinelist;
		if (dot_prod1 > dot_prod2)
		{
			dir_start = min_pt;
			dir_end = max_pt;
			updateLine(max_pt, tmpPt, &tmpLine);
			newlinelist.push_back(tmpLine);
			updateLine(tmpPt, min_pt, &tmpLine);
			newlinelist.push_back(tmpLine);
		}
		else
		{
			dir_start = max_pt;
			dir_end = min_pt;
			updateLine(min_pt, tmpPt, &tmpLine);
			newlinelist.push_back(tmpLine);
			updateLine(tmpPt, max_pt, &tmpLine);
			newlinelist.push_back(tmpLine);
		}

		bool addline = false;

		for (j = ptr_lines->begin(); j!= ptr_lines->end(); j++)
		{
			if ((j->start_pt.x == dir_start.x) && (j->start_pt.y == dir_start.y))
				addline = true;
			if ((j->start_pt.x == dir_end.x) && (j->start_pt.y == dir_end.y))
				addline = false;
			if (addline)
				newlinelist.push_back(*j);
		}
		if (addline)
		{
			for (j = ptr_lines->begin(); j!= ptr_lines->end(); j++)
			{
				if ((j->start_pt.x == dir_end.x) && (j->start_pt.y == dir_end.y))
					addline = false;
				if (addline)
					newlinelist.push_back(*j);
			}
		}

		ptr_lines->clear();

		for (j = newlinelist.begin(); j != newlinelist.end(); j++)
		{
			ptr_lines->push_back(*j);
		}

		newlinelist.clear();
	
		ptr_pts->push_back(tmpPt);

		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0051", __FILE__, __FUNCSIG__);return false;}
}

bool RBF::line_intersect_any_other_line(Line_3D line1, Line_3DList line_list, double precision)
{
	try
	{
		for (Line_3DList::iterator line_iter = line_list.begin(); line_iter != line_list.end(); line_iter++)
		{
			if (line_intersect(line1, *line_iter, precision) == true)
				return true;
		}
		return false;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0052", __FILE__, __FUNCSIG__); return false;}
}
bool RBF::line_intersect(Line_3D line1, Line_3D line2, double precision)
{
	try
	{
		if (line1 == line2) return true;
		//if solution exists for the simultaneous set of equations a*pt11 + (1-a)*pt12 = b*pt21 + (1-b)*pt22 where a and be are between 0 and 1, return true, else return false
		double x11 = line1.pt_1.x;
		double y11 = line1.pt_1.y;
		double z11 = line1.pt_1.z;

		double x12 = line1.pt_2.x;
		double y12 = line1.pt_2.y;
		double z12 = line1.pt_2.z;

		double x21 = line2.pt_1.x;
		double y21 = line2.pt_1.y;
		double z21 = line2.pt_1.z;

		double x22 = line2.pt_2.x;
		double y22 = line2.pt_2.y;
		double z22 = line2.pt_2.z;

		if ((abs(x11 - x21) > 2 * precision) || (abs(y11 - y21) > 2 * precision) || (abs(z11 - z21) > 2 * precision)) return false;
		if ((abs(x12 - x21) > 2 * precision) || (abs(y12 - y21) > 2 * precision) || (abs(z12 - z21) > 2 * precision)) return false;
		if ((abs(x11 - x22) > 2 * precision) || (abs(y11 - y22) > 2 * precision) || (abs(z11 - z22) > 2 * precision)) return false;
		if ((abs(x12 - x22) > 2 * precision) || (abs(y12 - y22) > 2 * precision) || (abs(z12 - z22) > 2 * precision)) return false;

		double a, b;
		double den;
		double den2;

		den = (x11 - x12) * (y21 - y22) - (y11 - y12) * (x21 - x22);

		if (abs(den) < Tolerance ) 
		{
			den2 = (x11 - x12) * (y11 - y22) - (y11 - y12) * (x11 - x22);
			if (abs(den2) < Tolerance)
			{
				if ((x11 <= x21) && (x11 <= x22) && (x12 <= x21) && (x12 <= x22))
					return false;
				if ((x11 >= x21) && (x11 >= x22) && (x12 >= x21) && (x12 >= x22))
					return false;
				if ((y11 <= y21) && (y11 <= y22) && (y12 <= y21) && (y12 <= y22))
					return false;
				if ((y11 >= y21) && (y11 >= y22) && (y12 >= y21) && (y12 >= y22))
					return false;
				if ((abs(z11 - z21) > precision) && (abs(z11 - z22) > precision) && (abs(z12 - z21) > precision) && (abs(z12 - z22) > precision))
					return false;
				return true;
			}
			return false;
		}

		if ((line1.pt_1 == line2.pt_1) || (line1.pt_1 == line2.pt_2) || (line1.pt_2 == line2.pt_1) || (line1.pt_2 == line2.pt_2))
			return false;

		a = ((x22 - x12) * (y21 - y22) - (y22 - y12) * (x21 - x22))/den;

		if ((a<= Tolerance) || ( a >= 1 - Tolerance)) return false;

		b = ((x22 - x12) * (y11 - y12) - (y22 - y12) * (x11 - x12))/den;

		if ((b<= Tolerance) || (b >= 1 - Tolerance)) return false;

		double z_err = a * (z11 - z12) - b * (z21 - z22) - (z22 - z12);

		if (abs(z_err) > precision) return false;

		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0052", __FILE__, __FUNCSIG__); return false;}
}
//ptslist is an array of 3-d points of size ptscount, ie an array of doubles of size 3*ptscount.  
//trianglelist is a pointer to an array of 3-d triangles of size trianglescount, ie an array of doubles of size 9*trianglescount.
bool RBF::TriangulateSurface(double* ptslist, int ptscount, double** trianglelist, int* trianglescount, bool triangleLengthUpperCutoff, int step, int MaxDistToAvgDist )
{
	try
	{
		Pt_3D pt_1;
		Pt_3D pt_2;
		Pt_3D pt_3;
		Line_3D line_1;
		Line_3D line_2;
		Line_3D line_3;
		double dist = 0;
		int min_index_1 = -1;
		int min_index_2 = -1;
		double PlanePts[9] = {0};
		double Plane[4] = {0,0,1,0};
		Pts_3DList pts_3d_list_variable;
		Line_3DList line_3d_list;
		Triangle_3DList triangle_3d_list;
		Pt_3D tmppt_3d;
		Line_3D tmpline_3d;
		Triangle_3D tmptriangle_3d;
		Pts_3DList::iterator pts_iter, pts_iter_1;
		Line_3DList::iterator line_iter, line_iter_1, line_iter_2;
		Triangle_3DList::iterator triangle_iter;
		int line_index = 0;
		int triangle_index = 0;
		bool line_in_list = false;
		bool triangle_in_list = false;
		bool boundary_line_found = true;
		bool first_pass = true;
		int prev_line_count = 0;
		bool common_boundary_pt_found = false;
		double min_d;
		double minimum_distance = dist_cutoff;

		double max_x =*ptslist, max_y=*(ptslist + 1), max_z=*(ptslist + 2), min_x=*ptslist, min_y=*(ptslist + 1), min_z=*(ptslist + 2);

		for (int i = 1; i < ptscount; i++)
		{
			if (max_x < *(ptslist + 3 * i))
				max_x = *(ptslist + 3 * i); 
			if (max_y < *(ptslist + 3 * i + 1))
				max_y = *(ptslist + 3 * i + 1); 
			if (max_z < *(ptslist + 3 * i + 2))
				max_z = *(ptslist + 3 * i + 2); 

			if (min_x > *(ptslist + 3 * i))
				min_x = *(ptslist + 3 * i); 
			if (min_y > *(ptslist + 3 * i + 1))
				min_y = *(ptslist + 3 * i + 1); 
			if (min_z > *(ptslist + 3 * i + 2))
				min_z = *(ptslist + 3 * i + 2); 
		}

		int N_x = 1, N_y = 1, N_z = 1;
		double x = 0;
		double x_side = max_x - min_x;
		double y_side = max_y - min_y;
		double z_side = max_z - min_z;
	
		bool is_cube = true;

		if ((x_side == 0) || (y_side == 0) || (z_side == 0))
			is_cube = false;

		double min_side = z_side;
		if (x_side < min_side) min_side = x_side;
		if (y_side < min_side) min_side = y_side;
		if (is_cube)
		{
			if ((min_side/x_side < 0.1) || (min_side/y_side < 0.1) || (min_side/z_side < 0.1))
				is_cube = false;
		}
		//approx side of cube occupied by points that are step apart
		if (is_cube)
		{
			x = pow((x_side * y_side * z_side * step / ptscount), (double) 1/3);
		}
		else
		{	
			//approx side of square occupied by points that are step apart
			if (min_side == x_side)
			{
				if ((y_side == 0) || (z_side == 0))
					return false;
				if ((y_side/z_side < 0.1) || (z_side/y_side < 0.1))
					return false;
				x = pow(( y_side * z_side * step / ptscount), (double) 1/2);
			}
			else if (min_side == z_side)
			{
				if ((y_side == 0) || (x_side == 0))
					return false;
				if ((y_side/x_side < 0.1) || (x_side/y_side < 0.1))
					return false;
				x = pow((x_side * y_side * step / ptscount), (double) 1/2);
			}
			else
			{
				if ((x_side == 0) || (z_side == 0))
					return false;
				if ((x_side/z_side < 0.1) || (z_side/x_side < 0.1))
					return false;
				x = pow((x_side * z_side * step / ptscount), (double) 1/2);
			}
		}
	
		N_x = (int) (x_side / x) + 1;
		N_y = (int) (y_side / x) + 1;
		N_z = (int) (z_side / x) + 1;
	
		if (triangleLengthUpperCutoff)
		{
			minimum_distance = MaxDistToAvgDist * x;
		}
		int* Occupancy = NULL;
		Occupancy = (int*) malloc(N_x * N_y * N_z * sizeof(int));
		memset(Occupancy, 0, N_x * N_y * N_z * sizeof(int));
		int a = 0, b = 0, c = 0;
		double ax = 0, by = 0, cz = 0;
		for (int i = 0; i < ptscount; i++)
		{
			if (max_x != min_x)
				ax = (*(ptslist + 3 * i) - min_x) * (N_x - 1) / (max_x - min_x);
			else
				ax = 0;
			a = (int) ax;

			if (max_y != min_y)
				by = (*(ptslist + 3 * i + 1) - min_y) * (N_y - 1) / (max_y - min_y);
			else
				by = 0;
			b = (int) by;

			if (max_z != min_z)
				cz = (*(ptslist + 3 * i + 2) - min_z) * (N_z - 1) / (max_z - min_z);
			else
				cz = 0;
			c = (int) cz;

			if ((*(Occupancy + N_y*N_x*c + N_x*b + a) == 0) || (step == 1))
			{
				*(Occupancy + N_y*N_x*c + N_x*b + a) = *(Occupancy + N_y*N_x*c + N_x*b + a) + 1;
				tmppt_3d.x = *(ptslist + 3 * i); tmppt_3d.y = *(ptslist + 3 * i + 1); tmppt_3d.z = *(ptslist + 3 * i + 2); tmppt_3d.pt_index = i;
				pts_3d_list_variable.push_back(tmppt_3d);
			}
		}

		free(Occupancy);
		int pts_cnt = pts_3d_list_variable.size();

		int n = 0;
		pts_iter = pts_3d_list_variable.begin();
		while (pts_iter != pts_3d_list_variable.end())
		{
			//take 3 non-collinear neighboring points as pt_1, pt_2 and pt_3 to form a triangle 
			pt_1.x = pts_iter->x; pt_1.y = pts_iter->y; pt_1.z = pts_iter->z; pt_1.pt_index = pts_iter->pt_index;
			pts_iter++;
			n++;
			if (n > pts_cnt) 
			{
				return false;
			}

			tmppt_3d = pt_1;

			min_d = minimum_distance;
			min_index_1 = -1;
			for (pts_iter_1 = pts_3d_list_variable.begin(); pts_iter_1 != pts_3d_list_variable.end(); pts_iter_1++)
			{
				if (pt_1.pt_index == pts_iter_1->pt_index) continue;

				pt_2.x = pts_iter_1->x; pt_2.y = pts_iter_1->y; pt_2.z = pts_iter_1->z; pt_2.pt_index = pts_iter_1->pt_index;
				line_1.pt_1 = pt_1;
				line_1.pt_2 = pt_2;

				if ((abs(pt_2.x - pt_1.x) > min_d) || (abs(pt_2.y - pt_1.y) > min_d) || (abs(pt_2.z - pt_1.z) > min_d)) continue;
				dist = hM->pythag(pt_2.x - pt_1.x, pt_2.y - pt_1.y, pt_2.z - pt_1.z);
				if (dist < min_d)
				{
					if (line_intersect_any_other_line(line_1, line_3d_list, minimum_distance) == true)
					continue;
					min_d = dist;
					min_index_1 = pt_2.pt_index;
				}
			}
			if (min_index_1 == -1) 
			{
				continue;
			}

			pt_2.x = *(ptslist + 3 * min_index_1); pt_2.y = *(ptslist + 3 * min_index_1 + 1); pt_2.z = *(ptslist + 3 * min_index_1 + 2); pt_2.pt_index = min_index_1;

			first_pass = true;

			boundary_line_found = true;
			//take the third point iteratively in below loop
			while (boundary_line_found)
			{
				while (1)
				{
					PlanePts[0] = pt_1.x; PlanePts[1] = pt_1.y; PlanePts[2] = pt_1.z;
					PlanePts[3] = pt_2.x; PlanePts[4] = pt_2.y; PlanePts[5] = pt_2.z;
					min_d = minimum_distance;
					min_index_2 = -1;
					for (pts_iter_1 = pts_3d_list_variable.begin(); pts_iter_1 != pts_3d_list_variable.end(); pts_iter_1++)
					{
						if (pt_1.pt_index == pts_iter_1->pt_index) continue;
						if (pt_2.pt_index == pts_iter_1->pt_index) continue;

						pt_3.x = pts_iter_1->x; pt_3.y = pts_iter_1->y; pt_3.z = pts_iter_1->z; pt_3.pt_index = pts_iter_1->pt_index;

						if ((abs(pt_3.x - pt_1.x) > minimum_distance) || (abs(pt_3.y - pt_1.y) > minimum_distance) || (abs(pt_3.z - pt_1.z) > minimum_distance)) continue;
						if ((abs(pt_2.x - pt_3.x) > minimum_distance) || (abs(pt_2.y - pt_3.y) > minimum_distance) || (abs(pt_2.z - pt_3.z) > minimum_distance)) continue;

						line_2.pt_1 = pt_2;
						line_2.pt_2 = pt_3;
					
						line_3.pt_1 = pt_3;
						line_3.pt_2 = pt_1;
					
						dist = hM->pythag(pt_3.x - pt_2.x, pt_3.y - pt_2.y, pt_3.z - pt_2.z);
						double dist2 = hM->pythag(pt_3.x - pt_1.x, pt_3.y - pt_1.y, pt_3.z - pt_1.z);
						if ((dist <= minimum_distance) && (dist2 <= minimum_distance) && (dist + dist2 < 2 * min_d))
						{
							PlanePts[6] = pt_3.x; PlanePts[7] = pt_3.y; PlanePts[8] = pt_3.z;
							Plane[0] = (pt_2.y - pt_1.y) * (pt_3.z - pt_1.z) - (pt_2.z - pt_1.z) * (pt_3.y - pt_1.y);
							Plane[1] = (pt_2.z - pt_1.z) * (pt_3.x - pt_1.x) - (pt_2.x - pt_1.x) * (pt_3.z - pt_1.z);
							Plane[2] = (pt_2.x - pt_1.x) * (pt_3.y - pt_1.y) - (pt_2.y - pt_1.y) * (pt_3.x - pt_1.x);

							if ((abs(Plane[0]) > Tolerance) || (abs(Plane[1]) > Tolerance) || (abs(Plane[2]) > Tolerance))
							{
								Line_3D testline;
								testline.pt_1 = pt_3;
								testline.pt_2.x = (pt_1.x + pt_2.x)/2;
								testline.pt_2.y = (pt_1.y + pt_2.y)/2;
								testline.pt_2.z = (pt_1.z + pt_2.z)/2;
								testline.pt_2.pt_index = -1;
								if (line_intersect_any_other_line(testline, line_3d_list, minimum_distance)) continue;
								testline.pt_2.x = (pt_1.x + 2 * pt_2.x)/3;
								testline.pt_2.y = (pt_1.y + 2 * pt_2.y)/3;
								testline.pt_2.z = (pt_1.z + 2 * pt_2.z)/3;
								if (line_intersect_any_other_line(testline, line_3d_list, minimum_distance) == true) continue;

								testline.pt_1 = pt_1;
								testline.pt_2.x = (pt_3.x + pt_2.x)/2;
								testline.pt_2.y = (pt_3.y + pt_2.y)/2;
								testline.pt_2.z = (pt_3.z + pt_2.z)/2;
								testline.pt_2.pt_index = -1;
								if (line_intersect_any_other_line(testline, line_3d_list, minimum_distance)) continue;
								testline.pt_2.x = (pt_3.x + 2 * pt_2.x)/3;
								testline.pt_2.y = (pt_3.y + 2 * pt_2.y)/3;
								testline.pt_2.z = (pt_3.z + 2 * pt_2.z)/3;
								if (line_intersect_any_other_line(testline, line_3d_list, minimum_distance) == true) continue;

								testline.pt_1 = pt_2;
								testline.pt_2.x = (pt_3.x + pt_1.x)/2;
								testline.pt_2.y = (pt_3.y + pt_1.y)/2;
								testline.pt_2.z = (pt_3.z + pt_1.z)/2;
								testline.pt_2.pt_index = -1;
								if (line_intersect_any_other_line(testline, line_3d_list, minimum_distance)) continue;
								testline.pt_2.x = (pt_3.x + 2 * pt_1.x)/3;
								testline.pt_2.y = (pt_3.y + 2 * pt_1.y)/3;
								testline.pt_2.z = (pt_3.z + 2 * pt_1.z)/3;
								if (line_intersect_any_other_line(testline, line_3d_list, minimum_distance) == true) continue;

								if (line_intersect_any_other_line(line_2, line_3d_list, minimum_distance)) continue;
								if (line_intersect_any_other_line(line_3, line_3d_list, minimum_distance)) continue;
							
								min_d = (dist + dist2) /2;

								min_index_2 = pt_3.pt_index;
							}
						}
					}
					if (min_index_2 == -1) 
					{
						break;
					}
					pt_3.x = *(ptslist + 3 * min_index_2); pt_3.y = *(ptslist + 3 * min_index_2 + 1); pt_3.z = *(ptslist + 3 * min_index_2 + 2); pt_3.pt_index = min_index_2;

					//add the 3 lines of triangle to line list.
					line_1.pt_1 = pt_1;
					line_1.pt_2 = pt_2;	
					if (first_pass)
					{
						line_1.isBoundary = true;
						line_1.isFinalBoundary = false;
						line_1.line_index = line_index++;
						line_3d_list.push_back(line_1);
					}
					else
					{
						line_in_list = false;
						for (line_iter = line_3d_list.begin(); line_iter != line_3d_list.end(); line_iter++)
						{
							if (line_1 == *line_iter)
							{
								line_in_list = true;
								line_iter->isBoundary = false;
								line_iter->isFinalBoundary = false;
								line_1 = *line_iter;
								break;
							}
						}
						if (line_in_list == false) 
						{
							return false;
						}
					}
					
					line_2.pt_1 = pt_2;
					line_2.pt_2 = pt_3;
					line_2.isBoundary = true;
					line_2.isFinalBoundary = false;
					line_2.line_index = line_index++;
					line_3d_list.push_back(line_2);

					line_3.pt_1 = pt_3;
					line_3.pt_2 = pt_1;
					line_3.isBoundary = true;
					line_3.isFinalBoundary = false;
					line_3.line_index = line_index++;
					line_3d_list.push_back(line_3);

					//add the triangle to triangle list.
					tmptriangle_3d.line_1 = line_1;
					tmptriangle_3d.line_2 = line_2;
					tmptriangle_3d.line_3 = line_3;
					tmptriangle_3d.triangle_index = triangle_index++;
					triangle_3d_list.push_back(tmptriangle_3d);

					//store pt_3 as pt_2 for next iteration
					pt_2 = pt_3;

					//update first_pass to false for subsequent iterations
					first_pass = false;
				}
				//set boundary line found to false so we can break from while loop unless we have boundary lines
				boundary_line_found = false;
				for (line_iter = line_3d_list.begin(); line_iter != line_3d_list.end(); line_iter++)
				{
					if (line_iter->isBoundary == false) continue;
					if (line_iter->isFinalBoundary == true) continue;
					pt_1 = line_iter->pt_1;
					pt_2 = line_iter->pt_2;
					//set final boundary flag to true so that in next iter, we don't check for this line again unless new neighboring points are found.
					//this flag as well as isBoundary flag will get reset to false in above while loop if additional neighboring points are found for this line.
					line_iter->isFinalBoundary = true;
					boundary_line_found = true;
					break;
				}
			}
		}	
		while (prev_line_count < line_index)
		{
			prev_line_count = line_index;
			//now iterate through boundary lines to check for neighboring boundaries missing the third line of triangle.
			for (line_iter = line_3d_list.begin(); line_iter != line_3d_list.end(); line_iter++)
			{
				common_boundary_pt_found = false;
				if (line_iter->isBoundary == false) continue;
				for (line_iter_1 = line_iter; line_iter_1 != line_3d_list.end(); line_iter_1++)
				{
					if (line_iter_1->line_index == line_iter->line_index) continue;
					if (line_iter_1->isBoundary == false) continue;

					if (line_iter_1->pt_1 == line_iter->pt_1)
					{
						tmppt_3d = line_iter->pt_1;
						pt_1 = line_iter->pt_2;
						pt_2 = line_iter_1->pt_2;
						common_boundary_pt_found = true;
					}
					else if (line_iter_1->pt_2 == line_iter->pt_2)
					{
						tmppt_3d = line_iter->pt_2;
						pt_1 = line_iter->pt_1;
						pt_2 = line_iter_1->pt_1;
						common_boundary_pt_found = true;
					}
					else if (line_iter_1->pt_2 == line_iter->pt_1)
					{
						tmppt_3d = line_iter->pt_1;
						pt_1 = line_iter->pt_2;
						pt_2 = line_iter_1->pt_1;
						common_boundary_pt_found = true;
					}
					else if (line_iter_1->pt_1 == line_iter->pt_2)
					{
						tmppt_3d = line_iter->pt_2;
						pt_1 = line_iter->pt_1;
						pt_2 = line_iter_1->pt_2;
						common_boundary_pt_found = true;
					}

					if (common_boundary_pt_found)
					{
						//reset flag to false unless third line of triangle is not already in line list and distance between outer points is less than cutoff distance
						//or if third line is in existing list but triangle is not in triangle list
						common_boundary_pt_found = false;

						Line_3D testLine;
						testLine.pt_1 = tmppt_3d;
						testLine.pt_2.x = (pt_1.x + pt_2.x)/2;
						testLine.pt_2.y = (pt_1.y + pt_2.y)/2;
						testLine.pt_2.z = (pt_1.z + pt_2.z)/2;
						testLine.pt_2.pt_index = -1;
						if (line_intersect_any_other_line(testLine, line_3d_list, minimum_distance) == true)
							continue;
						testLine.pt_2.x = (pt_1.x + 2 * pt_2.x)/3;
						testLine.pt_2.y = (pt_1.y + 2 * pt_2.y)/3;
						testLine.pt_2.z = (pt_1.z + 2 * pt_2.z)/3;
						if (line_intersect_any_other_line(testLine, line_3d_list, minimum_distance) == true)
							continue;

						testLine.pt_1 = pt_1;
						testLine.pt_2.x = (tmppt_3d.x + pt_2.x)/2;
						testLine.pt_2.y = (tmppt_3d.y + pt_2.y)/2;
						testLine.pt_2.z = (tmppt_3d.z + pt_2.z)/2;
						testLine.pt_2.pt_index = -1;
						if (line_intersect_any_other_line(testLine, line_3d_list, minimum_distance) == true)
							continue;
						testLine.pt_2.x = (tmppt_3d.x + 2 * pt_2.x)/3;
						testLine.pt_2.y = (tmppt_3d.y + 2 * pt_2.y)/3;
						testLine.pt_2.z = (tmppt_3d.z + 2 * pt_2.z)/3;
						if (line_intersect_any_other_line(testLine, line_3d_list, minimum_distance) == true)
							continue;

						testLine.pt_1 = pt_2;
						testLine.pt_2.x = (pt_1.x + tmppt_3d.x)/2;
						testLine.pt_2.y = (pt_1.y + tmppt_3d.y)/2;
						testLine.pt_2.z = (pt_1.z + tmppt_3d.z)/2;
						testLine.pt_2.pt_index = -1;
						if (line_intersect_any_other_line(testLine, line_3d_list, minimum_distance) == true)
							continue;
						testLine.pt_2.x = (pt_1.x + 2 * tmppt_3d.x)/3;
						testLine.pt_2.y = (pt_1.y + 2 * tmppt_3d.y)/3;
						testLine.pt_2.z = (pt_1.z + 2 * tmppt_3d.z)/3;
						if (line_intersect_any_other_line(testLine, line_3d_list, minimum_distance) == true)
							continue;

						tmpline_3d.pt_1 = pt_1;
						tmpline_3d.pt_2 = pt_2;
						line_in_list = false;
						for (line_iter_2 = line_3d_list.begin(); line_iter_2 != line_3d_list.end(); line_iter_2++)
						{
							if (tmpline_3d == *line_iter_2)
							{
								line_in_list = true;
								tmpline_3d = *line_iter_2;
								break;
							}
						}
						if (line_in_list) 
						{
							tmptriangle_3d.line_1 = *line_iter;
							tmptriangle_3d.line_2 = *line_iter_1;
							tmptriangle_3d.line_3 = tmpline_3d;	
							triangle_in_list = false;
							for (triangle_iter = triangle_3d_list.begin(); triangle_iter != triangle_3d_list.end(); triangle_iter++)
							{
								if (tmptriangle_3d == *triangle_iter)
								{
									triangle_in_list = true;
									break;
								}
							}
							if (triangle_in_list) continue;
							line_iter->isBoundary = false;
							line_iter_1->isBoundary = false;
							line_iter_2->isBoundary = false;
							tmpline_3d = *line_iter_2;
						}
						else
						{	
							if ((abs(pt_2.x - pt_1.x) > minimum_distance) || (abs(pt_2.y - pt_1.y) > minimum_distance) || (abs(pt_2.z - pt_1.z) > minimum_distance)) continue;
					
							dist = hM->pythag(pt_2.x - pt_1.x, pt_2.y - pt_1.y, pt_2.z - pt_1.z);
							if (dist > minimum_distance) continue;
							if (line_intersect_any_other_line(tmpline_3d, line_3d_list, minimum_distance) == true)
								continue;

							line_iter->isBoundary = false;
							line_iter_1->isBoundary = false;
							tmpline_3d.isBoundary = true;
							tmpline_3d.line_index = line_index++;
							line_3d_list.push_back(tmpline_3d);
						}
						common_boundary_pt_found = true;

						tmptriangle_3d.line_1 = *line_iter;
						tmptriangle_3d.line_2 = *line_iter_1;
						tmptriangle_3d.line_3 = tmpline_3d;
						tmptriangle_3d.triangle_index = triangle_index++;
						triangle_3d_list.push_back(tmptriangle_3d);
					}
					if (common_boundary_pt_found) break;
				}
			}
		}
		
		//now populate triangle list array for sending back
		*trianglescount = triangle_index;
		*trianglelist = (double*) malloc(9*triangle_index*sizeof(double));
		int i = 0;
		int index1, index2, index3;
		for (triangle_iter = triangle_3d_list.begin(); triangle_iter != triangle_3d_list.end(); triangle_iter++)
		{
			*(*trianglelist + i) = triangle_iter->line_1.pt_1.x; i++;
			*(*trianglelist + i) = triangle_iter->line_1.pt_1.y; i++;
			*(*trianglelist + i) = triangle_iter->line_1.pt_1.z; i++;

			*(*trianglelist + i) = triangle_iter->line_1.pt_2.x; i++;
			*(*trianglelist + i) = triangle_iter->line_1.pt_2.y; i++;
			*(*trianglelist + i) = triangle_iter->line_1.pt_2.z; i++;

			index1 = triangle_iter->line_1.pt_1.pt_index;
			index2 = triangle_iter->line_1.pt_2.pt_index;
			index3 = triangle_iter->line_2.pt_1.pt_index;
			if ((index3 == index1) || (index3 == index2))
				index3 = triangle_iter->line_2.pt_2.pt_index;

			*(*trianglelist + i) = *(ptslist + 3 * index3); i++;
			*(*trianglelist + i) = *(ptslist + 3 * index3 + 1); i++;
			*(*trianglelist + i) = *(ptslist + 3 * index3 + 2); i++;
		}

		triangle_3d_list.clear();
		line_3d_list.clear();
		pts_3d_list_variable.clear();

		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0053", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::BestFitSurface(double* pts, int PtsCount, double precision, Surface_3DList* p_surface_list, int step, bool pts_around_xy_plane)
{
	try
	{
		//take best fit plane, cylinder, cone and sphere with "precision" and check if any of them returns true
		//if yes, take the one which has least value for lsq_dist.
		//if none of them returns true, divide points collection into two parts by taking an arbitrary point and defining all points 
		//falling less than half the max distance in a temporary points collection.
		//for this collection repeat check of best fit plane, cylinder, cone and sphere with precision.
		//keep proceeding this way narrowing down the temporary points collection till we come with a valid best fit surface.
		//remove all points of original point collection belonging to the temporary points collection and 
		//repeat above steps with rest of points till we have no more points left.
		double tempAnswer[10] = {0};
		double Answer[10] = {0};
		Shapes_3D shape = nullshape;
		double lsq_dist = precision;
		int num_pts = PtsCount;
		double first_pt[3] = {0};
		int first_pt_index = 0;
		double* tempptscollection = NULL;
		Pt_3D tmppt_3d;
		Pts_3DList pts_3d_list;
		Pts_3DList tempptslist;
		Pts_3DList tempptslistcpy;
		Surface_3D tmpSurface;
		int shape_index = 0;
		double dist = 0;
		double max_dist = 0;
		double best_fit_precision;

		if (precision == -1)
			best_fit_precision = 999999;
		p_surface_list->clear();


		double max_x =*pts, max_y=*(pts + 1), max_z=*(pts + 2), min_x=*pts, min_y=*(pts + 1), min_z=*(pts + 2);

		for (int i = 1; i < PtsCount; i++)
		{
			if (max_x < *(pts + 3 * i))
				max_x = *(pts + 3 * i); 
			if (max_y < *(pts + 3 * i + 1))
				max_y = *(pts + 3 * i + 1); 
			if (max_z < *(pts + 3 * i + 2))
				max_z = *(pts + 3 * i + 2); 

			if (min_x > *(pts + 3 * i))
				min_x = *(pts + 3 * i); 
			if (min_y > *(pts + 3 * i + 1))
				min_y = *(pts + 3 * i + 1); 
			if (min_z > *(pts + 3 * i + 2))
				min_z = *(pts + 3 * i + 2); 
		}

		int N_x = 1, N_y = 1, N_z = 1;
		double x = 0;
		double x_side = max_x - min_x;
		double y_side = max_y - min_y;
		double z_side = max_z - min_z;
	
		bool is_cube = true;
		if (pts_around_xy_plane)
			is_cube = false;

		if ((x_side == 0) || (y_side == 0) || (z_side == 0))
			is_cube = false;

		double min_side = z_side;
		if (x_side < min_side) min_side = x_side;
		if (y_side < min_side) min_side = y_side;
		if (is_cube)
		{
			if ((min_side/x_side < 0.1) || (min_side/y_side < 0.1) || (min_side/z_side < 0.1))
				is_cube = false;
		}
		//approx side of cube occupied by points that are step apart
		if (is_cube)
		{
			x = pow((x_side * y_side * z_side * step / PtsCount), (double) 1/3);
		}
		else
		{	
			//approx side of square occupied by points that are step apart
			if (min_side == x_side)
			{
				if ((y_side == 0) || (z_side == 0))
					return false;
				if ((y_side/z_side < 0.1) || (z_side/y_side < 0.1))
					return false;
				x = pow(( y_side * z_side * step / PtsCount), (double) 1/2);
			}
			else if (min_side == z_side)
			{
				if ((y_side == 0) || (x_side == 0))
					return false;
				if ((y_side/x_side < 0.1) || (x_side/y_side < 0.1))
					return false;
				x = pow((x_side * y_side * step / PtsCount), (double) 1/2);
			}
			else
			{
				if ((x_side == 0) || (z_side == 0))
					return false;
				if ((x_side/z_side < 0.1) || (z_side/x_side < 0.1))
					return false;
				x = pow((x_side * z_side * step / PtsCount), (double) 1/2);
			}
		}
	
		N_x = (int) (x_side / x) + 1;
		N_y = (int) (y_side / x) + 1;
		N_z = (int) (z_side / x) + 1;
	
		int* Occupancy = NULL;
		Occupancy = (int*) malloc(N_x * N_y * N_z * sizeof(int));
		memset(Occupancy, 0, N_x * N_y * N_z * sizeof(int));
		int a = 0, b = 0, c = 0;
		double ax = 0, by = 0, cz = 0;
		for (int i = 0; i < PtsCount; i++)
		{
			if (max_x != min_x)
				ax = (*(pts + 3 * i) - min_x) * (N_x - 1) / (max_x - min_x);
			else
				ax = 0;
			a = (int) ax;

			if (max_y != min_y)
				by = (*(pts + 3 * i + 1) - min_y) * (N_y - 1) / (max_y - min_y);
			else
				by = 0;
			b = (int) by;

			if (max_z != min_z)
				cz = (*(pts + 3 * i + 2) - min_z) * (N_z - 1) / (max_z - min_z);
			else
				cz = 0;
			c = (int) cz;

			if ((*(Occupancy + N_y*N_x*c + N_x*b + a) == 0) || (step == 1))
			{
				*(Occupancy + N_y*N_x*c + N_x*b + a) = *(Occupancy + N_y*N_x*c + N_x*b + a) + 1;
				tmppt_3d.x = *(pts + 3 * i); tmppt_3d.y = *(pts + 3 * i + 1); tmppt_3d.z = *(pts + 3 * i + 2); tmppt_3d.pt_index = i;
				pts_3d_list.push_back(tmppt_3d);
				tempptslist.push_back(tmppt_3d);
			}
		}

		free(Occupancy);
		num_pts = pts_3d_list.size();

		tempptscollection = (double*) malloc(3*sizeof(double)*num_pts);
	
		while (num_pts >= 3)
		{
			memset(tempptscollection,0,3*sizeof(double)*num_pts);
			int i = 0;
		
			for (Pts_3DList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
			{
				*(tempptscollection + 3*i + 0) = ptr_iter->x;
				*(tempptscollection + 3*i + 1) = ptr_iter->y;
				*(tempptscollection + 3*i + 2) = ptr_iter->z;
				i++;
			}
			shape = nullshape;
			memset(tempAnswer,0,10*sizeof(double));
			if (precision != -1)
				best_fit_precision = precision * num_pts;
			lsq_dist = best_fit_precision;
			if (Plane_BestFit(tempptscollection, num_pts, tempAnswer, true, best_fit_precision))
			{
				lsq_dist = tempAnswer[4];
				memcpy(Answer, tempAnswer, 10*sizeof(double));
				shape = plane;
			}
		
			if (((shape == nullshape) || (precision == -1)) && (num_pts >= 5))
			{
				memset(tempAnswer,0,10*sizeof(double));
				if (Cylinder_BestFit(tempptscollection, num_pts, tempAnswer, best_fit_precision))
				{
					if (tempAnswer[7] < lsq_dist)
					{
						lsq_dist = tempAnswer[7];
						memcpy(Answer, tempAnswer, 10*sizeof(double));
						shape = cylinder;
					}
				}
			}
		
			if (((shape == nullshape) || (precision == -1)) && (num_pts >= 6))
			{
				memset(tempAnswer,0,10*sizeof(double));
				if (Cone_BestFit(tempptscollection, num_pts, tempAnswer, best_fit_precision))
				{
					if (tempAnswer[8] < lsq_dist)
					{
						lsq_dist = tempAnswer[8];
						memcpy(Answer, tempAnswer, 10*sizeof(double));
						shape = cone;
					}
				}
			}
			if (((shape == nullshape) || (precision == -1)) && (num_pts >= 4))
			{
				memset(tempAnswer,0,10*sizeof(double));
				if (Sphere_BestFit(tempptscollection, num_pts, tempAnswer, true, best_fit_precision))
				{
					if (tempAnswer[4] < lsq_dist)
					{
						lsq_dist = tempAnswer[4];
						memcpy(Answer, tempAnswer, 10*sizeof(double));
						shape = sphere;
					}
				}
			}
			//if shape == nullshape, take first point as origin and get max distance from here by going through all points of points list.
			//take all points at distance of < 1/2 max distance from origin in temporary points collection.  
			//for this list repeat above checks in a loop.
			//if shape != nullshape, store all points of temporary points collection along with the best fit surface retrieved in a best fit surface list.
			//remove all points of temporary points collection from original points list and repeat above steps till we have no further points left.
			//return the best fit surface list.

			if (shape == nullshape)
			{
				tmppt_3d = tempptslist.front();
				first_pt[0] = tmppt_3d.x; first_pt[1] = tmppt_3d.y; first_pt[2] = tmppt_3d.z; first_pt_index = tmppt_3d.pt_index;

				bool flag = true;

				//get max distance from first point to all points in current point collection
				int i = 0;
				for (i = 1; i < num_pts; i++)
				{
					/*dist = sqrt (pow(first_pt[0] - *(tempptscollection + 3*i),2) + pow(first_pt[1] - *(tempptscollection + 3*i + 1), 2) + pow(first_pt[2] - *(tempptscollection + 3*i + 2), 2));*/
					dist = abs(first_pt[0] - *(tempptscollection + 3*i));
					if (abs(first_pt[1] - *(tempptscollection + 3*i + 1)) > dist)
							dist = abs(first_pt[1] - *(tempptscollection + 3*i + 1));
					if (abs(first_pt[2] - *(tempptscollection + 3*i + 2)) > dist)
						dist = abs(first_pt[2] - *(tempptscollection + 3*i + 2));

					if (flag)
					{
						max_dist = dist;
						flag = false;
					}
					if (dist > max_dist)
					{
						max_dist = dist;
					}
				}

				//choose only points from collection that are within half max distance away from first point to try for best fit within smaller collection
				tempptslistcpy.clear();
				for (Pts_3DList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
				{
					/*dist = sqrt (pow(first_pt[0] - ptr_iter->x,2) + pow(first_pt[1] - ptr_iter->y, 2) + pow(first_pt[2] - ptr_iter->z, 2));*/
					dist = abs(first_pt[0] - ptr_iter->x);
					if (abs(first_pt[1] - ptr_iter->y) > dist)
						dist = abs(first_pt[1] - ptr_iter->y);
					if (abs(first_pt[2] - ptr_iter->z) > dist)
						dist = abs(first_pt[2] - ptr_iter->z);
					if (dist < max_dist / 2)
					{
						tempptslistcpy.push_back(*ptr_iter);
					}
				}
				num_pts = tempptslistcpy.size();
				if (num_pts < 3)
				{
					double min_dist_1 = max_dist;
					double min_dist_2 = max_dist;
					Pts_3DList::iterator ptr_iter1 = tempptslist.begin();
					Pts_3DList::iterator ptr_iter2 = tempptslist.begin();
					//choose additional points.
					for (Pts_3DList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
					{
						dist = abs(first_pt[0] - ptr_iter->x);
						if (abs(first_pt[1] - ptr_iter->y) > dist)
							dist = abs(first_pt[1] - ptr_iter->y);
						if (abs(first_pt[2] - ptr_iter->z) > dist)
							dist = abs(first_pt[2] - ptr_iter->z);
						if (dist < max_dist /2)
							continue;
						if (dist < min_dist_2) 
						{
							if (dist > min_dist_1)
							{
								min_dist_2 = dist;
								ptr_iter2 = ptr_iter;
							}
							else
							{
								min_dist_2 = min_dist_1;
								min_dist_1 = dist;
								ptr_iter2 = ptr_iter1;
								ptr_iter1 = ptr_iter;
							}
						}
					}
					if (min_dist_1 < max_dist)
					{
						tempptslistcpy.push_back(*ptr_iter1);
						num_pts++;
					}
					if (min_dist_2 < max_dist)
					{
						tempptslistcpy.push_back(*ptr_iter2);
						num_pts++;
					}
				}
			}
			else
			{
				tempptslistcpy.clear();
				double dev;
				if (precision != -1)
					dev = precision;
				else
					dev = lsq_dist / (tempptslist.size());
				//check for additional points that may fall in the shape within deviation determined by lsq_dist.
				//for those that are not, add them to remaining points list.

				Pts_3DList neighboring_pts_list;
				Pts_3DList neighboring_pts_list2;
				Pts_3DList tempptslistcpy2;
				for (Pts_3DList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
				{
					neighboring_pts_list.push_back(*ptr_iter);
				}

				for (Pts_3DList::iterator ptr_iter = pts_3d_list.begin(); ptr_iter != pts_3d_list.end(); ptr_iter++)
				{
					bool pt_in_list = false;
					for (Pts_3DList::iterator ptr_iter1 = tempptslist.begin(); ptr_iter1 != tempptslist.end(); ptr_iter1++)
					{
						if (ptr_iter1->pt_index == ptr_iter->pt_index) 
						{
							pt_in_list = true;
							break;
						}
					}
					if (pt_in_list == false)
					{
						tempptslistcpy.push_back(*ptr_iter);
					}
				}
				while (neighboring_pts_list.size() > 0)
				{
					for (Pts_3DList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
					{
						tempptslistcpy2.push_back(*ptr_iter);
					}
					for (Pts_3DList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
					{
						bool is_neighboring_pt = false;
						for (Pts_3DList::iterator ptr_iter2 = neighboring_pts_list.begin(); ptr_iter2 !=  neighboring_pts_list.end(); ptr_iter2++)
						{
							/*dist = sqrt (pow(ptr_iter2->x - ptr_iter->x,2) + pow(ptr_iter2->y - ptr_iter->y, 2) + pow(ptr_iter2->z - ptr_iter->z, 2));*/
							dist = abs(ptr_iter2->x - ptr_iter->x);
							if (abs(ptr_iter2->y - ptr_iter->y) > dist)
								dist = abs(ptr_iter2->y - ptr_iter->y) ;
							if (abs(ptr_iter2->z - ptr_iter->z) > dist)
								dist = abs(ptr_iter2->z - ptr_iter->z);
							if (dist < max_dist / 2)
							{
								is_neighboring_pt = true;
								break;
							}
						}
						if (is_neighboring_pt == true)
						{
							if (pt_in_shape(shape, Answer, *ptr_iter, dev))
							{
								tempptslist.push_back(*ptr_iter);
								neighboring_pts_list2.push_back(*ptr_iter);
								tempptslistcpy2.remove(*ptr_iter);
							}
						}
					}
					tempptslistcpy.clear();
					for (Pts_3DList::iterator ptr_iter = tempptslistcpy2.begin(); ptr_iter != tempptslistcpy2.end(); ptr_iter++)
					{
						tempptslistcpy.push_back(*ptr_iter);
					}
					tempptslistcpy2.clear();
					neighboring_pts_list.clear();
					for (Pts_3DList::iterator ptr_iter = neighboring_pts_list2.begin(); ptr_iter != neighboring_pts_list2.end(); ptr_iter++)
					{
						neighboring_pts_list.push_back(*ptr_iter);
					}
					neighboring_pts_list2.clear();
				}

				//populate temp surface and add to surface list.
				tmpSurface.shape = shape;
				tmpSurface.shape_index = shape_index++;
				memcpy(tmpSurface.answer, Answer, 10*sizeof(double));
				tmpSurface.pts_3d_list.clear();
				for (Pts_3DList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
				{
					tmpSurface.pts_3d_list.push_back(*ptr_iter);
				}
				p_surface_list->push_back(tmpSurface);

				//prepare for next iteration - points list will contain remaining set of points only now
				num_pts = tempptslistcpy.size();
				pts_3d_list.clear();
				for (Pts_3DList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
				{
					pts_3d_list.push_back(*ptr_iter);
				}
			}
			//if we have stand alone points not falling in any shape, remove them from points collection and prepare for next iteration
			if ((shape == nullshape) && (num_pts < 3))
			{
				for (Pts_3DList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
				{
					pts_3d_list.remove(*ptr_iter);
				}
				num_pts = pts_3d_list.size();
				tempptslistcpy.clear();
				for (Pts_3DList::iterator ptr_iter = pts_3d_list.begin(); ptr_iter != pts_3d_list.end(); ptr_iter++)
				{
					tempptslistcpy.push_back(*ptr_iter);
				}
			}

			tempptslist.clear();
		
			for (Pts_3DList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
			{
				tempptslist.push_back(*ptr_iter);
			}
		}

		free (tempptscollection);
		pts_3d_list.clear();
		tempptslist.clear();
		tempptslistcpy.clear();
		return true;
	}
	catch(...) 
	{
		SetAndRaiseErrorMessage("RBF0054", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::pt_in_shape(Shapes_3D shape, double* Answer, Pt_3D pt, double deviation)
{
	try
	{
		double sq_dist_from_pt = 0;
		double axial_dist = 0;
		double sq_radial_dist = 0;
		double exp_radial_dist = 0;
		switch (shape)
		{
			case plane:
				if (abs(*(Answer + 0) * pt.x + *(Answer + 1) * pt.y + *(Answer + 2) * pt.z + *(Answer + 3)) < deviation)
					return true;
				else 
					return false;
				break;
			case sphere:
				if (abs(sqrt(pow (pt.x - *(Answer + 0), 2) + pow (pt.y - *(Answer + 1), 2)+ pow (pt.z - *(Answer + 2), 2)) - *(Answer + 3)) < deviation)
					return true;
				else
					return false;
				break;
			case cylinder:
				sq_dist_from_pt = pow (pt.x - *(Answer + 0), 2) + pow (pt.y - *(Answer + 1), 2)+ pow (pt.z - *(Answer + 2), 2);
				axial_dist = (pt.x - *(Answer + 0)) * *(Answer + 3) + (pt.y - *(Answer + 1)) * *(Answer + 4) + (pt.z - *(Answer + 2)) * *(Answer + 5);
				sq_radial_dist = sq_dist_from_pt - pow(axial_dist, 2);
				if (abs(sqrt(sq_radial_dist) - *(Answer + 6)) < deviation)
					return true;
				else
					return false;
				break;
			case cone:
				sq_dist_from_pt = pow (pt.x - *(Answer + 0), 2) + pow (pt.y - *(Answer + 1), 2)+ pow (pt.z - *(Answer + 2), 2);
				axial_dist = (pt.x - *(Answer + 0)) * *(Answer + 3) + (pt.y - *(Answer + 1)) * *(Answer + 4) + (pt.z - *(Answer + 2)) * *(Answer + 5);
				sq_radial_dist = sq_dist_from_pt - pow(axial_dist, 2);
				exp_radial_dist = *(Answer + 6) + axial_dist * tan(*(Answer + 7));
				if (abs((sqrt(sq_radial_dist) - exp_radial_dist) * cos(*(Answer + 7))) < deviation)
					return true;
				else
					return false;
				break;
		}
		//will reach here only if shape not found in above list, so return false
		return false;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0055", __FILE__, __FUNCSIG__);return false;}
}
bool RBF::BestFitCurve(double* pts, int PtsCount, double precision_line, double precision_arc, Curve_2DList* p_curve_list, int step)
{
	try
	{

		//take best fit line and arc with "precision" and check if either of them returns true
		//if yes, take the one which has least value for lsq_dist.
		//if none of them returns true, divide points collection into two parts by taking an arbitrary point and defining all points 
		//falling less than half the max distance in a temporary points collection.
		//for this collection repeat check of best fit line or circle with precision.
		//keep proceeding this way narrowing down the temporary points collection till we come with a valid best fit curve.
		//remove all points of original point collection belonging to the temporary points collection and 
		//repeat above steps with rest of points till we have no more points left.
		double tempAnswer[4] = {0};
		double Answer[4] = {0};
		Curves_2D curve = nullcurve;
		double lsq_dist;
		int num_pts = PtsCount;
		double first_pt[2] = {0};
		int first_pt_index = 0;
		double* tempptscollection = NULL;
		Pt tmppt;
		PtsList pts_list;
		PtsList tempptslist;
		PtsList tempptslistcpy;
		Curve_2D tmpCurve;
		int curve_index = 0;
		double dist = 0;
		double max_dist = 0;

		p_curve_list->clear();

		for (int i = 0; i < PtsCount; i+= step)
		{
			tmppt.x = *(pts + 2 * i); tmppt.y = *(pts + 2 * i + 1); tmppt.pt_index = i;
			pts_list.push_back(tmppt);
			tempptslist.push_back(tmppt);
		}

		num_pts = pts_list.size();
		  
		tempptscollection = (double*) malloc(2*sizeof(double)*num_pts);
	
		while (num_pts >= 2)
		{
			memset(tempptscollection,0,2*sizeof(double)*num_pts);
			int i = 0;
		
			for (PtsList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
			{
				*(tempptscollection + 2*i + 0) = ptr_iter->x;
				*(tempptscollection + 2*i + 1) = ptr_iter->y;
				i++;
			}
			curve = nullcurve;
			memset(tempAnswer,0,4*sizeof(double));
			if (Line_BestFit_2D(tempptscollection, num_pts, tempAnswer, false, precision_line))
			{
				lsq_dist = tempAnswer[2];
				memcpy(Answer, tempAnswer, 4*sizeof(double));
				curve = line;
			}
		
			memset(tempAnswer,0,4*sizeof(double));
			if (curve == nullcurve)
			{
				if (num_pts >=3)
				{
					if (Circle_BestFit(tempptscollection, num_pts, tempAnswer, false, precision_arc))
					{
						lsq_dist = tempAnswer[3];
						memcpy(Answer, tempAnswer, 4*sizeof(double));
						curve = arc;
					}
				}
			}
		
			//if curve == nullcurve, take first point as origin and get max distance from here by going through all points of points list.
			//take all points at distance of < 1/2 max distance from origin in temporary points collection.  
			//for this list repeat above checks in a loop.
			//if curve != nullcurve, store all points of temporary points collection along with the best fit curve retrieved in a best fit curve list.
			//remove all points of temporary points collection from original points list and repeat above steps till we have no further points left.
			//return the best fit curve list.

			if (curve == nullcurve)
			{
				tmppt = tempptslist.front();
				first_pt[0] = tmppt.x; first_pt[1] = tmppt.y; first_pt_index = tmppt.pt_index;

				bool flag = true;

				//get max distance from first point to all points in current point collection
				int i = 0;
				for (i = 1; i < num_pts; i++)
				{
					dist = abs(first_pt[0] - *(tempptscollection + 2*i));
					if (abs(first_pt[1] - *(tempptscollection + 2*i + 1)) > dist)
						dist = abs(first_pt[1] - *(tempptscollection + 2*i + 1));
					if (flag)
					{
						max_dist = dist;
						flag = false;
					}
					if (dist > max_dist)
					{
						max_dist = dist;
					}
				}

				//choose only points from collection that are within half max distance away from first point to try for best fit within smaller collection
				tempptslistcpy.clear();
				for (PtsList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
				{
					dist = abs(first_pt[0] - ptr_iter->x);
					if (abs(first_pt[1] - ptr_iter->y) > dist)
						dist = abs(first_pt[1] - ptr_iter->y);
					if (dist < max_dist / 2)
					{
						tempptslistcpy.push_back(*ptr_iter);
					}
				}
				num_pts = tempptslistcpy.size();
				if (num_pts < 2)
				{
					double min_dist_1 = max_dist;
					PtsList::iterator ptr_iter1 = tempptslist.begin();
					//choose additional points.
					for (PtsList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
					{
						dist = abs(first_pt[0] - ptr_iter->x);
						if (abs(first_pt[1] - ptr_iter->y) > dist)
							dist = abs(first_pt[1] - ptr_iter->y);
						if (dist < max_dist /2)
							continue;
						if (dist < min_dist_1) 
						{
							min_dist_1 = dist;
							ptr_iter1 = ptr_iter;
						}
					}
					if (min_dist_1 < max_dist)
					{
						tempptslistcpy.push_back(*ptr_iter1);
						num_pts++;
					}
				}
			}
			else
			{
				tempptslistcpy.clear();
				double dev = lsq_dist / (tempptslist.size());
				//check for additional points that are neighboring to the points list of current curve that fall in the curve within deviation determined by lsq_dist.
				//for those that are not, add them to remaining points list.
				PtsList neighboring_pts_list;
				PtsList neighboring_pts_list2;
				PtsList tempptslistcpy2;
				for (PtsList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
				{
					neighboring_pts_list.push_back(*ptr_iter);
				}
				for (PtsList::iterator ptr_iter = pts_list.begin(); ptr_iter != pts_list.end(); ptr_iter++)
				{
					bool pt_in_list = false;
					for (PtsList::iterator ptr_iter1 = tempptslist.begin(); ptr_iter1 != tempptslist.end(); ptr_iter1++)
					{
						if (ptr_iter1->pt_index == ptr_iter->pt_index) 
						{
							pt_in_list = true;
							break;
						}
					}
					if (pt_in_list == false)
					{
						tempptslistcpy.push_back(*ptr_iter);
					}
				}
				while (neighboring_pts_list.size() > 0)
				{
					for (PtsList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
					{
						tempptslistcpy2.push_back(*ptr_iter);
					}
					for (PtsList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
					{
						bool is_neighboring_pt = false;
						for (PtsList::iterator ptr_iter2 = neighboring_pts_list.begin(); ptr_iter2 !=  neighboring_pts_list.end(); ptr_iter2++)
						{
							dist = abs(ptr_iter2->x - ptr_iter->x);
							if (abs(ptr_iter2->y - ptr_iter->y) > dist )
								dist = abs(ptr_iter2->y - ptr_iter->y) ;
							if (dist < max_dist / 2)
							{
								is_neighboring_pt = true;
								break;
							}
						}
						if (is_neighboring_pt == true)
						{
							if (pt_in_curve(curve, Answer, *ptr_iter, dev))
							{
								tempptslist.push_back(*ptr_iter);
								neighboring_pts_list2.push_back(*ptr_iter);
								tempptslistcpy2.remove(*ptr_iter);
							}
						}
					}
					tempptslistcpy.clear();
					for (PtsList::iterator ptr_iter = tempptslistcpy2.begin(); ptr_iter != tempptslistcpy2.end(); ptr_iter++)
					{
						tempptslistcpy.push_back(*ptr_iter);
					}
					tempptslistcpy2.clear();
					neighboring_pts_list.clear();
					for (PtsList::iterator ptr_iter = neighboring_pts_list2.begin(); ptr_iter != neighboring_pts_list2.end(); ptr_iter++)
					{
						neighboring_pts_list.push_back(*ptr_iter);
					}
					neighboring_pts_list2.clear();
				}

				//populate temp curve and add to curve list.
				tmpCurve.curve = curve;
				tmpCurve.curve_index = curve_index++;
				memcpy(tmpCurve.answer, Answer, 4*sizeof(double));
				tmpCurve.pts_list.clear();
				/*orderpoints(&tempptslist);*/
				for (PtsList::iterator ptr_iter = tempptslist.begin(); ptr_iter != tempptslist.end(); ptr_iter++)
				{
					tmpCurve.pts_list.push_back(*ptr_iter);
				}
				p_curve_list->push_back(tmpCurve);

				//prepare for next iteration - points list will contain remaining set of points only now
				num_pts = tempptslistcpy.size();
				pts_list.clear();
				for (PtsList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
				{
					pts_list.push_back(*ptr_iter);
				}
			}
			//if we have stand alone points not falling in any shape, remove them from points collection and prepare for next iteration
			if ((curve == nullcurve) && (num_pts < 2))
			{
				pts_list.remove(tmppt);
				num_pts = pts_list.size();
				tempptslistcpy.clear();
				for (PtsList::iterator ptr_iter = pts_list.begin(); ptr_iter != pts_list.end(); ptr_iter++)
				{
					tempptslistcpy.push_back(*ptr_iter);

				}
			}

			tempptslist.clear();
		
			for (PtsList::iterator ptr_iter = tempptslistcpy.begin(); ptr_iter != tempptslistcpy.end(); ptr_iter++)
			{
				tempptslist.push_back(*ptr_iter);

			}
		}

		free (tempptscollection);
		pts_list.clear();
		tempptslist.clear();
		tempptslistcpy.clear();
		return true;
	}
	catch (...)
	{
		SetAndRaiseErrorMessage("RBF0056", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::pt_in_curve(Curves_2D curve, double* Answer, Pt pt, double deviation)
{
	try
	{
		double point[2] = {0};
		point[0] = pt.x;
		point[1] = pt.y;
		switch (curve)
		{
			case line:
				if (abs(hM->Point_Line_Dist_2D(point, Answer)) < deviation)
					return true;
				else 
					return false;
				break;
			case arc:
				if (abs(sqrt(pow((point[0] - *(Answer+ 0)), 2) + pow((point[1] - *(Answer + 1)), 2)) - *(Answer + 2)) < deviation)
					return true;
				else
					return false;
				break;
		}
		//will reach here only if curve not found in above list, so return false
		return false;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0057", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::orderpoints(PtsList* ptr_ptslist, double* p_avgDist, int ptIndex)
{
	try
	{
		int TotalCnt = ptr_ptslist->size();
		if (TotalCnt == 0)
			return true;
		if (p_avgDist != NULL)
			*p_avgDist = 0;
		bool flag = true;
		PtsList tmpptslist, origPtslist;
		//if point index equal to -1, save original points list.  May be needed when retrying function with point index of an extreme point.
		if (ptIndex == -1)
		{
			for (PtsList::iterator pts_iter = ptr_ptslist->begin(); pts_iter != ptr_ptslist->end(); pts_iter++)
			{
				origPtslist.push_back(*pts_iter);
			}
		}		
		Pt firstpt;
		PtsList::iterator pt_iter2 = ptr_ptslist->begin();

		//if point index not equal to -1, get first point to start ordering points from.
		if (ptIndex != -1)
		{
			for(PtsList::iterator pts_iter1 = ptr_ptslist->begin(); pts_iter1 != ptr_ptslist->end(); pts_iter1++)
			{
				if (pts_iter1->pt_index == ptIndex)
				{
					pt_iter2 = pts_iter1;
					break;
				}
			}
		}
		firstpt.x = pt_iter2->x;
		firstpt.y = pt_iter2->y;
		firstpt.pt_index = pt_iter2->pt_index;

		tmpptslist.push_back(firstpt);
		ptr_ptslist->remove(firstpt);
		
		double dist_threshold = 0;
		bool firsttime = true;
		Pt tempPoint = firstpt;
		double avgDist = 0;
		for(int i = 0; i < TotalCnt - 1; i++)
		{
			Pt CurrentPt;
			double dist = 0, mindist = 0; flag = true;
			for(PtsList::iterator pts_iter = ptr_ptslist->begin(); pts_iter != ptr_ptslist->end(); pts_iter++)
			{
				double tempdist = 0;
				tempdist += pow((tempPoint.x - pts_iter->x), 2);  
				tempdist += pow((tempPoint.y - pts_iter->y), 2);  
				dist = sqrt(tempdist);
				if(flag)
				{ 
					mindist = dist; flag = false;
					CurrentPt.x = pts_iter->x; CurrentPt.y = pts_iter->y; CurrentPt.pt_index = pts_iter->pt_index;
				}
				else
				{
					if(mindist > dist)
					{
						mindist = dist;
						CurrentPt.x = pts_iter->x; CurrentPt.y = pts_iter->y; CurrentPt.pt_index = pts_iter->pt_index;
					}
				}
			}
			// if point index passed is -1, check for an extreme point during ordering of points.  Use this to start ordering points from.
			if (ptIndex == -1)
			{
				if (firsttime)
				{
					dist_threshold = mindist * 10;
					firsttime = false;
				}
				if (mindist > dist_threshold)
				{
					ptIndex = tempPoint.pt_index;
					ptr_ptslist->clear();
					tmpptslist.clear();
					for (PtsList::iterator pts_iter = origPtslist.begin(); pts_iter != origPtslist.end(); pts_iter++)
					{
						ptr_ptslist->push_back(*pts_iter);
					}
					origPtslist.clear();
					// use extreme point's point index to call the orderpoints function
					return orderpoints(ptr_ptslist, p_avgDist, ptIndex);
				}
			}
			avgDist += mindist;
			tmpptslist.push_back(CurrentPt);
			ptr_ptslist->remove(CurrentPt);
			tempPoint = CurrentPt;
		}
		if (TotalCnt > 1)
			avgDist = avgDist/(TotalCnt - 1);
		if (p_avgDist != NULL)
			*p_avgDist = avgDist;
		origPtslist.clear();
		ptr_ptslist->clear();

		for (PtsList::iterator pts_iter = tmpptslist.begin(); pts_iter != tmpptslist.end(); pts_iter++)
		{
			ptr_ptslist->push_back(*pts_iter);
		}

		tmpptslist.clear();
		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0058", __FILE__, __FUNCSIG__);return false;}
}
bool RBF::chk_in_triangle(double* pt, double* triangle, int triangle_index, double* min_d, int* pt_index)
{
	try
	{
		double d = 0;
		double v1[3] = {*(pt + 0) - *(triangle + 0), *(pt + 1) - *(triangle + 1), *(pt + 2) - *(triangle + 2)};
		double v2[3] = {*(pt + 0) - *(triangle + 3), *(pt + 1) - *(triangle + 4), *(pt + 2) - *(triangle + 5)};
		double v3[3] = {*(pt + 0) - *(triangle + 6), *(pt + 1) - *(triangle + 7), *(pt + 2) - *(triangle + 8)};

		double a1[3] = {*(triangle + 3) - *(triangle + 0), *(triangle + 4) - *(triangle + 1), *(triangle + 5) - *(triangle + 2)};
		double a2[3] = {*(triangle + 6) - *(triangle + 0), *(triangle + 7) - *(triangle + 1), *(triangle + 8) - *(triangle + 2)};

		double area1, area2, area3, area4;

		area_triangle(v1, v2, &area1);
		area_triangle(v2, v3, &area2);
		area_triangle(v3, v1, &area3);
		area_triangle(a1, a2, &area4);

		if (area1 + area2 + area3 - area4 > 0.1 * area4) 
		{
			d = v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2];
			if ((d < *min_d)||(*min_d == -1))
			{
				*min_d = d;
				*pt_index = 3 * triangle_index + 0;
			}
			d = v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2];
			if (d < *min_d)
			{
				*min_d = d;
				*pt_index = 3 * triangle_index + 1;
			}
			d = v3[0] * v3[0] + v3[1] * v3[1] + v3[2] * v3[2];
			if (d < *min_d)
			{
				*min_d = d;
				*pt_index = 3 * triangle_index + 2;
			}
			return false;
		}

		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0059", __FILE__, __FUNCSIG__); return false;}
}

void RBF::area_triangle(double* v1, double* v2, double* area)
{
	double w[3] = {*(v1 + 1) * *(v2 + 2) - *(v1 + 2) * *(v2 + 1), *(v1 + 2) * *(v2 + 0) - *(v1 + 0) * *(v2 + 2), *(v1 + 0) * *(v2 + 1) - *(v1 + 1) * *(v2 + 0)};
	*area = 0.5 * hM->pythag(w[0], w[1], w[2]);
	return;
}

bool RBF::twoDpt_inThreeDTriangleLst(double* pt, double* trianglelist, int trianglescount, int* triangle_index, double* z_coord, double dir_n_min_cutoff, double maxdist_from_nbr_pt)
{
	try
	{
		double curr_triangle[9] = {0};
		double pt_3d[3] = {*(pt + 0), *(pt + 1), 0};
		bool found_triangle = false;
		double min_d = -1;
		int pt_index = -1;
		for (int i = 0; i < trianglescount; i++)
		{
			memcpy(curr_triangle, trianglelist + 9*i, 9* sizeof(double));
			//convert to 2D triangle
			curr_triangle[2] = 0;
			curr_triangle[5] = 0;
			curr_triangle[8] = 0;

			if (chk_in_triangle(pt_3d, curr_triangle, i, &min_d, &pt_index) == true)
			{
				found_triangle = true;
				*triangle_index = i;
				break;
			}
		}
		if (found_triangle == false)
		{
			if ((min_d != -1) && (min_d <= maxdist_from_nbr_pt))
			{
				*z_coord = trianglelist[3 * pt_index + 2];
				return true;
			}
			return false;
		}
		double plane[4] = {0,0,1,0};

		//get plane of curr 3d triangle
		if (Plane_3Pt_2(trianglelist + 9* *(triangle_index), plane) == false)
			return false;

		//if plane is getting parallel to z axis (getting steep), return false
		if (abs(plane[2]) <= dir_n_min_cutoff) return false;

		//zcoord of 2d point passed returned below
		*z_coord = - (plane[0] * pt_3d[0] + plane[1] * pt_3d[1] + plane[3]) / plane[2];

		return true;
	}
	catch(...) {SetAndRaiseErrorMessage("RBF0060", __FILE__, __FUNCSIG__); return false;}
}

bool RBF::getZcoordInTriangleList(double* pts, int ptscount, double* trianglelist, int trianglescount, double dir_n_min_cutoff, double maxdist_from_nbr_pt)
{
	try
	{
		double curr_pt[3] = {0};
		double z_coord = 0;
		int triangle_index = 0;
		int n = 0;
		for (int i = 0; i < ptscount; i++)
		{
			memcpy(curr_pt, pts + 3*i, 3* sizeof(double));
			//if not found in triangle list, skip and continue.
			if (twoDpt_inThreeDTriangleLst(curr_pt, trianglelist, trianglescount, &triangle_index, &z_coord, dir_n_min_cutoff, maxdist_from_nbr_pt) == false)
			{
				n++;
				continue;
			}
			*(pts + 3*i + 2) = z_coord;
		}
		//if number of points not found is greater than 10% of points count, return false
		if (10 * n > ptscount) return false;

		return true;
	}
	catch(...) { SetAndRaiseErrorMessage("RBF0061", __FILE__, __FUNCSIG__);return false;}
}

bool RBF::getSphereBoundaryAngles(double* pts, int ptscount, double* sphereparam, double* boundaryangles)
{
	try
	{
		double theta_min = M_PI;
		double theta_max = 0;
		double phi_min = 2 * M_PI;
		double phi_max = 0;
		double theta = 0;
		double phi = 0;
		double rad[3] = {0};
		double radmod = 0;
		for (int i = 0; i < ptscount; i++)
		{
			rad[0] = (*(pts + 3 * i + 0) - *(sphereparam + 0)) ;
			rad[1] = (*(pts + 3 * i + 1) - *(sphereparam + 1)) ;
			rad[2] = (*(pts + 3 * i + 2) - *(sphereparam + 2)) ;
			radmod = sqrt (rad[0] * rad[0] + rad[1] * rad[1] + rad[2] * rad[2]);
			if (radmod == 0) continue;
			for (int j = 0; j < 3; j++)
				rad[j] = rad[j] / radmod;
			theta = acos (rad[2]);
			if (theta < theta_min)
				theta_min = theta;
			if (theta > theta_max)
				theta_max = theta;
			if ((rad[1] == 0) && (rad[0] == 0))
				continue;
			if (rad[0] == 0)
			{
				if (rad[1] > 0)
					phi = M_PI_2;
				else
					phi = 3 * M_PI_2;
			}
			else
			{
				phi = atan (rad[1]/rad[0]);
				if (rad[0] < 0)
				{
					phi += M_PI;
				}
				if (phi < 0 )
					phi += 2 * M_PI;
			}
			if (phi < phi_min)
				phi_min = phi;
			if (phi > phi_max)
				phi_max = phi;
		}
		//check for phi min and phi max being around 0 - in which case try to find correct phi min and phi max by moving zero to PI
		if ((phi_min < M_PI / 12) && (phi_max > (M_PI * 23) / 12))
		{
			phi_min = 3 * M_PI;
			phi_max = M_PI;
			for (int i = 0; i < ptscount; i++)
			{
				rad[0] = (*(pts + 3 * i + 0) - *(sphereparam + 0)) ;
				rad[1] = (*(pts + 3 * i + 1) - *(sphereparam + 1)) ;
				rad[2] = (*(pts + 3 * i + 2) - *(sphereparam + 2)) ;
				radmod = sqrt (rad[0] * rad[0] + rad[1] * rad[1] + rad[2] * rad[2]);
				if (radmod == 0) continue;
				for (int j = 0; j < 3; j++)
					rad[j] = rad[j] / radmod;
				if ((rad[1] == 0) && (rad[0] == 0))
					continue;
				if (rad[0] == 0)
				{
					if (rad[1] > 0)
						phi = 5 * M_PI_2;
					else
						phi = 3 * M_PI_2;
				}
				else
				{
					phi = atan (rad[1]/rad[0]);
					if (rad[0] < 0)
					{
						phi += M_PI;
					}
					if (phi < M_PI)
						phi += 2 * M_PI;
				}
				if (phi < phi_min)
					phi_min = phi;
				if (phi > phi_max)
					phi_max = phi;
			}
			//check for phi min and phi max being around PI - in which case retain phi min and phi max to 0 and 2 PI
			if ((phi_min < (M_PI * 13) / 12) && (phi_max > (M_PI * 35) / 12))
			{
				phi_min = 0;
				phi_max = 2 * M_PI;
			}
		}
		*(boundaryangles + 0) = theta_min;
		*(boundaryangles + 1) = theta_max;
		*(boundaryangles + 2) = phi_min;
		*(boundaryangles + 3) = phi_max;
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0062", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::getConeBoundaryAngles(double* pts, int ptscount, double* coneparam, double* boundaryangles)
{
	try
	{
		double phi_min = 2 * M_PI;
		double phi_max = 0;
		double phi = 0;
		double rad[3] = {0};
		double l = coneparam[3];
		double m = coneparam[4];
		double n = coneparam[5];
		double ez[3] = {0, 0, 1};
		double ez_dash[3] = {l, m, n};
		double ex_dash[3] = {1, 0, 0};
		double ey_dash[3] = {0, 1, 0};	
		double projn = sqrt(m*m + l*l);
		if (projn != 0) 
		{
			//unit vector along ez X ez_dash
			double a[3] = {-m/projn, l/projn, 0};
			//a X ez
			double b[3] = {l/projn, m/projn, 0};

			for (int i = 0; i < 3; i++)
			{
				ex_dash[i] = -m/projn * a[i] + l/projn * (b[i] * n - ez[i] * sqrt (1 - n * n) );
				ey_dash[i] = l/projn * a[i] + m/projn * (b[i] * n - ez[i] * sqrt (1 - n * n) );
			}
		}
		else if (n == -1)
			ey_dash[1] = -1;

		double zprojn = 0;
		double radmod = 0;
		double xproj = 0;
		double yproj = 0;
		for (int i = 0; i < ptscount; i++)
		{
			rad[0] = *(pts + 3 * i + 0) - *(coneparam + 0) ;
			rad[1] = *(pts + 3 * i + 1) - *(coneparam + 1) ;
			rad[2] = *(pts + 3 * i + 2) - *(coneparam + 2) ;
			zprojn = rad[0] * l + rad[1] * m + rad[2] * n;
			for (int j = 0; j < 3; j++)
				rad[j] = rad[j] - zprojn * ez_dash[j];
			radmod = sqrt (rad[0] * rad[0] + rad[1] * rad[1] + rad[2] * rad[2]);
			if (radmod == 0)
				continue;
			xproj = rad[0] * ex_dash[0] + rad[1] * ex_dash[1] + rad[2] * ex_dash[2];
			yproj = rad[0] * ey_dash[0] + rad[1] * ey_dash[1] + rad[2] * ey_dash[2];
			if (xproj == 0)
			{
				if (yproj > 0)
					phi = M_PI_2;
				else
					phi = 3 * M_PI_2;
			}
			else
			{
				phi = atan (yproj/xproj);
				if (xproj < 0)
				{
					phi += M_PI;
				}
				if (phi < 0 )
					phi += 2 * M_PI;
			}
			if (phi < phi_min)
				phi_min = phi;
			if (phi > phi_max)
				phi_max = phi;
		}

		//check for phi min and phi max being around 0 - in which case try to find correct phi min and phi max by moving zero to PI
		if ((phi_min < M_PI / 12) && (phi_max > (M_PI * 23) / 12))
		{
			phi_min = 3 * M_PI;
			phi_max = M_PI;
			for (int i = 0; i < ptscount; i++)
			{
				rad[0] = *(pts + 3 * i + 0) - *(coneparam + 0) ;
				rad[1] = *(pts + 3 * i + 1) - *(coneparam + 1) ;
				rad[2] = *(pts + 3 * i + 2) - *(coneparam + 2) ;
				zprojn = rad[0] * l + rad[1] * m + rad[2] * n;
				for (int j = 0; j < 3; j++)
					rad[j] = rad[j] - zprojn * ez_dash[j];
				radmod = sqrt (rad[0] * rad[0] + rad[1] * rad[1] + rad[2] * rad[2]);
				if (radmod == 0)
					continue;
				xproj = rad[0] * ex_dash[0] + rad[1] * ex_dash[1] + rad[2] * ex_dash[2];
				yproj = rad[0] * ey_dash[0] + rad[1] * ey_dash[1] + rad[2] * ey_dash[2];
				if (xproj == 0)
				{
					if (yproj > 0)
						phi = 5 * M_PI_2;
					else
						phi = 3 * M_PI_2;
				}
				else
				{
					phi = atan (yproj/xproj);
					if (xproj < 0)
					{
						phi += M_PI;
					}
					if (phi < M_PI)
						phi += 2 * M_PI;
				}
				if (phi < phi_min)
					phi_min = phi;
				if (phi > phi_max)
					phi_max = phi;
			}
			//check for phi min and phi max being around PI - in which case retain phi min and phi max to 0 and 2 PI
			if ((phi_min < (M_PI * 13) / 12) && (phi_max > (M_PI * 35) / 12))
			{
				phi_min = 0;
				phi_max = 2 * M_PI;
			}
		}
		*(boundaryangles + 0) = phi_min;
		*(boundaryangles + 1) = phi_max;
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0063", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::getProbeCorrectedPt(double* pts_array, int ptscount, bool threeD, double probe_dia, int dir, double* pt_in, double* pt_out, double upp)
{
	try
	{
		//dir is an integer from 0 to 5.  decreasing X is 0, increasing X is 1, decreasing Y is 2, increasing Y is 3, decreasing Z is 4, increasing Z is 5
		double movement_vector[3] = {0};
		switch(dir)
		{
		case 0:
			movement_vector[0] = -1;
			break;
		case 1:
			movement_vector[0] = 1;
			break;
		case 2:
			movement_vector[1] = -1;
			break;
		case 3:
			movement_vector[1] = 1;
			break;
		case 4:
			movement_vector[2] = -1;
			break;
		case 5:
			movement_vector[2] = 1;
			break;
		default:
			return false;
			break;
		}

		double nearest_pt1[3] = {0}, nearest_pt2[3] = {0};
		bool firsttime = true;
		double mind = 0;
		double currd = 0;
		double currd2 = 0;
		if (threeD)
		{
			int k = 0;
			for (int i = 0; i < ptscount; i++)
			{
				currd = abs(pts_array[3*i + 0] - pt_in[0]);
				if (abs(pts_array[3*i + 1] - pt_in[1]) > currd)
					currd = abs(pts_array[3*i + 1] - pt_in[1]);
				if (abs(pts_array[3*i + 2] - pt_in[2]) > currd)
					currd = abs(pts_array[3*i + 2] - pt_in[2]);
				if ((currd > upp) && ((firsttime) || (currd < mind)))
				{
					firsttime = false;
					nearest_pt1[0] = pts_array[3*i + 0]; nearest_pt1[1] = pts_array[3*i + 1]; nearest_pt1[2] = pts_array[3*i + 2];
					mind = currd;
					k = i;
				}
			}
			if (firsttime)
				return false;
			mind = 0;
			firsttime = true;
			for (int i = 0; i < ptscount; i++)
			{
				if (i == k)
					continue;

				currd = abs(pts_array[3*i + 0] - pt_in[0]);
				if (abs(pts_array[3*i + 1] - pt_in[1]) > currd)
					currd = abs(pts_array[3*i + 1] - pt_in[1]);
				if (abs(pts_array[3*i + 2] - pt_in[2]) > currd)
					currd = abs(pts_array[3*i + 2] - pt_in[2]);

				currd2 = abs(pts_array[3*i + 0] - nearest_pt1[0]);
				if (abs(pts_array[3*i + 1] - nearest_pt1[1]) > currd2)
					currd2 = abs(pts_array[3*i + 1] - nearest_pt1[1]);
				if (abs(pts_array[3*i + 2] - nearest_pt1[2]) > currd2)
					currd2 = abs(pts_array[3*i + 2] - nearest_pt1[2]);

				if ((currd > upp) && (currd2 > upp) && ((firsttime) || (currd < mind)))
				{
					firsttime = false;
					nearest_pt2[0] = pts_array[3*i + 0]; nearest_pt2[1] = pts_array[3*i + 1]; nearest_pt2[2] = pts_array[3*i + 2];
					mind = currd;
				}
			}
			if (firsttime)
				return false;
			double vector1[3] = {nearest_pt1[0] - pt_in[0], nearest_pt1[1] - pt_in[1], nearest_pt1[2] - pt_in[2]};
			double vector2[3] = {nearest_pt2[0] - pt_in[0], nearest_pt2[1] - pt_in[1], nearest_pt2[2] - pt_in[2]};
			double normal[3] = {vector1[1]*vector2[2] - vector1[2]*vector2[1], vector1[2]*vector2[0] - vector1[0]*vector2[2], vector1[0]*vector2[1] - vector1[1]*vector2[0]};
			double norm = sqrt(pow(normal[0],2) + pow(normal[1],2) + pow(normal[2],2));
			if (norm == 0)
				return false;
			for (int i = 0; i < 3; i++)
				normal[i] /= norm;
			double normal_projn_moving_vector = normal[0]*movement_vector[0] + normal[1]*movement_vector[1] + normal[2]*movement_vector[2];
			if (normal_projn_moving_vector < 0)
			{
				for (int i = 0; i < 3; i++)
					normal[i] = - normal[i];
			}
			for (int i = 0; i < 3; i++)
			{
				pt_out[i] = pt_in[i] + normal[i] * probe_dia / 2;
			}
		}
		else
		{
			//return false if movement is not in x or y directions for 2D case
			if ((dir != 0) && (dir != 1) && (dir != 2) && (dir != 3))
				return false;

			for (int i = 0; i < ptscount; i++)
			{
				currd = abs(pts_array[2*i + 0] - pt_in[0]);
				if (abs(pts_array[2*i + 1] - pt_in[1]) > currd)
					currd = abs(pts_array[2*i + 1] - pt_in[1]);

				if ((currd > upp) && ((firsttime) || (currd < mind)))
				{
					firsttime = false;
					nearest_pt1[0] = pts_array[2*i + 0]; nearest_pt1[1] = pts_array[2*i + 1]; 
					mind = currd;
				}
			}
			if (firsttime)
				return false;

			double vector1[2] = {nearest_pt1[0] - pt_in[0], nearest_pt1[1] - pt_in[1]};
			double normal[2] = {vector1[1], - vector1[0]};
			double norm = sqrt(pow(normal[0],2) + pow(normal[1],2));
			if (norm == 0)
				return false;
			for (int i = 0; i < 2; i++)
				normal[i] /= norm;
			double normal_projn_moving_vector = normal[0]*movement_vector[0] + normal[1]*movement_vector[1];
			if (normal_projn_moving_vector < 0)
			{
				for (int i = 0; i < 2; i++)
					normal[i] = - normal[i];
			}
			for (int i = 0; i < 2; i++)
			{
				pt_out[i] = pt_in[i] + normal[i] * probe_dia / 2;
			}
		}
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0064", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::BFalignment_2dpts_with_lines_arcs(double* pts_array, int ptscount, Curve_2DList curve_list, double* shift_x, double* shift_y, double* p_theta, double* rotn_origin, double resolution, int sample_size, int alignment_mode)
{
	try
	{
		double theta =0;
		double variance = 0;
		double min_variance = -1;
		double full_span_angle = 2* M_PI ;
		double* local_pts = NULL;
		double* pts_cpy = NULL;
		int l_step = 5;
		int ang_step = 16;
		int step = 1;

		if (ptscount > sample_size)
			step = ptscount/sample_size;
		int newptscount = ptscount/step;
		pts_cpy = (double*) malloc(2*sizeof(double)*newptscount);
		memset(pts_cpy,0,2*sizeof(double)*newptscount);
		local_pts = (double*) malloc(2*sizeof(double)*newptscount);
		memset(local_pts,0,2*sizeof(double)*newptscount);

		//redefine resolution as variance (ie sum of squares of resolution)
		resolution = newptscount * resolution * resolution;

		double cg_line_arc_x = 0;
		double cg_line_arc_y = 0;
		double tot_length = 0;

		//answer[6] will have slope, intercept, length for line; will have center[0], center[1], radius, length, start_ang, sweep_ang for arc
		for (Curve_2DList::iterator iter = curve_list.begin(); iter != curve_list.end(); iter++)
		{
			if (iter->curve == line)
			{
				if (iter->pts_list.size() >= 2)
				{
					cg_line_arc_x += (iter->pts_list.front().x + iter->pts_list.back().x) / 2 * (iter->answer[2]);
					cg_line_arc_y += (iter->pts_list.front().y + iter->pts_list.back().y) / 2 * (iter->answer[2]);
					tot_length += iter->answer[2];
				}
			}
			else //curve == arc
			{
				if (iter->answer[5] != 0)
				{
					cg_line_arc_x += 
						(iter->answer[0] + iter->answer[2] * (sin (iter->answer[4] + iter->answer[5]) - sin (iter->answer[4]) ) / (iter->answer[5])) * iter->answer[3];
					cg_line_arc_y +=
						(iter->answer[1] - iter->answer[2] * (cos (iter->answer[4] + iter->answer[5]) - cos (iter->answer[4]) ) / (iter->answer[5])) * iter->answer[3];
					tot_length += iter->answer[3];
				}
			}
		}

		cg_line_arc_x /= tot_length;
		cg_line_arc_y /= tot_length;

		double cg_pts_lst_x = 0;
		double cg_pts_lst_y = 0;
		int kk = 0;
		double min_x = *(pts_array);
		double max_x = min_x;
		double min_y = *(pts_array + 1);
		double max_y = min_y;
		for (int i = 0; i < newptscount; i++)
		{
			if (*(pts_array + 2*i*step) < min_x)
				min_x = *(pts_array + 2*i*step);
			if (*(pts_array + 2*i*step) > max_x)
				max_x = *(pts_array + 2*i*step);
			if (*(pts_array + 2*i*step + 1) < min_y)
				min_y = *(pts_array + 2*i*step + 1);
			if (*(pts_array + 2*i*step + 1) > max_y)
				max_y = *(pts_array + 2*i*step + 1);

			cg_pts_lst_x += *(pts_array + 2*i*step);
			cg_pts_lst_y += *(pts_array + 2*i*step + 1);
			kk++;
		}

		cg_pts_lst_x /= kk;
		cg_pts_lst_y /= kk;
		
		//alignment mode == 0 implies we want to rotate and translate points to get best fit
		//alignment mode == 1 implies we don't want to rotate points and only want to translate them to get best fit
		//alignment mode == 2 is when we don't want to translate points and only rotate them about their CG to find best fit, so cg_line_arc is taken same as cg_pts_lst
		if (alignment_mode == 1)
		{
			l_step = 1;
			ang_step = 1;
			full_span_angle = 0;
		}
		if (alignment_mode == 2)
		{
			cg_line_arc_x = cg_pts_lst_x;
			cg_line_arc_y = cg_pts_lst_y;
		}
		*shift_x = cg_line_arc_x - cg_pts_lst_x;
		*shift_y = cg_line_arc_y - cg_pts_lst_y;

		double rot_abt[2] = {cg_line_arc_x, cg_line_arc_y};

		*rotn_origin = rot_abt[0];
		*(rotn_origin + 1) = rot_abt[1];

		for (int i = 0; i < newptscount; i++)
		{
			pts_cpy[2*i] = pts_array[2*i*step] + *shift_x;
			pts_cpy[2*i + 1] = pts_array[2*i*step + 1] + *shift_y;
		}
	
		//call rotation function that takes arg as pts_cpy, curve_list, rot_abt and returns theta and variance.
		if (!getRotnAngleForPtsMatch(pts_cpy, newptscount, curve_list, rot_abt, p_theta, &min_variance, full_span_angle, resolution, l_step, ang_step))
		{
			free(pts_cpy);
			free(local_pts);
			return false;
		}
		if (min_variance < resolution)
		{
			*p_theta = theta;
			free(pts_cpy);
			free(local_pts);
			return true;
		}
		
		if (alignment_mode == 2)
		{
			free(pts_cpy);
			free(local_pts);
			return true;
		}

		double min_incrangle = 0;
		double x_max = 0;
		double y_max = 0;
//nudge logic to get value of x_nudge and y_nudge where variance is minimum
		if (alignment_mode == 1)
		{
			double x_nudge =0;
			double y_nudge =0;

			double full_span_x = (max_x - min_x)/2;
			double full_span_y = (max_y - min_y)/2;
			int l=0;

			theta = *p_theta;

			//rotate points by angle theta around rot_abt
			for (int i = 0; i < newptscount; i++)
			{
				local_pts[2*i] = rot_abt[0] + (pts_cpy[2*i] - rot_abt[0]) * cos(theta) - (pts_cpy[2*i + 1] - rot_abt[1]) * sin(theta);
				local_pts[2*i + 1] = rot_abt[1] + (pts_cpy[2*i] - rot_abt[0]) * sin(theta) + (pts_cpy[2*i + 1] - rot_abt[1]) * cos(theta);
			}
			memcpy(pts_cpy, local_pts, 2*sizeof(double)*newptscount);
			double incrangle = 0;
		

			while (l<4)
			{
				y_nudge = y_max - 3*full_span_y/4;
				x_nudge = x_max - 3*full_span_x/4;
				for (int k_y=0; k_y<5;k_y++)
				{
					y_nudge += full_span_y/4;
					for (int k_x=0; k_x<5;k_x++)
					{
						x_nudge += full_span_x/4;

						//shift points by x_nudge and y_nudge
						for (int i = 0; i < newptscount; i++)
						{
							local_pts[2*i] = pts_cpy[2*i] + x_nudge;
							local_pts[2*i + 1] = pts_cpy[2*i + 1] + y_nudge;
						}
						//get variance, ie sum of square of distances, of points from line-arc list
						double newrot_abt[2] = {rot_abt[0] + x_nudge, rot_abt[1] + y_nudge};
						variance = min_variance;
						if (!getRotnAngleForPtsMatch(local_pts, newptscount, curve_list, newrot_abt, &incrangle, &variance, 0, resolution, 1, 1))
						/*if (!getRotnAngleForPtsMatch(local_pts, newptscount, curve_list, newrot_abt, &incrangle, &variance, full_span_angle/16, resolution, 4, 4))*/
						{
							free(pts_cpy);
							free(local_pts);
							return false;
						}
						//if variance less than resolution, update shift_x, shift_y, rot_abt and return true
						if (variance < resolution)
						{
							*shift_x += x_nudge;
							*shift_y += y_nudge;
							rot_abt[0] += x_nudge;
							rot_abt[1] += y_nudge;
							*rotn_origin = rot_abt[0];
							*(rotn_origin + 1) = rot_abt[1];
							*p_theta += incrangle;
							free(pts_cpy);
							free(local_pts);
							return true;
						}
						//if variance less than min variance, assign x_max = x_nudge, y_max = y_nudge and min variance = variance
						if (variance < min_variance)
						{
							min_variance = variance;
							min_incrangle = incrangle;
							x_max = x_nudge;
							y_max = y_nudge;
						}
					}
					x_nudge -= 5*full_span_x/4;
				}
				full_span_x = full_span_x/2;
				x_nudge = x_max ;
				full_span_y = full_span_y/2;
				y_nudge = y_max ;
				l++;
			}
		}
//end of nudge logic

		*shift_x += x_max;
		*shift_y += y_max;
		*p_theta += min_incrangle;

		rot_abt[0] += x_max;
		rot_abt[1] += y_max;

		*rotn_origin = rot_abt[0];
		*(rotn_origin + 1) = rot_abt[1];

		free(pts_cpy);
		free(local_pts);
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0065", __FILE__, __FUNCSIG__);
		return false;
	}
}

double RBF::get_squared_dist(double* pt, Curve_2D curve)
{
	double endpt1[2] = {(curve.pts_list.front()).x,(curve.pts_list.front()).y};
	double endpt2[2] = {(curve.pts_list.back()).x, (curve.pts_list.back()).y};
	double dist = 0;
	double proj_pt[2];
	
	if (curve.curve == line)
	{
		if ((curve.answer[0] == M_PI_2) || (curve.answer[0] == 3 * M_PI_2))
			dist = pt[0] - curve.answer[1];
		else
			dist = (pt[1] - tan(curve.answer[0]) * pt[0] - curve.answer[1]) * cos(curve.answer[0]);
		proj_pt[0] = pt[0] + dist * sin(curve.answer[0]);
		proj_pt[1] = pt[1] - dist * cos(curve.answer[0]);
		if (((proj_pt[0] < endpt1[0]) && (proj_pt[0] < endpt2[0]))
			|| ((proj_pt[0] > endpt1[0]) && (proj_pt[0] > endpt2[0])) 
			|| ((proj_pt[1] > endpt1[1]) && (proj_pt[1] > endpt2[1]))
			|| ((proj_pt[1] < endpt1[1]) && (proj_pt[1] < endpt2[1])) )
		{
			double d1 = pow(pt[0] - endpt1[0],2) + pow(pt[1] - endpt1[1],2);
			double d2 = pow(pt[0] - endpt2[0],2) + pow(pt[1] - endpt2[1],2);
			if (d1 < d2)
				return d1;
			else
				return d2;
		}
		dist = dist * dist;
		return dist;
	}
	else
	{
		if (curve.pts_list.size() < 3) return -1;
		PtsList::iterator iter = curve.pts_list.begin();
		iter++;
		double midpt[2] = {iter->x, iter->y};
		dist = sqrt(pow(pt[0] - curve.answer[0],2) + pow(pt[1] - curve.answer[1],2)) - curve.answer[2];
		double startang = 0;
		if (endpt1[0] == curve.answer[0])
		{
			if (endpt1[1] > curve.answer[1])
				startang = M_PI_2;
			else
				startang = - M_PI_2;
		}
		else
		{
			startang = atan ((endpt1[1] - curve.answer[1])/(endpt1[0] - curve.answer[0]));
			if (endpt1[0] < curve.answer[0])
				startang += M_PI;
		}
		double endang = 0;
		if (endpt2[0] == curve.answer[0])
		{
			if (endpt2[1] > curve.answer[1])
				endang = M_PI_2;
			else
				endang = - M_PI_2;
		}
		else
		{
			endang = atan ((endpt2[1] - curve.answer[1])/(endpt2[0] - curve.answer[0]));
			if (endpt2[0] < curve.answer[0])
				endang += M_PI;
		}
		double midang = 0;
		if (midpt[0] == curve.answer[0])
		{
			if (midpt[1] > curve.answer[1])
				midang = M_PI_2;
			else
				midang = - M_PI_2;
		}
		else
		{
			midang = atan ((midpt[1] - curve.answer[1])/(midpt[0] - curve.answer[0]));
			if (midpt[0] < curve.answer[0])
				midang += M_PI;
		}
		if (endang > startang)
		{
			if (midang > endang)
			{
				startang += 2 * M_PI;
			}
			else if (midang < startang)
			{
				endang -= 2 * M_PI;
			}
		}
		else
		{
			if (midang > startang)
			{
				endang += 2 * M_PI;
			}
			else if (midang < endang)
			{
				startang -= 2 * M_PI;
			}
		}
		double ptang = 0;
		bool pt_not_in_arc = false;
		if (pt[0] == curve.answer[0])
		{
			if (pt[1] > curve.answer[1])
				ptang = M_PI_2;
			else
				ptang = - M_PI_2;
		}
		else
		{
			ptang = atan ((pt[1] - curve.answer[1])/(pt[0] - curve.answer[0]));
			if (pt[0] < curve.answer[0])
				ptang += M_PI;
		}
		ptang += 6 * M_PI;
		if (endang > startang)
		{
			ptang -= 2 * M_PI * (int) ((ptang - startang)/(2 * M_PI));
			if (ptang < startang)
				ptang += 2 * M_PI;
			if (ptang > endang)
				pt_not_in_arc = true;
		}
		else
		{
			ptang -= 2 * M_PI * (int) ((ptang - endang)/(2 * M_PI));
			if (ptang < endang)
				ptang += 2 * M_PI;
			if (ptang > startang)
				pt_not_in_arc = true;
		}
		if (pt_not_in_arc == true)
		{
			double d1 = pow(pt[0] - endpt1[0],2) + pow(pt[1] - endpt1[1],2);
			double d2 = pow(pt[0] - endpt2[0],2) + pow(pt[1] - endpt2[1],2);
			if (d1 < d2)
				return d1;
			else
				return d2;
		}
		dist = dist * dist;
		return dist;
	}
}

double RBF::get_min_squared_dist(double* pt, Curve_2DList curve_list)
{
	double squared_dist = 0;
	double min_squared_dist = -1;
	for (Curve_2DList::iterator iter = curve_list.begin(); iter != curve_list.end(); iter++)
	{
		squared_dist = get_squared_dist(pt, *iter);
		if (squared_dist == -1)
			continue;
		if ((min_squared_dist == -1) || (squared_dist < min_squared_dist))
			min_squared_dist = squared_dist;
	}
	return min_squared_dist;
}

double RBF::get_min_squared_dist(double* pt, double* pts_list, int ptscount)
{
	double squared_dist = 0;
	double min_squared_dist = -1;
	for (int i = 0; i < ptscount; i++)
	{
		squared_dist = pow(pt[0] - pts_list[3*i],2) + pow(pt[1] - pts_list[3*i + 1],2) + pow(pt[2] - pts_list[3*i + 2],2);
		if ((min_squared_dist == -1) || (squared_dist < min_squared_dist))
			min_squared_dist = squared_dist;
	}
	return min_squared_dist;
}
bool RBF::getRotnAngleForPtsMatch(double* pts, int ptscount, Curve_2DList curve_list, double* rot_abt, double* p_theta, double* p_variance, double full_span_angle, double resolution, int l_step, int ang_step)
{
	try
	{
		int l = 0; 
		double theta = 0;
		*p_theta = 0;
		double* local_pts = (double*) malloc(2*sizeof(double)*ptscount);
		double variance = 0;
		memset(local_pts,0,2*sizeof(double)*ptscount);
		while (l<l_step)
		{
			theta -= full_span_angle/ang_step;
			for (int k=0; k<ang_step;k++)
			{
				theta += full_span_angle/ang_step;
				//rotate points by angle theta around rot_abt
				for (int i = 0; i < ptscount; i++)
				{
					local_pts[2*i] = rot_abt[0] + (pts[2*i] - rot_abt[0]) * cos(theta) - (pts[2*i + 1] - rot_abt[1]) * sin(theta);
					local_pts[2*i + 1] = rot_abt[1] + (pts[2*i] - rot_abt[0]) * sin(theta) + (pts[2*i + 1] - rot_abt[1]) * cos(theta);
				}
				//get variance, ie sum of square of distances, of points from line-arc list
				variance = 0;
				for (int i = 0; i < ptscount; i++)
				{
					double d = get_min_squared_dist(local_pts + 2*i, curve_list);
					if (d == -1)
						continue;
					variance += d;
				}
				//if variance less than resolution, assign *p_theta = theta and return true
				if (variance < resolution)
				{
					*p_variance = variance;
					*p_theta = theta;
					free(local_pts);
					return true;
				}
				//if variance less than min variance, assign *p_theta = theta and min variance = variance
				if ((*p_variance == -1) || (variance < *p_variance))
				{
					*p_variance = variance;
					*p_theta = theta;
				}
			}
			full_span_angle = 2*full_span_angle/ang_step;
			theta = *p_theta - full_span_angle/2;
			l++;
		}
		free(local_pts);
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0066", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::BFalignment_3D_pts(double* pts_ref, int pts_ref_cnt, double* pts_obj, int pts_obj_cnt, double* shift_obj, double* p_theta, double* p_phi, double resolution, int sample_size)
{
	try
	{
		int step = 1;
		int ptscount = pts_ref_cnt;
		if (pts_obj_cnt < ptscount)
			ptscount = pts_obj_cnt;
		if (ptscount > sample_size)
			step = ptscount/sample_size;
		ptscount = ptscount/step;
		double* pts_ref_cpy = (double*) malloc(3*sizeof(double)*ptscount);
		memset(pts_ref_cpy,0,3*sizeof(double)*ptscount);
		double* pts_obj_cpy = (double*) malloc(3*sizeof(double)*ptscount);
		memset(pts_obj_cpy,0,3*sizeof(double)*ptscount);
		double* local_pts = (double*) malloc(3*sizeof(double)*ptscount);
		memset(local_pts,0,3*sizeof(double)*ptscount);

		//redefine resolution as variance (ie sum of squares of resolution)
		resolution = ptscount * resolution * resolution;
		double cg_ref[3] = {0};
		double cg_obj[3] = {0};
		for (int i = 0; i < ptscount; i++)
		{
			pts_ref_cpy[3*i] = pts_ref[3*i*step];
			pts_ref_cpy[3*i + 1] = pts_ref[3*i*step + 1];
			pts_ref_cpy[3*i + 2] = pts_ref[3*i*step + 2];
			pts_obj_cpy[3*i] = pts_obj[3*i*step];
			pts_obj_cpy[3*i + 1] = pts_obj[3*i*step + 1];
			pts_obj_cpy[3*i + 2] = pts_obj[3*i*step + 2];
		}
		for (int i = 0; i < ptscount; i++)
		{
			cg_ref[0] += pts_ref_cpy[3*i];
			cg_ref[1] += pts_ref_cpy[3*i + 1];
			cg_ref[2] += pts_ref_cpy[3*i + 2];

			cg_obj[0] += pts_obj_cpy[3*i];
			cg_obj[1] += pts_obj_cpy[3*i + 1];
			cg_obj[2] += pts_obj_cpy[3*i + 2];
		}

		for (int i = 0; i < 3; i++)
		{
			cg_ref[i] /= ptscount;
			cg_obj[i] /= ptscount;
		}

		for (int i = 0; i < 3; i++)
			shift_obj[i] = cg_ref[i] - cg_obj[i];

		for (int i = 0; i < ptscount; i++)
		{
			local_pts[3*i] = pts_ref_cpy[3*i] - cg_ref[0];
			local_pts[3*i + 1] = pts_ref_cpy[3*i + 1] - cg_ref[1];
			local_pts[3*i + 2] = pts_ref_cpy[3*i + 2] - cg_ref[2];
		}
		memcpy(pts_ref_cpy, local_pts, 3*sizeof(double)*ptscount);
		for (int i = 0; i < ptscount; i++)
		{
			local_pts[3*i] = pts_obj_cpy[3*i] - cg_obj[0];
			local_pts[3*i + 1] = pts_obj_cpy[3*i + 1] - cg_obj[1];
			local_pts[3*i + 2] = pts_obj_cpy[3*i + 2] - cg_obj[2];
		}
		memcpy(pts_obj_cpy, local_pts, 3*sizeof(double)*ptscount);	

		double full_span_theta = M_PI;
		double full_span_phi = 2*M_PI;
		int theta_step = 16;
		int phi_step = 16;
		int l_step = 5;
		int l = 0;
		double theta = 0;
		double phi = 0;
		double phi_start = 0;
		double variance = 0;
		double min_variance = -1;
		double x, y, z, x1, y1, z1, x2, y2, z2;
		while (l < l_step)
		{
			theta -= full_span_theta/theta_step;
			phi_start -= full_span_phi/phi_step;
			for (int k = 0; k < theta_step; k++)
			{
				theta += full_span_theta/theta_step;
				phi = phi_start;
				for (int m = 0; m < phi_step; m++)
				{
					phi += full_span_phi/phi_step;
					//rotate points by angle theta around y axis followed by phi around z axis
					for (int i = 0; i < ptscount; i++)
					{
						x = pts_obj_cpy[3*i];
						y = pts_obj_cpy[3*i + 1];
						z = pts_obj_cpy[3*i + 2];
						x1 = x * cos (theta) + z * sin (theta);
						y1 = y;
						z1 = -x * sin (theta) + z * cos (theta);
						x2 = x1 * cos (phi) - y1 * sin (phi);
						y2 = x1 * sin (phi) + y1 * cos (phi);
						z2 = z1;
						local_pts[3*i] = x2;
						local_pts[3*i + 1] = y2;
						local_pts[3*i + 2] = z2;
					}
					//get variance, ie sum of square of distances, of transformed points from pts_ref_cpy
					variance = 0;
					for (int i = 0; i < ptscount; i++)
					{
						double d = get_min_squared_dist(local_pts + 3*i, pts_ref_cpy, ptscount);
						if (d == -1)
							continue;
						variance += d;
					}
					//if variance less than resolution, assign *p_theta = theta, *p_phi = phi and return true
					if (variance < resolution)
					{
						*p_theta = theta;
						*p_phi = phi;
						free(local_pts);
						free(pts_ref_cpy);
						free(pts_obj_cpy);
						return true;
					}
					//if variance less than min variance, assign *p_theta = theta, *p_phi = phi
					if ((min_variance == -1) || (variance < min_variance))
					{
						min_variance = variance;
						*p_theta = theta;
						*p_phi = phi;
					}
				}
			}
			full_span_theta = 2*full_span_theta/theta_step;
			full_span_phi = 2*full_span_phi/phi_step;
			theta = *p_theta - full_span_theta/2;
			phi = *p_phi - full_span_phi/2;
			l++;
		}
		free(local_pts);
		free(pts_ref_cpy);
		free(pts_obj_cpy);
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0067", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::ArcTangentToLine(double* line, double* arc_in, double* arc_out)
{
	try
	{
		double x_c = arc_in[0];
		double y_c = arc_in[1];
		double r = arc_in[2];
		double x1 = arc_in[3];
		double y1 = arc_in[4];

		double x_L = line[0];
		double y_L = line[1];
		double line_ang = line[2];

		int sign = 1;
		
		double d = (x1 - x_L) * sin (line_ang) - (y1 - y_L) * cos (line_ang);
		if (d < 0) 
			sign = -1;

		double d2 = (x_c - x1) * sin (line_ang) - (y_c - y1) * cos (line_ang);

		if (d2/r == sign)
			return false;
		double r_new = - d/(d2/r - sign);
		double x_new = (x_c - x1)*r_new/r + x1;
		double y_new = (y_c - y1)*r_new/r + y1;

		arc_out[0] = x_new;
		arc_out[1] = y_new;
		arc_out[2] = r_new;

		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0068", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::ArcTangentToArc(double* arc_in_first, double* arc_in_second, double* arc_out_second)
{
	try
	{
		double x_c1 = arc_in_first[0];
		double y_c1 = arc_in_first[1];
		double r1 = arc_in_first[2];
		double x_1e2 = arc_in_first[3];
		double y_1e2 = arc_in_first[4];

		double x_c2 = arc_in_second[0];
		double y_c2 = arc_in_second[1];
		double r2 = arc_in_second[2];
		double x_2e2 = arc_in_second[3];
		double y_2e2 = arc_in_second[4];
		
		//double x_2e2_dash = (x_2e2 + x_1e2)/2;
		//double y_2e2_dash = (y_2e2 + y_1e2)/2;
		//double x_c2_dash = x_2e2_dash - (y_1e2 - y_2e2);
		//double y_c2_dash = y_2e2_dash + (x_1e2 - x_2e2);

		double num = (x_c2 - x_2e2) * (y_c1 - y_2e2) - (y_c2 - y_2e2) * (x_c1 - x_2e2);
		double den = (y_c2 - y_2e2) * (x_1e2 - x_c1) - (x_c2 - x_2e2) * (y_1e2 - y_c1);

		//double num = (x_c2_dash - x_2e2_dash) * (y_c1 - y_2e2_dash) - (y_c2_dash - y_2e2_dash) * (x_c1 - x_2e2_dash);
		//double den = (y_c2_dash - y_2e2_dash) * (x_1e2 - x_c1) - (x_c2 - x_2e2_dash) * (y_1e2 - y_c1);
		if (den == 0)
			return false;
		double t = num / den;
		double x_c2_new = x_1e2 * t + x_c1 * (1 - t);
		double y_c2_new = y_1e2 * t + y_c1 * (1 - t);
		double r2_new = hM->pythag(x_c2_new - x_1e2, y_c2_new - y_1e2);

		arc_out_second[0] = x_c2_new;
		arc_out_second[1] = y_c2_new;
		arc_out_second[2] = r2_new;
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0069", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::RetrieveDelphiCamPts(double ray_angle, double theta, double min_radius, double* pts)
{
	try
	{
		double r = 0;
		double xdash = 0;
		double ydash = 0;
		double norm = 0;
		if ((theta <= M_PI/6) || (theta >= 11*M_PI/6))
		{
			pts[0] = 0;
			pts[1] = 0;
			pts[2] = min_radius * cos (ray_angle + theta);
			pts[3] = min_radius * sin (ray_angle + theta);
		}
		else if ((theta >= 5*M_PI/6) && (theta <= 7*M_PI/6))
		{
			pts[0] = 0;
			pts[1] = 0;
			pts[2] = (min_radius + 4) * cos (ray_angle + theta);
			pts[3] = (min_radius + 4) * sin (ray_angle + theta);
		}
		else if (theta < 5*M_PI/6) //this is case when angle is between 30 and 150 degrees
		{
			r = min_radius + 2 - 2 * cos(1.5 * theta - M_PI/4);
			xdash = 3 * sin (1.5 * theta - M_PI/4) * cos (ray_angle + theta) - r * sin (ray_angle + theta);
			ydash = 3 * sin (1.5 * theta - M_PI/4) * sin (ray_angle + theta) + r * cos (ray_angle + theta);
			norm = sqrt(xdash*xdash + ydash*ydash);
			if (norm == 0)
				return false;
			//normalize the derivative vector - tangent vector is xdash, ydash and normal vector is -ydash, xdash
			xdash /= norm;
			ydash /= norm;
			//scale it by min_radius/2
			xdash *= min_radius/2;
			ydash *= min_radius/2;
			//probe point
			pts[2] = r * cos (ray_angle + theta);
			pts[3] = r * sin (ray_angle + theta);
			//probe position is shifted along normal to tangent vector at a distance of min_radius/2
			pts[0] = pts[2] - ydash;
			pts[1] = pts[3] + xdash;
		}
		else //this is case when angle is between 210 and 330 degrees
		{
			r = min_radius + 2 - 2 * sin(1.5 * theta - M_PI/4);
			xdash = -3 * cos (1.5 * theta - M_PI/4) * cos (ray_angle + theta) - r * sin (ray_angle + theta);
			ydash = -3 * cos (1.5 * theta - M_PI/4) * sin (ray_angle + theta) + r * cos (ray_angle + theta);
			norm = sqrt(xdash*xdash + ydash*ydash);
			if (norm == 0)
				return false;
			//normalize the derivative vector - tangent vector is xdash, ydash and normal vector is -ydash, xdash
			xdash /= norm;
			ydash /= norm;
			//scale it by min_radius/2
			xdash *= min_radius/2;
			ydash *= min_radius/2;
			//probe point
			pts[2] = r * cos (ray_angle + theta);
			pts[3] = r * sin (ray_angle + theta);
			//probe position is shifted along normal to tangent vector at a distance of min_radius/2
			pts[0] = pts[2] - ydash;
			pts[1] = pts[3] + xdash;
		}
		return true;
	}
	catch(...)
	{
		SetAndRaiseErrorMessage("RBF0070", __FILE__, __FUNCSIG__);
		return false;
	}
}

bool RBF::Sinusoid_BestFit(double* pts, int PtsCount, double* answer)
{
	//Calculate the max pt. Use this as the initial 
	//Sinusoid is Y0 + R*cos(theta - theta0)
	//So we need to get initial guess for Y0, theta0 and R
	//Get the max and min values. Take the average as the init guess for R
	//Max Y will be init Y0, and theta at maxY is theta0. 
	double maxY = -100000, minY = 100000;
	int maxIndex = 0;
	double* tempPts = (double*)malloc(sizeof(double)*PtsCount * 2);
	memcpy(tempPts, pts, PtsCount * 2 * sizeof(double));
	double initVal = pts[1];
	//Get the maxY and its index in the array
	for (int i = 0; i < PtsCount * 2; i += 2)
	{	
		tempPts[i + 1] -= initVal;
		if (tempPts[i + 1] > maxY)
		{
			maxY = tempPts[i + 1];
			maxIndex = i;
		}
		if (tempPts[i + 1] < minY)
			minY = tempPts[i + 1];
		
	}
	double initGuess[3] = {0};
	initGuess[0] = maxY; 
	initGuess[1] = pts[maxIndex];
	initGuess[2] = (maxY + minY)/2;
	//double sinusoid[3] = {0};
	//bool Succeeded = hM->LS_BF(pts, initGuess, PtsCount, 1, 3, -1, hM->Sinusoid, Tolerance, answer);
	bool Succeeded = hM->LS_BF_LM(tempPts, initGuess, PtsCount, 1, 3, -1, hM->Sinusoid, false, 10000, Tolerance, answer);

	//Now check if any point is more than some tolerance from the runout. Remove those points and then redo the best-fit. 
	//double runoutVal = 0;
	//double NoiseFilterTolerance = 0.005;
	//int ct = 0;
	//for (int i = 0; i < PtsCount * 2; i += 2)
	//{
	//	runoutVal = answer[0] + answer[2] * cos(pts[i] - answer[1]);
	//	if (abs(pts[i + 1] - initVal - runoutVal) > NoiseFilterTolerance)
	//		continue;
	//	tempPts[2 * ct] = pts[i]; tempPts[2 * ct + 1] = pts[i + 1] - initVal;
	//	ct ++;
	//}
	//if (ct > 0.7 * PtsCount) //Atleast 70% of the points should be good quality.
	//{
	//	Succeeded = hM->LS_BF_LM(tempPts, initGuess, ct, 1, 3, -1, hM->Sinusoid, false, 10000, Tolerance, answer);
	//}
	return Succeeded;
}

