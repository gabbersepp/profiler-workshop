using System;
using System.Runtime.InteropServices;

namespace DotnetApp;

public class Program
{
    [DllImport("DllTest.dll", CallingConvention = CallingConvention.StdCall, EntryPoint = "Add")]
    static extern int AddStdCall(int a, int b, int c, int d, int e, int f);

    [DllImport("DllTest.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "Add")]
    static extern int AddCdecl(int a, int b, int c, int d, int e, int f);

    [DllImport("DllTest.dll", CallingConvention = CallingConvention.Cdecl)]
    static extern int CallCppUseAsmCallCppWithin(int returnValue, string str, out int strLength);

    public static void Main()
    {
        Console.ReadLine();
        
        AddCdecl(1, 2, 3, 4, 5, 6);
        
        AddStdCall(1, 2, 3, 4, 5, 6);

        Console.WriteLine("finished");

        var t = new Test();
        t.Add(1, 2);

        Console.WriteLine("x86 Plattform? j/n");
        var yesNo = Console.ReadKey();
        if (yesNo.KeyChar == 'j')
        {
            var retVal = CallCppUseAsmCallCppWithin(0xffff, "Hallo Welt", out var len);
            Console.WriteLine($"retVal = {retVal}, Länge str = {len}");
        }

        Console.ReadLine();
    }

    public class Test
    {
        private ulong oneByteNumber = 0x1;
        private ulong twoByteNumber = 0x0102;
        private ulong threeByteNumber = 0x010203;
        private ulong fourByteNumber = 0x01020304;
        private ulong fiveByteNumber = 0x0102030405;
        private Test thisHolder;

        public Test()
        {
            thisHolder = this;
        }

        public int Add(int a, int b)
        {
            return a + b;
        }


    }
}

