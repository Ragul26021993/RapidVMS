#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "RW_CT_SphereCallibration.h"
#include "..\MAINDLL\Shapes\Sphere.h"
#include "..\MAINDLL\Shapes\Vector.h"

RWrapper::RW_CT_SphereCallibration::RW_CT_SphereCallibration()
{
	Init();
}

RWrapper::RW_CT_SphereCallibration::~RW_CT_SphereCallibration()
{
}

bool RWrapper::RW_CT_SphereCallibration::Check_CallibrationInstance()
{
	if(CallibrationInstance == nullptr)
		return false;
	else
		return true;
}

void RWrapper::RW_CT_SphereCallibration::Close_CallibrationWindow()
{
	try
	{
		if(CallibrationInstance == nullptr) return;
		delete CallibrationInstance;
		CallibrationInstance = nullptr;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration01", ex); }
}

RWrapper::RW_CT_SphereCallibration^ RWrapper::RW_CT_SphereCallibration::MYINSTANCE()
{
	if(CallibrationInstance == nullptr)
		CallibrationInstance = gcnew RW_CT_SphereCallibration();
	return CallibrationInstance;
}

void RWrapper::RW_CT_SphereCallibration::ClearAll()
{
	try
	{
		RWrapper::RW_DRO::MYINSTANCE()->ContourScan_Callibration = false;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration02", ex); }
}

void RWrapper::RW_CT_SphereCallibration::StartSphereCallibration(cli::array<double> ^RValue, double radius)
{
	try
	{
		RWrapper::RW_DRO::MYINSTANCE()->ContourScan_Callibration = true;
		for(int i = 0; i < 3; i++)
			this->Rlevel[i] = RValue[i];
		this->SphereRadius = radius;
		this->ReferencePoint[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
		this->ReferencePoint[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
		this->ReferencePoint[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
		CreatePathList();
		if(ProbePathPointList->Count < 1)
		{
			RWrapper::RW_DRO::MYINSTANCE()->ContourScan_Callibration = false;
			return;
		}
		this->CT_PtsCollection->deleteAll();
		ShapeWithList* Cshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(Cshape != NULL)
		{
			if(Cshape->ShapeType != RapidEnums::SHAPETYPE::SPHERE)
			{
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER);
			}
		}
		else
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER);
		}
		
		ParentShape1 = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();

		double target[4] = {ProbePathPointList[Listcount].PtX, ProbePathPointList[Listcount].PtY, ProbePathPointList[Listcount].PtZ, Rlevel[LevelCount]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(target, feedrate, TargetReachedCallback::CONTOURTRACER_CALLIBRATION_PATH);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration03", ex); }
}

void RWrapper::RW_CT_SphereCallibration::Init()
{
	try
	{
		 this->Rlevel = gcnew cli::array<double>(3);
		 this->ReferencePoint = gcnew cli::array<double>(3);
		 this->NormalVector = gcnew cli::array<double>(3);
		 this->Offset = gcnew cli::array<double>(3);
		 this->SphereRadius = 10;
		 this->LevelCount = 0; this->Listcount = 0; this->CommandSendCnt = 0;
		 this->ProbePathPointList = gcnew  System::Collections::Generic::List<PathList>();
		 this->CT_PtsCollection = new PointCollection();
		 this->ParentShape1 = NULL;  this->ParentShape2 = NULL;  this->ParentShape3 = NULL; 
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration04", ex); }
}

void RWrapper::RW_CT_SphereCallibration::CreatePathList()
{
	try
	{
		ProbePathPointList->Clear();
		this->Listcount = 0;
		double RadMulFactor[2] = {System::Math::Pow(5.0/9, 0.5) * SphereRadius, System::Math::Pow(7.0/16.0, 0.5) * SphereRadius};
		double Zfactor[2] = {SphereRadius / 2, SphereRadius * 3 / 4};
		
		PathList Tmp_Path_LstEntity;
		PathList Tmp_Point_LstEntity;

		Tmp_Path_LstEntity.PtX = ReferencePoint[0]; Tmp_Path_LstEntity.PtY = ReferencePoint[1]; Tmp_Path_LstEntity.PtZ = ReferencePoint[2] + Rlevel[LevelCount];
		Tmp_Path_LstEntity.Path = true;
		ProbePathPointList->Add(Tmp_Path_LstEntity);
		Tmp_Point_LstEntity.PtX = ReferencePoint[0]; Tmp_Point_LstEntity.PtY = ReferencePoint[1]; Tmp_Point_LstEntity.PtZ = ReferencePoint[2] - Rlevel[LevelCount];
		Tmp_Point_LstEntity.Path = false;
		ProbePathPointList->Add(Tmp_Point_LstEntity);
		
		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				PathList Path_LstEntity;
				PathList Point_LstEntity;
				if(j == 0)
				{
					Path_LstEntity.PtX = ReferencePoint[0] + RadMulFactor[i]; Path_LstEntity.PtY = ReferencePoint[1]; Path_LstEntity.PtZ = ReferencePoint[2] + Rlevel[LevelCount];
					Path_LstEntity.Path = true;
					ProbePathPointList->Add(Path_LstEntity);
					Point_LstEntity.PtX = ReferencePoint[0] + RadMulFactor[i]; Point_LstEntity.PtY = ReferencePoint[1]; Point_LstEntity.PtZ = ReferencePoint[2] - Zfactor[i];
					Point_LstEntity.Path = false;
					ProbePathPointList->Add(Point_LstEntity);
				}
				else if(j == 1)
				{
					Path_LstEntity.PtX = ReferencePoint[0]; Path_LstEntity.PtY = ReferencePoint[1] + RadMulFactor[i]; Path_LstEntity.PtZ = ReferencePoint[2] + Rlevel[LevelCount];
					Path_LstEntity.Path = true;
					ProbePathPointList->Add(Path_LstEntity);
					Point_LstEntity.PtX = ReferencePoint[0]; Point_LstEntity.PtY = ReferencePoint[1] + RadMulFactor[i]; Point_LstEntity.PtZ = ReferencePoint[2] - Zfactor[i];
					Point_LstEntity.Path = false;
					ProbePathPointList->Add(Point_LstEntity);
				}
				else if(j == 2)
				{
					Path_LstEntity.PtX = ReferencePoint[0] - RadMulFactor[i]; Path_LstEntity.PtY = ReferencePoint[1]; Path_LstEntity.PtZ = ReferencePoint[2] + Rlevel[LevelCount];
					Path_LstEntity.Path = true;
					ProbePathPointList->Add(Path_LstEntity);
					Point_LstEntity.PtX = ReferencePoint[0] - RadMulFactor[i]; Point_LstEntity.PtY = ReferencePoint[1]; Point_LstEntity.PtZ = ReferencePoint[2] - Zfactor[i];
					Point_LstEntity.Path = false;
					ProbePathPointList->Add(Point_LstEntity);
				}
				else if(j == 3)
				{
					Path_LstEntity.PtX = ReferencePoint[0]; Path_LstEntity.PtY = ReferencePoint[1] - RadMulFactor[i]; Path_LstEntity.PtZ = ReferencePoint[2] + Rlevel[LevelCount];
					Path_LstEntity.Path = true;
					ProbePathPointList->Add(Path_LstEntity);
					Point_LstEntity.PtX = ReferencePoint[0]; Point_LstEntity.PtY = ReferencePoint[1] - RadMulFactor[i]; Point_LstEntity.PtZ = ReferencePoint[2] - Zfactor[i];
					Point_LstEntity.Path = false;
					ProbePathPointList->Add(Point_LstEntity);
				}
			}
		}
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration05", ex); }
}

void RWrapper::RW_CT_SphereCallibration::Continue_Pause(bool ContinueFlag)
{
	try
	{
		if(!ContinueFlag)
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration06", ex); }
}

void RWrapper::RW_CT_SphereCallibration::SendDrotoGetPoint()
{
	try
	{
		this->Listcount++;
		DROInstance->Check_RaxisTargetReached = true;
		double target[4] = {ProbePathPointList[Listcount].PtX, ProbePathPointList[Listcount].PtY, ProbePathPointList[Listcount].PtZ, Rlevel[LevelCount]};
		double feedrate[4] = {1, 1, 1, 1};
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(target, feedrate, TargetReachedCallback::CONTOURTRACER_CALLIBRATION_POINT);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration04", ex); }
}

void RWrapper::RW_CT_SphereCallibration::SendDrotoNextPosition()
{
	try
	{
		if(this->Listcount == ProbePathPointList->Count - 1)
		{
			ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
				CShape->AddPoints(CT_PtsCollection);			
			this->CT_PtsCollection->deleteAll();
			this->LevelCount++;
			CreateNextSphere();
		}
		else
		{
			if(DROInstance->Check_RaxisTargetReached)
			{
				double target[4] = {ProbePathPointList[Listcount].PtX, ProbePathPointList[Listcount].PtY, ProbePathPointList[Listcount].PtZ - Rlevel[LevelCount], Rlevel[LevelCount]};
				double feedrate[4] = {0.2, 0.2, 0.2, 0.2};
				if(CommandSendCnt >= 1)
					target[2] = target[2] - CommandSendCnt * Rlevel[LevelCount] * 1.5;
				if(CommandSendCnt > 3)
				{
					RWrapper::RW_DRO::MYINSTANCE()->ContourScan_Callibration = false;
					return;
				}
				this->CommandSendCnt++;
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(target, feedrate, TargetReachedCallback::CONTOURTRACER_CALLIBRATION_POINT);
			}
			else
			{
				this->CommandSendCnt = 0;
				AddPointToSelectedSphere(DROInstance->CTDroCoordinate[0], DROInstance->CTDroCoordinate[1], DROInstance->CTDroCoordinate[2]);
				this->Listcount++;
				double target[4] = {ProbePathPointList[Listcount].PtX, ProbePathPointList[Listcount].PtY, ProbePathPointList[Listcount].PtZ, Rlevel[LevelCount]};
				double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(target, feedrate, TargetReachedCallback::CONTOURTRACER_CALLIBRATION_PATH);
			}
		}
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CT_SphereCallibration07", ex); }
}

void RWrapper::RW_CT_SphereCallibration::AddPointToSelectedSphere(double x, double y, double z)
{
	try
	{		
		if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			this->CT_PtsCollection->Addpoint(new SinglePoint(x, y, z));
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			this->CT_PtsCollection->Addpoint(new SinglePoint(x, z, y));
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			this->CT_PtsCollection->Addpoint(new SinglePoint(y, z, x));			
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			this->CT_PtsCollection->Addpoint(new SinglePoint(x, -z, y));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			this->CT_PtsCollection->Addpoint(new SinglePoint(y, -z, x));
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0008", ex); }
}

void RWrapper::RW_CT_SphereCallibration::CreateNextSphere()
{
	try
	{		
		if(LevelCount < 3)
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER);
			if(LevelCount == 1)
				ParentShape2 = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			else if(LevelCount == 2)
				ParentShape3 = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			CreatePathList();
		}
		else
		{
			if(ParentShape1 != NULL && ParentShape2 != NULL && ParentShape3 != NULL)
			{
				double CircleParam[7] = {0}, Point1[3] = {((Sphere*)ParentShape1)->getCenter()->getX(), ((Sphere*)ParentShape1)->getCenter()->getY(), ((Sphere*)ParentShape1)->getCenter()->getZ()}, 
					Point2[3] = {((Sphere*)ParentShape2)->getCenter()->getX(), ((Sphere*)ParentShape2)->getCenter()->getY(), ((Sphere*)ParentShape2)->getCenter()->getZ()},
					Point3[3] = {((Sphere*)ParentShape3)->getCenter()->getX(), ((Sphere*)ParentShape3)->getCenter()->getY(), ((Sphere*)ParentShape3)->getCenter()->getZ()};
				double CirclePts[9] = {Point1[0], Point1[1], Point1[2], Point2[0], Point2[1], Point2[2], Point3[0], Point3[1], Point3[2]};
				if(RMATH3DOBJECT->Circle_3Pt_3D(CirclePts, CircleParam))
				{
					double ang1 = RMATH2DOBJECT->ray_angle(CircleParam, Point1);
					double ang2 = RMATH2DOBJECT->ray_angle(CircleParam, Point2);
					double Angle = abs(ang1 - ang2);
				    ScalingCoefficient = (Rlevel[1] - Rlevel[0]) / Angle;
					if(CircleParam[4] < 0)
					{
						NormalVector[0] = CircleParam[3] * (-1); NormalVector[1] = CircleParam[4] * (-1); NormalVector[2] = CircleParam[5] * (-1);
					}
					else
					{
						NormalVector[0] = CircleParam[3]; NormalVector[1] = CircleParam[4]; NormalVector[2] = CircleParam[5];
					}
					Offset[0] = Point1[0] - CircleParam[0]; Offset[1] =  Point1[1] - CircleParam[1]; Offset[2] = Point1[2] - CircleParam[2];
					RWrapper::RW_DRO::MYINSTANCE()->ContourScan_Callibration = false;
					UpdateCallibrationValues::raise();
				}
				else
				{
					//message please take proper intermediate and target position
					RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus("RW_CircularInterPolation01", RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, false, "");
					return;
				}
			}
			else
			{

			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0009", ex); }
}

void RWrapper::RW_CT_SphereCallibration::StopCallibration()
{
	try
	{		
		RWrapper::RW_DRO::MYINSTANCE()->ContourScan_Callibration = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_CirInt0010", ex); }
}