//Dxf reader class..//
//Reads the dxf file and instantiate the respective shape/ measurement classes
#pragma once
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

class PointCollection;
class Shape;
class DimBase;

class DXFReader
{
private:
  double ColorCode;
  ifstream DXFData;
  Shape* dxfShape;
  DimBase* dxfdim;
  std::string Line1, Line2;
  double multipleValue;
  bool MakeDXFShapeasFixedShape;
  PointCollection* AllPointCollection;
public:
  //Constructor and destructor..//
   DXFReader();
  ~DXFReader();
  //Load the DXF file..//
  bool LoadFile(char* filename); // , bool FixDXFShapes);
  //Load the DRL file..//
  bool LoadDrillFile(char* filename);
  //Load NC file for reading points and touch points for Part Program generation
  bool LoadNCFile(char* filename);
  
private:
  //Function to read the point parameters..//
  void Pointparameter();
  //Read the line parameters..//
  void LineParameter();
  //Read the Xline Parameters../
  void XlineParameter();
  //Read Ray Parameters..//
  void XrayParameters();
  //Read Circle Parameters...//
  void CircleParameter();
  //Read arc parameters..//
  void ArcParameter();
  //Read Polyline parameters..//
  void Polylineparameter();
  //Read Polyline parameters..//
  void Polylineparameter2D();
  //Read the Text parameters..//
  void textparameter();
  //Read Spline Parameter..
  void SplineParameter();
  //Set Unit Multiply factor...
  void SetUnitMultiplyFactor(int type);

  std::string GetStringVal();
  double GetDoubleVal();
  int GetIntegerVal();
  std::string RemoveSpace(std::string inpStr);
  double DrillConverstionStringToDouble(string stringToConvet, bool leadingZeros);
};


//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!