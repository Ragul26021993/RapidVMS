using System;
using System.Windows;
using System.Windows.Controls;
using Rapid.AttachedProperties;
using RapidI.Help;

namespace Rapid.LightingControl
{
    //This class takes care of arranging all the zones as well as handling the interactions as well.
    public class LightingGrid : Grid
    {

        bool LoadingCompleted;
        //Event handler for the Value change event
        public event EventHandler ValueChanged;
        //Array of the Arc zones that will be drawn
        SingleLightingZone[] LightArcs;
        //The textbox in which user can write values after RightClick.
        protected internal TextBox EnterText;
        //To check if mouse drag is happening
        bool MouseDrag;
        //Point to store information of what is the selected zone when textbox has to be shown.
        Point SelectedTag = new Point(0, 0);
        //Constructor.
        public LightingGrid()
            : base()
        {
            //Making Auxillary to be always present as default.
            this.IsAuxillaryLightingPresent = true;
            //The default values for this Drwaing.(This can also be set at runtime as well.)
            this.TotalCircles = 4;
            this.NoOfZonesPerCircle_TowardsCenter = new int[] { 2, 2 };
            LightingDrawingDirection = new DirectionOfDrawing[] { DirectionOfDrawing.ClockWiseWithOffset, DirectionOfDrawing.ClockWise };
            this.ShapeThickness = 20;
            this.ZonesIds_TowardsCenterAntiClockWise = new int[] { 1, 2, 3, 4, 5, 6 };
            this.ZonesNames_TowardsCenterAntiClockWise = new string[] { "Zone 1", "Zone 2", "Zone 3", "Zone 4", "  Profile", " Auxillary" };
            this.OuterShapeType = LightingShapesTypes.Round;
            //Get the total no of shapes required & initialize the Arcs  & Value arrays.
            int count = 0;
            for (int i = 0; i < NoOfZonesPerCircle_TowardsCenter.Length; i++)
                count += NoOfZonesPerCircle_TowardsCenter[i];
            LightArcs = new SingleLightingZone[count];
            Values = new int[count];
            //handling the loaded event for the actual layout calculations.
            this.Loaded += new RoutedEventHandler(LightingGrid_Loaded);
            //handling the RightMouseDown event for the textbox to show.
            this.PreviewMouseRightButtonUp += new System.Windows.Input.MouseButtonEventHandler(LightingGrid_PreviewMouseRightButtonUp);
            //Initializing the textBox.
            EnterText = new TextBox();
            EnterText.Width = 40; EnterText.Height = 20;
            //make the textbox to appear always at top.
            Panel.SetZIndex(EnterText, 50);
            EnterText.Visibility = System.Windows.Visibility.Hidden;
            EnterText.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
            EnterText.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            EnterText.PreviewKeyUp += new System.Windows.Input.KeyEventHandler(EnterText_PreviewKeyUp);
            AttachedTextBoxBehaviour.SetMask(EnterText, MaskType.Integer);
            AttachedTextBoxBehaviour.SetMinimumValue(EnterText, 0);
            AttachedTextBoxBehaviour.SetMaximumValue(EnterText, 64);
            this.Children.Add(EnterText);
        }
        //handling the keydown events for the textbox.
        void EnterText_PreviewKeyUp(object sender, System.Windows.Input.KeyEventArgs e)
        {
            //On enter press just update the value of current zone.
            if (e.Key == System.Windows.Input.Key.Enter)
            {
                int LightValue = 0;
                if (int.TryParse(EnterText.Text, out LightValue))
                {
                    if (SelectedTag.Y == 0 && this.IsLocked)
                    {
                        for (int i = 0; i < 4; i++)
                            Values[i] = LightValue;
                    }
                    else
                        Values[((int)SelectedTag.X - 1)] = LightValue;
                }
                EnterText.Visibility = System.Windows.Visibility.Hidden;
            }
            //On Esc press just hide the textbox.
            else if (e.Key == System.Windows.Input.Key.Escape)
            {
                EnterText.Visibility = System.Windows.Visibility.Hidden;
            }
            //raise the Value change event.
            if (ValueChanged != null)
                ValueChanged(this, null);
        }
        //handling the RightMouseDown event for this class.
        void LightingGrid_PreviewMouseRightButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            //Check on which zone mouse is present.
            bool IsOverOneShape = false;
            for (int i = 0; i < LightArcs.Length; i++)
            {
                IsOverOneShape = LightArcs[i].IsMouseOver;
                SelectedTag = (Point)LightArcs[i].Tag;
                if (IsOverOneShape)
                    break;
            }
            if (IsOverOneShape)
            {
                Point currentPos = e.GetPosition(this);
                EnterText.Margin = new Thickness(currentPos.X, currentPos.Y, 0, 0);
                EnterText.Text = Values[(int)SelectedTag.X - 1].ToString();
                EnterText.SelectAll();
                EnterText.Visibility = System.Windows.Visibility.Visible;
            }
        }
        //Making all arrangeent and layouts of teh All Zones/Arcs in the load event.
        void LightingGrid_Loaded(object sender, RoutedEventArgs e)
        {
            //IsCoAxialLightingPresent = true;
            if (!LoadingCompleted)
            {
                if (OneShotLighting)
                {
                    this.TotalCircles = 1;
                    this.NoOfZonesPerCircle_TowardsCenter = new int[] { 2 };
                    //LightingDrawingDirection = new DirectionOfDrawing[] { DirectionOfDrawing.CounterClockWise };
                    LightingDrawingDirection = new DirectionOfDrawing[] { DirectionOfDrawing.ClockWise };
                    //this.ShapeThickness = 32;
                    this.ZonesIds_TowardsCenterAntiClockWise = new int[] { 1, 2 };
                    if (this.RenderSize.Height == 190)
                        this.ZonesNames_TowardsCenterAntiClockWise = new string[] { "    Surface", "     Profile" };
                    else
                        this.ZonesNames_TowardsCenterAntiClockWise = new string[] { "    Surface", "     Profile" };
                    this.OuterShapeType = LightingShapesTypes.Flat;
                    //Get the total no of shapes required.
                    int count = 0;
                    for (int i = 0; i < NoOfZonesPerCircle_TowardsCenter.Length; i++)
                        count += NoOfZonesPerCircle_TowardsCenter[i];
                    LightArcs = new SingleLightingZone[count];
                    Values = new int[count];
                }
                //else if (IsCoAxialLightingPresent)
                //{
                //    //this.TotalCircles = 4;
                //    this.NoOfZonesPerCircle_TowardsCenter = new int[] { 4, 3 };
                //    //LightingDrawingDirection = new DirectionOfDrawing[] { DirectionOfDrawing.ClockWiseWithOffset, DirectionOfDrawing.CounterClockWise };
                //    LightingDrawingDirection = new DirectionOfDrawing[] { DirectionOfDrawing.ClockWiseWithOffset, DirectionOfDrawing.ClockWise };
                //    //this.ShapeThickness = 32;
                //    this.ZonesIds_TowardsCenterAntiClockWise = new int[] { 1, 2, 3, 4, 5, 6, 7, 8 };
                //    if (this.RenderSize.Height == 190)
                //        this.ZonesNames_TowardsCenterAntiClockWise = new string[] { " Zone 1", " Zone 2", " Zone 3", "Zone 4", "  Pro", "  Aux", "CoAx" };
                //    else
                //        this.ZonesNames_TowardsCenterAntiClockWise = new string[] { "  Zone 1", "  Zone 2", "  Zone 3", " Zone 4", "   Pro", "   Aux", "Co-Ax" };
                //    this.OuterShapeType = LightingShapesTypes.Flat;
                //    //Get the total no of shapes required.
                //    int count = 0;
                //    for (int i = 0; i < NoOfZonesPerCircle_TowardsCenter.Length; i++)
                //        count += NoOfZonesPerCircle_TowardsCenter[i];
                //    LightArcs = new SingleLightingZone[count];
                //    Values = new int[count];
                //}
                else
                {
                    //The default values for this control.
                    //this.TotalCircles = 6;
                    this.NoOfZonesPerCircle_TowardsCenter = new int[this.TotalCircles];
                    for (int ii = 0; ii < this.TotalCircles - 1; ii++)
                        this.NoOfZonesPerCircle_TowardsCenter[ii] = 4;
                    this.NoOfZonesPerCircle_TowardsCenter[this.TotalCircles - 1] = 2;
                    if (IsCoAxialLightingPresent)
                        this.NoOfZonesPerCircle_TowardsCenter[this.TotalCircles - 1] = 3;

                        //if (this.TotalCircles == 6)
                        //    this.NoOfZonesPerCircle_TowardsCenter = new int[] { 4, 4, 4, 4, 4, 4, 4, 4 };
                        //else
                        //    this.NoOfZonesPerCircle_TowardsCenter = new int[] { 4, 4, 4, 2 };
                    //LightingDrawingDirection = new DirectionOfDrawing[] { DirectionOfDrawing.ClockWiseWithOffset, DirectionOfDrawing.CounterClockWise };
                    LightingDrawingDirection = new DirectionOfDrawing[] { DirectionOfDrawing.ClockWiseWithOffset, DirectionOfDrawing.ClockWise };
                    //this.ShapeThickness = 32;
                    int totalCt = 0;
                    for (int ii = 0; ii < this.NoOfZonesPerCircle_TowardsCenter.Length; ii++)
                        totalCt += this.NoOfZonesPerCircle_TowardsCenter[ii];
                    if (Math.IEEERemainder(totalCt, 2) != 0) totalCt += 1;
                    this.ZonesIds_TowardsCenterAntiClockWise = new int[totalCt]; // { 1, 2, 3, 4, 5, 6 };
                    this.ZonesNames_TowardsCenterAntiClockWise = new string[totalCt];
                    for (int ii = 0; ii < totalCt; ii++)
                        this.ZonesIds_TowardsCenterAntiClockWise[ii] = ii + 1;
                    string[] RingTag = new string[] { "A", "B", "C", "D", "E", "F", "G", "H" };
                    int tempC = 0;
                    if (this.TotalCircles == 2)
                    {
                        //this.ZonesNames_TowardsCenterAntiClockWise[4] = "Profile";
                        //this.ZonesNames_TowardsCenterAntiClockWise[5] = "Aux";
                        for (int i = 0; i < this.NoOfZonesPerCircle_TowardsCenter[0]; i++)
                            this.ZonesNames_TowardsCenterAntiClockWise[tempC++] = "Zone " + (i + 1);
                        this.ZonesNames_TowardsCenterAntiClockWise[tempC++] = "Profile";
                        this.ZonesNames_TowardsCenterAntiClockWise[tempC++] = "Aux";
                        if (IsCoAxialLightingPresent)
                        {
                            this.ZonesNames_TowardsCenterAntiClockWise[tempC - 2] = "Pro";
                            this.ZonesNames_TowardsCenterAntiClockWise[tempC++] = "Co-Ax";
                        }
                    }
                    else
                    {
                        for (int j = 0; j < this.NoOfZonesPerCircle_TowardsCenter.Length; j++)
                        {
                            for (int i = 0; i < this.NoOfZonesPerCircle_TowardsCenter[j]; i++)
                                this.ZonesNames_TowardsCenterAntiClockWise[tempC++] = RingTag[j] + (i + 1);
                        }
                    }
                    //if (this.RenderSize.Height != 190)
                        //this.ZonesNames_TowardsCenterAntiClockWise = new string[] { "  Zone 1", "  Zone 2", "  Zone 3", " Zone 4", "  Profile", "   Aux" };
                    //else
                        //this.ZonesNames_TowardsCenterAntiClockWise = new string[] { "  Zone 1", "  Zone 2", "  Zone 3", " Zone 4", "   Profile", " Auxillary" };
                    this.OuterShapeType = LightingShapesTypes.Flat;
                    //Get the total no of shapes required.
                    int count = 0;
                    for (int i = 0; i < NoOfZonesPerCircle_TowardsCenter.Length; i++)
                        count += NoOfZonesPerCircle_TowardsCenter[i];
                    LightArcs = new SingleLightingZone[count];
                    if (count > 20) count = 32;
                    Values = new int[totalCt];
                }
                //Get the line cap.
                System.Windows.Media.PenLineCap p = new System.Windows.Media.PenLineCap();
                switch (this.OuterShapeType)
                {
                    case LightingShapesTypes.Flat:
                        p = System.Windows.Media.PenLineCap.Flat;
                        break;
                    case LightingShapesTypes.Round:
                        p = System.Windows.Media.PenLineCap.Round;
                        break;
                    case LightingShapesTypes.Triangle:
                        p = System.Windows.Media.PenLineCap.Triangle;
                        break;
                }
                //Set the default types for the LightArcs
                for (int i = 0; i < LightArcs.Length; i++)
                {
                    LightArcs[i] = new SingleLightingZone();
                    LightArcs[i].Tag = new string[] { "Lighting", "", "zone" + i + 1 }; 
                    LightArcs[i].Thickness = this.ShapeThickness;
                    LightArcs[i].LineCap = p;
                    LightArcs[i].DrawnArcText.Foreground = System.Windows.Media.Brushes.Black;
                    LightArcs[i].PreviewMouseWheel += new System.Windows.Input.MouseWheelEventHandler(LightingGrid_PreviewMouseWheel);
                    LightArcs[i].PreviewMouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(LightingGrid_PreviewMouseLeftButtonDown);
                    LightArcs[i].PreviewMouseMove += new System.Windows.Input.MouseEventHandler(LightingGrid_PreviewMouseMove);
                    LightArcs[i].PreviewMouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(LightingGrid_PreviewMouseLeftButtonUp);
                    //LightArcs[i].PreviewMouseRightButtonDown +=new System.Windows.Input.MouseButtonEventHandler(LightingGrid_PreviewMouseRightButtonDown);
                    LightArcs[i].MouseDown += new System.Windows.Input.MouseButtonEventHandler(LightingGrid_MouseDown);
             
                    LightArcs[i].MouseLeave += new System.Windows.Input.MouseEventHandler(LightingGrid_PreviewMouseLeftButtonUp);

                    this.Children.Add(LightArcs[i]);
                }
                double[] gap = { 2, 2 };
                int ArcCount = 0;
                for (int i = 0; i < this.TotalCircles; i++)
                {
                    int angleStep = 360 / NoOfZonesPerCircle_TowardsCenter[i];
                    for (int ShapeCount = 0; ShapeCount < NoOfZonesPerCircle_TowardsCenter[i]; ShapeCount++)
                    {
                        if (NoOfZonesPerCircle_TowardsCenter[i] == 2)
                            DrawShapes(i, ShapeCount, ArcCount, angleStep, DirectionOfDrawing.CounterClockWise, 1); //LightingDrawingDirection[i % 2], gap[i%2 ]);
                        else
                            DrawShapes(i, ShapeCount, ArcCount, angleStep, DirectionOfDrawing.ClockWiseWithOffset, 1); //LightingDrawingDirection[i % 2], gap[i%2 ]);
    
                        ArcCount += 1;
                    }
                }
                LoadingCompleted = true;
            }
        }
        //Function to calculate and assign properties to the Arc zones.
        private void DrawShapes(int CircleCount, int Shapecount, int ArcCount, int AngleStepFactor, DirectionOfDrawing d, double SpaceBetweenShapes)
        {
            switch (d)
            {
                case DirectionOfDrawing.ClockWise:
                    LightArcs[ArcCount].StartAngle = (double)(AngleStepFactor * (Shapecount + 1)) - SpaceBetweenShapes;
                    LightArcs[ArcCount].EndAngle = (double)(AngleStepFactor * Shapecount) + SpaceBetweenShapes;
                    break;
                case DirectionOfDrawing.ClockWiseWithOffset:
                    {
                        LightArcs[ArcCount].StartAngle = (double)((3 * AngleStepFactor) / 2 - (AngleStepFactor * Shapecount)) - SpaceBetweenShapes;
                        LightArcs[ArcCount].EndAngle = (double)(AngleStepFactor / 2 - (AngleStepFactor * Shapecount)) + SpaceBetweenShapes;
                    }
                    break;
                case DirectionOfDrawing.CounterClockWiseWithOffset:
                    LightArcs[ArcCount].StartAngle = (double)(AngleStepFactor / 2 + (AngleStepFactor * Shapecount)) + SpaceBetweenShapes;
                    LightArcs[ArcCount].EndAngle = (double)((3 * AngleStepFactor) / 2 + (AngleStepFactor * Shapecount)) - SpaceBetweenShapes;
                    break;
                case DirectionOfDrawing.CounterClockWise:
                    {
                        LightArcs[ArcCount].StartAngle = (double)(0 + AngleStepFactor * Shapecount) + SpaceBetweenShapes;
                        LightArcs[ArcCount].EndAngle = (double)(AngleStepFactor * (Shapecount + 1)) - SpaceBetweenShapes;
                    }
                    break;
            }
            LightArcs[ArcCount].Radius = (int)Math.Abs((this.RenderSize.Width - LightArcs[ArcCount].Thickness * (2.5 * CircleCount)) / 2);
            LightArcs[ArcCount].Text = ZonesIds_TowardsCenterAntiClockWise[ArcCount].ToString();
            LightArcs[ArcCount].DrawnArcText.Text = ZonesNames_TowardsCenterAntiClockWise[ArcCount];
            LightArcs[ArcCount].Tag = new Point(ZonesIds_TowardsCenterAntiClockWise[ArcCount], CircleCount);
            LightArcs[ArcCount].BaseColor = GenerateBaseBrush(LightArcs[ArcCount].Radius, LightArcs[ArcCount].Thickness, ArcCount);
            LightArcs[ArcCount].FillColor = GenerateFillBrush(LightArcs[ArcCount].Radius, LightArcs[ArcCount].Thickness, ArcCount);
            LightArcs[ArcCount].DrawTicks(this.RenderSize.Width / 2, 16, 4);
        }
        //Handling the Scroll of Mouse
        void LightingGrid_PreviewMouseWheel(object sender, System.Windows.Input.MouseWheelEventArgs e)
        {
            SingleLightingZone l = (SingleLightingZone)sender;
            int currentValue = Values[((int)((Point)l.Tag).X - 1)];
            if (e.Delta >= 120)
            {
                if (currentValue < l.MaximumValue)
                    currentValue += l.GetStepValue;
            }
            else
            {
                if (currentValue > l.MinimumValue)
                    currentValue -= l.GetStepValue;
            }
            if (this.IsLocked && ((Point)l.Tag).Y == 0)
            {
                for (int i = 0; i < 4; i++)
                    Values[i] = currentValue;
            }
            else
                Values[((int)((Point)l.Tag).X - 1)] = currentValue;

            if (ValueChanged != null)
                ValueChanged(this, null);
        }
        //Handling the click of mose on any zone
        void LightingGrid_PreviewMouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            SingleLightingZone l = (SingleLightingZone)sender;
            //((Math.Atan(e.GetPosition().Y / e.GetPosition.X)) * 180 / Math.PI) - l.StartAngle;
            double RelativeMouseX = e.GetPosition(l).X - l.ActualWidth / 2;
            double RelativeMouseY = -(e.GetPosition(l).Y - l.ActualHeight / 2);
            double MousePosAngle = Math.Atan(RelativeMouseY / RelativeMouseX) * 180 / Math.PI;

