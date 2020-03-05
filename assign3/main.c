#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "shell.h"

/*
Timothy Mather shell
201527769

Includes:
    Redirection
    Suspension
    Cancellation
    History
*/

int children = 0;
int i = 0;
int fileredir = 0;
int err_redir = 0;
char *filename;

// Parses arguments from buffer
char **get_args(char *buffer)
{
    char delims[] = {' ', '\t', '\n'};
    char *token = strtok(buffer, delims);
    char **args = (char *)calloc(20, sizeof(char *));
    args[0] = token;
    int i = 1;
    int file_next = 0;
    while (token != NULL)
    {
        token = strtok(NULL, delims);
        if (token == NULL)
        {
            break;
        }
        if (file_next == 1)
        {
            file_next = 0;
            strcpy(filename, token);
        }
        else if (strcmp(token, ">") == 0)
        {
            fileredir = 1;
            file_next = 1;
        }
        else if (strcmp(token, "2>") == 0)
        {
            err_redir = 1;
            file_next = 1;
        }
        else
        {
            args[i] = token;
        }
        i++;
    }
    return args;
}

int main(int argc, char *argv[])
{
    // set up pipes
    filename = (char *)malloc(100 * sizeof(char));

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
            children = 0;
            // execute child
            int fd;
            // redirect outputs to file if requested
            if (fileredir == 1)
            {
                fd = open(filename,  O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                dup2(fd, 1);
            }
            if (err_redir == 1)
            {
                fd = open(filename,  O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                dup2(fd, 2);
            }
            close(fd);
            // execute arguments
            execvp(args[0], args);
        }
        else
        {
            // handle printing history
            if (strcmp(history + i, "history") == 0)
            {
                printf("Session History: \n");
                for (int j = 0; j < i + 1; j++)
                {
                    printf("%s\n", (history + j));
                }
                continue;
            }
            children += 1;
            int stat;
            char buffer[4096];
            pid_t w = waitpid(pid, &stat, WUNTRACED);
            children -= 1;
            fileredir = 0;
            err_redir = 0;
            if (WIFEXITED(stat))
            {
                printf("Exited with status: %d\n", WEXITSTATUS(stat));
            }
            else if (WTERMSIG(stat) != 127)
            {
                printf("\nTerminated by %s\n", strsignal(WTERMSIG(stat)));
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


// signal handling
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
