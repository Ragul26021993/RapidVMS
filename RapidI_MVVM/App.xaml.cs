using System;
using System.Windows;
using System.Windows.Media;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace Rapid
{
    public partial class App : Application
    {
        //initializing mutex for single instance application purpose
        static Mutex mutex = new Mutex(true, "{8F6F0AC4-B9A1-45fd-A8CF-72F04E6BDE8F}");
        //77D53813-AF77-4331-ADCD-8C361C4C9A39
        [DllImport("User32.dll")]
        public static extern int GetSystemMetrics(int nIndex);
        const int SM_CLEANBOOT = 67;
        private void OnStartup(object sender, StartupEventArgs e)
        {
            //MessageBox.Show("000");
            //Add unhandled exception handling across all threads in the application.
            AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(AllUnHandledExceptionHandler);
            

            #region checking for windows safemode
            long result;
            result = GetSystemMetrics(SM_CLEANBOOT);
            if (result != 0)
            {
                MessageBox.Show("Rapid-I can only run in Windows normal Mode" + "\n" + "Please restart your computer in Windows normal mode");
                Process.GetCurrentProcess().Kill();
                Application.Current.Shutdown();
            }
            #endregion

            # region single instance application
            if (mutex.WaitOne(TimeSpan.Zero, true))
            {
                mutex.ReleaseMutex();
            }
            else
            {
                MessageBox.Show("This application is already running" + "\n" + "You are trying to open another instance of this program");
                Process.GetCurrentProcess().Kill();
                Application.Current.Shutdown();
            }

            #endregion
            //MessageBox.Show("001");

            # region Check for the Port connection
            bool CheckFortheRunMode = true;
            string FileName = AppDomain.CurrentDomain.BaseDirectory + "\\CTPL_RapidIDebugMode.ilk";
            System.IO.FileInfo Myfileinfo = new System.IO.FileInfo(FileName);
            if (Myfileinfo.Exists)
                CheckFortheRunMode = false;
            if (CheckFortheRunMode)
            {
                string R_PortName = "";
                R_PortName = GetCommPortName();
                int CurrentConnection = 1;
                try
                {
                    CurrentConnection = CheckPortConnection(R_PortName);
                }
                catch (Exception exxx)
                {
                    CurrentConnection = 1;
                }
                if (CurrentConnection == 0)
                {
                    MessageBox.Show("Hardware is Already Connected");
                    Process.GetCurrentProcess().Kill();
                    Application.Current.Shutdown();
                }
                else if (CurrentConnection == 1)
                {
                    MessageBox.Show("Please Check the Hardware Connection");
                    Process.GetCurrentProcess().Kill();
                    Application.Current.Shutdown();
                }
            }
            #endregion
            //MessageBox.Show("002");

            //Handling the Startup of the Application
            try
            {
                //Check the Render Capablity of the system.If not harware assited,then render everything in software only.
                if ((RenderCapability.Tier >> 16) < 2)
                {
                    RenderOptions.ProcessRenderMode = System.Windows.Interop.RenderMode.SoftwareOnly;
                    System.Windows.Media.Animation.Timeline.DesiredFrameRateProperty.OverrideMetadata(typeof(System.Windows.Media.Animation.Timeline),
                    new FrameworkPropertyMetadata { DefaultValue = 30 });
                }
                //Initializing the MainView class & its Data Context.
                //MessageBox.Show("003");
                MainView view = new MainView();
                view.DataContext = new ViewModels.MainViewModel();
                //This application main window is the MainView.
                this.MainWindow = view;
                //MessageBox.Show("004");

                //Show the main window.
                view.Show();
                //MessageBox.Show("005");
            }
            catch (UnauthorizedAccessException ex)
            {
               // MessageBox.Show("E 003 -1");
                System.Diagnostics.EventLog.WriteEntry("Rapid-I 5.0", ex.ToString(),
                    System.Diagnostics.EventLogEntryType.Error, 1);
            }
            catch (TypeInitializationException ex)
            {
              //  MessageBox.Show("E 003 -2");
                System.Diagnostics.EventLog.WriteEntry("Rapid-I 5.0", ex.ToString(),
                   System.Diagnostics.EventLogEntryType.Error, 2);
            }
            catch (TypeLoadException ex)
            {
             //   MessageBox.Show("E 003 -3");
                System.Diagnostics.EventLog.WriteEntry("Rapid-I 5.0", ex.ToString(),
                   System.Diagnostics.EventLogEntryType.Error, 3);
            }
            catch (InvalidOperationException ex)
            {
             //   MessageBox.Show("E 003 -4");
                System.Diagnostics.EventLog.WriteEntry("Rapid-I 5.0", ex.ToString(),
                    System.Diagnostics.EventLogEntryType.Error, 4);
            }
            catch (ArgumentException exx)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AX02", exx);
                MessageBox.Show("E 003 -4-1");
            }
            catch (Exception ex)
            {
               // MessageBox.Show("E 003 -5");
                System.Diagnostics.EventLog.WriteEntry("Rapid-I 5.0", ex.ToString(),
                    System.Diagnostics.EventLogEntryType.Error, 5);
            }
        }
        //handling the premature Application session ending.
        void App_SessionEnding(object sender, SessionEndingCancelEventArgs e)
        {
            // Ask the user if they want to allow the session to end
            string msg = string.Format("You have choosen {0} of your system.\r\n Do you want to continue with that ?", e.ReasonSessionEnding);
            System.Windows.Forms.DialogResult result = System.Windows.Forms.MessageBox.Show(msg, "Rapid-I(Session Ending)",
         System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question);
            // End session, if specified
            if (result == System.Windows.Forms.DialogResult.No)
            {
                e.Cancel = true;
            }
        }
        //handling the application unhandled exception event.
        private void Application_DispatcherUnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
        {
            //Log the whole error when an unhandled exception comes in way.
            //System.Diagnostics.EventLog.WriteEntry("Rapid-I 5.0", e.Exception.ToString(),
            //       System.Diagnostics.EventLogEntryType.Error, 6);
            //Infor user about it.
            if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
            {
                MessageBox.Show("An unexpected error has occurred.\r\nPlease inform the manufacturer with a list of steps that caused this message to appear. We apologise for the inconvenience.\r\n" + e.Dispatcher.Thread.Name
                   + "\r\n" + e.Exception.Message + "\r\n" + e.Exception.StackTrace
                   , "Rapid-I", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            //RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("Critical_0000", e.Exception);
            //Make the exception handled to be true.
            e.Handled = true;
        }

        private void AllUnHandledExceptionHandler(object sender, UnhandledExceptionEventArgs e)
        {
            try
            {
                MessageBox.Show("Sorry, I got some error condition that I did not expect to see! But you can continue now", "Rapid-I 5.0", MessageBoxButton.OK);
            }
            catch(Exception exx)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("APX055", exx);

            }
        }
        string GetCommPortName()
        {
            try
            {

                //Microsoft.VisualBasic.Devices.Computer MyComputer = new Microsoft.VisualBasic.Devices.Computer();
                //MessageBox.Show("Started Comm Port Reading", "Rapid-I");
                RegistryKey i = Registry.LocalMachine.OpenSubKey("System\\CurrentControlSet\\Enum\\USB\\");
                if (i == null) MessageBox.Show("Couldnt get the registry key");
                string MPortName = "";
                foreach (string s in i.GetSubKeyNames())
                {
                    int r = s.IndexOf("10C4");
                    if (r == -1) r = s.IndexOf("10c4");
                    int tem = s.IndexOf("EA60");
                    if (tem == -1) tem = s.IndexOf("ea60");
                    int r1 = s.IndexOf("&Mi_");
                    int tem1 = s.IndexOf("_ea");
                    if ((r != -1 && tem != -1) || (r1 != -1 && tem1 != -1))
                    {
                        try
                        {
                            i = Registry.LocalMachine.OpenSubKey("System\\CurrentControlSet\\Enum\\USB\\" + s + "\\");
                            i = i.OpenSubKey((i.GetSubKeyNames())[0] + "\\Device Parameters");
                            string Pname = i.GetValue("PortName").ToString();
                            if (Pname != "") MPortName = Pname;
                        }
                        catch (Exception ex)
                        {
                            //RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("APX056", ex);
                            //MessageBox.Show(ex.Message + "  ::   " + ex.StackTrace, "Rapid-I");
                        }
                    }
                }
                return MPortName;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "  ::   " + ex.StackTrace, "Rapid-I");
                //RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("APX057", ex);
                return "";
            }
        }

        int CheckPortConnection(string PName)
        {
            try
            {
                //MessageBox.Show(PName, "Rapid-I");
                if (PName == null || PName == "") return 1;
                System.IO.Ports.SerialPort CommPort = new System.IO.Ports.SerialPort();
                CommPort.PortName = PName;
                if (CommPort.IsOpen) return 0;
                try { CommPort.Open(); }
                catch (Exception ex) {
                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("APX059", ex);
                    return 1; }
                CommPort.Close();
                return 2;
            }
            catch (Exception ex) {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("APX058", ex);
                return 1; }
        }
    }
}
