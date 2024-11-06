#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "../MainDll/Shapes/ShapeHeaders.h"

void CALLBACK FgPointIdList(std::list<int> *PtIdlist)
{
	try
	{
		System::Collections::Generic::List<System::Int32>^ IDList = gcnew System::Collections::Generic::List<System::Int32>();
		for each(int Cvalue in *PtIdlist)
		{
			 System::Int32 temp =  System::Int32(Cvalue);
	         IDList->Add(temp);
		}
		RWrapper::RW_ShapeParameter::MYINSTANCE()->ShowFgPointsEvent(IDList);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRSHPRD0001a", ex);
	}
}

RWrapper::RW_ShapeParameter::RW_ShapeParameter()
{
	try
	{
		this->ShParamInstance = this;	
		this->ShapePropertiesArray = gcnew System::Collections::ArrayList(); 
		this->SelectedShapeIDList = gcnew  System::Collections::Generic::List<System::Int32>; 
		this->ShapeParameter_ArrayList = gcnew System::Collections::ArrayList;
		MAINDllOBJECT->ShowFgPtIdList = &FgPointIdList;
		this->Shape_EntityID = 0;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRSHPRD0001", ex); }
}

RWrapper::RW_ShapeParameter::~RW_ShapeParameter()
{
}

RWrapper::RW_ShapeParameter^ RWrapper::RW_ShapeParameter::MYINSTANCE()
{
	return ShParamInstance;
}

void RWrapper::RW_ShapeParameter::ShowFgPointsEvent(System::Collections::Generic::List<System::Int32>^ IDList)
 {
	 ShowFgPtListEvent::raise(IDList);
 }

void RWrapper::RW_ShapeParameter::GetTotal_ShapeList()
{
	try
	{
		ShapePropertiesArray->Clear();
		//If the Shapelist count is "0" then reurn...//
		if(MAINDllOBJECT->getShapesList().count() <= 0) return;
		GetShapeList();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0002", ex); }
}

void RWrapper::RW_ShapeParameter::SetShapeParam(char* SName,char* Stype, double clicks,char* Ucs)
{
	try
	{
		this->ShapePropertiesArray->Add(gcnew System::String(SName));
		this->ShapePropertiesArray->Add(gcnew System::String(Stype));
		this->ShapePropertiesArray->Add(clicks);
		this->ShapePropertiesArray->Add(gcnew System::String(Ucs));
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0023", ex); }
}

void RWrapper::RW_ShapeParameter::GetShapeList()
{
	try
	{
		for (RC_ITER item1 = MAINDllOBJECT->getUCSList().getList().begin(); item1 != MAINDllOBJECT->getUCSList().getList().end(); item1++)
		{
			UCS* CurrentUcs = (UCS*)(*item1).second;
			//Get all the shapelist with , shape name , shape type, no. of points taken and name...//
			RCollectionBase& ShapeCollection = CurrentUcs->getShapes();
			for(RC_ITER item = ShapeCollection.getList().begin(); item != ShapeCollection.getList().end(); item++)
			{
				ShapeWithList* sh = (ShapeWithList*)(*item).second;
				switch(sh->ShapeType)
				{
				case RapidEnums::SHAPETYPE::RPOINT:
					SetShapeParam(sh->getModifiedName(),"Point",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::LINE:
					SetShapeParam(sh->getModifiedName(),"LineSegment",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::XLINE:
					SetShapeParam(sh->getModifiedName(),"Line",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::XRAY:
					SetShapeParam(sh->getModifiedName(),"Ray",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::CIRCLE:
					SetShapeParam(sh->getModifiedName(),"Circle",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::ARC:
					SetShapeParam(sh->getModifiedName(),"Arc",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
					SetShapeParam(sh->getModifiedName(),"Cloud Points",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::POLYLINE:
					SetShapeParam(sh->getModifiedName(),"PolyLine",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				case RapidEnums::SHAPETYPE::PERIMETER:
					SetShapeParam(sh->getModifiedName(),"Perimeter",sh->PointsList->Pointscount(),CurrentUcs->getModifiedName());
					break;
				}
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRPP0028", ex); }
}

void RWrapper::RW_ShapeParameter::SetShapeValues(double *m1)
{
	try
	{
		//System::String^ MeasureFormat = "0.0";
		//for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
		//	MeasureFormat = MeasureFormat + "0";
		//int Stype = (int)*(m1 + 4);
		//switch(Stype)
		//{
		//	case 0:
		//		{
		//		char AngledegMin[100];
		//		double angle = *(m1 + 1), angle1;
		//		System::String^ LineAngle;
		//		if(MAINDllOBJECT->AngleMeasurementMode() == 1)
		//		{
		//			angle1 = RMATH2DOBJECT->Radian2Degree(angle);
		//			ShapeParameter[1] = Microsoft::VisualBasic::Strings::Format(angle1, MeasureFormat);
		//		}
		//		else if(MAINDllOBJECT->AngleMeasurementMode() == 2)
		//		{
		//			ShapeParameter[1] = Microsoft::VisualBasic::Strings::Format(angle, MeasureFormat);
		//		}
		//		else if(MAINDllOBJECT->AngleMeasurementMode() == 0) 
		//		{
		//			RMATH2DOBJECT->Radian2Deg_Min_Sec(angle, &AngledegMin[0]);
		//			ShapeParameter[1] = gcnew System::String(&AngledegMin[0]);
		//		}
		//		ShapeParameter[0] = Microsoft::VisualBasic::Strings::Format(m1[0], MeasureFormat);
		//		ShapeParameter[2] = Microsoft::VisualBasic::Strings::Format(m1[2], MeasureFormat);
		//		ShapeParameter[3] = "";
		//		//RWrapper::RW_MainInterface::MYINSTANCE()->RWrapperUIelement->Dispatcher->Invoke(UpdateShapeParam_MouseMoveInvoke);
		//		}
		//		break;
		//	case 1:
		//		ShapeParameter[0] = Microsoft::VisualBasic::Strings::Format(m1[0], MeasureFormat);
		//		ShapeParameter[1] = Microsoft::VisualBasic::Strings::Format(m1[1], MeasureFormat);
		//		ShapeParameter[2] = Microsoft::VisualBasic::Strings::Format(m1[2], MeasureFormat);
		//		ShapeParameter[3] = Microsoft::VisualBasic::Strings::Format(m1[3], MeasureFormat);
		//		//RWrapper::RW_MainInterface::MYINSTANCE()->RWrapperUIelement->Dispatcher->Invoke(UpdateShapeParam_MouseMoveInvoke);
		//		break;
		//	case 2:
		//	case 3:
		//		ShapeParameter[0] = Microsoft::VisualBasic::Strings::Format(m1[0], MeasureFormat);
		//		ShapeParameter[1] = Microsoft::VisualBasic::Strings::Format(m1[1], MeasureFormat);
		//		ShapeParameter[2] = "";
		//		ShapeParameter[3] = "";
		//		//RWrapper::RW_MainInterface::MYINSTANCE()->RWrapperUIelement->Dispatcher->Invoke(UpdateShapeParam_MouseMoveInvoke);
		//		break;
		//	case 4:
		//		ShapeParameter[0] = Microsoft::VisualBasic::Strings::Format(m1[0], MeasureFormat);
		//		ShapeParameter[1] = Microsoft::VisualBasic::Strings::Format(m1[1], MeasureFormat);
		//		ShapeParameter[2] = Microsoft::VisualBasic::Strings::Format(m1[2], MeasureFormat);
		//		ShapeParameter[3] = Microsoft::VisualBasic::Strings::Format(m1[3], MeasureFormat);
		//		//RWrapper::RW_MainInterface::MYINSTANCE()->RWrapperUIelement->Dispatcher->Invoke(UpdateShapeParam_MouseMoveInvoke);
		//		break;
		//}
		
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0039", ex);
	}
}

void RWrapper::RW_ShapeParameter::SetShapeParameters(System::String^ lbl1txt, System::String^ lbl2txt, System::String^ lbl3txt, System::String^ lbl4txt, System::String^ txt1txt, System::String^ txt2txt, System::String^ txt3txt, System::String^ txt4txt)
{
	try
	{
		/*ShapeParameterlbl[0] = lbl1txt;
		ShapeParameterlbl[1] = lbl2txt;
		ShapeParameterlbl[2] = lbl3txt;
		ShapeParameterlbl[3] = lbl4txt;
		ShapeParameter[0] = txt1txt;
		ShapeParameter[1] = txt2txt;
		ShapeParameter[2] = txt3txt;
		ShapeParameter[3] = txt4txt;*/
		//RWrapper::RW_MainInterface::MYINSTANCE()->RWrapperUIelement->Dispatcher->Invoke(UpdateShapeParam_MouseMoveInvoke);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0040", ex);
	}
}

void RWrapper::RW_ShapeParameter::SetShapeParamForTable()
{
	try
	{
		double UnitDivide = 1;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			UnitDivide = 25.4;
		SelectedShapeIDList->Clear();
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		System::String^ MeasureFormat = "0.0";
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		System::Data::DataTable^ DTtable = gcnew System::Data::DataTable();
		RCollectionBase& Shapecoll = MAINDllOBJECT->getShapesList();
		DTtable->Columns->Add("ShapeID");
		DTtable->Columns->Add("Name");
		DTtable->Columns->Add("Type");
		DTtable->Columns->Add("Hide Property");
		DTtable->Columns->Add("Param1");
		DTtable->Columns->Add("Param2");		
		DTtable->Columns->Add("Param3");
		DTtable->Columns->Add("Param4");
		DTtable->Columns->Add("Param5");
		DTtable->Columns->Add("Param6");
		DTtable->Columns->Add("Param7");
		DTtable->Columns->Add("Param8");

		for(RC_ITER i = Shapecoll.getList().begin(); i != Shapecoll.getList().end(); i++)
		{
			BaseItem* item = &(*i->second);
			Shape* currentShape = (Shape*)item;
			if(!currentShape->Normalshape()) //&& !currentShape->IsUsedForReferenceDot()) 
				continue;
			if(currentShape->selected())SelectedShapeIDList->Add(currentShape->getId());
			//if(!currentShape->IsValid()) continue;
			switch(currentShape->ShapeType)
			{
			case RapidEnums::SHAPETYPE::LINE:
				{
				System::String^ LineAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((Line*)currentShape)->Angle(), CMeasureMode, MeasureFormat, true);
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Line", currentShape->IsHidden(), 
					"Px " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getX()/UnitDivide, MeasureFormat),
					"Py " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getY()/UnitDivide, MeasureFormat),
					"Pz " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getZ()/UnitDivide, MeasureFormat),
					"A " + LineAngle,
					"I " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Intercept()/UnitDivide, MeasureFormat),
					"L " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Length()/UnitDivide, MeasureFormat),
					"", "");
				}
				break;
			case RapidEnums::SHAPETYPE::CIRCLE:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Circle", currentShape->IsHidden(),
					"Cx " +  Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat),
					"Cy " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat),
					"Cz " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat),
					"R " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Radius()/UnitDivide, MeasureFormat),
					"", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::XLINE:
			case RapidEnums::SHAPETYPE::XRAY:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "XLine", currentShape->IsHidden(),
					"A " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Angle(), MeasureFormat),
					"I " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Intercept()/UnitDivide, MeasureFormat),
					"", "", "", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::RTEXT:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Text", currentShape->IsHidden(),
					"T " + gcnew System::String((char*)((RText*)currentShape)->getText().c_str()), "", "", "", "", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::LINE3D:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Line3D", currentShape->IsHidden(),
					"Px " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getX()/UnitDivide, MeasureFormat),
					"Py " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getY()/UnitDivide, MeasureFormat),
					"Pz " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getZ()/UnitDivide, MeasureFormat),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"LEN " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Length()/UnitDivide, MeasureFormat),
					"");
				break;
			case RapidEnums::SHAPETYPE::XLINE3D:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "XLine3D", currentShape->IsHidden(),
					"Px " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getX()/UnitDivide, MeasureFormat),
					"Py " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getY()/UnitDivide, MeasureFormat),
					"Pz " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getZ()/UnitDivide, MeasureFormat),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"", "");
				break;
			case RapidEnums::SHAPETYPE::ARC:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Arc", currentShape->IsHidden(),
					"Cx " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat),
					"Cy " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat),
					"Cz " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat),
					"R " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Radius()/UnitDivide, MeasureFormat),
					"SA " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Sweepangle(), MeasureFormat), "", "", "");
				break;
			case RapidEnums::SHAPETYPE::RPOINT:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Point", currentShape->IsHidden(),
					"X " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getX()/UnitDivide, MeasureFormat),
					"Y " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getY()/UnitDivide, MeasureFormat),
					"Z " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getZ()/UnitDivide, MeasureFormat),
					"", "", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::RPOINT3D:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Point3D", currentShape->IsHidden(),
					"X " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getX()/UnitDivide, MeasureFormat),
					"Y " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getY()/UnitDivide, MeasureFormat),
					"Z " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getZ()/UnitDivide, MeasureFormat),
					"", "", "", "" );
				break;
			case RapidEnums::SHAPETYPE::RELATIVEPOINT:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "RelativePoint", currentShape->IsHidden(),
					"X " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getX()/UnitDivide, MeasureFormat),
					"Y " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getY()/UnitDivide, MeasureFormat),
					"Z " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getZ()/UnitDivide, MeasureFormat),
					"", "", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::CLOUDPOINTS:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "CloudPoints", currentShape->IsHidden(), "", "", "", " ", " ", " ", " ", " ");
				break;
			case RapidEnums::SHAPETYPE::POLYLINE:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "PolyLine", currentShape->IsHidden(), "", "", "", " ", " ", " ", " ", " ");
				break;
			case RapidEnums::SHAPETYPE::CYLINDER:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Cylinder", currentShape->IsHidden(),
					"Px " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->getCenter1()->getX()/UnitDivide, MeasureFormat),
					"Py " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->getCenter1()->getY()/UnitDivide, MeasureFormat),
					"Pz " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->getCenter1()->getZ()/UnitDivide, MeasureFormat),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cylinder*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cylinder*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cylinder*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"R " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->Radius1()/UnitDivide, MeasureFormat),
					"");
				break;
			case RapidEnums::SHAPETYPE::CONE:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Cone", currentShape->IsHidden(),
					"Px " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->getCenter1()->getX()/UnitDivide, MeasureFormat),
					"Py " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->getCenter1()->getY()/UnitDivide, MeasureFormat),
					"Pz " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->getCenter1()->getZ()/UnitDivide, MeasureFormat),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cone*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cone*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cone*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"R1 " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->Radius1()/UnitDivide, MeasureFormat),
					"R2 " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->Radius2()/UnitDivide, MeasureFormat));
				break;
			case RapidEnums::SHAPETYPE::PLANE:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Plane", currentShape->IsHidden(),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Plane*)currentShape)->dir_a()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Plane*)currentShape)->dir_b()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Plane*)currentShape)->dir_c()) * 180/Math::PI, MeasureFormat),
					"D " + Microsoft::VisualBasic::Strings::Format(((Plane*)currentShape)->dir_d()/UnitDivide, MeasureFormat),
					"",	"", "", "");
				break;
			case RapidEnums::SHAPETYPE::SPHERE:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Sphere", currentShape->IsHidden(),
					"X " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat),
					"Y " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat),
					"Z " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat),
					"R " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->Radius()/UnitDivide, MeasureFormat),
					"", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::PERIMETER:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Perimeter", currentShape->IsHidden(),
					"P " + Microsoft::VisualBasic::Strings::Format(((Perimeter*)currentShape)->TotalPerimeter()/UnitDivide, MeasureFormat),
					"D " + Microsoft::VisualBasic::Strings::Format(((Perimeter*)currentShape)->Diameter()/UnitDivide, MeasureFormat),
					"A " + Microsoft::VisualBasic::Strings::Format(((Perimeter*)currentShape)->TotalArea()/(UnitDivide * UnitDivide), MeasureFormat),
					"", "", "", "", "");
				break;
		    case RapidEnums::SHAPETYPE::TORUS:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Torus", currentShape->IsHidden(),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Torus*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Torus*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Torus*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"R1 " + Microsoft::VisualBasic::Strings::Format(((Torus*)currentShape)->Radius1()/UnitDivide, MeasureFormat),
					"R2 " + Microsoft::VisualBasic::Strings::Format(((Torus*)currentShape)->Radius2()/UnitDivide, MeasureFormat), "", "", "");
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Circle3D", currentShape->IsHidden(),
					"Cx " +  Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat),
					"Cy " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat),
					"Cz " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat),
					"R " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Radius()/UnitDivide, MeasureFormat),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Circle*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Circle*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Circle*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat), "");
				break;
			case RapidEnums::SHAPETYPE::ELLIPSE3D:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Ellipse3D", currentShape->IsHidden(),
	            	"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"E " + Microsoft::VisualBasic::Strings::Format(((Conics3D*)currentShape)->eccentricity()/UnitDivide, MeasureFormat),
					"", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::PARABOLA3D:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Parabola3D", currentShape->IsHidden(),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"E " + Microsoft::VisualBasic::Strings::Format(((Conics3D*)currentShape)->eccentricity()/UnitDivide, MeasureFormat),
					"", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::HYPERBOLA3D:
				DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Hyperbola3D", currentShape->IsHidden(),
					"L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat),
					"M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat),
					"N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat),
					"E " + Microsoft::VisualBasic::Strings::Format(((Conics3D*)currentShape)->eccentricity()/UnitDivide, MeasureFormat),
					"", "", "", "");
				break;
			case RapidEnums::SHAPETYPE::TWOARC:
				{
					System::String^ LineAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((TwoArc*)currentShape)->Angle(), CMeasureMode, MeasureFormat, true);
				    DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "TwoArc", currentShape->IsHidden(),
					"S " + LineAngle,
					"R1 " + Microsoft::VisualBasic::Strings::Format(((TwoArc*)currentShape)->Radius1()/UnitDivide, MeasureFormat),
					"R2 " + Microsoft::VisualBasic::Strings::Format(((TwoArc*)currentShape)->Radius2()/UnitDivide, MeasureFormat),
					"", "", "", "", "");
					break;
				}
			case RapidEnums::SHAPETYPE::TWOLINE:
				{
					System::String^ Angle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((TwoLine*)currentShape)->Angle(), CMeasureMode, MeasureFormat, true);
					System::String^ LineAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((TwoLine*)currentShape)->LineAngle(), CMeasureMode, MeasureFormat, true);
				     DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "TwoLine", currentShape->IsHidden(),
					"S " + Angle,
					"LA " + LineAngle,
					"", "", "", "", "", "");
				    break;
				}
			case RapidEnums::SHAPETYPE::TRIANGULARSURFACE:
				{
					 DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "TriangularSurface", currentShape->IsHidden(),
					((TriangularSurface*)currentShape)->TriangleCount, "" ,	"", "", "", "", "", "");
				    break;
				}
			case RapidEnums::SHAPETYPE::DOTSCOUNT:
				{
					 DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "DotsCount", currentShape->IsHidden(),
					((DotsCount*)currentShape)->CircleCollection.size(), ((DotsCount*)currentShape)->FrameArea, "", "", "", "", "", "");
				    break;
				}
			case RapidEnums::SHAPETYPE::CAMPROFILE:
				{
					double CamParameter[4] = {0};
					((CamProfile*)currentShape)->GetCamProfileParam(CamParameter);
					 DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "CamProfile", currentShape->IsHidden(),
					"X " + Microsoft::VisualBasic::Strings::Format(CamParameter[0], MeasureFormat), "Y " + Microsoft::VisualBasic::Strings::Format(CamParameter[1], MeasureFormat),
					"R " + Microsoft::VisualBasic::Strings::Format(CamParameter[2], MeasureFormat), "A " + Microsoft::VisualBasic::Strings::Format(CamParameter[3] * 180 / M_PI, MeasureFormat), "", "", "", "");
				    break;
				}
			case RapidEnums::SHAPETYPE::SPLINE:
				{
					 DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Spline", currentShape->IsHidden(),
					"", "", "", "", "", "", "", "");
				    break;
				}
			case RapidEnums::SHAPETYPE::AREA:
				{
					 DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Area", currentShape->IsHidden(),
					"", "", "", "", "", "", "", "");
				    break;
				}
			case RapidEnums::SHAPETYPE::AUTOFOCUSSURFACE:
				{
					 DTtable->Rows->Add(currentShape->getId(), gcnew System::String(currentShape->getModifiedName()), "Surface", currentShape->IsHidden(),
					"", "", "", "", "", "", "", "");
				    break;
				}
			}
		}
		delete ShapeParameterTable;
		ShapeParameterTable = DTtable;
		RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_TABLE_UPDATE);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0040b", ex);
	}
}

