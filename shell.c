#include "shell.h"

/**
 * shell_loop - The main loop of the shell.
 * This function continuously prompts the user for input, reads the input,
 * splits it into commands and arguments, executes the command, and repeats
 * until the user exits the shell.
 */
void shell_loop(void)
{
	char *line;
	char **args;
	int status;

	do {
		printf("$ "); /* Display the prompt */
		line = read_line(); /* Read the user input */
		args = split_line(line); /* Split the input into command and arguments */
		status = execute_command(args); /* Execute the command */

		free(line);
		free(args);
	} while (status); /* Repeat until status is 0 (exit command) */
}

/**
 * read_line - Reads a line of input from the user.
 *
 * This function uses getline to read input from the user. It handles
 * the end-of-file condition (Ctrl+D) by exiting the shell. If another
 * error occurs while reading the input, an error message is displayed
 * and the program exits.
 *
 * Return: The line entered by the user.
 */

char *read_line(void)
{
	char *line = NULL;
	size_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1)  /* Read line */
	{
		if (feof(stdin))  /* End of file processing (Ctrl+D) */
		{
			exit(EXIT_SUCCESS);  /* exit from shell */
		}
		else
		{
			perror("read_line error");
			exit(EXIT_FAILURE);
		}
	}

	return (line);
}

/**
 * split_line - Splits a line into tokens (words).
 * @line: The line to be split.
 *
 * Return: An array of tokens (command and arguments).
 */
char **split_line(char *line)
{
	int bufsize = 64, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens)
	{
		fprintf(stderr, "Allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, " \t\r\n\a"); /* Split the line into tokens */
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= bufsize)
		{
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens)
			{
				fprintf(stderr, "Allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}
	tokens[position] = NULL;
	return (tokens);
}

/**
 * shell_cd - Implements the 'cd' command.
 * @args: The arguments provided by the user (args[1] should be the directory).
 *
 * This function changes the current working directory of the shell.
 * If the user does not provide an argument, an error message is displayed.
 * If the directory change fails, an error message is displayed using perror.
 *
 * Return: 1 to continue execution of the shell.
 */

int shell_cd(char **args)
{
	if (args[1] == NULL)
	{
		fprintf(stderr, "hsh: expected argument to \"cd\"\n");
	}
	else
	{
		if (chdir(args[1]) != 0)  /* Attempting to change directory */
		{
			perror("hsh");
		}
	}
	return (1);
}


/**
 * execute_command - Executes the command entered by the user.
 * @args: The command and arguments array.
 *
 * This function handles both built-in commands (cd, exit) and external
 * commands. If an external command is not found, an error message is
 * displayed. It also handles errors related to forking and waiting for
 * the child process.
 *
 * Return: 1 to continue execution of the shell, 0 to exit.
 */

int execute_command(char **args)
{
	pid_t pid;
	int status;

	if (args[0] == NULL)
	{
		return (1);
	}

	if (strcmp(args[0], "cd") == 0)
	{
		return (shell_cd(args));
	}

	if (strcmp(args[0], "exit") == 0)
	{
		return (0);
	}

	pid = fork();
	if (pid == 0) /* child process */
	{
		if (execvp(args[0], args) == -1)
		{
			/* error*/
			fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
			exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0)
	{
		perror("Fork error");
	}
	else
	{
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return (1);
}
