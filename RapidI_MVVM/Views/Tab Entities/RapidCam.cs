using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Rapid.Utilities
{
    class RapidCam
    {

        //E:\\Narayanan\\Software Development\\Cam Works 03 Feb 2015\\Cam Works\\WPF_CameraTester\\WPF_CameraTester\\bin\\Debug\\RapidCam.dll
        //E:\\Narayanan\\Software Development\\Cam Works 03 Feb 2015\\Cam Works\\RapidCamControl ApDS+FX3\\RapidCamControl\\Debug\\
        [DllImport("RapidCam.dll", CharSet=CharSet.Unicode, CallingConvention =CallingConvention.Cdecl) ]
        public static extern bool Initialise(System.IntPtr hwndApp, int CamLabel, int Colorkey);

        [DllImport("RapidCam.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SetVideoWindow(int ImageWidth, int ImageHeight, int FrameRate, bool SetAutoExposure, double DistortionFactor, long offset_x, long offset_Y);

        [DllImport("RapidCam.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SetVideoWindowEx(int ImageWidth, int ImageHeight, int SourceLeft, int SourceTop, int DisplayWidth, int DisplayHeight, int FrameRate, [MarshalAs(UnmanagedType.I1)]bool SetAutoExposure, double DistortionFactor, long offset_x, long offset_Y, [MarshalAs(UnmanagedType.I1)]bool CompressVideoFrame);

        [DllImport("RapidCam.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Preview();

        [DllImport("RapidCam.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void CameraClose();

    }
}
