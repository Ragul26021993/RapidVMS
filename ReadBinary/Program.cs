using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReadBinary
{
    class Program
    {
        static void Main(string[] args)
        {
            string filepath = @"C:\ProgramData\Rapid-I 5.0\Log\ErrorLog\LogFile\ErrorLog 17-03-2021.log";

            FileStream fs = new FileStream(filepath, FileMode.Open, FileAccess.Read);
            BinaryReader filebinReader = new BinaryReader(fs, new UnicodeEncoding());
            byte[] memoryData = new byte[fs.Length - fs.Position];
            for (int i = 0; i < memoryData.Length; i++)
            {
                memoryData[i] = (byte)(255 - filebinReader.ReadByte());
            }
            System.Text.UnicodeEncoding ecode = new UnicodeEncoding();
            char[] finalData = ecode.GetChars(memoryData);
            fs.Close();

            string saveFilePath = @"C:\ProgramData\Rapid-I 5.0\Log\ErrorLog\LogFile\converted_"+ DateTime.Now.Millisecond + ".txt";

            //save file
            {
                StreamWriter Swriter = new StreamWriter(saveFilePath, false);
                Swriter.Write(finalData);
                Swriter.Close();
            }
        }
    }
}
