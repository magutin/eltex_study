#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>		// для gettimeofday
#include <sys/types.h>		// для getpid
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#define data_size_ptk 64

#pragma pack(push,1)
/*  Заголовок IP 
	Сумма байт 20 */
struct ip_h{
	//нужен разворот для биг индиан  1 байт = VER.HLEN
	unsigned char  HELEN   : 4;		// Номер версии			4  бита--| 	
	unsigned char  VER     : 4;		// Длина заголовка  	4  бита--|
	unsigned char  SERV_TYPE;		// Тип Сервиса			8  бит
	unsigned short TOTL_LEN;		// Общая длина 			16 бит
	unsigned short IDENTIF;			// ID пакета			16 бит
	unsigned short F_OFFST : 13 ;	// Флаги			    3  бита--|
	unsigned short FLAGS   : 3;	    // Смещение фрагмента	13 бит---|
	unsigned char  TTL;				// Время жизни			8  бит
	unsigned char  PROTOCOL;		// Тип протокола		8  бит
	unsigned short H_CSUM;			// контрольная сумма	16 бит
	unsigned int   S_IP;		    // IP отправителя		32 бита
	unsigned int   D_IP;		    // IP получателя		32 бита
};
#pragma pack(pop)

/*	Заголовок ICMP
	Сумма байт 8 */
struct icmp_h{
	unsigned char  TYPE;		// порт отправителя		8  бит
	unsigned char  CODE;		// порт назначения		8  бит
	unsigned short CRC;			// Длина пакета			16 бит
	unsigned short ID;			// контрольная сумма	16 бит
	unsigned short SEQ_NUM;		// контрольная сумма	16 бит
};

unsigned long get_time_ms();
unsigned short in_cksum(unsigned short *ptr, int nbytes);

