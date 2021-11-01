#include "heads.h"


/*
	Парсер введенной команды.  
	разбирает строку буфера команды на токены и заполняет 
	структуру с настройками,либо просто сообщает о том,
	что пора запустить тест.Возвращает 1 в случае успешного разбора
	и 0 в случае ,если строка была введена неправильно

	<input= "* buf"      desc="буфер команды"/>
	<input= "len_buf"    desc="длина команды"/>
	<return="0||1"       desc="возвращает статус успеха разбора"/> 
*/
int cmd_parser(char* buf,int len_buf){
    printf("\n\r");
	
    if(!buf){perror("ERR: no command\n");return 0;}

    char *tokens[10];
    char *rest = buf;
    int cnt = 1;
    
    tokens[0] = strtok_r(rest," ",&rest);
    
    while ((tokens[cnt] = strtok_r(rest, " ", &rest))){cnt++;}
	if ( strstr(tokens[0],"clnselect") != NULL){
		if(cnt != 2){
			printf("\tERR: Invalid parameter format");
			printf("\n\r\tUse: clnselect <num_cln>");
			return 0;
		}
		else{
			_fd = atoi(tokens[1]);
			return 1;	
		}
	}
	if( strstr(tokens[0],"list") != NULL ){
		if(cnt != 1){
			printf("\tERR: Invalid parametr format");
			printf("\n\r\tUse: list");
			return 0;
		}
		else{return 1;}
	}
   	if(strstr(tokens[0],"run")!=NULL){
   		if(cnt != 2){
			printf("\tERR: Invalid format of parameters!");
   			printf("\n\r\tUse: run <command>");
   			return 0;}
   		if(strstr(tokens[1],"seq")!=NULL){settings.cmd = htons(1);return 1;}
   		if(strstr(tokens[1],"ping")!=NULL){settings.cmd = htons(2);return 1;}
   		if(strstr(tokens[1],"load")!=NULL){settings.cmd = htons(3);return 1;}
   		if(strstr(tokens[1],"turnoff")!=NULL){settings.cmd = htons(4);return 1;}
   	}
   	if(strstr(tokens[0],"set")!=NULL){
   		if(cnt <= 1){
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
    			printf("\tType protocol:\tTCP\n\r");
    		}else if(strstr(tokens[2],"-U")!=NULL){
    			settings.type_ptk = htons(2);
    			printf("\tType protocol:\tUDP\n\r");
    		}else{printf("\n\r\tInvalid type of protocol!");
    				return 0;} 

    		for(int i = 3;i<=5;i++){
    			if(check_param_types(atol(tokens[i]))==0){
    				printf("\tERR:invalid num %s!\n\r",tokens[i]);
    				printf("\tUse the range [0;65535]!");
    				return 0;
    			}
    		}

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
    		for(int i = 2;i<=3;i++){
    			if(check_param_types(atol(tokens[i]))==0){
    				printf("\tERR:invalid num %s!\n\r",tokens[i]);
    				printf("\tUse the range [0;65535]!");
    				return 0;
    			}
    		}
    		settings.cnt_ptk = htons(atoi(tokens[2]));
    		settings.time_test = htons(atoi(tokens[3]));
    		printf("\tCount package:\t%u\n\r",ntohs(settings.cnt_ptk));
    		printf("\tTesting time:\t%u\n\r",ntohs(settings.time_test));
    		return 1;

    	}else if(strstr(tokens[1],"load")!=NULL){
    		if(cnt != 4){
   				printf("\tInvalid format of parametrs!");
   				printf("\n\r\tUse: set load <type_proto> <len_ptk> ");
   				return 0;}
    		settings.cmd = htons(7);

    		if(strstr(tokens[2],"-U")!=NULL){
    			settings.type_ptk = htons(2);
    			printf("\tType protocol:\tUDP\n\r");}
    		else{printf("\n\r\tInvalid type of protocol!");return 0;}
    		
    		for(int i = 3;i<=3;i++){
    			if(check_param_types(atol(tokens[i]))==0){
    				printf("\tERR:invalid num %s!\n\r",tokens[i]);
    				printf("\tUse the range [0;65535]!");
    				return 0;
    			}
    		}
    		
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
	принял ли он команду.  

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
	//
	free(send_cmd);
}



/*
	Получаем результат от клиента и предлагаем сохранить его в файл.  

	<input= "_fd_sock"    desc="дескриптор сокета"/> 
	<input= "_test_type"  desc="ип теста для определения имени выходного файла"/>
*/
void recv_file(int _fd_sock,int _test_type){
	
	printf("\n\r\t[ !] Recieve file from client\n\r");
	char* buf = NULL;
	char* buffer_data = NULL;
	int size_str = 80;
	int size_recv=0;
	int all_size = 0;
	int counter = 0;
	char data[size_str];
	memset(data, 0, size_str);
	
	while(1){
		if((size_recv=recv(_fd_sock,data,size_str,0))==-1){
			perror("ERR: File recv():");exit(1);
		}
		if(strstr(data,"end ptk")){
			break;
		}
		if(size_recv!=0){
			counter++;
			all_size+=strlen(data);
			printf("\t%s\r",data); 
			buf = (char*) realloc (buffer_data, all_size+1);
     		if (!buf){
     			printf("!buf\n\r");
     			endwin();
     			exit(1);	
     		}
     		else{
     			buffer_data=buf;
       			strncpy(&buffer_data[all_size-strlen(data)],data,strlen(data));	
     		}
		}
	}
	printf("\t(%d) bytes of file reciev from client\n\r",all_size);
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
		fwrite(buffer_data,1,all_size,fname);
		fclose(fname);	
	}
	else{
		printf("\tExit without saving");
	}
	memset(&settings,0,sizeof(struct cmd_settings));
	free(buffer_data);
}


