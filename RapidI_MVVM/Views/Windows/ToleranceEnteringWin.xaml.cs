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
	public partial class ToleranceEnteringWin : Window,ISaveWindowsParameters
	{
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
        public bool DistMode { get { return InternalDistMode; } set { InternalDistMode = value; ShowHideRBtns(!value); } }
        private bool InternalDistMode = false;
                
        public double Nom, UpTol, LoTol;

        //Constructor
        public ToleranceEnteringWin()
		{
			this.InitializeComponent();
            ReadParameters();
            init();
		}
        void init()
        {
            UpTolTxtBx.TextChanged += new TextChangedEventHandler(TolTxtBx_TextChanged);
            SymmetricTolChkBx.Click += new RoutedEventHandler(SymmetricTolChkBx_Click);
        }

        //On ok click just return the dialog result as true.
        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            if (CheckEnteredVals())
            {
                this.Result = true;
                NomTxtBx.Text = "";
                UpTolTxtBx.Text = "";
                LoTolTxtBx.Text = "";
                this.Close();
            }
        }
        //On cancel click  just return the dialog result as false.
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Result = false;
            NomTxtBx.Text = "";
            UpTolTxtBx.Text = "";
            LoTolTxtBx.Text = "";
            this.Close(); 
        }
        //if user presses enter key in textbox just make the dialog result to be true.
        private void txtNewName_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                if (CheckEnteredVals())
                {
                    this.Result = true;
                    e.Handled = true;
                    NomTxtBx.Text = "";
                    UpTolTxtBx.Text = "";
                    LoTolTxtBx.Text = "";
                    this.Close();
                }
            }
            else if (e.Key == Key.Escape)
            {
                this.Result = false;
                e.Handled = true;
                NomTxtBx.Text = "";
                UpTolTxtBx.Text = "";
                LoTolTxtBx.Text = "";
                this.Close();
            }
        }
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            SaveParameters();
            this.Hide();
            e.Cancel = true;
        }
        private void TolTxtBx_TextChanged(object sender, TextChangedEventArgs e)
        {
            if ((bool)SymmetricTolChkBx.IsChecked)
                LoTolTxtBx.Text = "-" + UpTolTxtBx.Text;
        }
        private bool CheckEnteredVals()
        {
            if (!DistMode && (bool)Deg_Min_SecRBtn.IsChecked)
            {
                string[] SplitStr;
                int ReturnVal;
                double TmpVal = 0;

                //Nominal Value
                SplitStr = NomTxtBx.Text.ToString().Split(',');
                for (int i = 0; i < SplitStr.Length; i++)
                {
                    if (i == 3) break;
                    if (!int.TryParse(SplitStr[i], out ReturnVal))
                    {
                        MessageBox.Show("Please check the format in which you have entered the values.", "Rapid-I");
                        return false;
                    }
                    if (i > 0)
                        TmpVal = Math.Abs(TmpVal); //-ve sign can only be entered for the degree term, not for min or sec.
                    TmpVal += ReturnVal / Math.Pow(60, i);
                }
                Nom = TmpVal * Math.PI / 180;

                //Upper Tolerance Value
                TmpVal = 0;
                SplitStr = UpTolTxtBx.Text.ToString().Split(',');
                for (int i = 0; i < SplitStr.Length; i++)
                {
                    if (i == 3) break;
                    if (!int.TryParse(SplitStr[i], out ReturnVal))
                    {
                        MessageBox.Show("Please check the format in which you have entered the values.", "Rapid-I");
                        return false;
                    }
                    TmpVal += Math.Abs(ReturnVal) / Math.Pow(60, i); //Tolerance values will always be +ve
                }
                UpTol = TmpVal * Math.PI / 180;

                //Lower Tolerance Value
                TmpVal = 0;
                SplitStr = LoTolTxtBx.Text.ToString().Split(',');
                for (int i = 0; i < SplitStr.Length; i++)
                {
                    if (i == 3) break;
                    if (!int.TryParse(SplitStr[i], out ReturnVal))
                    {
                        MessageBox.Show("Please check the format in which you have entered the values.", "Rapid-I");
                        return false;
                    }
                    TmpVal += Math.Abs(ReturnVal) / Math.Pow(60, i); //Tolerance values will always be +ve
                }
                LoTol = TmpVal * Math.PI / 180;
                return true;
            }
            else
            {
                if (!double.TryParse(NomTxtBx.Text.ToString(), out Nom) || !double.TryParse(UpTolTxtBx.Text.ToString(), out UpTol) || !double.TryParse(LoTolTxtBx.Text.ToString(), out LoTol))
                {
                    MessageBox.Show("Please check the format in which you have entered the values.", "Rapid-I");
                    return false;
                }

                //if Decimal Degree
                if (!DistMode && (bool)DecimalDegreeRBtn.IsChecked)
                {
                    Nom = Nom * Math.PI / 180;
                    UpTol = Math.Abs(UpTol) * Math.PI / 180; ;
                    LoTol = Math.Abs(LoTol) * Math.PI / 180; ;
                }
                else
                {
                    //Form both distance mode and radian angle mode
                    UpTol = Math.Abs(UpTol);
                    LoTol = Math.Abs(LoTol);
                }
                return true;
            }
        }
        private void ShowHideRBtns(bool Visibile)
        {
            if (Visibile)
            {
                Deg_Min_SecRBtn.Visibility = System.Windows.Visibility.Visible;
                DecimalDegreeRBtn.Visibility = System.Windows.Visibility.Visible;
                RadianRBtn.Visibility = System.Windows.Visibility.Visible;
            }
            else
            {
                Deg_Min_SecRBtn.Visibility = System.Windows.Visibility.Hidden;
                DecimalDegreeRBtn.Visibility = System.Windows.Visibility.Hidden;
                RadianRBtn.Visibility = System.Windows.Visibility.Hidden;
            }
        }
        private void SymmetricTolChkBx_Click(object sender, RoutedEventArgs e)
        {
            if ((bool)SymmetricTolChkBx.IsChecked)
            {
                LoTolTxtBx.IsEnabled = false;
                LoTolTxtBx.Text = "-" + UpTolTxtBx.Text;
            }
            else
                LoTolTxtBx.IsEnabled = true;
        }
        public void SetAngleMode()
        {
            switch (GlobalSettings.CurrentAngleMode)
            {
                case GlobalSettings.AngleMode.Degree_Minute_Second:
                    Deg_Min_SecRBtn.IsChecked = true;
                    break;
                case GlobalSettings.AngleMode.Decimal_Degree:
                    DecimalDegreeRBtn.IsChecked = true;
                    break;
                case GlobalSettings.AngleMode.Radians:
                    RadianRBtn.IsChecked = true;
                    break;
                default:
                    Deg_Min_SecRBtn.IsChecked = true;
                    break;
            }
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ToleranceEnteringWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("SymmetricTol", SymmetricTolChkBx.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Deg_Min_Sec", Deg_Min_SecRBtn.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("DecimalDegree", DecimalDegreeRBtn.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Radian", RadianRBtn.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Nominal", NomTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("UpTol", UpTolTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("LoTol", LoTolTxtBx.Text.ToString()));                        
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "SymmetricTol")
                                {
                                    SymmetricTolChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "Deg_Min_Sec")
                                {
                                    Deg_Min_SecRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "DecimalDegree")
                                {
                                    DecimalDegreeRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "Radian")
                                {
                                    RadianRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "Nominal")
                                {
                                    NomTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "UpTol")
                                {
                                    UpTolTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "LoTol")
                                {
                                    LoTolTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ToleranceEnteringWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("SymmetricTol", SymmetricTolChkBx.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Deg_Min_Sec", Deg_Min_SecRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("DecimalDegree", DecimalDegreeRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Radian", RadianRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Nominal", NomTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("UpTol", UpTolTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("LoTol", LoTolTxtBx.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters = null;
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