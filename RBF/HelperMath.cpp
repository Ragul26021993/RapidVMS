
#include "StdAfx.h"
#include <malloc.h>
#include "HelperMath.h"


#define _USE_MATH_DEFINES
#include <math.h>

double getTangentAng(double s, int iter = 100)
{
	double phi = s;
	double phi_revised = s;
	for (int i = 0; i < iter; i++)
	{
		phi_revised = atan (phi) + s;
		if (abs(phi_revised - phi) < 0.0001 * phi)
			break;
		phi = phi_revised;
	}
	return phi_revised;
}

HelperMath::HelperMath(void)
{	CurrentParameterNumber = 3; CurrentPointsCt = 100;
	param = NULL;
	jacobian = NULL;
	residual = NULL;
	jac_inverse = NULL;
	jac_residual = NULL;
	jac_temp = NULL;
	deltaParam = NULL;
	Allocate_LSBF_Memory();
}

HelperMath::~HelperMath(void)
{	free((void*) param); free((void*) jacobian); free((void*) residual); free((void*) jac_temp); 
	free((void*) jac_inverse); free((void*) jac_residual); free((void*) deltaParam);
}

//=======================================================
// ============ " Helper functions " ====================

bool HelperMath::ShiftaroundCentroid_3D(double* pts, int PtsCount, double* Centroid)
{	//First get the centroid of all the points. This will be our first guess of a pt on the axis
	for (int i = 0; i < 3 * PtsCount; i += 3)
	{	//Get the sum of all the X, Y and Z respectively
		for (int j = 0; j < 3; j ++)
		{	*(Centroid + j) += *(pts + i + j);
		}
	}
	
	//Calculate the centroid and put into initGuess
	for (int j = 0; j < 3; j ++)
	{	*(Centroid + j) /= PtsCount;
	}
	
	//Now shift the origin of the pts to the centroid
	for (int i = 0; i < 3 * PtsCount; i += 3)
	{	for (int j = 0; j < 3; j ++) *(pts + i + j) -= *(Centroid + j);
	}
	return true;
}


bool HelperMath::ShiftaroundCentroid_2D(double* pts, int PtsCount, double* Centroid)
{	//First get the centroid of all the points. This will be our first guess of a pt on the axis
	for (int i = 0; i < 2 * PtsCount; i += 2)
	{	//Get the sum of all the X, Y and Z respectively
		for (int j = 0; j < 2; j ++)
		{	*(Centroid + j) += *(pts + i + j);
		}
	}
	
	//Calculate the centroid and put into initGuess
	for (int j = 0; j < 2; j ++)
	{	*(Centroid + j) /= PtsCount;
	}
	
	//Now shift the origin of the pts to the centroid
	for (int i = 0; i < 2 * PtsCount; i += 2)
	{	for (int j = 0; j < 2; j ++) *(pts + i + j) -= *(Centroid + j);
	}
	return true;
}

// --------------- " Point Line Distance 2D " 
double HelperMath::Point_Line_Dist_2D(double* p, double* L)
{	if (*L == 0.5 * M_PI)
		{return (double)abs(*p - *(L + 1));
		}
	else
		{return (double) abs((*(p + 1) - tan(*L) * *(p) - *(L + 1)) * cos(*L));
		}
}

// --------------- " Point Line Distance 3D " 
double HelperMath::Point_Line_Dist_3D(double* p, double* L)
{	double dist = 0, tD = 0;
	for (int i = 0; i < 3; i ++) dist += pow(*(p + i) - *(L + i), 2);
	for (int i = 0; i < 3; i ++) tD += *(L + 3 + i) *(*(p + i) - *(L + i));
	return sqrt(dist - tD * tD);
}

// --------------- " 2D Line Line Intersection Pt " 
int HelperMath::Line_lineintersection_2D( double slope1,double intercept1, double slope2, double intercept2, double *intersectnpt )
{ 
	try
	{
		Lslope(&slope1);
		Lslope(&slope2);
		//if slopes are equal then the lines are parallel//
		if(slope1 == slope2)
		{
			return 0;  
		}
		else if(slope1 == 0.5 * M_PI)  //If one line is vertical then the intersection point of the lines//
		{
			*intersectnpt = intercept1;
			*(intersectnpt + 1) = (tan(slope2)) * (*intersectnpt) + intercept2;
		}

		else if(slope2 == 0.5*M_PI) //If other line is vertical then the intersection point of the lines//
		{
			*intersectnpt = intercept2;
			*(intersectnpt + 1) = (tan(slope1)) * (*intersectnpt) + intercept1;
		}
		else
		{
			//Intersection point of the lines//
			*intersectnpt = (intercept1 - intercept2) / (tan(slope2) - tan(slope1));   
			*(intersectnpt + 1) = (tan(slope1)) * (*intersectnpt) + intercept1;
		}
		//OutputDebugString(L"Line_lineintersection is verified\n");
		return 1;
	}
	catch(...)
	{
		return 0;
	}
}

// ------------- A simple function to ensure all line slopes are in the region of 0 to pi
bool HelperMath::Lslope( double *slope )
{
	try
	{
		if(*slope < 0)
		{
			*slope += M_PI;
		}
		if(*slope > M_PI)
		{
			*slope -= M_PI;
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}

//---------------- " Transpose of a given matrix "
void HelperMath::TransposeMatrix(double* matrix, double* Transpose, int Rows, int Columns)
{	for (int i = 0; i < Rows; i ++)
	{	for (int j = 0; j < Columns; j ++)
		{	*(Transpose + j * 3 + i) = *(matrix + i * 3 + j);
		}
	}
}


// --------------- " Matrix multiplier " 
bool HelperMath::MultiplyMatrix(double *M1, int *S1, double *M2, int *S2, double *answer)
{
	int i, j , k;
	try
	{

		//For multiplication the no. of columns of the first matrix should be equal to no. of rows of the second matrix//
		if (*(S1 + 1) != *(S2)) 
		{	return false; 
		}
		//Sum of product of row elements of first matrix with  the column elements of second//
		for (i = 0; i < *(S1); i ++)
		{	
			for (j = 0; j < *(S2 + 1); j ++)
			{	//Initialing the answer to zero after calculation of each element of matrix//
				*(answer + *(S2 + 1)*i + j) = 0;
				for (k = 0; k < *(S2); k ++)
				{	
					//answer holds the result of matrix multiplication(sum of the products)//
					*(answer + *(S2 + 1)*i + j) += ((*(M1 + *(S1 + 1)*i + k)) * (*(M2 + *(S2 + 1) *k + j)));
				}
			}	
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}

void HelperMath::MultiplyMatrix(double Multiplier, double *Matrix, int *S1)
{	for (int i = 0; i < S1[0] * S1[1]; i ++) Matrix[i] *= Multiplier;
}

// --------------- " Determinant amd inverse finder for square matrix "
// --------------- Set flag to 1 to calculate inverse else, function terminates after calculating determinant
double HelperMath::OperateMatrix(double* matrix, int Size, int Flag, double* Answer)
{	
	//RowEchelon will be the local matrix on which row reduction will be done to get
	//Upper Diagonal matrix
	double* RowEchelon =(double*) malloc(sizeof(double) * Size * Size);
	//Inverse will be the inverse of the matrix, if required; 
	double* Inverse =(double*) malloc(sizeof(double) * Size * Size);
	int i = 0, j = 0; 
	int FlipCount = 0; //Keeps a count of the number of pivot steps that have been done 
	double max; int pivotRow = 0, currentRow; 
	double Determinant, Eliminator;
	//Copy all the elements of Matrix to RowEchelon
	memcpy((void*)RowEchelon, (void*)matrix , sizeof(double) * Size * Size);
	try
	{
		//For computing the inverse, initialise the inverse as the unit matrix
		for (j = 0; j < Size; j++)	
		{	for (i = 0; i < Size; i ++)
			{	if (i == j) *(Inverse + j * Size + i) = 1; else *(Inverse + j * Size + i) = 0;
			}
		}


		//Loop through all the columns
		for (currentRow = 0; currentRow < Size - 1; currentRow ++)
		{	max = abs(*(RowEchelon + currentRow * (Size + 1)));	pivotRow = 0;
			//Find out the largest member of the current column
			for (i = currentRow + 1; i < Size; i ++) 
			{	if (abs(*(RowEchelon + i * Size + currentRow)) > max) {max = abs(*(RowEchelon + i * Size + currentRow)); pivotRow = i;}
			}
			//If all the entries below the current row are 0, then we determinant 0 and non-invertible matrix!
			if (max == 0) continue;
			
			//Now flip currentRow and pivotRow, so that we can get an upper triangular matrix without awkward 0 in the diagonal
			if (pivotRow > currentRow) 
			{	FlipCount += 1;
				double Temp = 0.0;
				for (i = 0; i < Size; i ++)
				{	//Do it for the RowEchelon Matrix
					Temp = *(RowEchelon + currentRow * Size + i);
					*(RowEchelon + currentRow * Size + i) = *(RowEchelon + pivotRow * Size + i);
					*(RowEchelon + pivotRow * Size + i) = Temp;
					//Repeat for the Inverse Matrix
					Temp = *(Inverse + currentRow * Size + i);
					*(Inverse + currentRow * Size + i) = *(Inverse + pivotRow * Size + i);
					*(Inverse + pivotRow * Size + i) = Temp;
				}
			}
			//Eliminate to zeroes all the fellows below the currentrow in the current column
			//if (currentRow == 0) continue; 
			for (i = currentRow + 1; i < Size; i ++)
			{	Eliminator = *(RowEchelon + i * Size + currentRow) / *(RowEchelon + currentRow * (Size + 1));
				for (j = 0; j < Size; j ++)
				{	*(RowEchelon + i * Size + j) -= *(RowEchelon + currentRow * Size + j) * Eliminator;
					*(Inverse + i * Size + j) -= *(Inverse + currentRow * Size + j) * Eliminator;
				}
			}
		}
		//Finished with an upper diagonal matrix. 
		//Calculate the Determinant
		Determinant = 1;
		for (i = 0; i < Size; i ++)
		{	Determinant *= *(RowEchelon + i * (Size + 1));
		}
		Determinant *= pow(-1.0, FlipCount);
		//If inverse not required, and only determinant is required, 
		//fill answer with row echelon for linear solver, and return determinant
		//if (Flag == 0) //Require only determinant optional filling of upper diag matrix
		//{	
		//	//memcpy((void*)Answer, (void*)&Determinant, sizeof(double) * Size * Size);
		//}
		if (Flag == 1) //Require Inverse
		{	for (currentRow = 0; currentRow < Size; currentRow ++)
			{	//Process until RowEchelon becomes identity matrix
				//Make the diagonal element one, by dividing the row by the diagonal element of that row! 
				Eliminator = *(RowEchelon + currentRow * (Size + 1));
				for (j = 0; j < Size; j ++)
				{	*(RowEchelon + currentRow * Size + j) /= Eliminator;
					*(Inverse + currentRow * Size + j) /= Eliminator;
				}
			}
			for (currentRow = 0; currentRow < Size; currentRow ++)
			{	//Eliminate column-wise all elements until we get zeroes simply subtract 
				// To do this, for column no. i, get row number i (which will be all zero until this column!) and eliminate!
				for (i = currentRow + 1; i < Size; i ++)
				{	Eliminator = *(RowEchelon + currentRow * Size + i);	
					for (j = 0; j < Size; j ++)
					{	*(RowEchelon + currentRow * Size + j) -= *(RowEchelon + i * Size + j) * Eliminator;
						*(Inverse + currentRow * Size + j) -= *(Inverse + i * Size + j) * Eliminator;
					}
				}
			}
			memcpy((void*)Answer, (void*)Inverse, sizeof(double) * Size * Size);
		}
		free(RowEchelon); free(Inverse);
		return Determinant;
	}
	catch(...)
	{
		/*Rerrorcode = "RMATH0029";
		RerrorMessage = __FUNCDNAME__;
		RerrorStack1 = __FILE__;
		RerrorStack2 = __FUNCSIG__;
		RerrorStack3 = __TIMESTAMP__;
		RMathError();*/
		return false;
	}
}

//Calculate the pythag function for calculating the SVD
double HelperMath::pythag(double a, double b, double c)
{	//Computes sqrt(a^2 + b^2 + c^2) without overflow or underflow!
	double absA, absB, absC;
	absA = fabs(a); absB = fabs(b); absC = fabs(c);
	double maxabs = absC;
	if (absB > maxabs) maxabs = absB;
	if (absA > maxabs) maxabs = absA;
	if (maxabs > 0) return maxabs * sqrt ( pow (absA / maxabs, 2) + pow (absB / maxabs, 2) + pow (absC / maxabs, 2) );
	else return 0;
}

//-----------------  Calculate the Singular Value decomposition of a given matrix of size m x n
void HelperMath::SVD(double* A, int m, int n, double* diag, double *V)
{	//A is a linear form of matrix m X n with arrangement like a11, a12, a13.... a21,a22,...aMN.
	bool flag;
	int i, its, j, jj, k, L, nm;
	double anorm, c, f, g, h, s, scale, x, y, z;

	double* rv1 = (double*) malloc(sizeof(double) * n);
	g = scale = anorm = 0.0;		

	//	Householder reduction to bidiagonal form.
	for (i = 0; i < n; i++) 
	{	L= i + 2;
		*(rv1 + i) = scale * g;
		g = s = scale = 0.0;
		if (i < m) 
		{	for (k = i; k < m; k ++) scale += fabs(*(A + k * n + i));
			if (scale != 0.0) 
			{	for (k = i; k < m; k ++) 
				{	*(A + k * n + i) /= scale;
					s += *(A + k * n + i) * *(A + k * n + i);
				}
				f = *(A + i * n + i);
				g = -SIGN(sqrt(s) , f);
				h = f * g - s;
				*(A + i * n + i) = f - g;
				for (j = L- 1; j < n; j ++) 
				{	 for (s = 0.0, k = i; k < m; k ++) s += *(A + k * n + i) * *(A + k * n + j);
					f = s / h;
					for (k = i; k < m; k ++) *(A + k * n + j) += f * *(A + k * n + i);
				}
				for (k = i; k < m; k++) *(A + k * n + i) *=scale;
			}
		}
		*(diag + i) = scale * g;
		g = s = scale = 0.0;
		if (i + 1 <= m && i != n) 
		{	for( k = L - 1; k < n; k ++) scale += fabs (*(A + i * n + k));
			if( scale != 0.0) 
			{	for (k = L - 1; k < n; k ++) 
				{	*(A + i * n + k) /= scale;
					s += *(A + i * n + k) * *(A + i * n + k);
				}
				f = *(A + i * n + L- 1);
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				*(A +i * n + L- 1) = f - g;
				for (k = L - 1; k < n; k ++) *(rv1 + k) = *(A + i * n + k)/h;
				for (j = L - 1; j < m; j ++) 
				{	for (s = 0.0, k = L - 1; k < n; k ++) s += *(A +j * n + k) * *(A + i * n + k);
					for (k = L - 1; k < n; k ++) *(A + j * n + k) += s * *(rv1 + k);
				}
				for (k = L - 1; k < n; k ++) *(A + i * n + k) *= scale;
			}
		}
		anorm = max(anorm, (fabs (*(diag + i)) + fabs(*(rv1 + i))));
	}
	//Accumulation of right-hand transformations.
	for (i = n - 1; i >= 0; i --) 
	{	if (i < n - 1) 
		{	if (g != 0.0) 
			{	//Double division to avoid possible underflow.	
				for (j = L; j < n; j ++) *(V + j * n + i) = (*(A + i * n + j)/ *(A + i * n + L)) / g;
				for(j = L; j < n; j++)
				{	for(s = 0.0, k = L; k < n; k ++) s += *(A + i * n + k) * *(V + k * n + j);
					for (k = L; k < n; k ++) *(V + k * n + j) += s * *(V + k * n + i);
				}
			}
			for(j = L; j < n; j ++) *(V + i * n + j) = *(V + j * n + i) = 0.0;
		}
		*(V + i * n + i) = 1.0;
		g = *(rv1 + i);
		L = i;
	}
	//Accumulation of left hand transformations.
	for(i = min(m, n) - 1; i >= 0; i --)		
	{	L = i + 1;
		g = *(diag + i);
		for(j = L; j < n; j ++) *(A + i * n + j) = 0.0;
		if(g != 0.0)
		{	g = 1.0 / g;
			for(j = L; j < n; j ++)
			{	for(s = 0.0, k = L; k < m; k ++) s += *(A + k * n + i) * *(A + k * n + j);
				f = (s / *(A + i * n + i)) * g;
				for(k = i; k < m; k ++) *(A + k * n + j) = f * *(A + k * n + i);
			} 
			for(j = i; j < 
				m; j ++) *(A + j * n + i) *= g;
		}
		else 
		{	for (j = i; j < m; j ++)  *(A + j * n + i) = 0.0;
		}		
		++ *(A + i * n + i);
	}

	//	Diagonalization of the bidiagonal form :Loop over
	for(k = n - 1; k >= 0; k --)	  	 
	{	for(its = 0; its < 30; its ++)		//singular values, and over allowed iterations.
		{	flag = true;
			for(L = k; L >= 0; L --)		//Test for splitting.
			{	nm = L - 1;		//Note that rv1[0] is always zero.
				if(fabs(*(rv1 + L)) + anorm == anorm)
				{	flag = false; break;
				}
				if(fabs(*(diag + nm)) + anorm == anorm) break;
			}
			if (flag) 
			{	c = 0.0;  //Cancellation of rv1[1], if 1>0
				s = 1.0;
				for (i = L - 1; i < k + 1; i ++)
				{	f = s - *(rv1 + i);
					if (fabs(f) + anorm == anorm) break;
					g = *(diag + i);
					h = pythag(f, g);
					*(diag + i) = h;
					h = 1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 0; j < m; j ++)
					{	y=*(A + j * n + nm);
						z=*(A + j  * n + i);
						*(A + j * n + nm) = y - c + z - s;
						*(A + j * n + i)= z * c - y * s;
					}
				}
			}
			z = *(diag + k);
			//	Convergence.		
			if (L == k)
			{//	Singular value is made nonnegative.	
				if (z < 0.0)
				{	*(diag + k) = -z;
					for(j=0;j<n;j++) *(V + j * n + k) = -*(V + j * n + k);
				}
				break;
			}
//			if (its == 29)
				//	Nrerror(“no convergence in 30 svdcmp iterations”);
			x = *(diag + L);  //shift from bottom 2-by-2 minor.
			nm = k - 1;
			y = *(diag + nm);
			g = *(rv1 + nm);
			h = *(rv1 + k);
			f = ((y - z) * (y + z) + (g - h) * (g + h))/(2.0 * h * y);
			g = pythag(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g,f))) - h)) / x;
			c = s = 1.0;  // next QR transformation:
			for (j = L; j <= nm; j ++)
			{	i = j +1 ;
				g = *(rv1 + i);
				y = *(diag + i);
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				*(rv1 + j) = z;
				c = f/z;
				s = h/z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 0; jj < n; jj++)
				{	x = *(V + jj * n + j);
					z = *(V + jj * n + i);
					*(V + jj * n + j) = x * c + z * s;
					*(V + jj * n + i) = z * c - x * s;
				}
				z = pythag(f, h);
				*(diag + j) = z;    // Rotation can be arbitrary if z=0.
				if (z)
				{	z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				for(jj = 0; jj < m; jj++)
				{	y=*(A + jj * n + j);
					z=*(A + jj * n + i);
					*(A + jj * n + j) = y * c + z * s;
					*(A + jj * n + i) = z * c - y * s;
				}
			}
			*(rv1 + L) = 0.0;
			*(rv1 + k) = f;
			*(diag + k) = x;
		}
	}
	free(rv1);
}

