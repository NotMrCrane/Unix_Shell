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
    char history[5][MAX_LINE + 1]; /*2 dimensional array for remembering previous commands*/

    int history_count = 0;  /*Number of commands currently stored (max 5)*/
    int command_number = 0; /*Total number of commands added to history since the shell started*/

    while (should_run)
    {
        printf("osh>");
        fflush(stdout);

        int should_wait = 0; /* A variable to flag if our parent should wait for child or not*/

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        input[strcspn(input, "\n")] = '\0';
        char original[MAX_LINE + 1]; /*contains a separate copy of the command*/
        strcpy(original, input);

        int i = 0;
        char *token = strtok(input, " \t\n");

        while (token != NULL && i < MAX_LINE / 2)
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

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

        if (args[0] == NULL)
        {
            continue;
        }

        if (history_count < 5)
        {
            strcpy(history[history_count], original);
            history_count += 1;
        }
        else
        {
            for (int j = 0; j < 4; j++)
            {
                strcpy(history[j], history[j + 1]);
            }
            strcpy(history[4], original);
        }
        command_number += 1;

        printf("Current history:\n");

        for (int j = 0; j < history_count; j++)
        {
            printf("%s\n", history[j]);
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
            if (should_wait == 0)
            {
                wait(NULL);
            }
        }
    }

    return 0;
}