#pragma once

#include "..\MAINDll\Shapes\Shape.h"

namespace RWrapper
{
	public ref class RW_AutoProfileScanMeasure
	{
		Shape *ParentShape1, *ParentShape2;
		double Width;
		bool ArcBestFit, autoContinue;
		int NumberOfPts, SkipParam, MeasureCount;
		RW_AutoProfileScanMeasure();
		~RW_AutoProfileScanMeasure();
		static RW_AutoProfileScanMeasure^ AutoProfileScanInstance;
		void Init();
		
	public:	
		static bool Check_AutoProfileScan_Instance();
		static RW_AutoProfileScanMeasure^ MYINSTANCE();	
		void ClearAll();		
		void StartAutoProfileScan(double width, int BestFitNumberOfPts, int Jump, int MeasureCount, bool ArcBestFitType, bool _autoContinue);
		void NextprofileScan();
		void StartNextProfileScan();
	};
}

