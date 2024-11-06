#include "StdAfx.h"
#include "DimThread.h"
#include "DimChildMeasurement.h"
#include "..\Engine\RCollectionBase.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Actions\AddThreadMeasureAction.h"
#include "..\Engine\ThreadCalcFunctions.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"

DimThread::DimThread(TCHAR* myname):DimBase(genName(myname))
{
	init();
}

DimThread::DimThread(bool simply):DimBase(false)
{
	init();
}

DimThread::~DimThread()
{
	try
	{

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0001", __FILE__, __FUNCSIG__); }
}

void DimThread::init()
{
	try
	{
		this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_THREAD;
		this->ThrdMajordia = 0; this->ThrdMinordia = 0; this->ThrdEffdiaG = 0;
		this->ThrdEffdiaM = 0; this->ThrdPitch = 0; this->ThrdAngle = 0;
		this->ThrdTopRad = 0; this->ThrdRootRad = 0; this->ThrdPitchDia = 0;
		this->PointerToAction = NULL;
		//Initialise the child measurements... To handle all the conditions.. 
		//i.e. we need all the properties for each measurements..
		std::string DimName = this->getModifiedName();
		DimThrdMajordia = AddChildMeasurement(DimName + "_MajorDia");
		DimThrdMinordia = AddChildMeasurement(DimName + "_MinorDia");
		DimThrdEffdiaM = AddChildMeasurement(DimName + "_EffDiaG");
		DimThrdEffdiaG = AddChildMeasurement(DimName + "_EffDiaM");
		DimThrdPitch = AddChildMeasurement(DimName + "_Pitch");
		DimThrdAngle = AddChildMeasurement(DimName + "_Angle"); DimThrdAngle->DistanceMeasurement(false);
		DimThrdTopRadius = AddChildMeasurement(DimName + "_TopRad");
		DimThrdRootRadius = AddChildMeasurement(DimName + "_RootRad");
		DimThrdPitchDia = AddChildMeasurement(DimName + "_PitchDia");
		IsValid(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0001", __FILE__, __FUNCSIG__); }
}

DimChildMeasurement* DimThread::AddChildMeasurement(std::string str)
{
	try
	{
		DimChildMeasurement* Cdim = new DimChildMeasurement(true);
		Cdim->MeasurementType = this->MeasurementType;
		Cdim->setModifiedName(str);
		Cdim->setOriginalName(RMATH2DOBJECT->StringToWString(str));
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMPT0002", __FILE__, __FUNCSIG__); return NULL; }
}

TCHAR* DimThread::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		threadcnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(threadcnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0002", __FILE__, __FUNCSIG__); return _T("NA"); }
} 


void DimThread::AddChildeMeasureToCollection(RCollectionBase& MColl)
{
	try
	{
		MColl.addItem(DimThrdMajordia, false); MColl.addItem(DimThrdMinordia, false);
		MColl.addItem(DimThrdEffdiaM, false); MColl.addItem(DimThrdEffdiaG, false);
		MColl.addItem(DimThrdPitch, false); MColl.addItem(DimThrdAngle, false);
		MColl.addItem(DimThrdTopRadius, false); MColl.addItem(DimThrdRootRadius, false);
		MColl.addItem(DimThrdPitchDia, false);

		DimThrdMajordia->ParentMeasureId(this->getId()); DimThrdMinordia->ParentMeasureId(this->getId()); DimThrdEffdiaM->ParentMeasureId(this->getId());
		DimThrdEffdiaG->ParentMeasureId(this->getId()); DimThrdPitch->ParentMeasureId(this->getId()); DimThrdAngle->ParentMeasureId(this->getId());
		DimThrdTopRadius->ParentMeasureId(this->getId()); DimThrdRootRadius->ParentMeasureId(this->getId()); DimThrdPitchDia->ParentMeasureId(this->getId());

		TChildMeasureIdCollection.push_back(DimThrdMajordia->getId()); TChildMeasureIdCollection.push_back(DimThrdMinordia->getId());
		TChildMeasureIdCollection.push_back(DimThrdEffdiaM->getId()); TChildMeasureIdCollection.push_back(DimThrdEffdiaG->getId());
		TChildMeasureIdCollection.push_back(DimThrdPitch->getId()); TChildMeasureIdCollection.push_back(DimThrdAngle->getId());
		TChildMeasureIdCollection.push_back(DimThrdTopRadius->getId()); TChildMeasureIdCollection.push_back(DimThrdRootRadius->getId());
		TChildMeasureIdCollection.push_back(DimThrdPitchDia->getId());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0002a", __FILE__, __FUNCSIG__); }
}

void DimThread::CalculateMeasurement()
{
	try
	{
		RCollectionBase* ShColl = &PointerToAction->ShapeCollection_Actions;
		RCollectionBase* MeasureColl = &PointerToAction->ThrdMeasureCollection;
		unsigned int ThreadCount = PointerToAction->ThreadCount();
		bool TopSurfaceStatus = PointerToAction->TopSurfaceFlat();
		bool RootSurfaceStatus = PointerToAction->RootSurfaceFlat();
		int ActId = PointerToAction->getId();
		map<int, SinglePoint> LineParam_Upper;
		map<int, SinglePoint> LineParam_Lower;
		map<int, SinglePoint> FarthestPoints;
		map<int, SinglePoint> NearestPoints;
		PointCollection UpperTop_Intersection, LowerTop_Intersection, UpperLow_Intersection, LowerLow_Intersection;
		Line Intersect_UpperTopLine(false), Intersect_LowerTopLine(false);
		Line D_upperline(false), D_lowerLine(false), Angle_BisectorLine(false);
		DimBase* Cdim = NULL;
		map<int, double> effective_diameter;

		double lineangle = 0, lineintercept = 0, intersectionpt[2] = {0}, intercept1 = 0;;
		double FullDia = 0, Pitch = 0, TAngle = 0, EffDia = 0, MajorDia = 0, MinorDia = 0, EffectDia = 0, TopRad = 0, RootRad = 0;
		//delete shapes which are not part of thread(line, arc)........
		RC_ITER Shpitem = ShColl->getList().end();
		while(ShColl->getList().size() != 0)
		{
			if(ShColl->getList().size() <= ThreadCount * 8 - 2)
				break;
			Shpitem--;
			BaseItem* CShape = (BaseItem*)((*Shpitem).second);
			ShColl->EraseItem(CShape, false);
			delete CShape;
		}
		//delete all measurement.....
		MeasureColl->deleteAll();

		//Line Parameters...
		for(unsigned int i = 0; i < ThreadCount * 2; i++)
		{
			Line* Csh = (Line*)ShColl->getList()[2 * i];
			SinglePoint Spt(Csh->Angle(), Csh->Intercept(), 0);
			LineParam_Upper[i] = Spt;

			Line* Csh1 = (Line*)ShColl->getList()[ThreadCount * 4 - 1 + 2 * i];
			SinglePoint Spt1(Csh1->Angle(), Csh1->Intercept(), 0);
			LineParam_Lower[i] = Spt1;
		}

		//Upper Top and Lower Top Intersections..
		for(unsigned int i = 0; i < ThreadCount; i++)
		{
			RMATH2DOBJECT->Line_lineintersection(LineParam_Upper[2 * i].X, LineParam_Upper[2 * i].Y, LineParam_Upper[2 * i + 1].X, LineParam_Upper[2 * i + 1].Y, &intersectionpt[0]);
			UpperTop_Intersection.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));

			RMATH2DOBJECT->Line_lineintersection(LineParam_Lower[2 * i].X, LineParam_Lower[2 * i].Y, LineParam_Lower[2 * i + 1].X, LineParam_Lower[2 * i + 1].Y, &intersectionpt[0]);
			LowerTop_Intersection.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));
		}

		//Top low and Lower low intersections..
		for(unsigned int i = 0; i < ThreadCount - 1; i++)
		{
			RMATH2DOBJECT->Line_lineintersection(LineParam_Upper[2 * i + 1].X, LineParam_Upper[2 * i + 1].Y, LineParam_Upper[2 * i + 2].X, LineParam_Upper[2 * i + 2].Y, &intersectionpt[0]);
			UpperLow_Intersection.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));

			RMATH2DOBJECT->Line_lineintersection(LineParam_Lower[2 * i + 1].X, LineParam_Lower[2 * i + 1].Y, LineParam_Lower[2 * i + 2].X, LineParam_Lower[2 * i + 2].Y, &intersectionpt[0]);
			LowerLow_Intersection.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));
		}

		//Top and Btm Lines..
		Intersect_UpperTopLine.AddPoints(&UpperTop_Intersection);
		Intersect_LowerTopLine.AddPoints(&LowerTop_Intersection);

		//Angle Bisectior Line..
		double mp[2] = {(UpperTop_Intersection.getList()[0]->X + LowerTop_Intersection.getList()[0]->X)/2, (UpperTop_Intersection.getList()[0]->Y + LowerTop_Intersection.getList()[0]->Y)/2};
		RMATH2DOBJECT->Angle_bisector(Intersect_UpperTopLine.Angle(), Intersect_UpperTopLine.Intercept(), Intersect_LowerTopLine.Angle(), Intersect_LowerTopLine.Intercept(), &mp[0], &lineangle, &lineintercept);
		Angle_BisectorLine.setLineParameters(lineangle, lineintercept);

		//Perpendicular to angle bisector line..
		double tempslope = lineangle + M_PI_2;
		RMATH2DOBJECT->Angle_FirstScndQuad(&tempslope);
		
		//Angle Calcualtion..
		for(unsigned int i = 0; i < ThreadCount; i++)
		{
			double angle;
			angle = LineParam_Upper[2 * i].X - LineParam_Upper[2 * i + 1].X;
			if(angle < 0) angle += M_PI;
			if(angle > M_PI_2) angle = M_PI - angle;
			TAngle += angle;

			angle = LineParam_Lower[2 * i].X - LineParam_Lower[2 * i + 1].X;
			if(angle < 0) angle += M_PI;
			if(angle > M_PI_2) angle = M_PI - angle;
			TAngle += angle;
		}
		TAngle = TAngle/(ThreadCount * 2);

		//Pitch Calculation..
		for(unsigned int i = 0; i < ThreadCount - 1; i++)
		{
			double Dist;
			Dist = RMATH2DOBJECT->Pt2Pt_distance(UpperTop_Intersection.getList()[i]->X, UpperTop_Intersection.getList()[i]->Y, UpperTop_Intersection.getList()[i + 1]->X, UpperTop_Intersection.getList()[i + 1]->Y);
			Pitch += Dist;
			Dist = RMATH2DOBJECT->Pt2Pt_distance(LowerTop_Intersection.getList()[i]->X, LowerTop_Intersection.getList()[i]->Y, LowerTop_Intersection.getList()[i + 1]->X, LowerTop_Intersection.getList()[i + 1]->Y);
			Pitch += Dist;
		}
		Pitch = Pitch/((ThreadCount - 1) * 2);
			
		if(TAngle == 0 || Pitch == 0)
			return;

		//Eff dia and Full Dia Calculation..
		for(unsigned int i = 0; i < ThreadCount; i++)
		{
			double Dist = RMATH2DOBJECT->Pt2Line_Dist(UpperTop_Intersection.getList()[i]->X, UpperTop_Intersection.getList()[i]->Y, Intersect_LowerTopLine.Angle(), Intersect_LowerTopLine.Intercept());
			FullDia += Dist;
			Dist = RMATH2DOBJECT->Pt2Line_Dist(LowerTop_Intersection.getList()[i]->X, LowerTop_Intersection.getList()[i]->Y, Intersect_UpperTopLine.Angle(), Intersect_UpperTopLine.Intercept());
			FullDia += Dist;
		}
		FullDia = FullDia/(ThreadCount * 2);
		EffDia = FullDia - (Pitch/(2 * tan(TAngle/2)));//Effective Dia	
		
		//Eff Dia calculation...
		double PerAngleBisect_Angle = tempslope;
		double rad1, rad2, height, alpha1, alpha2, beta1, beta2, diff, Effrad, EffectRad1 = 0, EffectRad2 = 0;
		Line* CLine;
		map<int, Line*> D_upperlines;
		
		int sign = 1;
		if (UpperTop_Intersection.getList()[0]->Y < UpperLow_Intersection.getList()[0]->Y)
			sign = -1;
		for (int i = 0; i < UpperTop_Intersection.Pointscount() - 1; i++)
		{
			rad2 = RMATH2DOBJECT->Pt2Line_Dist(UpperTop_Intersection.getList()[i]->X, UpperTop_Intersection.getList()[i]->Y, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept());
			rad1 = RMATH2DOBJECT->Pt2Line_Dist(UpperLow_Intersection.getList()[i]->X, UpperLow_Intersection.getList()[i]->Y, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept());
			height = rad2 - rad1;
			alpha1 = PerAngleBisect_Angle - LineParam_Upper[2 * i].X;
			beta1 = LineParam_Upper[2 * i + 1].X - PerAngleBisect_Angle;
			alpha2 = beta1;
			beta2 = PerAngleBisect_Angle - LineParam_Upper[2 * i + 2].X;
			diff = height / (1 + ((abs(tan(alpha1)) + abs(tan(beta1)))/(abs(tan(alpha2)) + abs(tan(beta2)))));
			Effrad = rad2 - diff;
			EffectRad1 += Effrad;
			CLine = new Line(); 
			PointCollection PtColl;
			intercept1 = lineintercept + sign * Effrad / abs(cos(lineangle));
			CLine->setLineParameters(lineangle, intercept1);
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, LineParam_Upper[2 * i].X, LineParam_Upper[2 * i].Y, &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1]));
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, LineParam_Upper[2 * i + 2].X, LineParam_Upper[2 * i + 2].Y, &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1]));
			CLine->AddPoints(&PtColl);
			CLine->setId(ShColl->getList().size());
			CLine->UcsId(ActId);
			ShColl->addItem(CLine, false);
			THREADCALCOBJECT->AddPointsToMainCollectionShape((ShapeWithList*)CLine, true);
			D_upperlines[i] = CLine;
		}

		map<int, Line*> D_lowerlines;
		for (int i = 0; i < LowerTop_Intersection.Pointscount() -1; i++)
		{
			rad2 = RMATH2DOBJECT->Pt2Line_Dist(LowerTop_Intersection.getList()[i]->X, LowerTop_Intersection.getList()[i]->Y, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept());
			rad1 = RMATH2DOBJECT->Pt2Line_Dist(LowerLow_Intersection.getList()[i]->X, LowerLow_Intersection.getList()[i]->Y, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept());
			height = rad2 - rad1;
			alpha1 = PerAngleBisect_Angle - LineParam_Lower[2*i].X;
			beta1 = LineParam_Lower[2*i + 1].X  - PerAngleBisect_Angle;
			alpha2 = beta1;
			beta2 = PerAngleBisect_Angle - LineParam_Lower[2*i + 2].X;
			diff = height / (1 + ((abs(tan(alpha1)) + abs(tan(beta1)))/(abs(tan(alpha2)) + abs(tan(beta2)))));
			Effrad = rad2 - diff;
			EffectRad2 += Effrad;
			CLine = new Line(); 
			PointCollection PtColl;
			intercept1 = lineintercept - sign * Effrad / abs(cos(lineangle));
			CLine->setLineParameters(lineangle, intercept1);
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, LineParam_Lower[2*i].X , LineParam_Lower[2*i].Y, &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1]));
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, LineParam_Lower[2*i + 2].X, LineParam_Lower[2*i + 2].Y, &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1]));
			CLine->AddPoints(&PtColl);
			CLine->setId(ShColl->getList().size());
			CLine->UcsId(ActId);
			ShColl->addItem(CLine, false);
			THREADCALCOBJECT->AddPointsToMainCollectionShape((ShapeWithList*)CLine, true);
			D_lowerlines[i] = CLine;
		}

		//loop thru each pair of effective diameter lines taken in a sequence, one each from upper and lower thread.
		Line* line1;
		Line* line2;
		unsigned int cnt = D_upperlines.size();
		if (D_lowerlines.size() < cnt)
			cnt = D_lowerlines.size();
		int m = 0;
		for (unsigned int i = 0; i < cnt; i++)
		{
			line1 = D_upperlines[i];
			line2 = D_lowerlines[i];
			//call function to get distance between the parallel lines (line1, line2) and store in Effective Diameter array.
			Cdim = HELPEROBJECT->CreateLinetoLine_Dist(line1, line2, false);
			Cdim->setId(MeasureColl->getList().size());
			MeasureColl->addItem(Cdim, false);
			effective_diameter[m] = Cdim->getDimension();
			m++;
		}
		getaverage(effective_diameter, &EffectDia);


	
		double TempTopRad = 0, TempRootRad = 0;
		
		//Farthest Point/Nearest Point Calculation..
		int FCnt = 0, NCnt = 0;
		for(unsigned int i = 0; i < ThreadCount; i++)
		{
			if(TopSurfaceStatus)
			{
				Line* MyLine = (Line*)ShColl->getList()[4 * i + 1];
				SinglePoint Spt(MyLine->getMidPoint()->getX(), MyLine->getMidPoint()->getY(), 0);
				FarthestPoints[FCnt++] = Spt;
			}
			else
			{
				Circle* Cshape = (Circle*)ShColl->getList()[4 * i + 1];
				RMATH2DOBJECT->Farthestpoint_line(Cshape->PointsList->Pointscount(), Cshape->pts, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept(), &intersectionpt[0]);
				SinglePoint Spt(intersectionpt[0], intersectionpt[1], 0);
				FarthestPoints[FCnt++] = Spt;
				TempTopRad += Cshape->Radius();
			}

			if(RootSurfaceStatus)
			{
				Line* MyLine = (Line*)ShColl->getList()[ThreadCount * 4  + 4 * i];
				SinglePoint Spt(MyLine->getMidPoint()->getX(), MyLine->getMidPoint()->getY(), 0);
				FarthestPoints[FCnt++] = Spt;
			}
			else
			{
				Circle* Cshape1 = (Circle*)ShColl->getList()[ThreadCount * 4  + 4 * i];
				RMATH2DOBJECT->Farthestpoint_line(Cshape1->PointsList->Pointscount(), Cshape1->pts, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept(), &intersectionpt[0]);
				SinglePoint Spt1(intersectionpt[0], intersectionpt[1], 0);
				FarthestPoints[FCnt++] = Spt1;
				TempTopRad += Cshape1->Radius();
			}
		}
		for(unsigned int i = 0; i < ThreadCount - 1; i++)
		{
			if(TopSurfaceStatus)
			{
				Line* MyLine = (Line*)ShColl->getList()[4 * i + 3];
				SinglePoint Spt(MyLine->getMidPoint()->getX(), MyLine->getMidPoint()->getY(), 0);
				NearestPoints[NCnt++] = Spt;
			}
			else
			{
				Circle* Cshape = (Circle*)ShColl->getList()[4 * i + 3];
				RMATH2DOBJECT->Nearestpoint_line(Cshape->PointsList->Pointscount(), Cshape->pts, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept(), &intersectionpt[0]);
				SinglePoint Spt(intersectionpt[0], intersectionpt[1], 0);
				NearestPoints[NCnt++] = Spt;
				TempRootRad += Cshape->Radius();
			}

			if(RootSurfaceStatus)
			{
				Line* MyLine = (Line*)ShColl->getList()[ThreadCount * 4  + 4 * i + 2];
				SinglePoint Spt(MyLine->getMidPoint()->getX(), MyLine->getMidPoint()->getY(), 0);
				NearestPoints[NCnt++] = Spt;
			}
			else
			{
				Circle* Cshape1 = (Circle*)ShColl->getList()[ThreadCount * 4  + 4 * i + 2];
				RMATH2DOBJECT->Nearestpoint_line(Cshape1->PointsList->Pointscount(), Cshape1->pts, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept(), &intersectionpt[0]);
				SinglePoint Spt1(intersectionpt[0], intersectionpt[1], 0);
				NearestPoints[NCnt++] = Spt1;
				TempRootRad += Cshape1->Radius();
			}
		}

		//Top and Root radius...
		TopRad = TempTopRad/ThreadCount;
		RootRad = TempRootRad/(ThreadCount - 1);


		//Major Dia..
		for(unsigned int i = 0; i < ThreadCount * 2; i++)
		{
			double Dist = RMATH2DOBJECT->Pt2Line_Dist(FarthestPoints[i].X, FarthestPoints[i].Y, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept());
			MajorDia += Dist;
		}
		MajorDia = MajorDia / ThreadCount;

		//Minor Dia...
		for(unsigned int i = 0; i < (ThreadCount - 1) * 2; i++)
		{
			double Dist = RMATH2DOBJECT->Pt2Line_Dist(NearestPoints[i].X, NearestPoints[i].Y, Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept());
			MinorDia += Dist;
		}
		MinorDia = MinorDia/(ThreadCount - 1);
		if(!_isnan(MajorDia)) this->ThrdMajordia = MajorDia;
		if(!_isnan(MinorDia)) this->ThrdMinordia = MinorDia;
		if(!_isnan(EffDia)) this->ThrdEffdiaG = EffDia;
		if(!_isnan(EffectDia)) this->ThrdEffdiaM = EffectDia;
		if(!_isnan(Pitch)) this->ThrdPitch = Pitch;
		if(!_isnan(TAngle)) this->ThrdAngle = TAngle;
		if(!_isnan(TopRad)) this->ThrdTopRad = TopRad;
		if(!_isnan(RootRad)) this->ThrdRootRad = RootRad;
		if(!_isnan(MajorDia) && !_isnan(Pitch)) this->ThrdPitchDia = MajorDia - Pitch * 0.64952;
		UpdateMeasurement();
		IsValid(true);

		

		
		//create angle bisector line for upper two Lines.........
		Line* CshLine1 = (Line*)ShColl->getList()[0];
	    RC_ITER item = ShColl->getList().end();
		item--;
		while(((Shape*)(*item).second)->getId() > ThreadCount * 8 - 3)
		{
			item--;
		}	
		Shape* CShape = (Shape*)(*item).second;
		Line* CshLine2 = (Line*)CShape;
		double intesectPt1[2] = {0}, intesectPt2[2] = {0};
		Line* myLine_angleBisector  = new Line();
		myLine_angleBisector->LineType = RapidEnums::LINETYPE::FINITELINE;
		myLine_angleBisector->setLineParameters(Angle_BisectorLine.Angle(), Angle_BisectorLine.Intercept());
		RMATH2DOBJECT->Line_lineintersection(lineangle, lineintercept, CshLine1->Angle(), CshLine1->Intercept(), &intesectPt1[0]);
		RMATH2DOBJECT->Line_lineintersection(lineangle, lineintercept, CshLine2->Angle(), CshLine2->Intercept(), &intesectPt2[0]);
		PointCollection PtColl1;
		PtColl1.Addpoint(new SinglePoint (intesectPt1[0], intesectPt1[1]));
		PtColl1.Addpoint(new SinglePoint (intesectPt2[0], intesectPt2[1]));
		((ShapeWithList*)myLine_angleBisector)->AddPoints(&PtColl1);
		myLine_angleBisector->IsValid(true); 
		myLine_angleBisector->setId(ShColl->getList().size());
		myLine_angleBisector->UcsId(ActId);
		ShColl->addItem(myLine_angleBisector, false);
		THREADCALCOBJECT->AddPointsToMainCollectionShape((ShapeWithList*)myLine_angleBisector, true);
		

		//Angle measurement.............
		for(unsigned int i = 0; i < ThreadCount * 2 - 1; i++)
		{
			Shape* Csh = (Shape*)ShColl->getList()[2 * i];
			Shape* Csh1 = (Shape*)ShColl->getList()[2 * i + 2];
		    Cdim = HELPEROBJECT->CreateAngleMeasurment(Csh, Csh1, false);
			Cdim->setId(MeasureColl->getList().size());
			MeasureColl->addItem(Cdim, false);
			
			Shape* Csh2 = (Shape*)ShColl->getList()[ThreadCount * 4 - 1 + 2 * i];
			Shape* Csh3 = (Shape*)ShColl->getList()[ThreadCount * 4 - 1 + 2 * i + 2];
			Cdim = HELPEROBJECT->CreateAngleMeasurment(Csh2, Csh3, false);
			Cdim->setId(MeasureColl->getList().size());
			MeasureColl->addItem(Cdim, false);
		}

		//pitch distance calculation for upper thread.......
		for (int i = 0; i < UpperTop_Intersection.Pointscount() - 1; i++)
		{
			double intPt1[2] = {UpperTop_Intersection.getList()[i]->X, UpperTop_Intersection.getList()[i]->Y};
			double intPt2[2] = {UpperTop_Intersection.getList()[i + 1]->X, UpperTop_Intersection.getList()[i + 1]->Y};
			showPitch_Measurement(intPt1, intPt2, myLine_angleBisector);
		}
			

		//pitch distance calculation for lower thread.......
		for (int i = 0; i < LowerTop_Intersection.Pointscount() - 1; i++)
		{
			double intPt1[2] = {LowerTop_Intersection.getList()[i]->X, LowerTop_Intersection.getList()[i]->Y};
			double intPt2[2] = {LowerTop_Intersection.getList()[i + 1]->X, LowerTop_Intersection.getList()[i + 1]->Y};
			showPitch_Measurement(intPt1, intPt2, myLine_angleBisector);
		}

		THREADCALCOBJECT->CurrentSelectedThreadAction = this->PointerToAction;
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0003", __FILE__, __FUNCSIG__); }
}

