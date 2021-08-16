//Задание 3: Заполнить верхний треугольник матрицы 3х3 '1', а нижний '0'
#include "mtrx_operation.h"

void task3(unsigned int s,int (*massive)[s3]){

	int iter = 0;
	int iter1 = 0;

	for(iter = 0;iter<s;iter++){
		int iter1 = 0; 
		while((iter1+iter)<=(s-2)){
			massive[iter][iter1] = 1;
			iter1++;
		}
		while(iter1<=(s-1)){
			massive[iter][iter1] = 0;
			iter1++;
		}
	}

	/*for(iter = (n-1);iter>=0;iter--){
		int iter1 = n-1; 
		while((iter-iter1)>=0){
			massive[iter][iter1] = 0;
			iter1--;
		}
	}*/

	for (iter = 0; iter < s; iter++) {
		for(iter1 = 0;iter1<s;iter1++){
			printf("%d\t",massive[iter][iter1]);
		}
		printf("\n");
		
    }
}
