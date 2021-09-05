#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
    int pipefd[2];
    pid_t cpid;
    char *tokens[20];
    char cmd[20];
    char ch;

    // Дочерний процесс читает из канала
    do{
        if (pipe(pipefd) == -1) {   //создание канала.(Если произошла ошибка)
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        cpid = fork();              // создаем дочерний процесс
        if (cpid == -1) {           // проверка на ошибку создания процесса
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if(cpid == 0){
        close(pipefd[1]);    // Закрываем для него канал на запись
        int i = 0;
        while(read(pipefd[0],&ch,1) > 0){
            cmd[i] = ch;
            i++;
        }
        
        cmd[strlen(cmd) - 1] = '\0';
        i=0;
        tokens[0] = strtok(cmd," ");
        while(tokens[i] != NULL) {
            tokens[++i] = strtok(NULL, " ");
        }

        execvp(tokens[0],tokens);
        printf("\n");
        close(pipefd[0]);
        //_exit(EXIT_SUCCESS);

    // Родительский процесс пишет в канал
    }else{
        close(pipefd[0]);                   // читаем введенную пользователем строку
        printf("cmd>");
        fgets(cmd,20,stdin);

        int i = 0;
        int word = 0;
        write(pipefd[1],cmd,20);
        close(pipefd[1]);
        wait(NULL);
        //exit(EXIT_SUCCESS);
    
    }
        
    }while(1);
    return 0;
}
