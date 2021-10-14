#include "heads.h"
#include "sockets.h"


int main(int argc,char** argv){

	if (argc < 2) {
		printf("Invalid args!\n");
        printf("Use: %s <destination IP>\n", argv[0]);
        exit(0);
    }

	int fd_sock;							// десриптор сокета
	struct sockaddr_in sock;				// структура сокета
	char cmd[40];							// строка для введенных команд

	unsigned long saddr = inet_addr(argv[1]);
	
	char sym;								// посимвольная запись в буфер
	int input=1;							// для выхода из цикла ввода в буфер
	int i = 0;								// позиция сивола в буфере
	int iter = 0;							// итератор для клавиши TAB
	int len = 0;							// для хранения длины введенной строки
	int k = 0;								// для стирания символов

	strncpy(cmd,"",40);
	memset(&sock, 0, sizeof(sock));					// обнуляем структуру

	sock.sin_family = AF_INET;  					// в структуре сервера говорим,о соединении типа IPv4
    sock.sin_addr.s_addr = saddr; 					// ip адрес сокета
    sock.sin_port = htons(SRV_TCP_SPORT); 			// порт сокета 

    // Сокет UDP будет посылать настройки для теста клиенту
    if((fd_sock = socket(AF_INET, SOCK_RAW,IPPROTO_UDP)) == -1) {	//получаем дискриптор сокета сервера
		perror("ERR: socket_srv():\n\r");
		exit(1);}

	int on = 1;
	if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) {
        perror("ERR: setsockopt_srv():");
        exit (1);}

	if(bind(fd_sock, (struct sockaddr *)&sock, sizeof(sock)) == -1) {		// назначаем имя сокету
		perror("ERR: bind_srv():\n\r");
		exit(1);}

	if ( fcntl( fd_sock, F_SETFL, SOCK_NONBLOCK, on ) == -1 ){
        printf( "failed to set non-blocking socket\n" );
        return false;}
	
	initscr();
	cbreak();						// не дожидаемся нажатия enter
	
	//cbreak();
	//int x,y;
	printf("cli> ");

	while(input!=0){	// цикл ввода команд
		sym = getchar();//getch();
		switch(sym){
			case 0x09:// Обработка клавиши TAB
				++iter;
				if(iter==1){
					if(i <= 2){
						if (cmd[0] == 'q'){
							len = strlen(cmd);
							for(k = 0;k<len;k++){
								printf("\b");	
								delch();
							}
							strncpy(cmd,"",40);
							strcat(cmd,"quit ");
							i = 5;
							iter--;
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
							iter--;
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
							iter--;
							printf("%s",cmd);
						}	
						break;			
					}
					if(i>=3){
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
					break;
					iter--;
				}
				
			}
			if (iter==2){
				if((i-1)<=2){
					strncpy(cmd,"",40);printf("\n\r\trun\tset\tquit");printf("\n\rcli> ");
					i = 0;
					iter = 0;	
				}
				if((i-1)>=3){
					strncpy(cmd,"",40);printf("\n\r\tseq\tping\tload");printf("\n\rcli> ");
					i = 0;
					iter = 0;	
				}
			}	
			break;

			case 0x0D:		// Обработка клавиши ENTER
				if(i == 1){
					cmd[i] = ' ';	
				}

				if (strstr(cmd,"quit") != NULL){
					input=0;
					strncpy(cmd,"",40);
					close(fd_sock);
					printf("\n");
					endwin();
					return 0;
				}

				if (/*(*/strstr(cmd,"set") != NULL) /*|| (strncmp(cmd,"s ",2) == 0))*/{			// отправка команды клиенту на установку настроек
					// recv set cmd param
					if(cmd_parser(cmd,strlen(cmd))==1){
						cmd_sender(fd_sock,sock);
					}							
					strncpy(cmd,"",40);	
				}

				if (strstr(cmd,"run") != NULL){
					// recv set cmd param
					if(cmd_parser(cmd,strlen(cmd))==1){
						
						if(ntohs(settings.type_ptk)==2 && ntohs(settings.cmd)==1){
							pthread_create(&tid[1], NULL,thread_udp_sock,0);	// поток для сокета UDP			
							cmd_sender(fd_sock,sock);
							pthread_join(tid[1], NULL);							// поток с ожиданием завершения
						}
						if(ntohs(settings.type_ptk)==1 && ntohs(settings.cmd)==1){
							pthread_create(&tid[1], NULL,thread_tcp_sock,0);	// поток для сокета TCP			
							cmd_sender(fd_sock,sock);
							pthread_join(tid[1], NULL);							// поток с ожиданием завершения
						}
						if(ntohs(settings.cmd)==4){cmd_sender(fd_sock,sock);}
						if(ntohs(settings.cmd)==2){cmd_sender(fd_sock,sock);}
					}
					strncpy(cmd,"",40);	
				}
				printf("\n\rcli> ");
				strncpy(cmd,"",40); //очищаем буфеr команд
				iter = 0;
				i=0;
				break;
			default:	// обработка любых клафиш
				cmd[i] = (char)sym;
				printf("%c",cmd[i]);
				i++;

				break;
		}
	}
}






