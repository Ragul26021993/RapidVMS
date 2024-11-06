#pragma once
#include "..\MAINDll\Shapes\ShapeWithList.h"
namespace RWrapper
{
	public ref class RW_CT_SphereCallibration
	{
		RW_CT_SphereCallibration();
		~RW_CT_SphereCallibration();
		static RW_CT_SphereCallibration^ CallibrationInstance;
		
	public:
		value struct PathList
		{
			double PtX, PtY, PtZ;
			bool Path;
		};
		
		static bool Check_CallibrationInstance();
		static void Close_CallibrationWindow();		
		static RW_CT_SphereCallibration^ MYINSTANCE();

		delegate void UpdateCallibrationValuesEventHandler();
		event UpdateCallibrationValuesEventHandler^ UpdateCallibrationValues;

		double ScalingCoefficient;
		cli::array<double> ^NormalVector, ^Offset, ^Rlevel;
		void ClearAll();
		void StartSphereCallibration(cli::array<double> ^RValue, double radius);
		void Continue_Pause(bool ContinueFlag);
		void SendDrotoGetPoint();
		void SendDrotoNextPosition();
		void StopCallibration();
	private:
		PointCollection* CT_PtsCollection;
		cli::array<double> ^ReferencePoint;
		double SphereRadius;
		int LevelCount, Listcount, CommandSendCnt;
		System::Collections::Generic::List<PathList>^ ProbePathPointList;
		TargetReachedCallback CurrentTargetType;
		ShapeWithList *ParentShape1, *ParentShape2, *ParentShape3;

		void Init();
		void CreatePathList();
		void AddPointToSelectedSphere(double x, double y, double z);
		void CreateNextSphere();
	};
}

