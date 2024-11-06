#pragma once

//Current Selected Shape..
enum FixedGraphBox
{
	LINE_BOX,
	CIRCLE_BOX,
	TEXT_BOX,
	DEFALUT,
};
namespace RWrapper
{
public ref class  RW_FixedGraph
{
public:
	delegate void FixedGraphEventHandler();

private:
	//Colordialog to set the shape color..//
	System::Windows::Forms::ColorDialog^ _colorDialog;
	//Counter to maintain the total no. of fixed shape added...//
	int ShapeCounter;
	int LineCounter, CircleCounter, TextCounter;
	//fixed graph instance,...//
	static RW_FixedGraph^ Fixedgraph;
	System::String^ CurrentSelected;
public:
	double CenterX, CenterY, Radius;
	double Angle, Offset, lineType;
	bool Hatched, DontClear, NonClearable;
	bool Circle_DiameterType;
	bool Line_FromOrigin, Line_ThroughOrigin, Line_Xoffset, Line_Yoffset;
	bool NudgeFixedGraphics;
	double TextAngle;
	System::String^ CurrentText;
	System::String^ FixedShapeName;
	FixedGraphBox currentFixedGraph;
	System::Collections::Hashtable^ FixedShapeCollection;

	event FixedGraphEventHandler^ ClearFixedShapeSelection;
	event FixedGraphEventHandler^ UpdateSelectedShapeParam;

public:

	//Constructor and destructor...//
	RW_FixedGraph();
	~RW_FixedGraph();
	//Fixedgraph instance...//
	static RW_FixedGraph^ MYINSTANCE();

	void HandleFixedGraphics(System::String^ Str);
	void AddFixedShape(int ShapeType);
	void SelectFixedShape(System::String^ Str);
	void RemoveSelected(System::String^ Str);
	void ChangeColor_Seletecd();
	//Handle Param Changed//
	void CircleParameterChanged(double x, double y, double dia, bool Diameter);
	void LineParameterChanged(bool ThroughOrigin, bool FromOrigin, bool XOffset, bool YOffset, double LAngle, double LOffset);
	void TextParameterChanged(System::String^ Str, double angle);
	
	void ChangeFont();
	void Nudge_SelectedFixedShape();
	void NudgeFixedShape(double x, double y);
	void ClearFixedGraph(bool clearAll);
	void ClearCurrentSelectedShape();
};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!