# include "stdafx.h"
#include "IGESWriter.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PointCollection.h"

//Constructor ..//
IGESWriter::IGESWriter()
{
	dstr1 = "     110", dstr2 = "     120", dstr3 = "     124", dstr4 = "     142", dstr5 = "     144", dstr6 = "       1", dstr13 ="     102",
	dstr7 = "       0", dstr8 = "00010001D", dstr9 = "               0D", dstr10 = "     100", dstr11 = "00000001D", dstr12 ="     122", dstr13 = "     116";
}

//Destructor...//
IGESWriter::~IGESWriter()
{
}


//Add cylinder parameter........
void IGESWriter::addCylinderParameter(double X1, double Y1, double Z1, double X2, double Y2, double Z2, double r1, double r2)
{
	try
	{  
		std::string rev1, rev2, parametric1, trimmed1;
        directrix[0] = X1, directrix[1] = Y1, directrix[2] = Z1, directrix[3] = X2, directrix[4] = Y2, directrix[5] = Z2;
        getgeneratrix( X1, Y1, Z1, X2, Y2, Z2, r1, r2);
        getstring110(directrix, 6);
        rev1 = rev;
        getstring110(generatrix1, 6);
        rev2 = rev;
		getstring120(rev1, rev2);
		parametric1 = parametric;
		getstring142(parametric1, parametric1);
		trimmed1 = trimmed;
		getstring144(parametric1, trimmed1);
		get_upper_disc();
		get_lower_disc();
	}

	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0001", __FILE__, __FUNCSIG__); }
}

// Add cone parameter...........
void IGESWriter::addConeParameter(double X1, double Y1, double Z1, double X2, double Y2, double Z2, double r1, double r2)
{
	try
	{
		std::string rev1, rev2, parametric1, trimmed1;
        directrix[0] = X1, directrix[1] = Y1, directrix[2] = Z1, directrix[3] = X2, directrix[4] = Y2, directrix[5] = Z2;
        getgeneratrix( X1, Y1, Z1, X2, Y2, Z2, r1, r2);
        getstring110(directrix, 6);
        rev1 = rev;
        getstring110(generatrix1, 6);
        rev2 = rev;
		getstring120(rev1, rev2);
		parametric1 = parametric;
		getstring142(parametric1, parametric1);
		trimmed1 = trimmed;
		getstring144(parametric1, trimmed1);
		get_upper_disc();
		get_lower_disc();
	}

	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0002", __FILE__, __FUNCSIG__); }
}

