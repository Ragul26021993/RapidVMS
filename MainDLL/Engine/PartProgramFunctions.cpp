#pragma once
#include "stdafx.h"
#include "PartProgramFunctions.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\UCS.h"
#include "..\Actions\ActionHeaders.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Shapes\ShapeHeaders.h"
#include "..\Measurement\MeasurementHeaders.h"
#include "..\DXF\DXFExpose.h"
#include "..\Engine\FocusFunctions.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "..\Engine\ThreadCalcFunctions.h"
#include "..\Helper\Helper.h"

std::list<int> ShapeIDList;

//Constructor..
PartProgramFunctions::PartProgramFunctions()
{
	try
	{
		InitialisePartprogramRelatedFlags();
		OneShotpixelarray = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0001", __FILE__, __FUNCSIG__); }
}

//Destructor.../
PartProgramFunctions::~PartProgramFunctions()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0002", __FILE__, __FUNCSIG__); }
}

//Initialise flags..
void PartProgramFunctions::InitialisePartprogramRelatedFlags()
{
	try
	{
		this->ReferenceDotIstakenAsSecondHomePos(false);
		this->ProgramMadeUsingTwoReferenceDot(false);
		this->ReferencePtAsHomePos(false);
		this->ProgramRunningForFirstTime(false);
		this->ReferenceDotIstakenAsHomePosition(false);
		this->ProgramMadeUsingReferenceDot(false);
		this->ReferenceLineAsRefAngle(false);
		this->ProgramMadeUsingRefLine(false);
		this->ReferenceLineAngle = 0;
		this->FirstShapeisSetAsReferenceShape(false);
		this->VblockAxisAsReference(false);
		this->ProgramMadeUsingVblockAxis(false);
		this->ProgramMadeUsingFirstShapeAsReference(false);
		this->SelectedShapesForTwoStepHoming(false);
		this->FinishedManualTwoStepAlignment(false);
		this->PartProgramAlignmentIsDone(false);
		this->ProgramMadeUsingTwoStepHoming(false);
		this->UpdateFistFramegrab(false);
		this->PartprogramLoaded(false);
		this->IsPartProgramRunning(false);
		this->DrawPartprogramArrow(false);
		this->FirstMousedown_Partprogram(false);
		this->FirstMousedown_Partprogram_Manual(false);
		this->StartingPointofPartprogram(false);
		this->ProgramAutoIncrementMode(false);
		this->ProgramDoneForIdorOdMeasurement(false);
		this->PartprogramisLoadingEditMode(false);
		this->DoShortestPath(false);
		this->PartprogramisLoading(false);
		this->PPDontUpdateSnapPts(false);
		this->PPAlignEditFlag(false);
		this->ImageActionpresent = false;
		this->ExportPointsToCsvInPP(false);
		this->ExportParamToCsvInPP(false);
		this->SigmaModeInPP = false;
		this->IntersectionPointMissed(false);
		this->UseImageComparision(false);
		this->ScaleFactor(1);
		FirstProbePointActionFlag = false; ActionTobeTransformed = false;
		this->ImageRAngle = 0;
		ProbeTouchPointDro[0] = 0; ProbeTouchPointDro[1] = 0; ProbeTouchPointDro[2] = 0;
		this->RefreshImageForEdgeDetection = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0003", __FILE__, __FUNCSIG__); }
}



//Load the partprogram data..
void PartProgramFunctions::loadData(std::string filename)
{
	try
	{
		//MessageBox(NULL, L"Started Reading File", L"Rapid-I", MB_TOPMOST);

		bool enteringFlag = false;
		//bool Valid_FileEnd;
		bool isMCS = true;
		std::wstring buffer;
		std::wifstream ifile;
		ifile.open(filename.c_str());
		ifile >> buffer;
		ifile.precision(15);
		ifile >> buffer;
		DXFEXPOSEOBJECT->CurrentDXFShapeList.clear();
		//MessageBox(NULL, L"Cleared DXF FIles Memory", L"Rapid-I", MB_TOPMOST);
		//Read till end of the file..//
	while(!ifile.eof())//buffer !=  L"EndSelect")
		{
			try
			{
				if(!MAINDllOBJECT->PPLoadSuccessful())
				{
					MAINDllOBJECT->PPLoadSuccessful(true);
					MAINDllOBJECT->clearAll();
					MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
					//this->PPActionlist.clearAll();
					break;
				}
				if(buffer ==  L"EndSelect")
					break; //Logical end of file reached
				//MessageBox(NULL, buffer.c_str(), L"Rapid-I", MB_TOPMOST);
				//DRO Position
				if(buffer == L"CurrentDRO")
				{
					double vX, vY, vZ, vR;
					ifile >> vX;
					ifile >> vY;
					ifile >> vZ;
					ifile >> vR;
					SavedCurrentMachineDRO.set(vX, vY, vZ, vR);
				}
				else if(buffer == L"VblockCalibrationAxis")
				{
					bool flag = false;
					ifile >> flag;
					ProgramMadeUsingVblockAxis(flag);
					if(ProgramMadeUsingVblockAxis())
					{
						if(MAINDllOBJECT->Vblock_CylinderAxisLine == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
				}
				//Home Position for the probe...
				else if(buffer == L"ProbeHome")
				{
					double vX,vY,vZ;
					bool flag; int cn;
					ifile >> vX;
					ifile >> vY;
					ifile >> vZ;
					PPHomePosition.set(vX, vY, vZ);

					ifile >> flag;
					ProgramMadeUsingReferenceDot(flag);
					if(ProgramMadeUsingReferenceDot())
					{
						if(MAINDllOBJECT->ReferenceDotShape == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
					ifile >> flag; 
					ProgramMadeUsingFirstShapeAsReference(flag);
					ifile >> flag;
					ProgramMadeUsingTwoStepHoming(flag);
					ifile >> cn;
					ProgramMachineType = RapidEnums::RAPIDMACHINETYPE(cn);
					ifile >> cn;
					ProgramAngleMeasurementMode(cn);
					MAINDllOBJECT->AngleMeasurementMode(cn);
					ifile >> cn;
					ProgramMeasureUnitType = RapidEnums::RAPIDUNITS(cn);
					MAINDllOBJECT->MeasureUnitType = ProgramMeasureUnitType;
				}
				else if(buffer == L"ReferenceDotIstakenAsSecondHomePos")
				{
					bool flag = false;
					ifile >> flag;
					ProgramMadeUsingTwoReferenceDot(flag);
					if(this->ProgramMadeUsingTwoReferenceDot())
					{
						if(MAINDllOBJECT->SecondReferenceDotShape == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
				}
				else if(buffer == L"ReferenceLineAsRefAngle")
				{
					bool flag = false;
					ifile >> flag;
					this->ProgramMadeUsingRefLine(flag);
					ifile >> ReferenceLineAngle;
					if(this->ProgramMadeUsingRefLine())
					{
						if(MAINDllOBJECT->ReferenceLineShape == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail05", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
				}
				else if(buffer == L"SecondReference")
				{
					double vX = 0, vY = 0, vZ = 0;
					ifile >> vX;
					ifile >> vY;
					ifile >> vZ;
					PPHomePosition_2nd.set(vX, vY, vZ);
				}
				//Read the UCS properties..//
				else if(buffer == L"UCS")
				{
					if(isMCS)
						isMCS = false;
					else
					{
						UCS* ucs = new UCS(_T("pUCS"));
						ifile >> (*ucs);
						UCS& cucs = MAINDllOBJECT->getCurrentUCS();
						ucs->getWindow(0).init(cucs.getWindow(0));
						ucs->getWindow(1).init(cucs.getWindow(1));
						ucs->getWindow(2).init(cucs.getWindow(2));
						MAINDllOBJECT->AddingUCSModeFlag = true;
						AddUCSAction::addUCS(ucs);
						MAINDllOBJECT->AddingUCSModeFlag = false;
					}
				}
				//Read all shapes.. and its properties..//
				else if (buffer == L"shape")
				{
					int cn;	
					std::wstring cnstr;
					std::wstring skipcn;
					ifile >> cnstr;
					int ColonIndx = cnstr.find(':');
					if(ColonIndx != -1)
					{
						MAINDllOBJECT->IsOldPPFormat(false);
						std::wstring TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end() );
						cn=atoi((const char*)Val.c_str());
					}
					else
					{
						MAINDllOBJECT->IsOldPPFormat(true);
						std::string Val(cnstr.begin() , cnstr.end());
						cn=atoi((const char*)Val.c_str());
					}
					buffer = cn;
					RapidEnums::SHAPETYPE temp = RapidEnums::SHAPETYPE(cn);
					RapidEnums::CIRCLETYPE Ctype;
					Shape *item;
					enteringFlag = false;
					switch(temp)
					{
						case RapidEnums::SHAPETYPE::LINE:
							enteringFlag = true;
							item = new Line(_T("pL"), RapidEnums::SHAPETYPE::LINE);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::LINE3D:
							enteringFlag = true;
							item = new Line(_T("pL3D"), RapidEnums::SHAPETYPE::LINE3D);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XLINE3D:
							enteringFlag = true;
							item = new Line(_T("pXL3D"), RapidEnums::SHAPETYPE::XLINE3D);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XLINE:
							enteringFlag = true;
							item = new Line(_T("pXL"), RapidEnums::SHAPETYPE::XLINE);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XRAY:
							enteringFlag = true;
							item = new Line(_T("pR"), RapidEnums::SHAPETYPE::XRAY);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::CIRCLE:
							ifile >> cn;
							buffer = cn;
							Ctype = RapidEnums::CIRCLETYPE(cn);							
							ifile >> skipcn; buffer = skipcn;
							enteringFlag = true;
							if(Ctype == RapidEnums::CIRCLETYPE::PCDCIRCLE)
							{
								item = new PCDCircle(_T("pPCD"));
								ifile >> (*static_cast<PCDCircle*>(item));
							}
							else
							{
								item = new Circle(_T("pC"), RapidEnums::SHAPETYPE::CIRCLE);
								ifile >> (*static_cast<Circle*>(item));
							}
							break;

						case RapidEnums::SHAPETYPE::ARC:
							enteringFlag = true;
							item = new Circle(_T("pA"), RapidEnums::SHAPETYPE::ARC);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::TWOARC:
							enteringFlag = true;
							item = new TwoArc(_T("TAR"));
							ifile >> (*static_cast<TwoArc*>(item));
							break;

						case RapidEnums::SHAPETYPE::TWOLINE:
							enteringFlag = true;
							item = new TwoLine(_T("TL"));
							ifile >> (*static_cast<TwoLine*>(item));
							break;  

						case RapidEnums::SHAPETYPE::RPOINT:
							enteringFlag = true;
							item = new RPoint(_T("pP"), RapidEnums::SHAPETYPE::RPOINT);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RPOINT3D:
							enteringFlag = true;
							item = new RPoint(_T("pP3D"), RapidEnums::SHAPETYPE::RPOINT3D);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RELATIVEPOINT:
							enteringFlag = true;
							item = new RPoint(_T("pRP"), RapidEnums::SHAPETYPE::RELATIVEPOINT);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RTEXT:
							enteringFlag = true;
							item = new RText(_T("pT"));
							ifile >> (*static_cast<RText*>(item));
							break;

						case RapidEnums::SHAPETYPE::CLOUDPOINTS:
							enteringFlag = true;
							item = new CloudPoints(_T("pCP"));
							ifile >> (*static_cast<CloudPoints*>(item));
							break;

						case RapidEnums::SHAPETYPE::DUMMY:
							enteringFlag = true;
							item = new DummyShape();
							ifile >> (*static_cast<DummyShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::DEPTHSHAPE:
							enteringFlag = true;
							item = new DepthShape(_T("pD"));
							ifile >> (*static_cast<DepthShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::PERIMETER:
							enteringFlag = true;
							item = new Perimeter(_T("pPeri"));
							ifile >> (*static_cast<Perimeter*>(item));
							break;

						case RapidEnums::SHAPETYPE::PLANE:
							enteringFlag = true;
							item = new Plane(_T("pPL"));
							ifile >> (*static_cast<Plane*>(item));
							break;

						case RapidEnums::SHAPETYPE::CYLINDER:
							enteringFlag = true;
							item = new Cylinder(_T("pCL"));
							ifile >> (*static_cast<Cylinder*>(item));
							break;

						case RapidEnums::SHAPETYPE::SPHERE:
							enteringFlag = true;
							item = new Sphere(_T("pSH"));
							ifile >> (*static_cast<Sphere*>(item));
							break;

						case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE:
							enteringFlag = true;
							item = new IntersectionShape(_T("pIS"));
							ifile >> (*static_cast<IntersectionShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::CONE:
							enteringFlag = true;
							item = new Cone(_T("pCN"));
							ifile >> (*static_cast<Cone*>(item));
							break;

						case RapidEnums::SHAPETYPE::CIRCLE3D:
							enteringFlag = true;
							item = new Circle(_T("pC3D"),RapidEnums::SHAPETYPE::CIRCLE3D);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::ARC3D:
							enteringFlag = true;
							item = new Circle(_T("pA3D"),RapidEnums::SHAPETYPE::ARC3D);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::ELLIPSE3D:
							enteringFlag = true;
							item = new Conics3D(_T("pEL"), RapidEnums::SHAPETYPE::ELLIPSE3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;	

						case RapidEnums::SHAPETYPE::PARABOLA3D:
							enteringFlag = true;
							item = new Conics3D(_T("pPB"), RapidEnums::SHAPETYPE::PARABOLA3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;

						case RapidEnums::SHAPETYPE::HYPERBOLA3D:
							enteringFlag = true;
							item = new Conics3D(_T("pHB"), RapidEnums::SHAPETYPE::HYPERBOLA3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;

						case RapidEnums::SHAPETYPE::SPLINE:
							enteringFlag = true;
							item = new Spline(_T("S"));
							ifile >> (*static_cast<Spline*>(item));
							break;

						case RapidEnums::SHAPETYPE::TORUS:
							enteringFlag = true;
							item = new Torus(_T("TR"));
							ifile >> (*static_cast<Torus*>(item));
							break;

						case RapidEnums::SHAPETYPE::AREA:
							enteringFlag = true;
							item = new AreaShape(_T("A"));
							ifile >> (*static_cast<AreaShape*>(item));
							break;
					}
					if(!MAINDllOBJECT->PPLoadSuccessful())
					{
						MAINDllOBJECT->PPLoadSuccessful(true);
						MAINDllOBJECT->clearAll();
						MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
						break;
					}
					if(enteringFlag)
					{
						item->UpdateEnclosedRectangle();
						AddShapeAction::addShape(item, false);
						if(item->ShapeType == RapidEnums::SHAPETYPE::PERIMETER)
							((Perimeter*)item)->SetParentChildRelation();
					}
				}
				//Read the all the measurements.... 
				else if(buffer == L"Measurement")
				{
					int cn;					
					/*ifile >> cn;	*/				
					std::wstring cnstr;
					ifile >> cnstr;
					int ColonIndx = cnstr.find(':');
					if(ColonIndx != -1)
					{
					std::wstring TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
					std::string Val(TagVal.begin(), TagVal.end() );
					cn=atoi((const char*)Val.c_str());
					}
					else
					{
						std::string Val(cnstr.begin() , cnstr.end());
						cn=atoi((const char*)Val.c_str());
					}
					buffer = cn;
					RapidEnums::MEASUREMENTTYPE temp = RapidEnums::MEASUREMENTTYPE(cn);
					DimBase *item;
					enteringFlag = false;
					switch(temp)
					{
						case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
							enteringFlag = true;
							item = new DimPointCoordinate(_T("Center"));
							ifile >> (*static_cast<DimPointCoordinate*>(item));
							((DimPointCoordinate*)item)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
							enteringFlag = true;
							item = new DimOrdinate(_T("Pt_X"), temp);
							ifile >> (*static_cast<DimOrdinate*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
							enteringFlag = true;
							item = new DimOrdinate(_T("Pt_Y"), temp);
							ifile >> (*static_cast<DimOrdinate*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
							enteringFlag = true;
							item = new DimOrdinate(_T("Pt_Z"), temp);
							ifile >> (*static_cast<DimOrdinate*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE:
						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS:
							enteringFlag = true;
							item = new DimPointToPointDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToPointDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DIGITALMICROMETER:
							enteringFlag = true;
							item = new DimDigitalMicrometer(_T("duM"));
							ifile >> (*static_cast<DimDigitalMicrometer*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE:
							enteringFlag = true;
							item = new DimPointToLineDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE:
							enteringFlag = true;
							item = new DimPointToCircleDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToCircleDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE:
						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINERAYANGLE:
							enteringFlag = true;
							item = new DimLineToLineDistance(true);
							ifile >> (*static_cast<DimLineToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE:
							enteringFlag = true;
							item = new DimLineToLineDistance(true);
							ifile >> (*static_cast<DimLineToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
							enteringFlag = true;
							item = new DimAngularity(_T("Par"), temp);
							ifile >> (*static_cast<DimAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
							enteringFlag = true;
							item = new DimAngularity(_T("Per"), temp);
							ifile >> (*static_cast<DimAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
							enteringFlag = true;
							item = new DimAngularity(_T("Anglty"), temp);
							ifile >> (*static_cast<DimAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH:
							enteringFlag = true;
							item = new DimLine(_T("Dist"), temp);
							ifile >> (*static_cast<DimLine*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS:
						case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED:
							{
							enteringFlag = true;
							item = new DimLine(_T("St"), temp);
							ifile >> (*static_cast<DimLine*>(item));
							break;
							}

						case RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE:
							enteringFlag = true;
							item = new DimLine(_T("Ang"), temp);
							ifile >> (*static_cast<DimLine*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOCIRCLEDISTANCE:
							enteringFlag = true;
							item = new DimCircleToCircleDistance(_T("Dist"));
							ifile >> (*static_cast<DimCircleToCircleDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOLINEDISTANCE:
							enteringFlag = true;
							item = new DimCircleToLineDistance(_T("Dist"));
							ifile >> (*static_cast<DimCircleToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D:
							enteringFlag = true;
							item = new DimDiameter2D(_T("Dia"));
							ifile >> (*static_cast<DimDiameter2D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT:
							enteringFlag = true;
							item = new DimSplineMeasurement(_T("SpCt"));
							ifile >> (*static_cast<DimSplineMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMIC:
							enteringFlag = true;
							item = new DimSplineMeasurement(_T("SpMic"));
							ifile >> (*static_cast<DimSplineMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMCC:
							enteringFlag = true;
							item = new DimSplineMeasurement(_T("SpMcc"));
							ifile >> (*static_cast<DimSplineMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE:
							enteringFlag = true;
							item = new DimGrooveMeasurement(_T("Ang"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE);
							ifile >> (*static_cast<DimGrooveMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH:
							enteringFlag = true;
							item = new DimGrooveMeasurement(_T("Dpt"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH);
							ifile >> (*static_cast<DimGrooveMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH:
							enteringFlag = true;
							item = new DimGrooveMeasurement(_T("Wth"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH);
							ifile >> (*static_cast<DimGrooveMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D:
							enteringFlag = true;
							item = new DimRadius2D(_T("Radius"));
							ifile >> (*static_cast<DimRadius2D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY:
							enteringFlag = true;
							item = new DimCircularity(_T("Circ"), temp);
							ifile >> (*static_cast<DimCircularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY:
						case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY_D:
							enteringFlag = true;
							item = new DimConcentricity(_T("Concen"));
							ifile >> (*static_cast<DimConcentricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC:
							enteringFlag = true;
							item = new DimCircularity(_T("Mic"), temp);
							ifile >> (*static_cast<DimCircularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC:
							enteringFlag = true;
							item = new DimCircularity(_T("Mcc"), temp);
							ifile >> (*static_cast<DimCircularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
							enteringFlag = true;
							item = new DimTruePosition(_T("TPG"), temp);
							ifile >> (*static_cast<DimTruePosition*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
							enteringFlag = true;
							item = new DimTruePosition(_T("TPX"), temp);
							ifile >> (*static_cast<DimTruePosition*>(item));
							break;
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
							enteringFlag = true;
							item = new DimTruePosition(_T("TPY"), temp);
							ifile >> (*static_cast<DimTruePosition*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
							enteringFlag = true;
							item = new DimTruePositionPoint(_T("TPG"), temp);
							ifile >> (*static_cast<DimTruePositionPoint*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
							enteringFlag = true;
							item = new DimTruePositionPoint(_T("TPX"), temp);
							ifile >> (*static_cast<DimTruePositionPoint*>(item));
							break;
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
							enteringFlag = true;
							item = new DimTruePositionPoint(_T("TPY"), temp);
							ifile >> (*static_cast<DimTruePositionPoint*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D:
							enteringFlag = true;
							item = new DimTruePosition3D(_T("TPG3D"), temp);
							ifile >> (*static_cast<DimTruePosition3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D:
							enteringFlag = true;
							item = new DimTruePosition3DPoint(_T("TPG3D"), temp);
							ifile >> (*static_cast<DimTruePosition3DPoint*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE:
							enteringFlag = true;
							item = new DimArcAngle(_T("ArcAngle"), temp);
							ifile >> (*static_cast<DimArcAngle*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH:
							enteringFlag = true;
							item = new DimArcAngle(_T("Arclen"), temp);
							ifile >> (*static_cast<DimArcAngle*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DEPTH:
							enteringFlag = true;
							item = new DimDepth(_T("Depth"));
							ifile >> (*static_cast<DimDepth*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ROTARYANGLEMEASURE:
							enteringFlag = true;
							item = new DimRotaryAngleMeasure(_T("R_Ang"));
							ifile >> (*static_cast<DimRotaryAngleMeasure*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_THREAD:
							enteringFlag = true;
							item = new DimThread();
							ifile >> (*static_cast<DimThread*>(item));
							((DimThread*)item)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PCDOFFSET:
							enteringFlag = true;
							item = new DimPCDOffset();
							ifile >> (*static_cast<DimPCDOffset*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PCDANGLE:
							enteringFlag = true;
							item = new DimPCDAngle();
							ifile >> (*static_cast<DimPCDAngle*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D:
							enteringFlag = true;
							item = new DimPointToPointDistance3D(_T("Dist"));
							ifile >> (*static_cast<DimPointToPointDistance3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D:
							enteringFlag = true;
							item = new DimPointToLineDistance3D(_T("Dist"));
							ifile >> (*static_cast<DimPointToLineDistance3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE:
							enteringFlag = true;
							item = new DimPointToPlaneDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToPlaneDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimPointToCylinderDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimPointToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE3D:
							enteringFlag = true;
							item = new DimLineToLineDistance3D(false);
							ifile >> (*static_cast<DimLineToLineDistance3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOPLANEDISTANCE:
							enteringFlag = true;
							item = new DimLineToPlaneDistance(true);
							ifile >> (*static_cast<DimLineToPlaneDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimLineToCylinderDistance(true);
							ifile >> (*static_cast<DimLineToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimLineToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimLineToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOPLANEDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToPlaneDistance(true);
							ifile >> (*static_cast<DimPlaneToPlaneDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToCylinderDistance(true);
							ifile >> (*static_cast<DimPlaneToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimPlaneToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimCylinderToCylinderDistance(true);
							ifile >> (*static_cast<DimCylinderToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimCylinderToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimCylinderToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimSphereToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimSphereToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D:
							enteringFlag = true;
							item = new DimDiameter3D(_T("Dia"));
							ifile >> (*static_cast<DimDiameter3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS3D:
							enteringFlag = true;
							item = new DimRadius3D(_T("Radius"));
							ifile >> (*static_cast<DimRadius3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS:
						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED:
							enteringFlag = true;
							item = new DimLine3D(_T("Stl3d"), temp);
							ifile >> (*static_cast<DimLine3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
							enteringFlag = true;
							item = new DimLine3DAngularity(_T("l3dPar"), temp);
							ifile >> (*static_cast<DimLine3DAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
							enteringFlag = true;
							item = new DimLine3DAngularity(_T("l3dPer"), temp);
							ifile >> (*static_cast<DimLine3DAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
							enteringFlag = true;
							item = new DimLine3DAngularity(_T("l3dAng"), temp);
							ifile >> (*static_cast<DimLine3DAngularity*>(item));
							break;
					
						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY:
						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED:
							enteringFlag = true;
							item = new DimCylindricity(_T("Cyl"), temp);
							ifile >> (*static_cast<DimCylindricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC:
							enteringFlag = true;
							item = new DimCylindricity(_T("Cymic"), temp);
							ifile >> (*static_cast<DimCylindricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC:
							enteringFlag = true;
							item = new DimCylindricity(_T("Cymcc"), temp);
							ifile >> (*static_cast<DimCylindricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylPer"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylPar"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylAng"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylCoax"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConPer"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConAng"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConCoax"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConPar"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS:
						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED:
							enteringFlag = true;
							item = new DimFlatness(_T("Flt"));
							ifile >> (*static_cast<DimFlatness*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
							enteringFlag = true;
							item = new DimPlaneAngularity(_T("PlPar"), temp);
							ifile >> (*static_cast<DimPlaneAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
							enteringFlag = true;
							item = new DimPlaneAngularity(_T("PlPer"), temp);
							ifile >> (*static_cast<DimPlaneAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
							enteringFlag = true;
							item = new DimPlaneAngularity(_T("PlAng"), temp);
							ifile >> (*static_cast<DimPlaneAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY:
							enteringFlag = true;
							item = new DimSphericity(_T("Sph"), temp);
							ifile >> (*static_cast<DimSphericity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC:
							enteringFlag = true;
							item = new DimSphericity(_T("Sphmic"), temp);
							ifile >> (*static_cast<DimSphericity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC:
							enteringFlag = true;
							item = new DimSphericity(_T("Sphmcc"), temp);
							ifile >> (*static_cast<DimSphericity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER:
							enteringFlag = true;
							item = new DimPerimeter_Diameter(_T("P_Dia"), temp);
							ifile >> (*static_cast<DimPerimeter_Diameter*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_PERIMETER:
							enteringFlag = true;
							item = new DimPerimeter_Diameter(_T("P_Peri"), temp);
							ifile >> (*static_cast<DimPerimeter_Diameter*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_AREA:
							enteringFlag = true;
							item = new DimPerimeter_Diameter(_T("P_Area"), temp);
							ifile >> (*static_cast<DimPerimeter_Diameter*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimCircle3DToCircle3DDistance(true);
							ifile >> (*static_cast<DimCircle3DToCircle3DDistance*>(item));
							break;

						
						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCONEDISTANCE:
							enteringFlag = true;
							item = new DimCircle3DToConeDistance(true);
							ifile >> (*static_cast<DimCircle3DToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGLE:
							enteringFlag = true;
							item = new DimAngle3D(_T("Ang"));
							ifile >> (*static_cast<DimAngle3D*>(item));
							break;
						
						case RapidEnums::MEASUREMENTTYPE::DIM_CONETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimConeToConeDistance(true);
							ifile >> (*static_cast<DimConeToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCONEDISTANCE:
							enteringFlag = true;
							item = new DimCylinderToConeDistance(true);
							ifile >> (*static_cast<DimCylinderToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimLineToCircle3DDistance(true);
							ifile >> (*static_cast<DimLineToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimLineToConeDistance(true);
							ifile >> (*static_cast<DimLineToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToCircle3DDistance(true);
							ifile >> (*static_cast<DimPlaneToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToConeDistance(true);
							ifile >> (*static_cast<DimPlaneToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimPointToCircle3DDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE:
							enteringFlag = true;
							item = new DimPointToConeDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONICS3DDISTANCE:
							enteringFlag = true;
							item = new DimPointToConics3DDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToConics3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimSphereToCircle3DDistance(_T("Dist"));
							ifile >> (*static_cast<DimSphereToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimSphereToConeDistance(_T("Dist"));
							ifile >> (*static_cast<DimSphereToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_AXISLENGTH:
							enteringFlag = true;
							item = new DimAxisLength(_T("Dist"));
							ifile >> (*static_cast<DimAxisLength*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE:
							enteringFlag = true;
							item = new DimCircleDeviationMeasure(_T("Dev"));
							ifile >> (*static_cast<DimCircleDeviationMeasure*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CLOUDPOINTDEPTH:
							enteringFlag = true;
							item = new DimCloudPointDepth(_T("Dist"));
							ifile >> (*static_cast<DimCloudPointDepth*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIMTOTAL_LENGTH:
							enteringFlag = true;
							item = new DimTotalLength(_T("T_Per"));
							ifile >> (*static_cast<DimTotalLength*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT:
							enteringFlag = true;
							item = new DimAreaMeasurement(_T("Area"));
							ifile >> (*static_cast<DimAreaMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTFARTHEST:
						case RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTNEAREST:
						case RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTFARTHEST:
						case RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTNEAREST:
						case RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER:			//measurement added to part program when loading....    09/02/2014
						case RapidEnums::MEASUREMENTTYPE::DIM_MICROMETRICCUTTINGDIAMETER:       //vinod
						case RapidEnums::MEASUREMENTTYPE::DIM_DIST1:		    //vinod
						case RapidEnums::MEASUREMENTTYPE::DIM_DIST2:		    //vinod
						case RapidEnums::MEASUREMENTTYPE::DIM_AXIALRUNOUT:
							enteringFlag = true;
							item = new DimRunOutMeasurement(false);
							ifile >> (*static_cast<DimRunOutMeasurement*>(item));
							break;
					}
					if(!MAINDllOBJECT->PPLoadSuccessful())
					{
						MAINDllOBJECT->PPLoadSuccessful(true);
						MAINDllOBJECT->clearAll();
						MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
						break;
					}
					if(enteringFlag)
					{
						item->MeasurementType = temp;
						AddDimAction::addDim(item);
					}
				}
				else if (buffer == L"dxfshape")
				{
					int cn;	
					std::wstring cnstr;
					std::wstring skipcn;
					ifile >> cnstr;
					int ColonIndx = cnstr.find(':');
					if(ColonIndx != -1)
					{
						MAINDllOBJECT->IsOldPPFormat(false);
						std::wstring TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end() );
						cn=atoi((const char*)Val.c_str());
					}
					else
					{
						MAINDllOBJECT->IsOldPPFormat(true);
						std::string Val(cnstr.begin() , cnstr.end());
						cn=atoi((const char*)Val.c_str());
					}
					buffer = cn;
					RapidEnums::SHAPETYPE temp = RapidEnums::SHAPETYPE(cn);
					RapidEnums::CIRCLETYPE Ctype;
					Shape *item;
					enteringFlag = false;
					switch(temp)
					{
						case RapidEnums::SHAPETYPE::LINE:
							enteringFlag = true;
							item = new Line(_T("pL"), RapidEnums::SHAPETYPE::LINE);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::LINE3D:
							enteringFlag = true;
							item = new Line(_T("pL3D"), RapidEnums::SHAPETYPE::LINE3D);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XLINE3D:
							enteringFlag = true;
							item = new Line(_T("pXL3D"), RapidEnums::SHAPETYPE::XLINE3D);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XLINE:
							enteringFlag = true;
							item = new Line(_T("pXL"), RapidEnums::SHAPETYPE::XLINE);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XRAY:
							enteringFlag = true;
							item = new Line(_T("pR"), RapidEnums::SHAPETYPE::XRAY);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::CIRCLE:
							ifile >> cn;
							buffer = cn;
							Ctype = RapidEnums::CIRCLETYPE(cn);							
							ifile >> skipcn; buffer = skipcn;
							enteringFlag = true;
							if(Ctype == RapidEnums::CIRCLETYPE::PCDCIRCLE)
							{
								item = new PCDCircle(_T("pPCD"));
								ifile >> (*static_cast<PCDCircle*>(item));
							}
							else
							{
								item = new Circle(_T("pC"), RapidEnums::SHAPETYPE::CIRCLE);
								ifile >> (*static_cast<Circle*>(item));
							}
							break;

						case RapidEnums::SHAPETYPE::ARC:
							enteringFlag = true;
							item = new Circle(_T("pA"), RapidEnums::SHAPETYPE::ARC);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::TWOARC:
							enteringFlag = true;
							item = new TwoArc(_T("TAR"));
							ifile >> (*static_cast<TwoArc*>(item));
							break;

						case RapidEnums::SHAPETYPE::TWOLINE:
							enteringFlag = true;
							item = new TwoLine(_T("TL"));
							ifile >> (*static_cast<TwoLine*>(item));
							break;  

						case RapidEnums::SHAPETYPE::RPOINT:
							enteringFlag = true;
							item = new RPoint(_T("pP"), RapidEnums::SHAPETYPE::RPOINT);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RPOINT3D:
							enteringFlag = true;
							item = new RPoint(_T("pP3D"), RapidEnums::SHAPETYPE::RPOINT3D);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RELATIVEPOINT:
							enteringFlag = true;
							item = new RPoint(_T("pRP"), RapidEnums::SHAPETYPE::RELATIVEPOINT);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RTEXT:
							enteringFlag = true;
							item = new RText(_T("pT"));
							ifile >> (*static_cast<RText*>(item));
							break;

						case RapidEnums::SHAPETYPE::CLOUDPOINTS:
							enteringFlag = true;
							item = new CloudPoints(_T("pCP"));
							ifile >> (*static_cast<CloudPoints*>(item));
							break;

						case RapidEnums::SHAPETYPE::DUMMY:
							enteringFlag = true;
							item = new DummyShape();
							ifile >> (*static_cast<DummyShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::DEPTHSHAPE:
							enteringFlag = true;
							item = new DepthShape(_T("pD"));
							ifile >> (*static_cast<DepthShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::PERIMETER:
							enteringFlag = true;
							item = new Perimeter(_T("pPeri"));
							ifile >> (*static_cast<Perimeter*>(item));
							break;

						case RapidEnums::SHAPETYPE::PLANE:
							enteringFlag = true;
							item = new Plane(_T("pPL"));
							ifile >> (*static_cast<Plane*>(item));
							break;

						case RapidEnums::SHAPETYPE::CYLINDER:
							enteringFlag = true;
							item = new Cylinder(_T("pCL"));
							ifile >> (*static_cast<Cylinder*>(item));
							break;

						case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE :
							enteringFlag = true;
							item = new IntersectionShape(_T("pIS"));
							ifile >> (*static_cast<IntersectionShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::SPHERE:
							enteringFlag = true;
							item = new Sphere(_T("pSH"));
							ifile >> (*static_cast<Sphere*>(item));
							break;
							
						case RapidEnums::SHAPETYPE::CONE:
							enteringFlag = true;
							item = new Cone(_T("pCN"));
							ifile >> (*static_cast<Cone*>(item));
							break;

						case RapidEnums::SHAPETYPE::CIRCLE3D:
							enteringFlag = true;
							item = new Circle(_T("pC3D"),RapidEnums::SHAPETYPE::CIRCLE3D);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::ARC3D:
							enteringFlag = true;
							item = new Circle(_T("pA3D"),RapidEnums::SHAPETYPE::ARC3D);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::ELLIPSE3D:
							enteringFlag = true;
							item = new Conics3D(_T("pEL"), RapidEnums::SHAPETYPE::ELLIPSE3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;	

						case RapidEnums::SHAPETYPE::PARABOLA3D:
							enteringFlag = true;
							item = new Conics3D(_T("pPB"), RapidEnums::SHAPETYPE::PARABOLA3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;

						case RapidEnums::SHAPETYPE::HYPERBOLA3D:
							enteringFlag = true;
							item = new Conics3D(_T("pHB"), RapidEnums::SHAPETYPE::HYPERBOLA3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;

						case RapidEnums::SHAPETYPE::SPLINE:
							enteringFlag = true;
							item = new Spline(_T("S"));
							ifile >> (*static_cast<Spline*>(item));
							break;

						case RapidEnums::SHAPETYPE::TORUS:
							enteringFlag = true;
							item = new Torus(_T("TR"));
							ifile >> (*static_cast<Torus*>(item));
							break;
					}
					if(!MAINDllOBJECT->PPLoadSuccessful())
					{
						MAINDllOBJECT->PPLoadSuccessful(true);
						MAINDllOBJECT->clearAll();
						MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
						break;
					}
					if(enteringFlag)
					{
						item->UpdateEnclosedRectangle();
						DXFEXPOSEOBJECT->CurrentDXFShapeList.push_back(item);
						DXFEXPOSEOBJECT->PPDXFShapeList[item->getId()] = item;
						DXFEXPOSEOBJECT->PPDXFShapeListOriginal[item->getId()] = item->CreateDummyCopy();
						MAINDllOBJECT->UpdateShapesChanged();
						MAINDllOBJECT->dontUpdateGraphcis = false;
					}
				}
				//Read all the actions..//
				else if(buffer == L"Action")
				{
					int cn;	
					std::wstring cnstr;
					std::wstring skipcn;
					ifile >> cnstr;
					//MessageBox(NULL, cnstr.c_str(), L"Rapid-I", MB_TOPMOST);
					int ColonIndx = cnstr.find(':');
					if (ColonIndx != -1)
					{
						std::wstring TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end());
						cn = atoi((const char*)Val.c_str());
						//MessageBox(NULL, TagVal.c_str(), L"Rapid-I", MB_TOPMOST);
					}
					else
					{
						std::string Val(cnstr.begin() , cnstr.end());
						cn=atoi((const char*)Val.c_str());
					}
					/*int cn;
					ifile >> cn;*/

					buffer = cn;
					RapidEnums::ACTIONTYPE temp = RapidEnums::ACTIONTYPE(cn);
					RAction *item;
					enteringFlag = false;
					switch(temp)
					{
						case RapidEnums::ACTIONTYPE::ADDPOINTACTION:
							enteringFlag = true;
							item = new AddPointAction();
							ifile >> (*static_cast<AddPointAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION:
							enteringFlag = true;
							item = new AddThreadMeasureAction();
							ifile >> (*static_cast<AddThreadMeasureAction*>(item));
							break;
					
						case RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION:
							enteringFlag = true;
							item = new AddProbePathAction();
							ifile >> (*static_cast<AddProbePathAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::IMAGE_ACTION:
							enteringFlag = true;
							item = new AddImageAction();
							ifile >> (*static_cast<AddImageAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::COPYSHAPEACTION:
							enteringFlag = true;
							item = new CopyShapesAction();
							ifile >> (*static_cast<CopyShapesAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::ROTATIONACTION:
							enteringFlag = true;
							item = new RotationAction();
							ifile >> (*static_cast<RotationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION:
							enteringFlag = true;
							item = new LocalisedCorrectionAction();
							ifile >> (*static_cast<LocalisedCorrectionAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::REFLECTIONACTION:
							enteringFlag = true;
							item = new ReflectionAction();
							ifile >> (*static_cast<ReflectionAction*>(item));
							break;

					   case RapidEnums::ACTIONTYPE::TRANSLATIONACTION:
							enteringFlag = true;
							item = new TranslationAction();
							ifile >> (*static_cast<TranslationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad1PtAction();
							ifile >> (*static_cast<EditCad1PtAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION:
							enteringFlag = true;
							item = new Cad1PtAlignAction();
							ifile >> (*static_cast<Cad1PtAlignAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION:
							enteringFlag = true;
							item = new Cad2PtAlignAction();
							ifile >> (*static_cast<Cad2PtAlignAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION:
							enteringFlag = true;
							item = new Cad1Pt1LineAction();
							ifile >> (*static_cast<Cad1Pt1LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION:
							enteringFlag = true;
							item = new Cad2LineAction();
							ifile >> (*static_cast<Cad2LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION:
							enteringFlag = true;
							item = new CircularInterPolationAction();
							ifile >> (*static_cast<CircularInterPolationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION:
							enteringFlag = true;
							item = new LinearInterPolationAction();
							ifile >> (*static_cast<LinearInterPolationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION:
							enteringFlag = true;
							item = new ProfileScanAction();
							ifile >> (*static_cast<ProfileScanAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::AREA_ACTION:
							enteringFlag = true;
							item = new AreaShapeAction();
							ifile >> (*static_cast<AreaShapeAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad2PtAction();
							ifile >> (*static_cast<EditCad2PtAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::AUTOALIGNACTION:
							enteringFlag = true;
							item = new AutoAlignAction();
							ifile >> (*static_cast<AutoAlignAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::DEVIATIONACTION:
							enteringFlag = true;
							item = new DeviationAction();
							ifile >> (*static_cast<DeviationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION:
							enteringFlag = true;
							item = new DXF_DeviationAction();
							ifile >> (*static_cast<DXF_DeviationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION:
							enteringFlag = true;
							item = new CloudComparisionAction();
							ifile >> (*static_cast<CloudComparisionAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::MOVESHAPEACTION:
							enteringFlag = true;
							item = new MoveShapesAction();
							ifile >> (*static_cast<MoveShapesAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad2LineAction();
							ifile >> (*static_cast<EditCad2LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad1Pt1LineAction();
							ifile >> (*static_cast<EditCad1Pt1LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::PERIMETERACTION:
							enteringFlag = true;
							item = new PerimeterAction();
							ifile >> (*static_cast<PerimeterAction*>(item));
							break;
						case RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION:
							enteringFlag = true;
							item = new AddShapePointsAction();
							ifile >> (*static_cast<AddShapePointsAction*>(item));
							break;
					
					/*	case RapidEnums::ACTIONTYPE::ADDUCSACTION:
							enteringFlag = true;
							item = new AddUCSAction();
							ifile >> (*static_cast<AddUCSAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CHANGEUCSACTION:
							enteringFlag = true;
							item = new ChangeUCSAction();
							ifile >> (*static_cast<ChangeUCSAction*>(item));
							break;*/
					}
					if(!MAINDllOBJECT->PPLoadSuccessful())
					{
						MAINDllOBJECT->PPLoadSuccessful(true);
						MAINDllOBJECT->clearAll();
						MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
						break;
					}
					if(enteringFlag)
						getPPActionlist().addItem(item);
				}
				ifile >> buffer;
			}
			catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0004", __FILE__, __FUNCSIG__); }
		}
	
		//MessageBox(NULL, L"Completed Reading Till end of file", L"Rapid-I", MB_TOPMOST);

		for(RC_ITER Citem = MAINDllOBJECT->getShapesList().getList().begin(); Citem != MAINDllOBJECT->getShapesList().getList().end(); Citem++)
		{
			Shape* CShape = (Shape*)(*Citem).second;
			for each(int id in CShape->FtFgRelationshapes)
			{
				if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
				{
					CShape->addFgNearShapes(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id]);
				}
			}
			for each(int id in CShape->siblingrelationships)
			{
			  if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
			  {
				 CShape->addSibling(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id]);
			  }
			}
		}
		ifile.close();
		//MessageBox(NULL, L"Completed Reading and adjusting shapes", L"Rapid-I", MB_TOPMOST);

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0005", __FILE__, __FUNCSIG__); }
}

//Edit the Program data..
void PartProgramFunctions::EditData(std::string filename, bool WorkSpaceRecovery)
{
	try
	{
		bool enteringFlag = false;
		bool isMCS = true;
		std::wstring buffer;
		std::wifstream ifile;
		ifile.open(filename.c_str());
		ifile >> buffer;
		ifile.precision(15);
		ifile >> buffer;
		//Read till end of the file..//
		while(!ifile.eof())
		{
			try
			{
				if(buffer ==  L"EndSelect")
					break; //Logical end of file reached

				//DRO Position
				if(buffer == L"CurrentDRO")
				{
					double vX, vY, vZ, vR;
					ifile >> vX;
					ifile >> vY;
					ifile >> vZ;
					ifile >> vR;
					SavedCurrentMachineDRO.set(vX, vY, vZ, vR);
				}
				else if(buffer == L"VblockCalibrationAxis")
				{
					bool flag = false;
					ifile >> flag;
					ProgramMadeUsingVblockAxis(flag);
					if(ProgramMadeUsingVblockAxis())
					{
						if(MAINDllOBJECT->Vblock_CylinderAxisLine == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
				}
				//Home Position for the probe...
				else if(buffer == L"ProbeHome")
				{
					double vX,vY,vZ;
					bool flag; int cn;
					ifile >> vX;
					ifile >> vY;
					ifile >> vZ;
					PPHomePosition.set(vX, vY, vZ);

					ifile >> flag;
					ProgramMadeUsingReferenceDot(flag);
					if(ProgramMadeUsingReferenceDot())
					{
						if(MAINDllOBJECT->ReferenceDotShape == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
					ifile >> flag; 
					FirstShapeisSetAsReferenceShape(flag); 
					ifile >> flag;
					ProgramMadeUsingTwoStepHoming(flag);
					ifile >> cn;
					ProgramMachineType = RapidEnums::RAPIDMACHINETYPE(cn);
					ifile >> cn;
					ProgramAngleMeasurementMode(cn);
					MAINDllOBJECT->AngleMeasurementMode(cn);
					ifile >> cn;
					ProgramMeasureUnitType = RapidEnums::RAPIDUNITS(cn);
					MAINDllOBJECT->MeasureUnitType = ProgramMeasureUnitType;
				}
				else if(buffer == L"ReferenceDotIstakenAsSecondHomePos")
				{
					bool flag = false;
					ifile >> flag;
					ProgramMadeUsingTwoReferenceDot(flag);
					if(ProgramMadeUsingTwoReferenceDot())
					{
						if(MAINDllOBJECT->SecondReferenceDotShape == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
				}
				else if(buffer == L"ReferenceLineAsRefAngle")
				{
					bool flag = false;
					ifile >> flag;
					this->ProgramMadeUsingRefLine(flag);
					ifile >> ReferenceLineAngle;
					if(this->ProgramMadeUsingRefLine())
					{
						if(MAINDllOBJECT->ReferenceLineShape == NULL)
						{
							ifile.close();
							MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail05", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							break;
						}
					}
				}
				else if(buffer == L"SecondReference")
				{
					double vX = 0, vY = 0, vZ = 0;
					ifile >> vX;
					ifile >> vY;
					ifile >> vZ;
					PPHomePosition_2nd.set(vX, vY, vZ);
				}
				//Read the UCS properties..//
				else if(buffer == L"UCS")
				{
					if(isMCS)
						isMCS = false;
					else
					{						
						UCS* ucs = new UCS(_T("pUCS"));
						ifile >> (*ucs);
						UCS& cucs = MAINDllOBJECT->getCurrentUCS();
						ucs->getWindow(0).init(cucs.getWindow(0));
						ucs->getWindow(1).init(cucs.getWindow(1));
						ucs->getWindow(2).init(cucs.getWindow(2));
						MAINDllOBJECT->AddingUCSModeFlag = true;
						AddUCSAction::addUCS(ucs);
						MAINDllOBJECT->AddingUCSModeFlag = false;
					}
				}
				//Read all shapes.. and its properties..//
				else if (buffer == L"shape")
				{

					int cn;	
					std::wstring cnstr;
					std::wstring skipcn;
					ifile >> cnstr;
					int ColonIndx = cnstr.find(':');
					if(ColonIndx != -1)
					{
					MAINDllOBJECT->IsOldPPFormat(false);
					std::wstring TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
					std::string Val(TagVal.begin(), TagVal.end() );
					cn=atoi((const char*)Val.c_str());
					}
					else
					{
						MAINDllOBJECT->IsOldPPFormat(true);
						std::string Val(cnstr.begin() , cnstr.end());
						cn=atoi((const char*)Val.c_str());
					}
					buffer = cn;
					RapidEnums::SHAPETYPE temp = RapidEnums::SHAPETYPE(cn);
					RapidEnums::CIRCLETYPE Ctype;
					Shape *item;
					enteringFlag = false;
					switch(temp)
					{
						case RapidEnums::SHAPETYPE::LINE:
							enteringFlag = true;
							item = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::LINE3D:
							enteringFlag = true;
							item = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XLINE3D:
							enteringFlag = true;
							item = new Line(_T("XL3D"), RapidEnums::SHAPETYPE::XLINE3D);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XLINE:
							enteringFlag = true;
							item = new Line(_T("XL"), RapidEnums::SHAPETYPE::XLINE);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::XRAY:
							enteringFlag = true;
							item = new Line(_T("R"), RapidEnums::SHAPETYPE::XRAY);
							ifile >> (*static_cast<Line*>(item));
							break;

						case RapidEnums::SHAPETYPE::CIRCLE:
							ifile >> cn;
							buffer = cn;
							Ctype = RapidEnums::CIRCLETYPE(cn);							
							ifile >> skipcn; buffer = skipcn;
							enteringFlag = true;
							if(Ctype == RapidEnums::CIRCLETYPE::PCDCIRCLE)
							{
								item = new PCDCircle(_T("PCD"));
								ifile >> (*static_cast<PCDCircle*>(item));
							}
							else
							{
								item = new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE);
								ifile >> (*static_cast<Circle*>(item));
							}
							break;

						case RapidEnums::SHAPETYPE::ARC:
							enteringFlag = true;
							item = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::TWOARC:
							enteringFlag = true;
							item = new TwoArc(_T("TAR"));
							ifile >> (*static_cast<TwoArc*>(item));
							break;

						case RapidEnums::SHAPETYPE::TWOLINE:
							enteringFlag = true;
							item = new TwoLine(_T("TL"));
							ifile >> (*static_cast<TwoLine*>(item));
							break;

						case RapidEnums::SHAPETYPE::RPOINT:
							enteringFlag = true;
							item = new RPoint(_T("P"), RapidEnums::SHAPETYPE::RPOINT);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RPOINT3D:
							enteringFlag = true;
							item = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RELATIVEPOINT:
							enteringFlag = true;
							item = new RPoint(_T("RP"), RapidEnums::SHAPETYPE::RELATIVEPOINT);
							ifile >> (*static_cast<RPoint*>(item));
							break;

						case RapidEnums::SHAPETYPE::RTEXT:
							enteringFlag = true;
							item = new RText(_T("T"));
							ifile >> (*static_cast<RText*>(item));
							break;

						case RapidEnums::SHAPETYPE::CLOUDPOINTS:
							enteringFlag = true;
							item = new CloudPoints(_T("CP"));
							ifile >> (*static_cast<CloudPoints*>(item));
							break;

						case RapidEnums::SHAPETYPE::DUMMY:
							enteringFlag = true;
							item = new DummyShape();
							ifile >> (*static_cast<DummyShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::DEPTHSHAPE:
							enteringFlag = true;
							item = new DepthShape();
							ifile >> (*static_cast<DepthShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::PERIMETER:
							enteringFlag = true;
							item = new Perimeter(_T("Peri"));
							ifile >> (*static_cast<Perimeter*>(item));
							break;

						case RapidEnums::SHAPETYPE::PLANE:
							enteringFlag = true;
							item = new Plane(_T("PL"));
							ifile >> (*static_cast<Plane*>(item));
							break;

						case RapidEnums::SHAPETYPE::CYLINDER:
							enteringFlag = true;
							item = new Cylinder(_T("CL"));
							ifile >> (*static_cast<Cylinder*>(item));
							break;

						case RapidEnums::SHAPETYPE::SPHERE:
							enteringFlag = true;
							item = new Sphere(_T("SH"));
							ifile >> (*static_cast<Sphere*>(item));
							break;

						case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE:
							enteringFlag = true;
							item = new IntersectionShape(_T("IS"));
							ifile >> (*static_cast<IntersectionShape*>(item));
							break;

						case RapidEnums::SHAPETYPE::CONE:
							enteringFlag = true;
							item = new Cone(_T("CN"));
							ifile >> (*static_cast<Cone*>(item));
							break;

						case RapidEnums::SHAPETYPE::CIRCLE3D:
							enteringFlag = true;
							item = new Circle(_T("C3D"),RapidEnums::SHAPETYPE::CIRCLE3D);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::ARC3D:
							enteringFlag = true;
							item = new Circle(_T("A3D"),RapidEnums::SHAPETYPE::ARC3D);
							ifile >> (*static_cast<Circle*>(item));
							break;

						case RapidEnums::SHAPETYPE::ELLIPSE3D:
							enteringFlag = true;
							item = new Conics3D(_T("EL"), RapidEnums::SHAPETYPE::ELLIPSE3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;	

						case RapidEnums::SHAPETYPE::PARABOLA3D:
							enteringFlag = true;
							item = new Conics3D(_T("PB"), RapidEnums::SHAPETYPE::PARABOLA3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;

						case RapidEnums::SHAPETYPE::HYPERBOLA3D:
							enteringFlag = true;
							item = new Conics3D(_T("HB"), RapidEnums::SHAPETYPE::HYPERBOLA3D);
							ifile >> (*static_cast<Conics3D*>(item));
							break;

						case RapidEnums::SHAPETYPE::SPLINE:
							enteringFlag = true;
							item = new Spline(_T("S"));
							ifile >> (*static_cast<Spline*>(item));
							break;

						case RapidEnums::SHAPETYPE::TORUS:
							enteringFlag = true;
							item = new Torus(_T("TR"));
							ifile >> (*static_cast<Torus*>(item));
							break;

						case RapidEnums::SHAPETYPE::AREA:
							enteringFlag = true;
							item = new AreaShape(_T("A"));
							ifile >> (*static_cast<AreaShape*>(item));
							break;
					}
					if(!MAINDllOBJECT->PPLoadSuccessful())
					{
						MAINDllOBJECT->PPLoadSuccessful(true);
						MAINDllOBJECT->clearAll();
						MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
						break;
					}
					if(enteringFlag)
					{
						item->UpdateEnclosedRectangle();
						if(item->AddActionWhileEdit())
							AddShapeAction::addShape(item, false);
						else
						{
							RCollectionBase& rshapes = MAINDllOBJECT->getUCS(0)->getShapes();
							rshapes.addItem(item);
						}
						if(item->ShapeType == RapidEnums::SHAPETYPE::PERIMETER)
							((Perimeter*)item)->SetParentChildRelation();
					}
				}
				//Read the all the measurements.... 
				else if(buffer == L"Measurement")
				{
					int cn;
					std::wstring cnstr;
					ifile >> cnstr;
					int ColonIndx = cnstr.find(':');
					if(ColonIndx != -1)
					{
					std::wstring TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
					std::string Val(TagVal.begin(), TagVal.end() );
					cn=atoi((const char*)Val.c_str());
					}
					else
					{
						std::string Val(cnstr.begin() , cnstr.end());
						cn=atoi((const char*)Val.c_str());
					}
					buffer = cn;
					RapidEnums::MEASUREMENTTYPE temp = RapidEnums::MEASUREMENTTYPE(cn);
					DimBase *item;
					enteringFlag = false;
					switch(temp)
					{
						case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
							enteringFlag = true;
							item = new DimPointCoordinate(_T("Center"));
							ifile >> (*static_cast<DimPointCoordinate*>(item));
							((DimPointCoordinate*)item)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
							enteringFlag = true;
							item = new DimOrdinate(_T("Pt_X"), temp);
							ifile >> (*static_cast<DimOrdinate*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
							enteringFlag = true;
							item = new DimOrdinate(_T("Pt_Y"), temp);
							ifile >> (*static_cast<DimOrdinate*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
							enteringFlag = true;
							item = new DimOrdinate(_T("Pt_Z"), temp);
							ifile >> (*static_cast<DimOrdinate*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE:
						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS:
							enteringFlag = true;
							item = new DimPointToPointDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToPointDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DIGITALMICROMETER:
							enteringFlag = true;
							item = new DimDigitalMicrometer(_T("duM"));
							ifile >> (*static_cast<DimDigitalMicrometer*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE:
							enteringFlag = true;
							item = new DimPointToLineDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE:
							enteringFlag = true;
							item = new DimPointToCircleDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToCircleDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE:
							case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINERAYANGLE:
							enteringFlag = true;
							item = new DimLineToLineDistance(true);
							ifile >> (*static_cast<DimLineToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE:
							enteringFlag = true;
							item = new DimLineToLineDistance(true);
							ifile >> (*static_cast<DimLineToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
							enteringFlag = true;
							item = new DimAngularity(_T("Par"), temp);
							ifile >> (*static_cast<DimAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
							enteringFlag = true;
							item = new DimAngularity(_T("Per"), temp);
							ifile >> (*static_cast<DimAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
							enteringFlag = true;
							item = new DimAngularity(_T("Anglty"), temp);
							ifile >> (*static_cast<DimAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH:
							enteringFlag = true;
							item = new DimLine(_T("Dist"), temp);
							ifile >> (*static_cast<DimLine*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS:
						case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED:
							{
							enteringFlag = true;
							item = new DimLine(_T("St"), temp);
							ifile >> (*static_cast<DimLine*>(item));
							break;
							}

						case RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE:
							enteringFlag = true;
							item = new DimLine(_T("Ang"), temp);
							ifile >> (*static_cast<DimLine*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOCIRCLEDISTANCE:
							enteringFlag = true;
							item = new DimCircleToCircleDistance(_T("Dist"));
							ifile >> (*static_cast<DimCircleToCircleDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOLINEDISTANCE:
							enteringFlag = true;
							item = new DimCircleToLineDistance(_T("Dist"));
							ifile >> (*static_cast<DimCircleToLineDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D:
							enteringFlag = true;
							item = new DimDiameter2D(_T("Dia"));
							ifile >> (*static_cast<DimDiameter2D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT:
							enteringFlag = true;
							item = new DimSplineMeasurement(_T("SpCt"));
							ifile >> (*static_cast<DimSplineMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMIC:
							enteringFlag = true;
							item = new DimSplineMeasurement(_T("SpMic"));
							ifile >> (*static_cast<DimSplineMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMCC:
							enteringFlag = true;
							item = new DimSplineMeasurement(_T("SpMcc"));
							ifile >> (*static_cast<DimSplineMeasurement*>(item));
							break;

						 case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE:
							enteringFlag = true;
							item = new DimGrooveMeasurement(_T("Ang"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE);
							ifile >> (*static_cast<DimGrooveMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH:
							enteringFlag = true;
							item = new DimGrooveMeasurement(_T("Dpt"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH);
							ifile >> (*static_cast<DimGrooveMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH:
							enteringFlag = true;
							item = new DimGrooveMeasurement(_T("Wth"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH);
							ifile >> (*static_cast<DimGrooveMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D:
							enteringFlag = true;
							item = new DimRadius2D(_T("Radius"));
							ifile >> (*static_cast<DimRadius2D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY:
							enteringFlag = true;
							item = new DimCircularity(_T("Circ"), temp);
							ifile >> (*static_cast<DimCircularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY:
						case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY_D:
							enteringFlag = true;
							item = new DimConcentricity(_T("Concen"));
							ifile >> (*static_cast<DimConcentricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC:
							enteringFlag = true;
							item = new DimCircularity(_T("Mic"), temp);
							ifile >> (*static_cast<DimCircularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC:
							enteringFlag = true;
							item = new DimCircularity(_T("Mcc"), temp);
							ifile >> (*static_cast<DimCircularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
							enteringFlag = true;
							item = new DimTruePosition(_T("TPG"), temp);
							ifile >> (*static_cast<DimTruePosition*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
							enteringFlag = true;
							item = new DimTruePosition(_T("TPX"), temp);
							ifile >> (*static_cast<DimTruePosition*>(item));
							break;
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
							enteringFlag = true;
							item = new DimTruePosition(_T("TPY"), temp);
							ifile >> (*static_cast<DimTruePosition*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
							enteringFlag = true;
							item = new DimTruePositionPoint(_T("TPG"), temp);
							ifile >> (*static_cast<DimTruePositionPoint*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
							enteringFlag = true;
							item = new DimTruePositionPoint(_T("TPX"), temp);
							ifile >> (*static_cast<DimTruePositionPoint*>(item));
							break;
						case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
							enteringFlag = true;
							item = new DimTruePositionPoint(_T("TPY"), temp);
							ifile >> (*static_cast<DimTruePositionPoint*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D:
							enteringFlag = true;
							item = new DimTruePosition3D(_T("TPG3D"), temp);
							ifile >> (*static_cast<DimTruePosition3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R:
						case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D:
							enteringFlag = true;
							item = new DimTruePosition3DPoint(_T("TPG3D"), temp);
							ifile >> (*static_cast<DimTruePosition3DPoint*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE:
							enteringFlag = true;
							item = new DimArcAngle(_T("ArcAngle"), temp);
							ifile >> (*static_cast<DimArcAngle*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH:
							enteringFlag = true;
							item = new DimArcAngle(_T("Arclen"), temp);
							ifile >> (*static_cast<DimArcAngle*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DEPTH:
							enteringFlag = true;
							item = new DimDepth(_T("Depth"));
							ifile >> (*static_cast<DimDepth*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_ROTARYANGLEMEASURE:
							enteringFlag = true;
							item = new DimRotaryAngleMeasure(_T("R_Ang"));
							ifile >> (*static_cast<DimRotaryAngleMeasure*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_THREAD:
							enteringFlag = true;
							item = new DimThread();
							ifile >> (*static_cast<DimThread*>(item));
							((DimThread*)item)->AddChildeMeasureToCollection(MAINDllOBJECT->getDimList());
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PCDOFFSET:
							enteringFlag = true;
							item = new DimPCDOffset();
							ifile >> (*static_cast<DimPCDOffset*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PCDANGLE:
							enteringFlag = true;
							item = new DimPCDAngle();
							ifile >> (*static_cast<DimPCDAngle*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D:
							enteringFlag = true;
							item = new DimPointToPointDistance3D(_T("Dist"));
							ifile >> (*static_cast<DimPointToPointDistance3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D:
							enteringFlag = true;
							item = new DimPointToLineDistance3D(_T("Dist"));
							ifile >> (*static_cast<DimPointToLineDistance3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE:
							enteringFlag = true;
							item = new DimPointToPlaneDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToPlaneDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimPointToCylinderDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimPointToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE3D:
							enteringFlag = true;
							item = new DimLineToLineDistance3D(false);
							ifile >> (*static_cast<DimLineToLineDistance3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOPLANEDISTANCE:
							enteringFlag = true;
							item = new DimLineToPlaneDistance(true);
							ifile >> (*static_cast<DimLineToPlaneDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimLineToCylinderDistance(true);
							ifile >> (*static_cast<DimLineToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimLineToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimLineToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOPLANEDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToPlaneDistance(true);
							ifile >> (*static_cast<DimPlaneToPlaneDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToCylinderDistance(true);
							ifile >> (*static_cast<DimPlaneToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimPlaneToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCYLINDERDISTANCE:
							enteringFlag = true;
							item = new DimCylinderToCylinderDistance(true);
							ifile >> (*static_cast<DimCylinderToCylinderDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimCylinderToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimCylinderToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOSPHEREDISTANCE:
							enteringFlag = true;
							item = new DimSphereToSphereDistance(_T("Dist"));
							ifile >> (*static_cast<DimSphereToSphereDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D:
							enteringFlag = true;
							item = new DimDiameter3D(_T("Dia"));
							ifile >> (*static_cast<DimDiameter3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS3D:
							enteringFlag = true;
							item = new DimRadius3D(_T("Radius"));
							ifile >> (*static_cast<DimRadius3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS:
						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED:
							enteringFlag = true;
							item = new DimLine3D(_T("Stl3d"), temp);
							ifile >> (*static_cast<DimLine3D*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
							enteringFlag = true;
							item = new DimLine3DAngularity(_T("l3dPar"), temp);
							ifile >> (*static_cast<DimLine3DAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
							enteringFlag = true;
							item = new DimLine3DAngularity(_T("l3dPer"), temp);
							ifile >> (*static_cast<DimLine3DAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
							enteringFlag = true;
							item = new DimLine3DAngularity(_T("l3dAng"), temp);
							ifile >> (*static_cast<DimLine3DAngularity*>(item));
							break;
					
						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY:
						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED:
							enteringFlag = true;
							item = new DimCylindricity(_T("Cyl"), temp);
							ifile >> (*static_cast<DimCylindricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC:
							enteringFlag = true;
							item = new DimCylindricity(_T("Cymic"), temp);
							ifile >> (*static_cast<DimCylindricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC:
							enteringFlag = true;
							item = new DimCylindricity(_T("Cymcc"), temp);
							ifile >> (*static_cast<DimCylindricity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylPar"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylPer"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylAng"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
							enteringFlag = true;
							item = new DimCylinderAngularity(_T("CylCoax"), temp);
							ifile >> (*static_cast<DimCylinderAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConPer"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConAng"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConCoax"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
							enteringFlag = true;
							item = new DimConeAngularity(_T("ConPar"), temp);
							ifile >> (*static_cast<DimConeAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS:
						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED:
							enteringFlag = true;
							item = new DimFlatness(_T("Flt"));
							ifile >> (*static_cast<DimFlatness*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
							enteringFlag = true;
							item = new DimPlaneAngularity(_T("PlPar"), temp);
							ifile >> (*static_cast<DimPlaneAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
							enteringFlag = true;
							item = new DimPlaneAngularity(_T("PlPer"), temp);
							ifile >> (*static_cast<DimPlaneAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
							enteringFlag = true;
							item = new DimPlaneAngularity(_T("PlAng"), temp);
							ifile >> (*static_cast<DimPlaneAngularity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY:
							enteringFlag = true;
							item = new DimSphericity(_T("Sph"), temp);
							ifile >> (*static_cast<DimSphericity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC:
							enteringFlag = true;
							item = new DimSphericity(_T("Sphmic"), temp);
							ifile >> (*static_cast<DimSphericity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC:
							enteringFlag = true;
							item = new DimSphericity(_T("Sphmcc"), temp);
							ifile >> (*static_cast<DimSphericity*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER:
							enteringFlag = true;
							item = new DimPerimeter_Diameter(_T("P_Dia"), temp);
							ifile >> (*static_cast<DimPerimeter_Diameter*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_PERIMETER:
							enteringFlag = true;
							item = new DimPerimeter_Diameter(_T("P_Peri"), temp);
							ifile >> (*static_cast<DimPerimeter_Diameter*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_AREA:
							enteringFlag = true;
							item = new DimPerimeter_Diameter(_T("P_Area"), temp);
							ifile >> (*static_cast<DimPerimeter_Diameter*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimCircle3DToCircle3DDistance(true);
							ifile >> (*static_cast<DimCircle3DToCircle3DDistance*>(item));
							break;

						
						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCONEDISTANCE:
							enteringFlag = true;
							item = new DimCircle3DToConeDistance(true);
							ifile >> (*static_cast<DimCircle3DToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGLE:
							enteringFlag = true;
							item = new DimAngle3D(_T("Ang"));
							ifile >> (*static_cast<DimAngle3D*>(item));
							break;
						
						case RapidEnums::MEASUREMENTTYPE::DIM_CONETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimConeToConeDistance(true);
							ifile >> (*static_cast<DimConeToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCONEDISTANCE:
							enteringFlag = true;
							item = new DimCylinderToConeDistance(true);
							ifile >> (*static_cast<DimCylinderToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimLineToCircle3DDistance(true);
							ifile >> (*static_cast<DimLineToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimLineToConeDistance(true);
							ifile >> (*static_cast<DimLineToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToCircle3DDistance(true);
							ifile >> (*static_cast<DimPlaneToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimPlaneToConeDistance(true);
							ifile >> (*static_cast<DimPlaneToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimPointToCircle3DDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE:
							enteringFlag = true;
							item = new DimPointToConeDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONICS3DDISTANCE:
							enteringFlag = true;
							item = new DimPointToConics3DDistance(_T("Dist"));
							ifile >> (*static_cast<DimPointToConics3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCIRCLE3DDISTANCE:
							enteringFlag = true;
							item = new DimSphereToCircle3DDistance(_T("Dist"));
							ifile >> (*static_cast<DimSphereToCircle3DDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCONEDISTANCE:
							enteringFlag = true;
							item = new DimSphereToConeDistance(_T("Dist"));
							ifile >> (*static_cast<DimSphereToConeDistance*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_AXISLENGTH:
							enteringFlag = true;
							item = new DimAxisLength(_T("Dist"));
							ifile >> (*static_cast<DimAxisLength*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE:
							enteringFlag = true;
							item = new DimCircleDeviationMeasure(_T("Dev"));
							ifile >> (*static_cast<DimCircleDeviationMeasure*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_CLOUDPOINTDEPTH:
							enteringFlag = true;
							item = new DimCloudPointDepth(_T("Dist"));
							ifile >> (*static_cast<DimCloudPointDepth*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIMTOTAL_LENGTH:
							enteringFlag = true;
							item = new DimTotalLength(_T("T_Per"));
							ifile >> (*static_cast<DimTotalLength*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT:
							enteringFlag = true;
							item = new DimAreaMeasurement(_T("Area"));
							ifile >> (*static_cast<DimAreaMeasurement*>(item));
							break;

						case RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTFARTHEST:
						case RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTNEAREST:
						case RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTFARTHEST:
						case RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTNEAREST:
						case RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER:				//measurement added to part program when editing....    09/02/2014
						case RapidEnums::MEASUREMENTTYPE::DIM_MICROMETRICCUTTINGDIAMETER:		    //vinod
						case RapidEnums::MEASUREMENTTYPE::DIM_DIST1:		    //vinod
						case RapidEnums::MEASUREMENTTYPE::DIM_DIST2:		    //vinod
							enteringFlag = true;
							item = new DimRunOutMeasurement(false);
							ifile >> (*static_cast<DimRunOutMeasurement*>(item));
							break;
					}
					if(!MAINDllOBJECT->PPLoadSuccessful())
					{
						MAINDllOBJECT->PPLoadSuccessful(true);
						MAINDllOBJECT->clearAll();
						MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
						break;
					}
					if(enteringFlag)
					{
						item->MeasurementType = temp;
						if(item->AddActionWhileEdit())
							AddDimAction::addDim(item);
						else
						{
							if(!item->ChildMeasurementType())
							{
								item->ParentPoint1 = MAINDllOBJECT->getVectorPointer(item->ParentPoint1);
								if(item->ParentPoint1->IntersectionPoint)
									((IsectPt*)item->ParentPoint1)->SetRelatedDim(item);
							}
							RCollectionBase& rmeasure = MAINDllOBJECT->getDimList();
							rmeasure.addItem(item);
							if(item->selected())
								MAINDllOBJECT->getSelectedDimList().addItem(item);
							MAINDllOBJECT->selectMeasurement(item->getId(), false);
							MAINDllOBJECT->Measurement_updated();
						}
					}
				}
				//Read all the actions..//
				else if(buffer == L"Action")
				{
					int cn;
					std::wstring cnstr;
					std::wstring skipcn;
					ifile >> cnstr;
					int ColonIndx = cnstr.find(':');
					if(ColonIndx != -1)
					{
					std::wstring TagVal = cnstr.substr(ColonIndx + 1, cnstr.length() - ColonIndx - 1);
					std::string Val(TagVal.begin(), TagVal.end() );
					cn=atoi((const char*)Val.c_str());
					}
					else
					{
						std::string Val(cnstr.begin() , cnstr.end());
						cn=atoi((const char*)Val.c_str());
					}
					buffer = cn;
					RapidEnums::ACTIONTYPE temp = RapidEnums::ACTIONTYPE(cn);
					RAction *item;
					enteringFlag = false;
					switch(temp)
					{
						case RapidEnums::ACTIONTYPE::ADDPOINTACTION:
							enteringFlag = true;
							item = new AddPointAction();
							ifile >> (*static_cast<AddPointAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION:
							enteringFlag = true;
							item = new AddThreadMeasureAction();
							ifile >> (*static_cast<AddThreadMeasureAction*>(item));
							break;
					
						case RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION:
							enteringFlag = true;
							item = new AddProbePathAction();
							ifile >> (*static_cast<AddProbePathAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::IMAGE_ACTION:
							enteringFlag = true;
							item = new AddImageAction();
							ifile >> (*static_cast<AddImageAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::COPYSHAPEACTION:
							enteringFlag = true;
							item = new CopyShapesAction();
							ifile >> (*static_cast<CopyShapesAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION:
							enteringFlag = true;
							item = new LocalisedCorrectionAction();
							ifile >> (*static_cast<LocalisedCorrectionAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::ROTATIONACTION:
							enteringFlag = true;
							item = new RotationAction();
							ifile >> (*static_cast<RotationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::REFLECTIONACTION:
							enteringFlag = true;
							item = new ReflectionAction();
							ifile >> (*static_cast<ReflectionAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::TRANSLATIONACTION:
							enteringFlag = true;
							item = new TranslationAction();
							ifile >> (*static_cast<TranslationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION:
							enteringFlag = true;
							item = new Cad1PtAlignAction();
							ifile >> (*static_cast<Cad1PtAlignAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION:
							enteringFlag = true;
							item = new Cad2PtAlignAction();
							ifile >> (*static_cast<Cad2PtAlignAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION:
							enteringFlag = true;
							item = new Cad1Pt1LineAction();
							ifile >> (*static_cast<Cad1Pt1LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION:
							enteringFlag = true;
							item = new Cad2LineAction();
							ifile >> (*static_cast<Cad2LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad1PtAction();
							ifile >> (*static_cast<EditCad1PtAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION:
							enteringFlag = true;
							item = new CircularInterPolationAction();
							ifile >> (*static_cast<CircularInterPolationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION:
							enteringFlag = true;
							item = new LinearInterPolationAction();
							ifile >> (*static_cast<LinearInterPolationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION:
							enteringFlag = true;
							item = new ProfileScanAction();
							ifile >> (*static_cast<ProfileScanAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::AREA_ACTION:
							enteringFlag = true;
							item = new AreaShapeAction();
							ifile >> (*static_cast<AreaShapeAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad2PtAction();
							ifile >> (*static_cast<EditCad2PtAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::AUTOALIGNACTION:
							enteringFlag = true;
							item = new AutoAlignAction();
							ifile >> (*static_cast<AutoAlignAction*>(item));
							break;
					
						case RapidEnums::ACTIONTYPE::DEVIATIONACTION:
							enteringFlag = true;
							item = new DeviationAction();
							ifile >> (*static_cast<DeviationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION:
							enteringFlag = true;
							item = new DXF_DeviationAction();
							ifile >> (*static_cast<DXF_DeviationAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION:
							enteringFlag = true;
							item = new CloudComparisionAction();
							ifile >> (*static_cast<CloudComparisionAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::MOVESHAPEACTION:
							enteringFlag = true;
							item = new MoveShapesAction();
							ifile >> (*static_cast<MoveShapesAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad2LineAction();
							ifile >> (*static_cast<EditCad2LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION:
							enteringFlag = true;
							item = new EditCad1Pt1LineAction();
							ifile >> (*static_cast<EditCad1Pt1LineAction*>(item));
							break;

						case RapidEnums::ACTIONTYPE::PERIMETERACTION:
							enteringFlag = true;
							item = new PerimeterAction();
							ifile >> (*static_cast<PerimeterAction*>(item));
							break;
						case RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION:
							enteringFlag = true;
							item = new AddShapePointsAction();
							ifile >> (*static_cast<AddShapePointsAction*>(item));
							break;

					}
					if(!MAINDllOBJECT->PPLoadSuccessful())
					{
						MAINDllOBJECT->PPLoadSuccessful(true);
						MAINDllOBJECT->clearAll();
						MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadFail02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, "PartProgramFunctionsloadFail02");
						break;
					}
					if(enteringFlag)
						MAINDllOBJECT->getActionsHistoryList().addItem(item);
				}
				ifile >> buffer;
			}
			catch(...)
			{ 
				MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0006", __FILE__, __FUNCSIG__);
			}
		}

		for(RC_ITER Citem = MAINDllOBJECT->getShapesList().getList().begin(); Citem != MAINDllOBJECT->getShapesList().getList().end(); Citem++)
		{
			Shape* CShape = (Shape*)(*Citem).second;
			for each(int id in CShape->FtFgRelationshapes)
			{
				if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
					CShape->addFgNearShapes(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id]);
			}
			for each(int id in CShape->siblingrelationships)
			{
				if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
					CShape->addSibling(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id]);
			}
		}
		ifile.close();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0007", __FILE__, __FUNCSIG__); }
}

//write the partprogram data..
void PartProgramFunctions::saveData(std::string filename, bool SaveForRecovery)
{
	try
	{
		this->ImageActionpresent = false;
		std::wofstream myfile;
		//Create the file with give name..//
		myfile.open(filename.c_str());
		//Set the precision of the double values in the file...//
		if(myfile.fail() && !SaveForRecovery)
		{
			 myfile.close();
			 MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramLoadProgram02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			 return;
		}
		myfile.precision(15);
		//Indicates the start of the program file..//
		myfile << L"start" << endl;

		//Check Whether Two Step Alignment is there...
		bool UsedTwoStepHoming = false;
		int ShapeCount = 0; Shape* s1;

		UCS& CurrentMcs =  (*(UCS*)(*MAINDllOBJECT->getUCSList().getList().begin()).second);
		for(RC_ITER Sitem = CurrentMcs.getShapes().getList().begin(); Sitem != CurrentMcs.getShapes().getList().end(); Sitem++)
		{
			Shape* Csh = (Shape*)(*Sitem).second;
			if(Csh->ShapeUsedForTwoStepHoming())
			{
				s1 = Csh;
				ShapeCount++;
			}
		}
		if(ShapeCount == 0)
			UsedTwoStepHoming = false;
		else if(ShapeCount == 1)
		{
			std::string Tempstr = (const char*)s1->getModifiedName();
			//MAINDllOBJECT->SetStatusCode("You have Selected Only Shape For two step Homing : " + Tempstr +  ". Please Select One More and build Again.. Other Wise it will not take in partprogram", 1);
			UsedTwoStepHoming = false;
		}
		else
		{
			UsedTwoStepHoming = true;
			//MAINDllOBJECT->SetStatusCode("Program is done using Two Step Homing. Please remember the manual Positions", 1);
		}

		myfile << endl << L"VblockCalibrationAxis" << endl;
		myfile << VblockAxisAsReference() << endl;

		myfile << endl << L"CurrentDRO" << endl;
		myfile << CurrentMachineDRO.getX() << endl;
		myfile << CurrentMachineDRO.getY() << endl;
		myfile << CurrentMachineDRO.getZ() << endl;
		myfile << CurrentMachineDRO.getR() << endl;

		
		//Home Position for the probe...
		myfile << endl << L"ProbeHome" << endl;
		myfile << HomePosition.getX() << endl;
		myfile << HomePosition.getY() << endl;
		myfile << HomePosition.getZ() << endl;

		

		myfile << ReferenceDotIstakenAsHomePosition() << endl;
		myfile << FirstShapeisSetAsReferenceShape() << endl;
		myfile << UsedTwoStepHoming << endl;

		myfile << MAINDllOBJECT->CurrentMahcineType << endl;
		myfile << MAINDllOBJECT->AngleMeasurementMode() << endl;
		myfile << MAINDllOBJECT->MeasureUnitType << endl;

		myfile << endl << L"ReferenceDotIstakenAsSecondHomePos" << endl;
		myfile << ReferenceDotIstakenAsSecondHomePos() << endl;

		myfile << endl << L"SecondReference" << endl;
		myfile << HomePosition_2nd.getX() << endl;
		myfile << HomePosition_2nd.getY() << endl;
		myfile << HomePosition_2nd.getZ() << endl;

		myfile << endl << L"ReferenceLineAsRefAngle" << endl;
		myfile << ReferenceLineAsRefAngle() << endl;
		if(MAINDllOBJECT->ReferenceLineShape != NULL)
			myfile << ((Line*)MAINDllOBJECT->ReferenceLineShape)->Angle() << endl;
		else
			myfile << "0" << endl;

		for(RC_ITER item = MAINDllOBJECT->getUCSList().getList().begin(); item != MAINDllOBJECT->getUCSList().getList().end(); item++)
		{
			UCS* MyUcs = (UCS*)((*item).second);
			myfile << endl << L"UCS" << endl;
			myfile << (*MyUcs);
			for(RC_ITER item1 = MyUcs->getShapes().getList().begin(); item1 != MyUcs->getShapes().getList().end(); item1++)
			{
				Shape* MyShape = (Shape*)((*item1).second);
				if(!MyShape->Normalshape() || MyShape->DoNotSaveInPP)
				{
					for each(BaseItem* act in ((ShapeWithList*)(MyShape))->PointAtionList)
					{
						((AddPointAction*)act)->ActionStatus(false);
					}
					continue;
				}
				//Add this Shape to the list of shapes available in Part Program
				ShapeIDList.push_back(MyShape->getId());

				switch(MyShape->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
				case RapidEnums::SHAPETYPE::LINE3D:
				case RapidEnums::SHAPETYPE::XLINE3D:
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XRAY:
					myfile << endl << L"shape" << endl;
					myfile << (*(Line*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CIRCLE:
					myfile << endl << L"shape" << endl;
					if(((Circle*)MyShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
					{
						myfile << (*(PCDCircle*)MyShape);
						myfile << endl;
					}
					else
					{
						myfile << (*(Circle*)MyShape);
						myfile << endl;
					}
					break;

				case RapidEnums::SHAPETYPE::ARC:
					myfile << endl << L"shape" << endl;
					myfile << (*(Circle*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::TWOARC:
					myfile << endl << L"shape" << endl;
					myfile << (*(TwoArc*)MyShape);
					myfile << endl;
					break;
			   
				case RapidEnums::SHAPETYPE::TWOLINE:
					myfile << endl << L"shape" << endl;
					myfile << (*(TwoLine*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::RELATIVEPOINT:
				case RapidEnums::SHAPETYPE::RPOINT:
				case RapidEnums::SHAPETYPE::RPOINT3D:
					myfile << endl << L"shape" << endl;
					myfile << (*(RPoint*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::RTEXT:
					myfile << endl << L"shape" << endl;
					myfile << (*(RText*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					myfile << endl << L"shape" << endl;
					myfile << (*(CloudPoints*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::DUMMY:
					myfile << endl << L"shape" << endl;
					myfile << (*(DummyShape*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::DEPTHSHAPE:
					myfile << endl << L"shape" << endl;
					myfile << (*(DepthShape*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::PERIMETER:
					myfile << endl << L"shape" << endl;
					myfile << (*(Perimeter*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::PLANE:
					myfile << endl << L"shape" << endl;
					myfile << (*(Plane*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CYLINDER:
					myfile << endl << L"shape" << endl;
					myfile << (*(Cylinder*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::SPHERE:
					myfile << endl << L"shape" << endl;
					myfile << (*(Sphere*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE:
					myfile << endl << L"shape" << endl;
					myfile << (*(IntersectionShape*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CONE:
					myfile << endl << L"shape" << endl;
					myfile << (*(Cone*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CIRCLE3D:
				case RapidEnums::SHAPETYPE::ARC3D:
					myfile << endl << L"shape" << endl;
					myfile << (*(Circle*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::ELLIPSE3D:
				case RapidEnums::SHAPETYPE::PARABOLA3D:
				case RapidEnums::SHAPETYPE::HYPERBOLA3D:
					myfile << endl << L"shape" << endl;
					myfile << (*(Conics3D*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::SPLINE:
					myfile << endl << L"shape" << endl;
					myfile << (*(Spline*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::TORUS:
					myfile << endl << L"shape" << endl;
					myfile << (*(Torus*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::AREA:
					myfile << endl << L"shape" << endl;
					myfile << (*(AreaShape*)MyShape);
					myfile << endl;
					break;
				default:
					break;
				}
			}
			for(RC_ITER item2 = MyUcs->getDimensions().getList().begin(); item2 != MyUcs->getDimensions().getList().end(); item2++)
			{
				DimBase* Cdim = (DimBase*)((*item2).second);
				if(!Cdim->IsValid()) continue;
				if(Cdim->ChildMeasurementType()) continue;
				if(Cdim->DoNotSaveInPP) continue;
				switch(Cdim->MeasurementType)
				{
					case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointCoordinate*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
					case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
					case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimOrdinate*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE:
					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToPointDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_DIGITALMICROMETER:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimDigitalMicrometer*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToLineDistance*)Cdim);
						myfile << endl;
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToCircleDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE:
						case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINERAYANGLE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLineToLineDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimAngularity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLine*)Cdim);
						myfile << endl;
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOCIRCLEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCircleToCircleDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLETOLINEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCircleToLineDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimDiameter2D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT:
					case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMIC:
					case RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMCC:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimSplineMeasurement*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE:
					case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH:
					case RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimGrooveMeasurement*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimRadius2D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC:
					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCircularity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY_D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimConcentricity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimTruePosition*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimTruePositionPoint*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R:
					case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimTruePosition3D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R:
					case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimTruePosition3DPoint*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE:
					case RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimArcAngle*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_DEPTH:
						if(Cdim->IsValid())
						{
							myfile << endl << L"Measurement" << endl;
							myfile << (*(DimDepth*)Cdim);
							myfile << endl;
						}
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_ROTARYANGLEMEASURE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimRotaryAngleMeasure*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_THREAD:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimThread*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PCDOFFSET:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPCDOffset*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PCDANGLE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPCDAngle*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToPointDistance3D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToLineDistance3D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToPlaneDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToCylinderDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOSPHEREDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToSphereDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE3D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLineToLineDistance3D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOPLANEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLineToPlaneDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCYLINDERDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLineToCylinderDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOSPHEREDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLineToSphereDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOPLANEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPlaneToPlaneDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCYLINDERDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPlaneToCylinderDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOSPHEREDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPlaneToSphereDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCYLINDERDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCylinderToCylinderDistance*)Cdim);
						myfile << endl;
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOSPHEREDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCylinderToSphereDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOSPHEREDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimSphereToSphereDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimDiameter3D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_RADIUS3D:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimRadius3D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLine3D*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLine3DAngularity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC:
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC:
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCylindricity*)Cdim);
						myfile << endl;
						break;
	
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCylinderAngularity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimConeAngularity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS:
					case RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimFlatness*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
					case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPlaneAngularity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY:
					case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC:
					case RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimSphericity*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER:
					case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_PERIMETER:
					case RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_AREA:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPerimeter_Diameter*)Cdim);
						myfile << endl;
						break;


					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCIRCLE3DDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCircle3DToCircle3DDistance*)Cdim);
						myfile << endl;
						break;

						
					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLE3DTOCONEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCircle3DToConeDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGLE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimAngle3D*)Cdim);
						myfile << endl;
						break;
						
					case RapidEnums::MEASUREMENTTYPE::DIM_CONETOCONEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimConeToConeDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERTOCONEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCylinderToConeDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCIRCLE3DDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLineToCircle3DDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_LINETOCONEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimLineToConeDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCIRCLE3DDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPlaneToCircle3DDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_PLANETOCONEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPlaneToConeDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToCircle3DDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToConeDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONICS3DDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimPointToConics3DDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCIRCLE3DDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimSphereToCircle3DDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_SPHERETOCONEDISTANCE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimSphereToConeDistance*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_AXISLENGTH:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimAxisLength*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCircleDeviationMeasure*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIM_CLOUDPOINTDEPTH:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimCloudPointDepth*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIMTOTAL_LENGTH:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimTotalLength*)Cdim);
						myfile << endl;
						break;

					case RapidEnums::MEASUREMENTTYPE::DIMAREAMEASUREMENT:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimAreaMeasurement*)Cdim);
						myfile << endl;
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTFARTHEST:
					case RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTNEAREST:
					case RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTFARTHEST:
					case RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTNEAREST:
					case RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER:						//measurement added to part program when writing....    09/02/2014
					case RapidEnums::MEASUREMENTTYPE::DIM_MICROMETRICCUTTINGDIAMETER:					//vinod radial runouts....
					case RapidEnums::MEASUREMENTTYPE::DIM_DIST1:		    //vinod
					case RapidEnums::MEASUREMENTTYPE::DIM_DIST2:		    //vinod
					case RapidEnums::MEASUREMENTTYPE::DIM_AXIALRUNOUT:
						myfile << endl << L"Measurement" << endl;
						myfile << (*(DimRunOutMeasurement*)Cdim);
						myfile << endl;
						break;
				}
			}
		}
		if(DXFEXPOSEOBJECT->CurrentDXFNonEditedActionList.size() > 0)
		{
			for(list<Shape*>::iterator It = DXFEXPOSEOBJECT->CurrentDXFShapeList.begin(); It != DXFEXPOSEOBJECT->CurrentDXFShapeList.end(); It++)
			{
				Shape* MyShape = (Shape*)(*It);
				switch(MyShape->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
				case RapidEnums::SHAPETYPE::LINE3D:
				case RapidEnums::SHAPETYPE::XLINE3D:
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XRAY:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Line*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CIRCLE:
					myfile << endl << L"dxfshape" << endl;
					if(((Circle*)MyShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
					{
						myfile << (*(PCDCircle*)MyShape);
						myfile << endl;
					}
					else
					{
						myfile << (*(Circle*)MyShape);
						myfile << endl;
					}
					break;

				case RapidEnums::SHAPETYPE::ARC:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Circle*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::TWOARC:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(TwoArc*)MyShape);
					myfile << endl;
					break;
			   
				case RapidEnums::SHAPETYPE::TWOLINE:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(TwoLine*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::RELATIVEPOINT:
				case RapidEnums::SHAPETYPE::RPOINT:
				case RapidEnums::SHAPETYPE::RPOINT3D:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(RPoint*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::RTEXT:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(RText*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(CloudPoints*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::DUMMY:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(DummyShape*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::DEPTHSHAPE:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(DepthShape*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::PERIMETER:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Perimeter*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::PLANE:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Plane*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CYLINDER:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Cylinder*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::SPHERE:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Sphere*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(IntersectionShape*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CONE:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Cone*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::CIRCLE3D:
				case RapidEnums::SHAPETYPE::ARC3D:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Circle*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::ELLIPSE3D:
				case RapidEnums::SHAPETYPE::PARABOLA3D:
				case RapidEnums::SHAPETYPE::HYPERBOLA3D:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Conics3D*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::SPLINE:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Spline*)MyShape);
					myfile << endl;
					break;

				case RapidEnums::SHAPETYPE::TORUS:
					myfile << endl << L"dxfshape" << endl;
					myfile << (*(Torus*)MyShape);
					myfile << endl;
					break;
				}
			}
		}

		for(RC_ITER item3 = MAINDllOBJECT->getActionsHistoryList().getList().begin(); item3 != MAINDllOBJECT->getActionsHistoryList().getList().end(); item3++)
		{
			RAction* MyAction = (RAction*)((*item3).second);
			if(!MyAction->ActionStatus()) continue;
			if(MyAction->DontSaveInPartProgram()) continue;
			switch(MyAction->CurrentActionType)
			{
			case RapidEnums::ACTIONTYPE::ADDPOINTACTION:
				//int thisID = ;
				//Check if this shape is present...
				if (std::find(ShapeIDList.begin(), ShapeIDList.end(), ((AddPointAction*)MyAction)->getShape()->getId()) != ShapeIDList.end())
				{
					/* This Shape is present. So we can add this to PP */
					myfile << (*(AddPointAction*)MyAction);
					myfile << endl;
				}
				//else
				//	continue;

				break;

			case RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION:
				myfile << (*(AddThreadMeasureAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION:
				myfile << (*(AddProbePathAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::IMAGE_ACTION:
				this->ImageActionpresent = true;
				myfile << (*(AddImageAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::COPYSHAPEACTION:
				myfile << (*(CopyShapesAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::ROTATIONACTION:
				myfile << (*(RotationAction*)MyAction);
				myfile << endl;
				break;
				
			case RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION:
				myfile << (*(LocalisedCorrectionAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::REFLECTIONACTION:
				myfile << (*(ReflectionAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::TRANSLATIONACTION:
				myfile << (*(TranslationAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION:
				myfile << (*(EditCad1PtAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION:
				myfile << (*(Cad1PtAlignAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION:
				myfile << (*(Cad2PtAlignAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION:
				myfile << (*(Cad1Pt1LineAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION:
				myfile << (*(Cad2LineAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION:
				myfile << (*(CircularInterPolationAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION:
				myfile << (*(LinearInterPolationAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION:
				myfile << (*(ProfileScanAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::AREA_ACTION:
				myfile << (*(AreaShapeAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION:
				myfile << (*(EditCad2PtAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::AUTOALIGNACTION:
				myfile << (*(AutoAlignAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::DEVIATIONACTION:
				myfile << (*(DeviationAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION:
				myfile << (*(DXF_DeviationAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION:
				myfile << (*(CloudComparisionAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::MOVESHAPEACTION:
				myfile << (*(MoveShapesAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION:
				myfile << (*(EditCad2LineAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION:
				myfile << (*(EditCad1Pt1LineAction*)MyAction);
				myfile << endl;
				break;

			case RapidEnums::ACTIONTYPE::PERIMETERACTION:
				myfile << (*(PerimeterAction*)MyAction);
				myfile << endl;
				break;
			case RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION:
				myfile << (*(AddShapePointsAction*)MyAction);
				myfile << endl;
				break;

			//case RapidEnums::ACTIONTYPE::ADDUCSACTION:
			//	myfile << (*(AddUCSAction*)MyAction);
			//	myfile << endl;
			//	break;

			//case RapidEnums::ACTIONTYPE::CHANGEUCSACTION:
			//	myfile << (*(ChangeUCSAction*)MyAction);
			//	myfile << endl;
			//	break;
			}
		}
		//End of the file..//
		myfile << L"EndSelect";
		myfile.close();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0008", __FILE__, __FUNCSIG__); }
}

//Build partprogram...
void PartProgramFunctions::buildpp(std::string path, bool SaveForRecovery)
{
	try
	{
		if (MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::LINEARCFG_HANDLER)
			MAINDllOBJECT->setHandler(new DefaultHandler());
		this->saveData(path.c_str(), SaveForRecovery); 
		if(!SaveForRecovery && this->ImageActionpresent)
		{
			CreateRectangleExtentForImage();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0009", __FILE__, __FUNCSIG__); }
}

bool PartProgramFunctions::CreateProfileLineArcFrameGrabs()
{
	try
	{
		map<int, AddPointAction*> linearc_pointactions;
		int k = 0;
		bool prev_pointaction_exists = false;
		bool linearcaction_exists = false;
		ShapeWithList* cshape;
		FramegrabBase* cfgb;
		AddPointAction* action;
		for(RC_ITER ActItem = MAINDllOBJECT->getActionsHistoryList().getList().begin(); ActItem != MAINDllOBJECT->getActionsHistoryList().getList().end(); ActItem++)
		{
			RAction* CurrentAction = (RAction*)((*ActItem).second);
			if(!CurrentAction->ActionStatus()) continue;
			if(CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDLINEARCPFACTION)
			{
				linearcaction_exists = true;
				if(!((LineArcAction*)CurrentAction)->FramegrabactionAdded)
				{
					((LineArcAction*)CurrentAction)->determineFrameGrabs();
					//need to create below linearc_pointactions collection only if prev_pointaction_exists is false.  else it is not used.
					if(!prev_pointaction_exists)
					{
						for(int i = 0; i < (((LineArcAction*)CurrentAction)->FrameGrabCollection).size(); i++)
						{
							cshape = (ShapeWithList*) ((LineArcAction*)CurrentAction)->fgTempCollection[i];							
							for(int j = 0; j < ((((LineArcAction*)CurrentAction)->FrameGrabCollection)[i]).size(); j++)
							{
								cfgb = ((((LineArcAction*)CurrentAction)->FrameGrabCollection)[i])[j];
								action = new AddPointAction(cshape, cfgb);
								linearc_pointactions[k] = action;
								k++;
							}							
						}
					}
				}
			}
			if(CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION || CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION 
				|| CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION || CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION
				|| CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION || CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				prev_pointaction_exists = true;
			}
		}
		
		if ((!prev_pointaction_exists) && (linearcaction_exists))
		{
			MAINDllOBJECT->Derivedmodeflag = true;
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
			MAINDllOBJECT->setHandler(new LineArcFGHandler(linearc_pointactions));
			return false;
		}
		else
		{
			for(RC_ITER ActItem = MAINDllOBJECT->getActionsHistoryList().getList().begin(); ActItem != MAINDllOBJECT->getActionsHistoryList().getList().end(); ActItem++)
			{
				RAction* CurrentAction = (RAction*)((*ActItem).second);
				if(!CurrentAction->ActionStatus()) continue;
				if(CurrentAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDLINEARCPFACTION)
				{
					if(!((LineArcAction*)CurrentAction)->FramegrabactionAdded)
					{
						((LineArcAction*)CurrentAction)->addLineArcAction();
						((LineArcAction*)CurrentAction)->FramegrabactionAdded = true;
					}
				}
			}
			int z = linearc_pointactions.size();
			for (int i = 0; i < z; i++)
			{
				action = linearc_pointactions[z - i];
				linearc_pointactions.erase(z - i);
				delete action;
			}
			return true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0009", __FILE__, __FUNCSIG__); return false;}
}

//Edit partprogram.
void PartProgramFunctions::Editpp(std::string path, bool WorkSpaceRecovery)
{
	try
	{
		this->PPAlignEditFlag(true);
		this->PartprogramisLoadingEditMode(true);
		MAINDllOBJECT->dontUpdateGraphcis = true;
		this->EditData(path.c_str(), WorkSpaceRecovery);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->updateAll();
		CopyAllShapesfromPP();
		//Added by Rahul on 5 may 12.........
		int Cucsid = MAINDllOBJECT->getCurrentUCS().getId();
		if(Cucsid != 0)
			MAINDllOBJECT->MCStoUCS(MAINDllOBJECT->getCurrentUCS());
		this->PartprogramisLoadingEditMode(false);
		if(ReferenceDotIstakenAsHomePosition())
			ShiftShape_ForReferencedot_PPEdit();
	}
	catch(...){ this->PartprogramisLoadingEditMode(false); this->PPAlignEditFlag(false); MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0010", __FILE__, __FUNCSIG__);} 
}

//Load partprogram..
void PartProgramFunctions::loadpp(std::string path)
{
	try
	{
		//MessageBox(NULL, L"Started Loading PP", L"Rapid-I", MB_TOPMOST);
		this->PartprogramisLoading(true);
		MAINDllOBJECT->dontUpdateGraphcis = true;
		this->ProgramDoneForIdorOdMeasurement(false);
		this->DrawPartprogramArrow(false);
		this->StartingPointofPartprogram(false);
		this->IsPartProgramRunning(false);
		this->UpdateFistFramegrab(false);
		this->PPActionlist.clearAll();
		this->PPShortestPathActionlist.clearAll();
		this->BasePlaneWithFrameGrab(false);
		this->loadData(path.c_str());
		bool enteringflag = false;
		this->FirstMousedown_Partprogram(false);
		this->FirstMousedown_Partprogram_Manual(false);
		this->PartProgramAlignmentIsDone(false);

		//MessageBox(NULL, L"Finished init variables and loading file", L"Rapid-I", MB_TOPMOST);

		if(this->ProgramMadeUsingTwoStepHoming())
		{
			for(ppCurrentaction = getPPActionlist().getList().begin(); ppCurrentaction != getPPActionlist().getList().end(); ppCurrentaction++)
			{
				if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					FramegrabBase* Cfgb = ((AddPointAction*)(*ppCurrentaction).second)->getFramegrab();
					if(Cfgb->ActionForTwoStepHoming())
					{
						AssignFirstFGAction();
						break;
					}
				}
			}
		}
		else
		{
			for(ppCurrentaction = getPPActionlist().getList().begin(); ppCurrentaction != getPPActionlist().getList().end(); ppCurrentaction++)
			{
				if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					AssignFirstFGAction();
					break;
				}
				else if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
				{
					AssignFirstFGAction(false);
					break;
				}
				else if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
				{
					AssignFirstFGAction(false);
					break;
				}
				else if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
				{
					ppFirstgrab->PointDRO.set(*((ProfileScanAction*)((RAction*)(*ppCurrentaction).second))->GetCurrentPosition());
					InitilizePointPP = MAINDllOBJECT->getCurrentDRO();
					break;
				}
				else if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
				{
					RCollectionBase* Coll = &((AddThreadMeasureAction*)(*ppCurrentaction).second)->ActionCollection;
					AddThreadPointAction* THAction = (AddThreadPointAction*)(*Coll->getList().begin()).second;
					((AddThreadMeasureAction*)(*ppCurrentaction).second)->ThreadppCurrentaction = Coll->getList().begin();
					bool flag = SetFirstFrameGrabHandler(THAction->getFramegrab()->FGtype);
					if(flag)
					{
						CopyFirstActionProperties(THAction->getFramegrab());
						((FrameGrabHandler*)MAINDllOBJECT->getCurrentHandler())->setDrawFG(ppFirstgrab);
						UpdateFistFramegrab(true);
						THREADCALCOBJECT->ThreadMeasurementMode(true);
						break;
					}
				}
			}
		}
		//MessageBox(NULL, L"Assigned first FG Task", L"Rapid-I", MB_TOPMOST);
		
		bool checkflag = true;
		for(ppCurrentaction = getPPActionlist().getList().begin(); ppCurrentaction != getPPActionlist().getList().end(); ppCurrentaction++)
		{
			if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* Cfgb = ((AddPointAction*)(*ppCurrentaction).second)->getFramegrab();
				if(Cfgb->FGtype != RapidEnums::RAPIDFGACTIONTYPE::ODMEASUREMENT && Cfgb->FGtype != RapidEnums::RAPIDFGACTIONTYPE::IDMEASUREMENT)
					checkflag = false;
			}
		}
		if(checkflag)
			ProgramDoneForIdorOdMeasurement(true);
		//Copy all shapes to current UCS..//
		CopyAllShapesfromPP();
		MAINDllOBJECT->dontUpdateGraphcis = false;
		//MessageBox(NULL, L"Finished Copying shapes", L"Rapid-I", MB_TOPMOST);

		MAINDllOBJECT->getActionsHistoryList().clear();
		this->PartprogramisLoading(false);
		//Update the partprogram..//
		MAINDllOBJECT->updatePPGraphics(true);
		MAINDllOBJECT->UpdateVideoViewPortSettings();
		int Cucsid = MAINDllOBJECT->getCurrentUCS().getId();
		if(Cucsid != 0)
			MAINDllOBJECT->MCStoUCS(MAINDllOBJECT->getCurrentUCS());
		Sleep(20);
		//
		////Fit the Shapes to the Extent of the RapidCAD window.. Included on 07 May 2011 As per narayanan sir requirement..!
		//list<Shape*> CurrentPPShapeList;
		//for(RC_ITER i = getCurrentUCS().getShapes().getList().begin(); i != getCurrentUCS().getShapes().getList().end(); i++)
		//{
		//	Shape* sh = (Shape*)(*i).second;
		//	CurrentPPShapeList.push_back(sh);
		//}
		//if(CurrentPPShapeList.size() > 0)
		//{
		//	getWindow(1).ResetPan();
		//	double lefttop[2], rightbottom[2];
		//	HELPEROBJECT->GetSelectedShapeExtents(CurrentPPShapeList, &lefttop[0], &rightbottom[0]);
		//	double ext[4];
		//	getWindow(1).getExtents(&ext[0]);
		//	double Temparea = abs((lefttop[0] - rightbottom[0]) /(ext[0] - ext[1]));
		//	double Temparea1 = abs((lefttop[1] - rightbottom[1])/(ext[2] - ext[3]));
		//	double Tempavg = (Temparea + Temparea1)/2;
		//	double uppr = getWindow(1).getUppX() * Tempavg;
		//	getWindow(1).changeZoom(uppr);
		//	CurrentPPShapeList.clear();
		//}
		//currentwindow = 1;
		//getWindow(1).changeZoom(true);
		//
		//
		ReferencePtAsHomePos(false);
		if(ProgramMadeUsingReferenceDot())
			ReferencePtAsHomePos(true);
		MAINDllOBJECT->SetStatusCode("PP0001");
		if(!ReferenceDotIstakenAsHomePosition())
		{
			if(ProgramMadeUsingFirstShapeAsReference())
			{
				ReferencePtAsHomePos(true);
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgram_Run02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			}
		
			if(ProgramMadeUsingReferenceDot() && !ProgramMadeUsingFirstShapeAsReference())
			{
				MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctionsloadpp01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			}
		}
		//MessageBox(NULL, L"Setup init status for PP", L"Rapid-I", MB_TOPMOST);

		CheckForFrameGrab();
	}
	catch(...){ PartprogramisLoading(false); MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0011", __FILE__, __FUNCSIG__); }
}

std::string PartProgramFunctions::getFeatureList()
{
	try
	{
		std::string RetStr = "";
		//Writing features used list to the file
		map<int, bool> FeatureList;
		for(int i = 0; i < NoOfFeatures(); i++)
			FeatureList[i] = false;
		
		for(RC_ITER Aitem = getPPActionlist().getList().begin(); Aitem != getPPActionlist().getList().end(); Aitem++)
		{
			RAction* action = (RAction*)(*Aitem).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				if(((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
				{
					//Touch Probe related
					FeatureList[1] = true;
				}
				else if(((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG ||
					((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN)
				{
					//2D Contour Scan related
					FeatureList[3] = true;
				}
				else if (((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN)
				{
					//3D Scan related
					FeatureList[5] = true;
				}
				else if(((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH ||
					((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO ||
					((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX ||
					((AddPointAction*)action)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
				{
					//Auto-Focus & CNC Related...
					FeatureList[6] = true;
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				//Touch Probe related
				FeatureList[1] = true;
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::AUTOALIGNACTION)
			{
				//Dxf related
				FeatureList[2] = true;
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION ||
				action->CurrentActionType == RapidEnums::ACTIONTYPE::DEVIATIONACTION)
			{
				//Dxf Comparator related
				FeatureList[4] = true;
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION || 
				action->CurrentActionType == RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION)
			{
				//Profile Scan related
				FeatureList[9] = true;
			}
		}
		
		for(int i = 0; i < NoOfFeatures(); i++)
			if(FeatureList[i])
				RetStr += "1";
			else
				RetStr += "0";
		return RetStr;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0011", __FILE__, __FUNCSIG__); return ""; }
}

bool PartProgramFunctions::checkToleranceLimitForNogoMeasurement(int MeasureId)
{
	try
	{
		for(RC_ITER MeasureItr = MAINDllOBJECT->getDimList().getList().begin(); MeasureItr != MAINDllOBJECT->getDimList().getList().end(); MeasureItr++)
		{
			DimBase* Cdim = (DimBase*)(*MeasureItr).second;
			if(MeasureId == Cdim->getId())
			{
				if(Cdim->MeasurementUsedForPPAbort())
				{
					if((Cdim->getDimension() > Cdim->NominalValue() + Cdim->UpperTolerance()) || (Cdim->getDimension() < Cdim->NominalValue() + Cdim->LowerTolerance()))
					{
						AbortPartProgram();
						return true;
					}
				}
			}		
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC009a", __FILE__, __FUNCSIG__); return false; } 
}

bool PartProgramFunctions::checkToleranceLimitForCriticalShape(int ShapeId)
{
	try
	{
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getShapes().getList().begin(); Item_Shape != Cucs->getShapes().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->getId() == ShapeId)
				{
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape->ShapeType == RapidEnums::SHAPETYPE::XRAY || Cshape->ShapeType == RapidEnums::SHAPETYPE::XLINE)
					{
						double angle = ((Line*)Cshape)->BuildAngle(), angle1 = ((Line*)Cshape)->Angle();
						if(angle > M_PI_2)
							angle -= M_PI;
						else if(angle < -M_PI_2)
							angle += M_PI;
						if(angle1 > M_PI_2)
							angle1 -= M_PI;
						else if(angle1 < -M_PI_2)
							angle1 += M_PI;
						if(abs(angle - angle1) > abs(MAINDllOBJECT->AngleTolerance))
						{
							AbortPartProgram();
							return true;
						}
						else
							break;					
					}
					else if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC3D || Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						if(abs(((Circle*)Cshape)->BuildRadius() - ((Circle*)Cshape)->Radius()) > abs(((Circle*)Cshape)->BuildRadius() * MAINDllOBJECT->PercentageTolerance / 100))
						{
							AbortPartProgram();
							return true;
						}
						else
							return false;
					}
				}
			}
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC009b", __FILE__, __FUNCSIG__); return false; } 
}

void PartProgramFunctions::getLastActionIdForOffsetCorrection()
{
	try
	{
		LastActionIdList_OffsetAction.clear();
		std::list<int> ActionIdList_OffsetShape;
		
		std::list<int> parentActionIdList;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* CShape = (Shape*)((*Item_shape).second);
				if(!CShape->Normalshape()) continue;
				if((!CShape->XOffsetCorrection()) && (!CShape->YOffsetCorrection()) && (!CShape->ZOffsetCorrection()) && (!CShape->ROffsetCorrection())) continue;
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT || CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
				{
					if(((RPoint*)CShape)->PointType == RPoint::RPOINTTYPE::PROJECTEDPOINT3D || ((RPoint*)CShape)->PointType == RPoint::RPOINTTYPE::INTERSECTIONPOINT3D)
					{
						if(CShape->getParents().size() > 0)
						{
							std::list<int> parentShapeIdList;
							for(list<BaseItem*>::iterator ptr = CShape->getParents().begin(); ptr != CShape->getParents().end(); ptr++)
							{
								parentShapeIdList.push_back(((Shape*)*ptr)->getId());
							}
							std::list<int> ActionIdList;
							for each(int ShpId in parentShapeIdList)
							{		
								for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
								{
									if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
									{
										int ShapeID = ((AddPointAction*)(*ActionListItr).second)->getShape()->getId();
										if(ShapeID == ShpId)
										{
											ActionIdList.push_back(((RAction*)(*ActionListItr).second)->getId());
										}
									}
								}
							}
							int LastId = 0;
							for each(int ActId in ActionIdList)
							{
								if(ActId > LastId)
									LastId = ActId;
							}
							parentActionIdList.push_back(LastId);
						}
					}
					else
						ActionIdList_OffsetShape.push_back(CShape->getId());
				}
				else
					ActionIdList_OffsetShape.push_back(CShape->getId());
			}
		}

		for each(int ShpId in ActionIdList_OffsetShape)
		{
			std::list<int> ActionIdList;		
			for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
			{
				if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					int ShapeID = ((AddPointAction*)(*ActionListItr).second)->getShape()->getId();
					if(ShapeID == ShpId)
					{
						ActionIdList.push_back(((RAction*)(*ActionListItr).second)->getId());
					}
				}
			}
			int LastActionIdForOffsetAction = 0;
			for each(int ActID in ActionIdList)
			{
				if(LastActionIdForOffsetAction == 0)
					LastActionIdForOffsetAction = ActID;
				else if(LastActionIdForOffsetAction < ActID)
					LastActionIdForOffsetAction = ActID;
			}
			if(LastActionIdForOffsetAction != 0)
				LastActionIdList_OffsetAction.push_back(LastActionIdForOffsetAction);
		}
		for each(int ActId in parentActionIdList)
			LastActionIdList_OffsetAction.push_back(ActId);

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC009aa", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::StoreOriginalActionForNextRun(RCollectionBase* PPCollAction)
{
	try
	{
		for(RC_ITER Item_PPAction = PPCollAction->getList().begin(); Item_PPAction != PPCollAction->getList().end(); Item_PPAction++)
		{
			RAction* Caction = (RAction*)(*Item_PPAction).second;
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)Caction)->getFramegrab();
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_PointDRO.set(fb->PointDRO.getX(), fb->PointDRO.getY(), fb->PointDRO.getZ(), fb->PointDRO.getR(), fb->PointDRO.getP());
				ActionColl->AP_Points[0].set(fb->points[0].getX(), fb->points[0].getY(), fb->points[0].getZ(), fb->points[0].getR(), fb->points[0].getP());
				ActionColl->AP_Points[1].set(fb->points[1].getX(), fb->points[1].getY(), fb->points[1].getZ(), fb->points[1].getR(), fb->points[1].getP());
				ActionColl->AP_Points[2].set(fb->points[2].getX(), fb->points[2].getY(), fb->points[2].getZ(), fb->points[2].getR(), fb->points[2].getP());
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)Caction)->getFramegrab();
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_PointDRO.set(fb->PointDRO.getX(), fb->PointDRO.getY(), fb->PointDRO.getZ(), fb->PointDRO.getR(), fb->PointDRO.getP());
				ActionColl->AP_Points[0].set(fb->points[0].getX(), fb->points[0].getY(), fb->points[0].getZ(), fb->points[0].getR(), fb->points[0].getP());
				ActionColl->AP_Points[1].set(fb->points[1].getX(), fb->points[1].getY(), fb->points[1].getZ(), fb->points[1].getR(), fb->points[1].getP());
				ActionColl->AP_Points[2].set(fb->points[2].getX(), fb->points[2].getY(), fb->points[2].getZ(), fb->points[2].getR(), fb->points[2].getP());
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)Caction)->getFramegrab();
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_PointDRO.set(fb->PointDRO.getX(), fb->PointDRO.getY(), fb->PointDRO.getZ(), fb->PointDRO.getR(), fb->PointDRO.getP());
				ActionColl->AP_Points[0].set(fb->points[0].getX(), fb->points[0].getY(), fb->points[0].getZ(), fb->points[0].getR(), fb->points[0].getP());
				ActionColl->AP_Points[1].set(fb->points[1].getX(), fb->points[1].getY(), fb->points[1].getZ(), fb->points[1].getR(), fb->points[1].getP());
				ActionColl->AP_Points[2].set(fb->points[2].getX(), fb->points[2].getY(), fb->points[2].getZ(), fb->points[2].getR(), fb->points[2].getP());
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)Caction)->getFramegrab();
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_PointDRO.set(fb->PointDRO.getX(), fb->PointDRO.getY(), fb->PointDRO.getZ(), fb->PointDRO.getR(), fb->PointDRO.getP());
				ActionColl->AP_Points[0].set(fb->points[0].getX(), fb->points[0].getY(), fb->points[0].getZ(), fb->points[0].getR(), fb->points[0].getP());
				ActionColl->AP_Points[1].set(fb->points[1].getX(), fb->points[1].getY(), fb->points[1].getZ(), fb->points[1].getR(), fb->points[1].getP());
				ActionColl->AP_Points[2].set(fb->points[2].getX(), fb->points[2].getY(), fb->points[2].getZ(), fb->points[2].getR(), fb->points[2].getP());
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				RCollectionBase* THAction = &((AddThreadMeasureAction*)Caction)->ActionCollection;
				for(RC_ITER Item_PPAction1 = THAction->getList().begin(); Item_PPAction1 != THAction->getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					ActionsPointCollection* ActionColl = new ActionsPointCollection();
					ActionColl->AP_PointDRO.set(fb->PointDRO.getX(), fb->PointDRO.getY(), fb->PointDRO.getZ(), fb->PointDRO.getR(), fb->PointDRO.getP());
					ActionColl->AP_Points[0].set(fb->points[0].getX(), fb->points[0].getY(), fb->points[0].getZ(), fb->points[0].getR(), fb->points[0].getP());
					ActionColl->AP_Points[1].set(fb->points[1].getX(), fb->points[1].getY(), fb->points[1].getZ(), fb->points[1].getR(), fb->points[1].getP());
					ActionColl->AP_Points[2].set(fb->points[2].getX(), fb->points[2].getY(), fb->points[2].getZ(), fb->points[2].getR(), fb->points[2].getP());
					ActionColl->AP_Id = CthAction->getId();
					PPActionPositionColl[CthAction->getId()] = ActionColl;
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((CircularInterPolationAction*)Caction)->GetCenter();
				ActionColl->AP_Points[1] = *((CircularInterPolationAction*)Caction)->GetCurrentPosition();
				ActionColl->AP_Points[2] = *((CircularInterPolationAction*)Caction)->GetNormalVector();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((LinearInterPolationAction*)Caction)->GetTargetPosition();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_PointDRO = *((ProfileScanAction*)Caction)->GetCurrentPosition();
				ActionColl->AP_Points[0] = *((ProfileScanAction*)Caction)->GetFirstPt();
				ActionColl->AP_Points[1] = *((ProfileScanAction*)Caction)->GetSecondPt();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)Caction)->getFramegrab();
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_PointDRO.set(fb->PointDRO.getX(), fb->PointDRO.getY(), fb->PointDRO.getZ(), fb->PointDRO.getR(), fb->PointDRO.getP());
				ActionColl->AP_Points[0].set(fb->points[0].getX(), fb->points[0].getY(), fb->points[0].getZ(), fb->points[0].getR(), fb->points[0].getP());
				ActionColl->AP_Points[1].set(fb->points[1].getX(), fb->points[1].getY(), fb->points[1].getZ(), fb->points[1].getR(), fb->points[1].getP());
				ActionColl->AP_Points[2].set(fb->points[2].getX(), fb->points[2].getY(), fb->points[2].getZ(), fb->points[2].getR(), fb->points[2].getP());
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((EditCad1PtAction*)Caction)->getPoint1();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((Cad1PtAlignAction*)Caction)->getPoint1();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((Cad1Pt1LineAction*)Caction)->getPoint1();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((Cad2PtAlignAction*)Caction)->getPoint1();
				ActionColl->AP_Points[1] = *((Cad2PtAlignAction*)Caction)->getPoint2();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((EditCad2PtAction*)Caction)->getPoint1();
				ActionColl->AP_Points[1] = *((EditCad2PtAction*)Caction)->getPoint2();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				ActionsPointCollection* ActionColl = new ActionsPointCollection();
				ActionColl->AP_Points[0] = *((EditCad1Pt1LineAction*)Caction)->getPoint1();
				ActionColl->AP_Id = Caction->getId();
				PPActionPositionColl[Caction->getId()] = ActionColl;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC009ab", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::SetActionIdForNogoMeasurement()
{
	try
	{
		this->ActionIdListForNogoMeasurement.clear();
		std::list<int> *ActionIdList = new std::list<int>();
		//get all shape id which belongs to nogo measurement....
		for(RC_ITER MeasureItr = MAINDllOBJECT->getDimList().getList().begin(); MeasureItr != MAINDllOBJECT->getDimList().getList().end(); MeasureItr++)
		{
			std::list<int> ShapeIdList;
			DimBase* Cdim = (DimBase*)(*MeasureItr).second;
			if(Cdim->MeasurementUsedForPPAbort())
			{
				for(list<BaseItem*>::iterator shapeiterator = Cdim->getParents().begin();shapeiterator != Cdim->getParents().end(); shapeiterator++)
				{
					bool Idexist = false;
					int id = ((Shape*)(*shapeiterator))->getId();
					for each(int existingId in ShapeIdList)
					{
						if(existingId == id)
						{
							Idexist = true;
							break;
						}
					}
					if(!Idexist)
						ShapeIdList.push_back(id);			
				}
				std::list<int> tempActionIdList;
				//get all action Id which belongs to nogo measurement...........
				for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
				{
					if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
					{
						bool ShapeIdPresent = false;
						int actionID = 0;
						int ShapeID = ((AddPointAction*)(*ActionListItr).second)->getShape()->getId();
						for each(int ShpId in ShapeIdList)
						{
							if(ShapeID == ShpId)
							{
								ShapeIdPresent = true;
								break;
							}
						}
						if(ShapeIdPresent)
						{
							actionID = ((RAction*)(*ActionListItr).second)->getId();
							bool ActionIdexist = false;
							for each(int existingId in tempActionIdList)
							{
								if(existingId == actionID)
								{
									ActionIdexist = true;
									break;
								}
							}
							if(!ActionIdexist)
								tempActionIdList.push_back(actionID);
						}
					}
				}
				int LastActionId = 0;
				for each(int ActID in tempActionIdList)
				{
					bool IdPresent = false;
					for each(int Id in *ActionIdList)
					{
						if(ActID == Id)
						{
							IdPresent = true;
							break;
						}
					}
					if(!IdPresent)
						ActionIdList->push_back(ActID);
					if(ActID > LastActionId)
						LastActionId = ActID;
				}
				if(LastActionId > 0)
					ActionIdListForNogoMeasurement[Cdim->getId()] = LastActionId;
			}
		}
	
		getLastActionIdForOffsetCorrection();
		SetActionIdForCriticalShape(ActionIdList);
		ArrangeForShortestPath(ActionIdList);
		ActionIdList->clear();
		delete ActionIdList;

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0010a", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::SetActionIdForCriticalShape(std::list<int> *AbortCheckActionIdList)
{
	try
	{
		this->ActionIdListForCriticalShape.clear();	 
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getShapes().getList().begin(); Item_Shape != Cucs->getShapes().getList().end(); Item_Shape++)
			{
				bool CriticalShape = false;
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(!Cshape->Normalshape()) continue;
				if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape->ShapeType == RapidEnums::SHAPETYPE::XRAY || Cshape->ShapeType == RapidEnums::SHAPETYPE::XLINE)
				{
					if(((Line*)Cshape)->SetAsCriticalShape())
						CriticalShape = true;
				}
				else if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC3D || Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				{
					if(((Circle*)Cshape)->SetAsCriticalShape())
						CriticalShape = true;
				}
				if(CriticalShape)
				{
					std::list<int> tempActionIdList;
					//get all action Id which belongs to nogo measurement...........
					for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
					{
						if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
						{		
							if(Cshape->getId() == ((AddPointAction*)(*ActionListItr).second)->getShape()->getId())
							{
								int actionID = ((RAction*)(*ActionListItr).second)->getId();
								bool ActionIdexist = false;
								for each(int existingId in tempActionIdList)
								{
									if(existingId == actionID)
									{
										ActionIdexist = true;
										break;
									}
								}
								if(!ActionIdexist)
									tempActionIdList.push_back(actionID);
							}
						}
					}
					int LastActionId = 0;
					for each(int ActID in tempActionIdList)
					{
						bool IdPresent = false;
						for each(int Id in *AbortCheckActionIdList)
						{
							if(ActID == Id)
							{
								IdPresent = true;
								break;
							}
						}
						if(!IdPresent)
							AbortCheckActionIdList->push_back(ActID);
						if(ActID > LastActionId)
							LastActionId = ActID;
					}
					if(LastActionId > 0)
						ActionIdListForCriticalShape[Cshape->getId()] = LastActionId;
				}
			}
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0010b", __FILE__, __FUNCSIG__); } 
}


//Partprogram actions arrange for shortest path..
void PartProgramFunctions::ArrangeForShortestPath()
{
	try
	{
		bool DoArrangement = true;
		for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
		{
			if(((RAction*)(*ActionListItr).second)->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				DoArrangement = false;
				break;
			}
			else if(((AddPointAction*)(*ActionListItr).second)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
			{
				DoArrangement = false;
				break;
			}
		}
		if(DoArrangement)
		{
			int i = 0, TotalNoOfPts = getPPActionlist().getList().size(), NoOfPtsToArrange = 0, NoOfPtsWithoutArrange_AtEnd = 0, NoOfPtsWithoutArrange_AtStart = 0;
			double *LocationPtsList = (double*)malloc(sizeof(double) * TotalNoOfPts * 2);
			int *ActionIDs = (int*)malloc(sizeof(double) * TotalNoOfPts);
			int *NonArrangableActionIDs_AtEnd = (int*)malloc(sizeof(double) * TotalNoOfPts);
			int *ArrangedActionIDs = (int*)malloc(sizeof(double) * TotalNoOfPts);
			int *NonArrangableActionIDs_AtStart = (int*)malloc(sizeof(double) * TotalNoOfPts);

			for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
			{
				RapidEnums::RAPIDFGACTIONTYPE TmpFGActionType = ((AddPointAction*)(*ActionListItr).second)->getFramegrab()->FGtype;
				if(TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB ||
					TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::DETECT_ALLEDGES || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE ||
					TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB)
				{
					if(((AddPointAction*)(*ActionListItr).second)->getShape()->ShapeUsedForTwoStepHoming() || ((AddPointAction*)(*ActionListItr).second)->getShape()->ShapeAsReferenceShape())
					{
						NonArrangableActionIDs_AtStart[NoOfPtsWithoutArrange_AtStart] = ((RAction*)(*ActionListItr).second)->getId();
						NoOfPtsWithoutArrange_AtStart++;
					}
					else
					{
						Vector ActionDRO = ((AddPointAction*)(*ActionListItr).second)->getFramegrab()->PointDRO;
						LocationPtsList[2 * NoOfPtsToArrange] = ActionDRO.getX();
						LocationPtsList[2 * NoOfPtsToArrange + 1] = ActionDRO.getY();
						ActionIDs[NoOfPtsToArrange] = ((RAction*)(*ActionListItr).second)->getId();
						NoOfPtsToArrange++;
					}
				}
				else
				{
					NonArrangableActionIDs_AtEnd[NoOfPtsWithoutArrange_AtEnd] = ((RAction*)(*ActionListItr).second)->getId();
					NoOfPtsWithoutArrange_AtEnd++;
				}
			}
			if(NoOfPtsToArrange > 0)
				RMATH3DOBJECT->ShortestPathForPP(LocationPtsList, ActionIDs, NoOfPtsToArrange, ArrangedActionIDs);
			for(i = 0; i < NoOfPtsWithoutArrange_AtStart; i++)
			{
				PPShortestPathActionlist.addItemWithoutId((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtStart[i]]);
				((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtStart[i]])->setId(NonArrangableActionIDs_AtStart[i]);
			}
			for(i = 0; i < NoOfPtsToArrange; i++)
			{
				PPShortestPathActionlist.addItemWithoutId((RAction*)getPPActionlist().getList()[ArrangedActionIDs[i]]);
				((RAction*)getPPActionlist().getList()[ArrangedActionIDs[i]])->setId(ArrangedActionIDs[i]);
			}
			for(i = 0; i < NoOfPtsWithoutArrange_AtEnd; i++)
			{
				PPShortestPathActionlist.addItemWithoutId((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtEnd[i]]);
				((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtEnd[i]])->setId(NonArrangableActionIDs_AtEnd[i]);
			}
			free(LocationPtsList);
			free(ActionIDs);
			free(NonArrangableActionIDs_AtEnd);
			free(ArrangedActionIDs);
			free(NonArrangableActionIDs_AtStart);
		}
		else
		{
			for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
			{
				PPShortestPathActionlist.addItem(((RAction*)(*ActionListItr).second), false);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0011a", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::ArrangeForShortestPath(std::list<int> *AbortCheckActionIdList)
{
	try
	{
		bool DoArrangement = true, DoNotArrangeAction = false;
		for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
		{
			if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				if(((AddPointAction*)(*ActionListItr).second)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
				{
					DoArrangement = false;
					break;
				}
			}
			else if(((RAction*)(*ActionListItr).second)->CurrentActionType != RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION && ((RAction*)(*ActionListItr).second)->CurrentActionType != RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION)
			{
				DoArrangement = false;
				break;
			}
			else if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION)
			{
				DoNotArrangeAction = true;
			}
		}
		if(DoArrangement)
		{
			int i = 0, TotalNoOfPts = getPPActionlist().getList().size(), NoOfPtsToArrange = 0, NoOfPtsWithoutArrange_AtEnd = 0, NoOfPtsWithoutArrange_AtStart = 0;
			double *LocationPtsList = (double*)malloc(sizeof(double) * TotalNoOfPts * 2);
			int *ActionIDs = (int*)malloc(sizeof(double) * TotalNoOfPts);
			int *NonArrangableActionIDs_AtEnd = (int*)malloc(sizeof(double) * TotalNoOfPts);
			int *ArrangedActionIDs = (int*)malloc(sizeof(double) * TotalNoOfPts);
			int *NonArrangableActionIDs_AtStart = (int*)malloc(sizeof(double) * TotalNoOfPts);

			for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
			{
				if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					RapidEnums::RAPIDFGACTIONTYPE TmpFGActionType = ((AddPointAction*)(*ActionListItr).second)->getFramegrab()->FGtype;
					if((((AddPointAction*)(*ActionListItr).second)->getFramegrab()->CurrentWindowNo == 0) && (TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB ||
						TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::DETECT_ALLEDGES || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK || TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE ||
						TmpFGActionType == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB))
					{
						if(((AddPointAction*)(*ActionListItr).second)->getShape()->ShapeUsedForTwoStepHoming() || ((AddPointAction*)(*ActionListItr).second)->getShape()->ShapeAsReferenceShape())
						{
							NonArrangableActionIDs_AtStart[NoOfPtsWithoutArrange_AtStart] = ((RAction*)(*ActionListItr).second)->getId();
							NoOfPtsWithoutArrange_AtStart++;
							for each(int AbrtActionId in *AbortCheckActionIdList)
							{
								if(AbrtActionId == ((RAction*)(*ActionListItr).second)->getId())
								{
									AbortCheckActionIdList->remove(AbrtActionId);
									break;
								}
							}
						}
						else if(DoNotArrangeAction)
						{
							NonArrangableActionIDs_AtStart[NoOfPtsWithoutArrange_AtStart] = ((RAction*)(*ActionListItr).second)->getId();
							NoOfPtsWithoutArrange_AtStart++;
						}
						else
						{
							bool SkipId = false;
							for each(int AbrtActionId in *AbortCheckActionIdList)
							{
								if(AbrtActionId == ((RAction*)(*ActionListItr).second)->getId())
									SkipId = true;
							}
							if(SkipId) continue;
							Vector ActionDRO = ((AddPointAction*)(*ActionListItr).second)->getFramegrab()->PointDRO;
							LocationPtsList[2 * NoOfPtsToArrange] = ActionDRO.getX();
							LocationPtsList[2 * NoOfPtsToArrange + 1] = ActionDRO.getY();
							ActionIDs[NoOfPtsToArrange] = ((RAction*)(*ActionListItr).second)->getId();
							NoOfPtsToArrange++;
						}
					}
					else
					{
						bool SkipId = false;
						for each(int AbrtActionId in *AbortCheckActionIdList)
						{
							if(AbrtActionId == ((RAction*)(*ActionListItr).second)->getId())
								SkipId = true;
						}
						if(SkipId) continue;
						NonArrangableActionIDs_AtEnd[NoOfPtsWithoutArrange_AtEnd] = ((RAction*)(*ActionListItr).second)->getId();
						NoOfPtsWithoutArrange_AtEnd++;
					}
				}
				else if(((RAction*)(*ActionListItr).second)->CurrentActionType == RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION)
				{
					NonArrangableActionIDs_AtEnd[NoOfPtsWithoutArrange_AtEnd] = ((RAction*)(*ActionListItr).second)->getId();
					NoOfPtsWithoutArrange_AtEnd++;
				}
				else
				{
					NonArrangableActionIDs_AtStart[NoOfPtsWithoutArrange_AtStart] = ((RAction*)(*ActionListItr).second)->getId();
					NoOfPtsWithoutArrange_AtStart++;
				}
			}
			if(NoOfPtsToArrange > 0)
				RMATH3DOBJECT->ShortestPathForPP(LocationPtsList, ActionIDs, NoOfPtsToArrange, ArrangedActionIDs);
			for(i = 0; i < NoOfPtsWithoutArrange_AtStart; i++)
			{
				PPShortestPathActionlist.addItemWithoutId((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtStart[i]]);
				((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtStart[i]])->setId(NonArrangableActionIDs_AtStart[i]);
			}
			for each(int AbrtActionId in *AbortCheckActionIdList)
			{
				PPShortestPathActionlist.addItemWithoutId((RAction*)getPPActionlist().getList()[AbrtActionId]);
				((RAction*)getPPActionlist().getList()[AbrtActionId])->setId(AbrtActionId);
			}
			for(i = 0; i < NoOfPtsToArrange; i++)
			{
				PPShortestPathActionlist.addItemWithoutId((RAction*)getPPActionlist().getList()[ArrangedActionIDs[i]]);
				((RAction*)getPPActionlist().getList()[ArrangedActionIDs[i]])->setId(ArrangedActionIDs[i]);
			}
			for(i = 0; i < NoOfPtsWithoutArrange_AtEnd; i++)
			{
				PPShortestPathActionlist.addItemWithoutId((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtEnd[i]]);
				((RAction*)getPPActionlist().getList()[NonArrangableActionIDs_AtEnd[i]])->setId(NonArrangableActionIDs_AtEnd[i]);
			}
			free(LocationPtsList);
			free(ActionIDs);
			free(NonArrangableActionIDs_AtEnd);
			free(ArrangedActionIDs);
			free(NonArrangableActionIDs_AtStart);
		}
		else
		{
			for(RC_ITER ActionListItr = getPPActionlist().getList().begin(); ActionListItr != getPPActionlist().getList().end(); ActionListItr++)
			{
				PPShortestPathActionlist.addItem(((RAction*)(*ActionListItr).second), false);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0011a", __FILE__, __FUNCSIG__); } 
}

//First Fg action..
void PartProgramFunctions::AssignFirstFGAction(bool AddPointActionFlag)
{
	try
	{
		FramegrabBase* Cfgb;
		//MessageBox(NULL, L"Assiging first action", L"Rapid-I", MB_TOPMOST);

		if(AddPointActionFlag)
			Cfgb = ((AddPointAction*)(*ppCurrentaction).second)->getFramegrab();
		else
		{
			if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				Cfgb = ((AddProbePathAction*)(*ppCurrentaction).second)->getFramegrab();
			}
			else if(((RAction*)(*ppCurrentaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				Cfgb = ((PerimeterAction*)(*ppCurrentaction).second)->getFramegrab();
			}
		}
		bool flag = SetFirstFrameGrabHandler(Cfgb->FGtype);
		if(flag)
		{
			CopyFirstActionProperties(Cfgb);
			((FrameGrabHandler*)MAINDllOBJECT->getCurrentHandler())->setDrawFG(ppFirstgrab);
			UpdateFistFramegrab(true);
		}
		else
		{
			CopyFirstActionProperties(Cfgb);
			StartingPointofPartprogram(true);
		}
		//MessageBox(NULL, L"Finished First action assignment", L"Rapid-I", MB_TOPMOST);

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0012", __FILE__, __FUNCSIG__); } 
}

//First Framegrab handler.
bool PartProgramFunctions::SetFirstFrameGrabHandler(RapidEnums::RAPIDFGACTIONTYPE Cation)
{
	try
	{
		bool flag = false;
		switch(Cation)
		{
			case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
				MAINDllOBJECT->setHandler(new FrameGrabArc());
				flag = true;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
				MAINDllOBJECT->setHandler(new FrameGrabAngularRect());
				flag = true;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
				MAINDllOBJECT->setHandler(new FrameGrabCircle());
				flag = true;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
				MAINDllOBJECT->setHandler(new FrameGrabRectangle(false));
				flag = true;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES:
				MAINDllOBJECT->setHandler(new FrameGrabRectangle(true));
				flag = true;
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
				MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle());
				flag = true;
				break;
		}
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0013", __FILE__, __FUNCSIG__); return false; } 
}

//Copy first action properties..
void PartProgramFunctions::CopyFirstActionProperties(FramegrabBase* FirstFg)
{
	try
	{
		ppFirstgrab->PointDRO.set(FirstFg->PointDRO);
		for(int i = 0; i < 3; i++)
			ppFirstgrab->points[i].set(FirstFg->points[i]);
		for(int i = 0; i < 5; i++)
			ppFirstgrab->myPosition[i].set(FirstFg->myPosition[i]);
		ppFirstgrab->FGWidth = FirstFg->FGWidth;
		ppFirstgrab->FGdirection = FirstFg->FGdirection;
		ppFirstgrab->FGtype = FirstFg->FGtype;
		ppFirstgrab->camProperty = FirstFg->camProperty;
		ppFirstgrab->lightProperty = FirstFg->lightProperty;
		ppFirstgrab->magLevel = FirstFg->magLevel;
		InitilizePointPP = ppFirstgrab->points[0];
		if(FirstFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX || FirstFg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
		{
			MAINDllOBJECT->setHandler(new FocusDepthMultiBoxHandler(FirstFg));
			MAINDllOBJECT->getCurrentHandler()->PartProgramData();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0014", __FILE__, __FUNCSIG__); } 
}

//Load first action after program finish..
void PartProgramFunctions::LoadFirstActionAfterProgramFinish()
{
	try
	{
		bool flag = SetFirstFrameGrabHandler(ppFirstgrab->FGtype);
		if(flag)
		{
			((FrameGrabHandler*)MAINDllOBJECT->getCurrentHandler())->setDrawFG(ppFirstgrab);
			UpdateFistFramegrab(true);
		}
		else
		{
			StartingPointofPartprogram(true);
		}
		if(!MAINDllOBJECT->IsCNCMode())
			MAINDllOBJECT->setHandler(new DefaultHandler());
		MAINDllOBJECT->UpdateVideoViewPortSettings();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0015", __FILE__, __FUNCSIG__); } 
}

//Copy all the shapes from PP to current UCS/.
void PartProgramFunctions::CopyAllShapesfromPP()
{
	try
	{
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getShapes().getList().begin(); Item_Shape != Cucs->getShapes().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->Normalshape())
				{
					Cucs->getPPShapes_Original().addItem(Cshape->CreateDummyCopy(), false);
					Cucs->getPPShapes_Path().addItem(Cshape->CreateDummyCopy(), false);
				}
			}
			for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				if(Cdim->ChildMeasurementType()) continue;
				switch(Cdim->MeasurementType)
				{
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS:
					if(!Cdim->ChildMeasurementType())
					{
						Vector *tmpV = MAINDllOBJECT->getVectorPointer_UCS(Cdim->ParentPoint1, Cucs, true);
						if(tmpV == NULL)
							tmpV = MAINDllOBJECT->getVectorPointer_UCS(Cdim->ParentPoint1, Cucs);
						Cdim->ParentPoint1 = tmpV;
						tmpV = MAINDllOBJECT->getVectorPointer_UCS(Cdim->ParentPoint2, Cucs, true);
						if(tmpV == NULL)
							tmpV = MAINDllOBJECT->getVectorPointer_UCS(Cdim->ParentPoint2, Cucs);
						Cdim->ParentPoint2 = tmpV;
						if(Cdim->ParentPoint1 != NULL && Cdim->ParentPoint1->IntersectionPoint)
							((IsectPt*)Cdim->ParentPoint1)->SetRelatedDim(Cdim);
						if(Cdim->ParentPoint2 != NULL && Cdim->ParentPoint2->IntersectionPoint)
							((IsectPt*)Cdim->ParentPoint2)->SetRelatedDim(Cdim);
					}
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONICS3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTX:
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINTY:
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_D:
				case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
					if(!Cdim->ChildMeasurementType())
					{
						Vector *tmpV = MAINDllOBJECT->getVectorPointer_UCS(Cdim->ParentPoint1, Cucs, true);
						if(tmpV == NULL) {tmpV = MAINDllOBJECT->getVectorPointer_UCS(Cdim->ParentPoint1, Cucs);}
						Cdim->ParentPoint1 = tmpV;
						if(Cdim->ParentPoint1 != NULL) 
						{
							if(Cdim->ParentPoint1->IntersectionPoint)
							   ((IsectPt*)Cdim->ParentPoint1)->SetRelatedDim(Cdim);
						}
					}
					//{
					//	Vector Temp(*Cdim->ParentPoint1);
					//	delete Cdim->ParentPoint1;
					//	Vector* Vpt = MAINDllOBJECT->getVectorPointer_UCS(&Temp, Cucs);
					//	IsectPt* IsPt = (IsectPt*)Vpt;
					//	Cdim->ParentPoint1 = IsPt; 
					////Cdim->ParentPoint1 = MAINDllOBJECT->getVectorPointer(Cdim->ParentPoint1);
					//if(Cdim->ParentPoint1->IntersectionPoint)
					//	((IsectPt*)Cdim->ParentPoint1)->SetRelatedDim(Cdim);
					//}
					break;
				}
				Cdim->IsValid(true);
				DimBase* Cdim1 = Cdim->CreateDummyCopy();
				DimBase* Cdim2 = Cdim->CreateDummyCopy();
				Cucs->getPPMeasures_Original().addItem(Cdim1, false);
				Cucs->getPPMeasures_Path().addItem(Cdim2, false);
				if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
				{
					((DimPointCoordinate*)Cdim1)->AddChildeMeasureToCollection(Cucs->getPPMeasures_Original());
					((DimPointCoordinate*)Cdim2)->AddChildeMeasureToCollection(Cucs->getPPMeasures_Path());
				}
				else if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
				{
					((DimThread*)Cdim1)->AddChildeMeasureToCollection(Cucs->getPPMeasures_Original());
					((DimThread*)Cdim2)->AddChildeMeasureToCollection(Cucs->getPPMeasures_Path());
				}
			}
		}

		RCollectionBase *currentPPAction;
		if(PartprogramisLoadingEditMode())
			currentPPAction = &MAINDllOBJECT->getActionsHistoryList();
		else
			currentPPAction = &getPPActionlist();
		StoreOriginalActionForNextRun(currentPPAction);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0016", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::WriteDebugInfo()
{
	try
	{
		std::string filePath;
		std::wofstream PointCollectionWriter;
		filePath = MAINDllOBJECT->currentPath + "\\DebugInfo.txt";
		PointCollectionWriter.open(filePath.c_str(), std::ios_base::app);
		PointCollectionWriter << "HomePosition : " << HomePosition.getX() << ", "  << HomePosition.getY() << ", "  << HomePosition.getZ() << endl;
		PointCollectionWriter << "PPHomePosition : " << PPHomePosition.getX() << ", "  << PPHomePosition.getY() << ", "  << PPHomePosition.getZ() << endl;

		for(RC_ITER Item_PPAction = getPPActionlist().getList().begin(); Item_PPAction != getPPActionlist().getList().end(); Item_PPAction++)
		{
			RAction* Caction = (RAction*)(*Item_PPAction).second;
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)Caction)->getFramegrab();
				PointCollectionWriter << "CorrectedProbePoint : " << Caction->getId()  << endl;
				PointCollectionWriter << fb->PointDRO.getX() << ", " << fb->PointDRO.getY() << ", " << fb->PointDRO.getZ() << ", " << fb->PointDRO.getR() << endl;
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];	
					PointCollectionWriter << "SavedProbePoint : " << Caction->getId() << endl;
					PointCollectionWriter << ActionColl->AP_PointDRO.getX() << ", " << ActionColl->AP_PointDRO.getY() << ", " << ActionColl->AP_PointDRO.getZ() << ", " << ActionColl->AP_PointDRO.getR() << endl;
				}	
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)Caction)->getFramegrab();
				PointCollectionWriter << "CorrectedProbePath : " << Caction->getId() << endl;
				PointCollectionWriter << fb->PointDRO.getX() << ", " << fb->PointDRO.getY() << ", " << fb->PointDRO.getZ() << ", " << fb->PointDRO.getR() << endl;
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];	
					PointCollectionWriter << "SavedProbePath : " << Caction->getId() << endl;
					PointCollectionWriter << ActionColl->AP_PointDRO.getX() << ", " << ActionColl->AP_PointDRO.getY() << ", " << ActionColl->AP_PointDRO.getZ() << ", " << ActionColl->AP_PointDRO.getR() << endl;
				}	
			}
		}
		PointCollectionWriter.close();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0015", __FILE__, __FUNCSIG__); } 
}

//transform the shapes...//
void PartProgramFunctions::ChangePosition_FrameGrab()
{
	try
	{
		if(MAINDllOBJECT->getCurrentHandler() == NULL) return;
		if(dynamic_cast<FrameGrabHandler*>(MAINDllOBJECT->getCurrentHandler()))
		{
			double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
			for(int i = 0; i < MAINDllOBJECT->getCurrentHandler()->getMaxClicks(); i++)
			{
				double mx = cx + ppFirstgrab->myPosition[i].getX() * upp, my = cy - ppFirstgrab->myPosition[i].getY() * upp;
				MAINDllOBJECT->getCurrentHandler()->baseaction->points[i].set(mx, my);
			}
			((FrameGrabHandler*)MAINDllOBJECT->getCurrentHandler())->updateGraphicsProgram();
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0017", __FILE__, __FUNCSIG__);
	}
}

//Redraw the partpogram shapes list.
void PartProgramFunctions::buildPPlist()
{
	try
	{
		GRAFIX->DeleteGlList(7);
		GRAFIX->CreateNewGlList(7);
		double wupp = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(1, 0, 0);
		for(RC_ITER Shitem = MAINDllOBJECT->getPPShapesList_Path().getList().begin(); Shitem != MAINDllOBJECT->getPPShapesList_Path().getList().end(); Shitem++)
		{
			Shape* Cshape = (Shape*)(*Shitem).second;
			if(!Cshape->Normalshape()) continue;
			Cshape->drawShape(2, wupp);
		}
		GRAFIX->SetColor_Double(0,1, 0);
		for(RC_ITER Dimitem = MAINDllOBJECT->getPPMeasuresList_Path().getList().begin(); Dimitem != MAINDllOBJECT->getPPMeasuresList_Path().getList().end(); Dimitem++)
		{
			DimBase* Cdim = (DimBase*)(*Dimitem).second;
			Cdim->drawMeasurement(2, wupp);
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0018", __FILE__, __FUNCSIG__); }
}

//draw program path
void PartProgramFunctions::drawPartprogramArrow(double wupp)
{
	try
	{
		//Draw the partprogram arrow...//
		if(DrawPartprogramArrow())
		{
			if(!(startppPoint.operator==(endppPoint)))
				GRAFIX->drawPartprogramPathArrow(startppPoint.getX(), startppPoint.getY(), endppPoint.getX(), endppPoint.getY(), wupp);
		}
		//Draw the starting point of the partprogram...//
		if(StartingPointofPartprogram())
			GRAFIX->drawPartProgramStartPoint(InitilizePointPP.getX(), InitilizePointPP.getY(), wupp);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0019", __FILE__, __FUNCSIG__); }
}

//Check whether the frame is in view or not
bool PartProgramFunctions::CheckWithinCurrentView()
{
	try
	{
		double extnt[4];
		MAINDllOBJECT->getWindow(0).getExtents(&extnt[0]);
		FramegrabBase* fg;
		if(getCActionType() == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			fg = getThreadFrameGrab();
		else
			fg = getFrameGrab();
		bool withinview = false;
		switch(fg->FGtype)
		{
		case RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE:
		case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
			withinview = RMATH2DOBJECT->Ptisinwindow(fg->points[0].getX(), fg->points[0].getY(), extnt[0], extnt[2], extnt[1], extnt[3]); 
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
			break;
		//case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
		//	//commenting this out because it is causing few problems otherwise during part-program run by skipping few framegrabs.  Need to debug this.
		//	//-Chandar Apr 25 2012

		//	if(RMATH2DOBJECT->Ptisinwindow(fg->points[0].getX(), fg->points[0].getY(), extnt[0], extnt[2], extnt[1], extnt[3]) &&
		//		RMATH2DOBJECT->Ptisinwindow(fg->points[1].getX(), fg->points[1].getY(), extnt[0], extnt[2], extnt[1], extnt[3]))
		//		withinview = true;
		//	break;
		//case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
		//	//commenting this out because it is causing few problems otherwise during part-program run by skipping few framegrabs.  Need to debug this.
		//	//-Chandar Apr 25 2012

		//	if(RMATH2DOBJECT->Ptisinwindow(fg->points[0].getX(), fg->points[0].getY(), extnt[0], extnt[2], extnt[1], extnt[3]) &&
		//		RMATH2DOBJECT->Ptisinwindow(fg->points[1].getX(), fg->points[1].getY(), extnt[0], extnt[2], extnt[1], extnt[3]))
		//		withinview = true;
		//	break;
		}
		return withinview;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0020", __FILE__, __FUNCSIG__); return false; }
}

//Check whether current action type is profile light framegrab
bool PartProgramFunctions::CheckCurrentActionType_ProfileFG()
{
	try
	{
		FramegrabBase* fg;
		if(getCActionType() == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			fg = getThreadFrameGrab();
		else
			fg = getFrameGrab();
		RapidEnums::RAPIDFGACTIONTYPE CFgType = fg->FGtype;
		bool DoneFrameGrab = false;
		if(CFgType == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || CFgType == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB ||
			CFgType == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB || CFgType == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES || CFgType == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE ||
			CFgType == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
			DoneFrameGrab = true;
		if(DoneFrameGrab)
		{
			if(fg->ProfileON && !fg->SurfaceON)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0020", __FILE__, __FUNCSIG__); return false; }
}

//Check whether we can be get points with the current edge.. during program run
bool PartProgramFunctions::CheckCurrentFrameGrab_Fail()
{
	try
	{
		//return true;
		FramegrabBase* fg;
		if(getCActionType() == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			fg = getThreadFrameGrab();
		else
			fg = getFrameGrab();
		bool FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(fg, false);
		fg->ActionFinished(false);
		fg->AllPointsList.deleteAll();
		if(!FrameGrabDone || (fg->noofpts < fg->noofptstaken_build/10))
			return true;
		else
			return false;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0020", __FILE__, __FUNCSIG__); return false; }
}

//Calculate enclosed rectangle for current FG.
void PartProgramFunctions::CalculateFocusRectangeBox_FG()
{
	try
	{
	/*	double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double wupp = MAINDllOBJECT->getWindow(0).getUppX();*/
		int RectFGArray[4];
	/*	double point1[6], LeftTop[2], Rightbottom[2];*/
		
		FramegrabBase* fg;
		if(getCActionType() == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			fg = getThreadFrameGrab();
		else
			fg = getFrameGrab();
		RapidEnums::RAPIDFGACTIONTYPE CFGtype = fg->FGtype;
		//double width = fg->FGWidth * wupp;
		//fg->myPosition[0].FillDoublePointer(&point1[0]);
		//fg->myPosition[1].FillDoublePointer(&point1[2]);
		//fg->myPosition[2].FillDoublePointer(&point1[4]);

		int CFGdirection = fg->FGdirection;
		double CFGWidth = fg->FGWidth;

		double p1[2] = {fg->myPosition[0].getX(), fg->myPosition[0].getY()};
		double p2[2] = {fg->myPosition[1].getX(), fg->myPosition[1].getY()};
		double p3[2] = {fg->myPosition[2].getX(), fg->myPosition[2].getY()};
		int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y;

		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT) && MAINDllOBJECT->DigitalZoomModeForOneShot())
		{
			for(int i = 0; i < 2; i++)
			{
				p1[i] = p1[i] * 2;
				p2[i] = p2[i] * 2;
				p3[i] = p3[i] * 2;
			}
			wwidth = wwidth * 2;
			wheight = wheight * 2;
		}
		double CircularArray[6];
		//According to the type calculate the points(grab the points: edge)
		if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB)
		{
			RMATH2DOBJECT->CircleParameters_FG(&p1[0], &p2[0], CFGWidth, &CircularArray[0]);
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB)
		{
			p1[1] = wheight - p1[1]; p2[1] = wheight - p2[1]; p3[1] = wheight - p3[1];
			RMATH2DOBJECT->ArcParameters_FG(&p1[0], &p2[0], &p3[0], CFGWidth, &CircularArray[0]);
			CircularArray[1] = wheight - CircularArray[1];
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES || CFGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE)
		{
			RectFGArray[0] = min(p1[0], p2[0]); RectFGArray[1] = min(p1[1], p2[1]);
			RectFGArray[2] = max(p1[0], p2[0]) - RectFGArray[0];
			RectFGArray[3] = max(p1[1], p2[1]) - RectFGArray[1];
		}
		else if(CFGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB)
		{
			RectFGArray[0] = p1[0]; RectFGArray[1] = p1[1];
			RectFGArray[2] = p2[0] -  p1[0]; RectFGArray[3] = p2[1] -  p1[0];
		/*	RectFGArray[4] = CFGWidth * 2;*/
		}
		FOCUSCALCOBJECT->SetAutoFocusRectangle(&RectFGArray[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0020", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::DeleteUnwantedShapes()
{
	try
	{
		std::list<int> IDLIST;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->DoNotSaveInPP)
					IDLIST.push_back(Cshape->getId());
			}
		}
		MAINDllOBJECT->ClearShapeSelections();
		MAINDllOBJECT->selectShape(&IDLIST);
		MAINDllOBJECT->deleteShape();

		//std::list<int> ActionIdList;
		//std::list<int> MeasureIdlist;
		//std::list<int> ShapeIdlist;
		////get action id and remove parent child relationship for measurement........
		//for(RC_ITER item3 = MAINDllOBJECT->getActionsHistoryList().getList().begin(); item3 != MAINDllOBJECT->getActionsHistoryList().getList().end(); item3++)
		//{
		//	RAction* MyAction = (RAction*)((*item3).second);
		//	if(MyAction->ActionStatus()) continue;
		//	if(MyAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDDIMACTION)
		//	{
		//		ActionIdList.push_back(MyAction->getId());
		//		DimBase *Cdim = ((AddDimAction*)MyAction)->getDim();
		//		MeasureIdlist.push_back(Cdim->getId());
		//		for(list<BaseItem*>::iterator shapeiterator = Cdim->getParents().begin(); shapeiterator != Cdim->getParents().end(); shapeiterator++)
		//		{
		//			((Shape*)(*shapeiterator))->RemoveMChild(Cdim);
		//		}
		//	}
		//}

		////get action id and remove parent child relationship for Shapes........
		//for(RC_ITER item3 = MAINDllOBJECT->getActionsHistoryList().getList().begin(); item3 != MAINDllOBJECT->getActionsHistoryList().getList().end(); item3++)
		//{
		//	RAction* MyAction = (RAction*)((*item3).second);
		//	if(MyAction->ActionStatus()) continue;
		//	if(MyAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDSHAPEACTION)
		//	{
		//		ActionIdList.push_back(MyAction->getId());
		//		Shape* shape = ((AddShapeAction*)MyAction)->getShape();
		//		ShapeIdlist.push_back(shape->getId());
		//		for(list<BaseItem*>::iterator shapeiterator = shape->getParents().begin();shapeiterator != shape->getParents().end(); shapeiterator++)
		//		{
		//			((Shape*)(*shapeiterator))->RemoveChild(shape);
		//		}
		//	}
		//}

		////delete shapes from all ucs........
		//for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		//{
		//	UCS* Cucs = (UCS*)(*Item_ucs).second;
		//	RCollectionBase& rshapes = Cucs->getShapes();	
		//	std::list<int> currentUcsShapeIdlist;
		//	for(RC_ITER Item_shape = rshapes.getList().begin(); Item_shape != rshapes.getList().end(); Item_shape++)
		//	{
		//		BaseItem* Cshape = (BaseItem*)(*Item_shape).second;	
		//		for each(int id in ShapeIdlist)
		//		{
		//			if(Cshape->getId() == id)
		//			{
		//				currentUcsShapeIdlist.push_back(id);
		//				break;
		//			}
		//		}
		//	}
		//	for each(int shpId in currentUcsShapeIdlist)
		//		rshapes.removeItem((BaseItem*)(rshapes.getList()[shpId]), true, false);
		//}

		////delete measurement from all ucs........
		//for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		//{
		//	UCS* Cucs = (UCS*)(*Item_ucs).second;
		//	RCollectionBase& rmeasure = Cucs->getDimensions();	
		//	std::list<int> currentUcsMeasureIdlist;
		//	for(RC_ITER Item_shape = rmeasure.getList().begin(); Item_shape != rmeasure.getList().end(); Item_shape++)
		//	{
		//		BaseItem* Cdim = (BaseItem*)(*Item_shape).second;
		//		for each(int id in MeasureIdlist)
		//		{
		//			if(Cdim->getId() == id)
		//			{
		//				currentUcsMeasureIdlist.push_back(id);
		//				break;
		//			}
		//		}
		//	}
		//	for each(int shpId in currentUcsMeasureIdlist)
		//		rmeasure.removeItem((BaseItem*)(rmeasure.getList()[shpId]), true, false);
		//}

		//delete action.........
		if(MAINDllOBJECT->getActionsHistoryList().getList().size() > 0)
			MAINDllOBJECT->getActionsHistoryList().clear();
		/*std::list<int> ActionIdList;
		for(RC_ITER item = MAINDllOBJECT->getActionsHistoryList().getList().begin(); item != MAINDllOBJECT->getActionsHistoryList().getList().end(); item++)
		{
			BaseItem* _Bitem = (BaseItem*)(*item).second;
			ActionIdList.push_back(_Bitem->getId());
		}
		RCollectionBase& actionColl = MAINDllOBJECT->getActionsHistoryList();
		for each(int id in ActionIdList)
			actionColl.removeItem((BaseItem*)(actionColl.getList()[id]), true, true);*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0020a", __FILE__, __FUNCSIG__); }
}

//Begin aprtprogram... First mouse down on video...//
bool PartProgramFunctions::BeginPartProgram()
{
	try
	{
		if(getPPArrangedActionlist().getList().size() < 1) return false;
		FirstProbePointActionFlag = false; ActionTobeTransformed = false;
		DXFEXPOSEOBJECT->SetAlignedProperty(false);
		this->IsPartProgramRunning(true);
		//DeleteUnwantedShapes();
		//Copy all the shape Parameters to the Main List.. used for the 2nd run...//
		CopyShapesMeasurementFromPPList();
		SaveCurrentAction(&getPPArrangedActionlist());

		MAINDllOBJECT->changeUCS(0);
		MAINDllOBJECT->dontUpdateGraphcis = true;
		FirstMousedown_Partprogram_Manual(!FirstMousedown_Partprogram_Manual());
		FirstMousedown_Partprogram(!FirstMousedown_Partprogram());
		this->FinishedManualTwoStepAlignment(false);
		if(FirstMousedown_Partprogram())
		{
			this->ProgramAutoIncrementMode(false);
			if(this->ProgramMadeUsingTwoStepHoming())
			{
				for(RC_ITER pCaction = getPPArrangedActionlist().getList().begin(); pCaction != getPPArrangedActionlist().getList().end(); pCaction++)
				{
					if(((RAction*)(*pCaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
					{
						FramegrabBase* Cfgb = ((AddPointAction*)(*pCaction).second)->getFramegrab();
						if(Cfgb->ActionForTwoStepHoming())
						{
							ppCurrentaction = pCaction;
							break;
						}
					}
				}
			}
			else
				ppCurrentaction = getPPArrangedActionlist().getList().begin();
			Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
			startppPoint.set(CurrDRO.getX(), CurrDRO.getY(), CurrDRO.getZ(), CurrDRO.getR());
			RAction* Caction = (RAction*)(*ppCurrentaction).second;
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* Cfg = ((AddPointAction*)Caction)->getFramegrab();
				if(Cfg->CurrentWindowNo == 0)
				{
					if(Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
						endppPoint = Cfg->points[0];
					else
						endppPoint = Cfg->PointDRO;
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* Cfg = ((PerimeterAction*)Caction)->getFramegrab();
				if(Cfg->CurrentWindowNo == 0)
				{
					if(Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
						endppPoint = Cfg->points[0];
					else
						endppPoint = Cfg->PointDRO;
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* Cfg = ((AddProbePathAction*)Caction)->getFramegrab();
				//endppPoint = Cfg->points[0];
				endppPoint = Cfg->PointDRO;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* Cfg = ((AddImageAction*)Caction)->getFramegrab();
				endppPoint = Cfg->PointDRO;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* Cfg = ((AreaShapeAction*)Caction)->getFramegrab();
				endppPoint = Cfg->PointDRO;
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				AddThreadPointAction* CthAction = (AddThreadPointAction*)Caction;
				//endppPoint = ((AddThreadPointAction*)Caction)->getFramegrab()->points[0];
				endppPoint = ((AddThreadPointAction*)Caction)->getFramegrab()->PointDRO;
			}
			this->DrawPartprogramArrow(true);
			this->StartingPointofPartprogram(false); 
			this->IsPartProgramRunning(true);
			Sleep(50);
		}
		if(PartProgramAlignmentIsDone())
		{
			FirstMousedown_Partprogram_Manual(false);
			FirstMousedown_Partprogram(false);
		}
		//check if first action is probe action then take first point as manual and then run machine in cnc mode..
		if(MAINDllOBJECT->OneTouchProbeAlignment)
			CheckProbePointAction();
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->updatePPGraphics(true);
		return true;
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0021", __FILE__, __FUNCSIG__); return false; } 
}

void PartProgramFunctions::CheckProbePointAction()
{
	try
	{
		for(RC_ITER pCaction = getPPArrangedActionlist().getList().begin(); pCaction != getPPArrangedActionlist().getList().end(); pCaction++)
		{
			if(((RAction*)(*pCaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* Cfgb = ((AddPointAction*)(*pCaction).second)->getFramegrab();
				if(Cfgb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
				{
					FirstProbePointActionFlag = true;
					break;
				}
			}
			else if(((RAction*)(*pCaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FirstProbePointActionFlag = true;
				break;
			}
			else
			{
				break;
			}
		}
		if(FirstProbePointActionFlag)
		{
			ActionTobeTransformed = true;
			while(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				IncrementAction();
			}
			ActionTobeTransformed = false;
			//MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctions01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0021a", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::ShiftShape_ForFirstProbePoint(Vector difference, bool ActionOnly)
{
	try
	{
		if(!ActionOnly)
		{
			for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
			{
				UCS* Cucs = (UCS*)(*Item_ucs).second;
				if (Cucs->getId() != 0)
				{
					if (Cucs->UCSPoint == Cucs->OriginalUCSPoint)
					{
						Cucs->UCSPoint += difference;
						Cucs->UCSPt_B2S += difference;
					}continue;
				}
				for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
				{
					Shape* Cshape = (Shape*)(*Item_shape).second;
					if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
					if(Cshape->Normalshape())
					{
						if(!Cshape->ShapeAsReferenceShape())
							Cshape->Translate(difference);
						else
						{
							if(ProgramRunningForFirstTime())
								Cshape->Translate(difference);
						}
					}
				}
				for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
				{
					Shape* Cshape = (Shape*)(*Item_shape).second;
					if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
					if(Cshape->Normalshape())
						Cshape->Translate(difference);
				}
				if(Cucs->UCSMode() == 1)
				{
					Cucs->UCSPoint.set(Cucs->CenterPt->getX(), Cucs->CenterPt->getY(), Cucs->CenterPt->getZ());
					Vector ucsdiff(Cucs->OriginalUCSPoint.getX() - Cucs->UCSPoint.getX(), Cucs->OriginalUCSPoint.getY() - Cucs->UCSPoint.getY(), Cucs->OriginalUCSPoint.getZ() - Cucs->UCSPoint.getZ(), Cucs->OriginalUCSPoint.getR() - Cucs->UCSPoint.getR());
					TranslateUCSParameters(ucsdiff, *Cucs);
				}
			}
			for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
			{
				UCS* Cucs = (UCS*)(*Item_ucs).second;
				if (Cucs->getId() != 0) continue;
				for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
				{
					bool Tempflag = false;
					DimBase* Cdim = (DimBase*)(*Item_dim).second;
					for each(BaseItem* Bitem in Cdim->getParents())
					{
						Shape* Csh = (Shape*)Bitem;
						if(Csh->ShapeAsReferenceShape())
						{
							Tempflag = true;
							break;
						}
					}
					if(!Tempflag)
						Cdim->TranslatePosition(difference);
					else
					{
						if(ProgramRunningForFirstTime())
							Cdim->TranslatePosition(difference);
					}
					if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
						Cdim->UpdateMeasurement();
				}
				for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
				{
					DimBase* Cdim = (DimBase*)(*Item_dim).second;
					Cdim->TranslatePosition(difference);
					if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
						Cdim->UpdateMeasurement();
				}
			}
		}

		for(RC_ITER pp = getPPArrangedActionlist().getList().begin(); pp != getPPArrangedActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				Vector temp;
				temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				if (((AddPointAction*)action)->getShape()->UcsId() == 0)
				{
					fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ()); // , difference.getR() + fb->points[0].getR());
					fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ()); // , difference.getR() + fb->points[0].getR());
					fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ()); // , difference.getR() + fb->points[0].getR());
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				Vector temp;
				temp = fb->PointDRO;				
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ()); // , difference.getR() + fb->points[0].getR());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ()); // , difference.getR() + fb->points[0].getR());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ()); // , difference.getR() + fb->points[0].getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)action)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)action)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					Vector temp = fb->PointDRO;
					temp += difference;
					fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
					fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ()); // , difference.getR() + fb->points[0].getR());
					fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ()); // , difference.getR() + fb->points[0].getR());
					fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ()); // , difference.getR() + fb->points[0].getR());
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				((CircularInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				((LinearInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				((ProfileScanAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				Vector temp;
				temp = fb->PointDRO;				
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ()); // , difference.getR() + fb->points[0].getR());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ()); // , difference.getR() + fb->points[0].getR());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ()); // , difference.getR() + fb->points[0].getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1PtAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1PtAction*)action)->SetPoint1(AlignPos);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				if(((Cad1PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1PtAlignAction*)action)->SetPoint1(AlignPos);
				}  
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				if(((Cad2PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint1(AlignPos);
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint2();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint2(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				if(((Cad1Pt1LineAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1Pt1LineAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1Pt1LineAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				Vector AlignPos;
				if(((EditCad2PtAction*)action)->getActionCount() == 1)
				{					
					AlignPos = *((EditCad2PtAction*)action)->getPoint1();
					AlignPos += difference;
					((EditCad2PtAction*)action)->SetPoint1(AlignPos);
				}
				else if(((EditCad2PtAction*)action)->getActionCount() == 2)
				{
					AlignPos = *((EditCad2PtAction*)action)->getPoint2();
					AlignPos += difference;				
					((EditCad2PtAction*)action)->SetPoint2(AlignPos);
				}	
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1Pt1LineAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1Pt1LineAction*)action)->SetPoint1(AlignPos);
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0021b", __FILE__, __FUNCSIG__); } 
}

//Shift the shapes for first mouse down..
void PartProgramFunctions::ShiftShape_ForFirstMouseDown(Vector MousePos, bool Referenceflag)
{
	try
	{
		this->FirstMousedown_Partprogram(false);
		this->FirstMousedown_Partprogram_Manual(false);
		Vector difference;
		if(Referenceflag)
			difference.set(HomePosition.getX() - PPHomePosition.getX(), HomePosition.getY() - PPHomePosition.getY(), HomePosition.getZ() - PPHomePosition.getZ(), HomePosition.getR() - PPHomePosition.getR());
		else
			difference.set(MousePos.getX() - ppFirstgrab->PointDRO.getX(), MousePos.getY() - ppFirstgrab->PointDRO.getY(), MousePos.getZ() - ppFirstgrab->PointDRO.getZ(), MousePos.getR() - ppFirstgrab->PointDRO.getR());
		ShiftShape_ForFirstProbePoint(difference, false);
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0022", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::ShiftShape_ForProbeHoming(Vector MousePos)
{
	try
	{
		this->FirstMousedown_Partprogram(false);
		this->FirstMousedown_Partprogram_Manual(false);
		Vector difference;
		difference.set(MousePos.getX(), MousePos.getY(), MousePos.getZ(), 0);
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
				{
					if(Cshape->ShapeUsedForTwoStepHoming())continue;
					if(!Cshape->ShapeAsReferenceShape())
						Cshape->Translate(difference);
					else
					{
						if(ProgramRunningForFirstTime())
							Cshape->Translate(difference);
					}
				}
			}
			for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
					Cshape->Translate(difference);
			}
			if(Cucs->UCSMode() == 1)
			{
				Cucs->UCSPoint.set(Cucs->CenterPt->getX(), Cucs->CenterPt->getY(), Cucs->CenterPt->getZ());
				Vector ucsdiff(Cucs->OriginalUCSPoint.getX() - Cucs->UCSPoint.getX(), Cucs->OriginalUCSPoint.getY() - Cucs->UCSPoint.getY(), Cucs->OriginalUCSPoint.getZ() - Cucs->UCSPoint.getZ());
				TranslateUCSParameters(ucsdiff, *Cucs);
			}
		}
		
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
			{
				bool Tempflag = false;
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				for each(BaseItem* Bitem in Cdim->getParents())
				{
					Shape* Csh = (Shape*)Bitem;
					if(Csh->ShapeAsReferenceShape())
					{
						Tempflag = true;
						break;
					}
				}
				if(!Tempflag)
					Cdim->TranslatePosition(difference);
				else
				{
					if(ProgramRunningForFirstTime())
						Cdim->TranslatePosition(difference);
				}
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->TranslatePosition(difference);
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
		}

		for(RC_ITER pp = getPPArrangedActionlist().getList().begin(); pp != getPPArrangedActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
				temp = fb->points[0]; temp += difference;
				fb->points[0].set(temp.getX(), temp.getY(), temp.getZ());
				temp = fb->points[1]; temp += difference;
				fb->points[1].set(temp.getX(), temp.getY(), temp.getZ());
				temp = fb->points[2]; temp += difference;
				fb->points[2].set(temp.getX(), temp.getY(), temp.getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
				temp = fb->points[0]; temp += difference;
				fb->points[0].set(temp.getX(), temp.getY(), temp.getZ());
				temp = fb->points[1]; temp += difference;
				fb->points[1].set(temp.getX(), temp.getY(), temp.getZ());
				temp = fb->points[2]; temp += difference;
				fb->points[2].set(temp.getX(), temp.getY(), temp.getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)action)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)action)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					Vector temp = fb->PointDRO;
					temp += difference;
					fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
					fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
					fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
					fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());

				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				((CircularInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				((LinearInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				((ProfileScanAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				Vector temp;
				temp = fb->PointDRO;				
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1PtAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1PtAction*)action)->SetPoint1(AlignPos);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				if(((Cad1PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1PtAlignAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				if(((Cad2PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint1(AlignPos);
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint2();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint2(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				if(((Cad1Pt1LineAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1Pt1LineAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1Pt1LineAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				Vector AlignPos;
				if(((EditCad2PtAction*)action)->getActionCount() == 1)
				{					
					AlignPos = *((EditCad2PtAction*)action)->getPoint1();
					AlignPos += difference;
					((EditCad2PtAction*)action)->SetPoint1(AlignPos);
				}
				else if(((EditCad2PtAction*)action)->getActionCount() == 2)
				{
					AlignPos = *((EditCad2PtAction*)action)->getPoint2();
					AlignPos += difference;				
					((EditCad2PtAction*)action)->SetPoint2(AlignPos);
				}	
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1Pt1LineAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1Pt1LineAction*)action)->SetPoint1(AlignPos);
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0022", __FILE__, __FUNCSIG__); }
}

//Shift the shapes for first mouse down..
void PartProgramFunctions::ShiftShape_ForFirstMouseDown_Manual(Vector MousePos)
{
	try
	{
		this->FirstMousedown_Partprogram_Manual(false);
		this->FirstMousedown_Partprogram(false);
		
		Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
		Vector difference(MousePos.getX() - ppFirstgrab->points[0].getX(), MousePos.getY() - ppFirstgrab->points[0].getY(), MousePos.getZ() - ppFirstgrab->points[0].getZ());
		Vector difference1(CurrDRO.getX() - ppFirstgrab->PointDRO.getX(), CurrDRO.getY() - ppFirstgrab->PointDRO.getY(), CurrDRO.getZ() - ppFirstgrab->PointDRO.getZ(), CurrDRO.getR() - ppFirstgrab->PointDRO.getR());
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
				{
					if(!Cshape->ShapeAsReferenceShape())
						Cshape->Translate(difference);
					else
					{
						if(ProgramRunningForFirstTime())
							Cshape->Translate(difference);
					}
				}
			}
			for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
					Cshape->Translate(difference);
			}
			if(Cucs->UCSMode() == 1)
			{
				Cucs->UCSPoint.set(Cucs->CenterPt->getX(), Cucs->CenterPt->getY(), Cucs->CenterPt->getZ());
				Vector ucsdiff(Cucs->OriginalUCSPoint.getX() - Cucs->UCSPoint.getX(), Cucs->OriginalUCSPoint.getY() - Cucs->UCSPoint.getY(), Cucs->OriginalUCSPoint.getZ() - Cucs->UCSPoint.getZ());
				TranslateUCSParameters(ucsdiff, *Cucs);
			}
		}
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
			{
				bool Tempflag = false;
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				for each(BaseItem* Bitem in Cdim->getParents())
				{
					Shape* Csh = (Shape*)Bitem;
					if(Csh->ShapeAsReferenceShape())
					{
						Tempflag = true;
						break;
					}
				}
				if(!Tempflag)
					Cdim->TranslatePosition(difference);
				else
				{
					if(ProgramRunningForFirstTime())
						Cdim->TranslatePosition(difference);
				}
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->TranslatePosition(difference);
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
		}
		for(RC_ITER pp = getPPArrangedActionlist().getList().begin(); pp != getPPArrangedActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference1;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference1;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference1;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				((ProfileScanAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				Vector temp;
				temp = fb->PointDRO;				
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				RCollectionBase* THAction = &((AddThreadMeasureAction*)action)->ActionCollection;
				for(RC_ITER Item_PPAction1 = THAction->getList().begin(); Item_PPAction1 != THAction->getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					Vector temp = fb->PointDRO;
					temp += difference1;
					fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR(), temp.getP());
					fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
					fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
					fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());

				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				((CircularInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				((LinearInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1PtAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1PtAction*)action)->SetPoint1(AlignPos);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				if(((Cad1PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1PtAlignAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				if(((Cad2PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint1(AlignPos);
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint2();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint2(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				if(((Cad1Pt1LineAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1Pt1LineAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1Pt1LineAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				Vector AlignPos;
				if(((EditCad2PtAction*)action)->getActionCount() == 1)
				{					
					AlignPos = *((EditCad2PtAction*)action)->getPoint1();
					AlignPos += difference;
					((EditCad2PtAction*)action)->SetPoint1(AlignPos);
				}
			   if(((EditCad2PtAction*)action)->getActionCount() == 2)
				{
					AlignPos = *((EditCad2PtAction*)action)->getPoint2();
					AlignPos += difference;				
					((EditCad2PtAction*)action)->SetPoint2(AlignPos);
				}			
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1Pt1LineAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1Pt1LineAction*)action)->SetPoint1(AlignPos);
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0023", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::RotateActionForGridRun(double* RotationPt, double Theta)
{
	try
	{
		double Transform_Matrix[9] = {0};
		Transform_Matrix[0] = cos(Theta); Transform_Matrix[1] = -sin(Theta); Transform_Matrix[2] = RotationPt[0];
		Transform_Matrix[3] = sin(Theta); Transform_Matrix[4] = cos(Theta); Transform_Matrix[5] = RotationPt[1];
		Transform_Matrix[8] = 1;
		TrnasformProgram_TwoStepHoming(Transform_Matrix, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0022a", __FILE__, __FUNCSIG__); }
}

//Two Step alignment shape comparision..
void PartProgramFunctions::TwoStepHoming_Alignment()
{
	try
	{
		int ShapeCnt = 0;
		Shape *CSh_Original[3] = {NULL}, *CSh_Modified[3] = {NULL};
		for(RC_ITER Sitem = MAINDllOBJECT->getShapesList().getList().begin(); Sitem != MAINDllOBJECT->getShapesList().getList().end(); Sitem++)
		{
			Shape* Csh = (Shape*)(*Sitem).second;
			if(Csh->ShapeUsedForTwoStepHoming())
			{
				CSh_Modified[ShapeCnt] = Csh;
				CSh_Original[ShapeCnt] = (Shape*)MAINDllOBJECT->getPPShapesList_Path().getList()[Csh->getId()];
				ShapeCnt++;
			}
		}
		if(ShapeCnt == 2) // Do two step Homing..
			TwoStep_Alignment(CSh_Original[0], CSh_Original[1], CSh_Modified[0], CSh_Modified[1]);
		else //Do the Base Plane Alignement..//
			ThreeShapes_Alignment(CSh_Original, CSh_Modified);
			//PlaneLinePoint_Alignment(CSh_Original[0], CSh_Original[1], CSh_Original[2], CSh_Modified[0], CSh_Modified[1], CSh_Modified[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0024", __FILE__, __FUNCSIG__); } 
}

//Two step alignment ..
void PartProgramFunctions::TwoStep_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Mod1, Shape* Csh_Mod2)
{
	try
	{
		switch (Csh_Orgn1->ShapeType)
		{
		case RapidEnums::SHAPETYPE::CIRCLE:
		//case RapidEnums::SHAPETYPE::CIRCLE3D:
		case RapidEnums::SHAPETYPE::ARC:
		case RapidEnums::SHAPETYPE::RPOINT:
		//case RapidEnums::SHAPETYPE::RPOINT3D:
		{
			switch (Csh_Orgn2->ShapeType)
			{
			case RapidEnums::SHAPETYPE::CIRCLE:
			//case RapidEnums::SHAPETYPE::CIRCLE3D:
			case RapidEnums::SHAPETYPE::ARC:
			case RapidEnums::SHAPETYPE::RPOINT:
			//case RapidEnums::SHAPETYPE::RPOINT3D:
				TwoPointHoming_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Mod1, Csh_Mod2);
				break;

			case RapidEnums::SHAPETYPE::LINE:
			//case RapidEnums::SHAPETYPE::LINE3D:
			case RapidEnums::SHAPETYPE::XLINE:
			case RapidEnums::SHAPETYPE::XRAY:
			//case RapidEnums::SHAPETYPE::XLINE3D:
				OnePointOneLineHoming_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Mod1, Csh_Mod2);
				break;

			default:
				break;
			}
			break;
		}

		case RapidEnums::SHAPETYPE::LINE:
		//case RapidEnums::SHAPETYPE::LINE3D:
		case RapidEnums::SHAPETYPE::XLINE:
		case RapidEnums::SHAPETYPE::XRAY:
		//case RapidEnums::SHAPETYPE::XLINE3D:
		{
			switch (Csh_Orgn2->ShapeType)
			{
				case RapidEnums::SHAPETYPE::CIRCLE:
				//case RapidEnums::SHAPETYPE::CIRCLE3D:
				case RapidEnums::SHAPETYPE::ARC:
				case RapidEnums::SHAPETYPE::RPOINT:
				//case RapidEnums::SHAPETYPE::RPOINT3D:
					TwoPointHoming_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Mod1, Csh_Mod2);
					break;

				case RapidEnums::SHAPETYPE::LINE:
				//case RapidEnums::SHAPETYPE::LINE3D:
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XRAY:
				//case RapidEnums::SHAPETYPE::XLINE3D:
					TwoLineHoming_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Mod1, Csh_Mod2);
					break;

				default:
					break;
			}
			break;
		}
		default:
			break;
		}

		//if(compareboth<int>(Csh_Orgn1->ShapeType, Csh_Orgn2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::CIRCLE) ||
		//   compareboth<int>(Csh_Orgn1->ShapeType, Csh_Orgn2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::RPOINT))
		//{
		//	TwoPointHoming_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Mod1, Csh_Mod2);
		//}
		//else if(compareboth<int>(Csh_Orgn1->ShapeType, Csh_Orgn2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::LINE) ||
		//	compareboth<int>(Csh_Orgn1->ShapeType, Csh_Orgn2->ShapeType, RapidEnums::SHAPETYPE::RPOINT, RapidEnums::SHAPETYPE::LINE) ||
		//	compareboth<int>(Csh_Orgn1->ShapeType, Csh_Orgn2->ShapeType, RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::LINE))
		//{
		//	OnePointOneLineHoming_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Mod1, Csh_Mod2);
		//}
		//else if(compareboth<int>(Csh_Orgn1->ShapeType, Csh_Orgn2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::LINE))
		//{
		//	TwoLineHoming_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Mod1, Csh_Mod2);
		//}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0024", __FILE__, __FUNCSIG__); } 
}

//Base Plane alignment ..
void PartProgramFunctions::BasePlane_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3) 
{
	try
	{
		if(compareboth<int>(Csh_Orgn2->ShapeType, Csh_Orgn3->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::CIRCLE) ||
		   compareboth<int>(Csh_Orgn2->ShapeType, Csh_Orgn3->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::RPOINT))
		{
			PlanePointPoint_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Orgn3, Csh_Mod1, Csh_Mod2, Csh_Mod3);
		}
		else if(compareboth<int>(Csh_Orgn2->ShapeType, Csh_Orgn3->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::LINE) ||
				compareboth<int>(Csh_Orgn2->ShapeType, Csh_Orgn3->ShapeType, RapidEnums::SHAPETYPE::RPOINT, RapidEnums::SHAPETYPE::LINE))
		{
			PlaneLinePoint_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Orgn3, Csh_Mod1, Csh_Mod2, Csh_Mod3);
		}
		else if(compareboth<int>(Csh_Orgn2->ShapeType, Csh_Orgn3->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::LINE))
		{
			PlaneLineLine_Alignment(Csh_Orgn1, Csh_Orgn2, Csh_Orgn3, Csh_Mod1, Csh_Mod2, Csh_Mod3);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0024", __FILE__, __FUNCSIG__); } 
}

//Two point alignment...
void PartProgramFunctions::TwoPointHoming_Alignment(Shape* Csh_Original1, Shape* Csh_Original2, Shape* Csh_Modified1, Shape* Csh_Modified2)
{
	try
	{
		double Points1_Act[2], Points2_Act[2], Points1_Mod[2], Points2_Mod[2], TransformM[9];

		if(Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			((Circle*)Csh_Original1)->getCenter()->FillDoublePointer(&Points1_Act[0]);
			((Circle*)Csh_Modified1)->getCenter()->FillDoublePointer(&Points1_Mod[0]);
		}
		else if(Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
		{
			((RPoint*)Csh_Original1)->getPosition()->FillDoublePointer(&Points1_Act[0]);
			((RPoint*)Csh_Modified1)->getPosition()->FillDoublePointer(&Points1_Mod[0]);
		}

		if(Csh_Original2->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh_Original2->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			((Circle*)Csh_Original2)->getCenter()->FillDoublePointer(&Points2_Act[0]);
			((Circle*)Csh_Modified2)->getCenter()->FillDoublePointer(&Points2_Mod[0]);
		}
		else if(Csh_Original2->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
		{
			((RPoint*)Csh_Original2)->getPosition()->FillDoublePointer(&Points2_Act[0]);
			((RPoint*)Csh_Modified2)->getPosition()->FillDoublePointer(&Points2_Mod[0]);
		}
		RMATH2DOBJECT->TransformM_TwoPointAlign(&Points1_Act[0], &Points2_Act[0], &Points1_Mod[0], &Points2_Mod[0], &TransformM[0], &this->TwoStepRotationAngle, this->TwoStepOrigin);
		TrnasformProgram_TwoStepHoming(&TransformM[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0025", __FILE__, __FUNCSIG__); }
}

//one point one line alignment..
void PartProgramFunctions::OnePointOneLineHoming_Alignment(Shape* Csh_Original1, Shape* Csh_Original2, Shape* Csh_Modified1, Shape* Csh_Modified2)
{
	try
	{
		double Point_Act[2], Slope_Act, Intercept_Act, Point_Mod[2], Slope_Mod, Intercept_Mod, TransformM[9];
		if(Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::LINE || Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::XLINE || Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
		{
			Slope_Act = ((Line*)Csh_Original1)->Angle();
			Slope_Mod = ((Line*)Csh_Modified1)->Angle();
			Intercept_Act = ((Line*)Csh_Original1)->Intercept();
			Intercept_Mod = ((Line*)Csh_Modified1)->Intercept();
			if(Csh_Original2->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh_Original2->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				((Circle*)Csh_Original2)->getCenter()->FillDoublePointer(&Point_Act[0]);
				((Circle*)Csh_Modified2)->getCenter()->FillDoublePointer(&Point_Mod[0]);
			}
			else if(Csh_Original2->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
			{
				((RPoint*)Csh_Original2)->getPosition()->FillDoublePointer(&Point_Act[0]);
				((RPoint*)Csh_Modified2)->getPosition()->FillDoublePointer(&Point_Mod[0]);
			}
		}
		else
		{
			Slope_Act = ((Line*)Csh_Original2)->Angle();
			Slope_Mod = ((Line*)Csh_Modified2)->Angle();
			Intercept_Act = ((Line*)Csh_Original2)->Intercept();
			Intercept_Mod = ((Line*)Csh_Modified2)->Intercept();
			if(Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				((Circle*)Csh_Original1)->getCenter()->FillDoublePointer(&Point_Act[0]);
				((Circle*)Csh_Modified1)->getCenter()->FillDoublePointer(&Point_Mod[0]);
			}
			else if(Csh_Original1->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
			{
				((RPoint*)Csh_Original1)->getPosition()->FillDoublePointer(&Point_Act[0]);
				((RPoint*)Csh_Modified1)->getPosition()->FillDoublePointer(&Point_Mod[0]);
			}
		}
		RMATH2DOBJECT->TransformM_PointLineAlign(&Point_Act[0], Slope_Act, Intercept_Act, &Point_Mod[0], Slope_Mod, Intercept_Mod, &TransformM[0], &this->TwoStepRotationAngle, this->TwoStepOrigin);
		TrnasformProgram_TwoStepHoming(&TransformM[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0026", __FILE__, __FUNCSIG__); } 
}

//two line alignment..
void PartProgramFunctions::TwoLineHoming_Alignment(Shape* Csh_Original1, Shape* Csh_Original2, Shape* Csh_Modified1, Shape* Csh_Modified2)
{
	try
	{
		double TransformMatrix[9] = {0};
		double Line1Param[2] = {((Line*)Csh_Original1)->Angle(), ((Line*)Csh_Original1)->Intercept()};
		double Line2Param[2] = {((Line*)Csh_Original2)->Angle(), ((Line*)Csh_Original2)->Intercept()};
		double Line3Param[2] = {((Line*)Csh_Modified1)->Angle(), ((Line*)Csh_Modified1)->Intercept()};
		double Line4Param[2] = {((Line*)Csh_Modified2)->Angle(), ((Line*)Csh_Modified2)->Intercept()};
		RMATH2DOBJECT->TransformM_TwoLineAlign(&Line1Param[0], &Line2Param[0], &Line3Param[0], &Line4Param[0], &TransformMatrix[0], &this->TwoStepRotationAngle, this->TwoStepOrigin);
		TrnasformProgram_TwoStepHoming(&TransformMatrix[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0027", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::PlaneLinePoint_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3)
{
	try
	{
		double OPlaneParameters[4], OLineParameters[6], OPointParameter[3];
		double MPlaneParameters[4], MLineParameters[6], MPointParameter[3];
		((Plane*)Csh_Orgn1)->getParameters(&OPlaneParameters[0]);
		((Plane*)Csh_Mod1)->getParameters(&MPlaneParameters[0]);
		((Line*)Csh_Orgn2)->getParameters(&OLineParameters[0]);
		((Line*)Csh_Mod2)->getParameters(&MLineParameters[0]);
		((RPoint*)Csh_Orgn3)->getPosition()->FillDoublePointer(&OPointParameter[0], 3);
		((RPoint*)Csh_Mod3)->getPosition()->FillDoublePointer(&MPointParameter[0], 3);
		BasePlane_AlignmentTransform(&OPlaneParameters[0], &OLineParameters[0], &OPointParameter[0], &MPlaneParameters[0], &MLineParameters[0], &MPointParameter[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0024", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::ThreeShapes_Alignment(Shape** shapesOriginal, Shape** shapesModified)
{
	try
	{
		list<list<double>> ShapesParam1, ShapesParam2;
		double OPlaneParameters[4], OLineParameters[6], OPointParameter[3];
		double MPlaneParameters[4], MLineParameters[6], MPointParameter[3];
		for(int i = 0; i < 3; i++)
			HELPEROBJECT->getShapeDimension(shapesOriginal[i], &ShapesParam1);
		for(int i = 0; i < 3; i++)
			HELPEROBJECT->getShapeDimension(shapesModified[i], &ShapesParam2);
		if(!HELPEROBJECT->Co_OrdinateSystem_From_ThreeShape(ShapesParam1, OPlaneParameters, OLineParameters, OPointParameter)) return;
		if(!HELPEROBJECT->Co_OrdinateSystem_From_ThreeShape(ShapesParam2, MPlaneParameters, MLineParameters, MPointParameter)) return;
		BasePlane_AlignmentTransform(&OPlaneParameters[0], &OLineParameters[0], &OPointParameter[0], &MPlaneParameters[0], &MLineParameters[0], &MPointParameter[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0024a", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::PlanePointPoint_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3)
{
	try
	{
		double OPlaneParameters[4], OLineParameters[6];
		double MPlaneParameters[4], MLineParameters[6];
		double Points2_Act[3], Points2_Mod[3];
		((Plane*)Csh_Orgn1)->getParameters(&OPlaneParameters[0]);
		((Plane*)Csh_Mod1)->getParameters(&MPlaneParameters[0]);
		if(Csh_Orgn2->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		{
			((Circle*)Csh_Orgn2)->getCenter()->FillDoublePointer(&OLineParameters[0], 3);
			((Circle*)Csh_Mod2)->getCenter()->FillDoublePointer(&MLineParameters[0], 3);
		}
		else if(Csh_Orgn2->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
		{
			((RPoint*)Csh_Orgn2)->getPosition()->FillDoublePointer(&OLineParameters[0], 3);
			((RPoint*)Csh_Mod2)->getPosition()->FillDoublePointer(&MLineParameters[0], 3);
		}

		if(Csh_Orgn3->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		{
			((Circle*)Csh_Orgn3)->getCenter()->FillDoublePointer(&Points2_Act[0], 3);
			((Circle*)Csh_Mod3)->getCenter()->FillDoublePointer(&Points2_Mod[0], 3);
		}
		else if(Csh_Orgn3->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
		{
			((RPoint*)Csh_Orgn3)->getPosition()->FillDoublePointer(&Points2_Act[0], 3);
			((RPoint*)Csh_Mod3)->getPosition()->FillDoublePointer(&Points2_Mod[0], 3);
		}
	
		double O_length = sqrt(pow(OLineParameters[0] - Points2_Act[0], 2) + pow(OLineParameters[1] - Points2_Act[1], 2) + pow(OLineParameters[2] - Points2_Act[2], 2));
		double M_length = sqrt(pow(MLineParameters[0] - Points2_Mod[0], 2) + pow(MLineParameters[1] - Points2_Mod[1], 2) + pow(MLineParameters[2] - Points2_Mod[2], 2));
		double O_dirl = (OLineParameters[0] - Points2_Act[0])/O_length;
		double O_dirm = (OLineParameters[1] - Points2_Act[1])/O_length;
		double O_dirn = (OLineParameters[2] - Points2_Act[2])/O_length;

		double M_dirl = (MLineParameters[0] - Points2_Mod[0])/M_length;
		double M_dirm = (MLineParameters[1] - Points2_Mod[1])/M_length;
		double M_dirn = (MLineParameters[2] - Points2_Mod[2])/M_length;
	
		BasePlane_AlignmentTransform(&OPlaneParameters[0], &OLineParameters[0], &OLineParameters[0], &MPlaneParameters[0], &MLineParameters[0], &MLineParameters[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0024", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::PlaneLineLine_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0024", __FILE__, __FUNCSIG__); } 
}

void PartProgramFunctions::BasePlane_AlignmentTransform(double* O_Plane, double* O_Line, double* O_Point, double* M_Plane, double* M_Line, double* M_Point)
{
	try
	{
		double TransformMatrix[16] = {0}, OSystemOrigin[3] , NSystemOrigin[3];
		//Call Transform Matrix Calculation..
		if(O_Plane[0] * M_Plane[0] + O_Plane[1] * M_Plane[1] + O_Plane[2] * M_Plane[2] < 0) 
			for(int i = 0; i < 4; i++) M_Plane[i] = -1 * M_Plane[i];
		RMATH3DOBJECT->TransformationMatrix_BasePlaneAllignment(O_Plane, O_Line, O_Point, M_Plane, M_Line, M_Point, &TransformMatrix[0], &OSystemOrigin[0], &NSystemOrigin[0], BasePlaneWithFrameGrab());
		//First subtract the original base-plane origin from all the pts and entities
		ShiftShape_ForProbeHoming(Vector(-OSystemOrigin[0], -OSystemOrigin[1], -OSystemOrigin[2]));
		//Tranform the Shapes and Action Positions..
		TransformProgram_BasePlaneAlignment(&TransformMatrix[0]);
		//Next add the modified base-plane origin to all the pts and entities to bring it to position
		ShiftShape_ForProbeHoming(Vector(NSystemOrigin[0], NSystemOrigin[1], NSystemOrigin[2]));
		this->FirstMousedown_Partprogram_Manual(false);
		this->FirstMousedown_Partprogram(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0027", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::CheckForFrameGrab()
{
	try
	{
		for(RC_ITER pp = getPPActionlist().getList().begin(); pp != getPPActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				if(fb->FGtype != RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
				{
					BasePlaneWithFrameGrab(true);
				}
			}
		}
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0027b", __FILE__, __FUNCSIG__);}
}

//function to reset PPEdit alignment...........
void PartProgramFunctions::ResetPP_EditAlign()
{
	try
	{
		for(RC_ITER CItem = MAINDllOBJECT->getCurrentUCS().getPPShapes_Original().getList().begin(); CItem != MAINDllOBJECT->getCurrentUCS().getPPShapes_Original().getList().end(); CItem++)
		{										
			Shape* Current_Shape = (Shape*)(*CItem).second;
			Shape* Saved_Cshape = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[Current_Shape->getId()];
			Saved_Cshape->CopyOriginalProperties(Current_Shape);
		}

		/*for(RC_ITER CItem = MAINDllOBJECT->getCurrentUCS().getPPShapes_Path().getList().begin(); CItem != MAINDllOBJECT->getCurrentUCS().getPPShapes_Path().getList().end(); CItem++)
		{										
			Shape* Current_Shape = (Shape*)(*CItem).second;
			Shape* Saved_Cshape = (Shape*)MAINDllOBJECT->getCurrentUCS().getPPShapes_Path().getList()[Current_Shape->getId()];
			Saved_Cshape->CopyOriginalProperties(Current_Shape);
		}*/
		
		
		 for(RC_ITER CItem = MAINDllOBJECT->getCurrentUCS().getPPMeasures_Original().getList().begin(); CItem != MAINDllOBJECT->getCurrentUCS().getPPMeasures_Original().getList().end(); CItem++)
		{			
			DimBase* Cdim = (DimBase*)(*CItem).second;
			DimBase* Saved_CMeasure = (DimBase*)MAINDllOBJECT->getCurrentUCS().getDimensions().getList()[Cdim->getId()];
			Saved_CMeasure->CopyOriginalProperties(Cdim);
		}


		/*
		for(RC_ITER CItem = MAINDllOBJECT->getCurrentUCS().getPPMeasures_Path().getList().begin(); CItem != MAINDllOBJECT->getCurrentUCS().getPPMeasures_Path().getList().end(); CItem++)
		{			
			DimBase* Cdim = (DimBase*)(*CItem).second;
			DimBase* Saved_CMeasure = (DimBase*)MAINDllOBJECT->getCurrentUCS().getp.getDimensions().getList()[Cdim->getId()];
			Saved_CMeasure->CopyOriginalProperties(Cdim);
		}
	*/
		SaveCurrentAction(&MAINDllOBJECT->getActionsHistoryList());
		
		MAINDllOBJECT->UpdateShapesChanged();
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0027", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::SaveCurrentAction(RCollectionBase* PPColl)
{
	try
	{
		 for(RC_ITER Item_PPAction = PPColl->getList().begin(); Item_PPAction != PPColl->getList().end(); Item_PPAction++)
		{
			RAction* Caction = (RAction*)(*Item_PPAction).second;
			if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)Caction)->getFramegrab();
				fb->ActionFinished(false);
				fb->PartprogrmActionPassStatus(true);
				fb->AllPointsList.deleteAll();
				fb->PointActionIdList.clear();
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];				
					fb->PointDRO.set(ActionColl->AP_PointDRO.getX(), ActionColl->AP_PointDRO.getY(), ActionColl->AP_PointDRO.getZ(), ActionColl->AP_PointDRO.getR(), ActionColl->AP_PointDRO.getP());
					fb->points[0].set(ActionColl->AP_Points[0].getX(), ActionColl->AP_Points[0].getY(), ActionColl->AP_Points[0].getZ(), ActionColl->AP_Points[0].getR(), ActionColl->AP_Points[0].getP());
					fb->points[1].set(ActionColl->AP_Points[1].getX(), ActionColl->AP_Points[1].getY(), ActionColl->AP_Points[1].getZ(), ActionColl->AP_Points[1].getR(), ActionColl->AP_Points[1].getP());
					fb->points[2].set(ActionColl->AP_Points[2].getX(), ActionColl->AP_Points[2].getY(), ActionColl->AP_Points[2].getZ(), ActionColl->AP_Points[2].getR(), ActionColl->AP_Points[2].getP());
				}	
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)Caction)->getFramegrab();
				fb->ActionFinished(false);
				fb->PartprogrmActionPassStatus(true);
				fb->AllPointsList.deleteAll();
				fb->PointActionIdList.clear();
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];				
					fb->PointDRO.set(ActionColl->AP_PointDRO.getX(), ActionColl->AP_PointDRO.getY(), ActionColl->AP_PointDRO.getZ(), ActionColl->AP_PointDRO.getR(), ActionColl->AP_PointDRO.getP());
					fb->points[0].set(ActionColl->AP_Points[0].getX(), ActionColl->AP_Points[0].getY(), ActionColl->AP_Points[0].getZ(), ActionColl->AP_Points[0].getR(), ActionColl->AP_Points[0].getP());
					fb->points[1].set(ActionColl->AP_Points[1].getX(), ActionColl->AP_Points[1].getY(), ActionColl->AP_Points[1].getZ(), ActionColl->AP_Points[1].getR(), ActionColl->AP_Points[1].getP());
					fb->points[2].set(ActionColl->AP_Points[2].getX(), ActionColl->AP_Points[2].getY(), ActionColl->AP_Points[2].getZ(), ActionColl->AP_Points[2].getR(), ActionColl->AP_Points[2].getP());
				}				
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)Caction)->getFramegrab();
				fb->ActionFinished(false);
				fb->PartprogrmActionPassStatus(true);
				fb->AllPointsList.deleteAll();
				fb->PointActionIdList.clear();
				ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
				fb->PointDRO.set(ActionColl->AP_PointDRO.getX(), ActionColl->AP_PointDRO.getY(), ActionColl->AP_PointDRO.getZ(), ActionColl->AP_PointDRO.getR(), ActionColl->AP_PointDRO.getP());
				fb->points[0].set(ActionColl->AP_Points[0].getX(), ActionColl->AP_Points[0].getY(), ActionColl->AP_Points[0].getZ(), ActionColl->AP_Points[0].getR(), ActionColl->AP_Points[0].getP());
				fb->points[1].set(ActionColl->AP_Points[1].getX(), ActionColl->AP_Points[1].getY(), ActionColl->AP_Points[1].getZ(), ActionColl->AP_Points[1].getR(), ActionColl->AP_Points[1].getP());
				fb->points[2].set(ActionColl->AP_Points[2].getX(), ActionColl->AP_Points[2].getY(), ActionColl->AP_Points[2].getZ(), ActionColl->AP_Points[2].getR(), ActionColl->AP_Points[2].getP());
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)Caction)->getFramegrab();
				fb->ActionFinished(false);
				fb->PartprogrmActionPassStatus(true);
				fb->AllPointsList.deleteAll();
				fb->PointActionIdList.clear();
				ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
				fb->PointDRO.set(ActionColl->AP_PointDRO.getX(), ActionColl->AP_PointDRO.getY(), ActionColl->AP_PointDRO.getZ(), ActionColl->AP_PointDRO.getR(), ActionColl->AP_PointDRO.getP());
				fb->points[0].set(ActionColl->AP_Points[0].getX(), ActionColl->AP_Points[0].getY(), ActionColl->AP_Points[0].getZ(), ActionColl->AP_Points[0].getR(), ActionColl->AP_Points[0].getP());
				fb->points[1].set(ActionColl->AP_Points[1].getX(), ActionColl->AP_Points[1].getY(), ActionColl->AP_Points[1].getZ(), ActionColl->AP_Points[1].getR(), ActionColl->AP_Points[1].getP());
				fb->points[2].set(ActionColl->AP_Points[2].getX(), ActionColl->AP_Points[2].getY(), ActionColl->AP_Points[2].getZ(), ActionColl->AP_Points[2].getR(), ActionColl->AP_Points[2].getP());
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				((AddThreadMeasureAction*)Caction)->SelectTheFirstShape();
				((AddThreadMeasureAction*)Caction)->ThreadppCurrentaction = ((AddThreadMeasureAction*)Caction)->ActionCollection.getList().begin();
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)Caction)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)Caction)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					CthAction->getShape()->PointsList->deleteAll();
					CthAction->getShape()->PointsListOriginal->deleteAll();
					Shape* Csh = (Shape*)((AddThreadMeasureAction*)Caction)->ShapeCollection_ThreadWindow.getList()[CthAction->getShape()->getId()];
					Csh->FinishedThreadMeasurement(false);
					FramegrabBase* fb =	CthAction->getFramegrab();
					fb->ActionFinished(false);
					fb->PartprogrmActionPassStatus(true);
					fb->AllPointsList.deleteAll();
					fb->PointActionIdList.clear();
					ActionsPointCollection* ActionColl = PPActionPositionColl[CthAction->getId()];
					fb->PointDRO.set(ActionColl->AP_PointDRO.getX(), ActionColl->AP_PointDRO.getY(), ActionColl->AP_PointDRO.getZ(), ActionColl->AP_PointDRO.getR(), ActionColl->AP_PointDRO.getP());
					fb->points[0].set(ActionColl->AP_Points[0].getX(), ActionColl->AP_Points[0].getY(), ActionColl->AP_Points[0].getZ(), ActionColl->AP_Points[0].getR(), ActionColl->AP_Points[0].getP());
					fb->points[1].set(ActionColl->AP_Points[1].getX(), ActionColl->AP_Points[1].getY(), ActionColl->AP_Points[1].getZ(), ActionColl->AP_Points[1].getR(), ActionColl->AP_Points[1].getP());
					fb->points[2].set(ActionColl->AP_Points[2].getX(), ActionColl->AP_Points[2].getY(), ActionColl->AP_Points[2].getZ(), ActionColl->AP_Points[2].getR(), ActionColl->AP_Points[2].getP());
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
				((CircularInterPolationAction*)Caction)->SetCenter(ActionColl->AP_Points[0]);
				((CircularInterPolationAction*)Caction)->SetCurrentPosition(ActionColl->AP_Points[1]);
				((CircularInterPolationAction*)Caction)->SetNormalVector(ActionColl->AP_Points[2]);
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
				((LinearInterPolationAction*)Caction)->SetTargetPosition(ActionColl->AP_Points[0]);
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
				((ProfileScanAction*)Caction)->SetCurrentPos(ActionColl->AP_PointDRO);
				((ProfileScanAction*)Caction)->SetFirstPt(ActionColl->AP_Points[0]);
				((ProfileScanAction*)Caction)->SetSecondPt(ActionColl->AP_Points[1]);
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)Caction)->getFramegrab();
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];				
					fb->PointDRO.set(ActionColl->AP_PointDRO.getX(), ActionColl->AP_PointDRO.getY(), ActionColl->AP_PointDRO.getZ(), ActionColl->AP_PointDRO.getR(), ActionColl->AP_PointDRO.getP());
					fb->points[0].set(ActionColl->AP_Points[0].getX(), ActionColl->AP_Points[0].getY(), ActionColl->AP_Points[0].getZ(), ActionColl->AP_Points[0].getR(), ActionColl->AP_Points[0].getP());
					fb->points[1].set(ActionColl->AP_Points[1].getX(), ActionColl->AP_Points[1].getY(), ActionColl->AP_Points[1].getZ(), ActionColl->AP_Points[1].getR(), ActionColl->AP_Points[1].getP());
					fb->points[2].set(ActionColl->AP_Points[2].getX(), ActionColl->AP_Points[2].getY(), ActionColl->AP_Points[2].getZ(), ActionColl->AP_Points[2].getR(), ActionColl->AP_Points[2].getP());
				}	
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
					((EditCad1PtAction*)Caction)->SetPoint1(ActionColl->AP_Points[0]);
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
					((Cad1PtAlignAction*)Caction)->SetPoint1(ActionColl->AP_Points[0]);
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
					((Cad1Pt1LineAction*)Caction)->SetPoint1(ActionColl->AP_Points[0]);
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
					((Cad2PtAlignAction*)Caction)->SetPoint1(ActionColl->AP_Points[0]);
					((Cad2PtAlignAction*)Caction)->SetPoint2(ActionColl->AP_Points[1]);
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
					((EditCad2PtAction*)Caction)->SetPoint1(ActionColl->AP_Points[0]);
					((EditCad2PtAction*)Caction)->SetPoint2(ActionColl->AP_Points[1]);
				}
			}
			else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				if(PPActionPositionColl.end() != PPActionPositionColl.find(Caction->getId()))
				{
					ActionsPointCollection* ActionColl = PPActionPositionColl[Caction->getId()];
					((EditCad1Pt1LineAction*)Caction)->SetPoint1(ActionColl->AP_Points[0]);
				}
			}
		 }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0027", __FILE__, __FUNCSIG__); }
}

//Transform shapes for two step homing..
void PartProgramFunctions::TrnasformProgram_TwoStepHoming(double *TransformMatrix, bool RotateAroundPt, bool ActionsOnly)
{
	try
	{
		Vector Position, GotoOrigin;
		GotoOrigin.set(-TransformMatrix[2], -TransformMatrix[5]);
		Position.set(TransformMatrix[2], TransformMatrix[5]);
		TransformMatrix[2] = 0; TransformMatrix[5] = 0;
		if(!ActionsOnly)
		{
			for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
			{
				UCS* Cucs = (UCS*)(*Item_ucs).second;
				
				if (Cucs->getId() != 0) continue;

				for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
				{
					Shape* Cshape = (Shape*)(*Item_shape).second;
					if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
					if(Cshape->Normalshape())
					{
						if(RotateAroundPt)
						{
							Cshape->Translate(GotoOrigin);
							Cshape->Transform(TransformMatrix);
							Cshape->Translate(Position);
						}
						else if(!Cshape->ShapeUsedForTwoStepHoming())
						{
							Cshape->Transform(TransformMatrix);
							Cshape->Translate(Position);
						}
					}
				}
				for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
				{
					Shape* Cshape = (Shape*)(*Item_shape).second;
					if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
					if(Cshape->Normalshape())
					{
						if(RotateAroundPt)
							Cshape->Translate(GotoOrigin);
						Cshape->Transform(TransformMatrix);
						Cshape->Translate(Position);
					}
				}
			}
		
			for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
			{
				UCS* Cucs = (UCS*)(*Item_ucs).second;
				
				if (Cucs->getId() != 0) continue;

				for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
				{
					DimBase* Cdim = (DimBase*)(*Item_dim).second;
					if(RotateAroundPt)
						Cdim->TranslatePosition(GotoOrigin);
					Cdim->TransformPosition(TransformMatrix);
					Cdim->TranslatePosition(Position);
					Cdim->UpdateMeasurement();
				}
				for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
				{
					DimBase* Cdim = (DimBase*)(*Item_dim).second;
					if(RotateAroundPt)
						Cdim->TranslatePosition(GotoOrigin);
					Cdim->TransformPosition(TransformMatrix);
					Cdim->TranslatePosition(Position);
					if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
						Cdim->UpdateMeasurement();
				}
			}
		}

		while (this->TwoStepRotationAngle > M_PI_2)
			this->TwoStepRotationAngle -= M_PI;
		//Now that all shapes have been updated, let us call all UCS dependent on these shapes to update themselves
		for (RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;

			if (Cucs->getId() != 0)
			{
				if (Cucs->UCSMode() == 1)
				{
					double transformM[9] = { 0 };
					if (Cucs->UCSProjectionType() == 1)
					{
						transformM[1] = 1;   transformM[5] = 1;   transformM[6] = 1;
					}
					else if (Cucs->UCSProjectionType() == 0)
					{
						transformM[0] = 1;   transformM[4] = 1;   transformM[8] = 1;
					}
					else if (Cucs->UCSProjectionType() == 2)
					{
						transformM[0] = 1;   transformM[5] = 1;   transformM[7] = 1;
					}

					Vector difference(-Cucs->CenterPt->getX(), -Cucs->CenterPt->getY(), -Cucs->CenterPt->getZ());
					for (RC_ITER item = Cucs->getShapes().getList().begin(); item != Cucs->getShapes().getList().end(); item++)
					{
						Shape* CShape = (Shape*)(*item).second;
						if (CShape->getMCSParentShape() != NULL)
						{
							Shape* ParentShape = (Shape*)CShape->getMCSParentShape();
							CShape->CopyShapeParameters(ParentShape);
							if (CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
							{
								((CloudPoints*)CShape)->PointsListOriginal->deleteAll();
								((CloudPoints*)CShape)->PointsList->deleteAll();
								((CloudPoints*)CShape)->CopyPoints(ParentShape);
							}
							CShape->Translate(difference);
							CShape->Transform(transformM);
							CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);
						}
					}
					//Cucs->UCSPoint.set(Cucs->CenterPt->getX(), Cucs->CenterPt->getY(), Cucs->CenterPt->getZ());
				}
				else if (Cucs->UCSMode() == 2)
				{
					//MAINDllOBJECT->changeUCS(Cucs->getId());
					Cucs->UCSPt_B2S.set(Cucs->UCSPoint);
					Cucs->UCSAngle_B2S = Cucs->UCSangle();
					double UcsAngle = 0, Intercept = 0;
					if (Cucs->UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
					{
						switch (Cucs->AxisLine->ShapeType)
						{
						case RapidEnums::SHAPETYPE::XLINE:
						case RapidEnums::SHAPETYPE::XRAY:
						case RapidEnums::SHAPETYPE::LINE:
							UcsAngle = ((Line*)Cucs->AxisLine)->Angle();
							Intercept = ((Line*)Cucs->AxisLine)->Intercept();
							RMATH2DOBJECT->Angle_FirstFourthQuad(&UcsAngle);
							break;
						}
					}
					else
					{
						UcsAngle = Cucs->UCSangle();
						Intercept = Cucs->UCSIntercept();
					}
					Cucs->SetParameters(UcsAngle, Cucs->CenterPt);
				}
				else if (Cucs->UCSMode() == 3)
				{
					double p1[2] = { 0, 0 }, UcsAngle = Cucs->UCSangle();
					double transformM[9] = { 0 };
					RMATH2DOBJECT->TransformM_RotateAboutPoint(&p1[0], UcsAngle, &transformM[0]);
					RMATH2DOBJECT->TransformM_RotateAboutPoint(&p1[0], -UcsAngle, &Cucs->transform[0]);
					for (RC_ITER item = Cucs->getShapes().getList().begin(); item != Cucs->getShapes().getList().end(); item++)
					{
						Shape* CShape = (Shape*)(*item).second;
						if (CShape->getMCSParentShape() != NULL)
						{
							Shape* ParentShape = (Shape*)CShape->getMCSParentShape();
							CShape->CopyShapeParameters(ParentShape);
							CShape->Transform(&transformM[0]);
							CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);
						}
					}
				}
				else if (Cucs->UCSMode() == 4)
				{
					list<list<double>> ShapesParam;
					list<list<double>>* ShpParmPntr = &ShapesParam;
					for (map<int, Shape*>::iterator It1 = Cucs->ShapesCol.begin(); It1 != Cucs->ShapesCol.end(); It1++)
						HELPEROBJECT->getShapeDimension(It1->second, ShpParmPntr);
					for (map<int, Vector*>::iterator It2 = Cucs->PointsCol.begin(); It2 != Cucs->PointsCol.end(); It2++)
					{
						It2->second = MAINDllOBJECT->getVectorPointer_UCS(It2->second, MAINDllOBJECT->getUCS(0), true);
						HELPEROBJECT->getPointParam(It2->second, ShpParmPntr);
					}
					double TransformM[16] = { 0 }, InversOldTransM[16] = { 0 }, DiffTransM[16] = { 0 }, PCenter[3];
					if (!HELPEROBJECT->UCSBasePlaneTransformM(&ShapesParam, TransformM)) return;
					RMATH2DOBJECT->OperateMatrix4X4(&(Cucs->transform[0]), 4, 1, InversOldTransM);
					int Order1[2] = { 4,4 }, Order2[2] = { 4,4 };
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, InversOldTransM, Order2, DiffTransM);
					for (int i = 0; i < 16; i++) { Cucs->transform[i] = TransformM[i]; }
					for (RC_ITER item = Cucs->getShapes().getList().begin(); item != Cucs->getShapes().getList().end(); item++)
					{
						Shape* CShape = (Shape*)(*item).second;
						if (CShape->getMCSParentShape() != NULL)
						{
							Shape* ParentShape = (Shape*)CShape->getMCSParentShape();
							CShape->CopyShapeParameters(ParentShape);
							if (CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
							{
								((CloudPoints*)CShape)->PointsListOriginal->deleteAll();
								((CloudPoints*)CShape)->PointsList->deleteAll();
								((CloudPoints*)CShape)->CopyPoints(ParentShape);
							}
							CShape->AlignToBasePlane(TransformM);
							CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);
						}
						else
							((ShapeWithList*)CShape)->TransformShapePoints(DiffTransM);
					}
				}
				//Vector Ori; Ori.set(Cucs->UCSPoint.getX() - this->TwoStepOrigin[0], Cucs->UCSPoint.getY() - this->TwoStepOrigin[1]);
				//Cucs->SetParameters(Cucs->UCSangle() - this->TwoStepRotationAngle, &Ori);
			}
		}

		RCollectionBase *CurrentAction;
		CurrentAction = &getPPActionlist();
		//RotateAroundPt = true;

		for(RC_ITER pp = CurrentAction->getList().begin(); pp != CurrentAction->getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				if(RotateAroundPt)
					fb->TranslatePosition(GotoOrigin, ((AddPointAction*)action)->getShape()->UcsId());
				fb->TrasformthePosition(TransformMatrix, ((AddPointAction*)action)->getShape()->UcsId());
				fb->TranslatePosition(Position, ((AddPointAction*)action)->getShape()->UcsId());
				//fb->Transform_2S_Homing(TransformMatrix, ((AddPointAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				if(RotateAroundPt)
					((ProfileScanAction*)action)->Translate(GotoOrigin);
				((ProfileScanAction*)action)->Transform(TransformMatrix);
				((ProfileScanAction*)action)->Translate(Position);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				fb->TrasformthePosition(TransformMatrix, ((PerimeterAction*)action)->getShape()->UcsId());
				fb->TranslatePosition(Position, ((PerimeterAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				if(RotateAroundPt)
					fb->TranslatePosition(GotoOrigin, MAINDllOBJECT->getCurrentUCS().getId());
				fb->TrasformthePosition(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
				fb->TranslatePosition(Position, MAINDllOBJECT->getCurrentUCS().getId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				if(RotateAroundPt)
					fb->TranslatePosition(GotoOrigin, MAINDllOBJECT->getCurrentUCS().getId());
				fb->TrasformthePosition(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
				fb->TranslatePosition(Position, MAINDllOBJECT->getCurrentUCS().getId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				if(RotateAroundPt)
					fb->TranslatePosition(GotoOrigin, ((AreaShapeAction*)action)->getShape()->UcsId());
				fb->TrasformthePosition(TransformMatrix, ((AreaShapeAction*)action)->getShape()->UcsId());
				fb->TranslatePosition(Position, ((AreaShapeAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)action)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)action)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					if(RotateAroundPt)
						fb->TranslatePosition(GotoOrigin, ((AddThreadPointAction*)action)->getShape()->UcsId());
					fb->TrasformthePosition(TransformMatrix, CthAction->getShape()->UcsId());
					fb->TranslatePosition(Position, CthAction->getShape()->UcsId());
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				if(RotateAroundPt)
					((CircularInterPolationAction*)action)->Translate(GotoOrigin);
				((CircularInterPolationAction*)action)->Transform(TransformMatrix);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				if(RotateAroundPt)
					((LinearInterPolationAction*)action)->Translate(GotoOrigin);
				((LinearInterPolationAction*)action)->Transform(TransformMatrix);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				Vector AlignPos, AfterTransform;
				AlignPos = *((EditCad1PtAction*)action)->getPoint1();
				if(RotateAroundPt)
					AlignPos += GotoOrigin;
				AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(),  AlignPos.getZ());
				AfterTransform += Position;
				((EditCad1PtAction*)action)->SetPoint1(AfterTransform);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				if(((Cad1PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos, AfterTransform;
					AlignPos = *((Cad1PtAlignAction*)action)->getPoint1();
					if(RotateAroundPt)
						AlignPos += GotoOrigin;
					AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(),  AlignPos.getZ());
					AfterTransform += Position;
					((Cad1PtAlignAction*)action)->SetPoint1(AfterTransform);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				if(((Cad2PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos, AfterTransform;
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint1();
					if(RotateAroundPt)
						AlignPos += GotoOrigin;
					AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(), AlignPos.getZ());
					AfterTransform += Position;
					((Cad2PtAlignAction*)action)->SetPoint1(AfterTransform);
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint2();
					if(RotateAroundPt)
						AlignPos += GotoOrigin;
					AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(), AlignPos.getZ());
					AfterTransform += Position;
					((Cad2PtAlignAction*)action)->SetPoint2(AfterTransform);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				if(((Cad1Pt1LineAction*)action)->IsEdited)
				{
					Vector AlignPos, AfterTransform;
					AlignPos = *((Cad1Pt1LineAction*)action)->getPoint1();
					if(RotateAroundPt)
						AlignPos += GotoOrigin;
					AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(),  AlignPos.getZ());
					AfterTransform += Position;
					((Cad1Pt1LineAction*)action)->SetPoint1(AfterTransform);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				Vector AlignPos, AfterTransform;
				if(((EditCad2PtAction*)action)->getActionCount() == 1)
				{					
					AlignPos = *((EditCad2PtAction*)action)->getPoint1();
					if(RotateAroundPt)
						AlignPos += GotoOrigin;
					AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(),  AlignPos.getZ());
					AfterTransform += Position;
					((EditCad2PtAction*)action)->SetPoint1(AfterTransform);
				}
				else if(((EditCad2PtAction*)action)->getActionCount() == 2)
				{
					AlignPos = *((EditCad2PtAction*)action)->getPoint2();
					if(RotateAroundPt)
						AlignPos += GotoOrigin;
					AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(),  AlignPos.getZ());
					AfterTransform += Position;				
					((EditCad2PtAction*)action)->SetPoint2(AfterTransform);
				}	
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				Vector AlignPos, AfterTransform;
				AlignPos = *((EditCad1Pt1LineAction*)action)->getPoint1();
				if(RotateAroundPt)
					AlignPos += GotoOrigin;
				AfterTransform = MAINDllOBJECT->TransformMultiply(TransformMatrix, AlignPos.getX(), AlignPos.getY(),  AlignPos.getZ());
				AfterTransform += Position;
				((EditCad1Pt1LineAction*)action)->SetPoint1(AfterTransform);
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0028", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::TransformProgram_BasePlaneAlignment(double *TransformMatrix)
{
	try
	{
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
				{
					if(!Cshape->ShapeUsedForTwoStepHoming())
						Cshape->AlignToBasePlane(TransformMatrix);
				}
			}
			for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
					Cshape->AlignToBasePlane(TransformMatrix);
			}
		}
		
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->BasePlaneAlignment(TransformMatrix);
				Cdim->UpdateMeasurement();
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->BasePlaneAlignment(TransformMatrix);
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
		}
		for(RC_ITER pp = getPPArrangedActionlist().getList().begin(); pp != getPPArrangedActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				if(fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
				{
					//First add the probe offset value
					fb->PointDRO += Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
					//Next transform
					fb->TrasformthePosition_PlaneAlign(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
					//Subtract the probe offset value from the transformed point
					fb->PointDRO -= Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
					fb->ProbeDirection = RapidEnums::PROBEDIRECTION(RMATH2DOBJECT->GetTransformedProbeDirection(TransformMatrix, fb->OriginalProbeDirection));
				}
				else
					fb->TrasformthePosition_PlaneAlign(TransformMatrix, ((AddPointAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				if(fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
				{
					//First add the probe offset value
					fb->PointDRO += Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
					//Next transform
					fb->TrasformthePosition_PlaneAlign(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
					//Subtract the probe offset value from the transformed point
					fb->PointDRO -= Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
					fb->ProbeDirection = RapidEnums::PROBEDIRECTION(RMATH2DOBJECT->GetTransformedProbeDirection(TransformMatrix, fb->OriginalProbeDirection));
				}
				else
					fb->TrasformthePosition_PlaneAlign(TransformMatrix, ((PerimeterAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				//First add the probe offset value
				fb->PointDRO += Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
				//Next transform
				fb->TrasformthePosition_PlaneAlign(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
				//Subtract the probe offset value from the transformed point
				fb->PointDRO -= Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				fb->PointDRO += Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
				fb->TrasformthePosition_PlaneAlign(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
				fb->PointDRO -= Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();			
				//First add the probe offset value
				fb->PointDRO += Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
				//Next transform
				fb->TrasformthePosition_PlaneAlign(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
				//Subtract the probe offset value from the transformed point
				fb->PointDRO -= Vector(MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)action)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)action)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					fb->TrasformthePosition_PlaneAlign(TransformMatrix, CthAction->getShape()->UcsId());
				}
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0028", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::TranslateUCSParameters(Vector& difference, UCS& CurrentUCS)
{
	try
	{
		for(RC_ITER Item_shape = CurrentUCS.getShapes().getList().begin(); Item_shape != CurrentUCS.getShapes().getList().end(); Item_shape++)
		{
			Shape* Cshape = (Shape*)(*Item_shape).second;
			if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
			if(Cshape->Normalshape())
			{
				Shape* Cshape1 = (Shape*)CurrentUCS.getPPShapes_Path().getList()[Cshape->getId()];
				Cshape->Translate(difference);
				Cshape1->Translate(difference);
			}
		}
		for(RC_ITER Item_dim = CurrentUCS.getDimensions().getList().begin(); Item_dim != CurrentUCS.getDimensions().getList().end(); Item_dim++)
		{
			DimBase* Cdim = (DimBase*)(*Item_dim).second;
			DimBase* Cdim1 = (DimBase*)CurrentUCS.getPPMeasures_Path().getList()[Cdim->getId()];
			Cdim->TranslatePosition(difference);
			Cdim->UpdateMeasurement();
			Cdim1->TranslatePosition(difference);
			Cdim1->UpdateMeasurement();
		}
		MAINDllOBJECT->updatePPGraphics(true);
		/*for(RC_ITER pp = getPPArrangedActionlist().getList().begin(); pp != getPPArrangedActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				if(((AddPointAction*)action)->getShape()->UcsId() != CurrentUCS.getId()) continue;
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				fb->TranslatePosition(difference, ((AddPointAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				if(((PerimeterAction*)action)->getShape()->UcsId() != CurrentUCS.getId()) continue;
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				fb->TranslatePosition(difference, ((PerimeterAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				fb->TranslatePosition(difference, MAINDllOBJECT->getCurrentUCS().getId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				fb->TranslatePosition(difference, MAINDllOBJECT->getCurrentUCS().getId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				if(((AreaShapeAction*)action)->getShape()->UcsId() != CurrentUCS.getId()) continue;
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				fb->TranslatePosition(difference, ((AreaShapeAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)action)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)action)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					if(CthAction->getShape()->UcsId() != CurrentUCS.getId()) continue;
					fb->TranslatePosition(difference, CthAction->getShape()->UcsId());
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				((CircularInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				((LinearInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				((ProfileScanAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				Vector temp;
				temp = fb->PointDRO;				
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1PtAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1PtAction*)action)->SetPoint1(AlignPos);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				if(((Cad1PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1PtAlignAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				if(((Cad1Pt1LineAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1Pt1LineAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1Pt1LineAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				if(((Cad2PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint1(AlignPos);
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint2();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint2(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				Vector AlignPos;
				if(((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getActionCount() == 1)
				{					
					AlignPos = *((EditCad2PtAction*)action)->getPoint1();
					AlignPos += difference;
					((EditCad2PtAction*)action)->SetPoint1(AlignPos);
				}
				else if(((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getActionCount() == 2)
				{
					AlignPos = *((EditCad2PtAction*)action)->getPoint2();
					AlignPos += difference;				
					((EditCad2PtAction*)action)->SetPoint2(AlignPos);
				}	
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1Pt1LineAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1Pt1LineAction*)action)->SetPoint1(AlignPos);
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0028", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::TransformUCSParameters(double *TransformMatrix, UCS& CurrentUCS)
{
	try
	{
		for(RC_ITER Item_shape = CurrentUCS.getShapes().getList().begin(); Item_shape != CurrentUCS.getShapes().getList().end(); Item_shape++)
		{
			Shape* Cshape = (Shape*)(*Item_shape).second;
			if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
			if(Cshape->Normalshape())
			{
				Shape* Cshape1 = (Shape*)CurrentUCS.getPPShapes_Path().getList()[Cshape->getId()];
				Cshape->Transform(TransformMatrix);
				Cshape1->Transform(TransformMatrix);
			}
		}
		for(RC_ITER Item_dim = CurrentUCS.getDimensions().getList().begin(); Item_dim != CurrentUCS.getDimensions().getList().end(); Item_dim++)
		{
			DimBase* Cdim = (DimBase*)(*Item_dim).second;
			DimBase* Cdim1 = (DimBase*)CurrentUCS.getPPMeasures_Path().getList()[Cdim->getId()];
			Cdim->TransformPosition(TransformMatrix);
			Cdim->UpdateMeasurement();
			Cdim1->TransformPosition(TransformMatrix);
			Cdim1->UpdateMeasurement();
		}
		for(RC_ITER pp = getPPArrangedActionlist().getList().begin(); pp != getPPArrangedActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				if(((AddPointAction*)action)->getShape()->UcsId() != CurrentUCS.getId()) continue;
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				fb->TrasformthePosition(TransformMatrix, ((AddPointAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				if(((PerimeterAction*)action)->getShape()->UcsId() != CurrentUCS.getId()) continue;
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				fb->TrasformthePosition(TransformMatrix, ((PerimeterAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				fb->TrasformthePosition(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				fb->TrasformthePosition(TransformMatrix, MAINDllOBJECT->getCurrentUCS().getId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				if(((AreaShapeAction*)action)->getShape()->UcsId() != CurrentUCS.getId()) continue;
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				fb->TrasformthePosition(TransformMatrix, ((AreaShapeAction*)action)->getShape()->UcsId());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)action)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)action)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					if(CthAction->getShape()->UcsId() != CurrentUCS.getId()) continue;
					fb->TrasformthePosition(TransformMatrix, CthAction->getShape()->UcsId());
				}
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0028", __FILE__, __FUNCSIG__); }
}

//do homing..Shift the shapes for run clicked..
void PartProgramFunctions::DoHoming(bool UpdateGraphics)
{
	try
	{
		if(IsPartProgramRunning() && FirstMousedown_Partprogram())
		{
			Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
			Vector temp(CurrDRO.getX(), CurrDRO.getY(), CurrDRO.getZ() , CurrDRO.getR());
			if (ShiftProgram || this->ProgramMadeUsingTwoStepHoming())
				ShiftShape_ForFirstMouseDown(temp, ReferencePtAsHomePos());
			else
				HomePosition.set(temp);
		}
		if (UpdateGraphics) MAINDllOBJECT->updatePPGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0029", __FILE__, __FUNCSIG__); } 
}

//Run partpogram..//
void PartProgramFunctions::RunCurrentStep()
{
	try
	{
		//if (dynamic_cast<RotationScanHandler*>(MAINDllOBJECT->getCurrentHandler()))
		//	MessageBox(NULL, L"We got RSScan Handler", L"Rapid-I", MB_TOPMOST);

		MAINDllOBJECT->getCurrentHandler()->PartProgramData();
		if(getCurrentAction() == NULL)
		{
			if(dynamic_cast<DefaultHandler*>(MAINDllOBJECT->getCurrentHandler())) 
				return;
			MAINDllOBJECT->setHandler(new DefaultHandler());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0030", __FILE__, __FUNCSIG__); } 
}

//update current handler according to the currecnt action..
void PartProgramFunctions::drawRunpp()
{
	try
	{
		MAINDllOBJECT->Derivedmodeflag = false;
		if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			Shape *s = ((AddPointAction*)(*ppCurrentaction).second)->getShape();
			if(s->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(s->UcsId());
			if(!s->selected())
				MAINDllOBJECT->selectShape(s->getId(), false);
			FramegrabBase *fb = ((AddPointAction*)(*ppCurrentaction).second)->getFramegrab();
			//if ()
			{
				//if((fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR) || (fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE))
				//	MAINDllOBJECT->ShowExplicitIntersectionPoints.operator()((bool) false);					//vinod.. false means red snap mode in fe.. to take a point using crossmark and flexiblecrosshair
				//else
				//	MAINDllOBJECT->ShowExplicitIntersectionPoints.operator()((bool) true);		//vinod.. true means orange snap mode in fe.. for remaining action for a part programm run..		
				switch(fb->FGtype)
				{
				case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ARCFRAMEGRAB:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
					DoHoming(false);
					MAINDllOBJECT->setHandler(new RotationScanHandler());
					break;				
				case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ANGULARRECTANGLEFRAMEGRAB:
					//MessageBox(NULL, L"Set Rotation Handler Correctly", L"Rapid-I", MB_TOPMOST);
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
					DoHoming(false);
					MAINDllOBJECT->setHandler(new RotationScanHandler());
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_CIRCLEFRAMEGRAB:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
					DoHoming(false);
					MAINDllOBJECT->setHandler(new RotationScanHandler());
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_RECTANGLEFRAMEGRAB:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
					DoHoming(false);
					MAINDllOBJECT->setHandler(new RotationScanHandler());
					break;	
				case RapidEnums::RAPIDFGACTIONTYPE::ROTATIONSCAN_FIXEDRECTANGLE:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
					DoHoming(false);
					MAINDllOBJECT->setHandler(new RotationScanHandler());
					break;	
				case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
					//MAINDllOBJECT->setHandler(new FrameGrabArc(fb));
					DoHoming(false);
					MAINDllOBJECT->setHandler(new FrameGrabArc(fb));
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MOUSECLICK_GRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
					/*MAINDllOBJECT->setHandler(new FrameGrabEdgeMouseClick(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new FrameGrabEdgeMouseClick(fb));
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
					/*MAINDllOBJECT->setHandler(new FrameGrabAngularRect(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new FrameGrabAngularRect(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
					/*MAINDllOBJECT->setHandler(new FrameGrabCircle(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new FrameGrabCircle(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
				case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
					/*MAINDllOBJECT->setHandler(new FrameGrabRectangle(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new FrameGrabRectangle(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
					/*MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
					MAINDllOBJECT->setHandler(new FlexibleCrossHairHandler(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::SCANCROSSHAIR:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
					/*MAINDllOBJECT->setHandler(new ScanModeHandler(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new ScanModeHandler(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::LIVESCANCROSSHAIR:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
					/*MAINDllOBJECT->setHandler(new LiveScanModeHandler(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new LiveScanModeHandler(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::SELECTPOINTCROSSHAIR:
					MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
					/*MAINDllOBJECT->setHandler(new SelectPointHandler(fb));*/
					DoHoming(false);
					MAINDllOBJECT->setHandler(new SelectPointHandler(fb));
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::ODMEASUREMENT:
					MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::ODMEASURE_HANDLER);
					MAINDllOBJECT->getCurrentHandler()->baseaction = fb;
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::IDMEASUREMENT:
					MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::IDMEASURE_HANDLER);
					MAINDllOBJECT->getCurrentHandler()->baseaction = fb;
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE:
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
					if(MAINDllOBJECT->getCurrentHandler()->getClicksDone() == 0 || MAINDllOBJECT->getCurrentHandler()->getClicksDone() >= MAINDllOBJECT->getCurrentHandler()->getMaxClicks())
					{
						switch(s->ShapeType)
						{
						case RapidEnums::SHAPETYPE::LINE:
						case RapidEnums::SHAPETYPE::XLINE:
						case RapidEnums::SHAPETYPE::XRAY:
							switch(((Line*)s)->LineType)
							{
								case RapidEnums::LINETYPE::FINITELINE:
								case RapidEnums::LINETYPE::INFINITELINE:
								case RapidEnums::LINETYPE::RAYLINE:
								case RapidEnums::LINETYPE::LINEINTWOARC:
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER;
									MAINDllOBJECT->setHandler(new LineHandlers());
									break;

								case RapidEnums::LINETYPE::ANGLE_BISECTORL:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineAngleBisector());
									break;
								case RapidEnums::LINETYPE::ONEPT_ANGLELINE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineOnePointAngle());
									break;
								case RapidEnums::LINETYPE::ONEPT_OFFSETLINE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineOnePointOffsetDist());
									break;
								case RapidEnums::LINETYPE::PARALLEL_LINEL:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineParallel2Line());
									break;
								case RapidEnums::LINETYPE::PARALLEL_TANGENTL:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineParallelTangent());
									break;
								case RapidEnums::LINETYPE::PARALLEL_TANGENT2TWOCIRLE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_TANGENT_2CIRCLE;
									MAINDllOBJECT->setHandler(new LineParallelTangent2Circle());
									break;
								case RapidEnums::LINETYPE::PERPENDICULAR_LINEL:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LinePerpendicular2Line);
									break;
								case RapidEnums::LINETYPE::PERPENDICULAR_TANGENTL:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LinePerpendicularTangent());
									break;
								case RapidEnums::LINETYPE::TANGENT_THRU_POINT:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineTangentThruPoint());
									break;
								case RapidEnums::LINETYPE::TANGENT_ON_2_CIRCLE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineTangentOn2Circles());
									break;
								case RapidEnums::LINETYPE::TANGENT_ON_CIRCLEL:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE;
									MAINDllOBJECT->setHandler(new LineTangent2Circle());
									break;
								case RapidEnums::LINETYPE::LINEPERPENDICULAR2LINEONPLANE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINEPER2LINEONPLN_HANDLER;
									MAINDllOBJECT->setHandler(new LinePerpendicular2LineOnPlane());
									break;
							}
							break;
						case RapidEnums::SHAPETYPE::LINE3D:
						case RapidEnums::SHAPETYPE::XLINE3D:
								switch(((Line*)s)->LineType)
							{
								case RapidEnums::LINETYPE::PERPENDICULARLINE3D:
									{
										MAINDllOBJECT->Derivedmodeflag = true;
										MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE3D;
										MAINDllOBJECT->setHandler(new LinePerpendicularToLine3D());
									}
									break;
								case  RapidEnums::LINETYPE::PARALLELLINE3D:
									{
										MAINDllOBJECT->Derivedmodeflag = true;
										MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE3D;
										MAINDllOBJECT->setHandler(new LineParallelToLine3D());
									}
									break;
								default:
									{
										MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINE3D_HANDLER;
										MAINDllOBJECT->setHandler(new LineHandlers());
									}
								break;
							}
							break;

						case RapidEnums::SHAPETYPE::TWOARC:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::TWOARC_HANDLER;
							MAINDllOBJECT->setHandler(new GroupShapeHandler(s, RapidEnums::SHAPETYPE::TWOARC));
							break;

						case RapidEnums::SHAPETYPE::TWOLINE:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::TWOLINE_HANDLER;
							MAINDllOBJECT->setHandler(new GroupShapeHandler(s, RapidEnums::SHAPETYPE::TWOLINE));
							break;

						case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::INTERSECTIONSHAPEHANDLER;
							MAINDllOBJECT->setHandler(new IntersectionShapeHandler());
							break;

						case RapidEnums::SHAPETYPE::CLOUDPOINTS:
							if(((CloudPoints*)s)->CloudPointsType == RapidEnums::CLOUDPOINTSTYPE::INTERSECTIONSHAPEPROJECTION)
							{
								MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::SHAPEPROJECTIONHANDLER;
								MAINDllOBJECT->setHandler(new ProjectionHandler());
							}
							else
							{
							   MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTS_HANDLER;
							   MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							   MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							}
							break;
							

						case RapidEnums::SHAPETYPE::CIRCLE:
							switch(((Circle*)s)->CircleType)
							{
								case RapidEnums::CIRCLETYPE::TANCIRCLETO_CIRCLE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new CircleTangent2Circle());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::TANGENT_ON_CIRCLE;
									break;
								case RapidEnums::CIRCLETYPE::TANCIRCLETO_2CIRCLES_IN:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new CircleTangent2TwoCircle1());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE;
									break;
								case RapidEnums::CIRCLETYPE::TANCIRCLETO_2CIRCLES_OUT:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new CircleTangent2TwoCircle2());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE1;
									break;
								case RapidEnums::CIRCLETYPE::TANCIRCLETO_2LINES:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new CircleTangent2TwoLine());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::TANGENTIAL_CIRCLE;
									break;
								case RapidEnums::CIRCLETYPE::TANCIRCLETO_LINECIRCLE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new CircleTangent2LineCircle());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTOLINECIRCLE;
									break;
								case RapidEnums::CIRCLETYPE::TANCIRCLETO_TRIANG_IN:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new CircleInsideATraingle(true));
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_TANTOTRIANGLEIN;
									break;
								case RapidEnums::CIRCLETYPE::TANCIRCLETO_TRIANG_OUT:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new CircleInsideATraingle(false));
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_TANTOTRIANGLEOUT;
									break;
								case RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER:
								case RapidEnums::CIRCLETYPE::TWOPTCIRCLE:
								case RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE:
									MAINDllOBJECT->setHandler(new CircleHandlers());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER;
									break;
		
							}
							break;
						case RapidEnums::SHAPETYPE::CIRCLE3D:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE3D_HANDLER;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						case RapidEnums::SHAPETYPE::ARC:
							if(((Circle*)s)->CircleType == RapidEnums::CIRCLETYPE::PARALLELARCCIR)
							{
								MAINDllOBJECT->Derivedmodeflag = true;
								MAINDllOBJECT->setHandler(new ParallelArc());
								MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLELARC;
							}
							else if (((Circle*)s)->CircleType == RapidEnums::CIRCLETYPE::LINEARCLINE_CIRCLE)
							{
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new LineArcLineHandler());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINEARCLINE_HANDLER;
							}
							else
							{
								MAINDllOBJECT->setHandler(new ArcHandler());
								MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER;
							}
							break;
						case RapidEnums::SHAPETYPE::RPOINT:
							if(((RPoint*)s)->PointType == RPoint::RPOINTTYPE::POINTONLINE)
							{
								MAINDllOBJECT->setHandler(new PointOnLine());
								MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::POINTONLINE;
							}
							else if(((RPoint*)s)->PointType == RPoint::RPOINTTYPE::POINTONCIRCLE)
							{
								MAINDllOBJECT->setHandler(new PointOnCircle());
								MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::POINTONCIRCLE;
							}
							else if(((RPoint*)s)->PointType == RPoint::RPOINTTYPE::MIDPOINT)
							{
								MAINDllOBJECT->setHandler(new MidPointHandler());
								MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MIDPOINT;
							}
							else
							{
								MAINDllOBJECT->setHandler(new PointHandler());
								MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER;
							}
							break;
				
						case RapidEnums::SHAPETYPE::RPOINT3D:
							MAINDllOBJECT->setHandler(new PointHandler());
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER3D;
							break;
						case RapidEnums::SHAPETYPE::RTEXT:
							MAINDllOBJECT->setHandler(new RTextHandler());
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RTEXT_HANDLER;
							break;
						case RapidEnums::SHAPETYPE::DUMMY:
							MAINDllOBJECT->setHandler(new DMMHandler());
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DMM_HANDLER;
							break;
						case RapidEnums::SHAPETYPE::PERIMETER:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PERIMETER_HANDLER;
							MAINDllOBJECT->setHandler(new PerimeterHandler());
							break;
						case RapidEnums::SHAPETYPE::PLANE:
							switch(((Plane*)s)->PlaneType)
							{
								case RapidEnums::PLANETYPE::PERPENDICULARPLANE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new PlanePerpendicular2Plane());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE;
									break;
								case RapidEnums::PLANETYPE::PARALLELPLANE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new PlaneParallel2Plane());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARALLEL_PLANE;
									break;
								case RapidEnums::PLANETYPE::PLANEPERPENDICULAR2LINE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new PlanePerpendicular2LineHandler());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PERPENDICULARPLANE_ONLINE;
									break;
								case RapidEnums::PLANETYPE::PLANETHROUGHTWOLINE:
									MAINDllOBJECT->Derivedmodeflag = true;
									MAINDllOBJECT->setHandler(new PlaneThrough2Lines3D());
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PLANEFROMTWOLINEHANDLER;
									break;
								default:
									MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PLANE_HANDLER;
									MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
									MAINDllOBJECT->setHandler(new DummyHandler(fb));			
									break;
							}
							break;
						case RapidEnums::SHAPETYPE::SPLINE:
							MAINDllOBJECT->setHandler(new SplineHandler());
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::SPLINE_HANDLER;
							break;
						case RapidEnums::SHAPETYPE::RELATIVEPOINT:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RELATIVEPOINT_HANDLER;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						case RapidEnums::SHAPETYPE::CYLINDER:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CYLINDER_HANDLER;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						case RapidEnums::SHAPETYPE::SPHERE:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER ;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						case RapidEnums::SHAPETYPE::CONE:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CONE_HANDLER;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						case RapidEnums::SHAPETYPE::TORUS:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::TORUS_HANDLER;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						case RapidEnums::SHAPETYPE::ELLIPSE3D:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ELLIPSE3D_HANDLER;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						case RapidEnums::SHAPETYPE::PARABOLA3D:
							MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PARABOLA3D_HANDLER ;
							MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							MAINDllOBJECT->setHandler(new DummyHandler(fb));		
							break;
						}
					}
					if(IsPartProgramRunning() && FirstMousedown_Partprogram())
					{
						Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
						Vector MousePos(CurrDRO.getX(), CurrDRO.getY(), CurrDRO.getZ());
						Vector difference1(MousePos.getX() - ppFirstgrab->PointDRO.getX(), MousePos.getY() - ppFirstgrab->PointDRO.getY(), MousePos.getZ() - ppFirstgrab->PointDRO.getZ(), 0);
						if( getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
						{
							FramegrabBase* fb =	((AddPointAction*) getCurrentAction())->getFramegrab();
							Vector temp = fb->PointDRO;
							temp += difference1;
							fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
							fb->points[0].set(difference1.getX() + fb->points[0].getX(), difference1.getY() + fb->points[0].getY(), difference1.getY() + fb->points[0].getZ());
						}
					}
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH:
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
					MAINDllOBJECT->setHandler(new DefaultHandler());
					FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOCUSDEPTH_MODE;
					DoHoming(false);
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO:
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
					MAINDllOBJECT->setHandler(new DefaultHandler());
					DoHoming(false);
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX:
					MAINDllOBJECT->setHandler(new FocusDepthMultiBoxHandler(fb));
					FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE;
					FOCUSCALCOBJECT->FocusOnMode(false);
					DoHoming(false);
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX:
					MAINDllOBJECT->setHandler(new FocusDepthMultiBoxHandler(fb));
					FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE;
					FOCUSCALCOBJECT->FocusOnMode(false);
					DoHoming(false);
					break;
	
				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN:
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
					MAINDllOBJECT->setHandler(new DefaultHandler());
					FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE;
					DoHoming(false);
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG:
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
					MAINDllOBJECT->setHandler(new ContourScan_FG());
					FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::CONTOURFRAMEGRAB;
					DoHoming(false);
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN:
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
					MAINDllOBJECT->setHandler(new DefaultHandler());
					FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE;
					DoHoming(false);
					break;

				case RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT:
					MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
					MAINDllOBJECT->setHandler(new DefaultHandler());
					/*if(IsPartProgramRunning() && FirstMousedown_Partprogram())
					{
						Vector tmp = ppFirstgrab->PointDRO;
						ppFirstgrab->PointDRO.set(tmp.getX(), tmp.getY(), 0);
						Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
						Vector temp(CurrDRO.getX(), CurrDRO.getY(), 0);
						ShiftShape_ForFirstMouseDown(temp, ReferencePtAsHomePos());
					}*/
					break;
				}
			}
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
		{
			Shape *s = ((PerimeterAction*)(*ppCurrentaction).second)->getShape();
			if(s->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(s->UcsId());
			if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::PERIMETER_HANDLER)
			{
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PERIMETER_HANDLER;
				MAINDllOBJECT->setHandler(new PerimeterHandler());
			}
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
		{
			if(((CircularInterPolationAction*)(*ppCurrentaction).second)->GetShapeInfo())
			{
				Shape *s = ((CircularInterPolationAction*)(*ppCurrentaction).second)->getShape();
				if(s->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
					ChangeUCSAction_DuringProgram(s->UcsId());
				if(!s->selected())
					MAINDllOBJECT->selectShape(s->getId(), false);
			}	
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
		{
			if(((LinearInterPolationAction*)(*ppCurrentaction).second)->GetShapeInfo())
			{
				Shape *s = ((LinearInterPolationAction*)(*ppCurrentaction).second)->getShape();
				if(s->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
					ChangeUCSAction_DuringProgram(s->UcsId());
				if(!s->selected())
					MAINDllOBJECT->selectShape(s->getId(), false);
			}	
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
		{
			Shape *s = ((ProfileScanAction*)(*ppCurrentaction).second)->getShape();
			if(s->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(s->UcsId());
			if(!s->selected())
				MAINDllOBJECT->selectShape(s->getId(), false);	
			DoHoming(false);
			HELPEROBJECT->CurrentProfileCpShape = (ShapeWithList*)s;
			if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::PROFILESCANHANDLER)
			{
				MAINDllOBJECT->setHandler(new ProfileScanHandler());
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PROFILESCANHANDLER;
			}
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
		{
			Shape *s = ((AreaShapeAction*)(*ppCurrentaction).second)->getShape();
			if(s->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(s->UcsId());
			if(!s->selected())
				MAINDllOBJECT->selectShape(s->getId(), false);	
			FramegrabBase *fb = ((AreaShapeAction*)(*ppCurrentaction).second)->getFramegrab();
			switch(fb->FGtype)
			{
			case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabArc(fb));
				break;

			case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabAngularRect(fb));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabCircle(fb));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabRectangle(fb));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle(fb));
				break;
			}
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
		{
			int UCSID = ((EditCad1PtAction*)(*ppCurrentaction).second)->getUcsId();
			if(UCSID != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(UCSID);
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CAD_1PT_EDITALIGN_HANDLER;
			MAINDllOBJECT->setHandler(new EditCad1PtHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
		{
			int UCSID = ((Cad1PtAlignAction*)(ppCurrentaction->second))->getUcsId();
			if(UCSID != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(UCSID);
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT;
			MAINDllOBJECT->setHandler(new Cad1PtHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
		{
			int UCSID = ((Cad2PtAlignAction*)(ppCurrentaction->second))->getUcsId();
			if(UCSID != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(UCSID);
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2PT;
			//Cad2PtHandler c2 = new Cad2PtHandler();
			//c2.
			MAINDllOBJECT->setHandler(new Cad2PtHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
		{
			int UCSID = ((Cad1Pt1LineAction*)(ppCurrentaction->second))->getUcsId();
			if(UCSID != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(UCSID);
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE;
			MAINDllOBJECT->setHandler(new Cad1Pt1LineHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION)
		{
			int UCSID = ((Cad2LineAction*)(ppCurrentaction->second))->getUcsId();
			if(UCSID != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(UCSID);
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2LINE;
			MAINDllOBJECT->setHandler(new Cad2LineHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
		{
			if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::CAD_2PT_EDITALIGN_HANDLER)
			{
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CAD_2PT_EDITALIGN_HANDLER;
				MAINDllOBJECT->setHandler(new EditCad2PtHandler());
			}
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::AUTOALIGNACTION)
		{
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::TRANSLATE_HANDLER;
			MAINDllOBJECT->setHandler(new AutoAlignHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::DEVIATIONACTION)
		{
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEVIATION_HANDLER;
			MAINDllOBJECT->setHandler(new DeviationHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION)
		{
			int UCSID = ((DXF_DeviationAction*)(ppCurrentaction->second))->getUcsId();
			if(UCSID != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(UCSID);
			if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DXF_DEVIATION_HANDLER)
			{
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DXF_DEVIATION_HANDLER;
				MAINDllOBJECT->setHandler(new DXFDeviationHandler());
			}
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION)
		{
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CLOUDCOMPARISION_HANDLER;
			MAINDllOBJECT->setHandler(new CloudComparisionHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::MOVESHAPEACTION)
		{
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER;
			MAINDllOBJECT->setHandler(new MoveShapeHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION)
		{
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::EDITCAD_2LINEHANDLER;
			MAINDllOBJECT->setHandler(new EditCad2LineHandler());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION)
		{
			Shape *s = ((AddShapePointsAction*)(*ppCurrentaction).second)->getShape();
			if(s->UcsId() != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(s->UcsId());
			if(!s->selected())
				MAINDllOBJECT->selectShape(s->getId(), false);
			MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::POINTFROMSHAPEHANDLER;
			MAINDllOBJECT->setHandler(new PointFromShapeHandler(true));
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
		{
			int UCSID = ((EditCad1Pt1LineAction*)(*ppCurrentaction).second)->getUcsId();
			if(UCSID != MAINDllOBJECT->getCurrentUCS().getId())
				ChangeUCSAction_DuringProgram(UCSID);
			if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::EDITCAD_1PT1LINE_HANDLER)
			{
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::EDITCAD_1PT1LINE_HANDLER;
				MAINDllOBJECT->setHandler(new EditCad1Pt1LineHandler());
			}
		}
		MAINDllOBJECT->updatePPGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0031", __FILE__, __FUNCSIG__); } 
}

//update thread handler for current step..
void PartProgramFunctions::drawThreadRunpp()
{
	try
	{
		Shape *s = ((AddThreadPointAction*)getCurrentThreadAction())->getShape();
		((AddThreadMeasureAction*)getCurrentAction())->SelecttheCurrentPointer(s->getId());
		FramegrabBase *fb = getThreadFrameGrab();
		switch(fb->FGtype)
		{
			case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabArc(fb));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabAngularRect(fb));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabRectangle(fb));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				DoHoming(false);
				MAINDllOBJECT->setHandler(new FrameGrabFixedRectangle(fb));
				break;
			case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
				MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR;
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				MAINDllOBJECT->setHandler(new FlexibleCrossHairHandler(fb));
				break;
		}
		THREADCALCOBJECT->ThreadShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0032", __FILE__, __FUNCSIG__); } 
}

RAction* PartProgramFunctions::GetActionfor_ActionId(int ActionId)
{
	try
	{
		RC_ITER PPAction;
		bool ActionSynchronised = false;
		for(PPAction = getPPArrangedActionlist().getList().begin(); PPAction != getPPArrangedActionlist().getList().end(); PPAction++)
		{				
			if(PPAction->second->getId() == ActionId)
			{
				ActionSynchronised = true;
				break;
			}
		}
		if(ActionSynchronised)
			return static_cast<RAction*>((*PPAction).second);
		else
			return NULL;
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0032a", __FILE__, __FUNCSIG__); return NULL;}
}

void PartProgramFunctions::IncrementToCurrentAction()
{
	try
	{
		TmpPPActionPtr = ppCurrentaction;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0033a", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::IncrementStepForProbe()
{
	try
	{
		if(getCurrentAction() == NULL) return;
		this->ppCurrentaction++;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0033", __FILE__, __FUNCSIG__); }
}

//Increment action...//
void PartProgramFunctions::IncrementAction(bool insideFramework /*= false*/ )
{
	try
	{
		for each(int Idlist in LastActionIdList_OffsetAction)
		{
			if(getCurrentAction()->getId() == Idlist)
			{
				double OffsetValue[4] = {0};						
				int ShapeID = ((AddPointAction*)getCurrentAction())->getShape()->getId();
				for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
				{
					UCS* Cucs = (UCS*)(*Item_ucs).second;
					{
						for(RC_ITER Shpitem = Cucs->getShapes().getList().begin(); Shpitem != Cucs->getShapes().getList().end(); Shpitem++)
						{
							Shape* CShape = (Shape*)((*Shpitem).second);
							if(CShape->getId() == ShapeID)
							{
								for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
								{
									Shape* Cshp = (Shape*)(*Item_shape).second;
									if(Cshp->getId() == ShapeID)
									{
										if(Cshp->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || Cshp->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
										{
											if(CShape->XOffsetCorrection())
												OffsetValue[0] = ((Circle*)CShape)->getCenter()->getX() - ((Circle*)Cshp)->getCenter()->getX();
											if(CShape->YOffsetCorrection())
												OffsetValue[1] = ((Circle*)CShape)->getCenter()->getY() - ((Circle*)Cshp)->getCenter()->getY();
											if(CShape->ZOffsetCorrection())
												OffsetValue[2] = ((Circle*)CShape)->getCenter()->getZ() - ((Circle*)Cshp)->getCenter()->getZ();
										}
										else if(Cshp->ShapeType == RapidEnums::SHAPETYPE::RPOINT || Cshp->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
										{
											if(CShape->XOffsetCorrection())
												OffsetValue[0] = ((RPoint*)CShape)->getPosition()->getX() - ((RPoint*)Cshp)->getPosition()->getX();
											if(CShape->YOffsetCorrection())
												OffsetValue[1] = ((RPoint*)CShape)->getPosition()->getY() - ((RPoint*)Cshp)->getPosition()->getY();
											if(CShape->ZOffsetCorrection())
												OffsetValue[2] = ((RPoint*)CShape)->getPosition()->getZ() - ((RPoint*)Cshp)->getPosition()->getZ();
											if(CShape->ROffsetCorrection())
												OffsetValue[3] = ((RPoint*)CShape)->getPosition()->getR() - ((RPoint*)Cshp)->getPosition()->getR();
										}
										else if(Cshp->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
										{
											if(CShape->XOffsetCorrection())
												OffsetValue[0] = ((Sphere*)CShape)->getCenter()->getX() - ((Sphere*)Cshp)->getCenter()->getX();
											if(CShape->YOffsetCorrection())
												OffsetValue[1] = ((Sphere*)CShape)->getCenter()->getY() - ((Sphere*)Cshp)->getCenter()->getY();
											if(CShape->ZOffsetCorrection())
												OffsetValue[2] = ((Sphere*)CShape)->getCenter()->getZ() - ((Sphere*)Cshp)->getCenter()->getZ();
										}
									}
								}
								if(CShape->getChild().size() > 0)
								{
									for(list<BaseItem*>::iterator ptr = CShape->getChild().begin(); ptr != CShape->getChild().end(); ptr++)
									{
										Shape* ChildShape = (Shape*)*ptr;
										for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
										{
											Shape* Cshp = (Shape*)(*Item_shape).second;
											if(Cshp->getId() == ChildShape->getId())
											{
												if(Cshp->ShapeType == RapidEnums::SHAPETYPE::RPOINT || Cshp->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
												{
													if(ChildShape->XOffsetCorrection())
														OffsetValue[0] += ((RPoint*)ChildShape)->getPosition()->getX() - ((RPoint*)Cshp)->getPosition()->getX();
													if(ChildShape->YOffsetCorrection())
														OffsetValue[1] += ((RPoint*)ChildShape)->getPosition()->getY() - ((RPoint*)Cshp)->getPosition()->getY();
													if(ChildShape->ZOffsetCorrection())
														OffsetValue[2] += ((RPoint*)ChildShape)->getPosition()->getZ() - ((RPoint*)Cshp)->getPosition()->getZ();
													if(CShape->ROffsetCorrection())
														OffsetValue[3] += ((RPoint*)ChildShape)->getPosition()->getR() - ((RPoint*)Cshp)->getPosition()->getR();
												}
											}
										}
									}
								}
							}
						}
					}
				}
				Vector Shift;
				Shift.set(OffsetValue[0], OffsetValue[1], OffsetValue[2], OffsetValue[3]);
				ShiftShape_ForFirstProbePoint(Shift, true);
			}
		}

		if(ActionIdListForNogoMeasurement.size() > 0)
		{
			for(std::map<int, int>::iterator itr = ActionIdListForNogoMeasurement.begin(); itr != ActionIdListForNogoMeasurement.end(); itr++)
			{
				int Id = (*itr).second;
				if(getCurrentAction()->getId() == Id)
				{
					if(checkToleranceLimitForNogoMeasurement((*itr).first))
					{
						//Show Message box that measurement is out of tolerance
						MAINDllOBJECT->ShowMsgBoxString("PPFUNC_IncrementAction01",RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
						return;			
					}
					break;
				}
			}
		}

		if(ActionIdListForCriticalShape.size() > 0)
		{
			for(std::map<int, int>::iterator itr = ActionIdListForCriticalShape.begin(); itr != ActionIdListForCriticalShape.end(); itr++)
			{
				int Id = (*itr).second;
				if(getCurrentAction()->getId() == Id)
				{
					if(checkToleranceLimitForCriticalShape((*itr).first))
					{
						//Show Message box that Shape is out of tolerance
						MAINDllOBJECT->ShowMsgBoxString("PPFUNC_IncrementAction02",RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
						return;			
					}
					break;
				}
			}
		}

		this->ProgramAutoIncrementMode(false);
		Vector Cpoint, Cpoint1, CpointArcM;
		FramegrabBase *fb = NULL, *fb1 = NULL;
		Shape* CPPSelectedShape = NULL;
		if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			CPPSelectedShape = ((AddPointAction*)getCurrentAction())->getShape();
			fb = ((AddPointAction*)getCurrentAction())->getFramegrab();
			ActionsPointCollection* CTestColl = PPActionPositionColl[getCurrentAction()->getId()];
			Cpoint.set(CTestColl->AP_Points[0]);
			if(CPPSelectedShape->ShapeType == RapidEnums::SHAPETYPE::ARC && ((Circle*)CPPSelectedShape)->CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE)
			{
				if(MAINDllOBJECT->getCurrentHandler()->getClicksDone() == 2)
				{
					ActionsPointCollection* CTestColl2 = PPActionPositionColl[getCurrentAction()->getId()];
					checkArcClick.set(CTestColl2->AP_Points[0]);
					PreviousClick.set(fb->points[0]);
				}
			}
			if(fb->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH)
			{
				if(fb->noofpts == 2)
				{
					Cpoint.set(CTestColl->AP_PointDRO);
				}
			}
			//else 
		}
		else
		{
			if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ROTATIONACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::REFLECTIONACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::COPYSHAPEACTION
				|| getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION
				|| getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::AUTOALIGNACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::DEVIATIONACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::TRANSLATIONACTION
				|| getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::MOVESHAPEACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION
				 || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION
				 || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION
				|| getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION)
				this->ppCurrentaction++;
			else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION
				|| getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION || getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				this->ppCurrentaction++;
				if(getCurrentAction() == NULL) return;	
				startppPoint = endppPoint;
				switch(getCurrentAction()->CurrentActionType)
				{			
					case RapidEnums::ACTIONTYPE::PERIMETERACTION:
						{
							FramegrabBase* Cfg = ((PerimeterAction*)(*ppCurrentaction).second)->getFramegrab();
							if(Cfg->CurrentWindowNo == 0)
							{
								if(Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
									endppPoint = Cfg->points[0];
								else
									endppPoint = Cfg->PointDRO;
							}
						}
						break;
					case RapidEnums::ACTIONTYPE::ADDPOINTACTION:
						{
							FramegrabBase* Cfg = ((AddPointAction*)(*ppCurrentaction).second)->getFramegrab();
							if(Cfg->CurrentWindowNo == 0)
							{
								if(Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
									endppPoint = Cfg->points[0];
								else
									endppPoint = Cfg->PointDRO;
							}
						}
						break;
					case RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION:
						endppPoint = ((AddProbePathAction*)(*ppCurrentaction).second)->getFramegrab()->PointDRO;
						break;
					case RapidEnums::ACTIONTYPE::IMAGE_ACTION:
						endppPoint = ((AddImageAction*)(*ppCurrentaction).second)->getFramegrab()->PointDRO;
						break;
					case RapidEnums::ACTIONTYPE::AREA_ACTION:
						endppPoint = ((AreaShapeAction*)(*ppCurrentaction).second)->getFramegrab()->PointDRO;
						break;
				}
			}
			else
			{
				this->ppCurrentaction++;
				if(getCurrentAction() == NULL) return;
				partProgramReached();
			}
			return;
		}

		if(this->ProgramMadeUsingTwoStepHoming() && !this->FinishedManualTwoStepAlignment())
		{
			bool StepsLeft = false;
			for(RC_ITER pCaction = getPPArrangedActionlist().getList().begin(); pCaction != getPPArrangedActionlist().getList().end(); pCaction++)
			{
				if(((RAction*)(*pCaction).second)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					FramegrabBase* Cfgb = ((AddPointAction*)(*pCaction).second)->getFramegrab();
					if(Cfgb->ActionForTwoStepHoming())
					{
						if(!Cfgb->ActionFinished())
						{
							StepsLeft = true;
							ppCurrentaction = pCaction;
							break;
						}
					}
				}
			}
			if(!StepsLeft)
			{
				//Do the two step homing.. Allign all the shapes,,Actions and start the program..
				TwoStepHoming_Alignment();
				this->FinishedManualTwoStepAlignment(true);
				this->ppCurrentaction = getPPArrangedActionlist().getList().begin();
				FinishedTwoStepHomingMode();
				return;
			}
		}
		else if(FirstProbePointActionFlag && !ActionTobeTransformed)
		{
			FirstProbePointActionFlag = false;
			ActionTobeTransformed = true;
			FramegrabBase* Cfgb = ((AddPointAction*)getCurrentAction())->getFramegrab();
			Vector Shift;
			Shift.set(ProbeTouchPointDro[0] - Cfgb->OriginalDRO.getX(), ProbeTouchPointDro[1] - Cfgb->OriginalDRO.getY(), ProbeTouchPointDro[2] - Cfgb->OriginalDRO.getZ());
			ShiftShape_ForFirstProbePoint(Shift);
			this->ppCurrentaction++;
			FinishedFirstPointActionForProbe();
			return;
		}
		else
			this->ppCurrentaction++;
		if(getCurrentAction() == NULL) return;	
		switch(getCurrentAction()->CurrentActionType)
		{
			case RapidEnums::ACTIONTYPE::ADDPOINTACTION:
				fb1 = ((AddPointAction*)getCurrentAction())->getFramegrab();
				break;
			case RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION:
				((AddThreadMeasureAction*)getCurrentAction())->ThreadppCurrentaction = ((AddThreadMeasureAction*)getCurrentAction())->ActionCollection.getList().begin();
				THREADCALCOBJECT->CurrentSelectedThreadAction = (AddThreadMeasureAction*)getCurrentAction();
				THREADCALCOBJECT->ThreadMeasurementMode(true);
				return;
				break;
		}
		if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			if(CPPSelectedShape != NULL && fb1->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE && !CPPSelectedShape->IgnoreManualCrosshairSteps())
			{
				ActionsPointCollection* CTestColl1 = PPActionPositionColl[getCurrentAction()->getId()];
				Cpoint1.set(CTestColl1->AP_Points[0]);
				if(CPPSelectedShape->ShapeType == RapidEnums::SHAPETYPE::ARC && ((Circle*)CPPSelectedShape)->CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE )
				{
					if(CPPSelectedShape->getId() !=  ((AddPointAction*)getCurrentAction())->getShape()->getId() && checkArcClick == Cpoint1)
						this->ProgramAutoIncrementMode(true);
				}
				else if(CPPSelectedShape->ShapeType == RapidEnums::SHAPETYPE::ARC || CPPSelectedShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					if(CPPSelectedShape->getId() !=  ((AddPointAction*)getCurrentAction())->getShape()->getId() && Cpoint == Cpoint1)
						this->ProgramAutoIncrementMode(true);
				}
			}
		}
		
		if(getCurrentAction() == NULL) return;
		//Autoincrment for fasttrace...//
		if(ProgramAutoIncrementMode() && getFrameGrab()->CurrentWindowNo != 1)
		{
			drawRunpp();
			MAINDllOBJECT->getCurrentHandler()->PartProgramData();
			if(getCurrentAction() == NULL) return;
			switch(CPPSelectedShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::ARC:
					if(((Circle*)CPPSelectedShape)->CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE)
						MAINDllOBJECT->getCurrentHandler()->LmouseDown_x(PreviousClick.getX(), PreviousClick.getY());
					else
						MAINDllOBJECT->getCurrentHandler()->LmouseDown_x(fb->points[0].getX(), fb->points[0].getY());
					break;
				case RapidEnums::SHAPETYPE::LINE:
					MAINDllOBJECT->getCurrentHandler()->LmouseDown_x(fb->points[0].getX(), fb->points[0].getY());
					break;
			}
			IncrementAction(true); 
		}
		if(getCurrentAction() != NULL && !insideFramework)
		{
			startppPoint = endppPoint;
			switch(getCurrentAction()->CurrentActionType)
			{
				case RapidEnums::ACTIONTYPE::PERIMETERACTION:
					{
						FramegrabBase* Cfg = ((PerimeterAction*)(*ppCurrentaction).second)->getFramegrab();
						if(Cfg->CurrentWindowNo == 0)
						{
							if(Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
								endppPoint = Cfg->points[0];
							else
								endppPoint = Cfg->PointDRO;
						}
					}
					break;
				case RapidEnums::ACTIONTYPE::ADDPOINTACTION:
					{
						FramegrabBase* Cfg = ((AddPointAction*)(*ppCurrentaction).second)->getFramegrab();
						if(Cfg->CurrentWindowNo == 0)
						{
							if(Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE || Cfg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
								endppPoint = Cfg->points[0];
							else
								endppPoint = Cfg->PointDRO;
						}
					}
					break;
				case RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION:
					endppPoint = ((AddProbePathAction*)(*ppCurrentaction).second)->getFramegrab()->PointDRO;
					break;
				case RapidEnums::ACTIONTYPE::IMAGE_ACTION:
					endppPoint = ((AddImageAction*)(*ppCurrentaction).second)->getFramegrab()->PointDRO;
					break;
				case RapidEnums::ACTIONTYPE::AREA_ACTION:
					endppPoint = ((AreaShapeAction*)(*ppCurrentaction).second)->getFramegrab()->PointDRO;
					break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0033", __FILE__, __FUNCSIG__); }
}

//Handle UCS changeaction during program run..
void PartProgramFunctions::ChangeUCSAction_DuringProgram(int id )
{
	try
	{
		int CUcsId = id;
		MAINDllOBJECT->changeUCS(CUcsId);
		MAINDllOBJECT->PartProgramShape_updated();
		//partProgramReached();
	}
	catch(...)
	{ 
		MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->AddingUCSModeFlag =  false; 
		MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0034a", __FILE__, __FUNCSIG__); 
	}
}

//Increment thread program action
void PartProgramFunctions::IncrementActionThreadAction()
{
	try
	{
		this->FirstMousedown_Partprogram(false);
		this->ProgramAutoIncrementMode(false);
		((AddThreadMeasureAction*)getCurrentAction())->ThreadppCurrentaction++;
		if(((AddThreadMeasureAction*)getCurrentAction())->getCurrentThAction() != NULL)
		{	
			AddThreadPointAction* Caction = (AddThreadPointAction*)((AddThreadMeasureAction*)getCurrentAction())->getCurrentThAction();
			((AddThreadMeasureAction*)getCurrentAction())->CurrentSelectedThreadShape = Caction->getShape();
			startppPoint = endppPoint;
			//endppPoint = Caction->getFramegrab()->points[0];
			endppPoint = Caction->getFramegrab()->PointDRO;
		}
		else
		{
			THREADCALCOBJECT->CalculateThreadParameters();
			THREADCALCOBJECT->ThreadMeasurementMode(false);
			THREADCALCOBJECT->ThreadMeasurementModeStarted(false);
			//CurrentSelectedThreadAction = NULL;
			IncrementAction();
		}
	}
	catch(...)
	{
		MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->AddingUCSModeFlag =  false; 
		MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0035", __FILE__, __FUNCSIG__); 
	} 
}

//Decrement action.....//
void PartProgramFunctions::DecrementAction()
{
	try
	{
		if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			((AddThreadMeasureAction*)getCurrentAction())->ThreadppCurrentaction--;
		else
			this->ppCurrentaction--;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0036", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::ClearAfterProgramFinish()
{
	try
	{
		FOCUSCALCOBJECT->FocusOnStatus(false);
		MAINDllOBJECT->Measurement_updated();
		MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		this->IsPartProgramRunning(false);
		this->DrawPartprogramArrow(false);
		this->FirstMousedown_Partprogram(false);
		this->FirstMousedown_Partprogram_Manual(false);
		this->StartingPointofPartprogram(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0036a", __FILE__, __FUNCSIG__); }
}

//Partprogram action list...//
RCollectionBase& PartProgramFunctions::getPPActionlist()
{
	return PPActionlist;
}

//Exposting appropriate PP Action list according to shortest path flag
RCollectionBase& PartProgramFunctions::getPPArrangedActionlist()
{
	if(DoShortestPath())
		return PPShortestPathActionlist;
	else
		return PPActionlist;
}

void PartProgramFunctions::IncrementActionForBatchOfCommand(bool increment)
{
	try
	{
		if(increment)
			TmpPPActionPtr++;
		else
			TmpPPActionPtr--;
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0036b", __FILE__, __FUNCSIG__);} 
}

void PartProgramFunctions::IncrementActionForSynchronisation()
{
	try
	{
		ppCurrentaction++;
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0036c", __FILE__, __FUNCSIG__);} 
}

//Get the current action....//
RAction* PartProgramFunctions::getCurrentAction(bool tempPtr)
{
	try
	{
		if(tempPtr)
		{
			if(TmpPPActionPtr == getPPArrangedActionlist().getList().end())
				return NULL;
			else 
				return static_cast<RAction*>((*TmpPPActionPtr).second);
		}
		else
		{
			if(ppCurrentaction == getPPArrangedActionlist().getList().end())
				return NULL;
			else 
				return static_cast<RAction*>((*ppCurrentaction).second);
		}
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0037", __FILE__, __FUNCSIG__); return NULL; } 
}

//get thread measure action..
RAction* PartProgramFunctions::getCurrentThreadAction()
{
	return ((AddThreadMeasureAction*)getCurrentAction())->getCurrentThAction();
}

//Get the current Framegrab..//
FramegrabBase* PartProgramFunctions::getFrameGrab(bool tempPtrFlag)
{
	try
	{		
		if(getCurrentAction(tempPtrFlag) == NULL) return NULL;
		if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			return getFrameGrab_Edit1PtAlign(tempPtrFlag);
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			return getFrameGrab_Edit2PtAlign(tempPtrFlag);
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			return ((EditCad1Pt1LineAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			return ((AddPointAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			return ((PerimeterAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			return ((AddProbePathAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			return ((AddImageAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			return ((AreaShapeAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
		else if(getCurrentAction(tempPtrFlag)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			return getThreadFrameGrab();
		else
			return NULL;	
	}
	catch(...){MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0037a", __FILE__, __FUNCSIG__); return NULL; } 
}

//Get the current Framegrab..//
FramegrabBase* PartProgramFunctions::getFrameGrab_Edit1PtAlign(bool tempPtrFlag)
{
	return ((EditCad1PtAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
}

//Get the current Framegrab..//
FramegrabBase* PartProgramFunctions::getFrameGrab_Edit2PtAlign(bool tempPtrFlag)
{
	return ((EditCad2PtAction*)getCurrentAction(tempPtrFlag))->getFramegrab();
}

//current thread framegrab
FramegrabBase* PartProgramFunctions::getThreadFrameGrab()
{
	AddThreadPointAction* Caction = (AddThreadPointAction*)((AddThreadMeasureAction*)getCurrentAction())->getCurrentThAction();
	return Caction->getFramegrab();
}

//current shape..
Shape* PartProgramFunctions::getCurrentPPShape()
{
	return ((AddPointAction*)getCurrentAction())->getShape();
}

//current action type..
RapidEnums::ACTIONTYPE PartProgramFunctions::getCActionType(bool tempPtr)
{
	return getCurrentAction(tempPtr)->CurrentActionType;
}

//Get the current light property...//
char* PartProgramFunctions::getCurrentActionLightProperty()
{
	try
	{
		if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			MAINDllOBJECT->SetCamProperty(((AddPointAction*)getCurrentAction())->getFramegrab()->getCamProperties());
			return (((AddPointAction*)getCurrentAction())->getFramegrab()->getLightIntensity());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
		{
			MAINDllOBJECT->SetCamProperty(((AddImageAction*)getCurrentAction())->getFramegrab()->getCamProperties());
			return (((AddImageAction*)getCurrentAction())->getFramegrab()->getLightIntensity());
		}
		else if(getCurrentAction()->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
		{
			MAINDllOBJECT->SetCamProperty(((AreaShapeAction*)getCurrentAction())->getFramegrab()->getCamProperties());
			return (((AreaShapeAction*)getCurrentAction())->getFramegrab()->getLightIntensity());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0038", __FILE__, __FUNCSIG__); }
}

//current thread action light property..
char* PartProgramFunctions::getCurrentThreadActionLightProperty()
{
	try
	{
		MAINDllOBJECT->SetCamProperty(((AddThreadPointAction*)getCurrentThreadAction())->getFramegrab()->getCamProperties());
		return (((AddThreadPointAction*)getCurrentThreadAction())->getFramegrab()->getLightIntensity());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0039", __FILE__, __FUNCSIG__); }
}


//Set the program steps... Same a program monitor....//
void PartProgramFunctions::SetProgramSteps()
{
	try
	{
		FramegrabBase* fb;
		Shape* s1;
		int i = 0;
		shapename.clear();
		crosshairName.clear();
		int tempshapeID = 0;
		Vector tempVector;
		for( ppCurrentaction = getPPActionlist().getList().begin(); ppCurrentaction != getPPActionlist().getList().end(); ppCurrentaction++)
		{
			RAction* action = (RAction*)(*ppCurrentaction).second;
			if(!action->ActionStatus()) continue;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				if(tempshapeID != ((AddPointAction*)action)->getShape()->getId() && 
					((AddPointAction*)action)->getShape()->ShapeType == RapidEnums::SHAPETYPE::ARC && 
						((Circle*)((AddPointAction*)action)->getShape())->CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE)
				{
					RC_ITER getP = ppCurrentaction;
					getP++;
					tempVector = checkArcClick;
					checkArcClick = ((AddPointAction*)(*getP).second)->getFramegrab()->points[0];
					tempshapeID = ((AddPointAction*)(*getP).second)->getShape()->getId();
				}
				if(i > 0)
				{
					FramegrabBase* fb1 = ((AddPointAction*)action)->getFramegrab();
					if(fb1->FGtype != RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH)
					{
						if(s1->ShapeType == RapidEnums::SHAPETYPE::ARC && ((Circle*)s1)->CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE )
						{
							if(fb1->SurfaceON && s1->getId() !=  ((AddPointAction*)action)->getShape()->getId() && (checkArcClick == fb1->points[0] || tempVector == fb1->points[0]))
							{
								s1 = ((AddPointAction*)action)->getShape();
								fb = ((AddPointAction*)action)->getFramegrab();
								continue;
							}
						}
						else
						{
							if(fb1->SurfaceON && s1->getId() !=  ((AddPointAction*)action)->getShape()->getId() && fb->points[0] == fb1->points[0])
							{
								s1 = ((AddPointAction*)action)->getShape();
								fb = ((AddPointAction*)action)->getFramegrab();
								continue;
							}
						}
					}
				}
				s1 = ((AddPointAction*)action)->getShape();
				fb = ((AddPointAction*)action)->getFramegrab();
				shapename[i] = s1->getModifiedName();
				switch(fb->FGtype)
				{
				case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
					crosshairName[i] = "AR_Framegrab";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
					crosshairName[i] = "A_Framegrab";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
					crosshairName[i] = "C_Framegrab";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
					crosshairName[i] = "Flexible_CH";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH:
					crosshairName[i] = "Focus Depth";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE:
					crosshairName[i] = "Crosshair";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT:
					crosshairName[i] = "Probe_Point";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
					crosshairName[i] = "R_Framegrab";
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::SCANCROSSHAIR:
					crosshairName[i] = "Scan_CH";
				case RapidEnums::RAPIDFGACTIONTYPE::RCADPOINTS_FG:
					crosshairName[i] = "Rcad_FG";
					break;
				}
				i++;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0041", __FILE__, __FUNCSIG__); } 
}

//Shape names for PP Monitor...//
map<int, std::string> PartProgramFunctions::getShapeName()
{
   return shapename;
}

//Cross hair type for PP monitor...//
map<int, std::string> PartProgramFunctions::getCrosshairType()
{
   return crosshairName;
}



//Set one shot image for partprogram
bool PartProgramFunctions::SetRefImage_OneShot(BYTE* soruce, int length, int bpp)
{
	try
	{
		if (bpp == 1)
		{
			OneShotpixelarray = (BYTE*)malloc(sizeof(BYTE) *  length);
			memcpy(OneShotpixelarray, soruce, length);
		}
		else
		{
			int actualLength = length / bpp;
			OneShotpixelarray = (BYTE*)malloc(sizeof(BYTE) *  (length));// +10 * bpp));
			int indexer = 0;
			for (int i = 1; i < length; i += bpp)//+)
				OneShotpixelarray[indexer++] = soruce[i]; // bpp * i + 1];bpp * i + 1];
		}
		bool flag = BESTFITOBJECT->setOneShotImage(OneShotpixelarray, MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio, MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio,
												true, OneShotImgBoundryWidth(), OneShotImgBinariseVal(), 1, false, false, !MAINDllOBJECT->getSurface()); //ScaleFactor()
		free(OneShotpixelarray); 
		OneShotpixelarray = NULL;
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0042", __FILE__, __FUNCSIG__); return false; }  
}

//Set the Object Image and calculate the CG.. According to that continue or exit.
bool PartProgramFunctions::SetObjectImage_OneShot()
{
	try
	{
		int length = MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio * MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio;
		OneShotpixelarray = (BYTE*)malloc(sizeof(BYTE) * length * 3);
		bool dzm = MAINDllOBJECT->DigitalZoomMode();
		GetImageNow_byte_1bpp(1, OneShotpixelarray, !dzm);
		bool flag = BESTFITOBJECT->setOneShotImage(OneShotpixelarray, MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio, MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio, 
												true, OneShotImgBoundryWidth(), OneShotImgBinariseVal(), 1, false, true, !MAINDllOBJECT->getSurface());// ScaleFactor());
		//wchar_t ab[100];
		//int j = swprintf_s(ab, L"Ans :  %f\n", length);
		//MessageBox(NULL, ab, TEXT("RBF"), NULL);
		free(OneShotpixelarray);
		OneShotpixelarray = NULL;
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0043", __FILE__, __FUNCSIG__); return false; }  
}

//Redefine image about CG..
bool PartProgramFunctions::RedefineOneShot_AboutCG(bool DoForBothImages, int* Params)
{
	int flag = 0;
	try
	{
		//int Sht[6];
		flag = BESTFITOBJECT->RedefineImagesAboutCG(DoForBothImages, Params, !MAINDllOBJECT->getSurface(), OneShotTolPer());
		ImageRefShift.set(Params[0] / HELPEROBJECT->CamSizeRatio, Params[1] / HELPEROBJECT->CamSizeRatio);
		ImageObjShift.set(Params[2] / HELPEROBJECT->CamSizeRatio, Params[3] / HELPEROBJECT->CamSizeRatio);
		if (flag == 0) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0045_" + flag, __FILE__, __FUNCSIG__); return false; }  
}



//Rotate the image.. If there is a proper match then continue... else exit..
bool PartProgramFunctions::CalculateImage_RotateAngle()
{
	try
	{
		int flag = BESTFITOBJECT->getMaxImageMatch(&ImageRAngle, !MAINDllOBJECT->getSurface(), true, OneShotTargetTolPer()); // 
		if (flag == 0) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0046", __FILE__, __FUNCSIG__); return false; }  
}

bool PartProgramFunctions::Calculate_BestMatch() //Calculate best match..
{
	try
	{
		int Sht[4] = {0};
		int flag = BESTFITOBJECT->getBestImageMatch(&Sht[0], &ImageRAngle, false);
		ImageRefShift.set(Sht[0] / HELPEROBJECT->CamSizeRatio, Sht[1] / HELPEROBJECT->CamSizeRatio);
		ImageObjShift.set(Sht[2] / HELPEROBJECT->CamSizeRatio, Sht[3] / HELPEROBJECT->CamSizeRatio);
		if (flag == 0) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0046", __FILE__, __FUNCSIG__); return false; }  
}

//Rotate graphics after one shot angle calculation
void PartProgramFunctions::RotateFgGraphics_OneShot()
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
		double TransformM[9], Slope_Mod = 0;
		double Diff[2] = {(ImageObjShift.getX() - ImageRefShift.getX()), (ImageObjShift.getY() - ImageRefShift.getY())};
		Vector Temp(Diff[0] * upp, -Diff[1] * upp);
		Vector Temp1(Diff[0], Diff[1]);

		//Shift Shapes and Framegrab..
		ShiftShape_ForOneShot(Temp, Temp1);

		//Rotation angle calculation..
		if(ImageRAngle < 0) ImageRAngle += 2 * M_PI;
		Slope_Mod = 2 * M_PI - ImageRAngle;
		double Point_Temp[2] = {cx + ImageObjShift.getX() * upp, cy - ImageObjShift.getY() * upp};
		//Transformation Matrix..
		RMATH2DOBJECT->TransformM_RotateAboutPoint(&Point_Temp[0], Slope_Mod, &TransformM[0]);

		double Shift_X = -Point_Temp[0], Shift_Y = -Point_Temp[1];
		TransformM[2] = 0; TransformM[5] = 0;
		Vector ShftToOrigin_std(Shift_X, Shift_Y);
		Vector ShftToOrigin_pix(Shift_X / upp, -Shift_Y / upp);
		Vector ShftToOrigin_std_rev(-Shift_X, -Shift_Y);
		Vector ShftToOrigin_pix_rev(-Shift_X / upp, Shift_Y / upp);

		//Shift to origin
		ShiftShape_ForOneShot(ShftToOrigin_std, ShftToOrigin_pix);
		//Rotate Shapes, Framegrabs and all other actions..
		TrnasformProgram_TwoStepHoming(&TransformM[0]);
		//Shift back to its place
		ShiftShape_ForOneShot(ShftToOrigin_std_rev, ShftToOrigin_pix_rev);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0047", __FILE__, __FUNCSIG__); }   
}

void PartProgramFunctions::RotateFgGraphics_AfterImageMatch()
{
	try
	{ 
		Vector CurrDRO = MAINDllOBJECT->getCurrentDRO();
		Vector temp(CurrDRO.getX(), CurrDRO.getY(), CurrDRO.getZ());
		ShiftShape_ForFirstMouseDown(temp, false);

		double cx = MAINDllOBJECT->getWindow(0).getCam().x * HELPEROBJECT->CamSizeRatio, cy = MAINDllOBJECT->getWindow(0).getCam().y * HELPEROBJECT->CamSizeRatio, upp = MAINDllOBJECT->getWindow(0).getUppX();
		double TransformM[9], Slope_Mod = 0;
		double Diff[2] = {(ImageObjShift.getX() - ImageRefShift.getX()), (ImageObjShift.getY() - ImageRefShift.getY())};
		Vector Temp(Diff[0] * upp, -Diff[1] * upp);
		Vector Temp1(Diff[0], Diff[1]);

		ShiftShape_ForFirstProbePoint(Temp, false);

		if(ImageRAngle < 0) ImageRAngle += 2 * M_PI;
		Slope_Mod = 2 * M_PI - ImageRAngle;
		double Point_Temp[2] = {cx + ImageObjShift.getX() * upp, cy - ImageObjShift.getY() * upp};
			
		//Transformation Matrix..
		RMATH2DOBJECT->TransformM_RotateAboutPoint(&Point_Temp[0], Slope_Mod, &TransformM[0]);

		double Shift_X = -Point_Temp[0], Shift_Y = -Point_Temp[1];
		TransformM[2] = 0; TransformM[5] = 0;
		Vector ShftToOrigin_std(Shift_X, Shift_Y);
		Vector ShftToOrigin_std_rev(-Shift_X, -Shift_Y);
		ShiftShape_ForFirstProbePoint(ShftToOrigin_std);
		TrnasformProgram_TwoStepHoming(&TransformM[0]);
		ShiftShape_ForFirstProbePoint(ShftToOrigin_std_rev);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0047", __FILE__, __FUNCSIG__); }   
}

//Shift the graphics after one shot image match
void PartProgramFunctions::ShiftShape_ForOneShot(Vector MousePos, Vector PixeShift)
{
	try
	{
		this->FirstMousedown_Partprogram_Manual(false);
		this->FirstMousedown_Partprogram(false);
		Vector difference(MousePos);
		Vector difference1(PixeShift);

		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->Normalshape())
				{
					if(!Cshape->ShapeAsReferenceShape())
						Cshape->Translate(difference);
					else
					{
						if(ProgramRunningForFirstTime())
							Cshape->Translate(difference);
					}
				}
			}
			for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->Normalshape())
					Cshape->Translate(difference);
			}
		}
		
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
			{
				bool Tempflag = false;
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				for each(BaseItem* Bitem in Cdim->getParents())
				{
					Shape* Csh = (Shape*)Bitem;
					if(Csh->ShapeAsReferenceShape())
					{
						Tempflag = true;
						break;
					}
				}
				if(!Tempflag)
					Cdim->TranslatePosition(difference);
				else
				{
					if(ProgramRunningForFirstTime())
						Cdim->TranslatePosition(difference);
				}
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->TranslatePosition(difference);
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
		}
		for(RC_ITER pp = getPPActionlist().getList().begin(); pp != getPPActionlist().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				//Vector temp = fb->PointDRO;
				//temp += difference1;
				//fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
				fb->myPosition[0].set(difference1.getX() + fb->myPosition[0].getX(), difference1.getY() + fb->myPosition[0].getY());
				fb->myPosition[1].set(difference1.getX() + fb->myPosition[1].getX(), difference1.getY() + fb->myPosition[1].getY());
				fb->myPosition[2].set(difference1.getX() + fb->myPosition[2].getX(), difference1.getY() + fb->myPosition[2].getY());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				RCollectionBase* THAction = &((AddThreadMeasureAction*)action)->ActionCollection;
				for(RC_ITER Item_PPAction1 = THAction->getList().begin(); Item_PPAction1 != THAction->getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					//Vector temp = fb->PointDRO;
					//temp += difference1;
					//fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
					fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
					fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
					fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
					fb->myPosition[0].set(difference1.getX() + fb->myPosition[0].getX(), difference1.getY() + fb->myPosition[0].getY());
					fb->myPosition[1].set(difference1.getX() + fb->myPosition[1].getX(), difference1.getY() + fb->myPosition[1].getY());
					fb->myPosition[2].set(difference1.getX() + fb->myPosition[2].getX(), difference1.getY() + fb->myPosition[2].getY());

				}
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0048", __FILE__, __FUNCSIG__); }   
}

//Set the Object Image and calculate the CG.. According to that continue or exit.
bool PartProgramFunctions::SetObjImage_OneShot()
{
	try
	{
		list<BYTE*>::iterator tempitr = AllImageCollecton.begin();
		for(int i = 0; i < CurrentProramCount - 1; i++)
			tempitr++;
		BYTE* PixelVl = (*tempitr);
		bool flag = BESTFITOBJECT->setOneShotImage(PixelVl, MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio, MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio, 
													true, OneShotImgBoundryWidth(), OneShotImgBinariseVal(), ScaleFactor(), false, true);
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0049", __FILE__, __FUNCSIG__); return false; }   
}

//Get all images for current run..
int PartProgramFunctions::GetAlltheImages()
{
	try
	{
		int length = MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio * MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio;
		OneShotpixelarray = (BYTE*)malloc(sizeof(BYTE) *  length);
		bool dzm = MAINDllOBJECT->DigitalZoomMode();
		GetImageNow_byte_1bpp(1, OneShotpixelarray, !dzm);
		bool flag = BESTFITOBJECT->getIrreducibleComponents(OneShotpixelarray, MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio, MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio,
																true, &AllImageCollecton, &AllPointsColl, OneShotImgBoundryWidth(), OneShotImgBinariseVal());
		TotalNoOfImages = (int)AllImageCollecton.size();
		CurrentProramCount = 0;
		free(OneShotpixelarray); 
		OneShotpixelarray = NULL;
		return TotalNoOfImages;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0050", __FILE__, __FUNCSIG__); return 0; }  
}

//save all images.. for testing..
void PartProgramFunctions::SaveAllImages(BYTE* destination, int Cnt)
{
	try
	{
		list<BYTE*>::iterator tempitr = AllImageCollecton.begin();
		for(int i = 0; i < Cnt - 1; i++)
			tempitr++;
		BYTE* PixelVl = (*tempitr);
		int ImageSize = MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio * MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio * 3;
		for(int i = 0; i < ImageSize; i++)
			destination[i] = PixelVl[i];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0051", __FILE__, __FUNCSIG__); }   
}

//Clear the image list memory after all run opertations..
void PartProgramFunctions::ClearImageListMemory()
{
	try
	{
		for each(BYTE* ImageValue in AllImageCollecton)
			free(ImageValue);
		AllImageCollecton.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0052", __FILE__, __FUNCSIG__); }  
}

//Calculate Reference dot position
void PartProgramFunctions::CalculateReferenceDotPosition()
{
	try
	{
		MAINDllOBJECT->SetImageInEdgeDetection(1,false,2);
		double refdotParams[3] = { 0,0,0 };
		int noofpoints = EDSCOBJECT->Analyze_RotatingDot(true); // , MAINDllOBJECT->RefDot_Diameter(), refdotParams);

		if (noofpoints < EDSCOBJECT->RefDotDiameter / 2)
		{
			MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctions008", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}

		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y , upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio, ans[3];
		PointCollection tmpCollection;
		double* Temppts  = (double*)malloc(sizeof(double) *  noofpoints * 2);
		for (int n = 0; n < noofpoints; n++)
		{
			Temppts[n * 2] = cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp;
			Temppts[n * 2 + 1] = cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp;
			tmpCollection.Addpoint(new SinglePoint(Temppts[n * 2], Temppts[n * 2 + 1], MAINDllOBJECT->getCurrentDRO().getZ()));
		}
		MAINDllOBJECT->ReferenceDotShape = new Circle();
		MAINDllOBJECT->ReferenceDotShape->DoNotSaveInPP = true;
		MAINDllOBJECT->ReferenceDotShape->setId(MAINDllOBJECT->ToolAxisLineId - 1);
		((ShapeWithList*)MAINDllOBJECT->ReferenceDotShape)->AddPoints(&tmpCollection);
		HomePosition.set(((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getX(), ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		free(Temppts);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0053", __FILE__, __FUNCSIG__); }  
}

void PartProgramFunctions::CalculateSecondReferenceDotPosition()
{
	try
	{
		MAINDllOBJECT->SetImageInEdgeDetection();
		double refdotParams[3] = { 0,0,0 };
		int noofpoints = EDSCOBJECT->Analyze_RotatingDot(true); // , MAINDllOBJECT->RefDot_Diameter(), refdotParams);
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio, ans[3];
		PointCollection tmpCollection;
		double* Temppts  = (double*)malloc(sizeof(double) *  noofpoints * 2);
		for (int n = 0; n < noofpoints; n++)
		{
			Temppts[n * 2] = cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp;
			Temppts[n * 2 + 1] = cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp;
			tmpCollection.Addpoint(new SinglePoint(Temppts[n * 2], Temppts[n * 2 + 1], MAINDllOBJECT->getCurrentDRO().getZ()));
		}
		MAINDllOBJECT->SecondReferenceDotShape = new Circle();
		MAINDllOBJECT->SecondReferenceDotShape->DoNotSaveInPP = true;
		MAINDllOBJECT->SecondReferenceDotShape->setId(MAINDllOBJECT->ToolAxisLineId - 2);
		((ShapeWithList*)MAINDllOBJECT->SecondReferenceDotShape)->AddPoints(&tmpCollection);
		HomePosition_2nd.set(((Circle*)MAINDllOBJECT->SecondReferenceDotShape)->getCenter()->getX(), ((Circle*)MAINDllOBJECT->SecondReferenceDotShape)->getCenter()->getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		free(Temppts);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0053a", __FILE__, __FUNCSIG__); }  
}

//Set Home position after focus..
void PartProgramFunctions::SetReferenceDotPosition()
{
	try
	{
		if(FOCUSCALCOBJECT->FocusStatusFlag())
		{
			CalculateReferenceDotPosition();
			double x = HomePosition.getX(), y = HomePosition.getY();
			double z = HomePosition.getZ();
			if (abs(FOCUSCALCOBJECT->FocusZValue() - MAINDllOBJECT->getCurrentDRO().getZ()) < 2)
				z = FOCUSCALCOBJECT->FocusZValue();
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
			((Circle*)MAINDllOBJECT->ReferenceDotShape)->setCenter(Vector(x, y, z));// ));
			MAINDllOBJECT->getCurrentUCS().getShapes().addItem(MAINDllOBJECT->ReferenceDotShape);
			MAINDllOBJECT->SetShapeForReferenceDot();
			MAINDllOBJECT->UpdateShapesChanged();
			MAINDllOBJECT->SetStatusCode("PP0004");
		}
		else
		{
			MAINDllOBJECT->ShowMsgBoxString("PartProgramFunctions009", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
			if (MAINDllOBJECT->ReferenceDotShape != NULL)
			{
				MAINDllOBJECT->deleteReferenceShape((ShapeWithList*)MAINDllOBJECT->ReferenceDotShape);
				MAINDllOBJECT->ReferenceDotShape = NULL;
				PPCALCOBJECT->ReferenceDotIstakenAsHomePosition(false);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0054", __FILE__, __FUNCSIG__); }   
}

void PartProgramFunctions::SetSecondReferenceDotPosition()
{
	try
	{
		CalculateSecondReferenceDotPosition();

		double x = HomePosition_2nd.getX(), y = HomePosition_2nd.getY();
		double z = HomePosition_2nd.getZ();
		if (abs(FOCUSCALCOBJECT->FocusZValue() - MAINDllOBJECT->getCurrentDRO().getZ()) < 2)
			z = FOCUSCALCOBJECT->FocusZValue();
		FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
		((Circle*)MAINDllOBJECT->SecondReferenceDotShape)->setCenter(Vector(x, y, z));
		MAINDllOBJECT->getCurrentUCS().getShapes().addItem(MAINDllOBJECT->SecondReferenceDotShape);
		MAINDllOBJECT->SetShapeForReferenceDot(false);
		MAINDllOBJECT->UpdateShapesChanged();
		MAINDllOBJECT->SetStatusCode("PP0004");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0054a", __FILE__, __FUNCSIG__); }   
}

void PartProgramFunctions::ResetReferenceDotPosition(double* DroValue)
{
	try
	{
		if(MAINDllOBJECT->ReferenceDotShape != NULL)
		{
			Vector CenterPos;
			CenterPos.set(((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getX() - MAINDllOBJECT->getCurrentDRO().getX() + DroValue[0],
				 ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getY() - MAINDllOBJECT->getCurrentDRO().getY() + DroValue[1],
				 ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getZ() - MAINDllOBJECT->getCurrentDRO().getZ() + DroValue[2]);
			((Circle*)MAINDllOBJECT->ReferenceDotShape)->setCenter(CenterPos);
			MAINDllOBJECT->ReferenceDotShape->notifyAll(ITEMSTATUS::DATACHANGED, MAINDllOBJECT->ReferenceDotShape);
		}

		if(MAINDllOBJECT->SecondReferenceDotShape != NULL)
		{
			Vector CenterPos;
			CenterPos.set(((Circle*)MAINDllOBJECT->SecondReferenceDotShape)->getCenter()->getX() - MAINDllOBJECT->getCurrentDRO().getX() + DroValue[0],
				 ((Circle*)MAINDllOBJECT->SecondReferenceDotShape)->getCenter()->getY() - MAINDllOBJECT->getCurrentDRO().getY() + DroValue[1],
				 ((Circle*)MAINDllOBJECT->SecondReferenceDotShape)->getCenter()->getZ() - MAINDllOBJECT->getCurrentDRO().getZ() + DroValue[2]);
			((Circle*)MAINDllOBJECT->SecondReferenceDotShape)->setCenter(CenterPos);
			MAINDllOBJECT->SecondReferenceDotShape->notifyAll(ITEMSTATUS::DATACHANGED, MAINDllOBJECT->SecondReferenceDotShape);
		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0054b", __FILE__, __FUNCSIG__); }   
}

//Get the Success Status of Part program(Mainly after checking angle of rotary entity line)
bool PartProgramFunctions::CalculateDelphiPPSuccessStatus()
{
	try
	{
		Shape* s;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getShapes().getList().begin(); Item_Shape != Cucs->getShapes().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->ShapeAsRotaryEntity())
				{
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape->ShapeType == RapidEnums::SHAPETYPE::XLINE || Cshape->ShapeType == RapidEnums::SHAPETYPE::XRAY)
					{
						Line* Ln = (Line*)Cshape;
						if(Ln->Angle() > 5 * M_PI / 180 && Ln->Angle() < 175 * M_PI / 180)
							return false;
					}
				}
			}
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0055", __FILE__, __FUNCSIG__); }
}

void PartProgramFunctions::UpdateallSnapPtsForShapes()
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getShapes().getList().begin(); Item_Shape != Cucs->getShapes().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->IsDxfShape()) continue;
				if(Cshape->Normalshape())
				{
					Cucs->getIPManager().itemChanged(Cshape, NULL);
				}
			}
		}
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getShapes().getList().begin(); Item_Shape != Cucs->getShapes().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->IsDxfShape()) continue;
				if(Cshape->Normalshape())
				{
					Cshape->notifyAll(DATACHANGED, Cshape);
				}
			}
		}
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0056", __FILE__, __FUNCSIG__); }   
}

void PartProgramFunctions::CopyShapes_DuringProgram()
{
	try
	{
		CopyShapesAction* atn = static_cast<CopyShapesAction*>((*ppCurrentaction).second);
		MAINDllOBJECT->changeUCS(0);
		ChangeUCSAction_DuringProgram(atn->targetUcsId);
		MAINDllOBJECT->PartProgramShape_updated();
		partProgramReached();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0056", __FILE__, __FUNCSIG__); }  
}

void PartProgramFunctions::RotateShapes_DuringProgram()
{
	try
	{
		RotateShapeHandler *tmpHandler = new RotateShapeHandler();
		tmpHandler->PartProgramData();
		MAINDllOBJECT->PartProgramShape_updated();
		partProgramReached();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0056", __FILE__, __FUNCSIG__); }  
}

void PartProgramFunctions::ReflectShapes_DuringProgram()
{
	try
	{
		double reflectionParam[6] = {0};
		bool alongPlane = false;
		ReflectionAction* atn = static_cast<ReflectionAction*>((*ppCurrentaction).second);
		list<int>::iterator selectedShapeIter = atn->SelectedshapesId.begin();
		list<int>::iterator reflectedShapeIter = atn->ReflectedShapesId.begin();
		if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(atn->ReflectionSurfaceId))
		{
			Shape *reflectionSurface = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atn->ReflectionSurfaceId];
			if(reflectionSurface->ShapeType == RapidEnums::SHAPETYPE::PLANE )
			{
				((Plane*)reflectionSurface)->getParameters(reflectionParam);
				alongPlane = true;
			}
			else if(reflectionSurface->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				((Line*)reflectionSurface)->getParameters(reflectionParam);
			}
		
			for(int i = 0; (i < atn->SelectedshapesId.size()) && (i < atn->ReflectedShapesId.size()); i++)
			{
				if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(atn->ReflectionSurfaceId))
				{
					Shape *current_Shape = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[*selectedShapeIter];
					Shape *reflectedShape = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[*reflectedShapeIter];
					current_Shape->UpdateRefelctedShape(reflectedShape, reflectionParam, alongPlane);
					selectedShapeIter++;
					reflectedShapeIter++;
				}
			}
		}
		MAINDllOBJECT->PartProgramShape_updated();
		partProgramReached();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0056", __FILE__, __FUNCSIG__); }  
}

void PartProgramFunctions::TranslateShapes_DuringProgram()
{
	try
	{
		TranslationAction* atn = static_cast<TranslationAction*>((*ppCurrentaction).second);
		double translateArray[3] = {atn->TranslateDirection.getX(), atn->TranslateDirection.getY(), atn->TranslateDirection.getZ()};
		for(list<int>::iterator it = atn->TranslatedShapesId.begin(); it != atn->TranslatedShapesId.end(); it++)
		{
			if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(*it))
			{
				ShapeWithList *current_Shape = (ShapeWithList*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[*it];
				if(current_Shape->PointsList->getList().empty())
					current_Shape->Translate(atn->TranslateDirection);
				else
					current_Shape->TranslateShape(translateArray);
			}
		}
		MAINDllOBJECT->PartProgramShape_updated();
		partProgramReached();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0056", __FILE__, __FUNCSIG__); }  
}

void PartProgramFunctions::GetActionListInFailedMeasure(std::list<int> *actionIdList)
{
	try
	{
		for(RC_ITER MeasureItr = MAINDllOBJECT->getDimList().getList().begin(); MeasureItr != MAINDllOBJECT->getDimList().getList().end(); MeasureItr++)
		{
			DimBase* Cdim = (DimBase*)(*MeasureItr).second;
			if((Cdim->UpperTolerance() != 0.0 || Cdim->LowerTolerance() != 0.0) && Cdim->NominalValue() != 0.0)
			{
				if((Cdim->getDimension() > (Cdim->NominalValue() + Cdim->UpperTolerance())) || (Cdim->getDimension() < (Cdim->NominalValue() + Cdim->LowerTolerance())))
				{
					for(RC_ITER ppCAction = PPCALCOBJECT->getPPActionlist().getList().begin(); ppCAction != PPCALCOBJECT->getPPActionlist().getList().end(); ppCAction++)
					{
						RAction* Caction = (RAction*)(*ppCAction).second;
						if(Caction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) continue;
						for each(BaseItem *cShape in Cdim->getParents())  
						{
							if(cShape != NULL && (((AddPointAction*)Caction)->getShape()->getId() == cShape->getId()))
								actionIdList->push_back(Caction->getId());
						}
					}
				}
			}
		}
	}
	catch(...)
	{
	
	}
}

void PartProgramFunctions::SetCurrentAction(RAction* cAction)
{
	try
	{
		for(ppCurrentaction = getPPArrangedActionlist().getList().begin(); ppCurrentaction != getPPArrangedActionlist().getList().end(); ppCurrentaction++)
		{
			if(ppCurrentaction->second->getId() == cAction->getId()) break;
		}
	}
	catch(...){}
}

//Clear partprogram related flags.. delete memory etc..
void PartProgramFunctions::ClearAllPP()
{
	try
	{
		LastActionIdList_OffsetAction.clear();
		this->PPAlignEditFlag(false);
		this->PartprogramisLoadingEditMode(false);
		this->PartprogramisLoading(false);
		this->UpdateFistFramegrab(false);
		this->PartprogramLoaded(false);
		this->StartingPointofPartprogram(false);
		this->DrawPartprogramArrow(false);
		this->FirstMousedown_Partprogram(false);
		this->FirstMousedown_Partprogram_Manual(false);
		this->ProgramMadeUsingTwoStepHoming(false);
		this->SelectedShapesForTwoStepHoming(false);
		this->FinishedManualTwoStepAlignment(false);
		this->ProgramDoneForIdorOdMeasurement(false);
		this->ppCurrentaction = PPActionlist.getList().end();
		this->PPActionlist.deleteAll();
		this->PPShortestPathActionlist.clearAll();
		while(PPActionPositionColl.size() != 0)
		{
			map<int, ActionsPointCollection*>::iterator i = PPActionPositionColl.begin();
			ActionsPointCollection* bi = (*i).second;
			PPActionPositionColl.erase(bi->AP_Id);
			delete bi;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0055", __FILE__, __FUNCSIG__); }  
}

int PartProgramFunctions::ResetActionParameter()
{
	try
	{
		bool DeviationFlag = false; bool RelativePtFlag = false;
		for(RC_ITER pp = MAINDllOBJECT->getActionsHistoryList().getList().begin(); pp != MAINDllOBJECT->getActionsHistoryList().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::DEVIATIONACTION)
				DeviationFlag = true;
		}
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->Normalshape())
				{
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::RELATIVEPOINT)
						RelativePtFlag = true;					
				}
			}
		}
		if(DeviationFlag && RelativePtFlag)
			return int(RapidEnums::PROGRAM_PARAM::DEVIATION_RELATIVE_PT);
		else if(RelativePtFlag)
			return int(RapidEnums::PROGRAM_PARAM::RELATIVE_PTOFFSET);
		else if(DeviationFlag)
			return int(RapidEnums::PROGRAM_PARAM::DEVIATION);
		else
			return 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0056", __FILE__, __FUNCSIG__); return 0;}  
}

void PartProgramFunctions::SetDeviationTolerance(double Uppertolerance, double Lowertolerance)
{
	try
	{
		for(RC_ITER pp = MAINDllOBJECT->getActionsHistoryList().getList().begin(); pp != MAINDllOBJECT->getActionsHistoryList().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::DEVIATIONACTION)
				((DeviationAction*)action)->SetTolerance(Uppertolerance, Lowertolerance);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0057", __FILE__, __FUNCSIG__);}  
}

void PartProgramFunctions::ShiftShape_ForReferencedot_PPEdit()
{
	try
	{
		Vector difference;
		difference.set(HomePosition.getX() - PPHomePosition.getX(), HomePosition.getY() - PPHomePosition.getY(), HomePosition.getZ() - PPHomePosition.getZ());
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->Normalshape())
					Cshape->Translate(difference);				
			}
		}
		
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
			{
				bool Tempflag = false;
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->TranslatePosition(difference);
				if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
					Cdim->UpdateMeasurement();
			}
		}

		for(RC_ITER pp = MAINDllOBJECT->getActionsHistoryList().getList().begin(); pp != MAINDllOBJECT->getActionsHistoryList().getList().end(); pp++)
		{
			RAction* action = (RAction*)(*pp).second;
			if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fb =	((AddPointAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				FramegrabBase* fb =	((PerimeterAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			{
				FramegrabBase* fb =	((AddProbePathAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				FramegrabBase* fb =	((AddImageAction*)action)->getFramegrab();
				Vector temp = fb->PointDRO;
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			{
				for(RC_ITER Item_PPAction1 = ((AddThreadMeasureAction*)action)->ActionCollection.getList().begin(); Item_PPAction1 != ((AddThreadMeasureAction*)action)->ActionCollection.getList().end(); Item_PPAction1++)
				{
					AddThreadPointAction* CthAction = (AddThreadPointAction*)(*Item_PPAction1).second;
					FramegrabBase* fb =	CthAction->getFramegrab();
					Vector temp = fb->PointDRO;
					temp += difference;
					fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
					fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
					fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
					fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
			{
				((CircularInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
			{
				((LinearInterPolationAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
			{
				((ProfileScanAction*)action)->Translate(difference);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::AREA_ACTION)
			{
				FramegrabBase* fb =	((AreaShapeAction*)action)->getFramegrab();
				Vector temp;
				temp = fb->PointDRO;				
				temp += difference;
				fb->PointDRO.set(temp.getX(), temp.getY(), temp.getZ(), temp.getR());
				fb->points[0].set(difference.getX() + fb->points[0].getX(), difference.getY() + fb->points[0].getY(), difference.getZ() + fb->points[0].getZ());
				fb->points[1].set(difference.getX() + fb->points[1].getX(), difference.getY() + fb->points[1].getY(), difference.getZ() + fb->points[1].getZ());
				fb->points[2].set(difference.getX() + fb->points[2].getX(), difference.getY() + fb->points[2].getY(), difference.getZ() + fb->points[2].getZ());
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1PtAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1PtAction*)action)->SetPoint1(AlignPos);
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION)
			{
				if(((Cad1PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1PtAlignAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION)
			{
				if(((Cad2PtAlignAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint1(AlignPos);
					AlignPos = *((Cad2PtAlignAction*)action)->getPoint2();
					AlignPos += difference;
					((Cad2PtAlignAction*)action)->SetPoint2(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION)
			{
				if(((Cad1Pt1LineAction*)action)->IsEdited)
				{
					Vector AlignPos;
					AlignPos = *((Cad1Pt1LineAction*)action)->getPoint1();
					AlignPos += difference;
					((Cad1Pt1LineAction*)action)->SetPoint1(AlignPos);
				}
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION)
			{
				Vector AlignPos;
				if(((EditCad2PtAction*)action)->getActionCount() == 1)
				{					
					AlignPos = *((EditCad2PtAction*)action)->getPoint1();
					AlignPos += difference;
					((EditCad2PtAction*)action)->SetPoint1(AlignPos);
				}
				else if(((EditCad2PtAction*)action)->getActionCount() == 2)
				{
					AlignPos = *((EditCad2PtAction*)action)->getPoint2();
					AlignPos += difference;				
					((EditCad2PtAction*)action)->SetPoint2(AlignPos);
				}	
			}
			else if(action->CurrentActionType == RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION)
			{
				Vector AlignPos;
				AlignPos = *((EditCad1Pt1LineAction*)action)->getPoint1();
				AlignPos += difference;
				((EditCad1Pt1LineAction*)action)->SetPoint1(AlignPos);
			}
		}
		MAINDllOBJECT->updatePPGraphics(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0058", __FILE__, __FUNCSIG__);}  
}

void PartProgramFunctions::TranslateProgramShapeMeasureAction(Vector& TransVector)
{
	try
	{
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getPPShapes_Original().getList().begin(); Item_Shape != Cucs->getPPShapes_Original().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
				{
					Cshape->Translate(TransVector);
				}
			}
			for(RC_ITER Item_Shape = Cucs->getPPShapes_Path().getList().begin(); Item_Shape != Cucs->getPPShapes_Path().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
				{
					Cshape->Translate(TransVector);
				}
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Original().getList().begin(); Item_dim != Cucs->getPPMeasures_Original().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->TranslatePosition(TransVector);
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->TranslatePosition(TransVector);
			}
		}
		ShiftShape_ForFirstProbePoint(TransVector, true);
		CopyShapesMeasurementFromPPList();
		StoreOriginalActionForNextRun(&getPPActionlist());
		this->PartProgramAlignmentIsDone(true);
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0059", __FILE__, __FUNCSIG__);}  
}

void PartProgramFunctions::CopyShapesMeasurementFromPPList()
{
	try
	{
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			Cucs->getShapes().clearSelection();
			for(RC_ITER Item_shape = Cucs->getShapes().getList().begin(); Item_shape != Cucs->getShapes().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->Normalshape())
				{
					Cshape->DoneFrameGrab(false);
					if((Shape*)Cucs->getPPShapes_Original().ItemExists(Cshape->getId()))
					{
						Shape* Saved_Cshape = (Shape*)Cucs->getPPShapes_Original().getList()[Cshape->getId()];
						if(!Cshape->ShapeAsReferenceShape())
							Cshape->CopyOriginalProperties(Saved_Cshape);
						else
						{
							if(ProgramRunningForFirstTime())
								Cshape->CopyOriginalProperties(Saved_Cshape);
						}
						//Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);
					}
				}
			}
			for(RC_ITER Item_shape = Cucs->getPPShapes_Path().getList().begin(); Item_shape != Cucs->getPPShapes_Path().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(Cshape->Normalshape())
				{
					Shape* Saved_Cshape = (Shape*)Cucs->getPPShapes_Original().getList()[(*Item_shape).first];
					Cshape->CopyOriginalProperties(Saved_Cshape);
				}
			}
		}

		for(list<Shape*>::iterator Item_shape = DXFEXPOSEOBJECT->CurrentDXFShapeList.begin(); Item_shape != DXFEXPOSEOBJECT->CurrentDXFShapeList.end(); Item_shape++)
		{
			Shape* Cshape = (Shape*)(*Item_shape);
			if(DXFEXPOSEOBJECT->PPDXFShapeListOriginal.find(Cshape->getId()) != DXFEXPOSEOBJECT->PPDXFShapeListOriginal.end())
			{
				Shape *Saved_Cshape = DXFEXPOSEOBJECT->PPDXFShapeListOriginal.find(Cshape->getId())->second;
				Cshape->CopyOriginalProperties(Saved_Cshape);
				//Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);
			}
		}

		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			Cucs->getDimensions().clearSelection();
			for(RC_ITER Item_dim = Cucs->getDimensions().getList().begin(); Item_dim != Cucs->getDimensions().getList().end(); Item_dim++)
			{
				bool Tempflag = false;
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				DimBase* Saved_Dim = (DimBase*)Cucs->getPPMeasures_Original().getList()[Cdim->getId()];
				for each(BaseItem* Bitem in Cdim->getParents())
				{
					Shape* Csh = (Shape*)Bitem;
					if(Csh->ShapeAsReferenceShape())
					{
						Tempflag = true;
						break;
					}
				}
				if(!Tempflag)
					Cdim->CopyOriginalProperties(Saved_Dim, false);
				else
				{
					if(ProgramRunningForFirstTime())
						Cdim->CopyOriginalProperties(Saved_Dim, false);
				}
				//if(Cdim->MeasurementType != RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
				//	Cdim->UpdateMeasurement();
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				DimBase* Saved_Dim = (DimBase*)Cucs->getPPMeasures_Original().getList()[Cdim->getId()];
				Cdim->CopyOriginalProperties(Saved_Dim);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0060", __FILE__, __FUNCSIG__);}  
}

void PartProgramFunctions::TransformProgramShapeMeasureAction(double *TransformMatrix)
{
	try
	{
		Vector Position;
		Position.set(TransformMatrix[2], TransformMatrix[5]);
		TrnasformProgram_TwoStepHoming(TransformMatrix, false, true);
		
		for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getPPShapes_Original().getList().begin(); Item_Shape != Cucs->getPPShapes_Original().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
				{
					Cshape->Transform(TransformMatrix);
					Cshape->Translate(Position);
				}
			}
			for(RC_ITER Item_Shape = Cucs->getPPShapes_Path().getList().begin(); Item_Shape != Cucs->getPPShapes_Path().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->getId() > MAINDllOBJECT->ToolAxisLineId - MAINDllOBJECT->NumberOfReferenceShape && Cshape->getId() <= MAINDllOBJECT->ToolAxisLineId) continue;
				if(Cshape->Normalshape())
				{
					Cshape->Transform(TransformMatrix);
					Cshape->Translate(Position);
				}
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Original().getList().begin(); Item_dim != Cucs->getPPMeasures_Original().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				
				Cdim->TransformPosition(TransformMatrix);
				Cdim->TranslatePosition(Position);
			}
			for(RC_ITER Item_dim = Cucs->getPPMeasures_Path().getList().begin(); Item_dim != Cucs->getPPMeasures_Path().getList().end(); Item_dim++)
			{
				DimBase* Cdim = (DimBase*)(*Item_dim).second;
				Cdim->TransformPosition(TransformMatrix);
				Cdim->TranslatePosition(Position);
			}
		}
		
		StoreOriginalActionForNextRun(&getPPActionlist());
		this->PartProgramAlignmentIsDone(true);
		CopyShapesMeasurementFromPPList();
		MAINDllOBJECT->Measurement_updated();
		MAINDllOBJECT->UpdateShapesChanged();
		
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0061", __FILE__, __FUNCSIG__);
	}
}
