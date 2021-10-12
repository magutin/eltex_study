#include "heads.h"

extern struct cmd_settings settings; // глобальная переменная структуры команд


unsigned long get_time_s(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_s;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_s = tv.tv_sec;
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_s;
}

/* UDP сокет ,который будет создан серверу если выберут тип пакета UDP*/
void* thread_udp_sock(){

	struct sockaddr_in sock_srv;
	int fd_sock_srv;
	printf("\n\r\t[ !] Socket in work");
	unsigned long work_time = (unsigned long)ntohs(settings.time_test);
	int payload_len = ntohs(settings.len_ptk);
	
	int cnt_ptk = ntohs(settings.cnt_ptk);
	int packet_size = sizeof (struct iphdr) + sizeof(struct udphdr) + payload_len; 
	
	//выделяем память под пакеты
    char *snd_ptk = (char *) malloc (packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc (packet_size);	// буфер приема	
    if(!snd_ptk || !rcv_ptk){
    	perror("ERR: out of memory: rcv_ptk || snd_ptk");
    	exit(1);}

	struct iphdr *ip_hdr = (struct iphdr *) snd_ptk;
	struct udphdr *udp_hdr = (struct udphdr *) (snd_ptk + sizeof (struct iphdr));	
	
		
	// создаем сокет UDP
	if((fd_sock_srv = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {		//получаем дискриптор сокета UDP
		perror("ERR: socket() for UDP\n\r");
		exit(1);}
	
	//обещаем заполнить IP заголовок
  	if(setsockopt(fd_sock_srv, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) == -1) {
	    perror("ERR: setsocket() for UDP\n\r");
	    exit(1);}

  	sock_srv.sin_family = AF_INET;  			  	// в структуре сервера говорим,о соединении типа IPv4
   	sock_srv.sin_addr.s_addr = htonl(INADDR_ANY);   // все адреса локальной сети 0.0.0.0
   	sock_srv.sin_port = htons(UDP_SPORT); 			// конвертируем данные из узлового порядка расположения байтов в сетевой 

  	// назначаем имя сокету
  	if(bind(fd_sock_srv, (struct sockaddr *)&sock_srv, sizeof(sock_srv)) == -1) {		
		perror("ERR: bind() for UDP\n\r");									// сокету присвоено имя
		exit(1);}

	
 	socklen_t sock_len = sizeof(sock_srv);			// узнаем размер сокета

 	int i = 0;

 	unsigned long time = get_time_s();
 	unsigned long time_check;
 	
   	while(i<cnt_ptk ){

   		time_check = get_time_s();
   		
   		if ( (time_check-time)>work_time ){
   			break;
   		}
		if(recvfrom(fd_sock_srv, rcv_ptk, packet_size, 0, (struct sockaddr*)&sock_srv, &sock_len) == -1) {
			perror("Recvfrom:\n\r");
			exit(1);
		}

		memcpy(snd_ptk,rcv_ptk,packet_size);
		if(udp_hdr->dest==htons(UDP_SPORT)){
			
			ip_hdr->ttl = 65;
			ip_hdr->saddr=ip_hdr->daddr;//ntohl(ip_hdr_rcv.daddr);
			ip_hdr->daddr=ip_hdr->saddr;

			udp_hdr->source = htons(UDP_SPORT);
			udp_hdr->dest = htons(UDP_DPORT);
			udp_hdr->len = udp_hdr->len;
			
			if(sendto(fd_sock_srv, snd_ptk, packet_size, 0, (struct sockaddr*)&sock_srv, sock_len) == -1) {
				perror("Sendto:\r");
				exit(1);
			}
			
			i++;
		}
		memset(rcv_ptk,0,packet_size);
		memset(snd_ptk,0,packet_size);
    }
    
    close(fd_sock_srv);
    printf("\n\r\t[ !] Socket closed");
    //printf("[Socket UDP] - Socket closed\n\r");					// опции установлены
    return 0;
}



/* UDP сокет ,который будет создан серверу если выберут тип пакета UDP*/
/*
void* thread_tcp_sock(){
	char txt_ans[23] = "Server reciev msg time:";
	char answer[92];						// буфер ответа
	char reciev[92];						// буфер приема
	int fd_sock_srv;						// файловый дескриптор сокета сервера TCP
	int fd_sock_cln;						// файловый дескриптор сокета клиента TCP
	struct sockaddr_in sock_srv;
	struct sockaddr_in sock_cln;
	struct timeval tv;			// структура времени
 	struct tm* ptm;				
 	char time_string[40];		// строка для хранения времени
 	long ms;

 	struct ip_h ip_hdr_rcv;
	struct tcp_h tcp_hdr_rcv;

	memset(&sock_srv, 0, sizeof(sock_srv));	// обнуляем структуру
	memset(&sock_cln, 0, sizeof(sock_cln));	// обнуляем структуру

	
	// создаем сокет TCP
    if((fd_sock_srv = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {			//получаем дискриптор сокета TCP
		perror("ERR: socket() for TCP\n\r");
		exit(1);
	}
	printf("[Socket TCP] - Created. ID socket: %d\n\r",fd_sock_srv);				// id сокета

	// Обещаем заполнить заголовок IP
	if(setsockopt(fd_sock_srv, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) == -1) {
	    perror("ERR: setsocket() for TCP\n\r");
	    exit(1);}
	printf("[Socket TCP] - Options for IP header set\n\r");						// опции установлены

  	// назначаем имя сокету сервера
  	if(bind(fd_sock_srv, (struct sockaddr *)&sock_srv, sizeof(sock_srv)) == -1) {		
		perror("ERR: setsocket() for TCP\n\r");	// сокету присвоено имя
		exit(1);
	}
	printf("[Socket TCP] - bind() ok\n\r");		// опции установлены

	sock_srv.sin_family = AF_INET;  		// в структуре сервера говорим,о соединении типа IPv4
    	sock_srv.sin_addr.s_addr = htonl(INADDR_ANY);  // все адреса локальной сети 0.0.0.0
  	sock_srv.sin_port = htons(S_PORT); 	 // конвертируем данные из узлового порядка расположения байтов в сетевой 

    socklen_t sock_len = sizeof(sock_srv);			// узнаем размер сокета
*/
	/*if ((listen(fd_sock_srv, 10)) == -1) { 			// готовность принимать пакеты и задаем размер очереди 
       	perror("ERR: line 36. listen():"); 
       	exit(1); 
    }

    if( (fd_sock_cln = accept(fd_sock_srv, (struct sockaddr *)&sock_cln,&sock_len)) == -1) {		// принять соединение на сокете
    	perror("ERR: line 43. accept():");
    	exit(1);
    }*/
    //while(1){
/*    for(int i = 0;i<10;i++){
		if(recvfrom(fd_sock_srv, reciev, sizeof(reciev), MSG_WAITALL, (struct sockaddr*)&sock_cln, &sock_len) == -1) {
			perror("Recvfrom:\n\r");
			exit(1);
		}

		gettimeofday(&tv, NULL);	// Определение текущего времени и преобразование полученного значения в структуру типа tm. 
 		ptm = localtime(&tv.tv_sec);
 		strftime(time_string, sizeof(time_string),"%H:%M:%S", ptm);// Форматирование значения даты и времени с точностью до секунды. 

		memcpy(&ip_hdr_rcv,&reciev[0],20);
		memcpy(&tcp_hdr_rcv,&reciev[20],20);

		
		if(tcp_hdr_rcv.DPORT==D_PORT){
			printf("[TCP] prot=%u\tdp=%u\tsp=%u\n\r",ip_hdr_rcv.PROTOCOL,tcp_hdr_rcv.DPORT,tcp_hdr_rcv.SPORT);	
			
			if(sendto(fd_sock_srv, answer, strlen(answer), 0, (struct sockaddr*)&sock_cln, sock_len) == -1) {
				perror("Sendto:");
				exit(1);
			}

			//printf("\t[<-TCP]:%s\n",&reciev[40]);

 			//printf("\t[<-TCP]: %s %s\n",txt_ans,time_string); // Отображение даты и времени с указанием числа миллисекунд. 

			strcat(answer, txt_ans);
			strcat(answer,time_string);

			//printf("\t[TCP->]: %s %s\n",txt_ans,time_string);

		}
		strncpy(answer,"", 92);
		strncpy(reciev,"", 92);

    }

    close(fd_sock_srv);
    printf("[Socket TCP] - Socket closed\n\r");					// опции установлены
    return 0;
}*/

