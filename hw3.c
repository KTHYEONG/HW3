#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <curses.h>

int countElements(FILE *file)
{
    int count = 0;
    int value;

    while (fscanf(file, "%d", &value) == 1)
    {
        count++;
    }

    return count;
}

int sequentialSearch(int *data, int start, int end, int goal)
{
    for (int i = start; i < end; i++)
    {
        if (data[i] == goal)
        {
            return i; // Return the index if found
        }
    }
    return -1;
}

void signal_ign()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

int main(int argc, char *argv[])
{
    signal_ign();

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("Error opening input file");
        return 1;
    }

    int numElements = countElements(file);
    fseek(file, 0, SEEK_SET);
    int *data = malloc(numElements * sizeof(int));

    for (int i = 0; i < numElements; i++)
    {
        fscanf(file, "%d", &data[i]);
    }
    fclose(file);

    char check = 'y';
    while (1)
    {
        int numChildren;
        printf("Enter the number of child processes: ");
        scanf("%d", &numChildren);

        int goal;
        printf("Enter the number you want to find: ");
        scanf("%d", &goal);

        int elementsPerChild = numElements / numChildren;

        pid_t pid;
        for (int i = 0; i < numChildren; i++)
        {
            pid = fork();

            if (pid == -1)
            {
                perror("Error in fork");
                exit(1);
            }

            // Child process
            if (pid == 0)
            {
                int start = i * elementsPerChild;
                int end = (i == numChildren - 1) ? numElements : (i + 1) * elementsPerChild;

                // Measure time taken for the search
                clock_t startTime = clock();
                int result = sequentialSearch(data, start, end, goal);
                clock_t endTime = clock();

                if (result != -1)
                {
                    printf("[Child] Process %d found %d at index %d (Range: %d to %d)\n", i + 1, goal, result, start, end - 1);
                }
                else
                {
                    printf("[Child] Process %d did not find %d in the range %d to %d\n", i + 1, goal, start, end - 1);
                }

                sleep(1);

                double elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
                printf("[Child] Process %d took <%.6f> seconds!\n", i + 1, elapsedTime);

                exit(0);
            }
        }

        // Parent process
        if (pid > 0)
        {
            for (int i = 0; i < numChildren; i++)
            {
                wait(NULL);
            }

            clock_t startTime = clock();
            int result = sequentialSearch(data, 0, numElements - 1, goal);
            clock_t endTime = clock();
            double elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
            printf("[Parent] Process took <%.6f> seconds!\n\n", elapsedTime);

            // Find maximum, minimum, average values
            int max = data[0];
            int min = data[0];
            int sum = 0;

            for (int i = 0; i < numElements; i++)
            {
                sum += data[i];
                if (data[i] > max)
                    max = data[i];
                if (data[i] < min)
                    min = data[i];
            }
            double average = (double)sum / numElements;

            sleep(2);

            initscr();

            standout();
            move(10, 20);
            printw("Minimum value: %d", min);
            move(11, 20);
            printw("Maximum value: %d", max);
            move(12, 20);
            printw("Average value: %.2f", average);
            standend();

            move(15, 20);
            printw("Do you have any more data to look back on? (y/n): ");
            if (getch() == 'n')
                check = 'n';

            sleep(1);
            clear();
            endwin();
        }

        if (check == 'n')
            break;

        printf("\n");
    }

    free(data);

    return 0;
}
