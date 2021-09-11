#include "head.h"

typedef struct msg_buf {
    long mtype;                // тип сообщение
    char msg[MSGSZ];         // сообщение размером 128 байт
} msg_s;

void* send_msg();

msg_s recv;
msg_s send;

int msqid_r;
int msqid_s;

size_t size_name;
char name[NAME];

int main(){
	int msgflg = 0666;        // создание очереди с флагами на чтение и запись
	key_t key_r = 90;                            // ключ для создания очереди
	key_t key_s = 100;                            // ключ для создания очереди
	size_t buf_lenght;                    // размер сообщения
    
    
    pthread_t tid_msg;
    int status;
    send.mtype = 1;

    printf("Username (max 30 chars):");
    fgets(name,NAME,stdin);           //даем пользователю возможность выбрать имя
    size_name = strlen(name);       
    name[size_name-1] = ':';
    printf("%s\n",name);
    printf("Your id:%ld\n",send.mtype);
    for(int i = 0;i<size_name;i++){
    	send.msg[i] = name[i];
    }

    if ((msqid_s = msgget(key_s, msgflg)) < 0) {   // открываем очередь
        perror("msgget");
        exit(1);
    }

    if ((msqid_r = msgget(key_r, msgflg)) < 0) {   // открываем очередь
        perror("msgget");
        exit(1);
    }

     status = pthread_create(&tid_msg, NULL, send_msg ,NULL);
     if (status != 0) {
          printf("main error: can't create thread, status = %d\n", status);
          exit(1);
     }
     pthread_detach(tid_msg);

    while(1){
     /*status = pthread_create(&tid_msg, NULL, send_msg ,NULL);
     if (status != 0) {
          printf("main error: can't create thread, status = %d\n", status);
          exit(1);
     }*/
      //printf("[%s%ld]> ",name,size_name);
      for(int i = 0;i<MSGSZ;i++){
         send.msg[size_name] = ' ';   
      }
      
      
      fgets(&send.msg[size_name+1],60,stdin);        // юзер вводит сообщение
      buf_lenght = strlen(send.msg);
      send.msg[buf_lenght-1] = '.';
      
      if (msgsnd(msqid_s, &send,MSGSZ, IPC_NOWAIT) < 0) {    // отправка сообщения
         printf ("%d, %s, %ld\n", msqid_s,send.msg, buf_lenght);
         perror("msgsnd");
         exit(1);
      }else{printf("[msg ->]: %s\n",send.msg);

      	for(int i = 0;i<MSGSZ;i++){
            send.msg[size_name] = ' ';   
         }
  	  }
  	  //pthread_join(tid_msg);
   }
   exit(0);      
}   

void* send_msg(){
    while(1){
        if (msgrcv(msqid_r, &recv,MSGSZ,100,0) < 0) {    // отправка сообщения
            //printf ("%d, %s, %ld\n", msqid_r,recv.msg, buf_lenght);
            perror("msgrcv");
            exit(1);
        }
        else{
        	if(strncmp(recv.msg,name,size_name-1)!=0){printf("[<- msg]: %s\n", recv.msg);}
        } 
    }    
}   
