#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

/*
	[Дед]
	/   \
[Сын2]  [Сын1]
			\
		   [Внук]
*/

void main(){
	pid_t pid,pid1;
	pid_t const ppid = getpid();			// узнаем пид самого главного предка(Далее Дед)
	int return_val;

	printf("Я Дед,мой PID: [%d]\n",ppid);
	pid = fork();							// узнаем пид самого главного предка(Далее Сын1)						
	
	if(pid == -1){
		perror("fork pid");
		exit(-1);		     //завершаем процесс с Дедом
	}else if(pid == 0){						// Процесс Сын1
		printf("Я Сын1, мой PID:  [%d]\n",getpid());
		printf("PID родителя:     [%d]\n",getppid());
		printf("PID Деда:         [%d]\n",ppid);
	}else if(getpid()==ppid){
			printf("Это снова Дед!:   [%d]\n",ppid);
	}

	pid1 = fork();							// узнаем пид самого главного предка(Далее Сын2 и Внук)

	if(pid1 == -1){
		perror("fork pid1");
		exit(-2);		     //завершаем процесс с Сыном1 и Дедом
	}

	else if(pid1 == 0){
		if(getppid()==ppid){				// Процесс Сын2
			printf("Я Сын2, мой PID:  [%d]\n",getpid());
			printf("PID родителя:     [%d]\n",getppid());
			printf("PID Деда:         [%d]\n",ppid);
			return_val = 2;
			exit(return_val);
		}
		else if(pid1 == 0){					// Процесс Внук
			printf("Я Внук, мой PID:  [%d]\n",getpid());
			printf("PID родителя:     [%d]\n",getppid());
			printf("PID Деда:         [%d]\n",ppid);
			return_val = 3;
			exit(return_val);
		}
	}
	

	if(pid == 0){					// Ожидаем завершения работы Внука
		waitpid(pid1,&return_val,0);
		printf("Код возврата Внука:%d\n",WEXITSTATUS(return_val));
		return_val = 1;
		exit(return_val);
	}


	if(getpid()==ppid){				// Ожидаем завершения работы Сына1 и Сына2
		waitpid(pid,&return_val,0);
		printf("Код возврата Сын1 :%d\n",WEXITSTATUS(return_val));
		waitpid(pid1,&return_val,0);
		printf("Код возврата Сын2 :%d\n",WEXITSTATUS(return_val));
	}
}