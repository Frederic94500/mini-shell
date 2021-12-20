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

        if(s[i] == '>')
        {
            (*argv)[argi] = NULL;
            argi++;
        }
    }

    if(argi > 17)
    {
        return -1;
    }

    (*argv)[argi] = NULL;
    return argi;
}

int main(int argc, char const *argv[])
{
    char buffer[1024];
    char buffer_2[1024];
    int byte_read;
    int child_pid;
    int nb_arg;
    char **argvcmd = malloc(sizeof(char*) * 20);
    char **argvcmd_2 = malloc(sizeof(char*) * 20);

    sigset_t sig = {{SIGINT}};
    sigprocmask(SIG_BLOCK, &sig, NULL);

    while (1)
    {
        write(STDOUT_FILENO, "$ ", sizeof("$ "));
        byte_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        buffer[byte_read - 1] = ' ';
        buffer[byte_read] = '\0';

        if((nb_arg = parse_line(buffer, &argvcmd)) == -1)
        {
            char *temp = "Too much arguments\n";
            write(STDOUT_FILENO, temp, strlen(temp));
        }

        if(byte_read != 1 && nb_arg >= 0)
        {
            if(strncmp(buffer, "exit", 1) == 0){ //exit shell
                for (size_t i = 0; i < sizeof(argvcmd); i++)
                {
                    free(argvcmd[i]);
                    free(argvcmd_2[i]);
                }
                
                free(argvcmd);
                free(argvcmd_2);
                exit(0);
            }

            if((child_pid = fork()) == 0){ //Child
                sigprocmask(SIG_UNBLOCK, &sig, NULL);
                int pipefd[2];
                int sub_child_pid;

                if(strpbrk(buffer, "|"))
                {
                    argvcmd[nb_arg - 1] = NULL;
                    write(STDOUT_FILENO, "> ", 2);
                    int byte_read_2 = read(STDIN_FILENO, buffer_2, sizeof(buffer_2));
                    buffer_2[byte_read_2 - 1] = ' ';
                    buffer_2[byte_read_2] = '\0';

                    parse_line(buffer_2, &argvcmd_2);
                    pipe(pipefd);

                    if(sub_child_pid = fork())
                    {
                        wait(&sub_child_pid);
                        dup2(pipefd[0], STDIN_FILENO);
                        close(pipefd[0]);
                        close(pipefd[1]);
                        execvp(argvcmd_2[0], argvcmd_2);
                    }
                }

                if(strpbrk(buffer, ">")) //Redirection
                {
                    int fd_out = open(argvcmd[nb_arg - 1], O_RDWR | O_CREAT | O_TRUNC, 0666);
                    if(fd_out < 0)
                    {
                        perror("Error open: ");
                    } else {
                        if(dup2(fd_out, STDOUT_FILENO) < 0)
                        {
                            perror("Error dup2: ");
                        }
                        close(fd_out);
                    }       
                }

                if(sub_child_pid == 0)
                {
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[0]);
                    close(pipefd[1]);
                }

                if(execvp(argvcmd[0], argvcmd) < 0) //Execute
                {
                    perror("Error execution: ");
                    exit(-1);
                }

            } else if(child_pid < 0){
                perror("Error fork: ");
            } else { //Parent  
                wait(&child_pid);
            }
        }
    }
    
    return 0;
}