            if (RelativeMouseX > 0 && MousePosAngle < 0)
                MousePosAngle = 360 + MousePosAngle;
            else if (RelativeMouseX < 0)
                MousePosAngle = 180 + MousePosAngle;

            double TmpStrtAng = l.StartAngle < 0 ? 360 + l.StartAngle : l.StartAngle;
            if (l.StartAngle < 0 && l.EndAngle > 0)
            {
                TmpStrtAng = MousePosAngle < TmpStrtAng ? TmpStrtAng - 360 : TmpStrtAng;
            }
            else if (l.StartAngle > 0 && l.EndAngle < 0)
            {
                TmpStrtAng = MousePosAngle > TmpStrtAng ? TmpStrtAng + 360 : TmpStrtAng;
            }
            //double TmpStrtAng1 = (l.StartAngle < 0 && l.EndAngle > 0) ? : (l.StartAngle > 0 && l.EndAngle < 0) ? : ;
            int currentValue = (int)Math.Round((MousePosAngle - TmpStrtAng) / (l.EndAngle - l.StartAngle) * (l.MaximumValue - l.MinimumValue) / l.GetStepValue) * l.GetStepValue + l.MinimumValue;

            if (this.IsLocked && ((Point)l.Tag).Y == 0)
            {
                for (int i = 0; i < 4; i++)
                    Values[i] = currentValue;
            }
            else
                Values[((int)((Point)l.Tag).X - 1)] = currentValue;

