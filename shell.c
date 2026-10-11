#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void)
{
    char *args[MAX_LINE / 2 + 1]; /* command line arguments */
    int should_run = 1;           /* flag to determine when to exit program */

    char input[MAX_LINE + 1];

    while (should_run)
    {
        printf("osh>");
        fflush(stdout);

        int should_wait = 0; /* A variable to flag if our parent should wait for child or not*/

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        int i = 0;
        char *token = strtok(input, " \t\n");

        while (token != NULL && i < MAX_LINE / 2)
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

        if (args[0] == NULL)
        {
            continue;
        }

        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            break;
        }

        if (i > 0 && strcmp(args[i - 1], "&") == 0)
        {
            should_wait = 1;
            args[i - 1] = NULL;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            return 1;
        }

        else if (pid == 0)
        {
            execvp(args[0], args);
            perror("execvp failed");
            _exit(1);
        }

        else
        {   
            if (should_wait == 0){
                wait(NULL);
            }   
        }
    }

    return 0;
}