int main(int argc, char **argv){

	if (argc < 1) {
        printf("usage: %s <source IP> <destination IP> [payload size]\n", argv[0]);
        exit(0);
    }


    char snd_ptk[64];			// буфер отправки
    char rcv_ptk[64];			// буфер приема

    int fd_sock;				// десриптор сокета
	
	unsigned long time_ms = 0;	// хранение времени в мс
	unsigned long time_ms_check = 0;	// хранение времени в мс
	
	int packet_size;			// общий размер пакета
	socklen_t sock_len;			// размер сокета

	struct sockaddr_in sck_adrr;
	struct icmp_h      icmp_hdr;
	struct icmp_h      icmp_hdr_rcv;
	struct ip_h        ip_hdr;
	struct ip_h        ip_hdr_rcv;

	unsigned long d_ip;			// ip назначения
	
	char* d_ip_str = argv[1];
    d_ip = inet_addr(/*"192.168.1.4");*/argv[1]);  // конвертируем ip назначения

    //обнуление буферов и полей структуры
	strncpy(snd_ptk,"",64);
	strncpy(rcv_ptk,"",64);
	memset(&sck_adrr, 0, sizeof(sck_adrr));

	// Заполняю структуру заголовка IP	
	ip_hdr.VER 		 = 0x4;						// версия IPv4 - 4
	ip_hdr.HELEN     = 0x5;						// длина заголовка IP в словах ,мин 5(5*4 байта = 20 байт)
	ip_hdr.SERV_TYPE = 0x00;					// 
	ip_hdr.TOTL_LEN  = 0x40;//64;				// общая длина пакета,в нашем случае 108 байт(ip+udp+data)
	ip_hdr.IDENTIF   = 0x00;					// ID пакета			в нашем случае 0
	ip_hdr.FLAGS     = 0x00;					// Флаги			    в нашем случе 0
	ip_hdr.F_OFFST   = 0x00;    				// Смещение фрагмента   в нашем случае 0	
	ip_hdr.TTL       = 0x40;					// Время жизни			
	ip_hdr.PROTOCOL  = 0x01;					// Тип протокола		в нашем случае следующий заголовок от UDP -> 17
	ip_hdr.H_CSUM    = 0x00;					// контрольная сумма	линукс игнорит это поле ,поэтому 0
	ip_hdr.S_IP 	 = 0x00;					// IP отправителя		Linux заполнит сам
	ip_hdr.D_IP      = d_ip;					// IP получателя		
	
	// Заполняю структуру заголовка icmp
	icmp_hdr.TYPE 	 = 0x8;
	icmp_hdr.CODE 	 = 0x0;
	icmp_hdr.CRC  	 = 0x0;
	icmp_hdr.ID      = 0x1F4;
	icmp_hdr.SEQ_NUM = 0x0;
 	
 	memcpy(&snd_ptk[0],&ip_hdr,20);	// копирую в буфер заполненную структуру IP
 	memcpy(&snd_ptk[20],&icmp_hdr,8);	// копирую в буфер заполненную структуру ICMP

 	//создаем сокет 
    if((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
       	perror("ERR: line 59, socket():");
       	exit(1);
    }

    if(setsockopt(fd_sock, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) == -1) {
	    perror("setsockopt");
	    exit(1);
  	}
	
	
	for(int i = 0;i<5;i++){
		
    	// заполняем структуру сервера
        sck_adrr.sin_family    = AF_INET;  		// константа, определяющая коммуникационный домен "Интернет		
        sck_adrr.sin_addr.s_addr = d_ip;		// IP-адрес - это структура
        //srv.sin_port = htons(PORT);			// номер порта назначения

    
		//получаем длину сокета
		sock_len = sizeof(sck_adrr);


		printf("ping %s\t",&d_ip_str[0]);
		time_ms = get_time_ms();
		
		snd_ptk[28] = (char)(time_ms);
		snd_ptk[29] = (char)(time_ms >> 8);
		snd_ptk[30] = (char)(time_ms >> 16);
		snd_ptk[31] = (char)(time_ms >> 24);
		snd_ptk[32] = (char)(time_ms >> 32);
		snd_ptk[33] = (char)(time_ms >> 40);
		snd_ptk[34] = (char)(time_ms >> 48);
		snd_ptk[35] = (char)(time_ms >> 56);		

		icmp_hdr.TYPE 	 = 0x8;
		icmp_hdr.CODE 	 = 0x0;
		icmp_hdr.CRC  	 = 0x0;
		icmp_hdr.ID      = 0x1F4;
		icmp_hdr.SEQ_NUM = 0x0;
		icmp_hdr.SEQ_NUM = (char)(i);

		memcpy(&snd_ptk[20],&icmp_hdr,8);
		
		printf("ismp_seq=%u\t",icmp_hdr.SEQ_NUM);

		icmp_hdr.CRC = in_cksum((unsigned short *)&snd_ptk[20], 44);
		memcpy(&snd_ptk[20],&icmp_hdr,8);	// копирую в буфер заполненную структуру ICMP

		if(sendto(fd_sock, snd_ptk,sizeof(snd_ptk), 0, (struct sockaddr*)&sck_adrr, sock_len) == -1) {
        	perror("ERR: line 75, socket():");
        	exit(1);}

        if(recvfrom(fd_sock, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&sck_adrr, &sock_len) == -1) {
		   	perror("ERR: line 81, socket():");
		   	exit(1);
		}

		time_ms_check = get_time_ms();
		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&icmp_hdr_rcv,&rcv_ptk[20],8);
		int size = sizeof(rcv_ptk);
		printf("ttl=%u\t",ip_hdr_rcv.TTL);
		printf("time=%ld ms\t",time_ms_check-time_ms);	// милисекунды
		printf("bytes=%d\n",size);
		printf("type=%d\tcode=%d\tid1=%u\tseq_num2=%u\n",icmp_hdr_rcv.TYPE,icmp_hdr_rcv.CODE,icmp_hdr_rcv.ID,icmp_hdr_rcv.SEQ_NUM);


		sleep(1); 
 			
	}

	close(fd_sock);

	return 0;
}




unsigned long get_time_ms(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_ms;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_ms = tv.tv_sec * 1000 + (tv.tv_usec / 1000); // сек->мсек + мкс->мсек
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_ms;
}

//ptm = localtime(&tv.tv_sec); 								// преобразование времени в структуру типа tm. 
//strftime(time_string, sizeof(time_string),"%H:%M:%S", ptm);	// Форматирование значения времени в строку 
//printf("[%s]",time_string); 								// вывод времени 


unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return (answer);
}