void DimThread::getaverage(map<int, double>& tmplist, double* ptr_average)
{
	*ptr_average = 0;
	int cnt = tmplist.size();
	for (int i = 0; i < cnt; i++)
	{
		*(ptr_average) += tmplist[i];
	}
	*(ptr_average) /= cnt;
}

void DimThread::showPitch_Measurement(double *intersectPt1, double *intersectPt2, Shape* Cline)
{
	try
	{
		RCollectionBase* ShpColl = &PointerToAction->ShapeCollection_Actions;
		RCollectionBase* MeasColl = &PointerToAction->ThrdMeasureCollection;
		DimBase* dim = NULL;
		double dist = RMATH2DOBJECT->LXPt2Pt_distance_angle(&intersectPt1[0], &intersectPt2[0], ((Line*)Cline)->Angle());
		double TmpAng = ((Line*)Cline)->Angle();
		//this is temporary fix we need to get another method for getting direction.....
		if(TmpAng > M_PI_2)
		{
			TmpAng -= M_PI;
			for(int i = 0; i < 10; i++)
			{
				if(TmpAng <= M_PI_2)
					break;
				TmpAng -= M_PI;
			}
		}
		else if(TmpAng < - M_PI_2)
		{
			TmpAng += M_PI;
			for(int i = 0; i < 10; i++)
			{
				if(TmpAng >= - M_PI_2)
					break;
				TmpAng += M_PI;
			}
		}
		double newintPt[2] = {intersectPt1[0] + dist * cos(TmpAng), intersectPt1[1] + dist * sin(TmpAng)};

		/*double newintPt[2] = {0};
		if(intersectPt2[0] - intersectPt1[0] > 0)
			newintPt[0] = intersectPt1[0] + dist * abs(cos(((Line*)Cline)->Angle()));
		else
			newintPt[0] = intersectPt1[0] - dist * abs(cos(((Line*)Cline)->Angle()));
	
		if(intersectPt2[1] - intersectPt1[1] > 0)
			newintPt[1] = intersectPt1[1] + dist * abs(sin(((Line*)Cline)->Angle()));
		else
			newintPt[1] = intersectPt1[1] - dist * abs(sin(((Line*)Cline)->Angle()));*/

		Line* myLine  = new Line();
		myLine->ShapeType = RapidEnums::SHAPETYPE::LINE;
		myLine->LineType = RapidEnums::LINETYPE::FINITELINE;
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(intersectPt1[0], intersectPt1[1], 0));
		PtColl.Addpoint(new SinglePoint(newintPt[0], newintPt[1], 0));
		myLine->AddPoints(&PtColl);		
		myLine->Normalshape(true);	
		myLine->IsValid(false);
		myLine->setId(ShpColl->getList().size());
		myLine->UcsId(PointerToAction->getId());
		ShpColl->addItem(myLine, false);
		RC_ITER item = ShpColl->getList().end();
		item--;
		Shape* shpe = (Shape*)(*item).second;
		dim = HELPEROBJECT->CreateShapeToShapeDim2D(shpe, shpe, (intersectPt1[0] + newintPt[0]) / 2, (intersectPt1[1] + newintPt[1]) / 2, 0);
		dim->addParent(shpe);
		shpe->addMChild(dim);
		dim->IsValid(true);
		dim->UpdateMeasurement();
		dim->setId(MeasColl->getList().size());
		MeasColl->addItem(dim, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0004", __FILE__, __FUNCSIG__); }
}

void DimThread::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
}

