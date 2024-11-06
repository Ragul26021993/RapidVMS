using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Rapid.DGItemClasses;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for PPBuildWin.xaml
	/// </summary>
	public partial class PPBuildOptionsWin :OpenGlWin
	{
        ObservableCollection<PPBuildStepEntities> DGSourcing;

        public PPBuildOptionsWin()
		{
			this.InitializeComponent();
            // Insert code required on object creation below this point.
            init();
		}
        void init()
        {
            DGSourcing = new ObservableCollection<PPBuildStepEntities>();
            BuildStepsDG.ItemsSource = DGSourcing;

            List<ArrayList> StepsInfoList = RWrapper.RW_PartProgram.MYINSTANCE().GetProgramStepsList();
            for (int i = 0; i < StepsInfoList.Count; i++)
            {
                PPBuildStepEntities TmpStepObj = new PPBuildStepEntities();
                TmpStepObj.SlNo = (int)StepsInfoList[i][0];
                TmpStepObj.ShapeNameStr = StepsInfoList[i][1].ToString();
                TmpStepObj.NameStr = StepsInfoList[i][2].ToString();
                TmpStepObj.ID = (int)StepsInfoList[i][3];
                TmpStepObj.IsCriticalStep = (bool)StepsInfoList[i][4];
                DGSourcing.Add(TmpStepObj);
            }
        }

        void OkBtn_Click(object sender, RoutedEventArgs e)
        {
            List<ArrayList> StepsInfoList = new List<ArrayList>();
            for (int i = 0; i < DGSourcing.Count; i++)
            {
                ArrayList TmpStepObj = new ArrayList();
                TmpStepObj.Add(DGSourcing[i].SlNo);
                TmpStepObj.Add(DGSourcing[i].ShapeNameStr);
                TmpStepObj.Add(DGSourcing[i].NameStr);
                TmpStepObj.Add(DGSourcing[i].ID);
                TmpStepObj.Add(DGSourcing[i].IsCriticalStep);
                StepsInfoList.Add(TmpStepObj);
            }
            RWrapper.RW_PartProgram.MYINSTANCE().SetCriticalActionList(StepsInfoList);
            this.Close();
        }

        void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}