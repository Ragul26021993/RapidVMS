using System;
using System.Collections.Generic;
using System.Data;
using System.Windows.Forms;
using System.Xml;
using System.Collections;
using System.Data.OleDb;

namespace RapidI
{
    class XML_Exim
    {

        #region initialisation
        //String[] TableNames = new string[] {"UserSettings", "DXFSettings", "MeasurementSettings", "LightingSettings", "FixtureDetails",
        //                "MachineDetails", "VisionSettings", "MagLevels", "MagnifierSettings", 
        //                "DROSettings", "EEPROMSettings","CalibrationSettings","FocusScanCalibration","CameraAxisValue"};
        //List<string> TableNames = new List<string>();
        DataSet XmlDs;
        DataTable schematable = new DataTable();
        System.Data.OleDb.OleDbDataAdapter adapter;
        System.Data.OleDb.OleDbDataAdapter DataA;
        //System.Data.OleDb.OleDbDataAdapter SchemaDataA;
        System.Data.OleDb.OleDbCommandBuilder CommB;
        int tablecount = 0, rowcount = 0;
        public string MachineNo;
        OpenFileDialog OpenXmlFile = new OpenFileDialog();
        string StatusMessage = "";
        System.Windows.Controls.TextBox DisplayBox;
        #endregion

        //Thread td;//Thread to update the tables and show the status in Textbox;
        delegate void callbacksetTextbox(string s);

        public XML_Exim(System.Windows.Controls.TextBox displayer1)
        {
            DisplayBox = displayer1;
        }

        //Import Button click Event Function
        public void ImportXML()
        {
            bool check = false;
            OpenXmlFile.Title = "Select Database Xml File";
            OpenXmlFile.InitialDirectory = System.Environment.GetFolderPath(Environment.SpecialFolder.MyComputer);
            OpenXmlFile.Filter = "XML|*.xml";
            OpenXmlFile.ShowDialog();
            string Filepath = OpenXmlFile.FileName;
            if (Filepath != "")
            {
                XmlDs = new DataSet();
                check = readXML(Filepath);
                if (check)
                {
                    //string message = "Do you want to update the current Database tables...";
                    //string caption = "Database Update";
                    //MessageBoxButtons buttons = MessageBoxButtons.YesNo;
                    //MessageBoxIcon icon = MessageBoxIcon.Warning;
                    //DialogResult result;
                    //// Displays the MessageBox.
                    //result = MessageBox.Show(message, caption, buttons, icon);
                    //if (result == System.Windows.Forms.DialogResult.Yes)
                    //{
                        //td = new Thread(CopyData);
                        //td.Start();
                        CopyData();
                    //}
                }
                else
                {
                    MessageBox.Show("Your XML was probably bad...");
                }
            }
        }
        //Read the Xml File
        private bool readXML(string Filepath)
        {
            bool correctXml = false;
            try
            {
                tablecount = 0; rowcount = 0;
                XmlDs = new DataSet();
                XmlTextReader reader = new XmlTextReader(Filepath);
                reader.WhitespaceHandling = WhitespaceHandling.None;
                XmlDocument xmlDoc = new XmlDocument();
                xmlDoc.Load(reader);
                reader.Close();
                XmlNode xnod = xmlDoc.DocumentElement;
                //This is been hardcoded at the time of generation of XML ...
                //Just checking it here whether we are importing Correct XML file.
                if (xnod.Name.Substring(0, 9) == "MachineNo")
                {
                    //Reading the first XML Element to find out the Machine Number from length 9 to end of string .It is been done in such manner because the machine Number 
                    // can be integer or a string or combination of character as been told to me.
                    MachineNo = xnod.Name.Substring(9);
                    AddXMLTablesToDs(xnod, 1);
                    correctXml = true;
                }
                else
                {
                    DisplayBox.Text = "Your XML was probably bad...";
                    correctXml = false;
                }
            }
            catch (XmlException exception)
            {
                MessageBox.Show("Your XML was probably bad...");
                correctXml = false;
            }
            return correctXml;
        }