//Balance a non-symmetric square matrix for reducing to Hessenberg form and for Eigenproblem solutions
void HelperMath::BalanceMatrix(double* M, int n)
{	const double RADIX = 2;
	int i,j, last = 0;
	double s,r,g,f,c, sqrdx;
	sqrdx = RADIX * RADIX;
	while (last  == 0)
	{	last = 1;
		for (i = 0; i < n; i ++)
		{	r = c = 0.0;
			for (j = 0; j < n; j ++)
				if (j != i)	
				{	c += fabs(M[j * n + i]);
					r += fabs(M[i * n + j]);
				}
			if (c != 0.0 && r != 0.0)
			{	g = r / RADIX;
				f = 1.0;
				s = c + r;
				while (c < g)
				{	f *= RADIX;
					c *= sqrdx;
				}
				g = r * RADIX;
				while (c > g)
				{	f /= RADIX;
					c /= sqrdx;
				}
				if ((c + r) / f < 0.95 * s)
				{	last = 0;
					g = 1.0 / f;
					for (j = 0; j < n; j ++) M[i * n + j] *= g;
					for (j = 0; j < n; j ++) M[j * n + i] *= f;
				}
			}
		}
	}
}

void HelperMath::Swap(double a, double b)
{	double temp = b; b = a; a = temp;
}

void HelperMath::Hessenberg_Transform(double* M, int n)
{	int i, j, m;
	double x, y;
	for (m = 1; m < n - 1; m ++)
	{	x = 0.0;
		i = m;
		//Find the pivot
		for (j = m; j < n; j ++)
		{	if (fabs(M[j * i + m - 1]) > fabs(x))
			{	x = M[j * i + m - 1]; i = j;
			}
		}
		//INterchange rows and columns
		if (i != m)
		{	for (j = m - 1; j < n; j ++) Swap(M[i * n + j], M[m * n + j]);
			for (j = 0; j < n; j ++) Swap(M[j * n + i], M[j * n + m]);
		}
		//Carry out the elimination
		if (x != 0.0)
		{	for (i = m + 1; i < n; i ++)
			{	if ((y = M[i * n + m -1]) != 0.0)
				{	y /= x; M[i * n + m - 1] = y;
					for (j = m; j < n; j ++) M[i * n + j] -= y * M[m * n + j];
					for (j = 0; j < n; j ++) M[j * n + m] += y * M[j * n + i];
				}
			}
		}
	}
}

void HelperMath::H_QR_EigenValues(double* M, int n, double* eigenValues)
{	int nn, m, l, k, j, its, i, mmin;
	double z, y, x, w , v, u, t, s , r, q, p, anorm;
	anorm = 0.0;
	//Compute the matrix norm for possible use in locating single small subdiagonal element
	for (i = 0; i < n; i ++)
	{	for (j = max(i - 1, 0); j < n; j ++)
			anorm += fabs(M[i * n + j]);
	}
	nn = n -1;
	t = 0.0;
	while (nn >= 0) // Gets changed only by an exceptional shift. Begin search for next eigen value
	{	its = 0;
		do
		{	//Begin iteration; Look for small subdiagonal element
			for (l = nn; l > 0; l --)
			{	s = fabs(M[(l - 1) * (n + 1)] + M[l * (n + 1)]);
				if (s == 0.0) s = anorm;
				if (fabs(M[l * n + l - 1]) + s == s)
				{	M[l * n + l - 1] = 0.0; break;
				}
			}
			x =  M[nn * (n + 1)];
			if (l = nn)
			{	// One root found
				eigenValues[nn--] = x + t;
			} else
			{	y = M[(nn - 1) * (n + 1)];
				w = M[nn * n + nn - 1] * M[(nn - 1) * n + nn];
				if (l == nn - 1)
				{	// two roots found
					p = 0.5 * (y - x);
					q = p * p + w;
					z = sqrt(fabs(q));
					x += t;
					if (q >= 0.0)
					{	// A real pair
						z = p + SIGN(z, p);
						eigenValues[nn - 1] = eigenValues[nn] = x + z;
						if (z != 0.0) eigenValues[nn] = x - w / z;
					} else
					{	// A complex pair
						//eigenValues[nn] = 
						//
					}
				} else
				{	//no roots found. Continue iteration
					//if (its = 30) ;
					if (its == 10 || its == 20)
					{	// Form exceptional shift
						t += x;
						for (i = 0; i < nn + 1; i ++) M[i * (n + 1)] -= x;
						s = fabs(M[nn * n + nn - 1]) + fabs(M[(nn - 1) * n + nn - 2]);
						y = x = 0.75 * s;
						w = -0.4375 * s * s;
					}
					++its;
					for (m = nn - 2; m >= 1; m--)
					{	// Form shift and then look for 2 consecutive small subdiagonal elements
						z = M[m * (n + 1)];
						r = x - z;
						s = y - z;
						p = (r * s - w)/M[(m + 1) * n + m] + M[m * n + m + 1];
						q = M[(m + 1) * (n + 1)] - z - r - s;
						r = M[(m + 2) * n + m + 1];
						s = fabs(p) + fabs(q) + fabs(r);// scale to prevent overflow or underflow
						p /= s; q /= s; r /= s;
						if (m == 1) break;
						u = fabs(M[ m * n + m -1]) * (fabs(q) + fabs(r));
						v = fabs(p) * (fabs(M[(m - 1)* (n + 1)]) + fabs(z) + fabs(M[(m + 1) * (n + 1)]));
						if (u + v == v) break;
					}
					for (i = m; i < nn - 1; i ++)
					{	M[(i + 2) * n + i] = 0.0; 
						if (i != m) M[(i + 2) * n  + i - 1] = 0.0;
					}
					for (k = m; k < nn; k ++)
					{	// Double QR step on rows 1 to nn and columns m to nn
						if (k != m)
						{	//Begin steup of Householder vector
							p = M[k * n + k -1];
							q = M[(k + 1) * n + k - 1];
							r = 0.0;
							if (k + 1 != nn) r = M[(k + 2) * n + k - 1];
							if ((x = fabs(p) + fabs(q) + fabs(r)) != 0.0)
							{	// Scale to prevent underflow or overflow
								p /= x; q /= x; r /= x;
							}
						}
						if (s = SIGN(sqrt(p * p + q * q + r * r), p) != 0.0)
						{	if (k == m)
							{	if (l != m) M[k * n + k - 1] = -M[k * n + k - 1];
							} else
							{	M[k * n + k - 1] = -s * x;
							}
							p += s; x = p / s; y = q / s; z = r / s;
							q /= p; r /= p;
							for ( j = k; j < nn + 1; j ++)
							{	// Row modification
								p = M[k * n + j] + q * M[(k + 1) * n + j];
								if (k + 1 != nn)
								{	p += r * M[(k + 2) * n + j];
									M[(k + 2) * n + j] -= p * z;
								}
								M[(k + 1) * n + j] -= p * y;
								M[k * n + j] -= p * x;
							}
							mmin = nn < k + 3 ? nn : k + 3;
							for (i = l; i < mmin + 1; i ++)
							{	//Column Modification
								p = x * M[i * n + k] + y * M[i * n + k + 1];
								if (k != (nn))
								{	p += z * M[i * n + k + 2];
									 M[i * n + k + 2] -= p * r;
								}
								 M[i * n + k + 1] -= p * q;
								  M[i * n + k] -= p;
							}
						}
					}
				}
			}
		} while (l + 1 < nn);
	}
}

