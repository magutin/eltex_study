//#include <cmd.h>
#include "heads.h"
#include <sys/types.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct cmd_settings settings;
char* ip_s;

unsigned short in_cksum(unsigned short *ptr, int nbytes);
unsigned long get_time_ms();

void* run_ping();
void* test();
void* run_seq_udp();

int main(){
	int fd_sock_cln;
	struct sockaddr_in sock_cln;
	struct in_addr ip_n;
	
	struct ip_h        ip_hdr_rcv;
	struct udp_h      udp_hdr_rcv;
	char rcv_ptk[64];
	char snd_ptk[64];

	pthread_t th_id;	

	memset(&sock_cln, 0, sizeof(sock_cln));					// обнуляем структуру

	sock_cln.sin_family = AF_INET;  					// в структуре сервера говорим,о соединении типа IPv4
    sock_cln.sin_addr.s_addr = htonl(INADDR_ANY); 		// все адреса локальной сети 0.0.0.0
    sock_cln.sin_port = htons(SRV_TCP_DPORT); 			// конвертируем данные из узлового порядка расположения байтов в сетевой 

    // Сокет UDP бу(дет посылать настройки для теста клиенту
    if((fd_sock_cln = socket(AF_INET, SOCK_RAW,IPPROTO_UDP)) == -1) {	//получаем дискриптор сокета сервера
		perror("ERR: line 24, socket():\n\r");
		exit(1);
	}
	int on = 1;
	if (setsockopt (fd_sock_cln, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) {
        perror("setsockopt");
        exit (1);
    }

	socklen_t sock_len = sizeof(sock_cln);

	while(1){
		if(recvfrom(fd_sock_cln, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&sock_cln, &sock_len)==-1) {
			perror("Recvfrom:\n\r");
			exit(1);
		}
		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&udp_hdr_rcv,&rcv_ptk[20],8);
		//printf("DPORT=%u\n",ntohs(udp_hdr_rcv.DPORT));
		//printf("SPORT=%u\n",ntohs(udp_hdr_rcv.SPORT));

		if(ntohs(udp_hdr_rcv.DPORT)==SRV_TCP_DPORT){
			memcpy(&settings,&rcv_ptk[28],16);
			
			settings.cmd = ntohs(settings.cmd);
			settings.type_ptk = ntohs(settings.type_ptk);
			settings.len_ptk = ntohs(settings.len_ptk);
			settings.cnt_ptk = ntohs(settings.cnt_ptk);
			settings.time_test = ntohs(settings.time_test);
			settings.vlan = ntohs(settings.vlan);
			settings.ip_cln = ntohl(settings.ip_cln);

			/*
			printf("\tcmd=%u\t",settings.cmd);				// номер команды теста 
			printf("type_ptk=%u\t",settings.type_ptk);		// тип пакета (TCP/UDP/ICMP)
			printf("len_ptk=%u\t",settings.len_ptk);		// длина пакета
			printf("cnt_ptk=%u\t",settings.cnt_ptk);		// количество пакетов
			printf("time_test=%u\t",settings.time_test);	// время проведения теста
			printf("vlan=%u\t",settings.vlan);				// поддержка 802.1q,802.1p*/
			ip_n.s_addr = settings.ip_cln;
			//printf("ip_cln=%s\n",ip_s=inet_ntoa(ip_n));		//ntohs(settings.ip_cln));		// ip клиента
			ip_s=inet_ntoa(ip_n);
    		
			// ip заголовок в буфере
   			struct iphdr *ip = (struct iphdr *) snd_ptk;
    		ip->version = 4;
    		ip->ihl = 5;
    		ip->tos = 0;
    		ip->tot_len = htons(64);
    		ip->id = 500;
    		ip->frag_off = 0;
    		ip->ttl = 65;
    		ip->protocol = IPPROTO_UDP;
    		ip->saddr = 0;						//inet_addr("192.168.1.4");
    		ip->daddr = settings.ip_cln;
    		//ip->check = in_cksum ((u16 *) ip, sizeof (struct iphdr));
    		memcpy(&snd_ptk[0],ip,20);			// копирую в буфер заполненную структуру IP

    		// udp заголовок в буфере
    		struct udphdr *udp = (struct udphdr *) (snd_ptk + sizeof (struct iphdr));
    		udp->source = htons(SRV_TCP_DPORT);
    		udp->dest = htons(SRV_TCP_SPORT);
    		udp->len = htons(44);
			udp->check = in_cksum((unsigned short *)&snd_ptk[20], 44);

    		memcpy(&snd_ptk[20],udp,8);			// копирую в буфер заполненную структуру UDP
    		//memcpy(&snd_ptk[28],&settings,sizeof(&settings));
    		
			if(sendto(fd_sock_cln, snd_ptk, sizeof(snd_ptk), 0, (struct sockaddr*)&sock_cln, sock_len)==-1) {
				perror("\tERR: Command 'set' not send. send():");
				exit(1);
			}	
			//sleep(3);
			//стартуем тест порядка
			if(settings.cmd==1){
				printf("[->] RECIEVE command \"start seq\"\n");
				if(settings.type_ptk==2){
					pthread_create(&th_id, NULL,run_seq_udp,0);
					pthread_join(th_id, NULL);
				}
			}

			//стартуем тест пинг
			if(settings.cmd==2){
				printf("[->] RECIEVE command \"start ping\"\n");
				pthread_create(&th_id, NULL,run_ping,0);
				pthread_join(th_id, NULL);
			}
			
			//стартуем тест load
			if(settings.cmd==3){
				printf("[->] RECIEVE command \"start load\"\n");
			}
			
			//завершаем работу
			if(settings.cmd==4){
				printf("[->] RECIEVE command \"start turnoff\"\n");
				close(fd_sock_cln);
				return 0;
			}
			
			//задаем настройки для теста
			if(settings.cmd==5){
				printf("[->] RECIEVE command \"settings for seq\"\n");
			}

			//задаем настройки для теста
			if(settings.cmd==6){
				printf("[->] RECIEVE command \"settings for ping\"\n");
			}

			//задаем настройки для теста 
			if(settings.cmd==7){
				printf("[->] RECIEVE command \"settings for load\"\n");
				
			}
		}
					
	}

	return 0;
}

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

