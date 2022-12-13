/**
 * @file cola_usuarios.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Implementación de funciones de la cabecera de Cola de Usuarios.
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
// Librerías locales.
#include "cola_usuarios.h" // Librería de la cola de usuarios.
#include "constantes.h"    // Constantes de ejecución.
#include "mensaje.h"       // Para la estructura mensaje.

// Librerías estándar.
#include <stdio.h>      // Operaciones de entrada y salida.
#include <stdlib.h>     // Funciones estándar de C.
#include <string.h>     // Operaciones con strings.
#include <pthread.h>    // Para operaciones con mutexes.
#include <sys/socket.h> // Para usar send.

// Implementación de funciones.

/**
 * @brief Agrega un usuario a la cola.
 *
 * @param cola Un apuntador al inicio de la cola de usuarios.
 * @param usuario_nuevo El usuario a añadir a la cola.
 * @param desc_socket El descriptor del socket para el usuario en cuestión.
 * @param mutex Un apuntador al mutex que usamos para no hacer operaciones en la cola al mismo tiempo.
 *
 * @return 1 si la operación exitosa, -1 en otro caso.
 */
int agregar_en_cola(cola_usuarios **cola, usuario usuario_nuevo, int desc_socket, pthread_mutex_t **mutex)
{
  // Variables locales.
  cola_usuarios *temporal, // Un apuntador temporal para las iteraciones.
      *nuevo;              // Un apuntador para el nuevo elemento de la cola.

  // Asignamos la memoria para el nuevo.
  nuevo = (cola_usuarios *)malloc(sizeof(cola_usuarios));

  // Validamos que se haya podido crear la memoria.
  if (nuevo == NULL)
  {
    // Regresamos un error.
    return EJECUCION_ERROR;
  }

  // Colocamos la información del nuevo elemento.
  nuevo->data_usuario = usuario_nuevo;
  nuevo->socket = desc_socket;
  nuevo->siguiente = NULL;

  // Bloqueamos el mutex.
  pthread_mutex_lock(*mutex);

  // Verificamos si la cola está vacía.
  if ((*cola) == NULL)
  {
    // Aisgnamos el nuevo elemento al inicio.
    (*cola) = nuevo;
  }
  else
  {
    // Asignamos el nodo inicial a nuestra variable temporal.
    temporal = (*cola);

    // Ciclo para encontrar el último elemento de la cola.
    while (temporal->siguiente != NULL)
    {
      temporal = temporal->siguiente;
    }

    // Agregamos el elemento nuevo a la cola.
    temporal->siguiente = nuevo;
  }

  // Desbloqueamos el mutex.
  pthread_mutex_unlock(*mutex);

  // Regresamos un código de correcto.
  return EJECUCION_EXITOSA;
}

/**
 * @brief Elimina un usuario a la cola.
 *
 * @param cola Un apuntador al inicio de la cola de usuarios.
 * @param socket_usuario El socket del usuario a eliminar (ya que es único por usuario, espero!)
 * @param mutex Un apuntador al mutex que usamos para no hacer operaciones en la cola al mismo tiempo.
 *
 * @return 1 si la operación exitosa, -1 en otro caso.
 */
int eliminar_de_cola(cola_usuarios **cola, int socket_usuario, pthread_mutex_t **mutex)
{
  // Variables locales.
  cola_usuarios *iterador, // Variable para iterar por la cola.
      *previo;             // Variable para almacenar la referencia al elemento inmediato anterior.

  // Bloqueamos el mutex.
  pthread_mutex_lock(*mutex);

  // Verificamos si la cola está vacía.
  if ((*cola) == NULL)
  {
    // Desbloqueamos el mutex.
    pthread_mutex_unlock(*mutex);

    // Como la cola está vacía, regresamos un error.
    return EJECUCION_ERROR;
  }

  // Iniciamos el iterador en el principio de la cola.
  iterador = (*cola);

  // Verificamos si es el primer elemento de la lista.
  if (iterador->socket == socket_usuario)
  {
    // Ajustamos el apuntador del inicio de cola a nuestro iterador.
    (*cola) = iterador->siguiente;

    // Liberamos la memoria.
    free(iterador);

    // Desbloqueamos el mutex.
    pthread_mutex_unlock(*mutex);

    // Finalizamos ejecución.
    return EJECUCION_EXITOSA;
  }

  // Si es un elemento en medio, usamos dos apuntadores.
  previo = iterador;
  iterador = iterador->siguiente;

  // Iteramos en la cola hasta encontrar el elemento que buscamos.
  while (iterador != NULL)
  {
    // Verificamos si es el elmento que buscamos.
    if (iterador->socket == socket_usuario)
    {
      // Cambiamos la referencia del anterior.
      previo->siguiente = iterador->siguiente;

      // Liberamos la memoria del nodo actual.
      free(iterador);

      // Desbloqueamos el mutex.
      pthread_mutex_unlock(*mutex);

      // Regresamos un código de ejecución correcta.
      return EJECUCION_EXITOSA;
    }

    // Movemos los apuntadores.
    previo = iterador;
    iterador = previo->siguiente;
  }

  // Desbloqueamos el mutex.
  pthread_mutex_unlock(*mutex);

  // No se encontró el elemento.
  return EJECUCION_ERROR;
}

/**
 * @bfief Función que imprime la cola de usuarios.
 *
 * @param nodo_inicial El nodo inicial de la cola de usuarios.
 */
void imprimir_cola(cola_usuarios *nodo)
{
  // Iteramos por la cola.
  printf("[");
  while (nodo != NULL)
  {
    printf("(%s,%i)%s", nodo->data_usuario.nombre, nodo->socket, nodo->siguiente == NULL ? "" : ",");
    nodo = nodo->siguiente;
  }
  printf("]\n");
}

/**
 * @brief Función que transmite un mensaje a todos los usuarios presentes en la cola
 *
 * @param nodo El nodo inicial de la lista ligada con los clientes conectados.
 * @param msj_a_transmitir El mensaje a ser transmitido.
 * @param mutex El mutex de la cola de usuarios para prevenir que se añadan/eliminen mientras se realiza el proceso.
 */
void transmitir_mensaje_a_cola_de_usuarios(cola_usuarios *nodo, mensaje msj_a_transmitir, pthread_mutex_t **mutex)
{
  // Variables locales.
  int resultado; // Variable para el resultado de cada operación.

  // Bloqueamos el mutex.
  pthread_mutex_lock(*mutex);

  // Iteramos por todos los clientes.
  while (nodo != NULL)
  {
    // Enviamos el mensaje a cada uno.
    resultado = send(
        nodo->socket,                    // El descriptor del socket por el cual enviaremos el mensjae.
        (const char *)&msj_a_transmitir, // El mensaje a enviar.
        sizeof(mensaje),                 // El tamaño del mensaje a enviar.
        0                                // Sin banderas.
    );

    // Validamos que se haya enviado.
    if (resultado <= 0)
    {
      // Mostramos mensaje de error.
      printf("Error: No se ha podido retransmitir un mensaje para: %s\n", nodo->data_usuario.nombre);
    }

    // Aumentamos el iterador.
    nodo = nodo->siguiente;
  }

  // Desbloqueamos el mutex
  pthread_mutex_unlock(*mutex);
}
