//Задание 2: Вывести массив размером N в обратном порядке
#include "mtrx_operation.h"

void task2(unsigned int s,int *massive){
	int iter;
	massive = (int*)calloc(s,sizeof(int));

	if(massive != NULL){				  		  //Проверяем ,была ли выделена память
		printf("Память под массив ВЫДЕЛЕНА!\n");
	}else{printf("Память под массив НЕ ВЫДЕЛЕНА!");}

	for(iter = 0;iter<s;iter++){
		massive[iter] = iter+1;
	}

	printf("Исходный массив!\n");
	for(iter = 0;iter<s;iter++){
		printf("%d ",massive[iter]);
	}

	printf("\nПеревернутый массив!\n");
	for(iter = (s-1);iter>=0;iter--){
		printf("%d ",massive[iter]);
	}
	printf("\n");

	free(massive);	
}