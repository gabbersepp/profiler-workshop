namespace TestApp
{
    public class Program
    {
        public static void Main()
        {
            Console.ReadKey();
            Test(1);
            Console.ReadKey();
        }

        private static Random random = new ();

        public static bool Test(int i)
        {
            Console.WriteLine($"[from app] i = {i}");
            if (i > 10 && random.Next(1, 5) < 3)
            {
                return false;
            }

            return Test(i + 1) & Test(i + 2);
        }
    }
}