/*
	Получаем имя выходного файла по типу теста ,который проводился.  
	<input= " *name_str"    desc="дескриптор сокета"/> 
	<input= " size"  		desc="ип теста для определения имени выходного файла"/>
	<input= " type_test"	desc="тип проводимого теста">
*/
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
		if(_fd_sock == 0){
			printf("\n\r\tERR: You must select client fd!");
			return;
		}
		if(cmd_parser(_cmd,strlen(_cmd))==1){
			cmd_sender(_fd_sock);
		}
	}
}


/*
 	Функция выбирает дескриптор сокета определенного клиента для работы с ним
	<input= "_cmd" desc="указатель на буфер с командой">
	<input= "_strct" desc="указатель на структуру с сокетами">	
*/
void clnselect_cmd(char* _cmd,void* _strct){
	if (strstr(_cmd,"clnselect") != NULL){
		
		if(cmd_parser(_cmd,strlen(_cmd))==1){
			sock_param *sock = (sock_param*) _strct;
			if (sock->fd_sock_cln[_fd]!=0){
				printf("\tSwitching succsessfully\n\r");
				sock->curr_fd = sock->fd_sock_cln[_fd];
				printf("\tCurrent cln fd %d",sock->curr_fd);
			}
			else{
				printf("\tERR: client %d NOT connected",_fd);
			}
		}
		
	}
}	


