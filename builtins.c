#include "main.h"

int shell_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("shell");
        }
    }
    return 1;
}

int shell_exit(char **args)
{
    return 0;
}

int shell_env(char **args)
{
    extern char **environ;
    for (char **env = environ; *env != 0; env++)
    {
        printf("%s\n", *env);
    }
    return 1;
}
