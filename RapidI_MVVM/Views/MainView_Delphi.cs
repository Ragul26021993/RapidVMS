using System;
using System.Globalization;
using System.Collections.Generic;
using System.Windows;
using System.IO;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Controls.Primitives;
using System.Linq;
//using System.ComponentModel;
using LicensingModuleDLL;
using Rapid.Windows;
using Rapid.Panels;
using Rapid.TabEntities;
using Rapid.Utilities;
using Rapid.AttachedProperties;
using RapidI.Help;
using System.Threading;
using System.Data;
using System.Data.OleDb;

namespace Rapid
{
    public partial class MainView : Window
    {

        #region Especially For Delfi
        void MainView_ZeroingOffsetEvent(double[] Offset)
        {
            System.Data.DataTable DTable = GlobalSettings.SettingsReader.GetTable("ZeroingOffset");
            if (DTable.Rows.Count == 0)
            {
                System.Data.DataRow dr = DTable.NewRow();
                dr["MachineNo"] = GlobalSettings.MachineNo;
                dr["OffsetX"] = Offset[0];
                dr["OffestY"] = Offset[1];
                dr["OffsetZ"] = Offset[2];
                DTable.Rows.Add(dr);
            }
            else
            {
                System.Data.DataRow[] Drow = DTable.Select("MachineNo=" + GlobalSettings.MachineNo);
                Drow[0]["OffsetX"] = Offset[0];
                Drow[0]["OffestY"] = Offset[1];
                Drow[0]["OffsetZ"] = Offset[2];
            }
            GlobalSettings.SettingsReader.UpdateTable("ZeroingOffset");
        }
        #endregion

    }
}
