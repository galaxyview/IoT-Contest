using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Iotfitness
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        static void Main()
        {
            Serial mySerial = new Serial(1000);
            mySerial.showPorts();
            mySerial.openPort();
            while(true)
            {

            }
        }
    }
}