void RWrapper::RW_ShapeParameter::ResetShapePointsTable()
{
	try
	{
		System::Data::DataTable^ DTtable =  gcnew System::Data::DataTable();
		DTtable->Columns->Add("Name"); DTtable->Columns->Add("X"); DTtable->Columns->Add("Y");
		DTtable->Columns->Add("Z");	DTtable->Columns->Add("R");	DTtable->Columns->Add("PointID");
		RWrapper::RW_ShapeParameter::MYINSTANCE()->ShapePointsTable = DTtable;
		//RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_SELECTION_CHANGED);
	    //RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_POINTS_UPDATE);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0040", ex);
	}
}

void RWrapper::RW_ShapeParameter::UpdateSelectedShape_PointsTable()
{
	try
	{
		RCollectionBase& Shapecollselected = MAINDllOBJECT->getSelectedShapesList();
		if(Shapecollselected.count() == 1)
		{
			Shape* CShape = (Shape*)Shapecollselected.getList().begin()->second;
			if(CShape->ShapeType != RapidEnums::SHAPETYPE::DUMMY && CShape->ShapeType != RapidEnums::SHAPETYPE::DEPTHSHAPE)
				SetShapeProperties(CShape);
		}
		else
			ResetShapePointsTable();
		RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_POINTS_UPDATE);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0040", ex);
	}
}

