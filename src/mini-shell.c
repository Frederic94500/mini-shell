#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>

int main(int argc, char const *argv[])
{
    char c[255];
    int byte_read;
    int child_pid;
    int output;

    while (1)
    {
        write(STDOUT_FILENO, "$ ", sizeof("$ "));
        byte_read = read(STDIN_FILENO, c, sizeof(c));
        c[byte_read - 1] = '\0';
        if(byte_read != 1){
            if(strcmp(c, "exit") == 0){
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
