import java.util.Arrays;

public class Sort {

    public static int[] sort(int[] numbers) {
        for (int i = 0; i < numbers.length - 1; i++)
        {
            for (int j = 0; j < numbers.length - i - 1; j++)
            {      
                if (numbers[j] > numbers[j + 1])
                    {
                        int temp = numbers[j];
                        numbers[j] = numbers[j + 1];
                        numbers[j + 1] = temp;
                    }
            }
        }
        
        return numbers;
    }
    public static void main(String[] args) {
        int[] i = { 5, 3, 2, 3, 1, 3, 2, 3 };
        System.out.println(Arrays.toString(sort(i)));
    }
}
