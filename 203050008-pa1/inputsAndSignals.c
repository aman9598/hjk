#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include<string.h>
#include<sys/wait.h>
int child;
void handler(int);
void handler2(int);
int main()
{
   // printf("%d - processId \n", getpid());
    while (1)
    {
        int fd[2];
        pipe(fd);
        child = fork();
        if (child == 0)
        {
            close(fd[0]);
            char command[1000];
            printf("myShell> ");
            fflush(stdout);
            fflush(stdin);
            fgets(command, 1000, stdin);
            if(strcmp(command,"exit\n") == 0)
            {     char p = 'x';
                  write(fd[1],&p,sizeof(char));
                  close(fd[1]);
                  exit(1);
            }
            strtok(command,"\n");
            char *args[] = {command, NULL};
            execvp("./main", args);
        }
        else
        {
            signal(SIGINT, handler);
            signal(SIGTERM, handler2);
            close(fd[1]);
            int K;
            wait(NULL);
            char q;
            read(fd[0],&q,1);
            close(fd[0]);
            if(q == 'x') break;
        }
    }
    _exit(1);
}
void handler(int signum) //handling SIGINT
{
    if(child == 0)
    {
       exit(1);
    }
    else
    {
          char a;
          printf("the program is interrupted, do you want to exit [Y/N]:");
          scanf("%c", &a); getchar();
          if(a == 'Y')
          {
             exit(1);
          }
          fflush(stdout);
    } 
    
}
void handler2(int signum) // handling SIGTERM
{
   printf("Got SIGTERM-Leaving\n");
   exit(1);
}