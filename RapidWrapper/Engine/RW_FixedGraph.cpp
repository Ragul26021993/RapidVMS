#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDll\FixedTool\FixedToolCollection.h"
#include "..\MAINDll\FixedTool\FixedShape.h"
#define _USE_MATH_DEFINES
#include <math.h>

RWrapper::RW_FixedGraph::RW_FixedGraph()
{
	try
	{
		this->Fixedgraph = this;
		this->currentFixedGraph = FixedGraphBox::DEFALUT;
		this->_colorDialog = gcnew System::Windows::Forms::ColorDialog();
		this->ShapeCounter = 1;
		this->LineCounter = 0; this->CircleCounter = 0; this->TextCounter = 0;
		this->FixedShapeCollection = gcnew System::Collections::Hashtable;
		this->CurrentSelected = ""; this->CurrentText = "";
		this->DontClear = false;
		this->NonClearable = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0001", ex); }
}

RWrapper::RW_FixedGraph::~RW_FixedGraph()
{
}

RWrapper::RW_FixedGraph^ RWrapper::RW_FixedGraph::MYINSTANCE()
{
	return Fixedgraph;
}

void RWrapper::RW_FixedGraph::HandleFixedGraphics(System::String^ Str)
{
	try
	{
		if(Str == "Fixed Line")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics01");
			AddFixedShape(0);
		}
		else if(Str == "Fixed Circle")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics02");
			AddFixedShape(1);
		}
		else if(Str == "Fixed Text")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics03");
			AddFixedShape(2);
		}
		else if(Str == "Fixed Shape Color")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics04");
			ChangeColor_Seletecd();
		}
		else if(Str == "Delete Fixed Shape")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics05");
			RemoveSelected(this->CurrentSelected);
		}
		else if(Str == "Clear All Fixed Shape")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics06");
			ClearFixedGraph(false);
		}
		else if(Str == "Fixed Hatced Graphics")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics07");
			FIXEDTOOLOBJECT->ToggleFixedShpHatched();
			MAINDllOBJECT->update_VideoGraphics();
		}
		else if(Str == "Fixed Graphics Nudge")
		{
			MAINDllOBJECT->SetStatusCode("HandleFixedGraphics08");
			Nudge_SelectedFixedShape();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0002", ex); }
}

