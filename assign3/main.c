#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "shell.h"

int children = 0;

char **get_args(char *buffer)
{
    char delims[] = {' ', '\t', '\n'};
    char *token = strtok(buffer, delims);
    char **args = (char *)calloc(20, sizeof(char *));
    args[0] = token;
    int i = 1;
    while (token != NULL)
    {
        token = strtok(NULL, delims);
        args[i] = token;
        i++;
    }
    return args;
}

int main(int argc, char *argv[])
{
    int i = 0;
    int buff_size = 64;
    int pipe_fds[2];
    pipe(pipe_fds);
    char **args;
    char *buff = (char *)malloc(buff_size * sizeof(char));
    char *history[100];
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);

    while (1)
    {
        size_t c = getline(&buff, &buff_size, stdin);
        args = get_args(buff);
        pid_t pid = fork();
        int status;
        if (pid == 0)
        {
            children = 0;
            dup2(pipe_fds[1], stdout);
            close(pipe_fds[1]);
            close(pipe_fds[0]);
            int stat1 = execvp(args[0], args);
        }
        else
        {
            children += 1;
            close(pipe_fds[1]);
            char buffer[4096];
            int stat;
            int child_stat;
            pid_t w = waitpid(0, &stat, 0);
            children -= 1;
            read(pipe_fds[0], buffer, sizeof(buff));
            close(pipe_fds[0]);
            printf("Exited with status: %d\n", WEXITSTATUS(stat));
        }
    }
    return 0;
}

void signal_handler(int sig)
{
    pid_t sig_pid = getpid();
    if (sig_pid < 10)
    {
        kill(sig_pid, sig);
    }
    else
    {
        if (children == 0)
        {
            printf("Exiting shell with status: %d", sig);
            exit(sig);
        }
    }
}
