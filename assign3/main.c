#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "shell.h"

int children = 0;
int i = 0;

// Parses arguments from buffer
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
    // set up pipes
    int pipe_fds[2];
    pipe(pipe_fds);

    // allocate arguments and history space
    int buff_size = 64;
    char **args;
    char *buff = (char *)malloc(buff_size * sizeof(char));
    char **history = (char *)calloc(100, sizeof(char *));

    // set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);

    while (1)
    {
        printf("$ ");
        size_t c = getline(&buff, &buff_size, stdin);
        if (c == 1) // this prevents errors by executing child with no arguments
        {
            continue;
        }
        args = get_args(buff);
        strcpy(history + i, args[0]); // store argument in history
        pid_t pid = fork();
        if (pid == 0)
        {
            // execute child
            children = 0;
            dup2(pipe_fds[1], stdout);
            close(pipe_fds[1]);
            close(pipe_fds[0]);
            execvp(args[0], args);
        }
        else
        {
            // handle printing history
            if (strcmp(history+i, "history") == 0)
            {
                printf("Session History: \n");
                for (int j = 0; j < i+1; j++)
                {
                    printf("%s\n", (history + j));
                }
                continue;
            }
            children += 1;
            close(pipe_fds[1]);
            char buffer[4096];
            int stat;
            int child_stat;
            pid_t w = waitpid(pid, &stat, WUNTRACED);
            children -= 1;
            read(pipe_fds[0], buffer, sizeof(buff));
            close(pipe_fds[0]);
            if (WIFEXITED(stat))
            {
                printf("Exited with status: %d\n", WEXITSTATUS(stat));
            }
            else if (WTERMSIG(stat) != 127)
            {
                printf("Terminated by %s\n", strsignal(WTERMSIG(stat)));
            }
            else
            {
                printf("Suspended\n");
            }
        }
        i++;
    }
    free(buff);
    free(args);
    free(history);
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
            printf("Exiting shell by %s\n", strsignal(WTERMSIG(sig)));
            exit(sig);
        }
    }
}
