//Задание 4: Вывести массив размерностью NxN ,который заполненен 'улиткой'
#include "mtrx_operation.h"

void task4(unsigned int s){

	int val = 1; //число которе,мы будем инкрементировать в массиве
	int i = 0;	 //итератор строк
	int j = 0;	 //итератор столбцов
	int main_loop; //переменная для хранения главного кол-ва циклов
	int c = 0; // итератор главных циклов

	int **massive; 
	massive = (int **)calloc(s,sizeof(int*));     //Выделение динамической памяти под массив указателей
	
	//for(int iter = 0;iter<s;iter++){				  //Выделение динамической памяти для массива значений
	//	massive[iter] = (int**)malloc(s*sizeof(int));
	//}

	if(massive != NULL){				  		  //Проверяем ,была ли выделена память
		printf("Память под в указателей ВЫДЕЛЕНА!\n");
	}else{printf("Память под массив указателей НЕ ВЫДЕЛЕНА!");}

	for(int iter = 0;iter<s;iter++){				  //Выделение динамической памяти для массива значений
		massive[iter] = (int*)calloc(s,sizeof(int));
	}

	if (massive[0]!=0){							  //Проверяем ,была ли выделена память
	printf("Память под массив значений ВЫДЕЛЕНА\n");
	}else{printf("Память под массив значений НЕ ВЫДЕЛЕНА!\n");}

	// Если был введен размер для матрицы четного кол-ва элементов строк,то количество главных циклов будет
	// равна размеру строки/столбца матрицы деленному на 2.
	// Если число строк/столбцов нечетное то,прибавим 1. 
	if(s%2){main_loop=s/2+1;}else{main_loop = s/2;}

	
	for(c;c<main_loop;c++){
	

		for(i;j<s-c;j++){
			massive[i][j] = val;
			val++;		
		}
    	i++;
    	j--;
		//printf("i[%d]  j[%d]\n",i,j);
		for(j;i<s-c;i++){
			massive[i][j] = val;
			val++;
		}

    	i--;
    	j--; 
    	//printf("i[%d]  j[%d]\n",i,j);
		for(i;j-c>=0;j--){
			massive[i][j] = val;
			val++;
		}
		i--;
		j++;
		//printf("i[%d]  j[%d]\n",i,j);
		for(j;i-c>0;i--){
			massive[i][j] = val;
			val++;
		}
		i++;
		j++;
		//printf("i[%d]  j[%d]\n",i,j);
	}
	
	for (int iter = 0; iter < s; iter++) {
		for(int iter1 = 0;iter1<s;iter1++){
			printf("%d\t",massive[iter][iter1]);
		}
		printf("\n");
    }

    /*for (int iter = 0; iter < s; iter++) {   //смотрел как в памяти располагается двумерный массив выделенный динамически
		for(int iter1 = 0;iter1<s;iter1++){
			printf("&massive[%d][%d] = %p\t",iter,iter1,&massive[iter][iter1]);
			printf("\n");
		}
    }*/

    for(int iter = 0;iter<s;iter++){		  //Освобождаем память
		free(massive[iter]);
	}

	free(massive);


}