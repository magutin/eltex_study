#ifndef _SC_H_
#define _SC_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


/*shared_mem ключ */
#define SHMid 400

#define NAME      20
#define MSGSZ     40

typedef struct
{
	char string[512];
	int  count;
	int  cmd;
} SHMdata;

typedef struct msg_buf {
    char msg[MSGSZ];         // сообщение размером 40 byte 
} msg_s;

#define SEMid 127

/* семафоры */
#define access      0	//доступ
#define answer     1	//ответ
#define query       2	//очередь

struct sembuf sop;

int sem_wait(int sid, int snum)		// семафор для ожидания
{
    sop.sem_num = snum; /* номер семафора*/
    sop.sem_op = -1;
    return semop(sid,&sop,1);
}

int sem_post(int sid, int snum)		//разблокировка
{       
    sop.sem_num = snum; /* номер семафора*/
    sop.sem_op = 1;
    return semop(sid,&sop,1);
}

#endif
