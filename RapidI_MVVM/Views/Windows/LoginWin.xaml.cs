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
using CalibrationModule;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for LoginWin.xaml
	/// </summary>
	public partial class LoginWin : OpenGlWin
	{
		public bool InfoStatus = false;
		public string LoggedInUser;
		DB Settings_DataB;
		public List<string> LoginDetailsNameColl, PrevilageLevelColl, PasswordColl;
		public LoginWin()
		{
			try
			{
				this.InitializeComponent();
				this.WindowState = System.Windows.WindowState.Maximized;
				this.btnOK.Click += new RoutedEventHandler(btnOK_Click);
				// Insert code required on object creation below this point.
				init();
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LGNWIN01", ex); }
		}
		void init()
		{
			try
			{
				LoginDetailsNameColl = new List<string>();
				PrevilageLevelColl = new List<string>();
				PasswordColl = new List<string>();

				Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
				Settings_DataB.FillTable("Login");
				int RowC = Settings_DataB.GetRowCount("Login");
				for (int i = 0; i < RowC; i++)
				{
					LoginDetailsNameColl.Add(Settings_DataB.get_GetRecord("Login", i, "Login_Name").ToString());
					PrevilageLevelColl.Add(Settings_DataB.get_GetRecord("Login", i, "Login_Priviledge").ToString());
					PasswordColl.Add(Settings_DataB.get_GetRecord("Login", i, "Login_Password").ToString());
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LGNWIN02", ex); }
		}
		public string getPassword(string uname)
		{
			return PasswordColl[LoginDetailsNameColl.IndexOf(uname)];
		}
		public string getPrevilegeLevel(string uname)
		{
			return PrevilageLevelColl[LoginDetailsNameColl.IndexOf(uname)];
		}
		//On ok click just return the dialog result as true.
		private void btnOK_Click(object sender, RoutedEventArgs e)
		{ this.InfoStatus = true; this.Hide(); }
		//On cancel click  just return the dialog result as false.
		private void btnCancel_Click(object sender, RoutedEventArgs e)
		{ this.InfoStatus = false; this.Hide(); }
		//if user presses enter key in textbox just make the dialog result to be true.
		private void txtPassword_PreviewKeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter) 
			{
				this.InfoStatus = true;
				this.Hide();
				e.Handled = true;
			}
		}
		//when the user clicks on the user tablet button
		public void HandleUserTabletClick(object sender, RoutedEventArgs e)
		{
			LoggedInUser = ((RadioButton)sender).Tag.ToString();
		}
	}
}