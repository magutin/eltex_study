#include "head.h"

int main() {
	int file_desc_srv;		// id сокета сервера
	char answer[80];		// буфер для посылки
	int str_len;			// длина строки сообщения
	struct sockaddr_in srv;		// структура сокета сервера
	socklen_t sock_len;
	
	memset(&srv, 0, sizeof(srv));	// обнуляем структуру
        srv.sin_family    = AF_INET;  		// заполняем структуру сервера
        srv.sin_addr.s_addr = INADDR_ANY;
        srv.sin_port = htons(TCP_PORT);
	
        if((file_desc_srv = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {	// получаем id сокета сервера
                perror("ERR: line 14. socket()");
                exit(1);
        }
	
	printf("Connect to server...\n");
	printf("Server socket id: %d\n",file_desc_srv);

	sock_len = sizeof(srv);
	while(1){

		strncpy(&answer[0],"",80);
		
		printf("[msg ->]: ");			// юзер вводит сообщение
		fgets(answer,80,stdin);
		str_len = strlen(answer);

		strncpy(&answer[str_len-1],"",80-str_len);
		if(sendto(file_desc_srv, answer, strlen(answer), MSG_CONFIRM, (struct sockaddr*)&srv, sock_len) == -1) {	// отправляем сообщение
			perror("ERR: line 28. send()");
			exit(1);
		}

      		if(recvfrom(file_desc_srv, answer, sizeof(answer), MSG_WAITALL, (struct sockaddr*)&srv, &sock_len) == -1) {	// принимаем ответ
			perror("ERR: line 34. recv()");
			exit(1);
		}

		printf("[<- msg]: %s\n",answer);
		strncpy(answer,"", 80);
	}
}
