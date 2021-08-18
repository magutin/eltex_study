#include "struct_task.h"

strct_ph_dir *interface(strct_ph_dir *pStruct){
	int exit = 1;
	
	int actn;
	int id = 0;
	strct_ph_dir *ptr = pStruct;

	printf("---------------Menu------------------\n");
	printf("1.Show directory \t- enter '1'.\n");
	printf("2.Add abonent \t\t- enter '2'.\n");
	printf("3.Delete abonent \t- enter '3'.\n");
	printf("4.Exit programm \t- enter '4'.\n");
	printf("-------------------------------------\n");
	

	do{
	
		printf("Action: ");

		scanf("%d",&actn);
		//
			
		
			

		switch(actn){
			case 1:
				print_list(pStruct);
				do {actn = getchar();} while (actn != '\n');				//очистка входного потока,красивее варианта не нашел
				break;

			case 2:
				ptr = add_abonent(ptr);
				if((ptr->pNext == NULL) && (ptr->id_abonent == 1)){
					pStruct = ptr;
					pStruct->pNext = NULL;
				}
				printf(" - Abonent added...\n"); 
				break;

			case 3:
				printf("Enter id abonent:");
				do {actn = getchar();} while (actn != '\n');
				scanf("%d",&id);
				if(id != 1){del_elem(id,pStruct);} //если удаляется не корень списка
				if(id == 1){											 //если удаляется корень списка
					pStruct = del_root(pStruct);
					if(pStruct == NULL){ ptr = NULL;}
				}
				printf(" - Abonent deleted...\n");
				break;

			case 4:
				exit = 0;
				do {actn = getchar();} while (actn != '\n');
				printf(" - Exit programm...\n"); 
				break;

			default:
				do {actn = getchar();} while (actn != '\n');
				printf(" - Action is not exist...\n");
				break;
			
		}
		
	}while(exit!=0);
	return pStruct;
}



//Инициализация списка
strct_ph_dir *init_directotry(int a){ // а- значение первого узла
  struct phone_directory *phone_direct;
  
  // выделение памяти под корень списка
  phone_direct = (strct_ph_dir*)malloc(sizeof(strct_ph_dir));
 	strncpy(phone_direct->name,"Sergey Magutin\n",15);
 	strncpy(phone_direct->number,"89824125111\n",12);
  phone_direct->id_abonent = a;
  phone_direct->pNext = NULL; // это последний узел списка

  return(phone_direct);
}


void print_list(strct_ph_dir *pStruct){
  strct_ph_dir *ptr;
  ptr = pStruct;
  if (pStruct != NULL){
  	printf("-------Abonets drectory!-------\n");
  	do {
    	printf("Id:\t%d\n", ptr->id_abonent);
    	printf("Name:\t%s",ptr->name);
    	printf("Number:\t%s\n",ptr->number);

    	ptr = ptr->pNext; // переход к следующему узлу
  	} while (ptr != NULL);
  	printf("--------------------------------\n");
  }
  else{printf("Directory is empty!\n");}
  
  //free(ptr);
}

strct_ph_dir *add_abonent(strct_ph_dir *pStruct)
{	
	 //забавный факт.Для затирания сивола конца строки от scanf();
	char eat_n[10];
  fgets(eat_n,sizeof eat_n,stdin);
  fflush(stdin);
  
  if(pStruct != NULL){
  	strct_ph_dir *temp, *ptr;
  	temp = (strct_ph_dir*)malloc(sizeof(strct_ph_dir));
  	ptr = pStruct->pNext; // сохранение указателя на следующий узел
  	pStruct->pNext = temp; // предыдущий узел указывает на создаваемый
  
  	temp->id_abonent = pStruct->id_abonent+1; // сохранение поля данных добавляемого узла
  	printf("Name: ");
  	fgets(temp->name,30,stdin);
  	fflush(stdin);
  	printf("Number: ");
  	fgets(temp->number,15,stdin);
  	fflush(stdin);
  
 		temp->pNext = ptr; // созданный узел указывает на следующий элемент
 		return temp;
  }
  else{
  	pStruct = (strct_ph_dir*)malloc(sizeof(strct_ph_dir));
  	//strncpy(pStruct->name,"Sergey Magutin\n",15);
 		//strncpy(pStruct->number,"89824125111\n",12);
 		printf("Name :");
  	fgets(pStruct->name,30,stdin);
  	fflush(stdin);
  	printf("Number :");
  	fgets(pStruct->number,15,stdin);
  	fflush(stdin);
   	pStruct->id_abonent = 1;
  	pStruct->pNext = NULL; // это последний узел списка
  	return pStruct;
  }

  
}

//strct_ph_dir *deletelem(strct_ph_dir *pStruct, list *pStruct_root)//удаляемый узел ,корневой узел
void del_elem(int _id,strct_ph_dir *pStruct_root){

  if(_id != 1){											//Если происходит удаление не корня
  	strct_ph_dir *temp,*pStruct;
  	pStruct = pStruct_root;
  	temp = pStruct_root;
  	
   	while(pStruct->id_abonent != _id){

  		if(pStruct->id_abonent != _id-1){
  			temp = pStruct->pNext;
  		}
  		pStruct = pStruct->pNext;

  	}
  	temp->pNext = pStruct->pNext;
  	//free(pStruct);

  	pStruct = pStruct_root->pNext;
  	if(pStruct!=NULL){
  		int i = pStruct_root->id_abonent;
  		do {
    
    		pStruct->id_abonent = ++i;
    		pStruct = pStruct->pNext; // переход к следующему узлу
  		} while (pStruct != NULL);	
  	}
  	free(pStruct);
  	//free(temp);
	}
}


strct_ph_dir *del_root(strct_ph_dir *pStruct_root)
{
  
  if (pStruct_root->pNext != NULL){
  	strct_ph_dir *temp, *pStruct;
  	temp = pStruct_root->pNext;	
  	free(pStruct_root); // освобождение памяти текущего корня
  
  	temp->id_abonent = 1;
  	int i = temp->id_abonent;
  	if(temp->pNext != NULL){
  		pStruct = temp->pNext;
  		do {
     			pStruct->id_abonent = ++i;
    		pStruct = pStruct->pNext; // переход к следующему узлу
  			} while (pStruct != NULL);	
  	}

  	return temp; // новый корень списка
  }
  else{
  	pStruct_root->pNext = NULL;
  	free(pStruct_root);
  	return NULL;
  }

  
}