#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<ctype.h>

int parse_line(char *s, char **argv[])
{
    int first_letter = 0;
    int space = 0;
    int argvi = 0;
    for (size_t i = 0; i < sizeof(s); i++)
    {
        if(isspace(s[i]) != 0){
            space = i;
            *argv[argvi] = malloc(first_letter - space);
            for (size_t i = first_letter; i < space; i++)
            {
                
            }
            argvi++;
        }
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    char c[255];
    int byte_read;
    int child_pid;
    int output;
    char** argvcmd = malloc(18 * sizeof(char*));
    for (size_t i = 0; i < 18; i++)
    {
        argvcmd[i] = malloc(1024 * sizeof(char));
    }

    while (1)
    {
        write(STDOUT_FILENO, "$ ", sizeof("$ "));
        byte_read = read(STDIN_FILENO, c, sizeof(c));
        c[byte_read - 1] = '\0';
        parse_line(c, &argvcmd);
        if(byte_read != 1){
            if(strcmp(c, "exit") == 0){
                free(argvcmd);
                exit(0);
            }
            if((child_pid = fork()) == 0){
                output = execlp(c, c, NULL);
                if(output < 0){
                    perror("Erreur exécution: ");
                    exit(-1);
                }
            } else {
                wait(&child_pid);
            }
        }
    }
    
    return 0;
}
