using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;
using System.IO;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Controls.Primitives;
using Rapid.Windows;
using Rapid.DGItemClasses;
using RapidI.Help;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for ShapeStatusWin.xaml
    /// </summary>
    public partial class ShapeStatusWin : Grid
	{
		public delegate void FGPtsShowHideEventHandler(string State);
		public event FGPtsShowHideEventHandler ShowHideFG;
		bool ProcessingSelection = false, Processing_FGPtSelection = false;
		ObservableCollection<ShapeEntities> DGSourcing;
		int MaxNumOfShpParams = 0;
		CloudpointComparison cloudptwin;
		//Constructor
		public ShapeStatusWin()
		{
			try
			{
				this.InitializeComponent();

				// Insert code required on object creation below this point.
				init();
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH01", ex); }
		}
		//Raising event when FG points show/hide changes
		protected void OnShowHideFG(string e) 
		{
			try
			{
				if (ShowHideFG != null)
				{
					//Invokes the delegates.
					ShowHideFG(e);
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH02", ex); }
		}
		//public void
		private void init()
		{
			try
			{
				#region Handling the events related to Add/Modify/Delete of shape
				
				RWrapper.RW_ShapeParameter.MYINSTANCE().ShapeParameterUpdate += new RWrapper.RW_ShapeParameter.ShapeParameterEventHandler(ShapeStatusWin_ShapeParameterUpdate);
				RWrapper.RW_ShapeParameter.MYINSTANCE().ShowFgPtListEvent += new RWrapper.RW_ShapeParameter.SelectFgPtEventHandler(ShapePoints_Selection);                           
				#endregion
				ShapesDG.SelectionChanged += new SelectionChangedEventHandler(ShapesDG_SelectionChanged);
				ShapesDG.MouseUp += new MouseButtonEventHandler(ShapesDG_MouseUp);
				ShapesDG.CellEditEnding += new EventHandler<DataGridCellEditEndingEventArgs>(ShapesDG_CellEditEnding);
				ShapesDG.ContextMenuOpening += new ContextMenuEventHandler(ShapesDG_ContextMenuOpening);

				List<Image> ShpP_ImageList = GlobalSettings.SettingsReader.UpdateBtnInfoList("Shape Parameters", 30, 30);
				List<string> ShpP_ToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
				List<int> ShpPbuttonType = GlobalSettings.SettingsReader.ButtontypeList;
				List<string> ShpP_ButtonNameList = GlobalSettings.SettingsReader.ButtonBaseNameList;
				if (GlobalSettings.IsButtonWithNames)
				{
					ButtonsPanel.Height = 68;
					ScrollBarBorber.Height = 78;
					ScrollBarBorber.Margin = new Thickness(4, 0, 0, 2);
					ShapesDG.Margin = new Thickness(4, 0, 2, 0);
					ShapePointsDG.Margin = new Thickness(2, 0, 0, 0);
					MainPanel.RowDefinitions[0].Height = new GridLength(80, GridUnitType.Pixel);
					MainPanel.RowDefinitions[1].Height = new GridLength(.58, GridUnitType.Star);
				}
				else
				{
					MainPanel.RowDefinitions[0].Height = new GridLength(58, GridUnitType.Pixel);
					MainPanel.RowDefinitions[1].Height = new GridLength(.773, GridUnitType.Star);
				}
				for (int i = 0; i < ShpP_ImageList.Count; i++)
				{
					if (ShpPbuttonType[i] == 10)
					{
						Image Im = new Image();
						Im = ShpP_ImageList[i];
						Im.Width = 5; Im.Height = 40;
						Im.Margin = new Thickness(3, 0, 3, 0);
						ButtonsPanel.Children.Add(Im);
					}
					else
					{
						Button btn = new Button();
						if (GlobalSettings.IsButtonWithNames)
						{ btn.Width = 80; }
						else { btn.Width = 40; }                      
						btn.Margin = new Thickness(2, 0, 2, 0);
						btn.Style = GlobalSettings.ScrBar_NormalButtonStyle;
						btn.Content = ShpP_ImageList[i];
						btn.Tag = new string[] { "ShapeParameters", ShpP_ToolTipList[i], ShpP_ButtonNameList[i], "Shapes", GlobalSettings.SettingsReader.ButtonFeatureId[i].ToString() };
						btn.Click += new RoutedEventHandler(HandlePanelButtonsClick);
						btn.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
						btn.ToolTip = ShpP_ToolTipList[i];
						btn.SetValue(AttachedProperties.AttachedButtonName.ButtonName, btn.ToolTip);
						ButtonsPanel.Children.Add(btn);
					}
				}

				//Add an context menu for the datagridview of the ShaeParamameterWindow
				ContextMenu con = new System.Windows.Controls.ContextMenu();
				MenuItem delete = new MenuItem();
				delete.Header = "Delete";
				delete.Tag = "Delete";
				delete.Click += new RoutedEventHandler(HandleShapePointsDG_RowDeleteClicked);
				
				MenuItem Probepoint= new MenuItem();
				Probepoint.Header = "Take Probe Point";
				Probepoint.Tag = "TakeProbePoint";
				Probepoint.Click += new RoutedEventHandler(HandleShapePointsDG_RowDeleteClicked);

				MenuItem GoToCordinate = new MenuItem();
				GoToCordinate.Header = "Go to Point Co-ordinate";
				GoToCordinate.Tag = "GoToCordinate";
				GoToCordinate.Click += new RoutedEventHandler(HandleShapePointsDG_RowDeleteClicked);

				ShapePointsDG.SelectionChanged += new SelectionChangedEventHandler(HandleShapePointsDG_SelectionChanged);
				con.Items.Add(delete);
				con.Items.Add(GoToCordinate);
				con.Items.Add(Probepoint);
				ShapePointsDG.ContextMenu = con;
				//Calling MainFor previewkeydown when keys are pressed on this window.

				DGSourcing = new ObservableCollection<ShapeEntities>();
				ShapesDG.ItemsSource = DGSourcing;
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH03", ex); }
		}

		void btn_PreviewMouseRightButtonDown(object sender, MouseButtonEventArgs e)
		{
			try
			{
				ContentControl b = (ContentControl)sender;
				string[] tagStr = (string[])b.Tag;
				string BtnName = tagStr[2];
				string BtnLocationName = tagStr[3];
				if (HelpWindow.GetObj().ShowTopic(BtnLocationName, BtnName))
					HelpWindow.GetObj().Show();
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS07a", ex); }
		   
		}
		#region Front end Event handling

		//All scroll panel button handling
		void HandlePanelButtonsClick(object sender, RoutedEventArgs e)
		{
			try
			{
				ContentControl b = (ContentControl)sender;
				string[] tagStr = (string[])b.Tag;
				if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
				{
					if (!(bool)GlobalSettings.FeatureLicenseInfo[Convert.ToInt32(tagStr[4])])
					{
						MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
						if (b.GetType() == typeof(ToggleButton))
							((ToggleButton)b).IsChecked = false;
						if (b.GetType() == typeof(RadioButton))
							((RadioButton)b).IsChecked = false;
						return;
					}
				}
				if (tagStr[2] != "Reset Points")
					RWrapper.RW_MainInterface.MYINSTANCE().HandleShapeParameter_Click(tagStr[2]);
				switch (tagStr[2])
				{
					case "FG Points Visible":
						OnShowHideFG("FG Points Hidden");
						//ShowHideFGpointsStatus = "Hide FG Points";
						break;
					case "FG Points Hidden":
						OnShowHideFG("FG Points Visible");
						//ShowHideFGpointsStatus = "Show FG Points";
						break;
					case "Reset Points":
						if (System.Windows.Forms.MessageBox.Show("All shape points will be deleted for the selected shape.\nAre you sure you want to Clear the shape?",
						Application.Current.MainWindow.Title, System.Windows.Forms.MessageBoxButtons.YesNo,
						System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
							RWrapper.RW_MainInterface.MYINSTANCE().HandleShapeParameter_Click(tagStr[2]);
						break;
					case "Rename Shape":
						RenameShape();
						break;
					case "Show Specific Deviation":
						GlobalSettings.MainWin.AssignShapePanelToShapeInfoWindow(4, "Upper Tolerance;Lower Tolerance;Deviation Interval;Yellow Shapes Deviation;Linear (X);Linear (Y);Geometric;Deviation in 2D;Bestfit Type:;Line Bestfit;Arc Bestfit;Select Axis:;X Axis;Y Axis", "SheshaDeviationTolerance", true);
						break;
					case "Show All Deviation":
						GlobalSettings.MainWin.AssignShapePanelToShapeInfoWindow(10, "Upper Cutoff;Tolerance;Deviation Interval;Linear (X);Linear (Y);Geometric;Closed Loop", "GenericDeviationTolerance", true);
						break;
					case "FGandCldPtSize":
						GlobalSettings.PtSizeWin.ShowDialog();
						break;
					case "SetCloudPtColourInterval":
						GlobalSettings.MainWin.AssignShapePanelToShapeInfoWindow(0, "Enter the Z interval in mm", "SetCloudPtColourInterval", true);
						break;
					case "Cloudpoint Comparison" :
						cloudptwin = new CloudpointComparison();
						GlobalSettings.MainWin.SetOwnerofWindow(cloudptwin);
						cloudptwin.Show();
						break;
					case "Export To Excel":
						System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
						sd.Filter = "(*.xls)Excel Files|*.xls";
						sd.Title = "Save as";
						sd.DefaultExt = ".xls";
						if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
						{
							Microsoft.Office.Interop.Excel._Application ExlObj = new Microsoft.Office.Interop.Excel.Application();
							Microsoft.Office.Interop.Excel._Workbook XlWbk = ExlObj.Workbooks.Add();

							XlWbk.SaveAs(sd.FileName);
							XlWbk.Close();
							XlWbk = null;
							ExlObj.Quit();
							GlobalSettings.MainExcelInstance.ReportName = sd.FileName;
							GlobalSettings.MainExcelInstance.SelectedSheet = "Sheet1";
							GlobalSettings.MainExcelInstance.OpenExcel();
							for (int i = 0; i < DGSourcing.Count; i++)
							{
								//object[] RowVals = ((System.Data.DataRowView)ShapesDG.Items[i]).Row.ItemArray;
								//for (int j = 0; j < RowVals.Length - 1; j++)
								//{
								//    GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(RowVals[j].ToString(), i + 1, ((char)(65 + j)).ToString());
								//}
								GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(DGSourcing[i].NameStr, i + 1, "A");
								GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(DGSourcing[i].TypeStr, i + 1, "B");
								GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(DGSourcing[i].Param1Str, i + 1, "C");
								GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(DGSourcing[i].Param2Str, i + 1, "D");
								GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(DGSourcing[i].Param3Str, i + 1, "E");
								GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(DGSourcing[i].Param4Str, i + 1, "F");
								GlobalSettings.MainExcelInstance.CompareAndWrite_FieldValues(DGSourcing[i].Param5Str, i + 1, "G");
							}
							GlobalSettings.MainExcelInstance.CloseExcel();
							if (File.Exists(sd.FileName))
								System.Diagnostics.Process.Start(sd.FileName);
							else
								MessageBox.Show("The report file does not exist. It may not have been created properly.", "Rapid-I 5.0");
						}
						break;
					case "Point to Cloud Point Deviation":
						GlobalSettings.MainWin.SetOwnerofWindow(PointToCloudPointDeviation.MyInstance());
						PointToCloudPointDeviation.MyInstance().Show();
						break;
				}
			}
			catch (Exception ex)
			{
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH04", ex);
			}
		}
		//Handling the selction of Shapes row in UI
		void ShapesDG_SelectionChanged(object sender, EventArgs e)
		{
			try
			{
				if (ShapesDG.SelectedItems.Count <= 0) return;
				if (ProcessingSelection) return;
				ProcessingSelection = true;
				List<int> SelectdShpsID = new List<int>();

				for (int i = 0; i < ShapesDG.SelectedItems.Count; i++)
					SelectdShpsID.Add(((ShapeEntities)ShapesDG.SelectedItems[i]).ID);
				RWrapper.RW_ShapeParameter.MYINSTANCE().SelectShape(SelectdShpsID.ToArray());
				//SelectdShpsID = null;
				ProcessingSelection = false;
			}
			catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH05", ex); }
		}
		//Handling Middle click of shape for selecting it in measurement mode
		void ShapesDG_MouseUp(object sender, MouseButtonEventArgs e)
		{
			try
			{
				if (e.ChangedButton == MouseButton.Middle)
				{
					RadioButton TBtn = null;
					TBtn = (RadioButton)GlobalSettings.MainWin.RcadToolbar.GetContent().Get_Button("Measurement");
					if (TBtn != null)
					{
						if ((bool)TBtn.IsChecked)
						{
							DependencyObject dep = (DependencyObject)e.OriginalSource;
							// iteratively traverse the visual tree
							while ((dep != null) && !(dep is DataGridRow))
								dep = VisualTreeHelper.GetParent(dep);
							if (dep == null)
								return;

							if (dep is DataGridRow)
							{
								DataGridRow row = dep as DataGridRow;
								RWrapper.RW_MainInterface.MYINSTANCE().SetShapeForMeasure(((ShapeEntities)row.DataContext).ID);
							}
						}
					}
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH05a", ex); }
		}
		//Handling context menu for shape entities in DG
		void ShapesDG_ContextMenuOpening(object sender, RoutedEventArgs e)
		{
			try
			{
				if (ShapesDG.SelectedItems.Count != 1)
					return;
				RadioButton MRBtn = (RadioButton)GlobalSettings.MainWin.RcadToolbar.GetContent().Get_Button("Measurement");
				if (MRBtn != null)
				{
					if ((bool)MRBtn.IsChecked)
					{
						e.Handled = true;
						RWrapper.RW_MainInterface.MYINSTANCE().SetShapeRightClickForMeasure(((ShapeEntities)ShapesDG.SelectedItem).ID);
					}
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH05b", ex); }
		}
		//Deleting the shape points when context menu item "delete" is clicked
		void HandleShapePointsDG_RowDeleteClicked(object sender, RoutedEventArgs e)
		{
			try
			{
				if (((MenuItem)sender).Tag.ToString() == "Delete")
					RWrapper.RW_ShapeParameter.MYINSTANCE().DeleteSelectedPointsList();
				else if (((MenuItem)sender).Tag.ToString() == "GoToCordinate")
					RWrapper.RW_ShapeParameter.MYINSTANCE().GotoSelectedPtPosition();               

			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH06", ex); }
		}
		//For Shape points DG selection update
		void HandleShapePointsDG_SelectionChanged(object sender, RoutedEventArgs e)
		{
			try
			{
				if (ShapePointsDG.SelectedItems.Count <= 0) return;
				if (Processing_FGPtSelection) return;
				Processing_FGPtSelection = true;

				System.Collections.IList coll = ShapePointsDG.SelectedItems;
				List<int> PointIdList = new List<int>();
				for (int i = 0; i < coll.Count; i++)
				{
					System.Data.DataRowView dr = (System.Data.DataRowView)coll[i];
					int a = 0;
					if (int.TryParse(dr.Row[dr.Row.ItemArray.Length - 1].ToString(), out a))
						PointIdList.Add(a);
				}
				RWrapper.RW_ShapeParameter.MYINSTANCE().SelectPointsList(PointIdList.ToArray());
				PointIdList = null;
				Processing_FGPtSelection = false;
			}
			catch (Exception ex)
			{
				Processing_FGPtSelection = false;
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH07", ex);
			}

		}
		//Cell editing ends...for renaming the shape
		void ShapesDG_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
		{
			string EnteredStr = ((TextBox)e.EditingElement).Text;
			RWrapper.RW_ShapeParameter.MYINSTANCE().RenameShape(EnteredStr);
		}
		
		#endregion

		#region Handling framework event

		//One event handler to take care of all updates from framework
		void ShapeStatusWin_ShapeParameterUpdate(RWrapper.RW_Enum.RW_SHAPEPARAMTYPE ShpParam)
		{
			try
			{
				if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
				{
					switch (ShpParam)
					{
						case RWrapper.RW_Enum.RW_SHAPEPARAMTYPE.SHAPE_TABLE_UPDATE:
							UpdateShapeListTable();
							break;
						case RWrapper.RW_Enum.RW_SHAPEPARAMTYPE.SHAPE_ADDED:
							AddShape();
							break;
						case RWrapper.RW_Enum.RW_SHAPEPARAMTYPE.SHAPE_REMOVED:
							RemoveShape();
							break;
						case RWrapper.RW_Enum.RW_SHAPEPARAMTYPE.SHAPE_UPDATE:
							UpdateShape();
							break;
						case RWrapper.RW_Enum.RW_SHAPEPARAMTYPE.SHAPE_POINTS_UPDATE:
							UpdateShapePoints();
							break;
						case RWrapper.RW_Enum.RW_SHAPEPARAMTYPE.SHAPE_SELECTION_CHANGED:
							ChangeShapeSelection();
							break;
						case RWrapper.RW_Enum.RW_SHAPEPARAMTYPE.SHAPE_HIDE_UNHIDE:
							HideUnhideShape();
							break;
					}
				}
				else
				{
					this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
					new RWrapper.RW_ShapeParameter.ShapeParameterEventHandler(ShapeStatusWin_ShapeParameterUpdate), ShpParam);
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH08", ex); }
		}
		//Handling the update of the shape list table
		void UpdateShapeListTable()
		{
			try
			{
				DGSourcing.Clear();
				ShapeEntities TmpSEntity;

				System.Data.DataView TmpTb = RWrapper.RW_ShapeParameter.MYINSTANCE().ShapeParameterTable.DefaultView;                
				for (int i = 0; i < TmpTb.Count; i++)
				{
                    //if (RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList.Count == 0) continue;
                    TmpSEntity = new ShapeEntities();
					TmpSEntity.ID = Convert.ToInt16(TmpTb[i].Row[0].ToString());
					TmpSEntity.NameStr = TmpTb[i].Row[1].ToString();
					TmpSEntity.TypeStr = TmpTb[i].Row[2].ToString();
					//TmpSEntity.IsHiddenShape = Convert.ToBoolean(RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[3]);                    
					TmpSEntity.Param1Str = TmpTb[i].Row[4].ToString();
					TmpSEntity.Param2Str = TmpTb[i].Row[5].ToString();
					TmpSEntity.Param3Str = TmpTb[i].Row[6].ToString();
					TmpSEntity.Param4Str = TmpTb[i].Row[7].ToString();
					TmpSEntity.Param5Str = TmpTb[i].Row[8].ToString();
					TmpSEntity.Param6Str = TmpTb[i].Row[9].ToString();
					TmpSEntity.Param7Str = TmpTb[i].Row[10].ToString();
					TmpSEntity.Param8Str = TmpTb[i].Row[11].ToString();

					DGSourcing.Add(TmpSEntity);
					UpdateMaxColumns(TmpSEntity.NumberOfParameters());
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH09", ex); }
		}
		//New shape added
		void AddShape()
		{
			try
			{
                if (RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList.Count == 0) return;

				int ShpId = Convert.ToInt16(RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[0]);              
				//for (int i = 0; i < DGSourcing.Count; i++)
				//    if (DGSourcing[i].ID == ShpId)
				//        return;
				ShapeEntities TmpSEntity;
				TmpSEntity = new ShapeEntities();
				TmpSEntity.ID = ShpId;
				TmpSEntity.NameStr = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[1].ToString();
				TmpSEntity.TypeStr = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[2].ToString();
				TmpSEntity.IsHiddenShape = Convert.ToBoolean(RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[3]);                
				TmpSEntity.Param1Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[4].ToString();
				TmpSEntity.Param2Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[5].ToString();
				TmpSEntity.Param3Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[6].ToString();
				TmpSEntity.Param4Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[7].ToString();
				TmpSEntity.Param5Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[8].ToString();
				TmpSEntity.Param6Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[9].ToString();
				TmpSEntity.Param7Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[10].ToString();
				TmpSEntity.Param8Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[11].ToString();
				
				DGSourcing.Add(TmpSEntity);
				UpdateMaxColumns(TmpSEntity.NumberOfParameters());
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH09a", ex); }
		}
		//Shape removed
		void RemoveShape()
		{
			try
			{
				for (int i = 0; i < DGSourcing.Count; i++)
				{
					if (DGSourcing[i].ID == RWrapper.RW_ShapeParameter.MYINSTANCE().Shape_EntityID)
						DGSourcing.Remove(DGSourcing[i]);
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH09b", ex); }
		}
		//Update properties of a shape
		void UpdateShape()
		{
			try
			{
				int ShpId = -1;
				if (RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList.Count > 0)
					ShpId = Convert.ToInt16(RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[0]);                
				for (int i = 0; i < DGSourcing.Count; i++)
				{
					if (DGSourcing[i].ID == ShpId)
					{
						DGSourcing[i].NameStr = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[1].ToString();
						DGSourcing[i].TypeStr = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[2].ToString();
						DGSourcing[i].IsHiddenShape = Convert.ToBoolean(RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[3]);
						DGSourcing[i].Param1Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[4].ToString();
						DGSourcing[i].Param2Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[5].ToString();
						DGSourcing[i].Param3Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[6].ToString();
						DGSourcing[i].Param4Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[7].ToString();
						DGSourcing[i].Param5Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[8].ToString();
						DGSourcing[i].Param6Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[9].ToString();
						DGSourcing[i].Param7Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[10].ToString();
						DGSourcing[i].Param8Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[11].ToString();
						UpdateMaxColumns(DGSourcing[i].NumberOfParameters());
					}
				}               
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH09c", ex); }
		}
		//Update the DG that shows shape points
		void UpdateShapePoints()
		{
			try
			{
				if (RWrapper.RW_ShapeParameter.MYINSTANCE().ShapePointsTable == null)
				{
					ShapePointsDG.Items.Clear();
					return;
				}

				ShapePointsDG.ItemsSource = RWrapper.RW_ShapeParameter.MYINSTANCE().ShapePointsTable.DefaultView;
				if (ShapePointsDG.Items.Count > 0)
					if (ShapePointsDG.Columns.Count > 0)
						ShapePointsDG.Columns[ShapePointsDG.Columns.Count - 1].Visibility = System.Windows.Visibility.Hidden;
				ScrollAppropriateShapePoint_IntoView();
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH10", ex); }
		}
		//Hide unhide of a shape
		void HideUnhideShape()
		{
			try
			{
				int ShpId = Convert.ToInt16(RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[0]);
				for (int i = 0; i < DGSourcing.Count; i++)
				{
					if (DGSourcing[i].ID == ShpId)
					{
						DGSourcing[i].NameStr = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[1].ToString();
						DGSourcing[i].TypeStr = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[2].ToString();
						DGSourcing[i].IsHiddenShape = Convert.ToBoolean(RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[3]);
						DGSourcing[i].Param1Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[4].ToString();
						DGSourcing[i].Param2Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[5].ToString();
						DGSourcing[i].Param3Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[6].ToString();
						DGSourcing[i].Param4Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[7].ToString();
						DGSourcing[i].Param5Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[8].ToString();
						DGSourcing[i].Param6Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[9].ToString();
						DGSourcing[i].Param7Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[10].ToString();
						DGSourcing[i].Param8Str = RWrapper.RW_ShapeParameter.ShapeParameter_ArrayList[11].ToString();
						UpdateMaxColumns(DGSourcing[i].NumberOfParameters());
					}
				}
				ShapesDG.SelectedIndex = -1;
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH09c", ex); }
		}
		//handling the selection change for shapes from framework.
		void ChangeShapeSelection()
		{
			try
			{
				if (ProcessingSelection) return;
				ProcessingSelection = true;

				//deselect all the rows
				ShapesDG.SelectedItems.Clear();
				//Get the refrence to the selected shapes list from framewwork
				List<int> FrameworkShapeList = RWrapper.RW_ShapeParameter.MYINSTANCE().SelectedShapeIDList;
				for (int i = 0; i < DGSourcing.Count; i++)
					for (int j = 0; j < FrameworkShapeList.Count; j++)
						if (((ShapeEntities)DGSourcing[i]).ID == FrameworkShapeList[j])
						{
							ShapesDG.SelectedItems.Add(DGSourcing[i]);
							break;
						}
				ScrollAppropriateShape_IntoView();

				ProcessingSelection = false;
			}
			catch (Exception ex)
			{
				ProcessingSelection = false;
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH11", ex);
			}
		}
		//handling the renaming of the shapes.
		public void RenameShape()
		{
			try
			{
				if (ShapesDG.SelectedItems.Count != 1)
				{
					MessageBox.Show("Please select a single shape and then rename", "Rapid I");
					return;
				}
				//Show the shape rename window.
				ShapeRenameWin renameWin = new ShapeRenameWin();
				renameWin.txtNewName.Text = ((ShapeEntities)ShapesDG.SelectedItem).NameStr;
				renameWin.ShowDialog();
				if (renameWin.Result)
				{
					string SName = renameWin.txtNewName.Text.Replace(" ", "_");
					RWrapper.RW_ShapeParameter.MYINSTANCE().RenameShape(SName);
					((ShapeEntities)ShapesDG.SelectedItem).NameStr = SName;
				}
				renameWin.Hide();
				renameWin.Close();
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH12", ex); }
		}
		//Handlind the selection of FG points that are shown for a particular shape
		void ShapePoints_Selection(List<int> PtIDList)
		{
			try
			{
				if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
				{
					if (Processing_FGPtSelection) return;
					Processing_FGPtSelection = true;

					//deselect all the rows
					ShapePointsDG.SelectedItems.Clear();

					if (ShapePointsDG.Columns.Count > 0)
						for (int i = 0; i < ShapePointsDG.Items.Count; i++)
							for (int j = 0; j < PtIDList.Count; j++)
								if (Convert.ToInt32(((System.Data.DataRowView)ShapePointsDG.Items[i]).Row[ShapePointsDG.Columns.Count - 1].ToString()) == PtIDList[j])
								{
									ShapePointsDG.SelectedItems.Add(ShapePointsDG.Items[i]);
									ShapePointsDG.ScrollIntoView(ShapePointsDG.Items[i]);
									break;
								}

					Processing_FGPtSelection = false;
				}
				else
				{
					this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
					new RWrapper.RW_ShapeParameter.SelectFgPtEventHandler(ShapePoints_Selection), PtIDList);
				}
			}
			catch (Exception ex)
			{
				Processing_FGPtSelection = false;
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH13", ex);
			}
		}
	  
		#endregion

		//Update the maximum number of shape param columns that needs to be visible
		void UpdateMaxColumns(int TmpShpParams)
		{
			try
			{
				if (MaxNumOfShpParams < TmpShpParams)
				{
					DataGridColumn[] ShpDGCols = { Param1Col, Param2Col, Param3Col, Param4Col, Param5Col, Param6Col, Param7Col, Param8Col };
					for (int i = MaxNumOfShpParams; i < TmpShpParams; i++)
						ShpDGCols[i].Visibility = System.Windows.Visibility.Visible;
					MaxNumOfShpParams = TmpShpParams;
				}
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH15", ex); }
		}
		//Scroll to view either selected shape or last shape
		public void ScrollAppropriateShape_IntoView()
		{
			if (ShapesDG.SelectedItems.Count > 0)
				ShapesDG.ScrollIntoView(ShapesDG.SelectedItems[ShapesDG.SelectedItems.Count - 1]);
			else if (ShapesDG.Items.Count > 0)
				ShapesDG.ScrollIntoView(ShapesDG.Items[ShapesDG.Items.Count - 1]);
		}
		//Scroll to view either selected point or last point
		public void ScrollAppropriateShapePoint_IntoView()
		{
			if (ShapePointsDG.SelectedItems.Count > 0)
				ShapePointsDG.ScrollIntoView(ShapePointsDG.SelectedItems[ShapePointsDG.SelectedItems.Count - 1]);
			else if (ShapePointsDG.Items.Count > 0)
				ShapePointsDG.ScrollIntoView(ShapePointsDG.Items[ShapePointsDG.Items.Count - 1]);
		}
		//Clear all function
		public void ClearAll()
		{
			try
			{
				DGSourcing.Clear();
				ShapePointsDG.ItemsSource = null;
				OnShowHideFG("FG Points Hidden");

				DataGridColumn[] ShpDGCols = { Param1Col, Param2Col, Param3Col, Param4Col, Param5Col, Param6Col, Param7Col, Param8Col };
				for (int i = 0; i < ShpDGCols.Length; i++)
					ShpDGCols[i].Visibility = System.Windows.Visibility.Collapsed;
				MaxNumOfShpParams = 0;
			}
			catch (Exception ex)
			{ RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SH16", ex); }
		}
	}
}
