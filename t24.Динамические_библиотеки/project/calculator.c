#include "src/calc.h"
#include <stdio.h>
#include <stdbool.h>

#define true 1
#define false 0

void main(){
	float var1 = 0;
	float var2 = 0;
	float rezult = 0;
	short operation = 0;
	bool exit = true;

	printf("---------------------\n");
	printf("|     Calulator     |\n");
	printf("---------------------\n");
	printf("| 1 - addition      |\n");
	printf("| 2 - subtraction   |\n");
	printf("| 3 - multiplication|\n");
	printf("| 4 - division      |\n");
	printf("---------------------\n");
	printf("| 5 - exit          |\n");
	printf("---------------------\n");

	while(exit){
		printf("Enter operation: ");
		scanf("%hu",&operation);
		if( (var1 == 0 ) && (var2 ==  0) && (operation!=5) ){
			printf("Enter value 1: ");
			scanf("%f",&var1);
			printf("Enter value 2: ");
			scanf("%f",&var2);
		}
		
		switch(operation){
			case 1:	
					rezult = addition(var1,var2);
					printf("%f + %f = %f\n",var1,var2,rezult);
					var1 = 0;
					var2 = 0;
					break;

			case 2:
					rezult = subtraction(var1,var2);
					printf("%f - %f = %f\n",var1,var2,rezult);
					var1 = 0;
					var2 = 0;
					break;

			case 3:
					rezult = multiplication(var1,var2);
					printf("%f * %f = %f\n",var1,var2,rezult);
					var1 = 0;
					var2 = 0;
					break;
					
			case 4:
					rezult = division(var1,var2);
					printf("%f / %f = %f\n",var1,var2,rezult);
					var1 = 0;
					var2 = 0;
					break;

			case 5:
					printf(" - EXIT\n");
					exit = false;
					break;
					
			default:
					printf("ERR: Unknown operation\n");
					break;
		}
		
	}
}
