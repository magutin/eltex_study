#include "stdio.h"


// выполнить задание из лекции 11 через указатели
void main(){

	unsigned int num = 270533154;
	unsigned char *ptr_num;  //хранение байта введенного числа
	
	printf("task1.1: Print byte of 'num'\n");
	printf("num = %d\n",num);

	ptr_num = (char*)&num;
	for(int i = 0;i<sizeof(num);i++){	
		printf("%d byte = %u\n",i+1,*ptr_num);
		ptr_num++;
	}

	printf("\ntask:Change 3 byte of 'num'\n");

	printf("num = %d\n",num);
	ptr_num = (char*)&num;
	ptr_num +=2;
	*ptr_num = 12;
	ptr_num = (char*)&num;
	
	printf("new num = %d\n",num);
	for(int i = 0;i<sizeof(num);i++){	
		printf("%d byte = %u\n",i+1,*ptr_num);
		ptr_num++;
	}


}