            MouseDrag = true;

            if (ValueChanged != null)
                ValueChanged(this, null);
        }
        //Handling mouse drag to change the value
        void LightingGrid_PreviewMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (MouseDrag)
            {
                SingleLightingZone l = (SingleLightingZone)sender;
                //((Math.Atan(e.GetPosition().Y / e.GetPosition.X)) * 180 / Math.PI) - l.StartAngle;
                double RelativeMouseX = e.GetPosition(l).X - l.ActualWidth / 2;
                double RelativeMouseY = -(e.GetPosition(l).Y - l.ActualHeight / 2);
                double MousePosAngle = Math.Atan(RelativeMouseY / RelativeMouseX) * 180 / Math.PI;
                MousePosAngle = Math.Round(MousePosAngle);
                if (RelativeMouseX > 0 && MousePosAngle < 0)
                    MousePosAngle = 360 + MousePosAngle;
                else if (RelativeMouseX < 0)
                    MousePosAngle = 180 + MousePosAngle;

                double TmpStrtAng = l.StartAngle < 0 ? 360 + l.StartAngle : l.StartAngle;
                if (l.StartAngle < 0 && l.EndAngle > 0)
                {
                    TmpStrtAng = MousePosAngle < TmpStrtAng ? TmpStrtAng - 360 : TmpStrtAng;
                }
                else if (l.StartAngle > 0 && l.EndAngle < 0)
                {
                    TmpStrtAng = MousePosAngle > TmpStrtAng ? TmpStrtAng + 360 : TmpStrtAng;
                }
                //double TmpStrtAng1 = (l.StartAngle < 0 && l.EndAngle > 0) ? : (l.StartAngle > 0 && l.EndAngle < 0) ? : ;
                int currentValue = (int)Math.Round((MousePosAngle - TmpStrtAng) / (l.EndAngle - l.StartAngle) * (l.MaximumValue - l.MinimumValue) / l.GetStepValue) * l.GetStepValue + l.MinimumValue;

                if (currentValue > l.MaximumValue)
                    currentValue = l.MaximumValue;

                if (this.IsLocked && ((Point)l.Tag).Y == 0)
                {
                    for (int i = 0; i < 4; i++)
                        Values[i] = currentValue;
                }
                else
                    Values[((int)((Point)l.Tag).X - 1)] = currentValue;

                if (ValueChanged != null)
                    ValueChanged(this, null);
            }
        }
        //Handling mouse middle click to reset the lighting zone
        void LightingGrid_MouseDown(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (e.MiddleButton == System.Windows.Input.MouseButtonState.Pressed)
            {
                SingleLightingZone l = (SingleLightingZone)sender;
                if (this.IsLocked && ((Point)l.Tag).Y == 0)
                {
                    for (int i = 0; i < 4; i++)
                        Values[i] = 0;
                }
                else
                    Values[((int)((Point)l.Tag).X - 1)] = 0;

                if (ValueChanged != null)
                    ValueChanged(this, null);
            }
        }
        //To disable drag, handling mouse left up
        void LightingGrid_PreviewMouseLeftButtonUp(object sender, System.Windows.Input.MouseEventArgs e)
        {
            MouseDrag = false;
        }
        public void LightingGrid_PreviewMouseRightButtonDown(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                SingleLightingZone b = (SingleLightingZone)sender;
                string[] tagStr = (string[])b.Tag;
                string BtnName = tagStr[2];
                string BtnLocationName = tagStr[0];
                if (HelpWindow.GetObj().ShowTopic(BtnLocationName, BtnName))
                    HelpWindow.GetObj().Show();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS07a", ex); }
        }
        //This functions is called and values are passed which in turn will update the Zone values.
        public void UpdateValues(int[] NewValues)
        {
            try
            {
                if (NewValues != null && NewValues.Length >= Values.Length)
                {
                    int index = GlobalSettings.LightZonesForThisController - 1; //= MyLighting.TotalCircles - 4;
                    if (index > 7)
                    {
                        if (IsCoAxialLightingPresent)
                            index = Values.Length - 4;
                        else
                            index = Values.Length - 3;
                        for (int i = 0; i < index; i++)
                        {
                            Values[i] = NewValues[i];
                            LightArcs[i].Value = Values[i];
                        }
                        int ct = 0;
                        for (int i = index; i < Values.Length; i++)
                        {
                            Values[i] = NewValues[GlobalSettings.Light_Zone_Nos[ct++]];
                            if (i < LightArcs.Length)
                                LightArcs[i].Value = Values[i];
                            //if (ct == 3)
                        }
                    }
                    else
                    {
                        for (int i = 0; i < Values.Length; i++)
                        {
                            Values[i] = NewValues[i];
                            if (i < LightArcs.Length)
                               LightArcs[i].Value = Values[i];
                        }
                    }
                }
            }
            catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI09a", ex); }
        }

        #region Properties
        //The total no of circles
        public int TotalCircles { get; set; }
        //Total no of circles/Zone.
        public int[] NoOfZonesPerCircle_TowardsCenter { get; set; }
        //The thickness of the Arcs
        public double ShapeThickness { get; set; }
        //the Values array containing individual values of all zones.
        public int[] Values { get; set; }
        //The no. that has to be displayed on each & every zone.
        public int[] ZonesIds_TowardsCenterAntiClockWise { get; set; }
        //The Text that has to be displayed on each & every zone.
        public string[] ZonesNames_TowardsCenterAntiClockWise { get; set; }
        public LightingShapesTypes OuterShapeType { get; set; }
        public DirectionOfDrawing[] LightingDrawingDirection { get; set; }
        public bool IsCoAxialLightingPresent { get; set; }
        public bool IsAuxillaryLightingPresent { get; set; }
        public bool OneShotLighting { get; set; }
        //This property is added juts to retain current requirements.
        //Essentially this should be expanded to give user options to select which zones to select.
        //Remember this property is just a tradeoff currently.<Not recommened>
        public bool IsLocked { get; set; }
        private System.Windows.Media.Brush GenerateFillBrush(double radius, double thickness, int shapecount)
        {
            System.Windows.Media.SolidColorBrush rr = new System.Windows.Media.SolidColorBrush();
            rr.Color = System.Windows.Media.Color.FromArgb(128, 255, 255, 255);
            return rr;

            //System.Windows.Media.RadialGradientBrush r = new System.Windows.Media.RadialGradientBrush();
            //r.MappingMode = System.Windows.Media.BrushMappingMode.Absolute;
            //r.RadiusX = radius;
            //r.RadiusY = radius;
            //r.Center = new Point(radius, radius);
            //r.GradientOrigin = new Point(radius, radius);

            //if (shapecount <= 3)
            //{
            //    System.Windows.Media.GradientStop gr = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(100, 0,0,0), 0.7);
            //    System.Windows.Media.GradientStop gr2 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 80, 180, 255), 0.85);
            //    System.Windows.Media.GradientStop gr3 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 80, 180, 255), 1);
            //    r.GradientStops.Add(gr);
            //    r.GradientStops.Add(gr3);
            //    r.GradientStops.Add(gr2);
            //}
            //else
            //{
            //    if (!(this.RenderSize.Height <= 190))
            //    {
            //        System.Windows.Media.GradientStop gr = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(100, 0, 0, 0), 0.43);
            //        System.Windows.Media.GradientStop gr2 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 80, 180, 255), 0.721);
            //        System.Windows.Media.GradientStop gr3 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255,80, 180, 255 ), 1);
            //        r.GradientStops.Add(gr);
            //        r.GradientStops.Add(gr3);
            //        r.GradientStops.Add(gr2);
            //    }
            //    else
            //    {
            //        System.Windows.Media.GradientStop gr = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(20,0, 0, 0 ), 0.60);
            //        System.Windows.Media.GradientStop gr2 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 80, 180, 255), 0.84);
            //        System.Windows.Media.GradientStop gr3 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 80, 180, 255), 1);
            //        r.GradientStops.Add(gr);
            //        r.GradientStops.Add(gr3);	
            //        r.GradientStops.Add(gr2);
            //    }
            //}
            //return r;
        }
        private System.Windows.Media.Brush GenerateBaseBrush(double radius, double thickness, int shapecount)
        {
            System.Windows.Media.RadialGradientBrush r = new System.Windows.Media.RadialGradientBrush();
            double fullsize = radius;
            double innersize = (radius - this.ShapeThickness) / fullsize;
            double borderwidth = 1 / fullsize;

            r.MappingMode = System.Windows.Media.BrushMappingMode.Absolute;
            r.RadiusX = radius;
            r.RadiusY = radius;
            r.Center = new Point(radius, radius);
            r.GradientOrigin = new Point(radius, radius);
            //if (shapecount <= 3)
            //{   
            System.Windows.Media.GradientStop gr1 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 255, 255, 255), innersize);
            System.Windows.Media.GradientStop gr2 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 255, 255, 255), innersize + borderwidth);
            System.Windows.Media.GradientStop gr3 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(175, 64, 132, 196), innersize + borderwidth);
            System.Windows.Media.GradientStop gr4 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(175, 64, 132, 196), 1 - borderwidth);
            System.Windows.Media.GradientStop gr5 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 255, 255, 255), 1 - borderwidth);
            System.Windows.Media.GradientStop gr6 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 255, 255, 255), 1);
            r.GradientStops.Add(gr1);
            r.GradientStops.Add(gr2);
            r.GradientStops.Add(gr3);
            r.GradientStops.Add(gr4);
            r.GradientStops.Add(gr5);
            r.GradientStops.Add(gr6);
            //}
            //else
            //{
            //    System.Windows.Media.GradientStop gr = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 0, 124, 212), 0.52);
            //    System.Windows.Media.GradientStop gr2 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(128, 255, 255, 255), 0.66);
            //    System.Windows.Media.GradientStop gr3 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(128, 255, 255, 255), 0.84);
            //    System.Windows.Media.GradientStop gr4 = new System.Windows.Media.GradientStop(System.Windows.Media.Color.FromArgb(255, 0, 124, 212), 1);
            //    r.GradientStops.Add(gr);
            //    r.GradientStops.Add(gr3);
            //    r.GradientStops.Add(gr4);
            //    r.GradientStops.Add(gr2);
            //}


            return r;
        }
        #endregion
    }

    //The Arc corner types.
    public enum LightingShapesTypes
    {
        Round = 0,
        Triangle = 1,
        Flat = 2
    };
    //The direction of drawing.
    public enum DirectionOfDrawing
    {
        ClockWise = 0,
        CounterClockWise = 1,
        ClockWiseWithOffset = 2,
        CounterClockWiseWithOffset = 3
    };
}