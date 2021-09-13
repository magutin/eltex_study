#include "head.h"

#define D_PORT 8080
#define S_PORT 8081
#define PACK_LEN 108

struct udp_header{
	unsigned short SPORT;		// порт отправителя
	unsigned short DPORT;		// порт назначения
	unsigned short LEN;		// Длина пакета
	unsigned short CRC;		// контрольная сумма
};

int main(int argc, char *argv[]) {

	int file_desc_sock;
	char send_buf[80];		// буфер отправки
	char recv_buf[108];		// буфер приема
	socklen_t sock_len;		// размер сокета
	int send_len;			//длина сообщения
	
	unsigned short check;
	
	struct sockaddr_in srv;
	struct udp_header udp;
	//struct pack *recv;
	
	//обнуление буферов и полей структуры
	strncpy(send_buf,"",80);
	strncpy(recv_buf,"",80);
	memset(&srv, 0, sizeof(srv));
	
	//получаем указатель на буфер для приема
	//recv = (struct pack*)recv_buf;	
	
	// Заполняею структуру заголовка udp	
	udp.SPORT = htons(S_PORT);
	udp.DPORT = htons(D_PORT);
	udp.LEN   = htons(PACK_LEN);
	udp.CRC   = htons(0);
	
	// Заполняю посылку на отправку явным преобразованием и сдвигом
	send_buf[0] = (char)(udp.SPORT);		// забираем первый байт
	send_buf[1] = (char)(udp.SPORT >> 8);		// забираем второй байт
	send_buf[2] = (char)(udp.DPORT);
	send_buf[3] = (char)(udp.DPORT >> 8);
	send_buf[4] = (char)(udp.LEN);
	send_buf[5] = (char)(udp.LEN >> 8);
	send_buf[6] = (char)(udp.CRC);
	send_buf[7] = (char)(udp.CRC >> 8);
	
	strcat(&send_buf[8],"HELLO");
	//создаем сокет 
    	if((file_desc_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        	perror("ERR: line 59, socket():");
        	exit(1);
    	}
	// заполняем структуру сокета
    	srv.sin_family = AF_INET;  
    	srv.sin_addr.s_addr = INADDR_ANY;
    	srv.sin_port = htons(D_PORT);
	//получаем длину сокета
	sock_len = sizeof(srv);
	 	
 	while(1) {
		
		//printf("[msg ->]: %s\n",&send_buf[8]);
 		if(sendto(file_desc_sock, send_buf, PACK_LEN, 0, (struct sockaddr*)&srv, sock_len) == -1) {
        	perror("ERR: line 75, socket():");
        	exit(1);} 		
 		
		if(recvfrom(file_desc_sock, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&srv, &sock_len) == -1) {
		    perror("ERR: line 81, socket():");
		    exit(1);
		}
		/* Обрабатываю все пакеты и смотрю какие предназначены мне, а какие нет */

		check = (short)(recv_buf[21] << 8 | recv_buf[22]);
		//printf("%u\n",check);
		
		if(ntohs(check) != S_PORT) {
			printf("[<- msg]: ");
			printf("%s\n", &recv_buf[28]);
		}
		//sleep(1);		// я знаю что из-за этого рассинхрон по общению,но так хотябы пакеты неимовено быстро не пролетают
		
	}
}
