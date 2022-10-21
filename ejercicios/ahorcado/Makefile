# Makefile
# En este archivo definimos las tareas de compilación y limpieza del proyecto de ahorcado.
# Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
# Aplicaciones para comunicaciones en Red | 3CM13
# ESCOM - IPN
# (C) 2022

# Configuración del compilador.
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic

# Configuración del linker.
LDFLAGS=-lpthread # Para usar hilos.

# Binario y objeto del cliente.
CLIENTE=cliente
CLIENTE_OBJS=cliente.o

# Binario y objeto del servidor.
SERVIDOR=servidor
SERVIDOR_OBJS=servidor.o

# Definición de tarea "default" que compilará todo.
all: $(CLIENTE) $(SERVIDOR)

# Generar archivo(s) objeto(s) cliente.
$(CLIENTE): $(CLIENTE_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o$@ $^

# Generar archivo(s) objeto(s) servidor.
$(SERVIDOR): $(SERVIDOR_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o$@ $^

# Comando para limpiar
clean:
	rm -rf $(CLIENTE) $(CLIENTE_OBJS) $(SERVIDOR) $(SERVIDOR_OBJS)

# Definición de PHONY para el comportamiento default en algunas versiones de `make`.
.PHONY: all clean