//=====================================================================================================
//-------------------  " The Least Squares non-linear Function Fitter " -------------------------------
bool HelperMath::Get_Jac(Func FunctionName, double* Values, double* Param, int TotalPoints, int NofParams, double* jacobian, double* residual)
{
	try
	{
		double z, u, E, Q, R, Res, d;
		double g, modA;
		double a,b,c;
		int jj, kk;
		double dX[6];
		double f, v, w, dist;
		double x, y, pt_dot_axis;
		double ang, s;
		double radius;
		double dist_diretrix = 0;
		double sq_dist_diretrix = 0;
		double dist_focus = 0;
		double sq_dist_focus = 0;
		double temp;
		double da_dxf, db_dxf, dc_dxf, da_dyf, db_dyf, dc_dyf, da_dzf, db_dzf, dc_dzf, ddd_dxf, ddd_dyf, ddd_dzf, ddd_dxd, ddd_dyd, ddd_dzd;
		double rad_vec1[2] = {0};
		double rad_vec2[2] = {0};
		double norm = 0;
		bool retry = true;
		switch(FunctionName)
		{	case Focus:
				for (int i = 0; i < TotalPoints; i ++)
				{	//F() = B + A*Exp(-0.5((x-u)/sigma) ^2)
					//Param(0) = A; Param(1) = u; Param(2) = sigma; param(3) = B
					double x = 0;
					x = (Values[2 * i] - Param[1]) / Param[2];
					jacobian[NofParams * i + 0] = 1 / exp(0.5 * x * x);
					jacobian[NofParams * i + 1] = (Param[0] * x *  jacobian[NofParams * i + 0]) / Param[2];
					jacobian[NofParams * i + 2] = x *  jacobian[NofParams * i + 1];
					jacobian[NofParams * i + 3] = 1;
					residual[i] = Values[2 * i + 1] - (Param[3] + Param[0] * jacobian[NofParams * i + 0]);
				}
				break;
			case Circle_LMA:
				//A = param(0) //Theta = Param(1)  //D = param(2)
				E = sqrt(abs(1 + 4 * *(Param + 0) * *(Param + 2)));
				for (int i = 0; i < TotalPoints; i ++)
				{   u = *(Values + 2 * i) * cos(*(Param + 1)) + *(Values + 2 * i + 1) * sin(*(Param + 1));
					z = pow(*(Values + 2 * i), 2) + pow(*(Values + 2 * i + 1), 2);
					Res = *(Param + 0) * z + E * u + *(Param + 2);
					Q = sqrt(1 + 4 * *(Param + 0) * Res);
					d = 2 * Res / (1 + Q);
					R = 2 * (1 - *(Param + 0) * d) / (Q + 1);
					*(jacobian + NofParams * i + 0) = (z + 2 * *(Param + 2) * u / E) * R - d * d / Q;
					*(jacobian + NofParams * i + 1) = (-*(Values + 2 * i) * sin(*(Param + 1)) + *(Values + 2 * i + 1) * cos(*(Param + 1))) * E * R;
					*(jacobian + NofParams * i + 2) = R * (2 * *(Param + 0) * u / E + 1);
					*(residual + i) = -d;
				}
				break;
			case Cylinder:
				//	The parameter array consists of 7 numbers - 
				//  3 denoting a pt on the axis of the cylinder; 3 direction numbers of the axis line (this completely
				//  defines the axis line) & one radius. 
				//  the index is 0-2 denotes the coordinates of the pts, 3-5 denotes the direction numbers and 6 denotes radius
				//  now the dist of any pt on the cylinder from the axis line 
				//  The jacobian of the distance measure is now calculated 
			
				//The perpendicular distance of a pt from the axis line should be equal to radius.
				// hence the definition of the function to be minimised is calculated from that 
					//First normalise the direction numbers to get dir. cosines. 
				modA = sqrt(pow(*(Param + 3), 2) + pow(*(Param + 4), 2) + pow(*(Param + 5), 2));
				a = *(Param + 3) / modA;
				b = *(Param + 4) / modA; 
				c = *(Param + 5) / modA;
				*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;

							//bring x,y,z to point closest to origin.
				x = *Param;
				y = *(Param + 1);
				z = *(Param + 2);
				pt_dot_axis = x*a + y*b + z*c;

				x -= pt_dot_axis * a;
				y -= pt_dot_axis * b;
				z -= pt_dot_axis * c;

				*Param = x;
				*(Param + 1) = y;
				*(Param + 2) = z;

				//normalize radius 
				s = *(Param + 6);
				if (s < 0)
				{
					a = -a; b = -b; c = -c;
					*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;
					s = -s;
				}
				*(Param + 6) = s;
				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	g = 0;
					dX[0] = *(Values + i + 0) - *(Param + 0);
					dX[1] = *(Values + i + 1) - *(Param + 1);
					dX[2] = *(Values + i + 2) - *(Param + 2);
				
					g = a * dX[0] + b * dX[1] + c * dX[2];
				
					*(jacobian + jj) = 2 * (dX[0] - g * a); jj++;
					*(jacobian + jj) = 2 * (dX[1] - g * b); jj++;
					*(jacobian + jj) = 2 * (dX[2] - g * c); jj++;
					*(jacobian + jj) = 2 * dX[0] * g; jj++;
					*(jacobian + jj) = 2 * dX[1] * g; jj++;
					*(jacobian + jj) = 2 * dX[2] * g; jj++;
					*(jacobian + jj) = 2 * *(Param + 6); jj++;
					//The distance error between the guessed axis line and the given pt
					*(residual + kk) =	dX[0] * dX[0] + dX[1] *dX[1] + dX[2] * dX[2] - g * g - *(Param + 6) * *(Param + 6); kk++;
				}
				break;
			case Cone:
				//	The parameter array consists of 8 numbers - 
				//  3 denoting a pt on the axis of the cone; 3 direction numbers of the axis line (this completely
				//  defines the axis line), one radius at the perpendicular plane passing through point on axis, and one apex angle. 
				//  the index is 0-2 denotes the coordinates of the pts, 3-5 denotes the direction numbers and 6, 7 denotes radius 
				//  at given point and apex angle.
				//  The jacobian of the distance measure is now calculated 
			
				//The distance of a pt on the cone from the axis should be equal to the radius of circle at the perpendicular plane 
				//passing through that point
				//the definition of the function to be minimised is calculated from that 

					//First normalise the direction numbers to get dir. cosines. 
				modA = sqrt(pow(*(Param + 3), 2) + pow(*(Param + 4), 2) + pow(*(Param + 5), 2));
				a = *(Param + 3) / modA;
				b = *(Param + 4) / modA; 
				c = *(Param + 5) / modA;
				*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;

				//bring x,y,z to point closest to origin.
				x = *Param;
				y = *(Param + 1);
				z = *(Param + 2);
				pt_dot_axis = x*a + y*b + z*c;

				x -= pt_dot_axis * a;
				y -= pt_dot_axis * b;
				z -= pt_dot_axis * c;

				*Param = x;
				*(Param + 1) = y;
				*(Param + 2) = z;

				//normalize apex angle
				ang = *(Param + 7);

				ang -= int(ang/(M_PI*2.0));

				if (ang < 0 ) ang += M_PI * 2.0;
				if (ang > M_PI)
				{
					a = -a; b = -b; c = -c;
					*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;
					ang -= M_PI;
				}
				if (ang > M_PI/2.0) 
				{
					a = -a; b = -b; c = -c;
					*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;
					ang = M_PI - ang;
				}

				*(Param + 7) = ang;

				//normalize radius at base
				s = *(Param + 6);
				if (s < 0)
				{
					a = -a; b = -b; c = -c;
					*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;
					s = -s;
				}
				*(Param + 6) = s;

				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1) - y;
					dX[2] = *(Values + i + 2) - z;
				
					g = a * dX[0] + b * dX[1] + c * dX[2];
					u = c * dX[1] - b * dX[2];
					v = a * dX[2] - c * dX[0];
					w = b * dX[0] - a * dX[1];

					f = sqrt(pow(u,2) + pow(v,2) + pow(w,2));

					dist = f * f - (s  + tan(ang) * g) * (s  + tan(ang) * g);
				
					*(jacobian + jj) = 2 * (dX[0] - g * a - tan(ang) * (s + tan (ang) * g) * a); jj++;
					*(jacobian + jj) = 2 * (dX[1] - g * b - tan(ang) * (s + tan(ang) * g) * b); jj++;
					*(jacobian + jj) = 2 * (dX[2] - g * c - tan(ang) * (s + tan(ang) * g) * c); jj++;
					*(jacobian + jj) = 2 * (g + tan(ang) * (s + tan(ang) * g)) * dX[0] ; jj++;
					*(jacobian + jj) = 2 * (g + tan(ang) * (s + tan(ang) * g)) * dX[1]; jj++;
					*(jacobian + jj) = 2 * (g + tan(ang) * (s + tan(ang) * g)) * dX[2]; jj++;
					*(jacobian + jj) = 2 * (s + tan(ang) * g); jj++;
					*(jacobian + jj) = 2 * (1 / pow(cos(ang),2)) * (s + tan(ang) * g) * g; jj++;

					//The distance error between the guessed cone and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case ThreeDCircle:
				//	The parameter array consists of 7 numbers - 
				//  3 denoting center of circle; 3 direction numbers of the plane of circle and one radius of circle.
				//  the index is 0-2 denotes the coordinates of the pts, 3-5 denotes the direction numbers and 6 denotes radius o
				//  The jacobian of the distance measure is now calculated 
			
				//The distance of a pt on circle to center should be equal to the radius.
				//the definition of the function to be minimised is calculated from that 

					//First normalise the direction numbers to get dir. cosines. 
				modA = sqrt(pow(*(Param + 3), 2) + pow(*(Param + 4), 2) + pow(*(Param + 5), 2));
				a = *(Param + 3) / modA;
				b = *(Param + 4) / modA; 
				c = *(Param + 5) / modA;

				radius = *(Param + 6);
				if (radius < 0)
					radius = - radius;
				*(Param + 6) = radius;

				*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;
				jj = 0;
				kk = 0;
				temp = 0;
				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					dX[0] = *(Values + i + 0) - *(Param + 0);
					dX[1] = *(Values + i + 1) - *(Param + 1);
					dX[2] = *(Values + i + 2) - *(Param + 2);
				
					g = a * dX[0] + b * dX[1] + c * dX[2];
					u = c * dX[1] - b * dX[2];
					v = a * dX[2] - c * dX[0];
					w = b * dX[0] - a * dX[1];

					f = sqrt(pow(u,2) + pow(v,2) + pow(w,2));
				
					dist = pow(g,2) + pow(f - radius,2);

					*(jacobian + jj) = 2 * g * a - 2 * (f - radius) / f * ( v * c - w * b); jj++;
					*(jacobian + jj) = 2 * g * b - 2 * (f - radius) / f * ( w * a - u * c); jj++;
					*(jacobian + jj) = 2 * g * c - 2 * (f - radius) / f * ( u * b - v * a); jj++;
					*(jacobian + jj) = -2 * g * dX[0] - 2 * (f - radius) / f * ( v * dX[2] - w * dX[1]); jj++;
					*(jacobian + jj) = -2 * g * dX[1] - 2 * (f - radius) / f * ( w * dX[0] - u * dX[2]); jj++;
					*(jacobian + jj) = -2 * g * dX[2] - 2 * (f - radius) / f * ( u * dX[1] - v * dX[0]); jj++;
					*(jacobian + jj) = 2*(f - radius) ; jj++;

					//The distance error between the guessed 3 D circle and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist * dist;
				}
				break;
			case TwoDConic:
				//the parameter array consists of 6 numbers - 2 for x,y coordinates of focus, 1 for slope of diretrix,
				//2 for x,y coordinates of closest point on diretrix from focus, 1 for eccentricity
				//the distance measure to be minimized is based on conic property that ratio of distance of a point on conic from diretrix
				//and focal point is a constant, equal to eccentricity.

				//need to normalize parameters first.

				//normalize angle
				ang = *(Param + 2);
				ang -= int((ang / M_PI) + 0.5 ) * M_PI ;
				*(Param + 2) = ang;

				//normalize point on diretrix to closest point on diretrix from focus.
				//d is projection of distance from focus to point along the diretrix.  for closest point, d should be zero.
				d = (*(Param + 0) - *(Param + 3)) * cos (ang) + (*(Param + 1) - *(Param + 4)) * sin (ang);
				*(Param + 3) += cos (ang) * d;
				*(Param + 4) += sin (ang) * d;

				//calculate jacobian from the dist function defined as dist_focus - e * dist_diretrix
				jj = 0;
				kk = 0;
				temp = 0;
				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					d = (*(Values + i + 0) - *(Param + 3)) * cos (ang) + (*(Values + i + 1) - *(Param + 4)) * sin (ang);
					dist_diretrix = pow (*(Values + i + 0) - *(Param + 3), 2) + pow (*(Values + i + 1) - *(Param + 4), 2) - pow (d, 2);
				
					dist_focus = pow (*(Values + i + 0) - *(Param + 0), 2) + pow (*(Values + i + 1) - *(Param + 1), 2);

					dist = dist_focus - *(Param + 5) * *(Param + 5) * dist_diretrix;

					*(jacobian + jj) = 2 * (*(Values + i + 0) - *(Param + 0)); jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 1) - *(Param + 1)); jj++;
					*(jacobian + jj) = 2 * *(Param + 5) * *(Param + 5) * d * ((*(Values + i + 0) - *(Param + 3)) * sin (ang) - (*(Values + i + 1) - *(Param + 4)) * cos (ang)); jj++;
					*(jacobian + jj) = -2 * *(Param + 5) * *(Param + 5) * sin (ang) * ((*(Values + i + 0) - *(Param + 3)) * sin (ang) - (*(Values + i + 1) - *(Param + 4)) * cos (ang)); jj++;
					*(jacobian + jj) = 2 * *(Param + 5) * *(Param + 5) * cos (ang) * ((*(Values + i + 0) - *(Param + 3)) * sin (ang) - (*(Values + i + 1) - *(Param + 4)) * cos (ang)); jj++;
					*(jacobian + jj) = 2 * *(Param + 5) * dist_diretrix; jj++;

					//The distance error between the guessed 3 D conic and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist*dist;
				}
				d =0;
				break;
			case TwoDParabola:
				//the parameter array consists of 5 numbers - 2 for x,y coordinates of focus, 1 for slope of diretrix,
				//2 for x,y coordinates of closest point on diretrix from focus
				//the distance measure to be minimized is based on parabola property that ratio of distance of a point on conic from diretrix
				//and focal point is a constant, equal to 1.

				//need to normalize parameters first.

				//normalize angle
				ang = *(Param + 2);
				ang -= int((ang / M_PI) + 0.5 ) * M_PI ;
				*(Param + 2) = ang;

				//normalize point on diretrix to closest point on diretrix from focus.
				//d is projection of distance from focus to point along the diretrix.  for closest point, d should be zero.
				d = (*(Param + 0) - *(Param + 3)) * cos (ang) + (*(Param + 1) - *(Param + 4)) * sin (ang);
				*(Param + 3) += cos (ang) * d;
				*(Param + 4) += sin (ang) * d;

				//calculate jacobian from the dist function defined as dist_focus -  dist_diretrix
				jj = 0;
				kk = 0;
				temp = 0;
				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					d = (*(Values + i + 0) - *(Param + 3)) * cos (ang) + (*(Values + i + 1) - *(Param + 4)) * sin (ang);
					dist_diretrix = pow (*(Values + i + 0) - *(Param + 3), 2) + pow (*(Values + i + 1) - *(Param + 4), 2) - pow (d, 2);
				
					dist_focus = pow (*(Values + i + 0) - *(Param + 0), 2) + pow (*(Values + i + 1) - *(Param + 1), 2);

					dist = dist_focus - dist_diretrix;

					*(jacobian + jj) = 2 * (*(Values + i + 0) - *(Param + 0)) ; jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 1) - *(Param + 1)) ; jj++;
					*(jacobian + jj) = 2 * d * ((*(Values + i + 0) - *(Param + 3)) * sin (ang) - (*(Values + i + 1) - *(Param + 4)) * cos (ang)) ; jj++;
					*(jacobian + jj) = -2 * sin (ang) * ((*(Values + i + 0) - *(Param + 3)) * sin (ang) - (*(Values + i + 1) - *(Param + 4)) * cos (ang)) ; jj++;
					*(jacobian + jj) = 2 * cos (ang) * ((*(Values + i + 0) - *(Param + 3)) * sin (ang) - (*(Values + i + 1) - *(Param + 4)) * cos (ang)) ; jj++;
			   
					//The distance error between the guessed 3 D conic and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist*dist;
				}
				d =0;
				break;
			case Conicoid:
				//the parameter array consists of 7 numbers - 3 for coordinates of focus, 
				//3 coordinates of closest point on diretrix from focus, 1 for eccentricity
				//the distance measure to be minimized is based on conicoid property that ratio of distance of a point on conicoid from diretrix plane
				//and focal point is a constant, equal to eccentricity.

				//calculate jacobian from the dist function defined as dist = dist_focus^2 - e^2 * dist_diretrix^2
				jj = 0;
				kk = 0;
				temp = 0;

				//dir cosines of diretrix plane
				a = *(Param + 0) - *(Param + 3); b = *(Param + 1) - *(Param + 4); c = *(Param + 2) - *(Param + 5);
				modA = sqrt(pow(a,2) + pow(b,2) + pow(c,2));
				a /= modA; b /= modA; c /= modA;

				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					dist_diretrix = (*(Values + i + 0) - *(Param + 3)) * a + (*(Values + i + 1) - *(Param + 4)) * b + (*(Values + i + 2) - *(Param + 5)) * c;
					//square of distance to diretrix
					sq_dist_diretrix = pow(dist_diretrix, 2);
				
					//square of distance to focus
					sq_dist_focus = pow (*(Values + i + 0) - *(Param + 0), 2) + pow (*(Values + i + 1) - *(Param + 1), 2) + pow (*(Values + i + 2) - *(Param + 2), 2);

					dist = sq_dist_focus - *(Param + 6) * *(Param + 6) * sq_dist_diretrix;
					da_dxf = (1 - a * a) / modA; db_dxf = - b * b / modA; dc_dxf = - c * c / modA;
					da_dyf = - a * a / modA; db_dyf = (1 - b * b) / modA; dc_dyf = - c * c / modA;
					da_dzf = - a * a / modA; db_dzf = - b * b / modA; dc_dzf = (1 - c * c) / modA;
					ddd_dxf = 2 * dist_diretrix * ((*(Values + i + 0) - *(Param + 3)) * da_dxf + (*(Values + i + 1) - *(Param + 4)) * db_dxf + (*(Values + i + 2) - *(Param + 5)) * dc_dxf);
					ddd_dyf = 2 * dist_diretrix * ((*(Values + i + 0) - *(Param + 3)) * da_dyf + (*(Values + i + 1) - *(Param + 4)) * db_dyf + (*(Values + i + 2) - *(Param + 5)) * dc_dyf);
					ddd_dzf = 2 * dist_diretrix * ((*(Values + i + 0) - *(Param + 3)) * da_dzf + (*(Values + i + 1) - *(Param + 4)) * db_dzf + (*(Values + i + 2) - *(Param + 5)) * dc_dzf);
					ddd_dxd = - ddd_dxf - 2 * dist_diretrix * a;
					ddd_dyd = - ddd_dyf - 2 * dist_diretrix * b;
					ddd_dzd = - ddd_dzf - 2 * dist_diretrix * c;
					*(jacobian + jj) = 2 * (*(Values + i + 0) - *(Param + 0)) + *(Param + 6) * *(Param + 6) * ddd_dxf; jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 1) - *(Param + 1)) + *(Param + 6) * *(Param + 6) * ddd_dyf; jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 2) - *(Param + 2)) + *(Param + 6) * *(Param + 6) * ddd_dzf; jj++;
					*(jacobian + jj) = *(Param + 6) * *(Param + 6) * ddd_dxd; jj++;
					*(jacobian + jj) = *(Param + 6) * *(Param + 6) * ddd_dyd; jj++;
					*(jacobian + jj) = *(Param + 6) * *(Param + 6) * ddd_dzd; jj++;
					*(jacobian + jj) = 2 * *(Param + 6) * sq_dist_diretrix; jj++;

					//The distance error between the guessed conicoid and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist*dist;
				}
				d = 0;
				break;
			case Paraboloid:
				//the parameter array consists of 6 numbers - 3 for coordinates of focus, 
				//3 coordinates of closest point on diretrix from focus
				//the distance measure to be minimized is based on paraboloid property that ratio of distance of a point on conicoid from diretrix plane
				//and focal point is a constant, equal to 1.

				//calculate jacobian from the dist function defined as dist = dist_focus^2 - dist_diretrix^2
				jj = 0;
				kk = 0;
				temp = 0;

				//dir cosines of diretrix plane
				a = *(Param + 0) - *(Param + 3); b = *(Param + 1) - *(Param + 4); c = *(Param + 2) - *(Param + 5);
				modA = sqrt(pow(a,2) + pow(b,2) + pow(c,2));
				a /= modA; b /= modA; c /= modA;

				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					dist_diretrix = (*(Values + i + 0) - *(Param + 3)) * a + (*(Values + i + 1) - *(Param + 4)) * b + (*(Values + i + 2) - *(Param + 5)) * c;
					//square of distance to diretrix
					sq_dist_diretrix = pow(dist_diretrix, 2);
				
					//square of distance to focus
					sq_dist_focus = pow (*(Values + i + 0) - *(Param + 0), 2) + pow (*(Values + i + 1) - *(Param + 1), 2) + pow (*(Values + i + 2) - *(Param + 2), 2);

					dist = sq_dist_focus - sq_dist_diretrix;
					da_dxf = (1 - a * a) / modA; db_dxf = - b * b / modA; dc_dxf = - c * c / modA;
					da_dyf = - a * a / modA; db_dyf = (1 - b * b) / modA; dc_dyf = - c * c / modA;
					da_dzf = - a * a / modA; db_dzf = - b * b / modA; dc_dzf = (1 - c * c) / modA;
					ddd_dxf = 2 * dist_diretrix * ((*(Values + i + 0) - *(Param + 3)) * da_dxf + (*(Values + i + 1) - *(Param + 4)) * db_dxf + (*(Values + i + 2) - *(Param + 5)) * dc_dxf);
					ddd_dyf = 2 * dist_diretrix * ((*(Values + i + 0) - *(Param + 3)) * da_dyf + (*(Values + i + 1) - *(Param + 4)) * db_dyf + (*(Values + i + 2) - *(Param + 5)) * dc_dyf);
					ddd_dzf = 2 * dist_diretrix * ((*(Values + i + 0) - *(Param + 3)) * da_dzf + (*(Values + i + 1) - *(Param + 4)) * db_dzf + (*(Values + i + 2) - *(Param + 5)) * dc_dzf);
					ddd_dxd = - ddd_dxf - 2 * dist_diretrix * a;
					ddd_dyd = - ddd_dyf - 2 * dist_diretrix * b;
					ddd_dzd = - ddd_dzf - 2 * dist_diretrix * c;
					*(jacobian + jj) = 2 * (*(Values + i + 0) - *(Param + 0)) + ddd_dxf; jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 1) - *(Param + 1)) + ddd_dyf; jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 2) - *(Param + 2)) + ddd_dzf; jj++;
					*(jacobian + jj) = ddd_dxd; jj++;
					*(jacobian + jj) = ddd_dyd; jj++;
					*(jacobian + jj) = ddd_dzd; jj++;
			   
					//The distance error between the guessed conicoid and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist*dist;
				}
				d = 0;
				break;
			case Torus:
				//	The parameter array consists of 8 numbers - 
				//  3 denoting center of the torus; 3 direction numbers of the axis line, 
				//	one radius for outer circle and one for inner circle. 
				//  The jacobian of the distance measure is now calculated 
			
				//The distance of a pt on the torus from outer circle is a constant = equal to radius of the inner circle. 
				//the definition of the function to be minimised is calculated from that 

				//First normalise the direction numbers to get dir. cosines. 
				modA = sqrt(pow(*(Param + 3), 2) + pow(*(Param + 4), 2) + pow(*(Param + 5), 2));
				a = *(Param + 3) / modA;
				b = *(Param + 4) / modA; 
				c = *(Param + 5) / modA;
				*(Param + 3) = a; *(Param + 4) = b; *(Param + 5) = c;

				x = *(Param + 0);
				y = *(Param + 1);
				z = *(Param + 2);

				radius = *(Param + 6);
				if (radius < 0) 
					radius = - radius;
				*(Param + 6) = radius;
				s = *(Param + 7);
				if (s < 0)
					s = -s;
				*(Param + 7) = s;

				jj = 0;
				kk = 0;
				temp = 0;
				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1) - y;
					dX[2] = *(Values + i + 2) - z;
				
					g = a * dX[0] + b * dX[1] + c * dX[2];

					f = sqrt(pow(dX[0],2) + pow(dX[1],2) + pow(dX[2],2) - pow(g,2));

					dist = (radius - f) * (radius - f) + g * g - s * s;
				
					*(jacobian + jj) = -2 * (radius - f) * dX[0] / f + 2 * radius * g * a / f; jj++;
					*(jacobian + jj) = -2 * (radius - f) * dX[1] / f + 2 * radius * g * b / f; jj++;
					*(jacobian + jj) = -2 * (radius - f) * dX[2] / f + 2 * radius * g * c / f; jj++;
					*(jacobian + jj) = -2 * radius * g * dX[0] / f; jj++;
					*(jacobian + jj) = -2 * radius * g * dX[1] / f; jj++;
					*(jacobian + jj) = -2 * radius * g * dX[2] / f; jj++;
					*(jacobian + jj) = -2 * (radius - f); jj++;
					*(jacobian + jj) = 2 * s; jj++;

					//The distance error between the guessed torus and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist * dist;
				}
				d = 0;
				break;
			case TwoArc:
				//	The parameter array consists of 5 numbers - 
				//  2 denoting center of line joining centers of the arcs; 1 slope for reflection axis passing through this center, 
				//	one radial distance from center to each of the centers of the circles and one radius of the arcs/circles. 
				//  The jacobian of the distance measure is now calculated 
			
				//The distance of a pt on the "two arc" from its center is the radius of the arc. 
				//the definition of the function to be minimised is calculated from that 

				//First normalise the slope. 
				ang = Param[2];
				if (ang < 0)
					ang += 2 * M_PI;
				ang -= int((ang / M_PI) + 0.5 ) * M_PI ;
				Param[2] = ang;
				a = cos (ang);
				b = sin (ang);

				x = *(Param + 0);
				y = *(Param + 1);

				radius = *(Param + 3);
				if (radius < 0) 
					radius = - radius;
				*(Param + 3) = radius;
				s = *(Param + 4);
				if (s < 0)
					s = -s;
				*(Param + 4) = s;

				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					//R A N new algortihm - Any point on the arc will follow the eqn - 
					// (x - x1)^2 + (y-y1)^2 = r2^2. 
					//The point x1 and y1 are projected from a centre point x0, y0 on a circle of radius R1.
					//Therefore, x1 = x0 + R1 * cos(theta); y1 = y0 + R1 * sin(Theta)
					//Final eqn becomes : F = [x - (x0 + R1 * cos(theta)]^2 + [y - (y0 + R1 * sin(theta)]^2 - R2^2 = 0

					//Chandar's algorithm
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1) - y;
				
					g = a * dX[0] + b * dX[1];
					f = sqrt(pow(dX[0],2) + pow(dX[1],2) - pow(g,2));

					dist = (radius - f) * (radius - f) + g * g - s * s;
				
					*(jacobian + jj) = 2 * (radius - f) * (a * g - dX[0]) / f + 2 * g * a; jj++;
					*(jacobian + jj) = 2 * (radius - f) * (b * g - dX[1]) / f + 2 * g * b; jj++;
					*(jacobian + jj) = 2 * radius * g * (dX[0] * b - dX[1] * a) / f; jj++;
					*(jacobian + jj) = -2 * (radius - f); jj++;
					*(jacobian + jj) = 2 * s; jj++;

					//The distance error between the guessed "twoarc" and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case TwoArc_AxisGiven:
				//	the axis is assumed to be x-axis.  The parameter array consists of 3 numbers - 
				//  first one denoting point along x-axis of the center of 2-arc  
				//	second the outer radius, and third the inner radius. 
				//  The jacobian of the distance measure is now calculated 
			
				//The distance of a pt on the "two arc" from its center is the radius of the arc. 
				//the definition of the function to be minimised is calculated from that 

				x = *(Param + 0);

				radius = *(Param + 1);
				if (radius < 0) 
					radius = - radius;
				*(Param + 1) = radius;
				s = *(Param + 2);
				if (s < 0)
					s = -s;
				*(Param + 2) = s;

				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1);
				
					g = dX[0];
					f = sqrt(pow(dX[1],2));

					dist = (radius - f) * (radius - f) + g * g - s * s;
				
					*(jacobian + jj) = 2 * g; jj++;
					*(jacobian + jj) = -2 * (radius - f); jj++;
					*(jacobian + jj) = 2 * s; jj++;

					//The distance error between the guessed "twoarc" and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case TwoLine:
				//	The parameter array consists of 6 numbers - 
				//  first 3 denoting a pt on first line closest to origin and slope of first line; 
				//  next 3 likewise denoting these params for second line;
				//  The jacobian of the distance measure is now calculated 
			
				//A pt on the "two line" should reside on one of these two lines.

				ang = *(Param + 2);

				if (ang < 0)
					ang += 2*M_PI;
				ang -= int(ang / M_PI ) * M_PI ;
				*(Param + 2) = ang;

				////bring point on each of the lines to point closest to origin.
				x = *(Param + 0);
				y = *(Param + 1);

				x -= (x * cos (ang) + y * sin (ang)) * cos (ang);
				y -= (x * cos (ang) + y * sin (ang)) * sin (ang);
				*(Param + 0) = x;
				*(Param + 1) = y;

				s = *(Param + 5);
				if (s < 0)
					s += 2*M_PI;
				s -= int(s / M_PI ) * M_PI ;
				*(Param + 5) = s;
				u = *(Param + 3);
				v = *(Param + 4);
				u -= (u * cos (s) + v * sin (s)) * cos (s);
				v -= (u * cos (s) + v * sin (s)) * sin (s);
				*(Param + 3) = u;
				*(Param + 4) = v;

				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1) - y;
					dX[2] = *(Values + i + 0) - u;
					dX[3] = *(Values + i + 1) - v;
					g = sin(ang) * dX[0] - cos(ang) * dX[1];
					f = sin(s) *dX[2] - cos(s) * dX[3];

					dist = pow(g * f,2) ;
				
					*(jacobian + jj) = 2 * g * sin(ang) * f * f; jj++;
					*(jacobian + jj) = -2 * g * cos(ang) * f * f; jj++;
					*(jacobian + jj) = -2 * g * (dX[0] * cos(ang) + dX[1] * sin(ang)) * f * f; jj++;
					*(jacobian + jj) = 2 * f * sin(s) * g * g; jj++;
					*(jacobian + jj) = -2 * f * cos(s) * g * g; jj++;
					*(jacobian + jj) = -2 * f * (dX[2] * cos(s) + dX[3] * sin(s)) * g * g; jj++;

					//The distance error between the guessed "twoline" and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case TwoLine_AxisGiven:
				//	the reflection axis is assumed to be x-axis.  The parameter array consists of 2 numbers - 
				//  first one denoting distance of line-pair from origin and the second the angle of the line pair with x-axis.   
				//  The jacobian of the distance measure is now calculated 
			
				//A pt on the "two line" should reside on one of the two lines reflected around x axis at given angle/distance.
				//the definition of the function to be minimised is calculated from that 
				//First normalise the slope of axis. 

				//normalize slope of line pair with axis
				d = *(Param + 0);
				s = *(Param + 1);
				if (s < 0)
					s += 2*M_PI;
				s -= int(s / M_PI ) * M_PI ;
				if (d > 0)
				{
					if (s < M_PI_2)
						s = M_PI - s;
				}
				else
				{
					if (s > M_PI_2)
						s = M_PI - s;
				}
				*(Param + 1) = s;

				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					dX[0] = *(Values + i + 0);
					dX[1] = *(Values + i + 1);

					g = -dX[0] * sin (s) + dX[1] * cos (s) + d;
					f = dX[0] * sin (s) + dX[1] * cos (s) - d;

					dist = pow(g * f, 2);
				
					*(jacobian + jj) = -2 * g * f * f + 2 * g * g * f; jj++;
					*(jacobian + jj) = 2 * g * (dX[0] * cos (s) + dX[1] * sin (s)) * f * f - 2 * g * g * f * (dX[0] * cos (s) - dX[1] * sin (s)); jj++;
				
					//The distance error between the guessed "twoline" and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case LineArcLine:
				//	The parameter array consists of 5 numbers - ang1, ang2 the angles of the tangential lines.  Next 3 being circle center and radius.
				//  The jacobian of the distance measure is now calculated 
			
				// Distance function would be product of distance from center of circle and perpendicular distance from tangent line to center for each of lines..

				a = *(Param + 0);
				b = *(Param + 1);
				x = *(Param + 2);
				y = *(Param + 3);
				radius = *(Param + 4);
				if (radius < 0)
					radius = - radius;
				*(Param + 4) = radius;

				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1) - y;

					f = dX[0] * dX[0] + dX[1] * dX[1] - radius * radius;
					u = pow(dX[0] * sin (a) - dX[1] * cos (a),2) - pow(radius, 2);
					v = pow(dX[0] * sin (b) - dX[1] * cos (b),2) - pow(radius, 2);

					dist = f * u * v;
				
					*(jacobian + jj) = - 2 * f * v * (dX[0] * sin (a) - dX[1] * cos (a)) * (dX[0] * cos (a) + dX[1] * sin (a)); jj++;
					*(jacobian + jj) = - 2 * f * u * (dX[0] * sin (b) - dX[1] * cos (b)) * (dX[0] * cos (b) + dX[1] * sin (b)); jj++;
					*(jacobian + jj) = 2 * (dX[0] * u * v + f * v * (dX[0] * sin (a) - dX[1] * cos (a)) * sin (a) + f * u * (dX[0] * sin (b) - dX[1] * cos (b)) * sin (b)); jj++;
					*(jacobian + jj) = 2 * (dX[1] * u * v - f * v * (dX[0] * sin (a) - dX[1] * cos (a)) * cos (a) - f * u * (dX[0] * sin (b) - dX[1] * cos (b)) * cos (b)); jj++;
					*(jacobian + jj) = 2 * radius * (u * v + f * v + f * u); jj++;

					//The distance error between the guessed "line-arc-line" and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case LineArcLine_AxisGiven:
				//	The parameter array consists of 2 numbers - x value of center (along x-axis) and radius of arc.  
			
				// Distance function would be radius of the arc

				x = *(Param + 0);

				radius = *(Param + 1);
				if (radius < 0) 
					radius = - radius;
				*(Param + 1) = radius;

				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1);
				
					dist = dX[0] * dX[0] + dX[1] * dX[1] - radius * radius;
				
					*(jacobian + jj) = 2 * dX[0]; jj++;
					*(jacobian + jj) = 2 * radius; jj++;

					//The distance error between the guessed "twoarc" and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case ArcArc:
				//six parameters are - center1, radius1, radius2, intersection point
				a = Param[0]; b = Param[1]; 
				radius = Param[2]; s = Param[3];
				if (radius == 0)
					return false;
				u = Param[4]; v = Param[5];
				w = u - a; x = v - b;
				dist = pythag(w, x);
				if (dist == 0)
					return false;
			
				w = w/dist; x = x/dist;
				u = w * radius + a; v = x * radius + b;

				c = a + (radius - s) * w;
				d = b + (radius - s) * x;

				//normalize intersection point
				Param[4] = u; Param[5] = v;

				//distance function to be minimized is the product of the distance of points from the two centers minus each of the radii.
				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{
					g = (pow(*(Values + i + 0) - a, 2) + pow(*(Values + i + 1) - b, 2) - pow(radius, 2));
					f = (pow(*(Values + i + 0) - c, 2) + pow(*(Values + i + 1) - d, 2) - pow(s, 2));
					z = g * f;

					*(jacobian + jj) = 2 * f * (*(Values + i + 0) - a) + 2 * g * (*(Values + i + 0) - c) * ( 1 - (radius - s) / radius); jj++;
					*(jacobian + jj) = 2 * f * (*(Values + i + 1) - b) + 2 * g * (*(Values + i + 1) - d) * ( 1 - (radius - s) / radius); jj++;
					*(jacobian + jj) = 2 * f * radius + 2 * g * ((*(Values + i + 0) - c) * s / radius * w + (*(Values + i + 1) - d) * s / radius * x); jj++;
					*(jacobian + jj) = 2 * g * (s - (*(Values + i + 0) - c) * w - (*(Values + i + 1) - d) * x); jj++;
					*(jacobian + jj) = 2 * g * (*(Values + i + 0) - c) * (radius - s) / radius; jj++;
					*(jacobian + jj) = 2 * g * (*(Values + i + 1) - d) * (radius - s) / radius; jj++;

					//The distance error between the guessed "arcarc" and the given pt
					*(residual + kk) =	z; kk++;
				}
				break;
			case ArcArc_ArcGiven:
				u = Param[0]; 
				v = Param[1];
				s = Param[2];
				dist = pythag(u, v);
				if (dist == 0)
					return false;
				w = u/dist; x = v/dist;
				c = w * (dist - s); d = x * (dist - s);

				//distance function to be minimized is distance of points from center minus radius
				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{
					z = (pow(*(Values + i + 0) - c, 2) + pow(*(Values + i + 1) - d, 2) - pow(s, 2));

					*(jacobian + jj) = 2 * (*(Values + i + 0) - c) * ( 1 - s / dist * ( 1 - pow( w, 2))) + 2 * (*(Values + i + 0) - d) * (s / dist * pow(x, 2)); jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 0) - c) * (s / dist * pow( w, 2)) + 2 * (*(Values + i + 1) - d) * ( 1 - s / dist * ( 1 - pow(x, 2))); jj++;
					*(jacobian + jj) = - 2 * (*(Values + i + 0) - c) * w - 2 * (*(Values + i + 0) - d) * x + 2 * s; jj++;

					//The distance error between the guessed "arcarc" and the given pt
					*(residual + kk) =	z; kk++;
				}
				break;
			case LineArc:
				//four parameters are - slope, intercept and center of circle
				a = Param[0]; b = Param[1];
				u = Param[2]; v = Param[3];
				if ((a == M_PI_2) || (a == 3 * M_PI_2))
				{
					radius = abs(u - b);
					//store dr^2/da, dr^2/db, dr^2/du, dr^2/dv
					dX[0] = 0;
					dX[1] = - 2 * (u - b);
					dX[2] = 2 * (u - b);
					dX[3] = 0;
				}
				else
				{
					radius = abs((tan(a) * u + b - v) * cos(a));
					//store dr^2/da, dr^2/db, dr^2/du, dr^2/dv
					dX[0] = 2 * (u * sin(a) + (b-v) * cos(a)) * ( u * cos (a) - (b-v) * sin(a));
					dX[1] = 2 * (u * sin(a) + (b-v) * cos(a)) * cos (a);
					dX[2] = 2 * (u * sin(a) + (b-v) * cos(a)) * sin (a);
					dX[3] = - 2 * (u * sin(a) + (b-v) * cos(a)) * cos (a);
				}
				if (radius == 0)
					return false;
			
				//distance function to be minimized is the product of the distance of points from the center minus radius 
				//and distance of points to line thru center at same slope minus radius.
				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{
					g = pow(*(Values + i + 0) - u, 2) + pow(*(Values + i + 1) - v, 2) - pow(radius, 2);
					f = pow((*(Values + i + 0) - u) * sin (a) - (*(Values + i + 1) - v) * cos (a),2) - pow(radius, 2);
					z = g * f;

					*(jacobian + jj) = dX[0] * f + g * dX[0] - 2 * g * ((*(Values + i + 0) - u) * sin (a) - (*(Values + i + 1) - v) * cos (a)) * ((*(Values + i + 0) - u) * cos (a) + (*(Values + i + 1) - v) * sin (a)) ; jj++;
					*(jacobian + jj) = dX[1] * f + g * dX[1]; jj++;
					*(jacobian + jj) = dX[2] * f + g * dX[2] + 2 * f * (*(Values + i + 0) - u) + 2 * g * ((*(Values + i + 0) - u) * sin (a) - (*(Values + i + 1) - v) * cos (a)) * sin(a); jj++;
					*(jacobian + jj) = dX[3] * f + g * dX[3] + 2 * f * (*(Values + i + 1) - v) - 2 * g * ((*(Values + i + 0) - u) * sin (a) - (*(Values + i + 1) - v) * cos (a)) * cos(a); jj++;

					//The distance error between the guessed "linearc" and the given pt
					*(residual + kk) =	z; kk++;
				}
				break;
			case LineArc_LineGiven:
				u = Param[0]; 
				v = Param[1];

				//distance function to be minimized is distance of points from center minus radius which is equal to abs(v)
				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{
					z = pow(*(Values + i + 0) - u, 2) + pow(*(Values + i + 1) - v, 2) - pow(v, 2);

					*(jacobian + jj) = 2 * (*(Values + i + 0) - u); jj++;
					*(jacobian + jj) = 2 * (*(Values + i + 0) - v) + 2 * v; jj++;

					//The distance error between the guessed "linearc" and the given pt
					*(residual + kk) =	z; kk++;
				}
				break;
			case ArcArcArc:
				//	The parameter array consists of 9 numbers - center, radius of middle arc, radius of the two extreme arcs and the two intersection points on either side
			
				// Distance function would be product of distance from centers of all three arcs

				x = *(Param + 0);
				y = *(Param + 1);
				a = *(Param + 2);
				b = *(Param + 3);
				c = *(Param + 4);
				u = *(Param + 5);
				v = *(Param + 6);
				w = *(Param + 7);
				z = *(Param + 8);

				if (a < 0)
				{
					a = - a;
					*(Param + 2) = a;
				}

				if (b < 0)
				{
					b = -b;
					*(Param + 3) = b;
				}

				if (c < 0)
				{
					c = -c;
					*(Param + 4) = c;
				}
				rad_vec1[0] = x - u;
				rad_vec1[1] = y - v;
				rad_vec2[0] = x - w;
				rad_vec2[1] = y - z;
				norm = pythag(rad_vec1[0], rad_vec1[1]);
				if (norm == 0)
					return false;
				for (int i = 0; i < 2; i++)
					rad_vec1[i] = rad_vec1[i] / norm;
				norm = pythag(rad_vec2[0], rad_vec2[1]);
				if (norm == 0)
					return false;
				for (int i = 0; i < 2; i++)
					rad_vec2[i] = rad_vec2[i] / norm;	

				f = x + (a - b) * rad_vec1[0];
				g = y + (a - b) * rad_vec1[1];

				s = x - (b - c) * rad_vec2[0];
				d = y - (b - c) * rad_vec2[1];
				double dE_dx, dQ_dx, dR_dx, dE_dy, dQ_dy, dR_dy, dE_da, dQ_da, dR_da, dE_db, dQ_db, dR_db, dE_dc, dQ_dc, dR_dc, dE_du, dQ_du, dR_du, dE_dv, dQ_dv, dR_dv, dE_dw, dQ_dw, dR_dw, dE_dz, dQ_dz, dR_dz;
				jj = 0;
				kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					dX[0] = *(Values + i + 0) - x;
					dX[1] = *(Values + i + 1) - y;

					dX[2] = *(Values + i + 0) - f;
					dX[3] = *(Values + i + 1) - g;

					dX[4] = *(Values + i + 0) - s;
					dX[5] = *(Values + i + 1) - z;

					E = dX[0] * dX[0] + dX[1] * dX[1] - b * b;
					Q = dX[2] * dX[2] + dX[3] * dX[3] - a * a;
					R = dX[4] * dX[4] + dX[5] * dX[5] - c * c;

					dE_dx = - 2 * dX[0];
					dQ_dx = - 2 * dX[2] * (1 + (a - b) * pow(y - v, 2) / pow(pow (x-u, 2) + pow(y-v,2), 3/2) ) - 2 * dX[3] * (a - b) * (y - v) * (x - u) / pow(pow (x-u, 2) + pow(y-v,2), 3/2) ;
					dR_dx = - 2 * dX[4] * (1 - (b - c) * pow(y - z, 2) / pow(pow (x-w, 2) + pow(y-z,2), 3/2) ) + 2 * dX[5] * (b - c) * (y - z) * (x - w) / pow(pow (x-w, 2) + pow(y-z,2), 3/2) ;

					dE_dy = - 2 * dX[1];
					dQ_dy = - 2 * dX[2] * (a - b) * (x - u) * (y - v) / pow(pow (x-u, 2) + pow(y-v,2), 3/2) - 2 * dX[3] * (1 + (a - b) * pow(x - u, 2) / pow(pow (x-u, 2) + pow(y-v,2), 3/2)) ;
					dR_dy =  2 * dX[4] * (b - c) * (x - w) * (y - z) / pow(pow (x-w, 2) + pow(y-z,2), 3/2) - 2 * dX[5] * (1 - (b - c) * pow(x - w, 2) / pow(pow (x-w, 2) + pow(y-z,2), 3/2)) ;

					dE_da = 0;
					dQ_da = - 2 * a - 2 * dX[2] * (x - u) / sqrt (pow(x-u,2) + pow(y-v,2)) - 2 * dX[3] * (y - v) / sqrt (pow(x-u,2) + pow(y-v,2));
					dR_da = 0;

					dE_db = - 2 * b;
					dQ_db = 2 * dX[2] * (x - u) / sqrt (pow(x-u,2) + pow(y-v,2)) + 2 * dX[3] * (y - v) / sqrt (pow(x-u,2) + pow(y-v,2));
					dR_db = 2 * dX[4] * (x - w) / sqrt (pow(x-w,2) + pow(y-z,2)) + 2 * dX[5] * (y - z) / sqrt (pow(x-w,2) + pow(y-z,2));

					dE_dc = 0;
					dQ_dc = 0;
					dR_dc = - 2 * c - 2 * dX[4] * (x - w) / sqrt (pow(x-w,2) + pow(y-z,2)) - 2 * dX[5] * (y - z) / sqrt (pow(x-w,2) + pow(y-z,2));

					dE_du = 0;
					dQ_du = 2 * dX[2] * (a - b) * pow(y - v,2) / pow(pow (x-u, 2) + pow(y-v,2), 3/2) + 2 * dX[3] * (a - b) * (y - v) * (x - u) / pow(pow (x-u, 2) + pow(y-v,2), 3/2) ;
					dR_du = 0;

					dE_dv = 0;
					dQ_dv = 2 * dX[2] * (a - b) * (x - u) * (y - v) / pow(pow (x-u, 2) + pow(y-v,2), 3/2) + 2 * dX[3] * (a - b) * pow(x - u, 2) / pow(pow (x-u, 2) + pow(y-v,2), 3/2) ;
					dR_dv = 0;

					dE_dw = 0;
					dQ_dw = 0;
					dR_dw = 2 * dX[4] * (c - b) * pow(y - z,2) / pow(pow (x-w, 2) + pow(y-z,2), 3/2) + 2 * dX[5] * (c - b) * (y - z) * (x - w) / pow(pow (x-w, 2) + pow(y-z,2), 3/2) ;
				
					dE_dz = 0;
					dQ_dz = 0;
					dR_dz = 2 * dX[4] * (c - b) * (x - w) * (y - z) / pow(pow (x-w, 2) + pow(y-z,2), 3/2) + 2 * dX[5] * (c - b) * pow(x - w, 2) / pow(pow (x-w, 2) + pow(y-z,2), 3/2) ;

					dist = E * Q * R;
				
					*(jacobian + jj) = - dE_dx * Q * R - E * dQ_dx * R - E * Q * dR_dx; jj++;
					*(jacobian + jj) = - dE_dy * Q * R - E * dQ_dy * R - E * Q * dR_dy; jj++;
					*(jacobian + jj) = - dE_da * Q * R - E * dQ_da * R - E * Q * dR_da; jj++;
					*(jacobian + jj) = - dE_db * Q * R - E * dQ_db * R - E * Q * dR_db; jj++;
					*(jacobian + jj) = - dE_dc * Q * R - E * dQ_dc * R - E * Q * dR_dc; jj++;
					*(jacobian + jj) = - dE_du * Q * R - E * dQ_du * R - E * Q * dR_du; jj++;
					*(jacobian + jj) = - dE_dv * Q * R - E * dQ_dv * R - E * Q * dR_dv; jj++;
					*(jacobian + jj) = - dE_dw * Q * R - E * dQ_dw * R - E * Q * dR_dw; jj++;
					*(jacobian + jj) = - dE_dz * Q * R - E * dQ_dz * R - E * Q * dR_dz; jj++;

					//The distance error between the guessed "arc-arc-arc" and the given pt
					*(residual + kk) =	dist; kk++;
				}
				break;
			case Sphere:
				radius = *(Param + 3);
				if (radius < 0)
					radius = - radius;
				*(Param + 3) = radius;

				//distance function to be minimized is distance of points from center minus radius
				jj = 0;
				kk = 0;
				temp = 0;
				for(int i = 0; i < TotalPoints * 3; i += 3)
				{	
					dX[0] = *(Values + i + 0) - *(Param + 0);
					dX[1] = *(Values + i + 1) - *(Param + 1);
					dX[2] = *(Values + i + 2) - *(Param + 2);
				
					dist = pow(dX[0],2) + pow(dX[1],2) + pow(dX[2],2) - pow(radius,2);

					*(jacobian + jj) = 2 * dX[0]; jj++;
					*(jacobian + jj) = 2 * dX[1]; jj++;
					*(jacobian + jj) = 2 * dX[2]; jj++;
					*(jacobian + jj) = 2* radius ; jj++;

					//The distance error between the guessed sphere and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist * dist;
				}
				break;
			case CircleInvolute:
				radius = *(Param + 2);
				if (radius < 0)
					radius = - radius;
				*(Param + 2) = radius;
				x = *(Param + 0);
				y = *(Param + 1);
				ang = *(Param + 3);
				//ang -= int(ang / 2 * M_PI ) * 2 * M_PI ;
				//if (ang < 0)
				//	ang += 2 * M_PI;
				//*(Param + 3) = ang;

				//distance function to be minimized is distance of points from circle center minus the calculated R of involute.
				while (retry)
				{
					jj = 0;
					kk = 0;
					temp = 0;
					w = 0;
					f = 0;
					retry = false;
					for(int i = 0; i < TotalPoints * 2; i += 2)
					{	
						dX[0] = *(Values + i + 0) - *(Param + 0);
						dX[1] = *(Values + i + 1) - *(Param + 1);
						v = pow(dX[0],2) + pow(dX[1],2);
				
						if (dX[0] == 0)
						{
							if (dX[1] >= 0)
								s = M_PI_2;
							else
								s = 3*M_PI_2;
						}
						else
						{
							s = atan(dX[1]/dX[0]);
							if (dX[0] < 0)
								s += M_PI;
							if (s < 0)
								s += 2*M_PI;
						}
						if (s < ang)
							s += 2*M_PI;
						if ((v > w) && (s < f))
							s += 2 * M_PI;
						if ((v < w) && (s > f))
						{
							s -= 2 * M_PI;
							if (s < ang)
							{
								ang -= 2*M_PI;
								retry = true;
								break;
							}
						}
						f = s;
						w = v;
						s -= ang;
						//s is the polar angle of involute point and u is the polar angle of point on circle from where tangent is drawn to reach involute point.
						u = getTangentAng(s);
						//R is the square of the distance from involute point to center of base circle.
						R = pow(radius,2) * ( 1 + pow(u,2));
						//dist is the deviation in distance from point to center to the calculated distance for ideal involute.
						dist = v - R;
						//calculate jacobian by taking - derivative of dist with each of the params.
						*(jacobian + jj) = 2 * dX[0] + 2 * pow(radius, 2) * ((1 + pow(u,2))/u) * dX[1]/(pow(dX[0],2) + pow(dX[1],2)); jj++;
						*(jacobian + jj) = 2 * dX[1] - 2 * pow(radius, 2) * ((1 + pow(u,2))/u) * dX[0]/(pow(dX[0],2) + pow(dX[1],2)); jj++;
						*(jacobian + jj) = 2 * radius * (1 + pow(u,2)); jj++;
						*(jacobian + jj) = -2 * pow(radius, 2) * ((1 + pow(u,2))/u); jj++;

						//The distance error between the guessed circle involute and the given pt
						*(residual + kk) =	dist; kk++;
						temp += dist * dist;
					}
				}
				break;
			case CamProfile:
				radius = *(Param + 2);
				if (radius < 0)
					radius = - radius;
				*(Param + 2) = radius;
				x = *(Param + 0);
				y = *(Param + 1);
				ang = *(Param + 3);
				ang -= int(ang / (2 * M_PI) ) * 2 * M_PI ;
				if (ang < 0)
					ang += 2 * M_PI;
				*(Param + 3) = ang;
				//distance function to be minimized is distance of points from the cam profile.
				jj = 0;
				kk = 0;
				temp = 0;
				w = 0;
				f = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{	
					dX[0] = *(Values + i + 0) - *(Param + 0);
					dX[1] = *(Values + i + 1) - *(Param + 1);

					//point to center distance
					v = sqrt(pow(dX[0],2) + pow(dX[1],2));

					//a is ray angle of point
					if (v > 0)
					{
						a = acos(dX[0]/v);
						if (dX[1] < 0)
							a = 2*M_PI - a;
					}
					else
						a = 0;

					//c is ray angle wrt major dia
					c = a - ang; 
					c -= int(c / (2 * M_PI) ) * 2 * M_PI ;
					if (c < 0)
						c += 2 * M_PI;

					//dist is distance from ideal cam profile to point
					//jacobian is - derivative of dist wrt each of the 4 params
					if ((c <= M_PI/6) || (c >=11*M_PI/6))
					{
						dist = radius - v;
						*(jacobian + jj) = -dX[0]/v; jj++;
						*(jacobian + jj) = -dX[1]/v; jj++;
						*(jacobian + jj) = -1; jj++;
						*(jacobian + jj) = 0; jj++;
					}
					else if ((c >= 5*M_PI/6) && (c <=7*M_PI/6))
					{
						dist = radius + 4 - v;
						*(jacobian + jj) = -dX[0]/v; jj++;
						*(jacobian + jj) = -dX[1]/v; jj++;
						*(jacobian + jj) = -1; jj++;
						*(jacobian + jj) = 0; jj++;
					}
					else if ((c > M_PI/6) && (c <5*M_PI/6))
					{
						dist = radius + 2 * (1 - cos(1.5*c - M_PI/4)) - v;
						*(jacobian + jj) = -dX[0]/v - 3 * sin(1.5*c - M_PI/4) * dX[1] / (v * v); jj++;
						*(jacobian + jj) = -dX[1]/v + 3 * sin(1.5*c - M_PI/4) * dX[0] / (v * v); jj++;
						*(jacobian + jj) = -1; jj++;
						*(jacobian + jj) = 3 * sin(1.5*c - M_PI/4) ; jj++;
					}
					else
					{
						dist = radius + 2 * (1 - sin(1.5*c - M_PI/4)) - v;
						*(jacobian + jj) = -dX[0]/v + 3 * cos(1.5*c - M_PI/4) * dX[1] / (v * v); jj++;
						*(jacobian + jj) = -dX[1]/v - 3 * cos(1.5*c - M_PI/4) * dX[0] / (v * v); jj++;
						*(jacobian + jj) = -1; jj++;
						*(jacobian + jj) = -3 * cos(1.5*c - M_PI/4); jj++;
					}

					//The distance error between the guessed cam profile and the given pt
					*(residual + kk) =	dist; kk++;
					temp += dist * dist;
				}
				break;
			case Sinusoid:
				double tempAngle = 0;
				jj = 0; kk = 0;
				for(int i = 0; i < TotalPoints * 2; i += 2)
				{
					tempAngle = Values[i] - Param[1];
					jacobian[jj] = -1; jj ++;
					jacobian[jj] = Param[2] * sin(tempAngle); jj++;
					jacobian[jj] = -cos(tempAngle); jj++;
					residual[kk] = Param[0] + Param[2] * cos(tempAngle) - Values[i + 1]; kk++;
				}
				break;
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}

