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
SOURCES :=  calc

# файлы библиотек файлов
DLIBS := my_dlib

# файлы заголовочников файлов
HEADERS := calc

OUTPUT_FILENAME := task24
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
	@	$(CC) $(C_TAMPLATE) -c 
	@	echo - Localization objects files...
	@	mv $(O_FILES) -t $(OBJ_DIR)
	@	$(CC) -shared $(O_TAMPLATE) -o $(DL_FILES)
	@	echo - Dlibs collected...
	@	echo - Building elf file...
	@	cd $(DLIB_DIR)
	@	$(CC) $(BASE_DIR)$(MAIN) -o $(OUTPUT_FILENAME)  -L. -l$(DLIBS)
	@	echo - Elf file collected...
	@	echo - Localization lib files...
	@	mv $(DL_FILES) $(DLIB_DIR)
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
	@	$(CC) fPIC $(C_TAMPLATE) -c 
	@	echo - Localization objects files...
	@	mv $(O_FILES) -t $(OBJ_DIR)
	@	$(CC) -shared $(O_TAMPLATE) -o $(DL_FILES)
	@	echo - Localization dlib files...
	@	mv $(DL_FILES) $(DLIB_DIR)
	@	echo - Dlib collected...

help:
	@	echo make -f Makefile [options]
	@	echo "\t"[options]
	@	echo 	"\t"all:"\t\t"	   Compiling project.
	@	echo 	"\t"clean:"\t\t"	   Cleaning files .elf .o .a.
	@	echo 	"\t"dlib_build:"\t" Dynamic library collecting.
	@	echo 	"\t"run:"\t\t" Running program LD_LIBRARY_PATH = $PWD:$LD_LIBRARY_PATH ./task24 
run:		
	@	echo Running...
		LD_LIBRARY_PATH=$(CURDIR)/$(DLIB_DIR) ./$(OUTPUT_FILENAME)
