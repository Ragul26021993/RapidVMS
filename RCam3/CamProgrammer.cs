using System;
using System.Reflection;
using System.IO;

namespace RCam3
{
    public class CamProgrammer
    {

        //private Assembly cyusbAssembly;
        private String[] AsbyNames;
        public CamProgrammer()
        {
            //Get CyUSB.dll from embedded resource and then load to memory
            try
            {
                AsbyNames = Assembly.GetExecutingAssembly().GetManifestResourceNames();
                //AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(CurrentDomain_AssemblyResolve);
                for (int i = 0; i < AsbyNames.Length; i++)
                {
                    if (AsbyNames[i].EndsWith("dll") )
                        WriteFileToDisk(AsbyNames[i]);
                }

                //Assembly.Load()
            }
            catch (Exception ex)
            {
                String ss = ex.Message;
            }
        }

        //~CamProgrammer()
        //{
            
        //}

        private static byte[] StreamToBytes(Stream input)
        {
            int capacity = input.CanSeek ? (int)input.Length : 0;
            using (MemoryStream output = new MemoryStream(capacity))
            {
                int readLength;
                byte[] buffer = new byte[4096];

                do
                {
                    readLength = input.Read(buffer, 0, buffer.Length);
                    output.Write(buffer, 0, readLength);
                }
                while (readLength != 0);

                return output.ToArray();
            }
        }

        Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            //return EmbeddedAssembly.Get(args.Name);
            byte[] cyusbData = new byte[4096];

            int i = 0;
            for (i = 0; i < AsbyNames.Length; i++)
            {
                if (AsbyNames[i].Contains("CyUSB"))
                {
                    using (Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(AsbyNames[i]))
                    {
                        cyusbData = StreamToBytes(stream);
                        stream.Close();
                    }
                }
                //else // all other embedded files are to be written to disk...
                //{
                //    WriteFileToDisk(AsbyNames[i]);
                //}
            }
            //Assembly.GetExecutingAssembly();

            return Assembly.Load(cyusbData);
        }

        private static bool WriteFileToDisk(String FileName)
        {
            try
            {
                Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(FileName);
                int Length = (int)stream.Length;
                byte[] fileData = new byte[Length];

                stream.Read(fileData, 0, Length);
                File.WriteAllBytes(AppDomain.CurrentDomain.BaseDirectory + FileName, fileData);
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        public bool Program_Cam(int CamType) //, bool FlipHorizontal, bool FlipVertical)
        {
            bool ProgStatus = false;
            try
            {
                //Program the camera with the correct .img file
                CyUSB.USBDeviceList usbDevices = new CyUSB.USBDeviceList(CyUSB.CyConst.DEVICES_CYUSB);
                CyUSB.CyFX3Device fx;
                foreach (CyUSB.USBDevice uDevice in usbDevices)
                {
                    CyUSB.CyUSBDevice chosenDevice = uDevice as CyUSB.CyUSBDevice;
                    //if (chosenDevice.bSuperSpeed)
                    //{                //USB_DEVICE_DESCRIPTOR descriptor = new USB_DEVICE_DESCRIPTOR();
                    if (chosenDevice.FriendlyName.Contains("Cypress FX3"))
                    {
                        fx = chosenDevice as CyUSB.CyFX3Device;
                        // check for bootloader first, if it is not running then prompt message to user.
                        if (!fx.IsBootLoaderRunning())
                        {
                            System.Windows.Forms.MessageBox.Show("Please reset your device to download firmware", "Bootloader is not running");
                            return false;
                        }
                        CyUSB.FX3_FWDWNLOAD_ERROR_CODE enmResult = CyUSB.FX3_FWDWNLOAD_ERROR_CODE.FAILED;
                        if (CamType == 2) //FlipHorizontal && !FlipVertical)
                            enmResult = fx.DownloadFw(AppDomain.CurrentDomain.BaseDirectory + "\\RCam3.rcam3_2M.dll", CyUSB.FX3_FWDWNLOAD_MEDIA_TYPE.RAM);
                        else if (CamType == 10)
                            enmResult = fx.DownloadFw(AppDomain.CurrentDomain.BaseDirectory + "\\RCam3.rcam3_10M.dll", CyUSB.FX3_FWDWNLOAD_MEDIA_TYPE.RAM);
                        //fx.ReConnect();
                        //fx.IsBootLoaderRunning();
                        System.Threading.Thread.Sleep(5000);
                        //Now that programming is Over, let us delete the img files
                        //for (int i = 0; i < AsbyNames.Length; i++)
                        //{
                        //    if (File.Exists(AppDomain.CurrentDomain.BaseDirectory + AsbyNames[i]))
                        //        File.Delete(AppDomain.CurrentDomain.BaseDirectory + AsbyNames[i]);
                        //}
                        if (enmResult == CyUSB.FX3_FWDWNLOAD_ERROR_CODE.SUCCESS) ProgStatus = true;

                        //break;
                    }
                    //}
                    //chosenDevice.GetDeviceDescriptor(ref descriptor);
                    //if (descriptor.iManufacturer == 4)
                }

                //Remember to close the memory loaded dll when exiting...
            }
            catch (Exception ex)
            {
                String ss = ex.Message;
                //return false;
            }
            for (int i = 0; i < AsbyNames.Length; i++)
            {
                if (File.Exists(AppDomain.CurrentDomain.BaseDirectory + AsbyNames[i]))
                    File.Delete(AppDomain.CurrentDomain.BaseDirectory + AsbyNames[i]);
            }
            return ProgStatus;

        }
    }
}
