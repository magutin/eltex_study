#include "head.h"

#define D_PORT 8080
#define S_PORT 8081
#define PACK_LEN 108

#pragma pack(push,1)
struct ip_header{
	//unsigned char  VER_HLEN;	

	//Забавный факт,разворот нужен для биг индиан  1 байт = VER.HLEN
	unsigned char  HELEN   : 4;		// Номер версии			4  бита--| 	
	unsigned char  VER     : 4;		// Длина заголовка  	4  бита--|

	unsigned char  SERV_TYPE;		// Тип Сервиса			8  бит
	unsigned short TOTL_LEN;		// Общая длина 			16 бит
	unsigned short IDENTIF;			// ID пакета			16 бит
	unsigned short F_OFFST : 13 ;	// Флаги			    3  бита--|
	unsigned short FLAGS   : 3;	    // Смещение фрагмента	13 бит---|
	unsigned char  TTL;				// Время жизни			8  бит
	unsigned char  PROTOCOL;		// Тип протокола		8  бит
	unsigned short H_CSUM;			// контрольная сумма	16 бит
	unsigned int  S_IP;			    // IP отправителя		32 бита
	unsigned int  D_IP;			    // IP получателя		32 бита
									/* -------------------------------*/
									/*         Сумма байт - 20 байт   */
};
#pragma pack(pop)

struct udp_header{
	unsigned short SPORT;		// порт отправителя
	unsigned short DPORT;		// порт назначения
	unsigned short LEN;		// Длина пакета
	unsigned short CRC;		// контрольная сумма
};

int main(int argc, char *argv[]) {

	int file_desc_sock;
	char send_buf[108];		// буфер отправки
	char recv_buf[108];		// буфер приема
	socklen_t sock_len;		// размер сокета
	int send_len;			//длина сообщения
	
	unsigned short check;
	
	struct sockaddr_in srv;
	struct udp_header udp;
	struct ip_header ip;
	
	//обнуление буферов и полей структуры
	strncpy(send_buf,"",80);
	strncpy(recv_buf,"",80);
	memset(&srv, 0, sizeof(srv));
	
	// Заполняю структуру заголовка IP	
	//printf("%ld\n",sizeof(ip));	// длинна соответствует 20 байтам

	ip.VER 		 = 0x4;
	ip.HELEN     = 0x5;						// длина заголовка IP в словах ,мин 5(5*4 байта = 20 байт)
	ip.SERV_TYPE = 0x0;						// 
	ip.TOTL_LEN  = 0x6C;//108;						// общая длина пакета,в нашем случае 108 байт(ip+udp+data)
	ip.IDENTIF   = 0x0;						// ID пакета			в нашем случае 0
	ip.FLAGS     = 0x0;						// Флаги			    в нашем случе 0
	ip.F_OFFST   = 0x0;	    				// Смещение фрагмента   в нашем случае 0	
	ip.TTL       = 0x0;						// Время жизни			
	ip.PROTOCOL  = 0x11;						// Тип протокола		в нашем случае следующий заголовок от UDP -> 17
	ip.H_CSUM    = 0x0;						// контрольная сумма	линукс игнорит это поле ,поэтому 0
	ip.S_IP 	 = inet_addr("127.0.0.1");	// IP отправителя		
	ip.D_IP      = inet_addr("127.0.0.1");	// IP получателя		
	
	// Заполняю структуру заголовка udp	
	udp.SPORT = htons(S_PORT);
	udp.DPORT = htons(D_PORT);
	udp.LEN   = htons(PACK_LEN-20);
	udp.CRC   = htons(0);
	
	memcpy(&send_buf[0],&ip,20);	// копирую в буфер заполненную структуру IP

	// Заполняю посылку на отправку явным преобразованием и сдвигом
	send_buf[20] = (char)(udp.SPORT);		    // забираем первый байт
	send_buf[21] = (char)(udp.SPORT >> 8);		// забираем второй байт
	send_buf[22] = (char)(udp.DPORT);
	send_buf[23] = (char)(udp.DPORT >> 8);
	send_buf[24] = (char)(udp.LEN);
	send_buf[25] = (char)(udp.LEN >> 8);
	send_buf[26] = (char)(udp.CRC);
	send_buf[27] = (char)(udp.CRC >> 8);
	
	strcat(&send_buf[28],"HELLO");	// Заполняю поле для сообщения

	//создаем сокет 
    if((file_desc_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
       	perror("ERR: line 59, socket():");
       	exit(1);
    }

    if(setsockopt(file_desc_sock, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) == -1) {
	    perror("setsockopt");
	    exit(1);
  	}
	
	// заполняем структуру сокета
    srv.sin_family = AF_INET;  
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(D_PORT);

	//получаем длину сокета
	sock_len = sizeof(srv);
	 	
 	while(1) {
		
		printf("[msg ->]: %s\n",&send_buf[8]);
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