void DimThread::UpdateForParentChange(BaseItem* sender)
{
}

void DimThread::UpdateMeasurement()
{
	try
	{
		double d = 1;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			d = 25.4;
		DimThrdMajordia->ChildMeasureValue(ThrdMajordia/d);
		DimThrdMinordia->ChildMeasureValue(ThrdMinordia/d);
		DimThrdEffdiaM->ChildMeasureValue(ThrdEffdiaG/d);
		DimThrdEffdiaG->ChildMeasureValue(ThrdEffdiaM/d);
		DimThrdPitch->ChildMeasureValue(ThrdPitch/d);
		DimThrdTopRadius->ChildMeasureValue(ThrdTopRad/d);
		DimThrdRootRadius->ChildMeasureValue(ThrdRootRad/d);
		DimThrdAngle->ChildMeasureValue(ThrdAngle);
		DimThrdPitchDia->ChildMeasureValue(ThrdPitchDia/d);
		MAINDllOBJECT->dontUpdateGraphcis = true;
		if(PointerToAction != NULL)
		{
			RCollectionBase* MeasureColl = &PointerToAction->ThrdMeasureCollection;
			for(RC_ITER bi = MeasureColl->getList().begin(); bi != MeasureColl->getList().end(); bi++)
			{
				DimBase* dim = (DimBase*)(*bi).second;
				dim->UpdateMeasurement();
			}
		}
		this->notifyAll(ITEMSTATUS::DATACHANGED, this);	
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->Measurement_updated();
	}
	catch(...)
	{
		MAINDllOBJECT->dontUpdateGraphcis = false; 
		MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0003a", __FILE__, __FUNCSIG__); 
	}
}