unsigned long get_time_ms(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_ms;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_ms = tv.tv_sec * 1000 + (tv.tv_usec / 1000); // сек->мсек + мкс->мсек
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_ms;
}

void* test(){
	for(int i = 0;i<4;i++){
		printf("thread\n");
	}
}

void* run_ping(){


    char snd_ptk[64];			// буфер отправки
    char rcv_ptk[64];			// буфер приема

    int fd_sock_p;				// десриптор сокета
	unsigned long time_ms = 0;	// хранение времени в мс
	unsigned long time_ms_check = 0;	// хранение времени в мс
	int packet_size;			// общий размер пакета
	socklen_t sock_len;			// размер сокета

	struct sockaddr_in sck_adrr_p;
	struct icmp_h      icmp_hdr;
	struct icmp_h      icmp_hdr_rcv;
	struct ip_h        ip_hdr;
	struct ip_h        ip_hdr_rcv;
	struct in_addr ip_n;
	unsigned long d_ip;			// ip назначения

	char* d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);  // конвертируем ip назначения

    //обнуление буферов и полей структуры
	strncpy(snd_ptk,"",64);
	strncpy(rcv_ptk,"",64);
	memset(&sck_adrr_p, 0, sizeof(sck_adrr_p));

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
    if((fd_sock_p = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
       	perror("ERR: line 59, socket():");
       	exit(1);
    }
    
    if(setsockopt(fd_sock_p, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) == -1) {
	    perror("setsockopt");
	    exit(1);
  	}
	
	printf("[ !] START TEST\n");
	
	for(int i = 0;i<settings.cnt_ptk;i++){
    	
    	// заполняем структуру сервера
        sck_adrr_p.sin_family    = AF_INET;  		// константа, определяющая коммуникационный домен "Интернет		
        sck_adrr_p.sin_addr.s_addr = d_ip;		// IP-адрес - это структура
        //srv.sin_port = htons(PORT);			// номер порта назначения

    
		//получаем длину сокета
		sock_len = sizeof(sck_adrr_p);


		
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

		printf("\tping %s  %ld(%ld) bytes of data\tpackage num %u\n",d_ip_str,sizeof(snd_ptk)-28,sizeof(snd_ptk),icmp_hdr.SEQ_NUM);
		memcpy(&snd_ptk[20],&icmp_hdr,8);
		
		//printf("ismp_seq=%u\n",icmp_hdr.SEQ_NUM);

		icmp_hdr.CRC = in_cksum((unsigned short *)&snd_ptk[20], 44);
		memcpy(&snd_ptk[20],&icmp_hdr,8);	// копирую в буфер заполненную структуру ICMP

		if(sendto(fd_sock_p, snd_ptk,sizeof(snd_ptk), 0, (struct sockaddr*)&sck_adrr_p, sock_len) == -1) {
        	perror("ERR: line 75, socket():");
        	exit(1);}

        if(recvfrom(fd_sock_p, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&sck_adrr_p, &sock_len) == -1) {
		   	perror("ERR: line 81, socket():");
		   	exit(1);
		}

		time_ms_check = get_time_ms();
		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&icmp_hdr_rcv,&rcv_ptk[20],8);
		int size = sizeof(rcv_ptk);
		//ip_hdr_rcv.S_IP = ip_hdr_rcv.S_IP;
		ip_n.s_addr = ip_hdr_rcv.S_IP;
		printf("\t%d bytes from: %s ",size,inet_ntoa(ip_n));
		printf("icmp_seq=%u ttl=%u time=%ld ms\n",icmp_hdr_rcv.SEQ_NUM,ip_hdr_rcv.TTL,time_ms_check-time_ms);
		sleep(1); 
 			
	}
	printf("[ !] END TEST\n");
	close(fd_sock_p);

}

