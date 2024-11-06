#include "StdAfx.h"
#include "TriangulationCalc.h"
#include "RCadApp.h"
#include <process.h>
TriangulationCalc* Myinstance;
//constructor..
TriangulationCalc::TriangulationCalc(bool SplitCalcintothread, int PointStep)
{
	NumberOfThreads = 0;
	ThreadFinishedCount = 0;
	MinimumPointCount = PointStep;
	RunOnSeperateThread = SplitCalcintothread;
	Myinstance = this;
	MinX = 0; MinY = 0; MinZ = 0; MaxX = 0; MaxY = 0; MaxZ = 0;
	NoofCellsX = 0; NoofCellsY = 0; NoofCellsZ = 0;
}

//destructor.. Delete the collection of collection of collections containing objects containing collection of object.!
TriangulationCalc::~TriangulationCalc()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0001", __FILE__, __FUNCSIG__); }
}

//Clear memory..
void TriangulationCalc::ClearAllMemory()
{
	try
	{
		TempPointCollection.deleteAll();
		while(AllPtCubeCollection.size() != 0)
		{
			std::list<std::list<std::list<PtCube*>*>*>::iterator ListIter = AllPtCubeCollection.begin();
			std::list<std::list<PtCube*>*>* XPtCubeColl = (*ListIter);
			while(XPtCubeColl->size() != 0)
			{
				std::list<std::list<PtCube*>*>::iterator ListIterX = XPtCubeColl->begin();
				std::list<PtCube*>* ZPtCubeColl = (*ListIterX);
				while(ZPtCubeColl->size() != 0)
				{
					std::list<PtCube*>::iterator ListIterZ = ZPtCubeColl->begin();
					PtCube* Zptcube = (*ListIterZ);	
					ZPtCubeColl->remove(Zptcube);
					Zptcube->CPointColl.deleteAll();
					delete Zptcube;					
				}
				ZPtCubeColl->clear();
				XPtCubeColl->remove(ZPtCubeColl);
			}
			XPtCubeColl->clear();
			AllPtCubeCollection.remove(XPtCubeColl);
		}
		AllPtCubeCollection.clear();
	

		int tempCnt = 0;
		while(AllPointsCollectionColl.size() != 0)
		{
			map<int, PointCollection*>::iterator i = AllPointsCollectionColl.begin();
			PointCollection* PtC = (*i).second;
			AllPointsCollectionColl.erase(tempCnt); tempCnt++;
			delete PtC;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0001", __FILE__, __FUNCSIG__); }
}

int TriangulationCalc::Triangulate3DPointCollection(PointCollection& PointColl, double** TriangleCollection, double tolerance)
{
	try
	{
		SetPointCollection(PointColl);
		if((MaxZ -MinZ) < tolerance)
		{
			MinZ = MinZ - tolerance/3;
			MaxZ = MaxZ + tolerance/3;
		}
		double NcX = (MaxX - MinX) / tolerance, NcY = (MaxY - MinY) / tolerance, NcZ = (MaxZ - MinZ) / tolerance;
		MinX = MinX - tolerance/2; MinY = MinY - tolerance/2;
		NoofCellsX = ceil(NcX); NoofCellsY = ceil(NcY); NoofCellsZ = ceil(NcZ);
		//Calculate all cubes
		CalculateAllCubes(tolerance);
		//check points
		CheckPointInsideEachCube();
		if(RunOnSeperateThread)
		{
			bool waitflag = true;
			//wait..
			while(ThreadFinishedCount != NumberOfThreads)
			{
				waitflag = true;
			}
		}
		//Triangulate points collection
		TriangulatePoints();

		ClearAllMemory();

		//convert to double pointer..
		int PtCnt = TrainglePointCollection.Pointscount(), PtCntCnter = 0;
		int NoofTriangulates = PtCnt/3;
		*TriangleCollection = (double*)calloc(PtCnt * 3, sizeof(double));
		for(PC_ITER SpItem = TrainglePointCollection.getList().begin(); SpItem != TrainglePointCollection.getList().end(); SpItem++)
		{
			SinglePoint* Spt = (*SpItem).second;
			(*TriangleCollection)[PtCntCnter++] = Spt->X;
			(*TriangleCollection)[PtCntCnter++] = Spt->Y;
			(*TriangleCollection)[PtCntCnter++] = Spt->Z;
		}
		TrainglePointCollection.deleteAll();
		return NoofTriangulates;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0002", __FILE__, __FUNCSIG__); }
}


//Add/ Set point Collection
void TriangulationCalc::SetPointCollection(PointCollection& PointColl)
{
	try
	{
		//Calculate bounds...and create a copy of point collections..
		int PointsCnt = PointColl.Pointscount(), PtCnter = 0;
		double* PointsXColl = (double*)calloc(PointsCnt, sizeof(double));
		double* PointsYColl = (double*)calloc(PointsCnt, sizeof(double));
		double* PointsZColl = (double*)calloc(PointsCnt, sizeof(double));
		if(RunOnSeperateThread) //Make points collection into clusters..
		{
			double ptcnt = PointsCnt, mincnt = MinimumPointCount;
			double NoofColl = ptcnt / mincnt; 
			NumberOfThreads = ceil(NoofColl);
			int CollCount = 0, PointsCounter = 0;
			PointCollection* TempPtColl;
			for(PC_ITER SpItem = PointColl.getList().begin(); SpItem != PointColl.getList().end(); SpItem++)
			{
				if(PointsCounter == 0)
				{
					TempPtColl = new PointCollection();
					AllPointsCollectionColl[CollCount] = TempPtColl;
					CollCount++;
				}
				SinglePoint* Spt = (*SpItem).second;
				TempPtColl->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
				PointsXColl[PtCnter] = Spt->X; PointsYColl[PtCnter] = Spt->Y; PointsZColl[PtCnter] = Spt->Z; PtCnter++;
				PointsCounter++;
				if(PointsCounter == MinimumPointCount) PointsCounter = 0;
			}
		}
		else // One Point collection..
		{
			for(PC_ITER SpItem = PointColl.getList().begin(); SpItem != PointColl.getList().end(); SpItem++)
			{
				SinglePoint* Spt = (*SpItem).second;
				this->TempPointCollection.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
				PointsXColl[PtCnter] = Spt->X; PointsYColl[PtCnter] = Spt->Y; PointsZColl[PtCnter] = Spt->Z; PtCnter++;
			}
		}
		MinX = RMATH2DOBJECT->minimum(PointsXColl, PointsCnt); MaxX = RMATH2DOBJECT->maximum(PointsXColl, PointsCnt);
		MinY = RMATH2DOBJECT->minimum(PointsYColl, PointsCnt); MaxY = RMATH2DOBJECT->maximum(PointsYColl, PointsCnt);
		MinZ = RMATH2DOBJECT->minimum(PointsZColl, PointsCnt); MaxZ = RMATH2DOBJECT->maximum(PointsZColl, PointsCnt);
		free(PointsXColl); free(PointsYColl); free(PointsZColl);
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0003", __FILE__, __FUNCSIG__); }
}

//Calculate all rectangules that isin bounded region.
void TriangulationCalc::CalculateAllCubes(double tolerance)
{
	try
	{
		for(int y = 0; y < NoofCellsY; y++)
		{
				std::list<std::list<PtCube*>*>* XPtCubeColl = new std::list<std::list<PtCube*>*>;
				for(int x = 0; x < NoofCellsX; x++)
				{
					std::list<PtCube*>* ZPtCubeColl = new std::list<PtCube*>;
					for(int z = 0; z < NoofCellsZ; z++)
					{
						PtCube* newptCb = new PtCube();
						newptCb->LeftBtm.SetValues(MinX + x * tolerance, MinY + y * tolerance, MinZ + z * tolerance);
						newptCb->RightTop.SetValues(MinX + (x + 1) * tolerance, MinY + (y + 1) * tolerance, MinZ + (z + 1) * tolerance);
						ZPtCubeColl->push_back(newptCb);
					}
					XPtCubeColl->push_back(ZPtCubeColl);
				}
				AllPtCubeCollection.push_back(XPtCubeColl);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0004", __FILE__, __FUNCSIG__); }
}

//Check points inside each cube..
void TriangulationCalc::CheckPointInsideEachCube()
{
	try
	{
		if(RunOnSeperateThread)
		{
			for(int i = 0; i < NumberOfThreads; i++)
			{
				PointCollection* TempPtColl = AllPointsCollectionColl[i];
				_beginthread(&CheckPointInCube_Thread, 0, (void*)(i));
			}
		}
		else
		{
			for each(std::list<std::list<PtCube*>*>* XptCubeColl in AllPtCubeCollection)
			{
				for each(std::list<PtCube*>* ZptCube in *XptCubeColl)
				{
					for each(PtCube* CZCube in *ZptCube)
					{					
						std::list<int> PtCollId;
						for(PC_ITER SpItem = TempPointCollection.getList().begin(); SpItem != TempPointCollection.getList().end(); SpItem++)
						{
							SinglePoint* Spt = (*SpItem).second;
							if((Spt->X >= CZCube->LeftBtm.X && Spt->X <= CZCube->RightTop.X) && (Spt->Y >= CZCube->LeftBtm.Y && Spt->Y <= CZCube->RightTop.Y) && (Spt->Z >= CZCube->LeftBtm.Z && Spt->Z <= CZCube->RightTop.Z))
							{
								CZCube->CPointColl.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
								PtCollId.push_back(Spt->PtID);				
							}
						}	
						for each(int Id in PtCollId)
							TempPointCollection.deletePoint(Id);
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0005", __FILE__, __FUNCSIG__); }
}

//Run point on seperate thread..
void CheckPointInCube_Thread(void* anything)
{
	try
	{
		int Cnt = (int)anything;
		std::list<std::list<std::list<PtCube*>*>*> CurrentPtCubeColl = Myinstance->AllPtCubeCollection;
		PointCollection* CurrentPointColl = Myinstance->AllPointsCollectionColl[Cnt];
		for each(std::list<std::list<PtCube*>*>* XptCubeColl in CurrentPtCubeColl)
		{
			for each(std::list<PtCube*>* ZptCube in *XptCubeColl)
			{
				for each(PtCube* CZCube in *ZptCube)
				{
					std::list<int> PtCollId;
					for(PC_ITER SpItem = CurrentPointColl->getList().begin(); SpItem != CurrentPointColl->getList().end(); SpItem++)
					{
						SinglePoint* Spt = (*SpItem).second;
						if(RMATH2DOBJECT->PtisinCube(Spt->X, Spt->Y, Spt->Z, CZCube->LeftBtm.X, CZCube->LeftBtm.Y, CZCube->LeftBtm.Z, CZCube->RightTop.X, CZCube->RightTop.Y, CZCube->RightTop.Z))
						{
							CZCube->CPointColl.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
							PtCollId.push_back(Spt->PtID);
						}
					}
					for each(int Id in PtCollId)
						CurrentPointColl->deletePoint(Id);
				}
			}
		}
		CurrentPointColl->deleteAll();
		Myinstance->ThreadFinishedCount++;
		_endthread();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0005", __FILE__, __FUNCSIG__); }
}

//Create mesh..If there are points in adjacent cubes then join it.. that is it..easy!

//void TriangulationCalc::TriangulatePoints()
//{
//	try
//	{
//		std::list<std::list<std::list<PtCube*>*>*>::iterator ListIter1 = AllPtCubeCollection.begin();
//		std::list<std::list<std::list<PtCube*>*>*>::iterator ListIter2 = AllPtCubeCollection.begin();
//		if(ListIter2 != AllPtCubeCollection.end())
//		{
//			ListIter2++;
//			for(int y = 0; y < NoofCellsY - 1; y++)
//			{
//				std::list<std::list<PtCube*>*>* XPtCubeColl1 = (*ListIter1);
//				std::list<std::list<PtCube*>*>* XPtCubeColl2 = (*ListIter2);
//				std::list<std::list<PtCube*>*>::iterator ListIterX1 = XPtCubeColl1->begin();
//				std::list<std::list<PtCube*>*>::iterator ListIterX2 = XPtCubeColl2->begin();
//				for(int x = 0; x < NoofCellsX - 1; x++)
//				{
//					std::list<PtCube*>* ZPtCubeColl11 = (*ListIterX1++); std::list<PtCube*>* ZPtCubeColl21 = (*ListIterX2++);
//					std::list<PtCube*>* ZPtCubeColl12 = (*ListIterX1); std::list<PtCube*>* ZPtCubeColl22 = (*ListIterX2);
//					std::list<PtCube*>::iterator ZPtCube11 = ZPtCubeColl11->begin();
//					std::list<PtCube*>::iterator ZPtCube21 = ZPtCubeColl21->begin();
//					std::list<PtCube*>::iterator ZPtCube12 = ZPtCubeColl12->begin();
//					std::list<PtCube*>::iterator ZPtCube22 = ZPtCubeColl22->begin();
//					std::list<PtCube*> CubeColl;
//					std::list<Pt_Present> CubeCollWithPt;
//					for(int z = 0; z <NoofCellsZ - 1; z++)
//					{
//						CubeColl.clear();
//						CubeCollWithPt.clear();
//						CubeColl.push_back(*ZPtCube11++); CubeColl.push_back(*ZPtCube11);
//						CubeColl.push_back(*ZPtCube21++); CubeColl.push_back(*ZPtCube21);
//						CubeColl.push_back(*ZPtCube12++); CubeColl.push_back(*ZPtCube12);
//						CubeColl.push_back(*ZPtCube22++); CubeColl.push_back(*ZPtCube22);
//						int StructId = 0;
//						for each(PtCube* PointCube in CubeColl)
//						{
//							if(PointCube->CPointColl.Pointscount() > 0)
//							{
//								Pt_Present CubeWithPt;
//								CubeWithPt.Id = StructId++;
//								SinglePoint* Spt = PointCube->CPointColl.getList()[0];
//								CubeWithPt.Point_Value.SetValues(Spt->X, Spt->Y, Spt->Z);
//								CubeCollWithPt.push_back(CubeWithPt);
//							}
//						}
//						if(CubeCollWithPt.size() < 3) continue;
//						for(std::list<Pt_Present>::iterator itr = CubeCollWithPt.begin(); itr != CubeCollWithPt.end(); itr++)
//						{
//							Pt_Present CubeWithPt = (*itr);
//							for(std::list<Pt_Present>::iterator itr1 = CubeCollWithPt.begin(); itr1 != CubeCollWithPt.end(); itr1++)
//							{
//								Pt_Present CubeWithPt1 = (*itr1);
//								if(CubeWithPt.Id <= CubeWithPt1.Id) continue;
//								for(std::list<Pt_Present>::iterator itr2 = CubeCollWithPt.begin(); itr2 != CubeCollWithPt.end(); itr2++)
//								{
//									Pt_Present CubeWithPt2 = (*itr2);
//									if(CubeWithPt2.Id >= CubeWithPt1.Id) continue;
//									AddOneTrianglePoints(&CubeWithPt.Point_Value, &CubeWithPt1.Point_Value, &CubeWithPt2.Point_Value);
//								}
//							}
//						}
//					}
//				}
//				if(ListIter2 != AllPtCubeCollection.end())
//	          	{
//				    ListIter1++; ListIter2++;
//				}
//		   }
//		}
//	}
//	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0006", __FILE__, __FUNCSIG__); }
//}


void TriangulationCalc::TriangulatePoints()
{
	try
	{
		std::list<std::list<std::list<PtCube*>*>*>::iterator ListIter1 = AllPtCubeCollection.begin();
		std::list<std::list<std::list<PtCube*>*>*>::iterator ListIter2 = AllPtCubeCollection.begin();
		if(ListIter2 != AllPtCubeCollection.end())
		{
			ListIter2++;
			for(int y = 0; y < NoofCellsY - 1; y++)
			{
				std::list<std::list<PtCube*>*>* XPtCubeColl1 = (*ListIter1);
				std::list<std::list<PtCube*>*>* XPtCubeColl2 = (*ListIter2);
				std::list<std::list<PtCube*>*>::iterator ListIterX1 = XPtCubeColl1->begin();
				std::list<std::list<PtCube*>*>::iterator ListIterX2 = XPtCubeColl2->begin();
				for(int x = 0; x < NoofCellsX - 1; x++)
				{
					std::list<PtCube*>* ZPtCubeColl11 = (*ListIterX1++); std::list<PtCube*>* ZPtCubeColl21 = (*ListIterX2++);
					std::list<PtCube*>* ZPtCubeColl12 = (*ListIterX1); std::list<PtCube*>* ZPtCubeColl22 = (*ListIterX2);
					std::list<PtCube*>::iterator ZPtCube11 = ZPtCubeColl11->begin();
					std::list<PtCube*>::iterator ZPtCube21 = ZPtCubeColl21->begin();
					std::list<PtCube*>::iterator ZPtCube12 = ZPtCubeColl12->begin();
					std::list<PtCube*>::iterator ZPtCube22 = ZPtCubeColl22->begin();
					if(NoofCellsZ > 1)
					{
						for(int z = 0; z < NoofCellsZ - 1; z++)
						{
							PtCube* ZPtCube110 = *ZPtCube11++; PtCube* ZPtCube111 = *ZPtCube11;
							PtCube* ZPtCube210 = *ZPtCube21++; PtCube* ZPtCube211 = *ZPtCube21;
							PtCube* ZPtCube120 = *ZPtCube12++; PtCube* ZPtCube121 = *ZPtCube12;
							PtCube* ZPtCube220 = *ZPtCube22++; PtCube* ZPtCube221 = *ZPtCube22;
							//CalculateTriangle(ZPtCube110, ZPtCube120, ZPtCube210, ZPtCube220); //Bottom Surface
							CalculateTriangle(ZPtCube110, ZPtCube210, ZPtCube120, ZPtCube220); //Bottom Surface
							CalculateTriangle(ZPtCube111, ZPtCube121, ZPtCube211, ZPtCube221); //Top Surface							
							CalculateTriangle(ZPtCube110, ZPtCube120, ZPtCube111, ZPtCube121);  //Front Surface				
							//CalculateTriangle(ZPtCube210, ZPtCube220, ZPtCube211, ZPtCube221); //Back Surface
							CalculateTriangle(ZPtCube210, ZPtCube211, ZPtCube220, ZPtCube221); //Back Surface
							CalculateTriangle(ZPtCube110, ZPtCube111, ZPtCube210, ZPtCube211);  //Left Surface
							//CalculateTriangle(ZPtCube120, ZPtCube121, ZPtCube220, ZPtCube221); //right Surface
							CalculateTriangle(ZPtCube120, ZPtCube220, ZPtCube121, ZPtCube221); //right Surface
							
						}
					}
					else
					{
						PtCube* ZPtCube110 = *ZPtCube11; 
						PtCube* ZPtCube210 = *ZPtCube21; 
						PtCube* ZPtCube120 = *ZPtCube12; 
						PtCube* ZPtCube220 = *ZPtCube22; 
						CalculateTriangle(ZPtCube110, ZPtCube120, ZPtCube210, ZPtCube220);
					}
				}
				if(ListIter2 != AllPtCubeCollection.end())
	          	{
				    ListIter1++; ListIter2++;
				}
		   }
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0006", __FILE__, __FUNCSIG__); }
}

void TriangulationCalc::CalculateTriangle(PtCube* ZPtCubeColl11, PtCube* ZPtCubeColl12, PtCube* ZPtCubeColl21, PtCube* ZPtCubeColl22)
{
	try
	{
		if(ZPtCubeColl11->CPointColl.Pointscount() > 0 && ZPtCubeColl21->CPointColl.Pointscount() > 0 && ZPtCubeColl22->CPointColl.Pointscount() > 0)
		{
			if(MAINDllOBJECT->CreateSurfaceForClosedSurface())
				AddOneTrianglePoints(ZPtCubeColl11->CPointColl.getList()[0], ZPtCubeColl22->CPointColl.getList()[0], ZPtCubeColl21->CPointColl.getList()[0]);
			else
				AddOneTrianglePoints(ZPtCubeColl11->CPointColl.getList()[0], ZPtCubeColl21->CPointColl.getList()[0], ZPtCubeColl22->CPointColl.getList()[0]);
			if(ZPtCubeColl12->CPointColl.Pointscount() > 0)
				AddOneTrianglePoints(ZPtCubeColl11->CPointColl.getList()[0], ZPtCubeColl12->CPointColl.getList()[0], ZPtCubeColl22->CPointColl.getList()[0]);
		}
		else if(ZPtCubeColl21->CPointColl.Pointscount() > 0 && ZPtCubeColl22->CPointColl.Pointscount() > 0 && ZPtCubeColl12->CPointColl.Pointscount() > 0)
			AddOneTrianglePoints(ZPtCubeColl21->CPointColl.getList()[0], ZPtCubeColl12->CPointColl.getList()[0], ZPtCubeColl22->CPointColl.getList()[0]);
		else if(ZPtCubeColl22->CPointColl.Pointscount() > 0 && ZPtCubeColl12->CPointColl.Pointscount() > 0 && ZPtCubeColl11->CPointColl.Pointscount() > 0)
			AddOneTrianglePoints(ZPtCubeColl22->CPointColl.getList()[0], ZPtCubeColl11->CPointColl.getList()[0], ZPtCubeColl12->CPointColl.getList()[0]);
		else if(ZPtCubeColl12->CPointColl.Pointscount() > 0 && ZPtCubeColl11->CPointColl.Pointscount() > 0 && ZPtCubeColl21->CPointColl.Pointscount() > 0)
			AddOneTrianglePoints(ZPtCubeColl12->CPointColl.getList()[0], ZPtCubeColl21->CPointColl.getList()[0], ZPtCubeColl11->CPointColl.getList()[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0007", __FILE__, __FUNCSIG__); }
}

//void TriangulationCalc::TriangulatePoints()
//{
//	try
//	{
//		std::list<std::list<std::list<PtCube*>*>*>::iterator ListIter1 = AllPtCubeCollection.begin();
//		std::list<std::list<std::list<PtCube*>*>*>::iterator ListIter2 = AllPtCubeCollection.begin();
//		if(ListIter2 != AllPtCubeCollection.end())
//		{
//			ListIter2++;
//			for(int y = 0; y < NoofCellsY - 1; y++)
//			{
//				std::list<std::list<PtCube*>*>* XPtCubeColl1 = (*ListIter1);
//				std::list<std::list<PtCube*>*>* XPtCubeColl2 = (*ListIter2);
//				std::list<std::list<PtCube*>*>::iterator ListIterX1 = XPtCubeColl1->begin();
//				std::list<std::list<PtCube*>*>::iterator ListIterX2 = XPtCubeColl2->begin();
//				for(int x = 0; x < NoofCellsX - 1; x++)
//				{
//					std::list<PtCube*>* ZPtCubeColl11 = (*ListIterX1++); std::list<PtCube*>* ZPtCubeColl21 = (*ListIterX2++);
//					std::list<PtCube*>* ZPtCubeColl12 = (*ListIterX1); std::list<PtCube*>* ZPtCubeColl22 = (*ListIterX2);
//					SinglePoint* Point11 = NULL, *Point12 = NULL, *Point21 = NULL, *Point22 = NULL;
//					bool Point11flag = CheckForPointinCube(ZPtCubeColl11, &Point11);
//					bool Point12flag = CheckForPointinCube(ZPtCubeColl12, &Point12);
//					bool Point21flag = CheckForPointinCube(ZPtCubeColl21, &Point21);
//					bool Point22flag = CheckForPointinCube(ZPtCubeColl22, &Point22);
//					if(Point11flag && Point21flag && Point22flag)
//					{
//						AddOneTrianglePoints(Point11, Point21, Point22);
//						if(Point12flag)
//							AddOneTrianglePoints(Point11, Point12, Point22);
//					}
//					else if(Point21flag && Point22flag && Point12flag)
//						AddOneTrianglePoints(Point21, Point22, Point12);
//					else if(Point22flag && Point12flag && Point11flag)
//						AddOneTrianglePoints(Point22, Point12, Point11);
//					else if(Point12flag && Point11flag && Point21flag)
//						AddOneTrianglePoints(Point12, Point11, Point21);
//				}
//				if(ListIter2 != AllPtCubeCollection.end())
//	          	{
//				    ListIter1++; ListIter2++;
//				}
//		   }
//		}
//	}
//	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0006", __FILE__, __FUNCSIG__); }
//}

//Create one triangle..
void TriangulationCalc::AddOneTrianglePoints(SinglePoint* Point1, SinglePoint* Point2, SinglePoint* Point3)
{
	try
	{
		TrainglePointCollection.Addpoint(new SinglePoint(Point1->X, Point1->Y, Point1->Z));
		TrainglePointCollection.Addpoint(new SinglePoint(Point2->X, Point2->Y, Point2->Z));
		TrainglePointCollection.Addpoint(new SinglePoint(Point3->X, Point3->Y, Point3->Z));
		/*TrainglePointCollection.Addpoint(new SinglePoint(Point1->X, Point1->Y, Point1->Z));
		TrainglePointCollection.Addpoint(new SinglePoint(Point3->X, Point3->Y, Point3->Z));
		TrainglePointCollection.Addpoint(new SinglePoint(Point2->X, Point2->Y, Point2->Z));*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0007", __FILE__, __FUNCSIG__); }
}

void TriangulationCalc::AddOneLinePoints(SinglePoint* Point1, SinglePoint* Point2)
{
	try
	{
		LinePointCollection.Addpoint(new SinglePoint(Point1->X, Point1->Y, Point1->Z));
		LinePointCollection.Addpoint(new SinglePoint(Point2->X, Point2->Y, Point2->Z));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0007a", __FILE__, __FUNCSIG__); }
}

//Get one point from one box if point exists... Returns First point!
bool TriangulationCalc::CheckForPointinCube(std::list<PtCube*>* ZPtCubeColl, SinglePoint** Point)
{
	try
	{
		bool Checkflag = false;
		for each(PtCube* CZCube in *ZPtCubeColl)
		{
			if(CZCube->CPointColl.Pointscount() > 0)
			{
				*Point = CZCube->CPointColl.getList()[0];
				Checkflag = true; break;
			}
		}
		return Checkflag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TACFN0008", __FILE__, __FUNCSIG__); }
} 