bool HelperMath::LS_BF(	double *Values, 
						double* StartingGuess, 
						int TotalPts, 
						int ConstraintParamIndex, 
						int NoOfParams, 
						int DoNotUpdateIndex, 
						Func Function, 
						double toleranceVal,
						double* Answer)
{	//If the parameter to be kept constant is the same as the parameter on whose basis we evaluate convergence, 
	//calling definitions are wrong!! exit immediately!!
	if (DoNotUpdateIndex >= 0 && DoNotUpdateIndex == ConstraintParamIndex) return false;
	//If we dont have points, we cannot iterate!!
	if (TotalPts == 0) return false;

	//Local Variables ...
	bool Succeeded = false;
	double* param = (double*) calloc(NoOfParams, sizeof(double));
	double* jacobian = (double*) calloc(NoOfParams * TotalPts, sizeof(double));
	double* residual = (double*) calloc(TotalPts, sizeof(double));
	double* jac_temp = (double*) calloc(NoOfParams * NoOfParams, sizeof(double) );
	double* jac_inverse = (double*) calloc(NoOfParams * NoOfParams, sizeof(double));
	double* jac_residual = (double*) calloc(NoOfParams, sizeof(double));
	double* deltaParam = (double*) calloc(NoOfParams, sizeof(double));

	//First Copy the starting guess to the param array. 
	memcpy((void*)param, (void*)StartingGuess, NoOfParams * sizeof(double)); 
	
	int iterationCt = 0;
	//Initialise the delta to be a large number to start iterations...
	*(deltaParam + ConstraintParamIndex) = 200.0;
	bool ContinueIterations = true;

	//Check if iterations have to continue...
	while (ContinueIterations) //(abs(*(deltaParam + ConstraintParamIndex)) > 0.000001)
	{	//If we have iterated more than 3000 times, most likely we are not getting convergence. exit
		if (iterationCt > 6000) 
		{	return false;
		}
		//Increase iteration ct
		iterationCt ++;
		//Initialise the non-updated parameter to the initial value itself
		if (DoNotUpdateIndex >= 0) *(param + DoNotUpdateIndex) = *(StartingGuess + DoNotUpdateIndex);
		
		//Get the jacobian matrix
		ZeroMemory(jacobian, sizeof(double)* NoOfParams * TotalPts);
		Get_Jac(Function, Values, param, TotalPts, NoOfParams, jacobian, residual);
		
		//reset all memory to 0
		ZeroMemory(jac_temp, sizeof(double) * NoOfParams * NoOfParams);
		ZeroMemory(jac_residual, sizeof(double) * NoOfParams);
		ZeroMemory(deltaParam, sizeof(double) * NoOfParams);
		ZeroMemory(jac_inverse, sizeof(double) * NoOfParams * NoOfParams);
		
		//Compute the nXn matrix JacTranspose * Jacobian
		for (int j = 0; j < NoOfParams; j ++)
		{	for (int k = j; k < NoOfParams; k ++)
			{	for (int i = 0; i < TotalPts; i ++)
				{	*(jac_temp + j * NoOfParams + k) += *(jacobian + i * NoOfParams + j) * *(jacobian + i * NoOfParams + k);
				}
			}
		}
		
		//fill up jac_temp
		for (int j = 0; j < NoOfParams; j ++)
		{	for (int k = 0; k <= j; k ++)
			{	*(jac_temp + j * NoOfParams + k) = *(jac_temp + k * NoOfParams + j);
			}
		}
		
		//Invert the jac_temp matrix
		OperateMatrix(jac_temp, NoOfParams, 1, jac_inverse);
		
		//Now calculate JacT* K
		for (int j = 0; j < NoOfParams; j ++)
		{	for (int i = 0; i < TotalPts; i ++)
			{	*(jac_residual + j) += *(jacobian + i * NoOfParams + j) * *(residual + i);
			}
		}
		
		//Multiply the inverse with above vector to give correction to parameters
		for (int j = 0; j < NoOfParams; j ++)
		{	for (int i = 0; i < NoOfParams; i ++)
			{	*(deltaParam + j) += *(jac_inverse + j * NoOfParams + i) * *(jac_residual + j);
			}
		}

		//int S1[2] = {NoOfParams, NoOfParams};
		//int S2[2] = {NoOfParams, 1};
		//MultiplyMatrix(jac_inverse, &S1[0], jac_residual, &S2[0], deltaParam);

		//Check for convergence
		if (*(deltaParam + ConstraintParamIndex) < toleranceVal) 
		{	ContinueIterations = true; Succeeded = true;
		}
	
		//Update the values and continue iterations...
		for (int j = 0; j < NoOfParams; j ++)
		{	*(param + j) += *(deltaParam + j);
		}
	}
		//Exited loop. So we got the answer. Copy to location and return true!
	Succeeded = true;
	if (Succeeded) memcpy((void*)Answer, (void*) param, NoOfParams * sizeof(double));
	free(param); free(jac_temp); free(residual); free(jacobian); free(jac_inverse); free(jac_residual); free(deltaParam);
	return Succeeded;
}


