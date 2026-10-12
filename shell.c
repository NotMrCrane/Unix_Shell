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

        int should_wait = 0; /* A variable to flag if our parent should wait for child or not. 0: parent waits, 1: parent does not wait*/

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        /* Checks if the command exceeds the maximum length of 80 characters */
        if (strlen(input) == MAX_LINE && strchr(input, '\n') == NULL)
        {
            int ch = getchar();

            if (ch != '\n' && ch != EOF)
            {
                /* Discards the remaining characters until newline or EOF */
                while ((ch = getchar()) != '\n' && ch != EOF)
                {
                    // getchar() discards each character
                }

                printf("Error: command exceeds 80 characters.\n");
                continue;
            }
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "!!") == 0) /* Retrieve and execute the most recent command if the user enters !! */
        {
            if (history_count == 0)
            {
                printf("No commands in history.\n");
                continue;
            }

            strcpy(input, history[history_count - 1]);
            printf("%s\n", input);
        }

        /* Save the original command before strtok() modifies the input */
        char original[MAX_LINE + 1];
        strcpy(original, input);

        /* Split the input into arguments to be passed to execvp() */
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

        if (i > 0 && strcmp(args[i - 1], "&") == 0) /* Check for & at the end of the command and enable background execution */
        {
            should_wait = 1;
            args[i - 1] = NULL;
        }

        if (args[0] == NULL)
        {
            continue;
        }

        /* Displays the last 5 commands in reverse order without storing "history" itself */
        if (strcmp(args[0], "history") == 0)
        {
            for (int j = history_count - 1; j >= 0; j--)
            {
                printf("%d %s\n", command_number - (history_count - 1 - j), history[j]);
            }
            continue;
        }

        /* Stores the last 5 commands, removing the oldest when history is full */
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
                waitpid(pid, NULL, 0); /*the parent waits specifically for the child created*/
            }
        }
    }

    return 0;
}