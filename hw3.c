#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <limits.h>
#include <curses.h>

// Function to initialize a matrix with random values
void initializeMatrix(int matrixSize, int matrix[matrixSize][matrixSize])
{
    srand(time(NULL));

    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            matrix[i][j] = rand() % 100; // Assign random values for simplicity
        }
    }
}

// Function to perform matrix multiplication for a specific range
void processMatrixMultiplication(int start, int end, int matrixSize, int result[matrixSize][matrixSize], int processNum, FILE *outputFile)
{
    clock_t startTime, endTime;

    if (processNum != 0)
        printf("Child Process %d is multiplying matrices from row %d to %d\n", processNum, start, end - 1);
    else
        printf("Parent Process is multiplying matrices from row %d to %d\n", start, end - 1);

    startTime = clock(); // Record the start time

    // Assume two matrices A and B
    int matrixA[matrixSize][matrixSize];
    int matrixB[matrixSize][matrixSize];

    // Initialize matrices (for simplicity, you can modify this part if you have specific matrices)
    initializeMatrix(matrixSize, matrixA);
    initializeMatrix(matrixSize, matrixB);

    // Perform matrix multiplication and store the result in the specified range
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            result[i][j] = 0;
            for (int k = 0; k < matrixSize; k++)
            {
                result[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    endTime = clock(); // Record the end time
    double elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

    sleep(1);

    if (processNum != 0)
    {
        printf("Child %d took %.6f seconds!\n", processNum, elapsedTime);

        // Append the results to the common output file
        for (int i = start; i < end; i++)
        {
            for (int j = 0; j < matrixSize; j++)
            {
                fprintf(outputFile, "%d ", result[i][j]);
            }
            fprintf(outputFile, "\n");
        }
    }
    else
    {
        printf("Parent took %.6f seconds!\n", elapsedTime);
    }
}

int main()
{
    int numChildren;
    pid_t pid;

    char restartChoice; // Variable to store user's choice for restarting the program

    do
    {
        // 사용자로부터 matrix 사이즈 및 자식 프로세스의 수 입력 받기
        int matrixSize;
        printf("Enter the matrix size: ");
        scanf("%d", &matrixSize);

        printf("Enter the number of child processes: ");
        scanf("%d", &numChildren);

        FILE *outputFile = fopen("output.txt", "a");
        if (outputFile == NULL)
        {
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }

        int resultMatrix[matrixSize][matrixSize];

        for (int i = 0; i < numChildren; i++)
        {
            pid = fork();

            if (pid == -1)
            {
                perror("Error in fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0)
            { // 자식 프로세스
                int start = i * (matrixSize / numChildren);
                int end = (i + 1) * (matrixSize / numChildren);

                if (i == numChildren - 1) // If it's the last child process
                {
                    end = matrixSize;
                }

                processMatrixMultiplication(start, end, matrixSize, resultMatrix, i + 1, outputFile);

                fclose(outputFile); // Close the file in the child process
                exit(EXIT_SUCCESS);
            }
        }

        // 부모 프로세스는 나머지 부분의 행렬 곱셈을 수행
        if (pid > 0)
        {
            // Wait for all child processes to finish
            for (int i = 0; i < numChildren; i++)
            {
                wait(NULL);
            }

            processMatrixMultiplication(0, matrixSize, matrixSize, resultMatrix, 0, outputFile);

            // Find and print the minimum, maximum, and average values in resultMatrix
            int minVal = INT_MAX;
            int maxVal = INT_MIN;
            int sum = 0;

            for (int i = 0; i < matrixSize; i++)
            {
                for (int j = 0; j < matrixSize; j++)
                {
                    if (resultMatrix[i][j] < minVal)
                    {
                        minVal = resultMatrix[i][j];
                    }
                    if (resultMatrix[i][j] > maxVal)
                    {
                        maxVal = resultMatrix[i][j];
                    }
                    sum += resultMatrix[i][j];
                }
            }

            double average = (double)sum / (matrixSize * matrixSize);

            endwin();

            initscr();

            move(20, 30);
            printw("Minimum value: %d", minVal);
            refresh();
            sleep(1);

            move(21, 30);
            printw("Maximum value: %d", maxVal);
            refresh();
            sleep(1);

            move(22, 30);
            printw("Average value: %.2f", average);
            refresh();
            sleep(1);

            standout();
            move(25, 30);
            printw("Press 'y' to restart, 'n' to exit ...");
            refresh();
            standend();

            restartChoice = getch(); // Get user's choice for restarting or exiting

            erase();
            endwin();

            fclose(outputFile); // Close the file in the parent process

            // Delete the file after displaying its contents
            if (remove("output.txt") == -1)
            {
                perror("Error deleting the file");
                exit(EXIT_FAILURE);
            }
        }

        printf("\n");

    } while (restartChoice == 'y');

    return 0;
}