void RWrapper::RW_ShapeParameter::SetShapeProperties(Shape* currentShape)
{
	try
	{
		//if(PPCALCOBJECT->IsPartProgramRunning()) return;
		double UnitDivide = 1;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			UnitDivide = 25.4;
		System::String^ MeasureFormat = "0.0";
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		if(currentShape->ShapeType == RapidEnums::SHAPETYPE::DUMMY || currentShape->ShapeType == RapidEnums::SHAPETYPE::DEPTHSHAPE)
		{
			ResetShapePointsTable();
			return;
		}
		int _counter = 0;
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		PointCollection* Pointlist = ((ShapeWithList*)currentShape)->PointsListOriginal;
		RapidEnums::RAPIDPROJECTIONTYPE Cptype = RapidEnums::RAPIDPROJECTIONTYPE(MAINDllOBJECT->getCurrentUCS().UCSProjectionType());
		System::Data::DataTable^ DTtable =  gcnew System::Data::DataTable();
		if(RWrapper::RW_DBSettings::MYINSTANCE()->NoofMachineAxis == 4)
		{
			DTtable->Columns->Add("Name");
			DTtable->Columns->Add("X");
			DTtable->Columns->Add("Y");
			DTtable->Columns->Add("Z");		
			DTtable->Columns->Add("R");	
			DTtable->Columns->Add("PointID");
			for(PC_ITER Item = Pointlist->getList().begin(); Item != Pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = Item->second;
				_counter += 1;
				System::String^ _str = System::String::Concat("Pt", _counter);
				System::String^ LineAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(Spt->R, CMeasureMode, MeasureFormat, true);
				if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XY)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->X/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Z/UnitDivide, MeasureFormat), LineAngle, Spt->PtID);
				else if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->Z/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->X/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y/UnitDivide, MeasureFormat), LineAngle, Spt->PtID);
				else if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->X/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Z/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y/UnitDivide, MeasureFormat), LineAngle, Spt->PtID);
				else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(-Spt->Z / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->X / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y / UnitDivide, MeasureFormat), LineAngle, Spt->PtID);
				else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->X / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(-Spt->Z / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y / UnitDivide, MeasureFormat), LineAngle, Spt->PtID);
			}
		}
		else
		{
			DTtable->Columns->Add("Name");
			DTtable->Columns->Add("X");
			DTtable->Columns->Add("Y");
			DTtable->Columns->Add("Z");		
			DTtable->Columns->Add("PointID");
			for(PC_ITER Item = Pointlist->getList().begin(); Item != Pointlist->getList().end(); Item++)
			{
				SinglePoint* Spt = Item->second;
				_counter += 1;
				System::String^ _str = System::String::Concat("Pt",_counter);
				if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XY)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->X/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Z/UnitDivide, MeasureFormat), Spt->PtID);
				else if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->Z/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->X/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y/UnitDivide, MeasureFormat), Spt->PtID);
				else if(Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->X/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Z/UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y/UnitDivide, MeasureFormat), Spt->PtID);
				else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(-Spt->Z / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->X / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y / UnitDivide, MeasureFormat), Spt->PtID);
				else if (Cptype == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
					DTtable->Rows->Add(_str, Microsoft::VisualBasic::Strings::Format(Spt->X / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(-Spt->Z / UnitDivide, MeasureFormat), Microsoft::VisualBasic::Strings::Format(Spt->Y / UnitDivide, MeasureFormat), Spt->PtID);
			}
		}
		delete ShapePointsTable;
		ShapePointsTable = DTtable;
		//RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_POINTS_UPDATE);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0040b", ex);
	}
}