        //Transform Xml tables to System.data.dataset Tables
        //Dont make any change to this function it will return the dataset as a collection of tables from the XML .......
        //By default there are 5 levels in the XML that is generated from front end 
        private void AddXMLTablesToDs(XmlNode xnod, Int32 intLevel)
        {
            XmlNode xnodWorking;
            if (intLevel == 5)
            {
                if (XmlDs.Tables[tablecount - 1].Rows.Count < rowcount)
                    XmlDs.Tables[tablecount - 1].Rows.Add();
                XmlDs.Tables[tablecount - 1].Rows[rowcount - 1][xnod.ParentNode.Name] = xnod.Value;
            }
            if (xnod.NodeType == XmlNodeType.Element)
            {
                XmlNamedNodeMap mapAttributes = xnod.Attributes;
                if (xnod.Name == "DataRow")
                {
                    rowcount++;
                }
                foreach (XmlNode xnodAttribute in mapAttributes)
                {
                    if (xnod.Name == "Table")
                    {
                        DataTable dt = new DataTable(xnodAttribute.Value);
                        XmlDs.Tables.Add(dt);
                        tablecount++;
                        rowcount = 0;
                    }
                    else if (intLevel == 4)
                    {
                        if (rowcount == 1)
                        {
                            XmlDs.Tables[tablecount - 1].Columns.Add(xnod.Name, Type.GetType(xnodAttribute.Value));
                        }
                    }
                }
                if (xnod.HasChildNodes)
                {
                    xnodWorking = xnod.FirstChild;
                    while (xnodWorking != null)
                    {
                        AddXMLTablesToDs(xnodWorking, intLevel + 1);
                        xnodWorking = xnodWorking.NextSibling;
                    }
                }
            }
        }
        //Returns the table with desired tablename and according to Compare result (i.e rows found after the Where Clause Constraints)
        private DataTable GetDatabaseTable(string CapmareTo, string CompareValue, string TableName)
        {
            System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection();
            con.ConnectionString = RWrapper.RW_MainInterface.DBConnectionString;
            string query = "Select * from " + TableName + " where " + CapmareTo + " = '" + CompareValue + "'";
            con.Open();
            adapter = new System.Data.OleDb.OleDbDataAdapter(query, con);
            DataTable DT = new DataTable(TableName);
            adapter.Fill(DT);
            con.Close();
            return DT;
        }
        //To copy XmlTable to Original table and original table to Backup Table.
        private void CopyData()
        {
            for (int i = 0; i < XmlDs.Tables.Count; i++)
            {
                DataTable OriginalTable = new DataTable();
                DataTable BackupTable = new DataTable();
                DataTable XmlTable = new DataTable();
                OriginalTable = GetDatabaseTable("MachineNo", MachineNo, XmlDs.Tables[i].TableName);
                //try
                //{
                //    BackupTable = GetDatabaseTable("MachineNo", MachineNo, TableNames[i] + "_BK");
                //}
                //catch (Exception ex)
                //{
                //    createTable(TableNames[i]);
                //    BackupTable = GetDatabaseTable("MachineNo", MachineNo, TableNames[i] + "_BK");
                //}
                XmlTable = XmlDs.Tables[i];
                /*Check if Entries are already there in Original Tables . If there are related rows then 
                 first copy the rows to backup table and then Update the original Tables From Xml File . If No related Entries are found in Original Tables
                 then This means the new Mahine is been Added so first add the entries to original table  and then to backup tables.*/

                if (OriginalTable.Rows.Count == 0)
                {
                    //Copy Xml Data to Current Table
                    CopyTables(XmlTable, OriginalTable, false);
                    //Copy Current Data To Backuptable
                    //CopyTables(OriginalTable, BackupTable, true);
                }
                else
                {
                    //First Copy Current Data To Backuptable
                    //CopyTables(OriginalTable, BackupTable, true);
                    //Now remove all the rows from Original table
                    Delete_table_Rows("MachineNo", MachineNo, OriginalTable.TableName);
                    //Get the Latest Table into DataTable
                    OriginalTable = GetDatabaseTable("MachineNo", MachineNo, OriginalTable.TableName);
                    //Copy Xml Data to Current Table
                    CopyTables(XmlTable, OriginalTable, false);
                }
            }
            MessageBox.Show("Database Updated");
            //td.Abort();
        }
        //This function is used to update the table in database
        private bool Update_Table(string TableName, DataTable Table)
        {
            try
            {
                DataA = new System.Data.OleDb.OleDbDataAdapter("SELECT * FROM [" + TableName + "]", RWrapper.RW_MainInterface.DBConnectionString);
                CommB = new System.Data.OleDb.OleDbCommandBuilder(DataA);
                DataA.Update(Table);
                return true;
            }
            catch (Exception ex)
            {
                return false;
            }
        }

