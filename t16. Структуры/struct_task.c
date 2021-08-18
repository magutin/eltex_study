#include "struct_task.h"

void main(){
	strct_ph_dir *phone_dir;

	phone_dir = init_directotry(1);
	phone_dir = interface(phone_dir);

	//free(phone_dir);
	do
	{
    	strct_ph_dir *tmp = phone_dir;
    	phone_dir = phone_dir->pNext;
    	free(tmp);
	}while(phone_dir->pNext!=NULL);
	free(phone_dir);
}
