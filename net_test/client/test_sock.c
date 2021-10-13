#include "heads.h"

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


void* run_seq_tcp(){

#define FREE_MEM free(snd_ptk);free(rcv_ptk)

    unsigned long d_ip;								// ip назначения
    int count_packg;								// количество пакетов
    int payload_len = settings.len_ptk;		// размер блока данных
    
    int sent = 0;
    int recv = 0;

    unsigned long time_snd = 0;						// время отправки пакета
	unsigned long time_rcv = 0;						// время приема пакета

    count_packg = settings.cnt_ptk;
    // конвертируем ip назначения

    char* d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);  // конвертируем ip назначения

    // вычисляем общий размер пакета
    int packet_size = sizeof (struct iphdr) + sizeof(struct tcphdr) + payload_len; 
    
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
    if((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1){
    	perror("ERR: socket();\n");
    	FREE_MEM;
    	exit(1);
    }
    
    int on = 1;
    // Мы обещаем предоставить IP заголовок
    if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) {
        perror("setsockopt");
        FREE_MEM;
        close(fd_sock);
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
    ip->protocol = IPPROTO_TCP;
    ip->saddr = 0;
    ip->daddr = d_ip;
    
    struct tcphdr *tcp = (struct tcphdr *) (snd_ptk + sizeof (struct iphdr));
    tcp->source = htons(TCP_DPORT);
    tcp->dest = htons(TCP_SPORT);
    tcp->seq = htons(1);
    tcp->ack_seq = 0;
    tcp->window = htons(32767);
    tcp->check = 0;
    tcp->urg_ptr = 0;

    char* data =  snd_ptk + sizeof (struct iphdr) + sizeof(struct tcphdr);
	ip->check = in_cksum((unsigned short *)data, payload_len);

	struct iphdr *ip_rcv = (struct iphdr *) rcv_ptk;
    struct tcphdr *tcp_rcv = (struct tcphdr *) (rcv_ptk + sizeof (struct iphdr));
    
 	memcpy(&snd_ptk[0],ip,20);	// копирую в буфер заполненную структуру IP
    memcpy(&snd_ptk[20],tcp,20);	// копирую в буфер заполненную структуру ICMP
  	
   	sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(TCP_DPORT);
    
    sock_len = sizeof(sck_adrr);

	for(int i = 0;i<count_packg;i++){

		memset(snd_ptk + sizeof(struct iphdr) + sizeof(struct udphdr), (char)1+i, payload_len);

        printf("\n");
        if(sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, sock_len) < 1) {	// отправляем сообщение
			perror("ERR: line 28. send()");
			FREE_MEM;
			close(fd_sock);
			exit(1);
		}

		++sent;
		
        printf("%d packets sent\n", sent);
        fflush(stdout);

        if(recvfrom(fd_sock, rcv_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, &sock_len) == -1) {
		    perror("ERR: line 81, socket():");
		    FREE_MEM;
		    close(fd_sock);
		    exit(1);
		}
		if(tcp_rcv->dest==htons(TCP_DPORT)){
			++recv;
			printf("prot=%u ",ip_rcv->protocol);
			printf("dp=%u\n",ntohs(tcp_rcv->dest));
			printf("%d packets recv\n", recv);	
		}  
        //usleep(10000);  //microseconds
        sleep(1);
	}
	FREE_MEM;
	close(fd_sock);
#undef FREE_MEM

	return 0;
}


void* run_seq_udp(){
#define FREE_MEM free(snd_ptk);free(rcv_ptk)
    
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

    struct iphdr *ip_rcv = (struct iphdr *) rcv_ptk;
    struct udphdr *udp_rcv = (struct udphdr *) (rcv_ptk + sizeof (struct iphdr));

    struct sockaddr_in sck_adrr;		// структура сокета
    int fd_sock;						// десриптор сокета
	socklen_t sock_len;					// длина сокета
	
	// Создаем сокет в зависимости от выбранного протокола
    if((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1){
    	perror("ERR: socket();\n");
    	FREE_MEM;
    	exit(1);
    }
    
    // Мы обещаем предоставить IP заголовок
    int on = 1;
    if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) 
    {
        perror("setsockopt");
        FREE_MEM;
        close(fd_sock);
        exit (1);
    }

  	// ip заголовок в буфере
    struct iphdr *ip = (struct iphdr *) snd_ptk;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(packet_size);
    ip->id = 0;
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
		ip->id = i;
		udp->check = 0;
        udp->check = in_cksum((unsigned short *)udp, sizeof(struct udphdr) + payload_len);
        
        if(sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, sock_len) < 1) {	// отправляем сообщение
			perror("ERR: line 28. send()");
			FREE_MEM;
			close(fd_sock);
			exit(1);
		}
		++sent;
        printf("%d packets sent\n", sent);
        
        
		if(recvfrom(fd_sock, rcv_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, &sock_len) == -1) {
		    perror("ERR: line 81, socket():");
		    FREE_MEM;
		    close(fd_sock);
		    exit(1);
		}

		printf("prot=%u ",ip_rcv->protocol);
		printf("dp=%u\n",ntohs(udp_rcv->dest));
		if(ntohs(udp_rcv->dest)==UDP_DPORT){
			++recv;
			printf("%d packets recv\n", sent);
			
		}
		
        usleep(50000);  //microseconds
        //sleep(1);
	}
	printf("[ !] END TEST!\n");
	FREE_MEM;
	close(fd_sock);
#undef FREE_MEM
	return 0;
}