void RWrapper::RW_ShapeParameter::RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE ShapeType)
{
	ShapeParameterUpdate::raise(ShapeType);
		
}

void RWrapper::RW_ShapeParameter::SelectShape(int Shapeid)
 {
	 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 MAINDllOBJECT->selectShape(Shapeid, MAINDllOBJECT->ControlOn);
 }

void RWrapper::RW_ShapeParameter::SelectShape(cli::array<int, 1>^ IdList)
 {
	 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 std::list<int> PtIdlist;
	 for each(System::Int32^ Cvalue in IdList)
	 {
		 int temp = (int)Cvalue;
	     PtIdlist.push_back(temp);
	 }
	 MAINDllOBJECT->selectShape(&PtIdlist);
 }

void RWrapper::RW_ShapeParameter::SelectPointsList(cli::array<int, 1>^ IdList)
 {
	 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 std::list<int> PtIdlist;
	 for each(System::Int32^ Cvalue in IdList)
	 {
		 int temp = (int)Cvalue;
	     PtIdlist.push_back(temp);
	 }
	 MAINDllOBJECT->selectFGPoints(&PtIdlist);
 }

void RWrapper::RW_ShapeParameter::DeleteSelectedShape()
 {
	 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 MAINDllOBJECT->deleteShape(); 
 }

