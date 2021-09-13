#include "head.h"

int main() {
	char txt_ans[23] = "Server reciev msg time:";
	char answer[80];			// буфер ответа
	char reciev[80];			// буфер приема
	int file_desc;			// файловый дескриптор сервера
	//int file_desc_cln;			// файловый дескриптор клиента

	struct timeval tv;			// структура времени
 	struct tm* ptm;				
 	char time_string[40];		// строка для хранения времени
 	socklen_t sock_len;
 	long ms;

	struct sockaddr_in cln;		// стуктура для свервера и клиента
	struct sockaddr_in srv;	

	memset(&cln, 0, sizeof(cln));
	memset(&srv, 0, sizeof(srv));				// обнуление структур
	
	srv.sin_family = AF_INET;  					// в структуре сервера говорим,о соединении типа IPv4
    srv.sin_addr.s_addr = INADDR_ANY; 			// все адреса локальной сети 0.0.0.0
    srv.sin_port = htons(PORT); 			// конвертируем данные из узлового порядка расположения байтов в сетевой 


	if((file_desc = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {	//получаем дискриптор сокета сервера
		perror("ERR: line 24, socket():");
		exit(1);
	}

	printf("Server socket id: %d\n",file_desc);

	if(bind(file_desc, (struct sockaddr *)&srv, sizeof(srv)) == -1) {		// назначаем имя сокету
		perror("ERR: line 29. bind():");
		exit(1);
	}
    
    sock_len = sizeof(cln);
	while(1) {
		if( recvfrom(file_desc, reciev, sizeof(reciev), 0, (struct sockaddr*)&cln, &sock_len ) == -1) {
			perror("ERR: line 49. recv():");
			exit(1);
		}
		
		gettimeofday(&tv, NULL);	// Определение текущего времени и преобразование полученного значения в структуру типа tm. 
 		ptm = localtime(&tv.tv_sec);
 		strftime(time_string, sizeof(time_string),"%H:%M:%S", ptm);// Форматирование значения даты и времени с точностью до секунды. 

		printf("[<- msg]:%s\n",reciev);

		
 		printf(" - %s %s\n",txt_ans,time_string); // Отображение даты и времени с указанием числа миллисекунд. 

		strcat(answer, txt_ans);
		strcat(answer,time_string);
		if(sendto(file_desc, answer, strlen(answer), 0, (struct sockaddr*)&cln, sock_len) == -1) {
			perror("ERR: line 58. send():");
			exit(1);
		}
		printf("[msg ->]: %s %s\n",txt_ans,time_string);

		strncpy(answer,"", 80);
		strncpy(reciev,"", 80);
	}
	return 0;
}