void IGESWriter::addSphereParameter(double ax, double bx, double cx, double r)
{
	try
	{
		double X1, X2, Y1, Y2, Z1, Z2;
		std::string rev1, rev2, parametric1, trimmed1, trans1;
		X1 = ax, Y1 = bx, Z1 = cx + r, X2 = ax, Y2 = bx, Z2 = cx - r;
		directrix[0] = X1, directrix[1] = Y1, directrix[2] = Z1, directrix[3] = X2, directrix[4] = Y2, directrix[5] = Z2;
		getstring110(directrix, 6);
        rev1 = rev;
		getstring124(ax, bx, cx);
		trans1 = trans;
		get_arc(r, trans1);
		rev2 = rev;
		getstring120(rev1, rev2);
		parametric1 = parametric;
		getstring142(parametric1, parametric1);
		trimmed1 = trimmed;
		getstring144(parametric1, trimmed1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0003", __FILE__, __FUNCSIG__); }
}

void IGESWriter::addPlaneParameter(PointCollection* PolygonPts,  double X1, double Y1, double Z1, double X2, double Y2, double Z2, double X3, double Y3, double Z3)
{
	try
	{
		 std::string rev1, parametric1, parametric2, trimmed1;
		 std::list<std::string> curve1;
		 double* x = NULL;
		 int n = PolygonPts->Pointscount();
		 x = (double*) malloc(n*sizeof(double)*3);
		 int l = 0;
		 directrix[0] = X1, directrix[1] = Y1, directrix[2] = Z1, directrix[3] = X2, directrix[4] = Y2, directrix[5] = Z2;
         getstring110(directrix, 6);
         rev1 = rev;
		 getstring122(rev1, X3, Y3, Z3);
		 parametric1 = parametric;
		 for (int i = 0; i < n; i++)
		 {
			 *(x + l) = PolygonPts->getList()[i]->X; l++;
			 *(x + l) = PolygonPts->getList()[i]->Y; l++;
			 *(x + l) = PolygonPts->getList()[i]->Z; l++;
		 }
		 for(int k = 0; k < n; k++)
		 {
			 if(l > 3*k + 5)
			 {
				 directrix[0] = *(x + 3*k);
				 directrix[1] = *(x + 3*k + 1);
				 directrix[2] = *(x + 3*k + 2);
				 directrix[3] = *(x + 3*k + 3);
				 directrix[4] = *(x + 3*k + 4);
				 directrix[5] = *(x + 3*k + 5);
			 }
			 else
			 {
				 directrix[0] = *(x + 3*k);
				 directrix[1] = *(x + 3*k + 1);
				 directrix[2] = *(x + 3*k + 2);
				 directrix[3] = *(x + 0);
				 directrix[4] = *(x + 1);
				 directrix[5] = *(x + 2);
			 }
			 getstring110(directrix, 6);
             curve1.push_back (rev);
		  }
		 free(x);
		 x = NULL;
		 getstring102(curve1);
		 parametric2 = parametric;
		 getstring142(parametric1, parametric2);
		 trimmed1 = trimmed;
		 getstring144(parametric1, trimmed1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0004", __FILE__, __FUNCSIG__); }
}

// get string for curve on  parametric surface entity........
	void IGESWriter::getstring102(std::list<std::string> S1)
	{
		try
		{
			std::list<std::string>::iterator itr;
			std::list<std::string> temp, temp1;
			std::string S2 = "", S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17;
		    int i, j ,k, l, n;
			i = S1.size();
			char test4[4];
	    	_itoa(i, test4, 10);
	    	S17 = (const char*)(test4);
			for(itr = S1.begin(); itr != S1.end(); itr++)
			{
				S2 = S2 + "," + *itr;
			}
			S3 = "102," + S17 + S2 +";";
			n = S3.length();
			if(n <= 64)
		    {
				postspace(&S4, S3, 64);
			    j = PElist.size() + 1;
			    char test[4];
		        _itoa(j, test, 10);
		        S5 = (const char*)(test);
				k = DElist.size() + 1;
	     		char test1[4];
	    		_itoa(k, test1, 10);
	    		S6 = (const char*)(test1);
				parametric = S6;
				S7 = S6 + "P";
				prespace(&S8, S7, 9);
				prespace(&S9, S5, 7);
				S10 = S4 + S8 + S9;
				PElist.push_back(S10);
				prespace(&S11, S5, 8);
				prespace(&S12, S6, 7);
				S13 = dstr13 + S11 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + S12;
				DElist.push_back(S13);
				l = k + 1;
				char test2[4];
	    		_itoa(l, test2, 10);
	    		S14 = (const char*)(test2);
				prespace(&S15, S14, 7);
				S16 = dstr13 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S15;
				DElist.push_back(S16);
			}
			else if(n <= 128)
			{
				std::string nline = "       2";
				split_string(&S4, &S5, S3);
			    std::string mystr3, mystr4, mystr5, mystr6, mystr7, mystr8;
				j = PElist.size() + 1;
				char test[4];
				_itoa(j, test, 10);
				S6 = (const char*)(test);
				k = DElist.size() + 1;
	     		char test1[4];
	    		_itoa(k, test1, 10);
	    		S7 = (const char*)(test1);
				S8 = S7 + "P";
				parametric = S8;
				postspace(&S9, S4, 64);
				prespace(&S10, S8, 9);
				prespace(&S11, S6, 7);
				mystr3 = S9 + S10 + S11;
				PElist.push_back(mystr3);
				l = j + 1;
				char test2[4];
	    		_itoa(l, test2, 10);
	    		S12 = (const char*)(test2);
				postspace(&S13, S5, 64);
				prespace(&S14, S12, 7);
				mystr4 = S13 + S10 + S14;
				PElist.push_back(mystr4);
				int m = k + 1;
				char test3[4];
	    		_itoa(m, test3, 10);
	    		S15 = (const char*)(test3);
				prespace(&S16, S6, 8);
				prespace(&mystr5, S7, 7);
				mystr6 = dstr13 + S16 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + mystr5;
				DElist.push_back(mystr6);
				prespace(&mystr7, S15, 7);
				mystr8 = dstr13 + dstr7 + dstr7 + nline + dstr7 + dstr7 + dstr7 + dstr9 + mystr7;
				DElist.push_back(mystr8);		
			}
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0005", __FILE__, __FUNCSIG__); }
}

 // get string for tabulated cylinder entity.......
	void IGESWriter::getstring122(std::string S1, double X1, double Y1, double Z1)
{
	try
	{
		std::string S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17, S18;
		int i, j ,k, l, n;
		S2 = DoubletoString(X1);
		S3 = DoubletoString(Y1);
		S4 = DoubletoString(Z1);
		S5 = "122," + S1 + "," + S2 + "," + S3 + "," + S4 +";";
		postspace(&S6, S5, 64);
		i = PElist.size() + 1;
		char test[4];
	    _itoa(i, test, 10);
	    S7 = (const char*)(test);
		j = DElist.size() + 1;
	   	char test1[4];
	   	_itoa(j, test1, 10);
	   	S8 = (const char*)(test1);
		parametric = S8;
		S9 = S8 + "P";
		prespace(&S10, S9, 9);
		prespace(&S11, S7, 7);
		S12 = S6 + S10 + S11;
		PElist.push_back(S12);
		prespace(&S13, S7, 8);
		prespace(&S14, S8, 7);
		S15 = dstr12 + S13 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + S14;
		DElist.push_back(S15);
		k = j + 1;
		char test2[4];
	   	_itoa(k, test2, 10);
	   	S16 = (const char*)(test2);
		prespace(&S17, S16, 7);
		S18 = dstr12 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S17;
		DElist.push_back(S18);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0006", __FILE__, __FUNCSIG__); }
}


//Add 3D Line parameter........
	void IGESWriter::addLine3DParameter(double X1, double Y1, double Z1, double X2, double Y2, double Z2)
{
	try
	{
		std::string rev1, rev2, parametric1, trimmed1;
		 directrix[0] = X1, directrix[1] = Y1, directrix[2] = Z1, directrix[3] = X2, directrix[4] = Y2, directrix[5] = Z2;
         getstring110(directrix, 6);
         rev1 = rev;
		 getstring122(rev1, X2, Y2, Z2);
		 parametric1 = parametric;
		 getstring142(parametric1, parametric1);
		 trimmed1 = trimmed;
		 getstring144(parametric1, trimmed1);

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0007", __FILE__, __FUNCSIG__); }
}

	// Add point parameter.......
void IGESWriter::addPointparameter(double X1, double Y1, double Z1)
{
	try
    {
		getstring116(X1, Y1, Z1);

    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0008", __FILE__, __FUNCSIG__); }
}

//get parameter and directory list for point entity.....
void IGESWriter::getstring116(double X1, double Y1, double Z1)
{
	try
	{
		std::string S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17;
		int i, j ,k, l, n;
		S1 = DoubletoString(X1);
		S2 = DoubletoString(Y1);
		S3 = DoubletoString(Z1);
		S4 = "116," + S1 + "," + S2 + "," + S3 + ",0;";
		n = S4.length();
		if(n <= 64)
		{
			postspace(&S5, S4, 64);
			i = PElist.size() + 1;
			char test[4];
		    _itoa(i, test, 10);
		    S6 = (const char*)(test);
			j = DElist.size() + 1;
			char test1[4];
	    	_itoa(j, test1, 10);
	    	S7 = (const char*)(test1);
			S8 = S7 + "P";
			prespace(&S9, S8, 9);
			prespace(&S10, S6, 7);
			S11 = S5 + S9 + S10;
			PElist.push_back(S11);
			prespace(&S12, S6, 8);
			prespace(&S13, S7, 7);
			S14 = dstr13 + S12 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + S13;
			DElist.push_back(S14);
			k = j + 1;
			char test2[4];
	    	_itoa(k, test2, 10);
	    	S15 = (const char*)(test2);
			prespace(&S16, S15, 7);
			S17 = dstr13 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S16;
			DElist.push_back(S17);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0009", __FILE__, __FUNCSIG__); }
}
void IGESWriter::getstring124(double x, double y, double z)
{
	try
	{
		std::string S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17;
		int i, j ,k, l, n;
		S1 = DoubletoString(x);
		S2 = DoubletoString(y);
		S3 = DoubletoString(z);
		S4 = "124,0.0,1.0,0.0," + S1 +",0.0,0.0,1.0," + S2 +",1.0,0.0,0.0," + S3 + ";";
		n = S4.length();
		if(n <= 64)
		{
			postspace(&S5, S4, 64);
			i = PElist.size() + 1;
			char test[4];
		    _itoa(i, test, 10);
		    S6 = (const char*)(test);
			j = DElist.size() + 1;
	     	char test1[4];
	    	_itoa(j, test1, 10);
	    	S7 = (const char*)(test1);
			trans = S7;
			S8 = S7 + "P";
			prespace(&S9, S8, 9);
			prespace(&S10, S6, 7);
			S11 = S5 + S9 + S10;
			PElist.push_back(S11);
			prespace(&S12, S6, 8);
			prespace(&S13, S7, 7);
			S14 = dstr3 + S12 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + S13;
			DElist.push_back(S14);
			k = j + 1;
			char test2[4];
	    	_itoa(k, test2, 10);
	    	S15 = (const char*)(test2);
			prespace(&S16, S15, 7);
			S17 = dstr3 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S16;
			DElist.push_back(S17);
		}
		else if(n <= 128)
		{
			std::string nline = "       2";
			split_string(&S5, &S6, S4);
			std::string mystr3, mystr4, mystr5, mystr6, mystr7, mystr8;
			i = PElist.size() + 1;
			char test[4];
		    _itoa(i, test, 10);
		    S7 = (const char*)(test);
			j = DElist.size() + 1;
	     	char test1[4];
	    	_itoa(j, test1, 10);
	    	S8 = (const char*)(test1);
			S9 = S8 + "P";
			trans = S8;
			postspace(&S10, S5, 64);
			prespace(&S11, S9, 9);
			prespace(&S12, S7, 7);
			mystr3 = S10 + S11 + S12;
			PElist.push_back(mystr3);
			k = i + 1;
			char test2[4];
	    	_itoa(k, test2, 10);
	    	S13 = (const char*)(test2);
			postspace(&S14, S6, 64);
			prespace(&S15, S13, 7);
			mystr4 = S14 + S11 + S15;
			PElist.push_back(mystr4);
			int m = j + 1;
			char test3[4];
	    	_itoa(m, test3, 10);
	    	S16 = (const char*)(test3);
			prespace(&S17, S7, 8);
			prespace(&mystr5, S8, 7);
			mystr6 = dstr3 + S17 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + mystr5;
			DElist.push_back(mystr6);
			prespace(&mystr7, S16, 7);
			mystr8 = dstr3 + dstr7 + dstr7 + nline + dstr7 + dstr7 + dstr7 + dstr9 + mystr7;
			DElist.push_back(mystr8);		
		}		
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0010", __FILE__, __FUNCSIG__); }
}

//get generatrix parameter....for cylinder and cone.....//
void IGESWriter::getgeneratrix(double X1, double Y1, double Z1, double X2, double Y2, double Z2, double r1, double r2)
{
	try
	{
		double ddr[3] = {X1- X2, Y1 - Y2, Z1 - Z2}, dcp[3];
		double ddc[3], dcx[3] = {1, 0, 0}, dcy[3] = {0, 1, 0};
		RMATH3DOBJECT->DirectionCosines(ddr, ddc);
		if(!RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(ddc, dcx, dcp))
		{
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(ddc, dcy, dcp);
		}
		
		generatrix1[0] = X1 + dcp[0] * r1, generatrix1[1] = Y1 + dcp[1] * r1, generatrix1[2] = Z1 + dcp[2] * r1, generatrix1[3] = X2 + dcp[0] * r2,
		generatrix1[4] = Y2 + dcp[1] * r2, generatrix1[5] = Z2 + dcp[2] * r2;
		generatrix2[0] = X1 - dcp[0] * r1, generatrix2[1] = Y1 - dcp[1] * r1, generatrix2[2] = Z1 - dcp[2] * r1, generatrix2[3] = X2 - dcp[0] * r2,
		generatrix2[4] = Y2 - dcp[1] * r2, generatrix2[5] = Z2 - dcp[2] * r2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0011", __FILE__, __FUNCSIG__); }
}

void IGESWriter::get_arc(double r, std::string trans2)
{
	try
	{
	    std::string S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17;
		int i, j ,k, l, n;
		S1 = DoubletoString(r);
		S2 = "100,0.,0.,0.,-" + S1 + ",0.," + S1 +  +",0.;";
		i = PElist.size() + 1;
		char test[4];
	    _itoa(i, test, 10);
		S3 = (const char*)(test);
		j = DElist.size() + 1;
		char test1[4];
		_itoa(j, test1, 10);
		S4 = (const char*)(test1);
		S5 = S4 + "P";
		postspace(&S6, S2, 64);
		prespace(&S7, S5, 9);
		prespace(&S8, S3, 7);
		S9 = S6 + S7 + S8;
		PElist.push_back(S9);
		rev = S4;
		prespace(&S10, S3, 8);
	    prespace(&S11, S4, 7);
		prespace(&S12, trans2, 8);
		S13 = dstr10 + S10 + dstr7 + dstr7 + dstr7 + dstr7 + S12 + dstr7 + dstr8 + S11;
		DElist.push_back(S13);
		k = j + 1;
		char test2[4];
		_itoa(k, test2, 10);
		S14 = (const char*)(test2);
		prespace(&S15, S14, 7);
		S16 = dstr10 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S15;
		DElist.push_back(S16);
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0012", __FILE__, __FUNCSIG__); }
}

// get parameter and directory list...... 
void IGESWriter::getstring110(double dbl[], int size)
{
	try
	{
		std::string dstr, str, str1, str2, str3, str4, str5, str6;
		int n, i, k;
	    getstring(&str, dbl, size);
	    dstr = "110," + str;
		n = (int)dstr.size();
		if(n <= 64)
		{
			postspace(&str1, dstr, 64);
			i = PElist.size();
		    if(i == 0)
			{		
				str2 = str1 + "       1P" + "      1";
				rev = "1";
				PElist.push_back(str2);
				str3 = dstr1 + dstr6 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + "      1";
				DElist.push_back(str3);
				str4 = dstr1 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + "      2";
				DElist.push_back(str4);
			}
			else
			{
				int l, m, l1, m1;
				std::string mystr3, mystr4, mystr5, mystr6, mystr7;
				// k is the sequence no of PE section......
				k = i + 1;
				// convert integer to string.......
			    char* test1 = new char[4];
				_itoa(k, test1, 10);
				string mystr  = (const char*)(test1);
				// m is the sequence no. of DE section.......
				l = DElist.size();
				m = l + 1;
				char* test2 = new char[4];
				_itoa(m, test2, 10);
				string mystr1  = (const char*)(test2);
				postspace(&str1, dstr, 64);
				//create DE pointer in PE section.....
				str2 = mystr1 + "P";
				// get generatrix DE no. for revolution entity........
				rev = mystr1;
				prespace(&str3, mystr, 7);
				prespace(&str4, str2, 9);
				str5 = str1 + str4 + str3;
				PElist.push_back(str5);
				// l1 is the next seqno. of DE section.......
				l1 = m + 1;
				char* test3 = new char[4];
				_itoa(l1, test3, 10);
				string mystr2  = (const char*)(test3);
				prespace(&mystr6, mystr1, 7);
				prespace(&mystr7, mystr, 8);
				//create string for DE section........
				mystr3 = dstr1 + mystr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 +dstr8 + mystr6;
				DElist.push_back(mystr3);
				prespace(&mystr4, mystr2, 7);
				mystr5 = dstr1 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + mystr4;
				DElist.push_back(mystr5);

			}
		}
		else if(n <= 128)
		{
			std::string nline = "       2";
			split_string(&str1, &str2, dstr);
			i = PElist.size();
			if(i == 0)
			{
				std::string S1, S2, S3, S4, S5;
				postspace(&str3, str1, 64);
				str4 = str3 + "       1P" + "      1";
				rev = "1";
				PElist.push_back(str4);
				postspace(&str5, str2, 64);
				str6 = str5 + "       1P" + "      2";
				PElist.push_back(str6);
				S1 = dstr1 + dstr6 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + "      1";
				DElist.push_back(S1);
				S2 = dstr1 + dstr7 + dstr7 + nline + dstr7 + dstr7 + dstr7 + dstr9 + "      2";
				DElist.push_back(S2);
			}
			else
			{
				int l, m, l1, m1;
				std::string mystr3, mystr4, mystr5, mystr6, mystr7, S1, S2, S3, S4, S5, S6, S7;
				// k is the sequence no of PE section......
				k = i + 1;
				// convert integer to string.......
			    char test1[4];
				_itoa(k, test1, 10);
				string mystr  = (const char*)(test1);
				// m is the sequence no. of DE section.......
				l = DElist.size();
				m = l + 1;
				char test2[4];
				_itoa(m, test2, 10);
				string mystr1  = (const char*)(test2);
				//create DE pointer in PE section.....
				mystr6 = mystr1 + "P";
				// get generatrix DE no. for revolution entity........
				rev = mystr1;
				postspace(&str3, str1, 64);
				prespace(&str4, mystr6, 9);
				prespace(&str5, mystr, 7);
				mystr3 = str3 + str4 + str5;
				PElist.push_back(mystr3);
				l1 = k + 1;
				char test3[4];
				_itoa(l1, test3, 10);
				string mystr2  = (const char*)(test3);
				prespace(&mystr4, mystr2, 7);
				postspace(&mystr5, str2, 64);
				prespace(&S5, mystr6, 9);
				mystr7 = mystr5 + S5 + mystr4;
				PElist.push_back(mystr7);
				// mystr1 is the sequence no. of DE section....
				prespace(&S1, mystr1, 7);
				prespace(&S7, mystr, 8);
				S2 = dstr1 + S7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + S1;
				DElist.push_back(S2);
				m1 = m + 1;
				char test4[4];
				_itoa(m1, test4, 10);
				S3 = (const char*)(test4);
				prespace(&S6, S3, 7);
				S4 = dstr1 + dstr7 + dstr7 + nline + dstr7 + dstr7 + dstr7 + dstr9 + S6;
				DElist.push_back(S4);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0013", __FILE__, __FUNCSIG__); }
}

//get string for surace of revolution.........
void IGESWriter::getstring120(std::string S1, std::string S2)
{
	try
	{
		std::string S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16;
		int i, j, k, l;
		S3 = "120," + S1 + "," + S2 + "," + "0." + "," + "6.28318530717959;";
		postspace(&S4, S3, 64);
		i = PElist.size();
		j = i + 1;
		char test1[4];
		_itoa(j, test1, 10);
		 S5 = (const char*)(test1);
		 prespace(&S6, S5, 7);
		 k = ((DElist.size()) + 1);
		 char test2[4];
		_itoa(k, test2, 10);
		 S7 = (const char*)(test2);
		 S8 = S7 + "P";
		 parametric = S7;
		 prespace(&S9, S8, 9);
		 S10 = S4 + S9 + S6;
		 PElist.push_back(S10);
		 prespace(&S11, S5, 8);
		 prespace(&S12, S7, 7);
		 l = k + 1;
		 char test3[4];
		_itoa(l, test3, 10);
		 S13 = (const char*)(test3);
		 S14 = dstr2 + S11 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + S12;
		 DElist.push_back(S14);
		 prespace(&S15, S13, 7);
		 S16 = dstr2 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S15;
		 DElist.push_back(S16);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0014", __FILE__, __FUNCSIG__); }
}

// get string for parametric curve surface entity........
void IGESWriter::getstring142(std::string S1, std::string para_Surface)
{
	try
	{
		std::string S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15;
		int i, j , k;
		S2 = "142,1," + S1 + "," + "0" + "," + para_Surface + "," +"2;";
		postspace(&S3, S2, 64);
		i = PElist.size() + 1;
		char test1[4];
		_itoa(i, test1, 10);
		S4 = (const char*)(test1);
		prespace(&S5, S4, 7);
		j = DElist.size() + 1;
		char test2[4];
		_itoa(j, test2, 10);
		S6 = (const char*)(test2);
		S7 = S6 + "P";
		trimmed = S6;
		prespace(&S8, S7, 9);
		S9 = S3 + S8 + S5;
		PElist.push_back(S9);
		prespace(&S10, S4, 8);
		prespace(&S11, S6, 7);
		S12 = dstr4 + S10 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr8 + S11;
		DElist.push_back(S12);
		k = j + 1;
		char test3[4];
		_itoa(k, test3, 10);
		S13 = (const char*)(test3);
		prespace(&S14, S13, 7);
		S15 = dstr4 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S14;
		DElist.push_back(S15);
	}
catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0015", __FILE__, __FUNCSIG__); }
}

	// get string for trimmed curve surface entity.........
	void IGESWriter::getstring144(std::string S1, std::string S2)
	{
		try
		{
			std::string S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16;
			int i, j , k;
			S3 = "144," + S1 + ",1,0," + S2 +";";
			postspace(&S4, S3, 64);
			i = DElist.size() + 1;
			char test1[4];
		    _itoa(i, test1, 10);
		    S5 = (const char*)(test1);
			S6 = S5 + "P";
			j = PElist.size() + 1;
			char test2[4];
		    _itoa(j, test2, 10);
		    S7 = (const char*)(test2);
			prespace(&S8, S6, 9);
			prespace(&S9, S7, 7);
			S10 = S4 + S8 + S9;
			PElist.push_back(S10);
			prespace(&S11, S7, 8);
			prespace(&S12, S5, 7);
			S13 = dstr5 + S11 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr7 + dstr11 +S12;
			DElist.push_back(S13);
			k = i + 1;
			char test3[4];
		    _itoa(k, test3, 10);
		    S14 = (const char*)(test3);
			prespace(&S15, S14, 7);
			S16 = dstr5 + dstr7 + dstr7 + dstr6 + dstr7 + dstr7 + dstr7 + dstr9 + S15;
			DElist.push_back(S16);
        }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0016", __FILE__, __FUNCSIG__); }
}
	// get upper disc of cylinder.........
	void IGESWriter::get_upper_disc()
	{  
	try
	{
		std::string rev1, rev2, parametric1, trimmed1;
		double generatrix3[6];
		getstring110(directrix, 6);
        rev1 = rev;
		generatrix3[0] = directrix[0], generatrix3[1] = directrix[1], generatrix3[2] = directrix[2], generatrix3[3] = generatrix1[0], generatrix3[4] = generatrix1[1], generatrix3[5] = generatrix1[2];
        getstring110(generatrix3, 6);
        rev2 = rev;
		getstring120(rev1, rev2);
		parametric1 = parametric;
		getstring142(parametric1, parametric1);
		trimmed1 = trimmed;
		getstring144(parametric1, trimmed1);
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0017", __FILE__, __FUNCSIG__); }
	}

// get lower disc of cylinder......
void IGESWriter::get_lower_disc()
{
	try
	{
		std::string rev1, rev2, parametric1, trimmed1;
		double generatrix4[6];
		getstring110(directrix, 6);
        rev1 = rev;
		generatrix4[0] = directrix[3], generatrix4[1] = directrix[4], generatrix4[2] = directrix[5], generatrix4[3] = generatrix1[3], generatrix4[4] = generatrix1[4], generatrix4[5] = generatrix1[5];
        getstring110(generatrix4, 6);
        rev2 = rev;
		getstring120(rev1, rev2);
		parametric1 = parametric;
		getstring142(parametric1, parametric1);
		trimmed1 = trimmed;
		getstring144(parametric1, trimmed1);

    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0018", __FILE__, __FUNCSIG__); }
}

// split string into two parts.........for PE section......
void IGESWriter::split_string(std::string *result1, std::string *result2, std::string inpstr)
{
	try 
	{
		int i;
		std::string S1 = "";
		*result1 = inpstr.substr(0,64);
		*result2 = inpstr.substr(64);
		for(i = 63; i!= 0; i--)
		{
			S1 = result1->substr(i,1);
			if(S1 == ",")
			{
				*result1 = result1->substr(0, i + 1);
				break;
			}
			*result2 = S1 + *result2;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0019", __FILE__, __FUNCSIG__); }
}

void IGESWriter::getstring(std::string *result, double xyz[], int size)
{
	try
	{
		*result = "";
		int i;
		for(i = 0;i!= size;i++)
		{
			if(i!= size - 1)
			{
			*result = *result + DoubletoString(xyz[i]) +",";
			}
			else if(i == size - 1)
			{
				*result = *result + DoubletoString(xyz[i]);
			}
		}
		*result = *result + ";";
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0020", __FILE__, __FUNCSIG__); }
}
//Convert double to string...//
char* IGESWriter::DoubletoString(double x)
{
	try
	{
		if(abs(x) > 100000)
			return "0.0";
		RMATH2DOBJECT->Double2String(x, 5, cd, false); 
		return cd;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0021", __FILE__, __FUNCSIG__); return cd; }
}
// add spaces at the end of string.....
void IGESWriter::postspace(std::string *result, std::string inpstr, int size)
{
	try
	{
		*result = inpstr;
		int n = inpstr.length();
		for(int i = n; i < size; i++)
			*result = *result + " ";
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0022", __FILE__, __FUNCSIG__);  }
}

//add space at start of string.........
void IGESWriter::prespace(std::string *result, std::string inpstr, int size)
{
	try
	{
		*result = inpstr;
		int n = inpstr.length();
		for(int i = n; i < size; i++)
			*result = " " + *result;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESW0023", __FILE__, __FUNCSIG__);  }
}
//Generate the dxf file///
bool IGESWriter::generateDXFFile(char* filename)
{
	try
	{
		std::string S1, S2, S3, S4, S5, S6, S7, S8, S9, S10;
		std::list<std::string> SGlist;
		int i, j;
		i = DElist.size();
		char test3[4];
		_itoa(i, test3, 10);
		S5 = (const char*)(test3);
		S8 = S5 + "P";
		j = PElist.size();
		char test4[4];
		_itoa(j, test4, 10);
		 S6 = (const char*)(test4);
		 prespace(&S9, S6, 7);
		 prespace(&S10, S8, 8);
		 S7 = "S      1G      3D" + S10 + S9 +"                                        T      1";
	     S1 = "RAPID-I Customised Technologies Pvt. Ltd. Electronic City Phase-1 (BLR) S      1";
         S2 = "1H,,1H;,6HNoname,11Hexport0.igs,13HSpatial Corp.,20H3D InterOp ACIS/IGESG      1";
         S3 = ",32,38, 6,308,15,6HNoname,1.000,2,2HMM,1,1.000,15H20110301.095435,      G      2";
         S4 = "1.0e-006,0.00,6HNoname,6HNoname,11,0,15H20110301.095435;                G      3";
		 SGlist.push_back(S1);
		 SGlist.push_back(S2);
		 SGlist.push_back(S3);
		 SGlist.push_back(S4);
		 PElist.push_back(S7);
		 this->filename = filename;
		 DXFDataWriter.open(filename);
		 if(!DXFDataWriter)
			 return false;
		
		 for(SGlistIterator = SGlist.begin(); SGlistIterator != SGlist.end(); SGlistIterator++)
			 DXFDataWriter << *SGlistIterator << "\n";
		 for(DElistIterator = DElist.begin(); DElistIterator != DElist.end(); DElistIterator++)
			 DXFDataWriter << *DElistIterator << "\n";
		 for(PElistIterator = PElist.begin(); PElistIterator != PElist.end(); PElistIterator++)
			 DXFDataWriter << *PElistIterator << "\n";
		 
		 DXFDataWriter.close();
   		 return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGEW0024", __FILE__, __FUNCSIG__); return false; }
}