bool HelperMath::LS_BF_LM(
							double *Values, 
							double* StartingGuess, 
							int TotalPts, 
							int ConstraintParamIndex, 
							int NoOfParams, 
							int DoNotUpdateIndex, 
							Func Function, 
							bool ConstrainAllParams,
							int MaxIterationCt,
							double toleranceVal,
			
							double* Answer)

{	
	try
	{
		if (DoNotUpdateIndex >= 0 && DoNotUpdateIndex == ConstraintParamIndex) return false;
		if (TotalPts == 0) return false;
	
		//Local Variables
		bool Succeeded = false;
		if (NoOfParams > CurrentParameterNumber || TotalPts > CurrentPointsCt)
		{	CurrentParameterNumber = NoOfParams; CurrentPointsCt = TotalPts;
			Allocate_LSBF_Memory();
		}


		bool ContinueIterations = true;
		double Nu  = 2, Res_Size = 0, PrevRes_Size;
		int iterationCt = 0;
		double maxJii = 0, TauValue = 0.001, delP_Size;

		//First Copy the starting guess to the param array. 
		memcpy((void*)param, (void*)StartingGuess, NoOfParams * sizeof(double)); 
		for (int i = 0; i < NoOfParams; i ++) *(param + i) = *(StartingGuess + i);

		double* bestparam = (double*) malloc(NoOfParams * sizeof(double));
		memcpy((void*)bestparam, (void*)StartingGuess, NoOfParams * sizeof(double));
		double minResSize = 0;
		*(deltaParam + ConstraintParamIndex) = 200.0;
	 
		while (ContinueIterations)
		{	if (iterationCt > MaxIterationCt) 
			{	
				break;
			}

			iterationCt ++;
			if (DoNotUpdateIndex >= 0) *(param + DoNotUpdateIndex) = *(StartingGuess + DoNotUpdateIndex);
		
			//Get the jacobian matrix
			ZeroMemory(jacobian, sizeof(double)* NoOfParams * TotalPts);
			//Get the Jacobian and Residual
			Get_Jac(Function, Values, param, TotalPts, NoOfParams, jacobian, residual);
		
			PrevRes_Size = Res_Size; Res_Size = 0;
			//Get the sum of squares of the residuals
			for (int i = 0; i < TotalPts; i ++) Res_Size += *(residual + i) * *(residual + i);
		
			if (iterationCt == 1)
			{
				minResSize = Res_Size;
			}
			else
			{
				if (Res_Size < minResSize)
				{
					minResSize = Res_Size;
					memcpy((void*)bestparam, (void*)param, NoOfParams * sizeof(double));
					Succeeded = true;
				}
			}
			//reset all memory to 0
			ZeroMemory(jac_temp, sizeof(double) * NoOfParams * NoOfParams);
			ZeroMemory(jac_residual, sizeof(double) * NoOfParams);
			ZeroMemory(jac_inverse, sizeof(double) * NoOfParams * NoOfParams);
		
			//Compute the nXn matrix JacTranspose * Jacobian
			for (int j = 0; j < NoOfParams; j ++)
			{	for (int k = j; k < NoOfParams; k ++)
				{	for (int i = 0; i < TotalPts; i ++)
					{	*(jac_temp + j * NoOfParams + k) += *(jacobian + i * NoOfParams + j) * *(jacobian + i * NoOfParams + k);
					}
				}
			}
		
			//fill up jac_temp
			for (int j = 0; j < NoOfParams; j ++)
			{	for (int k = 0; k <= j; k ++)
				{	*(jac_temp + j * NoOfParams + k) = *(jac_temp + k * NoOfParams + j);
				}
			}
		
			//If its the first run, calculate the initial max diagonal element. From next time onwards, 
			//we will shift the diagonal elements by adjusting multiplier to this number! 
			if (iterationCt == 1)
			{	maxJii = *(jac_temp);
				for (int i = 0; i < NoOfParams; i ++)
				{	if (*(jac_temp + i * (NoOfParams + 1)) > maxJii) maxJii = *(jac_temp + i * (NoOfParams + 1));
				}
				maxJii *= TauValue;
			}
		
			//Let us add maxJii to the diagonal element of JacT x Jac
			for (int i = 0; i < NoOfParams; i ++) *(jac_temp + i * (NoOfParams + 1)) += maxJii;
		

			//Invert the jac_temp matrix
			OperateMatrix(jac_temp, NoOfParams, 1, jac_inverse);
		
			//Now calculate JacT* K
			for (int j = 0; j < NoOfParams; j ++)
			{	for (int i = 0; i < TotalPts; i ++)
				{	*(jac_residual + j) += *(jacobian + i * NoOfParams + j) * *(residual + i);
				}
			}
		
			//  Jac_Res is vector g in LM pseudocode Lourakis document
			//''residual is Epsilon_p! p is param(); delta_P is deltaParam()
			//''Multiply the inverse with above vector to give correction to parameters


			//Multiply the inverse with above vector to give correction to parameters
			int S1[2] = {NoOfParams, NoOfParams};
			int S2[2] = {NoOfParams, 1};
			MultiplyMatrix(jac_inverse, &S1[0], jac_residual, &S2[0], deltaParam);
		
			//Update the values and continue iterations...
			delP_Size = 0;
			for (int j = 0; j < NoOfParams; j ++)
			{	*(param + j) += *(deltaParam + j);
				delP_Size += *(deltaParam + j) * *(deltaParam + j);
			}

			//Reset parameters that should not be updated! 
			if (DoNotUpdateIndex >= 0) *(param + DoNotUpdateIndex) = *(StartingGuess + DoNotUpdateIndex);
		
			//Check for constraints to see if we have convergence
			ContinueIterations = false;

			if (ConstrainAllParams)
			{	for (int i = 0; i < NoOfParams; i ++)
				{	if (abs(*(deltaParam + i)) > toleranceVal) 
					{	ContinueIterations = true; break;
					}
				}
			}
			else
			{	if (abs(*(deltaParam + ConstraintParamIndex)) > toleranceVal) ContinueIterations = true;
			}

			//Update rho and Mu and maxJii for next iteration
			double Lgrad = 0;
			for (int j = 0; j < NoOfParams; j ++) Lgrad += *(deltaParam + j) * (maxJii * *(deltaParam + j) + *(jac_residual + j));
					
			double Rho = (PrevRes_Size - Res_Size) / Lgrad;

			if (Rho > 0)
			{	double mJt = 1 - pow((2 * Rho - 1), 3);
				if (mJt < 0.3333333333333) mJt = 0.3333333333333333;
				maxJii = maxJii * mJt;	 Nu = 2;
			}
			else
			{	maxJii = maxJii * Nu ; Nu *= 2;
			}
		} //End of loop

			//Exited loop. So we got the answer. Copy to location and return true!
		if (Succeeded) memcpy((void*)Answer, (void*) bestparam, NoOfParams * sizeof(double));
		//free(param); free(jac_temp); free(residual); free(jacobian); free(jac_inverse); free(jac_residual); free(deltaParam);

		//following is to store the Res_Size in the last element of parameters being returned.
		//the caller will decide further actions based on value of Res_Size.
		if (Succeeded) 
		{
			if ((Function==Cone)||(Function==Circle_LMA)||(Function==Focus)||(Function==Conicoid)||
				(Function==Paraboloid)||(Function==Torus)||(Function==Cylinder)||(Function==ThreeDCircle)||
				(Function==TwoDConic)||(Function==TwoDParabola))
			{
				*(Answer + NoOfParams) = minResSize;
			}
		}
		free(bestparam);
		return Succeeded;
	}
	catch(...)
	{
		return false;
	}
}

