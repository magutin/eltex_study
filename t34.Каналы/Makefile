################################################################################
# s.magutin 2021
# 
# Файл для автоматической сборки проекта.
# $(MAIN)      		 - файл с функцией main.
# $(CC)      		 - полное имя компилятора.
# $(OUTPUT_FILENAME) - имя исполняемого файла
################################################################################
# Пути и инициализация файлов

# файл main
MAIN := cmd_interp.c


OUTPUT_FILENAME := cmd
CC := gcc

################################################################################
#Сценарии

.PHONY: lib_build clean all

# Сборка исполняемого файла
all:
	@	echo - Compiling start!    
	@	$(CC) $(MAIN) -o $(OUTPUT_FILENAME)
	@	echo - Elf file collected...
	@	echo - Сompiling сomplete!

# Очистка
clean:
	@	echo Cleaning...
	@	rm -rf $(OUTPUT_FILENAME)
	@	echo Cleaning Complete...

help:
	@	echo make -f Makefile [options]
	@	echo "\t"[options]
	@	echo "\t"all:"\t\t"	   -Compiling project.
	@	echo "\t"clean:"\t\t"	   -Cleaning files .elf .o .a.
	@	echo "\t"run:"\t\t" -Running program.

run:
	@	./$(OUTPUT_FILENAME)