/*
 	Функция выводит список подключенных клиентов.
 	<input= "_cmd"   desc="указатель на буфер с командой">
 	<input= "_strct" desc="указатель на структуру с сокетами">	
*/
void list_cmd(char* _cmd,void* _strct){
	if (strstr(_cmd,"list") != NULL){
		if (!_cmd && !_strct)
			return;
		
		sock_param *sock = (sock_param*) _strct;
		int sd;
		int count_cln = 0;
		int addrlen = sizeof(sock->srv);
		
		if(cmd_parser(_cmd,strlen(_cmd))==1){
			for(int i = 0;i<5;i++){
				sd = sock->fd_sock_cln[i];
				if (sd != 0){
					getpeername(sd , (struct sockaddr*)&sock->cln,(socklen_t*)&addrlen);  
					printf("\tClient num %d, client fd %d, client ip %s ,client port%u \n\r",
						i,sock->fd_sock_cln[i],inet_ntoa(sock->cln.sin_addr) , ntohs(sock->cln.sin_port));	
						count_cln++;
				}
			}
			printf("\t--------------------------------------------------------\n\r");
			printf("\t%d connected clients,%d free places for connections.\n\r",count_cln,(5-count_cln));
			if(sock->curr_fd!=0){
				printf("\tCurrent client fd = %d",sock->curr_fd);		
			}else{printf("\tCurrent client NOT select");}	
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
		if(_fd_sock == 0){
			printf("\n\r\tERR: You must select client fd!");
			return;
		}

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
			if(ntohs(settings.cmd)==2){								// команда пинг
				
				cmd_sender(_fd_sock);
				recv_file(_fd_sock,test_ping);
				
			}
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
	if (cmd[0] == 'c'){
		len = strlen(cmd);
		for (k=0;k<len;k++){
			printf("\b");
			delch();
		}
		strncpy(cmd,"",40);
		strcat(cmd,"clnselect ");
		i = 10;
		printf("%s",cmd);
	}
	if(cmd[0] == 'l'){
		len = strlen(cmd);
		for(k = 0;k<len;k++){
			printf("\b");
			delch();
		}
		strncpy(cmd,"",40);
		strcat(cmd,"list ");
		i = 5;
		printf("%s",cmd);
	}	
	return i;
}


/*
	Функция дополнительного потока для обработки клиентов,которые 
	хотят отключиться или подключиться.
	<input= "_sock_param"  desc="указатель на структуру с дескрипторами сокетов"/>
*/
void* accept_new_client(void* _sock_param){

	sock_param *sock = (sock_param*) _sock_param;
	
	int addrlen = sizeof(sock->srv);
	char buffer[1025];  // буфер данных 1K
	int new_socket;
	int max_sd;  
    int max_clients = 5;
    int activity, i , valread , sd;  
    int res;

	fd_set readfds;

	// инициализируем все client_socket [] в 0, чтобы не проверялось
    for (i = 0; i < max_clients; i++){  
        sock->fd_sock_cln[i] = 0;  
    } 

	while(true){
		// очищаем набор сокетов
	    FD_ZERO(&readfds);  

    	// добавить мастер сокет для установки
    	FD_SET(sock->fd_sock_srv, &readfds);  
    	max_sd = sock->fd_sock_srv;  

    	// добавляем дочерние сокеты для установки
    	for ( i = 0 ; i < max_clients ; i++) {  
    	    // дескриптор сокета
    	    sd = sock->fd_sock_cln[i]; 
        
        	// если действительный дескриптор сокета, то добавить в список чтения
        	if(sd > 0)  
    		    FD_SET( sd , &readfds);  
    	
    		// наибольший номер дескриптора файла, нужен для функции выбора
    		if(sd > max_sd)  
        		max_sd = sd;  
    	}  
    
    	// ожидание активности на одном из сокетов, время ожидания равно NULL,
    	// так что ждать бесконечно
    	activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
		if ((activity < 0) && (errno!=EINTR)) {  
    		printf("ERR: select()\n\r");  
		}
		


    	// Если что-то случилось с мастер-сокетом,
    	// тогда это входящее соединение
    	while(1){
    		res=pthread_mutex_trylock(&mut);
    		if(res==EBUSY)
    			break;
    		
    		if(res==0){
    			if (FD_ISSET(sock->fd_sock_srv, &readfds)) {    		
    			
    				if((new_socket = accept(sock->fd_sock_srv, (struct sockaddr *)&sock->cln, &addrlen)) == -1) {	
   						perror("ERR: accept():\n\r");
	   					exit(1);
   					}
   			
        			// сообщить пользователю номер сокета - используется в командах отправки и получения
        			printf("\n\r\tClient connected!");
    				printf("\n\r\tSocket fd: %d ,Client ip: %s ,Client port: %u",new_socket,inet_ntoa(sock->cln.sin_addr),ntohs(sock->cln.sin_port));

		        	// добавляем новый сокет в массив сокетов
    	    		for (i = 0; i < max_clients; i++) {  
        		    	// если позиция пуста
    	    	    	if( sock->fd_sock_cln[i] == 0 ) {  
	            	   		sock->fd_sock_cln[i] = new_socket;  
               				printf("\n\r\tAdding to list of sockets as %d" , i);    
               				break;  
	            		}  
    	    		}  
    		    	printf("\n\rcli> ");
	        		fflush(stdout);				        	
	        	}
	        	pthread_mutex_unlock(&mut);
	        	break;
    		}    			    		
    	}
    	

    	// иначе это какая-то операция ввода-вывода в другом сокете
    	while(1){
    		res=pthread_mutex_trylock(&mut);
    		if(res==EBUSY)
    			break;
    		
    		if(res==0){
        		for (i = 0; i < max_clients; i++)  {  
            		sd = sock->fd_sock_cln[i];  
            		if (FD_ISSET( sd , &readfds)) {  
                	// Проверить, было ли это для закрытия
                		if ((valread = read( sd , buffer, 1024)) == 0)  {  
                    	// Кто-то отключился, получить его данные и распечатать
                	
                    	getpeername(sd , (struct sockaddr*)&sock->cln,(socklen_t*)&addrlen);  
                    	printf("\n\r\tClient disconnected , ip %s , port %d " ,inet_ntoa(sock->cln.sin_addr) , ntohs(sock->cln.sin_port));  
	                    // Закрыть сокет и отметить как 0 в списке для повторного использования              
    	                if(sd = sock->fd_sock_cln[i]){
    	                	sock->curr_fd = 0;
    	                }
    	                
    	                close( sd );  
        	            sock->fd_sock_cln[i] = 0;
            	        printf("\n\rcli> ");  
                	    fflush(stdout);                	
	        	        }    	        
        	    	}  
        		}
        		pthread_mutex_unlock(&mut);
	        	break;
        	}
        }    
	}
}


int check_param_types(unsigned long num){
	if (num>65535){
		return 0;
	}else{
		return 1;
	}
}