        private void Delete_table_Rows(string CapmareTo, string CompareValue, string TableName)
        {
            System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection();
            con.ConnectionString = RWrapper.RW_MainInterface.DBConnectionString;
            try
            {
                System.Data.OleDb.OleDbCommand cmd = new System.Data.OleDb.OleDbCommand();
                string query = "Delete * from " + TableName + " where " + CapmareTo + " = '" + CompareValue + "'";
                cmd.CommandText = query;
                cmd.Connection = con;
                con.Open();
                cmd.ExecuteNonQuery();
                //adapter = new System.Data.OleDb.OleDbDataAdapter(query, con);           
                con.Close();
            }
            catch (Exception ex)
            {
                con.Close();
            }
        }
        //Function to copy rows from One table to another
        private void CopyTables(DataTable Table_Source, DataTable Table_Destination, bool IsDestinationBackupTable)
        {
            int SourceRow_Count = Table_Source.Rows.Count;
            int DestinationRow_Count = Table_Destination.Rows.Count;
            //to check whether update was successful or not  
            bool status;

            //check... if rows are already there then Update those rows else Insert new rows . 
            if (DestinationRow_Count == 0)
            {
                for (int i = 0; i < SourceRow_Count; i++)
                {
                    Table_Destination.Rows.Add(setdefaultvalues(Table_Destination));
                    if (IsDestinationBackupTable)
                    {
                        Table_Destination.Rows[i]["Version"] = 0;
                        StatusMessage += "\r\n Copying " + Table_Source.TableName + " To  " + Table_Destination.TableName + "...";
                    }
                    else
                    {
                        StatusMessage += "\r\n Update Table " + Table_Destination.TableName + "...";
                    }

                    for (int j = 0; j < Table_Source.Columns.Count; j++)
                    {
                        string Collumn_name = Table_Source.Columns[j].ColumnName;
                        try
                        {
                            Table_Destination.Rows[i][Collumn_name] = Table_Source.Rows[i][Collumn_name];
                        }
                        catch (Exception ex)
                        {
                            //Table_Destination.Columns.Add(Collumn_name);
                            //Addcolumn(Table_Destination.TableName, Collumn_name, Table_Source);
                            //Table_Destination.Rows[i][Collumn_name] = Table_Source.Rows[i][Collumn_name];
                        }
                    }
                }
                status = Update_Table(Table_Destination.TableName, Table_Destination);
                StatusMessage += StatusToResultconvertor(status);
            }
            else
            {
                int rowposition = 0;
                for (int i = 0; i < SourceRow_Count; i++)
                {
                    if (IsDestinationBackupTable)
                    {
                        rowposition = DestinationRow_Count;
                        Table_Destination.Rows.Add(setdefaultvalues(Table_Destination));
                        Table_Destination.Rows[rowposition + i]["Version"] = DestinationRow_Count / SourceRow_Count;
                        StatusMessage += "\r\n Copying " + Table_Source.TableName + "To  " + Table_Destination.TableName + "...";
                    }
                    else
                    {
                        StatusMessage += "\r\n Update Table " + Table_Destination.TableName + "...";
                    }
                    for (int j = 0; j < Table_Source.Columns.Count; j++)
                    {
                        string Collumn_name = Table_Source.Columns[j].ColumnName;
                        try
                        {
                            Table_Destination.Rows[rowposition + i][Collumn_name] = Table_Source.Rows[i][Collumn_name];
                        }
                        catch (Exception ex)
                        {
                            StatusMessage += "\r\n  " + Collumn_name + "Does not Exist in   " + Table_Destination.TableName + "...";
                            Table_Destination.Columns.Add(Collumn_name);
                            StatusMessage += "\r\n Creating " + Collumn_name + "in   " + Table_Destination.TableName + "...";
                            Addcolumn(Table_Destination.TableName, Collumn_name, Table_Source);
                            Table_Destination.Rows[rowposition + i][Collumn_name] = Table_Source.Rows[i][Collumn_name];
                        }
                    }
                }
                status = Update_Table(Table_Destination.TableName, Table_Destination);
                StatusMessage += StatusToResultconvertor(status);
            }
            DisplayStatus(StatusMessage);
        }
        //Function to write in textbox about current status of table
        public void DisplayStatus(string str)
        {
            DisplayBox.Text = str;
            DisplayBox.Select(DisplayBox.Text.Length, 0);
            DisplayBox.ScrollToEnd();
        }

