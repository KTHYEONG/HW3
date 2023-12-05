#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#define TMP_FILE "temp_file.txt"

int main()
{
    int pid;
    int fd;
    int tmp_fd;
    char filename[256];

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    printf("Enter file names (type 'finish' to sort, 'exit' to end):\n");

    tmp_fd = open(TMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (tmp_fd == -1)
    {
        perror("open");
        exit(1);
    }

    while (1)
    {
        printf("Enter file name or finish: ");
        scanf("%255s", filename);

        if (strcmp(filename, "finish") == 0)
        {
            close(tmp_fd);

            if ((pid = fork()) == -1)
            {
                perror("fork");
                exit(1);
            }

            if (pid == 0)
            {
                fd = open(TMP_FILE, O_RDONLY);
                if (fd == -1)
                {
                    perror("open");
                    exit(1);
                }

                if (dup2(fd, 0) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(1);
                }
                close(fd);

                execlp("sort", "sort", NULL);
                perror("execlp");
                exit(1);
            }

            if (pid != 0)
            {
                wait(NULL);

                remove(TMP_FILE);

                tmp_fd = open(TMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (tmp_fd == -1)
                {
                    perror("open");
                    exit(1);
                }

                printf("Enter file names (type 'finish' to sort, 'exit' to end):\n");
                continue;
            }
        }
        else if (strcmp(filename, "exit") == 0)
            break;

        fd = open(filename, O_RDONLY);
        if (fd == -1)
        {
            perror("open");
            close(tmp_fd);
            exit(1);
        }

        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
        {
            write(tmp_fd, buffer, bytesRead);
        }

        close(fd);
    }

    close(tmp_fd);

    return 0;
}