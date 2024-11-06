using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Rapid.Interface;
using Rapid.IO;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for ShapeInfoWin.xaml
    /// </summary>
    public partial class LineArcPropertiesWin : Window
    {
        //Constructor
        public LineArcPropertiesWin()
        {
            try
            {
                this.InitializeComponent();
                ReadParameters();
                init();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN01", ex); }
        }
        void init()
        {
            try
            {
                TolFactorSlider.Value = 3;
                NoiseFactorSlider.Value = 3;
                MaxDistCutoffSlider.Value = 1;
                MinAngleCutoffSlider.Value = 5;

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    MaxRadiusSlider.Value = 15;
                    MinRadiusSlider.Value = 0.05;
                }
                else
                {
                    MaxRadiusSlider.Value = 2;
                    MinRadiusSlider.Value = 0.01;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN02", ex); }
        }

        //Function to Set the initial Values
        public void SetInitialValues(double Tolerance, double Noise, double Maxradius, double MinRadius, double MaxDistCutoff, double Angle, bool ClosedLoop, bool ShpAsFasTrace, bool JoinAllPts)
        {
            try
            {
                TolFactorSlider.Value = Tolerance;
                NoiseFactorSlider.Value = Noise;
                MaxRadiusSlider.Value = Maxradius;
                MinRadiusSlider.Value = MinRadius;
                MaxDistCutoffSlider.Value = MaxDistCutoff;
                MinAngleCutoffSlider.Value = Angle * 180 / Math.PI;
                ClosedLoopChkBx.IsChecked = ClosedLoop;
                ShpAsFasTraceChkBx.IsChecked = ShpAsFasTrace;
                JoinAllPtsChkBx.IsChecked = JoinAllPts;

                TolFactorSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                NoiseFactorSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                MaxRadiusSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                MinRadiusSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                MaxDistCutoffSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                MinAngleCutoffSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);

                ClosedLoopChkBx.Checked += new RoutedEventHandler(ChkBx_CheckedChange);
                ClosedLoopChkBx.Unchecked += new RoutedEventHandler(ChkBx_CheckedChange);
                ShpAsFasTraceChkBx.Checked += new RoutedEventHandler(ChkBx_CheckedChange);
                ShpAsFasTraceChkBx.Unchecked += new RoutedEventHandler(ChkBx_CheckedChange);
                JoinAllPtsChkBx.Checked += new RoutedEventHandler(ChkBx_CheckedChange);
                JoinAllPtsChkBx.Unchecked += new RoutedEventHandler(ChkBx_CheckedChange);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN03", ex); }
        }
        //On apply click - apply the values to framework
        void UpdateValueChange()
        {
            try
            {
                double TmpToleranceVal = TolFactorSlider.Value,
                TmpNoiseVal = NoiseFactorSlider.Value,
                TmpMinAngVal = MinAngleCutoffSlider.Value * Math.PI / 180,
                TmpMinRadiusVal = MinRadiusSlider.Value,
                TmpMaxRadiusVal = MaxRadiusSlider.Value,
                TmpMaxDistCutoff = MaxDistCutoffSlider.Value;

                RWrapper.RW_MainInterface.MYINSTANCE().GetLineArcDiffShapes(TmpToleranceVal, TmpMaxRadiusVal, TmpMinRadiusVal, TmpMinAngVal, TmpNoiseVal, TmpMaxDistCutoff,
                    (bool)ClosedLoopChkBx.IsChecked, (bool)ShpAsFasTraceChkBx.IsChecked, (bool)JoinAllPtsChkBx.IsChecked);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN03", ex); }
        }
        //Handling all slider value changes
        void Slider_ValueChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                UpdateValueChange();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN04", ex); }
        }
        //Handling all checkbox checked property changed
        void ChkBx_CheckedChange(object sender, RoutedEventArgs e)
        {
            try
            {
                UpdateValueChange();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN04", ex); }
        }
        //On ok click
        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().AddLineArcDiffShapes();
                this.Close();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN04", ex); }
        }
        //On Cancel click
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().CancelLineArc();
                this.Close();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LAWIN06", ex); }
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            SaveParameters();
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\LineArcPropertiesWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("TolFactor", TolFactorSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NoiseFactor", NoiseFactorSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("MaxRadius", MaxRadiusSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("MinRadiusSlider", MinRadiusSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("MaxDistCutoff", MaxDistCutoffSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("MinAngleCutoff", MinAngleCutoffSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ClosedLoopChkBx", ClosedLoopChkBx.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ShpAsFasTraceChkBx", ShpAsFasTraceChkBx.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("JoinAllPtsChkBx", JoinAllPtsChkBx.IsChecked.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "TolFactor")
                                {
                                    TolFactorSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "NoiseFactor")
                                {
                                    NoiseFactorSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "MaxRadius")
                                {
                                    MaxRadiusSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "MinRadiusSlider")
                                {
                                    MinRadiusSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "MaxDistCutoff")
                                {
                                    MaxDistCutoffSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "MinAngleCutoff")
                                {
                                    MinAngleCutoffSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "ClosedLoopChkBx")
                                {
                                    ClosedLoopChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "MinAngleCutoff")
                                {
                                    ShpAsFasTraceChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "MinAngleCutoff")
                                {
                                    JoinAllPtsChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception)
            {

            }
        }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\LineArcPropertiesWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("TolFactor", TolFactorSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NoiseFactor", NoiseFactorSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MaxRadius", MaxRadiusSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MinRadiusSlider", MinRadiusSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MaxDistCutoff", MaxDistCutoffSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MinAngleCutoff", MinAngleCutoffSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ClosedLoopChkBx", ClosedLoopChkBx.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ShpAsFasTraceChkBx", ShpAsFasTraceChkBx.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("JoinAllPtsChkBx", JoinAllPtsChkBx.IsChecked.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        #endregion
    }
}