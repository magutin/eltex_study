################################################################################
# s.magutin 2021
# 
# Файл для автоматической сборки проекта.
# $(BASE_DIR)        - корневая директория
# $(SRC_DIR)         - директория исходных файлов.
# $(LIB_DIR)         - директория библиотек
# $(OBJ_DIR)         - директория объектных файлов
#
# $(CC)      		 - полное имя компилятора.
# $(OUTPUT_FILENAME) - имя исполняемого файла
################################################################################
# Пути и инициализация файлов

BASE_DIR := project/
LIB_DIR := $(BASE_DIR)libs/
SRC_DIR := $(BASE_DIR)src/
OBJ_DIR := $(BASE_DIR)obj/

# файл main
MAIN := calculator.c

# файлы исходных ресурсных файлов
SOURCES :=  calc

# файлы библиотек файлов
LIBS := my_lib

# файлы заголовочников файлов
HEADERS := calc

OUTPUT_FILENAME := task23
CC := gcc


################################################################################
# Формирование списков необходимых файлов

# Список *.c файлов
C_FILES := $(addsuffix .c,$(SOURCES)) 
C_TAMPLATE := $(addprefix $(SRC_DIR),$(C_FILES))

# Список *.h файлов
H_FILES := $(addsuffix .h,$(HEADERS)) 
H_TAMPLATE := $(addprefix $(SRC_DIR),$(H_FILES))

# Список .о файлов
O_FILES := $(addsuffix .o,$(SOURCES)) 
O_TAMPLATE := $(addprefix $(OBJ_DIR),$(O_FILES))

# Список библиотек
L_FILES := $(addsuffix .a,$(LIBS)) 
L_FILES := $(addprefix lib,$(L_FILES))
L_TAMPLATE := $(addprefix $(LIB_DIR),$(L_FILES))



################################################################################
#Сценарии

.PHONY: lib_build clean all

# Сборка исполняемого файла
all:
	@	echo - Compiling start!   
	@	echo - Building libs.. 
	@	$(CC) $(C_TAMPLATE) -c 
	@	echo - Localization objects files...
	@	mv $(O_FILES) -t $(OBJ_DIR)
	@	ar rc  $(L_FILES) $(O_TAMPLATE)
	@	echo - Libs collected...
	@	echo - Building elf file...
	@	cd $(LIB_DIR)
	@	$(CC) $(BASE_DIR)$(MAIN) -o $(OUTPUT_FILENAME)  -L. -l$(LIBS)
	@	echo - Elf file collected...
	@	echo - Localization lib files...
	@	mv $(L_FILES) $(LIB_DIR)
	@	echo - Сompiling сomplete!

# Очистка
clean:
	@	echo Cleaning...
	@	rm -rf $(OUTPUT_FILENAME)
	@	rm -rf $(O_TAMPLATE)
	@	rm -rf $(L_TAMPLATE)
	@	echo Cleaning Complete...

# Сборка библиотек
lib_build: 
	@	echo - Building libs.. 
	@	$(CC) $(C_TAMPLATE) -c 
	@	echo - Localization objects files...
	@	mv $(O_FILES) -t $(OBJ_DIR)
	@	ar rc  $(L_FILES) $(O_TAMPLATE)
	@	echo - Localization lib files...
	@	mv $(L_FILES) $(LIB_DIR)
	@	echo - Libs collected...

help:
	@	echo make -f Makefile [options]
	@	echo "\t"[options]
	@	echo 	"\t"all:"\t\t"	   Compiling project.
	@	echo 	"\t"clean:"\t\t"	   Cleaning files .elf .o .a.
	@	echo 	"\t"lib_build:"\t" Library collecting.
	@	echo 	"\t"run:"\t\t" Running program.
run:
		./$(OUTPUT_FILENAME)