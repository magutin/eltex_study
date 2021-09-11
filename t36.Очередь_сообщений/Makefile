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
CLN := client.c
CLN2 := client2.c
SRV := server.c

OUTPUT_CLN := cln
OUTPUT_CLN2 := cln2
OUTPUT_SRV := srv

CC := gcc

################################################################################
#Сценарии

.PHONY: lib_build clean all

# Сборка исполняемого файла
all:
	@	echo - Compiling start!    
	@	$(CC) $(CLN) -o $(OUTPUT_CLN) -lpthread
	@	echo - Client compiling complete
	@	$(CC) $(CLN2) -o $(OUTPUT_CLN2) -lpthread
	@	echo - Client2 compiling complete
	@	$(CC) $(SRV) -o $(OUTPUT_SRV)
	@	echo - Server compiling complete
	@	echo - Сompiling сomplete!

# Очистка
clean:
	@	echo Cleaning...
	@	rm -rf $(OUTPUT_CLN)
	@	rm -rf $(OUTPUT_CLN2)
	@	rm -rf $(OUTPUT_SRV)
	@	echo Cleaning Complete...

help:
	@	echo make -f Makefile [options]
	@	echo "\t"[options]
	@	echo "\t"all:"\t\t"	   -Compiling project.
	@	echo "\t"clean:"\t\t"	   -Cleaning files 