# include "stdafx.h"
#include "DXFWriter.h"
#include "..\Engine\RCadApp.h"

//Constructor ..//
DXFWriter::DXFWriter()
{
	this->HandleCount = 81;
}

//Destructor...//
DXFWriter::~DXFWriter()
{
}

//Convert double to string...//
char* DXFWriter::DoubletoString(double x)
{
	try
	{
		if(abs(x) > 1000000000)
			return "0.0";
		RMATH2DOBJECT->Double2String(x, 10, cd, false); 
		return cd;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0001", __FILE__, __FUNCSIG__); return "0.0"; }
}

char* DXFWriter::IntegertoString(int x)
{
	try
	{
		_itoa(x, cd, 10);		
		return cd;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0002", __FILE__, __FUNCSIG__); return "0"; }
}

//Add point parameters...//
void DXFWriter::addPointparameter(double x1, double y1, double z1, int colorcode)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
		{
			x1 = x1 / 25.4; y1 = y1 / 25.4; z1 = z1 / 25.4;
		}		
		setCommonParameter("AcDbPoint", "POINT", x1, y1, z1, colorcode);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0003", __FILE__, __FUNCSIG__); }
}

//Add the circle parameters...
void DXFWriter::addCircleParameter(double cx, double cy, double cz, double crad, int colorcode)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
		{
			cx /= 25.4; cy /= 25.4; cz /= 25.4; crad /= 25.4;
		}
		setCommonParameter("AcDbCircle", "CIRCLE", cx, cy, cz, colorcode);
		DXFList.push_back("40");
		DXFList.push_back(DoubletoString(crad));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0004", __FILE__, __FUNCSIG__); }
}

//Add arc parameters...//
void DXFWriter::addArcParameter(double ax, double ay, double az, double arad, double startangle, double endangle, int colorcode)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
		{
			ax /= 25.4; ay /= 25.4; az /= 25.4; arad /= 25.4;
		}
		setCommonParameter("AcDbCircle", "ARC", ax, ay, az, colorcode);
		DXFList.push_back("40");
		DXFList.push_back(DoubletoString(arad));
		DXFList.push_back("100");
		DXFList.push_back("AcDbArc");
		DXFList.push_back("50");
		DXFList.push_back(DoubletoString(startangle * 180 / M_PI));
		DXFList.push_back("51");
		DXFList.push_back(DoubletoString(endangle * 180 / M_PI));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0005", __FILE__, __FUNCSIG__); }
}

//Add line parameters...//
void DXFWriter::addLineParameter(char* str, double lsx, double lsy, double lsz, double lex, double ley, double lez, int colorcode)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
		{
			lsx /= 25.4; lsy /= 25.4; lsz /= 25.4; lex /= 25.4; ley /= 25.4; lez /= 25.4;
		}
		setCommonParameter("AcDbLine", str, lsx, lsy, lsz, colorcode);
		DXFList.push_back("11");
		DXFList.push_back(DoubletoString(lex));
		DXFList.push_back("21");
		DXFList.push_back(DoubletoString(ley));
		DXFList.push_back("31");
		DXFList.push_back(DoubletoString(lez));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0006", __FILE__, __FUNCSIG__); }
}

//Add Xline parameters...//
void DXFWriter::addXLineParameter(char* str, double lsx, double lsy, double lsz, double lex, double ley, double lez, int colorcode)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
		{
			lsx /= 25.4; lsy /= 25.4; lsz /= 25.4;
		}
		setCommonParameter("AcDbXline", str, lsx, lsy, lsz, colorcode);
		DXFList.push_back("11");
		DXFList.push_back(DoubletoString(lex));
		DXFList.push_back("21");
		DXFList.push_back(DoubletoString(ley));
		DXFList.push_back("31");
		DXFList.push_back(DoubletoString(lez));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0007", __FILE__, __FUNCSIG__); }
}

//Add Xline parameters...//
void DXFWriter::addRayParameter(char* str, double lsx, double lsy, double lsz, double lex, double ley, double lez, int colorcode)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
		{
			lsx /= 25.4; lsy /= 25.4; lsz /= 25.4;
		}
		setCommonParameter("AcDbRay", str, lsx, lsy, lsz, colorcode);
		DXFList.push_back("11");
		DXFList.push_back(DoubletoString(lex));
		DXFList.push_back("21");
		DXFList.push_back(DoubletoString(ley));
		DXFList.push_back("31");
		DXFList.push_back(DoubletoString(lez));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0008", __FILE__, __FUNCSIG__); }
}

