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

#LIB_PATH := $(LD_LIBRARY_PATH)


# файл main
MAIN := curses.c
TEXT := text.txt
OUTPUT_FILENAME := curs
CC := gcc

################################################################################
#Сценарии

.PHONY: lib_build clean all run

# Сборка исполняемого файла
all:
	@	echo - Compiling start!   
	@	$(CC) $(MAIN) -o $(OUTPUT_FILENAME) -lncurses
	@	echo - Сompiling сomplete!

# Очистка
clean:
	@	echo Cleaning...
	@	rm -rf $(TEXT) 
	@	rm -rf $(OUTPUT_FILENAME) 
	@	echo Cleaning Complete...

clean_txt:
	@	echo Cleaning txt...
	@	rm -rf $(TEXT)
	@	echo Cleaning txt Complete...

run:		
	@	./$(OUTPUT_FILENAME)
