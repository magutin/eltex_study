#include "sc.h"

msg_s *send;

int main(){  
    int shmid;
    int semid;
    char loc_buf[80];
    size_t buf_lenght;
    char name[NAME];
    size_t sz_name;

    printf("Username (max 30 chars):");
    fgets(name,NAME,stdin);           //даем пользователю возможность выбрать имя
    sz_name = strlen(name);       
    name[sz_name-1] = ':';
    printf("%s\n",name);

    shmid = shmget(SHMid, sizeof(msg_s),0777);
    send = (msg_s *)shmat(shmid,0,0);
    
    semid = semget(SEMid,0,0);
    
    while(1){
    
      if(sem_wait(semid,access)<0) { 
      	 perror("sem_wait/access");
      	 break;}
	
      printf("[msg ->]: ");
      
      strncpy(&loc_buf[0],"",80);
      strncpy(send->msg,"",40);
      
      fgets(&loc_buf[0],MSGSZ,stdin);        // юзер вводит сообщение
      buf_lenght = strlen(loc_buf);

      strncpy(send->msg,&loc_buf[0],strlen(loc_buf));
      
      sem_post(semid,query);
      // Ждем ответа
      
      if(sem_wait(semid,answer)<0)  perror("sem_wait/access");
      printf("[<- msg]: %s   \n",send->msg);
      
   }
   exit(0);      
}   
