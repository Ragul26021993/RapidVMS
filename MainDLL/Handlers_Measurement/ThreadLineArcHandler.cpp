#include "StdAfx.h"
#include "ThreadLineArcHandler.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Perimeter.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\LineArcAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Helper\Helper.h"

//Constrctor.. Initialise settings
ThreadLineArcHandler::ThreadLineArcHandler()
{
	try
	{
		ValidFlag = false;
		ThreadCount = 3;
		//get this flag from settings to determine whether we need to also measure top and root radii or not.  
		//will measure top/root radius only if flag is true.
		line_arc_line_thrd = false;
		for(int i = 0; i < 2; i++)
			SelectedShapes[i] = NULL;
		init();
		ValidFlag = true;
		MAINDllOBJECT->SetStatusCode("ThreadLineArcHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0001", __FILE__, __FUNCSIG__); }
}

ThreadLineArcHandler::~ThreadLineArcHandler()
{
	ValidFlag = false;
	ptr_TempShapeCollection = NULL;
	NotNeededShapes.clear();
	UpperThrdShapes.clear();
	LowerThrdShapes.clear();
	IntPt1_upper.EraseAll();
	IntPt2_upper.EraseAll();
	IntPt1_lower.EraseAll();
	IntPt2_lower.EraseAll();
}

