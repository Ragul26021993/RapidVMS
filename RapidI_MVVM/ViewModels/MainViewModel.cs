using System.Windows;
using System.Windows.Input;
using Rapid.Commands;

namespace Rapid.ViewModels
{

    public class MainViewModel : ViewModelBase
    {
        //Instance to the Rapid-I Framework wrapper class.

        private DelegateCommand exitCommand;
        private DelegateCommand minimizeCommand;
        private DelegateCommand doNothingCommand;
        public bool IsExiting = false;

        #region Constructor

        public MainViewModel()
        {
         
        }

        #endregion

        public ICommand ExitCommand
        {
            get
            {
                if (exitCommand == null)
                {
                    exitCommand = new DelegateCommand(Exit);
                }
                return exitCommand;
            }
        }
        public ICommand MinimizeCommand
        {
            get
            {
                if (minimizeCommand == null)
                {
                    minimizeCommand = new DelegateCommand(Minimize);
                }
                return minimizeCommand;
            }
        }
        public ICommand DoNothingCommand
        {
            get
            {
                if (doNothingCommand == null)
                {
                    doNothingCommand = new DelegateCommand(DoNothing);
                }
                return doNothingCommand;
            }
        }
        private void Exit()
        {
                Application.Current.MainWindow.Close();
        }
        private void DoNothing(){}
        private void Minimize()
        {
            Application.Current.MainWindow.WindowState = WindowState.Minimized;
        }

    }
}
