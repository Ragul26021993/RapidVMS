//Include required headerfiles...//
#include "stdafx.h"
#include "IGESReader.h"
#include "..\Engine\RCadApp.h"
#include "..\MainDLL.h"
#include "math.h"
#include "..\Shapes\ShapeHeaders.h"
#include "DXFExpose.h"
#include "..\Engine\PointCollection.h"

//Constructor..//
IGESReader::IGESReader()
{  
	ncount = 0;
	rapidi = false; alibre = false; param = false, cflag = false, cloudPointFlag = false;
	rap1 = 1; ali1 = 1; rap2 = 0; rap3 = 0, checkduplicate = 0, check102 = 0, checkduplicate116 = 0, checkduplicate120 = 0, multipleValue = 0;
	Allcloud_points = new PointCollection(); this->dxfShape = NULL;
}

//Destructor...//
IGESReader::~IGESReader()
{
	try
	{
		Allcloud_points->deleteAll();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0011", __FILE__, __FUNCSIG__); }
}

//Load the iges file.///
bool IGESReader::LoadFile(char* filename)
{
	try
	{
		IGESData.open(filename);	    
		if(!IGESData)
			return false;
		
		bool nflag = true;
		std::list<std::string>::iterator itr;
		std::list<std::string> temp;
		std::string str1;
		int n = -1, l = -1, g1 = -1, exit1 = -1;
		//Read till end of the file///
		while(!IGESData.eof())
		{
		    getline(IGESData,str1);
			exit1 = str1.substr(72).find("T");
			if(exit1 != -1)
			{
				break;
			}
			g1 =str1.substr(72).find("S");
			if(g1 != -1)
			{
			    mylist12.push_back (str1);
				if(mylist12.begin()->substr(0,7) == "RAPID-I")
				{
					rapidi = true;
				}
				else
				{
					alibre = true;
				}
			}
		    n = str1.substr(72).find("G");
		    l = str1.substr(72).find("P");
			if(n != -1)
			{
				mylist1.push_back(str1);
			    delimiter1 = mylist1.begin()->substr(2,1);
				rlimiter1 = mylist1.begin()->substr(6,1);
				std::string strg = "";
				for(itr = mylist1.begin(); itr != mylist1.end(); itr++)
				{
					strg = strg + *itr;
				}
				char * test = (char*)delimiter1.c_str ();
				split( &temp, strg, *test);
				std::list<std::string>::iterator itr1;
				int j = 0;
		        for(itr1 = temp.begin (); itr1 != temp.end (); itr1++)
				{
					unittype1 = *itr1;
					if(j == 14)
					{
						if(unittype1 == "1")
		                {
			                 multipleValue = 25.4;
		                }
		                else if(unittype1 == "10")
		                {
			                multipleValue = 10;
	                  	}
		                else if(unittype1 == "2")
		                {
			                multipleValue = 1;
		                }
      					break;
					}
					j++;
				}
			}
			else if(l != -1)
			{
				mylist.push_back(str1);
				
			}
		}
		
		IGESData.close ();
		getParameterData();	
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0001", __FILE__, __FUNCSIG__); }
}

//function to get parameter section data...//
void IGESReader::getParameterData()
{
	try
	{
		
		std::list<std::string>::iterator itr;
		std::list<std::string> temp, temp1;
		std::string str3, str2;
		bool CurveOnParametricFlag = false;
		int  m = -1, k = -1, r = -1, l = -1, n = -1, p = -1;
		for(itr = mylist.begin(); itr != mylist.end(); itr++)
				{
					templiststr.push_back(*itr);
					m = itr->substr(0,4).find("120,");
					l = itr->substr(0,4).find("122,");
					n = itr->substr(0,4).find("102,");
					p = itr->substr(0,4).find("116,");
					if(m != -1)
					{
						MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_m_If", __FUNCSIG__);
						//get generatrix and directrix for 120 entity........
						getGeneratrix_Directrix120(itr);

					}
				    if(rapidi)
					{
						if(l != -1)
						{
							MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_l_If", __FUNCSIG__);
							std::string str4, str5;					
							str4 = itr->substr(64).substr(0,8);
							removespace(&str5, str4);
							char * test11 = (char*)str5.c_str();
				             int duplicate = _atoi64(test11);
							 if (duplicate > checkduplicate)
							 {
							      //get generatrix and directrix for 122....
							      getGeneratrix_Directrix122(itr);
								  checkduplicate = duplicate;
							 }							
						 }
						else if(p != -1)
						{	
							MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_p_If", __FUNCSIG__);
							std::string str4, str5;					
							str4 = itr->substr(73);
							removespace(&str5, str4);
							char * test11 = (char*)str5.c_str();
				            int duplicate116 = _atoi64(test11);
						    if (duplicate116 > checkduplicate116)
							{
							     //get cloud point collection...
							     getAllCloudPoints_Collection(itr);
								 cloudPointFlag = true;
							     checkduplicate116 = duplicate116;								
							}			
							
						}
						if((n != -1) || (CurveOnParametricFlag))
						{
							MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_n_If", __FUNCSIG__);
							int i1 = -1, checkduplicate102;
							CurveOnParametricFlag = true;
							std::string duplicate_str1, duplicate_str2;
							i1 = itr->find(";");
							if (i1 != -1)
							{
								if(itr->substr(0,4) == "102,")
								{
									duplicate_str1 = itr->substr(73);
									removespace(&duplicate_str2, duplicate_str1);
									char * test11 = (char*)duplicate_str2.c_str();
									checkduplicate102 = _atoi64(test11);
								}							
								 if(checkduplicate102 > check102)
								 {
									 //get polygon points.....
									 getPolygon_Points(itr);
									 check102 = checkduplicate102;
									 CurveOnParametricFlag = false;
								 }
							}
						}
					}
				if(cflag == true)
				{
					std::list<std::string>::iterator itr4, itr5;
					
					for(itr5 = templiststr.begin(); itr5 != templiststr.end(); itr5++)
					{
						int i1 = -1, j1 = -1;
						i1 = itr5->find(liststr2);
						j1 = itr5->find(liststr1);
						std::string resultstr, resultstr1;
						//removespace(&str2,(itr->substr(65).substr(0,8)));
						if(i1 != -1)
						{
							k = itr5->find(rlimiter1);
							itr4 = --itr5;
								resultstr = *itr4;
								itr5 = ++itr5;
								if(itr4 != templiststr.begin())
								{
									if(itr4->substr(0,3) == "124")
									{
									}
									else if((--itr4)->substr(0,3) == "124")
									{
										resultstr1 = (itr4)->substr(0,64);
										itr4 = ++itr4;
										resultstr = resultstr1 + *itr4;
									}
								}
								std::list<std::string>::iterator itr3;
								itr3 = ++itr5;
								itr5 = --itr5;
								r = (itr5->substr(0,64) + (itr3->substr(0,64))).find(rlimiter1);
								if(k != -1)
								{
									MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_k_If", __FUNCSIG__);
									removespace(&str3,(itr5->substr(0,3)));
									if(str3 == "110")
									{
										MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_k_If_110", __FUNCSIG__);
										generatrix1 = *itr5;
										if(cflag)
										{
										   Cylinder_Coneparameter();
										   cflag = false;
										   if(param)
										   {
											  DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
											  param = false;
											  break;
										   }
										}
									 }
									else if(str3 =="100")
									{
										MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_k_If_100", __FUNCSIG__);
										tmatrix1 = resultstr;
										generatrix1 = *itr5;
										sphereflag = true;
										if(cflag)
										{
										   Sphereparameter();
										   cflag = false;
										   if(param)
										   {
											  DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
											  param = false;
											   break;
										   }
									   }
									}
								}
								else if(r != -1)
								{
									MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_r_If", __FUNCSIG__);
									 removespace(&str3,((itr5->substr(0,64) + itr3->substr(0,64))).substr(0,3));
									 if(str3 == "110")
									{
										 MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_r_If_110", __FUNCSIG__);
										generatrix1 = itr5->substr(0,64) + (itr3->substr(0,64));
									   // cylinderflag = true;
									
					             		if(cflag)
										{
											Cylinder_Coneparameter();
											cflag = false;
											if(param)
										   {
												DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
												// ncount = 0;
												param = false;
												 break;
										   }
										 }
									 }
									 else if(str3 =="100")
									 {
										 MAINDllOBJECT->SetAndRaiseErrorMessage("GetParameter", "Inside_r_If_100", __FUNCSIG__);
										tmatrix1 = resultstr;
										generatrix1 = itr5->substr(0,64) + (itr3->substr(0,64));
										sphereflag = true;
										if(cflag)
										{
											Sphereparameter();
											cflag = false;
											if(param)
											{
												DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
     											param = false;
												 break;
											}
										}
									}
								}
							}
							else if(j1 != -1)
							{
								 k = itr5->find(rlimiter1);
								 std::list<std::string>::iterator itr3;
								 itr3 = ++itr5;
								 itr5 = --itr5;
								 r = (itr5->substr(0,64) + (itr3->substr(0,64))).find(rlimiter1);
								 if((k != -1) && (ncount == 0))
								 {
									directrix1 = *itr5;
									ncount++;
								 }
								 else if((r != -1) && (ncount == 0))
								 {
									itr3 = ++itr5;
									itr5 = --itr5;
									directrix1 = itr5->substr(0,64) + (itr3->substr(0,64));
									ncount++;
								}
							}
						}
				     }
			}
			if(cloudPointFlag)
			{
				PointParameter(Allcloud_points);
			    DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
			}

			
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0002", __FILE__, __FUNCSIG__); }
}

//function to read polygon points.....
void IGESReader::getPolygon_Points(std::list<std::string>::iterator itr)
{
	try
	{
		std::string S1, S2, S3, S4;
		std::list<std::string>::iterator itr1, itr2, itr3, itr4, itr5, itr6, itr7, itr8, itr9;
		std::list<std::string> temp, temp1, temp2, temp3;
		PointCollection *polygon_points = new PointCollection();
		
		_CRT_DOUBLE coord1, coord2;
		double linepts[7];
		int i = 0, itrlength, j = 0;		
		itr1 = itr;
		itr1--;
		S1 = itr->substr(0,64) + itr1->substr(0,64);		
		if(itr->substr(0,4) == "102,")
		{
			char * test = (char*)rlimiter1.c_str();
	        split( &temp, *itr, *test);
			itr2 = temp.begin();
			char * test1 = (char*)delimiter1.c_str();
			split( &temp1, *itr2, *test1);
			itr3 = temp1.begin();
			S2 = *(++itr3);
			itr3--;
			char * test2 = (char*)S2.c_str();
		    itrlength = _atoi64(test2);
			for(itr4 = itr; itr4 != mylist.begin(); itr4--)
			{
				if(itr4->substr(0,4) == "110,")
				{
					SinglePoint *point_coordinate = new SinglePoint();
					char * test3 = (char*)rlimiter1.c_str();
					split( &temp2, *itr4, *test3);
					itr5 = temp2.begin();
					char * test4 = (char*)delimiter1.c_str();
					split( &temp3, *itr5, *test4);
					itr6 = temp3.begin();
					itr7 = itr6;
					itr7++;
					char * test5 = (char*)(*itr7).c_str();
					double x1 = atof(test5);
					itr8 = itr7;
					itr8++;
					char * test6 = (char*)(*itr8).c_str();
					double y1 = atof(test6);
					itr9 = itr8;
					itr9++;
					char * test7 = (char*)(*itr9).c_str();
					double z1 = atof(test7);
					point_coordinate->SetValues(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue, 0);				
					polygon_points->Addpoint(point_coordinate);
					j++;
				}
				else 
				{
					continue;
				}
				
				if(j == itrlength)
				{
					break;			
				}
			}
		}
		else if(S1.substr(0,4) == "102,")
		{
			char * test = (char*)rlimiter1.c_str();
	        split( &temp, S1, *test);
			itr2 = temp.begin();
			char * test1 = (char*)delimiter1.c_str();
			split( &temp1, *itr2, *test1);
			itr3 = temp1.begin();
			S2 = *(++itr3);
			itr3--;
			char * test2 = (char*)S2.c_str();
		    itrlength = _atoi64(test2);
			for(itr4 = itr; itr4 != mylist.begin(); itr4--)
			{
				if(itr4->substr(0,4) == "110,")
				{
					SinglePoint *point_coordinate = new SinglePoint();
					char * test3 = (char*)rlimiter1.c_str();
					split( &temp2, *itr4, *test3);
					itr5 = temp2.begin();
					char * test4 = (char*)delimiter1.c_str();
					split( &temp3, *itr5, *test4);
					itr6 = temp3.begin();
					itr7 = itr6;
					itr7++;
					char * test5 = (char*)(*itr7).c_str();
					double x1 = atof(test5);
					itr8 = itr7;
					itr8++;
					char * test6 = (char*)(*itr8).c_str();
					double y1 = atof(test6);
					itr9 = itr8;
					itr9++;
					char * test7 = (char*)(*itr9).c_str();
					double z1 = atof(test7);
					point_coordinate->SetValues(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue, 0);				
					polygon_points->Addpoint(point_coordinate);
					j++;
				}
				else 
				{
					continue;
				}
				
				if(j == itrlength)
				{
					break;			
				}
			}
		}
			getPolygon_parameter(polygon_points);
			DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0003", __FILE__, __FUNCSIG__); }
}

//function to get generatrix and direcrix.....for 122 entity...
void IGESReader::getGeneratrix_Directrix122(std::list<std::string>::iterator itr)
{
	try
	{
		std::list<std::string>::iterator itr1, itr2, itr3, itr4, itr5, itr6;
		std::string S1, S2, S3, S4;
		int i = 0, m = -1, k = -1, r = -1, l = -1;
		double linepts[7], linepts2[5];
		_CRT_DOUBLE coord1, coord2;
		std::list<std::string> temp, temp1, temp2, temp3;
		itr1 = --itr;
		itr = ++itr;
		m = itr1->find("110");
		if (m != -1)
		{
			char * test = (char*)rlimiter1.c_str();
	        split( &temp, *itr1, *test);
			itr2 = temp.begin();
			char * test1 = (char*)delimiter1.c_str();
			split( &temp1, *itr2, *test1);
			for(itr3 = temp1.begin(); itr3 != temp1.end(); itr3++)
			{
				 S1 = *itr3;
		         char * test2 = (char*)S1.c_str();
		         _atodbl(&coord1,test2);
		         linepts[i] = coord1.x;
		         i++;
			}			
		}
		else
		{
			itr2 = --itr1;
			itr1 = ++itr1;
			k = itr2->find("110");
			if (k != -1)
			{
				 S1 = *itr2 + *itr1;
				 char * test = (char*)rlimiter1.c_str();
				 split( &temp, S1, *test);
				 itr3 = temp.begin();
				 char * test1 = (char*)delimiter1.c_str();
				 split( &temp1, *itr3, *test1);
				 for(itr4 = temp1.begin(); itr4 != temp1.end(); itr4++)
				 {
					  S2 = *itr4;
					  char * test2 = (char*)S2.c_str();
					  _atodbl(&coord1,test2);
					  linepts[i] = coord1.x;
					  i++;
				 }		
			}
		}
		char * test3 = (char*)rlimiter1.c_str();
	    split( &temp2, *itr, *test3);
		itr5 = temp2.begin();
		char * test4 = (char*)delimiter1.c_str();
		split( &temp3, *itr5, *test4);
		i = 0;
		for(itr6 = temp3.begin(); itr6 != temp3.end(); itr6++)
		{
			S3 = *itr6;
		    char * test5 = (char*)S3.c_str();
		    _atodbl(&coord2,test5);
		    linepts2[i] = coord2.x;
		    i++;
		}
		if(linepts[4] == linepts2[2] && linepts[5] == linepts2[3] && linepts[6] == linepts2[4])
		{
			getLine3D_parameter(&linepts[1]);
			DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
		}

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0004", __FILE__, __FUNCSIG__); }
}

//function to get generatrix and direcrix.....for 120 entity...
void IGESReader::getGeneratrix_Directrix120(std::list<std::string>::iterator itr)
{
		try
		{
			std::list<std::string> temp;
			char * test = (char*)delimiter1.c_str();
		    split( &temp, *itr, *test);
			std::list<std::string>::iterator itr1;
			itr1 = temp.begin ();
			std::list<std::string>::iterator itr2;
			itr2 = ++itr1;
			int duplicate120 = 0;
			std::string duplicatestr, duplicatestr2;
			duplicatestr = itr->substr(73);
			removespace(&duplicatestr2, duplicatestr);
			char * test11 = (char*)duplicatestr2.c_str();
			duplicate120 = _atoi64(test11);										
			if(duplicate120 > checkduplicate120)
			{
				// get directix DE no..............
				liststr1 = *(itr2) + "P";
				// get generatrix DE no............
				liststr2 = (++itr2)->substr() + "P";
				cflag = true;
				checkduplicate120 = duplicate120;
			}
		}
		catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0005", __FILE__, __FUNCSIG__); }
}

void IGESReader::split(std::list<std::string> *temp, std::string str1, char delim )	
{
	try
	{
		std::string::iterator itr1;
		int i = 0, n = 0;
		temp->clear();
		std::string tmp;
		itr1 = str1.begin ();
		n = str1.length();
		for(i = 0; i != n + 1; i++)
		{
    		if((const char)*itr1 != delim  && i != n)
			{
				tmp +=*itr1;
			}
			else 
			{
				temp->push_back (tmp);
				tmp="";
			}
			if (i != n - 1 && i != n)
			{
			itr1++;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0006", __FILE__, __FUNCSIG__); }
}

void IGESReader::removespace(std::string *result, std::string inpstr)
{
	try
	{
		 int n, k;
		 std::string strc1;
		 *result = "";
		 n = inpstr.length ();
		 for (k = 0; k!= n + 1; k++)
	     {
			strc1="";
			strc1 = inpstr.substr (k,1);
			int Tmp = (int)((const char*)strc1.c_str())[0];
			if (strc1 == " ")
		       strc1 = "";
			else if(Tmp == 10 || Tmp == 13 || Tmp == 9)
				strc1 = "";
            *result = *result + strc1;
	    }
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0007", __FILE__, __FUNCSIG__); }
}

void IGESReader::Cylinder_Coneparameter()
{   
	try
	{
		
		 bool cylinder1 = false, cone1 = false;
         double X1 = 0, Y1 = 0, Z1 = 0, X2 = 0, Y2 = 0, Z2 = 0, X3 = 0, Y3 = 0, Z3 = 0, X4 = 0, Y4 = 0, Z4 = 0;
		 double x1 = 0, y1 = 0, z1 = 0, x2 = 0, y2 = 0, z2 = 0, Cradius1 = 0, Cradius2 = 0;
		 double d1, d2, d3, g1, g2, g3;
		 int l, m, n, k, i = 0,r = 0,s = 0;
		 std::list<std::string> temp7, temp8,temp9,temp10;
		 double lpoints[7], gpoints[7];
         _CRT_DOUBLE coord1, coord2;
	     std::string str1, str3, str2, str4, str5, str6;
	 	 str1 = directrix1;
		 str2 = generatrix1;
		
		 //get directrix points........
		 char * test = (char*)rlimiter1.c_str();
		 split(&temp7,str1,*test);
		 std::list<std::string>::iterator itr1;
	     itr1 = temp7.begin();
	     str3 = *itr1;
	     char * test1 = (char*)delimiter1.c_str();
		 std::list<std::string>::iterator itr2;
	     split(&temp8, str3, *test1);
		 for(itr2 = temp8.begin(); itr2 != temp8.end(); itr2++)
	     {
		       str4=*itr2;
		       char * test1 = (char*)str4.c_str();
		       _atodbl(&coord1,test1);
		       lpoints[r] = coord1.x;
			   r++;
	     }
		 X1 = lpoints[1], Y1 = lpoints[2], Z1 = lpoints[3], X2 = lpoints[4], Y2 = lpoints[5], Z2 = lpoints[6];

		 //get generatrix coordinate.......
		 char * test2 = (char*)rlimiter1.c_str();
		 split(&temp9,str2,*test2);
		 std::list<std::string>::iterator itr3;
	     itr3 = temp9.begin();
	     str5 = *itr3;
	     char * test3 = (char*)delimiter1.c_str();
		 std::list<std::string>::iterator itr4;
	     split(&temp10, str5, *test3);
		 for(itr4 = temp10.begin(); itr4 != temp10.end(); itr4++)
	     {
		     str6=*itr4;
		     char * test4 = (char*)str6.c_str();
		     _atodbl(&coord2,test4);
		     gpoints[s] = coord2.x;
		     s++;
	     }
	     X3 = gpoints[1], Y3 = gpoints[2], Z3 = gpoints[3], X4 = gpoints[4], Y4 = gpoints[5], Z4 = gpoints[6];
		 double drd[] = {X2 - X1, Y2 - Y1, Z2 - Z1};
		 double drg[] = {X3 - X4, Y3 - Y4, Z3 - Z4};
		 double drd1[3], drg1[3];
		 RMATH3DOBJECT->DirectionCosines(drd, drd1);
		 RMATH3DOBJECT->DirectionCosines(drg, drg1);
	     d1 = drd1[0], d2 = drd1[1], d3 = drd1[2], g1 = drg1[0], g2 = drg1[1], g3 = drg1[2];
		 double tempangle =  RMATH3DOBJECT->Angle_Btwn_2Directions(drd, drg, true, false);
		 double dirarray[6] = {X2, Y2, Z2, d1, d2, d3}, genarray[3] = {X3, Y3, Z3}, ucentrept[3], bcentrept[3], bgenarray[3] = {X4, Y4, Z4};
		 RMATH3DOBJECT->Projection_Point_on_Line (genarray, dirarray, ucentrept);
		 RMATH3DOBJECT->Projection_Point_on_Line (bgenarray, dirarray, bcentrept);
		 if(( tempangle < 0.0025) || ((tempangle < 3.1425) && (tempangle > 3.1400)))
		 {
			 cylinder1 = true;
			 ncount = 0;
		 }
		 else 
		 {
			 cone1 = true;
			 ncount = 0;
	    }
	    if(cylinder1)
		{
		    if(alibre)
			{
				if(ali1 % 2 != 0)
				{

			        x1 = ucentrept[0], y1 = ucentrept[1], z1 = ucentrept[2];
			        x2 = bcentrept[0], y2 = bcentrept[1], z2 = bcentrept[2];
			        Cradius1 = sqrt((X3 - x1) * (X3 - x1) + (Y3 - y1) * (Y3 - y1) + (Z3 - z1) *(Z3 - z1));
			        Cradius2 = sqrt((X4 - x2) * (X4 - x2) + (Y4 - y2) * (Y4 - y2) + (Z4 - z2) * (Z4 - z2));
			        Cylinder* mycylinder = new Cylinder();
                    mycylinder->Radius1 (Cradius1 * multipleValue);
                    mycylinder->Radius2 (Cradius2 * multipleValue);
                    mycylinder->SetCenter1 (Vector(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue));
                    mycylinder->SetCenter2 (Vector(x2 * multipleValue, y2 * multipleValue, z2 * multipleValue));
					mycylinder->Length(sqrt(pow(x2 * multipleValue - x1 * multipleValue, 2) + pow(y2 * multipleValue - y1 * multipleValue, 2) + pow(z2 * multipleValue - z1 * multipleValue, 2)));
					mycylinder->dir_l((x2 * multipleValue - x1 * multipleValue) / mycylinder->Length());
					mycylinder->dir_m((y2 * multipleValue - y1 * multipleValue) / mycylinder->Length());
					mycylinder->dir_n((z2 * multipleValue - z1 * multipleValue) / mycylinder->Length());
					//mycylinder->updateParameters();
		            dxfShape = mycylinder;
		            dxfShape->IsValid(true);
			        ali1++;
					param = true;
				}
				else
				{
					ali1++;
				}
		    }
			else if(rapidi)
			{
				if(rap2 == rap3)
				{
					x1 = ucentrept[0], y1 = ucentrept[1], z1 = ucentrept[2];
			        x2 = bcentrept[0], y2 = bcentrept[1], z2 = bcentrept[2];
			        Cradius1 = sqrt((X3 - x1) * (X3 - x1) + (Y3 - y1) * (Y3 - y1) + (Z3 - z1) *(Z3 - z1));
			        Cradius2 = sqrt((X4 - x2) * (X4 - x2) + (Y4 - y2) * (Y4 - y2) + (Z4 - z2) * (Z4 - z2));
			        Cylinder* mycylinder = new Cylinder();
                    mycylinder->Radius1 (Cradius1 * multipleValue);
                    mycylinder->Radius2 (Cradius2 * multipleValue);
                    mycylinder->SetCenter1 (Vector(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue));
                    mycylinder->SetCenter2 (Vector(x2 * multipleValue, y2 * multipleValue, z2 * multipleValue));
					mycylinder->Length(sqrt(pow(x2 * multipleValue - x1 * multipleValue, 2) + pow(y2 * multipleValue - y1 * multipleValue, 2) + pow(z2 * multipleValue - z1 * multipleValue, 2)));
					mycylinder->dir_l((x2 * multipleValue - x1 * multipleValue) / mycylinder->Length());
					mycylinder->dir_m((y2 * multipleValue - y1 * multipleValue) / mycylinder->Length());
					mycylinder->dir_n((z2 * multipleValue - z1 * multipleValue) / mycylinder->Length());
					//mycylinder->updateParameters();
		            dxfShape = mycylinder;
		            dxfShape->IsValid(true);
			        rap3 = rap3 + 3;
					rap2++;
					param = true;
				}
				else
				{
					rap2++;
				}
			}
	   }
	   if(cone1)
	   {
		  if(alibre)
		  {
			 if(ali1 % 2 != 0)
			 {
			       x1 = ucentrept[0], y1 = ucentrept[1], z1 = ucentrept[2];
			       x2 = bcentrept[0], y2 = bcentrept[1], z2 = bcentrept[2];
			       Cradius1 = sqrt((X3 - x1) * (X3 - x1) + (Y3 - y1) * (Y3 - y1) + (Z3 - z1) *(Z3 - z1));
			       Cradius2 = sqrt((X4 - x2) * (X4 - x2) + (Y4 - y2) * (Y4 - y2) + (Z4 - z2) * (Z4 - z2));
			       Cone* mycone = new Cone();
			       mycone->Radius1 (Cradius1 * multipleValue);
			       mycone->Radius2 (Cradius2 * multipleValue);
			       mycone->SetCenter1 (Vector(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue));
			       mycone->SetCenter2 (Vector(x2 * multipleValue, y2 * multipleValue, z2 * multipleValue));
				   mycone->Length(sqrt(pow(x2 * multipleValue - x1 * multipleValue, 2) + pow(y2 * multipleValue - y1 * multipleValue, 2) + pow(z2 * multipleValue - z1 * multipleValue, 2)));
				   mycone->dir_l((x2 * multipleValue - x1 * multipleValue) / mycone->Length());
				   mycone->dir_m((y2 * multipleValue - y1 * multipleValue) / mycone->Length());
				   mycone->dir_n((z2 * multipleValue - z1 * multipleValue) / mycone->Length());
				   //mycone->updateParameters();
				   mycone->ApexAngle(tempangle);
			       dxfShape = mycone;
			       dxfShape->IsValid(true);
			       ali1++;
				   param = true;
			}
		    else
			{
				ali1++;
			}
		}
	    else if(rapidi)
	    {
		    if(rap2 == rap3)
		    {
			       x1 = ucentrept[0], y1 = ucentrept[1], z1 = ucentrept[2];
			       x2 = bcentrept[0], y2 = bcentrept[1], z2 = bcentrept[2];
			       Cradius1 = sqrt((X3 - x1) * (X3 - x1) + (Y3 - y1) * (Y3 - y1) + (Z3 - z1) *(Z3 - z1));
			       Cradius2 = sqrt((X4 - x2) * (X4 - x2) + (Y4 - y2) * (Y4 - y2) + (Z4 - z2) * (Z4 - z2));
			       Cone* mycone = new Cone();
			       mycone->Radius1 (Cradius1 * multipleValue);
			       mycone->Radius2 (Cradius2 * multipleValue);
			       mycone->SetCenter1 (Vector(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue));
			       mycone->SetCenter2 (Vector(x2 * multipleValue, y2 * multipleValue, z2 * multipleValue));
				   mycone->Length(sqrt(pow(x2 * multipleValue - x1 * multipleValue, 2) + pow(y2 * multipleValue - y1 * multipleValue, 2) + pow(z2 * multipleValue - z1 * multipleValue, 2)));
				   mycone->dir_l((x2 * multipleValue - x1 * multipleValue) / mycone->Length());
				   mycone->dir_m((y2 * multipleValue - y1 * multipleValue) / mycone->Length());
				   mycone->dir_n((z2 * multipleValue - z1 * multipleValue) / mycone->Length());
				   //mycone->updateParameters();
				   mycone->ApexAngle(tempangle);
			       dxfShape = mycone;
			       dxfShape->IsValid(true);
				   rap3 = rap3 + 3;
			       rap2++;
				   param = true;
		   }
		   else
		   {
			  rap2++;
		   }			
	    }
     }

   }
catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0008", __FILE__, __FUNCSIG__); }
}

void IGESReader::Sphereparameter ()
{
	try 
	{
		if(alibre)
	    {
			 if(ali1 % 2 != 0)
		     {
					double X1, Y1, Z1, X2, Y2, Z2, X3, Y3, Z3, R1, R2, R3, R4, R5, R6, R7, R8, R9, T1, T2, T3;
					int l = -1, m = -1, n = -1, k = 0, j = 0, i = 0;
					std::string str1, str3, str2, str4, str5, str6;
					std::list<std::string> temp7, temp8,temp9,temp10;
				    std::list<std::string>::iterator itr1, itr2, itr3, itr4;
					_CRT_DOUBLE coord;
					double entityno[13], entityno1[13];
					char * test = (char*)rlimiter1.c_str();
					split(&temp7, tmatrix1, *test);
					itr1 = temp7.begin ();
					str1 = *itr1;
					char * test1 = (char*)delimiter1.c_str();
					split(&temp8, str1, *test1);
					for(itr2 = temp8.begin(); itr2 != temp8.end(); itr2++)
					{
						 str2=*itr2;
						 char * test2 = (char*)str2.c_str();
						 _atodbl(&coord,test2);
						 entityno[i] = coord.x;
						 i++;
					}
					R1 = entityno[1], R2 = entityno[2], R3 = entityno[3], T1 = entityno[4], R4 = entityno[5], R5 = entityno[6], R6 = entityno[7], T2 = entityno[8], R7 = entityno[9], R8 = entityno[10], R9 = entityno[11], T3 = entityno[12];
					char * test3 = (char*)rlimiter1.c_str();
					split(&temp9, generatrix1, *test3);
					itr3 = temp9.begin();
					str3 = *itr3;
					char * test4 = (char*)delimiter1.c_str();
					split(&temp10, str3, *test4);
					for(itr4 = temp10.begin(); itr4 != temp10.end(); itr4++)
					{
						 str4 = *itr4;
						 char * test5 = (char*)str4.c_str();
						 _atodbl(&coord,test5);
						 entityno1[j] = coord.x;
						 j++;
					}
    			    X1 = entityno1[2], Y1 = entityno1[3], Z1 = entityno1[1], X2 = entityno1[4], Y2 = entityno1[5], Z2 = entityno1[1], X3 = entityno1[6], Y3 = entityno1[7], Z3 = entityno1[1];
				    double Transform[9] = {R1, R2, R3, R4, R5, R6, R7, R8, R9}, inputcoord1[3] = {X1, Y1, Z1}, inputcoord2[3] = {X2, Y2, Z2}, inputcoord3[3] = {X3, Y3, Z3};
				    double ans1[3], ans2[3], ans3[3];
				    double C1, C2, C3, St1, St2, St3, Tt1, Tt2, Tt3, Sradius;
				    int S1[2] = {3, 3}, S2[2] = {3, 1};
					RMATH2DOBJECT->MultiplyMatrix1(Transform, S1, inputcoord1, S2, ans1);
					RMATH2DOBJECT->MultiplyMatrix1(Transform, S1, inputcoord2, S2, ans2);
					RMATH2DOBJECT->MultiplyMatrix1(Transform, S1, inputcoord3, S2, ans3);
					C1 = ans1[0] + T1, C2 = ans1[1] + T2, C3 = ans1[2] + T3;
					St1 = ans2[0] + T1, St2 = ans2[1] + T2, St3 = ans2[2] + T3;
					Tt1 = ans3[0] + T1, Tt2 = ans3[1] + T2, Tt3 = ans3[2] + T3;
					Sradius = sqrt((C1 - St1) * (C1 - St1) + (C2 - St2) * (C2 - St2) + (C3 - St3) * (C3 - St3));
					Sphere* mysphere = new Sphere();
					mysphere->Radius (Sradius * multipleValue);
					mysphere->setCenter (Vector(C1 * multipleValue, C2 * multipleValue, C3 * multipleValue));
					dxfShape = mysphere;
					dxfShape->IsValid(true);
			        ali1++;
				    param = true;
			  }
			  else
			  {
				 ali1++;
					
			  }
		 }
		 else if(rapidi)
		 {
			double X1, Y1, Z1, X2, Y2, Z2, X3, Y3, Z3, R1, R2, R3, R4, R5, R6, R7, R8, R9, T1, T2, T3;
			int l = -1, m = -1, n = -1, k = 0, j = 0, i = 0;
			std::string str1, str3, str2, str4, str5, str6;
			std::list<std::string> temp7, temp8,temp9,temp10;
		    std::list<std::string>::iterator itr1, itr2, itr3, itr4;
			_CRT_DOUBLE coord;
			double entityno[13], entityno1[13];
			char * test = (char*)rlimiter1.c_str();
			split(&temp7, tmatrix1, *test);
			itr1 = temp7.begin ();
			str1 = *itr1;
			char * test1 = (char*)delimiter1.c_str();
		    split(&temp8, str1, *test1);
			for(itr2 = temp8.begin(); itr2 != temp8.end(); itr2++)
			{
				 str2=*itr2;
		         char * test2 = (char*)str2.c_str();
		         _atodbl(&coord,test2);
		         entityno[i] = coord.x;
		         i++;
			}
			R1 = entityno[1], R2 = entityno[2], R3 = entityno[3], T1 = entityno[4], R4 = entityno[5], R5 = entityno[6], R6 = entityno[7], T2 = entityno[8], R7 = entityno[9], R8 = entityno[10], R9 = entityno[11], T3 = entityno[12];
		    char * test3 = (char*)rlimiter1.c_str();
			split(&temp9, generatrix1, *test3);
			itr3 = temp9.begin();
			str3 = *itr3;
			char * test4 = (char*)delimiter1.c_str();
		    split(&temp10, str3, *test4);
			for(itr4 = temp10.begin(); itr4 != temp10.end(); itr4++)
			{
				 str4 = *itr4;
		         char * test5 = (char*)str4.c_str();
		         _atodbl(&coord,test5);
		         entityno1[j] = coord.x;
		         j++;
			}
    		X1 = entityno1[2], Y1 = entityno1[3], Z1 = entityno1[1], X2 = entityno1[4], Y2 = entityno1[5], Z2 = entityno1[1], X3 = entityno1[6], Y3 = entityno1[7], Z3 = entityno1[1];
			double Transform[9] = {R1, R2, R3, R4, R5, R6, R7, R8, R9}, inputcoord1[3] = {X1, Y1, Z1}, inputcoord2[3] = {X2, Y2, Z2}, inputcoord3[3] = {X3, Y3, Z3};
		    double ans1[3], ans2[3], ans3[3];
			double C1, C2, C3, St1, St2, St3, Tt1, Tt2, Tt3, Sradius;
			int S1[2] = {3, 3}, S2[2] = {3, 1};
			RMATH2DOBJECT->MultiplyMatrix1(Transform, S1, inputcoord1, S2, ans1);
			RMATH2DOBJECT->MultiplyMatrix1(Transform, S1, inputcoord2, S2, ans2);
			RMATH2DOBJECT->MultiplyMatrix1(Transform, S1, inputcoord3, S2, ans3);
			C1 = ans1[0] + T1, C2 = ans1[1] + T2, C3 = ans1[2] + T3;
			St1 = ans2[0] + T1, St2 = ans2[1] + T2, St3 = ans2[2] + T3;
			Tt1 = ans3[0] + T1, Tt2 = ans3[1] + T2, Tt3 = ans3[2] + T3;
			Sradius = sqrt((C1 - St1) * (C1 - St1) + (C2 - St2) * (C2 - St2) + (C3 - St3) * (C3 - St3));
			Sphere* mysphere = new Sphere();
			mysphere->Radius (Sradius * multipleValue);
			mysphere->setCenter (Vector(C1 * multipleValue, C2 * multipleValue, C3 * multipleValue));
			dxfShape = mysphere;
			dxfShape->IsValid(true);
			param = true;
		}
    }
catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0009", __FILE__, __FUNCSIG__); }
}

//function to read the 3DLine  parameter...//
void IGESReader::getLine3D_parameter(double *endpts)
{
	try
	{
		Line* myline = new Line();
		myline->setPoint1(Vector(endpts[0] * multipleValue, endpts[1] * multipleValue, endpts[2] * multipleValue));
		myline->setPoint2(Vector(endpts[3] * multipleValue, endpts[4] * multipleValue, endpts[5] * multipleValue));
		dxfShape = myline;
		dxfShape->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0010", __FILE__, __FUNCSIG__); }
}

//function to read polygon parameter...//
void IGESReader::getPolygon_parameter(PointCollection *polygonpoints)
{
	try 
	{
		Plane* myplane = new Plane();
		myplane->AddPoints(polygonpoints);
		dxfShape = myplane;
		dxfShape->IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0011", __FILE__, __FUNCSIG__); }
}

//function to get point parameter.......
void IGESReader::PointParameter(PointCollection *polygonpoints)
{
	try
	{
		CloudPoints* mycloudpoints = new CloudPoints();
		mycloudpoints->AddPoints(polygonpoints);
		dxfShape = mycloudpoints;
		dxfShape->IsValid(true);
	}
   catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0012", __FILE__, __FUNCSIG__); }
}

//function to get cloud points data......
void IGESReader::getAllCloudPoints_Collection(std::list<std::string>::iterator itr)
{
	try
	{
		std::list<std::string>::iterator itr1, itr2, itr3, itr4, itr5;
		std::list<std::string> temp, temp1;
		SinglePoint *point_coordinate = new SinglePoint();
					
		if(itr->substr(0,4) == "116,")
		{
			char * test = (char*)rlimiter1.c_str();
	        split( &temp, *itr, *test);
			itr1 = temp.begin();
			char * test1 = (char*)delimiter1.c_str();
			split( &temp1, *itr1, *test1);
			itr2 = temp1.begin ();
			itr3 = itr2;
			itr3++;
			char * test2 = (char*)(*itr3).c_str();
			double x1 = atof(test2);
			itr4 = itr3;
			itr4++;
			char * test3 = (char*)(*itr4).c_str();
			double y1 = atof(test3);
			itr5 = itr4;
			itr5++;
			char * test4 = (char*)(*itr5).c_str();
			double z1 = atof(test4);
			point_coordinate->SetValues(x1 * multipleValue, y1 * multipleValue, z1 * multipleValue);
			Allcloud_points->Addpoint(point_coordinate);
		}
		else
			delete point_coordinate;

	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IGESR0013", __FILE__, __FUNCSIG__); }
}
