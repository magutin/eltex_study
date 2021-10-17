#include "heads.h"



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

	if ( fcntl( fd_sock, F_SETFL, O_NONBLOCK, on ) == -1 ){
        printf( "failed to set non-blocking socket\n" );
        return false;}
	
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
				if(quit_cmd(cmd,fd_sock)==0){input = 0;break;}
				set_cmd(cmd,fd_sock,sock);								
				run_cmd(cmd,fd_sock,sock);
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