        //This function convert true false to Sucess and Failed respectively .
        string StatusToResultconvertor(bool status)
        {
            if (status == true)
                return "Successful";
            else
                return "Failed";
        }

        //This will set the default values in to the new row that is to be inserted
        private DataRow setdefaultvalues(DataTable Table)
        {
            System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection(RWrapper.RW_MainInterface.DBConnectionString);
            DataRow newCustomersRow;
            newCustomersRow = Table.NewRow();
            try
            {
                Hashtable Temp_ListString = new Hashtable();
                con.Open();
                DataTable dtTable = con.GetOleDbSchemaTable(OleDbSchemaGuid.Columns, new object[] { null, null, Table.TableName, null });
                con.Close();
                for (int i = 0; i < dtTable.Rows.Count; i++)
                {
                    if (GetDataType(Convert.ToInt32(dtTable.Rows[i]["DATA_TYPE"])) == "BIT")
                    {
                        if (dtTable.Rows[i][8].ToString() == "0")
                        {
                            Temp_ListString.Add(dtTable.Rows[i][3], false);
                        }
                        else
                            Temp_ListString.Add(dtTable.Rows[i][3], true);
                    }
                    else
                        Temp_ListString.Add(dtTable.Rows[i][3], dtTable.Rows[i][8]);
                }
                for (int i = 0; i < Table.Columns.Count; i++)
                {
                    if (Temp_ListString[Table.Columns[i].ColumnName].ToString() != "")
                    {
                        newCustomersRow[Table.Columns[i].ColumnName] = Temp_ListString[Table.Columns[i].ColumnName];
                    }
                }
            }
            finally
            {
                con.Close();
            }
            return newCustomersRow;
        }