void HelperMath::Allocate_LSBF_Memory()//(bool Resize)
{	//if (Resize)
	{	param = (double*) realloc(param, CurrentParameterNumber * sizeof(double));
		jacobian = (double*) realloc(jacobian, CurrentParameterNumber * CurrentPointsCt * sizeof(double));
		residual = (double*) realloc(residual, CurrentPointsCt * sizeof(double));
		jac_temp = (double*) realloc(jac_temp, CurrentParameterNumber * CurrentParameterNumber * sizeof(double) );
		jac_inverse = (double*) realloc(jac_inverse, CurrentParameterNumber * CurrentParameterNumber * sizeof(double));
		jac_residual = (double*) realloc(jac_residual, CurrentParameterNumber * sizeof(double));
		deltaParam = (double*) realloc(deltaParam, CurrentParameterNumber * sizeof(double));
	}
	/*else
	{
		param = (double*) calloc(CurrentParameterNumber, sizeof(double));
		jacobian = (double*) calloc(CurrentParameterNumber * CurrentPointsCt, sizeof(double));
		residual = (double*) calloc(CurrentPointsCt, sizeof(double));
		jac_temp = (double*) calloc(CurrentParameterNumber * CurrentParameterNumber, sizeof(double) );
		jac_inverse = (double*) calloc(CurrentParameterNumber * CurrentParameterNumber, sizeof(double));
		jac_residual = (double*) calloc(CurrentParameterNumber, sizeof(double));
		deltaParam = (double*) calloc(CurrentParameterNumber, sizeof(double));
	}*/

	ZeroMemory(param, sizeof(double) * CurrentParameterNumber);
	ZeroMemory(jacobian, sizeof(double) * CurrentParameterNumber * CurrentPointsCt);
	ZeroMemory(residual, sizeof(double) * CurrentPointsCt);
	ZeroMemory(jac_temp, sizeof(double) * CurrentParameterNumber * CurrentParameterNumber);
	ZeroMemory(jac_inverse, sizeof(double) * CurrentParameterNumber * CurrentParameterNumber);
	ZeroMemory(jac_residual, sizeof(double) * CurrentParameterNumber);
	ZeroMemory(deltaParam, sizeof(double) * CurrentParameterNumber);

}


