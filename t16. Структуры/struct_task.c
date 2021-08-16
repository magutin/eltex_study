#include "struct_task.h"

void main(){
	strct_ph_dir *phone_dir;

	phone_dir = init_directotry(1);
	phone_dir = interface(phone_dir);

	free(phone_dir);
}
