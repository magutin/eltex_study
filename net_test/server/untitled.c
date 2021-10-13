/*case 0x08:
				len = strlen(cmd);
				getsyx(x,y);
				for(k=0;k<len;k++){
					printf("\b");
					delch();
				}
				
				
				i = i-1;
				cmd[i] = ' '; 
				setsyx(x,y-2);


				printf("%s",cmd);
				break;*/
			case KEY_BACKSPACE:
				//move(x, y);
				printf("\b");
				delch();// функция удаляет симовол на котором стоит коретка
				printf("\b");
				printf("%s",cmd);
				break;