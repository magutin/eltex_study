#include "heads.h"

/* Обработчик пришедшей команды */
int rcv_cmd_parser(){
	//стартуем тест порядка
	if(settings.cmd==1){
		printf("[<-] RECIEVE command \"start seq\"\n");
		usleep(1500);
		if(settings.type_ptk==2){pthread_create(&th_id, NULL,run_seq_udp,0);pthread_join(th_id, NULL);return sndseq_u;}
		if (settings.type_ptk==1){pthread_create(&th_id, NULL,run_seq_tcp,0);pthread_join(th_id, NULL);return sndseq_t;}
		
	}

	//стартуем тест пинг
	if(settings.cmd==2){
		printf("[<-] RECIEVE command \"start ping\"\n");
		usleep(1500);
		pthread_create(&th_id, NULL,run_ping,0);pthread_join(th_id, NULL);
		return sndping;
	}
			
	//стартуем тест load
	if(settings.cmd==3){
		printf("[<-] RECIEVE command \"start load\"\n");
		usleep(1500);
		if(settings.type_ptk==2){pthread_create(&th_id, NULL,run_load_udp,0);pthread_join(th_id, NULL);}
		//if(settings.type_ptk==1){pthread_create(&th_id, NULL,run_load_tcp,0);pthread_join(th_id, NULL);}
		return sndload;	
	}
			
	//завершаем работу
	if(settings.cmd==4){
		printf("[<-] RECIEVE command \"start turnoff\"\n");	
		return turnoff;
	}
			
	//задаем настройки для теста
	if(settings.cmd==5){
		printf("[<-] RECIEVE command \"settings for seq\"\n");
	}

	//задаем настройки для теста
	if(settings.cmd==6){
		printf("[<-] RECIEVE command \"settings for ping\"\n");
	}

	//задаем настройки для теста 
	if(settings.cmd==7){
		printf("[<-] RECIEVE command \"settings for load\"\n");
		
	}
	return 0;
}

/*  Конвертируем струтуру настроек после поллучения команды от сервера */
void ns_settings_to_s(){

	settings.cmd = ntohs(settings.cmd);
	settings.type_ptk = ntohs(settings.type_ptk);
	settings.len_ptk = ntohs(settings.len_ptk);
	settings.cnt_ptk = ntohs(settings.cnt_ptk);
	settings.time_test = ntohs(settings.time_test);
	settings.vlan = ntohs(settings.vlan);
	settings.ip_cln = ntohl(settings.ip_cln);
	//settings.ip_srv = ntohl(settings.ip_srv);
	//ip_n.s_addr = settings.ip_srv;
	//ip_s=inet_ntoa(ip_n);
	//printf("ip_s = %s\n",ip_s);
}

/* Расчет CRC */
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

/* Получить время в милисекундах*/
unsigned long get_time_ms(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_ms;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_ms = tv.tv_sec * 1000 + (tv.tv_usec / 1000); // сек->мсек + мкс->мсек
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_ms;
}

/* Получить время в секундах*/
unsigned long get_time_s(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_s;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_s = tv.tv_sec;
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_s;
}

void get_time_string(char* time_string,int size){
	strncpy(time_string,"",size);
	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_s;
	gettimeofday(&tv, NULL);	// Определение текущего времени и преобразование полученного значения в структуру типа tm. 
 	ptm = localtime(&tv.tv_sec);
 	strftime(time_string, size,"%H:%M:%S", ptm);// Форматирование значения даты и времени с точностью до секунды. 
}

void send_file(int _fd_sock,int type_test){
	struct agreement agree;
	memset(&agree,0,sizeof(agree));

	char msg[80];
	//strcat(agree.command,"start_ptk");
	//strncpy(agree.command,"start_ptk",9);
	//agree.size = 80;
	
	//if(send(_fd_sock,&agree,sizeof(agree),0)==-1){
	//	perror("ERR: line 58. send():");exit(1);}
	
	FILE *fname;
	char name[30];
	get_file_name(name,30,type_test);

    if ((fname = fopen(name, "r")) == NULL){
    	printf("ERR: fopen();\n");
    	exit(1);}
    sleep(2);

    while(fgets(msg, 80, fname) != NULL){
    	if(send(_fd_sock,msg,sizeof(msg),0)==-1){
		perror("ERR: line 58. send():");exit(1);}
		usleep(5000);
    }
    if(send(_fd_sock,"end ptk",80,0)==-1){
		perror("ERR: line 58. send():");exit(1);}
	
	fclose(fname);
	printf("[->] File send to server\"\n");
}


void get_file_name(char *name_str,int sz_str,int type_test){
	strncpy(name_str,"cln_files/",10);

	if(type_test==sndseq_u){
		strcat(name_str,"test_seq_udp.txt");
	}else if(type_test==sndseq_t){
		strcat(name_str,"test_seq_tcp.txt");
	}else if(type_test==sndping){
		strcat(name_str,"test_ping.txt");
	}else if(type_test==sndload){
		strcat(name_str,"test_load_udp.txt");
	}
}