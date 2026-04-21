#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define Input_Size 1024
#define Token_Size 64
#define Token_Delimiters " \t\r\n\a"

char *get_input();
char **split_input(char *entered_input);
int launch_process(char **args);
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int builtins();
int execute(char **args);

 char *builtin_str[] =
 {
    "cd", "help", "exit"
 };

 int (*builtin_func[]) (char **) =
 {
    &shell_cd,
    &shell_help,
    &shell_exit,
 };


int main(int argc, char *argv[])
{
    char *input;
    char **split;
    int status;

    do
    {
       printf("My Shell > ");

        input = get_input();
        split = split_input(input);
        status = execute(split);

        free(input);
        free(split);

    } while (status);

    return EXIT_SUCCESS;


}


char *get_input()
{
    int bufsize = Input_Size;
    int position = 0;

    char *buffer;
    buffer = (char*)malloc(sizeof(char)*bufsize);
    if(buffer == NULL) exit(EXIT_FAILURE);

    int c;

    while(1)
    {
        c = getchar();

        if(c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }

        position++;

        if(position >= bufsize)
        {
            bufsize+= bufsize;

            buffer = realloc(buffer, bufsize * sizeof(char));
            if(buffer == NULL) exit(EXIT_FAILURE);
        }
    }

    return buffer;
}

char **split_input(char *entered_input)
{
    int bufsize = Input_Size, position = 0;

    char **tokens = malloc(bufsize * sizeof(char*));
    if(tokens == NULL) exit(EXIT_FAILURE);
    char *token;

    token = strtok(entered_input, Token_Delimiters);

    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if(position >= bufsize)
        {
            bufsize+= bufsize;

            tokens = realloc(tokens, bufsize * sizeof(char*));
            if(tokens == NULL) exit(EXIT_FAILURE);
        }

        token = strtok(NULL, Token_Delimiters);
    }

    tokens[position] = NULL;

    return tokens;
}

int launch_process(char **args)
{
    pid_t pid = fork(), wpid;
    int status;

    if(pid < 0)
    {
        perror("lsh");
    }
    else if(pid == 0)
    {if(execvp(args[0], args)== -1)
        {
            perror("lsh");
        }

    }
    else
    {
       do
       {
            wpid = waitpid(pid, &status, WUNTRACED);
       } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }


    return 1;

}

int builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int shell_cd(char **args)
{
    if (args[1] == NULL) {
    fprintf(stderr, "Expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int shell_help(char **args)
{
    printf("Built in commands:\n");
    for (int i = 0; i < builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }

    return 1;
}

int shell_exit(char **args)
{
   return 0;
}

int execute(char **args)
{
    if(args[0] == NULL)
        return 1;

    for(int i = 0; i <builtins(); i++)
    {
        if(strcmp(args[0], builtin_str[i]) == 0)
        {
            return(*builtin_func[i])(args);
        }
    }

    return launch_process(args);
}