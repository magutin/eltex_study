################################################################################
# s.magutin 2021
# 
# Файл для автоматической сборки проекта.
# $(SRV_DIR/CLN_DIR)  			- директория исходных файлов сервера и клиента.
# $(MAIN_SRV/MAIN_CLN)			- файлы с функцией main 
# $(SOURCES_SRV/SOURCES_CLN)	- Имена исходных файлов
# $(HEADERS)					- Имена заголовчных файлов
# $(CC)      		  			- полное имя компилятора.
# $(FILENAME_CLN/SRV) 			- имя исполняемого файла
################################################################################
# Пути и инициализация файлов

#BASE_DIR := project/
SRV_DIR := $(BASE_DIR)server/
CLN_DIR := $(BASE_DIR)client/

# файл main
MAIN_SRV := server.c
MAIN_CLN := client.c

# файлы исходных ресурсных файлов
SOURCES_SRV := srv_func sockets 
SOURCES_CLN := cln_func test_sock

# файлы заголовочников файлов
HEADERS := heads

FILENAME_CLN := cln
FILENAME_SRV := srv

CC := gcc

################################################################################
# Формирование списков необходимых файлов

# Список *.c файлов srv
C_FILES_SRV := $(addsuffix .c,$(SOURCES_SRV)) 
C_TAMPLATE_SRV := $(addprefix $(SRV_DIR),$(C_FILES_SRV))

# Список *.h файлов
H_FILES_SRV := $(addsuffix .h,$(HEADERS)) 
H_TAMPLATE_SRV := $(addprefix $(SRV_DIR),$(H_FILES_SRV))

# Список .о файлов
O_FILES_SRV := $(addsuffix .o,$(SOURCES_SRV)) 
O_TAMPLATE_SRV := $(addprefix $(SRV_DIR),$(O_FILES_SRV))


# Список *.c файлов srv
C_FILES_CLN := $(addsuffix .c,$(SOURCES_CLN)) 
C_TAMPLATE_CLN := $(addprefix $(CLN_DIR),$(C_FILES_CLN))

# Список *.h файлов
H_FILES_CLN := $(addsuffix .h,$(HEADERS)) 
H_TAMPLATE_CLN := $(addprefix $(CLN_DIR),$(H_FILES_CLN))

# Список .о файлов
O_FILES_CLN := $(addsuffix .o,$(SOURCES_CLN)) 
O_TAMPLATE_CLN := $(addprefix $(CLN_DIR),$(O_FILES_CLN))


################################################################################
#Сценарии

.PHONY: clean all build_srv build_cln run_srv run_cln

# Сборка исполняемого файла
all:
	@	echo - Compiling start!   
	@	echo - Building server.. 
	@	$(CC) $(C_TAMPLATE_SRV) -c
	@	$(CC) $(SRV_DIR)$(MAIN_SRV) $(C_TAMPLATE_SRV) -o $(FILENAME_SRV) -pthread -lncurses 
	@	rm -rf $(O_FILES_SRV)
	@	echo - Complete
	@	echo - Building client.. 
	@	$(CC) $(C_TAMPLATE_CLN) -c
	@	$(CC) $(CLN_DIR)$(MAIN_CLN) $(C_TAMPLATE_CLN) -o $(FILENAME_CLN) -pthread
	@   rm -rf $(O_FILES_CLN)
	@	echo - Complete
	@	echo - Compiling complete!   

# Очистка
clean:
	@	echo - Cleaning...
	@	rm -rf $(FILENAME_CLN) 
	@	rm -rf $(FILENAME_SRV) 
	@	echo - Cleaning Complete...

# Помощь
help:
	@	echo make -f Makefile [options]
	@	echo "\t"[options]
	@	echo 	"\t"all:"\t\t"		- Compiling client and server.
	@	echo 	"\t"build_srv:"\t"	- Compiling server.
	@	echo 	"\t"build_cln:"\t"	- Compiling client.
	@	echo 	"\t"clean:"\t\t"    - Cleaning files .elf .o client and server.
	@	echo 	"\t"run_cln/run_srv: - Running

# Запуск
run_srv:		
		sudo ./srv 127.0.0.1
run_cln:		
		sudo ./cln