void RWrapper::RW_FixedGraph::AddFixedShape(int ShapeType)
{
	try
	{
		NudgeFixedGraphics = false;
		FIXEDTOOLOBJECT->AddFixedShapes(ShapeType, this->ShapeCounter, DontClear);
		currentFixedGraph = FixedGraphBox(ShapeType);
		if(ShapeType == 0)
		{
			this->LineCounter++;
			FixedShapeName = String::Concat("FL", this->LineCounter.ToString());
			//NudgeFixedGraphics = true;
		}
		else if(ShapeType == 1)
		{
			this->CircleCounter++;
			FixedShapeName = String::Concat("FC", this->CircleCounter.ToString());
			//NudgeFixedGraphics = true;
		}
		else if(ShapeType == 2)
		{
			this->TextCounter++;
			FixedShapeName = String::Concat("FT", this->TextCounter.ToString());
			NudgeFixedGraphics = true;
		}
		this->CurrentSelected = FixedShapeName;
		this->FixedShapeCollection->Add(FixedShapeName, this->ShapeCounter);
		this->ShapeCounter++;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0003", ex); }
}

void RWrapper::RW_FixedGraph::ChangeColor_Seletecd()
{
	try
	{
		if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			System::Drawing::Color _colorRgb = this->_colorDialog->Color;
			FIXEDTOOLOBJECT->setFixedToolColor(_colorRgb.R, _colorRgb.G, _colorRgb.B);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0004", ex); }
}

void RWrapper::RW_FixedGraph::SelectFixedShape(System::String^ Str)
{
	try
	{
		if(Str != "")
		{
			this->CurrentSelected = Str;
			int CurrentId = (int)FixedShapeCollection[Str];
			FIXEDTOOLOBJECT->selectFixedTool(CurrentId);
			Hatched = FIXEDTOOLOBJECT->FixedHatched();
			NonClearable = FIXEDTOOLOBJECT->FixedNonClearable();
			switch(FIXEDTOOLOBJECT->CurrentSelectedFixedshape())
			{
				case 0:
					currentFixedGraph = FixedGraphBox::LINE_BOX;
					Angle = FIXEDTOOLOBJECT->FixedgetX() * 180 /M_PI;
					Offset = FIXEDTOOLOBJECT->FixedgetY();
					lineType = FIXEDTOOLOBJECT->FixedRadius();
					switch((int)lineType)
					{
						case 0:
							Line_FromOrigin = true; Line_ThroughOrigin = false;
							Line_Xoffset = false; Line_Yoffset = false;
							break;
						case 1:
							Line_FromOrigin = false; Line_ThroughOrigin = true;
							Line_Xoffset = false; Line_Yoffset = false;
							break;
						case 2:
							Line_FromOrigin = false; Line_ThroughOrigin = false;
							Line_Xoffset = true; Line_Yoffset = false;
							break;
						case 3:
							Line_FromOrigin = false; Line_ThroughOrigin = false;
							Line_Xoffset = false; Line_Yoffset = true;
							break;
					}
					break;
				case 1:
					currentFixedGraph = FixedGraphBox::CIRCLE_BOX;
					CenterX = FIXEDTOOLOBJECT->FixedgetX();
					CenterY = FIXEDTOOLOBJECT->FixedgetY();
					Radius = FIXEDTOOLOBJECT->FixedRadius();
					if(FIXEDTOOLOBJECT->FixedCircleType())
						Circle_DiameterType = true;
					else
						Circle_DiameterType = false;
					break;
				case 2:
					currentFixedGraph = FixedGraphBox::TEXT_BOX;
					CurrentText = gcnew System::String(FIXEDTOOLOBJECT->FixedString());
					TextAngle = FIXEDTOOLOBJECT->FixedRadius();
					break;
			}
			MAINDllOBJECT->update_VideoGraphics();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0010", ex); }
}

void RWrapper::RW_FixedGraph::RemoveSelected(System::String^ Str)
{
	try
	{
		if(Str != "")
		{
			FIXEDTOOLOBJECT->RemoveSelected();
			FixedShapeCollection->Remove(Str);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0011", ex); }
}

void RWrapper::RW_FixedGraph::ChangeFont()
{
	try
	{
		//System::Windows::Forms::FontDialog^ fontDialog = gcnew System::Windows::Forms::FontDialog;
		//if(fontDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		//{
			//(HFONT)from->ToHfont().ToPointer();
			//MAINDllOBJECT->ChangeVideoFont((HFONT)fontDialog->Font->ToHfont().ToPointer());
		//}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0012", ex); }
}

void RWrapper::RW_FixedGraph::Nudge_SelectedFixedShape()
{
	try
	{
		if(RW_FixedGraph::MYINSTANCE()->currentFixedGraph != FixedGraphBox::DEFALUT)
			NudgeFixedGraphics = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0013", ex); }
}

void RWrapper::RW_FixedGraph::NudgeFixedShape(double x, double y)
{
	try
	{
		if(currentFixedGraph == FixedGraphBox::CIRCLE_BOX)
		{
			double temp = Radius; double ctype = 0;
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				temp = temp * 25.4;
			if(Circle_DiameterType)
				ctype = 1;
			FIXEDTOOLOBJECT->setFixedToolParameter(x, y, ctype, &temp);
		}
		else if(currentFixedGraph == FixedGraphBox::LINE_BOX)
		{
			double temp = lineType;
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				temp = temp * 25.4;
			FIXEDTOOLOBJECT->setFixedToolParameter(x, y, Angle * M_PI / 180, &temp);
		}
		else if(currentFixedGraph == FixedGraphBox::TEXT_BOX)
		{
			FIXEDTOOLOBJECT->setFixedToolParameter(x, y, 0, (void*)FIXEDTOOLOBJECT->FixedString());
		}
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0014", ex); }
}

void RWrapper::RW_FixedGraph::ClearFixedGraph(bool clearAll)
{
	try
	{
		currentFixedGraph = FixedGraphBox::DEFALUT;
		if(clearAll)
		{
			  for(map<int, FixedShape*>::iterator It = FIXEDTOOLOBJECT->FixedShapeCollection.begin(); It != FIXEDTOOLOBJECT->FixedShapeCollection.end(); It++)
	  		  {
				 if(It->second->DontClear()) continue;
				 for each(System::Collections::DictionaryEntry^ item in  FixedShapeCollection)
				 {
				   if((int)item->Value == It->first)
				   {
					  FixedShapeCollection->Remove(item->Key);
					  break;
				   }
				 }
			  }
			  TextCounter = 0;
		}
		else
		{
	    	FixedShapeCollection->Clear();
			ShapeCounter = 1; LineCounter = 0;
	    	CircleCounter = 0; TextCounter = 0;
		}
		
		FIXEDTOOLOBJECT->clearFixedTool(clearAll);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0015", ex); }
}

void RWrapper::RW_FixedGraph::ClearCurrentSelectedShape()
{
	if(CurrentSelected != "")
	{
		SelectFixedShape(CurrentSelected);
		currentFixedGraph = FixedGraphBox::DEFALUT;
		UpdateSelectedShapeParam::raise();
		CurrentSelected = "";
		ClearFixedShapeSelection::raise();
	}
}

void RWrapper::RW_FixedGraph::CircleParameterChanged(double x, double y, double dia, bool Diameter)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
		{
			x = x * 25.4; y = y * 25.4;
			dia = dia * 25.4;
		}
		if(currentFixedGraph != FixedGraphBox::CIRCLE_BOX) return;
		double _radius = 0.0;
		Circle_DiameterType = false;
		int dtype = 0;
		CenterX = x; CenterY = y;
		Radius = dia;
		_radius = dia;
		if(Diameter)
		{
			Circle_DiameterType = true;
			dtype = 1;
		}
		FIXEDTOOLOBJECT->setToolParameter(x, y, (double)dtype, &_radius);
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0006", ex); }
}

void RWrapper::RW_FixedGraph::LineParameterChanged(bool ThroughOrigin, bool FromOrigin, bool XOffset, bool YOffset, double LAngle, double LOffset)
{
	try
	{
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			LOffset = LOffset * 25.4;
		if(currentFixedGraph != FixedGraphBox::LINE_BOX) return;
		Angle = LAngle;
		Offset = LOffset;
		double temp = 0;
		if(FromOrigin)
			FIXEDTOOLOBJECT->setToolParameter(0, 0, LAngle * M_PI / 180, &temp);
		else if(ThroughOrigin)
		{
			temp = 1;
			FIXEDTOOLOBJECT->setToolParameter(0, 0, LAngle * M_PI / 180, &temp);
		}
		else if(XOffset)
		{
			temp = 2;
			FIXEDTOOLOBJECT->setToolParameter(LOffset, 0, LAngle * M_PI / 180, &temp);
		}
		else if(YOffset)
		{
			temp = 3;
			FIXEDTOOLOBJECT->setToolParameter(LOffset, 1, LAngle * M_PI / 180, &temp);
		}
		lineType = temp;
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0009", ex); }
}

void RWrapper::RW_FixedGraph::TextParameterChanged(System::String^ Str, double angle)
{
	try
	{
		if(currentFixedGraph != FixedGraphBox::TEXT_BOX) return;
		if(Str != "")
		{
			CurrentText = Str;
			TextAngle = angle;
			FIXEDTOOLOBJECT->setToolParameter(FIXEDTOOLOBJECT->FixedgetX(), FIXEDTOOLOBJECT->FixedgetY(), angle, (void*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
			MAINDllOBJECT->update_VideoGraphics();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFG0007", ex); }
}