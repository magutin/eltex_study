#include "sc.h"

msg_s *recv;

int main()
{
    int shmid;
    int semid;
    char loc_buf[80];
    size_t buf_lenght;
    char name[NAME];
    size_t sz_name;
    short initarray[3] = {1,0,0};			

    /* Создаем разделяемую память */
    shmid = shmget(SHMid,sizeof(msg_s), 0777 | IPC_CREAT);
    recv = (msg_s *)shmat(shmid,0,0);//,0,0);
    printf("Shared mem create\n");
    

    /* создаем и инициализируем семафор */
    semid = semget(SEMid,3,0777 | IPC_CREAT);
    semctl(semid,3,SETALL,initarray); 	/* access=1,answer=0,query=0 */
    // sem_init(&SEM->access,1,1)
    // sem_init(&SEM->answer,1,0)
    // sem_init(&SEM->query,1,0)

    // получаем сообщение
    while(1){
    	
	if(sem_wait(semid,query)<0) perror("sem_wait/query");
    	
	if(strlen(recv->msg)==0)
    	    break;
    	strncpy(&loc_buf[0],recv->msg,strlen(recv->msg));
    	printf("[<- msg]: %s\n",loc_buf);
      	strncpy(&loc_buf[0],"Hello! I got your msg",21);
      	strncpy(recv->msg,&loc_buf[0],strlen(loc_buf));
    	
	if(sem_post(semid,answer)<0)  perror("sem_post/answer");
        if(sem_post(semid,access)<0)  perror("sem_post/access");
        
	strncpy(&loc_buf[0],"",80);

    }
	    /* удаляем семафор  */
        semctl(semid,3,IPC_RMID,0);

        /* удаляем разделяемую память */
        shmctl(shmid,IPC_RMID,0);
        printf("\n\nserver terminated\n");
    exit(0);
}
      