void RWrapper::RW_ShapeParameter::DeleteSelectedPointsList()
 {
	 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 MAINDllOBJECT->deleteFGPoints();
 }

void RWrapper::RW_ShapeParameter::GotoSelectedPtPosition()
{
	try
	{
		ShapeWithList *Cshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(Cshape->PointsList->SelectedPointsCnt == 0) return;
		if(MAINDllOBJECT->getSelectedShapesList().count() == 1)
		{
			bool SendCommand = false;
			double target[4] = {0};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			for(PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->IsSelected)
				{			
					SendCommand = true;
					target[0] = Spt->X; target[1] = Spt->Y; target[2] = Spt->Z; target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];					
					break;
				}
			}
			if(SendCommand)
			{
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);			
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], RWrapper::RW_DRO::MYINSTANCE()->TargetReachedCallback::MACHINE_GOTOCOMMAND);
			}
		}
		else
			MAINDllOBJECT->SetStatusCode("RCadApp16");
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}

void RWrapper::RW_ShapeParameter::RenameShape(System::String^ Str)
{
	try
	{
		Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		CShape->setModifiedName((char*)(void*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
		RWrapper::RW_MainInterface::EntityID = CShape->getId();
		RWrapper::RW_MainInterface::EntityName = gcnew System::String(CShape->getModifiedName());
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0043", ex);
	}
}

void RWrapper::RW_ShapeParameter::SetTextValue_TextShape(System::String^ Str)
{
	 MAINDllOBJECT->SetTextForTextShape((char*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
}

//changes by rahul on 18 may 12.............
void RWrapper::RW_ShapeParameter::SetShapeParamForFrontend(Shape* currentShape)
{
	try
	{
		this->ShapeParameter_ArrayList->Clear();
		double UnitDivide = 1;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			UnitDivide = 25.4;
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		System::String^ MeasureFormat = "0.0";
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		if(!currentShape->Normalshape()) // && !currentShape->IsUsedForReferenceDot()) 
			return;
		switch(currentShape->ShapeType)
			{
			case RapidEnums::SHAPETYPE::LINE:
				{
					System::String^ LineAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((Line*)currentShape)->Angle(), CMeasureMode, MeasureFormat, true);
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("Line");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("Px " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getX()/UnitDivide, MeasureFormat));
			    	this->ShapeParameter_ArrayList->Add("Py " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getY()/UnitDivide, MeasureFormat));
			    	this->ShapeParameter_ArrayList->Add("Pz " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getZ()/UnitDivide, MeasureFormat));
					this->ShapeParameter_ArrayList->Add("A " + LineAngle);
					this->ShapeParameter_ArrayList->Add("I " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Intercept()/UnitDivide, MeasureFormat));
					this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Length()/UnitDivide, MeasureFormat));
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
				}
				break;
			case RapidEnums::SHAPETYPE::CIRCLE:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Circle");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("Cx " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Cy " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Cz " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Radius()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::XLINE:
			case RapidEnums::SHAPETYPE::XRAY:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("XLine");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("A " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Angle(), MeasureFormat));
				this->ShapeParameter_ArrayList->Add("I " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Intercept()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::RTEXT:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Text");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("T " + gcnew System::String((char*)((RText*)currentShape)->getText().c_str()));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::LINE3D:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Line3D");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("Px " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getX()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Py " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getY()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Pz " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("LEN " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->Length()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::XLINE3D:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("XLine3D");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("Px " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getX()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Py " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getY()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Pz " + Microsoft::VisualBasic::Strings::Format(((Line*)currentShape)->getPoint1()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Line*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::ARC:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Arc");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("Cx " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Cy " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Cz " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Radius()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("SA " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Sweepangle(), MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::RPOINT:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Point");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("X " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getX()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Y " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getY()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Z " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::RPOINT3D:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Point3D");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("X " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getX()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Y " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getY()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Z " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::RELATIVEPOINT:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("RelativePoint");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("X " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getX()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Y " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getY()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Z " + Microsoft::VisualBasic::Strings::Format(((RPoint*)currentShape)->getPosition()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::CLOUDPOINTS:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("CloudPoints");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::POLYLINE:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("PolyLine");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::CYLINDER:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Cylinder");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("Px " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->getCenter1()->getX()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Py " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->getCenter1()->getY()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Pz " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->getCenter1()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cylinder*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cylinder*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cylinder*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R " + Microsoft::VisualBasic::Strings::Format(((Cylinder*)currentShape)->Radius1()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::CONE:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Cone");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("Px " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->getCenter1()->getX()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Py " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->getCenter1()->getY()/UnitDivide, MeasureFormat));
			    this->ShapeParameter_ArrayList->Add("Pz " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->getCenter1()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cone*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cone*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Cone*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R1 " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->Radius1()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R2 " + Microsoft::VisualBasic::Strings::Format(((Cone*)currentShape)->Radius2()/UnitDivide, MeasureFormat));
				break;
			case RapidEnums::SHAPETYPE::PLANE:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Plane");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Plane*)currentShape)->dir_a()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Plane*)currentShape)->dir_b()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Plane*)currentShape)->dir_c()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("D " + Microsoft::VisualBasic::Strings::Format(((Plane*)currentShape)->dir_d()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::SPHERE:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Sphere");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("X " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Y " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Z " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R " + Microsoft::VisualBasic::Strings::Format(((Sphere*)currentShape)->Radius()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::PERIMETER:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Perimeter");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("P " + Microsoft::VisualBasic::Strings::Format(((Perimeter*)currentShape)->TotalPerimeter()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("D " + Microsoft::VisualBasic::Strings::Format(((Perimeter*)currentShape)->Diameter()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("A " + Microsoft::VisualBasic::Strings::Format(((Perimeter*)currentShape)->TotalArea()/(UnitDivide * UnitDivide), MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
		    case RapidEnums::SHAPETYPE::TORUS:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Torus");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Torus*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Torus*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Torus*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R1 " + Microsoft::VisualBasic::Strings::Format(((Torus*)currentShape)->Radius1()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R2 " + Microsoft::VisualBasic::Strings::Format(((Torus*)currentShape)->Radius2()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Circle3D");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("Cx " +  Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getX()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Cy " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getY()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("Cz " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->getCenter()->getZ()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("R " + Microsoft::VisualBasic::Strings::Format(((Circle*)currentShape)->Radius()/UnitDivide, MeasureFormat));
	    		this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Circle*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Circle*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Circle*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::ELLIPSE3D:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Ellipse3D");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("E " + Microsoft::VisualBasic::Strings::Format(((Conics3D*)currentShape)->eccentricity()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::PARABOLA3D:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Parabola3D");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("E " + Microsoft::VisualBasic::Strings::Format(((Conics3D*)currentShape)->eccentricity()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::HYPERBOLA3D:
				this->ShapeParameter_ArrayList->Add(currentShape->getId());
				this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
				this->ShapeParameter_ArrayList->Add("Hyperbola3D");
				this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
				this->ShapeParameter_ArrayList->Add("L " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_l()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("M " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_m()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("N " + Microsoft::VisualBasic::Strings::Format(Math::Acos(((Conics3D*)currentShape)->dir_n()) * 180/Math::PI, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("E " + Microsoft::VisualBasic::Strings::Format(((Conics3D*)currentShape)->eccentricity()/UnitDivide, MeasureFormat));
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				this->ShapeParameter_ArrayList->Add("");
				break;
			case RapidEnums::SHAPETYPE::TWOARC:
				{
					System::String^ Angle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((TwoArc*)currentShape)->Angle(), CMeasureMode, MeasureFormat, true);
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("TwoArc");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("S " + Angle);
					this->ShapeParameter_ArrayList->Add("R1 " + Microsoft::VisualBasic::Strings::Format(((TwoArc*)currentShape)->Radius1()/UnitDivide, MeasureFormat));
					this->ShapeParameter_ArrayList->Add("R2 " + Microsoft::VisualBasic::Strings::Format(((TwoArc*)currentShape)->Radius2()/UnitDivide, MeasureFormat));
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
				 	break;
				}
			case RapidEnums::SHAPETYPE::TWOLINE:
				{
					System::String^ Angle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((TwoLine*)currentShape)->Angle(), CMeasureMode, MeasureFormat, true);
					System::String^ LineAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(((TwoLine*)currentShape)->LineAngle(), CMeasureMode, MeasureFormat, true);
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("TwoLine");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("S " + Angle);
					this->ShapeParameter_ArrayList->Add("LA " + LineAngle);
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
				    break;
				}
			case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE:
				{
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("IntersectionShape");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}
				case RapidEnums::SHAPETYPE::DOTSCOUNT:
				{
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("DotsCount");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add(((DotsCount*)currentShape)->CircleCollection.size());
					this->ShapeParameter_ArrayList->Add(((DotsCount*)currentShape)->FrameArea);
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}
				case RapidEnums::SHAPETYPE::CAMPROFILE:
				{
					double CamParameter[4] = {0};
					((CamProfile*)currentShape)->GetCamProfileParam(CamParameter);
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("CamProfile");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("X " + Microsoft::VisualBasic::Strings::Format(CamParameter[0], MeasureFormat));
					this->ShapeParameter_ArrayList->Add("Y " + Microsoft::VisualBasic::Strings::Format(CamParameter[1], MeasureFormat));
					this->ShapeParameter_ArrayList->Add("R " + Microsoft::VisualBasic::Strings::Format(CamParameter[2], MeasureFormat));
					this->ShapeParameter_ArrayList->Add("A " + Microsoft::VisualBasic::Strings::Format(CamParameter[3] * 180 / M_PI, MeasureFormat));
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}
			case RapidEnums::SHAPETYPE::TRIANGULARSURFACE:
				{
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("TriangularSurface");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add(((TriangularSurface*)currentShape)->TriangleCount);
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}	
			case RapidEnums::SHAPETYPE::SPLINE:
				{
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("Spline");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}
			case RapidEnums::SHAPETYPE::AREA:
				{
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("Area");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}
				case RapidEnums::SHAPETYPE::CIRCLEINVOLUTE:
				{
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("CIRCLEINVOLUTE");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}
			    case RapidEnums::SHAPETYPE::AUTOFOCUSSURFACE:
				{
					this->ShapeParameter_ArrayList->Add(currentShape->getId());
					this->ShapeParameter_ArrayList->Add(gcnew System::String(currentShape->getModifiedName()));
					this->ShapeParameter_ArrayList->Add("Surface");
					this->ShapeParameter_ArrayList->Add(currentShape->IsHidden());
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
					this->ShapeParameter_ArrayList->Add("");
			    	break;
				}
			}
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0044", ex);
	}
}
