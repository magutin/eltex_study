#include "calc.h"

float division(float _var1,float _var2){
	return (_var1 / _var2);
}

void lib_func(char *_str){
	char func_name[8] = {'d','i','v','i','s','i','o','n'};
	strcpy(_str,func_name);
}