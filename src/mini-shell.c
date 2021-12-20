#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>

char* get_filename(char *s){
    int argi = 0;
    char *arg;
    int space = 0;
    char *argv[18];

    for (size_t i = 0; i < strlen(s); i++)
    {
        if(isspace(s[i]) || s[i] == '\0'){
            if(i - space == 0)
            {
                space++;
                continue;
            }
            arg = (char*)(malloc(sizeof(char) * (i - space + 1)));
            for (size_t j = 0; j < i - space; j++)
            {
                arg[j] = s[j + space];
            }
            arg[i - space] = '\0';
            argv[argi] = arg;
            argi++;
            space = i + 1;
        }
    }

    return argv[argi - 1];
}

int parse_line(char *s, char **argv[])
{
    int argi = 0;
    char *arg;
    int space = 0;

    for (size_t i = 0; i < strlen(s); i++)
    {
        if(isspace(s[i]) || s[i] == '\0'){
            if(i - space == 0)
            {
                space++;
                continue;
            }
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

        if(strpbrk(s, ">"))
        {
            break;
        }
    }

    if(argi > 17){
        return -1;
    }

    (*argv)[argi] = NULL;
    return 0;
}

/*void intHandler(int pid, int sig){
    kill(pid);
}*/

int main(int argc, char const *argv[])
{
    char buffer[1024];
    int byte_read;
    int child_pid;
    int nb_arg;
    char **argvcmd = malloc(sizeof(char*) * 18);

    while (1)
    {
        write(STDOUT_FILENO, "$ ", sizeof("$ "));
        byte_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        buffer[byte_read - 1] = ' ';
        buffer[byte_read] = '\0';

        if((nb_arg = parse_line(buffer, &argvcmd)) == -1){
            char *temp = "Trop d'arguments\n";
            write(STDOUT_FILENO, temp, strlen(temp));
        }

        if(byte_read != 1 && nb_arg == 0){
            if(strncmp(buffer, "exit", 1) == 0){ //exit shell
                for (size_t i = 0; i < sizeof(argvcmd); i++)
                {
                    free(argvcmd[i]);
                }
                
                free(argvcmd);
                exit(0);
            }

            if((child_pid = fork()) == 0){ //child
                if(strpbrk(buffer, ">"))
                {
                    int fd_out = open(get_filename(buffer), O_RDWR | O_CREAT | O_TRUNC, 0666);
                    if(fd_out < 0)
                    {
                        perror("Erreur open: ");
                    } else {
                        if(dup2(fd_out, STDOUT_FILENO) < 0)
                        {
                            perror("Erreur dup2: ");
                        }
                        close(fd_out);
                    }       
                }
                if(execvp(argvcmd[0], argvcmd) < 0){ //Problème, ne redirige pas
                    perror("Erreur execution: ");
                    exit(-1);
                }
            } else if(child_pid < 0){
                perror("Erreur fork: ");
            } else { //parent
                wait(&child_pid);
                //signal(SIGINT, send_kill(child_pid));
            }
        }
    }
    
    return 0;
}
