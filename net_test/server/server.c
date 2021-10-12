#include "heads.h"
#include "sockets.h"


struct cmd_settings settings;


pthread_t tid[5];

int cmd_parser(char* _buf,int _len_buf);
void cmd_sender(int _socket_desk,struct sockaddr_in _sock);
unsigned long get_time_ms();
unsigned short in_cksum(unsigned short *ptr, int nbytes);

int main(){

	int fd_sock;							// десриптор сокета
	struct sockaddr_in sock;				// структура сокета
	char cmd[40];							// строка для введенных команд
	
	char sym;								// посимвольная запись в буфер
	int input=1;							// для выхода из цикла ввода в буфер
	int i = 0;								// позиция сивола в буфере
	int iter = 0;							// итератор для клавиши TAB
	int len = 0;							// для хранения длины введенной строки
	int k = 0;								// для стирания символов

	strncpy(cmd,"",40);
	memset(&sock, 0, sizeof(sock));					// обнуляем структуру

	sock.sin_family = AF_INET;  					// в структуре сервера говорим,о соединении типа IPv4
    sock.sin_addr.s_addr = htonl(INADDR_ANY); 				// все адреса локальной сети 0.0.0.0
    sock.sin_port = htons(SRV_TCP_SPORT); 			// конвертируем данные из узлового порядка расположения байтов в сетевой 

    // Сокет UDP будет посылать настройки для теста клиенту
    if((fd_sock = socket(AF_INET, SOCK_RAW,IPPROTO_UDP)) == -1) {	//получаем дискриптор сокета сервера
		perror("ERR: socket_srv():\n\r");
		exit(1);
	}
	int on = 1;
	if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) {
        perror("ERR: setsockopt_srv():");
        exit (1);
    }
	if(bind(fd_sock, (struct sockaddr *)&sock, sizeof(sock)) == -1) {		// назначаем имя сокету
		perror("ERR: bind_srv():\n\r");
		exit(1);
	}
	
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
							//strcpy(cmd,"");
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
							//strcpy(cmd,"");
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
							//strcpy(cmd,"");
							strcpy(&cmd[4],"ping ");
							i = 9;
							printf("%s",cmd);
						}
						if (strstr(&cmd[4],"l")!=NULL){
							len = strlen(cmd);
							for(k = 0;k<len;k++){
								printf("\b");	
								delch();
							}
							//strcpy(cmd,"");
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
					strncpy(cmd,"",40);
					printf("\n\r\trun");
					printf("\tset ");//seq <type_proto> <len_pkg> <cnt_pkg> <test_time> <ip_cln> ");
					/*	printf("\n\r\tset ping <cnt_pkg> <test_time> <ip_cln>");
					printf("\n\r\tset load <> <> <> <> <>");*/
					printf("\tquit");
					printf("\n\rcli> ");
					i = 0;
					iter = 0;	
				}
				if((i-1)>=3){
					strncpy(cmd,"",40);
					printf("\n\r\tseq");
					printf("\tping");//seq <type_proto> <len_pkg> <cnt_pkg> <test_time> <ip_cln> ");
					/*	printf("\n\r\tset ping <cnt_pkg> <test_time> <ip_cln>");
					printf("\n\r\tset load <> <> <> <> <>");*/
					printf("\tload");
					printf("\n\rcli> ");
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
							cmd_sender(fd_sock,sock);
							pthread_create(&tid[1], NULL,thread_udp_sock,0);	// поток для сокета UDP			
							pthread_join(tid[1], NULL);							// поток с ожиданием завершения
						}
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

