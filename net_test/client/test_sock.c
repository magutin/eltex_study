#include "heads.h"

/*
	Сокет создается при выборе теста пинг.
	Вообще он может пингануть любой айпишник,я проверял.
	Но в тз вроде как нужно пинговать именно айпи сервера.
	Сокет для типа протокола ICMP.
*/
void *run_ping()
{

    char snd_ptk[64];		// буфер отправки
    char rcv_ptk[64];		// буфер приема
    FILE *fping;
    char name[40];		// = "cln_files/test_ping.txt";
    get_file_name(name, 40, sndping);

    if (create_file(name) == 1)
	return 0;

    if ((fping = fopen(name, "w")) == NULL) {
	printf("ERR: fopen();\n");
	return 0;
    }
    char time_string[15];	// строка для хранения времени

    int fd_sock_p;		// десриптор сокета
    unsigned long time_ms = 0;	// хранение времени в мс
    unsigned long time_ms_check = 0;	// хранение времени в мс
    int packet_size;		// общий размер пакета
    socklen_t sock_len;		// размер сокета

    struct sockaddr_in sck_adrr_p;
    struct sockaddr_in sck_rcv;
    struct icmp_h icmp_hdr;
    struct icmp_h icmp_hdr_rcv;
    struct ip_h ip_hdr;
    struct ip_h ip_hdr_rcv;
    struct in_addr ip_n;
    unsigned long d_ip;		// ip назначения

    char *d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);	// конвертируем ip назначения

    //обнуление буферов и полей структуры
    strncpy(snd_ptk, "", 64);
    strncpy(rcv_ptk, "", 64);
    memset(&sck_adrr_p, 0, sizeof(sck_adrr_p));

    // Заполняю структуру заголовка IP    
    ip_hdr.VER = 0x4;		// версия IPv4 - 4
    ip_hdr.HELEN = 0x5;		// длина заголовка IP в словах ,мин 5(5*4 байта = 20 байт)
    ip_hdr.SERV_TYPE = 0x00;	// 
    ip_hdr.TOTL_LEN = 0x40;	//64;                           // общая длина пакета,в нашем случае 108 байт(ip+udp+data)
    ip_hdr.IDENTIF = 0x00;	// ID пакета                      в нашем случае 0
    ip_hdr.FLAGS = 0x00;	// Флаги                       в нашем случе 0
    ip_hdr.F_OFFST = 0x00;	// Смещение фрагмента   в нашем случае 0   
    ip_hdr.TTL = 0x40;		// Время жизни                        
    ip_hdr.PROTOCOL = 0x01;	// Тип протокола            в нашем случае следующий заголовок от UDP -> 17
    ip_hdr.H_CSUM = 0x00;	// контрольная сумма    линукс игнорит это поле ,поэтому 0
    ip_hdr.S_IP = 0x00;		// IP отправителя            Linux заполнит сам
    ip_hdr.D_IP = d_ip;		// IP получателя              

    // Заполняю структуру заголовка icmp
    icmp_hdr.TYPE = 0x8;
    icmp_hdr.CODE = 0x0;
    icmp_hdr.CRC = 0x0;
    icmp_hdr.ID = 0x1F4;
    icmp_hdr.SEQ_NUM = 0x0;

    memcpy(&snd_ptk[0], &ip_hdr, 20);	// копирую в буфер заполненную структуру IP
    memcpy(&snd_ptk[20], &icmp_hdr, 8);	// копирую в буфер заполненную структуру ICMP

    //создаем сокет 
    if ((fd_sock_p = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
	perror("ERR: line 59, socket():");
	exit(1);
    }

    if (setsockopt(fd_sock_p, IPPROTO_IP, IP_HDRINCL, &(int) { 1 },
		   sizeof(int)) == -1) {
	perror("setsockopt");
	exit(1);
    }

    printf("[ !] START TEST\n");
    fprintf(fping, "[ !] START TEST\n");
    get_time_string(time_string, sizeof(time_string));
    fprintf(fping, "  Time start test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд. 

    // заполняем структуру сервера
    sck_adrr_p.sin_family = AF_INET;	// константа, определяющая коммуникационный домен "Интернет           
    sck_adrr_p.sin_addr.s_addr = d_ip;	// IP-адрес - это структура
    //получаем длину сокета
    sock_len = sizeof(sck_adrr_p);
    for (int i = 0; i < settings.cnt_ptk; i++) {



	time_ms = get_time_ms();
	fprintf(fping, "  [%d]", i);
	snd_ptk[28] = (char) (time_ms);
	snd_ptk[29] = (char) (time_ms >> 8);
	snd_ptk[30] = (char) (time_ms >> 16);
	snd_ptk[31] = (char) (time_ms >> 24);
	snd_ptk[32] = (char) (time_ms >> 32);
	snd_ptk[33] = (char) (time_ms >> 40);
	snd_ptk[34] = (char) (time_ms >> 48);
	snd_ptk[35] = (char) (time_ms >> 56);

	icmp_hdr.TYPE = 0x8;
	icmp_hdr.CODE = 0x0;
	icmp_hdr.CRC = 0x0;
	icmp_hdr.ID = 0x1F4;
	icmp_hdr.SEQ_NUM = 0x0;
	icmp_hdr.SEQ_NUM = (char) (i);

	printf("  ping %s  %ld(%ld) bytes of data\tpackage num %u\n",
	       d_ip_str, sizeof(snd_ptk) - 28, sizeof(snd_ptk),
	       icmp_hdr.SEQ_NUM);
	memcpy(&snd_ptk[20], &icmp_hdr, 8);
	fprintf(fping,
		"\tping %s  %ld(%ld) bytes of data\tpackage num %u\n",
		d_ip_str, sizeof(snd_ptk) - 28, sizeof(snd_ptk),
		icmp_hdr.SEQ_NUM);

	//printf("ismp_seq=%u\n",icmp_hdr.SEQ_NUM);

	icmp_hdr.CRC = in_cksum((unsigned short *) &snd_ptk[20], 44);
	memcpy(&snd_ptk[20], &icmp_hdr, 8);	// копирую в буфер заполненную структуру ICMP

	if (sendto
	    (fd_sock_p, snd_ptk, sizeof(snd_ptk), 0,
	     (struct sockaddr *) &sck_adrr_p, sock_len) == -1) {
	    perror("ERR: File test_sock.c, line 132, socket():");
	    exit(1);
	}

	while (1) {
	    if (recvfrom
		(fd_sock_p, rcv_ptk, sizeof(rcv_ptk), 0,
		 (struct sockaddr *) &sck_rcv, &sock_len) == -1) {
		perror("ERR:File test_sock.c, line 140, socket():");
		exit(1);
	    }
	    time_ms_check = get_time_ms();
	    memcpy(&ip_hdr_rcv, &rcv_ptk[0], 20);
	    memcpy(&icmp_hdr_rcv, &rcv_ptk[20], 8);
	    if (icmp_hdr_rcv.TYPE == 0 && icmp_hdr_rcv.TYPE == 0) {
		int size = sizeof(rcv_ptk);
		printf
		    ("  %d bytes from: %s icmp_seq=%u ttl=%u time=%ld ms\n",
		     size, inet_ntoa(ip_n), icmp_hdr_rcv.SEQ_NUM,
		     ip_hdr_rcv.TTL, time_ms_check - time_ms);
		fprintf(fping,
			"\t%d bytes from: %s icmp_seq=%u ttl=%u time=%ld ms\n",
			size, inet_ntoa(ip_n), icmp_hdr_rcv.SEQ_NUM,
			ip_hdr_rcv.TTL, time_ms_check - time_ms);
		break;
	    }

	    if (icmp_hdr_rcv.TYPE == 3) {
		printf
		    ("  Bad icmp answer! icmp_seq=%u type=%u code=%d time=%ld ms\n",
		     icmp_hdr_rcv.SEQ_NUM, icmp_hdr_rcv.TYPE,
		     icmp_hdr_rcv.CODE, time_ms_check - time_ms);
		fprintf(fping,
			"   \tBad icmp answer! icmp_seq=%u type=%u code=%d time=%ld ms\n",
			icmp_hdr_rcv.SEQ_NUM, icmp_hdr_rcv.TYPE,
			icmp_hdr_rcv.CODE, time_ms_check - time_ms);
	    }

	}

    }
    get_time_string(time_string, sizeof(time_string));
    fprintf(fping, "  Time end test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд. 
    printf("[ !] END TEST\n");
    fprintf(fping, "[ !] END TEST\n");
    fclose(fping);
    close(fd_sock_p);

}

/*
	Сокет создается при выборе теста последовательной передачи пакетов.
	Не понял что конкретно было нужно,то ли имитировать нарушение последовательности пакетов,то ли 
	проверить канал на нарушение последовательности.
	Поэтому сокет реализован как отпрвка-прием,последовательность пакетов.
	Сокет для типа протокола TCP
*/
void *run_seq_tcp()
{

#define FREE_MEM free(snd_ptk);free(rcv_ptk)

    unsigned long d_ip;		// ip назначения
    int count_packg;		// количество пакетов
    int payload_len = settings.len_ptk;	// размер блока данных

    int sent = 0;
    int recv = 0;

    FILE *ftcpseq;
    char name[40];		// = "cln_files/test_seq_tcp.txt";
    get_file_name(name, 40, sndseq_t);

    if (create_file(name) == 1)
	return 0;

    if ((ftcpseq = fopen(name, "w")) == NULL) {
	printf("ERR: File test_sock.c, line 211, fopen():\n");
	return 0;
    }
    char time_string[15];	// строка для хранения времени

    unsigned long time_snd = 0;	// время отправки пакета
    unsigned long time_rcv = 0;	// время приема пакета

    count_packg = settings.cnt_ptk;
    // конвертируем ip назначения

    char *d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);	// конвертируем ip назначения

    // вычисляем общий размер пакета
    int packet_size =
	sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_len;

    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc(packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc(packet_size);	// буфер приема      

    if (!snd_ptk || !rcv_ptk) {
	perror("ERR: File test_sock.c, line 234, malloc()");
	exit(1);
    }
    // Обнуляем буферы
    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct sockaddr_in sck_adrr;	// структура сокета
    int fd_sock;		// десриптор сокета
    socklen_t sock_len;		// длина сокета

    // Создаем сокет в зависимости от выбранного протокола
    if ((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {
	perror("ERR: File test_sock.c, line 247, socket()");
	FREE_MEM;
	exit(1);
    }

    int on = 1;
    // Мы обещаем предоставить IP заголовок
    if (setsockopt
	(fd_sock, IPPROTO_IP, IP_HDRINCL, (const char *) &on,
	 sizeof(on)) == -1) {
	perror("ERR: File test_sock.c, line 255, setsockopt()");
	FREE_MEM;
	close(fd_sock);
	exit(1);
    }
    // ip заголовок в буфере
    struct iphdr *ip = (struct iphdr *) snd_ptk;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(packet_size);
    ip->id = 500;
    ip->frag_off = 0;
    ip->ttl = 65;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = 0;
    ip->daddr = d_ip;

    struct tcphdr *tcp =
	(struct tcphdr *) (snd_ptk + sizeof(struct iphdr));
    tcp->source = htons(TCP_DPORT);
    tcp->dest = htons(TCP_SPORT);
    tcp->seq = 0;
    tcp->doff = 5;
    tcp->ack_seq = 0;
    tcp->window = htons(32767);
    tcp->check = 0;
    tcp->urg_ptr = 0;

    char *data = snd_ptk + sizeof(struct iphdr) + sizeof(struct tcphdr);
    ip->check = in_cksum((unsigned short *) data, payload_len);

    struct iphdr *ip_rcv = (struct iphdr *) rcv_ptk;
    struct tcphdr *tcp_rcv =
	(struct tcphdr *) (rcv_ptk + sizeof(struct iphdr));

    memcpy(&snd_ptk[0], ip, 20);	// копирую в буфер заполненную структуру IP
    memcpy(&snd_ptk[20], tcp, 20);	// копирую в буфер заполненную структуру ICMP

    sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(TCP_DPORT);

    sock_len = sizeof(sck_adrr);

    printf("[ !] START TEST\n");
    fprintf(ftcpseq, "[ !] START TEST\n");
    get_time_string(time_string, sizeof(time_string));
    fprintf(ftcpseq, "  Time start test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд. 

    for (int i = 0; i < count_packg; i++) {

	//memset(snd_ptk + sizeof(struct iphdr) + sizeof(struct tcphdr), (char)1+i, payload_len);
	if (sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr *) &sck_adrr, sock_len) < 1) {	// отправляем сообщение
	    perror("ERR: File test_sock.c, line 311, sendto()");
	    FREE_MEM;
	    close(fd_sock);
	    exit(1);
	}

	++sent;

	printf("   %d packet send seq\n", sent);
	fprintf(ftcpseq, "   %d packet send seq\n", sent);
	while (1) {
	    if (recvfrom
		(fd_sock, rcv_ptk, packet_size, 0,
		 (struct sockaddr *) &sck_adrr, &sock_len) == -1) {
		perror("ERR: File test_sock.c, line 323, recvfrom():");
		FREE_MEM;
		close(fd_sock);
		exit(1);
	    }
	    if (tcp_rcv->dest == htons(TCP_DPORT)) {
		++recv;
		//printf("prot=%u ",ip_rcv->protocol);
		//printf("dp=%u\n",ntohs(tcp_rcv->dest));
		printf("   %d packet reciev seq\n", recv);
		fprintf(ftcpseq, "   %d packet reciev seq\n", sent);
		break;
	    }
	}
    }
    printf("----------------result---------------\n");
    fprintf(ftcpseq, "----------------result---------------\n");
    printf
	("   Package send=%d\n   Package reciev=%d\n   Len package=%d (20 bytes IP H,20 bytes TCP H)\n",
	 sent, recv, packet_size);
    fprintf(ftcpseq,
	    "   Package send=%d\n   Package reciev=%d\n   Len package=%d (20 bytes IP H,20 bytes TCP H)\n",
	    sent, recv, packet_size);
    get_time_string(time_string, sizeof(time_string));
    fprintf(ftcpseq, "  Time end test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд. 
    printf("[ !] END TEST\n");
    fprintf(ftcpseq, "[ !] END TEST\n");
    FREE_MEM;
    close(fd_sock);
    fclose(ftcpseq);
#undef FREE_MEM

    return 0;
}

/*
	Сокет создается при выборе теста последовательной передачи пакетов.
	Не понял что конкретно было нужно,то ли имитировать нарушение последовательности пакетов,то ли 
	проверить канал на нарушение последовательности.
	Поэтому сокет реализован как отпрвка-прием,последовательность пакетов.
	Сокет для типа протокола UDP
*/
void *run_seq_udp()
{
#define FREE_MEM free(snd_ptk);free(rcv_ptk)

    unsigned long d_ip;		// ip назначения
    int count_packg;		// количество пакетов
    int payload_len = settings.len_ptk;	// размер блока данных
    FILE *fudpseq;
    char name[40];		//= "cln_files/test_seq_udp.txt";
    get_file_name(name, 40, sndseq_u);

    if (create_file(name) == 1)
	return 0;

    if ((fudpseq = fopen(name, "w")) == NULL) {
	printf("ERR: File test_sock.c, line 382, fopen():\n");
	return 0;
    }
    char time_string[15];	// строка для хранения времени

    int sent = 0;
    int recv = 0;
    unsigned long time_snd = 0;	// время отправки пакета
    unsigned long time_rcv = 0;	// время приема пакета

    // конвертируем количество пакетов
    count_packg = settings.cnt_ptk;


    // конвертируем ip назначения
    char *d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);	// конвертируем ip назначения

    // вычисляем общий размер пакета
    int packet_size =
	sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len;


    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc(packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc(packet_size);	// буфер приема      
    if (!snd_ptk || !rcv_ptk) {
	perror("ERR: File test_sock.c, line 407/408, malloc()");
	exit(1);
    }
    // Обнуляем буферы
    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct iphdr *ip_rcv = (struct iphdr *) rcv_ptk;
    struct udphdr *udp_rcv =
	(struct udphdr *) (rcv_ptk + sizeof(struct iphdr));

    struct sockaddr_in sck_adrr;	// структура сокета
    int fd_sock;		// десриптор сокета
    socklen_t sock_len;		// длина сокета

    // Создаем сокет в зависимости от выбранного протокола
    if ((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
	perror("ERR: File test_sock.c, line 426, socket()");
	FREE_MEM;
	exit(1);
    }
    // Мы обещаем предоставить IP заголовок
    int on = 1;
    if (setsockopt
	(fd_sock, IPPROTO_IP, IP_HDRINCL, (const char *) &on,
	 sizeof(on)) == -1) {
	perror("ERR: File test_sock.c, line 433, setsockopt()");
	FREE_MEM;
	close(fd_sock);
	exit(1);
    }
    // ip заголовок в буфере
    struct iphdr *ip = (struct iphdr *) snd_ptk;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(packet_size);
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = 65;
    ip->protocol = IPPROTO_UDP;
    ip->saddr = 0;
    ip->daddr = d_ip;
    memcpy(&snd_ptk[0], ip, 20);	// копирую в буфер заполненную структуру IP

    struct udphdr *udp =
	(struct udphdr *) (snd_ptk + sizeof(struct iphdr));
    udp->source = htons(UDP_DPORT);
    udp->dest = htons(UDP_SPORT);
    udp->len = htons(packet_size - 20);
    memcpy(&snd_ptk[20], udp, 8);	// копирую в буфер заполненную структуру ICMP

    sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(UDP_DPORT);

    sock_len = sizeof(sck_adrr);
    printf("[ !] START TEST\n");
    fprintf(fudpseq, "[ !] START TEST\n");
    get_time_string(time_string, sizeof(time_string));
    fprintf(fudpseq, "  Time start test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд.
    for (int i = 0; i < count_packg; i++) {

	memset(snd_ptk + sizeof(struct iphdr) + sizeof(struct udphdr),
	       (char) 1 + i, payload_len);
	ip->id = i;
	udp->check = 0;
	udp->check =
	    in_cksum((unsigned short *) udp,
		     sizeof(struct udphdr) + payload_len);

	if (sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr *) &sck_adrr, sock_len) < 1) {	// отправляем сообщение
	    perror("ERR: File test_sock.c, line 481, sendto()");
	    FREE_MEM;
	    close(fd_sock);
	    exit(1);
	}
	++sent;
	printf("   %d packet send seq\n", sent);
	fprintf(fudpseq, "   %d packet send seq\n", sent);

	while (1) {
	    if (recvfrom
		(fd_sock, rcv_ptk, packet_size, 0,
		 (struct sockaddr *) &sck_adrr, &sock_len) == -1) {
		perror("ERR: File test_sock.c, line 492, recvfrom()");
		FREE_MEM;
		close(fd_sock);
		exit(1);
	    }
	    if (udp_rcv->dest == htons(UDP_DPORT)) {
		++recv;
		printf("   %d packet reciev seq\n", recv);
		fprintf(fudpseq, "   %d packet reciev seq\n", sent);
		break;
	    }
	}
    }
    printf("----------------result---------------\n");
    fprintf(fudpseq, "----------------result---------------\n");
    printf
	("   Package send=%d\n   Package reciev=%d\n   Len package=%d (20 bytes IP HEADER,8 bytes UDP HEADER)\n",
	 sent, recv, packet_size);
    fprintf(fudpseq,
	    "   Package send=%d\n   Package reciev=%d\n   Len package=%d (20 bytes IP HEADER,8 bytes UDP HEADER)\n",
	    sent, recv, packet_size);
    get_time_string(time_string, sizeof(time_string));
    fprintf(fudpseq, "  Time end test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд. 
    printf("[ !] END TEST\n");
    fprintf(fudpseq, "[ !] END TEST\n");
    FREE_MEM;
    fclose(fudpseq);
    close(fd_sock);
#undef FREE_MEM
    return 0;
}

/*
	Сокет создается при выборе теста лоад.
	Не нашел как имитировать загрузку канала.
	Поэтому что-бы что-то хоть было,сделал тест пропускную способность канала
*/
void *run_load_udp()
{
#define FREE_MEM free(snd_ptk);free(rcv_ptk)

    unsigned long d_ip;		// ip назначения
    int count_packg;		// количество пакетов
    int payload_len = settings.len_ptk;	// размер блока данных
    //printf("payload_len=%d\t",payload_len);
    int sent = 0;
    int recv = 0;
    FILE *fudpload;
    char name[40];		//= "cln_files/test_load_udp.txt";
    get_file_name(name, 40, sndload);

    if (create_file(name) == 1)
	return 0;

    if ((fudpload = fopen(name, "w")) == NULL) {
	printf("ERR: File test_sock.c, line 549, fopen():\n");
	return 0;
    }
    char time_string[15];	// строка для хранения времени
    unsigned long time_start = 0;	// время отправки пакета
    unsigned long time_sleep = 0;	// время приема пакета
    unsigned short sucsess = 0;
    float perc = 0.0;
    int count_test = 1;

    // конвертируем количество пакетов
    //count_packg = settings.cnt_ptk;
    //printf("count_ptk=%d\t",count_packg);

    // конвертируем ip назначения
    char *d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);	// конвертируем ip назначения

    // вычисляем общий размер пакета
    int packet_size =
	sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len;


    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc(packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc(packet_size);	// буфер приема      
    if (!snd_ptk || !rcv_ptk) {
	perror("ERR: File test_sock.c, line 574/575, malloc()");
	exit(1);
    }
    // Обнуляем буферы
    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct iphdr *ip_rcv = (struct iphdr *) rcv_ptk;
    struct udphdr *udp_rcv =
	(struct udphdr *) (rcv_ptk + sizeof(struct iphdr));
    struct data_ptk *data_rcv =
	(struct data_ptk *) (rcv_ptk + sizeof(struct iphdr) +
			     sizeof(struct udphdr));

    struct sockaddr_in sck_adrr;	// структура сокета
    int fd_sock;		// десриптор сокета
    socklen_t sock_len;		// длина сокета

    // Создаем сокет в зависимости от выбранного протокола
    if ((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
	perror("ERR: File test_sock.c, line 596, socket():\n");
	FREE_MEM;
	exit(1);
    }
    // Мы обещаем предоставить IP заголовок
    int on = 1;
    if (setsockopt
	(fd_sock, IPPROTO_IP, IP_HDRINCL, (const char *) &on,
	 sizeof(on)) == -1) {
	perror("ERR: File test_sock.c, line 596, setsockopt()");
	FREE_MEM;
	close(fd_sock);
	exit(1);
    }
    // ip заголовок в буфере
    struct iphdr *ip = (struct iphdr *) snd_ptk;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(packet_size);
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = 65;
    ip->protocol = IPPROTO_UDP;
    ip->saddr = 0;
    ip->daddr = d_ip;

    struct udphdr *udp =
	(struct udphdr *) (snd_ptk + sizeof(struct iphdr));
    udp->source = htons(UDP_DPORT);
    udp->dest = htons(UDP_SPORT);
    udp->len = htons(payload_len + 8);

    struct data_ptk *data =
	(struct data_ptk *) (snd_ptk + sizeof(struct iphdr) +
			     sizeof(struct udphdr));

    sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(UDP_DPORT);

    sock_len = sizeof(sck_adrr);
    printf("[ !] START TEST\n");
    fprintf(fudpload, "[ !] START TEST\n");
    get_time_string(time_string, sizeof(time_string));
    fprintf(fudpload, "  Time start test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд.
    time_start = get_time_ms();
    int i = 0;
    while (1) {

	if ((get_time_ms() - time_start) > 2000) {
	    printf("   [%d]", count_test);
	    if (sent > recv) {
		time_sleep += 100;
		sucsess = 0;
		perc = (float) (sent - recv) / 100;
		printf(" Lost = %.3f %%", perc);
		fprintf(fudpload, "   Lost = %f %%", perc);
	    }
	    if (sent == recv) {
		sucsess++;
	    }
	    printf("   %d sent %d recv %u sucsess\n", sent, recv, sucsess);
	    fprintf(fudpload, "   %d sent %d recv %u sucsess\n", sent,
		    recv, sucsess);
	    sent = 0;
	    recv = 0;
	    count_test += 1;
	    time_start = get_time_ms();
	}
	if (sucsess == 2) {
	    printf("----------------result---------------\n");
	    fprintf(fudpload, "----------------result---------------\n");
	    printf
		("   Sucsess test num=%d\n   Len package=%d (20 bytes IP H,8 bytes UDP H)\n   Good time sleep=%ld mcs\n",
		 (count_test - 1), packet_size, time_sleep);
	    fprintf(fudpload,
		    "   Sucsess test num=%d\n   Len package=%d (20 bytes IP H,8 bytes UDP H)\n   Good time sleep=%ld mcs\n",
		    (count_test - 1), packet_size, time_sleep);
	    break;
	}

	udp->check = 0;
	udp->check =
	    in_cksum((unsigned short *) udp,
		     sizeof(struct udphdr) + payload_len);
	//printf("packet_size = %d",packet_size);
	if (sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr *) &sck_adrr, sock_len) < 1) {	// отправляем сообщение
	    perror("ERR: File test_sock.c, line 684, sendto()");
	    FREE_MEM;
	    close(fd_sock);
	    exit(1);
	}
	++sent;
	//printf("%d packets sent\n", sent);


	if (recvfrom
	    (fd_sock, rcv_ptk, packet_size, 0,
	     (struct sockaddr *) &sck_adrr, &sock_len) == -1) {
	    perror("ERR: File test_sock.c, line 694, recvfrom():");
	    FREE_MEM;
	    close(fd_sock);
	    exit(1);
	}
	if (ntohs(udp_rcv->dest) == UDP_DPORT) {
	    ++recv;
	    //printf("%d packets recv\tptk_num=%d\n", recv,ntohl(data_rcv->num_ptk));

	}

	usleep(time_sleep);	//microseconds
	i++;
    }
    get_time_string(time_string, sizeof(time_string));
    fprintf(fudpload, "  Time end test: %s\n", time_string);	// Отображение даты и времени с указанием числа миллисекунд. 
    printf("[ !] END TEST\n");
    fprintf(fudpload, "[ !] END TEST\n");
    FREE_MEM;
    close(fd_sock);
    fclose(fudpload);
#undef FREE_MEM
    return 0;
}


/* Не работает,не отлажено =(
void* run_load_tcp(){
#define FREE_MEM free(snd_ptk);free(rcv_ptk)
    
    unsigned long d_ip;								// ip назначения
    int count_packg;								// количество пакетов
    int payload_len = settings.len_ptk;						// размер блока данных
    
    int sent = 0;
    int recv = 0;
    unsigned long time_start = 0;						// время отправки пакета
	unsigned long time_sleep = 0;						// время приема пакета
	unsigned short sucsess = 0; 
	float perc = 0.0;
	int count_test = 1;

    // конвертируем количество пакетов
    //count_packg = settings.cnt_ptk;
    //printf("count_ptk=%d\t",count_packg);
    
    // конвертируем ip назначения
    char* d_ip_str = ip_s;
    d_ip = inet_addr(ip_s);  // конвертируем ip назначения

    // вычисляем общий размер пакета
    int packet_size = sizeof (struct iphdr) + sizeof(struct tcphdr) + payload_len; 
    //printf("packet_size=%d\n",packet_size);

    //выделяем память под пакеты
    char *snd_ptk = (char *) malloc (packet_size);	// буфер отправки
    char *rcv_ptk = (char *) malloc (packet_size);	// буфер приема	
    if(!snd_ptk || !rcv_ptk){
    	perror("ERR: out of memory: rcv_ptk || snd_ptk");
    	exit(1);
    }
    // Обнуляем буферы
    memset(snd_ptk, 0, packet_size);
    memset(rcv_ptk, 0, packet_size);

    struct iphdr *ip_rcv = (struct iphdr *) rcv_ptk;
    struct tcphdr *tcp_rcv = (struct tcphdr *) (rcv_ptk + sizeof (struct iphdr));
    //struct data_ptk *data_rcv = (struct data_ptk*) (rcv_ptk + sizeof (struct iphdr) + sizeof(struct tcphdr));

    struct sockaddr_in sck_adrr;		// структура сокета
    int fd_sock;						// десриптор сокета
	socklen_t sock_len;					// длина сокета
	
	// Создаем сокет в зависимости от выбранного протокола
    if((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1){
    	perror("ERR: socket();\n");
    	FREE_MEM;
    	exit(1);
    }
    
    // Мы обещаем предоставить IP заголовок
    int on = 1;
    if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) 
    {
        perror("setsockopt");
        FREE_MEM;
        close(fd_sock);
        exit (1);
    }

  	// ip заголовок в буфере
    struct iphdr *ip = (struct iphdr *) snd_ptk;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(packet_size);
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = 65;
    ip->protocol = IPPROTO_UDP;
    ip->saddr = 0;
    ip->daddr = d_ip;

   	struct tcphdr *tcp = (struct tcphdr *) (snd_ptk + sizeof (struct iphdr));
    tcp->source = htons(TCP_DPORT);
    tcp->dest = htons(TCP_SPORT);
    tcp->seq = 0;
    tcp->doff = 5;
    tcp->ack_seq = 0;
    tcp->window = htons(32767);
    tcp->check = 0;
    tcp->urg_ptr = 0;

   	char* data =  snd_ptk + sizeof (struct iphdr) + sizeof(struct tcphdr);
   	ip->check = in_cksum((unsigned short *)data, payload_len);

   	sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(TCP_DPORT);
    
    sock_len = sizeof(sck_adrr);
    printf("[ !] START TEST!\n");
    time_start = get_time_ms();
    int i = 0;
	while(1){

		if((get_time_ms()-time_start)>2000){
			if(sent>recv){
				time_sleep+=100;
				sucsess = 0;
				perc = (float)(sent-recv)/100;
				printf("Lost = %f \n",perc);
			}
			if(sent==recv){
				sucsess++;
			}
			printf("%d sent %d recv %u sucsess\n",sent,recv,sucsess);
			sent = 0;
			recv = 0;
			count_test+=1;
			time_start = get_time_ms();
			
		}
		if(sucsess==2){
			printf("----------------result---------------\n");
			printf("Sucsess test num=%d\nLen package=%d\ntime sleep=%ld mcs\n",count_test,packet_size,time_sleep);
			break;
		}

		tcp->check = 0;
        tcp->check = in_cksum((unsigned short *)tcp, sizeof(struct tcphdr) + payload_len);
        
        if(sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, sock_len) < 1) {	// отправляем сообщение
			perror("ERR: sendto()");
			FREE_MEM;
			close(fd_sock);
			exit(1);
		}
		++sent;
        while(1){
        	if(recvfrom(fd_sock, rcv_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, &sock_len) == -1) {
		    	perror("ERR: recvfrom():");
		    	FREE_MEM;
		    	close(fd_sock);
		    	exit(1);
			}
			if(ntohs(tcp_rcv->dest)==TCP_DPORT){
				++recv;
				break;		
			}	
        }
        
        usleep(time_sleep);  //microseconds
        i++;
	}
	printf("[ !] END TEST!\n");
	FREE_MEM;
	close(fd_sock);
#undef FREE_MEM
	return 0;
}*/
