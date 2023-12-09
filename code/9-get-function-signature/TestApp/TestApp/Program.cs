namespace TestApp
{
    public class Program
    {
        public static void Main()
        {
            Console.ReadKey();
            Blub_i(1);
            Blub_arr(new []{1,2,3,4,5,6});
            Blub_str("Hello, World");
            Blub(1);
            Blub(1, true);
            new Test().Blub(1, false);
            Console.ReadKey();
        }

        public static void Blub(int i)
        {
        }
        public static int Blub(int i, bool bla)
        {
            return 0;
        }

        public static void Blub_i(int i)
        {
            Console.WriteLine(i);
        }

        public static void Blub_str(string str)
        {
            Console.WriteLine(str);
        }

        public static void Blub_arr(int[] intArray)
        {
            Console.WriteLine(intArray.Length);
        }

        public class Test
        {
            public int Blub(int i, bool bla)
            {
                return 0;
            }
        }
    }
}
