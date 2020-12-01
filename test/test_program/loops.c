#include <stdio.h>

int main()
{
    int sum = 0;
    for (int i = 0; i < 100; i++)
    {
        sum += i;
    }

    int counter = 10;
    while (1)
    {
        int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        for (int i = 0; i < 10; i++)
        {
            sum += array[i];
        }

        if (counter <= 0)
        {
            break;
        }
        counter--;
    }

    return sum;
}
