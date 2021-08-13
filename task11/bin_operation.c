
#include <stdio.h>

void main(){
	unsigned int num = 0xaabbccdd;
	unsigned char byte_num;  //хранение байта введенного числа
	
	printf("task1.1: Print byte of 'num'\n");
	printf("HEX num = %#X\n",num);
	
	for(int i = 0;i<=3;i++){
		byte_num = ((num >> 8*i) & 0xFF);
		printf("%d byte = %#X\n",i+1,byte_num);
	}

	printf("\ntask1.2:Convert 3 byte of 'num' to DD\n");
	/*мы не можем использовать перменную byte_num типа char т.к. она равна 8 байт 
	 и остальные байты переменной num будут затираться при присвоении, 
	 (Этой проблемы можно избежать,если использовать указатели)*/
	num = (num & 0xff00ffff) | 0x00dd0000;
	printf("num = %#X\n",num);
}
