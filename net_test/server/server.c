#include "heads.h"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

int main(){
	

	char sym;					// посимвольная запись в буфер
	int input=1;				// для выхода из цикла ввода в буфер
	int i = 0;					// позиция сивола в буфере
	int iter = 0;				// итератор для клавиши TAB
	int len = 0;				// для хранения длины введенной строки
	int k = 0;					// для стирания символов
	char cmd[40];				// строка для введенных команд

	int on = 1;					// для setcoskopt
	
	sock_param sockets;
	
	pthread_t accpt_pt;
	//int fd_sock_srv;			// десриптор сокета
	

	//struct sockaddr_in srv,cln;	// структура сокета

    // обнуляем структуру и строку команд
	strncpy(cmd,"",40);
	memset(&sockets, 0, sizeof(sockets));

	// тип сокета создан
	sockets.srv.sin_family = AF_INET;  			// в структуре сервера говорим,о соединении типа IPv4
    sockets.srv.sin_addr.s_addr = INADDR_ANY; 	// ip адрес сокета
    sockets.srv.sin_port = htons(SRV_TCP_SPORT);// порт сокета 

    //получаем дискриптор сокета сервера
    if((sockets.fd_sock_srv = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	
		perror("ERR: socket():");exit(1);}

	printf("Server socket id: %d Server IP: %s Server port: %u\n",sockets.fd_sock_srv,inet_ntoa(sockets.srv.sin_addr),ntohs(sockets.srv.sin_port));

	// устанавливаем мастер сокет для разрешения нескольких соединений,
	if (setsockopt (sockets.fd_sock_srv, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof (on)) == -1) {
        perror("ERR: setsockopt_srv():");
        exit (1);}

    // привязываем сокет к локальному порту 60400
    // назначаем имя сокету
	if(bind(sockets.fd_sock_srv, (struct sockaddr *)&sockets.srv, sizeof(sockets.srv)) == -1) {		
		perror("ERR: bind():");close(sockets.fd_sock_srv);exit(1);}

	sockets.sock_len = sizeof(sockets.srv);

	// пытаемся указать максимум 3 ожидающих соединения для мастер сокета
	// готовность принимать пакеты и задаем размер очереди 
	if ((listen(sockets.fd_sock_srv, 3)) == -1) { 			
       	perror("ERR: listen():"); close(sockets.fd_sock_srv);exit(1);}

    // принять входящее соединение
    printf("Waiting connect from client...\n");
    
    /*if((fd_sock_cln = accept(fd_sock_srv, (struct sockaddr *)&cln, &sock_len)) == -1) {	
   		perror("ERR: line 43. accept():");close(fd_sock_srv);exit(1);}
	
	printf("Client connected!\n");
    printf("Client ip: %s Client port: %u \n",inet_ntoa(cln.sin_addr),ntohs(cln.sin_port));*/

	initscr();
	cbreak();		// не дожидаемся нажатия enter
	nodelay(stdscr,true);
    pthread_create(&accpt_pt, NULL,accept_new_client,(void*)&sockets);	
    pthread_detach(accpt_pt);

	printf("cli> ");
	int res = 5;
	// цикл ввода команд
	while(input!=0){	
		sym = getchar();
		switch(sym){
			// Обработка клавиши TAB
			case 0x09:
				++iter;
				if(iter==1){
					if(i<=2){i=iter_tab1(cmd,i);iter--;break;}
					if(i>=3){i=iter_tab2(cmd,i);break;}
				}

				if (iter==2){
					if((i-1)<=2){strncpy(cmd,"",40);printf("\n\r\trun\tset\tquit");printf("\n\rcli> ");
						i = 0;iter = 0;}

					if((i-1)>=3){strncpy(cmd,"",40);printf("\n\r\tseq\tping\tload");printf("\n\rcli> ");
						i = 0;iter = 0;}
				}
				break;
			// Обработка клавиши ENTER
			case 0x0D:	
				res = pthread_mutex_trylock(&mut); 	
				if(res == EBUSY)
					break;	
				if(res == 0){
					if(i == 1){cmd[i] = ' ';}
					if(quit_cmd(cmd,sockets.curr_fd)==0){input = 0;break;}
					set_cmd(cmd,sockets.curr_fd);								
					run_cmd(cmd,sockets.curr_fd);
					list_cmd(cmd,(void*)&sockets);
					clnselect_cmd(cmd,(void*)&sockets);
					printf("\n\rcli> ");
					strncpy(cmd,"",40); //очищаем буфеr команд
					iter = 0;
					i=0;	
					pthread_mutex_unlock(&mut);
				}
				break;
			// обработка любых клафиш
			default:
				
				cmd[i] = (char)sym;
				printf("%c",cmd[i]);
				i++;

				break;
		}
	}
}




