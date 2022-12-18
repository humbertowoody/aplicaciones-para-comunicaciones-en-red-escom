# Makefile
# En este archivo definimos las tareas de compilación y limpieza para la práctica 4
# Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
# Aplicaciones para comunicaciones en Red | 3CM13
# ESCOM - IPN
# (C) 2022

# Configuración del compilador.
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic

# Configuración del linker.
LDFLAGS=-lpthread # Para usar hilos.

# Binario y objeto del servidor.
SERVIDOR=servidor
SERVIDOR_OBJS=servidor.o http.o

# Definición de tarea "default" que compilará todo.
all:  $(SERVIDOR)

# Generar archivo(s) objeto(s) servidor.
$(SERVIDOR): $(SERVIDOR_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o$@ $^

# Comando para limpiar
clean:
	rm -rf $(SERVIDOR) $(SERVIDOR_OBJS)

# Definición de PHONY para el comportamiento default en algunas versiones de `make`.
.PHONY: all clean
