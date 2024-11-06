using System;
using System.Collections.Generic;
using System.Collections;
using System.Windows.Controls;

namespace Rapid.Utilities
{
    public class RapidiContextMenu
    {
        public delegate void MenuItemClickEventHandler(object Sender, System.Windows.RoutedEventArgs e);
        public MenuItemClickEventHandler OnMenuItemClick;

        public RapidiContextMenu(MenuItemClickEventHandler MenuItemClickEvent)
        {
            if (MenuItemClickEvent != null)
            {
                OnMenuItemClick = MenuItemClickEvent;
            }
        }
        public void GetMenu(List<ContentMenuObjects> MenuObjects, ref System.Windows.Controls.ContextMenu Cm)
        {//Content menu ParentId need to zero always.
            try
            {
                Hashtable MenuObj = new Hashtable();
                MenuObj[0] = Cm;
                foreach (ContentMenuObjects Mo in MenuObjects)
                {
                    if (Mo.isSeperator)
                    {
                        Cm.Items.Add(new Separator());
                        continue;
                    }
                    if (Mo.ParentId == 0)
                    {
                        MenuItem Mi = GetObjectFilled(Mo);
                        MenuObj[Mo.Id] = Mi;
                        (MenuObj[Mo.ParentId] as System.Windows.Controls.ContextMenu).Items.Add(Mi);
                        FillTree(MenuObjects, Mo, ref MenuObj);
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("ContextMenu01", ex); }
        }
        public void FillTree(List<ContentMenuObjects> Source, ContentMenuObjects Me, ref Hashtable MenuObj)
        {
            try
            {
                foreach (ContentMenuObjects Mo in Source)
                {
                    if (Mo.ParentId == Me.Id)
                    {
                        MenuItem Mi = GetObjectFilled(Mo);
                        MenuObj[Mo.Id] = Mi;
                        (MenuObj[Mo.ParentId] as MenuItem).Items.Add(Mi);
                        FillTree(Source, Mo, ref MenuObj);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("ContextMenu02", ex);
            }
        }
        public MenuItem GetObjectFilled(ContentMenuObjects Mo)
        {
            MenuItem Mi = new MenuItem();
            Mi.Header = Mo.ToolTip;
            Mi.ToolTip = Mo.ToolTip;
            Mi.Name = Mo.ContextMenuname;
            Mi.Tag = new string[] { Mo.Alignment, Mo.ItemName };
            if (Mo.Image != null)
            {
                Mi.Icon = Mo.Image;
            }
            if (Mo.HasClickEvent)
            {
                if (OnMenuItemClick != null)
                {
                    Mi.Click += new System.Windows.RoutedEventHandler(OnMenuItemClick);
                }
            }
            return Mi;
        }
    }
}