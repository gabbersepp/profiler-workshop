namespace TestApp
{
    public class Program
    {
        public static void Main()
        {
            Console.ReadKey();
            Console.WriteLine("Hello, World!");
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
