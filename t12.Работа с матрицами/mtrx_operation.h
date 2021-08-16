#ifndef MTRX_OP_H
#define MTRX_OP_H

#include "stdio.h"
#include "malloc.h"
#include "stdlib.h"
#include "locale.h"

#define s3 4

void task1(unsigned int s,int **massive); 	   //Задание 1: Вывести квадратную матрицу по заданному N
void task2(unsigned int s,int *massive); 	   //Задание 2: Вывести массив размером N в обратном порядке
void task3(unsigned int s,int (*massive)[s3]); //Задание 3: Заполнить верхний треугольник матрицы 3х3 '1', а нижний '0'
void task4(unsigned int s);					   //Задание 4: Вывести массив размерностью NxN ,который заполненен 'улиткой'


#endif