/*
void* run_seq_tcp(){

    char send[92];
    
    unsigned long d_ip;								// ip назначения
    int count_packg;								// количество пакетов
    int payload_len = settings.len_ptk;		// размер блока данных
    
    printf("payload_len=%d\t",payload_len);
    int sent = 0;

    unsigned long time_snd = 0;						// время отправки пакета
	unsigned long time_rcv = 0;						// время приема пакета

    count_packg = settings.cnt_ptk;
    printf("count_ptk=%d\t",count_packg);
    // конвертируем ip назначения

    char* d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);  // конвертируем ip назначения

    // вычисляем общий размер пакета
    int packet_size = sizeof (struct iphdr) + (struct tcphdr) + payload_len; 
    printf("packet_size=%d\n",packet_size);
    
    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc (packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc (packet_size);	// буфер приема	

    if(!snd_ptk || !rcv_ptk){
    	perror("ERR: out of memory: rcv_ptk || snd_ptk");
    	exit(1);
    }

    // Обнуляем буферы
    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct sockaddr_in sck_adrr;		// структура сокета
    int fd_sock;						// десриптор сокета
	socklen_t sock_len;					// длина сокета
	
	// Создаем сокет в зависимости от выбранного протокола
    if((fd_sock = socket(AF_INET, SOCK_RAW, type_proto)) == -1){
    	perror("ERR: socket();\n");
    	free(snd_ptk);
		free(rcv_ptk);
    	exit(1);
    }
    
    int on = 1;
    // Мы обещаем предоставить IP заголовок
    if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) {
        perror("setsockopt");
        free(snd_ptk);
		free(rcv_ptk);
        exit (1);
    }

  	// ip заголовок в буфере
    struct iphdr *ip = (struct iphdr *) snd_ptk;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(packet_size);
    ip->id = 500;
    ip->frag_off = 0;
    ip->ttl = 65;
    ip->protocol = type_proto;
    ip->saddr = 0;
    ip->daddr = d_ip;
    memcpy(&snd_ptk[0],ip,20);	// копирую в буфер заполненную структуру IP

    struct udphdr *tcp = (struct tcphdr *) (snd_ptk + sizeof (struct iphdr));
    tcp->source = htons(S_PORT);
    tcp->dest = htons(D_PORT);
    tcp->len = htons(packet_size-20);
    tcp->check = 0;
    memcpy(&snd_ptk[20],tcp,20);	// копирую в буфер заполненную структуру ICMP
  	
   	sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(S_PORT);
    
    sock_len = sizeof(sck_adrr);

	for(int i = 0;i<count_packg;i++){

		memset(snd_ptk + sizeof(struct iphdr) + sizeof(struct udphdr), (char)1+i, payload_len);

		udp->check = 0;
        udp->check = in_cksum((unsigned short *)udp, sizeof(struct udphdr) + payload_len);
        
        for(int j = 0;j < packet_size;j++){
        	printf("%x ",ntohs(snd_ptk[j]));
        }
        printf("\n");
        if(sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, sock_len) < 1) {	// отправляем сообщение
			perror("ERR: line 28. send()");
			exit(1);
		}

		++sent;
        printf("%d packets sent\r", sent);
        fflush(stdout);
        
        //usleep(10000);  //microseconds
        sleep(1);
	}

	
	free(snd_ptk);
	free(rcv_ptk);
	return 0;

}
*/