        //This will create Backup table if it doesnot Exist
        private void createTable(string sampletable)
        {
            List<string> Tablename = new List<string>();
            string NewTable = sampletable + "_BK";
            System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection(RWrapper.RW_MainInterface.DBConnectionString);
            System.Data.OleDb.OleDbCommand cmd = new OleDbCommand();
            con.Open();
            DataTable dtTable1 = con.GetOleDbSchemaTable(OleDbSchemaGuid.Tables, new object[] { null, null, null, "TABLE" });
            con.Close();
            for (int i = 0; i < dtTable1.Rows.Count - 1; i++)
            {
                Tablename.Add(dtTable1.Rows[i][2].ToString());
            }
            bool exist = false;
            foreach (string tbname in Tablename)
            {
                if (tbname == NewTable)
                {
                    exist = true;
                    return;
                }
            }
            if (exist == false)
            {
                con.Open();
                string query = "CREATE TABLE " + NewTable + " AS SELECT * FROM " + sampletable + " WITH NO DATA";
                cmd.CommandText = query;
                cmd.Connection = con;
                try
                {
                    cmd.ExecuteNonQuery();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        public struct collumnAttribute
        {
            public string Name;
            public string datatype;
            public string defaultvalues;
        }
        //return the datatype of Collumn from Table Schema
        private string GetDataType(int? OLEDataType, string ColumnSize, int? NumericPrecision, int? NumericScale)
        {
            string CS = ColumnSize;
            switch (OLEDataType)
            {
                case 2:
                    return "Integer";
                case 3: //Long
                    switch (CS)
                    {
                        case "-1":
                            return "Long";
                        case "-2": //AutoNumber
                            return "counter(1,1)";
                        default:
                            return "Long";
                    }
                case 4:
                    return "Single";

                case 5:
                    return "Double";

                case 6:
                    return "Money";//CURRECY

                case 7:
                    return "DATETIME";

                case 11: //Yes/No fields
                    return "BIT";

                case 17:
                    return "BYTE";

                case 72:
                    return "MEMO";

                case 130:
                    if (CS == "0")
                    {
                        return "MEMO";

                    }
                    else if (CS == "-1")
                    {
                        return "MEMO";
                    }
                    else
                    {
                        return "VARCHAR(" + CS + ")";
                    }

                case 131:
                    return "decimal(" + NumericPrecision + "," + NumericScale + ")"; //decimal

                case 128:
                    if (ColumnSize == "-1")
                    {
                        return "MEMO";
                    }
                    else if (ColumnSize == "0")
                    {
                        return "MENO";
                    } //OLE Object
                    else
                        return "VARCHAR(" + ColumnSize + ")";
                default:
                    if (ColumnSize == "-1")
                    {
                        return "MEMO";
                    }
                    else
                        return "VARCHAR(" + ColumnSize + ")";
            }
        }
        private string GetDataType(int OLEDataType)
        {
            switch (OLEDataType)
            {
                case 11:
                    return "BIT";
                case 2:
                    return "Integer";
                case 4:
                    return "Single";

                case 5:
                    return "Double";

                case 6:
                    return "Money";//CURRECY

                case 7:
                    return "DATETIME";

                case 17:
                    return "BYTE";

                case 72:
                    return "MEMO";
                default:
                    return null;
            }
        }

        //Add the Collumn into the table
        private void Addcolumn(string Tablename, string ColumnName, DataTable referencetable)
        {
            int NumericScale = 0, Numericprec = 0, Datatpe = 0;
            string CollWidth = "", datatype = "Memo";
            System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection(RWrapper.RW_MainInterface.DBConnectionString);
            System.Data.OleDb.OleDbCommand cmd = new OleDbCommand();
            cmd.Connection = con;
            con.Open();
            DataTable dtTable = con.GetOleDbSchemaTable(OleDbSchemaGuid.Columns, new object[] { null, null, referencetable.TableName, null });
            con.Close();
            try
            {
                for (int i = 0; i < dtTable.Rows.Count - 1; i++)
                {
                    if (dtTable.Rows[i][3].ToString() == ColumnName)
                    {
                        Datatpe = Convert.ToInt32(dtTable.Rows[i]["Data_Type"]);
                        if (dtTable.Rows[i]["CHARACTER_MAXIMUM_LENGTH"].ToString() == "true")
                        {
                            CollWidth = dtTable.Rows[i]["CHARACTER_MAXIMUM_LENGTH"].ToString();
                        }
                        if (dtTable.Rows[i]["NUMERIC_PRECISION"] != DBNull.Value)
                        {
                            Numericprec = Convert.ToInt32(dtTable.Rows[i]["NUMERIC_PRECISION"]);
                        }
                        if (dtTable.Rows[i]["NUMERIC_SCALE"] != DBNull.Value)
                        {
                            NumericScale = Convert.ToInt32(dtTable.Rows[i]["NUMERIC_SCALE"]);
                        }
                        datatype = GetDataType(Datatpe, CollWidth, Numericprec, NumericScale);
                        try
                        {
                            con.Open();
                            string query = "ALTER TABLE " + Tablename + " ADD COLUMN " + ColumnName + " " + datatype;
                            cmd.CommandText = query;
                            cmd.ExecuteNonQuery();
                            con.Close();
                            return;
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show(ex.Message);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}


