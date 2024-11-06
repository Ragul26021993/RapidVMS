using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Controls.Primitives;
using Rapid.DGItemClasses;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for ActionStatusWin.xaml
    /// </summary>
    public partial class ActionStatusWin : Grid
    {
        ObservableCollection<ActionEntities> DGSourcing;
        public bool IsActionReRun = false;
        private bool SetAllCritical = false;
        public ActionStatusWin()
        {
            this.InitializeComponent();
            // Insert code required on object creation below this point.
            init();
        }
        void init()
        {
            try
            {
                //Handling the events related to Add/Modify/Delete of Actions
                RWrapper.RW_ActionParameter.MYINSTANCE().ActionParameterUpdate += new RWrapper.RW_ActionParameter.ActionParameterEventHandler(ActionStatusWin_ActionParameterUpdate);
                ActionsDG.MouseDoubleClick += new MouseButtonEventHandler(ActionsDG_MouseDoubleClick);
                ActionsDG.PreviewKeyDown += new KeyEventHandler(ActionsDG_PreviewKeyDown);
                DGSourcing = new ObservableCollection<ActionEntities>();
                ActionsDG.ItemsSource = DGSourcing;

                //Add an context menu for the datagridview of the ActionStatus List
                ContextMenu con = new System.Windows.Controls.ContextMenu();
                MenuItem delete = new MenuItem();
                delete.Header = "Delete";
                delete.Tag = "Delete";
                delete.Click += new RoutedEventHandler(Handle_Delete_ActionMenuItem);
                con.Items.Add(delete);
                ActionsDG.ContextMenu = con;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS01", ex); }
        }

        #region Frontend event Handling

        //Handling rerun of program step if failed
        void ActionsDG_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try
            {
                if (e.LeftButton != MouseButtonState.Pressed) return;
                ActionEntities ClickedAction = (ActionEntities)ActionsDG.SelectedItem;
                if (ClickedAction != null)
                {
                    if (!ClickedAction.PassFailStatus)
                    {
                        IsActionReRun = true;
                        GlobalSettings.MainExcelInstance.UpdateLastReport = true;
                        System.Threading.Thread TmpTh = new System.Threading.Thread(GlobalSettings.MainExcelInstance.OpenExcel);
                        TmpTh.Start();
                        RWrapper.RW_ActionParameter.MYINSTANCE().RerunAction(ClickedAction.ID);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS01a", ex);
            }
        }
        //Handling delete press
        void ActionsDG_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
            {
                e.Handled = true;
            }
        }
        //Intimating the framework about critical action selected
        void CliticalChkBx_Checked(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_ActionParameter.MYINSTANCE().SetCriticalAction(((ActionEntities)((CheckBox)sender).DataContext).ID, true);
        }
        //Intimating the framework about critical action unselected
        void CliticalChkBx_Unchecked(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_ActionParameter.MYINSTANCE().SetCriticalAction(((ActionEntities)((CheckBox)sender).DataContext).ID, false);
        }

        #endregion

        #region Framework event handling

        //Handling action list related events
        void ActionStatusWin_ActionParameterUpdate(RWrapper.RW_Enum.RW_ACTIONPARAMTYPE ActionParam)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (ActionParam)
                    {
                        case RWrapper.RW_Enum.RW_ACTIONPARAMTYPE.ACTION_ADDED:
                            AddAction();
                            break;
                        case RWrapper.RW_Enum.RW_ACTIONPARAMTYPE.ACTION_REMOVED:
                            RemoveAction();
                            break;
                        case RWrapper.RW_Enum.RW_ACTIONPARAMTYPE.ACTION_UPDATE:
                            UpdateAction();
                            break;
                        case RWrapper.RW_Enum.RW_ACTIONPARAMTYPE.ACTION_SELECTION_CHANGED:
                            ChangeActionSelection();
                            break;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_ActionParameter.ActionParameterEventHandler(ActionStatusWin_ActionParameterUpdate), ActionParam);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS02", ex);
            }
        }
        //A new Action added
        void AddAction()
        {
            try
            {
                ActionEntities TmpAEntity = new ActionEntities();
                TmpAEntity.ID = Convert.ToInt32(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[0]);
                TmpAEntity.NameStr = RWrapper.RW_ActionParameter.ActionParameter_ArrayList[1].ToString();
                TmpAEntity.ParentShapeStr = RWrapper.RW_ActionParameter.ActionParameter_ArrayList[2].ToString();
                TmpAEntity.IsCriticalAction = Convert.ToBoolean(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[3]);
                TmpAEntity.NoOfPtsValue = Convert.ToInt32(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[4]);
                TmpAEntity.PassFailStatus = Convert.ToBoolean(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[5]);
                TmpAEntity.IsUserEditEnabled = Convert.ToBoolean(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[6]);
                TmpAEntity.IsProgramNotLoaded = !GlobalSettings.PartProgramLoaded;
                //Convert.ToInt32(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[])
                DGSourcing.Add(TmpAEntity);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS03", ex);
            }
        }
        //Existing Action removed
        void RemoveAction()
        {
            try
            {
                for (int i = 0; i < DGSourcing.Count; i++)
                {
                    if (DGSourcing[i].ID == RWrapper.RW_ActionParameter.MYINSTANCE().Action_EntityID)
                        DGSourcing.Remove(DGSourcing[i]);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS04", ex);
            }
        }
        //Existing Action updated
        void UpdateAction()
        {
            try
            {
                int ActionId = Convert.ToInt16(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[0]);
                for (int i = 0; i < DGSourcing.Count; i++)
                {
                    if (DGSourcing[i].ID == ActionId)
                    {
                        DGSourcing[i].NameStr = RWrapper.RW_ActionParameter.ActionParameter_ArrayList[1].ToString();
                        DGSourcing[i].ParentShapeStr = RWrapper.RW_ActionParameter.ActionParameter_ArrayList[2].ToString();
                        DGSourcing[i].IsCriticalAction = Convert.ToBoolean(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[3]);
                        DGSourcing[i].NoOfPtsValue = Convert.ToInt16(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[4]);
                        DGSourcing[i].PassFailStatus = Convert.ToBoolean(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[5]);
                        DGSourcing[i].IsUserEditEnabled = Convert.ToBoolean(RWrapper.RW_ActionParameter.ActionParameter_ArrayList[6]);
                        DGSourcing[i].IsProgramNotLoaded = !GlobalSettings.PartProgramLoaded;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS05", ex);
            }
        }
        //Select currently running action
        void ChangeActionSelection()
        {
            try
            {
                int ActionId = RWrapper.RW_ActionParameter.MYINSTANCE().Action_EntityID;
                ActionsDG.SelectedItems.Clear();
                if (ActionId > -1)
                {
                    for (int i = 0; i < DGSourcing.Count; i++)
                    {
                        if (DGSourcing[i].ID == ActionId)
                        {
                            ActionsDG.SelectedIndex = i;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS06", ex);
            }
        }

        #endregion

        //Clear All function
        public void ClearAll()
        {
            try
            {
                DGSourcing.Clear();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS07", ex); }
        }

        private void HeaderMouseClick(object sender, RoutedEventArgs e)
        {
            try
            {
                DataGridColumnHeader DGC = (DataGridColumnHeader)sender;
                if (DGC.Content.ToString() == "Critical")
                {
                    if (DGSourcing != null && DGSourcing.Count > 0)
                    {
                        foreach (ActionEntities actn in DGSourcing)
                        {
                            if (actn.IsProgramNotLoaded)
                                actn.IsCriticalAction = !SetAllCritical;
                        }
                        SetAllCritical = !SetAllCritical;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS01HeaderMouseClick", ex);
            }
        }

        void Handle_Delete_ActionMenuItem(object sender, RoutedEventArgs e)
        {
            try
            {
                ActionEntities sae = DGSourcing[ActionsDG.SelectedIndex];
                if (sae.NameStr == "AddProbePath")
                   RWrapper.RW_ActionParameter.MYINSTANCE().DeleteAction(sae.ID);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AS09", ex);
            }
        }
    }
}