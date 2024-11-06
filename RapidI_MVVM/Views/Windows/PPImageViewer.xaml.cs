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
using System.Data.OleDb;
using System.Data;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for PPIMAGEVIEWER.xaml
    /// </summary>
    public partial class PPImageViewer : Window
    {
        double UnitWidth, UnitHeight;
        PPreviewWin PPPreview;
        string ConnectionString;
        System.Collections.Hashtable PartProgramNameToTextfileRelation;
        System.Collections.Hashtable ImageFolderNameToDirectoryRelation;
        string imageDirectoryLocation;
        string textfile;
        List<string> ProgramRun;
        ContextMenu Contextmenustrip;
        Border SelectedRectangle;
        bool ShowThumbnailImmage = false;
        public PPImageViewer()
        {
            if (System.Environment.Is64BitOperatingSystem)
                ConnectionString = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + @"\Rapid-I 5.0\Database\ExcelDoc.mdb;Persist Security Info=False;Jet OLEDB:Database Password=excelforctpl";
            else
                ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + @"\Rapid-I 5.0\Database\ExcelDoc.mdb;Persist Security Info=False;Jet OLEDB:Database Password=excelforctpl";

            this.InitializeComponent();
            this.WindowStartupLocation = WindowStartupLocation.CenterScreen;
            this.WindowState = WindowState.Maximized;
            ImageFolderNameToDirectoryRelation = new System.Collections.Hashtable();
            PartProgramNameToTextfileRelation = new System.Collections.Hashtable();
            SelectedRectangle = new Border();
            Contextmenustrip = new System.Windows.Controls.ContextMenu();
            ContextMenuContents(ref Contextmenustrip);
            ProgramRun = new List<string>();
            LoadProgramReportNames(true);
            ImageGrid.SizeChanged += new SizeChangedEventHandler(ImageGrid_SizeChanged);
            CBShowThumbnail.Click += new RoutedEventHandler(CBShowThumbnail_Click);
        }

        void CalculateRectangles(double GridWidth, double GridHeight, string textfilepath, string imageFilePath)
        {
            try
            {
                int distinctX, distinctY;
                ImageGrid.Children.Clear();
                UnitWidth = 20;
                UnitHeight = UnitWidth;
                List<ImagePP> PPImageCount = new List<ImagePP>();
                readImagetxt(textfilepath, ref PPImageCount);
                distinctX = distinctvalues(PPImageCount, 'x');
                distinctY = distinctvalues(PPImageCount, 'y');
                UnitWidth = GridWidth / distinctX;
                UnitHeight = GridHeight / distinctY;
                DataTable dt = new DataTable();
                dt = GetProgramRunRelatedData(PartProgramList.SelectedItem.ToString(), PPRunList.SelectedItem.ToString());
                if (dt.Rows.Count > 0)
                {
                    for (int i = 0; i < PPImageCount.Count; i++)
                    {
                        Brush color;
                        DataRow[] dr;
                        dr = dt.Select("ImageName" + "=" + "'" + (PPImageCount[i].imagename) + "'");
                        PPImageCount[i].accepted = Convert.ToBoolean(dr[0]["Accepted"]);
                        PPImageCount[i].rejected = Convert.ToBoolean(dr[0]["Rejected"]);
                        PPImageCount[i].imagename = imageFilePath + "\\" + PPImageCount[i].imagename + ".jpg";

                        if (PPImageCount[i].accepted)
                        {
                            var bc = new BrushConverter();
                            color = (Brush)bc.ConvertFrom("#98FB98");
                        }
                        else if (PPImageCount[i].rejected)
                            color = System.Windows.Media.Brushes.Red;
                        else
                            color = System.Windows.Media.Brushes.White;
                        ImageGrid.Children.Add(AddRectangle(color, UnitWidth, UnitHeight, PPImageCount[i].XMultiplicationFactor, PPImageCount[i].YmultiplicationFactor, GridWidth, GridHeight, PPImageCount[i].imagename, PartProgramList.SelectedItem.ToString(), PPRunList.SelectedItem.ToString(), ShowThumbnailImmage));
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        Border AddRectangle(Brush color, double CellWidth, double CellHeight, double HorrMulFactor, double VerticalMulFactor, double ContainerWidth, double ContainerHeight, string ImagePath, string PartProgramName, string RunName, bool showThumbnail = false)
        {
            double BorderToWidthRatiovalue;
            Border Bdr = new Border();
            BorderToWidthRatiovalue = Math.Sqrt(CellWidth * CellHeight) * 3 / 100;
            Bdr.BorderThickness = new Thickness(BorderToWidthRatiovalue);
            Bdr.BorderBrush = new SolidColorBrush(Colors.Gray);
            Bdr.Width = CellWidth;
            Bdr.Height = CellHeight;
            Bdr.Margin = new Thickness((ContainerWidth - CellWidth) * HorrMulFactor, (ContainerHeight - CellHeight) * (1 - VerticalMulFactor), 0, 0);
            Bdr.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
            Bdr.VerticalAlignment = System.Windows.VerticalAlignment.Top;

            TextBlock tb = new TextBlock();
            tb.Text = ImagePath.Substring(ImagePath.LastIndexOf("\\") + 1, (ImagePath.LastIndexOf(".") - ImagePath.LastIndexOf("\\") - 1));
            tb.Margin = new Thickness(5, -2, 0, 0);

            Canvas cnvas = new Canvas();
            cnvas.Children.Add(tb);
            cnvas.Background = color;
            cnvas.Width = Bdr.Width - 2 * BorderToWidthRatiovalue;
            cnvas.Height = Bdr.Height - 2 * BorderToWidthRatiovalue;
            cnvas.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
            cnvas.VerticalAlignment = System.Windows.VerticalAlignment.Top;

            Image ThumbnailImage = new Image();
            ThumbnailImage.Height = cnvas.Height;
            ThumbnailImage.Width = cnvas.Width;

            BitmapImage newImage = new BitmapImage();
            newImage.BeginInit();
            newImage.UriSource = new Uri(ImagePath, UriKind.Absolute);
            newImage.EndInit();
            ThumbnailImage.Source = newImage;
            ThumbnailImage.VerticalAlignment = System.Windows.VerticalAlignment.Center;
            if (showThumbnail == true)
                cnvas.Children.Add(ThumbnailImage);

            CellInformaton CI = new CellInformaton();
            CI.Imagename = tb.Text;
            CI.x = (ContainerWidth - CellWidth) * HorrMulFactor;
            CI.y = (1 - (ContainerHeight - CellHeight)) * VerticalMulFactor;
            CI.ImagePath = ImagePath;
            CI.PartProgramName = PartProgramName;
            CI.RunName = RunName;
            CI.color = color;

            Bdr.Tag = CI;
            //Bdr.ContextMenu = Contextmenustrip;
            Bdr.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(rect_MouseLeftButtonDown);
            Bdr.MouseRightButtonDown += new MouseButtonEventHandler(Bdr_MouseRightButtonDown);
            Bdr.MouseEnter += new MouseEventHandler(rect_MouseEnter);
            Bdr.Child = cnvas;
            return Bdr;
        }

        void rect_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            try
            {
                Border BrdrRect = (Border)(sender);
                Canvas Cnvs = (Canvas)BrdrRect.Child;
                CellInformaton PPImageDetail = (CellInformaton)BrdrRect.Tag;
                DataTable dt = new DataTable();
                dt = GetProgramRunRelatedData(PPImageDetail.PartProgramName, PPImageDetail.RunName);
                DataRow[] dr;
                if (dt.Rows.Count > 0)
                {
                    dr = dt.Select("ImageName" + "=" + "'" + (PPImageDetail.Imagename) + "'");
                    if (Convert.ToBoolean(dr[0]["Accepted"]))
                    {
                        dr[0]["Accepted"] = false;
                        SelectedRectangle.Background = System.Windows.Media.Brushes.White;
                    }
                    else
                    {
                        dr[0]["Accepted"] = true;
                        dr[0]["Rejected"] = false;
                        SelectedRectangle.Background = System.Windows.Media.Brushes.ForestGreen;
                    }
                    UpdateTable(dt);
                    CalculateRectangles(ImageGrid.RenderSize.Width, ImageGrid.RenderSize.Height, textfile, imageDirectoryLocation);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PPImageViewer02", ex); }
        }

        void rect_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                Border BrdrRect = (Border)(sender);
                Canvas Cnvs = (Canvas)BrdrRect.Child;
                for (int i = 0; i < ImageGrid.Children.Count; i++)
                {
                    Border R = (Border)ImageGrid.Children[i];
                    R.BorderBrush = new SolidColorBrush(Colors.Gray);
                }
                BrdrRect.BorderBrush = new SolidColorBrush(Colors.Blue);
                ((CellInformaton)BrdrRect.Tag).IsVisited = true;
                ((CellInformaton)BrdrRect.Tag).isSelected = true;

                CellInformaton CI = (CellInformaton)BrdrRect.Tag;
                CI.IsVisited = true;
                string FlName = CI.ImagePath;
                if (System.IO.File.Exists(FlName))
                {
                    showImage(FlName);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PPImageViewer04", ex); }
        }

        void Bdr_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            Border BrdrRect = (Border)(sender);
            Canvas Cnvs = (Canvas)BrdrRect.Child;
            CellInformaton PPImageDetail = (CellInformaton)BrdrRect.Tag;
            DataTable dt = new DataTable();
            dt = GetProgramRunRelatedData(PPImageDetail.PartProgramName, PPImageDetail.RunName);
            DataRow[] dr;
            if (dt.Rows.Count > 0)
            {
                dr = dt.Select("ImageName" + "=" + "'" + (PPImageDetail.Imagename) + "'");
                if (Convert.ToBoolean(dr[0]["Rejected"]))
                {
                    dr[0]["Rejected"] = false;
                    SelectedRectangle.Background = System.Windows.Media.Brushes.White;
                }
                else
                {
                    dr[0]["Rejected"] = true;
                    dr[0]["Accepted"] = false;
                    SelectedRectangle.Background = System.Windows.Media.Brushes.Red;
                }
                UpdateTable(dt);
                CalculateRectangles(ImageGrid.RenderSize.Width, ImageGrid.RenderSize.Height, textfile, imageDirectoryLocation);
            }
        }

        void LoadProgramReportNames(bool flag = true)
        {
            try
            {
                System.Data.DataTable Full_ProgramNames = GlobalSettings.LoadExcelInstance.GetAllProgramNames("Program_Id ASC").ToTable();
                List<string> Modified_ProgramNames = new List<string>();

                PartProgramNameToTextfileRelation.Clear();
                for (int i = 0; i < Full_ProgramNames.Rows.Count; i++)
                {
                    string programName = Full_ProgramNames.Rows[i][1].ToString();
                    string path = programName.Substring(0, programName.LastIndexOf("\\") + 1);
                    string OnlyProgramName = programName.Substring(programName.LastIndexOf("\\") + 1);
                    string TextFileName = path + OnlyProgramName.Substring(0, OnlyProgramName.LastIndexOf(".")) + "_image.txt";
                    if (System.IO.File.Exists(TextFileName))
                    {
                        PartProgramNameToTextfileRelation.Add(OnlyProgramName, TextFileName);
                        Modified_ProgramNames.Add(OnlyProgramName);
                    }
                }
                PartProgramList.ItemsSource = Modified_ProgramNames;
                if (flag)
                {
                    PartProgramList.SelectionChanged += new SelectionChangedEventHandler(PartProgramList_SelectionChanged);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:Imageviewer004", ex); }
        }

        void PartProgramList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (PartProgramList.SelectedIndex >= 0)
            {
                string ProgramName = PartProgramList.SelectedItem.ToString();
                ProgramRun.Clear();
                PPRunList.Items.Clear();
                ImageFolderNameToDirectoryRelation.Clear();
                showImage("");
                ImageGrid.Children.Clear();
                string PPRunImgFolderPath = GlobalSettings.ProgramPath.Substring(0, GlobalSettings.ProgramPath.LastIndexOf("\\")) + "\\Images";
                PPRunImgFolderPath = PPRunImgFolderPath + "\\" + ProgramName.Substring(0, ProgramName.LastIndexOf("."));
                if (System.IO.Directory.Exists(PPRunImgFolderPath))
                {
                    string[] folders = System.IO.Directory.GetDirectories(PPRunImgFolderPath);
                    for (int i = 0; i < folders.Length; i++)
                    {
                        string name = folders[i].Substring(folders[i].LastIndexOf("\\") + 1);
                        ImageFolderNameToDirectoryRelation.Add(name, folders[i]);
                        ProgramRun.Add(name);
                        PPRunList.Items.Add(name);
                    }
                    //PPRunList.ItemsSource = ProgramRun;
                    PPRunList.SelectionChanged += new SelectionChangedEventHandler(PPRunList_SelectionChanged);
                }
            }
        }

        void PPRunList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (PPRunList.SelectedIndex >= 0)
                {
                    string RunName = PPRunList.SelectedItem.ToString();
                    imageDirectoryLocation = ImageFolderNameToDirectoryRelation[RunName].ToString();
                    textfile = PartProgramNameToTextfileRelation[PartProgramList.SelectedItem].ToString();
                    showImage("");
                    CalculateRectangles(ImageGrid.RenderSize.Width, ImageGrid.RenderSize.Height, textfile, imageDirectoryLocation);
                }
                else
                {
                    textfile = "";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        void readImagetxt(string Path, ref List<ImagePP> Imgpp)
        {
            try
            {
                System.IO.StreamReader Settingsfile = new System.IO.StreamReader(Path);
                string line = "";
                while (Settingsfile != null)
                {
                    line = Settingsfile.ReadLine();
                    if (line == null)
                        break;
                    ImagePP pp = new ImagePP();
                    string[] split = line.Split(':');
                    pp.imagename = split[0].ToString();
                    pp.XMultiplicationFactor = Convert.ToDouble(split[1]);
                    pp.YmultiplicationFactor = Convert.ToDouble(split[2]);
                    Imgpp.Add(pp);
                }
                Settingsfile.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PPImageViewerG03", ex);
            }
        }

        DataTable GetProgramRunRelatedData(string ProgramName, string RunName)
        {
            try
            {
                DataTable dt = new DataTable("PartProgramAndItsRun");
                System.Data.OleDb.OleDbDataAdapter DataA = new System.Data.OleDb.OleDbDataAdapter("SELECT * FROM " + "[" + "PartProgramAndItsRun" + "]" + "where PartProgramName = '" + ProgramName + "' And RunName= '" + RunName + "'", ConnectionString);
                DataA.Fill(dt);
                return dt;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PPImageViewerG09", ex); return null;
            }
        }

        private bool UpdateTable(DataTable Dt)
        {
            System.Data.OleDb.OleDbDataAdapter DataA = new System.Data.OleDb.OleDbDataAdapter("SELECT * FROM " + "[" + Dt.TableName + "]", ConnectionString);
            System.Data.OleDb.OleDbCommandBuilder CommB = new System.Data.OleDb.OleDbCommandBuilder(DataA);
            try
            {
                DataA.Update(Dt);
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEnd :UpdateTable06", ex);
                return false;
            }
        }

        private void changeGridColor(Brush color)
        {
            ImageViewerGridBorder.BorderBrush = color;
        }

        void ContextMenuContents(ref ContextMenu _contextMenu)
        {
            string[] Tags = { "Accepted", "Rejected" };
            for (int i = 0; i < Tags.Length; i++)
            {
                MenuItem mi = new MenuItem();
                mi.Header = Tags[i];
                mi.Tag = Tags[i];
                mi.Click += new RoutedEventHandler(HandlingContextMenuBtnClick);
                _contextMenu.Items.Add(mi);
            }
        }

        protected void HandlingContextMenuBtnClick(object sender, RoutedEventArgs e)
        {
            MenuItem mi = (MenuItem)sender;
            CellInformaton PPImageDetail = (CellInformaton)SelectedRectangle.Tag;
            DataTable dt = new DataTable();
            dt = GetProgramRunRelatedData(PPImageDetail.PartProgramName, PPImageDetail.RunName);
            DataRow[] dr;
            if (mi.Tag.ToString() == "Accepted")
            {
                if (dt.Rows.Count > 0)
                {
                    dr = dt.Select("ImageName" + "=" + "'" + (PPImageDetail.Imagename) + "'");
                    if (Convert.ToBoolean(dr[0]["Accepted"]))
                    {
                        dr[0]["Accepted"] = false;
                        ((Canvas)SelectedRectangle.Child).Background = System.Windows.Media.Brushes.White;
                        //SelectedRectangle.Background = System.Windows.Media.Brushes.White;
                    }
                    else
                    {
                        dr[0]["Accepted"] = true;
                        dr[0]["Rejected"] = false;
                        ((Canvas)SelectedRectangle.Child).Background = System.Windows.Media.Brushes.Green;
                        //SelectedRectangle.Background = System.Windows.Media.Brushes.Green;
                    }
                    UpdateTable(dt);
                    CalculateRectangles(ImageGrid.RenderSize.Width, ImageGrid.RenderSize.Height, textfile, imageDirectoryLocation);
                }
            }
            else if (mi.Tag.ToString() == "Rejected")
            {
                if (dt.Rows.Count > 0)
                {
                    dr = dt.Select("ImageName" + "=" + "'" + (PPImageDetail.Imagename) + "'");
                    if (Convert.ToBoolean(dr[0]["Rejected"]))
                    {
                        dr[0]["Rejected"] = false;
                        SelectedRectangle.Background = System.Windows.Media.Brushes.White;
                    }
                    else
                    {
                        dr[0]["Rejected"] = true;
                        dr[0]["Accepted"] = false;
                        SelectedRectangle.Background = System.Windows.Media.Brushes.Red;
                    }
                    UpdateTable(dt);
                    CalculateRectangles(ImageGrid.RenderSize.Width, ImageGrid.RenderSize.Height, textfile, imageDirectoryLocation);
                }
            }
        }

        void showImage(string path)
        {
            BitmapImage newImage = new BitmapImage();
            newImage.BeginInit();
            newImage.UriSource = new Uri(path, UriKind.RelativeOrAbsolute);
            newImage.EndInit();
            PPImage.Source = newImage;
        }

        void rectangle_ContextMenuOpening(object sender, ContextMenuEventArgs e)
        {
            SelectedRectangle = (Border)sender;
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            if (PPPreview != null)
            {
                PPPreview.CloseWin();
                PPPreview = null;
            }
            base.OnClosing(e);
        }

        int distinctvalues(List<ImagePP> ImagePositionList, char coordinate)
        {
            int dictinct = 0;
            bool Matchfound = false;
            if (coordinate == 'x' || coordinate == 'X')
            {
                for (int i = 0; i < ImagePositionList.Count; i++)
                {
                    for (int j = i + 1; j < ImagePositionList.Count; j++)
                    {
                        if (Math.Round(ImagePositionList[i].XMultiplicationFactor, 2) == Math.Round(ImagePositionList[j].XMultiplicationFactor, 2))
                        {
                            Matchfound = true;
                        }
                    }
                    if (Matchfound == false)
                    {
                        dictinct++;
                    }
                    Matchfound = false;
                }
            }
            else if (coordinate == 'y' || coordinate == 'Y')
            {
                for (int i = 0; i < ImagePositionList.Count; i++)
                {
                    for (int j = i + 1; j < ImagePositionList.Count; j++)
                    {
                        if (Math.Round(ImagePositionList[i].YmultiplicationFactor, 2) == Math.Round(ImagePositionList[j].YmultiplicationFactor, 2))
                        {
                            Matchfound = true;
                        }
                    }
                    if (Matchfound == false)
                    {
                        dictinct++;
                    }
                    Matchfound = false;
                }
            }
            return dictinct;
        }

        void ImageGrid_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (textfile != "" && textfile != null)
            {
                CalculateRectangles(ImageGrid.RenderSize.Width, ImageGrid.RenderSize.Height, textfile, imageDirectoryLocation);
            }
        }

        void CBShowThumbnail_Click(object sender, RoutedEventArgs e)
        {
            CheckBox chbk = (CheckBox)sender;
            ShowThumbnailImmage = (bool)chbk.IsChecked;
            if (textfile != "" && textfile != null)
            {
                CalculateRectangles(ImageGrid.RenderSize.Width, ImageGrid.RenderSize.Height, textfile, imageDirectoryLocation);
            }
        }
    }

    class ImagePP
    {
        public string imagename = "";
        public double XMultiplicationFactor = 0;
        public double YmultiplicationFactor = 0;
        public bool accepted = false;
        public bool rejected = false;
    }

    class CellInformaton
    {
        public double x, y;
        public string ImagePath;
        public string PartProgramName;
        public string RunName;
        public string Imagename;
        public bool IsVisited = false;
        public bool isSelected = false;
        public Brush color;
    }
}