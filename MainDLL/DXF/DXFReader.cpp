//Include required headerfiles...//
#include "stdafx.h"
#include "DXFReader.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\ShapeHeaders.h"
#include "..\Measurement\MeasurementHeaders.h"
#include "DXFExpose.h"
#include "..\Engine\PointCollection.h"

//Constructor..//
DXFReader::DXFReader()
{  
	ColorCode = 7;//Default color code...//
	this->dxfdim = NULL;
	AllPointCollection = new PointCollection();
}

//Destructor...//
DXFReader::~DXFReader()
{
	AllPointCollection->deleteAll();
}

//Load the dxf file.///
bool DXFReader::LoadFile(char* filename) //, bool FixDXFShapes)
{
	try
	{
		MAINDllOBJECT->ShowHideWaitCursor(1);
		DXFData.open(filename);
		if(!DXFData)
			return false;
		multipleValue = 1;
		int Unittype = 4;
		bool entityFlag = false;
		this->MakeDXFShapeasFixedShape = true;
		//Read till end of the file///
		while(!DXFData.eof())
		{
			try
			{
				Line2 = GetStringVal();
				if(Line2 == "ENTITIES")
					entityFlag = true;
				if(Line2 == "$INSUNITS")
				{
					Line1 = GetStringVal();
					Unittype = GetDoubleVal();
					SetUnitMultiplyFactor(Unittype);
				}

				if (entityFlag) 
				 {
					 //Read the Arc parameters..//
					if (Line2 == "ARC")
						ArcParameter();
					//Read the circle parameters../
					else if (Line2 == "CIRCLE")
						CircleParameter();
					//Read the Line parameters..//
					else if (Line2 == "LINE")
						LineParameter();
					//Read the Xline Parameters..//
					else if (Line2 == "XLINE")
						XlineParameter();
					//Read the Xray Parameters..//
					else if (Line2 == "RAY")
						XrayParameters();
					//Read the Point paramets../
					else if (Line2 == "POINT")
					{
						Pointparameter();
						if (!DXFEXPOSEOBJECT->AllPointsAsCloudPoint()) // && !FixDXFShapes)
							DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
						continue;
					}
					//Read the Text Parameters..//
					//else if(Line2 == "MTEXT")
					   // textparameter();
					   // DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
					 //Read the Polyline Parameters..//
					else if (Line2 == "POLYLINE")
						Polylineparameter();
					//Read Polyline Parameters.....//
					else if (Line2 == "LWPOLYLINE")
						Polylineparameter2D();
					else if (Line2 == "SPLINE")
						 SplineParameter();
						//continue;
					 else
						 continue;
					 //Got the shape. Now let us add to collection...
					 //if (!FixDXFShapes)
					 DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
				 }
			}
			catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0001", __FILE__, __FUNCSIG__); }
		 }
		//Close the dxf file...///
		DXFData.close();
		if(DXFEXPOSEOBJECT->AllPointsAsCloudPoint())
		{
			if(AllPointCollection->Pointscount() > 0)
			{
				dxfShape = new CloudPoints();
				MAINDllOBJECT->DoNotSamplePts = true;
				((ShapeWithList*)dxfShape)->AddPoints(AllPointCollection);
				MAINDllOBJECT->DoNotSamplePts = false;
				DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
				AllPointCollection->deleteAll();
			}
		}
		MAINDllOBJECT->ShowHideWaitCursor(2);
		MAINDllOBJECT->ZoomToWindowExtents(2);
		entityFlag = false;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0001", __FILE__, __FUNCSIG__); return false; }
}

//Read arc parameter......//
void  DXFReader::ArcParameter()
{
	try
	{
		//Variable declaration..//
		double x = 0, y = 0, z = 0, arcr = 0, stangle = 0, endangle = 0, DirVector1 = 0, DirVector2 = 0, DirVector3 = 0;
		do
		{
			Line1 = GetStringVal();
			if(Line1== "62")ColorCode = GetDoubleVal();
			else if(Line1 == "10"){x = GetDoubleVal(); x *= multipleValue;}
			else if(Line1 == "20"){y = GetDoubleVal(); y *= multipleValue;}
			else if(Line1 == "30"){z = GetDoubleVal(); z *= multipleValue;}
			else if(Line1 == "40"){arcr = GetDoubleVal(); arcr *= multipleValue;}
			else if(Line1 == "210")DirVector1 = GetDoubleVal();
			else if(Line1 == "220")DirVector2 = GetDoubleVal();
			else if(Line1 == "230")DirVector3 = GetDoubleVal();
			else if(Line1 == "50")stangle = GetDoubleVal();
			else if(Line1 == "51")endangle = GetDoubleVal();
			else Line2 = GetStringVal();
		} while(Line1 != "51" && !DXFData.eof() && Line1 != "0");

		if(DirVector3 == -1)
		{
			x = -x;
			double sangle = 180 - endangle;
			double eangle = 180 - stangle;
			stangle = sangle;
			endangle = eangle;
		}

		//////////////////////////
		if(ColorCode == 0)
			ColorCode = 7;
		dxfShape = new Circle(false, RapidEnums::SHAPETYPE::ARC);
		dxfShape->IsDxfShape(true);
		//Setting Radius to Arc
		((Circle*)dxfShape)->Radius(arcr);
		((Circle*)dxfShape)->Startangle(stangle * M_PI / 180);
		endangle = endangle - stangle;
		if(endangle < 0) endangle += 2 * 180;
		stangle = stangle * M_PI / 180;
		endangle = endangle * M_PI / 180;
		((Circle*)dxfShape)->Sweepangle(endangle);
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		//Get UCS Projecton during Loading of DXF file.
		((Circle*)dxfShape)->setCenter(Vector(x, y, z));
		double end[4];
		double cn[2] = {x, y};
		RMATH2DOBJECT->ArcEndPoints(&cn[0], arcr, stangle, endangle,&end[0]);
		((Circle*)dxfShape)->setendpoint1(Vector(end[0], end[1], z));
		((Circle*)dxfShape)->setendpoint2(Vector(end[2], end[3], z));
		((Circle*)dxfShape)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
		((Circle*)dxfShape)->calculateAttributes();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0002", __FILE__, __FUNCSIG__); }
}

//Read circle parameter........//
void  DXFReader::CircleParameter()
{	
	try
	{
		//Variable declaration...//
		double x = 0, y = 0, z = 0, circleradius, DirVector1 = 0, DirVector2 = 0, DirVector3 = 0;
		bool dirVector_CheckFlag = false;
		do
		{
			Line1 = GetStringVal();
			if(Line1== "62")ColorCode = GetDoubleVal();
			else if(Line1 == "10"){x = GetDoubleVal(); x *= multipleValue;}
			else if(Line1 == "20"){y = GetDoubleVal(); y *= multipleValue;}
			else if(Line1 == "30"){z = GetDoubleVal(); z *= multipleValue;}
			else if(Line1 == "40"){circleradius = GetDoubleVal(); circleradius *= multipleValue;}
			else Line2 = GetStringVal();
			dirVector_CheckFlag = true;
		 } while(Line1 != "40" && !DXFData.eof() && Line1 != "0");
		if(dirVector_CheckFlag)
		{
			do
			{
				Line1 = GetStringVal();
				if(Line1 == "210")DirVector1 = GetDoubleVal();
				else if(Line1 == "220")DirVector2 = GetDoubleVal();
				else if(Line1 == "230")DirVector3 = GetDoubleVal();
			}while(Line1 != "0" && !DXFData.eof());
		}
		
		if(DirVector3 == -1)
			x = -x;

		//Initilize Circle shape.....
		if(ColorCode == 0)
			ColorCode = 7;
		dxfShape = new Circle(false, RapidEnums::SHAPETYPE::CIRCLE);
		dxfShape->IsDxfShape(true);
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		Vector temp(x, y, z);
		((Circle*)dxfShape)->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
		((Circle*)dxfShape)->setCenter(temp);
		((Circle*)dxfShape)->Radius(circleradius);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0003", __FILE__, __FUNCSIG__); }
}

//Read Line Parameters...//
void DXFReader::LineParameter()
{
	try
	{
		double sx = 0, sy = 0, sz = 0, ex = 0, ey = 0, ez = 0, DirVector1 = 0, DirVector2 = 0, DirVector3 = 0;
		bool dirVector_CheckFlag = false;
		int i = 0;
		do
		{
			Line1 = GetStringVal();
			if(Line1== "62") ColorCode = GetDoubleVal();
			else if(Line1 == "10"){sx = GetDoubleVal(); sx *= multipleValue;}
			else if(Line1 == "20"){sy = GetDoubleVal(); sy *= multipleValue;}
			else if(Line1 == "30"){sz = GetDoubleVal(); sz *= multipleValue;}
			else if(Line1 == "11"){ex = GetDoubleVal(); ex *= multipleValue;}
			else if(Line1 == "21"){ey = GetDoubleVal(); ey *= multipleValue;}
			else if(Line1 == "31"){ez = GetDoubleVal(); ez *= multipleValue;}
			else Line2 = GetStringVal();
			dirVector_CheckFlag = true;
		} while(Line1 != "31" && !DXFData.eof() && Line1 != "0");

		if(dirVector_CheckFlag)
		{
			do
			{
				Line1 = GetStringVal();
				if(Line1 == "210")DirVector1 = GetDoubleVal();
				else if(Line1 == "220")DirVector2 = GetDoubleVal();
				else if(Line1 == "230")DirVector3 = GetDoubleVal();
			}while(Line1 != "0" && !DXFData.eof());
		}
		
		if(DirVector3 == -1)
		{
			sx = -sx;
			ex = -ex;
		}

		//Initilize Line shapes.
		if(ColorCode == 0)
			ColorCode = 7;
		//If we have to fix the DXF shapes we add fixed shapes here...
		//if (this->MakeDXFShapeasFixedShape)
		//{
		//	bool FromOri, ThroughOri, YOffset, XOffset;
		//	if (sx == 0 && sy == 0)
		//}

		if(abs(sz - ez) > 0.0001)
			dxfShape = new Line(false, RapidEnums::SHAPETYPE::LINE3D);
		else
			dxfShape = new Line(false);
		dxfShape->IsDxfShape(true);
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(sx, sy, sz));
		PtColl.Addpoint(new SinglePoint(ex, ey, ez));
		((ShapeWithList*)dxfShape)->AddPoints(&PtColl);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0004", __FILE__, __FUNCSIG__); }
}

//Xline Parameter...//
void DXFReader::XlineParameter()
 {
	 try
	 {
		double sx, sy, sz, ex, ey, ez;
		do
		{
			Line1 = GetStringVal();
			if(Line1 == "62")ColorCode = GetDoubleVal();
			else if(Line1 == "10"){sx = GetDoubleVal(); sx *= multipleValue;}
			else if(Line1 == "20"){sy = GetDoubleVal(); sy *= multipleValue;}
			else if(Line1 == "30"){sz = GetDoubleVal(); sz *= multipleValue;}
			else if(Line1 == "11"){ex = GetDoubleVal(); ex *= multipleValue;}
			else if(Line1 == "21"){ey = GetDoubleVal(); ey *= multipleValue;}
			else if(Line1 == "31"){ez = GetDoubleVal(); ez *= multipleValue;}
			else Line2 = GetStringVal();
		  } while(Line1 != "31" && !DXFData.eof());

		if(ColorCode == 0)
			ColorCode = 7;
		ex = sx + ex;
		ey = sy + ey;
		ez = sz + ez;
		if(abs(sz - ez) > 0.0001)
			dxfShape = new Line(false, RapidEnums::SHAPETYPE::XLINE3D);
		else
			dxfShape = new Line(false, RapidEnums::SHAPETYPE::XLINE);
		dxfShape->IsDxfShape(true);
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(sx, sy, sz));
		PtColl.Addpoint(new SinglePoint(ex, ey, ez));
		((ShapeWithList*)dxfShape)->AddPoints(&PtColl);
	 }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0005", __FILE__, __FUNCSIG__); }
 }

//Xray parameters.../
void DXFReader::XrayParameters()
 {
	 try
	 {
		double sx, sy, sz, ex, ey, ez;
		do
		{
			Line1 = GetStringVal();
			if(Line1 == "62")ColorCode = GetDoubleVal();
			else if(Line1 == "10"){sx = GetDoubleVal(); sx *= multipleValue;}
			else if(Line1 == "20"){sy = GetDoubleVal(); sy *= multipleValue;}
			else if(Line1 == "30"){sz = GetDoubleVal(); sz *= multipleValue;}
			else if(Line1 == "11"){ex = GetDoubleVal(); ex *= multipleValue;}
			else if(Line1 == "21"){ey = GetDoubleVal(); ey *= multipleValue;}
			else if(Line1 == "31"){ez = GetDoubleVal(); ez *= multipleValue;}
			else Line2 = GetStringVal();
		} while(Line1 != "31" && !DXFData.eof() && Line1 != "0");
		if(ColorCode == 0)
			ColorCode = 7;
		dxfShape = new Line(false, RapidEnums::SHAPETYPE::XRAY);
		dxfShape->IsDxfShape(true);
		ex = sx + ex;
		ey = sy + ey;
		ez = sz + ez;
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(sx, sy, sz));
		PtColl.Addpoint(new SinglePoint(ex, ey, ez));
		((ShapeWithList*)dxfShape)->AddPoints(&PtColl);
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0006", __FILE__, __FUNCSIG__); }
 }

 //Point parameters..//
void DXFReader::Pointparameter()
{
	try
	{
		double x , y , z,  DirVector1 = 0, DirVector2 = 0, DirVector3 = 0;
		bool dirVector_CheckFlag = false;
		do
		{
			Line1 = GetStringVal();
			if(Line1 == "62")ColorCode = GetDoubleVal();
			else if(Line1 == "10"){x = GetDoubleVal(); x *= multipleValue;}
			else if(Line1 == "20"){y = GetDoubleVal(); y *= multipleValue;}
			else if(Line1 == "30"){z = GetDoubleVal(); z *= multipleValue;}
			else Line2 = GetStringVal();
		} while(Line1 != "30" && !DXFData.eof() && Line1 != "0");

		if(dirVector_CheckFlag)
		{
			do
			{
				Line1 = GetStringVal();
				if(Line1 == "210")DirVector1 = GetDoubleVal();
				else if(Line1 == "220")DirVector2 = GetDoubleVal();
				else if(Line1 == "230")DirVector3 = GetDoubleVal();
			}while(Line1 != "0" && !DXFData.eof());
		}
		
		if(DirVector3 == -1)
			x = -x;

		if(DXFEXPOSEOBJECT->AllPointsAsCloudPoint())
		{
			AllPointCollection->Addpoint(new SinglePoint(x, y, z));
		}
		else
		{
			//Initlize Point shape..........
			if(ColorCode == 0)
				ColorCode = 7;
			dxfShape = new RPoint(false, RapidEnums::SHAPETYPE::RPOINT);
			dxfShape->IsDxfShape(true);
			dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
			((RPoint*)dxfShape)->setpoint(Vector(x, y, z));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0007", __FILE__, __FUNCSIG__); }
}

//Text parameters....//
void DXFReader::textparameter()
{
	try
	{
		std::string temp;
		double x, y, z, textwidth, textheight;
		do
		{
			Line1 = GetStringVal();
			if(Line1 == "62")ColorCode;
			else if(Line1 == "10"){x = GetDoubleVal(); x *= multipleValue;}
			else if(Line1 == "20"){y = GetDoubleVal(); y *= multipleValue;}
			else if(Line1 == "30"){z = GetDoubleVal(); z *= multipleValue;}
			else if(Line1 == "40")textheight = GetDoubleVal();
			else if(Line1 == "41")textwidth = GetDoubleVal();
			else if(Line1 == "1")temp = GetDoubleVal();
			else Line2 = GetStringVal();
		} while(Line1 != "1" && !DXFData.eof());

		///Initilize Shapes............
		if(ColorCode == 0)
			ColorCode = 7;
		dxfShape = new RText(false);
		dxfShape->IsDxfShape(true);
		Vector Pos;
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		Pos.set(x, y, z);
		((RText*)dxfShape)->SetCurrentText(temp);
		((RText*)dxfShape)->SetTextPosition(Vector(x, y, z));
		((RText*)dxfShape)->TextWidth(textwidth);
		((RText*)dxfShape)->TextHeight(textheight);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0008", __FILE__, __FUNCSIG__); }
}

//Read the polyline Parameters...//
void DXFReader::Polylineparameter()
{
	try
	{
		double x, y, z;
		PointCollection TempPointColl;
		do
		{
			if(Line1 != "VERTEX")
				Line1 = GetStringVal();
			if(Line1 == "62")ColorCode = GetDoubleVal();
			if(Line2 == "VERTEX")
			{
				int i = 0;
				i = 3;
			}
			if(Line1 == "VERTEX")
			{
				bool exitcheck = false;
				do
				{
					Line1 = GetStringVal();
					if(Line1 == "10"){x = GetDoubleVal(); x *= multipleValue;}
					else if(Line1 == "20"){y = GetDoubleVal(); y *= multipleValue;}
					else if(Line1 == "30")
					{
						z = GetDoubleVal(); z *= multipleValue;
						TempPointColl.Addpoint(new SinglePoint(x, y, z));
						exitcheck = true;
					}
					else if(!exitcheck)
					{
						if(Line1 == "VERTEX" || Line1 == "SEQEND")
							TempPointColl.Addpoint(new SinglePoint(x, y));
					}

				} while(Line1 != "VERTEX" && Line1 != "SEQEND");
			}
		} while(Line1 != "SEQEND" && !DXFData.eof());
		if(ColorCode == 0)
			ColorCode = 7;
		dxfShape = new Pline(false);
		dxfShape->IsDxfShape(true);
		dxfShape->ShapeAs3DShape(true);
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		((ShapeWithList*)dxfShape)->AddPoints(&TempPointColl);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0009", __FILE__, __FUNCSIG__); }
}

//Read the polyline Parameters...//
void DXFReader::Polylineparameter2D()
{
	try
	{
		double x, y;
		PointCollection TempPointColl;
		do
		{
			Line1 = GetStringVal();
			if(Line1 == "62")ColorCode = GetDoubleVal();
			else if(Line1 == "10"){x = GetDoubleVal(); x *= multipleValue;}
			else if(Line1 == "20")
			{
				y = GetDoubleVal(); y *= multipleValue;
				TempPointColl.Addpoint(new SinglePoint(x, y, 0));
			}
			else if(Line1 == "0"){}
			else Line2 = GetStringVal();
		} while(Line1 != "0" && !DXFData.eof());
		if(ColorCode == 0)
			ColorCode = 7;
		dxfShape = new Pline(false);
		dxfShape->ShapeAs3DShape(false);
		dxfShape->IsDxfShape(true);
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		((ShapeWithList*)dxfShape)->AddPoints(&TempPointColl);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0010", __FILE__, __FUNCSIG__); }
}

void DXFReader::SplineParameter()
{
	try
	{
		PointCollection ControlPtsColl;
		double* Knots_Array = NULL;
		bool ClosedSpline = false, CorruptedFile = false;
		double sx = 0, sy = 0, sz = 0, Tmp = 0;
		int degree = 0, ncpts = 0, nknots = 0, nCnt = 0;
		do
		{
			Line1 = GetStringVal();
			if(Line1== "62") ColorCode = GetDoubleVal();
			else if(Line1== "70") 
			{
				if(GetIntegerVal() % 2 != 0)
					ClosedSpline = true;
			}
			else if(Line1== "71") 
			{
				degree = GetIntegerVal();
				if(degree != 3)
				{
					CorruptedFile = true;
					break;
				}
			}
			else if(Line1== "72")
			{
				nknots = GetIntegerVal();
				Knots_Array = new double[nknots];
			}
			else if(Line1== "73") 
			{
				ncpts = GetIntegerVal();
				if(nknots != ncpts + degree + 1 || ncpts < 4)
				{
					CorruptedFile = true;
					break;
				}
			}
			else if(Line1 == "10"){sx = GetDoubleVal(); sx *= multipleValue;}
			else if(Line1 == "20"){sy = GetDoubleVal(); sy *= multipleValue;}
			else if(Line1 == "30")
			{
				sz = GetDoubleVal();
				sz *= multipleValue;
				ControlPtsColl.Addpoint(new SinglePoint(sx, sy, sz));
			}
			else if(Line1 == "40")
			{
				Knots_Array[nCnt++] = GetDoubleVal();
			}
			else if(Line1 == "0") break;
			else Line2 = GetStringVal();
		} while(Line1 != "11" && !DXFData.eof());
		if(ControlPtsColl.Pointscount() != ncpts)
		{
			ControlPtsColl.deleteAll();
			CorruptedFile = true;
		}
		if(CorruptedFile)
		{
			if(Knots_Array != NULL)
			{
				delete [] Knots_Array;
				Knots_Array = NULL;
			}
			return;
		}

		if(ColorCode == 0)
			ColorCode = 7;
		dxfShape = new Spline(false);
		((Spline*)dxfShape)->ClosedSpline = ClosedSpline;
		dxfShape->IsDxfShape(true);
		dxfShape->ShapeColor.set(DXFEXPOSEOBJECT->CADcolors[ColorCode]->X/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Y/255, DXFEXPOSEOBJECT->CADcolors[ColorCode]->Z/255);
		
		if(((Spline*)dxfShape)->KnotsArray != NULL)
		{
			delete [] ((Spline*)dxfShape)->KnotsArray;
			((Spline*)dxfShape)->KnotsArray = NULL;
		}
		((Spline*)dxfShape)->KnotsArray = new double[nknots];
		for(int i = 0; i < nknots; i++)
			((Spline*)dxfShape)->KnotsArray[i] = Knots_Array[i];
		if(Knots_Array != NULL)
		{
			delete [] Knots_Array;
			Knots_Array = NULL;
		}

		if(ControlPtsColl.Pointscount() > 210)
		{
			((Spline*)dxfShape)->framgrabShape = true;
			((ShapeWithList*)dxfShape)->AddPoints(&ControlPtsColl);
		}
		else
		{
			((Spline*)dxfShape)->framgrabShape = false;
			for(PC_ITER SptItem = ControlPtsColl.getList().begin(); SptItem != ControlPtsColl.getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				((Spline*)dxfShape)->ControlPointsList->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
			}
		}	
		ControlPtsColl.deleteAll();
		dxfShape->UpdateBestFit();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0011a", __FILE__, __FUNCSIG__); }
}

//Multiply Factor according to the unit type .. from the DXF file format..
void DXFReader::SetUnitMultiplyFactor(int type)
{
	try
	{
		//Whatever the type convert to millimeter (i.e. here it is microns) and proceed;
		switch(type)
		{
			case 0: // Unitless...
				multipleValue = 1;				
				break;
			case 1: // Inches...
				multipleValue = 25.4;
				break;
			case 2: // Feet...
				multipleValue = 304.8;
				break;
			case 3: // Miles...
				multipleValue = 1;
				break;
			case 4: // Millimeters...
				multipleValue = 1;
				break;
			case 5: // Centimeters...
				multipleValue = 10;
				break;
			case 6: // Meter...
				multipleValue = 1000;
				break;
			case 7: // Kilometers...
				multipleValue = 1;
				break;
			case 8: // Micorinches...
				multipleValue = 1;
				break;
			case 9: // Mils...
				multipleValue = 1;
				break;
			case 10: // Yards...
				multipleValue = 1;
				break;
			case 11: // Angstroms...
				multipleValue = 1;
				break;
			case 12: // Nanometers...
				multipleValue = 1;
				break;
			case 13: // Microns...
				multipleValue = 1;
				break;
			case 14: // Decimeters...
				multipleValue = 1;
				break;
			case 15: // Decameters...
				multipleValue = 1;
				break;
			case 16: // Hectometers...
				multipleValue = 1;
				break;
			case 17: // Gigameters...
				multipleValue = 1;
				break;
			case 18: // Astronomical units...
				multipleValue = 1;
				break;
			case 19: // Light Years...
				multipleValue = 1;
				break;
			case 20: // Parsecs...
				multipleValue = 1;
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0015", __FILE__, __FUNCSIG__); }
}

//Load the drl file.///
bool DXFReader::LoadDrillFile(char* filename)
{
	try
	{
		MAINDllOBJECT->ShowHideWaitCursor(1);
		DXFData.open(filename);
		if(!DXFData)
			return false;
		multipleValue = 1;
		double MulFactor = 1, CurrRadius = 0, x = 0, y = 0;
		std::map<int, double> ToolVals;
		ToolVals.clear();
		bool HeaderRegion = false, BodyRegion = false, LeadingZeros = true;
		int Tmp1, Tmp2, Tmp3;
		//Read till end of the file///
		while(!DXFData.eof())
		{
			try
			{
				Line2 = GetStringVal();
				if(Line2 == "M48")
				{
					if(!HeaderRegion) HeaderRegion = true;
				}
				else if(Line2 == "M30" || Line2 == "M00")
				{
					break;
				}
				else if(Line2 == "%" || Line2 == "M95")
				{
					if(!BodyRegion) BodyRegion = true;
				}
				else if(*Line2.begin() == 'T')
				{
					if(HeaderRegion && !BodyRegion)
					{
						int ValStartIndx = Line2.find('C');
						ValStartIndx++;
						ToolVals[atoi(Line2.substr(1, ValStartIndx - 2).c_str())] = DrillConverstionStringToDouble(Line2.substr(ValStartIndx, Line2.length() - ValStartIndx), LeadingZeros);
					}
					else if(BodyRegion)
					{
						CurrRadius = ToolVals[atoi(Line2.substr(1, Line2.length() - 1).c_str())] * MulFactor / 2;
					}
				}
				else if(*Line2.begin() == 'X')
				{
					if(BodyRegion)
					{
						int ValYStartIndx = Line2.find('Y');
						x = DrillConverstionStringToDouble(Line2.substr(1, ValYStartIndx - 1), LeadingZeros) * MulFactor;
						ValYStartIndx++;
						y = DrillConverstionStringToDouble(Line2.substr(ValYStartIndx, Line2.length() - ValYStartIndx), LeadingZeros) * MulFactor;
						//Initilize Circle shape.....
						dxfShape = new Circle(false, RapidEnums::SHAPETYPE::CIRCLE);
						dxfShape->IsDxfShape(true);
						Vector temp(x, y, 0);
						((Circle*)dxfShape)->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
						((Circle*)dxfShape)->setCenter(temp);
						((Circle*)dxfShape)->Radius(CurrRadius);
						DXFEXPOSEOBJECT->AddDXFShapes(dxfShape);
					}
				}
				else
				{
					Tmp1 = Line2.find("INCH");
					Tmp2 = Line2.find("M72");
					Tmp3 = Line2.find("TZ");
					if(Tmp1 > -1 || Tmp2 > -1)
					{
						if(HeaderRegion && !BodyRegion)
						{
							MulFactor = 25.4;
							if(Tmp3 > -1)
								LeadingZeros = false;
						}
					}
					else if(Tmp3 > -1)
					{
						if(HeaderRegion && !BodyRegion) LeadingZeros = false;
					}
				}
				
			}
			catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0001", __FILE__, __FUNCSIG__); }
		}
		//Close the dxf file...///
		DXFData.close();
		MAINDllOBJECT->ShowHideWaitCursor(2);
		MAINDllOBJECT->ZoomToWindowExtents(2);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0001", __FILE__, __FUNCSIG__); return false; }
}

bool DXFReader::LoadNCFile(char* filename)
{
	try
	{
		MAINDllOBJECT->ShowHideWaitCursor(1);
		DXFData.open(filename);
		if (!DXFData) return false;

		bool GotProbeSection = false;
		//Read till end of the file///
		while (!DXFData.eof())
		{
			try
			{
				Line2 = GetStringVal();
				if (Line2.length() == 0)
				{
					GotProbeSection = false;
					continue;
				}
				if (Line2.substr(0, 6) == "(Probe")
					GotProbeSection = true;
				if (GotProbeSection)
				{
					if (Line2.substr(4, 3) == "G00" || Line2.substr(4, 2) == "G0")
					{
						//We got a go to position (PRobe position in Rapid-I)
						//RAction probePositionAction = new RAction();
						double xVal = 0, yVal = 0, zVal = 0;
						int ValStartIndx = 0, valEndIndx = 0;
						ValStartIndx = Line2.find('X');
						if (ValStartIndx >= 0)
						{
							ValStartIndx++;
							valEndIndx = Line2.substr(ValStartIndx).find(' ');
							xVal = DrillConverstionStringToDouble(Line2.substr(ValStartIndx, valEndIndx - ValStartIndx), 0);
						}
						else
							xVal = MAINDllOBJECT->getCurrentDRO().getX();

						if (ValStartIndx >= 0)
						{
							ValStartIndx = Line2.find('Y');
							ValStartIndx++;
							valEndIndx = Line2.substr(ValStartIndx).find(' ');
							yVal = DrillConverstionStringToDouble(Line2.substr(ValStartIndx, valEndIndx - ValStartIndx), 0);
						}
						else
							yVal = MAINDllOBJECT->getCurrentDRO().getY();

						ValStartIndx = Line2.find('Z');
						if (ValStartIndx >= 0)
						{
							ValStartIndx++;
							valEndIndx = Line2.substr(ValStartIndx).find(' ');
							zVal = DrillConverstionStringToDouble(Line2.substr(ValStartIndx, valEndIndx - ValStartIndx), 0);
						}
						else
							zVal = MAINDllOBJECT->getCurrentDRO().getZ();

						MAINDllOBJECT->NotedownProbePath(0, xVal, yVal, zVal);
						//if (!HeaderRegion) HeaderRegion = true;
					}
					else if (Line2.substr(4, 3) == "G65") //Line2 == "M30" || Line2 == "M00")
					{

					}
					else if (Line2.substr(4, 3) == "G43")
					{
						
					}
					else if (*Line2.begin() == 'X')
					{
						
					}
					else
					{
						
					}
				}
			}
			catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0001", __FILE__, __FUNCSIG__); }
		}
		//Close the dxf file...///
		DXFData.close();
		MAINDllOBJECT->ShowHideWaitCursor(2);
		MAINDllOBJECT->ZoomToWindowExtents(2);
		return true;
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0001", __FILE__, __FUNCSIG__); return false; }
}


std::string DXFReader::GetStringVal()
{
	try
	{
		std::string tempstr;
		getline(DXFData, tempstr);
		return RemoveSpace(tempstr);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0016", __FILE__, __FUNCSIG__); }
}

  double DXFReader::GetDoubleVal()
  {
	  try
	  {
		  std::string tempstr, AnsStr = "";
		  getline(DXFData, tempstr);
		  AnsStr = RemoveSpace(tempstr);
		  return atof((const char*)AnsStr.c_str());
	  }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0017", __FILE__, __FUNCSIG__); }
}

  int DXFReader::GetIntegerVal()
  {
	  try
	  {
		  std::string tempstr, AnsStr = "";
		  getline(DXFData, tempstr);
		  AnsStr = RemoveSpace(tempstr);
		  return atoi((const char*)AnsStr.c_str());
	  }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0017a", __FILE__, __FUNCSIG__); return 0;}
}

  std::string DXFReader::RemoveSpace(std::string inpstr)
  {
	  try
	  {
		  int n, k;
		  std::string strc1, result = "";
		  n = inpstr.length();
		  for (k = 0; k < n; k++)
	      {
			 strc1 = "";
			 strc1 = inpstr.substr (k,1);
			 int Tmp = (int)((const char*)strc1.c_str())[0];
			 if (strc1 == " ")
			 {
		        strc1 = "";
	         }
			 else if(Tmp == 10 || Tmp == 13 || Tmp == 9)
				 strc1 = "";
             result = result + strc1;
	     }
		 return result;
	  }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0018", __FILE__, __FUNCSIG__); }
}

double DXFReader::DrillConverstionStringToDouble(string stringToConvet, bool leadingZeros)
{
	try
	{
		int DotIndx = stringToConvet.find('.'), PlusIndx = stringToConvet.find('+'), MinusIndx = stringToConvet.find('-');
		string sign = "";
		if(DotIndx > -1)
		   return atof(stringToConvet.c_str());
		if(PlusIndx > -1)
		{
			stringToConvet = stringToConvet.substr(1, 6);
			sign = "+";
		}
		else if(MinusIndx > -1)
		{
			stringToConvet = stringToConvet.substr(1, 6);
			sign = "-";
		}
		if(leadingZeros)
		{
			if(stringToConvet.length() == 2) stringToConvet += "0";
     		string str1 =  stringToConvet.substr(0, 2);
			string str2 =  stringToConvet.substr(2, 6);
			string ansstr = str1 + "." + str2;
			if(sign == "-")
				ansstr = sign + ansstr;
			return atof(ansstr.c_str());
		}
		else
		{
			double ans = atof(stringToConvet.c_str());
			ans /= 10000;
			if(sign == "-")
				ans *= -1;
			return ans;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFR0018a", __FILE__, __FUNCSIG__); }
}