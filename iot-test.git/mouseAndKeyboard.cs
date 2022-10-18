using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;
namespace Iotfitness
{
    class Keyboard
    {
        const uint KEYEVENTF_EXTENDEDKEY = 0x1;   //按下按键
        const uint KEYEVENTF_KEYUP = 0x2;         //松开按键

        [DllImport("user32.dll")]
        static extern short GetKeyState(int nVirtKey);
        [DllImport("user32.dll")]
        static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, uint dwExtraInfo);


        public static bool GetState(int Key)
        {
            return (GetKeyState((int)Key) == 1);
        }

        //按下一次并松开按键
        public static void SetState(int Key, bool State)
        {
            if (State != GetState(Key))
            {
                keybd_event((byte)Key, 0, KEYEVENTF_EXTENDEDKEY, 0);

                keybd_event((byte)Key, 0, KEYEVENTF_KEYUP, 0);
            }
        }


        //关闭键盘大写
        public static void press(int Key)
        {

            SetState(Key, true);
        }

    }


    static class Mouse
    {
        static int PositionX = Cursor.Position.X;
        static int PositionY = Cursor.Position.Y;
        static int speedX = 0;
        static int speedY = 0;



        //移动鼠标
        [DllImport("user32.dll")]
        static extern bool SetCursorPos(int x, int y);

        //鼠标控制事件
        [DllImport("user.dll")]
        static extern void mouse_event(MouseEventArgs flags, int dx, int dy, uint data, UIntPtr extraInfo);

        [Flags]
        enum MouseEventFlag : uint 
        {
            Move = 0x0001,
            LeftDown = 0x0002,
            LeftUp = 0x0004,
            RightDown = 0x0008,
            RightUp = 0x0010,
            MiddleDown = 0x0020,
            MiddleUp = 0x0040,
            XDown = 0x0080,
            XUp = 0x0100,
            Wheel = 0x0800,
            VirtualDesk = 0x4000,
            Absolute = 0x8000
        }

        public static void judgeSpeed(double AccX, double AccY)
        {
            double k = 0.05;
            
            speedX = Convert.ToInt32(speedX + AccX * k);
            speedY = Convert.ToInt32(speedY + AccY * k);
        }

        public static void moveTest()
        {
                Console.WriteLine("Po:{1},{2}" , PositionX, PositionY);
                PositionX = PositionX + speedX;
                PositionY = PositionY + speedY;
                SetCursorPos(PositionX+speedX, PositionY+speedY);
        }
    }

}

