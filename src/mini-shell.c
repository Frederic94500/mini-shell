#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<ctype.h>
#include<string.h>

int parse_line(char *s, char **argv[])
{
    int argi = 0;
    char *arg;
    int space = 0;

    for (size_t i = 0; i < strlen(s); i++)
    {
        if(isspace(s[i]) != 0){
            arg = (char*)(malloc(sizeof(char) * (i - space + 1)));
            for (size_t j = 0; j < i - space; j++)
            {
                arg[j] = s[j + space];
            }
            arg[i - space] = '\0';
            (*argv)[argi] = arg;
            argi++;
            space = i + 1;
        }
    }

    (*argv)[argi] = NULL;
    return 0;
}

int main(int argc, char const *argv[])
{
    char buffer[1024];
    int byte_read;
    int child_pid;
    char **argvcmd = malloc(sizeof(char*) * 18);

    while (1)
    {
        write(STDOUT_FILENO, "$ ", sizeof("$ "));
        byte_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        buffer[byte_read - 1] = ' ';
        buffer[byte_read] = '\0';
        parse_line(buffer, &argvcmd);
        if(byte_read != 1){
            if(strcmp(buffer, "exit ") == 0){
                for (size_t i = 0; i < sizeof(argvcmd); i++)
                {
                    free(argvcmd[i]);
                }
                
                free(argvcmd);
                exit(0);
            }
            if((child_pid = fork()) == 0){ //child
                if(execvp(argvcmd[0], argvcmd) < 0){
                    perror("Erreur exécution: ");
                    exit(-1);
                }
            } else { //parent
                wait(&child_pid);
            }
        }
    }
    
    return 0;
}
