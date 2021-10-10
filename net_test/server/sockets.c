/* UDP сокет ,который будет создан серверу если выберут тип пакета UDP*/

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
	
	/*if ((listen(fd_sock_srv, 10)) == -1) { 			// готовность принимать пакеты и задаем размер очереди 
       	perror("ERR: line 36. listen():"); 
       	exit(1); 
    }

    if( (fd_sock_cln = accept(fd_sock_srv, (struct sockaddr *)&sock_cln,&sock_len)) == -1) {		// принять соединение на сокете
    	perror("ERR: line 43. accept():");
    	exit(1);
    }*/
    //while(1){
    for(int i = 0;i<10;i++){
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
}


/* UDP сокет ,который будет создан серверу если выберут тип пакета UDP*/
void* thread_udp_sock(){
	char txt_ans[23] = "Server reciev msg time:";
	char answer[92];						// буфер ответа
	char reciev[92];						// буфер приема

	int fd_sock_srv;						// файловый дескриптор сокета сервера UDP
	int fd_sock_cln;						// файловый дескриптор сокета клиента UDP
	
	struct sockaddr_in sock_srv;
	struct sockaddr_in sock_cln;

	struct ip_h ip_hdr_rcv;
	//struct udp_h udp_hdr_rcv;
	struct udphdr udp_hdr_rcv;
	
	struct timeval tv;			// структура времени
 	struct tm* ptm;				
 	char time_string[40];		// строка для хранения времени
 	long ms;

	memset(&sock_srv, 0, sizeof(sock_srv)); // обнуляем структуру
	memset(&sock_cln, 0, sizeof(sock_cln)); // обнуляем структуру

	// создаем сокет UDP
	if((fd_sock_srv = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {		//получаем дискриптор сокета UDP
		perror("ERR: socket() for UDP\n\r");
		exit(1);}
	printf("[Socket UDP] - Created. ID socket: %d\n\r",fd_sock_srv);		// id сокета
	
	//обещаем заполнить IP заголовок
  	if(setsockopt(fd_sock_srv, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) == -1) {
	    perror("ERR: setsocket() for UDP\n\r");
	    exit(1);}
	printf("[Socket UDP] - Options for IP header set\n\r");					// опции установлены
  	
  	// назначаем имя сокету
  	if(bind(fd_sock_srv, (struct sockaddr *)&sock_srv, sizeof(sock_srv)) == -1) {		
		perror("ERR: bind() for UDP\n\r");									// сокету присвоено имя
		exit(1);}
	printf("[Socket UDP] - bind() ok\n\r");					// опции установлены

	sock_srv.sin_family = AF_INET;  			  // в структуре сервера говорим,о соединении типа IPv4
    	sock_srv.sin_addr.s_addr = htonl(INADDR_ANY);   // все адреса локальной сети 0.0.0.0
    	sock_srv.sin_port = htons(S_PORT); 			  // конвертируем данные из узлового порядка расположения байтов в сетевой 

 	socklen_t sock_len = sizeof(sock_srv);			// узнаем размер сокета
    	for(int i = 0;i<10;i++){

		if(recvfrom(fd_sock_srv, reciev, sizeof(reciev), 0, (struct sockaddr*)&sock_srv, &sock_len) == -1) {
			perror("Recvfrom:\n\r");
			exit(1);
		}

		gettimeofday(&tv, NULL);	// Определение текущего времени и преобразование полученного значения в структуру типа tm. 
 		ptm = localtime(&tv.tv_sec);
 		strftime(time_string, sizeof(time_string),"%H:%M:%S", ptm);// Форматирование значения даты и времени с точностью до секунды. 

		memcpy(&ip_hdr_rcv,&reciev[0],20);
		memcpy(&udp_hdr_rcv,&reciev[20],8);
		if(ntohs(udp_hdr_rcv.dest)==S_PORT){
			printf("[UDP <-rcv] prot=%u\n\r",ip_hdr_rcv.PROTOCOL);
			//printf("\tdp=%u\tsp=%u\tcrc=%u\tlen=%u\n",ntohs(udp_hdr_rcv.DPORT),ntohs(udp_hdr_rcv.SPORT),ntohs(udp_hdr_rcv.CRC),ntohs(udp_hdr_rcv.LEN));
			printf("\tdp=%u\tsp=%u\tcrc=%u\tlen=%u\n\r",ntohs(udp_hdr_rcv.dest),ntohs(udp_hdr_rcv.source),ntohs(udp_hdr_rcv.check),ntohs(udp_hdr_rcv.len));

			strcat(answer, txt_ans);
			strcat(answer,time_string);

			if(sendto(fd_sock_srv, answer, strlen(answer), 0, (struct sockaddr*)&sock_srv, sock_len) == -1) {
				perror("Sendto:\r");
				exit(1);
			}
			printf("[UDP snd->] prot=%u\n\r",ip_hdr_rcv.PROTOCOL);
			//printf("\t[UDP->]: %s %s\n",txt_ans,time_string);
		}

		strncpy(answer,"", 92);
		strncpy(reciev,"", 92);
    }

    close(fd_sock_srv);
    printf("[Socket UDP] - Socket closed\n\r");					// опции установлены
    return 0;
}