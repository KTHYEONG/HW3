#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 10

void processArray(int start, int end, int *array, int processNum)
{
    printf("Process %d is calculating elements from %d to %d\n", processNum, start, end);
    for (int i = start; i < end; i++)
    {
        array[i] *= 2; // 배열의 각 요소를 2배로 변경 (임의의 처리 예시)
    }
}

int main()
{
    int data[ARRAY_SIZE];
    int numChildren;

    // 배열 초기화
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        data[i] = i;
    }

    // 사용자로부터 자식 프로세스의 수 입력 받기
    printf("Enter the number of child processes: ");
    scanf("%d", &numChildren);

    // 병렬 처리를 사용하여 배열을 처리하는 부분
    clock_t parallelStart = clock();

    pid_t pid;

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
            int start = i * (ARRAY_SIZE / numChildren);
            int end = (i + 1) * (ARRAY_SIZE / numChildren);

            processArray(start, end, data, i + 1);

            exit(EXIT_SUCCESS);
        }
    }

    // 부모 프로세스는 모든 자식 프로세스의 종료를 기다림
    for (int i = 0; i < numChildren; i++)
    {
        wait(NULL);
    }

    clock_t parallelEnd = clock();
    double parallelTime = (double)(parallelEnd - parallelStart) / CLOCKS_PER_SEC;

    // 순차 처리를 사용하여 배열을 처리하는 부분
    clock_t sequentialStart = clock();
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        data[i] *= 2;
    }

    // 결과 출력
    printf("Parallel processing time: %.6f seconds\n", parallelTime);
    printf("Sequential processing time: %.6f seconds\n", sequentialTime);

    return 0;
}