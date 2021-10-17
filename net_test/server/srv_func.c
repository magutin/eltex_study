#include "heads.h"


/*
	Парсер введенной команды.  
	разбирает строку буфера команды на токены и заполняет 
	структуру с настройками,либо просто сообщает о том,
	что пора запустить тест.Возвращает 1 в случае успешного разбора
	и 0 в случае ,если строка была введена неправильно

	<input= "* buf"      desc="буфер команды"/>
	<input= "len_buf"    desc="длина команды"/>
	<return="0\1"    	 desc="возвращает статус успеха разбора"/> 
*/
int cmd_parser(char* buf,int len_buf){
	printf("\n\r");
	
	if(!buf){
		perror("ERR: no command\n");
		return 0;
	}

	char *tokens[10];
	char *rest = buf;
    int cnt = 1;
    
    tokens[0] = strtok_r(rest," ",&rest);
    
    while ((tokens[cnt] = strtok_r(rest, " ", &rest))){cnt++;}

   	if(strstr(tokens[0],"run")!=NULL){
   		if(cnt != 2){
			printf("\tERR: Invalid format of parametrs!");
   			printf("\n\r\tUse: run <command>");
   			return 0;}
   		if(strstr(tokens[1],"seq")){settings.cmd = htons(1);return 1;}
   		if(strstr(tokens[1],"ping")){settings.cmd = htons(2);return 1;}
   		if(strstr(tokens[1],"load")){settings.cmd = htons(3);return 1;}
   		if(strstr(tokens[1],"turnoff")){settings.cmd = htons(4);return 1;}
   	}
   	if(strstr(tokens[0],"set")!=NULL){
   		if(cnt <= 2){
   			printf("\tERR: Invalid format of parametrs!");
   			printf("\n\r\tUse: set <command>");
   			return 0;}

    	if (strstr(tokens[1],"seq")!=NULL){
    		if(cnt < 7){
   			printf("\tERR: Invalid format of parametrs!");
   			printf("\n\r\tUse: set seq <type_proto> <len_ptk> <cnt_ptk> <time_test> <ip_cln>");
   			return 0;}	

    		printf("\tSetting for test sequence:\n\r");
    		settings.cmd = htons(5);
    		if(strstr(tokens[2],"-T")!=NULL){
    			settings.type_ptk = htons(1);
    			printf("\tType protocol:\tTCP\n\r");}
    		else if(strstr(tokens[2],"-U")!=NULL){
    			settings.type_ptk = htons(2);
    			printf("\tType protocol:\tUDP\n\r");}
    		else{printf("\n\r\tInvalid type of protocol!");
    				return 0;} 

    		settings.len_ptk = htons(atoi(tokens[3]));
    		if(ntohs(settings.len_ptk)>65507 && ntohs(settings.type_ptk)==2){printf("\n\r\tToo long to package UDP!");return 0;}
    		if(ntohs(settings.len_ptk)>65495 && ntohs(settings.type_ptk)==1){printf("\n\r\tToo long to package TCP!");return 0;}
    		settings.cnt_ptk = htons(atoi(tokens[4]));
    		settings.time_test = htons(atoi(tokens[5]));
    		settings.ip_cln = htonl(inet_addr(tokens[6]));

    		printf("\tCount package:\t%u\n\r",ntohs(settings.cnt_ptk));
    		printf("\tLen package:\t%u\n\r",ntohs(settings.len_ptk));
    		printf("\tTesting time:\t%u\n\r",ntohs(settings.time_test));
    		printf("\tСlient IP:\t%s\n\r",tokens[6]);
    		return 1;

    	}else if(strstr(tokens[1],"ping")!=NULL){
    		if(cnt != 5){
   				printf("\tInvalid format of parametrs!");
   				printf("\n\r\tUse: set ping <cnt_ptk> <time_test> <ip_cln>");
   				return 0;}
    		printf("\tSetting for test ping:\n\r");
    		settings.cmd = htons(6);
    		settings.cnt_ptk = htons(atoi(tokens[2]));
    		settings.time_test = htons(atoi(tokens[3]));
    		settings.ip_cln = htonl(inet_addr(tokens[4]));
    		printf("\tCount package:\t%u\n\r",ntohs(settings.cnt_ptk));
    		printf("\tTesting time:\t%u\n\r",ntohs(settings.time_test));
    		printf("\tСlient IP:\t%s\n\r",tokens[4]);
    		return 1;

    	}else if(strstr(tokens[1],"load")!=NULL){
    		if(cnt != 5){
   				printf("\tInvalid format of parametrs!");
   				printf("\n\r\tUse: set load <type_proto> <len_ptk> <ip_cln>");
   				return 0;}
    		settings.cmd = htons(7);

    		/*if(strstr(tokens[2],"-T")!=NULL){
    			settings.type_ptk = htons(1);
    			printf("\tType protocol:\tTCP\n\r");}*/
    		if(strstr(tokens[2],"-U")!=NULL){
    			settings.type_ptk = htons(2);
    			printf("\tType protocol:\tUDP\n\r");}
    		else{printf("\n\r\tInvalid type of protocol!");
    				return 0;}
    		settings.len_ptk = htons(atoi(tokens[3]));
    		if(ntohs(settings.len_ptk)>65507){printf("\n\r\tToo long to package UDP!");return 0;}
    		settings.ip_cln = htonl(inet_addr(tokens[4]));
    		printf("\tLen package:\t%u\n\r",ntohs(settings.len_ptk));
    		printf("\tСlient IP:\t%s\n\r",tokens[4]);
    		return 1;
    	}else if(strstr(tokens[1],"seq")==NULL && strstr(tokens[1],"load")==NULL && strstr(tokens[1],"ping")==NULL){
    		printf("\t\rERR:Unknown command");
    		return 0;
    	}
	}else{printf("\t\rERR:Unknown command");return 0;} 
}