int cmd_parser(char* buf,int len_buf){
	printf("\n\r");
	if(!buf){
		perror("ERR: no command\n");
		return 0;
	}

	char *tokens[40];
	char *rest = buf;
    int cnt = 1;
    
    tokens[0] = strtok_r(rest," ",&rest);
    
    while ((tokens[cnt] = strtok_r(rest, " ", &rest))){
        cnt++;
   	}
   	if(strstr(tokens[0],"run")!=NULL){
   		if(cnt <= 1){
   				printf("\tERR: Invalid format of parametrs!");
   				printf("\n\r\tUse: run <command>");
   				return 0;
   		}
   		if(strstr(tokens[1],"seq")){
   			settings.cmd = htons(1);
   			return 1;
   		}
   		if(strstr(tokens[1],"ping")){
   			settings.cmd = htons(2);
   			return 1;
   		}
   		if(strstr(tokens[1],"load")){
   			settings.cmd = htons(3);
   			return 1;
   		}
   		if(strstr(tokens[1],"turnoff")){
   			settings.cmd = htons(4);
   			return 1;
   		}
   	}
   	if(strstr(tokens[0],"set")!=NULL){
   		if(cnt <= 1){
   				printf("\tERR: Invalid format of parametrs!");
   				printf("\n\r\tUse: set <command>");
   				return 0;
   		}

    	if (strstr(tokens[1],"seq")!=NULL){
    		if(cnt < 6){
   				printf("\tERR: Invalid format of parametrs!");
   				printf("\n\r\tUse: set seq <type_proto> <len_ptk> <cnt_ptk> <time_test> <ip_cln>");
   				return 0;
   			}	

    		printf("\tSetting for test sequence:\n\r");
    		settings.cmd = htons(5);
    		if(strstr(tokens[2],"-T")!=NULL){
    			settings.type_ptk = htons(1);
    			printf("\tType protoco:\tTCP\n\r");
    		}
    		else if(strstr(tokens[2],"-U")!=NULL){
    			settings.type_ptk = htons(2);
    			printf("\tType protocol:\tUDP\n\r");
    		}
    		else if(strstr(tokens[2],"-I")!=NULL){
    			settings.type_ptk = htons(3);
    			printf("\tType protocol:\tICMP\n\r");
    		}else{printf("\t\n\rInvalid type of protocol!");}

    		settings.len_ptk = htons(atoi(tokens[3]));
    		printf("\tLen package:\t%u\n\r",settings.len_ptk);
    		settings.cnt_ptk = htons(atoi(tokens[4]));
    		printf("\tCount package:\t%u\n\r",settings.cnt_ptk);
    		settings.time_test = htons(atoi(tokens[5]));
    		printf("\tTesting time:\t%u\n\r",settings.time_test);
    		settings.ip_cln = htonl(inet_addr(tokens[6]));
    		printf("\tСlient IP:\t%d\n\r",settings.ip_cln);
    		return 1;

    	}else if(strstr(tokens[1],"ping")!=NULL){
    		if(cnt < 4){
   				printf("\tInvalid format of parametrs!");
   				printf("\n\r\tUse: set ping <cnt_ptk> <time_test> <ip_cln>");
   				return 0;
   			}
    		printf("\tSetting for test ping:\n\r");
    		settings.cmd = htons(6);

    		settings.cnt_ptk = htons(atoi(tokens[2]));
    		printf("\tCount package:\t%d\n\r",settings.cnt_ptk);
    		settings.time_test = htons(atoi(tokens[3]));
    		printf("\tTesting time:\t%d\n\r",settings.time_test);
    		settings.ip_cln = htonl(inet_addr(tokens[4]));
    		printf("\tСlient IP:\t%s\n\r",tokens[4]);
    		return 1;

    	}else if(strstr(tokens[1],"load")!=NULL){
    		printf("\tload");
    		return 1;
    	}else if(strstr(tokens[1],"seq")==NULL && strstr(tokens[1],"load")==NULL && strstr(tokens[1],"ping")==NULL){
    		printf("\t\rERR:Unknown command");
    		return 0;
    	}
	} 
}

void cmd_sender(int _socket_desk,struct sockaddr_in _sock){
	char rcv_ptk[64];
	char snd_ptk[64];
	unsigned short cnt_attempt = 1; // количество попыток отправки
	unsigned short flag=0;
	unsigned long razn = 1000;
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
    ip->saddr = 0;						//inet_addr("192.168.1.4");
    ip->daddr = settings.ip_cln;
    //ip->check = in_cksum ((u16 *) ip, sizeof (struct iphdr));
    memcpy(&snd_ptk[0],ip,20);			// копирую в буфер заполненную структуру IP

    // udp заголовок в буфере
    struct udphdr *udp = (struct udphdr *) (snd_ptk + sizeof (struct iphdr));
    udp->source = htons(SRV_TCP_SPORT);
    udp->dest = htons(SRV_TCP_DPORT);
    udp->len = htons(44);
    udp->check = 0;
    memcpy(&snd_ptk[20],udp,8);			// копирую в буфер заполненную структуру UDP
    memcpy(&snd_ptk[28],&settings,16);

    //получаем длину сокета
	socklen_t sock_len = sizeof(_sock);

	udp->check = in_cksum((unsigned short *)&snd_ptk[20], 44);
	unsigned long check = 0;
	
	printf("\t[->] Command SEND to client (pckg cnt %d)\n\r",cnt_attempt);
	time_ms_check = get_time_ms();
    if(sendto(_socket_desk, snd_ptk, sizeof(snd_ptk), 0, (struct sockaddr*)&_sock, sock_len)==-1) {
		perror("\tERR: Command 'set' not send. send():");
		close(_socket_desk);
		endwin();
		exit(1);
	}
	
    while(1){
		if(recvfrom(_socket_desk, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&_sock, &sock_len)==-1) {
			perror("Recvfrom:\n\r");
			close(_socket_desk);
			endwin();	
			exit(1);
		}
		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&udp_hdr_rcv,&rcv_ptk[20],8);
		//printf("ttl=%u\t",ip_hdr_rcv.TTL);
		
		if(ntohs(udp_hdr_rcv.DPORT)==SRV_TCP_SPORT){
			printf("\t[<-] Client RECIEV the command\n\r");
			flag = 1;
		}	

		if(flag==0){
		//	printf("\t[ !] Waiting...\n\r");
			++cnt_attempt;				
		}		
		if (flag==1){
			break;
		}
	}
	if(flag==0){
		printf("\t[ !] Client no answer!\n\r");
	}	
}

unsigned long get_time_ms(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_ms;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_ms = tv.tv_sec * 1000 + (tv.tv_usec / 1000); // сек->мсек + мкс->мсек
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_ms;
}

unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return (answer);
}


