//Задание 1: Вывести квадратную матрицу по заданному N
#include "mtrx_operation.h"

void task1(unsigned int s,int **massive){

	int val = 1;
	char iter = 0;
	char iter1 = 0;
	massive = (int **)malloc(s*sizeof(int*));     //Выделение динамической памяти под массив указателей
	
	if(massive != NULL){				  		  //Проверяем ,была ли выделена память
		printf("Память под массив указателей ВЫДЕЛЕНА!\n");
	}else{printf("Память под массив указателей НЕ ВЫДЕЛЕНА!");}

	for(iter = 0;iter<s;iter++){				  //Выделение динамической памяти для массива значений
		massive[iter] = (int*)malloc(s*sizeof(int));
	}

	if (massive[iter]!=0){							  //Проверяем ,была ли выделена память
	printf("Память под массив значений ВЫДЕЛЕНА\n");
	}else{printf("Память под массив значений НЕ ВЫДЕЛЕНА!\n");}

	for (iter = 0; iter < s; iter++) {
		for(iter1 = 0;iter1<s;iter1++){
			massive[iter][iter1] = val;
			val++;
			printf("%d\t",massive[iter][iter1]);
		}
		printf("\n");
		
    }

	for(iter = 0;iter<s;iter++){		  //Освобождаем память
		free(massive[iter]);
	}

	free(massive);

}