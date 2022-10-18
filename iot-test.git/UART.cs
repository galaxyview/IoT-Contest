using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Threading;
namespace Iotfitness
{
    public class SensorData
    {

        public int Num = 0;
        public int pitch = 0;
        public int roll = 0;
        public double AcceX = 0;
        public double AcceY = 0;
        public double AcceZ = 0;

        public SensorData()
        { 
            
        }

        public SensorData(int p,int r,double X,double Y,double Z)
        {
            pitch = p;
            roll = r;
            AcceX = X;
            AcceY = Y;
            AcceZ = Z;
        }


    }

    //存储最近100条数据
    class dataBuffer
    {
        int BUFFER_SIZE;

        SensorData[] Buffer;

        int index;

        //互斥信号量  对缓冲区的互斥访问
        static Mutex mutex = new Mutex();

        //同步信号量  标记空、满缓冲单元数量(初始、最大)
        static Semaphore empty;
        static Semaphore full;

        //先试试非循环式的
        public dataBuffer(int size)
        {
            //初始化缓冲区大小
            BUFFER_SIZE = size;

            //初始化信号量
            empty = new Semaphore(BUFFER_SIZE, BUFFER_SIZE);
            full = new Semaphore(0, BUFFER_SIZE);


            Buffer = new SensorData[BUFFER_SIZE];
            index = 0;
        }

        //向缓冲区插入一个数据
        public void ProduceData(string s)
        {
            string[] data = s.Split(' ');
            if (data.Length != 9)
                return;
            SensorData item = new SensorData();
            item.Num = Convert.ToInt32(data[0]);
            item.roll = Convert.ToInt32(data[1]);
            item.pitch = Convert.ToInt32(data[2]);
            item.AcceX = Convert.ToDouble(data[3]);
            item.AcceY = Convert.ToDouble(data[4]);
            item.AcceZ = Convert.ToDouble(data[5]);

            
            while (!empty.WaitOne(5))
            {
                Console.WriteLine("缓冲区已满");
            }
            //占据缓冲池
            mutex.WaitOne();

            Buffer[index] = item;
            index++;

            mutex.ReleaseMutex();
            full.Release();
        }

        //从缓冲区取数据
        public SensorData ConsumerData()
        {
            SensorData result = new SensorData();

            while (!full.WaitOne(10))
            {
                
            }

            index--;
            result = Buffer[index];
            

            mutex.ReleaseMutex();
            empty.Release();


            return result;
        }
    }



    //串口数据读取（多线程）
    public class Serial
    {
        //读线程
        Thread ReadThread;
        Thread ProcessThread;

        //可用串口列表
        string[] ports;

        //串口对象
        SerialPort COM;

        //控制读与处理过程
        bool continueRead;
        bool continueProcess;

        //数据缓冲区
        int BUFFER_SIZE;
        SensorData[] Buffer;
        int index;

        //互斥信号量  对缓冲区的互斥访问
        static Mutex mutex = new Mutex();

        //同步信号量  标记空、满缓冲单元数量(初始、最大)
        static Semaphore empty;
        static Semaphore full;

        public Serial(int buffer_size)
        {
            //初始化缓冲区及相关信号量
            BUFFER_SIZE = buffer_size;
            Buffer = new SensorData[BUFFER_SIZE];

            index = 0;
            empty = new Semaphore(BUFFER_SIZE, BUFFER_SIZE);
            full = new Semaphore(0, BUFFER_SIZE);

            //初始化串口
            COM = new SerialPort();
            ports = SerialPort.GetPortNames();
        }


        public void showPorts()
        {
            foreach (string s in ports)
            {
                Console.WriteLine(s + " ");
            }
        }

        public void openPort(string Name = "COM3", int Baud = 115200, int Bits = 8)
        {
            bool flag = false;
            foreach (string s in ports)
            {
                if (s == "COM3")
                    flag = true;
            }

            if ((COM == null || !COM.IsOpen) && flag)
            {
                COM.PortName = Name;
                COM.Open();
                COM.BaudRate = 115200;
                COM.DataBits = 8;
                COM.StopBits = StopBits.One;
                COM.Parity = Parity.None;
                COM.ReadTimeout = 100;
                COM.WriteTimeout = -1;
                //使用DataReceived事件接收数据，在辅助线程，优先级较低
                //达到Threshold设置的字符个数或者接受到了文件结束符放入输入缓冲区时被触发
                //COM.DataReceived += new SerialDataReceivedEventHandler(receivedData);
                COM.ReceivedBytesThreshold = 100;
                COM.ReadTimeout = 100;

                //启动读线程
                ReadThread = new Thread(Read);
                continueRead = true;
                ReadThread.Start();

                //启动数据处理线程
                ProcessThread = new Thread(ProcessTest);
                continueProcess = true;
                ProcessThread.Start();
            }
            else
            {
                Console.WriteLine("打开串口失败");
            }
        }

        public void closePort()
        {
            if (COM.IsOpen)
            {
                COM.Close();
            }
        }

        public void Read()
        {
            while (continueRead)
            {
                try
                {
                    
                    string message = COM.ReadLine();
                    //用一个特殊标号表示数据
                    /*
                    if (message[0]>='0' && message[0]<='9')
                    {
                        string[] data = message.Split(' ');
                        if (data.Length != 9)
                            return;
                        SensorData item = new SensorData();
                        item.Num = Convert.ToInt32(data[0]);
                        item.roll = Convert.ToInt32(data[1]);
                        item.pitch = Convert.ToInt32(data[2]);
                        item.AcceX = Convert.ToDouble(data[3]);
                        item.AcceY = Convert.ToDouble(data[4]);
                        item.AcceZ = Convert.ToDouble(data[5]);

                        while (!empty.WaitOne(5))
                        {
                            Console.WriteLine("缓冲区已满");
                        }
                        //占据缓冲池
                        mutex.WaitOne();

                        Buffer[index] = item;
                        index++;

                        mutex.ReleaseMutex();
                        full.Release();

                    }
                    */
                    Console.WriteLine(message);
                }
                catch (TimeoutException)
                {
                   
                }
            }

        }

        public void ProcessTest()
        {
            while (continueProcess)
            {
                try
                {

                    SensorData result = new SensorData();

                    while (!full.WaitOne(10))
                    {

                    }

                    index--;
                    result = Buffer[index];
                    Mouse.judgeSpeed(result.AcceX, result.AcceY);
                    Mouse.moveTest();

                    mutex.ReleaseMutex();
                    empty.Release();
                }
                catch 
                { }
            }
        }

    }
}
