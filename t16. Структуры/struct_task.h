#ifndef STRUCT_TASK_H
#define STRUCT_TASK_H

#include <stdio.h>
#include <wchar.h>
#include <malloc.h>
#include <string.h>

#include <stdlib.h>

/* реализация справочника через односвязный список */
typedef struct phone_directory{
	int id_abonent;					// Идентификатор абонента
	char name[30];					// Имя абонента не больше 20 сиволов
	char number[12];				// Номер абонента (стандартные 11 цифр)
	struct phone_directory *pNext;  // Указатель на следующий элемент списка
}strct_ph_dir;


strct_ph_dir *init_directotry(int a);					//инициализация списка
void print_list(strct_ph_dir *pStruct);					//вывод списка
strct_ph_dir *interface(strct_ph_dir *pStruct);			//меню интерфейса
strct_ph_dir *add_abonent(strct_ph_dir *pStruct);		//добавить абонента в справочник
void del_elem(int _id,strct_ph_dir *pStruct_root);		//удалить абонента из справочника
strct_ph_dir *del_root(strct_ph_dir *pStruct_root);		//удаление корневого абонента

#endif