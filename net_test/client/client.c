//#include <cmd.h>
#include "heads.h"
#include <sys/types.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct cmd_settings settings;
unsigned short in_cksum(unsigned short *ptr, int nbytes);

int main(){
	int fd_sock_cln;
	struct sockaddr_in sock_cln;
	struct in_addr ip_n;
	char* ip_s;
	struct ip_h        ip_hdr_rcv;
	struct udp_h      udp_hdr_rcv;
	char rcv_ptk[64];
	char snd_ptk[64];

	memset(&sock_cln, 0, sizeof(sock_cln));					// обнуляем структуру

	
    //udp->check = 0;
    

	sock_cln.sin_family = AF_INET;  					// в структуре сервера говорим,о соединении типа IPv4
    sock_cln.sin_addr.s_addr = htonl(INADDR_ANY); 				// все адреса локальной сети 0.0.0.0
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

	while(1){
		if(recvfrom(fd_sock_cln, rcv_ptk, sizeof(rcv_ptk), 0, (struct sockaddr*)&sock_cln, &sock_len)==-1) {
			perror("Recvfrom:\n\r");
			exit(1);
		}
		memcpy(&ip_hdr_rcv,&rcv_ptk[0],20);
		memcpy(&udp_hdr_rcv,&rcv_ptk[20],8);
		//printf("DPORT=%u\n",ntohs(udp_hdr_rcv.DPORT));
		//printf("SPORT=%u\n",ntohs(udp_hdr_rcv.SPORT));

		if(ntohs(udp_hdr_rcv.DPORT)==SRV_TCP_DPORT){
			memcpy(&settings,&rcv_ptk[28],16);
			printf("[->] RECIEV the command\n");
			
			settings.cmd = ntohs(settings.cmd);
			settings.type_ptk = ntohs(settings.type_ptk);
			settings.len_ptk = ntohs(settings.len_ptk);
			settings.cnt_ptk = ntohs(settings.cnt_ptk);
			settings.time_test = ntohs(settings.time_test);
			settings.vlan = ntohs(settings.vlan);
			settings.ip_cln = ntohl(settings.ip_cln);

			printf("\tcmd=%u\t",settings.cmd);				// номер команды теста 
			printf("type_ptk=%u\t",settings.type_ptk);		// тип пакета (TCP/UDP/ICMP)
			printf("len_ptk=%u\t",settings.len_ptk);		// длина пакета
			printf("cnt_ptk=%u\t",settings.cnt_ptk);		// количество пакетов
			printf("time_test=%u\t",settings.time_test);	// время проведения теста
			printf("vlan=%u\t",settings.vlan);				// поддержка 802.1q,802.1p
			ip_n.s_addr = settings.ip_cln;
			printf("ip_cln=%s\n",ip_s=inet_ntoa(ip_n));		//ntohs(settings.ip_cln));		// ip клиента

    		
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
    		//memcpy(&snd_ptk[28],&settings,sizeof(&settings));
    		
			if(sendto(fd_sock_cln, snd_ptk, sizeof(snd_ptk), 0, (struct sockaddr*)&sock_cln, sock_len)==-1) {
				perror("\tERR: Command 'set' not send. send():");
				exit(1);
			}	
			//sleep(3);
			//стартуем тест порядка
			if(settings.cmd==1){printf("seq\n");}

			//стартуем тест пинг
			if(settings.cmd==2){printf("ping\n");}
			
			//стартуем тест load
			if(settings.cmd==3){printf("load\n");}
			
			//завершаем работу
			if(settings.cmd==4){printf("shutdown\n");}
			
			//задаем настройки для теста
			if(settings.cmd==5){printf("seq\n");}

			//задаем настройки для теста
			if(settings.cmd==6){printf("ping\n");}

			//задаем настройки для теста 
			if(settings.cmd==7){printf("load\n");}
			

		}
					
	}

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
