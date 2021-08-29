################################################################################
# s.magutin 2021
# 
# Файл для автоматической сборки проекта.
# $(BASE_DIR)        - корневая директория
# $(SRC_DIR)         - директория исходных файлов.
# $(DLIB_DIR)        - директория динамических библиотек
# $(OBJ_DIR)         - директория объектных файлов
#
# $(CC)      		 - полное имя компилятора.
# $(OUTPUT_FILENAME) - имя исполняемого файла
################################################################################
# Пути и инициализация файлов

BASE_DIR := project/
DLIB_DIR := $(BASE_DIR)so/
SRC_DIR := $(BASE_DIR)src/
OBJ_DIR := $(BASE_DIR)obj/

#LIB_PATH := $(LD_LIBRARY_PATH)


# файл main
MAIN := calculator.c

# файлы исходных ресурсных файлов
SOURCES :=  add sub div mul

# файлы библиотек файлов
DLIBS := mul sub add div

# файлы заголовочников файлов
HEADERS := calc

OUTPUT_FILENAME := task25
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
DL_FILES := $(addsuffix .so,$(DLIBS)) 
DL_FILES := $(addprefix lib,$(DL_FILES))
DL_TAMPLATE := $(addprefix $(DLIB_DIR),$(DL_FILES))



################################################################################
#Сценарии

.PHONY: lib_build clean all run

# Сборка исполняемого файла
all:
	@	echo - Compiling start!   
	@	echo - Building dlibs.. 
	@	$(CC) -fPIC $(C_TAMPLATE) -c 
	@	echo - Localization objects files...
	@	mv $(O_FILES) -t $(OBJ_DIR)
	@	$(CC) -shared $(OBJ_DIR)add.o -o libadd.so
	@	$(CC) -shared $(OBJ_DIR)mul.o -o libmul.so
	@	$(CC) -shared $(OBJ_DIR)div.o -o libdiv.so
	@	$(CC) -shared $(OBJ_DIR)sub.o -o libsub.so
	@	mv $(DL_FILES) -t $(DLIB_DIR) 
	@	echo - Dlibs collected...
	@	echo - Building elf file...
	@	$(CC) $(BASE_DIR)$(MAIN) $(DL_TAMPLATE) -o $(OUTPUT_FILENAME)  -ldl -Wl,-rpath,./project/so/
	@	echo - Elf file collected...
	@	echo - Localization lib files...
	@	echo - Сompiling сomplete!

# Очистка
clean:
	@	echo Cleaning...
	@	rm -rf $(OUTPUT_FILENAME)
	@	rm -rf $(O_TAMPLATE)
	@	rm -rf $(DL_TAMPLATE)
	@	echo Cleaning Complete...

# Сборка библиотек
dlib_build: 
	@	echo - Building dlib.. 
	@	$(CC) -fPIC $(C_TAMPLATE) -c 
	@	echo - Localization objects files...
	@	mv $(O_FILES) -t $(OBJ_DIR)
	@	$(CC) -shared $(OBJ_DIR)add.o -o libadd.so
	@	$(CC) -shared $(OBJ_DIR)mul.o -o libmul.so
	@	$(CC) -shared $(OBJ_DIR)div.o -o libdiv.so
	@	$(CC) -shared $(OBJ_DIR)sub.o -o libsub.so
	@	mv $(DL_FILES) -t $(DLIB_DIR) 
	@	echo - Dlibs collected...

help:
	@	echo make -f Makefile [options]
	@	echo "\t"[options]
	@	echo 	"\t"all:"\t\t"	   Compiling project.
	@	echo 	"\t"clean:"\t\t"	   Cleaning files .elf .o .a.
	@	echo 	"\t"dlib_build:"\t" Dynamic library collecting.
	@	echo 	"\t"run:"\t\t" Running program LD_LIBRARY_PATH = $PWD:$LD_LIBRARY_PATH ./task24 
run:		
	@	LD_LIBRARY_PATH=$(CURDIR)/$(DLIB_DIR) ./$(OUTPUT_FILENAME)
