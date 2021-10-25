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
   		if(strstr(tokens[1],"seq")!=NULL){settings.cmd = htons(1);return 1;}
   		if(strstr(tokens[1],"ping")!=NULL){settings.cmd = htons(2);return 1;}
   		if(strstr(tokens[1],"load")!=NULL){settings.cmd = htons(3);return 1;}
   		if(strstr(tokens[1],"turnoff")!=NULL){settings.cmd = htons(4);return 1;}
   	}
   	if(strstr(tokens[0],"set")!=NULL){
   		if(cnt <= 2){
   			printf("\tERR: Invalid format of parametrs!");
   			printf("\n\r\tUse: set <command>");
   			return 0;}

    	if (strstr(tokens[1],"seq")!=NULL){
    		if(cnt < 6){
   			printf("\tERR: Invalid format of parametrs!");
   			printf("\n\r\tUse: set seq <type_proto> <len_ptk> <cnt_ptk> <time_test> ");//<ip_cln>");
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
    		//settings.ip_cln = htonl(inet_addr(tokens[6]));

    		printf("\tCount package:\t%u\n\r",ntohs(settings.cnt_ptk));
    		printf("\tLen package:\t%u\n\r",ntohs(settings.len_ptk));
    		printf("\tTesting time:\t%u\n\r",ntohs(settings.time_test));
    		//printf("\tСlient IP:\t%s\n\r",tokens[6]);
    		return 1;

    	}else if(strstr(tokens[1],"ping")!=NULL){
    		if(cnt != 4){
   				printf("\tInvalid format of parametrs!");
   				printf("\n\r\tUse: set ping <cnt_ptk> <time_test> ");//<ip_cln>");
   				return 0;}
    		printf("\tSetting for test ping:\n\r");
    		settings.cmd = htons(6);
    		settings.cnt_ptk = htons(atoi(tokens[2]));
    		settings.time_test = htons(atoi(tokens[3]));
    		//settings.ip_cln = htonl(inet_addr(tokens[4]));
    		printf("\tCount package:\t%u\n\r",ntohs(settings.cnt_ptk));
    		printf("\tTesting time:\t%u\n\r",ntohs(settings.time_test));
    		//printf("\tСlient IP:\t%s\n\r",tokens[4]);
    		return 1;

    	}else if(strstr(tokens[1],"load")!=NULL){
    		if(cnt != 5){
   				printf("\tInvalid format of parametrs!");
   				printf("\n\r\tUse: set load <type_proto> <len_ptk> ");//<ip_cln>");
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
    		//settings.ip_cln = htonl(inet_addr(tokens[4]));
    		printf("\tLen package:\t%u\n\r",ntohs(settings.len_ptk));
    		//printf("\tСlient IP:\t%s\n\r",tokens[4]);
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
void cmd_sender(int _socket_desk){
	int data_size = sizeof(struct cmd_settings);
	
	char *send_cmd = (char *)malloc(data_size);
	if(!send_cmd){
    	perror("ERR: out of memory: send_cmd");
    	exit(1);
    }
    memset(send_cmd, 0, data_size);
	memcpy(send_cmd,(char*)&settings,data_size);

	printf("\t[->] Command SEND to client");

    if(send(_socket_desk, send_cmd, data_size, 0) == -1) {
		perror("ERR: line 58. send():");
		exit(1);
	}
	
	free(send_cmd);
}




void recv_file(int _fd_sock,int _test_type){
	printf("\n\r\t[ !]Recieve file from client\n\r");
	char* buf = NULL;
	char* buffer_data = NULL;
	int size_str = 80;
	int size_recv=0;
	int counter = 0;
	char data[size_str];
	memset(data, 0, size_str);
	
	while(1){
		if(recv(_fd_sock,data,size_str,0)==-1){perror("ERR: File recv():");exit(1);}
		if(strstr(data,"end ptk")){break;}
		else if(size_recv!=-1){
			counter++;
			size_recv+=strlen(data);
			printf("\t%s\r",data); 
			buf = (char*) realloc (buffer_data, size_recv);
 
     		if (buf != NULL){
       			buffer_data = buf;
       			strncat(buffer_data,data,size_recv);
     		}else{
       			free(buffer_data);                                   // удалить массив
       			endwin();
       			exit (1);                                          	// завершить работу программы
     		}
		}
	}
	printf("\t(%d) bytes of file reciev from client\n\r",size_recv);
	printf("\tDo you want save result in file?(y/n): ");
	char chose = getchar();
	printf("%c\n\r",chose);
	if(chose=='y' || chose=='Y'){
		FILE *fname;
		char name[30];
		get_file_name(name,30,_test_type);
    	if ((fname = fopen(name, "w+")) == NULL){
    		printf("ERR: fopen();\n");
    		endwin();
    		exit(1);
    	}
    	printf("\tFile saved as %s",name);
		fprintf(fname,"%s",buffer_data);
		fclose(fname);
		
	}else{printf("\tExit without saving");
	
	}
		free(buffer_data);
		buffer_data = NULL;
}



void get_file_name(char *name_str,int size,int type_test){
	strncpy(name_str,"",size);
	strncpy(name_str,"srv_files/",10);

	if(type_test==test_seq_u){
		strcat(name_str,"test_seq_udp.txt");
	}else if(type_test==test_seq_t){
		strcat(name_str,"test_seq_tcp.txt");
	}else if(type_test==test_ping){
		strcat(name_str,"test_ping.txt");
	}else if(type_test==test_load){
		strcat(name_str,"test_load.txt");
	}
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
void set_cmd(char* _cmd,int _fd_sock){
	if (strstr(_cmd,"set") != NULL){			// отправка команды клиенту на установку настроек
		// recv set cmd param
		if(cmd_parser(_cmd,strlen(_cmd))==1){
			cmd_sender(_fd_sock);
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
void run_cmd(char* _cmd,int _fd_sock){
	if (strstr(_cmd,"run") != NULL){
	// recv set cmd param
		if(cmd_parser(_cmd,strlen(_cmd))==1){		
			if(ntohs(settings.type_ptk)==2 && ntohs(settings.cmd)==1){	// команда seq udp
				pthread_create(&tid[1], NULL,thread_udp_sock,0);	
				cmd_sender(_fd_sock);
				pthread_join(tid[1], NULL);
				recv_file(_fd_sock,test_seq_u);							
			}
			if(ntohs(settings.type_ptk)==1 && ntohs(settings.cmd)==1){	// команда seq tcp
				pthread_create(&tid[1], NULL,thread_tcp_sock,0);	
				cmd_sender(_fd_sock);
				pthread_join(tid[1], NULL);
				recv_file(_fd_sock,test_seq_t);							
			}
			if(ntohs(settings.cmd)==4){cmd_sender(_fd_sock);}		// команда выключения
			if(ntohs(settings.cmd)==2){cmd_sender(_fd_sock);
			recv_file(_fd_sock,test_ping);}		// команда пинг
			if(ntohs(settings.type_ptk)==2 && ntohs(settings.cmd)==3){	// команда load udp
				pthread_create(&tid[1], NULL,thread_udp_sock_load,0);	
				cmd_sender(_fd_sock);
				pthread_join(tid[1], NULL);
				recv_file(_fd_sock,test_load);							
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