#include "heads.h"

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
    
    while ((tokens[cnt] = strtok_r(rest, " ", &rest))){
        cnt++;
   	}
   	if(strstr(tokens[0],"run")!=NULL){
   		if(cnt <= 1){
   				printf("\tERR: Invalid format of parametrs!");
   				printf("\n\r\tUse: run <command>");
   				return 0;
   		}
   		if(strstr(tokens[1],"seq")){settings.cmd = htons(1);
   			return 1;}
   		if(strstr(tokens[1],"ping")){settings.cmd = htons(2);
   			return 1;}
   		if(strstr(tokens[1],"load")){settings.cmd = htons(3);
   			return 1;}
   		if(strstr(tokens[1],"turnoff")){settings.cmd = htons(4);
   			return 1;}
   	}
   	if(strstr(tokens[0],"set")!=NULL){
   		if(cnt <= 1){
   			printf("\tERR: Invalid format of parametrs!");
   			printf("\n\r\tUse: set <command>");
   			return 0;}

    	if (strstr(tokens[1],"seq")!=NULL){
    		if(cnt < 6){
   			printf("\tERR: Invalid format of parametrs!");
   			printf("\n\r\tUse: set seq <type_proto> <len_ptk> <cnt_ptk> <time_test> <ip_cln>");
   			return 0;}	

    		printf("\tSetting for test sequence:\n\r");
    		settings.cmd = htons(5);
    		if(strstr(tokens[2],"-T")!=NULL){
    			settings.type_ptk = htons(1);
    			printf("\tType protoco:\tTCP\n\r");}
    		else if(strstr(tokens[2],"-U")!=NULL){
    			settings.type_ptk = htons(2);
    			printf("\tType protocol:\tUDP\n\r");}
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
    		settings.ip_cln = inet_addr(tokens[6]);
    		printf("\tСlient IP:\t%d\n\r",settings.ip_cln);
    		return 1;

    	}else if(strstr(tokens[1],"ping")!=NULL){
    		if(cnt < 4){
   				printf("\tInvalid format of parametrs!");
   				printf("\n\r\tUse: set ping <cnt_ptk> <time_test> <ip_cln>");
   				return 0;}
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
	unsigned long razn = 5000;
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
		
		recvfrom(_socket_desk, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&_sock, &sock_len);//) {

		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&udp_hdr_rcv,&rcv_ptk[20],8);
		//printf("ttl=%u\t",ip_hdr_rcv.TTL);
		
		if(ntohs(udp_hdr_rcv.DPORT)==SRV_TCP_SPORT){
			printf("\t[<-] Client RECIEV the command");flag = 1;}	

		if (flag==1){break;}
		
		time_check = get_time_ms();
   		if ( (time_check-time_ms_check)>razn ){break;}
   			
		
	}
	if(flag==0){printf("\t[ !] Client no answer!");}	
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





















/*case 0x08:
				len = strlen(cmd);
				getsyx(x,y);
				for(k=0;k<len;k++){
					printf("\b");
					delch();
				}
				
				
				i = i-1;
				cmd[i] = ' '; 
				setsyx(x,y-2);


				printf("%s",cmd);
				break;
			case KEY_BACKSPACE:
				//move(x, y);
				printf("\b");
				delch();// функция удаляет симовол на котором стоит коретка
				printf("\b");
				printf("%s",cmd);
				break;*/