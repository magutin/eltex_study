#include "stdio.h"
#include "malloc.h"
#include "stdlib.h"
#include "locale.h"

#define s3 4

void task1(unsigned int s,int **massive); //Задание 1: Вывести квадратную матрицу по заданному N
void task2(unsigned int s,int *massive); //Задание 2: Вывести массив размером N в обратном порядке
void task3(unsigned int s,int (*massive)[s3]); //Задание 3: Заполнить верхний треугольник матрицы 3х3 '1', а нижний '0'
void task4(unsigned int s);
void main(){
	setlocale(LC_ALL,"RUS");
	unsigned int size;	//перемнная для задания кол-ва элементов массива
	int **array;		//для двумерного массива
	int *array1;		//для одномерного массива
	int array2[s3][s3] = {{0,0,1,1},{0,1,1,1},{1,1,1,1}}; //для задания 3
	

	printf("Задание 1: Вывести квадратную матрицу по заданному N\n");
	printf("Введите размер для двумерного массива array[N][N]: N = ");
	scanf("%d",&size);

	task1(size,array); //Задание 1: Вывести квадратную матрицу по заданному N
	
	printf("\n\nЗадание 2: Вывести массив размером N в обратном порядке\n");
	printf("Введите размер для массива array[N]: N = ");
	scanf("%d",&size);	
	task2(size,array1);

	printf("\n\nЗадание 3: Заполнить верхний треугольник матрицы 3х3 '1', а нижний '0'\n");
	task3(s3,array2);
	
	printf("\n\nЗадание 4: Вывести массив размерностью NxN ,который заполненен 'улиткой'\n");
	printf("Введите размер для двумерного массива array[N][N]: N = ");
	scanf("%d",&size);
	task4(size);

	
}

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

	for (iter = 0; iter < s; iter++) {
		for(iter1 = 0;iter1<s;iter1++){
			printf("%d\t",massive[iter][iter1]);
		}
		printf("\n");
		
    }
}


void task4(unsigned int s){

	int val = 1; //число которе,мы будем инкрементировать в массиве
	int i = 0;	 //итератор строк
	int j = 0;	 //итератор столбцов
	int main_loop; //переменная для хранения главного кол-ва циклов
	int c = 0; // итератор главных циклов

	int **massive; 
	massive = (int **)calloc(s,sizeof(int*));     //Выделение динамической памяти под массив указателей

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
	// равно размеру строки/столбца матрицы деленному на 2.
	// Если число строк/столбцов нечетное то,прибавим 1. 
	if(s%2){main_loop=s/2+1;}else{main_loop = s/2;}

	
	for(c;c<main_loop;c++){
	

		for(i;j<s-c;j++){
			massive[i][j] = val;
			val++;		
		}
    	i++;
    	j--;

		for(j;i<s-c;i++){
			massive[i][j] = val;
			val++;
		}

    	i--;
    	j--; 
    
		for(i;j-c>=0;j--){
			massive[i][j] = val;
			val++;
		}
		i--;
		j++;
	
		for(j;i-c>0;i--){
			massive[i][j] = val;
			val++;
		}
		i++;
		j++;
	}
	
	for (int iter = 0; iter < s; iter++) {
		for(int iter1 = 0;iter1<s;iter1++){
			printf("%d\t",massive[iter][iter1]);
		}
		printf("\n");
    }

    for(int iter = 0;iter<s;iter++){		  //Освобождаем память
		free(massive[iter]);
	}

	free(massive);


}