/*
	Отправляем команду клиенту и ждем ответа три секунды,
	принял ли он команду.Неблокирруемый сокет.  

	<input= "_sock"       desc="структура сокета"/>
	<input= "_fd_sock"    desc="дескриптор сокета"/> 
*/
void cmd_sender(int _socket_desk,struct sockaddr_in _sock){
	char rcv_ptk[64];
	char snd_ptk[64];

	unsigned short cnt_attempt = 1; // количество попыток отправки
	unsigned short flag=0;
	unsigned long razn = 3000;
	unsigned long time_ms_check;	// переменная для хранения времени	

	struct ip_h        ip_hdr_rcv;
	struct udp_h      udp_hdr_rcv;

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
    ip->saddr = 0;						
    ip->daddr = htonl(settings.ip_cln);
    memcpy(&snd_ptk[0],ip,20);			// копирую в буфер заполненную структуру IP

    // udp заголовок в буфере
    struct udphdr *udp = (struct udphdr *) (snd_ptk + sizeof (struct iphdr));
    udp->source = htons(SRV_TCP_SPORT);
    udp->dest = htons(SRV_TCP_DPORT);
    udp->len = htons(44);
    udp->check = 0;
    memcpy(&snd_ptk[20],udp,8);			// копирую в буфер заполненную структуру UDP
    
    // указатель на данные 
    char* data = snd_ptk + sizeof (struct iphdr) + sizeof (struct udphdr);
    memcpy(data,&settings,sizeof(struct cmd_settings));

    //получаем длину сокета
	socklen_t sock_len = sizeof(_sock);

	udp->check = in_cksum((unsigned short *)&snd_ptk[20], 44);
	unsigned long check = 0;
	// конвертируем данные из узлового порядка расположения байтов в сетевой 

	printf("\t[->] Command SEND to client (pckg cnt %d)\n\r",cnt_attempt);

    if(sendto(_socket_desk, snd_ptk, sizeof(snd_ptk), 0, (struct sockaddr*)&_sock, sock_len)==-1) {
		perror("\tERR: Command 'set' not send. send():");
		close(_socket_desk);
		endwin();
		exit(1);
	}
	time_ms_check = get_time_ms();
 	unsigned long time_check;

    while(1){
		recvfrom(_socket_desk, rcv_ptk,sizeof(rcv_ptk), 0, (struct sockaddr*)&_sock, &sock_len) ;

		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&udp_hdr_rcv,&rcv_ptk[20],8);
		
		if(ntohs(udp_hdr_rcv.DPORT)==SRV_TCP_SPORT){
			printf("\t[<-] Client RECIEV the command");flag = 1;}	

		time_check = get_time_ms();
   		if ( (time_check-time_ms_check)>razn ){break;}
   			
		if (flag==1){break;}
	}
	if(flag==0){printf("\t[ !] Client no answer!");}	
}





/*
	Функция получения времени в мс  
	<return = "_time_ms"    desc="время в миллисекундах"/> 
*/
unsigned long get_time_ms(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_ms;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_ms = tv.tv_sec * 1000 + (tv.tv_usec / 1000); // сек->мсек + мкс->мсек
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_ms;
}





/*
	Функция подсчета CRC  
	<input = "*ptr"  	 desc="указатель на буфер пакета"/>
	<input = "nbytes"    desc="размер буфера в байтах"/> 
*/
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;}

    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;}

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return (answer);
}





/*
	Функция завершает работу сервера 
	<input= "_cmd"  	  desc="указатель на буфер с командой"/>
	<input= "_fd_sock"    desc="дескриптор сокета"/> 
*/
int quit_cmd(char* _cmd,int _fd_sock){
	if (strstr(_cmd,"quit") != NULL){
		strncpy(_cmd,"",40);
		close(_fd_sock);
		printf("\n");
		endwin();
		return 0;
	}else{
		return 1;
	}
}




