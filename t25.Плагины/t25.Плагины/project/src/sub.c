#include "calc.h"

float subtraction(float _var1,float _var2){
	return (_var1 - _var2);
}

void lib_func(char *_str){
	char func_name[11] = {'s','u','b','t','r','a','c','t','i','o','n'};
	strcpy(_str,func_name);
}
