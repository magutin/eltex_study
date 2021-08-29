#include "src/calc.h"


void main(){
	char chosen_plug[4] = {'5','5','5','5'};
	void (*ptr)();
	char func_in_lib[4][20];						// массив символов функции
    char lib_name[num_plag][40];					// двумерный массив найденных библиотек
    struct dirent **namelist;						// структура для поиска имен в директории
    void *ptr_plug[num_plag];						// массив указателей на библиотеки
    float (*ptr_func[num_plag])(float,float);		// массив указателей на найденные функции
    int i = 0;
    int n = 0;
    int our_n = n;

    // поиск доступных плагинов по именам
    printf("%s - Looking for plagins ...\n",green);
    n = scandir("project/so", &namelist, 0, alphasort); //сканируем папку
    if (n < 0)								    //если ничего не нашли,то ошибка
        perror("scandir");
    else { 										//иначе продолжаем
	
	printf(" - Found %s%d %s plagins ...\n",fiol,n-2,green);     //найдено имен в папке n-2 (исключаем "." и "..")
	
	int our_n = n-1;
	        
        for(our_n ;our_n!=1;our_n--) {						//копируем имена найденны файлов в структуру
            strncpy(lib_name[i],namelist[our_n]->d_name,40);
            i++;
        }

        while(n--){									//очищаем структуру найденных имен в папке so
			free(namelist[n]);
		}
		free(namelist);
	
        printf("\n%s - Plagins list ...\n",RESET);
        printf("Num\tName\n");
        for (i = 0;i<num_plag;i++){				    // выводим на экран найденные библиотеки
        	printf("%d\t%s\n",i,lib_name[i]);
        }
        printf("\n");
       
	
    }

    // Добавление плагинов выбранных пользователем
    i = 0;
    char chose = 0;
    while(i!=5){
       printf("%sWhich plugin to add?%s[number of plagin] / [ 5 - for exit]%s: %s",green,fiol,green,RESET);
	   scanf("%d",&i);
	   char actn;
	   do {actn = getchar();} while (actn != '\n');				//очистка входного потока,красивее варианта не нашел
	   switch(i){
	   		case 0:	
	   				printf("%s - Plugin [%s%s%s] added\n",green,fiol,lib_name[i],green);
	   				chosen_plug[i] = i;
	   				break;
	   		case 1:
	   				printf("%s - Plugin [%s%s%s] added\n",green,fiol,lib_name[i],green);
	   				chosen_plug[i] = i;
	   				break;
	   		case 2:
	   				printf("%s - Plugin [%s%s%s] added\n",green,fiol,lib_name[i],green);
	   				chosen_plug[i] = i;
	   				break;
	   		case 3:
	   				printf("%s - Plugin [%s%s%s] added\n",green,fiol,lib_name[i],green);
	   				chosen_plug[i] = i;
	   				break;
	   		case 5:
	   				printf("%s - Exit...\n\n",green);
	   				break;
	   		default:
	   				printf("%s - ERR! There is no such plugin...%s\n",RED,RESET);
	   				break;
	   }
	   
    }
    
    // открываем выбранные плагины
    printf("%s - Opening dll ...\n",green);
    for (i = 0;i<num_plag;i++){
    	if(chosen_plug[i] != '5'){
    		ptr_plug[i] = dlopen(lib_name[i],RTLD_NOW);   	// открываем библиотеку
			if(ptr_plug[i] == NULL){			// проверяем открылась ли она
				fprintf(stderr,"%s\n",dlerror());
				return;
			}
			printf(" - Lib [%s%s%s] is open ...\n",fiol,lib_name[i],green);
    	}
    }
    printf("\n");

    // ищем функции внутри плагинов 
    printf("%s - Find name functions ..\n",green);
    for(i = 0;i<num_plag;i++){
    	if(chosen_plug[i] != '5'){
    	ptr = dlsym(ptr_plug[i], "lib_func");
			if(ptr == NULL){
				fprintf(stderr,"%s\n",dlerror());
			}else{
				ptr(&func_in_lib[i]);
				ptr_func[i] = (float(*)(float,float))dlsym(ptr_plug[i], func_in_lib[i]);
				if(ptr_func[i] == NULL){
					fprintf(stderr,"%s\n",dlerror());	
				}else{
				printf("Function [%s%s%s] found in lib [%s%s%s], her num [%s%d%s]\n"
					,fiol,func_in_lib[i],green,fiol, lib_name[i],green,fiol, i,green);
				//float rez = ptr_func[i](1.0,2.0);
				}	
			}
		}
    }
    printf("\n");

    // запуск калькулятора,вывод интерфейса
    printf("%s-------Calculator-------\n",lazur);				// вывод интерфейса
    for(i = 0;i<num_plag;i++){
    	if(chosen_plug[i] != '5'){
    		printf(" %d - %s. \n",i,func_in_lib[i]);	
    	}	
    }   
    printf(" 5 - exit.\n"); 
    printf("------------------------\n");
    i = 0;
    while(i!=5){
    	printf(" - Chose operation: ");
    	scanf("%d",&i);
    	char actn;
    	do {actn = getchar();} while (actn != '\n');				//очистка входного потока,красивее варианта не нашел
    	if(i != 5){													// условие для выхода
    		if(chosen_plug[i] != '5' && chosen_plug[i]==i){
    			printf(" - You chose %s \n - Enter first value: ",func_in_lib[i]);
    			float first_val = 0.0;
    			float second_val = 0.0;
    			float rez;
    			scanf("%f",&first_val);
    			printf(" - Enter first value: ");
    			scanf("%f",&second_val);
    			do {actn = getchar();} while (actn != '\n');				//очистка входного потока,красивее варианта не нашел
    			rez = ptr_func[i](first_val,second_val);
    			printf(" - Answer = %f\n",rez);	
    		}
    		else{ printf("%s - ERR! This function wasn't exist\n%s",RED,lazur);}	
    	}
    	else{printf(" - Exit ...\n\n");}
    	
    }

    // освобождение библиотек
    for(i=0;i<num_plag;i++){
    	if(chosen_plug[i]!='5'){
    		printf("%s - Close lib [%s%s%s] ...\n",green,fiol,lib_name[i],green);
    		dlclose(ptr_plug[i]);
    	}
    }
    printf("\n%s - Programm close ...\n",RESET);
	 
}
