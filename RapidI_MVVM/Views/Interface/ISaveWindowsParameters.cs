using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Rapid.Interface
{
    public interface ISaveWindowsParameters
    {
        string TextFilePath { get; set; }
        void SaveParameters();
        void ReadParameters();
    }
}
