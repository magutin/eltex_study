#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/time.h>		// для gettimeofday
#include <sys/types.h>		// для getpid
#include <sys/socket.h>		// для сокета

#include <netinet/ip.h>		// для ip заголовка
#include <netinet/udp.h> 	// для udp заголовка
#include <netinet/tcp.h>	// для TCP заголовка
#include <arpa/inet.h>

#define TCP 20
#define UDP 8

#define S_PORT 60564
#define D_PORT 60565


unsigned short in_cksum(unsigned short *ptr, int nbytes);

int main(int argc,char** argv){

	if (argc < 4) {
        printf("usage: <len> <type_pack> <count_pack> <des_ip>\n");
        exit(0);
    }
    char send[92];
    int len_proto;									// длина выбранного протокола
    int type_proto;									// длина выбранного протокола
    unsigned long d_ip;								// ip назначения
    int count_packg;								// количество пакетов
    int payload_len = atoi(argv[1]);				// размер блока данных
    printf("payload_len=%d\t",payload_len);

    int sent = 0;

    unsigned long time_snd = 0;						// время отправки пакета
	unsigned long time_rcv = 0;						// время приема пакета

    if(atoi(argv[2])==2){							// если выбран TCP протокол
    	len_proto = TCP;
    	type_proto = IPPROTO_TCP;
    	printf("TCP\t");
    }	
    else if(atoi(argv[2])==1){						// если выбран UDP протокол
    	len_proto = UDP;
    	type_proto = IPPROTO_UDP;
    	printf("UDP\t");
    }
    else{
    	printf("ERR: Unknown protokol\n");
    	exit(0);
    }

    // конвертируем количество пакетов
    count_packg = atoi(argv[3]);
    printf("count_ptk=%d\t",count_packg);
    // конвертируем ip назначения
    printf("ip=%s\n",argv[4]);
    d_ip = inet_addr(argv[4]);  

    // вычисляем общий размер пакета
    int packet_size = sizeof (struct iphdr) + len_proto + payload_len; 
    printf("packet_size=%d\n",packet_size);
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

    struct sockaddr_in sck_adrr;		// структура сокета
    int fd_sock;						// десриптор сокета
	socklen_t sock_len;					// длина сокета

	//memset(&fd_sock.sin_zero, 0, sizeof(fd_sock.sin_zero));
	
	// Создаем сокет в зависимости от выбранного протокола
    if((fd_sock = socket(AF_INET, SOCK_RAW, type_proto)) == -1){
    	perror("ERR: socket();\n");
    	exit(1);
    }
    
    int on = 1;
    // Мы обещаем предоставить IP заголовок
    if (setsockopt (fd_sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) 
    {
        perror("setsockopt");
        exit (1);
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
    ip->protocol = type_proto;
    ip->saddr = 0;//inet_addr("192.168.1.4");
    ip->daddr = d_ip;
    //ip->check = in_cksum ((u16 *) ip, sizeof (struct iphdr));
    memcpy(&snd_ptk[0],ip,20);	// копирую в буфер заполненную структуру IP

    //if(type_proto==IPPROTO_UDP)				// udp заголовок в буфере
    	struct udphdr *udp = (struct udphdr *) (snd_ptk + sizeof (struct iphdr));
    	udp->source = htons(S_PORT);
    	udp->dest = htons(D_PORT);
    	udp->len = htons(packet_size-20);
    	udp->check = 0;
    	memcpy(&snd_ptk[20],udp,8);	// копирую в буфер заполненную структуру ICMP
   	
    /*}else if(type_proto==IPPROTO_TCP){			// tcp заголовок в буфере
    	struct tcphdr *tcp = (struct tcphdr *) (snd_ptk + sizeof (struct iphdr));
    }else{
    	printf("ERR: Unknown protokol\n");
    	exit(0);	
    }*/
    
    
    
    
 	

   	sck_adrr.sin_family = AF_INET;
    sck_adrr.sin_addr.s_addr = d_ip;
    sck_adrr.sin_port = htons(S_PORT);
    
    sock_len = sizeof(sck_adrr);

	for(int i = 0;i<count_packg;i++){

		memset(snd_ptk + sizeof(struct iphdr) + sizeof(struct udphdr), (char)1+i, payload_len);

		udp->check = 0;
        udp->check = in_cksum((unsigned short *)udp, sizeof(struct udphdr) + payload_len);
        
        for(int j = 0;j < packet_size;j++){
        	printf("%x ",ntohs(snd_ptk[j]));
        }
        printf("\n");
        if(sendto(fd_sock, snd_ptk, packet_size, 0, (struct sockaddr*)&sck_adrr, sock_len) < 1) {	// отправляем сообщение
			perror("ERR: line 28. send()");
			exit(1);
		}

		++sent;
        printf("%d packets sent\r", sent);
        fflush(stdout);
        
        //usleep(10000);  //microseconds
        sleep(1);
	}

	
	free(snd_ptk);
	free(rcv_ptk);
	return 0;

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