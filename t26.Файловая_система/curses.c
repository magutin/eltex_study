#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>



struct winsize size;

void clear_wind();										//Очистка экрана
void sig_winch(int signo); 								//изменение размера
void create_file(char *fname, struct winsize size);	//Открытие файла
void open_file(char *fname, struct winsize size);	//Открытие файла
int menu();												//Интерфейс



int main()
{
	char t;
	int fdesc;
	char fname[30] = "text.txt";
	chtype line_txt[80];
	int chek;
	int symb;
	char ex = '1';
	initscr();						// инициализируем структуры данных ncurses
	signal(SIGWINCH, sig_winch);	// обработка сигнала изменения размера окна
	cbreak();						// не дожидаемся нажатия enter
	noecho();						// отключение вводимых символов с клавиутаруы. полагаю обратная echo();
	start_color();
	curs_set(0);					// видимость курсора 0-невидимый
	refresh();	
	ioctl(fileno(stdout), TIOCGWINSZ, &size);
	keypad(stdscr, true);

	//	вызвали меню
	t = '1';
	move(size.ws_row/2-1, size.ws_col/2-15);
	printw("Press any key for continue...");
	while(t!='0'){

		switch(menu()){
			// анализ ключей
			case KEY_F(1):	//создаем файл
					clear_wind(5,0);  //очищаем окно
					refresh();
					//curs_set(1);
					//nocbreak();
					//fgets(fname, 30, stdin);
					create_file(fname,size);					
					break;

			case KEY_F(2):	// открываем файл
					clear_wind(5,0);  //очищаем окно
					refresh();
					open_file(fname,size);
					break;

			case KEY_F(3):
					clear_wind(5,0);	//очищаем окно
					refresh();
					move(size.ws_row, 0);
					//S_IRWXO
					fdesc = open(fname, O_WRONLY);
					if(fdesc == -1){
						move(0, 0);
						printw("MSG: The file [%s] doesn't exist.",fname);
						move(1, 0);
						printw("MSG: First, create a file.",fname);
					}
					else {
						for(int s = 0; s < size.ws_row - 2; s++){
							mvinchnstr(s, 0, line_txt, 79);// Считывание s строки из 79 символов в буффер str
							for(int i = 0; i < 80; i++) {
								chek = write(fdesc, &line_txt[i], 1);
								if(chek == -1) {
									move(10,0);
									printw("MSG: Error of input to file");
								}
							}
						}
						close(fdesc);
						move(0, 0);
						printw("MSG: File [%s] saved", fname);
				 		clear_wind(5,0);
						refresh();
					}
					move(0, 0);
					insdelln(5); // Функция сдвигает текст вниз и добовляет пустую строку
					break;
		case KEY_F(5):
				clear_wind(5,0);
				refresh();
				move(0,0);
				printw("MSG: Programm close. Press any key for exit ...");
				t = '0';
				break;
		}
	}
	
	getch();
	endwin();			// для закрытия окна (завершение initscr())
	exit(EXIT_SUCCESS);
}



void create_file(char *fname, struct winsize size) {
	int fdesc = open(fname, O_CREAT,S_IRWXU);   
	if(fdesc<=0){
		clear_wind(5,0);  //очищаем окно
		move(size.ws_row - 3,0);
		printw("MSG: ERR[%d] Cannot create file [%s].",fdesc,fname);
	}else{
		clear_wind(5,0);  //очищаем окно
		move(size.ws_row - 3,0);
		printw("MSG: DONE! File [%s] create.",fname);
	}
	close(fdesc);
}

void open_file(char *fname, struct winsize size) {
	FILE *fdesc = fopen(fname,"r");
	char str[81];
	char sym;
	int i = 0;
	char check = '1';
	curs_set(1);   
	if(fdesc<=0){
		clear_wind(5,0);  //очищаем окно
		move(size.ws_row - 3,0);
		printw("MSG: ERR[%d] Cannot open file [%s].",fdesc,fname);
	}else{
		move(size.ws_row - 3,0);
		printw("MSG: Reding file [%s].",fname);
		while (fgets(str, 81, fdesc) != NULL){
			move(i,0);	
			printw("%s\n", str);
			i++;
		}
		fclose(fdesc);	
	
	}
	fdesc = fopen(fname,"a");
	curs_set(1);   
	if(fdesc<=0){
		clear_wind(5,0);  //очищаем окно
		move(size.ws_row - 3,0);
		printw("MSG: ERR[%d] Cannot open file [%s].",fdesc,fname);
	}else{
		move(size.ws_row - 4,0);
		printw("MSG: DONE! File [%s] open. ",fname);
		move(size.ws_row - 3,0);
		printw(" -INSERT- (esc - exit with saving). ");
		int x = i;
		int y = 0;
		move(x,y);
		cbreak();
		noecho();
  		keypad(stdscr, true);
		refresh();
		while(check =='1'){
			sym = getch();

			switch(sym){
				case KEY_UP:
							if(x > 0){
								x--;
								move(x, y);	
							}
							break;

				case KEY_DOWN:
							if(x < (size.ws_row - 1)){
								x++;
								move(x, y);	
							}
							break;

				case KEY_LEFT:
							if(y > 0){
								y--;
								move(x, y);
							}
							break;

				case KEY_RIGHT:
							if(y < (size.ws_col - 1))
							y++;
							move(x, y);
							break;
			
				case 27: // Esc
							check ='0';
							break;
				
				case KEY_BACKSPACE:
							y--;
							move(x, y);
							delch();// функция удаляет симовол на котором стоит коретка
							
							move(x, y);
							break;

				case 10:
							x++;
							y = 0;
							move(x, 0);
							break;
				default :
							insch(sym);// ввод обычных символов
							y++;
							move(x, y);
							break;
				}
			move(size.ws_row - 3 , size.ws_col-25);
			printw("line|row - (%d | %d)", x, y);
			move(x, y);
			refresh();
			putc(sym,fdesc);
				
				
			}

		}


		fclose(fdesc);
			clear_wind(2,0);
	//		return 0;
	}
	




void clear_wind(int row, int col){
	for(int i = 0; i < size.ws_row - row; i++) {
		for(int j = 0; j < size.ws_col - col; j++){
			move(i, j);	
			printw(" ");
		}
	}
}

int menu(){
	int cmd=0;
	curs_set(0);

	//move(size.ws_row - 2, size.ws_col-23);
	for(int j = 0; j < size.ws_col ; j++){
		move(size.ws_row - 2, j);	
		printw("-");
	}
	move(size.ws_row-1,size.ws_col/2-33);
	printw(" | Create file - f1 | Edit exist file - f2 | Exit programm - f5 | ");

	cmd = getch();
	refresh();	
	return cmd;
}

void sig_winch(int signo){ // обработка сигнала изменения размера окна
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, &size); //Необходимые для resizeterm() значения размеров окна мы получаем с помощью специального вызова ioctl()
	resizeterm(size.ws_row, size.ws_col);			   //resizeterm() следует вызывать сразу после изменения размеров окна терминала
	clear_wind(0,0);
	int a = menu();
}