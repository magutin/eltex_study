#include "head.h"

typedef struct msg_buf {
    long mtype;                // тип сообщение
    char msg[MSGSZ];         // сообщение размером 128 байт
} msg_s;

msg_s recv;
msg_s send;

int msqid_r;
int msqid_s;
int msqid_r2;


int main()
{
    send.mtype=100;
    int msgflg = IPC_CREAT | 0666;        // создание очереди с флагами на чтение и запись
    key_t key_r2 = 110;                    // ключ для создания очереди прочитать
    key_t key_r = 100;                    // ключ для создания очереди прочитать
    key_t key_s = 90;                     // ключ для создания очереди отправить
    size_t buf_length;                    // размер сообщения
    size_t size_name;
    char name[NAME];					

    // открываем очередь
    if ((msqid_r = msgget(key_r, msgflg)) < 0) {
        perror("msgget");
        exit(1);
    }

    if ((msqid_s = msgget(key_s, msgflg)) < 0) {
        perror("msgget");
        exit(1);
    }
    
    if ((msqid_r2 = msgget(key_r2, msgflg)) < 0) {
       perror("msgget");
       exit(1);
    }
    
    // получаем сообщение

    
    while(1){
    	if (msgrcv(msqid_r, &recv, MSGSZ, 0, 0) < 0) {
        	perror("msgrcv");
        	exit(1);
        }else{printf("[msg]: %s\n", recv.msg);
            for(int i =0;i<80;i++){
                send.msg[i] = recv.msg[i];
            }
            msgsnd(msqid_s, &send,MSGSZ, IPC_NOWAIT);
            msgsnd(msqid_s, &send,MSGSZ, IPC_NOWAIT);
        }      
        if (msgrcv(msqid_r2, &recv, MSGSZ, 0, 0) < 0) {
        	perror("msgrcv");
        	exit(1);
        }else{printf("[msg]: %s\n", recv.msg);
            for(int i =0;i<80;i++){
                send.msg[i] = recv.msg[i];
            }
            msgsnd(msqid_s, &send,MSGSZ, IPC_NOWAIT);
            msgsnd(msqid_s, &send,MSGSZ, IPC_NOWAIT);
        }  
    }
    
    exit(0);
}
/*
void* send_msg(){
    while(1){
        if (msgsnd(msqid_w, &send,MSGSZ, IPC_NOWAIT) < 0) {    // отправка сообщения
            printf ("%d, %s, %ld\n", msqid,send.msg, buf_length);
            perror("msgsnd");
            exit(1);
        }
        else{printf("[msg]: %s\n", send.msg);} 
    }    
}
*/         