void* run_seq_udp(){

    char send[92];  
    unsigned long d_ip;								// ip назначения
    int count_packg;								// количество пакетов
    int payload_len = settings.len_ptk;				// размер блока данных
    printf("payload_len=%d\t",payload_len);
    int sent = 0;
    int recv = 0;
    unsigned long time_snd = 0;						// время отправки пакета
	unsigned long time_rcv = 0;						// время приема пакета

    // конвертируем количество пакетов
    count_packg = settings.cnt_ptk;
    printf("count_ptk=%d\t",count_packg);
    
    // конвертируем ip назначения
    char* d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);  // конвертируем ip назначения

    // вычисляем общий размер пакета
    int packet_size = sizeof (struct iphdr) + sizeof(struct udphdr) + payload_len; 
    printf("packet_size=%d\n",packet_size);

    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc (packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc (packet_size);	// буфер приема	
    if(!snd_ptk || !rcv_ptk){
    	perror("ERR: out of memory: rcv_ptk || snd_ptk");
    	exit(1);
    }
    // Обнуляем буферы
    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct ip_h ip_rcv ;
    struct udp_h udp_rcv;

    struct sockaddr_in sck_adrr;		// структура сокета
    int fd_sock;						// десриптор сокета
	socklen_t sock_len;					// длина сокета
	
	// Создаем сокет в зависимости от выбранного протокола
    if((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1){
    	perror("ERR: socket();\n");
    	free(snd_ptk);
		free(rcv_ptk);
    	exit(1);
    }
    
    // Мы обещаем предоставить IP заголовок
    int on = 1;
    if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) 
    {
        perror("setsockopt");
        free(snd_ptk);
		free(rcv_ptk);
        exit (1);
    }

  	// ip заголовок в буфере
    struct iphdr *ip = (struct iphdr *) snd_ptk;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(packet_size);
    ip->id = 500;
    ip->frag_off = 0;
    ip->ttl = 65;
    ip->protocol = IPPROTO_UDP;
    ip->saddr = 0;
    ip->daddr = d_ip;
    memcpy(&snd_ptk[0],ip,20);	// копирую в буфер заполненную структуру IP

   	struct udphdr *udp = (struct udphdr *) (snd_ptk + sizeof (struct iphdr));
   	udp->source = htons(UDP_DPORT);
   	udp->dest = htons(UDP_SPORT);
   	udp->len = htons(packet_size-20);
   	memcpy(&snd_ptk[20],udp,8);	// копирую в буфер заполненную структуру ICMP
   	
   	sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(UDP_DPORT);
    
    sock_len = sizeof(sck_adrr);
    printf("[ !] START TEST!\n");
	for(int i = 0;i<count_packg;i++){

		memset(snd_ptk + sizeof(struct iphdr) + sizeof(struct udphdr), (char)1+i, payload_len);

		udp->check = 0;
        udp->check = in_cksum((unsigned short *)udp, sizeof(struct udphdr) + payload_len);
        
        if(sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, sock_len) < 1) {	// отправляем сообщение
			perror("ERR: line 28. send()");
			free(snd_ptk);
			free(rcv_ptk);
			close(fd_sock);
			exit(1);
		}
		++sent;
        printf("%d packets sent\n", sent);
        
        
		if(recvfrom(fd_sock, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&sck_adrr, &sock_len) == -1) {
		    perror("ERR: line 81, socket():");
		    free(snd_ptk);
			free(rcv_ptk);
			close(fd_sock);
		    exit(1);
		}

		memcpy(&ip_rcv,&rcv_ptk[0],20);
		memcpy(&udp_rcv,&rcv_ptk[20],8);

		int size = sizeof(rcv_ptk);
		
		if(ntohs(udp_rcv.DPORT)==UDP_DPORT){
			++recv;
			printf("%d packets sent\n", sent);
			
		}
		
        usleep(50000);  //microseconds
        //sleep(1);
	}
	printf("[ !] END TEST!\n");
	free(snd_ptk);
	free(rcv_ptk);
	close(fd_sock);
}