DimBase* DimThread::Clone(int n)
{
	try
	{
		DimThread* Cdim = new DimThread();
		Cdim->CopyOriginalProperties(this);
		Cdim->DimThrdMajordia->CopyOriginalProperties(this->DimThrdMajordia);
		Cdim->DimThrdMinordia->CopyOriginalProperties(this->DimThrdMinordia);
		Cdim->DimThrdEffdiaM->CopyOriginalProperties(this->DimThrdEffdiaM);
		Cdim->DimThrdEffdiaG->CopyOriginalProperties(this->DimThrdEffdiaG);
		Cdim->DimThrdPitch->CopyOriginalProperties(this->DimThrdPitch);
		Cdim->DimThrdAngle->CopyOriginalProperties(this->DimThrdAngle);
		Cdim->DimThrdTopRadius->CopyOriginalProperties(this->DimThrdTopRadius);
		Cdim->DimThrdRootRadius->CopyOriginalProperties(this->DimThrdRootRadius);
		Cdim->DimThrdPitchDia->CopyOriginalProperties(this->DimThrdPitchDia);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0004", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimThread::CreateDummyCopy()
{
	try
	{
		DimThread* Cdim = new DimThread(false);
		Cdim->CopyOriginalProperties(this);

		//Temp fix to copy the parameters... Can be changed future!!
		//You can use a list of child measurements..! that will be good..generalised!
		Cdim->DimThrdMajordia->setId(this->DimThrdMajordia->getId());
		Cdim->DimThrdMinordia->setId(this->DimThrdMinordia->getId());
		Cdim->DimThrdEffdiaM->setId(this->DimThrdEffdiaM->getId());
		Cdim->DimThrdEffdiaG->setId(this->DimThrdEffdiaG->getId());
		Cdim->DimThrdPitch->setId(this->DimThrdPitch->getId());
		Cdim->DimThrdAngle->setId(this->DimThrdAngle->getId());
		Cdim->DimThrdTopRadius->setId(this->DimThrdTopRadius->getId());
		Cdim->DimThrdRootRadius->setId(this->DimThrdRootRadius->getId());
		Cdim->DimThrdPitchDia->setId(this->DimThrdPitchDia->getId());

		Cdim->DimThrdMajordia->CopyOriginalProperties(this->DimThrdMajordia);
		Cdim->DimThrdMinordia->CopyOriginalProperties(this->DimThrdMinordia);
		Cdim->DimThrdEffdiaM->CopyOriginalProperties(this->DimThrdEffdiaM);
		Cdim->DimThrdEffdiaG->CopyOriginalProperties(this->DimThrdEffdiaG);
		Cdim->DimThrdPitch->CopyOriginalProperties(this->DimThrdPitch);
		Cdim->DimThrdAngle->CopyOriginalProperties(this->DimThrdAngle);
		Cdim->DimThrdTopRadius->CopyOriginalProperties(this->DimThrdTopRadius);
		Cdim->DimThrdRootRadius->CopyOriginalProperties(this->DimThrdRootRadius);
		Cdim->DimThrdPitchDia->CopyOriginalProperties(this->DimThrdPitchDia);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0005", __FILE__, __FUNCSIG__); return NULL;}
}

void DimThread::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0006", __FILE__, __FUNCSIG__); }
}

void DimThread::reset()
{
	threadcnt = 0;
}
int DimThread::threadcnt = 0;

wostream& operator<<( wostream& os, DimThread& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os << "DimThread" <<endl;
		os << "DimThrdMajordia:value"<< endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdMajordia));
		os << "DimThrdMinordia:value"<< endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdMinordia));
		os << "DimThrdEffdiaM:value" << endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdEffdiaM));
		os << "DimThrdEffdiaG:value" << endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdEffdiaG));
		os << "DimThrdPitch:value" << endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdPitch));
		os << "DimThrdAngle:value" << endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdAngle));
		os << "DimThrdTopRadius:value" << endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdTopRadius));
		os << "DimThrdRootRadius:value" << endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdRootRadius));
		os << "DimThrdPitchDia:value" << endl<< (*static_cast<DimChildMeasurement*>(x.DimThrdPitchDia));
		os << "EndDimThread" <<endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, DimThread& x)
{
	try
	{
		is >> (*(DimBase*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"DimThread")
			{
			while(Tagname!=L"EndDimThread")
				{	
					std::wstring Linestr;				
					is >> Linestr;
					if(is.eof())
					{						
						MAINDllOBJECT->PPLoadSuccessful(false);
						break;
					}
					int ColonIndx = Linestr.find(':');
					if(ColonIndx==-1)
					{
						Tagname=Linestr;
					}
					else
					{
						std::wstring Tag = Linestr.substr(0, ColonIndx);
						std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname=Tag;
						if(Tagname==L"DimThrdMajordia")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdMajordia));
						}
						else if(Tagname==L"DimThrdMinordia")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdMinordia));
						}	
						else if(Tagname==L"DimThrdPitchDia")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdPitchDia));
						}	
						else if(Tagname==L"DimThrdEffdiaM")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdEffdiaM));
						}
						else if(Tagname==L"DimThrdEffdiaG")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdEffdiaG));
						}	
						else if(Tagname==L"DimThrdPitch")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdPitch));
						}	
						else if(Tagname==L"DimThrdAngle")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdAngle));
						}	
						else if(Tagname==L"DimThrdTopRadius")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdTopRadius));
						}	
						else if(Tagname==L"DimThrdRootRadius")
						{
							is >> (*static_cast<DimChildMeasurement*>(x.DimThrdRootRadius));
						}	
					 }
				}
				x.UpdateMeasurement();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0008", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimThread& x)
{
	try
	{
		double d;
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdMajordia));
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdMinordia));
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdEffdiaM));
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdEffdiaG));
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdPitch));
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdAngle));
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdTopRadius));
		is >> (*static_cast<DimChildMeasurement*>(x.DimThrdRootRadius));
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMTHREAD0008", __FILE__, __FUNCSIG__);  }
}