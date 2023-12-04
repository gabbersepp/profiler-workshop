using System.Runtime.InteropServices;

namespace TestApp
{
    public class Program
    {
        [DllImport("kernel32.dll")]
        static extern int GetCurrentThreadId();
        public static void Main()
        {
            Console.WriteLine($"Hello, World from native thread {GetCurrentThreadId()}!");

            try
            {
                throw new Exception();
            }
            catch (Exception e)
            {

            }

            Console.ReadKey();
        }
    }
}
