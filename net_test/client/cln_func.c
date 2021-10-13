#include "heads.h"

/* Обработчик пришедшей команды */
int rcv_cmd_parser(){
	//стартуем тест порядка
	if(settings.cmd==1){
		printf("[->] RECIEVE command \"start seq\"\n");
		usleep(1500);
		if(settings.type_ptk==2){pthread_create(&th_id, NULL,run_seq_udp,0);pthread_join(th_id, NULL);}
		if (settings.type_ptk==1){pthread_create(&th_id, NULL,run_seq_tcp,0);pthread_join(th_id, NULL);}
	}

	//стартуем тест пинг
	if(settings.cmd==2){
		printf("[->] RECIEVE command \"start ping\"\n");
		usleep(1500);
		pthread_create(&th_id, NULL,run_ping,0);pthread_join(th_id, NULL);
	}
			
	//стартуем тест load
	if(settings.cmd==3){
		printf("[->] RECIEVE command \"start load\"\n");
	}
			
	//завершаем работу
	if(settings.cmd==4){
		printf("[->] RECIEVE command \"start turnoff\"\n");	
		return 1;
	}
			
	//задаем настройки для теста
	if(settings.cmd==5){
		printf("[->] RECIEVE command \"settings for seq\"\n");
	}

	//задаем настройки для теста
	if(settings.cmd==6){
		printf("[->] RECIEVE command \"settings for ping\"\n");
	}

	//задаем настройки для теста 
	if(settings.cmd==7){
		printf("[->] RECIEVE command \"settings for load\"\n");
		
	}
	return 0;
}

/*  Конвертируем струтуру настроек после поллучения команды от сервера */
void ns_settings_to_s(){

	struct in_addr ip_n;

	settings.cmd = ntohs(settings.cmd);
	settings.type_ptk = ntohs(settings.type_ptk);
	settings.len_ptk = ntohs(settings.len_ptk);
	settings.cnt_ptk = ntohs(settings.cnt_ptk);
	settings.time_test = ntohs(settings.time_test);
	settings.vlan = ntohs(settings.vlan);
	settings.ip_cln = ntohl(settings.ip_cln);
	ip_n.s_addr = settings.ip_cln;
	ip_s=inet_ntoa(ip_n);
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

/* Получить время в секундах*/
unsigned long get_time_ms(){

	struct timeval tv;			// структура времени
	struct tm* ptm;	
	unsigned long _time_ms;

	gettimeofday(&tv, NULL);	// Определение текущего времени
	_time_ms = tv.tv_sec * 1000 + (tv.tv_usec / 1000); // сек->мсек + мкс->мсек
	//printf("[%ld]\n",_time_ms);	// милисекунды

	return _time_ms;
}