#include "heads.h"

/* Обработчик пришедшей команды */
int rcv_cmd_parser()
{
    //стартуем тест порядка
    if (settings.cmd == 1) {
	printf("[<-] RECIEVE command \"start seq\"\n");
	//usleep(1500);
	if (settings.type_ptk == 2) {
	    debug_printf("pthread_create(): File cln_func.c,line 12. Creating thread and new socket for test seq\n");
	    pthread_create(&th_id, NULL, run_seq_udp, 0);
	    pthread_join(th_id, NULL);
	    debug_printf("pthread_create(): File cln_func.c,line 12. Thread and new socket for test seq was closed\n");
	    return sndseq_u;
	}
	if (settings.type_ptk == 1) {
	    debug_printf("pthread_create(): File cln_func.c,line 19. Creating thread and new socket for test seq\n");
	    pthread_create(&th_id, NULL, run_seq_tcp, 0);
	    pthread_join(th_id, NULL);
	    debug_printf("pthread_create(): File cln_func.c,line 19. Thread and new socket for test seq was closed\n");
	    return sndseq_t;
	}

    }
    //стартуем тест пинг
    if (settings.cmd == 2) {
	printf("[<-] RECIEVE command \"start ping\"\n");
	//usleep(1500);
	debug_printf
	    ("pthread_create(): File cln_func.c,line 32. Creating thread and new socket for test ping\n");
	pthread_create(&th_id, NULL, run_ping, 0);
	pthread_join(th_id, NULL);
	debug_printf
	    ("pthread_create(): File cln_func.c,line 32. Thread and new socket for test ping was closed\n");
	return sndping;
    }
    //стартуем тест load
    if (settings.cmd == 3) {
	printf("[<-] RECIEVE command \"start load\"\n");
	usleep(1500);
	if (settings.type_ptk == 2) {
	    debug_printf
		("pthread_create(): File cln_func.c,line 45. Creating thread and new socket for test load\n");
	    pthread_create(&th_id, NULL, run_load_udp, 0);
	    pthread_join(th_id, NULL);
	    debug_printf
		("pthread_create(): File cln_func.c,line 45. Thread and new socket for test load was closed\n");
	}
	//if(settings.type_ptk==1){pthread_create(&th_id, NULL,run_load_tcp,0);pthread_join(th_id, NULL);}
	return sndload;
    }
    //завершаем работу
    if (settings.cmd == 4) {
	printf("[<-] RECIEVE command \"start turnoff\"\n");
	debug_printf
	    ("rcv_cmd_parser(): Settings: code cmd %u,type ptk %u,len ptk %u,cnt ptk %u, time test %u.\n",
	     settings.cmd, settings.type_ptk, settings.len_ptk,
	     settings.cnt_ptk, settings.time_test);
	debug_printf
	    ("rcv_cmd_parser(): File cln_func.c,line 54. Client closed\n");
	return turnoff;
    }
    //задаем настройки для теста
    if (settings.cmd == 5) {
	printf("[<-] RECIEVE command \"settings for seq\"\n");
	debug_printf
	    ("rcv_cmd_parser(): Settings: code cmd %u,type ptk %u,len ptk %u,cnt ptk %u, time test %u.\n",
	     settings.cmd, settings.type_ptk, settings.len_ptk,
	     settings.cnt_ptk, settings.time_test);
    }
    //задаем настройки для теста
    if (settings.cmd == 6) {
	printf("[<-] RECIEVE command \"settings for ping\"\n");
	debug_printf
	    ("rcv_cmd_parser(): Settings: code cmd %u, cnt ptk %u, time test %u.\n",
	     settings.cmd, settings.cnt_ptk, settings.time_test);
    }
    //задаем настройки для теста 
    if (settings.cmd == 7) {
	printf("[<-] RECIEVE command \"settings for load\"\n");
	debug_printf
	    ("rcv_cmd_parser(): Settings: code cmd %u,type ptk %u,len ptk %u.\n",
	     settings.cmd, settings.type_ptk, settings.len_ptk);

    }
    return 0;
}

