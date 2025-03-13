# Makefile para proyecto con estructura de directorios
# Estructura:
#   - src/     -> Archivos .c (excepto main.c)
#   - include/ -> Archivos .h
#   - obj/     -> Archivos .o generados
#   - EJECUTABLE en carpeta principal

# Configuración
TARGET      = corneta
CC          = gcc
LIBS        = -lm -lpthread
CFLAGS      = -Iinclude -Wall -Wextra

# Directorios
SRC_DIR     = src
INC_DIR     = include
OBJ_DIR     = obj

# Archivos
SOURCES     = $(wildcard $(SRC_DIR)/*.c)
OBJECTS     = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
HEADERS     = $(wildcard $(INC_DIR)/*.h)

# Regla principal
.PHONY: all clean cleanall

all: $(TARGET)  # Ejecutable en la raíz del proyecto

# Enlazado: objetos -> ejecutable en carpeta principal
$(TARGET): $(OBJECTS)
	$(CC) $^ $(LIBS) -o $@

# Compilación: src/*.c -> obj/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Crear directorio obj/ si no existe
$(OBJ_DIR):
	mkdir -p $@

# Limpieza
clean:
	rm -rf $(OBJ_DIR)/*.o

cleanall: clean
	rm -f $(TARGET)  # Borra el ejecutable de la carpeta principal