void HelperMath::Calculate_CG(BYTE* image, int width, int height, int *x_CG, int *y_CG, double* Orientation)
{
	int jj = 0;
	double M_x = 0, M_y = 0, tot = 0; 
	for (int j = 0; j < height; j++)
	{ 
		for (int i = 0; i < width; i++)
		{
			M_x += image[jj] * i;
			M_y += image[jj] * j;
			tot += image[jj];
			jj++;
		}
	}
	*x_CG = int(M_x / tot);
	*y_CG = int(M_y / tot); //height -
	//WE compute the eigen values and vectors and calculate the orientation of the object in the image.
	//the matrix whose eigenvalues we need is [x^2 x*y; x*y y^2]. 
	double X_sq_Sum = 0, Y_sq_Sum = 0, X_Y_Sum = 0;
	int ti = 0, tj = 0; 
	jj = 0;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (image[jj] > 0)
			{
				ti = i - *x_CG;
				tj = j - *y_CG;
				X_sq_Sum += ti * ti; //x^2
				Y_sq_Sum += tj * tj; //y^2
				X_Y_Sum += ti * tj; //x * y
			}
			jj++;
		}
	}
	double eigenValues[2] = { 0,0 };
	double b = X_sq_Sum + Y_sq_Sum;
	//solve the characteristic equation. Since our matrix is guaranteed to have real eigenvalues
	//Since its symmetric and real, we are guaranteed real roots to the eigen problem and hence we will ALWAYS get
	//orientation with this method. the angular match will be quite exact, without any iterative processes!
	double realroottester_ref = b * b - 4 * (X_sq_Sum * Y_sq_Sum - X_Y_Sum * X_Y_Sum);
	double temptester = sqrt(realroottester_ref);
	eigenValues[0] = (b + temptester) / 2;
	eigenValues[1] = (b - temptester) / 2;
	//Calculate the slopes. We return purely the slopes from y = mx formula here, we get 
	//orientation in -90 to +90 deg form. 
	// To DO: Make this angle 0-360 degrees. We may need to to eigenvector analysis and understand how to do this. 
	//with asymmetric components. Drills are symmetric at 180 degrees rotation so this will work out very well for those. 
	//by RAN 09 Jul 2016. 
	if (eigenValues[0] > eigenValues[1])
		*Orientation = atan((eigenValues[0] - X_sq_Sum) / X_Y_Sum);
	else
		*Orientation = atan((eigenValues[1] - X_sq_Sum) / X_Y_Sum);
	////To get to 360 degree basis, we get the eigen vector (solution of x and y for largest eigenvalue) and see its orientation wrt to CG
	//double x_e = 0, y_e = 0;
	//double e_large = eigenValues[0];
	//if (eigenValues[1] > eigenValues[0])
	//	e_large = eigenValues[1];
	//y_e = (Y_sq_Sum - e_large) * (X_sq_Sum - e_large) / (X_Y_Sum * X_Y_Sum);
	//x_e = -(Y_sq_Sum - e_large) / X_Y_Sum;
	//if (x_e > *x_CG)
	//{
	//	if (y_e < *y_CG)
	//		*Orientation = 2 * M_PI - *Orientation;
	//}
	//else
	//{
	//	*Orientation += M_PI;
	//}
}