/*  Конвертируем струтуру настроек после поллучения команды от сервера */
void ns_settings_to_s()
{
    debug_printf
	("ns_settings_to_s(): File cln_func.c,line 91. Token settings.\n");
    settings.cmd = ntohs(settings.cmd);
    settings.type_ptk = ntohs(settings.type_ptk);
    settings.len_ptk = ntohs(settings.len_ptk);
    settings.cnt_ptk = ntohs(settings.cnt_ptk);
    settings.time_test = ntohs(settings.time_test);
    settings.vlan = ntohs(settings.vlan);
    settings.ip_cln = ntohl(settings.ip_cln);
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
unsigned long get_time_ms()
{

    struct timeval tv;		// структура времени
    struct tm *ptm;
    unsigned long _time_ms;

    gettimeofday(&tv, NULL);	// Определение текущего времени
    _time_ms = tv.tv_sec * 1000 + (tv.tv_usec / 1000);	// сек->мсек + мкс->мсек
    //printf("[%ld]\n",_time_ms);   // милисекунды

    return _time_ms;
}

/* Получить время в секундах*/
unsigned long get_time_s()
{

    struct timeval tv;		// структура времени
    struct tm *ptm;
    unsigned long _time_s;

    gettimeofday(&tv, NULL);	// Определение текущего времени
    _time_s = tv.tv_sec;
    //printf("[%ld]\n",_time_ms);   // милисекунды

    return _time_s;
}

void get_time_string(char *time_string, int size)
{
    strncpy(time_string, "", size);
    struct timeval tv;		// структура времени
    struct tm *ptm;
    unsigned long _time_s;
    gettimeofday(&tv, NULL);	// Определение текущего времени и преобразование полученного значения в структуру типа tm. 
    ptm = localtime(&tv.tv_sec);
    strftime(time_string, size, "%H:%M:%S", ptm);	// Форматирование значения даты и времени с точностью до секунды. 
}

void send_file(int _fd_sock, int type_test)
{
    debug_printf
	("send_file(): File cln_func.c,line 171. Send file to server.\n");
    struct agreement agree;
    memset(&agree, 0, sizeof(agree));

    char msg[80];

    FILE *fname;
    char name[30];
    get_file_name(name, 30, type_test);

    if ((fname = fopen(name, "r")) == NULL) {
	printf("ERR: fopen(). file cln_func.c, func send_file().\n");
	exit(1);
    }
    sleep(2);

    while (fgets(msg, 80, fname) != NULL) {
	if (send(_fd_sock, msg, sizeof(msg), 0) == -1) {
	    perror("ERR: send(). file cln_func.c func send_file().");
	    exit(1);
	}
    }
    if (send(_fd_sock, "end ptk", 80, 0) == -1) {
	perror
	    ("ERR: send(). file cln_func.c func send_file(),send msg \"end ptk\".");
	exit(1);
    }

    fclose(fname);
    printf("[->] File was send to server\"\n");
    memset(&settings, 0, sizeof(struct cmd_settings));
}

int create_file(char *name_str)
{
    debug_printf
	("create_file(): File cln_func.c,line 207. Creating file for tests result\n");
    if (!name_str)
	return 1;

    int fdesc = open(name_str, O_CREAT, S_IRWXU);
    if (fdesc <= 0) {
	printf
	    ("ERR: Cannot create file [%s].file cln_func.c, func create_file().\n",
	     name_str);
	close(fdesc);
	return 1;
    } else {
	printf("[ !] File %s created\n", name_str);
	close(fdesc);
	return 0;
    }
}

void get_file_name(char *name_str, int sz_str, int type_test)
{
    debug_printf
	("get_file_name(): File cln_func.c,line 228. Getting file name\n");
    strncpy(name_str, "", sz_str);
    strncpy(name_str, "cln_files/", 10);
    char spid[12];
    sprintf(spid, "%d", getpid());

    if (type_test == sndseq_u) {
	strcat(name_str, "test_seq_udp");
	strcat(name_str, spid);
	strcat(name_str, ".txt");
    } else if (type_test == sndseq_t) {
	strcat(name_str, "test_seq_tcp");
	strcat(name_str, spid);
	strcat(name_str, ".txt");
    } else if (type_test == sndping) {
	strcat(name_str, "test_ping");
	strcat(name_str, spid);
	strcat(name_str, ".txt");
    } else if (type_test == sndload) {
	strcat(name_str, "test_load_udp");
	strcat(name_str, spid);
	strcat(name_str, ".txt");
    }
}
