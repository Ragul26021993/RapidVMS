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

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for ShapeInfoWin.xaml
    /// </summary>
    public partial class SurfaceFGEntityWin : Window
    {
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
        public string SelectedEntity = "";
        public bool ShapeSelected = false;
        public SurfaceFGEntityWin()
        {
            this.InitializeComponent();

            // Insert code required on object creation below this point.
        }
        //On ok click just return the dialog result as true.
        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            string Entityname;
            if (!ShapeSelected)
                Entityname = ((RadioButton)EntityList.Children[0]).Content.ToString();
            else
                Entityname = SelectedEntity;
            RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED(Entityname);
            this.Result = true;
            this.Close();
        }
        //On cancel click  just return the dialog result as false.
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().CancelPressed();
            this.Result = false;
            this.Close();
        }
        //On cancel click  just return the dialog result as false.
        private void SaveFGTypeBtn_Click(object sender, RoutedEventArgs e)
        {
            if (!ShapeSelected)
                RWrapper.RW_MainInterface.MYINSTANCE().SetSurfaceAloType(0);
            else
            {
                for (int i = 0; i < EntityList.Children.Count; i++)
                    if ((bool)((RadioButton)EntityList.Children[i]).IsChecked)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetSurfaceAloType(i);
                        RWrapper.RW_MainInterface.MYINSTANCE().CancelPressed();
                        break;
                    }
            }
            this.Result = false;
            this.Close();
        }
        //if user presses enter key in textbox just make the dialog result to be true.
        private void txtNewName_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                this.Result = true;
                e.Handled = true;
                this.Close();
            }
            else if (e.Key == Key.Escape)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().CancelPressed();
                this.Result = false;
                e.Handled = true;
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
            this.Hide();
            e.Cancel = true;
        }
        public void AddEntities(string EntityStr)
        {
            try
            {
                string[] AllEntities = EntityStr.Split(',');
                for (int i = 0; i < AllEntities.Length; i++)
                {
                    RadioButton rb = new RadioButton();
                    rb.Height = 40;
                    rb.Width = 40;
                    rb.Margin = new Thickness(2, 0, 2, 0);
                    rb.Style = GlobalSettings.RadioButtonStyle;
                    rb.Content = AllEntities[i];
                    rb.Checked += new RoutedEventHandler(EntityRBtn_Checked);
                    EntityList.Children.Add(rb);
                }
            }
            catch (Exception)
            {
                throw;
            }
        }
        public void ResetWindow()
        {
            try
            {
                ShapeSelected = false;
                SelectedEntity = "";
                EntityList.Children.Clear();
            }
            catch (Exception)
            {
                throw;
            }
        }
        void EntityRBtn_Checked(object sender, RoutedEventArgs e)
        {
            ShapeSelected = true;
            SelectedEntity = ((RadioButton)sender).Content.ToString();
            RWrapper.RW_MainInterface.MYINSTANCE().HighlightUserChosenSurfaceED(SelectedEntity);
        }
    }
}