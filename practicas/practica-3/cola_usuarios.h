/**
 * @file cola_usuarios.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Cabecera con la definición del tipo "cola_usuarios" y funciones relacionadas.
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
#ifndef COLA_USUARIOS_INCLUIDO
#define COLA_USUARIOS_INCLUIDO

// Librerías locales.
#include "usuario.h" // Para la estructura del usuario.
#include "mensaje.h" // Para la estructura del mensaje.

// Librerías de sistema
#include <pthread.h> // Para la estructura del mutex.

// Estructura para representar una cola de usuarios.
typedef struct st_cola_usuarios
{
  usuario data_usuario;               // El usuario en la cola.
  int socket;                         // El descriptor del socket para el usuario en la cola.
  struct st_cola_usuarios *siguiente; // Un apuntador al siguiente elemento en la cola.
} cola_usuarios;

// Prototipos de funciones.
int agregar_en_cola(cola_usuarios **, usuario, int, pthread_mutex_t **);                  // Agrega un nuevo elemento en la cola.
int eliminar_de_cola(cola_usuarios **, int, pthread_mutex_t **);                          // Elimina un elemento de la cola en función del descriptor del socket.
void imprimir_cola(cola_usuarios *);                                                      // Imprime la cola.
void transmitir_mensaje_a_cola_de_usuarios(cola_usuarios *, mensaje, pthread_mutex_t **); // Función que transmite un mensaje a todos los clientes en la cola.
cola_usuarios *encontrar_usuario(cola_usuarios *, usuario);                               // Función para encontrar un usuario en la cola.

#endif // COLA_USUARIOS_INCLUIDO
