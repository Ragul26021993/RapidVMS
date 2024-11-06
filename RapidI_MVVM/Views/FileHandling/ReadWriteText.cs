using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Rapid.IO
{
    public static class ReadWriteText
    {
        public static void ReadFileWhenWindowOpens(string FilePath, ref List<WindowsParameters<string>> ParametersList)
        {
            if (System.IO.File.Exists(FilePath))
            {
                using (System.IO.TextReader tr = new System.IO.StreamReader(FilePath))
                {
                    string ReadLine = tr.ReadLine();
                    while (ReadLine != null)
                    {
                        string[] SavedValue = ReadLine.Split(':');
                        if (SavedValue.Length > 1)
                        {
                            SavedValue[0] = SavedValue[0].Replace(" ", "");
                        }
                        try
                        {
                            foreach (WindowsParameters<string> WP in ParametersList)
                            {
                                if (WP.Name == SavedValue[0])
                                {
                                    WP.Value = SavedValue[1];
                                }
                            }
                        }
                        catch (Exception)
                        {
                        }
                        ReadLine = tr.ReadLine();
                    }
                }
            }
        }
        public static void WriteFileWhenWindowClose(string FilePath, List<WindowsParameters<String>> ParametersList)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("==================================================Last User Input==================================================");
            for (int i = 0; i < ParametersList.Count; i++)
            {
                sb.AppendLine(ParametersList[i].Name.ToString() + ":" + ParametersList[i].Value);
            }
            System.IO.StreamWriter file = new System.IO.StreamWriter(FilePath);
            file.WriteLine(sb);
            file.Close();
        }
    }
    public class WindowsParameters<T>
    {
        public string Name = "";
        public T Value;
        public WindowsParameters(string PropertyName, T PropertyValue)
        {
            Name = PropertyName;
            Value = PropertyValue;
        }
    }
}