/*
	Функция отправляет настройки клиенту
	<input= "_cmd"  	  desc="указатель на буфер с командой"/>
	<input= "_sock"       desc="структура сокета"/>
	<input= "_fd_sock"    desc="дескриптор сокета"/> 
*/
void set_cmd(char* _cmd,int _fd_sock,struct sockaddr_in _sock){
	if (strstr(_cmd,"set") != NULL){			// отправка команды клиенту на установку настроек
		// recv set cmd param
		if(cmd_parser(_cmd,strlen(_cmd))==1){
			cmd_sender(_fd_sock,_sock);
		}
	}
}





/*
	Функция запускает введенную команду,Запускается новый поток с сокетом 
	для выбранного теста
	<input= "_cmd"  	  desc="указатель на буфер с командой"/>
	<input= "_sock"       desc="структура сокета"/>
	<input= "_fd_sock"    desc="дескриптор сокета"/> 
*/
void run_cmd(char* _cmd,int _fd_sock,struct sockaddr_in _sock){
	if (strstr(_cmd,"run") != NULL){
	// recv set cmd param
		if(cmd_parser(_cmd,strlen(_cmd))==1){		
			if(ntohs(settings.type_ptk)==2 && ntohs(settings.cmd)==1){	// команда seq udp
				pthread_create(&tid[1], NULL,thread_udp_sock,0);	
				cmd_sender(_fd_sock,_sock);
				pthread_join(tid[1], NULL);							
			}
			if(ntohs(settings.type_ptk)==1 && ntohs(settings.cmd)==1){	// команда seq tcp
				pthread_create(&tid[1], NULL,thread_tcp_sock,0);	
				cmd_sender(_fd_sock,_sock);
				pthread_join(tid[1], NULL);							
			}
			if(ntohs(settings.cmd)==4){cmd_sender(_fd_sock,_sock);}		// команда выключения
			if(ntohs(settings.cmd)==2){cmd_sender(_fd_sock,_sock);}		// команда пинг
			if(ntohs(settings.type_ptk)==2 && ntohs(settings.cmd)==3){	// команда load udp
				pthread_create(&tid[1], NULL,thread_udp_sock_load,0);	
				cmd_sender(_fd_sock,_sock);
				pthread_join(tid[1], NULL);							
			}
			/*if(ntohs(settings.type_ptk)==1 && ntohs(settings.cmd)==3){	// команда load tcp не работает =(
				pthread_create(&tid[1], NULL,thread_tcp_sock_load,0);			не отлажено =(
				cmd_sender(_fd_sock,_sock);
				pthread_join(tid[1], NULL);							
			}*/
		}
	}	
}



/*
	Функция дополнения введенной второй команды от символа до полного назавания
	<input= "cmd"  desc="указатель на буфер с командой"/>
	<input= "i"    desc="позиция последнего символа в команде"/>
	<return="i"    desc="новая позиция последнего символа в команде"/> 
*/
int iter_tab2(char* cmd,int i){
	int len,k;
	if (strstr(&cmd[4],"s")!=NULL){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");	
			delch();
		}
		strcpy(&cmd[4],"seq ");
		i = 8;
		printf("%s",cmd);				
	}
	if (strstr(&cmd[4],"t")!=NULL){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");	
			delch();
		}
		strcpy(&cmd[4],"turnoff ");
		i = 12;
		printf("%s",cmd);				
	}
	if (strstr(&cmd[4],"p")!=NULL){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");	
			delch();
		}
		strcpy(&cmd[4],"ping ");
		i = 9;
		printf("%s",cmd);
	}
	if (strstr(&cmd[4],"l")!=NULL){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");delch();}
		strcpy(&cmd[4],"load ");
		i =9;
		printf("%s",cmd);		
	}
	return i;
}



/*
	Функция дополнения введенной первой команды от символа до полного назавания
	<input= "cmd"  desc="указатель на буфер с командой"/>
	<input= "i"    desc="позиция последнего символа в команде"/>
	<return="i"    desc="новая позиция последнего символа в команде"/> 
*/
int iter_tab1(char* cmd,int i){
	int len,k;
	if (cmd[0] == 'q'){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");	
			delch();
		}
		strncpy(cmd,"",40);
		strcat(cmd,"quit ");
		i = 5;
		printf("%s",cmd);
	}
	if (cmd[0] == 'r'){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");	
			delch();
		}
		strncpy(cmd,"",40);
		strcat(cmd,"run ");
		i = 4;
		printf("%s",cmd);
	}		
	if (cmd[0] == 's'){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");	
			delch();
		}
		strncpy(cmd,"",40);
		strcat(cmd,"set ");
		i = 4;
		printf("%s",cmd);
	}
	return i;
}