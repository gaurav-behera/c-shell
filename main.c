#include "headers.h"

char home[4096];
int main()
{
    setup();

    while (1)
    {
        prompt();

        char *input = malloc(sizeof(char) * 4096);
        fgets(input, 4096, stdin);

        checkBackgroundCompletion();

        int startPos = 0;
        int currentPos = 0;

        input = modifyInput(input);
        // printf("modified str: .%s.\n", input);
        input[strlen(input)] = '\n';
        int inputLength = strlen(input);

        if (inputLength == 1)
        {
            free(input);
            continue;
        }
        int success = 0;
        bool exit = false;
        for (currentPos = 0; currentPos < inputLength + 1; currentPos++)
        {
            if (strncmp(input + currentPos, "exit", 4) == 0)
            {
                exit = true;
                break;
            }

            if (input[currentPos] == ';' || input[currentPos] == '&' || input[currentPos] == '|' || input[currentPos] == '>' || input[currentPos] == '<' || input[currentPos] == '\n')
            {
                char command[currentPos - startPos + 1];
                for (int _p = startPos; _p < currentPos; _p++)
                {
                    command[_p - startPos] = input[_p];
                }
                command[currentPos - startPos] = '\0';

                executeCommand(command, input[currentPos]);
                startPos = currentPos + 1;
            }
        }

        saveInput(input);
        free(input);
        if (exit == true)
        {
            saveLastCommand("");
            return 0;
        }
    }
    return 0;
}