//Initialise settings..
void ThreadLineArcHandler::init()
{
	try
	{
		resetClicks();
		ptr_TempShapeCollection = NULL;
		MaxMouseActionSuccess = false;
		for(int i = 0; i < 2; i++)
		{
			if(SelectedShapes[i] != NULL)
				SelectedShapes[i]->HighlightedForMeasurement(false);
		}
		ShapeHighilghted = false;
		HighlightedShape = NULL;
		setMaxClicks(4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0002", __FILE__, __FUNCSIG__); }
}

//Selected End Shapes..
void ThreadLineArcHandler::LmouseDown()
{
	try
	{
		if (MaxMouseActionSuccess == true)
		{
			return;
		}

		if (getClicksDone() == 1)
		{
			HELPEROBJECT->AddLineArcAction_ProfileScan(true);
			return;
		}

		if ((getClicksDone() == 2))
		{
			if (HELPEROBJECT->profile_scan_running == true)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			if (HELPEROBJECT->profile_scan_failed == true)
			{
				//need to continue with profile scan till it meets with success.
				HELPEROBJECT->AddLineArcAction_ProfileScan(true);
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		if ((getClicksDone() == 2))
		{
			if (HELPEROBJECT->ProfileScanLineArcAction == NULL) 
			{
				HELPEROBJECT->ProfileScanLineArcAction = new LineArcAction(true);
				((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->lineArcSep(HELPEROBJECT->CurrentProfileCpShape);
				ptr_TempShapeCollection = &((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection;
			}
			else
			{
				ptr_TempShapeCollection = &((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection;
			}
			return;
		}
		//below is for first shape when clicksdone is 3
		if(ShapeHighilghted)
		{
			HighlightedShape->HighlightedForMeasurement(true);
			SelectedShapes[0] = HighlightedShape;
		}
		else
		{
			//Wrong click..
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0004", __FILE__, __FUNCSIG__); }
}

//Highlight Shapes for mouse move...
void ThreadLineArcHandler::mouseMove()
{
	try
	{
		if (!ValidFlag) return;

		if (ptr_TempShapeCollection == NULL) return;
		Vector* pt = &getClicksValue(getClicksDone());
		if(HighlightedShape != NULL)
			HighlightedShape->HighlightedFormouse(false);
		ShapeHighilghted = false; 
		HighlightedShape = NULL;
		double snapdist;
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			snapdist = MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX();
		}
		else
		{
			snapdist = MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getUppX() * SNAPDIST;
		}

		Shape* nshape = getNearestShape(*ptr_TempShapeCollection, pt->getX(), pt->getY(),  snapdist);
		if(nshape != NULL)
		{
			HighlightedShape = nshape;
			HighlightedShape->HighlightedFormouse(true);
			ShapeHighilghted = true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0005", __FILE__, __FUNCSIG__); }
}

//Check is there any shape near to the mouse position..
Shape* ThreadLineArcHandler::getNearestShape(map<int, Shape*> Shapecollection, double x, double y, double snapdist)
{
	try
	{
		Vector c1;
		c1.set(x, y);
		for(PSC_ITER pitem = Shapecollection.begin();  pitem != Shapecollection.end(); pitem++)
		{
			Shape* Cshape = (*pitem).second;
			if(Cshape->Shape_IsInWindow(c1, snapdist))
				return Cshape;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0006", __FILE__, __FUNCSIG__); return NULL; }
}
 
//Reset the selection..
void ThreadLineArcHandler::EscapebuttonPress()
{
	init();
}

//draw the Shape Collection and Selected Shapes....//
void ThreadLineArcHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(!ValidFlag) return;

		if (ptr_TempShapeCollection == NULL) return;

		for(PSC_ITER pitem = ptr_TempShapeCollection->begin();  pitem != ptr_TempShapeCollection->end(); pitem++)
		{
			Shape* Cshape = (*pitem).second;
			if(Cshape->HighlightedForMeasurement())
				GRAFIX->SetColor_Double(0, 1, 0);
			else if(Cshape->HighlightedFormouse())
				GRAFIX->SetColor_Double(100.0/255.0, 147.0/255.0, 237.0/255.0);
			else 
				GRAFIX->SetColor_Double(1, 0, 1);
			Cshape->drawShape(windowno, WPixelWidth);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0007", __FILE__, __FUNCSIG__); }
}

//Max mouse click.. Will be called after selecting second shape.. 
void ThreadLineArcHandler::LmaxmouseDown()
{
	try
	{
		if (MaxMouseActionSuccess == true)
		{
			return;
		}
		if (ptr_TempShapeCollection == NULL) 
		{
			return;
		}
		// select second shape if not already selected.
		if ((SelectedShapes[1] == NULL) && (ShapeHighilghted))
		{
			HighlightedShape->HighlightedForMeasurement(true);
			SelectedShapes[1] = HighlightedShape;
		}
		
		//if we don't have two selected shapes by now, return.
		if ((SelectedShapes[0] == NULL) ||  (SelectedShapes[1] == NULL)) 
		{
			setClicksDone(4);
			return;
		}
		
		//initialize below flag to false.  For any return before completion of routine, flag would be false so further mouse clicks would be possible.
		MaxMouseActionSuccess = false;
		//clicks done is set back to stage for the two shapes to be selected
		setClicksDone(2);
		SelectedShapes[0]->HighlightedForMeasurement(false);
		SelectedShapes[1]->HighlightedForMeasurement(false);

		int shape_0_index = -1;
		int shape_2_index = -1;
		int i = 0;
		int spurious_shape_index = -1;
		int cnt = ptr_TempShapeCollection->size();
		map<int, Shape*> TmpShapeCollection;
		map<int, Shape*> ThrdShapes;
		
		//populate local map TmpShapeCollection from TempShapeCollection
		for(PSC_ITER pitem = ptr_TempShapeCollection->begin();  pitem != ptr_TempShapeCollection->end(); pitem++)
		{
			Shape* Cshape = (*pitem).second;
			TmpShapeCollection[i] = Cshape;
			i++;
		}

		//first find shape_0
		for (i = 0; i < cnt; i++)
		{
			if (TmpShapeCollection[i] != SelectedShapes[0])
			{
				continue;
			}
			else 
			{
				shape_0_index = i;
				break;
			}
		}

		if (shape_0_index == -1) return; //did not find shape_0 in temporary shape collection

		//now find shape_2
		for (i = 0; i < cnt; i++)
		{
			if (TmpShapeCollection[i] != SelectedShapes[1])
			{
				continue;
			}
			else 
			{
				shape_2_index = i;
				break;
			}
		}

		if (shape_2_index == -1) return; //did not find shape_2 in temporary shape collection

		bool thread_direction;
		if (shape_0_index < shape_2_index)
			thread_direction = true;
		else
			thread_direction = false;

		int upperThrdCnt = 0;
		int lowerThrdCnt = 0;
		if (thread_direction)
		{
			for(i = shape_0_index; i <= shape_2_index; i++)
			{
				ThrdShapes[upperThrdCnt] = TmpShapeCollection[i];
				upperThrdCnt++;
			}
		}
		else
		{
			for(i = shape_0_index; i >= shape_2_index; i--)
			{
				ThrdShapes[upperThrdCnt] = TmpShapeCollection[i];
				upperThrdCnt++;
			}
		}
		z = 0;
		CreateThreadShapes(ThrdShapes);

		cnt = UpperThrdShapes.size();
		for (int i = 0; i < cnt; i++)
		{
			LowerThrdShapes[i] = UpperThrdShapes[cnt - 1 - i];
		}
		MAINDllOBJECT->DerivedShapeCallback();

		//set flag to true indicating successful completion of function... this will prevent further mouse click actions.
		MaxMouseActionSuccess = true;
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0008", __FILE__, __FUNCSIG__); }
}

void ThreadLineArcHandler::SetAnyData(double *data)
{
	try
	{
		ThreadCount = data[0];
		Shape* newshape;
		MAINDllOBJECT->getActionsHistoryList().addItem((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction);
		((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->autothread = false;
		((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection.clear();
		((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection.clear();
 		if (!CreateThrdMeasurements())
		{
			for (int i = 0; i < UpperThrdShapes.size(); i++)
			{
				newshape = UpperThrdShapes[i];
				((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection[i] = newshape;
				((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection[i] = newshape;
				ChkaddShape(newshape, false);
			}
			MAINDllOBJECT->ShowMsgBoxString("ThreadLineArcHandlerSetAnyData01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		}
		else
		{
			for (int i = 0; i < FinalThrdShapes.size(); i++)
			{
				newshape = FinalThrdShapes[i];
				((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->TempShapeCollection[i] = newshape;
				((LineArcAction*)HELPEROBJECT->ProfileScanLineArcAction)->fgTempCollection[i] = newshape;
			}
		}
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0008", __FILE__, __FUNCSIG__); }
}

void ThreadLineArcHandler::ChkaddShape(Shape* shape, bool SelectShape)
{
	try
	{
		Shape* tmpShape;
		RCollectionBase& Sshapes = MAINDllOBJECT->getShapesList();
		for (RC_ITER shape_iter = Sshapes.getList().begin(); shape_iter != Sshapes.getList().end(); shape_iter++)
		{
			tmpShape = (Shape*) ((*shape_iter).second);
			if (tmpShape == shape) return;
		}
		AddShapeAction::addShape(shape, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0008", __FILE__, __FUNCSIG__); }
}

void ThreadLineArcHandler::CreateThreadShapes(map<int, Shape*> Shapecollection)
{
	try
	{
		double max_radius = 0.5;
		double min_angle_cutoff = M_PI/9;
		double min_length = 0.5;
		double noise_length = 0.3;

		map<int, Shape*> ThrdShapecollection;
		int cnt = Shapecollection.size();
		Shape* Cshape;
		Shape* Cshape2;
		double radius = 0;
		PointCollection PtColl;
		map<int, Shape*> Shapecollection2;
		double length = 0;
		double length_0 = 0;
		int k = 0;
		int j = 0;
		Shape* newshape;

		//merge successive shapes where one of them is less than the noise_length
		for(int i = 0; i < cnt - 1; i++)
		{
			Cshape = Shapecollection[i];

			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				length_0 = ((Circle*) Cshape)->Length();
			else
				length_0 = ((Line*) Cshape)->Length();
			j = i + 1;
			Cshape2 = Shapecollection[j];
			
			if (Cshape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
				length = ((Circle*) Cshape2)->Length();
			else
				length = ((Line*) Cshape2)->Length();
			if ((length < noise_length)||(length_0 < noise_length))
			{
				newshape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				((Circle*)newshape)->CircleType = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
				PtColl = *(((ShapeWithList*) Cshape)->PointsList);
				((ShapeWithList*)newshape)->AddPoints(&PtColl);
				NotNeededShapes[z] = Cshape;
				z++;
				Cshape = newshape;
			}
			PtColl = *(((ShapeWithList*) Cshape2)->PointsList);
			while ((length < noise_length)||(length_0 < noise_length))
			{
				((ShapeWithList*) Cshape)->AddPoints(&PtColl);
				length_0 = ((Circle*) Cshape)->Length();
				NotNeededShapes[z] = Cshape2;
				z++;
				i++;
				j++;
				if (j == cnt) break;
				Cshape2 = Shapecollection[j];
				PtColl = *(((ShapeWithList*) Cshape2)->PointsList);
				if (Cshape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
					length = ((Circle*) Cshape2)->Length();
				else
					length = ((Line*) Cshape2)->Length();
			}

			Shapecollection2[k] = Cshape;
			k++;
		}

		if (j == cnt - 1)
		{
			Shapecollection2[k] = Shapecollection[cnt  - 1];
			k++;
		}
		Shapecollection.clear();
		cnt = Shapecollection2.size();
		int m = 0;
		//treat arcs greater than the max_radius as straight lines.
		for(int i = 0; i < cnt; i++)
		{
			Cshape = Shapecollection2[i];
			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				radius = ((Circle*) Cshape)->Radius();
				if (radius > max_radius)
				{
					Cshape2 = new Line();
					PtColl = *(((ShapeWithList*) Cshape)->PointsList);
					((ShapeWithList*)Cshape2)->AddPoints(&PtColl);
					NotNeededShapes[z] = Cshape;
					z++;
					Cshape = Cshape2;
				}
			}
			Shapecollection[m] = Cshape;
			m++;
		}

		Shapecollection2.clear();
		cnt = Shapecollection.size();
		k = 0;
		j = 0;
		//successive lines with slopes falling less than min_angle_cutoff away are treated as a single line
		for(int i = 0; i < cnt - 1; i++)
		{
			Cshape = Shapecollection[i];
			j = i + 1;
			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double slope = ((Line*) Cshape)->Angle();
				Cshape2 = Shapecollection[j];
				while (Cshape2->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					double slope2 = ((Line*) Cshape2)->Angle();
					double diff_slope = abs(slope - slope2);
					diff_slope -= int(diff_slope/M_PI) * M_PI;
					if (diff_slope > M_PI_2)
						diff_slope = M_PI - diff_slope;
					if (diff_slope > min_angle_cutoff) 
						break;
					PtColl = *(((ShapeWithList*) Cshape2)->PointsList);
					((ShapeWithList*) Cshape)->AddPoints(&PtColl);
					NotNeededShapes[z] = Cshape2;
					z++;
					i++;
					j++;
					if (j == cnt) break;
					Cshape2 = Shapecollection[j];
				}
			}
			Shapecollection2[k] = Cshape;
			k++;
		}
		if (j == cnt - 1)
		{
			Shapecollection2[k] = Shapecollection[cnt  - 1];
			k++;
		}
		cnt = Shapecollection2.size();
		k = 0;
		//shapes with length less than min_length are ignored.
		for(int i = 0; i < cnt ; i++)
		{
			Cshape = Shapecollection2[i];
			
			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				length = ((Circle*) Cshape)->Length();
			else
				length = ((Line*) Cshape)->Length();
			if (length > min_length)
			{
				ThrdShapecollection[k] = Cshape;
				k++;
			}
			else
			{
				NotNeededShapes[z] = Cshape;
				z++;
			}
		}

		cnt = ThrdShapecollection.size();
		k = 0;
		j = 0;
		//check again for successive lines with slope falling less than min_angle_cutoff away.  this is because
		//some new successive lines may have come up because of ignoring shapes less than min_length
		for(int i = 0; i < cnt - 1; i++)
		{
			Cshape = ThrdShapecollection[i];
			j = i + 1;
			if (Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double slope = ((Line*) Cshape)->Angle();
				Cshape2 = ThrdShapecollection[j];
				while (Cshape2->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					double slope2 = ((Line*) Cshape2)->Angle();
					double diff_slope = abs(slope - slope2);
					diff_slope -= int(diff_slope/M_PI) * M_PI;
					if (diff_slope > M_PI_2)
						diff_slope = M_PI - diff_slope;
					if (diff_slope > min_angle_cutoff) 
						break;
					PtColl = *(((ShapeWithList*) Cshape2)->PointsList);
					((ShapeWithList*) Cshape)->AddPoints(&PtColl);
					NotNeededShapes[z] = Cshape2;
					z++;
					i++;
					j++;
					if (j == cnt) break;
					Cshape2 = ThrdShapecollection[j];
				}
			}
			UpperThrdShapes[k] = Cshape;
			k++;
		}
		if (j == cnt - 1)
		{
			UpperThrdShapes[k] = ThrdShapecollection[cnt  - 1];
			k++;
		}

		MAINDllOBJECT->Shape_Updated();
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0009", __FILE__, __FUNCSIG__); }
}

bool ThreadLineArcHandler::CreateThrdMeasurements()
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->dontUpdateGraphcis = true;
		bool thrdmeasurementsucceeded = true;
		//get upper thrd angles and intersection points
		int cnt = UpperThrdShapes.size();
		int i_0 = 0;
		Shape* CShape = UpperThrdShapes[0];
		Shape* CShape2 = UpperThrdShapes[2];
		PointCollection PtColl;
		map<int, double> Ang1_upper, Ang2_upper, Ang1_lower, Ang2_lower, rad1_upper, rad2_upper, rad1_lower, rad2_lower, pitch_upper, pitch_lower, effective_diameter;
		
		double slope1, slope2, intercept1, intercept2, lineangle, lineintercept;
		double intersectionpt[2]={0};
		upperlines.clear();
		lowerlines.clear();
		upperarcs.clear();
		lowerarcs.clear();
		FinalThrdShapes.clear();
		DimBase* Cdim;
		bool isEvenPair = true;
		if (CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			i_0 = 1;
		}
		int seq = 0;
		int m=0, n=0, p=0, q=0, arc1=0, arc2=0;
		bool to_add_shape2 = false;
		bool second_try = false;
		double angle_chk = 0;
		int upper_thrd_cnt = 0;
		int lower_thrd_cnt = 0;
		for (int j = i_0; j < cnt - 1; j++)
		{
			to_add_shape2 = false;
			CShape = UpperThrdShapes[j];

			upperlines[p] = (Line*) CShape;
			if (p ==0) 
			{
				AddShapeAction::addShape(CShape, false);
				FinalThrdShapes[seq] = CShape;
				seq++;
			}

			CShape2 = UpperThrdShapes[j+1];
			second_try = false;
			if (CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				upperarcs[q] = (Circle*) CShape2;
				AddShapeAction::addShape(CShape2, false);
				FinalThrdShapes[seq] = CShape2;
				seq++;
				//call function to get and draw radius measurement of arc.
				Cdim = HELPEROBJECT->CreateRadiusMeasurement(CShape2);
				//if p is even, add radius to rad1_upper, else add radius to rad2_upper double map.
				if (p % 2 == 0) 
				 { rad1_upper[arc1] = Cdim->getDimension(); arc1++; }
				else 
				 { rad2_upper[arc2] = Cdim->getDimension(); arc2++; }
				q++;
				second_try = true;
				j++;
				if (j + 1 > cnt -1) break;
				CShape2 = UpperThrdShapes[j+1];
				if (CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC) 
				{
					break;
				}
			}

			getLine2LineAngle(CShape, CShape2, &angle_chk);

			if ((angle_chk > M_PI_2 * 0.75) || (angle_chk < M_PI_4))
			{
				if (second_try == false)
				{
					j++;
					if (j + 1 > cnt -1) break;
					CShape2 = UpperThrdShapes[j+1];
					if (CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC) 
					{
						break;
					}

					getLine2LineAngle(CShape, CShape2, &angle_chk);

					if ((angle_chk > M_PI_2 * 0.75) || (angle_chk < M_PI_4))
					{
						break;
					}
				}
				else
					break;
			}
			to_add_shape2 = true;
			//get intersection of CShape and CShape2 and store in IntPt1_upper and IntPt2_upper arrays alternately

			AddShapeAction::addShape(CShape2, false);
			FinalThrdShapes[seq] = CShape2;
			seq++;
			//call function to get angle (CShape, CShape2) and store in Ang1_upper and Ang2_upper arrays alternately
			Cdim = HELPEROBJECT->CreateAngleMeasurment(CShape, CShape2);
			
			slope1 = ((Line*) (CShape))->Angle();
			intercept1 = ((Line*) (CShape))->Intercept();

			slope2 = ((Line*) (CShape2))->Angle();
			intercept2 = ((Line*) (CShape2))->Intercept();

			RMATH2DOBJECT->Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectionpt[0]);
			if (isEvenPair)
			{
				Ang1_upper[m] = Cdim->getDimension();
				m++;
				IntPt1_upper.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));
				isEvenPair = false;
			}
			else
			{
				Ang2_upper[n] = Cdim->getDimension();
				n++;
				IntPt2_upper.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));
				isEvenPair = true;
			}
			p++;
			upper_thrd_cnt = int ((p - 1)/2);
			if (p >= 2* ThreadCount + 1) break;
		}
		if (to_add_shape2)
			upperlines[p] = (Line*) CShape2;

		if (to_add_shape2 == false)
		{
			thrdmeasurementsucceeded = false;
		}

		//get lower thrd angles and intersection points
		cnt = LowerThrdShapes.size();
		int i_1 = 0;
		isEvenPair = true;
		CShape = LowerThrdShapes[0];
		CShape2 = LowerThrdShapes[2];
		if (CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			i_1 = 1;
		}
		m = 0;
		n = 0;
		p = 0;
		q = 0;
		arc1 = 0;
		arc2 = 0;
		for (int j = i_1; j < cnt - 1; j ++)
		{
			to_add_shape2 = false;
			CShape = LowerThrdShapes[j];

			lowerlines[p] = (Line*) CShape;
			if (p ==0) 
			{
				AddShapeAction::addShape(CShape, false);
				FinalThrdShapes[seq] = CShape;
				seq++;
			}
			CShape2 = LowerThrdShapes[j+1];
			second_try = false;
			if (CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				lowerarcs[q] = (Circle*) CShape2;
				AddShapeAction::addShape(CShape2, false);
				FinalThrdShapes[seq] = CShape2;
				seq++;
				//call function to get and draw radius measurement of arc.
				Cdim = HELPEROBJECT->CreateRadiusMeasurement(CShape2);
				//if p is even, add radius to rad1_lower, else add radius to rad2_lower double map.
				if (p % 2 == 0) 
				 { rad1_lower[arc1] = Cdim->getDimension(); arc1++; }
				else 
				 { rad2_lower[arc2] = Cdim->getDimension(); arc2++; }
				q++;
				second_try = true;
				j++;
				if (j + 1 > cnt -1) break;
				CShape2 = LowerThrdShapes[j+1];
				if (CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC) 
				{
					break;
				}
			}

			getLine2LineAngle(CShape, CShape2, &angle_chk);

			if ((angle_chk > M_PI_2 * 0.75) || (angle_chk < M_PI_4))
			{
				if (second_try == false)
				{
					j++;
					if (j + 1 > cnt -1) break;
					CShape2 = LowerThrdShapes[j+1];
					if (CShape2->ShapeType == RapidEnums::SHAPETYPE::ARC) 
					{
						break;
					}

					getLine2LineAngle(CShape, CShape2, &angle_chk);
					if ((angle_chk > M_PI_2 * 0.75) || (angle_chk < M_PI_4))
					{
						break;
					}
				}
				else
					break;
			}
			to_add_shape2 = true;
			AddShapeAction::addShape(CShape2, false);
			FinalThrdShapes[seq] = CShape2;
			seq++;
			//call function to get angle (CShape, CShape2) and store in Ang1_upper and Ang2_upper arrays alternately
			Cdim = HELPEROBJECT->CreateAngleMeasurment(CShape, CShape2);
			//get intersection of CShape and CShape2 and store in IntPt1_upper and IntPt2_upper arrays alternately
			slope1 = ((Line*) (CShape))->Angle();
			intercept1 = ((Line*) (CShape))->Intercept();

			slope2 = ((Line*) (CShape2))->Angle();
			intercept2 = ((Line*) (CShape2))->Intercept();
			RMATH2DOBJECT->Line_lineintersection(slope1, intercept1, slope2, intercept2, &intersectionpt[0]);
			if (isEvenPair)
			{
				Ang2_lower[m] = Cdim->getDimension();
				m++;
				IntPt2_lower.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));
				isEvenPair = false;
			}
			else
			{
				Ang1_lower[n] = Cdim->getDimension();
				n++;
				IntPt1_lower.Addpoint(new SinglePoint(intersectionpt[0], intersectionpt[1], 0));
				isEvenPair = true;
			}
			p++;
			lower_thrd_cnt = int ((p - 1)/2);
			if (p >= 2 * ThreadCount + 1) break;
		}
		if (to_add_shape2)
			lowerlines[p] = (Line*) CShape2;

		if (to_add_shape2 == false)
		{
			thrdmeasurementsucceeded = false;
		}

		int thread_count = 0;
		if (lower_thrd_cnt < upper_thrd_cnt)
			thread_count = lower_thrd_cnt;
		else
			thread_count = upper_thrd_cnt;

		if (thread_count == 0) 
		{
			MAINDllOBJECT->dontUpdateGraphcis = false;
			return false;
		}

		//draw best fit line of IntPt1_upper and IntPt2_lower arrays (outer lines of thread)

		Line* UpperThrdLine;
		Line* LowerThrdLine;
		Line* Angle_BisectorLine;

		UpperThrdLine = new Line();
		UpperThrdLine->AddPoints(&IntPt1_upper);
		AddShapeAction::addShape(UpperThrdLine, false);
		
		LowerThrdLine = new Line();
		LowerThrdLine->AddPoints(&IntPt2_lower);
		AddShapeAction::addShape(LowerThrdLine, false);

		//draw angle bisector of above two lines as axis of thread
		Angle_BisectorLine = new Line();
		double mp[2] = {(IntPt1_upper.getList()[0]->X + IntPt2_lower.getList()[0]->X)/2, (IntPt1_upper.getList()[0]->Y + IntPt2_lower.getList()[0]->Y)/2};
		RMATH2DOBJECT->Angle_bisector(UpperThrdLine->Angle(), UpperThrdLine->Intercept(), LowerThrdLine->Angle(), LowerThrdLine->Intercept(), &mp[0], &lineangle, &lineintercept);
		RMATH2DOBJECT->Line_lineintersection(lineangle, lineintercept, lowerlines[0]->Angle(), lowerlines[0]->Intercept(), &intersectionpt[0]);
		PtColl.Addpoint (new SinglePoint(intersectionpt[0], intersectionpt[1], MAINDllOBJECT->getCurrentDRO().getZ()));
		RMATH2DOBJECT->Line_lineintersection(lineangle, lineintercept, lowerlines[p]->Angle(), lowerlines[p]->Intercept(), &intersectionpt[0]);
		PtColl.Addpoint (new SinglePoint(intersectionpt[0], intersectionpt[1], MAINDllOBJECT->getCurrentDRO().getZ()));
		Angle_BisectorLine->AddPoints(&PtColl);
		AddShapeAction::addShape(Angle_BisectorLine, false);

		//Get perpendicular to angle bisector line..
		double tempslope = lineangle + M_PI_2;
		RMATH2DOBJECT->Angle_FirstScndQuad(&tempslope);

		//get upper thread pitches

		//for each point of IntPt1_upper array from start to last but one, get the next point of IntPt1_upper array.
		cnt = IntPt1_upper.Pointscount();
		map <int, SinglePoint*> ptslist = IntPt1_upper.getList();
		m = 0;
		for (int i = 0; i < cnt - 1; i++)
		{
			//for each such pair of points, call function to get pitch distance (pt1, pt2), which will be the projected distance
			//of the distance along axis of thread.  Store in pitch_upper array.
			Cdim = HELPEROBJECT->CreatePt2PtDist_WrtLine(Angle_BisectorLine, upperlines[2*i], upperlines[2*i + 1], upperlines[2*i + 2], upperlines[2*i + 3]);
			pitch_upper[m] = Cdim->getDimension();
			m++;
		}

		//along similar lines get lower thread pitches
		cnt = IntPt2_lower.Pointscount();
		ptslist = IntPt2_lower.getList();
		m = 0;
		for (int i = 0; i < cnt - 1; i++)
		{
			//call function to get pitch distance (pt1, pt2), which will be the projected distance
			//of the distance along axis of thread.  Store in pitch_lower array.
			Cdim = HELPEROBJECT->CreatePt2PtDist_WrtLine(Angle_BisectorLine, lowerlines[2*i], lowerlines[2*i + 1], lowerlines[2*i + 2], lowerlines[2*i + 3]);
			pitch_lower[m] = Cdim->getDimension();
			m++;
		}

		//for each pair of points taken in a sequence, one each from IntPt1_upper and IntPt2_upper, get the effective 
		//diameter line for the thread using angles between the three thread lines and the normal to axis.
		
		//upper thrd
		cnt = IntPt1_upper.Pointscount();
		double PerAngleBisect_Angle = tempslope;
		double rad1, rad2, height, alpha1, alpha2, beta1, beta2, diff, Effrad;
		Line* CLine;
		map<int, Line*> D_upperlines;
		int sign = 1;
		if (IntPt1_upper.getList()[0]->Y < IntPt2_upper.getList()[0]->Y)
			sign = -1;
		for (int i = 0; i < cnt -1; i++)
		{
			rad2 = RMATH2DOBJECT->Pt2Line_Dist(IntPt1_upper.getList()[i]->X, IntPt1_upper.getList()[i]->Y, Angle_BisectorLine->Angle(), Angle_BisectorLine->Intercept());
			rad1 = RMATH2DOBJECT->Pt2Line_Dist(IntPt2_upper.getList()[i]->X, IntPt2_upper.getList()[i]->Y, Angle_BisectorLine->Angle(), Angle_BisectorLine->Intercept());
			height = rad2 - rad1;
			alpha1 = PerAngleBisect_Angle - upperlines[2*i]->Angle();
			beta1 = upperlines[2*i +1]->Angle() - PerAngleBisect_Angle;
			alpha2 = beta1;
			beta2 = PerAngleBisect_Angle - upperlines[2*i + 2]->Angle();
			diff = height / (1 + ((abs(tan(alpha1)) + abs(tan(beta1)))/(abs(tan(alpha2)) + abs(tan(beta2)))));
			Effrad = rad2 - diff;
			CLine = new Line(); 
			PtColl.EraseAll();
			intercept1 = lineintercept + sign * Effrad * abs(cos(lineangle));
			CLine->setLineParameters(lineangle, intercept1);
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, upperlines[2*i]->Angle(), upperlines[2*i]->Intercept(), &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1], MAINDllOBJECT->getCurrentDRO().getZ()));
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, upperlines[2*i + 2]->Angle(), upperlines[2*i + 2]->Intercept(), &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1], MAINDllOBJECT->getCurrentDRO().getZ()));
			CLine->AddPoints(&PtColl);
			AddShapeAction::addShape(CLine, false);
			D_upperlines[i] = CLine;
		}

		//lower thrd
		cnt = IntPt2_lower.Pointscount();
		map<int, Line*> D_lowerlines;
		for (int i = 0; i < cnt -1; i++)
		{
			rad2 = RMATH2DOBJECT->Pt2Line_Dist(IntPt2_lower.getList()[i]->X, IntPt2_lower.getList()[i]->Y, Angle_BisectorLine->Angle(), Angle_BisectorLine->Intercept());
			rad1 = RMATH2DOBJECT->Pt2Line_Dist(IntPt1_lower.getList()[i]->X, IntPt1_lower.getList()[i]->Y, Angle_BisectorLine->Angle(), Angle_BisectorLine->Intercept());
			height = rad2 - rad1;
			alpha1 = PerAngleBisect_Angle - lowerlines[2*i]->Angle();
			beta1 = lowerlines[2*i + 1]->Angle() - PerAngleBisect_Angle;
			alpha2 = beta1;
			beta2 = PerAngleBisect_Angle - lowerlines[2*i + 2]->Angle();
			diff = height / (1 + ((abs(tan(alpha1)) + abs(tan(beta1)))/(abs(tan(alpha2)) + abs(tan(beta2)))));
			Effrad = rad2 - diff;
			CLine = new Line(); 
			PtColl.EraseAll();
			intercept1 = lineintercept - sign * Effrad * abs(cos(lineangle));
			CLine->setLineParameters(lineangle, intercept1);
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, lowerlines[2*i]->Angle(), lowerlines[2*i]->Intercept(), &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1], MAINDllOBJECT->getCurrentDRO().getZ()));
			RMATH2DOBJECT->Line_lineintersection(lineangle, intercept1, lowerlines[2*i + 2]->Angle(), lowerlines[2*i + 2]->Intercept(), &intersectionpt[0]);
			PtColl.Addpoint(new SinglePoint (intersectionpt[0], intersectionpt[1], MAINDllOBJECT->getCurrentDRO().getZ()));
			CLine->AddPoints(&PtColl);
			AddShapeAction::addShape(CLine, false);
			D_lowerlines[i] = CLine;
		}

		//loop thru each pair of effective diameter lines taken in a sequence, one each from upper and lower thread.
		Line* line1;
		Line* line2;
		cnt = D_upperlines.size();
		if (D_lowerlines.size() < cnt)
			cnt = D_lowerlines.size();
		m = 0;
		for (int i = 0; i < cnt; i++)
		{
			line1 = D_upperlines[i];
			line2 = D_lowerlines[i];
			//call function to get distance between the parallel lines (line1, line2) and store in Effective Diameter array.
			Cdim = HELPEROBJECT->CreateLinetoLine_Dist(line1, line2);
			effective_diameter[m] = Cdim->getDimension();
			m++;
		}

		//get average values for Ang1_upper, Ang2_upper, Ang1_lower, Ang2_lower, pitch_upper, pitch_lower, effective diameter.
		double av_ang1_upper, av_ang2_upper, av_ang1_lower, av_ang2_lower, av_rad1_upper, av_rad2_upper, av_rad1_lower, av_rad2_lower, av_pitch_upper, av_pitch_lower, av_eff_diam;

		getaverage(Ang1_upper, &av_ang1_upper);
		getaverage(Ang2_upper, &av_ang2_upper);
		getaverage(Ang1_lower, &av_ang1_lower);
		getaverage(Ang2_lower, &av_ang2_lower);
		getaverage(rad1_upper, &av_rad1_upper);
		getaverage(rad2_upper, &av_rad2_upper);
		getaverage(rad1_lower, &av_rad1_lower);
		getaverage(rad2_lower, &av_rad2_lower);
		getaverage(pitch_upper, &av_pitch_upper);
		getaverage(pitch_lower, &av_pitch_lower);
		getaverage(effective_diameter, &av_eff_diam);

		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->Shape_Updated();
		return thrdmeasurementsucceeded;
		//display the average values before exiting
	}
	catch (...) { MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("THLNARCMH0010", __FILE__, __FUNCSIG__); return false;}
}

void ThreadLineArcHandler::getaverage(map<int, double>& list, double* ptr_average)
{
	*ptr_average = 0;
	int cnt = list.size();
	for (int i = 0; i < cnt; i++)
	{
		*(ptr_average) += list[i];
	}
	*(ptr_average) /= cnt;
}

void ThreadLineArcHandler::getLine2LineAngle(Shape* csh1, Shape* csh2, double* angle)
{
	try
	{
		Line* Cshape1 = (Line*)csh1;
		Line* Cshape2 = (Line*)csh2;

		double langle1 = Cshape1->Angle(), langle2 =  Cshape2->Angle(), radius, r_angle1, r_angle2, mid_angle;
		double ptofIntersection[2], L1point1[2], L1point2[2], L2point1[2], L2point2[2];
		Cshape1->getMidPoint()->FillDoublePointer(&ptofIntersection[0]);
		Cshape1->getPoint1()->FillDoublePointer(&L1point1[0]); Cshape1->getPoint2()->FillDoublePointer(&L1point2[0]);
		Cshape2->getPoint1()->FillDoublePointer(&L2point1[0]); Cshape2->getPoint2()->FillDoublePointer(&L2point2[0]);
		RMATH2DOBJECT->Line_lineintersection(Cshape1->Angle(), Cshape1->Intercept(), Cshape2->Angle(), Cshape2->Intercept(), &ptofIntersection[0]);
		if(RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L1point1[0]) > RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L1point2[0]))
			r_angle1 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L1point1[0]);
		else
			r_angle1 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L1point2[0]);
		if(RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L2point1[0]) > RMATH2DOBJECT->Pt2Pt_distance(&ptofIntersection[0], &L2point2[0]))
			r_angle2 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L2point1[0]);
		else
			r_angle2 = RMATH2DOBJECT->ray_angle(&ptofIntersection[0], &L2point2[0]);
		if(abs(r_angle1 - r_angle2) > M_PI)
		{
			if(r_angle1 < r_angle2) r_angle1 += 2 * M_PI;
			else r_angle2 += 2 * M_PI;
		}
		*angle = abs(r_angle1 - r_angle2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0015", __FILE__, __FUNCSIG__); }
}