#include <stdio.h>
#include <unistd.h>
#include <string.h>

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

        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless command included &
         */
    }

    return 0;
}