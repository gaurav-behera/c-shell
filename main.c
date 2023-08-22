#include "headers.h"

char home[4096];
int main()
{
    setup();

    while (1)
    {
        prompt();

        char *input= malloc(sizeof(char)*4096);
        fgets(input, 4096, stdin);

        checkBackgroundCompletion();

        int startPos = 0;
        int currentPos = 0;
        int inputLength = strlen(input);
        // input[inputLength] = '\0';

        input = modifyInput(input);
        printf("Modified Input: .%s.\n", input);
        input[strlen(input)] = '\n';

        if (inputLength == 0)
        {
            continue;
        }
        
        int success = 0;
        for (currentPos = 0; currentPos < inputLength + 1; currentPos++)
        {
            if (input[currentPos] == ';' || input[currentPos] == '&' || input[currentPos] == '\n')
            {
                char command[currentPos - startPos + 1];
                for (int _p = startPos; _p < currentPos; _p++)
                {
                    command[_p - startPos] = input[_p];
                }
                command[currentPos - startPos] = '\0';
                switch (input[currentPos])
                {
                case ';':
                    executeInForeground(command);
                    break;
                case '&':
                    executeInBackground(command);
                    break;
                default:
                    executeInForeground(command);
                    break;
                }

                // success = executeCommand(command, input[currentPos]);
                // if (success == 0)
                // {
                //     break;
                // }
                startPos = currentPos + 1;
            }
        }
        saveInput(input);
    }
}
