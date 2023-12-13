// Test를 위한 data.txt 이름의 파일 생성

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_NUMBERS 100000

int isDuplicate(int *numbers, int count, int newNumber)
{
    for (int i = 0; i < count; i++)
    {
        if (numbers[i] == newNumber)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    srand(time(NULL));

    FILE *file = fopen("data.txt", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    int generatedNumbers[NUM_NUMBERS];
    int count = 0;

    while (count < NUM_NUMBERS)
    {
        int randomNum = rand() % (2 * NUM_NUMBERS + 1) - NUM_NUMBERS;

        if (!isDuplicate(generatedNumbers, count, randomNum))
        {
            fprintf(file, "%d\n", randomNum);
            generatedNumbers[count] = randomNum;
            count++;
        }
    }

    fclose(file);
    printf("Random numbers generated and saved to data.txt\n");

    return 0;
}
