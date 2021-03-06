#include "heads.h"

extern struct cmd_settings settings;	// глобальная переменная структуры команд


unsigned long get_time_s()
{

    struct timeval tv;		// структура времени
    struct tm *ptm;
    unsigned long _time_s;

    gettimeofday(&tv, NULL);	// Определение текущего времени
    _time_s = tv.tv_sec;
    //printf("[%ld]\n",_time_ms);   // милисекунды

    return _time_s;
}

/* UDP сокет ,который будет создан серверу если выберут тип пакета UDP*/
void *thread_udp_sock()
{

#define FREE_MEM free(snd_ptk);free(rcv_ptk)

    struct sockaddr_in sock_srv;
    int fd_sock_srv;
    //printf("\n\r\t[ !] Socket in work");
    unsigned long work_time = (unsigned long) ntohs(settings.time_test);
    int payload_len = ntohs(settings.len_ptk);

    int cnt_ptk = ntohs(settings.cnt_ptk);
    int packet_size =
	sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len;

    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc(packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc(packet_size);	// буфер приема      
    if (!snd_ptk || !rcv_ptk) {
	perror("ERR: out of memory: rcv_ptk || snd_ptk");
	endwin();
	exit(1);
    }

    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct iphdr *ip_hdr = (struct iphdr *) snd_ptk;
    struct udphdr *udp_hdr =
	(struct udphdr *) (snd_ptk + sizeof(struct iphdr));


    // создаем сокет UDP
    if ((fd_sock_srv = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {	//получаем дискриптор сокета UDP
	perror("ERR: socket() for UDP\n\r");
	FREE_MEM;
	endwin();
	exit(1);
    }
    //обещаем заполнить IP заголовок
    if (setsockopt(fd_sock_srv, IPPROTO_IP, IP_HDRINCL, &(int) { 1 },
		   sizeof(int)) == -1) {
	perror("ERR: setsocket() for UDP\n\r");
	FREE_MEM;
	close(fd_sock_srv);
	endwin();
	exit(1);
    }

    sock_srv.sin_family = AF_INET;	// в структуре сервера говорим,о соединении типа IPv4
    sock_srv.sin_addr.s_addr = htonl(INADDR_ANY);	// все адреса локальной сети 0.0.0.0
    sock_srv.sin_port = htons(UDP_SPORT);	// конвертируем данные из узлового порядка расположения байтов в сетевой 

    // назначаем имя сокету
    if (bind(fd_sock_srv, (struct sockaddr *) &sock_srv, sizeof(sock_srv))
	== -1) {
	perror("ERR: bind() for UDP\n\r");	// сокету присвоено имя
	endwin();
	exit(1);
    }


    socklen_t sock_len = sizeof(sock_srv);	// узнаем размер сокета

    int i = 0;

    unsigned long time = get_time_s();
    unsigned long time_check;

    while (i < cnt_ptk) {

	time_check = get_time_s();

	if ((time_check - time) > work_time) {
	    break;
	}
	if (recvfrom
	    (fd_sock_srv, rcv_ptk, packet_size, 0,
	     (struct sockaddr *) &sock_srv, &sock_len) == -1) {
	    perror("Recvfrom:\n\r");
	    FREE_MEM;
	    close(fd_sock_srv);
	    endwin();
	    exit(1);
	}

	memcpy(snd_ptk, rcv_ptk, packet_size);
	if (udp_hdr->dest == htons(UDP_SPORT)) {

	    ip_hdr->ttl = 65;
	    ip_hdr->saddr = ip_hdr->daddr;	//ntohl(ip_hdr_rcv.daddr);
	    ip_hdr->daddr = ip_hdr->saddr;

	    udp_hdr->source = htons(UDP_SPORT);
	    udp_hdr->dest = htons(UDP_DPORT);
	    udp_hdr->len = udp_hdr->len;

	    if (sendto
		(fd_sock_srv, snd_ptk, packet_size, 0,
		 (struct sockaddr *) &sock_srv, sock_len) == -1) {
		perror("Sendto:\r");
		FREE_MEM;
		close(fd_sock_srv);
		endwin();
		exit(1);
	    }

	    i++;
	}
	memset(rcv_ptk, 0, packet_size);
	memset(snd_ptk, 0, packet_size);
    }

    FREE_MEM;
    close(fd_sock_srv);
    //printf("\n\r\t[ !] Socket closed");

#undef FREE_MEM
    return 0;
}



/* TCP сокет ,который будет создан серверу если выберут тип пакета TCP*/

void *thread_tcp_sock()
{

#define FREE_MEM free(snd_ptk);free(rcv_ptk)

    int fd_srv;
    struct sockaddr_in sock_srv;

    //printf("\n\r\t[ !] Socket in work");

    unsigned long work_time = (unsigned long) ntohs(settings.time_test);
    int payload_len = ntohs(settings.len_ptk);

    int cnt_ptk = ntohs(settings.cnt_ptk);
    int packet_size =
	sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_len;

    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc(packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc(packet_size);	// буфер приема      
    if (!snd_ptk || !rcv_ptk) {
	perror("ERR: out of memory: rcv_ptk || snd_ptk");
	endwin();
	exit(0);
    }

    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct iphdr *ip_hdr = (struct iphdr *) snd_ptk;
    struct tcphdr *tcp_hdr =
	(struct tcphdr *) (snd_ptk + sizeof(struct iphdr));

    sock_srv.sin_family = AF_INET;	// в структуре сервера говорим,о соединении типа IPv4
    sock_srv.sin_addr.s_addr = htonl(INADDR_ANY);	// все адреса локальной сети 0.0.0.0
    sock_srv.sin_port = htons(TCP_SPORT);	// конвертируем данные из узлового порядка расположения байтов в сетевой 

    socklen_t sock_len = sizeof(sock_srv);	// узнаем размер сокета

    // создаем сокет TCP
    if ((fd_srv = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {	//получаем дискриптор сокета TCP
	perror("ERR: socket() for TCP\n\r");
	FREE_MEM;
	endwin();
	exit(0);
    }
    // Обещаем заполнить заголовок IP
    if (setsockopt(fd_srv, IPPROTO_IP, IP_HDRINCL, &(int) { 1 },
		   sizeof(int)) == -1) {
	perror("ERR: setsocket() for TCP\n\r");
	FREE_MEM;
	close(fd_srv);
	endwin();
	exit(0);
    }
    // назначаем имя сокету сервера
    if (bind(fd_srv, (struct sockaddr *) &sock_srv, sock_len) == -1) {
	perror("ERR: setsocket() for TCP\n\r");	// сокету присвоено имя
	FREE_MEM;
	close(fd_srv);
	endwin();
	exit(0);
    }

    unsigned long time = get_time_s();
    unsigned long time_check;
    int i = 0;

    while (i < cnt_ptk) {

	time_check = get_time_s();

	if ((time_check - time) > work_time) {
	    break;
	}

	if (recvfrom
	    (fd_srv, rcv_ptk, packet_size, 0,
	     (struct sockaddr *) &sock_srv, &sock_len) == -1) {
	    perror("ERR: line 49. recv():");
	    FREE_MEM;
	    close(fd_srv);
	    endwin();
	    exit(0);
	}

	memcpy(snd_ptk, rcv_ptk, packet_size);

	if (tcp_hdr->dest == htons(TCP_SPORT)) {
	    //printf("[TCP] prot=%u\tdp=%u\tsp=%u\n\r",ip_hdr_rcv.PROTOCOL,tcp_hdr_rcv.DPORT,tcp_hdr_rcv.SPORT);    

	    ip_hdr->ttl = 65;
	    ip_hdr->saddr = ip_hdr->daddr;	//ntohl(ip_hdr_rcv.daddr);
	    ip_hdr->daddr = ip_hdr->saddr;

	    tcp_hdr->source = htons(TCP_SPORT);
	    tcp_hdr->dest = htons(TCP_DPORT);
	    //tcp_hdr->len = tcp_hdr->len;

	    if (sendto
		(fd_srv, snd_ptk, packet_size, 0,
		 (struct sockaddr *) &sock_srv, sock_len) == -1) {
		perror("ERR: line 58. send():");
		FREE_MEM;
		close(fd_srv);
		endwin();
		exit(0);
	    }
	    i++;
	}

    }

    FREE_MEM;
    close(fd_srv);

#undef FREE_MEM

    return 0;
}


/* UDP сокет ,который будет создан серверу если выберут тип пакета UDP*/
void *thread_udp_sock_load()
{

#define FREE_MEM free(snd_ptk);free(rcv_ptk)

    struct sockaddr_in sock_srv;
    int fd_sock_srv;
    //printf("\n\r\t[ !] Socket in work");
    unsigned long work_time = 5;
    int payload_len = ntohs(settings.len_ptk);

    int cnt_ptk = ntohs(settings.cnt_ptk);
    int packet_size =
	sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len;

    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc(packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc(packet_size);	// буфер приема      
    if (!snd_ptk || !rcv_ptk) {
	perror("ERR: out of memory: rcv_ptk || snd_ptk");
	endwin();
	exit(1);
    }

    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct iphdr *ip_hdr = (struct iphdr *) snd_ptk;
    struct udphdr *udp_hdr =
	(struct udphdr *) (snd_ptk + sizeof(struct iphdr));


    // создаем сокет UDP
    if ((fd_sock_srv = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {	//получаем дискриптор сокета UDP
	perror("ERR: socket() for UDP\n\r");
	FREE_MEM;
	endwin();
	exit(1);
    }
    //обещаем заполнить IP заголовок
    if (setsockopt(fd_sock_srv, IPPROTO_IP, IP_HDRINCL, &(int) { 1 },
		   sizeof(int)) == -1) {
	perror("ERR: setsocket() for UDP\n\r");
	FREE_MEM;
	close(fd_sock_srv);
	endwin();
	exit(1);
    }

    sock_srv.sin_family = AF_INET;	// в структуре сервера говорим,о соединении типа IPv4
    sock_srv.sin_addr.s_addr = htonl(INADDR_ANY);	// все адреса локальной сети 0.0.0.0
    sock_srv.sin_port = htons(UDP_SPORT);	// конвертируем данные из узлового порядка расположения байтов в сетевой 

    // назначаем имя сокету
    if (bind(fd_sock_srv, (struct sockaddr *) &sock_srv, sizeof(sock_srv))
	== -1) {
	perror("ERR: bind() for UDP\n\r");	// сокету присвоено имя
	endwin();
	exit(1);
    }


    socklen_t sock_len = sizeof(sock_srv);	// узнаем размер сокета

    int i = 0;

    unsigned long time = get_time_s();
    unsigned long time_check;

    while (1) {

	time_check = get_time_s();

	if ((time_check - time) > work_time) {
	    break;
	}

	if (recvfrom
	    (fd_sock_srv, rcv_ptk, packet_size, 0,
	     (struct sockaddr *) &sock_srv, &sock_len) == -1) {
	    perror("Recvfrom:\n\r");
	    FREE_MEM;
	    close(fd_sock_srv);
	    endwin();
	    exit(1);
	}

	memcpy(snd_ptk, rcv_ptk, packet_size);
	if (udp_hdr->dest == htons(UDP_SPORT)) {
	    time = get_time_s();
	    ip_hdr->ttl = 65;
	    ip_hdr->saddr = ip_hdr->daddr;	//ntohl(ip_hdr_rcv.daddr);
	    ip_hdr->daddr = ip_hdr->saddr;

	    udp_hdr->source = htons(UDP_SPORT);
	    udp_hdr->dest = htons(UDP_DPORT);
	    udp_hdr->len = udp_hdr->len;

	    if (sendto
		(fd_sock_srv, snd_ptk, packet_size, 0,
		 (struct sockaddr *) &sock_srv, sock_len) == -1) {
		perror("Sendto:\r");
		FREE_MEM;
		close(fd_sock_srv);
		endwin();
		exit(1);
	    }

	    i++;
	}
	memset(rcv_ptk, 0, packet_size);
	memset(snd_ptk, 0, packet_size);
    }

    FREE_MEM;
    close(fd_sock_srv);
    //printf("\n\r\t[ !] Socket closed");

#undef FREE_MEM
    return 0;
}



/* TCP сокет ,который будет создан серверу если выберут тип пакета TCP*/
/*		Не работает, не отлажено =(
void* thread_tcp_sock_load(){
	
#define FREE_MEM free(snd_ptk);free(rcv_ptk)	

	int fd_srv;
	struct sockaddr_in sock_srv;
	
	//printf("\n\r\t[ !] Socket in work");
	
	unsigned long work_time = (unsigned long)ntohs(settings.time_test);
	int payload_len = ntohs(settings.len_ptk);
	
	int cnt_ptk = ntohs(settings.cnt_ptk);
	int packet_size = sizeof (struct iphdr) + sizeof(struct tcphdr) + payload_len; 

	//выделяем память под пакеты
    char *snd_ptk = (char *) malloc (packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc (packet_size);	// буфер приема	
    if(!snd_ptk || !rcv_ptk){
    	perror("ERR: out of memory: rcv_ptk || snd_ptk");
    	endwin();
    	exit(0);}

    memset(snd_ptk,0,packet_size);
    memset(rcv_ptk,0,packet_size);

	struct iphdr *ip_hdr = (struct iphdr *) snd_ptk;
	struct tcphdr *tcp_hdr = (struct tcphdr *) (snd_ptk + sizeof (struct iphdr));	

	sock_srv.sin_family = AF_INET;  		// в структуре сервера говорим,о соединении типа IPv4
    sock_srv.sin_addr.s_addr = htonl(INADDR_ANY);  // все адреса локальной сети 0.0.0.0
  	sock_srv.sin_port = htons(TCP_SPORT); 	 // конвертируем данные из узлового порядка расположения байтов в сетевой 

  	socklen_t sock_len = sizeof(sock_srv);			// узнаем размер сокета

	// создаем сокет TCP
    if((fd_srv = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {			//получаем дискриптор сокета TCP
		perror("ERR: socket() for TCP\n\r");
		FREE_MEM;
		endwin();
		exit(0);
	}

	// Обещаем заполнить заголовок IP
	if(setsockopt(fd_srv, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) == -1) {
	    perror("ERR: setsocket() for TCP\n\r");
	    FREE_MEM;
	    close(fd_srv);
	    endwin();
	    exit(0);}

  	// назначаем имя сокету сервера
  	if(bind(fd_srv, (struct sockaddr *)&sock_srv, sock_len) == -1) {		
		perror("ERR: setsocket() for TCP\n\r");	// сокету присвоено имя
		FREE_MEM;
		close(fd_srv);
		endwin();
		exit(0);
	}
    unsigned long time = get_time_s();
 	unsigned long time_check;
    int i = 0;

    while(i<cnt_ptk){

    	time_check = get_time_s();
   		
   		if ( (time_check-time)>work_time ){
   			break;
   		}

		if(recvfrom(fd_srv, rcv_ptk, packet_size, 0, (struct sockaddr*)&sock_srv, &sock_len) ==-1){
			perror("ERR: line 49. recv():");
			FREE_MEM;
    		close(fd_srv);
    		endwin();
			exit(0);
		}

		memcpy(snd_ptk,rcv_ptk,packet_size);

		if( tcp_hdr->dest == htons(TCP_SPORT) ){
			//printf("[TCP] prot=%u\tdp=%u\tsp=%u\n\r",ip_hdr_rcv.PROTOCOL,tcp_hdr_rcv.DPORT,tcp_hdr_rcv.SPORT);	
			
			ip_hdr->ttl = 65;
			ip_hdr->saddr=ip_hdr->daddr;//ntohl(ip_hdr_rcv.daddr);
			ip_hdr->daddr=ip_hdr->saddr;

			tcp_hdr->source = htons(TCP_SPORT);
			tcp_hdr->dest = htons(TCP_DPORT);
			

			if(sendto(fd_srv, snd_ptk, packet_size, 0, (struct sockaddr*)&sock_srv, sock_len) == -1) {
				perror("ERR: line 58. send():");
				FREE_MEM;
    			close(fd_srv);
    			endwin();
				exit(0);
			}
			i++;
		}

    }

    FREE_MEM;
    close(fd_srv);

#undef FREE_MEM

    return 0;
}*/
