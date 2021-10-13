#include "heads.h"

int main(){
	int fd_sock_cln,turnoff = 0;
	struct sockaddr_in sock_cln;
		
	struct ip_h        ip_hdr_rcv;
	struct udp_h      udp_hdr_rcv;
	char rcv_ptk[64];
	char snd_ptk[64];

	memset(&sock_cln, 0, sizeof(sock_cln));					// обнуляем структуру

	sock_cln.sin_family = AF_INET;  					// в структуре сервера говорим,о соединении типа IPv4
    sock_cln.sin_addr.s_addr = htonl(INADDR_ANY); 		// все адреса локальной сети 0.0.0.0
    sock_cln.sin_port = htons(SRV_TCP_DPORT); 			// конвертируем данные из узлового порядка расположения байтов в сетевой 

    // Сокет UDP бу(дет посылать настройки для теста клиенту
    if((fd_sock_cln = socket(AF_INET, SOCK_RAW,IPPROTO_UDP)) == -1) {	//получаем дискриптор сокета сервера
		perror("ERR: line 24, socket():\n\r");
		exit(1);
	}
	int on = 1;
	if (setsockopt (fd_sock_cln, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) {
        perror("setsockopt");
        exit (1);
    }

	socklen_t sock_len = sizeof(sock_cln);

	while(1!=turnoff){
		if(recvfrom(fd_sock_cln, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&sock_cln, &sock_len)==-1) {
			perror("Recvfrom:\n\r");
			exit(1);
		}
		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&udp_hdr_rcv,&rcv_ptk[20],8);

		if(ntohs(udp_hdr_rcv.DPORT)==SRV_TCP_DPORT){

			memcpy(&settings,&rcv_ptk[28],16);	// забираем настройки для теста
    		ns_settings_to_s();					// конвертируем из литл индиан в биг индиан

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
    		udp->source = htons(SRV_TCP_DPORT);
    		udp->dest = htons(SRV_TCP_SPORT);
    		udp->len = htons(44);
			udp->check = in_cksum((unsigned short *)&snd_ptk[20], 44);

    		memcpy(&snd_ptk[20],udp,8);			// копирую в буфер заполненную структуру UDP
    		
			if(sendto(fd_sock_cln, snd_ptk, sizeof(snd_ptk), 0, (struct sockaddr*)&sock_cln, sock_len)==-1) {
				perror("\tERR: Command 'set' not send. send():");
				exit(1);
			}	
			//sleep(3);
			turnoff = rcv_cmd_parser();		
		}
	}
	close(fd_sock_cln);
	return 0;
}