bool HelperMath::Get_Eigen_2D(double* points, int ptsCt, double* eigenValues, double* eigenSlopes)
{
	try
	{
		//First let us calculate the centroid of the points given. 
		double xCG = 0, yCG = 0, total = 0;
		for (int j = 0; j < ptsCt; j++)
		{
			xCG += points[2 * j]; yCG += points[2 * j + 1];
		}
		xCG /= ptsCt; yCG /= ptsCt;
		double X_sq_Sum = 0, Y_sq_Sum = 0, X_Y_Sum = 0;
		double ti = 0, tj = 0;

		for (int j = 0; j < ptsCt; j++)
		{
			ti = points[2 * j];// -xCG;
			tj = points[2 * j + 1];// -yCG;
			X_sq_Sum += ti * ti; //x^2
			Y_sq_Sum += tj * tj; //y^2
			X_Y_Sum += ti * tj; //x * y
		}
		//double eigenValues[2] = { 0,0 };
		double b = X_sq_Sum + Y_sq_Sum;
		//solve the characteristic equation. Since our matrix is guaranteed to have real eigenvalues
		//Since its symmetric and real, we are guaranteed real roots to the eigen problem and hence we will ALWAYS get
		//orientation with this method. the angular match will be quite exact, without any iterative processes!
		double realroottester_ref = b * b - 4 * (X_sq_Sum * Y_sq_Sum - X_Y_Sum * X_Y_Sum);
		double temptester = sqrt(realroottester_ref);
		eigenValues[0] = (b + temptester) / 2;
		eigenValues[1] = (b - temptester) / 2;
		eigenSlopes[0] = atan((eigenValues[0] - X_sq_Sum) / X_Y_Sum);
		eigenSlopes[1] = atan((eigenValues[1] - X_sq_Sum) / X_Y_Sum);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

//================  End Helper Functions ================
//=======================================================
