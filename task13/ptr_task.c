#include "stdio.h"


/* изменить значение указателя ptr1 через указатель ptr2 так,чтобы он указывал не на 'A' ,а на 'B'  */
void main(){
	char a = 'A';
	char b = 'B';
	char *ptr1;
	char **ptr2;

	ptr1 = &a;

	ptr2 = &ptr1;

	printf("ptr2\t = %p\n",ptr2);	// значения указателей
	printf("ptr1\t = %p\n",ptr1);
	

	printf("**ptr2\t = %c\n",**ptr2); // достучаться до значений на которые указывают указатели
	printf("*ptr1\t = %c\n",*ptr1);
	printf("*ptr2\t = %p\n",*ptr2);
	

	printf("&ptr1\t = %p\n",&ptr1);	// адреса самих указателей
	printf("&ptr2\t = %p\n",&ptr2);
	printf("&a\t = %p\n",&a);

	*ptr2 = &b;
	printf("**ptr2\t = %c\n",**ptr2);
	printf("*ptr1\t = %c\n",*ptr1);

	

}