//Add the text parameters...//
void DXFWriter::addtextparameter(double ax, double bx, double cx, double width, double height, char* text, int colorcode)
{
	try
	{
		setCommonParameter("AcDbMText", "MTEXT", ax, bx, cx, colorcode);
		DXFList.push_back("40");
		DXFList.push_back(DoubletoString(height));
		DXFList.push_back("41");
		DXFList.push_back(DoubletoString(width));
		DXFList.push_back("1");
		DXFList.push_back(text);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0009", __FILE__, __FUNCSIG__); }
}

//Add the polyline2D., for set of points...//
void DXFWriter::AddPolylineparameters2D(PointCollection* Pointlist, int colorcode)
{
	try
	{
		DXFList.push_back("0");
		DXFList.push_back("LWPOLYLINE");
		DXFList.push_back("5");
		DXFList.push_back(IntegertoString(this->HandleCount));
		this->HandleCount += 2;
		DXFList.push_back("330");
		DXFList.push_back("1F");
		DXFList.push_back("100");
		DXFList.push_back("AcDbEntity");
		DXFList.push_back("8");
		DXFList.push_back("0");
		DXFList.push_back("100");
		DXFList.push_back("AcDbPolyline");
		DXFList.push_back("62");
		DXFList.push_back(DoubletoString(colorcode));
		DXFList.push_back("90");
		DXFList.push_back(DoubletoString(Pointlist->Pointscount()));
		for(PC_ITER Item = Pointlist->getList().begin(); Item != Pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double X = Spt->X, Y = Spt->Y;
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
			{
				X /= 25.4; Y /= 25.4;
			}
			DXFList.push_back("10");
			DXFList.push_back(DoubletoString(X));
			DXFList.push_back("20");
			DXFList.push_back(DoubletoString(Y));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0010", __FILE__, __FUNCSIG__); }
}

//Add the polyline., for set of points...//
void DXFWriter::AddPolylineparameters(PointCollection* Pointlist, int colorcode)
{
	try
	{
		DXFList.push_back("0");
		DXFList.push_back("POLYLINE");
		DXFList.push_back("5");
		DXFList.push_back(IntegertoString(this->HandleCount));
		this->HandleCount += 2;
		DXFList.push_back("330");
		DXFList.push_back("1F");
		DXFList.push_back("100");
		DXFList.push_back("AcDbEntity");
		DXFList.push_back("8");
		DXFList.push_back("0");
		DXFList.push_back("100");
		DXFList.push_back("AcDb3dPolyline");
		DXFList.push_back("62");
		DXFList.push_back(IntegertoString(colorcode));
		DXFList.push_back("66");
		DXFList.push_back("1");
		DXFList.push_back("10");
		DXFList.push_back("0");
		DXFList.push_back("20");
		DXFList.push_back("0");
		DXFList.push_back("30");
		DXFList.push_back("0");
		for(PC_ITER Item = Pointlist->getList().begin(); Item != Pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
			{
				Spt->X /= 25.4; Spt->Y /= 25.4; Spt->Z /= 25.4;
			}
			setCommonParameter("AcDb3dPolylineVertex", "VERTEX", Spt->X, Spt->Y, Spt->Z, colorcode, false);
		}
		DXFList.push_back("0");
		DXFList.push_back("SEQEND");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0011", __FILE__, __FUNCSIG__); }
}

//Add Spline Parameter...
void DXFWriter::AddSplineParameter(PointCollection* Pointlist, int NumberOfKnots, double* Knots_Array, bool ClosedSpline, int colorcode)
{
	try
	{
		DXFList.push_back("0");
		DXFList.push_back("SPLINE");
		DXFList.push_back("5");
		DXFList.push_back(IntegertoString(this->HandleCount));
		this->HandleCount += 2;
		DXFList.push_back("330");
		DXFList.push_back("1F");
		DXFList.push_back("100");
		DXFList.push_back("AcDbEntity");
		DXFList.push_back("8");
		DXFList.push_back("0");
		DXFList.push_back("100");
		DXFList.push_back("AcDbSpline");
		DXFList.push_back("62");
		DXFList.push_back(IntegertoString(colorcode));
		DXFList.push_back("70");
		if(ClosedSpline)
			DXFList.push_back("11");
		else
			DXFList.push_back("8");
		DXFList.push_back("71");
		DXFList.push_back("3");
		DXFList.push_back("72");
		DXFList.push_back(IntegertoString(NumberOfKnots));
		DXFList.push_back("73");
		DXFList.push_back(IntegertoString(Pointlist->Pointscount()));
		for(int i = 0; i < NumberOfKnots; i++)
		{
			DXFList.push_back("40");
			double X = Knots_Array[i];
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
			{
				X /= 25.4;
			}
			DXFList.push_back(DoubletoString(X));
		}
		for(PC_ITER Item = Pointlist->getList().begin(); Item != Pointlist->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double X = Spt->X, Y = Spt->Y, Z = Spt->Z;
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
			{
				X /= 25.4; Y /= 25.4; Z /= 25.4;
			}
			DXFList.push_back("10");
			DXFList.push_back(DoubletoString(X));
			DXFList.push_back("20");
			DXFList.push_back(DoubletoString(Y));
			DXFList.push_back("30");
			DXFList.push_back(DoubletoString(Z));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW00012", __FILE__, __FUNCSIG__);}
}

//Set common parameter for each shape....//
void DXFWriter::setCommonParameter(char* SubclassMarker, char* EntityName, double x1, double x2, double x3, int colorcode, bool flag)
{
	try
	{
		DXFList.push_back("0");
		DXFList.push_back(EntityName);
		DXFList.push_back("5");
		DXFList.push_back(IntegertoString(this->HandleCount));
		DXFList.push_back("330");
		DXFList.push_back("1F");
		DXFList.push_back("100");
		DXFList.push_back("AcDbEntity");
		DXFList.push_back("8"); 
		DXFList.push_back("0");
		DXFList.push_back("100");
		DXFList.push_back(SubclassMarker);
		if(flag)
		{
			DXFList.push_back("62");
			DXFList.push_back(IntegertoString(colorcode));
		}
		DXFList.push_back("10");
		DXFList.push_back(DoubletoString(x1));
		DXFList.push_back("20");
		DXFList.push_back(DoubletoString(x2));
		DXFList.push_back("30");
		DXFList.push_back(DoubletoString(x3));
		this->HandleCount += 2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0013", __FILE__, __FUNCSIG__); }
}

//Generate the dxf file///
bool DXFWriter::generateDXFFile(char* filename)
{
	try
	{
		 this->filename = filename;
		 DXFDataWriter.open(filename);//, std::ios_base::app);
		 if(!DXFDataWriter)
			 return false;
		 bool FileCompleted = false;
		 if(WriteHeader())
		 {
			 for(DXFListIterator = DXFList.begin(); DXFListIterator != DXFList.end(); DXFListIterator++)
				 DXFDataWriter << *DXFListIterator << "\n";
			 FileCompleted = WriteTail();
		 }
		 DXFDataWriter.close();
   		 return FileCompleted;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0014", __FILE__, __FUNCSIG__); return false; }
}

bool DXFWriter::WriteHeader()
{
	try
	{
		std::string UnitType = "4";
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)	
			UnitType = "1";
		std::list<std::string> Unitlist;
		DXFDataWriter << "0" << "\n";	
		DXFDataWriter << "SECTION" << "\n";
		DXFDataWriter << "2" << "\n";
		DXFDataWriter << "HEADER" << "\n";
		DXFDataWriter << "9" << "\n";
		DXFDataWriter << "$ACADVER" << "\n";
		DXFDataWriter << "1" << "\n";
		DXFDataWriter << "AC1018" << "\n";
		DXFDataWriter << "9" << "\n";
		DXFDataWriter << "$INSUNITS" << "\n";
		DXFDataWriter << "70" << "\n";
		DXFDataWriter << UnitType << "\n";
		DXFDataWriter << "9" << "\n";
		DXFDataWriter << "$HANDSEED" << "\n";
		DXFDataWriter << "5" << "\n";
		DXFDataWriter << HandleCount << "\n";
		
		//read header...
		std::ifstream HeaderFile;
		HeaderFile.open(MAINDllOBJECT->DatabaseFolderPath + "\\Database\\EntryHeader.dxf");
		if(!HeaderFile)
			return false;
		std::string TempStr;
		while(!HeaderFile.eof())
		{
			getline(HeaderFile, TempStr);
			DXFDataWriter << TempStr << "\n";
		}
		HeaderFile.close();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0015", __FILE__, __FUNCSIG__);  return false;}
}

bool DXFWriter::WriteTail()
{
	try
	{
		std::ifstream TailFile;
		TailFile.open(MAINDllOBJECT->DatabaseFolderPath + "\\Database\\ExitHeader.dxf");
		if(!TailFile)
			return false;
		std::string TempStr;
		while(!TailFile.eof())
		{
			getline(TailFile, TempStr);
			DXFDataWriter << TempStr << "\n";
		}
		TailFile.close();
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFW0016", __FILE__, __FUNCSIG__); return false;}
}

void DXFWriter::ResetHandleCount()
{
	this->HandleCount = 81;
}