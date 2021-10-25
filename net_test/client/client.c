#include "heads.h"

int main(int argc, char** argv){
	
	if(argc<2){
		printf("Invalid args\n");
		printf("Use: %s <server IP>\n",argv[0]);
		exit(1);
	}

	unsigned long saddr = inet_addr(argv[1]);

	int fd_sock_cln,result = 0;
	struct sockaddr_in sock_srv;
	struct in_addr ip_n;

	char rcv_ptk[64];
	char snd_ptk[64];

	char answer[80];		// буфер для посылки
	int str_len;			// длина строки сообщения

	memset(&sock_srv, 0, sizeof(sock_srv));	// обнуляем структуру
    sock_srv.sin_family    = AF_INET;  		// заполняем структуру сервера
    sock_srv.sin_addr.s_addr = saddr;
    sock_srv.sin_port = htons(SRV_TCP_SPORT);
	
	socklen_t sock_len = sizeof(sock_srv);

	ip_n.s_addr = saddr;
	ip_s=inet_ntoa(ip_n);

	if((fd_sock_cln = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	// получаем id сокета сервера
           	perror("ERR: line 14. socket()");exit(1);}
        
	
	if(connect(fd_sock_cln, (struct sockaddr *)&sock_srv, sizeof(sock_srv)) == -1) {	// подклчаемся к серверу
		perror("ERR: line 21. connect()");exit(1);}

	printf("Connect to server...\n");
	printf("Server socket id: %d\n",fd_sock_cln);

	while(turnoff!=result){
		if(recv(fd_sock_cln, rcv_ptk, sizeof(rcv_ptk), 0) == -1) {
			perror("ERR: line 49. recv():");
			exit(1);
		}

		memcpy(&settings,&rcv_ptk[0],16);	// забираем настройки для теста
    		
    	ns_settings_to_s();					// конвертируем из литл индиан в биг индиан    		
		
		result = rcv_cmd_parser();	
		if(result==sndseq_t||result==sndseq_u||result==sndping||result==sndload){
			send_file(fd_sock_cln,result);
		}
		
	}
	close(fd_sock_cln);
	return 0;
}

