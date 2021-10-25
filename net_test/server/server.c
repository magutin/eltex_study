#include "heads.h"



int main(){
	char sym;								// посимвольная запись в буфер
	int input=1;							// для выхода из цикла ввода в буфер
	int i = 0;								// позиция сивола в буфере
	int iter = 0;							// итератор для клавиши TAB
	int len = 0;							// для хранения длины введенной строки
	int k = 0;								// для стирания символов

	int fd_sock_srv;						// десриптор сокета
	int fd_sock_cln;						// десриптор сокета
	struct sockaddr_in srv,cln;				// структура сокета
	char cmd[40];							// строка для введенных команд

	strncpy(cmd,"",40);
	memset(&srv, 0, sizeof(srv));					// обнуляем структуру

	srv.sin_family = AF_INET;  					// в структуре сервера говорим,о соединении типа IPv4
    srv.sin_addr.s_addr = INADDR_ANY; 					// ip адрес сокета
    srv.sin_port = htons(SRV_TCP_SPORT); 			// порт сокета 

    if((fd_sock_srv = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	//получаем дискриптор сокета сервера
		perror("ERR: line 24, socket():");exit(1);}

	printf("Server socket id: %d Server IP: %s Server port: %u\n",fd_sock_srv,inet_ntoa(srv.sin_addr),ntohs(srv.sin_port));

	int on = 1;
	if (setsockopt (fd_sock_srv, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof (on)) == -1) {
        perror("ERR: setsockopt_srv():");
        exit (1);}

	if(bind(fd_sock_srv, (struct sockaddr *)&srv, sizeof(srv)) == -1) {		// назначаем имя сокету
		perror("ERR: line 29. bind():");close(fd_sock_srv);exit(1);}

	socklen_t sock_len = sizeof(srv);

	if ((listen(fd_sock_srv, 1)) == -1) { 			// готовность принимать пакеты и задаем размер очереди 
       	perror("ERR: line 36. listen():"); close(fd_sock_srv);exit(1);}

    printf("Waiting connect from client...\n");
    if((fd_sock_cln = accept(fd_sock_srv, (struct sockaddr *)&cln, &sock_len)) == -1) {		// принять соединение на сокете
   		perror("ERR: line 43. accept():");close(fd_sock_srv);exit(1);}
	printf("Client connected!\n");
    printf("Client ip: %s Client port: %u \n",inet_ntoa(cln.sin_addr),ntohs(cln.sin_port));

	initscr();
	cbreak();						// не дожидаемся нажатия enter
	printf("cli> ");

	while(input!=0){	// цикл ввода команд
		sym = getchar();//getch();
		switch(sym){
			case 0x09:// Обработка клавиши TAB
				++iter;
				if(iter==1){
					if(i<=2){i=iter_tab1(cmd,i);iter--;break;}
					if(i>=3){i=iter_tab2(cmd,i);iter--;break;}
				}

				if (iter==2){
					if((i-1)<=2){strncpy(cmd,"",40);printf("\n\r\trun\tset\tquit");printf("\n\rcli> ");
						i = 0;iter = 0;}

					if((i-1)>=3){strncpy(cmd,"",40);printf("\n\r\tseq\tping\tload");printf("\n\rcli> ");
						i = 0;iter = 0;}
				}	
				break;

			case 0x0D:		// Обработка клавиши ENTER
				if(i == 1){cmd[i] = ' ';}
				if(quit_cmd(cmd,fd_sock_cln)==0){input = 0;break;}
				set_cmd(cmd,fd_sock_cln);								
				run_cmd(cmd,fd_sock_cln);
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






