using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for PPreviewWin.xaml
    /// </summary>
    public partial class PPreviewWin : Window
    {       
        BitmapImage newImage;

        public PPreviewWin()
        {
            InitializeComponent();
            this.MouseEnter += new MouseEventHandler(PPreviewWin_MouseEnter);
            this.MouseLeave += new MouseEventHandler(PPreviewWin_MouseLeave);
        }
        void PPreviewWin_MouseEnter(object sender, RoutedEventArgs e)
        {
            this.CloseWin();
        }
        void PPreviewWin_MouseLeave(object sender, RoutedEventArgs e)
        {
            this.CloseWin();
        }

        public void ShowImageFile(string FileName)
        {
            if (FileName.Length != 0)
            {
                newImage = new BitmapImage();
                newImage.BeginInit();
                newImage.UriSource = new Uri(FileName, UriKind.RelativeOrAbsolute);
                newImage.CacheOption = BitmapCacheOption.OnLoad;
                newImage.EndInit();                
                PreviewImage.Source = newImage;                
            }
        }
        public void ShowWin()
        {
            //this.Opacity = 0;
            this.Show();

            //var anim = new DoubleAnimation(1, (Duration)TimeSpan.FromSeconds(0.1));
            //this.BeginAnimation(UIElement.OpacityProperty, anim);
        }
        public void CloseWin()
        {
            this.Close();           
            //UnloadImage();
            //var anim = new DoubleAnimation(0, (Duration)TimeSpan.FromSeconds(0.1));
            //anim.Completed += (s, _) => this.Close();
            //this.BeginAnimation(UIElement.OpacityProperty, anim);
        }
        
        private void UnloadImage()
        {
            try
            {
                PreviewImage.Source = null;
                GC.Collect();
                System.Threading.Thread.Sleep(10);
                GC.WaitForPendingFinalizers();
                System.Threading.Thread.Sleep(10);
                System.Windows.Forms.Application.DoEvents();
                System.Threading.Thread.Sleep(50);
                GC.Collect();
                System.Threading.Thread.Sleep(10);
                GC.WaitForPendingFinalizers();
                System.Threading.Thread.Sleep(10);
                System.Windows.Forms.Application.DoEvents();
                this.Content = null;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PPImageViewerG05", ex);
            }
        }
        void ReleaseImage()
        {
            try
            {
                ((BitmapImage)PreviewImage.Source).UriSource = null;
                PreviewImage.Source = null;
                GC.Collect();
                //System.Threading.Thread.Sleep(10);
                GC.WaitForPendingFinalizers();
                //System.Threading.Thread.Sleep(10);
                System.Windows.Forms.Application.DoEvents();
                //System.Threading.Thread.Sleep(50);
                GC.Collect();
                //System.Threading.Thread.Sleep(10);
                GC.WaitForPendingFinalizers();
                //System.Threading.Thread.Sleep(10);
                System.Windows.Forms.Application.DoEvents();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PPImageViewerG03", ex);
            }
        }    
    }
}
