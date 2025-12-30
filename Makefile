# Název výsledného programu
TARGET = SSH_Master.exe

# Překladače
CC = gcc
RC = windres

# Soubory
SRC = main.c
RC_SRC = resource.rc
RC_OBJ = resource.o

# Parametry kompilátoru
CFLAGS = -Wall -O2

# Hlavní pravidlo
all: clean_target $(TARGET)

# Smaže starý exe. Pomlčka na začátku říká make, aby pokračoval, i když soubor neexistuje.
# 2>nul (nebo 2>/dev/null) potlačí chybová hlášení.
clean_target:
	-del /f /q $(TARGET) 2>nul || rm -f $(TARGET)

$(TARGET): $(RC_OBJ)
	$(CC) $(SRC) $(RC_OBJ) -o $(TARGET) $(CFLAGS)
	@echo ==========================================
	@echo [ OK ] Program $(TARGET) byl uspesne vytvoren.
	@echo ==========================================

$(RC_OBJ): $(RC_SRC)
	$(RC) $(RC_SRC) -o $(RC_OBJ)

# Kompletní vyčištění
clean:
	-del /f /q $(TARGET) $(RC_OBJ) 2>nul || rm -f $(TARGET) $(RC_OBJ)
	@echo [ CLEAN ] Hotovo.